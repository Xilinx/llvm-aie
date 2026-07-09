;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; AIE target AA (AIEBaseAliasAnalysis): the noalias-arg-root disambiguation can
; be enabled per function through the "aie-propagate-restrict" function
; attribute, independently of the module-wide command-line option. These RUN
; lines deliberately do NOT pass --aie-alias-analysis-noalias-arg-roots, so the
; behaviour is driven purely by the attribute. The analysis is target-generic
; across the AIE family, so a single set of CHECK lines covers all triples.
;
; RUN: opt -mtriple=aie2 -passes=aa-eval -print-all-alias-modref-info -disable-output < %s 2>&1 | FileCheck %s
; RUN: opt -mtriple=aie2p -passes=aa-eval -print-all-alias-modref-info -disable-output < %s 2>&1 | FileCheck %s
; RUN: opt -mtriple=aie2ps -passes=aa-eval -print-all-alias-modref-info -disable-output < %s 2>&1 | FileCheck %s

;--- Function opts in via the attribute: distinct noalias arg roots -> NoAlias

; CHECK-LABEL: Function: attr_enabled: 4 pointers, 0 call sites
; CHECK-NEXT:    NoAlias:	ptr* %buf_a, ptr* %buf_b
; CHECK-NEXT:    NoAlias:	ptr* %buf_a, i32* %pa
; CHECK-NEXT:    NoAlias:	ptr* %buf_b, i32* %pa
; CHECK-NEXT:    NoAlias:	ptr* %buf_a, i32* %pb
; CHECK-NEXT:    NoAlias:	ptr* %buf_b, i32* %pb
; CHECK-NEXT:    NoAlias:	i32* %pa, i32* %pb

define void @attr_enabled(ptr noalias %buf_a, ptr noalias %buf_b) #0 {
entry:
  %pa = load ptr, ptr %buf_a, align 8
  %pb = load ptr, ptr %buf_b, align 8
  %va = load i32, ptr %pa, align 4
  %vb = load i32, ptr %pb, align 4
  store i32 %va, ptr %pa, align 4
  store i32 %vb, ptr %pb, align 4
  ret void
}

;--- Function without the attribute: stays conservative (MayAlias) ------------

; CHECK-LABEL: Function: attr_disabled: 4 pointers, 0 call sites
; CHECK-NEXT:    NoAlias:	ptr* %buf_a, ptr* %buf_b
; CHECK-NEXT:    NoAlias:	ptr* %buf_a, i32* %pa
; CHECK-NEXT:    NoAlias:	ptr* %buf_b, i32* %pa
; CHECK-NEXT:    NoAlias:	ptr* %buf_a, i32* %pb
; CHECK-NEXT:    NoAlias:	ptr* %buf_b, i32* %pb
; CHECK-NEXT:    MayAlias:	i32* %pa, i32* %pb

define void @attr_disabled(ptr noalias %buf_a, ptr noalias %buf_b) {
entry:
  %pa = load ptr, ptr %buf_a, align 8
  %pb = load ptr, ptr %buf_b, align 8
  %va = load i32, ptr %pa, align 4
  %vb = load i32, ptr %pb, align 4
  store i32 %va, ptr %pa, align 4
  store i32 %vb, ptr %pb, align 4
  ret void
}

attributes #0 = { "aie-propagate-restrict" }
