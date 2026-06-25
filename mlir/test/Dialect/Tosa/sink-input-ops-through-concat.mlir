// Modifications (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
// RUN: mlir-opt --split-input-file --sink-input-ops-through-concat %s | FileCheck %s

!in_type = tensor<1x8x8xf32>
!out_type = tensor<3x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.add %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %1 = tosa.add %arg1, %arg0 : (!in_type, !in_type) -> !in_type
  %2 = tosa.add %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %3 = tosa.concat %0, %1, %2 {axis = 0 : i32} : (!in_type, !in_type, !in_type) -> !out_type
  return %3 : !out_type
}

// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<3x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.add [[VAR_0_]], [[VAR_1_]] : (tensor<3x8x8xf32>, tensor<3x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK:           return [[VAR_2_]] : tensor<3x8x8xf32>
// CHECK:        }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat_no_op(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.concat %arg0, %arg1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %0 : !out_type
}

// CHECK-LABEL:  func.func @switch_op_concat_no_op
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_0_]] : tensor<2x8x8xf32>
// CHECK:        }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat_not_same(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.add %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %1 = tosa.abs  %arg0 : (!in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}

// CHECK-LABEL:  func.func @switch_op_concat_not_same
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.add [[PARAM_0_]], [[PARAM_1_]] : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<1x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.abs [[PARAM_0_]] : (tensor<1x8x8xf32>) -> tensor<1x8x8xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[VAR_0_]], [[VAR_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_2_]] : tensor<2x8x8xf32>
// CHECK:        }
// -----

!in_type = tensor<1x8x8xi32>
!out_type = tensor<2x8x8xi32>
func.func @switch_op_concat_without_input() -> !out_type {
  %0 = "tosa.const"() {value = dense<0> : !in_type} : () -> !in_type
  %1 = "tosa.const"() {value = dense<2> : !in_type} : () -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}

// CHECK-LABEL:  func.func @switch_op_concat_without_input
// CHECK-SAME:   () -> tensor<2x8x8xi32> {
// CHECK:           [[VAR_0_:%.+]] = "tosa.const"() <{value = dense<0> : tensor<1x8x8xi32>}> : () -> tensor<1x8x8xi32>
// CHECK:           [[VAR_1_:%.+]] = "tosa.const"() <{value = dense<2> : tensor<1x8x8xi32>}> : () -> tensor<1x8x8xi32>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[VAR_0_]], [[VAR_1_]] {axis = 0 : i32} : (tensor<1x8x8xi32>, tensor<1x8x8xi32>) -> tensor<2x8x8xi32>
// CHECK:           return [[VAR_2_]] : tensor<2x8x8xi32>
// CHECK:        }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat_mergable_broadcasting(%arg0: !in_type, %arg1: !in_type, %broadcast: tensor<1x1x8xf32>) -> !out_type {
  %0 = tosa.add %arg0, %broadcast: (!in_type, tensor<1x1x8xf32>) -> !in_type
  %1 = tosa.add %arg1, %broadcast: (!in_type, tensor<1x1x8xf32>) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}

// CHECK-LABEL:  func.func @switch_op_concat_mergable_broadcasting
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>, [[PARAM_2_:%.+]]: tensor<1x1x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.concat [[PARAM_2_]], [[PARAM_2_]] {axis = 0 : i32} : (tensor<1x1x8xf32>, tensor<1x1x8xf32>) -> tensor<2x1x8xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.add [[VAR_0_]], [[VAR_1_]] : (tensor<2x8x8xf32>, tensor<2x1x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_2_]] : tensor<2x8x8xf32>
// CHECK:        }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<1x16x8xf32>
func.func @switch_op_concat_unmergable_broadcasting1(%arg0: !in_type, %arg1: !in_type, %broadcast: tensor<1x1x8xf32>) -> !out_type {
  %0 = tosa.add %arg0, %broadcast: (!in_type, tensor<1x1x8xf32>) -> !in_type
  %1 = tosa.add %arg1, %broadcast: (!in_type, tensor<1x1x8xf32>) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 1 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}

// CHECK-LABEL:  func.func @switch_op_concat_unmergable_broadcasting1
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>, [[PARAM_2_:%.+]]: tensor<1x1x8xf32>) -> tensor<1x16x8xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.add [[PARAM_0_]], [[PARAM_2_]] : (tensor<1x8x8xf32>, tensor<1x1x8xf32>) -> tensor<1x8x8xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.add [[PARAM_1_]], [[PARAM_2_]] : (tensor<1x8x8xf32>, tensor<1x1x8xf32>) -> tensor<1x8x8xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[VAR_0_]], [[VAR_1_]] {axis = 1 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<1x16x8xf32>
// CHECK:           return [[VAR_2_]] : tensor<1x16x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat_unmergable_broadcasting2(%arg0: !in_type, %arg1: !in_type, %broadcast: tensor<1x1x8xf32>) -> !out_type {
  %0 = tosa.add %arg0, %broadcast: (!in_type, tensor<1x1x8xf32>) -> !in_type
  %1 = tosa.add %arg1, %arg1: (!in_type, !in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}

// CHECK-LABEL:  func.func @switch_op_concat_unmergable_broadcasting2
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>, [[PARAM_2_:%.+]]: tensor<1x1x8xf32>) -> tensor<2x8x8xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.add [[PARAM_0_]], [[PARAM_2_]] : (tensor<1x8x8xf32>, tensor<1x1x8xf32>) -> tensor<1x8x8xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.add [[PARAM_1_]], [[PARAM_1_]] : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<1x8x8xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[VAR_0_]], [[VAR_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_2_]] : tensor<2x8x8xf32>
// CHECK:         }
// -----

!in_type1 = tensor<1x8x3xf32>
!in_type2 = tensor<1x3x6xf32>
!out_type = tensor<1x8x6xf32>
!concat_type = tensor<2x8x6xf32>
func.func @switch_op_concat_matmul1(%arg0: !in_type1, %arg1: !in_type2, %arg2: !in_type2) -> !concat_type {
  %0 = tosa.matmul %arg0, %arg1: (!in_type1, !in_type2) -> !out_type
  %1 = tosa.matmul %arg0, %arg2: (!in_type1, !in_type2) -> !out_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!out_type, !out_type) -> !concat_type
  return %2 : !concat_type
}

// CHECK-LABEL:  func.func @switch_op_concat_matmul1
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x3xf32>, [[PARAM_1_:%.+]]: tensor<1x3x6xf32>, [[PARAM_2_:%.+]]: tensor<1x3x6xf32>) -> tensor<2x8x6xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x3xf32>, tensor<1x8x3xf32>) -> tensor<2x8x3xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_2_]] {axis = 0 : i32} : (tensor<1x3x6xf32>, tensor<1x3x6xf32>) -> tensor<2x3x6xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.matmul [[VAR_0_]], [[VAR_1_]] : (tensor<2x8x3xf32>, tensor<2x3x6xf32>) -> tensor<2x8x6xf32>
// CHECK:           return [[VAR_2_]] : tensor<2x8x6xf32>
// CHECK:         }
// -----

!in_type1 = tensor<1x8x3xf32>
!in_type2 = tensor<1x3x6xf32>
!out_type = tensor<1x8x6xf32>
!concat_type = tensor<1x16x6xf32>
func.func @switch_op_concat_matmul2(%arg0: !in_type1, %arg1: !in_type2, %arg2: !in_type2) -> !concat_type {
  %0 = tosa.matmul %arg0, %arg1: (!in_type1, !in_type2) -> !out_type
  %1 = tosa.matmul %arg0, %arg2: (!in_type1, !in_type2) -> !out_type
  %2 = tosa.concat %0, %1 {axis = 1 : i32} : (!out_type, !out_type) -> !concat_type
  return %2 : !concat_type
}

// CHECK-LABEL:  func.func @switch_op_concat_matmul2
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x3xf32>, [[PARAM_1_:%.+]]: tensor<1x3x6xf32>, [[PARAM_2_:%.+]]: tensor<1x3x6xf32>) -> tensor<1x16x6xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.matmul [[PARAM_0_]], [[PARAM_1_]] : (tensor<1x8x3xf32>, tensor<1x3x6xf32>) -> tensor<1x8x6xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.matmul [[PARAM_0_]], [[PARAM_2_]] : (tensor<1x8x3xf32>, tensor<1x3x6xf32>) -> tensor<1x8x6xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[VAR_0_]], [[VAR_1_]] {axis = 1 : i32} : (tensor<1x8x6xf32>, tensor<1x8x6xf32>) -> tensor<1x16x6xf32>
// CHECK:           return [[VAR_2_]] : tensor<1x16x6xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat_attr(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.clamp %arg0 {min_fp = 0.0 : f32, max_fp = 1.0 : f32, min_int = 0 : i64, max_int = 1 : i64} : (!in_type) -> !in_type
  %1 = tosa.clamp %arg1 {min_fp = 0.0 : f32, max_fp = 1.0 : f32, min_int = 0 : i64, max_int = 1 : i64} : (!in_type) -> !in_type
  %3 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %3 : !out_type
}

// CHECK-LABEL:  func.func @switch_op_concat_attr
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.clamp [[VAR_0_]] {max_fp = 1.000000e+00 : f32, max_int = 1 : i64, min_fp = 0.000000e+00 : f32, min_int = 0 : i64} : (tensor<2x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat_attr_mismatch(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.clamp %arg0 {min_fp = 0.0 : f32, max_fp = 2.0 : f32, min_int = 0 : i64, max_int = 1 : i64} : (!in_type) -> !in_type
  %1 = tosa.clamp %arg1 {min_fp = 0.0 : f32, max_fp = 1.0 : f32, min_int = 0 : i64, max_int = 1 : i64} : (!in_type) -> !in_type
  %3 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %3 : !out_type
}

// CHECK-LABEL:  func.func @switch_op_concat_attr_mismatch
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.clamp [[PARAM_0_]] {max_fp = 2.000000e+00 : f32, max_int = 1 : i64, min_fp = 0.000000e+00 : f32, min_int = 0 : i64} : (tensor<1x8x8xf32>) -> tensor<1x8x8xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.clamp [[PARAM_1_]] {max_fp = 1.000000e+00 : f32, max_int = 1 : i64, min_fp = 0.000000e+00 : f32, min_int = 0 : i64} : (tensor<1x8x8xf32>) -> tensor<1x8x8xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[VAR_0_]], [[VAR_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_2_]] : tensor<2x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<1x1x8xf32>
!concat_type = tensor<2x1x8xf32>
func.func @switch_op_concat_axis(%arg0: !in_type, %arg1: !in_type) -> !concat_type {
  %0 = tosa.reduce_max %arg0 {axis = 1 : i32} : (!in_type) -> !out_type
  %1 = tosa.reduce_max %arg1 {axis = 1 : i32} : (!in_type) -> !out_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!out_type, !out_type) -> !concat_type
  return %2 : !concat_type
}

// CHECK-LABEL:  func.func @switch_op_concat_axis
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x1x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.reduce_max [[VAR_0_]] {axis = 1 : i32} : (tensor<2x8x8xf32>) -> tensor<2x1x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x1x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<1x1x8xf32>
!concat_type = tensor<1x2x8xf32>
func.func @switch_op_concat_axis_mismatch(%arg0: !in_type, %arg1: !in_type) -> !concat_type {
  %0 = tosa.reduce_max %arg0 {axis = 1 : i32} : (!in_type) -> !out_type
  %1 = tosa.reduce_max %arg1 {axis = 1 : i32} : (!in_type) -> !out_type
  %2 = tosa.concat %0, %1 {axis = 1 : i32} : (!out_type, !out_type) -> !concat_type
  return %2 : !concat_type
}

// CHECK-LABEL:  func.func @switch_op_concat_axis_mismatch
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<1x2x8xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.reduce_max [[PARAM_0_]] {axis = 1 : i32} : (tensor<1x8x8xf32>) -> tensor<1x1x8xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.reduce_max [[PARAM_1_]] {axis = 1 : i32} : (tensor<1x8x8xf32>) -> tensor<1x1x8xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[VAR_0_]], [[VAR_1_]] {axis = 1 : i32} : (tensor<1x1x8xf32>, tensor<1x1x8xf32>) -> tensor<1x2x8xf32>
// CHECK:           return [[VAR_2_]] : tensor<1x2x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x1x5xf32>
!re_type = tensor<1x1x1x5xf32>
!out_type = tensor<2x1x1x5xf32>
func.func @reshape_other_axis(%arg0: !in_type, %arg1: !in_type) -> !out_type {
    %0 = tosa.reshape %arg0 {new_shape = array<i64: 1, 1, 1, 5>} : (!in_type) -> !re_type
    %1 = tosa.reshape %arg1 {new_shape = array<i64: 1, 1, 1, 5>} : (!in_type) -> !re_type
    %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!re_type, !re_type) -> !out_type
  return %2 : !out_type
}

// CHECK-LABEL:  func.func @reshape_other_axis
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x1x5xf32>, [[PARAM_1_:%.+]]: tensor<1x1x5xf32>) -> tensor<2x1x1x5xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x1x5xf32>, tensor<1x1x5xf32>) -> tensor<2x1x5xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.reshape [[VAR_0_]] {new_shape = array<i64: 2, 1, 1, 5>} : (tensor<2x1x5xf32>) -> tensor<2x1x1x5xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x1x1x5xf32>
// CHECK:         }
// -----

