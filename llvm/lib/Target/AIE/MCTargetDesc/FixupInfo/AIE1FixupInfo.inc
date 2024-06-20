//===- AIEFixupInfo.inc -------------------------------------------------===//
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
  fixup_aie_0 = FirstTargetFixupKind, /* => R_AIE_0 (iword) */
  fixup_aie_1, /* => R_AIE_1 (iword) */
  fixup_aie_2, /* => R_AIE_2 (iword) */
  fixup_aie_3, /* => R_AIE_3 (iword) */
  fixup_aie_4, /* => R_AIE_4 (iword) */
  fixup_aie_5, /* => R_AIE_5 (iword) */
  fixup_aie_6, /* => R_AIE_6 (iword) */
  fixup_aie_7, /* => R_AIE_7 (iword) */
  fixup_aie_8, /* => R_AIE_8 (iword) */
  fixup_aie_9, /* => R_AIE_9 (iword) */
  fixup_aie_10, /* => R_AIE_10 (iword) */
  fixup_aie_11, /* => R_AIE_11 (iword) */
  fixup_aie_12, /* => R_AIE_12 (iword) */
  fixup_aie_13, /* => R_AIE_13 (iword) */
  fixup_aie_14, /* => R_AIE_14 (iword) */
  fixup_aie_15, /* => R_AIE_15 (iword) */
  fixup_aie_16, /* => R_AIE_16 (iword) */
  fixup_aie_17, /* => R_AIE_17 (iword) */
  fixup_aie_18, /* => R_AIE_18 (iword) */
  fixup_aie_19, /* => R_AIE_19 (iword) */
  fixup_aie_20, /* => R_AIE_20 (iword) */
  fixup_aie_21, /* => R_AIE_21 (iword) */
  fixup_aie_22, /* => R_AIE_22 (iword) */
  fixup_aie_23, /* => R_AIE_23 (iword) */
  fixup_aie_24, /* => R_AIE_24 (iword) */
  fixup_aie_25, /* => R_AIE_25 (iword) */
  fixup_aie_26, /* => R_AIE_26 (iword) */
  fixup_aie_27, /* => R_AIE_27 (iword) */
  fixup_aie_28, /* => R_AIE_28 (iword) */
  fixup_aie_29, /* => R_AIE_29 (iword) */
  fixup_aie_30, /* => R_AIE_30 (iword) */
  fixup_aie_31, /* => R_AIE_31 (iword) */
  fixup_aie_32, /* => R_AIE_32 (iword) */
  fixup_aie_33, /* => R_AIE_33 (iword) */
  fixup_aie_34, /* => R_AIE_34 (iword) */
  fixup_aie_35, /* => R_AIE_35 (iword) */
  fixup_aie_36, /* => R_AIE_36 (iword) */
  fixup_aie_37, /* => R_AIE_37 (iword) */
  fixup_aie_38, /* => R_AIE_38 (iword) */
  fixup_aie_39, /* => R_AIE_39 (iword) */
  fixup_aie_40, /* => R_AIE_40 (iword) */
  fixup_aie_41, /* => R_AIE_41 (iword) */
  fixup_aie_42, /* => R_AIE_42 (iword) */
  fixup_aie_43, /* => R_AIE_43 (iword) */
  fixup_aie_44, /* => R_AIE_44 (iword) */
  fixup_aie_45, /* => R_AIE_45 (iword) */
  fixup_aie_46, /* => R_AIE_46 (iword) */
  fixup_aie_47, /* => R_AIE_47 (iword) */
  fixup_aie_48, /* => R_AIE_48 (iword) */
  fixup_aie_49, /* => R_AIE_49 (iword) */
  fixup_aie_50, /* => R_AIE_50 (iword) */
  fixup_aie_51, /* => R_AIE_51 (iword) */
  fixup_aie_52, /* => R_AIE_52 (iword) */
  fixup_aie_53, /* => R_AIE_53 (iword) */
  fixup_aie_54, /* => R_AIE_54 (iword) */
  fixup_aie_55, /* => R_AIE_55 (iword) */
  fixup_aie_56, /* => R_AIE_56 (iword) */
  fixup_aie_57, /* => R_AIE_57 (iword) */
  fixup_aie_58, /* => R_AIE_58 (iword) */
  fixup_aie_59, /* => R_AIE_59 (iword) */
  fixup_aie_60, /* => R_AIE_60 (iword) */
  fixup_aie_61, /* => R_AIE_61 (iword) */
  fixup_aie_62, /* => R_AIE_62 (iword) */
  fixup_aie_63, /* => R_AIE_63 (iword) */
  fixup_aie_64, /* => R_AIE_64 (iword) */
  fixup_aie_65, /* => R_AIE_65 (iword) */
  fixup_aie_66, /* => R_AIE_66 (iword) */
  fixup_aie_67, /* => R_AIE_67 (iword) */
  fixup_aie_68, /* => R_AIE_68 (iword) */
  fixup_aie_69, /* => R_AIE_69 (iword) */
  fixup_aie_70, /* => R_AIE_70 (iword) */
  fixup_aie_71, /* => R_AIE_71 (iword) */
  fixup_aie_72, /* => R_AIE_72 (dmword) */
  fixup_aie_73, /* => R_AIE_73 (dmword) */
  fixup_aie_74, /* => R_AIE_74 (dmword) */
  fixup_aie_75, /* => R_AIE_75 (dmword) */

  // fixup_invalid - used as a sentinel and a marker, must be last fixup
  fixup_invalid,
  NumTargetFixupKinds = fixup_invalid - FirstTargetFixupKind
};

