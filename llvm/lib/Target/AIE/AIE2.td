//===-- AIE2.td - Describe the AIEngine V2 Target Machine --*- tablegen -*-===//
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

include "AIE2RegisterInfo.td"
include "AIE2Schedule.td"
include "AIE2CallingConv.td"
include "AIE2InstrInfo.td"
include "AIE2RegisterBanks.td"

class AIEGenericInstruction : GenericInstruction {
  let Namespace = "AIE2";
}
include "AIEInstrGISel.td"

include "AIECombine.td"

//===----------------------------------------------------------------------===//
// AIEngine V2 processors supported.
//===----------------------------------------------------------------------===//

def : ProcessorModel<"aie2", AIE2SchedModel, []>;

//===----------------------------------------------------------------------===//
// Define the AIE2Engine target.
//===----------------------------------------------------------------------===//

def AIE2InstrInfo : InstrInfo {
  let guessInstructionProperties = 0;
}

def AIE2AsmParser : AsmParser {
  let ShouldEmitMatchRegisterAltName = 1;
  let AllowDuplicateRegisterNames = 1;
}

def AIE2AsmWriter : AsmWriter {
  int PassSubtarget = 1;
}

def AIECodeGenFormat : CodeGenFormat {
}

def AIE2 : Target {
  let InstructionSet = AIE2InstrInfo;
  let AssemblyParsers = [AIE2AsmParser];
  let AssemblyWriters = [AIE2AsmWriter];
  let AllowRegisterRenaming = 1;
}
