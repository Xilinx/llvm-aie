; RUN: llc -mtriple=aie2p  %s --debug-only=legalizer | FileCheck %s

; Valid
define <32 x float> @extend(bfloat %o, <32 x bfloat> %in) nounwind {
  %X = fpext <32 x bfloat> %in to <32 x float>
  ret <32 x float> %X
}

; Non-power of 2 vector size
define <17 x float> @extend_non_power_of_2(<17 x bfloat> %in) nounwind {
  %X = fpext <17 x bfloat> %in to <17 x float>
  ret <17 x float> %X
}

; Less than 512-bits, pad the inputs to 512-bits
define <16 x float> @fpext_bf16_to_f32(<16 x bfloat> %in) nounwind {
  %X = fpext <16 x bfloat> %in to <16 x float>
  ret <16 x float> %X
}

; Test the scalar path
define float @fpext_scalar_bf16_to_f32(bfloat %in) nounwind {
  %X = fpext bfloat %in to float
  ret float %X
}