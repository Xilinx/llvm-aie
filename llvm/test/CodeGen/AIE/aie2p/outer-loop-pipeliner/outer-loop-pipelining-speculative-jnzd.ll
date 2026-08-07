; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=false -aie-enable-outer-loop-pipelining \
; RUN:     -aie-outer-loop-pipelining-speculative \
; RUN:     -stop-after=aie-outer-loop-pipeliner -o - %s | FileCheck %s
;
; A side-effect-free no-anchor stage 0 selects speculative lowering. The outer
; downcounting loop still uses JNZD, but its trip count is the full N because no
; iteration is peeled.

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

; CHECK-LABEL: define void @speculative_jnzd(
; CHECK:       stage0.top:
; CHECK:       load i32, ptr %a
; CHECK:       %outer.jnzd.tc = sub i32 %n, 0
; CHECK:       %outer.ctr.init = call i32 @llvm.start.loop.iterations.i32(i32 %outer.jnzd.tc)
; CHECK:       steady.stage1.top:
; CHECK:       %outer.ctr = phi i32 [ %outer.ctr.init, %stage0.top ]
; CHECK:       steady.stage1.bottom.and.stage0.top:
; CHECK:       %outer.ctr.next = call i32 @llvm.loop.decrement.reg.i32
; CHECK:       %outer.loop.cond = icmp ne i32 %outer.ctr.next, 0
; CHECK:       br i1 %outer.loop.cond, label %steady.stage1.top, label %exit
; CHECK-NOT:   lastiter.stage1.top
; CHECK:       "llvm.loop.hint.aie_outerloop_pipeliner_speculative", i64 1
define void @speculative_jnzd(ptr %a, ptr %c, i32 %n, i32 %m) {
entry:
  %has.work = icmp ugt i32 %n, 1
  br i1 %has.work, label %outer.header, label %exit

outer.header:
  %iv = phi i32 [ %n, %entry ], [ %iv.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  %loaded = load i32, ptr %a.ptr, align 4
  call void @llvm.set.loop.iterations.i32(i32 %m)
  br label %inner.header

inner.header:
  %result = phi i32 [ 0, %outer.header ], [ %result.next, %inner.header ]
  %result.next = add i32 %result, %loaded
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  store i32 %result.next, ptr %c.ptr, align 4
  %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 1
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %outer.header, !llvm.loop !0

exit:
  ret void
}

!0 = distinct !{!0, !2, !3}
!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.mustprogress"}
!3 = !{!"llvm.loop.itercount.range", i32 2}
