// RUN: mlir-opt --tosa-layerwise-constant-fold %s | FileCheck %s

// CHECK-LABEL:  func.func @concat_i32_axis0
// CHECK-SAME:   () -> tensor<4x2xi32> {
// CHECK:           [[VAR_0_:%.+]] = "tosa.const"() <{value = dense<{{.}}[1, 2], [3, 4], [5, 6], [7, 8]{{.}}> : tensor<4x2xi32>}> : () -> tensor<4x2xi32>
// CHECK:           return [[VAR_0_]] : tensor<4x2xi32>
func.func @concat_i32_axis0() -> (tensor<4x2xi32>) {
  %c0 = "tosa.const"() {value = dense<[[1, 2], [3, 4]]> : tensor<2x2xi32>} : () -> tensor<2x2xi32>
  %c1 = "tosa.const"() {value = dense<[[5, 6], [7, 8]]> : tensor<2x2xi32>} : () -> tensor<2x2xi32>
  %0 = "tosa.concat"(%c0, %c1) {axis = 0 : i32} : (tensor<2x2xi32>, tensor<2x2xi32>) -> tensor<4x2xi32>
  return %0 : tensor<4x2xi32>
}

// CHECK-LABEL:  func.func @concat_f32_axis1
// CHECK-SAME:   () -> tensor<2x3xf32> {
// CHECK:           [[VAR_0_:%.+]] = "tosa.const"() <{value = dense<{{.}}[1.000000e+00, 2.000000e+00, 3.000000e+00], [4.000000e+00, 5.000000e+00, 6.000000e+00]{{.}}> : tensor<2x3xf32>}> : () -> tensor<2x3xf32>
// CHECK:           return [[VAR_0_]] : tensor<2x3xf32>
func.func @concat_f32_axis1() -> (tensor<2x3xf32>) {
  %c0 = "tosa.const"() {value = dense<[[1.0, 2.0], [4.0, 5.0]]> : tensor<2x2xf32>} : () -> tensor<2x2xf32>
  %c1 = "tosa.const"() {value = dense<[[3.0], [6.0]]> : tensor<2x1xf32>} : () -> tensor<2x1xf32>
  %0 = "tosa.concat"(%c0, %c1) {axis = 1 : i32} : (tensor<2x2xf32>, tensor<2x1xf32>) -> tensor<2x3xf32>
  return %0 : tensor<2x3xf32>
}

// CHECK-LABEL:  func.func @concat_i8_three_inputs_axis1
// CHECK-SAME:   () -> tensor<1x5xi8> {
// CHECK:           [[VAR_0_:%.+]] = "tosa.const"() <{value = dense<{{.}}[1, 2, 3, 4, 5]{{.}}> : tensor<1x5xi8>}> : () -> tensor<1x5xi8>
// CHECK:           return [[VAR_0_]] : tensor<1x5xi8>
func.func @concat_i8_three_inputs_axis1() -> (tensor<1x5xi8>) {
  %c0 = "tosa.const"() {value = dense<[[1, 2]]> : tensor<1x2xi8>} : () -> tensor<1x2xi8>
  %c1 = "tosa.const"() {value = dense<[[3]]> : tensor<1x1xi8>} : () -> tensor<1x1xi8>
  %c2 = "tosa.const"() {value = dense<[[4, 5]]> : tensor<1x2xi8>} : () -> tensor<1x2xi8>
  %0 = "tosa.concat"(%c0, %c1, %c2) {axis = 1 : i32} : (tensor<1x2xi8>, tensor<1x1xi8>, tensor<1x2xi8>) -> tensor<1x5xi8>
  return %0 : tensor<1x5xi8>
}

// CHECK-LABEL:  func.func @concat_i32_with_splat_axis0
// CHECK-SAME:   () -> tensor<3x1xi32> {
// CHECK:           [[VAR_0_:%.+]] = "tosa.const"() <{value = dense<{{.}}[7], [7], [8]{{.}}> : tensor<3x1xi32>}> : () -> tensor<3x1xi32>
// CHECK:           return [[VAR_0_]] : tensor<3x1xi32>
func.func @concat_i32_with_splat_axis0() -> (tensor<3x1xi32>) {
  %c0 = "tosa.const"() {value = dense<7> : tensor<2x1xi32>} : () -> tensor<2x1xi32>
  %c1 = "tosa.const"() {value = dense<[[8]]> : tensor<1x1xi32>} : () -> tensor<1x1xi32>
  %0 = "tosa.concat"(%c0, %c1) {axis = 0 : i32} : (tensor<2x1xi32>, tensor<1x1xi32>) -> tensor<3x1xi32>
  return %0 : tensor<3x1xi32>
}

