// RUN: mlir-opt -convert-memref-to-emitc %s -split-input-file | FileCheck %s --check-prefixes=COMMON,DIRECT
// RUN: mlir-opt -convert-to-emitc="filter-dialects=memref" %s -split-input-file | FileCheck %s --check-prefixes=COMMON,COMBINED

// COMMON: module {
// COMMON-LABEL: alloca()
func.func @alloca() {
  // COMMON-NEXT: %[[ALLOCA:.*]] = "emitc.variable"() <{value = #emitc.opaque<"">}> : () -> !emitc.array<2xf32>
  %0 = memref.alloca() : memref<2xf32>
  return
}

// -----

// COMMON-LABEL: memref_store
// COMMON-SAME:  %[[buff:.*]]: memref<4x8xf32>, %[[v:.*]]: f32, %[[argi:.*]]: index, %[[argj:.*]]: index
func.func @memref_store(%buff : memref<4x8xf32>, %v : f32, %i: index, %j: index) {
  // DIRECT-NEXT: %[[j:.*]] = builtin.unrealized_conversion_cast %[[argj]] : index to !emitc.size_t
  // DIRECT-NEXT: %[[i:.*]] = builtin.unrealized_conversion_cast %[[argi]] : index to !emitc.size_t
  // DIRECT-NEXT: %[[BUFFER:.*]] = builtin.unrealized_conversion_cast %[[buff]] : memref<4x8xf32> to !emitc.array<4x8xf32>

  // DIRECT-NEXT: %[[SUBSCRIPT:.*]] = emitc.subscript %[[BUFFER]][%[[i]], %[[j]]] : (!emitc.array<4x8xf32>, !emitc.size_t, !emitc.size_t) -> !emitc.lvalue<f32>
  // DIRECT-NEXT: emitc.assign %[[v]] : f32 to %[[SUBSCRIPT]] : <f32>
  memref.store %v, %buff[%i, %j] : memref<4x8xf32>
  return
}

// COMBINED: emitc.subscript {{.*}} : (!emitc.array<4x8xf32>, index, index) -> !emitc.lvalue<f32>

// -----

// COMMON-LABEL: memref_load
// COMMON-SAME:  %[[buff:.*]]: memref<4x8xf32>, %[[argi:.*]]: index, %[[argj:.*]]: index
func.func @memref_load(%buff : memref<4x8xf32>, %i: index, %j: index) -> f32 {
  // DIRECT-NEXT: %[[j:.*]] = builtin.unrealized_conversion_cast %[[argj]] : index to !emitc.size_t
  // DIRECT-NEXT: %[[i:.*]] = builtin.unrealized_conversion_cast %[[argi]] : index to !emitc.size_t
  // DIRECT-NEXT: %[[BUFFER:.*]] = builtin.unrealized_conversion_cast %[[buff]] : memref<4x8xf32> to !emitc.array<4x8xf32>
  // DIRECT-NEXT: %[[SUBSCRIPT:.*]] = emitc.subscript %[[BUFFER]][%[[i]], %[[j]]] : (!emitc.array<4x8xf32>, !emitc.size_t, !emitc.size_t) -> !emitc.lvalue<f32>
  // DIRECT-NEXT: %[[LOAD:.*]] = emitc.load %[[SUBSCRIPT]] : <f32>
  %1 = memref.load %buff[%i, %j] : memref<4x8xf32>
  // DIRECT-NEXT: return %[[LOAD]] : f32
  return %1 : f32
}

// -----

// COMMON-LABEL: globals
module @globals {
  memref.global "private" constant @internal_global : memref<3x7xf32> = dense<4.0>
  // COMMON-NEXT: emitc.global static const @internal_global : !emitc.array<3x7xf32> = dense<4.000000e+00>
  memref.global @public_global : memref<3x7xf32>
  // COMMON-NEXT: emitc.global extern @public_global : !emitc.array<3x7xf32>
  memref.global @uninitialized_global : memref<3x7xf32> = uninitialized
  // COMMON-NEXT: emitc.global extern @uninitialized_global : !emitc.array<3x7xf32>

  // COMMON-LABEL: use_global
  func.func @use_global() {
    // COMMON-NEXT: emitc.get_global @public_global : !emitc.array<3x7xf32>
    %0 = memref.get_global @public_global : memref<3x7xf32>
    return
  }
}

// -----

// COMBINED-LABEL: func.func @memref_index_values
// DIRECT-LABEL: memref_index_values
// DIRECT-SAME:  %[[argi:.*]]: index, %[[argj:.*]]: index
// DIRECT-SAME: -> index
func.func @memref_index_values(%i: index, %j: index) -> index {
  // DIRECT: %[[j:.*]] = builtin.unrealized_conversion_cast %[[argj]] : index to !emitc.size_t
  // DIRECT: %[[i:.*]] = builtin.unrealized_conversion_cast %[[argi]] : index to !emitc.size_t

  // DIRECT: %[[ALLOCA:.*]] = "emitc.variable"() <{value = #emitc.opaque<"">}> : () -> !emitc.array<4x8x!emitc.size_t>
  %0 = memref.alloca() : memref<4x8xindex>

  // DIRECT: %[[SUB:.*]] = emitc.subscript %[[ALLOCA]][%[[i]], %[[j]]] : (!emitc.array<4x8x!emitc.size_t>, !emitc.size_t, !emitc.size_t) -> !emitc.lvalue<!emitc.size_t>
  // DIRECT: %[[LOAD:.*]] = emitc.load %[[SUB]] : <!emitc.size_t>
  %1 = memref.load %0[%i, %j] : memref<4x8xindex>

  // DIRECT: %[[CAST_RET:.*]] = builtin.unrealized_conversion_cast %[[LOAD]] : !emitc.size_t to index
  // DIRECT: return %[[CAST_RET]] : index
  return %1 : index
}

// COMBINED: !emitc.array<4x8xindex>
// COMBINED: emitc.load {{.*}} : <index>

// -----

// COMMON-LABEL: memref_expand_shape
func.func @memref_expand_shape(%arg: memref<10xi32>) -> memref<2x5xi32> {
  // COMMON: emitc.cast %{{[^ ]*}} : !emitc.array<10xi32> to !emitc.array<2x5xi32> ref
  %0 = memref.expand_shape %arg [[0, 1]] output_shape [2, 5] : memref<10xi32> into memref<2x5xi32>
  return %0 : memref<2x5xi32>
}


// -----

// COMMON-LABEL: memref_collapse_shape
func.func @memref_collapse_shape(%arg: memref<2x5xi32>) -> memref<10xi32> {
  // COMMON: emitc.cast %{{[^ ]*}} : !emitc.array<2x5xi32> to !emitc.array<10xi32> ref
  %0 = memref.collapse_shape %arg [[0, 1]] : memref<2x5xi32> into memref<10xi32>
  return %0 : memref<10xi32>
}
