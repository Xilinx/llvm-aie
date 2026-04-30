//===--AIE2PSInstrPatterns.td -ISel patterns for AIE2ps ----*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file describes instruction selection patterns for the AIE2ps
// instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

include "AIEBaseInstrPatterns.td"

// Placeholder for a bare frameindex. This pseudo represents the
// pointer register to be allocated, initialized with the address
// represented by the frameindex in its only operand.
let Uses = [sp] in
def PseudoFI : Pseudo<(outs eP:$dst), (ins i32imm:$imm)>;

// SCHED_BARRIER
def : Pat<(int_aie2ps_sched_barrier), (SCHED_BARRIER)>;

// A bare frameindex. Placeholder until eliminateFrameIndex.
def : Pat<(ptr0 frameindex:$fi), (PseudoFI(frameindex_to_targetframeindex $fi))>;

class VecConf {
  int AMODE_I32  = 0;
  int AMODE_I64  = 1;
  int AMODE_FP32 = 2;

  int BMODE_8x8_I32           = 0;
  int BMODE_16x16_I32         = 1;
  int BMODE_32x16_I64         = 0;
  int BMODE_16x16_I64         = 1;
  int BMODE_16x32_I64         = 2; //FIXME:
  int BMODE_BFP13XBFP13_FP32  = 0;
  int BMODE_BFP16XBFP16_FP32  = 1;
  int BMODE_8x8_FP32          = 2;
  int BMODE_16x16_FP32        = 3;


  int CMODE_I32_8x8_1_8x8_8x8 = 1;
  int CMODE_I32_8x8_1_8x16_16x8_SP = 5;
  int CMODE_I32_8x8_1_4x16_16x8_SP = 5;
  int CMODE_I32_8x8_1_4x8_8x16 = 6;
  int CMODE_I32_8x8_1_4x16_16x16_SP = 7;
  int CMODE_I32_8x8_64_1x2_2x1 = 1;
  int CMODE_I32_8x8_64_1x1_1x1 = 1;
  int CMODE_I32_8x8_8_8x8_8x1 = 2;
  int CMODE_I32_8x8_1_64x8_8x1 = 4;
  int CMODE_I32_16x16_1_8x2_2x8 = 0;
  int CMODE_I32_16x16_64_1x1_1x1 = 1;
  int CMODE_I32_16x16_32_1x1_1x1 = 1;
  int CMODE_I64_16x16_1_4x4_4x8 = 0;
  int CMODE_I64_16x16_1_4x8_8x8_SP = 5;
  int CMODE_I64_16x16_32_1x1_1x1 = 2;
  int CMODE_I64_16x16_32_1x2_2x1 = 2;
  int CMODE_I64_16x16_1_32x4_4x1 = 3;
  int CMODE_I64_16x16_8_4x4_4x1 = 1;
  int CMODE_I64_C16xC16_16_1x2_2x1 = 4;
  int CMODE_I64_C16xC16_16_1x1_1x1 = 4;
  int CMODE_I64_32x16_1_4x2_2x8 = 0;
  int CMODE_I64_C32xC16_16_1x1_1x1 = 1;
  int CMODE_I64_C32xC16_8_1x1_1x1 = 1;
  int CMODE_FP32_FP8xFP8_1_8x8_8x8 = 0;
  int CMODE_FP32_FP8xFP8_1_4x16_16x16_SP = 1;
  int CMODE_FP32_FP16xFP16_64_1x1_1x1 = 1;
  int CMODE_FP32_FP16xFP16_32_1x1_1x1 = 1;
  int CMODE_FP32_FP16xFP16_1_4x8_8x8 = 2;
  int CMODE_FP32_FP16xFP16_1_4x16_16x8_SP = 3;
  int CMODE_FP32_BF16xBF16_64_1x1_1x1 = 1;
  int CMODE_FP32_BF16xBF16_32_1x1_1x1 = 1;
  int CMODE_FP32_BF16xBF16_1_4x8_8x8 = 2;
  int CMODE_FP32_BF16xBF16_1_4x16_16x8_SP = 3;
  int CMODE_FP32_BFP16xBFP16_1_4x8_8x16 = 6;
  int CMODE_FP32_BFP13xBFP13_1_4x16_16x16 = 0;

  bits<1> dynZeroAccum    = 0;  // 0 – Use default first accumulator input to the post-adder. 1 – Replace default first accumulator with zeros.
  bits<2> amode           = 0;  // Accumulator width (see above)
  bits<2> bmode           = 0;  // Multiplication precision (see above)
  bits<3> cmode           = 0;  // Multiplication mode
  bits<1> signY           = 0;  // 0 – Y buffer has an unsigned datatype. 1 – Signed
  bits<1> signX           = 0;  // 0 – X buffer has an unsigned datatype. 1 – Signed
  bits<1> accShift        = 0;  // Accumulator left shift by 16 bits
  bits<1> dynMulNeg       = 0;  // 0 – Do nothing. 1 – Invert instruction behavior regarding negation of the multiplier results.
  bits<1> dynAcc0Neg      = 0;  // 0 – Do nothing. 1 – Invert instruction behavior regarding negation of the first accumulator input.
  bits<1> dynAcc1Neg      = 0;  // 0 – Do nothing. 1 – Invert instruction behavior regarding negation of the second accumulator input.
  bits<2> reserved1       = 0;
  bits<8> dynTermNeg      = 0;  // Negation of terms in complex multiplications to allow complex handling.
  bits<1> floatformatX    = 0;  // For bf16/fp16 modes: 0 - fp16; 1 - bf16. For fp8 modes: 0 - fp8 e4m3; 1 - fp8 e5m2 (BF8)
  bits<1> floatformatY    = 0;  // For bf16/fp16 modes: 0 - fp16; 1 - bf16. For fp8 modes: 0 - fp8 e4m3; 1 - fp8 e5m2 (BF8)
  bits<6> reserved2       = 0;

  bits<32> all = {reserved2, floatformatY, floatformatX, dynTermNeg, reserved1, dynAcc1Neg, dynAcc0Neg, dynMulNeg, accShift, signX, signY, cmode, bmode, amode, dynZeroAccum};
  dag ConfBits = (MOVXM_lng_cg (i32 all));
}
// The multiplication mode (cmode) should be irrelevant for an addition/substraction but it is part of 
// the amode, bmode, cmode combination to perform an element wise multiplication according to ISA, we set it 
// to the value of that combination (1) to silence our tools (e.g. simulator)
def accfp32_vecconf : VecConf { let amode = AMODE_FP32; let bmode = BMODE_16x16_FP32; let cmode = 1;}
def mulbf16_vecconf : VecConf { let amode = AMODE_FP32; let bmode = BMODE_16x16_FP32; let cmode = CMODE_FP32_BF16xBF16_64_1x1_1x1; let floatformatX = 1;  let floatformatY = 1;}

/// Generic pattern classes
class PatGpr<SDPatternOperator OpNode, AIE2PSInst Inst, ValueType type>
    : Pat<(type (OpNode (type eR:$rs))), (Inst eR:$rs)>;

class PatGprGpr<SDPatternOperator OpNode, AIE2PSInst Inst, ValueType type>
    : Pat<(type (OpNode (type eR:$rs1), (type eR:$rs2))), (Inst eR:$rs1, eR:$rs2)>;


// 3.28 PADD – Pointer increments
// All PADD immediate ranges are the same on AIE2PS, always select to
// the multi-slot pseudo
def : Pat< (ptradd eP:$ptr, (i20 c10s_step64:$imm)),
           (PADD_imm_pseudo eP:$ptr, c10s_step64:$imm)>;
def : Pat< (ptradd eP:$ptr, eM:$mod),
           (PADD_mod_pseudo eP:$ptr, eM:$mod)>;

// 4.1.2 CLB/CLZ – Count leading bits/zeros
// CLZ is a standard llvm intrinsic
def : PatGpr<int_aie2ps_clb, CLB, i32>;
def : PatGpr<ctlz, CLZ, i32>;
def : PatGpr<ctlz_zero_undef, CLZ, i32>;
def : PatGpr<ctpop, POPCOUNT, i32>;

// 4.1.4 EXTEND – Sign extend / Zero Extend
// Note: Sign extensions are handled in AIE2PSInstructionSelector.cpp.
def : Pat<(i32 (and eR:$rs1, (i32 255))), (EXTEND_u8 eR:$rs1)>;
def : Pat<(i32 (and eR:$rs1, (i32 65535))), (EXTEND_u16 eR:$rs1)>;

multiclass ALUPatterns<ValueType type> {
// 4.1.6 ABS - Absolute Value
def : PatGpr<abs, ABS, type>;
// 4.1.7 ADD - Add Immediate
def : Pat<(type (add (type eR:$s0), c7s:$imm)), (ADD_add_r_ri eR:$s0, c7s:$imm)>;
def : Pat<(type (sub (type eR:$s0), c7s_negated:$imm)),
          (ADD_add_r_ri eR:$s0, (NegateImm c7s_negated:$imm))>;

// 4.1.9 ADD/SUB/ADC/SBC - Addition/subtraction
def : PatGprGpr<add, ADD_alu_r_rr, type>;
def : PatGprGpr<sub, SUB, type>;
// 4.1.10 AND / OR / XOR - Bitwise Operations
def : PatGprGpr<and, AND, type>;
def : PatGprGpr<or, OR, type>;
def : PatGprGpr<xor, XOR, type>;
// 4.1.13 MUL - Scalar multiplication
def : PatGprGpr<mul, MUL, type>;
// 4.1.14 MAX/MIN - Maximum/Minimum (signed and unsigned)
def : PatGprGpr<smax, MAX, type>;
def : PatGprGpr<smin, MIN, type>;
def : PatGprGpr<umax, MAXU, type>;
def : PatGprGpr<umin, MINU, type>;
}
defm : ALUPatterns<i32>;
// We can safely select bitwise operators for v2i16 and v4i8 types.
foreach Ty = [v2i16, v4i8] in {
def : PatGprGpr<and, AND, Ty>;
def : PatGprGpr<or, OR, Ty>;
def : PatGprGpr<xor, XOR, Ty>;
}

// 4.1.8 SELECT
class SelectPat<ValueType type> :
         Pat<(type (select (i32 mR27_select:$rs1), (type eR:$rs2), (type eR:$rs3))),
          (SEL_NEZ mRx:$rs2, mRy:$rs3, mR27_select:$rs1)>;
def : SelectPat<ptr0>;
def : SelectPat<i32>;


class SelectNezPat<ValueType type> :
         Pat<(type (select (i32 (setne mR27_select:$rs1, (i32 0))), (type eR:$rs2), (type eR:$rs3))),
          (SEL_NEZ mRx:$rs2, mRy:$rs3, mR27_select:$rs1)>;

def : SelectNezPat<ptr0>;
def : SelectNezPat<i32>;

class SelectNotPat<ValueType type> :
         Pat<(type (select (i32 (seteq mR27_select:$rs1, (i32 0))), (type eR:$rs2), (type eR:$rs3))),
          (SEL_EQZ mRx:$rs2, mRy:$rs3, mR27_select:$rs1)>;

def : SelectNotPat<ptr0>;
def : SelectNotPat<i32>;

// Compare operations with corresponding instructions
class CmpPat<PatFrag CondOp, AIE2PSInst Inst, ValueType type>
    : Pat<(i32 (CondOp (type eR:$rs1), (type eR:$rs2))),
	       (Inst eR:$rs1, eR:$rs2)>;
multiclass CmpZeroPat<PatFrag CondOp, AIE2PSInst Inst, ValueType type> {
  def : Pat<(i32 (CondOp (type eR:$rs1), (type 0))),
	       (Inst eR:$rs1)>;
  def : Pat<(i32 (CondOp (type 0), (type eR:$rs1))),
	       (Inst eR:$rs1)>;
}

// Compare operations without corresponding instructions, but which
// are trivially supported by swapping the two input operands
class CmpSwapPat<PatFrag CondOp, AIE2PSInst Inst, ValueType type>
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

// 4.1.5 EQZ / NEZ – Compare to zero
// 4.1.11 EQ / NE / GE / GEU / LT / LTU – Compare registers
defm : CmpPatternsUnsigned<i32>;
defm : CmpPatternsUnsigned<ptr0>;
defm : CmpPatternsSigned<i32>;

// 4.1.12 LSHL / ASHL – Shift register
// TODO: Could we always lower right shifts to left shifts to help simplify constants?
def : Pat<(srl (i32 eR:$rs1), (i32 eR:$rs2)), (LSHL eR:$rs1, (SUB (MOV_RLC_imm11_pseudo (i32 0)), eR:$rs2))>;
def : Pat<(sra (i32 eR:$rs1), (i32 eR:$rs2)), (ASHL eR:$rs1, (SUB (MOV_RLC_imm11_pseudo (i32 0)), eR:$rs2))>;
def : Pat<(shl (i32 eR:$rs1), (i32 eR:$rs2)), (LSHL eR:$rs1, eR:$rs2)>;
def : Pat<(srl (i32 eR:$rs1), (i32 c32s:$amount)),
          (LSHL eR:$rs1, (MOV_RLC_imm11_pseudo (i32 (NegateImm c32s:$amount))))>;
def : Pat<(sra (i32 eR:$rs1), (i32 c32s:$amount)),
          (ASHL eR:$rs1, (MOV_RLC_imm11_pseudo (i32 (NegateImm c32s:$amount))))>;

// ACQ – Acquire semaphore
def : Pat<(int_aie2ps_acquire eR:$mRx, eR:$mRy), (ACQ_mLockId_reg eR:$mRx, eR:$mRy)>;
def : Pat<(int_aie2ps_acquire_cond eR:$mRx, eR:$mRy, mR23_lock:$s2), (ACQ_COND_mLockId_reg eR:$mRx, eR:$mRy, mR23_lock:$s2)>;
def : Pat<(int_aie2ps_acquire c6u:$id, eR:$mRy), (ACQ_mLockId_imm c6u:$id, eR:$mRy)>;
def : Pat<(int_aie2ps_acquire_cond c6u:$id, eR:$mRy, mR23_lock:$s2), (ACQ_COND_mLockId_imm c6u:$id, eR:$mRy, mR23_lock:$s2)>;

// REL – Release semaphore
def : Pat<(int_aie2ps_release eR:$mRx, eR:$mRy), (REL_mLockId_reg eR:$mRx, eR:$mRy)>;
def : Pat<(int_aie2ps_release_cond eR:$mRx, eR:$mRy, mR23_lock:$s2), (REL_COND_mLockId_reg eR:$mRx, eR:$mRy, mR23_lock:$s2)>;
def : Pat<(int_aie2ps_release c6u:$id, eR:$mRy), (REL_mLockId_imm c6u:$id, eR:$mRy)>;
def : Pat<(int_aie2ps_release_cond c6u:$id, eR:$mRy, mR23_lock:$s2), (REL_COND_mLockId_imm c6u:$id, eR:$mRy, mR23_lock:$s2)>;

// 4.4.2 DONE
def : Pat<(int_aie2ps_done), (DONE)>;

// J – Absolute Jump
// Note we cannot use addr20 in the output pattern, as it isn't a valid MVT.
def : Pat<(br bb:$addr), (PseudoJ_jump_imm bb:$addr)>;

// JZ/JNZ
// Base pattern
def : Pat <(brcond (i32 eR:$mRx), bb:$addr), (PseudoJNZ eR:$mRx, bb:$addr)>;
// Absorb conversion to 0/1
def : Pat <(brcond (i32 (setne eR:$mRx, (i32 0))), bb:$addr), (PseudoJNZ eR:$mRx, bb:$addr)>;
// Absorb logical negation by reversing branch condition
def : Pat <(brcond (i32 (seteq eR:$mRx, (i32 0))), bb:$addr), (PseudoJZ eR:$mRx, bb:$addr)>;

