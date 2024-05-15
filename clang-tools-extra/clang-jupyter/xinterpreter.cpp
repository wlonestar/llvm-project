#include "xinterpreter.h"
#include "xbuffer.h"
#include "xparser.h"

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/DiagnosticFrontend.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Interpreter/CodeCompletion.h"
#include "clang/Interpreter/Interpreter.h"

#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <xeus/xeus_context.hpp>
#include <xeus/xrequest_context.hpp>

// Disable LSan for this test.
// FIXME: Re-enable once we can assume GCC 13.2 or higher.
// https://llvm.org/github.com/llvm/llvm-project/issues/67586.
#if LLVM_ADDRESS_SANITIZER_BUILD || LLVM_HWADDRESS_SANITIZER_BUILD
#include <sanitizer/lsan_interface.h>
LLVM_ATTRIBUTE_USED int __lsan_is_turned_off() { return 1; }
#endif

namespace clang {

static llvm::cl::opt<bool> CudaEnabled("cuda", llvm::cl::Hidden);
static llvm::cl::opt<std::string> CudaPath("cuda-path", llvm::cl::Hidden);
static llvm::cl::opt<std::string> OffloadArch("offload-arch", llvm::cl::Hidden);

static llvm::cl::list<std::string>
    ClangArgs("Xcc",
              llvm::cl::desc("Argument to pass to the CompilerInvocation"),
              llvm::cl::CommaSeparated);
static llvm::cl::opt<bool> OptHostSupportsJit("host-supports-jit",
                                              llvm::cl::Hidden);
static llvm::cl::list<std::string> OptInputs(llvm::cl::Positional,
                                             llvm::cl::desc("[code to run]"));

static void llvmErrorHandler(void *UserData, const char *Message,
                             bool GenCrashDiag) {
  auto &Diags = *static_cast<clang::DiagnosticsEngine *>(UserData);

  Diags.Report(clang::diag::err_fe_error_backend) << Message;

  // Run the interrupt handlers to make sure any special cleanups get done, in
  // particular that we remove files registered with RemoveFileOnSignal.
  llvm::sys::RunInterruptHandlers();

  // We cannot recover from llvm errors.  When reporting a fatal error, exit
  // with status 70 to generate crash diagnostics.  For BSD systems this is
  // defined as an internal software error. Otherwise, exit with status 1.

  exit(GenCrashDiag ? 70 : 1);
}

XInterpreter::XInterpreter(int Argc, const char *const *Argv)
    : CoutBuffer(std::bind(&XInterpreter::publishStdout, this,
                           std::placeholders::_1, std::placeholders::_2)),
      CerrBuffer(std::bind(&XInterpreter::publishStderr, this,
                           std::placeholders::_1, std::placeholders::_2)) {
  createInterpr(Argc, Argv);
  redirectOutput();
}

XInterpreter::~XInterpreter() { restoreOutput(); }

void XInterpreter::execute_request_impl(xeus::xrequest_context RequestContext,
                                        send_reply_callback Cb,
                                        int ExecutionCounter,
                                        const std::string &Code,
                                        xeus::execute_request_config Config,
                                        nl::json UserExpressions) {
  /// Set the correct `xrequest_context` for `XOutputBuffer` callback
  XRequestContext = RequestContext;

  nl::json KernelRes;

  // Split Code line by line
  auto Lines = extractInput(Code);

  auto ErrorLevel = 0;
  std::string Ename;
  std::string Evalue;
  Value Output;

  // For standard out and err, save current buffer
  CoutStrBuf = std::cout.rdbuf();
  CerrStrBuf = std::cerr.rdbuf();

  if (Config.silent) {
    auto Null = XNull();
    std::cout.rdbuf(&Null);
    std::cerr.rdbuf(&Null);
  }

  for (auto &Line : Lines) {
    // load dynamic library
    if (Line.rfind("%lib ", 0) == 0) {
      if (auto Err = Interpreter->LoadDynamicLibrary(Line.data() + 5)) {
        llvm::raw_string_ostream OS(Evalue);
        llvm::logAllUnhandledErrors(std::move(Err), OS, "");
        ErrorLevel = 1;
        Ename = "Error";
      }
    } else {
      if (auto Err = Interpreter->ParseAndExecute(Line, &Output)) {
        llvm::raw_string_ostream OS(Evalue);
        llvm::logAllUnhandledErrors(std::move(Err), OS, "");
        ErrorLevel = 1;
        Ename = "Error";
      }
    }
  }

  // Restore original buffer
  std::cout << std::flush;
  std::cerr << std::flush;
  if (Config.silent) {
    std::cout.rdbuf(CoutStrBuf);
    std::cerr.rdbuf(CerrStrBuf);
  }

  if (ErrorLevel == 1) {
    std::vector<std::string> Traceback({Ename + ": " + Evalue});
    publish_execution_error(RequestContext, Ename, Evalue, Traceback);
    KernelRes["status"] = "error";
    KernelRes["ename"] = Ename;
    KernelRes["evalue"] = Evalue;
    KernelRes["traceback"] = Traceback;
  } else {
    if (Output.hasValue()) {
      std::string Val = Output.print();
      nl::json PubData;
      PubData["text/plain"] = Val;
      publish_execution_result(RequestContext, ExecutionCounter,
                               std::move(PubData), nl::json::object());
    }

    KernelRes["status"] = "ok";
    KernelRes["payload"] = nl::json::array();
    KernelRes["user_expressions"] = UserExpressions;
  }
  Cb(KernelRes);
}

nl::json XInterpreter::complete_request_impl(const std::string &Code,
                                             int CursorPos) {
  std::vector<std::string> Results;
  nl::json KernelRes;

  std::string Delims = " \t\n`!@#$^&*()=+[{]}\\|;:\'\",<>?.";
  auto Text = splitLine(Code, Delims, CursorPos);
  std::string ToComplete = Text.back().c_str();

  auto CI = CB.CreateCpp();
  if (auto Err = CI.takeError()) {
    /// TODO: error handling
    return {};
  }

  size_t Lines =
      std::count(Code.begin(), std::next(Code.begin(), CursorPos), '\n') + 1;
  auto Interp = clang::Interpreter::create(std::move(*CI));
  if (auto Err = Interp.takeError()) {
    /// TODO: error handling
    return {};
  }

  auto *MainCI = (*Interp)->getCompilerInstance();
  auto CC = clang::ReplCodeCompleter();
  CC.codeComplete(MainCI, Code, Lines, CursorPos + 1,
                  Interpreter->getCompilerInstance(), Results);

  std::vector<std::string> Comps;
  for (auto C : Results) {
    if (C.find(CC.Prefix) == 0) {
      Comps.push_back(C);
    }
  }

  KernelRes["matches"] = Comps;
  KernelRes["cursor_start"] = CursorPos - ToComplete.length();
  KernelRes["cursor_end"] = CursorPos;
  KernelRes["metadata"] = nl::json::object();
  KernelRes["status"] = "ok";
  return KernelRes;
}

nl::json XInterpreter::inspect_request_impl(const std::string & /*Code*/,
                                            int /*CursorPos*/,
                                            int /* DetailLevel */) {
  nl::json KernelRes;
  return KernelRes;
}

nl::json XInterpreter::is_complete_request_impl(const std::string & /*Code*/) {
  nl::json KernelRes;
  KernelRes["status"] = "complete";
  return KernelRes;
}

nl::json XInterpreter::kernel_info_request_impl() {
  nl::json Result;
  Result["implementation"] = "xeus-clang";
  Result["implementation_version"] = "18.1.1";

  std::string Banner = ""
                       "  __  _____ _   _ ___\n"
                       "  \\ \\/ / _ \\ | | / __|\n"
                       "   >  <  __/ |_| \\__ \\\n"
                       "  /_/\\_\\___|\\__,_|___/\n"
                       "\n"
                       "  xeus-clang: a Jupyter Kernel C++ - based on LLVM\n"
                       "  Clang";
  Banner.append("18.1.1");
  Result["banner"] = Banner;
  Result["language_info"]["name"] = "c++";
  Result["language_info"]["version"] = "18.1.1";
  Result["language_info"]["mimetype"] = "text/x-c++src";
  Result["language_info"]["codemirror_mode"] = "text/x-c++src";
  Result["language_info"]["file_extension"] = ".cpp";
  Result["status"] = "ok";
  return Result;
}

void XInterpreter::shutdown_request_impl() { restoreOutput(); }

void XInterpreter::redirectOutput() {
  CoutStrBuf = std::cout.rdbuf();
  CerrStrBuf = std::cerr.rdbuf();

  std::cout.rdbuf(&CoutBuffer);
  std::cerr.rdbuf(&CerrBuffer);
}

void XInterpreter::restoreOutput() {
  std::cout.rdbuf(CoutStrBuf);
  std::cerr.rdbuf(CerrStrBuf);
}

void XInterpreter::createInterpr(int Argc, const char *const *Argv) {
  llvm::ExitOnError ExitOnErr;
  ExitOnErr.setBanner("clang-repl: ");
  llvm::cl::ParseCommandLineOptions(Argc, Argv);

  llvm::llvm_shutdown_obj Y; // Call llvm_shutdown() on exit.

  std::vector<const char *> ClangArgv(ClangArgs.size());
  std::transform(ClangArgs.begin(), ClangArgs.end(), ClangArgv.begin(),
                 [](const std::string &S) -> const char * { return S.data(); });
  // Initialize all targets (required for device offloading)
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();

  // For static linkage to LLVM
  llvm::InitializeNativeTargetAsmParser();

  if (OptHostSupportsJit) {
    auto J = llvm::orc::LLJITBuilder().create();
    if (J) {
      llvm::outs() << "true\n";
    } else {
      llvm::consumeError(J.takeError());
      llvm::outs() << "false\n";
    }
  }

  // clang::IncrementalCompilerBuilder CB;
  CB.SetCompilerArgs(ClangArgv);

  std::unique_ptr<clang::CompilerInstance> DeviceCI;
  if (CudaEnabled) {
    if (!CudaPath.empty()) {
      CB.SetCudaSDK(CudaPath);
    }

    if (OffloadArch.empty()) {
      OffloadArch = "sm_35";
    }
    CB.SetOffloadArch(OffloadArch);

    DeviceCI = ExitOnErr(CB.CreateCudaDevice());
  }

  // FIXME: Investigate if we could use runToolOnCodeWithArgs from tooling. It
  // can replace the boilerplate code for creation of the compiler instance.
  std::unique_ptr<clang::CompilerInstance> CI;
  if (CudaEnabled) {
    CI = ExitOnErr(CB.CreateCudaHost());
  } else {
    CI = ExitOnErr(CB.CreateCpp());
  }

  // Set an error handler, so that any LLVM backend diagnostics go through our
  // error handler.
  llvm::install_fatal_error_handler(llvmErrorHandler,
                                    static_cast<void *>(&CI->getDiagnostics()));

  // Load any requested plugins.
  CI->LoadRequestedPlugins();
  if (CudaEnabled) {
    DeviceCI->LoadRequestedPlugins();
  }

  if (CudaEnabled) {
    Interpreter = ExitOnErr(
        clang::Interpreter::createWithCUDA(std::move(CI), std::move(DeviceCI)));

    if (CudaPath.empty()) {
      ExitOnErr(Interpreter->LoadDynamicLibrary("libcudart.so"));
    } else {
      auto CudaRuntimeLibPath = CudaPath + "/lib/libcudart.so";
      ExitOnErr(Interpreter->LoadDynamicLibrary(CudaRuntimeLibPath.c_str()));
    }
  } else {
    Interpreter = ExitOnErr(clang::Interpreter::create(std::move(CI)));
  }
}

void XInterpreter::publishStdout(xeus::xrequest_context RequestContext,
                                 const std::string &S) {
  publish_stream(RequestContext, "stdout", S);
}

void XInterpreter::publishStderr(xeus::xrequest_context RequestContext,
                                 const std::string &S) {
  publish_stream(RequestContext, "stderr", S);
}

} // namespace clang
