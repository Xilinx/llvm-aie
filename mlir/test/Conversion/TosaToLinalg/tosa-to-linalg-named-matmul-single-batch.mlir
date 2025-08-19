// RUN: mlir-opt --split-input-file -pass-pipeline="builtin.module(func.func(tosa-to-linalg-named{use-matmul-for-single-batch=true},cse))" %s -verify-diagnostics -o -| FileCheck %s

// CHECK-LABEL: @matmul
func.func @matmul(%arg0: tensor<1x5x3xf32>, %arg1: tensor<1x3x6xf32>) -> (tensor<1x5x6xf32>) {
  // CHECK: %[[COLLAPSE1:.*]] = tensor.collapse_shape %arg0 {{\[\[}}0, 1], [2]] : tensor<1x5x3xf32> into tensor<5x3xf32>
  // CHECK: %[[COLLAPSE2:.*]] = tensor.collapse_shape %arg1 {{\[\[}}0, 1], [2]] : tensor<1x3x6xf32> into tensor<3x6xf32>
  // CHECK: %[[CONST:.*]] = arith.constant 0.000000e+00
  // CHECK: %[[EMPTY:.*]] = tensor.empty() : tensor<5x6xf32>
  // CHECK: %[[FILL:.*]] = linalg.fill ins(%[[CONST]] : f32) outs(%[[EMPTY]] : tensor<5x6xf32>) -> tensor<5x6xf32>
  // CHECK: %[[MATMUL:.*]] = linalg.matmul ins(%[[COLLAPSE1]], %[[COLLAPSE2]] : tensor<5x3xf32>, tensor<3x6xf32>) outs(%[[FILL]] : tensor<5x6xf32>) -> tensor<5x6xf32>
  // CHECK: tensor.expand_shape %[[MATMUL]] {{\[\[}}0, 1], [2]] output_shape [1, 5, 6] : tensor<5x6xf32> into tensor<1x5x6xf32>
  %0 = "tosa.matmul"(%arg0, %arg1) : (tensor<1x5x3xf32>, tensor<1x3x6xf32>)  -> (tensor<1x5x6xf32>)
  return %0 : tensor<1x5x6xf32>
}

// -----

