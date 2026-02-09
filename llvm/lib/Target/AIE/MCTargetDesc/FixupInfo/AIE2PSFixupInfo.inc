//===- output.inc ---------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===-----------------------------------------------------------------------===//

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
  fixup_aie2ps_0 = FirstTargetFixupKind, /* => R_AIE2PS_0 (iword) */
  fixup_aie2ps_1,                        /* => R_AIE2PS_1 (iword) */
  fixup_aie2ps_2,                        /* => R_AIE2PS_2 (iword) */
  fixup_aie2ps_3,                        /* => R_AIE2PS_3 (iword) */
  fixup_aie2ps_4,                        /* => R_AIE2PS_4 (iword) */
  fixup_aie2ps_5,                        /* => R_AIE2PS_5 (iword) */
  fixup_aie2ps_6,                        /* => R_AIE2PS_6 (iword) */
  fixup_aie2ps_7,                        /* => R_AIE2PS_7 (iword) */
  fixup_aie2ps_8,                        /* => R_AIE2PS_8 (iword) */
  fixup_aie2ps_9,                        /* => R_AIE2PS_9 (iword) */
  fixup_aie2ps_10,                       /* => R_AIE2PS_10 (iword) */
  fixup_aie2ps_11,                       /* => R_AIE2PS_11 (iword) */
  fixup_aie2ps_12,                       /* => R_AIE2PS_12 (iword) */
  fixup_aie2ps_13,                       /* => R_AIE2PS_13 (iword) */
  fixup_aie2ps_14,                       /* => R_AIE2PS_14 (iword) */
  fixup_aie2ps_15,                       /* => R_AIE2PS_15 (iword) */
  fixup_aie2ps_16,                       /* => R_AIE2PS_16 (iword) */
  fixup_aie2ps_17,                       /* => R_AIE2PS_17 (iword) */
  fixup_aie2ps_18,                       /* => R_AIE2PS_18 (iword) */
  fixup_aie2ps_19,                       /* => R_AIE2PS_19 (iword) */
  fixup_aie2ps_20,                       /* => R_AIE2PS_20 (iword) */
  fixup_aie2ps_21,                       /* => R_AIE2PS_21 (iword) */
  fixup_aie2ps_22,                       /* => R_AIE2PS_22 (iword) */
  fixup_aie2ps_23,                       /* => R_AIE2PS_23 (iword) */
  fixup_aie2ps_24,                       /* => R_AIE2PS_24 (iword) */
  fixup_aie2ps_25,                       /* => R_AIE2PS_25 (iword) */
  fixup_aie2ps_26,                       /* => R_AIE2PS_26 (iword) */
  fixup_aie2ps_27,                       /* => R_AIE2PS_27 (iword) */
  fixup_aie2ps_28,                       /* => R_AIE2PS_28 (iword) */
  fixup_aie2ps_29,                       /* => R_AIE2PS_29 (iword) */
  fixup_aie2ps_30,                       /* => R_AIE2PS_30 (iword) */
  fixup_aie2ps_31,                       /* => R_AIE2PS_31 (iword) */
  fixup_aie2ps_32,                       /* => R_AIE2PS_32 (iword) */
  fixup_aie2ps_33,                       /* => R_AIE2PS_33 (iword) */
  fixup_aie2ps_34,                       /* => R_AIE2PS_34 (iword) */
  fixup_aie2ps_35,                       /* => R_AIE2PS_35 (iword) */
  fixup_aie2ps_36,                       /* => R_AIE2PS_36 (iword) */
  fixup_aie2ps_37,                       /* => R_AIE2PS_37 (iword) */
  fixup_aie2ps_38,                       /* => R_AIE2PS_38 (iword) */
  fixup_aie2ps_39,                       /* => R_AIE2PS_39 (iword) */
  fixup_aie2ps_40,                       /* => R_AIE2PS_40 (iword) */
  fixup_aie2ps_41,                       /* => R_AIE2PS_41 (iword) */
  fixup_aie2ps_42,                       /* => R_AIE2PS_42 (iword) */
  fixup_aie2ps_43,                       /* => R_AIE2PS_43 (iword) */
  fixup_aie2ps_44,                       /* => R_AIE2PS_44 (iword) */
  fixup_aie2ps_45,                       /* => R_AIE2PS_45 (iword) */
  fixup_aie2ps_46,                       /* => R_AIE2PS_46 (iword) */
  fixup_aie2ps_47,                       /* => R_AIE2PS_47 (iword) */
  fixup_aie2ps_48,                       /* => R_AIE2PS_48 (iword) */
  fixup_aie2ps_49,                       /* => R_AIE2PS_49 (iword) */
  fixup_aie2ps_50,                       /* => R_AIE2PS_50 (iword) */
  fixup_aie2ps_51,                       /* => R_AIE2PS_51 (iword) */
  fixup_aie2ps_52,                       /* => R_AIE2PS_52 (iword) */
  fixup_aie2ps_53,                       /* => R_AIE2PS_53 (iword) */
  fixup_aie2ps_54,                       /* => R_AIE2PS_54 (iword) */
  fixup_aie2ps_55,                       /* => R_AIE2PS_55 (iword) */
  fixup_aie2ps_56,                       /* => R_AIE2PS_56 (iword) */
  fixup_aie2ps_57,                       /* => R_AIE2PS_57 (iword) */
  fixup_aie2ps_58,                       /* => R_AIE2PS_58 (iword) */
  fixup_aie2ps_59,                       /* => R_AIE2PS_59 (iword) */
  fixup_aie2ps_60,                       /* => R_AIE2PS_60 (iword) */
  fixup_aie2ps_61,                       /* => R_AIE2PS_61 (iword) */
  fixup_aie2ps_62,                       /* => R_AIE2PS_62 (iword) */
  fixup_aie2ps_63,                       /* => R_AIE2PS_63 (iword) */
  fixup_aie2ps_64,                       /* => R_AIE2PS_64 (iword) */
  fixup_aie2ps_65,                       /* => R_AIE2PS_65 (iword) */
  fixup_aie2ps_66,                       /* => R_AIE2PS_66 (iword) */
  fixup_aie2ps_67,                       /* => R_AIE2PS_67 (iword) */
  fixup_aie2ps_68,                       /* => R_AIE2PS_68 (iword) */
  fixup_aie2ps_69,                       /* => R_AIE2PS_69 (iword) */
  fixup_aie2ps_70,                       /* => R_AIE2PS_70 (iword) */
  fixup_aie2ps_71,                       /* => R_AIE2PS_71 (iword) */
  fixup_aie2ps_72,                       /* => R_AIE2PS_72 (iword) */
  fixup_aie2ps_73,                       /* => R_AIE2PS_73 (iword) */
  fixup_aie2ps_74,                       /* => R_AIE2PS_74 (iword) */
  fixup_aie2ps_75,                       /* => R_AIE2PS_75 (iword) */
  fixup_aie2ps_76,                       /* => R_AIE2PS_76 (iword) */
  fixup_aie2ps_77,                       /* => R_AIE2PS_77 (iword) */
  fixup_aie2ps_78,                       /* => R_AIE2PS_78 (iword) */
  fixup_aie2ps_79,                       /* => R_AIE2PS_79 (iword) */
  fixup_aie2ps_80,                       /* => R_AIE2PS_80 (iword) */
  fixup_aie2ps_81,                       /* => R_AIE2PS_81 (iword) */
  fixup_aie2ps_82,                       /* => R_AIE2PS_82 (iword) */
  fixup_aie2ps_83,                       /* => R_AIE2PS_83 (iword) */
  fixup_aie2ps_84,                       /* => R_AIE2PS_84 (iword) */
  fixup_aie2ps_85,                       /* => R_AIE2PS_85 (iword) */
  fixup_aie2ps_86,                       /* => R_AIE2PS_86 (iword) */
  fixup_aie2ps_87,                       /* => R_AIE2PS_87 (iword) */
  fixup_aie2ps_88,                       /* => R_AIE2PS_88 (iword) */
  fixup_aie2ps_89,                       /* => R_AIE2PS_89 (iword) */
  fixup_aie2ps_90,                       /* => R_AIE2PS_90 (iword) */
  fixup_aie2ps_91,                       /* => R_AIE2PS_91 (iword) */
  fixup_aie2ps_92,                       /* => R_AIE2PS_92 (iword) */
  fixup_aie2ps_93,                       /* => R_AIE2PS_93 (iword) */
  fixup_aie2ps_94,                       /* => R_AIE2PS_94 (iword) */
  fixup_aie2ps_95,                       /* => R_AIE2PS_95 (iword) */
  fixup_aie2ps_96,                       /* => R_AIE2PS_96 (iword) */
  fixup_aie2ps_97,                       /* => R_AIE2PS_97 (iword) */
  fixup_aie2ps_98,                       /* => R_AIE2PS_98 (iword) */
  fixup_aie2ps_99,                       /* => R_AIE2PS_99 (iword) */
  fixup_aie2ps_100,                      /* => R_AIE2PS_100 (iword) */
  fixup_aie2ps_101,                      /* => R_AIE2PS_101 (iword) */
  fixup_aie2ps_102,                      /* => R_AIE2PS_102 (iword) */
  fixup_aie2ps_103,                      /* => R_AIE2PS_103 (iword) */
  fixup_aie2ps_104,                      /* => R_AIE2PS_104 (iword) */
  fixup_aie2ps_105,                      /* => R_AIE2PS_105 (iword) */
  fixup_aie2ps_106,                      /* => R_AIE2PS_106 (iword) */
  fixup_aie2ps_107,                      /* => R_AIE2PS_107 (iword) */
  fixup_aie2ps_108,                      /* => R_AIE2PS_108 (iword) */
  fixup_aie2ps_109,                      /* => R_AIE2PS_109 (iword) */
  fixup_aie2ps_110,                      /* => R_AIE2PS_110 (iword) */
  fixup_aie2ps_111,                      /* => R_AIE2PS_111 (iword) */
  fixup_aie2ps_112,                      /* => R_AIE2PS_112 (iword) */
  fixup_aie2ps_113,                      /* => R_AIE2PS_113 (iword) */
  fixup_aie2ps_114,                      /* => R_AIE2PS_114 (iword) */
  fixup_aie2ps_115,                      /* => R_AIE2PS_115 (iword) */
  fixup_aie2ps_116,                      /* => R_AIE2PS_116 (iword) */
  fixup_aie2ps_117,                      /* => R_AIE2PS_117 (iword) */
  fixup_aie2ps_118,                      /* => R_AIE2PS_118 (iword) */
  fixup_aie2ps_119,                      /* => R_AIE2PS_119 (iword) */
  fixup_aie2ps_120,                      /* => R_AIE2PS_120 (iword) */
  fixup_aie2ps_121,                      /* => R_AIE2PS_121 (iword) */
  fixup_aie2ps_122,                      /* => R_AIE2PS_122 (iword) */
  fixup_aie2ps_123,                      /* => R_AIE2PS_123 (iword) */
  fixup_aie2ps_124,                      /* => R_AIE2PS_124 (iword) */
  fixup_aie2ps_125,                      /* => R_AIE2PS_125 (iword) */
  fixup_aie2ps_126,                      /* => R_AIE2PS_126 (iword) */
  fixup_aie2ps_127,                      /* => R_AIE2PS_127 (iword) */
  fixup_aie2ps_128,                      /* => R_AIE2PS_128 (iword) */
  fixup_aie2ps_129,                      /* => R_AIE2PS_129 (iword) */
  fixup_aie2ps_130,                      /* => R_AIE2PS_130 (iword) */
  fixup_aie2ps_131,                      /* => R_AIE2PS_131 (iword) */
  fixup_aie2ps_132,                      /* => R_AIE2PS_132 (iword) */
  fixup_aie2ps_133,                      /* => R_AIE2PS_133 (iword) */
  fixup_aie2ps_134,                      /* => R_AIE2PS_134 (iword) */
  fixup_aie2ps_135,                      /* => R_AIE2PS_135 (dmword) */
  fixup_aie2ps_136,                      /* => R_AIE2PS_136 (dmword) */
  fixup_aie2ps_137,                      /* => R_AIE2PS_137 (dmword) */
  fixup_aie2ps_138,                      /* => R_AIE2PS_138 (dmword) */

  // fixup_invalid - used as a sentinel and a marker, must be last fixup
  fixup_invalid,
  NumTargetFixupKinds = fixup_invalid - FirstTargetFixupKind
};

