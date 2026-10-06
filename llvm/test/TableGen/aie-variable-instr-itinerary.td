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
def II_ADD : InstrItinClass;
def II_ADD_GPR : InstrItinClass;
def II_ADD_PTR : InstrItinClass;

// STAGE tests: verify that the equivalence signature is based on stage field
// values (Cycles, Units, TimeInc, TM), not on stage record names.
//
// Two itineraries are equivalent iff every stage field matches. Note that
// TimeInc defines when the *next* stage begins, so the TimeInc of the last
// stage in a sequence is irrelevant.
//
// II_STAGE_A and II_STAGE_B have identical flat data, so they are equivalent
// even though each InstrStage instantiation creates a distinct anonymous record.
//
// The remaining pairs (CYC, FU, TINC, EMP) each test one kind of non-equiv-
// alence: different Cycles, different FuncUnits, different TimeInc on a
// non-last stage, and different empty-cycle counts respectively.
def FU_TEST : FuncUnit;
def FU_TEST2 : FuncUnit;

def II_STAGE_A : InstrItinClass;
def II_STAGE_B : InstrItinClass;
def II_STAGE_CYC2 : InstrItinClass;
def II_STAGE_EMP1 : InstrItinClass;
def II_STAGE_EMP4 : InstrItinClass;
def II_STAGE_FU2 : InstrItinClass;
def II_STAGE_TINC0 : InstrItinClass;
def II_STAGE_TINC1 : InstrItinClass;

