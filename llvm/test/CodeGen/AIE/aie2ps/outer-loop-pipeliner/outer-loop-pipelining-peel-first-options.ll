; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; REQUIRES: asserts
; RUN: llc -mtriple=aie2ps -aie-enable-outer-loop-pipelining \
; RUN:     -aie-outer-loop-pipelining-mode=first \
; RUN:     -aie-outer-loop-pipelining-speculative \
; RUN:     -aie-outer-loop-pipelining-lean-stage0 \
; RUN:     -start-before=aie-outer-loop-pipeliner \
; RUN:     -stop-after=aie-outer-loop-pipeliner \
; RUN:     -debug-only=aie-outer-loop-pipeliner -o - %s 2>&1 \
; RUN:   | FileCheck %s
; RUN: llc -mtriple=aie2ps -aie-enable-outer-loop-pipelining \
; RUN:     -aie-outer-loop-pipelining-mode=first \
; RUN:     -start-before=aie-outer-loop-pipeliner \
; RUN:     -stop-after=aie-outer-loop-pipeliner \
; RUN:     -debug-only=aie-outer-loop-pipeliner -o - %s 2>&1 \
; RUN:   | FileCheck %s --check-prefix=DEFAULT
;
; Peel-first does not use the peel-last options. When they are set explicitly
; it reports that it ignores them, and still peels the first iteration.

; CHECK: Peel-first: ignoring -aie-outer-loop-pipelining-speculative, which only applies to peel-last mode
; CHECK: Peel-first: ignoring -aie-outer-loop-pipelining-lean-stage0, which only applies to peel-last mode
; CHECK: firstiter.stage0.top:
; CHECK: lastiter.stage1.bottom:
; CHECK-NOT: aie_outerloop_pipeliner_speculative

; DEFAULT-NOT: Peel-first: ignoring
; DEFAULT: firstiter.stage0.top:

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

define void @peel_last_options(ptr noalias %src, ptr noalias %dst, i32 %N, i32 %M) {
entry:
  %cmp = icmp sgt i32 %N, 1
  br i1 %cmp, label %ph, label %exit

ph:
  br label %outer.header

outer.header:
  %i = phi i32 [ 0, %ph ], [ %i.next, %outer.latch ]
  %src.ptr = phi ptr [ %src, %ph ], [ %src.next, %outer.latch ]
  %dst.ptr = phi ptr [ %dst, %ph ], [ %dst.next, %outer.latch ]
  %val = load i32, ptr %src.ptr, align 4
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  %acc = phi i32 [ 0, %outer.header ], [ %acc.next, %inner.header ]
  %acc.next = add i32 %acc, %val
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  %result = phi i32 [ %acc.next, %inner.header ]
  store i32 %result, ptr %dst.ptr, align 4
  %src.next = getelementptr inbounds i32, ptr %src.ptr, i32 1
  %dst.next = getelementptr inbounds i32, ptr %dst.ptr, i32 1
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