#endif // GET_FIXUPS_DECL

#ifdef GET_FIXUPKINDS_DECL
#undef GET_FIXUPKINDS_DECL

std::unique_ptr<AIEMCFixupKinds> createAIE1MCFixupKinds();

#endif // GET_FIXUPKINDS_DECL

#ifdef GET_MCFIXUPKINDS_IMPLEM
#undef GET_MCFIXUPKINDS_IMPLEM

/// Map between each supported Fixup and its corresponding set of fields that
/// it patches up.
/// The offsets are expressed in Big endian.
const std::map<unsigned, SmallVector<FixupField>> AIE1FixupFieldsInfos = {
  { AIE::fixup_aie_0, {{/*Offset=*/64, /*Size=*/3}, {/*Offset=*/71, /*Size=*/17}} },
  { AIE::fixup_aie_1, {{/*Offset=*/4, /*Size=*/3}, {/*Offset=*/11, /*Size=*/17}} },
  { AIE::fixup_aie_2, {{/*Offset=*/28, /*Size=*/3}, {/*Offset=*/35, /*Size=*/17}} },
  { AIE::fixup_aie_3, {{/*Offset=*/25, /*Size=*/3}, {/*Offset=*/32, /*Size=*/17}} },
  { AIE::fixup_aie_4, {{/*Offset=*/64, /*Size=*/3}, {/*Offset=*/71, /*Size=*/17}} },
  { AIE::fixup_aie_5, {{/*Offset=*/25, /*Size=*/3}, {/*Offset=*/32, /*Size=*/17}} },
  { AIE::fixup_aie_6, {{/*Offset=*/8, /*Size=*/10}} },
  { AIE::fixup_aie_7, {{/*Offset=*/9, /*Size=*/10}} },
  { AIE::fixup_aie_8, {{/*Offset=*/8, /*Size=*/10}} },
  { AIE::fixup_aie_9, {{/*Offset=*/9, /*Size=*/10}} },
  { AIE::fixup_aie_10, {{/*Offset=*/8, /*Size=*/10}} },
  { AIE::fixup_aie_11, {{/*Offset=*/57, /*Size=*/12}} },
  { AIE::fixup_aie_12, {{/*Offset=*/2, /*Size=*/12}} },
  { AIE::fixup_aie_13, {{/*Offset=*/19, /*Size=*/12}} },
  { AIE::fixup_aie_14, {{/*Offset=*/37, /*Size=*/12}} },
  { AIE::fixup_aie_15, {{/*Offset=*/2, /*Size=*/12}} },
  { AIE::fixup_aie_16, {{/*Offset=*/21, /*Size=*/12}} },
  { AIE::fixup_aie_17, {{/*Offset=*/57, /*Size=*/12}} },
  { AIE::fixup_aie_18, {{/*Offset=*/19, /*Size=*/12}} },
  { AIE::fixup_aie_19, {{/*Offset=*/20, /*Size=*/12}} },
  { AIE::fixup_aie_20, {{/*Offset=*/21, /*Size=*/12}} },
  { AIE::fixup_aie_21, {{/*Offset=*/37, /*Size=*/12}} },
  { AIE::fixup_aie_22, {{/*Offset=*/37, /*Size=*/12}} },
  { AIE::fixup_aie_23, {{/*Offset=*/83, /*Size=*/12}} },
  { AIE::fixup_aie_24, {{/*Offset=*/56, /*Size=*/12}} },
  { AIE::fixup_aie_25, {{/*Offset=*/20, /*Size=*/8}} },
  { AIE::fixup_aie_26, {{/*Offset=*/3, /*Size=*/8}} },
  { AIE::fixup_aie_27, {{/*Offset=*/39, /*Size=*/8}} },
  { AIE::fixup_aie_28, {{/*Offset=*/22, /*Size=*/8}} },
  { AIE::fixup_aie_29, {{/*Offset=*/21, /*Size=*/8}} },
  { AIE::fixup_aie_30, {{/*Offset=*/26, /*Size=*/9}} },
  { AIE::fixup_aie_31, {{/*Offset=*/9, /*Size=*/9}} },
  { AIE::fixup_aie_32, {{/*Offset=*/26, /*Size=*/9}} },
  { AIE::fixup_aie_33, {{/*Offset=*/9, /*Size=*/9}} },
  { AIE::fixup_aie_34, {{/*Offset=*/26, /*Size=*/9}} },
  { AIE::fixup_aie_35, {{/*Offset=*/60, /*Size=*/8}} },
  { AIE::fixup_aie_36, {{/*Offset=*/37, /*Size=*/8}} },
  { AIE::fixup_aie_37, {{/*Offset=*/62, /*Size=*/6}, {/*Offset=*/74, /*Size=*/14}} },
  { AIE::fixup_aie_38, {{/*Offset=*/23, /*Size=*/6}, {/*Offset=*/35, /*Size=*/14}} },
  { AIE::fixup_aie_39, {{/*Offset=*/62, /*Size=*/6}, {/*Offset=*/74, /*Size=*/14}} },
  { AIE::fixup_aie_40, {{/*Offset=*/23, /*Size=*/6}, {/*Offset=*/35, /*Size=*/14}} },
  { AIE::fixup_aie_41, {{/*Offset=*/3, /*Size=*/8}} },
  { AIE::fixup_aie_42, {{/*Offset=*/20, /*Size=*/8}} },
  { AIE::fixup_aie_43, {{/*Offset=*/24, /*Size=*/8}} },
  { AIE::fixup_aie_44, {{/*Offset=*/41, /*Size=*/8}} },
  { AIE::fixup_aie_45, {{/*Offset=*/21, /*Size=*/8}} },
  { AIE::fixup_aie_46, {{/*Offset=*/20, /*Size=*/8}} },
  { AIE::fixup_aie_47, {{/*Offset=*/37, /*Size=*/8}} },
  { AIE::fixup_aie_48, {{/*Offset=*/76, /*Size=*/8}} },
  { AIE::fixup_aie_49, {{/*Offset=*/60, /*Size=*/8}} },
  { AIE::fixup_aie_50, {{/*Offset=*/2, /*Size=*/6}, {/*Offset=*/14, /*Size=*/14}} },
  { AIE::fixup_aie_51, {{/*Offset=*/26, /*Size=*/6}, {/*Offset=*/38, /*Size=*/14}} },
  { AIE::fixup_aie_52, {{/*Offset=*/23, /*Size=*/6}, {/*Offset=*/35, /*Size=*/14}} },
  { AIE::fixup_aie_53, {{/*Offset=*/62, /*Size=*/6}, {/*Offset=*/74, /*Size=*/14}} },
  { AIE::fixup_aie_54, {{/*Offset=*/2, /*Size=*/6}, {/*Offset=*/14, /*Size=*/14}} },
  { AIE::fixup_aie_55, {{/*Offset=*/26, /*Size=*/6}, {/*Offset=*/38, /*Size=*/14}} },
  { AIE::fixup_aie_56, {{/*Offset=*/23, /*Size=*/6}, {/*Offset=*/35, /*Size=*/14}} },
  { AIE::fixup_aie_57, {{/*Offset=*/62, /*Size=*/6}, {/*Offset=*/74, /*Size=*/14}} },
  { AIE::fixup_aie_58, {{/*Offset=*/63, /*Size=*/4}, {/*Offset=*/71, /*Size=*/4}} },
  { AIE::fixup_aie_59, {{/*Offset=*/8, /*Size=*/4}, {/*Offset=*/16, /*Size=*/4}} },
  { AIE::fixup_aie_60, {{/*Offset=*/25, /*Size=*/4}, {/*Offset=*/33, /*Size=*/4}} },
  { AIE::fixup_aie_61, {{/*Offset=*/43, /*Size=*/4}, {/*Offset=*/51, /*Size=*/4}} },
  { AIE::fixup_aie_62, {{/*Offset=*/8, /*Size=*/4}, {/*Offset=*/16, /*Size=*/4}} },
  { AIE::fixup_aie_63, {{/*Offset=*/27, /*Size=*/4}, {/*Offset=*/35, /*Size=*/4}} },
  { AIE::fixup_aie_64, {{/*Offset=*/63, /*Size=*/4}, {/*Offset=*/71, /*Size=*/4}} },
  { AIE::fixup_aie_65, {{/*Offset=*/25, /*Size=*/4}, {/*Offset=*/33, /*Size=*/4}} },
  { AIE::fixup_aie_66, {{/*Offset=*/26, /*Size=*/4}, {/*Offset=*/34, /*Size=*/4}} },
  { AIE::fixup_aie_67, {{/*Offset=*/27, /*Size=*/4}, {/*Offset=*/35, /*Size=*/4}} },
  { AIE::fixup_aie_68, {{/*Offset=*/43, /*Size=*/4}, {/*Offset=*/51, /*Size=*/4}} },
  { AIE::fixup_aie_69, {{/*Offset=*/43, /*Size=*/4}, {/*Offset=*/51, /*Size=*/4}} },
  { AIE::fixup_aie_70, {{/*Offset=*/89, /*Size=*/4}, {/*Offset=*/97, /*Size=*/4}} },
  { AIE::fixup_aie_71, {{/*Offset=*/62, /*Size=*/4}, {/*Offset=*/70, /*Size=*/4}} }
};

