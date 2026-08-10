; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; A user who sets the versioning pragma and gets nothing should be able to find
; out why. Each way a hinted loop can be turned down reports itself, as does
; success. Reasons are target independent, so aie2 alone covers them.
;
; RUN: llc -mtriple=aie2 -stop-after=aie-inner-loop-versioning \
; RUN:   -pass-remarks=aie-inner-loop-versioning \
; RUN:   -pass-remarks-missed=aie-inner-loop-versioning %s -o /dev/null 2>&1 \
; RUN:   | FileCheck %s

; CHECK: loop versioned: a runtime trip-count guard selects a copy the post-pipeliner may pipeline
define void @versioned(ptr noalias %a, ptr noalias %b, i32 %n) {
entry:
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop ]
  %pa = getelementptr i32, ptr %a, i32 %i
  %x = load i32, ptr %pa, align 4
  %pb = getelementptr i32, ptr %b, i32 %i
  store i32 %x, ptr %pb, align 4
  %i.next = add i32 %i, 1
  %c = icmp slt i32 %i.next, %n
  br i1 %c, label %loop, label %exit, !llvm.loop !0
exit:
  ret void
}

; CHECK: loop not versioned because its trip count is unknown or does not fit 32 bits
define void @unprofitable(ptr noalias %a, ptr noalias %b, i64 %n) {
entry:
  br label %loop
loop:
  %i = phi i64 [ 0, %entry ], [ %i.next, %loop ]
  %pa = getelementptr i32, ptr %a, i64 %i
  %x = load i32, ptr %pa, align 4
  %pb = getelementptr i32, ptr %b, i64 %i
  store i32 %x, ptr %pb, align 4
  %i.next = add i64 %i, 1
  %c = icmp slt i64 %i.next, %n
  br i1 %c, label %loop, label %exit, !llvm.loop !1
exit:
  ret void
}

; CHECK: loop not versioned because it is not innermost
define void @not_innermost(ptr noalias %a, ptr noalias %b, i32 %n, i32 %m) {
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
  %pb = getelementptr i32, ptr %b, i32 %idx
  store i32 %x, ptr %pb, align 4
  %j.next = add i32 %j, 1
  %cj = icmp slt i32 %j.next, %m
  br i1 %cj, label %inner, label %outer.latch
outer.latch:
  %i.next = add i32 %i, 1
  %ci = icmp slt i32 %i.next, %n
  br i1 %ci, label %outer, label %exit, !llvm.loop !2
exit:
  ret void
}

!0 = distinct !{!0, !3}
!1 = distinct !{!1, !3}
!2 = distinct !{!2, !3}
!3 = !{!"llvm.loop.hint.aie-loop-versioning", i64 1}
