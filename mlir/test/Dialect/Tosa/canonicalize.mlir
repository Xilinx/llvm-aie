// Modifications (c) Copyright 2026 Advanced Micro Devices, Inc. or its
// affiliates

// RUN: mlir-opt --split-input-file -canonicalize="test-convergence" %s | FileCheck %s

// CHECK-LABEL: @argmax_nofold
func.func @argmax_nofold(%arg0: tensor<?x1xf32>) -> tensor<?x1xi32> {
  // CHECK: tosa.argmax
  %0 = tosa.argmax %arg0 {axis = 0 : i32}: (tensor<?x1xf32>) -> tensor<?x1xi32>
  return %0 : tensor<?x1xi32>
}

// -----

// CHECK-LABEL: @add_bcast_zero_int
func.func @add_bcast_zero_int(%arg0: tensor<4x2x3xi32>) -> tensor<4x2x3xi32> {
  // CHECK-NOT: tosa.add
  // CHECK: return %arg0
  %zeros = "tosa.const"() {value = dense<0> : tensor<1x1x1xi32>} : () -> tensor<1x1x1xi32>
  %1 = tosa.add %arg0, %zeros : (tensor<4x2x3xi32>, tensor<1x1x1xi32>) -> tensor<4x2x3xi32>
  return %1 : tensor<4x2x3xi32>
}

// -----

// CHECK-LABEL: @add_zero_int
func.func @add_zero_int(%arg0: tensor<2x3xi32>) -> tensor<2x3xi32> {
  // CHECK: return %arg0
  // CHECK-NOT: tosa.add
  %zeros = "tosa.const"() {value = dense<0> : tensor<2x3xi32>} : () -> tensor<2x3xi32>
  %1 = tosa.add %arg0, %zeros : (tensor<2x3xi32>, tensor<2x3xi32>) -> tensor<2x3xi32>
  return %1 : tensor<2x3xi32>
}

// -----

// CHECK-LABEL: @cast_fold
func.func @cast_fold(%arg0: tensor<?x1xf32>) -> tensor<?x1xf32> {
  // CHECK: return %arg0
  %0 = tosa.cast %arg0 : (tensor<?x1xf32>) -> tensor<?x1xf32>
  return %0 : tensor<?x1xf32>
}

// -----

// CHECK-LABEL: @cast_nofold
func.func @cast_nofold(%arg0: tensor<?x1xf32>) -> tensor<?x1xi32> {
  // CHECK: tosa.cast
  %0 = tosa.cast %arg0 : (tensor<?x1xf32>) -> tensor<?x1xi32>
  return %0 : tensor<?x1xi32>
}

// CHECK-LABEL: @cast_fold_double
func.func @cast_fold_double(%arg0: tensor<?x1xf32>) -> tensor<?x1xi8> {
  // CHECK: tosa.cast{{.*}} (tensor<?x1xf32>) -> tensor<?x1xi8>
  %0 = tosa.cast %arg0 : (tensor<?x1xf32>) -> tensor<?x1xi16>
  %1 = tosa.cast %0 : (tensor<?x1xi16>) -> tensor<?x1xi8>
  return %1 : tensor<?x1xi8>
}

// CHECK-LABEL: @cast_fold_double
func.func @cast_fold_double2(%arg0: tensor<?x1xbf16>) -> tensor<?x1xbf16> {
  // CHECK: return %arg0
  %0 = tosa.cast %arg0 : (tensor<?x1xbf16>) -> tensor<?x1xf32>
  %1 = tosa.cast %0 : (tensor<?x1xf32>) -> tensor<?x1xbf16>
  return %1 : tensor<?x1xbf16>
}

// CHECK-LABEL: @cast_no_fold_double1
func.func @cast_no_fold_double1(%arg0: tensor<?x1xf32>) -> tensor<?x1xi8> {
  // CHECK: tosa.cast{{.*}} (tensor<?x1xf32>) -> tensor<?x1xui16>
  // CHECK: tosa.cast{{.*}} (tensor<?x1xui16>) -> tensor<?x1xi8>
  %0 = tosa.cast %arg0 : (tensor<?x1xf32>) -> tensor<?x1xui16>
  %1 = tosa.cast %0 : (tensor<?x1xui16>) -> tensor<?x1xi8>
  return %1 : tensor<?x1xi8>
}

// CHECK-LABEL: @cast_no_fold_double2
func.func @cast_no_fold_double2(%arg0: tensor<?x1xf32>) -> tensor<?x1xi16> {
  // CHECK: tosa.cast{{.*}} (tensor<?x1xf32>) -> tensor<?x1xi8>
  // CHECK: tosa.cast{{.*}} (tensor<?x1xi8>) -> tensor<?x1xi16>
  %0 = tosa.cast %arg0 : (tensor<?x1xf32>) -> tensor<?x1xi8>
  %1 = tosa.cast %0 : (tensor<?x1xi8>) -> tensor<?x1xi16>
  return %1 : tensor<?x1xi16>
}

// -----

// CHECK-LABEL: @clamp_i32_not_noop
func.func @clamp_i32_not_noop(%arg0: tensor<4xi32>) -> tensor<4xi32> {
  // CHECK: tosa.clamp
  %0 = tosa.clamp %arg0 {min_int = 1 : i64, max_int = 4 : i64, min_fp = 1.0 : f32, max_fp = 4.0 : f32} : (tensor<4xi32>) -> tensor<4xi32>
  return %0 : tensor<4xi32>
}

// -----

// CHECK-LABEL: @clamp_f16_not_noop
func.func @clamp_f16_not_noop(%arg0: tensor<4xf16>) -> tensor<4xf16> {
  // CHECK: tosa.clamp
  %0 = tosa.clamp %arg0 {min_int = -128 : i64, max_int = 127 : i64, min_fp = -3.40282347E+38 : f32, max_fp = 3.40282347E+38 : f32} : (tensor<4xf16>) -> tensor<4xf16>
  return %0 : tensor<4xf16>
}

// -----

// CHECK-LABEL: @clamp_f32_not_noop
func.func @clamp_f32_not_noop(%arg0: tensor<4xf32>) -> tensor<4xf32> {
  // CHECK: tosa.clamp
  %0 = tosa.clamp %arg0 {min_int = -128 : i64, max_int = 127 : i64, min_fp = -3.40282347E+38 : f32, max_fp = 3.40282347E+38 : f32} : (tensor<4xf32>) -> tensor<4xf32>
  return %0 : tensor<4xf32>
}

// -----

// CHECK-LABEL: @clamp_f16_is_noop
func.func @clamp_f16_is_noop(%arg0: tensor<4xf16>) -> tensor<4xf16> {
  // CHECK: return %arg0
  // CHECK-NOT: "tosa.clamp"
  // 0xFF800000 and 0x7F800000 are respectively negative and positive F32 infinity.
  %0 = tosa.clamp %arg0 {min_int = -128 : i64, max_int = 127 : i64, min_fp = 0xFF800000 : f32, max_fp = 0x7F800000 : f32} : (tensor<4xf16>) -> tensor<4xf16>
  return %0 : tensor<4xf16>
}

// -----

// CHECK-LABEL: @clamp_f32_is_noop
func.func @clamp_f32_is_noop(%arg0: tensor<4xf32>) -> tensor<4xf32> {
  // CHECK: return %arg0
  // CHECK-NOT: "tosa.clamp"
  // 0xFF800000 and 0x7F800000 are respectively negative and positive F32 infinity.
  %0 = tosa.clamp %arg0 {min_int = -128 : i64, max_int = 127 : i64, min_fp = 0xFF800000 : f32, max_fp = 0x7F800000 : f32} : (tensor<4xf32>) -> tensor<4xf32>
  return %0 : tensor<4xf32>
}

// -----

// CHECK-LABEL: @clamp_int8_is_noop
func.func @clamp_int8_is_noop(%arg0: tensor<4xi8>) -> tensor<4xi8> {
  // CHECK: return %arg0
  // CHECK-NOT: tosa.clamp
  %0 = tosa.clamp %arg0 {min_int = -128 : i64, max_int = 127 : i64, min_fp = -3.40282347E+38 : f32, max_fp = 3.40282347E+38 : f32} :  (tensor<4xi8>) -> tensor<4xi8>
  return %0 : tensor<4xi8>
}

// -----

// CHECK-LABEL: @clamp_int16_is_noop
func.func @clamp_int16_is_noop(%arg0: tensor<4xi16>) -> tensor<4xi16> {
  // CHECK: return %arg0
  // CHECK-NOT: tosa.clamp
  %0 = tosa.clamp %arg0 {min_int = -32768 : i64, max_int = 32767 : i64, min_fp = -3.40282347E+38 : f32, max_fp = 3.40282347E+38 : f32} :  (tensor<4xi16>) -> tensor<4xi16>
  return %0 : tensor<4xi16>
}

// -----

// CHECK-LABEL: @clamp_uint8_is_noop
func.func @clamp_uint8_is_noop(%arg0: tensor<4xui8>) -> tensor<4xui8> {
  // CHECK: return %arg0
  // CHECK-NOT: tosa.clamp
  %0 = tosa.clamp %arg0 {min_int = 0 : i64, max_int = 255 : i64, min_fp = -3.40282347E+38 : f32, max_fp = 3.40282347E+38 : f32} :  (tensor<4xui8>) -> tensor<4xui8>
  return %0 : tensor<4xui8>
}

// -----

// CHECK-LABEL: @clamp_twice_is_single_clamp
func.func @clamp_twice_is_single_clamp(%arg0: tensor<4xi8>) -> tensor<4xi8> {
  // CHECK: tosa.clamp %arg0 {max_fp = 3.000000e+00 : f32, max_int = 2 : i64, min_fp = -3.000000e+00 : f32, min_int = -2 : i64}
  %0 = tosa.clamp %arg0 {max_fp = 3.0 : f32, max_int = 4 : i64, min_fp = -5.0 : f32, min_int = -2 : i64} :  (tensor<4xi8>) -> tensor<4xi8>
  %1 = tosa.clamp %0 {max_fp = 5.0 : f32, max_int = 2 : i64, min_fp = -3.0 : f32, min_int = -4 : i64} :  (tensor<4xi8>) -> tensor<4xi8>
  return %1 : tensor<4xi8>
}

func.func @clamp_minimum_i32(%arg0: tensor<4xi32>) -> tensor<4xi32> {
  // CHECK: tosa.clamp %arg0 {max_fp = 6.000000e+00 : f32, max_int = 6 : i64, min_fp = -3.40282347E+38 : f32, min_int = -2147483648 : i64}
  %0 = "tosa.const"() <{value = dense<6> : tensor<1xi32>}> : () -> tensor<1xi32>
  %1 = tosa.minimum %arg0, %0 : (tensor<4xi32>, tensor<1xi32>) -> tensor<4xi32>
  return %1 : tensor<4xi32>
}

func.func @clamp_minimum_f32(%arg0: tensor<4xf32>) -> tensor<4xf32> {
    // CHECK: tosa.clamp %arg0 {max_fp = 6.000000e+00 : f32, max_int = 6 : i64, min_fp = -3.40282347E+38 : f32, min_int = -2147483648 : i64}
  %0 = "tosa.const"() <{value = dense<6.0> : tensor<1xf32>}> : () -> tensor<1xf32>
  %1 = tosa.minimum %arg0, %0 : (tensor<4xf32>, tensor<1xf32>) -> tensor<4xf32>
  return %1 : tensor<4xf32>
}

func.func @clamp_maximum_i32(%arg0: tensor<4xi32>) -> tensor<4xi32> {
  // CHECK: tosa.clamp %arg0 {max_fp = 3.40282347E+38 : f32, max_int = 9223372036854775807 : i64, min_fp = -6.000000e+00 : f32, min_int = -6 : i64}
  %0 = "tosa.const"() <{value = dense<-6> : tensor<1xi32>}> : () -> tensor<1xi32>
  %1 = tosa.maximum %arg0, %0 : (tensor<4xi32>, tensor<1xi32>) -> tensor<4xi32>
  return %1 : tensor<4xi32>
}

func.func @clamp_maximum_f32(%arg0: tensor<4xf32>) -> tensor<4xf32> {
    // CHECK: tosa.clamp %arg0 {max_fp = 3.40282347E+38 : f32, max_int = 9223372036854775807 : i64, min_fp = -6.000000e+00 : f32, min_int = -6 : i64}
  %0 = "tosa.const"() <{value = dense<-6.0> : tensor<1xf32>}> : () -> tensor<1xf32>
  %1 = tosa.maximum %arg0, %0 : (tensor<4xf32>, tensor<1xf32>) -> tensor<4xf32>
  return %1 : tensor<4xf32>
}

// CHECK-LABEL: @concat_fold_zero
func.func @concat_fold_zero(%arg0: tensor<?x0xf32>, %arg1: tensor<?x1xf32>, %arg2: tensor<?x2xf32>) -> tensor<?x3xf32> {
  // CHECK: tosa.concat %arg1, %arg2 {axis = 1 : i32}
  %0 = tosa.concat %arg0, %arg1, %arg2 {axis = 1 : i32}: (tensor<?x0xf32>, tensor<?x1xf32>, tensor<?x2xf32>) -> tensor<?x3xf32>
  return %0 : tensor<?x3xf32>
}
// -----

// CHECK-LABEL: @concat_fold_zero
func.func @concat_fold_zero_all(%arg0: tensor<?x0xf32>, %arg1: tensor<?x0xf32>) -> tensor<?x0xf32> {
  // CHECK: return %arg1
  %0 = tosa.concat %arg0, %arg1 {axis = 1 : i32}: (tensor<?x0xf32>, tensor<?x0xf32>) -> tensor<?x0xf32>
  return %0 : tensor<?x0xf32>
}

// -----

// CHECK-LABEL: @concat_fold_zero
func.func @concat_fold_zero_different_axis(%arg0: tensor<0x2xf32>, %arg1: tensor<0x4xf32>) -> tensor<0x6xf32> {
  // CHECK: tosa.concat %arg0, %arg1
  %0 = tosa.concat %arg0, %arg1 {axis = 1 : i32}: (tensor<0x2xf32>, tensor<0x4xf32>) -> tensor<0x6xf32>
  return %0 : tensor<0x6xf32>
}

// -----

// CHECK-LABEL: @concat_fold_zero_size
func.func @concat_fold_zero_size(%arg0: tensor<?x0xf32>, %arg1: tensor<?x1xf32>, %arg2: tensor<?x2xf32>) -> tensor<?x3xf32> {
  // CHECK: tosa.concat %arg1, %arg2 {axis = 1 : i32}
  %0 = tosa.concat %arg0, %arg1, %arg2 {axis = 1 : i32}: (tensor<?x0xf32>, tensor<?x1xf32>, tensor<?x2xf32>) -> tensor<?x3xf32>
  return %0 : tensor<?x3xf32>
}

// -----

// CHECK: @disjoint_clamp_twice_is_not_single_clamp(%[[INPUT:.*]]: tensor<4xi8>)
func.func @disjoint_clamp_twice_is_not_single_clamp(%arg0: tensor<4xi8>) -> tensor<4xi8> {
  // CHECK: %[[CLAMP_1:.*]] = tosa.clamp %[[INPUT]] {max_fp = -5.000000e+00 : f32, max_int = -5 : i64, min_fp = -1.000000e+00 : f32, min_int = -10 : i64} :  (tensor<4xi8>) -> tensor<4xi8>
  // CHECK-NEXT: tosa.clamp %[[CLAMP_1]] {max_fp = 5.000000e+00 : f32, max_int = 5 : i64, min_fp = 1.000000e+00 : f32, min_int = 1 : i64} :  (tensor<4xi8>) -> tensor<4xi8>
  %0 = tosa.clamp %arg0 {max_fp = -5.0 : f32, max_int = -5 : i64, min_fp = -1.0 : f32, min_int = -10 : i64} :  (tensor<4xi8>) -> tensor<4xi8>
  %1 = tosa.clamp %0 {max_fp = 5.0 : f32, max_int = 5 : i64, min_fp = 1.0 : f32, min_int = 1 : i64} :  (tensor<4xi8>) -> tensor<4xi8>
  return %1 : tensor<4xi8>
}

// -----

// CHECK-LABEL: @clamp_twice_with_nan_propagate_is_single_clamp
func.func @clamp_twice_with_nan_propagate_is_single_clamp(%arg0: tensor<4xi8>) -> tensor<4xi8> {
  // CHECK: tosa.clamp %arg0 {max_fp = 3.000000e+00 : f32, max_int = 2 : i64, min_fp = -3.000000e+00 : f32, min_int = -2 : i64}
  %0 = tosa.clamp %arg0 {max_fp = 3.0 : f32, max_int = 4 : i64, min_fp = -5.0 : f32, min_int = -2 : i64, nan_mode = "PROPAGATE"} :  (tensor<4xi8>) -> tensor<4xi8>
  %1 = tosa.clamp %0 {max_fp = 5.0 : f32, max_int = 2 : i64, min_fp = -3.0 : f32, min_int = -4 : i64, nan_mode = "PROPAGATE"} :  (tensor<4xi8>) -> tensor<4xi8>
  return %1 : tensor<4xi8>
}

// -----

// CHECK-LABEL: @clamp_twice_with_nan_ignore_is_single_clamp
func.func @clamp_twice_with_nan_ignore_is_single_clamp(%arg0: tensor<4xi8>) -> tensor<4xi8> {
  // CHECK: tosa.clamp %arg0 {max_fp = 3.000000e+00 : f32, max_int = 2 : i64, min_fp = -3.000000e+00 : f32, min_int = -2 : i64, nan_mode = "IGNORE"}
  %0 = tosa.clamp %arg0 {max_fp = 3.0 : f32, max_int = 4 : i64, min_fp = -5.0 : f32, min_int = -2 : i64, nan_mode = "IGNORE"} :  (tensor<4xi8>) -> tensor<4xi8>
  %1 = tosa.clamp %0 {max_fp = 5.0 : f32, max_int = 2 : i64, min_fp = -3.0 : f32, min_int = -4 : i64, nan_mode = "IGNORE"} :  (tensor<4xi8>) -> tensor<4xi8>
  return %1 : tensor<4xi8>
}

// -----

// CHECK-LABEL: @clamp_twice_with_nan_ignore_propagate_is_single_clamp
func.func @clamp_twice_with_nan_ignore_propagate_is_single_clamp(%arg0: tensor<4xi8>) -> tensor<4xi8> {
  // CHECK: tosa.clamp %arg0 {max_fp = 3.000000e+00 : f32, max_int = 2 : i64, min_fp = -3.000000e+00 : f32, min_int = -2 : i64, nan_mode = "IGNORE"}
  %0 = tosa.clamp %arg0 {max_fp = 3.0 : f32, max_int = 4 : i64, min_fp = -5.0 : f32, min_int = -2 : i64, nan_mode = "IGNORE"} :  (tensor<4xi8>) -> tensor<4xi8>
  %1 = tosa.clamp %0 {max_fp = 5.0 : f32, max_int = 2 : i64, min_fp = -3.0 : f32, min_int = -4 : i64, nan_mode = "PROPAGATE"} :  (tensor<4xi8>) -> tensor<4xi8>
  return %1 : tensor<4xi8>
}

