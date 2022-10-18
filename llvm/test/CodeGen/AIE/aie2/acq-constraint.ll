;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie2 -O2 --filetype=asm %s -o - | FileCheck %s

; Similar to AIECC-211, acq in stead of select

; CHECK: acq.cond  {{.*}}, {{.*}}, r26
; CHECK: acq.cond  {{.*}}, {{.*}}, r26

define dso_local void @_Z1fiii(i32 %x, i32 %y, i32 %z) {
entry:
  %add = add i32 %z, 1
  tail call void @llvm.aie2.acquire.cond(i32 1, i32 %x, i32 %add)
  tail call void @llvm.aie2.acquire.cond(i32 %y, i32 %add, i32 %z)
  ret void
}

declare void @llvm.aie2.acquire.cond(i32, i32, i32)
