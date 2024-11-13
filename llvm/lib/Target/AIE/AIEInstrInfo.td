//===-- AIEInstrInfo.td - Target Description for AIE ---*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file describes the AIEngine instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// AIEngine specific DAG Nodes.
//===----------------------------------------------------------------------===//

include "AIEBaseInstrInfo.td"

// Lock use_val flag
def uimm1 : txxu<1, i32>;
// index into vector register
// packet_header intrinsics
def t02u : txxu<2, i32>;
// index into vector register
// packet_header intrinsics
def t03u : txxu<3, i32>;
// index into vector register
def t04u : txxu<4, i32>;
// bitset/bitget index
def t05u : txxu<5, i32>;
// Lock IDs
def uimm6 : txxu<6, i32>;
// add_s_ri
def simm6 : txxs<6, iAny>;
// add immediate
def simm7 : txxs<7, iAny>;
// Jump addresses
def uimm20 : txxu<20, i32>;
def tuimm : txxu<20, i20>;
// Signed Immediates
def simm12 : txxs<12, iAny>;
// Signed Immediates
def simm20 : txxs<20, i20>;
def addr20 : Operand<OtherVT> { // , ImmLeaf<XLenVT, [{return isInt<20>(Imm);}]> {
// let ParserMatchClass = UImmAsmOperand<20>;
//  let EncoderMethod = "getImmOpValueAsr1";
  let DecoderMethod = "decodeSImmOperand<20>";
  let MCOperandPredicate = [{
    int64_t Imm;
    if (MCOp.evaluateAsConstantImm(Imm))
      return isInt<20>(Imm);
    return MCOp.isBareSymbolRef();
  }];
}
// Stack offset, user for stack indirect + immediate addressing.
// In the instructions, the immediate is multiplied by 32bits to get the stack offset.
def imm4x4 : immx4<4, i20>;
def imm5x32 : immx32<5>;

//post-modify offsets
//5..2 zero
def t06s_step4  : immx4<4, i32>;
//7..4 zero
def t08s_step16 : immx16<4>;
//8..5 zero
def t09s_step32 : immx32<4>;
//9..5 zero
def t10s_step32 : immx32<5>;

//stack indexed offsets
// These are always negative offsets from the stack pointer.
//one 6..0
def t08n : immx1_negative<7, i32>;
//one 8..2 zero
def t10n_step4 : immx4_negative<7, i32>;
def c10n_step4 : immx4_negative<7, i32>;
//one 15..2 zero
def t17n_step4 : immx4_negative<14, i32>;
//one 10..4 zero
def t12n_step16 : immx16_negative<7>;
//one 11..5 zero
def t13n_step32 : immx32_negative<7>;

// Pseudo-type, not an instruction operand
def simm32     : ImmLeaf<i32, [{return isInt<32>(Imm);}]>;
def immfp32    : ImmLeaf<f32, [{return true;}] >;

// There are lots of different register encodings in this
// architecture, so depending on the context a register is often
// encoded completely differently. As a result we need some specific
// code to encode registers correctly on a per-registerClass basis.
// (Note that decoding in the disassembler is done on a
// per-registerClass basis by default.)
class AIERegisterOperand<RegisterClass c> : RegisterOperand<c> {
  let EncoderMethod = "get" # c # OpValue;
}

// WARNING: These register classes contain registers of different
// sizes!  This case is not well handled in LLVM and can silently
// cause miscompilations if a 20-bit (PTR) register is chosen to store
// a value that doesn't fit!  Codegen patterns should use the
// more specialized *_GPR and *_PTR register classes below.
def OP_mMvScl   : AIERegisterOperand<mMvScl>;
def OP_mSclSt   : AIERegisterOperand<mSclSt>;
def OP_mLdaScl  : AIERegisterOperand<mLdaScl>;

let EncoderMethod = "get" # mMvScl # OpValue in {
def OP_mMvScl_GPR   : AIERegisterOperand<mRCm>;
def OP_mMvScl_PTR   : AIERegisterOperand<PTRMODCSCB>;
}
let EncoderMethod = "get" # mSclSt # OpValue in {
def OP_mSclSt_GPR   : AIERegisterOperand<mRCm>;
def OP_mSclSt_PTR   : AIERegisterOperand<mSclSt_PTR>;
def OP_mSclSt_MOD   : AIERegisterOperand<MOD>;
}
let EncoderMethod = "get" # mLdaScl # OpValue in {
def OP_mLdaScl_GPR   : AIERegisterOperand<mRCm>;
def OP_mLdaScl_PTR   : AIERegisterOperand<PTRMODCSCB>;
def OP_mLdaScl_MOD   : AIERegisterOperand<MOD>;
}

def OP_mLdbScl  : AIERegisterOperand<mLdbScl>;
def OP_mAluCg12 : AIERegisterOperand<mAluCg12>;
def OP_mMv0Cg20 : AIERegisterOperand<mMv0Cg20>;
def OP_mRCm     : AIERegisterOperand<mRCm>;
def OP_mWAv    : AIERegisterOperand<mWAv>;
def OP_mWABv   : AIERegisterOperand<mWABv>;
def OP_mWABDv   : AIERegisterOperand<mWABDv>;
def OP_mWBDv    : AIERegisterOperand<mWBDv>;
def OP_mWCDv    : AIERegisterOperand<mWCDv>;
def OP_mXABv    : AIERegisterOperand<mXABv>;
def OP_mXCDv    : AIERegisterOperand<mXCDv>;
//mVn, mVs, mVm, mVa
def OP_mVn      : AIERegisterOperand<mVn>;

// Extract least significant 20 bits from an immediate value.
def LO20 : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant(N->getZExtValue() & 0xfffff,
                                   SDLoc(N), N->getValueType(0));
}]>;

// Extract the most significant 12 bits from an immediate value.
def HI12 : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant(((N->getSExtValue()) >> 20),
                                   SDLoc(N), N->getValueType(0));
}]>;
def FPLO20 : SDNodeXForm<fpimm, [{
  const APInt &Imm = N->getValueAPF().bitcastToAPInt();
  assert(Imm.getBitWidth() == 32);
  return CurDAG->getTargetConstant(Imm.getZExtValue() & 0xfffff,
                                   SDLoc(N), MVT::i32);
}]>;
def FPHI12 : SDNodeXForm<fpimm, [{
  const APInt &Imm = N->getValueAPF().bitcastToAPInt();
  assert(Imm.getBitWidth() == 32);
  return CurDAG->getTargetConstant((Imm.getSExtValue() >> 20),
                                   SDLoc(N), MVT::i32);
}]>;

// Define some special parsing for symbols used in a call.
def CallSymbol : AsmOperandClass {
  let Name = "CallSymbol";
  let RenderMethod = "addImmOperands";
  let ParserMethod = "parseCallSymbol";
}

// A bare symbol used in call/tail only.
def call_symbol : Operand<i32> {
  let ParserMatchClass = CallSymbol;
}

include "AIEInstrFormats.td"
include "AIECompositeFormats.td"

// Patterns for Vector registers with different types.
def V16I8:   PatLeaf<(v16i8 VEC128:$R)>;
def V8I16:   PatLeaf<(v8i16 VEC128:$R)>;
def V4I32:   PatLeaf<(v4i32 VEC128:$R)>;
def V4F32:   PatLeaf<(v4f32 VEC128:$R)>;

/////////////////////////////////////////////////////////////////
// Basic operations

// Instructions for materializing constants
let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in {
let Itinerary = II_MOVI in {
let isReMaterializable = 1, isMoveImm = 1 in {
def MOV_U20 : AIE_mov20<(outs OP_mMv0Cg20:$reg), (ins uimm20:$imm), [], "mov.u20", "$reg, $imm">;
let isCodeGenOnly = 1 in {
def MOV_U20_I20 : AIE_mov20<(outs OP_mMv0Cg20:$reg), (ins tuimm:$imm), [], "mov.u20", "$reg, $imm">;
}
def MOV_S20 : AIE_mov20s<(outs GPR:$reg), (ins simm20:$imm), [], "mov.s20", "$reg, $imm">;
def MOVT_S12 : AIE_movu12<(outs OP_mRCm:$reg), (ins OP_mRCm:$src, simm12:$imm), [], "movt.s12", "$reg, $imm"> {
  let Constraints = "$src = $reg";
}
} // isRematerializable
} // Itinerary

let Itinerary = II_ALU, isMoveImm = 1 in
def MOV_S12 : AIE_cg_s12<(outs OP_mAluCg12:$dst), (ins simm12:$imm), [], "mov.s12", "$dst, $imm">;

let Itinerary = II_MOV, isMoveReg = 1 in {
def MV_SPECIAL2R : AIE_mv_special2r<(outs GPR:$d0), (ins SPR:$s0), [], "mov", "$d0, $s0">;
def MV_R2SPECIAL : AIE_mv_r2special<(outs SPR:$d0), (ins GPR:$s0), [], "mov", "$d0, $s0">;
} // Itinerary
} // hasSideEffects

let Itinerary = II_VMOV, isMoveReg = 1  in {
let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in {
def ACCUMULATOR_MOVE : AIE_Accumulator_mv_a<(outs ACC384:$d0), (ins ACC384:$s0), [], "vmov", "$d0, $s0">;
} // hasSideEffects
} // Itinerary


// Various convenience definitions
// A 32-bit constant
class Const32<int val> {
  dag DAG = (MOVT_S12 (MOV_U20 (i32 (LO20 (i32 val)))), (i32 (HI12 (i32 val))));
}
// A 20-bit constant
class Const20<int val> {
  dag DAG = (i32 (MOV_U20 (i32 (LO20 (i32 val)))));
}
class Const_v2i32_lo<int lo> {
	// Create a dag with v2i32 type with the low 32-bits specified
	// and the high 32 bits undefined.
	dag DAG = (INSERT_SUBREG (v2i32 (IMPLICIT_DEF)),
						Const32<lo>.DAG, sub_32_lo);
}
class Const_v2i32<int hi, int lo> {
	// Merge high 32-bits and low 32-bits defining a configuration register
	// together into a dag with v2i32 type.
	dag DAG = (INSERT_SUBREG (INSERT_SUBREG (v2i32 (IMPLICIT_DEF)),
					    Const32<hi>.DAG, sub_32_hi),
					    Const32<lo>.DAG, sub_32_lo);
}
class Make_v16i32<dag hi, dag lo> {
	dag DAG = (INSERT_SUBREG (INSERT_SUBREG (v16i32 (IMPLICIT_DEF)),
	      hi, sub_256bit_hi),
		  lo, sub_256bit_lo);
}
class Make_v16i32_lo<dag lo> {
	dag DAG = (INSERT_SUBREG (v16i32 (IMPLICIT_DEF)),
		  lo, sub_256bit_lo);
}
class Make_v32i32_lolo<dag lo> {
	dag DAG = (INSERT_SUBREG (v32i32 (IMPLICIT_DEF)),
				                 (INSERT_SUBREG (v16i32 (IMPLICIT_DEF)),
								                lo,
												sub_256bit_lo),
								  sub_512bit_lo);
}

let Itinerary = II_VUPDATE in {
let Constraints = "$dst = $vreg" in {
def S2V_SHIFTV0_R16 : AIE_s2v_shiftv0<0b0, (outs OP_mVn:$dst), (ins OP_mVn:$vreg, GPR:$reg), [], "vshl0.16", "$vreg, $reg">;
def S2V_SHIFTV0_R32 : AIE_s2v_shiftv0<0b1, (outs OP_mVn:$dst), (ins OP_mVn:$vreg, GPR:$reg), [], "vshl0.32", "$vreg, $reg">;
def S2V_SHIFTW0_R16 : AIE_s2v_shiftw0<0b0, (outs VEC256:$dst), (ins VEC256:$vreg, GPR:$reg), [], "vshl0.16", "$vreg, $reg">;
def S2V_SHIFTW0_R32 : AIE_s2v_shiftw0<0b1, (outs VEC256:$dst), (ins VEC256:$vreg, GPR:$reg), [], "vshl0.32", "$vreg, $reg">;
// def S2V_SHIFTV1_R16 : AIE_s2v_shiftv1<0b0, (outs VEC128:$dst), (ins VEC128:$vreg, GPR:$reg), [], "vshl1.16", "$vreg, $reg">;
// def S2V_SHIFTV1_R32 : AIE_s2v_shiftv1<0b1, (outs VEC128:$dst), (ins VEC128:$vreg, GPR:$reg), [], "vshl1.32", "$vreg, $reg">;
def S2V_UPD_R16 : AIE_s2v_upd_r16<(outs OP_mVn:$dst), (ins OP_mVn:$vreg, t03u:$idx, GPR:$reg), [], "vupd.16", "$vreg[$idx], $reg">;
def S2W_UPD_R16 : AIE_s2w_upd_r16<(outs VEC256:$dst), (ins VEC256:$vreg, t04u:$idx, GPR:$reg), [], "vupd.16_w", "$vreg[$idx], $reg">;
def S2X_UPD_R16 : AIE_s2x_upd_r16<(outs VEC512:$dst), (ins VEC512:$vreg, t05u:$idx, GPR:$reg), [], "vupd.16_x", "$vreg[$idx], $reg">;

def S2V_UPD_R32 : AIE_s2v_upd_r32<(outs OP_mVn:$dst), (ins OP_mVn:$vreg, t02u:$idx, GPR:$reg), [], "vupd.32", "$vreg[$idx], $reg">;
def S2W_UPD_R32 : AIE_s2w_upd_r32<(outs VEC256:$dst), (ins VEC256:$vreg, t02u:$idx, GPR:$reg), [], "vupd.32_w", "$vreg[$idx], $reg">;
def S2X_UPD_R32 : AIE_s2x_upd_r32<(outs VEC512:$dst), (ins VEC512:$vreg, t02u:$idx, GPR:$reg), [], "vupd.32_x", "$vreg[$idx], $reg">;
}
}

// def SPLIT : AIE_split<0, (outs mCS:$d0, GPR $d1), (ins GPR:$s, S4_only:$s4),
//     [], "split", "$d0, $d1, $s, $s4">;
// def SPLIT_DELAYED : AIE_split(1, (outs mCS:$d0, GPR $d1), (ins GPR:$s, S4_only:$s4))
//     [], "split.d7", "$d0, $d1, $s, $s4">;

let Itinerary = II_ALU in {
// FIXME: need mc
def BITGET : AIE_bitget<1, (outs GPR0_7:$dst), (ins GPR:$src, t05u:$idx),
	 [], "mov", "$dst, $src[$idx]">;
def BITSET : AIE_bitset<1, (outs GPR:$out), (ins GPR:$dst, t05u:$idx, GPR0_7:$src),
	 [], "mov", "$dst[$idx], $src[0]"> {
	 let Constraints = "$dst = $out";
}
def BITGET_M : AIE_bitget<0, (outs GPR0_7:$dst), (ins mMCMD:$src, t05u:$idx),
	 [], "mov", "$dst, $src[$idx]">;
def BITSET_M : AIE_bitset<0, (outs mMCMD:$out), (ins mMCMD:$dst, t05u:$idx, GPR0_7:$src),
	 [], "mov", "$dst[$idx], $src[0]"> {
	 let Constraints = "$dst = $out";
}

// Some variants which enable the stream intrinsics to work.
let isCodeGenOnly = 1 in {
def BITSET_MD0 : AIE_bitset<0, (outs MD0:$out), (ins t05u:$idx, GPR0_7:$src),
	 [], "mov", "md0[$idx], $src[0]"> {
	 let dst = 0b1100; // Encoding for MD0
}
def BITSET_MD1 : AIE_bitset<0, (outs MD1:$out), (ins t05u:$idx, GPR0_7:$src),
	 [], "mov", "md1[$idx], $src[0]"> {
	 let dst = 0b1101; // Encoding for MD1
}
def BITGET_MC0 : AIE_bitget<0, (outs GPR0_7:$dst), (ins t05u:$idx),
	 [], "mov", "$dst, mc0[$idx]"> {
	 let src = 0b1000; // Encoding for MC0
	 let Uses = [mc0];
}
def BITGET_MC1 : AIE_bitget<0, (outs GPR0_7:$dst), (ins t05u:$idx),
	 [], "mov", "$dst, mc1[$idx]"> {
	 let src = 0b1001; // Encoding for MC1
	 let Uses = [mc1];
}
}
}

def : Pat<(i32 (simm12:$imm)), (MOV_S12 imm:$imm)> { let AddedComplexity = -30; }
def : Pat<(i32 (uimm20:$imm)), (MOV_U20 imm:$imm)> { let AddedComplexity = -10; }
def : Pat<(i20 (tuimm:$imm)), (MOV_U20_I20 imm:$imm)> { let AddedComplexity = -10; }
def : Pat<(simm20:$imm), (MOV_S20 imm:$imm)> { let AddedComplexity = -20; }
// Materialize 32 bit signed constants require 2 instructions to materialize.
def : Pat<(i32 (simm32:$imm)), (MOVT_S12 (i32 (MOV_U20 (i32 (LO20 imm:$imm)))), (i32 (HI12 imm:$imm)))> { let AddedComplexity = -200; }
def : Pat<(fpimm:$imm), (MOVT_S12 (MOV_U20 (FPLO20 $imm)), (i32 (FPHI12 $imm)))> { let AddedComplexity = -200; }


// let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
// class BranchCC_rri<bits<3> funct3, string opcodestr>
//     : RVInstB<funct3, OPC_BRANCH, (outs),
//               (ins GPR:$rs1, GPR:$rs2, simm13_lsb0:$imm12),
//               opcodestr, "$rs1, $rs2, $imm12"> {
//   let isBranch = 1;
//   let isTerminator = 1;
// }

let Itinerary = II_ALU in {
def ADD      : AIE_alu_r_rr<0b0000, "add">;
def SUB      : AIE_alu_r_rr<0b0001, "sub">;
def ADC      : AIE_alu_r_rr<0b0010, "adc">;
def SBC      : AIE_alu_r_rr<0b0011, "sbc">;
def AND      : AIE_alu_r_rr<0b0100, "and">;
def OR       : AIE_alu_r_rr<0b0101, "or">;
def XOR      : AIE_alu_r_rr<0b0110, "xor">;
def MUL     : AIE_alu_r_rr<0b0111, "mul"> { let Itinerary = II_MUL; }
def EQ      : AIE_alu_r_rr<0b1000, "eq">;
def NE      : AIE_alu_r_rr<0b1001, "ne">;
def GE      : AIE_alu_r_rr<0b1010, "ge">;
def LT      : AIE_alu_r_rr<0b1011, "lt">;
def GEU     : AIE_alu_r_rr<0b1100, "geu">;
def LTU     : AIE_alu_r_rr<0b1101, "ltu">;
// Note: right shifts have the sign bit set.
def LSHL    : AIE_alu_r_rr<0b1110, "lshl">;
def ASHL    : AIE_alu_r_rr<0b1111, "ashl">;

def CLB      : AIE_alu_r_r<0b0000, "clb">;
def CLBU     : AIE_alu_r_r<0b0001, "clbu">;
def SE8      : AIE_alu_r_r<0b0010, "se.8">;
def SE16     : AIE_alu_r_r<0b0011, "se.16">;
def ZE8      : AIE_alu_r_r<0b0100, "ze.8">;
def ZE16     : AIE_alu_r_r<0b0101, "ze.16">;
def SEQ0     : AIE_alu_r_r<0b0110, "eqz">;
def SNE0     : AIE_alu_r_r<0b0111, "nez">;
def ABS      : AIE_alu_r_r<0b1000, "abs">;

def ADDI     : AIE_add_r_ri<(outs GPR:$d0), (ins GPR:$s0, simm7:$imm),
      [], "add", "$d0, $s0, $imm">;
def ADD_S_RI_ILV64     : AIE_add_s_ri<0b01, (outs mS:$d0), (ins GPR:$s0, simm6:$imm),
      [], "add.ilv.64", "$d0, $s0, $imm">;
} // Itinerary

let Itinerary = II_ALU in {
def ITE_EQZ : AIE_select_r_rr<0b0, "ite_eqz">;
def ITE_NEZ : AIE_select_r_rr<0b1, "ite_nez">;
}

let Itinerary = II_MOV, isMoveReg = 1 in {
def MOV : AIE_mv_scl<(outs OP_mMvScl:$dst), (ins OP_mMvScl:$src),
					 [], "mov", "$dst, $src">;
}
let Itinerary = II_MOV, isMoveReg = 1 in {
def MV_V : AIE_mv_vec<(outs VEC128:$dst), (ins VEC128:$src),
					 [], "vmov", "$dst, $src">;
def MV_W : AIE_mv_vecw<(outs VEC256:$dst), (ins VEC256:$src),
					 [], "vmov", "$dst, $src">;
def MV_X : AIE_mv_vecx<(outs VEC512:$dst), (ins VEC512:$src),
					 [], "vmov", "$dst, $src">;
def MV_AM : AIE_mv_vecam<(outs ACC384:$dst), (ins ACC384:$src),
					 [], "vmov", "$dst, $src">;
}

def : Pat<(i20 (trunc i32:$src)), (COPY_TO_REGCLASS $src, PTR)>;

let Itinerary = II_SRS in {
def MV_W_SRS0_48_srsl : AIE_mv_w_srs0_am<0b0, (outs VEC256:$dst/*, MC0:$modc*/), (ins ACC384:$src, mS:$shift, MD0:$mod),
					 [], "vmovs.48.srsl", "$dst, $src, $shift">;
def MV_W_SRS0_80_srsl : AIE_mv_w_srs0_am<0b1, (outs VEC256:$dst, MC0:$modc), (ins ACC384:$src, mS:$shift, MD0:$mod),
					 [], "vmovs.80.srsl", "$dst, $src, $shift">;
// def MV_W_SRS0_48_srss : AIE_mv_w_srs0_bm<0b0, (outs ACC384:$dst, MC0:$modc), (ins ACC384:$src, mS:$shift, MD0:$mod),
// 					 [], "vmovs.48.srss", "$dst, $src, $shift">;
def MV_W_SRS0_80_srss : AIE_mv_w_srs0_bm<0b1, (outs VEC256:$dst/*, MC0:$modc*/), (ins ACC768:$src, mS:$shift, MD0:$mod),
					 [], "vmovs.80.srss", "$dst, $src, $shift">;
def MV_W_SRS1_48 : AIE_mv_w_srs1<0b0, (outs ACC384:$dst/*, MC0:$modc*/), (ins ACC384:$src, S7:$shift, MD0:$mod),
					 [], "vmov1.48.srsl", "$dst, $src, s7">;
def MV_W_SRS1_80 : AIE_mv_w_srs1<0b1, (outs ACC384:$dst/*, MC0:$modc*/), (ins ACC384:$src, S7:$shift, MD0:$mod),
					 [], "vmov1.80.srsl", "$dst, $src, s7">;
}

