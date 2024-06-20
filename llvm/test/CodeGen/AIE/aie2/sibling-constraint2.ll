;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -O2 --mtriple=aie2 --filetype=asm -o - %s | FileCheck %s

; Similar to sibling-constraint.ll, this could not be register allocated
; because R27 could not be moved to another GPR between the selects.
; We check that we can successfully register allocate all three selects

; CHECK: sel.eqz {{.*}}, {{.*}}, {{.*}}, r27
; CHECK: sel.nez {{.*}}, {{.*}}, {{.*}}, r27
; CHECK: sel.nez {{.*}}, {{.*}}, {{.*}}, r27

define void @test(i8 %a, i32 %b) {
entry:
  %cmp0 = icmp eq i32 %b, 0
  %sel0 = select i1 %cmp0, i8 1, i8 0
  store i8 %sel0, ptr null, align 32
  %cmp1 = icmp ugt i8 %a, 0
  %sel1 = select i1 %cmp0, i1 %cmp1, i1 false
  %sel2 = select i1 %sel1, i8 1, i8 0
  store i8 %sel2, ptr null, align 2
  %sel3 = select i1 %cmp1, i16 0, i16 1
  store i16 %sel3, ptr null, align 2
  ret void
}
