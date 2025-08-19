// RUN: mlir-opt --split-input-file -verify-diagnostics -tosa-layerwise-constant-fold="enable-cast-folding-int-input=false" %s | FileCheck %s

// CHECK-LABEL: @cast_fold_f32_to_i32
func.func @cast_fold_f32_to_i32() -> tensor<3xi32> {
  // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}12, 4, 5{{.*}}tensor<3xi32>
  // CHECK-NOT: tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value = dense<[12.0, 4.0, 5.0]> : tensor<3xf32>} : () -> tensor<3xf32>
  %1 = "tosa.cast"(%0) : (tensor<3xf32>) -> tensor<3xi32>
  return %1 : tensor<3xi32>
}

// CHECK-LABEL: @cast_fold_i16_to_f32
func.func @cast_fold_i16_to_f32() -> tensor<3xf32> {
  // CHECK: tosa.const
  // CHECK: [[RES:]] ={{.*}}tosa.cast
  // CHECK: return [[RES]]
  %0 = "tosa.const"() {value =
                        dense<[12, 0, -5]> :
                        tensor<3xi16>
                      } : () -> tensor<3xi16>
  %1 = "tosa.cast"(%0) : (tensor<3xi16>) -> tensor<3xf32>
  return %1 : tensor<3xf32>
}
