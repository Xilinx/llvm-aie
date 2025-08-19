// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold=enable-tile-folding=true %s | FileCheck %s
// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold %s | FileCheck --check-prefix=NO-FOLDING-CHECK %s

// CHECK-LABEL: @tile_int_one_dim
func.func @tile_int_one_dim() -> (tensor<6xi32>) {
  // CHECK: "tosa.const"() <{value = dense
  // CHECK-SAME: {{\[}}1, 2, 3, 1, 2, 3]
  %0 = "tosa.const"() {value = dense<[1, 2, 3]> : tensor<3xi32>} : () -> tensor<3xi32>
  %cst = tosa.const_shape { value = dense<[2]> : tensor<1xindex> } : () -> !tosa.shape<1>
  %1 = tosa.tile %0, %cst : (tensor<3xi32>, !tosa.shape<1>) -> tensor<6xi32>
  return %1 : tensor<6xi32>
  // NO-FOLDING-CHECK: tosa.tile
}

// CHECK-LABEL: @tile_bool
func.func @tile_bool() -> (tensor<1x3x2x3xi1>) {
  // CHECK: "tosa.const"() <{value = dense
  // CHECK-SAME: {{\[\[}}{{\[\[}}true, true, true],
  // CHECK-SAME: [true, true, true]],
  // CHECK-SAME: [false, false, false],
  // CHECK-SAME: [false, false, false]],
  // CHECK-SAME: [true, true, true],
  // CHECK-SAME: [true, true, true]]]]
  %0 = "tosa.const"() {value = dense<[[[[true]], [[false]], [[true]]]]> : tensor<1x3x1x1xi1>} : () -> tensor<1x3x1x1xi1>
  %cst = tosa.const_shape { value = dense<[1, 1, 2, 3]> : tensor<4xindex> } : () -> !tosa.shape<4>
  %1 = tosa.tile %0, %cst : (tensor<1x3x1x1xi1>, !tosa.shape<4>) -> tensor<1x3x2x3xi1>
  return %1 : tensor<1x3x2x3xi1>
  // NO-FOLDING-CHECK: tosa.tile
}

// CHECK-LABEL: @tile_bf16
func.func @tile_bf16() -> (tensor<1x3x2x2xbf16>) {
  // CHECK: "tosa.const"() <{value = dense
  // CHECK-SAME: {{\[\[}}{{\[\[}}2.500000e-01, 1.250000e-01],
  // CHECK-SAME:  [2.500000e-01, 1.250000e-01]],
  // CHECK-SAME:  [5.000000e-01, 1.000000e+00],
  // CHECK-SAME:  [5.000000e-01, 1.000000e+00]],
  // CHECK-SAME:  [2.000000e+00, 4.000000e+00],
  // CHECK-SAME:  [2.000000e+00, 4.000000e+00]]]]
  %0 = "tosa.const"() {value = dense<[[[[0.25, 0.125]], [[0.5, 1.0]], [[2.0, 4.0]]]]> : tensor<1x3x1x2xbf16>} : () -> tensor<1x3x1x2xbf16>
  %cst = tosa.const_shape { value = dense<[1, 1, 2, 1]> : tensor<4xindex> } : () -> !tosa.shape<4>
  %1 = tosa.tile %0, %cst : (tensor<1x3x1x2xbf16>, !tosa.shape<4>) -> tensor<1x3x2x2xbf16>
  return %1 : tensor<1x3x2x2xbf16>
  // NO-FOLDING-CHECK: tosa.tile
}

// CHECK-LABEL: @tile_f32
func.func @tile_f32() -> (tensor<4x4xf32>) {
  // CHECK: "tosa.const"() <{value = dense
  // CHECK-SAME: {{\[\[}}2.500000e-01, 1.250000e+00, 2.500000e-01, 1.250000e+00],
  // CHECK-SAME: [2.250000e+00, 3.250000e+00, 2.250000e+00, 3.250000e+00],
  // CHECK-SAME: [2.500000e-01, 1.250000e+00, 2.500000e-01, 1.250000e+00],
  // CHECK-SAME: [2.250000e+00, 3.250000e+00, 2.250000e+00, 3.250000e+00]]
  %0 = "tosa.const"() {value = dense<[[0.25, 1.25],[2.25, 3.25]]> : tensor<2x2xf32>} : () -> tensor<2x2xf32>
  %cst = tosa.const_shape { value = dense<[2, 2]> : tensor<2xindex> } : () -> !tosa.shape<2>
  %1 = tosa.tile %0, %cst: (tensor<2x2xf32>, !tosa.shape<2>) -> tensor<4x4xf32>
  return %1 : tensor<4x4xf32>
  // NO-FOLDING-CHECK: tosa.tile
}

