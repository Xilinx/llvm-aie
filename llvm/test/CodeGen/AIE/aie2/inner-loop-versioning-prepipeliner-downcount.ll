; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; inner-loop-versioning-prepipeliner.ll, but for a loop the pre-RA
; MachinePipeliner handles as a DownCountLoop rather than a zero-overhead loop:
; hardware loops are disabled and the loop counts its induction variable down
; to zero.
;
; The runtime guard promises the high-trip-count copy enough iterations for any
; stage count, so it is pipelined as if the trip count were static: the guard
; threshold is patched to the stage count and the prologue stages run without
; guards. Without the hint the loop has only a dynamic trip count; it settles
; for fewer stages and guards each prologue stage. So does the fallback copy.
;
; The guard logic is target independent, so aie2 alone covers it.
;
; RUN: llc -mtriple=aie2 -O2 -enable-aie-hardware-loops=false \
; RUN:   -stop-after=pipeliner %s -o - | FileCheck %s

; CHECK-LABEL: name: versioned
; CHECK-LABEL: bb.1.loop.lver.guard:
; CHECK: PseudoLoopVersionThreshold 3
; Three stages, so two prologue blocks, each falling through to the next
; without a guard; the first conditional branch is the kernel's back edge.
; CHECK-LABEL: bb.2.loop.ph.lver.high:
; CHECK-NOT: PseudoJZ
; CHECK: PseudoJ_jump_imm %bb.[[#PRO2:]]
; CHECK-NOT: PseudoJZ
; CHECK: bb.[[#PRO2]].loop.lver.high:
; CHECK-NOT: PseudoJZ
; CHECK: PseudoJ_jump_imm %bb.[[#KERNEL:]]
; CHECK-NOT: PseudoJZ
; CHECK: bb.[[#KERNEL]].loop.lver.high:
; CHECK-NOT: PseudoJZ
; CHECK: PseudoJNZ %{{[0-9]+}}, %bb.[[#KERNEL]]
; The fallback copy keeps its dynamic trip count, so its prologue is guarded.
; CHECK-LABEL: bb.4.loop.ph:
; CHECK: PseudoJZ

; CHECK-LABEL: name: no_hint
; CHECK-NOT: PseudoLoopVersionThreshold
; CHECK: PseudoJZ

define void @versioned(ptr noalias %a, ptr noalias %b, i32 %n) {
entry:
  br label %loop
loop:
  %i = phi i32 [ %n, %entry ], [ %i.next, %loop ]
  %pa = getelementptr i32, ptr %a, i32 %i
  %x = load i32, ptr %pa, align 4
  %m1 = mul i32 %x, %x
  %m2 = mul i32 %m1, %x
  %m3 = add i32 %m2, %m1
  %m4 = xor i32 %m3, %x
  %pb = getelementptr i32, ptr %b, i32 %i
  store i32 %m4, ptr %pb, align 4
  %i.next = add i32 %i, -1
  %c = icmp ne i32 %i.next, 0
  br i1 %c, label %loop, label %exit, !llvm.loop !0
exit:
  ret void
}

define void @no_hint(ptr noalias %a, ptr noalias %b, i32 %n) {
entry:
  br label %loop
loop:
  %i = phi i32 [ %n, %entry ], [ %i.next, %loop ]
  %pa = getelementptr i32, ptr %a, i32 %i
  %x = load i32, ptr %pa, align 4
  %m1 = mul i32 %x, %x
  %m2 = mul i32 %m1, %x
  %m3 = add i32 %m2, %m1
  %m4 = xor i32 %m3, %x
  %pb = getelementptr i32, ptr %b, i32 %i
  store i32 %m4, ptr %pb, align 4
  %i.next = add i32 %i, -1
  %c = icmp ne i32 %i.next, 0
  br i1 %c, label %loop, label %exit
exit:
  ret void
}

!0 = distinct !{!0, !1}
!1 = !{!"llvm.loop.hint.aie-loop-versioning", i64 1}
