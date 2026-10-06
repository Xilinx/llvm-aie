;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2 -stop-after=irtranslator %s -o - | FileCheck %s
; RUN: llc -mtriple=aie2p -stop-after=irtranslator %s -o - | FileCheck %s
; RUN: llc -mtriple=aie2ps -stop-after=irtranslator %s -o - | FileCheck %s

; Return assignments only depend on the return type, so a function with several
; returns of different values must translate without hitting the return
; pre-lowering consistency check.

define i32 @multiple_returns(i32 %x) {
  ; CHECK-LABEL: name: multiple_returns
  ; CHECK: bb.2.a:
  ; CHECK: PseudoRET implicit $lr, implicit $r0
  ; CHECK: bb.3.b:
  ; CHECK: PseudoRET implicit $lr, implicit $r0
entry:
  %c = icmp eq i32 %x, 0
  br i1 %c, label %a, label %b
a:
  ret i32 10
b:
  ret i32 20
}