// -----

// CHECK: @clamp_twice_with_nan_propagate_ignore_is_not_single_clamp(%[[INPUT:.*]]: tensor<4xi8>)
func.func @clamp_twice_with_nan_propagate_ignore_is_not_single_clamp(%arg0: tensor<4xi8>) -> tensor<4xi8> {
  // CHECK: %[[CLAMP_1:.*]] = tosa.clamp %[[INPUT]] {max_fp = 3.000000e+00 : f32, max_int = 4 : i64, min_fp = -5.000000e+00 : f32, min_int = -2 : i64} :  (tensor<4xi8>) -> tensor<4xi8>
  // CHECK-NEXT: tosa.clamp %[[CLAMP_1]] {max_fp = 5.000000e+00 : f32, max_int = 2 : i64, min_fp = -3.000000e+00 : f32, min_int = -4 : i64, nan_mode = "IGNORE"} :  (tensor<4xi8>) -> tensor<4xi8>
  %0 = tosa.clamp %arg0 {max_fp = 3.0 : f32, max_int = 4 : i64, min_fp = -5.0 : f32, min_int = -2 : i64, nan_mode = "PROPAGATE"} :  (tensor<4xi8>) -> tensor<4xi8>
  %1 = tosa.clamp %0 {max_fp = 5.0 : f32, max_int = 2 : i64, min_fp = -3.0 : f32, min_int = -4 : i64, nan_mode = "IGNORE"} :  (tensor<4xi8>) -> tensor<4xi8>
  return %1 : tensor<4xi8>
}

// -----

// CHECK-LABEL: @concat_fold
func.func @concat_fold(%arg0: tensor<?x1xf32>) -> tensor<?x1xf32> {
  // CHECK: return %arg0
  %0 = tosa.concat %arg0 {axis = 0 : i32}: (tensor<?x1xf32>) -> tensor<?x1xf32>
  return %0 : tensor<?x1xf32>
}

// -----

// CHECK-LABEL: @concat_fold_cast
func.func @concat_fold_cast(%arg0: tensor<?x1xf32>) -> tensor<?x?xf32> {
  // CHECK: %[[VAR0:.*]] = tensor.cast %arg0
  // CHECK: return %[[VAR0]]
  %0 = tosa.concat %arg0 {axis = 0 : i32}: (tensor<?x1xf32>) -> tensor<?x?xf32>
  return %0 : tensor<?x?xf32>
}

// -----

// CHECK-LABEL: @conv2d_stride_2
func.func @conv2d_stride_2(%arg0: tensor<4x10x10x2xf32>) -> tensor<4x10x10x3xf32> {
  // CHECK: tosa.conv2d
  %weight = "tosa.const"() {value = dense<[[[[1.0, 1.0]]], [[[1.0, 1.0]]], [[[1.0, 1.0]]]]> : tensor<3x1x1x2xf32>} : ()-> tensor<3x1x1x2xf32>
  %bias = "tosa.const"() {value = dense<0.0> : tensor<3xf32>} : ()-> tensor<3xf32>
  %0 = tosa.conv2d %arg0, %weight, %bias {acc_type = f32, pad = array<i64: 0, 0, 0, 0>, stride = array<i64: 2, 2>, dilation = array<i64: 1, 1>} : (tensor<4x10x10x2xf32>, tensor<3x1x1x2xf32>, tensor<3xf32>) -> tensor<4x10x10x3xf32>
  return %0 : tensor<4x10x10x3xf32>
}

// -----

// CHECK-LABEL: @conv2d_weight_2x2
func.func @conv2d_weight_2x2(%arg0: tensor<4x10x10x1xf32>) -> tensor<4x10x10x1xf32> {
  // CHECK: tosa.conv2d
  %weight = "tosa.const"() {value = dense<[[[[1.0], [1.0]], [[1.0], [1.0]]]]> : tensor<1x2x2x1xf32>} : ()-> tensor<1x2x2x1xf32>
  %bias = "tosa.const"() {value = dense<0.0> : tensor<1xf32>} : ()-> tensor<1xf32>
  %0 = tosa.conv2d %arg0, %weight, %bias {acc_type = f32, pad = array<i64: 0, 0, 0, 0>, stride = array<i64: 1, 1>, dilation = array<i64: 1, 1>} : (tensor<4x10x10x1xf32>, tensor<1x2x2x1xf32>, tensor<1xf32>) -> tensor<4x10x10x1xf32>
  return %0 : tensor<4x10x10x1xf32>
}

// -----

// CHECK-LABEL: @depthwise_conv2d_stride_2
func.func @depthwise_conv2d_stride_2(%arg0: tensor<4x10x10x2xf32>, %arg1: tensor<1x1x2x3xf32>, %arg2: tensor<6xf32>) -> tensor<4x10x10x6xf32> {
  // CHECK: tosa.depthwise_conv2d
  %0 = tosa.depthwise_conv2d %arg0, %arg1, %arg2 {acc_type = f32, pad = array<i64: 0, 0, 0, 0>, stride = array<i64: 2, 2>, dilation = array<i64: 1, 1>} : (tensor<4x10x10x2xf32>, tensor<1x1x2x3xf32>, tensor<6xf32>) -> tensor<4x10x10x6xf32>
  return %0 : tensor<4x10x10x6xf32>
}

// -----

// CHECK-LABEL: @depthwise_conv2d_weight_2x2
func.func @depthwise_conv2d_weight_2x2(%arg0: tensor<4x10x10x2xf32>, %arg1: tensor<2x2x2x3xf32>, %arg2: tensor<6xf32>) -> tensor<4x10x10x6xf32> {
  // CHECK: tosa.depthwise_conv2d
  %0 = tosa.depthwise_conv2d %arg0, %arg1, %arg2 {acc_type = f32, pad = array<i64: 0, 0, 0, 0>, stride = array<i64: 1, 1>, dilation = array<i64: 1, 1>} : (tensor<4x10x10x2xf32>, tensor<2x2x2x3xf32>, tensor<6xf32>) -> tensor<4x10x10x6xf32>
  return %0 : tensor<4x10x10x6xf32>
}

// -----

// CHECK-LABEL: @max_pool2d_is_noop
func.func @max_pool2d_is_noop(%arg0: tensor<10x1x1x3xf32>) -> tensor<10x1x1x3xf32> {
  // CHECK-NOT: tosa.max_pool2d
  // CHECK: return %arg0
  %0 = tosa.max_pool2d %arg0 {kernel = array<i64: 1, 1>, pad = array<i64: 0, 0, 0, 0>, stride = array<i64: 1, 1>, dilation = array<i64: 1, 1>} : (tensor<10x1x1x3xf32>) -> tensor<10x1x1x3xf32>
  return %0 : tensor<10x1x1x3xf32>
}

// -----

// CHECK-LABEL: @pad_noop
func.func @pad_noop(%arg0: tensor<?x?xf32>) -> tensor<?x?xf32> {
  // CHECK: return %arg0
  %0 = tosa.const_shape { value = dense<0> : tensor<4xindex>} : () -> !tosa.shape<4>
  %1 = tosa.pad %arg0, %0 : (tensor<?x?xf32>, !tosa.shape<4>) -> tensor<?x?xf32>
  return %1 : tensor<?x?xf32>
}

// -----

// CHECK-LABEL: @pad_noop_padding_mismatch_nofold
func.func @pad_noop_padding_mismatch_nofold(%arg0: tensor<?x?xf32>) -> tensor<?x?xf32> {
  // CHECK: %[[PAD:.+]] = tosa.pad
  // CHECK: return %[[PAD]]
  %shape = tosa.const_shape { value = dense<[1, 0, 0, 1]> : tensor<4xindex>} : () -> !tosa.shape<4>
  %1 = tosa.pad %arg0, %shape : (tensor<?x?xf32>, !tosa.shape<4>) -> tensor<?x?xf32>
  return %1 : tensor<?x?xf32>
}

// -----

// CHECK-LABEL: @pad_noop_type_mismatch_nofold
func.func @pad_noop_type_mismatch_nofold(%arg0: tensor<10xf32>) -> tensor<?xf32> {
  // CHECK: %[[PAD:.+]] = tosa.pad
  // CHECK: return %[[PAD]]
  %shape = tosa.const_shape { value = dense<[1, 2]> : tensor<2xindex>} : () -> !tosa.shape<2>
  %0 = tosa.pad %arg0, %shape : (tensor<10xf32>, !tosa.shape<2>) -> tensor<?xf32>
  return %0 : tensor<?xf32>
}

// -----

// CHECK-LABEL: @pad_determine_val_i32
func.func @pad_determine_val_i32(%arg0: tensor<?x?xi32>, %arg1 : tensor<2x2xi32>) -> tensor<?x?xi32> {
  // CHECK-DAG: %[[ZERO:.+]] = "tosa.const"() <{value = dense<0> : tensor<i32>}
  // CHECK-DAG: %[[PADDING:.+]] = tosa.const_shape {value = dense<[1, 0, 0, 1]> : tensor<4xindex>} : () -> !tosa.shape<4>
  // CHECK: tosa.pad %arg0, %[[PADDING]], %[[ZERO]]
  %0 = tosa.const_shape { value = dense<[1, 0, 0, 1]> : tensor<4xindex>} : () -> !tosa.shape<4>
  %1 = tosa.pad %arg0, %0 : (tensor<?x?xi32>, !tosa.shape<4>) -> tensor<?x?xi32>
  return %1 : tensor<?x?xi32>
}

// -----

// CHECK-LABEL: @pad_determine_val_f32
func.func @pad_determine_val_f32(%arg0: tensor<?x?xf32>, %arg1 : tensor<2x2xi32>) -> tensor<?x?xf32> {
  // CHECK-DAG: %[[ZERO:.+]] = "tosa.const"() <{value = dense<0.000000e+00> : tensor<f32>}
  // CHECK-DAG: %[[PADDING:.+]] = tosa.const_shape {value = dense<[1, 0, 0, 1]> : tensor<4xindex>} : () -> !tosa.shape<4>
  // CHECK: tosa.pad %arg0, %[[PADDING]], %[[ZERO]]
  %0 = tosa.const_shape { value = dense<[1, 0, 0, 1]> : tensor<4xindex>} : () -> !tosa.shape<4>
  %1 = tosa.pad %arg0, %0 : (tensor<?x?xf32>, !tosa.shape<4>) -> tensor<?x?xf32>
  return %1 : tensor<?x?xf32>
}

// -----

// CHECK-LABEL: @pad_determine_val_quant
func.func @pad_determine_val_quant(%arg0: tensor<?x?xi32>, %arg1 : tensor<2x2xi32>) -> tensor<?x?xi32> {
  // CHECK-DAG: %[[ZERO:.+]] = "tosa.const"() <{value = dense<0> : tensor<i32>}
  // CHECK-DAG: %[[PADDING:.+]] = tosa.const_shape {value = dense<[1, 0, 0, 1]> : tensor<4xindex>} : () -> !tosa.shape<4>
  // CHECK: tosa.pad %arg0, %[[PADDING]], %[[ZERO]]
  %0 = tosa.const_shape { value = dense<[1, 0, 0, 1]> : tensor<4xindex>} : () -> !tosa.shape<4>
  %1 = tosa.pad %arg0, %0 {input_zp = 42 : i32} : (tensor<?x?xi32>, !tosa.shape<4>) -> tensor<?x?xi32>
  return %1 : tensor<?x?xi32>
}

// -----

// CHECK-LABEL: @pad_pad_optimization_dense_values_bf16
func.func @pad_pad_optimization_dense_values_bf16(%arg0: tensor<1x478x640x32xbf16>) -> tensor<1x483x644x32xbf16> {
  // CHECK-DAG: %[[CONST:.+]] = "tosa.const"() <{value = dense<2.000000e+00> : tensor<bf16>}> : () -> tensor<bf16>
  // CHECK-DAG: %[[PADDING:.+]] = tosa.const_shape  {value = dense<[0, 0, 2, 3, 2, 2, 0, 0]> : tensor<8xindex>} : () -> !tosa.shape<8>
  // CHECK:tosa.pad %arg0, %[[PADDING]], %[[CONST]]
  %0 = tosa.const_shape  {value = dense<[0, 0, 0, 1, 0, 0, 0, 0]> : tensor<8xindex>} : () -> !tosa.shape<8>
  %1 = "tosa.const"() <{value = dense<2.000000e+00> : tensor<bf16>}> : () -> tensor<bf16>
  %2 = tosa.const_shape  {value = dense<[0, 0, 2, 2, 2, 2, 0, 0]> : tensor<8xindex>} : () -> !tosa.shape<8>
  %3 = tosa.pad %arg0, %2, %1 : (tensor<1x478x640x32xbf16>, !tosa.shape<8>, tensor<bf16>) -> tensor<1x482x644x32xbf16>
  %4 = tosa.pad %3, %0, %1 : (tensor<1x482x644x32xbf16>, !tosa.shape<8>, tensor<bf16>) -> tensor<1x483x644x32xbf16>
  return %4 : tensor<1x483x644x32xbf16>
}

// -----

// CHECK-LABEL: @pad_pad_optimization_dense_values_f16
func.func @pad_pad_optimization_dense_values_f16(%arg0: tensor<1x478x640x32xf16>) -> tensor<1x483x644x32xf16> {
  // CHECK-DAG: %[[CONST:.+]] = "tosa.const"() <{value = dense<2.000000e+00> : tensor<f16>}> : () -> tensor<f16>
  // CHECK-DAG: %[[PADDING:.+]] = tosa.const_shape  {value = dense<[0, 0, 2, 3, 2, 2, 0, 0]> : tensor<8xindex>} : () -> !tosa.shape<8>
  // CHECK:tosa.pad %arg0, %[[PADDING]], %[[CONST]]
  %0 = tosa.const_shape  {value = dense<[0, 0, 0, 1, 0, 0, 0, 0]> : tensor<8xindex>} : () -> !tosa.shape<8>
  %1 = "tosa.const"() <{value = dense<2.000000e+00> : tensor<f16>}> : () -> tensor<f16>
  %2 = tosa.const_shape  {value = dense<[0, 0, 2, 2, 2, 2, 0, 0]> : tensor<8xindex>} : () -> !tosa.shape<8>
  %3 = tosa.pad %arg0, %2, %1 : (tensor<1x478x640x32xf16>, !tosa.shape<8>, tensor<f16>) -> tensor<1x482x644x32xf16>
  %4 = tosa.pad %3, %0, %1 : (tensor<1x482x644x32xf16>, !tosa.shape<8>, tensor<f16>) -> tensor<1x483x644x32xf16>
  return %4 : tensor<1x483x644x32xf16>
}

// -----

// CHECK-LABEL: @pad_pad_optimization_dense_values_i32
func.func @pad_pad_optimization_dense_values_i32(%arg0: tensor<478x640xi32>) -> tensor<483x644xi32> {
  // CHECK-DAG: %[[CONST:.+]] = "tosa.const"() <{value = dense<3> : tensor<i32>}> : () -> tensor<i32>
  // CHECK-DAG: %[[PADDING:.+]] = tosa.const_shape  {value = dense<[2, 3, 2, 2]> : tensor<4xindex>} : () -> !tosa.shape<4>
  // CHECK:tosa.pad %arg0, %[[PADDING]], %[[CONST]]
  %0 = tosa.const_shape  {value = dense<[0, 1, 0, 0]> : tensor<4xindex>} : () -> !tosa.shape<4>
  %1 = "tosa.const"() <{value = dense<3> : tensor<i32>}> : () -> tensor<i32>
  %2 = tosa.const_shape  {value = dense<[2, 2, 2, 2]> : tensor<4xindex>} : () -> !tosa.shape<4>
  %3 = tosa.pad %arg0, %2, %1 : (tensor<478x640xi32>, !tosa.shape<4>, tensor<i32>) -> tensor<482x644xi32>
  %4 = tosa.pad %3, %0, %1 : (tensor<482x644xi32>, !tosa.shape<4>, tensor<i32>) -> tensor<483x644xi32>
  return %4 : tensor<483x644xi32>
}

// -----

// CHECK-LABEL:@pad_pad_optimization_splat_values
func.func @pad_pad_optimization_splat_values(%arg0: tensor<1x478x640x32xbf16>) -> tensor<1x483x644x32xbf16> {
  // CHECK-DAG: %[[CONST:.+]] = "tosa.const"() <{value = dense<0.000000e+00> : tensor<bf16>}> : () -> tensor<bf16>
  // CHECK-DAG: %[[PADDING:.+]] = tosa.const_shape  {value = dense<4> : tensor<8xindex>} : () -> !tosa.shape<8>
  // CHECK: tosa.pad %arg0, %[[PADDING]], %[[CONST]]
  %0 = tosa.const_shape  {value = dense<1> : tensor<8xindex>} : () -> !tosa.shape<8>
  %1 = "tosa.const"() <{value = dense<0.000000e+00> : tensor<bf16>}> : () -> tensor<bf16>
  %2 = tosa.const_shape  {value = dense<3> : tensor<8xindex>} : () -> !tosa.shape<8>
  %3 = tosa.pad %arg0, %2, %1 : (tensor<1x478x640x32xbf16>, !tosa.shape<8>, tensor<bf16>) -> tensor<1x482x644x32xbf16>
  %4 = tosa.pad %3, %0, %1 : (tensor<1x482x644x32xbf16>, !tosa.shape<8>, tensor<bf16>) -> tensor<1x483x644x32xbf16>
  return %4 : tensor<1x483x644x32xbf16>
}

// -----

