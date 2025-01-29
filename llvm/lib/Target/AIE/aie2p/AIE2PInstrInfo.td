//===-- AIE2PInstrInfo.td - Target Description for AIE2p --*- tablegen --*-==//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file describes the AIE2p instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// AIE2p specific DAG Nodes.
//===----------------------------------------------------------------------===//

include "AIEBaseInstrInfo.td"
include "aie2p/AIE2PInstrFormats.td"
include "aie2p/AIE2PCompositeFormats.td"
include "aie2p/AIE2PImmOperands.td"

// An operand for target addresses. A simple imm20 cannot be used as it isn't
// compatible with the `bb` SDNode, which prevents writing patterns.
def cpmaddr : Operand<OtherVT> {
  let MCOperandPredicate = [{
    int64_t Imm;
    if (MCOp.evaluateAsConstantImm(Imm))
      return isUInt<20>(Imm);
    return MCOp.isBareSymbolRef();
  }];
}

include "AIE2PRegOperandDef.td"
include "aie2p/AIE2PGenInstrInfo.td"

let isMeta = true, hasSideEffects = 0, mayLoad = 0, mayStore = 0 in {
  def DelayedSchedBarrier : Pseudo<(outs), (ins)>;
}

// Pessimistically assume the stack pointer will be clobbered
let Defs = [sp], Uses = [sp],
    hasSideEffects=false, mayLoad=false, mayStore=false in {
  def ADJCALLSTACKUP   : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2)>;
  def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2)>;
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
  def PseudoJL : Pseudo<(outs), (ins c20u:$cpmaddr)>,
      PreSchedInstExpansion<JL_lng, DelayedSchedBarrier>;
  def PseudoJL_IND : Pseudo<(outs), (ins eP:$mPm)>,
      PreSchedInstExpansion<JL_alumv_or, DelayedSchedBarrier>;
}

//NOP
let hasSideEffects = 0, mayLoad = 0, mayStore = 0, isSlotNOP = true in {
  def NOP : AIE2P_nop_nop_inst_nop<(outs), (ins), "nop">;
}

let hasNoSchedulingInfo = true, hasSideEffects = true, mayLoad = false, mayStore = false, isConvergent = true, isMeta = true in {
  def SCHED_BARRIER : Pseudo<(outs), (ins)>;
}

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
  def LoopStart : Pseudo<(outs), (ins eR:$src), "loop_start", "${src}">;

  // PseudoLoopEnd represents the terminator of a ZOL. You can view it as a
  // conditional branch to the loop body. We make this a meta instruction,
  // since it doesn't need to be emitted itself, it only carries info for
  // its predecessor bundle
  let isBranch = true,  isTerminator = true, isMeta = true in
    def PseudoLoopEnd : Pseudo<(outs), (ins cpmaddr:$lastInstr, cpmaddr:$target),
      "pseudo_loop_end", "${lastInstr}, ${target}">;
}
// Pseudo Copy
// Target specific impl. of isCopyInstrImp causes the MOVE to be deleted, the Pseudo is
// kind of work around so that its not deleted
let Itinerary = II_MOV_alu_mv_mv_mv_scl, hasSideEffects = 0,  mayLoad = 0, mayStore = 0 in {
  def PseudoMove :
      Pseudo<(outs OP_mMvSclDst:$mMvSclDst), (ins OP_mMvSclSrc:$mMvSclSrc), "copy","$mMvSclDst, $mMvSclSrc">;
}

let isMeta = true, hasSideEffects = 0, mayLoad = 0, mayStore = 0 in {
  def CYCLE_SEPARATOR : Pseudo<(outs), (ins)>;
}

// Pseudo instructions for branches. Those are used by ISel and carry the right
// attributes for optimizations to work. They get expanded right before post-RA
// scheduling to target instructions with encoding information.
let hasDelaySlot = true, isBranch = true, isTerminator = true,
    hasSideEffects = false, mayLoad = 0, mayStore = 0 in {
  def PseudoJZ : Pseudo<(outs), (ins eR:$mRx, cpmaddr:$cpmaddr)>,
      PreSchedInstExpansion<JZ, DelayedSchedBarrier>;
  def PseudoJNZ : Pseudo<(outs), (ins eR:$mRx, cpmaddr:$cpmaddr)>,
      PreSchedInstExpansion<JNZ, DelayedSchedBarrier>;

  let isIndirectBranch = true, Defs = [srCarry] in
  def PseudoJNZD : Pseudo<(outs eR:$mRx), (ins eR:$mRx0, eP:$mPm)>,
      PreSchedInstExpansion<JNZD, DelayedSchedBarrier>;

  let isBarrier = true in {
    def PseudoJ_jump_imm : Pseudo<(outs), (ins cpmaddr:$cpmaddr)>,
        PreSchedInstExpansion<J_lng, DelayedSchedBarrier>;
    def PseudoJ_jump_ind : Pseudo<(outs), (ins eP:$mPm)>,
        PreSchedInstExpansion<J_alumv_or, DelayedSchedBarrier>;
  }
}