// CHECK-LABEL:  func.func @concat_bool_axis0
// CHECK-SAME:   () -> tensor<2x2xi1> {
// CHECK:           [[VAR_0_:%.+]] = "tosa.const"() <{value = dense<{{.}}[true, false], [false, true]{{.}}> : tensor<2x2xi1>}> : () -> tensor<2x2xi1>
// CHECK:           return [[VAR_0_]] : tensor<2x2xi1>
func.func @concat_bool_axis0() -> (tensor<2x2xi1>) {
  %c0 = "tosa.const"() {value = dense<[[true], [false]]> : tensor<2x1xi1>} : () -> tensor<2x1xi1>
  %c1 = "tosa.const"() {value = dense<[[false], [true]]> : tensor<2x1xi1>} : () -> tensor<2x1xi1>
  %0 = "tosa.concat"(%c0, %c1) {axis = 1 : i32} : (tensor<2x1xi1>, tensor<2x1xi1>) -> tensor<2x2xi1>
  return %0 : tensor<2x2xi1>
}

// CHECK-LABEL:  func.func @concat_rank1_i32_axis0
// CHECK-SAME:   () -> tensor<5xi32> {
// CHECK:           [[VAR_0_:%.+]] = "tosa.const"() <{value = dense<[1, 2, 3, 4, 5]> : tensor<5xi32>}> : () -> tensor<5xi32>
// CHECK:           return [[VAR_0_]] : tensor<5xi32>
func.func @concat_rank1_i32_axis0() -> (tensor<5xi32>) {
  %c0 = "tosa.const"() {value = dense<[1, 2, 3]> : tensor<3xi32>} : () -> tensor<3xi32>
  %c1 = "tosa.const"() {value = dense<[4, 5]> : tensor<2xi32>} : () -> tensor<2xi32>
  %0 = "tosa.concat"(%c0, %c1) {axis = 0 : i32} : (tensor<3xi32>, tensor<2xi32>) -> tensor<5xi32>
  return %0 : tensor<5xi32>
}

// CHECK-LABEL:  func.func @concat_empty_tensor_axis0
// CHECK-SAME:   () -> tensor<2x2xi32> {
// CHECK:           [[VAR_0_:%.+]] = "tosa.const"() <{value = dense<{{.}}[1, 2], [3, 4]{{.}}> : tensor<2x2xi32>}> : () -> tensor<2x2xi32>
// CHECK:           return [[VAR_0_]] : tensor<2x2xi32>
func.func @concat_empty_tensor_axis0() -> (tensor<2x2xi32>) {
  %c0 = "tosa.const"() {value = dense<[[1, 2], [3, 4]]> : tensor<2x2xi32>} : () -> tensor<2x2xi32>
  %c1 = "tosa.const"() {value = dense<> : tensor<0x2xi32>} : () -> tensor<0x2xi32>
  %0 = "tosa.concat"(%c0, %c1) {axis = 0 : i32} : (tensor<2x2xi32>, tensor<0x2xi32>) -> tensor<2x2xi32>
  return %0 : tensor<2x2xi32>
}

// CHECK-LABEL:  func.func @concat_all_empty_tensors_axis1
// CHECK-SAME:   () -> tensor<2x0xi32> {
// CHECK:           [[VAR_0_:%.+]] = "tosa.const"() <{value = dense<> : tensor<2x0xi32>}> : () -> tensor<2x0xi32>
// CHECK:           return [[VAR_0_]] : tensor<2x0xi32>
func.func @concat_all_empty_tensors_axis1() -> (tensor<2x0xi32>) {
  %c0 = "tosa.const"() {value = dense<> : tensor<2x0xi32>} : () -> tensor<2x0xi32>
  %c1 = "tosa.const"() {value = dense<> : tensor<2x0xi32>} : () -> tensor<2x0xi32>
  %0 = "tosa.concat"(%c0, %c1) {axis = 1 : i32} : (tensor<2x0xi32>, tensor<2x0xi32>) -> tensor<2x0xi32>
  return %0 : tensor<2x0xi32>
}

