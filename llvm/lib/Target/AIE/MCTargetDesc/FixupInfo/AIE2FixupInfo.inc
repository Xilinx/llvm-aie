//===- AIE2FixupInfo.inc
//---------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// This file contains the tables which describe our target specific fixups.
// Automatically generated file, do not edit!

#ifdef GET_FIXUPS_DECL
#undef GET_FIXUPS_DECL

// The current support is only addressing the relocations of instruction
// word(s). It means that relocations of data memory word(s) need to be
// manually handled. For them, LLVM is usually emitting its own fixups
// (FK_DATA_XX). The auto-generated ones can be use if necessary as an
// intermediate during the lowering of the fixups into relocs.
// NOTE: - iword means instruction word(s)
//       - dmword means data memory word(s)
enum Fixups {
  // main fixups
  fixup_aie2_0 = FirstTargetFixupKind, /* => R_AIE2_0 (iword) */
  fixup_aie2_1, /* => R_AIE2_1 (iword) */
  fixup_aie2_2, /* => R_AIE2_2 (iword) */
  fixup_aie2_3, /* => R_AIE2_3 (iword) */
  fixup_aie2_4, /* => R_AIE2_4 (iword) */
  fixup_aie2_5, /* => R_AIE2_5 (iword) */
  fixup_aie2_6, /* => R_AIE2_6 (iword) */
  fixup_aie2_7, /* => R_AIE2_7 (iword) */
  fixup_aie2_8, /* => R_AIE2_8 (iword) */
  fixup_aie2_9, /* => R_AIE2_9 (iword) */
  fixup_aie2_10, /* => R_AIE2_10 (iword) */
  fixup_aie2_11, /* => R_AIE2_11 (iword) */
  fixup_aie2_12, /* => R_AIE2_12 (iword) */
  fixup_aie2_13, /* => R_AIE2_13 (iword) */
  fixup_aie2_14, /* => R_AIE2_14 (iword) */
  fixup_aie2_15, /* => R_AIE2_15 (iword) */
  fixup_aie2_16, /* => R_AIE2_16 (iword) */
  fixup_aie2_17, /* => R_AIE2_17 (iword) */
  fixup_aie2_18, /* => R_AIE2_18 (iword) */
  fixup_aie2_19, /* => R_AIE2_19 (iword) */
  fixup_aie2_20, /* => R_AIE2_20 (iword) */
  fixup_aie2_21, /* => R_AIE2_21 (iword) */
  fixup_aie2_22, /* => R_AIE2_22 (iword) */
  fixup_aie2_23, /* => R_AIE2_23 (iword) */
  fixup_aie2_24, /* => R_AIE2_24 (iword) */
  fixup_aie2_25, /* => R_AIE2_25 (iword) */
  fixup_aie2_26, /* => R_AIE2_26 (iword) */
  fixup_aie2_27, /* => R_AIE2_27 (iword) */
  fixup_aie2_28, /* => R_AIE2_28 (iword) */
  fixup_aie2_29, /* => R_AIE2_29 (iword) */
  fixup_aie2_30, /* => R_AIE2_30 (iword) */
  fixup_aie2_31, /* => R_AIE2_31 (iword) */
  fixup_aie2_32, /* => R_AIE2_32 (iword) */
  fixup_aie2_33, /* => R_AIE2_33 (iword) */
  fixup_aie2_34, /* => R_AIE2_34 (iword) */
  fixup_aie2_35, /* => R_AIE2_35 (iword) */
  fixup_aie2_36, /* => R_AIE2_36 (iword) */
  fixup_aie2_37, /* => R_AIE2_37 (iword) */
  fixup_aie2_38, /* => R_AIE2_38 (iword) */
  fixup_aie2_39, /* => R_AIE2_39 (iword) */
  fixup_aie2_40, /* => R_AIE2_40 (iword) */
  fixup_aie2_41, /* => R_AIE2_41 (iword) */
  fixup_aie2_42, /* => R_AIE2_42 (iword) */
  fixup_aie2_43, /* => R_AIE2_43 (iword) */
  fixup_aie2_44, /* => R_AIE2_44 (iword) */
  fixup_aie2_45, /* => R_AIE2_45 (iword) */
  fixup_aie2_46, /* => R_AIE2_46 (iword) */
  fixup_aie2_47, /* => R_AIE2_47 (iword) */
  fixup_aie2_48, /* => R_AIE2_48 (iword) */
  fixup_aie2_49, /* => R_AIE2_49 (iword) */
  fixup_aie2_50, /* => R_AIE2_50 (dmword) */
  fixup_aie2_51, /* => R_AIE2_51 (dmword) */
  fixup_aie2_52, /* => R_AIE2_52 (dmword) */
  fixup_aie2_53, /* => R_AIE2_53 (dmword) */

