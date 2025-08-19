// RUN: mlir-opt %s -transform-interpreter -split-input-file -debug-only=tile-using-interface --mlir-disable-threading 2>&1 | FileCheck %s

func.func @tile_order_ceil_then_negf(%arg: tensor<256xf32>) -> tensor<256xf32> {
  // Ops are tiled by lower priority: linalg.powf, linalg.ceil (1st operand of powf, priority = 0),
  // linalg.negf (2nd operand of powf, priority = 1), linalg.ceil (operand of negf, priority = 0)
  %empty = tensor.empty() : tensor<256xf32>
  %0 = linalg.ceil {tiling_priority = 0 : i64} ins(%arg: tensor<256xf32>) outs(%empty: tensor<256xf32>) -> tensor<256xf32>
  %empty1 = tensor.empty() : tensor<256xf32>
  %1 = linalg.negf {tiling_priority = 1 : i64} ins(%0 : tensor<256xf32>) outs(%empty1: tensor<256xf32>) -> tensor<256xf32>
  %empty2 = tensor.empty() : tensor<256xf32>
  %2 = linalg.powf {tile} ins(%0, %1: tensor<256xf32>, tensor<256xf32>) outs(%empty2: tensor<256xf32>) -> tensor<256xf32>

  // The order of these checks is the order in which the ops are actually tiled.
  // CHECK: worklist: producer is %{{.*}} = linalg.ceil
  // CHECK: worklist: producer is %{{.*}} = linalg.negf
  // CHECK: worklist: producer is %{{.*}} = linalg.ceil

  return %2 : tensor<256xf32>
}

module attributes {transform.with_named_sequence} {
  transform.named_sequence @__transform_main(%arg1: !transform.any_op {transform.readonly}) {
    %0 = transform.structured.match ops{["linalg.powf"]} attributes {"tile"} in %arg1 : (!transform.any_op) -> !transform.any_op
    %1, %loops = transform.test.tile_fuse_ordered %0 [32] : (!transform.any_op) -> (!transform.any_op, !transform.any_op)
    transform.yield
  }
}

// -----

func.func @tile_negf_then_ceil(%arg: tensor<256xf32>) -> tensor<256xf32> {
  // Ops are tiled by lower priority: linalg.powf, linalg.negf (2nd operand of powf, priority = 0),
  // linalg.ceil (1st oprand of powf, priority = 1), linalg.ceil (operand of negf, priority = 1)
  %empty = tensor.empty() : tensor<256xf32>
  %0 = linalg.ceil {tiling_priority = 1 : i64} ins(%arg: tensor<256xf32>) outs(%empty: tensor<256xf32>) -> tensor<256xf32>
  %empty1 = tensor.empty() : tensor<256xf32>
  %1 = linalg.negf {tiling_priority = 0 : i64} ins(%0 : tensor<256xf32>) outs(%empty1: tensor<256xf32>) -> tensor<256xf32>
  %empty2 = tensor.empty() : tensor<256xf32>
  %2 = linalg.powf {tile} ins(%0, %1: tensor<256xf32>, tensor<256xf32>) outs(%empty2: tensor<256xf32>) -> tensor<256xf32>

  // CHECK: worklist: producer is %{{.*}} = linalg.negf
  // CHECK: worklist: producer is %{{.*}} = linalg.ceil
  // CHECK: worklist: producer is %{{.*}} = linalg.ceil

  return %2 : tensor<256xf32>
}

module attributes {transform.with_named_sequence} {
  transform.named_sequence @__transform_main(%arg1: !transform.any_op {transform.readonly}) {
    %0 = transform.structured.match ops{["linalg.powf"]} attributes {"tile"} in %arg1 : (!transform.any_op) -> !transform.any_op
    %1, %loops = transform.test.tile_fuse_ordered %0 [32] : (!transform.any_op) -> (!transform.any_op, !transform.any_op)
    transform.yield
  }
}

// -----

func.func @tile_negf_then_ceil_swap_in_powf(%arg: tensor<256xf32>) -> tensor<256xf32> {
  // This gives the same tiling order as above regardless of the operand order in the powf
  // linalg.powf, linalg.negf (1st operand of powf, priority = 0),
  // linalg.ceil (2nd oprand of powf, priority = 1), linalg.ceil (operand of negf, priority = 1)
  %empty = tensor.empty() : tensor<256xf32>
  %0 = linalg.ceil {tiling_priority = 1 : i64} ins(%arg: tensor<256xf32>) outs(%empty: tensor<256xf32>) -> tensor<256xf32>
  %empty1 = tensor.empty() : tensor<256xf32>
  %1 = linalg.negf {tiling_priority = 0 : i64} ins(%0 : tensor<256xf32>) outs(%empty1: tensor<256xf32>) -> tensor<256xf32>
  %empty2 = tensor.empty() : tensor<256xf32>
  %2 = linalg.powf {tile} ins(%1, %0: tensor<256xf32>, tensor<256xf32>) outs(%empty2: tensor<256xf32>) -> tensor<256xf32>

  // CHECK: worklist: producer is %{{.*}} = linalg.negf
  // CHECK: worklist: producer is %{{.*}} = linalg.ceil
  // CHECK: worklist: producer is %{{.*}} = linalg.ceil

  return %2 : tensor<256xf32>
}

module attributes {transform.with_named_sequence} {
  transform.named_sequence @__transform_main(%arg1: !transform.any_op {transform.readonly}) {
    %0 = transform.structured.match ops{["linalg.powf"]} attributes {"tile"} in %arg1 : (!transform.any_op) -> !transform.any_op
    %1, %loops = transform.test.tile_fuse_ordered %0 [32] : (!transform.any_op) -> (!transform.any_op, !transform.any_op)
    transform.yield
  }
}

