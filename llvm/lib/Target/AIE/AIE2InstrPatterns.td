//===--AIE2InstrPatterns.td -ISel patterns for AIE2 --------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file describes instruction selection patterns for the AIEngine V2
// instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

// Defining a custom renderer seems to be the easiest way to negate immediates.
// Using a !sub "bang operantor" is overly complicated, requiring dag<->int
// conversions and some black magic.
def NegateImm : SDNodeXForm<imm, [{
  return CurDAG->getConstant(-N->getSExtValue(), SDLoc(N), MVT::i32);
}]>;
def gi_NegateImm : GICustomOperandRenderer<"renderNegateImm">,
  GISDNodeXFormEquiv<NegateImm>;

// 3.23 PADD – Pointer increments
// Pseudo uses predicate imm9x4, since this fits for both slot A and B.
def : Pat< (ptradd eP:$ptr, (i20 imm9x4:$imm)),
           (PADD_imm_pseudo eP:$ptr, imm9x4:$imm)>;
// PADDA allows 10 bits.
// Here negative complexity reduces the preference of PADDA
// when the imm fits in 9 bits (and hence 10 bits).
// In which case it will use the pseudo insn.
def : Pat< (ptradd eP:$ptr, (i20 imm10x4:$imm)),
           (PADDA_lda_ptr_inc_idx_imm eP:$ptr, imm10x4:$imm)> { let AddedComplexity = -1; }
def : Pat< (ptradd eP:$ptr, eM:$mod),
           (PADD_mod_pseudo eP:$ptr, eM:$mod)>;

/// Generic pattern classes
class PatGpr<SDPatternOperator OpNode, AIE2Inst Inst, ValueType type>
    : Pat<(type (OpNode (type eR:$rs))), (Inst eR:$rs)>;

class PatGprGpr<SDPatternOperator OpNode, AIE2Inst Inst, ValueType type>
    : Pat<(type (OpNode (type eR:$rs1), (type eR:$rs2))), (Inst eR:$rs1, eR:$rs2)>;

// 4.1.2 CLB/CLZ – Count leading bits/zeros
// CLZ is a standard llvm intrinsic
def : PatGpr<int_aie2_clb, CLB, i32>;
def : PatGpr<ctlz, CLZ, i32>;
def : PatGpr<ctlz_zero_undef, CLZ, i32>;


// 4.1.3 EXTEND – Sign extend / Zero Extend
// Note: Sign extensions are handled in AIE2InstructionSelector.cpp.
def : Pat<(i32 (and eR:$rs1, (i32 255))), (EXTENDu8 eR:$rs1)>;
def : Pat<(i32 (and eR:$rs1, (i32 65535))), (EXTENDu16 eR:$rs1)>;

multiclass ALUPatterns<ValueType type> {
// 4.1.5 ABS - Absolute Value
def : PatGpr<abs, ABS, type>;
// 4.1.6 ADD – Add Immediate
def : Pat<(type (add (type eR:$s0), simm7:$imm)), (ADD_add_r_ri eR:$s0, simm7:$imm)>;
def : Pat<(type (sub (type eR:$s0), simm7_negated:$imm)),
          (ADD_add_r_ri eR:$s0, (NegateImm simm7_negated:$imm))>;

// 4.1.8 ADD/SUB/ADC/SBC – Addition/subtraction
def : PatGprGpr<add, ADD, type>;
def : PatGprGpr<sub, SUB, type>;
// 4.1.9 AND / OR / XOR – Bitwise Operations
def : PatGprGpr<and, AND, type>;
def : PatGprGpr<or, OR, type>;
def : PatGprGpr<xor, XOR, type>;
// 4.1.12 MUL – Scalar multiplication
def : PatGprGpr<mul, MUL_mul_r_rr, type>;
}
defm : ALUPatterns<i32>;

// 4.1.7 SELECT
class SelectPat<ValueType type> :
         Pat<(type (select (i32 eR27:$rs1), (type eR:$rs2), (type eR:$rs3))),
          (SELNEZ eR:$rs2, eR:$rs3, eR27:$rs1)>;
def : SelectPat<ptr0>;
def : SelectPat<i32>;

// For vector select, VSEL instruction is used, since it does select word by word
// sel input is modified, "sel ? v1 : v2 is equivalent to vsel(sel-1, v1, v2)"
// rs1(sel) is zero-extended from the original 1 bit type
foreach vec256Ty = [v32i8, v16i16, v8i32] in {
def : Pat<(vec256Ty (select (i32 eRS8:$rs1), VEC256:$rs2, VEC256:$rs3)),
          (vec256Ty (EXTRACT_SUBREG (VSEL_32
                     (v32i16 (REG_SEQUENCE VEC512, VEC256:$rs2, sub_256_lo)),
                     (v32i16 (REG_SEQUENCE VEC512, VEC256:$rs3, sub_256_lo)),
                     (ADD_add_r_ri eR:$rs1, (i32 -1))),
                   sub_256_lo))>;
}
foreach vec512Ty = [v64i8, v32i16, v16i32] in {
  def : Pat<(vec512Ty (select (i32 eRS8:$rs1), VEC512:$rs2, VEC512:$rs3)),
            (vec512Ty (VSEL_32 VEC512:$rs2, VEC512:$rs3, (ADD_add_r_ri eR:$rs1, (i32 -1))))>;
}
foreach vec1024Ty = [v128i8, v64i16, v32i32] in {
def : Pat<(vec1024Ty (select (i32 eRS8:$rs1), VEC1024:$rs2, VEC1024:$rs3)),
          (REG_SEQUENCE VEC1024,
             (VSEL_32 (v16i32 (EXTRACT_SUBREG VEC1024:$rs2, sub_512_lo)),
                      (v16i32 (EXTRACT_SUBREG VEC1024:$rs3, sub_512_lo)),
                      (ADD_add_r_ri eR:$rs1, (i32 -1))), sub_512_lo,
             (VSEL_32 (v16i32 (EXTRACT_SUBREG VEC1024:$rs2, sub_512_hi)),
                      (v16i32 (EXTRACT_SUBREG VEC1024:$rs3, sub_512_hi)),
                      (ADD_add_r_ri eR:$rs1, (i32 -1))), sub_512_hi
          )>;
}

class SelectNezPat<ValueType type> :
         Pat<(type (select (i32 (setne eR27:$rs1, (i32 0))), (type eR:$rs2), (type eR:$rs3))),
          (SELNEZ eR:$rs2, eR:$rs3, eR27:$rs1)>;

def : SelectNezPat<ptr0>;
def : SelectNezPat<i32>;

class SelectNotPat<ValueType type> :
         Pat<(type (select (i32 (seteq eR27:$rs1, (i32 0))), (type eR:$rs2), (type eR:$rs3))),
          (SELEQZ eR:$rs2, eR:$rs3, eR27:$rs1)>;

def : SelectNotPat<ptr0>;
def : SelectNotPat<i32>;

// Compare operations with corresponding instructions
class CmpPat<PatFrag CondOp, AIE2Inst Inst, ValueType type>
    : Pat<(i32 (CondOp (type eR:$rs1), (type eR:$rs2))),
	       (Inst eR:$rs1, eR:$rs2)>;
multiclass CmpZeroPat<PatFrag CondOp, AIE2Inst Inst, ValueType type> {
  def : Pat<(i32 (CondOp (type eR:$rs1), (type 0))),
	       (Inst eR:$rs1)>;
  def : Pat<(i32 (CondOp (type 0), (type eR:$rs1))),
	       (Inst eR:$rs1)>;
}

// Compare operations without corresponding instructions, but which
// are trivially supported by swapping the two input operands
class CmpSwapPat<PatFrag CondOp, AIE2Inst Inst, ValueType type>
    : Pat<(i32 (CondOp (type eR:$rs1), (type eR:$rs2))),
          (Inst eR:$rs2, eR:$rs1)>;

multiclass CmpPatternsUnsigned<ValueType type> {
def : CmpPat<seteq, EQ, type>;
defm : CmpZeroPat<seteq, EQZ, type>;
def : CmpPat<setne, NE, type>;
defm : CmpZeroPat<setne, NEZ, type>;
def : CmpPat<setult, LTU, type>;
def : CmpPat<setuge, GEU, type>;
def : CmpSwapPat<setugt, LTU, type>;
def : CmpSwapPat<setule, GEU, type>;
}
multiclass CmpPatternsSigned<ValueType type> {
def : CmpPat<setlt, LT, type>;
def : CmpPat<setge, GE, type>;
def : CmpSwapPat<setgt, LT, type>;
def : CmpSwapPat<setle, GE, type>;
}

// 4.1.4 EQZ / NEZ – Compare to zero
// 4.1.10 EQ / NE / GE / GEU / LT / LTU – Compare registers
defm : CmpPatternsUnsigned<i32>;
defm : CmpPatternsUnsigned<ptr0>;
defm : CmpPatternsSigned<i32>;

