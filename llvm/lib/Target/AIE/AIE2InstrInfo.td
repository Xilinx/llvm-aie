//===--AIE2InstrInfo.td -Target Description for AIEngine V2 -*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file describes the AIEngine V2 instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// AIEngine V2 specific DAG Nodes.
//===----------------------------------------------------------------------===//

include "AIEBaseInstrInfo.td"

def t02u : txxu<2, i32>;
def imm2 : txxu<2,i32>;
def imm3 : txxu<3,i32>;
def imm4 : txxu<4,i32>;
def imm5 : txxu<5,i32>;
def imm6 : txxu<6,i32>;
def imm20 : txxu<20,i32>;
def imm10 : txxu<10,i32>;

def simm4 : txxs<4,i32>;
def simm3 : txxs<3,i32>;
def simm6 : txxs<6,i32>;
def simm7 : txxs<7,i32>;
def simm7_negated : txxsn<7,i32>;
def simm10 : txxs<10,i32>;
def simm11 : txxs<11,i32>;
def simm32 : txxs<32,i32>;


// Immediate for stack adjustments, twelve bits scaled by 32
def imm12x32 : immx32<12>;
// Immediate for stack adjustments, thirteen bits scaled by 32
def imm13x32 : immx32<13>;
// Immediate for stack spills, twelve bits scaled by 4
def imm12x4 : immx4<12, i32>;
// Immediate for stack spills, twelve bits scaled by 32
def imm12x32_neg : immx32_negative<12>;
// w17n_step16 (17 bit   signed min=-65536  max=-16   step=16);
// Immediate for stack spills (128-bit addressing modes)
def imm12x16_neg : immx16_negative<12>;

// Immediate for indexed are mapped to i20, for encoding/decoding constraints,
// the types are vastly irrelevant. However, if we want to use those predicates
// for ISel patterns, types are relevant.
// Immediate for indexed addressing, six bits scaled by 4
def imm6x4 : immx4<6, i20>;
// Immediate for indexed addressing, three bits scaled by 32
def imm3x32 : immx32<3, i20>;
// Immediate for indexed addressing, six bits scaled by 32
def imm6x32 : immx32<6, i20>;
// Immediate for post-increment addressing, four bits scaled by 32
def imm4x32 : immx32<4, i20>;
// Immediate for post-increment addressing, seven bits scaled by 4
def imm7x4 : immx4<7, i20>;
// Immediate for post-increment addressing, seven bits scaled by 32
def imm7x32 : immx32<7, i20>;
// Immediate for post-increment addressing, nine bits scaled by 4
def imm9x4 : immx4<9, i20>;
// Immediate for indexed addressing, ten bits scaled by 4
def imm10x4 : immx4<10, i20>;
// w10_step16 (10 bit   signed step=16),
// Immediate for ag_idx_imm(128-bit addressing modes)
def imm6x16 : immx16<6, i20>;
// w11_step16 (11 bit  signed step=16),
// Immediate pstm_nrm_imm (128-bit addressing modes)
def imm7x16 : immx16<7>;

// An operand for target addresses. A simple imm20 cannot be used as it isn't
// compatible with the `bb` SDNode, which prevents writing patterns.
def addr20 : Operand<OtherVT> {
  let MCOperandPredicate = [{
    int64_t Imm;
    if (MCOp.evaluateAsConstantImm(Imm))
      return isUInt<20>(Imm);
    return MCOp.isBareSymbolRef();
  }];
}

// Leaf coding classes. These are context-free, and inherited in several other classes
// Someone told me tablegen doesn't allow to use a template parameter for the width of
// a bitfield -- which is annoying.

// ptr, imm<n>
class AIE2_ag_ptr_imm3 {
  bits<3> ptr;
  bits<3> imm;
  bits<6> ag_ptr_imm = {ptr, imm};
}

class AIE2_ag_ptr_imm4 {
  bits<3> ptr;
  bits<4> imm;
  bits<7> ag_ptr_imm = {ptr, imm};
}

class AIE2_ag_ptr_imm6 {
  bits<3> ptr;
  bits<6> imm;
  bits<9> ag_ptr_imm = {ptr, imm};
}

class AIE2_ag_ptr_imm7 {
  bits<3> ptr;
  bits<7> imm;
  bits<10> ag_ptr_imm = {ptr, imm};
}

