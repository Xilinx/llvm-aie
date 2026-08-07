; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; The two command line overrides of the versioning pragma: one forces the pass
; off, the other picks loops by their known minimum iteration count so
; versioning can be exercised without editing the source under test.
;
; RUN: llc -mtriple=aie2 -stop-after=aie-inner-loop-versioning %s -o - \
; RUN:   | FileCheck %s --check-prefix=DEFAULT
; RUN: llc -mtriple=aie2 -aie-inner-loop-versioning-min-itercount=1 \
; RUN:   -stop-after=aie-inner-loop-versioning %s -o - \
; RUN:   | FileCheck %s --check-prefix=FORCE
; The pass is not in the pipeline at all when disabled, so stop after the next
; one instead.
; RUN: llc -mtriple=aie2 -aie-disable-inner-loop-versioning \
; RUN:   -stop-after=hardware-loops %s -o - \
; RUN:   | FileCheck %s --check-prefix=DISABLE

; DISABLE-NOT: lver

; The pragma alone versions, with or without the option.
; DEFAULT-LABEL: define void @hinted
; DEFAULT: loop.lver.guard:
; FORCE-LABEL: define void @hinted
; FORCE: loop.lver.guard:
define void @hinted(ptr noalias %a, ptr noalias %b, i32 %n) {
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

; A minimum iteration count of 1 and no pragma: versioned only when the option
; asks for loops that low.
; DEFAULT-LABEL: define void @min_itercount_1
; DEFAULT-NOT: lver
; FORCE-LABEL: define void @min_itercount_1
; FORCE: loop.lver.guard:
define void @min_itercount_1(ptr noalias %a, ptr noalias %b, i32 %n) {
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
  br i1 %c, label %loop, label %exit, !llvm.loop !2
exit:
  ret void
}

; Already carries the versioned marker, as a copy left by an earlier run does.
; The option ignores pragmas, so this marker is what keeps the pass from
; versioning the same loop twice.
; DEFAULT-LABEL: define void @already_versioned
; DEFAULT-NOT: lver
; FORCE-LABEL: define void @already_versioned
; FORCE-NOT: lver
define void @already_versioned(ptr noalias %a, ptr noalias %b, i32 %n) {
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
  br i1 %c, label %loop, label %exit, !llvm.loop !4
exit:
  ret void
}

; The fallback copy of an earlier run keeps the iteration-count range that made
; it a candidate, so without its own marker the option would version it again
; and nest a second guard.
; DEFAULT-LABEL: define void @version_fallback
; DEFAULT-NOT: lver
; FORCE-LABEL: define void @version_fallback
; FORCE-NOT: lver
define void @version_fallback(ptr noalias %a, ptr noalias %b, i32 %n) {
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
  br i1 %c, label %loop, label %exit, !llvm.loop !6
exit:
  ret void
}

!0 = distinct !{!0, !1}
!1 = !{!"llvm.loop.hint.aie-loop-versioning", i64 1}
!2 = distinct !{!2, !3}
!3 = !{!"llvm.loop.itercount.range", i32 1, i32 100}
!4 = distinct !{!4, !3, !5}
!5 = !{!"llvm.loop.hint.aie-loop-versioned", i32 1}
!6 = distinct !{!6, !3, !7}
!7 = !{!"llvm.loop.hint.aie-loop-version-fallback", i32 1}
