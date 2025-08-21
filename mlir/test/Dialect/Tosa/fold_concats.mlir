// RUN: mlir-opt --split-input-file --canonicalize %s | FileCheck %s

func.func @single_concat(%arg0: tensor<1x1x7x7xf32>) -> tensor<1x2x7x7xf32> {
  %0 = tosa.concat %arg0, %arg0 {axis = 1 : i32} : (tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>) -> tensor<1x2x7x7xf32>
  return %0 : tensor<1x2x7x7xf32>
}

// CHECK-LABEL:  func.func @single_concat
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x1x7x7xf32>) -> tensor<1x2x7x7xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.const_shape  {value = dense<[1, 2, 1, 1]> : tensor<4xindex>} : () -> !tosa.shape<4>
// CHECK:           [[VAR_1_:%.+]] = tosa.tile [[PARAM_0_]], [[VAR_0_]] : (tensor<1x1x7x7xf32>, !tosa.shape<4>) -> tensor<1x2x7x7xf32>
// CHECK:           return [[VAR_1_]] : tensor<1x2x7x7xf32>
// CHECK:         }

// -----

func.func @concat_different_axis(%arg0: tensor<1x1x7x7xf32>) -> tensor<2x2x7x7xf32> {
  %0 = tosa.concat %arg0, %arg0 {axis = 1 : i32} : (tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>) -> tensor<1x2x7x7xf32>
  %1 = tosa.concat %0, %0 {axis = 0 : i32} : (tensor<1x2x7x7xf32>, tensor<1x2x7x7xf32>) -> tensor<2x2x7x7xf32>
  return %1 : tensor<2x2x7x7xf32>
}

// CHECK-LABEL:  func.func @concat_different_axis
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x1x7x7xf32>) -> tensor<2x2x7x7xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.const_shape  {value = dense<[2, 2, 1, 1]> : tensor<4xindex>} : () -> !tosa.shape<4>
// CHECK:           [[VAR_1_:%.+]] = tosa.tile [[PARAM_0_]], [[VAR_0_]] : (tensor<1x1x7x7xf32>, !tosa.shape<4>) -> tensor<2x2x7x7xf32>
// CHECK:           return [[VAR_1_]] : tensor<2x2x7x7xf32>
// CHECK:         }

// -----

func.func @fold_concats(%arg0: tensor<1x1x7x7xf32>) -> tensor<1x4x7x7xf32> {
  %tmp = tensor.empty() : tensor<1x1x7x7xf32>
  %0 = tosa.concat %arg0, %arg0 {axis = 1 : i32} : (tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>) -> tensor<1x2x7x7xf32>
  %1 = tosa.concat %tmp, %0, %tmp {axis = 1 : i32} : (tensor<1x1x7x7xf32>, tensor<1x2x7x7xf32>, tensor<1x1x7x7xf32>) -> tensor<1x4x7x7xf32>
  return %1 : tensor<1x4x7x7xf32>
}

// CHECK-LABEL:   func.func @fold_concats(
// CHECK-SAME:                            %[[VAL_0:.*]]: tensor<1x1x7x7xf32>) -> tensor<1x4x7x7xf32> {
// CHECK:           %[[VAL_1:.*]] = tensor.empty() : tensor<1x1x7x7xf32>
// CHECK:           %[[VAL_2:.*]] = tosa.concat %[[VAL_1]], %[[VAL_0]], %[[VAL_0]], %[[VAL_1]] {axis = 1 : i32} : (tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>) -> tensor<1x4x7x7xf32>
// CHECK:           return %[[VAL_2]] : tensor<1x4x7x7xf32>
// CHECK:         }

// -----

func.func @nested_fold(%arg0: tensor<1x1x7x7xf32>) -> tensor<1x8x7x7xf32> {
  %tmp = tensor.empty() : tensor<1x1x7x7xf32>
  %0 = tosa.concat %arg0, %arg0 {axis = 1 : i32} : (tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>) -> tensor<1x2x7x7xf32>
  %1 = tosa.concat %tmp, %0, %tmp {axis = 1 : i32} : (tensor<1x1x7x7xf32>, tensor<1x2x7x7xf32>, tensor<1x1x7x7xf32>) -> tensor<1x4x7x7xf32>
  %2 = tosa.concat %1, %1 {axis = 1 : i32} : (tensor<1x4x7x7xf32>, tensor<1x4x7x7xf32>) -> tensor<1x8x7x7xf32>
  return %2 : tensor<1x8x7x7xf32>
}

