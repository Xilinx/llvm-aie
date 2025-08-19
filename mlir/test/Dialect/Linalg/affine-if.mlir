// RUN: mlir-opt -canonicalize %s | FileCheck %s

// Check that linalg.index does not cause folding of affine.if set to
// a symbolic set.
// This is a deviation from upstream MLIR.
// The origin of this test is that PR:
// https://github.com/Xilinx/llvm-project/pull/537

// CHECK: = affine_set<(d0) : (-d0 + 5 >= 0)>
#set = affine_set<(d0) : (-d0 + 5 >= 0)>

func.func @linalg_index_affine_if(%in: tensor<10xf32>) -> tensor<10xf32> {
  %empty = tensor.empty() : tensor<10xf32>
  %out = linalg.generic {
      indexing_maps = [affine_map<(i) -> (i)>, affine_map<(i) -> (i)>], 
      iterator_types = ["parallel"]}
    ins(%in : tensor<10xf32>)
    outs(%empty : tensor<10xf32>) {
      ^bb0(%a: f32, %b: f32):
        %c42f = arith.constant 42.0 : f32
        %i = linalg.index 0 : index
        %ret = affine.if #set(%i) -> f32 {
          affine.yield %a : f32
        } else {
          affine.yield %c42f : f32
        }
        linalg.yield %ret : f32
    } -> tensor<10xf32>
  return %out : tensor<10xf32>
}
