//===--- ValuePrinter.cpp - Value Printer -----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements some value printer functions for clang-repl.
///
//===----------------------------------------------------------------------===//

#include "ValuePrinter.h"
#include "InterpreterUtils.h"

#include "clang/AST/Type.h"

#include <map>
#include <sstream>
#include <unordered_set>

namespace clang {

std::string printAddress(const void *ptr, const char prefix) {
  if (!ptr) {
    return "nullptr";
  }
  std::ostringstream ostr;
  if (prefix) {
    ostr << prefix;
  }
  ostr << ptr;
  return ostr.str();
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const void *ptr) { return printAddress(ptr, '@'); }

REPL_EXTERNAL_VISIBILITY
std::string printValue(const void **ptr) { return printAddress(*ptr); }

REPL_EXTERNAL_VISIBILITY
std::string printValue(const bool *ptr) { return *ptr ? "true" : "false"; }

REPL_EXTERNAL_VISIBILITY
std::string printValue(const char *ptr) {
  std::string value = "'";
  switch (*ptr) {
  case '\t':
    value += "\\t";
    break;
  case '\n':
    value += "\\n";
    break;
  case '\r':
    value += "\\r";
    break;
  case '\f':
    value += "\\f";
    break;
  case '\v':
    value += "\\v";
    break;
  default:
    value += *ptr;
  }
  value += "'";
  return value;
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const signed char *ptr) {
  std::string value = "'";
  switch (*ptr) {
  case '\t':
    value += "\\t";
    break;
  case '\n':
    value += "\\n";
    break;
  case '\r':
    value += "\\r";
    break;
  case '\f':
    value += "\\f";
    break;
  case '\v':
    value += "\\v";
    break;
  default:
    value += *ptr;
  }
  value += "'";
  return value;
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const unsigned char *ptr) {
  std::string value = "'";
  switch (*ptr) {
  case '\t':
    value += "\\t";
    break;
  case '\n':
    value += "\\n";
    break;
  case '\r':
    value += "\\r";
    break;
  case '\f':
    value += "\\f";
    break;
  case '\v':
    value += "\\v";
    break;
  default:
    value += *ptr;
  }
  value += "'";
  return value;
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const wchar_t *ptr) {
  std::string value = "'";
  switch (*ptr) {
  case '\t':
    value += "\\t";
    break;
  case '\n':
    value += "\\n";
    break;
  case '\r':
    value += "\\r";
    break;
  case '\f':
    value += "\\f";
    break;
  case '\v':
    value += "\\v";
    break;
  default:
    value += *ptr;
  }
  value += "'";
  return value;
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const char16_t *ptr) {
  std::string value = "'";
  switch (*ptr) {
  case '\t':
    value += "\\t";
    break;
  case '\n':
    value += "\\n";
    break;
  case '\r':
    value += "\\r";
    break;
  case '\f':
    value += "\\f";
    break;
  case '\v':
    value += "\\v";
    break;
  default:
    value += *ptr;
  }
  value += "'";
  return value;
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const char32_t *ptr) {
  std::string value = "'";
  switch (*ptr) {
  case '\t':
    value += "\\t";
    break;
  case '\n':
    value += "\\n";
    break;
  case '\r':
    value += "\\r";
    break;
  case '\f':
    value += "\\f";
    break;
  case '\v':
    value += "\\v";
    break;
  default:
    value += *ptr;
  }
  value += "'";
  return value;
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const unsigned short *ptr) {
  std::ostringstream ostr;
  ostr << *ptr;
  return ostr.str();
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const short *ptr) {
  std::ostringstream ostr;
  ostr << *ptr;
  return ostr.str();
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const unsigned int *ptr) {
  std::ostringstream ostr;
  ostr << *ptr;
  return ostr.str();
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const int *ptr) {
  std::ostringstream ostr;
  ostr << *ptr;
  return ostr.str();
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const unsigned long *ptr) {
  std::ostringstream ostr;
  ostr << *ptr;
  return ostr.str();
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const long *ptr) {
  std::ostringstream ostr;
  ostr << *ptr;
  return ostr.str();
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const unsigned long long *ptr) {
  std::ostringstream ostr;
  ostr << *ptr;
  return ostr.str();
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const long long *ptr) {
  std::ostringstream ostr;
  ostr << *ptr;
  return ostr.str();
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const float *ptr) {
  std::ostringstream ostr;
  ostr << *ptr;
  return ostr.str();
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const double *ptr) {
  std::ostringstream ostr;
  ostr << *ptr;
  return ostr.str();
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const long double *ptr) {
  std::ostringstream ostr;
  ostr << *ptr;
  return ostr.str();
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(char *ptr, bool seq, size_t size) {
  std::string value = "\"";
  char *p = ptr;
  size_t idx = 0;
  if (size != 0) {
    while (idx < size) {
      value += *(p + idx);
      idx++;
    }
  } else {
    while (*p != '\0') {
      value += *p;
      p++;
    }
  }
  value += "\"";
  return value;
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(signed char *ptr, bool seq, size_t size) {
  std::string value = "\"";
  signed char *p = ptr;
  size_t idx = 0;
  if (size != 0) {
    while (idx < size) {
      value += *(p + idx);
      idx++;
    }
  } else {
    while (*p != '\0') {
      value += *p;
      p++;
    }
  }
  value += "\"";
  return value;
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(unsigned char *ptr, bool seq, size_t size) {
  std::string value = "\"";
  unsigned char *p = ptr;
  size_t idx = 0;
  if (size != 0) {
    while (idx < size) {
      value += *(p + idx);
      idx++;
    }
  } else {
    while (*p != '\0') {
      value += *p;
      p++;
    }
  }
  value += "\"";
  return value;
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(wchar_t *ptr, bool seq, size_t size) {
  std::wstring value;
  wchar_t *p = ptr;
  size_t idx = 0;
  if (size != 0) {
    while (idx < size) {
      value += *(p + idx);
      idx++;
    }
  } else {
    while (*p != '\0') {
      value += *p;
      p++;
    }
  }
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  return "\"" + converter.to_bytes(value) + "\"";
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(char16_t *ptr, bool seq, size_t size) {
  std::u16string value;
  char16_t *p = ptr;
  size_t idx = 0;
  if (size != 0) {
    while (idx < size) {
      value += *(p + idx);
      idx++;
    }
  } else {
    while (*p != '\0') {
      value += *p;
      p++;
    }
  }
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
  return "\"" + converter.to_bytes(value) + "\"";
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(char32_t *ptr, bool seq, size_t size) {
  std::u32string value;
  char32_t *p = ptr;
  size_t idx = 0;
  if (size != 0) {
    while (idx < size) {
      value += *(p + idx);
      idx++;
    }
  } else {
    while (*p != '\0') {
      value += *p;
      p++;
    }
  }
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
  return "\"" + converter.to_bytes(value) + "\"";
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const std::string *ptr) { return "\"" + *ptr + "\""; }

REPL_EXTERNAL_VISIBILITY
std::string printValue(const std::wstring *ptr) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  return "\"" + converter.to_bytes(*ptr) + "\"";
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const std::u16string *ptr) {
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
  return "\"" + converter.to_bytes(*ptr) + "\"";
}

REPL_EXTERNAL_VISIBILITY
std::string printValue(const std::u32string *ptr) {
  std::wstring_convert<std::codecvt_utf8_utf16<char32_t>, char32_t> converter;
  return "\"" + converter.to_bytes(*ptr) + "\"";
}

/// If the Value is a builtin type, we need to use `Value::convertTo<>()` to
/// convert the Value to specific builtin type.
/// Only print inner builtin type such as struct fields should we use
/// `printBuiltinTypeByPtr`
std::string printBuiltinTypeByValue(const Value &V, BuiltinType::Kind Kind) {
  switch (Kind) {
  case BuiltinType::Bool:
    return V.convertTo<bool>() ? "true" : "false";
  case BuiltinType::Char_U:
  case BuiltinType::UChar: {
    auto val = V.convertTo<unsigned char>();
    return printValue(&val);
  }
  case BuiltinType::Char_S:
  case BuiltinType::SChar: {
    auto val = V.convertTo<signed char>();
    return printValue(&val);
  }
  case BuiltinType::WChar_S: {
    auto val = V.convertTo<wchar_t>();
    return printValue(&val);
  }
  case BuiltinType::Char16: {
    auto val = V.convertTo<char16_t>();
    return printValue(&val);
  }
  case BuiltinType::Char32: {
    auto val = V.convertTo<char32_t>();
    return printValue(&val);
  }
  case BuiltinType::UShort:
    return std::to_string(V.convertTo<unsigned short>());
  case BuiltinType::Short:
    return std::to_string(V.convertTo<short>());
  case BuiltinType::UInt:
    return std::to_string(V.convertTo<unsigned int>());
  case BuiltinType::Int:
    return std::to_string(V.convertTo<int>());
  case BuiltinType::ULong:
    return std::to_string(V.convertTo<unsigned long>());
  case BuiltinType::Long:
    return std::to_string(V.convertTo<long>());
  case BuiltinType::ULongLong:
    return std::to_string(V.convertTo<unsigned long long>());
  case BuiltinType::LongLong:
    return std::to_string(V.convertTo<long long>());
  case BuiltinType::Float:
    return std::to_string(V.convertTo<float>());
  case BuiltinType::Double:
    return std::to_string(V.convertTo<double>());
  case BuiltinType::LongDouble:
    return std::to_string(V.convertTo<long double>());
  default:
    break;
  }
  return "";
}

std::string printBuiltinTypeByPtr(void *ptr, BuiltinType::Kind Kind) {
  switch (Kind) {
  case BuiltinType::Bool:
    return printValue((bool *)ptr);
  case BuiltinType::Char_U:
  case BuiltinType::UChar:
    return printValue((unsigned char *)ptr);
  case BuiltinType::Char_S:
  case BuiltinType::SChar:
    return printValue((signed char *)ptr);
  case BuiltinType::WChar_S:
    return printValue((wchar_t *)ptr);
  case BuiltinType::Char16:
    return printValue((char16_t *)ptr);
  case BuiltinType::Char32:
    return printValue((char32_t *)ptr);
  case BuiltinType::UShort:
    return printValue((unsigned short *)ptr);
  case BuiltinType::Short:
    return printValue((short *)ptr);
  case BuiltinType::UInt:
    return printValue((unsigned int *)ptr);
  case BuiltinType::Int:
    return printValue((int *)ptr);
  case BuiltinType::ULong:
    return printValue((unsigned long *)ptr);
  case BuiltinType::Long:
    return printValue((long *)ptr);
  case BuiltinType::ULongLong:
    return printValue((unsigned long long *)ptr);
  case BuiltinType::LongLong:
    return printValue((long long *)ptr);
  case BuiltinType::Float:
    return printValue((float *)ptr);
  case BuiltinType::Double:
    return printValue((double *)ptr);
  case BuiltinType::LongDouble:
    return printValue((long double *)ptr);
  default:
    break;
  }
  return "";
}

std::string printEnumByValue(const Value &V, const ASTContext &C,
                             QualType Type) {
  std::ostringstream ostr;
  const EnumType *enumTy = Type->getAs<EnumType>();
  EnumDecl *decl = enumTy->getDecl();
  uint64_t value = V.getULongLong();
  bool first = true;
  llvm::APSInt valAsInt = C.MakeIntValue(value, Type);
  using enum_iter = EnumDecl::enumerator_iterator;
  for (enum_iter I = decl->enumerator_begin(), E = decl->enumerator_end();
       I != E; ++I) {
    if (I->getInitVal() == valAsInt) {
      if (!first) {
        ostr << " ? ";
      }
      ostr << "(" << I->getQualifiedNameAsString() << ")";
      first = false;
    }
  }
  ostr << " : " << decl->getIntegerType().getAsString() << " "
       << llvm::toString(valAsInt, 10);
  return ostr.str();
}

std::string printEnumByPtr(void *ptr, const ASTContext &C, QualType Type) {
  std::ostringstream ostr;
  const EnumType *enumTy = Type->getAs<EnumType>();
  EnumDecl *decl = enumTy->getDecl();
  uint64_t value = *(unsigned long int *)ptr;
  bool first = true;
  llvm::APSInt valAsInt = C.MakeIntValue(value, Type);
  using enum_iter = EnumDecl::enumerator_iterator;
  for (enum_iter I = decl->enumerator_begin(), E = decl->enumerator_end();
       I != E; ++I) {
    if (I->getInitVal() == valAsInt) {
      if (!first) {
        ostr << " ? ";
      }
      ostr << "(" << I->getQualifiedNameAsString() << ")";
      first = false;
    }
  }
  ostr << " : " << decl->getIntegerType().getAsString() << " "
       << llvm::toString(valAsInt, 10);
  return ostr.str();
}

bool isCharType(const Type *type) {
  if (const BuiltinType *bt = llvm::dyn_cast<BuiltinType>(type)) {
    BuiltinType::Kind Kind = bt->getKind();
    if (Kind == BuiltinType::Char_U || Kind == BuiltinType::UChar ||
        Kind == BuiltinType::Char_S || Kind == BuiltinType::SChar ||
        Kind == BuiltinType::WChar_S || Kind == BuiltinType::Char16 ||
        Kind == BuiltinType::Char32) {
      return true;
    }
  }
  return false;
}

std::string printCharArray(void *ptr, QualType Type, size_t size) {
  if (const BuiltinType *bt = llvm::dyn_cast<BuiltinType>(Type.getTypePtr())) {
    BuiltinType::Kind Kind = bt->getKind();
    switch (Kind) {
    case BuiltinType::Char_U:
    case BuiltinType::UChar:
      return printValue((unsigned char *)ptr, true, size);
    case BuiltinType::Char_S:
    case BuiltinType::SChar:
      return printValue((signed char *)ptr, true, size);
    case BuiltinType::WChar_S:
      return printValue((wchar_t *)ptr, true, size);
    case BuiltinType::Char16:
      return printValue((char16_t *)ptr, true, size);
    case BuiltinType::Char32:
      return printValue((char32_t *)ptr, true, size);
    default:
      break;
    }
  }
  return "";
}

std::string printArray(void *ptr, const Value &V, const ASTContext &C,
                       QualType Type) {
  if (const ArrayType *decl = Type->getAsArrayTypeUnsafe()) {
    if (const ConstantArrayType *cdecl =
            dyn_cast<const ConstantArrayType>(decl)) {
      QualType elemType = decl->getElementType();
      uint64_t arraySize = cdecl->getSize().getZExtValue();
      uint64_t elemSize = C.getTypeSize(elemType) /* bits */ / 8;

      /// for typedef \c char[]
      if (isCharType(elemType.getTypePtr())) {
        return printCharArray(ptr, elemType, arraySize);
      }

      std::ostringstream ostr;
      ostr << "{ ";

      /// for typedef \c char*[]
      if (elemType->isPointerType()) {
        if (isCharType(elemType->getPointeeType().getTypePtr())) {
          for (uint64_t i = 0; i < arraySize; i++) {
            void *elemPtr =
                (void *)(((uint64_t *)((char *)ptr + i * elemSize))[0]);
            ostr << printValuePtr(elemPtr, V, C, elemType);
            if (i != arraySize - 1) {
              ostr << ", ";
            }
          }
          ostr << " }";
          return ostr.str();
        }
      }

      /// for other \c T[]
      for (uint64_t i = 0; i < arraySize; i++) {
        void *elemPtr = (void *)((char *)ptr + i * elemSize);
        ostr << printValuePtr(elemPtr, V, C, elemType);
        if (i != arraySize - 1) {
          ostr << ", ";
        }
      }
      ostr << " }";
      return ostr.str();
    }
  }

  if (Type->isPointerType()) {
    QualType elemType = Type->getPointeeType();
    if (elemType->isPointerType()) {
      return "";
    }
    /// for typedef \c char*
    if (isCharType(elemType.getTypePtr())) {
      return printCharArray(ptr, elemType, 0);
    }
  }
  return "";
}

std::string printStdString(void *ptr, const ASTContext &C, QualType Type) {
  std::string typeStr = GetFullTypeName(C, Type);
  std::string val = "";
  if (typeStr == "std::string" || typeStr == "std::basic_string<char>") {
    val = printValue((std::string *)ptr);
  }
  if (typeStr == "std::wstring" || typeStr == "std::basic_string<wchar_t>") {
    val = printValue((std::wstring *)ptr);
  }
  if (typeStr == "std::u16string" || typeStr == "std::basic_string<char16_t>") {
    val = printValue((std::u16string *)ptr);
  }
  if (typeStr == "std::u32string" || typeStr == "std::basic_string<char32_t>") {
    val = printValue((std::u32string *)ptr);
  }
  return val;
}

template <typename T> std::string printBuiltinTypeVector(void *ptr) {
  std::vector<T> *vecPtr = static_cast<std::vector<T> *>(ptr);
  std::ostringstream ostr;
  ostr << "{ ";
  for (auto it = vecPtr->begin(), e = vecPtr->end(); it != e; ++it) {
    ostr << *it;
    auto tmp = it;
    if (++tmp != e) {
      ostr << ", ";
    }
  }
  ostr << " }";
  return ostr.str();
}

template <typename T> std::string printBuiltinTypeDeque(void *ptr) {
  std::deque<T> *deqPtr = static_cast<std::deque<T> *>(ptr);
  std::ostringstream ostr;
  ostr << "{ ";
  for (auto it = deqPtr->begin(), e = deqPtr->end(); it != e; ++it) {
    ostr << *it;
    auto tmp = it;
    if (++tmp != e) {
      ostr << ", ";
    }
  }
  ostr << " }";
  return ostr.str();
}

template <typename T> std::string printBuiltinTypeList(void *ptr) {
  std::list<T> *listPtr = static_cast<std::list<T> *>(ptr);
  std::ostringstream ostr;
  ostr << "{ ";
  for (auto it = listPtr->begin(), e = listPtr->end(); it != e; ++it) {
    ostr << *it;
    auto tmp = it;
    if (++tmp != e) {
      ostr << ", ";
    }
  }
  ostr << " }";
  return ostr.str();
}

template <typename T> std::string printBuiltinTypeSet(void *ptr) {
  std::set<T> *setPtr = static_cast<std::set<T> *>(ptr);
  std::ostringstream ostr;
  ostr << "{ ";
  for (auto it = setPtr->begin(), e = setPtr->end(); it != e; ++it) {
    ostr << *it;
    auto tmp = it;
    if (++tmp != e) {
      ostr << ", ";
    }
  }
  ostr << " }";
  return ostr.str();
}

template <typename T> std::string printBuiltinTypeMultiSet(void *ptr) {
  std::multiset<T> *setPtr = static_cast<std::multiset<T> *>(ptr);
  std::ostringstream ostr;
  ostr << "{ ";
  for (auto it = setPtr->begin(), e = setPtr->end(); it != e; ++it) {
    ostr << *it;
    auto tmp = it;
    if (++tmp != e) {
      ostr << ", ";
    }
  }
  ostr << " }";
  return ostr.str();
}

template <typename T> std::string printBuiltinTypeUnorderedSet(void *ptr) {
  std::unordered_set<T> *setPtr = static_cast<std::unordered_set<T> *>(ptr);
  std::ostringstream ostr;
  ostr << "{ ";
  for (auto it = setPtr->begin(), e = setPtr->end(); it != e; ++it) {
    ostr << *it;
    auto tmp = it;
    if (++tmp != e) {
      ostr << ", ";
    }
  }
  ostr << " }";
  return ostr.str();
}

/// TODO: support more element type for std::vector<>
std::string printStdVector(void *ptr, const Value &V, const ASTContext &C,
                           CXXRecordDecl *RecordDecl, QualType Type) {
  const auto *specDecl = dyn_cast<ClassTemplateSpecializationDecl>(RecordDecl);
  if (!specDecl) {
    return "";
  }

  const TemplateArgumentList &tplArgs = specDecl->getTemplateArgs();
  assert(tplArgs.size() != 0);
  const TemplateArgument &tplArg = tplArgs[0];
  /// for builtin type vector, use `printBuiltinTypeVector`
  if (tplArg.getKind() == TemplateArgument::Type) {
    if (const BuiltinType *BT =
            dyn_cast<BuiltinType>(tplArg.getAsType().getTypePtr())) {
      switch (BT->getKind()) {
      case BuiltinType::Bool:
        return printBuiltinTypeVector<bool>(ptr);
      case BuiltinType::Char_U:
      case BuiltinType::UChar:
        return printBuiltinTypeVector<unsigned char>(ptr);
      case BuiltinType::Char_S:
      case BuiltinType::SChar:
        return printBuiltinTypeVector<signed char>(ptr);
      case BuiltinType::WChar_S:
        return printBuiltinTypeVector<wchar_t>(ptr);
      case BuiltinType::Char16:
        return printBuiltinTypeVector<char16_t>(ptr);
      case BuiltinType::Char32:
        return printBuiltinTypeVector<char32_t>(ptr);
      case BuiltinType::UShort:
        return printBuiltinTypeVector<unsigned short>(ptr);
      case BuiltinType::Short:
        return printBuiltinTypeVector<short>(ptr);
      case BuiltinType::UInt:
        return printBuiltinTypeVector<unsigned int>(ptr);
      case BuiltinType::Int:
        return printBuiltinTypeVector<int>(ptr);
      case BuiltinType::ULong:
        return printBuiltinTypeVector<unsigned long>(ptr);
      case BuiltinType::Long:
        return printBuiltinTypeVector<long>(ptr);
      case BuiltinType::ULongLong:
        return printBuiltinTypeVector<unsigned long long>(ptr);
      case BuiltinType::LongLong:
        return printBuiltinTypeVector<long long>(ptr);
      case BuiltinType::Float:
        return printBuiltinTypeVector<float>(ptr);
      case BuiltinType::Double:
        return printBuiltinTypeVector<double>(ptr);
      case BuiltinType::LongDouble:
        return printBuiltinTypeVector<long double>(ptr);
      default:
        break;
      }
    }
  }
  return "";
}

std::string printStdDeque(void *ptr, CXXRecordDecl *RecordDecl, QualType Type) {
  const auto *specDecl = dyn_cast<ClassTemplateSpecializationDecl>(RecordDecl);
  if (!specDecl) {
    return "";
  }

  const TemplateArgumentList &tplArgs = specDecl->getTemplateArgs();
  assert(tplArgs.size() != 0);
  const TemplateArgument &tplArg = tplArgs[0];
  /// for builtin type deque, use `printBuiltinTypeDeque`
  if (tplArg.getKind() == TemplateArgument::Type) {
    if (const BuiltinType *BT =
            dyn_cast<BuiltinType>(tplArg.getAsType().getTypePtr())) {
      switch (BT->getKind()) {
      case BuiltinType::Bool:
        return printBuiltinTypeDeque<bool>(ptr);
      case BuiltinType::Char_U:
      case BuiltinType::UChar:
        return printBuiltinTypeDeque<unsigned char>(ptr);
      case BuiltinType::Char_S:
      case BuiltinType::SChar:
        return printBuiltinTypeDeque<signed char>(ptr);
      case BuiltinType::WChar_S:
        return printBuiltinTypeDeque<wchar_t>(ptr);
      case BuiltinType::Char16:
        return printBuiltinTypeDeque<char16_t>(ptr);
      case BuiltinType::Char32:
        return printBuiltinTypeDeque<char32_t>(ptr);
      case BuiltinType::UShort:
        return printBuiltinTypeDeque<unsigned short>(ptr);
      case BuiltinType::Short:
        return printBuiltinTypeDeque<short>(ptr);
      case BuiltinType::UInt:
        return printBuiltinTypeDeque<unsigned int>(ptr);
      case BuiltinType::Int:
        return printBuiltinTypeDeque<int>(ptr);
      case BuiltinType::ULong:
        return printBuiltinTypeDeque<unsigned long>(ptr);
      case BuiltinType::Long:
        return printBuiltinTypeDeque<long>(ptr);
      case BuiltinType::ULongLong:
        return printBuiltinTypeDeque<unsigned long long>(ptr);
      case BuiltinType::LongLong:
        return printBuiltinTypeDeque<long long>(ptr);
      case BuiltinType::Float:
        return printBuiltinTypeDeque<float>(ptr);
      case BuiltinType::Double:
        return printBuiltinTypeDeque<double>(ptr);
      case BuiltinType::LongDouble:
        return printBuiltinTypeDeque<long double>(ptr);
      default:
        break;
      }
    }
  }
  return "";
}

std::string printStdArray(void *ptr, const Value &V, const ASTContext &C,
                          CXXRecordDecl *RecordDecl, QualType Type) {
  const auto *specDecl = dyn_cast<ClassTemplateSpecializationDecl>(RecordDecl);
  if (!specDecl) {
    return "";
  }

  const TemplateArgumentList &tplArgs = specDecl->getTemplateArgs();
  assert(tplArgs.size() != 0);
  const TemplateArgument &typeArg = tplArgs[0];
  const TemplateArgument &sizeArg = tplArgs[1];
  if (typeArg.getKind() == TemplateArgument::Type &&
      sizeArg.getKind() == TemplateArgument::Integral) {
    auto arraySize = sizeArg.getAsIntegral().getZExtValue();
    auto elemType = typeArg.getAsType();
    auto elemSize = C.getTypeSize(elemType) /* bits */ / 8;

    std::ostringstream ostr;
    ostr << "{ ";

    if (elemType->isPointerType() &&
        isCharType(elemType->getPointeeType().getTypePtr())) {
      /// support for typedef `char *[]`
      for (uint64_t i = 0; i < arraySize; i++) {
        void *elemPtr = (void *)(((uint64_t *)((char *)ptr + i * elemSize))[0]);
        ostr << printValuePtr(elemPtr, V, C, elemType);
        if (i != arraySize - 1) {
          ostr << ", ";
        }
      }
      ostr << " }";
      return ostr.str();
    }

    for (uint64_t i = 0; i < arraySize; i++) {
      void *elemPtr = (void *)((char *)ptr + i * elemSize);
      ostr << printValuePtr(elemPtr, V, C, elemType);
      if (i != arraySize - 1) {
        ostr << ", ";
      }
    }
    ostr << " }";
    return ostr.str();
  }
  return "";
}

std::string printList(void *ptr, CXXRecordDecl *RecordDecl, QualType Type) {
  const auto *specDecl = dyn_cast<ClassTemplateSpecializationDecl>(RecordDecl);
  if (!specDecl) {
    return "";
  }

  const TemplateArgumentList &tplArgs = specDecl->getTemplateArgs();
  assert(tplArgs.size() != 0);
  const TemplateArgument &tplArg = tplArgs[0];
  /// for builtin type list, use `printBuiltinTypeList`
  if (tplArg.getKind() == TemplateArgument::Type) {
    if (const BuiltinType *BT =
            dyn_cast<BuiltinType>(tplArg.getAsType().getTypePtr())) {
      switch (BT->getKind()) {
      case BuiltinType::Bool:
        return printBuiltinTypeList<bool>(ptr);
      case BuiltinType::Char_U:
      case BuiltinType::UChar:
        return printBuiltinTypeList<unsigned char>(ptr);
      case BuiltinType::Char_S:
      case BuiltinType::SChar:
        return printBuiltinTypeList<signed char>(ptr);
      case BuiltinType::WChar_S:
        return printBuiltinTypeList<wchar_t>(ptr);
      case BuiltinType::Char16:
        return printBuiltinTypeList<char16_t>(ptr);
      case BuiltinType::Char32:
        return printBuiltinTypeList<char32_t>(ptr);
      case BuiltinType::UShort:
        return printBuiltinTypeList<unsigned short>(ptr);
      case BuiltinType::Short:
        return printBuiltinTypeList<short>(ptr);
      case BuiltinType::UInt:
        return printBuiltinTypeList<unsigned int>(ptr);
      case BuiltinType::Int:
        return printBuiltinTypeList<int>(ptr);
      case BuiltinType::ULong:
        return printBuiltinTypeList<unsigned long>(ptr);
      case BuiltinType::Long:
        return printBuiltinTypeList<long>(ptr);
      case BuiltinType::ULongLong:
        return printBuiltinTypeList<unsigned long long>(ptr);
      case BuiltinType::LongLong:
        return printBuiltinTypeList<long long>(ptr);
      case BuiltinType::Float:
        return printBuiltinTypeList<float>(ptr);
      case BuiltinType::Double:
        return printBuiltinTypeList<double>(ptr);
      case BuiltinType::LongDouble:
        return printBuiltinTypeList<long double>(ptr);
      default:
        break;
      }
    }
  }
  return "";
}

std::string printSet(void *ptr, CXXRecordDecl *RecordDecl, QualType Type) {
  const auto *specDecl = dyn_cast<ClassTemplateSpecializationDecl>(RecordDecl);
  if (!specDecl) {
    return "";
  }

  const TemplateArgumentList &tplArgs = specDecl->getTemplateArgs();
  assert(tplArgs.size() != 0);
  const TemplateArgument &tplArg = tplArgs[0];
  /// for builtin type set, use `printBuiltinTypeSet`
  if (tplArg.getKind() == TemplateArgument::Type) {
    if (const BuiltinType *BT =
            dyn_cast<BuiltinType>(tplArg.getAsType().getTypePtr())) {
      switch (BT->getKind()) {
      case BuiltinType::Bool:
        return printBuiltinTypeSet<bool>(ptr);
      case BuiltinType::Char_U:
      case BuiltinType::UChar:
        return printBuiltinTypeSet<unsigned char>(ptr);
      case BuiltinType::Char_S:
      case BuiltinType::SChar:
        return printBuiltinTypeSet<signed char>(ptr);
      case BuiltinType::WChar_S:
        return printBuiltinTypeSet<wchar_t>(ptr);
      case BuiltinType::Char16:
        return printBuiltinTypeSet<char16_t>(ptr);
      case BuiltinType::Char32:
        return printBuiltinTypeSet<char32_t>(ptr);
      case BuiltinType::UShort:
        return printBuiltinTypeSet<unsigned short>(ptr);
      case BuiltinType::Short:
        return printBuiltinTypeSet<short>(ptr);
      case BuiltinType::UInt:
        return printBuiltinTypeSet<unsigned int>(ptr);
      case BuiltinType::Int:
        return printBuiltinTypeSet<int>(ptr);
      case BuiltinType::ULong:
        return printBuiltinTypeSet<unsigned long>(ptr);
      case BuiltinType::Long:
        return printBuiltinTypeSet<long>(ptr);
      case BuiltinType::ULongLong:
        return printBuiltinTypeSet<unsigned long long>(ptr);
      case BuiltinType::LongLong:
        return printBuiltinTypeSet<long long>(ptr);
      case BuiltinType::Float:
        return printBuiltinTypeSet<float>(ptr);
      case BuiltinType::Double:
        return printBuiltinTypeSet<double>(ptr);
      case BuiltinType::LongDouble:
        return printBuiltinTypeSet<long double>(ptr);
      default:
        break;
      }
    }
  }
  return "";
}

std::string printMultiSet(void *ptr, CXXRecordDecl *RecordDecl, QualType Type) {
  const auto *specDecl = dyn_cast<ClassTemplateSpecializationDecl>(RecordDecl);
  if (!specDecl) {
    return "";
  }

  const TemplateArgumentList &tplArgs = specDecl->getTemplateArgs();
  assert(tplArgs.size() != 0);
  const TemplateArgument &tplArg = tplArgs[0];
  /// for builtin type multi set, use `printBuiltinTypeMultiSet`
  if (tplArg.getKind() == TemplateArgument::Type) {
    if (const BuiltinType *BT =
            dyn_cast<BuiltinType>(tplArg.getAsType().getTypePtr())) {
      switch (BT->getKind()) {
      case BuiltinType::Bool:
        return printBuiltinTypeMultiSet<bool>(ptr);
      case BuiltinType::Char_U:
      case BuiltinType::UChar:
        return printBuiltinTypeMultiSet<unsigned char>(ptr);
      case BuiltinType::Char_S:
      case BuiltinType::SChar:
        return printBuiltinTypeMultiSet<signed char>(ptr);
      case BuiltinType::WChar_S:
        return printBuiltinTypeMultiSet<wchar_t>(ptr);
      case BuiltinType::Char16:
        return printBuiltinTypeMultiSet<char16_t>(ptr);
      case BuiltinType::Char32:
        return printBuiltinTypeMultiSet<char32_t>(ptr);
      case BuiltinType::UShort:
        return printBuiltinTypeMultiSet<unsigned short>(ptr);
      case BuiltinType::Short:
        return printBuiltinTypeMultiSet<short>(ptr);
      case BuiltinType::UInt:
        return printBuiltinTypeMultiSet<unsigned int>(ptr);
      case BuiltinType::Int:
        return printBuiltinTypeMultiSet<int>(ptr);
      case BuiltinType::ULong:
        return printBuiltinTypeMultiSet<unsigned long>(ptr);
      case BuiltinType::Long:
        return printBuiltinTypeMultiSet<long>(ptr);
      case BuiltinType::ULongLong:
        return printBuiltinTypeMultiSet<unsigned long long>(ptr);
      case BuiltinType::LongLong:
        return printBuiltinTypeMultiSet<long long>(ptr);
      case BuiltinType::Float:
        return printBuiltinTypeMultiSet<float>(ptr);
      case BuiltinType::Double:
        return printBuiltinTypeMultiSet<double>(ptr);
      case BuiltinType::LongDouble:
        return printBuiltinTypeMultiSet<long double>(ptr);
      default:
        break;
      }
    }
  }
  return "";
}

std::string printUnorderedSet(void *ptr, CXXRecordDecl *RecordDecl,
                              QualType Type) {
  const auto *specDecl = dyn_cast<ClassTemplateSpecializationDecl>(RecordDecl);
  if (!specDecl) {
    return "";
  }

  const TemplateArgumentList &tplArgs = specDecl->getTemplateArgs();
  assert(tplArgs.size() != 0);
  const TemplateArgument &tplArg = tplArgs[0];
  /// for builtin type unordered set, use `printBuiltinTypeUnorderedSet`
  if (tplArg.getKind() == TemplateArgument::Type) {
    if (const BuiltinType *BT =
            dyn_cast<BuiltinType>(tplArg.getAsType().getTypePtr())) {
      switch (BT->getKind()) {
      case BuiltinType::Bool:
        return printBuiltinTypeUnorderedSet<bool>(ptr);
      case BuiltinType::Char_U:
      case BuiltinType::UChar:
        return printBuiltinTypeUnorderedSet<unsigned char>(ptr);
      case BuiltinType::Char_S:
      case BuiltinType::SChar:
        return printBuiltinTypeUnorderedSet<signed char>(ptr);
      case BuiltinType::WChar_S:
        return printBuiltinTypeUnorderedSet<wchar_t>(ptr);
      case BuiltinType::Char16:
        return printBuiltinTypeUnorderedSet<char16_t>(ptr);
      case BuiltinType::Char32:
        return printBuiltinTypeUnorderedSet<char32_t>(ptr);
      case BuiltinType::UShort:
        return printBuiltinTypeUnorderedSet<unsigned short>(ptr);
      case BuiltinType::Short:
        return printBuiltinTypeUnorderedSet<short>(ptr);
      case BuiltinType::UInt:
        return printBuiltinTypeUnorderedSet<unsigned int>(ptr);
      case BuiltinType::Int:
        return printBuiltinTypeUnorderedSet<int>(ptr);
      case BuiltinType::ULong:
        return printBuiltinTypeUnorderedSet<unsigned long>(ptr);
      case BuiltinType::Long:
        return printBuiltinTypeUnorderedSet<long>(ptr);
      case BuiltinType::ULongLong:
        return printBuiltinTypeUnorderedSet<unsigned long long>(ptr);
      case BuiltinType::LongLong:
        return printBuiltinTypeUnorderedSet<long long>(ptr);
      case BuiltinType::Float:
        return printBuiltinTypeUnorderedSet<float>(ptr);
      case BuiltinType::Double:
        return printBuiltinTypeUnorderedSet<double>(ptr);
      case BuiltinType::LongDouble:
        return printBuiltinTypeUnorderedSet<long double>(ptr);
      default:
        break;
      }
    }
  }
  return "";
}

std::string printRecord(void *ptr, const Value &V, const ASTContext &C,
                        CXXRecordDecl *RecordDecl, QualType Type) {
  if (RecordDecl->getNameAsString() == "basic_string") {
    return printStdString(ptr, C, Type);
  }
  if (RecordDecl->getNameAsString() == "vector") {
    return printStdVector(ptr, V, C, RecordDecl, Type);
  }
  if (RecordDecl->getNameAsString() == "deque") {
    return printStdDeque(ptr, RecordDecl, Type);
  }
  if (RecordDecl->getNameAsString() == "array") {
    return printStdArray(ptr, V, C, RecordDecl, Type);
  }
  if (RecordDecl->getNameAsString() == "list") {
    return printList(ptr, RecordDecl, Type);
  }
  if (RecordDecl->getNameAsString() == "set") {
    return printSet(ptr, RecordDecl, Type);
  }
  if (RecordDecl->getNameAsString() == "multiset") {
    return printMultiSet(ptr, RecordDecl, Type);
  }
  if (RecordDecl->getNameAsString() == "unordered_set") {
    return printUnorderedSet(ptr, RecordDecl, Type);
  }

  std::ostringstream values;
  auto fields = RecordDecl->fields();
  for (auto I = fields.begin(), E = fields.end(), tmp = I; I != E;
       ++I, tmp = I) {
    FieldDecl *field = *I;
    QualType fieldType = field->getType();
    size_t offset = C.getFieldOffset(field) /* bits */ / 8;
    void *fieldPtr = (void *)((char *)ptr + offset);
    values << field->getNameAsString() << ": (" << GetFullTypeName(C, fieldType)
           << ") " << printValuePtr(fieldPtr, V, C, fieldType);
    if (++tmp != E) {
      values << ", ";
    }
  }
  if (!values.str().empty()) {
    return "{ " + values.str() + " }";
  }
  return "";
}

std::string printValuePtr(void *ptr, const Value &V, const ASTContext &C,
                          QualType Type) {
  if (!ptr) {
    return "nullptr";
  }
  if (Type->isNullPtrType()) {
    return "nullptr_t";
  }
  if (Type->isEnumeralType()) {
    return printEnumByPtr(ptr, C, Type);
  }
  if (const BuiltinType *bt =
          llvm::dyn_cast<BuiltinType>(Type.getCanonicalType().getTypePtr())) {
    BuiltinType::Kind Kind = bt->getKind();
    std::string str = printBuiltinTypeByPtr(ptr, Kind);
    if (!str.empty()) {
      return str;
    }
  } else if (CXXRecordDecl *RecordDecl = Type->getAsCXXRecordDecl()) {
    if (RecordDecl->isLambda()) {
      return printAddress(ptr, '@');
    }
    std::string str = printRecord(ptr, V, C, RecordDecl, Type);
    if (!str.empty()) {
      return str;
    }
  } else {
    std::string str = printArray(ptr, V, C, Type);
    if (!str.empty()) {
      return str;
    }
  }
  return printAddress(ptr, '@');
}

std::string printValuePtrTopLevel(void *ptr, const Value &V, QualType Type) {
  const ASTContext &C = V.getASTContext();
  if (!ptr) {
    return "nullptr";
  }
  if (Type->isNullPtrType()) {
    return "nullptr_t";
  }
  if (Type->isEnumeralType()) {
    return printEnumByValue(V, C, Type);
  }
  if (const BuiltinType *bt =
          llvm::dyn_cast<BuiltinType>(Type.getCanonicalType().getTypePtr())) {
    BuiltinType::Kind Kind = bt->getKind();
    std::string str = printBuiltinTypeByValue(V, Kind);
    if (!str.empty()) {
      return str;
    }
  } else if (CXXRecordDecl *RecordDecl = Type->getAsCXXRecordDecl()) {
    if (RecordDecl->isLambda()) {
      return printAddress(ptr, '@');
    }
    std::string str = printRecord(ptr, V, C, RecordDecl, Type);
    if (!str.empty()) {
      return str;
    }
  } else {
    std::string str = printArray(ptr, V, C, Type);
    if (!str.empty()) {
      return str;
    }
  }
  return printAddress(ptr, '@');
}

std::string Value::printValueInternal() const {
  const ASTContext &C = getASTContext();
  const QualType Td = getType().getDesugaredType(C);
  const QualType Ty = Td.getNonReferenceType();
  return printValuePtrTopLevel(this->getPtr(), *this, Ty);
}

} // namespace clang