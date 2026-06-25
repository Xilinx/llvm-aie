; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pipelining \
; RUN:     -stop-after=aie-outer-loop-pipeliner \
; RUN:     -o - %s 2>&1 | FileCheck %s


; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pipelining \
; RUN:     -stop-after=aie-outer-loop-pipeliner -o - %s \
; RUN:   | llc -mtriple=aie2p -x mir -run-pass=none -o /dev/null

; Tests for correct handling of arbitrary constant induction steps in the
; AIE Outer Loop Pipeliner pass.
;
; For a decrement outer loop with step -k, removing one iteration requires
; adjusting the exit limit by k (NewLimit = OldLimit - Step = OldLimit + k).
; The unified formula NewLimit = Limit - Step handles all step magnitudes.
;
; JNZD hardware loop conversion is restricted to step -1 because
; @llvm.loop.decrement.reg always decrements the counter by exactly 1.

; ============================================================================
; Test 1: Decrement outer loop with step -2.
;
; The outer loop exits when %iv.next (= %iv + (-2)) == 0.
; The pipeliner must adjust the limit from 0 to 2 (= 0 - (-2)).
; The loop is pipelined but NOT converted to a JNZD hardware loop (step != -1).
; ============================================================================

; CHECK-LABEL: define void @decrement_step2

; Warm-up block: data load only, no outer hardware-loop setup.
; Absence of @llvm.start.loop.iterations here confirms no JNZD conversion.
; CHECK: outer.header.peel.pro:
; CHECK:   %v0.peel = load i32
; CHECK-NOT: @llvm.start.loop.iterations
; CHECK:   br label %outer.header

; Outer header: pipelined load PHI, standard inner-loop set.loop.iterations.
; No @llvm.start.loop.iterations for outer (outer loop is NOT a JNZD hw loop).
; CHECK: outer.header:
; CHECK:   %v0.phi = phi i32 [ %v0.peel, %outer.header.peel.pro ], [ %v0.epi, %outer.latch ]
; CHECK:   call void @llvm.set.loop.iterations.i32(i32 %M)
; CHECK:   br label %inner.header

; Outer latch: step-2 counter + adjusted limit, then epilogue load.
; The limit is adjusted to 2 (constant folded: 0 - (-2) = 2), not 1.
; No @llvm.loop.decrement.reg — outer loop kept as software icmp (step != -1).
; CHECK: outer.latch:
; CHECK:   store i32
; CHECK:   %iv.next = add i32 %iv, -2
; CHECK:   %outer.cond = icmp eq i32 %iv.next, 2
; CHECK:   %v0.epi = load i32
; CHECK-NEXT:   br i1 %outer.cond, label %cooldown.entry, label %outer.header

define void @decrement_step2(ptr noalias %a, ptr noalias %c, i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %outer.header, label %exit

outer.header:
  ; Induction variable starts at %N and decrements by 2 each iteration.
  ; The loop runs N/2 times (N must be a multiple of 2).
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  %v0 = load i32, ptr %a.ptr, align 4
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  %acc = phi i32 [ 0, %outer.header ], [ %acc.next, %inner.header ]
  %acc.next = add i32 %acc, %v0
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  store i32 %acc.next, ptr %c.ptr, align 4
  ; Pointer advances — lifted to outer.header by liftEpiloguePointerUpdatesToPrologue
  ; (no epilogue uses outside the chain).
  %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 2
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 2
  ; Step = -2. The exit condition is (iv - 2) == 0, i.e., iv == 2.
  %iv.next = add i32 %iv, -2
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %outer.header, !llvm.loop !0

exit:
  ret void
}

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

!0 = distinct !{!0, !2, !3}
!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.mustprogress"}
!3 = !{!"llvm.loop.itercount.range", i32 2}

; ============================================================================
; Test 2: Decrement outer loop with step -1 (regression guard).
;
; With step -1, the outer loop is both pipelined AND converted to a JNZD
; hardware loop. @llvm.start.loop.iterations appears in the warm-up block
; and @llvm.loop.decrement.reg replaces the software icmp in the latch.
; ============================================================================

; CHECK-LABEL: define void @decrement_step1

; Warm-up block: data load + outer JNZD hardware-loop setup (step -1 only).
; CHECK: outer.header.peel.pro:
; CHECK:   %v0.peel = load i32
; CHECK:   %outer.jnzd.tc = sub i32 %N, 1
; CHECK:   %outer.ctr.init = call i32 @llvm.start.loop.iterations.i32(i32 %outer.jnzd.tc)
; CHECK:   br label %outer.header

; Outer header: hardware-loop counter PHI for the outer JNZD loop.
; CHECK: outer.header:
; CHECK:   %v0.phi = phi i32 [ %v0.peel, %outer.header.peel.pro ], [ %v0.epi, %outer.latch ]
; CHECK:   %outer.ctr = phi i32 [ %outer.ctr.init, %outer.header.peel.pro ]
; CHECK:   call void @llvm.set.loop.iterations.i32(i32 %M)

; Outer latch: @llvm.loop.decrement.reg replaces the software %iv.next/%outer.cond.
; CHECK: outer.latch:
; CHECK:   store i32
; CHECK:   %v0.epi = load i32
; CHECK:   %outer.ctr.next = call i32 @llvm.loop.decrement.reg.i32
; CHECK:   %outer.loop.cond = icmp ne i32 %outer.ctr.next, 0
; CHECK:   br i1 %outer.loop.cond, label %outer.header, label %cooldown.entry

define void @decrement_step1(ptr noalias %a, ptr noalias %c, i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %outer.header, label %exit

outer.header:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  %v0 = load i32, ptr %a.ptr, align 4
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  %acc = phi i32 [ 0, %outer.header ], [ %acc.next, %inner.header ]
  %acc.next = add i32 %acc, %v0
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !5

outer.latch:
  store i32 %acc.next, ptr %c.ptr, align 4
  %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 1
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  ; Step = -1: eligible for JNZD hardware-loop conversion.
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %outer.header, !llvm.loop !4

exit:
  ret void
}

!4 = distinct !{!4, !6, !7}
!5 = distinct !{!5, !6}
!6 = !{!"llvm.loop.mustprogress"}
!7 = !{!"llvm.loop.itercount.range", i32 2}
