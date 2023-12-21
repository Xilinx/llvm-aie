//===- AIECombine.td ---------------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
include "llvm/Target/GlobalISel/Combine.td"

// AIE-specific offset folding for G_GLOBAL_VALUE.
def combine_globalval_offset_matchdata : GIDefMatchData<"uint64_t">;
def combine_globalval_offset : GICombineRule<
  (defs root:$root, combine_globalval_offset_matchdata:$matchinfo),
  (match (wip_match_opcode G_GLOBAL_VALUE):$root,
          [{ return matchGlobalValOffset(*${root}, MRI, ${matchinfo}); }]),
  (apply [{  applyGlobalValOffset(*${root}, MRI, B, Observer, ${matchinfo});}])
>;

def combine_S20NarrowingOpt_instruction_queue : GIDefMatchData<"std::set<InstrNode>">;
def combine_S20NarrowingOpt : GICombineRule<
  (defs root:$root, combine_S20NarrowingOpt_instruction_queue:$matchinfo),
  (match (wip_match_opcode G_PTR_ADD, G_INTRINSIC):$root,
          [{ return matchS20NarrowingOpt(*${root}, MRI, ${matchinfo}); }]),
  (apply [{  applyS20NarrowingOpt(*${root}, MRI, B, Observer, Helper, ${matchinfo});}])
>;

def combine_extract_vector_elt_and_zsa_ext_matchdata: GIDefMatchData<"std::pair<MachineInstr *, bool>">;
def combine_extract_vector_elt_and_zsa_ext : GICombineRule<
  (defs root:$root, combine_extract_vector_elt_and_zsa_ext_matchdata:$matchinfo),
  (match (wip_match_opcode G_EXTRACT_VECTOR_ELT): $root,
  [{ return matchExtractVecEltAndExt(*${root}, MRI, ${matchinfo}); }]),
  (apply [{ applyExtractVecEltAndExt(*${root}, MRI, B, ${matchinfo}); }])
>;

def combine_splat_vector_matchdata: GIDefMatchData<"std::pair<Register, Register>">;
def combine_splat_vector : GICombineRule<
  (defs root:$root, combine_splat_vector_matchdata:$matchinfo),
  (match (wip_match_opcode G_BUILD_VECTOR): $root,
  [{ return matchSplatVector(*${root}, MRI, ${matchinfo}); }]),
  (apply [{ applySplatVector(*${root}, MRI, B, ${matchinfo}); }])
>;

def combine_pad_vector_matchdata: GIDefMatchData<"Register">;
def combine_pad_vector : GICombineRule<
  (defs root:$root, combine_pad_vector_matchdata:$matchinfo),
  (match (wip_match_opcode G_BUILD_VECTOR): $root,
  [{ return matchPadVector(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ applyPadVector(*${root}, MRI, B, ${matchinfo}); }])
>;

def combine_unpad_vector : GICombineRule<
  (defs root:$root),
  (match (wip_match_opcode G_UNMERGE_VALUES): $root,
  [{ return matchUnpadVector(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII()); }]),
  (apply [{ applyUnpadVector(*${root}, MRI, B); }])
>;

def AIE2PreLegalizerCombiner
    : GICombiner<"AIE2PreLegalizerCombinerImpl", [ combine_unpad_vector, combine_pad_vector,
                                                                 all_combines, combine_S20NarrowingOpt,
                                                                 combine_globalval_offset,
                                                                 combine_extract_vector_elt_and_zsa_ext,
                                                                 combine_splat_vector ]> {
}

def AIE2PostLegalizerGenericCombiner
    : GICombiner<"AIE2PostLegalizerGenericCombinerImpl", [ all_combines ]> {
}

def load_store_increment_matchdata : GIDefMatchData<"AIELoadStoreCombineMatchData">;
def combine_load_store_increment : GICombineRule <
  (defs root:$root, load_store_increment_matchdata:$matchinfo),
  (match (wip_match_opcode G_LOAD, G_ZEXTLOAD, G_SEXTLOAD, G_STORE):$root,
          [{ return matchLdStInc(*${root}, MRI, ${matchinfo}, Helper, B.getTII()); }]),
  (apply [{ applyLdStInc(*${root}, MRI, B, ${matchinfo}, Observer); }] )
>;

def AIE2PostLegalizerCustomCombiner
    : GICombiner<"AIE2PostLegalizerCustomCombinerImpl", [ combine_load_store_increment ]> {
}
