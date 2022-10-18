;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie %s --filetype=obj -o - | \
; RUN:   llvm-readobj -r - | FileCheck %s --check-prefixes=AIE,AIE1
; RUN: llc -mtriple=aie2 %s --filetype=obj -o - | \
; RUN:   llvm-readobj -r - | FileCheck %s --check-prefixes=AIE,AIE2

; AIE:  Relocations [
; AIE:    Section (4) .rela.data.align.4 {
; AIE1:     0x0 R_AIE_72 x 0x0
; AIE2:     0x0 R_AIE_50 x 0x0
; AIE:    }
; AIE:  ]

@x = external dso_local global i32, align 4
@p = dso_local global i32* @x, align 4
