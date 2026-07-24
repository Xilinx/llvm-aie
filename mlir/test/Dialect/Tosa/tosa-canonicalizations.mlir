// RUN: mlir-opt --split-input-file -canonicalize="test-convergence" %s | FileCheck %s

// Test cast folding for the scenarios involving a widening cast followed
// by a narrowing cast.

// f32 -> bf16 -> f32 -> bf16 is folded to f32 -> bf16
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

// f32 -> f16 -> f32 -> f16 -> f32 is folded to f32 -> f16 -> f32
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

// f32 -> f16 -> f32 -> bf16 -> f32 fold only the f16 -> f32 -> bf16
// part to f16 -> bf16. Other casts are not folded. f16->bf16->f32 does not
// fold since the intermediate bf16 cannot fully represent f16.
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
// CHECK-NEXT:  tosa.cast {{.*}} : (tensor<1x3x3xf16>) -> tensor<1x3x3xbf16>
// CHECK-NEXT:  tosa.cast {{.*}} : (tensor<1x3x3xbf16>) -> tensor<1x3x3xf32>
// CHECK-NEXT:  return {{.*}} : tensor<1x3x3xf32>


// -----

// f32 -> f16 -> f32 is not folded as the first conversion is a narrowing one.
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


// -----

// f32 -> bf16 -> f32 is not folded as the first conversion is a narrowing one.
// CHECK-LABEL: @no_fold_same_src_dst
func.func @no_fold_same_src_dst(%arg0: tensor<4xf32>) -> tensor<4xf32> {
  %0 = tosa.cast %arg0 : (tensor<4xf32>) -> tensor<4xbf16>
  %1 = tosa.cast %0 : (tensor<4xbf16>) -> tensor<4xf32>
  return %1 : tensor<4xf32>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<4xf32>) -> tensor<4xf32>
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<4xf32>) -> tensor<4xbf16>
// CHECK-NEXT:  tosa.cast {{.*}} : (tensor<4xbf16>) -> tensor<4xf32>
// CHECK-NEXT:  return {{.*}} : tensor<4xf32>

// -----

// i1 -> i64 -> i8 is folded to i1 -> i8
// CHECK-LABEL: @fold_widening_then_narrowing
func.func @fold_widening_then_narrowing(%arg0: tensor<4xi1>) -> tensor<4xi8> {
  %0 = tosa.cast %arg0 : (tensor<4xi1>) -> tensor<4xi64>
  %1 = tosa.cast %0 : (tensor<4xi64>) -> tensor<4xi8>
  return %1 : tensor<4xi8>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<4xi1>) -> tensor<4xi8>
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<4xi1>) -> tensor<4xi8>
// CHECK-NEXT:  return {{.*}} : tensor<4xi8>

// -----

// i8 -> i1 -> f16 is not folded because its the first conversion is a narrowing one.
// CHECK-LABEL: @no_fold_narrowing_inner
func.func @no_fold_narrowing_inner(%arg0: tensor<4xi8>) -> tensor<4xf16> {
  %0 = tosa.cast %arg0 : (tensor<4xi8>) -> tensor<4xi1>
  %1 = tosa.cast %0 : (tensor<4xi1>) -> tensor<4xf16>
  return %1 : tensor<4xf16>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<4xi8>) -> tensor<4xf16>
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<4xi8>) -> tensor<4xi1>
// CHECK-NEXT:  tosa.cast {{.*}} : (tensor<4xi1>) -> tensor<4xf16>
// CHECK-NEXT:  return {{.*}} : tensor<4xf16>

// -----

// i1 -> f32 -> bf16 is folded to i1 -> bf16
// CHECK-LABEL: @fold_widening_fp
func.func @fold_widening_fp(%arg0: tensor<4xi1>) -> tensor<4xbf16> {
  %0 = tosa.cast %arg0 : (tensor<4xi1>) -> tensor<4xf32>
  %1 = tosa.cast %0 : (tensor<4xf32>) -> tensor<4xbf16>
  return %1 : tensor<4xbf16>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<4xi1>) -> tensor<4xbf16>
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<4xi1>) -> tensor<4xbf16>
// CHECK-NEXT:  return {{.*}} : tensor<4xbf16>

// -----

// i16 -> f32 -> bf16 folds to i16 -> bf16. f32 precision(=24) can represent
// all signed i16 values, so the first cast is lossless.
// CHECK-LABEL: @fold_widening_fp_from_i16
func.func @fold_widening_fp_from_i16(%arg0: tensor<4xi16>) -> tensor<4xbf16> {
  %0 = tosa.cast %arg0 : (tensor<4xi16>) -> tensor<4xf32>
  %1 = tosa.cast %0 : (tensor<4xf32>) -> tensor<4xbf16>
  return %1 : tensor<4xbf16>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<4xi16>) -> tensor<4xbf16>
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<4xi16>) -> tensor<4xbf16>
// CHECK-NEXT:  return {{.*}} : tensor<4xbf16>