  // fixup_invalid - used as a sentinel and a marker, must be last fixup
  fixup_invalid,
  NumTargetFixupKinds = fixup_invalid - FirstTargetFixupKind
};

#endif // GET_FIXUPS_DECL

#ifdef GET_FIXUPKINDS_DECL
#undef GET_FIXUPKINDS_DECL

std::unique_ptr<AIEMCFixupKinds> createAIE2MCFixupKinds();

#endif // GET_FIXUPKINDS_DECL

#ifdef GET_MCFIXUPKINDS_IMPLEM
#undef GET_MCFIXUPKINDS_IMPLEM

/// Map between each supported Fixup and its corresponding set of fields that
/// it patches up.
/// The offsets are expressed in Big endian.
const std::map<unsigned, SmallVector<FixupField>> AIE2FixupFieldsInfos = {
  { AIE2::fixup_aie2_0, {{/*Offset=*/63, /*Size=*/20}} },
  { AIE2::fixup_aie2_1, {{/*Offset=*/5, /*Size=*/20}} },
  { AIE2::fixup_aie2_2, {{/*Offset=*/15, /*Size=*/20}} },
  { AIE2::fixup_aie2_3, {{/*Offset=*/31, /*Size=*/20}} },
  { AIE2::fixup_aie2_4, {{/*Offset=*/10, /*Size=*/20}} },
  { AIE2::fixup_aie2_5, {{/*Offset=*/47, /*Size=*/20}} },
  { AIE2::fixup_aie2_6, {{/*Offset=*/26, /*Size=*/20}} },
  { AIE2::fixup_aie2_7, {{/*Offset=*/22, /*Size=*/20}} },
  { AIE2::fixup_aie2_8, {{/*Offset=*/42, /*Size=*/20}} },
  { AIE2::fixup_aie2_9, {{/*Offset=*/16, /*Size=*/11}} },
  { AIE2::fixup_aie2_10, {{/*Offset=*/5, /*Size=*/11}} },
  { AIE2::fixup_aie2_11, {{/*Offset=*/0, /*Size=*/11}} },
  { AIE2::fixup_aie2_12, {{/*Offset=*/0, /*Size=*/11}} },
  { AIE2::fixup_aie2_13, {{/*Offset=*/0, /*Size=*/11}} },
  { AIE2::fixup_aie2_14, {{/*Offset=*/0, /*Size=*/11}} },
  { AIE2::fixup_aie2_15, {{/*Offset=*/0, /*Size=*/11}} },
  { AIE2::fixup_aie2_16, {{/*Offset=*/58, /*Size=*/5}, {/*Offset=*/69, /*Size=*/6}} },
  { AIE2::fixup_aie2_17, {{/*Offset=*/5, /*Size=*/5}, {/*Offset=*/16, /*Size=*/6}} },
  { AIE2::fixup_aie2_18, {{/*Offset=*/21, /*Size=*/5}, {/*Offset=*/32, /*Size=*/6}} },
  { AIE2::fixup_aie2_19, {{/*Offset=*/10, /*Size=*/5}, {/*Offset=*/21, /*Size=*/6}} },
  { AIE2::fixup_aie2_20, {{/*Offset=*/37, /*Size=*/5}, {/*Offset=*/48, /*Size=*/6}} },
  { AIE2::fixup_aie2_21, {{/*Offset=*/53, /*Size=*/5}, {/*Offset=*/64, /*Size=*/6}} },
  { AIE2::fixup_aie2_22, {{/*Offset=*/26, /*Size=*/5}, {/*Offset=*/37, /*Size=*/6}} },
  { AIE2::fixup_aie2_23, {{/*Offset=*/5, /*Size=*/5}, {/*Offset=*/16, /*Size=*/6}} },
  { AIE2::fixup_aie2_24, {{/*Offset=*/42, /*Size=*/5}, {/*Offset=*/53, /*Size=*/6}} },
  { AIE2::fixup_aie2_25, {{/*Offset=*/21, /*Size=*/5}, {/*Offset=*/32, /*Size=*/6}} },
  { AIE2::fixup_aie2_26, {{/*Offset=*/17, /*Size=*/5}, {/*Offset=*/28, /*Size=*/6}} },
  { AIE2::fixup_aie2_27, {{/*Offset=*/37, /*Size=*/5}, {/*Offset=*/48, /*Size=*/6}} },
  { AIE2::fixup_aie2_28, {{/*Offset=*/85, /*Size=*/9}, {/*Offset=*/96, /*Size=*/1}} },
  { AIE2::fixup_aie2_29, {{/*Offset=*/12, /*Size=*/9}, {/*Offset=*/23, /*Size=*/1}} },
  { AIE2::fixup_aie2_30, {{/*Offset=*/28, /*Size=*/9}, {/*Offset=*/39, /*Size=*/1}} },
  { AIE2::fixup_aie2_31, {{/*Offset=*/37, /*Size=*/9}, {/*Offset=*/48, /*Size=*/1}} },
  { AIE2::fixup_aie2_32, {{/*Offset=*/12, /*Size=*/9}, {/*Offset=*/23, /*Size=*/1}} },
  { AIE2::fixup_aie2_33, {{/*Offset=*/53, /*Size=*/9}, {/*Offset=*/64, /*Size=*/1}} },
  { AIE2::fixup_aie2_34, {{/*Offset=*/28, /*Size=*/9}, {/*Offset=*/39, /*Size=*/1}} },
  { AIE2::fixup_aie2_35, {{/*Offset=*/32, /*Size=*/9}, {/*Offset=*/43, /*Size=*/1}} },
  { AIE2::fixup_aie2_36, {{/*Offset=*/69, /*Size=*/9}, {/*Offset=*/80, /*Size=*/1}} },
  { AIE2::fixup_aie2_37, {{/*Offset=*/44, /*Size=*/9}, {/*Offset=*/55, /*Size=*/1}} },
  { AIE2::fixup_aie2_38, {{/*Offset=*/48, /*Size=*/9}, {/*Offset=*/59, /*Size=*/1}} },
  { AIE2::fixup_aie2_39, {{/*Offset=*/65, /*Size=*/9}, {/*Offset=*/76, /*Size=*/1}} },
  { AIE2::fixup_aie2_40, {{/*Offset=*/64, /*Size=*/9}, {/*Offset=*/75, /*Size=*/1}} },
  { AIE2::fixup_aie2_41, {{/*Offset=*/58, /*Size=*/20}, {/*Offset=*/85, /*Size=*/12}} },
  { AIE2::fixup_aie2_42, {{/*Offset=*/0, /*Size=*/20}, {/*Offset=*/27, /*Size=*/12}} },
  { AIE2::fixup_aie2_43, {{/*Offset=*/10, /*Size=*/20}, {/*Offset=*/37, /*Size=*/12}} },
  { AIE2::fixup_aie2_44, {{/*Offset=*/26, /*Size=*/20}, {/*Offset=*/53, /*Size=*/12}} },
  { AIE2::fixup_aie2_45, {{/*Offset=*/5, /*Size=*/20}, {/*Offset=*/32, /*Size=*/12}} },
  { AIE2::fixup_aie2_46, {{/*Offset=*/42, /*Size=*/20}, {/*Offset=*/69, /*Size=*/12}} },
  { AIE2::fixup_aie2_47, {{/*Offset=*/21, /*Size=*/20}, {/*Offset=*/48, /*Size=*/12}} },
  { AIE2::fixup_aie2_48, {{/*Offset=*/17, /*Size=*/20}, {/*Offset=*/44, /*Size=*/12}} },
  { AIE2::fixup_aie2_49, {{/*Offset=*/37, /*Size=*/20}, {/*Offset=*/64, /*Size=*/12}} }
};

