// RUN: mlir-opt --split-input-file -pass-pipeline="builtin.module(func.func(tosa-to-linalg-named))" %s -verify-diagnostics -o -| FileCheck %s

// CHECK-LABEL: @matmul
func.func @matmul(%arg0: tensor<1x5x3xi2>, %arg1: tensor<1x3x6xi2>) -> (tensor<1x5x6xi2>) {
  // CHECK: [[C0:%.+]] = arith.constant 0 : i2
  // CHECK: [[INIT:%.+]] = tensor.empty()
  // CHECK: [[FILLED:%.+]] = linalg.fill ins([[C0]] : i2) outs([[INIT]] : tensor<1x5x6xi2>) -> tensor<1x5x6xi2>
  // CHECK: linalg.batch_matmul ins(%arg0, %arg1 : tensor<1x5x3xi2>, tensor<1x3x6xi2>) outs([[FILLED]] : tensor<1x5x6xi2>) -> tensor<1x5x6xi2>
  %0 = "tosa.matmul"(%arg0, %arg1) : (tensor<1x5x3xi2>, tensor<1x3x6xi2>)  -> (tensor<1x5x6xi2>)
  return %0 : tensor<1x5x6xi2>
}

// -----

// CHECK-LABEL: @matmul
func.func @matmul(%arg0: tensor<1x5x3xi2>, %arg1: tensor<1x3x6xi2>) -> (tensor<1x5x6xi4>) {
  // CHECK: [[C0:%.+]] = arith.constant 0 : i4
  // CHECK: [[INIT:%.+]] = tensor.empty()
  // CHECK: [[FILLED:%.+]] = linalg.fill ins([[C0]] : i4) outs([[INIT]] : tensor<1x5x6xi4>) -> tensor<1x5x6xi4>
  // CHECK: linalg.batch_matmul ins(%arg0, %arg1 : tensor<1x5x3xi2>, tensor<1x3x6xi2>) outs([[FILLED]] : tensor<1x5x6xi4>) -> tensor<1x5x6xi4>
  %0 = "tosa.matmul"(%arg0, %arg1) : (tensor<1x5x3xi2>, tensor<1x3x6xi2>)  -> (tensor<1x5x6xi4>)
  return %0 : tensor<1x5x6xi4>
}

// -----
