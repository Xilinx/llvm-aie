; RUN: llc -mtriple=aie2p -O0 -stop-after=legalizer %s -o - 2>&1 | FileCheck %s
; This test is a carved out test from for sending patch upstream.
; iree-amd-aie/compiler/plugins/target/AMD-AIE/iree-amd-aie/Transforms/test/multi_reduction_to_reduction_sizes_types.mlirUntitled-1.mlir

; works
define bfloat @multi_reduction_1d_16_bf16(<16 x bfloat> %0, bfloat %1) {
  %3 = call reassoc bfloat @llvm.vector.reduce.fadd.v16bf16(bfloat %1, <16 x bfloat> %0)
  ret bfloat %3
}
; works
define bfloat @multi_reduction_1d_32_bf16(<32 x bfloat> %0, bfloat %1) {
  %3 = call reassoc bfloat @llvm.vector.reduce.fadd.v32bf16(bfloat %1, <32 x bfloat> %0)
  ret bfloat %3
}

; Fails 
define bfloat @multi_reduction_1d_64_bf16(<64 x bfloat> %0, bfloat %1) {
  %3 = call reassoc bfloat @llvm.vector.reduce.fadd.v64bf16(bfloat %1, <64 x bfloat> %0)
  ret bfloat %3
}