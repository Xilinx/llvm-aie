// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold="fold-splat-or-single-use-only=0" %s | FileCheck %s
// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold="fold-splat-or-single-use-only=1" %s | FileCheck %s --check-prefix=ONLY-SINGLE-USE-CHECK

// CHECK-LABEL: @slice_bf16
func.func @slice_bf16() -> (tensor<3x3xbf16>, tensor<3x2xbf16>) {
  // CHECK-DAG: "tosa.const"() <{value = dense<{{\[\[}}3.000000e+00, 4.000000e+00, 5.000000e+00], [6.000000e+00, 7.000000e+00, 8.000000e+00], [9.000000e+00, 1.000000e+01, 1.100000e+01]]>
  // CHECK-DAG: "tosa.const"() <{value = dense<{{\[\[}}4.000000e+00, 5.000000e+00], [7.000000e+00, 8.000000e+00], [1.000000e+01, 1.100000e+01]]>
  // ONLY-SINGLE-USE-CHECK: tosa.slice
  %0 = "tosa.const"() {value = dense<[[3.0, 4.0, 5.0], [6.0, 7.0, 8.0], [9.0, 10.0, 11.0]]> : tensor<3x3xbf16>} : () -> tensor<3x3xbf16>
  %1 = "tosa.slice"(%0){size = array<i64: 3, 2>, start = array<i64: 0, 1>} : (tensor<3x3xbf16>) -> tensor<3x2xbf16>
  return %0, %1 : tensor<3x3xbf16>, tensor<3x2xbf16>
}