// 4.1.11 LSHL / ASHL – Shift register
// TODO: Could we always lower right shifts to left shifts to help simplify constants?
def : Pat<(srl (i32 eR:$rs1), (i32 eR:$rs2)), (LSHL eR:$rs1, (SUB (MOV_RLC_imm10_pseudo (i32 0)), eR:$rs2))>;
def : Pat<(sra (i32 eR:$rs1), (i32 eR:$rs2)), (ASHL eR:$rs1, (SUB (MOV_RLC_imm10_pseudo (i32 0)), eR:$rs2))>;
def : Pat<(shl (i32 eR:$rs1), (i32 eR:$rs2)), (LSHL eR:$rs1, eR:$rs2)>;
def : Pat<(srl (i32 eR:$rs1), (i32 simm32:$amount)),
          (LSHL eR:$rs1, (MOV_RLC_imm10_pseudo (i32 (NegateImm simm32:$amount))))>;
def : Pat<(sra (i32 eR:$rs1), (i32 simm32:$amount)),
          (ASHL eR:$rs1, (MOV_RLC_imm10_pseudo (i32 (NegateImm simm32:$amount))))>;

// This converts a generic FrameIndex into a TargetFrameIndex, frameindex
// cannot be used in an output pattern.
def frameindex_to_targetframeindex : SDNodeXForm<frameindex, [{
  auto FI = cast<FrameIndexSDNode>(N);
  return CurDAG->getTargetFrameIndex(FI->getIndex(), MVT::i32);
}]>;

// Contrary to SelectionDAG which adds the FrameIndex to the selected instruction
// automatically (in SelectionDAG/InstrEmitter.cpp), GlobalIsel does not.
// This is done in a manually written renderFrameIndex function instead.
def gi_frameindex_to_targetframeindex : GICustomOperandRenderer<"renderFrameIndex">,
  GISDNodeXFormEquiv<frameindex_to_targetframeindex>;

// A bare frameindex. Placeholder until eliminateFrameIndex.
def : Pat<(ptr0 frameindex:$fi), (PseudoFI(frameindex_to_targetframeindex $fi))>;

// 3.16 ST – Scalar Word Store
// 3.17 ST.s8 / ST.s16 – Scalar Sub-Word Store
class StPat<PatFrag StoreOp, AIE2Inst InstInd, ValueType type, RegisterClass RegClass>
    : Pat<(StoreOp (type RegClass:$rs2), eP:$rs1), (InstInd RegClass:$rs2, eP:$rs1, (i20 0))>;
foreach Ty = [i20, i32, ptr0] in {
	def : StPat<store, ST_dms_sts_idx_imm, Ty, eR>;
}
def : StPat<store, ST_dms_sts_idx_imm, ptr0, eP>; // Handle stores from PTR
def : StPat<store, ST_dms_sts_idx_imm, i20, eM>; // Handle stores from MOD

class PartWordStPat<PatFrag StoreOp, AIE2Inst InstInd, ValueType type, RegisterClass RegClass>
    : Pat<(StoreOp (type RegClass:$rs2), eP:$rs1), (InstInd RegClass:$rs2, eP:$rs1, (i32 0))>;
foreach Ty = [i32] in {
def : PartWordStPat<truncstorei8, ST_S8_ag_idx_imm, Ty, eR>;
def : PartWordStPat<truncstorei16, ST_S16_ag_idx_imm, Ty, eR>;
}

// 3.2 VLD - Vector Load
class VectorLdPat<ValueType ResType, AIE2Inst Inst = VLDA_dmw_lda_w_ag_idx_imm>
    : Pat<(ResType (load eP:$rs1)), (Inst eP:$rs1, (i20 0))>;

// 1024-bit vector load
class VectorLdPat1024<ValueType Rtype, ValueType type, AIE2Inst Inst = VLDA_dmw_lda_w_ag_idx_imm>
    : Pat<(Rtype (load eP:$rs1)),
             (INSERT_SUBREG
               (INSERT_SUBREG
                 (Rtype  (IMPLICIT_DEF)),
                 (INSERT_SUBREG
                   (INSERT_SUBREG (type (IMPLICIT_DEF)), (Inst eP:$rs1, (i20 0)), sub_256_lo),
                   (Inst eP:$rs1, (i20 32)),
                   sub_256_hi),
                 sub_512_lo),
               (INSERT_SUBREG
                 (INSERT_SUBREG (type (IMPLICIT_DEF)), (Inst eP:$rs1, (i20 64)), sub_256_lo),
                 (Inst eP:$rs1, (i20 96)),
                 sub_256_hi),
               sub_512_hi) >;
def : VectorLdPat1024<v128i8, v64i8>;
def : VectorLdPat1024<v64i16, v32i16>;
def : VectorLdPat1024<v32i32, v16i32>;
// 1024-bit accumulator load
def : VectorLdPat1024<v16i64, v8i64, VLDA_dmw_lda_am_ag_idx_imm>;

def : Pat<(int_aie2_load_4x16_lo VEC256:$ptr), (VLDB_4x16_LO VEC256:$ptr)>;
def : Pat<(int_aie2_load_4x16_hi VEC256:$ptr), (VLDB_4x16_HI VEC256:$ptr)>;
def : Pat<(int_aie2_load_4x32_lo VEC256:$ptr), (VLDB_4x32_LO VEC256:$ptr)>;
def : Pat<(int_aie2_load_4x32_hi VEC256:$ptr), (VLDB_4x32_HI VEC256:$ptr)>;
def : Pat<(int_aie2_load_4x64_lo VEC256:$ptr), (VLDB_4x64_LO VEC256:$ptr)>;
def : Pat<(int_aie2_load_4x64_hi VEC256:$ptr), (VLDB_4x64_HI VEC256:$ptr)>;

// 3.20 LD – Scalar Load
class LdPat<PatFrag LoadOp, AIE2Inst Inst, ValueType ResType>
    : Pat<(ResType (LoadOp eP:$rs1)), (Inst eP:$rs1, (i20 0))>;
foreach Ty = [i20, i32, ptr0] in {
  def : LdPat<load, LDA_dms_lda_idx_imm, Ty>;
}
// Note: Loads to PTR are handled in AIE2InstructionSelector.cpp.
class PartWordLdPat<PatFrag LoadOp, AIE2Inst Inst, ValueType ResType>
    : Pat<(ResType (LoadOp eP:$rs1)), (Inst eP:$rs1, (i32 0 ))>;
foreach Ty = [i32] in {
def : PartWordLdPat<sextloadi16, LDA_S16_ag_idx_imm, Ty>;
def : PartWordLdPat<extloadi16, LDA_S16_ag_idx_imm, Ty>;
def : PartWordLdPat<zextloadi16, LDA_U16_ag_idx_imm, Ty>;
def : PartWordLdPat<sextloadi8, LDA_S8_ag_idx_imm, Ty>;
def : PartWordLdPat<extloadi8, LDA_S8_ag_idx_imm, Ty>;
def : PartWordLdPat<zextloadi8, LDA_U8_ag_idx_imm, Ty>;
}

// 4.2.1 ACQ – Acquire semaphore
def : Pat<(int_aie2_acquire eR:$mRx, eR:$mRy), (ACQ_mLockId_reg eR:$mRx, eR:$mRy)>;
def : Pat<(int_aie2_acquire_cond eR:$mRx, eR:$mRy, eR26:$s2), (ACQ_COND_mLockId_reg eR:$mRx, eR:$mRy, eR26:$s2)>;
def : Pat<(int_aie2_acquire imm6:$id, eR:$mRy), (ACQ_mLockId_imm imm6:$id, eR:$mRy)>;
def : Pat<(int_aie2_acquire_cond imm6:$id, eR:$mRy, eR26:$s2), (ACQ_COND_mLockId_imm imm6:$id, eR:$mRy, eR26:$s2)>;

// 4.2.2 REL – Release semaphore
def : Pat<(int_aie2_release eR:$mRx, eR:$mRy), (REL_mLockId_reg eR:$mRx, eR:$mRy)>;
def : Pat<(int_aie2_release_cond eR:$mRx, eR:$mRy, eR26:$s2), (REL_COND_mLockId_reg eR:$mRx, eR:$mRy, eR26:$s2)>;
def : Pat<(int_aie2_release imm6:$id, eR:$mRy), (REL_mLockId_imm imm6:$id, eR:$mRy)>;
def : Pat<(int_aie2_release_cond imm6:$id, eR:$mRy, eR26:$s2), (REL_COND_mLockId_imm imm6:$id, eR:$mRy, eR26:$s2)>;

// 4.4.2 DONE
def : Pat<(int_aie2_done), (DONE)>;

// 4.4.4 J – Absolute Jump
// Note we cannot use addr20 in the output pattern, as it isn't a valid MVT.
def : Pat<(br bb:$addr), (PseudoJ_jump_imm bb:$addr)>;

// 4.4.7 JZ/JNZ
// Out of line here. Filling a gap in SelectionDAGCompat.td
def : GINodeEquiv<G_BRCOND, brcond>;

