;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; REQUIRES: asserts
; RUN: not --crash llc -mtriple=aie2 -stop-after=irtranslator %s -o - 2>&1 | FileCheck %s
; RUN: not --crash llc -mtriple=aie2p -stop-after=irtranslator %s -o - 2>&1 | FileCheck %s
; RUN: not --crash llc -mtriple=aie2ps -stop-after=irtranslator %s -o - 2>&1 | FileCheck %s

; AIE pre-lowers the return via CallLowering::preLowerReturn(), but IRTranslator
; only pre-lowers the *first* ReturnInst of the function. When a function has
; several returns yielding different values, AIECallLowering::lowerReturnVal()
; is later called with a Value that does not match the cached one, tripping the
; RetAssignments.RetVal == Val assertion.
; FIXME: preLowerReturn must handle multiple returns.

; CHECK: RetAssignments.RetVal == Val

define i32 @multiple_returns(i32 %x) {
entry:
  %c = icmp eq i32 %x, 0
  br i1 %c, label %a, label %b
a:
  ret i32 10
b:
  ret i32 20
}