!in_type = tensor<6x1x6xf32>
!out_type = tensor<2x3x2x2x3xf32>
!re_type = tensor<2x3x1x2x3xf32>
func.func @reshape_with_product(%arg0: !in_type, %arg1: !in_type) -> !out_type {
    %0 = tosa.reshape %arg0 {new_shape = array<i64: 2, 3, 1, 2, 3>} : (!in_type) -> !re_type
    %1 = tosa.reshape %arg1 {new_shape = array<i64: 2, 3, 1, 2, 3>} : (!in_type) -> !re_type
    %2 = tosa.concat %0, %1 {axis = 2 : i32} : (!re_type, !re_type) -> !out_type
  return %2 : !out_type
}

// CHECK-LABEL:  func.func @reshape_with_product
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<6x1x6xf32>, [[PARAM_1_:%.+]]: tensor<6x1x6xf32>) -> tensor<2x3x2x2x3xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 1 : i32} : (tensor<6x1x6xf32>, tensor<6x1x6xf32>) -> tensor<6x2x6xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.reshape [[VAR_0_]] {new_shape = array<i64: 2, 3, 2, 2, 3>} : (tensor<6x2x6xf32>) -> tensor<2x3x2x2x3xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x3x2x2x3xf32>
// CHECK:         }
// -----

!in_type = tensor<1x4x1xf32>
!out_type = tensor<1x8xf32>
!re_type = tensor<1x4xf32>
func.func @reshape_big_concat_axis(%arg0: !in_type, %arg1: !in_type) -> !out_type {
    %0 = tosa.reshape %arg0 {new_shape = array<i64: 1, 4>} : (!in_type) -> !re_type
    %1 = tosa.reshape %arg1 {new_shape = array<i64: 1, 4>} : (!in_type) -> !re_type
    %2 = tosa.concat %0, %1 {axis = 1 : i32} : (!re_type, !re_type) -> !out_type
  return %2 : !out_type
}

// CHECK-LABEL:  func.func @reshape_big_concat_axis
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x4x1xf32>, [[PARAM_1_:%.+]]: tensor<1x4x1xf32>) -> tensor<1x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x4x1xf32>, tensor<1x4x1xf32>) -> tensor<2x4x1xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.reshape [[VAR_0_]] {new_shape = array<i64: 1, 8>} : (tensor<2x4x1xf32>) -> tensor<1x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<1x8xf32>
// CHECK:         }
// -----

!in_type = tensor<2xf32>
!re_type = tensor<1x2xf32>
!out_type = tensor<2x2xf32>
func.func @reshape_unit_dim_boundary_alignment(%arg0: !in_type, %arg1: !in_type) -> !out_type {
    %0 = tosa.reshape %arg0 {new_shape = array<i64: 1, 2>} : (!in_type) -> !re_type
    %1 = tosa.reshape %arg1 {new_shape = array<i64: 1, 2>} : (!in_type) -> !re_type
    %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!re_type, !re_type) -> !out_type
  return %2 : !out_type
}

// CHECK-LABEL:  func.func @reshape_unit_dim_boundary_alignment
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<2xf32>, [[PARAM_1_:%.+]]: tensor<2xf32>) -> tensor<2x2xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<2xf32>, tensor<2xf32>) -> tensor<4xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.reshape [[VAR_0_]] {new_shape = array<i64: 2, 2>} : (tensor<4xf32>) -> tensor<2x2xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x2xf32>
// CHECK:         }
// -----

!in_type0 = tensor<1x2001x1x1xi8>
!in_type1 = tensor<1x1x1x1xi8>
!re_type0 = tensor<1x2001xi8>
!re_type1 = tensor<1x1xi8>
!out_type = tensor<1x2002xi8>
func.func @reshape_heterogeneous_shapes(%arg0: !in_type0, %arg1: !in_type1) -> !out_type {
    %0 = tosa.reshape %arg0 {new_shape = array<i64: 1, 2001>} : (!in_type0) -> !re_type0
    %1 = tosa.reshape %arg1 {new_shape = array<i64: 1, 1>} : (!in_type1) -> !re_type1
    %2 = tosa.concat %0, %1 {axis = 1 : i32} : (!re_type0, !re_type1) -> !out_type
  return %2 : !out_type
}

// CHECK-LABEL:  func.func @reshape_heterogeneous_shapes
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x2001x1x1xi8>, [[PARAM_1_:%.+]]: tensor<1x1x1x1xi8>) -> tensor<1x2002xi8> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 1 : i32} : (tensor<1x2001x1x1xi8>, tensor<1x1x1x1xi8>) -> tensor<1x2002x1x1xi8>
// CHECK:           [[VAR_1_:%.+]] = tosa.reshape [[VAR_0_]] {new_shape = array<i64: 1, 2002>} : (tensor<1x2002x1x1xi8>) -> tensor<1x2002xi8>
// CHECK:           return [[VAR_1_]] : tensor<1x2002xi8>
// CHECK:         }
// -----

!in_type0 = tensor<1x2x3xf32>
!in_type1 = tensor<2x3x1xf32>
!re_type = tensor<2x3xf32>
!out_type = tensor<2x6xf32>
func.func @reshape_fail_no_common_axis(%arg0: !in_type0, %arg1: !in_type1) -> !out_type {
    %0 = tosa.reshape %arg0 {new_shape = array<i64: 2, 3>} : (!in_type0) -> !re_type
    %1 = tosa.reshape %arg1 {new_shape = array<i64: 2, 3>} : (!in_type1) -> !re_type
    %2 = tosa.concat %0, %1 {axis = 1 : i32} : (!re_type, !re_type) -> !out_type
  return %2 : !out_type
}

// CHECK-LABEL:  func.func @reshape_fail_no_common_axis
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x2x3xf32>, [[PARAM_1_:%.+]]: tensor<2x3x1xf32>) -> tensor<2x6xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.reshape [[PARAM_0_]] {new_shape = array<i64: 2, 3>} : (tensor<1x2x3xf32>) -> tensor<2x3xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.reshape [[PARAM_1_]] {new_shape = array<i64: 2, 3>} : (tensor<2x3x1xf32>) -> tensor<2x3xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[VAR_0_]], [[VAR_1_]] {axis = 1 : i32} : (tensor<2x3xf32>, tensor<2x3xf32>) -> tensor<2x6xf32>
// CHECK:           return [[VAR_2_]] : tensor<2x6xf32>
// CHECK:         }
// -----

!in_type0 = tensor<2x3x4xf32>
!in_type1 = tensor<2x4x3xf32>
!re_type = tensor<2x12xf32>
!out_type = tensor<4x12xf32>
func.func @reshape_fail_incompatible_raw_inputs(%arg0: !in_type0, %arg1: !in_type1) -> !out_type {
    %0 = tosa.reshape %arg0 {new_shape = array<i64: 2, 12>} : (!in_type0) -> !re_type
    %1 = tosa.reshape %arg1 {new_shape = array<i64: 2, 12>} : (!in_type1) -> !re_type
    %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!re_type, !re_type) -> !out_type
  return %2 : !out_type
}

// CHECK-LABEL:  func.func @reshape_fail_incompatible_raw_inputs
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<2x3x4xf32>, [[PARAM_1_:%.+]]: tensor<2x4x3xf32>) -> tensor<4x12xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.reshape [[PARAM_0_]] {new_shape = array<i64: 2, 12>} : (tensor<2x3x4xf32>) -> tensor<2x12xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.reshape [[PARAM_1_]] {new_shape = array<i64: 2, 12>} : (tensor<2x4x3xf32>) -> tensor<2x12xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[VAR_0_]], [[VAR_1_]] {axis = 0 : i32} : (tensor<2x12xf32>, tensor<2x12xf32>) -> tensor<4x12xf32>
// CHECK:           return [[VAR_2_]] : tensor<4x12xf32>
// CHECK:         }
// -----

!in_type = tensor<6x1x6xf32>
!out_type = tensor<2x6x1x2x3xf32>
!re_type = tensor<2x3x1x2x3xf32>
func.func @reshape_fail1(%arg0: !in_type, %arg1: !in_type) -> !out_type {
    %0 = tosa.reshape %arg0 {new_shape = array<i64: 2, 3, 1, 2, 3>} : (!in_type) -> !re_type
    %1 = tosa.reshape %arg1 {new_shape = array<i64: 2, 3, 1, 2, 3>} : (!in_type) -> !re_type
    %2 = tosa.concat %0, %1 {axis = 1 : i32} : (!re_type, !re_type) -> !out_type
  return %2 : !out_type
}

// CHECK-LABEL:  func.func @reshape_fail1
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<6x1x6xf32>, [[PARAM_1_:%.+]]: tensor<6x1x6xf32>) -> tensor<2x6x1x2x3xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.reshape [[PARAM_0_]] {new_shape = array<i64: 2, 3, 1, 2, 3>} : (tensor<6x1x6xf32>) -> tensor<2x3x1x2x3xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.reshape [[PARAM_1_]] {new_shape = array<i64: 2, 3, 1, 2, 3>} : (tensor<6x1x6xf32>) -> tensor<2x3x1x2x3xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[VAR_0_]], [[VAR_1_]] {axis = 1 : i32} : (tensor<2x3x1x2x3xf32>, tensor<2x3x1x2x3xf32>) -> tensor<2x6x1x2x3xf32>
// CHECK:           return [[VAR_2_]] : tensor<2x6x1x2x3xf32>
// CHECK:         }
// -----

!in_type = tensor<1x6xf32>
!out_type = tensor<6x2xf32>
!re_type = tensor<6x1xf32>
func.func @reshape_fail2(%arg0: !in_type, %arg1: !in_type) -> !out_type {
    %0 = tosa.reshape %arg0 {new_shape = array<i64: 6, 1>} : (!in_type) -> !re_type
    %1 = tosa.reshape %arg1 {new_shape = array<i64: 6, 1>} : (!in_type) -> !re_type
    %2 = tosa.concat %0, %1 {axis = 1 : i32} : (!re_type, !re_type) -> !out_type
  return %2 : !out_type
}

// CHECK-LABEL:  func.func @reshape_fail2
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x6xf32>, [[PARAM_1_:%.+]]: tensor<1x6xf32>) -> tensor<6x2xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.reshape [[PARAM_0_]] {new_shape = array<i64: 6, 1>} : (tensor<1x6xf32>) -> tensor<6x1xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.reshape [[PARAM_1_]] {new_shape = array<i64: 6, 1>} : (tensor<1x6xf32>) -> tensor<6x1xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[VAR_0_]], [[VAR_1_]] {axis = 1 : i32} : (tensor<6x1xf32>, tensor<6x1xf32>) -> tensor<6x2xf32>
// CHECK:           return [[VAR_2_]] : tensor<6x2xf32>
// CHECK:         }
// -----

!in_type1 = tensor<1x6xf32>
!in_type2 = tensor<2x3xf32>
!out_type = tensor<2x1x6xf32>
!re_type = tensor<1x1x6xf32>
func.func @reshape_fail3(%arg0: !in_type1, %arg1: !in_type2) -> !out_type {
    %0 = tosa.reshape %arg0 {new_shape = array<i64: 1, 1, 6>} : (!in_type1) -> !re_type
    %1 = tosa.reshape %arg1 {new_shape = array<i64: 1, 1, 6>} : (!in_type2) -> !re_type
    %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!re_type, !re_type) -> !out_type
  return %2 : !out_type
}

// CHECK-LABEL:  func.func @reshape_fail3
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x6xf32>, [[PARAM_1_:%.+]]: tensor<2x3xf32>) -> tensor<2x1x6xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.reshape [[PARAM_0_]] {new_shape = array<i64: 1, 1, 6>} : (tensor<1x6xf32>) -> tensor<1x1x6xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.reshape [[PARAM_1_]] {new_shape = array<i64: 1, 1, 6>} : (tensor<2x3xf32>) -> tensor<1x1x6xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[VAR_0_]], [[VAR_1_]] {axis = 0 : i32} : (tensor<1x1x6xf32>, tensor<1x1x6xf32>) -> tensor<2x1x6xf32>
// CHECK:           return [[VAR_2_]] : tensor<2x1x6xf32>
// CHECK:         }
// -----

!in_type = tensor<f32>
!out_type = tensor<2xf32>
!re_type = tensor<1xf32>
func.func @reshape_tensor0(%arg0: !in_type, %arg1: !in_type) -> !out_type {
    %0 = tosa.reshape %arg0 {new_shape = array<i64: 1>} : (!in_type) -> !re_type
    %1 = tosa.reshape %arg1 {new_shape = array<i64: 1>} : (!in_type) -> !re_type
    %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!re_type, !re_type) -> !out_type
  return %2 : !out_type
}