class AIE2_ag_ptr_imm10 {
  bits<3> ptr;
  bits<10> imm;
  bits<13> ag_ptr_imm = {ptr, imm};
}

class AIE2_agb_ptr_imm9 {
  bits<3> ptr;
  bits<9> imm;
  bits<14> ag_ptr_imm = {ptr, imm{8-3}, 0b11, imm{2-0}};
}

// ptr, modreg
class AIE2_ag_ptr_mod {
  bits<3> ptr;
  bits<3> mod;
  bits<6> ag_ptr_mod = {ptr, mod};
}

// ptr, djreg
class AIE2_ag_ptr_dj {
  bits<3> ptr;
  bits<3> dj;
  bits<6> ag_ptr_mod = {ptr, dj};
}

/////
// ag_all: addressing modes for the more common loads and stores.
class AIE2_ag_all {
  bits<13> ag_all = 0;
}

// *(ptr + dj)
class AIE2_ag_all__ag_idx : AIE2_ag_all, AIE2_ag_ptr_dj  {
  let ag_all = {ag_ptr_mod, 0b100, 0b0010};
}

// *(ptr + imm)
class AIE2_ag_all__ag_idx_imm : AIE2_ag_all, AIE2_ag_ptr_imm6 {
  let ag_all = {ag_ptr_imm, 0b1010};
}

// Post-increment with modreg
// t = *ptr, ptr += m, t
class AIE2_ag_all__ag_pstm_nrm : AIE2_ag_all, AIE2_ag_ptr_mod {
  let ag_all = {ag_ptr_mod, 0b010, 0b0010};
}

// Post increment with immediate
// t = *ptr, ptr += imm, t;
class AIE2_ag_all__ag_pstm_nrm_imm : AIE2_ag_all, AIE2_ag_ptr_imm7 {
  let ag_all = {ag_ptr_imm, 0b110};
}

// ptr += add2d(d) (2D addressing)
class AIE2_ag_all__ag_pstm_2d : AIE2_ag_all, AIE2_ag_ptr_mod, AIE_HasTiedSubregister {
  let ag_all = {ag_ptr_mod,0b110,0b0010};
}

// ptr += d (3d addressing)
class AIE2_ag_all__ag_pstm_3d : AIE2_ag_all, AIE_HasTiedSubregister {
  bits<3> ptr;
  bits<2> mod;
  let ag_all = {ptr, 0b0, mod, 0b000, 0b0010};
}

// Stack pointer immediate
// *(sp + imm)
class AIE2_ag_all__ag_spill : AIE2_ag_all {
  bits<12> imm;
  let ag_all = {imm, 0b1};
}

/////
// ag_nospill: for sub-word loads and stores
class AIE2_ag_nospill {
  bits<9> ag_nospill = 0;
}

// *(ptr + dj)
class AIE2_ag_nospill__ag_idx : AIE2_ag_nospill, AIE2_ag_ptr_dj  {
  let ag_nospill = {ag_ptr_mod, 0b100};
}

// Unit "B"
// *(ptr + dj)
class AIE2_agb_nospill__ag_idx : AIE2_ag_nospill, AIE2_ag_ptr_dj  {
  let ag_nospill = {ag_ptr_mod, 0b000};
}

// *(ptr + imm)
class AIE2_ag_nospill__ag_idx_imm : AIE2_ag_nospill, AIE2_ag_ptr_imm3  {
  let ag_nospill = {ag_ptr_imm, 0b011};
}

// Post-increment with modreg
// t = *ptr, ptr += m, t
class AIE2_ag_nospill__ag_pstm_nrm : AIE2_ag_nospill, AIE2_ag_ptr_mod  {
  let ag_nospill = {ag_ptr_mod, 0b010};
}

// Post increment with immediate
// t = *ptr, ptr += imm, t;
class AIE2_ag_nospill__ag_pstm_nrm_imm : AIE2_ag_nospill, AIE2_ag_ptr_imm4  {
  let ag_nospill = {ag_ptr_imm, 0b01};
}

// 2D Post increment
// ptr += add2d(d)
class AIE2_ag_nospill__ag_pstm_2d : AIE2_ag_nospill, AIE2_ag_ptr_mod, AIE_HasTiedSubregister  {
  let ag_nospill = {ag_ptr_mod, 0b110};
}

// 3D Post increment
// ptr += d
class AIE2_ag_nospill__ag_pstm_3d : AIE2_ag_nospill, AIE_HasTiedSubregister {
  bits<3> ptr;
  bits<2> mod;
  let ag_nospill = {ptr, 0b0, mod, 0b000};
}