// -----

// f32 -> bf16 -> f16 is not folded as the first conversion is a narrowing one.
// CHECK-LABEL: @no_fold_f32_to_bf16_narrowing
func.func @no_fold_f32_to_bf16_narrowing(%arg0: tensor<4xf32>) -> tensor<4xf16> {
  %0 = tosa.cast %arg0 : (tensor<4xf32>) -> tensor<4xbf16>
  %1 = tosa.cast %0 : (tensor<4xbf16>) -> tensor<4xf16>
  return %1 : tensor<4xf16>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<4xf32>) -> tensor<4xf16>
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<4xf32>) -> tensor<4xbf16>
// CHECK-NEXT:  tosa.cast {{.*}} : (tensor<4xbf16>) -> tensor<4xf16>
// CHECK-NEXT:  return {{.*}} : tensor<4xf16>

// -----

// i1 -> i64 -> i16 is not folded as the result of the first conversion has multiple
// users.
// CHECK-LABEL: @no_fold_multi_use
func.func @no_fold_multi_use(%arg0: tensor<4xi1>) -> (tensor<4xf16>, tensor<4xi64>) {
  %0 = tosa.cast %arg0 : (tensor<4xi1>) -> tensor<4xi64>
  %1 = tosa.cast %0 : (tensor<4xi64>) -> tensor<4xf16>
  return %1, %0 : tensor<4xf16>, tensor<4xi64>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<4xi1>) -> (tensor<4xf16>, tensor<4xi64>)
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<4xi1>) -> tensor<4xi64>
// CHECK-NEXT:  tosa.cast {{.*}} : (tensor<4xi64>) -> tensor<4xf16>
// CHECK-NEXT:  return {{.*}} : tensor<4xf16>, tensor<4xi64>

// -----

// bf16 -> i32 -> f32 does not fold: float-to-int conversion is not
// information-preserving as it truncates the fractional part.
// CHECK-LABEL: @no_fold_bf16_to_i32_to_f32
func.func @no_fold_bf16_to_i32_to_f32(%arg0: tensor<4xbf16>) -> tensor<4xf32> {
  %0 = tosa.cast %arg0 : (tensor<4xbf16>) -> tensor<4xi32>
  %1 = tosa.cast %0 : (tensor<4xi32>) -> tensor<4xf32>
  return %1 : tensor<4xf32>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<4xbf16>) -> tensor<4xf32>
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<4xbf16>) -> tensor<4xi32>
// CHECK-NEXT:  tosa.cast {{.*}} : (tensor<4xi32>) -> tensor<4xf32>
// CHECK-NEXT:  return {{.*}} : tensor<4xf32>

// -----

// i8 -> i32 -> i8 is folded to identity: i8 -> i32 is a lossless widening
// so the chain is equivalent to i8 -> i8 which is a no-op.
// CHECK-LABEL: @fold_i8_to_i32_to_i8
func.func @fold_i8_to_i32_to_i8(%arg0: tensor<4xi8>) -> tensor<4xi8> {
  %0 = tosa.cast %arg0 : (tensor<4xi8>) -> tensor<4xi32>
  %1 = tosa.cast %0 : (tensor<4xi32>) -> tensor<4xi8>
  return %1 : tensor<4xi8>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<4xi8>) -> tensor<4xi8>
// CHECK-NEXT:  return %[[ARG0]] : tensor<4xi8>

// -----

// i8 -> f16 -> i16 is folded because f16 can fully represent i8.
// CHECK-LABEL: @fold_i8_to_f16_to_i16
func.func @fold_i8_to_f16_to_i16(%arg0: tensor<4xi8>) -> tensor<4xi16> {
  %0 = tosa.cast %arg0 : (tensor<4xi8>) -> tensor<4xf16>
  %1 = tosa.cast %0 : (tensor<4xf16>) -> tensor<4xi16>
  return %1 : tensor<4xi16>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<4xi8>) -> tensor<4xi16>
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<4xi8>) -> tensor<4xi16>
// CHECK-NEXT:  return {{.*}} : tensor<4xi16>

// -----

