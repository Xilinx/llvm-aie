// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold %s | FileCheck %s


// CHECK-LABEL: @fold
func.func @fold() -> tensor<2xi1> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}[false, true]
  // CHECK-NOT: tosa.logical_not
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<[true, false]> : tensor<2xi1>} : () -> tensor<2xi1>
  %1 = "tosa.logical_not"(%0) : (tensor<2xi1>) -> tensor<2xi1>
  return %1 : tensor<2xi1>
}