/////
// aga_sa used for pointer arithmetic
class AIE2_aga_sa {
  bits<19> aga_sa = 0;
}

// ags_sa used for pointer arithmetic
class AIE2_ags_sa {
  bits<19> ags_sa = 0;
}

class AIE2_ag_standalone : AIE2_aga_sa, AIE2_ags_sa {
  bits<19> ag_standalone;
  let aga_sa = ag_standalone;
  let ags_sa = ag_standalone;
}

// ptr += m
class AIE2_ag_standalone__ag_pstm_nrm : AIE2_ag_standalone, AIE2_ag_ptr_mod {
  field bits<2> dontcare2;
  field bits<4> dontcare4;
  let ag_standalone = {ag_ptr_mod,0b010,dontcare4,0b11,dontcare2,0b01};
}

// ptr += imm
class AIE2_ag_standalone__ag_pstm_nrm_imm : AIE2_ag_standalone, AIE2_ag_ptr_imm10 {
  field bits<2> dontcare2;
  let ag_standalone = {ag_ptr_imm,0b11,dontcare2,0b11};
}

// Unit "B"
// agb_sa used for pointer arithmetic
class AIE2_agb_sa {
  bits<16> agb_sa = 0;
}

class AIE2_agb_standalone : AIE2_agb_sa {
  bits<16> agb_standalone;
  let agb_sa = agb_standalone;
}

// ptr += m
class AIE2_agb_standalone__agb_nospill__ag_pstm_nrm : AIE2_agb_standalone, AIE2_ag_nospill__ag_pstm_nrm {
  field bits<3> dontcare3;
  let agb_standalone = {ag_nospill,0b11,dontcare3,0b01};
}

// ptr += imm
class AIE2_agb_standalone__ag_pstm_nrm_imm : AIE2_agb_standalone, AIE2_agb_ptr_imm9 {
  let agb_standalone = {ag_ptr_imm, 0b10};
}

// ptr += add2d(d) (2D addressing)
class AIE2_ag_standalone__ag_pstm_2d : AIE2_ag_standalone, AIE2_ag_ptr_mod, AIE_HasTiedSubregister  {
  field bits<2> dontcare2;
  field bits<4> dontcare4;
  let ag_standalone = {ag_ptr_mod,0b110,dontcare4,0b11,dontcare2,0b01};
}

// Unit "B"
class AIE2_agb_standalone__agb_nospill__ag_pstm_2d : AIE2_agb_standalone, AIE2_ag_ptr_mod, AIE_HasTiedSubregister {
  field bits<3> dontcare3;
  let agb_standalone = {ag_ptr_mod, 0b10011, dontcare3, 0b01};
}

// ptr += d (3D addressing)
class AIE2_ag_standalone__ag_pstm_3d : AIE2_ag_standalone, AIE_HasTiedSubregister {
  field bits<2> dontcare2;
  field bits<4> dontcare4;
  bits<3> ptr;
  bits<2> mod;
  let ag_standalone = {ptr,0b0,mod,0b000,dontcare4,0b11,dontcare2,0b01};
}

// Unit "B"
class AIE2_agb_standalone__agb_nospill__ag_pstm_3d : AIE2_agb_standalone, AIE_HasTiedSubregister {
  bits<3> ptr;
  bits<2> mod;
  field bits<3> dontcare3;
  let agb_standalone = {ptr, 0b0, mod, 0b11011, dontcare3, 0b01};
}

class AIE2_ag_standalone__ag_pstm_sp_imm : AIE2_ag_standalone {
  bits<13> imm;
  field bits<2> dontcare2;
  let ag_standalone = {imm,0b11,dontcare2,0b10};
}

class AIE2_agb_standalone__agb_pstm_sp_imm : AIE2_agb_standalone {
  bits<12> imm;
  let agb_standalone = {imm{11-3}, 0b11, imm{2-0}, 0b00};
}

/////
// Unit "B"
// ag_no_imm: for sub-word loads
class AIE2_ag_no_imm {
  bits<8> ag_no_imm = 0;
}

// *(ptr + dj)
class AIE2_ag_no_imm__ag_idx : AIE2_ag_no_imm, AIE2_ag_ptr_dj  {
  let ag_no_imm = {ag_ptr_mod, 0b00};
}