// CHECK-LABEL: @pad_pad_optimization_dense_and_splat_values
func.func @pad_pad_optimization_dense_and_splat_values(%arg0: tensor<1x478x640x32xbf16>) -> tensor<1x483x644x32xbf16> {
  // CHECK-DAG: %[[CONST:.+]] = "tosa.const"() <{value = dense<0.000000e+00> : tensor<bf16>}> : () -> tensor<bf16>
  // CHECK-DAG: %[[PADDING:.+]] = tosa.const_shape  {value = dense<[1, 1, 3, 3, 3, 3, 1, 1]> : tensor<8xindex>} : () -> !tosa.shape<8>
  // CHECK:tosa.pad %arg0, %[[PADDING]], %[[CONST]] : (tensor<1x478x640x32xbf16>, !tosa.shape<8>, tensor<bf16>) -> tensor<1x483x644x32xbf16>
  %0 = tosa.const_shape  {value = dense<1> : tensor<8xindex>} : () -> !tosa.shape<8>
  %1 = "tosa.const"() <{value = dense<0.000000e+00> : tensor<bf16>}> : () -> tensor<bf16>
  %2 = tosa.const_shape  {value = dense<[0, 0, 2, 2, 2, 2, 0, 0]> : tensor<8xindex>} : () -> !tosa.shape<8>
  %3 = tosa.pad %arg0, %2, %1 : (tensor<1x478x640x32xbf16>, !tosa.shape<8>, tensor<bf16>) -> tensor<1x482x644x32xbf16>
  %4 = tosa.pad %3, %0, %1 : (tensor<1x482x644x32xbf16>, !tosa.shape<8>, tensor<bf16>) -> tensor<1x483x644x32xbf16>
  return %4 : tensor<1x483x644x32xbf16>
}

// -----

// CHECK-LABEL: @pad_pad_optimization_default_const_value
func.func @pad_pad_optimization_default_const_value(%arg0: tensor<1x478x640x32xbf16>) -> tensor<1x483x644x32xbf16> {
  // CHECK-DAG: %[[CONST:.+]] = "tosa.const"() <{value = dense<0.000000e+00> : tensor<bf16>}> : () -> tensor<bf16>
  // CHECK-DAG: %[[PADDING:.+]] = tosa.const_shape  {value = dense<[1, 1, 3, 3, 3, 3, 1, 1]> : tensor<8xindex>} : () -> !tosa.shape<8>
  // CHECK: tosa.pad %arg0, %[[PADDING]], %[[CONST]]
  %0 = tosa.const_shape  {value = dense<1> : tensor<8xindex>} : () -> !tosa.shape<8>
  %2 = tosa.const_shape  {value = dense<[0, 0, 2, 2, 2, 2, 0, 0]> : tensor<8xindex>} : () -> !tosa.shape<8>
  %3 = tosa.pad %arg0, %2 : (tensor<1x478x640x32xbf16>, !tosa.shape<8>) -> tensor<1x482x644x32xbf16>
  %5 = tosa.pad %3, %0 : (tensor<1x482x644x32xbf16>, !tosa.shape<8>) -> tensor<1x483x644x32xbf16>
  return %5 : tensor<1x483x644x32xbf16>
}

// -----

// CHECK-LABEL: @pad_pad_optimization_nonconst
func.func @pad_pad_optimization_nonconst(%arg0: tensor<1x478x640x32xbf16>, %arg1: tensor<bf16>) -> tensor<1x483x644x32xbf16> {
  // CHECK-DAG: %[[PADDING:.+]] = tosa.const_shape  {value = dense<[1, 1, 3, 3, 3, 3, 1, 1]> : tensor<8xindex>} : () -> !tosa.shape<8>
  // CHECK:tosa.pad %arg0, %[[PADDING]], %arg1 : (tensor<1x478x640x32xbf16>, !tosa.shape<8>, tensor<bf16>) -> tensor<1x483x644x32xbf16>
  %0 = tosa.const_shape  {value = dense<1> : tensor<8xindex>} : () -> !tosa.shape<8>
  %1 = tosa.const_shape  {value = dense<[0, 0, 2, 2, 2, 2, 0, 0]> : tensor<8xindex>} : () -> !tosa.shape<8>
  %2 = tosa.pad %arg0, %1, %arg1 : (tensor<1x478x640x32xbf16>, !tosa.shape<8>, tensor<bf16>) -> tensor<1x482x644x32xbf16>
  %3 = tosa.pad %2, %0, %arg1 : (tensor<1x482x644x32xbf16>, !tosa.shape<8>, tensor<bf16>) -> tensor<1x483x644x32xbf16>
  return %3 : tensor<1x483x644x32xbf16>
}

// -----

// CHECK-LABEL: @no_pad_pad_optimization_different_value
func.func @no_pad_pad_optimization_different_value(%arg0: tensor<1x478x640x32xbf16>) -> tensor<1x483x644x32xbf16> {
  // CHECK: tosa.pad
  // CHECK: tosa.pad
  %0 = tosa.const_shape  {value = dense<1> : tensor<8xindex>} : () -> !tosa.shape<8>
  %1 = "tosa.const"() <{value = dense<0.000000e+00> : tensor<bf16>}> : () -> tensor<bf16>
  %2 = tosa.const_shape  {value = dense<[0, 0, 2, 2, 2, 2, 0, 0]> : tensor<8xindex>} : () -> !tosa.shape<8>
  %3 = tosa.pad %arg0, %2, %1 : (tensor<1x478x640x32xbf16>, !tosa.shape<8>, tensor<bf16>) -> tensor<1x482x644x32xbf16>
  %4 = "tosa.const"() <{value = dense<1.000000e+00> : tensor<bf16>}> : () -> tensor<bf16>
  %5 = tosa.pad %3, %0, %4 : (tensor<1x482x644x32xbf16>, !tosa.shape<8>, tensor<bf16>) -> tensor<1x483x644x32xbf16>
  return %5 : tensor<1x483x644x32xbf16>
}

// -----

// CHECK-LABEL: @slice_pad_fold_poison
// CHECK-SAME:   %[[ARG:.*]]: tensor<1x10x134x4xf32>
func.func @slice_pad_fold_poison(%arg0: tensor<1x10x134x4xf32>) -> tensor<1x10x134x4xf32> {
  // CHECK-NOT: tosa.slice
  // CHECK-NOT: tosa.pad
  // CHECK: return %[[ARG]]
  %sliced = tosa.slice %arg0 {size = array<i64: 1, 10, 133, 4>, start = array<i64: 0, 0, 0, 0>} : (tensor<1x10x134x4xf32>) -> tensor<1x10x133x4xf32>
  %poison = ub.poison : tensor<f32>
  %shape = tosa.const_shape {value = dense<[0, 0, 0, 0, 0, 1, 0, 0]> : tensor<8xindex>} : () -> !tosa.shape<8>
  %padded = tosa.pad %sliced, %shape, %poison : (tensor<1x10x133x4xf32>, !tosa.shape<8>, tensor<f32>) -> tensor<1x10x134x4xf32>
  return %padded : tensor<1x10x134x4xf32>
}

// -----

// The pad re-adds the sliced region but does not fill poison, so it must not fold.
// CHECK-LABEL: @slice_pad_no_fold_nonpoison
func.func @slice_pad_no_fold_nonpoison(%arg0: tensor<1x10x134x4xf32>) -> tensor<1x10x134x4xf32> {
  // CHECK: tosa.slice
  // CHECK: tosa.pad
  %sliced = tosa.slice %arg0 {size = array<i64: 1, 10, 133, 4>, start = array<i64: 0, 0, 0, 0>} : (tensor<1x10x134x4xf32>) -> tensor<1x10x133x4xf32>
  %zero = "tosa.const"() <{value = dense<0.0> : tensor<f32>}> : () -> tensor<f32>
  %shape = tosa.const_shape {value = dense<[0, 0, 0, 0, 0, 1, 0, 0]> : tensor<8xindex>} : () -> !tosa.shape<8>
  %padded = tosa.pad %sliced, %shape, %zero : (tensor<1x10x133x4xf32>, !tosa.shape<8>, tensor<f32>) -> tensor<1x10x134x4xf32>
  return %padded : tensor<1x10x134x4xf32>
}

// -----

// The pad does not re-add exactly what the slice removed (low pad 2 != slice
// start 0 on the padded axis), so it must not fold even with poison.
// CHECK-LABEL: @slice_pad_no_fold_mismatched_region
func.func @slice_pad_no_fold_mismatched_region(%arg0: tensor<1x10x134x4xf32>) -> tensor<1x10x134x4xf32> {
  // CHECK: tosa.slice
  // CHECK: tosa.pad
  %sliced = tosa.slice %arg0 {size = array<i64: 1, 10, 133, 4>, start = array<i64: 0, 0, 0, 0>} : (tensor<1x10x134x4xf32>) -> tensor<1x10x133x4xf32>
  %poison = ub.poison : tensor<f32>
  %shape = tosa.const_shape {value = dense<[0, 0, 0, 0, 1, 0, 0, 0]> : tensor<8xindex>} : () -> !tosa.shape<8>
  %padded = tosa.pad %sliced, %shape, %poison : (tensor<1x10x133x4xf32>, !tosa.shape<8>, tensor<f32>) -> tensor<1x10x134x4xf32>
  return %padded : tensor<1x10x134x4xf32>
}

// -----

// A slice that keeps a contiguous linear prefix followed by a poison pad on a
// different axis is rewritten into a flattened poison pad plus a reshape. The
// slice (dropping trailing poison) disappears.
// CHECK-LABEL: @slice_pad_to_flat_pad_reshape_poison
// CHECK-SAME:   %[[ARG:.*]]: tensor<1x1008x1x1xbf16>
func.func @slice_pad_to_flat_pad_reshape_poison(%arg0: tensor<1x1008x1x1xbf16>) -> tensor<16x1000x1x1xbf16> {
  // CHECK-NOT: tosa.slice
  // CHECK: %[[FLAT:.*]] = tosa.reshape %[[ARG]] {new_shape = array<i64: 1008>} : (tensor<1x1008x1x1xbf16>) -> tensor<1008xbf16>
  // CHECK: %[[PAD:.*]] = tosa.pad %[[FLAT]], %{{.*}}, %{{.*}} : (tensor<1008xbf16>, !tosa.shape<2>, tensor<bf16>) -> tensor<16000xbf16>
  // CHECK: %[[RES:.*]] = tosa.reshape %[[PAD]] {new_shape = array<i64: 16, 1000, 1, 1>} : (tensor<16000xbf16>) -> tensor<16x1000x1x1xbf16>
  // CHECK: return %[[RES]]
  %sliced = tosa.slice %arg0 {size = array<i64: 1, 1000, 1, 1>, start = array<i64: 0, 0, 0, 0>} : (tensor<1x1008x1x1xbf16>) -> tensor<1x1000x1x1xbf16>
  %poison = ub.poison : tensor<bf16>
  %shape = tosa.const_shape {value = dense<[0, 15, 0, 0, 0, 0, 0, 0]> : tensor<8xindex>} : () -> !tosa.shape<8>
  %padded = tosa.pad %sliced, %shape, %poison : (tensor<1x1000x1x1xbf16>, !tosa.shape<8>, tensor<bf16>) -> tensor<16x1000x1x1xbf16>
  return %padded : tensor<16x1000x1x1xbf16>
}

// -----

// Negative: pad with a non-poison value must not be rewritten.
// CHECK-LABEL: @slice_pad_to_flat_pad_reshape_no_fold_nonpoison
func.func @slice_pad_to_flat_pad_reshape_no_fold_nonpoison(%arg0: tensor<1x1008x1x1xbf16>) -> tensor<16x1000x1x1xbf16> {
  // CHECK: tosa.slice
  // CHECK: tosa.pad
  %sliced = tosa.slice %arg0 {size = array<i64: 1, 1000, 1, 1>, start = array<i64: 0, 0, 0, 0>} : (tensor<1x1008x1x1xbf16>) -> tensor<1x1000x1x1xbf16>
  %zero = "tosa.const"() <{value = dense<0.0> : tensor<bf16>}> : () -> tensor<bf16>
  %shape = tosa.const_shape {value = dense<[0, 15, 0, 0, 0, 0, 0, 0]> : tensor<8xindex>} : () -> !tosa.shape<8>
  %padded = tosa.pad %sliced, %shape, %zero : (tensor<1x1000x1x1xbf16>, !tosa.shape<8>, tensor<bf16>) -> tensor<16x1000x1x1xbf16>
  return %padded : tensor<16x1000x1x1xbf16>
}

// -----

// Negative: the slice does not start at 0, so it does not keep the linear prefix.
// CHECK-LABEL: @slice_pad_to_flat_pad_reshape_no_fold_offset
func.func @slice_pad_to_flat_pad_reshape_no_fold_offset(%arg0: tensor<1x1008x1x1xbf16>) -> tensor<16x1000x1x1xbf16> {
  // CHECK: tosa.slice
  // CHECK: tosa.pad
  %sliced = tosa.slice %arg0 {size = array<i64: 1, 1000, 1, 1>, start = array<i64: 0, 8, 0, 0>} : (tensor<1x1008x1x1xbf16>) -> tensor<1x1000x1x1xbf16>
  %poison = ub.poison : tensor<bf16>
  %shape = tosa.const_shape {value = dense<[0, 15, 0, 0, 0, 0, 0, 0]> : tensor<8xindex>} : () -> !tosa.shape<8>
  %padded = tosa.pad %sliced, %shape, %poison : (tensor<1x1000x1x1xbf16>, !tosa.shape<8>, tensor<bf16>) -> tensor<16x1000x1x1xbf16>
  return %padded : tensor<16x1000x1x1xbf16>
}

// -----

// Negative: the slice does not keep a contiguous linear prefix because an outer
// dimension (axis 1) has extent > 1 while an inner axis is reduced.
// CHECK-LABEL: @slice_pad_to_flat_pad_reshape_no_fold_noncontiguous
func.func @slice_pad_to_flat_pad_reshape_no_fold_noncontiguous(%arg0: tensor<1x2x1008x1xbf16>) -> tensor<16x2x1000x1xbf16> {
  // CHECK: tosa.slice
  // CHECK: tosa.pad
  %sliced = tosa.slice %arg0 {size = array<i64: 1, 2, 1000, 1>, start = array<i64: 0, 0, 0, 0>} : (tensor<1x2x1008x1xbf16>) -> tensor<1x2x1000x1xbf16>
  %poison = ub.poison : tensor<bf16>
  %shape = tosa.const_shape {value = dense<[0, 15, 0, 0, 0, 0, 0, 0]> : tensor<8xindex>} : () -> !tosa.shape<8>
  %padded = tosa.pad %sliced, %shape, %poison : (tensor<1x2x1000x1xbf16>, !tosa.shape<8>, tensor<bf16>) -> tensor<16x2x1000x1xbf16>
  return %padded : tensor<16x2x1000x1xbf16>
}

// -----

// CHECK-LABEL: @mul_one_float
func.func @mul_one_float(%arg0: tensor<2x3xf32>) -> tensor<2x3xf32> {
  // CHECK: return %arg0
  // CHECK-NOT: tosa.mul
  %shift = "tosa.const"() <{value = dense<0> : tensor<1xi8>}> : () -> tensor<1xi8>
  %ones = "tosa.const"() {value = dense<1.0> : tensor<2x3xf32>} : () -> tensor<2x3xf32>
  %1 = tosa.mul %arg0, %ones, %shift : (tensor<2x3xf32>, tensor<2x3xf32>, tensor<1xi8>) -> tensor<2x3xf32>
  return %1 : tensor<2x3xf32>
}

// -----

// CHECK-LABEL: @mul_bcast_one_float
func.func @mul_bcast_one_float(%arg0: tensor<2x3xf32>) -> tensor<2x3xf32> {
  // CHECK: return %arg0
  // CHECK-NOT: tosa.mul
  %ones = "tosa.const"() {value = dense<1.0> : tensor<1x1xf32>} : () -> tensor<1x1xf32>
  %shift = "tosa.const"() <{value = dense<0> : tensor<1xi8>}> : () -> tensor<1xi8>
  %1 = tosa.mul %ones, %arg0, %shift : (tensor<1x1xf32>, tensor<2x3xf32>, tensor<1xi8>) -> tensor<2x3xf32>
  return %1 : tensor<2x3xf32>
}

// -----

// CHECK-LABEL: @mul_one_int
func.func @mul_one_int(%arg0: tensor<2x3xi32>) -> tensor<2x3xi32> {
  // CHECK: return %arg0
  // CHECK-NOT: tosa.mul
  %shift = "tosa.const"() <{value = dense<0> : tensor<1xi8>}> : () -> tensor<1xi8>
  %ones = "tosa.const"() {value = dense<1> : tensor<2x3xi32>} : () -> tensor<2x3xi32>
  %1 = tosa.mul %arg0, %ones, %shift : (tensor<2x3xi32>, tensor<2x3xi32>, tensor<1xi8>) -> tensor<2x3xi32>
  return %1 : tensor<2x3xi32>
}

// -----

// CHECK-LABEL: @mul_one_int_and_shift
func.func @mul_one_int_and_shift(%arg0: tensor<2x3xi32>) -> tensor<2x3xi32> {
  // CHECK-DAG: %[[VAL_1:.*]] = "tosa.const"() <{value = dense<1> : tensor<2x3xi32>}>
  // CHECK-DAG: %[[VAL_2:.*]] = "tosa.const"() <{value = dense<31> : tensor<1xi8>}>
  // CHECK: %[[VAL_3:.*]] = tosa.mul %arg0, %[[VAL_1]], %[[VAL_2]] : (tensor<2x3xi32>, tensor<2x3xi32>, tensor<1xi8>)
  %ones = "tosa.const"() {value = dense<1> : tensor<2x3xi32>} : () -> tensor<2x3xi32>
  %shift = "tosa.const"() <{value = dense<31> : tensor<1xi8>}> : () -> tensor<1xi8>
  %1 = tosa.mul %arg0, %ones, %shift : (tensor<2x3xi32>, tensor<2x3xi32>, tensor<1xi8>) -> tensor<2x3xi32>
  return %1 : tensor<2x3xi32>
}

// -----

// CHECK-LABEL: @mul_zero_broadcast
func.func @mul_zero_broadcast(%arg0: tensor<2x3xf32>) -> (tensor<2x3xf32>, tensor<2x3xf32>) {
  // CHECK: %[[ZERO:.*]] = "tosa.const"() <{value = dense<0.000000e+00> : tensor<2x3xf32>}
  // CHECK-NOT: tosa.mul
  %zeros = "tosa.const"() {value = dense<0.0> : tensor<1x1xf32>} : () -> tensor<1x1xf32>
  %shift = "tosa.const"() <{value = dense<0> : tensor<1xi8>}> : () -> tensor<1xi8>
  %1 = tosa.mul %arg0, %zeros, %shift : (tensor<2x3xf32>, tensor<1x1xf32>, tensor<1xi8>) -> tensor<2x3xf32>

  // CHECK-NOT: tosa.mul
  // CHECK: return %[[ZERO]], %[[ZERO]]
  %2 = tosa.mul %zeros, %arg0, %shift : (tensor<1x1xf32>, tensor<2x3xf32>, tensor<1xi8>) -> tensor<2x3xf32>
  return %1, %2 : tensor<2x3xf32>, tensor<2x3xf32>
}

