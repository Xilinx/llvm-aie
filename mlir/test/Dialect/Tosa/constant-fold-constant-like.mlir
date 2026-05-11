// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold %s | FileCheck %s

// CHECK-LABEL: @reciprocal_fold_arith_const
func.func @reciprocal_fold_arith_const() -> tensor<f32> {
  // CHECK: %[[RES:.*]] = "tosa.const"() <{value = dense<2.500000e-01> : tensor<f32>}> : () -> tensor<f32>
  // CHECK-NOT: arith.constant
  // CHECK-NOT: tosa.reciprocal
  // CHECK: return %[[RES]]
  %0 = arith.constant dense<4.0> : tensor<f32>
  %1 = "tosa.reciprocal"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// -----

// CHECK-LABEL: @reshape_fold_arith_const
func.func @reshape_fold_arith_const() -> tensor<1x2xf32> {
  // CHECK: %[[RES:.*]] = "tosa.const"(){{.*}}-> tensor<1x2xf32>
  // CHECK-NOT: tosa.reshape
  // CHECK: return %[[RES]]
  %0 = arith.constant dense<4.0> : tensor<2xf32>
  %1 = tosa.reshape %0 {new_shape = array<i64: 1, 2>}: (tensor<2xf32>) -> tensor<1x2xf32>
  return %1 : tensor<1x2xf32>
}

// -----

// CHECK-LABEL: @add_fold_two_arith_const
func.func @add_fold_two_arith_const() -> tensor<2xf32> {
  // CHECK: %[[RES:.*]] = "tosa.const"() <{value = dense<[4.000000e+00, 6.000000e+00]> : tensor<2xf32>}>
  // CHECK-NOT: tosa.add
  // CHECK: return %[[RES]]
  %0 = arith.constant dense<[1.0, 2.0]> : tensor<2xf32>
  %1 = arith.constant dense<[3.0, 4.0]> : tensor<2xf32>
  %2 = "tosa.add"(%0, %1) : (tensor<2xf32>, tensor<2xf32>) -> tensor<2xf32>
  return %2 : tensor<2xf32>
}

// -----

// CHECK-LABEL: @add_fold_mixed_const_inputs
func.func @add_fold_mixed_const_inputs() -> tensor<2xf32> {
  // CHECK: %[[RES:.*]] = "tosa.const"() <{value = dense<[4.000000e+00, 6.000000e+00]> : tensor<2xf32>}>
  // CHECK-NOT: tosa.add
  // CHECK: return %[[RES]]
  %0 = "tosa.const"() {value = dense<[1.0, 2.0]> : tensor<2xf32>} : () -> tensor<2xf32>
  %1 = arith.constant dense<[3.0, 4.0]> : tensor<2xf32>
  %2 = "tosa.add"(%0, %1) : (tensor<2xf32>, tensor<2xf32>) -> tensor<2xf32>
  return %2 : tensor<2xf32>
}

// -----

// CHECK-LABEL: @cast_fold_arith_const
func.func @cast_fold_arith_const() -> tensor<2xi32> {
  // CHECK: %[[RES:.*]] = "tosa.const"() <{value = dense<[1, 2]> : tensor<2xi32>}>
  // CHECK-NOT: tosa.cast
  // CHECK: return %[[RES]]
  %0 = arith.constant dense<[1.0, 2.0]> : tensor<2xf32>
  %1 = "tosa.cast"(%0) : (tensor<2xf32>) -> tensor<2xi32>
  return %1 : tensor<2xi32>
}

// -----

// CHECK-LABEL: @add_no_fold_non_const
func.func @add_no_fold_non_const(%arg0: tensor<2xf32>) -> tensor<2xf32> {
  // CHECK: %[[CST:.*]] = arith.constant dense<[1.000000e+00, 2.000000e+00]> : tensor<2xf32>
  // CHECK: %[[RES:.*]] = tosa.add %arg0, %[[CST]]
  // CHECK: return %[[RES]]
  %0 = arith.constant dense<[1.0, 2.0]> : tensor<2xf32>
  %1 = "tosa.add"(%arg0, %0) : (tensor<2xf32>, tensor<2xf32>) -> tensor<2xf32>
  return %1 : tensor<2xf32>
}
