// RUN: mlir-opt --split-input-file -pass-pipeline="builtin.module(func.func(tosa-to-linalg))" %s -verify-diagnostics -o -| FileCheck %s

func.func @test_cast(%arg0: tensor<1xf32>) -> tensor<1xf32> {
  // CHECK: linalg.generic
  // CHECK: math.roundeven
  // CHECK: arith.constant -2.000000e+00
  // CHECK: arith.constant 1.000000e+00
  // CHECK: arith.minimumf
  // CHECK: arith.maximumf
  // CHECK: arith.fptosi
  %1 = "tosa.cast"(%arg0) : (tensor<1xf32>) -> tensor<1xi2>

  // CHECK: linalg.generic
  // CHECK: arith.sitofp
  %2 = "tosa.cast"(%1) : (tensor<1xi2>) -> tensor<1xf32>

  return %2 : tensor<1xf32>
}
