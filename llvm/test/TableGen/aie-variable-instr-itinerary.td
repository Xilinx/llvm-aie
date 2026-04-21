//===- aie-variable-instr-itinerary.td ---------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
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

let Itinerary = II_MOV, ItineraryRegPairs = [ItinRegClassPair<II_MOV_GPR,[OperandRegClass<0, GPR>]>]  in
  def MOV_instr_GPR : MOVInstruction<>;

let Itinerary = II_MOV, ItineraryRegPairs = [ItinRegClassPair<II_MOV_PTR,[OperandRegClass<0, PTR>]>]  in
  def MOV_instr_PTR : MOVInstruction<>;

let Itinerary = II_MOV, ItineraryRegPairs = [ItinRegClassPair<II_MOV_GPR,[OperandRegClass<1, GPR>]>, ItinRegClassPair<II_MOV_PTR,[OperandRegClass<1, PTR>]>]  in
  def MOV_instr_GPR_PTR : MOVInstruction<>;

let Itinerary = II_MOV, ItineraryRegPairs = [ItinRegClassPair<II_MOV_GPR,[OperandRegClass<0, GPR>,OperandRegClass<1, PTR>]>]  in
  def MOV_instr_GP : MOVInstruction<>;

let Itinerary = II_MOV, ItineraryRegPairs = [ItinRegClassPair<II_MOV_GPR,[OperandRegClass<0, PTR>,OperandRegClass<1, GPR>]>]  in
  def MOV_instr_PG : MOVInstruction<>;

let Itinerary = II_MOV, ItineraryRegPairs = [ItinRegClassPair<II_MOV_GPR,[OperandRegClass<0, GPR>,OperandRegClass<1, PTR>]>, ItinRegClassPair<II_MOV_PTR,[OperandRegClass<1, GPR>,OperandRegClass<0, PTR>]>]  in
  def MOV_instr_GP_PG : MOVInstruction<>;

let Itinerary = II_MOV, ItineraryRegPairs = [ItinRegClassPair<II_MOV_GPR,[OperandRegClass<0, GPR>]>]  in {
  def MOV_instr_GPR_1 : MOVInstruction<>;
  def MOV_instr_GPR_2 : MOVInstruction<>;  
}

let Itinerary = II_MOV, ItineraryRegPairs = [ItinRegClassPair<II_MOV_GPR,[OperandRegClass<1, GPR>]>, ItinRegClassPair<II_MOV_PTR,[OperandRegClass<1, PTR>]>]  in {
  def MOV_instr_GPR_PTR_1 : MOVInstruction<>;
  def MOV_instr_GPR_PTR_2 : MOVInstruction<>;
}

let Itinerary = II_MOV, ItineraryRegPairs = [ItinRegClassPair<II_MOV_GPR,[OperandRegClass<0, GPR>,OperandRegClass<1, PTR>]>, ItinRegClassPair<II_MOV_PTR,[OperandRegClass<1, GPR>,OperandRegClass<0, PTR>]>]  in {
  def MOV_instr_GP_PG_1 : MOVInstruction<>;
  def MOV_instr_GP_PG_2 : MOVInstruction<>;  
}

// CHECK:      static constexpr llvm::OperandRCRequirement const OperandRCRequirements[] = {
// CHECK-NEXT: // II_MOV_GPR 0
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT:   {1, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_GPR 2
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_MOV_GPR 3
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_MOV_GPR 4
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_MOV_GPR 5
// CHECK-NEXT:   {1, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_MOV_PTR 6
// CHECK-NEXT:   {1, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_GPR 7
// CHECK-NEXT:   {1, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_MOV_PTR 8
// CHECK-NEXT:   {1, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_GPR 9
// CHECK-NEXT:   {1, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_MOV_PTR 10
// CHECK-NEXT:   {1, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_GPR 11
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT:   {1, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_PTR 13
// CHECK-NEXT:   {1, &TestAIE::GPRRegClass},
// CHECK-NEXT:   {0, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_GPR 15
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT:   {1, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_PTR 17
// CHECK-NEXT:   {1, &TestAIE::GPRRegClass},
// CHECK-NEXT:   {0, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_GPR 19
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT:   {1, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_PTR 21
// CHECK-NEXT:   {1, &TestAIE::GPRRegClass},
// CHECK-NEXT:   {0, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_GPR 23
// CHECK-NEXT:   {0, &TestAIE::PTRRegClass},
// CHECK-NEXT:   {1, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_MOV_PTR 25
// CHECK-NEXT:   {0, &TestAIE::PTRRegClass},
// CHECK-NEXT: };
// CHECK-EMPTY:
// CHECK-NEXT: static constexpr SchedVariantInfo const SchedVariants[] = {
// CHECK-NEXT: // MOV_instr_GP 0
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[0], 2)},
// CHECK-NEXT: // MOV_instr_GPR 1
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[2], 1)},
// CHECK-NEXT: // MOV_instr_GPR_1 2
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[3], 1)},
// CHECK-NEXT: // MOV_instr_GPR_2 3
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[4], 1)},
// CHECK-NEXT: // MOV_instr_GPR_PTR 4
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[5], 1)},
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_PTR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[6], 1)},
// CHECK-NEXT: // MOV_instr_GPR_PTR_1 6
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[7], 1)},
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_PTR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[8], 1)},
// CHECK-NEXT: // MOV_instr_GPR_PTR_2 8
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[9], 1)},
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_PTR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[10], 1)},
// CHECK-NEXT: // MOV_instr_GP_PG 10
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[11], 2)},
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_PTR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[13], 2)},
// CHECK-NEXT: // MOV_instr_GP_PG_1 12
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[15], 2)},
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_PTR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[17], 2)},
// CHECK-NEXT: // MOV_instr_GP_PG_2 14
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[19], 2)},
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_PTR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[21], 2)},
// CHECK-NEXT: // MOV_instr_PG 16
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[23], 2)},
// CHECK-NEXT: // MOV_instr_PTR 17
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_PTR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[25], 1)},
// CHECK-NEXT: };
// CHECK-EMPTY:
// CHECK-NEXT: static constexpr InstrVariantInfo const InstrVariantInfos[] = {
// CHECK-NEXT:   {TestAIE::MOV_instr_GP, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[0], 1)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GPR, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[1], 1)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GPR_1, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[2], 1)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GPR_2, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[3], 1)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GPR_PTR, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[4], 2)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GPR_PTR_1, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[6], 2)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GPR_PTR_2, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[8], 2)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GP_PG, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[10], 2)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GP_PG_1, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[12], 2)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GP_PG_2, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[14], 2)},
// CHECK-NEXT:   {TestAIE::MOV_instr_PG, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[16], 1)},
// CHECK-NEXT:   {TestAIE::MOV_instr_PTR, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[17], 1)},
// CHECK-NEXT: };
// CHECK-EMPTY:
// CHECK-NEXT: // Interface object providing access to the variant tables.
// CHECK-NEXT: static const VarItinInterface TestAIEVarItinInterfaceImpl = {
// CHECK-NEXT:   InstrVariantInfos
// CHECK-NEXT: };
// CHECK-EMPTY:
// CHECK-NEXT: VarItinInterface TestAIEInstrInfo::getVarItinInterface() const {
// CHECK-NEXT:   return TestAIEVarItinInterfaceImpl;
// CHECK-NEXT: }
