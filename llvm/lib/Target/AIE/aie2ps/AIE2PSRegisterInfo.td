//===- AIE2PSRegisterInfo.td -------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//===----------------------------------------------------------------------===//
//  Declarations that describe the AIE2ps Register files
//===----------------------------------------------------------------------===//

include "AIEBaseRegisterInfo.td"

let Namespace = "AIE2PS" in {

def sub_l_even : SubRegIndex<32,  0>;
def sub_l_odd  : SubRegIndex<32, 32>;

def sub_mod        : SubRegIndex<20,  0>;
def sub_dim_size   : SubRegIndex<20, 20>;
def sub_dim_stride : SubRegIndex<20, 40>;
def sub_dim_count  : SubRegIndex<20, 60>;

def sub_lo_dim : SubRegIndex<80,  0>;
def sub_hi_dim : SubRegIndex<80, 80>;

def sub_256_lo  : SubRegIndex<256,   0>;  // sub_512_lo_256_lo
def sub_256_hi  : SubRegIndex<256, 256>;  // sub_512_lo_256_hi

def sub_512_lo  : SubRegIndex<512,   0>;
def sub_512_hi  : SubRegIndex<512, 512>;

def sub_512_acc_lo  : SubRegIndex<512,   0>;
def sub_512_acc_hi  : SubRegIndex<512, 512>;

def sub_1024_acc_lo  : SubRegIndex<1024,   0>;
def sub_1024_acc_hi  : SubRegIndex<1024, 1024>;

def sub_512_hi_256_lo : ComposedSubRegIndex<sub_512_hi, sub_256_lo>;
def sub_512_hi_256_hi : ComposedSubRegIndex<sub_512_hi, sub_256_hi>;

def sub_lo_exp : SubRegIndex<32,  0>;
def sub_hi_exp : SubRegIndex<32, 32>;

def sub_f_lo : SubRegIndex<64,  0>;
def sub_f_hi : SubRegIndex<64, 64>;

def sub_lo_fifo: SubRegIndex<512,  0>;
def sub_hi_fifo : SubRegIndex<512, 512>;

// 320-bits bfp subidxes
def sub_bfp_e32 : SubRegIndex<32, 288>;
def sub_bfp_g32 : SubRegIndex<32, 256>;
def sub_bfp_v256 : SubRegIndex<256, 0>;

// 640-bit bfp subindexes
def sub_bfp320_hi : SubRegIndex<320, 320>;
def sub_bfp320_lo : SubRegIndex<320, 0>;

// 1280-bit bfp subindexes
def sub_bfp640_lo : SubRegIndex<640,   0>;
def sub_bfp640_hi : SubRegIndex<640, 640>;

class AIE2PSGPReg<bits<4> Enc, string n> : Register<n> {
  let HWEncoding{3 - 0} = Enc;
}

class AIE2PGPRPair<bits<4> Enc, int first, int second> :
    Register<"r" # second # ":r" # first> {
  let HWEncoding{3 - 0} = Enc;
  let SubRegs = [!cast<Register>("r"#first),!cast<Register>("r"#second)];
}

class AIE2PS20BitReg<bits<3> Enc, string n> : Register<n> {
  let HWEncoding{2 - 0} = Enc;
}

class AIE2PS1BitReg<bits<1> Enc, string n, list<Register> subregs = []> : Register<n> {
  let HWEncoding{0 - 0} = Enc;
  let SubRegs = subregs;
}

class AIE2PS2BitReg<bits<2> Enc, string n, list<Register> subregs = []> : Register<n> {
  let HWEncoding{1 - 0} = Enc;
  let SubRegs = subregs;
}

class AIE2PS3BitReg<bits<3> Enc, string n, list<Register> subregs = []> : Register<n> {
  let HWEncoding{2 - 0} = Enc;
  let SubRegs = subregs;
}

class AIE2PS4BitReg<bits<4> Enc, string n, list<Register> subregs = []> : Register<n> {
  let HWEncoding{3 - 0} = Enc;
  let SubRegs = subregs;
}

class AIE2PSRegisterClass <int size, int align, list<ValueType> regTypes, dag reglist, RegAltNameIndex idx = NoRegAltName> :
               AIEBaseRegisterClass<"AIE2PS", regTypes, /*alignment*/size, reglist, idx> {
  dag Regs = reglist;
  let RegInfos = RegInfoByHwMode<
      [DefaultMode],
      [RegInfo</*size*/size, /*spill size*/size, /*spill alignment*/align>]>;
  let DecodeZeroBitOperand = true;
}

class AIE2PSScalarRegisterClass<dag reglist, RegAltNameIndex idx = NoRegAltName>
    : AIE2PSRegisterClass<32, 32, [v8i1, v16i1, v32i1, bf16, f16, v2f16, i20, v2bf16, i32, f32, v4i8, v2i16], reglist,
                        idx>;

class AIE2PSScalar64RegisterClass<dag reglist, RegAltNameIndex idx = NoRegAltName>
    : AIE2PSRegisterClass<64, 32, [i64, v8i8, v4i16, v4bf16, v4f16,v2i32, v2f32, v64i1], reglist,
                        idx>;

class AIE2PSVector128RegisterClass<dag reglist> :
    AIE2PSRegisterClass<128, 128, [i128, v16i8, v8i16, v8bf16, v8f16, v4i32, v4f32], reglist>;

class AIE2PSVector256RegisterClass<dag reglist> :
    AIE2PSRegisterClass<256, 256, [v32i8, v16i16, v16bf16,v16f16, v8f32, v8i32], reglist>;

class AIE2PSVector512RegisterClass<dag reglist> :
    AIE2PSRegisterClass<512, 512, [v64i8, v32i16, v32bf16, v32f16,v16i32, v8i64, v16f32], reglist>;

class AIE2PSVector1024RegisterClass<dag reglist> :
    AIE2PSRegisterClass<1024, 512, [v128i8, v64i16, v64bf16, v64f16,v32i32, v16i64, v32f32], reglist>;

class AIE2PSAcc2048RegisterClass<dag reglist> :
    AIE2PSRegisterClass<2048, 512, [v64i32, v32i64, v64f32, v128f16], reglist>;

class AIE2PS20BitRegisterClass <dag reglist, RegAltNameIndex idx = NoRegAltName> :
                 AIEBaseRegisterClass<"AIE2PS", [i20], /*alignment*/ 32, reglist, idx> {
  dag Regs = reglist;
  let RegInfos = RegInfoByHwMode<
      [DefaultMode],
      [RegInfo</*size*/20, /*spill size*/32, /*spill alignment*/32>]>;
  let DecodeZeroBitOperand = true;
} 

class AIE2PSDim2DRegisterClass <dag reglist, RegAltNameIndex idx = NoRegAltName> :
                 AIEBaseRegisterClass<"AIE2PS", [i20], /*alignment*/ 32, reglist, idx> {
  dag Regs = reglist;
  let RegInfos = RegInfoByHwMode<
      [DefaultMode],
      [RegInfo</*size*/80, /*spill size*/128, /*spill alignment*/32>]>;
}

class AIE2PSDim3DRegisterClass <dag reglist, RegAltNameIndex idx = NoRegAltName> :
                 AIEBaseRegisterClass<"AIE2PS", [i20], /*alignment*/ 32, reglist, idx> {
  dag Regs = reglist;
  let RegInfos = RegInfoByHwMode<
      [DefaultMode],
      [RegInfo</*size*/160, /*spill size*/256, /*spill alignment*/32>]>;
}


// Scalar registers (GPR)
foreach i = 0 - 15 in {
  def r #!mul(i,2): AIE2PSGPReg<i, "r" #!mul(i,2)>;
  def r #!add(!mul(i,2), 1): AIE2PSGPReg<i, "r" #!add(!mul(i,2), 1)>;
}


def eReS8 : AIE2PSScalarRegisterClass<(add r0, r2, r4, r6)>;
def eReV16 : AIE2PSScalarRegisterClass<(add r0, r2, r4, r6, r8, r10, r12, r14)>;
def eRe : AIE2PSScalarRegisterClass<(add r0, r2, r4, r6, r8, r10, r12, r14, r16, r18, r20, r22, r24, r26, r28, r30)>;
def eRoS8 : AIE2PSScalarRegisterClass<(add r1, r3, r5, r7)>;
def eRoV16 : AIE2PSScalarRegisterClass<(add r1, r3, r5, r7, r9, r11, r13, r15)>;
def eRo : AIE2PSScalarRegisterClass<(add r1, r3, r5, r7, r9, r11, r13, r15, r17, r19, r21, r23, r25, r27, r29, r31)>;
def mR16m : AIE2PSScalarRegisterClass<(add r16)>;
def eReS16 : AIE2PSScalarRegisterClass<(add r16, r18, r20, r22, r24, r26, r28, r30)>;
def eRoS16 : AIE2PSScalarRegisterClass<(add r17, r19, r21, r23, r25, r27, r29, r31)>;
def mR23_lock : AIE2PSScalarRegisterClass<(add r23)>;
def mReF2b : AIE2PSScalarRegisterClass<(add r24)>;
def mReF2a : AIE2PSScalarRegisterClass<(add r24)>;
def mRoF2b : AIE2PSScalarRegisterClass<(add r25)>;
def mRoF2a : AIE2PSScalarRegisterClass<(add r25)>;
def mR26_fifo_st : AIE2PSScalarRegisterClass<(add r26)>;
def mR27_select : AIE2PSScalarRegisterClass<(add r27)>;
def mMStream_tlast_reg : AIE2PSScalarRegisterClass<(add r28)>;
def mR28_tlast : AIE2PSScalarRegisterClass<(add r28)>;
def mR29_insert : AIE2PSScalarRegisterClass<(add r29)>;
def mR30_shiftx : AIE2PSScalarRegisterClass<(add r30)>;
def mR30_fifo_step_e1 : AIE2PSScalarRegisterClass<(add r30)>;
def mR30_fifo_step_e7 : AIE2PSScalarRegisterClass<(add r30)>;
def mR31_scd : AIE2PSScalarRegisterClass<(add r31)>;
def mR31_divs : AIE2PSScalarRegisterClass<(add r31)>;
def mSCD_r_incr : AIE2PSScalarRegisterClass<(add r31)>;
def mSCD_r : AIE2PSScalarRegisterClass<(add r31)>;
def eReV8 : AIE2PSScalarRegisterClass<(add r8, r10, r12, r14)>;
def eRoV8 : AIE2PSScalarRegisterClass<(add r9, r11, r13, r15)>;
def mReS8s : AIE2PSScalarRegisterClass<(add eReS8)>;
def mRy8 : AIE2PSScalarRegisterClass<(add eReS8, eRoS8)>;
def mReV16s : AIE2PSScalarRegisterClass<(add eReV16)>;
def mRV16s : AIE2PSScalarRegisterClass<(add eReV16, eRoV16)>;
def mRex : AIE2PSScalarRegisterClass<(add eRe)>;
def mRea : AIE2PSScalarRegisterClass<(add eRe)>;
def mRes : AIE2PSScalarRegisterClass<(add eRe)>;
def mRez : AIE2PSScalarRegisterClass<(add eRe)>;
def mRey : AIE2PSScalarRegisterClass<(add eRe)>;
def mRem : AIE2PSScalarRegisterClass<(add eRe)>;
def mRs : AIE2PSScalarRegisterClass<(add eRe, eRo)>;
def mRy : AIE2PSScalarRegisterClass<(add eRe, eRo)>;
def mRz : AIE2PSScalarRegisterClass<(add eRe, eRo)>;
def mRa : AIE2PSScalarRegisterClass<(add eRe, eRo)>;
def mRx : AIE2PSScalarRegisterClass<(add eRe, eRo)>;
def mRm : AIE2PSScalarRegisterClass<(add eRe, eRo)>;
def mRoS8s : AIE2PSScalarRegisterClass<(add eRoS8)>;
def mRoV16s : AIE2PSScalarRegisterClass<(add eRoV16)>;
def mRom : AIE2PSScalarRegisterClass<(add eRo)>;
def mRox : AIE2PSScalarRegisterClass<(add eRo)>;
def mRos : AIE2PSScalarRegisterClass<(add eRo)>;
def mRoz : AIE2PSScalarRegisterClass<(add eRo)>;
def mRoy : AIE2PSScalarRegisterClass<(add eRo)>;
def mRoa : AIE2PSScalarRegisterClass<(add eRo)>;
def mReS16m : AIE2PSScalarRegisterClass<(add eReS16)>;
def mRS16m : AIE2PSScalarRegisterClass<(add eReS16, eRoS16)>;
def mRoS16m : AIE2PSScalarRegisterClass<(add eRoS16)>;
def mRF2a : AIE2PSScalarRegisterClass<(add mReF2a, mRoF2a)>;
def mRF2b : AIE2PSScalarRegisterClass<(add mReF2b, mRoF2b)>;
def mReV8s : AIE2PSScalarRegisterClass<(add eReV8)>;
def mRv8 : AIE2PSScalarRegisterClass<(add eReV8, eRoV8)>;
// Utility class to hold all GPR
def eR : AIE2PSScalarRegisterClass<(add eRo, eRe)>;

// 64-bit L registers
foreach i = 0 - 15 in {
  let SubRegIndices = [sub_l_even, sub_l_odd], CoveredBySubRegs = 1 in
    def l # i : AIE2PGPRPair<i, !mul(i,2), !add(!mul(i,2), 1)>;
}

def eL : AIE2PSScalar64RegisterClass<(add l0, l1, l2, l3, l4, l5, l6, l7, l8, l9, l10, l11, l12, l13, l14, l15)>;
def mL8m : AIE2PSScalar64RegisterClass<(add l8)>;
def mLa : AIE2PSScalar64RegisterClass<(add eL)>;
def mLm : AIE2PSScalar64RegisterClass<(add eL)>;
def mLs : AIE2PSScalar64RegisterClass<(add eL)>;

// Addressing registers
foreach i = 0 - 7 in {
  // Pointer registers
  def p #i : AIE2PS20BitReg<i, "p" #i>;
  // Modifier registers
  def m #i : AIE2PS20BitReg<i, "m" #i>;
  // Dimension count registers
  def dc #i : AIE2PS20BitReg<i, "dc" #i>;
  // Dimension stride (jump) registers
  def dj #i : AIE2PS20BitReg<i, "dj" #i>;
  // Dimension size registers
  def dn #i : AIE2PS20BitReg<i, "dn" #i>;
}

def ePS : AIE2PS20BitRegisterClass<(add p0, p1)>;
def eP : AIE2PS20BitRegisterClass<(add p0, p1, p2, p3, p4, p5, p6, p7)>;
def mPfs : AIE2PS20BitRegisterClass<(add p2)>;
def mPfb : AIE2PS20BitRegisterClass<(add ePS)>;
def mPfa : AIE2PS20BitRegisterClass<(add ePS)>;
def mPb : AIE2PS20BitRegisterClass<(add eP)>;
def mPa : AIE2PS20BitRegisterClass<(add eP)>;
def mPs : AIE2PS20BitRegisterClass<(add eP)>;
def mPm : AIE2PS20BitRegisterClass<(add eP)>;

def eM : AIE2PS20BitRegisterClass<(add m0, m1, m2, m3, m4, m5, m6, m7)>;
def mMa : AIE2PS20BitRegisterClass<(add eM)>;
def mMb : AIE2PS20BitRegisterClass<(add eM)>;
def mMm : AIE2PS20BitRegisterClass<(add eM)>;
def mMs : AIE2PS20BitRegisterClass<(add eM)>;

def eDCL : AIE2PS20BitRegisterClass<(add dc0, dc1, dc2, dc3)>;
def eDC : AIE2PS20BitRegisterClass<(add dc0, dc1, dc2, dc3, dc4, dc5, dc6, dc7)>;
def eDCH : AIE2PS20BitRegisterClass<(add dc4, dc5, dc6, dc7)>;
def mDCLb : AIE2PS20BitRegisterClass<(add eDCL)>;
def mDCLs : AIE2PS20BitRegisterClass<(add eDCL)>;
def mDCLa : AIE2PS20BitRegisterClass<(add eDCL)>;
def mDCHs : AIE2PS20BitRegisterClass<(add eDCH)>;
def mDCHa : AIE2PS20BitRegisterClass<(add eDCH)>;
def mDCHb : AIE2PS20BitRegisterClass<(add eDCH)>;
def mDCa : AIE2PS20BitRegisterClass<(add eDC)>;
def mDCb : AIE2PS20BitRegisterClass<(add eDC)>;
def mDCm : AIE2PS20BitRegisterClass<(add eDC)>;
def mDCs : AIE2PS20BitRegisterClass<(add eDC)>;

def eDJL : AIE2PS20BitRegisterClass<(add dj0, dj1, dj2, dj3)>;
def eDJ : AIE2PS20BitRegisterClass<(add dj0, dj1, dj2, dj3, dj4, dj5, dj6, dj7)>;
def eDJH : AIE2PS20BitRegisterClass<(add dj4, dj5, dj6, dj7)>;
def mDJa : AIE2PS20BitRegisterClass<(add eDJ)>;
def mDJm : AIE2PS20BitRegisterClass<(add eDJ)>;
def mDJb : AIE2PS20BitRegisterClass<(add eDJ)>;
def mDJs : AIE2PS20BitRegisterClass<(add eDJ)>;
def mDJHb : AIE2PS20BitRegisterClass<(add eDJH)>;
def mDJHa : AIE2PS20BitRegisterClass<(add eDJH)>;
def mDJHs : AIE2PS20BitRegisterClass<(add eDJH)>;
def mDJLs : AIE2PS20BitRegisterClass<(add eDJL)>;
def mDJLb : AIE2PS20BitRegisterClass<(add eDJL)>;
def mDJLa : AIE2PS20BitRegisterClass<(add eDJL)>;

def eDNL : AIE2PS20BitRegisterClass<(add dn0, dn1, dn2, dn3)>;
def eDN : AIE2PS20BitRegisterClass<(add dn0, dn1, dn2, dn3, dn4, dn5, dn6, dn7)>;
def eDNH : AIE2PS20BitRegisterClass<(add dn4, dn5, dn6, dn7)>;
def mDNa : AIE2PS20BitRegisterClass<(add eDN)>;
def mDNs : AIE2PS20BitRegisterClass<(add eDN)>;
def mDNb : AIE2PS20BitRegisterClass<(add eDN)>;
def mDNm : AIE2PS20BitRegisterClass<(add eDN)>;
def mDNHa : AIE2PS20BitRegisterClass<(add eDNH)>;
def mDNHs : AIE2PS20BitRegisterClass<(add eDNH)>;
def mDNHb : AIE2PS20BitRegisterClass<(add eDNH)>;
def mDNLb : AIE2PS20BitRegisterClass<(add eDNL)>;
def mDNLs : AIE2PS20BitRegisterClass<(add eDNL)>;
def mDNLa : AIE2PS20BitRegisterClass<(add eDNL)>;

def mDm : AIE2PS20BitRegisterClass<(add eDN, eDJ, eDC, eM)>;
def mAguDst : AIE2PS20BitRegisterClass<(add eP, eM, eDN, eDJ, eDC)>;

let SubRegIndices = [sub_mod, sub_dim_size, sub_dim_stride, sub_dim_count], CoveredBySubRegs = 1 in {
  foreach i = 0 - 7 in {
    def d #i : AIE2PS3BitReg<i, "d" #i, [!cast<Register>("m" #i), 
                                                !cast<Register>("dn" #i),
                                                !cast<Register>("dj" #i),
                                                !cast<Register>("dc" #i)]>;
  }
} // let SubRegIndices = [sub_mod, sub_dim_size, sub_dim_stride, sub_dim_count], CoveredBySubRegs = 1

class AIE2PSRegs3D<bits<2> Enc, string n, list<Register> subregs> : Register<n> {
  let HWEncoding{1 - 0} = Enc;
  let SubRegs = subregs;
}
let SubRegIndices = [sub_lo_dim, sub_hi_dim], CoveredBySubRegs = 1 in {
  def d0_3d : AIE2PSRegs3D<0b00, "d0", [d0, d4]>;
  def d1_3d : AIE2PSRegs3D<0b01, "d1", [d1, d5]>;
  def d2_3d : AIE2PSRegs3D<0b10, "d2", [d2, d6]>;
  def d3_3d : AIE2PSRegs3D<0b11, "d3", [d3, d7]>;
} // let SubRegIndices = [sub_lo_dim, sub_hi_dim], CoveredBySubRegs = 1

def eD : AIE2PSDim2DRegisterClass<(add d0, d1, d2, d3, d4, d5, d6, d7)>;
def mDa : AIE2PSDim2DRegisterClass<(add eD)>;
def mDs : AIE2PSDim2DRegisterClass<(add eD)>;
def mDb : AIE2PSDim2DRegisterClass<(add eD)>;

def eDS : AIE2PSDim3DRegisterClass<(add d0_3d, d1_3d, d2_3d, d3_3d)>;
def mDSa : AIE2PSDim3DRegisterClass<(add eDS)>;
def mDSs : AIE2PSDim3DRegisterClass<(add eDS)>;
def mDSb : AIE2PSDim3DRegisterClass<(add eDS)>;

class AIE2PSShiftReg<bits<2> Enc, string n> : Register<n> {
  let HWEncoding{1 - 0} = Enc;
}
// Shift registers
foreach i = 0 - 3 in {
  def s #i : AIE2PSShiftReg<i, "s" #i>;
}

def eS : AIE2PSScalarRegisterClass<(add s0, s1, s2, s3)>;
def mFx2flt : AIE2PSScalarRegisterClass<(add s2)>;
def mNlfFx2flt : AIE2PSScalarRegisterClass<(add s2)>;
def mS2 : AIE2PSScalarRegisterClass<(add s2)>;
def mNlfFlt2fx : AIE2PSScalarRegisterClass<(add s3)>;
def mS3 : AIE2PSScalarRegisterClass<(add s3)>;
def mFlt2fx : AIE2PSScalarRegisterClass<(add s3)>;
def mFlt2fx_delayed : AIE2PSScalarRegisterClass<(add s3)>;
def mSa : AIE2PSScalarRegisterClass<(add eS)>;
def mSs : AIE2PSScalarRegisterClass<(add eS)>;
def mSm : AIE2PSScalarRegisterClass<(add eS)>;


foreach i = 0 - 5 in {
  def wh #!mul(i,2) : AIE2PS3BitReg<i, "wh" #!mul(i,2)>;
  def wh #!add(!mul(i,2), 1) : AIE2PS3BitReg<i, "wh" #!add(!mul(i,2), 1)>;
  def wl #!mul(i,2) : AIE2PS3BitReg<i, "wl" #!mul(i,2)>;
  def wl #!add(!mul(i,2), 1) : AIE2PS3BitReg<i, "wl" #!add(!mul(i,2), 1)>;
}

def eWhe : AIE2PSVector256RegisterClass<(add wh0, wh2, wh4, wh6, wh8, wh10)>;
def eWho : AIE2PSVector256RegisterClass<(add wh1, wh3, wh5, wh7, wh9, wh11)>;
def eWle : AIE2PSVector256RegisterClass<(add wl0, wl2, wl4, wl6, wl8, wl10)>;
def eWlo : AIE2PSVector256RegisterClass<(add wl1, wl3, wl5, wl7, wl9, wl11)>;
def mWheb : AIE2PSVector256RegisterClass<(add eWhe)>;
def mWhes : AIE2PSVector256RegisterClass<(add eWhe)>;
def mWhem : AIE2PSVector256RegisterClass<(add eWhe)>;
def mWhea : AIE2PSVector256RegisterClass<(add eWhe)>;
def mWhs : AIE2PSVector256RegisterClass<(add eWhe, eWho)>;
def mWhb : AIE2PSVector256RegisterClass<(add eWhe, eWho)>;
def mWha : AIE2PSVector256RegisterClass<(add eWhe, eWho)>;
def mWhm : AIE2PSVector256RegisterClass<(add eWhe, eWho)>;
def mWhom : AIE2PSVector256RegisterClass<(add eWho)>;
def mWhob : AIE2PSVector256RegisterClass<(add eWho)>;
def mWhoa : AIE2PSVector256RegisterClass<(add eWho)>;
def mWhos : AIE2PSVector256RegisterClass<(add eWho)>;
def mWlem : AIE2PSVector256RegisterClass<(add eWle)>;
def mWles : AIE2PSVector256RegisterClass<(add eWle)>;
def mWleb : AIE2PSVector256RegisterClass<(add eWle)>;
def mWlea : AIE2PSVector256RegisterClass<(add eWle)>;
def mWlm : AIE2PSVector256RegisterClass<(add eWle, eWlo)>;
def mWlb : AIE2PSVector256RegisterClass<(add eWle, eWlo)>;
def mWls : AIE2PSVector256RegisterClass<(add eWle, eWlo)>;
def mWla : AIE2PSVector256RegisterClass<(add eWle, eWlo)>;
def mWb : AIE2PSVector256RegisterClass<(add eWle, eWlo, eWhe, eWho)>;
def mWa : AIE2PSVector256RegisterClass<(add eWle, eWlo, eWhe, eWho)>;
def mWs : AIE2PSVector256RegisterClass<(add eWle, eWlo, eWhe, eWho)>;
def mWm : AIE2PSVector256RegisterClass<(add eWle, eWlo, eWhe, eWho)>;
def mWs_128b : AIE2PSVector256RegisterClass<(add eWle, eWlo, eWhe, eWho)>;
def mFl2FxSrc_W : AIE2PSVector256RegisterClass<(add eWle, eWlo, eWhe, eWho)>;
def mWlom : AIE2PSVector256RegisterClass<(add eWlo)>;
def mWlos : AIE2PSVector256RegisterClass<(add eWlo)>;
def mWloa : AIE2PSVector256RegisterClass<(add eWlo)>;
def mWlob : AIE2PSVector256RegisterClass<(add eWlo)>;

class AIE2PSVRegY<bits<3> Enc, string n, list<Register> subregs = []> : Register<n> {
  let HWEncoding{2-0} = Enc;
  let SubRegs = subregs;
}
foreach i = 0 - 5 in {
  let SubRegIndices = [sub_256_lo, sub_256_hi], CoveredBySubRegs = 1 in {
    def x #!mul(i,2) : AIE2PS3BitReg<i, "x" #!mul(i,2), [!cast<Register>("wl" #!mul(i,2)), !cast<Register>("wh" #!mul(i,2))]>;
    def x #!add(!mul(i,2), 1) : AIE2PS3BitReg<i, "x" #!add(!mul(i,2), 1), [!cast<Register>("wl" #!add(!mul(i,2), 1)), !cast<Register>("wh" #!add(!mul(i,2), 1))]>;
  } // let SubRegIndices = [sub_256_lo, sub_256_hi], CoveredBySubRegs = 1
  let SubRegIndices = [sub_512_lo, sub_512_hi], CoveredBySubRegs = 1 in {
    def y #i : AIE2PSVRegY<i, "y" #i, [!cast<Register>("x" #!mul(i,2)), !cast<Register>("x" #!add(!mul(i,2),1))]>;
  } // let SubRegIndices = [sub_512_lo, sub_512_hi], CoveredBySubRegs = 1
}

def eXe : AIE2PSVector512RegisterClass<(add x0, x2, x4, x6, x8, x10)>;
def eXo : AIE2PSVector512RegisterClass<(add x1, x3, x5, x7, x9, x11)>;
def mXev : AIE2PSVector512RegisterClass<(add eXe)>;
def mXem : AIE2PSVector512RegisterClass<(add eXe)>;
def mXea : AIE2PSVector512RegisterClass<(add eXe)>;
def mXew : AIE2PSVector512RegisterClass<(add eXe)>;
def mXen : AIE2PSVector512RegisterClass<(add eXe)>;
def mXeb : AIE2PSVector512RegisterClass<(add eXe)>;
def mXes : AIE2PSVector512RegisterClass<(add eXe)>;
def mXw : AIE2PSVector512RegisterClass<(add eXe, eXo)>;
def mXs : AIE2PSVector512RegisterClass<(add eXe, eXo)>;
def mXm : AIE2PSVector512RegisterClass<(add eXe, eXo)>;
def mXn : AIE2PSVector512RegisterClass<(add eXe, eXo)>;
def mXv : AIE2PSVector512RegisterClass<(add eXe, eXo)>;
def mXa : AIE2PSVector512RegisterClass<(add eXe, eXo)>;
def mXb : AIE2PSVector512RegisterClass<(add eXe, eXo)>;
def mMvXDst : AIE2PSVector512RegisterClass<(add eXe, eXo)>;
def mMcdXSrc : AIE2PSVector512RegisterClass<(add eXe, eXo)>;
def mShflXDst : AIE2PSVector512RegisterClass<(add eXe, eXo)>;
def mMvXSrc : AIE2PSVector512RegisterClass<(add eXe, eXo)>;
def mXs_split : AIE2PSVector512RegisterClass<(add eXe, eXo)>;
def mXov : AIE2PSVector512RegisterClass<(add eXo)>;
def mXos : AIE2PSVector512RegisterClass<(add eXo)>;
def mXob : AIE2PSVector512RegisterClass<(add eXo)>;
def mXoa : AIE2PSVector512RegisterClass<(add eXo)>;
def mXom : AIE2PSVector512RegisterClass<(add eXo)>;
def mXow : AIE2PSVector512RegisterClass<(add eXo)>;
def mXon : AIE2PSVector512RegisterClass<(add eXo)>;

def eY : AIE2PSVector1024RegisterClass<(add y0, y1, y2, y3, y4, y5)>;
def mYb : AIE2PSVector1024RegisterClass<(add eY)>;
def mYs : AIE2PSVector1024RegisterClass<(add eY)>;
def mYv : AIE2PSVector1024RegisterClass<(add eY)>;
def mYw : AIE2PSVector1024RegisterClass<(add eY)>;

foreach i = 0 - 7 in {
  def bmll #i : AIE2PS3BitReg<i, "bmll" #i>;
}
foreach i = 0 - 7 in {
  def bmlh #i : AIE2PS3BitReg<i, "bmlh" #i>;
}
foreach i = 0 - 7 in {
  def bmhl #i : AIE2PS3BitReg<i, "bmhl" #i>;
}
foreach i = 0 - 7 in {
  def bmhh #i : AIE2PS3BitReg<i, "bmhh" #i>;
}

def eBMSHH : AIE2PSVector512RegisterClass<(add bmhh0, bmhh1, bmhh2, bmhh3)>;
def eBMHH : AIE2PSVector512RegisterClass<(add bmhh0, bmhh1, bmhh2, bmhh3, bmhh4, bmhh5, bmhh6, bmhh7)>;
def eBMSHL : AIE2PSVector512RegisterClass<(add bmhl0, bmhl1, bmhl2, bmhl3)>;
def eBMHL : AIE2PSVector512RegisterClass<(add bmhl0, bmhl1, bmhl2, bmhl3, bmhl4, bmhl5, bmhl6, bmhl7)>;
def eBMSLH : AIE2PSVector512RegisterClass<(add bmlh0, bmlh1, bmlh2, bmlh3)>;
def eBMLH : AIE2PSVector512RegisterClass<(add bmlh0, bmlh1, bmlh2, bmlh3, bmlh4, bmlh5, bmlh6, bmlh7)>;
def eBMSLL : AIE2PSVector512RegisterClass<(add bmll0, bmll1, bmll2, bmll3)>;
def eBMLL : AIE2PSVector512RegisterClass<(add bmll0, bmll1, bmll2, bmll3, bmll4, bmll5, bmll6, bmll7)>;
def mBMHHs : AIE2PSVector512RegisterClass<(add eBMHH)>;
def mBMHHa : AIE2PSVector512RegisterClass<(add eBMHH)>;
def mBMHHm : AIE2PSVector512RegisterClass<(add eBMHH)>;
def mBMHLm : AIE2PSVector512RegisterClass<(add eBMHL)>;
def mBMHLs : AIE2PSVector512RegisterClass<(add eBMHL)>;
def mBMHLa : AIE2PSVector512RegisterClass<(add eBMHL)>;
def mBMLHm : AIE2PSVector512RegisterClass<(add eBMLH)>;
def mBMLHs : AIE2PSVector512RegisterClass<(add eBMLH)>;
def mBMLHa : AIE2PSVector512RegisterClass<(add eBMLH)>;
def mBMLLm : AIE2PSVector512RegisterClass<(add eBMLL)>;
def mBMLLa : AIE2PSVector512RegisterClass<(add eBMLL)>;
def mBMLLs : AIE2PSVector512RegisterClass<(add eBMLL)>;
def mBMa : AIE2PSVector512RegisterClass<(add eBMLL, eBMLH, eBMHL, eBMHH)>;
def mBMm : AIE2PSVector512RegisterClass<(add eBMLL, eBMLH, eBMHL, eBMHH)>;
def mBMs : AIE2PSVector512RegisterClass<(add eBMLL, eBMLH, eBMHL, eBMHH)>;
def mMvBMSrc : AIE2PSVector512RegisterClass<(add eBMLL, eBMLH, eBMHL, eBMHH)>;
def mMvBMDst : AIE2PSVector512RegisterClass<(add eBMLL, eBMLH, eBMHL, eBMHH)>;
def mBM_ld : AIE2PSVector512RegisterClass<(add eBMLL, eBMLH, eBMHL, eBMHH)>;
def mMcdBMSrc : AIE2PSVector512RegisterClass<(add eBMLL, eBMLH, eBMHL, eBMHH)>;
def mBMs_v16w32 : AIE2PSVector512RegisterClass<(add eBMLL, eBMLH, eBMHL, eBMHH)>;
def mBMSHHm : AIE2PSVector512RegisterClass<(add eBMSHH)>;
def mBMSHLm : AIE2PSVector512RegisterClass<(add eBMSHL)>;
def mBMSLHm : AIE2PSVector512RegisterClass<(add eBMSLH)>;
def mBMSLLm : AIE2PSVector512RegisterClass<(add eBMSLL)>;
def mBMSm : AIE2PSVector512RegisterClass<(add eBMSLL, eBMSLH, eBMSHL, eBMSHH)>;
def mShflBMDst : AIE2PSVector512RegisterClass<(add eBMSLL, eBMSLH, eBMSHL, eBMSHH)>;

class AIE2PSVRegDM<bits<3> Enc, string n, list<Register> subregs = []> : Register<n> {
  let HWEncoding{2-0} = Enc;
  let SubRegs = subregs;
}
foreach i = 0 - 7 in {
  let SubRegIndices = [sub_512_acc_lo, sub_512_acc_hi], CoveredBySubRegs = 1 in {
    def cml #i : AIE2PS3BitReg<i, "cml" #i, [!cast<Register>("bmll" #i), !cast<Register>("bmlh" #i)]>;
    def cmh #i : AIE2PS3BitReg<i, "cmh" #i, [!cast<Register>("bmhl" #i), !cast<Register>("bmhh" #i)]>;
  } // let SubRegIndices = [sub_512_acc_lo, sub_512_acc_hi], CoveredBySubRegs = 1
  let SubRegIndices = [sub_1024_acc_lo, sub_1024_acc_hi], CoveredBySubRegs = 1 in {
      def dm #i : AIE2PSVRegDM<i, "dm" #i, [!cast<Register>("cml" #i), !cast<Register>("cmh" #i)]>;
  } // let SubRegIndices = [sub_1024_acc_lo, sub_1024_acc_hi], CoveredBySubRegs = 1
}

def eCMH : AIE2PSVector1024RegisterClass<(add cmh0, cmh1, cmh2, cmh3, cmh4, cmh5, cmh6, cmh7)>;
def eCML : AIE2PSVector1024RegisterClass<(add cml0, cml1, cml2, cml3, cml4, cml5, cml6, cml7)>;
def mCMHs : AIE2PSVector1024RegisterClass<(add eCMH)>;
def mCMHm : AIE2PSVector1024RegisterClass<(add eCMH)>;
def mCMHa : AIE2PSVector1024RegisterClass<(add eCMH)>;
def mCMLv : AIE2PSVector1024RegisterClass<(add eCML)>;
def mCMLm : AIE2PSVector1024RegisterClass<(add eCML)>;
def mCMLs : AIE2PSVector1024RegisterClass<(add eCML)>;
def mCMLa : AIE2PSVector1024RegisterClass<(add eCML)>;
def mCMm : AIE2PSVector1024RegisterClass<(add eCML, eCMH)>;
def mCMa : AIE2PSVector1024RegisterClass<(add eCML, eCMH)>;
def mCMs : AIE2PSVector1024RegisterClass<(add eCML, eCMH)>;

def eDM : AIE2PSAcc2048RegisterClass<(add dm0, dm1, dm2, dm3, dm4, dm5, dm6, dm7)>;
def mDMv : AIE2PSAcc2048RegisterClass<(add eDM)>;
def mDMs : AIE2PSAcc2048RegisterClass<(add eDM)>;
def mDMm : AIE2PSAcc2048RegisterClass<(add eDM)>;

// Load FIFO register
def lfh0 : AIE2PS1BitReg <0, "lfh0">;
def lfh1 : AIE2PS1BitReg <1, "lfh1">;
def lfl0 : AIE2PS1BitReg <0, "lfl0">;
def lfl1 : AIE2PS1BitReg <1, "lfl1">;

let SubRegIndices = [sub_lo_fifo, sub_hi_fifo], CoveredBySubRegs = 1 in {
  def lf0 : AIE2PS1BitReg <0, "lf0", [lfl0, lfh0]>;
  def lf1 : AIE2PS1BitReg <1, "lf1", [lfl1, lfh1]>;
}

def eLdFifoReg : AIE2PSVector1024RegisterClass<(add lf0, lf1)>;
def mLdFifo_b : AIE2PSVector1024RegisterClass<(add eLdFifoReg)>;
def mLdFifo_a : AIE2PSVector1024RegisterClass<(add eLdFifoReg)>;

def eLdFifoHReg : AIE2PSVector512RegisterClass<(add lfh0, lfh1)>;
def eLdFifoLReg : AIE2PSVector512RegisterClass<(add lfl0, lfl1)>;
def mLdFifoH : AIE2PSVector512RegisterClass<(add eLdFifoHReg)>;
def mLdFifoL : AIE2PSVector512RegisterClass<(add eLdFifoLReg)>;

// FIFO extra register
def lfe : Register <"lfe">;
  // Store FIFO register
def sfh : Register <"sfh">;
def sfl : Register <"sfl">;

let SubRegIndices = [sub_lo_fifo, sub_hi_fifo], SubRegs = [sfl, sfh], CoveredBySubRegs = 1 in {
  def sf : Register<"sf">;
}

def mStFifoh : AIE2PSVector512RegisterClass<(add sfh)>;
def mStFifol : AIE2PSVector512RegisterClass<(add sfl)>;

def mFifoExtra : AIE2PSVector512RegisterClass<(add lfe)>;
def mFifoHLReg : AIE2PSVector512RegisterClass<(add mStFifoh, mStFifol, eLdFifoHReg, eLdFifoLReg, mFifoExtra)>;
def mMvFifoSrc : AIE2PSVector512RegisterClass<(add mStFifoh, mStFifol, eLdFifoHReg, eLdFifoLReg, mFifoExtra)>;
def mMvFifoDst : AIE2PSVector512RegisterClass<(add mStFifoh, mStFifol, eLdFifoHReg, eLdFifoLReg, mFifoExtra)>;
def mMvBMXSrc : AIE2PSVector512RegisterClass<(add eBMLL, eBMLH, eBMHL, eBMHH, eXe, eXo, mStFifoh, mStFifol, eLdFifoHReg, eLdFifoLReg, mFifoExtra)>;
def mMvBMXDst : AIE2PSVector512RegisterClass<(add eBMLL, eBMLH, eBMHL, eBMHH, eXe, eXo, mStFifoh, mStFifol, eLdFifoHReg, eLdFifoLReg, mFifoExtra)>;

def mStFifo : AIE2PSVector1024RegisterClass<(add sf)>;

// Exponent registers
// 32-bit Exponent registers
foreach i = 0 - 5 in {
  def el #!mul(i,2) : AIE2PS3BitReg<i, "el" #!mul(i,2)>;
  def el #!add(!mul(i,2), 1) : AIE2PS3BitReg<i, "el" #!add(!mul(i,2), 1)>;
  def eh #!mul(i,2) : AIE2PS3BitReg<i, "eh" #!mul(i,2)>;
  def eh #!add(!mul(i,2), 1) : AIE2PS3BitReg<i, "eh" #!add(!mul(i,2), 1)>;
}
// 64-bit Exponent registers
foreach i = 0 - 5 in {
  let SubRegIndices = [sub_lo_exp, sub_hi_exp], CoveredBySubRegs = 1 in {
    // def e #i : AIE2PS4BitReg<i, "e" #i, [!cast<Register>("el" #i), !cast<Register>("eh" #i)]>;
    def e #!mul(i,2) : AIE2PS3BitReg<i, "e" #!mul(i,2), [!cast<Register>("el" #!mul(i,2)), !cast<Register>("eh" #!mul(i,2))]>;
    def e #!add(!mul(i,2), 1) : AIE2PS3BitReg<i, "e" #!add(!mul(i,2), 1), [!cast<Register>("el" #!add(!mul(i,2), 1)), !cast<Register>("eh" #!add(!mul(i,2), 1))]>;
  }
}

def eEle : AIE2PSScalarRegisterClass<(add el0, el2, el4, el6, el8, el10)>;
def eElo : AIE2PSScalarRegisterClass<(add el1, el3, el5, el7, el9, el11)>;
def eEhe : AIE2PSScalarRegisterClass<(add eh0, eh2, eh4, eh6, eh8, eh10)>;
def eEho : AIE2PSScalarRegisterClass<(add eh1, eh3, eh5, eh7, eh9, eh11)>;

def mElea : AIE2PSScalarRegisterClass<(add eEle)>;
def mEles : AIE2PSScalarRegisterClass<(add eEle)>;
def mElem : AIE2PSScalarRegisterClass<(add eEle)>;
def mEls : AIE2PSScalarRegisterClass<(add eEle, eElo)>;
def mElm : AIE2PSScalarRegisterClass<(add eEle, eElo)>;
def mEla : AIE2PSScalarRegisterClass<(add eEle, eElo)>;
def mEs : AIE2PSScalarRegisterClass<(add eEle, eElo, eEhe, eEho)>;
def mEa : AIE2PSScalarRegisterClass<(add eEle, eElo, eEhe, eEho)>;
def mElos : AIE2PSScalarRegisterClass<(add eElo)>;
def mEloa : AIE2PSScalarRegisterClass<(add eElo)>;
def mElom : AIE2PSScalarRegisterClass<(add eElo)>;
def mEhes : AIE2PSScalarRegisterClass<(add eEhe)>;
def mEhem : AIE2PSScalarRegisterClass<(add eEhe)>;
def mEhea : AIE2PSScalarRegisterClass<(add eEhe)>;
def mEhm : AIE2PSScalarRegisterClass<(add eEhe, eEho)>;
def mEha : AIE2PSScalarRegisterClass<(add eEhe, eEho)>;
def mEhs : AIE2PSScalarRegisterClass<(add eEhe, eEho)>;
def mEhom : AIE2PSScalarRegisterClass<(add eEho)>;
def mEhos : AIE2PSScalarRegisterClass<(add eEho)>;
def mEhoa : AIE2PSScalarRegisterClass<(add eEho)>;

def eEEe : AIE2PSScalar64RegisterClass<(add e0, e2, e4, e6, e8, e10)>;
def eEEo : AIE2PSScalar64RegisterClass<(add e1, e3, e5, e7, e9, e11)>;
def mEEem : AIE2PSScalar64RegisterClass<(add eEEe)>;
def mEEea : AIE2PSScalar64RegisterClass<(add eEEe)>;
def mEEes : AIE2PSScalar64RegisterClass<(add eEEe)>;
def mEEs : AIE2PSScalar64RegisterClass<(add eEEe, eEEo)>;
def mEEa : AIE2PSScalar64RegisterClass<(add eEEe, eEEo)>;
def mEEm : AIE2PSScalar64RegisterClass<(add eEEe, eEEo)>;
def mEEoa : AIE2PSScalar64RegisterClass<(add eEEo)>;
def mEEos : AIE2PSScalar64RegisterClass<(add eEEo)>;
def mEEom : AIE2PSScalar64RegisterClass<(add eEEo)>;

// F registers
// 64-bit Mantissa registers
foreach i = 0 - 5 in {
  def fl #!mul(i,2) : AIE2PS3BitReg<i, "fl" #!mul(i,2)>;
  def fl #!add(!mul(i,2), 1) : AIE2PS3BitReg<i, "fl" #!add(!mul(i,2), 1)>;
  def fh #!mul(i,2) : AIE2PS3BitReg<i, "fh" #!mul(i,2)>;
  def fh #!add(!mul(i,2), 1) : AIE2PS3BitReg<i, "fh" #!add(!mul(i,2), 1)>;
}
// 128-bit Mantissa registers
foreach i = 0 - 5 in {
  let SubRegIndices = [sub_f_lo, sub_f_hi], CoveredBySubRegs = 1 in {
    // def f #i : AIE2PS4BitReg<i, "f" #i, [!cast<Register>("fl" #i), !cast<Register>("fh" #i)]>;
    def f #!mul(i,2) : AIE2PS3BitReg<i, "f" #!mul(i,2), [!cast<Register>("fl" #!mul(i,2)), !cast<Register>("fh" #!mul(i,2))]>;
    def f #!add(!mul(i,2), 1) : AIE2PS3BitReg<i, "f" #!add(!mul(i,2), 1), [!cast<Register>("fl" #!add(!mul(i,2), 1)), !cast<Register>("fh" #!add(!mul(i,2), 1))]>;
  }
}

def eFle : AIE2PSScalar64RegisterClass<(add fl0, fl2, fl4, fl6, fl8, fl10)>;
def eFlo : AIE2PSScalar64RegisterClass<(add fl1, fl3, fl5, fl7, fl9, fl11)>;
def eFhe : AIE2PSScalar64RegisterClass<(add fh0, fh2, fh4, fh6, fh8, fh10)>;
def eFho : AIE2PSScalar64RegisterClass<(add fh1, fh3, fh5, fh7, fh9, fh11)>;
def mFles : AIE2PSScalar64RegisterClass<(add eFle)>;
def mFlem : AIE2PSScalar64RegisterClass<(add eFle)>;
def mFlea : AIE2PSScalar64RegisterClass<(add eFle)>;
def mFls : AIE2PSScalar64RegisterClass<(add eFle, eFlo)>;
def mFlm : AIE2PSScalar64RegisterClass<(add eFle, eFlo)>;
def mFla : AIE2PSScalar64RegisterClass<(add eFle, eFlo)>;
def mFa : AIE2PSScalar64RegisterClass<(add eFle, eFlo, eFhe, eFho)>;
def mFm : AIE2PSScalar64RegisterClass<(add eFle, eFlo, eFhe, eFho)>;
def mFs : AIE2PSScalar64RegisterClass<(add eFle, eFlo, eFhe, eFho)>;
def mFlos : AIE2PSScalar64RegisterClass<(add eFlo)>;
def mFlom : AIE2PSScalar64RegisterClass<(add eFlo)>;
def mFloa : AIE2PSScalar64RegisterClass<(add eFlo)>;
def mFhem : AIE2PSScalar64RegisterClass<(add eFhe)>;
def mFhea : AIE2PSScalar64RegisterClass<(add eFhe)>;
def mFhes : AIE2PSScalar64RegisterClass<(add eFhe)>;
def mFha : AIE2PSScalar64RegisterClass<(add eFhe, eFho)>;
def mFhm : AIE2PSScalar64RegisterClass<(add eFhe, eFho)>;
def mFhs : AIE2PSScalar64RegisterClass<(add eFhe, eFho)>;
def mFhoa : AIE2PSScalar64RegisterClass<(add eFho)>;
def mFhom : AIE2PSScalar64RegisterClass<(add eFho)>;
def mFhos : AIE2PSScalar64RegisterClass<(add eFho)>;
def eFFe : AIE2PSVector128RegisterClass<(add f0, f2, f4, f6, f8, f10)>;
def eFFo : AIE2PSVector128RegisterClass<(add f1, f3, f5, f7, f9, f11)>;
def mFFes : AIE2PSVector128RegisterClass<(add eFFe)>;
def mFFea : AIE2PSVector128RegisterClass<(add eFFe)>;
def mFFem : AIE2PSVector128RegisterClass<(add eFFe)>;
def mFFa : AIE2PSVector128RegisterClass<(add eFFe, eFFo)>;
def mFFs : AIE2PSVector128RegisterClass<(add eFFe, eFFo)>;
def mFFm : AIE2PSVector128RegisterClass<(add eFFe, eFFo)>;
def mFFom : AIE2PSVector128RegisterClass<(add eFFo)>;
def mFFos : AIE2PSVector128RegisterClass<(add eFFo)>;
def mFFoa : AIE2PSVector128RegisterClass<(add eFFo)>;

// G registers
// Extra Sub-tile Shift Registers
// 32-bit Extra Sub-tile Shift Registers
foreach i = 0 - 5 in {
  def gl #!mul(i,2) : AIE2PS3BitReg<i, "gl" #!mul(i,2)>;
  def gl #!add(!mul(i,2), 1) : AIE2PS3BitReg<i, "gl" #!add(!mul(i,2), 1)>;
  def gh #!mul(i,2) : AIE2PS3BitReg<i, "gh" #!mul(i,2)>;
  def gh #!add(!mul(i,2), 1) : AIE2PS3BitReg<i, "gh" #!add(!mul(i,2), 1)>;
}
// 64-bit Extra Sub-tile Shift Registers
foreach i = 0 - 5 in {
  let SubRegIndices = [sub_lo_exp, sub_hi_exp], CoveredBySubRegs = 1 in {
    // def g #i : AIE2PS4BitReg<i, "g" #i, [!cast<Register>("gl" #i), !cast<Register>("gh" #i)]>;
    def g #!mul(i,2) : AIE2PS3BitReg<i, "g" #!mul(i,2), [!cast<Register>("gl" #!mul(i,2)), !cast<Register>("gh" #!mul(i,2))]>;
    def g #!add(!mul(i,2), 1) : AIE2PS3BitReg<i, "g" #!add(!mul(i,2), 1), [!cast<Register>("gl" #!add(!mul(i,2), 1)), !cast<Register>("gh" #!add(!mul(i,2), 1))]>;
  }
}

def eGhe : AIE2PSScalarRegisterClass<(add gh0, gh2, gh4, gh6, gh8, gh10)>;
def eGho : AIE2PSScalarRegisterClass<(add gh1, gh3, gh5, gh7, gh9, gh11)>;
def eGle : AIE2PSScalarRegisterClass<(add gl0, gl2, gl4, gl6, gl8, gl10)>;
def eGlo : AIE2PSScalarRegisterClass<(add gl1, gl3, gl5, gl7, gl9, gl11)>;

def mGhea : AIE2PSScalarRegisterClass<(add eGhe)>;
def mGhes : AIE2PSScalarRegisterClass<(add eGhe)>;
def mGhem : AIE2PSScalarRegisterClass<(add eGhe)>;
def mGha : AIE2PSScalarRegisterClass<(add eGhe, eGho)>;
def mGhm : AIE2PSScalarRegisterClass<(add eGhe, eGho)>;
def mGhs : AIE2PSScalarRegisterClass<(add eGhe, eGho)>;
def mGhom : AIE2PSScalarRegisterClass<(add eGho)>;
def mGhos : AIE2PSScalarRegisterClass<(add eGho)>;
def mGhoa : AIE2PSScalarRegisterClass<(add eGho)>;
def mGles : AIE2PSScalarRegisterClass<(add eGle)>;
def mGlea : AIE2PSScalarRegisterClass<(add eGle)>;
def mGlem : AIE2PSScalarRegisterClass<(add eGle)>;
def mGls : AIE2PSScalarRegisterClass<(add eGle, eGlo)>;
def mGlm : AIE2PSScalarRegisterClass<(add eGle, eGlo)>;
def mGla : AIE2PSScalarRegisterClass<(add eGle, eGlo)>;
def mGa : AIE2PSScalarRegisterClass<(add eGle, eGlo, eGhe, eGho)>;
def mGs : AIE2PSScalarRegisterClass<(add eGle, eGlo, eGhe, eGho)>;
def mGlos : AIE2PSScalarRegisterClass<(add eGlo)>;
def mGlom : AIE2PSScalarRegisterClass<(add eGlo)>;
def mGloa : AIE2PSScalarRegisterClass<(add eGlo)>;

def eGGe : AIE2PSScalar64RegisterClass<(add g0, g2, g4, g6, g8, g10)>;
def eGGo : AIE2PSScalar64RegisterClass<(add g1, g3, g5, g7, g9, g11)>;
def mGGem : AIE2PSScalar64RegisterClass<(add eGGe)>;
def mGGes : AIE2PSScalar64RegisterClass<(add eGGe)>;
def mGGea : AIE2PSScalar64RegisterClass<(add eGGe)>;
def mGGs : AIE2PSScalar64RegisterClass<(add eGGe, eGGo)>;
def mGGm : AIE2PSScalar64RegisterClass<(add eGGe, eGGo)>;
def mGGa : AIE2PSScalar64RegisterClass<(add eGGe, eGGo)>;
def mGGos : AIE2PSScalar64RegisterClass<(add eGGo)>;
def mGGoa : AIE2PSScalar64RegisterClass<(add eGGo)>;
def mGGom : AIE2PSScalar64RegisterClass<(add eGGo)>;

// 320-bit Block Floating-Point 16' Vector Registers
// These consist of 32-bit exponent + 32-bit sub-tile shift + 256-bit vector registers
foreach i = 0 - 5 in {
  let SubRegIndices = [sub_bfp_v256, sub_bfp_g32, sub_bfp_e32], CoveredBySubRegs = 1 in    {
    def ewl #!mul(i,2) : AIE2PS3BitReg<i, "ewl" #!mul(i,2), [!cast<Register>("wl" #!mul(i,2)), !cast<Register>("gl" #!mul(i,2)), !cast<Register>("el" #!mul(i,2))]>;
    def ewh #!mul(i,2) : AIE2PS3BitReg<i, "ewh" #!mul(i,2), [!cast<Register>("wh" #!mul(i,2)), !cast<Register>("gh" #!mul(i,2)), !cast<Register>("eh" #!mul(i,2))]>;
    def ewl #!add(!mul(i,2),1) : AIE2PS3BitReg<i, "ewl" #!add(!mul(i,2),1), [!cast<Register>("wl" #!add(!mul(i,2),1)), !cast<Register>("gl" #!add(!mul(i,2),1)), !cast<Register>("el" #!add(!mul(i,2),1))]>;
    def ewh #!add(!mul(i,2),1) : AIE2PS3BitReg<i, "ewh" #!add(!mul(i,2),1), [!cast<Register>("wh" #!add(!mul(i,2),1)), !cast<Register>("gh" #!add(!mul(i,2),1)), !cast<Register>("eh" #!add(!mul(i,2),1))]>;
  }
}

// 640-bit Block Floating-Point 16' Vector Registers
// These consist of two 320 bit Block Floating-Point 16' Vector Registers 
foreach i = 0 - 5 in {
  let SubRegIndices = [sub_bfp320_lo, sub_bfp320_hi], CoveredBySubRegs = 1 in {
    def ex #!mul(i,2) : AIE2PS3BitReg<i, "ex" #!mul(i,2), [!cast<Register>("ewl" #!mul(i,2)), !cast<Register>("ewh" #!mul(i,2))]>;
    def ex #!add(!mul(i,2),1) : AIE2PS3BitReg<i, "ex" #!add(!mul(i,2),1), [!cast<Register>("ewl" #!add(!mul(i,2),1)), !cast<Register>("ewh" #!add(!mul(i,2),1))]>;
  }
}

class AIE2PSVector320RegisterClass<dag reglist, RegAltNameIndex idx = NoRegAltName>
    : AIE2PSRegisterClass<320, 256, [i32], reglist, idx>;
class AIE2PSVector640RegisterClass<dag reglist, RegAltNameIndex idx = NoRegAltName>
    : AIE2PSRegisterClass<640, 512, [i32], reglist, idx>;

def eEWhe : AIE2PSVector320RegisterClass<(add ewh0, ewh2, ewh4, ewh6, ewh8, ewh10)>;
def eEWho : AIE2PSVector320RegisterClass<(add ewh1, ewh3, ewh5, ewh7, ewh9, ewh11)>;
def eEWle : AIE2PSVector320RegisterClass<(add ewl0, ewl2, ewl4, ewl6, ewl8, ewl10)>;
def eEWlo : AIE2PSVector320RegisterClass<(add ewl1, ewl3, ewl5, ewl7, ewl9, ewl11)>;
def mEWhem : AIE2PSVector320RegisterClass<(add eEWhe)>;
def mEWhev : AIE2PSVector320RegisterClass<(add eEWhe)>;
def mEWhv : AIE2PSVector320RegisterClass<(add eEWhe, eEWho)>;
def mEWhm : AIE2PSVector320RegisterClass<(add eEWhe, eEWho)>;
def mEWhov : AIE2PSVector320RegisterClass<(add eEWho)>;
def mEWhom : AIE2PSVector320RegisterClass<(add eEWho)>;
def mEWlev_bis : AIE2PSVector320RegisterClass<(add eEWle)>;
def mEWlem : AIE2PSVector320RegisterClass<(add eEWle)>;
def mEWlv_bis : AIE2PSVector320RegisterClass<(add eEWle, eEWlo)>;
def mEWlm : AIE2PSVector320RegisterClass<(add eEWle, eEWlo)>;
def mEWm : AIE2PSVector320RegisterClass<(add eEWle, eEWlo, eEWhe, eEWho)>;
def mEWlov_bis : AIE2PSVector320RegisterClass<(add eEWlo)>;
def mEWlom : AIE2PSVector320RegisterClass<(add eEWlo)>;

def eEXe_s : AIE2PSVector640RegisterClass<(add ex0, ex2, ex4, ex6, ex8, ex10)>;
def eEXo_s : AIE2PSVector640RegisterClass<(add ex1, ex3, ex5, ex7, ex9, ex11)>;
def mEXew : AIE2PSVector640RegisterClass<(add eEXe_s)>;
def mEXem : AIE2PSVector640RegisterClass<(add eEXe_s)>;
def mEXes : AIE2PSVector640RegisterClass<(add eEXe_s)>;
def mEXen : AIE2PSVector640RegisterClass<(add eEXe_s)>;
def mEXeb : AIE2PSVector640RegisterClass<(add eEXe_s)>;
def mEXea : AIE2PSVector640RegisterClass<(add eEXe_s)>;
def mEXs : AIE2PSVector640RegisterClass<(add eEXe_s, eEXo_s)>;
def mEXa : AIE2PSVector640RegisterClass<(add eEXe_s, eEXo_s)>;
def mEXb : AIE2PSVector640RegisterClass<(add eEXe_s, eEXo_s)>;
def mEXm : AIE2PSVector640RegisterClass<(add eEXe_s, eEXo_s)>;
def mEXn : AIE2PSVector640RegisterClass<(add eEXe_s, eEXo_s)>;
def mEXw : AIE2PSVector640RegisterClass<(add eEXe_s, eEXo_s)>;
def mEXon : AIE2PSVector640RegisterClass<(add eEXo_s)>;
def mEXom : AIE2PSVector640RegisterClass<(add eEXo_s)>;
def mEXoa : AIE2PSVector640RegisterClass<(add eEXo_s)>;
def mEXow : AIE2PSVector640RegisterClass<(add eEXo_s)>;
def mEXos : AIE2PSVector640RegisterClass<(add eEXo_s)>;
def mEXob : AIE2PSVector640RegisterClass<(add eEXo_s)>;

// 1280-bit Block Floating-Point 16' Vector Registers
// These consist of two 640-bit Block Floating-Point 16' vector-exponent registers
let SubRegIndices = [sub_bfp640_lo, sub_bfp640_hi], CoveredBySubRegs = 1 in {
  foreach i = 0 - 5 in {
    def ey #i : AIE2PS3BitReg<i, "ey" #i, [!cast<Register>("ex" #!mul(i,2)), !cast<Register>("ex" #!add(!mul(i,2),1))]>;
  }
} // let SubRegIndices = [sub_bfp640_lo, sub_bfp640_hi]
class AIE2PSVector1280RegisterClass<dag reglist, RegAltNameIndex idx = NoRegAltName>
    : AIE2PSRegisterClass<1280, 512, [i32], reglist, idx>;

def eEYs : AIE2PSVector1280RegisterClass<(add ey0, ey1, ey2, ey3)>;
def eEY_s : AIE2PSVector1280RegisterClass<(add ey0, ey1, ey2, ey3, ey4, ey5)>;
def mEYsw : AIE2PSVector1280RegisterClass<(add eEYs)>;
def mEYw : AIE2PSVector1280RegisterClass<(add eEY_s)>;

def sub_few_bfp_e32 : SubRegIndex<32,   352>;
def sub_few_bfp_g32 : SubRegIndex<32,   320>;
def sub_few_bfp_f64 : SubRegIndex<64,   256>;
def sub_few_bfp_w : SubRegIndex<256, 0>;

def sub_bfp768_lo : SubRegIndex<768,   0>;
def sub_bfp768_hi : SubRegIndex<768, 768>;

// Subregister indices for 768-bit BFP composed of two 384-bit BFPs
def sub_bfp384_lo : SubRegIndex<384,   0>;
def sub_bfp384_hi : SubRegIndex<384, 384>;

class AIE2PSVector384RegisterClass<dag reglist, RegAltNameIndex idx = NoRegAltName>
    : AIE2PSRegisterClass<384, 256, [i32], reglist, idx>;
class AIE2PSVector768RegisterClass<dag reglist, RegAltNameIndex idx = NoRegAltName>
    : AIE2PSRegisterClass<768, 512, [i32], reglist, idx>;

// 384-bit Block Floating-Point 13' Vector Registers
// These consist of 32-bit exponent (E) + 32-bit sub-tile shift + 64-bit sign (F) + 256-bit vector (X) registers
foreach i = 0 - 5 in {
  let SubRegIndices = [sub_few_bfp_w, sub_few_bfp_f64, sub_few_bfp_g32, sub_few_bfp_e32], CoveredBySubRegs = 1 in    {
    def fewl #!mul(i,2) : AIE2PS3BitReg<i, "fewl" #!mul(i,2), [!cast<Register>("wl" #!mul(i,2)), !cast<Register>("fl" #!mul(i,2)), !cast<Register>("gl" #!mul(i,2)), !cast<Register>("el" #!mul(i,2))]>;
    def fewh #!mul(i,2) : AIE2PS3BitReg<i, "fewh" #!mul(i,2), [!cast<Register>("wh" #!mul(i,2)), !cast<Register>("fh" #!mul(i,2)), !cast<Register>("gh" #!mul(i,2)), !cast<Register>("eh" #!mul(i,2))]>;
    def fewl #!add(!mul(i,2),1) : AIE2PS3BitReg<i, "fewl" #!add(!mul(i,2),1), [!cast<Register>("wl" #!add(!mul(i,2),1)), !cast<Register>("fl" #!add(!mul(i,2),1)), !cast<Register>("gl" #!add(!mul(i,2),1)), !cast<Register>("el" #!add(!mul(i,2),1))]>;
    def fewh #!add(!mul(i,2),1) : AIE2PS3BitReg<i, "fewh" #!add(!mul(i,2),1), [!cast<Register>("wh" #!add(!mul(i,2),1)), !cast<Register>("fh" #!add(!mul(i,2),1)), !cast<Register>("gh" #!add(!mul(i,2),1)), !cast<Register>("eh" #!add(!mul(i,2),1))]>;
  }
}

def eFEWhe : AIE2PSVector384RegisterClass<(add fewh0, fewh2, fewh4, fewh6, fewh8, fewh10)>;
def eFEWho : AIE2PSVector384RegisterClass<(add fewh1, fewh3, fewh5, fewh7, fewh9, fewh11)>;
def eFEWle : AIE2PSVector384RegisterClass<(add fewl0, fewl2, fewl4, fewl6, fewl8, fewl10)>;
def eFEWlo : AIE2PSVector384RegisterClass<(add fewl1, fewl3, fewl5, fewl7, fewl9, fewl11)>;
def mFEWhea : AIE2PSVector384RegisterClass<(add eFEWhe)>;
def mFEWhes : AIE2PSVector384RegisterClass<(add eFEWhe)>;
def mFEWhem : AIE2PSVector384RegisterClass<(add eFEWhe)>;
def mFEWhs : AIE2PSVector384RegisterClass<(add eFEWhe, eFEWho)>;
def mFEWha : AIE2PSVector384RegisterClass<(add eFEWhe, eFEWho)>;
def mFEWhm : AIE2PSVector384RegisterClass<(add eFEWhe, eFEWho)>;
def mFEWhos : AIE2PSVector384RegisterClass<(add eFEWho)>;
def mFEWhoa : AIE2PSVector384RegisterClass<(add eFEWho)>;
def mFEWhom : AIE2PSVector384RegisterClass<(add eFEWho)>;
def mFEWlea : AIE2PSVector384RegisterClass<(add eFEWle)>;
def mFEWles : AIE2PSVector384RegisterClass<(add eFEWle)>;
def mFEWlem : AIE2PSVector384RegisterClass<(add eFEWle)>;
def mFEWls : AIE2PSVector384RegisterClass<(add eFEWle, eFEWlo)>;
def mFEWla : AIE2PSVector384RegisterClass<(add eFEWle, eFEWlo)>;
def mFEWlm : AIE2PSVector384RegisterClass<(add eFEWle, eFEWlo)>;
def mFEWm : AIE2PSVector384RegisterClass<(add eFEWle, eFEWlo, eFEWhe, eFEWho)>;
def mFEWa : AIE2PSVector384RegisterClass<(add eFEWle, eFEWlo, eFEWhe, eFEWho)>;
def mFEWs : AIE2PSVector384RegisterClass<(add eFEWle, eFEWlo, eFEWhe, eFEWho)>;
def mFEWlom : AIE2PSVector384RegisterClass<(add eFEWlo)>;
def mFEWlos : AIE2PSVector384RegisterClass<(add eFEWlo)>;
def mFEWloa : AIE2PSVector384RegisterClass<(add eFEWlo)>;

// 768-bit Block Floating-Point Registers composed of two 384-bit BFP registers
// These consist of two FEW registers (2 x 384-bit): combining fewl (low) and fewh (high)
foreach i = 0 - 5 in {
  let SubRegIndices = [sub_bfp384_lo, sub_bfp384_hi], CoveredBySubRegs = 1 in {
    def fex #!mul(i,2) : AIE2PS3BitReg<i, "fex" #!mul(i,2), [!cast<Register>("fewl" #!mul(i,2)), !cast<Register>("fewh" #!mul(i,2))]>;
    def fex #!add(!mul(i,2),1) : AIE2PS3BitReg<i, "fex" #!add(!mul(i,2),1), [!cast<Register>("fewl" #!add(!mul(i,2),1)), !cast<Register>("fewh" #!add(!mul(i,2),1))]>;
  }
}

def eFEXe : AIE2PSVector768RegisterClass<(add fex0, fex2, fex4, fex6, fex8, fex10)>;
def eFEXo : AIE2PSVector768RegisterClass<(add fex1, fex3, fex5, fex7, fex9, fex11)>;
def mFEXem : AIE2PSVector768RegisterClass<(add eFEXe)>;
def mFEXeb : AIE2PSVector768RegisterClass<(add eFEXe)>;
def mFEXev : AIE2PSVector768RegisterClass<(add eFEXe)>;
def mFEXes : AIE2PSVector768RegisterClass<(add eFEXe)>;
def mFEXea : AIE2PSVector768RegisterClass<(add eFEXe)>;
def mFEXen : AIE2PSVector768RegisterClass<(add eFEXe)>;
def mFEXv : AIE2PSVector768RegisterClass<(add eFEXe, eFEXo)>;
def mFEXa : AIE2PSVector768RegisterClass<(add eFEXe, eFEXo)>;
def mFEXn : AIE2PSVector768RegisterClass<(add eFEXe, eFEXo)>;
def mFEXm : AIE2PSVector768RegisterClass<(add eFEXe, eFEXo)>;
def mFEXb : AIE2PSVector768RegisterClass<(add eFEXe, eFEXo)>;
def mFEXs : AIE2PSVector768RegisterClass<(add eFEXe, eFEXo)>;
def mFEXoa : AIE2PSVector768RegisterClass<(add eFEXo)>;
def mFEXos : AIE2PSVector768RegisterClass<(add eFEXo)>;
def mFEXov : AIE2PSVector768RegisterClass<(add eFEXo)>;
def mFEXom : AIE2PSVector768RegisterClass<(add eFEXo)>;
def mFEXon : AIE2PSVector768RegisterClass<(add eFEXo)>;
def mFEXob : AIE2PSVector768RegisterClass<(add eFEXo)>;

// 1536-bit Block Floating-Point 13' Vector Registers
// These consist of two 768-bit Block Floating-Point 13' vector-exponent registers
let SubRegIndices = [sub_bfp768_lo, sub_bfp768_hi], CoveredBySubRegs = 1 in {
  foreach i = 0 - 5 in {
    def fey #i : AIE2PS3BitReg<i, "fey" #i, [!cast<Register>("fex" #!mul(i,2)), !cast<Register>("fex" #!add(!mul(i,2),1))]>;
  }
} // let SubRegIndices = [sub_bfp768_lo, sub_bfp768_hi]
class AIE2PSVector1536RegisterClass<dag reglist, RegAltNameIndex idx = NoRegAltName>
    : AIE2PSRegisterClass<1536, 512, [i32], reglist, idx>;

def eFEY : AIE2PSVector1536RegisterClass<(add fey0, fey1, fey2, fey3, fey4, fey5)>;
def mFEYw : AIE2PSVector1536RegisterClass<(add eFEY)>;


def sub_eg_e32 : SubRegIndex<32, 32>;
def sub_eg_g32 : SubRegIndex<32, 0>;
def sub_eg_e : SubRegIndex<64, 64>;
def sub_eg_g : SubRegIndex<64, 0>;

// 64-bit g-e registers
// These consist of 32-bit exponent + 32-bit g registers
foreach i = 0 - 5 in {
  let SubRegIndices = [sub_eg_g32, sub_eg_e32], CoveredBySubRegs = 1 in    {
    def egl #!mul(i,2) : AIE2PS3BitReg<i, "egl" #!mul(i,2), [!cast<Register>("gl" #!mul(i,2)), !cast<Register>("el" #!mul(i,2))]>;
    def egh #!mul(i,2) : AIE2PS3BitReg<i, "egh" #!mul(i,2), [!cast<Register>("gh" #!mul(i,2)), !cast<Register>("eh" #!mul(i,2))]>;
    def egl #!add(!mul(i,2),1) : AIE2PS3BitReg<i, "egl" #!add(!mul(i,2),1), [!cast<Register>("gl" #!add(!mul(i,2),1)), !cast<Register>("el" #!add(!mul(i,2),1))]>;
    def egh #!add(!mul(i,2),1) : AIE2PS3BitReg<i, "egh" #!add(!mul(i,2),1), [!cast<Register>("gh" #!add(!mul(i,2),1)), !cast<Register>("eh" #!add(!mul(i,2),1))]>;
  }
}

def eEGhe_s : AIE2PSScalar64RegisterClass<(add egh0, egh2, egh4, egh6, egh8, egh10)>;
def eEGho_s : AIE2PSScalar64RegisterClass<(add egh1, egh3, egh5, egh7, egh9, egh11)>;
def eEGle_s : AIE2PSScalar64RegisterClass<(add egl0, egl2, egl4, egl6, egl8, egl10)>;
def eEGlo_s : AIE2PSScalar64RegisterClass<(add egl1, egl3, egl5, egl7, egl9, egl11)>;
def mEGhes : AIE2PSScalar64RegisterClass<(add eEGhe_s)>;
def mEGhem : AIE2PSScalar64RegisterClass<(add eEGhe_s)>;
def mEGhea : AIE2PSScalar64RegisterClass<(add eEGhe_s)>;
def mEGhm : AIE2PSScalar64RegisterClass<(add eEGhe_s, eEGho_s)>;
def mEGha : AIE2PSScalar64RegisterClass<(add eEGhe_s, eEGho_s)>;
def mEGhs : AIE2PSScalar64RegisterClass<(add eEGhe_s, eEGho_s)>;
def mEGhoa : AIE2PSScalar64RegisterClass<(add eEGho_s)>;
def mEGhos : AIE2PSScalar64RegisterClass<(add eEGho_s)>;
def mEGhom : AIE2PSScalar64RegisterClass<(add eEGho_s)>;
def mEGlem : AIE2PSScalar64RegisterClass<(add eEGle_s)>;
def mEGlea : AIE2PSScalar64RegisterClass<(add eEGle_s)>;
def mEGles : AIE2PSScalar64RegisterClass<(add eEGle_s)>;
def mEGlm : AIE2PSScalar64RegisterClass<(add eEGle_s, eEGlo_s)>;
def mEGls : AIE2PSScalar64RegisterClass<(add eEGle_s, eEGlo_s)>;
def mEGla : AIE2PSScalar64RegisterClass<(add eEGle_s, eEGlo_s)>;
def mEGm : AIE2PSScalar64RegisterClass<(add eEGle_s, eEGlo_s, eEGhe_s, eEGho_s)>;
def mEGs : AIE2PSScalar64RegisterClass<(add eEGle_s, eEGlo_s, eEGhe_s, eEGho_s)>;
def mEGa : AIE2PSScalar64RegisterClass<(add eEGle_s, eEGlo_s, eEGhe_s, eEGho_s)>;
def mEGlom : AIE2PSScalar64RegisterClass<(add eEGlo_s)>;
def mEGlos : AIE2PSScalar64RegisterClass<(add eEGlo_s)>;
def mEGloa : AIE2PSScalar64RegisterClass<(add eEGlo_s)>;

// 128-bit g-e registers
foreach i = 0 - 5 in {
  let SubRegIndices = [sub_eg_g, sub_eg_e], CoveredBySubRegs = 1 in {
    def eg #!mul(i,2) : AIE2PS3BitReg<i, "eg" #!mul(i,2), [!cast<Register>("g" #!mul(i,2)), !cast<Register>("e" #!mul(i,2))]>;
    def eg #!add(!mul(i,2),1) : AIE2PS3BitReg<i, "eg" #!add(!mul(i,2),1), [!cast<Register>("g" #!add(!mul(i,2),1)), !cast<Register>("e" #!add(!mul(i,2),1))]>;
  }
}
def eEG2e_s : AIE2PSVector128RegisterClass<(add eg0, eg2, eg4, eg6, eg8, eg10)>;
def eEG2o_s : AIE2PSVector128RegisterClass<(add eg1, eg3, eg5, eg7, eg9, eg11)>;
def mEG2em : AIE2PSVector128RegisterClass<(add eEG2e_s)>;
def mEG2es : AIE2PSVector128RegisterClass<(add eEG2e_s)>;
def mEG2ea : AIE2PSVector128RegisterClass<(add eEG2e_s)>;
def mEG2a : AIE2PSVector128RegisterClass<(add eEG2e_s, eEG2o_s)>;
def mEG2s : AIE2PSVector128RegisterClass<(add eEG2e_s, eEG2o_s)>;
def mEG2m : AIE2PSVector128RegisterClass<(add eEG2e_s, eEG2o_s)>;
def mEG2oa : AIE2PSVector128RegisterClass<(add eEG2o_s)>;
def mEG2os : AIE2PSVector128RegisterClass<(add eEG2o_s)>;
def mEG2om : AIE2PSVector128RegisterClass<(add eEG2o_s)>;

def sub_feg_f64 : SubRegIndex<64, 64>;
def sub_feg_e32 : SubRegIndex<32, 32>;
def sub_feg_g32 : SubRegIndex<32, 0>;

def sub_feg_f : SubRegIndex<128, 128>;
def sub_feg_e : SubRegIndex<64, 64>;
def sub_feg_g : SubRegIndex<64, 0>;

// 128-bit g-e-f registers
foreach i = 0 - 5 in {
  let SubRegIndices = [sub_feg_g32, sub_feg_e32, sub_feg_f64], CoveredBySubRegs = 1 in    {
    def fegl #!mul(i,2) : AIE2PS3BitReg<i, "fegl" #!mul(i,2), [!cast<Register>("gl" #!mul(i,2)), !cast<Register>("el" #!mul(i,2)), !cast<Register>("fl" #!mul(i,2))]>;
    def fegh #!mul(i,2) : AIE2PS3BitReg<i, "fegh" #!mul(i,2), [!cast<Register>("gh" #!mul(i,2)), !cast<Register>("eh" #!mul(i,2)), !cast<Register>("fh" #!mul(i,2))]>;
    def fegl #!add(!mul(i,2),1) : AIE2PS3BitReg<i, "fegl" #!add(!mul(i,2),1), [!cast<Register>("gl" #!add(!mul(i,2),1)), !cast<Register>("el" #!add(!mul(i,2),1)), !cast<Register>("fl" #!add(!mul(i,2),1))]>;
    def fegh #!add(!mul(i,2),1) : AIE2PS3BitReg<i, "fegh" #!add(!mul(i,2),1), [!cast<Register>("gh" #!add(!mul(i,2),1)), !cast<Register>("eh" #!add(!mul(i,2),1)), !cast<Register>("fh" #!add(!mul(i,2),1))]>;
  }
}

def eFEGhe : AIE2PSVector128RegisterClass<(add fegh0, fegh2, fegh4, fegh6, fegh8, fegh10)>;
def eFEGho : AIE2PSVector128RegisterClass<(add fegh1, fegh3, fegh5, fegh7, fegh9, fegh11)>;
def eFEGle : AIE2PSVector128RegisterClass<(add fegl0, fegl2, fegl4, fegl6, fegl8, fegl10)>;
def eFEGlo : AIE2PSVector128RegisterClass<(add fegl1, fegl3, fegl5, fegl7, fegl9, fegl11)>;
def mFEGhea : AIE2PSVector128RegisterClass<(add eFEGhe)>;
def mFEGhes : AIE2PSVector128RegisterClass<(add eFEGhe)>;
def mFEGhs : AIE2PSVector128RegisterClass<(add eFEGhe, eFEGho)>;
def mFEGha : AIE2PSVector128RegisterClass<(add eFEGhe, eFEGho)>;
def mFEGhos : AIE2PSVector128RegisterClass<(add eFEGho)>;
def mFEGhoa : AIE2PSVector128RegisterClass<(add eFEGho)>;
def mFEGles : AIE2PSVector128RegisterClass<(add eFEGle)>;
def mFEGlea : AIE2PSVector128RegisterClass<(add eFEGle)>;
def mFEGla : AIE2PSVector128RegisterClass<(add eFEGle, eFEGlo)>;
def mFEGls : AIE2PSVector128RegisterClass<(add eFEGle, eFEGlo)>;
def mFEGa : AIE2PSVector128RegisterClass<(add eFEGle, eFEGlo, eFEGhe, eFEGho)>;
def mFEGs : AIE2PSVector128RegisterClass<(add eFEGle, eFEGlo, eFEGhe, eFEGho)>;
def mFEGlos : AIE2PSVector128RegisterClass<(add eFEGlo)>;
def mFEGloa : AIE2PSVector128RegisterClass<(add eFEGlo)>;

// 256-bit g-e-f registers
foreach i = 0 - 5 in {
  let SubRegIndices = [sub_feg_g, sub_feg_e, sub_feg_f], CoveredBySubRegs = 1 in {
    def feg #!mul(i,2) : AIE2PS3BitReg<i, "feg" #!mul(i,2), [!cast<Register>("g" #!mul(i,2)), !cast<Register>("e" #!mul(i,2)), !cast<Register>("f" #!mul(i,2))]>;
    def feg #!add(!mul(i,2),1) : AIE2PS3BitReg<i, "feg" #!add(!mul(i,2),1), [!cast<Register>("g" #!add(!mul(i,2),1)), !cast<Register>("e" #!add(!mul(i,2),1)), !cast<Register>("f" #!add(!mul(i,2),1))]>;
  }
}

def eFEG2e : AIE2PSVector256RegisterClass<(add feg0, feg2, feg4, feg6, feg8, feg10)>;
def eFEG2o : AIE2PSVector256RegisterClass<(add feg1, feg3, feg5, feg7, feg9, feg11)>;
def mFEG2es : AIE2PSVector256RegisterClass<(add eFEG2e)>;
def mFEG2ea : AIE2PSVector256RegisterClass<(add eFEG2e)>;
def mFEG2s : AIE2PSVector256RegisterClass<(add eFEG2e, eFEG2o)>;
def mFEG2a : AIE2PSVector256RegisterClass<(add eFEG2e, eFEG2o)>;
def mFEG2os : AIE2PSVector256RegisterClass<(add eFEG2o)>;
def mFEG2oa : AIE2PSVector256RegisterClass<(add eFEG2o)>;

// Special registers & Control registers
def upsSign0 : AIE2PS1BitReg <0, "upssign0">;
def upsSign1 : AIE2PS1BitReg <1, "upssign1">;
def vaddSign0 : AIE2PS1BitReg <0, "vaddsign0">;
def vaddSign1 : AIE2PS1BitReg <1, "vaddsign1">;
def packSign0 : AIE2PS1BitReg <0, "packsign0">;
def packSign1 : AIE2PS1BitReg <1, "packsign1">;
def srsSign0 : AIE2PS1BitReg <0, "srssign0">;
def srsSign1 : AIE2PS1BitReg <1, "srssign1">;
def unpackSign0 : AIE2PS1BitReg <0, "unpacksign0">;
def unpackSign1 : AIE2PS1BitReg <1, "unpacksign1">;

foreach RegName = [
  "lc",
  "lci",
  "le",
  "lr",
  "ls",
  "pc",
  "sp",
  "CORE_ID",
  "crBF8conf",
  "crF2BMask",
  "crF2FMask",
  "crF2IMask",
  "crFP8conf",
  "crFPCnvFl2FxMask",
  "crFPCnvFx2FlMask",
  "crFPConvSat",
  "crFPMask",
  "crFPNlfMask",
  "crMCDEn",
  "crPackSize",
  "crRnd",
  "crSCDEn",
  "crSRSMode",
  "crSat",
  "crUPSMode",
  "crUnpackSize",
  "srCarry",
  "srF2BFlags",
  "srF2FFlags",
  "srF2IFlags",
  "srFPCnvFl2Fx",
  "srFPCnvFx2Fl",
  "srFPFlags",
  "srFPNlf",
  "srFifo_of",
  "srFifo_uf",
  "srMS0",
  "srSRS_of",
  "srSS0",
  "srSparse_of",
  "srUPS_of"
] in {
  def RegName : Register <!tolower(RegName)>;
}

def rel2abs : AIE2PS20BitRegisterClass<(add pc)>;
def mLRm : AIE2PS20BitRegisterClass<(add lr)>;
def mLRs : AIE2PS20BitRegisterClass<(add lr)>;
def mLRx : AIE2PS20BitRegisterClass<(add lr)>;
def mLRa : AIE2PS20BitRegisterClass<(add lr)>;
def mLSm : AIE2PS20BitRegisterClass<(add ls)>;
def mSPm : AIE2PS20BitRegisterClass<(add sp)>;
def mSPa : AIE2PS20BitRegisterClass<(add sp)>;
def mSPs : AIE2PS20BitRegisterClass<(add sp)>;
def mCoreID : AIE2PS20BitRegisterClass<(add CORE_ID)>;

def mLEm : AIE2PS20BitRegisterClass<(add le)>;

def mLCaCg : AIE2PSScalarRegisterClass<(add lc)>;
def mLCm : AIE2PSScalarRegisterClass<(add lc)>;
def mLCmCg : AIE2PSScalarRegisterClass<(add lc)>;
def mLCxCg : AIE2PSScalarRegisterClass<(add lc)>;

def eUnpackSign : AIE2PSScalarRegisterClass<(add unpackSign0, unpackSign1)>;
def eUPSSign : AIE2PSScalarRegisterClass<(add upsSign0, upsSign1)>;
def eVaddSign : AIE2PSScalarRegisterClass<(add vaddSign0, vaddSign1)>;
def eSRSSign : AIE2PSScalarRegisterClass<(add srsSign0, srsSign1)>;
def ePackSign : AIE2PSScalarRegisterClass<(add packSign0, packSign1)>;
def mCRBF8conf : AIE2PSScalarRegisterClass<(add crBF8conf)>;
def mCRF2BMask : AIE2PSScalarRegisterClass<(add crF2BMask)>;
def mCRF2FMask : AIE2PSScalarRegisterClass<(add crF2FMask)>;
def mCRF2IMask : AIE2PSScalarRegisterClass<(add crF2IMask)>;
def mCRFP8conf : AIE2PSScalarRegisterClass<(add crFP8conf)>;
def mCRFPCnvFl2FxMask : AIE2PSScalarRegisterClass<(add crFPCnvFl2FxMask)>;
def mCRFPCnvFx2FlMask : AIE2PSScalarRegisterClass<(add crFPCnvFx2FlMask)>;
def mCRFPConvSat : AIE2PSScalarRegisterClass<(add crFPConvSat)>;
def mCRFPMask : AIE2PSScalarRegisterClass<(add crFPMask)>;
def mCRFPNlfMask : AIE2PSScalarRegisterClass<(add crFPNlfMask)>;
def mCRMCDEn : AIE2PSScalarRegisterClass<(add crMCDEn)>;
def mCRPackSize : AIE2PSScalarRegisterClass<(add crPackSize)>;
def mCRRnd : AIE2PSScalarRegisterClass<(add crRnd)>;
def mCRSCDEn : AIE2PSScalarRegisterClass<(add crSCDEn)>;
def mCRSRSMode : AIE2PSScalarRegisterClass<(add crSRSMode)>;
def mCRSat : AIE2PSScalarRegisterClass<(add crSat)>;
def mCRUPSMode : AIE2PSScalarRegisterClass<(add crUPSMode)>;
def mCRUnpackSize : AIE2PSScalarRegisterClass<(add crUnpackSize)>;
def mCRPackSign_m : AIE2PSScalarRegisterClass<(add ePackSign)>;
def mCRPackSign_d : AIE2PSScalarRegisterClass<(add ePackSign)>;
def mCRSRSSign_m : AIE2PSScalarRegisterClass<(add eSRSSign)>;
def mCRSRSSign_d : AIE2PSScalarRegisterClass<(add eSRSSign)>;
def mCRm_file : AIE2PSScalarRegisterClass<(add eSRSSign, eUPSSign, ePackSign, eUnpackSign, eVaddSign)>;
def mCRx_file : AIE2PSScalarRegisterClass<(add eSRSSign, eUPSSign, ePackSign, eUnpackSign, eVaddSign)>;
def mCRUPSSign_m : AIE2PSScalarRegisterClass<(add eUPSSign)>;
def mCRUPSSign_d : AIE2PSScalarRegisterClass<(add eUPSSign)>;
def mCRUnpackSign_d : AIE2PSScalarRegisterClass<(add eUnpackSign)>;
def mCRUnpackSign_m : AIE2PSScalarRegisterClass<(add eUnpackSign)>;
def mCRVaddSign_d : AIE2PSScalarRegisterClass<(add eVaddSign)>;
def mCRVaddSign_m : AIE2PSScalarRegisterClass<(add eVaddSign)>;
def mSRCarry : AIE2PSScalarRegisterClass<(add srCarry)>;
def mSRF2BFlags : AIE2PSScalarRegisterClass<(add srF2BFlags)>;
def mSRF2FFlags : AIE2PSScalarRegisterClass<(add srF2FFlags)>;
def mSRF2IFlags : AIE2PSScalarRegisterClass<(add srF2IFlags)>;
def mSRFPCnvFl2Fx : AIE2PSScalarRegisterClass<(add srFPCnvFl2Fx)>;
def mSRFPCnvFx2Fl : AIE2PSScalarRegisterClass<(add srFPCnvFx2Fl)>;
def mSRFPFlags : AIE2PSScalarRegisterClass<(add srFPFlags)>;
def mSRFPNlf : AIE2PSScalarRegisterClass<(add srFPNlf)>;
def mSRFifo_of : AIE2PSScalarRegisterClass<(add srFifo_of)>;
def mSRFifo_uf : AIE2PSScalarRegisterClass<(add srFifo_uf)>;
def mSRMS0 : AIE2PSScalarRegisterClass<(add srMS0)>;
def mSRSRS_of : AIE2PSScalarRegisterClass<(add srSRS_of)>;
def mSRSS0 : AIE2PSScalarRegisterClass<(add srSS0)>;
def mSRSparse_of : AIE2PSScalarRegisterClass<(add srSparse_of)>;
def mSRUPS_of : AIE2PSScalarRegisterClass<(add srUPS_of)>;
def mSCD_cr : AIE2PSScalarRegisterClass<(add crSCDEn)>;
def mCR_single : AIE2PSScalarRegisterClass<(add mCRSat, mCRRnd, mCRFPMask, mCRF2IMask, mCRF2FMask, mCRF2BMask, mCRSRSMode, mCRUPSMode, mCRUnpackSize, mCRPackSize, mCRBF8conf, mCRFP8conf, mCRFPConvSat, mCRSCDEn, mCRMCDEn, mCRFPNlfMask, mCRFPCnvFx2FlMask, mCRFPCnvFl2FxMask)>;
def mCRx : AIE2PSScalarRegisterClass<(add mCRSat, mCRRnd, mCRFPMask, mCRF2IMask, mCRF2FMask, mCRF2BMask, mCRSRSMode, mCRUPSMode, mCRUnpackSize, mCRPackSize, mCRBF8conf, mCRFP8conf, mCRFPConvSat, mCRSCDEn, mCRMCDEn, mCRFPNlfMask, mCRFPCnvFx2FlMask, mCRFPCnvFl2FxMask, eSRSSign, eUPSSign, ePackSign, eUnpackSign, eVaddSign)>;
def mSCm : AIE2PSScalarRegisterClass<(add mCRSat, mCRRnd, mCRFPMask, mCRF2IMask, mCRF2FMask, mCRF2BMask, mCRSRSMode, mCRUPSMode, mCRUnpackSize, mCRPackSize, mCRBF8conf, mCRFP8conf, mCRFPConvSat, mCRSCDEn, mCRMCDEn, mCRFPNlfMask, mCRFPCnvFx2FlMask, mCRFPCnvFl2FxMask, mSRCarry, mSRSS0, mSRMS0, mSRSRS_of, mSRUPS_of, mSRFifo_of, mSRFifo_uf, mSRSparse_of, mSRFPFlags, mSRF2IFlags, mSRF2FFlags, mSRF2BFlags, mSRFPNlf, mSRFPCnvFx2Fl, mSRFPCnvFl2Fx)>;
def mSRm : AIE2PSScalarRegisterClass<(add mSRCarry, mSRSS0, mSRMS0, mSRSRS_of, mSRUPS_of, mSRFifo_of, mSRFifo_uf, mSRSparse_of, mSRFPFlags, mSRF2IFlags, mSRF2FFlags, mSRF2BFlags, mSRFPNlf, mSRFPCnvFx2Fl, mSRFPCnvFl2Fx)>;

def mlSts : AIE2PSScalar64RegisterClass<(add eFle, eFlo, eFhe, eFho, eEEe, eEEo, eGGe, eGGo, eEGle_s, eEGlo_s, eEGhe_s, eEGho_s, eL)>;
def mAguSrc : AIE2PSScalarRegisterClass<(add eP, eM, eDN, eDJ, eDC, eRe, eRo)>;
def mMvSclSrc : AIE2PSScalarRegisterClass<(add eRe, eRo, eDN, eDJ, eDC, eM, eP, eS, mLRm, mSPm, eSRSSign, eUPSSign, ePackSign, eUnpackSign, eVaddSign, mLCm, mLEm, mLSm, mCoreID)>;
def mMvSclDst : AIE2PSScalarRegisterClass<(add eRe, eRo, eDN, eDJ, eDC, eM, eP, eS, mLRm, mSPm, eSRSSign, eUPSSign, ePackSign, eUnpackSign, eVaddSign, mLCm)>;
def mMvSclDstCg : AIE2PSScalarRegisterClass<(add eRe, eRo, eDN, eDJ, eDC, eM, eP, eS, mLRm, mSPm, eSRSSign, eUPSSign, ePackSign, eUnpackSign, eVaddSign, mLCmCg)>;
def mLdaScl : AIE2PSScalarRegisterClass<(add eRe, eRo, eDN, eDJ, eDC, eM, eP, eEle, eElo, eEhe, eEho, mLRa)>;
def mSclSt : AIE2PSScalarRegisterClass<(add eRe, eRo, eDN, eDJ, eDC, eM, eP, eEle, eElo, eEhe, eEho, mLRs)>;
def mLdaCg : AIE2PSScalarRegisterClass<(add eRe, eRo, eDN, eDJ, eDC, eM, eP, mLCaCg)>;
def mSclMS : AIE2PSScalarRegisterClass<(add eRe, eRo, eDN, eDJ, eDC, eM, eP)>;
def mAluCg : AIE2PSScalarRegisterClass<(add eRe, eRo, mLCxCg)>;
def mLockId_reg : AIE2PSScalarRegisterClass<(add eRe, eRo)>;
def mRoV8s : AIE2PSScalarRegisterClass<(add eRoV8)>;

// TODO: ISA does not have much information about this register, review again
def pe2_ads : AIE2PS20BitReg<0, "pe2_ads">;

def VEC128 : AIE2PSVector128RegisterClass<(add eEG2e_s, eEG2o_s)>;
def VEC256 : AIE2PSVector256RegisterClass<(add eWhe, eWho, eWle, eWlo)>;
def VEC512 : AIE2PSVector512RegisterClass<(add mXm)>;
def VEC1024 : AIE2PSVector1024RegisterClass<(add eY)>;

def ACC512 : AIE2PSVector512RegisterClass<(add eBMLL, eBMLH, eBMHL, eBMHH)>;
def ACC1024 : AIE2PSVector1024RegisterClass<(add eCML, eCMH)>;
def ACC2048 : AIE2PSAcc2048RegisterClass<(add eDM)>;

def FIFO512 : AIE2PSVector512RegisterClass<(add mFifoHLReg)>;
def FIFO1024 : AIE2PSVector1024RegisterClass<(add eLdFifoReg, sf)>;

def EXPVEC64 : AIE2PSRegisterClass<64, 32, [v8i8],
                                   (add e0, e1, e2, e3, e4, e5, e6, e7, e8, e9,
                                       e10, e11)>;

def eSpecial20 : AIE2PS20BitRegisterClass<(add ls, lr, le, sp, pc, CORE_ID)>;
foreach RC = [eP, eM, eDN, eDJ, eDC, eSpecial20] in
    def RC # _as_32Bit : AIE2PSScalarRegisterClass<(add RC)>;

def spill_eS_to_eR : AIE2PSScalarRegisterClass<(add eS, eR)>;
def spill_eP_to_eR : AIE2PSScalarRegisterClass<(add eP, eR)>;
def spill_eM_to_eR : AIE2PSScalarRegisterClass<(add eM, eR, eDN)>;
def spill_eDN_to_eR : AIE2PSScalarRegisterClass<(add eDN, eR)>;
def spill_eDJ_to_eR : AIE2PSScalarRegisterClass<(add eDJ, eR, eDN)>;
def spill_eDC_to_eR : AIE2PSScalarRegisterClass<(add eDC, eR)>;

def spill_vec512_to_composite : AIE2PSVector512RegisterClass<(add mXm, mBMm, lfh0, lfh1, lfl0, lfl1, sfl, sfh, lfe)> {
  let ConsiderInPreRAScheduling = false;
}

class AIE2PVector1076FifoRegisterClass<dag reglist> :
    AIE2PSRegisterClass<1088, 512, [i32], reglist>;
def sub_fifo : SubRegIndex<1024, 0>;
def sub_avail : SubRegIndex<32, 1024>;
def sub_ptr : SubRegIndex<20, 1056>;

let SubRegIndices = [sub_fifo, sub_avail, sub_ptr], CoveredBySubRegs = 1 in {
  def plfr0 : AIE2PS1BitReg<0b0, "plfr0", [lf0, r24, p0]>;
  def plfr1 : AIE2PS1BitReg<0b1, "plfr1", [lf1, r25, p1]>;
}
def ePSRFLdF : AIE2PVector1076FifoRegisterClass<(add plfr0, plfr1)>;

} // End AIE2PS Namespace
