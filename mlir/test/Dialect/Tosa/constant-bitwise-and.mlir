// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold %s | FileCheck %s

// CHECK-LABEL: @bitwise_and_fold_single_valued
func.func @bitwise_and_fold_single_valued() -> tensor<i32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}-65536
  // CHECK-NOT: tosa.bitwise_and
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0xFFFFFFFF> : tensor<i32>} : () -> tensor<i32>
  %1 = "tosa.const"() {value = dense<0xFFFF0000> : tensor<i32>} : () -> tensor<i32>
  %2 = "tosa.bitwise_and"(%0, %1) : (tensor<i32>, tensor<i32>) -> tensor<i32>
  return %2 : tensor<i32>
}

// CHECK-LABEL: @bitwise_and_fold_splat
func.func @bitwise_and_fold_splat() -> tensor<12x7xi32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}65535
  // CHECK-NOT: tosa.bitwise_and
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<0xFFFFFFFF> : tensor<12x7xi32>} : () -> tensor<12x7xi32>
  %1 = "tosa.const"() {value = dense<0x0000FFFF> : tensor<12x7xi32>} : () -> tensor<12x7xi32>
  %2 = "tosa.bitwise_and"(%0, %1) : (tensor<12x7xi32>, tensor<12x7xi32>) -> tensor<12x7xi32>
  return %2 : tensor<12x7xi32>
}

// CHECK-LABEL: @bitwise_and_no_fold
// The folding optimization works only intra-procedurally, so we won't be able
// to fold anything here
func.func @bitwise_and_no_fold(%arg0: tensor<?x?xi32>, %arg1: tensor<?x?xi32>) -> tensor<?x?xi32> {
  // CHECK: tosa.bitwise_and
  // CHECK-NEXT: return
  %0 = "tosa.bitwise_and"(%arg0, %arg1) : (tensor<?x?xi32>, tensor<?x?xi32>) -> tensor<?x?xi32>
  return %0 : tensor<?x?xi32>
}

// CHECK-LABEL: @bitwise_and_fold
func.func @bitwise_and_fold() -> tensor<2x6xi32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const
  // CHECK-SAME{LITERAL}: [[-1, -2, -3, -4, -5, -6],
  // CHECK-SAME{LITERAL}:  [1, 2, 3, 4, 5, 6]]
  // CHECK-NOT: tosa.bitwise_and
  // CHECK: return [[RES]]
  %0 = "tosa.const"() { value = dense<
                        [[0xFFFFFFFF, 0xFFFFFFFE, 0xFFFFFFFD,
                          0xFFFFFFFC, 0xFFFFFFFB, 0xFFFFFFFA],
                         [1, 2, 3, 4, 5, 6]]>
                        : tensor<2x6xi32>
                      } : () -> tensor<2x6xi32>
  %1 = "tosa.const"() { value = dense<
                        [[0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                          0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF],
                         [0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                          0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF]]>
                        : tensor<2x6xi32>
                      } : () -> tensor<2x6xi32>
  %2 = "tosa.bitwise_and"(%0, %1) : (tensor<2x6xi32>, tensor<2x6xi32>) -> tensor<2x6xi32>
  return %2 : tensor<2x6xi32>
}

// CHECK-LABEL: @bitwise_and_of_const_sparse
// Sparse tensors are currently not supported
func.func @bitwise_and_of_const_sparse() -> tensor<32xi8> {
  // CHECK: tosa.const
  // CHECK: tosa.bitwise_and
    %0 = "tosa.const"() { value = sparse<
          [[0], [3], [11], [17], [20], [23], [25], [30], [31]],
          [0, 1, 2, 3, 4, 0xFF, 0xFE, 0xFD, 0xFC]>
          : tensor<32xi8> } : () -> tensor<32xi8>
    %1 = "tosa.const"() { value = sparse<
          [[0], [3], [11], [17], [20], [23], [25], [30], [31]],
          [0, 1, 2, 3, 4, 0xFF, 0xFE, 0xFD, 0xFC]>
          : tensor<32xi8> } : () -> tensor<32xi8>
    %2 = "tosa.bitwise_and"(%0, %1) : (tensor<32xi8>, tensor<32xi8>) -> tensor<32xi8>
    return %2 : tensor<32xi8>
}
