; RUN: llc -mtriple=aie2p -O0 -stop-after=legalizer %s -o - 2>&1 | FileCheck %s


; Validates bfloat -> float legalization.
; CHECK-LABEL: name: extend
; CHECK:   [[COPY:%[0-9]+]]:_(<32 x s16>) = COPY $x0
; CHECK-NOT: G_SHL
; CHECK-NEXT:   [[C2:%[0-9]+]]:_(s32) = G_CONSTANT i32 2
; CHECK-NEXT:   [[C3:%[0-9]+]]:_(s32) = G_CONSTANT i32 3
; CHECK-NEXT:   [[C0:%[0-9]+]]:_(s32) = G_CONSTANT i32 0
; CHECK-NEXT:   [[BCAST:%[0-9]+]]:_(<32 x s16>) = G_AIE_BROADCAST_VECTOR [[C0]](s32)
; CHECK-NEXT:   [[SHUF1:%[0-9]+]]:_(<32 x s16>) = G_AIE_SHUFFLE_VECTOR [[BCAST]], [[COPY]], [[C2]](s32)
; CHECK-NEXT:   [[SHUF2:%[0-9]+]]:_(<32 x s16>) = G_AIE_SHUFFLE_VECTOR [[BCAST]], [[COPY]], [[C3]](s32)
; CHECK-NEXT:   [[BIT1:%[0-9]+]]:_(<16 x s32>) = G_BITCAST [[SHUF1]](<32 x s16>)
; CHECK-NEXT:   [[BIT2:%[0-9]+]]:_(<16 x s32>) = G_BITCAST [[SHUF2]](<32 x s16>)
; CHECK-NEXT:   [[CONCAT:%[0-9]+]]:_(<32 x s32>) = G_CONCAT_VECTORS [[BIT1]](<16 x s32>), [[BIT2]](<16 x s32>)

define <32 x float> @extend(bfloat %o, <32 x bfloat> %in) nounwind {
  %X = fpext <32 x bfloat> %in to <32 x float>
  ret <32 x float> %X
}

; Pads the 17 valid values with undefined values to form a 32 size vector.

; CHECK-LABEL: name: extend_non_power_of_2
; CHECK:   [[COPY:%[0-9]+]]:_(<32 x s16>) = COPY $x0
; CHECK-COUNT-17: G_AIE_SEXT_EXTRACT_VECTOR_ELT
; CHECK-COUNT-32: G_AIE_ADD_VECTOR_ELT_HI
; CHECK-NEXT:   [[C2:%[0-9]+]]:_(s32) = G_CONSTANT i32 2
; CHECK-NEXT:   [[C3:%[0-9]+]]:_(s32) = G_CONSTANT i32 3
; CHECK-NEXT:   [[C0:%[0-9]+]]:_(s32) = G_CONSTANT i32 0
; CHECK-NEXT:   [[BCAST:%[0-9]+]]:_(<32 x s16>) = G_AIE_BROADCAST_VECTOR [[C0]](s32)
; CHECK-NEXT:   [[SHUF1:%[0-9]+]]:_(<32 x s16>) = G_AIE_SHUFFLE_VECTOR [[BCAST]], %{{[0-9]+}}, [[C2]](s32)
; CHECK-NEXT:   [[SHUF2:%[0-9]+]]:_(<32 x s16>) = G_AIE_SHUFFLE_VECTOR [[BCAST]], %{{[0-9]+}}, [[C3]](s32)
; CHECK-NEXT:   [[BIT1:%[0-9]+]]:_(<16 x s32>) = G_BITCAST [[SHUF1]](<32 x s16>)
; CHECK-NEXT:   [[BIT2:%[0-9]+]]:_(<16 x s32>) = G_BITCAST [[SHUF2]](<32 x s16>)
; CHECK-COUNT-17: G_AIE_SEXT_EXTRACT_VECTOR_ELT
; CHECK-COUNT-32: G_AIE_ADD_VECTOR_ELT_HI
; CHECK-NEXT:   [[CONCAT:%[0-9]+]]:_(<32 x s32>) = G_CONCAT_VECTORS %{{[0-9]+}}(<16 x s32>), %{{[0-9]+}}(<16 x s32>)
define <17 x float> @extend_non_power_of_2(<17 x bfloat> %in) nounwind {
  %X = fpext <17 x bfloat> %in to <17 x float>
  ret <17 x float> %X
}

; Validates if vector size < 256 bits

; CHECK-LABEL: name: fpext_bf16_to_f32
; CHECK: bb.1
; CHECK: [[VEC_CONCAT:%[0-9]+]]:_(<32 x s16>) = G_CONCAT_VECTORS
; CHECK: G_AIE_SEXT_EXTRACT_VECTOR_ELT [[VEC_CONCAT]]
; CHECK: G_AIE_ADD_VECTOR_ELT_HI
; CHECK: [[SHUFFLE_VEC:%[0-9]+]]:_(<32 x s16>) = G_AIE_SHUFFLE_VECTOR
; CHECK-NOT: G_AIE_SHUFFLE_VECTOR
; CHECK: [[BITCAST:%[0-9]+]]:_(<16 x s32>) = G_BITCAST [[SHUFFLE_VEC]]
; CHECK: $x0 = COPY [[BITCAST]]
define <16 x float> @fpext_bf16_to_f32(<16 x bfloat> %in) nounwind {
  %X = fpext <16 x bfloat> %in to <16 x float>
  ret <16 x float> %X
}

; Validates scalar path
; CHECK-LABEL: name: fpext_scalar_bf16_to_f32
; CHECK:   [[COPY:%[0-9]+]]:_(s32) = COPY $r1
; CHECK-NEXT:   [[C16:%[0-9]+]]:_(s32) = G_CONSTANT i32 16
; CHECK-NEXT:   [[SHL:%[0-9]+]]:_(s32) = G_SHL [[COPY]], [[C16]](s32)
; CHECK-NOT: G_AIE_SHUFFLE_VECTOR
; CHECK-NEXT:   $r0 = COPY [[SHL]](s32)
; CHECK-NEXT:   PseudoRET implicit $lr, implicit $r0

define float @fpext_scalar_bf16_to_f32(bfloat %in) nounwind {
  %X = fpext bfloat %in to float
  ret float %X
}