// Absorb negation through xor(setcc,1) by inverting conditional branch
multiclass BrcondXorCmpPat<PatFrag CondOp, Instruction CmpInst> {
  def : Pat<(brcond (i32 (xor (i32 (CondOp (i32 eR:$a), (i32 eR:$b))), (i32 1))), bb:$addr),
            (PseudoJZ (CmpInst eR:$a, eR:$b), bb:$addr)>;
}
multiclass BrcondXorCmpSwapPat<PatFrag CondOp, Instruction CmpInst> {
  def : Pat<(brcond (i32 (xor (i32 (CondOp (i32 eR:$a), (i32 eR:$b))), (i32 1))), bb:$addr),
            (PseudoJZ (CmpInst eR:$b, eR:$a), bb:$addr)>;
}
defm : BrcondXorCmpPat<seteq, EQ>;
defm : BrcondXorCmpPat<setne, NE>;
defm : BrcondXorCmpPat<setlt, LT>;
defm : BrcondXorCmpPat<setge, GE>;
defm : BrcondXorCmpPat<setult, LTU>;
defm : BrcondXorCmpPat<setuge, GEU>;
defm : BrcondXorCmpSwapPat<setgt, LT>;
defm : BrcondXorCmpSwapPat<setle, GE>;
defm : BrcondXorCmpSwapPat<setugt, LTU>;
defm : BrcondXorCmpSwapPat<setule, GEU>;

// ACC ADD
def : Pat<(int_aie2ps_ACC2048_add_conf ACC2048:$acc1, ACC2048:$acc2, eR:$conf),
           (VADD_vacc ACC2048:$acc1, ACC2048:$acc2, eR:$conf)>;
def : PatInaccessibleMem<(int_aie2ps_ACC2048_accfloat_add_conf ACC2048:$acc1, ACC2048:$acc2, eR:$conf),
           (VADD_f_vacc_fp ACC2048:$acc1, ACC2048:$acc2, eR:$conf)>;
def : Pat<(int_aie2ps_ACC2048_sub_conf ACC2048:$acc1, ACC2048:$acc2, eR:$conf),
           (VSUB_vacc ACC2048:$acc1, ACC2048:$acc2, eR:$conf)>;
def : PatInaccessibleMem<(int_aie2ps_ACC2048_accfloat_sub_conf ACC2048:$acc1, ACC2048:$acc2, eR:$conf),
           (VSUB_f_vacc_fp ACC2048:$acc1, ACC2048:$acc2, eR:$conf)>;

def : Pat<(int_aie2ps_ACC1024_add_conf ACC1024:$acc1, ACC1024:$acc2, eR:$conf),
           (VADD_vacc_half ACC1024:$acc1, ACC1024:$acc2, eR:$conf)>;
def : PatInaccessibleMem<(int_aie2ps_ACC1024_accfloat_add_conf ACC1024:$acc1, ACC1024:$acc2, eR:$conf),
           (VADD_f_vacc_half_fp ACC1024:$acc1, ACC1024:$acc2, eR:$conf)>;
def : Pat<(int_aie2ps_ACC1024_sub_conf ACC1024:$acc1, ACC1024:$acc2, eR:$conf),
           (VSUB_vacc_half ACC1024:$acc1, ACC1024:$acc2, eR:$conf)>;
def : PatInaccessibleMem<(int_aie2ps_ACC1024_accfloat_sub_conf ACC1024:$acc1, ACC1024:$acc2, eR:$conf),
           (VSUB_f_vacc_half_fp ACC1024:$acc1, ACC1024:$acc2, eR:$conf)>;

def : Pat<(v64f32 (fadd ACC2048:$acc1, ACC2048:$acc2)),
          (VADD_f_vacc_fp ACC2048:$acc1, ACC2048:$acc2, (i32 accfp32_vecconf.ConfBits))>;
def : Pat<(v64f32 (fsub ACC2048:$acc1, ACC2048:$acc2)),
          (VSUB_f_vacc_fp ACC2048:$acc1, ACC2048:$acc2, (i32 accfp32_vecconf.ConfBits))>;

// MUL
def : Pat<(v64bf16 (fmul v64bf16:$vec1, v64bf16:$vec2)),
          (v64bf16 (REG_SEQUENCE VEC1024,
            (VCONV_bf16_fp32_mv_conv_mv_x_srs_bf16
              (EXTRACT_SUBREG
                (VMUL_f_vmul_bf_vmul_bf_core_Y_Y VEC1024:$vec1, VEC1024:$vec2, (i32 mulbf16_vecconf.ConfBits)),
                sub_1024_acc_lo)),
            sub_512_lo,
            (VCONV_bf16_fp32_mv_conv_mv_x_srs_bf16
              (EXTRACT_SUBREG
                (VMUL_f_vmul_bf_vmul_bf_core_Y_Y VEC1024:$vec1, VEC1024:$vec2, (i32 mulbf16_vecconf.ConfBits)),
                sub_1024_acc_hi)),
            sub_512_hi))>;

def : Pat<(v32bf16 (fmul v32bf16:$vec1, v32bf16:$vec2)),
    (VCONV_bf16_fp32_mv_conv_mv_x_srs_bf16
                (VMUL_f_vmul_bf_half_vmul_bf_half_core_X_X VEC512:$vec1, VEC512:$vec2, (i32 mulbf16_vecconf.ConfBits)))>;


// Scalar MAC
def : Pat<(i32 (add eR:$rd, (mul eR:$rs1, eR:$rs2))),
          (MAC eR:$rd, eR:$rs1, eR:$rs2)>;
def : Pat<(i32 (sub eR:$rd, (mul eR:$rs1, eR:$rs2))),
          (MSC eR:$rd, eR:$rs1, eR:$rs2)>; 

// VMUL/VMAC Intrinsics

def : Pat<(int_aie2ps_I1024_I1024_ACC2048_addmac_conf VEC1024:$s1, VEC1024:$s2, ACC2048:$acc1, ACC2048:$acc2, eR:$conf),
           (VADDMAC_vaddmac_vmul_cm_core_Y_Y
            ACC2048:$acc1, ACC2048:$acc2, VEC1024:$s1, VEC1024:$s2, eR:$conf)>;
