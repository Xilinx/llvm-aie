// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold %s | FileCheck %s

// CHECK-LABEL: @pow_fold_tiny
func.func @pow_fold_tiny() -> tensor<f32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}1.6{{0*}}e+01{{.*}}tensor<f32>
  // CHECK-NOT: tosa.pow
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<4.0> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.const"() {value = dense<2.0> : tensor<f32>} : () -> tensor<f32>
  %2 = "tosa.pow"(%0, %1) : (tensor<f32>, tensor<f32>) -> tensor<f32>
  return %2 : tensor<f32>
}

// CHECK-LABEL: @pow_fold_tensor
func.func @pow_fold_tensor() -> tensor<3xf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}2.56{{0*}}e+02, 1.191410e+00, -3.099610e+00{{.*}}tensor<3xf16>
  // CHECK-NOT: tosa.pow
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<[4.0, 2.22, -3.1]> : tensor<3xf16>} : () -> tensor<3xf16>
  %1 = "tosa.const"() {value = dense<[4.0, 0.22, 1.0]> : tensor<3xf16>} : () -> tensor<3xf16>
  %2 = "tosa.pow"(%0, %1) : (tensor<3xf16>, tensor<3xf16>) -> tensor<3xf16>
  return %2 : tensor<3xf16>
}

// CHECK-LABEL: @pow_fold_overflow
func.func @pow_fold_overflow() -> tensor<2xf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0x7C00, 0xFC00{{.*}}tensor<2xf16>
  // CHECK-NOT: tosa.pow
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<[65500.0, -65500.0]> : tensor<2xf16>} : () -> tensor<2xf16>
  %1 = "tosa.const"() {value = dense<[2.0, 3.0]> : tensor<2xf16>} : () -> tensor<2xf16>
  %2 = "tosa.pow"(%0, %1) : (tensor<2xf16>, tensor<2xf16>) -> tensor<2xf16>
  return %2 : tensor<2xf16>
}

// CHECK-LABEL: @pow_fold_underflow
func.func @pow_fold_underflow() -> tensor<2xf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}[0.0{{0*}}e+00, -0.0{{0*}}e+00{{.*}}tensor<2xf16>
  // CHECK-NOT: tosa.pow
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<[0.000001, -0.000001]> : tensor<2xf16>} : () -> tensor<2xf16>
  %1 = "tosa.const"() {value = dense<[10.0, 9.0]> : tensor<2xf16>} : () -> tensor<2xf16>
  %2 = "tosa.pow"(%0, %1) : (tensor<2xf16>, tensor<2xf16>) -> tensor<2xf16>
  return %2 : tensor<2xf16>
}

// CHECK-LABEL: @pow_fold_nan_cases
func.func @pow_fold_nan_cases() -> tensor<3xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}<0x7FC00000>{{.*}}tensor<3xf32>
  // CHECK-NOT: tosa.pow
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<[0.0, -1.25, 0x7FC00000]> : tensor<3xf32>} : () -> tensor<3xf32>
  %1 = "tosa.const"() {value = dense<[0.0, 0.745, 2.0]> : tensor<3xf32>} : () -> tensor<3xf32>
  %2 = "tosa.pow"(%0, %1) : (tensor<3xf32>, tensor<3xf32>) -> tensor<3xf32>
  return %2 : tensor<3xf32>
}

// CHECK-LABEL: @pow_fold_equal_args
func.func @pow_fold_equal_args() -> tensor<2xf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}2.56{{0*}}e+02, 5.8
  // CHECK-NOT: tosa.pow
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<[4.0, 2.22]> : tensor<2xf16>} : () -> tensor<2xf16>
  %2 = "tosa.pow"(%0, %0) : (tensor<2xf16>, tensor<2xf16>) -> tensor<2xf16>
  return %2 : tensor<2xf16>
}

// CHECK-LABEL: @pow_fold_tensor_broadcast_exp
func.func @pow_fold_tensor_broadcast_exp() -> tensor<3xf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}1.6{{0*}}e+01, 4.929690e+00, 9.609370e+00{{.*}}tensor<3xf16>
  // CHECK-NOT: tosa.pow
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<[4.0, 2.22, -3.1]> : tensor<3xf16>} : () -> tensor<3xf16>
  %1 = "tosa.const"() {value = dense<2.0> : tensor<1xf16>} : () -> tensor<1xf16>
  %2 = "tosa.pow"(%0, %1) : (tensor<3xf16>, tensor<1xf16>) -> tensor<3xf16>
  return %2 : tensor<3xf16>
}

// CHECK-LABEL: @pow_fold_tensor_broadcast_base
func.func @pow_fold_tensor_broadcast_base() -> tensor<3xf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}1.6{{0*}}e+01, 4.660160e+00, 1.166380e-01{{.*}}tensor<3xf16>
  // CHECK-NOT: tosa.pow
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<[4.0, 2.22, -3.1]> : tensor<3xf16>} : () -> tensor<3xf16>
  %1 = "tosa.const"() {value = dense<2.0> : tensor<1xf16>} : () -> tensor<1xf16>
  %2 = "tosa.pow"(%1, %0) : (tensor<1xf16>, tensor<3xf16>) -> tensor<3xf16>
  return %2 : tensor<3xf16>
}

// CHECK-LABEL: @pow_fold_broadcast_two_dimensions
func.func @pow_fold_broadcast_two_dimensions() -> tensor<3x3xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const
  // CHECK-SAME{LITERAL}: [[388.023529, 1.102940e+03, 2554.37329],
  // CHECK-SAME{LITERAL}:  [75281.1328, 538664.813, 0x4A1FF040],
  // CHECK-SAME{LITERAL}:  [24.2514629, 42.4044418, 66.4508896]]
  // CHECK-NOT: tosa.pow
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<[[4.0, 5.1, 6.2]]> : tensor<1x3xf32>} : () -> tensor<1x3xf32>
  %1 = "tosa.const"() {value = dense<[[4.3], [8.1], [2.3]]> : tensor<3x1xf32>} : () -> tensor<3x1xf32>
  %2 = "tosa.pow"(%0, %1) : (tensor<1x3xf32>, tensor<3x1xf32>) -> tensor<3x3xf32>
  return %2 : tensor<3x3xf32>
}
