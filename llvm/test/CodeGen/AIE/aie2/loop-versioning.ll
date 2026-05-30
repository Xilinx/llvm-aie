; NOTE: This test checks the IR-level loop-versioning transform.
;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2 -stop-after=aie-loop-versioning %s -o - | FileCheck %s

; A loop carrying the versioning hint is split into a fast copy (keeps the hint,
; gets pipelined later) and a slow copy (hint stripped), behind a runtime
; trip-count guard. The guard starts at a placeholder threshold that the
; postpipeliner later overwrites with the actual stage count.

; CHECK-LABEL: define void @versioned
; CHECK:         icmp ult i32 %{{.*}}, 2
; CHECK:         br i1
; The cloned slow copy gives the body a second ".lver.slow" occurrence, and the
; fast copy keeps the versioning hint while the slow copy's loop id is rebuilt
; without it.
; CHECK:       loop.lver.slow:
; CHECK:         br i1 %{{.*}}, !llvm.loop [[SLOWMD:![0-9]+]]
; CHECK:         br i1 %{{.*}}, !llvm.loop [[FASTMD:![0-9]+]]
define void @versioned(ptr noalias %a, ptr noalias %b, i32 %n) {
entry:
  br label %loop
loop:
  %i = phi i32 [0, %entry], [%i.next, %loop]
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

; Two versioned loops in one function get DISTINCT placeholder thresholds (2 and
; 3), so MachineCSE cannot merge their guard constants into one definition and
; the per-loop fixup can patch each guard independently.
; CHECK-LABEL: define void @two_versioned
; CHECK:         icmp ult i32 %{{.*}}, 2
; CHECK:         icmp ult i32 %{{.*}}, 3
define void @two_versioned(ptr noalias %a, ptr noalias %b, i32 %n, i32 %m) {
entry:
  br label %loop1
loop1:
  %i = phi i32 [0, %entry], [%i.next, %loop1]
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
  %j = phi i32 [0, %mid], [%j.next, %loop2]
  %pb2 = getelementptr i32, ptr %b, i32 %j
  %z = load i32, ptr %pb2, align 4
  %w = add i32 %z, 99
  %pa2 = getelementptr i32, ptr %a, i32 %j
  store i32 %w, ptr %pa2, align 4
  %j.next = add i32 %j, 1
  %c2 = icmp slt i32 %j.next, %m
  br i1 %c2, label %loop2, label %exit, !llvm.loop !0
exit:
  ret void
}

; Without the hint the loop is left untouched: no guard, no clone.
; CHECK-LABEL: define void @not_versioned
; CHECK-NOT:     icmp ult
; CHECK-NOT:     lver
define void @not_versioned(ptr noalias %a, i32 %n) {
entry:
  br label %loop
loop:
  %i = phi i32 [0, %entry], [%i.next, %loop]
  %p = getelementptr i32, ptr %a, i32 %i
  store i32 %i, ptr %p, align 4
  %i.next = add i32 %i, 1
  %c = icmp slt i32 %i.next, %n
  br i1 %c, label %loop, label %exit
exit:
  ret void
}

; Metadata defs live at the module end, past the CHECK-LABEL barriers: the fast
; copy's loop id still references the versioning hint; the slow copy's was
; rebuilt without it (single self-reference operand).
; CHECK-DAG: [[FASTMD]] = distinct !{[[FASTMD]], [[HINT:![0-9]+]]}
; CHECK-DAG: [[HINT]] = !{!"llvm.loop.hint.aie-loop-versioning", i64 1}
; CHECK-DAG: [[SLOWMD]] = distinct !{[[SLOWMD]]}

!0 = distinct !{!0, !1}
!1 = !{!"llvm.loop.hint.aie-loop-versioning", i64 1}
