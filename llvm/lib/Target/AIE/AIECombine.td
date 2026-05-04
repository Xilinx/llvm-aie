//===- AIECombine.td ---------------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
include "llvm/Target/GlobalISel/Combine.td"

// Explicitly listing each generic combine here ensures direct visibility and
// control over all functionalities.
def aie_generic_combines : GICombineGroup<[trivial_combines, vector_ops_combines,
  insert_vec_elt_combines, extract_vec_elt_combines, combines_for_extload,
  combine_extracted_vector_load,
  undef_combines, identity_combines, phi_combines,
  simplify_add_to_sub, hoist_logic_op_with_same_opcode_hands, shifts_too_big,
  reassocs, ptr_add_immed_chain,
  shl_ashr_to_sext_inreg, sext_inreg_of_load,
  width_reduction_combines, select_combines,
  known_bits_simplifications, cast_combines,
  not_cmp_fold, opt_brcond_by_inverting_cond,
  trunc_shift, merge_combines,
  const_combines, xor_of_and_with_same_reg, ptr_add_with_zero,
  shift_immed_chain, shift_of_shifted_logic_chain, load_or_combine,
  div_rem_to_divrem, funnel_shift_combines, commute_shift,
  form_bitfield_extract, constant_fold_binops, constant_fold_fma,
  constant_fold_cast_op, fabs_fneg_fold,
  intdiv_combines, mulh_combines, redundant_neg_operands,
  and_or_disjoint_mask, fma_combines, fold_binop_into_select,
  sub_add_reg, select_to_minmax, cmp_combines,
  fsub_to_fneg, commute_constant_to_rhs, match_ands, match_ors,
  match_addos, combine_shuffle_concat]>;

// AIE-specific offset folding for G_GLOBAL_VALUE.
def combine_globalval_offset_matchdata : GIDefMatchData<"uint64_t">;
def combine_globalval_offset : GICombineRule<
  (defs root:$root, combine_globalval_offset_matchdata:$matchinfo),
  (match (wip_match_opcode G_GLOBAL_VALUE):$root,
          [{ return matchGlobalValOffset(*${root}, MRI, ${matchinfo}); }]),
  (apply [{  applyGlobalValOffset(*${root}, MRI, B, Observer, ${matchinfo});}])
>;

def combine_extract_vector_elt_and_zsa_ext_matchdata: GIDefMatchData<"std::pair<MachineInstr *, bool>">;
def combine_extract_vector_elt_and_zsa_ext : GICombineRule<
  (defs root:$root, combine_extract_vector_elt_and_zsa_ext_matchdata:$matchinfo),
  (match (wip_match_opcode G_EXTRACT_VECTOR_ELT): $root,
  [{ return matchExtractVecEltAndExt(*${root}, MRI, ${matchinfo}); }]),
  (apply [{ applyExtractVecEltAndExt(*${root}, MRI, B, ${matchinfo}, Observer); }])
>;

def combine_extract_vector_assert_combine : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_AIE_ZEXT_EXTRACT_VECTOR_ELT, G_AIE_SEXT_EXTRACT_VECTOR_ELT): $root,
  [{ return matchExtractVecEltAssertBcst(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), Observer, ${matchinfo}); }]),
   (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])>;

