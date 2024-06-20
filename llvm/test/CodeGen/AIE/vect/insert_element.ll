;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc < %s -mtriple=aie | FileCheck %s --check-prefixes=CHECK,ASSEMBLE
; RUN: llc < %s -mtriple=aie --filetype=obj -o %t
; RUN: llvm-objdump --triple=aie -dr %t | FileCheck %s --check-prefixes=CHECK,DISASSEMBLE

define <4 x float>  @insert_v4f32(<4 x float> %v, float %a) nounwind {
; CHECK-LABEL: insert_v4f32
; ASSEMBLE:      vupd.32 vl0[#3], r6
; DISASSEMBLE:   vupd.32 vl0[#3], r6
  %1 = insertelement <4 x float> %v, float %a, i32 3
  ret <4 x float> %1
}

define <4 x i32>  @insert_v4i32(<4 x i32> %v, i32 %a) nounwind {
; CHECK-LABEL: insert_v4i32
; ASSEMBLE:      vupd.32 vl0[#3], r6
; DISASSEMBLE:   vupd.32 vl0[#3], r6
  %1 = insertelement <4 x i32> %v, i32 %a, i32 3
  ret <4 x i32> %1
}

define <8 x float>  @insert_v8f32(<8 x float> %v, float %a) nounwind {
; CHECK-LABEL: insert_v8f32
; ASSEMBLE:      vupd.32_w wr0[#7], r6
; DISASSEMBLE:   vupd.32 vh0[#3], r6
  %1 = insertelement <8 x float> %v, float %a, i32 7
  ret <8 x float> %1
}

define <8 x i32>  @insert_v8i32(<8 x i32> %v, i32 %a) nounwind {
; CHECK-LABEL: insert_v8i32
; ASSEMBLE:      vupd.32_w wr0[#7], r6
; DISASSEMBLE:   vupd.32 vh0[#3], r6
  %1 = insertelement <8 x i32> %v, i32 %a, i32 7
  ret <8 x i32> %1
}

define <16 x float>  @insert_v16f32(<16 x float> %v, float %a) nounwind {
; CHECK-LABEL: insert_v16f32
; ASSEMBLE:      vupd.32_x xa[#11], r6
; DISASSEMBLE:   vupd.32 vl1[#3], r6
  %1 = insertelement <16 x float> %v, float %a, i32 11
  ret <16 x float> %1
}

define <16 x i32>  @insert_v16i32(<16 x i32> %v, i32 %a) nounwind {
; CHECK-LABEL: insert_v16i32
; ASSEMBLE:      vupd.32_x xa[#11], r6
; DISASSEMBLE:   vupd.32 vl1[#3], r6
  %1 = insertelement <16 x i32> %v, i32 %a, i32 11
  ret <16 x i32> %1
}


define <8 x i16>  @insert_v8i16(<8 x i16> %v, i16 %a) nounwind {
; CHECK-LABEL: insert_v8i16
; ASSEMBLE:      vupd.16 vl0[#3], r6
; DISASSEMBLE:   vupd.16 vl0[#3], r6
  %1 = insertelement <8 x i16> %v, i16 %a, i32 3
  ret <8 x i16> %1
}
define <16 x i16>  @insert_v16i16(<16 x i16> %v, i16 %a) nounwind {
; CHECK-LABEL: insert_v16i16
; ASSEMBLE:      vupd.16_w wr0[#11], r6
; DISASSEMBLE:   vupd.16 vh0[#3], r6
  %1 = insertelement <16 x i16> %v, i16 %a, i32 11
  ret <16 x i16> %1
}
define <32 x i16>  @insert_v32i16(<32 x i16> %v, i16 %a) nounwind {
; CHECK-LABEL: insert_v32i16
; ASSEMBLE:      vupd.16_x xa[#19], r6
; DISASSEMBLE:   vupd.16 vl1[#3], r6
  %1 = insertelement <32 x i16> %v, i16 %a, i32 19
  ret <32 x i16> %1
}