// CHECK-LABEL: @matmul_quantized
func.func @matmul_quantized(%arg0: tensor<1x5x3xi8>, %arg1: tensor<1x3x6xi8>) -> (tensor<1x5x6xi32>) {
  // CHECK: %[[COLLAPSE1:.*]] = tensor.collapse_shape %arg0 {{\[\[}}0, 1], [2]] : tensor<1x5x3xi8> into tensor<5x3xi8>
  // CHECK: %[[COLLAPSE2:.*]] = tensor.collapse_shape %arg1 {{\[\[}}0, 1], [2]] : tensor<1x3x6xi8> into tensor<3x6xi8>
  // CHECK: %[[VAL_4:.*]] = arith.constant 0
  // CHECK: %[[EMPTY:.*]] = tensor.empty() : tensor<5x6xi32>
  // CHECK: %[[FILL:.*]] = linalg.fill ins(%[[VAL_4]] : i32) outs(%[[EMPTY]] : tensor<5x6xi32>) -> tensor<5x6xi32>
  // CHECK: %[[CONST1:.*]] = arith.constant 1
  // CHECK: %[[CONST2:.*]] = arith.constant 2
  // CHECK: %[[VAL_9:.*]] = linalg.quantized_matmul ins(%[[COLLAPSE1]], %[[COLLAPSE2]], %[[CONST1]], %[[CONST2]] : tensor<5x3xi8>, tensor<3x6xi8>, i32, i32) outs(%[[FILL]] : tensor<5x6xi32>) -> tensor<5x6xi32>
  // CHECK:  tensor.expand_shape %[[VAL_9]] {{\[\[}}0, 1], [2]] output_shape [1, 5, 6] : tensor<5x6xi32> into tensor<1x5x6xi32>
  %0 = "tosa.matmul"(%arg0, %arg1) {quantization_info = #tosa.matmul_quant<a_zp = 1, b_zp = 2>} : (tensor<1x5x3xi8>, tensor<1x3x6xi8>) -> (tensor<1x5x6xi32>)
  return %0 : tensor<1x5x6xi32>
}

// -----

// CHECK-LABEL: @matmul_dyn_batch_no_matmul
func.func @matmul_dyn_batch_no_matmul(%arg0: tensor<?x5x3xf32>, %arg1: tensor<?x3x6xf32>) -> (tensor<?x5x6xf32>) {
  // CHECK: %[[C0:.+]] = arith.constant 0
  // CHECK: %[[DIM:.+]] = tensor.dim %arg0, %[[C0]]
  // CHECK: %[[C0_0:.+]] = arith.constant 0
  // CHECK: %[[INIT:.+]] = tensor.empty(%[[DIM]])
  // CHECK: %[[FILLED:.+]] = linalg.fill ins(%[[C0_0]] : f32) outs(%[[INIT]] : tensor<?x5x6xf32>) -> tensor<?x5x6xf32>
  // CHECK: linalg.batch_matmul ins(%arg0, %arg1 : tensor<?x5x3xf32>, tensor<?x3x6xf32>) outs(%[[FILLED]] : tensor<?x5x6xf32>) -> tensor<?x5x6xf32>
  %0 = "tosa.matmul"(%arg0, %arg1) : (tensor<?x5x3xf32>, tensor<?x3x6xf32>)  -> (tensor<?x5x6xf32>)
  return %0 : tensor<?x5x6xf32>
}

// -----

// CHECK-LABEL: @matmul_dyn_independent_dim
func.func @matmul_dyn_independent_dim(%arg0: tensor<1x5x3xf32>, %arg1: tensor<1x3x?xf32>) -> (tensor<1x5x?xf32>) {
  // CHECK: %[[CONST1:.*]] = arith.constant 2
  // CHECK: %[[DIM:.*]] = tensor.dim %arg1, %[[CONST1]] : tensor<1x3x?xf32>
  // CHECK: %[[COLLAPSE1:.*]] = tensor.collapse_shape %arg0 {{\[\[}}0, 1], [2]] : tensor<1x5x3xf32> into tensor<5x3xf32>
  // CHECK: %[[COLLAPSE2:.*]] = tensor.collapse_shape %arg1 {{\[\[}}0, 1], [2]] : tensor<1x3x?xf32> into tensor<3x?xf32>
  // CHECK: %[[CONST2:.*]] = arith.constant 0.000000e+00
  // CHECK: %[[EMPTY:.*]] = tensor.empty(%[[DIM]]) : tensor<5x?xf32>
  // CHECK: %[[FILL:.*]] = linalg.fill ins(%[[CONST2]] : f32) outs(%[[EMPTY]] : tensor<5x?xf32>) -> tensor<5x?xf32>
  // CHECK: %[[MATMUL:.*]] = linalg.matmul ins(%[[COLLAPSE1]], %[[COLLAPSE2]] : tensor<5x3xf32>, tensor<3x?xf32>) outs(%[[FILL]] : tensor<5x?xf32>) -> tensor<5x?xf32>
  // CHECK:           %[[VAL_10:.*]] = tensor.expand_shape %[[MATMUL]] {{\[\[}}0, 1], [2]] output_shape [1, 5, {{.*}}] : tensor<5x?xf32> into tensor<1x5x?xf32>
  %0 = "tosa.matmul"(%arg0, %arg1) : (tensor<1x5x3xf32>, tensor<1x3x?xf32>)  -> (tensor<1x5x?xf32>)
  return %0 : tensor<1x5x?xf32>
}

// -----

// CHECK-LABEL: @matmul_dyn_independent_dim_no_matmul
func.func @matmul_dyn_independent_dim_no_matmul(%arg0: tensor<1x5x?xf32>, %arg1: tensor<1x?x6xf32>) -> (tensor<1x5x6xf32>) {
  // CHECK: %[[C0:.+]] = arith.constant 0
  // CHECK: %[[INIT:.+]] = tensor.empty()
  // CHECK: %[[FILLED:.+]] = linalg.fill ins(%[[C0]] : f32) outs(%[[INIT]] : tensor<1x5x6xf32>) -> tensor<1x5x6xf32>
  // CHECK: linalg.batch_matmul ins(%arg0, %arg1 : tensor<1x5x?xf32>, tensor<1x?x6xf32>) outs(%[[FILLED]] : tensor<1x5x6xf32>) -> tensor<1x5x6xf32>
  %0 = "tosa.matmul"(%arg0, %arg1) : (tensor<1x5x?xf32>, tensor<1x?x6xf32>)  -> (tensor<1x5x6xf32>)
  return %0 : tensor<1x5x6xf32>
}
