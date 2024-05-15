# notice name and path
set(EXECUTABLE_NAME clang_kernel)

include(GNUInstallDirs)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/cpp11/kernel.json.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/cpp11/kernel.json"
)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/cpp14/kernel.json.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/cpp14/kernel.json"
)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/cpp17/kernel.json.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/cpp17/kernel.json"
)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/cpp20/kernel.json.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/cpp20/kernel.json"
)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/cppomp/kernel.json.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/cppomp/kernel.json"
)

option(XEUS_STATIC_DEPENDENCIES "link statically with xeus dependencies" OFF)
if (XEUS_STATIC_DEPENDENCIES)
  set(xeus-zmq_target "xeus-zmq-static")
else ()
  set(xeus-zmq_target "xeus-zmq")
endif ()

set(xeus-zmq_REQUIRED_VERSION 2.0.0)
find_package(xeus-zmq ${xeus-zmq_REQUIRED_VERSION} REQUIRED)
find_package(Threads)

include(CheckCXXCompilerFlag)

set(LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Core
  Option
  OrcJIT
  Support
)

add_clang_tool(${EXECUTABLE_NAME}
  main.cpp
  xinterpreter.cpp
  xparser.cpp
)
clang_target_link_libraries(${EXECUTABLE_NAME} PRIVATE
  clangAST
  clangBasic
  clangFrontend
  clangInterpreter
  ${xeus-zmq_target} 
  Threads::Threads
)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
  INSTALL_RPATH_USE_LINK_PATH TRUE
)

if(CLANG_PLUGIN_SUPPORT)
  export_executable_symbols_for_plugins(${EXECUTABLE_NAME})
endif()

string(TOUPPER "${CMAKE_SYSTEM_PROCESSOR}" system_processor)
if(system_processor MATCHES "ARM")
  set(FLAG_LONG_PLT "-Wl,--long-plt")
  llvm_check_linker_flag(CXX ${FLAG_LONG_PLT} LINKER_HAS_FLAG_LONG_PLT)
  # Linkers without this flag are assumed to have long PLTs by default
  if(LINKER_HAS_FLAG_LONG_PLT)
    target_link_options(${EXECUTABLE_NAME} PRIVATE ${FLAG_LONG_PLT})
  endif()
endif()

install(TARGETS ${EXECUTABLE_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set(XJUPYTER_DATA_DIR "share/jupyter" CACHE STRING "Jupyter data directory")

set(MY_KERNELSPEC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels)
install(DIRECTORY ${MY_KERNELSPEC_DIR}
        DESTINATION ${XJUPYTER_DATA_DIR}
        PATTERN "*.in" EXCLUDE)
