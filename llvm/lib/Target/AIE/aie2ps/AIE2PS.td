//===- AIE2PS.td - Describe the AIE2ps Target Machine *- tablegen -------*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
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

include "aie2ps/AIE2PSRegisterInfo.td"
include "aie2ps/AIE2PSSchedule.td"
include "aie2ps/AIE2PSCallingConv.td"
include "aie2ps/AIE2PSRegisterBanks.td"

class AIEGenericInstruction : GenericInstruction {
  let Namespace = "AIE2PS";
}

include "AIEInstrGISel.td"
include "aie2ps/AIE2PSInstrInfo.td"

include "AIECombine.td"

//===----------------------------------------------------------------------===//
// AIEngine AIE2ps processors supported.
//===----------------------------------------------------------------------===//

def : ProcessorModel<"aie2ps", AIE2PSSchedModel, []>;

//===----------------------------------------------------------------------===//
// Define the AIE2PSEngine target.
//===----------------------------------------------------------------------===//

def AIE2PSInstrInfo : InstrInfo {
  let guessInstructionProperties = 0;
}

def AIE2PSAsmParser : AsmParser {
  let ShouldEmitMatchRegisterAltName = 1;
  let AllowDuplicateRegisterNames = 1;
}

def AIE2PSAsmWriter : AsmWriter {
  int PassSubtarget = 1;
}

def AIECodeGenFormat : CodeGenFormat {
}

def AIE2PS : Target {
  let InstructionSet = AIE2PSInstrInfo;
  let AssemblyParsers = [AIE2PSAsmParser];
  let AssemblyWriters = [AIE2PSAsmWriter];
  let AllowRegisterRenaming = 1;
}