// Base pattern
def : Pat <(brcond (i32 eR:$mRx), bb:$addr), (PseudoJNZ eR:$mRx, bb:$addr)>;
// Absorb conversion to 0/1
def : Pat <(brcond (i32 (setne eR:$mRx, (i32 0))), bb:$addr), (PseudoJNZ eR:$mRx, bb:$addr)>;
// Absorb logical negation by reversing branch condition
def : Pat <(brcond (i32 (seteq eR:$mRx, (i32 0))), bb:$addr), (PseudoJZ eR:$mRx, bb:$addr)>;


// MUL Intrinsic
def : Pat<(int_aie2_I512_I512_acc64_mul_conf VEC512:$s1, VEC512:$s2, eR:$c),
           (VMUL_vmac_cm_core_dense VEC512:$s1, VEC512:$s2, eR:$c)>;
def : Pat<(int_aie2_I512_I512_acc32_mul_conf VEC512:$s1, VEC512:$s2, eR:$c),
           (VMUL_vmac_cm_core_dense VEC512:$s1, VEC512:$s2, eR:$c)>;
def : Pat<(int_aie2_bf_mul16_conf VEC512:$s1, VEC512:$s2, eR:$c),
           (VMUL_F_vmac_bm_core_dense VEC512:$s1, VEC512:$s2, eR:$c)>;

// NEGMUL Intrinsic
def : Pat<(int_aie2_I512_I512_acc64_negmul_conf VEC512:$s1, VEC512:$s2, eR:$c),
           (VNEGMUL_vmac_cm_core_dense VEC512:$s1, VEC512:$s2, eR:$c)>;
def : Pat<(int_aie2_I512_I512_acc32_negmul_conf VEC512:$s1, VEC512:$s2, eR:$c),
           (VNEGMUL_vmac_cm_core_dense VEC512:$s1, VEC512:$s2, eR:$c)>;
def : Pat<(int_aie2_bf_negmul16_conf VEC512:$s1, VEC512:$s2, eR:$c),
           (VNEGMUL_F_vmac_bm_core_dense VEC512:$s1, VEC512:$s2, eR:$c)>;

// MAC Intrinsic
def : Pat<(int_aie2_I512_I512_ACC1024_acc64_mac_conf VEC512:$s1, VEC512:$s2, ACC1024:$acc1, eR:$c),
           (VMAC_vmac_cm_core_dense ACC1024:$acc1, VEC512:$s1, VEC512:$s2, eR:$c)>;
def : Pat<(int_aie2_I512_I512_ACC1024_acc32_mac_conf VEC512:$s1, VEC512:$s2, ACC1024:$acc1, eR:$c),
           (VMAC_vmac_cm_core_dense ACC1024:$acc1, VEC512:$s1, VEC512:$s2, eR:$c)>;
def : Pat<(int_aie2_bf_mac16_conf VEC512:$s1, VEC512:$s2, ACC512:$acc1, eR:$c),
           (VMAC_F_vmac_bm_core_dense ACC512:$acc1, VEC512:$s1, VEC512:$s2, eR:$c)>;

// MSC Intrinsic
def : Pat<(int_aie2_I512_I512_ACC1024_acc64_msc_conf VEC512:$s1, VEC512:$s2, ACC1024:$acc1, eR:$c),
           (VMSC_vmac_cm_core_dense ACC1024:$acc1, VEC512:$s1, VEC512:$s2, eR:$c)>;
def : Pat<(int_aie2_I512_I512_ACC1024_acc32_msc_conf VEC512:$s1, VEC512:$s2, ACC1024:$acc1, eR:$c),
           (VMSC_vmac_cm_core_dense ACC1024:$acc1, VEC512:$s1, VEC512:$s2, eR:$c)>;
def : Pat<(int_aie2_bf_msc16_conf  VEC512:$s1, VEC512:$s2, ACC512:$acc1, eR:$c),
           (VMSC_F_vmac_bm_core_dense ACC512:$acc1, VEC512:$s1, VEC512:$s2, eR:$c)>;

// NEGMAC Intrinsic
def : Pat<(int_aie2_I512_I512_ACC1024_acc64_negmac_conf VEC512:$s1, VEC512:$s2, ACC1024:$acc1, eR:$c),
           (VNEGMAC_vmac_cm_core_dense ACC1024:$acc1, VEC512:$s1, VEC512:$s2, eR:$c)>;
def : Pat<(int_aie2_I512_I512_ACC1024_acc32_negmac_conf VEC512:$s1, VEC512:$s2, ACC1024:$acc1, eR:$c),
           (VNEGMAC_vmac_cm_core_dense ACC1024:$acc1, VEC512:$s1, VEC512:$s2, eR:$c)>;

// NEGMSC Intrinsic
def : Pat<(int_aie2_I512_I512_ACC1024_acc64_negmsc_conf VEC512:$s1, VEC512:$s2, ACC1024:$acc1, eR:$c),
           (VNEGMSC_vmac_cm_core_dense ACC1024:$acc1, VEC512:$s1, VEC512:$s2, eR:$c)>;
def : Pat<(int_aie2_I512_I512_ACC1024_acc32_negmsc_conf VEC512:$s1, VEC512:$s2, ACC1024:$acc1, eR:$c),
           (VNEGMSC_vmac_cm_core_dense ACC1024:$acc1, VEC512:$s1, VEC512:$s2, eR:$c)>;

// ADDMAC Intrinsic
// Matrix mul - add/sub
class VADDSUB_dense<Intrinsic ID, Instruction Inst> :
    Pat<(ID VEC512:$s1, VEC512:$s2, ACC1024:$acc1, ACC1024:$acc2, eR:$c),
        (Inst ACC1024:$acc1, ACC1024:$acc2, VEC512:$s1, VEC512:$s2, eR:$c)>;

def : VADDSUB_dense<int_aie2_I512_I512_ACC1024_ACC1024_acc64_addmac_conf, VADDMAC_vmac_bm_core_dense>;
def : VADDSUB_dense<int_aie2_I512_I512_ACC1024_ACC1024_acc64_addmsc_conf, VADDMSC_vmac_bm_core_dense>;
def : VADDSUB_dense<int_aie2_I512_I512_ACC1024_ACC1024_acc64_submac_conf, VSUBMAC_vmac_bm_core_dense>;
def : VADDSUB_dense<int_aie2_I512_I512_ACC1024_ACC1024_acc64_submsc_conf, VSUBMSC_vmac_bm_core_dense>;

def : VADDSUB_dense<int_aie2_I512_I512_ACC1024_ACC1024_acc32_addmac_conf, VADDMAC_vmac_bm_core_dense>;
def : VADDSUB_dense<int_aie2_I512_I512_ACC1024_ACC1024_acc32_addmsc_conf, VADDMSC_vmac_bm_core_dense>;
def : VADDSUB_dense<int_aie2_I512_I512_ACC1024_ACC1024_acc32_submac_conf, VSUBMAC_vmac_bm_core_dense>;
def : VADDSUB_dense<int_aie2_I512_I512_ACC1024_ACC1024_acc32_submsc_conf, VSUBMSC_vmac_bm_core_dense>;

def : Pat<(int_aie2_bf_addmac16_conf VEC512:$s1, VEC512:$s2, ACC512:$acc1, ACC512:$acc2, eR:$c),
           (VADDMAC_F_vmac_bm_core_dense ACC512:$acc1, ACC512:$acc2, VEC512:$s1, VEC512:$s2, eR:$c)>;

// ADDMSC Intrinsic
def : Pat<(int_aie2_bf_addmsc16_conf VEC512:$s1, VEC512:$s2, ACC512:$acc1, ACC512:$acc2, eR:$c),
           (VADDMSC_F_vmac_bm_core_dense ACC512:$acc1, ACC512:$acc2, VEC512:$s1, VEC512:$s2, eR:$c)>;

// VNEG Intrinsic
def : Pat<(int_aie2_ACC1024_acc32_neg_conf ACC1024:$s1, eR:$c), (VNEG ACC1024:$s1, eR:$c)>;
def : Pat<(int_aie2_ACC1024_acc64_neg_conf ACC1024:$s1, eR:$c), (VNEG ACC1024:$s1, eR:$c)>;

// VNEG_F Intrinsic
def : Pat<(int_aie2_ACC512_accfloat_neg_conf ACC512:$s1, eR:$c), (VNEG_F ACC512:$s1, eR:$c)>;

// UPS Intrinsic
// Note : Non-constant sign is handled in .cpp
def : Pat<(int_aie2_acc32_v16_I256_ups VEC256:$src, mSs:$shift, 0x0),
           (VUPS_S32_D16_mv_ups_w2b VEC256:$src, mSs:$shift)>;
def : Pat<(int_aie2_acc32_v32_I256_ups VEC256:$src, mSs:$shift, 0x0),
           (VUPS_S32_D8_mv_ups_w2c VEC256:$src, mSs:$shift)>;
