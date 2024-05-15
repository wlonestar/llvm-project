#include "xinterpreter.h"

#include <xeus-zmq/xserver_zmq.hpp>
#include <xeus-zmq/xzmq_context.hpp>
#include <xeus/xeus_context.hpp>
#include <xeus/xinterpreter.hpp>
#include <xeus/xkernel.hpp>
#include <xeus/xkernel_configuration.hpp>

#include <iostream>
#include <memory>
#include <signal.h>
#include <string>

void stopHandler(int /*sig*/) { exit(0); }

std::string extractFilename(int *Argc, char *Argv[]) {
  std::string Res = "";
  for (int I = 0; I < *Argc; ++I) {
    if ((std::string(Argv[I]) == "-f") && (I + 1 < *Argc)) {
      Res = Argv[I + 1];
      for (int J = I; J < *Argc - 2; ++J) {
        Argv[J] = Argv[J + 2];
      }
      *Argc -= 2;
      break;
    }
  }
  return Res;
}

using interpr_ptr = std::unique_ptr<clang::XInterpreter>;

interpr_ptr buildInterpreter(int Argc, char **Argv) {
  interpr_ptr InterpPtr = interpr_ptr(new clang::XInterpreter(Argc, Argv));
  return InterpPtr;
}

int main(int argc, char *argv[]) {
  // If we are called from the Jupyter launcher, silence all logging. This
  // is important for a JupyterHub configured with cleanup_servers = False:
  // Upon restart, spawned single-user servers keep running but without the
  // std* streams. When a user then tries to start a new kernel, xeus-cling
  // will get a SIGPIPE when writing to any of these and exit.
  if (std::getenv("JPY_PARENT_PID") != NULL) {
    std::clog.setstate(std::ios_base::failbit);
  }

  // Registering SIGSEGV handler
  signal(SIGINT, stopHandler);

  std::string FileName = extractFilename(&argc, argv);

  std::cout << "args:\n";
  for (int I = 0; I < argc; I++) {
    std::cout << argv[I] << "\n";
  }

  interpr_ptr Interpreter = buildInterpreter(argc, argv);
  auto Context = xeus::make_zmq_context();

  if (!FileName.empty()) {
    xeus::xconfiguration Config = xeus::load_configuration(FileName);

    xeus::xkernel Kernel(
        Config, xeus::get_user_name(), std::move(Context),
        std::move(Interpreter), xeus::make_xserver_default,
        xeus::make_in_memory_history_manager(),
        xeus::make_console_logger(
            xeus::xlogger::msg_type,
            xeus::make_file_logger(xeus::xlogger::content, "xeus.log")));

    std::clog << "Starting xeus-clang kernel...\n\n"
                 "If you want to connect to this kernel from an other client, "
                 "you can use"
                 " the " +
                     FileName + " file."
              << std::endl;

    Kernel.start();
  } else {
    xeus::xkernel Kernel(
        xeus::get_user_name(), std::move(Context), std::move(Interpreter),
        xeus::make_xserver_default, xeus::make_in_memory_history_manager(),
        xeus::make_console_logger(
            xeus::xlogger::msg_type,
            xeus::make_file_logger(xeus::xlogger::content, "xeus.log")));

    const auto &Config = Kernel.get_config();
    std::clog << "Starting xeus-clang kernel...\n\n"
                 "If you want to connect to this kernel from an other client, "
                 "just copy"
                 " and paste the following content inside of a `kernel.json` "
                 "file. And then run for example:\n\n"
                 "# jupyter console --existing kernel.json\n\n"
                 "kernel.json\n```\n{\n"
                 "    \"transport\": \"" +
                     Config.m_transport +
                     "\",\n"
                     "    \"ip\": \"" +
                     Config.m_ip +
                     "\",\n"
                     "    \"control_port\": " +
                     Config.m_control_port +
                     ",\n"
                     "    \"shell_port\": " +
                     Config.m_shell_port +
                     ",\n"
                     "    \"stdin_port\": " +
                     Config.m_stdin_port +
                     ",\n"
                     "    \"iopub_port\": " +
                     Config.m_iopub_port +
                     ",\n"
                     "    \"hb_port\": " +
                     Config.m_hb_port +
                     ",\n"
                     "    \"signature_scheme\": \"" +
                     Config.m_signature_scheme +
                     "\",\n"
                     "    \"key\": \"" +
                     Config.m_key +
                     "\"\n"
                     "}\n```\n";

    Kernel.start();
  }

  return 0;
}