let Itinerary = II_SRS in {
def MV_V_SRS0_48_srss : AIE_mv_v_srs0_am<0b0, (outs OP_mVn:$dst, MC0:$modc), (ins ACC384:$src, mS:$shift, MD0:$mod),
					 [], "vmovs.48.srss", "$dst, $src, $shift">;
def MV_V_SRS0_80_srss : AIE_mv_v_srs0_am<0b1, (outs OP_mVn:$dst, MC0:$modc), (ins ACC384:$src, mS:$shift, MD0:$mod),
					 [], "vmovs.80.srss", "$dst, $src, $shift">;
def MV_V_SRS0_48_srsb : AIE_mv_v_srs0_bm<0b1, (outs OP_mVn:$dst, MC0:$modc), (ins ACC768:$src, mS:$shift, MD0:$mod),
					 [], "vmovs.48.srsb", "$dst, $src, $shift">;
def MV_V_SRS0_48_srsbu : AIE_mv_v_srs0_bm<0b0, (outs OP_mVn:$dst, MC0:$modc), (ins ACC768:$src, mS:$shift, MD0:$mod),
					 [], "vmovs.48.srsbu", "$dst, $src, $shift">;
}

let Itinerary = II_STSRS in {
let Uses = [md0] in {
class VST_STS_SRS_ind_bm<bits<1> val, string srs>
    : AIE_dmv_sts_srs_bm<val, (outs), (ins ACC768:$sd, mS:$shft, PTR:$ptr), [], "vst.48."#srs#"", "$sd, $shft, [$ptr]">,  ag_short_ind;
class VST_STS_SRS_idx_bm<bits<1> val, string srs>
    : AIE_dmv_sts_srs_bm<val, (outs), (ins ACC768:$sd, mS:$shft, PTR:$ptr, mCS:$cs), [], "vst.48."#srs#"", "$sd, $shft, [$ptr, $cs]">,  ag_short_idx;
def VST_SRSU_ind : VST_STS_SRS_idx_bm<0b0, "srsbu">;
def VST_SRSS_ind : VST_STS_SRS_idx_bm<0b1, "srsb">;
def VST_SRSU_idx : VST_STS_SRS_ind_bm<0b0, "srsbu">;
def VST_SRSS_idx : VST_STS_SRS_ind_bm<0b1, "srsb">;

let Uses = [SP] in {
let Defs = [SP] in {
class VST_STS_SRS_sp_imm_bm<bits<1> val, string srs>
    : AIE_dmv_sts_srs_bm<val, (outs), (ins ACC768:$sd, mS:$shft, imm5x32:$imm), [], "vst.48."#srs#"", "$sd, $shft, [sp], $imm">, ag_short_pstm_sp_imm;
} // Defs SP
class VST_STS_SRS_spis_bm<bits<1> val, string srs>
    : AIE_dmv_sts_srs_bm<val, (outs  MC0:$modc), (ins ACC768:$sd, mS:$shft, t12n_step16:$imm), [], "vst.48."#srs#"", "$sd, $shft, [sp, $imm]">, ag_short_pstm_spis;
def VST_SRSU_sp_imm : VST_STS_SRS_sp_imm_bm<0b0, "srsbu">;
def VST_SRSS_sp_imm : VST_STS_SRS_sp_imm_bm<0b1, "srsb">;
def VST_SRSS_spis : VST_STS_SRS_spis_bm<0b1, "srsb">;
def VST_SRSU_spis : VST_STS_SRS_spis_bm<0b0, "srsbu">;
} // Uses SP
} // Uses md0
} // Itinerary II_STSRS

//bsrs
def : Pat<(int_aie_bsrs_v16i8_v16acc48 ACC768:$acc, GPR:$shft),
          (MV_V_SRS0_48_srsb ACC768:$acc, (ADD_S_RI_ILV64 GPR:$shft, (i32 1)), (IMPLICIT_DEF))>;
def : Pat<(int_aie_ubsrs_v16i8_v16acc48 ACC768:$acc, GPR:$shft),
          (MV_V_SRS0_48_srsbu ACC768:$acc, (ADD_S_RI_ILV64 GPR:$shft, (i32 1)), (IMPLICIT_DEF))>;

let Itinerary = II_VEXTRACT in {
def S2V_EXT_R32 : AIE_s2v_ext_r32<(outs GPR:$d0), (ins OP_mVn:$s0, t02u:$idx), [], "vext.32", "$d0, $s0[$idx]">;
def S2W_EXT_R32 : AIE_s2w_ext_r32<(outs GPR:$d0), (ins VEC256:$s0, t03u:$idx), [], "vext.32_w", "$d0, $s0[$idx]">;
def S2X_EXT_R32 : AIE_s2x_ext_r32<(outs GPR:$d0), (ins VEC512:$s0, t04u:$idx), [], "vext.32_x", "$d0, $s0[$idx]">;

def S2V_EXT_P32 : AIE_s2v_ext_p32<(outs PTR:$d0), (ins OP_mVn:$s0, t02u:$idx), [], "vext.32", "$d0, $s0[$idx]">;
def S2W_EXT_P32 : AIE_s2w_ext_p32<(outs PTR:$d0), (ins VEC256:$s0, t03u:$idx), [], "vext.32_w", "$d0, $s0[$idx]">;
def S2X_EXT_P32 : AIE_s2x_ext_p32<(outs PTR:$d0), (ins VEC512:$s0, t04u:$idx), [], "vext.32_x", "$d0, $s0[$idx]">;

def S2V_EXT_R16 : AIE_s2v_ext_r16<(outs GPR:$d0), (ins OP_mVn:$s0, t03u:$idx), [], "vext.16", "$d0, $s0[$idx]">;
def S2W_EXT_R16 : AIE_s2w_ext_r16<(outs GPR:$d0), (ins VEC256:$s0, t04u:$idx), [], "vext.16_w", "$d0, $s0[$idx]">;
def S2X_EXT_R16 : AIE_s2x_ext_r16<(outs GPR:$d0), (ins VEC512:$s0, t05u:$idx), [], "vext.16_x", "$d0, $s0[$idx]">;
}


def : Pat<(extractelt (v4i32 VEC128:$src),  t02u:$lane),
         (S2V_EXT_R32 (v4i32 VEC128:$src),  t02u:$lane)>;
def : Pat<(extractelt (v8i32 VEC256:$src),  t03u:$lane),
         (S2W_EXT_R32 (v8i32 VEC256:$src),  t03u:$lane)>;
def : Pat<(extractelt (v16i32 VEC512:$src), t04u:$lane),
         (S2X_EXT_R32 (v16i32 VEC512:$src), t04u:$lane)>;
def : Pat<(extractelt (v4f32 VEC128:$src),  t02u:$lane),
         (S2V_EXT_R32 (v4f32 VEC128:$src),  t02u:$lane)>;
def : Pat<(extractelt (v8f32 VEC256:$src),  t03u:$lane),
         (S2W_EXT_R32 (v8f32 VEC256:$src),  t03u:$lane)>;
def : Pat<(extractelt (v16f32 VEC512:$src), t04u:$lane),
         (S2X_EXT_R32 (v16f32 VEC512:$src), t04u:$lane)>;
// FIXME: add patterns for s2v_ext_p32?

// We need to use vector_extract here, as it allows extractions to implicitly extended the vector element
// extractelt requires the destination type to be exactly the vector element type
def : Pat<(i32 (vector_extract (v8i16 VEC128:$src),  t03u:$lane)),
         (S2V_EXT_R16 (v8i16 VEC128:$src),  t03u:$lane)>;
def : Pat<(i32 (vector_extract (v16i16 VEC256:$src), t04u:$lane)),
         (S2W_EXT_R16 (v16i16 VEC256:$src), t04u:$lane)>;
def : Pat<(i32 (vector_extract (v32i16 VEC512:$src), t05u:$lane)),
         (S2X_EXT_R16 (v32i16 VEC512:$src), t05u:$lane)>;


def : Pat<(insertelt (v4i32 VEC128:$src),  (i32 GPR:$reg), t02u:$lane),
         (v4i32 (S2V_UPD_R32 (v4i32 VEC128:$src), t02u:$lane, (i32 GPR:$reg)))>;
def : Pat<(insertelt (v8i32 VEC256:$src),  (i32 GPR:$reg), t03u:$lane),
         (v8i32 (S2W_UPD_R32 (v8i32 VEC256:$src), t03u:$lane, (i32 GPR:$reg)))>;
def : Pat<(insertelt (v16i32 VEC512:$src),  (i32 GPR:$reg), t04u:$lane),
         (v16i32 (S2X_UPD_R32 (v16i32 VEC512:$src), t04u:$lane, (i32 GPR:$reg)))>;
def : Pat<(insertelt (v4f32 VEC128:$src),  (f32 GPR:$reg), t02u:$lane),
         (v4f32 (S2V_UPD_R32 (v4f32 VEC128:$src), t02u:$lane, (f32 GPR:$reg)))>;
def : Pat<(insertelt (v8f32 VEC256:$src),  (f32 GPR:$reg), t03u:$lane),
         (v8f32 (S2W_UPD_R32 (v8f32 VEC256:$src), t03u:$lane, (f32 GPR:$reg)))>;
def : Pat<(insertelt (v16f32 VEC512:$src),  (f32 GPR:$reg), t04u:$lane),
         (v16f32 (S2X_UPD_R32 (v16f32 VEC512:$src), t04u:$lane, (f32 GPR:$reg)))>;


def : Pat<(v8i16 (vector_insert (v8i16 VEC128:$src), (i32 GPR:$reg), t03u:$lane)),
		(v8i16 (S2V_UPD_R16 (v8i16 VEC128:$src), t03u:$lane, (i32 GPR:$reg)))>;
def : Pat<(v16i16 (vector_insert (v16i16 VEC256:$src), (i32 GPR:$reg), t04u:$lane)),
		(v16i16 (S2W_UPD_R16 (v16i16 VEC256:$src), t04u:$lane, (i32 GPR:$reg)))>;
def : Pat<(v32i16 (vector_insert (v32i16 VEC512:$src), (i32 GPR:$reg), t05u:$lane)),
		(v32i16 (S2X_UPD_R16 (v32i16 VEC512:$src), t05u:$lane, (i32 GPR:$reg)))>;

// Intrinsics
let Itinerary = II_EVENT in
def EVENT : AIE_event<"event">;
def : Pat<(int_aie_event t02u:$val), (EVENT t02u:$val)>;

let Itinerary = II_LOCK in {
def ACQ_reg : AIE_lock_reg<0b0, "acq">;
def ACQ_imm : AIE_lock_imm<0b0, "acq">;
def REL_reg : AIE_lock_reg<0b1, "rel">;
def REL_imm : AIE_lock_imm<0b1, "rel">;
}
let Itinerary = II_PKTHD in {
def GEN_PCKT_HEADER_reg0 : AIE_gen_pckt_header_reg<0b0, "PKTHD", "MS.md0[10]">;
def GEN_PCKT_HEADER_reg1 : AIE_gen_pckt_header_reg<0b1, "PKTHD", "MS.md0[11]">;
def GEN_PCKT_HEADER_imm0 : AIE_gen_pckt_header_imm<0b0, "PKTHD", "MS.md0[10]">;
def GEN_PCKT_HEADER_imm1 : AIE_gen_pckt_header_imm<0b1, "PKTHD", "MS.md0[11]">;
}
let Itinerary = II_CPKTHD in {
def GEN_CTRL_PCKT_HEADER_reg0 : AIE_gen_ctrl_pckt_header_reg<0b0, "CPKTHD", "MS.md0[10]">;
def GEN_CTRL_PCKT_HEADER_reg1 : AIE_gen_ctrl_pckt_header_reg<0b1, "CPKTHD", "MS.md0[11]">;
def GEN_CTRL_PCKT_HEADER_imm0 : AIE_gen_ctrl_pckt_header_imm<0b0, "CPKTHD", "MS.md0[10]">;
def GEN_CTRL_PCKT_HEADER_imm1 : AIE_gen_ctrl_pckt_header_imm<0b1, "CPKTHD", "MS.md0[11]">;
}
let Itinerary = II_SCL2MS in {
def MV_SCL2MSA_reg0 : AIE_mv_scl2msa_reg<0b0, 0b0, "movs", "MS.md0[10]">;
def MV_SCL2MSA_reg1 : AIE_mv_scl2msa_reg<0b1, 0b0, "movs",
 "MS.md0[11]">;
def MV_SCL2MSA_imm0 : AIE_mv_scl2msa_imm<0b0, 0b0, "movs", "MS.md0[10]">;
def MV_SCL2MSA_imm1 : AIE_mv_scl2msa_imm<0b1, 0b0, "movs", "MS.md0[11]">;
def MV_SCL2MSANB_reg0 : AIE_mv_scl2msa_reg<0b0, 0b1, "movs", "MS.md0[10].nb">;
def MV_SCL2MSANB_reg1 : AIE_mv_scl2msa_reg<0b1, 0b1, "movs", "MS.md0[11].nb">;
def MV_SCL2MSANB_imm0 : AIE_mv_scl2msa_imm<0b0, 0b1, "movs", "MS.md0[10].nb">;
def MV_SCL2MSANB_imm1 : AIE_mv_scl2msa_imm<0b1, 0b1, "movs", "MS.md0[11].nb">;
def MV_SCL2MSB_reg0 : AIE_mv_scl2msb_reg<0b0, 0b0, "mov1", "MS.md0[10]">;
def MV_SCL2MSB_reg1 : AIE_mv_scl2msb_reg<0b1, 0b0, "mov1", "MS.md0[11]">;
def MV_SCL2MSB_imm0 : AIE_mv_scl2msb_imm<0b0, 0b0, "mov1", "MS.md0[10]">;
def MV_SCL2MSB_imm1 : AIE_mv_scl2msb_imm<0b1, 0b0, "mov1", "MS.md0[11]">;
def MV_SCL2MSBNB_reg0 : AIE_mv_scl2msb_reg<0b0, 0b1, "mov1", "MS.md0[10].nb">;
def MV_SCL2MSBNB_reg1 : AIE_mv_scl2msb_reg<0b1, 0b1, "mov1", "MS.md0[11].nb">;
def MV_SCL2MSBNB_imm0 : AIE_mv_scl2msb_imm<0b0, 0b1, "mov1", "MS.md0[10].nb">;
def MV_SCL2MSBNB_imm1 : AIE_mv_scl2msb_imm<0b1, 0b1, "mov1", "MS.md0[11].nb">;
}

let Itinerary = II_SS2SCL in {
def MV_SSA2SCL_0 : AIE_mv_ssa2scl<0b0, 0b0, "mov0", "SS.md0[8]">;
def MV_SSA2SCL_1 : AIE_mv_ssa2scl<0b1, 0b0, "mov0", "SS.md0[9]">;
def MV_SSA2SCLNB_0 : AIE_mv_ssa2scl<0b0, 0b1, "mov0", "SS.md0[8].nb">;
def MV_SSA2SCLNB_1 : AIE_mv_ssa2scl<0b1, 0b1, "mov0", "SS.md0[9].nb">;
def MV_SSB2SCL_0 : AIE_mv_ssb2scl<0b0, 0b0, "mov1", "SS.md0[8]">;
def MV_SSB2SCL_1 : AIE_mv_ssb2scl<0b1, 0b0, "mov1", "SS.md0[9]">;
def MV_SSB2SCLNB_0 : AIE_mv_ssb2scl<0b0, 0b1, "mov1", "SS.md0[8].nb">;
def MV_SSB2SCLNB_1 : AIE_mv_ssb2scl<0b1, 0b1, "mov1", "SS.md0[9].nb">;
}

/// Generic pattern classes
class PatGpr<SDPatternOperator OpNode, AIEInst Inst>
    : Pat<(i32 (OpNode (i32 GPR:$rs))), (Inst GPR:$rs)>;

class PatGprGpr<SDPatternOperator OpNode, AIEInst Inst, ValueType type>
    : Pat<(type (OpNode (type GPR:$rs1), (type GPR:$rs2))), (Inst GPR:$rs1, GPR:$rs2)>;

multiclass ALUPatterns<ValueType type> {
def : PatGprGpr<add, ADD, type>;
def : PatGprGpr<sub, SUB, type>;
def : PatGprGpr<and, AND, type>;
def : PatGprGpr<or, OR, type>;
def : PatGprGpr<xor, XOR, type>;
def : PatGprGpr<mul, MUL, type>;
def : Pat<(type (add (type GPR:$s0), simm7:$imm)), (ADDI GPR:$s0, simm7:$imm)>;
}

defm : ALUPatterns<i32>;
defm : ALUPatterns<i20>;

// 32-bit permute configurations, used for simplified instructions
def PermuteConf32 {
    // Wire 8 lanes straight through.
	dag DAGnone8 = 	Const32<0x76543210>.DAG;
}
// 64-bit permute configurations, used for general instructions
def PermuteConf {
	// Wire 16 lanes straight through.
	dag DAGnone16 = Const_v2i32<0xFEDCBA98, 0x76543210>.DAG;
	// Wire 8 lanes straight through, high 8 lanes don't care.
	dag DAGnone8 = 	Const_v2i32_lo<0x76543210>.DAG;
	// Select high 8 lanes in low 8 lanes, high 8 lanes don't care.
	dag DAGnonehi8 = Const_v2i32_lo<0xFEDCBA98>.DAG;
    // Replicate low 8 lanes into high 8 lanes.
	dag DAGnone88 = Const_v2i32<0x76543210, 0x76543210>.DAG;
	// Wire 16 lanes straight through.
	dag DAGnonehi8lo8 = Const_v2i32<0xFEDCBA98, 0x76543210>.DAG;
    // Swap low 8 and high 8 lanes.
	dag DAGnonelo8hi8 = Const_v2i32<0x76543210, 0xFEDCBA98>.DAG;
}
class VIntConf {
  int pra_nrm      =  0;
  int pra_lt       =  1;
  int pra_ge       =  2;
  int pra_sel      =  3;
  int pra_min      =  4;
  int pra_max      =  5;
  int pra_abs      =  6;
  int pra_maxdiff  =  7;
  int pra_add      =  8;
  int pra_sub      =  9;
  int pra_nrm_8bit =  10;

// VCMP can perform vector compare, maximum, add, sub, depending on the configuration.
// Configuration bits below
  bits<6> xstep     = 0; // c[5:0] X Step (6 bits)
  bits<1> butterfly = 0; // c[6] Butterfly mode (1 bit, use 1 to use mode)
  bits<1> swapz     = 0; // c[7] Swap Z (1 bit, use 1 to use mode)
  bits<6> zstep     = 0; // c[13:8] Z step (6 bits)
  bits<1> self_mac  = 1; // c[14] Self-vector accumulate (1 bit, use 1 to use mode)
  bits<1> symmetric = 1; // c[15] Symmetric mode (1 bit, use 1 to use mode)

  bits<1> anti_sym  = 1; // c[16] Anti-symmetric mode (1 bit, use 1 to use mode)
  bits<1> xcj       = 0; // c[17] Conjugate X (1 bit, use 1 to use mode)
  bits<1> zcj       = 0; // c[18] Conjugate Z (1 bit, use 1 to use mode)
  bits<1> two_buf   = 1; // c[19] Two buffer mode (1 bit, use 1 to use mode)
  bits<1> small_buf = 1; // c[20] Small buffer mode (1 bit, use 1 to use mode)
  bits<1> splitacc  = 0; // c[21] Split accumulator mode (1 bit, use 1 to use mode)
  bits<1> conj_sym  = 0; // c[22] Conjugate symmetric mode (1 bit, use 1 to use mode)
  bits<4> pra_mode  = 0; // c[26:23] Vector Compare PRA mode (4 bits)
  bits<5> with_ctap = 0; // c[31:27] Center Tap (5 bits)

  bits<8> xperm     = 0; // c[39:32] Permutations in X (8 bits)
  bits<8> zperm     = 0; // c[47:40] Permutations in Z (8 bits)
  bits<1> no_mul    = 1; // c[48] No multiplication mode (1 bit, use 1 to use mode)
  bits<1> msc       = 0; // c[49] Multiply and subtract mode (1 bit, use 1 to use mode)

  bits<18> hi = {msc, no_mul, xperm, xperm};
  bits<32> lo = {with_ctap, pra_mode, conj_sym, splitacc, small_buf, two_buf, zcj, xcj, anti_sym,
    symmetric, self_mac, zstep, swapz, butterfly, xstep};
  dag DAG = Const_v2i32<hi, lo>.DAG;
}

def lt_conf : VIntConf { let pra_mode = pra_lt; }
def ge_conf : VIntConf { let pra_mode = pra_ge;}
def eq_conf : VIntConf { let pra_mode = pra_ge; let two_buf = 0;}
def ne_conf : VIntConf { let pra_mode = pra_lt; let two_buf = 0;}
def min_conf : VIntConf { let pra_mode = pra_min; }
def max_conf : VIntConf { let pra_mode = pra_max;}
def add_conf : VIntConf { let anti_sym = 0; let two_buf = 0; let small_buf = 0;}
def sub_conf : VIntConf { let xcj = 1;}
def mac_conf : VIntConf { let two_buf = 0; let small_buf = 0; let self_mac = 0; let symmetric = 0;}
def mul_conf : VIntConf { let two_buf = 0; let small_buf = 0; let self_mac = 0; let symmetric = 0; let no_mul = 0;}

