; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates

; RUN: llc -O2 -stop-after=hardware-loops -mtriple=aie2 \
; RUN:    --enable-aie-zero-overhead-loops %s -o - | FileCheck %s
; RUN: llc -O2 -stop-after=hardware-loops -mtriple=aie2p \
; RUN:    --enable-aie-zero-overhead-loops %s -o - | FileCheck %s
; RUN: llc -O2 -stop-after=hardware-loops -mtriple=aie2ps \
; RUN:    --enable-aie-zero-overhead-loops %s -o - | FileCheck %s

; Hardware-loop decisions for the declared minimum iteration count and for the
; per-loop hint llvm.loop.hint.enable-aie-hardware-loops. An accepted loop sets
; its counter in the preheader, behind the zero-trip guard.

; CHECK-LABEL: define void @no_md(
; CHECK:       for.body.preheader:
; CHECK-NEXT:    call void @llvm.set.loop.iterations.i32(i32 %n)
; CHECK:         call i1 @llvm.loop.decrement.i32(i32 1)
define void @no_md(i32 noundef %n, ptr nocapture readonly %in, ptr nocapture writeonly %out) {
entry:
  %guard = icmp sgt i32 %n, 0
  br i1 %guard, label %for.body, label %exit

for.body:
  %i = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %v = load i32, ptr %in, align 4
  %s = add i32 %v, %i
  %idx = trunc i32 %i to i20
  %p = getelementptr inbounds i32, ptr %out, i20 %idx
  store i32 %s, ptr %p, align 4
  %inc = add nuw nsw i32 %i, 1
  %done = icmp eq i32 %inc, %n
  br i1 %done, label %exit, label %for.body

exit:
  ret void
}

; CHECK-LABEL: define void @min_zero(
; CHECK:       for.body.preheader:
; CHECK-NEXT:    call void @llvm.set.loop.iterations.i32(i32 %n)
; CHECK:         call i1 @llvm.loop.decrement.i32(i32 1)
define void @min_zero(i32 noundef %n, ptr nocapture readonly %in, ptr nocapture writeonly %out) {
entry:
  %guard = icmp sgt i32 %n, 0
  br i1 %guard, label %for.body, label %exit

for.body:
  %i = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %v = load i32, ptr %in, align 4
  %s = add i32 %v, %i
  %idx = trunc i32 %i to i20
  %p = getelementptr inbounds i32, ptr %out, i20 %idx
  store i32 %s, ptr %p, align 4
  %inc = add nuw nsw i32 %i, 1
  %done = icmp eq i32 %inc, %n
  br i1 %done, label %exit, label %for.body, !llvm.loop !0

exit:
  ret void
}

; CHECK-LABEL: define void @min_zero_max_eight(
; CHECK:       for.body.preheader:
; CHECK-NEXT:    call void @llvm.set.loop.iterations.i32(i32 %n)
; CHECK:         call i1 @llvm.loop.decrement.i32(i32 1)
define void @min_zero_max_eight(i32 noundef %n, ptr nocapture readonly %in, ptr nocapture writeonly %out) {
entry:
  %guard = icmp sgt i32 %n, 0
  br i1 %guard, label %for.body, label %exit

for.body:
  %i = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %v = load i32, ptr %in, align 4
  %s = add i32 %v, %i
  %idx = trunc i32 %i to i20
  %p = getelementptr inbounds i32, ptr %out, i20 %idx
  store i32 %s, ptr %p, align 4
  %inc = add nuw nsw i32 %i, 1
  %done = icmp eq i32 %inc, %n
  br i1 %done, label %exit, label %for.body, !llvm.loop !2

exit:
  ret void
}

; CHECK-LABEL: define void @min_one(
; CHECK-NOT:     @llvm.set.loop.iterations
; CHECK-NOT:     @llvm.loop.decrement
; CHECK:         ret void
define void @min_one(i32 noundef %n, ptr nocapture readonly %in, ptr nocapture writeonly %out) {
entry:
  %guard = icmp sgt i32 %n, 0
  br i1 %guard, label %for.body, label %exit

for.body:
  %i = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %v = load i32, ptr %in, align 4
  %s = add i32 %v, %i
  %idx = trunc i32 %i to i20
  %p = getelementptr inbounds i32, ptr %out, i20 %idx
  store i32 %s, ptr %p, align 4
  %inc = add nuw nsw i32 %i, 1
  %done = icmp eq i32 %inc, %n
  br i1 %done, label %exit, label %for.body, !llvm.loop !4

exit:
  ret void
}

; CHECK-LABEL: define void @min_two(
; CHECK:       for.body.preheader:
; CHECK-NEXT:    call void @llvm.set.loop.iterations.i32(i32 %n)
; CHECK:         call i1 @llvm.loop.decrement.i32(i32 1)
define void @min_two(i32 noundef %n, ptr nocapture readonly %in, ptr nocapture writeonly %out) {
entry:
  %guard = icmp sgt i32 %n, 0
  br i1 %guard, label %for.body, label %exit

for.body:
  %i = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %v = load i32, ptr %in, align 4
  %s = add i32 %v, %i
  %idx = trunc i32 %i to i20
  %p = getelementptr inbounds i32, ptr %out, i20 %idx
  store i32 %s, ptr %p, align 4
  %inc = add nuw nsw i32 %i, 1
  %done = icmp eq i32 %inc, %n
  br i1 %done, label %exit, label %for.body, !llvm.loop !6

exit:
  ret void
}

; CHECK-LABEL: define void @min_four(
; CHECK:       for.body.preheader:
; CHECK-NEXT:    call void @llvm.set.loop.iterations.i32(i32 %n)
; CHECK:         call i1 @llvm.loop.decrement.i32(i32 1)
define void @min_four(i32 noundef %n, ptr nocapture readonly %in, ptr nocapture writeonly %out) {
entry:
  %guard = icmp sgt i32 %n, 0
  br i1 %guard, label %for.body, label %exit

for.body:
  %i = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %v = load i32, ptr %in, align 4
  %s = add i32 %v, %i
  %idx = trunc i32 %i to i20
  %p = getelementptr inbounds i32, ptr %out, i20 %idx
  store i32 %s, ptr %p, align 4
  %inc = add nuw nsw i32 %i, 1
  %done = icmp eq i32 %inc, %n
  br i1 %done, label %exit, label %for.body, !llvm.loop !8

exit:
  ret void
}

; CHECK-LABEL: define void @hint_off(
; CHECK-NOT:     @llvm.set.loop.iterations
; CHECK-NOT:     @llvm.loop.decrement
; CHECK:         ret void
define void @hint_off(i32 noundef %n, ptr nocapture readonly %in, ptr nocapture writeonly %out) {
entry:
  %guard = icmp sgt i32 %n, 0
  br i1 %guard, label %for.body, label %exit

for.body:
  %i = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %v = load i32, ptr %in, align 4
  %s = add i32 %v, %i
  %idx = trunc i32 %i to i20
  %p = getelementptr inbounds i32, ptr %out, i20 %idx
  store i32 %s, ptr %p, align 4
  %inc = add nuw nsw i32 %i, 1
  %done = icmp eq i32 %inc, %n
  br i1 %done, label %exit, label %for.body, !llvm.loop !10

exit:
  ret void
}

; CHECK-LABEL: define void @hint_off_min_four(
; CHECK-NOT:     @llvm.set.loop.iterations
; CHECK-NOT:     @llvm.loop.decrement
; CHECK:         ret void
define void @hint_off_min_four(i32 noundef %n, ptr nocapture readonly %in, ptr nocapture writeonly %out) {
entry:
  %guard = icmp sgt i32 %n, 0
  br i1 %guard, label %for.body, label %exit

for.body:
  %i = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %v = load i32, ptr %in, align 4
  %s = add i32 %v, %i
  %idx = trunc i32 %i to i20
  %p = getelementptr inbounds i32, ptr %out, i20 %idx
  store i32 %s, ptr %p, align 4
  %inc = add nuw nsw i32 %i, 1
  %done = icmp eq i32 %inc, %n
  br i1 %done, label %exit, label %for.body, !llvm.loop !12

exit:
  ret void
}

; CHECK-LABEL: define void @hint_on_min_one(
; CHECK-NOT:     @llvm.set.loop.iterations
; CHECK-NOT:     @llvm.loop.decrement
; CHECK:         ret void
define void @hint_on_min_one(i32 noundef %n, ptr nocapture readonly %in, ptr nocapture writeonly %out) {
entry:
  %guard = icmp sgt i32 %n, 0
  br i1 %guard, label %for.body, label %exit

for.body:
  %i = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %v = load i32, ptr %in, align 4
  %s = add i32 %v, %i
  %idx = trunc i32 %i to i20
  %p = getelementptr inbounds i32, ptr %out, i20 %idx
  store i32 %s, ptr %p, align 4
  %inc = add nuw nsw i32 %i, 1
  %done = icmp eq i32 %inc, %n
  br i1 %done, label %exit, label %for.body, !llvm.loop !14

exit:
  ret void
}

!0 = distinct !{!0, !1}
!1 = !{!"llvm.loop.itercount.range", i64 0}
!2 = distinct !{!2, !3}
!3 = !{!"llvm.loop.itercount.range", i64 0, i64 8}
!4 = distinct !{!4, !5}
!5 = !{!"llvm.loop.itercount.range", i64 1}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.itercount.range", i64 2}
!8 = distinct !{!8, !9}
!9 = !{!"llvm.loop.itercount.range", i64 4}
!10 = distinct !{!10, !11}
!11 = !{!"llvm.loop.hint.enable-aie-hardware-loops", i64 0}
!12 = distinct !{!12, !11, !9}
!13 = !{!"llvm.loop.hint.enable-aie-hardware-loops", i64 1}
!14 = distinct !{!14, !13, !5}
