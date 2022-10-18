;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc < %s -mtriple=aie | FileCheck %s --check-prefixes=CHECK,ASSEMBLE
; RUN: llc < %s -mtriple=aie --filetype=obj -o %t
; RUN: llvm-objdump --triple=aie -dr %t | FileCheck %s --check-prefixes=CHECK,DISASSEMBLE

define float  @extract_v4f32(<4 x float> %v) nounwind {
; CHECK-LABEL: extract_v4f32
; ASSEMBLE:      vext.32 r0, vl0[#3]
; DISASSEMBLE:   vext.32 r0, vl0[#3]
  %1 = extractelement <4 x float> %v, i32 3
  ret float %1
}

define i32  @extract_v4i32(<4 x i32> %v) nounwind {
; CHECK-LABEL: extract_v4i32
; ASSEMBLE:      vext.32 r0, vl0[#3]
; DISASSEMBLE:   vext.32 r0, vl0[#3]
  %1 = extractelement <4 x i32> %v, i32 3
  ret i32 %1
}

define float  @extract_v8f32(<8 x float> %v) nounwind {
; CHECK-LABEL: extract_v8f32
; ASSEMBLE:      vext.32_w r0, wr0[#7]
; DISASSEMBLE:   vext.32 r0, vh0[#3]
  %1 = extractelement <8 x float> %v, i32 7
  ret float %1
}

define i32  @extract_v8i32(<8 x i32> %v) nounwind {
; CHECK-LABEL: extract_v8i32
; ASSEMBLE:      vext.32_w r0, wr0[#7]
; DISASSEMBLE:   vext.32 r0, vh0[#3]
  %1 = extractelement <8 x i32> %v, i32 7
  ret i32 %1
}

define float  @extract_v16f32(<16 x float> %v) nounwind {
; CHECK-LABEL: extract_v16f32
; ASSEMBLE:      vext.32_x r0, xa[#11]
; DISASSEMBLE:   vext.32 r0, vl1[#3]
  %1 = extractelement <16 x float> %v, i32 11
  ret float %1
}

define i32  @extract_v16i32(<16 x i32> %v) nounwind {
; CHECK-LABEL: extract_v16i32
; ASSEMBLE:      vext.32_x r0, xa[#11]
; DISASSEMBLE:   vext.32 r0, vl1[#3]
  %1 = extractelement <16 x i32> %v, i32 11
  ret i32 %1
}


define i16  @extract_v8i16(<8 x i16> %v) nounwind {
; CHECK-LABEL: extract_v8i16
; ASSEMBLE:      vext.16 r0, vl0[#3]
; DISASSEMBLE:   vext.16 r0, vl0[#3]
  %1 = extractelement <8 x i16> %v, i32 3
  ret i16 %1
}
define i16  @extract_v16i16(<16 x i16> %v) nounwind {
; CHECK-LABEL: extract_v16i16
; ASSEMBLE:      vext.16_w r0, wr0[#11]
; DISASSEMBLE:   vext.16 r0, vh0[#3]
  %1 = extractelement <16 x i16> %v, i32 11
  ret i16 %1
}
define i16  @extract_v32i16(<32 x i16> %v) nounwind {
; CHECK-LABEL: extract_v32i16
; ASSEMBLE:      vext.16_x r0, xa[#19]
; DISASSEMBLE:   vext.16 r0, vl1[#3]
  %1 = extractelement <32 x i16> %v, i32 19
  ret i16 %1
}
