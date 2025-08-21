// RUN: mlir-opt --split-input-file -verify-diagnostics --tosa-layerwise-constant-fold %s | FileCheck %s

// Float comparisons

// CHECK-LABEL: @equal_fold_float
func.func @equal_fold_float() -> tensor<4xi1> {
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[false, false, true, false]>
  // CHECK-NOT: tosa.equal
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[-17.4978, 4.9882, -0.0, -0.0]> :
                        tensor<4xf16>
                      } : () -> tensor<4xf16>
  %1 = "tosa.const"() {value =
                        dense<[-132.7, -3.0, -0.0, 1.0]> :
                        tensor<4xf16>
                      } : () -> tensor<4xf16>
  %2 = "tosa.equal"(%0, %1) : (tensor<4xf16>, tensor<4xf16>) -> tensor<4xi1>
  return %2 : tensor<4xi1>
}

// CHECK-LABEL: @equal_fold_float_infinity_nan
func.func @equal_fold_float_infinity_nan() -> tensor<6xi1> {
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[false, false, true, false, false, false]>
  // CHECK-NOT: tosa.equal
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[0x7F800000, 0xFF800000, 0x7F800000, 0xFF800000, 0x7FC00000, 0x7F800000]> :
                        tensor<6xf32>
                      } : () -> tensor<6xf32>
  %1 = "tosa.const"() {value =
                        dense<[0xFF800000, 0x7F800000, 0x7F800000, 0x7F800000, 1.0, 0xFF800000]> :
                        tensor<6xf32>
                      } : () -> tensor<6xf32>
  %2 = "tosa.equal"(%0, %1) : (tensor<6xf32>, tensor<6xf32>) -> tensor<6xi1>
  return %2 : tensor<6xi1>
}

// -----
// Int comparison

// CHECK-LABEL: @equal_fold_int
func.func @equal_fold_int() -> tensor<4xi1> {
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[true, false, true, false]>
  // CHECK-NOT: tosa.equal
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[-17, 4, 0, 0]> :
                        tensor<4xi32>
                      } : () -> tensor<4xi32>
  %1 = "tosa.const"() {value =
                        dense<[-17, -3, 0, 5]> :
                        tensor<4xi32>
                      } : () -> tensor<4xi32>
  %2 = "tosa.equal"(%0, %1) : (tensor<4xi32>, tensor<4xi32>) -> tensor<4xi1>
  return %2 : tensor<4xi1>
}

// -----
// Broadcasted

// CHECK-LABEL: @equal_fold_int_broadcast_simple
func.func @equal_fold_int_broadcast_simple() -> tensor<3xi1> {
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[true, false, false]>
  // CHECK-NOT: tosa.equal
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[-12, 4, 0]> :
                        tensor<3xi32>
                      } : () -> tensor<3xi32>
  %1 = "tosa.const"() {value =
                        dense<-12> :
                        tensor<1xi32>
                      } : () -> tensor<1xi32>
  %2 = "tosa.equal"(%0, %1) : (tensor<3xi32>, tensor<1xi32>) -> tensor<3xi1>
  return %2 : tensor<3xi1>
}

// CHECK-LABEL: @equal_fold_int_broadcast_complex
func.func @equal_fold_int_broadcast_complex() -> tensor<3x3xi1> {
  // CHECK: [[RES:]] ={{.*}}tosa.const
  // CHECK-SAME{LITERAL}: [[true, false, false]
  // CHECK-SAME{LITERAL}:  [false, true, false],
  // CHECK-SAME{LITERAL}:  [false, false, true]]
  // CHECK-NOT: tosa.equal
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[[-12], [1], [4]]> :
                        tensor<3x1xi32>
                      } : () -> tensor<3x1xi32>
  %1 = "tosa.const"() {value =
                        dense<[[-12, 1, 4]]> :
                        tensor<1x3xi32>
                      } : () -> tensor<1x3xi32>
  %2 = "tosa.equal"(%0, %1) : (tensor<3x1xi32>, tensor<1x3xi32>) -> tensor<3x3xi1>
  return %2 : tensor<3x3xi1>
}
