// RUN: mlir-opt -convert-ub-to-emitc -split-input-file -verify-diagnostics %s

func.func @poison_memref() {
  // expected-error @+1 {{failed to legalize operation 'ub.poison'}}
  %0 = ub.poison : memref<i32>
  return
}

// -----

func.func @poison_tensor() {
  // expected-error @+1 {{failed to legalize operation 'ub.poison'}}
  %1 = ub.poison : tensor<f32>
  return
}

// -----

func.func @poison_vector() {
  // expected-error @+1 {{failed to legalize operation 'ub.poison'}}
  %1 = "ub.poison"() {value = #ub.poison} : () -> vector<4xi64>
  return
}