def combine_extract_broadcast_to_scalar : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_AIE_ZEXT_EXTRACT_VECTOR_ELT, G_AIE_SEXT_EXTRACT_VECTOR_ELT): $root,
  [{ return matchExtractBroadcastToScalar(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])>;

def combine_symmetric_build_vector : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_BUILD_VECTOR): $root,
  [{ return matchSymmetricBuildVector(*${root}, MRI, Observer, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])
>;

def combine_alternating_build_vector_matchdata: GIDefMatchData<"AIEAlternatingBuildVectorMatchData">;
def combine_alternating_build_vector : GICombineRule<
  (defs root:$root, combine_alternating_build_vector_matchdata:$matchinfo),
  (match (wip_match_opcode G_BUILD_VECTOR): $root,
  [{ return matchAlternatingBuildVector(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ applyAlternatingBuildVector(*${root}, MRI, B, ${matchinfo}, Observer); }])
>;

def combine_split_intrinsic_for_store : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_INTRINSIC_W_SIDE_EFFECTS, G_INTRINSIC): $root,
  [{ return matchSplitIntrinsicForStore(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])
>;

def combine_splat_vector_matchdata: GIDefMatchData<"std::pair<Register, Register>">;
def combine_splat_vector : GICombineRule<
  (defs root:$root, combine_splat_vector_matchdata:$matchinfo),
  (match (wip_match_opcode G_BUILD_VECTOR): $root,
  [{ return matchSplatVector(*${root}, MRI, ${matchinfo}); }]),
  (apply [{ applySplatVector(*${root}, MRI, B, ${matchinfo}, Observer); }])
>;

def combine_single_diff_build_vector_matchdata: GIDefMatchData<"AIESingleDiffLaneBuildVectorMatchData">;
def combine_single_diff_build_vector : GICombineRule<
  (defs root:$root, combine_single_diff_build_vector_matchdata:$matchinfo),
  (match (wip_match_opcode G_BUILD_VECTOR): $root,
  [{ return matchSingleDiffLaneBuildVector(*${root}, MRI, ${matchinfo}); }]),
  (apply [{ applySingleDiffLaneBuildVector(*${root}, MRI, B, ${matchinfo}, Observer); }])
>;

def combine_pad_vector_matchdata: GIDefMatchData<"Register">;
def combine_pad_vector : GICombineRule<
  (defs root:$root, combine_pad_vector_matchdata:$matchinfo),
  (match (wip_match_opcode G_BUILD_VECTOR): $root,
  [{ return matchPadVector(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ applyPadVector(*${root}, MRI, B, ${matchinfo}, Observer); }])
>;

def combine_concat_to_pad_vector : GICombineRule<
  (defs root:$root, combine_pad_vector_matchdata:$matchinfo),
  (match (wip_match_opcode G_CONCAT_VECTORS): $root,
  [{ return matchConcatPadVector(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ applyPadVector(*${root}, MRI, B, ${matchinfo}, Observer); }])
>;

def combine_unpad_vector : GICombineRule<
  (defs root:$root),
  (match (wip_match_opcode G_UNMERGE_VALUES): $root,
  [{ return matchUnpadVector(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII()); }]),
  (apply [{ applyUnpadVector(*${root}, MRI, B, Observer); }])
>;

def combine_vshift_chain_to_copy : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_AIE_VSHIFT_RIGHT): $root,
  [{ return matchVShiftChainToCopy(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])
>;

def combine_pad_unpad_to_copy : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_AIE_UNPAD_VECTOR): $root,
  [{ return matchPadUnpadToCopy(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])
>;

def combine_unpad_pad_to_copy : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_AIE_PAD_VECTOR_UNDEF): $root,
  [{ return matchUnpadPadToCopy(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])
>;

def combine_unpad_unmerge : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_AIE_UNPAD_VECTOR): $root,
  [{ return matchUnpadUnmerge(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), Helper, Observer, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])
>;

def combine_pad_unpad_fusion : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_AIE_UNPAD_VECTOR): $root,
  [{ return matchPadUnpadFusion(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])
>;

def combine_pad_pad_fusion : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_AIE_PAD_VECTOR_UNDEF): $root,
  [{ return matchPadPadFusion(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])
>;

def combine_unpad_unpad_fusion : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_AIE_UNPAD_VECTOR): $root,
  [{ return matchUnpadUnpadFusion(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])
>;

def combine_concat_unpad_fusion : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_AIE_UNPAD_VECTOR): $root,
  [{ return matchConcatUnpadFusion(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])
>;

def combine_flatten_nested_concat : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_CONCAT_VECTORS): $root,
  [{ return matchFlattenNestedConcat(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])
>;

def combine_vector_broadcast : GICombineRule<
  (defs root:$root, combine_splat_vector_matchdata:$matchinfo),
  (match (wip_match_opcode G_SHUFFLE_VECTOR): $root,
         [{ return matchBroadcastElement(*${root}, MRI, ${matchinfo}); }]),
  (apply [{ applySplatVector(*${root}, MRI, B, ${matchinfo}, Observer); }])>;

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

def combine_mostly_sequential_shuffle_with_insertions : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_SHUFFLE_VECTOR): $root,
         [{ return matchMostlySequentialShuffleWithInsertions(*${root}, MRI, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])>;

def combine_vector_shuffle_bcst_to_copy : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_SHUFFLE_VECTOR): $root,
         [{ return matchShuffleBcstToCopy(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])>;

def combine_paired_extracts : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_EXTRACT_VECTOR_ELT): $root,
         [{ return matchPairedExtracts(*${root}, MRI, Helper, (const AIEBaseInstrInfo &)B.getTII(), Observer, ${matchinfo}); }]),
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

// Transform XOR with MSB-only broadcast constant to ADD
// XOR(vec, broadcast(MSB_constant)) -> ADD(vec, broadcast(MSB_constant))
// Valid because XOR with MSB toggles sign bit, equivalent to ADD for MSB-only values
def combine_xor_msb_broadcast_to_add : GICombineRule<
  (defs root:$dst),
  (match (G_AIE_BROADCAST_VECTOR $bcst, $scalar),
         (G_XOR $dst, $vec, $bcst):$root,
         [{ return matchMsbScalar(${scalar}.getReg(), ${bcst}.getReg(), MRI); }]),
  (apply (G_ADD $dst, $vec, $bcst))>;

def combine_narrow_phi_node_s20 : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_PHI): $root,
         [{ return matchNarrowPhi(*${root}, MRI, Helper, Observer, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])>;

def combine_narrow_trunc_s20_const : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_TRUNC): $root,
         [{ return matchNarrowTruncConstant(*${root}, MRI, Observer, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])>;

def combine_narrow_zext_s20 : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_ZEXT): $root,
         [{ return matchNarrowZext(*${root}, MRI, Observer, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])>;

def combine_widen_fmul : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_FMUL): $root,
         [{ return matchWidenFMul(*${root}, MRI, Observer, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])>;

def concat_unmerge_matchdata : GIDefMatchData<"AIEConcatUnmergeCombineMatchData">;
def combine_concat_unmerge_phis : GICombineRule <
  (defs root:$root, concat_unmerge_matchdata:$matchinfo),
  (match (wip_match_opcode G_CONCAT_VECTORS):$root,
  [{return matchConcatUnmergePhis(*${root}, MRI, Helper, ${matchinfo});}]),
  (apply [{ applyConcatUnmergePhis(*${root}, MRI, B,${matchinfo}, Observer);}])
>;

// Fold trunc ([asz]ext x) -> x or ([asz]ext x) or (trunc x).
// NOTE: we have similar upstream combiner, but with this
// one we have more freedom to accept multiple uses of the
// result of the EXT operation.
def combine_trunc_ext: GICombineRule <
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_TRUNC):$root,
         [{ return matchCombineExtAndTrunc(*${root}, MRI, ${matchinfo}); }]),
  (apply [{Helper.applyBuildFn(*${root}, ${matchinfo}); }])