// CHECK-LABEL: @mul_zero_broadcast_dynamic_result
func.func @mul_zero_broadcast_dynamic_result(%arg0: tensor<?x3xf32>) -> (tensor<?x3xf32>, tensor<?x3xf32>) {
  // CHECK: tosa.mul
  // CHECK: tosa.mul
  %zeros = "tosa.const"() {value = dense<0.0> : tensor<1x1xf32>} : () -> tensor<1x1xf32>
  %shift = "tosa.const"() <{value = dense<0> : tensor<1xi8>}> : () -> tensor<1xi8>
  %1 = tosa.mul %arg0, %zeros, %shift : (tensor<?x3xf32>, tensor<1x1xf32>, tensor<1xi8>) -> tensor<?x3xf32>
  %2 = tosa.mul %zeros, %arg0, %shift : (tensor<1x1xf32>, tensor<?x3xf32>, tensor<1xi8>) -> tensor<?x3xf32>
  return %1, %2 : tensor<?x3xf32>, tensor<?x3xf32>
}

// -----

// CHECK-LABEL: @select_same_value
func.func @select_same_value(%arg0: tensor<2x3xi1>, %arg1: tensor<2x3xi32>) -> tensor<2x3xi32> {
  %0 = tosa.select %arg0, %arg1, %arg1 : (tensor<2x3xi1>, tensor<2x3xi32>, tensor<2x3xi32>) -> tensor<2x3xi32>
  // CHECK: return %arg1
  // CHECK-NOT: tosa.select
  return %0 : tensor<2x3xi32>
}

// -----

// CHECK-LABEL: @select_true_value
func.func @select_true_value(%arg0: tensor<2x3xi32>, %arg1: tensor<2x3xi32>) -> tensor<2x3xi32> {
  %c1 = "tosa.const"() {value = dense<1> : tensor<2x3xi1>} : () -> tensor<2x3xi1>
  %0 = tosa.select %c1, %arg0, %arg1 : (tensor<2x3xi1>, tensor<2x3xi32>, tensor<2x3xi32>) -> tensor<2x3xi32>
  // CHECK: return %arg0
  // CHECK-NOT: tosa.select
  return %0 : tensor<2x3xi32>
}

// -----

// CHECK-LABEL: @select_false_value
func.func @select_false_value(%arg0: tensor<2x3xi32>, %arg1: tensor<2x3xi32>) -> tensor<2x3xi32> {
  %c0 = "tosa.const"() {value = dense<0> : tensor<2x3xi1>} : () -> tensor<2x3xi1>
  %0 = tosa.select %c0, %arg0, %arg1 : (tensor<2x3xi1>, tensor<2x3xi32>, tensor<2x3xi32>) -> tensor<2x3xi32>
  // CHECK: return %arg1
  // CHECK-NOT: tosa.select
  return %0 : tensor<2x3xi32>
}

// -----

// CHECK-LABEL: @select_not_pred
func.func @select_not_pred(%arg0: tensor<2x3xi1>, %arg1: tensor<2x3xi32>, %arg2: tensor<2x3xi32>) -> tensor<2x3xi32> {
  %0 = tosa.logical_not %arg0 : (tensor<2x3xi1>) -> tensor<2x3xi1>
  %1 = tosa.select %0, %arg1, %arg2 : (tensor<2x3xi1>, tensor<2x3xi32>, tensor<2x3xi32>) -> tensor<2x3xi32>
  // CHECK: tosa.select %arg0, %arg2, %arg1
  return %1 : tensor<2x3xi32>
}

// -----

// CHECK-LABEL: @reduce_all_fold
func.func @reduce_all_fold(%arg0: tensor<?x1xf32>) -> tensor<?x1xf32> {
  // CHECK: return %arg0
  %0 = tosa.reduce_all %arg0 {axis = 1 : i32}: (tensor<?x1xf32>) -> tensor<?x1xf32>
  return %0 : tensor<?x1xf32>
}

// -----

// CHECK-LABEL: @reduce_all_nofold
func.func @reduce_all_nofold(%arg0: tensor<?x1xf32>) -> tensor<?x1xf32> {
  // CHECK: tosa.reduce_all
  %0 = tosa.reduce_all %arg0 {axis = 0 : i32}: (tensor<?x1xf32>) -> tensor<?x1xf32>
  return %0 : tensor<?x1xf32>
}

// -----

// CHECK-LABEL: @reduce_any_fold
func.func @reduce_any_fold(%arg0: tensor<?x1xf32>) -> tensor<?x1xf32> {
  // CHECK: return %arg0
  %0 = tosa.reduce_any %arg0 {axis = 1 : i32}: (tensor<?x1xf32>) -> tensor<?x1xf32>
  return %0 : tensor<?x1xf32>
}

// -----

// CHECK-LABEL: @reduce_any_nofold
func.func @reduce_any_nofold(%arg0: tensor<?x1xf32>) -> tensor<?x1xf32> {
  // CHECK: tosa.reduce_any
  %0 = tosa.reduce_any %arg0 {axis = 0 : i32}: (tensor<?x1xf32>) -> tensor<?x1xf32>
  return %0 : tensor<?x1xf32>
}

// -----

// CHECK-LABEL: @reduce_max_fold
func.func @reduce_max_fold(%arg0: tensor<?x1xf32>) -> tensor<?x1xf32> {
  // CHECK: return %arg0
  %0 = tosa.reduce_max %arg0 {axis = 1 : i32}: (tensor<?x1xf32>) -> tensor<?x1xf32>
  return %0 : tensor<?x1xf32>
}

// -----

// CHECK-LABEL: @reduce_max_nofold
func.func @reduce_max_nofold(%arg0: tensor<?x1xf32>) -> tensor<?x1xf32> {
  // CHECK: tosa.reduce_max
  %0 = tosa.reduce_max %arg0 {axis = 0 : i32}: (tensor<?x1xf32>) -> tensor<?x1xf32>
  return %0 : tensor<?x1xf32>
}

// -----

// CHECK-LABEL: @reduce_min_fold
func.func @reduce_min_fold(%arg0: tensor<?x1xf32>) -> tensor<?x1xf32> {
  // CHECK: return %arg0
  %0 = tosa.reduce_min %arg0 {axis = 1 : i32}: (tensor<?x1xf32>) -> tensor<?x1xf32>
  return %0 : tensor<?x1xf32>
}

// -----

// CHECK-LABEL: @reduce_min_nofold
func.func @reduce_min_nofold(%arg0: tensor<?x1xf32>) -> tensor<?x1xf32> {
  // CHECK: tosa.reduce_min
  %0 = tosa.reduce_min %arg0 {axis = 0 : i32}: (tensor<?x1xf32>) -> tensor<?x1xf32>
  return %0 : tensor<?x1xf32>
}

// -----

// CHECK-LABEL: @reduce_prod_fold
func.func @reduce_prod_fold(%arg0: tensor<?x1xf32>) -> tensor<?x1xf32> {
  // CHECK: return %arg0
  %0 = tosa.reduce_prod %arg0 {axis = 1 : i32}: (tensor<?x1xf32>) -> tensor<?x1xf32>
  return %0 : tensor<?x1xf32>
}

// -----

// CHECK-LABEL: @reduce_prod_nofold
func.func @reduce_prod_nofold(%arg0: tensor<?x1xf32>) -> tensor<?x1xf32> {
  // CHECK: tosa.reduce_prod
  %0 = tosa.reduce_prod %arg0 {axis = 0 : i32}: (tensor<?x1xf32>) -> tensor<?x1xf32>
  return %0 : tensor<?x1xf32>
}

// -----

// CHECK-LABEL: @reduce_sum_fold
func.func @reduce_sum_fold(%arg0: tensor<?x1xf32>) -> tensor<?x1xf32> {
  // CHECK: return %arg0
  %0 = tosa.reduce_sum %arg0 {axis = 1 : i32}: (tensor<?x1xf32>) -> tensor<?x1xf32>
  return %0 : tensor<?x1xf32>
}

// -----

// CHECK-LABEL: @reduce_sum_nofold
func.func @reduce_sum_nofold(%arg0: tensor<?x1xf32>) -> tensor<?x1xf32> {
  // CHECK: tosa.reduce_sum
  %0 = tosa.reduce_sum %arg0 {axis = 0 : i32}: (tensor<?x1xf32>) -> tensor<?x1xf32>
  return %0 : tensor<?x1xf32>
}

// -----

// CHECK-LABEL: @reshape_canonicalize
func.func @reshape_canonicalize(%arg0: tensor<?x10xf32>) -> tensor<?x10xf32> {
  // CHECK: return %arg0
  %0 = tosa.reshape %arg0 {new_shape = array<i64: -1, 10>}: (tensor<?x10xf32>) -> tensor<?x10xf32>
  return %0 : tensor<?x10xf32>
}

// -----

// CHECK-LABEL: @reshape_canonicalize_dyn_nofold
func.func @reshape_canonicalize_dyn_nofold(%arg0: tensor<?x?x10xf32>) -> tensor<?x?x10xf32> {
  // CHECK: %[[VAR0:.+]] = tosa.reshape %arg0 {new_shape = array<i64: -1, 2, 10>} : (tensor<?x?x10xf32>) -> tensor<?x?x10xf32>
  // CHECK: return %[[VAR0]] : tensor<?x?x10xf32>
  %0 = tosa.reshape %arg0 {new_shape = array<i64: -1, 2, 10>} : (tensor<?x?x10xf32>) -> tensor<?x?x10xf32>
  return %0 : tensor<?x?x10xf32>
}

// -----

// CHECK-LABEL: @reshape_canonicalize_double
func.func @reshape_canonicalize_double(%arg0: tensor<?x10xf32>) -> tensor<?x5xf32> {
  // CHECK: %[[VAL_1:.*]] = tosa.reshape %arg0 {new_shape = array<i64: -1, 5>}
  // CHECK: return %[[VAL_1]]
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 5, -1>}: (tensor<?x10xf32>) -> tensor<5x?xf32>
  %1 = tosa.reshape %0 {new_shape = array<i64: -1, 5>}: (tensor<5x?xf32>) -> tensor<?x5xf32>
  return %1 : tensor<?x5xf32>
}

// -----

// CHECK-LABEL: @reshape_canonicalize_const
func.func @reshape_canonicalize_const() -> tensor<1x5xi32> {
  // CHECK: %[[VAR0:.+]] = "tosa.const"() <{value = dense<{{\[\[}}0, 1, 2, 3, 4]]> : tensor<1x5xi32>}
  // CHECK: return %[[VAR0]]
  %0 = "tosa.const"() {value = dense<[0, 1, 2, 3, 4]> : tensor<5xi32>} : () -> tensor<5xi32>
  %1 = tosa.reshape %0 {new_shape = array<i64: 1, 5>} : (tensor<5xi32>) -> tensor<1x5xi32>
  return %1 : tensor<1x5xi32>
}

// -----

// CHECK-LABEL: @reshape_canonicalize_const_dynamic
func.func @reshape_canonicalize_const_dynamic() -> tensor<1x?xi32> {
  // CHECK: tosa.reshape
  %0 = "tosa.const"() {value = dense<[0, 1, 2, 3, 4]> : tensor<5xi32>} : () -> tensor<5xi32>
  %1 = tosa.reshape %0 {new_shape = array<i64: 1, 5>} : (tensor<5xi32>) -> tensor<1x?xi32>
  return %1 : tensor<1x?xi32>
}

// -----

// CHECK-LABEL: @reshape_canonicalize_const_splat
func.func @reshape_canonicalize_const_splat() -> (tensor<10xi32>, tensor<1x10xi32>) {
  // CHECK-DAG: %[[VAR0:.+]] = "tosa.const"() <{value = dense<0> : tensor<10xi32>}
  // CHECK-DAG: %[[VAR1:.+]] = "tosa.const"() <{value = dense<0> : tensor<1x10xi32>}
  // CHECK: return %[[VAR0]], %[[VAR1]]
  %0 = "tosa.const"() {value = dense<0> : tensor<10xi32>} : () -> tensor<10xi32>
  %1 = tosa.reshape %0 {new_shape = array<i64: 1, 10>} : (tensor<10xi32>) -> tensor<1x10xi32>
  return %0 , %1 : tensor<10xi32>, tensor<1x10xi32>
}

// -----

// CHECK-LABEL: @reshape_canonicalize_const_sparse
func.func @reshape_canonicalize_const_sparse() -> (tensor<3xi32>, tensor<1x3xi32>) {
  // CHECK: tosa.reshape
  %0 = "tosa.const"() {value = dense<[1, 2, 3]> : tensor<3xi32>} : ()-> tensor<3xi32>
  %1 = tosa.reshape %0 {new_shape = array<i64: 1, 3>} : (tensor<3xi32>) -> tensor<1x3xi32>
  return %0 , %1 : tensor<3xi32>, tensor<1x3xi32>
}

// -----

// CHECK-LABEL: @reshape_canonicalize_quant_nofold
func.func @reshape_canonicalize_quant_nofold() -> (tensor<1x3x!quant.uniform<i8:f32, 1.000000e+00>>) {
  // disabled folding for quantized element types
  // CHECK{LITERAL}: "tosa.const"() <{value = dense<[1, 2, 3]> : tensor<3xi8>}> : () -> tensor<3x!quant.uniform<i8:f32, 1.000000e+00>>
  // CHECK{LITERAL}: tosa.reshape %0 {new_shape = array<i64: 1, 3>} : (tensor<3x!quant.uniform<i8:f32, 1.000000e+00>>) -> tensor<1x3x!quant.uniform<i8:f32, 1.000000e+00>>
  %0 = "tosa.const"() {value = dense<[1, 2, 3]> : tensor<3xi8>} : ()-> tensor<3x!quant.uniform<i8:f32, 1.000000e+00>>
  %1 = tosa.reshape %0 {new_shape = array<i64: 1, 3>} : (tensor<3x!quant.uniform<i8:f32, 1.000000e+00>>) -> tensor<1x3x!quant.uniform<i8:f32, 1.000000e+00>>
  return %1 :  tensor<1x3x!quant.uniform<i8:f32, 1.000000e+00>>
}

// -----

// CHECK-LABEL: @transpose_canonicalize_strip_quant
func.func @transpose_canonicalize_strip_quant() -> (tensor<2x1x3x!quant.uniform<i8:f32, 1.000000e+00>>) {
  // CHECK: "tosa.const"() <{value = dense<0> : tensor<1x2x3xi8>}> : () -> tensor<1x2x3x!quant.uniform<i8:f32, 1.000000e+00>>
  // CHECK: tosa.reshape %0 {new_shape = array<i64: 2, 1, 3>} : (tensor<1x2x3x!quant.uniform<i8:f32, 1.000000e+00>>) -> tensor<2x1x3x!quant.uniform<i8:f32, 1.000000e+00>>
  %perms = "tosa.const"() {value = dense<[1, 0, 2]> : tensor<3xi32>} : () -> tensor<3xi32>
  %0 = "tosa.const"() {value = dense<0> : tensor<1x2x3xi8>} : ()-> tensor<1x2x3x!quant.uniform<i8:f32, 1.000000e+00>>
  %1 = tosa.transpose %0, %perms : (tensor<1x2x3x!quant.uniform<i8:f32, 1.000000e+00>>, tensor<3xi32>) -> tensor<2x1x3x!quant.uniform<i8:f32, 1.000000e+00>>
  return %1 :  tensor<2x1x3x!quant.uniform<i8:f32, 1.000000e+00>>
}

// -----

// CHECK-LABEL: @slice_fold
func.func @slice_fold(%arg0: tensor<3x4xf32>) -> tensor<3x4xf32> {
  // CHECK: return %arg0
  %0 = tosa.slice %arg0 { size = array<i64: 3, 4>, start = array<i64: 0, 0>}: (tensor<3x4xf32>) -> tensor<3x4xf32>
  return %0 : tensor<3x4xf32>
}

// -----

// CHECK-LABEL: @slice_nofold
func.func @slice_nofold(%arg0: tensor<?x4xf32>) -> tensor<?x4xf32> {
  // CHECK: tosa.slice
  %0 = tosa.slice %arg0 { size = array<i64: 3, 4>, start = array<i64: 0, 0>}: (tensor<?x4xf32>) -> tensor<?x4xf32>
  return %0 : tensor<?x4xf32>
}

// -----

// CHECK-LABEL: @slice_fuse
func.func @slice_fuse(%arg0: tensor<3x4xf32>) -> tensor<1x2xf32> {
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<3x4xf32>) -> tensor<1x2xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.slice [[PARAM_0_]] {size = array<i64: 1, 2>, start = array<i64: 0, 0>} : (tensor<3x4xf32>) -> tensor<1x2xf32>
// CHECK:           return [[VAR_0_]] : tensor<1x2xf32>
  %0 = tosa.slice %arg0 { size = array<i64: 2, 3>, start = array<i64: 0, 0>}: (tensor<3x4xf32>) -> tensor<2x3xf32>
  %1 = tosa.slice %0 { size = array<i64: 1, 2>, start = array<i64: 0, 0>}: (tensor<2x3xf32>) -> tensor<1x2xf32>
  return %1 : tensor<1x2xf32>
}

// -----

// CHECK-LABEL: @slice_fuse_different_step
func.func @slice_fuse_different_step(%arg0: tensor<3x4xf32>) -> tensor<1x1xf32> {
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<3x4xf32>) -> tensor<1x1xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.slice [[PARAM_0_]] {size = array<i64: 1, 1>, start = array<i64: 0, 0>} : (tensor<3x4xf32>) -> tensor<1x1xf32>
// CHECK:           return [[VAR_0_]] : tensor<1x1xf32>
  %0 = tosa.slice %arg0 { size = array<i64: 1, 3>, start = array<i64: 0, 0>}: (tensor<3x4xf32>) -> tensor<1x3xf32>
  %1 = tosa.slice %0 { size = array<i64: 1, 1>, start = array<i64: 0, 0>}: (tensor<1x3xf32>) -> tensor<1x1xf32>
  return %1 : tensor<1x1xf32>
}

// -----

// CHECK-LABEL: @slice_fuse_different_start
func.func @slice_fuse_different_start(%arg0: tensor<3x4xf32>) -> tensor<1x1xf32> {
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<3x4xf32>) -> tensor<1x1xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.slice [[PARAM_0_]] {size = array<i64: 1, 1>, start = array<i64: 1, 0>} : (tensor<3x4xf32>) -> tensor<1x1xf32>
// CHECK:           return [[VAR_0_]] : tensor<1x1xf32>
  %0 = tosa.slice %arg0 { size = array<i64: 1, 3>, start = array<i64: 1, 0>}: (tensor<3x4xf32>) -> tensor<1x3xf32>
  %1 = tosa.slice %0 { size = array<i64: 1, 1>, start = array<i64: 0, 0>}: (tensor<1x3xf32>) -> tensor<1x1xf32>
  return %1 : tensor<1x1xf32>
}

// -----

