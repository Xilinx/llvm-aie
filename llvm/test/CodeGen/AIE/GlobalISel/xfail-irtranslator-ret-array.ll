;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: not --crash llc -mtriple=aie -stop-after=irtranslator -global-isel -verify-machineinstrs %s -o - 2>&1 | FileCheck %s

; Types larger than 4x32 bits should be returned indirectly already in the IR,
; and we should only have to handle return types that fit in registers.

define [5 x i32] @test_ret_array() {
  ; CHECK: LLVM ERROR: unable to pre-lower return type: ptr (in function: test_ret_array)
  ret [5 x i32] zeroinitializer
}
