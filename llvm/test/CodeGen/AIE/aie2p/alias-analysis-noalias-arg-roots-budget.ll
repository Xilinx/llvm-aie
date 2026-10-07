;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; The arg-root walk visits each value once and gives no root once it has
; visited more than -aie-alias-analysis-max-root-nodes of them. Tracing %s3
; visits %s3, %s2, %s1, %s0, %pa and %a, so a limit of 6 is enough and a limit
; of 3 is not. Before the walk shared a visited set this chain of converging
; selects took 2^4 steps.
;
; RUN: opt -mtriple=aie2p -passes=aa-eval -aa-pipeline=aie-aa -print-all-alias-modref-info --aie-alias-analysis-noalias-arg-roots=true --aie-alias-analysis-max-root-nodes=6 -disable-output < %s 2>&1 | FileCheck %s --check-prefix=WIDE
; RUN: opt -mtriple=aie2p -passes=aa-eval -aa-pipeline=aie-aa -print-all-alias-modref-info --aie-alias-analysis-noalias-arg-roots=true --aie-alias-analysis-max-root-nodes=3 -disable-output < %s 2>&1 | FileCheck %s --check-prefix=NARROW

; WIDE-LABEL: Function: select_chain
; WIDE: NoAlias:{{.*}}%pb{{.*}}%s3

; NARROW-LABEL: Function: select_chain
; NARROW: MayAlias:{{.*}}%pb{{.*}}%s3

define void @select_chain(ptr noalias %a, ptr noalias %b, i1 %c) {
entry:
  %pa = load ptr, ptr %a, align 8
  %pb = load ptr, ptr %b, align 8
  %s0 = select i1 %c, ptr %pa, ptr %pa
  %s1 = select i1 %c, ptr %s0, ptr %s0
  %s2 = select i1 %c, ptr %s1, ptr %s1
  %s3 = select i1 %c, ptr %s2, ptr %s2
  store i32 0, ptr %pb, align 4
  load i32, ptr %s3, align 4
  ret void
}
