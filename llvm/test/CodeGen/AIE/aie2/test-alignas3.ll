;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -O0 -verify-machineinstrs %s -mtriple=aie2 --filetype=obj -o %t
; RUN: llvm-readelf -st %t | FileCheck %s

; CHECK: Section Headers:
; CHECK:   [ {{[0-9]}}] .data.align.4
; CHECK-NEXT:            PROGBITS        00000000 {{0000[0-9a-f][048c]}} 000040 00   0   0  4
; CHECK:   [ {{[0-9]}}] .bss.align.4
; CHECK-NEXT:            NOBITS          00000000 {{0000[0-9a-f][048c]}} 000040 00   0   0  4

; CHECK: Symbol table '.symtab' contains 4 entries:
; CHECK:     2: 00000000    64 OBJECT  GLOBAL DEFAULT     3 lcpPing
; CHECK:     3: 00000000    64 OBJECT  GLOBAL DEFAULT     4 lcpPing1

@lcpPing = global <{ [8 x i32], [8 x i32] }> <{ [8 x i32] [i32 65535, i32 65535, i32 65535, i32 65535, i32 65535, i32 65535, i32 65535, i32 65535], [8 x i32] zeroinitializer }>, align 4
@lcpPing1 = global [16 x i32] zeroinitializer, align 4
