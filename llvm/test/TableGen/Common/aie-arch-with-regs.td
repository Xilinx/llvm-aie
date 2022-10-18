//===- aie-arch-with-regs.td -------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"
include "llvm/Target/AIETarget.td"

def TestAIETargetInstrInfo : InstrInfo;

def TestAIE : Target {
  let InstructionSet = TestAIETargetInstrInfo;
}

def REG : Register<"REG">;
def GPR : RegisterClass<"TestAIE", [i32], 32, (add REG)>;

class TestInstruction : Instruction {
  let Namespace = "TestAIE";
}
