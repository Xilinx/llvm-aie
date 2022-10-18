;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie2 -O0 --global-isel %s -o - 2>&1 | FileCheck %s

; This could not be compiled using -O0's fast register allocator when CSE is enabled,
; because it cannot rematerialize operands using larger register classes, thus ended up
; with two operands both being constrained to r27

; CHECK-NOT: ran out of registers during register allocation

define internal i64 @test(i32 %0) {
entry:
  %sh_prom180 = zext i32 %0 to i64
  %shl181 = shl i64 0, %sh_prom180
  store i64 %shl181, ptr null, align 8
  ret i64 0
}
