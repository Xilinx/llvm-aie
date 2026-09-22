;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -O0 -verify-machineinstrs -mtriple=aie2p %s -o - | FileCheck %s

; A stack frame larger than the sp_imm encodable range (+-2^18 bytes) must be
; materialized in a modifier register and applied with the register form of the
; SP pointer add, rather than triggering the "adjustSPReg cannot yet handle
; adjustments > +-2^18 bytes" fatal error.
define void @big_frame(ptr %out) {
; CHECK-LABEL: big_frame:
; CHECK: movxm [[M:m[0-9]+]], #300032
; CHECK: paddxm [sp], [[M]]
; CHECK: movxm [[M2:m[0-9]+]], #-300032
; CHECK: paddxm [sp], [[M2]]
  %buf = alloca [300000 x i8], align 64
  %p = getelementptr [300000 x i8], ptr %buf, i64 0, i64 0
  store volatile i8 42, ptr %p
  %v = load volatile i8, ptr %p
  store i8 %v, ptr %out
  ret void
}

; A frame within range still uses the compact sp_imm form (no regression, and the
; register fallback is only taken for large frames).
define void @small_frame(ptr %out) {
; CHECK-LABEL: small_frame:
; CHECK: paddxm [sp], #64
; CHECK-NOT: paddxm [sp], m
; CHECK: paddxm [sp], #-64
  %buf = alloca [64 x i8], align 64
  %p = getelementptr [64 x i8], ptr %buf, i64 0, i64 0
  store volatile i8 42, ptr %p
  %v = load volatile i8, ptr %p
  store i8 %v, ptr %out
  ret void
}
