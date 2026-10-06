; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=false \
; RUN:     -aie-enable-outer-loop-pipelining \
; RUN:     -stop-after=aie-outer-loop-pipeliner -o - %s | FileCheck %s

; The merge PHI in the steady header exists so stage-1 can read a value the
; preheader or the previous bottom produced. A stage-0 value whose only
; consumers are in stage 0 has no such reader: those consumers are cloned into
; the preheader and the bottom, where they use the clones directly.

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

; Both loads feed only the multiply, which is in stage 0 as well, so only the
; multiply needs a merge PHI.
define void @dead_stage0_load_phi(ptr noalias %a, ptr noalias %b, ptr noalias %c,
                                  i32 %N, i32 %M) {
; CHECK-LABEL: define void @dead_stage0_load_phi(
; CHECK:       stage0.top:
; CHECK:         %v0.steady.top = load i32, ptr %a
; CHECK:         %v1.steady.top = load i32, ptr %b
; CHECK:         %prod.steady.top = mul i32 %v0.steady.top, %v1.steady.top
; CHECK:       steady.stage1.top:
; CHECK-NOT:     %v0.steady.phi
; CHECK-NOT:     %v1.steady.phi
; CHECK:         %prod.steady.phi = phi i32 [ %prod.steady.top, %stage0.top ], [ %prod.steady.bottom, %steady.stage1.bottom.and.stage0.top ]
; CHECK:         ret void
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %outer.header, label %exit

outer.header:
  %i = phi i32 [ 0, %entry ], [ %i.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %b.ptr = phi ptr [ %b, %entry ], [ %b.ptr.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  %v0 = load i32, ptr %a.ptr, align 4
  %v1 = load i32, ptr %b.ptr, align 4
  %prod = mul i32 %v0, %v1
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  %acc = phi i32 [ 0, %outer.header ], [ %acc.next, %inner.header ]
  %acc.next = add i32 %acc, %prod
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  store i32 %acc.next, ptr %c.ptr, align 4
  %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 1
  %b.ptr.next = getelementptr inbounds i32, ptr %b.ptr, i32 1
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  %i.next = add nuw i32 %i, 1
  %outer.cond = icmp eq i32 %i.next, %N
  br i1 %outer.cond, label %exit, label %outer.header, !llvm.loop !0

exit:
  ret void
}

; The load is read from inside the inner loop, which stays in stage 1, so its
; merge PHI is live and must survive.
define void @live_stage0_load_phi(ptr noalias %a, ptr noalias %c, i32 %N,
                                  i32 %M) {
; CHECK-LABEL: define void @live_stage0_load_phi(
; CHECK:       steady.stage1.top:
; CHECK:         %v0.steady.phi = phi i32 [ %v0.steady.top, %stage0.top ], [ %v0.steady.bottom, %steady.stage1.bottom.and.stage0.top ]
; CHECK:       steady.stage1.inner.inner.header:
; CHECK:         add i32 %{{.*}}, %v0.steady.phi
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
  %i.next = add nuw i32 %i, 1
  %outer.cond = icmp eq i32 %i.next, %N
  br i1 %outer.cond, label %exit, label %outer.header, !llvm.loop !0

exit:
  ret void
}

!0 = distinct !{!0, !2, !3}
!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.mustprogress"}
!3 = !{!"llvm.loop.itercount.range", i32 2}