def AIEItineraries : ProcessorItineraries<[FU_TEST, FU_TEST2], [], [
  InstrItinData<II_MOV, /*stages*/[], [1,1]>,
  InstrItinData<II_MOV_GPR, /*stages*/[], [1,1]>,
  InstrItinData<II_MOV_PTR, /*stages*/[], [1,1]>,
  InstrItinData<II_OTHER, /*stages*/[], [1,1]>,
  InstrItinData<II_ADD, /*stages*/[], [2,1]>,
  InstrItinData<II_ADD_GPR, /*stages*/[], [2,1]>,
  InstrItinData<II_ADD_PTR, /*stages*/[], [2,2]>,
  // Equivalent pair: identical stage data (Cycles=1, FU_TEST, TimeInc=0).
  InstrItinData<II_STAGE_A, [InstrStage<1, [FU_TEST], 0>], [1]>,
  InstrItinData<II_STAGE_B, [InstrStage<1, [FU_TEST], 0>], [1]>,
  // Non-equivalent: Cycles=2 vs Cycles=1.
  InstrItinData<II_STAGE_CYC2, [InstrStage<2, [FU_TEST], 0>], [1]>,
  // Non-equivalent: FU_TEST2 vs FU_TEST.
  InstrItinData<II_STAGE_FU2, [InstrStage<1, [FU_TEST2], 0>], [1]>,
  // Non-equivalent: TimeInc=0 vs TimeInc=1 on the *first* (non-last) stage.
  // TimeInc defines when the next stage begins; it is meaningful only for
  // stages that are followed by another stage.
  InstrItinData<II_STAGE_TINC0, [InstrStage<1, [FU_TEST], 0>, InstrStage<1, [FU_TEST], 1>], [1]>,
  InstrItinData<II_STAGE_TINC1, [InstrStage<1, [FU_TEST], 1>, InstrStage<1, [FU_TEST], 1>], [1]>,
  // Non-equivalent: empty-cycle prefix with TimeInc=1 vs TimeInc=4.
  // An empty-cycle stage (Cycles=0) uses TimeInc as a pure cycle-counter
  // advance without consuming a functional unit.
  InstrItinData<II_STAGE_EMP1, [InstrStage<0, [FU_TEST], 1>, InstrStage<1, [FU_TEST], 1>], [1]>,
  InstrItinData<II_STAGE_EMP4, [InstrStage<0, [FU_TEST], 4>, InstrStage<1, [FU_TEST], 1>], [1]>
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

// ADD instructions: II_ADD_GPR and II_ADD_PTR have different operand cycles,
// so they are non-equivalent itineraries and will be distinguished in the output.
class ADDInstruction : TestInstruction {
  int Inst = 2;
  let OutOperandList = (outs );
  let InOperandList = (ins GPR:$src1, GPR:$src2);
}

let Itinerary = II_ADD  in
  def ADD_instr : ADDInstruction<>;

let Itinerary = II_ADD, ItineraryRegPairs = [ItinRegClassPair<II_ADD_GPR,[OperandRegClass<0, GPR>]>]  in
  def ADD_instr_GPR : ADDInstruction<>;

let Itinerary = II_ADD, ItineraryRegPairs = [ItinRegClassPair<II_ADD_PTR,[OperandRegClass<0, PTR>]>]  in
  def ADD_instr_PTR : ADDInstruction<>;

let Itinerary = II_ADD, ItineraryRegPairs = [ItinRegClassPair<II_ADD_GPR,[OperandRegClass<0, GPR>]>, ItinRegClassPair<II_ADD_PTR,[OperandRegClass<0, PTR>]>]  in
  def ADD_instr_GPR_PTR : ADDInstruction<>;

let Itinerary = II_ADD, ItineraryRegPairs = [ItinRegClassPair<II_ADD_GPR,[OperandRegClass<0, GPR>]>]  in {
  def ADD_instr_GPR_1 : ADDInstruction<>;
  def ADD_instr_GPR_2 : ADDInstruction<>;
}

// STAGE instructions: one per equivalence/non-equivalence scenario.
class STAGEInstruction : TestInstruction {
  int Inst = 3;
  let OutOperandList = (outs);
  let InOperandList = (ins GPR:$val);
}

// II_STAGE_A (Cycles=1) vs II_STAGE_CYC2 (Cycles=2): non-equivalent.
let Itinerary = II_STAGE_A, ItineraryRegPairs = [
    ItinRegClassPair<II_STAGE_A,    [OperandRegClass<0, GPR>]>,
    ItinRegClassPair<II_STAGE_CYC2, [OperandRegClass<0, PTR>]>] in
  def STAGE_cyc_instr : STAGEInstruction<>;

// II_STAGE_EMP1 (TimeInc=1 on empty) vs II_STAGE_EMP4 (TimeInc=4): non-equiv.
let Itinerary = II_STAGE_EMP1, ItineraryRegPairs = [
    ItinRegClassPair<II_STAGE_EMP1, [OperandRegClass<0, GPR>]>,
    ItinRegClassPair<II_STAGE_EMP4, [OperandRegClass<0, PTR>]>] in
  def STAGE_emp_instr : STAGEInstruction<>;

// II_STAGE_A ≡ II_STAGE_B (same stage data): both appear under II_STAGE_A.
let Itinerary = II_STAGE_A, ItineraryRegPairs = [
    ItinRegClassPair<II_STAGE_A, [OperandRegClass<0, GPR>]>,
    ItinRegClassPair<II_STAGE_B, [OperandRegClass<0, PTR>]>] in
  def STAGE_equiv_instr : STAGEInstruction<>;

// II_STAGE_A (FU_TEST) vs II_STAGE_FU2 (FU_TEST2): non-equivalent.
let Itinerary = II_STAGE_A, ItineraryRegPairs = [
    ItinRegClassPair<II_STAGE_A,   [OperandRegClass<0, GPR>]>,
    ItinRegClassPair<II_STAGE_FU2, [OperandRegClass<0, PTR>]>] in
  def STAGE_fu_instr : STAGEInstruction<>;

// II_STAGE_TINC0 (first stage TimeInc=0) vs II_STAGE_TINC1 (TimeInc=1): non-equiv.
let Itinerary = II_STAGE_TINC0, ItineraryRegPairs = [
    ItinRegClassPair<II_STAGE_TINC0, [OperandRegClass<0, GPR>]>,
    ItinRegClassPair<II_STAGE_TINC1, [OperandRegClass<0, PTR>]>] in
  def STAGE_tinc_instr : STAGEInstruction<>;

// II_ADD_GPR and II_ADD_PTR have distinct operand cycles ([2,1] vs [2,2]),
// so they appear as separate entries in OperandRCRequirements (non-equivalence).
// II_MOV_GPR and II_MOV_PTR have the same operand cycles ([1,1]), so they are
// equivalent and both appear under the II_MOV_GPR label (equivalence).

// CHECK:      static constexpr llvm::OperandRCRequirement const OperandRCRequirements[] = {
// CHECK-NEXT: // II_ADD_GPR 0
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_ADD_GPR 1
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_ADD_GPR 2
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_ADD_GPR 3
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_ADD_PTR 4
// CHECK-NEXT:   {0, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_ADD_PTR 5
// CHECK-NEXT:   {0, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_GPR 6
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT:   {1, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_GPR 8
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_MOV_GPR 9
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_MOV_GPR 10
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_MOV_GPR 11
// CHECK-NEXT:   {1, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_MOV_GPR 12
// CHECK-NEXT:   {1, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_GPR 13
// CHECK-NEXT:   {1, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_MOV_GPR 14
// CHECK-NEXT:   {1, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_GPR 15
// CHECK-NEXT:   {1, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_MOV_GPR 16
// CHECK-NEXT:   {1, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_GPR 17
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT:   {1, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_GPR 19
// CHECK-NEXT:   {1, &TestAIE::GPRRegClass},
// CHECK-NEXT:   {0, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_GPR 21
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT:   {1, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_GPR 23
// CHECK-NEXT:   {1, &TestAIE::GPRRegClass},
// CHECK-NEXT:   {0, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_GPR 25
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT:   {1, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_GPR 27
// CHECK-NEXT:   {1, &TestAIE::GPRRegClass},
// CHECK-NEXT:   {0, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_MOV_GPR 29
// CHECK-NEXT:   {0, &TestAIE::PTRRegClass},
// CHECK-NEXT:   {1, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_MOV_PTR 31
// CHECK-NEXT:   {0, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_STAGE_A 32
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_STAGE_CYC2 33
// CHECK-NEXT:   {0, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_STAGE_EMP1 34
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_STAGE_EMP4 35
// CHECK-NEXT:   {0, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_STAGE_A 36
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_STAGE_A 37
// CHECK-NEXT:   {0, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_STAGE_A 38
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_STAGE_FU2 39
// CHECK-NEXT:   {0, &TestAIE::PTRRegClass},
// CHECK-NEXT: // II_STAGE_TINC0 40
// CHECK-NEXT:   {0, &TestAIE::GPRRegClass},
// CHECK-NEXT: // II_STAGE_TINC1 41
// CHECK-NEXT:   {0, &TestAIE::PTRRegClass},
// CHECK-NEXT: };
// CHECK-EMPTY:
// CHECK-NEXT: static constexpr SchedVariantInfo const SchedVariants[] = {
// CHECK-NEXT: // ADD_instr_GPR 0
// CHECK-NEXT:   {TestAIE::Sched::II_ADD_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[0], 1)},
// CHECK-NEXT: // ADD_instr_GPR_1 1
// CHECK-NEXT:   {TestAIE::Sched::II_ADD_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[1], 1)},
// CHECK-NEXT: // ADD_instr_GPR_2 2
// CHECK-NEXT:   {TestAIE::Sched::II_ADD_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[2], 1)},
// CHECK-NEXT: // ADD_instr_GPR_PTR 3
// CHECK-NEXT:   {TestAIE::Sched::II_ADD_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[3], 1)},
// CHECK-NEXT:   {TestAIE::Sched::II_ADD_PTR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[4], 1)},
// CHECK-NEXT: // ADD_instr_PTR 5
// CHECK-NEXT:   {TestAIE::Sched::II_ADD_PTR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[5], 1)},
// CHECK-NEXT: // MOV_instr_GP 6
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[6], 2)},
// CHECK-NEXT: // MOV_instr_GPR 7
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[8], 1)},
// CHECK-NEXT: // MOV_instr_GPR_1 8
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[9], 1)},
// CHECK-NEXT: // MOV_instr_GPR_2 9
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[10], 1)},
// CHECK-NEXT: // MOV_instr_GPR_PTR 10
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[11], 1)},
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[12], 1)},
// CHECK-NEXT: // MOV_instr_GPR_PTR_1 12
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[13], 1)},
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[14], 1)},
// CHECK-NEXT: // MOV_instr_GPR_PTR_2 14
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[15], 1)},
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[16], 1)},
// CHECK-NEXT: // MOV_instr_GP_PG 16
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[17], 2)},
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[19], 2)},
// CHECK-NEXT: // MOV_instr_GP_PG_1 18
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[21], 2)},
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[23], 2)},
// CHECK-NEXT: // MOV_instr_GP_PG_2 20
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[25], 2)},
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[27], 2)},
// CHECK-NEXT: // MOV_instr_PG 22
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_GPR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[29], 2)},
// CHECK-NEXT: // MOV_instr_PTR 23
// CHECK-NEXT:   {TestAIE::Sched::II_MOV_PTR, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[31], 1)},
// CHECK-NEXT: // STAGE_cyc_instr 24
// CHECK-NEXT:   {TestAIE::Sched::II_STAGE_A, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[32], 1)},
// CHECK-NEXT:   {TestAIE::Sched::II_STAGE_CYC2, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[33], 1)},
// CHECK-NEXT: // STAGE_emp_instr 26
// CHECK-NEXT:   {TestAIE::Sched::II_STAGE_EMP1, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[34], 1)},
// CHECK-NEXT:   {TestAIE::Sched::II_STAGE_EMP4, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[35], 1)},
// CHECK-NEXT: // STAGE_equiv_instr 28
// CHECK-NEXT:   {TestAIE::Sched::II_STAGE_A, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[36], 1)},
// CHECK-NEXT:   {TestAIE::Sched::II_STAGE_A, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[37], 1)},
// CHECK-NEXT: // STAGE_fu_instr 30
// CHECK-NEXT:   {TestAIE::Sched::II_STAGE_A, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[38], 1)},
// CHECK-NEXT:   {TestAIE::Sched::II_STAGE_FU2, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[39], 1)},
// CHECK-NEXT: // STAGE_tinc_instr 32
// CHECK-NEXT:   {TestAIE::Sched::II_STAGE_TINC0, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[40], 1)},
// CHECK-NEXT:   {TestAIE::Sched::II_STAGE_TINC1, llvm::ArrayRef<llvm::OperandRCRequirement>(&OperandRCRequirements[41], 1)},
// CHECK-NEXT: };
// CHECK-EMPTY:
// CHECK-NEXT: static constexpr InstrVariantInfo const InstrVariantInfos[] = {
// CHECK-NEXT:   {TestAIE::ADD_instr_GPR, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[0], 1)},
// CHECK-NEXT:   {TestAIE::ADD_instr_GPR_1, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[1], 1)},
// CHECK-NEXT:   {TestAIE::ADD_instr_GPR_2, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[2], 1)},
// CHECK-NEXT:   {TestAIE::ADD_instr_GPR_PTR, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[3], 2)},
// CHECK-NEXT:   {TestAIE::ADD_instr_PTR, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[5], 1)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GP, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[6], 1)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GPR, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[7], 1)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GPR_1, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[8], 1)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GPR_2, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[9], 1)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GPR_PTR, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[10], 2)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GPR_PTR_1, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[12], 2)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GPR_PTR_2, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[14], 2)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GP_PG, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[16], 2)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GP_PG_1, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[18], 2)},
// CHECK-NEXT:   {TestAIE::MOV_instr_GP_PG_2, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[20], 2)},
// CHECK-NEXT:   {TestAIE::MOV_instr_PG, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[22], 1)},
// CHECK-NEXT:   {TestAIE::MOV_instr_PTR, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[23], 1)},
// CHECK-NEXT:   {TestAIE::STAGE_cyc_instr, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[24], 2)},
// CHECK-NEXT:   {TestAIE::STAGE_emp_instr, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[26], 2)},
// CHECK-NEXT:   {TestAIE::STAGE_equiv_instr, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[28], 2)},
// CHECK-NEXT:   {TestAIE::STAGE_fu_instr, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[30], 2)},
// CHECK-NEXT:   {TestAIE::STAGE_tinc_instr, llvm::ArrayRef<SchedVariantInfo>(&SchedVariants[32], 2)},
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
