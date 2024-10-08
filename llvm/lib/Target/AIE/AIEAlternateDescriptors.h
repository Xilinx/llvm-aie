//== AIEAlternateDescriptors.h - Define Alternate descriptor Class *-C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares the AIEngine Alternate instruction descriptor class
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_SUPPORT_AIEALTERNATEDESCRIPTORS_H
#define LLVM_SUPPORT_AIEALTERNATEDESCRIPTORS_H

#include "AIEBaseSubtarget.h"
#include "MCTargetDesc/AIEMCFormats.h"

#include <unordered_map>

namespace llvm {

using MIAltDescsMap = std::unordered_map<MachineInstr *, const MCInstrDesc *>;

class AIEAlternateDescriptors {
  MIAltDescsMap AlternateDescs;

public:
  AIEAlternateDescriptors() = default;
  ~AIEAlternateDescriptors() = default;

  // Construct an alternate descriptor with the given alternate descriptors.
  AIEAlternateDescriptors(const MIAltDescsMap &AltDescs)
      : AlternateDescs(AltDescs) {}

  // Set the alternate descriptor for the given multi-opcode instruction.
  void setAlternateDescriptor(MachineInstr *MI, const unsigned AltInstOpcode) {
    const AIEBaseSubtarget &STI = AIEBaseSubtarget::get(*MI->getMF());
    const AIEBaseInstrInfo *TII = STI.getInstrInfo();

    AlternateDescs[MI] = &TII->get(AltInstOpcode);
  }

  // Return the alternate descriptor for the given multi-opcode instruction.
  std::optional<const MCInstrDesc *>
  getSelectedDescriptor(MachineInstr *MI) const {
    if (auto It = AlternateDescs.find(MI); It != AlternateDescs.end())
      return It->second;
    return std::nullopt;
  }

  const MCInstrDesc *getDesc(MachineInstr *MI) const {
    return getSelectedDescriptor(MI).value_or(&MI->getDesc());
  }

  // Return the alternate opcode for the given multi-opcode instruction.
  std::optional<unsigned> getSelectedOpcode(MachineInstr *MI) const {
    if (auto It = AlternateDescs.find(MI); It != AlternateDescs.end())
      return It->second->getOpcode();
    return std::nullopt;
  }

  unsigned getOpcode(MachineInstr *MI) const {
    return getSelectedOpcode(MI).value_or(MI->getDesc().getOpcode());
  }

  void clear() { AlternateDescs.clear(); }
};

} // end namespace llvm

#endif // LLVM_SUPPORT_AIEALTERNATEDESCRIPTOR_H
