// RUN: mlir-opt --split-input-file -verify-diagnostics --tosa-layerwise-constant-fold %s | FileCheck %s

// Float additions

// CHECK-LABEL: @add_fold_float
func.func @add_fold_float() -> tensor<4xf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}-1.5{{.*}}e+02, 1.9{{.*}}e+00, 0.{{0*}}e+00, 5.{{0*}}e+00
  // CHECK-NOT: tosa.add
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[-17.4978, 4.9882, 0.0, -0.0]> :
                        tensor<4xf16>
                      } : () -> tensor<4xf16>
  %1 = "tosa.const"() {value =
                        dense<[-132.7, -3.0, -0.0, 5.0]> :
                        tensor<4xf16>
                      } : () -> tensor<4xf16>
  %2 = "tosa.add"(%0, %1) : (tensor<4xf16>, tensor<4xf16>) -> tensor<4xf16>
  return %2 : tensor<4xf16>
}

// CHECK-LABEL: @add_fold_float_infinity_nan
func.func @add_fold_float_infinity_nan() -> tensor<6xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0x7F800000, 0xFF800000, 0x7F800000, 0xFF800000, 0x7FC00000, 0x7FC00000
  // CHECK-NOT: tosa.add
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[0x7F800000, 0xFF800000, 0x7F800000, 0xFF800000, 0x7FC00000, 0x7F800000]> :
                        tensor<6xf32>
                      } : () -> tensor<6xf32>
  %1 = "tosa.const"() {value =
                        dense<[3.0, -3.0, -3.0, 3.0, 1.0, 0xFF800000]> :
                        tensor<6xf32>
                      } : () -> tensor<6xf32>
  %2 = "tosa.add"(%0, %1) : (tensor<6xf32>, tensor<6xf32>) -> tensor<6xf32>
  return %2 : tensor<6xf32>
}

// CHECK-LABEL: @add_fold_float_overflow
func.func @add_fold_float_overflow() -> tensor<2xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0x7F800000, 0xFF800000
  // CHECK-NOT: tosa.add
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[3.1e+38, -3.1e+38]> :
                        tensor<2xf32>
                      } : () -> tensor<2xf32>
  %1 = "tosa.const"() {value =
                        dense<[2.1e+38, -1.1e+38]> :
                        tensor<2xf32>
                      } : () -> tensor<2xf32>
  %2 = "tosa.add"(%0, %1) : (tensor<2xf32>, tensor<2xf32>) -> tensor<2xf32>
  return %2 : tensor<2xf32>
}

// -----
// Int additions

// CHECK-LABEL: @add_fold_int
func.func @add_fold_int() -> tensor<4xi32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}-149, 1, 0, 5
  // CHECK-NOT: tosa.add
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[-17, 4, 0, 0]> :
                        tensor<4xi32>
                      } : () -> tensor<4xi32>
  %1 = "tosa.const"() {value =
                        dense<[-132, -3, 0, 5]> :
                        tensor<4xi32>
                      } : () -> tensor<4xi32>
  %2 = "tosa.add"(%0, %1) : (tensor<4xi32>, tensor<4xi32>) -> tensor<4xi32>
  return %2 : tensor<4xi32>
}

// CHECK-LABEL: @add_fold_int_overflow
func.func @add_fold_int_overflow() -> tensor<4xi32> {
  // Don't expect any specific results for the overflowing addition, just
  // expect that it is folded.
  // CHECK: [[RES:]] ={{.*}}tosa.const
  // CHECK-NOT: tosa.add
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[2147483647, 2147483640, -2147483648, -2147483640]> :
                        tensor<4xi32>
                      } : () -> tensor<4xi32>
  %1 = "tosa.const"() {value =
                        dense<[1, 10, -1, -30]> :
                        tensor<4xi32>
                      } : () -> tensor<4xi32>
  // expected-warning@below {{Addition did overflow. The results are unspecified.}}
  %2 = "tosa.add"(%0, %1) : (tensor<4xi32>, tensor<4xi32>) -> tensor<4xi32>
  return %2 : tensor<4xi32>
}

// -----
// self-addition

// CHECK-LABEL: @add_fold_equal_args
func.func @add_fold_equal_args() -> tensor<3xi32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}-34, 8, 0
  // CHECK-NOT: tosa.add
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[-17, 4, 0]> :
                        tensor<3xi32>
                      } : () -> tensor<3xi32>
  %2 = "tosa.add"(%0, %0) : (tensor<3xi32>, tensor<3xi32>) -> tensor<3xi32>
  return %2 : tensor<3xi32>
}

// -----
// Broadcasted additions

// CHECK-LABEL: @add_fold_int_broadcast_simple
func.func @add_fold_int_broadcast_simple() -> tensor<3xi32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}-29, -8, -12
  // CHECK-NOT: tosa.add
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[-17, 4, 0]> :
                        tensor<3xi32>
                      } : () -> tensor<3xi32>
  %1 = "tosa.const"() {value =
                        dense<-12> :
                        tensor<1xi32>
                      } : () -> tensor<1xi32>
  %2 = "tosa.add"(%0, %1) : (tensor<3xi32>, tensor<1xi32>) -> tensor<3xi32>
  return %2 : tensor<3xi32>
}

// CHECK-LABEL: @add_fold_int_broadcast_complex
func.func @add_fold_int_broadcast_complex() -> tensor<3x3xi32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const
  // CHECK-SAME{LITERAL}: [[-29, -10, -13],
  // CHECK-SAME{LITERAL}:  [-11, 8, 5],
  // CHECK-SAME{LITERAL}:  [7, 26, 23]]
  // CHECK-NOT: tosa.add
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[[-17], [1], [19]]> :
                        tensor<3x1xi32>
                      } : () -> tensor<3x1xi32>
  %1 = "tosa.const"() {value =
                        dense<[[-12, 7, 4]]> :
                        tensor<1x3xi32>
                      } : () -> tensor<1x3xi32>
  %2 = "tosa.add"(%0, %1) : (tensor<3x1xi32>, tensor<1x3xi32>) -> tensor<3x3xi32>
  return %2 : tensor<3x3xi32>
}
