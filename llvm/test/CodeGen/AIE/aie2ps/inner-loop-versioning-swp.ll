; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; End-to-end: a versioned loop that the post-pipeliner can schedule gets its
; guard threshold patched with the required stage count (>= 2). The threshold is
; a dedicated side-effecting multi-slot pseudo (so MachineCSE never merges
; sibling guards' placeholders); the post-RA scheduler materializes it into a
; concrete scalar move. Without patching, the placeholder would remain 0.
;
; This also pins down the correctness coupling that has no other direct test:
; the guard threshold (NStages) and the high-trip-count copy's ZOL trip-count
; adjustment (-(NStages-1)) must agree. The guard guarantees the pipelined copy
; only runs when count >= NStages, so lc = count-(NStages-1) stays >= 1. Here
; NStages is 2: threshold #2, high-copy peel #-1, and the low copy keeps an
; unpeeled #0.
;
; RUN: llc -mtriple=aie2ps -O2 -aie-force-postpipeliner %s -o - | FileCheck %s

; CHECK-LABEL: versioned:
; The guard block holds the patched threshold (the stage count) and the unsigned
; trip-count compare selecting the pipelined vs fallback copy. The threshold is a
; small positive constant materialized into a scalar register (a mova, no
; slot-blocking movxm), and the compare tests the trip count against it.
; CHECK: mova [[THR:r[0-9]+]], #[[NSTAGES:[2-9][0-9]*]]
; CHECK: ltu r{{[0-9]+}}, r{{[0-9]+}}, [[THR]]
; The low-trip-count (fallback) copy runs the loop verbatim: its ZOL count is
; unpeeled.
; CHECK-LABEL: %loop.ph
; The high-trip-count (pipelined) copy peels NStages-1 stages from its ZOL
; count. For NStages == 2 that is an adjustment of -1.
; CHECK-LABEL: %loop.ph.lver.high
; CHECK: add.nc lc, r{{[0-9]+}}, #-1

define void @versioned(ptr noalias %a, ptr noalias %b, i32 %n) {
entry:
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop ]
  %pa = getelementptr i32, ptr %a, i32 %i
  %x = load i32, ptr %pa, align 4
  %m1 = mul i32 %x, %x
  %m2 = mul i32 %m1, %x
  %m3 = add i32 %m2, %m1
  %m4 = xor i32 %m3, %x
  %pb = getelementptr i32, ptr %b, i32 %i
  store i32 %m4, ptr %pb, align 4
  %i.next = add i32 %i, 1
  %c = icmp slt i32 %i.next, %n
  br i1 %c, label %loop, label %exit, !llvm.loop !0
exit:
  ret void
}

!0 = distinct !{!0, !1}
!1 = !{!"llvm.loop.hint.aie-loop-versioning", i64 1}
