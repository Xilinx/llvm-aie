// RUN: mlir-opt --split-input-file --tosa-optional-decompositions %s | FileCheck %s

// -----

// CHECK-LABEL: @depthwise_conv2d_as_mul
func.func @depthwise_conv2d_as_mul(%arg0: tensor<4x10x10x2xf32>, %arg1: tensor<1x1x2x3xf32>, %arg2: tensor<6xf32>) -> tensor<4x10x10x6xf32> {
  // CHECK-NOT: tosa.depthwise_conv2d
  // CHECK: %[[VAR0:.*]] = tosa.reshape %arg0 {new_shape = array<i64: 4, 10, 10, 2, 1>}
  // CHECK-SAME: -> tensor<4x10x10x2x1xf32>
  // CHECK: %[[VAR1:.*]] = tosa.reshape %arg1 {new_shape = array<i64: 1, 1, 1, 2, 3>}
  // CHECK-SAME: -> tensor<1x1x1x2x3xf32>
  // CHECK: %[[VAR2:.*]] = tosa.mul %[[VAR0]], %[[VAR1]]
  // CHECK-SAME: -> tensor<4x10x10x2x3xf32>
  // CHECK: %[[VAR3:.*]] = tosa.reshape %[[VAR2]] {new_shape = array<i64: 4, 10, 10, 6>}
  // CHECK-SAME: -> tensor<4x10x10x6xf32>
  // CHECK: %[[VAR4:.*]] = tosa.reshape %arg2 {new_shape = array<i64: 1, 1, 1, 6>}
  // CHECK-SAME: -> tensor<1x1x1x6xf32>
  // CHECK: %[[VAR5:.*]] = tosa.add %[[VAR3]], %[[VAR4]]
  // CHECK-SAME: -> tensor<4x10x10x6xf32>
  // CHECK: return %[[VAR5]]
  %0 = tosa.depthwise_conv2d %arg0, %arg1, %arg2 {acc_type = f32, pad = array<i64: 0, 0, 0, 0>, stride = array<i64: 1, 1>, dilation = array<i64: 1, 1>} : (tensor<4x10x10x2xf32>, tensor<1x1x2x3xf32>, tensor<6xf32>) -> tensor<4x10x10x6xf32>
  return %0 : tensor<4x10x10x6xf32>
}

// -----

// CHECK-LABEL: @depthwise_conv2d_as_mul_q
func.func @depthwise_conv2d_as_mul_q(%arg0: tensor<4x10x10x2xi8>, %arg1: tensor<1x1x2x3xi8>, %arg2: tensor<6xi32>) -> tensor<4x10x10x6xi32> {
  // CHECK-DAG: %[[INPUT_ZP:.+]] = "tosa.const"() <{value = dense<7> : tensor<1x1x1x1x1xi32>}
  // CHECK-DAG: %[[WEIGHT_ZP:.+]] = "tosa.const"() <{value = dense<11> : tensor<1x1x1x1xi32>}
  // CHECK: %[[RESHAPE_I:.+]] = tosa.reshape %arg0 {new_shape = array<i64: 4, 10, 10, 2, 1>}
  // CHECK: %[[CAST_I:.+]] = tosa.cast %[[RESHAPE_I]] : (tensor<4x10x10x2x1xi8>) -> tensor<4x10x10x2x1xi32>
  // CHECK: %[[CAST_W:.+]] = tosa.cast %arg1 : (tensor<1x1x2x3xi8>) -> tensor<1x1x2x3xi32>
  // CHECK: %[[SUB_I:.+]] = tosa.sub %[[CAST_I]], %[[INPUT_ZP]]
  // CHECK: %[[SUB_W:.+]] = tosa.sub %[[CAST_W]], %[[WEIGHT_ZP]]
  // CHECK: %[[RESHAPE_W:.+]] = tosa.reshape %[[SUB_W]] {new_shape = array<i64: 1, 1, 1, 2, 3>}
  // CHECK: %[[MUL:.+]] = tosa.mul %[[SUB_I]], %[[RESHAPE_W]]
  // CHECK: %[[RESHAPE_O:.+]] = tosa.reshape %[[MUL]] {new_shape = array<i64: 4, 10, 10, 6>}
  // CHECK: %[[RESHAPE_ARG2:.+]] = tosa.reshape %arg2 {new_shape = array<i64: 1, 1, 1, 6>}
  // CHECK: %[[ADD:.+]] = tosa.add %[[RESHAPE_O]], %[[RESHAPE_ARG2]]
  %0 = tosa.depthwise_conv2d %arg0, %arg1, %arg2 {acc_type = i32, pad = array<i64: 0, 0, 0, 0>, stride = array<i64: 1, 1>, dilation = array<i64: 1, 1>, quantization_info = #tosa.conv_quant<input_zp = 7, weight_zp = 11>} : (tensor<4x10x10x2xi8>, tensor<1x1x2x3xi8>, tensor<6xi32>) -> tensor<4x10x10x6xi32>
  return %0 : tensor<4x10x10x6xi32>
}

// -----

// CHECK-LABEL: @depthwise_conv2d_as_mul_padded
func.func @depthwise_conv2d_as_mul_padded(%arg0: tensor<4x10x10x2xf32>, %arg1: tensor<1x1x2x3xf32>, %arg2: tensor<6xf32>) -> tensor<4x12x12x6xf32> {
  // CHECK-DAG: %[[PAD:.+]] = tosa.const_shape {value = dense<[0, 0, 1, 1, 1, 1, 0, 0, 0, 0]> : tensor<10xindex>} : () -> !tosa.shape<10>
  // CHECK-DAG: %[[ZERO:.+]] = "tosa.const"() <{value = dense<0.000000e+00> : tensor<f32>}
  // CHECK: %[[RESHAPE_I:.+]] = tosa.reshape %arg0 {new_shape = array<i64: 4, 10, 10, 2, 1>}
  // CHECK: %[[PAD_I:.+]] = tosa.pad %[[RESHAPE_I]], %[[PAD]], %[[ZERO]] : (tensor<4x10x10x2x1xf32>, !tosa.shape<10>, tensor<f32>) -> tensor<4x12x12x2x1xf32>
  // CHECK: %[[RESHAPE_ARG1:.+]] = tosa.reshape %arg1 {new_shape = array<i64: 1, 1, 1, 2, 3>}
  // CHECK: %[[MUL:.+]] = tosa.mul %[[PAD_I]], %[[RESHAPE_ARG1]]
  // CHECK: %[[RESHAPE_O:.+]] = tosa.reshape %[[MUL]] {new_shape = array<i64: 4, 12, 12, 6>}
  // CHECK: %[[RESHAPE_ARG2:.+]] = tosa.reshape %arg2 {new_shape = array<i64: 1, 1, 1, 6>}
  // CHECK: %[[ADD:.+]] = tosa.add %[[RESHAPE_O]], %[[RESHAPE_ARG2]]
  %0 = tosa.depthwise_conv2d %arg0, %arg1, %arg2 {acc_type = f32, pad = array<i64: 1, 1, 1, 1>, stride = array<i64: 1, 1>, dilation = array<i64: 1, 1>} : (tensor<4x10x10x2xf32>, tensor<1x1x2x3xf32>, tensor<6xf32>) -> tensor<4x12x12x6xf32>
  return %0 : tensor<4x12x12x6xf32>
}
