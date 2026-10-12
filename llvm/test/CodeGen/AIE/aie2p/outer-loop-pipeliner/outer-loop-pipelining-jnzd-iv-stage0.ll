; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=false -aie-enable-outer-loop-pipelining \
; RUN:     -aie-outer-loop-type=hw -stop-after=aie-outer-loop-pipeliner \
; RUN:     -o - %s | FileCheck %s

; The JNZD conversion leaves the old outer IV and its counting add as a use
; cycle. The cycle may only be broken when nothing else reads either of them.

; The IV addresses the stage-0 load. The bottom prefetch clone reads the
; next-iteration IV through the counting add, so the IV must keep counting.
; CHECK-LABEL: define void @iv_feeds_stage0
; CHECK:       steady.stage1.top:
; CHECK-NEXT:    %iv.steady = phi i32 [ %iv.next.steady, %steady.stage1.bottom.and.stage0.top ], [ %N, %stage0.top ]
; CHECK:       steady.stage1.bottom.and.stage0.top:
; CHECK:         %iv.next.steady = add i32 %iv.steady, -1
; CHECK-NEXT:    %a.row.steady.bottom = getelementptr inbounds i32, ptr %a, i32 %iv.next.steady
; CHECK-NEXT:    %v0.steady.bottom = load i32, ptr %a.row.steady.bottom, align 4
; CHECK:         %outer.ctr.next = call i32 @llvm.loop.decrement.reg.i32(i32 %outer.ctr, i32 1)
define void @iv_feeds_stage0(ptr noalias %a, ptr noalias %c, i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %outer.header, label %exit

outer.header:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  %a.row = getelementptr inbounds i32, ptr %a, i32 %iv
  %v0 = load i32, ptr %a.row, align 4
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  %acc = phi i32 [ 0, %outer.header ], [ %acc.next, %inner.header ]
  %acc.next = add i32 %acc, %v0
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  store i32 %acc.next, ptr %c.ptr, align 4
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %outer.header, !llvm.loop !0

exit:
  ret void
}

; The IV only feeds its counting add, so the cycle is dead and erased.
; CHECK-LABEL: define void @iv_only_counts
; CHECK-NOT:     %iv
; CHECK:         %outer.ctr.next = call i32 @llvm.loop.decrement.reg.i32(i32 %outer.ctr, i32 1)
; CHECK-NOT:     %iv
; CHECK:         ret void
define void @iv_only_counts(ptr noalias %a, ptr noalias %c, i32 %N, i32 %M) {
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
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  store i32 %acc.next, ptr %c.ptr, align 4
  %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 1
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  %iv.next = add i32 %iv, -1
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
