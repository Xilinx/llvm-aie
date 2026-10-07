; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; End-to-end: the pre-RA MachinePipeliner pipelines the high-trip-count copy of
; a versioned loop and patches the guard threshold with the stage count it
; needed. Without the runtime guard the loop has no known minimum trip count,
; so the pipeliner refuses it outright: @no_hint is the same loop without the
; hint and stays unpipelined.
;
; The post-pipeliner is kept out of the picture entirely:
; --aie-postpipeliner-maxii=0 leaves no II for it to try, and the remark run
; below asserts that the loop is claimed by the prepipeliner.
;
; This body needs four stages here, one more than the prepipeliner hands out by
; default before deferring to the postpipeliner, so raise that limit rather
; than rely on a pipeliner this test has disabled.
;
; RUN: llc -mtriple=aie2ps -O2 --aie-postpipeliner-maxii=0 \
; RUN:   --aie-pipeliner-max-stagecount=4 %s -o - | FileCheck %s

; RUN: llc -mtriple=aie2ps -O2 --aie-postpipeliner-maxii=0 \
; RUN:   --aie-pipeliner-max-stagecount=4 \
; RUN:   -pass-remarks-output=- -pass-remarks-filter=pipeliner %s -o /dev/null \
; RUN:   | FileCheck %s --check-prefix=REMARK \
; RUN:       --implicit-check-not=postpipeliner

; With default options the prepipeliner leaves the four-stage schedule to the
; postpipeliner. It only defers loops promised more than one iteration, as the
; postpipeliner can do nothing with fewer; the runtime guard is that promise.
; The postpipeliner then patches the guard, and threshold and peel must still
; agree.
; RUN: llc -mtriple=aie2ps -O2 %s -o - | FileCheck %s
; RUN: llc -mtriple=aie2ps -O2 \
; RUN:   -pass-remarks-output=- -pass-remarks-filter=pipeliner %s -o /dev/null \
; RUN:   | FileCheck %s --check-prefix=DEFAULT-REMARK \
; RUN:       --implicit-check-not=prepipeliner

; CHECK-LABEL: {{^}}versioned:
; The guard block holds the patched threshold and the unsigned trip-count
; compare selecting the pipelined vs fallback copy. NSTAGES is captured here and
; reused below: the guard threshold and the ZOL peel are two halves of one fact,
; and the pipelined copy is only correct while they agree.
; CHECK: mova [[THR:r[0-9]+]], #[[#NSTAGES:]]
; CHECK: ltu r{{[0-9]+}}, r{{[0-9]+}}, [[THR]]
; The low-trip-count (fallback) copy runs the loop verbatim: its ZOL count is
; unpeeled.
; CHECK-LABEL: %loop.ph
; CHECK: add.nc lc, r{{[0-9]+}}, #0
; The high-trip-count copy peels NSTAGES-1 iterations into its prologue, so its
; ZOL count is that much shorter. The guard above admits exactly the trip counts
; that keep it positive.
; CHECK-LABEL: %loop.ph.lver.high
; CHECK: add.nc lc, r{{[0-9]+}}, #-[[#NSTAGES-1]]

; The same loop without the versioning hint: no guard, hence no minimum trip
; count to peel against, so the loop is left alone.
; CHECK-LABEL: {{^}}no_hint:
; CHECK-NOT: ltu
; CHECK: add.nc lc, r{{[0-9]+}}, #0
; CHECK-NOT: add.nc lc

; The high-trip-count copy is claimed by the prepipeliner. --implicit-check-not
; covers the other half of the claim: no remark mentions the postpipeliner.
; REMARK:      - Pipeliner:       prepipeliner
; REMARK:      - Loop:            bb.{{[0-9]+}}.loop.lver.high

; DEFAULT-REMARK:      - Pipeliner:       postpipeliner
; DEFAULT-REMARK:      - Loop:            bb.{{[0-9]+}}.loop.lver.high

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
  %c = icmp ne i32 %i.next, %n
  br i1 %c, label %loop, label %exit, !llvm.loop !0
exit:
  ret void
}

define void @no_hint(ptr noalias %a, ptr noalias %b, i32 %n) {
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
  %c = icmp ne i32 %i.next, %n
  br i1 %c, label %loop, label %exit
exit:
  ret void
}

!0 = distinct !{!0, !1}
!1 = !{!"llvm.loop.hint.aie-loop-versioning", i64 1}
