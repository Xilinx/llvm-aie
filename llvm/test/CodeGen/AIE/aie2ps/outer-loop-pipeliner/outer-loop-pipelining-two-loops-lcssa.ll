; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2ps -O2 -aie-enable-outer-loop-pipelining \
; RUN:     -stop-after=aie-outer-loop-pipeliner \
; RUN:     -o - %s 2>&1 | FileCheck %s

; Test: Two mutually exclusive pipelined outer loops with a common exit block.
;
; This test verifies that LCSSA form is properly maintained when pipelining
; loops that exit through intermediate blocks to a common exit with phi nodes.
;
; Structure:
;   - Function has two mutually exclusive outer loops (loop1 when cond, loop2 when !cond)
;   - Both loops compute values that flow to a common exit block via phi nodes
;   - Each loop exits through an intermediate block (loop1.exit, loop2.exit)
;   - After pipelining, the exit phi nodes must use the correct LCSSA phi values

; CHECK-LABEL: define void @two_loops_shared_exit

; Verify loop1 is pipelined
; CHECK: stage0.top:
; CHECK:   load i32

; Verify loop2 is pipelined  
; CHECK: stage0.top{{[0-9]+}}:
; CHECK:   load i32

; The exit phi must have correct LCSSA values (not poison) after pipelining.
; CHECK: exit:
; CHECK:   %final.acc = phi i32 [ 0, %entry ], [ %{{[a-z0-9.]+}}, %loop1.exit ], [ %{{[a-z0-9.]+}}, %loop2.exit ]
; CHECK-NOT: poison

define void @two_loops_shared_exit(ptr noalias %a, ptr noalias %b, ptr noalias %c,
                                    i32 %N, i32 %M, i1 %cond) {
entry:
  %cmp = icmp sgt i32 %N, 1
  br i1 %cmp, label %choose.loop, label %exit

choose.loop:
  br i1 %cond, label %outer1.preheader, label %outer2.preheader

; ========== Loop 1 (when cond is true) ==========
; Structure matches working test: preheader -> outer.header -> inner.header -> outer.latch
outer1.preheader:
  br label %outer1.header

outer1.header:
  %i1 = phi i32 [ 0, %outer1.preheader ], [ %i1.next, %outer1.latch ]
  %a.ptr1 = phi ptr [ %a, %outer1.preheader ], [ %a.ptr1.next, %outer1.latch ]
  %b.ptr1 = phi ptr [ %b, %outer1.preheader ], [ %b.ptr1.next, %outer1.latch ]
  %c.ptr1 = phi ptr [ %c, %outer1.preheader ], [ %c.ptr1.next, %outer1.latch ]
  ; Prologue: loads for the inner loop
  %v0.1 = load i32, ptr %a.ptr1, align 4
  %v1.1 = load i32, ptr %b.ptr1, align 4
  ; Set up hardware loop counter
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner1.header

inner1.header:
  %acc1 = phi i32 [ 0, %outer1.header ], [ %acc1.next, %inner1.header ]
  %prod1 = mul i32 %v0.1, %v1.1
  %acc1.next = add i32 %acc1, %prod1
  ; Hardware loop decrement
  %inner1.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner1.cond, label %inner1.header, label %outer1.latch, !llvm.loop !1

outer1.latch:
  ; Epilogue: store result + compute live-out values
  store i32 %acc1.next, ptr %c.ptr1, align 4
  ; Advance pointers
  %a.ptr1.next = getelementptr inbounds i32, ptr %a.ptr1, i32 1
  %b.ptr1.next = getelementptr inbounds i32, ptr %b.ptr1, i32 1
  %c.ptr1.next = getelementptr inbounds i32, ptr %c.ptr1, i32 1
  %i1.next = add i32 %i1, 1
  %outer1.cond = icmp slt i32 %i1.next, %N
  br i1 %outer1.cond, label %outer1.header, label %loop1.exit, !llvm.loop !0

; Intermediate exit block for loop 1
loop1.exit:
  ; Store something specific to loop 1
  store i32 42, ptr %a, align 4
  br label %exit

; ========== Loop 2 (when cond is false) ==========
; Structure matches working test: preheader -> outer.header -> inner.header -> outer.latch
outer2.preheader:
  br label %outer2.header

outer2.header:
  %i2 = phi i32 [ 0, %outer2.preheader ], [ %i2.next, %outer2.latch ]
  %a.ptr2 = phi ptr [ %a, %outer2.preheader ], [ %a.ptr2.next, %outer2.latch ]
  %b.ptr2 = phi ptr [ %b, %outer2.preheader ], [ %b.ptr2.next, %outer2.latch ]
  %c.ptr2 = phi ptr [ %c, %outer2.preheader ], [ %c.ptr2.next, %outer2.latch ]
  ; Prologue: loads + a computation
  %v0.2 = load i32, ptr %a.ptr2, align 4
  %v1.2 = load i32, ptr %b.ptr2, align 4
  %init2 = add i32 %v0.2, %v1.2
  ; Set up hardware loop counter
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner2.header

inner2.header:
  %acc2 = phi i32 [ %init2, %outer2.header ], [ %acc2.next, %inner2.header ]
  %prod2 = mul i32 %v0.2, %v1.2
  %acc2.next = add i32 %acc2, %prod2
  ; Hardware loop decrement
  %inner2.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner2.cond, label %inner2.header, label %outer2.latch, !llvm.loop !3

outer2.latch:
  ; Epilogue: store result + compute live-out values
  store i32 %acc2.next, ptr %c.ptr2, align 4
  ; Advance pointers
  %a.ptr2.next = getelementptr inbounds i32, ptr %a.ptr2, i32 1
  %b.ptr2.next = getelementptr inbounds i32, ptr %b.ptr2, i32 1
  %c.ptr2.next = getelementptr inbounds i32, ptr %c.ptr2, i32 1
  %i2.next = add i32 %i2, 1
  %outer2.cond = icmp slt i32 %i2.next, %N
  br i1 %outer2.cond, label %outer2.header, label %loop2.exit, !llvm.loop !2

; Intermediate exit block for loop 2
loop2.exit:
  ; Store something specific to loop 2
  store i32 24, ptr %b, align 4
  br label %exit

; ========== Common exit ==========
exit:
  ; This phi must have valid LCSSA values from BOTH loops after pipelining.
  ; The formLCSSARecursively call ensures proper LCSSA phis are created.
  %final.acc = phi i32 [ 0, %entry ], [ %acc1.next, %loop1.exit ], [ %acc2.next, %loop2.exit ]
  store i32 %final.acc, ptr %c, align 4
  ret void
}

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

; Loop 1 outer loop metadata
!0 = distinct !{!0, !4, !5}
; Loop 1 inner loop metadata
!1 = distinct !{!1, !4}

; Loop 2 outer loop metadata
!2 = distinct !{!2, !4, !5}
; Loop 2 inner loop metadata
!3 = distinct !{!3, !4}

!4 = !{!"llvm.loop.mustprogress"}
!5 = !{!"llvm.loop.itercount.range", i32 2}
