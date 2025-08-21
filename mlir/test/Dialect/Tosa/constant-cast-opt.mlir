// RUN: mlir-opt --split-input-file -verify-diagnostics --tosa-layerwise-constant-fold %s | FileCheck %s

// Casts from float to int

// CHECK-LABEL: @cast_fold_f32_to_i1_all_none_zero
func.func @cast_fold_f32_to_i1_all_none_zero() -> tensor<3xi1> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}true{{.*}}tensor<3xi1>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<[12.0, 4.0, 5.0]> : tensor<3xf32>} : () -> tensor<3xf32>
  %1 = "tosa.cast"(%0) : (tensor<3xf32>) -> tensor<3xi1>
  return %1 : tensor<3xi1>
}

// CHECK-LABEL: @cast_fold_f32_to_i1
func.func @cast_fold_f32_to_i1() -> tensor<3xi1> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}true, false, true{{.*}}tensor<3xi1>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<[12.0, 0.0, 5.0]> : tensor<3xf32>} : () -> tensor<3xf32>
  %1 = "tosa.cast"(%0) : (tensor<3xf32>) -> tensor<3xi1>
  return %1 : tensor<3xi1>
}

// CHECK-LABEL: @cast_fold_f32_to_i32
func.func @cast_fold_f32_to_i32() -> tensor<3xi32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}12, 4, 5{{.*}}tensor<3xi32>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<[12.0, 4.0, 5.0]> : tensor<3xf32>} : () -> tensor<3xf32>
  %1 = "tosa.cast"(%0) : (tensor<3xf32>) -> tensor<3xi32>
  return %1 : tensor<3xi32>
}

// CHECK-LABEL: @cast_fold_f32_to_i16
func.func @cast_fold_f32_to_i16() -> tensor<5xi16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}12, 0, 5, 32767, -32768{{.*}}tensor<5xi16>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[12.0, 0.0, 5.0, 32770.11, -32770.11]> :
                        tensor<5xf32>
                      } : () -> tensor<5xf32>
  %1 = "tosa.cast"(%0) : (tensor<5xf32>) -> tensor<5xi16>
  return %1 : tensor<5xi16>
}

// CHECK-LABEL: @cast_fold_f16_to_i32
func.func @cast_fold_f16_to_i32() -> tensor<6xi32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}12, 14, 0, 5, 277, -278{{.*}}tensor<6xi32>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[12.5, 13.5, 0.0, 5.0, 277.11, -277.71]> :
                        tensor<6xf16>
                      } : () -> tensor<6xf16>
  %1 = "tosa.cast"(%0) : (tensor<6xf16>) -> tensor<6xi32>
  return %1 : tensor<6xi32>
}

// CHECK-LABEL: @cast_fold_f32_to_i8
func.func @cast_fold_f32_to_i8() -> tensor<5xi8> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}12, 0, 5, 127, -128{{.*}}tensor<5xi8>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[12.0, 0.0, 5.0, 32770.11, -32770.11]> :
                        tensor<5xf32>
                      } : () -> tensor<5xf32>
  %1 = "tosa.cast"(%0) : (tensor<5xf32>) -> tensor<5xi8>
  return %1 : tensor<5xi8>
}

// CHECK-LABEL: @cast_fold_f32_to_ui8
// COM: Do not fold casts from floats to uint
func.func @cast_fold_f32_to_ui8() -> tensor<5xui8> {
  // CHECK: tosa.const
  // CHECK-NOT: tensor<5xui8>
  // CHECK: tosa.cast
  %0 = "tosa.const"() {value =
                        dense<[12.0, 0.0, 5.0, 32770.11, -32770.11]> :
                        tensor<5xf32>
                      } : () -> tensor<5xf32>
  %1 = "tosa.cast"(%0) : (tensor<5xf32>) -> tensor<5xui8>
  return %1 : tensor<5xui8>
}

// CHECK-LABEL: @cast_fold_float_to_int_infinity_zero_nan
func.func @cast_fold_float_to_int_infinity_zero_nan() -> tensor<5xi16> {
  // Check if infinity and zero are translated properly. Don't expect any
  // specific value for NaN, as the casted int value for NaN is unspecified.
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}32767, -32768, 0, 0, {{.*}}tensor<5xi16>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[0x7F800000, 0xFF800000, 0.0, -0.0, 0x7FC00000]> :
                        tensor<5xf32>
                      } : () -> tensor<5xf32>
  // expected-warning@below {{Float tensor is casted to integer and it contains NaN values.}}
  %1 = "tosa.cast"(%0) : (tensor<5xf32>) -> tensor<5xi16>
  return %1 : tensor<5xi16>
}