// Post-increment with modreg
// t = *ptr, ptr += m, t
class AIE2_ag_no_imm__ag_pstm_nrm : AIE2_ag_no_imm, AIE2_ag_ptr_mod  {
  let ag_no_imm = {ag_ptr_mod, 0b01};
}

// 2D Post increment
// ptr += add2d(d)
class AIE2_ag_no_imm__ag_pstm_2d : AIE2_ag_no_imm, AIE2_ag_ptr_mod, AIE_HasTiedSubregister  {
  let ag_no_imm = {ag_ptr_mod, 0b10};
}

// 3D Post increment
// ptr += d
class AIE2_ag_no_imm__ag_pstm_3d : AIE2_ag_no_imm, AIE_HasTiedSubregister {
  bits<3> ptr;
  bits<2> mod;
  let ag_no_imm = {ptr, 0b0, mod, 0b11};
}

include "AIE2RegOperandDef.td"
include "AIE2InstrFormats.td"
include "AIE2GenInstrInfo.td"
include "AIE2CompositeFormats.td"
// Manual fixes to the auto-generated files
include "AIE2GenFixupInstrInfo.td"
include "AIE2MultiSlotPseudoInstrInfo.td"

//Intrinsics
let Itinerary = II_EVENT, hasSideEffects = 1, mayLoad = 0, mayStore = 0 in {
  def EVENT : AIE2_event_inst_alu<(outs), (ins t02u:$val), "event", "$val">;
}
def : Pat<(int_aie2_event t02u:$val), (EVENT t02u:$val)>;

let Itinerary = II_RET, hasDelaySlot = true, Uses = [lr],
    hasSideEffects = false, mayLoad = false, mayStore = false in {
  def RET : AIE2_jump_return_inst_alu <(outs), (ins), "ret lr", ""> ;
}

let isMeta = true, hasSideEffects = 0, mayLoad = 0, mayStore = 0 in {
  def DelayedSchedBarrier : Pseudo<(outs), (ins)>;
}

let isMeta = true, hasSideEffects = 0, mayLoad = 0, mayStore = 0 in {
  def CYCLE_SEPARATOR : Pseudo<(outs), (ins)>;
}

