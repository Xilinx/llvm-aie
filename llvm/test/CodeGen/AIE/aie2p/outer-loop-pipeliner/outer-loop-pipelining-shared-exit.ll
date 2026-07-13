; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; REQUIRES: asserts
; RUN: not --crash llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pipelining \
; RUN:     -stop-after=aie-outer-loop-pipeliner \
; RUN:     -o - %s 2>&1 | FileCheck %s
;
; The outer loop's exit block is shared between the preheader-bypass edge
; (entry -> exit) and the loop back-edge, so the live-out is an LCSSA phi with
; an incoming value from entry and no dedicated exit.loopexit block. The
; original latch still branches to the exit until it is deleted, so its phi
; entry must survive swap-in (a clone-latch incoming is added, not renamed) and
; then be retargeted to the last-iteration epilogue. This previously crashed
; while deleting the original loop.

; CHECK: Invalid basic block argument to remove!

define i32 @shared_exit(ptr noalias %a, ptr noalias %c, i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %outer.header, label %exit

outer.header:
  %i = phi i32 [ 0, %entry ], [ %i.next, %outer.latch ]
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
  %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 1
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  %i.next = add i32 %i, 1
  %outer.cond = icmp slt i32 %i.next, %N
  br i1 %outer.cond, label %outer.header, label %exit, !llvm.loop !0

exit:
  %acc.lcssa = phi i32 [ 0, %entry ], [ %acc.next, %outer.latch ]
  ret i32 %acc.lcssa
}

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

!0 = distinct !{!0, !2, !3}
!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.mustprogress"}
!3 = !{!"llvm.loop.itercount.range", i32 2}
