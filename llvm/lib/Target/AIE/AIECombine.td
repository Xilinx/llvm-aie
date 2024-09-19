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
  let CombineAllMethodName = "tryCombineAllImpl";
}

def AIE2PostLegalizerGenericCombiner
    : GICombiner<"AIE2PostLegalizerGenericCombinerImpl", [ all_combines ]> {
}

def combine_extract_concat_matchdata: GIDefMatchData<"Register">;
def combine_extract_concat : GICombineRule<
  (defs root:$root, combine_extract_concat_matchdata:$matchinfo),
  (match (wip_match_opcode G_INTRINSIC): $root,
  [{ return matchExtractConcat(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ applyExtractConcat(*${root}, MRI, B, ${matchinfo}); }])
>;

def combine_unmerge_concat_matchdata: GIDefMatchData<"std::pair<MachineInstr *, unsigned>">;
def combine_unmerge_concat : GICombineRule<
  (defs root:$root, combine_unmerge_concat_matchdata:$matchinfo),
  (match (wip_match_opcode G_UNMERGE_VALUES): $root,
  [{ return matchUnmergeConcat(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ applyUnmergeConcat(*${root}, MRI, B, ${matchinfo}); }])
>;

def combine_upd_to_concat_matchdata: GIDefMatchData<"std::map<unsigned, Register>">;
def combine_upd_to_concat : GICombineRule<
  (defs root:$root, combine_upd_to_concat_matchdata:$matchinfo),
  (match (wip_match_opcode G_INTRINSIC): $root,
  [{ return matchUpdToConcat(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ applyUpdToConcat(*${root}, MRI, B, ${matchinfo}); }])
>;

def load_store_increment_matchdata : GIDefMatchData<"AIELoadStoreCombineMatchData">;
def combine_load_store_increment : GICombineRule <
  (defs root:$root, load_store_increment_matchdata:$matchinfo),
  (match (wip_match_opcode G_LOAD, G_ZEXTLOAD, G_SEXTLOAD, G_STORE):$root,
          [{ return matchLdStInc(*${root}, MRI, ${matchinfo}, Helper, B.getTII()); }]),
  (apply [{ applyLdStInc(*${root}, MRI, B, ${matchinfo}, Observer); }] )
>;

def combine_add_vector_elt_undef : GICombineRule <
  (defs root:$root),
  (match (wip_match_opcode G_AIE_ADD_VECTOR_ELT_HI):$root,
          [{ return matchAddVecEltUndef(*${root}, MRI); }]),
  (apply [{ applyAddVecEltUndef(*${root}, MRI, B); }] )
>;

def combine_load_store_split_matchdata: GIDefMatchData<"unsigned">;
def combine_load_store_split : GICombineRule<
  (defs root:$root, combine_load_store_split_matchdata:$matchinfo),
  (match (wip_match_opcode G_LOAD, G_STORE): $root,
  [{ return matchLoadStoreSplit(cast<GLoadStore>(*${root}), MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ applyLoadStoreSplit(cast<GLoadStore>(*${root}), MRI, B, ${matchinfo}); }])
>;

def combine_offset_load_store_ptradd_matchdata: GIDefMatchData<"std::pair<Register, int64_t>">;
def combine_offset_load_store_ptradd : GICombineRule<
  (defs root:$root, combine_offset_load_store_ptradd_matchdata:$matchinfo),
  (match (wip_match_opcode G_AIE_OFFSET_LOAD, G_AIE_OFFSET_STORE): $root,
  [{ return matchOffsetLoadStorePtrAdd(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ applyOffsetLoadStorePtrAdd(*${root}, MRI, B, ${matchinfo}); }])
>;

def combine_offset_load_store_share_ptradd_matchdata: GIDefMatchData<"Register">;
def combine_offset_load_store_share_ptradd : GICombineRule<
  (defs root:$root, combine_offset_load_store_share_ptradd_matchdata:$matchinfo),
  (match (wip_match_opcode G_AIE_OFFSET_LOAD, G_AIE_OFFSET_STORE): $root,
  [{ return matchOffsetLoadStoreSharePtrAdd(*${root}, MRI, Helper, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ applyOffsetLoadStoreSharePtrAdd(*${root}, MRI, B, ${matchinfo}); }])
>;

def AIE2PostLegalizerCustomCombiner
    : GICombiner<"AIE2PostLegalizerCustomCombinerImpl", [ combine_load_store_split,
                                                          ptr_add_immed_chain,
                                                          combine_load_store_increment,
                                                          combine_offset_load_store_ptradd,
                                                          combine_offset_load_store_share_ptradd,
                                                          combine_add_vector_elt_undef,
                                                          combine_extract_concat,
                                                          combine_unmerge_concat,
                                                          combine_upd_to_concat 
                                                          ]> {
}