// i12 -> bf16 -> i16 is not folded because bf16 precision is 8 bits which is
// not sufficient to represent i12.
// CHECK-LABEL: @no_fold_i12_to_bf16_to_i16
func.func @no_fold_i12_to_bf16_to_i16(%arg0: tensor<4xi12>) -> tensor<4xi16> {
  %0 = tosa.cast %arg0 : (tensor<4xi12>) -> tensor<4xbf16>
  %1 = tosa.cast %0 : (tensor<4xbf16>) -> tensor<4xi16>
  return %1 : tensor<4xi16>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<4xi12>) -> tensor<4xi16>
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<4xi12>) -> tensor<4xbf16>
// CHECK-NEXT:  tosa.cast {{.*}} : (tensor<4xbf16>) -> tensor<4xi16>
// CHECK-NEXT:  return {{.*}} : tensor<4xi16>


// -----

// i12 -> f16 -> i16 is folded because f16 has just enough precision to
// represent i12 ([-2048, +2048] suffices to represent [-2048, +2047]).
// CHECK-LABEL: @fold_i12_to_f16_to_i16
func.func @fold_i12_to_f16_to_i16(%arg0: tensor<4xi12>) -> tensor<4xi16> {
  %0 = tosa.cast %arg0 : (tensor<4xi12>) -> tensor<4xf16>
  %1 = tosa.cast %0 : (tensor<4xf16>) -> tensor<4xi16>
  return %1 : tensor<4xi16>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<4xi12>) -> tensor<4xi16>
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<4xi12>) -> tensor<4xi16>
// CHECK-NEXT:  return {{.*}} : tensor<4xi16>


// -----

// i13 -> f16 -> i16 is not folded because f16 does not have enough precision to
// represent i13.
// CHECK-LABEL: @no_fold_i13_to_f16_to_i16
func.func @no_fold_i13_to_f16_to_i16(%arg0: tensor<4xi13>) -> tensor<4xi16> {
  %0 = tosa.cast %arg0 : (tensor<4xi13>) -> tensor<4xf16>
  %1 = tosa.cast %0 : (tensor<4xf16>) -> tensor<4xi16>
  return %1 : tensor<4xi16>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<4xi13>) -> tensor<4xi16>
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<4xi13>) -> tensor<4xf16>
// CHECK-NEXT:  tosa.cast {{.*}} : (tensor<4xf16>) -> tensor<4xi16>
// CHECK-NEXT:  return {{.*}} : tensor<4xi16>


// -----

// f8E5M2 -> tf32 -> bf16 is folded because tf32 can represent every f8E5M2 value
// (prec 3<=11, minExp -14>=-126, maxExp 15<=127).
// CHECK-LABEL: @fold_f8E5M2_via_tf32
func.func @fold_f8E5M2_via_tf32(%arg0: tensor<4xf8E5M2>) -> tensor<4xbf16> {
  %0 = tosa.cast %arg0 : (tensor<4xf8E5M2>) -> tensor<4xtf32>
  %1 = tosa.cast %0 : (tensor<4xtf32>) -> tensor<4xbf16>
  return %1 : tensor<4xbf16>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<4xf8E5M2>) -> tensor<4xbf16>
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<4xf8E5M2>) -> tensor<4xbf16>
// CHECK-NEXT:  return {{.*}} : tensor<4xbf16>


// -----

// tf32 -> f16 -> f32 is not folded because f16 cannot represent all tf32 values.
// CHECK-LABEL: @no_fold_tf32_via_f16
func.func @no_fold_tf32_via_f16(%arg0: tensor<4xtf32>) -> tensor<4xf32> {
  %0 = tosa.cast %arg0 : (tensor<4xtf32>) -> tensor<4xf16>
  %1 = tosa.cast %0 : (tensor<4xf16>) -> tensor<4xf32>
  return %1 : tensor<4xf32>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<4xtf32>) -> tensor<4xf32>
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<4xtf32>) -> tensor<4xf16>
// CHECK-NEXT:  tosa.cast {{.*}} : (tensor<4xf16>) -> tensor<4xf32>
// CHECK-NEXT:  return {{.*}} : tensor<4xf32>


// -----

// i32 -> i16 -> i8: a case of trivial cast chain that is folded.
// CHECK-LABEL: @fold_i32_to_i16_to_i8
func.func @fold_i32_to_i16_to_i8(%arg0: tensor<4xi32>) -> tensor<4xi8> {
  %0 = tosa.cast %arg0 : (tensor<4xi32>) -> tensor<4xi16>
  %1 = tosa.cast %0 : (tensor<4xi16>) -> tensor<4xi8>
  return %1 : tensor<4xi8>
}
// CHECK-SAME:  (%[[ARG0:.*]]: tensor<4xi32>) -> tensor<4xi8>
// CHECK-NEXT:  tosa.cast %[[ARG0]] : (tensor<4xi32>) -> tensor<4xi8>
// CHECK-NEXT:  return {{.*}} : tensor<4xi8>
