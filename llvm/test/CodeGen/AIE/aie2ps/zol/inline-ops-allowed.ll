;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -O2 --mtriple=aie2ps %s -o - --stop-before=irtranslator | \
; RUN:   FileCheck %s
; RUN: llc -O2 --mtriple=aie2ps %s -o - --stop-after=legalizer | \
; RUN:   FileCheck %s --check-prefix=LEGAL

; Operations the shared ZOL filter rejects because they are libcalls on other
; targets, but which AIE2PS lowers inline: scalar f32 add/sub/neg and integer
; division by a constant power of two, or its negation for sdiv.
; A loop body made only of them must become a zero-overhead loop (CHECK run:
; the hallmark intrinsics), and its legalized body must hold no call (LEGAL
; run: PseudoJL is the AIE call opcode used for libcalls).
;
; The neighbours that still lower to libcalls must keep failing the filter;
; they come first so that every CHECK-NOT region ends at a later label.

; srem by a power of two is not expanded and lowers to __modsi3.
; CHECK-LABEL: srem_pow2
; CHECK-NOT: call void @llvm.set.loop.iterations
; LEGAL-LABEL: name: srem_pow2
; LEGAL: PseudoJL &__modsi3
define dso_local i32 @srem_pow2(i32 noundef %n) local_unnamed_addr {
entry:
  br label %for.body

for.body:
  %i = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %a = phi i32 [ 1000, %entry ], [ %r, %for.body ]
  %r = srem i32 %a, 8
  %inc = add nuw nsw i32 %i, 1
  %exitcond.not = icmp eq i32 %inc, %n
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body, !llvm.loop !0

for.cond.cleanup:
  ret i32 %r
}

; sdiv by a constant that is not a power of two lowers to __divsi3.
; CHECK-LABEL: sdiv_const
; CHECK-NOT: call void @llvm.set.loop.iterations
; LEGAL-LABEL: name: sdiv_const
; LEGAL: PseudoJL &__divsi3
define dso_local i32 @sdiv_const(i32 noundef %n) local_unnamed_addr {
entry:
  br label %for.body

for.body:
  %i = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %a = phi i32 [ 1000, %entry ], [ %d, %for.body ]
  %d = sdiv i32 %a, 7
  %inc = add nuw nsw i32 %i, 1
  %exitcond.not = icmp eq i32 %inc, %n
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body, !llvm.loop !0

for.cond.cleanup:
  ret i32 %d
}

; f32 compares lower to __*sf2 libcalls.
; CHECK-LABEL: f32_fcmp
; CHECK-NOT: call void @llvm.set.loop.iterations
; LEGAL-LABEL: name: f32_fcmp
; LEGAL: PseudoJL &__ltsf2
define dso_local float @f32_fcmp(i32 noundef %n, float %y) local_unnamed_addr {
entry:
  br label %for.body

for.body:
  %i = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %a = phi float [ 0.000000e+00, %entry ], [ %s, %for.body ]
  %c = fcmp olt float %a, %y
  %s = select i1 %c, float %y, float %a
  %inc = add nuw nsw i32 %i, 1
  %exitcond.not = icmp eq i32 %inc, %n
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body, !llvm.loop !0

for.cond.cleanup:
  ret float %s
}

; CHECK-LABEL: f32_fadd
; CHECK: call void @llvm.set.loop.iterations
; CHECK: call i1 @llvm.loop.decrement
; LEGAL-LABEL: name: f32_fadd
; LEGAL-NOT: PseudoJL
define dso_local float @f32_fadd(i32 noundef %n) local_unnamed_addr {
entry:
  br label %for.body

for.body:
  %i = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %a = phi float [ 0.000000e+00, %entry ], [ %add, %for.body ]
  %add = fadd float %a, 1.000000e+00
  %inc = add nuw nsw i32 %i, 1
  %exitcond.not = icmp eq i32 %inc, %n
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body, !llvm.loop !0

for.cond.cleanup:
  ret float %add
}