class VFPConf {
  int lt       =  0;
  int ge       =  1;
  int nrm      =  2;

  bits<19> reserved = 0;
  bits<1> abs       = 0;  // fpconf[19]=abs(If true the absolute value is taken before accumulation)
  bits<1> reserved2 = 1;
  bits<1> ones      = 0;  // fpconf[21]=ones (If true all lanes from ‘sc’ are replaced with 1.0)
  bits<2> mode      = 0;  // fpconf[23:22]=compare mode (lt=0,ge=1,nrm=2)
  bits<8> addmask   = 0;  // fpconf[31:24]=addmask (8 x 1 LSB bits: Corresponding lane is negated if bit is set (depending on fpaddmode)).

  bits<32> all = {addmask, mode, ones, reserved2, abs, reserved};
  dag DAG = Const32<all>.DAG;
}
def lt_fpconf     : VFPConf { let ones = 1; let mode = lt;  } // 0x300000
def ge_fpconf     : VFPConf { let ones = 1; let mode = ge;  } // 0x700000
def mul_fpconf    : VFPConf { let ones = 0; let mode = nrm; } // 0x900000
def addsub_fpconf : VFPConf { let ones = 1; let mode = nrm; } // 0xB00000
def abs_fpconf    : VFPConf { let ones = 1; let mode = nrm; let abs = 1; } // 0xB80000

let Itinerary = II_VFMUL in
def VFPMUL : AIE_vec_float_noacc<(outs OP_mWBDv:$d0, GPR0_7:$d1),
								 (ins VEC1024:$sx, GPR:$fpxstart, mCL:$fpxoffs,
                                      VECFWC:$sc, t03u:$fpzstart, mCL:$fpzoffs,
									  t02u:$fpaddmode, mCL:$fpconf), [], "vfpmul",
    "$d0, $d1, $sx, $fpxstart, $fpxoffs, $sc, $fpzstart, $fpzoffs, $fpaddmode, $fpconf">;
let Itinerary = II_VFMAC in
def VFPMAC : AIE_vec_float_acc<(outs OP_mWBDv:$d0, GPR0_7:$d1),
								 (ins OP_mWBDv:$acc, VEC1024:$sx, GPR:$fpxstart, mCL:$fpxoffs,
                                      VECFWC:$sc, t03u:$fpzstart, mCL:$fpzoffs,
									  t02u:$fpaddmode, mCL:$fpconf), [], "vfpmac",
    "$d0, $d1, $acc, $sx, $fpxstart, $fpxoffs, $sc, $fpzstart, $fpzoffs, $fpaddmode, $fpconf">;

let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in {
let Itinerary = II_VFMUL in
def VFPMUL256 : Pseudo<(outs OP_mWBDv:$d0, GPR0_7:$d1),
								 (ins OP_mWABDv:$sx, mCL:$fpxoffs,
                                      VECFWC:$sc, mCL:$fpzoffs,
									  t02u:$fpaddmode, mCL:$fpconf), [], "vfpmul256",
    "$d0, $d1, $sx, $fpxoffs, $sc, $fpzoffs, $fpaddmode, $fpconf">;
let Itinerary = II_VFMAC in
def VFPMAC256 : Pseudo<(outs OP_mWBDv:$d0, GPR0_7:$d1),
								 (ins OP_mWBDv:$acc, OP_mWABDv:$sx, mCL:$fpxoffs,
                                      VECFWC:$sc, mCL:$fpzoffs,
									  t02u:$fpaddmode, mCL:$fpconf), [], "vfpmac256",
    "$d0, $d1, $acc, $sx, $fpxoffs, $sc, $fpzoffs, $fpaddmode, $fpconf">;
// Same as above, but with the output swapped, making it easy to get at the second output.
// This is a simple way of implementing these instructions without resorting to explicit
// code.
let Itinerary = II_VFMUL in
def VFPMUL256GPR : Pseudo<(outs GPR0_7:$d1, OP_mWBDv:$d0),
								 (ins OP_mWABDv:$sx, mCL:$fpxoffs,
                                      VECFWC:$sc, mCL:$fpzoffs,
									  t02u:$fpaddmode, mCL:$fpconf), [], "vfpmul256gpr",
    "$d1, $d0, $sx, $fpxoffs, $sc, $fpzoffs, $fpaddmode, $fpconf">;
let Itinerary = II_VFMAC in
def VFPMAC256GPR : Pseudo<(outs GPR0_7:$d1, OP_mWBDv:$d0),
								 (ins OP_mWBDv:$acc, OP_mWABDv:$sx, mCL:$fpxoffs,
                                      VECFWC:$sc, mCL:$fpzoffs,
									  t02u:$fpaddmode, mCL:$fpconf), [], "vfpmac256gpr",
    "$d1, $d0, $acc, $sx, $fpxoffs, $sc, $fpzoffs, $fpaddmode, $fpconf">;
}

// Expose the vector instructions as intrinsics.
def : Pat<(int_aie_v4f32undef), (v4f32 (IMPLICIT_DEF))>;
def : Pat<(int_aie_v8f32undef), (v8f32 (IMPLICIT_DEF))>;
def : Pat<(int_aie_v16f32undef), (v16f32 (IMPLICIT_DEF))>;
def : Pat<(int_aie_v32f32undef), (v32f32 (IMPLICIT_DEF))>;
def : Pat<(int_aie_v2i32undef), (v2i32 (IMPLICIT_DEF))>;
def : Pat<(int_aie_v8i48undef), (v8i48 (IMPLICIT_DEF))>;
def : Pat<(int_aie_v16i48undef), (v16i48 (IMPLICIT_DEF))>;
def : Pat<(int_aie_v16i16undef), (v16i16 (IMPLICIT_DEF))>;
def : Pat<(int_aie_v32i16undef), (v32i16 (IMPLICIT_DEF))>;
def : Pat<(int_aie_v16i32undef), (v16i32 (IMPLICIT_DEF))>;
def : Pat<(int_aie_v32i32undef), (v32i32 (IMPLICIT_DEF))>;
def : Pat<(int_aie_v128i8undef), (v128i8 (IMPLICIT_DEF))>;

def : Pat<(int_aie_vfpmul VEC1024:$sx, GPR:$fpxstart, mCL:$fpxoffs,
                          VECFWC:$sc, t03u:$fpzstart, mCL:$fpzoffs,
						  t02u:$fpaddmode, mCL:$fpconf),
          (VFPMUL VEC1024:$sx, GPR:$fpxstart, mCL:$fpxoffs,
					VECFWC:$sc, t03u:$fpzstart, mCL:$fpzoffs,
					t02u:$fpaddmode, mCL:$fpconf)>;

def : Pat<(int_aie_vfpmac (v8f32 OP_mWBDv:$acc),
                          VEC1024:$sx, GPR:$fpxstart, mCL:$fpxoffs,
                          VECFWC:$sc, t03u:$fpzstart, mCL:$fpzoffs,
						  t02u:$fpaddmode, mCL:$fpconf),
          (VFPMAC (v8f32 OP_mWBDv:$acc),
		          VEC1024:$sx, GPR:$fpxstart, mCL:$fpxoffs,
					VECFWC:$sc, t03u:$fpzstart, mCL:$fpzoffs,
					t02u:$fpaddmode, mCL:$fpconf)>;

def : Pat<(int_aie_vfpsimplemul (v8f32 OP_mWABDv:$r1), VECFWC:$r2, t02u:$fpaddmode, mCL:$fpconf),
          (VFPMUL256 $r1,
			      PermuteConf32.DAGnone8,
				  $r2,
				  PermuteConf32.DAGnone8,
                  t02u:$fpaddmode, mCL:$fpconf)>;

def : Pat<(int_aie_vfpsimplemac OP_mWBDv:$acc,
                                OP_mWABDv:$r1, VECFWC:$r2, t02u:$fpaddmode, mCL:$fpconf),
          (VFPMAC256 OP_mWBDv:$acc, $r1,
			      PermuteConf32.DAGnone8,
                  $r2,
				  PermuteConf32.DAGnone8,
                  t02u:$fpaddmode, mCL:$fpconf)>;

// Note that we only select into one possible input lane.  This lane is chosen to reduce
// the register conflicts with other operands to the this instruction, simplifying
// register allocation.  Ideally this should be generalized, but it means we need
// a different instruction encoding that is less consistent with the instruction itself.

// Perform a vector multiply, with no addition.
class VecFloatNoACCIntrinsicPat<SDNode opcode, int fpaddmode, VFPConf fpconf, ValueType type>
    : Pat<(opcode (type OP_mWABDv:$r1), (type VECFWC:$r2)),
          (VFPMUL256 $r1,
			      PermuteConf32.DAGnone8,
                  $r2,
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf.DAG)>;

// Perform a vector add operation, with no multiply.
class VecFloatACCIntrinsicPat<SDNode opcode, int fpaddmode, VFPConf fpconf, ValueType type>
    : Pat<(opcode (type OP_mWBDv:$r1), (type VEC256:$r2)),
          (VFPMAC256 $r1, $r2,
			      PermuteConf32.DAGnone8,
                  (IMPLICIT_DEF),
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf.DAG)>;

// compute a floating point MAC
class VecFloatMACPat<int fpaddmode, VFPConf fpconf, ValueType type>
    : Pat<(fadd (type VEC256:$r1), (fmul (type OP_mWBDv:$r2), (type VECFWC:$r3))),
          (VFPMAC256 $r1, $r2,
			      PermuteConf32.DAGnone8,
                  $r3,
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf.DAG)>;
// A-(B*C) is possible, (B*C)-A is not directly possible
class VecFloatMSCPat<int fpaddmode, VFPConf fpconf, ValueType type>
    : Pat<(fsub (type VEC256:$r1), (fmul (type OP_mWBDv:$r2), (type VECFWC:$r3))),
          (VFPMAC256 $r1, $r2,
			      PermuteConf32.DAGnone8,
                  $r3,
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf.DAG)>;


// Perform a simple comparison, returning the result in a GPR with the given RType
class VecFloatCMPIntrinsicPat<PatFrags opcode, int fpaddmode, VFPConf fpconf, ValueType Rtype, ValueType type>
    : Pat<(Rtype (opcode (type OP_mWBDv:$r1), (type VEC256:$r2))),
          (VFPMAC256GPR $r1, $r2,
			      PermuteConf32.DAGnone8,
                  (IMPLICIT_DEF),
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf.DAG)>;

// Perform a simple comparison, with the inputs swapped
class VecFloatCMPIntrinsicPatSwap<PatFrags opcode, int fpaddmode, VFPConf fpconf, ValueType Rtype, ValueType type>
    : Pat<(Rtype (opcode (type OP_mWBDv:$r1), (type VEC256:$r2))),
          (VFPMAC256GPR $r2, $r1,
			      PermuteConf32.DAGnone8,
                  (IMPLICIT_DEF),
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf.DAG)>;

// Perform 2 compares, with the inputs swapped, then combine the results with 'combine'
class VecFloatCMPIntrinsicPatComb<PatFrags opcode, int fpaddmode, VFPConf fpconf, ValueType Rtype, ValueType type, AIEInst combine>
    : Pat<(Rtype (opcode (type OP_mWBDv:$r1), (type VEC256:$r2))),
          (combine
		  (VFPMAC256GPR $r1, $r2,
			      PermuteConf32.DAGnone8,
                  (IMPLICIT_DEF),
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf.DAG),
          (VFPMAC256GPR $r2, $r1,
			      PermuteConf32.DAGnone8,
                  (IMPLICIT_DEF),
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf.DAG))>;

// Perform 2 compares, with 2 different floating point ops, then combine the results with 'combine'
class VecFloatCMPIntrinsicPatCombOrd<PatFrags opcode, int fpaddmode, VFPConf fpconf1, VFPConf fpconf2, ValueType Rtype, ValueType type, AIEInst combine>
    : Pat<(Rtype (opcode (type OP_mWBDv:$r1), (type VEC256:$r2))),
          (combine
		  (VFPMAC256GPR $r1, $r2,
			      PermuteConf32.DAGnone8,
                  (IMPLICIT_DEF),
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf1.DAG),
          (VFPMAC256GPR $r1, $r2,
			      PermuteConf32.DAGnone8,
                  (IMPLICIT_DEF),
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf2.DAG))>;

class VecFloatCMPIntrinsicPatCombNotOrd<PatFrags opcode, int fpaddmode, VFPConf fpconf1, VFPConf fpconf2, ValueType Rtype, ValueType type, AIEInst combine>
    : Pat<(Rtype (opcode (type OP_mWBDv:$r1), (type VEC256:$r2))),
          (XOR
		  (combine
		  (VFPMAC256GPR $r1, $r2,
			      PermuteConf32.DAGnone8,
                  (IMPLICIT_DEF),
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf1.DAG),
          (VFPMAC256GPR $r1, $r2,
			      PermuteConf32.DAGnone8,
                  (IMPLICIT_DEF),
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf2.DAG)),
		  (combine
		  (VFPMAC256GPR $r1, $r2,
			      PermuteConf32.DAGnone8,
                  (IMPLICIT_DEF),
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf1.DAG),
          (VFPMAC256GPR $r1, $r2,
			      PermuteConf32.DAGnone8,
                  (IMPLICIT_DEF),
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf2.DAG)))>;

class VecFloatUnaryIntrinsicPat<SDNode opcode, int fpaddmode, VFPConf fpconf, ValueType type>
    : Pat<(opcode (type VEC256:$r1)),
          (VFPMUL256 $r1,
			      PermuteConf32.DAGnone8,
                  (IMPLICIT_DEF),
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf.DAG)>;

def pat_vec_cmp_oeq_v8f32 : VecFloatCMPIntrinsicPatComb<setoeq, /*fpaddmode*/1, ge_fpconf, v8i1, v8f32, AND>;
def pat_vec_cmp_olt_v8f32 : VecFloatCMPIntrinsicPat    <setolt, /*fpaddmode*/1, lt_fpconf, v8i1, v8f32>;
def pat_vec_cmp_oge_v8f32 : VecFloatCMPIntrinsicPat    <setoge, /*fpaddmode*/1, ge_fpconf, v8i1, v8f32>;
def pat_vec_cmp_ogt_v8f32 : VecFloatCMPIntrinsicPatSwap<setogt, /*fpaddmode*/1, lt_fpconf, v8i1, v8f32>;
def pat_vec_cmp_ole_v8f32 : VecFloatCMPIntrinsicPatSwap<setole, /*fpaddmode*/1, ge_fpconf, v8i1, v8f32>;
def pat_vec_cmp_one_v8f32 : VecFloatCMPIntrinsicPatComb<setone, /*fpaddmode*/1, lt_fpconf, v8i1, v8f32, OR>;

def pat_vec_cmp_o_v8f32   : VecFloatCMPIntrinsicPatCombOrd   <seto , /*fpaddmode*/1, lt_fpconf, ge_fpconf, v8i1, v8f32, OR>;
def pat_vec_cmp_uo_v8f32  : VecFloatCMPIntrinsicPatCombNotOrd<setuo, /*fpaddmode*/1, lt_fpconf, ge_fpconf, v8i1, v8f32, OR>;

// Below need to handle NaNs incorrectly.
def pat_vec_cmp_ueq_v8f32 : VecFloatCMPIntrinsicPatComb<setueq, /*fpaddmode*/1, ge_fpconf, v8i1, v8f32, AND>;
def pat_vec_cmp_ult_v8f32 : VecFloatCMPIntrinsicPat    <setult, /*fpaddmode*/1, lt_fpconf, v8i1, v8f32>;
def pat_vec_cmp_uge_v8f32 : VecFloatCMPIntrinsicPat    <setuge, /*fpaddmode*/1, ge_fpconf, v8i1, v8f32>;
def pat_vec_cmp_ugt_v8f32 : VecFloatCMPIntrinsicPatSwap<setugt, /*fpaddmode*/1, lt_fpconf, v8i1, v8f32>;
def pat_vec_cmp_ule_v8f32 : VecFloatCMPIntrinsicPatSwap<setule, /*fpaddmode*/1, ge_fpconf, v8i1, v8f32>;
def pat_vec_cmp_une_v8f32 : VecFloatCMPIntrinsicPatComb<setune, /*fpaddmode*/1, lt_fpconf, v8i1, v8f32, OR>;

// Same as Vector floating point ops, but select only the lowest bit.
class FloatCMPIntrinsicPat<PatFrags opcode, int fpaddmode, VFPConf fpconf, ValueType type>
    : Pat<(i32 (opcode (type OP_mWBDv:$r1), (type VEC256:$r2))),
          (BITGET (VFPMAC256GPR $r1, $r2,
			      PermuteConf32.DAGnone8,
                  (IMPLICIT_DEF),
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf.DAG),
				0)>;

class FloatCMPIntrinsicPatSwap<PatFrags opcode, int fpaddmode, VFPConf fpconf, ValueType type>
    : Pat<(i32 (opcode (type OP_mWBDv:$r1), (type VEC256:$r2))),
          (BITGET (VFPMAC256GPR $r2, $r1,
			      PermuteConf32.DAGnone8,
                  (IMPLICIT_DEF),
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf.DAG),
				0)>;

class FloatCMPIntrinsicPatComb<PatFrags opcode, int fpaddmode, VFPConf fpconf, ValueType type, AIEInst combine>
    : Pat<(i32 (opcode (type OP_mWBDv:$r1), (type VEC256:$r2))),
          (BITGET (combine
		  (VFPMAC256GPR $r1, $r2,
			      PermuteConf32.DAGnone8,
                  (IMPLICIT_DEF),
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf.DAG),
          (VFPMAC256GPR $r2, $r1,
			      PermuteConf32.DAGnone8,
                  (IMPLICIT_DEF),
				  PermuteConf32.DAGnone8,
                  fpaddmode,
				  fpconf.DAG)),
			    0)>;


// TODO: These are probably more efficient as integer comparisons?
def pat_vec_cmp_oeq_f32 : FloatCMPIntrinsicPatComb<setoeq, /*fpaddmode*/1, ge_fpconf, f32, AND>;
def pat_vec_cmp_olt_f32 : FloatCMPIntrinsicPat    <setolt, /*fpaddmode*/1, lt_fpconf, f32>;
def pat_vec_cmp_oge_f32 : FloatCMPIntrinsicPat    <setoge, /*fpaddmode*/1, ge_fpconf, f32>;
def pat_vec_cmp_ogt_f32 : FloatCMPIntrinsicPatSwap<setogt, /*fpaddmode*/1, lt_fpconf, f32>;
def pat_vec_cmp_ole_f32 : FloatCMPIntrinsicPatSwap<setole, /*fpaddmode*/1, ge_fpconf, f32>;
def pat_vec_cmp_one_f32 : FloatCMPIntrinsicPatComb<setone, /*fpaddmode*/1, lt_fpconf, f32, OR>;
def pat_vec_cmp_ueq_f32 : FloatCMPIntrinsicPatComb<setueq, /*fpaddmode*/1, ge_fpconf, f32, AND>;
def pat_vec_cmp_ult_f32 : FloatCMPIntrinsicPat    <setult, /*fpaddmode*/1, lt_fpconf, f32>;
def pat_vec_cmp_uge_f32 : FloatCMPIntrinsicPat    <setuge, /*fpaddmode*/1, ge_fpconf, f32>;
def pat_vec_cmp_ugt_f32 : FloatCMPIntrinsicPatSwap<setugt, /*fpaddmode*/1, lt_fpconf, f32>;
def pat_vec_cmp_ule_f32 : FloatCMPIntrinsicPatSwap<setule, /*fpaddmode*/1, ge_fpconf, f32>;
def pat_vec_cmp_une_f32 : FloatCMPIntrinsicPatComb<setune, /*fpaddmode*/1, lt_fpconf, f32, OR>;

//def : VecFloatNoACCIntrinsicPat<int_aie_vfpbinary, /*fpaddmode*/0, mul_fpconf>;
def pat_vec_add_v8f32    : VecFloatACCIntrinsicPat  <fadd,     /*fpaddmode*/0, addsub_fpconf, v8f32>;
def pat_vec_sub_v8f32    : VecFloatACCIntrinsicPat  <fsub,     /*fpaddmode*/1, addsub_fpconf, v8f32>;
def pat_vec_mul_v8f32    : VecFloatNoACCIntrinsicPat<fmul,     /*fpaddmode*/0, mul_fpconf, v8f32>;
def pat_vec_max_v8f32    : VecFloatACCIntrinsicPat  <fmaximum, /*fpaddmode*/1, lt_fpconf, v8f32>;
def pat_vec_min_v8f32    : VecFloatACCIntrinsicPat  <fminimum, /*fpaddmode*/1, ge_fpconf, v8f32>;
def pat_vec_maxnum_v8f32 : VecFloatACCIntrinsicPat  <fmaxnum,  /*fpaddmode*/1, lt_fpconf, v8f32>;
def pat_vec_minnum_v8f32 : VecFloatACCIntrinsicPat  <fminnum,  /*fpaddmode*/1, ge_fpconf, v8f32>;
def pat_vec_fneg_v8f32   : VecFloatUnaryIntrinsicPat<fneg,     /*fpaddmode*/1, addsub_fpconf, v8f32>;
def pat_vec_fabs_v8f32   : VecFloatUnaryIntrinsicPat<fabs,     /*fpaddmode*/0, abs_fpconf, v8f32>;

def pat_vec_mul_add_v8f32 : VecFloatMACPat</*fpaddmode*/0, mul_fpconf, v8f32>;
def pat_vec_mul_sub_v8f32 : VecFloatMSCPat</*fpaddmode*/1, mul_fpconf, v8f32>;

