;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -O0 -verify-machineinstrs -mtriple=aie2 %s -o - | FileCheck %s

; AIE2 has no register-form SP pointer add (only the sp_imm forms) and SP is
; reserved, so a stack frame larger than the sp_imm range (+-2^17 bytes) is
; adjusted by copying SP into a scratch pointer register, adding the materialized
; offset there, and copying back, rather than triggering the "adjustSPReg cannot
; yet handle adjustments > +-2^17 bytes" fatal error.
define void @big_frame(ptr %out) {
; CHECK-LABEL: big_frame:
; CHECK: mov [[P:p[0-9]+]], sp
; CHECK: movxm [[M:m[0-9]+]], #200000
; CHECK: padda {{\[}}[[P]]{{\]}}, [[M]]
; CHECK: mov sp, [[P]]
  %buf = alloca [200000 x i8], align 32
  %p = getelementptr [200000 x i8], ptr %buf, i64 0, i64 0
  store volatile i8 42, ptr %p
  %v = load volatile i8, ptr %p
  store i8 %v, ptr %out
  ret void
}

; A frame within range still uses the compact sp_imm form (no regression, and the
; scratch-register sequence is only used for large frames).
define void @small_frame(ptr %out) {
; CHECK-LABEL: small_frame:
; CHECK-NOT: mov sp, p
; CHECK: paddb [sp], #-32
  %buf = alloca [32 x i8], align 32
  %p = getelementptr [32 x i8], ptr %buf, i64 0, i64 0
  store volatile i8 42, ptr %p
  %v = load volatile i8, ptr %p
  store i8 %v, ptr %out
  ret void
}