/// This structure maps a set of fields to the corresponding set of Fixups,
/// which are dispatched according to the Format size they target.
/// NOTE: std::map garantees logarithmic complexity search into the data
/// structure.
const std::map<SmallVector<FixupField>, std::set<unsigned>> AIE1FixupFieldsMapper = {
  { {{/*Offset=*/64, /*Size=*/3}, {/*Offset=*/71, /*Size=*/17}}, {AIE::fixup_aie_0 /*(FormatSize=16, Flag=Unrestricted)*/, AIE::fixup_aie_4 /*(FormatSize=12, Flag=Unrestricted)*/} },
  { {{/*Offset=*/4, /*Size=*/3}, {/*Offset=*/11, /*Size=*/17}}, {AIE::fixup_aie_1 /*(FormatSize=4, Flag=Unrestricted)*/} },
  { {{/*Offset=*/28, /*Size=*/3}, {/*Offset=*/35, /*Size=*/17}}, {AIE::fixup_aie_2 /*(FormatSize=8, Flag=Unrestricted)*/} },
  { {{/*Offset=*/25, /*Size=*/3}, {/*Offset=*/32, /*Size=*/17}}, {AIE::fixup_aie_3 /*(FormatSize=8, Flag=Unrestricted)*/, AIE::fixup_aie_5 /*(FormatSize=12, Flag=Unrestricted)*/} },
  { {{/*Offset=*/8, /*Size=*/10}}, {AIE::fixup_aie_6 /*(FormatSize=16, Flag=isDataSigned)*/, AIE::fixup_aie_8 /*(FormatSize=8, Flag=isDataSigned)*/, AIE::fixup_aie_10 /*(FormatSize=12, Flag=isDataSigned)*/} },
  { {{/*Offset=*/9, /*Size=*/10}}, {AIE::fixup_aie_7 /*(FormatSize=4, Flag=isDataSigned)*/, AIE::fixup_aie_9 /*(FormatSize=8, Flag=isDataSigned)*/} },
  { {{/*Offset=*/57, /*Size=*/12}}, {AIE::fixup_aie_11 /*(FormatSize=16, Flag=isDataSigned)*/, AIE::fixup_aie_17 /*(FormatSize=12, Flag=isDataSigned)*/} },
  { {{/*Offset=*/2, /*Size=*/12}}, {AIE::fixup_aie_12 /*(FormatSize=4, Flag=isDataSigned)*/, AIE::fixup_aie_15 /*(FormatSize=8, Flag=isDataSigned)*/} },
  { {{/*Offset=*/19, /*Size=*/12}}, {AIE::fixup_aie_13 /*(FormatSize=8, Flag=isDataSigned)*/, AIE::fixup_aie_18 /*(FormatSize=12, Flag=isDataSigned)*/} },
  { {{/*Offset=*/37, /*Size=*/12}}, {AIE::fixup_aie_14 /*(FormatSize=8, Flag=isDataSigned)*/, AIE::fixup_aie_21 /*(FormatSize=16, Flag=isDataSigned)*/, AIE::fixup_aie_22 /*(FormatSize=12, Flag=isDataSigned)*/} },
  { {{/*Offset=*/21, /*Size=*/12}}, {AIE::fixup_aie_16 /*(FormatSize=8, Flag=isDataSigned)*/, AIE::fixup_aie_20 /*(FormatSize=12, Flag=isDataSigned)*/} },
  { {{/*Offset=*/20, /*Size=*/12}}, {AIE::fixup_aie_19 /*(FormatSize=12, Flag=isDataSigned)*/} },
  { {{/*Offset=*/83, /*Size=*/12}}, {AIE::fixup_aie_23 /*(FormatSize=16, Flag=isDataSigned)*/} },
  { {{/*Offset=*/56, /*Size=*/12}}, {AIE::fixup_aie_24 /*(FormatSize=12, Flag=isDataSigned)*/} },
  { {{/*Offset=*/20, /*Size=*/8}}, {AIE::fixup_aie_25 /*(FormatSize=16, Flag=isDataSigned)*/, AIE::fixup_aie_42 /*(FormatSize=8, Flag=isDataSigned)*/, AIE::fixup_aie_46 /*(FormatSize=12, Flag=isDataSigned)*/} },
  { {{/*Offset=*/3, /*Size=*/8}}, {AIE::fixup_aie_26 /*(FormatSize=8, Flag=isDataSigned)*/, AIE::fixup_aie_41 /*(FormatSize=4, Flag=isDataSigned)*/} },
  { {{/*Offset=*/39, /*Size=*/8}}, {AIE::fixup_aie_27 /*(FormatSize=12, Flag=isDataSigned)*/} },
  { {{/*Offset=*/22, /*Size=*/8}}, {AIE::fixup_aie_28 /*(FormatSize=12, Flag=isDataSigned)*/} },
  { {{/*Offset=*/21, /*Size=*/8}}, {AIE::fixup_aie_29 /*(FormatSize=12, Flag=isDataSigned)*/, AIE::fixup_aie_45 /*(FormatSize=8, Flag=isDataSigned)*/} },
  { {{/*Offset=*/26, /*Size=*/9}}, {AIE::fixup_aie_30 /*(FormatSize=16, Flag=isDataSigned)*/, AIE::fixup_aie_32 /*(FormatSize=8, Flag=isDataSigned)*/, AIE::fixup_aie_34 /*(FormatSize=12, Flag=isDataSigned)*/} },
  { {{/*Offset=*/9, /*Size=*/9}}, {AIE::fixup_aie_31 /*(FormatSize=4, Flag=isDataSigned)*/, AIE::fixup_aie_33 /*(FormatSize=8, Flag=isDataSigned)*/} },
  { {{/*Offset=*/60, /*Size=*/8}}, {AIE::fixup_aie_35 /*(FormatSize=16, Flag=isDataSigned)*/, AIE::fixup_aie_49 /*(FormatSize=12, Flag=isDataSigned)*/} },
  { {{/*Offset=*/37, /*Size=*/8}}, {AIE::fixup_aie_36 /*(FormatSize=12, Flag=isDataSigned)*/, AIE::fixup_aie_47 /*(FormatSize=8, Flag=isDataSigned)*/} },
  { {{/*Offset=*/62, /*Size=*/6}, {/*Offset=*/74, /*Size=*/14}}, {AIE::fixup_aie_37 /*(FormatSize=16, Flag=isDataSigned)*/, AIE::fixup_aie_39 /*(FormatSize=16, Flag=isDataUnsigned)*/, AIE::fixup_aie_53 /*(FormatSize=12, Flag=isDataSigned)*/, AIE::fixup_aie_57 /*(FormatSize=12, Flag=isDataUnsigned)*/} },
  { {{/*Offset=*/23, /*Size=*/6}, {/*Offset=*/35, /*Size=*/14}}, {AIE::fixup_aie_38 /*(FormatSize=12, Flag=isDataSigned)*/, AIE::fixup_aie_40 /*(FormatSize=12, Flag=isDataUnsigned)*/, AIE::fixup_aie_52 /*(FormatSize=8, Flag=isDataSigned)*/, AIE::fixup_aie_56 /*(FormatSize=8, Flag=isDataUnsigned)*/} },
  { {{/*Offset=*/24, /*Size=*/8}}, {AIE::fixup_aie_43 /*(FormatSize=8, Flag=isDataSigned)*/} },
  { {{/*Offset=*/41, /*Size=*/8}}, {AIE::fixup_aie_44 /*(FormatSize=8, Flag=isDataSigned)*/} },
  { {{/*Offset=*/76, /*Size=*/8}}, {AIE::fixup_aie_48 /*(FormatSize=12, Flag=isDataSigned)*/} },
  { {{/*Offset=*/2, /*Size=*/6}, {/*Offset=*/14, /*Size=*/14}}, {AIE::fixup_aie_50 /*(FormatSize=4, Flag=isDataSigned)*/, AIE::fixup_aie_54 /*(FormatSize=4, Flag=isDataUnsigned)*/} },
  { {{/*Offset=*/26, /*Size=*/6}, {/*Offset=*/38, /*Size=*/14}}, {AIE::fixup_aie_51 /*(FormatSize=8, Flag=isDataSigned)*/, AIE::fixup_aie_55 /*(FormatSize=8, Flag=isDataUnsigned)*/} },
  { {{/*Offset=*/63, /*Size=*/4}, {/*Offset=*/71, /*Size=*/4}}, {AIE::fixup_aie_58 /*(FormatSize=16, Flag=isDataSigned)*/, AIE::fixup_aie_64 /*(FormatSize=12, Flag=isDataSigned)*/} },
  { {{/*Offset=*/8, /*Size=*/4}, {/*Offset=*/16, /*Size=*/4}}, {AIE::fixup_aie_59 /*(FormatSize=4, Flag=isDataSigned)*/, AIE::fixup_aie_62 /*(FormatSize=8, Flag=isDataSigned)*/} },
  { {{/*Offset=*/25, /*Size=*/4}, {/*Offset=*/33, /*Size=*/4}}, {AIE::fixup_aie_60 /*(FormatSize=8, Flag=isDataSigned)*/, AIE::fixup_aie_65 /*(FormatSize=12, Flag=isDataSigned)*/} },
  { {{/*Offset=*/43, /*Size=*/4}, {/*Offset=*/51, /*Size=*/4}}, {AIE::fixup_aie_61 /*(FormatSize=8, Flag=isDataSigned)*/, AIE::fixup_aie_68 /*(FormatSize=16, Flag=isDataSigned)*/, AIE::fixup_aie_69 /*(FormatSize=12, Flag=isDataSigned)*/} },
  { {{/*Offset=*/27, /*Size=*/4}, {/*Offset=*/35, /*Size=*/4}}, {AIE::fixup_aie_63 /*(FormatSize=8, Flag=isDataSigned)*/, AIE::fixup_aie_67 /*(FormatSize=12, Flag=isDataSigned)*/} },
  { {{/*Offset=*/26, /*Size=*/4}, {/*Offset=*/34, /*Size=*/4}}, {AIE::fixup_aie_66 /*(FormatSize=12, Flag=isDataSigned)*/} },
  { {{/*Offset=*/89, /*Size=*/4}, {/*Offset=*/97, /*Size=*/4}}, {AIE::fixup_aie_70 /*(FormatSize=16, Flag=isDataSigned)*/} },
  { {{/*Offset=*/62, /*Size=*/4}, {/*Offset=*/70, /*Size=*/4}}, {AIE::fixup_aie_71 /*(FormatSize=12, Flag=isDataSigned)*/} }
};

