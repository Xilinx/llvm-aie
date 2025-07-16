; RUN: llc -mtriple=aie2p  %s --debug-only=legalizer | FileCheck %s

; Tests for legalizer support for vector types for G_FPEXT.

; define float @multi_reduction_1d_16_f32(<16 x float> %0, float %1) {
;   %3 = call reassoc float @llvm.vector.reduce.fadd.v16f32(float %1, <16 x float> %0)
;   ret float %3
; }

; ; FPEXT
define <32 x float> @extend(bfloat %o, <32 x bfloat> %in) nounwind {
  %X = fpext <32 x bfloat> %in to <32 x float>
  ret <32 x float> %X
}

; Not supported
; Non-power of 2 vector size
; CHECK: LLVM ERROR: unable to legalize
; define <17 x float> @extend_non_power_of_2(<17 x bfloat> %in) nounwind {
;   %X = fpext <17 x bfloat> %in to <17 x float>
;   ret <17 x float> %X
; }



