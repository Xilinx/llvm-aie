; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; Test that outer loop type selection can be driven by per-loop metadata
; overrides (llvm.loop.hint.aie-outer-loop-type) with string enum values.
;
; Metadata-driven hw mode: !"hw" in loop metadata forces JNZD conversion
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=false \
; RUN:     -stop-after=aie-outer-loop-pipeliner -o - %s \
; RUN:     | FileCheck %s --check-prefix=HW
;
; Metadata-driven soft mode: !"soft" in loop metadata forces software loop
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=false \
; RUN:     -stop-after=aie-outer-loop-pipeliner -o - %s \
; RUN:     | FileCheck %s --check-prefix=SOFT

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

; --- Test 1: metadata requests hw loop ---
; HW-LABEL: define void @metadata_hw_loop(
; HW:       stage0.top:
; HW:       %outer.ctr.init = call i32 @llvm.start.loop.iterations.i32
; HW:       steady.stage1.top:
; HW:       %outer.ctr = phi i32
; HW:       steady.stage1.bottom.and.stage0.top:
; HW:       %outer.ctr.next = call i32 @llvm.loop.decrement.reg.i32
; HW:       %outer.loop.cond = icmp ne i32 %outer.ctr.next, 0

define void @metadata_hw_loop(ptr %a, ptr %c, i32 %n, i32 %m) {
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
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !4

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

; --- Test 2: metadata requests soft loop ---
; SOFT-LABEL: define void @metadata_soft_loop(
; SOFT:       steady.stage1.bottom.and.stage0.top:
; SOFT:       %iv.next{{.*}} = add i32 %iv{{.*}}, -1
; SOFT:       icmp eq i32 %iv.next
; SOFT-NOT:   outer.ctr
; SOFT-NOT:   outer.jnzd
; SOFT:       exit:

define void @metadata_soft_loop(ptr %a, ptr %c, i32 %n, i32 %m) {
entry:
  %has.work = icmp ugt i32 %n, 1
  br i1 %has.work, label %outer.header, label %exit

outer.header:
  %iv = phi i32 [ %n, %entry ], [ %iv.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  %loaded = load i32, ptr %a.ptr, align 4
  call void @llvm.set.loop.iterations.i32(i32 %m)
  br label %inner.header2

inner.header2:
  %result2 = phi i32 [ 0, %outer.header ], [ %result2.next, %inner.header2 ]
  %result2.next = add i32 %result2, %loaded
  %inner.cond2 = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond2, label %inner.header2, label %outer.latch, !llvm.loop !9

outer.latch:
  store i32 %result2.next, ptr %c.ptr, align 4
  %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 1
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %outer.header, !llvm.loop !5

exit:
  ret void
}

; Outer loop metadata for hw mode: enable pipelining + request hw loop type
!0 = distinct !{!0, !1, !2, !3, !10}
!1 = !{!"llvm.loop.mustprogress"}
!2 = !{!"llvm.loop.itercount.range", i32 2}
!3 = !{!"llvm.loop.hint.aie-enable-outer-loop-pipelining", i64 1}
!4 = distinct !{!4, !1}

; Outer loop metadata for soft mode: enable pipelining + request soft loop type
!5 = distinct !{!5, !6, !7, !8, !11}
!6 = !{!"llvm.loop.mustprogress"}
!7 = !{!"llvm.loop.itercount.range", i32 2}
!8 = !{!"llvm.loop.hint.aie-enable-outer-loop-pipelining", i64 1}
!9 = distinct !{!9, !6}

; The key metadata entries: string enum values for outer-loop-type
!10 = !{!"llvm.loop.hint.aie-outer-loop-type", !"hw"}
!11 = !{!"llvm.loop.hint.aie-outer-loop-type", !"soft"}
