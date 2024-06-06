//===- aie-split-instr-mappings.td -------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: llvm-tblgen --gen-aie-split-instr-tables -I %p/../../include -I %p/Common %s | FileCheck %s

// Verify how TableGen uses SplitPseudo records to generate
// getOpcodeWithAtomicOperands() and getOpcodeWithTupleOperands().

include "aie-arch-with-regs.td"

class SplitPseudo<Instruction Inst> : TestInstruction {
  Instruction OriginalInstr = Inst;
  let OutOperandList = Inst.OutOperandList;
  let InOperandList = Inst.InOperandList;
}

def TARGET_INSTR : TestInstruction {
  let OutOperandList = (outs GPR:$rd);
  let InOperandList = (ins i32imm:$val);
}

def TARGET_INSTR_split : SplitPseudo<TARGET_INSTR>;

// CHECK: std::optional<unsigned>
// CHECK: TestAIEInstrInfo::getOpcodeWithAtomicOperands(unsigned Opcode) const {
// CHECK:   switch (Opcode) {
// CHECK:   default: return {};
// CHECK:   case TestAIE::TARGET_INSTR: return TestAIE::TARGET_INSTR_split;
// CHECK:   }
// CHECK: }

// CHECK: std::optional<unsigned>
// CHECK: TestAIEInstrInfo::getOpcodeWithTupleOperands(unsigned Opcode) const {
// CHECK:   switch (Opcode) {
// CHECK:   default: return {};
// CHECK:   case TestAIE::TARGET_INSTR_split: return TestAIE::TARGET_INSTR;
// CHECK:   }
// CHECK: }