// CHECK-LABEL:   func.func @nested_fold(
// CHECK-SAME:                           %[[VAL_0:.*]]: tensor<1x1x7x7xf32>) -> tensor<1x8x7x7xf32> {
// CHECK:           %[[VAL_1:.*]] = tensor.empty() : tensor<1x1x7x7xf32>
// CHECK:           %[[VAL_2:.*]] = tosa.concat %[[VAL_1]], %[[VAL_0]], %[[VAL_0]], %[[VAL_1]], %[[VAL_1]], %[[VAL_0]], %[[VAL_0]], %[[VAL_1]] {axis = 1 : i32} : (tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>) -> tensor<1x8x7x7xf32>
// CHECK:           return %[[VAL_2]] : tensor<1x8x7x7xf32>
// CHECK:         }

// -----

func.func @concat_multiple_users(%arg0: tensor<1x1x7x7xf32>, %arg1: tensor<1x1x7x7xf32>) -> (tensor<1x3x7x7xf32>, tensor<1x2x7x7xf32>) {
  %tmp = tensor.empty() : tensor<1x1x7x7xf32>
  %0 = tosa.concat %arg0, %arg1 {axis = 1 : i32} : (tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>) -> tensor<1x2x7x7xf32>
  %1 = tosa.concat %tmp, %0 {axis = 1 : i32} : (tensor<1x1x7x7xf32>, tensor<1x2x7x7xf32>) -> tensor<1x3x7x7xf32>
  %2 = tosa.add %0, %0 : (tensor<1x2x7x7xf32>, tensor<1x2x7x7xf32>) -> tensor<1x2x7x7xf32>
  return %1, %2 : tensor<1x3x7x7xf32>, tensor<1x2x7x7xf32>
}

// CHECK-LABEL:  func.func @concat_multiple_users
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x1x7x7xf32>, [[PARAM_1_:%.+]]: tensor<1x1x7x7xf32>) -> (tensor<1x3x7x7xf32>, tensor<1x2x7x7xf32>) {
// CHECK-DAG:       [[VAR_0_:%.+]] = tensor.empty() : tensor<1x1x7x7xf32>
// CHECK-DAG:       [[VAR_1_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]] {axis = 1 : i32} : (tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>) -> tensor<1x2x7x7xf32>
// CHECK-NOT: separator of consecutive DAGs
// CHECK-DAG:       [[VAR_2_:%.+]] = tosa.concat [[VAR_0_]], [[VAR_1_]] {axis = 1 : i32} : (tensor<1x1x7x7xf32>, tensor<1x2x7x7xf32>) -> tensor<1x3x7x7xf32>
// CHECK-DAG:       [[VAR_3_:%.+]] = tosa.add [[VAR_1_]], [[VAR_1_]] : (tensor<1x2x7x7xf32>, tensor<1x2x7x7xf32>) -> tensor<1x2x7x7xf32>
// CHECK:           return [[VAR_2_]], [[VAR_3_]] : tensor<1x3x7x7xf32>, tensor<1x2x7x7xf32>
// CHECK:         }

// -----

func.func @concat_diamond_shape(%arg0: tensor<1x1x7x7xf32>, %arg1: tensor<1x1x7x7xf32>, %arg2: tensor<1x1x7x7xf32>, %arg3: tensor<1x1x7x7xf32>) -> tensor<1x6x7x7xf32> {
  %tmp = tensor.empty() : tensor<1x1x7x7xf32>
  %0 = tosa.concat %arg0, %arg1 {axis = 1 : i32} : (tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>) -> tensor<1x2x7x7xf32>
  %1 = tosa.concat %0, %arg2 {axis = 1 : i32} : (tensor<1x2x7x7xf32>, tensor<1x1x7x7xf32>) -> tensor<1x3x7x7xf32>
  %2 = tosa.concat %0, %arg3 {axis = 1 : i32} : (tensor<1x2x7x7xf32>, tensor<1x1x7x7xf32>) -> tensor<1x3x7x7xf32>
  %3 = tosa.concat %1, %2 {axis = 1 : i32} : (tensor<1x3x7x7xf32>, tensor<1x3x7x7xf32>) -> tensor<1x6x7x7xf32>
  return %3 : tensor<1x6x7x7xf32>
}

// CHECK-LABEL:  func.func @concat_diamond_shape
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x1x7x7xf32>, [[PARAM_1_:%.+]]: tensor<1x1x7x7xf32>, [[PARAM_2_:%.+]]: tensor<1x1x7x7xf32>, [[PARAM_3_:%.+]]: tensor<1x1x7x7xf32>) -> tensor<1x6x7x7xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_1_]], [[PARAM_2_]], [[PARAM_0_]], [[PARAM_1_]], [[PARAM_3_]] {axis = 1 : i32} : (tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>) -> tensor<1x6x7x7xf32>
// CHECK:           return [[VAR_0_]] : tensor<1x6x7x7xf32>
// CHECK:         }