// CHECK-LABEL: @slice_fuse_different_start_2
func.func @slice_fuse_different_start_2(%arg0: tensor<10x10xf32>) -> tensor<1x1xf32> {
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<10x10xf32>) -> tensor<1x1xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.slice [[PARAM_0_]] {size = array<i64: 1, 1>, start = array<i64: 4, 1>} : (tensor<10x10xf32>) -> tensor<1x1xf32>
// CHECK:           return [[VAR_0_]] : tensor<1x1xf32>
  %0 = tosa.slice %arg0 { size = array<i64: 5, 5>, start = array<i64: 4, 0>}: (tensor<10x10xf32>) -> tensor<5x5xf32>
  %1 = tosa.slice %0  { size = array<i64: 3, 3>, start = array<i64: 0, 0>}: (tensor<5x5xf32>) -> tensor<3x3xf32>
  %2 = tosa.slice %1 { size = array<i64: 1, 1>, start = array<i64: 0, 1>}: (tensor<3x3xf32>) -> tensor<1x1xf32>
  return %2 : tensor<1x1xf32>
}

// -----

// CHECK-LABEL: @slice_fuse_different_start_3
func.func @slice_fuse_different_start_3(%arg0: tensor<10x10xf32>) -> tensor<1x1xf32> {
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<10x10xf32>) -> tensor<1x1xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.slice [[PARAM_0_]] {size = array<i64: 1, 1>, start = array<i64: 4, 2>} : (tensor<10x10xf32>) -> tensor<1x1xf32>
// CHECK:           return [[VAR_0_]] : tensor<1x1xf32>
  %0 = tosa.slice %arg0 { size = array<i64: 5, 5>, start = array<i64: 4, 1>}: (tensor<10x10xf32>) -> tensor<5x5xf32>
  %1 = tosa.slice %0  { size = array<i64: 3, 3>, start = array<i64: 0, 0>}: (tensor<5x5xf32>) -> tensor<3x3xf32>
  %2 = tosa.slice %1 { size = array<i64: 1, 1>, start = array<i64: 0, 1>}: (tensor<3x3xf32>) -> tensor<1x1xf32>
  return %2 : tensor<1x1xf32>
}

// -----

// CHECK-LABEL:  func.func @slice_fuse_different_start_dynamic
func.func @slice_fuse_different_start_dynamic(%arg0: tensor<*xf32>) -> tensor<*xf32> {
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<*xf32>) -> tensor<*xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.slice [[PARAM_0_]] {size = array<i64: 1, 1>, start = array<i64: 4, 1>} : (tensor<*xf32>) -> tensor<*xf32>
// CHECK:           return [[VAR_0_]] : tensor<*xf32>
  %0 = tosa.slice %arg0 { size = array<i64: 5, 5>, start = array<i64: 4, 0>}: (tensor<*xf32>) -> tensor<*xf32>
  %1 = tosa.slice %0  { size = array<i64: 3, 3>, start = array<i64: 0, 0>}: (tensor<*xf32>) -> tensor<*xf32>
  %2 = tosa.slice %1 { size = array<i64: 1, 1>, start = array<i64: 0, 1>}: (tensor<*xf32>) -> tensor<*xf32>
  return %2 : tensor<*xf32>
}

// -----

// CHECK-LABEL: @tile_fold
func.func @tile_fold(%arg0: tensor<3x4xf32>) -> tensor<3x4xf32> {
  // CHECK: return %arg0
  %cst = tosa.const_shape { value = dense<1> : tensor<2xindex> } : () -> !tosa.shape<2>
  %0 = tosa.tile %arg0, %cst: (tensor<3x4xf32>, !tosa.shape<2>) -> tensor<3x4xf32>
  return %0 : tensor<3x4xf32>
}

// -----

// CHECK-LABEL:  func.func @tile_fuse_consecutive
func.func @tile_fuse_consecutive(%arg0: tensor<3x4xf32>) -> tensor<6x16xf32> { 
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<3x4xf32>) -> tensor<6x16xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.const_shape  {value = dense<[2, 4]> : tensor<2xindex>} : () -> !tosa.shape<2>
// CHECK:           [[VAR_1_:%.+]] = tosa.tile [[PARAM_0_]], [[VAR_0_]] : (tensor<3x4xf32>, !tosa.shape<2>) -> tensor<6x16xf32>
// CHECK:           return [[VAR_1_]] : tensor<6x16xf32>
  %cst = tosa.const_shape { value = dense<[1, 2]> : tensor<2xindex> } : () -> !tosa.shape<2>
  %cst_1 = tosa.const_shape { value = dense<[2, 2]> : tensor<2xindex> } : () -> !tosa.shape<2>
  %0 = tosa.tile %arg0, %cst: (tensor<3x4xf32>, !tosa.shape<2>) -> tensor<3x8xf32>
  %1 = tosa.tile %0, %cst_1: (tensor<3x8xf32>, !tosa.shape<2>) -> tensor<6x16xf32>
  return %1 : tensor<6x16xf32>
}

// -----

// CHECK-LABEL:  func.func @tile_no_fold_consecutive_multi_use
func.func @tile_no_fold_consecutive_multi_use(%arg0: tensor<3x4xf32>) -> (tensor<3x8xf32>, tensor<6x16xf32>) {
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<3x4xf32>) -> (tensor<3x8xf32>, tensor<6x16xf32>) {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.const_shape  {value = dense<[1, 2]> : tensor<2xindex>} : () -> !tosa.shape<2>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.const_shape  {value = dense<2> : tensor<2xindex>} : () -> !tosa.shape<2>
// CHECK:           [[VAR_2_:%.+]] = tosa.tile [[PARAM_0_]], [[VAR_0_]] : (tensor<3x4xf32>, !tosa.shape<2>) -> tensor<3x8xf32>
// CHECK:           [[VAR_3_:%.+]] = tosa.tile [[VAR_2_]], [[VAR_1_]] : (tensor<3x8xf32>, !tosa.shape<2>) -> tensor<6x16xf32>
// CHECK:           return [[VAR_2_]], [[VAR_3_]] : tensor<3x8xf32>, tensor<6x16xf32>
  %cst = tosa.const_shape { value = dense<[1, 2]> : tensor<2xindex> } : () -> !tosa.shape<2>
  %cst_1 = tosa.const_shape { value = dense<[2, 2]> : tensor<2xindex> } : () -> !tosa.shape<2>
  %0 = tosa.tile %arg0, %cst : (tensor<3x4xf32>, !tosa.shape<2>) -> tensor<3x8xf32>
  %1 = tosa.tile %0, %cst_1 : (tensor<3x8xf32>, !tosa.shape<2>) -> tensor<6x16xf32>
  return %0, %1 : tensor<3x8xf32>, tensor<6x16xf32>
}

// -----

// CHECK-LABEL: @tile_nofold
func.func @tile_nofold(%arg0: tensor<3x4xf32>) -> tensor<3x8xf32> {
  // CHECK: tosa.tile
  %cst = tosa.const_shape { value = dense<[1, 2]> : tensor<2xindex> } : () -> !tosa.shape<2>
  %0 = tosa.tile %arg0, %cst: (tensor<3x4xf32>, !tosa.shape<2>) -> tensor<3x8xf32>
  return %0 : tensor<3x8xf32>
}

// -----

// CHECK-LABEL: @transpose_no_op
func.func @transpose_no_op(%arg0: tensor<3x4x5x6xf32>) -> tensor<3x4x5x6xf32> {
  // CHECK: return %arg0
  // CHECK-NOT: tosa.transpose
  %perms = "tosa.const"() {value = dense<[0, 1, 2, 3]> : tensor<4xi32>} : () -> tensor<4xi32>
  %1 = tosa.transpose %arg0, %perms : (tensor<3x4x5x6xf32>, tensor<4xi32>) -> tensor<3x4x5x6xf32>
  return %1 : tensor<3x4x5x6xf32>
}

// -----

// CHECK-LABEL: @transpose_is_reshape
func.func @transpose_is_reshape(%arg0: tensor<1x4x5x1xf32>) -> tensor<1x4x1x5xf32> {
  // CHECK: tosa.reshape %arg0 {new_shape = array<i64: 1, 4, 1, 5>} : (tensor<1x4x5x1xf32>) -> tensor<1x4x1x5xf32>
  %perms = "tosa.const"() <{value = dense<[3, 1, 0, 2]> : tensor<4xi32>}> : () -> tensor<4xi32>
  %0 = tosa.transpose %arg0, %perms : (tensor<1x4x5x1xf32>, tensor<4xi32>) -> tensor<1x4x1x5xf32>
  return %0 : tensor<1x4x1x5xf32>
}

// -----

// CHECK-LABEL: @single_bit_reshape
// https://github.com/llvm/llvm-project/issues/55440
func.func @single_bit_reshape() -> tensor<1xi1> {
  // CHECK: "tosa.const"() <{value = dense<true> : tensor<1xi1>}
  %0 = arith.constant dense<true> : tensor<1x1xi1>
  %1 = tosa.reshape %0 {new_shape = array<i64: 1>} : (tensor<1x1xi1>) -> tensor<1xi1>
  return %1 : tensor<1xi1>
}

// -----

// CHECK-LABEL: @fold_resize_nearest
func.func @fold_resize_nearest(%arg0 : tensor<1x15x13x1xi8>) -> tensor<1x15x13x1xi8> {
  // CHECK: return %arg0
  %resize = tosa.resize %arg0 {mode = "NEAREST_NEIGHBOR" , scale = array<i64: 2, 2, 1, 1>, offset = array<i64: 0, 0>, border = array<i64: 0, 0>} : (tensor<1x15x13x1xi8>) -> tensor<1x15x13x1xi8>
  return %resize : tensor<1x15x13x1xi8>
}

// -----

// CHECK-LABEL: @fold_resize_bilinear
func.func @fold_resize_bilinear(%arg0 : tensor<1x15x13x1xi8>) -> tensor<1x15x13x1xi8> {
  // CHECK: return %arg0
  %resize = tosa.resize %arg0 {mode = "BILINEAR" , scale = array<i64: 2, 2, 1, 1>, offset = array<i64: 0, 0>, border = array<i64: 0, 0>} : (tensor<1x15x13x1xi8>) -> tensor<1x15x13x1xi8>
  return %resize : tensor<1x15x13x1xi8>
}

// -----

// CHECK-LABEL: @canonicalize_concat_slice_final_axis
// CHECK-SAME: %[[VAL_0:.*]]: tensor<1x12x12x1xf32>, %[[VAL_1:.*]]: tensor<1x12x12x1xf32>
// CHECK: return %[[VAL_0]], %[[VAL_1]] : tensor<1x12x12x1xf32>, tensor<1x12x12x1xf32>
func.func @canonicalize_concat_slice_final_axis(%arg0 : tensor<1x12x12x1xf32>, %arg1 : tensor<1x12x12x1xf32>) -> (tensor<1x12x12x1xf32>, tensor<1x12x12x1xf32>) {
  %0 = tosa.concat %arg0, %arg1 {axis = 3 : i32} : (tensor<1x12x12x1xf32>, tensor<1x12x12x1xf32>) -> tensor<1x12x12x2xf32>
  %1 = tosa.slice %0 {size = array<i64: 1, 12, 12, 1>, start = array<i64: 0, 0, 0, 0>} : (tensor<1x12x12x2xf32>) -> tensor<1x12x12x1xf32>
  %2 = tosa.slice %0 {size = array<i64: 1, 12, 12, 1>, start = array<i64: 0, 0, 0, 1>} : (tensor<1x12x12x2xf32>) -> tensor<1x12x12x1xf32>
  return %1, %2 : tensor<1x12x12x1xf32>, tensor<1x12x12x1xf32>
}

// -----

// CHECK-LABEL: @canonicalize_concat_slice_middle_axis
// CHECK-SAME: %[[VAL_0:.*]]: tensor<1x12x12xf32>, %[[VAL_1:.*]]: tensor<1x12x12xf32>
// CHECK: return %[[VAL_0]], %[[VAL_1]] : tensor<1x12x12xf32>, tensor<1x12x12xf32>
func.func @canonicalize_concat_slice_middle_axis(%arg0 : tensor<1x12x12xf32>, %arg1 : tensor<1x12x12xf32>) -> (tensor<1x12x12xf32>, tensor<1x12x12xf32>) {
  %0 = tosa.concat %arg0, %arg1 {axis = 1 : i32} : (tensor<1x12x12xf32>, tensor<1x12x12xf32>) -> tensor<1x24x12xf32>
  %1 = tosa.slice %0 {size = array<i64: 1, 12, 12>, start = array<i64: 0, 0, 0>} : (tensor<1x24x12xf32>) -> tensor<1x12x12xf32>
  %2 = tosa.slice %0 {size = array<i64: 1, 12, 12>, start = array<i64: 0, 12, 0>} : (tensor<1x24x12xf32>) -> tensor<1x12x12xf32>
  return %1, %2 : tensor<1x12x12xf32>, tensor<1x12x12xf32>
}

// -----

// CHECK-LABEL: @canonicalize_cross_concat_inputs
// CHECK-SAME: %[[VAL_0:.*]]: tensor<1x12x12xf32>, %[[VAL_1:.*]]: tensor<1x12x12xf32>
// CHECK: %[[VAL_2:.*]] = tosa.concat %[[VAL_0]], %[[VAL_1]] {axis = 2 : i32} : (tensor<1x12x12xf32>, tensor<1x12x12xf32>) -> tensor<1x12x24xf32>
// CHECK: %[[VAL_3:.*]] = tosa.slice %[[VAL_2]] {size = array<i64: 1, 12, 15>, start = array<i64: 0, 0, 0>} : (tensor<1x12x24xf32>) -> tensor<1x12x15xf32>
// CHECK: %[[VAL_4:.*]] = tosa.slice %[[VAL_2]] {size = array<i64: 1, 12, 20>, start = array<i64: 0, 0, 4>} : (tensor<1x12x24xf32>) -> tensor<1x12x20xf32>
// CHECK: return %[[VAL_3]], %[[VAL_4]] : tensor<1x12x15xf32>, tensor<1x12x20xf32>
func.func @canonicalize_cross_concat_inputs(%arg0 : tensor<1x12x12xf32>, %arg1 : tensor<1x12x12xf32>) -> (tensor<1x12x15xf32>, tensor<1x12x20xf32>) {
  %0 = tosa.concat %arg0, %arg1 {axis = 2 : i32} : (tensor<1x12x12xf32>, tensor<1x12x12xf32>) -> tensor<1x12x24xf32>
  %1 = tosa.slice %0 {size = array<i64: 1, 12, 15>, start = array<i64: 0, 0, 0>} : (tensor<1x12x24xf32>) -> tensor<1x12x15xf32>
  %2 = tosa.slice %0 {size = array<i64: 1, 12, 20>, start = array<i64: 0, 0, 4>} : (tensor<1x12x24xf32>) -> tensor<1x12x20xf32>
  return %1, %2 : tensor<1x12x15xf32>, tensor<1x12x20xf32>
}

// -----

// xHECK-LABEL: @canonicalize_concat_slice_on_non_concat_axis
// xHECK-SAME: %[[VAL_0:.*]]: tensor<1x12x12xf32>, %[[VAL_1:.*]]: tensor<1x12x12xf32>
// xHECK: %[[VAL_2:.*]] = tosa.slice %[[VAL_0]] {size = array<i64: 1, 6, 12>, start = array<i64: 0, 0, 0>} : (tensor<1x12x12xf32>) -> tensor<1x6x12xf32>
// TODO: This upstream test case seems broken because the start of the next line (12) is out of bounds with the input shape
// xHECK: %[[VAL_3:.*]] = tosa.slice %[[VAL_1]] {size = array<i64: 1, 3, 12>, start = array<i64: 1, 3, 0>} : (tensor<1x12x12xf32>) -> tensor<1x3x12xf32>
// xHECK: return %[[VAL_2]], %[[VAL_3]] : tensor<1x6x12xf32>, tensor<1x3x12xf32>
// func.func @canonicalize_concat_slice_on_non_concat_axis(%arg0 : tensor<1x12x12xf32>, %arg1 : tensor<1x12x12xf32>) -> (tensor<1x6x12xf32>, tensor<1x3x12xf32>) {
//   %0 = tosa.concat %arg0, %arg1 {axis = 2 : i32} : (tensor<1x12x12xf32>, tensor<1x12x12xf32>) -> tensor<1x12x24xf32>
//   %1 = tosa.slice %0 {size = array<i64: 1, 6, 12>, start = array<i64: 0, 0, 0>} : (tensor<1x12x24xf32>) -> tensor<1x6x12xf32>
//   %2 = tosa.slice %0 {size = array<i64: 1, 3, 12>, start = array<i64: 1, 3, 12>} : (tensor<1x12x24xf32>) -> tensor<1x3x12xf32>
//   return %1, %2 : tensor<1x6x12xf32>, tensor<1x3x12xf32>
// }

// -----

// CHECK-LABEL:  func.func @canonicalize_concat_slice_partial_concat_start_overlap
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x12x12x2xf32>, [[PARAM_1_:%.+]]: tensor<1x12x12x2xf32>, [[PARAM_2_:%.+]]: tensor<1x12x12x2xf32>) -> tensor<1x12x12x2xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 3 : i32} : (tensor<1x12x12x2xf32>, tensor<1x12x12x2xf32>) -> tensor<1x12x12x4xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.slice [[VAR_0_]] {size = array<i64: 1, 12, 12, 2>, start = array<i64: 0, 0, 0, 1>} : (tensor<1x12x12x4xf32>) -> tensor<1x12x12x2xf32>
// CHECK:           return [[VAR_1_]] : tensor<1x12x12x2xf32>
func.func @canonicalize_concat_slice_partial_concat_start_overlap(%arg0 : tensor<1x12x12x2xf32>, %arg1 : tensor<1x12x12x2xf32>, %arg2 : tensor<1x12x12x2xf32>) -> tensor<1x12x12x2xf32> {
  %0 = tosa.concat %arg0, %arg1, %arg2 {axis = 3 : i32} : (tensor<1x12x12x2xf32>, tensor<1x12x12x2xf32>, tensor<1x12x12x2xf32>) -> tensor<1x12x12x6xf32>
  %1 = tosa.slice %0 {size = array<i64: 1, 12, 12, 2>, start = array<i64: 0, 0, 0, 1>} : (tensor<1x12x12x6xf32>) -> tensor<1x12x12x2xf32>
  return %1 : tensor<1x12x12x2xf32>
}

// -----

