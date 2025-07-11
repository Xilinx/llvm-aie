; RUN: llc -mtriple=aie2p  %s -verify-machineinstrs -o - | FileCheck %s

; Test to check legalizer works for a vector of f32 elements.

; define float @multi_reduction_1d_16_f32(<16 x float> %0, float %1) {
;   %3 = call reassoc float @llvm.vector.reduce.fadd.v16f32(float %1, <16 x float> %0)
;   ret float %3
; }

; ; FPEXT
define <16 x float> @extend(bfloat %o, <16 x bfloat> %in) nounwind {
  %X = fpext <16 x bfloat> %in to <16 x float>
  ret <16 x float> %X
}