// Pseudo instructions for VLD.SPARSE to be expanded after RA. 
// They allow us to keep QW registers as non allocatable to avoid 
// spilling half mask registers.
let mayLoad = true, mayStore = true, hasSideEffects = true in {
  let Constraints = "$ptr_out = $ptr" in {
    class PSEUDO_VLD_SPARSE_OP_set_low : Pseudo<
      (outs eP:$ptr_out, SPARSEVEC640:$dst),
      (ins eP:$ptr)>;
  }
  let Constraints = "$ptr_out = $ptr, $dst = $src" in {
    class PSEUDO_VLD_SPARSE_OP_insert_hi : Pseudo<
      (outs eP:$ptr_out, SPARSEVEC640:$dst),
      (ins eP:$ptr, SPARSEVEC640:$src)>;
  }
  class PSEUDO_VLD_SPARSE_OP_and_get_pointer : Pseudo<
    (outs eDN0:$ptr_out, SPARSEVEC640:$dst),
    (ins eP:$ptr)>;
  class PSEUDO_VLD_SPARSE_INIT_and_get_pointer : Pseudo<
    (outs eDN0:$ptr_out), (ins eP:$ptr)>;

  def PSEUDO_VLD_SPARSE_PEEK_16_set_low : PSEUDO_VLD_SPARSE_OP_set_low;
  def PSEUDO_VLD_SPARSE_PEEK_16_insert_hi : PSEUDO_VLD_SPARSE_OP_insert_hi;
  def PSEUDO_VLD_SPARSE_PEEK_16_and_get_pointer : PSEUDO_VLD_SPARSE_OP_and_get_pointer;
  def PSEUDO_VLD_SPARSE_POP_16_set_low : PSEUDO_VLD_SPARSE_OP_set_low;
  def PSEUDO_VLD_SPARSE_POP_16_insert_hi : PSEUDO_VLD_SPARSE_OP_insert_hi;
  def PSEUDO_VLD_SPARSE_POP_16_and_get_pointer : PSEUDO_VLD_SPARSE_OP_and_get_pointer;

  def PSEUDO_VLD_SPARSE_PEEK_8_set_low : PSEUDO_VLD_SPARSE_OP_set_low;
  def PSEUDO_VLD_SPARSE_PEEK_8_insert_hi : PSEUDO_VLD_SPARSE_OP_insert_hi;
  def PSEUDO_VLD_SPARSE_PEEK_8_and_get_pointer : PSEUDO_VLD_SPARSE_OP_and_get_pointer;
  def PSEUDO_VLD_SPARSE_POP_8_set_low : PSEUDO_VLD_SPARSE_OP_set_low;
  def PSEUDO_VLD_SPARSE_POP_8_insert_hi : PSEUDO_VLD_SPARSE_OP_insert_hi;
  def PSEUDO_VLD_SPARSE_POP_8_and_get_pointer : PSEUDO_VLD_SPARSE_OP_and_get_pointer;

  def PSEUDO_VLD_SPARSE_PEEK_4_set_low : PSEUDO_VLD_SPARSE_OP_set_low;
  def PSEUDO_VLD_SPARSE_PEEK_4_insert_hi : PSEUDO_VLD_SPARSE_OP_insert_hi;
  def PSEUDO_VLD_SPARSE_PEEK_4_and_get_pointer : PSEUDO_VLD_SPARSE_OP_and_get_pointer;
  def PSEUDO_VLD_SPARSE_POP_4_set_low : PSEUDO_VLD_SPARSE_OP_set_low;
  def PSEUDO_VLD_SPARSE_POP_4_insert_hi : PSEUDO_VLD_SPARSE_OP_insert_hi;
  def PSEUDO_VLD_SPARSE_POP_4_and_get_pointer : PSEUDO_VLD_SPARSE_OP_and_get_pointer;

  def PSEUDO_VLD_SPARSE_RESET_16_and_get_pointer : PSEUDO_VLD_SPARSE_INIT_and_get_pointer;
  def PSEUDO_VLD_SPARSE_FILL_16_and_get_pointer : PSEUDO_VLD_SPARSE_INIT_and_get_pointer;
  def PSEUDO_VLD_SPARSE_RESET_8_and_get_pointer : PSEUDO_VLD_SPARSE_INIT_and_get_pointer;
  def PSEUDO_VLD_SPARSE_FILL_8_and_get_pointer : PSEUDO_VLD_SPARSE_INIT_and_get_pointer;
  def PSEUDO_VLD_SPARSE_RESET_4_and_get_pointer : PSEUDO_VLD_SPARSE_INIT_and_get_pointer;
  def PSEUDO_VLD_SPARSE_FILL_4_and_get_pointer : PSEUDO_VLD_SPARSE_INIT_and_get_pointer;
}

// Pseudo instructions for branches. Those are used by ISel and carry the right
// attributes for optimizations to work. They get expanded right before post-RA
// scheduling to target instructions with encoding information.
let hasDelaySlot = true, isBranch = true, isTerminator = true,
    hasSideEffects = false, mayLoad = 0, mayStore = 0 in {
  def PseudoJZ : Pseudo<(outs), (ins eR:$mRx, addr20:$cpmaddr)>,
      PreSchedInstExpansion<JZ, DelayedSchedBarrier>;
  def PseudoJNZ : Pseudo<(outs), (ins eR:$mRx, addr20:$cpmaddr)>,
      PreSchedInstExpansion<JNZ, DelayedSchedBarrier>;

  let isIndirectBranch = true, Defs = [srCarry] in
  def PseudoJNZD : Pseudo<(outs eR:$mRx), (ins eR:$mRx0, eP:$mPm)>,
      PreSchedInstExpansion<JNZD, DelayedSchedBarrier>;

  let isBarrier = true in {
    def PseudoJ_jump_imm : Pseudo<(outs), (ins addr20:$cpmaddr)>,
        PreSchedInstExpansion<J_jump_imm, DelayedSchedBarrier>;
    def PseudoJ_jump_ind : Pseudo<(outs), (ins eP:$mPm)>,
        PreSchedInstExpansion<J_jump_ind, DelayedSchedBarrier>;
  }
}

// Pre-scheduling pseudo instruction for RET
let Uses = [lr],
    hasDelaySlot = 1, isBarrier = 1, isReturn = 1, isTerminator = 1,
    hasSideEffects = false, mayLoad = 0, mayStore = 0 in
def PseudoRET : Pseudo<(outs), (ins)>,
                PreSchedInstExpansion<RET, DelayedSchedBarrier>;

