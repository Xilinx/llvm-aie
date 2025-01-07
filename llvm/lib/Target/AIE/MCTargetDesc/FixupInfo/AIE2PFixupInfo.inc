//===- AIE2PFixupInfo.inc
//---------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
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
  fixup_aie2p_0 = FirstTargetFixupKind, /* => R_AIE2P_0 (iword) */
  fixup_aie2p_1,                        /* => R_AIE2P_1 (iword) */
  fixup_aie2p_2,                        /* => R_AIE2P_2 (iword) */
  fixup_aie2p_3,                        /* => R_AIE2P_3 (iword) */
  fixup_aie2p_4,                        /* => R_AIE2P_4 (iword) */
  fixup_aie2p_5,                        /* => R_AIE2P_5 (iword) */
  fixup_aie2p_6,                        /* => R_AIE2P_6 (iword) */
  fixup_aie2p_7,                        /* => R_AIE2P_7 (iword) */
  fixup_aie2p_8,                        /* => R_AIE2P_8 (iword) */
  fixup_aie2p_9,                        /* => R_AIE2P_9 (iword) */
  fixup_aie2p_10,                       /* => R_AIE2P_10 (iword) */
  fixup_aie2p_11,                       /* => R_AIE2P_11 (iword) */
  fixup_aie2p_12,                       /* => R_AIE2P_12 (iword) */
  fixup_aie2p_13,                       /* => R_AIE2P_13 (iword) */
  fixup_aie2p_14,                       /* => R_AIE2P_14 (iword) */
  fixup_aie2p_15,                       /* => R_AIE2P_15 (iword) */
  fixup_aie2p_16,                       /* => R_AIE2P_16 (iword) */
  fixup_aie2p_17,                       /* => R_AIE2P_17 (iword) */
  fixup_aie2p_18,                       /* => R_AIE2P_18 (iword) */
  fixup_aie2p_19,                       /* => R_AIE2P_19 (iword) */
  fixup_aie2p_20,                       /* => R_AIE2P_20 (iword) */
  fixup_aie2p_21,                       /* => R_AIE2P_21 (iword) */
  fixup_aie2p_22,                       /* => R_AIE2P_22 (iword) */
  fixup_aie2p_23,                       /* => R_AIE2P_23 (iword) */
  fixup_aie2p_24,                       /* => R_AIE2P_24 (iword) */
  fixup_aie2p_25,                       /* => R_AIE2P_25 (iword) */
  fixup_aie2p_26,                       /* => R_AIE2P_26 (iword) */
  fixup_aie2p_27,                       /* => R_AIE2P_27 (iword) */
  fixup_aie2p_28,                       /* => R_AIE2P_28 (iword) */
  fixup_aie2p_29,                       /* => R_AIE2P_29 (iword) */
  fixup_aie2p_30,                       /* => R_AIE2P_30 (iword) */
  fixup_aie2p_31,                       /* => R_AIE2P_31 (iword) */
  fixup_aie2p_32,                       /* => R_AIE2P_32 (iword) */
  fixup_aie2p_33,                       /* => R_AIE2P_33 (iword) */
  fixup_aie2p_34,                       /* => R_AIE2P_34 (iword) */
  fixup_aie2p_35,                       /* => R_AIE2P_35 (iword) */
  fixup_aie2p_36,                       /* => R_AIE2P_36 (iword) */
  fixup_aie2p_37,                       /* => R_AIE2P_37 (iword) */
  fixup_aie2p_38,                       /* => R_AIE2P_38 (iword) */
  fixup_aie2p_39,                       /* => R_AIE2P_39 (iword) */
  fixup_aie2p_40,                       /* => R_AIE2P_40 (iword) */
  fixup_aie2p_41,                       /* => R_AIE2P_41 (iword) */
  fixup_aie2p_42,                       /* => R_AIE2P_42 (iword) */
  fixup_aie2p_43,                       /* => R_AIE2P_43 (iword) */
  fixup_aie2p_44,                       /* => R_AIE2P_44 (iword) */
  fixup_aie2p_45,                       /* => R_AIE2P_45 (iword) */
  fixup_aie2p_46,                       /* => R_AIE2P_46 (iword) */
  fixup_aie2p_47,                       /* => R_AIE2P_47 (iword) */
  fixup_aie2p_48,                       /* => R_AIE2P_48 (iword) */
  fixup_aie2p_49,                       /* => R_AIE2P_49 (iword) */
  fixup_aie2p_50,                       /* => R_AIE2P_50 (iword) */
  fixup_aie2p_51,                       /* => R_AIE2P_51 (iword) */
  fixup_aie2p_52,                       /* => R_AIE2P_52 (iword) */
  fixup_aie2p_53,                       /* => R_AIE2P_53 (iword) */
  fixup_aie2p_54,                       /* => R_AIE2P_54 (iword) */
  fixup_aie2p_55,                       /* => R_AIE2P_55 (iword) */
  fixup_aie2p_56,                       /* => R_AIE2P_56 (iword) */
  fixup_aie2p_57,                       /* => R_AIE2P_57 (iword) */
  fixup_aie2p_58,                       /* => R_AIE2P_58 (iword) */
  fixup_aie2p_59,                       /* => R_AIE2P_59 (iword) */
  fixup_aie2p_60,                       /* => R_AIE2P_60 (iword) */
  fixup_aie2p_61,                       /* => R_AIE2P_61 (iword) */
  fixup_aie2p_62,                       /* => R_AIE2P_62 (dmword) */
  fixup_aie2p_63,                       /* => R_AIE2P_63 (dmword) */
  fixup_aie2p_64,                       /* => R_AIE2P_64 (dmword) */
  fixup_aie2p_65,                       /* => R_AIE2P_65 (dmword) */

  // fixup_invalid - used as a sentinel and a marker, must be last fixup
  fixup_invalid,
  NumTargetFixupKinds = fixup_invalid - FirstTargetFixupKind
};

