; RUN: llvm-as %s -o %t.o
; RUN: llvm-as %p/Inputs/ctors2.ll -o %t2.o
; RUN: %gold -plugin %llvmshlibdir/LLVMgold.so \
; RUN:    --plugin-opt=emit-llvm \
; RUN:    -shared %t.o %t2.o -o %t3.o
; RUN: llvm-dis %t3.o -o - | FileCheck %s

target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"

@llvm.global_ctors = appending global [1 x { i32, void ()*, i8* }] [{ i32, void ()*, i8* } { i32 65535, void ()* @foo, i8* null }]

define void @foo() {
  ret void
}

; CHECK: @llvm.global_ctors = appending global [2 x { i32, void ()*, i8* }] [{ i32, void ()*, i8* } { i32 65535, void ()* @foo, i8* null }, { i32, void ()*, i8* } { i32 65535, void ()* @bar, i8* null }]
