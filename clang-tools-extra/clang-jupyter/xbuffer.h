#ifndef LLVM_CLANG_TOOLS_EXTRA_XBUFFER_H
#define LLVM_CLANG_TOOLS_EXTRA_XBUFFER_H

#include <functional>
#include <ios>
#include <mutex>
#include <streambuf>
#include <string>
#include <xeus/xrequest_context.hpp>
#include <xeus/xserver.hpp>

namespace clang {

// default constructor
static xeus::xrequest_context XRequestContext(nl::json::object(),
                                              xeus::channel::SHELL,
                                              std::vector<std::string>{});

class XOutputBuffer : public std::streambuf {
public:
  using base_type = std::streambuf;
  using callback_type = std::function<void(
      xeus::xrequest_context RequestContext, const std::string &)>;
  using traits_type = base_type::traits_type;

  XOutputBuffer(callback_type Callbk) : Callback(std::move(Callbk)) {}

protected:
  traits_type::int_type overflow(traits_type::int_type C) override {
    std::lock_guard<std::mutex> Lock(Mutex);
    if (!traits_type::eq_int_type(C, traits_type::eof())) {
      Output.push_back(traits_type::to_char_type(C));
    }
    return C;
  }

  std::streamsize xsputn(const char *S, std::streamsize Count) override {
    std::lock_guard<std::mutex> Lock(Mutex);
    Output.append(S, Count);
    return Count;
  }

  traits_type::int_type sync() override {
    std::lock_guard<std::mutex> Lock(Mutex);
    if (!Output.empty()) {
      Callback(XRequestContext, Output);
      Output.clear();
    }
    return 0;
  }

  callback_type Callback;
  std::string Output;
  std::mutex Mutex;
};

class XInputBuffer : public std::streambuf {
public:
  using base_type = std::streambuf;
  using callback_type = std::function<void(const std::string &)>;
  using traits_type = base_type::traits_type;

  XInputBuffer(callback_type Callbk) : Callback(std::move(Callbk)), Value() {
    char *Data = const_cast<char *>(Value.data());
    this->setg(Data, Data, Data);
  }

protected:
  traits_type::int_type underflow() override {
    Callback(Value);
    Value += '\n';
    char *Data = const_cast<char *>(Value.data());
    setg(Data, Data, Data + Value.size());
    return traits_type::to_int_type(*gptr());
  }

  callback_type Callback;
  std::string Value;
};

class XNull : public std::streambuf {
  using base_type = std::streambuf;
  using traits_type = base_type::traits_type;

  traits_type::int_type overflow(traits_type::int_type C) override { return C; }
};

} // namespace clang

#endif
