// RUN: mlir-opt -split-input-file -control-flow-sink %s | FileCheck %s

#set = affine_set<(d0) : (-d0 + 3 >= 0)>
#map = affine_map<(d0) -> (d0)>

func.func @test_affine_if_sink(%arg1: tensor<4xf32>) -> tensor<4xf32> {
  %0 = tensor.empty() : tensor<4xf32>
  %1 = linalg.generic {indexing_maps = [#map, #map], iterator_types = ["parallel"]}
  ins(%arg1: tensor<4xf32>) outs(%0: tensor<4xf32>) {
  ^bb0(%in: f32, %out: f32):
    %index = linalg.index 0 : index
    %const0 = arith.constant 0.0 : f32
    %add = arith.addf %in, %in: f32
    %4 = affine.if #set(%index) -> f32 {
      affine.yield %add : f32
    } else {
      affine.yield %const0 : f32
    }
    linalg.yield %4 : f32
  } -> (tensor<4xf32>)
  return %1: tensor<4xf32>
}

// CHECK-LABEL: affine.if 
// CHECK-NEXT:    %[[ADD:.*]] = arith.addf
// CHECK-NEXT:    affine.yield %[[ADD]] : f32
// CHECK-NEXT:  } else {
// CHECK-NEXT:    %[[ZERO:.*]] = arith.constant 0.000000e+00 : f32
// CHECK-NEXT:    affine.yield %[[ZERO]] : f32
// CHECK-NEXT:  }

// -----

#set = affine_set<(d0) : (-d0 + 3 >= 0)>
#map = affine_map<(d0) -> (d0)>

func.func @test_affine_if_sink_with_loop_independenct_code(%arg0: f32, %arg1: tensor<4xf32>) -> tensor<4xf32> {
  %const0 = arith.constant 0.0 : f32
  %const1 = arith.constant 1.0 : f32
  %0 = tensor.empty() : tensor<4xf32>
  %1 = linalg.generic {indexing_maps = [#map, #map], iterator_types = ["parallel"]}
  ins(%arg1: tensor<4xf32>) outs(%0: tensor<4xf32>) {
  ^bb0(%in: f32, %out: f32):
    %index = linalg.index 0 : index
    %4 = affine.if #set(%index) -> f32 {
      affine.yield %const1 : f32
    } else {
      affine.yield %const0 : f32
    }
    linalg.yield %4 : f32
  } -> (tensor<4xf32>)
  return %1: tensor<4xf32>
}

// CHECK-LABEL: affine.if
// CHECK-NEXT:   %[[C1:.*]] = arith.constant 1.0
// CHECK-NEXT:   affine.yield %[[C1]] : f32
// CHECK-NEXT: } else {
// CHECK-NEXT:   %[[C0:.*]] = arith.constant 0.0
// CHECK-NEXT:   affine.yield %[[C0]] : f32
// CHECK-NEXT: }


// -----

func.func private @external(f32) -> ()

#map = affine_map<(d0) -> (d0)>

func.func @affine_if_no_else(%arg0: f32, %arg1: tensor<4xf32>) -> tensor<4xf32> {
  %const1 = arith.constant 1.0 : f32
  %0 = tensor.empty() : tensor<4xf32>
  %1 = linalg.generic {indexing_maps = [#map, #map], iterator_types = ["parallel"]}
  ins(%arg1: tensor<4xf32>) outs(%0: tensor<4xf32>) {
  ^bb0(%in: f32, %out: f32):
    %index = linalg.index 0 : index
    affine.if affine_set<(d0) : (-d0 + 3 >= 0)>(%index) {
        func.call @external(%const1) : (f32) -> ()
    }
    linalg.yield %in : f32
  } -> (tensor<4xf32>)
  return %1: tensor<4xf32>
}

// CHECK-LABEL: affine.if
// CHECK-NEXT:   %[[C1:.*]] = arith.constant 1.0
// CHECK-NEXT:   func.call @external(%[[C1]]) : (f32) -> ()
// CHECK-NEXT: }
