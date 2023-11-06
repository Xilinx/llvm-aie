//===- aie-memory-cycles-none.td ---------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: llvm-tblgen --gen-aie-memory-cycles -I %p/../../include -I %p/Common %s | FileCheck %s

// Verify TableGen generates valid getFirstMemoryCycle() and
// getLastMemoryCycle() in absence of MemInstrItinData record.

include "aie-arch-with-regs.td"

def II_OTHER : InstrItinClass;
def AIEItineraries : ProcessorItineraries<[], [], [
  InstrItinData<II_OTHER, /*stages*/[], [1,1]>
]>;

def AIESchedModel : SchedMachineModel {
  let Itineraries = AIEItineraries;
  let CompleteModel = 0;
}

def : ProcessorModel<"TestAIE", AIESchedModel, []>;

def FOO : TestInstruction {
  let OutOperandList = (outs );
  let InOperandList = (ins GPR:$val, i32imm:$adr);
  let Itinerary = II_OTHER;
}

// CHECK:      std::optional<int>
// CHECK-NEXT: TestAIEInstrInfo::getFirstMemoryCycle(unsigned SchedClass) const {
// CHECK-NEXT:   switch (SchedClass) {
// CHECK-NEXT:   default: return {};
// CHECK-NEXT:   }
// CHECK-NEXT: }

// CHECK-LABEL: int TestAIEInstrInfo::getMinFirstMemoryCycle() const {
// CHECK-NEXT:  return 2147483647;
// CHECK-LABEL:int TestAIEInstrInfo::getMaxFirstMemoryCycle() const {
// CHECK-NEXT:  return -2147483648;

// CHECK:      std::optional<int>
// CHECK-NEXT: TestAIEInstrInfo::getLastMemoryCycle(unsigned SchedClass) const {
// CHECK-NEXT:   switch (SchedClass) {
// CHECK-NEXT:   default: return {};
// CHECK-NEXT:   }
// CHECK-NEXT: }

// CHECK-LABEL: TestAIEInstrInfo::getMinLastMemoryCycle() const {
// CHECK-NEXT:  return 2147483647;
// CHECK-LABEL: int TestAIEInstrInfo::getMaxLastMemoryCycle() const {
// CHECK-NEXT:  return -2147483648;