def : Pat<(int_aie2_acc32_v32_I512_ups VEC512:$src, mSs:$shift, 0x0),
           (VUPS_S32_D16_mv_ups_x2c VEC512:$src, mSs:$shift)>;
def : Pat<(int_aie2_acc64_v16_I256_ups VEC256:$src, mSs:$shift, 0x0),
           (VUPS_S64_D16_mv_ups_w2c VEC256:$src, mSs:$shift)>;
def : Pat<(int_aie2_acc64_v16_I512_ups VEC512:$src, mSs:$shift, 0x0),
           (VUPS_S64_D32_mv_ups_x2c VEC512:$src, mSs:$shift)>;
def : Pat<(int_aie2_acc64_v8_I256_ups  VEC256:$src, mSs:$shift, 0x0),
           (VUPS_S64_D32_mv_ups_w2b VEC256:$src, mSs:$shift)>;
def : Pat<(int_aie2_acc32_v16_I256_ups VEC256:$src, mSs:$shift, 0x1),
           (VUPS_S32_S16_mv_ups_w2b VEC256:$src, mSs:$shift)>;
def : Pat<(int_aie2_acc32_v32_I256_ups VEC256:$src, mSs:$shift, 0x1),
           (VUPS_S32_S8_mv_ups_w2c VEC256:$src, mSs:$shift)>;
def : Pat<(int_aie2_acc32_v32_I512_ups VEC512:$src, mSs:$shift, 0x1),
           (VUPS_S32_S16_mv_ups_x2c VEC512:$src, mSs:$shift)>;
def : Pat<(int_aie2_acc64_v16_I256_ups VEC256:$src, mSs:$shift, 0x1),
           (VUPS_S64_S16_mv_ups_w2c VEC256:$src, mSs:$shift)>;
def : Pat<(int_aie2_acc64_v16_I512_ups VEC512:$src, mSs:$shift, 0x1),
           (VUPS_S64_S32_mv_ups_x2c VEC512:$src, mSs:$shift)>;
def : Pat<(int_aie2_acc64_v8_I256_ups  VEC256:$src, mSs:$shift, 0x1),
           (VUPS_S64_S32_mv_ups_w2b VEC256:$src, mSs:$shift)>;

// SRS Intrinsic
// Note : Non-constant sign is handled in .cpp
def : Pat<(int_aie2_I256_v16_acc32_srs ACC512:$src,  mSs:$shft, 0x0),
           (VSRS_D16_S32_mv_w_srs ACC512:$src,  mSs:$shft)>;
def : Pat<(int_aie2_I256_v16_acc64_srs ACC1024:$src, mSs:$shft, 0x0),
           (VSRS_D16_S64_mv_w_srs ACC1024:$src, mSs:$shft)>;
def : Pat<(int_aie2_I256_v32_acc32_srs ACC1024:$src, mSs:$shft, 0x0),
           (VSRS_D8_S32_mv_w_srs  ACC1024:$src, mSs:$shft)>;
def : Pat<(int_aie2_I256_v8_acc64_srs  ACC512:$src,  mSs:$shft, 0x0),
           (VSRS_D32_S64_mv_w_srs ACC512:$src,  mSs:$shft)>;
def : Pat<(int_aie2_I512_v16_acc64_srs ACC1024:$src, mSs:$shft, 0x0),
           (VSRS_D32_S64_mv_x_srs ACC1024:$src, mSs:$shft)>;
def : Pat<(int_aie2_I512_v32_acc32_srs ACC1024:$src, mSs:$shft, 0x0),
           (VSRS_D16_S32_mv_x_srs ACC1024:$src, mSs:$shft)>;
def : Pat<(int_aie2_I256_v16_acc32_srs ACC512:$src,  mSs:$shft, 0x1),
           (VSRS_S16_S32_mv_w_srs ACC512:$src,  mSs:$shft)>;
def : Pat<(int_aie2_I256_v16_acc64_srs ACC1024:$src, mSs:$shft, 0x1),
           (VSRS_S16_S64_mv_w_srs ACC1024:$src, mSs:$shft)>;
def : Pat<(int_aie2_I256_v32_acc32_srs ACC1024:$src, mSs:$shft, 0x1),
           (VSRS_S8_S32_mv_w_srs  ACC1024:$src, mSs:$shft)>;
def : Pat<(int_aie2_I256_v8_acc64_srs  ACC512:$src,  mSs:$shft, 0x1),
           (VSRS_S32_S64_mv_w_srs ACC512:$src,  mSs:$shft)>;
def : Pat<(int_aie2_I512_v16_acc64_srs ACC1024:$src, mSs:$shft, 0x1),
           (VSRS_S32_S64_mv_x_srs ACC1024:$src, mSs:$shft)>;
def : Pat<(int_aie2_I512_v32_acc32_srs ACC1024:$src, mSs:$shft, 0x1),
           (VSRS_S16_S32_mv_x_srs ACC1024:$src, mSs:$shft)>;

// VSHUFFLE
def : Pat<(int_aie2_vshuffle VEC512:$s1, VEC512:$s2, eR:$mod),
           (VSHUFFLE VEC512:$s1, VEC512:$s2, eR:$mod)>;
def : Pat<(int_aie2_vshuffle_bf16 VEC512:$s1, VEC512:$s2, eR:$mod),
           (VSHUFFLE VEC512:$s1, VEC512:$s2, eR:$mod)>;

// VSHIFT Intrinsic (shift/shiftx/shift_bytes)
def : Pat<(int_aie2_vshift_I512_I512 VEC512:$s1, VEC512:$s2, 0x0, eR:$shift),
           (VSHIFT VEC512:$s1, VEC512:$s2, eR:$shift)>;
def : Pat<(int_aie2_vshift_I512_I512 VEC512:$s1, VEC512:$s2, mSs:$pre ,eR:$shift),
           (VSHIFT_ALIGN VEC512:$s1, mSs:$pre, VEC512:$s2, eR:$shift)>;
def : Pat<(int_aie2_vshift_bf512_bf512 VEC512:$s1, VEC512:$s2, mSs:$pre ,eR:$shift),
           (VSHIFT_ALIGN VEC512:$s1, mSs:$pre, VEC512:$s2, eR:$shift)>;

// VSEL
def : Pat<(int_aie2_vsel8 VEC512:$src1, VEC512:$src2, eL:$lsel),
           (VSEL_8 VEC512:$src1, VEC512:$src2, eL:$lsel)>;

def : Pat<(int_aie2_vsel16 VEC512:$src1, VEC512:$src2, eRS8:$rsel),
           (VSEL_16 VEC512:$src1, VEC512:$src2, eRS8:$rsel)>;

def : Pat<(int_aie2_vsel32 VEC512:$src1, VEC512:$src2, eRS8:$rsel),
           (VSEL_32 VEC512:$src1, VEC512:$src2, eRS8:$rsel)>;

// VADDSUB
def : Pat<(int_aie2_vaddsub8 VEC512:$src1, VEC512:$src2, eL:$lsel),
           (VADDSUB_8 VEC512:$src1, VEC512:$src2, eL:$lsel)>;

def : Pat<(int_aie2_vaddsub16 VEC512:$src1, VEC512:$src2, eRS8:$rsel),
           (VADDSUB_16 VEC512:$src1, VEC512:$src2, eRS8:$rsel)>;

def : Pat<(int_aie2_vaddsub32 VEC512:$src1, VEC512:$src2, eRS8:$rsel),
           (VADDSUB_32 VEC512:$src1, VEC512:$src2, eRS8:$rsel)>;

// VMAXDIFF_LT
def : Pat<(int_aie2_vmaxdiff_lt8 VEC512:$src1, VEC512:$src2, 0x0),
           (VMAX_LT_D8 VEC512:$src1, VEC512:$src2)>;
def : Pat<(int_aie2_vmaxdiff_lt16 VEC512:$src1, VEC512:$src2, 0x0),
           (VMAX_LT_D16 VEC512:$src1, VEC512:$src2)>;
def : Pat<(int_aie2_vmaxdiff_lt32 VEC512:$src1, VEC512:$src2, 0x0),
           (VMAX_LT_D32 VEC512:$src1, VEC512:$src2)>;
def : Pat<(int_aie2_vmaxdiff_lt8 VEC512:$src1, VEC512:$src2, 0x1),
           (VMAX_LT_S8 VEC512:$src1, VEC512:$src2)>;
def : Pat<(int_aie2_vmaxdiff_lt16 VEC512:$src1, VEC512:$src2, 0x1),
           (VMAX_LT_S16 VEC512:$src1, VEC512:$src2)>;
def : Pat<(int_aie2_vmaxdiff_lt32 VEC512:$src1, VEC512:$src2, 0x1),
           (VMAX_LT_S32 VEC512:$src1, VEC512:$src2)>;

// VADD/VSUB
def : Pat<(add v64i8:$src1, v64i8:$src2),   (VADD_8  VEC512:$src1, VEC512:$src2)>;
def : Pat<(add v32i16:$src1, v32i16:$src2), (VADD_16 VEC512:$src1, VEC512:$src2)>;
def : Pat<(add v16i32:$src1, v16i32:$src2), (VADD_32 VEC512:$src1, VEC512:$src2)>;

