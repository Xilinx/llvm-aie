// RUN: mlir-opt %s -p 'builtin.module(func.func(linalg-fuse-elementwise-ops{remove-outs-dependency=0}))' -split-input-file | FileCheck %s

#identity = affine_map<(d0) -> (d0)>

func.func @keep_outs_dependency(%arg: tensor<4xf32>) -> tensor<4xf32> {
  // CHECK-NOT: tensor.empty
  %1 = linalg.generic {indexing_maps = [#identity, #identity], iterator_types = ["parallel"] } ins(%arg: tensor<4xf32>) outs(%arg: tensor<4xf32>) {
    ^bb0(%in: f32, %out: f32):
      %exp = arith.negf %in: f32
      linalg.yield %exp : f32
  } -> tensor<4xf32>
  return %1 : tensor<4xf32>
}
