;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie < %s

; A check for selection DAG.  In this case, the i8's need to be legalized
; by selection DAG and are transformed into i32's  However, there needs
; to be some pointer arithmetic, so the i32 needs to be truncated back to
; i20:

; t4: i8,ch = load<(dereferenceable load (s8) from %ir.A3, align 4)> t0, FrameIndex:i20<1>, undef:i20
;   t5: i20 = sign_extend t4
; t6: i20 = add FrameIndex:i20<1>, t5
; t7: i8,ch = load<(load (s8) from %ir.G1)> t0, t6, undef:i20

; Without modification, this hits an assert in selectionDAG when handling the sign_extend.
; We patch LLVM to allow truncation in this case.

define void @f() {
BB:
  %A4 = alloca i8*, align 8
  %A3 = alloca i8, align 1
  %L3 = load i8, i8* %A3, align 1
  %L2 = load i8, i8* %A3, align 1
  %G10 = getelementptr i8, i8* %A3, i8 %L3
  %G1 = getelementptr i8, i8* %A3, i8 %L2

  %L4 = load i8, i8* %G1, align 1
  %B17 = sub i8 %L3, %L4
  store i8 %B17, i8* %G10, align 1

  ret void
}
