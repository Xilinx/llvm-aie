//===- no_default_slot.td ----------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// Testing that the Backend behaves correctly when no default slot is defined

// RUN: llvm-tblgen --gen-instr-format -I %p -I %p/../ -I %p/../../../../include %s 2>&1 | FileCheck %s

include "Inputs/CommonSlotTesting.td"

let Namespace = "TEST" in
{
  // These 2 slots are valid ones
  def fu0_slot     : InstSlot<"fu0",  12>;
  def fu1_slot     : InstSlot<"fu1",  42>;

// Let's check that the default slot isn't generated in the enumeration
// CHECK:     #ifdef GET_FORMATS_SLOTKINDS
// CHECK-NOT: SLOT_UNKNOWN
// CHECK:     SLOT_FU0
// CHECK:     SLOT_FU1
// CHECK-NOT: SLOT_UNKNOWN
// CHECK:     #endif // GET_FORMATS_SLOTKINDS

// CHECK:       #ifdef GET_FORMATS_CLASS_DEF
// CHECK:       class TESTSlotKind: public MCSlotKind
// CHECK:       #endif // GET_FORMATS_CLASS_DEF

// Check there is no wild instantiation of the default slot
// CHECK:       #ifdef GET_FORMATS_SLOTS_DEFS
// CHECK:       static constexpr const TESTSlotInfo TESTSlots[] = {

// CHECK:       TESTSlotKind::TEST_SLOT_FU0,
// CHECK-NEXT:  "fu0"
// CHECK-NEXT:  12
// CHECK:       TESTSlotKind::TEST_SLOT_FU1,
// CHECK-NEXT:  "fu1"
// CHECK NEXT:  42,
// CHECK-NOT:   "unknown",
// CHECK:       #endif // GET_FORMATS_SLOTS_DEFS
}
