// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold %s | FileCheck %s

// CHECK-LABEL: @rsqrt_fold_single_valued
func.func @rsqrt_fold_single_valued() -> tensor<f32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0.288675129{{.*}}tensor<f32>
  // CHECK-NOT: tosa.rsqrt
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<12.0> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.rsqrt"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @rsqrt_int
func.func @rsqrt_int() -> tensor<i32> {
  // CHECK: tosa.const{{.*}}12{{.*}}tensor<i32>
  // CHECK: [[RES:]] ={{.*}}tosa.rsqrt
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<12> : tensor<i32>} : () -> tensor<i32>
  %1 = "tosa.rsqrt"(%0) : (tensor<i32>) -> tensor<i32>
  return %1 : tensor<i32>
}

// CHECK-LABEL: @rsqrt_fold_splat
func.func @rsqrt_fold_splat() -> tensor<12x7xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}5.0{{0*}}e-01{{.*}}tensor<12x7xf32>
  // CHECK-NOT: tosa.rsqrt
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<4.0> : tensor<12x7xf32>} : () -> tensor<12x7xf32>
  %1 = "tosa.rsqrt"(%0) : (tensor<12x7xf32>) -> tensor<12x7xf32>
  return %1 : tensor<12x7xf32>
}

// CHECK-LABEL: @rsqrt_zero
func.func @rsqrt_zero() -> tensor<f32> {
  // 0x7F800000 is the value for +infinity
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0x7F800000
  // CHECK-NOT: tosa.rsqrt
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0.0> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.rsqrt"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @rsqrt_neg_zero
func.func @rsqrt_neg_zero() -> tensor<f32> {
  // 0xFF800000 is the value for -infinity
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0xFF800000
  // CHECK-NOT: tosa.rsqrt
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<-0.0> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.rsqrt"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @rsqrt_nan
func.func @rsqrt_nan() -> tensor<f32> {
  // 0x7FC00000 is the value for NAN
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0x7FC00000
  // CHECK-NOT: tosa.rsqrt
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0x7FC00000> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.rsqrt"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @rsqrt_infinity
func.func @rsqrt_infinity() -> tensor<f32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}<0.{{0*}}e+00>
  // CHECK-NOT: tosa.rsqrt
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0x7F800000> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.rsqrt"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @rsqrt_neg_infinity
func.func @rsqrt_neg_infinity() -> tensor<f32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0x7FC00000
  // CHECK-NOT: tosa.rsqrt
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0xFF800000> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.rsqrt"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @rsqrt_neg_value
func.func @rsqrt_neg_value() -> tensor<f32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0x7FC00000
  // CHECK-NOT: tosa.rsqrt
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<-4.0> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.rsqrt"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @rsqrt_no_fold
// The folding optimization works only intra-procedurally, so we won't be able
// to fold anything here
func.func @rsqrt_no_fold(%arg0: tensor<?x?xf32>) -> tensor<?x?xf32> {
  // CHECK: tosa.rsqrt
  // CHECK-NEXT: return
  %0 = "tosa.rsqrt"(%arg0) : (tensor<?x?xf32>) -> tensor<?x?xf32>
  return %0 : tensor<?x?xf32>
}

// CHECK-LABEL: @rsqrt_fold
func.func @rsqrt_fold() -> tensor<4x6xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const
  // CHECK-SAME{LITERAL}: [[2.38501215, 3.38255072, 1.29924929, 0.824786067, 0x7FC00000, 0.585075498],
  // CHECK-SAME{LITERAL}:  [0x7FC00000, 0.835511982, 0.808901607, 0x7FC00000, 0x7FC00000, 0x7FC00000],
  // CHECK-SAME{LITERAL}:  [0x7FC00000, 3.62499738, 1.37659585, 0.941054106, 2.02195644, 1.19986558],
  // CHECK-SAME{LITERAL}:  [1.42436266, 0x7FC00000, 0.743458449, 2.6754806, 0.803089797, 0x7FC00000]]
  // CHECK-NOT: tosa.rsqrt
  // CHECK: return [[RES]]
  %0 = "tosa.const"() { value = dense<[
                        [ 0.1758,  0.0874,  0.5924,  1.4700, -1.1424,  2.9213],
                        [-0.2078,  1.4325,  1.5283, -0.0121, -0.2306, -1.3377],
                        [-0.0804,  0.0761,  0.5277,  1.1292,  0.2446,  0.6946],
                        [ 0.4929, -0.6524,  1.8092,  0.1397,  1.5505, -1.0270]]>
                        : tensor<4x6xf32>
                      } : () -> tensor<4x6xf32>
  %1 = "tosa.rsqrt"(%0) : (tensor<4x6xf32>) -> tensor<4x6xf32>
  return %1 : tensor<4x6xf32>
}

// CHECK-LABEL: @rsqrt_fold_single_valued_bf16
func.func @rsqrt_fold_single_valued_bf16() -> tensor<bf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}2.890630e-01{{.*}}tensor<bf16>
  // CHECK-NOT: tosa.rsqrt
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<12.0> : tensor<bf16>} : () -> tensor<bf16>
  %1 = "tosa.rsqrt"(%0) : (tensor<bf16>) -> tensor<bf16>
  return %1 : tensor<bf16>
}

// CHECK-LABEL: @rsqrt_of_const_sparse
// Sparse tensors are currently not supported
func.func @rsqrt_of_const_sparse() -> tensor<32xbf16> {
  // CHECK: tosa.const
  // CHECK: tosa.rsqrt
    %0 = "tosa.const"() { value = sparse<
          [[0], [3], [11], [17], [20], [23], [25], [30], [31]],
          [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0]>
          : tensor<32xbf16> } : () -> tensor<32xbf16>
    %1 = "tosa.rsqrt"(%0) : (tensor<32xbf16>) -> tensor<32xbf16>
    return %1 : tensor<32xbf16>
}
