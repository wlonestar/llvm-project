#ifndef LLVM_CLANG_TOOLS_EXTRA_XINTERPRETER_H
#define LLVM_CLANG_TOOLS_EXTRA_XINTERPRETER_H

#include "xbuffer.h"

#include "clang/Interpreter/Interpreter.h"

#include <memory>
#include <streambuf>
#include <xeus/xinterpreter.hpp>

namespace clang {

class XInterpreter : public xeus::xinterpreter {
public:
  XInterpreter(int Argc, const char *const *Argv);
  virtual ~XInterpreter();

  void publishStdout(xeus::xrequest_context RequestContext,
                     const std::string &S);
  void publishStderr(xeus::xrequest_context RequestContext,
                     const std::string &S);

private:
  void configure_impl() override {}

  void execute_request_impl(xeus::xrequest_context RequestContext,
                            send_reply_callback Cb, int ExecutionCounter,
                            const std::string &Code,
                            xeus::execute_request_config Config,
                            nl::json UserExpressions) override;

  nl::json complete_request_impl(const std::string &Code,
                                 int CursorPos) override;

  nl::json inspect_request_impl(const std::string &Code, int CursorPos,
                                int DetailLevel) override;

  nl::json is_complete_request_impl(const std::string &Code) override;

  nl::json kernel_info_request_impl() override;

  void shutdown_request_impl() override;

  void redirectOutput();
  void restoreOutput();

  void createInterpr(int Argc, const char *const *Argv);

  clang::IncrementalCompilerBuilder CB;
  std::unique_ptr<Interpreter> Interpreter;

  std::streambuf *CoutStrBuf;
  std::streambuf *CerrStrBuf;

  XOutputBuffer CoutBuffer;
  XOutputBuffer CerrBuffer;
};

} // namespace clang

#endif
