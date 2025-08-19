// RUN: mlir-opt %s -split-input-file | FileCheck %s

// CHECK: emitc.func @f
// CHECK-SAME: %{{[^:]*}}: i32 ref
emitc.func @f(%x: i32 {emitc.reference}) {
    emitc.return
}

// -----

// CHECK: emitc.func @f
// CHECK-SAME: %{{[^:]*}}: i32 ref
emitc.func @f(%x: i32 ref) {
    emitc.return
}

// -----

// CHECK: emitc.func private @f
// CHECK-SAME: i32 ref
emitc.func private @f(i32 ref)

// -----

// CHECK: emitc.func private @f
// CHECK-SAME: i32 ref
emitc.func private @f(i32 {emitc.reference})
