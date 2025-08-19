// RUN: mlir-opt --tosa-layerwise-constant-fold %s | FileCheck %s
// RUN: mlir-opt --tosa-layerwise-constant-fold="fold-splat-or-single-use-only=0" %s \
// RUN:   | FileCheck %s --check-prefix CHECK-ALWAYS

// CHECK-LABEL: @reshape_single_user
func.func @reshape_single_user() -> tensor<1x2xf32> {
  // CHECK: %[[RES:.*]] = "tosa.const"{{.*}}-> tensor<1x2xf32>
  // CHECK: return %[[RES]]
  %0 = "tosa.const"() {value = dense<4.0> : tensor<2xf32>} : () -> tensor<2xf32>
  %1 = tosa.reshape %0 {new_shape = array<i64: 1, 2>}: (tensor<2xf32>) -> tensor<1x2xf32>
  return %1 : tensor<1x2xf32>
}

// Splat constants are always folded, even when they have multiple users.
// CHECK-LABEL: @reshape_multi_user_splat
func.func @reshape_multi_user_splat() -> (tensor<1x2xf32>, tensor<2xf32>) {
  // CHECK-DAG: %[[RES:.*]] = "tosa.const"{{.*}}-> tensor<2xf32>
  // CHECK-DAG: %[[RESHAPED:.*]] = "tosa.const"{{.*}}-> tensor<1x2xf32>
  // CHECK: return %[[RESHAPED]], %[[RES]]
  %0 = "tosa.const"() {value = dense<4.0> : tensor<2xf32>} : () -> tensor<2xf32>
  %1 = tosa.reshape %0 {new_shape = array<i64: 1, 2>}: (tensor<2xf32>) -> tensor<1x2xf32>
  return %1, %0 : tensor<1x2xf32>, tensor<2xf32>
}

// Non-splat constants with multiple users are only folded when
// fold-splat-or-single-use-only=0 is set.
// CHECK-LABEL: @reshape_multi_user_non_splat
func.func @reshape_multi_user_non_splat() -> (tensor<1x2xf32>, tensor<2xf32>) {
  // CHECK: %[[CONST:.*]] = "tosa.const"{{.*}}-> tensor<2xf32>
  // CHECK: %[[RES:.*]] = tosa.reshape
  // CHECK: return %[[RES]], %[[CONST]]
  // CHECK-ALWAYS: %[[CONST_1:.*]] = "tosa.const"() <{value = dense<4.000000e+00> : tensor<2xf32>}> : () -> tensor<2xf32>
  // CHECK-ALWAYS: %[[CONST_2:.*]] = "tosa.const"() <{value = dense<4.000000e+00> : tensor<1x2xf32>}> : () -> tensor<1x2xf32>
  // CHECK-ALWAYS: return %[[CONST_2]], %[[CONST_1]]
  %0 = "tosa.const"() {value = dense<[4.0, 3.0]> : tensor<2xf32>} : () -> tensor<2xf32>
  %1 = tosa.reshape %0 {new_shape = array<i64: 1, 2>}: (tensor<2xf32>) -> tensor<1x2xf32>
  return %1, %0 : tensor<1x2xf32>, tensor<2xf32>
}
