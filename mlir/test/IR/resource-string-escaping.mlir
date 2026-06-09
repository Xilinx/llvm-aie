// Modifications (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
// Resource string values escape the double-quote character as the C-style
// escape `\"` (not the hexadecimal escape `\22`). Both escapes are accepted by
// the MLIR lexer (see Token::getStringValue), but `\"` keeps embedded
// structured text -- such as the pass-pipeline string stored in an
// `mlir_reproducer` resource -- readable and directly reusable.

// Print once, and then round-trip the printed output back through the parser to
// make sure the `\"`-escaped form parses again and prints identically.
// RUN: mlir-opt %s | FileCheck %s
// RUN: mlir-opt %s | mlir-opt | FileCheck %s

// The input below mixes both escape forms in a single string: the `k` key uses
// the hexadecimal `\22` escape while the `s`/`v` strings use the C-style `\"`
// escape. The parser must accept both, and the printer must normalize them all
// to the `\"` form.

// CHECK:      external_resources: {
// CHECK-NEXT:   reproducer_like: {
// CHECK-NEXT:     pipeline: "builtin.module(some-pass{opts={\"k\":1,\"s\":\"v\"}})"
// CHECK-NEXT:   }
// CHECK-NEXT: }

"test.op"() : () -> ()

{-#
  external_resources: {
    reproducer_like: {
      pipeline: "builtin.module(some-pass{opts={\22k\22:1,\"s\":\"v\"}})"
    }
  }
#-}
