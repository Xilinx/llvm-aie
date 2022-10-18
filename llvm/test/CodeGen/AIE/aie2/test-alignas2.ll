;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -O0 -verify-machineinstrs %s -mtriple=aie2 --filetype=obj -o %t
; RUN: llvm-readelf -st %t | FileCheck %s

; CHECK: Section Headers:
; CHECK:   [ {{[0-9]}}] .bss.align.512
; CHECK-NEXT:            NOBITS          00000000 {{000[02468ace]00}} 000004 00   0   0  512
; CHECK:   [ {{[0-9]}}] .data.align.1024
; CHECK-NEXT:            PROGBITS        00000000 {{000[048c]00}} 000004 00   0   0  1024

; CHECK: Symbol table '.symtab' contains 4 entries:
; CHECK:      2: 00000000     4 OBJECT  GLOBAL DEFAULT     3 n1
; CHECK:      3: 00000000     4 OBJECT  GLOBAL DEFAULT     4 n2

@n1 = global i32 0, align 512
@n2 = global i32 5, align 1024
