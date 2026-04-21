//===- aie-no-variable-instr-itinerary.td -------------------*- tablegen -*-===//
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

// CHECK:      static constexpr llvm::OperandRCRequirement const OperandRCRequirements[] = {
// CHECK-NEXT: };
// CHECK-EMPTY:
// CHECK-NEXT: static constexpr SchedVariantInfo const SchedVariants[] = {
// CHECK-NEXT: };
// CHECK-EMPTY:
// CHECK-NEXT: static constexpr InstrVariantInfo const InstrVariantInfos[] = {
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
