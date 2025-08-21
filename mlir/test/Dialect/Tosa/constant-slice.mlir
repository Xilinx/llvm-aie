// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold %s | FileCheck %s

// CHECK-LABEL: @slice_int8
func.func @slice_int8() -> (tensor<1x1xi8>) {
  // CHECK: "tosa.const"() <{value = dense<3>
  %0 = "tosa.const"() {value = dense<[[3, 4], [5, 6]]> : tensor<2x2xi8>} : () -> tensor<2x2xi8>
  %1 = "tosa.slice"(%0){size = array<i64: 1, 1>, start = array<i64: 0, 0>} : (tensor<2x2xi8>) -> tensor<1x1xi8>
  return %1 : tensor<1x1xi8>
}

func.func @slice_int16() -> (tensor<2x1xi16>) {
  // CHECK: "tosa.const"() <{value = dense<{{\[\[}}3], [5]]>
  %0 = "tosa.const"() {value = dense<[[3, 4], [5, 6]]> : tensor<2x2xi16>} : () -> tensor<2x2xi16>
  %1 = "tosa.slice"(%0){size = array<i64: 2, 1>, start = array<i64: 0, 0>} : (tensor<2x2xi16>) -> tensor<2x1xi16>
  return %1 : tensor<2x1xi16>
}

// CHECK-LABEL: @slice_int32
func.func @slice_int32() -> (tensor<2x1xi32>) {
  // CHECK: "tosa.const"() <{value = dense<{{\[\[}}4], [6]]>
  %0 = "tosa.const"() {value = dense<[[3, 4], [5, 6]]> : tensor<2x2xi32>} : () -> tensor<2x2xi32>
  %1 = "tosa.slice"(%0){size = array<i64: 2, 1>, start = array<i64: 0, 1>} : (tensor<2x2xi32>) -> tensor<2x1xi32>
  return %1 : tensor<2x1xi32>
}

// CHECK-LABEL: @slice_int32_default_value
func.func @slice_int32_default_value() -> (tensor<3x1xi32>) {
  // CHECK: "tosa.const"() <{value = dense<{{\[\[}}3], [6], [9]]>
  %0 = "tosa.const"() {value = dense<[[3, 4, 5], [6, 7, 8], [9, 10, 11]]> : tensor<3x3xi32>} : () -> tensor<3x3xi32>
  %1 = "tosa.slice"(%0){size = array<i64: 3, 1>, start = array<i64: 0, 0>} : (tensor<3x3xi32>) -> tensor<3x1xi32>
  return %1 : tensor<3x1xi32>
}

// CHECK-LABEL: @slice_bf16_default_value
func.func @slice_bf16_default_value() -> (tensor<3x2xbf16>) {
  // CHECK: "tosa.const"() <{value = dense<{{\[\[}}4.000000e+00, 5.000000e+00], [7.000000e+00, 8.000000e+00], [1.000000e+01, 1.100000e+01]]>
  %0 = "tosa.const"() {value = dense<[[3.0, 4.0, 5.0], [6.0, 7.0, 8.0], [9.0, 10.0, 11.0]]> : tensor<3x3xbf16>} : () -> tensor<3x3xbf16>
  %1 = "tosa.slice"(%0){size = array<i64: 3, 2>, start = array<i64: 0, 1>} : (tensor<3x3xbf16>) -> tensor<3x2xbf16>
  return %1 : tensor<3x2xbf16>
}

// -----

// Following tests are all done with the following tensor, and different configurations:
// [[[1.0 ,  2.25  ,  3.50 , 4.75],
// [ 5.0 ,  6.25  ,  7.50 , 8.75]],
// [[ 13.0 ,  14.25  ,  15.50 , 16.75 ],
// [ 17.0 ,  18.25  ,  19.50 , 20.75]],
// [[-1.0 ,  -2.25  ,  -3.50 , -4.75],
// [ -5.0 ,  -6.25  ,  -7.50 , -8.75]],
// [[ -13.0 ,  -14.25  ,  -15.50 , -16.75 ],
// [ -17.0 ,  -18.25  ,  -19.50 , -20.75]]]

