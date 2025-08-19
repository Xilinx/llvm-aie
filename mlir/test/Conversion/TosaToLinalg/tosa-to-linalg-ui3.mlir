// RUN: mlir-opt --split-input-file -pass-pipeline="builtin.module(func.func(tosa-to-linalg))" %s | FileCheck %s

func.func @test_cast(%arg0: tensor<1xf32>) -> tensor<1xui3> {
  // CHECK: linalg.generic
  // CHECK: arith.constant 0.000000e+00
  // CHECK: arith.constant 7.000000e+00
  // CHECK: math.roundeven
  // CHECK: arith.minimumf
  // CHECK: arith.maximumf
  // CHECK: arith.fptoui {{.*}} : f32 to i3
  // CHECK: builtin.unrealized_conversion_cast
  %1 = "tosa.cast"(%arg0) : (tensor<1xf32>) -> tensor<1xui3>

  return %1 : tensor<1xui3>
}