; CHECK-LABEL: f32_fsub
; CHECK: call void @llvm.set.loop.iterations
; CHECK: call i1 @llvm.loop.decrement
; LEGAL-LABEL: name: f32_fsub
; LEGAL-NOT: PseudoJL
define dso_local float @f32_fsub(i32 noundef %n) local_unnamed_addr {
entry:
  br label %for.body

for.body:
  %i = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %a = phi float [ 1.000000e+00, %entry ], [ %sub, %for.body ]
  %sub = fsub float %a, 1.000000e+00
  %inc = add nuw nsw i32 %i, 1
  %exitcond.not = icmp eq i32 %inc, %n
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body, !llvm.loop !0

for.cond.cleanup:
  ret float %sub
}

; CHECK-LABEL: f32_fneg
; CHECK: call void @llvm.set.loop.iterations
; CHECK: call i1 @llvm.loop.decrement
; LEGAL-LABEL: name: f32_fneg
; LEGAL-NOT: PseudoJL
define dso_local void @f32_fneg(ptr noalias %p, ptr noalias %q, i32 noundef %n) local_unnamed_addr {
entry:
  br label %for.body

for.body:
  %i = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %pp = getelementptr inbounds float, ptr %p, i32 %i
  %x = load float, ptr %pp, align 4
  %neg = fneg float %x
  %qq = getelementptr inbounds float, ptr %q, i32 %i
  store float %neg, ptr %qq, align 4
  %inc = add nuw nsw i32 %i, 1
  %exitcond.not = icmp eq i32 %inc, %n
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body, !llvm.loop !0

for.cond.cleanup:
  ret void
}

; CHECK-LABEL: sdiv_pow2
; CHECK: call void @llvm.set.loop.iterations
; CHECK: call i1 @llvm.loop.decrement
; LEGAL-LABEL: name: sdiv_pow2
; LEGAL-NOT: PseudoJL
define dso_local i32 @sdiv_pow2(i32 noundef %n) local_unnamed_addr {
entry:
  br label %for.body

for.body:
  %i = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %a = phi i32 [ 1000, %entry ], [ %d, %for.body ]
  %d = sdiv i32 %a, 4
  %inc = add nuw nsw i32 %i, 1
  %exitcond.not = icmp eq i32 %inc, %n
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body, !llvm.loop !0

for.cond.cleanup:
  ret i32 %d
}

; CHECK-LABEL: sdiv_neg_pow2
; CHECK: call void @llvm.set.loop.iterations
; CHECK: call i1 @llvm.loop.decrement
; LEGAL-LABEL: name: sdiv_neg_pow2
; LEGAL-NOT: PseudoJL
define dso_local i32 @sdiv_neg_pow2(i32 noundef %n) local_unnamed_addr {
entry:
  br label %for.body

for.body:
  %i = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %a = phi i32 [ 1000, %entry ], [ %d, %for.body ]
  %d = sdiv i32 %a, -8
  %inc = add nuw nsw i32 %i, 1
  %exitcond.not = icmp eq i32 %inc, %n
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body, !llvm.loop !0

for.cond.cleanup:
  ret i32 %d
}

; udiv by a power of two is normally an lshr by the time it reaches codegen,
; but the combiner expands it as well.
; CHECK-LABEL: udiv_pow2
; CHECK: call void @llvm.set.loop.iterations
; CHECK: call i1 @llvm.loop.decrement
; LEGAL-LABEL: name: udiv_pow2
; LEGAL-NOT: PseudoJL
define dso_local i32 @udiv_pow2(i32 noundef %n) local_unnamed_addr {
entry:
  br label %for.body

for.body:
  %i = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %a = phi i32 [ 1000, %entry ], [ %d, %for.body ]
  %d = udiv i32 %a, 16
  %inc = add nuw nsw i32 %i, 1
  %exitcond.not = icmp eq i32 %inc, %n
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body, !llvm.loop !0

for.cond.cleanup:
  ret i32 %d
}

!0 = distinct !{!0, !1, !2}
!1 = !{!"llvm.loop.mustprogress"}
!2 = !{!"llvm.loop.itercount.range", i64 10}