// CHECK-LABEL:  func.func @reshape_tensor0
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<f32>, [[PARAM_1_:%.+]]: tensor<f32>) -> tensor<2xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.reshape [[PARAM_0_]] {new_shape = array<i64: 1>} : (tensor<f32>) -> tensor<1xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.reshape [[PARAM_1_]] {new_shape = array<i64: 1>} : (tensor<f32>) -> tensor<1xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[VAR_0_]], [[VAR_1_]] {axis = 0 : i32} : (tensor<1xf32>, tensor<1xf32>) -> tensor<2xf32>
// CHECK:           return [[VAR_2_]] : tensor<2xf32>
// CHECK:         }
// -----

// This excerpt is a transformation of two convolutions with a kernel size of 1x1 outputting into the same concat on the kernel axis.
// They were transformed to a matmul to be supported by concat sinking.

func.func @reshape_complex_match(%arg0: tensor<1x42x1x1xbf16>, %arg1: tensor<1x42x1x1xbf16>, %arg2: tensor<12x42x1x1xbf16>, %arg3: tensor<12x42x1x1xbf16>) -> tensor<1x2x1x12xbf16> {
    %0 = "tosa.const"() <{value = dense<[0, 2, 1]> : tensor<3xi32>}> : () -> tensor<3xi32>
    %1 = "tosa.const"() <{value = dense<[0, 1, 3, 2, 4, 5]> : tensor<6xi32>}> : () -> tensor<6xi32>
    %2 = "tosa.const"() <{value = dense<[0, 3, 1, 2]> : tensor<4xi32>}> : () -> tensor<4xi32>
    %3 = tosa.reshape %arg0 {new_shape = array<i64: 1, 1, 1, 1, 1, 42>} : (tensor<1x42x1x1xbf16>) -> tensor<1x1x1x1x1x42xbf16>
    %4 = tosa.transpose %3, %1 : (tensor<1x1x1x1x1x42xbf16>, tensor<6xi32>) -> tensor<1x1x1x1x1x42xbf16>
    %5 = tosa.reshape %4 {new_shape = array<i64: 1, 1, 42>} : (tensor<1x1x1x1x1x42xbf16>) -> tensor<1x1x42xbf16>
    %6 = tosa.reshape %arg2 {new_shape = array<i64: 1, 12, 42>} : (tensor<12x42x1x1xbf16>) -> tensor<1x12x42xbf16>
    %7 = tosa.transpose %6, %0 : (tensor<1x12x42xbf16>, tensor<3xi32>) -> tensor<1x42x12xbf16>
    %8 = tosa.matmul %5, %7 : (tensor<1x1x42xbf16>, tensor<1x42x12xbf16>) -> tensor<1x1x12xbf16>
    %9 = tosa.reshape %8 {new_shape = array<i64: 1, 1, 1, 12>} : (tensor<1x1x12xbf16>) -> tensor<1x1x1x12xbf16>
    %10 = tosa.reshape %arg1 {new_shape = array<i64: 1, 1, 1, 1, 1, 42>} : (tensor<1x42x1x1xbf16>) -> tensor<1x1x1x1x1x42xbf16>
    %11 = tosa.transpose %10, %1 : (tensor<1x1x1x1x1x42xbf16>, tensor<6xi32>) -> tensor<1x1x1x1x1x42xbf16>
    %12 = tosa.reshape %11 {new_shape = array<i64: 1, 1, 42>} : (tensor<1x1x1x1x1x42xbf16>) -> tensor<1x1x42xbf16>
    %13 = tosa.reshape %arg3 {new_shape = array<i64: 1, 12, 42>} : (tensor<12x42x1x1xbf16>) -> tensor<1x12x42xbf16>
    %14 = tosa.transpose %13, %0 : (tensor<1x12x42xbf16>, tensor<3xi32>) -> tensor<1x42x12xbf16>
    %15 = tosa.matmul %12, %14 : (tensor<1x1x42xbf16>, tensor<1x42x12xbf16>) -> tensor<1x1x12xbf16>
    %16 = tosa.reshape %15 {new_shape = array<i64: 1, 1, 1, 12>} : (tensor<1x1x12xbf16>) -> tensor<1x1x1x12xbf16>
    %17 = tosa.concat %9, %16 {axis = 1 : i32} : (tensor<1x1x1x12xbf16>, tensor<1x1x1x12xbf16>) -> tensor<1x2x1x12xbf16>
  return %17 : tensor<1x2x1x12xbf16>
}

// CHECK-LABEL:  func.func @reshape_complex_match
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x42x1x1xbf16>, [[PARAM_1_:%.+]]: tensor<1x42x1x1xbf16>, [[PARAM_2_:%.+]]: tensor<12x42x1x1xbf16>, [[PARAM_3_:%.+]]: tensor<12x42x1x1xbf16>) -> tensor<1x2x1x12xbf16> {
// CHECK-DAG:       [[VAR_0_:%.+]] = "tosa.const"() <{value = dense<[0, 2, 1]> : tensor<3xi32>}> : () -> tensor<3xi32>
// CHECK-DAG:       [[VAR_1_:%.+]] = "tosa.const"() <{value = dense<[0, 1, 3, 2, 4, 5]> : tensor<6xi32>}> : () -> tensor<6xi32>
// CHECK-DAG:       [[VAR_2_:%.+]] = tosa.reshape [[PARAM_0_]] {new_shape = array<i64: 1, 1, 1, 1, 1, 42>} : (tensor<1x42x1x1xbf16>) -> tensor<1x1x1x1x1x42xbf16>
// CHECK-NOT: separator of consecutive DAGs
// CHECK-DAG:       [[VAR_3_:%.+]] = tosa.transpose [[VAR_2_]], [[VAR_1_]] : (tensor<1x1x1x1x1x42xbf16>, tensor<6xi32>) -> tensor<1x1x1x1x1x42xbf16>
// CHECK-DAG:       [[VAR_4_:%.+]] = tosa.reshape [[PARAM_2_]] {new_shape = array<i64: 1, 12, 42>} : (tensor<12x42x1x1xbf16>) -> tensor<1x12x42xbf16>
// CHECK-NOT: separator of consecutive DAGs
// CHECK-DAG:       [[VAR_5_:%.+]] = tosa.transpose [[VAR_4_]], [[VAR_0_]] : (tensor<1x12x42xbf16>, tensor<3xi32>) -> tensor<1x42x12xbf16>
// CHECK-DAG:       [[VAR_6_:%.+]] = tosa.reshape [[PARAM_1_]] {new_shape = array<i64: 1, 1, 1, 1, 1, 42>} : (tensor<1x42x1x1xbf16>) -> tensor<1x1x1x1x1x42xbf16>
// CHECK-NOT: separator of consecutive DAGs
// CHECK-DAG:       [[VAR_7_:%.+]] = tosa.transpose [[VAR_6_]], [[VAR_1_]] : (tensor<1x1x1x1x1x42xbf16>, tensor<6xi32>) -> tensor<1x1x1x1x1x42xbf16>
// CHECK-DAG:       [[VAR_8_:%.+]] = tosa.reshape [[PARAM_3_]] {new_shape = array<i64: 1, 12, 42>} : (tensor<12x42x1x1xbf16>) -> tensor<1x12x42xbf16>
// CHECK-NOT: separator of consecutive DAGs
// CHECK-DAG:       [[VAR_9_:%.+]] = tosa.transpose [[VAR_8_]], [[VAR_0_]] : (tensor<1x12x42xbf16>, tensor<3xi32>) -> tensor<1x42x12xbf16>
// CHECK-DAG:       [[VAR_10_:%.+]] = tosa.concat [[VAR_3_]], [[VAR_7_]] {axis = 0 : i32} : (tensor<1x1x1x1x1x42xbf16>, tensor<1x1x1x1x1x42xbf16>) -> tensor<2x1x1x1x1x42xbf16>
// CHECK-NOT: separator of consecutive DAGs
// CHECK-DAG:       [[VAR_11_:%.+]] = tosa.reshape [[VAR_10_]] {new_shape = array<i64: 2, 1, 42>} : (tensor<2x1x1x1x1x42xbf16>) -> tensor<2x1x42xbf16>
// CHECK-DAG:       [[VAR_12_:%.+]] = tosa.concat [[VAR_5_]], [[VAR_9_]] {axis = 0 : i32} : (tensor<1x42x12xbf16>, tensor<1x42x12xbf16>) -> tensor<2x42x12xbf16>
// CHECK:           [[VAR_13_:%.+]] = tosa.matmul [[VAR_11_]], [[VAR_12_]] : (tensor<2x1x42xbf16>, tensor<2x42x12xbf16>) -> tensor<2x1x12xbf16>
// CHECK:           [[VAR_14_:%.+]] = tosa.reshape [[VAR_13_]] {new_shape = array<i64: 1, 2, 1, 12>} : (tensor<2x1x12xbf16>) -> tensor<1x2x1x12xbf16>
// CHECK:           return [[VAR_14_]] : tensor<1x2x1x12xbf16>
// CHECK:         }
// -----

// Operands reshape to the same concat shape but split the concat axis
// differently: three split 256 as 16x16, one as 32x8. The odd operand is
// adapted to the majority 16x16 split so a single high-rank concat plus one
// trailing reshape can sink all four reshapes.
func.func @reshape_differing_decomposition(%arg0: tensor<64x16x16x80x1xbf16>, %arg1: tensor<64x16x16x80x1xbf16>, %arg2: tensor<64x16x16x80x1xbf16>, %arg3: tensor<64x32x8x80x1xbf16>) -> tensor<64x1024x80x1xbf16> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 64, 256, 80, 1>} : (tensor<64x16x16x80x1xbf16>) -> tensor<64x256x80x1xbf16>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 64, 256, 80, 1>} : (tensor<64x16x16x80x1xbf16>) -> tensor<64x256x80x1xbf16>
  %2 = tosa.reshape %arg2 {new_shape = array<i64: 64, 256, 80, 1>} : (tensor<64x16x16x80x1xbf16>) -> tensor<64x256x80x1xbf16>
  %3 = tosa.reshape %arg3 {new_shape = array<i64: 64, 256, 80, 1>} : (tensor<64x32x8x80x1xbf16>) -> tensor<64x256x80x1xbf16>
  %4 = tosa.concat %0, %1, %2, %3 {axis = 1 : i32} : (tensor<64x256x80x1xbf16>, tensor<64x256x80x1xbf16>, tensor<64x256x80x1xbf16>, tensor<64x256x80x1xbf16>) -> tensor<64x1024x80x1xbf16>
  return %4 : tensor<64x1024x80x1xbf16>
}

// CHECK-LABEL:  func.func @reshape_differing_decomposition
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<64x16x16x80x1xbf16>, [[PARAM_1_:%.+]]: tensor<64x16x16x80x1xbf16>, [[PARAM_2_:%.+]]: tensor<64x16x16x80x1xbf16>, [[PARAM_3_:%.+]]: tensor<64x32x8x80x1xbf16>) -> tensor<64x1024x80x1xbf16> {
// CHECK:           [[VAR_0_:%.+]] = tosa.reshape [[PARAM_3_]] {new_shape = array<i64: 64, 16, 16, 80, 1>} : (tensor<64x32x8x80x1xbf16>) -> tensor<64x16x16x80x1xbf16>
// CHECK:           [[VAR_1_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_2_]], [[VAR_0_]] {axis = 1 : i32} : (tensor<64x16x16x80x1xbf16>, tensor<64x16x16x80x1xbf16>, tensor<64x16x16x80x1xbf16>, tensor<64x16x16x80x1xbf16>) -> tensor<64x64x16x80x1xbf16>
// CHECK:           [[VAR_2_:%.+]] = tosa.reshape [[VAR_1_]] {new_shape = array<i64: 64, 1024, 80, 1>} : (tensor<64x64x16x80x1xbf16>) -> tensor<64x1024x80x1xbf16>
// CHECK:           return [[VAR_2_]] : tensor<64x1024x80x1xbf16>
// CHECK:         }
// -----

// Two minority operands (32x8 and 8x32) are both adapted to the 16x16 majority.
// 4 reshapes -> 2 adapters + 1 trailing reshape still reduces the count.
func.func @reshape_two_minorities(%arg0: tensor<64x16x16x80x1xbf16>, %arg1: tensor<64x16x16x80x1xbf16>, %arg2: tensor<64x32x8x80x1xbf16>, %arg3: tensor<64x8x32x80x1xbf16>) -> tensor<64x1024x80x1xbf16> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 64, 256, 80, 1>} : (tensor<64x16x16x80x1xbf16>) -> tensor<64x256x80x1xbf16>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 64, 256, 80, 1>} : (tensor<64x16x16x80x1xbf16>) -> tensor<64x256x80x1xbf16>
  %2 = tosa.reshape %arg2 {new_shape = array<i64: 64, 256, 80, 1>} : (tensor<64x32x8x80x1xbf16>) -> tensor<64x256x80x1xbf16>
  %3 = tosa.reshape %arg3 {new_shape = array<i64: 64, 256, 80, 1>} : (tensor<64x8x32x80x1xbf16>) -> tensor<64x256x80x1xbf16>
  %4 = tosa.concat %0, %1, %2, %3 {axis = 1 : i32} : (tensor<64x256x80x1xbf16>, tensor<64x256x80x1xbf16>, tensor<64x256x80x1xbf16>, tensor<64x256x80x1xbf16>) -> tensor<64x1024x80x1xbf16>
  return %4 : tensor<64x1024x80x1xbf16>
}