/// This structure maps a set of fields to the corresponding set of Fixups,
/// which are dispatched according to the Format size they target.
/// NOTE: std::map garantees logarithmic complexity search into the data
/// structure.
const std::map<SmallVector<FixupField>, std::set<unsigned>> AIE2FixupFieldsMapper = {
  { {{/*Offset=*/63, /*Size=*/20}}, {AIE2::fixup_aie2_0 /*(FormatSize=16, Flag=Unrestricted)*/} },
  { {{/*Offset=*/5, /*Size=*/20}}, {AIE2::fixup_aie2_1 /*(FormatSize=6, Flag=Unrestricted)*/} },
  { {{/*Offset=*/15, /*Size=*/20}}, {AIE2::fixup_aie2_2 /*(FormatSize=8, Flag=Unrestricted)*/} },
  { {{/*Offset=*/31, /*Size=*/20}}, {AIE2::fixup_aie2_3 /*(FormatSize=10, Flag=Unrestricted)*/} },
  { {{/*Offset=*/10, /*Size=*/20}}, {AIE2::fixup_aie2_4 /*(FormatSize=10, Flag=Unrestricted)*/} },
  { {{/*Offset=*/47, /*Size=*/20}}, {AIE2::fixup_aie2_5 /*(FormatSize=12, Flag=Unrestricted)*/} },
  { {{/*Offset=*/26, /*Size=*/20}}, {AIE2::fixup_aie2_6 /*(FormatSize=12, Flag=Unrestricted)*/} },
  { {{/*Offset=*/22, /*Size=*/20}}, {AIE2::fixup_aie2_7 /*(FormatSize=12, Flag=Unrestricted)*/} },
  { {{/*Offset=*/42, /*Size=*/20}}, {AIE2::fixup_aie2_8 /*(FormatSize=14, Flag=Unrestricted)*/} },
  { {{/*Offset=*/16, /*Size=*/11}}, {AIE2::fixup_aie2_9 /*(FormatSize=16, Flag=Unrestricted)*/} },
  { {{/*Offset=*/5, /*Size=*/11}}, {AIE2::fixup_aie2_10 /*(FormatSize=4, Flag=Unrestricted)*/} },
  { {{/*Offset=*/0, /*Size=*/11}}, {AIE2::fixup_aie2_11 /*(FormatSize=6, Flag=Unrestricted)*/, AIE2::fixup_aie2_12 /*(FormatSize=8, Flag=Unrestricted)*/, AIE2::fixup_aie2_13 /*(FormatSize=10, Flag=Unrestricted)*/, AIE2::fixup_aie2_14 /*(FormatSize=12, Flag=Unrestricted)*/, AIE2::fixup_aie2_15 /*(FormatSize=14, Flag=Unrestricted)*/} },
  { {{/*Offset=*/58, /*Size=*/5}, {/*Offset=*/69, /*Size=*/6}}, {AIE2::fixup_aie2_16 /*(FormatSize=16, Flag=Unrestricted)*/} },
  { {{/*Offset=*/5, /*Size=*/5}, {/*Offset=*/16, /*Size=*/6}}, {AIE2::fixup_aie2_17 /*(FormatSize=4, Flag=Unrestricted)*/, AIE2::fixup_aie2_23 /*(FormatSize=10, Flag=Unrestricted)*/} },
  { {{/*Offset=*/21, /*Size=*/5}, {/*Offset=*/32, /*Size=*/6}}, {AIE2::fixup_aie2_18 /*(FormatSize=6, Flag=Unrestricted)*/, AIE2::fixup_aie2_25 /*(FormatSize=12, Flag=Unrestricted)*/} },
  { {{/*Offset=*/10, /*Size=*/5}, {/*Offset=*/21, /*Size=*/6}}, {AIE2::fixup_aie2_19 /*(FormatSize=8, Flag=Unrestricted)*/} },
  { {{/*Offset=*/37, /*Size=*/5}, {/*Offset=*/48, /*Size=*/6}}, {AIE2::fixup_aie2_20 /*(FormatSize=8, Flag=Unrestricted)*/, AIE2::fixup_aie2_27 /*(FormatSize=14, Flag=Unrestricted)*/} },
  { {{/*Offset=*/53, /*Size=*/5}, {/*Offset=*/64, /*Size=*/6}}, {AIE2::fixup_aie2_21 /*(FormatSize=10, Flag=Unrestricted)*/} },
  { {{/*Offset=*/26, /*Size=*/5}, {/*Offset=*/37, /*Size=*/6}}, {AIE2::fixup_aie2_22 /*(FormatSize=10, Flag=Unrestricted)*/} },
  { {{/*Offset=*/42, /*Size=*/5}, {/*Offset=*/53, /*Size=*/6}}, {AIE2::fixup_aie2_24 /*(FormatSize=12, Flag=Unrestricted)*/} },
  { {{/*Offset=*/17, /*Size=*/5}, {/*Offset=*/28, /*Size=*/6}}, {AIE2::fixup_aie2_26 /*(FormatSize=12, Flag=Unrestricted)*/} },
  { {{/*Offset=*/85, /*Size=*/9}, {/*Offset=*/96, /*Size=*/1}}, {AIE2::fixup_aie2_28 /*(FormatSize=16, Flag=Unrestricted)*/} },
  { {{/*Offset=*/12, /*Size=*/9}, {/*Offset=*/23, /*Size=*/1}}, {AIE2::fixup_aie2_29 /*(FormatSize=4, Flag=Unrestricted)*/, AIE2::fixup_aie2_32 /*(FormatSize=8, Flag=Unrestricted)*/} },
  { {{/*Offset=*/28, /*Size=*/9}, {/*Offset=*/39, /*Size=*/1}}, {AIE2::fixup_aie2_30 /*(FormatSize=6, Flag=Unrestricted)*/, AIE2::fixup_aie2_34 /*(FormatSize=10, Flag=Unrestricted)*/} },
  { {{/*Offset=*/37, /*Size=*/9}, {/*Offset=*/48, /*Size=*/1}}, {AIE2::fixup_aie2_31 /*(FormatSize=8, Flag=Unrestricted)*/} },
  { {{/*Offset=*/53, /*Size=*/9}, {/*Offset=*/64, /*Size=*/1}}, {AIE2::fixup_aie2_33 /*(FormatSize=10, Flag=Unrestricted)*/} },
  { {{/*Offset=*/32, /*Size=*/9}, {/*Offset=*/43, /*Size=*/1}}, {AIE2::fixup_aie2_35 /*(FormatSize=10, Flag=Unrestricted)*/} },
  { {{/*Offset=*/69, /*Size=*/9}, {/*Offset=*/80, /*Size=*/1}}, {AIE2::fixup_aie2_36 /*(FormatSize=12, Flag=Unrestricted)*/} },
  { {{/*Offset=*/44, /*Size=*/9}, {/*Offset=*/55, /*Size=*/1}}, {AIE2::fixup_aie2_37 /*(FormatSize=12, Flag=Unrestricted)*/} },
  { {{/*Offset=*/48, /*Size=*/9}, {/*Offset=*/59, /*Size=*/1}}, {AIE2::fixup_aie2_38 /*(FormatSize=12, Flag=Unrestricted)*/} },
  { {{/*Offset=*/65, /*Size=*/9}, {/*Offset=*/76, /*Size=*/1}}, {AIE2::fixup_aie2_39 /*(FormatSize=14, Flag=Unrestricted)*/} },
  { {{/*Offset=*/64, /*Size=*/9}, {/*Offset=*/75, /*Size=*/1}}, {AIE2::fixup_aie2_40 /*(FormatSize=14, Flag=Unrestricted)*/} },
  { {{/*Offset=*/58, /*Size=*/20}, {/*Offset=*/85, /*Size=*/12}}, {AIE2::fixup_aie2_41 /*(FormatSize=16, Flag=Unrestricted)*/} },
  { {{/*Offset=*/0, /*Size=*/20}, {/*Offset=*/27, /*Size=*/12}}, {AIE2::fixup_aie2_42 /*(FormatSize=6, Flag=Unrestricted)*/} },
  { {{/*Offset=*/10, /*Size=*/20}, {/*Offset=*/37, /*Size=*/12}}, {AIE2::fixup_aie2_43 /*(FormatSize=8, Flag=Unrestricted)*/} },
  { {{/*Offset=*/26, /*Size=*/20}, {/*Offset=*/53, /*Size=*/12}}, {AIE2::fixup_aie2_44 /*(FormatSize=10, Flag=Unrestricted)*/} },
  { {{/*Offset=*/5, /*Size=*/20}, {/*Offset=*/32, /*Size=*/12}}, {AIE2::fixup_aie2_45 /*(FormatSize=10, Flag=Unrestricted)*/} },
  { {{/*Offset=*/42, /*Size=*/20}, {/*Offset=*/69, /*Size=*/12}}, {AIE2::fixup_aie2_46 /*(FormatSize=12, Flag=Unrestricted)*/} },
  { {{/*Offset=*/21, /*Size=*/20}, {/*Offset=*/48, /*Size=*/12}}, {AIE2::fixup_aie2_47 /*(FormatSize=12, Flag=Unrestricted)*/} },
  { {{/*Offset=*/17, /*Size=*/20}, {/*Offset=*/44, /*Size=*/12}}, {AIE2::fixup_aie2_48 /*(FormatSize=12, Flag=Unrestricted)*/} },
  { {{/*Offset=*/37, /*Size=*/20}, {/*Offset=*/64, /*Size=*/12}}, {AIE2::fixup_aie2_49 /*(FormatSize=14, Flag=Unrestricted)*/} }
};

