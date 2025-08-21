// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold %s | FileCheck %s

// CHECK-LABEL: @exp_fold_single_valued
func.func @exp_fold_single_valued() -> tensor<f32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0.3678{{.*}} tensor<f32>
  // CHECK-NOT: tosa.exp
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<-1.0> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.exp"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @exp_int
func.func @exp_int() -> tensor<i32> {
  // CHECK: tosa.const{{.*}}12{{.*}}tensor<i32>
  // CHECK: [[RES:]] ={{.*}}tosa.exp
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<12> : tensor<i32>} : () -> tensor<i32>
  %1 = "tosa.exp"(%0) : (tensor<i32>) -> tensor<i32>
  return %1 : tensor<i32>
}

// CHECK-LABEL: @exp_fold_splat
func.func @exp_fold_splat() -> tensor<12x7xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}1.06449{{[0-9]+}}
  // CHECK-NOT: tosa.exp
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0.0625> : tensor<12x7xf32>} : () -> tensor<12x7xf32>
  %1 = "tosa.exp"(%0) : (tensor<12x7xf32>) -> tensor<12x7xf32>
  return %1 : tensor<12x7xf32>
}

// CHECK-LABEL: @exp_fold_bf16
func.func @exp_fold_bf16() -> tensor<12x7xbf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}1.062500e+00
  // CHECK-NOT: tosa.exp
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0.0625> : tensor<12x7xbf16>} : () -> tensor<12x7xbf16>
  %1 = "tosa.exp"(%0) : (tensor<12x7xbf16>) -> tensor<12x7xbf16>
  return %1 : tensor<12x7xbf16>
}

// CHECK-LABEL: @exp_zero
func.func @exp_zero() -> tensor<f32> {
  // 0xFF800000 is the value for -Inf
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}1.000000e+00
  // CHECK-NOT: tosa.exp
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0.0> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.exp"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @exp_neg_zero
func.func @exp_neg_zero() -> tensor<f32> {
  // 0xFF800000 is the value for -Inf
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}1.000000e+00
  // CHECK-NOT: tosa.exp
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<-0.0> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.exp"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @exp_nan
func.func @exp_nan() -> tensor<f32> {
  // 0x7FC00000 is the value for NAN
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0x7FC00000
  // CHECK-NOT: tosa.exp
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0x7FC00000> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.exp"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @exp_infinity
func.func @exp_infinity() -> tensor<f32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0x7F800000
  // CHECK-NOT: tosa.exp
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0x7F800000> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.exp"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @exp_neg_infinity
func.func @exp_neg_infinity() -> tensor<f32> {
  // 0xFFC00000 is the value for NAN
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0.000000e+00
  // CHECK-NOT: tosa.exp
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0xFF800000> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.exp"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @exp_neg_value
func.func @exp_neg_value() -> tensor<f32> {
  // 0xFFC00000 is the value for NAN
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0.0183{{[0-9]+}}
  // CHECK-NOT: tosa.exp
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<-4.0> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.exp"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @exp_no_fold
// The folding optimization works only intra-procedurally, so we won't be able
// to fold anything here
func.func @exp_no_fold(%arg0: tensor<?x?xf32>) -> tensor<?x?xf32> {
  // CHECK: tosa.exp
  // CHECK-NEXT: return
  %0 = "tosa.exp"(%arg0) : (tensor<?x?xf32>) -> tensor<?x?xf32>
  return %0 : tensor<?x?xf32>
}

// CHECK-LABEL: @exp_fold_f16
func.func @exp_fold_f16() -> tensor<12x7xf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}1.0644{{[0-9]+}}e+00
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<6.250000e-02> : tensor<12x7xf16>} : () -> tensor<12x7xf16>
  %1 = "tosa.exp"(%0) : (tensor<12x7xf16>) -> tensor<12x7xf16>
  return %1 : tensor<12x7xf16>
}

// CHECK-LABEL: @exp_fold
func.func @exp_fold() -> tensor<4x6xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const
  // CHECK-SAME: 1.192{{[0-9]+}}, 1.0913{{[0-9]+}}, 1.8083{{[0-9]+}}, 4.3492{{[0-9]+}}, 0.3190{{[0-9]+}}, 18.5654{{[0-9]+}}
  // CHECK-SAME: [0.8123{{[0-9]+}}, 4.1891{{[0-9]+}}, 4.6103{{[0-9]+}}, 0.9879{{[0-9]+}}, 7.940{{[0-9]+}}e-01, 0.2624{{[0-9]+}}
  // CHECK-SAME: [0.9227{{[0-9]+}}, 1.0790{{[0-9]+}}, 1.6950{{[0-9]+}}, 3.0931{{[0-9]+}}, 1.2771{{[0-9]+}}, 2.0029{{[0-9]+}}
  // CHECK-SAME: [1.6370{{[0-9]+}}, 0.5207{{[0-9]+}}, 6.1055{{[0-9]+}}, 1.1499{{[0-9]+}}, 4.7138{{[0-9]+}}, 0.35807{{[0-9]+}}
  // CHECK-NOT: tosa.exp
  // CHECK: return [[RES]]
  %0 = "tosa.const"() { value = dense<[
                        [ 0.1758,  0.0874,  0.5924,  1.4700, -1.1424,  2.9213],
                        [-0.2078,  1.4325,  1.5283, -0.0121, -0.2306, -1.3377],
                        [-0.0804,  0.0761,  0.5277,  1.1292,  0.2446,  0.6946],
                        [ 0.4929, -0.6524,  1.8092,  0.1397,  1.5505, -1.0270]]>
                        : tensor<4x6xf32>
                      } : () -> tensor<4x6xf32>
  %1 = "tosa.exp"(%0) : (tensor<4x6xf32>) -> tensor<4x6xf32>
  return %1 : tensor<4x6xf32>
}

// CHECK-LABEL: @exp_of_const_sparse
// Sparse tensors are currently not supported
func.func @exp_of_const_sparse() -> tensor<32xbf16> {
  // CHECK: tosa.const
  // CHECK: tosa.exp
    %0 = "tosa.const"() { value = sparse<
          [[0], [3], [11], [17], [20], [23], [25], [30], [31]],
          [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0]>
          : tensor<32xbf16> } : () -> tensor<32xbf16>
    %1 = "tosa.exp"(%0) : (tensor<32xbf16>) -> tensor<32xbf16>
    return %1 : tensor<32xbf16>
}