// Should produce
// 1.0, 2.25, 3.50, 4.75,
// 13.0, 14.25, 15.50, 16.75,
// -1.0, -2.25, -3.50, -4.75,
// -13.0, -14.25, -15.50, -16.75
func.func @slice_bf16_dim_1_start_zero() -> (tensor<4x1x4xbf16>) {
// CHECK-LABEL: @slice_bf16_dim_1_start_zero
// CHECK: 1.000000e+00, 2.250000e+00, 3.500000e+00, 4.750000e+00
// CHECK-SAME: 1.300000e+01, 1.425000e+01, 1.550000e+01, 1.675000e+01
// CHECK-SAME: -1.000000e+00, -2.250000e+00, -3.500000e+00, -4.750000e+00
// CHECK-SAME: -1.300000e+01, -1.425000e+01, -1.550000e+01, -1.675000e+01
  %0 = "tosa.const"() {value = dense<[[[1.0, 2.25, 3.50, 4.75], [ 5.0, 6.25, 7.50, 8.75]], [[ 13.0, 14.25, 15.50, 16.75 ], [ 17.0, 18.25, 19.50, 20.75]], [[-1.0, -2.25, -3.50, -4.75], [ -5.0, -6.25, -7.50, -8.75]], [[ -13.0, -14.25, -15.50, -16.75 ], [ -17.0, -18.25, -19.50, -20.75]]]> : tensor<4x2x4xbf16>} : () -> tensor<4x2x4xbf16>
  %1 = "tosa.slice"(%0){size = array<i64: 4, 1, 4>, start = array<i64: 0, 0, 0>} : (tensor<4x2x4xbf16>) -> tensor<4x1x4xbf16>
  return %1 : tensor<4x1x4xbf16>
}

// Should produce
// 1.0, 2.25, 3.50, 4.75,
// 13.0, 14.25, 15.50, 16.75,
// -1.0, -2.25, -3.50, -4.75,
// -13.0, -14.25, -15.50, -16.75
func.func @slice_f16_dim_1_start_zero() -> (tensor<4x1x4xf16>) {
// CHECK-LABEL: @slice_f16_dim_1_start_zero
// CHECK: 1.000000e+00, 2.250000e+00, 3.500000e+00, 4.750000e+00
// CHECK-SAME: 1.300000e+01, 1.425000e+01, 1.550000e+01, 1.675000e+01
// CHECK-SAME: -1.000000e+00, -2.250000e+00, -3.500000e+00, -4.750000e+00
// CHECK-SAME: -1.300000e+01, -1.425000e+01, -1.550000e+01, -1.675000e+01
  %0 = "tosa.const"() {value = dense<[[[1.0, 2.25, 3.50, 4.75], [ 5.0, 6.25, 7.50, 8.75]], [[ 13.0, 14.25, 15.50, 16.75 ], [ 17.0, 18.25, 19.50, 20.75]], [[-1.0, -2.25, -3.50, -4.75], [ -5.0, -6.25, -7.50, -8.75]], [[ -13.0, -14.25, -15.50, -16.75 ], [ -17.0, -18.25, -19.50, -20.75]]]> : tensor<4x2x4xf16>} : () -> tensor<4x2x4xf16>
  %1 = "tosa.slice"(%0){size = array<i64: 4, 1, 4>, start = array<i64: 0, 0, 0>} : (tensor<4x2x4xf16>) -> tensor<4x1x4xf16>
  return %1 : tensor<4x1x4xf16>
}

// Should produce
// 5.0, 6.25, 7.50, 8.75
// 17.0, 18.25, 19.50, 20.75
// -5.0, -6.25, -7.50, -8.75
// -17.0, -18.25, -19.50, -20.75
func.func @slice_bf16_start_dim_1_start_one() -> (tensor<4x1x4xbf16>) {
// CHECK-LABEL: @slice_bf16_start_dim_1_start_one
// CHECK: 5.000000e+00, 6.250000e+00, 7.500000e+00, 8.750000e+00
// CHECK-SAME: 1.700000e+01, 1.825000e+01, 1.950000e+01, 2.075000e+01
// CHECK-SAME: -5.000000e+00, -6.250000e+00, -7.500000e+00, -8.750000e+00
// CHECK-SAME: -1.700000e+01, -1.825000e+01, -1.950000e+01, -2.075000e+01
  %0 = "tosa.const"() {value = dense<[[[1.0, 2.25, 3.50, 4.75], [ 5.0, 6.25, 7.50, 8.75]], [[ 13.0, 14.25, 15.50, 16.75 ], [ 17.0, 18.25, 19.50, 20.75]], [[-1.0, -2.25, -3.50, -4.75], [ -5.0, -6.25, -7.50, -8.75]], [[ -13.0, -14.25, -15.50, -16.75 ], [ -17.0, -18.25, -19.50, -20.75]]]> : tensor<4x2x4xbf16>} : () -> tensor<4x2x4xbf16>
  %1 = "tosa.slice"(%0){size = array<i64: 4, 1, 4>, start = array<i64: 0, 1, 0>} : (tensor<4x2x4xbf16>) -> tensor<4x1x4xbf16>
  return %1 : tensor<4x1x4xbf16>
}