// CHECK-LABEL:  func.func @canonicalize_concat_slice_partial_concat_end_overlap
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x12x12x2xf32>, [[PARAM_1_:%.+]]: tensor<1x12x12x2xf32>, [[PARAM_2_:%.+]]: tensor<1x12x12x2xf32>) -> tensor<1x12x12x2xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_2_]] {axis = 3 : i32} : (tensor<1x12x12x2xf32>, tensor<1x12x12x2xf32>) -> tensor<1x12x12x4xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.slice [[VAR_0_]] {size = array<i64: 1, 12, 12, 2>, start = array<i64: 0, 0, 0, 1>} : (tensor<1x12x12x4xf32>) -> tensor<1x12x12x2xf32>
// CHECK:           return [[VAR_1_]] : tensor<1x12x12x2xf32>
func.func @canonicalize_concat_slice_partial_concat_end_overlap(%arg0 : tensor<1x12x12x2xf32>, %arg1 : tensor<1x12x12x2xf32>, %arg2 : tensor<1x12x12x2xf32>) -> tensor<1x12x12x2xf32> {
  %0 = tosa.concat %arg0, %arg1, %arg2 {axis = 3 : i32} : (tensor<1x12x12x2xf32>, tensor<1x12x12x2xf32>, tensor<1x12x12x2xf32>) -> tensor<1x12x12x6xf32>
  %1 = tosa.slice %0 {size = array<i64: 1, 12, 12, 2>, start = array<i64: 0, 0, 0, 3>} : (tensor<1x12x12x6xf32>) -> tensor<1x12x12x2xf32>
  return %1 : tensor<1x12x12x2xf32>
}

// -----

// CHECK-LABEL:  func.func @canonicalize_concat_slice_partial_concat_all_overlap
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x12x12x2xf32>, [[PARAM_1_:%.+]]: tensor<1x12x12x2xf32>, [[PARAM_2_:%.+]]: tensor<1x12x12x2xf32>) -> tensor<1x12x12x4xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_2_]] {axis = 3 : i32} : (tensor<1x12x12x2xf32>, tensor<1x12x12x2xf32>, tensor<1x12x12x2xf32>) -> tensor<1x12x12x6xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.slice [[VAR_0_]] {size = array<i64: 1, 12, 12, 4>, start = array<i64: 0, 0, 0, 1>} : (tensor<1x12x12x6xf32>) -> tensor<1x12x12x4xf32>
// CHECK:           return [[VAR_1_]] : tensor<1x12x12x4xf32>
func.func @canonicalize_concat_slice_partial_concat_all_overlap(%arg0 : tensor<1x12x12x2xf32>, %arg1 : tensor<1x12x12x2xf32>, %arg2 : tensor<1x12x12x2xf32>) -> tensor<1x12x12x4xf32> {
  %0 = tosa.concat %arg0, %arg1, %arg2 {axis = 3 : i32} : (tensor<1x12x12x2xf32>, tensor<1x12x12x2xf32>, tensor<1x12x12x2xf32>) -> tensor<1x12x12x6xf32>
  %1 = tosa.slice %0 {size = array<i64: 1, 12, 12, 4>, start = array<i64: 0, 0, 0, 1>} : (tensor<1x12x12x6xf32>) -> tensor<1x12x12x4xf32>
  return %1 : tensor<1x12x12x4xf32>
}

// -----

// CHECK-LABEL:  func.func @canonicalize_concat_slice_partial_concat_multi_use
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x12x12x2xf32>, [[PARAM_1_:%.+]]: tensor<1x12x12x2xf32>, [[PARAM_2_:%.+]]: tensor<1x12x12x2xf32>) -> (tensor<1x12x12x6xf32>, tensor<1x12x12x2xf32>) {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_2_]] {axis = 3 : i32} : (tensor<1x12x12x2xf32>, tensor<1x12x12x2xf32>, tensor<1x12x12x2xf32>) -> tensor<1x12x12x6xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.slice [[VAR_0_]] {size = array<i64: 1, 12, 12, 2>, start = array<i64: 0, 0, 0, 1>} : (tensor<1x12x12x6xf32>) -> tensor<1x12x12x2xf32>
// CHECK:           return [[VAR_0_]], [[VAR_1_]] : tensor<1x12x12x6xf32>, tensor<1x12x12x2xf32>
func.func @canonicalize_concat_slice_partial_concat_multi_use(%arg0 : tensor<1x12x12x2xf32>, %arg1 : tensor<1x12x12x2xf32>, %arg2 : tensor<1x12x12x2xf32>) -> (tensor<1x12x12x6xf32>, tensor<1x12x12x2xf32>) {
  %0 = tosa.concat %arg0, %arg1, %arg2 {axis = 3 : i32} : (tensor<1x12x12x2xf32>, tensor<1x12x12x2xf32>, tensor<1x12x12x2xf32>) -> tensor<1x12x12x6xf32>
  %1 = tosa.slice %0 {size = array<i64: 1, 12, 12, 2>, start = array<i64: 0, 0, 0, 1>} : (tensor<1x12x12x6xf32>) -> tensor<1x12x12x2xf32>
  return %0, %1 : tensor<1x12x12x6xf32>, tensor<1x12x12x2xf32>
}

// -----

// CHECK-LABEL:  func.func @canonicalize_concat_slice_zero_dim
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x12x12x2xf32>, [[PARAM_1_:%.+]]: tensor<1x12x12x2xf32>, [[PARAM_2_:%.+]]: tensor<1x12x12x2xf32>) -> tensor<1x12x12x0xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_2_]] {axis = 3 : i32} : (tensor<1x12x12x2xf32>, tensor<1x12x12x2xf32>, tensor<1x12x12x2xf32>) -> tensor<1x12x12x6xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.slice [[VAR_0_]] {size = array<i64: 1, 12, 12, 0>, start = array<i64: 0, 0, 0, 0>} : (tensor<1x12x12x6xf32>) -> tensor<1x12x12x0xf32>
// CHECK:           return [[VAR_1_]] : tensor<1x12x12x0xf32>
// CHECK:         }
func.func @canonicalize_concat_slice_zero_dim(%arg0 : tensor<1x12x12x2xf32>, %arg1 : tensor<1x12x12x2xf32>, %arg2 : tensor<1x12x12x2xf32>) -> tensor<1x12x12x0xf32> {
  %0 = tosa.concat %arg0, %arg1, %arg2 {axis = 3 : i32} : (tensor<1x12x12x2xf32>, tensor<1x12x12x2xf32>, tensor<1x12x12x2xf32>) -> tensor<1x12x12x6xf32>
  %1 = tosa.slice %0 {size = array<i64: 1, 12, 12, 0>, start = array<i64: 0, 0, 0, 0>} : (tensor<1x12x12x6xf32>) -> tensor<1x12x12x0xf32>
  return %1 : tensor<1x12x12x0xf32>
}

// -----

// CHECK-LABEL:  func.func @canonicalize_tile_slice
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x12x12x10x10x10xf32>) -> tensor<1x120x12x10x16x5xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.const_shape  {value = dense<[1, 10, 2, 2, 3, 1]> : tensor<6xindex>} : () -> !tosa.shape<6>
// CHECK:           [[VAR_1_:%.+]] = tosa.tile [[PARAM_0_]], [[VAR_0_]] : (tensor<1x12x12x10x10x10xf32>, !tosa.shape<6>) -> tensor<1x120x24x20x30x10xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.slice [[VAR_1_]] {size = array<i64: 1, 120, 12, 10, 16, 5>, start = array<i64: 0, 0, 1, 1, 8, 1>} : (tensor<1x120x24x20x30x10xf32>) -> tensor<1x120x12x10x16x5xf32>
// CHECK:           return [[VAR_2_]] : tensor<1x120x12x10x16x5xf32>
func.func @canonicalize_tile_slice(%arg0 : tensor<1x12x12x10x10x10xf32>) -> tensor<1x120x12x10x16x5xf32> {
  %cst = tosa.const_shape { value = dense<[10, 10, 10, 10, 10, 10]> : tensor<6xindex> } : () -> !tosa.shape<6>
  %0 = tosa.tile %arg0, %cst : (tensor<1x12x12x10x10x10xf32>, !tosa.shape<6>) -> tensor<10x120x120x100x100x100xf32>
  %1 = tosa.slice %0 {size = array<i64: 1, 120, 12, 10, 16, 5>, start = array<i64: 0, 0, 1, 1, 18, 1>} : (tensor<10x120x120x100x100x100xf32>) -> tensor<1x120x12x10x16x5xf32>
  return  %1 :  tensor<1x120x12x10x16x5xf32>
}

// -----

// CHECK-LABEL:  func.func @canonicalize_tile_slice_fold
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x12x12x10x10x10xf32>) -> tensor<1x12x12x10x10x10xf32> {
// CHECK:           return [[PARAM_0_]] : tensor<1x12x12x10x10x10xf32>
func.func @canonicalize_tile_slice_fold(%arg0 : tensor<1x12x12x10x10x10xf32>) -> tensor<1x12x12x10x10x10xf32> {
   %cst = tosa.const_shape { value = dense<[10, 10, 10, 10, 10, 10]> : tensor<6xindex> } : () -> !tosa.shape<6>
  %0 = tosa.tile %arg0, %cst : (tensor<1x12x12x10x10x10xf32>, !tosa.shape<6>) -> tensor<10x120x120x100x100x100xf32>
  %1 = tosa.slice %0 {size = array<i64: 1, 12, 12, 10, 10, 10>, start = array<i64: 0, 24, 12, 10, 10, 0>} : (tensor<10x120x120x100x100x100xf32>) -> tensor<1x12x12x10x10x10xf32>
  return  %1 :  tensor<1x12x12x10x10x10xf32>
}

// -----

// CHECK-LABEL:  func.func @canonicalize_self_concat_slice
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x2x3x4xf32>) -> tensor<1x2x3x4xf32> {
// CHECK:           return [[PARAM_0_]] : tensor<1x2x3x4xf32>
func.func @canonicalize_self_concat_slice(%arg0 : tensor<1x2x3x4xf32>) -> tensor<1x2x3x4xf32> {
  %0 = tosa.concat %arg0, %arg0 {axis = 3 : i32} : (tensor<1x2x3x4xf32>, tensor<1x2x3x4xf32>) -> tensor<1x2x3x8xf32>
  %1 = tosa.slice %0 {size = array<i64: 1, 2, 3, 4>, start = array<i64: 0, 0, 0, 0>} : (tensor<1x2x3x8xf32>) -> tensor<1x2x3x4xf32>
  return  %1 :  tensor<1x2x3x4xf32>
}

// -----

// CHECK-LABEL:  func.func @canonicalize_tile_slice_zero_dim
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x12x12x10x10xf32>) -> tensor<1x0x12x10x16xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.const_shape  {value = dense<10> : tensor<5xindex>} : () -> !tosa.shape<5>
// CHECK:           [[VAR_1_:%.+]] = tosa.tile [[PARAM_0_]], [[VAR_0_]] : (tensor<1x12x12x10x10xf32>, !tosa.shape<5>) -> tensor<10x120x120x100x100xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.slice [[VAR_1_]] {size = array<i64: 1, 0, 12, 10, 16>, start = array<i64: 0, 0, 1, 1, 18>} : (tensor<10x120x120x100x100xf32>) -> tensor<1x0x12x10x16xf32>
// CHECK:           return [[VAR_2_]] : tensor<1x0x12x10x16xf32>
func.func @canonicalize_tile_slice_zero_dim(%arg0 : tensor<1x12x12x10x10xf32>) -> tensor<1x0x12x10x16xf32> {
  %cst = tosa.const_shape { value = dense<[10, 10, 10, 10, 10]> : tensor<5xindex> } : () -> !tosa.shape<5>
  %0 = tosa.tile %arg0, %cst : (tensor<1x12x12x10x10xf32>, !tosa.shape<5>) -> tensor<10x120x120x100x100xf32>
  %1 = tosa.slice %0 {size = array<i64: 1, 0, 12, 10, 16>, start = array<i64: 0, 0, 1, 1, 18>} : (tensor<10x120x120x100x100xf32>) -> tensor<1x0x12x10x16xf32>
  return  %1 :  tensor<1x0x12x10x16xf32>
}

// -----

// CHECK-LABEL:  func.func @canonicalize_tile_slice_multi_output
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x12x12x10x10xf32>) -> (tensor<10x120x120x100x100xf32>, tensor<1x12x12x10x16xf32>) {
// CHECK:           [[VAR_0_:%.+]] = tosa.const_shape  {value = dense<10> : tensor<5xindex>} : () -> !tosa.shape<5>
// CHECK:           [[VAR_1_:%.+]] = tosa.tile [[PARAM_0_]], [[VAR_0_]] : (tensor<1x12x12x10x10xf32>, !tosa.shape<5>) -> tensor<10x120x120x100x100xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.slice [[VAR_1_]] {size = array<i64: 1, 12, 12, 10, 16>, start = array<i64: 0, 0, 1, 1, 18>} : (tensor<10x120x120x100x100xf32>) -> tensor<1x12x12x10x16xf32>
// CHECK:           return [[VAR_1_]], [[VAR_2_]] : tensor<10x120x120x100x100xf32>, tensor<1x12x12x10x16xf32>
func.func @canonicalize_tile_slice_multi_output(%arg0 : tensor<1x12x12x10x10xf32>) -> (tensor<10x120x120x100x100xf32>, tensor<1x12x12x10x16xf32>) {
  %cst = tosa.const_shape { value = dense<[10, 10, 10, 10, 10]> : tensor<5xindex> } : () -> !tosa.shape<5>
  %0 = tosa.tile %arg0, %cst : (tensor<1x12x12x10x10xf32>, !tosa.shape<5>) -> tensor<10x120x120x100x100xf32>
  %1 = tosa.slice %0 {size = array<i64: 1, 12, 12, 10, 16>, start = array<i64: 0, 0, 1, 1, 18>} : (tensor<10x120x120x100x100xf32>) -> tensor<1x12x12x10x16xf32>
  return  %0, %1 :  tensor<10x120x120x100x100xf32>, tensor<1x12x12x10x16xf32>
}

// -----

// The slice window lies entirely within the unpadded input, so the pad is
// dropped and the slice reads the pad's input directly.
// CHECK-LABEL:  func.func @canonicalize_pad_slice_drop_pad
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<3x51xf32>)
// CHECK-NOT:       tosa.pad
// CHECK:           [[VAR_0_:%.+]] = tosa.slice [[PARAM_0_]] {size = array<i64: 3, 1>, start = array<i64: 0, 4>} : (tensor<3x51xf32>) -> tensor<3x1xf32>
// CHECK:           return [[VAR_0_]] : tensor<3x1xf32>
func.func @canonicalize_pad_slice_drop_pad(%arg0: tensor<3x51xf32>) -> tensor<3x1xf32> {
  %padding = tosa.const_shape {value = dense<[0, 0, 0, 1]> : tensor<4xindex>} : () -> !tosa.shape<4>
  %pad_const = "tosa.const"() <{value = dense<0.000000e+00> : tensor<f32>}> : () -> tensor<f32>
  %0 = tosa.pad %arg0, %padding, %pad_const : (tensor<3x51xf32>, !tosa.shape<4>, tensor<f32>) -> tensor<3x52xf32>
  %1 = tosa.slice %0 {size = array<i64: 3, 1>, start = array<i64: 0, 4>} : (tensor<3x52xf32>) -> tensor<3x1xf32>
  return %1 : tensor<3x1xf32>
}

// -----

// The slice reads part of the leading pad, so the pad is reduced to only the
// padding still read and applied to the input sub-region the slice covers.
// CHECK-LABEL:  func.func @canonicalize_pad_slice_reduce_pad
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<3x51xf32>)
// CHECK-DAG:       [[VAL:%.+]] = "tosa.const"() <{value = dense<0.000000e+00> : tensor<f32>}> : () -> tensor<f32>
// CHECK-DAG:       [[PAD:%.+]] = tosa.const_shape  {value = dense<[0, 0, 1, 0]> : tensor<4xindex>} : () -> !tosa.shape<4>
// CHECK:           [[SLICE:%.+]] = tosa.slice [[PARAM_0_]] {size = array<i64: 3, 3>, start = array<i64: 0, 0>} : (tensor<3x51xf32>) -> tensor<3x3xf32>
// CHECK:           [[PADDED:%.+]] = tosa.pad [[SLICE]], [[PAD]], [[VAL]] : (tensor<3x3xf32>, !tosa.shape<4>, tensor<f32>) -> tensor<3x4xf32>
// CHECK:           return [[PADDED]] : tensor<3x4xf32>
func.func @canonicalize_pad_slice_reduce_pad(%arg0: tensor<3x51xf32>) -> tensor<3x4xf32> {
  %padding = tosa.const_shape {value = dense<[0, 0, 2, 2]> : tensor<4xindex>} : () -> !tosa.shape<4>
  %pad_const = "tosa.const"() <{value = dense<0.000000e+00> : tensor<f32>}> : () -> tensor<f32>
  %0 = tosa.pad %arg0, %padding, %pad_const : (tensor<3x51xf32>, !tosa.shape<4>, tensor<f32>) -> tensor<3x55xf32>
  %1 = tosa.slice %0 {size = array<i64: 3, 4>, start = array<i64: 0, 1>} : (tensor<3x55xf32>) -> tensor<3x4xf32>
  return %1 : tensor<3x4xf32>
}

// -----

// The pad has multiple uses, so it must not be modified.
// CHECK-LABEL:  func.func @canonicalize_pad_slice_multi_use
// CHECK:           tosa.pad
// CHECK:           tosa.slice
func.func @canonicalize_pad_slice_multi_use(%arg0: tensor<3x51xf32>) -> (tensor<3x52xf32>, tensor<3x1xf32>) {
  %padding = tosa.const_shape {value = dense<[0, 0, 0, 1]> : tensor<4xindex>} : () -> !tosa.shape<4>
  %pad_const = "tosa.const"() <{value = dense<0.000000e+00> : tensor<f32>}> : () -> tensor<f32>
  %0 = tosa.pad %arg0, %padding, %pad_const : (tensor<3x51xf32>, !tosa.shape<4>, tensor<f32>) -> tensor<3x52xf32>
  %1 = tosa.slice %0 {size = array<i64: 3, 1>, start = array<i64: 0, 4>} : (tensor<3x52xf32>) -> tensor<3x1xf32>
  return %0, %1 : tensor<3x52xf32>, tensor<3x1xf32>
}

// -----

// CHECK-LABEL: @canonicalize_optimize_sqrt_reciprocal
func.func @canonicalize_optimize_sqrt_reciprocal(%arg0: tensor<1x5x1x1xf32>) -> tensor<1x5x1x1xf32> {
  // CHECK: %[[RSQRT:.*]] = tosa.rsqrt %arg{{.*}} : (tensor<1x5x1x1xf32>) -> tensor<1x5x1x1xf32>
  // CHECK: return %[[RSQRT]] : tensor<1x5x1x1xf32>
  %0 = "tosa.const"() <{value = dense<5.000000e-01> : tensor<1x1x1x1xf32>}> : () -> tensor<1x1x1x1xf32>
  %1 = tosa.pow %arg0, %0 : (tensor<1x5x1x1xf32>, tensor<1x1x1x1xf32>) -> tensor<1x5x1x1xf32>
  %2 = tosa.reciprocal %1 : (tensor<1x5x1x1xf32>) -> tensor<1x5x1x1xf32>
  return %2 : tensor<1x5x1x1xf32>
}

