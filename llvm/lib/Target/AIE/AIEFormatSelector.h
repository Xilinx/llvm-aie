//=== AIEFormatSelector.h - VLIW Format Determination ---------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
/// \file
/// Utilities to select VLIW formats.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEFORMATSELECTOR_H
#define LLVM_LIB_TARGET_AIE_AIEFORMATSELECTOR_H

#include "AIEBundle.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"

namespace llvm {

class FunctionPass;
class VLIWFormat;

/// Creates a pass that iterates over the BUNDLE instructions to:
///  - select the right VLIW format for that bundle (essentially replacing the
///    BUNDLE opcode with a target-specific VLIW format opcode)
///  - re-order the instructions belonging to that bundle so that they appear
///    in the same order as their corresponding slots appear in the VLIW format.
FunctionPass *createAIEFormatSelector();

/// Reorders the instruction in \p Bundle within their parent basic block so
/// that they appear in the same order as their corresponding slot in \p Format.
/// \param Bundle A set of instructions that run in the same cycle
/// \param Format The requested VLIW format
/// \param BundleRoot If exists, the instruction under which all the
///                   instructions in \p Bundle are nested.
///                   E.g. a BUNDLE instruction.
/// \param InsertPoint Where to insert the re-ordered instructions
/// \param InsertMissingNops Whether to insert nops for empty slots.
void applyFormatOrdering(AIE::MachineBundle &Bundle, const VLIWFormat &Format,
                         MachineInstr *BundleRoot,
                         MachineBasicBlock::iterator InsertPoint,
                         bool InsertMissingNops = false);

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEFORMATSELECTOR_H
