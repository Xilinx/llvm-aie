// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold="fold-splat-or-single-use-only=0" %s | FileCheck %s
// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold="fold-splat-or-single-use-only=1" %s | FileCheck %s --check-prefix=ONLY-SINGLE-USE-CHECK

// CHECK-LABEL: @cast_fold_f32_to_i1_multiple_users
func.func @cast_fold_f32_to_i1_multiple_users() -> (tensor<3xf32>, tensor<3xi1>) {
  // CHECK-DAG: "tosa.const"() <{value = dense<[1.200000e+01, 4.000000e+00, 5.000000e+00]> : tensor<3xf32>}>
  // CHECK-DAG: "tosa.const"() <{value = dense<true> : tensor<3xi1>}>
  %0 = "tosa.const"() {value = dense<[12.0, 4.0, 5.0]> : tensor<3xf32>} : () -> tensor<3xf32>
  %1 = "tosa.cast"(%0) : (tensor<3xf32>) -> tensor<3xi1>
  // ONLY-SINGLE-USE-CHECK: tosa.cast
  return %0, %1 : tensor<3xf32>, tensor<3xi1>
}
