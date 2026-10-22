// Modifications (c) Copyright 2026 Advanced Micro Devices, Inc. or its
// affiliates

// RUN: mlir-opt --split-input-file -canonicalize="test-convergence" %s | FileCheck %s

// CHECK-LABEL: @negate_negate_quant_nofold
// CHECK-NEXT: tosa.negate
// CHECK-NEXT: tosa.negate

// The output of negate should be clipped to the range of the storage type.
// However, the canonicalization pass illegally removes the intermediate clip.
// Thus, negate(negate(x)) = x is not valid when x carries a quant type.
// A simple counter-example is neg(neg(-128)) = neg(127) = -127 != -128
func.func @negate_negate_quant_nofold(%arg0: tensor<4x!quant.uniform<i8:f32, 0.05>>) -> tensor<4x!quant.uniform<i8:f32, 0.05>> {
  %0 = tosa.negate %arg0 : (tensor<4x!quant.uniform<i8:f32, 0.05>>) -> tensor<4x!quant.uniform<i8:f32, 0.05>>
  %1 = tosa.negate %0 : (tensor<4x!quant.uniform<i8:f32, 0.05>>) -> tensor<4x!quant.uniform<i8:f32, 0.05>>
  return %1 : tensor<4x!quant.uniform<i8:f32, 0.05>>
}

// -----

// CHECK-LABEL: @exp_log_quant_nofold
// CHECK-NEXT: tosa.exp
// CHECK-NEXT: tosa.log
func.func @exp_log_quant_nofold(%arg0: tensor<4x!quant.uniform<i8:f32, 0.05>>) -> tensor<4x!quant.uniform<i8:f32, 0.05>> {
  %0 = tosa.exp %arg0 : (tensor<4x!quant.uniform<i8:f32, 0.05>>) -> tensor<4x!quant.uniform<i8:f32, 0.05>>
  %1 = tosa.log %0 : (tensor<4x!quant.uniform<i8:f32, 0.05>>) -> tensor<4x!quant.uniform<i8:f32, 0.05>>
  return %1 : tensor<4x!quant.uniform<i8:f32, 0.05>>
}

// -----

// CHECK-LABEL: @log_exp_quant_nofold
// CHECK-NEXT: tosa.log
// CHECK-NEXT: tosa.exp
func.func @log_exp_quant_nofold(%arg0: tensor<4x!quant.uniform<i8:f32, 0.05>>) -> tensor<4x!quant.uniform<i8:f32, 0.05>> {
  %0 = tosa.log %arg0 : (tensor<4x!quant.uniform<i8:f32, 0.05>>) -> tensor<4x!quant.uniform<i8:f32, 0.05>>
  %1 = tosa.exp %0 : (tensor<4x!quant.uniform<i8:f32, 0.05>>) -> tensor<4x!quant.uniform<i8:f32, 0.05>>
  return %1 : tensor<4x!quant.uniform<i8:f32, 0.05>>
}

// -----

// CHECK-LABEL: @min_to_clamp_quant
// CHECK: tosa.clamp %arg0 {max_fp = 6.000000e+00 : f32, max_int = 6 : i64,
// CHECK-SAME: min_fp = -3.40282347E+38 : f32, min_int = -2147483648 : i64}
func.func @min_to_clamp_quant(%arg0: tensor<4x!quant.uniform<i8:f32, 0.05>>) -> tensor<4x!quant.uniform<i8:f32, 0.05>> {
  %0 = "tosa.const"() <{value = dense<6> : tensor<1xi8>}> : () -> tensor<1x!quant.uniform<i8:f32, 0.05>>
  %1 = tosa.minimum %arg0, %0 : (tensor<4x!quant.uniform<i8:f32, 0.05>>, tensor<1x!quant.uniform<i8:f32, 0.05>>) -> tensor<4x!quant.uniform<i8:f32, 0.05>>
  return %1 : tensor<4x!quant.uniform<i8:f32, 0.05>>
}

// -----

// CHECK-LABEL: @max_to_clamp_quant
// CHECK: tosa.clamp %arg0 {max_fp = 3.40282347E+38 : f32, max_int = 9223372036854775807 : i64,
// CHECK-SAME: min_fp = -6.000000e+00 : f32, min_int = -6 : i64}
func.func @max_to_clamp_quant(%arg0: tensor<4x!quant.uniform<i8:f32, 0.05>>) -> tensor<4x!quant.uniform<i8:f32, 0.05>> {
  %0 = "tosa.const"() <{value = dense<-6> : tensor<1xi8>}> : () -> tensor<1x!quant.uniform<i8:f32, 0.05>>
  %1 = tosa.maximum %arg0, %0 : (tensor<4x!quant.uniform<i8:f32, 0.05>>, tensor<1x!quant.uniform<i8:f32, 0.05>>) -> tensor<4x!quant.uniform<i8:f32, 0.05>>
  return %1 : tensor<4x!quant.uniform<i8:f32, 0.05>>
}
