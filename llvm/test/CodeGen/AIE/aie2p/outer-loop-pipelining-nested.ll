; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -stop-after=aie-outer-loop-pipeliner \
; RUN:     -o - %s 2>&1 | FileCheck %s

; Test for the AIE Outer Loop Pipelining pass with nested loop structures.
;
; This test verifies that the pass correctly handles triple-nested loops
; where only the middle+innermost pair is pipelineable. The pass should
; recursively traverse the loop hierarchy to find valid candidates.
;
; Input structure:
;   outermost_loop {        <- NOT pipelined (has only one subloop but
;                              that subloop itself has a subloop)
;     middle_loop {         <- PIPELINED (single subloop with hardware loop)
;       innermost_loop      <- hardware loop
;     }
;   }
;
; Expected: The pass should recursively find middle_loop and pipeline it,
; producing the standard pipelined structure for the middle+innermost pair.

; CHECK-LABEL: define void @nested_three_level

; The outermost loop remains unchanged
; CHECK: outermost.header:
; CHECK:   br label %middle.header.peel.pro

; The middle loop should be pipelined: check for the warm-up block
; CHECK: middle.header.peel.pro:
; CHECK:   %v0.peel = load i32, ptr %a, align 4
; CHECK:   %v1.peel = load i32, ptr %b, align 4
; CHECK-NOT:  call void @llvm.set.loop.iterations
; CHECK:   br label %middle.header

; Middle header should have PHI nodes for pipelined values
; CHECK: middle.header:
; CHECK-DAG:   %v0.phi = phi i32 [ %v0.peel, %middle.header.peel.pro ], [ %v0.epi, %middle.latch ]
; CHECK-DAG:   %v1.phi = phi i32 [ %v1.peel, %middle.header.peel.pro ], [ %v1.epi, %middle.latch ]
; CHECK:   call void @llvm.set.loop.iterations.i32(i32 %M)
; CHECK:   br label %innermost.header

; Middle latch should have stores + loads for NEXT iteration
; CHECK: middle.latch:
; CHECK:   store i32
; CHECK:   %v0.epi = load i32, ptr %a.ptr.next, align 4
; CHECK:   %v1.epi = load i32, ptr %b.ptr.next, align 4
; CHECK:   br i1 %middle.cond, label %middle.header, label %cooldown.entry

; Cool-down entry should have set.loop.iterations cloned
; CHECK: cooldown.entry:
; CHECK:   call void @llvm.set.loop.iterations.i32(i32 %M)
; CHECK:   br label %innermost.header.cd

; Cloned inner loop
; CHECK: innermost.header.cd:
; CHECK:   br i1 %innermost.cond.cd, label %innermost.header.cd, label %cooldown.exit

; Cool-down exit should have stores only (no loads), branches to outermost.latch
; CHECK: cooldown.exit:
; CHECK:   store i32
; CHECK:   br label %outermost.latch

; Success marker should be present in the metadata
; CHECK: "llvm.loop.hint.aie_outerloop_pipeliner_success", i64 1

define void @nested_three_level(ptr noalias %a, ptr noalias %b, ptr noalias %c,
                                 i32 %K, i32 %N, i32 %M) {
entry:
  %cmp.outermost = icmp sgt i32 %K, 0
  br i1 %cmp.outermost, label %outermost.header, label %exit

outermost.header:
  %k = phi i32 [ 0, %entry ], [ %k.next, %outermost.latch ]
  br label %middle.header

middle.header:
  %i = phi i32 [ 0, %outermost.header ], [ %i.next, %middle.latch ]
  %a.ptr = phi ptr [ %a, %outermost.header ], [ %a.ptr.next, %middle.latch ]
  %b.ptr = phi ptr [ %b, %outermost.header ], [ %b.ptr.next, %middle.latch ]
  %c.ptr = phi ptr [ %c, %outermost.header ], [ %c.ptr.next, %middle.latch ]
  ; Prologue: loads for the innermost loop
  %v0 = load i32, ptr %a.ptr, align 4
  %v1 = load i32, ptr %b.ptr, align 4
  ; Set up hardware loop counter
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %innermost.header

innermost.header:
  %acc = phi i32 [ 0, %middle.header ], [ %acc.next, %innermost.header ]
  %prod = mul i32 %v0, %v1
  %acc.next = add i32 %acc, %prod
  ; Hardware loop decrement: returns i1 (true = continue loop)
  %innermost.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %innermost.cond, label %innermost.header, label %middle.latch, !llvm.loop !2

middle.latch:
  ; Epilogue: store result
  store i32 %acc.next, ptr %c.ptr, align 4
  ; Advance pointers
  %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 1
  %b.ptr.next = getelementptr inbounds i32, ptr %b.ptr, i32 1
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  %i.next = add i32 %i, 1
  %middle.cond = icmp slt i32 %i.next, %N
  br i1 %middle.cond, label %middle.header, label %outermost.latch, !llvm.loop !0

outermost.latch:
  %k.next = add i32 %k, 1
  %outermost.cond = icmp slt i32 %k.next, %K
  br i1 %outermost.cond, label %outermost.header, label %exit, !llvm.loop !3

exit:
  ret void
}

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

; Middle loop metadata - enable outer loop pipelining
!0 = distinct !{!0, !1, !4, !5}
!1 = !{!"llvm.loop.mustprogress"}
!4 = !{!"llvm.loop.itercount.range", i32 2}
!5 = !{!"llvm.loop.hint.aie-enable-outer-loop-pipelining", i64 1}

; Innermost loop metadata
!2 = distinct !{!2, !1}

; Outermost loop metadata - no pipelining enabled
!3 = distinct !{!3, !1}
