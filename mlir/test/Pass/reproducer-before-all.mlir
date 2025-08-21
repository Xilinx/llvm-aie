// RUN: rm -rf %t || true
// RUN: mlir-opt %s -mlir-disable-threading -mlir-reproducer-before-all=%t \
// RUN:   -pass-pipeline='builtin.module(canonicalize,cse,func.func(canonicalize))'
// RUN: FileCheck %s -input-file=%t/00_canonicalize.mlir --check-prefixes CHECK0
// RUN: FileCheck %s -input-file=%t/01_cse.mlir --check-prefixes CHECK1
// RUN: FileCheck %s -input-file=%t/02_canonicalize.mlir --check-prefixes CHECK2

builtin.module @outer {
  func.func @symA() {
    return
  }
}

// CHECK0:      module @outer {
// CHECK0:      {-#
// CHECK0-NEXT: external_resources: {
// CHECK0-NEXT:     mlir_reproducer: {
// CHECK0-NEXT:       pipeline: "builtin.module(canonicalize
// CHECK0-NEXT:       disable_threading: true,
// CHECK0-NEXT:       verify_each: true
// CHECK0-NEXT:     }
// CHECK0-NEXT:   }
// CHECK0-NEXT: #-}

// CHECK1:       pipeline: "builtin.module(cse
// CHECK2:       pipeline: "builtin.module(func.func(canonicalize
