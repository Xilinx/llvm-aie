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
; RUN: llc -mtriple=aie2 -stop-after=aie-inner-loop-versioning %s -o - \
; RUN:   | FileCheck %s

; CHECK-LABEL: define void @versioned
; The guard: threshold from the intrinsic, unsigned trip-count compare. Below
; the threshold take the low-trip-count (original) copy, otherwise the cloned
; high-trip-count copy.
; CHECK: %[[THR:.*]] = call i32 @llvm.aie2.loop.version.threshold(i32 -1)
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
  br i1 %c, label %loop, label %exit, !llvm.loop !2
exit:
  ret void
}

; The candidate and profitability gates are pure SCEV and LoopInfo reasoning,
; so the four cases below are covered for aie2 only.
;
; An i16 induction variable gives an i17 trip count, which fits i32 with room
; to spare. Versioning applies, and the guard widens the trip count instead of
; truncating it.
; CHECK-LABEL: define void @narrow_trip_count
; CHECK: %[[TC:.*]] = zext i17 %{{.*}} to i32
; CHECK: icmp ult i32 %[[TC]]
define void @narrow_trip_count(ptr noalias %a, ptr noalias %b, i16 %n) {
entry:
  br label %loop
loop:
  %i = phi i16 [ 0, %entry ], [ %i.next, %loop ]
  %idx = sext i16 %i to i32
  %pa = getelementptr i32, ptr %a, i32 %idx
  %x = load i32, ptr %pa, align 4
  %y = mul i32 %x, 1234
  %pb = getelementptr i32, ptr %b, i32 %idx
  store i32 %y, ptr %pb, align 4
  %i.next = add i16 %i, 1
  %c = icmp slt i16 %i.next, %n
  br i1 %c, label %loop, label %exit, !llvm.loop !3
exit:
  ret void
}

; An unsigned "i != n" loop has an exit count of up to UINT32_MAX, so its trip
; count needs 33 bits even though the exit count is i32. The guard could not
; hold it, so versioning bails. A check on the exit count's type width would
; have accepted this loop.
; CHECK-LABEL: define void @unsigned_wrapping_trip_count
; CHECK-NOT: icmp ult
; CHECK-NOT: lver
define void @unsigned_wrapping_trip_count(ptr noalias %a, ptr noalias %b,
                                          i32 %n) {
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
  %c = icmp ne i32 %i.next, %n
  br i1 %c, label %loop, label %exit, !llvm.loop !4
exit:
  ret void
}

; A data-dependent exit leaves SCEV with no backedge count at all, so there is
; no trip count to build a guard on. This is the profitability gate's other
; rejection, the one the wide and wrapping cases above do not reach.
; CHECK-LABEL: define void @unknown_trip_count
; CHECK-NOT: icmp ult
; CHECK-NOT: lver
define void @unknown_trip_count(ptr noalias %a, ptr noalias %b) {
entry:
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop ]
  %pa = getelementptr i32, ptr %a, i32 %i
  %x = load i32, ptr %pa, align 4
  %pb = getelementptr i32, ptr %b, i32 %i
  store i32 %x, ptr %pb, align 4
  %i.next = add i32 %i, 1
  %c = icmp ne i32 %x, 0
  br i1 %c, label %loop, label %exit, !llvm.loop !7
exit:
  ret void
}

; Both loops of a nest carry the hint. Versioning the outer one would clone the
; inner one into the hot copy, where nothing revisits it: the inner loop would
; end up versioned on the cold path only, with a live request hint stranded on
; the hot path. Only the innermost loop is versioned; the outer keeps its hint.
; CHECK-LABEL: define void @nested_hints
; CHECK-NOT: outer.lver
; CHECK: inner.lver.guard
; CHECK-NOT: outer.lver
define void @nested_hints(ptr noalias %a, ptr noalias %b, i32 %n, i32 %m) {
entry:
  br label %outer
outer:
  %i = phi i32 [ 0, %entry ], [ %i.next, %outer.latch ]
  br label %inner
inner:
  %j = phi i32 [ 0, %outer ], [ %j.next, %inner ]
  %idx = add i32 %i, %j
  %pa = getelementptr i32, ptr %a, i32 %idx
  %x = load i32, ptr %pa, align 4
  %y = mul i32 %x, 1234
  %pb = getelementptr i32, ptr %b, i32 %idx
  store i32 %y, ptr %pb, align 4
  %j.next = add i32 %j, 1
  %cj = icmp slt i32 %j.next, %m
  br i1 %cj, label %inner, label %outer.latch, !llvm.loop !5
outer.latch:
  %i.next = add i32 %i, 1
  %ci = icmp slt i32 %i.next, %n
  br i1 %ci, label %outer, label %exit, !llvm.loop !6
exit:
  ret void
}

; A loop that is not always entered: the exit block is reached from the
; function entry as well, and its PHI mixes a value defined in the loop with
; one that bypasses it. Cloning must keep that merge intact for both copies.
; CHECK-LABEL: define i32 @conditional_entry
; The guard lands in the preheader canonicalization created, so the bypass edge
; out of entry is untouched.
; CHECK: br i1 %enter, label %loop.lver.guard, label %exit
; CHECK: loop.lver.guard:
; CHECK: br i1 %{{.*}}, label %loop.ph, label %loop.ph.lver.high
; Each copy exits through its own dedicated block; the loop-defined value is
; merged there, and the bypass PHI keeps selecting -1 on the entry edge.
; CHECK: %[[LCSSA:.*]] = phi i32 [ %{{.*}}, %exit.lver.high.loopexit ], [ %{{.*}}, %exit.lver.low.loopexit ]
; CHECK: %result = phi i32 [ -1, %entry ], [ %[[LCSSA]], %exit.loopexit ]
define i32 @conditional_entry(ptr %a, i32 %n, i1 %enter) {
entry:
  br i1 %enter, label %loop, label %exit
loop:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop ]
  %x = load i32, ptr %a, align 4
  %i.next = add i32 %i, 1
  %c = icmp slt i32 %i.next, %n
  br i1 %c, label %loop, label %exit, !llvm.loop !8
exit:
  %result = phi i32 [ -1, %entry ], [ %x, %loop ]
  ret i32 %result
}

; Metadata defs live at the module end, past every CHECK-LABEL barrier: the
; high-trip-count copy's loop id references the aie-loop-versioned marker.
; CHECK-DAG: [[HIGHMD]] = distinct !{[[HIGHMD]], [[MARK:![0-9]+]]}
; CHECK-DAG: [[MARK]] = !{!"llvm.loop.hint.aie-loop-versioned", i32 1}

; One loop id per loop; they share the hint entry node.
!0 = distinct !{!0, !1}
!1 = !{!"llvm.loop.hint.aie-loop-versioning", i64 1}
!2 = distinct !{!2, !1}
!3 = distinct !{!3, !1}
!4 = distinct !{!4, !1}
!5 = distinct !{!5, !1}
!6 = distinct !{!6, !1}
!7 = distinct !{!7, !1}
!8 = distinct !{!8, !1}