/// Size of the (instruction) Format targeted by each AIE fixups
const std::map<unsigned, unsigned> AIE1FixupFormatSize = {
  { AIE::fixup_aie_0, 16 },
  { AIE::fixup_aie_1, 4 },
  { AIE::fixup_aie_2, 8 },
  { AIE::fixup_aie_3, 8 },
  { AIE::fixup_aie_4, 12 },
  { AIE::fixup_aie_5, 12 },
  { AIE::fixup_aie_6, 16 },
  { AIE::fixup_aie_7, 4 },
  { AIE::fixup_aie_8, 8 },
  { AIE::fixup_aie_9, 8 },
  { AIE::fixup_aie_10, 12 },
  { AIE::fixup_aie_11, 16 },
  { AIE::fixup_aie_12, 4 },
  { AIE::fixup_aie_13, 8 },
  { AIE::fixup_aie_14, 8 },
  { AIE::fixup_aie_15, 8 },
  { AIE::fixup_aie_16, 8 },
  { AIE::fixup_aie_17, 12 },
  { AIE::fixup_aie_18, 12 },
  { AIE::fixup_aie_19, 12 },
  { AIE::fixup_aie_20, 12 },
  { AIE::fixup_aie_21, 16 },
  { AIE::fixup_aie_22, 12 },
  { AIE::fixup_aie_23, 16 },
  { AIE::fixup_aie_24, 12 },
  { AIE::fixup_aie_25, 16 },
  { AIE::fixup_aie_26, 8 },
  { AIE::fixup_aie_27, 12 },
  { AIE::fixup_aie_28, 12 },
  { AIE::fixup_aie_29, 12 },
  { AIE::fixup_aie_30, 16 },
  { AIE::fixup_aie_31, 4 },
  { AIE::fixup_aie_32, 8 },
  { AIE::fixup_aie_33, 8 },
  { AIE::fixup_aie_34, 12 },
  { AIE::fixup_aie_35, 16 },
  { AIE::fixup_aie_36, 12 },
  { AIE::fixup_aie_37, 16 },
  { AIE::fixup_aie_38, 12 },
  { AIE::fixup_aie_39, 16 },
  { AIE::fixup_aie_40, 12 },
  { AIE::fixup_aie_41, 4 },
  { AIE::fixup_aie_42, 8 },
  { AIE::fixup_aie_43, 8 },
  { AIE::fixup_aie_44, 8 },
  { AIE::fixup_aie_45, 8 },
  { AIE::fixup_aie_46, 12 },
  { AIE::fixup_aie_47, 8 },
  { AIE::fixup_aie_48, 12 },
  { AIE::fixup_aie_49, 12 },
  { AIE::fixup_aie_50, 4 },
  { AIE::fixup_aie_51, 8 },
  { AIE::fixup_aie_52, 8 },
  { AIE::fixup_aie_53, 12 },
  { AIE::fixup_aie_54, 4 },
  { AIE::fixup_aie_55, 8 },
  { AIE::fixup_aie_56, 8 },
  { AIE::fixup_aie_57, 12 },
  { AIE::fixup_aie_58, 16 },
  { AIE::fixup_aie_59, 4 },
  { AIE::fixup_aie_60, 8 },
  { AIE::fixup_aie_61, 8 },
  { AIE::fixup_aie_62, 8 },
  { AIE::fixup_aie_63, 8 },
  { AIE::fixup_aie_64, 12 },
  { AIE::fixup_aie_65, 12 },
  { AIE::fixup_aie_66, 12 },
  { AIE::fixup_aie_67, 12 },
  { AIE::fixup_aie_68, 16 },
  { AIE::fixup_aie_69, 12 },
  { AIE::fixup_aie_70, 16 },
  { AIE::fixup_aie_71, 12 }
};

