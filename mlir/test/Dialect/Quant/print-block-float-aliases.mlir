// (c) Copyright 2026 Advanced Micro Devices, Inc. or its
// affiliates

// RUN: mlir-opt %s -split-input-file | FileCheck %s

// CHECK: !mx6 = !quant.block_float<mode=MX6, axis=1>
// CHECK-LABEL: func.func @alias_positive(
// CHECK: tensor<4x!mx6>
// CHECK: return %arg0 : tensor<4x!mx6>
func.func @alias_positive(%arg0: tensor<4x!quant.block_float<mode=MX6, axis=1>>)
    -> tensor<4x!quant.block_float<mode=MX6, axis=1>> {
  return %arg0 : tensor<4x!quant.block_float<mode=MX6, axis=1>>
}

// -----

// CHECK-LABEL: func.func @no_alias_mx6_other_axis(
// CHECK-NOT: !mx6
// CHECK: tensor<4x!quant.block_float<mode=MX6, axis=2>>
func.func @no_alias_mx6_other_axis(
    %arg0: tensor<4x!quant.block_float<mode=MX6, axis=2>>)
    -> tensor<4x!quant.block_float<mode=MX6, axis=2>> {
  return %arg0 : tensor<4x!quant.block_float<mode=MX6, axis=2>>
}

// -----

// CHECK: !bfp16 = !quant.block_float<mode=BFP16, axis=1>
// CHECK-LABEL: func.func @alias_bfp16_axis1(
// CHECK: tensor<4x!bfp16>
func.func @alias_bfp16_axis1(
    %arg0: tensor<4x!quant.block_float<mode=BFP16, axis=1>>)
    -> tensor<4x!quant.block_float<mode=BFP16, axis=1>> {
  return %arg0 : tensor<4x!quant.block_float<mode=BFP16, axis=1>>
}

// -----

// CHECK-LABEL: func.func @no_alias_bfp16_other_axis(
// CHECK-NOT: !bfp16
// CHECK: tensor<4x!quant.block_float<mode=BFP16, axis=0>>
func.func @no_alias_bfp16_other_axis(
    %arg0: tensor<4x!quant.block_float<mode=BFP16, axis=0>>)
    -> tensor<4x!quant.block_float<mode=BFP16, axis=0>> {
  return %arg0 : tensor<4x!quant.block_float<mode=BFP16, axis=0>>
}
