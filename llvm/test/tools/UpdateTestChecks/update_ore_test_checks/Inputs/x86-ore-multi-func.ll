; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates

; RUN: llc -mtriple=x86_64-linux-gnu -pass-remarks-output=- -pass-remarks-filter=asm-printer %s -o /dev/null | FileCheck %s

; Multi-function fixture: two functions in one file. Verifies that the
; script groups remark CHECK lines per-function rather than spilling all
; remarks under the first function.

define i32 @add(i32 %a, i32 %b) {
  %c = add i32 %a, %b
  ret i32 %c
}

define i32 @sub(i32 %a, i32 %b) {
  %c = sub i32 %a, %b
  ret i32 %c
}