// -----

// CHECK-LABEL: @canonicalize_optimize_sqrt_reciprocal
func.func @canonicalize_optimize_sqrt_reciprocal_bf16(%arg0: tensor<1x5x1x1xbf16>) -> tensor<1x5x1x1xbf16> {
  // CHECK: %[[RSQRT:.*]] = tosa.rsqrt %arg{{.*}} : (tensor<1x5x1x1xbf16>) -> tensor<1x5x1x1xbf16>
  // CHECK: return %[[RSQRT]] : tensor<1x5x1x1xbf16>
  %0 = "tosa.const"() <{value = dense<5.000000e-01> : tensor<1x1x1x1xbf16>}> : () -> tensor<1x1x1x1xbf16>
  %1 = tosa.pow %arg0, %0 : (tensor<1x5x1x1xbf16>, tensor<1x1x1x1xbf16>) -> tensor<1x5x1x1xbf16>
  %2 = tosa.reciprocal %1 : (tensor<1x5x1x1xbf16>) -> tensor<1x5x1x1xbf16>
  return %2 : tensor<1x5x1x1xbf16>
}

// -----

// CHECK-LABEL: @canonicalize_optimize_sqrt_reciprocal_no_match
func.func @canonicalize_optimize_sqrt_reciprocal_no_match(%arg0: tensor<1x5x1x1xf32>) -> tensor<1x5x1x1xf32> {
  // CHECK-NOT: tosa.rsqrt"(%arg{{.*}})
  %0 = "tosa.const"() <{value = dense<4.000000e-01> : tensor<1x1x1x1xf32>}> : () -> tensor<1x1x1x1xf32>
  %1 = tosa.pow %arg0, %0 : (tensor<1x5x1x1xf32>, tensor<1x1x1x1xf32>) -> tensor<1x5x1x1xf32>
  %2 = tosa.reciprocal %1 : (tensor<1x5x1x1xf32>) -> tensor<1x5x1x1xf32>
  return %2 : tensor<1x5x1x1xf32>
}

// -----

// CHECK-LABEL: @canonicalize_optimize_sqrt_reciprocal_tile_no_match
func.func @canonicalize_optimize_sqrt_reciprocal_tile_no_match(%arg0: tensor<1x5x1x1xf32>) -> tensor<1x5x7x1xf32> {
  // CHECK-NOT: tosa.rsqrt"(%arg{{.*}})
  %0 = "tosa.const"() <{value = dense<5.000000e-01> : tensor<1x1x7x1xf32>}> : () -> tensor<1x1x7x1xf32>
  %1 = tosa.pow %arg0, %0 : (tensor<1x5x1x1xf32>, tensor<1x1x7x1xf32>) -> tensor<1x5x7x1xf32>
  %2 = tosa.reciprocal %1 : (tensor<1x5x7x1xf32>) -> tensor<1x5x7x1xf32>
  return %2 : tensor<1x5x7x1xf32>
}

// -----

// CHECK-LABEL: @fold_log_exp
func.func @fold_log_exp(%arg0: tensor<?x1xf32>) -> tensor<?x1xf32> {
  // CHECK: return %arg{{.*}} : tensor<?x1xf32>
  %0 = tosa.exp %arg0 : (tensor<?x1xf32>) -> tensor<?x1xf32>
  %1 = tosa.log %0 : (tensor<?x1xf32>) -> tensor<?x1xf32>
  return %1 : tensor<?x1xf32>
}

// -----

// CHECK-LABEL: @fold_exp_log
func.func @fold_exp_log(%arg0: tensor<?x1xf32>) -> tensor<?x1xf32> {
  // CHECK: return %arg{{.*}} : tensor<?x1xf32>
  %0 = tosa.log %arg0 : (tensor<?x1xf32>) -> tensor<?x1xf32>
  %1 = tosa.exp %0 : (tensor<?x1xf32>) -> tensor<?x1xf32>
  return %1 : tensor<?x1xf32>
}

// -----

// CHECK-LABEL: @fold_negate_negate
func.func @fold_negate_negate(%arg0: tensor<?x1xf32>) -> tensor<?x1xf32> {
  // CHECK: return %arg{{.*}} : tensor<?x1xf32>
  %0 = tosa.negate %arg0 : (tensor<?x1xf32>) -> tensor<?x1xf32>
  %1 = tosa.negate %0 : (tensor<?x1xf32>) -> tensor<?x1xf32>
  return %1 : tensor<?x1xf32>
}

// -----

// CHECK-LABEL: @fold_abs_abs
func.func @fold_abs_abs(%arg0: tensor<?x1xf32>) -> tensor<?x1xf32> {
  // CHECK: %[[ABS:.*]] = tosa.abs %arg{{.*}} : (tensor<?x1xf32>) -> tensor<?x1xf32>
  // CHECK: return %[[ABS]] : tensor<?x1xf32>
  %0 = tosa.abs %arg0 : (tensor<?x1xf32>) -> tensor<?x1xf32>
  %1 = tosa.abs %0 : (tensor<?x1xf32>) -> tensor<?x1xf32>
  return %1 : tensor<?x1xf32>
}

// -----

// CHECK-LABEL: @fold_reduce_rank_zero
func.func @fold_reduce_rank_zero() {
  // CHECK-NOT: tosa.reduce_min
  // CHECK-NOT: tosa.reverse
  %0 = tensor.empty() : tensor<i32>
  %1 = tosa.reduce_min %0 {axis = 0 : i32} : (tensor<i32>) -> tensor<i32>
  %2 = tosa.reverse %0 {axis = 0 : i32} : (tensor<i32>) -> tensor<i32>
  return
}

// -----

// CHECK-LABEL: @fold_tile_rank_zero
func.func nested @fold_tile_rank_zero() -> tensor<i32> {
  // CHECK-NOT: tosa.tile
  %0 = tensor.empty() : tensor<i32>
  %cst = tosa.const_shape { value = dense<> : tensor<0xindex> } : () -> !tosa.shape<0>
  %1 = tosa.tile %0, %cst : (tensor<i32>, !tosa.shape<0>) -> tensor<i32>
  return %1 : tensor<i32>
}

// -----

// CHECK-LABEL: @reshape_quant_nofold
// check that segfault is fixed
func.func @reshape_quant_nofold() -> tensor<1x1x1x1xi32> {
   %0 = "tosa.const"() {value = dense<127> : tensor<i8>} : () -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   %1 = tosa.reshape %0 {new_shape = array<i64: 1, 1, 1, 1>} : (tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>) -> tensor<1x1x1x1x!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   %2 = tosa.rescale %1 {double_round = true, input_zp = -128 : i32, multiplier = array<i32: 1073741824>, output_zp = 0 : i32, per_channel = false, scale32 = true, shift = array<i8: 30>} : (tensor<1x1x1x1x!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>) -> tensor<1x1x1x1xi32>
   return %2 : tensor<1x1x1x1xi32>
}

// -----

// CHECK-LABEL: @add_quant_nofold
// check that segfault is fixed
func.func @add_quant_nofold() -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>> {
   %0 = "tosa.const"() {value = dense<127> : tensor<i8>} : () -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   %1 = tosa.add %0, %0 : (tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>, tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>) -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   return %1 : tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
}

// -----

// CHECK-LABEL: @sub_quant_nofold
// check that segfault is fixed
func.func @sub_quant_nofold() -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>> {
   %0 = "tosa.const"() {value = dense<127> : tensor<i8>} : () -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   %1 = tosa.sub %0, %0 : (tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>, tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>) -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   return %1 : tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
}

// -----

// CHECK-LABEL: @greater_quant_fold
func.func @greater_quant_fold() -> tensor<i1> {
   %0 = "tosa.const"() {value = dense<0> : tensor<i8>} : () -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   // CHECK: "tosa.const"() <{value = dense<false>
   %2 = "tosa.greater"(%0, %0) : (tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>, tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>) -> tensor<i1>
   return %2 : tensor<i1>
}

// -----

// CHECK-LABEL: @greater_equal_quant_fold
func.func @greater_equal_quant_fold() -> tensor<i1> {
   %0 = "tosa.const"() {value = dense<0> : tensor<i8>} : () -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   // CHECK: "tosa.const"() <{value = dense<true>
   %2 = "tosa.greater_equal"(%0, %0) : (tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>, tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>) -> tensor<i1>
   return %2 : tensor<i1>
}

// -----

// CHECK-LABEL: @equal_quant_fold
func.func @equal_quant_fold() -> tensor<i1> {
   %0 = "tosa.const"() {value = dense<0> : tensor<i8>} : () -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   // CHECK: "tosa.const"() <{value = dense<true>
   %2 = "tosa.equal"(%0, %0) : (tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>, tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>) -> tensor<i1>
   return %2 : tensor<i1>
}

// -----

// CHECK-LABEL: @cast_quant_nofold
func.func @cast_quant_nofold() -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:3>> {
  // CHECK: tosa.cast
   %0 = "tosa.const"() {value = dense<0> : tensor<i8>} : () -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   %1 = "tosa.cast"(%0) : (tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>) -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:3>>
   return %1 : tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:3>>
}

// -----

// CHECK-LABEL: @reverse_quant_fold
func.func @reverse_quant_fold() -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>> {
   // CHECK: %[[CST:.*]] = "tosa.const"() <{value = dense<0> : tensor<i8>}> : () -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   // CHECK: return %[[CST]]
   %0 = "tosa.const"() {value = dense<0> : tensor<i8>} : () -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   %1 = "tosa.reverse"(%0) { axis = 0 : i32 } : (tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>) -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   return %1 : tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
}

// -----

// CHECK-LABEL: @select_quant_fold
func.func @select_quant_fold() -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>> {
   // CHECK: %[[CONST_0:.*]] = "tosa.const"() <{value = dense<0> : tensor<i8>}> : () -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   // CHECK: return %[[CONST_0]]
   %0 = "tosa.const"() {value = dense<true> : tensor<i1>} : () -> tensor<i1>
   %1 = "tosa.const"() {value = dense<0> : tensor<i8>} : () -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   %2 = "tosa.const"() {value = dense<127> : tensor<i8>} : () -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   %3 = "tosa.select"(%0, %1, %2) : (tensor<i1>, tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>, tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>) -> tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   return %3 : tensor<!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
}

// -----

// CHECK-LABEL: @mul_quant_nofold
func.func @mul_quant_nofold() -> tensor<1x!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>> {
   // CHECK: tosa.mul
   %0 = "tosa.const"() {value = dense<0> : tensor<1xi8>} : () -> tensor<1x!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   %1 = "tosa.const"() {value = dense<1> : tensor<1xi8>} : () -> tensor<1x!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   %shift = "tosa.const"() <{value = dense<0> : tensor<1xi8>}> : () -> tensor<1xi8>
   %2 = tosa.mul %0, %1, %shift : (tensor<1x!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>, tensor<1x!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>, tensor<1xi8>) -> tensor<1x!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
   return %2 : tensor<1x!quant.uniform<i8:f32, 3.0757404601899907E-5:-128>>
}


// -----

// CHECK-LABEL: @fold_reciprocal
func.func nested @fold_reciprocal() -> tensor<3x600x1200xf32> {
  // CHECK:           %[[VAL_0:.*]] = "tosa.const"() <{value = dense<8.620690e-03> : tensor<3x600x1200xf32>}> : () -> tensor<3x600x1200xf32>
  // CHECK:           return %[[VAL_0]] : tensor<3x600x1200xf32>
  // CHECK:         }
  %0 = "tosa.const"(){ value = dense<116.0>: tensor<3x600x1200xf32> }: () -> tensor<3x600x1200xf32>
  %1 = "tosa.reciprocal"(%0): (tensor<3x600x1200xf32>) -> tensor<3x600x1200xf32>
  return %1 : tensor<3x600x1200xf32>
}

// -----

// CHECK-LABEL: @do_not_fold_reciprocal_int
func.func nested @do_not_fold_reciprocal_int() -> tensor<3x600x1200xi32> {
  // CHECK:           tosa.reciprocal
  %0 = "tosa.const"(){ value = dense<11>: tensor<3x600x1200xi32> }: () -> tensor<3x600x1200xi32>
  %1 = "tosa.reciprocal"(%0): (tensor<3x600x1200xi32>) -> tensor<3x600x1200xi32>
  return %1 : tensor<3x600x1200xi32>
}

// -----

// CHECK-LABEL: @canonicalize_select_to_clamp
func.func @canonicalize_select_to_clamp(%arg0: tensor<13x21x3xf32>) -> tensor<13x21x3xf32> {
// CHECK:           %[[VAL_1:.*]] = tosa.clamp %arg{{.*}} {max_fp = 0x7F800000 : f32, max_int = 9223372036854775807 : i64, min_fp = 1.500000e+00 : f32, min_int = -9223372036854775808 : i64} : (tensor<13x21x3xf32>) -> tensor<13x21x3xf32>
// CHECK:           return %[[VAL_1]] : tensor<13x21x3xf32>
  %0 =  "tosa.const"() <{value = dense<1.500000e+00> : tensor<13x21x3xf32>}>: () -> tensor<13x21x3xf32>
  %1 = tosa.greater_equal %arg0, %0: (tensor<13x21x3xf32>, tensor<13x21x3xf32>) -> tensor<13x21x3xi1>
  %2 = tosa.select %1, %arg0, %0: ( tensor<13x21x3xi1>, tensor<13x21x3xf32>, tensor<13x21x3xf32>) -> tensor<13x21x3xf32>
  return %2  :  tensor<13x21x3xf32>
}

// -----

// CHECK-LABEL: @canonicalize_select_to_clamp_not_splat
func.func @canonicalize_select_to_clamp_not_splat(%arg0: tensor<4xi32>) -> tensor<4xi32> {
// CHECK-NOT:       tosa.clamp
  %0 =  "tosa.const"() <{value = dense<[1, 2, 3, 4]> : tensor<4xi32>}>: () -> tensor<4xi32>
  %1 = tosa.greater_equal %arg0, %0: (tensor<4xi32>, tensor<4xi32>) -> tensor<4xi1>
  %2 = tosa.select %1, %arg0, %0: ( tensor<4xi1>, tensor<4xi32>, tensor<4xi32>) -> tensor<4xi32>
  return %2  :  tensor<4xi32>
}

// -----

// CHECK-LABEL: @canonicalize_select_to_clamp_bf16
func.func @canonicalize_select_to_clamp_bf16(%arg0: tensor<13x21x3xbf16>) -> tensor<13x21x3xbf16> {
// CHECK:           %[[VAL_1:.*]] = tosa.clamp %arg{{.*}} {max_fp = 0x7F80 : bf16, max_int = 9223372036854775807 : i64, min_fp = 1.500000e+00 : bf16, min_int = -9223372036854775808 : i64} : (tensor<13x21x3xbf16>) -> tensor<13x21x3xbf16>
// CHECK:           return %[[VAL_1]] : tensor<13x21x3xbf16>
  %0 =  "tosa.const"() <{value = dense<1.500000e+00> : tensor<13x21x3xbf16>}>: () -> tensor<13x21x3xbf16>
  %1 = tosa.greater_equal %arg0, %0: (tensor<13x21x3xbf16>, tensor<13x21x3xbf16>) -> tensor<13x21x3xi1>
  %2 = tosa.select %1, %arg0, %0: ( tensor<13x21x3xi1>, tensor<13x21x3xbf16>, tensor<13x21x3xbf16>) -> tensor<13x21x3xbf16>
  return %2  :  tensor<13x21x3xbf16>
}

// -----

// CHECK-LABEL: @canonicalize_select_to_clamp_ui64
func.func @canonicalize_select_to_clamp_ui64(%arg0: tensor<13x21x3xui64>) -> tensor<13x21x3xui64> {
// CHECK-NOT:       tosa.clamp
  %0 =  "tosa.const"() <{value = dense<1> : tensor<13x21x3xui64>}>: () -> tensor<13x21x3xui64>
  %1 = tosa.greater_equal %arg0, %0: (tensor<13x21x3xui64>, tensor<13x21x3xui64>) -> tensor<13x21x3xi1>
  %2 = tosa.select %1, %arg0, %0: ( tensor<13x21x3xi1>, tensor<13x21x3xui64>, tensor<13x21x3xui64>) -> tensor<13x21x3xui64>
  return %2  :  tensor<13x21x3xui64>
}

// -----

// CHECK-LABEL: @canonicalize_select_to_clamp_ui4
func.func @canonicalize_select_to_clamp_ui4(%arg0: tensor<13x21x3xui4>) -> tensor<13x21x3xui4> {
// CHECK:           %[[VAL_1:.*]] = tosa.clamp %arg{{.*}} {max_fp = 0x7F800000 : f32, max_int = 9223372036854775807 : i64, min_fp = 0xFF800000 : f32, min_int = 8 : i64} : (tensor<13x21x3xui4>) -> tensor<13x21x3xui4>
// CHECK:           return %[[VAL_1]] : tensor<13x21x3xui4>
  %0 =  "tosa.const"() <{value = dense<8> : tensor<13x21x3xui4>}>: () -> tensor<13x21x3xui4>
  %1 = tosa.greater_equal %arg0, %0: (tensor<13x21x3xui4>, tensor<13x21x3xui4>) -> tensor<13x21x3xi1>
  %2 = tosa.select %1, %arg0, %0: ( tensor<13x21x3xi1>, tensor<13x21x3xui4>, tensor<13x21x3xui4>) -> tensor<13x21x3xui4>
  return %2  :  tensor<13x21x3xui4>
}

// -----

// CHECK-LABEL: @canonicalize_select_to_clamp_i16_pat2
func.func @canonicalize_select_to_clamp_i16_pat2(%arg0: tensor<13x21x3xi16>) -> tensor<13x21x3xi16> {
// CHECK:           %[[VAL_1:.*]] = tosa.clamp %arg{{.*}} {max_fp = 0x7F800000 : f32, max_int = 3 : i64, min_fp = 0xFF800000 : f32, min_int = -9223372036854775808 : i64} : (tensor<13x21x3xi16>) -> tensor<13x21x3xi16>
// CHECK:           return %[[VAL_1]] : tensor<13x21x3xi16>
  %0 =  "tosa.const"() <{value = dense<3> : tensor<13x21x3xi16>}>: () -> tensor<13x21x3xi16>
  %1 = tosa.greater_equal %arg0, %0: (tensor<13x21x3xi16>, tensor<13x21x3xi16>) -> tensor<13x21x3xi1>
  %2 = tosa.select %1, %0, %arg0: ( tensor<13x21x3xi1>, tensor<13x21x3xi16>, tensor<13x21x3xi16>) -> tensor<13x21x3xi16>
  return %2  :  tensor<13x21x3xi16>
}
// -----