// CHECK-LABEL: @tile_int_many_dimensions
func.func @tile_int_many_dimensions() -> (tensor<4x6x4xi32>) {
  // CHECK: "tosa.const"() <{value = dense
  // CHECK-SAME: {{\[\[\[}}1, 2, 1, 2],
  // CHECK-SAME:  [3, 4, 3, 4],
  // CHECK-SAME:  [5, 6, 5, 6],
  // CHECK-SAME:  [1, 2, 1, 2],
  // CHECK-SAME:  [3, 4, 3, 4],
  // CHECK-SAME:  [5, 6, 5, 6]],
  // CHECK-SAME: {{\[\[}}7, 8, 7, 8],
  // CHECK-SAME:  [9, 10, 9, 10],
  // CHECK-SAME:  [11, 12, 11, 12],
  // CHECK-SAME:  [7, 8, 7, 8],
  // CHECK-SAME:  [9, 10, 9, 10],
  // CHECK-SAME:  [11, 12, 11, 12]],
  // CHECK-SAME: {{\[\[}}1, 2, 1, 2],
  // CHECK-SAME:  [3, 4, 3, 4],
  // CHECK-SAME:  [5, 6, 5, 6],
  // CHECK-SAME:  [1, 2, 1, 2],
  // CHECK-SAME:  [3, 4, 3, 4],
  // CHECK-SAME:  [5, 6, 5, 6]],
  // CHECK-SAME: {{\[\[}}7, 8, 7, 8],
  // CHECK-SAME:  [9, 10, 9, 10],
  // CHECK-SAME:  [11, 12, 11, 12],
  // CHECK-SAME:  [7, 8, 7, 8],
  // CHECK-SAME:  [9, 10, 9, 10],
  // CHECK-SAME:  [11, 12, 11, 12]]]
  %0 = "tosa.const"() {value = dense<[[[1, 2],[3, 4],[5, 6]], [[7, 8],[9, 10],[11, 12]]]> : tensor<2x3x2xi32>} : () -> tensor<2x3x2xi32>
  %cst = tosa.const_shape { value = dense<[2, 2, 2]> : tensor<3xindex> } : () -> !tosa.shape<3>
  %1 = tosa.tile %0, %cst : (tensor<2x3x2xi32>, !tosa.shape<3>) -> tensor<4x6x4xi32>
  // NO-FOLDING-CHECK: tosa.tile
  return %1 : tensor<4x6x4xi32>
}

// CHECK-LABEL: @tile_f16_many_dimensions
func.func @tile_f16_many_dimensions() -> (tensor<6x2x2xf16>) {
  // CHECK: "tosa.const"() <{value = dense
  // CHECK-SAME: {{\[\[\[}}1.000000e+00, 1.000000e+00],
  // CHECK-SAME:  [1.000000e+00, 1.000000e+00]],
  // CHECK-SAME: {{\[\[}}2.000000e+00, 2.000000e+00],
  // CHECK-SAME:  [2.000000e+00, 2.000000e+00]],
  // CHECK-SAME: {{\[\[}}3.000000e+00, 3.000000e+00],
  // CHECK-SAME:  [3.000000e+00, 3.000000e+00]],
  // CHECK-SAME: {{\[\[}}1.000000e+00, 1.000000e+00],
  // CHECK-SAME:  [1.000000e+00, 1.000000e+00]],
  // CHECK-SAME: {{\[\[}}2.000000e+00, 2.000000e+00],
  // CHECK-SAME:  [2.000000e+00, 2.000000e+00]],
  // CHECK-SAME: {{\[\[}}3.000000e+00, 3.000000e+00],
  // CHECK-SAME:  [3.000000e+00, 3.000000e+00]]]
  %0 = "tosa.const"() {value = dense<[[[1.0]], [[2.0]], [[3.0]]]> : tensor<3x1x1xf16>} : () -> tensor<3x1x1xf16>
  %cst = tosa.const_shape { value = dense<[3, 2, 1]> : tensor<3xindex> } : () -> !tosa.shape<3>
  %1 = tosa.tile %0, %cst : (tensor<3x1x1xf16>, !tosa.shape<3>) -> tensor<6x2x2xf16>
  // NO-FOLDING-CHECK: tosa.tile
  return %1 : tensor<6x2x2xf16>
}