#endif // GET_FIXUPS_DECL

#ifdef GET_FIXUPKINDS_DECL
#undef GET_FIXUPKINDS_DECL

std::unique_ptr<AIEMCFixupKinds> createAIE2PMCFixupKinds();

#endif // GET_FIXUPKINDS_DECL

#ifdef GET_MCFIXUPKINDS_IMPLEM
#undef GET_MCFIXUPKINDS_IMPLEM

/// Map between each supported Fixup and its corresponding set of fields that
/// it patches up.
/// The offsets are expressed in Big endian.
const std::map<unsigned, SmallVector<FixupField>> AIE2PFixupFieldsInfos = {
    {AIE2P::fixup_aie2p_0, {{/*Offset=*/63, /*Size=*/20}}},
    {AIE2P::fixup_aie2p_1, {{/*Offset=*/5, /*Size=*/20}}},
    {AIE2P::fixup_aie2p_2, {{/*Offset=*/31, /*Size=*/20}}},
    {AIE2P::fixup_aie2p_3, {{/*Offset=*/10, /*Size=*/20}}},
    {AIE2P::fixup_aie2p_4, {{/*Offset=*/47, /*Size=*/20}}},
    {AIE2P::fixup_aie2p_5, {{/*Offset=*/26, /*Size=*/20}}},
    {AIE2P::fixup_aie2p_6, {{/*Offset=*/22, /*Size=*/20}}},
    {AIE2P::fixup_aie2p_7, {{/*Offset=*/42, /*Size=*/20}}},
    {AIE2P::fixup_aie2p_8, {{/*Offset=*/0, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_9, {{/*Offset=*/5, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_10, {{/*Offset=*/0, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_11, {{/*Offset=*/0, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_12, {{/*Offset=*/0, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_13, {{/*Offset=*/0, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_14, {{/*Offset=*/0, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_15,
     {{/*Offset=*/58, /*Size=*/5}, {/*Offset=*/69, /*Size=*/6}}},
    {AIE2P::fixup_aie2p_16,
     {{/*Offset=*/5, /*Size=*/5}, {/*Offset=*/16, /*Size=*/6}}},
    {AIE2P::fixup_aie2p_17,
     {{/*Offset=*/20, /*Size=*/5}, {/*Offset=*/31, /*Size=*/6}}},
    {AIE2P::fixup_aie2p_18,
     {{/*Offset=*/0, /*Size=*/5}, {/*Offset=*/11, /*Size=*/6}}},
    {AIE2P::fixup_aie2p_19,
     {{/*Offset=*/10, /*Size=*/5}, {/*Offset=*/21, /*Size=*/6}}},
    {AIE2P::fixup_aie2p_20,
     {{/*Offset=*/37, /*Size=*/5}, {/*Offset=*/48, /*Size=*/6}}},
    {AIE2P::fixup_aie2p_21,
     {{/*Offset=*/53, /*Size=*/5}, {/*Offset=*/64, /*Size=*/6}}},
    {AIE2P::fixup_aie2p_22,
     {{/*Offset=*/26, /*Size=*/5}, {/*Offset=*/37, /*Size=*/6}}},
    {AIE2P::fixup_aie2p_23,
     {{/*Offset=*/5, /*Size=*/5}, {/*Offset=*/16, /*Size=*/6}}},
    {AIE2P::fixup_aie2p_24,
     {{/*Offset=*/42, /*Size=*/5}, {/*Offset=*/53, /*Size=*/6}}},
    {AIE2P::fixup_aie2p_25,
     {{/*Offset=*/21, /*Size=*/5}, {/*Offset=*/32, /*Size=*/6}}},
    {AIE2P::fixup_aie2p_26,
     {{/*Offset=*/17, /*Size=*/5}, {/*Offset=*/28, /*Size=*/6}}},
    {AIE2P::fixup_aie2p_27,
     {{/*Offset=*/37, /*Size=*/5}, {/*Offset=*/48, /*Size=*/6}}},
    {AIE2P::fixup_aie2p_28, {{/*Offset=*/85, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_29, {{/*Offset=*/12, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_30, {{/*Offset=*/27, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_31, {{/*Offset=*/37, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_32, {{/*Offset=*/12, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_33, {{/*Offset=*/53, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_34, {{/*Offset=*/28, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_35, {{/*Offset=*/32, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_36, {{/*Offset=*/69, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_37, {{/*Offset=*/44, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_38, {{/*Offset=*/48, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_39, {{/*Offset=*/65, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_40, {{/*Offset=*/64, /*Size=*/11}}},
    {AIE2P::fixup_aie2p_41,
     {{/*Offset=*/58, /*Size=*/20}, {/*Offset=*/85, /*Size=*/12}}},
    {AIE2P::fixup_aie2p_42,
     {{/*Offset=*/0, /*Size=*/20}, {/*Offset=*/27, /*Size=*/12}}},
    {AIE2P::fixup_aie2p_43,
     {{/*Offset=*/26, /*Size=*/20}, {/*Offset=*/53, /*Size=*/12}}},
    {AIE2P::fixup_aie2p_44,
     {{/*Offset=*/5, /*Size=*/20}, {/*Offset=*/32, /*Size=*/12}}},
    {AIE2P::fixup_aie2p_45,
     {{/*Offset=*/42, /*Size=*/20}, {/*Offset=*/69, /*Size=*/12}}},
    {AIE2P::fixup_aie2p_46,
     {{/*Offset=*/21, /*Size=*/20}, {/*Offset=*/48, /*Size=*/12}}},
    {AIE2P::fixup_aie2p_47,
     {{/*Offset=*/17, /*Size=*/20}, {/*Offset=*/44, /*Size=*/12}}},
    {AIE2P::fixup_aie2p_48,
     {{/*Offset=*/37, /*Size=*/20}, {/*Offset=*/64, /*Size=*/12}}},
    {AIE2P::fixup_aie2p_49, {{/*Offset=*/90, /*Size=*/8}}},
    {AIE2P::fixup_aie2p_50, {{/*Offset=*/17, /*Size=*/8}}},
    {AIE2P::fixup_aie2p_51, {{/*Offset=*/32, /*Size=*/8}}},
    {AIE2P::fixup_aie2p_52, {{/*Offset=*/42, /*Size=*/8}}},
    {AIE2P::fixup_aie2p_53, {{/*Offset=*/17, /*Size=*/8}}},
    {AIE2P::fixup_aie2p_54, {{/*Offset=*/58, /*Size=*/8}}},
    {AIE2P::fixup_aie2p_55, {{/*Offset=*/33, /*Size=*/8}}},
    {AIE2P::fixup_aie2p_56, {{/*Offset=*/37, /*Size=*/8}}},
    {AIE2P::fixup_aie2p_57, {{/*Offset=*/74, /*Size=*/8}}},
    {AIE2P::fixup_aie2p_58, {{/*Offset=*/49, /*Size=*/8}}},
    {AIE2P::fixup_aie2p_59, {{/*Offset=*/53, /*Size=*/8}}},
    {AIE2P::fixup_aie2p_60, {{/*Offset=*/70, /*Size=*/8}}},
    {AIE2P::fixup_aie2p_61, {{/*Offset=*/69, /*Size=*/8}}}};

/// This structure maps a set of fields to the corresponding set of Fixups,
/// which are dispatched according to the Format size they target.
/// NOTE: std::map garantees logarithmic complexity search into the data
/// structure.
const std::map<SmallVector<FixupField>, std::set<unsigned>>
    AIE2PFixupFieldsMapper = {
        {{{/*Offset=*/63, /*Size=*/20}},
         {AIE2P::fixup_aie2p_0 /*(FormatSize=16, Flag=Unrestricted)*/}},
        {{{/*Offset=*/5, /*Size=*/20}},
         {AIE2P::fixup_aie2p_1 /*(FormatSize=6, Flag=Unrestricted)*/}},
        {{{/*Offset=*/31, /*Size=*/20}},
         {AIE2P::fixup_aie2p_2 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/10, /*Size=*/20}},
         {AIE2P::fixup_aie2p_3 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/47, /*Size=*/20}},
         {AIE2P::fixup_aie2p_4 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/26, /*Size=*/20}},
         {AIE2P::fixup_aie2p_5 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/22, /*Size=*/20}},
         {AIE2P::fixup_aie2p_6 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/42, /*Size=*/20}},
         {AIE2P::fixup_aie2p_7 /*(FormatSize=14, Flag=Unrestricted)*/}},
        {{{/*Offset=*/0, /*Size=*/11}},
         {AIE2P::fixup_aie2p_8 /*(FormatSize=16, Flag=Unrestricted)*/,
          AIE2P::fixup_aie2p_10 /*(FormatSize=6, Flag=Unrestricted)*/,
          AIE2P::fixup_aie2p_11 /*(FormatSize=8, Flag=Unrestricted)*/,
          AIE2P::fixup_aie2p_12 /*(FormatSize=10, Flag=Unrestricted)*/,
          AIE2P::fixup_aie2p_13 /*(FormatSize=12, Flag=Unrestricted)*/,
          AIE2P::fixup_aie2p_14 /*(FormatSize=14, Flag=Unrestricted)*/}},
        {{{/*Offset=*/5, /*Size=*/11}},
         {AIE2P::fixup_aie2p_9 /*(FormatSize=4, Flag=Unrestricted)*/}},
        {{{/*Offset=*/58, /*Size=*/5}, {/*Offset=*/69, /*Size=*/6}},
         {AIE2P::fixup_aie2p_15 /*(FormatSize=16, Flag=Unrestricted)*/}},
        {{{/*Offset=*/5, /*Size=*/5}, {/*Offset=*/16, /*Size=*/6}},
         {AIE2P::fixup_aie2p_16 /*(FormatSize=4, Flag=Unrestricted)*/,
          AIE2P::fixup_aie2p_23 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/20, /*Size=*/5}, {/*Offset=*/31, /*Size=*/6}},
         {AIE2P::fixup_aie2p_17 /*(FormatSize=6, Flag=Unrestricted)*/}},
        {{{/*Offset=*/0, /*Size=*/5}, {/*Offset=*/11, /*Size=*/6}},
         {AIE2P::fixup_aie2p_18 /*(FormatSize=6, Flag=Unrestricted)*/}},
        {{{/*Offset=*/10, /*Size=*/5}, {/*Offset=*/21, /*Size=*/6}},
         {AIE2P::fixup_aie2p_19 /*(FormatSize=8, Flag=Unrestricted)*/}},
        {{{/*Offset=*/37, /*Size=*/5}, {/*Offset=*/48, /*Size=*/6}},
         {AIE2P::fixup_aie2p_20 /*(FormatSize=8, Flag=Unrestricted)*/,
          AIE2P::fixup_aie2p_27 /*(FormatSize=14, Flag=Unrestricted)*/}},
        {{{/*Offset=*/53, /*Size=*/5}, {/*Offset=*/64, /*Size=*/6}},
         {AIE2P::fixup_aie2p_21 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/26, /*Size=*/5}, {/*Offset=*/37, /*Size=*/6}},
         {AIE2P::fixup_aie2p_22 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/42, /*Size=*/5}, {/*Offset=*/53, /*Size=*/6}},
         {AIE2P::fixup_aie2p_24 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/21, /*Size=*/5}, {/*Offset=*/32, /*Size=*/6}},
         {AIE2P::fixup_aie2p_25 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/17, /*Size=*/5}, {/*Offset=*/28, /*Size=*/6}},
         {AIE2P::fixup_aie2p_26 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/85, /*Size=*/11}},
         {AIE2P::fixup_aie2p_28 /*(FormatSize=16, Flag=Unrestricted)*/}},
        {{{/*Offset=*/12, /*Size=*/11}},
         {AIE2P::fixup_aie2p_29 /*(FormatSize=4, Flag=Unrestricted)*/,
          AIE2P::fixup_aie2p_32 /*(FormatSize=8, Flag=Unrestricted)*/}},
        {{{/*Offset=*/27, /*Size=*/11}},
         {AIE2P::fixup_aie2p_30 /*(FormatSize=6, Flag=Unrestricted)*/}},
        {{{/*Offset=*/37, /*Size=*/11}},
         {AIE2P::fixup_aie2p_31 /*(FormatSize=8, Flag=Unrestricted)*/}},
        {{{/*Offset=*/53, /*Size=*/11}},
         {AIE2P::fixup_aie2p_33 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/28, /*Size=*/11}},
         {AIE2P::fixup_aie2p_34 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/32, /*Size=*/11}},
         {AIE2P::fixup_aie2p_35 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/69, /*Size=*/11}},
         {AIE2P::fixup_aie2p_36 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/44, /*Size=*/11}},
         {AIE2P::fixup_aie2p_37 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/48, /*Size=*/11}},
         {AIE2P::fixup_aie2p_38 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/65, /*Size=*/11}},
         {AIE2P::fixup_aie2p_39 /*(FormatSize=14, Flag=Unrestricted)*/}},
        {{{/*Offset=*/64, /*Size=*/11}},
         {AIE2P::fixup_aie2p_40 /*(FormatSize=14, Flag=Unrestricted)*/}},
        {{{/*Offset=*/58, /*Size=*/20}, {/*Offset=*/85, /*Size=*/12}},
         {AIE2P::fixup_aie2p_41 /*(FormatSize=16, Flag=Unrestricted)*/}},
        {{{/*Offset=*/0, /*Size=*/20}, {/*Offset=*/27, /*Size=*/12}},
         {AIE2P::fixup_aie2p_42 /*(FormatSize=6, Flag=Unrestricted)*/}},
        {{{/*Offset=*/26, /*Size=*/20}, {/*Offset=*/53, /*Size=*/12}},
         {AIE2P::fixup_aie2p_43 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/5, /*Size=*/20}, {/*Offset=*/32, /*Size=*/12}},
         {AIE2P::fixup_aie2p_44 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/42, /*Size=*/20}, {/*Offset=*/69, /*Size=*/12}},
         {AIE2P::fixup_aie2p_45 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/21, /*Size=*/20}, {/*Offset=*/48, /*Size=*/12}},
         {AIE2P::fixup_aie2p_46 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/17, /*Size=*/20}, {/*Offset=*/44, /*Size=*/12}},
         {AIE2P::fixup_aie2p_47 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/37, /*Size=*/20}, {/*Offset=*/64, /*Size=*/12}},
         {AIE2P::fixup_aie2p_48 /*(FormatSize=14, Flag=Unrestricted)*/}},
        {{{/*Offset=*/90, /*Size=*/8}},
         {AIE2P::fixup_aie2p_49 /*(FormatSize=16, Flag=Unrestricted)*/}},
        {{{/*Offset=*/17, /*Size=*/8}},
         {AIE2P::fixup_aie2p_50 /*(FormatSize=4, Flag=Unrestricted)*/,
          AIE2P::fixup_aie2p_53 /*(FormatSize=8, Flag=Unrestricted)*/}},
        {{{/*Offset=*/32, /*Size=*/8}},
         {AIE2P::fixup_aie2p_51 /*(FormatSize=6, Flag=Unrestricted)*/}},
        {{{/*Offset=*/42, /*Size=*/8}},
         {AIE2P::fixup_aie2p_52 /*(FormatSize=8, Flag=Unrestricted)*/}},
        {{{/*Offset=*/58, /*Size=*/8}},
         {AIE2P::fixup_aie2p_54 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/33, /*Size=*/8}},
         {AIE2P::fixup_aie2p_55 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/37, /*Size=*/8}},
         {AIE2P::fixup_aie2p_56 /*(FormatSize=10, Flag=Unrestricted)*/}},
        {{{/*Offset=*/74, /*Size=*/8}},
         {AIE2P::fixup_aie2p_57 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/49, /*Size=*/8}},
         {AIE2P::fixup_aie2p_58 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/53, /*Size=*/8}},
         {AIE2P::fixup_aie2p_59 /*(FormatSize=12, Flag=Unrestricted)*/}},
        {{{/*Offset=*/70, /*Size=*/8}},
         {AIE2P::fixup_aie2p_60 /*(FormatSize=14, Flag=Unrestricted)*/}},
        {{{/*Offset=*/69, /*Size=*/8}},
         {AIE2P::fixup_aie2p_61 /*(FormatSize=14, Flag=Unrestricted)*/}}};

/// Size of the (instruction) Format targeted by each AIE2P fixups
const std::map<unsigned, unsigned> AIE2PFixupFormatSize = {
    {AIE2P::fixup_aie2p_0, 16},  {AIE2P::fixup_aie2p_1, 6},
    {AIE2P::fixup_aie2p_2, 10},  {AIE2P::fixup_aie2p_3, 10},
    {AIE2P::fixup_aie2p_4, 12},  {AIE2P::fixup_aie2p_5, 12},
    {AIE2P::fixup_aie2p_6, 12},  {AIE2P::fixup_aie2p_7, 14},
    {AIE2P::fixup_aie2p_8, 16},  {AIE2P::fixup_aie2p_9, 4},
    {AIE2P::fixup_aie2p_10, 6},  {AIE2P::fixup_aie2p_11, 8},
    {AIE2P::fixup_aie2p_12, 10}, {AIE2P::fixup_aie2p_13, 12},
    {AIE2P::fixup_aie2p_14, 14}, {AIE2P::fixup_aie2p_15, 16},
    {AIE2P::fixup_aie2p_16, 4},  {AIE2P::fixup_aie2p_17, 6},
    {AIE2P::fixup_aie2p_18, 6},  {AIE2P::fixup_aie2p_19, 8},
    {AIE2P::fixup_aie2p_20, 8},  {AIE2P::fixup_aie2p_21, 10},
    {AIE2P::fixup_aie2p_22, 10}, {AIE2P::fixup_aie2p_23, 10},
    {AIE2P::fixup_aie2p_24, 12}, {AIE2P::fixup_aie2p_25, 12},
    {AIE2P::fixup_aie2p_26, 12}, {AIE2P::fixup_aie2p_27, 14},
    {AIE2P::fixup_aie2p_28, 16}, {AIE2P::fixup_aie2p_29, 4},
    {AIE2P::fixup_aie2p_30, 6},  {AIE2P::fixup_aie2p_31, 8},
    {AIE2P::fixup_aie2p_32, 8},  {AIE2P::fixup_aie2p_33, 10},
    {AIE2P::fixup_aie2p_34, 10}, {AIE2P::fixup_aie2p_35, 10},
    {AIE2P::fixup_aie2p_36, 12}, {AIE2P::fixup_aie2p_37, 12},
    {AIE2P::fixup_aie2p_38, 12}, {AIE2P::fixup_aie2p_39, 14},
    {AIE2P::fixup_aie2p_40, 14}, {AIE2P::fixup_aie2p_41, 16},
    {AIE2P::fixup_aie2p_42, 6},  {AIE2P::fixup_aie2p_43, 10},
    {AIE2P::fixup_aie2p_44, 10}, {AIE2P::fixup_aie2p_45, 12},
    {AIE2P::fixup_aie2p_46, 12}, {AIE2P::fixup_aie2p_47, 12},
    {AIE2P::fixup_aie2p_48, 14}, {AIE2P::fixup_aie2p_49, 16},
    {AIE2P::fixup_aie2p_50, 4},  {AIE2P::fixup_aie2p_51, 6},
    {AIE2P::fixup_aie2p_52, 8},  {AIE2P::fixup_aie2p_53, 8},
    {AIE2P::fixup_aie2p_54, 10}, {AIE2P::fixup_aie2p_55, 10},
    {AIE2P::fixup_aie2p_56, 10}, {AIE2P::fixup_aie2p_57, 12},
    {AIE2P::fixup_aie2p_58, 12}, {AIE2P::fixup_aie2p_59, 12},
    {AIE2P::fixup_aie2p_60, 14}, {AIE2P::fixup_aie2p_61, 14}};

/// Map between each supported fixup and a flag indicating the signedness
const std::map<unsigned, FixupFlag> AIE2PFixupFlagMap = {
    {AIE2P::fixup_aie2p_0, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_1, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_2, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_3, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_4, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_5, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_6, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_7, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_8, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_9, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_10, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_11, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_12, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_13, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_14, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_15, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_16, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_17, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_18, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_19, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_20, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_21, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_22, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_23, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_24, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_25, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_26, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_27, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_28, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_29, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_30, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_31, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_32, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_33, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_34, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_35, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_36, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_37, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_38, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_39, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_40, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_41, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_42, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_43, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_44, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_45, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_46, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_47, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_48, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_49, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_50, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_51, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_52, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_53, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_54, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_55, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_56, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_57, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_58, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_59, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_60, FixupFlag::Unrestricted},
    {AIE2P::fixup_aie2p_61, FixupFlag::Unrestricted}};

std::unique_ptr<AIEMCFixupKinds> llvm::createAIE2PMCFixupKinds() {
  return std::make_unique<AIEMCFixupKinds>(
      AIE2PFixupFieldsInfos, AIE2PFixupFieldsMapper, AIE2PFixupFormatSize,
      AIE2PFixupFlagMap, AIE2PInstrFixupFlags);
}

#endif // GET_MCFIXUPKINDS_IMPLEM

#ifdef RELOCS_DEFINITIONS
#undef RELOCS_DEFINITIONS

// The following declarations must be copied in a separate file at
// llvm/include/llvm/BinaryFormat/ELFRelocs/AIE2P.def
#ifndef ELF_RELOC
#error "ELF_RELOC must be defined"
#endif

ELF_RELOC(R_AIE2P_0, 0)
ELF_RELOC(R_AIE2P_1, 1)
ELF_RELOC(R_AIE2P_2, 2)
ELF_RELOC(R_AIE2P_3, 3)
ELF_RELOC(R_AIE2P_4, 4)
ELF_RELOC(R_AIE2P_5, 5)
ELF_RELOC(R_AIE2P_6, 6)
ELF_RELOC(R_AIE2P_7, 7)
ELF_RELOC(R_AIE2P_8, 8)
ELF_RELOC(R_AIE2P_9, 9)
ELF_RELOC(R_AIE2P_10, 10)
ELF_RELOC(R_AIE2P_11, 11)
ELF_RELOC(R_AIE2P_12, 12)
ELF_RELOC(R_AIE2P_13, 13)
ELF_RELOC(R_AIE2P_14, 14)
ELF_RELOC(R_AIE2P_15, 15)
ELF_RELOC(R_AIE2P_16, 16)
ELF_RELOC(R_AIE2P_17, 17)
ELF_RELOC(R_AIE2P_18, 18)
ELF_RELOC(R_AIE2P_19, 19)
ELF_RELOC(R_AIE2P_20, 20)
ELF_RELOC(R_AIE2P_21, 21)
ELF_RELOC(R_AIE2P_22, 22)
ELF_RELOC(R_AIE2P_23, 23)
ELF_RELOC(R_AIE2P_24, 24)
ELF_RELOC(R_AIE2P_25, 25)
ELF_RELOC(R_AIE2P_26, 26)
ELF_RELOC(R_AIE2P_27, 27)
ELF_RELOC(R_AIE2P_28, 28)
ELF_RELOC(R_AIE2P_29, 29)
ELF_RELOC(R_AIE2P_30, 30)
ELF_RELOC(R_AIE2P_31, 31)
ELF_RELOC(R_AIE2P_32, 32)
ELF_RELOC(R_AIE2P_33, 33)
ELF_RELOC(R_AIE2P_34, 34)
ELF_RELOC(R_AIE2P_35, 35)
ELF_RELOC(R_AIE2P_36, 36)
ELF_RELOC(R_AIE2P_37, 37)
ELF_RELOC(R_AIE2P_38, 38)
ELF_RELOC(R_AIE2P_39, 39)
ELF_RELOC(R_AIE2P_40, 40)
ELF_RELOC(R_AIE2P_41, 41)
ELF_RELOC(R_AIE2P_42, 42)
ELF_RELOC(R_AIE2P_43, 43)
ELF_RELOC(R_AIE2P_44, 44)
ELF_RELOC(R_AIE2P_45, 45)
ELF_RELOC(R_AIE2P_46, 46)
ELF_RELOC(R_AIE2P_47, 47)
ELF_RELOC(R_AIE2P_48, 48)
ELF_RELOC(R_AIE2P_49, 49)
ELF_RELOC(R_AIE2P_50, 50)
ELF_RELOC(R_AIE2P_51, 51)
ELF_RELOC(R_AIE2P_52, 52)
ELF_RELOC(R_AIE2P_53, 53)
ELF_RELOC(R_AIE2P_54, 54)
ELF_RELOC(R_AIE2P_55, 55)
ELF_RELOC(R_AIE2P_56, 56)
ELF_RELOC(R_AIE2P_57, 57)
ELF_RELOC(R_AIE2P_58, 58)
ELF_RELOC(R_AIE2P_59, 59)
ELF_RELOC(R_AIE2P_60, 60)
ELF_RELOC(R_AIE2P_61, 61)
ELF_RELOC(R_AIE2P_62, 62)
ELF_RELOC(R_AIE2P_63, 63)
ELF_RELOC(R_AIE2P_64, 64)
ELF_RELOC(R_AIE2P_65, 65)

#endif // RELOCS_DEFINITIONS