def pat_vec_add_f32    : VecFloatACCIntrinsicPat  <fadd,     /*fpaddmode*/0, addsub_fpconf, f32>;
def pat_vec_sub_f32    : VecFloatACCIntrinsicPat  <fsub,     /*fpaddmode*/1, addsub_fpconf, f32>;
def pat_vec_mul_f32    : VecFloatNoACCIntrinsicPat<fmul,     /*fpaddmode*/0, mul_fpconf, f32>;
def pat_vec_max_f32    : VecFloatACCIntrinsicPat  <fmaximum, /*fpaddmode*/1, lt_fpconf, f32>;
def pat_vec_min_f32    : VecFloatACCIntrinsicPat  <fminimum, /*fpaddmode*/1, ge_fpconf, f32>;
def pat_vec_maxnum_f32 : VecFloatACCIntrinsicPat  <fmaxnum,  /*fpaddmode*/1, lt_fpconf, f32>;
def pat_vec_minnum_f32 : VecFloatACCIntrinsicPat  <fminnum,  /*fpaddmode*/1, ge_fpconf, f32>;

def pat_fneg_f32 : Pat <(fneg GPR:$reg),
   (XOR $reg, Const32<0x80000000>.DAG)>;
def pat_fabs_f32 : Pat <(fabs GPR:$reg),
   (AND $reg, Const32<0x7FFFFFFF>.DAG)>;
def pat_fcopysign_f32 : Pat <(fcopysign GPR:$dst, GPR:$src),
   (BITSET $dst, 31, (BITGET $src, 31))>;

let Itinerary = II_MOVConvert in {
def FX2FLT : AIE_fx2flt<>;
def FLT2FX : AIE_flt2fx<>;
}
def pat_lrint : Pat <(i32 (lrint (f32 GPR:$reg))), (FLT2FX $reg, (MOV_U20 0))>;
def pat_sitofp : Pat <(f32 (sint_to_fp (i32 GPR:$reg))), (FX2FLT $reg, (MOV_U20 0))>;
//def pat_roundeven : Pat <(f32 (froundeven (f32 GPR:$reg))), (FX2FLT (FLT2FX $reg))>;

// sqrt, invsqrt, inv
multiclass NLF_instructions <AIEOpcode_nlf_combo opcode> {
let Itinerary = II_NLF_FLT_FLT in {
def NAME#_f32    : AIE_nlf_combo_flt_flt<opcode>;
}
let Itinerary = II_NLF_FIX_FLT in {
def NAME#_i32_f32    : AIE_nlf_combo_fix_flt<opcode>;
}
let Itinerary = II_NLF_FLT_FIX in {
def NAME#_f32_i32    : AIE_nlf_combo_flt_fix<opcode>;
}
let Itinerary = II_NLF_FIX_FIX in {
def NAME#_i32    : AIE_nlf_combo_fix_fix<opcode>;
}
}
class pat_builtin_nlf_flt_flt<Intrinsic intrin, AIEInst inst> : Pat <
    (f32 (intrin (f32 GPR:$reg))),
    (inst $reg)>;
class pat_builtin_nlf_fix_flt<Intrinsic intrin, AIEInst inst> : Pat <
    (i32 (intrin (i32 mS:$sft1), (f32 GPR:$reg))),
    (inst $sft1, $reg)>;
class pat_builtin_nlf_flt_fix<Intrinsic intrin, AIEInst inst> : Pat <
    (f32 (intrin (i32 GPR:$reg), (i32 mS:$sft2))),
    (inst $reg, $sft2)>;
class pat_builtin_nlf_fix_fix<Intrinsic intrin, AIEInst inst> : Pat <
    (i32 (intrin (i32 mS:$sft1), (i32 GPR:$reg), (i32 mS:$sft2))),
    (inst $sft1, $reg, $sft2)>;
defm sqrt : NLF_instructions<OPC_NLF_COMBO_SQRT>;
def : pat_builtin_nlf_flt_flt<int_aie_sqrt_flt_flt, sqrt_f32>;
def : pat_builtin_nlf_fix_flt<int_aie_sqrt_fix_flt, sqrt_i32_f32>;
def : pat_builtin_nlf_flt_fix<int_aie_sqrt_flt_fix, sqrt_f32_i32>;
def : pat_builtin_nlf_fix_fix<int_aie_sqrt_fix_fix, sqrt_i32>;
defm invsqrt : NLF_instructions<OPC_NLF_COMBO_INVSQRT>;
def : pat_builtin_nlf_flt_flt<int_aie_invsqrt_flt_flt, invsqrt_f32>;
def : pat_builtin_nlf_fix_flt<int_aie_invsqrt_fix_flt, invsqrt_i32_f32>;
def : pat_builtin_nlf_flt_fix<int_aie_invsqrt_flt_fix, invsqrt_f32_i32>;
def : pat_builtin_nlf_fix_fix<int_aie_invsqrt_fix_fix, invsqrt_i32>;
defm inv : NLF_instructions<OPC_NLF_COMBO_INV>;
def : pat_builtin_nlf_flt_flt<int_aie_inv_flt_flt, inv_f32>;
def : pat_builtin_nlf_fix_flt<int_aie_inv_fix_flt, inv_i32_f32>;
def : pat_builtin_nlf_flt_fix<int_aie_inv_flt_fix, inv_f32_i32>;
def : pat_builtin_nlf_fix_fix<int_aie_inv_fix_fix, inv_i32>;

def pat_sqrt : Pat <(f32 (fsqrt (f32 GPR:$reg))), (sqrt_f32 $reg)>;

def pat_mul_add_f32 : VecFloatMACPat  </*fpaddmode*/0, mul_fpconf, f32>;
def pat_mul_sub_f32 : VecFloatMSCPat  </*fpaddmode*/1, mul_fpconf, f32>;

class BitConvert <ValueType dt, ValueType st, RegisterClass rc> : Pat <
  (dt (bitconvert (st rc:$src0))),
  (dt rc:$src0)
>;
def : BitConvert <i32, f32, GPR>;
def : BitConvert <f32, i32, GPR>;

// def Pat<(llvm_fabs_v8f32 (v8f32 OP_mWBDv:$r2)),
//           (VFPMAC /*FIXME: initialize vector to zero ?*/,
// 		          (INSERT_SUBREG (v32f32 (IMPLICIT_DEF)),
// 		  		                 (INSERT_SUBREG (v16f32 (IMPLICIT_DEF)),
// 		  						                $r2,
// 												sub_256bit_lo),
// 								  sub_512bit_lo), (MOV_U20 0),
// 		  	      PermuteConf32.DAGnone8,
//                   (v8f32 $r2 /*(IMPLICIT_DEF)*/), 0,
// 				  PermuteConf32.DAGnone8,
//                   /*fpaddmode*/0,
// 				  (MOVT_S12 (MOV_U20 (LO20 (i32 abs_fpconf))), (HI12 (i32 abs_fpconf))))>;


// Vector integer MAC instructions
let Itinerary = II_VMUL in {
def VMUL80_S16_S16_AM : AIE_vec_acm_am_zero<0b1, 0b0, 0b00, 0b0, 0b00,
                                 (outs ACC768:$sd),
								 (ins VEC1024:$pmx, GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
								 VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf), [], "vmul.80",
    "$sd, ${pmx}.s16, $xstart, $xoffs, $ystart, ${ipm}.s16, $zstart, $zoffs, $conf">;
def VMUL80_S32_S32_AM : AIE_vec_acm_am_zero<0b1, 0b0, 0b01, 0b0, 0b01,
                                 (outs ACC384:$sd),
								 (ins VEC1024:$pmx, GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
								 VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf), [], "vmul.80",
    "$sd, ${pmx}.s32, $xstart, $xoffs, $ystart, ${ipm}.s32, $zstart, $zoffs, $conf">;

def VMUL48_S8_S8_BM : AIE_vec_acm_bm_zero<0b0, 0b0, 0b11, 0b0, 0b11,
                                 (outs ACC768:$sd),
								 (ins VEC1024:$pmx, GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
								 VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf), [], "vmul.48",
    "$sd, ${pmx}.s8, $xstart, $xoffs, $ystart, ${ipm}.s8, $zstart, $zoffs, $conf">;

def VMUL48_S32_S16_BM : AIE_vec_acm_bm_zero<0b0, 0b0, 0b01, 0b0, 0b00,
                                 (outs ACC768:$sd),
								 (ins VEC1024:$pmx, GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
								 VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf), [], "vmul.48",
    "$sd, ${pmx}.s32, $xstart, $xoffs, $ystart, ${ipm}.s16, $zstart, $zoffs, $conf">;
def VMUL80_S32_S32_BM : AIE_vec_acm_bm_zero<0b1, 0b0, 0b01, 0b0, 0b01,
                                 (outs ACC768:$sd),
								 (ins VEC1024:$pmx, GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
								 VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf), [], "vmul.80",
    "$sd, ${pmx}.s32, $xstart, $xoffs, $ystart, ${ipm}.s32, $zstart, $zoffs, $conf">;
}

let Itinerary = II_VMAC, Constraints = "$sd = $sdin" in {
def VMAC80_S16_S16_AM : AIE_vec_acm_am_acc<0b1, 0b0, 0b00, 0b0, 0b00,
                                 (outs ACC768:$sd),
								 (ins ACC768:$sdin, VEC1024:$pmx, GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
								 VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf), [], "vmac.80",
    "$sd, ${pmx}.s16, $xstart, $xoffs, $ystart, ${ipm}.s16, $zstart, $zoffs, $conf">;
def VMAC80_S32_S32_AM : AIE_vec_acm_am_acc<0b1, 0b0, 0b01, 0b0, 0b01,
                                 (outs ACC384:$sd),
								 (ins ACC384:$sdin, VEC1024:$pmx, GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
								 VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf), [], "vmac.80",
    "$sd, ${pmx}.s32, $xstart, $xoffs, $ystart, ${ipm}.s32, $zstart, $zoffs, $conf">;
def VMAC48_S32_S16_BM : AIE_vec_acm_bm_acc<0b0, 0b0, 0b01, 0b0, 0b00,
                                 (outs ACC768:$sd),
								 (ins ACC768:$sdin, VEC1024:$pmx, GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
								 VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf), [], "vmac.48",
    "$sd, ${pmx}.s32, $xstart, $xoffs, $ystart, ${ipm}.s16, $zstart, $zoffs, $conf">;
def VMAC48_S16_S32_BM : AIE_vec_acm_bm_acc<0b0, 0b0, 0b00, 0b0, 0b01,
                                 (outs ACC768:$sd),
								 (ins ACC768:$sdin, VEC1024:$pmx, GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
								 VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf), [], "vmac.48",
    "$sd, ${pmx}.s16, $xstart, $xoffs, $ystart, ${ipm}.s32, $zstart, $zoffs, $conf">;
def VMAC48_S8_S8_BM : AIE_vec_acm_bm_acc<0b0, 0b0, 0b11, 0b0, 0b11,
                                 (outs ACC768:$sd),
								 (ins ACC768:$sdin, VEC1024:$pmx, GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
								 VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf), [], "vmac.48",
    "$sd, ${pmx}.s8, $xstart, $xoffs, $ystart, ${ipm}.s8, $zstart, $zoffs, $conf">;
def VMAC80_S32_S32_BM : AIE_vec_acm_bm_acc<0b1, 0b0, 0b01, 0b0, 0b01,
                                 (outs ACC768:$sd),
								 (ins ACC768:$sdin, VEC1024:$pmx, GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
								 VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf), [], "vmac.80",
    "$sd, ${pmx}.s32, $xstart, $xoffs, $ystart, ${ipm}.s32, $zstart, $zoffs, $conf">;
}

def : Pat<(int_aie_upd_w_v16i32_lo VEC512:$sbuf, VEC256:$val),
           (INSERT_SUBREG VEC512:$sbuf, VEC256:$val, sub_256bit_lo)>;
def : Pat<(int_aie_upd_w_v16i32_hi VEC512:$sbuf, VEC256:$val),
           (INSERT_SUBREG VEC512:$sbuf, VEC256:$val, sub_256bit_hi)>;

def : Pat<(int_aie_upd_v_v8i32_lo VEC256:$sbuf, VEC128:$val),
           (INSERT_SUBREG VEC256:$sbuf, VEC128:$val, sub_128bit_lo)>;
def : Pat<(int_aie_upd_v_v8i32_hi VEC256:$sbuf, VEC128:$val),
           (INSERT_SUBREG VEC256:$sbuf, VEC128:$val, sub_128bit_hi)>;

def : Pat<(int_aie_ext_w_v16i32_lo VEC512:$sbuf),
           (EXTRACT_SUBREG VEC512:$sbuf, sub_256bit_lo)>;
def : Pat<(int_aie_ext_w_v16i32_hi VEC512:$sbuf),
           (EXTRACT_SUBREG VEC512:$sbuf, sub_256bit_hi)>;

def : Pat<(int_aie_ext_w_v32i16_lo VEC512:$sbuf),
           (EXTRACT_SUBREG VEC512:$sbuf, sub_256bit_lo)>;
def : Pat<(int_aie_ext_w_v32i16_hi VEC512:$sbuf),
           (EXTRACT_SUBREG VEC512:$sbuf, sub_256bit_hi)>;

def : Pat<(int_aie_ext_v_v8i32_lo VEC256:$sbuf),
           (EXTRACT_SUBREG VEC256:$sbuf, sub_128bit_lo)>;
def : Pat<(int_aie_ext_v_v8i32_hi VEC256:$sbuf),
           (EXTRACT_SUBREG VEC256:$sbuf, sub_128bit_hi)>;

def : Pat<(int_aie_concat_v16i16 VEC256:$abuf, VEC256:$bbuf),
           (REG_SEQUENCE VEC512, $abuf, sub_256bit_lo, $bbuf, sub_256bit_hi)>;

def : Pat<(int_aie_concat_v64i16 VEC512:$abuf, VEC512:$bbuf),
           (REG_SEQUENCE VEC1024, $abuf, sub_512bit_lo, $bbuf, sub_512bit_hi)>;

def : Pat<(int_aie_lmul4_v32int32 VEC1024:$pmx, VECFWC:$ipm, GPR:$xstart, GPR0_7:$ystart,
			    t04u:$zstart, mC:$xoffs, mC:$zoffs, mC:$conf),
           (VMUL80_S32_S32_AM VEC1024:$pmx, GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
			    VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf)>;

def : Pat<(int_aie_lmul4_v16int32 VEC512:$src, VECFWC:$ipm, GPR:$xstart, GPR0_7:$ystart,
			    t04u:$zstart, mC:$xoffs, mC:$zoffs, mC:$conf),
           (VMUL80_S32_S32_AM (INSERT_SUBREG (v32i32 (IMPLICIT_DEF)), VEC512:$src, sub_512bit_lo), GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
                           VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf)>;

def : Pat<(int_aie_lmul8_v32int32 VEC1024:$pmx, VECFWC:$ipm, GPR:$xstart, GPR0_7:$ystart,
			    t04u:$zstart, mC:$xoffs, mC:$zoffs, mC:$conf),
           (VMUL80_S32_S32_BM VEC1024:$pmx, GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
			    VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf)>;

def : Pat<(int_aie_mul16_v32int32 VEC1024:$pmx, VECFWC:$ipm, GPR:$xstart, GPR0_7:$ystart,
			    t04u:$zstart, mC:$xoffs, mC:$zoffs, mC:$conf),
           (VMUL48_S32_S16_BM VEC1024:$pmx, GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
			    VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf)>;

def : Pat<(int_aie_mul16_v128int8 VEC1024:$pmx, VECFWC:$ipm, GPR:$xstart, GPR0_7:$ystart,
			    t04u:$zstart, mC:$xoffs, mC:$zoffs, mC:$conf),
           (VMUL48_S8_S8_BM VEC1024:$pmx, GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
			    VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf)>;

def : Pat<(int_aie_mac16_v32int32 VEC1024:$pmx, VECFWC:$ipm, ACC768:$acc, GPR:$xstart, GPR0_7:$ystart,
			    t04u:$zstart, mC:$xoffs, mC:$zoffs, mC:$conf),
           (VMAC48_S32_S16_BM ACC768:$acc, VEC1024:$pmx, GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
			    VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf)>;

def : Pat<(int_aie_mac16_v64int16 VEC1024:$pmx, VECFWC:$ipm, ACC768:$acc, GPR:$xstart, GPR0_7:$ystart,
			    t04u:$zstart, mC:$xoffs, mC:$zoffs, mC:$conf),
           (VMAC48_S16_S32_BM ACC768:$acc, VEC1024:$pmx, GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
			    VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf)>;

def : Pat<(int_aie_mac16_v128int8 VEC1024:$pmx, VECFWC:$ipm, ACC768:$acc, GPR:$xstart, GPR0_7:$ystart,
			    t04u:$zstart, mC:$xoffs, mC:$zoffs, mC:$conf),
           (VMAC48_S8_S8_BM ACC768:$acc, VEC1024:$pmx, GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
			    VECFWC:$ipm, t04u:$zstart, mC:$zoffs, mC:$conf)>;

// def : Pat<(int_aie_vmac (v8f32 OP_mWBDv:$acc),
//                           VEC1024:$sx, GPR:$fpxstart, mCL:$fpxoffs,
//                           VECFWC:$sc, t03u:$fpzstart, mCL:$fpzoffs,
// 						  t02u:$fpaddmode, mCL:$fpconf),
//           (VFPMAC (v8f32 OP_mWBDv:$acc),
// 		          VEC1024:$sx, GPR:$fpxstart, mCL:$fpxoffs,
// 					VECFWC:$sc, t03u:$fpzstart, mCL:$fpzoffs,
// 					t02u:$fpaddmode, mCL:$fpconf)>;

// Note that there's an explicit constant expansion here, which seems like it should be
// handled by the simm32 expansion, but it seems like further expansion isn't applied to
// constants here?

class VecNoACCIntrinsicPat<SDNode node, VIntConf conf>
    : Pat<(node (v8i32 VEC256:$r1), (v8i32 VECFWC:$r2)),
        (MV_W_SRS0_80_srsl
		    (VMUL80_S32_S32_AM (INSERT_SUBREG (v32i32 (IMPLICIT_DEF)),
				                 (INSERT_SUBREG (v16i32 (IMPLICIT_DEF)),
								                $r1,
												sub_256bit_lo),
								  sub_512bit_lo),
				  Const20<0>.DAG,
			      PermuteConf.DAGnone8,
                  Const20<0>.DAG,
			      VECFWC:$r2, 0,
			      PermuteConf.DAGnone8,
			      conf.DAG
		    ), (MOV_U20 0), (MOV_U20 12) /* Round to nearest even, no saturation */
		)>;

def pat_vec_mul_v8i32 : VecNoACCIntrinsicPat<mul, mul_conf>;

// class VecACCIntrinsicPat<SDNode opcode, int conf>
//     : Pat<(opcode (v16i48 ACC768:$r1), (v8i32 VECFWC:$r2)),
//           (VMAC80_S32_S32_AM $r1, (INSERT_SUBREG (v32i32 (IMPLICIT_DEF)),
// 				                 (INSERT_SUBREG (v16i32 (IMPLICIT_DEF)),
// 								                $r1,
// 												sub_256bit_lo),
// 								  sub_512bit_lo), Const20<0>.DAG,
//                PermuteConf.DAGnone8,
//                Const20<0>.DAG,
//                (v8i32 (IMPLICIT_DEF)), 0,
// 				  PermuteConf.DAGnone8,
// 				  Const_v2i32<confhi, conflo>.DAG);

let Itinerary = II_VCMP in {
class VCMP<bits<2> xsz, string size> :
            AIE_vec_compare<xsz,
								(outs OP_mXCDv:$dst, GPR0_7:$cmp),
								(ins VEC1024:$sx, GPR:$xstart, mC:$xoffs,
                                      GPR0_7:$ystart, mC:$yoffs,
									  mC:$conf), [], "vcmp",
    !strconcat("$dst, $cmp, ${sx}.", size, ", $xstart, $xoffs, $ystart, $yoffs, $conf")>;

// These Pseudos require 2 temporaries (in r5 and r15) when they are expanded.
let hasSideEffects = 0, mayLoad = 0, mayStore = 0, Defs=[r5,r15] in {
// Note that the register classes for these pseudo-instructions are more restricted than
// or VCMP because sx and sy have to come from the same 1024-bit register.  We don't have
// an easy way of describing this register restriction, so we always pick ya.

// Select and operate on 512 bit vectors.
class VCMP512<bits<2> xsz, string size> :
		    Pseudo<(outs OP_mXCDv:$dst, GPR0_7:$cmp),
								 (ins OP_mXABv:$sx, OP_mXABv:$sy, mC:$xoffs,
                                      mC:$yoffs, mC:$conf), [], "vcmp512",
    !strconcat("$dst, $cmp, ${sx}.", size, ", $sy, $xoffs, $yoffs, $conf")> {
		bits<2> _xsz = xsz; // Otherwise unused, silence warnings
	}

class VCMP512GPR<bits<2> xsz, string size> :
		    Pseudo<(outs GPR0_7:$cmp, OP_mXCDv:$dst),
								 (ins OP_mXABv:$sx, OP_mXABv:$sy, mC:$xoffs,
                                      mC:$yoffs, mC:$conf), [], "vcmp512gpr",
    !strconcat("$dst, $cmp, ${sx}.", size, ", $sy, $xoffs, $yoffs, $conf")> {
		bits<2> _xsz = xsz; // Otherwise unused, silence warnings
	}
}
}

// Could this be a multiclass?
def VCMP_S16 : VCMP<0b00, "s16">;
def VCMP_S32 : VCMP<0b01, "s32">;
def VCMP_U8  : VCMP<0b10, "u8">;
def VCMP_S8  : VCMP<0b11, "s8">;
def VCMP512_S16 : VCMP512<0b00, "s16">;
def VCMP512_S32 : VCMP512<0b01, "s32">;
def VCMP512_U8  : VCMP512<0b10, "u8">;
def VCMP512_S8  : VCMP512<0b11, "s8">;
def VCMP512GPR_S16 : VCMP512GPR<0b00, "s16">;
def VCMP512GPR_S32 : VCMP512GPR<0b01, "s32">;
def VCMP512GPR_U8  : VCMP512GPR<0b10, "u8">;
def VCMP512GPR_S8  : VCMP512GPR<0b11, "s8">;

