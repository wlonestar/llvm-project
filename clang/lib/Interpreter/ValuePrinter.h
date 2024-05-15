//===--- ValuePrinter.h - Value Printer -------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file defines some value printer functions for clang-repl.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_VALUE_PRINTER_H
#define LLVM_CLANG_VALUE_PRINTER_H

#include "InterpreterUtils.h"

#include "clang/AST/APValue.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/Type.h"
#include "clang/AST/TypeLoc.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/Specifiers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Interpreter/Interpreter.h"
#include "clang/Interpreter/Value.h"
#include "clang/Sema/Lookup.h"
#include "clang/Sema/ParsedAttr.h"

#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>
#include <codecvt>
#include <cstddef>
#include <cstdint>
#include <locale>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

namespace clang {

std::string printAddress(const void *ptr, const char prefix = 0);

REPL_EXTERNAL_VISIBILITY
std::string printValue(const void *ptr);
REPL_EXTERNAL_VISIBILITY
std::string printValue(const void **ptr);

REPL_EXTERNAL_VISIBILITY
std::string printValue(const bool *ptr);

REPL_EXTERNAL_VISIBILITY
std::string printValue(const char *ptr);
REPL_EXTERNAL_VISIBILITY
std::string printValue(const signed char *ptr);
REPL_EXTERNAL_VISIBILITY
std::string printValue(const unsigned char *ptr);
REPL_EXTERNAL_VISIBILITY
std::string printValue(const wchar_t *ptr);
REPL_EXTERNAL_VISIBILITY
std::string printValue(const char16_t *ptr);
REPL_EXTERNAL_VISIBILITY
std::string printValue(const char32_t *ptr);

REPL_EXTERNAL_VISIBILITY
std::string printValue(const unsigned short *ptr);
REPL_EXTERNAL_VISIBILITY
std::string printValue(const short *ptr);
REPL_EXTERNAL_VISIBILITY
std::string printValue(const unsigned int *ptr);
REPL_EXTERNAL_VISIBILITY
std::string printValue(const int *ptr);
REPL_EXTERNAL_VISIBILITY
std::string printValue(const unsigned long *ptr);
REPL_EXTERNAL_VISIBILITY
std::string printValue(const long *ptr);
REPL_EXTERNAL_VISIBILITY
std::string printValue(const unsigned long long *ptr);
REPL_EXTERNAL_VISIBILITY
std::string printValue(const long long *ptr);

REPL_EXTERNAL_VISIBILITY
std::string printValue(const float *ptr);
REPL_EXTERNAL_VISIBILITY
std::string printValue(const double *ptr);
REPL_EXTERNAL_VISIBILITY
std::string printValue(const long double *ptr);

REPL_EXTERNAL_VISIBILITY
std::string printValue(char *ptr, bool seq);
REPL_EXTERNAL_VISIBILITY
std::string printValue(signed char *ptr, bool seq);
REPL_EXTERNAL_VISIBILITY
std::string printValue(unsigned char *ptr, bool seq);
REPL_EXTERNAL_VISIBILITY
std::string printValue(wchar_t *ptr, bool seq);
REPL_EXTERNAL_VISIBILITY
std::string printValue(char16_t *ptr, bool seq);
REPL_EXTERNAL_VISIBILITY
std::string printValue(char32_t *ptr, bool seq);

REPL_EXTERNAL_VISIBILITY
std::string printValue(const std::string *ptr);
REPL_EXTERNAL_VISIBILITY
std::string printValue(const std::wstring *ptr);
REPL_EXTERNAL_VISIBILITY
std::string printValue(const std::u16string *ptr);
REPL_EXTERNAL_VISIBILITY
std::string printValue(const std::u32string *ptr);

std::string printBuiltinTypeByValue(const Value &V, BuiltinType::Kind Kind);
std::string printBuiltinTypeByPtr(void *ptr, BuiltinType::Kind Kind);

std::string printEnumByValue(const Value &V, const ASTContext &C,
                             QualType Type);
std::string printEnumByPtr(void *ptr, const ASTContext &C, QualType Type);

bool isCharType(const Type *type);

std::string printCharArray(void *ptr, QualType Type);

std::string printArray(void *ptr, const Value &V, const ASTContext &C,
                       QualType Type);

std::string printStdString(void *ptr, const Value &V, const ASTContext &C,
                           QualType Type);

template <typename T> std::string printBuiltinTypeVector(void *ptr);
template <typename T> std::string printBuiltinTypeDeque(void *ptr);
template <typename T> std::string printBuiltinTypeList(void *ptr);
template <typename T> std::string printBuiltinTypeSet(void *ptr);
template <typename T> std::string printBuiltinTypeMultiSet(void *ptr);
template <typename T> std::string printBuiltinTypeUnorderedSet(void *ptr);

std::string printStdVector(void *ptr, const Value &V, const ASTContext &C,
                           CXXRecordDecl *RecordDecl, QualType Type);
std::string printStdDeque(void *ptr, CXXRecordDecl *RecordDecl, QualType Type);
std::string printStdArray(void *ptr, const Value &V, const ASTContext &C,
                          CXXRecordDecl *RecordDecl, QualType Type);
std::string printList(void *ptr, CXXRecordDecl *RecordDecl, QualType Type);
std::string printSet(void *ptr, CXXRecordDecl *RecordDecl, QualType Type);
std::string printMultiSet(void *ptr, CXXRecordDecl *RecordDecl, QualType Type);
std::string printUnorderedSet(void *ptr, CXXRecordDecl *RecordDecl,
                              QualType Type);

std::string printRecord(void *ptr, const Value &V, const ASTContext &C,
                        CXXRecordDecl *RecordDecl, QualType Type);

std::string printValuePtr(void *ptr, const Value &V, const ASTContext &C,
                          QualType Type);
std::string printValuePtrTopLevel(void *ptr, const Value &V, QualType Type);

} // namespace clang

#endif