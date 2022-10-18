;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -O2 --mtriple=aie2 --filetype=asm -o - %s | FileCheck %s


; AIECC-211 was caused by the r27 constraint on the condition of a select
; instruction to be propagated to the data operand of another select, which
; could then not be allocated because r27 was required twice.
; %cmp occurs in this way in the two selects below.
; We just check that compilation succeeds and produces two properly
; constrained selects

; CHECK: sel.nez {{.*}}, {{.*}}, {{.*}}, r27
; CHECK: sel.nez {{.*}}, {{.*}}, {{.*}}, r27

define i1 @boolselect(i32 %a, i32 %b, i1 %c, i1 %d) {
entry:
  %cmp = icmp ne i32 %a, 0
  %cond = select i1 %cmp, i32 %b, i32 0
  tail call void @f(i32 %cond)
  %cond1 = select i1 %d, i1 %cmp, i1 %c
  ret i1 %cond1
}

declare void @f(i32)
