;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; AIE target AA (AIEBaseAliasAnalysis): trace memory pointers to noalias
; function-argument roots through GEP/cast/phi/select and LoadInst.
;
; RUN: opt -mtriple=aie2p -passes=aa-eval -print-all-alias-modref-info --aie-alias-analysis-noalias-arg-roots=true -disable-output < %s 2>&1 | FileCheck %s --check-prefix=ON
; RUN: opt -mtriple=aie2p -passes=aa-eval -print-all-alias-modref-info --aie-alias-analysis-noalias-arg-roots=false -disable-output < %s 2>&1 | FileCheck %s --check-prefix=OFF

;--- io_buffer: inner data pointers loaded from distinct noalias args --------

; ON-LABEL: Function: distinct_noalias_arg_roots
; ON: NoAlias:{{.*}}%pa{{.*}},{{.*}}%pb

; OFF-LABEL: Function: distinct_noalias_arg_roots
; OFF: MayAlias:{{.*}}%pa{{.*}},{{.*}}%pb

define void @distinct_noalias_arg_roots(ptr noalias %buf_a, ptr noalias %buf_b) {
entry:
  %pa = load ptr, ptr %buf_a, align 8
  %pb = load ptr, ptr %buf_b, align 8
  %va = load i32, ptr %pa, align 4
  %vb = load i32, ptr %pb, align 4
  store i32 %va, ptr %pa, align 4
  store i32 %vb, ptr %pb, align 4
  ret void
}

; ON-LABEL: Function: gep_through_loaded_ptrs
; ON: NoAlias:{{.*}}%addr_in{{.*}},{{.*}}%addr_out

; OFF-LABEL: Function: gep_through_loaded_ptrs
; OFF: MayAlias:{{.*}}%addr_in{{.*}},{{.*}}%addr_out

define void @gep_through_loaded_ptrs(ptr noalias %in, ptr noalias %out) {
entry:
  %pIn = load ptr, ptr %in, align 8
  %pOut = load ptr, ptr %out, align 8
  %addr_in = getelementptr i16, ptr %pIn, i64 5
  %addr_out = getelementptr float, ptr %pOut, i64 5
  %v = load i16, ptr %addr_in, align 2
  store float 0.0, ptr %addr_out, align 4
  ret void
}

;--- GEP directly on noalias args (often handled without LoadInst look-through)

; ON-LABEL: Function: direct_gep_on_args
; ON: NoAlias:{{.*}}%addr_in{{.*}},{{.*}}%addr_out

; OFF-LABEL: Function: direct_gep_on_args
; OFF: NoAlias:{{.*}}%addr_in{{.*}},{{.*}}%addr_out

define void @direct_gep_on_args(ptr noalias %in, ptr noalias %out) {
entry:
  %addr_in = getelementptr i16, ptr %in, i64 5
  %addr_out = getelementptr float, ptr %out, i64 5
  %v = load i16, ptr %addr_in, align 2
  store float 0.0, ptr %addr_out, align 4
  ret void
}

;--- Same noalias arg root: must stay conservative (MayAlias) -----------------

; ON-LABEL: Function: same_noalias_arg_root
; ON: MayAlias:{{.*}}%pa{{.*}},{{.*}}%pb

; OFF-LABEL: Function: same_noalias_arg_root
; OFF: MayAlias:{{.*}}%pa{{.*}},{{.*}}%pb

define void @same_noalias_arg_root(ptr noalias %buf) {
entry:
  %pa = load ptr, ptr %buf, align 8
  %pb = load ptr, ptr %buf, align 8
  %va = load i32, ptr %pa, align 4
  store i32 %va, ptr %pb, align 4
  ret void
}

;--- Phi with a single noalias root on all incoming values --------------------

; ON-LABEL: Function: phi_unified_root
; ON: MayAlias:{{.*}}%p{{.*}},{{.*}}%p1

; OFF-LABEL: Function: phi_unified_root
; OFF: MayAlias:{{.*}}%p{{.*}},{{.*}}%p1

define void @phi_unified_root(ptr noalias %buf, i1 %cond) {
entry:
  %p0 = load ptr, ptr %buf, align 8
  br i1 %cond, label %t, label %f

t:
  br label %join

f:
  br label %join

join:
  %p = phi ptr [ %p0, %t ], [ %p0, %f ]
  %p1 = load ptr, ptr %buf, align 8
  load i32, ptr %p, align 4
  store i32 0, ptr %p1, align 4
  ret void
}

;--- Phi mixing two noalias args: cannot unify to one root; arg pair stays NoAlias

; ON-LABEL: Function: phi_mixed_roots
; ON: NoAlias:{{.*}}%a{{.*}},{{.*}}%b

