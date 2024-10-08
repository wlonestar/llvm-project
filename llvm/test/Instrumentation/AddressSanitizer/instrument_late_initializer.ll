; NOTE: Assertions have been autogenerated by utils/update_test_checks.py UTC_ARGS: --check-globals all --global-value-regex ".*global_ctors.*" --version 5
; RUN: opt < %s -passes=asan -S | FileCheck %s
; RUN: opt < %s -passes=asan -S -asan-initialization-order=0 | FileCheck %s --check-prefixes=NOINIT

target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64"
target triple = "x86_64-unknown-linux-gnu"

@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @__late_ctor, ptr null }]
@g = internal global i32 0, align 4, sanitize_address_dyninit  ; With dynamic initializer.

;.
; CHECK: @llvm.global_ctors = appending global [2 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @__late_ctor, ptr null }, { i32, ptr, ptr } { i32 1, ptr @asan.module_ctor, ptr @asan.module_ctor }]
;.
; NOINIT: @llvm.global_ctors = appending global [2 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @__late_ctor, ptr null }, { i32, ptr, ptr } { i32 1, ptr @asan.module_ctor, ptr @asan.module_ctor }]
;.
define i32 @initializer() uwtable {
; CHECK-LABEL: define i32 @initializer(
; CHECK-SAME: ) #[[ATTR0:[0-9]+]] {
; CHECK-NEXT:  [[ENTRY:.*:]]
; CHECK-NEXT:    ret i32 42
;
; NOINIT-LABEL: define i32 @initializer(
; NOINIT-SAME: ) #[[ATTR0:[0-9]+]] {
; NOINIT-NEXT:  [[ENTRY:.*:]]
; NOINIT-NEXT:    ret i32 42
;
entry:
  ret i32 42
}

define internal void @__cxx_global_var_init() section ".text.startup" {
; CHECK-LABEL: define internal void @__cxx_global_var_init() section ".text.startup" {
; CHECK-NEXT:  [[ENTRY:.*:]]
; CHECK-NEXT:    [[CALL:%.*]] = call i32 @initializer()
; CHECK-NEXT:    store i32 [[CALL]], ptr @g, align 4
; CHECK-NEXT:    ret void
;
; NOINIT-LABEL: define internal void @__cxx_global_var_init() section ".text.startup" {
; NOINIT-NEXT:  [[ENTRY:.*:]]
; NOINIT-NEXT:    [[CALL:%.*]] = call i32 @initializer()
; NOINIT-NEXT:    store i32 [[CALL]], ptr @g, align 4
; NOINIT-NEXT:    ret void
;
entry:
  %call = call i32 @initializer()
  store i32 %call, ptr @g, align 4
  ret void
}

define internal void @__late_ctor() sanitize_address section ".text.startup" {
; CHECK-LABEL: define internal void @__late_ctor(
; CHECK-SAME: ) #[[ATTR1:[0-9]+]] section ".text.startup" {
; CHECK-NEXT:  [[ENTRY:.*:]]
; CHECK-NEXT:    call void @__asan_before_dynamic_init(i64 ptrtoint (ptr @___asan_gen_module to i64))
; CHECK-NEXT:    call void @__asan_after_dynamic_init()
; CHECK-NEXT:    ret void
;
; NOINIT-LABEL: define internal void @__late_ctor(
; NOINIT-SAME: ) #[[ATTR1:[0-9]+]] section ".text.startup" {
; NOINIT-NEXT:  [[ENTRY:.*:]]
; NOINIT-NEXT:    ret void
;
entry:
  ret void
}
