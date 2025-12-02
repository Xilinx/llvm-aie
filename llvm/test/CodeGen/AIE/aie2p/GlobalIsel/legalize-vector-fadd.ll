; RUN: llc -mtriple=aie2p -O0 -stop-after=legalizer %s -o - 2>&1 | FileCheck %s
; This test is a carved out test for sending patch upstream from 
; iree-amd-aie/compiler/plugins/target/AMD-AIE/iree-amd-aie/Transforms/test/multi_reduction_to_reduction_sizes_types.mlirUntitled-1.mlir

; Ideally reduction should be as follows(with minor changes for each shape):
  ; Input1: <32xbf16> and Input2: <32xbf16>
  ; Extended1<32xf32> = fpext <32xbf16>
  ; Extended2<32xf32> = fpext <32xbf16>
  ; Zero<32xf32> = zeroinitializer
  ; Out1<64xf32> = Concat zero, <Extended1<32xf32>>
  ; Out2<64xf32> = Concat zero, <Extended2<32xf32>>
  ; Result<64xf32> = fadd <Out1<64xf32>>, <Out2<64xf32>>
  ; R1<32xf32>, R2<32xf32> = unmerge <Result<64xf32>>
  ; R2 is all 0s
  ; R1<32xbf16> = trunc <R1<32xf32>>
  
; check the vadd.f
; pad checks
; checks similar to <32xbf16>
; unpad checks
define bfloat @multi_reduction_1d_16_bf16(<16 x bfloat> %0, bfloat %1) {
  %3 = call reassoc bfloat @llvm.vector.reduce.fadd.v16bf16(bfloat %1, <16 x bfloat> %0)
  ret bfloat %3
}



; CHECK-LABEL: name:            multi_reduction_1d_32_bf16
; CHECK: G_CONSTANT i32 0
; CHECK: G_AIE_BROADCAST_VECTOR %{{[0-9]+}}(s32)
; CHECK: G_CONSTANT i32 2
; CHECK: G_CONSTANT i32 3
; CHECK: G_AIE_SHUFFLE_VECTOR %{{[0-9]+}}, %{{[0-9]+}}, %{{[0-9]+}}(s32)
; CHECK: G_AIE_SHUFFLE_VECTOR %{{[0-9]+}}, %{{[0-9]+}}, %{{[0-9]+}}(s32)
; CHECK: G_BITCAST %{{[0-9]+}}(<32 x s16>)
; CHECK: G_BITCAST %{{[0-9]+}}(<32 x s16>)
; CHECK: G_CONCAT_VECTORS %{{[0-9]+}}(<16 x s32>), %{{[0-9]+}}(<16 x s32>)
; CHECK: G_IMPLICIT_DEF
; CHECK: G_CONCAT_VECTORS %{{[0-9]+}}(<32 x s32>), %{{[0-9]+}}(<32 x s32>)
; CHECK: G_FADD %{{[0-9]+}}, %{{[0-9]+}}
; CHECK: G_UNMERGE_VALUES %{{[0-9]+}}(<64 x s32>)
; CHECK: G_INTRINSIC_W_SIDE_EFFECTS intrinsic(@llvm.aie2p.v32accfloat.to.v32bf16), %{{[0-9]+}}(<32 x s32>)
define bfloat @multi_reduction_1d_32_bf16(<32 x bfloat> %0, bfloat %1) {
  %3 = call reassoc bfloat @llvm.vector.reduce.fadd.v32bf16(bfloat %1, <32 x bfloat> %0)
  ret bfloat %3
}

; ; Converted to chunks of <32 x bf16>
; Check if the input is split into 2 chunks of <32 x bf16>
; Check for each chunk similar to <32xbf16> case
; Check if both inputs get concatenated to <64xbf16>

define bfloat @multi_reduction_1d_64_bf16(<64 x bfloat> %0, bfloat %1) {
  %3 = call reassoc bfloat @llvm.vector.reduce.fadd.v64bf16(bfloat %1, <64 x bfloat> %0)
  ret bfloat %3
}
