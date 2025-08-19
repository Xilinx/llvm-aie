// RUN: mlir-opt -convert-memref-to-emitc %s -split-input-file | FileCheck %s

// CHECK-LABEL: alloca()
func.func @alloca() {
  // CHECK-NEXT: %[[ALLOCA:.*]] = "emitc.variable"() <{value = #emitc.opaque<"">}> : () -> !emitc.array<2xf32>
  %0 = memref.alloca() : memref<2xf32>
  return
}

// -----

// CHECK-LABEL: memref_store
// CHECK-SAME:  %[[buff:.*]]: memref<4x8xf32>, %[[v:.*]]: f32, %[[argi:.*]]: index, %[[argj:.*]]: index
func.func @memref_store(%buff : memref<4x8xf32>, %v : f32, %i: index, %j: index) {
  // CHECK-NEXT: %[[j:.*]] = builtin.unrealized_conversion_cast %[[argj]] : index to !emitc.size_t 
  // CHECK-NEXT: %[[i:.*]] = builtin.unrealized_conversion_cast %[[argi]] : index to !emitc.size_t 
  // CHECK-NEXT: %[[BUFFER:.*]] = builtin.unrealized_conversion_cast %[[buff]] : memref<4x8xf32> to !emitc.array<4x8xf32>
  
  // CHECK-NEXT: %[[SUBSCRIPT:.*]] = emitc.subscript %[[BUFFER]][%[[i]], %[[j]]] : (!emitc.array<4x8xf32>, !emitc.size_t, !emitc.size_t) -> !emitc.lvalue<f32>
  // CHECK-NEXT: emitc.assign %[[v]] : f32 to %[[SUBSCRIPT]] : <f32>
  memref.store %v, %buff[%i, %j] : memref<4x8xf32>
  return
}

// -----

// CHECK-LABEL: memref_load
// CHECK-SAME:  %[[buff:.*]]: memref<4x8xf32>, %[[argi:.*]]: index, %[[argj:.*]]: index
func.func @memref_load(%buff : memref<4x8xf32>, %i: index, %j: index) -> f32 {
  // CHECK-NEXT: %[[j:.*]] = builtin.unrealized_conversion_cast %[[argj]] : index to !emitc.size_t
  // CHECK-NEXT: %[[i:.*]] = builtin.unrealized_conversion_cast %[[argi]] : index to !emitc.size_t
  // CHECK-NEXT: %[[BUFFER:.*]] = builtin.unrealized_conversion_cast %[[buff]] : memref<4x8xf32> to !emitc.array<4x8xf32>
  // CHECK-NEXT: %[[SUBSCRIPT:.*]] = emitc.subscript %[[BUFFER]][%[[i]], %[[j]]] : (!emitc.array<4x8xf32>, !emitc.size_t, !emitc.size_t) -> !emitc.lvalue<f32>
  // CHECK-NEXT: %[[LOAD:.*]] = emitc.load %[[SUBSCRIPT]] : <f32>
  %1 = memref.load %buff[%i, %j] : memref<4x8xf32>
  // CHECK-NEXT: return %[[LOAD]] : f32
  return %1 : f32
}

// -----

// CHECK-LABEL: globals
module @globals {
  memref.global "private" constant @internal_global : memref<3x7xf32> = dense<4.0>
  // CHECK-NEXT: emitc.global static const @internal_global : !emitc.array<3x7xf32> = dense<4.000000e+00>
  memref.global @public_global : memref<3x7xf32>
  // CHECK-NEXT: emitc.global extern @public_global : !emitc.array<3x7xf32>
  memref.global @uninitialized_global : memref<3x7xf32> = uninitialized
  // CHECK-NEXT: emitc.global extern @uninitialized_global : !emitc.array<3x7xf32>

  // CHECK-LABEL: use_global
  func.func @use_global() {
    // CHECK-NEXT: emitc.get_global @public_global : !emitc.array<3x7xf32>
    %0 = memref.get_global @public_global : memref<3x7xf32>
    return
  }
}

// -----

// CHECK-LABEL: memref_index_values
// CHECK-SAME:  %[[argi:.*]]: index, %[[argj:.*]]: index
// CHECK-SAME: -> index
func.func @memref_index_values(%i: index, %j: index) -> index {
  // CHECK: %[[j:.*]] = builtin.unrealized_conversion_cast %[[argj]] : index to !emitc.size_t
  // CHECK: %[[i:.*]] = builtin.unrealized_conversion_cast %[[argi]] : index to !emitc.size_t

  // CHECK: %[[ALLOCA:.*]] = "emitc.variable"() <{value = #emitc.opaque<"">}> : () -> !emitc.array<4x8x!emitc.size_t>
  %0 = memref.alloca() : memref<4x8xindex>

  // CHECK: %[[SUB:.*]] = emitc.subscript %[[ALLOCA]][%[[i]], %[[j]]] : (!emitc.array<4x8x!emitc.size_t>, !emitc.size_t, !emitc.size_t) -> !emitc.lvalue<!emitc.size_t>
  // CHECK: %[[LOAD:.*]] = emitc.load %[[SUB]] : <!emitc.size_t>
  %1 = memref.load %0[%i, %j] : memref<4x8xindex>

  // CHECK: %[[CAST_RET:.*]] = builtin.unrealized_conversion_cast %[[LOAD]] : !emitc.size_t to index
  // CHECK: return %[[CAST_RET]] : index
  return %1 : index
}

// -----

// CHECK-LABEL: memref_expand_shape
func.func @memref_expand_shape(%arg: memref<10xi32>) -> memref<2x5xi32> {
  // CHECK: emitc.cast %{{[^ ]*}} : !emitc.array<10xi32> to !emitc.array<2x5xi32> ref
  %0 = memref.expand_shape %arg [[0, 1]] output_shape [2, 5] : memref<10xi32> into memref<2x5xi32>
  return %0 : memref<2x5xi32>
}


// -----

// CHECK-LABEL: memref_collapse_shape
func.func @memref_collapse_shape(%arg: memref<2x5xi32>) -> memref<10xi32> {
  // CHECK: emitc.cast %{{[^ ]*}} : !emitc.array<2x5xi32> to !emitc.array<10xi32> ref
  %0 = memref.collapse_shape %arg [[0, 1]] : memref<2x5xi32> into memref<10xi32>
  return %0 : memref<10xi32>
}
