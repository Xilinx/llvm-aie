// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold="fold-splat-or-single-use-only=0" %s | FileCheck %s
// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold="fold-splat-or-single-use-only=1" %s | FileCheck %s --check-prefix=ONLY-SINGLE-USE-CHECK

// CHECK-LABEL: @transpose_bf16_multi_users
func.func @transpose_bf16_multi_users() -> (tensor<2x3xbf16>, tensor<3x2xbf16>) {
  %input = "tosa.const"() {value = dense<[[0.0, 1.0, 2.0], [3.0, 4.0, 5.0]]> : tensor<2x3xbf16>} : () -> tensor<2x3xbf16>
  %perms = "tosa.const"() {value = dense<[1, 0]> : tensor<2xi32>} : () -> tensor<2xi32>
  // CHECK-DAG: "tosa.const"() <{value = dense<{{\[\[}}0.000000e+00, 1.000000e+00, 2.000000e+00], [3.000000e+00, 4.000000e+00, 5.000000e+00]]>
  // CHECK-DAG: "tosa.const"() <{value = dense<{{\[\[}}0.000000e+00, 3.000000e+00], [1.000000e+00, 4.000000e+00], [2.000000e+00, 5.000000e+00]]>
  %1 = "tosa.transpose"(%input, %perms) : (tensor<2x3xbf16>, tensor<2xi32>) -> tensor<3x2xbf16>
  // ONLY-SINGLE-USE-CHECK: tosa.transpose
  return %input, %1 : tensor<2x3xbf16>, tensor<3x2xbf16>
}
