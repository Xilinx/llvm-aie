// RUN: mlir-opt %s | mlir-opt | FileCheck %s
// AMD extention to allow any rank in tosa reduction operations

// -----
// CHECK-LABEL: reduce_all
func.func @test_reduce_all(%arg0: tensor<13x21x3x5x9xi1>) -> tensor<1x21x3x5x9xi1> {
  %0 = tosa.reduce_all %arg0 {axis = 0 : i32} : (tensor<13x21x3x5x9xi1>) -> tensor<1x21x3x5x9xi1>
  return %0 : tensor<1x21x3x5x9xi1>
}

// -----
// CHECK-LABEL: reduce_any
func.func @test_reduce_any(%arg0: tensor<13x21x3x5x9xi1>) -> tensor<1x21x3x5x9xi1> {
  %0 = tosa.reduce_any %arg0 {axis = 0 : i32} : (tensor<13x21x3x5x9xi1>) -> tensor<1x21x3x5x9xi1>
  return %0 : tensor<1x21x3x5x9xi1>
}

// -----
// CHECK-LABEL: reduce_max
func.func @test_reduce_max(%arg0: tensor<13x21x3x5x9xf32>) -> tensor<1x21x3x5x9xf32> {
  %0 = tosa.reduce_max %arg0 {axis = 0 : i32} : (tensor<13x21x3x5x9xf32>) -> tensor<1x21x3x5x9xf32>
  return %0 : tensor<1x21x3x5x9xf32>
}

// -----
// CHECK-LABEL: reduce_min
func.func @test_reduce_min(%arg0: tensor<13x21x3x5x9xf32>) -> tensor<1x21x3x5x9xf32> {
  %0 = tosa.reduce_min %arg0 {axis = 0 : i32} : (tensor<13x21x3x5x9xf32>) -> tensor<1x21x3x5x9xf32>
  return %0 : tensor<1x21x3x5x9xf32>
}

// -----
// CHECK-LABEL: reduce_product
func.func @test_reduce_product(%arg0: tensor<13x21x3x5x9xf32>) -> tensor<1x21x3x5x9xf32> {
  %0 = tosa.reduce_prod %arg0 {axis = 0 : i32} : (tensor<13x21x3x5x9xf32>) -> tensor<1x21x3x5x9xf32>
  return %0 : tensor<1x21x3x5x9xf32>
}

// -----
// CHECK-LABEL: reduce_sum
func.func @test_reduce_sum(%arg0: tensor<13x21x3x5x9xf32>) -> tensor<1x21x3x5x9xf32> {
  %0 = tosa.reduce_sum %arg0 {axis = 0 : i32} : (tensor<13x21x3x5x9xf32>) -> tensor<1x21x3x5x9xf32>
  return %0 : tensor<1x21x3x5x9xf32>
}
