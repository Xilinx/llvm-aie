// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold %s | FileCheck %s

// CHECK-LABEL: @pad_bool
func.func @pad_bool() -> (tensor<5x5xi1>) {
  // CHECK: "tosa.const"() <{value = dense<{{\[\[}}false, false, false, false, false],
  // CHECK-SAME: [false, true, true, false, false], [false, true, true, false, false],
  // CHECK-SAME: [false, false, false, false, false], [false, false, false, false, false]]>
  %0 = "tosa.const"() {value = dense<true> : tensor<2x2xi1>} : () -> tensor<2x2xi1>
  %1 = tosa.const_shape {value = dense<[1, 2, 1, 2]> : tensor<4xindex>} : () -> !tosa.shape<4>
  %2 = "tosa.const"() {value = dense<false> : tensor<i1>} : () -> tensor<i1>
  %3 = "tosa.pad"(%0, %1, %2) : (tensor<2x2xi1>, !tosa.shape<4>, tensor<i1>) -> tensor<5x5xi1>
  return %3 : tensor<5x5xi1>
}

// CHECK-LABEL: @pad_int8
func.func @pad_int8() -> (tensor<5x5xi8>) {
  // CHECK: "tosa.const"() <{value = dense<{{\[\[}}1, 1, 1, 1, 1], [1, 3, 4, 1, 1], [1, 5, 6, 1, 1], [1, 1, 1, 1, 1], [1, 1, 1, 1, 1]]>
  %0 = "tosa.const"() {value = dense<[[3, 4], [5, 6]]> : tensor<2x2xi8>} : () -> tensor<2x2xi8>
  %1 = tosa.const_shape {value = dense<[1, 2, 1, 2]> : tensor<4xindex>} : () -> !tosa.shape<4>
  %2 = "tosa.const"() {value = dense<1> : tensor<i8>} : () -> tensor<i8>
  %3 = "tosa.pad"(%0, %1, %2) : (tensor<2x2xi8>, !tosa.shape<4>, tensor<i8>) -> tensor<5x5xi8>
  return %3 : tensor<5x5xi8>
}

// CHECK-LABEL: @pad_int32
func.func @pad_int32() -> (tensor<5x5xi32>) {
  // CHECK: "tosa.const"() <{value = dense<{{\[\[}}1, 1, 1, 1, 1], [1, 3, 4, 1, 1], [1, 5, 6, 1, 1], [1, 1, 1, 1, 1], [1, 1, 1, 1, 1]]>
  %0 = "tosa.const"() {value = dense<[[3, 4], [5, 6]]> : tensor<2x2xi32>} : () -> tensor<2x2xi32>
  %1 = tosa.const_shape {value = dense<[1, 2, 1, 2]> : tensor<4xindex>} : () -> !tosa.shape<4>
  %2 = "tosa.const"() {value = dense<1> : tensor<i32>} : () -> tensor<i32>
  %3 = "tosa.pad"(%0, %1, %2) : (tensor<2x2xi32>, !tosa.shape<4>, tensor<i32>) -> tensor<5x5xi32>
  return %3 : tensor<5x5xi32>
}

// CHECK-LABEL: @pad_int32_default_value
func.func @pad_int32_default_value() -> (tensor<5x5xi32>) {
  // CHECK: "tosa.const"() <{value = dense<{{\[\[}}0, 0, 0, 0, 0], [0, 3, 4, 0, 0], [0, 5, 6, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0]]>
  %0 = "tosa.const"() {value = dense<[[3, 4], [5, 6]]> : tensor<2x2xi32>} : () -> tensor<2x2xi32>
  %1 = tosa.const_shape {value = dense<[1, 2, 1, 2]> : tensor<4xindex>} : () -> !tosa.shape<4>
  %2 = "tosa.pad"(%0, %1) : (tensor<2x2xi32>, !tosa.shape<4>) -> tensor<5x5xi32>
  return %2 : tensor<5x5xi32>
}

