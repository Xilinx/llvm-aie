// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold %s | FileCheck %s

// CHECK-LABEL: @sin_fold_single_valued
func.func @sin_fold_single_valued() -> tensor<f32> {
    // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0.000000e+00{{.*}} tensor<f32>
    // CHECK-NOT: tosa.sin
    // CHECK: return [[RES]]
    %0 = "tosa.const"() {value = dense<0.0> : tensor<f32>} : () -> tensor<f32>
    %1 = "tosa.sin"(%0) : (tensor<f32>) -> tensor<f32>
    return %1 : tensor<f32>
}

// CHECK-LABEL: @sin_fold_splat
func.func @sin_fold_splat() -> tensor<2x3xf32> {
    // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}0.000000e+00{{.*}} tensor<2x3xf32>
    // CHECK-NOT: tosa.sin
    // CHECK: return [[RES]]
    %0 = "tosa.const"() {value = dense<0.0> : tensor<2x3xf32>} : () -> tensor<2x3xf32>
    %1 = "tosa.sin"(%0) : (tensor<2x3xf32>) -> tensor<2x3xf32>
    return %1 : tensor<2x3xf32>
}

// CHECK-LABEL: @sin_fold_bf16
func.func @sin_fold_bf16() -> tensor<2x3xbf16> {
    // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}6.250000e-02
    // CHECK-NOT: tosa.sin
    // CHECK: return [[RES]]
    %0 = "tosa.const"() {value = dense<0.0625> : tensor<2x3xbf16>} : () -> tensor<2x3xbf16>
    %1 = "tosa.sin"(%0) : (tensor<2x3xbf16>) -> tensor<2x3xbf16>
    return %1 : tensor<2x3xbf16>
}

// CHECK-LABEL: @sin_neg_zero
func.func @sin_neg_zero() -> tensor<f32> {
    // CHECK: [[RES:]] = {{.*}}tosa.const{{.*}}-0.000000e+00
    // CHECK-NOT: tosa.sin
    // CHECK: return [[RES]]
    %0 = "tosa.const"() {value = dense<-0.0> : tensor<f32>} : () -> tensor<f32>
    %1 = "tosa.sin"(%0) : (tensor<f32>) -> tensor<f32>
    return %1 : tensor<f32>
}

// CHECK-LABE: @sin_nan
func.func @sin_nan() -> tensor<f32> {
    // CHECK: [[RES:]] = {{.*}}tosa.const{{.*}}0x7FC00000
    // CHECK-NOT: tosa.sin
    // CHECK: return [[RES]]
    %0 = "tosa.const"() {value = dense<0x7FC00000> : tensor<f32>} : () -> tensor<f32>
    %1 = "tosa.sin"(%0) : (tensor<f32>) -> tensor<f32>
    return %1 : tensor<f32>
}

// CHECK-LABEL: @sin_no_fold
func.func @sin_no_fold(%arg0: tensor<?x?xf32>) -> tensor<?x?xf32> {
    // CHECK: tosa.sin
    // CHECK-NEXT: return
    %0 = "tosa.sin"(%arg0) : (tensor<?x?xf32>) -> tensor<?x?xf32>
    return %0 : tensor<?x?xf32>
}

// CHECK-LABEL: @sin_fold
func.func @sin_fold() -> tensor<2x3xf32> {
    // CHECK: [[RES:]] ={{.*}}tosa.const
    // CHECK-SAME: 0.1255{{[0-9]+}}, 0.0085{{[0-9]+}}, 0.1725{{[0-9]+}}
    // CHECK-SAME: [-0.7443{{[0-9]+}}, 0.5713{{[0-9]+}}, 0.6996{{[0-9]+}}
    // CHECK-NOT: tosa.sin
    // CHECK: return [[RES]]
    %0 = "tosa.const"() { value = dense<[
                          [ 0.1259, 0.0086, 0.1734],
                          [-0.8396, 0.6082, 0.7749]]>
                          : tensor<2x3xf32>
                        } : () -> tensor<2x3xf32>
    %1 = "tosa.sin"(%0) : (tensor<2x3xf32>) -> tensor<2x3xf32>
    return %1 : tensor<2x3xf32>
}