; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=false \
; RUN:     -aie-enable-outer-loop-pipelining \
; RUN:     -stop-after=aie-outer-loop-pipeliner -o - %s | FileCheck %s

; The peeled last iteration drops every latch load, because a prefetch for an
; iteration that never runs is dead. That only holds when nothing outliving the
; peel reads the loaded value, so check that such a load is kept: the clone of
; a reader would otherwise keep pointing at the original load, which collapses
; to poison once the original loop is deleted.

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

; A latch load consumed by another latch instruction, which is cloned into the
; peeled bottom and therefore needs the value.
define void @latch_load_with_latch_user(ptr noalias %a, ptr noalias %c, i32 %n,
                                        i32 %m) {
; CHECK-LABEL: define void @latch_load_with_latch_user(
; CHECK:       lastiter.stage1.bottom:
; CHECK:         %[[LOAD:.*]] = load i32, ptr %a.ptr.next.lastiter
; CHECK-NEXT:    %latch.use.lastiter = add i32 %[[LOAD]], %acc.next.lastiter
; CHECK-NEXT:    store i32 %latch.use.lastiter, ptr %c.ptr.next.steady
entry:
  %has.work = icmp sgt i32 %n, 1
  br i1 %has.work, label %outer.header, label %exit

outer.header:
  %i = phi i32 [ 0, %entry ], [ %i.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  %hdr = load i32, ptr %a.ptr, align 4
  call void @llvm.set.loop.iterations.i32(i32 %m)
  br label %inner.header

inner.header:
  %acc = phi i32 [ 0, %outer.header ], [ %acc.next, %inner.header ]
  %acc.next = add i32 %acc, %hdr
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 1
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  %latch.load = load i32, ptr %a.ptr.next, align 4
  %latch.use = add i32 %latch.load, %acc.next
  store i32 %latch.use, ptr %c.ptr, align 4
  %i.next = add nuw i32 %i, 1
  %outer.cond = icmp eq i32 %i.next, %n
  br i1 %outer.cond, label %exit, label %outer.header, !llvm.loop !0

exit:
  ret void
}

; A latch load read after the loop. The exit PHI is repointed at the peeled
; bottom, so the last iteration has to produce the value.
define void @latch_load_live_out(ptr noalias %a, ptr noalias %c, i32 %n,
                                 i32 %m) {
; CHECK-LABEL: define void @latch_load_live_out(
; CHECK:       lastiter.stage1.bottom:
; CHECK:         %[[LOAD:.*]] = load i32, ptr %a.ptr.next.lastiter
; CHECK:       exit:
; CHECK:         %out = phi i32 [ 0, %entry ], [ %[[LOAD]], %lastiter.stage1.bottom ]
entry:
  %has.work = icmp sgt i32 %n, 1
  br i1 %has.work, label %outer.header, label %exit

outer.header:
  %i = phi i32 [ 0, %entry ], [ %i.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  %hdr = load i32, ptr %a.ptr, align 4
  call void @llvm.set.loop.iterations.i32(i32 %m)
  br label %inner.header

inner.header:
  %acc = phi i32 [ 0, %outer.header ], [ %acc.next, %inner.header ]
  %acc.next = add i32 %acc, %hdr
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 1
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  %latch.load = load i32, ptr %a.ptr.next, align 4
  store i32 %acc.next, ptr %c.ptr, align 4
  %i.next = add nuw i32 %i, 1
  %outer.cond = icmp eq i32 %i.next, %n
  br i1 %outer.cond, label %exit, label %outer.header, !llvm.loop !0

exit:
  %out = phi i32 [ 0, %entry ], [ %latch.load, %outer.latch ]
  store i32 %out, ptr %c, align 4
  ret void
}

!0 = distinct !{!0, !2, !3}
!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.mustprogress"}
!3 = !{!"llvm.loop.itercount.range", i32 2}
