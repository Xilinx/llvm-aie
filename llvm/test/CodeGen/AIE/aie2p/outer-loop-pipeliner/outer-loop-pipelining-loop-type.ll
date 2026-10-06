; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; Test outer loop type selection: -aie-outer-loop-type=[soft,hw,auto] and
; -aie-outer-loop-pointer-threshold=N
;
; Explicit soft mode: forces software loop regardless of pointer count
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pipelining \
; RUN:     -aie-outer-loop-type=soft \
; RUN:     -stop-after=aie-outer-loop-pipeliner -o - %s | FileCheck %s --check-prefix=SOFT
;
; Explicit hw mode: forces hardware loop (JNZD) regardless of pointer count
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pipelining \
; RUN:     -aie-outer-loop-type=hw \
; RUN:     -stop-after=aie-outer-loop-pipeliner -o - %s | FileCheck %s --check-prefix=HW
;
; Auto mode with low threshold (1): 2 pointer PHIs >= 1, so use soft loop
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pipelining \
; RUN:     -aie-outer-loop-pointer-threshold=1 \
; RUN:     -stop-after=aie-outer-loop-pipeliner -o - %s | FileCheck %s --check-prefix=SOFT
;
; Auto mode with high threshold (100): 2 pointer PHIs < 100, so use hw loop
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pipelining \
; RUN:     -aie-outer-loop-pointer-threshold=100 \
; RUN:     -stop-after=aie-outer-loop-pipeliner -o - %s | FileCheck %s --check-prefix=HW

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

; SOFT-LABEL: define void @outer_loop_type_test(
; SOFT:       steady.stage1.bottom.and.stage0.top:
; SOFT:       %iv.next{{.*}} = add i32 %iv{{.*}}, -1
; SOFT:       icmp eq i32 %iv.next
; SOFT-NOT:   llvm.loop.decrement.reg
; SOFT-NOT:   llvm.start.loop.iterations

; HW-LABEL: define void @outer_loop_type_test(
; HW:       stage0.top:
; HW:       %outer.jnzd.tc = sub i32 %n, 1
; HW:       %outer.ctr.init = call i32 @llvm.start.loop.iterations.i32(i32 %outer.jnzd.tc)
; HW:       steady.stage1.top:
; HW:       %outer.ctr = phi i32 [ %outer.ctr.init, %stage0.top ]
; HW:       steady.stage1.bottom.and.stage0.top:
; HW:       %outer.ctr.next = call i32 @llvm.loop.decrement.reg.i32
; HW:       %outer.loop.cond = icmp ne i32 %outer.ctr.next, 0
; HW:       br i1 %outer.loop.cond, label %steady.stage1.top, label %lastiter

define void @outer_loop_type_test(ptr %a, ptr %c, i32 %n, i32 %m) {
entry:
  %has.work = icmp ugt i32 %n, 1
  br i1 %has.work, label %outer.header, label %exit

outer.header:
  ; IV PHI (non-pointer) + 2 pointer PHIs = 2 base pointers
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
