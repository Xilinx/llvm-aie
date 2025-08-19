// RUN: mlir-opt -convert-ub-to-emitc %s | FileCheck %s

// CHECK-LABEL: func.func @poison
func.func @poison() {
  // CHECK: "emitc.constant"() <{value = 42 : i32}> : () -> i32
  %0 = ub.poison : i32
  // CHECK: "emitc.constant"() <{value = 4.200000e+01 : f32}> : () -> f32
  %1 = ub.poison : f32
  // CHECK: "emitc.constant"() <{value = 42 : index}> : () -> !emitc.size_t
  %2 = ub.poison : index
  return
}