// CHECK-LABEL:  func.func @concat_i32_axis1_three_inputs_two_splats
// CHECK-SAME:   () -> tensor<2x4xi32> {
// CHECK:           [[VAR_0_:%.+]] = "tosa.const"() <{value = dense<{{.}}[1, 10, 11, 2], [1, 12, 13, 2]{{.}}> : tensor<2x4xi32>}> : () -> tensor<2x4xi32>
// CHECK:           return [[VAR_0_]] : tensor<2x4xi32>
func.func @concat_i32_axis1_three_inputs_two_splats() -> (tensor<2x4xi32>) {
  %c0_splat = "tosa.const"() {value = dense<1> : tensor<2x1xi32>} : () -> tensor<2x1xi32>
  %c1_dense = "tosa.const"() {value = dense<[[10, 11], [12, 13]]> : tensor<2x2xi32>} : () -> tensor<2x2xi32>
  %c2_splat = "tosa.const"() {value = dense<2> : tensor<2x1xi32>} : () -> tensor<2x1xi32>
  %0 = "tosa.concat"(%c0_splat, %c1_dense, %c2_splat) {axis = 1 : i32} : (tensor<2x1xi32>, tensor<2x2xi32>, tensor<2x1xi32>) -> tensor<2x4xi32>
  return %0 : tensor<2x4xi32>
}

// CHECK-LABEL:  func.func @concat_ui16_axis0
// CHECK-SAME:   () -> tensor<3x2xui16> {
// CHECK:           [[VAR_0_:%.+]] = "tosa.const"() <{value = dense<{{.}}[100, 200], [300, 400], [500, 600]{{.}}> : tensor<3x2xui16>}> : () -> tensor<3x2xui16>
// CHECK:           return [[VAR_0_]] : tensor<3x2xui16>
func.func @concat_ui16_axis0() -> (tensor<3x2xui16>) {
  %c0 = "tosa.const"() {value = dense<[[100, 200], [300, 400]]> : tensor<2x2xui16>} : () -> tensor<2x2xui16>
  %c1 = "tosa.const"() {value = dense<[[500, 600]]> : tensor<1x2xui16>} : () -> tensor<1x2xui16>
  %0 = "tosa.concat"(%c0, %c1) {axis = 0 : i32} : (tensor<2x2xui16>, tensor<1x2xui16>) -> tensor<3x2xui16>
  return %0 : tensor<3x2xui16>
}

// CHECK-LABEL:  func.func @concat_3d_bf16_axis1
// CHECK-SAME:   () -> tensor<2x3x2xbf16> {
// CHECK:           [[VAR_0_:%.+]] = "tosa.const"() <{value = dense<{{.}}{{.}}[1.000000e+00, 2.000000e+00], [5.000000e+00, 6.000000e+00], [7.000000e+00, 8.000000e+00]{{.}}, {{.}}[3.000000e+00, 4.000000e+00], [9.000000e+00, 1.000000e+01], [1.100000e+01, 1.200000e+01]{{.}}{{.}}> : tensor<2x3x2xbf16>}> : () -> tensor<2x3x2xbf16>
// CHECK:           return [[VAR_0_]] : tensor<2x3x2xbf16>
func.func @concat_3d_bf16_axis1() -> (tensor<2x3x2xbf16>) {
  %c0 = "tosa.const"() {value = dense<[[[1.0, 2.0]], [[3.0, 4.0]]]> : tensor<2x1x2xbf16>} : () -> tensor<2x1x2xbf16>
  %c1 = "tosa.const"() {value = dense<[[[5.0, 6.0], [7.0, 8.0]], [[9.0, 10.0], [11.0, 12.0]]]> : tensor<2x2x2xbf16>} : () -> tensor<2x2x2xbf16>
  %0 = "tosa.concat"(%c0, %c1) {axis = 1 : i32} : (tensor<2x1x2xbf16>, tensor<2x2x2xbf16>) -> tensor<2x3x2xbf16>
  return %0 : tensor<2x3x2xbf16>
}
