; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; IR-level loop-versioning transform: a loop carrying the versioning hint is
; split into a high-trip-count copy (the clone, keeps the hint, pipelined later)
; and a low-trip-count copy (the original, hint stripped), behind a runtime
; trip-count guard. The guard threshold is a placeholder produced by the thin
; intrinsic; the postpipeliner patches it.
;
; RUN: llc -mtriple=aie2ps -stop-after=aie-inner-loop-versioning %s -o - \
; RUN:   | FileCheck %s

; CHECK-LABEL: define void @versioned
; The guard: threshold from the intrinsic, unsigned trip-count compare. Below
; the threshold take the low-trip-count (original) copy, otherwise the cloned
; high-trip-count copy.
; CHECK: %[[THR:.*]] = call i32 @llvm.aie2ps.loop.version.threshold(i32 -1)
; CHECK: %[[LOW:.*]] = icmp ult i32 %{{.*}}, %[[THR]]
; CHECK: br i1 %[[LOW]], label %{{.*}}.ph, label %{{.*}}.lver.high
; The cloned high-trip-count copy carries the ".lver.high" suffix and is marked
; with the aie-loop-versioned hint.
; CHECK: loop.lver.high:
; CHECK: br i1 %{{.*}}, !llvm.loop [[HIGHMD:![0-9]+]]
; The original low-trip-count copy's loop id is rebuilt without the hint.
; CHECK: br i1 %{{.*}}, !llvm.loop [[LOWMD:![0-9]+]]
; Cloning leaves both copies branching to the same exit block, so each gets its
; own dedicated exit again and stays in loop-simplify form for later passes.
; CHECK: exit.lver.high.loopexit: ; preds = %loop.lver.high
; CHECK: exit.lver.low.loopexit: ; preds = %loop{{$}}

define void @versioned(ptr noalias %a, ptr noalias %b, i32 %n) {
entry:
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop ]
  %pa = getelementptr i32, ptr %a, i32 %i
  %x = load i32, ptr %pa, align 4
  %y = mul i32 %x, 1234
  %pb = getelementptr i32, ptr %b, i32 %i
  store i32 %y, ptr %pb, align 4
  %i.next = add i32 %i, 1
  %c = icmp slt i32 %i.next, %n
  br i1 %c, label %loop, label %exit, !llvm.loop !0
exit:
  ret void
}

; A loop whose exit count is wider than i32 (i64 induction variable) cannot be
; expressed as a 32-bit zero-overhead loop, so the high-trip-count copy could
; never be pipelined. Versioning bails and leaves the loop untouched: no guard,
; no clone.
; CHECK-LABEL: define void @wide_trip_count
; CHECK-NOT: icmp ult
; CHECK-NOT: lver
define void @wide_trip_count(ptr noalias %a, ptr noalias %b, i64 %n) {
entry:
  br label %loop
loop:
  %i = phi i64 [ 0, %entry ], [ %i.next, %loop ]
  %pa = getelementptr i32, ptr %a, i64 %i
  %x = load i32, ptr %pa, align 4
  %y = mul i32 %x, 1234
  %pb = getelementptr i32, ptr %b, i64 %i
  store i32 %y, ptr %pb, align 4
  %i.next = add i64 %i, 1
  %c = icmp slt i64 %i.next, %n
  br i1 %c, label %loop, label %exit, !llvm.loop !0
exit:
  ret void
}

; Metadata defs live at the module end, past both CHECK-LABEL barriers: the
; high-trip-count copy's loop id references the aie-loop-versioned marker.
; CHECK-DAG: [[HIGHMD]] = distinct !{[[HIGHMD]], [[MARK:![0-9]+]]}
; CHECK-DAG: [[MARK]] = !{!"llvm.loop.hint.aie-loop-versioned", i32 1}

!0 = distinct !{!0, !1}
!1 = !{!"llvm.loop.hint.aie-loop-versioning", i64 1}