// -----
// Casts from int to int

// CHECK-LABEL: @cast_fold_i16_to_i32
func.func @cast_fold_i16_to_i32() -> tensor<3xi32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}12, 0, -5{{.*}}tensor<3xi32>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[12, 0, -5]> :
                        tensor<3xi16>
                      } : () -> tensor<3xi16>
  %1 = "tosa.cast"(%0) : (tensor<3xi16>) -> tensor<3xi32>
  return %1 : tensor<3xi32>
}

// CHECK-LABEL: @cast_fold_i32_to_i8
func.func @cast_fold_i32_to_i8() -> tensor<5xi8> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}12, 0, -5, -1, 1{{.*}}tensor<5xi8>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[12, 0, -5, 511, -511]> :
                        tensor<5xi32>
                      } : () -> tensor<5xi32>
  %1 = "tosa.cast"(%0) : (tensor<5xi32>) -> tensor<5xi8>
  return %1 : tensor<5xi8>
}

// CHECK-LABEL: @cast_fold_i8_to_ui8
func.func @cast_fold_i8_to_ui8() -> tensor<3xui8> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}4, 0, 251{{.*}}tensor<3xui8>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[4, 0, -5]> :
                        tensor<3xi8>
                      } : () -> tensor<3xi8>
  %1 = "tosa.cast"(%0) : (tensor<3xi8>) -> tensor<3xui8>
  return %1 : tensor<3xui8>
}

// CHECK-LABEL: @cast_fold_ui8_to_i8
func.func @cast_fold_ui8_to_i8() -> tensor<3xi8> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}4, 0, -6{{.*}}tensor<3xi8>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[4, 0, 250]> :
                        tensor<3xui8>
                      } : () -> tensor<3xui8>
  %1 = "tosa.cast"(%0) : (tensor<3xui8>) -> tensor<3xi8>
  return %1 : tensor<3xi8>
}

// CHECK-LABEL: @cast_fold_ui8_to_i16
func.func @cast_fold_ui8_to_i16() -> tensor<3xi16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}4, 0, 250{{.*}}tensor<3xi16>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[4, 0, 250]> :
                        tensor<3xui8>
                      } : () -> tensor<3xui8>
  %1 = "tosa.cast"(%0) : (tensor<3xui8>) -> tensor<3xi16>
  return %1 : tensor<3xi16>
}

// CHECK-LABEL: @cast_fold_ui8_to_i1
func.func @cast_fold_ui8_to_i1() -> tensor<3xi1> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}true, false, true{{.*}}tensor<3xi1>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[4, 0, 250]> :
                        tensor<3xui8>
                      } : () -> tensor<3xui8>
  %1 = "tosa.cast"(%0) : (tensor<3xui8>) -> tensor<3xi1>
  return %1 : tensor<3xi1>
}

// CHECK-LABEL: @cast_fold_ui8_to_ui1
func.func @cast_fold_ui8_to_ui1() -> tensor<3xui1> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}true, false, true{{.*}}tensor<3xui1>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[4, 0, 250]> :
                        tensor<3xui8>
                      } : () -> tensor<3xui8>
  %1 = "tosa.cast"(%0) : (tensor<3xui8>) -> tensor<3xui1>
  return %1 : tensor<3xui1>
}


// CHECK-LABEL: @cast_fold_i16_to_i1
func.func @cast_fold_i16_to_i1() -> tensor<3xi1> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}true, false, true{{.*}}tensor<3xi1>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[12, 0, -5]> :
                        tensor<3xi16>
                      } : () -> tensor<3xi16>
  %1 = "tosa.cast"(%0) : (tensor<3xi16>) -> tensor<3xi1>
  return %1 : tensor<3xi1>
}

// -----
// Casts from int to float

// CHECK-LABEL: @cast_fold_i16_to_f32
func.func @cast_fold_i16_to_f32() -> tensor<3xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}1.2{{0*}}e+01, 0.{{0*}}e+00, -5.{{0*}}e+00
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[12, 0, -5]> :
                        tensor<3xi16>
                      } : () -> tensor<3xi16>
  %1 = "tosa.cast"(%0) : (tensor<3xi16>) -> tensor<3xf32>
  return %1 : tensor<3xf32>
}

// CHECK-LABEL: @cast_fold_i16_to_f16
func.func @cast_fold_i16_to_f16() -> tensor<3xf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}1.2{{0*}}e+01, 0.{{0*}}e+00, -5.{{0*}}e+00
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[12, 0, -5]> :
                        tensor<3xi16>
                      } : () -> tensor<3xi16>
  %1 = "tosa.cast"(%0) : (tensor<3xi16>) -> tensor<3xf16>
  return %1 : tensor<3xf16>
}

