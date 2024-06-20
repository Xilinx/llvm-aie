;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie2 -O2 --filetype=asm %s -o - | FileCheck %s

; Similiar to AIECC-211, rel in stead of select

; CHECK: rel.cond  {{.*}}, {{.*}}, r26
; CHECK: rel.cond  {{.*}}, {{.*}}, r26

define dso_local void @f(i32 %x, i32 %y, i32 %z) {
entry:
  %add = add i32 %z, 1
  tail call void @llvm.aie2.release.cond(i32 1, i32 %x, i32 %add)
  tail call void @llvm.aie2.release.cond(i32 %y, i32 %add, i32 %z)
  ret void
}

declare void @llvm.aie2.release.cond(i32, i32, i32)