class VecCMP512IntrinsicPat<SDPatternOperator opcode, AIEInst inst, VIntConf conf, ValueType Rtype, ValueType type>
    : Pat<(Rtype (opcode (type VEC512:$r1), (type VEC512:$r2))),
          (inst $r1, $r2, PermuteConf.DAGnone16, PermuteConf.DAGnone16, conf.DAG)>;

class VecCMP512IntrinsicPatSwap<SDPatternOperator opcode, AIEInst inst, VIntConf conf, ValueType Rtype, ValueType type>
    : Pat<(Rtype (opcode (type VEC512:$r1), (type VEC512:$r2))),
          (inst $r2, $r1, PermuteConf.DAGnone16, PermuteConf.DAGnone16, conf.DAG)>;

class VecCMP512IntrinsicPatComb<SDPatternOperator opcode, AIEInst inst, VIntConf conf, ValueType Rtype, ValueType type, AIEInst combine>
    : Pat<(Rtype (opcode (type VEC512:$r1), (type VEC512:$r2))),
          (combine
		    (inst $r1, $r2, PermuteConf.DAGnone16, PermuteConf.DAGnone16, conf.DAG),
			(inst $r2, $r1, PermuteConf.DAGnone16, PermuteConf.DAGnone16, conf.DAG))>;

// Expand v8 -> v16.  Its arguable this should be done during legalization, but doing it here gives us the opportunity
// to do better register allocation.
class VecCMP256IntrinsicPat<SDPatternOperator opcode, AIEInst inst, VIntConf conf, ValueType Rtype, ValueType type>
    : Pat<(Rtype (opcode type:$r1, type:$r2)),
		(type (EXTRACT_SUBREG
			(inst
				(INSERT_SUBREG (v16i32 (IMPLICIT_DEF)), $r1, sub_256bit_lo),
				(INSERT_SUBREG (v16i32 (IMPLICIT_DEF)), $r2, sub_256bit_lo),
				PermuteConf.DAGnone8, PermuteConf.DAGnone8, conf.DAG),
			sub_256bit_lo))>;

class VecCMP256IntrinsicPatSwap<SDPatternOperator opcode, AIEInst inst, VIntConf conf, ValueType Rtype, ValueType type>
    : Pat<(Rtype (opcode type:$r2, type:$r1)),
		(type (EXTRACT_SUBREG
			(inst
				(INSERT_SUBREG (v16i32 (IMPLICIT_DEF)), $r1, sub_256bit_lo),
				(INSERT_SUBREG (v16i32 (IMPLICIT_DEF)), $r2, sub_256bit_lo),
				PermuteConf.DAGnone8, PermuteConf.DAGnone8, conf.DAG),
			sub_256bit_lo))>;

class VecCMP256GPRIntrinsicPat<SDPatternOperator opcode, AIEInst inst, VIntConf conf, ValueType Rtype, ValueType type>
    : Pat<(Rtype (opcode type:$r1, type:$r2)),
		(v8i1 (ZE8
			(inst
				(INSERT_SUBREG (v16i32 (IMPLICIT_DEF)), $r1, sub_256bit_lo),
				(INSERT_SUBREG (v16i32 (IMPLICIT_DEF)), $r2, sub_256bit_lo),
				PermuteConf.DAGnone8, PermuteConf.DAGnone8, conf.DAG)))>;
class VecCMP256GPRIntrinsicPatSwap<SDPatternOperator opcode, AIEInst inst, VIntConf conf, ValueType Rtype, ValueType type>
    : Pat<(Rtype (opcode type:$r1, type:$r2)),
		(v8i1 (ZE8
			(inst
				(INSERT_SUBREG (v16i32 (IMPLICIT_DEF)), $r2, sub_256bit_lo),
				(INSERT_SUBREG (v16i32 (IMPLICIT_DEF)), $r1, sub_256bit_lo),
				PermuteConf.DAGnone8, PermuteConf.DAGnone8, conf.DAG)))>;

// This is a bit of a hack to get things working.  In reality, the register classes here are overconstrained and the
// generated DAG could probably be simplified by assuming one-buffer mode.
class VecCMP256GPRIntrinsicPatComb<SDPatternOperator opcode, AIEInst inst, VIntConf conf, ValueType Rtype, ValueType type, AIEInst combine>
    : Pat<(Rtype (opcode (type OP_mWAv:$r1), (type OP_mWAv:$r2))),
		(v8i1 (ZE8
            (combine
				(inst
					(INSERT_SUBREG (INSERT_SUBREG (v16i32 (IMPLICIT_DEF)), $r1, sub_256bit_lo), $r2, sub_256bit_hi),
					(INSERT_SUBREG (INSERT_SUBREG (v16i32 (IMPLICIT_DEF)), $r1, sub_256bit_lo), $r2, sub_256bit_hi),
					PermuteConf.DAGnonelo8hi8, PermuteConf.DAGnonehi8lo8, conf.DAG),
				(LSHL
				   (inst
					(INSERT_SUBREG (INSERT_SUBREG (v16i32 (IMPLICIT_DEF)), $r1, sub_256bit_lo), $r2, sub_256bit_hi),
					(INSERT_SUBREG (INSERT_SUBREG (v16i32 (IMPLICIT_DEF)), $r1, sub_256bit_lo), $r2, sub_256bit_hi),
					PermuteConf.DAGnonelo8hi8, PermuteConf.DAGnonehi8lo8, conf.DAG),
					Const32<-2056>.DAG))))>; // shift right by 8

def pat_vec_cmp_oeq_i32 : VecCMP256GPRIntrinsicPatComb<seteq, VCMP512GPR_S32, eq_conf, v8i1, v8i32, AND>;
def pat_vec_cmp_olt_i32 : VecCMP256GPRIntrinsicPat    <setlt, VCMP512GPR_S32, lt_conf, v8i1, v8i32>;
def pat_vec_cmp_oge_i32 : VecCMP256GPRIntrinsicPat    <setge, VCMP512GPR_S32, ge_conf, v8i1, v8i32>;
def pat_vec_cmp_ogt_i32 : VecCMP256GPRIntrinsicPatSwap<setgt, VCMP512GPR_S32, lt_conf, v8i1, v8i32>;
def pat_vec_cmp_ole_i32 : VecCMP256GPRIntrinsicPatSwap<setle, VCMP512GPR_S32, ge_conf, v8i1, v8i32>;
def pat_vec_cmp_one_i32 : VecCMP256GPRIntrinsicPatComb<setne, VCMP512GPR_S32, ne_conf, v8i1, v8i32, OR>;

def pat_vec_cmp_oeq_v16i32 : VecCMP512IntrinsicPatComb<seteq, VCMP512GPR_S32, ge_conf, v16i1, v16i32, AND>;
def pat_vec_cmp_olt_v16i32 : VecCMP512IntrinsicPat    <setlt, VCMP512GPR_S32, lt_conf, v16i1, v16i32>;
def pat_vec_cmp_oge_v16i32 : VecCMP512IntrinsicPat    <setge, VCMP512GPR_S32, ge_conf, v16i1, v16i32>;
def pat_vec_cmp_ogt_v16i32 : VecCMP512IntrinsicPatSwap<setgt, VCMP512GPR_S32, lt_conf, v16i1, v16i32>;
def pat_vec_cmp_ole_v16i32 : VecCMP512IntrinsicPatSwap<setle, VCMP512GPR_S32, ge_conf, v16i1, v16i32>;
def pat_vec_cmp_one_v16i32 : VecCMP512IntrinsicPatComb<setne, VCMP512GPR_S32, lt_conf, v16i1, v16i32, OR>;

def pat_vec_add_v16i32: VecCMP512IntrinsicPat    <add, VCMP512_S32, add_conf, v16i32, v16i32>;
def pat_vec_add_v8i32 : VecCMP256IntrinsicPat    <add, VCMP512_S32, add_conf, v8i32, v8i32>;
def pat_vec_sub_v16i32: VecCMP512IntrinsicPat    <sub, VCMP512_S32, sub_conf, v16i32, v16i32>;
def pat_vec_sub_v8i32 : VecCMP256IntrinsicPat    <sub, VCMP512_S32, sub_conf, v8i32, v8i32>;
def pat_vec_min_v16i32: VecCMP512IntrinsicPat    <smin, VCMP512_S32, min_conf, v16i32, v16i32>;
def pat_vec_min_v8i32 : VecCMP256IntrinsicPat    <smin, VCMP512_S32, min_conf, v8i32, v8i32>;
def pat_vec_max_v16i32: VecCMP512IntrinsicPat    <smax, VCMP512_S32, max_conf, v16i32, v16i32>;
def pat_vec_max_v8i32 : VecCMP256IntrinsicPat    <smax, VCMP512_S32, max_conf, v8i32, v8i32>;


let Itinerary = II_VSEL in {
class VSEL<bits<2> xsz, string size> :
           AIE_vec_select<xsz,
								(outs OP_mXCDv:$dst),
								(ins VEC1024:$sx, GPR:$xstart, mC:$xoffs,
                                      GPR0_7:$ystart, mC:$yoffs,
									  mC:$conf, GPR:$sel), [], "vsel",
    !strconcat("$dst, ${sx}.", size, ", $xstart, $xoffs, $ystart, $yoffs, $conf, $sel")>;
def VSEL_S16 : VSEL<0b00, "s16">;
def VSEL_S32 : VSEL<0b01, "s32">;
def VSEL_U8  : VSEL<0b10, "u8">;
def VSEL_S8  : VSEL<0b11, "s8">;
}

def : Pat<(int_aie_prim_v32int16_select VEC512:$src, GPR:$sel, GPR:$xstart, GPR0_7:$ystart,
			    mC:$xoffs, mC:$yoffs, mC:$conf),
           (VSEL_S16 (INSERT_SUBREG (v32i32 (IMPLICIT_DEF)), VEC512:$src, sub_512bit_lo), GPR:$xstart, mC:$xoffs, GPR0_7:$ystart,
                            mC:$yoffs, mC:$conf, GPR:$sel)>;

def pat_vec_select_v8i32: Pat<(vselect GPR:$sel, (v8i32 VEC256:$r1), (v8i32 VEC256:$r2)),
		    (EXTRACT_SUBREG (VSEL_S32 (INSERT_SUBREG (INSERT_SUBREG (v32i32 (IMPLICIT_DEF)),
				                 (INSERT_SUBREG (v16i32 (IMPLICIT_DEF)),
								                $r1,
												sub_256bit_lo),
								  sub_512bit_lo),
				                 (INSERT_SUBREG (v16i32 (IMPLICIT_DEF)),
								                $r2,
												sub_256bit_lo),
								  sub_512bit_hi),
				  Const20<0>.DAG, // Offset zero
			      PermuteConf.DAGnone8,
				  Const20<16>.DAG, // Offset 16
			      PermuteConf.DAGnone8,
				  add_conf.DAG,
				  $sel
		    ),
		    sub_256bit_lo)>;


// Select and SelectCC

class SelectPat<ValueType type> :
         Pat<(type (select (i32 GPR0:$rs1), (type GPR:$rs2), (type GPR:$rs3))),
          (ITE_NEZ GPR:$rs2, GPR:$rs3, GPR0:$rs1)>;
def : SelectPat<i20>;
def : SelectPat<ptr0>;
def : SelectPat<i32>;
def : SelectPat<f32>;

/// Branches and jumps

// Reads in E1, 5 branch delay slots.
let hasDelaySlot = 1 in
let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
class ba_instr<bits<1> opcode, string opcodestr>
    : AIE_ba_instr<(outs), (ins GPR:$reg, addr20:$addr),
      [], opcode, opcodestr, "$reg, $addr"> {
  let isBranch = 1;
  let isTerminator = 1;
}

let Itinerary = II_ALU in {
def BEQZ : ba_instr<0b0, "beqz">;
def BNEZ : ba_instr<0b1, "bnez">;
}

// Compare operations with corresponding instructions
class CmpPat<PatFrag CondOp, AIEInst Inst, ValueType type>
    : Pat<(CCType (CondOp (type GPR:$rs1), (type GPR:$rs2))),
	       (Inst GPR:$rs1, GPR:$rs2)>;
// As above, but with type extension on the inputs
class CmpPatExtend<PatFrag CondOp, AIEInst Inst, ValueType type, AIEInst Extend>
    : Pat<(CCType (CondOp (type GPR:$rs1), (type GPR:$rs2))),
	       (Inst (Extend GPR:$rs1), (Extend GPR:$rs2))>;
class CmpPatZExtendi20<PatFrag CondOp, AIEInst Inst, ValueType type>
    : Pat<(CCType (CondOp (type GPR:$rs1), (type GPR:$rs2))),
	       (Inst (MOVT_S12 GPR:$rs1, (i32 0)), (MOVT_S12 GPR:$rs2, (i32 0)))>;

// Compare operations without corresponding instructions, but which
// are trivially supported by swapping the two input operands
class CmpSwapPat<PatFrag CondOp, AIEInst Inst, ValueType type>
    : Pat<(CCType (CondOp (type GPR:$rs1), (type GPR:$rs2))),
          (Inst GPR:$rs2, GPR:$rs1)>;
// As above, but with type extension on the inputs
class CmpSwapPatExtend<PatFrag CondOp, AIEInst Inst, ValueType type, AIEInst Extend>
    : Pat<(CCType (CondOp (type GPR:$rs1), (type GPR:$rs2))),
          (Inst (Extend GPR:$rs2), (Extend GPR:$rs1))>;
class CmpSwapPatZExtendi20<PatFrag CondOp, AIEInst Inst, ValueType type>
    : Pat<(CCType (CondOp (type GPR:$rs1), (type GPR:$rs2))),
          (Inst (MOVT_S12 GPR:$rs2, (i32 0)), (MOVT_S12 GPR:$rs1, (i32 0)))>;

multiclass CmpPatterns<ValueType type> {
def : CmpPat<seteq, EQ, type>;
def : CmpPat<setne, NE, type>;
def : CmpPat<setlt, LT, type>;
def : CmpPat<setge, GE, type>;
def : CmpPat<setult, LTU, type>;
def : CmpPat<setuge, GEU, type>;
def : CmpSwapPat<setgt, LT, type>;
def : CmpSwapPat<setle, GE, type>;
def : CmpSwapPat<setugt, LTU, type>;
def : CmpSwapPat<setule, GEU, type>;
}
multiclass CmpPatternsExtend<ValueType type, AIEInst ZExt, AIEInst SExt> {
def : CmpPatExtend<seteq, EQ, type, ZExt>;
def : CmpPatExtend<setne, NE, type, ZExt>;
def : CmpPatExtend<setlt, LT, type, SExt>;
def : CmpPatExtend<setge, GE, type, SExt>;
def : CmpPatExtend<setult, LTU, type, ZExt>;
def : CmpPatExtend<setuge, GEU, type, ZExt>;
def : CmpSwapPatExtend<setgt, LT, type, SExt>;
def : CmpSwapPatExtend<setle, GE, type, SExt>;
def : CmpSwapPatExtend<setugt, LTU, type, ZExt>;
def : CmpSwapPatExtend<setule, GEU, type, ZExt>;
}
// i20 is pointer so only need unsigned comparisons.
// However, when comparing with a signed integer, it's
// still possible to generate signed compares for i20.
multiclass CmpPatternsExtendi20<ValueType type> {
def : CmpPatZExtendi20<seteq, EQ, type>;
def : CmpPatZExtendi20<setne, NE, type>;
def : CmpPatZExtendi20<setlt, LTU, type>;
def : CmpPatZExtendi20<setge, GEU, type>;
def : CmpPatZExtendi20<setult, LTU, type>;
def : CmpPatZExtendi20<setuge, GEU, type>;
def : CmpSwapPatZExtendi20<setgt, LTU, type>;
def : CmpSwapPatZExtendi20<setle, GEU, type>;
def : CmpSwapPatZExtendi20<setugt, LTU, type>;
def : CmpSwapPatZExtendi20<setule, GEU, type>;
}

multiclass CmpPatternsPtr<ValueType type> {
def : CmpPat<seteq, EQ, type>;
def : CmpPat<setne, NE, type>;
def : CmpPat<setult, LTU, type>;
def : CmpPat<setuge, GEU, type>;
def : CmpSwapPat<setugt, LTU, type>;
def : CmpSwapPat<setule, GEU, type>;
}

defm : CmpPatterns<i32>;
defm : CmpPatternsExtendi20<i20>;
defm : CmpPatternsPtr<ptr0>;

// Conditional branches.  The condition is probably computed by one of the
// previous compare instructions
def : Pat<(brcond (CCType GPR:$cond), bb:$imm20), (BNEZ GPR:$cond, addr20:$imm20)>;
def : Pat<(brcond (i20 GPR:$cond), bb:$imm20), (BNEZ GPR:$cond, addr20:$imm20)>;

// // Unconditional branch
// def : Pat<(br bb:$imm20), (BNEZ GPR:$cond, bb:$imm20)>;

/////////
// Shifts

// Shift right is implemented as a negative shift left
def : Pat<(srl (i32 GPR:$rs1), (i32 GPR:$rs2)), (LSHL GPR:$rs1, (SUB (MOV_U20 0), GPR:$rs2))>;
def : Pat<(sra (i32 GPR:$rs1), (i32 GPR:$rs2)), (ASHL GPR:$rs1, (SUB (MOV_U20 0), GPR:$rs2))>;
def : Pat<(shl (i32 GPR:$rs1), (i32 GPR:$rs2)), (LSHL GPR:$rs1, GPR:$rs2)>;

/* The below shifts are required because of getelementptr in LLVM IR.
 * For getelementptr the ValueType is set as i20 because of getPointerSize.
 */

def : Pat<(i20 (srl (i20 GPR:$rs1), (i20 GPR:$rs2))), (LSHL  GPR:$rs1, (SUB (MOV_U20 0), GPR:$rs2))>;
// Note: Sign extending below.
def : Pat<(i20 (sra (i20 GPR:$rs1), (i20 GPR:$rs2))), (ASHL  (LSHL (MOV_U20 12), GPR:$rs1), (SUB (MOV_U20 0), (ADD (MOV_U20 12), GPR:$rs2)))>;
def : Pat<(i20 (shl (i20 GPR:$rs1), (i20 GPR:$rs2))), (LSHL  GPR:$rs1, GPR:$rs2)>;

def : Pat<(i20 (shl (i20 (trunc (i32 GPR:$rs1))), (i20 GPR:$rs2))), (LSHL  GPR:$rs1, GPR:$rs2)>;
////////
// Sign/Zero Extension for 8 and 16 bit numbers.
def : Pat<(i32 (sext_inreg GPR:$rs, i8)),  (SE8  GPR:$rs)>;
def : Pat<(i32 (sext_inreg GPR:$rs, i16)), (SE16 GPR:$rs)>;

// We can also zero extend within a register
def : Pat<(i32 (and GPR:$rs1, (i32 255))), (ZE8 GPR:$rs1)>;
def : Pat<(i32 (and GPR:$rs1, (i32 65535))), (ZE16 GPR:$rs1)>;

def : Pat<(i32 (zext   (i20 GPR:$rs))), (MOVT_S12 GPR:$rs, (i32 0))>;
def : Pat<(i32 (anyext (i20 GPR:$rs))), (MOVT_S12 GPR:$rs, (i32 0))>;
def : Pat<(i20 (sext_inreg GPR:$rs, i8)),  (SE8  GPR:$rs)>;
def : Pat<(i20 (sext_inreg GPR:$rs, i16)),  (SE8  GPR:$rs)>;

///////
// Count Leading Zeros
def : Pat<(i32 (ctlz (i32 GPR:$rs))), (CLB GPR:$rs)>;

///////
// ABS
def : Pat<(i32 (abs (i32 GPR:$rs))), (ABS GPR:$rs)>;

//def : InstAlias<"add $rd, $rs1, $imm12",
//                (ADDI  GPR:$rd, GPR:$rs1, simm12:$imm12)>;
//def : InstAlias<"and $rd, $rs1, $imm12",
//                (ANDI  GPR:$rd, GPR:$rs1, simm12:$imm12)>;
//def : InstAlias<"xor $rd, $rs1, $imm12",
//                (XORI  GPR:$rd, GPR:$rs1, simm12:$imm12)>;
//def : InstAlias<"or $rd, $rs1, $imm12",
//                (ORI  GPR:$rd, GPR:$rs1, simm12:$imm12)>;

multiclass LDA_instructions <RegisterOperand regclass> {
let Itinerary = II_LDA in {
def LDA_ind_#NAME : AIE_dms_lda<(outs regclass:$reg), (ins PTR:$ptr), [], "lda", "$reg, [$ptr]">,
	 ag_short_ind;
def LDA_imm_#NAME : AIE_dms_lda<(outs regclass:$reg), (ins PTR:$ptr, imm5x32:$imm), [], "lda", "$reg, [$ptr], $imm">,
	 ag_short_pstm_nrm_imm;
def LDA_idx_#NAME : AIE_dms_lda<(outs regclass:$reg), (ins PTR:$ptr, mCS:$cs), [], "lda", "$reg, [$ptr, $cs]">,
	 ag_short_idx;
} // Itinerary = II_LDA
let Itinerary = II_LDASP_RW, Defs = [SP], Uses = [SP] in
def LDA_sp_imm_#NAME : AIE_dms_lda<(outs regclass:$reg), (ins imm5x32:$imm), [], "lda", "$reg, [sp], $imm">,
	 ag_short_pstm_sp_imm;
let Itinerary = II_LDASP_R, Uses = [SP] in {
def LDA_spis_#NAME : AIE_dms_lda<(outs regclass:$reg), (ins c10n_step4:$imm), [], "lda", "$reg, [sp, $imm]">,
	 ag_short_pstm_spis;

def LDA_SPIL_#NAME : AIE_dms_lda_spil<(outs regclass:$reg), (ins t17n_step4:$imm), [], "lda.spil", "$reg, [sp, $imm]">;
} // Itinerary = II_LDASP_R, Uses = [SP]
} // multiclass

