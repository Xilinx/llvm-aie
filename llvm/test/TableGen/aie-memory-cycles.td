//===- aie-memory-cycles.td --------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: llvm-tblgen --gen-aie-memory-cycles -I %p/../../include -I %p/Common %s | FileCheck %s

// Verify how TableGen uses MemInstrItinData to generate
// getFirstMemoryCycle() and getLastMemoryCycle()

include "aie-arch-with-regs.td"

def II_ST : InstrItinClass;
def II_LATE_ST : InstrItinClass;
def II_PART_ST : InstrItinClass;
def II_OTHER : InstrItinClass;
def AIEItineraries : ProcessorItineraries<[], [], [
  MemInstrItinData<II_ST, /*stages*/[], [1,1], MemoryCycles<5>>,
  MemInstrItinData<II_LATE_ST, /*stages*/[], [1,1], MemoryCycles<7>>,
  MemInstrItinData<II_PART_ST, /*stages*/[], [1,1], MemoryCycles<5, 11>>,
  InstrItinData<II_OTHER, /*stages*/[], [1,1]>
]>;

def AIESchedModel : SchedMachineModel {
  let Itineraries = AIEItineraries;
  let CompleteModel = 0;
}

def : ProcessorModel<"TestAIE", AIESchedModel, []>;

class StoreInstruction : TestInstruction {
  let OutOperandList = (outs );
  let InOperandList = (ins GPR:$val, i32imm:$adr);
}

let Itinerary = II_ST in
  def ST : StoreInstruction<>;
let Itinerary = II_LATE_ST in
  def LATE_ST : StoreInstruction<>;
let Itinerary = II_PART_ST in
  def PART_ST : StoreInstruction<>;
let Itinerary = II_OTHER in
  def FAKE_ST : StoreInstruction<>;

// CHECK:      std::optional<int>
// CHECK-NEXT: TestAIEInstrInfo::getFirstMemoryCycle(unsigned SchedClass) const {
// CHECK-NEXT:   switch (SchedClass) {
// CHECK-NEXT:   default: return {};
// CHECK-NEXT:   case 2: return 7; // II_LATE_ST
// CHECK-NEXT:   case 3: return 5; // II_PART_ST
// CHECK-NEXT:   case 4: return 5; // II_ST
// CHECK-NEXT:   }
// CHECK-NEXT: }

// CHECK:      std::optional<int>
// CHECK-NEXT: TestAIEInstrInfo::getLastMemoryCycle(unsigned SchedClass) const {
// CHECK-NEXT:   switch (SchedClass) {
// CHECK-NEXT:   default: return {};
// CHECK-NEXT:   case 2: return 7; // II_LATE_ST
// CHECK-NEXT:   case 3: return 11; // II_PART_ST
// CHECK-NEXT:   case 4: return 5; // II_ST
// CHECK-NEXT:   }
// CHECK-NEXT: }
