; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; Two versioned loops in one function. Each guard's threshold pseudo is
; non-CSE-able, so the two placeholders never merge and the post-pipeliner
; patches each guard independently with its own stage count. Mis-identification
; would abort compilation (report_fatal_error), so reaching the end and emitting
; two guards is the check.
;
; RUN: llc -mtriple=aie2ps -O2 -aie-force-postpipeliner %s -o - | FileCheck %s

; CHECK-LABEL: two_versioned:
; Both loops are versioned: two guard blocks, each with its own patched
; threshold move and trip-count compare.
; CHECK-DAG: loop1.lver.guard
; CHECK-DAG: loop2.lver.guard
; CHECK-DAG: ltu r{{[0-9]+}}, r{{[0-9]+}}, r{{[0-9]+}}

define void @two_versioned(ptr noalias %a, ptr noalias %b, i32 %n, i32 %m) {
entry:
  br label %loop1
loop1:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop1 ]
  %pa = getelementptr i32, ptr %a, i32 %i
  %x = load i32, ptr %pa, align 4
  %y = mul i32 %x, 7
  %pb = getelementptr i32, ptr %b, i32 %i
  store i32 %y, ptr %pb, align 4
  %i.next = add i32 %i, 1
  %c1 = icmp slt i32 %i.next, %n
  br i1 %c1, label %loop1, label %mid, !llvm.loop !0
mid:
  br label %loop2
loop2:
  %j = phi i32 [ 0, %mid ], [ %j.next, %loop2 ]
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
!1 = !{!"llvm.loop.hint.aie-loop-versioning", i64 1}
!2 = distinct !{!2, !1}