def : Pat<(sub v64i8:$src1, v64i8:$src2),   (VSUB_8  VEC512:$src1, VEC512:$src2)>;
def : Pat<(sub v32i16:$src1, v32i16:$src2), (VSUB_16 VEC512:$src1, VEC512:$src2)>;
def : Pat<(sub v16i32:$src1, v16i32:$src2), (VSUB_32 VEC512:$src1, VEC512:$src2)>;

// VSUB_LT/VSUB_GE
class VSUB_LT_GE_INSTR<SDPatternOperator OpNode, AIE2Inst Inst, bits<1> sgn>:
    Pat<(OpNode VEC512:$src1, VEC512:$src2, sgn), (Inst VEC512:$src1, VEC512:$src2)>;

def : VSUB_LT_GE_INSTR<int_aie2_vsub_lt8, VSUB_LT_D8, 0x0>;
def : VSUB_LT_GE_INSTR<int_aie2_vsub_lt16, VSUB_LT_D16, 0x0>;
def : VSUB_LT_GE_INSTR<int_aie2_vsub_lt32, VSUB_LT_D32, 0x0>;

def : VSUB_LT_GE_INSTR<int_aie2_vsub_ge8, VSUB_GE_D8, 0x0>;
def : VSUB_LT_GE_INSTR<int_aie2_vsub_ge16, VSUB_GE_D16, 0x0>;
def : VSUB_LT_GE_INSTR<int_aie2_vsub_ge32, VSUB_GE_D32, 0x0>;

def : VSUB_LT_GE_INSTR<int_aie2_vsub_lt8, VSUB_LT_S8, 0x1>;
def : VSUB_LT_GE_INSTR<int_aie2_vsub_lt16, VSUB_LT_S16, 0x1>;
def : VSUB_LT_GE_INSTR<int_aie2_vsub_lt32, VSUB_LT_S32, 0x1>;

def : VSUB_LT_GE_INSTR<int_aie2_vsub_ge8, VSUB_GE_S8, 0x1>;
def : VSUB_LT_GE_INSTR<int_aie2_vsub_ge16, VSUB_GE_S16, 0x1>;
def : VSUB_LT_GE_INSTR<int_aie2_vsub_ge32, VSUB_GE_S32, 0x1>;

// VINSERT
def : Pat<(int_aie2_vinsert8_I512 VEC512:$s1, eR29:$idx, eR:$s0),
           (VINSERT_8 VEC512:$s1, eR29:$idx, eR:$s0)>;
def : Pat<(int_aie2_vinsert16_I512 VEC512:$s1, eR29:$idx, eR:$s0),
           (VINSERT_16 VEC512:$s1, eR29:$idx, eR:$s0)>;
def : Pat<(int_aie2_vinsert32_I512 VEC512:$s1, eR29:$idx, eR:$s0),
           (VINSERT_32 VEC512:$s1, eR29:$idx, eR:$s0)>;
def : Pat<(int_aie2_vinsert64_I512 VEC512:$s1, eR29:$idx, eL:$s0),
           (VINSERT_64 VEC512:$s1, eR29:$idx, eL:$s0)>;
def : Pat<(int_aie2_vinsert16_bf512 VEC512:$s1, eR29:$idx, eR:$s0),
           (VINSERT_16 VEC512:$s1, eR29:$idx, eR:$s0)>;
def : Pat<(int_aie2_vinsert32_bf512 VEC512:$s1, eR29:$idx, eR:$s0),
           (VINSERT_32 VEC512:$s1, eR29:$idx, eR:$s0)>;
def : Pat<(int_aie2_vinsert64_bf512 VEC512:$s1, eR29:$idx, eL:$s0),
           (VINSERT_64 VEC512:$s1, eR29:$idx, eL:$s0)>;
def : Pat<(int_aie2_vinsert32_accfloat ACC512:$s1, eR29:$idx, eR:$s0),
           (COPY_TO_REGCLASS (VINSERT_32 (COPY_TO_REGCLASS ACC512:$s1, VEC512), eR29:$idx, eR:$s0), ACC512)>;

// VEXTRACT
// Note : Non-constant sign is handled in .cpp
def : Pat<(int_aie2_vextract_elem8_I512  VEC512:$s1, eRS4:$idx, 0x0),
           (VEXTRACT_D8 VEC512:$s1,  eRS4:$idx)>;
def : Pat<(int_aie2_vextract_elem16_I512 VEC512:$s1, eRS4:$idx, 0x0),
           (VEXTRACT_D16 VEC512:$s1, eRS4:$idx)>;
def : Pat<(int_aie2_vextract_elem32_I512 VEC512:$s1, eRS4:$idx, 0x0),
           (VEXTRACT_D32 VEC512:$s1, eRS4:$idx)>;
def : Pat<(int_aie2_vextract_elem64_I512 VEC512:$s1, eRS4:$idx, 0x0),
           (VEXTRACT_D64 VEC512:$s1, eRS4:$idx)>;

def : Pat<(int_aie2_vextract_elem8_I512  VEC512:$s1, eRS4:$idx, 0x1),
           (VEXTRACT_S8 VEC512:$s1,  eRS4:$idx)>;
def : Pat<(int_aie2_vextract_elem16_I512 VEC512:$s1, eRS4:$idx, 0x1),
           (VEXTRACT_S16 VEC512:$s1, eRS4:$idx)>;
def : Pat<(int_aie2_vextract_elem32_I512 VEC512:$s1, eRS4:$idx, 0x1),
           (VEXTRACT_S32 VEC512:$s1, eRS4:$idx)>;
def : Pat<(int_aie2_vextract_elem64_I512 VEC512:$s1, eRS4:$idx, 0x1),
           (VEXTRACT_S64 VEC512:$s1, eRS4:$idx)>;

// VLT/VGE
// Note : Non-constant sign is handled in .cpp
class VLT_VGE_INSTR<SDPatternOperator OpNode, AIE2Inst Inst, bits<1> sgn>:
    Pat<(OpNode VEC512:$s1, VEC512:$s2, sgn), (Inst VEC512:$s1, VEC512:$s2)>;

def : VLT_VGE_INSTR<int_aie2_vlt8, VLT_D8, 0x0>;
def : VLT_VGE_INSTR<int_aie2_vlt16, VLT_D16, 0x0>;
def : VLT_VGE_INSTR<int_aie2_vlt32, VLT_D32, 0x0>;

def : VLT_VGE_INSTR<int_aie2_vge8, VGE_D8, 0x0>;
def : VLT_VGE_INSTR<int_aie2_vge16, VGE_D16, 0x0>;
def : VLT_VGE_INSTR<int_aie2_vge32, VGE_D32, 0x0>;

def : VLT_VGE_INSTR<int_aie2_vlt8, VLT_S8, 0x1>;
def : VLT_VGE_INSTR<int_aie2_vlt16, VLT_S16, 0x1>;
def : VLT_VGE_INSTR<int_aie2_vlt32, VLT_S32, 0x1>;

def : VLT_VGE_INSTR<int_aie2_vge8, VGE_S8, 0x1>;
def : VLT_VGE_INSTR<int_aie2_vge16, VGE_S16, 0x1>;
def : VLT_VGE_INSTR<int_aie2_vge32, VGE_S32, 0x1>;

// VGE/VLT BF16
def : Pat<(int_aie2_vltbf16 VEC512:$s1, VEC512:$s2),
            (VLT_BF16 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2_vgebf16 VEC512:$s1, VEC512:$s2),
            (VGE_BF16 VEC512:$s1, VEC512:$s2)>;

// VMAX_LT
// Note : Non-constant sign is handled in .cpp
def : Pat<(int_aie2_vmax_lt8 VEC512:$s1, VEC512:$s2, 0x0),
           (VMAX_LT_D8 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2_vmax_lt16 VEC512:$s1, VEC512:$s2, 0x0),
           (VMAX_LT_D16 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2_vmax_lt32 VEC512:$s1, VEC512:$s2, 0x0),
           (VMAX_LT_D32 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2_vmax_lt8 VEC512:$s1, VEC512:$s2, 0x1),
           (VMAX_LT_S8 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2_vmax_lt16 VEC512:$s1, VEC512:$s2, 0x1),
           (VMAX_LT_S16 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2_vmax_lt32 VEC512:$s1, VEC512:$s2, 0x1),
           (VMAX_LT_S32 VEC512:$s1, VEC512:$s2)>;
// bf16 is always signed
def : Pat<(int_aie2_vmax_ltbf16 VEC512:$s1, VEC512:$s2),
           (VMAX_LT_BF16 VEC512:$s1, VEC512:$s2)>;

