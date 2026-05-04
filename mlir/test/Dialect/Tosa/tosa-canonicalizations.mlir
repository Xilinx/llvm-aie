// RUN: mlir-opt --split-input-file -canonicalize="test-convergence" %s | FileCheck %s

// Test cast folding for the no-op scenarios:
//   f16 -> f32 -> f16
//   bf16 -> f32 -> bf16

// CHECK-LABEL: @test_cast_bf16
func.func @test_cast_bf16(%arg0: tensor<1x3x3xf32>) -> tensor<1x3x3xbf16> {
    %0 = tosa.cast %arg0 : (tensor<1x3x3xf32>) -> tensor<1x3x3xbf16>
    %1 = tosa.cast %0 : (tensor<1x3x3xbf16>) -> tensor<1x3x3xf32>
    %2 = tosa.cast %1 : (tensor<1x3x3xf32>) -> tensor<1x3x3xbf16>
    return %2 : tensor<1x3x3xbf16>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<1x3x3xf32>) -> tensor<1x3x3xbf16>
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<1x3x3xf32>) -> tensor<1x3x3xbf16>
// CHECK-NEXT:  return {{.*}} : tensor<1x3x3xbf16>

// -----

// CHECK-LABEL: @test_cast_f16
func.func @test_cast_f16(%arg0: tensor<1x3x3xf32>) -> tensor<1x3x3xf32> {
    %0 = tosa.cast %arg0 : (tensor<1x3x3xf32>) -> tensor<1x3x3xf16>
    %1 = tosa.cast %0 : (tensor<1x3x3xf16>) -> tensor<1x3x3xf32>
    %2 = tosa.cast %1 : (tensor<1x3x3xf32>) -> tensor<1x3x3xf16>
    %3 = tosa.cast %2 : (tensor<1x3x3xf16>) -> tensor<1x3x3xf32>
    return %3 : tensor<1x3x3xf32>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<1x3x3xf32>) -> tensor<1x3x3xf32>
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<1x3x3xf32>) -> tensor<1x3x3xf16>
// CHECK-NEXT:  tosa.cast {{.*}} : (tensor<1x3x3xf16>) -> tensor<1x3x3xf32>
// CHECK-NEXT:  return {{.*}} : tensor<1x3x3xf32>


// -----

// CHECK-LABEL: @test_cast_unequal_types
func.func @test_cast_unequal_types(%arg0: tensor<1x3x3xf32>) -> tensor<1x3x3xf32> {
    %0 = tosa.cast %arg0 : (tensor<1x3x3xf32>) -> tensor<1x3x3xf16>
    %1 = tosa.cast %0 : (tensor<1x3x3xf16>) -> tensor<1x3x3xf32>
    %2 = tosa.cast %1 : (tensor<1x3x3xf32>) -> tensor<1x3x3xbf16>
    %3 = tosa.cast %2 : (tensor<1x3x3xbf16>) -> tensor<1x3x3xf32>
    return %3 : tensor<1x3x3xf32>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<1x3x3xf32>) -> tensor<1x3x3xf32>
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<1x3x3xf32>) -> tensor<1x3x3xf16>
// CHECK-NEXT:  tosa.cast {{.*}} : (tensor<1x3x3xf16>) -> tensor<1x3x3xf32>
// CHECK-NEXT:  tosa.cast {{.*}} : (tensor<1x3x3xf32>) -> tensor<1x3x3xbf16>
// CHECK-NEXT:  tosa.cast {{.*}} : (tensor<1x3x3xbf16>) -> tensor<1x3x3xf32>
// CHECK-NEXT:  return {{.*}} : tensor<1x3x3xf32>


// -----

// CHECK-LABEL: @test_cast_lossy_conversion
func.func @test_cast_lossy_conversion(%arg0: tensor<1x3x3xf32>) -> tensor<1x3x3xf32> {
    %0 = tosa.cast %arg0 : (tensor<1x3x3xf32>) -> tensor<1x3x3xf16>
    %1 = tosa.cast %0 : (tensor<1x3x3xf16>) -> tensor<1x3x3xf32>
    return %1 : tensor<1x3x3xf32>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<1x3x3xf32>) -> tensor<1x3x3xf32>
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<1x3x3xf32>) -> tensor<1x3x3xf16>
// CHECK-NEXT:  tosa.cast {{.*}} : (tensor<1x3x3xf16>) -> tensor<1x3x3xf32>
// CHECK-NEXT:  return {{.*}} : tensor<1x3x3xf32>