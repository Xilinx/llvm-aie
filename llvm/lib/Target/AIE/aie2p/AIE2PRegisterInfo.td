//===- AIE2PRegisterInfo.td -------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//===----------------------------------------------------------------------===//
//  Declarations that describe the AIE2p Register files
//===----------------------------------------------------------------------===//

include "AIEBaseRegisterInfo.td"

let Namespace = "AIE2P" in {

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

def sub_lo_mask : SubRegIndex<64,  0>;
def sub_hi_mask : SubRegIndex<64, 64>;

def sub_q : SubRegIndex<64,   0>;
def sub_w : SubRegIndex<256, 64>;

def sub_sparse_q : SubRegIndex<128,   512>;
def sub_sparse_x : SubRegIndex<512,   0>;

def sub_even_vecmask_640 : SubRegIndex<640,   0>;
def sub_odd_vecmask_640 : SubRegIndex<640, 640>;

def sub_lo_exp : SubRegIndex<32,  0>;
def sub_hi_exp : SubRegIndex<32, 32>;

// 288-bits bfp subidxes
def sub_bfp_exp : SubRegIndex<32,   0>;
def sub_bfp_vec : SubRegIndex<256, 32>;

def sub_bfp16_e : SubRegIndex<64,   512>;
def sub_bfp16_x : SubRegIndex<512, 0>;

def sub_bfp576_lo : SubRegIndex<576,   0>;
def sub_bfp576_hi : SubRegIndex<576, 576>;

def sub_bfp576_hi_bfp16_e : ComposedSubRegIndex<sub_bfp576_hi, sub_bfp16_e>;
def sub_bfp576_hi_bfp16_x : ComposedSubRegIndex<sub_bfp576_hi, sub_bfp16_x>;

def sub_q_vecmaskexp_352 : SubRegIndex<64,   0>;
def sub_e_vecmaskexp_352 : SubRegIndex<32,   64>;
def sub_w_vecmaskexp_352 : SubRegIndex<256,   96>;

def sub_lo_vecmaskexp_352 : SubRegIndex<352,   0>;
def sub_hi_vecmaskexp_352 : SubRegIndex<352,   352>;

def sub_even_vecmaskexp_704 : SubRegIndex<704,   0>;
def sub_odd_vecmaskexp_704 : SubRegIndex<704, 704>;

class AIE2PVReg<bits<4> Enc, string n, list<Register> subregs = []> : Register<n> {
  let HWEncoding{3-0} = Enc;
  let SubRegs = subregs;
}

class AIE2PGPReg<bits<5> Enc, string n> : Register<n> {
  let HWEncoding{4 - 0} = Enc;
}

class AIE2P20BitReg<bits<3> Enc, string n> : Register<n> {
  let HWEncoding{2 - 0} = Enc;
}

class AIE2P1BitReg<bits<1> Enc, string n, list<Register> subregs = []> : Register<n> {
  let HWEncoding{0 - 0} = Enc;
  let SubRegs = subregs;
}
class AIE2P2BitReg<bits<2> Enc, string n, list<Register> subregs = []> : Register<n> {
  let HWEncoding{1 - 0} = Enc;
  let SubRegs = subregs;
}
class AIE2P3BitReg<bits<3> Enc, string n, list<Register> subregs = []> : Register<n> {
  let HWEncoding{2 - 0} = Enc;
  let SubRegs = subregs;
}

class AIE2PRegisterClass <int size, int align, list<ValueType> regTypes, dag reglist, RegAltNameIndex idx = NoRegAltName> :
               AIEBaseRegisterClass<"AIE2P", regTypes, /*alignment*/size, reglist, idx> {
  dag Regs = reglist;
  let RegInfos = RegInfoByHwMode<
      [DefaultMode],
      [RegInfo</*size*/size, /*spill size*/size, /*spill alignment*/align>]>;
}

  class AIE2PScalarRegisterClass<dag reglist, RegAltNameIndex idx = NoRegAltName>
      : AIE2PRegisterClass<32, 32, [v8i1, v16i1, v32i1, bf16, i20, v2bf16, i32, f32, v4i8, v2i16], reglist,
                          idx>;


  // Scalar registers (GPR)
  foreach i = 0 - 31 in {
    def r #i : AIE2PGPReg<i, "r" #i>;
  }

  def eR : AIE2PScalarRegisterClass<(add r0, r1, r2, r3, r4, r5, r6, r7, r8, r9,
                                    r10, r11, r12, r13, r14, r15, r16, r17, r18,
                                    r19, r20, r21, r22, r23, r24, r25, r26, r27,
                                    r28, r29, r30, r31)>;
  def mRa : AIE2PScalarRegisterClass<(add eR)>;
  def mRm : AIE2PScalarRegisterClass<(add eR)>;
  def mRs : AIE2PScalarRegisterClass<(add eR)>;
  def mRx : AIE2PScalarRegisterClass<(add eR)>;
  def mRy : AIE2PScalarRegisterClass<(add eR)>;
  def mRz : AIE2PScalarRegisterClass<(add eR)>;
  def mRv : AIE2PScalarRegisterClass<(add eR)>;
  def mLockId_reg : AIE2PScalarRegisterClass<(add eR)>;

  // Singleton register classes
  def eR26 : AIE2PScalarRegisterClass<(add r26)>;
  def mR26_fifo_st : AIE2PScalarRegisterClass<(add eR26)>;
  def mR26_lock : AIE2PScalarRegisterClass<(add eR26)>;
  def eR27 : AIE2PScalarRegisterClass<(add r27)>;
  def mR27_select : AIE2PScalarRegisterClass<(add eR27)>;
  def eR28 : AIE2PScalarRegisterClass<(add r28)>;
  def mR28_tlast : AIE2PScalarRegisterClass<(add eR28)>;
  def mMStream_tlast_reg : AIE2PScalarRegisterClass<(add eR28)>;
  def eR29 : AIE2PScalarRegisterClass<(add r29)>;
  def mR29_insert : AIE2PScalarRegisterClass<(add eR29)>;
  def eR31 : AIE2PScalarRegisterClass<(add r31)>;
  def mR31_divs : AIE2PScalarRegisterClass<(add eR31)>;
  def mR31_scd : AIE2PScalarRegisterClass<(add eR31)>;
  def mSCD_r : AIE2PScalarRegisterClass<(add eR31)>;
  def mSCD_r_incr : AIE2PScalarRegisterClass<(add eR31)>;
  def eR30 : AIE2PScalarRegisterClass<(add r30)>;
  def mR30_fifo_step_e1 : AIE2PScalarRegisterClass<(add eR30)>;
  def mR30_fifo_step_e7 : AIE2PScalarRegisterClass<(add eR30)>;
  def mR30_shiftx : AIE2PScalarRegisterClass<(add eR30)>;
  def eR16 : AIE2PScalarRegisterClass<(add r16)>;
  def mR16_vcompare : AIE2PScalarRegisterClass<(add eR16)>;

  def eRS4 : AIE2PScalarRegisterClass<(add r16, r17, r18, r19)>;
  def eRS16
      : AIE2PScalarRegisterClass<(add r16, r17, r18, r19, r20, r21, r22,
                                 r23, r24, r25, r26, r27, r28, r29, r30,
                                 r31)>;
  def mRS16m : AIE2PScalarRegisterClass<(add eRS16)>;

  def eRF2 : AIE2PScalarRegisterClass<(add r24, r25)>;
  def mRF2a : AIE2PScalarRegisterClass<(add eRF2)>;
  def mRF2b : AIE2PScalarRegisterClass<(add eRF2)>;

  // Even-Odd register pairs, starting at R16
  class AIE2PGPRPair<bits<4> Enc, int first, int second> :
      Register<"r" # second # ":r" # first> {
    let HWEncoding{3 - 0} = Enc;
    let SubRegs = [!cast<Register>("r"#first),!cast<Register>("r"#second)];
  }
  foreach i = 0 - 15 in {
    let SubRegIndices = [sub_l_even, sub_l_odd], CoveredBySubRegs = 1 in
      def l # i : AIE2PGPRPair<i, !mul(i,2), !add(!mul(i,2), 1)>;
  }

  class AIE2PScalar64RegisterClass<dag reglist, RegAltNameIndex idx = NoRegAltName>
      : AIE2PRegisterClass<64, 32, [i64, v8i8, v4i16, v4bf16, v2i32, v2f32, v64i1], reglist,
                          idx>;

  def eL : AIE2PScalar64RegisterClass<(add l0, l1, l2, l3, l4, l5, l6, l7,
                              l8, l9, l10, l11, l12, l13, l14, l15)>;
  def mLm : AIE2PScalar64RegisterClass<(add eL)>;
  def mL8m : AIE2PScalar64RegisterClass<(add l8)>;

  class AIE2PVector128RegisterClass<dag reglist>
      : AIE2PRegisterClass<128, 128, [i128, v16i8, v8i16, v8bf16, v4i32, v4f32],
                           reglist>;

  class AIE2PVector256RegisterClass<dag reglist> :
      AIE2PRegisterClass<256, 256, [v32i8, v16i16, v16bf16, v8f32, v8i32], reglist>;

  class AIE2PVector512RegisterClass<dag reglist> :
      AIE2PRegisterClass<512, 512, [v64i8, v32i16, v32bf16, v16i32, v8i64, v16f32], reglist>;

  class AIE2PVector1024RegisterClass<dag reglist> :
      AIE2PRegisterClass<1024, 512, [v128i8, v64i16, v64bf16, v32i32, v16i64, v32f32], reglist>;

  class AIE2PAcc512RegisterClass<dag reglist> :
      AIE2PRegisterClass<512, 512, [v16i32, v8i64, v16f32], reglist>;

  class AIE2PAcc1024RegisterClass<dag reglist> :
      AIE2PRegisterClass<1024, 512, [v32i32, v16i64, v32f32], reglist>;

  class AIE2PAcc2048RegisterClass<dag reglist> :
      AIE2PRegisterClass<2048, 512, [v64i32, v32i64, v64f32], reglist>;

class AIE2P20BitRegisterClass <dag reglist, RegAltNameIndex idx = NoRegAltName> :
                 AIEBaseRegisterClass<"AIE2P", [i20], /*alignment*/ 32, reglist, idx> {
  dag Regs = reglist;
  let RegInfos = RegInfoByHwMode<
      [DefaultMode],
      [RegInfo</*size*/20, /*spill size*/32, /*spill alignment*/32>]>;
  let DecodeZeroBitOperand = true;
}

class AIE2PDim2DRegisterClass <dag reglist, RegAltNameIndex idx = NoRegAltName> :
                 AIEBaseRegisterClass<"AIE2P", [i20], /*alignment*/ 32, reglist, idx> {
  dag Regs = reglist;
  let RegInfos = RegInfoByHwMode<
      [DefaultMode],
      [RegInfo</*size*/80, /*spill size*/128, /*spill alignment*/32>]>;
}

class AIE2PDim3DRegisterClass <dag reglist, RegAltNameIndex idx = NoRegAltName> :
                 AIEBaseRegisterClass<"AIE2P", [i20], /*alignment*/ 32, reglist, idx> {
  dag Regs = reglist;
  let RegInfos = RegInfoByHwMode<
      [DefaultMode],
      [RegInfo</*size*/160, /*spill size*/256, /*spill alignment*/32>]>;
}

// Special registers
  class AIE2PSPLReg<bits<4> Enc, string n> : Register<n> {
    let HWEncoding{2 - 0} = 0b111;
    let HWEncoding{6 - 3} = Enc;
  }

  def pe2_ads : Register<"pe2_ads">;

  // Stack Pointer
  def sp : AIE2PSPLReg<5, "sp">;
  def eSP : AIE2P20BitRegisterClass<(add sp)>;
  def mSPa : AIE2P20BitRegisterClass<(add sp)>;
  def mSPb : AIE2P20BitRegisterClass<(add sp)>;
  def mSPs : AIE2P20BitRegisterClass<(add sp)>;
  // Loop start
  def ls : AIE2PSPLReg<9, "ls">;
  // Link register
  def lr : AIE2PSPLReg<1, "lr">;
  def mLRa : AIE2P20BitRegisterClass<(add lr)>;
  def mLRs : AIE2P20BitRegisterClass<(add lr)>;
  def mLRx : AIE2P20BitRegisterClass<(add lr)>;
  // Loop end
  def le : AIE2PSPLReg<14, "le">;
  // Loop count
  def lc : AIE2PSPLReg<6, "lc">;
  // Core ID
  def CORE_ID : AIE2PSPLReg<10, "core_id">;
  def mCoreID : AIE2P20BitRegisterClass<(add CORE_ID)>;
  // Store FIFO register
  def sfh : AIE2P3BitReg<0b110,"sfh">;
  def sfl : AIE2P3BitReg<0b011,"sfl">;
  // Load FIFO register 0
  def lfh0 : AIE2P1BitReg<0b0,"lfh0">;
  def lfl0 : AIE2P1BitReg<0b0,"lfl0">;
  // Load FIFO register 1
  def lfh1 : AIE2P1BitReg<0b1,"lfh1">;
  def lfl1 : AIE2P1BitReg<0b1,"lfl1">;
  // Load FIFO extra register
  def lfe : AIE2P3BitReg<0b010,"lfe">;

  def sub_lo_fifo: SubRegIndex<512,  0>;
  def sub_hi_fifo : SubRegIndex<512, 512>;
  let SubRegIndices = [sub_lo_fifo, sub_hi_fifo], CoveredBySubRegs = 1 in {
    def lf0 : AIE2P1BitReg<0b0, "lf0", [lfl0, lfh0]>;
    def lf1 : AIE2P1BitReg<0b1, "lf1", [lfl1, lfh1]>;
    def sf : AIE2P1BitReg<0b1, "sf", [sfl, sfh]>;
  }

  def eLdFifoHReg : AIE2PVector512RegisterClass<(add lfh0, lfh1)>;
  def eLdFifoLReg : AIE2PVector512RegisterClass<(add lfl0, lfl1)>;
  def mLdFifo_a : AIE2PVector512RegisterClass<(add lfl0, lfh0, lfl1, lfh1)>;
  def mLdFifo_b : AIE2PVector512RegisterClass<(add mLdFifo_a)>;
  def eLdFifoReg : AIE2PVector1024RegisterClass<(add lf0, lf1)>;

  def mFifoHLReg : AIE2PVector512RegisterClass<(add sfh, sfl, lfh0, lfh1, lfl0, lfl1, lfe)>;
  def mFifoExtra : AIE2PVector512RegisterClass<(add lfe)>;
  def mStFifo :  AIE2PVector1024RegisterClass<(add sf)>;
  def mStFifoh : AIE2PVector512RegisterClass<(add sfh)>;
  def mStFifol : AIE2PVector512RegisterClass<(add sfl)>;

  def eSpecial20 : AIE2P20BitRegisterClass<(add ls, lr, le, sp, CORE_ID)>;

  // Control registers
  class AIE2PControlReg<bits<7> Enc, string n> : Register<n> {
    let HWEncoding{6 - 0} = Enc;
  }
  // The encodings of control and status registers are insignificant now
  // that operand encoder methods are generated as those bits are the fixed
  // and only encoding bit of the singleton register class itself.
  def crSat : AIE2PControlReg<0b0100111, "crsat">;
  def crRnd : AIE2PControlReg<0b1111011, "crrnd">;
  def crFPMask : AIE2PControlReg<0b1101011, "crfpmask">;
  def crF2IMask : AIE2PControlReg<0b0001011, "crf2imask">;
  def crF2FMask : AIE2PControlReg<0b1110011, "crf2fmask">;
  def crF2BMask : AIE2PControlReg<0b0110011, "crf2bmask">;
  def crSRSMode : AIE2PControlReg<0b1000111, "crsrsmode">;
  def crUPSMode : AIE2PControlReg<0b1100111, "crupsmode">;
  def crUnpackSize : AIE2PControlReg<0b0010111, "crunpacksize">;
  def crPackSize : AIE2PControlReg<0b0111011, "crpacksize">;

  def srsSign0 : AIE2P1BitReg<0b0, "srssign0">;
  def srsSign1 : AIE2P1BitReg<0b1, "srssign1">;
  def upsSign0 : AIE2P1BitReg<0b0, "upssign0">;
  def upsSign1 : AIE2P1BitReg<0b1, "upssign1">;
  def packSign0 : AIE2P1BitReg<0b0, "packsign0">;
  def packSign1 : AIE2P1BitReg<0b1, "packsign1">;
  def unpackSign0 : AIE2P1BitReg<0b0, "unpacksign0">;
  def unpackSign1 : AIE2P1BitReg<0b1, "unpacksign1">;
  def vaddSign0 : AIE2P1BitReg<0b0, "vaddsign0">;
  def vaddSign1 : AIE2P1BitReg<0b1, "vaddsign1">;

  // The following 5 classes are only needed for generated encoder methods
  // as the expansion of mode rules there stops at the enums. The classes
  // are not used by instructions as those enum will be expanded resulting in 2
  // instructions for each control register.
  def eVaddSign : AIE2PScalarRegisterClass<(add vaddSign0, vaddSign1)>;
  def eUnpackSign : AIE2PScalarRegisterClass<(add unpackSign0, unpackSign1)>;
  def ePackSign : AIE2PScalarRegisterClass<(add packSign0, packSign1)>;
  def eUPSSign : AIE2PScalarRegisterClass<(add upsSign0, upsSign1)>;
  def eSRSSign : AIE2PScalarRegisterClass<(add srsSign0, srsSign1)>;

  def crSCDEn : AIE2PControlReg<0b0000111, "crscden">;
  def crMCDEn : AIE2PControlReg<0b1011011, "crmcden">;

  def crFPNlfMask : AIE2PControlReg<0b0011011, "crfpnlfmask">;
  def crFPCnvFx2FlMask : AIE2PControlReg<0b0101011, "crfpcnvfx2flmask">;
  def crFPCnvFl2FxMask : AIE2PControlReg<0b1001011, "crfpcnvfl2fxmask">;

  def mCRSat : AIE2PScalarRegisterClass<(add crSat)>;
  def mCRRnd : AIE2PScalarRegisterClass<(add crRnd)>;
  def mCRSRSMode : AIE2PScalarRegisterClass<(add crSRSMode)>;
  def mCRUPSMode : AIE2PScalarRegisterClass<(add crUPSMode)>;
  def mCRUnpackSize : AIE2PScalarRegisterClass<(add crUnpackSize)>;
  def mCRPackSize : AIE2PScalarRegisterClass<(add crPackSize)>;
  def mCRSCDEn : AIE2PScalarRegisterClass<(add crSCDEn)>;
  def mCRMCDEn : AIE2PScalarRegisterClass<(add crMCDEn)>;

  def mCRm : AIE2PRegisterClass<32, 32, [i32],
      (add crSat, crRnd, crFPMask, crF2IMask, crF2FMask, crF2BMask,
       crSRSMode, crUPSMode, crUnpackSize, crPackSize,
       srsSign0, srsSign1, upsSign0, upsSign1, packSign0, packSign1,
       unpackSign0, unpackSign1, vaddSign0, vaddSign1,
       crSCDEn, crMCDEn)>;

  def mCRFP : AIE2PRegisterClass<32, 32, [i32],
      (add crFPNlfMask, crFPCnvFx2FlMask, crFPCnvFl2FxMask)>;

  // Status registers
  class AIE2PStatusReg<bits<4> Enc, string n> : Register<n> {
    let HWEncoding{3 - 0} = Enc;
  }
  def srCarry : AIE2PStatusReg<0b0000, "srcarry">;
  def srSS0 : AIE2PStatusReg<0b1100, "srss0">;
  def srMS0 : AIE2PStatusReg<0b1010, "srms0">;
  def srSRS_of : AIE2PStatusReg<0b1011, "srsrs_of">;
  def srUPS_of : AIE2PStatusReg<0b1110, "srups_of">;
  def srFifo_of : AIE2PStatusReg<0b1000, "srfifo_of">;
  def srFifo_uf : AIE2PStatusReg<0b1001, "srfifo_uf">;
  def srSparse_of : AIE2PStatusReg<0b1101, "srsparse_of">;
  def srFPFlags : AIE2PStatusReg<0b0110, "srfpflags">;
  def srF2IFlags : AIE2PStatusReg<0b0011, "srf2iflags">;
  def srF2FFlags : AIE2PStatusReg<0b0010, "srf2fflags">;
  def srF2BFlags : AIE2PStatusReg<0b0001, "srf2bflags">;
  def srFPNlf : AIE2PStatusReg<0b0111, "srfpnlf">;
  def srFPCnvFx2Fl : AIE2PStatusReg<0b0101, "srfpcnvfx2fl">;
  def srFPCnvFl2Fx : AIE2PStatusReg<0b0100, "srfpcnvfl2fx">;

  def eRCR : AIE2PScalarRegisterClass<(add mCRm, eR)>;

  let HWEncoding = 0 in
    def tile_cntr : Register<"tile_cntr">;

  def mSRCarry : AIE2PScalarRegisterClass<(add srCarry)>;
  def mSRSS0 : AIE2PScalarRegisterClass<(add srSS0)>;
  def mSRMS0 : AIE2PScalarRegisterClass<(add srMS0)>;

  def mSRm : AIE2PRegisterClass<32, 32, [i32],
      (add srCarry, srSS0, srMS0, srSRS_of, srUPS_of,
       srFifo_of, srFifo_uf,
       srSparse_of, srFPFlags, srF2IFlags, srF2FFlags,
       srF2BFlags, srFPNlf, srFPCnvFx2Fl, srFPCnvFl2Fx)>;


  foreach i = 0 - 4 in {
    def bmll #i : AIE2P3BitReg<i, "bmll" #i>;
  }

  foreach i = 0 - 4 in {
    def bmlh #i : AIE2P3BitReg<i, "bmlh" #i>;
  }

  foreach i = 0 - 4 in {
    def bmhl #i : AIE2P3BitReg<i, "bmhl" #i>;
  }

  foreach i = 0 - 4 in {
    def bmhh #i : AIE2P3BitReg<i, "bmhh" #i>;
  }

  def eBMLL : AIE2PAcc512RegisterClass<(add bmll0, bmll1, bmll2, bmll3, bmll4)>;
  def eBMLH : AIE2PAcc512RegisterClass<(add bmlh0, bmlh1, bmlh2, bmlh3, bmlh4)>;
  def eBMHL : AIE2PAcc512RegisterClass<(add bmhl0, bmhl1, bmhl2, bmhl3, bmhl4)>;
  def eBMHH : AIE2PAcc512RegisterClass<(add bmhh0, bmhh1, bmhh2, bmhh3, bmhh4)>;
  def mBMLLm : AIE2PAcc512RegisterClass<(add eBMLL)>;
  def mBMLHm : AIE2PAcc512RegisterClass<(add eBMLH)>;
  def mBMHLm : AIE2PAcc512RegisterClass<(add eBMHL)>;
  def mBMHHm : AIE2PAcc512RegisterClass<(add eBMHH)>;
  def mBMm : AIE2PAcc512RegisterClass<(add mBMLLm, mBMLHm, mBMHLm, mBMHHm)>;
  def mBMs : AIE2PAcc512RegisterClass<(add mBMm)>;
  def mMcdBMSrc : AIE2PAcc512RegisterClass<(add mBMm)>;
  def eBMSLL : AIE2PAcc512RegisterClass<(add bmll0, bmll1, bmll2, bmll3)>;
  def eBMSLH : AIE2PAcc512RegisterClass<(add bmlh0, bmlh1, bmlh2, bmlh3)>;
  def eBMSHL : AIE2PAcc512RegisterClass<(add bmhl0, bmhl1, bmhl2, bmhl3)>;
  def eBMSHH : AIE2PAcc512RegisterClass<(add bmhh0, bmhh1, bmhh2, bmhh3)>;
  def mBMSm : AIE2PAcc512RegisterClass<(add eBMSLL, eBMSLH, eBMSHL, eBMSHH)>;


  let SubRegIndices = [sub_512_acc_lo, sub_512_acc_hi], CoveredBySubRegs = 1 in {
    foreach i = 0 - 4 in {
      def cml #i : AIE2P3BitReg<i, "cml" #i, [!cast<Register>("bmll" #i), !cast<Register>("bmlh" #i)]>;
    }
    foreach i = 0 - 4 in {
      def cmh #i : AIE2P3BitReg<i, "cmh" #i, [!cast<Register>("bmhl" #i), !cast<Register>("bmhh" #i)]>;
    }
  } // let SubRegIndices = [sub_512_acc_lo, sub_512_acc_hi], CoveredBySubRegs = 1

  def eCML : AIE2PAcc1024RegisterClass<(add cml0, cml1, cml2, cml3, cml4)>;
  def eCMH : AIE2PAcc1024RegisterClass<(add cmh0, cmh1, cmh2, cmh3, cmh4)>;
  def mCMLm : AIE2PAcc1024RegisterClass<(add eCML)>;
  def mCMHm : AIE2PAcc1024RegisterClass<(add eCMH)>;
  def mCMLs : AIE2PAcc1024RegisterClass<(add eCML)>;
  def mCMHs : AIE2PAcc1024RegisterClass<(add eCMH)>;  
  def mCMm : AIE2PAcc1024RegisterClass<(add mCMLm, mCMHm)>;
  def mCMs : AIE2PAcc1024RegisterClass<(add mCMm)>;
 
 class AIE2PVRegDM<bits<3> Enc, string n, list<Register> subregs = []> : Register<n> {
    let HWEncoding{2-0} = Enc;
    let SubRegs = subregs;
  }

  let SubRegIndices = [sub_1024_acc_lo, sub_1024_acc_hi], CoveredBySubRegs = 1 in {
    foreach i = 0 - 4 in {
      def dm #i : AIE2PVRegDM<i, "dm" #i, [!cast<Register>("cml" #i), !cast<Register>("cmh" #i)]>;
    }
  } // let SubRegIndices = [sub_1024_acc_lo, sub_1024_acc_hi], CoveredBySubRegs = 1

  def eDM : AIE2PAcc2048RegisterClass<(add dm0, dm1, dm2, dm3, dm4)>;
  def mDMa : AIE2PAcc2048RegisterClass<(add eDM)>;
  def mDMm : AIE2PAcc2048RegisterClass<(add eDM)>;
  def mDMs : AIE2PAcc2048RegisterClass<(add eDM)>;

  // Addressing registers
  foreach i = 0 - 7 in {
    // Pointer registers
    def p #i : AIE2P20BitReg<i, "p" #i>;
    // Modifier registers
    def m #i : AIE2P20BitReg<i, "m" #i>;
    // Dimension count registers
    def dc #i : AIE2P20BitReg<i, "dc" #i>;
    // Dimension stride (jump) registers
    def dj #i : AIE2P20BitReg<i, "dj" #i>;
    // Dimension size registers
    def dn #i : AIE2P20BitReg<i, "dn" #i>;
  }

  def eP : AIE2P20BitRegisterClass<(add p0, p1, p2, p3, p4, p5, p6, p7)>;
  def ePS : AIE2P20BitRegisterClass<(add p0, p1)>;
  def mPfs : AIE2P20BitRegisterClass<(add p2)>;
  def mPfa : AIE2P20BitRegisterClass<(add ePS)>;
  def mPfb : AIE2P20BitRegisterClass<(add ePS)>;
  def mPa : AIE2P20BitRegisterClass<(add eP)>;
  def mPb : AIE2P20BitRegisterClass<(add eP)>;
  def mPs : AIE2P20BitRegisterClass<(add eP)>;
  def mPm : AIE2P20BitRegisterClass<(add eP)>;
  def eDC
      : AIE2P20BitRegisterClass<(add dc0, dc1, dc2, dc3, dc4, dc5, dc6, dc7)>;
  def mDCa : AIE2P20BitRegisterClass<(add eDC)>;
  def mDCb : AIE2P20BitRegisterClass<(add eDC)>;
  def mDCs : AIE2P20BitRegisterClass<(add eDC)>;
  def mDCm : AIE2P20BitRegisterClass<(add eDC)>;
  def eDCH : AIE2P20BitRegisterClass<(add dc4, dc5, dc6, dc7)>;
  def mDCHa : AIE2P20BitRegisterClass<(add eDCH)>;
  def mDCHb : AIE2P20BitRegisterClass<(add eDCH)>;
  def mDCHs : AIE2P20BitRegisterClass<(add eDCH)>;
  def eDCL : AIE2P20BitRegisterClass<(add dc0, dc1, dc2, dc3)>;
  def mDCLa : AIE2P20BitRegisterClass<(add eDCL)>;
  def mDCLb : AIE2P20BitRegisterClass<(add eDCL)>;
  def mDCLs : AIE2P20BitRegisterClass<(add eDCL)>;
  def eDJ
      : AIE2P20BitRegisterClass<(add dj0, dj1, dj2, dj3, dj4, dj5, dj6, dj7)>;
  def mDJa : AIE2P20BitRegisterClass<(add eDJ)>;
  def mDJb : AIE2P20BitRegisterClass<(add eDJ)>;
  def mDJs : AIE2P20BitRegisterClass<(add eDJ)>;
  def mDJm : AIE2P20BitRegisterClass<(add eDJ)>;
  def eDJH : AIE2P20BitRegisterClass<(add dj4, dj5, dj6, dj7)>;
  def mDJHa : AIE2P20BitRegisterClass<(add eDJH)>;
  def mDJHb : AIE2P20BitRegisterClass<(add eDJH)>;
  def mDJHs : AIE2P20BitRegisterClass<(add eDJH)>;
  def eDJL : AIE2P20BitRegisterClass<(add dj0, dj1, dj2, dj3)>;
  def mDJLa : AIE2P20BitRegisterClass<(add eDJL)>;
  def mDJLb : AIE2P20BitRegisterClass<(add eDJL)>;
  def mDJLs : AIE2P20BitRegisterClass<(add eDJL)>;
  def eDN
      : AIE2P20BitRegisterClass<(add dn0, dn1, dn2, dn3, dn4, dn5, dn6, dn7)>;
  def mDNa : AIE2P20BitRegisterClass<(add eDN)>;
  def mDNb : AIE2P20BitRegisterClass<(add eDN)>;
  def mDNs : AIE2P20BitRegisterClass<(add eDN)>;
  def mDNm : AIE2P20BitRegisterClass<(add eDN)>;
  def eDNH : AIE2P20BitRegisterClass<(add dn4, dn5, dn6, dn7)>;
  def mDNHa : AIE2P20BitRegisterClass<(add eDNH)>;
  def mDNHb : AIE2P20BitRegisterClass<(add eDNH)>;
  def mDNHs : AIE2P20BitRegisterClass<(add eDNH)>;
  def eDNL : AIE2P20BitRegisterClass<(add dn0, dn1, dn2, dn3)>;
  def mDNLa : AIE2P20BitRegisterClass<(add eDNL)>;
  def mDNLb : AIE2P20BitRegisterClass<(add eDNL)>;
  def mDNLs : AIE2P20BitRegisterClass<(add eDNL)>;
  def eM : AIE2P20BitRegisterClass<(add m0, m1, m2, m3, m4, m5, m6, m7)>;
  def mMa : AIE2P20BitRegisterClass<(add eM)>;
  def mMb : AIE2P20BitRegisterClass<(add eM)>;
  def mMs : AIE2P20BitRegisterClass<(add eM)>;
  def mMm : AIE2P20BitRegisterClass<(add eM)>;

  class AIE2PVReg0FixedBits3<bits<3> Enc, string n, list<Register> subregs> : Register<n> {
    let HWEncoding{2 - 0} = Enc;
    let SubRegs = subregs;
  }

  let SubRegIndices = [sub_mod, sub_dim_size, sub_dim_stride, sub_dim_count], CoveredBySubRegs = 1 in {
    foreach i = 0 - 7 in {
      def d #i : AIE2P3BitReg<i, "d" #i, [!cast<Register>("m" #i), 
                                                  !cast<Register>("dn" #i),
                                                  !cast<Register>("dj" #i),
                                                  !cast<Register>("dc" #i)]>;
    }
  } // let SubRegIndices = [sub_mod, sub_dim_size, sub_dim_stride, sub_dim_count], CoveredBySubRegs = 1

  def eD : AIE2PDim2DRegisterClass<(add d0, d1, d2, d3, d4, d5, d6, d7)>;
  def mDa : AIE2PDim2DRegisterClass<(add eD)>;
  def mDb : AIE2PDim2DRegisterClass<(add eD)>;
  def mDs : AIE2PDim2DRegisterClass<(add eD)>;

  class AIE2PRegs3D<bits<2> Enc, string n, list<Register> subregs> : Register<n> {
    let HWEncoding{1 - 0} = Enc;
    let SubRegs = subregs;
  }

  let SubRegIndices = [sub_lo_dim, sub_hi_dim], CoveredBySubRegs = 1 in {
    def d0_3d : AIE2PRegs3D<0b00, "d0", [d0, d4]>;
    def d1_3d : AIE2PRegs3D<0b01, "d1", [d1, d5]>;
    def d2_3d : AIE2PRegs3D<0b10, "d2", [d2, d6]>;
    def d3_3d : AIE2PRegs3D<0b11, "d3", [d3, d7]>;
  } // let SubRegIndices = [sub_lo_dim, sub_hi_dim], CoveredBySubRegs = 1

  def eDS : AIE2PDim3DRegisterClass<(add d0_3d, d1_3d, d2_3d, d3_3d)>;
  def mDSa : AIE2PDim3DRegisterClass<(add eDS)>;
  def mDSb : AIE2PDim3DRegisterClass<(add eDS)>;
  def mDSs : AIE2PDim3DRegisterClass<(add eDS)>;

  def mDm : AIE2P20BitRegisterClass<(add eDC, eDJ, eDN, eM)>;

  class AIE2PVReg3FixedBits1000<bits<1> Enc, string n> : Register<n> {
    let HWEncoding{2 - 0} = 0b000;
    let HWEncoding{3} = Enc;
  }

  class AIE2PShiftReg<bits<2> Enc, string n> : Register<n> {
    let HWEncoding{1 - 0} = Enc;
  }

  // Shift registers
  foreach i = 0 - 3 in {
    def s #i : AIE2PShiftReg<i, "s" #i>;
  }
  def eS : AIE2PScalarRegisterClass<(add s0, s1, s2, s3)>;
  def mSs : AIE2PScalarRegisterClass<(add eS)>;
  def mSm : AIE2PScalarRegisterClass<(add eS)>;
  def mSa : AIE2PScalarRegisterClass<(add eS)>;
  def mS2 : AIE2PScalarRegisterClass<(add s2)>;
  def mS3 : AIE2PScalarRegisterClass<(add s3)>;
  def mFx2flt : AIE2PScalarRegisterClass<(add mS2)>;
  def mFlt2fx : AIE2PScalarRegisterClass<(add mS3)>;
  def mOptConv : AIE2PScalarRegisterClass<(add mS2)>;
  def mOptConvDel : AIE2PScalarRegisterClass<(add mS3)>;

  foreach i = 0 - 5 in {
    def wh #!mul(i,2) : AIE2P3BitReg<i, "wh" #!mul(i,2)>;
    def wh #!add(!mul(i,2), 1) : AIE2P3BitReg<i, "wh" #!add(!mul(i,2), 1)>;
    def wl #!mul(i,2) : AIE2P3BitReg<i, "wl" #!mul(i,2)>;
    def wl #!add(!mul(i,2), 1) : AIE2P3BitReg<i, "wl" #!add(!mul(i,2), 1)>;
  }

  def eWhe : AIE2PVector256RegisterClass<(add wh0, wh2, wh4, wh6, wh8, wh10)>;
  def mWhea : AIE2PVector256RegisterClass<(add eWhe)>;
  def mWheb : AIE2PVector256RegisterClass<(add eWhe)>;
  def mWhes : AIE2PVector256RegisterClass<(add eWhe)>;
  def mWhem : AIE2PVector256RegisterClass<(add eWhe)>;

  def eWho : AIE2PVector256RegisterClass<(add wh1, wh3, wh5, wh7, wh9, wh11)>;
  def mWhoa : AIE2PVector256RegisterClass<(add eWho)>;
  def mWhob : AIE2PVector256RegisterClass<(add eWho)>;
  def mWhos : AIE2PVector256RegisterClass<(add eWho)>;
  def mWhom : AIE2PVector256RegisterClass<(add eWho)>;

  def eWle : AIE2PVector256RegisterClass<(add wl0, wl2, wl4, wl6, wl8, wl10)>;
  def mWlea : AIE2PVector256RegisterClass<(add eWle)>;
  def mWleb : AIE2PVector256RegisterClass<(add eWle)>;
  def mWles : AIE2PVector256RegisterClass<(add eWle)>;
  def mWlem : AIE2PVector256RegisterClass<(add eWle)>;

  def eWlo : AIE2PVector256RegisterClass<(add wl1, wl3, wl5, wl7, wl9, wl11)>;
  def mWlom : AIE2PVector256RegisterClass<(add eWlo)>;
  def mWloa : AIE2PVector256RegisterClass<(add eWlo)>;
  def mWlob : AIE2PVector256RegisterClass<(add eWlo)>;
  def mWlos : AIE2PVector256RegisterClass<(add eWlo)>;

  def mWm : AIE2PVector256RegisterClass<(add eWhe, eWho, eWle, eWlo)>;
  def mWa : AIE2PVector256RegisterClass<(add eWhe, eWho, eWle, eWlo)>;
  def mWb : AIE2PVector256RegisterClass<(add eWhe, eWho, eWle, eWlo)>;
  def mWs : AIE2PVector256RegisterClass<(add eWhe, eWho, eWle, eWlo)>;
  def eWL : AIE2PVector256RegisterClass<(add eWle, eWlo)>;
  def eWH : AIE2PVector256RegisterClass<(add eWhe, eWho)>;

  class AIE2PVRegY<bits<3> Enc, string n, list<Register> subregs = []> : Register<n> {
    let HWEncoding{2-0} = Enc;
    let SubRegs = subregs;
  }
  foreach i = 0 - 5 in {
    let SubRegIndices = [sub_256_lo, sub_256_hi], CoveredBySubRegs = 1 in {
      def x #!mul(i,2) : AIE2P3BitReg<i, "x" #!mul(i,2), [!cast<Register>("wl" #!mul(i,2)), !cast<Register>("wh" #!mul(i,2))]>;
      def x #!add(!mul(i,2), 1) : AIE2P3BitReg<i, "x" #!add(!mul(i,2), 1), [!cast<Register>("wl" #!add(!mul(i,2), 1)), !cast<Register>("wh" #!add(!mul(i,2), 1))]>;
    } // let SubRegIndices = [sub_256_lo, sub_256_hi], CoveredBySubRegs = 1
    let SubRegIndices = [sub_512_lo, sub_512_hi], CoveredBySubRegs = 1 in {
      def y #i : AIE2PVRegY<i, "y" #i, [!cast<Register>("x" #!mul(i,2)), !cast<Register>("x" #!add(!mul(i,2),1))]>;
    } // let SubRegIndices = [sub_512_lo, sub_512_hi], CoveredBySubRegs = 1
  }

  def eXe : AIE2PVector512RegisterClass<(add x0, x2, x4, x6, x8, x10)>;
  def eXo : AIE2PVector512RegisterClass<(add x1, x3, x5, x7, x9, x11)>;
  def mXem : AIE2PVector512RegisterClass<(add eXe)>;
  def mXom : AIE2PVector512RegisterClass<(add eXo)>;
  def mXen : AIE2PVector512RegisterClass<(add eXe)>;
  def mXon : AIE2PVector512RegisterClass<(add eXo)>;
  def mXea : AIE2PVector512RegisterClass<(add eXe)>;
  def mXoa : AIE2PVector512RegisterClass<(add eXo)>;

  def mXm : AIE2PVector512RegisterClass<(add eXe, eXo)>;
  def mXn : AIE2PVector512RegisterClass<(add mXm)>;
  def mXv : AIE2PVector512RegisterClass<(add mXm)>;
  def mXw : AIE2PVector512RegisterClass<(add mXm)>;
  def mXs : AIE2PVector512RegisterClass<(add mXm)>;
  def mXa : AIE2PVector512RegisterClass<(add mXm)>;
  def mXb : AIE2PVector512RegisterClass<(add mXm)>;
  def mMcdXSrc : AIE2PVector512RegisterClass<(add mXm)>;

  def eY : AIE2PVector1024RegisterClass<(add y0, y1, y2, y3, y4, y5)>;
  def mYv : AIE2PVector1024RegisterClass<(add eY)>;
  def mYw : AIE2PVector1024RegisterClass<(add eY)>;
  def mYs : AIE2PVector1024RegisterClass<(add eY)>;
  def mYb : AIE2PVector1024RegisterClass<(add eY)>;

  // mask registers
  foreach i = 0 - 3 in {
    // Synthetic 64-bit mask registers
    // Mask registers are actually 128-bit wide, but the 64-bit sub-registers can
    // be grouped together with 256-bit vector registers to form 320-bit
    // vector-mask registers
    def ql #i : AIE2P3BitReg<i, "ql" #i>;
    def qh #i : AIE2P3BitReg<i, "qh" #i>;
    // 128-bit mask registers
    let SubRegIndices = [sub_lo_mask, sub_hi_mask], CoveredBySubRegs = 1 in {
      def q #i : AIE2P3BitReg<i, "q" #i, [!cast<Register>("ql" #i), !cast<Register>("qh" #i)]>;
    }
  }

  def eQQse : AIE2PVector128RegisterClass<(add q0, q2)>;
  def eQQso : AIE2PVector128RegisterClass<(add q1, q3)>;
  def mQQsa : AIE2PVector128RegisterClass<(add eQQse, eQQso)>;
  def mQQss : AIE2PVector128RegisterClass<(add mQQsa)>;
  def mQQsm : AIE2PVector128RegisterClass<(add mQQsa)>;

  // 320-bit vector-mask registers
  // These consist of 64-bit mask + 256-bit vector registers
  foreach i = 0 - 3 in {
    let SubRegIndices = [sub_q, sub_w], CoveredBySubRegs = 1 in {
      def qwl #i : AIE2P3BitReg<i, "qwl" #i, [!cast<Register>("ql" #i), !cast<Register>("wl" #i)]>;
      def qwh #i : AIE2P3BitReg<i, "qwh" #i, [!cast<Register>("qh" #i), !cast<Register>("wh" #i)]>;
    }
  }

  // 640-bit vector-mask registers
  // These consist of two 320-bit vector-mask registers
  foreach i = 0 - 1 in {
    let SubRegIndices = [sub_sparse_x, sub_sparse_q], CoveredBySubRegs = 1 in {
      def qx #!mul(i,2) : AIE2P3BitReg<i, "qx" #!mul(i,2), [!cast<Register>("x" #!mul(i,2)), !cast<Register>("q" #!mul(i,2))]>;
      def qx #!add(!mul(i,2), 1) : AIE2P3BitReg<i, "qx" #!add(!mul(i,2), 1), [!cast<Register>("x" #!add(!mul(i,2), 1)), !cast<Register>("q" #!add(!mul(i,2), 1))]>;
    }
  }


  let SubRegIndices = [sub_even_vecmask_640, sub_odd_vecmask_640], CoveredBySubRegs = 1 in {
    def qy0 : AIE2P1BitReg<0, "qy0", [qx0, qx1]>;
    def qy1 : AIE2P1BitReg<1, "qy1", [qx2, qx3]>;
  }

  // i32 in the list of regTypes is only there to keep TableGen happy as it does not like an empty list of "legal types".
  // This is not an issue for us since it is a SelectionDAG concept.
  class AIE2PVector640RegisterClass<dag reglist, RegAltNameIndex idx = NoRegAltName>
      : AIE2PRegisterClass<640, 512, [i32], reglist, idx>;
  class AIE2PVector1280RegisterClass<dag reglist, RegAltNameIndex idx = NoRegAltName>
      : AIE2PRegisterClass<1280, 512, [i32], reglist, idx>;

  // There are no 320-bit sparse register classes (qwh and qwl) as those are not used by any
  // instructions.
  def eQXse : AIE2PVector640RegisterClass<(add qx0, qx2)>;
  def eQXso : AIE2PVector640RegisterClass<(add qx1, qx3)>;
  def eQXs : AIE2PVector640RegisterClass<(add eQXse, eQXso)>;
  def mQXsea : AIE2PVector640RegisterClass<(add eQXse)>;
  def mQXsoa : AIE2PVector640RegisterClass<(add eQXso)>;
  def mQXsa : AIE2PVector640RegisterClass<(add eQXs)>;
  def mQXsb : AIE2PVector640RegisterClass<(add eQXs)>;
  def mQXsm : AIE2PVector640RegisterClass<(add eQXs)>;
  def mQXss : AIE2PVector640RegisterClass<(add eQXs)>;
  def mQXsw : AIE2PVector640RegisterClass<(add eQXs)>;

  def eQXsea : AIE2PVector640RegisterClass<(add eQXse)>;
  def eQXseb : AIE2PVector640RegisterClass<(add eQXse)>;
  def eQXses : AIE2PVector640RegisterClass<(add eQXse)>;
  def eQXsem : AIE2PVector640RegisterClass<(add eQXse)>;
  def eQXsew : AIE2PVector640RegisterClass<(add eQXse)>;
  def eQXsoa : AIE2PVector640RegisterClass<(add eQXso)>;
  def eQXsob : AIE2PVector640RegisterClass<(add eQXso)>;
  def eQXsos : AIE2PVector640RegisterClass<(add eQXso)>;
  def eQXsom : AIE2PVector640RegisterClass<(add eQXso)>;
  def eQXsow : AIE2PVector640RegisterClass<(add eQXso)>;

  def eQYs : AIE2PVector1280RegisterClass<(add  qy0, qy1)>;
  def mQYsa: AIE2PVector1280RegisterClass<(add eQYs)>;
  def mQYsb: AIE2PVector1280RegisterClass<(add eQYs)>;
  def mQYss: AIE2PVector1280RegisterClass<(add eQYs)>;
  def mQYsm: AIE2PVector1280RegisterClass<(add eQYs)>;
  def mQYsw: AIE2PVector1280RegisterClass<(add eQYs)>;

  def SPARSEVEC640 : AIE2PVector640RegisterClass<(add eQXse, eQXso)>;
  def SPARSEVEC1280 : AIE2PVector1280RegisterClass<(add eQYs)>;


  // Exponent registers
  // 64-bit Exponent registers
  class AIE2PExpReg<bits<4> Enc, string n, list<Register> subregs = []> : Register<n> {
    let HWEncoding{3 - 0} = Enc;
    let SubRegs = subregs;
  }

  foreach i = 0 - 5 in {
    def el #!mul(i,2) : AIE2P3BitReg<i, "el" #!mul(i,2)>;
    def el #!add(!mul(i,2), 1) : AIE2P3BitReg<i, "el" #!add(!mul(i,2), 1)>;
    def eh #!mul(i,2) : AIE2P3BitReg<i, "eh" #!mul(i,2)>;
    def eh #!add(!mul(i,2), 1) : AIE2P3BitReg<i, "eh" #!add(!mul(i,2), 1)>;
  }

  foreach i = 0 - 11 in {
    let SubRegIndices = [sub_lo_exp, sub_hi_exp], CoveredBySubRegs = 1 in {
      def e #i : AIE2PExpReg<i, "e" #i, [!cast<Register>("el" #i), !cast<Register>("eh" #i)]>;
    }
  }
  def eEhe : AIE2PScalarRegisterClass<(add eh0, eh2, eh4, eh6, eh8, eh10)>;
  def eEho : AIE2PScalarRegisterClass<(add eh1, eh3, eh5, eh7, eh9, eh11)>;
  def mEhea : AIE2PScalarRegisterClass<(add eEhe)>;
  def mEhoa : AIE2PScalarRegisterClass<(add eEho)>;
  def eEle : AIE2PScalarRegisterClass<(add el0, el2, el4, el6, el8, el10)>;
  def eElo : AIE2PScalarRegisterClass<(add el1, el3, el5, el7, el9, el11)>;
  def mElm : AIE2PScalarRegisterClass<(add eEle, eElo)>;
  def mEhm : AIE2PScalarRegisterClass<(add eEhe, eEho)>;
  def eE : AIE2PScalarRegisterClass<(add mElm, mEhm)>;
  def mEs : AIE2PScalarRegisterClass<(add eE)>;

  // 64-bit vector-exponent registers
  def EXPVEC64 : AIE2PRegisterClass<64, 32, [v8i8],
                                    (add e0, e1, e2, e3, e4, e5, e6, e7, e8, e9,
                                     e10, e11)>;

  // 288-bit vector-exponent registers
  // These consist of 32-bit exponent + 256-bit vector registers
  foreach i = 0 - 5 in {
    let SubRegIndices = [sub_bfp_exp, sub_bfp_vec], CoveredBySubRegs = 1 in    {
      def ewl #!mul(i,2) : AIE2P3BitReg<i, "ewl" #!mul(i,2), [!cast<Register>("el" #!mul(i,2)), !cast<Register>("wl" #!mul(i,2))]>;
      def ewh #!mul(i,2) : AIE2P3BitReg<i, "ewh" #!mul(i,2), [!cast<Register>("eh" #!mul(i,2)), !cast<Register>("wh" #!mul(i,2))]>;

      def ewl #!add(!mul(i,2),1) : AIE2P3BitReg<i, "ewl" #!add(!mul(i,2),1), [!cast<Register>("el" #!add(!mul(i,2),1)), !cast<Register>("wl" #!add(!mul(i,2),1))]>;
      def ewh #!add(!mul(i,2),1) : AIE2P3BitReg<i, "ewh" #!add(!mul(i,2),1), [!cast<Register>("eh" #!add(!mul(i,2),1)), !cast<Register>("wh" #!add(!mul(i,2),1))]>;

    }
  }

  // 576-bit vector-exponent registers
  // These consist of two 288-bit vector-exponent registers
  foreach i = 0 - 5 in {
    let SubRegIndices = [sub_bfp16_x, sub_bfp16_e], CoveredBySubRegs = 1 in {
      def ex #!mul(i,2) : AIE2P3BitReg<i, "ex" #!mul(i,2), [!cast<Register>("x" #!mul(i,2)), !cast<Register>("e" #!mul(i,2))]>;
      def ex #!add(!mul(i,2),1) : AIE2P3BitReg<i, "ex" #!add(!mul(i,2),1), [!cast<Register>("x" #!add(!mul(i,2),1)), !cast<Register>("e" #!add(!mul(i,2),1))]>;
    }
  }
  class AIE2PVector576RegisterClass<dag reglist, RegAltNameIndex idx = NoRegAltName>
      : AIE2PRegisterClass<576, 512, [i32], reglist, idx>;

  def eEXe : AIE2PVector576RegisterClass<(add ex0, ex2, ex4, ex6, ex8, ex10)>;
  def eEXo : AIE2PVector576RegisterClass<(add ex1, ex3, ex5, ex7, ex9, ex11)>;
  def mEXea : AIE2PVector576RegisterClass<(add eEXe)>;
  def mEXoa :  AIE2PVector576RegisterClass<(add eEXo)>;
  def mEXa : AIE2PVector576RegisterClass<(add eEXe, eEXo)>;
  def mEXb : AIE2PVector576RegisterClass<(add eEXe, eEXo)>;
  def mEXs : AIE2PVector576RegisterClass<(add eEXe, eEXo)>;
  def mEXm : AIE2PVector576RegisterClass<(add eEXe, eEXo)>;
  def mEXn : AIE2PVector576RegisterClass<(add eEXe, eEXo)>;
  def mEXw : AIE2PVector576RegisterClass<(add eEXe, eEXo)>;
  def mEXv : AIE2PVector576RegisterClass<(add eEXe, eEXo)>;

  // 1152-bit vector-exponent registers
  // These consist of two 576-bit vector-exponent registers
  let SubRegIndices = [sub_bfp576_lo, sub_bfp576_hi], CoveredBySubRegs = 1 in {
    foreach i = 0 - 5 in {
      def ey #i : AIE2P3BitReg<i, "ey" #i, [!cast<Register>("ex" #!mul(i,2)), !cast<Register>("ex" #!add(!mul(i,2),1))]>;
    }
  } // let SubRegIndices = [sub_bfp576_lo, sub_bfp576_hi]

  class AIE2PVector1152RegisterClass<dag reglist, RegAltNameIndex idx = NoRegAltName>
      : AIE2PRegisterClass<1152, 512, [i32], reglist, idx>;

  def eEY : AIE2PVector1152RegisterClass<(add ey0, ey1, ey2, ey3, ey4, ey5)>;
  def mEYv : AIE2PVector1152RegisterClass<(add eEY)>;
  def mEYw : AIE2PVector1152RegisterClass<(add eEY)>;

  // 352-bit vector-mask-exponent registers
  // These consist of one 256-bit vector register, a 64-bit mask 
  // register and a 32-bit exponent register.
  class AIE2PVExpMaskReg<bits<2> Enc, string n, list<Register> subregs = []> : Register<n> {
    let HWEncoding{1 - 0} = Enc;
    let SubRegs = subregs;
  }
  foreach i = 0 - 3 in {
    let SubRegIndices = [sub_q_vecmaskexp_352, sub_e_vecmaskexp_352, sub_w_vecmaskexp_352], CoveredBySubRegs = 1 in {
      def qewl #i : AIE2PVExpMaskReg<i, "qewl" #i, [!cast<Register>("ql" #i), !cast<Register>("el" #i), !cast<Register>("wl" #i)]>;
      def qewh #i : AIE2PVExpMaskReg<i, "qewh" #i, [!cast<Register>("qh" #i), !cast<Register>("eh" #i), !cast<Register>("wh" #i)]>;  
    }
  }

  // 704-bit vector-mask-exponent registers
  // These consist of two 352-bit vector-mask-exponent registers
  foreach i = 0 - 1 in {
    let SubRegIndices = [sub_lo_vecmaskexp_352, sub_hi_vecmaskexp_352], CoveredBySubRegs = 1 in {
      def qex #!mul(i,2) : AIE2PVExpMaskReg<i, "qex" #!mul(i,2), [!cast<Register>("qewl" #!mul(i,2)), !cast<Register>("qewh" #!mul(i,2))]>;
      def qex #!add(!mul(i,2), 1) : AIE2PVExpMaskReg<i, "qex" #!add(!mul(i,2), 1), [!cast<Register>("qewl" #!add(!mul(i,2), 1)), !cast<Register>("qewh" #!add(!mul(i,2), 1))]>;
    }
  }

  class AIE2PVector704RegisterClass<dag reglist, RegAltNameIndex idx = NoRegAltName>
      : AIE2PRegisterClass<704, 512, [i32], reglist, idx>;
  def eQEXse : AIE2PVector704RegisterClass<(add qex0, qex2)>;
  def eQEXso : AIE2PVector704RegisterClass<(add qex1, qex3)>;
  def mQEXsw : AIE2PVector704RegisterClass<(add eQEXse, eQEXso)>;
  def mQEXsm : AIE2PVector704RegisterClass<(add eQEXse, eQEXso)>;
  def mQEXsa : AIE2PVector704RegisterClass<(add eQEXse, eQEXso)>;
  def mQEXsb : AIE2PVector704RegisterClass<(add eQEXse, eQEXso)>;
  def mQEXss : AIE2PVector704RegisterClass<(add eQEXse, eQEXso)>;

  // 1408-bit vector-mask-exponent registers
  // These consist of two 704-bit vector-mask-exponent registers
  class AIE2PVExpMaskRegY<bits<1> Enc, string n, list<Register> subregs = []> : Register<n> {
    let HWEncoding{0} = Enc;
    let SubRegs = subregs;
  }
  let SubRegIndices = [sub_even_vecmaskexp_704, sub_odd_vecmaskexp_704], CoveredBySubRegs = 1 in {
    def qey0 : AIE2PVExpMaskRegY<0, "qey0", [qex0, qex1]>;
    def qey1 : AIE2PVExpMaskRegY<1, "qey1", [qex2, qex3]>;
  }

  class AIE2PVector1408RegisterClass<dag reglist, RegAltNameIndex idx = NoRegAltName>
      : AIE2PRegisterClass<1408, 512, [i32], reglist, idx>;

  def eQEYs : AIE2PVector1408RegisterClass<(add qey0, qey1)>;
  def mQEYsw : AIE2PVector1408RegisterClass<(add eQEYs)>;

def VEC128 : AIE2PVector128RegisterClass<(add mQQsm, mQQsa, mQQss)>;
def VEC256 : AIE2PVector256RegisterClass<(add eWhe, eWho, eWle, eWlo)>;
def VEC512 : AIE2PVector512RegisterClass<(add mXm)>;
def VEC1024 : AIE2PVector1024RegisterClass<(add eY)>;
def VEC576 : AIE2PVector576RegisterClass<(add mEXa)>;

def ACC512 : AIE2PAcc512RegisterClass<(add mBMm)>;
def ACC1024 : AIE2PAcc1024RegisterClass<(add mCMm)>;
def ACC2048 : AIE2PAcc2048RegisterClass<(add eDM)>;

def mAluCg : AIE2PScalarRegisterClass<(add eR, lc)>;
def mMvSclDst : AIE2PScalarRegisterClass<(add eR, mDm, eP, eS, le, ls, lr, sp, mCRm, mSRm, mCRFP, lc)>;
def mMvSclDstCg : AIE2PScalarRegisterClass<(add mMvSclDst)>;
def mMvSclSrc : AIE2PScalarRegisterClass<(add mMvSclDst, CORE_ID)>;

def mLdaScl : AIE2PScalarRegisterClass<(add eP, eR, mDm, eE, lr)>;
def mSclSt : AIE2PScalarRegisterClass<(add eP, eR, mDm, mEs, lr)>;
def mLdaCg : AIE2PScalarRegisterClass<(add eP, eR, mDm, lc)>;
def mSclMS : AIE2PScalarRegisterClass<(add eR, mDm, eP)>;
def mAguSrc : AIE2PScalarRegisterClass<(add mSclMS)>;
def mAguDst : AIE2P20BitRegisterClass<(add eP, mDm)>;

def mShflXDst : AIE2PVector512RegisterClass<(add mXm)>;
def mShflBMDst : AIE2PAcc512RegisterClass<(add mBMSm)>;

def mFp2B0 : AIE2PAcc2048RegisterClass<(add eDM)>;
def mFp2B1 : AIE2PAcc2048RegisterClass<(add eDM)>;
def mBp2Bp : AIE2PVector576RegisterClass<(add mEXa)>;
def mWbfsrs : AIE2PAcc512RegisterClass<(add mBMs)>;
def mXbfsrs : AIE2PAcc1024RegisterClass<(add mCMs)>;
def mFl2FxSrc_W : AIE2PVector256RegisterClass<(add mWm)>;

// VMOV register classes
// These classes only encode operand constraints, and will be constrained before
// register allocation using register bank assignments. That's good, because
// these are big classes, and they would crash the TableGen backend when trying
// to generate the register pressure logic. isAllocatable = false simplifies the
// work for the RegisterInfoEmitter backend (mainly, no need to compute register
// unit weights), and solves the problem.
let isAllocatable = false, GeneratePressureSet = false in {
def mMvBMXDst : AIE2PVector512RegisterClass<(add mBMs, mXm, mFifoHLReg)>;
def mMvBMXSrc : AIE2PVector512RegisterClass<(add mMvBMXDst)>;
}

def FIFO512 : AIE2PVector512RegisterClass<(add mFifoHLReg)>;
def FIFO1024 : AIE2PVector1024RegisterClass<(add eLdFifoReg, sf)>;

// There are strange classes, but llvm would define them anyway by performing an
// "AND" on e.g. the mSPa and mMvSclDst register classes.
// Here, we just give them more explicit names.
foreach RC = [eP, eM, eDN, eDJ, eDC, eSP, eSpecial20] in
    def RC # _as_32Bit : AIE2PScalarRegisterClass<(add RC)>;

class AIE2PVector1076FifoRegisterClass<dag reglist> :
    AIE2PRegisterClass<1088, 512, [i32], reglist>;
def sub_ptr : SubRegIndex<20,  0>;
def sub_fifo : SubRegIndex<1024, 20>;
def sub_avail : SubRegIndex<32, 1044>;

let SubRegIndices = [sub_ptr, sub_fifo, sub_avail], CoveredBySubRegs = 1 in {
  def plfr0 : AIE2P1BitReg<0b0, "plfr0", [p0, lf0, r24]>;
  def plfr1 : AIE2P1BitReg<0b1, "plfr1", [p1, lf1, r25]>;
}
def ePSRFLdF : AIE2PVector1076FifoRegisterClass<(add plfr0, plfr1)>;

def spill_eS_to_eR : AIE2PScalarRegisterClass<(add eS, eR)>;
def spill_eP_to_eR : AIE2PScalarRegisterClass<(add eP, eR)>;
def spill_eM_to_eR : AIE2PScalarRegisterClass<(add eM, eR, eDN)>;
def spill_eDN_to_eR : AIE2PScalarRegisterClass<(add eDN, eR)>;
def spill_eDJ_to_eR : AIE2PScalarRegisterClass<(add eDJ, eR, eDN)>;
def spill_eDC_to_eR : AIE2PScalarRegisterClass<(add eDC, eR)>;

def spill_vec512_to_composite : AIE2PVector512RegisterClass<(add mXm, mBMm, lfh0, lfh1, lfl0, lfl1, sfl, sfh, lfe)> {
  let ConsiderInPreRAScheduling = false;
}

def mSRF2IFlags : AIE2PScalarRegisterClass<(add srF2IFlags)>;
def mCRFPNlfMask : AIE2PScalarRegisterClass<(add crFPNlfMask)>;
def mSRFPCnvFl2Fx : AIE2PScalarRegisterClass<(add srFPCnvFl2Fx)>;
def mLEm : AIE2PScalarRegisterClass<(add le)>;
def mLRm : AIE2PScalarRegisterClass<(add lr)>;
def mSRFifo_of : AIE2PScalarRegisterClass<(add srFifo_of)>;
def mCRF2FMask : AIE2PScalarRegisterClass<(add crF2FMask)>;
def mSRF2FFlags : AIE2PScalarRegisterClass<(add srF2FFlags)>;
def mSRF2BFlags : AIE2PScalarRegisterClass<(add srF2BFlags)>;
def mSRFPNlf : AIE2PScalarRegisterClass<(add srFPNlf)>;
def mSRFPCnvFx2Fl : AIE2PScalarRegisterClass<(add srFPCnvFx2Fl)>;
def mSRSparse_of : AIE2PScalarRegisterClass<(add srSparse_of)>;
def mSRUPS_of : AIE2PScalarRegisterClass<(add srUPS_of)>;
def mSRSRS_of : AIE2PScalarRegisterClass<(add srSRS_of)>;
def mSRFPFlags : AIE2PScalarRegisterClass<(add srFPFlags)>;
def mSRFifo_uf : AIE2PScalarRegisterClass<(add srFifo_uf)>;
def mCRVaddSign : AIE2PScalarRegisterClass<(add eVaddSign)>;
def mCRF2IMask : AIE2PScalarRegisterClass<(add crF2IMask)>;
def mCRUnpackSign : AIE2PScalarRegisterClass<(add eUnpackSign)>;
def mCRUPSSign : AIE2PScalarRegisterClass<(add eUPSSign)>;
def mCRFPMask : AIE2PScalarRegisterClass<(add crFPMask)>;
def mCRF2BMask : AIE2PScalarRegisterClass<(add crF2BMask)>;
def mCRSRSSign : AIE2PScalarRegisterClass<(add eSRSSign)>;
def mCRFPCnvFx2FlMask : AIE2PScalarRegisterClass<(add crFPCnvFx2FlMask)>;
def mCRPackSign : AIE2PScalarRegisterClass<(add ePackSign)>;
def mCRFPCnvFl2FxMask : AIE2PScalarRegisterClass<(add crFPCnvFl2FxMask)>;
def mLCm : AIE2PScalarRegisterClass<(add lc)>;
def mLSm : AIE2PScalarRegisterClass<(add ls)>;
def mSPm : AIE2PScalarRegisterClass<(add sp)>;
def mLCaCg : AIE2PScalarRegisterClass<(add lc)>;
def mLCmCg : AIE2PScalarRegisterClass<(add lc)>;


} // End AIE2P Namespace