// VMIN_GE
// Note : Non-constant sign is handled in .cpp
def : Pat<(int_aie2_vmin_ge8 VEC512:$s1, VEC512:$s2, 0x0),
           (VMIN_GE_D8 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2_vmin_ge16 VEC512:$s1, VEC512:$s2, 0x0),
           (VMIN_GE_D16 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2_vmin_ge32 VEC512:$s1, VEC512:$s2, 0x0),
           (VMIN_GE_D32 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2_vmin_ge8 VEC512:$s1, VEC512:$s2, 0x1),
           (VMIN_GE_S8 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2_vmin_ge16 VEC512:$s1, VEC512:$s2, 0x1),
           (VMIN_GE_S16 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2_vmin_ge32 VEC512:$s1, VEC512:$s2, 0x1),
           (VMIN_GE_S32 VEC512:$s1, VEC512:$s2)>;
// bf16 is always signed
def : Pat<(int_aie2_vmin_gebf16 VEC512:$s1, VEC512:$s2),
           (VMIN_GE_BF16 VEC512:$s1, VEC512:$s2)>;

// VEQZ
class VEQZ_INSTR<SDPatternOperator OpNode, AIE2Inst Inst>:
    Pat<(OpNode VEC512:$s2), (Inst VEC512:$s2)>;

def : VEQZ_INSTR<int_aie2_veqz8, VEQZ_8>;
def : VEQZ_INSTR<int_aie2_veqz16, VEQZ_16>;
def : VEQZ_INSTR<int_aie2_veqz32, VEQZ_32>;

// VBCST
def : Pat<(int_aie2_vbroadcast8_I512 eR:$s0),  (VBCST_8 eR:$s0)>;
def : Pat<(int_aie2_vbroadcast16_I512 eR:$s0), (VBCST_16 eR:$s0)>;
def : Pat<(int_aie2_vbroadcast32_I512 eR:$s0), (VBCST_32 eR:$s0)>;
def : Pat<(int_aie2_vbroadcast64_I512 eL:$s0), (VBCST_64 eL:$s0)>;
def : Pat<(int_aie2_vbroadcast16_bf512 eR:$s0), (VBCST_16 eR:$s0)>;
def : Pat<(int_aie2_vbroadcast32_bf512 eR:$s0), (VBCST_32 eR:$s0)>;
def : Pat<(int_aie2_vbroadcast64_bf512 eL:$s0), (VBCST_64 eL:$s0)>;

// VCLR
def : Pat<(int_aie2_vbroadcast_zero_acc1024), (VCLR_vclr)>;

// VEXTBCST
def : Pat<(int_aie2_vextract_broadcast8_I512 VEC512:$src, eR:$idx),
           (VEXTBCST_8_mRm  VEC512:$src, eR:$idx)>;
def : Pat<(int_aie2_vextract_broadcast16_I512 VEC512:$src, eR:$idx),
           (VEXTBCST_16_mRm  VEC512:$src, eR:$idx)>;
def : Pat<(int_aie2_vextract_broadcast32_I512 VEC512:$src, eR:$idx),
           (VEXTBCST_32_mRm  VEC512:$src, eR:$idx)>;
//def : Pat<(int_aie2_vextract_broadcast64_I512 VEC512:$src, eR:$idx),
//           (VEXTBCST_64_mRm  VEC512:$src, eR:$idx)>;
def : Pat<(int_aie2_vextract_broadcast32_bf512 VEC512:$src, eR:$idx),
           (VEXTBCST_32_mRm  VEC512:$src, eR:$idx)>;

def : Pat<(int_aie2_vextract_broadcast8_I512 VEC512:$src, imm5:$idx),
           (VEXTBCST_8_mExtractIdxImm  VEC512:$src, imm5:$idx)>;
def : Pat<(int_aie2_vextract_broadcast16_I512 VEC512:$src, imm5:$idx),
           (VEXTBCST_16_mExtractIdxImm  VEC512:$src, imm5:$idx)>;
def : Pat<(int_aie2_vextract_broadcast32_I512 VEC512:$src, imm5:$idx),
           (VEXTBCST_32_mExtractIdxImm  VEC512:$src, imm5:$idx)>;
//def : Pat<(int_aie2_vextract_broadcast64_I512 VEC512:$src, imm5:$idx),
//           (VEXTBCST_64_mExtractIdxImm  VEC512:$src, imm5:$idx)>;
def : Pat<(int_aie2_vextract_broadcast32_bf512 VEC512:$src, imm5:$idx),
           (VEXTBCST_32_mExtractIdxImm  VEC512:$src, imm5:$idx)>;


// VBCSTSHFL
def : Pat<(int_aie2_vbcst_shuffle8 eR:$s0, eR29:$idx),
           (VBCSTSHFL_8 eR:$s0, eR29:$idx)>;
def : Pat<(int_aie2_vbcst_shuffle16 eR:$s0, eR29:$idx),
           (VBCSTSHFL_16 eR:$s0, eR29:$idx)>;
def : Pat<(int_aie2_vbcst_shuffle32 eR:$s0, eR29:$idx),
           (VBCSTSHFL_32 eR:$s0, eR29:$idx)>;
def : Pat<(int_aie2_vbcst_shuffle64 eL:$s0, eR29:$idx),
           (VBCSTSHFL_64 eL:$s0, eR29:$idx)>;
def : Pat<(int_aie2_vbcst_shuffle_bf16 eR:$s0, eR29:$idx),
           (VBCSTSHFL_16 eR:$s0, eR29:$idx)>;

// Update
// int, bf16
foreach intr = [int_aie2_upd_I512_I256, int_aie2_upd_bf512_bf256] in {
  def : Pat<(intr VEC512:$src1, VEC256:$src2, 0x0),
              (INSERT_SUBREG VEC512:$src1, VEC256:$src2, sub_256_lo)>;
  def : Pat<(intr VEC512:$src1, VEC256:$src2, 0x1),
              (INSERT_SUBREG VEC512:$src1, VEC256:$src2, sub_256_hi)>;
}
foreach intr = [int_aie2_upd_I1024_I512, int_aie2_upd_bf1024_bf512] in {
  def : Pat<(intr VEC1024:$src1, VEC512:$src2, 0x0),
              (INSERT_SUBREG VEC1024:$src1, VEC512:$src2, sub_512_lo)>;
  def : Pat<(intr VEC1024:$src1, VEC512:$src2, 0x1),
              (INSERT_SUBREG VEC1024:$src1, VEC512:$src2, sub_512_hi)>;
}
foreach intr = [int_aie2_upd_I1024_I256, int_aie2_upd_bf1024_bf256] in {
  def : Pat<(intr VEC1024:$src1, VEC256:$src2, 0x0),
             (INSERT_SUBREG VEC1024:$src1,
              (v16i32 (REG_SEQUENCE VEC512, VEC256:$src2, sub_256_lo)),
             sub_512_lo)>;
  def : Pat<(intr VEC1024:$src1, VEC256:$src2, 0x1),
             (INSERT_SUBREG VEC1024:$src1,
              (v16i32 (REG_SEQUENCE VEC512, VEC256:$src2, sub_256_hi)),
             sub_512_lo)>;
  def : Pat<(intr VEC1024:$src1, VEC256:$src2, 0x2),
             (INSERT_SUBREG VEC1024:$src1,
              (v16i32 (REG_SEQUENCE VEC512, VEC256:$src2, sub_256_lo)),
             sub_512_hi)>;
  def : Pat<(intr VEC1024:$src1, VEC256:$src2, 0x3),
             (INSERT_SUBREG VEC1024:$src1,
              (v16i32 (REG_SEQUENCE VEC512, VEC256:$src2, sub_256_hi)),
             sub_512_hi)>;
}
// acc32, acc64
def : Pat<(int_aie2_upd_512_256_acc ACC512:$src1, ACC256:$src2, 0x0),
            (INSERT_SUBREG ACC512:$src1, ACC256:$src2, sub_256_lo)>;
def : Pat<(int_aie2_upd_512_256_acc ACC512:$src1, ACC256:$src2, 0x1),
            (INSERT_SUBREG ACC512:$src1, ACC256:$src2, sub_256_hi)>;
def : Pat<(int_aie2_upd_1024_512_acc ACC1024:$src1, ACC512:$src2, 0x0),
            (INSERT_SUBREG ACC1024:$src1, ACC512:$src2, sub_512_lo)>;
def : Pat<(int_aie2_upd_1024_512_acc ACC1024:$src1, ACC512:$src2, 0x1),
            (INSERT_SUBREG ACC1024:$src1, ACC512:$src2, sub_512_hi)>;
def : Pat<(int_aie2_upd_1024_256_acc ACC1024:$src1, ACC256:$src2, 0x0),
            (INSERT_SUBREG ACC1024:$src1,
            (v8i64 (REG_SEQUENCE ACC512, ACC256:$src2, sub_256_lo)),
            sub_512_lo)>;
def : Pat<(int_aie2_upd_1024_256_acc ACC1024:$src1, ACC256:$src2, 0x1),
            (INSERT_SUBREG ACC1024:$src1,
            (v8i64 (REG_SEQUENCE ACC512, ACC256:$src2, sub_256_hi)),
            sub_512_lo)>;