// CHECK-LABEL:  func.func @reshape_two_minorities
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<64x16x16x80x1xbf16>, [[PARAM_1_:%.+]]: tensor<64x16x16x80x1xbf16>, [[PARAM_2_:%.+]]: tensor<64x32x8x80x1xbf16>, [[PARAM_3_:%.+]]: tensor<64x8x32x80x1xbf16>) -> tensor<64x1024x80x1xbf16> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.reshape [[PARAM_2_]] {new_shape = array<i64: 64, 16, 16, 80, 1>} : (tensor<64x32x8x80x1xbf16>) -> tensor<64x16x16x80x1xbf16>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.reshape [[PARAM_3_]] {new_shape = array<i64: 64, 16, 16, 80, 1>} : (tensor<64x8x32x80x1xbf16>) -> tensor<64x16x16x80x1xbf16>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[VAR_0_]], [[VAR_1_]] {axis = 1 : i32} : (tensor<64x16x16x80x1xbf16>, tensor<64x16x16x80x1xbf16>, tensor<64x16x16x80x1xbf16>, tensor<64x16x16x80x1xbf16>) -> tensor<64x64x16x80x1xbf16>
// CHECK:           [[VAR_3_:%.+]] = tosa.reshape [[VAR_2_]] {new_shape = array<i64: 64, 1024, 80, 1>} : (tensor<64x64x16x80x1xbf16>) -> tensor<64x1024x80x1xbf16>
// CHECK:           return [[VAR_3_]] : tensor<64x1024x80x1xbf16>
// CHECK:         }
// -----

// The majority operands keep the concat axis unsplit (single axis 16); the
// minority 4x4 split is collapsed to match. The trailing identity reshape folds
// away, leaving just the adapter reshape and the concat.
func.func @reshape_collapse_minority(%arg0: tensor<4x16x5xbf16>, %arg1: tensor<4x16x5xbf16>, %arg2: tensor<4x4x4x5xbf16>) -> tensor<4x48x5xbf16> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 4, 16, 5>} : (tensor<4x16x5xbf16>) -> tensor<4x16x5xbf16>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 4, 16, 5>} : (tensor<4x16x5xbf16>) -> tensor<4x16x5xbf16>
  %2 = tosa.reshape %arg2 {new_shape = array<i64: 4, 16, 5>} : (tensor<4x4x4x5xbf16>) -> tensor<4x16x5xbf16>
  %3 = tosa.concat %0, %1, %2 {axis = 1 : i32} : (tensor<4x16x5xbf16>, tensor<4x16x5xbf16>, tensor<4x16x5xbf16>) -> tensor<4x48x5xbf16>
  return %3 : tensor<4x48x5xbf16>
}

// CHECK-LABEL:  func.func @reshape_collapse_minority
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<4x16x5xbf16>, [[PARAM_1_:%.+]]: tensor<4x16x5xbf16>, [[PARAM_2_:%.+]]: tensor<4x4x4x5xbf16>) -> tensor<4x48x5xbf16> {
// CHECK:           [[VAR_0_:%.+]] = tosa.reshape [[PARAM_2_]] {new_shape = array<i64: 4, 16, 5>} : (tensor<4x4x4x5xbf16>) -> tensor<4x16x5xbf16>
// CHECK:           [[VAR_1_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[VAR_0_]] {axis = 1 : i32} : (tensor<4x16x5xbf16>, tensor<4x16x5xbf16>, tensor<4x16x5xbf16>) -> tensor<4x48x5xbf16>
// CHECK:           return [[VAR_1_]] : tensor<4x48x5xbf16>
// CHECK:         }
// -----

// No decomposition is shared by a majority (all three operands differ), so
// adapting would not reduce the reshape count: the pass must not fire.
func.func @reshape_no_majority(%arg0: tensor<4x4x4x5xbf16>, %arg1: tensor<4x2x8x5xbf16>, %arg2: tensor<4x8x2x5xbf16>) -> tensor<4x48x5xbf16> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 4, 16, 5>} : (tensor<4x4x4x5xbf16>) -> tensor<4x16x5xbf16>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 4, 16, 5>} : (tensor<4x2x8x5xbf16>) -> tensor<4x16x5xbf16>
  %2 = tosa.reshape %arg2 {new_shape = array<i64: 4, 16, 5>} : (tensor<4x8x2x5xbf16>) -> tensor<4x16x5xbf16>
  %3 = tosa.concat %0, %1, %2 {axis = 1 : i32} : (tensor<4x16x5xbf16>, tensor<4x16x5xbf16>, tensor<4x16x5xbf16>) -> tensor<4x48x5xbf16>
  return %3 : tensor<4x48x5xbf16>
}

// CHECK-LABEL:  func.func @reshape_no_majority
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<4x4x4x5xbf16>, [[PARAM_1_:%.+]]: tensor<4x2x8x5xbf16>, [[PARAM_2_:%.+]]: tensor<4x8x2x5xbf16>) -> tensor<4x48x5xbf16> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.reshape [[PARAM_0_]] {new_shape = array<i64: 4, 16, 5>} : (tensor<4x4x4x5xbf16>) -> tensor<4x16x5xbf16>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.reshape [[PARAM_1_]] {new_shape = array<i64: 4, 16, 5>} : (tensor<4x2x8x5xbf16>) -> tensor<4x16x5xbf16>
// CHECK-DAG:       [[VAR_2_:%.+]] = tosa.reshape [[PARAM_2_]] {new_shape = array<i64: 4, 16, 5>} : (tensor<4x8x2x5xbf16>) -> tensor<4x16x5xbf16>
// CHECK:           [[VAR_3_:%.+]] = tosa.concat [[VAR_0_]], [[VAR_1_]], [[VAR_2_]] {axis = 1 : i32} : (tensor<4x16x5xbf16>, tensor<4x16x5xbf16>, tensor<4x16x5xbf16>) -> tensor<4x48x5xbf16>
// CHECK:           return [[VAR_3_]] : tensor<4x48x5xbf16>
// CHECK:         }
// -----

// Two distinct minorities (2x8 and 8x2) are adapted to the 4x4 majority layout
// shared by arg0 and arg3 (arg3 reaches it with a differing 8x4 split of a
// larger concat dim). 4 reshapes -> 2 adapters + 1 trailing reshape.
func.func @reshape_with_majority(%arg0: tensor<4x4x4x5xbf16>, %arg1: tensor<4x2x8x5xbf16>, %arg2: tensor<4x8x2x5xbf16>, %arg3: tensor<4x8x4x5xbf16>) -> tensor<4x80x5xbf16> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 4, 16, 5>} : (tensor<4x4x4x5xbf16>) -> tensor<4x16x5xbf16>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 4, 16, 5>} : (tensor<4x2x8x5xbf16>) -> tensor<4x16x5xbf16>
  %2 = tosa.reshape %arg2 {new_shape = array<i64: 4, 16, 5>} : (tensor<4x8x2x5xbf16>) -> tensor<4x16x5xbf16>
  %4 = tosa.reshape %arg3 {new_shape = array<i64: 4, 32, 5>} : (tensor<4x8x4x5xbf16>) -> tensor<4x32x5xbf16>
  %3 = tosa.concat %0, %1, %2, %4 {axis = 1 : i32} : (tensor<4x16x5xbf16>, tensor<4x16x5xbf16>, tensor<4x16x5xbf16>, tensor<4x32x5xbf16>) -> tensor<4x80x5xbf16>
  return %3 : tensor<4x80x5xbf16>
}

// CHECK-LABEL:  func.func @reshape_with_majority
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<4x4x4x5xbf16>, [[PARAM_1_:%.+]]: tensor<4x2x8x5xbf16>, [[PARAM_2_:%.+]]: tensor<4x8x2x5xbf16>, [[PARAM_3_:%.+]]: tensor<4x8x4x5xbf16>) -> tensor<4x80x5xbf16> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.reshape [[PARAM_1_]] {new_shape = array<i64: 4, 4, 4, 5>} : (tensor<4x2x8x5xbf16>) -> tensor<4x4x4x5xbf16>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.reshape [[PARAM_2_]] {new_shape = array<i64: 4, 4, 4, 5>} : (tensor<4x8x2x5xbf16>) -> tensor<4x4x4x5xbf16>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[PARAM_0_]], [[VAR_0_]], [[VAR_1_]], [[PARAM_3_]] {axis = 1 : i32} : (tensor<4x4x4x5xbf16>, tensor<4x4x4x5xbf16>, tensor<4x4x4x5xbf16>, tensor<4x8x4x5xbf16>) -> tensor<4x20x4x5xbf16>
// CHECK:           [[VAR_3_:%.+]] = tosa.reshape [[VAR_2_]] {new_shape = array<i64: 4, 80, 5>} : (tensor<4x20x4x5xbf16>) -> tensor<4x80x5xbf16>
// CHECK:           return [[VAR_3_]] : tensor<4x80x5xbf16>
// CHECK:         }

// -----

// The minority operand splits the concat axis differently (16x8 vs 16x16) *and*
// carries a differing suffix decomposition (40x2 vs 80x1). The suffixes hold
// the same number of elements (80) and the minority reshape is a pure collapse,
// so it can still be adapted to the majority 16x16 / 80x1 layout: a single
// adapter reshape relinearizes it, then one concat plus one trailing reshape
// sinks all four reshapes.
func.func @reshape_suffix_mismatch(%arg0: tensor<64x16x16x80x1xbf16>, %arg1: tensor<64x4x16x80x1xbf16>, %arg2: tensor<64x2x16x80x1xbf16>, %arg3: tensor<64x16x8x40x2xbf16>) -> tensor<64x480x80x1xbf16> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 64, 256, 80, 1>} : (tensor<64x16x16x80x1xbf16>) -> tensor<64x256x80x1xbf16>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 64, 64, 80, 1>} : (tensor<64x4x16x80x1xbf16>) -> tensor<64x64x80x1xbf16>
  %2 = tosa.reshape %arg2 {new_shape = array<i64: 64, 32, 80, 1>} : (tensor<64x2x16x80x1xbf16>) -> tensor<64x32x80x1xbf16>
  %3 = tosa.reshape %arg3 {new_shape = array<i64: 64, 128, 80, 1>} : (tensor<64x16x8x40x2xbf16>) -> tensor<64x128x80x1xbf16>
  %4 = tosa.concat %0, %1, %2, %3 {axis = 1 : i32} : (tensor<64x256x80x1xbf16>, tensor<64x64x80x1xbf16>, tensor<64x32x80x1xbf16>, tensor<64x128x80x1xbf16>) -> tensor<64x480x80x1xbf16>
  return %4 : tensor<64x480x80x1xbf16>
}

// CHECK-LABEL:  func.func @reshape_suffix_mismatch
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<64x16x16x80x1xbf16>, [[PARAM_1_:%.+]]: tensor<64x4x16x80x1xbf16>, [[PARAM_2_:%.+]]: tensor<64x2x16x80x1xbf16>, [[PARAM_3_:%.+]]: tensor<64x16x8x40x2xbf16>) -> tensor<64x480x80x1xbf16> {
// CHECK:           [[VAR_0_:%.+]] = tosa.reshape [[PARAM_3_]] {new_shape = array<i64: 64, 8, 16, 80, 1>} : (tensor<64x16x8x40x2xbf16>) -> tensor<64x8x16x80x1xbf16>
// CHECK:           [[VAR_1_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_2_]], [[VAR_0_]] {axis = 1 : i32} : (tensor<64x16x16x80x1xbf16>, tensor<64x4x16x80x1xbf16>, tensor<64x2x16x80x1xbf16>, tensor<64x8x16x80x1xbf16>) -> tensor<64x30x16x80x1xbf16>
// CHECK:           [[VAR_2_:%.+]] = tosa.reshape [[VAR_1_]] {new_shape = array<i64: 64, 480, 80, 1>} : (tensor<64x30x16x80x1xbf16>) -> tensor<64x480x80x1xbf16>
// CHECK:           return [[VAR_2_]] : tensor<64x480x80x1xbf16>
// CHECK:         }
// -----

// Operands describe the same layout but place their unit dimensions
// differently (arg2 leads with a unit dim, the others trail with one). Unit
// dims are ignored when matching, so the majority real layout (2x4x8x1) is
// chosen and the minority operand is adapted to it. 3 reshapes -> 1 adapter +
// 1 trailing reshape.
func.func @reshape_unit_dim_placement(%arg0: tensor<2x4x8x1xf32>, %arg1: tensor<2x4x8x1xf32>, %arg2: tensor<1x2x4x8xf32>) -> tensor<2x96x1xf32> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 2, 32, 1>} : (tensor<2x4x8x1xf32>) -> tensor<2x32x1xf32>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 2, 32, 1>} : (tensor<2x4x8x1xf32>) -> tensor<2x32x1xf32>
  %2 = tosa.reshape %arg2 {new_shape = array<i64: 2, 32, 1>} : (tensor<1x2x4x8xf32>) -> tensor<2x32x1xf32>
  %3 = tosa.concat %0, %1, %2 {axis = 1 : i32} : (tensor<2x32x1xf32>, tensor<2x32x1xf32>, tensor<2x32x1xf32>) -> tensor<2x96x1xf32>
  return %3 : tensor<2x96x1xf32>
}