// CHECK-LABEL: @pad_bfloat16
func.func @pad_bfloat16() -> (tensor<5x5xbf16>) {
  // CHECK: "tosa.const"()
  // CHECK-SAME: {{\[\[}}-1.000000e+00, -1.000000e+00, -1.000000e+00, -1.000000e+00, -1.000000e+00],
  // CHECK-SAME: [-1.000000e+00, 1.000000e+00, 2.000000e+00, -1.000000e+00, -1.000000e+00],
  // CHECK-SAME: [-1.000000e+00, 3.000000e+00, 4.000000e+00, -1.000000e+00, -1.000000e+00],
  // CHECK-SAME: [-1.000000e+00, -1.000000e+00, -1.000000e+00, -1.000000e+00, -1.000000e+00],
  // CHECK-SAME: [-1.000000e+00, -1.000000e+00, -1.000000e+00, -1.000000e+00, -1.000000e+00]]>
  %0 = "tosa.const"() {value = dense<[[1.0, 2.0], [3.0, 4.0]]> : tensor<2x2xbf16>} : () -> tensor<2x2xbf16>
  %1 = tosa.const_shape {value = dense<[1, 2, 1, 2]> : tensor<4xindex>} : () -> !tosa.shape<4>
  %2 = "tosa.const"() {value = dense<-1.0> : tensor<bf16>} : () -> tensor<bf16>
  %3 = "tosa.pad"(%0, %1, %2) : (tensor<2x2xbf16>, !tosa.shape<4>, tensor<bf16>) -> tensor<5x5xbf16>
  return %3 : tensor<5x5xbf16>
}

// CHECK-LABEL: @pad_bfloat16_default_value
func.func @pad_bfloat16_default_value() -> (tensor<5x5xbf16>) {
  // CHECK: "tosa.const"()
  // CHECK-SAME: {{\[\[}}0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00],
  // CHECK-SAME: [0.000000e+00, 1.000000e+00, 2.000000e+00, 0.000000e+00, 0.000000e+00],
  // CHECK-SAME: [0.000000e+00, 3.000000e+00, 4.000000e+00, 0.000000e+00, 0.000000e+00],
  // CHECK-SAME: [0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00],
  // CHECK-SAME: [0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00]]
  %0 = "tosa.const"() {value = dense<[[1.0, 2.0], [3.0, 4.0]]> : tensor<2x2xbf16>} : () -> tensor<2x2xbf16>
  %1 = tosa.const_shape {value = dense<[1, 2, 1, 2]> : tensor<4xindex>} : () -> !tosa.shape<4>
  %2 = "tosa.pad"(%0, %1) : (tensor<2x2xbf16>, !tosa.shape<4>) -> tensor<5x5xbf16>
  return %2 : tensor<5x5xbf16>
}

// CHECK-LABEL: @pad_f32_3d
func.func @pad_f32_3d() -> (tensor<3x3x4xf32>) {
  // CHECK: "tosa.const"()
  // CHECK-SAME: {{\[\[}}-1.000000e+00, -1.000000e+00, -1.000000e+00, -1.000000e+00],
  // CHECK-SAME: [-1.000000e+00, -1.000000e+00, -1.000000e+00, -1.000000e+00],
  // CHECK-SAME: [-1.000000e+00, -1.000000e+00, -1.000000e+00, -1.000000e+00]],
  // CHECK-SAME: {{\[\[}}-1.000000e+00, 1.000000e+00, 2.000000e+00, -1.000000e+00],
  // CHECK-SAME: [-1.000000e+00, 3.000000e+00, 4.000000e+00, -1.000000e+00],
  // CHECK-SAME: [-1.000000e+00, -1.000000e+00, -1.000000e+00, -1.000000e+00]],
  // CHECK-SAME: {{\[\[}}-1.000000e+00, 5.000000e+00, 6.000000e+00, -1.000000e+00],
  // CHECK-SAME: [-1.000000e+00, 7.000000e+00, 8.000000e+00, -1.000000e+00],
  // CHECK-SAME: [-1.000000e+00, -1.000000e+00, -1.000000e+00, -1.000000e+00]]]>
  %0 = "tosa.const"() {value = dense<[[[1.0, 2.0], [3.0, 4.0]],[[5.0, 6.0], [7.0, 8.0]]]> : tensor<2x2x2xf32>} : () -> tensor<2x2x2xf32>
  %1 = tosa.const_shape {value = dense<[1, 0, 0, 1, 1, 1]> : tensor<6xindex>} : () -> !tosa.shape<6>
  %2 = "tosa.const"() {value = dense<-1.0> : tensor<f32>} : () -> tensor<f32>
  %3 = "tosa.pad"(%0, %1, %2) : (tensor<2x2x2xf32>, !tosa.shape<6>, tensor<f32>) -> tensor<3x3x4xf32>
  return %3 : tensor<3x3x4xf32>
}

