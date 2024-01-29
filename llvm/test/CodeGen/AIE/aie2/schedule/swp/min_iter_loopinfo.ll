;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc --mtriple=aie2 -O2 --stop-after=pipeliner %s -o - | FileCheck %s

; We check that we have a three stage pipeline with unconditional controlflow in the prologue

; CHECK:  bb.4
; CHECK:   LDA
; CHECK:   PseudoJ_jump_imm %bb.5
; CHECK:  bb.5
; CHECK:   LDA
; CHECK:   PseudoJ_jump_imm %bb.6
; CHECK:  bb.6
; CHECK:   LDA
; CHECK:   PseudoJNZ {{.*}}, %bb.6

define dso_local i32 @f(ptr nocapture readonly %p, i32 noundef %n) {
entry:
  br label %for.body

for.body:                                         ; preds = %entry, %for.body
  %i.06 = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %s.05 = phi i32 [ %or, %for.body ], [ 0, %entry ]
  %p.addr.04 = phi ptr [ %incdec.ptr, %for.body ], [ %p, %entry ]
  %incdec.ptr = getelementptr inbounds i32, ptr %p.addr.04, i20 1
  %0 = load i32, ptr %p.addr.04, align 4, !tbaa !2
  %xor = xor i32 %0, 7
  %or = or i32 %xor, %s.05
  %inc = add nuw nsw i32 %i.06, 1
  %exitcond.not = icmp eq i32 %inc, %n
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body, !llvm.loop !6

for.cond.cleanup:                                 ; preds = %for.body, %entry
  ret i32 %or

}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 17.0.0 (git@gitenterprise.xilinx.com:XRLabs/llvm-aie.git c07f38f221ca4d3736152230e12f5cfc637a1d47)"}
!2 = !{!3, !3, i64 0}
!3 = !{!"int", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.itercount.range", i64 3}
