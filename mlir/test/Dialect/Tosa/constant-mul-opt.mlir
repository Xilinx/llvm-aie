// RUN: mlir-opt --split-input-file -verify-diagnostics --tosa-layerwise-constant-fold %s | FileCheck %s

// Float multiplications

// CHECK-LABEL: @mul_fold_float
func.func @mul_fold_float() -> tensor<4xf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}2.32{{.*}}e+03, -1.49{{.*}}e+01, -0.{{0*}}e+00, -0.{{0*}}e+00
  // CHECK-NOT: tosa.mul
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[-17.4978, 4.9882, 0.0, -0.0]> :
                        tensor<4xf16>
                      } : () -> tensor<4xf16>
  %1 = "tosa.const"() {value =
                        dense<[-132.7, -3.0, -0.0, 5.0]> :
                        tensor<4xf16>
                      } : () -> tensor<4xf16>
  %2 = "tosa.mul"(%0, %1) {shift = 0 : i8} : (tensor<4xf16>, tensor<4xf16>) -> tensor<4xf16>
  return %2 : tensor<4xf16>
}

// CHECK-LABEL: @mul_fold_float_infinity_nan
func.func @mul_fold_float_infinity_nan() -> tensor<7xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0x7F800000, 0x7F800000, 0xFF800000, 0xFF800000, 0x7FC00000, 0xFF800000, 0x7FC00000
  // CHECK-NOT: tosa.mul
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[0x7F800000, 0xFF800000, 0x7F800000, 0xFF800000, 0x7FC00000, 0x7F800000, 0xFF800000]> :
                        tensor<7xf32>
                      } : () -> tensor<7xf32>
  %1 = "tosa.const"() {value =
                        dense<[3.0, -3.0, -3.0, 3.0, 1.0, 0xFF800000, 0.0]> :
                        tensor<7xf32>
                      } : () -> tensor<7xf32>
  %2 = "tosa.mul"(%0, %1) {shift = 0 : i8} : (tensor<7xf32>, tensor<7xf32>) -> tensor<7xf32>
  return %2 : tensor<7xf32>
}

// CHECK-LABEL: @add_fold_float_overflow
func.func @add_fold_float_overflow() -> tensor<2xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0x7F800000, 0xFF800000
  // CHECK-NOT: tosa.mul
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[3.1e+38, -3.1e+38]> :
                        tensor<2xf32>
                      } : () -> tensor<2xf32>
  %1 = "tosa.const"() {value =
                        dense<[2.1e+38, 1.1e+38]> :
                        tensor<2xf32>
                      } : () -> tensor<2xf32>
  %2 = "tosa.mul"(%0, %1) {shift = 0 : i8} : (tensor<2xf32>, tensor<2xf32>) -> tensor<2xf32>
  return %2 : tensor<2xf32>
}

// -----
// Int multiplications

// CHECK-LABEL: @mul_fold_int
func.func @mul_fold_int() -> tensor<4xi32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}2244, -12, 0, 0
  // CHECK-NOT: tosa.mul
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[-17, 4, 0, 0]> :
                        tensor<4xi32>
                      } : () -> tensor<4xi32>
  %1 = "tosa.const"() {value =
                        dense<[-132, -3, 0, 5]> :
                        tensor<4xi32>
                      } : () -> tensor<4xi32>
  %2 = "tosa.mul"(%0, %1) {shift = 0 : i8} : (tensor<4xi32>, tensor<4xi32>) -> tensor<4xi32>
  return %2 : tensor<4xi32>
}

// CHECK-LABEL: @mul_fold_i8
func.func @mul_fold_i8() -> tensor<4xi32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}204, -12, 0, 0
  // CHECK-NOT: tosa.mul
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[-17, 4, -2, 0]> :
                        tensor<4xi8>
                      } : () -> tensor<4xi8>
  %1 = "tosa.const"() {value =
                        dense<[-12, -3, 0, 5]> :
                        tensor<4xi8>
                      } : () -> tensor<4xi8>
  // TODO: This is wrongly rejected as illegal, see https://reviews.llvm.org/D150472#4484478
  // %2 = "tosa.mul"(%0, %1) {shift = 0 : i8} : (tensor<4xi8>, tensor<4xi8>) -> tensor<4xi32>
  %a = "tosa.cast"(%0) : (tensor<4xi8>) -> tensor<4xi32>
  %b = "tosa.cast"(%1) : (tensor<4xi8>) -> tensor<4xi32>
  %2 = "tosa.mul"(%a, %b) {shift = 0 : i8} : (tensor<4xi32>, tensor<4xi32>) -> tensor<4xi32>

  return %2 : tensor<4xi32>
}

