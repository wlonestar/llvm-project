#ifndef LLVM_CLANG_TOOLS_EXTRA_XPARSER_H
#define LLVM_CLANG_TOOLS_EXTRA_XPARSER_H

#include <cstdarg>
#include <string>
#include <vector>

namespace clang {

std::vector<std::string> splitLine(const std::string &Input,
                                   const std::string &Delims,
                                   std::size_t CursorPos);

std::vector<std::string> getLines(const std::string &Input);

std::vector<std::string> extractInput(const std::string &Input);

} // namespace clang

#endif
