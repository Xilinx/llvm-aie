;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -O2 -mtriple=aie --stop-after=prologepilog < %s | FileCheck -check-prefix=FPELIM %s
; RUN: llc -O2 -mtriple=aie --stop-after=prologepilog -frame-pointer=all < %s | FileCheck -check-prefix=WITHFP %s
; XFAIL: *

@error = dso_local local_unnamed_addr global i32 0, align 4
@_b = dso_local local_unnamed_addr global float* null, align 4

declare dso_local void @do_line(i32* inreg nocapture %line_start_address, float inreg %Im, float inreg %MinRe, float inreg %StepRe, float %cr, float %ci, i32 %limit)

; Function Attrs: mustprogress nofree noinline norecurse nosync nounwind
define dso_local void @_Z5juliaPiffffff(i32* inreg nocapture %framebuffer, float inreg %MinRe, float inreg %MinIm, float inreg %StepRe, float %StepIm, float %cr, float %ci) local_unnamed_addr {
; all stack pointers relative to current frame, since we reserved argument space when the function started.
; FPELIM-LABEL:  bb.2.for.body:
; FPELIM:   ST_SPIL_GPR undef renamable $r12, -4, implicit $sp
; FPELIM-NEXT:    ST_SPIL_GPR undef renamable $r12, -8, implicit $sp
; FPELIM-NEXT:    $p0 = COPY renamable $p6
; FPELIM-NEXT:    $r6 = COPY renamable $r10
; FPELIM-NEXT:    $r7 = COPY renamable $r4
; FPELIM-NEXT:    $r8 = LDA_SPIL_GPR -36, implicit $sp :: (load (s32) from %stack.1)
; FPELIM-NEXT:    JAL @do_line, csr_aie1, implicit-def dead $lr, implicit $p0, implicit $r6, implicit $r7, implicit $r8, implicit undef $r9, implicit-def $sp
;
; WITHFP-LABEL:  bb.2.for.body:
; WITHFP:         PADDA_sp_imm 32, implicit-def $sp, implicit $sp
; Stores here relative to new stack frame.
; WITHFP-NEXT:    ST_SPIL_GPR undef renamable $r12, -4, implicit $sp
; WITHFP-NEXT:    ST_SPIL_GPR undef renamable $r12, -8, implicit $sp
; WITHFP-NEXT:    $p0 = COPY renamable $p6
; WITHFP-NEXT:    $r6 = COPY renamable $r10
; WITHFP-NEXT:    $r7 = COPY renamable $r4
; Load here has to be relative to the old stack frame, since it comes from a spill.
; This is currently generated incorrectly.
; WITHFP-NEXT:    $r8 = LDA_SPIL_GPR -68, implicit $sp :: (load (s32) from %stack.1)
; WITHFP-NEXT:    JAL @do_line, csr_aie1, implicit-def dead $lr, implicit $p0, implicit $r6, implicit $r7, implicit $r8, implicit undef $r9, implicit-def $sp
; WITHFP-NEXT:    PADDA_sp_imm -32, implicit-def $sp, implicit $sp

entry:
  br label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  store i32 32, i32* %framebuffer, align 4, !tbaa !4
  ret void

for.body:                                         ; preds = %entry, %for.body
  %y.018 = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %Im.017 = phi float [ %MinIm, %entry ], [ %add2, %for.body ]
  %line_start_address.016 = phi i32* [ %framebuffer, %entry ], [ %add.ptr, %for.body ]
  tail call void @do_line(i32* inreg %line_start_address.016, float inreg %Im.017, float inreg %MinRe, float inreg %StepRe, float undef, float undef, i32 undef) #2
  %add.ptr = getelementptr inbounds i32, i32* %line_start_address.016, i20 32
  store i32 %y.018, i32* %add.ptr, align 4, !tbaa !4
  %0 = load float*, float** @_b, align 4, !tbaa !9
  %add = add nuw nsw i32 %y.018, 8
  %1 = trunc i32 %add to i20
  %arrayidx1 = getelementptr inbounds float, float* %0, i20 %1
  store float %Im.017, float* %arrayidx1, align 4, !tbaa !11
  %add2 = fadd float %Im.017, %StepIm
  %inc = add nuw nsw i32 %y.018, 1
  %exitcond.not = icmp eq i32 %inc, 32
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body, !llvm.loop !13
}



!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 14.0.0"}
!2 = distinct !{!2, !3}
!3 = !{!"llvm.loop.mustprogress"}
!4 = !{!5, !5, i64 0}
!5 = !{!"int", !6, i64 0}
!6 = !{!"omnipotent char", !7, i64 0}
!7 = !{!"Simple C++ TBAA"}
!8 = distinct !{!8, !3}
!9 = !{!10, !10, i64 0}
!10 = !{!"any pointer", !6, i64 0}
!11 = !{!12, !12, i64 0}
!12 = !{!"float", !6, i64 0}
!13 = distinct !{!13, !3}
