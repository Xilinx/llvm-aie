// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold %s | FileCheck %s

// CHECK-LABEL: @log_fold_single_valued
func.func @log_fold_single_valued() -> tensor<f32> {
  // 0xFFC00000 is the value for NAN
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0xFFC00000{{.*}}tensor<f32>
  // CHECK-NOT: tosa.log
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<-1.0> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.log"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @log_int
func.func @log_int() -> tensor<i32> {
  // CHECK: tosa.const{{.*}}12{{.*}}tensor<i32>
  // CHECK: [[RES:]] ={{.*}}tosa.log
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<12> : tensor<i32>} : () -> tensor<i32>
  %1 = "tosa.log"(%0) : (tensor<i32>) -> tensor<i32>
  return %1 : tensor<i32>
}

// CHECK-LABEL: @log_fold_splat
func.func @log_fold_splat() -> tensor<12x7xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}-2.77258873
  // CHECK-NOT: tosa.log
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0.0625> : tensor<12x7xf32>} : () -> tensor<12x7xf32>
  %1 = "tosa.log"(%0) : (tensor<12x7xf32>) -> tensor<12x7xf32>
  return %1 : tensor<12x7xf32>
}

// CHECK-LABEL: @log_fold_bf16
func.func @log_fold_bf16() -> tensor<12x7xbf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}-2.765630e+00
  // CHECK-NOT: tosa.log
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0.0625> : tensor<12x7xbf16>} : () -> tensor<12x7xbf16>
  %1 = "tosa.log"(%0) : (tensor<12x7xbf16>) -> tensor<12x7xbf16>
  return %1 : tensor<12x7xbf16>
}

// CHECK-LABEL: @log_zero
func.func @log_zero() -> tensor<f32> {
  // 0xFF800000 is the value for -Inf
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0xFF800000
  // CHECK-NOT: tosa.log
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0.0> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.log"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @log_neg_zero
func.func @log_neg_zero() -> tensor<f32> {
  // 0xFF800000 is the value for -Inf
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0xFF800000
  // CHECK-NOT: tosa.log
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<-0.0> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.log"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @log_nan
func.func @log_nan() -> tensor<f32> {
  // 0x7FC00000 is the value for NAN
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0x7FC00000
  // CHECK-NOT: tosa.log
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0x7FC00000> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.log"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @log_infinity
func.func @log_infinity() -> tensor<f32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0x7F800000
  // CHECK-NOT: tosa.log
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0x7F800000> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.log"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @log_neg_infinity
func.func @log_neg_infinity() -> tensor<f32> {
  // 0xFFC00000 is the value for NAN
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0xFFC00000
  // CHECK-NOT: tosa.log
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0xFF800000> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.log"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @log_neg_value
func.func @log_neg_value() -> tensor<f32> {
  // 0xFFC00000 is the value for NAN
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0xFFC00000
  // CHECK-NOT: tosa.log
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<-4.0> : tensor<f32>} : () -> tensor<f32>
  %1 = "tosa.log"(%0) : (tensor<f32>) -> tensor<f32>
  return %1 : tensor<f32>
}

// CHECK-LABEL: @log_no_fold
// The folding optimization works only intra-procedurally, so we won't be able
// to fold anything here
func.func @log_no_fold(%arg0: tensor<?x?xf32>) -> tensor<?x?xf32> {
  // CHECK: tosa.log
  // CHECK-NEXT: return
  %0 = "tosa.log"(%arg0) : (tensor<?x?xf32>) -> tensor<?x?xf32>
  return %0 : tensor<?x?xf32>
}

// CHECK-LABEL: @log_fold_f16
func.func @log_fold_f16() -> tensor<12x7xf16> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}-2.773440e+00
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<6.250000e-02> : tensor<12x7xf16>} : () -> tensor<12x7xf16>
  %1 = "tosa.log"(%0) : (tensor<12x7xf16>) -> tensor<12x7xf16>
  return %1 : tensor<12x7xf16>
}

// CHECK-LABEL: @log_fold
func.func @log_fold() -> tensor<4x6xf32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const
  // CHECK-SAME{LITERAL}: [[-1.73840833, -2.43726015, -0.52357316, 0.38526243, 0xFFC00000, 1.07202876],
  // CHECK-SAME{LITERAL}: [0xFFC00000, 0.359421164, 0.42415604, 0xFFC00000, 0xFFC00000, 0xFFC00000],
  // CHECK-SAME{LITERAL}: [0xFFC00000, -2.57570696, -0.63922733, 0.121509403, -1.40813112, -0.364419162],
  // CHECK-SAME{LITERAL}: [-0.707448959, 0xFFC00000, 0.592884779, -1.96825802, 0.438577473, 0xFFC00000]]
  // CHECK-NOT: tosa.log
  // CHECK: return [[RES]]
  %0 = "tosa.const"() { value = dense<[
                        [ 0.1758,  0.0874,  0.5924,  1.4700, -1.1424,  2.9213],
                        [-0.2078,  1.4325,  1.5283, -0.0121, -0.2306, -1.3377],
                        [-0.0804,  0.0761,  0.5277,  1.1292,  0.2446,  0.6946],
                        [ 0.4929, -0.6524,  1.8092,  0.1397,  1.5505, -1.0270]]>
                        : tensor<4x6xf32>
                      } : () -> tensor<4x6xf32>
  %1 = "tosa.log"(%0) : (tensor<4x6xf32>) -> tensor<4x6xf32>
  return %1 : tensor<4x6xf32>
}

// CHECK-LABEL: @log_of_const_sparse
// Sparse tensors are currently not supported
func.func @log_of_const_sparse() -> tensor<32xbf16> {
  // CHECK: tosa.const
  // CHECK: tosa.log
    %0 = "tosa.const"() { value = sparse<
          [[0], [3], [11], [17], [20], [23], [25], [30], [31]],
          [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0]>
          : tensor<32xbf16> } : () -> tensor<32xbf16>
    %1 = "tosa.log"(%0) : (tensor<32xbf16>) -> tensor<32xbf16>
    return %1 : tensor<32xbf16>
}
