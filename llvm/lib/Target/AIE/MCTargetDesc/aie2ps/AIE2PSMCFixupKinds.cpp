//===-- AIE2PSMCFixupKinds.cpp - AIE2ps Specific Fixup Entries --*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//

#include "AIE2PSMCFixupKinds.h"
#include "AIE2PSMCTargetDesc.h"
#include "AIEMCTargetDesc.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <memory>
#include <vector>

using namespace llvm;

// ZOL (Zero-Overhead Loop) instructions produce PC-relative fixups.
// This flag drives fixup selection in findFixupfromFixupFields() to
// disambiguate ZOL fixups (49-74) from absolute fixups (15-40, 109-134)
// that share the same field layout and format size.
const static std::map<unsigned, FixupFlag> AIE2PSInstrFixupFlags = {
    {AIE2PS::ADD_NC_alu_cg_or_alu_cg_ls_rel, FixupFlag::isPCRelative},
    {AIE2PS::ADD_NC_alu_cg_or_alu_cg_le_rel, FixupFlag::isPCRelative},
    {AIE2PS::ADDM_NC_mv_cg_or_mv_cg_ls_rel, FixupFlag::isPCRelative},
    {AIE2PS::ADDM_NC_mv_cg_or_mv_cg_le_rel, FixupFlag::isPCRelative},
};

// Need to be placed after InstrFixupFlags definition
// Define before include to use custom factory function below
#define AIE2PS_CUSTOM_FIXUP_KINDS
#define GET_MCFIXUPKINDS_IMPLEM
#include "FixupInfo/AIE2PSFixupInfo.inc"

// Override for AIE2PS to handle PC-relative fixup selection and
// ZOL sub-instruction fixup field resolution.
namespace llvm {
class AIE2PSMCFixupKinds : public AIEMCFixupKinds {
public:
  using AIEMCFixupKinds::AIEMCFixupKinds;

  bool isPCRelFixup(MCFixupKind Kind) const override {
    if (!isTargetFixup(Kind))
      return false;
    const unsigned FixupNum = Kind - AIE2PS::fixup_aie2ps_1 + 1;
    // ZOL (Zero-Overhead Loop) fixups: 49-74 (composite) and 139-140
    // (transient standalone, immediately translated by
    // translateFixupsInComposite).
    return (FixupNum >= 49 && FixupNum <= 74) || FixupNum == 139 ||
           FixupNum == 140;
  }

  /// Resolve sub-instruction fixup field mismatches for MV ZOL
  /// (Zero-Overhead Loop) instructions.
  ///
  /// The MV ZOL standalone encoding produces a single merged field {7,11}
  /// because the scrambled immediate bits i{6:0} and i{10:7} occupy
  /// contiguous instruction bits mv[14:4], but the fixup table stores them
  /// as two split fields {14,4}{7,7} reflecting the operand-bit-to-
  /// instruction-bit mapping, so this hook splits the merged field to
  /// match the table's representation.
  ///
  /// ALU ZOL (add.nc ls/le) does NOT need this hook: its standalone fields
  /// {0,5}{11,6} already exist in the FixupFieldsMapper, and the transient
  /// fixup_139 at FormatSize=4 with isPCRelative provides a direct match.
  ///
  /// Other relocatable instructions (jl, jnz, movxm) use the lng slot
  /// where standalone offsets coincide with composite offsets, so no
  /// correction is needed.
  std::optional<SmallVector<FixupField>> resolveSubInstFixupFields(
      unsigned Opcode, const SmallVector<FixupField> &Fields) const override {
    switch (Opcode) {
    case AIE2PS::ADDM_NC_mv_cg_or_mv_cg_ls_rel:
    case AIE2PS::ADDM_NC_mv_cg_or_mv_cg_le_rel:
      // MV ZOL: split the merged 11-bit field into 4+7 sub-fields matching
      // the fixup table's operand-bit-aware encoding.
      // mv = {0b1111111, i{6:0}, i{10:7}, 0b0101}
      //   i{10:7} (4 bits) at offset+7 from merged field start
      //   i{6:0}  (7 bits) at offset   from merged field start
      if (Fields.size() == 1 && Fields[0].Size == 11) {
        SmallVector<FixupField> Split;
        Split.emplace_back(Fields[0].Offset + 7, 4); // i{10:7}
        Split.emplace_back(Fields[0].Offset, 7);     // i{6:0}
        return Split;
      }
      return std::nullopt;

    default:
      return std::nullopt;
    }
  }
};

// Provide the factory function that returns our custom subclass
std::unique_ptr<AIEMCFixupKinds> createAIE2PSMCFixupKinds() {
  return std::make_unique<AIE2PSMCFixupKinds>(
      AIE2PSFixupFieldsInfos, AIE2PSFixupFieldsMapper, AIE2PSFixupFormatSize,
      AIE2PSFixupFlagMap, AIE2PSInstrFixupFlags);
}
} // namespace llvm
