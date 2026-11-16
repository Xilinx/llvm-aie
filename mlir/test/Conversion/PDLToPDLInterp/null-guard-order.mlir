// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2026 Advanced Micro Devices, Inc. or its
// affiliates

// RUN: mlir-opt -convert-pdl-to-pdl-interp %s | FileCheck %s

// Repeated pdl.result queries increase the frequency score of the result's
// null check. Computing that result must still follow the defining-op guard.
// CHECK-LABEL: pdl_interp.func @matcher
// CHECK: %[[OPERAND:.*]] = pdl_interp.get_operand 0 of
// CHECK: %[[PARENT:.*]] = pdl_interp.get_defining_op of %[[OPERAND]]
// CHECK-NEXT: pdl_interp.is_not_null %[[PARENT]] : !pdl.operation -> ^[[SUCCESS:bb[0-9]+]],
// CHECK: ^[[SUCCESS]]:
// CHECK: %[[RESULT:.*]] = pdl_interp.get_result 0 of %[[PARENT]]
// CHECK-NEXT: pdl_interp.is_not_null %[[RESULT]] : !pdl.value
module {
  pdl.pattern @repeated_result : benefit(1) {
    %input = operand
    %parentType = type
    %parent = operation(%input : !pdl.value) -> (%parentType : !pdl.type)
    %result = result 0 of %parent
    %rootType = type
    %root = operation "test.root"(%result : !pdl.value) -> (%rootType : !pdl.type)
    %r0 = result 0 of %parent
    apply_native_constraint "use"(%r0 : !pdl.value)
    %r1 = result 0 of %parent
    apply_native_constraint "use"(%r1 : !pdl.value)
    %r2 = result 0 of %parent
    apply_native_constraint "use"(%r2 : !pdl.value)
    rewrite %root with "rewriter"
  }
}
