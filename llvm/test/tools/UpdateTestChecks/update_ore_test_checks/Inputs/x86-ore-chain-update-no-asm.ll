; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates

; RUN: llc -mtriple=x86_64-linux-gnu -pass-remarks-output=- -pass-remarks-filter=asm-printer %s -o /dev/null | FileCheck %s

; Regression fixture: a remark-only test (no asm RUN line) updated with
; --update-llc-checks. The asm-update step has nothing to regenerate and must
; be skipped, rather than stripping the remark RUN line and handing the sibling
; update_llc_test_checks.py a file with no RUN lines (which crashed with
; UnboundLocalError). Only REMARKS-style CHECK lines should be produced.

define i32 @add(i32 %a, i32 %b) {
  %c = add i32 %a, %b
  ret i32 %c
}
