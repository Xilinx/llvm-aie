//===- aie-variable-instr-itinerary.td ---------------------*- tablegen -*-===//
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
// getOperandIdxForSchedClassDisambiguation() and getSchedClass()

include "aie-arch-with-regs.td"

def II_MOV : InstrItinClass;
def II_MOV_GPR : InstrItinClass;
def II_MOV_PTR : InstrItinClass;
def II_OTHER : InstrItinClass;

def AIEItineraries : ProcessorItineraries<[], [], [
  InstrItinData<II_MOV, /*stages*/[], [1,1]>,
  InstrItinData<II_MOV_GPR, /*stages*/[], [1,1]>,
  InstrItinData<II_MOV_PTR, /*stages*/[], [1,1]>,
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

let Itinerary = II_MOV, ItineraryRegPairs = [ItinRegClassPair<II_MOV_GPR,GPR>]  in
  def MOV_instr_GPR : MOVInstruction<>;

let Itinerary = II_MOV, ItineraryRegPairs = [ItinRegClassPair<II_MOV_PTR,PTR>]  in
  def MOV_instr_PTR : MOVInstruction<>;

let Itinerary = II_MOV, ItineraryRegPairs = [ItinRegClassPair<II_MOV_GPR,GPR, 1>, ItinRegClassPair<II_MOV_PTR,PTR, 1>]  in
  def MOV_instr_GPR_PTR : MOVInstruction<>;


let Itinerary = II_MOV, ItineraryRegPairs = [ItinRegClassPair<II_MOV_GPR,GPR>]  in {
  def MOV_instr_GPR_1 : MOVInstruction<>;
  def MOV_instr_GPR_2 : MOVInstruction<>;  
}

let Itinerary = II_MOV, ItineraryRegPairs = [ItinRegClassPair<II_MOV_GPR,GPR, 1>, ItinRegClassPair<II_MOV_PTR,PTR, 1>]  in {
  def MOV_instr_GPR_PTR_1 : MOVInstruction<>;
  def MOV_instr_GPR_PTR_2 : MOVInstruction<>;
}

// CHECK:      unsigned TestAIEInstrInfo::getSchedClass(
// CHECK-NEXT:     const MCInstrDesc &Desc,
// CHECK-NEXT:     iterator_range<const MachineOperand *> Operands,
// CHECK-NEXT:     const MachineRegisterInfo &MRI) const {
// CHECK-NEXT:   if (Operands.begin() == Operands.end())
// CHECK-NEXT:     return Desc.getSchedClass();
// CHECK-EMPTY:
// CHECK-NEXT:   auto checkRCForOperand =
// CHECK-NEXT:       [&Operands, &MRI](const TargetRegisterClass &ExpectedRC, unsigned OpIdx) {
// CHECK-NEXT:         const MachineOperand &MO = *std::next(Operands.begin(), OpIdx);
// CHECK-NEXT:         assert(MO.isReg() && "Operand must be of type Reg");
// CHECK-NEXT:         const Register Reg = MO.getReg();
// CHECK-NEXT:         const TargetRegisterClass *RC =
// CHECK-NEXT:             Reg.isVirtual() ? MRI.getRegClass(Reg) : nullptr;
// CHECK-NEXT:         return AIEBaseInstrInfo::regClassMatches(ExpectedRC, RC, Reg);
// CHECK-NEXT:       };
// CHECK-EMPTY:
// CHECK-NEXT:   switch (Desc.getOpcode()) {
// CHECK-NEXT:   default:
// CHECK-NEXT:     return Desc.getSchedClass();
// CHECK-NEXT:   case TestAIE::MOV_instr_GPR:
// CHECK-NEXT:   case TestAIE::MOV_instr_GPR_1:
// CHECK-NEXT:   case TestAIE::MOV_instr_GPR_2:
// CHECK-NEXT:   {
// CHECK-NEXT:     if (checkRCForOperand(TestAIE::GPRRegClass,0))
// CHECK-NEXT:       return TestAIE::Sched::II_MOV_GPR;
// CHECK-NEXT:     LLVM_DEBUG(dbgs() << "No matching RegClass found for instruction: "
// CHECK-NEXT:       << Desc.getOpcode() << "\n");
// CHECK-NEXT:     return Desc.getSchedClass();
// CHECK-NEXT:   }
// CHECK-NEXT:   case TestAIE::MOV_instr_GPR_PTR:
// CHECK-NEXT:   case TestAIE::MOV_instr_GPR_PTR_1:
// CHECK-NEXT:   case TestAIE::MOV_instr_GPR_PTR_2:
// CHECK-NEXT:   {
// CHECK-NEXT:     if (checkRCForOperand(TestAIE::GPRRegClass,1))
// CHECK-NEXT:       return TestAIE::Sched::II_MOV_GPR;
// CHECK-NEXT:     if (checkRCForOperand(TestAIE::PTRRegClass,1))
// CHECK-NEXT:       return TestAIE::Sched::II_MOV_PTR;
// CHECK-NEXT:     LLVM_DEBUG(dbgs() << "No matching RegClass found for instruction: "
// CHECK-NEXT:       << Desc.getOpcode() << "\n");
// CHECK-NEXT:     return Desc.getSchedClass();
// CHECK-NEXT:   }
// CHECK-NEXT:   case TestAIE::MOV_instr_PTR:
// CHECK-NEXT:   {
// CHECK-NEXT:     if (checkRCForOperand(TestAIE::PTRRegClass,0))
// CHECK-NEXT:       return TestAIE::Sched::II_MOV_PTR;
// CHECK-NEXT:     LLVM_DEBUG(dbgs() << "No matching RegClass found for instruction: "
// CHECK-NEXT:       << Desc.getOpcode() << "\n");
// CHECK-NEXT:     return Desc.getSchedClass();
// CHECK-NEXT:   }
// CHECK-NEXT:   }
// CHECK-NEXT: }
