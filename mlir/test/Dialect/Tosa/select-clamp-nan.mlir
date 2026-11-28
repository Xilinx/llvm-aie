// RUN: mlir-opt --split-input-file -canonicalize="test-convergence" %s | FileCheck %s --implicit-check-not=tosa.clamp

// A NaN input makes the ordered comparison false, so this select returns the
// finite bound. A clamp with NaN propagation would instead return NaN.
// CHECK-LABEL: func.func @preserve_fp_lower_bound_select(
// CHECK-SAME: %[[INPUT:.*]]: tensor<4xf32>
// CHECK-DAG: %[[BOUND:.*]] = "tosa.const"() {{.*}}dense<1.500000e+00>
// CHECK-DAG: %[[PRED:.*]] = tosa.greater_equal %[[INPUT]], %[[BOUND]]
// CHECK-DAG: %[[RESULT:.*]] = tosa.select %[[PRED]], %[[INPUT]], %[[BOUND]]
// CHECK: return %[[RESULT]]
func.func @preserve_fp_lower_bound_select(%input: tensor<4xf32>) -> tensor<4xf32> {
  %bound = "tosa.const"() <{value = dense<1.5> : tensor<4xf32>}> : () -> tensor<4xf32>
  %pred = tosa.greater_equal %input, %bound : (tensor<4xf32>, tensor<4xf32>) -> tensor<4xi1>
  %result = tosa.select %pred, %input, %bound : (tensor<4xi1>, tensor<4xf32>, tensor<4xf32>) -> tensor<4xf32>
  return %result : tensor<4xf32>
}

// -----

// The floating-point restriction also preserves the reversed selection order.
// CHECK-LABEL: func.func @preserve_fp_upper_bound_select(
// CHECK-SAME: %[[INPUT:.*]]: tensor<4xf32>
// CHECK-DAG: %[[BOUND:.*]] = "tosa.const"() {{.*}}dense<1.500000e+00>
// CHECK-DAG: %[[PRED:.*]] = tosa.greater_equal %[[INPUT]], %[[BOUND]]
// CHECK-DAG: %[[RESULT:.*]] = tosa.select %[[PRED]], %[[BOUND]], %[[INPUT]]
// CHECK: return %[[RESULT]]
func.func @preserve_fp_upper_bound_select(%input: tensor<4xf32>) -> tensor<4xf32> {
  %bound = "tosa.const"() <{value = dense<1.5> : tensor<4xf32>}> : () -> tensor<4xf32>
  %pred = tosa.greater_equal %input, %bound : (tensor<4xf32>, tensor<4xf32>) -> tensor<4xi1>
  %result = tosa.select %pred, %bound, %input : (tensor<4xi1>, tensor<4xf32>, tensor<4xf32>) -> tensor<4xf32>
  return %result : tensor<4xf32>
}