// CHECK-LABEL: @cast_fold_i32_to_f16
func.func @cast_fold_i32_to_f16() -> tensor<4xf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}1.2{{0*}}e+01, 0.{{0*}}e+00, -5.{{0*}}e+00, 0x7C00
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[12, 0, -5, 2147483647]> :
                        tensor<4xi32>
                      } : () -> tensor<4xi32>
  %1 = "tosa.cast"(%0) : (tensor<4xi32>) -> tensor<4xf16>
  return %1 : tensor<4xf16>
}

// CHECK-LABEL: @cast_fold_ui8_to_f32
func.func @cast_fold_ui8_to_f32() -> tensor<4xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0.000000e+00, 1.000000e+00, 4.000000e+00, 2.550000e+02{{.*}}tensor<4xf32>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[0, 1, 4, 255]> :
                        tensor<4xui8>
                      } : () -> tensor<4xui8>
  %1 = "tosa.cast"(%0) : (tensor<4xui8>) -> tensor<4xf32>
  return %1 : tensor<4xf32>
}

// -----
// Casts from float to float

// CHECK-LABEL: @cast_fold_f32_to_f16
func.func @cast_fold_f32_to_f16() -> tensor<5xf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}1.2{{0*}}e+01, 0.{{0*}}e+00, 5.{{.*}}, 3.2{{.*}}+04, -3.2{{.*}}e+04{{.*}}tensor<5xf16>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[12.0, 0.0, 5.2, 32770.11, -32770.11]> :
                        tensor<5xf32>
                      } : () -> tensor<5xf32>
  %1 = "tosa.cast"(%0) : (tensor<5xf32>) -> tensor<5xf16>
  return %1 : tensor<5xf16>
}

// CHECK-LABEL: @cast_fold_f32_to_f16_imprecise
func.func @cast_fold_f32_to_f16_imprecise() -> tensor<5xf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}9.56{{.*}}e-02, 0x7C00, 0xFC00, 0.{{0*}}e+00, -0.{{0*}}e+00{{.*}}tensor<5xf16>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[0.0956256023875237592352073,
                               346534769.23495863245, -346534769.23495863245,
                               0.000000000003, -0.000000000000001]> :
                        tensor<5xf32>
                      } : () -> tensor<5xf32>
  %1 = "tosa.cast"(%0) : (tensor<5xf32>) -> tensor<5xf16>
  return %1 : tensor<5xf16>
}

// CHECK-LABEL: @cast_fold_f32_to_f16_infinity_zero_nan
func.func @cast_fold_f32_to_f16_infinity_zero_nan() -> tensor<5xf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0x7C00, 0xFC00, 0.{{0*}}e+00, -0.{{0*}}e+00, 0x7E00{{.*}}tensor<5xf16>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[0x7F800000, 0xFF800000, 0.0, -0.0, 0x7FC00000]> :
                        tensor<5xf32>
                      } : () -> tensor<5xf32>
  %1 = "tosa.cast"(%0) : (tensor<5xf32>) -> tensor<5xf16>
  return %1 : tensor<5xf16>
}

// CHECK-LABEL: @cast_fold_f16_to_f32
func.func @cast_fold_f16_to_f32() -> tensor<5xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}1.2{{0*}}e+01, 0.{{0*}}e+00, 5.{{.*}}, 3.2{{.*}}+04, -3.2{{.*}}e+04{{.*}}tensor<5xf32>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[12.0, 0.0, 5.2, 32770.11, -32770.11]> :
                        tensor<5xf16>
                      } : () -> tensor<5xf16>
  %1 = "tosa.cast"(%0) : (tensor<5xf16>) -> tensor<5xf32>
  return %1 : tensor<5xf32>
}

// CHECK-LABEL: @cast_fold_f16_to_f32
func.func @cast_fold_f16_to_f32_infinity_zero_nan() -> tensor<5xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0x7F800000, 0xFF800000, 0.{{0*}}e+00, -0.{{0*}}e+00, 0x7FC00000{{.*}}tensor<5xf32>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[0x7C00, 0xFC00, 0.0, -0.0, 0x7E00]> :
                        tensor<5xf16>
                      } : () -> tensor<5xf16>
  %1 = "tosa.cast"(%0) : (tensor<5xf16>) -> tensor<5xf32>
  return %1 : tensor<5xf32>
}
