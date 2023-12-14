//===- positive_default_slot_size.td -----------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// Testing if the default slot(s) are behaving correctly.
// The size of a given slot must be > 0 if the slot is valid and >= 0 if it's
// a default one.

// RUN: llvm-tblgen --gen-instr-format -I %p -I %p/../../../ -I %p/../../../../../../include %s 2>&1 | FileCheck %s

include "Inputs/CommonSlotTesting.td"

let Namespace = "TEST" in
{
  // This is allowed, even though it's non-sense
  def fu2_slot     : InstSlot<"fu2", 42, true>;
  // CHECK-NOT: assertion failed
  // CHECK-NOT: Invalid size of the slot fu2_slot
}