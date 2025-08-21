// RUN: mlir-opt %s -one-shot-bufferize="bufferize-function-boundaries=1" -split-input-file | FileCheck %s

func.func private @foo() -> tensor<?xf32>
// CHECK: func.func private @foo() -> memref<?xf32, strided<[?], offset: ?>>

// -----

func.func private @foo(tensor<?xf32>) -> (f32, tensor<?xf32>, f32)
// CHECK: func.func private @foo(memref<?xf32, strided<[?], offset: ?>>) -> (f32, memref<?xf32, strided<[?], offset: ?>>, f32)

func.func @call_to_unknown_tensor_returning_func(%t : tensor<?xf32>) {
  call @foo(%t) : (tensor<?xf32>) -> (f32, tensor<?xf32>, f32)
  // CHECK: call @foo(%{{.*}}) : (memref<?xf32, strided<[?], offset: ?>>) -> (f32, memref<?xf32, strided<[?], offset: ?>>, f32)
  return
}