// CHECK-LABEL:  func.func @reshape_unit_dim_placement
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<2x4x8x1xf32>, [[PARAM_1_:%.+]]: tensor<2x4x8x1xf32>, [[PARAM_2_:%.+]]: tensor<1x2x4x8xf32>) -> tensor<2x96x1xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.reshape [[PARAM_2_]] {new_shape = array<i64: 2, 4, 8, 1>} : (tensor<1x2x4x8xf32>) -> tensor<2x4x8x1xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[VAR_0_]] {axis = 1 : i32} : (tensor<2x4x8x1xf32>, tensor<2x4x8x1xf32>, tensor<2x4x8x1xf32>) -> tensor<2x12x8x1xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.reshape [[VAR_1_]] {new_shape = array<i64: 2, 96, 1>} : (tensor<2x12x8x1xf32>) -> tensor<2x96x1xf32>
// CHECK:           return [[VAR_2_]] : tensor<2x96x1xf32>
// CHECK:         }
// -----

// Same differing-decomposition adaptation, but the minority operand (32x8) is
// the *first* concat operand. The adapter is still placed on it and the concat
// preserves operand order.
func.func @reshape_minority_first(%arg0: tensor<64x32x8x80x1xbf16>, %arg1: tensor<64x16x16x80x1xbf16>, %arg2: tensor<64x16x16x80x1xbf16>) -> tensor<64x768x80x1xbf16> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 64, 256, 80, 1>} : (tensor<64x32x8x80x1xbf16>) -> tensor<64x256x80x1xbf16>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 64, 256, 80, 1>} : (tensor<64x16x16x80x1xbf16>) -> tensor<64x256x80x1xbf16>
  %2 = tosa.reshape %arg2 {new_shape = array<i64: 64, 256, 80, 1>} : (tensor<64x16x16x80x1xbf16>) -> tensor<64x256x80x1xbf16>
  %3 = tosa.concat %0, %1, %2 {axis = 1 : i32} : (tensor<64x256x80x1xbf16>, tensor<64x256x80x1xbf16>, tensor<64x256x80x1xbf16>) -> tensor<64x768x80x1xbf16>
  return %3 : tensor<64x768x80x1xbf16>
}

// CHECK-LABEL:  func.func @reshape_minority_first
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<64x32x8x80x1xbf16>, [[PARAM_1_:%.+]]: tensor<64x16x16x80x1xbf16>, [[PARAM_2_:%.+]]: tensor<64x16x16x80x1xbf16>) -> tensor<64x768x80x1xbf16> {
// CHECK:           [[VAR_0_:%.+]] = tosa.reshape [[PARAM_0_]] {new_shape = array<i64: 64, 16, 16, 80, 1>} : (tensor<64x32x8x80x1xbf16>) -> tensor<64x16x16x80x1xbf16>
// CHECK:           [[VAR_1_:%.+]] = tosa.concat [[VAR_0_]], [[PARAM_1_]], [[PARAM_2_]] {axis = 1 : i32} : (tensor<64x16x16x80x1xbf16>, tensor<64x16x16x80x1xbf16>, tensor<64x16x16x80x1xbf16>) -> tensor<64x48x16x80x1xbf16>
// CHECK:           [[VAR_2_:%.+]] = tosa.reshape [[VAR_1_]] {new_shape = array<i64: 64, 768, 80, 1>} : (tensor<64x48x16x80x1xbf16>) -> tensor<64x768x80x1xbf16>
// CHECK:           return [[VAR_2_]] : tensor<64x768x80x1xbf16>
// CHECK:         }
// -----

// The minority operand (32x8) sits in the *middle* of the concat operands.
func.func @reshape_minority_middle(%arg0: tensor<64x16x16x80x1xbf16>, %arg1: tensor<64x32x8x80x1xbf16>, %arg2: tensor<64x16x16x80x1xbf16>) -> tensor<64x768x80x1xbf16> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 64, 256, 80, 1>} : (tensor<64x16x16x80x1xbf16>) -> tensor<64x256x80x1xbf16>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 64, 256, 80, 1>} : (tensor<64x32x8x80x1xbf16>) -> tensor<64x256x80x1xbf16>
  %2 = tosa.reshape %arg2 {new_shape = array<i64: 64, 256, 80, 1>} : (tensor<64x16x16x80x1xbf16>) -> tensor<64x256x80x1xbf16>
  %3 = tosa.concat %0, %1, %2 {axis = 1 : i32} : (tensor<64x256x80x1xbf16>, tensor<64x256x80x1xbf16>, tensor<64x256x80x1xbf16>) -> tensor<64x768x80x1xbf16>
  return %3 : tensor<64x768x80x1xbf16>
}

// CHECK-LABEL:  func.func @reshape_minority_middle
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<64x16x16x80x1xbf16>, [[PARAM_1_:%.+]]: tensor<64x32x8x80x1xbf16>, [[PARAM_2_:%.+]]: tensor<64x16x16x80x1xbf16>) -> tensor<64x768x80x1xbf16> {
// CHECK:           [[VAR_0_:%.+]] = tosa.reshape [[PARAM_1_]] {new_shape = array<i64: 64, 16, 16, 80, 1>} : (tensor<64x32x8x80x1xbf16>) -> tensor<64x16x16x80x1xbf16>
// CHECK:           [[VAR_1_:%.+]] = tosa.concat [[PARAM_0_]], [[VAR_0_]], [[PARAM_2_]] {axis = 1 : i32} : (tensor<64x16x16x80x1xbf16>, tensor<64x16x16x80x1xbf16>, tensor<64x16x16x80x1xbf16>) -> tensor<64x48x16x80x1xbf16>
// CHECK:           [[VAR_2_:%.+]] = tosa.reshape [[VAR_1_]] {new_shape = array<i64: 64, 768, 80, 1>} : (tensor<64x48x16x80x1xbf16>) -> tensor<64x768x80x1xbf16>
// CHECK:           return [[VAR_2_]] : tensor<64x768x80x1xbf16>
// CHECK:         }
// -----

// Rank-reducing reshapes (3D -> 1D) that fully collapse the tensor. The single
// shared pre-reshape axis 0 lets the concat sink with no adapter.
func.func @reshape_merge_to_1d(%arg0: tensor<2x3x4xf32>, %arg1: tensor<2x3x4xf32>) -> tensor<48xf32> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 24>} : (tensor<2x3x4xf32>) -> tensor<24xf32>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 24>} : (tensor<2x3x4xf32>) -> tensor<24xf32>
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (tensor<24xf32>, tensor<24xf32>) -> tensor<48xf32>
  return %2 : tensor<48xf32>
}

// CHECK-LABEL:  func.func @reshape_merge_to_1d
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<2x3x4xf32>, [[PARAM_1_:%.+]]: tensor<2x3x4xf32>) -> tensor<48xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<2x3x4xf32>, tensor<2x3x4xf32>) -> tensor<4x3x4xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.reshape [[VAR_0_]] {new_shape = array<i64: 48>} : (tensor<4x3x4xf32>) -> tensor<48xf32>
// CHECK:           return [[VAR_1_]] : tensor<48xf32>
// CHECK:         }
// -----

// Rank-increasing reshapes (2D -> 4D) that split a dimension. The concat axis 0
// maps to input axis 0 and sinks with no adapter.
func.func @reshape_split_higher_rank(%arg0: tensor<3x24xf32>, %arg1: tensor<3x24xf32>) -> tensor<6x2x3x4xf32> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 3, 2, 3, 4>} : (tensor<3x24xf32>) -> tensor<3x2x3x4xf32>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 3, 2, 3, 4>} : (tensor<3x24xf32>) -> tensor<3x2x3x4xf32>
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (tensor<3x2x3x4xf32>, tensor<3x2x3x4xf32>) -> tensor<6x2x3x4xf32>
  return %2 : tensor<6x2x3x4xf32>
}

// CHECK-LABEL:  func.func @reshape_split_higher_rank
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<3x24xf32>, [[PARAM_1_:%.+]]: tensor<3x24xf32>) -> tensor<6x2x3x4xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<3x24xf32>, tensor<3x24xf32>) -> tensor<6x24xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.reshape [[VAR_0_]] {new_shape = array<i64: 6, 2, 3, 4>} : (tensor<6x24xf32>) -> tensor<6x2x3x4xf32>
// CHECK:           return [[VAR_1_]] : tensor<6x2x3x4xf32>
// CHECK:         }
// -----

// Complex reshape: the minority operand both drops a unit dimension (implicit
// squeeze of the leading-group 1) *and* keeps the concat axis unsplit (12)
// while the majority splits it (3x4). It is adapted to the majority 3x4 layout.
func.func @reshape_squeeze_and_split(%arg0: tensor<2x3x4x5xf32>, %arg1: tensor<2x3x4x5xf32>, %arg2: tensor<2x1x12x5xf32>) -> tensor<2x36x5xf32> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 2, 12, 5>} : (tensor<2x3x4x5xf32>) -> tensor<2x12x5xf32>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 2, 12, 5>} : (tensor<2x3x4x5xf32>) -> tensor<2x12x5xf32>
  %2 = tosa.reshape %arg2 {new_shape = array<i64: 2, 12, 5>} : (tensor<2x1x12x5xf32>) -> tensor<2x12x5xf32>
  %3 = tosa.concat %0, %1, %2 {axis = 1 : i32} : (tensor<2x12x5xf32>, tensor<2x12x5xf32>, tensor<2x12x5xf32>) -> tensor<2x36x5xf32>
  return %3 : tensor<2x36x5xf32>
}

// CHECK-LABEL:  func.func @reshape_squeeze_and_split
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<2x3x4x5xf32>, [[PARAM_1_:%.+]]: tensor<2x3x4x5xf32>, [[PARAM_2_:%.+]]: tensor<2x1x12x5xf32>) -> tensor<2x36x5xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.reshape [[PARAM_2_]] {new_shape = array<i64: 2, 3, 4, 5>} : (tensor<2x1x12x5xf32>) -> tensor<2x3x4x5xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[VAR_0_]] {axis = 1 : i32} : (tensor<2x3x4x5xf32>, tensor<2x3x4x5xf32>, tensor<2x3x4x5xf32>) -> tensor<2x9x4x5xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.reshape [[VAR_1_]] {new_shape = array<i64: 2, 36, 5>} : (tensor<2x9x4x5xf32>) -> tensor<2x36x5xf32>
// CHECK:           return [[VAR_2_]] : tensor<2x36x5xf32>
// CHECK:         }
// -----

// Complex reshape: the majority carries a trailing unit dim (2x6x1x8) while the
// minority carries a leading one (1x2x6x8). Unit dims are ignored when matching,
// so the minority is reshaped to the majority real layout before the concat.
func.func @reshape_mixed_unit_dims(%arg0: tensor<2x6x1x8xf32>, %arg1: tensor<2x6x1x8xf32>, %arg2: tensor<1x2x6x8xf32>) -> tensor<2x18x8xf32> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 2, 6, 8>} : (tensor<2x6x1x8xf32>) -> tensor<2x6x8xf32>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 2, 6, 8>} : (tensor<2x6x1x8xf32>) -> tensor<2x6x8xf32>
  %2 = tosa.reshape %arg2 {new_shape = array<i64: 2, 6, 8>} : (tensor<1x2x6x8xf32>) -> tensor<2x6x8xf32>
  %3 = tosa.concat %0, %1, %2 {axis = 1 : i32} : (tensor<2x6x8xf32>, tensor<2x6x8xf32>, tensor<2x6x8xf32>) -> tensor<2x18x8xf32>
  return %3 : tensor<2x18x8xf32>
}

// CHECK-LABEL:  func.func @reshape_mixed_unit_dims
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<2x6x1x8xf32>, [[PARAM_1_:%.+]]: tensor<2x6x1x8xf32>, [[PARAM_2_:%.+]]: tensor<1x2x6x8xf32>) -> tensor<2x18x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.reshape [[PARAM_2_]] {new_shape = array<i64: 2, 6, 1, 8>} : (tensor<1x2x6x8xf32>) -> tensor<2x6x1x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[VAR_0_]] {axis = 1 : i32} : (tensor<2x6x1x8xf32>, tensor<2x6x1x8xf32>, tensor<2x6x1x8xf32>) -> tensor<2x18x1x8xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.reshape [[VAR_1_]] {new_shape = array<i64: 2, 18, 8>} : (tensor<2x18x1x8xf32>) -> tensor<2x18x8xf32>
// CHECK:           return [[VAR_2_]] : tensor<2x18x8xf32>
// CHECK:         }
// -----

