// RUN: mlir-opt -split-input-file -mlir-print-debuginfo -canonicalize="test-convergence" %s | FileCheck %s

// CHECK-LABEL: @clamp_twice_is_single_clamp
func.func @clamp_twice_is_single_clamp(%arg0: tensor<4xi8>) -> tensor<4xi8> {
  // CHECK: tosa.clamp %arg0 {max_fp = 3.000000e+00 : f32, max_int = 2 : i64, min_fp = -3.000000e+00 : f32, min_int = -2 : i64} {{.*}} loc(#[[FUSED:.*]])
  // CHECK-DAG: #[[A:.*]] = loc("Clamp_A")
  // CHECK-DAG: #[[B:.*]] = loc("Clamp_B")
  // CHECK:     #[[FUSED]] = loc(fused[#[[B]], #[[A]]])
  %0 = tosa.clamp %arg0 {max_fp = 3.0 : f32, max_int = 4 : i64, min_fp = -5.0 : f32, min_int = -2 : i64} :  (tensor<4xi8>) -> tensor<4xi8> loc(#loc0)
  %1 = tosa.clamp %0 {max_fp = 5.0 : f32, max_int = 2 : i64, min_fp = -3.0 : f32, min_int = -4 : i64} :  (tensor<4xi8>) -> tensor<4xi8> loc(#loc1)
  return %1 : tensor<4xi8>
}
#loc0 = loc("Clamp_A")
#loc1 = loc("Clamp_B")

// -----

// CHECK-LABEL: @canonicalize_optimize_sqrt_reciprocal
func.func @canonicalize_optimize_sqrt_reciprocal_with_debinfo(%arg0: tensor<1x5x1x1xf32>) -> tensor<1x5x1x1xf32> {
  // CHECK: %[[RSQRT:.*]] = tosa.rsqrt %arg{{.*}} : (tensor<1x5x1x1xf32>) -> tensor<1x5x1x1xf32> loc([[LOC:.*]])
  // CHECK-DAG: #[[A:.*]] = loc("Pow_A")
  // CHECK-DAG: #[[B:.*]] = loc("Reciprocal_B")
  // CHECK-DAG: [[LOC]] = loc(fused[#[[A]], #[[B]]])
  %0 = "tosa.const"() <{value = dense<5.000000e-01> : tensor<1x1x1x1xf32>}> : () -> tensor<1x1x1x1xf32>
  %1 = tosa.pow %arg0, %0 : (tensor<1x5x1x1xf32>, tensor<1x1x1x1xf32>) -> tensor<1x5x1x1xf32> loc(#loc0)
  %2 = tosa.reciprocal %1 : (tensor<1x5x1x1xf32>) -> tensor<1x5x1x1xf32> loc(#loc1)
  return %2 : tensor<1x5x1x1xf32>
}
#loc0 = loc("Pow_A")
#loc1 = loc("Reciprocal_B")

// -----

// CHECK-LABEL: @canonicalize_optimize_sqrt_reciprocal
func.func @canonicalize_optimize_sqrt_reciprocal_bf16(%arg0: tensor<1x5x1x1xbf16>) -> tensor<1x5x1x1xbf16> {
  // CHECK: %[[RSQRT:.*]] = tosa.rsqrt %arg{{.*}} : (tensor<1x5x1x1xbf16>) -> tensor<1x5x1x1xbf16> loc([[LOC:.*]])
  // CHECK-DAG: #[[A:.*]] = loc("Pow_B")
  // CHECK-DAG: #[[B:.*]] = loc("Reciprocal_C")
  // CHECK-DAG: [[LOC]] = loc(fused[#[[A]], #[[B]]])
  %0 = "tosa.const"() <{value = dense<5.000000e-01> : tensor<1x1x1x1xbf16>}> : () -> tensor<1x1x1x1xbf16>
  %1 = tosa.pow %arg0, %0 : (tensor<1x5x1x1xbf16>, tensor<1x1x1x1xbf16>) -> tensor<1x5x1x1xbf16> loc(#loc0)
  %2 = tosa.reciprocal %1 : (tensor<1x5x1x1xbf16>) -> tensor<1x5x1x1xbf16> loc(#loc1)
  return %2 : tensor<1x5x1x1xbf16>
}
#loc0 = loc("Pow_B")
#loc1 = loc("Reciprocal_C")

// -----

// CHECK-LABEL: @reshape_canonicalize_double
func.func @reshape_canonicalize_double(%arg0: tensor<?x10xf32>) -> tensor<?x5xf32> {
  // CHECK: %[[VAL_1:.*]] = tosa.reshape %arg0 {new_shape = array<i64: -1, 5>} {{.*}} loc([[LOC:.*]])
  // CHECK: return %[[VAL_1]]
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 5, -1>}: (tensor<?x10xf32>) -> tensor<5x?xf32> loc(#loc0)
  %1 = tosa.reshape %0 {new_shape = array<i64: -1, 5>}: (tensor<5x?xf32>) -> tensor<?x5xf32> loc(#loc1)
  return %1 : tensor<?x5xf32>
}
#loc0 = loc("reshape1")
#loc1 = loc("reshape2")

// CHECK-DAG: #[[A:.*]] = loc("reshape1")
// CHECK-DAG: #[[B:.*]] = loc("reshape2")
// CHECK-DAG: [[LOC]] = loc(fused[#[[A]], #[[B]]])

// -----

// CHECK-LABEL: @reshape_canonicalize_double_fused_locs
func.func @reshape_canonicalize_double_fused_locs(%arg0: tensor<?x10xf32>) -> tensor<?x5xf32> {
  // CHECK: %[[VAL_1:.*]] = tosa.reshape %arg0 {new_shape = array<i64: -1, 5>} {{.*}} loc([[LOC:.*]])
  // CHECK: return %[[VAL_1]]
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 5, -1>}: (tensor<?x10xf32>) -> tensor<5x?xf32> loc(#fused_loc0)
  %1 = tosa.reshape %0 {new_shape = array<i64: -1, 5>}: (tensor<5x?xf32>) -> tensor<?x5xf32> loc(#fused_loc1)
  return %1 : tensor<?x5xf32>
}
#loc0 = loc("reshape1_1")
#loc1 = loc("reshape1_2")
#loc2 = loc("reshape2_1")
#loc3 = loc("reshape2_2")

// CHECK-DAG: #[[A:.*]] = loc("reshape1_1")
// CHECK-DAG: #[[B:.*]] = loc("reshape1_2")
// CHECK-DAG: #[[C:.*]] = loc("reshape2_1")
// CHECK-DAG: #[[D:.*]] = loc("reshape2_2")
// CHECK-DAG: [[LOC]] = loc(fused[#[[A]], #[[B]], #[[C]], #[[D]]])

#fused_loc0 = loc(fused[#loc0, #loc1])
#fused_loc1 = loc(fused[#loc2, #loc3])