/// Size of the (instruction) Format targeted by each AIE2 fixups
const std::map<unsigned, unsigned> AIE2FixupFormatSize = {
  { AIE2::fixup_aie2_0, 16 },
  { AIE2::fixup_aie2_1, 6 },
  { AIE2::fixup_aie2_2, 8 },
  { AIE2::fixup_aie2_3, 10 },
  { AIE2::fixup_aie2_4, 10 },
  { AIE2::fixup_aie2_5, 12 },
  { AIE2::fixup_aie2_6, 12 },
  { AIE2::fixup_aie2_7, 12 },
  { AIE2::fixup_aie2_8, 14 },
  { AIE2::fixup_aie2_9, 16 },
  { AIE2::fixup_aie2_10, 4 },
  { AIE2::fixup_aie2_11, 6 },
  { AIE2::fixup_aie2_12, 8 },
  { AIE2::fixup_aie2_13, 10 },
  { AIE2::fixup_aie2_14, 12 },
  { AIE2::fixup_aie2_15, 14 },
  { AIE2::fixup_aie2_16, 16 },
  { AIE2::fixup_aie2_17, 4 },
  { AIE2::fixup_aie2_18, 6 },
  { AIE2::fixup_aie2_19, 8 },
  { AIE2::fixup_aie2_20, 8 },
  { AIE2::fixup_aie2_21, 10 },
  { AIE2::fixup_aie2_22, 10 },
  { AIE2::fixup_aie2_23, 10 },
  { AIE2::fixup_aie2_24, 12 },
  { AIE2::fixup_aie2_25, 12 },
  { AIE2::fixup_aie2_26, 12 },
  { AIE2::fixup_aie2_27, 14 },
  { AIE2::fixup_aie2_28, 16 },
  { AIE2::fixup_aie2_29, 4 },
  { AIE2::fixup_aie2_30, 6 },
  { AIE2::fixup_aie2_31, 8 },
  { AIE2::fixup_aie2_32, 8 },
  { AIE2::fixup_aie2_33, 10 },
  { AIE2::fixup_aie2_34, 10 },
  { AIE2::fixup_aie2_35, 10 },
  { AIE2::fixup_aie2_36, 12 },
  { AIE2::fixup_aie2_37, 12 },
  { AIE2::fixup_aie2_38, 12 },
  { AIE2::fixup_aie2_39, 14 },
  { AIE2::fixup_aie2_40, 14 },
  { AIE2::fixup_aie2_41, 16 },
  { AIE2::fixup_aie2_42, 6 },
  { AIE2::fixup_aie2_43, 8 },
  { AIE2::fixup_aie2_44, 10 },
  { AIE2::fixup_aie2_45, 10 },
  { AIE2::fixup_aie2_46, 12 },
  { AIE2::fixup_aie2_47, 12 },
  { AIE2::fixup_aie2_48, 12 },
  { AIE2::fixup_aie2_49, 14 }
};

