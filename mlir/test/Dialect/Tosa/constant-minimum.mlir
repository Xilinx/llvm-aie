// RUN: mlir-opt --split-input-file -verify-diagnostics --tosa-layerwise-constant-fold %s | FileCheck %s

// CHECK-LABEL: @minimum_fold_float
func.func @minimum_fold_float() -> tensor<4xf16> {
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[-1.327500e+02, -3.000000e+00, -1.000000e+00, -0.000000e+00]>
  // CHECK-NOT: tosa.minimum
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[-17.4978, 4.9882, -1.0, -0.0]> :
                        tensor<4xf16>
                      } : () -> tensor<4xf16>
  %1 = "tosa.const"() {value =
                        dense<[-132.7, -3.0, -0.0, 1.0]> :
                        tensor<4xf16>
                      } : () -> tensor<4xf16>
  %2 = "tosa.minimum"(%0, %1) : (tensor<4xf16>, tensor<4xf16>) -> tensor<4xf16>
  return %2 : tensor<4xf16>
}

// CHECK-LABEL: @minimum_fold_float_infinity_nan
func.func @minimum_fold_float_infinity_nan() -> tensor<6xf32> {
  // Any comparison with NAN results in NAN
  // 0x7FC00000 is the value for NAN
  // 0x7F800000 is the value for Inf
  // 0xFF800000 is the value for -Inf
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[3.000000e+00, 0xFF800000, -3.000000e+00, 0xFF800000, 0x7FC00000, 0x7FC00000]>
  // CHECK-NOT: tosa.minimum
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[0x7F800000, 0xFF800000, 0x7F800000, 0xFF800000, 0x7FC00000, 0xFF800000]> :
                        tensor<6xf32>
                      } : () -> tensor<6xf32>
  %1 = "tosa.const"() {value =
                        dense<[3.0, -3.0, -3.0, 3.0, 1.0, 0x7FC00000]> :
                        tensor<6xf32>
                      } : () -> tensor<6xf32>
  %2 = "tosa.minimum"(%0, %1) : (tensor<6xf32>, tensor<6xf32>) -> tensor<6xf32>
  return %2 : tensor<6xf32>
}

// -----

// CHECK-LABEL: @minimum_fold_int
func.func @minimum_fold_int() -> tensor<4xi32> {
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[-132, -3, 0, 0]>
  // CHECK-NOT: tosa.minimum
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[-17, 4, 0, 0]> :
                        tensor<4xi32>
                      } : () -> tensor<4xi32>
  %1 = "tosa.const"() {value =
                        dense<[-132, -3, 0, 5]> :
                        tensor<4xi32>
                      } : () -> tensor<4xi32>
  %2 = "tosa.minimum"(%0, %1) : (tensor<4xi32>, tensor<4xi32>) -> tensor<4xi32>
  return %2 : tensor<4xi32>
}

// -----
// Broadcasted

// CHECK-LABEL: @minimum_fold_int_broadcast_simple
func.func @minimum_fold_int_broadcast_simple() -> tensor<3xi32> {
  // CHECK: [[RES:]] = "tosa.const"() <{value = dense<[-17, -12, -12]>
  // CHECK-NOT: tosa.minimum
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[-17, 4, 0]> :
                        tensor<3xi32>
                      } : () -> tensor<3xi32>
  %1 = "tosa.const"() {value =
                        dense<-12> :
                        tensor<1xi32>
                      } : () -> tensor<1xi32>
  %2 = "tosa.minimum"(%0, %1) : (tensor<3xi32>, tensor<1xi32>) -> tensor<3xi32>
  return %2 : tensor<3xi32>
}

// CHECK-LABEL: @minimum_fold_int_broadcast_complex
func.func @minimum_fold_int_broadcast_complex() -> tensor<3x3xi32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const
  // CHECK-SAME{LITERAL}: [[-17, -17, -17], [-12, 1, 1], [-12, 7, 4]]
  // CHECK-NOT: tosa.minimum
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[[-17], [1], [19]]> :
                        tensor<3x1xi32>
                      } : () -> tensor<3x1xi32>
  %1 = "tosa.const"() {value =
                        dense<[[-12, 7, 4]]> :
                        tensor<1x3xi32>
                      } : () -> tensor<1x3xi32>
  %2 = "tosa.minimum"(%0, %1) : (tensor<3x1xi32>, tensor<1x3xi32>) -> tensor<3x3xi32>
  return %2 : tensor<3x3xi32>
}
