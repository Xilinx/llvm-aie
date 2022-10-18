;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -O2 -mtriple=aie2 --stop-after=branch-folder %s -o - | FileCheck %s

; This effectively checks whether branch optimization gets it right.
; We get both conditional branches generated with optimal fallthrough

; CHECK: body:             |
; CHECK:   bb.0.entry:
; CHECK:     PseudoJNZ killed renamable $r0, %bb.2  
; CHECK:   bb.1.if.then:
; CHECK:     PseudoJL @f0
; CHECK:   bb.2.if.end:
; CHECK:     PseudoJZ killed renamable $r16, %bb.4
; CHECK:   bb.3.if.then2:
; CHECK:     PseudoJL @f0
; CHECK:   bb.4.if.end3:
; CHECK:     PseudoRET

define void @f(i32 %i, i32 %j) {
entry:
  %cmp = icmp eq i32 %i, 0
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  tail call void @f0() #2
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %cmp1.not = icmp eq i32 %j, 0
  br i1 %cmp1.not, label %if.end3, label %if.then2

if.then2:                                         ; preds = %if.end
  tail call void @f0() #2
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.end
  ret void
}

declare void @f0()