/// Map between each supported fixup and a flag indicating the signedness
const std::map<unsigned, FixupFlag> AIE2FixupFlagMap = {
  { AIE2::fixup_aie2_0, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_1, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_2, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_3, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_4, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_5, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_6, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_7, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_8, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_9, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_10, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_11, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_12, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_13, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_14, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_15, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_16, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_17, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_18, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_19, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_20, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_21, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_22, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_23, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_24, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_25, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_26, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_27, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_28, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_29, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_30, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_31, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_32, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_33, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_34, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_35, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_36, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_37, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_38, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_39, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_40, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_41, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_42, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_43, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_44, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_45, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_46, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_47, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_48, FixupFlag::Unrestricted },
  { AIE2::fixup_aie2_49, FixupFlag::Unrestricted }
};

std::unique_ptr<AIEMCFixupKinds> llvm::createAIE2MCFixupKinds()
{
  return std::make_unique<AIEMCFixupKinds>(
    AIE2FixupFieldsInfos,
    AIE2FixupFieldsMapper,
    AIE2FixupFormatSize,
    AIE2FixupFlagMap,
    AIE2InstrFixupFlags
  );
}

#endif // GET_MCFIXUPKINDS_IMPLEM

#ifdef RELOCS_DEFINITIONS
#undef RELOCS_DEFINITIONS