defm "" : LDA_instructions<OP_mLdaScl>;
let isCodeGenOnly = 1 in {
	defm GPR : LDA_instructions<OP_mLdaScl_GPR>;
	defm PTR : LDA_instructions<OP_mLdaScl_PTR>;
	defm MOD : LDA_instructions<OP_mLdaScl_MOD>;
}

// Load from memory.
let Itinerary = II_LDA in {
// Note: ordering matters here because we want to merge references to ag_short!
def LDA_s8_ind : AIE_dmhb_lda<0b00, (outs GPR0_7:$reg), (ins PTR:$ptr), [], "lda.s8", "$reg, [$ptr]">,
	 ag_short_ind;
def LDA_u8_ind : AIE_dmhb_lda<0b01, (outs GPR0_7:$reg), (ins PTR:$ptr), [], "lda.u8", "$reg, [$ptr]">,
	 ag_short_ind;
def LDA_s16_ind : AIE_dmhb_lda<0b10, (outs GPR0_7:$reg), (ins PTR:$ptr), [], "lda.s16", "$reg, [$ptr]">,
	 ag_short_ind;
def LDA_u16_ind : AIE_dmhb_lda<0b11, (outs GPR0_7:$reg), (ins PTR:$ptr), [], "lda.u16", "$reg, [$ptr]">,
	 ag_short_ind;
def VLDA_ind : AIE_dmv_lda<(outs OP_mVn:$reg), (ins PTR:$ptr), [], "vlda", "$reg, [$ptr]">,
	 ag_short_ind;
def WVLDA_ind : AIE_dmw_lda_W<(outs VEC256:$reg), (ins PTR:$ptr), [], "vlda", "$reg, [$ptr]">,
	 ag_short_ind;
def LDA_s8_idx : AIE_dmhb_lda<0b00, (outs GPR0_7:$reg), (ins PTR:$ptr, mCS:$cs), [], "lda.s8", "$reg, [$ptr, $cs]">,
	 ag_short_idx;
def LDA_u8_idx : AIE_dmhb_lda<0b01, (outs GPR0_7:$reg), (ins PTR:$ptr, mCS:$cs), [], "lda.u8", "$reg, [$ptr, $cs]">,
	 ag_short_idx;
def LDA_s16_idx : AIE_dmhb_lda<0b10, (outs GPR0_7:$reg), (ins PTR:$ptr, mCS:$cs), [], "lda.s16", "$reg, [$ptr, $cs]">,
	 ag_short_idx;
def LDA_u16_idx : AIE_dmhb_lda<0b11, (outs GPR0_7:$reg), (ins PTR:$ptr, mCS:$cs), [], "lda.u16", "$reg, [$ptr, $cs]">,
	 ag_short_idx;
def VLDA_idx : AIE_dmv_lda<(outs OP_mVn:$reg), (ins PTR:$ptr, mCS:$cs), [], "vlda", "$reg, [$ptr, $cs]">,
	 ag_short_idx;
def WVLDA_idx : AIE_dmw_lda_W<(outs VEC256:$reg), (ins PTR:$ptr, mCS:$cs), [], "vlda", "$reg, [$ptr, $cs]">,
	 ag_short_idx;
def WVLDA_imm : AIE_dmw_lda_W<(outs VEC256:$reg), (ins PTR:$ptr, imm5x32:$imm), [], "vlda", "$reg, [$ptr], $imm">,
 ag_short_pstm_nrm_imm;
} // Itinerary II_LDA

// The LR register cannot be loaded through a regular LDA_SPIL_PTR instruction.  If the offset is small,
// then we can use a LDB_spis instruction, but otherwise need to fall back to a 2-instruction sequence.
// Ideally, we'd lay out the stack so that the LR spill slot is always next to SP, but this seems not
// as easy as it should be.
// We reserve r15 as a bounce register for the 2 instruction sequence.
// Note that defining an itinerary for this is tricky, so we give it the itinerary of the LDB_spis
// instruction and hope that things don't get too out of whack.
let Itinerary = II_LDBSP_R, hasSideEffects = 0, mayLoad = 1, mayStore = 0, Uses = [SP], Defs=[r15] in {
def LR_LOAD : Pseudo<(outs LR:$reg), (ins t17n_step4:$imm), [], "lda.lr", "${reg}, [sp, $imm]">;
}

let Itinerary = II_LDASP_R, Uses = [SP] in {
class VLDA_SPIL_AM <bits<1> val, bits<1> shft, immx4_negative offset, string lane>
    : AIE_dmw_lda_spil_mLoadAcc_AM<val, shft, (outs ACC384:$sd), (ins offset:$imm), [], "vlda.spil.48", "${sd}."#lane#", [sp, $imm]">;

def I384_LOAD : Pseudo<(outs ACC384:$sd), (ins t17n_step4:$imm), [], "vlda384bit", "${sd}, [sp, $imm]">;
def I768_LOAD : Pseudo<(outs ACC768:$sd), (ins t17n_step4:$imm), [], "vlda768bit", "${sd}, [sp, $imm]">;
def VLDA_SPIL_AM_LO_I48 : VLDA_SPIL_AM<0b0, 0b0, t17n_step4, "lo">;
def VLDA_SPIL_AM_HI_I48 : VLDA_SPIL_AM<0b1, 0b0, t17n_step4, "hi">;
} // II_LDASP_R, Uses SP

let Itinerary = II_LDASP_RW, Defs = [SP], Uses = [SP] in {
def VLDA_sp_imm : AIE_dmv_lda<(outs OP_mVn:$reg), (ins imm5x32:$imm), [], "vlda", "$reg, [sp], $imm">,
	 ag_short_pstm_sp_imm;
def WVLDA_sp_imm : AIE_dmw_lda_W<(outs VEC256:$reg), (ins imm5x32:$imm), [], "vlda", "$reg, [sp], $imm">,
	 ag_short_pstm_sp_imm;
def VLDA_AM_sp_imm : AIE_dmw_lda_AM<0b0, 0b0, (outs ACC384:$reg), (ins imm5x32:$imm), [], "vlda.48", "${reg}.lo, [sp], $imm">,
	 ag_short_pstm_sp_imm;
} // II_LDASP_RW, Defs SP, Uses SP
let Itinerary = II_LDASP_R, Uses = [SP] in {
def VLDA_spis : AIE_dmv_lda<(outs OP_mVn:$reg), (ins t12n_step16:$imm), [], "vlda", "$reg, [sp, $imm]">,
	 ag_short_pstm_spis;
def WVLDA_spis : AIE_dmw_lda_W<(outs VEC256:$reg), (ins t13n_step32:$imm), [], "vlda", "$reg, [sp, $imm]">,
	 ag_short_pstm_spis;
def VLDA_SPIL : AIE_dmv_lda_spil<(outs OP_mVn:$reg), (ins t17n_step4:$imm), [], "vlda.spil", "$reg, [sp, $imm]">;
def WVLDA_SPIL : AIE_dmw_lda_spil_mWa<(outs VEC256:$reg), (ins t17n_step4:$imm), [], "vlda.spil", "$reg, [sp, $imm]">;
def VLDA_AM_spis : AIE_dmw_lda_AM<0b0, 0b0, (outs ACC384:$reg), (ins t13n_step32:$imm), [], "vlda.48", "${reg}.lo, [sp, $imm]">,
	 ag_short_pstm_spis;
def I512_LOAD : Pseudo<(outs VEC512:$reg), (ins t17n_step4:$imm), [], "vlda512bit", "${reg}, [sp, $imm]">;
def I1024_LOAD : Pseudo<(outs VEC1024:$reg), (ins t17n_step4:$imm), [], "vlda1024bit", "${reg}, [sp, $imm]">;
} // II_LDASP_R, Uses SP

let Itinerary = II_LDB in {
def LDB_ind : AIE_dms_ldb<(outs GPR:$reg), (ins PTR:$ptr), [], "ldb", "$reg, [$ptr]">,
	 ag_short_ind;
def VLDB_ind : AIE_dmv_ldb<(outs OP_mVn:$reg), (ins PTR:$ptr), [], "vldb", "$reg, [$ptr]">,
	 ag_short_ind;
def WVLDB_ind : AIE_dmw_ldb<(outs VEC256:$reg), (ins PTR:$ptr), [], "vldb", "$reg, [$ptr]">,
	 ag_short_ind;
//def LDB_imm : AIE_dms_ldb<(outs GPR:$reg), (ins imm5x32:$imm), [], "ldb", "$reg, $imm">,
// ag_short_imm;
def LDB_idx : AIE_dms_ldb<(outs OP_mLdbScl:$reg), (ins PTR:$ptr, mCS:$cs), [], "ldb", "$reg, [$ptr, $cs]">,
	 ag_short_idx;
def VLDB_idx : AIE_dmv_ldb<(outs OP_mVn:$reg), (ins PTR:$ptr, mCS:$cs), [], "vldb", "$reg, [$ptr, $cs]">,
	 ag_short_idx;
def WVLDB_idx : AIE_dmw_ldb<(outs VEC256:$reg), (ins PTR:$ptr, mCS:$cs), [], "vldb", "$reg, [$ptr, $cs]">,
	 ag_short_idx;
} // Itinerary II_LDB
let Itinerary = II_LDBSP_RW, Defs = [SP], Uses = [SP] in {
def LDB_sp_imm : AIE_dms_ldb<(outs GPR:$reg), (ins imm5x32:$imm), [], "ldb", "$reg, [sp], $imm">,
	 ag_short_pstm_sp_imm;
def VLDB_sp_imm : AIE_dmv_ldb<(outs OP_mVn:$reg), (ins imm5x32:$imm), [], "vldb", "$reg, [sp], $imm">,
	 ag_short_pstm_sp_imm;
def WVLDB_sp_imm : AIE_dmw_ldb<(outs VEC256:$reg), (ins imm5x32:$imm), [], "vldb", "$reg, [sp], $imm">,
	 ag_short_pstm_sp_imm;
} // Itinerary II_LDBSP_RW, Defs SP, Uses SP
let Itinerary = II_LDBSP_R, Uses = [SP] in {
def LDB_spis : AIE_dms_ldb<(outs OP_mLdbScl:$reg), (ins c10n_step4:$imm), [], "ldb", "$reg, [sp, $imm]">,
	 ag_short_pstm_spis;
def VLDB_spis : AIE_dmv_ldb<(outs OP_mVn:$reg), (ins t12n_step16:$imm), [], "vldb", "$reg, [sp, $imm]">,
	 ag_short_pstm_spis;
def WVLDB_spis : AIE_dmw_ldb<(outs VEC256:$reg), (ins t13n_step32:$imm), [], "vldb", "$reg, [sp, $imm]">,
	 ag_short_pstm_spis;
} // Itinerary II_LDBSP_R, Uses SP

// Store to memory.
let Itinerary = II_VST_SPIL in {
let Uses = [SP] in {
class WVST_SPIL_AM <bits<1> val, bits<1> shft, immx4_negative offset, string lane>
    : AIE_dmw_sts_spil_mStoreAcc_AM<val, shft, (outs), (ins ACC384:$sd, offset:$imm), [], "vst.spil.48", "${sd}."#lane#", [sp, $imm]">;

def WVST_SPIL_AM_LO_I48 : WVST_SPIL_AM<0b0, 0b0, t17n_step4, "lo">;
def WVST_SPIL_AM_HI_I48 : WVST_SPIL_AM<0b1, 0b0, t17n_step4, "hi">;

//Pseudos
def I384_STORE : Pseudo<(outs), (ins ACC384:$sd, t17n_step4:$imm), [], "vst384bit", "${sd}, [sp, $imm]">;
def I768_STORE : Pseudo<(outs), (ins ACC768:$sd, t17n_step4:$imm), [], "vst768bit", "${sd}, [sp, $imm]">;
}
}

// Store to memory.
multiclass ST_instructions <RegisterOperand regclass> {
let Itinerary = II_STS in {
def ST_pstm_nrm_imm_#NAME : AIE_dms_sts<(outs), (ins regclass:$reg, PTR:$ptr, imm4x4:$imm),
	 [], "st", "$reg, [$ptr], $imm">, ag_short_pstm_nrm_imm;
def ST_ind_#NAME : AIE_dms_sts<(outs), (ins regclass:$reg, PTR:$ptr),
	 [], "st", "$reg, [$ptr]">,  ag_short_ind;
def ST_idx_#NAME  : AIE_dms_sts<(outs), (ins regclass:$reg, PTR:$ptr, mCS:$cs),
	 [], "st", "$reg, [$ptr, $cs]">,  ag_short_idx;
}
let Itinerary = II_ST_SPIL in {
let Uses = [SP] in {
let Defs = [SP] in
def ST_pstm_sp_imm_#NAME :  AIE_dms_sts<(outs), (ins regclass:$reg, imm5x32:$imm),
	 [], "st", "$reg, [sp], $imm">, ag_short_pstm_sp_imm;
def ST_spis_#NAME :         AIE_dms_sts<(outs), (ins regclass:$reg, c10n_step4:$imm),
	 [], "st", "$reg, [sp, $imm]">, ag_short_pstm_spis;
def ST_SPIL_#NAME :         AIE_dms_sts_spil<(outs), (ins regclass:$reg, t17n_step4:$imm),
	 [], "st.spil", "$reg, [sp, $imm]">;
}
}
}
defm "" : ST_instructions<OP_mSclSt>;
let isCodeGenOnly = 1 in {
	defm GPR : ST_instructions<OP_mSclSt_GPR>;
	defm PTR : ST_instructions<OP_mSclSt_PTR>;
	defm MOD : ST_instructions<OP_mSclSt_MOD>;
}

let Itinerary = II_STS_HB in {
def ST_s8_ind :          AIE_dmhb_sts<0b0, (outs), (ins GPR:$reg, PTR:$ptr),
	 [], "st.s8", "$reg, [$ptr]">,  ag_short_ind;
def ST_s16_ind :         AIE_dmhb_sts<0b1, (outs), (ins GPR:$reg, PTR:$ptr),
	 [], "st.s16", "$reg, [$ptr]">,  ag_short_ind;
}

let Itinerary = II_VSTS in {
def VST_pstm_nrm_imm : AIE_dmv_sts<(outs), (ins OP_mVn:$reg, PTR:$ptr, imm4x4:$imm),
	 [], "vst", "$reg, [$ptr], $imm">, ag_short_pstm_nrm_imm;
def VST_ind :          AIE_dmv_sts<(outs), (ins OP_mVn:$reg, PTR:$ptr),
	 [], "vst", "$reg, [$ptr]">,  ag_short_ind;
def WVST_ind :          AIE_dmw_sts_WG<(outs), (ins VEC256:$reg, PTR:$ptr),
	 [], "vst", "$reg, [$ptr]">,  ag_short_ind;
}

let Itinerary = II_STS_HB in {
def ST_s8_idx :          AIE_dmhb_sts<0b0, (outs), (ins GPR:$reg, PTR:$ptr, mCS:$cs),
	 [], "st.s8", "$reg, [$ptr, $cs]">,  ag_short_idx;
def ST_s16_idx :         AIE_dmhb_sts<0b1, (outs), (ins GPR:$reg, PTR:$ptr, mCS:$cs),
	 [], "st.s16", "$reg, [$ptr, $cs]">,  ag_short_idx;
}

let Itinerary = II_VSTS in {
def VST_idx :          AIE_dmv_sts<(outs), (ins OP_mVn:$reg, PTR:$ptr, mCS:$cs),
	 [], "vst", "$reg, [$ptr, $cs]">,  ag_short_idx;
def WVST_idx :          AIE_dmw_sts_WG<(outs), (ins VEC256:$reg, PTR:$ptr, mCS:$cs),
	 [], "vst", "$reg, [$ptr, $cs]">,  ag_short_idx;
}
let Itinerary = II_VST_SPIL in {
let Uses = [SP] in {
let Defs = [SP] in {
def VST_pstm_sp_imm :  AIE_dmv_sts<(outs), (ins OP_mVn:$reg, imm5x32:$imm),
	 [], "vst", "$reg, [sp], $imm">, ag_short_pstm_sp_imm;
def WVST_pstm_sp_imm :  AIE_dmw_sts_WG<(outs), (ins VEC256:$reg, imm5x32:$imm),
	 [], "vst", "$reg, [sp], $imm">, ag_short_pstm_sp_imm;
}
def VST_spis :         AIE_dmv_sts<(outs), (ins OP_mVn:$reg, t12n_step16:$imm),
	 [], "vst", "$reg, [sp, $imm]">, ag_short_pstm_spis;
def WVST_spis :         AIE_dmw_sts_WG<(outs), (ins VEC256:$reg, t13n_step32:$imm),
	 [], "vst", "$reg, [sp, $imm]">, ag_short_pstm_spis;
def WVST_spis_pack_signed :    AIE_dmv_sts_pack<0b1, (outs), (ins VEC256:$reg, t13n_step32:$imm),
	 [], "vst.pack.s8", "$reg, [sp, $imm]">, ag_short_pstm_spis;
def WVST_spis_pack_unsigned :    AIE_dmv_sts_pack<0b0, (outs), (ins VEC256:$reg, t13n_step32:$imm),
	 [], "vst.pack.u8", "$reg, [sp, $imm]">, ag_short_pstm_spis;
def WVST_dmw_spis_pack_signed :    AIE_dmw_sts_pack<0b1, (outs), (ins VEC512:$reg, t13n_step32:$imm),
	 [], "vst.pack.s8", "$reg, [sp, $imm]">, ag_short_pstm_spis;
def WVST_dmw_spis_pack_unsigned :    AIE_dmw_sts_pack<0b0, (outs), (ins VEC512:$reg, t13n_step32:$imm),
	 [], "vst.pack.u8", "$reg, [sp, $imm]">, ag_short_pstm_spis;
def VST_SPIL :        AIE_dmv_sts_spil<(outs), (ins OP_mVn:$reg, t17n_step4:$imm),
	 [], "vst.spil", "$reg, [sp, $imm]">;
def WVST_SPIL :       AIE_dmw_sts_spil_mWs<(outs), (ins VEC256:$reg, t17n_step4:$imm),
	 [], "vst.spil", "$reg, [sp, $imm]">;

def I512_STORE : Pseudo<(outs), (ins VEC512:$reg, t17n_step4:$imm), [], "vst512bit", "${reg}, [sp, $imm]">;
def I1024_STORE : Pseudo<(outs), (ins VEC1024:$reg, t17n_step4:$imm), [], "vst1024bit", "${reg}, [sp, $imm]">;
} // Uses SP
} // Itinerary II_VST_SPIL

// Pointer modification instructions.   Used to manipulate the stack pointer, for instance.
let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in {
let Uses = [SP], Defs = [SP] in {
let Itinerary = II_PADDA_sp in {
def PADDA_sp       : AIE_lda_ptr_inc<(outs)         , (ins MOD:$mod),               [], "padda", "[sp], $mod">,
	 ag_a_standalone, ag_short_pstm_sp;
def PADDA_sp_imm   : AIE_lda_ptr_inc<(outs)         , (ins imm5x32:$imm),           [], "padda", "[sp], $imm">,
	 ag_a_standalone, ag_short_pstm_sp_imm;
} // Itinerary II_PADDA_sp
let Itinerary = II_PADDB_sp in {
def PADDB_sp       : AIE_ldb_ptr_inc<(outs)         , (ins MOD:$mod),               [], "paddb", "[sp], $mod">,
	 ag_a_standalone, ag_short_pstm_sp;
def PADDB_sp_imm   : AIE_ldb_ptr_inc<(outs)         , (ins imm5x32:$imm),           [], "paddb", "[sp], $imm">,
	 ag_a_standalone, ag_short_pstm_sp_imm;
} // Itinerary II_PADDB_sp
}
}

let hasSideEffects = 0, mayLoad = 0, mayStore = 0, Constraints = "$ptr = $reg" in {
let Itinerary = II_PADDA in {
def PADDA_nrm      : AIE_lda_ptr_inc<(outs PTR:$reg), (ins PTR:$ptr, MOD:$mod),     [], "padda", "[$ptr], $mod">,
	 ag_a_standalone, ag_short_pstm_nrm;
def PADDA_nrm_imm  : AIE_lda_ptr_inc<(outs PTR:$reg), (ins PTR:$ptr, imm4x4:$imm), [], "padda", "[$ptr], $imm">,
	 ag_a_standalone, ag_short_pstm_nrm_imm;
// def PADDA_cyc      : AIE_lda_ptr_inc<(outs GPR:$reg), (ins GPR:$src, imm4x4:$imm), [], "padda", "[sp], $mod, $cyc">,
// 	 ag_a_standalone, ag_short_cyc;
} // Itinerary II_PADDA
let Itinerary = II_PADDB in {
def PADDB_nrm      : AIE_ldb_ptr_inc<(outs PTR:$reg), (ins PTR:$ptr, MOD:$mod),     [], "paddb", "[$ptr], $mod">,
	 ag_a_standalone, ag_short_pstm_nrm;
def PADDB_nrm_imm  : AIE_ldb_ptr_inc<(outs PTR:$reg), (ins PTR:$ptr, imm4x4:$imm), [], "paddb", "[$ptr], $imm">,
	 ag_a_standalone, ag_short_pstm_nrm_imm;
// def PADDB_cyc      : AIE_ldb_ptr_inc<(outs GPR:$reg), (ins GPR:$src, imm4x4:$imm), [], "paddb", "[sp], $mod, $cyc">,
// 	 ag_a_standalone, ag_short_cyc;
} // Itinerary II_PADDB
}

let hasSideEffects = 0, mayLoad = 0, mayStore = 0, Itinerary = II_PADDA in {
// Like PADDA_nrm_imm, but takes a 20-bit integer.  The PTR is expected to be a frameindex, and this will
// be replaced in eliminateFrameIndex with an appropriate code sequence.
def PADDA_pseudo  : Pseudo<(outs PTR:$reg), (ins PTR:$ptr, tuimm:$imm), [], "padda_pseudo", "[$ptr], $imm">;
}

