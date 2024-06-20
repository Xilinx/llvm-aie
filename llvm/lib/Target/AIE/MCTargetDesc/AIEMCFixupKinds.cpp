//===-- AIEMCFixupKinds.cpp - AIE1 Specific Fixup Entries ------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/AIEMCFixupKinds.h"
#include "AIEMCTargetDesc.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>

using namespace llvm;

SmallVector<FixupField> AIEMCFixupKinds::convertFieldsLocsInFixupFields(
    ArrayRef<const MCFormatField *> Fields) {
  SmallVector<FixupField> FixupFields;
  for (auto &Field : Fields)
    FixupFields.emplace_back(Field->getOffsets().LeftOffset, Field->getSize());
  return FixupFields;
}

const SmallVector<FixupField> &
AIEMCFixupKinds::getFixupFields(MCFixupKind Kind) const {
  auto It = FixupFieldsInfos.find(Kind);
  if (It == FixupFieldsInfos.end())
    llvm_unreachable("Missing fixups definition into the FixupFieldsInfos");
  return It->second;
}

MCFixupKind AIEMCFixupKinds::findFixupfromFixupFields(
    const MCInst &Inst, unsigned FormatSize,
    const SmallVector<FixupField> &Fields) const {
  if (Fields.empty())
    llvm_unreachable("No field to map");

  auto CandidatesIt = FixupFieldsMapper.find(Fields);
  if (CandidatesIt == FixupFieldsMapper.end())
    llvm_unreachable("Missing fields definition into the FixupFieldsMapper");

  // Retrieve the signedness property of the relocatable instruction
  FixupFlag InstrSignedness = getInstrFieldSignedness(Inst.getOpcode());
  auto FixupPredicate = [&](unsigned Candidate) -> bool {
    // If the InstrSignedness is Unrestricted, it means that we don't care
    // about the signedness of the fixup/reloc. Thus, if the size of the
    // format is correct, we can select the candidate.
    if (getFixupFormatSize(Candidate) == FormatSize &&
        InstrSignedness == FixupFlag::Unrestricted)
      return true;

    return getFixupFormatSize(Candidate) == FormatSize &&
           getFixupSignedness(Candidate) == InstrSignedness;
  };

  const std::set<unsigned> &Candidates = CandidatesIt->second;
  // Select the fixup satisfiying the predicate above.
  auto FixupSelectionIt =
      std::find_if(Candidates.begin(), Candidates.end(), FixupPredicate);

  if (FixupSelectionIt == Candidates.end())
    llvm_unreachable("No fixup found");
  return static_cast<MCFixupKind>(*FixupSelectionIt);
}

unsigned AIEMCFixupKinds::getFixupFormatSize(unsigned Fixup) const {
  auto It = FixupFormatSize.find(Fixup);
  if (It == FixupFormatSize.end()) {
    llvm::dbgs() << "Fixup number " << Fixup
                 << "not defined in the FixupFormatSize Table\n";
    llvm_unreachable("Undefined Fixup in FixupFormatSize");
  }
  return It->second;
}

/// Return the specific Flag of a given fixup.
FixupFlag AIEMCFixupKinds::getFixupSignedness(unsigned Fixup) const {
  auto It = FixupFlagMap.find(Fixup);
  if (It == FixupFlagMap.end()) {
    llvm::dbgs() << "Fixup number " << Fixup
                 << "not defined in the FixupFieldsMapper\n";
    llvm_unreachable("Undefined Fixup in FixupFieldsMapper");
  }
  return It->second;
}

FixupFlag AIEMCFixupKinds::getInstrFieldSignedness(unsigned Opcode) const {
  auto It = InstrFixupFlags.find(Opcode);
  if (It == InstrFixupFlags.end())
    return FixupFlag::Unrestricted;
  return It->second;
}
