//===- AIE2P.td - Describe the AIE2p Target Machine *- tablegen --------*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
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

include "aie2p/AIE2PRegisterInfo.td"
include "aie2p/AIE2PSchedule.td"
include "aie2p/AIE2PCallingConv.td"
include "aie2p/AIE2PRegisterBanks.td"

class AIEGenericInstruction : GenericInstruction {
  let Namespace = "AIE2P";
}
include "AIEInstrGISel.td"
include "aie2p/AIE2PInstrInfo.td"
include "aie2p/AIE2PCopyMaterialization.td"

include "AIECombine.td"

//===----------------------------------------------------------------------===//
// AIEngine V2 processors supported.
//===----------------------------------------------------------------------===//

def : ProcessorModel<"aie2p", AIE2PSchedModel, []>;

//===----------------------------------------------------------------------===//
// Define the AIE2PEngine target.
//===----------------------------------------------------------------------===//

def AIE2PInstrInfo : InstrInfo {
  let guessInstructionProperties = 0;
}

def AIE2PAsmParser : AsmParser {
  let ShouldEmitMatchRegisterAltName = 1;
  let AllowDuplicateRegisterNames = 1;
}

def AIE2PAsmWriter : AsmWriter {
  int PassSubtarget = 1;
}

def AIECodeGenFormat : CodeGenFormat {
}

def AIE2P : Target {
  let InstructionSet = AIE2PInstrInfo;
  let AssemblyParsers = [AIE2PAsmParser];
  let AssemblyWriters = [AIE2PAsmWriter];
  let AllowRegisterRenaming = 1;
}
