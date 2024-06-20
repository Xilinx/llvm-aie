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

; Make sure symbols of different alignments go into different .data or .bss
; sections.

; AIE-LABEL: SYMBOL TABLE:
; AIE: 00000400 l     O .bss.align.4   00000020 internal_buf
; AIE: 00000400 l     O .data.align.4  00000020 internal_buf_init
; AIE: 00000000 g     O .bss.align.4   00000400 buf_4_align
; AIE: 00000000 g     O .bss.align.16  00000400 buf_16_align
; AIE: 00000400 g     O .bss.align.16  00000400 buf2_16_align
; AIE: 00000000 g     O .bss.align.64  00000400 buf_64_align
; AIE: 00000000 g     O .data.align.4  00000400 buf_4_align_init
; AIE: 00000000 g     O .data.align.16 00000400 buf_16_align_init
; AIE: 00000400 g     O .data.align.16 00000400 buf2_16_align_init
; AIE: 00000000 g     O .data.align.64 00000400 buf_64_align_init

@buf_4_align    = dso_local local_unnamed_addr global [256 x i32] zeroinitializer, align 4
@buf_16_align   = dso_local local_unnamed_addr global [256 x i32] zeroinitializer, align 16
@buf2_16_align  = dso_local local_unnamed_addr global [256 x i32] zeroinitializer, align 16
@buf_64_align   = dso_local local_unnamed_addr global [256 x i32] zeroinitializer, align 64
@internal_buf   = internal global [8 x i32] zeroinitializer, align 4

@buf_4_align_init   = dso_local local_unnamed_addr global <{ i32, [255 x i32] }> <{ i32 1, [255 x i32] zeroinitializer }>, align 4
@buf_16_align_init  = dso_local local_unnamed_addr global <{ i32, [255 x i32] }> <{ i32 1, [255 x i32] zeroinitializer }>, align 16
@buf2_16_align_init = dso_local local_unnamed_addr global <{ i32, [255 x i32] }> <{ i32 1, [255 x i32] zeroinitializer }>, align 16
@buf_64_align_init  = dso_local local_unnamed_addr global <{ i32, [255 x i32] }> <{ i32 1, [255 x i32] zeroinitializer }>, align 64
@internal_buf_init  = internal global [8 x i32] [i32 0, i32 -1, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0], align 4
