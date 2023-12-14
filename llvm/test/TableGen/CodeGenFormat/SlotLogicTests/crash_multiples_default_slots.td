//===- crash_multiples_default_slots.td --------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// Testing if the default slot(s) are behaving correctly:
// - a unique default slot must be defined
// - The size of a default slot doesn't matter
// - if multiple default slots are defined, the TG Backend crash

// RUN: not llvm-tblgen --gen-instr-format -I %p -I %p/../ -I %p/../../../../include %s 2>&1 | FileCheck %s

include "Inputs/CommonSlotTesting.td"

let Namespace = "TEST" in
{
  def fu0_slot     : InstSlot<"fu0",   0, true>;
  def fu1_slot     : InstSlot<"fu1",  42, true>;
  // The Backend will crash before the evaluation of this one.
  def fu2_slot     : InstSlot<"fu2",   1, true>;
  // CHECK:       Multiples default slots defined:
  // CHECK-NEXT:  - fu0_slot
  // CHECK-NEXT:  - fu1_slot
  // CHECK-NEXT: One default slot (at most) must be defined
}