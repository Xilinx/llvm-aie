// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold %s | FileCheck %s

// CHECK-LABEL: @select_fold_int
func.func @select_fold_int() -> tensor<4xi32> {
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[10, 50, 30, 80]>
  // CHECK-NOT: tosa.select
  // CHECK: return [[RES]]
  %pred = "tosa.const"() {value = dense<[1, 0, 1, 0]> : tensor<4xi1>} : () -> tensor<4xi1>
  %a = "tosa.const"() {value = dense<[10, 20, 30, 40]> : tensor<4xi32>} : () -> tensor<4xi32>
  %b = "tosa.const"() {value = dense<[50, 50, 70, 80]> : tensor<4xi32>} : () -> tensor<4xi32>
  %0 = tosa.select %pred, %a, %b : (tensor<4xi1>, tensor<4xi32>, tensor<4xi32>) -> tensor<4xi32>
  return %0 : tensor<4xi32>
}

// -----

// CHECK-LABEL: @select_fold_float
func.func @select_fold_float() -> tensor<4xf32> {
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[1.000000e+00, 6.000000e+00, 3.000000e+00, 8.000000e+00]>
  // CHECK-NOT: tosa.select
  // CHECK: return [[RES]]
  %pred = "tosa.const"() {value = dense<[1, 0, 1, 0]> : tensor<4xi1>} : () -> tensor<4xi1>
  %a = "tosa.const"() {value = dense<[1.0, 2.0, 3.0, 4.0]> : tensor<4xf32>} : () -> tensor<4xf32>
  %b = "tosa.const"() {value = dense<[5.0, 6.0, 7.0, 8.0]> : tensor<4xf32>} : () -> tensor<4xf32>
  %0 = tosa.select %pred, %a, %b : (tensor<4xi1>, tensor<4xf32>, tensor<4xf32>) -> tensor<4xf32>
  return %0 : tensor<4xf32>
}

// -----

// CHECK-LABEL: @select_fold_all_true
func.func @select_fold_all_true() -> tensor<3xi32> {
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[10, 20, 30]>
  // CHECK-NOT: tosa.select
  // CHECK: return [[RES]]
  %pred = "tosa.const"() {value = dense<1> : tensor<3xi1>} : () -> tensor<3xi1>
  %a = "tosa.const"() {value = dense<[10, 20, 30]> : tensor<3xi32>} : () -> tensor<3xi32>
  %b = "tosa.const"() {value = dense<[40, 50, 60]> : tensor<3xi32>} : () -> tensor<3xi32>
  %0 = tosa.select %pred, %a, %b : (tensor<3xi1>, tensor<3xi32>, tensor<3xi32>) -> tensor<3xi32>
  return %0 : tensor<3xi32>
}

// -----

// CHECK-LABEL: @select_fold_all_false
func.func @select_fold_all_false() -> tensor<3xi32> {
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[40, 50, 60]>
  // CHECK-NOT: tosa.select
  // CHECK: return [[RES]]
  %pred = "tosa.const"() {value = dense<0> : tensor<3xi1>} : () -> tensor<3xi1>
  %a = "tosa.const"() {value = dense<[10, 20, 30]> : tensor<3xi32>} : () -> tensor<3xi32>
  %b = "tosa.const"() {value = dense<[40, 50, 60]> : tensor<3xi32>} : () -> tensor<3xi32>
  %0 = tosa.select %pred, %a, %b : (tensor<3xi1>, tensor<3xi32>, tensor<3xi32>) -> tensor<3xi32>
  return %0 : tensor<3xi32>
}

// -----

// CHECK-LABEL: @select_fold_broadcast_pred
func.func @select_fold_broadcast_pred() -> tensor<4xi32> {
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[10, 20, 30, 40]>
  // CHECK-NOT: tosa.select
  // CHECK: return [[RES]]
  %pred = "tosa.const"() {value = dense<1> : tensor<1xi1>} : () -> tensor<1xi1>
  %a = "tosa.const"() {value = dense<[10, 20, 30, 40]> : tensor<4xi32>} : () -> tensor<4xi32>
  %b = "tosa.const"() {value = dense<[50, 60, 70, 80]> : tensor<4xi32>} : () -> tensor<4xi32>
  %0 = tosa.select %pred, %a, %b : (tensor<1xi1>, tensor<4xi32>, tensor<4xi32>) -> tensor<4xi32>
  return %0 : tensor<4xi32>
}

// -----

// CHECK-LABEL: @select_fold_broadcast_values
func.func @select_fold_broadcast_values() -> tensor<4xi32> {
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[99, 50, 99, 80]>
  // CHECK-NOT: tosa.select
  // CHECK: return [[RES]]
  %pred = "tosa.const"() {value = dense<[1, 0, 1, 0]> : tensor<4xi1>} : () -> tensor<4xi1>
  %a = "tosa.const"() {value = dense<99> : tensor<1xi32>} : () -> tensor<1xi32>
  %b = "tosa.const"() {value = dense<[50, 50, 70, 80]> : tensor<4xi32>} : () -> tensor<4xi32>
  %0 = tosa.select %pred, %a, %b : (tensor<4xi1>, tensor<1xi32>, tensor<4xi32>) -> tensor<4xi32>
  return %0 : tensor<4xi32>
}

