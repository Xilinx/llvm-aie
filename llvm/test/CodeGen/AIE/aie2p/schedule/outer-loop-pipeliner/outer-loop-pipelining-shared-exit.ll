; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pipelining \
; RUN:     -stop-after=aie-outer-loop-pipeliner \
; RUN:     -o - %s 2>&1 | FileCheck %s
;
; CHECK lines mirror the full pass-output IR (the RUN command's `--- |`
; section); regenerate them whenever the pass output changes.

; The outer loop's exit block is shared between the preheader-bypass edge
; (entry -> exit) and the loop back-edge, so the live-out is an LCSSA phi with
; an incoming value from entry and no dedicated exit.loopexit block. The
; original latch still branches to the exit until it is deleted, so its phi
; entry must survive swap-in (a clone-latch incoming is added, not renamed) and
; then be retargeted to the last-iteration epilogue. This previously crashed
; while deleting the original loop.

; CHECK-LABEL: define i32 @shared_exit(ptr noalias %a, ptr noalias %c, i32 %N, i32 %M) {
; CHECK: entry:
; CHECK-NEXT:   %cmp.outer = icmp sgt i32 %N, 1
; CHECK-NEXT:   br i1 %cmp.outer, label %outer.header.preheader, label %exit
;
; CHECK: outer.header.preheader:                           ; preds = %entry
; CHECK-NEXT:   br label %steady.preheader
;
; CHECK: steady.preheader:                                 ; preds = %outer.header.preheader
; CHECK-NEXT:   %v0.steady.peel = load i32, ptr %a, align 4
; CHECK-NEXT:   %outer.trip.adj = sub i32 %N, 1
; CHECK-NEXT:   br label %steady.header
;
; CHECK: steady.header:                                    ; preds = %steady.preheader, %steady.latch
; CHECK-NEXT:   %i.steady = phi i32 [ %i.next.steady, %steady.latch ], [ 0, %steady.preheader ]
; CHECK-NEXT:   %a.ptr.steady = phi ptr [ %a.ptr.next.steady, %steady.latch ], [ %a, %steady.preheader ]
; CHECK-NEXT:   %c.ptr.steady = phi ptr [ %c.ptr.next.steady, %steady.latch ], [ %c, %steady.preheader ]
; CHECK-NEXT:   %v0.steady.phi = phi i32 [ %v0.steady.peel, %steady.preheader ], [ %v0.steady.epi, %steady.latch ]
; CHECK-NEXT:   call void @llvm.set.loop.iterations.i32(i32 %M)
; CHECK-NEXT:   %a.ptr.next.steady = getelementptr inbounds i32, ptr %a.ptr.steady, i32 1
; CHECK-NEXT:   %c.ptr.next.steady = getelementptr inbounds i32, ptr %c.ptr.steady, i32 1
; CHECK-NEXT:   br label %steady.inner.header
;
; CHECK: steady.inner.header:                              ; preds = %steady.inner.header, %steady.header
; CHECK-NEXT:   %acc.steady = phi i32 [ 0, %steady.header ], [ %acc.next.steady, %steady.inner.header ]
; CHECK-NEXT:   %acc.next.steady = add i32 %acc.steady, %v0.steady.phi
; CHECK-NEXT:   %inner.cond.steady = call i1 @llvm.loop.decrement.i32(i32 1)
; CHECK-NEXT:   br i1 %inner.cond.steady, label %steady.inner.header, label %steady.latch, !llvm.loop !0
;
; CHECK: steady.latch:                                     ; preds = %steady.inner.header
; CHECK-NEXT:   store i32 %acc.next.steady, ptr %c.ptr.steady, align 4
; CHECK-NEXT:   %i.next.steady = add i32 %i.steady, 1
; CHECK-NEXT:   %outer.cond.steady = icmp slt i32 %i.next.steady, %outer.trip.adj
; CHECK-NEXT:   %v0.steady.epi = load i32, ptr %a.ptr.next.steady, align 4
; CHECK-NEXT:   br i1 %outer.cond.steady, label %steady.header, label %lastiter.prologue, !llvm.loop !2
;
; CHECK: lastiter.prologue:                                ; preds = %steady.latch
; CHECK-NEXT:   call void @llvm.set.loop.iterations.i32(i32 %M)
; CHECK-NEXT:   br label %steady.inner.header.lastiter
;
; CHECK: steady.inner.header.lastiter:                     ; preds = %lastiter.prologue, %steady.inner.header.lastiter
; CHECK-NEXT:   %acc.steady.lastiter = phi i32 [ 0, %lastiter.prologue ], [ %acc.next.steady.lastiter, %steady.inner.header.lastiter ]
; CHECK-NEXT:   %acc.next.steady.lastiter = add i32 %acc.steady.lastiter, %v0.steady.epi
; CHECK-NEXT:   %inner.cond.steady.lastiter = call i1 @llvm.loop.decrement.i32(i32 1)
; CHECK-NEXT:   br i1 %inner.cond.steady.lastiter, label %steady.inner.header.lastiter, label %lastiter.epilogue, !llvm.loop !0
;
; CHECK: lastiter.epilogue:                                ; preds = %steady.inner.header.lastiter
; CHECK-NEXT:   store i32 %acc.next.steady.lastiter, ptr %c.ptr.next.steady, align 4
; CHECK-NEXT:   br label %exit
;
; CHECK: exit:                                             ; preds = %lastiter.epilogue, %entry
; CHECK-NEXT:   %acc.lcssa = phi i32 [ 0, %entry ], [ %acc.next.steady.lastiter, %lastiter.epilogue ]
; CHECK-NEXT:   ret i32 %acc.lcssa
; CHECK: }

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
  ; Live-out shared with the entry-bypass edge: LCSSA phi, no exit.loopexit.
  %acc.lcssa = phi i32 [ 0, %entry ], [ %acc.next, %outer.latch ]
  ret i32 %acc.lcssa
}

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

!0 = distinct !{!0, !2, !3}
!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.mustprogress"}
!3 = !{!"llvm.loop.itercount.range", i32 2}
