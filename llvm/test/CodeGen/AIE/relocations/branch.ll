;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie -O0 %s --filetype=obj -o %t.o
; RUN: llvm-objdump -dr %t.o | FileCheck --check-prefix=AIE1 %s

; RUN: llc -mtriple=aie2 -O0 %s --filetype=obj -o %t.o
; RUN: llvm-objdump -dr %t.o | FileCheck --check-prefix=AIE2 %s

; RUN: llc -mtriple=aie2p -O0 %s --filetype=obj -o %t.o
; RUN: llvm-objdump -dr %t.o | FileCheck --check-prefix=AIE2 %s

; Before linking, this should have a relocation with offset 64.
; AIE1: bnez {{.*}}, #0
; AIE1-NEXT: R_AIE_1 .LBB0_2
; AIE1: j #0
; AIE1-NEXT: R_AIE_1 .LBB0_1
; AIE1: j #0
; AIE1-NEXT: R_AIE_1 .LBB0_2

; AIE2: jnz {{.*}}, #0
; AIE2-NEXT: R_AIE_1 .LBB0_2
; AIE2: j #0
; AIE2-NEXT: R_AIE_1 .LBB0_1
; AIE2: j #0
; AIE2-NEXT: R_AIE_1 .LBB0_2

define void @foo(i32 %a, i32 %b, i1 %c) {
  %tst1 = icmp eq i32 %a, %b
  br i1 %tst1, label %end, label %test2

test2:
  br label %end

end:
  ret void
}
