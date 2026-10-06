;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2 -stop-after=codegenprepare %s -o - | FileCheck %s
; RUN: llc -mtriple=aie2p -stop-after=codegenprepare %s -o - | FileCheck %s
; RUN: llc -mtriple=aie2ps -stop-after=codegenprepare %s -o - | FileCheck %s

declare i32 @f0()
declare i32 @f1()

define i32 @dup(i1 %c) {
; CHECK-LABEL: @dup
; CHECK:       %ta = tail call i32 @f0()
; CHECK-NEXT:  ret i32 %ta
; CHECK:       %tb = tail call i32 @f1()
; CHECK-NEXT:  ret i32 %tb
; CHECK-NOT:   phi
entry:
  br i1 %c, label %a, label %b
a:
  %ta = tail call i32 @f0()
  br label %exit
b:
  %tb = tail call i32 @f1()
  br label %exit
exit:
  %p = phi i32 [ %ta, %a ], [ %tb, %b ]
  ret i32 %p
}
