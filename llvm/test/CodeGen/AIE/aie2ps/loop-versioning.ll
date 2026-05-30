; NOTE: Full-pipeline test of loop versioning on aie2ps, with two versioned
; loops in one function. The post-RA fixup must identify and patch each loop's
; runtime guard independently; mis-identification aborts compilation, so a
; passing run proves both guards were located and overwritten correctly.
;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2ps -stop-after=postmisched %s -o - \
; RUN:   | FileCheck %s --implicit-check-not='{{MOV[A-Za-z0-9_]*}} 3'

; Each loop is versioned: a guard block selects the pipelined fast copy or the
; un-pipelined slow copy. Phase 1 gives the two loops distinct placeholder guard
; thresholds (2 and 3); phase 2 overwrites each with the actual stage count (2),
; so no constant move of 3 survives (the --implicit-check-not above).
; CHECK-DAG: loop1.lver.guard
; CHECK-DAG: loop2.lver.guard
; Both fast copies are software-pipelined: their zero-overhead-loop trip count is
; reduced by NStages-1 (a negative adjustment), while the slow copies keep 0.
; CHECK-COUNT-2: ADD_NC_add_lc_ri{{.*}}-1

define void @two_loops(ptr noalias %a, ptr noalias %b, i32 %n, i32 %m) {
entry:
  br label %loop1
loop1:
  %i = phi i32 [0, %entry], [%i.next, %loop1]
  %pa = getelementptr i32, ptr %a, i32 %i
  %x = load i32, ptr %pa, align 4
  %y = mul i32 %x, 1234
  %pb = getelementptr i32, ptr %b, i32 %i
  store i32 %y, ptr %pb, align 4
  %i.next = add i32 %i, 1
  %c1 = icmp slt i32 %i.next, %n
  br i1 %c1, label %loop1, label %mid, !llvm.loop !0
mid:
  br label %loop2
loop2:
  %j = phi i32 [0, %mid], [%j.next, %loop2]
  %pb2 = getelementptr i32, ptr %b, i32 %j
  %z = load i32, ptr %pb2, align 4
  %w = add i32 %z, 99
  %pa2 = getelementptr i32, ptr %a, i32 %j
  store i32 %w, ptr %pa2, align 4
  %j.next = add i32 %j, 1
  %c2 = icmp slt i32 %j.next, %m
  br i1 %c2, label %loop2, label %exit, !llvm.loop !2
exit:
  ret void
}

!0 = distinct !{!0, !1}
!1 = !{!"llvm.loop.hint.aie-loop-versioning", i64 8}
!2 = distinct !{!2, !3}
!3 = !{!"llvm.loop.hint.aie-loop-versioning", i64 4}