// Pre-scheduling pseudo instructions for calls.
let Defs = [lr],
    hasDelaySlot = true, isCall = true, isTerminator = false,
    hasSideEffects = true, mayLoad = false, mayStore = false  in {
  def PseudoJL : Pseudo<(outs), (ins imm20:$cpmaddr)>,
      PreSchedInstExpansion<JL, DelayedSchedBarrier>;
  def PseudoJL_IND : Pseudo<(outs), (ins eP:$mPm)>,
      PreSchedInstExpansion<JL_IND, DelayedSchedBarrier>;
}

// Target-independent type requirements, but with target-specific formats.
def SDT_CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>,
                                       SDTCisVT<1, i32>]>;
def SDT_CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i32>,
                                     SDTCisVT<1, i32>]>;
// Target-independent nodes, but with target-specific formats.
def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_CallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_CallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;
// Pessimistically assume the stack pointer will be clobbered
let Defs = [SP], Uses = [SP],
    hasSideEffects=false, mayLoad=false, mayStore=false in {
  def ADJCALLSTACKUP   : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2)>;
  def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2)>;
}

// Placeholder for a bare frameindex. This pseudo represents the
// pointer register to be allocated, initialized with the address
// represented by the frameindex in its only operand.
let Uses = [SP] in 
def PseudoFI : Pseudo<(outs eP:$dst), (ins i32imm:$imm)>;

// NOPs for each VLIW Slot
let Itinerary = II_NOP, hasSideEffects = 0, mayLoad = 0, mayStore = 0, isSlotNOP = true in {
  def NOP : AIE2_nop_nop_inst_nop<(outs), (ins), "nop">;
  def NOPA : AIE2_nop_lda_inst_lda<(outs), (ins), "nopa">;
  def NOPB : AIE2_nop_ldb_inst_ldb<(outs), (ins), "nopb">;
  def NOPX : AIE2_nop_alu_inst_alu<(outs), (ins), "nopx">;
  def NOPM : AIE2_nop_mv_inst_mv<(outs), (ins), "nopm">;
  def NOPS : AIE2_nop_sts_inst_st<(outs), (ins), "nops">;
  def NOPV : AIE2_nop_vec_inst_vec<(outs), (ins), "nopv">;
  def NOPXM : AIE2_nop_lng_lng<(outs), (ins), "nopxm">;
}

// Spill instructions
let hasSideEffects = false, Uses = [SP] in {
let Itinerary = II_LDA, mayLoad = true, mayStore = false in
def LDA_dms_spill : AIE2_dms_lda_inst_lda<(outs OP_mLdaScl:$mLdaScl), (ins imm12x4:$imm),
    "lda", "$mLdaScl, [sp, $imm]">, 
  AIE2_ag_all__ag_spill;

let Itinerary = II_ST, mayLoad = false, mayStore = true in
def ST_dms_spill : AIE2_dms_sts_inst_st<(outs), (ins OP_mSclSt:$mSclSt, imm12x4:$imm),
    "st", "$mSclSt, [sp, $imm]">,
  AIE2_ag_all__ag_spill;

let mayLoad = true, mayStore = false in {
def VLDA_L_SPILL : Pseudo<(outs eL:$dst), (ins imm12x4:$imm), "vlda_l_spill", "$dst, [sp, $imm]">;
def VLDA_X_SPILL : Pseudo<(outs VEC512:$dst), (ins  imm12x32_neg:$imm), "vlda_x_spill", "${dst}, [sp, $imm]">;
def VLDA_QX_SPILL : Pseudo<(outs SPARSEVEC640:$dst), (ins  imm12x32_neg:$imm), "vlda_qx_spill", "${dst}, [sp, $imm]">;
def VLDA_Y_SPILL : Pseudo<(outs VEC1024:$dst), (ins imm12x32_neg:$imm), "vlda_y_spill", "${dst}, [sp, $imm]">;
def VLDA_BM_SPILL : Pseudo<(outs ACC512:$dst), (ins  imm12x32_neg:$imm), "vlda_bm_spill", "${dst}, [sp, $imm]">;
def VLDA_CM_SPILL : Pseudo<(outs ACC1024:$dst), (ins imm12x32_neg:$imm), "vlda_cm_spill", "${dst}, [sp, $imm]">;
def LDA_D_SPILL : Pseudo<(outs eD:$dst), (ins imm12x4:$imm), "lda_d_spill", "${dst}, [sp, $imm]">;
def LDA_DS_SPILL : Pseudo<(outs eDS:$dst), (ins imm12x4:$imm), "lda_ds_spill", "${dst}, [sp, $imm]">;
} // let mayLoad = true, mayStore = false

let mayLoad = false, mayStore = true in {
def VST_L_SPILL : Pseudo<(outs ), (ins eL:$src, imm12x4:$imm), "vst_l_spill", "$src, [sp, $imm]">;
def VST_X_SPILL : Pseudo<(outs ), (ins VEC512:$src,  imm12x32_neg:$imm), "vst_x_spill", "${src}, [sp, $imm]">;
def VST_QX_SPILL : Pseudo<(outs ), (ins SPARSEVEC640:$src,  imm12x32_neg:$imm), "vst_qx_spill", "${src}, [sp, $imm]">;
def VST_Y_SPILL : Pseudo<(outs ), (ins VEC1024:$src, imm12x32_neg:$imm), "vst_y_spill", "${src}, [sp, $imm]">;
def VST_BM_SPILL : Pseudo<(outs ), (ins ACC512:$src,  imm12x32_neg:$imm), "vst_bm_spill", "${src}, [sp, $imm]">;
def VST_CM_SPILL : Pseudo<(outs ), (ins ACC1024:$src, imm12x32_neg:$imm), "vst_cm_spill", "${src}, [sp, $imm]">;
def ST_D_SPILL : Pseudo<(outs ), (ins eD:$src, imm12x4:$imm), "st_d_spill", "$src, [sp, $imm]">;
def ST_DS_SPILL : Pseudo<(outs ), (ins eDS:$src, imm12x4:$imm), "st_ds_spill", "$src, [sp, $imm]">;
} // let mayLoad = true, mayStore = false

} // let hasSideEffects = false, Uses = [SP]

