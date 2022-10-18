;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -O0 -verify-machineinstrs %s -mtriple=aie2 --filetype=obj -o %t
; RUN: llvm-readelf -st %t | FileCheck %s

; CHECK: Section Headers:
; CHECK:   [ {{[0-9]}}] .data.align.256
; CHECK-NEXT:            PROGBITS        00000000 {{000[0-9a-f]00}} 000100 00   0   0  256
; CHECK:   [ {{[0-9]}}] .bss.align.512
; CHECK-NEXT:            NOBITS          00000000 {{000[02468ace]00}} 000200 00   0   0  512

; CHECK: Symbol table '.symtab' contains 4 entries:
; CHECK:     2: 00000000   256 OBJECT  GLOBAL DEFAULT     3 s1
; CHECK:     3: 00000000   512 OBJECT  GLOBAL DEFAULT     4 s2


%struct.S1 = type { i32, [252 x i8] }
%struct.S2 = type { i32, [508 x i8] }

@s1 = dso_local global %struct.S1 { i32 5, [252 x i8] undef }, align 256
@s2 = dso_local global %struct.S2 zeroinitializer, align 512
