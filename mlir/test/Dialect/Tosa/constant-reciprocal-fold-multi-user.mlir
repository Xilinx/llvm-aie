// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold="fold-splat-or-single-use-only=0" %s | FileCheck %s

// CHECK-LABEL: @reciprocal_fold
func.func @reciprocal_fold() -> (tensor<4x6xf32>,tensor<4x6xf32>) {
  // CHECK: %[[ORIG:.*]] = "tosa.const"()
  // CHECK-SAME{LITERAL}: [[1.758000e-01
  // CHECK: %[[RES:.*]] ={{.*}}tosa.const
  // CHECK-SAME{LITERAL}: [[5.68828249, 11.4416485, 1.6880486, 0.680272102, -0.875350117, 0.342313349],
  // CHECK-SAME{LITERAL}:  [-4.81231928, 0.698080301, 0.65432179, -82.6446304, -4.33651352, -0.747551739],
  // CHECK-SAME{LITERAL}:  [-12.4378109, 13.140605, 1.89501607, 0.885582745, 4.08830738, 1.4396776],
  // CHECK-SAME{LITERAL}:  [2.02880907, -1.53280187, 0.552730501, 7.15819644, 0.64495325, -0.973709881]]
  // CHECK-NOT: tosa.reciprocal
  // CHECK: return %[[ORIG]], %[[RES]]
  %0 = "tosa.const"() { value = dense<[
                        [ 0.1758,  0.0874,  0.5924,  1.4700, -1.1424,  2.9213],
                        [-0.2078,  1.4325,  1.5283, -0.0121, -0.2306, -1.3377],
                        [-0.0804,  0.0761,  0.5277,  1.1292,  0.2446,  0.6946],
                        [ 0.4929, -0.6524,  1.8092,  0.1397,  1.5505, -1.0270]]>
                        : tensor<4x6xf32>
                      } : () -> tensor<4x6xf32>
  %1 = "tosa.reciprocal"(%0) : (tensor<4x6xf32>) -> tensor<4x6xf32>
  return %0, %1 : tensor<4x6xf32>, tensor<4x6xf32>
}