// The following declarations must be copied in a separate file at
// llvm/include/llvm/BinaryFormat/ELFRelocs/AIE2.def
#ifndef ELF_RELOC
#error "ELF_RELOC must be defined"
#endif

ELF_RELOC(R_AIE2_0,  0)
ELF_RELOC(R_AIE2_1,  1)
ELF_RELOC(R_AIE2_2,  2)
ELF_RELOC(R_AIE2_3,  3)
ELF_RELOC(R_AIE2_4,  4)
ELF_RELOC(R_AIE2_5,  5)
ELF_RELOC(R_AIE2_6,  6)
ELF_RELOC(R_AIE2_7,  7)
ELF_RELOC(R_AIE2_8,  8)
ELF_RELOC(R_AIE2_9,  9)
ELF_RELOC(R_AIE2_10,  10)
ELF_RELOC(R_AIE2_11,  11)
ELF_RELOC(R_AIE2_12,  12)
ELF_RELOC(R_AIE2_13,  13)
ELF_RELOC(R_AIE2_14,  14)
ELF_RELOC(R_AIE2_15,  15)
ELF_RELOC(R_AIE2_16,  16)
ELF_RELOC(R_AIE2_17,  17)
ELF_RELOC(R_AIE2_18,  18)
ELF_RELOC(R_AIE2_19,  19)
ELF_RELOC(R_AIE2_20,  20)
ELF_RELOC(R_AIE2_21,  21)
ELF_RELOC(R_AIE2_22,  22)
ELF_RELOC(R_AIE2_23,  23)
ELF_RELOC(R_AIE2_24,  24)
ELF_RELOC(R_AIE2_25,  25)
ELF_RELOC(R_AIE2_26,  26)
ELF_RELOC(R_AIE2_27,  27)
ELF_RELOC(R_AIE2_28,  28)
ELF_RELOC(R_AIE2_29,  29)
ELF_RELOC(R_AIE2_30,  30)
ELF_RELOC(R_AIE2_31,  31)
ELF_RELOC(R_AIE2_32,  32)
ELF_RELOC(R_AIE2_33,  33)
ELF_RELOC(R_AIE2_34,  34)
ELF_RELOC(R_AIE2_35,  35)
ELF_RELOC(R_AIE2_36,  36)
ELF_RELOC(R_AIE2_37,  37)
ELF_RELOC(R_AIE2_38,  38)
ELF_RELOC(R_AIE2_39,  39)
ELF_RELOC(R_AIE2_40,  40)
ELF_RELOC(R_AIE2_41,  41)
ELF_RELOC(R_AIE2_42,  42)
ELF_RELOC(R_AIE2_43,  43)
ELF_RELOC(R_AIE2_44,  44)
ELF_RELOC(R_AIE2_45,  45)
ELF_RELOC(R_AIE2_46,  46)
ELF_RELOC(R_AIE2_47,  47)
ELF_RELOC(R_AIE2_48,  48)
ELF_RELOC(R_AIE2_49,  49)
ELF_RELOC(R_AIE2_50,  50)
ELF_RELOC(R_AIE2_51,  51)
ELF_RELOC(R_AIE2_52,  52)
ELF_RELOC(R_AIE2_53,  53)

#endif // RELOCS_DEFINITIONS
