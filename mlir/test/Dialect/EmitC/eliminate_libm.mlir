// RUN: mlir-opt %s --eliminate-libm --verify-diagnostics --split-input-file | FileCheck %s

// CHECK: emitc.include <"cmath">
// CHECK-NOT: emitc.func private @expm1
// CHECK-DAG: emitc.func @call_expm1(%[[IN:.*]]: f64) -> f64
// CHECK-DAG: %[[RESULT:.*]] = call_opaque "expm1"(%[[IN]]) : (f64) -> f64
// CHECK-DAG: return %[[RESULT]]
module {
  emitc.func private @expm1(f64) -> f64 attributes {libm, llvm.readnone, specifiers = ["extern"]}
  emitc.func @call_expm1(%in : f64) -> f64 {
    %e1 = emitc.call @expm1(%in) : (f64) -> f64
    emitc.return %e1 : f64
  }
}

// -----

// CHECK-NOT: emitc.include <"cmath">
// CHECK: emitc.func private @expm1
// CHECK: emitc.func @call_expm1(%[[IN:.*]]: f64) -> f64
// CHECK-NEXT: %[[RESULT:.*]] = call @expm1(%[[IN]]) : (f64) -> f64
// CHECK-NEXT: return %[[RESULT]]
module {
  emitc.func private @expm1(f64) -> f64 attributes {llvm.readnone}
  emitc.func @call_expm1(%in : f64) -> f64 {
    %e1 = emitc.call @expm1(%in) : (f64) -> f64
    emitc.return %e1 : f64
  }
}
