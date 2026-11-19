; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2ps -aie-enable-outer-loop-pipelining \
; RUN:     -aie-outer-loop-pipelining-mode=first \
; RUN:     -start-before=aie-outer-loop-pipeliner \
; RUN:     -stop-after=aie-outer-loop-pipeliner \
; RUN:     -o - %s | FileCheck %s
;
; The hardware-loop counter of a JNZD input is also read by the loop body.
; Peeling the first iteration lowers the start.loop.iterations count, which
; would shift that read, so peel-first declines the loop.

; CHECK-LABEL: define void @jnzd_counter_read(
; CHECK-NOT:   firstiter.
; CHECK:       ret void

declare i32 @llvm.start.loop.iterations.i32(i32)
declare i32 @llvm.loop.decrement.reg.i32(i32, i32)
declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

define void @jnzd_counter_read(ptr noalias %in, ptr noalias %out, i32 %outer.trip, i32 %inner.trip) {
entry:
  %runs.twice = icmp ugt i32 %outer.trip, 1
  br i1 %runs.twice, label %outer.ph, label %exit

outer.ph:
  %outer.hwloop.start = call i32 @llvm.start.loop.iterations.i32(i32 %outer.trip)
  br label %outer.header

outer.header:
  %outer.remaining = phi i32 [ %outer.hwloop.start, %outer.ph ], [ %outer.remaining.next, %outer.latch ]
  %in.ptr = phi ptr [ %in, %outer.ph ], [ %in.ptr.next, %outer.latch ]
  %out.ptr = phi ptr [ %out, %outer.ph ], [ %out.ptr.next, %outer.latch ]
  %in.val = load i32, ptr %in.ptr, align 4
  call void @llvm.set.loop.iterations.i32(i32 %inner.trip)
  br label %inner.header

inner.header:
  %acc = phi i32 [ %outer.remaining, %outer.header ], [ %acc.next, %inner.header ]
  %acc.next = add i32 %acc, %in.val
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  %result = phi i32 [ %acc.next, %inner.header ]
  store i32 %result, ptr %out.ptr, align 4
  %in.ptr.next = getelementptr inbounds i32, ptr %in.ptr, i32 1
  %out.ptr.next = getelementptr inbounds i32, ptr %out.ptr, i32 1
  %outer.remaining.next = call i32 @llvm.loop.decrement.reg.i32(i32 %outer.remaining, i32 1)
  %outer.continue = icmp ne i32 %outer.remaining.next, 0
  br i1 %outer.continue, label %outer.header, label %exit, !llvm.loop !0

exit:
  ret void
}

!0 = distinct !{!0, !2, !3}
!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.mustprogress"}
!3 = !{!"llvm.loop.itercount.range", i32 2}
