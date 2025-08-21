// RUN: mlir-opt --split-input-file -verify-diagnostics --tosa-layerwise-constant-fold %s | FileCheck %s

// Float greateritions

// CHECK-LABEL: @greater_fold_float
func.func @greater_fold_float() -> tensor<4xi1> {
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[true, true, false, false]>
  // CHECK-NOT: tosa.greater
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[-17.4978, 4.9882, 0.0, -0.0]> :
                        tensor<4xf16>
                      } : () -> tensor<4xf16>
  %1 = "tosa.const"() {value =
                        dense<[-132.7, -3.0, -0.0, 5.0]> :
                        tensor<4xf16>
                      } : () -> tensor<4xf16>
  %2 = "tosa.greater"(%0, %1) : (tensor<4xf16>, tensor<4xf16>) -> tensor<4xi1>
  return %2 : tensor<4xi1>
}

// CHECK-LABEL: @greater_fold_float_infinity_nan
func.func @greater_fold_float_infinity_nan() -> tensor<6xi1> {
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[true, false, true, false, false, false]>
  // CHECK-NOT: tosa.greater
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[0x7F800000, 0xFF800000, 0x7F800000, 0xFF800000, 0x7FC00000, 0xFF800000]> :
                        tensor<6xf32>
                      } : () -> tensor<6xf32>
  %1 = "tosa.const"() {value =
                        dense<[3.0, -3.0, -3.0, 3.0, 1.0, 0x7FC00000]> :
                        tensor<6xf32>
                      } : () -> tensor<6xf32>
  %2 = "tosa.greater"(%0, %1) : (tensor<6xf32>, tensor<6xf32>) -> tensor<6xi1>
  return %2 : tensor<6xi1>
}

// -----
// Int comparison

// CHECK-LABEL: @greater_fold_int
func.func @greater_fold_int() -> tensor<4xi1> {
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[true, true, false, false]>
  // CHECK-NOT: tosa.greater
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[-17, 4, 0, 0]> :
                        tensor<4xi32>
                      } : () -> tensor<4xi32>
  %1 = "tosa.const"() {value =
                        dense<[-132, -3, 0, 5]> :
                        tensor<4xi32>
                      } : () -> tensor<4xi32>
  %2 = "tosa.greater"(%0, %1) : (tensor<4xi32>, tensor<4xi32>) -> tensor<4xi1>
  return %2 : tensor<4xi1>
}

// -----
// Broadcasted

// CHECK-LABEL: @greater_fold_int_broadcast_simple
func.func @greater_fold_int_broadcast_simple() -> tensor<3xi1> {
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[false, true, true]>
  // CHECK-NOT: tosa.greater
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[-17, 4, 0]> :
                        tensor<3xi32>
                      } : () -> tensor<3xi32>
  %1 = "tosa.const"() {value =
                        dense<-12> :
                        tensor<1xi32>
                      } : () -> tensor<1xi32>
  %2 = "tosa.greater"(%0, %1) : (tensor<3xi32>, tensor<1xi32>) -> tensor<3xi1>
  return %2 : tensor<3xi1>
}

// CHECK-LABEL: @greater_fold_int_broadcast_complex
func.func @greater_fold_int_broadcast_complex() -> tensor<3x3xi1> {
  // CHECK: [[RES:]] ={{.*}}tosa.const
  // CHECK-SAME{LITERAL}: [[false, false, false]
  // CHECK-SAME{LITERAL}:  [true, false, false],
  // CHECK-SAME{LITERAL}:  [true, true, true]]
  // CHECK-NOT: tosa.greater
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[[-17], [1], [19]]> :
                        tensor<3x1xi32>
                      } : () -> tensor<3x1xi32>
  %1 = "tosa.const"() {value =
                        dense<[[-12, 7, 4]]> :
                        tensor<1x3xi32>
                      } : () -> tensor<1x3xi32>
  %2 = "tosa.greater"(%0, %1) : (tensor<3x1xi32>, tensor<1x3xi32>) -> tensor<3x3xi1>
  return %2 : tensor<3x3xi1>
}
