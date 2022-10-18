;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc --mtriple=aie2 -O2 --issue-limit=6 %s -o - | FileCheck %s

; Similar to stage0.ll, but now with a do-while. Again we expect a three
; stage pipeline. We check for two prologue guards and one loop branch;
; the prologue branches have reversed the loop condition, since they exit
; the loop.
; NOTE: we use a large immediate offset in the pointer increments to force
; using padda rather than the padd pseudo. This is awaiting proper handling
; of pseudos in pre-RA scheduling/pipelining

; CHECK-LABEL dot:
;	CHECK: add 	[[LC:r[0-9]+]], r1, #-1
; CHECK: jz 	[[LC]], #.LBB0_4

; CHECK: add 	[[LC]], [[LC]], #-1
; CHECK: jz 	[[LC]], #.LBB0_3

; CHECK:.LBB0_2:
; CHECK: add 	[[LC]], [[LC]], #-1
; CHECK: jnz 	[[LC]], #.LBB0_2
; CHECK:.LBB0_3:
; CHECK:.LBB0_4:

define dso_local i32 @dot(ptr nocapture readonly %a, ptr nocapture readonly %b, i32 noundef %n) {
entry:
  br label %do.body

do.body:                                          ; preds = %do.body, %entry
  %a.addr.0 = phi ptr [ %a, %entry ], [ %incdec.ptr, %do.body ]
  %b.addr.0 = phi ptr [ %b, %entry ], [ %incdec.ptr1, %do.body ]
  %n.addr.0 = phi i32 [ %n, %entry ], [ %dec, %do.body ]
  %s.0 = phi i32 [ 0, %entry ], [ %or, %do.body ]
  %incdec.ptr = getelementptr inbounds i32, ptr %a.addr.0, i20 511
  %0 = load i32, ptr %a.addr.0, align 4
  %incdec.ptr1 = getelementptr inbounds i32, ptr %b.addr.0, i20 511
  %1 = load i32, ptr %b.addr.0, align 4
  %and = and i32 %1, %0
  %or = or i32 %and, %s.0
  %dec = add nsw i32 %n.addr.0, -1
  %cmp.not = icmp eq i32 %dec, 0
  br i1 %cmp.not, label %do.end, label %do.body

do.end:                                           ; preds = %do.body
  ret i32 %or
}
