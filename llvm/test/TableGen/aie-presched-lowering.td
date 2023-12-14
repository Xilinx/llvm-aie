//===- aie-presched-lowering.td ----------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: llvm-tblgen --gen-aie-presched-lowering -I %p/../../include -I %p/Common %s | FileCheck %s

// Verify how TableGen uses PreSchedInstExpansion to generate
// getPseudoBranchExpandInfo().

include "aie-arch-with-regs.td"

class Pseudo<dag outs, dag ins> : TestInstruction {
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let isPseudo = 1;
}

def TARGET_INSTR : TestInstruction {
  let OutOperandList = (outs GPR:$rd);
  let InOperandList = (ins i32imm:$val);
}
def BARRIER_INSTR : TestInstruction {
  let OutOperandList = (outs);
  let InOperandList = (ins);
}

def PSEUDO : Pseudo<(outs GPR:$rd), (ins)>,
             PreSchedInstExpansion<TARGET_INSTR, BARRIER_INSTR>;

// CHECK: std::optional<AIEBaseInstrInfo::PseudoBranchExpandInfo>
// CHECK: TestAIEInstrInfo::getPseudoBranchExpandInfo(const MachineInstr &MI) const {
// CHECK:   switch (MI.getOpcode()) {
// CHECK:   default: return {};
// CHECK:   case TestAIE::PSEUDO:
// CHECK:     return PseudoBranchExpandInfo{TestAIE::TARGET_INSTR, TestAIE::BARRIER_INSTR};
// CHECK:   }
// CHECK: }
