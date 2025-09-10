; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -O2 -mtriple=aie2 -stop-after=irtranslator %s -o - | FileCheck %s
; CHECK: llvm.set.loop.iterations
; CHECK: llvm.loop.decrement

define dso_local void @_Z3sqrPK8bfloat16PS_i(ptr noalias nocapture readonly %in, ptr noalias nocapture writeonly %out, i32 noundef %N) {
entry:
  tail call void @llvm.aie2.event(i32 0)
  %cmp9 = icmp sgt i32 %N, 0
  br i1 %cmp9, label %for.body, label %for.cond.cleanup

for.body:                                         ; preds = %entry, %for.body
  %i.010 = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %0 = trunc i32 %i.010 to i20
  %arrayidx = getelementptr inbounds bfloat, ptr %in, i20 %0
  %1 = load bfloat, ptr %arrayidx, align 2
  %unpromotion = fmul bfloat %1, %1
  %arrayidx3 = getelementptr inbounds bfloat, ptr %out, i20 %0
  store bfloat %unpromotion, ptr %arrayidx3, align 2
  %inc = add nuw nsw i32 %i.010, 1
  %exitcond.not = icmp eq i32 %inc, %N
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body

for.cond.cleanup:                                 ; preds = %for.body, %entry
  tail call void @llvm.aie2.event(i32 1)
  ret void
}

declare void @llvm.aie2.event(i32)