def : Pat<(int_aie2_upd_1024_256_acc ACC1024:$src1, ACC256:$src2, 0x2),
            (INSERT_SUBREG ACC1024:$src1,
            (v8i64 (REG_SEQUENCE ACC512, ACC256:$src2, sub_256_lo)),
            sub_512_hi)>;
def : Pat<(int_aie2_upd_1024_256_acc ACC1024:$src1, ACC256:$src2, 0x3),
            (INSERT_SUBREG ACC1024:$src1,
            (v8i64 (REG_SEQUENCE ACC512, ACC256:$src2, sub_256_hi)),
            sub_512_hi)>;

// Set
foreach intr = [int_aie2_set_I512_I256, int_aie2_set_bf512_bf256] in {
  def : Pat<(intr VEC256:$src, 0x0),
             (REG_SEQUENCE VEC512, VEC256:$src, sub_256_lo)>;
  def : Pat<(intr VEC256:$src, 0x1),
             (REG_SEQUENCE VEC512, VEC256:$src, sub_256_hi)>;
}
foreach intr = [int_aie2_set_I1024_I512, int_aie2_set_bf1024_bf512] in {
  def : Pat<(intr VEC512:$src, 0x0),
             (REG_SEQUENCE VEC1024, VEC512:$src, sub_512_lo)>;
  def : Pat<(intr VEC512:$src, 0x1),
             (REG_SEQUENCE VEC1024, VEC512:$src, sub_512_hi)>;
}
foreach intr = [int_aie2_set_I1024_I256, int_aie2_set_bf1024_bf256] in {
  def : Pat<(intr VEC256:$src, 0x0),
             (REG_SEQUENCE VEC1024, VEC256:$src, sub_256_lo)>;
  def : Pat<(intr VEC256:$src, 0x1),
             (REG_SEQUENCE VEC1024, VEC256:$src, sub_256_hi)>;
  def : Pat<(intr VEC256:$src, 0x2),
             (REG_SEQUENCE VEC1024, VEC256:$src, sub_512_hi_256_lo)>;
  def : Pat<(intr VEC256:$src, 0x3),
             (REG_SEQUENCE VEC1024, VEC256:$src, sub_512_hi_256_hi)>;
}

foreach intr = [int_aie2_set_512_256_acc32, int_aie2_set_512_256_acc64] in {
  def : Pat<(intr ACC256:$src, 0x0),
             (REG_SEQUENCE ACC512, ACC256:$src, sub_256_lo)>;
  def : Pat<(intr ACC256:$src, 0x1),
             (REG_SEQUENCE ACC512, ACC256:$src, sub_256_hi)>;
}
foreach intr = [int_aie2_set_1024_512_acc32, int_aie2_set_1024_512_acc64] in {
  def : Pat<(intr ACC512:$src, 0x0),
             (REG_SEQUENCE ACC1024, ACC512:$src, sub_512_lo)>;
  def : Pat<(intr ACC512:$src, 0x1),
             (REG_SEQUENCE ACC1024, ACC512:$src, sub_512_hi)>;
}
foreach intr = [int_aie2_set_1024_256_acc32, int_aie2_set_1024_256_acc64] in {
  def : Pat<(intr ACC256:$src, 0x0),
             (REG_SEQUENCE ACC1024, ACC256:$src, sub_256_lo)>;
  def : Pat<(intr ACC256:$src, 0x1),
             (REG_SEQUENCE ACC1024, ACC256:$src, sub_256_hi)>;
  def : Pat<(intr ACC256:$src, 0x2),
             (REG_SEQUENCE ACC1024, ACC256:$src, sub_512_hi_256_lo)>;
  def : Pat<(intr ACC256:$src, 0x3),
             (REG_SEQUENCE ACC1024, ACC256:$src, sub_512_hi_256_hi)>;
}

// Concat
class Concat256<SDPatternOperator OpNode, RegisterClass ConcatClass, RegisterClass RegClass>
    : Pat<(OpNode RegClass:$src1, RegClass:$src2),
            (REG_SEQUENCE ConcatClass, RegClass:$src1, sub_256_lo, RegClass:$src2, sub_256_hi)>;

class Concat512<SDPatternOperator OpNode, RegisterClass ConcatClass, RegisterClass RegClass>
    : Pat<(OpNode RegClass:$src1, RegClass:$src2),
            (REG_SEQUENCE ConcatClass, RegClass:$src1, sub_512_lo, RegClass:$src2, sub_512_hi)>;

class Concat1024<SDPatternOperator OpNode, RegisterClass ConcatClass, RegisterClass RegClass>
    : Pat<(OpNode RegClass:$src1, RegClass:$src2, RegClass:$src3, RegClass:$src4),
            (REG_SEQUENCE ConcatClass, RegClass:$src1, sub_256_lo, RegClass:$src2, sub_256_hi,
             RegClass:$src3, sub_512_hi_256_lo, RegClass:$src4, sub_512_hi_256_hi)>;

// int
def : Concat256<int_aie2_concat_I512_I256, VEC512, VEC256>;
def : Concat512<int_aie2_concat_I1024_I512, VEC1024, VEC512>;
def : Concat1024<int_aie2_concat_I1024_I256, VEC1024, VEC256>;

// bf16
def : Concat256<int_aie2_concat_bf512_bf256, VEC512, VEC256>;
def : Concat512<int_aie2_concat_bf1024_bf512, VEC1024, VEC512>;
def : Concat1024<int_aie2_concat_bf1024_bf256, VEC1024, VEC256>;

// acc32
def : Concat256<int_aie2_concat_512_256_acc32, ACC512, ACC256>;
def : Concat512<int_aie2_concat_1024_512_acc32, ACC1024, ACC512>;
def : Concat1024<int_aie2_concat_1024_256_acc32, ACC1024, ACC256>;

// acc64
def : Concat256<int_aie2_concat_512_256_acc64, ACC512, ACC256>;
def : Concat512<int_aie2_concat_1024_512_acc64, ACC1024, ACC512>;
def : Concat1024<int_aie2_concat_1024_256_acc64, ACC1024, ACC256>;

// Extract
def : Pat<(int_aie2_ext_I256_I512 VEC512:$src, 0x0),
           (v8i32 (EXTRACT_SUBREG VEC512:$src, sub_256_lo))>;
def : Pat<(int_aie2_ext_I256_I512 VEC512:$src, 0x1),
           (v8i32 (EXTRACT_SUBREG VEC512:$src, sub_256_hi))>;
def : Pat<(int_aie2_ext_I512_I1024 VEC1024:$src, 0x0),
           (v16i32 (EXTRACT_SUBREG VEC1024:$src, sub_512_lo))>;
def : Pat<(int_aie2_ext_I512_I1024 VEC1024:$src, 0x1),
           (v16i32 (EXTRACT_SUBREG VEC1024:$src, sub_512_hi))>;
def : Pat<(int_aie2_ext_I256_I1024 VEC1024:$src, 0x0),
           (v8i32 (EXTRACT_SUBREG VEC1024:$src, sub_256_lo))>;
def : Pat<(int_aie2_ext_I256_I1024 VEC1024:$src, 0x1),
           (v8i32 (EXTRACT_SUBREG VEC1024:$src, sub_256_hi))>;
def : Pat<(int_aie2_ext_I256_I1024 VEC1024:$src, 0x2),
           (v8i32 (EXTRACT_SUBREG VEC1024:$src, sub_512_hi_256_lo))>;
def : Pat<(int_aie2_ext_I256_I1024 VEC1024:$src, 0x3),
           (v8i32 (EXTRACT_SUBREG VEC1024:$src, sub_512_hi_256_hi))>;
def : Pat<(int_aie2_ext_bf256_bf512 VEC512:$src, 0x0),
           (v16bf16 (EXTRACT_SUBREG VEC512:$src, sub_256_lo))>;
def : Pat<(int_aie2_ext_bf256_bf512 VEC512:$src, 0x1),
           (v16bf16 (EXTRACT_SUBREG VEC512:$src, sub_256_hi))>;
def : Pat<(int_aie2_ext_bf512_bf1024 VEC1024:$src, 0x0),
           (v32bf16 (EXTRACT_SUBREG VEC1024:$src, sub_512_lo))>;
def : Pat<(int_aie2_ext_bf512_bf1024 VEC1024:$src, 0x1),
           (v32bf16 (EXTRACT_SUBREG VEC1024:$src, sub_512_hi))>;
def : Pat<(int_aie2_ext_bf256_bf1024 VEC1024:$src, 0x0),
           (v16bf16 (EXTRACT_SUBREG VEC1024:$src, sub_256_lo))>;
def : Pat<(int_aie2_ext_bf256_bf1024 VEC1024:$src, 0x1),
           (v16bf16 (EXTRACT_SUBREG VEC1024:$src, sub_256_hi))>;
def : Pat<(int_aie2_ext_bf256_bf1024 VEC1024:$src, 0x2),
           (v16bf16 (EXTRACT_SUBREG VEC1024:$src, sub_512_hi_256_lo))>;
