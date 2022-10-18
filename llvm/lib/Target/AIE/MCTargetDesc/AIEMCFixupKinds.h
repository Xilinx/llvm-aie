//===-- AIEMCFixupKinds.h - AIE Specific Fixup Entries --------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEMCFIXUPKINDS_H
#define LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEMCFIXUPKINDS_H

#include "AIEMCFormats.h"
#include "AIEMCTargetDesc.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/Transforms/Utils/ASanStackFrameLayout.h"
#include <algorithm>
#include <map>
#include <set>

namespace llvm {

/// Represents a (unique) instruction field to be patched
/// as a fixup is a set of (possibly multiple) instruction fields.
/// NOTE: It is needed as the (LLVM) base data structure doesn't allow the
/// registration of a Fixup performing a patch on mutliple fields.
///
/// MSB          _______FixupField_______            LSB
///  |----------|------------------------|------------|
///   =======> FP.O                      |
///   ============================> FP.O + FP.S
/// Big-endian indexing:
/// FP.O = FixupField.Offset
/// FP.S = FixupField.Size
struct FixupField {
  // NOTE: mandatory for the usage of emplace_back()
  FixupField(unsigned Offset, unsigned Size) : Offset(Offset), Size(Size) {}
  /// Offset (in Bits) of the field (Big-endian indexing currently)
  unsigned Offset;
  /// Size (in Bits) of the field
  unsigned Size;
  // Needed when searching into the std::map
  inline bool operator<(const FixupField &Rhs) const {
    return std::tie(Offset, Size) < std::tie(Rhs.Offset, Rhs.Size);
  }
};

/// Flag of the fixup indicating whether there is an additionnal signedness
/// criterion to consider when choosing a particular fixup.
enum class FixupFlag { Unrestricted, isDataSigned, isDataUnsigned };

/// Main interface (AIE version agnostic) which implement the logic for
/// Fixup translation in the MCCodeEmitter. It is based on 5 tables (whose 4 of
/// them are currently auto-generated).
class AIEMCFixupKinds {
  /// Table giving for each fixup, the relocatable field(s) that it targets.
  /// AIE Fixup -> FixupField(s)
  const std::map<unsigned, SmallVector<FixupField>> &FixupFieldsInfos;
  /// Table mapping a vector of relocatable field(s) to a set of target fixups.
  /// FixupField(s) -> { AIE-Fixup0, AIE-Fixup1, ... }.
  const std::map<SmallVector<FixupField>, std::set<unsigned>>
      &FixupFieldsMapper;
  /// Table mapping each fixup to its target Format size
  const std::map<unsigned, unsigned> &FixupFormatSize;
  /// This Table maps, for each target fixup, the corresponding fixup flag.
  const std::map<unsigned, FixupFlag> &FixupFlagMap;
  /// Table mapping an opcode to a set of fixup flags, used mostly to pick the
  /// right fixup if an ambiguity is present (on AIE1, MOV_S20 and MOV_U20 need
  /// a patch on exactly the same bits, but the first one needs a "signed" data
  /// whereas the other one needs an "unsigned" data. This fact is checked by
  /// the linker. See in lld/ELF/Arch/AIE.cpp-AIERela.inc checkUInt() and
  /// checkInt()). It results in a different choice of fixups and thus,
  /// relocations.
  const std::map<unsigned, FixupFlag> &InstrFixupFlags;

public:
  AIEMCFixupKinds(
      const std::map<unsigned, SmallVector<FixupField>> &FixupFieldsInfos,
      const std::map<SmallVector<FixupField>, std::set<unsigned>>
          &FixupFieldsMapper,
      const std::map<unsigned, unsigned> &FixupFormatSize,
      const std::map<unsigned, FixupFlag> &FixupFlagMap,
      const std::map<unsigned, FixupFlag> &InstrFixupFlags)
      : FixupFieldsInfos(FixupFieldsInfos),
        FixupFieldsMapper(FixupFieldsMapper), FixupFormatSize(FixupFormatSize),
        FixupFlagMap(FixupFlagMap), InstrFixupFlags(InstrFixupFlags) {}

  static bool isTargetFixup(MCFixupKind Kind) {
    return Kind >= FirstTargetFixupKind;
  }

  /// Convert the Offsets of the field location into a vector of FixupField
  static SmallVector<FixupField>
  convertFieldsLocsInFixupFields(const ArrayRef<const MCFormatField *> Fields);

  /// Retrieve the set of fields targeted by the fixup.
  /// A FixupField represents one contiguous field that we need to patch.
  const SmallVector<FixupField> &getFixupFields(MCFixupKind Kind) const;

  /// Retrieve the representative Fixup from a set of Fields. The MCInst
  /// description is used to drive the choice if an ambiguity is present.
  /// NOTE: FormatSize is the size of the VLIW instruction, which is not
  /// necessarily the size of the instruction represented by the InstrDesc
  /// of Inst (as it could be a standalone instruction).
  MCFixupKind
  findFixupfromFixupFields(const MCInst &Inst, unsigned FormatSize,
                           const SmallVector<FixupField> &Fields) const;

private:
  /// Return the targeted Format Size of a Fixup
  unsigned getFixupFormatSize(unsigned Fixup) const;

  /// Return the specific Flag of a given fixup.
  FixupFlag getFixupSignedness(unsigned Fixup) const;

  /// Return the FixupFlag binded to an instruction or "Unrestricted".
  FixupFlag getInstrFieldSignedness(unsigned Opcode) const;
};

} // end namespace llvm

#endif
