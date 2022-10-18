;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie2 -global-isel -verify-machineinstrs -o - < %s | FileCheck %s

; Check that the outgoing call frame is incorporated in the total stack frame size.
; Use an outrageous number of parameters, requiring 64 bytes in the callframe.
; The total stack frame, created/destroyed by padda, should include that amount
; plus some more for callee save spill/reload. Alignment padding will bring it to 96

define void @test() {
; CHECK-LABEL: test:
; CHECK:    paddb [sp], #96
; CHECK:    paddb [sp], #-96
entry:
  call void @f(i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 12, i32 13, i32 14, i32 15, i32 16, i32 17, i32 18, i32 19, i32 20, i32 21, i32 22, i32 23, i32 24)
  ret void
}

declare void @f(i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32)
