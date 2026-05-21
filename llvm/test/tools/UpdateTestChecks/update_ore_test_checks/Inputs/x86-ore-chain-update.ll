; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates

; RUN: llc -mtriple=x86_64-linux-gnu %s -o - | FileCheck %s --check-prefix ASM
; RUN: llc -mtriple=x86_64-linux-gnu -pass-remarks-output=- -pass-remarks-filter=asm-printer %s -o /dev/null | FileCheck %s --check-prefix REMARKS

; Chain-update fixture: starts with no CHECK lines at all. Exercises
; --update-llc-checks, which first invokes update_llc_test_checks.py to
; populate ASM CHECK lines, then adds REMARKS CHECK lines on top.

define i32 @add(i32 %a, i32 %b) {
  %c = add i32 %a, %b
  ret i32 %c
}
