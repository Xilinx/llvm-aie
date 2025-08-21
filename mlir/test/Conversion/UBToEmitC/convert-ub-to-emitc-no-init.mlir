// RUN: mlir-opt -p 'builtin.module(convert-ub-to-emitc{no-initialization})' %s | FileCheck %s

// CHECK-LABEL: func.func @poison
func.func @poison() {
  // CHECK: %[[V:[a-zA-Z0-9_]+]] = "emitc.variable"() <{value = #emitc.opaque<"">}> : () -> !emitc.lvalue<i32>
  // CHECK: emitc.load %[[V]]
  %0 = ub.poison : i32
  // CHECK: %[[V:[a-zA-Z0-9_]+]] = "emitc.variable"() <{value = #emitc.opaque<"">}> : () -> !emitc.lvalue<f32>
  // CHECK: emitc.load %[[V]]
  %1 = ub.poison : f32
  // CHECK: %[[V:[a-zA-Z0-9_]+]] = "emitc.variable"() <{value = #emitc.opaque<"">}> : () -> !emitc.lvalue<!emitc.size_t>
  // CHECK: emitc.load %[[V]]
  %2 = ub.poison : index
  return
}
