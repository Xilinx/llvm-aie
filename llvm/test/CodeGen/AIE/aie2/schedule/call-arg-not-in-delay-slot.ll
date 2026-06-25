; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; RUN: llc -O2 -mtriple=aie2 %s -o - | FileCheck %s

; Regression test: a load that defines a call's argument register must not be
; scheduled into that call's delay slots. AIE2 is an in-order exposed-pipeline
; machine with no interlocks; a scalar load has a multi-cycle def latency while
; a jl has only 5 delay slots, so a load in the call's delay shadow leaves the
; argument register stale when the callee reads it (a silent miscompile).
;
; The int->float->int kernel below calls the soft-float helper __floatsisf,
; whose integer argument is in r1. Verify the r1-defining load is emitted before
; the call, not in its delay slots.

; CHECK-LABEL: add_one:
; CHECK:       lda r1,
; CHECK:       jl #__floatsisf

define void @add_one(ptr %input, ptr %output, i32 %tile_size) {
entry:
  %cmp6 = icmp sgt i32 %tile_size, 0
  br i1 %cmp6, label %for.body, label %for.cond.cleanup

for.cond.cleanup:
  ret void

for.body:
  %i.07 = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %arrayidx = getelementptr inbounds i32, ptr %input, i32 %i.07
  %0 = load i32, ptr %arrayidx, align 4
  %conv = sitofp i32 %0 to float
  %add = fadd float %conv, 1.000000e+00
  %conv1 = fptosi float %add to i32
  %arrayidx2 = getelementptr inbounds i32, ptr %output, i32 %i.07
  store i32 %conv1, ptr %arrayidx2, align 4
  %inc = add nuw nsw i32 %i.07, 1
  %exitcond.not = icmp eq i32 %inc, %tile_size
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body
}