// Five operands: three share the unsplit majority layout (6) and two distinct
// minorities (2x3 and 3x2) are each adapted. 5 reshapes -> 2 adapters and the
// trailing reshape folds away (concat already yields the result shape).
func.func @reshape_five_operands(%arg0: tensor<8x6x10xf32>, %arg1: tensor<8x6x10xf32>, %arg2: tensor<8x6x10xf32>, %arg3: tensor<8x2x3x10xf32>, %arg4: tensor<8x3x2x10xf32>) -> tensor<8x30x10xf32> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 8, 6, 10>} : (tensor<8x6x10xf32>) -> tensor<8x6x10xf32>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 8, 6, 10>} : (tensor<8x6x10xf32>) -> tensor<8x6x10xf32>
  %2 = tosa.reshape %arg2 {new_shape = array<i64: 8, 6, 10>} : (tensor<8x6x10xf32>) -> tensor<8x6x10xf32>
  %3 = tosa.reshape %arg3 {new_shape = array<i64: 8, 6, 10>} : (tensor<8x2x3x10xf32>) -> tensor<8x6x10xf32>
  %4 = tosa.reshape %arg4 {new_shape = array<i64: 8, 6, 10>} : (tensor<8x3x2x10xf32>) -> tensor<8x6x10xf32>
  %5 = tosa.concat %0, %1, %2, %3, %4 {axis = 1 : i32} : (tensor<8x6x10xf32>, tensor<8x6x10xf32>, tensor<8x6x10xf32>, tensor<8x6x10xf32>, tensor<8x6x10xf32>) -> tensor<8x30x10xf32>
  return %5 : tensor<8x30x10xf32>
}

// CHECK-LABEL:  func.func @reshape_five_operands
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<8x6x10xf32>, [[PARAM_1_:%.+]]: tensor<8x6x10xf32>, [[PARAM_2_:%.+]]: tensor<8x6x10xf32>, [[PARAM_3_:%.+]]: tensor<8x2x3x10xf32>, [[PARAM_4_:%.+]]: tensor<8x3x2x10xf32>) -> tensor<8x30x10xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.reshape [[PARAM_3_]] {new_shape = array<i64: 8, 6, 10>} : (tensor<8x2x3x10xf32>) -> tensor<8x6x10xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.reshape [[PARAM_4_]] {new_shape = array<i64: 8, 6, 10>} : (tensor<8x3x2x10xf32>) -> tensor<8x6x10xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_2_]], [[VAR_0_]], [[VAR_1_]] {axis = 1 : i32} : (tensor<8x6x10xf32>, tensor<8x6x10xf32>, tensor<8x6x10xf32>, tensor<8x6x10xf32>, tensor<8x6x10xf32>) -> tensor<8x30x10xf32>
// CHECK:           return [[VAR_2_]] : tensor<8x30x10xf32>
// CHECK:         }
// -----

// A reshape result feeding both the concat and another consumer is not
// single-use, so the pattern must not fire.
func.func @reshape_multiple_uses(%arg0: tensor<2x3x4xf32>, %arg1: tensor<2x3x4xf32>) -> (tensor<48xf32>, tensor<24xf32>) {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 24>} : (tensor<2x3x4xf32>) -> tensor<24xf32>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 24>} : (tensor<2x3x4xf32>) -> tensor<24xf32>
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (tensor<24xf32>, tensor<24xf32>) -> tensor<48xf32>
  return %2, %0 : tensor<48xf32>, tensor<24xf32>
}

// CHECK-LABEL:  func.func @reshape_multiple_uses
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<2x3x4xf32>, [[PARAM_1_:%.+]]: tensor<2x3x4xf32>) -> (tensor<48xf32>, tensor<24xf32>) {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.reshape [[PARAM_0_]] {new_shape = array<i64: 24>} : (tensor<2x3x4xf32>) -> tensor<24xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.reshape [[PARAM_1_]] {new_shape = array<i64: 24>} : (tensor<2x3x4xf32>) -> tensor<24xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[VAR_0_]], [[VAR_1_]] {axis = 0 : i32} : (tensor<24xf32>, tensor<24xf32>) -> tensor<48xf32>
// CHECK:           return [[VAR_2_]], [[VAR_0_]] : tensor<48xf32>, tensor<24xf32>
// CHECK:         }
// -----

// Dynamic reshape input shapes are unsupported, so the pattern must not fire.
func.func @reshape_dynamic_shape(%arg0: tensor<?x4xf32>, %arg1: tensor<?x4xf32>) -> tensor<?x2x2xf32> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: -1, 2, 2>} : (tensor<?x4xf32>) -> tensor<?x2x2xf32>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: -1, 2, 2>} : (tensor<?x4xf32>) -> tensor<?x2x2xf32>
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (tensor<?x2x2xf32>, tensor<?x2x2xf32>) -> tensor<?x2x2xf32>
  return %2 : tensor<?x2x2xf32>
}

// CHECK-LABEL:  func.func @reshape_dynamic_shape
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<?x4xf32>, [[PARAM_1_:%.+]]: tensor<?x4xf32>) -> tensor<?x2x2xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.reshape [[PARAM_0_]] {new_shape = array<i64: -1, 2, 2>} : (tensor<?x4xf32>) -> tensor<?x2x2xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.reshape [[PARAM_1_]] {new_shape = array<i64: -1, 2, 2>} : (tensor<?x4xf32>) -> tensor<?x2x2xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[VAR_0_]], [[VAR_1_]] {axis = 0 : i32} : (tensor<?x2x2xf32>, tensor<?x2x2xf32>) -> tensor<?x2x2xf32>
// CHECK:           return [[VAR_2_]] : tensor<?x2x2xf32>
// CHECK:         }
// -----

// Concat on the innermost axis: the reshape merges the trailing 2x3 into 6, so
// the concat sinks onto input axis 1 (the start of that merged group).
func.func @reshape_innermost_axis(%arg0: tensor<4x2x3xf32>, %arg1: tensor<4x2x3xf32>) -> tensor<4x12xf32> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 4, 6>} : (tensor<4x2x3xf32>) -> tensor<4x6xf32>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 4, 6>} : (tensor<4x2x3xf32>) -> tensor<4x6xf32>
  %2 = tosa.concat %0, %1 {axis = 1 : i32} : (tensor<4x6xf32>, tensor<4x6xf32>) -> tensor<4x12xf32>
  return %2 : tensor<4x12xf32>
}

// CHECK-LABEL:  func.func @reshape_innermost_axis
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<4x2x3xf32>, [[PARAM_1_:%.+]]: tensor<4x2x3xf32>) -> tensor<4x12xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 1 : i32} : (tensor<4x2x3xf32>, tensor<4x2x3xf32>) -> tensor<4x4x3xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.reshape [[VAR_0_]] {new_shape = array<i64: 4, 12>} : (tensor<4x4x3xf32>) -> tensor<4x12xf32>
// CHECK:           return [[VAR_1_]] : tensor<4x12xf32>
// CHECK:         }
// -----

// Concat on the innermost axis of a 3D result produced from a 4D input: the
// concat sinks onto the input's innermost axis 3.
func.func @reshape_innermost_high_rank(%arg0: tensor<2x3x4x5xf32>, %arg1: tensor<2x3x4x5xf32>) -> tensor<2x12x10xf32> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 2, 12, 5>} : (tensor<2x3x4x5xf32>) -> tensor<2x12x5xf32>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 2, 12, 5>} : (tensor<2x3x4x5xf32>) -> tensor<2x12x5xf32>
  %2 = tosa.concat %0, %1 {axis = 2 : i32} : (tensor<2x12x5xf32>, tensor<2x12x5xf32>) -> tensor<2x12x10xf32>
  return %2 : tensor<2x12x10xf32>
}

// CHECK-LABEL:  func.func @reshape_innermost_high_rank
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<2x3x4x5xf32>, [[PARAM_1_:%.+]]: tensor<2x3x4x5xf32>) -> tensor<2x12x10xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 3 : i32} : (tensor<2x3x4x5xf32>, tensor<2x3x4x5xf32>) -> tensor<2x3x4x10xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.reshape [[VAR_0_]] {new_shape = array<i64: 2, 12, 10>} : (tensor<2x3x4x10xf32>) -> tensor<2x12x10xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x12x10xf32>
// CHECK:         }
// -----

// Concat on a middle axis (2) of a 4D result produced from a 5D input: the
// 4x5 group merged into 20 sinks the concat onto input axis 2.
func.func @reshape_mid_axis_high_rank(%arg0: tensor<2x3x4x5x6xf32>, %arg1: tensor<2x3x4x5x6xf32>) -> tensor<2x3x40x6xf32> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 2, 3, 20, 6>} : (tensor<2x3x4x5x6xf32>) -> tensor<2x3x20x6xf32>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 2, 3, 20, 6>} : (tensor<2x3x4x5x6xf32>) -> tensor<2x3x20x6xf32>
  %2 = tosa.concat %0, %1 {axis = 2 : i32} : (tensor<2x3x20x6xf32>, tensor<2x3x20x6xf32>) -> tensor<2x3x40x6xf32>
  return %2 : tensor<2x3x40x6xf32>
}

// CHECK-LABEL:  func.func @reshape_mid_axis_high_rank
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<2x3x4x5x6xf32>, [[PARAM_1_:%.+]]: tensor<2x3x4x5x6xf32>) -> tensor<2x3x40x6xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 2 : i32} : (tensor<2x3x4x5x6xf32>, tensor<2x3x4x5x6xf32>) -> tensor<2x3x8x5x6xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.reshape [[VAR_0_]] {new_shape = array<i64: 2, 3, 40, 6>} : (tensor<2x3x8x5x6xf32>) -> tensor<2x3x40x6xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x3x40x6xf32>
// CHECK:         }
// -----

// Concat on the innermost axis (3) of a 4D result produced from a 5D input:
// the trailing 5x6 merged into 30 sinks the concat onto input axis 3.
func.func @reshape_axis3_merge(%arg0: tensor<2x3x4x5x6xf32>, %arg1: tensor<2x3x4x5x6xf32>) -> tensor<2x3x4x60xf32> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 2, 3, 4, 30>} : (tensor<2x3x4x5x6xf32>) -> tensor<2x3x4x30xf32>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 2, 3, 4, 30>} : (tensor<2x3x4x5x6xf32>) -> tensor<2x3x4x30xf32>
  %2 = tosa.concat %0, %1 {axis = 3 : i32} : (tensor<2x3x4x30xf32>, tensor<2x3x4x30xf32>) -> tensor<2x3x4x60xf32>
  return %2 : tensor<2x3x4x60xf32>
}

// CHECK-LABEL:  func.func @reshape_axis3_merge
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<2x3x4x5x6xf32>, [[PARAM_1_:%.+]]: tensor<2x3x4x5x6xf32>) -> tensor<2x3x4x60xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 3 : i32} : (tensor<2x3x4x5x6xf32>, tensor<2x3x4x5x6xf32>) -> tensor<2x3x4x10x6xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.reshape [[VAR_0_]] {new_shape = array<i64: 2, 3, 4, 60>} : (tensor<2x3x4x10x6xf32>) -> tensor<2x3x4x60xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x3x4x60xf32>
// CHECK:         }
// -----

// Rank-increasing reshape (3D->4D, inserts a leading unit dim into the result)
// with concat on axis 2: the concat sinks onto input axis 1.
func.func @reshape_unsqueeze_mid_axis(%arg0: tensor<2x3x4xf32>, %arg1: tensor<2x3x4xf32>) -> tensor<2x1x6x4xf32> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 2, 1, 3, 4>} : (tensor<2x3x4xf32>) -> tensor<2x1x3x4xf32>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 2, 1, 3, 4>} : (tensor<2x3x4xf32>) -> tensor<2x1x3x4xf32>
  %2 = tosa.concat %0, %1 {axis = 2 : i32} : (tensor<2x1x3x4xf32>, tensor<2x1x3x4xf32>) -> tensor<2x1x6x4xf32>
  return %2 : tensor<2x1x6x4xf32>
}

// CHECK-LABEL:  func.func @reshape_unsqueeze_mid_axis
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<2x3x4xf32>, [[PARAM_1_:%.+]]: tensor<2x3x4xf32>) -> tensor<2x1x6x4xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 1 : i32} : (tensor<2x3x4xf32>, tensor<2x3x4xf32>) -> tensor<2x6x4xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.reshape [[VAR_0_]] {new_shape = array<i64: 2, 1, 6, 4>} : (tensor<2x6x4xf32>) -> tensor<2x1x6x4xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x1x6x4xf32>
// CHECK:         }
// -----

// Adapter path on the innermost axis (2): two majority operands split 256 as
// 16x16, the minority splits 32x8 and is reshaped to the majority layout
// before a single high-rank concat plus one trailing reshape.
func.func @reshape_adapter_innermost(%arg0: tensor<2x64x16x16xf32>, %arg1: tensor<2x64x16x16xf32>, %arg2: tensor<2x64x32x8xf32>) -> tensor<2x64x768xf32> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 2, 64, 256>} : (tensor<2x64x16x16xf32>) -> tensor<2x64x256xf32>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 2, 64, 256>} : (tensor<2x64x16x16xf32>) -> tensor<2x64x256xf32>
  %2 = tosa.reshape %arg2 {new_shape = array<i64: 2, 64, 256>} : (tensor<2x64x32x8xf32>) -> tensor<2x64x256xf32>
  %3 = tosa.concat %0, %1, %2 {axis = 2 : i32} : (tensor<2x64x256xf32>, tensor<2x64x256xf32>, tensor<2x64x256xf32>) -> tensor<2x64x768xf32>
  return %3 : tensor<2x64x768xf32>
}

