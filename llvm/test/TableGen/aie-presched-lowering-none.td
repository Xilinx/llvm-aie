//===- aie-presched-lowering-none.td -----------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: llvm-tblgen --gen-aie-presched-lowering -I %p/../../include -I %p/Common %s | FileCheck %s

// Verify TableGen generates a valid getPseudoBranchExpandInfo()
// without any PreSchedInstExpansion records.

include "aie-arch-with-regs.td"

def TEST_INSTR : TestInstruction {
  let OutOperandList = (outs);
  let InOperandList = (ins);
}

// CHECK: std::optional<AIEBaseInstrInfo::PseudoBranchExpandInfo>
// CHECK: TestAIEInstrInfo::getPseudoBranchExpandInfo(const MachineInstr &MI) const {
// CHECK:   return {};
// CHECK: }