def : Pat<(add i20:$s0, MOD:$mod),
	      (PADDA_nrm $s0, MOD:$mod)>;
def : Pat<(add iPTR:$s0, imm4x4:$imm),
	      (PADDA_nrm_imm $s0, imm4x4:$imm)> { let AddedComplexity = 200; }

//////
// Stack loads and stores

// This converts a generic FrameIndex into a TargetFrameIndex.
def frameindex_to_targetframeindex : SDNodeXForm<frameindex, [{
  auto FI = cast<FrameIndexSDNode>(N);
  return CurDAG->getTargetFrameIndex(FI->getIndex(), MVT::i20);
}]>;

def gi_frameindex_to_targetframeindex : GICustomOperandRenderer<"renderFrameIndex">,
  GISDNodeXFormEquiv<frameindex_to_targetframeindex>;

//  When accessing arguments from within a function,
//  we match load and store with a frame index.
//  When passing arguments at a call site, we have load and stores referring
//  directly to the stack pointer.  These loads and stores need to be legalized
//  correctly.
foreach Ty = [i32, f32, v8i1, v16i1] in {
	def : Pat<(Ty (load (i32 frameindex:$fi))),
			(LDA_SPIL_GPR (frameindex_to_targetframeindex $fi))>;
	def : Pat<(Ty (load (i20 frameindex:$fi))),
			(LDA_SPIL_GPR (frameindex_to_targetframeindex $fi))>;
}

def : Pat<(i20 (load (i20 frameindex:$fi))),
              (LDA_SPIL_PTR (frameindex_to_targetframeindex $fi))>;
def : Pat<(i20 (load (i32 frameindex:$fi))),
              (LDA_SPIL_PTR (frameindex_to_targetframeindex $fi))>;

foreach VecTy = [v16i8, v8i16, v4i32, v4f32] in {
  def : Pat<(VecTy (load (i32 frameindex:$fi))),
	      (VLDA_spis (frameindex_to_targetframeindex $fi))>;
  def : Pat<(VecTy (load (i20 frameindex:$fi))),
              (VLDA_spis (frameindex_to_targetframeindex $fi))>;
  def : Pat<(VecTy (load (i32 frameindex:$fi))),
	      (VLDB_spis (frameindex_to_targetframeindex $fi))>;
  def : Pat<(VecTy (load (i20 frameindex:$fi))),
              (VLDB_spis (frameindex_to_targetframeindex $fi))>;
}
foreach VecTy = [v32i8, v16i16, v8i32, v8f32] in {
  def : Pat<(VecTy (load (i32 frameindex:$fi))),
	      (WVLDA_spis (frameindex_to_targetframeindex $fi))>;
  def : Pat<(VecTy (load (i32 frameindex:$fi))),
	      (WVLDB_spis (frameindex_to_targetframeindex $fi))>;
  def : Pat<(VecTy (load (i20 frameindex:$fi))),
              (WVLDA_spis (frameindex_to_targetframeindex $fi))>;
  def : Pat<(VecTy (load (i20 frameindex:$fi))),
              (WVLDB_spis (frameindex_to_targetframeindex $fi))>;
}
foreach VecTy = [v64i8, v32i16, v16i32] in {
def : Pat<(VecTy (load (i20 frameindex:$fi))),
	      (I512_LOAD (frameindex_to_targetframeindex $fi))>;
def : Pat<(VecTy (load PTR:$rs1)),
              (INSERT_SUBREG
                (INSERT_SUBREG (VecTy (IMPLICIT_DEF)), (WVLDA_imm PTR:$rs1, (i32 32)), sub_256bit_lo),
                (WVLDA_ind PTR:$rs1),
                sub_256bit_hi)>;
}
foreach VecTy = [v128i8, v64i16, v32i32] in {
def : Pat<(VecTy (load (i20 frameindex:$fi))),
	      (I1024_LOAD (frameindex_to_targetframeindex $fi))>;
}
class VecLoadFromPTR<ValueType Rtype, ValueType type>
    : Pat<(Rtype (load PTR:$rs1)),
             (INSERT_SUBREG
               (INSERT_SUBREG
                 (Rtype  (IMPLICIT_DEF)),
                 (INSERT_SUBREG
                   (INSERT_SUBREG (type (IMPLICIT_DEF)), (WVLDA_imm PTR:$rs1, (i32 32)), sub_256bit_lo),
                   (WVLDA_imm PTR:$rs1, (i32 32)),
                   sub_256bit_hi),
                 sub_512bit_lo),
               (INSERT_SUBREG
                 (INSERT_SUBREG (type (IMPLICIT_DEF)), (WVLDA_imm PTR:$rs1, (i32 32)), sub_256bit_lo),
                 (WVLDA_ind PTR:$rs1),
                 sub_256bit_hi),
               sub_512bit_hi) >;
def load_ptr_v128i8 : VecLoadFromPTR<v128i8, v64i8>;
def load_ptr_v64i16 : VecLoadFromPTR<v64i16, v32i16>;
def load_ptr_v32i32 : VecLoadFromPTR<v32i32, v16i32>;

def : Pat<(v8i48 (load (i20 frameindex:$fi))),
	      (I384_LOAD (frameindex_to_targetframeindex $fi))>;
def : Pat<(v16i48 (load (i20 frameindex:$fi))),
	      (I768_LOAD (frameindex_to_targetframeindex $fi))>;

foreach Ty = [i32, f32, v8i1, v16i1] in {
	def : Pat<(store (Ty OP_mSclSt_GPR:$reg), (i32 frameindex:$fi)),
			(ST_SPIL_GPR OP_mSclSt_GPR:$reg, (frameindex_to_targetframeindex $fi))>;
	def : Pat<(store (Ty OP_mSclSt_GPR:$reg), (i20 frameindex:$fi)),
				(ST_SPIL_GPR OP_mSclSt_GPR:$reg, (frameindex_to_targetframeindex $fi))>;
}
foreach Ty = [i20] in {
	def : Pat<(store (Ty OP_mSclSt_PTR:$reg), (i32 frameindex:$fi)),
			(ST_SPIL_PTR OP_mSclSt_PTR:$reg, (frameindex_to_targetframeindex $fi))>;
	def : Pat<(store (Ty OP_mSclSt_PTR:$reg), (i20 frameindex:$fi)),
				(ST_SPIL_PTR OP_mSclSt_PTR:$reg, (frameindex_to_targetframeindex $fi))>;
}
// Direct stack access, without frame index.
// def : Pat<(load (add SP, c10n_step4:$imm)),
//  	      (LDA_spis c10n_step4:$imm)>;
// def : Pat<(load SP),
//  	      (LDA_spis 0)>;
// def : Pat<(store GPR:$reg, (add SP, c10n_step4:$imm)),
// 	      (ST_spis GPR:$reg, c10n_step4:$imm)>;
// def : Pat<(store GPR:$reg, SP),
// 	      (ST_spis GPR:$reg, 0)>;
foreach VecTy = [v16i8, v8i16, v4i32, v4f32] in {
  def : Pat<(store (VecTy VEC128:$reg), (i32 frameindex:$fi)),
	      (VST_spis VEC128:$reg, (frameindex_to_targetframeindex $fi))>;
  def : Pat<(store (VecTy VEC128:$reg), (i20 frameindex:$fi)),
              (VST_spis VEC128:$reg, (frameindex_to_targetframeindex $fi))>;
}
foreach VecTy = [v32i8, v16i16, v8i32, v8f32] in {
  def : Pat<(store (VecTy VEC256:$reg), (i32 frameindex:$fi)),
	      (WVST_spis VEC256:$reg, (frameindex_to_targetframeindex $fi))>;
  def : Pat<(store (VecTy VEC256:$reg), (i20 frameindex:$fi)),
              (WVST_spis VEC256:$reg, (frameindex_to_targetframeindex $fi))>;
}
foreach VecTy = [v64i8, v32i16, v16i32] in {
def : Pat<(store (VecTy VEC512:$reg), (i20 frameindex:$fi)),
	   (I512_STORE VEC512:$reg, (frameindex_to_targetframeindex $fi))>;
}
foreach VecTy = [v128i8, v64i16, v32i32] in {
def : Pat<(store (VecTy VEC1024:$reg), (i20 frameindex:$fi)),
	   (I1024_STORE VEC1024:$reg, (frameindex_to_targetframeindex $fi))>;
}
def : Pat<(store (v8i48 ACC384:$reg), (i20 frameindex:$fi)),
	   (I384_STORE ACC384:$reg, (frameindex_to_targetframeindex $fi))>;
def : Pat<(store (v16i48 ACC768:$reg), (i20 frameindex:$fi)),
	   (I768_STORE ACC768:$reg, (frameindex_to_targetframeindex $fi))>;

//srs and store
// FIXME: VST_SRS{SU}_spis have an output operand "modc" of type MC0. The patterns below do not declare this
//        output operand, which is ignored by SelectionDAG, but not GlobalISel. Should the MC0 be rather an
//        implicit output operand?
// def : Pat<(store (int_aie_bsrs_v16i8_v16acc48 (v16i48 ACC768:$reg), (i32 GPR:$shft)), (i20 frameindex:$fi)),
// 	   (VST_SRSS_spis ACC768:$reg, (ADD_S_RI_ILV64 GPR:$shft, (i32 1)), (frameindex_to_targetframeindex $fi))>;
// def : Pat<(store (int_aie_ubsrs_v16i8_v16acc48 (v16i48 ACC768:$reg), (i32 GPR:$shft)), (i20 frameindex:$fi)),
// 	   (VST_SRSU_spis ACC768:$reg, (ADD_S_RI_ILV64 GPR:$shft, (i32 1)), (frameindex_to_targetframeindex $fi))>;

// def : Pat<(store GPR:$reg, (add r0, c10n_step4:$imm)),
//  	      (ST_spis GPR:$reg, c10n_step4:$imm)>;
// def : Pat<(store GPR:$reg, SP),
// 	      (ST_spis GPR:$reg, 0)>;

// In eliminateFrameIndex, this will get converted to a sequence of instructions.  The
// exact sequence of instructions depends on the size of the frame offset.  The logic is
// implemented in AIEFrameLowering::adjustReg.
// The DAGCombiner likes to convert ADD -> OR when there are no shared bits.  There's not
// a good way to selectively disable this, unfortunately.  This pattern undoes the 'optimization'
// assuming there are no shared bits, but this is unsafe if we somehow get an OR for other reasons.
def : Pat<(i20 (or (i20 frameindex:$fi), tuimm:$imm)),
        (i20 (PADDA_pseudo (i20 (frameindex_to_targetframeindex $fi)), tuimm:$imm))>;
def : Pat<(add frameindex:$fi, tuimm:$imm),
        (i20 (PADDA_pseudo (i20 (frameindex_to_targetframeindex $fi)), tuimm:$imm))>;
def : Pat<(i20 frameindex:$fi),
        (i20 (PADDA_pseudo (i20 (frameindex_to_targetframeindex $fi)), (i20 0)))>;
def : Pat<(ptr0 frameindex:$fi),
        (i20 (PADDA_pseudo (i20 (frameindex_to_targetframeindex $fi)), (i20 0)))>;

// Implement simple loads in AIE.
multiclass LdPat<PatFrag LoadOp, AIEInst InstInd, AIEInst InstIdx, ValueType ResType, ValueType ResTypeOut = ResType> {
  def : Pat<(ResTypeOut (LoadOp PTR:$rs1)), (InstInd PTR:$rs1)>;
  // def : Pat<(LoadOp AddrFI:$rs1), (Inst AddrFI:$rs1, 0)>;
  // MOV_S12 can load directly to a CS register
  def : Pat<(ResTypeOut (LoadOp (add GPR:$rs1, simm12:$imm))),
            (InstIdx GPR:$rs1, (MOV_S12 simm12:$imm))>;
  // MOV_S20 cannot load directly to a CS register: requires another
  // register move
  def : Pat<(ResTypeOut (LoadOp (add GPR:$rs1, simm20:$imm))),
            (InstIdx GPR:$rs1, (MOV_S20 simm20:$imm))>;
  // def : Pat<(LoadOp (add AddrFI:$rs1, simm12:$imm12)),
  //           (Inst AddrFI:$rs1, simm12:$imm12)>;
  // def : Pat<(LoadOp (IsOrAdd AddrFI:$rs1, simm12:$imm12)),
  //           (Inst AddrFI:$rs1, simm12:$imm12)>;
}
foreach Ty = [i32, f32] in {
  defm : LdPat<load, LDA_ind_GPR, LDA_idx_GPR, Ty>;
  defm : LdPat<load, LDB_ind, LDB_idx, Ty>;
}
def :  Pat<(ptr0 (load PTR:$rs1)), (LDA_ind_GPR PTR:$rs1)>;
defm : LdPat<load, LDA_u8_ind, LDA_u8_idx, v8i1>;
defm : LdPat<load, LDA_u16_ind, LDA_u16_idx, v16i1>;
defm : LdPat<load, LDA_ind_PTR, LDA_idx_PTR, i20>;
defm : LdPat<load, LDA_ind_MOD, LDA_idx_MOD, i20>;
defm : LdPat<load, LDB_ind, LDB_idx, i20>;
foreach Ty = [i32, i20] in {
defm : LdPat<sextloadi16, LDA_s16_ind, LDA_s16_idx, Ty>;
defm : LdPat<extloadi16 , LDA_s16_ind, LDA_s16_idx, Ty>;
defm : LdPat<zextloadi16, LDA_u16_ind, LDA_u16_idx, Ty>;
defm : LdPat<sextloadi8 , LDA_s8_ind , LDA_s8_idx , Ty>;
defm : LdPat<extloadi8  , LDA_s8_ind , LDA_s8_idx , Ty>;
defm : LdPat<zextloadi8 , LDA_u8_ind , LDA_u8_idx , Ty>;
}

multiclass StPat<PatFrag StoreOp, AIEInst InstInd, AIEInst InstIdx, ValueType type> {
  def : Pat<(StoreOp (type GPR:$rs2), PTR:$rs1), (InstInd GPR:$rs2, PTR:$rs1)> { /*quiet warning*/ AIEInst dummy=InstIdx; }
  // def : Pat<(StoreOp GPR:$rs2, AddrFI:$rs1), (Inst GPR:$rs2, AddrFI:$rs1, 0)>;
  def : Pat<(StoreOp (type GPR:$rs2), (add GPR:$rs1, simm12:$imm)),
            (InstIdx GPR:$rs2, GPR:$rs1, (MOV_S12 simm12:$imm))>;
  def : Pat<(StoreOp (type GPR:$rs2), (add GPR:$rs1, simm20:$imm)),
            (InstIdx GPR:$rs2, GPR:$rs1, (MOV_S20 simm20:$imm))>;
//   def : Pat<(StoreOp (type GPR:$rs2), (add AddrFI:$rs1, simm12:$imm12)),
//             (InstIdx $rs2, $rs1, $imm12)>;
  // def : Pat<(StoreOp GPR:$rs2, (IsOrAdd AddrFI:$rs1, simm12:$imm12)),
  //           (Inst GPR:$rs2, AddrFI:$rs1, simm12:$imm12)>;
}
foreach Ty = [i32, f32] in {
	defm : StPat<store, ST_ind_GPR, ST_idx_GPR, Ty>;
}
foreach Ty = [i20] in {
	defm : StPat<store, ST_ind_PTR, ST_idx_PTR, Ty>;
}
def :  Pat<(store (ptr0 PTR:$rs2), PTR:$rs1), (ST_ind_PTR PTR:$rs2, PTR:$rs1)>;
def :  Pat<(store (i20 MOD:$rs2), PTR:$rs1), (ST_ind_MOD MOD:$rs2, PTR:$rs1)>;
def :  Pat<(store (ptr0 GPR:$rs2), PTR:$rs1), (ST_ind_GPR GPR:$rs2, PTR:$rs1)>;
defm : StPat<store, ST_s8_ind, ST_s8_idx, v8i1>;
defm : StPat<store, ST_s16_ind, ST_s16_idx, v16i1>;
foreach Ty = [i32, i20] in {
defm : StPat<truncstorei8, ST_s8_ind, ST_s8_idx, Ty>;
defm : StPat<truncstorei16, ST_s16_ind, ST_s16_idx, Ty>;
}

multiclass LdVecPat<PatFrag LoadOp, AIEInst Inst, ValueType ResType> {
  def : Pat<(ResType (LoadOp PTR:$rs1)), (Inst PTR:$rs1)>;
  def : Pat<(ResType (LoadOp i20:$rs1)), (Inst i20:$rs1)>;
  // def : Pat<(LoadOp AddrFI:$rs1), (Inst AddrFI:$rs1, 0)>;
  // def : Pat<(LoadOp (add GPR:$rs1, simm12:$imm12)),
  //           (Inst GPR:$rs1, simm12:$imm12)>;
  // def : Pat<(LoadOp (add AddrFI:$rs1, simm12:$imm12)),
  //           (Inst AddrFI:$rs1, simm12:$imm12)>;
  // def : Pat<(LoadOp (IsOrAdd AddrFI:$rs1, simm12:$imm12)),
  //           (Inst AddrFI:$rs1, simm12:$imm12)>;
}
foreach VecTy = [v16i8, v8i16, v4i32, v4f32] in {
  defm : LdVecPat<load, VLDA_ind, VecTy>;
  defm : LdVecPat<load, VLDB_ind, VecTy>;
}
foreach VecTy = [v32i8, v16i16, v8i32, v8f32] in {
  defm : LdVecPat<load, WVLDA_ind, VecTy>;
  defm : LdVecPat<load, WVLDB_ind, VecTy>;
}
multiclass StVecPat<PatFrag StoreOp, AIEInst Inst, ValueType StTy> {
  def : Pat<(StoreOp StTy:$rs2, PTR:$rs1), (Inst StTy:$rs2, PTR:$rs1)>;
  // def : Pat<(StoreOp StTy:$rs2, AddrFI:$rs1), (Inst StTy:$rs2, AddrFI:$rs1, 0)>;
  // def : Pat<(StoreOp StTy:$rs2, (add GPR:$rs1, simm12:$imm12)),
  //           (Inst StTy:$rs2, GPR:$rs1, simm12:$imm12)>;
  // def : Pat<(StoreOp StTy:$rs2, (add AddrFI:$rs1, simm12:$imm12)),
  //           (Inst StTy:$rs2, AddrFI:$rs1, simm12:$imm12)>;
  // def : Pat<(StoreOp StTy:$rs2, (IsOrAdd AddrFI:$rs1, simm12:$imm12)),
  //           (Inst StTy:$rs2, AddrFI:$rs1, simm12:$imm12)>;
}
foreach VecTy = [v16i8, v8i16, v4i32, v4f32] in {
  defm : StVecPat<store, VST_ind, VecTy>;
}
foreach VecTy = [v32i8, v16i16, v8i32, v8f32] in {
  defm : StVecPat<store, WVST_ind, VecTy>;
}

