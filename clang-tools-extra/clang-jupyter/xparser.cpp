#include "xparser.h"

#include <cstdarg>
#include <cstdio>
#include <iterator>
#include <regex>
#include <sstream>

namespace clang {

std::vector<std::string> splitLine(const std::string &Input,
                                   const std::string &Delims,
                                   std::size_t CursorPos) {
  std::vector<std::string> Result;
  std::stringstream Ss;
  Ss << "[";
  for (auto C : Delims) {
    Ss << "\\" << C;
  }
  Ss << "]";

  std::regex Re(Ss.str());
  std::copy(std::sregex_token_iterator(Input.begin(),
                                       Input.begin() + CursorPos + 1, Re, -1),
            std::sregex_token_iterator(), std::back_inserter(Result));

  return Result;
}

std::vector<std::string> getLines(const std::string &Input) {
  std::vector<std::string> Lines;
  std::regex Re("\\n");
  std::copy(std::sregex_token_iterator(Input.begin(), Input.end(), Re, -1),
            std::sregex_token_iterator(), std::back_inserter(Lines));
  return Lines;
}

/// split and merge input lines by leading %lib
/// assume expression input only one pre cell
std::vector<std::string> extractInput(const std::string &Input) {
  std::vector<std::string> Blocks = getLines(Input);
  if (Blocks.empty()) {
    return {};
  }
  std::vector<std::string> Results;
  bool Last = false;
  for (auto &Block : Blocks) {
    if (Block.rfind("%lib ", 0) == 0) {
      Results.push_back(Block);
      Last = true;
    } else {
      if (Last) {
        Results.push_back(Block);
      } else {
        if (Results.empty()) {
          Results.push_back(Block);
        } else {
          Results[Results.size() - 1].append("\n" + Block);
        }
      }
      Last = false;
    }
  }
  return Results;
}

} // namespace clang
