// RUN: mlir-opt %s --split-input-file --annotate-function-type | FileCheck %s

// CHECK-LABEL: func.func @one_arg(%arg0: tensor<f32> {func.orig_type = tensor<f32>}) -> (tensor<f32> {func.orig_type = tensor<f32>}) {
func.func @one_arg(%arg0: tensor<f32>) -> tensor<f32> {
  return %arg0 : tensor<f32>
}

// -----

// CHECK-LABEL: func.func @one_arg_int(%arg0: tensor<ui8> {func.orig_type = tensor<ui8>}) -> (tensor<ui8> {func.orig_type = tensor<ui8>}) {
func.func @one_arg_int(%arg0: tensor<ui8>) -> tensor<ui8> {
  return %arg0 : tensor<ui8>
}

// -----

// CHECK-LABEL: func.func @n_rank_tensor(%arg0: tensor<3x4x5xf32> {func.orig_type = tensor<3x4x5xf32>}) -> (tensor<3x4x5xf32> {func.orig_type = tensor<3x4x5xf32>}) {
func.func @n_rank_tensor(%arg0: tensor<3x4x5xf32>) -> tensor<3x4x5xf32> {
  return %arg0 : tensor<3x4x5xf32>
}

// -----

// CHECK-LABEL: func.func @two_args(%arg0: f32 {func.orig_type = f32}, %arg1: f32 {func.orig_type = f32}) -> (f32 {func.orig_type = f32}) {
func.func @two_args(%arg0: f32, %arg1: f32) -> f32 {
  return %arg0 : f32
}