>;

def combine_load_const : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_LOAD): $root,
         [{ return matchConstLoad(*${root}, MRI, Observer, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])>;

def combine_bitcast_unmerge_swap : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_UNMERGE_VALUES): $root,
         [{ return matchBitcastUnmerge(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), Observer, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])>;

def combine_phi_bitcast_swap : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_PHI): $root,
         [{ return matchPhiBitcast(*${root}, MRI, Helper, (const AIEBaseInstrInfo &)B.getTII(), Observer, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])>;

def combine_vsel_to_unmerge_concat : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_AIE_VSEL): $root,
  [{ return matchVSelToUnmergeConcatOrCopy(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])
>;

def combine_phi_undef : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_PHI): $root,
         [{ return matchPhiOfUndef(*${root}, MRI, Observer, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])>;

def combine_align_memset : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_MEMSET): $root,
         [{ return matchAlignMemset(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), Observer, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])>;

def combine_peel_memset : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_MEMSET): $root,
         [{ return matchPeelMemset(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), Observer, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])>;

def combine_pack_stores_into_memset : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_STORE): $root,
         [{ return matchSequentialStores(cast<GStore>(*${root}), MRI, Observer, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])>;

def combine_split_concat_store : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_STORE, G_AIE_OFFSET_STORE, G_AIE_POSTINC_STORE): $root,
         [{ return matchSplitConcatStore(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])>;

def combine_chained_ptradd_with_s20_nonconst : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_PTR_ADD): $root,
  [{ return matchChainedPtrAddWithNonConstOffsets(*${root}, MRI, Helper, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])>;

def combine_postincloadstore_ptradd_with_trunc : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_PTR_ADD): $root,
  [{ return matchPostIncLoadStorePtrAddWithTrunc(*${root}, MRI, Helper, (const AIEBaseInstrInfo &)B.getTII(), Observer, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])>;

def combine_trunc_load : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_LOAD): $root,
         [{ return matchNarrowTruncLoad(*${root}, MRI, Helper, Observer, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])>;

def combine_load_inttoptr_fold : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_LOAD): $root,
         [{ return matchLoadInttoptrFold(*${root}, MRI, Helper, Observer, ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFnNoErase(*${root}, ${matchinfo}); }])>;

