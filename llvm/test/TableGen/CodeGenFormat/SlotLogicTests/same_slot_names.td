//===- same_slot_names.td ----------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// Testing that the Backend behaves correctly when no default slot is defined
// and when another slot already took the name of the "reserved" default one.

// RUN: not llvm-tblgen --gen-instr-format -I %p -I %p/../ -I %p/../../../../include %s 2>&1 | FileCheck %s

include "Inputs/CommonSlotTesting.td"

let Namespace = "TEST" in
{
  def fu0_slot     : InstSlot<"fu0",  12>;
  def u1t33        : InstSlot<"fu0",  42>;

// Let's check that this TableGen program crash.
// CHECK: Record Slot u1t33 and Slot fu0_slot have the same slot name (fu0)
}