// Define _split variants for instructions using 2D registers
class Split2DInstr<Instruction RealInst, int opidx> : SplitPseudo<RealInst,
    opidx, (ins eM:$mod, eDN:$dim_size, eDJ:$dim_stride, eDC:$dim_count)> {}
foreach instr = [PADDA_2D, PADDB_2D, PADDS_2D] in
    def instr # _split : Split2DInstr<instr, /*opidx=*/3>;

// Define _split variants for instructions using 3D registers
class Split3DInstr<Instruction RealInst, int opidx> : SplitPseudo<RealInst,
    opidx, (ins eM:$mod1, eDN:$dim_size1, eDJ:$dim_stride1, eDC:$dim_count1,
                eM:$mod2, eDN:$dim_size2, eDJ:$dim_stride2, eDC:$dim_count2)> {}
foreach instr = [PADDA_3D, PADDB_3D, PADDS_3D] in
    def instr # _split : Split3DInstr<instr, /*opidx=*/4>;

let hasSideEffects = 0, Uses = [sp] in {
let mayLoad = false, mayStore = true in {
// Although it is ideal to avoid the use of ST_R_SPILL and LDA_R_SPILL,
// these instructions are included to simplify handling in certain
// scenarios. For example, they help eliminate the need for custom handling
// in places like eliminateFrameIndex. By including these instructions, we
// can streamline the code and improve readability.
def ST_R_SPILL : Pseudo<(outs ), (ins OP_mSclSt:$src, c12n_step4:$imm), "vst_r_spill", "$src, [sp, $imm]">;
def VST_L_SPILL : Pseudo<(outs ), (ins eL:$src, c12n_step4:$imm), "vst_l_spill", "$src, [sp, $imm]">;
def VST_Y_SPILL : Pseudo<(outs ), (ins VEC1024:$src, c16n_step64:$imm), "vst_y_spill", "${src}, [sp, $imm]">;
def VST_CM_SPILL : Pseudo<(outs ), (ins ACC1024:$src, c16n_step64:$imm), "vst_cm_spill", "${src}, [sp, $imm]">;
def VST_DM_SPILL : Pseudo<(outs ), (ins ACC2048:$src, c16n_step64:$imm), "vst_dm_spill", "${src}, [sp, $imm]">;
def ST_D_SPILL : Pseudo<(outs ), (ins eD:$src, c12n_step4:$imm), "st_d_spill", "$src, [sp, $imm]">;
def ST_DS_SPILL : Pseudo<(outs ), (ins eDS:$src, c12n_step4:$imm), "st_ds_spill", "$src, [sp, $imm]">;
def VST_FIFO_SPILL : Pseudo<(outs ), (ins FIFO1024:$src, c16n_step64:$imm), "vst_fifo_spill", "${src}, [sp, $imm]">;
def VST_PLFR_SPILL : Pseudo<(outs ), (ins ePSRFLdF:$src, c16n_step64:$imm), "vst_plfr_spill", "${src}, [sp, $imm]">;
}

let mayLoad = true, mayStore = false in {
def LDA_R_SPILL : Pseudo<(outs OP_mLdaScl:$dst), (ins c12n_step4:$imm), "vlda_r_spill", "$dst, [sp, $imm]">;
def VLDA_L_SPILL : Pseudo<(outs eL:$dst), (ins c12n_step4:$imm), "vlda_l_spill", "$dst, [sp, $imm]">;
def VLDA_Y_SPILL : Pseudo<(outs VEC1024:$dst), (ins c16n_step64:$imm), "vlda_y_spill", "${dst}, [sp, $imm]">;
def VLDA_CM_SPILL : Pseudo<(outs ACC1024:$dst), (ins c16n_step64:$imm), "vlda_cm_spill", "${dst}, [sp, $imm]">;
def VLDA_FIFO_SPILL : Pseudo<(outs FIFO1024:$dst), (ins c16n_step64:$imm), "vlda_fifo_spill", "${dst}, [sp, $imm]">;
def VLDA_PLFR_SPILL : Pseudo<(outs ePSRFLdF:$dst), (ins c16n_step64:$imm), "vlda_plfr_spill", "${dst}, [sp, $imm]">;
def VLDA_DM_SPILL : Pseudo<(outs ACC2048:$dst), (ins c16n_step64:$imm), "vlda_dm_spill", "${dst}, [sp, $imm]">;
def LDA_D_SPILL : Pseudo<(outs eD:$dst), (ins c12n_step4:$imm), "lda_d_spill", "${dst}, [sp, $imm]">;
def LDA_DS_SPILL : Pseudo<(outs eDS:$dst), (ins c12n_step4:$imm), "lda_ds_spill", "${dst}, [sp, $imm]">;
}
}

include "aie2p/AIE2PMultiSlotPseudoInstrInfo.td"
include "aie2p/AIE2PInstrPatterns.td"
