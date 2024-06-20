//===- crash_null_slot_size.td -----------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// Testing if invalid size results in an assertion.

// RUN: not llvm-tblgen --gen-instr-format -I %p -I %p/../../../ -I %p/../../../../../../include %s 2>&1 | FileCheck %s

include "Inputs/CommonSlotTesting.td"

let Namespace = "TEST" in
{
  def fu0_slot     : InstSlot<"fu0",  0>;
  // CHECK: assertion failed
  // CHECK: Size of the Slot must be > 0 if the slot isn't the default one, current size being: 0
  // CHECK: Invalid size of the slot fu0_slot
}