// CHECK-LABEL: @pad_f32_3d_default_value
func.func @pad_f32_3d_default_value() -> (tensor<3x3x4xf32>) {
  // CHECK: "tosa.const"()
  // CHECK-SAME: {{\[\[}}0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00],
  // CHECK-SAME: [0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00],
  // CHECK-SAME: [0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00]],
  // CHECK-SAME: {{\[\[}}0.000000e+00, 1.000000e+00, 2.000000e+00, 0.000000e+00],
  // CHECK-SAME: [0.000000e+00, 3.000000e+00, 4.000000e+00, 0.000000e+00],
  // CHECK-SAME: [0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00]],
  // CHECK-SAME: {{\[\[}}0.000000e+00, 5.000000e+00, 6.000000e+00, 0.000000e+00],
  // CHECK-SAME: [0.000000e+00, 7.000000e+00, 8.000000e+00, 0.000000e+00],
  // CHECK-SAME: [0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00]]]>
  %0 = "tosa.const"() {value = dense<[[[1.0, 2.0], [3.0, 4.0]],[[5.0, 6.0], [7.0, 8.0]]]> : tensor<2x2x2xf32>} : () -> tensor<2x2x2xf32>
  %1 = tosa.const_shape {value = dense<[1, 0, 0, 1, 1, 1]> : tensor<6xindex>} : () -> !tosa.shape<6>
  %2 = "tosa.pad"(%0, %1) : (tensor<2x2x2xf32>, !tosa.shape<6>) -> tensor<3x3x4xf32>
  return %2 : tensor<3x3x4xf32>
}

// CHECK-LABEL: @pad_int32_multi_user
func.func @pad_int32_multi_user() -> (tensor<2x2xi32>, tensor<5x5xi32>) {
// CHECK-DAG:       [[VAR_0_:%.+]] = "tosa.const"() <{value = dense<2> : tensor<2x2xi32>}> : () -> tensor<2x2xi32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.const_shape {value = dense<[1, 2, 1, 2]> : tensor<4xindex>} : () -> !tosa.shape<4>
// CHECK-DAG:       [[VAR_2_:%.+]] = "tosa.const"() <{value = dense<1> : tensor<i32>}> : () -> tensor<i32>
// CHECK:           [[VAR_3_:%.+]] = tosa.pad [[VAR_0_]], [[VAR_1_]], [[VAR_2_]] : (tensor<2x2xi32>, !tosa.shape<4>, tensor<i32>) -> tensor<5x5xi32>
  %0 = "tosa.const"() {value = dense<2> : tensor<2x2xi32>} : () -> tensor<2x2xi32>
  %5 = tosa.const_shape {value = dense<[1, 2, 1, 2]> : tensor<4xindex>} : () -> !tosa.shape<4>
  %6 = "tosa.const"() {value = dense<1> : tensor<i32>} : () -> tensor<i32>
  %1 = "tosa.pad"(%0, %5, %6) : (tensor<2x2xi32>, !tosa.shape<4>, tensor<i32>) -> tensor<5x5xi32>
  return %0, %1 : tensor<2x2xi32>, tensor<5x5xi32>
}