// Should produce
// 5.0, 6.25, 7.50, 8.75
// 17.0, 18.25, 19.50, 20.75
// -5.0, -6.25, -7.50, -8.75
// -17.0, -18.25, -19.50, -20.75
func.func @slice_f16_start_dim_1_start_one() -> (tensor<4x1x4xf16>) {
// CHECK-LABEL: @slice_f16_start_dim_1_start_one
// CHECK: 5.000000e+00, 6.250000e+00, 7.500000e+00, 8.750000e+00
// CHECK-SAME: 1.700000e+01, 1.825000e+01, 1.950000e+01, 2.075000e+01
// CHECK-SAME: -5.000000e+00, -6.250000e+00, -7.500000e+00, -8.750000e+00
// CHECK-SAME: -1.700000e+01, -1.825000e+01, -1.950000e+01, -2.075000e+01
  %0 = "tosa.const"() {value = dense<[[[1.0, 2.25, 3.50, 4.75], [ 5.0, 6.25, 7.50, 8.75]], [[ 13.0, 14.25, 15.50, 16.75 ], [ 17.0, 18.25, 19.50, 20.75]], [[-1.0, -2.25, -3.50, -4.75], [ -5.0, -6.25, -7.50, -8.75]], [[ -13.0, -14.25, -15.50, -16.75 ], [ -17.0, -18.25, -19.50, -20.75]]]> : tensor<4x2x4xf16>} : () -> tensor<4x2x4xf16>
  %1 = "tosa.slice"(%0){size = array<i64: 4, 1, 4>, start = array<i64: 0, 1, 0>} : (tensor<4x2x4xf16>) -> tensor<4x1x4xf16>
  return %1 : tensor<4x1x4xf16>
}

// Should produce
// 1.0, 2.25, 3.50
// 13.0, 14.25, 15.50
// -1.0, -2.25, -3.50
func.func @slice_bf16_start_zero_multiple_dims() -> (tensor<3x1x3xbf16>) {
// CHECK-LABEL: @slice_bf16_start_zero_multiple_dims
// CHECK: 1.000000e+00, 2.250000e+00, 3.500000e+00
// CHECK-SAME: 1.300000e+01, 1.425000e+01, 1.550000e+01
// CHECK-SAME: -1.000000e+00, -2.250000e+00, -3.500000e+00
  %0 = "tosa.const"() {value = dense<[[[1.0, 2.25, 3.50, 4.75], [ 5.0, 6.25, 7.50, 8.75]], [[ 13.0, 14.25, 15.50, 16.75 ], [ 17.0, 18.25, 19.50, 20.75]], [[-1.0, -2.25, -3.50, -4.75], [ -5.0, -6.25, -7.50, -8.75]], [[ -13.0, -14.25, -15.50, -16.75 ], [ -17.0, -18.25, -19.50, -20.75]]]> : tensor<4x2x4xbf16>} : () -> tensor<4x2x4xbf16>
  %1 = "tosa.slice"(%0){size = array<i64: 3, 1, 3>, start = array<i64: 0, 0, 0>} : (tensor<4x2x4xbf16>) -> tensor<3x1x3xbf16>
  return %1 : tensor<3x1x3xbf16>
}