// Loop related Pseudos
// JNZD loops
let hasSideEffects = 0, mayLoad = 0, mayStore = 0, Defs = [srCarry] in
def LoopDec :
    Pseudo<(outs eR:$dst), (ins eR:$src), "loop_dec", "${dst}, ${src}">;

let hasSideEffects = 0,  mayLoad = 0, mayStore = 0, isBranch = 1, isIndirectBranch = 1, isTerminator = true in
def LoopJNZ :
    Pseudo<(outs), (ins eR:$tc, eP:$target), "loop_jnz", "${tc}, ${target}">;

// Zero overhead loops
let hasSideEffects = true, mayLoad = false, mayStore = false,
    isNotDuplicable = true in {
  def LoopStart : Pseudo<(outs), (ins eR:$src, simm6:$adj), "loop_start", "${src}, ${adj}">;

  // PseudoLoopEnd represents the terminator of a ZOL. You can view it as a
  // conditional branch to the loop body. We make this a meta instruction,
  // since it doesn't need to be emitted itself, it only carries info for
  // its predecessor bundle
  let isBranch = true,  isTerminator = true, isMeta = true in
    def PseudoLoopEnd : Pseudo<(outs), (ins addr20:$lastInstr, addr20:$target),
      "pseudo_loop_end", "${lastInstr}, ${target}">;
}

// Pseudo Copy
// Target specific impl. of isCopyInstrImp causes the MOVE to be deleted, the Pseudo is
// kind of work around so that its not deleted
let Itinerary = II_MOV_SCL, hasSideEffects = 0,  mayLoad = 0, mayStore = 0 in {
  def PseudoMove :
      Pseudo<(outs OP_mMvSclDst:$mMvSclDst), (ins OP_mMvSclSrc:$mMvSclSrc), "copy","$mMvSclDst, $mMvSclSrc">;
}

// Define _split variants for instructions using 2D registers
class Split2DInstr<Instruction RealInst, int opidx> : SplitPseudo<RealInst,
    opidx, (ins eM:$mod, eDN:$dim_size, eDJ:$dim_stride, eDC:$dim_count)> {}