// -----

func.func @wide_fold(%arg0: tensor<1x1x7x7xf32>, %arg1: tensor<1x1x7x7xf32>) -> tensor<1x4x7x7xf32> {
  %0 = tosa.concat %arg0, %arg0 {axis = 1 : i32} : (tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>) -> tensor<1x2x7x7xf32>
  %1 = tosa.concat %arg1, %arg1 {axis = 1 : i32} : (tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>) -> tensor<1x2x7x7xf32>
  %2 = tosa.concat %0, %1 {axis = 1 : i32} : (tensor<1x2x7x7xf32>, tensor<1x2x7x7xf32>) -> tensor<1x4x7x7xf32>
  return %2 : tensor<1x4x7x7xf32>
}

// CHECK-LABEL:   func.func @wide_fold(
// CHECK-SAME:                         %[[VAL_0:.*]]: tensor<1x1x7x7xf32>,
// CHECK-SAME:                         %[[VAL_1:.*]]: tensor<1x1x7x7xf32>) -> tensor<1x4x7x7xf32> {
// CHECK:           %[[VAL_2:.*]] = tosa.concat %[[VAL_0]], %[[VAL_0]], %[[VAL_1]], %[[VAL_1]] {axis = 1 : i32} : (tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>, tensor<1x1x7x7xf32>) -> tensor<1x4x7x7xf32>
// CHECK:           return %[[VAL_2]] : tensor<1x4x7x7xf32>
// CHECK:         }

// -----

func.func @partially_foldable(%arg0: tensor<1x1x8x8xf32>, %arg1: tensor<1x2x4x8xf32>) -> tensor<1x4x8x8xf32> {
  %0 = tosa.concat %arg0, %arg0 {axis = 1 : i32} : (tensor<1x1x8x8xf32>, tensor<1x1x8x8xf32>) -> tensor<1x2x8x8xf32>
  %1 = tosa.concat %arg1, %arg1 {axis = 2 : i32} : (tensor<1x2x4x8xf32>, tensor<1x2x4x8xf32>) -> tensor<1x2x8x8xf32>
  %2 = tosa.concat %0, %1 {axis = 1 : i32} : (tensor<1x2x8x8xf32>, tensor<1x2x8x8xf32>) -> tensor<1x4x8x8xf32>
  return %2 : tensor<1x4x8x8xf32>
}

// CHECK-LABEL:  func.func @partially_foldable
// CHECK-SAME:   ([[PARAM_0_:%.+]]: tensor<1x1x8x8xf32>, [[PARAM_1_:%.+]]: tensor<1x2x4x8xf32>) -> tensor<1x4x8x8xf32> {
// CHECK:           [[VAR_0_:%.+]] = tosa.const_shape  {value = dense<[1, 1, 2, 1]> : tensor<4xindex>} : () -> !tosa.shape<4>
// CHECK:           [[VAR_1_:%.+]] = tosa.tile [[PARAM_1_]], [[VAR_0_]] : (tensor<1x2x4x8xf32>, !tosa.shape<4>) -> tensor<1x2x8x8xf32>
// CHECK:           [[VAR_2_:%.+]] = tosa.concat [[PARAM_0_]], [[PARAM_0_]], [[VAR_1_]] {axis = 1 : i32} : (tensor<1x1x8x8xf32>, tensor<1x1x8x8xf32>, tensor<1x2x8x8xf32>) -> tensor<1x4x8x8xf32>
// CHECK:           return [[VAR_2_]] : tensor<1x4x8x8xf32>
// CHECK:         }