// CHECK-LABEL: @canonicalize_select_to_clamp_i8_neg
func.func @canonicalize_select_to_clamp_i8_neg(%arg0: tensor<13x21x3xi8>) -> tensor<13x21x3xi8> {
// CHECK:           %[[VAL_1:.*]] = tosa.clamp %arg{{.*}} {max_fp = 0x7F800000 : f32, max_int = 9223372036854775807 : i64, min_fp = 0xFF800000 : f32, min_int = -42 : i64} : (tensor<13x21x3xi8>) -> tensor<13x21x3xi8>
// CHECK:           return %[[VAL_1]] : tensor<13x21x3xi8>
  %0 =  "tosa.const"() <{value = dense<-42> : tensor<13x21x3xi8>}>: () -> tensor<13x21x3xi8>
  %1 = tosa.greater_equal %arg0, %0: (tensor<13x21x3xi8>, tensor<13x21x3xi8>) -> tensor<13x21x3xi1>
  %2 = tosa.select %1, %arg0, %0: ( tensor<13x21x3xi1>, tensor<13x21x3xi8>, tensor<13x21x3xi8>) -> tensor<13x21x3xi8>
  return %2  :  tensor<13x21x3xi8>
}

// -----

// CHECK-LABEL: @canonicalize_select_to_clamp_f64_pat2_neg
func.func @canonicalize_select_to_clamp_f64_pat2_neg(%arg0: tensor<13x21x3xf64>) -> tensor<13x21x3xf64> {
// CHECK:           %[[VAL_1:.*]] = tosa.clamp %arg{{.*}} {max_fp = -3.500000e+00 : f64, max_int = 9223372036854775807 : i64, min_fp = 0xFFF0000000000000 : f64, min_int = -9223372036854775808 : i64} : (tensor<13x21x3xf64>) -> tensor<13x21x3xf64>
// CHECK:           return %[[VAL_1]] : tensor<13x21x3xf64>
  %0 =  "tosa.const"() <{value = dense<-3.5> : tensor<13x21x3xf64>}>: () -> tensor<13x21x3xf64>
  %1 = tosa.greater_equal %arg0, %0: (tensor<13x21x3xf64>, tensor<13x21x3xf64>) -> tensor<13x21x3xi1>
  %2 = tosa.select %1, %0, %arg0: ( tensor<13x21x3xi1>, tensor<13x21x3xf64>, tensor<13x21x3xf64>) -> tensor<13x21x3xf64>
  return %2  :  tensor<13x21x3xf64>
}
// -----

// CHECK-LABEL: @canonicalize_select_lrelu_zero_pattern
func.func @canonicalize_select_lrelu_zero_pattern(%arg0: tensor<13x21x3xf32>) -> tensor<13x21x3xf32> {
// CHECK:           %[[VAL_1:.*]] = tosa.clamp %arg{{.*}} {max_fp = 0x7F800000 : f32, max_int = 9223372036854775807 : i64, min_fp = 0.000000e+00 : f32, min_int = -9223372036854775808 : i64} : (tensor<13x21x3xf32>) -> tensor<13x21x3xf32>
// CHECK:           return %[[VAL_1]] : tensor<13x21x3xf32>
  %0 =  "tosa.const"() <{value = dense<0.000000e+00> : tensor<1x1x1xf32>}>: () -> tensor<1x1x1xf32>
  %shift = "tosa.const"() <{value = dense<0> : tensor<1xi8>}> : () -> tensor<1xi8>
  %1 = tosa.mul %arg0, %0, %shift : (tensor<13x21x3xf32>, tensor<1x1x1xf32>, tensor<1xi8>) -> tensor<13x21x3xf32>
  %2 = tosa.greater_equal %arg0, %0: (tensor<13x21x3xf32>, tensor<1x1x1xf32>) -> tensor<13x21x3xi1>
  %3 = tosa.select %2, %arg0, %1: ( tensor<13x21x3xi1>, tensor<13x21x3xf32>, tensor<13x21x3xf32>) -> tensor<13x21x3xf32>
  return %3  :  tensor<13x21x3xf32>
}

// -----

// CHECK-LABEL: @canonicalize_select_to_clamp_i64_and_i8_pat1
func.func @canonicalize_select_to_clamp_i64_and_i8_pat1(%arg0: tensor<13x21x3xi64>, %arg1: tensor<13x21x3xi8>) -> tensor<13x21x3xi8> {
// CHECK:           %[[VAL_1:.*]] = tosa.cast %arg{{.*}} : (tensor<13x21x3xi64>) -> tensor<13x21x3xi8>
// CHECK:           %[[VAL_2:.*]] = tosa.clamp %[[VAL_1]] {max_fp = 0x7F800000 : f32, max_int = 9223372036854775807 : i64, min_fp = 0xFF800000 : f32, min_int = 42 : i64} : (tensor<13x21x3xi8>) -> tensor<13x21x3xi8>
// CHECK:           return %[[VAL_2]] : tensor<13x21x3xi8>
  %0 =  "tosa.const"() <{value = dense<42> : tensor<13x21x3xi64>}>: () -> tensor<13x21x3xi64>
  %1 =  "tosa.const"() <{value = dense<42> : tensor<13x21x3xi8>}>: () -> tensor<13x21x3xi8>
  %2 = tosa.greater_equal %arg0, %0: (tensor<13x21x3xi64>, tensor<13x21x3xi64>) -> tensor<13x21x3xi1>
  %3 = tosa.select %2, %arg1, %1: ( tensor<13x21x3xi1>, tensor<13x21x3xi8>, tensor<13x21x3xi8>) -> tensor<13x21x3xi8>
  return %3  :  tensor<13x21x3xi8>
}

// -----

// CHECK-LABEL: @canonicalize_select_to_clamp_i64_and_i8_pat2
func.func @canonicalize_select_to_clamp_i64_and_i8_pat2(%arg0: tensor<13x21x3xi64>, %arg1: tensor<13x21x3xi8>) -> tensor<13x21x3xi8> {
// CHECK:           %[[VAL_1:.*]] = tosa.cast %arg{{.*}} : (tensor<13x21x3xi64>) -> tensor<13x21x3xi8>
// CHECK:           %[[VAL_2:.*]] = tosa.clamp %[[VAL_1]] {max_fp = 0x7F800000 : f32, max_int = -42 : i64, min_fp = 0xFF800000 : f32, min_int = -9223372036854775808 : i64} : (tensor<13x21x3xi8>) -> tensor<13x21x3xi8>
// CHECK:           return %[[VAL_2]] : tensor<13x21x3xi8>
  %0 =  "tosa.const"() <{value = dense<-42> : tensor<13x21x3xi64>}>: () -> tensor<13x21x3xi64>
  %1 =  "tosa.const"() <{value = dense<-42> : tensor<13x21x3xi8>}>: () -> tensor<13x21x3xi8>
  %2 = tosa.greater_equal %arg0, %0: (tensor<13x21x3xi64>, tensor<13x21x3xi64>) -> tensor<13x21x3xi1>
  %3 = tosa.select %2, %1, %arg1 : ( tensor<13x21x3xi1>, tensor<13x21x3xi8>, tensor<13x21x3xi8>) -> tensor<13x21x3xi8>
  return %3  :  tensor<13x21x3xi8>
}

// -----

// CHECK-LABEL: @canonicalize_select_to_clamp_i8_and_i64_pat1
func.func @canonicalize_select_to_clamp_i8_and_i64_pat1(%arg0: tensor<13x21x3xi8>, %arg1: tensor<13x21x3xi64>) -> tensor<13x21x3xi64> {
// CHECK:           %[[VAL_1:.*]] = tosa.cast %arg{{.*}} : (tensor<13x21x3xi8>) -> tensor<13x21x3xi64>
// CHECK:           %[[VAL_2:.*]] = tosa.clamp %[[VAL_1]] {max_fp = 0x7F800000 : f32, max_int = 9223372036854775807 : i64, min_fp = 0xFF800000 : f32, min_int = 42 : i64} : (tensor<13x21x3xi64>) -> tensor<13x21x3xi64>
// CHECK:           return %[[VAL_2]] : tensor<13x21x3xi64>
  %0 =  "tosa.const"() <{value = dense<42> : tensor<13x21x3xi8>}>: () -> tensor<13x21x3xi8>
  %1 =  "tosa.const"() <{value = dense<42> : tensor<13x21x3xi64>}>: () -> tensor<13x21x3xi64>
  %2 = tosa.greater_equal %arg0, %0: (tensor<13x21x3xi8>, tensor<13x21x3xi8>) -> tensor<13x21x3xi1>
  %3 = tosa.select %2, %arg1, %1: ( tensor<13x21x3xi1>, tensor<13x21x3xi64>, tensor<13x21x3xi64>) -> tensor<13x21x3xi64>
  return %3  :  tensor<13x21x3xi64>
}

// -----

// CHECK-LABEL: @canonicalize_select_to_clamp_i8_and_i64_pat2
func.func @canonicalize_select_to_clamp_i8_and_i64_pat2(%arg0: tensor<13x21x3xi8>, %arg1: tensor<13x21x3xi64>) -> tensor<13x21x3xi64> {
// CHECK:           %[[VAL_1:.*]] = tosa.cast %arg{{.*}} : (tensor<13x21x3xi8>) -> tensor<13x21x3xi64>
// CHECK:           %[[VAL_2:.*]] = tosa.clamp %[[VAL_1]] {max_fp = 0x7F800000 : f32, max_int = -42 : i64, min_fp = 0xFF800000 : f32, min_int = -9223372036854775808 : i64} : (tensor<13x21x3xi64>) -> tensor<13x21x3xi64>
// CHECK:           return %[[VAL_2]] : tensor<13x21x3xi64>
  %0 =  "tosa.const"() <{value = dense<-42> : tensor<13x21x3xi8>}>: () -> tensor<13x21x3xi8>
  %1 =  "tosa.const"() <{value = dense<-42> : tensor<13x21x3xi64>}>: () -> tensor<13x21x3xi64>
  %2 = tosa.greater_equal %arg0, %0: (tensor<13x21x3xi8>, tensor<13x21x3xi8>) -> tensor<13x21x3xi1>
  %3 = tosa.select %2, %1, %arg1: ( tensor<13x21x3xi1>, tensor<13x21x3xi64>, tensor<13x21x3xi64>) -> tensor<13x21x3xi64>
  return %3  :  tensor<13x21x3xi64>
}

// -----

func.func @concat_reshape_fusion_axis0_to_axis1(
    %arg0: tensor<1x256x100x100xf32>,
    %arg1: tensor<1x256x100x100xf32>,
    %arg2: tensor<1x256x100x100xf32>,
    %arg3: tensor<1x256x100x100xf32>) -> tensor<1x1024x100x100xf32> {
  %0 = tosa.concat %arg0, %arg1, %arg2, %arg3 {axis = 0 : i32} : (tensor<1x256x100x100xf32>, tensor<1x256x100x100xf32>, tensor<1x256x100x100xf32>, tensor<1x256x100x100xf32>) -> tensor<4x256x100x100xf32>
  %1 = tosa.reshape %0 {new_shape = array<i64: 1, 1024, 100, 100>} : (tensor<4x256x100x100xf32>) -> tensor<1x1024x100x100xf32>
  return %1 : tensor<1x1024x100x100xf32>
}
// CHECK-LABEL: @concat_reshape_fusion_axis0_to_axis1
// CHECK-SAME: %[[X0:.*]]: tensor<1x256x100x100xf32>, %[[X1:.*]]: tensor<1x256x100x100xf32>, %[[X2:.*]]: tensor<1x256x100x100xf32>, %[[X3:.*]]: tensor<1x256x100x100xf32>
// CHECK: %[[RES:.*]] = tosa.concat %[[X0]], %[[X1]], %[[X2]], %[[X3]] {axis = 1 : i32} : (tensor<1x256x100x100xf32>, tensor<1x256x100x100xf32>, tensor<1x256x100x100xf32>, tensor<1x256x100x100xf32>) -> tensor<1x1024x100x100xf32>
// CHECK: return %[[RES]] : tensor<1x1024x100x100xf32>

// -----

func.func @concat_reshape_fusion_axis1_to_axis0(
    %arg0: tensor<1x4x2xf32>, %arg1: tensor<1x4x2xf32>) -> tensor<2x4x2xf32> {
  %0 = tosa.concat %arg0, %arg1 {axis = 1 : i32} : (tensor<1x4x2xf32>, tensor<1x4x2xf32>) -> tensor<1x8x2xf32>
  %1 = tosa.reshape %0 {new_shape = array<i64: 2, 4, 2>} : (tensor<1x8x2xf32>) -> tensor<2x4x2xf32>
  return %1 : tensor<2x4x2xf32>
}
// CHECK-LABEL: @concat_reshape_fusion_axis1_to_axis0
// CHECK-SAME: %[[X0:.*]]: tensor<1x4x2xf32>, %[[X1:.*]]: tensor<1x4x2xf32>
// CHECK: %[[RES:.*]] = tosa.concat %[[X0]], %[[X1]] {axis = 0 : i32} : (tensor<1x4x2xf32>, tensor<1x4x2xf32>) -> tensor<2x4x2xf32>
// CHECK: return %[[RES]] : tensor<2x4x2xf32>

// -----

func.func @concat_reshape_fusion_non_adjacent(
    %arg0: tensor<1x1x64xf32>,
    %arg1: tensor<1x1x64xf32>,
    %arg2: tensor<1x1x64xf32>) -> tensor<1x1x192xf32> {
  %0 = tosa.concat %arg0, %arg1, %arg2 {axis = 0 : i32} : (tensor<1x1x64xf32>, tensor<1x1x64xf32>, tensor<1x1x64xf32>) -> tensor<3x1x64xf32>
  %1 = tosa.reshape %0 {new_shape = array<i64: 1, 1, 192>} : (tensor<3x1x64xf32>) -> tensor<1x1x192xf32>
  return %1 : tensor<1x1x192xf32>
}
// CHECK-LABEL: @concat_reshape_fusion_non_adjacent
// CHECK-SAME: %[[X0:.*]]: tensor<1x1x64xf32>, %[[X1:.*]]: tensor<1x1x64xf32>, %[[X2:.*]]: tensor<1x1x64xf32>
// CHECK: %[[RES:.*]] = tosa.concat %[[X0]], %[[X1]], %[[X2]] {axis = 2 : i32} : (tensor<1x1x64xf32>, tensor<1x1x64xf32>, tensor<1x1x64xf32>) -> tensor<1x1x192xf32>
// CHECK: return %[[RES]] : tensor<1x1x192xf32>

// -----

func.func @concat_reshape_fusion_dim_in_range_not_one(
    %arg0: tensor<2x3xf32>, %arg1: tensor<2x3xf32>) -> tensor<2x6xf32> {
  %0 = tosa.concat %arg0, %arg1 {axis = 0 : i32} : (tensor<2x3xf32>, tensor<2x3xf32>) -> tensor<4x3xf32>
  %1 = tosa.reshape %0 {new_shape = array<i64: 2, 6>} : (tensor<4x3xf32>) -> tensor<2x6xf32>
  return %1 : tensor<2x6xf32>
}
// CHECK-LABEL: @concat_reshape_fusion_dim_in_range_not_one
// CHECK: tosa.concat
// CHECK: tosa.reshape

// -----

func.func @concat_reshape_fusion_non_adjacent_intermediate_not_one(
    %arg0: tensor<1x3x4xf32>, %arg1: tensor<1x3x4xf32>) -> tensor<1x3x8xf32> {
  %0 = tosa.concat %arg0, %arg1 {axis = 0 : i32} : (tensor<1x3x4xf32>, tensor<1x3x4xf32>) -> tensor<2x3x4xf32>
  %1 = tosa.reshape %0 {new_shape = array<i64: 1, 3, 8>} : (tensor<2x3x4xf32>) -> tensor<1x3x8xf32>
  return %1 : tensor<1x3x8xf32>
}
// CHECK-LABEL: @concat_reshape_fusion_non_adjacent_intermediate_not_one
// CHECK: tosa.concat
// CHECK: tosa.reshape

// -----

func.func @concat_reshape_fusion_rank_change(
    %arg0: tensor<1x256x100xf32>, %arg1: tensor<1x256x100xf32>) -> tensor<512x100xf32> {
  %0 = tosa.concat %arg0, %arg1 {axis = 0 : i32} : (tensor<1x256x100xf32>, tensor<1x256x100xf32>) -> tensor<2x256x100xf32>
  %1 = tosa.reshape %0 {new_shape = array<i64: 512, 100>} : (tensor<2x256x100xf32>) -> tensor<512x100xf32>
  return %1 : tensor<512x100xf32>
}
// CHECK-LABEL: @concat_reshape_fusion_rank_change
// CHECK: tosa.concat
// CHECK: tosa.reshape

// -----

// Negative test: concat has multiple uses, so the reshape cannot be folded.
func.func @concat_reshape_fusion_multi_use(
    %arg0: tensor<1x256x100xf32>,
    %arg1: tensor<1x256x100xf32>) -> (tensor<1x512x100xf32>, tensor<2x256x100xf32>) {
  %0 = tosa.concat %arg0, %arg1 {axis = 0 : i32} : (tensor<1x256x100xf32>, tensor<1x256x100xf32>) -> tensor<2x256x100xf32>
  %1 = tosa.reshape %0 {new_shape = array<i64: 1, 512, 100>} : (tensor<2x256x100xf32>) -> tensor<1x512x100xf32>
  return %1, %0 : tensor<1x512x100xf32>, tensor<2x256x100xf32>
}
// CHECK-LABEL: @concat_reshape_fusion_multi_use
// CHECK: tosa.concat
// CHECK: tosa.reshape

// -----

// Negative test: reshape changes more than two dimensions.
func.func @concat_reshape_fusion_multi_dim_reshape(
    %arg0: tensor<1x4x2xf32>,
    %arg1: tensor<1x4x2xf32>) -> tensor<2x2x4xf32> {
  %0 = tosa.concat %arg0, %arg1 {axis = 0 : i32} : (tensor<1x4x2xf32>, tensor<1x4x2xf32>) -> tensor<2x4x2xf32>
  %1 = tosa.reshape %0 {new_shape = array<i64: 2, 2, 4>} : (tensor<2x4x2xf32>) -> tensor<2x2x4xf32>
  return %1 : tensor<2x2x4xf32>
}
// CHECK-LABEL: @concat_reshape_fusion_multi_dim_reshape
// CHECK: tosa.concat
// CHECK: tosa.reshape