/// Map between each supported fixup and a flag indicating the signedness
const std::map<unsigned, FixupFlag> AIE1FixupFlagMap = {
  { AIE::fixup_aie_0, FixupFlag::Unrestricted },
  { AIE::fixup_aie_1, FixupFlag::Unrestricted },
  { AIE::fixup_aie_2, FixupFlag::Unrestricted },
  { AIE::fixup_aie_3, FixupFlag::Unrestricted },
  { AIE::fixup_aie_4, FixupFlag::Unrestricted },
  { AIE::fixup_aie_5, FixupFlag::Unrestricted },
  { AIE::fixup_aie_6, FixupFlag::isDataSigned },
  { AIE::fixup_aie_7, FixupFlag::isDataSigned },
  { AIE::fixup_aie_8, FixupFlag::isDataSigned },
  { AIE::fixup_aie_9, FixupFlag::isDataSigned },
  { AIE::fixup_aie_10, FixupFlag::isDataSigned },
  { AIE::fixup_aie_11, FixupFlag::isDataSigned },
  { AIE::fixup_aie_12, FixupFlag::isDataSigned },
  { AIE::fixup_aie_13, FixupFlag::isDataSigned },
  { AIE::fixup_aie_14, FixupFlag::isDataSigned },
  { AIE::fixup_aie_15, FixupFlag::isDataSigned },
  { AIE::fixup_aie_16, FixupFlag::isDataSigned },
  { AIE::fixup_aie_17, FixupFlag::isDataSigned },
  { AIE::fixup_aie_18, FixupFlag::isDataSigned },
  { AIE::fixup_aie_19, FixupFlag::isDataSigned },
  { AIE::fixup_aie_20, FixupFlag::isDataSigned },
  { AIE::fixup_aie_21, FixupFlag::isDataSigned },
  { AIE::fixup_aie_22, FixupFlag::isDataSigned },
  { AIE::fixup_aie_23, FixupFlag::isDataSigned },
  { AIE::fixup_aie_24, FixupFlag::isDataSigned },
  { AIE::fixup_aie_25, FixupFlag::isDataSigned },
  { AIE::fixup_aie_26, FixupFlag::isDataSigned },
  { AIE::fixup_aie_27, FixupFlag::isDataSigned },
  { AIE::fixup_aie_28, FixupFlag::isDataSigned },
  { AIE::fixup_aie_29, FixupFlag::isDataSigned },
  { AIE::fixup_aie_30, FixupFlag::isDataSigned },
  { AIE::fixup_aie_31, FixupFlag::isDataSigned },
  { AIE::fixup_aie_32, FixupFlag::isDataSigned },
  { AIE::fixup_aie_33, FixupFlag::isDataSigned },
  { AIE::fixup_aie_34, FixupFlag::isDataSigned },
  { AIE::fixup_aie_35, FixupFlag::isDataSigned },
  { AIE::fixup_aie_36, FixupFlag::isDataSigned },
  { AIE::fixup_aie_37, FixupFlag::isDataSigned },
  { AIE::fixup_aie_38, FixupFlag::isDataSigned },
  { AIE::fixup_aie_39, FixupFlag::isDataUnsigned },
  { AIE::fixup_aie_40, FixupFlag::isDataUnsigned },
  { AIE::fixup_aie_41, FixupFlag::isDataSigned },
  { AIE::fixup_aie_42, FixupFlag::isDataSigned },
  { AIE::fixup_aie_43, FixupFlag::isDataSigned },
  { AIE::fixup_aie_44, FixupFlag::isDataSigned },
  { AIE::fixup_aie_45, FixupFlag::isDataSigned },
  { AIE::fixup_aie_46, FixupFlag::isDataSigned },
  { AIE::fixup_aie_47, FixupFlag::isDataSigned },
  { AIE::fixup_aie_48, FixupFlag::isDataSigned },
  { AIE::fixup_aie_49, FixupFlag::isDataSigned },
  { AIE::fixup_aie_50, FixupFlag::isDataSigned },
  { AIE::fixup_aie_51, FixupFlag::isDataSigned },
  { AIE::fixup_aie_52, FixupFlag::isDataSigned },
  { AIE::fixup_aie_53, FixupFlag::isDataSigned },
  { AIE::fixup_aie_54, FixupFlag::isDataUnsigned },
  { AIE::fixup_aie_55, FixupFlag::isDataUnsigned },
  { AIE::fixup_aie_56, FixupFlag::isDataUnsigned },
  { AIE::fixup_aie_57, FixupFlag::isDataUnsigned },
  { AIE::fixup_aie_58, FixupFlag::isDataSigned },
  { AIE::fixup_aie_59, FixupFlag::isDataSigned },
  { AIE::fixup_aie_60, FixupFlag::isDataSigned },
  { AIE::fixup_aie_61, FixupFlag::isDataSigned },
  { AIE::fixup_aie_62, FixupFlag::isDataSigned },
  { AIE::fixup_aie_63, FixupFlag::isDataSigned },
  { AIE::fixup_aie_64, FixupFlag::isDataSigned },
  { AIE::fixup_aie_65, FixupFlag::isDataSigned },
  { AIE::fixup_aie_66, FixupFlag::isDataSigned },
  { AIE::fixup_aie_67, FixupFlag::isDataSigned },
  { AIE::fixup_aie_68, FixupFlag::isDataSigned },
  { AIE::fixup_aie_69, FixupFlag::isDataSigned },
  { AIE::fixup_aie_70, FixupFlag::isDataSigned },
  { AIE::fixup_aie_71, FixupFlag::isDataSigned }
};

std::unique_ptr<AIEMCFixupKinds> llvm::createAIE1MCFixupKinds()
{
  return std::make_unique<AIEMCFixupKinds>(AIE1FixupFieldsInfos,
                                           AIE1FixupFieldsMapper,
                                           AIE1FixupFormatSize,
                                           AIE1FixupFlagMap,
                                           AIE1InstrFixupFlags);
}

#endif // GET_MCFIXUPKINDS_IMPLEM