// -----

// CHECK-LABEL: @select_fold_2d
func.func @select_fold_2d() -> tensor<2x3xi32> {
  //               CHECK: [[RES:]] ={{.*}}tosa.const
  // CHECK-SAME{LITERAL}: value = dense<[[10, 50, 30], [70, 40, 90]]>
  // CHECK-NOT: tosa.select
  // CHECK: return [[RES]]
  %pred = "tosa.const"() {value = dense<[[1, 0, 1], [0, 1, 0]]> : tensor<2x3xi1>} : () -> tensor<2x3xi1>
  %a = "tosa.const"() {value = dense<[[10, 20, 30], [40, 40, 60]]> : tensor<2x3xi32>} : () -> tensor<2x3xi32>
  %b = "tosa.const"() {value = dense<[[40, 50, 60], [70, 80, 90]]> : tensor<2x3xi32>} : () -> tensor<2x3xi32>
  %0 = tosa.select %pred, %a, %b : (tensor<2x3xi1>, tensor<2x3xi32>, tensor<2x3xi32>) -> tensor<2x3xi32>
  return %0 : tensor<2x3xi32>
}

// -----

// Gather negative-index normalization: the full chain folds end-to-end.
// indices = [-1, 2, -3, 0], axis_dim_size = 5
//   offset       = indices + 5          = [4, 7, 2, 5]
//   isNonNeg     = indices >= 0         = [false, true, false, true]
//   normalized   = select(isNonNeg, indices, offset) = [4, 2, 2, 0]
//   cast_to_ui32 = cast(normalized)     = [4, 2, 2, 0] : ui32
// CHECK-LABEL: @select_fold_generic
func.func @select_fold_generic() -> tensor<4xui32> {
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[4, 2, 2, 0]> : tensor<4xui32>}>
  // CHECK-NOT: tosa.add
  // CHECK-NOT: tosa.greater_equal
  // CHECK-NOT: tosa.select
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %indices = "tosa.const"() {value = dense<[-1, 2, -3, 0]> : tensor<4xi32>} : () -> tensor<4xi32>
  %axis_dim = "tosa.const"() {value = dense<5> : tensor<4xi32>} : () -> tensor<4xi32>
  %zero = "tosa.const"() {value = dense<0> : tensor<4xi32>} : () -> tensor<4xi32>
  %offset = tosa.add %indices, %axis_dim : (tensor<4xi32>, tensor<4xi32>) -> tensor<4xi32>
  %is_non_neg = tosa.greater_equal %indices, %zero : (tensor<4xi32>, tensor<4xi32>) -> tensor<4xi1>
  %normalized = tosa.select %is_non_neg, %indices, %offset : (tensor<4xi1>, tensor<4xi32>, tensor<4xi32>) -> tensor<4xi32>
  %result = tosa.cast %normalized : (tensor<4xi32>) -> tensor<4xui32>
  return %result : tensor<4xui32>
}

// -----

// Non-constant input: pattern should NOT fire
// CHECK-LABEL: @select_no_fold_dynamic_pred
func.func @select_no_fold_dynamic_pred(%arg0: tensor<4xi1>) -> tensor<4xi32> {
  // CHECK: tosa.select
  %a = "tosa.const"() {value = dense<[10, 20, 30, 40]> : tensor<4xi32>} : () -> tensor<4xi32>
  %b = "tosa.const"() {value = dense<[50, 60, 70, 80]> : tensor<4xi32>} : () -> tensor<4xi32>
  %0 = tosa.select %arg0, %a, %b : (tensor<4xi1>, tensor<4xi32>, tensor<4xi32>) -> tensor<4xi32>
  return %0 : tensor<4xi32>
}

// -----

// Non-constant on_true: pattern should NOT fire
// CHECK-LABEL: @select_no_fold_dynamic_on_true
func.func @select_no_fold_dynamic_on_true(%arg0: tensor<4xi32>) -> tensor<4xi32> {
  // CHECK: tosa.select
  %pred = "tosa.const"() {value = dense<[1, 0, 1, 0]> : tensor<4xi1>} : () -> tensor<4xi1>
  %b = "tosa.const"() {value = dense<[50, 60, 70, 80]> : tensor<4xi32>} : () -> tensor<4xi32>
  %0 = tosa.select %pred, %arg0, %b : (tensor<4xi1>, tensor<4xi32>, tensor<4xi32>) -> tensor<4xi32>
  return %0 : tensor<4xi32>
}