// CHECK-LABEL: @mul_fold_int_overflow
func.func @mul_fold_int_overflow() -> tensor<4xi32> {
  // Don't expect any specific results for the overflowing multiplication, just
  // that it is folded.
  // CHECK: [[RES:]] ={{.*}}tosa.const
  // CHECK-NOT: tosa.mul
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[2147483647, 2147483640, -2147483648, -2147483640]> :
                        tensor<4xi32>
                      } : () -> tensor<4xi32>
  %1 = "tosa.const"() {value =
                        dense<[1, 10, 1, 30]> :
                        tensor<4xi32>
                      } : () -> tensor<4xi32>
  // expected-warning@below {{Multiplication did overflow. The results are unspecified.}}
  %2 = "tosa.mul"(%0, %1) {shift = 0 : i8} : (tensor<4xi32>, tensor<4xi32>) -> tensor<4xi32>
  return %2 : tensor<4xi32>
}

// -----
// self-multiplication

// CHECK-LABEL: @mul_fold_equal_args
func.func @mul_fold_equal_args() -> tensor<3xi32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}289, 16, 0
  // CHECK-NOT: tosa.mul
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[-17, 4, 0]> :
                        tensor<3xi32>
                      } : () -> tensor<3xi32>
  %2 = "tosa.mul"(%0, %0) {shift = 0 : i8} : (tensor<3xi32>, tensor<3xi32>) -> tensor<3xi32>
  return %2 : tensor<3xi32>
}

// -----
// Broadcasted multiplications

// CHECK-LABEL: @mul_fold_int_broadcast_simple
func.func @mul_fold_int_broadcast_simple() -> tensor<3xi32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}204, -48, 0
  // CHECK-NOT: tosa.mul
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[-17, 4, 0]> :
                        tensor<3xi32>
                      } : () -> tensor<3xi32>
  %1 = "tosa.const"() {value =
                        dense<-12> :
                        tensor<1xi32>
                      } : () -> tensor<1xi32>
  %2 = "tosa.mul"(%0, %1) {shift = 0 : i8} : (tensor<3xi32>, tensor<1xi32>) -> tensor<3xi32>
  return %2 : tensor<3xi32>
}

// CHECK-LABEL: @mul_fold_int_broadcast_complex
func.func @mul_fold_int_broadcast_complex() -> tensor<3x3xi32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const
  // CHECK-SAME{LITERAL}: [[204, -119, -68],
  // CHECK-SAME{LITERAL}:  [-12, 7, 4],
  // CHECK-SAME{LITERAL}:  [-228, 133, 76]]
  // CHECK-NOT: tosa.mul
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[[-17], [1], [19]]> :
                        tensor<3x1xi32>
                      } : () -> tensor<3x1xi32>
  %1 = "tosa.const"() {value =
                        dense<[[-12, 7, 4]]> :
                        tensor<1x3xi32>
                      } : () -> tensor<1x3xi32>
  %2 = "tosa.mul"(%0, %1) {shift = 0 : i8} : (tensor<3x1xi32>, tensor<1x3xi32>) -> tensor<3x3xi32>
  return %2 : tensor<3x3xi32>
}

// CHECK-LABEL: @mul_fold_int_non_zero_shift
func.func @mul_fold_int_non_zero_shift() -> tensor<4xi32> {
  // CHECK: [[FIRST:]] ={{.*}}tosa.const
  // CHECK-NEXT: [[SECOND:]] ={{.*}}tosa.const
  // CHECK-NEXT: [[MUL:]] ={{.*}}tosa.mul{{.*}}[[FIRST]], [[SECOND]]
  // CHECK-NEXT: return [[MUL]]
  %0 = "tosa.const"() {value =
                        dense<[-17, 4, 0, 0]> :
                        tensor<4xi32>
                      } : () -> tensor<4xi32>
  %1 = "tosa.const"() {value =
                        dense<[-132, -3, 0, 5]> :
                        tensor<4xi32>
                      } : () -> tensor<4xi32>
  %2 = "tosa.mul"(%0, %1) {shift = 1 : i8} : (tensor<4xi32>, tensor<4xi32>) -> tensor<4xi32>
  return %2 : tensor<4xi32>
}
