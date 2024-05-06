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

define dso_local i32 @dot(ptr nocapture readonly %a, ptr nocapture readonly %b, i32 noundef %n) {
; CHECK-LABEL: dot:
; CHECK:         .p2align 4
; CHECK-NEXT:  // %bb.0: // %entry
; CHECK-NEXT:    nopa ; add.nc r1, r1, #-1
; CHECK-NEXT:    jz r1, #.LBB0_2
; CHECK-NEXT:    lda r2, [p0, #0] // Delay Slot 5
; CHECK-NEXT:    padda [p0], #2044 // Delay Slot 4
; CHECK-NEXT:    lda r3, [p1, #0] // Delay Slot 3
; CHECK-NEXT:    padda [p1], #2044 // Delay Slot 2
; CHECK-NEXT:    mova r0, #0 // Delay Slot 1
; CHECK-NEXT:    .p2align 4
; CHECK-NEXT:  .LBB0_1: // %do.body
; CHECK-NEXT:    // =>This Inner Loop Header: Depth=1
; CHECK-NEXT:    lda r4, [p0, #0]; nopb ; nopx
; CHECK-NEXT:    lda r5, [p1, #0]
; CHECK-NEXT:    nop
; CHECK-NEXT:    add.nc r1, r1, #-1
; CHECK-NEXT:    jnz r1, #.LBB0_1
; CHECK-NEXT:    nop // Delay Slot 5
; CHECK-NEXT:    nop // Delay Slot 4
; CHECK-NEXT:    and r2, r3, r2 // Delay Slot 3
; CHECK-NEXT:    padda [p0], #2044; or r0, r2, r0; mov r2, r4 // Delay Slot 2
; CHECK-NEXT:    padda [p1], #2044; mov r3, r5 // Delay Slot 1
; CHECK-NEXT:    .p2align 4
; CHECK-NEXT:  .LBB0_2:
; CHECK-NEXT:    nopb ; nopa ; nops ; ret lr ; nopm ; nopv
; CHECK-NEXT:    nopx // Delay Slot 5
; CHECK-NEXT:    nop // Delay Slot 4
; CHECK-NEXT:    nop // Delay Slot 3
; CHECK-NEXT:    and r1, r3, r2 // Delay Slot 2
; CHECK-NEXT:    or r0, r1, r0 // Delay Slot 1
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
