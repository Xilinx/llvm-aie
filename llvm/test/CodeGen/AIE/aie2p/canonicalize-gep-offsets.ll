; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -O2 -mtriple=aie2p -stop-after=aie-canonicalize-gep-offsets \
; RUN:   %s -o - 2>&1 | FileCheck %s --check-prefix=IR
; RUN: llc -O2 -mtriple=aie2p %s -o - | FileCheck %s --check-prefix=ASM

; Unrolled strided stores: the constant offsets are hidden inside the scalar
; index expressions (add/or of the induction variable with a constant).
; Canonicalization hoists the constants onto a shared base pointer so that the
; backend forms a post-increment store chain.
define dso_local void @hoist_offsets_out_of_index(ptr noalias writeonly captures(none) %out) {
; IR-LABEL:  define dso_local void @hoist_offsets_out_of_index(
; IR:          %[[BASE:.*]] = getelementptr inbounds nuw i8, ptr %out, i20 %iv
; IR-NEXT:     store <32 x bfloat> zeroinitializer, ptr %[[BASE]], align 64
; IR-NEXT:     %[[P1:.*]] = getelementptr i8, ptr %[[BASE]], i20 64
; IR-NEXT:     store <32 x bfloat> zeroinitializer, ptr %[[P1]], align 64
; IR-NEXT:     %[[P2:.*]] = getelementptr i8, ptr %[[BASE]], i20 128
; IR-NEXT:     store <32 x bfloat> zeroinitializer, ptr %[[P2]], align 64
; ASM-LABEL: hoist_offsets_out_of_index:
; ASM:         vst x0, [p1], #64
entry:
  br label %for.body

for.body:
  %iv = phi i20 [ 0, %entry ], [ %iv.next, %for.body ]
  %p0 = getelementptr inbounds nuw i8, ptr %out, i20 %iv
  %i1 = add nuw nsw i20 %iv, 64
  %p1 = getelementptr inbounds nuw i8, ptr %out, i20 %i1
  %i2 = add nuw nsw i20 %iv, 128
  %p2 = getelementptr inbounds nuw i8, ptr %out, i20 %i2
  store <32 x bfloat> zeroinitializer, ptr %p0, align 64
  store <32 x bfloat> zeroinitializer, ptr %p1, align 64
  store <32 x bfloat> zeroinitializer, ptr %p2, align 64
  %iv.next = add nuw nsw i20 %iv, 192
  %cmp = icmp ult i20 %iv, 31616
  br i1 %cmp, label %for.body, label %exit

exit:
  ret void
}

; or-disjoint index arithmetic (as produced by InstCombine) is split too.
define dso_local void @hoist_offsets_or_disjoint(ptr noalias writeonly captures(none) %out) {
; IR-LABEL:  define dso_local void @hoist_offsets_or_disjoint(
; IR:          %[[BASE:.*]] = getelementptr inbounds nuw i8, ptr %out, i20 %iv
; IR:          %[[P1:.*]] = getelementptr i8, ptr %[[BASE]], i20 64
; IR-NEXT:     store <32 x bfloat> zeroinitializer, ptr %[[P1]], align 64
entry:
  br label %for.body

for.body:
  %iv = phi i20 [ 0, %entry ], [ %iv.next, %for.body ]
  %p0 = getelementptr inbounds nuw i8, ptr %out, i20 %iv
  %i1 = or disjoint i20 %iv, 64
  %p1 = getelementptr inbounds nuw i8, ptr %out, i20 %i1
  store <32 x bfloat> zeroinitializer, ptr %p1, align 64
  %iv.next = add nuw nsw i20 %iv, 64
  %cmp = icmp ult i20 %iv, 31616
  br i1 %cmp, label %for.body, label %exit

exit:
  ret void
}

; Subtracted constants (negative offsets) are hoisted too.
define dso_local void @hoist_negative_offset(ptr noalias writeonly captures(none) %out) {
; IR-LABEL:  define dso_local void @hoist_negative_offset(
; IR:          %[[BASE:.*]] = getelementptr inbounds nuw i8, ptr %out, i20 %iv
; IR:          store <32 x bfloat> zeroinitializer, ptr %[[BASE]], align 64
; IR-NEXT:     %[[P1:.*]] = getelementptr i8, ptr %[[BASE]], i20 -64
; IR-NEXT:     store <32 x bfloat> zeroinitializer, ptr %[[P1]], align 64
entry:
  br label %for.body

for.body:
  %iv = phi i20 [ 0, %entry ], [ %iv.next, %for.body ]
  %p0 = getelementptr inbounds nuw i8, ptr %out, i20 %iv
  %i1 = sub nuw i20 %iv, 64
  %p1 = getelementptr inbounds nuw i8, ptr %out, i20 %i1
  store <32 x bfloat> zeroinitializer, ptr %p0, align 64
  store <32 x bfloat> zeroinitializer, ptr %p1, align 64
  %iv.next = add nuw nsw i20 %iv, 192
  %cmp = icmp ult i20 %iv, 31616
  br i1 %cmp, label %for.body, label %exit

exit:
  ret void
}

; A walk of pure const-/term-links feeding memory operations directly is the
; form the backend post-increment combiner wants — it must stay untouched:
; merging would create extra pointer copies.
define dso_local void @walk_chain_unchanged(ptr noalias writeonly captures(none) %out) {
; IR-LABEL:  define dso_local void @walk_chain_unchanged(
; IR-NEXT:   entry:
; IR-NEXT:     br label %for.body
; IR:        for.body:
; IR-NEXT:     %iv = phi i20 [ 0, %entry ], [ %iv.next, %for.body ]
; IR-NEXT:     %p0 = getelementptr inbounds nuw i8, ptr %out, i20 %iv
; IR-NEXT:     %p1 = getelementptr inbounds nuw i8, ptr %p0, i20 64
; IR-NEXT:     store <32 x bfloat> zeroinitializer, ptr %p0, align 64
; IR-NEXT:     store <32 x bfloat> zeroinitializer, ptr %p1, align 64
; IR-NEXT:     %iv.next = add nuw nsw i20 %iv, 128
entry:
  br label %for.body

for.body:
  %iv = phi i20 [ 0, %entry ], [ %iv.next, %for.body ]
  %p0 = getelementptr inbounds nuw i8, ptr %out, i20 %iv
  %p1 = getelementptr inbounds nuw i8, ptr %p0, i20 64
  store <32 x bfloat> zeroinitializer, ptr %p0, align 64
  store <32 x bfloat> zeroinitializer, ptr %p1, align 64
  %iv.next = add nuw nsw i20 %iv, 128
  %cmp = icmp ult i20 %iv, 31616
  br i1 %cmp, label %for.body, label %exit

exit:
  ret void
}

; A plain gep(base, iv) address is already canonical and stays untouched.
define dso_local void @normal_form_unchanged(ptr noalias writeonly captures(none) %out, i20 %iv) {
; IR-LABEL:  define dso_local void @normal_form_unchanged(
; IR-NEXT:   entry:
; IR-NEXT:     %p = getelementptr inbounds nuw i8, ptr %out, i20 %iv
; IR-NEXT:     store <32 x bfloat> zeroinitializer, ptr %p, align 64
; IR-NEXT:     ret void
entry:
  %p = getelementptr inbounds nuw i8, ptr %out, i20 %iv
  store <32 x bfloat> zeroinitializer, ptr %p, align 64
  ret void
}