let Itinerary = II_ALU in
let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in {

// NOP32, NOP64, NOP96, NOP128 are kind of "meta-instructions". They are used
// to model a VLIW format filled with NOPs. They are now composed of non-
// concrete bitsets, to avoid the usage of 64-bit+ fields composed of 0. As
// explained further below, the non-concrete bitsets act, as if we were using
// a string filled with 0 on the code emission side and as if we don't care
// about their value on the disassembler side. They will be thus never
// disassembled.
def NOP128 : AIE_instr128<(outs), (ins), [], "nop128", ""> {
  // NOTE: even though we switch for the APInt interface on the disassembler
  // side, the usage of uint64_t temporaries (automatically generated by the
  // DecoderEmitter) prevent us to extract fields where the size is greater
  // than 64-bit. Brief history below:
  // https://reviews.llvm.org/D52100 introduced the APInt interface with some
  // limitations: "no string of bits extracted from the encoding may exceeed
  // 64-bits. Fields are still permitted to exceed 64-bits so long as they
  // aren't one contiguous string of bits."
  // Then, this one https://reviews.llvm.org/D98046 came up and breaked the
  // previous assumption "by always using a uint64_t tmp in the generated
  // decodeToMCInst" and left few guidelines as a TODO in
  // DecoderEmitter::emitDecoderFunction.
  // For now (aie-16.0), we must use tricks to encode 64-bit+ fields.

  // The extra keyword "field" below means the bitset can be a non-concrete
  // value (see RecordVal::FieldKind::FK_NonconcreteOK).
  // If the bitset is left undefined, it basically means that we don't care of
  // the value of these fields:
  // - on the MCCodeEmitter side, zeroes will be emitted ;
  // - on the Disassembler side, it will not look at the value of these fields,
  //   making the identification of the nop128 less restrictive than its
  //   equivalent Composite instruction, thus nop128 will be checked after the
  //   Composite instruction (at the end of the state machine). As we will
  //   always find a composite instruction to model it, it should never be
  //   decoded in normal circumstances.

  field bits<64> low_field;
  field bits<63> high_field;
  let instr128{0-63} = low_field;
  let instr128{64-126} = high_field;
  // NOTE: If we define low_field and high_field, TableGen will factor their
  // bitsets values to end up with a 127-bit wide value, which is currently
  // illegal on the disassembler side.
}

def NOP96 : AIE_i96_novec<(outs), (ins), [], "nop96", ""> {
	field bits<64> low_field;
  field bits<27> high_field;
  // 91-bit wide - same problem as the NOP128
  let i96_novec = {low_field, high_field};
}

// NOTE: for the NOP32 and the NOP64, we are using non concrete bitsets here as
// well (even if it's not mandatory as their size is less than 64-bit) to
// uniformize the way the instructions are decoded. Currently, NOP64 will be
// decoded as an equivalent Composite instruction.
def NOP64 : AIE_i64_novec<(ins), "nop64", ""> {
  field bits<59> plain_field;
  let i64_novec = {plain_field};
}

// Will be decoded as a NOP_LNG
def NOP32 : AIE_lng<(outs), (ins), [], "nop32", ""> {
  field bits<28> plain_field;
  let lng_base = {plain_field};
}

// Set of NOPs for each slot, described as a Standalone instruction
def NOP_LNG : AIE_lng<(outs), (ins), [], "nop", ""> {
	// NOP representative of the LNG Slot
	let isSlotNOP = true;
  let lng_base = 0;
}

def NOP_ALU : AIE_alu_format<(outs), (ins), [], "nop", ""> {
	// NOP representative of the ALU Slot
	let isSlotNOP = true;
    let alu_base = 0;
}

def NOP_LDA : AIE_i32_lda<(outs), (ins), [], "nop", ""> {
	// NOP representative of the LDA Slot
	let isSlotNOP = true;
    let lda_base = 0;
}

def NOP_LDB : AIE_ldb_base<(outs), (ins), [], "nop", ""> {
	// NOP representative of the LDB Slot
	let isSlotNOP = true;
    let ldb_base = 0;
}

def NOP_MV0 : AIE_mv0_base<(outs), (ins), [], "nop", ""> {
	// NOP representative of the MV0 Slot
	let isSlotNOP = true;
  let mv0_base = 0;
}

def NOP_MV1 : AIE_mv1_base<(outs), (ins), [], "nop_mv1", ""> {
	// NOP representative of the MV1 Slot
	let isSlotNOP = true;
  let mv1_base = 0;
}

def NOP_ST : AIE_st_base<(outs), (ins), [], "nop", ""> {
	// NOP representative of the ST Slot
	let isSlotNOP = true;
  let st_base = 0;
}

// NOP_VECSTRM, NOP_VECSHFT, NOP_VECSHRT have the same encoding as NOP_VECM.
// To avoid conflicts, we modified slightly their encoding (flipping the first
// bit). It raises others issues (regarding the decoding of the NOPs, check
// disassembler_ambiguity.mir), which can be solved completely only by using a
// different decoding table (check the instructions written for AIE_vec_dpd in
// InstrFormats.td) for each slot and using the same in-slot encoding for the
// NOPs in the same family.
def NOP_VECSTRM : AIE_vec_strm<(outs), (ins), [], "nop_vecstrm", ""> {
	// NOP representative of the VECSTRM Slot
  let isSlotNOP = true;
  // FIXME: flip first "don't care" bit at 0
  let vecstrm_base = {/*xxxx*/0b1000, 0b0};
}

def NOP_VECSHFT : AIE_vec_shft<(outs), (ins), [], "nop_vecshft", ""> {
	// NOP representative of the VECSHFT Slot
  let isSlotNOP = true;
  // FIXME: flip first "don't care" bit at 0
  let vecshft_base = {/*xxxxx0*/0b10000, 0b0};
}

def NOP_VECSHRT : AIE_vec_short<(outs), (ins), [], "nop_vecshrt", ""> {
	// NOP representative of the VECSHRT Slot
  let isSlotNOP = true;
  // FIXME: flip first "don't care" bit at 0
  let vecshrt_base = {0b00, /*x*/0b1, /*xxxx*/0b0000,	/*xxxx*/0b0000};
}

def NOP_VECM : AIE_vec_med<(outs), (ins), [], "nop_vecm", ""> {
  // NOP representative of the VECM Slot
  // NOTE: There is only one NOP defined in the ISA and plays the role of the
  // NOP instruction for every vector slots:
  // VECA -> VECM -> VECSHORT -> NOP
  // As it is not possible to model a multi-slot instruction, we define for
  // each vector slot its own NOP. It works well with NOP_VECA as it is relying
  // on a instr64 format but not so well with the vec_dpd sub-slots (as they
  // are relying on the same instr32 format).
  let isSlotNOP = true;
  let vecm_base = 0;
}

// Same nML encoding as NOP_MV1 -> first bit flipped to avoid conflict.
// FIXME: define it with 0 everywhere in mv1ssb_base (so it can match NOP_MV1),
// requires to split the instruction in a different DecoderTable (by rewriting
// the format on which it relies, see the instructions in AIEInstrFormats.td)
// and modify bits out-of-the-slot if their are still decoding conflict.
def NOP_MV1SSB : AIE_mv1ssb_base<(outs), (ins), [], "nop_mv1ssb", ""> {
  let isSlotNOP = true;
  // FIXME: flip first "don't care" bit at 0
  let mv1ssb_base = {/*xxxxx*/0b10000, 0b00};
}

// NOTE: There is no conflict with NOP_VECM
def NOP_VECA : AIE_veca_base<(outs), (ins), [], "nop", ""> {
  // NOP representative of the VECA Slot
  let isSlotNOP = true;
  let veca_base = 0;
}
// TODO (if necessary): define a NOP for MV1_SSB (same problem as VECA/VECM
// but with MV1).

def DONE : AIE_alu_format<(outs), (ins), [], "done", ""> {
	 bits<14> unknown14 = 0;
	 let alu_base = {0b00100, unknown14};
}
def NOP : AIE_instr16<(outs), (ins), [], "nop", ""> {
	 let instr16 = 0;
}
}

// Reads in E1, 5 branch delay slots.
let hasDelaySlot = 1 in
let hasSideEffects = 1, mayLoad = 0, mayStore = 0,
	 isBarrier = 1, isBranch = 1, isTerminator = 1 in {
def J : AIE_j_instr<(outs), (ins call_symbol:$addr), [], "j", "$addr">; // br pattern?
def JA : AIE_ja_instr<(outs), (ins mCB:$dst), [(brind mCB:$dst)], "ja", "$dst">;
}

// Reads in E1, writes LR in E4, 5 branch delay slots.
// Note that instruction scheduling cannot moved other operations around CALLs
let hasDelaySlot = 1, Defs = [lr] in
let hasSideEffects = 1, mayLoad = 0, mayStore = 0,
	 isBarrier = 0, isBranch = 0, isCall = 1, isTerminator = 0 in {
def JAL : AIE_jal_instr<(outs), (ins call_symbol:$addr), [], "jal", "$addr">; // br pattern?
def JAL_IND : AIE_jal_ind_instr<(outs), (ins mCB:$reg), [], "jal", "$reg">; // brind pattern?
def JAL64 : AIE_i64_jal_instr<(ins call_symbol:$addr), "jal", "$addr">; // br pattern?
}

let hasSideEffects = 1, mayLoad = 0, mayStore = 0, Itinerary = II_RET,
	 isBarrier = 1, isReturn = 1, isTerminator = 1, Uses = [lr] in {
def RET : AIE_instr16<(outs), (ins), [], "ret lr", ""> {
   let instr16 = {0b0000000, 0b1000000};
}
}

/////////////////////////////////////////////////////////////////////////////////////
//  Pseudo-instructions

// Target-independent type requirements, but with target-specific formats.
def SDT_CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>,
                                       SDTCisVT<1, i32>]>;
def SDT_CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i32>,
                                     SDTCisVT<1, i32>]>;

// Target-dependent type requirements.
def SDT_AIECall     : SDTypeProfile<0, -1, [SDTCisVT<0, i32>]>;
def SDT_AIESelectCC : SDTypeProfile<1, 5, [SDTCisSameAs<1, 2>,
                                             SDTCisSameAs<0, 4>,
                                             SDTCisSameAs<4, 5>]>;
def SDT_AIEMov : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>]>;
def SDT_AIEStreamWrite : SDTypeProfile<1, 1, [SDTCisVT<0, i32>]>;
def SDT_AIEStreamRead  : SDTypeProfile<1, 2, [SDTCisVT<0, i32>, SDTCisVT<0, i32>]>;
def SDT_AIEGPRcast : SDTypeProfile<1, 1, [SDTCisVT<0, i32>]>;
def SDT_AIEStackSave : SDTypeProfile<0, 2, [SDTCisVT<1, i32>]>;
def SDT_AIEStackRestore : SDTypeProfile<1, 1, [SDTCisVT<0, i32>]>;

// Target-independent nodes, but with target-specific formats.
def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_CallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_CallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;


def aie_call      : SDNode<"AIEISD::CALL", SDT_AIECall,
                             [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue,
                               SDNPVariadic]>;
def aie_ret_flag  : SDNode<"AIEISD::RET_FLAG", SDTNone,
                             [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;
def aie_tail      : SDNode<"AIEISD::TAIL", SDT_AIECall,
                             [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue,
                              SDNPVariadic]>;
def aie_get_ss    : SDNode<"AIEISD::GET_SS", SDT_AIEStreamRead,
                             []>;
def aie_put_ms    : SDNode<"AIEISD::PUT_MS", SDT_AIEStreamWrite,
                             []>;

// This will get converted into a realized immediate value representing the
// Global Address.
def aie_global_address : SDNode<"AIEISD::GLOBALADDRESSWRAPPER", SDT_AIEMov, []>;

// Cast from i32 to other data types supported in GPRs.  This is important
// because many of the other types (e.g. v8i1) don't have a corresponding
// supported integer type.
def aie_gpr_cast  : SDNode<"AIEISD::GPR_CAST", SDT_AIEGPRcast, []>;

// Save a value on the stack.  Takes a value and an i32 offset.
def aie_stack_save  : SDNode<"AIEISD::STACK_SAVE", SDT_AIEStackSave, [SDNPHasChain, SDNPMayStore]>;

// Restore a value from the stack.  Takes an i32 offset.
def aie_stack_restore  : SDNode<"AIEISD::STACK_RESTORE", SDT_AIEStackRestore, [SDNPHasChain, SDNPMayLoad]>;

// Pessimistically assume the stack pointer will be clobbered
let Defs = [SP], Uses = [SP] in {
def ADJCALLSTACKUP   : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                              [(callseq_start timm:$amt1, timm:$amt2)]>;
def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                              [(callseq_end timm:$amt1, timm:$amt2)]>;
}

let hasDelaySlot = 1, Itinerary = II_RET in
let isBarrier = 1, isReturn = 1, isTerminator = 1, Uses = [lr] in
def PseudoRET : Pseudo<(outs), (ins), [(aie_ret_flag)]>,
                PseudoInstExpansion<(RET)>;

let hasDelaySlot = 1 in
let isBarrier = 1, isBranch = 1, isTerminator = 1 in
def PseudoBR : Pseudo<(outs), (ins addr20:$addr), [(br bb:$addr)]>,
					PseudoInstExpansion<(J call_symbol:$addr)>;

// PseudoTAIL is a pseudo instruction modeling a tail call that will
// eventually expand to an unconditional jump 'j'.
// The main reason for this is that the jump instruction is modeled as a
// branch, expecting to target another basic block, while the tail call
// is modeled as a return, exiting for another function.  This enables
// prolog-epilog-inserter to do the right thing.
let hasDelaySlot = 1 in
let hasSideEffects = 1, mayLoad = 0, mayStore = 0,
	 isBarrier = 1, isReturn = 1, isTerminator = 1 in
def PseudoTAIL : Pseudo<(outs), (ins call_symbol:$addr), []>,
					PseudoInstExpansion<(J call_symbol:$addr)>;

// Lower aie_call to jal
def : Pat<(aie_call texternalsym:$func), (JAL texternalsym:$func)>;
def : Pat<(aie_call tglobaladdr:$func),  (JAL tglobaladdr:$func)>;

// Lower aie_tail to a pseudo, which will eventually be replaced with 'j'.
def : Pat<(aie_tail texternalsym:$func), (PseudoTAIL texternalsym:$func)>;
def : Pat<(aie_tail tglobaladdr:$func),  (PseudoTAIL tglobaladdr:$func)>;

// Lower global addresses to MOV instruction
def : Pat<(aie_global_address tglobaladdr:$addr),
           (i20 (MOV_U20_I20 tglobaladdr:$addr))>;
def : Pat<(aie_global_address tglobaltlsaddr:$addr),
           (i20 (MOV_U20_I20 tglobaltlsaddr:$addr))>;

def : Pat<(add (aie_global_address tglobaladdr:$addr), (i20 GPR:$rs2)),
           (ADD (MOV_U20_I20 tglobaladdr:$addr), GPR:$rs2)>;

// Replace this cast with a COPY that should get eliminated
def : Pat<(aie_gpr_cast (i32 GPR:$val)),
           (COPY GPR:$val)>;

// Lower stack saves and restores to an appropriate machine instruction
def : Pat<(aie_stack_save (i32 OP_mMvScl_GPR:$reg), (i32 t17n_step4:$imm)),
	      (ST_SPIL_GPR OP_mMvScl_GPR:$reg, t17n_step4:$imm)>;
def : Pat<(aie_stack_save (f32 OP_mMvScl_GPR:$reg), (i32 t17n_step4:$imm)),
	      (ST_SPIL_GPR OP_mMvScl_GPR:$reg, t17n_step4:$imm)>;
def : Pat<(aie_stack_save (i20 OP_mMvScl_PTR:$reg), (i32 t17n_step4:$imm)),
	      (ST_SPIL_PTR OP_mMvScl_PTR:$reg, t17n_step4:$imm)>;
def : Pat<(aie_stack_restore (i32 t17n_step4:$imm)),
	      (LDA_SPIL_GPR t17n_step4:$imm)>;

// Lock Instrinsic patterns
// The lock can come from a register.
class LockRegIntrinsicPat<Intrinsic intrin, AIEInst Inst>
    : Pat<(intrin GPR:$lock, GPR:$val),
          (Inst GPR:$lock, GPR:$val, 1)>;
// The lock can come from an immediate.
class LockImmIntrinsicPat<Intrinsic intrin, AIEInst Inst>
    : Pat<(intrin uimm6:$lock, GPR:$val),
          (Inst uimm6:$lock, GPR:$val, 1)>;

def : LockRegIntrinsicPat<int_aie_lock_acquire_reg, ACQ_reg>;
def : LockImmIntrinsicPat<int_aie_lock_acquire_reg, ACQ_imm>;
def : LockRegIntrinsicPat<int_aie_lock_release_reg, REL_reg>;
def : LockImmIntrinsicPat<int_aie_lock_release_reg, REL_imm>;

// Bitget/Bitset Instrinsics patterns
// bitset order is odd: dst[idx] = src[0]
def : Pat<(int_aie_bitset GPR:$dst, GPR0_7:$src, t05u:$idx),
          (BITSET GPR:$dst, t05u:$idx, GPR0_7:$src)>;
def : Pat<(int_aie_bitget GPR0_7:$src, t05u:$idx),
          (BITGET GPR0_7:$src, t05u:$idx)>;
def : Pat<(int_aie_bitset_md0 t05u:$idx, GPR0_7:$src),
          (BITSET_MD0 t05u:$idx, GPR0_7:$src)>;
def : Pat<(int_aie_bitset_md1 t05u:$idx, GPR0_7:$src),
          (BITSET_MD1 t05u:$idx, GPR0_7:$src)>;
def : Pat<(int_aie_bitget_mc0 t05u:$idx),
          (BITGET_MC0 t05u:$idx)>;
def : Pat<(int_aie_bitget_mc1 t05u:$idx),
          (BITGET_MC1 t05u:$idx)>;

// PacketHeader Instrinsic patterns (int_aie_packet_header, int_aie_ctrl_packet_header)
class GenPcktHeaderIntrinsicPat<AIEInst inst, int md0idx> :
		Pat<(int_aie_put_ms uimm1:$stream,
                          (int_aie_packet_header GPR:$reg, t03u:$type)),
          (inst (BITSET_MD0 md0idx, (MOV_S12 uimm1:$stream)),
									GPR:$reg, t03u:$type, /*tlast*/0)>;
def : GenPcktHeaderIntrinsicPat<GEN_PCKT_HEADER_imm0, 10>;
def : GenPcktHeaderIntrinsicPat<GEN_PCKT_HEADER_imm1, 11>;

class GenCtrlPcktHeaderIntrinsicPat<AIEInst inst, int md0idx> :
		Pat<(int_aie_put_ms uimm1:$stream,
				  (int_aie_ctrl_packet_header MOD:$addr, t03u:$nw, t02u:$op, GPR:$retid)),
          (inst (BITSET_MD0 md0idx, (MOV_S12 uimm1:$stream)),
									MOD:$addr, t03u:$nw, t02u:$op, GPR:$retid, /*tlast*/0)>;
def : GenCtrlPcktHeaderIntrinsicPat<GEN_CTRL_PCKT_HEADER_imm0, 10>;
def : GenCtrlPcktHeaderIntrinsicPat<GEN_CTRL_PCKT_HEADER_imm1, 11>;


// So, in theory, something like the below should work.  However it turns
// out that the TwoAddressConversion doesn't support explicit registers as
// the source address.
// def : Pat<(int_aie_get_ss uimm1:$stream),
//           (MV_SSA2SCL_0 (BITSET md0, 8, (MOV_S12 uimm1:$stream)))>;
// def : Pat<(int_aie_put_ms uimm1:$stream, GPR:$reg),
//           (MV_SCL2MSA_imm0 (BITSET md0, 10, (MOV_S12 uimm1:$stream)), GPR:$reg, 0)>;

// StreamRead Intrinsic patterns (int_aie_get_ss)
class StreamReadIntrinsicPat<AIEInst inst, int md0idx>
    : Pat<(int_aie_get_ss uimm1:$stream),
          (inst (BITSET_MD0 md0idx, (MOV_S12 uimm1:$stream)))>;
def : StreamReadIntrinsicPat<MV_SSA2SCL_0, 8>;
def : StreamReadIntrinsicPat<MV_SSA2SCL_1, 9>;
def : StreamReadIntrinsicPat<MV_SSB2SCL_0, 8>;
def : StreamReadIntrinsicPat<MV_SSB2SCL_1, 9>;

// StreamWrite Intrinsic patterns (int_aie_put_ms)
class StreamWriteIntrinsicPat<AIEInst inst, int md0idx>
    : Pat<(int_aie_put_ms uimm1:$stream, GPR:$reg),
          (inst (BITSET_MD0 md0idx, (MOV_S12 uimm1:$stream)),
					 GPR:$reg,
					 0)>;
def : StreamWriteIntrinsicPat<MV_SCL2MSA_imm0, 10>;
def : StreamWriteIntrinsicPat<MV_SCL2MSA_imm1, 11>;
def : StreamWriteIntrinsicPat<MV_SCL2MSB_imm0, 10>;
def : StreamWriteIntrinsicPat<MV_SCL2MSB_imm1, 11>;

// extract_vector
def : Pat<(extractelt (v2i32 mC:$Rn), 0),
          (i32 (EXTRACT_SUBREG mC:$Rn, sub_32_lo))>;
def : Pat<(extractelt (v2i32 mC:$Rn), 1),
          (i32 (EXTRACT_SUBREG mC:$Rn, sub_32_hi))>;

// build_vector
def : Pat<(build_vector (i32 mCL:$a1), (i32 mCH:$a2)),
          (INSERT_SUBREG
	    (INSERT_SUBREG (v2i32 (IMPLICIT_DEF)), (i32 mCL:$a1), sub_32_lo),
            (i32 mCH:$a2), sub_32_hi)>;

// Unknown instructions:
let hasSideEffects = 1, mayLoad = 1, mayStore = 1 in {

// If regular decoding fails, then we fallback to these instructions,
// which give us a little more information during disassembly than
// just "unknown"
let DecoderNamespace = "Fallback" in {
	 def UNKNOWN16 : AIE_instr16<(outs), (ins), [], "unknown16", "">;
	 def UNKNOWN32 : AIE_instr32<(outs), (ins), [], "unknown32", ""> { let Slot = unknown_slot; }
	 def UNKNOWN_dms_lda : AIE_dms_lda<(outs OP_mLdaScl:$reg), (ins), [], "unknown_dms_lda", "">;
	 def UNKNOWN_mv0_ldb : AIE_mv0_ldb<(outs), (ins), [], "unknown_mv0_ldb", "">;
	 def UNKNOWN_st : AIE_st_base<(outs), (ins), [], "unknown_st", "">;
	 def UNKNOWN_lng : AIE_lng<(outs), (ins), [], "unknown_lng", "">;
	 def UNKNOWN_vec_dpd : AIE_vec_dpd<(outs), (ins), [], "unknown_vec_dpd", "">;
	 def UNKNOWN_alu_format : AIE_alu_format<(outs), (ins), [], "unknown_alu_format", "">;
	 def UNKNOWN_i32_lda : AIE_i32_lda<(outs), (ins), [], "unknown_i32_lda", "">;
	 def UNKNOWN_ldb : AIE_ldb_base<(outs), (ins), [], "unknown_ldb", "">;
	 def UNKNOWN_vec_med : AIE_vec_med<(outs), (ins), [], "unknown_vec_med", "">;
	 def UNKNOWN_vec_all : AIE_veca_base<(outs), (ins), [], "unknown_vec_all", "">;
	 def UNKNOWN64_fallback : AIE_instr64<(outs), (ins), [], "unknown64", "">;
	 def UNKNOWN96_fallback : AIE_instr96<(outs), (ins), [], "unknown96", ""> {
		  let instr96{43} = 0x0;
	 }

	 def UNKNOWN128_fallback : AIE_instr128<(outs), (ins), [], "unknown128", ""> {
	     let instr128{43} = 0x0;
	 // field bits<62> dummy2;
	 // let instr128{11-72} = dummy2;
	 // field bits<38> dummy;
	 // let instr128{32-69} = dummy;
	 // let instr128 = 0;
	 // let instr128{70} = 0x0;
	 }
} // Namespace Fallback
	 def UNKNOWN64 : AIE_instr64<(outs), (ins), [], "unknown64", "">;
	 def UNKNOWN96 : AIE_instr96<(outs), (ins), [], "unknown96", ""> {
		  let instr96{43} = 0x0;
	 }

	 def UNKNOWN128 : AIE_instr128<(outs), (ins), [], "unknown128", ""> {
	     let instr128{43} = 0x0;
	 // field bits<62> dummy2;
	 // let instr128{11-72} = dummy2;
	 // field bits<38> dummy;
	 // let instr128{32-69} = dummy;
	 // let instr128 = 0;
	 // let instr128{70} = 0x0;
	 }
}
