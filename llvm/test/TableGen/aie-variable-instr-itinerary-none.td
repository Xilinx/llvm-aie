//===- aie-no-variable-instr-itinerary.td -------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: llvm-tblgen --gen-aie-alternate-itinerary-emitter -I %p/../../include -I %p/Common %s | FileCheck %s

// Verify how TableGen uses InstrInfo to generate
// getOperandIdxForSchedClassDisambiguation() and getSchedClass() when no instuction have
// variable itinerary based on reg class

include "aie-arch-with-regs.td"

def II_MOV : InstrItinClass;
def II_OTHER : InstrItinClass;

def AIEItineraries : ProcessorItineraries<[], [], [
  InstrItinData<II_MOV, /*stages*/[], [1,1]>,
  InstrItinData<II_OTHER, /*stages*/[], [1,1]>
]>;

def AIESchedModel : SchedMachineModel {
  let Itineraries = AIEItineraries;
  let CompleteModel = 0;
}

def : ProcessorModel<"TestAIE", AIESchedModel, []>;

class MOVInstruction : TestInstruction {
  int Inst = 1;
  let OutOperandList = (outs );
  let InOperandList = (ins GPR:$val, i32imm:$adr);
}

let Itinerary = II_MOV  in
  def MOV_instr : MOVInstruction<>;

// CHECK:      unsigned TestAIEInstrInfo::getSchedClass(
// CHECK-NEXT:   const MCInstrDesc &Desc,
// CHECK-NEXT:   iterator_range<const MachineOperand *> Operands,
// CHECK-NEXT:   const MachineRegisterInfo &MRI) const {
// CHECK-NEXT:   return Desc.getSchedClass();
// CHECK-NEXT: }