foreach instr = [VLDA_2D_dmw_lda_w, VLDA_2D_dmw_lda_am, VLDA_2D_CONV_FP32_BF16,
                 VLDB_2D, VLDB_2D_128, LDA_2D_dmv_lda_q, VLDB_2D_UNPACK_S8_S4,
                 VLDB_2D_UNPACK_S16_S8, VLDB_2D_UNPACK_D8_D4, VLDB_2D_UNPACK_D16_D8,
                 VLD_2D_pseudo, VST_2D_dmw_sts_w, VST_2D_dmw_sts_am, VST_2D_128,
                 ST_2D_dmv_sts_q, LDA_2D_dms_lda, LDA_2D_S8_dmhb_lda, LDA_2D_U8_dmhb_lda,
                 LDA_2D_S16_dmhb_lda, LDA_2D_U16_dmhb_lda,
                 ST_2D_dms_sts, ST_2D_S8, ST_2D_S16] in
    def instr # _split : Split2DInstr<instr, /*opidx=*/4>;
foreach instr = [VLDA_2D_UPS_S32_D16, VLDA_2D_UPS_S64_D32, VLDA_2D_UPS_S32_D8,
                 VLDA_2D_UPS_S64_D16, VLDA_2D_UPS_S32_S16, VLDA_2D_UPS_S64_S32,
                 VLDA_2D_UPS_S32_S8, VLDA_2D_UPS_S64_S16] in
    def instr # _split : Split2DInstr<instr, /*opidx=*/5>;
foreach instr = [VST_2D_SRS_D8_S32, VST_2D_SRS_D16_S64, VST_2D_SRS_D16_S32,
                 VST_2D_SRS_D32_S64, VST_2D_SRS_S8_S32, VST_2D_SRS_S16_S64,
                 VST_2D_SRS_S16_S32, VST_2D_SRS_S32_S64, VST_CONV_2D_BF16_FP32,
                 VST_2D_PACK_D4_D8, VST_2D_PACK_D8_D16, VST_2D_PACK_S4_S8,
                 VST_2D_PACK_S8_S16,
                 PADDA_2D, PADDB_2D, PADDS_2D] in
    def instr # _split : Split2DInstr<instr, /*opidx=*/3>;

// Define _split variants for instructions using 3D registers
class Split3DInstr<Instruction RealInst, int opidx> : SplitPseudo<RealInst,
    opidx, (ins eM:$mod1, eDN:$dim_size1, eDJ:$dim_stride1, eDC:$dim_count1,
                eM:$mod2, eDN:$dim_size2, eDJ:$dim_stride2, eDC:$dim_count2)> {}
foreach instr = [VLDA_3D_dmw_lda_w, VLDA_3D_dmw_lda_am, VLDA_3D_CONV_FP32_BF16,
                 VLDB_3D, VLDB_3D_128, LDA_3D_dmv_lda_q, VLDB_3D_UNPACK_S8_S4,
                 VLDB_3D_UNPACK_S16_S8, VLDB_3D_UNPACK_D8_D4, VLDB_3D_UNPACK_D16_D8,
                 VLD_3D_pseudo, VST_3D_dmw_sts_w, VST_3D_dmw_sts_am, VST_3D_128, 
                 ST_3D_dmv_sts_q, LDA_3D_dms_lda, LDA_3D_S8_dmhb_lda, LDA_3D_U8_dmhb_lda,
                 LDA_3D_S16_dmhb_lda, LDA_3D_U16_dmhb_lda,
                 ST_3D_dms_sts, ST_3D_S8, ST_3D_S16] in
    def instr # _split : Split3DInstr<instr, /*opidx=*/5>;
foreach instr = [VLDA_3D_UPS_S32_D16, VLDA_3D_UPS_S64_D32, VLDA_3D_UPS_S32_D8,
                 VLDA_3D_UPS_S64_D16, VLDA_3D_UPS_S32_S16, VLDA_3D_UPS_S64_S32,
                 VLDA_3D_UPS_S32_S8, VLDA_3D_UPS_S64_S16] in
    def instr # _split : Split3DInstr<instr, /*opidx=*/6>;
foreach instr = [VST_3D_SRS_D8_S32, VST_3D_SRS_D16_S64, VST_3D_SRS_D16_S32,
                 VST_3D_SRS_D32_S64, VST_3D_SRS_S8_S32, VST_3D_SRS_S16_S64,
                 VST_3D_SRS_S16_S32, VST_3D_SRS_S32_S64, VST_CONV_3D_BF16_FP32,
                 VST_3D_PACK_D4_D8, VST_3D_PACK_D8_D16, VST_3D_PACK_S4_S8,
                 VST_3D_PACK_S8_S16,
                 PADDA_3D, PADDB_3D, PADDS_3D] in
    def instr # _split : Split3DInstr<instr, /*opidx=*/4>;

include "AIE2InstrPatterns.td"