// CHECK-LABEL:  func.func @reshape_adapter_innermost
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<2x64x16x16xf32>, [[PARAM_1_:%.+]]: tensor<2x64x16x16xf32>, [[PARAM_2_:%.+]]: tensor<2x64x32x8xf32>) -> tensor<2x64x768xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.reshape [[PARAM_2_]] {new_shape = array<i64: 2, 64, 16, 16>} : (tensor<2x64x32x8xf32>) -> tensor<2x64x16x16xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[VAR_0_]] {axis = 2 : i32} : (tensor<2x64x16x16xf32>, tensor<2x64x16x16xf32>, tensor<2x64x16x16xf32>) -> tensor<2x64x48x16xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.reshape [[VAR_1_]] {new_shape = array<i64: 2, 64, 768>} : (tensor<2x64x48x16xf32>) -> tensor<2x64x768xf32>
// CHECK:           return [[VAR_2_]] : tensor<2x64x768xf32>
// CHECK:         }
// -----

// Adapter path on a middle axis (2) with a trailing suffix (5D inputs): only
// the minority 32x8 operand is reshaped.
func.func @reshape_adapter_mid_axis(%arg0: tensor<2x64x16x16x5xf32>, %arg1: tensor<2x64x16x16x5xf32>, %arg2: tensor<2x64x32x8x5xf32>) -> tensor<2x64x768x5xf32> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 2, 64, 256, 5>} : (tensor<2x64x16x16x5xf32>) -> tensor<2x64x256x5xf32>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 2, 64, 256, 5>} : (tensor<2x64x16x16x5xf32>) -> tensor<2x64x256x5xf32>
  %2 = tosa.reshape %arg2 {new_shape = array<i64: 2, 64, 256, 5>} : (tensor<2x64x32x8x5xf32>) -> tensor<2x64x256x5xf32>
  %3 = tosa.concat %0, %1, %2 {axis = 2 : i32} : (tensor<2x64x256x5xf32>, tensor<2x64x256x5xf32>, tensor<2x64x256x5xf32>) -> tensor<2x64x768x5xf32>
  return %3 : tensor<2x64x768x5xf32>
}

// CHECK-LABEL:  func.func @reshape_adapter_mid_axis
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<2x64x16x16x5xf32>, [[PARAM_1_:%.+]]: tensor<2x64x16x16x5xf32>, [[PARAM_2_:%.+]]: tensor<2x64x32x8x5xf32>) -> tensor<2x64x768x5xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.reshape [[PARAM_2_]] {new_shape = array<i64: 2, 64, 16, 16, 5>} : (tensor<2x64x32x8x5xf32>) -> tensor<2x64x16x16x5xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[VAR_0_]] {axis = 2 : i32} : (tensor<2x64x16x16x5xf32>, tensor<2x64x16x16x5xf32>, tensor<2x64x16x16x5xf32>) -> tensor<2x64x48x16x5xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.reshape [[VAR_1_]] {new_shape = array<i64: 2, 64, 768, 5>} : (tensor<2x64x48x16x5xf32>) -> tensor<2x64x768x5xf32>
// CHECK:           return [[VAR_2_]] : tensor<2x64x768x5xf32>
// CHECK:         }
// -----

// Adapter path on the innermost axis (3) of a 4D result (5D inputs): the
// minority 32x8 operand is reshaped to the 16x16 majority layout.
func.func @reshape_adapter_axis3(%arg0: tensor<2x4x8x16x16xf32>, %arg1: tensor<2x4x8x16x16xf32>, %arg2: tensor<2x4x8x32x8xf32>) -> tensor<2x4x8x768xf32> {
  %0 = tosa.reshape %arg0 {new_shape = array<i64: 2, 4, 8, 256>} : (tensor<2x4x8x16x16xf32>) -> tensor<2x4x8x256xf32>
  %1 = tosa.reshape %arg1 {new_shape = array<i64: 2, 4, 8, 256>} : (tensor<2x4x8x16x16xf32>) -> tensor<2x4x8x256xf32>
  %2 = tosa.reshape %arg2 {new_shape = array<i64: 2, 4, 8, 256>} : (tensor<2x4x8x32x8xf32>) -> tensor<2x4x8x256xf32>
  %3 = tosa.concat %0, %1, %2 {axis = 3 : i32} : (tensor<2x4x8x256xf32>, tensor<2x4x8x256xf32>, tensor<2x4x8x256xf32>) -> tensor<2x4x8x768xf32>
  return %3 : tensor<2x4x8x768xf32>
}

// CHECK-LABEL:  func.func @reshape_adapter_axis3
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<2x4x8x16x16xf32>, [[PARAM_1_:%.+]]: tensor<2x4x8x16x16xf32>, [[PARAM_2_:%.+]]: tensor<2x4x8x32x8xf32>) -> tensor<2x4x8x768xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.reshape [[PARAM_2_]] {new_shape = array<i64: 2, 4, 8, 16, 16>} : (tensor<2x4x8x32x8xf32>) -> tensor<2x4x8x16x16xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[VAR_0_]] {axis = 3 : i32} : (tensor<2x4x8x16x16xf32>, tensor<2x4x8x16x16xf32>, tensor<2x4x8x16x16xf32>) -> tensor<2x4x8x48x16xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.reshape [[VAR_1_]] {new_shape = array<i64: 2, 4, 8, 768>} : (tensor<2x4x8x48x16xf32>) -> tensor<2x4x8x768xf32>
// CHECK:           return [[VAR_2_]] : tensor<2x4x8x768xf32>
// CHECK:         }
// -----