#endif // GET_FIXUPS_DECL

#ifdef GET_FIXUPKINDS_DECL
#undef GET_FIXUPKINDS_DECL

std::unique_ptr<AIEMCFixupKinds> createAIE2PSMCFixupKinds();

#endif // GET_FIXUPKINDS_DECL

#ifdef GET_MCFIXUPKINDS_IMPLEM
#undef GET_MCFIXUPKINDS_IMPLEM

/// Map between each supported Fixup and its corresponding set of fields that
/// it patches up.
/// The offsets are expressed in Big endian.
const std::map<unsigned, SmallVector<FixupField>> AIE2PSFixupFieldsInfos = {
    {AIE2PS::fixup_aie2ps_0, {{/*Offset=*/64, /*Size=*/20}}},
    {AIE2PS::fixup_aie2ps_1, {{/*Offset=*/5, /*Size=*/20}}},
    {AIE2PS::fixup_aie2ps_2, {{/*Offset=*/31, /*Size=*/20}}},
    {AIE2PS::fixup_aie2ps_3, {{/*Offset=*/6, /*Size=*/20}}},
    {AIE2PS::fixup_aie2ps_4, {{/*Offset=*/48, /*Size=*/20}}},
    {AIE2PS::fixup_aie2ps_5, {{/*Offset=*/26, /*Size=*/20}}},
    {AIE2PS::fixup_aie2ps_6, {{/*Offset=*/22, /*Size=*/20}}},
    {AIE2PS::fixup_aie2ps_7, {{/*Offset=*/43, /*Size=*/20}}},
    {AIE2PS::fixup_aie2ps_8,
     {{/*Offset=*/0, /*Size=*/9}, {/*Offset=*/16, /*Size=*/2}}},
    {AIE2PS::fixup_aie2ps_9,
     {{/*Offset=*/4, /*Size=*/9}, {/*Offset=*/20, /*Size=*/2}}},
    {AIE2PS::fixup_aie2ps_10,
     {{/*Offset=*/0, /*Size=*/9}, {/*Offset=*/16, /*Size=*/2}}},
    {AIE2PS::fixup_aie2ps_11,
     {{/*Offset=*/0, /*Size=*/9}, {/*Offset=*/16, /*Size=*/2}}},
    {AIE2PS::fixup_aie2ps_12,
     {{/*Offset=*/0, /*Size=*/9}, {/*Offset=*/16, /*Size=*/2}}},
    {AIE2PS::fixup_aie2ps_13,
     {{/*Offset=*/0, /*Size=*/9}, {/*Offset=*/16, /*Size=*/2}}},
    {AIE2PS::fixup_aie2ps_14,
     {{/*Offset=*/0, /*Size=*/9}, {/*Offset=*/16, /*Size=*/2}}},
    {AIE2PS::fixup_aie2ps_15,
     {{/*Offset=*/59, /*Size=*/5}, {/*Offset=*/70, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_16,
     {{/*Offset=*/5, /*Size=*/5}, {/*Offset=*/16, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_17,
     {{/*Offset=*/21, /*Size=*/5}, {/*Offset=*/32, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_18,
     {{/*Offset=*/0, /*Size=*/5}, {/*Offset=*/11, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_19,
     {{/*Offset=*/10, /*Size=*/5}, {/*Offset=*/21, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_20,
     {{/*Offset=*/38, /*Size=*/5}, {/*Offset=*/49, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_21,
     {{/*Offset=*/53, /*Size=*/5}, {/*Offset=*/64, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_22,
     {{/*Offset=*/26, /*Size=*/5}, {/*Offset=*/37, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_23,
     {{/*Offset=*/1, /*Size=*/5}, {/*Offset=*/12, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_24,
     {{/*Offset=*/43, /*Size=*/5}, {/*Offset=*/54, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_25,
     {{/*Offset=*/21, /*Size=*/5}, {/*Offset=*/32, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_26,
     {{/*Offset=*/17, /*Size=*/5}, {/*Offset=*/28, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_27,
     {{/*Offset=*/38, /*Size=*/5}, {/*Offset=*/49, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_28,
     {{/*Offset=*/93, /*Size=*/4}, {/*Offset=*/86, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_29,
     {{/*Offset=*/18, /*Size=*/4}, {/*Offset=*/11, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_30,
     {{/*Offset=*/35, /*Size=*/4}, {/*Offset=*/28, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_31,
     {{/*Offset=*/34, /*Size=*/4}, {/*Offset=*/27, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_32,
     {{/*Offset=*/45, /*Size=*/4}, {/*Offset=*/38, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_33,
     {{/*Offset=*/19, /*Size=*/4}, {/*Offset=*/12, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_34,
     {{/*Offset=*/60, /*Size=*/4}, {/*Offset=*/53, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_35,
     {{/*Offset=*/35, /*Size=*/4}, {/*Offset=*/28, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_36,
     {{/*Offset=*/77, /*Size=*/4}, {/*Offset=*/70, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_37,
     {{/*Offset=*/52, /*Size=*/4}, {/*Offset=*/45, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_38,
     {{/*Offset=*/55, /*Size=*/4}, {/*Offset=*/48, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_39,
     {{/*Offset=*/51, /*Size=*/4}, {/*Offset=*/44, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_40,
     {{/*Offset=*/72, /*Size=*/4}, {/*Offset=*/65, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_41,
     {{/*Offset=*/59, /*Size=*/20}, {/*Offset=*/86, /*Size=*/12}}},
    {AIE2PS::fixup_aie2ps_42,
     {{/*Offset=*/0, /*Size=*/20}, {/*Offset=*/27, /*Size=*/12}}},
    {AIE2PS::fixup_aie2ps_43,
     {{/*Offset=*/26, /*Size=*/20}, {/*Offset=*/53, /*Size=*/12}}},
    {AIE2PS::fixup_aie2ps_44,
     {{/*Offset=*/1, /*Size=*/20}, {/*Offset=*/28, /*Size=*/12}}},
    {AIE2PS::fixup_aie2ps_45,
     {{/*Offset=*/43, /*Size=*/20}, {/*Offset=*/70, /*Size=*/12}}},
    {AIE2PS::fixup_aie2ps_46,
     {{/*Offset=*/21, /*Size=*/20}, {/*Offset=*/48, /*Size=*/12}}},
    {AIE2PS::fixup_aie2ps_47,
     {{/*Offset=*/17, /*Size=*/20}, {/*Offset=*/44, /*Size=*/12}}},
    {AIE2PS::fixup_aie2ps_48,
     {{/*Offset=*/38, /*Size=*/20}, {/*Offset=*/65, /*Size=*/12}}},
    {AIE2PS::fixup_aie2ps_49,
     {{/*Offset=*/59, /*Size=*/5}, {/*Offset=*/70, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_50,
     {{/*Offset=*/5, /*Size=*/5}, {/*Offset=*/16, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_51,
     {{/*Offset=*/21, /*Size=*/5}, {/*Offset=*/32, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_52,
     {{/*Offset=*/0, /*Size=*/5}, {/*Offset=*/11, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_53,
     {{/*Offset=*/10, /*Size=*/5}, {/*Offset=*/21, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_54,
     {{/*Offset=*/38, /*Size=*/5}, {/*Offset=*/49, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_55,
     {{/*Offset=*/53, /*Size=*/5}, {/*Offset=*/64, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_56,
     {{/*Offset=*/26, /*Size=*/5}, {/*Offset=*/37, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_57,
     {{/*Offset=*/1, /*Size=*/5}, {/*Offset=*/12, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_58,
     {{/*Offset=*/43, /*Size=*/5}, {/*Offset=*/54, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_59,
     {{/*Offset=*/21, /*Size=*/5}, {/*Offset=*/32, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_60,
     {{/*Offset=*/17, /*Size=*/5}, {/*Offset=*/28, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_61,
     {{/*Offset=*/38, /*Size=*/5}, {/*Offset=*/49, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_62,
     {{/*Offset=*/93, /*Size=*/4}, {/*Offset=*/86, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_63,
     {{/*Offset=*/18, /*Size=*/4}, {/*Offset=*/11, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_64,
     {{/*Offset=*/35, /*Size=*/4}, {/*Offset=*/28, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_65,
     {{/*Offset=*/34, /*Size=*/4}, {/*Offset=*/27, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_66,
     {{/*Offset=*/45, /*Size=*/4}, {/*Offset=*/38, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_67,
     {{/*Offset=*/19, /*Size=*/4}, {/*Offset=*/12, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_68,
     {{/*Offset=*/60, /*Size=*/4}, {/*Offset=*/53, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_69,
     {{/*Offset=*/35, /*Size=*/4}, {/*Offset=*/28, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_70,
     {{/*Offset=*/77, /*Size=*/4}, {/*Offset=*/70, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_71,
     {{/*Offset=*/52, /*Size=*/4}, {/*Offset=*/45, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_72,
     {{/*Offset=*/55, /*Size=*/4}, {/*Offset=*/48, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_73,
     {{/*Offset=*/51, /*Size=*/4}, {/*Offset=*/44, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_74,
     {{/*Offset=*/72, /*Size=*/4}, {/*Offset=*/65, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_75, {{/*Offset=*/64, /*Size=*/20}}},
    {AIE2PS::fixup_aie2ps_76, {{/*Offset=*/5, /*Size=*/20}}},
    {AIE2PS::fixup_aie2ps_77, {{/*Offset=*/31, /*Size=*/20}}},
    {AIE2PS::fixup_aie2ps_78, {{/*Offset=*/6, /*Size=*/20}}},
    {AIE2PS::fixup_aie2ps_79, {{/*Offset=*/48, /*Size=*/20}}},
    {AIE2PS::fixup_aie2ps_80, {{/*Offset=*/26, /*Size=*/20}}},
    {AIE2PS::fixup_aie2ps_81, {{/*Offset=*/22, /*Size=*/20}}},
    {AIE2PS::fixup_aie2ps_82, {{/*Offset=*/43, /*Size=*/20}}},
    {AIE2PS::fixup_aie2ps_83, {{/*Offset=*/85, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_84, {{/*Offset=*/10, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_85, {{/*Offset=*/27, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_86, {{/*Offset=*/26, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_87, {{/*Offset=*/37, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_88, {{/*Offset=*/11, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_89, {{/*Offset=*/52, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_90, {{/*Offset=*/27, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_91, {{/*Offset=*/69, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_92, {{/*Offset=*/44, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_93, {{/*Offset=*/47, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_94, {{/*Offset=*/43, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_95, {{/*Offset=*/64, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_96, {{/*Offset=*/91, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_97, {{/*Offset=*/16, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_98, {{/*Offset=*/33, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_99, {{/*Offset=*/32, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_100, {{/*Offset=*/43, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_101, {{/*Offset=*/17, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_102, {{/*Offset=*/58, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_103, {{/*Offset=*/33, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_104, {{/*Offset=*/75, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_105, {{/*Offset=*/50, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_106, {{/*Offset=*/53, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_107, {{/*Offset=*/49, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_108, {{/*Offset=*/70, /*Size=*/8}}},
    {AIE2PS::fixup_aie2ps_109,
     {{/*Offset=*/93, /*Size=*/4}, {/*Offset=*/86, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_110,
     {{/*Offset=*/18, /*Size=*/4}, {/*Offset=*/11, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_111,
     {{/*Offset=*/35, /*Size=*/4}, {/*Offset=*/28, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_112,
     {{/*Offset=*/34, /*Size=*/4}, {/*Offset=*/27, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_113,
     {{/*Offset=*/45, /*Size=*/4}, {/*Offset=*/38, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_114,
     {{/*Offset=*/19, /*Size=*/4}, {/*Offset=*/12, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_115,
     {{/*Offset=*/60, /*Size=*/4}, {/*Offset=*/53, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_116,
     {{/*Offset=*/35, /*Size=*/4}, {/*Offset=*/28, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_117,
     {{/*Offset=*/77, /*Size=*/4}, {/*Offset=*/70, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_118,
     {{/*Offset=*/52, /*Size=*/4}, {/*Offset=*/45, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_119,
     {{/*Offset=*/55, /*Size=*/4}, {/*Offset=*/48, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_120,
     {{/*Offset=*/51, /*Size=*/4}, {/*Offset=*/44, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_121,
     {{/*Offset=*/72, /*Size=*/4}, {/*Offset=*/65, /*Size=*/7}}},
    {AIE2PS::fixup_aie2ps_122,
     {{/*Offset=*/59, /*Size=*/5}, {/*Offset=*/70, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_123,
     {{/*Offset=*/5, /*Size=*/5}, {/*Offset=*/16, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_124,
     {{/*Offset=*/21, /*Size=*/5}, {/*Offset=*/32, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_125,
     {{/*Offset=*/0, /*Size=*/5}, {/*Offset=*/11, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_126,
     {{/*Offset=*/10, /*Size=*/5}, {/*Offset=*/21, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_127,
     {{/*Offset=*/38, /*Size=*/5}, {/*Offset=*/49, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_128,
     {{/*Offset=*/53, /*Size=*/5}, {/*Offset=*/64, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_129,
     {{/*Offset=*/26, /*Size=*/5}, {/*Offset=*/37, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_130,
     {{/*Offset=*/1, /*Size=*/5}, {/*Offset=*/12, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_131,
     {{/*Offset=*/43, /*Size=*/5}, {/*Offset=*/54, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_132,
     {{/*Offset=*/21, /*Size=*/5}, {/*Offset=*/32, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_133,
     {{/*Offset=*/17, /*Size=*/5}, {/*Offset=*/28, /*Size=*/6}}},
    {AIE2PS::fixup_aie2ps_134,
     {{/*Offset=*/38, /*Size=*/5}, {/*Offset=*/49, /*Size=*/6}}}};

/// This structure maps a set of fields to the corresponding set of Fixups,
/// which are dispatched according to the Format size they target.
/// NOTE: std::map garantees logarithmic complexity search into the data
/// structure.
const std::map<SmallVector<FixupField>, std::set<unsigned>>
    AIE2PSFixupFieldsMapper = {
        {{{/*Offset=*/64, /*Size=*/20}},
         {AIE2PS::fixup_aie2ps_0 /*(FormatSize=16, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_75 /*(FormatSize=16, Flag=Unrestricted)*/}},
        {{{/*Offset=*/5, /*Size=*/20}},
         {AIE2PS::fixup_aie2ps_1 /*(FormatSize=6, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_76 /*(FormatSize=6, Flag=Unrestricted)*/}},
        {{{/*Offset=*/31, /*Size=*/20}},
         {AIE2PS::fixup_aie2ps_2 /*(FormatSize=10, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_77 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/6, /*Size=*/20}},
         {AIE2PS::fixup_aie2ps_3 /*(FormatSize=10, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_78 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/48, /*Size=*/20}},
         {AIE2PS::fixup_aie2ps_4 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_79 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/26, /*Size=*/20}},
         {AIE2PS::fixup_aie2ps_5 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_80 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/22, /*Size=*/20}},
         {AIE2PS::fixup_aie2ps_6 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_81 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/43, /*Size=*/20}},
         {AIE2PS::fixup_aie2ps_7 /*(FormatSize=14, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_82 /*(FormatSize=14, Flag=Unrestricted)*/}},
        {{{/*Offset=*/0, /*Size=*/9}, {/*Offset=*/16, /*Size=*/2}},
         {AIE2PS::fixup_aie2ps_8 /*(FormatSize=16, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_10 /*(FormatSize=6, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_11 /*(FormatSize=8, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_12 /*(FormatSize=10, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_13 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_14 /*(FormatSize=14, Flag=Unrestricted)*/}},
        {{{/*Offset=*/4, /*Size=*/9}, {/*Offset=*/20, /*Size=*/2}},
         {AIE2PS::fixup_aie2ps_9 /*(FormatSize=4, Flag=Unrestricted)*/}},
        {{{/*Offset=*/59, /*Size=*/5}, {/*Offset=*/70, /*Size=*/6}},
         {AIE2PS::fixup_aie2ps_15 /*(FormatSize=16, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_49 /*(FormatSize=16, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_122 /*(FormatSize=16, Flag=Unrestricted)*/}},
        {{{/*Offset=*/5, /*Size=*/5}, {/*Offset=*/16, /*Size=*/6}},
         {AIE2PS::fixup_aie2ps_16 /*(FormatSize=4, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_50 /*(FormatSize=4, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_123 /*(FormatSize=4, Flag=Unrestricted)*/}},
        {{{/*Offset=*/21, /*Size=*/5}, {/*Offset=*/32, /*Size=*/6}},
         {AIE2PS::fixup_aie2ps_17 /*(FormatSize=6, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_25 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_51 /*(FormatSize=6, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_59 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_124 /*(FormatSize=6, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_132 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/0, /*Size=*/5}, {/*Offset=*/11, /*Size=*/6}},
         {AIE2PS::fixup_aie2ps_18 /*(FormatSize=6, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_52 /*(FormatSize=6, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_125 /*(FormatSize=6, Flag=Unrestricted)*/}},
        {{{/*Offset=*/10, /*Size=*/5}, {/*Offset=*/21, /*Size=*/6}},
         {AIE2PS::fixup_aie2ps_19 /*(FormatSize=8, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_53 /*(FormatSize=8, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_126 /*(FormatSize=8, Flag=Unrestricted)*/}},
        {{{/*Offset=*/38, /*Size=*/5}, {/*Offset=*/49, /*Size=*/6}},
         {AIE2PS::fixup_aie2ps_20 /*(FormatSize=8, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_27 /*(FormatSize=14, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_54 /*(FormatSize=8, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_61 /*(FormatSize=14, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_127 /*(FormatSize=8, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_134 /*(FormatSize=14, Flag=Unrestricted)*/}},
        {{{/*Offset=*/53, /*Size=*/5}, {/*Offset=*/64, /*Size=*/6}},
         {AIE2PS::fixup_aie2ps_21 /*(FormatSize=10, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_55 /*(FormatSize=10, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_128 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/26, /*Size=*/5}, {/*Offset=*/37, /*Size=*/6}},
         {AIE2PS::fixup_aie2ps_22 /*(FormatSize=10, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_56 /*(FormatSize=10, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_129 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/1, /*Size=*/5}, {/*Offset=*/12, /*Size=*/6}},
         {AIE2PS::fixup_aie2ps_23 /*(FormatSize=10, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_57 /*(FormatSize=10, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_130 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/43, /*Size=*/5}, {/*Offset=*/54, /*Size=*/6}},
         {AIE2PS::fixup_aie2ps_24 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_58 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_131 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/17, /*Size=*/5}, {/*Offset=*/28, /*Size=*/6}},
         {AIE2PS::fixup_aie2ps_26 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_60 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_133 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/93, /*Size=*/4}, {/*Offset=*/86, /*Size=*/7}},
         {AIE2PS::fixup_aie2ps_28 /*(FormatSize=16, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_62 /*(FormatSize=16, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_109 /*(FormatSize=16, Flag=Unrestricted)*/}},
        {{{/*Offset=*/18, /*Size=*/4}, {/*Offset=*/11, /*Size=*/7}},
         {AIE2PS::fixup_aie2ps_29 /*(FormatSize=4, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_63 /*(FormatSize=4, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_110 /*(FormatSize=4, Flag=Unrestricted)*/}},
        {{{/*Offset=*/35, /*Size=*/4}, {/*Offset=*/28, /*Size=*/7}},
         {AIE2PS::fixup_aie2ps_30 /*(FormatSize=6, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_35 /*(FormatSize=10, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_64 /*(FormatSize=6, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_69 /*(FormatSize=10, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_111 /*(FormatSize=6, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_116 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/34, /*Size=*/4}, {/*Offset=*/27, /*Size=*/7}},
         {AIE2PS::fixup_aie2ps_31 /*(FormatSize=6, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_65 /*(FormatSize=6, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_112 /*(FormatSize=6, Flag=Unrestricted)*/}},
        {{{/*Offset=*/45, /*Size=*/4}, {/*Offset=*/38, /*Size=*/7}},
         {AIE2PS::fixup_aie2ps_32 /*(FormatSize=8, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_66 /*(FormatSize=8, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_113 /*(FormatSize=8, Flag=Unrestricted)*/}},
        {{{/*Offset=*/19, /*Size=*/4}, {/*Offset=*/12, /*Size=*/7}},
         {AIE2PS::fixup_aie2ps_33 /*(FormatSize=8, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_67 /*(FormatSize=8, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_114 /*(FormatSize=8, Flag=Unrestricted)*/}},
        {{{/*Offset=*/60, /*Size=*/4}, {/*Offset=*/53, /*Size=*/7}},
         {AIE2PS::fixup_aie2ps_34 /*(FormatSize=10, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_68 /*(FormatSize=10, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_115 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/77, /*Size=*/4}, {/*Offset=*/70, /*Size=*/7}},
         {AIE2PS::fixup_aie2ps_36 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_70 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_117 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/52, /*Size=*/4}, {/*Offset=*/45, /*Size=*/7}},
         {AIE2PS::fixup_aie2ps_37 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_71 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_118 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/55, /*Size=*/4}, {/*Offset=*/48, /*Size=*/7}},
         {AIE2PS::fixup_aie2ps_38 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_72 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_119 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/51, /*Size=*/4}, {/*Offset=*/44, /*Size=*/7}},
         {AIE2PS::fixup_aie2ps_39 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_73 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_120 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/72, /*Size=*/4}, {/*Offset=*/65, /*Size=*/7}},
         {AIE2PS::fixup_aie2ps_40 /*(FormatSize=14, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_74 /*(FormatSize=14, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_121 /*(FormatSize=14, Flag=Unrestricted)*/}},
        {{{/*Offset=*/59, /*Size=*/20}, {/*Offset=*/86, /*Size=*/12}},
         {AIE2PS::fixup_aie2ps_41 /*(FormatSize=16, Flag=Unrestricted)*/}},
        {{{/*Offset=*/0, /*Size=*/20}, {/*Offset=*/27, /*Size=*/12}},
         {AIE2PS::fixup_aie2ps_42 /*(FormatSize=6, Flag=Unrestricted)*/}},
        {{{/*Offset=*/26, /*Size=*/20}, {/*Offset=*/53, /*Size=*/12}},
         {AIE2PS::fixup_aie2ps_43 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/1, /*Size=*/20}, {/*Offset=*/28, /*Size=*/12}},
         {AIE2PS::fixup_aie2ps_44 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/43, /*Size=*/20}, {/*Offset=*/70, /*Size=*/12}},
         {AIE2PS::fixup_aie2ps_45 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/21, /*Size=*/20}, {/*Offset=*/48, /*Size=*/12}},
         {AIE2PS::fixup_aie2ps_46 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/17, /*Size=*/20}, {/*Offset=*/44, /*Size=*/12}},
         {AIE2PS::fixup_aie2ps_47 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/38, /*Size=*/20}, {/*Offset=*/65, /*Size=*/12}},
         {AIE2PS::fixup_aie2ps_48 /*(FormatSize=14, Flag=Unrestricted)*/}},
        {{{/*Offset=*/85, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_83 /*(FormatSize=16, Flag=Unrestricted)*/}},
        {{{/*Offset=*/10, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_84 /*(FormatSize=4, Flag=Unrestricted)*/}},
        {{{/*Offset=*/27, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_85 /*(FormatSize=6, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_90 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/26, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_86 /*(FormatSize=6, Flag=Unrestricted)*/}},
        {{{/*Offset=*/37, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_87 /*(FormatSize=8, Flag=Unrestricted)*/}},
        {{{/*Offset=*/11, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_88 /*(FormatSize=8, Flag=Unrestricted)*/}},
        {{{/*Offset=*/52, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_89 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/69, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_91 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/44, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_92 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/47, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_93 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/43, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_94 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_100 /*(FormatSize=8, Flag=Unrestricted)*/}},
        {{{/*Offset=*/64, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_95 /*(FormatSize=14, Flag=Unrestricted)*/}},
        {{{/*Offset=*/91, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_96 /*(FormatSize=16, Flag=Unrestricted)*/}},
        {{{/*Offset=*/16, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_97 /*(FormatSize=4, Flag=Unrestricted)*/}},
        {{{/*Offset=*/33, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_98 /*(FormatSize=6, Flag=Unrestricted)*/,
          AIE2PS::fixup_aie2ps_103 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/32, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_99 /*(FormatSize=6, Flag=Unrestricted)*/}},
        {{{/*Offset=*/17, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_101 /*(FormatSize=8, Flag=Unrestricted)*/}},
        {{{/*Offset=*/58, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_102 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/75, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_104 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/50, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_105 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/53, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_106 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/49, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_107 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/70, /*Size=*/8}},
         {AIE2PS::fixup_aie2ps_108 /*(FormatSize=14, Flag=Unrestricted)*/}}};

/// Size of the (instruction) Format targeted by each AIE2PS fixups
const std::map<unsigned, unsigned> AIE2PSFixupFormatSize = {
    {AIE2PS::fixup_aie2ps_0, 16},   {AIE2PS::fixup_aie2ps_1, 6},
    {AIE2PS::fixup_aie2ps_2, 10},   {AIE2PS::fixup_aie2ps_3, 10},
    {AIE2PS::fixup_aie2ps_4, 12},   {AIE2PS::fixup_aie2ps_5, 12},
    {AIE2PS::fixup_aie2ps_6, 12},   {AIE2PS::fixup_aie2ps_7, 14},
    {AIE2PS::fixup_aie2ps_8, 16},   {AIE2PS::fixup_aie2ps_9, 4},
    {AIE2PS::fixup_aie2ps_10, 6},   {AIE2PS::fixup_aie2ps_11, 8},
    {AIE2PS::fixup_aie2ps_12, 10},  {AIE2PS::fixup_aie2ps_13, 12},
    {AIE2PS::fixup_aie2ps_14, 14},  {AIE2PS::fixup_aie2ps_15, 16},
    {AIE2PS::fixup_aie2ps_16, 4},   {AIE2PS::fixup_aie2ps_17, 6},
    {AIE2PS::fixup_aie2ps_18, 6},   {AIE2PS::fixup_aie2ps_19, 8},
    {AIE2PS::fixup_aie2ps_20, 8},   {AIE2PS::fixup_aie2ps_21, 10},
    {AIE2PS::fixup_aie2ps_22, 10},  {AIE2PS::fixup_aie2ps_23, 10},
    {AIE2PS::fixup_aie2ps_24, 12},  {AIE2PS::fixup_aie2ps_25, 12},
    {AIE2PS::fixup_aie2ps_26, 12},  {AIE2PS::fixup_aie2ps_27, 14},
    {AIE2PS::fixup_aie2ps_28, 16},  {AIE2PS::fixup_aie2ps_29, 4},
    {AIE2PS::fixup_aie2ps_30, 6},   {AIE2PS::fixup_aie2ps_31, 6},
    {AIE2PS::fixup_aie2ps_32, 8},   {AIE2PS::fixup_aie2ps_33, 8},
    {AIE2PS::fixup_aie2ps_34, 10},  {AIE2PS::fixup_aie2ps_35, 10},
    {AIE2PS::fixup_aie2ps_36, 12},  {AIE2PS::fixup_aie2ps_37, 12},
    {AIE2PS::fixup_aie2ps_38, 12},  {AIE2PS::fixup_aie2ps_39, 12},
    {AIE2PS::fixup_aie2ps_40, 14},  {AIE2PS::fixup_aie2ps_41, 16},
    {AIE2PS::fixup_aie2ps_42, 6},   {AIE2PS::fixup_aie2ps_43, 10},
    {AIE2PS::fixup_aie2ps_44, 10},  {AIE2PS::fixup_aie2ps_45, 12},
    {AIE2PS::fixup_aie2ps_46, 12},  {AIE2PS::fixup_aie2ps_47, 12},
    {AIE2PS::fixup_aie2ps_48, 14},  {AIE2PS::fixup_aie2ps_49, 16},
    {AIE2PS::fixup_aie2ps_50, 4},   {AIE2PS::fixup_aie2ps_51, 6},
    {AIE2PS::fixup_aie2ps_52, 6},   {AIE2PS::fixup_aie2ps_53, 8},
    {AIE2PS::fixup_aie2ps_54, 8},   {AIE2PS::fixup_aie2ps_55, 10},
    {AIE2PS::fixup_aie2ps_56, 10},  {AIE2PS::fixup_aie2ps_57, 10},
    {AIE2PS::fixup_aie2ps_58, 12},  {AIE2PS::fixup_aie2ps_59, 12},
    {AIE2PS::fixup_aie2ps_60, 12},  {AIE2PS::fixup_aie2ps_61, 14},
    {AIE2PS::fixup_aie2ps_62, 16},  {AIE2PS::fixup_aie2ps_63, 4},
    {AIE2PS::fixup_aie2ps_64, 6},   {AIE2PS::fixup_aie2ps_65, 6},
    {AIE2PS::fixup_aie2ps_66, 8},   {AIE2PS::fixup_aie2ps_67, 8},
    {AIE2PS::fixup_aie2ps_68, 10},  {AIE2PS::fixup_aie2ps_69, 10},
    {AIE2PS::fixup_aie2ps_70, 12},  {AIE2PS::fixup_aie2ps_71, 12},
    {AIE2PS::fixup_aie2ps_72, 12},  {AIE2PS::fixup_aie2ps_73, 12},
    {AIE2PS::fixup_aie2ps_74, 14},  {AIE2PS::fixup_aie2ps_75, 16},
    {AIE2PS::fixup_aie2ps_76, 6},   {AIE2PS::fixup_aie2ps_77, 10},
    {AIE2PS::fixup_aie2ps_78, 10},  {AIE2PS::fixup_aie2ps_79, 12},
    {AIE2PS::fixup_aie2ps_80, 12},  {AIE2PS::fixup_aie2ps_81, 12},
    {AIE2PS::fixup_aie2ps_82, 14},  {AIE2PS::fixup_aie2ps_83, 16},
    {AIE2PS::fixup_aie2ps_84, 4},   {AIE2PS::fixup_aie2ps_85, 6},
    {AIE2PS::fixup_aie2ps_86, 6},   {AIE2PS::fixup_aie2ps_87, 8},
    {AIE2PS::fixup_aie2ps_88, 8},   {AIE2PS::fixup_aie2ps_89, 10},
    {AIE2PS::fixup_aie2ps_90, 10},  {AIE2PS::fixup_aie2ps_91, 12},
    {AIE2PS::fixup_aie2ps_92, 12},  {AIE2PS::fixup_aie2ps_93, 12},
    {AIE2PS::fixup_aie2ps_94, 12},  {AIE2PS::fixup_aie2ps_95, 14},
    {AIE2PS::fixup_aie2ps_96, 16},  {AIE2PS::fixup_aie2ps_97, 4},
    {AIE2PS::fixup_aie2ps_98, 6},   {AIE2PS::fixup_aie2ps_99, 6},
    {AIE2PS::fixup_aie2ps_100, 8},  {AIE2PS::fixup_aie2ps_101, 8},
    {AIE2PS::fixup_aie2ps_102, 10}, {AIE2PS::fixup_aie2ps_103, 10},
    {AIE2PS::fixup_aie2ps_104, 12}, {AIE2PS::fixup_aie2ps_105, 12},
    {AIE2PS::fixup_aie2ps_106, 12}, {AIE2PS::fixup_aie2ps_107, 12},
    {AIE2PS::fixup_aie2ps_108, 14}, {AIE2PS::fixup_aie2ps_109, 16},
    {AIE2PS::fixup_aie2ps_110, 4},  {AIE2PS::fixup_aie2ps_111, 6},
    {AIE2PS::fixup_aie2ps_112, 6},  {AIE2PS::fixup_aie2ps_113, 8},
    {AIE2PS::fixup_aie2ps_114, 8},  {AIE2PS::fixup_aie2ps_115, 10},
    {AIE2PS::fixup_aie2ps_116, 10}, {AIE2PS::fixup_aie2ps_117, 12},
    {AIE2PS::fixup_aie2ps_118, 12}, {AIE2PS::fixup_aie2ps_119, 12},
    {AIE2PS::fixup_aie2ps_120, 12}, {AIE2PS::fixup_aie2ps_121, 14},
    {AIE2PS::fixup_aie2ps_122, 16}, {AIE2PS::fixup_aie2ps_123, 4},
    {AIE2PS::fixup_aie2ps_124, 6},  {AIE2PS::fixup_aie2ps_125, 6},
    {AIE2PS::fixup_aie2ps_126, 8},  {AIE2PS::fixup_aie2ps_127, 8},
    {AIE2PS::fixup_aie2ps_128, 10}, {AIE2PS::fixup_aie2ps_129, 10},
    {AIE2PS::fixup_aie2ps_130, 10}, {AIE2PS::fixup_aie2ps_131, 12},
    {AIE2PS::fixup_aie2ps_132, 12}, {AIE2PS::fixup_aie2ps_133, 12},
    {AIE2PS::fixup_aie2ps_134, 14}};

/// Map between each supported fixup and a flag indicating the signedness
const std::map<unsigned, FixupFlag> AIE2PSFixupFlagMap = {
    {AIE2PS::fixup_aie2ps_0, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_1, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_2, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_3, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_4, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_5, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_6, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_7, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_8, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_9, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_10, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_11, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_12, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_13, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_14, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_15, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_16, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_17, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_18, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_19, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_20, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_21, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_22, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_23, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_24, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_25, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_26, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_27, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_28, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_29, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_30, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_31, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_32, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_33, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_34, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_35, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_36, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_37, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_38, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_39, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_40, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_41, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_42, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_43, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_44, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_45, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_46, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_47, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_48, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_49, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_50, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_51, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_52, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_53, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_54, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_55, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_56, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_57, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_58, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_59, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_60, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_61, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_62, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_63, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_64, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_65, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_66, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_67, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_68, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_69, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_70, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_71, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_72, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_73, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_74, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_75, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_76, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_77, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_78, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_79, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_80, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_81, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_82, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_83, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_84, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_85, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_86, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_87, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_88, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_89, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_90, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_91, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_92, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_93, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_94, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_95, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_96, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_97, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_98, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_99, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_100, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_101, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_102, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_103, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_104, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_105, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_106, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_107, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_108, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_109, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_110, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_111, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_112, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_113, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_114, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_115, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_116, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_117, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_118, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_119, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_120, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_121, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_122, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_123, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_124, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_125, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_126, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_127, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_128, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_129, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_130, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_131, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_132, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_133, FixupFlag::Unrestricted},
    {AIE2PS::fixup_aie2ps_134, FixupFlag::Unrestricted}};

std::unique_ptr<AIEMCFixupKinds> llvm::createAIE2PSMCFixupKinds() {
  return std::make_unique<AIEMCFixupKinds>(
      AIE2PSFixupFieldsInfos, AIE2PSFixupFieldsMapper, AIE2PSFixupFormatSize,
      AIE2PSFixupFlagMap, AIE2PSInstrFixupFlags);
}

#endif // GET_MCFIXUPKINDS_IMPLEM

#ifdef RELOCS_DEFINITIONS
#undef RELOCS_DEFINITIONS

// The following declarations must be copied in a separate file at
// llvm/include/llvm/BinaryFormat/ELFRelocs/AIE2PS.def
#ifndef ELF_RELOC
#error "ELF_RELOC must be defined"
#endif

ELF_RELOC(R_AIE2PS_0, 0)
ELF_RELOC(R_AIE2PS_1, 1)
ELF_RELOC(R_AIE2PS_2, 2)
ELF_RELOC(R_AIE2PS_3, 3)
ELF_RELOC(R_AIE2PS_4, 4)
ELF_RELOC(R_AIE2PS_5, 5)
ELF_RELOC(R_AIE2PS_6, 6)
ELF_RELOC(R_AIE2PS_7, 7)
ELF_RELOC(R_AIE2PS_8, 8)
ELF_RELOC(R_AIE2PS_9, 9)
ELF_RELOC(R_AIE2PS_10, 10)
ELF_RELOC(R_AIE2PS_11, 11)
ELF_RELOC(R_AIE2PS_12, 12)
ELF_RELOC(R_AIE2PS_13, 13)
ELF_RELOC(R_AIE2PS_14, 14)
ELF_RELOC(R_AIE2PS_15, 15)
ELF_RELOC(R_AIE2PS_16, 16)
ELF_RELOC(R_AIE2PS_17, 17)
ELF_RELOC(R_AIE2PS_18, 18)
ELF_RELOC(R_AIE2PS_19, 19)
ELF_RELOC(R_AIE2PS_20, 20)
ELF_RELOC(R_AIE2PS_21, 21)
ELF_RELOC(R_AIE2PS_22, 22)
ELF_RELOC(R_AIE2PS_23, 23)
ELF_RELOC(R_AIE2PS_24, 24)
ELF_RELOC(R_AIE2PS_25, 25)
ELF_RELOC(R_AIE2PS_26, 26)
ELF_RELOC(R_AIE2PS_27, 27)
ELF_RELOC(R_AIE2PS_28, 28)
ELF_RELOC(R_AIE2PS_29, 29)
ELF_RELOC(R_AIE2PS_30, 30)
ELF_RELOC(R_AIE2PS_31, 31)
ELF_RELOC(R_AIE2PS_32, 32)
ELF_RELOC(R_AIE2PS_33, 33)
ELF_RELOC(R_AIE2PS_34, 34)
ELF_RELOC(R_AIE2PS_35, 35)
ELF_RELOC(R_AIE2PS_36, 36)
ELF_RELOC(R_AIE2PS_37, 37)
ELF_RELOC(R_AIE2PS_38, 38)
ELF_RELOC(R_AIE2PS_39, 39)
ELF_RELOC(R_AIE2PS_40, 40)
ELF_RELOC(R_AIE2PS_41, 41)
ELF_RELOC(R_AIE2PS_42, 42)
ELF_RELOC(R_AIE2PS_43, 43)
ELF_RELOC(R_AIE2PS_44, 44)
ELF_RELOC(R_AIE2PS_45, 45)
ELF_RELOC(R_AIE2PS_46, 46)
ELF_RELOC(R_AIE2PS_47, 47)
ELF_RELOC(R_AIE2PS_48, 48)
ELF_RELOC(R_AIE2PS_49, 49)
ELF_RELOC(R_AIE2PS_50, 50)
ELF_RELOC(R_AIE2PS_51, 51)
ELF_RELOC(R_AIE2PS_52, 52)
ELF_RELOC(R_AIE2PS_53, 53)
ELF_RELOC(R_AIE2PS_54, 54)
ELF_RELOC(R_AIE2PS_55, 55)
ELF_RELOC(R_AIE2PS_56, 56)
ELF_RELOC(R_AIE2PS_57, 57)
ELF_RELOC(R_AIE2PS_58, 58)
ELF_RELOC(R_AIE2PS_59, 59)
ELF_RELOC(R_AIE2PS_60, 60)
ELF_RELOC(R_AIE2PS_61, 61)
ELF_RELOC(R_AIE2PS_62, 62)
ELF_RELOC(R_AIE2PS_63, 63)
ELF_RELOC(R_AIE2PS_64, 64)
ELF_RELOC(R_AIE2PS_65, 65)
ELF_RELOC(R_AIE2PS_66, 66)
ELF_RELOC(R_AIE2PS_67, 67)
ELF_RELOC(R_AIE2PS_68, 68)
ELF_RELOC(R_AIE2PS_69, 69)
ELF_RELOC(R_AIE2PS_70, 70)
ELF_RELOC(R_AIE2PS_71, 71)
ELF_RELOC(R_AIE2PS_72, 72)
ELF_RELOC(R_AIE2PS_73, 73)
ELF_RELOC(R_AIE2PS_74, 74)
ELF_RELOC(R_AIE2PS_75, 75)
ELF_RELOC(R_AIE2PS_76, 76)
ELF_RELOC(R_AIE2PS_77, 77)
ELF_RELOC(R_AIE2PS_78, 78)
ELF_RELOC(R_AIE2PS_79, 79)
ELF_RELOC(R_AIE2PS_80, 80)
ELF_RELOC(R_AIE2PS_81, 81)
ELF_RELOC(R_AIE2PS_82, 82)
ELF_RELOC(R_AIE2PS_83, 83)
ELF_RELOC(R_AIE2PS_84, 84)
ELF_RELOC(R_AIE2PS_85, 85)
ELF_RELOC(R_AIE2PS_86, 86)
ELF_RELOC(R_AIE2PS_87, 87)
ELF_RELOC(R_AIE2PS_88, 88)
ELF_RELOC(R_AIE2PS_89, 89)
ELF_RELOC(R_AIE2PS_90, 90)
ELF_RELOC(R_AIE2PS_91, 91)
ELF_RELOC(R_AIE2PS_92, 92)
ELF_RELOC(R_AIE2PS_93, 93)
ELF_RELOC(R_AIE2PS_94, 94)
ELF_RELOC(R_AIE2PS_95, 95)
ELF_RELOC(R_AIE2PS_96, 96)
ELF_RELOC(R_AIE2PS_97, 97)
ELF_RELOC(R_AIE2PS_98, 98)
ELF_RELOC(R_AIE2PS_99, 99)
ELF_RELOC(R_AIE2PS_100, 100)
ELF_RELOC(R_AIE2PS_101, 101)
ELF_RELOC(R_AIE2PS_102, 102)
ELF_RELOC(R_AIE2PS_103, 103)
ELF_RELOC(R_AIE2PS_104, 104)
ELF_RELOC(R_AIE2PS_105, 105)
ELF_RELOC(R_AIE2PS_106, 106)
ELF_RELOC(R_AIE2PS_107, 107)
ELF_RELOC(R_AIE2PS_108, 108)
ELF_RELOC(R_AIE2PS_109, 109)
ELF_RELOC(R_AIE2PS_110, 110)
ELF_RELOC(R_AIE2PS_111, 111)
ELF_RELOC(R_AIE2PS_112, 112)
ELF_RELOC(R_AIE2PS_113, 113)
ELF_RELOC(R_AIE2PS_114, 114)
ELF_RELOC(R_AIE2PS_115, 115)
ELF_RELOC(R_AIE2PS_116, 116)
ELF_RELOC(R_AIE2PS_117, 117)
ELF_RELOC(R_AIE2PS_118, 118)
ELF_RELOC(R_AIE2PS_119, 119)
ELF_RELOC(R_AIE2PS_120, 120)
ELF_RELOC(R_AIE2PS_121, 121)
ELF_RELOC(R_AIE2PS_122, 122)
ELF_RELOC(R_AIE2PS_123, 123)
ELF_RELOC(R_AIE2PS_124, 124)
ELF_RELOC(R_AIE2PS_125, 125)
ELF_RELOC(R_AIE2PS_126, 126)
ELF_RELOC(R_AIE2PS_127, 127)
ELF_RELOC(R_AIE2PS_128, 128)
ELF_RELOC(R_AIE2PS_129, 129)
ELF_RELOC(R_AIE2PS_130, 130)
ELF_RELOC(R_AIE2PS_131, 131)
ELF_RELOC(R_AIE2PS_132, 132)
ELF_RELOC(R_AIE2PS_133, 133)
ELF_RELOC(R_AIE2PS_134, 134)
ELF_RELOC(R_AIE2PS_135, 135)
ELF_RELOC(R_AIE2PS_136, 136)
ELF_RELOC(R_AIE2PS_137, 137)
ELF_RELOC(R_AIE2PS_138, 138)

#endif // RELOCS_DEFINITIONS
