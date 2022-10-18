;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie2 -verify-machineinstrs -o - < %s | FileCheck %s --check-prefix=GISEL

target triple = "aie2"

define void @test() {
; GISEL-LABEL: test:
; GISEL:         .p2align 4
; GISEL-NEXT:  // %bb.0: // %entry
; GISEL-NEXT:    ret lr
entry:
  ret void
}