// AIE-specifc combines (currently shared by AIE2 and AIE2P).
def aie_additional_combines : GICombineGroup<[
  combine_unpad_vector,
  combine_pad_vector,
  combine_pad_unpad_to_copy,
  combine_unpad_pad_to_copy,
  combine_pad_pad_fusion,
  combine_unpad_unpad_fusion,
  combine_narrow_phi_node_s20,
  combine_globalval_offset,
  combine_extract_vector_elt_and_zsa_ext,
  combine_splat_vector,
  combine_alternating_build_vector,
  combine_concat_to_pad_vector,
  combine_single_diff_build_vector,
  combine_symmetric_build_vector,
  combine_broadcast_shl_to_add,
  combine_xor_msb_broadcast_to_add,
  combine_vector_shuffle_extract_subvec,
  combine_narrow_trunc_s20_const,
  combine_narrow_zext_s20,
  combine_concat_unmerge_phis,
  combine_trunc_ext,
  combine_load_const,
  combine_phi_undef,
  combine_align_memset,
  combine_peel_memset,
  combine_pack_stores_into_memset,
  combine_trunc_load,
  combine_load_inttoptr_fold
]>;

// AIE2P-specific combines.
// TODO: Move HW-agnostic combines to `aie_additional_combines`.
def aie2p_additional_combines : GICombineGroup<[
  combine_vector_shuffle_to_copy,
  combine_vector_shuffle_bcst_to_copy,
  combine_vector_broadcast,
  combine_vector_shuffle_vsel,
  combine_vector_shuffle_broadcast,
  combine_mostly_sequential_shuffle_with_insertions,
  combine_vector_shuffle_concat_extracted_subvectors,
  combine_paired_extracts,
  combine_widen_fmul,
  combine_vector_shuffle_to_extract_insert_elt_to_broadcast,
  combine_bitcast_unmerge_swap,
  combine_phi_bitcast_swap,
  combine_vsel_to_unmerge_concat,
  combine_vshift_chain_to_copy,
  combine_unpad_unmerge,
  combine_pad_unpad_fusion,
  combine_concat_unpad_fusion,
  combine_flatten_nested_concat,
  combine_chained_ptradd_with_s20_nonconst
]>;

def AIE2PreLegalizerCombiner
    : GICombiner<"AIE2PreLegalizerCombinerImpl", [aie_generic_combines, aie_additional_combines]> {
  let CombineAllMethodName = "tryCombineAllImpl";
}

def AIE2PPreLegalizerCombiner
    : GICombiner<"AIE2PPreLegalizerCombinerImpl",
                      [aie_generic_combines, aie_additional_combines, aie2p_additional_combines]> {
  let CombineAllMethodName = "tryCombineAllImpl";
}

def AIE2PostLegalizerGenericCombiner
    : GICombiner<"AIE2PostLegalizerGenericCombinerImpl", [aie_generic_combines]> {
}

def AIE2PPostLegalizerGenericCombiner
    : GICombiner<"AIE2PPostLegalizerGenericCombinerImpl", [aie_generic_combines]> {
}

