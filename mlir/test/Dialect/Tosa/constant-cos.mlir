// RUN: mlir-opt --split-input-file --tosa-layerwise-constant-fold %s | FileCheck %s

// CHECK-LABEL: @cos_fold_single_valued
func.func @cos_fold_single_valued() -> tensor<f32> {
    // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}1.000000e+00{{.*}} tensor<f32>
    // CHECK-NOT: tosa.cos
    // CHECK: return [[RES]]
    %0 = "tosa.const"() {value = dense<0.0> : tensor<f32>} : () -> tensor<f32>
    %1 = "tosa.cos"(%0) : (tensor<f32>) -> tensor<f32>
    return %1 : tensor<f32>
}

// CHECK-LABEL: @cos_fold_splat
func.func @cos_fold_splat() -> tensor<2x3xf32> {
    // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}1.000000e+00{{.*}} tensor<2x3xf32>
    // CHECK-NOT: tosa.cos
    // CHECK: return [[RES]]
    %0 = "tosa.const"() {value = dense<0.0> : tensor<2x3xf32>} : () -> tensor<2x3xf32>
    %1 = "tosa.cos"(%0) : (tensor<2x3xf32>) -> tensor<2x3xf32>
    return %1 : tensor<2x3xf32>
}

// CHECK-LABEL: @cos_fold_bf16
func.func @cos_fold_bf16() -> tensor<2x3xbf16> {
    // CHECK: [[RES:]] ={{.*}}tosa.const{{.*}}8.125000e-01
    // CHECK-NOT: tosa.cos
    // CHECK: return [[RES]]
    %0 = "tosa.const"() {value = dense<0.625> : tensor<2x3xbf16>} : () -> tensor<2x3xbf16>
    %1 = "tosa.cos"(%0) : (tensor<2x3xbf16>) -> tensor<2x3xbf16>
    return %1 : tensor<2x3xbf16>
}

// CHECK-LABEL: @cos_neg_zero
func.func @cos_neg_zero() -> tensor<f32> {
    // CHECK: [[RES:]] = {{.*}}tosa.const{{.*}}1.000000e+00
    // CHECK-NOT: tosa.cos
    // CHECK: return [[RES]]
    %0 = "tosa.const"() {value = dense<-0.0> : tensor<f32>} : () -> tensor<f32>
    %1 = "tosa.cos"(%0) : (tensor<f32>) -> tensor<f32>
    return %1 : tensor<f32>
}

// CHECK-LABE: @cos_nan
func.func @cos_nan() -> tensor<f32> {
    // CHECK: [[RES:]] = {{.*}}tosa.const{{.*}}0x7FC00000
    // CHECK-NOT: tosa.cos
    // CHECK: return [[RES]]
    %0 = "tosa.const"() {value = dense<0x7FC00000> : tensor<f32>} : () -> tensor<f32>
    %1 = "tosa.cos"(%0) : (tensor<f32>) -> tensor<f32>
    return %1 : tensor<f32>
}

// CHECK-LABEL: @cos_no_fold
func.func @cos_no_fold(%arg0: tensor<?x?xf32>) -> tensor<?x?xf32> {
    // CHECK: tosa.cos
    // CHECK-NEXT: return
    %0 = "tosa.cos"(%arg0) : (tensor<?x?xf32>) -> tensor<?x?xf32>
    return %0 : tensor<?x?xf32>
}

// CHECK-LABEL: @cos_fold
func.func @cos_fold() -> tensor<2x3xf32> {
    // CHECK: [[RES:]] ={{.*}}tosa.const
    // CHECK-SAME: 0.9920{{[0-9]+}}, 0.9999{{[0-9]+}}, 0.9850{{[0-9]+}}
    // CHECK-SAME: [0.6677{{[0-9]+}}, 0.8206{{[0-9]+}}, 0.8893{{[0-9]+}}
    // CHECK-NOT: tosa.cos
    // CHECK: return [[RES]]
    %0 = "tosa.const"() { value = dense<[
                          [ 0.1259, 0.0086, 0.1734],
                          [-0.8396, 0.6082, 0.4749]]>
                          : tensor<2x3xf32>
                        } : () -> tensor<2x3xf32>
    %1 = "tosa.cos"(%0) : (tensor<2x3xf32>) -> tensor<2x3xf32>
    return %1 : tensor<2x3xf32>
}