//===- regular_slot_logic.td -------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// Testing if everything works as intended.

// RUN: llvm-tblgen --gen-instr-format -I %p -I %p/../ -I %p/../../../../include %s 2>&1 | FileCheck %s
include "Inputs/CommonSlotTesting.td"

let Namespace = "TEST" in
{
  let isDefaultSlot = true in
  def default_slot : InstSlot<"fu0", 0, true>;
  def fu1_slot     : InstSlot<"fu1", 32>;
  def fu2_slot     : InstSlot<"fsdsds2", 45>;

// CHECK:         #ifdef GET_FORMATS_SLOTKINDS
// CHECK-NOT:     SLOT_FU0,
// CHECK:         SLOT_FU1,
// CHECK-NEXT:    SLOT_FSDSDS2,
// CHECK:         #endif // GET_FORMATS_SLOTKINDS

// CHECK:       #ifdef GET_FORMATS_HEADER
// CHECK:       class TESTSlotInfo;
// CHECK-NOT:   extern const TESTSlotInfo fu0_slot;
// CHECK-NEXT:  extern const TESTSlotInfo fu1_slot;
// CHECK-NEXT:  extern const TESTSlotInfo fu2_slot;
// CHECK-NOT:   extern const TESTSlotInfo fu0_slot;
// CHECK:       #endif // GET_FORMATS_HEADER

// CHECK:       #ifdef GET_FORMATS_CLASS_DEF
// CHECK:       class TESTSlotKind: public MCSlotKind
// CHECK:       #endif // GET_FORMATS_CLASS_DEF

// CHECK:       #ifdef GET_FORMATS_SLOTS_DEFS
// CHECK:       constexpr const TESTSlotInfo fu1_slot = {
// CHECK-NEXT:    TESTSlotKind::TEST_SLOT_FU1,
// CHECK-NEXT:    "fu1",
// CHECK-NEXT:    32,
// CHECK:       constexpr const TESTSlotInfo fu2_slot = {
// CHECK-NEXT:    TESTSlotKind::TEST_SLOT_FSDSDS2,
// CHECK-NEXT:    "fsdsds2",
// CHECK-NEXT:    45,
// CHECK:       #endif // GET_FORMATS_SLOTS_DEFS
}
