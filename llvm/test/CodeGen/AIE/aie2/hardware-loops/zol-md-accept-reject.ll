; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates

; RUN: llc -O2 -stop-after=hardware-loops -mtriple=aie2 --enable-aie-hardware-loops \
; RUN:    --enable-aie-zero-overhead-loops %s -o - | FileCheck %s

define void @simple_loop_accept_no_md(i32 noundef %n, ptr nocapture readonly %in, ptr nocapture writeonly %out) {
entry:
  %cmp4 = icmp sgt i32 %n, 0
  br i1 %cmp4, label %for.body, label %for.cond.cleanup

for.cond.cleanup:                                 ; preds = %for.body, %entry
  ret void

; CHECK-NOT: [[DEC:%[0-9]+]] = call i1 @llvm.loop.decrement.i32(i32 1)
for.body:                                         ; preds = %entry, %for.body
  %i.05 = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %0 = load i32, ptr %in, align 4
  %add = sub nsw i32 1, %i.05
  %sub = add i32 %add, %0
  %1 = trunc i32 %i.05 to i20
  %arrayidx = getelementptr inbounds i32, ptr %out, i20 %1
  store i32 %sub, ptr %arrayidx, align 4
  %inc = add nuw nsw i32 %i.05, 1
  %exitcond.not = icmp eq i32 %inc, %n
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body
}

define void @simple_loop_reject_md(i32 noundef %n, ptr nocapture readonly %in, ptr nocapture writeonly %out) {
entry:
  %cmp4 = icmp sgt i32 %n, 0
  br i1 %cmp4, label %for.body, label %for.cond.cleanup

for.cond.cleanup:                                 ; preds = %for.body, %entry
  ret void

; CHECK-NOT: [[DEC:%[0-9]+]] = call i1 @llvm.loop.decrement.i32(i32 1)
for.body:                                         ; preds = %entry, %for.body
  %i.05 = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %0 = load i32, ptr %in, align 4
  %add = sub nsw i32 1, %i.05
  %sub = add i32 %add, %0
  %1 = trunc i32 %i.05 to i20
  %arrayidx = getelementptr inbounds i32, ptr %out, i20 %1
  store i32 %sub, ptr %arrayidx, align 4
  %inc = add nuw nsw i32 %i.05, 1
  %exitcond.not = icmp eq i32 %inc, %n
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body, !llvm.loop !1
}

define void @simple_loop_accept_md(i32 noundef %n, ptr nocapture readonly %in, ptr nocapture writeonly %out) {
entry:
  %cmp4 = icmp sgt i32 %n, 0
  br i1 %cmp4, label %for.body, label %for.cond.cleanup

for.cond.cleanup:                                 ; preds = %for.body, %entry
  ret void

; CHECK: [[DEC:%[0-9]+]] = call i1 @llvm.loop.decrement.i32(i32 1)
for.body:                                         ; preds = %entry, %for.body
  %i.05 = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %0 = load i32, ptr %in, align 4
  %add = sub nsw i32 1, %i.05
  %sub = add i32 %add, %0
  %1 = trunc i32 %i.05 to i20
  %arrayidx = getelementptr inbounds i32, ptr %out, i20 %1
  store i32 %sub, ptr %arrayidx, align 4
  %inc = add nuw nsw i32 %i.05, 1
  %exitcond.not = icmp eq i32 %inc, %n
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body, !llvm.loop !3
}

!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.itercount.range", i64 1}
!3 = distinct !{!3, !4}
!4 = !{!"llvm.loop.itercount.range", i64 4}