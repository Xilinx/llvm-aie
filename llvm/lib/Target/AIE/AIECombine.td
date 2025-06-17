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

// Explicitly listing each generic combine here ensures direct visibility and 
// control over all functionalities.
def aie_all_combines : GICombineGroup<[trivial_combines, vector_ops_combines,
  insert_vec_elt_combines, extract_vec_elt_combines, combines_for_extload,
  combine_extracted_vector_load,
  undef_combines, identity_combines, phi_combines,
  simplify_add_to_sub, hoist_logic_op_with_same_opcode_hands, shifts_too_big,
  reassocs, ptr_add_immed_chain,
  shl_ashr_to_sext_inreg, sext_inreg_of_load,
  width_reduction_combines, select_combines,
  known_bits_simplifications, ext_ext_fold,
  not_cmp_fold, opt_brcond_by_inverting_cond,
  unmerge_merge, unmerge_cst, unmerge_dead_to_trunc,
  unmerge_zext_to_zext, merge_unmerge, trunc_ext_fold, trunc_shift,
  const_combines, xor_of_and_with_same_reg, ptr_add_with_zero,
  shift_immed_chain, shift_of_shifted_logic_chain, load_or_combine,
  div_rem_to_divrem, funnel_shift_combines, commute_shift,
  form_bitfield_extract, constant_fold_binops, constant_fold_fma,
  constant_fold_cast_op, fabs_fneg_fold,
  intdiv_combines, mulh_combines, redundant_neg_operands,
  and_or_disjoint_mask, fma_combines, fold_binop_into_select,
  sub_add_reg, select_to_minmax, redundant_binop_in_equality,
  fsub_to_fneg, commute_constant_to_rhs, match_ands, match_ors,
  double_icmp_zero_and_or_combine, match_addos, combine_shuffle_concat]>;

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
  (match (wip_match_opcode G_PTR_ADD, G_INTRINSIC, G_INTRINSIC_W_SIDE_EFFECTS):$root,
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

def combine_symmetric_build_vector : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_BUILD_VECTOR): $root,
  [{ return matchSymmetricBuildVector(*${root}, MRI, Observer, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])
>;

def combine_splat_vector_matchdata: GIDefMatchData<"std::pair<Register, Register>">;
def combine_splat_vector : GICombineRule<
  (defs root:$root, combine_splat_vector_matchdata:$matchinfo),
  (match (wip_match_opcode G_BUILD_VECTOR): $root,
  [{ return matchSplatVector(*${root}, MRI, ${matchinfo}); }]),
  (apply [{ applySplatVector(*${root}, MRI, B, ${matchinfo}); }])
>;

def combine_single_diff_build_vector_matchdata: GIDefMatchData<"AIESingleDiffLaneBuildVectorMatchData">;
def combine_single_diff_build_vector : GICombineRule<
  (defs root:$root, combine_single_diff_build_vector_matchdata:$matchinfo),
  (match (wip_match_opcode G_BUILD_VECTOR): $root,
  [{ return matchSingleDiffLaneBuildVector(*${root}, MRI, ${matchinfo}); }]),
  (apply [{ applySingleDiffLaneBuildVector(*${root}, MRI, B, ${matchinfo}); }])
>;

def combine_pad_vector_matchdata: GIDefMatchData<"Register">;
def combine_pad_vector : GICombineRule<
  (defs root:$root, combine_pad_vector_matchdata:$matchinfo),
  (match (wip_match_opcode G_BUILD_VECTOR): $root,
  [{ return matchPadVector(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ applyPadVector(*${root}, MRI, B, ${matchinfo}); }])
>;

def combine_concat_to_pad_vector : GICombineRule<
  (defs root:$root, combine_pad_vector_matchdata:$matchinfo),
  (match (wip_match_opcode G_CONCAT_VECTORS): $root,
  [{ return matchConcatPadVector(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ applyPadVector(*${root}, MRI, B, ${matchinfo}); }])
>;

def combine_unpad_vector : GICombineRule<
  (defs root:$root),
  (match (wip_match_opcode G_UNMERGE_VALUES): $root,
  [{ return matchUnpadVector(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII()); }]),
  (apply [{ applyUnpadVector(*${root}, MRI, B); }])
>;

def combine_vector_broadcast : GICombineRule<
  (defs root:$root, combine_splat_vector_matchdata:$matchinfo),
  (match (wip_match_opcode G_SHUFFLE_VECTOR): $root,
         [{ return matchBroadcastElement(*${root}, MRI, ${matchinfo}); }]),
  (apply [{ applySplatVector(*${root}, MRI, B, ${matchinfo}); }])>;

def combine_vector_shuffle_broadcast : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_SHUFFLE_VECTOR): $root,
         [{ return matchShuffleToBroadcast(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])>;

def combine_vector_shuffle_vsel : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_SHUFFLE_VECTOR): $root,
         [{ return matchShuffleToVSel(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])>;

def combine_vector_shuffle_extract_subvec : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_SHUFFLE_VECTOR): $root,
         [{ return matchShuffleToExtractSubvec(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])>;

def combine_vector_shuffle_concat_extracted_subvectors : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_SHUFFLE_VECTOR): $root,
         [{ return matchShuffleToConcatExtractedSubvectors(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])>;

def combine_vector_shuffle_to_copy : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_SHUFFLE_VECTOR): $root,
         [{ return matchShuffleToCopy(*${root}, MRI, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])>;

def combine_vector_shuffle_to_extract_insert_elt : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_SHUFFLE_VECTOR): $root,
         [{ return matchShuffleToExtractInsertElt(*${root}, MRI, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])>;

def combine_vector_shuffle_bcst_to_copy : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_SHUFFLE_VECTOR): $root,
         [{ return matchShuffleBcstToCopy(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])>;

def combine_paired_extracts : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_EXTRACT_VECTOR_ELT): $root,
         [{ return matchPairedExtracts(*${root}, MRI, Helper, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])>;

def combine_vector_shuffle_to_extract_insert_elt_to_broadcast : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_SHUFFLE_VECTOR): $root,
         [{ return matchShuffleToExtractInsertEltToBroadcast(*${root}, MRI, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])>;

def combine_broadcast_shl_to_add : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_SHL): $root,
         [{ return matchBroadcastToShl(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])>;

def AIE2PreLegalizerCombiner
    : GICombiner<"AIE2PreLegalizerCombinerImpl", [ combine_unpad_vector, combine_pad_vector,
                                                                 aie_all_combines, combine_S20NarrowingOpt,
                                                                 combine_globalval_offset,
                                                                 combine_extract_vector_elt_and_zsa_ext,
                                                                 combine_splat_vector, combine_concat_to_pad_vector, 
                                                                 combine_single_diff_build_vector,
                                                                 combine_symmetric_build_vector,
                                                                 combine_broadcast_shl_to_add]> {
  let CombineAllMethodName = "tryCombineAllImpl";
}

def AIE2PPreLegalizerCombiner
    : GICombiner<"AIE2PPreLegalizerCombinerImpl", [ combine_unpad_vector, combine_pad_vector,
                                                                 combine_vector_shuffle_to_copy,
                                                                 combine_vector_shuffle_extract_subvec,
                                                                 aie_all_combines, combine_S20NarrowingOpt,
                                                                 combine_globalval_offset,
                                                                 combine_extract_vector_elt_and_zsa_ext,
                                                                 combine_splat_vector, 
                                                                 combine_vector_shuffle_bcst_to_copy,
                                                                 combine_vector_broadcast,
                                                                 combine_concat_to_pad_vector, 
                                                                 combine_vector_shuffle_vsel,
                                                                 combine_vector_shuffle_broadcast,
                                                                 combine_single_diff_build_vector,
                                                                 combine_vector_shuffle_to_extract_insert_elt,
                                                                 combine_vector_shuffle_concat_extracted_subvectors,
                                                                 combine_paired_extracts,
                                                                 combine_vector_shuffle_to_extract_insert_elt_to_broadcast,
                                                                 combine_symmetric_build_vector,
                                                                 combine_broadcast_shl_to_add]> {
  let CombineAllMethodName = "tryCombineAllImpl";
}

def AIE2PostLegalizerGenericCombiner
    : GICombiner<"AIE2PostLegalizerGenericCombinerImpl", [ aie_all_combines ]> {
}

def AIE2PPostLegalizerGenericCombiner
    : GICombiner<"AIE2PPostLegalizerGenericCombinerImpl", [ aie_all_combines ]> {
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

def combine_load_store_increment : GICombineRule <
  (defs root:$root),
  (match (wip_match_opcode G_LOAD, G_ZEXTLOAD, G_SEXTLOAD, G_STORE):$root,
          [{ return matchLdStInc(*${root}, MRI, Helper, B.getTII(), GlobalCombiners); }]),
  (apply [{ applyLdStInc(*${root}, MRI, Helper, B, Observer, GlobalCombiners); }] )
>;

def combine_add_vector_elt_undef : GICombineRule <
  (defs root:$root),
  (match (wip_match_opcode G_AIE_ADD_VECTOR_ELT_HI):$root,
          [{ return matchAddVecEltUndef(*${root}, MRI, B.getTII()); }]),
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

def AIE2PPostLegalizerCustomCombiner
    : GICombiner<"AIE2PPostLegalizerCustomCombinerImpl", [ combine_load_store_increment,
                                                           ptr_add_immed_chain, 
                                                           combine_offset_load_store_ptradd,
                                                           combine_offset_load_store_share_ptradd,
                                                           combine_add_vector_elt_undef ]> {
}
