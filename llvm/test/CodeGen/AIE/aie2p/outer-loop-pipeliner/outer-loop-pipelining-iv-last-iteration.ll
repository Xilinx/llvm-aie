; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=false -aie-enable-outer-loop-pipelining \
; RUN:     -stop-after=aie-outer-loop-pipeliner \
; RUN:     -o - %s | FileCheck %s

; The peeled last iteration must read the outer IV as the value the steady loop
; carries out on its back edge, like every other outer header PHI.

; Up-counting IV, read by the bottom block.
; CHECK-LABEL: define void @soft_iv_in_bottom
; CHECK:       steady.stage1.bottom.and.stage0.top:
; CHECK:         %iv.next.steady = add i32 %iv.steady, 1
; CHECK:       lastiter.stage1.bottom:
; CHECK-NEXT:    %c.elt.lastiter = getelementptr inbounds i32, ptr %c, i32 %iv.next.steady
; CHECK-NEXT:    store i32 %acc.next.lastiter, ptr %c.elt.lastiter, align 4
; CHECK-NEXT:    br label %exit
define void @soft_iv_in_bottom(ptr noalias %a, ptr noalias %c, i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %outer.header, label %exit

outer.header:
  %iv = phi i32 [ 0, %entry ], [ %iv.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %v0 = load i32, ptr %a.ptr, align 4
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  %acc = phi i32 [ 0, %outer.header ], [ %acc.next, %inner.header ]
  %acc.next = add i32 %acc, %v0
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  %c.elt = getelementptr inbounds i32, ptr %c, i32 %iv
  store i32 %acc.next, ptr %c.elt, align 4
  %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 1
  %iv.next = add i32 %iv, 1
  %outer.cond = icmp eq i32 %iv.next, %N
  br i1 %outer.cond, label %exit, label %outer.header, !llvm.loop !0

exit:
  ret void
}

; Down-counting IV, read by the bottom block.
; CHECK-LABEL: define void @down_iv_in_bottom
; CHECK:       steady.stage1.top:
; CHECK-NEXT:    %iv.steady = phi i32 [ %iv.next.steady, %steady.stage1.bottom.and.stage0.top ], [ %N, %stage0.top ]
; CHECK:       steady.stage1.bottom.and.stage0.top:
; CHECK:         %iv.next.steady = add i32 %iv.steady, -1
; CHECK:       lastiter.stage1.bottom:
; CHECK-NEXT:    %c.elt.lastiter = getelementptr inbounds i32, ptr %c, i32 %iv.next.steady
; CHECK-NEXT:    store i32 %acc.next.lastiter, ptr %c.elt.lastiter, align 4
; CHECK-NEXT:    br label %exit
define void @down_iv_in_bottom(ptr noalias %a, ptr noalias %c, i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %outer.header, label %exit

outer.header:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %v0 = load i32, ptr %a.ptr, align 4
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  %acc = phi i32 [ 0, %outer.header ], [ %acc.next, %inner.header ]
  %acc.next = add i32 %acc, %v0
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  %c.elt = getelementptr inbounds i32, ptr %c, i32 %iv
  store i32 %acc.next, ptr %c.elt, align 4
  %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 1
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %outer.header, !llvm.loop !0

exit:
  ret void
}

; Down-counting IV, read by the inner loop, so its last-iteration clone needs it
; too.
; CHECK-LABEL: define void @down_iv_in_inner
; CHECK:       steady.stage1.top:
; CHECK-NEXT:    %iv.steady = phi i32 [ %iv.next.steady, %steady.stage1.bottom.and.stage0.top ], [ %N, %stage0.top ]
; CHECK:       lastiter.stage1.inner.inner.header:
; CHECK:         %acc.next.lastiter = add i32 %acc.v.lastiter, %iv.next.steady
; CHECK:       lastiter.stage1.bottom:
; CHECK-NEXT:    store i32 %acc.next.lastiter, ptr %c.ptr.next.steady, align 4
; CHECK-NEXT:    br label %exit
define void @down_iv_in_inner(ptr noalias %a, ptr noalias %c, i32 %N, i32 %M) {
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
  %acc.v = add i32 %acc, %v0
  %acc.next = add i32 %acc.v, %iv
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