; OFF-LABEL: Function: phi_mixed_roots
; OFF: NoAlias:{{.*}}%a{{.*}},{{.*}}%b
; OFF: MayAlias:{{.*}}%pa{{.*}},{{.*}}%pb

define void @phi_mixed_roots(ptr noalias %a, ptr noalias %b, i1 %cond) {
entry:
  %pa = load ptr, ptr %a, align 8
  %pb = load ptr, ptr %b, align 8
  br i1 %cond, label %t, label %f

t:
  br label %join

f:
  br label %join

join:
  %p = phi ptr [ %pa, %t ], [ %pb, %f ]
  load i32, ptr %pa, align 4
  store i32 0, ptr %pb, align 4
  ret void
}

;--- Select with different roots on the arms: direct %pa/%pb accesses stay NoAlias

; ON-LABEL: Function: select_mismatch_roots
; ON: NoAlias:{{.*}}%a{{.*}},{{.*}}%b

; OFF-LABEL: Function: select_mismatch_roots
; OFF: NoAlias:{{.*}}%a{{.*}},{{.*}}%b
; OFF: MayAlias:{{.*}}%pa{{.*}},{{.*}}%pb

define void @select_mismatch_roots(ptr noalias %a, ptr noalias %b, i1 %cond) {
entry:
  %pa = load ptr, ptr %a, align 8
  %pb = load ptr, ptr %b, align 8
  %p = select i1 %cond, ptr %pa, ptr %pb
  load i32, ptr %pa, align 4
  store i32 0, ptr %pb, align 4
  ret void
}

;--- Select arms agree: cross-access between %pa and %pb is NoAlias -----------

; ON-LABEL: Function: select_unified_cross
; ON: NoAlias:{{.*}}%p{{.*}},{{.*}}%pb

; OFF-LABEL: Function: select_unified_cross
; OFF: MayAlias:{{.*}}%p{{.*}},{{.*}}%pb

define void @select_unified_cross(ptr noalias %a, ptr noalias %b, i1 %cond) {
entry:
  %pa = load ptr, ptr %a, align 8
  %pb = load ptr, ptr %b, align 8
  %p = select i1 %cond, ptr %pa, ptr %pa
  load i32, ptr %p, align 4
  store i32 0, ptr %pb, align 4
  ret void
}

;--- Nested load chain through one noalias arg meta pointer -------------------

; ON-LABEL: Function: nested_load_chain
; ON: NoAlias:{{.*}}%q{{.*}},{{.*}}%x

; OFF-LABEL: Function: nested_load_chain
; OFF: MayAlias:{{.*}}%q{{.*}},{{.*}}%x

define void @nested_load_chain(ptr noalias %meta, ptr noalias %other) {
entry:
  %p = load ptr, ptr %meta, align 8
  %q = load ptr, ptr %p, align 8
  %x = load ptr, ptr %other, align 8
  load i32, ptr %q, align 4
  load i32, ptr %x, align 4
  ret void
}

;--- Args without noalias attribute: do not infer disjointness ----------------

; ON-LABEL: Function: plain_args_no_attr
; ON: MayAlias:{{.*}}%pa{{.*}},{{.*}}%pb

; OFF-LABEL: Function: plain_args_no_attr
; OFF: MayAlias:{{.*}}%pa{{.*}},{{.*}}%pb

define void @plain_args_no_attr(ptr %a, ptr %b) {
entry:
  %pa = load ptr, ptr %a, align 8
  %pb = load ptr, ptr %b, align 8
  load i32, ptr %pa, align 4
  store i32 0, ptr %pb, align 4
  ret void
}

; ON-LABEL: Function: config_copy_loop
; ON: NoAlias:{{.*}}%ip{{.*}},{{.*}}%op

; OFF-LABEL: Function: config_copy_loop
; OFF: MayAlias:{{.*}}%ip{{.*}},{{.*}}%op

define void @config_copy_loop(ptr noalias %input, ptr noalias %output) {
entry:
  %ip0 = load ptr, ptr %input, align 4
  %op0 = load ptr, ptr %output, align 4
  br label %for.body

for.body:
  %i = phi i32 [ 0, %entry ], [ %i.next, %for.body ]
  %ip = phi ptr [ %ip0, %entry ], [ %ip.next, %for.body ]
  %op = phi ptr [ %op0, %entry ], [ %op.next, %for.body ]
  %v = load <32 x i32>, ptr %ip, align 64
  store <32 x i32> %v, ptr %op, align 64
  %ip.next = getelementptr inbounds i8, ptr %ip, i20 128
  %op.next = getelementptr inbounds i8, ptr %op, i20 128
  %i.next = add i32 %i, 1
  %done = icmp eq i32 %i.next, 32
  br i1 %done, label %exit, label %for.body

exit:
  ret void
}
