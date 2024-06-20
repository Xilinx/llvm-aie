;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -march=aie2 -O0 %s --filetype=obj -o %t.o
; RUN: llvm-objdump -dr --triple=aie2 %t.o | FileCheck --check-prefix=CHECK %s

; CHECK-LABEL: f0
; CHECK: movxm
; CHECK-NEXT: R_AIE_42 f
; CHECK: movxm
; CHECK-NEXT: R_AIE_42 pointer

@pointer = global ptr null, align 4

define void @f0() {
entry:
  store ptr @f, ptr @pointer, align 4
  ret void
}

declare void @f()