def : Pat<(int_aie2_ext_bf256_bf1024 VEC1024:$src, 0x3),
           (v16bf16 (EXTRACT_SUBREG VEC1024:$src, sub_512_hi_256_hi))>;


def : Pat<(int_aie2_ext_256_512_acc ACC512:$src, 0x0), (v4i64 (EXTRACT_SUBREG ACC512:$src, sub_256_lo))>;
def : Pat<(int_aie2_ext_256_512_acc ACC512:$src, 0x1), (v4i64 (EXTRACT_SUBREG ACC512:$src, sub_256_hi))>;
def : Pat<(int_aie2_ext_512_1024_acc ACC1024:$src, 0x0), (v8i64 (EXTRACT_SUBREG ACC1024:$src, sub_512_lo))>;
def : Pat<(int_aie2_ext_512_1024_acc ACC1024:$src, 0x1), (v8i64 (EXTRACT_SUBREG ACC1024:$src, sub_512_hi))>;
def : Pat<(int_aie2_ext_256_1024_acc ACC1024:$src, 0x0),
            (v4i64 (EXTRACT_SUBREG ACC1024:$src, sub_256_lo))>;
def : Pat<(int_aie2_ext_256_1024_acc ACC1024:$src, 0x1),
            (v4i64 (EXTRACT_SUBREG ACC1024:$src, sub_256_hi))>;
def : Pat<(int_aie2_ext_256_1024_acc ACC1024:$src, 0x2),
            (v4i64 (EXTRACT_SUBREG ACC1024:$src, sub_512_hi_256_lo))>;
def : Pat<(int_aie2_ext_256_1024_acc ACC1024:$src, 0x3),
            (v4i64 (EXTRACT_SUBREG ACC1024:$src, sub_512_hi_256_hi))>;

// VBOR / VBAND
foreach vec256Ty = [v32i8, v16i16, v8i32] in {
  def : Pat<(vec256Ty (or VEC256:$src1, VEC256:$src2)),
            (vec256Ty (EXTRACT_SUBREG (VBOR
                        (v32i16 (REG_SEQUENCE VEC512, VEC256:$src1, sub_256_lo)),
                        (v32i16 (REG_SEQUENCE VEC512, VEC256:$src2, sub_256_lo))),
                       sub_256_lo))>;
  def : Pat<(vec256Ty (and VEC256:$src1, VEC256:$src2)),
            (vec256Ty (EXTRACT_SUBREG (VBAND
                        (v32i16 (REG_SEQUENCE VEC512, VEC256:$src1, sub_256_lo)),
                        (v32i16 (REG_SEQUENCE VEC512, VEC256:$src2, sub_256_lo))),
                       sub_256_lo))>;
}
foreach vec512Ty = [v64i8, v32i16, v16i32] in {
  def : Pat<(vec512Ty (or VEC512:$src1, VEC512:$src2)), (VBOR VEC512:$src1, VEC512:$src2)>;
  def : Pat<(vec512Ty (and VEC512:$src1, VEC512:$src2)), (VBAND VEC512:$src1, VEC512:$src2)>;
}
foreach vec1024Ty = [v128i8, v64i16, v32i32] in {
def : Pat<(vec1024Ty (or VEC1024:$src1, VEC1024:$src2)),
          (REG_SEQUENCE VEC1024,
                 (VBOR (v16i32 (EXTRACT_SUBREG VEC1024:$src1, sub_512_lo)),
                       (v16i32 (EXTRACT_SUBREG VEC1024:$src2, sub_512_lo))), sub_512_lo,
                 (VBOR (v16i32 (EXTRACT_SUBREG VEC1024:$src1, sub_512_hi)),
                       (v16i32 (EXTRACT_SUBREG VEC1024:$src2, sub_512_hi))), sub_512_hi
          )>;
def : Pat<(vec1024Ty (and VEC1024:$src1, VEC1024:$src2)),
          (REG_SEQUENCE VEC1024,
                 (VBAND (v16i32 (EXTRACT_SUBREG VEC1024:$src1, sub_512_lo)),
                        (v16i32 (EXTRACT_SUBREG VEC1024:$src2, sub_512_lo))), sub_512_lo,
                 (VBAND (v16i32 (EXTRACT_SUBREG VEC1024:$src1, sub_512_hi)),
                        (v16i32 (EXTRACT_SUBREG VEC1024:$src2, sub_512_hi))), sub_512_hi
          )>;
}


// VBNEG_LTZ
def : Pat<(int_aie2_vbneg_ltz8  VEC512:$s1), (VBNEG_LTZ_S8  VEC512:$s1)>;
def : Pat<(int_aie2_vbneg_ltz16 VEC512:$s1), (VBNEG_LTZ_S16 VEC512:$s1)>;
def : Pat<(int_aie2_vbneg_ltz32 VEC512:$s1), (VBNEG_LTZ_S32 VEC512:$s1)>;

// VNEG_GTZ
def : Pat<(int_aie2_vneg_gtz8  VEC512:$s1), (VNEG_GTZ8  VEC512:$s1)>;
def : Pat<(int_aie2_vneg_gtz16 VEC512:$s1), (VNEG_GTZ16 VEC512:$s1)>;
def : Pat<(int_aie2_vneg_gtz32 VEC512:$s1), (VNEG_GTZ32 VEC512:$s1)>;

// VABS_GTZ
// Note : Non-constant sign is handled in .cpp
def : Pat<(int_aie2_vabs_gtz8  VEC512:$s1, 0x0), (VABS_GTZ_D8  VEC512:$s1)>;
def : Pat<(int_aie2_vabs_gtz16 VEC512:$s1, 0x0), (VABS_GTZ_D16 VEC512:$s1)>;
def : Pat<(int_aie2_vabs_gtz32 VEC512:$s1, 0x0), (VABS_GTZ_D32 VEC512:$s1)>;
def : Pat<(int_aie2_vabs_gtz8  VEC512:$s1, 0x1), (VABS_GTZ_S8  VEC512:$s1)>;
def : Pat<(int_aie2_vabs_gtz16 VEC512:$s1, 0x1), (VABS_GTZ_S16 VEC512:$s1)>;
def : Pat<(int_aie2_vabs_gtz32 VEC512:$s1, 0x1), (VABS_GTZ_S32 VEC512:$s1)>;

// VADD
def : Pat<(int_aie2_add_acc ACC1024:$acc1, ACC1024:$acc2, eR:$c),
          (VADD ACC1024:$acc1, ACC1024:$acc2, eR:$c)>;

// VSUB
def : Pat<(int_aie2_sub_acc ACC1024:$acc1, ACC1024:$acc2, eR:$c),
          (VSUB ACC1024:$acc1, ACC1024:$acc2, eR:$c)>;

// VADD.f
def : Pat<(int_aie2_add_accfloat ACC512:$acc1, ACC512:$acc2, eR:$c),
          (VADD_F ACC512:$acc1, ACC512:$acc2, eR:$c)>;

// VSUB.f
def : Pat<(int_aie2_sub_accfloat ACC512:$acc1, ACC512:$acc2, eR:$c),
          (VSUB_F ACC512:$acc1, ACC512:$acc2, eR:$c)>;

// VNEGADD
def : Pat<(int_aie2_negadd_acc ACC1024:$acc1, ACC1024:$acc2, eR:$c),
          (VNEGADD ACC1024:$acc1, ACC1024:$acc2, eR:$c)>;

// VNEGSUB
def : Pat<(int_aie2_negsub_acc ACC1024:$acc1, ACC1024:$acc2, eR:$c),
          (VNEGSUB ACC1024:$acc1, ACC1024:$acc2, eR:$c)>;

// VNEGADD.f
def : Pat<(int_aie2_negadd_accfloat ACC512:$acc1, ACC512:$acc2, eR:$c),
          (VNEGADD_F ACC512:$acc1, ACC512:$acc2, eR:$c)>;

// VNEGSUB.f
def : Pat<(int_aie2_negsub_accfloat ACC512:$acc1, ACC512:$acc2, eR:$c),
          (VNEGSUB_F ACC512:$acc1, ACC512:$acc2, eR:$c)>;

// VCONV_BF16_FP32
def : Pat<(int_aie2_v16accfloat_to_v16bf16 ACC512:$src),
          (VCONV_BF16_FP32 ACC512:$src)>;

// VCONV_FP32_BF16
def : Pat<(int_aie2_v16bf16_to_v16accfloat VEC256:$src),
          (VCONV_FP32_BF16 VEC256:$src)>;

// SCHED_BARRIER
def : Pat<(int_aie2_sched_barrier), (SCHED_BARRIER)>;

// vector of bfloat16 to vector of int
def : Pat<(int_aie2_v16bf16_to_v16i32 VEC256:$src, mSs:$shft),
          (VFLOOR_S32_BF16_mFl2FxSrc_W VEC256:$src, mSs:$shft)>;

// VCLR_vclr_BF
def : Pat<(int_aie2_clr16f_conf),
          (VCLR_vclr_BF)>;