// valid tests for all supported operations

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.abs %arg0 : (!in_type) -> !in_type
  %1 = tosa.abs %arg1 : (!in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.abs [[VAR_0_]] : (tensor<2x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x8x8xf32>
// CHECK:         }

// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.bitwise_not %arg0 : (!in_type) -> !in_type
  %1 = tosa.bitwise_not %arg1 : (!in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.bitwise_not [[VAR_0_]] : (tensor<2x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.ceil %arg0 : (!in_type) -> !in_type
  %1 = tosa.ceil %arg1 : (!in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.ceil [[VAR_0_]] : (tensor<2x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.clz %arg0 : (!in_type) -> !in_type
  %1 = tosa.clz %arg1 : (!in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.clz [[VAR_0_]] : (tensor<2x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.cos %arg0 : (!in_type) -> !in_type
  %1 = tosa.cos %arg1 : (!in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.cos [[VAR_0_]] : (tensor<2x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x8x8xf32>
// CHECK:         }
// -----


!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.erf %arg0 : (!in_type) -> !in_type
  %1 = tosa.erf %arg1 : (!in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.erf [[VAR_0_]] : (tensor<2x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.exp %arg0 : (!in_type) -> !in_type
  %1 = tosa.exp %arg1 : (!in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.exp [[VAR_0_]] : (tensor<2x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.floor %arg0 : (!in_type) -> !in_type
  %1 = tosa.floor %arg1 : (!in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.floor [[VAR_0_]] : (tensor<2x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.log %arg0 : (!in_type) -> !in_type
  %1 = tosa.log %arg1 : (!in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.log [[VAR_0_]] : (tensor<2x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xi1>
!out_type = tensor<2x8x8xi1>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.logical_not %arg0 : (!in_type) -> !in_type
  %1 = tosa.logical_not %arg1 : (!in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xi1>, [[PARAM_1_:%.+]]: tensor<1x8x8xi1>) -> tensor<2x8x8xi1> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<2x8x8xi1>
// CHECK:           [[VAR_1_:%.+]] = tosa.logical_not [[VAR_0_]] : (tensor<2x8x8xi1>) -> tensor<2x8x8xi1>
// CHECK:           return [[VAR_1_]] : tensor<2x8x8xi1>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.negate %arg0 : (!in_type) -> !in_type
  %1 = tosa.negate %arg1 : (!in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.negate [[VAR_0_]] : (tensor<2x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.reciprocal %arg0 : (!in_type) -> !in_type
  %1 = tosa.reciprocal %arg1 : (!in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.reciprocal [[VAR_0_]] : (tensor<2x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.rsqrt %arg0 : (!in_type) -> !in_type
  %1 = tosa.rsqrt %arg1 : (!in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.rsqrt [[VAR_0_]] : (tensor<2x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.sigmoid %arg0 : (!in_type) -> !in_type
  %1 = tosa.sigmoid %arg1 : (!in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.sigmoid [[VAR_0_]] : (tensor<2x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.sin %arg0 : (!in_type) -> !in_type
  %1 = tosa.sin %arg1 : (!in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.sin [[VAR_0_]] : (tensor<2x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.tanh %arg0 : (!in_type) -> !in_type
  %1 = tosa.tanh %arg1 : (!in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.tanh [[VAR_0_]] : (tensor<2x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xi1>
!out_type = tensor<3x8x8xi1>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.equal %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %1 = tosa.equal %arg1, %arg0 : (!in_type, !in_type) -> !in_type
  %2 = tosa.equal %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %3 = tosa.concat %0, %1, %2 {axis = 0 : i32} : (!in_type, !in_type, !in_type) -> !out_type
  return %3 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xi1>, [[PARAM_1_:%.+]]: tensor<1x8x8xi1>) -> tensor<3x8x8xi1> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK:           [[VAR_2_:%.+]] = tosa.equal [[VAR_0_]], [[VAR_1_]] : (tensor<3x8x8xi1>, tensor<3x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK:           return [[VAR_2_]] : tensor<3x8x8xi1>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xi1>
!out_type = tensor<3x8x8xi1>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.greater %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %1 = tosa.greater %arg1, %arg0 : (!in_type, !in_type) -> !in_type
  %2 = tosa.greater %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %3 = tosa.concat %0, %1, %2 {axis = 0 : i32} : (!in_type, !in_type, !in_type) -> !out_type
  return %3 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xi1>, [[PARAM_1_:%.+]]: tensor<1x8x8xi1>) -> tensor<3x8x8xi1> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK:           [[VAR_2_:%.+]] = tosa.greater [[VAR_0_]], [[VAR_1_]] : (tensor<3x8x8xi1>, tensor<3x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK:           return [[VAR_2_]] : tensor<3x8x8xi1>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xi1>
!out_type = tensor<3x8x8xi1>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.greater_equal %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %1 = tosa.greater_equal %arg1, %arg0 : (!in_type, !in_type) -> !in_type
  %2 = tosa.greater_equal %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %3 = tosa.concat %0, %1, %2 {axis = 0 : i32} : (!in_type, !in_type, !in_type) -> !out_type
  return %3 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xi1>, [[PARAM_1_:%.+]]: tensor<1x8x8xi1>) -> tensor<3x8x8xi1> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK:           [[VAR_2_:%.+]] = tosa.greater_equal [[VAR_0_]], [[VAR_1_]] : (tensor<3x8x8xi1>, tensor<3x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK:           return [[VAR_2_]] : tensor<3x8x8xi1>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xi1>
!out_type = tensor<3x8x8xi1>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.logical_and %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %1 = tosa.logical_and %arg1, %arg0 : (!in_type, !in_type) -> !in_type
  %2 = tosa.logical_and %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %3 = tosa.concat %0, %1, %2 {axis = 0 : i32} : (!in_type, !in_type, !in_type) -> !out_type
  return %3 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xi1>, [[PARAM_1_:%.+]]: tensor<1x8x8xi1>) -> tensor<3x8x8xi1> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK:           [[VAR_2_:%.+]] = tosa.logical_and [[VAR_0_]], [[VAR_1_]] : (tensor<3x8x8xi1>, tensor<3x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK:           return [[VAR_2_]] : tensor<3x8x8xi1>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xi1>
!out_type = tensor<3x8x8xi1>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.logical_or %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %1 = tosa.logical_or %arg1, %arg0 : (!in_type, !in_type) -> !in_type
  %2 = tosa.logical_or %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %3 = tosa.concat %0, %1, %2 {axis = 0 : i32} : (!in_type, !in_type, !in_type) -> !out_type
  return %3 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xi1>, [[PARAM_1_:%.+]]: tensor<1x8x8xi1>) -> tensor<3x8x8xi1> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK:           [[VAR_2_:%.+]] = tosa.logical_or [[VAR_0_]], [[VAR_1_]] : (tensor<3x8x8xi1>, tensor<3x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK:           return [[VAR_2_]] : tensor<3x8x8xi1>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<2x8x8xf32>
!select_type = tensor<1x8x8xi1>
func.func @switch_op_concat(%arg0: !select_type, %arg1: !in_type, %arg2: !in_type) -> !out_type {
  %0 = tosa.select %arg0, %arg1, %arg2 : (!select_type, !in_type, !in_type) -> !in_type
  %1 = tosa.select %arg0, %arg2, %arg1 : (!select_type, !in_type, !in_type) -> !in_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!in_type, !in_type) -> !out_type
  return %2 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xi1>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>, [[PARAM_2_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x8x8xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<2x8x8xi1>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_2_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK-DAG:       [[VAR_2_:%.+]] = tosa.concat [[PARAM_2_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_3_:%.+]] = tosa.select [[VAR_0_]], [[VAR_1_]], [[VAR_2_]] : (tensor<2x8x8xi1>, tensor<2x8x8xf32>, tensor<2x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           return [[VAR_3_]] : tensor<2x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xi32>
!out_type = tensor<3x8x8xi32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.arithmetic_right_shift %arg0, %arg1 {round = false} : (!in_type, !in_type) -> !in_type
  %1 = tosa.arithmetic_right_shift %arg1, %arg0 {round = false} : (!in_type, !in_type) -> !in_type
  %2 = tosa.arithmetic_right_shift %arg0, %arg1 {round = false} : (!in_type, !in_type) -> !in_type
  %3 = tosa.concat %0, %1, %2 {axis = 0 : i32} : (!in_type, !in_type, !in_type) -> !out_type
  return %3 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xi32>, [[PARAM_1_:%.+]]: tensor<1x8x8xi32>) -> tensor<3x8x8xi32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xi32>, tensor<1x8x8xi32>, tensor<1x8x8xi32>) -> tensor<3x8x8xi32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xi32>, tensor<1x8x8xi32>, tensor<1x8x8xi32>) -> tensor<3x8x8xi32>
// CHECK:           [[VAR_2_:%.+]] = tosa.arithmetic_right_shift [[VAR_0_]], [[VAR_1_]] {round = false} : (tensor<3x8x8xi32>, tensor<3x8x8xi32>) -> tensor<3x8x8xi32>
// CHECK:           return [[VAR_2_]] : tensor<3x8x8xi32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xi1>
!out_type = tensor<3x8x8xi1>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.bitwise_and %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %1 = tosa.bitwise_and %arg1, %arg0 : (!in_type, !in_type) -> !in_type
  %2 = tosa.bitwise_and %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %3 = tosa.concat %0, %1, %2 {axis = 0 : i32} : (!in_type, !in_type, !in_type) -> !out_type
  return %3 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xi1>, [[PARAM_1_:%.+]]: tensor<1x8x8xi1>) -> tensor<3x8x8xi1> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK:           [[VAR_2_:%.+]] = tosa.bitwise_and [[VAR_0_]], [[VAR_1_]] : (tensor<3x8x8xi1>, tensor<3x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK:           return [[VAR_2_]] : tensor<3x8x8xi1>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xi1>
!out_type = tensor<3x8x8xi1>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.bitwise_or %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %1 = tosa.bitwise_or %arg1, %arg0 : (!in_type, !in_type) -> !in_type
  %2 = tosa.bitwise_or %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %3 = tosa.concat %0, %1, %2 {axis = 0 : i32} : (!in_type, !in_type, !in_type) -> !out_type
  return %3 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xi1>, [[PARAM_1_:%.+]]: tensor<1x8x8xi1>) -> tensor<3x8x8xi1> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK:           [[VAR_2_:%.+]] = tosa.bitwise_or [[VAR_0_]], [[VAR_1_]] : (tensor<3x8x8xi1>, tensor<3x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK:           return [[VAR_2_]] : tensor<3x8x8xi1>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xi1>
!out_type = tensor<3x8x8xi1>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.bitwise_xor %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %1 = tosa.bitwise_xor %arg1, %arg0 : (!in_type, !in_type) -> !in_type
  %2 = tosa.bitwise_xor %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %3 = tosa.concat %0, %1, %2 {axis = 0 : i32} : (!in_type, !in_type, !in_type) -> !out_type
  return %3 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xi1>, [[PARAM_1_:%.+]]: tensor<1x8x8xi1>) -> tensor<3x8x8xi1> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK:           [[VAR_2_:%.+]] = tosa.bitwise_xor [[VAR_0_]], [[VAR_1_]] : (tensor<3x8x8xi1>, tensor<3x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK:           return [[VAR_2_]] : tensor<3x8x8xi1>
// CHECK:         }
// -----
!in_type = tensor<1x8x8xi32>
!out_type = tensor<3x8x8xi32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.int_div %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %1 = tosa.int_div %arg1, %arg0 : (!in_type, !in_type) -> !in_type
  %2 = tosa.int_div %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %3 = tosa.concat %0, %1, %2 {axis = 0 : i32} : (!in_type, !in_type, !in_type) -> !out_type
  return %3 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xi32>, [[PARAM_1_:%.+]]: tensor<1x8x8xi32>) -> tensor<3x8x8xi32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xi32>, tensor<1x8x8xi32>, tensor<1x8x8xi32>) -> tensor<3x8x8xi32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xi32>, tensor<1x8x8xi32>, tensor<1x8x8xi32>) -> tensor<3x8x8xi32>
// CHECK:           [[VAR_2_:%.+]] = tosa.int_div [[VAR_0_]], [[VAR_1_]] : (tensor<3x8x8xi32>, tensor<3x8x8xi32>) -> tensor<3x8x8xi32>
// CHECK:           return [[VAR_2_]] : tensor<3x8x8xi32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<3x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.logical_left_shift %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %1 = tosa.logical_left_shift %arg1, %arg0 : (!in_type, !in_type) -> !in_type
  %2 = tosa.logical_left_shift %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %3 = tosa.concat %0, %1, %2 {axis = 0 : i32} : (!in_type, !in_type, !in_type) -> !out_type
  return %3 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<3x8x8xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.logical_left_shift [[VAR_0_]], [[VAR_1_]] : (tensor<3x8x8xf32>, tensor<3x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK:           return [[VAR_2_]] : tensor<3x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<3x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.logical_right_shift %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %1 = tosa.logical_right_shift %arg1, %arg0 : (!in_type, !in_type) -> !in_type
  %2 = tosa.logical_right_shift %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %3 = tosa.concat %0, %1, %2 {axis = 0 : i32} : (!in_type, !in_type, !in_type) -> !out_type
  return %3 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<3x8x8xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.logical_right_shift [[VAR_0_]], [[VAR_1_]] : (tensor<3x8x8xf32>, tensor<3x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK:           return [[VAR_2_]] : tensor<3x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xi1>
!out_type = tensor<3x8x8xi1>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.logical_xor %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %1 = tosa.logical_xor %arg1, %arg0 : (!in_type, !in_type) -> !in_type
  %2 = tosa.logical_xor %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %3 = tosa.concat %0, %1, %2 {axis = 0 : i32} : (!in_type, !in_type, !in_type) -> !out_type
  return %3 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xi1>, [[PARAM_1_:%.+]]: tensor<1x8x8xi1>) -> tensor<3x8x8xi1> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xi1>, tensor<1x8x8xi1>, tensor<1x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK:           [[VAR_2_:%.+]] = tosa.logical_xor [[VAR_0_]], [[VAR_1_]] : (tensor<3x8x8xi1>, tensor<3x8x8xi1>) -> tensor<3x8x8xi1>
// CHECK:           return [[VAR_2_]] : tensor<3x8x8xi1>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<3x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.maximum %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %1 = tosa.maximum %arg1, %arg0 : (!in_type, !in_type) -> !in_type
  %2 = tosa.maximum %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %3 = tosa.concat %0, %1, %2 {axis = 0 : i32} : (!in_type, !in_type, !in_type) -> !out_type
  return %3 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<3x8x8xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.maximum [[VAR_0_]], [[VAR_1_]] : (tensor<3x8x8xf32>, tensor<3x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK:           return [[VAR_2_]] : tensor<3x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<3x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.minimum %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %1 = tosa.minimum %arg1, %arg0 : (!in_type, !in_type) -> !in_type
  %2 = tosa.minimum %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %3 = tosa.concat %0, %1, %2 {axis = 0 : i32} : (!in_type, !in_type, !in_type) -> !out_type
  return %3 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<3x8x8xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.minimum [[VAR_0_]], [[VAR_1_]] : (tensor<3x8x8xf32>, tensor<3x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK:           return [[VAR_2_]] : tensor<3x8x8xf32>
// CHECK:         }
// -----
!in_type = tensor<1x8x8xf32>
!out_type = tensor<3x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.pow %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %1 = tosa.pow %arg1, %arg0 : (!in_type, !in_type) -> !in_type
  %2 = tosa.pow %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %3 = tosa.concat %0, %1, %2 {axis = 0 : i32} : (!in_type, !in_type, !in_type) -> !out_type
  return %3 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<3x8x8xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.pow [[VAR_0_]], [[VAR_1_]] : (tensor<3x8x8xf32>, tensor<3x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK:           return [[VAR_2_]] : tensor<3x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<3x8x8xf32>
func.func @switch_op_concat(%arg0: !in_type, %arg1: !in_type) -> !out_type {
  %0 = tosa.sub %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %1 = tosa.sub %arg1, %arg0 : (!in_type, !in_type) -> !in_type
  %2 = tosa.sub %arg0, %arg1 : (!in_type, !in_type) -> !in_type
  %3 = tosa.concat %0, %1, %2 {axis = 0 : i32} : (!in_type, !in_type, !in_type) -> !out_type
  return %3 : !out_type
}
// CHECK-LABEL:  func.func @switch_op_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<3x8x8xf32> {
// CHECK-DAG:       [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_0_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_1_]], [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.sub [[VAR_0_]], [[VAR_1_]] : (tensor<3x8x8xf32>, tensor<3x8x8xf32>) -> tensor<3x8x8xf32>
// CHECK:           return [[VAR_2_]] : tensor<3x8x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<1x1x8xf32>
!concat_type = tensor<2x1x8xf32>
func.func @switch_op_concat_axis(%arg0 : !in_type, %arg1 : !in_type) -> !concat_type {
  %0 = tosa.reduce_all %arg0 {axis = 1 : i32} : (!in_type) -> !out_type 
  %1 = tosa.reduce_all %arg1 {axis = 1 : i32} : (!in_type) -> !out_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!out_type, !out_type) -> !concat_type
  return %2 : !concat_type
}
// CHECK-LABEL:  func.func @switch_op_concat_axis
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x1x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.reduce_all [[VAR_0_]] {axis = 1 : i32} : (tensor<2x8x8xf32>) -> tensor<2x1x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x1x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<1x1x8xf32>
!concat_type = tensor<2x1x8xf32>
func.func @switch_op_concat_axis(%arg0 : !in_type, %arg1 : !in_type) -> !concat_type {
  %0 = tosa.reduce_any %arg0 {axis = 1 : i32} : (!in_type) -> !out_type 
  %1 = tosa.reduce_any %arg1 {axis = 1 : i32} : (!in_type) -> !out_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!out_type, !out_type) -> !concat_type
  return %2 : !concat_type
}
// CHECK-LABEL:  func.func @switch_op_concat_axis
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x1x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.reduce_any [[VAR_0_]] {axis = 1 : i32} : (tensor<2x8x8xf32>) -> tensor<2x1x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x1x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<1x1x8xf32>
!concat_type = tensor<2x1x8xf32>
func.func @switch_op_concat_axis(%arg0 : !in_type, %arg1 : !in_type) -> !concat_type {
  %0 = tosa.reduce_min %arg0 {axis = 1 : i32} : (!in_type) -> !out_type 
  %1 = tosa.reduce_min %arg1 {axis = 1 : i32} : (!in_type) -> !out_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!out_type, !out_type) -> !concat_type
  return %2 : !concat_type
}
// CHECK-LABEL:  func.func @switch_op_concat_axis
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x1x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.reduce_min [[VAR_0_]] {axis = 1 : i32} : (tensor<2x8x8xf32>) -> tensor<2x1x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x1x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<1x1x8xf32>
!concat_type = tensor<2x1x8xf32>
func.func @switch_op_concat_axis(%arg0 : !in_type, %arg1 : !in_type) -> !concat_type {
  %0 = tosa.reduce_prod %arg0 {axis = 1 : i32} : (!in_type) -> !out_type 
  %1 = tosa.reduce_prod %arg1 {axis = 1 : i32} : (!in_type) -> !out_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!out_type, !out_type) -> !concat_type
  return %2 : !concat_type
}
// CHECK-LABEL:  func.func @switch_op_concat_axis
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x1x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.reduce_prod [[VAR_0_]] {axis = 1 : i32} : (tensor<2x8x8xf32>) -> tensor<2x1x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x1x8xf32>
// CHECK:         }
// -----

!in_type = tensor<1x8x8xf32>
!out_type = tensor<1x1x8xf32>
!concat_type = tensor<2x1x8xf32>
func.func @switch_op_concat_axis(%arg0 : !in_type, %arg1 : !in_type) -> !concat_type {
  %0 = tosa.reduce_sum %arg0 {axis = 1 : i32} : (!in_type) -> !out_type 
  %1 = tosa.reduce_sum %arg1 {axis = 1 : i32} : (!in_type) -> !out_type
  %2 = tosa.concat %0, %1 {axis = 0 : i32} : (!out_type, !out_type) -> !concat_type
  return %2 : !concat_type
}
// CHECK-LABEL:  func.func @switch_op_concat_axis
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x8x8xf32>) -> tensor<2x1x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 0 : i32} : (tensor<1x8x8xf32>, tensor<1x8x8xf32>) -> tensor<2x8x8xf32>
// CHECK:           [[VAR_1_:%.+]] = tosa.reduce_sum [[VAR_0_]] {axis = 1 : i32} : (tensor<2x8x8xf32>) -> tensor<2x1x8xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x1x8xf32>
// CHECK:         }