foreach intr = [int_aie2ps_bf16_I1024_bf16_ACC2048_bf_addmac_conf, int_aie2ps_bf16_I1024_fp16_ACC2048_bf_addmac_conf,
                int_aie2ps_fp16_I1024_fp16_ACC2048_bf_addmac_conf, int_aie2ps_fp16_I1024_bf16_ACC2048_bf_addmac_conf] in {
   def : PatInaccessibleMem<(intr VEC1024:$s1, eR:$s2, ACC2048:$acc1, ACC2048:$acc2,  eR:$conf),
            (VADDMAC_f_vaddmac_bf_vmul_bf_core_Y_R
             ACC2048:$acc1, ACC2048:$acc2, VEC1024:$s1, eR:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_ACC1024_bf_addmac_conf, int_aie2ps_bf16_I512_fp16_ACC1024_bf_addmac_conf,
                int_aie2ps_fp16_I512_fp16_ACC1024_bf_addmac_conf, int_aie2ps_fp16_I512_bf16_ACC1024_bf_addmac_conf] in {
   def : PatInaccessibleMem<(intr VEC512:$s1, eR:$s2, ACC1024:$acc1, ACC1024:$acc2,  eR:$conf),
            (VADDMAC_f_vaddmac_bf_half_vmul_bf_core_X_R
             ACC1024:$acc1, ACC1024:$acc2, VEC512:$s1, eR:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_addmac_conf, int_aie2ps_bf16_I512_fp16_I512_ACC1024_bf_addmac_conf,
                int_aie2ps_fp16_I512_fp16_I512_ACC1024_bf_addmac_conf, int_aie2ps_fp16_I512_bf16_I512_ACC1024_bf_addmac_conf] in {
   def : PatInaccessibleMem<(intr VEC512:$s1, VEC512:$s2, ACC1024:$acc1, ACC1024:$acc2,  eR:$conf),
            (VADDMAC_f_vaddmac_bf_half_vmul_bf_half_core_X_X ACC1024:$acc1, ACC1024:$acc2, 
             VEC512:$s1, VEC512:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_addmac_conf, int_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_addmac_conf,
                int_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_addmac_conf, int_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_addmac_conf] in {
   def : PatInaccessibleMem<(intr VEC512:$s1, VEC1024:$s2, ACC1024:$acc1, ACC1024:$acc2,  eR:$conf),
           (VADDMAC_f_vaddmac_bf_half_vmul_bf_core_X_Y ACC1024:$acc1, ACC1024:$acc2,  VEC512:$s1, VEC1024:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_addmac_conf, int_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_addmac_conf,
                int_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_addmac_conf, int_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_addmac_conf] in {
   def : PatInaccessibleMem<(intr VEC512:$s1, VEC1024:$s2, ACC1024:$acc1, ACC1024:$acc2,  eR:$conf),
           (VADDMAC_f_vaddmac_bf_half_vmul_bf_core_X_Y ACC1024:$acc1, ACC1024:$acc2,  VEC512:$s1, VEC1024:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_addmac_conf, int_aie2ps_bf16_I1024_fp16_I1024_ACC2048_bf_addmac_conf,
                int_aie2ps_fp16_I1024_fp16_I1024_ACC2048_bf_addmac_conf, int_aie2ps_fp16_I1024_bf16_I1024_ACC2048_bf_addmac_conf] in {
  def : PatInaccessibleMem<(intr VEC1024:$s1, VEC1024:$s2, ACC2048:$acc1, ACC2048:$acc2, eR:$conf),
           (VADDMAC_f_vaddmac_bf_vmul_bf_core_Y_Y ACC2048:$acc1, ACC2048:$acc2, VEC1024:$s1, VEC1024:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I1024_bf16_ACC2048_bf_addmsc_conf, int_aie2ps_bf16_I1024_fp16_ACC2048_bf_addmsc_conf,
                int_aie2ps_fp16_I1024_fp16_ACC2048_bf_addmsc_conf, int_aie2ps_fp16_I1024_bf16_ACC2048_bf_addmsc_conf] in {
   def : PatInaccessibleMem<(intr VEC1024:$s1, eR:$s2, ACC2048:$acc1, ACC2048:$acc2,  eR:$conf),
            (VADDMSC_f_vaddmac_bf_vmul_bf_core_Y_R
             ACC2048:$acc1, ACC2048:$acc2, VEC1024:$s1, eR:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_ACC1024_bf_addmsc_conf, int_aie2ps_bf16_I512_fp16_ACC1024_bf_addmsc_conf,
                int_aie2ps_fp16_I512_fp16_ACC1024_bf_addmsc_conf, int_aie2ps_fp16_I512_bf16_ACC1024_bf_addmsc_conf] in {
   def : PatInaccessibleMem<(intr VEC512:$s1, eR:$s2, ACC1024:$acc1, ACC1024:$acc2,  eR:$conf),
            (VADDMSC_f_vaddmac_bf_half_vmul_bf_core_X_R
             ACC1024:$acc1, ACC1024:$acc2, VEC512:$s1, eR:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_addmsc_conf, int_aie2ps_bf16_I512_fp16_I512_ACC1024_bf_addmsc_conf,
                int_aie2ps_fp16_I512_fp16_I512_ACC1024_bf_addmsc_conf, int_aie2ps_fp16_I512_bf16_I512_ACC1024_bf_addmsc_conf] in {
   def : PatInaccessibleMem<(intr VEC512:$s1, VEC512:$s2, ACC1024:$acc1, ACC1024:$acc2,  eR:$conf),
            (VADDMSC_f_vaddmac_bf_half_vmul_bf_half_core_X_X
             ACC1024:$acc1, ACC1024:$acc2, VEC512:$s1, VEC512:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_addmsc_conf, int_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_addmsc_conf,
                int_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_addmsc_conf, int_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_addmsc_conf] in {
   def : PatInaccessibleMem<(intr VEC512:$s1, VEC1024:$s2, ACC1024:$acc1, ACC1024:$acc2,  eR:$conf),
           (VADDMSC_f_vaddmac_bf_half_vmul_bf_core_X_Y ACC1024:$acc1, ACC1024:$acc2,  VEC512:$s1, VEC1024:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_addmsc_conf, int_aie2ps_bf16_I1024_fp16_I1024_ACC2048_bf_addmsc_conf,
                int_aie2ps_fp16_I1024_fp16_I1024_ACC2048_bf_addmsc_conf, int_aie2ps_fp16_I1024_bf16_I1024_ACC2048_bf_addmsc_conf] in {
  def : PatInaccessibleMem<(intr VEC1024:$s1, VEC1024:$s2, ACC2048:$acc1, ACC2048:$acc2, eR:$conf),
           (VADDMSC_f_vaddmac_bf_vmul_bf_core_Y_Y ACC2048:$acc1, ACC2048:$acc2, VEC1024:$s1, VEC1024:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I1024_bf16_ACC2048_bf_mac_conf, int_aie2ps_bf16_I1024_fp16_ACC2048_bf_mac_conf,
                int_aie2ps_fp16_I1024_fp16_ACC2048_bf_mac_conf, int_aie2ps_fp16_I1024_bf16_ACC2048_bf_mac_conf] in {
  def : PatInaccessibleMem<(intr VEC1024:$s1, eR:$s2, ACC2048:$acc1, eR:$conf),
           (VMAC_f_vmac_bf_vmul_bf_core_Y_R ACC2048:$acc1, VEC1024:$s1, eR:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_ACC1024_bf_mac_conf, int_aie2ps_bf16_I512_fp16_ACC1024_bf_mac_conf,
                int_aie2ps_fp16_I512_fp16_ACC1024_bf_mac_conf, int_aie2ps_fp16_I512_bf16_ACC1024_bf_mac_conf] in {
  def : PatInaccessibleMem<(intr VEC512:$s1, eR:$s2, ACC1024:$acc1, eR:$conf),
            (VMAC_f_vmac_bf_half_vmul_bf_core_X_R ACC1024:$acc1, VEC512:$s1, eR:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_mac_conf, int_aie2ps_bf16_I512_fp16_I512_ACC1024_bf_mac_conf,
                int_aie2ps_fp16_I512_fp16_I512_ACC1024_bf_mac_conf, int_aie2ps_fp16_I512_bf16_I512_ACC1024_bf_mac_conf] in {
  def : PatInaccessibleMem<(intr VEC512:$s1, VEC512:$s2, ACC1024:$acc1, eR:$conf),
            (VMAC_f_vmac_bf_half_vmul_bf_half_core_X_X ACC1024:$acc1, VEC512:$s1, VEC512:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_mac_conf, int_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_mac_conf,
                int_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_mac_conf, int_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_mac_conf] in {
  def : PatInaccessibleMem<(intr VEC512:$s1, VEC1024:$s2, ACC1024:$acc1, eR:$conf),
           (VMAC_f_vmac_bf_half_vmul_bf_core_X_Y ACC1024:$acc1, VEC512:$s1, VEC1024:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_mac_conf, int_aie2ps_bf16_I1024_fp16_I1024_ACC2048_bf_mac_conf,
                int_aie2ps_fp16_I1024_fp16_I1024_ACC2048_bf_mac_conf, int_aie2ps_fp16_I1024_bf16_I1024_ACC2048_bf_mac_conf] in {
  def : PatInaccessibleMem<(intr VEC1024:$s1, VEC1024:$s2, ACC2048:$acc1, eR:$conf),
           (VMAC_f_vmac_bf_vmul_bf_core_Y_Y ACC2048:$acc1, VEC1024:$s1, VEC1024:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I1024_bf16_ACC2048_bf_msc_conf, int_aie2ps_bf16_I1024_fp16_ACC2048_bf_msc_conf,
                int_aie2ps_fp16_I1024_fp16_ACC2048_bf_msc_conf, int_aie2ps_fp16_I1024_bf16_ACC2048_bf_msc_conf] in {
  def : PatInaccessibleMem<(intr VEC1024:$s1, eR:$s2, ACC2048:$acc1, eR:$conf),
           (VMSC_f_vmac_bf_vmul_bf_core_Y_R ACC2048:$acc1, VEC1024:$s1, eR:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_ACC1024_bf_msc_conf, int_aie2ps_bf16_I512_fp16_ACC1024_bf_msc_conf,
                int_aie2ps_fp16_I512_fp16_ACC1024_bf_msc_conf, int_aie2ps_fp16_I512_bf16_ACC1024_bf_msc_conf] in {
  def : PatInaccessibleMem<(intr VEC512:$s1, eR:$s2, ACC1024:$acc1, eR:$conf),
           (VMSC_f_vmac_bf_half_vmul_bf_core_X_R ACC1024:$acc1, VEC512:$s1, eR:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_msc_conf, int_aie2ps_bf16_I512_fp16_I512_ACC1024_bf_msc_conf,
                int_aie2ps_fp16_I512_fp16_I512_ACC1024_bf_msc_conf, int_aie2ps_fp16_I512_bf16_I512_ACC1024_bf_msc_conf] in {
  def : PatInaccessibleMem<(intr VEC512:$s1, VEC512:$s2, ACC1024:$acc1, eR:$conf),
           (VMSC_f_vmac_bf_half_vmul_bf_half_core_X_X ACC1024:$acc1, VEC512:$s1, VEC512:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_msc_conf, int_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_msc_conf,
                int_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_msc_conf, int_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_msc_conf] in {
  def : PatInaccessibleMem<(intr VEC512:$s1, VEC1024:$s2, ACC1024:$acc1, eR:$conf),
           (VMSC_f_vmac_bf_half_vmul_bf_core_X_Y ACC1024:$acc1, VEC512:$s1, VEC1024:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_msc_conf, int_aie2ps_bf16_I1024_fp16_I1024_ACC2048_bf_msc_conf,
                int_aie2ps_fp16_I1024_fp16_I1024_ACC2048_bf_msc_conf, int_aie2ps_fp16_I1024_bf16_I1024_ACC2048_bf_msc_conf] in {
  def : PatInaccessibleMem<(intr VEC1024:$s1, VEC1024:$s2, ACC2048:$acc1, eR:$conf),
           (VMSC_f_vmac_bf_vmul_bf_core_Y_Y ACC2048:$acc1, VEC1024:$s1, VEC1024:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I1024_bf16_ACC2048_bf_mul_conf, int_aie2ps_bf16_I1024_fp16_ACC2048_bf_mul_conf,
                int_aie2ps_fp16_I1024_fp16_ACC2048_bf_mul_conf, int_aie2ps_fp16_I1024_bf16_ACC2048_bf_mul_conf] in {
  def : PatInaccessibleMem<(intr VEC1024:$s1, eR:$s2, eR:$conf),
            (VMUL_f_vmul_bf_vmul_bf_core_Y_R VEC1024:$s1, eR:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_ACC1024_bf_mul_conf, int_aie2ps_bf16_I512_fp16_ACC1024_bf_mul_conf,
                int_aie2ps_fp16_I512_fp16_ACC1024_bf_mul_conf, int_aie2ps_fp16_I512_bf16_ACC1024_bf_mul_conf] in {
  def : PatInaccessibleMem<(intr VEC512:$s1, eR:$s2, eR:$conf),
            (VMUL_f_vmul_bf_half_vmul_bf_core_X_R VEC512:$s1, eR:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_mul_conf, int_aie2ps_bf16_I512_fp16_I512_ACC1024_bf_mul_conf,
                int_aie2ps_fp16_I512_fp16_I512_ACC1024_bf_mul_conf, int_aie2ps_fp16_I512_bf16_I512_ACC1024_bf_mul_conf] in {
  def : PatInaccessibleMem<(intr VEC512:$s1, VEC512:$s2, eR:$conf),
                    (VMUL_f_vmul_bf_half_vmul_bf_half_core_X_X VEC512:$s1, VEC512:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_mul_conf, int_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_mul_conf,
                int_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_mul_conf, int_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_mul_conf] in {
  def : PatInaccessibleMem<(intr VEC512:$s1, VEC1024:$s2, eR:$conf),
           (VMUL_f_vmul_bf_half_vmul_bf_core_X_Y VEC512:$s1, VEC1024:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_mul_conf, int_aie2ps_bf16_I1024_fp16_I1024_ACC2048_bf_mul_conf,
                int_aie2ps_fp16_I1024_fp16_I1024_ACC2048_bf_mul_conf, int_aie2ps_fp16_I1024_bf16_I1024_ACC2048_bf_mul_conf] in {
  def : PatInaccessibleMem<(intr VEC1024:$s1, VEC1024:$s2, eR:$conf),
           (VMUL_f_vmul_bf_vmul_bf_core_Y_Y VEC1024:$s1, VEC1024:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I1024_bf16_ACC2048_bf_negmul_conf, int_aie2ps_bf16_I1024_fp16_ACC2048_bf_negmul_conf,
                int_aie2ps_fp16_I1024_fp16_ACC2048_bf_negmul_conf, int_aie2ps_fp16_I1024_bf16_ACC2048_bf_negmul_conf] in {
  def : PatInaccessibleMem<(intr VEC1024:$s1, eR:$s2, eR:$conf),
            (VNEGMUL_f_vmul_bf_vmul_bf_core_Y_R VEC1024:$s1, eR:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_ACC1024_bf_negmul_conf, int_aie2ps_bf16_I512_fp16_ACC1024_bf_negmul_conf,
                int_aie2ps_fp16_I512_fp16_ACC1024_bf_negmul_conf, int_aie2ps_fp16_I512_bf16_ACC1024_bf_negmul_conf] in {
  def : PatInaccessibleMem<(intr VEC512:$s1, eR:$s2, eR:$conf),
            (VNEGMUL_f_vmul_bf_half_vmul_bf_core_X_R VEC512:$s1, eR:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_negmul_conf, int_aie2ps_bf16_I512_fp16_I512_ACC1024_bf_negmul_conf,
                int_aie2ps_fp16_I512_fp16_I512_ACC1024_bf_negmul_conf, int_aie2ps_fp16_I512_bf16_I512_ACC1024_bf_negmul_conf] in {
  def : PatInaccessibleMem<(intr VEC512:$s1, VEC512:$s2, eR:$conf),
           (VNEGMUL_f_vmul_bf_half_vmul_bf_half_core_X_X VEC512:$s1, VEC512:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_negmul_conf, int_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_negmul_conf,
                int_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_negmul_conf, int_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_negmul_conf] in {
  def : PatInaccessibleMem<(intr VEC512:$s1, VEC1024:$s2, eR:$conf),
           (VNEGMUL_f_vmul_bf_half_vmul_bf_core_X_Y VEC512:$s1, VEC1024:$s2, eR:$conf)>;
}
foreach intr = [int_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_negmul_conf, int_aie2ps_bf16_I1024_fp16_I1024_ACC2048_bf_negmul_conf,
                int_aie2ps_fp16_I1024_fp16_I1024_ACC2048_bf_negmul_conf, int_aie2ps_fp16_I1024_bf16_I1024_ACC2048_bf_negmul_conf] in {
  def : PatInaccessibleMem<(intr VEC1024:$s1, VEC1024:$s2, eR:$conf),
           (VNEGMUL_f_vmul_bf_vmul_bf_core_Y_Y VEC1024:$s1, VEC1024:$s2, eR:$conf)>;
}
def : Pat<(int_aie2ps_I1024_I1024_ACC2048_mac_conf VEC1024:$s1, VEC1024:$s2, ACC2048:$acc1, eR:$conf),
           (VMAC_vmac_vmul_cm_core_Y_Y ACC2048:$acc1, VEC1024:$s1, VEC1024:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I1024_I1024_ACC2048_mul_conf VEC1024:$s1, VEC1024:$s2, eR:$conf),
           (VMUL_vmul_vmul_cm_core_Y_Y VEC1024:$s1, VEC1024:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I1024_I512_ACC2048_addmac_conf
           VEC1024:$s1, VEC512:$s2, ACC2048:$acc1, ACC2048:$acc2, eR:$conf),
           (VADDMAC_vaddmac_vmul_cm_core_Y_X
            ACC2048:$acc1, ACC2048:$acc2, VEC1024:$s1, VEC512:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I1024_I512_ACC2048_mac_conf VEC1024:$s1, VEC512:$s2, ACC2048:$acc1, eR:$conf),
           (VMAC_vmac_vmul_cm_core_Y_X ACC2048:$acc1, VEC1024:$s1, VEC512:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I1024_I512_ACC2048_mul_conf VEC1024:$s1, VEC512:$s2, eR:$conf),
           (VMUL_vmul_vmul_cm_core_Y_X VEC1024:$s1, VEC512:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I512_I1024_ACC2048_addmac_conf VEC512:$s1, VEC1024:$s2, ACC2048:$acc1, ACC2048:$acc2, eR:$conf),
           (VADDMAC_vaddmac_vmul_cm_core_X_Y ACC2048:$acc1, ACC2048:$acc2, VEC512:$s1, VEC1024:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I512_I1024_ACC2048_mac_conf VEC512:$s1, VEC1024:$s2, ACC2048:$acc1, eR:$conf),
           (VMAC_vmac_vmul_cm_core_X_Y ACC2048:$acc1, VEC512:$s1, VEC1024:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I512_I1024_ACC2048_mul_conf VEC512:$s1, VEC1024:$s2, eR:$conf),
           (VMUL_vmul_vmul_cm_core_X_Y VEC512:$s1, VEC1024:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I512_char_ACC2048_addmac_conf VEC512:$s1, eR:$s2, ACC2048:$acc1, ACC2048:$acc2, eR:$conf),
           (VADDMAC_vaddmac_vmul_cm_core_X_R ACC2048:$acc1, ACC2048:$acc2, VEC512:$s1, eR:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I512_char_ACC2048_mac_conf VEC512:$s1, eR:$s2, ACC2048:$acc1, eR:$conf),
           (VMAC_vmac_vmul_cm_core_X_R ACC2048:$acc1, VEC512:$s1, eR:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I512_char_ACC2048_mul_conf VEC512:$s1, eR:$s2, eR:$conf),
           (VMUL_vmul_vmul_cm_core_X_R VEC512:$s1, eR:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I512_short_ACC2048_addmac_conf VEC512:$s1, eR:$s2, ACC2048:$acc1, ACC2048:$acc2, eR:$conf),
           (VADDMAC_vaddmac_vmul_cm_core_X_R ACC2048:$acc1, ACC2048:$acc2, VEC512:$s1, eR:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I512_short_ACC2048_mac_conf VEC512:$s1, eR:$s2, ACC2048:$acc1, eR:$conf),
           (VMAC_vmac_vmul_cm_core_X_R ACC2048:$acc1, VEC512:$s1, eR:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I512_short_ACC2048_mul_conf VEC512:$s1, eR:$s2, eR:$conf),
           (VMUL_vmul_vmul_cm_core_X_R VEC512:$s1, eR:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I1024_short_ACC2048_addmac_conf VEC1024:$s1, eR:$s2, ACC2048:$acc1, ACC2048:$acc2, eR:$conf),
           (VADDMAC_vaddmac_vmul_cm_core_Y_R ACC2048:$acc1, ACC2048:$acc2, VEC1024:$s1, eR:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I1024_short_ACC2048_mac_conf VEC1024:$s1, eR:$s2, ACC2048:$acc1, eR:$conf),
           (VMAC_vmac_vmul_cm_core_Y_R ACC2048:$acc1, VEC1024:$s1, eR:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I1024_short_ACC2048_mul_conf VEC1024:$s1, eR:$s2, eR:$conf),
           (VMUL_vmul_vmul_cm_core_Y_R VEC1024:$s1, eR:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I512_I512_ACC1024_addmac_conf VEC512:$s1, VEC512:$s2, ACC1024:$acc1, ACC1024:$acc2, eR:$conf),
            (VADDMAC_vaddmac_half_vmul_cm_half_core_X_X ACC1024:$acc1, ACC1024:$acc2, 
             VEC512:$s1, VEC512:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I512_I512_ACC1024_mac_conf VEC512:$s1, VEC512:$s2, ACC1024:$acc1, eR:$conf),
           (VMAC_vmac_half_vmul_cm_half_core_X_X ACC1024:$acc1, 
            VEC512:$s1, VEC512:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I512_I512_ACC1024_mul_conf VEC512:$s1, VEC512:$s2, eR:$conf),
           (VMUL_vmul_half_vmul_cm_half_core_X_X VEC512:$s1, VEC512:$s2, eR:$conf)>;


def : Pat<(int_aie2ps_I512_I512_ACC2048_addmac_conf VEC512:$s1, VEC512:$s2, ACC2048:$acc1, ACC2048:$acc2, eR:$conf),
           (VADDMAC_vaddmac_vmul_cm_core_X_X ACC2048:$acc1, ACC2048:$acc2,
            VEC512:$s1, VEC512:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I512_I512_ACC2048_mac_conf VEC512:$s1, VEC512:$s2, ACC2048:$acc1, eR:$conf),
           (VMAC_vmac_vmul_cm_core_X_X ACC2048:$acc1, VEC512:$s1, VEC512:$s2, eR:$conf)>;

def : Pat<(int_aie2ps_I512_I512_ACC2048_mul_conf VEC512:$s1, VEC512:$s2, eR:$conf),
           (VMUL_vmul_vmul_cm_core_X_X VEC512:$s1, VEC512:$s2, eR:$conf)>;

// Instruction Combining: Fuse MUL + ADD -> VADDMAC/VADDMSC
// When a MAC/MSC result feeds into an ADD(ZeroAddConf), the multiply and
// both accumulations are performed by a single VADDMAC/VADDMSC instruction.
//
// Integer patterns use VAddMacMscBitconvertPats because AIE2PS integer
// MAC intrinsics return v64i32 while ADD intrinsics use v32i64 for ACC2048.

// Integer patterns
defm : VAddMacMscBitconvertPats<int_aie2ps_ACC2048_add_conf,
  int_aie2ps_I512_I512_ACC2048_mac_conf, int_aie2ps_I512_I512_ACC2048_msc_conf,
  VEC512, VEC512, ACC2048, eR,
  VADDMAC_vaddmac_vmul_cm_core_X_X,
  VADDMSC_vaddmac_vmul_cm_core_X_X, v32i64>;
defm : VAddMacMscBitconvertPats<int_aie2ps_ACC2048_add_conf,
  int_aie2ps_I1024_I1024_ACC2048_mac_conf, int_aie2ps_I1024_I1024_ACC2048_msc_conf,
  VEC1024, VEC1024, ACC2048, eR,
  VADDMAC_vaddmac_vmul_cm_core_Y_Y,
  VADDMSC_vaddmac_vmul_cm_core_Y_Y, v32i64>;
defm : VAddMacMscBitconvertPats<int_aie2ps_ACC2048_add_conf,
  int_aie2ps_I512_I1024_ACC2048_mac_conf, int_aie2ps_I512_I1024_ACC2048_msc_conf,
  VEC512, VEC1024, ACC2048, eR,
  VADDMAC_vaddmac_vmul_cm_core_X_Y,
  VADDMSC_vaddmac_vmul_cm_core_X_Y, v32i64>;
defm : VAddMacMscBitconvertPats<int_aie2ps_ACC2048_add_conf,
  int_aie2ps_I1024_I512_ACC2048_mac_conf, int_aie2ps_I1024_I512_ACC2048_msc_conf,
  VEC1024, VEC512, ACC2048, eR,
  VADDMAC_vaddmac_vmul_cm_core_Y_X,
  VADDMSC_vaddmac_vmul_cm_core_Y_X, v32i64>;

// BF16 patterns (HasInaccessibleMem=1 for FP state access)
defm : VAddMacMscPats<int_aie2ps_ACC2048_accfloat_add_conf,
  int_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_mac_conf,
  int_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_msc_conf,
  VEC1024, VEC1024, ACC2048, eR,
  VADDMAC_f_vaddmac_bf_vmul_bf_core_Y_Y,
  VADDMSC_f_vaddmac_bf_vmul_bf_core_Y_Y, /*HasInaccessibleMem=*/1>;
defm : VAddMacMscPats<int_aie2ps_ACC1024_accfloat_add_conf,
  int_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_mac_conf,
  int_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_msc_conf,
  VEC512, VEC512, ACC1024, eR,
  VADDMAC_f_vaddmac_bf_half_vmul_bf_half_core_X_X,
  VADDMSC_f_vaddmac_bf_half_vmul_bf_half_core_X_X, /*HasInaccessibleMem=*/1>;

// FP8 VMUL/VMAC Intrinsics
def : PatInaccessibleMem<(int_aie2ps_fp8_I512_fp8_I512_ACC2048_bf_addmac_conf VEC512:$s1, VEC512:$s2, ACC2048:$acc1, ACC2048:$acc2, eR:$conf),
           (VADDMAC_f_vaddmac_bf_vmul_bf_core_X_X ACC2048:$acc1, ACC2048:$acc2, VEC512:$s1, VEC512:$s2, eR:$conf)>;
def : PatInaccessibleMem<(int_aie2ps_fp8_I512_fp8_I512_ACC2048_bf_addmsc_conf VEC512:$s1, VEC512:$s2, ACC2048:$acc1, ACC2048:$acc2, eR:$conf),
           (VADDMSC_f_vaddmac_bf_vmul_bf_core_X_X ACC2048:$acc1, ACC2048:$acc2, VEC512:$s1, VEC512:$s2, eR:$conf)>;
def : PatInaccessibleMem<(int_aie2ps_fp8_I512_fp8_I512_ACC2048_bf_mac_conf VEC512:$s1, VEC512:$s2, ACC2048:$acc, eR:$conf),
           (VMAC_f_vmac_bf_vmul_bf_core_X_X ACC2048:$acc, VEC512:$s1, VEC512:$s2, eR:$conf)>;
def : PatInaccessibleMem<(int_aie2ps_fp8_I512_fp8_I512_ACC2048_bf_msc_conf VEC512:$s1, VEC512:$s2, ACC2048:$acc, eR:$conf),
           (VMSC_f_vmac_bf_vmul_bf_core_X_X ACC2048:$acc, VEC512:$s1, VEC512:$s2, eR:$conf)>;
def : PatInaccessibleMem<(int_aie2ps_fp8_I512_fp8_I512_ACC2048_bf_mul_conf VEC512:$s1, VEC512:$s2, eR:$conf),
           (VMUL_f_vmul_bf_vmul_bf_core_X_X VEC512:$s1, VEC512:$s2, eR:$conf)>;
def : PatInaccessibleMem<(int_aie2ps_fp8_I512_fp8_I512_ACC2048_bf_negmul_conf VEC512:$s1, VEC512:$s2, eR:$conf),
           (VNEGMUL_f_vmul_bf_vmul_bf_core_X_X VEC512:$s1, VEC512:$s2, eR:$conf)>;

// NEG/SUB Intrinsics

def : PatInaccessibleMem<(int_aie2ps_ACC2048_accfloat_neg_conf ACC2048:$acc1, eR:$conf),
           (VNEG_f_vneg_fp ACC2048:$acc1, eR:$conf)>;
def : Pat<(int_aie2ps_ACC2048_neg_conf ACC2048:$acc1, eR:$conf),
           (VNEG_vneg ACC2048:$acc1, eR:$conf)>;
def : PatInaccessibleMem<(int_aie2ps_ACC1024_accfloat_neg_conf ACC1024:$acc1, eR:$conf),
           (VNEG_f_vneg_half_fp ACC1024:$acc1, eR:$conf)>;
def : Pat<(int_aie2ps_ACC1024_neg_conf ACC1024:$acc1, eR:$conf),
           (VNEG_vneg_half ACC1024:$acc1, eR:$conf)>;

// MSC/NEGMUL Intrinsics
def : Pat<(int_aie2ps_I1024_I1024_ACC2048_addmsc_conf
           VEC1024:$s1,
           VEC1024:$s2,
           ACC2048:$acc1,
           ACC2048:$acc2,
           eR:$conf),
          (VADDMSC_vaddmac_vmul_cm_core_Y_Y
           ACC2048:$acc1,
           ACC2048:$acc2,
           VEC1024:$s1,
           VEC1024:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I1024_I1024_ACC2048_msc_conf
           VEC1024:$s1,
           VEC1024:$s2,
           ACC2048:$acc1,
           eR:$conf),
          (VMSC_vmac_vmul_cm_core_Y_Y
           ACC2048:$acc1,
           VEC1024:$s1,
           VEC1024:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I1024_I1024_ACC2048_negmul_conf
           VEC1024:$s1,
           VEC1024:$s2,
           eR:$conf),
          (VNEGMUL_vmul_vmul_cm_core_Y_Y
           VEC1024:$s1,
           VEC1024:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I1024_I512_ACC2048_addmsc_conf
           VEC1024:$s1,
           VEC512:$s2,
           ACC2048:$acc1,
           ACC2048:$acc2,
           eR: $conf),
          (VADDMSC_vaddmac_vmul_cm_core_Y_X
           ACC2048:$acc1,
           ACC2048:$acc2,
           VEC1024:$s1,
           VEC512:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I1024_I512_ACC2048_msc_conf
           VEC1024:$s1,
           VEC512:$s2,
           ACC2048:$acc1,
           eR: $conf),
          (VMSC_vmac_vmul_cm_core_Y_X
           ACC2048:$acc1,
           VEC1024:$s1,
           VEC512:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I1024_I512_ACC2048_negmul_conf
           VEC1024:$s1,
           VEC512:$s2,
           eR: $conf),
          (VNEGMUL_vmul_vmul_cm_core_Y_X
           VEC1024:$s1,
           VEC512:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I512_I1024_ACC2048_addmsc_conf
           VEC512:$s1,
           VEC1024:$s2,
           ACC2048:$acc1,
           ACC2048:$acc2,
           eR: $conf),
          (VADDMSC_vaddmac_vmul_cm_core_X_Y
           ACC2048:$acc1,
           ACC2048:$acc2,
           VEC512:$s1,
           VEC1024:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I512_I1024_ACC2048_msc_conf
           VEC512:$s1,
           VEC1024:$s2,
           ACC2048:$acc1,
           eR: $conf),
          (VMSC_vmac_vmul_cm_core_X_Y
           ACC2048:$acc1,
           VEC512:$s1,
           VEC1024:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I512_I1024_ACC2048_negmul_conf
           VEC512:$s1,
           VEC1024:$s2,
           eR:$conf),
          (VNEGMUL_vmul_vmul_cm_core_X_Y
           VEC512:$s1,
           VEC1024:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I512_char_ACC2048_addmsc_conf
           VEC512:$s1,
           eR:$s2,
           ACC2048:$acc1,
           ACC2048:$acc2,
           eR: $conf),
          (VADDMSC_vaddmac_vmul_cm_core_X_R
           ACC2048:$acc1,
           ACC2048:$acc2,
           VEC512:$s1,
           eR:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I512_char_ACC2048_msc_conf
           VEC512:$s1,
           eR:$s2,
           ACC2048:$acc1,
           eR: $conf),
          (VMSC_vmac_vmul_cm_core_X_R
           ACC2048:$acc1,
           VEC512:$s1,
           eR:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I512_char_ACC2048_negmul_conf
           VEC512:$s1,
           eR:$s2,
           eR:$conf),
          (VNEGMUL_vmul_vmul_cm_core_X_R
           VEC512:$s1,
           eR:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I512_short_ACC2048_addmsc_conf
           VEC512:$s1,
           eR:$s2,
           ACC2048:$acc1,
           ACC2048:$acc2,
           eR: $conf),
          (VADDMSC_vaddmac_vmul_cm_core_X_R
           ACC2048:$acc1,
           ACC2048:$acc2,
           VEC512:$s1,
           eR:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I512_short_ACC2048_msc_conf
           VEC512:$s1,
           eR:$s2,
           ACC2048:$acc1,
           eR: $conf),
          (VMSC_vmac_vmul_cm_core_X_R
           ACC2048:$acc1,
           VEC512:$s1,
           eR:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I512_short_ACC2048_negmul_conf
           VEC512:$s1,
           eR:$s2,
           eR:$conf),
          (VNEGMUL_vmul_vmul_cm_core_X_R
           VEC512:$s1,
           eR:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I1024_short_ACC2048_addmsc_conf
           VEC1024:$s1,
           eR:$s2,
           ACC2048:$acc1,
           ACC2048:$acc2,
           eR: $conf),
          (VADDMSC_vaddmac_vmul_cm_core_Y_R
           ACC2048:$acc1,
           ACC2048:$acc2,
           VEC1024:$s1,
           eR:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I1024_short_ACC2048_msc_conf
           VEC1024:$s1,
           eR:$s2,
           ACC2048:$acc1,
           eR: $conf),
          (VMSC_vmac_vmul_cm_core_Y_R
           ACC2048:$acc1,
           VEC1024:$s1,
           eR:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I1024_short_ACC2048_negmul_conf
           VEC1024:$s1,
           eR:$s2,
           eR:$conf),
          (VNEGMUL_vmul_vmul_cm_core_Y_R
           VEC1024:$s1,
           eR:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I512_I512_ACC1024_addmsc_conf
           VEC512:$s1,
           VEC512:$s2,
           ACC1024:$acc1,
           ACC1024:$acc2,
           eR:$conf),
          (VADDMSC_vaddmac_half_vmul_cm_half_core_X_X
           ACC1024:$acc1,
           ACC1024:$acc2, 
           VEC512:$s1,
           VEC512:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I512_I512_ACC1024_msc_conf
           VEC512:$s1,
           VEC512:$s2,
           ACC1024:$acc1,
           eR:$conf),
          (VMSC_vmac_half_vmul_cm_half_core_X_X
           ACC1024:$acc1, 
           VEC512:$s1,
           VEC512:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I512_I512_ACC1024_negmul_conf
           VEC512:$s1,
           VEC512:$s2,
           eR:$conf),
          (VNEGMUL_vmul_half_vmul_cm_half_core_X_X
           VEC512:$s1,
           VEC512:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I512_I512_ACC2048_addmsc_conf
           VEC512:$s1,
           VEC512:$s2,
           ACC2048:$acc1,
           ACC2048:$acc2,
           eR:$conf),
           (VADDMSC_vaddmac_vmul_cm_core_X_X
            ACC2048:$acc1,
            ACC2048:$acc2,
            VEC512:$s1,
            VEC512:$s2,
            eR:$conf)>;
def : Pat<(int_aie2ps_I512_I512_ACC2048_msc_conf
           VEC512:$s1,
           VEC512:$s2,
           ACC2048:$acc1,
           eR:$conf),
          (VMSC_vmac_vmul_cm_core_X_X
           ACC2048:$acc1,
           VEC512:$s1,
           VEC512:$s2,
           eR:$conf)>;
def : Pat<(int_aie2ps_I512_I512_ACC2048_negmul_conf
           VEC512:$s1,
           VEC512:$s2,
           eR:$conf),
          (VNEGMUL_vmul_vmul_cm_core_X_X
           VEC512:$s1,
           VEC512:$s2,
           eR:$conf)>;
//CLR
def : Pat<(int_aie2ps_clr_ACC2048_conf), (VCLR_vclr)>;
def : Pat<(int_aie2ps_clr_ACC1024l_conf), (VCLR_vclr_half)>;

// SRS Intrinsic
// Note : Non-constant sign is handled in .cpp
foreach intr = [int_aie2ps_I256_v16_acc32_srs, int_aie2ps_I256_v8_acc64_srs] in {
  def : PatInaccessibleMem<(intr ACC512:$src,  eS:$shft, 0x0),
             (VSRS_2x_mv_w_srs_bm_srsSign0 ACC512:$src,  eS:$shft)>;
  def : PatInaccessibleMem<(intr ACC512:$src,  eS:$shft, 0x1),
             (VSRS_2x_mv_w_srs_bm_srsSign1 ACC512:$src,  eS:$shft)>;
}
foreach intr = [int_aie2ps_I256_v32_acc32_srs, int_aie2ps_I256_v16_acc64_srs] in {
  def : PatInaccessibleMem<(intr ACC1024:$src,  eS:$shft, 0x0),
             (VSRS_4x_mv_w_srs_cm_srsSign0 ACC1024:$src,  eS:$shft)>;
  def : PatInaccessibleMem<(intr ACC1024:$src,  eS:$shft, 0x1),
             (VSRS_4x_mv_w_srs_cm_srsSign1 ACC1024:$src,  eS:$shft)>;
}
foreach intr = [int_aie2ps_I512_v32_acc32_srs, int_aie2ps_I512_v16_acc64_srs] in {
  def : PatInaccessibleMem<(intr ACC1024:$src,  eS:$shft, 0x0),
             (VSRS_2x_mv_x_srs_cm_srsSign0 ACC1024:$src,  eS:$shft)>;
  def : PatInaccessibleMem<(intr ACC1024:$src,  eS:$shft, 0x1),
             (VSRS_2x_mv_x_srs_cm_srsSign1 ACC1024:$src,  eS:$shft)>;
}
foreach intr = [int_aie2ps_I512_v64_acc32_srs, int_aie2ps_I512_v32_acc64_srs] in {
  def : PatInaccessibleMem<(intr ACC2048:$src,  eS:$shft, 0x0),
             (VSRS_4x_mv_x_srs_dm_srsSign0 ACC2048:$src,  eS:$shft)>;
  def : PatInaccessibleMem<(intr ACC2048:$src,  eS:$shft, 0x1),
             (VSRS_4x_mv_x_srs_dm_srsSign1 ACC2048:$src,  eS:$shft)>;
}

// UPS Intrinsic
// Note : Non-constant sign is handled in .cpp
foreach intr = [int_aie2ps_acc32_v16_I256_ups, int_aie2ps_acc64_v8_I256_ups] in {
  def : PatInaccessibleMem<(intr VEC256:$src,  eS:$shft, 0x0),
             (VUPS_2x_mv_ups_w2b_upsSign0 VEC256:$src,  eS:$shft)>;
  def : PatInaccessibleMem<(intr VEC256:$src,  eS:$shft, 0x1),
             (VUPS_2x_mv_ups_w2b_upsSign1 VEC256:$src,  eS:$shft)>;
}
foreach intr = [int_aie2ps_acc32_v32_I256_ups, int_aie2ps_acc64_v16_I256_ups] in {
  def : PatInaccessibleMem<(intr VEC256:$src,  eS:$shft, 0x0),
             (VUPS_4x_mv_ups_w2c_upsSign0 VEC256:$src,  eS:$shft)>;
  def : PatInaccessibleMem<(intr VEC256:$src,  eS:$shft, 0x1),
             (VUPS_4x_mv_ups_w2c_upsSign1 VEC256:$src,  eS:$shft)>;
}
foreach intr = [int_aie2ps_acc32_v32_I512_ups, int_aie2ps_acc64_v16_I512_ups] in {
  def : PatInaccessibleMem<(intr VEC512:$src,  eS:$shft, 0x0),
             (VUPS_2x_mv_ups_x2c_upsSign0 VEC512:$src,  eS:$shft)>;
  def : PatInaccessibleMem<(intr VEC512:$src,  eS:$shft, 0x1),
             (VUPS_2x_mv_ups_x2c_upsSign1 VEC512:$src,  eS:$shft)>;
}
foreach intr = [int_aie2ps_acc32_v64_I512_ups, int_aie2ps_acc64_v32_I512_ups] in {
  def : PatInaccessibleMem<(intr VEC512:$src,  eS:$shft, 0x0),
             (VUPS_4x_mv_ups_x2d_upsSign0 VEC512:$src,  eS:$shft)>;
  def : PatInaccessibleMem<(intr VEC512:$src,  eS:$shft, 0x1),
             (VUPS_4x_mv_ups_x2d_upsSign1 VEC512:$src,  eS:$shft)>;
}

// LD – Scalar Load 
class LdPat<PatFrag LoadOp, AIE2PSInst Inst, ValueType ResType>
    : Pat<(ResType (LoadOp eP:$rs1)), (Inst eP:$rs1, (i20 0))>;
foreach Ty = [i20, i32, ptr0] in {
  def : LdPat<load, LDA_dms_lda_scalar_ld_idx_imm, Ty>;
}
// Note: Loads to PTR are handled in AIE2PSInstructionSelector.cpp.
class PartWordLdPat<PatFrag LoadOp, AIE2PSInst Inst, ValueType ResType>
    : Pat<(ResType (LoadOp eP:$rs1)), (Inst eP:$rs1, (i20 0 ))>;
def : LdPat<sextloadi16, LDA_s16_idx_imm, i32>;
def : LdPat<extloadi16, LDA_s16_idx_imm, i32>;
def : LdPat<zextloadi16, LDA_u16_idx_imm, i32>;
def : LdPat<sextloadi8, LDA_s8_idx_imm, i32>;
def : LdPat<extloadi8, LDA_s8_idx_imm, i32>;
def : LdPat<zextloadi8, LDA_u8_idx_imm, i32>;
def : LdPat<load, LDA_dml_lda_scalar_L_ld_idx_imm, i64>;

// ST – Scalar Word Store
class StPat<PatFrag StoreOp, AIE2PSInst InstInd, ValueType type, RegisterClass RegClass>
    : Pat<(StoreOp (type RegClass:$rs2), eP:$rs1), (InstInd RegClass:$rs2, eP:$rs1, (i20 0))>;
foreach Ty = [i20, i32, ptr0] in {
	def : StPat<store, ST_dms_sts_scalar_st_idx_imm, Ty, eR>;
}
def : StPat<store, ST_dml_sts_scalar_st_idx_imm, i64, eL>;
def : StPat<store, ST_dms_sts_scalar_st_idx_imm, ptr0, eP>; // Handle stores from PTR
def : StPat<store, ST_dms_sts_scalar_st_idx_imm, i20, eM>; // Handle stores from MOD

// ST.s8 / ST.s16 – Scalar Sub-Word Store
class PartWordStPat<PatFrag StoreOp, AIE2PSInst InstInd, ValueType type, RegisterClass RegClass>
    : Pat<(StoreOp (type RegClass:$rs2), eP:$rs1), (InstInd RegClass:$rs2, eP:$rs1, (i20 0))>;
def : StPat<truncstorei8, ST_s8_idx_imm, i32, eR>;
def : StPat<truncstorei16, ST_s16_idx_imm, i32, eR>;


// VADD/VSUB
def : Pat<(add v64i8:$src1, v64i8:$src2),   (VADD_8  VEC512:$src1, VEC512:$src2)>;
def : Pat<(add v32i16:$src1, v32i16:$src2), (VADD_16 VEC512:$src1, VEC512:$src2)>;
def : Pat<(add v16i32:$src1, v16i32:$src2), (VADD_32 VEC512:$src1, VEC512:$src2)>;

def : Pat<(sub v64i8:$src1, v64i8:$src2),   (VSUB_8  VEC512:$src1, VEC512:$src2)>;
def : Pat<(sub v32i16:$src1, v32i16:$src2), (VSUB_16 VEC512:$src1, VEC512:$src2)>;
def : Pat<(sub v16i32:$src1, v16i32:$src2), (VSUB_32 VEC512:$src1, VEC512:$src2)>;

// G_AIE_BROADCAST_VECTOR
def bcst_vector_node : SDNode<"AIE2PS::G_AIE_BROADCAST_VECTOR",
                              SDTypeProfile<1, 1, []>>;
def : GINodeEquiv<G_AIE_BROADCAST_VECTOR, bcst_vector_node>;

// Scalar i32 src operands select the VBCST instruction according to the dst
// vector element type
def : Pat<(v64i8(bcst_vector_node(i32 eR : $s0))), (VBCST_8 eR : $s0)>;
def : Pat<(v32i16(bcst_vector_node(i32 eR : $s0))), (VBCST_16 eR : $s0)>;
def : Pat<(v16i32(bcst_vector_node(i32 eR : $s0))), (VBCST_32 eR : $s0)>;

// 32-bit src vector types are expected to supply defined values for each lane,
// thus select the 32-bit VBCST
def : Pat<(v64i8(bcst_vector_node(v4i8 eR : $s0))), (VBCST_32 eR : $s0)>;
def : Pat<(v32i16(bcst_vector_node(v2i16 eR : $s0))), (VBCST_32 eR : $s0)>;

// 64-bit src vector types are expected to supply defined values for each lane,
// thus select the 64-bit VBCST
def : Pat<(v64i8(bcst_vector_node(v8i8 eL : $s0))), (VBCST_64 eL : $s0)>;
def : Pat<(v32i16(bcst_vector_node(v4i16 eL : $s0))), (VBCST_64 eL : $s0)>;
def : Pat<(v16i32(bcst_vector_node(v2i32 eL : $s0))), (VBCST_64 eL : $s0)>;
// FIXME: Broadcast of v8i64 is supported through v2i32, which is the only case
// where the src vector element type does not match the dst vector element type.
def : Pat<(v8i64(bcst_vector_node(v2i32 eL : $s0))), (VBCST_64 eL : $s0)>;

// VSHUFFLE
foreach intr = [VSHUFFLE_vec_shuffle_x, VSHUFFLE_vec_shuffle_bm] in {
  def : Pat<(int_aie2ps_vshuffle VEC512:$s1, VEC512:$s2, eR:$mod),
             (intr VEC512:$s1, VEC512:$s2, eR:$mod)>;
}

// VSHIFT Intrinsic (shift/shiftx/shift_bytes)
def : Pat<(int_aie2ps_vshift_I512_I512 VEC512:$s1, VEC512:$s2, 0x0, eR:$shift),
           (VSHIFT VEC512:$s1, VEC512:$s2, eR:$shift)>;
def : Pat<(int_aie2ps_vshift_bf512_bf512 VEC512:$s1, VEC512:$s2, 0x0, eR:$shift),
           (VSHIFT VEC512:$s1, VEC512:$s2, eR:$shift)>;
def : Pat<(int_aie2ps_vshift_I512_I512 VEC512:$s1, VEC512:$s2, mSs:$pre, eR:$shift),
           (VSHIFT_ALIGN VEC512:$s1, mSs:$pre, VEC512:$s2, eR:$shift)>;
def : Pat<(int_aie2ps_vshift_bf512_bf512 VEC512:$s1, VEC512:$s2, mSs:$pre, eR:$shift),
           (VSHIFT_ALIGN VEC512:$s1, mSs:$pre, VEC512:$s2, eR:$shift)>;

// Combine broadcast + shift into VPUSH_hi_64
def : Pat<(int_aie2ps_vshift_I512_I512 (v16i32 VEC512:$s0), (v16i32 (bcst_vector_node(v2i32 eL : $s1))), 0x0, 0x8),
           (VPUSH_hi_64 VEC512:$s0, eL:$s1)>;

// VINSERT
def : Pat<(int_aie2ps_vinsert32_bf512 VEC512:$s1, mR29_insert:$idx, eR:$s0),
           (VINSERT_32_mR29_insert VEC512:$s1, mR29_insert:$idx, eR:$s0)>;
def : Pat<(int_aie2ps_vinsert64_bf512 VEC512:$s1, mR29_insert:$idx, eL:$s0),
           (VINSERT_64_mR29_insert VEC512:$s1, mR29_insert:$idx, eL:$s0)>;

// G_AIE_EXTRACT_SUBVECTOR
def extract_subvec_node : SDNode<"AIE2PS::G_AIE_EXTRACT_SUBVECTOR", SDTypeProfile<1, 2, []>>;
def : GINodeEquiv<G_AIE_EXTRACT_SUBVECTOR , extract_subvec_node>;
// G_AIE_EXTRACT_SUBVECTOR extracts only 32-bit or 64-bit subvector.
class ExtractSubvecPat<AIE2PSInst Inst, ValueType DstTy, ValueType SrcTy,
                       DAGOperand IdxTy> :
  Pat<(DstTy (extract_subvec_node(SrcTy VEC512:$s0), (i32 IdxTy:$idx))),
      (Inst VEC512:$s0, IdxTy:$idx)>;

class ExtractSubvecPair<AIE2PSInst inst, DAGOperand idxTy>{
  AIE2PSInst Inst = inst;
  DAGOperand IdxTy = idxTy;
}

defset list<ExtractSubvecPair> ExtrSubvec32Pairs={
  def : ExtractSubvecPair<VEXTRACT_32_vec_extract_imm_vaddSign1, c6u>;
  def : ExtractSubvecPair<VEXTRACT_32_vec_extract_r_vaddSign1, eR>;

}

defset list<ExtractSubvecPair> ExtrSubvec64Pairs={
  def : ExtractSubvecPair<VEXTRACT_64_vec_extract_imm_vaddSign1, c6u>;
  def : ExtractSubvecPair<VEXTRACT_64_vec_extract_r_vaddSign1, eR>;
}

foreach Pair = ExtrSubvec32Pairs in {
  def : ExtractSubvecPat<Pair.Inst, v4i8, v64i8, Pair.IdxTy>;
  def : ExtractSubvecPat<Pair.Inst, v2i16, v32i16, Pair.IdxTy>;
}

foreach Pair = ExtrSubvec64Pairs in {
  def : ExtractSubvecPat<Pair.Inst, v8i8, v64i8, Pair.IdxTy>;
  def : ExtractSubvecPat<Pair.Inst, v4i16, v32i16, Pair.IdxTy>;
  def : ExtractSubvecPat<Pair.Inst, v2i32, v16i32, Pair.IdxTy>;
  def : ExtractSubvecPat<Pair.Inst, v2i32, v8i64, Pair.IdxTy>;
}

// VBCST
def : Pat<(int_aie2ps_vbroadcast32_bf512 eR:$s0), (VBCST_32 eR:$s0)>;
def : Pat<(int_aie2ps_vbroadcast64_bf512 eL:$s0), (VBCST_64 eL:$s0)>;

// VEXTBCST
def : Pat<(int_aie2ps_vextract_broadcast128_I512 VEC512:$src, eR:$idx),
           (VEXTBCST_128_vec_extract_broadcast_r  VEC512:$src, eR:$idx)>;
def : Pat<(int_aie2ps_vextract_broadcast128_I512 VEC512:$src, c6u:$idx),
           (VEXTBCST_128_vec_extract_broadcast_imm  VEC512:$src, c6u:$idx)>;

// Combine G_AIE_BROADCAST(G_AIE_EXTRACT_SUBVECTOR) into VEXTBCST
// Note:  Broadcast destination type is the same as subvector source type
class ExtBcstSubvecPat<AIE2PSInst Inst, ValueType BcstDstTy,
                      ValueType SubvecDstTy, DAGOperand IdxTy> :
  Pat<(BcstDstTy (bcst_vector_node
                      (SubvecDstTy(extract_subvec_node(BcstDstTy VEC512:$s0),
                                                      (i32 IdxTy:$idx))))),
(Inst VEC512:$s0, IdxTy:$idx)>;

// Extract scalar instead of a subvector
class ExtBcstScalarPat<SDPatternOperator VextrExt, AIE2PSInst Inst, ValueType BcstDstTy,
                      ValueType ScalarDstTy, DAGOperand IdxTy> :
  Pat<(BcstDstTy (bcst_vector_node
                      (ScalarDstTy(VextrExt(BcstDstTy VEC512:$s0),
                                                      (i32 IdxTy:$idx))))),
(Inst VEC512:$s0, IdxTy:$idx)>;

class ExtBcstPair<AIE2PSInst inst, DAGOperand idxTy>{
  AIE2PSInst Inst = inst;
  DAGOperand IdxTy = idxTy;
}

defset list<ExtBcstPair> ExtBcst8Pairs={
  def : ExtBcstPair<VEXTBCST_8_vec_extract_broadcast_imm, c6u>;
  def : ExtBcstPair<VEXTBCST_8_vec_extract_broadcast_r, eR>;
}

defset list<ExtBcstPair> ExtBcst16Pairs={
  def : ExtBcstPair<VEXTBCST_16_vec_extract_broadcast_imm, c6u>;
  def : ExtBcstPair<VEXTBCST_16_vec_extract_broadcast_r, eR>;
}

defset list<ExtBcstPair> ExtBcst32Pairs={
  def : ExtBcstPair<VEXTBCST_32_vec_extract_broadcast_imm, c6u>;
  def : ExtBcstPair<VEXTBCST_32_vec_extract_broadcast_r, eR>;
}

defset list<ExtBcstPair> ExtBcst64Pairs={
  def : ExtBcstPair<VEXTBCST_64_vec_extract_broadcast_imm, c6u>;
  def : ExtBcstPair<VEXTBCST_64_vec_extract_broadcast_r, eR>;
}

foreach  Pair = ExtBcst8Pairs in {
  def : ExtBcstScalarPat<vextract_sext, Pair.Inst, v64i8, i32, Pair.IdxTy>;
  def : ExtBcstScalarPat<vextract_zext, Pair.Inst, v64i8, i32, Pair.IdxTy>;
}

foreach Pair = ExtBcst16Pairs in {
  def : ExtBcstScalarPat<vextract_sext, Pair.Inst, v32i16, i32, Pair.IdxTy>;
  def : ExtBcstScalarPat<vextract_zext,Pair.Inst, v32i16, i32, Pair.IdxTy>;
}

foreach Pair = ExtBcst32Pairs in {
  def : ExtBcstSubvecPat<Pair.Inst, v64i8, v4i8, Pair.IdxTy>;
  def : ExtBcstSubvecPat<Pair.Inst, v32i16, v2i16, Pair.IdxTy>;
  def : ExtBcstScalarPat<vextract_sext, Pair.Inst, v16i32, i32, Pair.IdxTy>;
  def : ExtBcstScalarPat<vextract_zext, Pair.Inst, v16i32, i32, Pair.IdxTy>;
}

foreach  Pair = ExtBcst64Pairs in {
  def : ExtBcstSubvecPat<Pair.Inst, v64i8, v8i8, Pair.IdxTy>;
  def : ExtBcstSubvecPat<Pair.Inst, v32i16, v4i16, Pair.IdxTy>;
  def : ExtBcstSubvecPat<Pair.Inst, v16i32, v2i32, Pair.IdxTy>;
  def : ExtBcstSubvecPat<Pair.Inst, v8i64, v2i32, Pair.IdxTy>;
}

// VEXTBCSTSHFL
// 16 bit
def : Pat<(int_aie2ps_vshuffle
              (v16i32 (bitconvert (v32bf16 (bcst_vector_node
                  (i32 (vextract_sext (v32i16 VEC512:$s0), (i32 mRm:$idx))))))),
              undef, mR29_insert:$mod),
           (VEXTBCSTSHFL_16 VEC512:$s0, mRm:$idx, mR29_insert:$mod)>;
//32 bit
def : Pat<(int_aie2ps_vshuffle
              (v16i32 (bitconvert (v32bf16 (int_aie2ps_vbroadcast32_bf512
                (v2bf16 (bitconvert (i32 (vextract_sext (v16i32 VEC512:$s0), (i32 mRm:$idx))))))))),
              undef, mR29_insert:$mod),
           (VEXTBCSTSHFL_32 VEC512:$s0, mRm:$idx, mR29_insert:$mod)>;
// 64 bit
def : Pat<(int_aie2ps_vshuffle
              (v16i32 (bitconvert (v32bf16 (int_aie2ps_vbroadcast64_bf512
                (v4bf16 (bitconvert (v2i32 (extract_subvec_node (v16i32 VEC512:$s0), (i32 mRm:$idx))))))))),
              undef, mR29_insert:$mod),
           (VEXTBCSTSHFL_64 VEC512:$s0, mRm:$idx, mR29_insert:$mod)>;
// 128 bit
def : Pat<(int_aie2ps_vshuffle
              (v16i32(int_aie2ps_vextract_broadcast128_I512 (v16i32 VEC512:$s0), (i32 mRm:$idx))),
              undef, mR29_insert:$mod),
           (VEXTBCSTSHFL_128 VEC512:$s0, mRm:$idx, mR29_insert:$mod)>;

// VBCSTSHFL
// Note: There are VBCSTSHFL instructions, which return accumulator types, but it seems no Intrinsics of that type.
def : Pat<(int_aie2ps_vbcst_shuffle8 mRm:$s0, mR29_insert:$idx),
           (VBCSTSHFL_8_vec_broadcast_shuffle_x mRm:$s0, mR29_insert:$idx)>;
def : Pat<(int_aie2ps_vbcst_shuffle16 mRm:$s0, mR29_insert:$idx),
           (VBCSTSHFL_16_vec_broadcast_shuffle_x mRm:$s0, mR29_insert:$idx)>;
def : Pat<(int_aie2ps_vbcst_shuffle32 mRm:$s0, mR29_insert:$idx),
           (VBCSTSHFL_32_vec_broadcast_shuffle_x mRm:$s0, mR29_insert:$idx)>;
def : Pat<(int_aie2ps_vbcst_shuffle64 eL:$s0, mR29_insert:$idx),
           (VBCSTSHFL_64_vec_broadcast_shuffle_x eL:$s0, mR29_insert:$idx)>;

// concat_vector - vector bank
def : Pat<(v16i32 (concat_vectors (v8i32 VEC256:$src0), (v8i32 VEC256:$src1))),
  (v16i32 (REG_SEQUENCE VEC512, VEC256:$src0, sub_256_lo, VEC256:$src1, sub_256_hi))>;
def : Pat<(v32i32 (concat_vectors (v16i32 VEC512:$src0), (v16i32 VEC512:$src1))),
  (v32i32 (REG_SEQUENCE VEC1024, VEC512:$src0, sub_512_lo, VEC512:$src1, sub_512_hi))>;

def : Pat<(v32i16 (concat_vectors (v16i16 VEC256:$src0), (v16i16 VEC256:$src1))),
  (v32i16 (REG_SEQUENCE VEC512, VEC256:$src0, sub_256_lo, VEC256:$src1, sub_256_hi))>;
def : Pat<(v64i16 (concat_vectors (v32i16 VEC512:$src0), (v32i16 VEC512:$src1))),
  (v64i16 (REG_SEQUENCE VEC1024, VEC512:$src0, sub_512_lo, VEC512:$src1, sub_512_hi))>;

def : Pat<(v64i8 (concat_vectors (v32i8 VEC256:$src0), (v32i8 VEC256:$src1))),
  (v64i8 (REG_SEQUENCE VEC512, VEC256:$src0, sub_256_lo, VEC256:$src1, sub_256_hi))>;
def : Pat<(v128i8 (concat_vectors (v64i8 VEC512:$src0), (v64i8 VEC512:$src1))),
  (v128i8 (REG_SEQUENCE VEC1024, VEC512:$src0, sub_512_lo, VEC512:$src1, sub_512_hi))>;

// concat_vector - accumulator bank
def : Pat<(v16i64 (concat_vectors (v8i64 ACC512:$src0), (v8i64 ACC512:$src1))),
  (v16i64 (REG_SEQUENCE ACC1024, ACC512:$src0, sub_512_acc_lo, ACC512:$src1, sub_512_acc_hi))>;

def : Pat<(v32i32 (concat_vectors (v16i32 ACC512:$src0), (v16i32 ACC512:$src1))),
  (v32i32 (REG_SEQUENCE ACC1024, ACC512:$src0, sub_512_acc_lo, ACC512:$src1, sub_512_acc_hi))>;

def : Pat<(v32i64 (concat_vectors (v16i64 ACC1024:$src0), (v16i64 ACC1024:$src1))),
  (v32i64 (REG_SEQUENCE ACC2048, ACC1024:$src0, sub_1024_acc_lo, ACC1024:$src1, sub_1024_acc_hi))>;

def : Pat<(v64i32 (concat_vectors (v32i32 ACC1024:$src0), (v32i32 ACC1024:$src1))),
  (v64i32 (REG_SEQUENCE ACC2048, ACC1024:$src0, sub_1024_acc_lo, ACC1024:$src1, sub_1024_acc_hi))>;

// concat_vector - gpr bank
foreach Ty = [v4i16, v8i8] in {
def : Pat<(Ty (concat_vectors eR:$src0, eR:$src1)),
  (Ty (REG_SEQUENCE eL, eR:$src0, sub_l_even, eR:$src1, sub_l_odd))>;
}

// VCONV_BF16_FP32
def : PatInaccessibleMem<(int_aie2ps_v16accfloat_to_v16bf16 ACC512:$src),
          (VCONV_bf16_fp32_mv_conv_mv_w_srs_bf16 ACC512:$src)>;
def : PatInaccessibleMem<(int_aie2ps_v32accfloat_to_v32bf16 ACC1024:$src),
          (VCONV_bf16_fp32_mv_conv_mv_x_srs_bf16 ACC1024:$src)>;

// VCONV_FP16_FP32
def : PatInaccessibleMem<(int_aie2ps_v16accfloat_to_v16float16 ACC512:$src),
          (EXTRACT_SUBREG (VCONV_fp16_fp32_mv_x_srs_fp16 (
              REG_SEQUENCE ACC1024, ACC512:$src, sub_512_acc_lo)), sub_256_lo)>;
def : PatInaccessibleMem<(int_aie2ps_v32accfloat_to_v32float16 ACC1024:$src),
          (VCONV_fp16_fp32_mv_x_srs_fp16 ACC1024:$src)>;

// VCONV_FP8_FP32
def : PatInaccessibleMem<(int_aie2ps_v32accfloat_to_v32float8 ACC1024:$src, eS:$s),
          (VCONV_fp8_fp32 ACC1024:$src, eS:$s)>;

// VCONV_BF8_FP32
def : PatInaccessibleMem<(int_aie2ps_v32accfloat_to_v32bfloat8 ACC1024:$src, eS:$s),
          (VCONV_bf8_fp32 ACC1024:$src, eS:$s)>;

// VCONV_FP32_BF16
def : Pat<(int_aie2ps_v16bf16_to_v16accfloat VEC256:$src),
          (VCONV_fp32_bf16_mv_mv_w_ups_bf16 VEC256:$src)>;
def : Pat<(int_aie2ps_v32bf16_to_v32accfloat VEC512:$src),
          (VCONV_fp32_bf16_mv_mv_x_ups_f16 VEC512:$src)>;

// VCONV_FP32_FP16
def : Pat<(int_aie2ps_v16float16_to_v16accfloat VEC256:$src),
          (EXTRACT_SUBREG (VCONV_fp32_fp16_mv (REG_SEQUENCE VEC512, VEC256:$src, sub_256_lo)), sub_512_acc_lo)>;
def : Pat<(int_aie2ps_v32float16_to_v32accfloat VEC512:$src),
          (VCONV_fp32_fp16_mv VEC512:$src)>;


// VABS_GTZ
// Note : Non-constant sign is handled in .cpp
def : Pat<(int_aie2ps_vabs_gtz8  VEC512:$s1, 0x0), (VABS_GTZ8_vaddSign0  VEC512:$s1)>;
def : Pat<(int_aie2ps_vabs_gtz16 VEC512:$s1, 0x0), (VABS_GTZ16_vaddSign0 VEC512:$s1)>;
def : Pat<(int_aie2ps_vabs_gtz32 VEC512:$s1, 0x0), (VABS_GTZ32_vaddSign0 VEC512:$s1)>;
def : Pat<(int_aie2ps_vabs_gtz8  VEC512:$s1, 0x1), (VABS_GTZ8_vaddSign1  VEC512:$s1)>;
def : Pat<(int_aie2ps_vabs_gtz16 VEC512:$s1, 0x1), (VABS_GTZ16_vaddSign1 VEC512:$s1)>;
def : Pat<(int_aie2ps_vabs_gtz32 VEC512:$s1, 0x1), (VABS_GTZ32_vaddSign1 VEC512:$s1)>;

// VADDSUB
def : Pat<(int_aie2ps_vaddsub8 VEC512:$src1, VEC512:$src2, eL:$lsel),
           (VADDSUB_8 VEC512:$src1, VEC512:$src2, eL:$lsel)>;

def : Pat<(int_aie2ps_vaddsub16 VEC512:$src1, VEC512:$src2, mRS16m:$rsel),
           (VADDSUB_16 VEC512:$src1, VEC512:$src2, mRS16m:$rsel)>;

def : Pat<(int_aie2ps_vaddsub32 VEC512:$src1, VEC512:$src2, mRS16m:$rsel),
           (VADDSUB_32 VEC512:$src1, VEC512:$src2, mRS16m:$rsel)>;

// VBNEG_LTZ
def : Pat<(int_aie2ps_vbneg_ltz8  VEC512:$s1), (VBNEG_LTZ_s8  VEC512:$s1)>;
def : Pat<(int_aie2ps_vbneg_ltz16 VEC512:$s1), (VBNEG_LTZ_s16 VEC512:$s1)>;
def : Pat<(int_aie2ps_vbneg_ltz32 VEC512:$s1), (VBNEG_LTZ_s32 VEC512:$s1)>;

// VEQZ
class VEQZ_INSTR<SDPatternOperator OpNode, AIE2PSInst Inst>:
    Pat<(OpNode VEC512:$s2), (Inst VEC512:$s2)>;

def : VEQZ_INSTR<int_aie2ps_veqz8, VEQZ_8>;
def : VEQZ_INSTR<int_aie2ps_veqz16, VEQZ_16>;
def : VEQZ_INSTR<int_aie2ps_veqz32, VEQZ_32>;

// VLT/VGE
// Note : Non-constant sign is handled in .cpp
class VLT_VGE_INSTR<SDPatternOperator OpNode, AIE2PSInst Inst, bits<1> sgn>:
    Pat<(OpNode VEC512:$s1, VEC512:$s2, sgn), (Inst VEC512:$s1, VEC512:$s2)>;

def : VLT_VGE_INSTR<int_aie2ps_vlt8, VLT_8_vaddSign0, 0x0>;
def : VLT_VGE_INSTR<int_aie2ps_vlt16, VLT_16_vaddSign0, 0x0>;
def : VLT_VGE_INSTR<int_aie2ps_vlt32, VLT_32_vaddSign0, 0x0>;

def : VLT_VGE_INSTR<int_aie2ps_vge8, VGE_8_vaddSign0, 0x0>;
def : VLT_VGE_INSTR<int_aie2ps_vge16, VGE_16_vaddSign0, 0x0>;
def : VLT_VGE_INSTR<int_aie2ps_vge32, VGE_32_vaddSign0, 0x0>;

def : VLT_VGE_INSTR<int_aie2ps_vlt8, VLT_8_vaddSign1, 0x1>;
def : VLT_VGE_INSTR<int_aie2ps_vlt16, VLT_16_vaddSign1, 0x1>;
def : VLT_VGE_INSTR<int_aie2ps_vlt32, VLT_32_vaddSign1, 0x1>;

def : VLT_VGE_INSTR<int_aie2ps_vge8, VGE_8_vaddSign1, 0x1>;
def : VLT_VGE_INSTR<int_aie2ps_vge16, VGE_16_vaddSign1, 0x1>;
def : VLT_VGE_INSTR<int_aie2ps_vge32, VGE_32_vaddSign1, 0x1>;

// VGE/VLT BF16,FP16, FP8, BF8
def : Pat<(int_aie2ps_vltbfloat16 VEC512:$s1, VEC512:$s2),
            (VLT_bf16 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vgebfloat16 VEC512:$s1, VEC512:$s2),
            (VGE_bf16 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vltfloat16 VEC512:$s1, VEC512:$s2),
            (VLT_fp16 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vgefloat16 VEC512:$s1, VEC512:$s2),
            (VGE_fp16 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vltbfloat8 VEC512:$s1, VEC512:$s2),
            (VLT_bf8 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vgebfloat8 VEC512:$s1, VEC512:$s2),
            (VGE_bf8 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vltfloat8 VEC512:$s1, VEC512:$s2),
            (VLT_fp8 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vgefloat8 VEC512:$s1, VEC512:$s2),
            (VGE_fp8 VEC512:$s1, VEC512:$s2)>;

// direct comparisons without intrinsics.

// G_AIE_VECTOR_ICMP
def SDTAIESetCC : SDTypeProfile<1, 3, [        // setcc
  SDTCisInt<0>, SDTCisSameAs<1, 2>, SDTCisVT<3, OtherVT>
]>;

def aie_setcc : SDNode<"AIE2PS::G_AIE_VECTOR_ICMP", SDTAIESetCC>;
def : GINodeEquiv<G_AIE_VECTOR_ICMP, aie_setcc>;

def aie_seteq  : PatFrag<(ops node:$lhs, node:$rhs),
                     (aie_setcc node:$lhs, node:$rhs, SETEQ)>;
def aie_setgt  : PatFrag<(ops node:$lhs, node:$rhs),
                     (aie_setcc node:$lhs, node:$rhs, SETGT)>;
def aie_setugt  : PatFrag<(ops node:$lhs, node:$rhs),
                     (aie_setcc node:$lhs, node:$rhs, SETUGT)>;
def aie_setge  : PatFrag<(ops node:$lhs, node:$rhs),
                     (aie_setcc node:$lhs, node:$rhs, SETGE)>;
def aie_setuge  : PatFrag<(ops node:$lhs, node:$rhs),
                     (aie_setcc node:$lhs, node:$rhs, SETUGE)>;
def aie_setlt  : PatFrag<(ops node:$lhs, node:$rhs),
                     (aie_setcc node:$lhs, node:$rhs, SETLT)>;
def aie_setult  : PatFrag<(ops node:$lhs, node:$rhs),
                     (aie_setcc node:$lhs, node:$rhs, SETULT)>;
def aie_setle  : PatFrag<(ops node:$lhs, node:$rhs),
                     (aie_setcc node:$lhs, node:$rhs, SETLE)>;
def aie_setule  : PatFrag<(ops node:$lhs, node:$rhs),
                     (aie_setcc node:$lhs, node:$rhs, SETULE)>;
def aie_setne  : PatFrag<(ops node:$lhs, node:$rhs),
                     (aie_setcc node:$lhs, node:$rhs, SETNE)>;

// LT
def : Pat<(v64i1 (aie_setlt (v64i8 VEC512:$rs1), (v64i8 VEC512:$rs2))),
	       (VLT_8_vaddSign1 VEC512:$rs1, VEC512:$rs2)>;
def : Pat<(v32i1 (aie_setlt (v32i16 VEC512:$rs1), (v32i16 VEC512:$rs2))),
	       (VLT_16_vaddSign1 VEC512:$rs1, VEC512:$rs2)>;
def : Pat<(v32i1 (aie_setlt (v16i32 VEC512:$rs1), (v16i32 VEC512:$rs2))),
	       (VLT_32_vaddSign1 VEC512:$rs1, VEC512:$rs2)>;

def : Pat<(v64i1 (aie_setult (v64i8 VEC512:$rs1), (v64i8 VEC512:$rs2))),
	       (VLT_8_vaddSign0 VEC512:$rs1, VEC512:$rs2)>;
def : Pat<(v32i1 (aie_setult (v32i16 VEC512:$rs1), (v32i16 VEC512:$rs2))),
	       (VLT_16_vaddSign0 VEC512:$rs1, VEC512:$rs2)>;
def : Pat<(v32i1 (aie_setult (v16i32 VEC512:$rs1), (v16i32 VEC512:$rs2))),
	       (VLT_32_vaddSign0 VEC512:$rs1, VEC512:$rs2)>;

// LE
def : Pat<(v64i1 (aie_setle (v64i8 VEC512:$rs1), (v64i8 VEC512:$rs2))),
         (REG_SEQUENCE eL,
            (XOR (EXTRACT_SUBREG
                    (VLT_8_vaddSign1 VEC512:$rs2, VEC512:$rs1), sub_l_even),
                    (MOV_RLC_imm11_pseudo (i32 -1))), sub_l_even,
            (XOR (EXTRACT_SUBREG
                     (VLT_8_vaddSign1 VEC512:$rs2, VEC512:$rs1), sub_l_odd),
                     (MOV_RLC_imm11_pseudo (i32 -1))), sub_l_odd)>;

def : Pat<(v32i1 (aie_setle (v32i16 VEC512:$rs1), (v32i16 VEC512:$rs2))),
	       (OR
            (VLT_16_vaddSign1 VEC512:$rs1, VEC512:$rs2),
            (VEQZ_16 (VSUB_16 VEC512:$rs1, VEC512:$rs2)))>;

def : Pat<(v32i1 (aie_setle (v16i32 VEC512:$rs1), (v16i32 VEC512:$rs2))),
	       (OR
            (VLT_32_vaddSign1 VEC512:$rs1, VEC512:$rs2),
            (VEQZ_32 (VSUB_32 VEC512:$rs1, VEC512:$rs2)))>;

def : Pat<(v64i1 (aie_setule (v64i8 VEC512:$rs1), (v64i8 VEC512:$rs2))),
         (REG_SEQUENCE eL,
            (XOR (EXTRACT_SUBREG
                    (VLT_8_vaddSign0 VEC512:$rs2, VEC512:$rs1), sub_l_even),
                    (MOV_RLC_imm11_pseudo (i32 -1))), sub_l_even,
            (XOR (EXTRACT_SUBREG
                     (VLT_8_vaddSign0 VEC512:$rs2, VEC512:$rs1), sub_l_odd),
                     (MOV_RLC_imm11_pseudo (i32 -1))), sub_l_odd)>;

def : Pat<(v32i1 (aie_setule (v32i16 VEC512:$rs1), (v32i16 VEC512:$rs2))),
	       (OR
            (VLT_16_vaddSign0 VEC512:$rs1, VEC512:$rs2),
            (VEQZ_16 (VSUB_16 VEC512:$rs1, VEC512:$rs2)))>;

def : Pat<(v32i1 (aie_setule (v16i32 VEC512:$rs1), (v16i32 VEC512:$rs2))),
	       (OR
            (VLT_32_vaddSign0 VEC512:$rs1, VEC512:$rs2),
            (VEQZ_32 (VSUB_32 VEC512:$rs1, VEC512:$rs2)))>;

// GE
def  : Pat<(v64i1 (aie_setge (v64i8 VEC512:$rs1), (v64i8 VEC512:$rs2))),
	       (VGE_8_vaddSign1 VEC512:$rs1, VEC512:$rs2)>;
def  : Pat<(v32i1 (aie_setge (v32i16 VEC512:$rs1), (v32i16 VEC512:$rs2))),
	       (VGE_16_vaddSign1 VEC512:$rs1, VEC512:$rs2)>;
def  : Pat<(v32i1 (aie_setge (v16i32 VEC512:$rs1), (v16i32 VEC512:$rs2))),
	       (VGE_32_vaddSign1 VEC512:$rs1, VEC512:$rs2)>;

def  : Pat<(v64i1 (aie_setuge (v64i8 VEC512:$rs1), (v64i8 VEC512:$rs2))),
	       (VGE_8_vaddSign0 VEC512:$rs1, VEC512:$rs2)>;
def  : Pat<(v32i1 (aie_setuge (v32i16 VEC512:$rs1), (v32i16 VEC512:$rs2))),
	       (VGE_16_vaddSign0 VEC512:$rs1, VEC512:$rs2)>;
def  : Pat<(v32i1 (aie_setuge (v16i32 VEC512:$rs1), (v16i32 VEC512:$rs2))),
	       (VGE_32_vaddSign0 VEC512:$rs1, VEC512:$rs2)>;

// GT
def  : Pat<(v64i1 (aie_setgt (v64i8 VEC512:$rs1), (v64i8 VEC512:$rs2))),
         (REG_SEQUENCE eL,
            (XOR (EXTRACT_SUBREG
                    (VGE_8_vaddSign1 VEC512:$rs2, VEC512:$rs1), sub_l_even),
                    (MOV_RLC_imm11_pseudo (i32 -1))), sub_l_even,
            (XOR (EXTRACT_SUBREG
                    (VGE_8_vaddSign1 VEC512:$rs2, VEC512:$rs1), sub_l_odd),
                    (MOV_RLC_imm11_pseudo (i32 -1))), sub_l_odd)>;

def  : Pat<(v32i1 (aie_setgt (v32i16 VEC512:$rs1), (v32i16 VEC512:$rs2))),
	       (XOR (VGE_16_vaddSign1 VEC512:$rs2, VEC512:$rs1), (MOV_RLC_imm11_pseudo (i32 -1)))>;
def  : Pat<(v32i1 (aie_setgt (v16i32 VEC512:$rs1), (v16i32 VEC512:$rs2))),
	       (XOR (VGE_32_vaddSign1 VEC512:$rs2, VEC512:$rs1), (MOV_RLC_imm11_pseudo (i32 -1)))>;

def  : Pat<(v64i1 (aie_setugt (v64i8 VEC512:$rs1), (v64i8 VEC512:$rs2))),
         (REG_SEQUENCE eL,
            (XOR (EXTRACT_SUBREG
                    (VGE_8_vaddSign0 VEC512:$rs2, VEC512:$rs1), sub_l_even),
                    (MOV_RLC_imm11_pseudo (i32 -1))), sub_l_even,
            (XOR (EXTRACT_SUBREG
                    (VGE_8_vaddSign0 VEC512:$rs2, VEC512:$rs1), sub_l_odd),
                    (MOV_RLC_imm11_pseudo (i32 -1))), sub_l_odd)>;

def  : Pat<(v32i1 (aie_setugt (v32i16 VEC512:$rs1), (v32i16 VEC512:$rs2))),
	       (XOR (VGE_16_vaddSign0 VEC512:$rs2, VEC512:$rs1), (MOV_RLC_imm11_pseudo (i32 -1)))>;
def  : Pat<(v32i1 (aie_setugt (v16i32 VEC512:$rs1), (v16i32 VEC512:$rs2))),
	       (XOR (VGE_32_vaddSign0 VEC512:$rs2, VEC512:$rs1), (MOV_RLC_imm11_pseudo (i32 -1)))>;

// EQ
def  : Pat<(v64i1 (aie_seteq (v64i8 VEC512:$rs1), (v64i8 VEC512:$rs2))),
	       (VEQZ_8 (VSUB_8 VEC512:$rs1, VEC512:$rs2))>;
def  : Pat<(v32i1 (aie_seteq (v32i16 VEC512:$rs1), (v32i16 VEC512:$rs2))),
	       (VEQZ_16 (VSUB_16 VEC512:$rs1, VEC512:$rs2))>;
def  : Pat<(v32i1 (aie_seteq (v16i32 VEC512:$rs1), (v16i32 VEC512:$rs2))),
	       (VEQZ_32 (VSUB_32 VEC512:$rs1, VEC512:$rs2))>;

// NE
def  : Pat<(v64i1 (aie_setne (v64i8 VEC512:$rs1), (v64i8 VEC512:$rs2))),
          (REG_SEQUENCE eL,
             (XOR (EXTRACT_SUBREG
                (VEQZ_8 (VSUB_8 VEC512:$rs1, VEC512:$rs2)), sub_l_even),
                (MOV_RLC_imm11_pseudo (i32 -1))), sub_l_even,
              (XOR (EXTRACT_SUBREG
                (VEQZ_8 (VSUB_8 VEC512:$rs1, VEC512:$rs2)), sub_l_odd),
                (MOV_RLC_imm11_pseudo (i32 -1))), sub_l_odd)>;

def  : Pat<(v32i1 (aie_setne (v32i16 VEC512:$rs1), (v32i16 VEC512:$rs2))),
	       (XOR (VEQZ_16 (VSUB_16 VEC512:$rs1, VEC512:$rs2)), (MOV_RLC_imm11_pseudo (i32 -1)))>;
def  : Pat<(v32i1 (aie_setne (v16i32 VEC512:$rs1), (v16i32 VEC512:$rs2))),
	       (XOR (VEQZ_32 (VSUB_32 VEC512:$rs1, VEC512:$rs2)), (MOV_RLC_imm11_pseudo (i32 -1)))>;

// VMAXDIFF_LT
def : Pat<(int_aie2ps_vmaxdiff_lt8 VEC512:$src1, VEC512:$src2, 0x0),
           (VMAXDIFF_LT_8_vaddSign0 VEC512:$src1, VEC512:$src2)>;
def : Pat<(int_aie2ps_vmaxdiff_lt16 VEC512:$src1, VEC512:$src2, 0x0),
           (VMAXDIFF_LT_16_vaddSign0 VEC512:$src1, VEC512:$src2)>;
def : Pat<(int_aie2ps_vmaxdiff_lt32 VEC512:$src1, VEC512:$src2, 0x0),
           (VMAXDIFF_LT_32_vaddSign0 VEC512:$src1, VEC512:$src2)>;
def : Pat<(int_aie2ps_vmaxdiff_lt8 VEC512:$src1, VEC512:$src2, 0x1),
           (VMAXDIFF_LT_8_vaddSign1 VEC512:$src1, VEC512:$src2)>;
def : Pat<(int_aie2ps_vmaxdiff_lt16 VEC512:$src1, VEC512:$src2, 0x1),
           (VMAXDIFF_LT_16_vaddSign1 VEC512:$src1, VEC512:$src2)>;
def : Pat<(int_aie2ps_vmaxdiff_lt32 VEC512:$src1, VEC512:$src2, 0x1),
           (VMAXDIFF_LT_32_vaddSign1 VEC512:$src1, VEC512:$src2)>;

// VMAX_LT
// Note : Non-constant sign is handled in .cpp
def : Pat<(int_aie2ps_vmax_lt8 VEC512:$s1, VEC512:$s2, 0x0),
           (VMAX_LT_8_vaddSign0 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vmax_lt16 VEC512:$s1, VEC512:$s2, 0x0),
           (VMAX_LT_16_vaddSign0 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vmax_lt32 VEC512:$s1, VEC512:$s2, 0x0),
           (VMAX_LT_32_vaddSign0 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vmax_lt8 VEC512:$s1, VEC512:$s2, 0x1),
           (VMAX_LT_8_vaddSign1 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vmax_lt16 VEC512:$s1, VEC512:$s2, 0x1),
           (VMAX_LT_16_vaddSign1 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vmax_lt32 VEC512:$s1, VEC512:$s2, 0x1),
           (VMAX_LT_32_vaddSign1 VEC512:$s1, VEC512:$s2)>;
// bf16,fp16,bf8,fp8 is always signed
def : Pat<(int_aie2ps_vmax_ltfloat8 VEC512:$s1, VEC512:$s2),
           (VMAX_LT_fp8 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vmax_ltfloat16 VEC512:$s1, VEC512:$s2),
           (VMAX_LT_fp16 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vmax_ltbfloat8 VEC512:$s1, VEC512:$s2),
           (VMAX_LT_bf8 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vmax_ltbfloat16 VEC512:$s1, VEC512:$s2),
           (VMAX_LT_bf16 VEC512:$s1, VEC512:$s2)>;

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

// For vector select, VSEL instruction is used, since it does select word by word
// sel input is modified, "sel ? v1 : v2 is equivalent to vsel(sel-1, v1, v2)"
// rs1(sel) is zero-extended from the original 1 bit type
foreach vec512Ty = [v64i8, v32i16, v16i32] in {
  def : Pat<(vec512Ty (select (i32 eR:$rs1), VEC512:$rs2, VEC512:$rs3)),
            (vec512Ty (VSEL_32 VEC512:$rs2, VEC512:$rs3, (ADD_add_r_ri eR:$rs1, (i32 -1))))>;
}

// VMIN_GE
// Note : Non-constant sign is handled in .cpp
def : Pat<(int_aie2ps_vmin_ge8 VEC512:$s1, VEC512:$s2, 0x0),
           (VMIN_GE_8_vaddSign0 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vmin_ge16 VEC512:$s1, VEC512:$s2, 0x0),
           (VMIN_GE_16_vaddSign0 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vmin_ge32 VEC512:$s1, VEC512:$s2, 0x0),
           (VMIN_GE_32_vaddSign0 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vmin_ge8 VEC512:$s1, VEC512:$s2, 0x1),
           (VMIN_GE_8_vaddSign1 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vmin_ge16 VEC512:$s1, VEC512:$s2, 0x1),
           (VMIN_GE_16_vaddSign1 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vmin_ge32 VEC512:$s1, VEC512:$s2, 0x1),
           (VMIN_GE_32_vaddSign1 VEC512:$s1, VEC512:$s2)>;
// bf16/fp16/bf8/fp8 is always signed
def : Pat<(int_aie2ps_vmin_gefloat8 VEC512:$s1, VEC512:$s2),
           (VMIN_GE_fp8 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vmin_gebfloat8 VEC512:$s1, VEC512:$s2),
           (VMIN_GE_bf8 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vmin_gefloat16 VEC512:$s1, VEC512:$s2),
           (VMIN_GE_fp16 VEC512:$s1, VEC512:$s2)>;
def : Pat<(int_aie2ps_vmin_gebfloat16 VEC512:$s1, VEC512:$s2),
           (VMIN_GE_bf16 VEC512:$s1, VEC512:$s2)>;

// VNEG_GTZ
def : Pat<(int_aie2ps_vneg_gtz8  VEC512:$s1), (VNEG_GTZ_8  VEC512:$s1)>;
def : Pat<(int_aie2ps_vneg_gtz16 VEC512:$s1), (VNEG_GTZ_16 VEC512:$s1)>;
def : Pat<(int_aie2ps_vneg_gtz32 VEC512:$s1), (VNEG_GTZ_32 VEC512:$s1)>;

// VSEL
def : Pat<(int_aie2ps_vsel8 VEC512:$src1, VEC512:$src2, eL:$lsel),
           (VSEL_8 VEC512:$src1, VEC512:$src2, eL:$lsel)>;

def : Pat<(int_aie2ps_vsel16 VEC512:$src1, VEC512:$src2, mRS16m:$rsel),
           (VSEL_16 VEC512:$src1, VEC512:$src2, mRS16m:$rsel)>;

def : Pat<(int_aie2ps_vsel32 VEC512:$src1, VEC512:$src2, mRS16m:$rsel),
           (VSEL_32 VEC512:$src1, VEC512:$src2, mRS16m:$rsel)>;

// G_AIE_VSEL
def vsel_node : SDNode<"AIE2PS::G_AIE_VSEL",
                       SDTypeProfile<1, 3, []>>;
def : GINodeEquiv<G_AIE_VSEL, vsel_node>;

def : Pat<(v64i8(vsel_node (v64i8 VEC512:$src1), (v64i8 VEC512:$src2), (i64 eL:$lsel))), 
  (VSEL_8 VEC512:$src1, VEC512:$src2, eL:$lsel)>;
def : Pat<(v32i16(vsel_node (v32i16 VEC512:$src1), (v32i16 VEC512:$src2), (i32 mRS16m:$rsel))), 
  (VSEL_16 VEC512:$src1, VEC512:$src2, mRS16m:$rsel)>;
def : Pat<(v16i32(vsel_node (v16i32 VEC512:$src1), (v16i32 VEC512:$src2), (i32 mRS16m:$rsel))), 
  (VSEL_32 VEC512:$src1, VEC512:$src2, mRS16m:$rsel)>;

// VSUB_LT/VSUB_GE
// Note : Non-constant sign is handled in .cpp
class VSUB_LT_GE_INSTR<SDPatternOperator OpNode, AIE2PSInst Inst, bits<1> sgn>:
    Pat<(OpNode VEC512:$src1, VEC512:$src2, sgn), (Inst VEC512:$src1, VEC512:$src2)>;

def : VSUB_LT_GE_INSTR<int_aie2ps_vsub_lt8, VSUB_LT_8_vaddSign0, 0x0>;
def : VSUB_LT_GE_INSTR<int_aie2ps_vsub_lt16, VSUB_LT_16_vaddSign0, 0x0>;
def : VSUB_LT_GE_INSTR<int_aie2ps_vsub_lt32, VSUB_LT_32_vaddSign0, 0x0>;

def : VSUB_LT_GE_INSTR<int_aie2ps_vsub_ge8, VSUB_GE_8_vaddSign0, 0x0>;
def : VSUB_LT_GE_INSTR<int_aie2ps_vsub_ge16, VSUB_GE_16_vaddSign0, 0x0>;
def : VSUB_LT_GE_INSTR<int_aie2ps_vsub_ge32, VSUB_GE_32_vaddSign0, 0x0>;

def : VSUB_LT_GE_INSTR<int_aie2ps_vsub_lt8, VSUB_LT_8_vaddSign1, 0x1>;
def : VSUB_LT_GE_INSTR<int_aie2ps_vsub_lt16, VSUB_LT_16_vaddSign1, 0x1>;
def : VSUB_LT_GE_INSTR<int_aie2ps_vsub_lt32, VSUB_LT_32_vaddSign1, 0x1>;

def : VSUB_LT_GE_INSTR<int_aie2ps_vsub_ge8, VSUB_GE_8_vaddSign1, 0x1>;
def : VSUB_LT_GE_INSTR<int_aie2ps_vsub_ge16, VSUB_GE_32_vaddSign1, 0x1>;
def : VSUB_LT_GE_INSTR<int_aie2ps_vsub_ge32, VSUB_GE_32_vaddSign1, 0x1>;

// addr intrinsics
def : Pat<(int_aie2ps_load_4x16_lo VEC256:$ptr), (VLDB_4x16_lo VEC256:$ptr)>;
def : Pat<(int_aie2ps_load_4x16_hi VEC256:$ptr), (VLDB_4x16_hi VEC256:$ptr)>;
def : Pat<(int_aie2ps_load_4x32_lo VEC256:$ptr), (VLDB_4x32_lo VEC256:$ptr)>;
def : Pat<(int_aie2ps_load_4x32_hi VEC256:$ptr), (VLDB_4x32_hi VEC256:$ptr)>;
def : Pat<(int_aie2ps_load_4x64_lo VEC256:$ptr), (VLDB_4x64_lo VEC256:$ptr)>;
def : Pat<(int_aie2ps_load_4x64_hi VEC256:$ptr), (VLDB_4x64_hi VEC256:$ptr)>;

// vector of bfloat16 to vector of int
def : PatInaccessibleMem<(int_aie2ps_v16bf16_to_v16i32 VEC256:$src, eS:$s),
          (VFLOOR_s32_bf16 VEC256:$src, eS:$s)>;

// fx2flt and flt2fx
def : Pat<(int_aie2ps_fx2flt eR:$s0, mS2:$m),
          (MOV_alu_mv_alu_fx2flt eR:$s0, mS2:$m)>;
def : Pat<(int_aie2ps_flt2fx eR:$s0, mS3:$m),
          (MOV_alu_mv_alu_flt2fx eR:$s0, mS3:$m)>;
def : Pat<(int_aie2ps_exp2 ACC512:$src), (VEXP2_bf16 ACC512:$src)>;
def : Pat<(int_aie2ps_exp2_bf20 ACC512:$src), (VEXP2_fp32 ACC512:$src)>;
def : Pat<(int_aie2ps_tanh ACC512:$src), (VTANH ACC512:$src)>;
def : Pat<(int_aie2ps_inv eR:$src), (INV_nlf_inv eR:$src)>;
def : Pat<(int_aie2ps_invsqrt eR:$src), (INVSQRT_nlf_invsqrt eR:$src)>;
def : Pat<(int_aie2ps_sqrtf eR:$src), (SQRT_nlf_sqrt eR:$src)>;
// AIE does not have a native xor instruction for vector operands.
// Res = A xor B --> Res = AB' || A'B
foreach Ty = [v64i8, v32i16, v16i32] in {
def : Pat<(xor Ty:$src1, Ty:$src2),
          (VBOR
            (VBAND (VBNEG_LTZ_s32 Ty:$src1), Ty:$src2),
            (VBAND (VBNEG_LTZ_s32 Ty:$src2), Ty:$src1))>;
}

// DIVS
def : Pat<(int_aie2ps_divs mR31_divs:$sd_in, eR:$src0, eR:$src1),
          (DIVS mR31_divs:$sd_in, eR:$src0, eR:$src1)>;

// G_AIE_VSHIFT_RIGHT
def vshift_node : SDNode<"AIE2PS::G_AIE_VSHIFT_RIGHT",
                          SDTypeProfile<1, 3, [SDTCisVec<0>, SDTCisInt<3>,
                                              SDTCisSameAs<0,1>,
                                              SDTCisSameAs<0,2>]>>;
def : GINodeEquiv<G_AIE_VSHIFT_RIGHT, vshift_node>;


foreach Ty = [v64i8, v32i16, v16i32, v8i64] in {
def : Pat<(Ty (vshift_node VEC512:$src1, VEC512:$src2, (i32 eR:$shift))),
          (VSHIFT VEC512:$src1, VEC512:$src2, eR:$shift)>;
}

// G_AIE_SHUFFLE_VECTOR
def shuffle_vector_node : SDNode<"AIE2PS::G_AIE_SHUFFLE_VECTOR",
                          SDTypeProfile<1, 3, [SDTCisVec<0>, SDTCisInt<3>,
                                              SDTCisSameAs<0,1>,
                                              SDTCisSameAs<0,2>]>>;
def : GINodeEquiv<G_AIE_SHUFFLE_VECTOR, shuffle_vector_node>;


foreach Ty = [v64i8, v32i16, v16i32, v8i64] in {
def : Pat<(Ty (shuffle_vector_node VEC512:$src1, VEC512:$src2, (i32 eR:$mod))),
          (VSHUFFLE_vec_shuffle_x VEC512:$src1, VEC512:$src2, eR:$mod)>;
}

// G_AIE_[SZ]EXT_EXTRACT_VECTOR_ELT
defm : Extract_512<i32, v64i8, (i32 eR:$idx), VEXTRACT_8_vec_extract_r_vaddSign0, VEXTRACT_8_vec_extract_r_vaddSign1>;
defm : Extract_512<i32, v32i16, (i32 eR:$idx), VEXTRACT_16_vec_extract_r_vaddSign0, VEXTRACT_16_vec_extract_r_vaddSign1>;
defm : Extract_512<i32, v16i32, (i32 eR:$idx), VEXTRACT_32_vec_extract_r_vaddSign0, VEXTRACT_32_vec_extract_r_vaddSign1>;
defm : Extract_512<i64, v8i64, (i32 eR:$idx), VEXTRACT_64_vec_extract_r_vaddSign0, VEXTRACT_64_vec_extract_r_vaddSign1>;

defm : Extract_512<i32, v64i8, (i32 c6u:$idx), VEXTRACT_8_vec_extract_imm_vaddSign0, VEXTRACT_8_vec_extract_imm_vaddSign1>;
defm : Extract_512<i32, v32i16, (i32 c6u:$idx), VEXTRACT_16_vec_extract_imm_vaddSign0, VEXTRACT_16_vec_extract_imm_vaddSign1>;
defm : Extract_512<i32, v16i32, (i32 c6u:$idx), VEXTRACT_32_vec_extract_imm_vaddSign0, VEXTRACT_32_vec_extract_imm_vaddSign1>;
defm : Extract_512<i64, v8i64, (i32 c6u:$idx), VEXTRACT_64_vec_extract_imm_vaddSign0, VEXTRACT_64_vec_extract_imm_vaddSign1>;

// G_AIE_INSERT_VECTOR_ELT
def : Insert512Pat<v64i8, (i32 eR:$val), (i32 eR:$idx), VINSERT_8_mR29_insert>;
def : Insert512Pat<v32i16, (i32 eR:$val), (i32 eR:$idx), VINSERT_16_mR29_insert>;
def : Insert512Pat<v16i32, (i32 eR:$val), (i32 eR:$idx), VINSERT_32_mR29_insert>;
def : Insert512Pat<v8i64, (i64 eL:$val), (i32 eR:$idx), VINSERT_64_mR29_insert>;

// Special case: insert at index 0
// The instruction does not encode the idx register
class Insert512Imm0Pat<ValueType VecTy, dag Val, dag Idx, Instruction Inst> :
    Pat<(VecTy (vinsert VecTy:$src1, Val, Idx)),
        (Inst VecTy:$src1, Val)>;

def : Insert512Imm0Pat<v64i8, (i32 eR:$idx), (i32 0), VINSERT_8_mIdxImm0>;
def : Insert512Imm0Pat<v32i16, (i32 eR:$idx), (i32 0), VINSERT_16_mIdxImm0>;
def : Insert512Imm0Pat<v16i32, (i32 eR:$idx), (i32 0), VINSERT_32_mIdxImm0>;
def : Insert512Imm0Pat<v8i64, (i64 eL:$idx), (i32 0), VINSERT_64_mIdxImm0>;

// G_AIE_ADD_VECTOR_ELT_HI
def add_elt_hi_node : SDNode<"AIE2PS::G_AIE_ADD_VECTOR_ELT_HI",
                          SDTypeProfile<1, 2, [SDTCisVec<0>, SDTCisInt<2>,
                                               SDTCisSameAs<0,1>]>>;
def : GINodeEquiv<G_AIE_ADD_VECTOR_ELT_HI, add_elt_hi_node>;

def : Pat<(v64i8 (add_elt_hi_node VEC512:$vec, (i32 eR:$scalar))),
          (VPUSH_hi_8 VEC512:$vec, eR:$scalar)>;
def : Pat<(v32i16 (add_elt_hi_node VEC512:$vec, (i32 eR:$scalar))),
          (VPUSH_hi_16 VEC512:$vec, eR:$scalar)>;
def : Pat<(v16i32 (add_elt_hi_node VEC512:$vec, (i32 eR:$scalar))),
          (VPUSH_hi_32 VEC512:$vec, eR:$scalar)>;
def : Pat<(v8i64 (add_elt_hi_node VEC512:$vec, (i64 eL:$scalar))),
          (VPUSH_hi_64 VEC512:$vec, eL:$scalar)>;

// G_TRUNC

// The G_TRUNC operation can be efficiently implemented using a VSHUFFLE
// instruction. Modes 0, 2, and 4 correspond to the deinterleaved operation of
// 1, 2, and 4 bytes respectively on the concatenated src0 and src1 input
// vectors of the VSHUFFLE instruction.
class Trunc1024Pat<ValueType DstTy, ValueType SrcTy, Instruction ShuffleInstOpc, int Mode> :
  Pat<(DstTy (trunc SrcTy:$s1)),
    (ShuffleInstOpc
      (EXTRACT_SUBREG VEC1024:$s1, sub_512_lo ),
      (EXTRACT_SUBREG VEC1024:$s1, sub_512_hi ),
      (MOV_RLC_imm11_pseudo (i32 Mode)))>;

def : Trunc1024Pat<v16i32, v16i64, VSHUFFLE_vec_shuffle_x, 24>;
def : Trunc1024Pat<v16i32, v16i64, VSHUFFLE_vec_shuffle_bm, 24>;
def : Trunc1024Pat<v32i16, v32i32, VSHUFFLE_vec_shuffle_x, 26>;
def : Trunc1024Pat<v64i8, v64i16, VSHUFFLE_vec_shuffle_x, 28>;

class Trunc512Pat<ValueType DstTy, ValueType SrcTy, int Mode> :
  Pat<(DstTy (trunc SrcTy:$s1)),
    (EXTRACT_SUBREG
      (VSHUFFLE_vec_shuffle_x VEC512:$s1, VEC512:$s1, (MOV_RLC_imm11_pseudo (i32 Mode))),
      sub_256_lo)>;
def : Trunc512Pat<v8i32, v8i64, 24>;
def : Trunc512Pat<v16i16, v16i32, 26>;
def : Trunc512Pat<v32i8, v32i16, 28>;

class EventPat<AIE2PSInst Inst, dag Imm> :
  Pat<(int_aie2ps_event Imm), (Inst)>;

def : EventPat<EVENT_event0,  (i32 0)>;
def : EventPat<EVENT_event1,  (i32 1)>;
def : EventPat<EVENT_WARNING, (i32 2)>;
def : EventPat<EVENT_ERROR,   (i32 3)>;
