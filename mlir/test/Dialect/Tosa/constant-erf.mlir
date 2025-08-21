// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold %s | FileCheck %s

// CHECK-LABEL: @erf_fold_single_valued
func.func @erf_fold_single_valued() -> tensor<f32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}-0.842700779{{.*}}tensor<f32>
  // CHECK-NOT: tosa.erf
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<-1.0> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.erf"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @erf_int
func.func @erf_int() -> tensor<i32> {
  // CHECK: tosa.const{{.*}}12{{.*}}tensor<i32>
  // CHECK: [[RES:]] ={{.*}}tosa.erf
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<12> : tensor<i32>} : () -> tensor<i32>
  %1 = "tosa.erf"(%0) : (tensor<i32>) -> tensor<i32>
  return %1 : tensor<i32>
}

// CHECK-LABEL: @erf_fold_splat
func.func @erf_fold_splat() -> tensor<12x7xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0.0704319775
  // CHECK-NOT: tosa.erf
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0.0625> : tensor<12x7xf32>} : () -> tensor<12x7xf32>
  %1 = "tosa.erf"(%0) : (tensor<12x7xf32>) -> tensor<12x7xf32>
  return %1 : tensor<12x7xf32>
}

// CHECK-LABEL: @erf_fold_bf16
func.func @erf_fold_bf16() -> tensor<12x7xbf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}7.031250e-02
  // CHECK-NOT: tosa.erf
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0.0625> : tensor<12x7xbf16>} : () -> tensor<12x7xbf16>
  %1 = "tosa.erf"(%0) : (tensor<12x7xbf16>) -> tensor<12x7xbf16>
  return %1 : tensor<12x7xbf16>
}

// CHECK-LABEL: @erf_zero
func.func @erf_zero() -> tensor<f32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0.000000e+00
  // CHECK-NOT: tosa.erf
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0.0> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.erf"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @erf_neg_zero
func.func @erf_neg_zero() -> tensor<f32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}-0.000000e+00
  // CHECK-NOT: tosa.erf
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<-0.0> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.erf"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @erf_nan
func.func @erf_nan() -> tensor<f32> {
  // 0x7FC00000 is the value for NAN
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0x7FC00000
  // CHECK-NOT: tosa.erf
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0x7FC00000> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.erf"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @erf_infinity
func.func @erf_infinity() -> tensor<f32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}1.000000e+00
  // CHECK-NOT: tosa.erf
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0x7F800000> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.erf"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @erf_neg_infinity
func.func @erf_neg_infinity() -> tensor<f32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}-1.000000e+00
  // CHECK-NOT: tosa.erf
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0xFF800000> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.erf"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @erf_neg_value
func.func @erf_neg_value() -> tensor<f32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}-1.000000e+00
  // CHECK-NOT: tosa.erf
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<-4.0> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.erf"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @erf_no_fold
// The folding optimization works only intra-procedurally, so we won't be able
// to fold anything here
func.func @erf_no_fold(%arg0: tensor<?x?xf32>) -> tensor<?x?xf32> {
  // CHECK: tosa.erf
  // CHECK-NEXT: return
  %0 = "tosa.erf"(%arg0) : (tensor<?x?xf32>) -> tensor<?x?xf32>
  return %0 : tensor<?x?xf32>
}

// CHECK-LABEL: @erf_no_fold_f16
func.func @erf_no_fold_f16() -> tensor<12x7xf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}6.250000e-02
  // CHECK: tosa.erf
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<6.250000e-02> : tensor<12x7xf16>} : () -> tensor<12x7xf16>
  %1 = "tosa.erf"(%0) : (tensor<12x7xf16>) -> tensor<12x7xf16>
  return %1 : tensor<12x7xf16>
}

// CHECK-LABEL: @erf_fold
func.func @erf_fold() -> tensor<4x6xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const
  // CHECK-SAME{LITERAL}: [[0.1963
  // CHECK-SAME{LITERAL}: [-0.2311
  // CHECK-SAME{LITERAL}: [-0.0905
  // CHECK-SAME{LITERAL}: [0.5142
  // CHECK-NOT: tosa.erf
  // CHECK: return [[RES]]
  %0 = "tosa.const"() { value = dense<[
                        [ 0.1758,  0.0874,  0.5924,  1.4700, -1.1424,  2.9213],
                        [-0.2078,  1.4325,  1.5283, -0.0121, -0.2306, -1.3377],
                        [-0.0804,  0.0761,  0.5277,  1.1292,  0.2446,  0.6946],
                        [ 0.4929, -0.6524,  1.8092,  0.1397,  1.5505, -1.0270]]>
                        : tensor<4x6xf32>
                      } : () -> tensor<4x6xf32>
  %1 = "tosa.erf"(%0) : (tensor<4x6xf32>) -> tensor<4x6xf32>
  return %1 : tensor<4x6xf32>
}

// CHECK-LABEL: @erf_of_const_sparse
// Sparse tensors are currently not supported
func.func @erf_of_const_sparse() -> tensor<32xbf16> {
  // CHECK: tosa.const
  // CHECK: tosa.erf
    %0 = "tosa.const"() { value = sparse<
          [[0], [3], [11], [17], [20], [23], [25], [30], [31]],
          [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0]>
          : tensor<32xbf16> } : () -> tensor<32xbf16>
    %1 = "tosa.erf"(%0) : (tensor<32xbf16>) -> tensor<32xbf16>
    return %1 : tensor<32xbf16>
}