def combine_extract_concat_matchdata: GIDefMatchData<"Register">;
def combine_extract_concat : GICombineRule<
  (defs root:$root, combine_extract_concat_matchdata:$matchinfo),
  (match (wip_match_opcode G_INTRINSIC): $root,
  [{ return matchExtractConcat(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ applyExtractConcat(*${root}, MRI, B, ${matchinfo}, Observer); }])
>;

def combine_unmerge_concat_matchdata: GIDefMatchData<"std::pair<MachineInstr *, unsigned>">;
def combine_unmerge_concat : GICombineRule<
  (defs root:$root, combine_unmerge_concat_matchdata:$matchinfo),
  (match (wip_match_opcode G_UNMERGE_VALUES): $root,
  [{ return matchUnmergeConcat(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ applyUnmergeConcat(*${root}, MRI, B, ${matchinfo}, Observer); }])
>;

def combine_upd_to_concat_matchdata: GIDefMatchData<"std::map<unsigned, Register>">;
def combine_upd_to_concat : GICombineRule<
  (defs root:$root, combine_upd_to_concat_matchdata:$matchinfo),
  (match (wip_match_opcode G_INTRINSIC): $root,
  [{ return matchUpdToConcat(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ applyUpdToConcat(*${root}, MRI, B, ${matchinfo}, Observer); }])
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
  (apply [{ applyAddVecEltUndef(*${root}, MRI, B, Observer); }] )
>;

def combine_insert_extract_vector_elt_to_copy : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_AIE_INSERT_VECTOR_ELT): $root,
  [{ return matchInsertExtractVectorEltToCopy(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])
>;

def combine_broadcast_extract_to_copy : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_AIE_BROADCAST_VECTOR): $root,
  [{ return matchBroadcastExtractToCopy(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])
>;

def combine_load_store_split_matchdata: GIDefMatchData<"unsigned">;
def combine_load_store_split : GICombineRule<
  (defs root:$root, combine_load_store_split_matchdata:$matchinfo),
  (match (wip_match_opcode G_LOAD, G_STORE): $root,
  [{ return matchLoadStoreSplit(cast<GLoadStore>(*${root}), MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ applyLoadStoreSplit(cast<GLoadStore>(*${root}), MRI, B, ${matchinfo}, Observer); }])
>;

def combine_offset_load_store_ptradd_matchdata: GIDefMatchData<"std::pair<Register, int64_t>">;
def combine_offset_load_store_ptradd : GICombineRule<
  (defs root:$root, combine_offset_load_store_ptradd_matchdata:$matchinfo),
  (match (wip_match_opcode G_AIE_OFFSET_LOAD, G_AIE_OFFSET_STORE): $root,
  [{ return matchOffsetLoadStorePtrAdd(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ applyOffsetLoadStorePtrAdd(*${root}, MRI, B, ${matchinfo}, Observer); }])
>;

def combine_offset_load_store_share_ptradd_matchdata: GIDefMatchData<"Register">;
def combine_offset_load_store_share_ptradd : GICombineRule<
  (defs root:$root, combine_offset_load_store_share_ptradd_matchdata:$matchinfo),
  (match (wip_match_opcode G_AIE_OFFSET_LOAD, G_AIE_OFFSET_STORE): $root,
  [{ return matchOffsetLoadStoreSharePtrAdd(*${root}, MRI, Helper, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ applyOffsetLoadStoreSharePtrAdd(*${root}, MRI, B, ${matchinfo}, Observer); }])
>;

def combine_cse_vector_ops_matchdata: GIDefMatchData<"Register">;
def combine_cse_vector_ops : GICombineRule<
  (defs root:$root, combine_cse_vector_ops_matchdata:$matchinfo),
  (match (wip_match_opcode G_CONCAT_VECTORS):$root,
         [{ return matchCSEVectorOp(*${root}, MRI, Helper, ${matchinfo}); }]),
  (apply [{ 
    B.setInstrAndDebugLoc(*${root});
    B.buildCopy(${root}->getOperand(0).getReg(), ${matchinfo});
    Observer.erasingInstr(*${root});
    ${root}->eraseFromParent();
  }])
>;

// Eliminate redundant widen <-> narrow conversion pairs.
// Pattern: narrow -> wide -> narrow becomes COPY
// Note: This optimization applies to exact conversions (no rounding occurs),
// making the round-trip conversion (narrow -> wide -> narrow) a no-op that can
// be safely eliminated. Examples include bf16 <-> accfloat conversions.
def combine_redundant_widen_narrow_conversion : GICombineRule<
  (defs root:$root, build_fn_matchinfo:$matchinfo),
  (match (wip_match_opcode G_INTRINSIC_W_SIDE_EFFECTS):$root,
         [{ return matchRedundantWidenNarrowConversion(*${root}, MRI, (const AIEBaseInstrInfo &)B.getTII(), ${matchinfo}); }]),
  (apply [{ Helper.applyBuildFn(*${root}, ${matchinfo}); }])
>;

def combine_global_load_store_increment : GICombineRule <
  (defs root:$root),
  (match (wip_match_opcode G_LOAD, G_ZEXTLOAD, G_SEXTLOAD, G_STORE):$root,
          [{ return matchGlobalPtrModOptimizer(*${root}, MRI, Helper, B.getTII(), GlobalCombiners); }]),
  (apply [{ applyLdStInc(*${root}, MRI, Helper, B, Observer, GlobalCombiners); }] )>;



// Post-legalizer custom combines: shared base plus per-target additions.
def aie_postlegalizer_custom_shared_combines : GICombineGroup<[
  combine_broadcast_extract_to_copy,
  combine_global_load_store_increment,
  combine_load_store_increment,
  ptr_add_immed_chain,
  combine_offset_load_store_ptradd,
  combine_offset_load_store_share_ptradd,
  combine_add_vector_elt_undef,
  combine_insert_extract_vector_elt_to_copy,
  combine_split_concat_store
]>;

def aie2_postlegalizer_custom_combines : GICombineGroup<[
  combine_load_store_split,
  combine_extract_concat,
  combine_unmerge_concat,
  combine_upd_to_concat
]>;

// Post-legalizer custom combines for AIE2P and more recent targets (AIE2PS, etc.).
def aie2p_plus_postlegalizer_custom_shared_combines : GICombineGroup<[
  combine_extract_vector_assert_combine,
  combine_extract_broadcast_to_scalar,
  combine_vshift_chain_to_copy,
  combine_pad_unpad_to_copy,
  combine_unpad_pad_to_copy,
  combine_cse_vector_ops,
  combine_redundant_widen_narrow_conversion
]>;

// AIE2PS-specific final combines (runs after custom combiner to avoid conflicts).
def aie2ps_postlegalizer_final_combines : GICombineGroup<[
  combine_chained_ptradd_with_s20_nonconst,
  combine_postincloadstore_ptradd_with_trunc
]>;

def AIE2PostLegalizerCustomCombiner
    : GICombiner<"AIE2PostLegalizerCustomCombinerImpl",
                 [aie_postlegalizer_custom_shared_combines, aie2_postlegalizer_custom_combines]> {
}

def AIE2PPostLegalizerCustomCombiner
    : GICombiner<"AIE2PPostLegalizerCustomCombinerImpl",
                 [aie_postlegalizer_custom_shared_combines, aie2p_plus_postlegalizer_custom_shared_combines]> {
}

// AIE2PS-specific pre-legalizer combines
def aie2ps_prelegalizer_additional_combines : GICombineGroup<[
  combine_split_intrinsic_for_store
]>;

def AIE2PSPreLegalizerCombiner
    : GICombiner<"AIE2PSPreLegalizerCombinerImpl",
                      [aie_generic_combines, aie_additional_combines, aie2p_additional_combines, aie2ps_prelegalizer_additional_combines]> {
  let CombineAllMethodName = "tryCombineAllImpl";
}

def AIE2PSPostLegalizerGenericCombiner
    : GICombiner<"AIE2PSPostLegalizerGenericCombinerImpl", [aie_generic_combines]> {
}

def AIE2PSPostLegalizerCustomCombiner
    : GICombiner<"AIE2PSPostLegalizerCustomCombinerImpl",
                 [aie_postlegalizer_custom_shared_combines, aie2p_plus_postlegalizer_custom_shared_combines]> {
}

def AIE2PSPostLegalizerFinalCombiner
    : GICombiner<"AIE2PSPostLegalizerFinalCombinerImpl",
                 [aie2ps_postlegalizer_final_combines]> {
}

def combine_copy_of_implicit_def : GICombineRule<
  (defs root:$root),
  (match (wip_match_opcode COPY):$root,
         [{ return matchCopyOfImplicitDef(*${root}, MRI); }]),
  (apply [{ applyCopyOfImplicitDef(*${root}, MRI, B, Observer); }])
>;

def AIEPreISelCombiner
    : GICombiner<"AIEPreISelCombinerImpl",
                 [combine_copy_of_implicit_def]> {
}
