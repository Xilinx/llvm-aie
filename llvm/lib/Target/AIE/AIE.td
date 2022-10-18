//===-- AIE.td - Describe the AIE Target Machine -------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

// AIE-specific TableGen interfaces
include "llvm/Target/AIETarget.td"

// includes extra definitions for the usage of the CodeGenFormat Backend
include "llvm/Target/CodeGenFormat.td"

def ptr0 : PtrValueType<i20, 0>;

//===----------------------------------------------------------------------===//
// Registers, calling conventions, instruction descriptions.
//===----------------------------------------------------------------------===//

include "AIERegisterInfo.td"
include "AIESchedule.td"
include "AIECallingConv.td"
include "AIEInstrInfo.td"
include "AIERegisterBanks.td"

class AIEGenericInstruction : GenericInstruction {
  let Namespace = "AIE";
}
include "AIEInstrGISel.td"

//===----------------------------------------------------------------------===//
// Named operands for CSR instructions.
//===----------------------------------------------------------------------===//

//include "AIESystemOperands.td"

//===----------------------------------------------------------------------===//
// AIEngine processors supported.
//===----------------------------------------------------------------------===//

def : ProcessorModel<"aie", AIESchedModel, []>;

//===----------------------------------------------------------------------===//
// Define the AIEngine target.
//===----------------------------------------------------------------------===//

def AIEInstrInfo : InstrInfo {
  let guessInstructionProperties = 0;
}

def AIEAsmParser : AsmParser {
  let ShouldEmitMatchRegisterAltName = 1;
  let AllowDuplicateRegisterNames = 1;
}

def AIEAsmWriter : AsmWriter {
  int PassSubtarget = 1;
}

def AIECodeGenFormat : CodeGenFormat {
}

def AIE : Target {
  let InstructionSet = AIEInstrInfo;
  let AssemblyParsers = [AIEAsmParser];
  let AssemblyWriters = [AIEAsmWriter];
  let AllowRegisterRenaming = 1;
}
