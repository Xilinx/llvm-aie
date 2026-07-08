; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie2 -stop-after=instruction-select %s -o - | FileCheck %s --check-prefix=SELECT
; RUN: llc -mtriple=aie2 -stop-after=postmisched %s -o - | FileCheck %s --check-prefix=MOV

; The intrinsic lowers to the multi-slot PseudoLoopVersionThreshold, which the
; post-RA scheduler materializes into a concrete scalar move.

; SELECT-LABEL: name: threshold
; SELECT: PseudoLoopVersionThreshold 9

; MOV-LABEL: name: threshold
; MOV: MOVA_lda_cg 9
define i32 @threshold() {
entry:
  %t = call i32 @llvm.aie2.loop.version.threshold(i32 9)
  ret i32 %t
}

declare i32 @llvm.aie2.loop.version.threshold(i32 immarg)
