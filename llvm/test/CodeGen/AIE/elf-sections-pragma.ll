;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie %s --filetype=obj -o - | \
; RUN:   llvm-objdump -t --triple=aie  - | FileCheck %s --check-prefixes=AIE
; RUN: llc -mtriple=aie2 %s --filetype=obj -o - | \
; RUN:   llvm-objdump -t --triple=aie2 - | FileCheck %s --check-prefixes=AIE

; Similar to elf-sections.ll but with section names fixed through a
; #pragma clang section

; AIE-LABEL: SYMBOL TABLE:
; AIE: 00001000 l     O myBSS  00000020 internal_buf
; AIE: 00001000 l     O myData 00000020 internal_buf_init
; AIE: 00000000 g     O myBSS  00000400 buf_4_align
; AIE: 00000400 g     O myBSS  00000400 buf_16_align
; AIE: 00000800 g     O myBSS  00000400 buf2_16_align
; AIE: 00000c00 g     O myBSS  00000400 buf_64_align
; AIE: 00000000 g     O myData 00000400 buf_4_align_init
; AIE: 00000400 g     O myData 00000400 buf_16_align_init
; AIE: 00000800 g     O myData 00000400 buf2_16_align_init
; AIE: 00000c00 g     O myData 00000400 buf_64_align_init

@buf_4_align    = dso_local local_unnamed_addr global [256 x i32] zeroinitializer, align 4 #0
@buf_16_align   = dso_local local_unnamed_addr global [256 x i32] zeroinitializer, align 16 #0
@buf2_16_align  = dso_local local_unnamed_addr global [256 x i32] zeroinitializer, align 16 #0
@buf_64_align   = dso_local local_unnamed_addr global [256 x i32] zeroinitializer, align 64 #0
@internal_buf   = internal global [8 x i32] zeroinitializer, align 4 #0

@buf_4_align_init   = dso_local local_unnamed_addr global <{ i32, [255 x i32] }> <{ i32 1, [255 x i32] zeroinitializer }>, align 4 #0
@buf_16_align_init  = dso_local local_unnamed_addr global <{ i32, [255 x i32] }> <{ i32 1, [255 x i32] zeroinitializer }>, align 16 #0
@buf2_16_align_init = dso_local local_unnamed_addr global <{ i32, [255 x i32] }> <{ i32 1, [255 x i32] zeroinitializer }>, align 16 #0
@buf_64_align_init  = dso_local local_unnamed_addr global <{ i32, [255 x i32] }> <{ i32 1, [255 x i32] zeroinitializer }>, align 64 #0
@internal_buf_init  = internal global [8 x i32] [i32 0, i32 -1, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0], align 4 #0

attributes #0 = { "bss-section"="myBSS" "data-section"="myData" "relro-section"="myRelro" "rodata-section"="myRodata" }