// CHECK-LABEL: @tile_i1_splat
func.func @tile_i1_splat() -> (tensor<1x2x2x2xi1>) {
  // CHECK: "tosa.const"() <{value = dense<false> : tensor<1x2x2x2xi1>}>
  %0 = "tosa.const"() <{value = dense<false> : tensor<1x1x1x1xi1>}> : () -> tensor<1x1x1x1xi1>
  %cst = tosa.const_shape { value = dense<[1, 2, 2, 2]> : tensor<4xindex> } : () -> !tosa.shape<4>
  %1 = tosa.tile %0, %cst : (tensor<1x1x1x1xi1>, !tosa.shape<4>) -> tensor<1x2x2x2xi1>
  // NO-FOLDING-CHECK: tosa.tile
  return %1 : tensor<1x2x2x2xi1>
}

// CHECK-LABEL: @tile_i32_splat
func.func @tile_i32_splat() -> (tensor<1x2x2x2xi32>) {
  // CHECK: "tosa.const"() <{value = dense<2> : tensor<1x2x2x2xi32>}>
  %0 = "tosa.const"() <{value = dense<2> : tensor<1x1x1x1xi32>}> : () -> tensor<1x1x1x1xi32>
  %cst = tosa.const_shape { value = dense<[1, 2, 2, 2]> : tensor<4xindex> } : () -> !tosa.shape<4>
  %1 = tosa.tile %0, %cst : (tensor<1x1x1x1xi32>, !tosa.shape<4>) -> tensor<1x2x2x2xi32>
  // NO-FOLDING-CHECK: tosa.tile
  return %1 : tensor<1x2x2x2xi32>
}

// CHECK-LABEL: @tile_f16_splat
func.func @tile_f16_splat() -> (tensor<1x2x2x2xf16>) {
  // CHECK: "tosa.const"() <{value = dense<1.000000e+00> : tensor<1x2x2x2xf16>}>
  %0 = "tosa.const"() <{value = dense<1.000000e+00> : tensor<1x1x1x1xf16>}> : () -> tensor<1x1x1x1xf16>
  %cst = tosa.const_shape { value = dense<[1, 2, 2, 2]> : tensor<4xindex> } : () -> !tosa.shape<4>
  %1 = tosa.tile %0, %cst : (tensor<1x1x1x1xf16>, !tosa.shape<4>) -> tensor<1x2x2x2xf16>
  // NO-FOLDING-CHECK: tosa.tile
  return %1 : tensor<1x2x2x2xf16>
}

// CHECK-LABEL: @tile_bf16_splat
func.func @tile_bf16_splat() -> (tensor<1x2x2x2xbf16>) {
  // CHECK: "tosa.const"() <{value = dense<1.000000e+00> : tensor<1x2x2x2xbf16>}>
  %0 = "tosa.const"() <{value = dense<1.000000e+00> : tensor<1x1x1x1xbf16>}> : () -> tensor<1x1x1x1xbf16>
  %cst = tosa.const_shape { value = dense<[1, 2, 2, 2]> : tensor<4xindex> } : () -> !tosa.shape<4>
  %1 = tosa.tile %0, %cst : (tensor<1x1x1x1xbf16>, !tosa.shape<4>) -> tensor<1x2x2x2xbf16>
  // NO-FOLDING-CHECK: tosa.tile
  return %1 : tensor<1x2x2x2xbf16>
}