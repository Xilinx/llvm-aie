// RUN: mlir-opt --split-input-file -verify-diagnostics --tosa-layerwise-constant-fold %s | FileCheck %s

// Int clamp

// CHECK-LABEL: @clamp_fold_integer
func.func @clamp_fold_integer() -> tensor<3xi16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}-2, 0, 1{{.*}}tensor<3xi16>
  // CHECK-NOT: tosa.clamp
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<[-12, 0, 5]> : tensor<3xi16>} : () -> tensor<3xi16>
  %1 = "tosa.clamp"(%0) {max_fp = 0.00 : f32, max_int = 1 : i64, min_fp = 0.0 : f32, min_int = -2 : i64}
        : (tensor<3xi16>) -> tensor<3xi16>
  return %1 : tensor<3xi16>
}

// CHECK-LABEL: @clamp_fold_integer_equal_lower_upper
func.func @clamp_fold_integer_equal_lower_upper() -> tensor<3xi8> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}<17>{{.*}}tensor<3xi8>
  // CHECK-NOT: tosa.clamp
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<[2, 0, -5]> : tensor<3xi8>} : () -> tensor<3xi8>
  %1 = "tosa.clamp"(%0) {max_fp = 0.00 : f32, max_int = 17 : i64, min_fp = 0.0 : f32, min_int = 17 : i64}
        : (tensor<3xi8>) -> tensor<3xi8>
  return %1 : tensor<3xi8>
}

// CHECK-LABEL: @clamp_fold_integer_maximum_larger_than_result_type
func.func @clamp_fold_integer_maximum_larger_than_result_type() -> tensor<3xi8> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}9, 4, 4{{.*}}tensor<3xi8>
  // CHECK-NOT: tosa.clamp
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<[9, 0, -5]> : tensor<3xi8>} : () -> tensor<3xi8>
  %1 = "tosa.clamp"(%0) {max_fp = 0.00 : f32, max_int = 9223372036854775807 : i64, min_fp = 0.0 : f32, min_int = 4 : i64}
        : (tensor<3xi8>) -> tensor<3xi8>
  return %1 : tensor<3xi8>
}

// Float clamp

// CHECK-LABEL: @clamp_fold_float
func.func @clamp_fold_float() -> tensor<3xf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}-2.{{0*}}e+00, {{[8-9]}}.{{[0-9]*}}e-01, 1.{{0*}}e+00{{.*}}tensor<3xf16>
  // CHECK-NOT: tosa.clamp
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<[-12.4, 0.9, 5.2]> : tensor<3xf16>} : () -> tensor<3xf16>
  %1 = "tosa.clamp"(%0) {max_fp = 1.00 : f32, max_int = 1594 : i64, min_fp = -2.0 : f32, min_int = -17 : i64}
        : (tensor<3xf16>) -> tensor<3xf16>
  return %1 : tensor<3xf16>
}

// CHECK-LABEL: @clamp_fold_float_infty_nan
func.func @clamp_fold_float_infty_nan() -> tensor<5xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}1.{{0*}}e+00, -2.{{0*}}e+00, 0.{{0*}}e+00, -0.{{0*}}e+00, 0x7FC00000{{.*}}tensor<5xf32>
  // CHECK-NOT: tosa.clamp
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[0x7F800000, 0xFF800000, 0.0, -0.0, 0x7FC00000]> :
                        tensor<5xf32>
                      } : () -> tensor<5xf32>
  %1 = "tosa.clamp"(%0) {max_fp = 1.00 : f32, max_int = 1594 : i64, min_fp = -2.0 : f32, min_int = -17 : i64}
        : (tensor<5xf32>) -> tensor<5xf32>
  return %1 : tensor<5xf32>
}

// CHECK-LABEL: @clamp_fold_float_infinity_upper
func.func @clamp_fold_float_infinity_upper() -> tensor<5xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0x7F800000, -2.{{0*}}e+00, 9.{{0*}}e+00, -0.{{0*}}e+00, 0x7FC00000{{.*}}tensor<5xf32>
  // CHECK-NOT: tosa.clamp
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[0x7F800000, 0xFF800000, 9.0, -0.0, 0x7FC00000]> :
                        tensor<5xf32>
                      } : () -> tensor<5xf32>
  %1 = "tosa.clamp"(%0) {max_fp = 0x7F800000 : f32, max_int = 1594 : i64, min_fp = -2.0 : f32, min_int = -17 : i64}
        : (tensor<5xf32>) -> tensor<5xf32>
  return %1 : tensor<5xf32>
}

// CHECK-LABEL: @clamp_fold_float_maximum_larger_than_result_type
func.func @clamp_fold_float_maximum_larger_than_result_type() -> tensor<2xf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}1.83{{[0-9]*}}e+01, -5.{{0*}}e-01
  // CHECK-NOT: tosa.clamp
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[18.32, -0.98747]> :
                        tensor<2xf16>
                      } : () -> tensor<2xf16>
  %1 = "tosa.clamp"(%0) {max_fp = 3.4028234e+38 : f32, max_int = 1594 : i64, min_fp = -0.5 : f32, min_int = -17 : i64}
        : (tensor<2xf16>) -> tensor<2xf16>
  return %1 : tensor<2xf16>
}