// Should produce
// 1.0, 2.25, 3.50
// 13.0, 14.25, 15.50
// -1.0, -2.25, -3.50
func.func @slice_f16_start_zero_multiple_dims() -> (tensor<3x1x3xf16>) {
// CHECK-LABEL: @slice_f16_start_zero_multiple_dims
// CHECK: 1.000000e+00, 2.250000e+00, 3.500000e+00
// CHECK-SAME: 1.300000e+01, 1.425000e+01, 1.550000e+01
// CHECK-SAME: -1.000000e+00, -2.250000e+00, -3.500000e+00
  %0 = "tosa.const"() {value = dense<[[[1.0, 2.25, 3.50, 4.75], [ 5.0, 6.25, 7.50, 8.75]], [[ 13.0, 14.25, 15.50, 16.75 ], [ 17.0, 18.25, 19.50, 20.75]], [[-1.0, -2.25, -3.50, -4.75], [ -5.0, -6.25, -7.50, -8.75]], [[ -13.0, -14.25, -15.50, -16.75 ], [ -17.0, -18.25, -19.50, -20.75]]]> : tensor<4x2x4xf16>} : () -> tensor<4x2x4xf16>
  %1 = "tosa.slice"(%0){size = array<i64: 3, 1, 3>, start = array<i64: 0, 0, 0>} : (tensor<4x2x4xf16>) -> tensor<3x1x3xf16>
  return %1 : tensor<3x1x3xf16>
}

// Produces
// 18.25, 19.50, 20.75
// -6.25, -7.50, -8.75
// -18.25, -19.50, -20.75
func.func @slice_bf16_start_non_zero_multiple_dims() -> (tensor<3x1x3xbf16>) {
// CHECK-LABEL: @slice_bf16_start_non_zero_multiple_dims
// CHECK: 1.825000e+01, 1.950000e+01, 2.075000e+01
// CHECK-SAME: -6.250000e+00, -7.500000e+00, -8.750000e+00
// CHECK-SAME: -1.825000e+01, -1.950000e+01, -2.075000e+01
  %0 = "tosa.const"() {value = dense<[[[1.0, 2.25, 3.50, 4.75], [ 5.0, 6.25, 7.50, 8.75]], [[ 13.0, 14.25, 15.50, 16.75 ], [ 17.0, 18.25, 19.50, 20.75]], [[-1.0, -2.25, -3.50, -4.75], [ -5.0, -6.25, -7.50, -8.75]], [[ -13.0, -14.25, -15.50, -16.75 ], [ -17.0, -18.25, -19.50, -20.75]]]> : tensor<4x2x4xbf16>} : () -> tensor<4x2x4xbf16>
  %1 = "tosa.slice"(%0){size = array<i64: 3, 1, 3>, start = array<i64: 1, 1, 1>} : (tensor<4x2x4xbf16>) -> tensor<3x1x3xbf16>
  return %1 : tensor<3x1x3xbf16>
}

// Produces
// 18.25, 19.50, 20.75
// -6.25, -7.50, -8.75
// -18.25, -19.50, -20.75
func.func @slice_f16_start_non_zero_multiple_dims() -> (tensor<3x1x3xf16>) {
// CHECK-LABEL: @slice_f16_start_non_zero_multiple_dims
// CHECK: 1.825000e+01, 1.950000e+01, 2.075000e+01
// CHECK-SAME: -6.250000e+00, -7.500000e+00, -8.750000e+00
// CHECK-SAME: -1.825000e+01, -1.950000e+01, -2.075000e+01
  %0 = "tosa.const"() {value = dense<[[[1.0, 2.25, 3.50, 4.75], [ 5.0, 6.25, 7.50, 8.75]], [[ 13.0, 14.25, 15.50, 16.75 ], [ 17.0, 18.25, 19.50, 20.75]], [[-1.0, -2.25, -3.50, -4.75], [ -5.0, -6.25, -7.50, -8.75]], [[ -13.0, -14.25, -15.50, -16.75 ], [ -17.0, -18.25, -19.50, -20.75]]]> : tensor<4x2x4xf16>} : () -> tensor<4x2x4xf16>
  %1 = "tosa.slice"(%0){size = array<i64: 3, 1, 3>, start = array<i64: 1, 1, 1>} : (tensor<4x2x4xf16>) -> tensor<3x1x3xf16>
  return %1 : tensor<3x1x3xf16>
}