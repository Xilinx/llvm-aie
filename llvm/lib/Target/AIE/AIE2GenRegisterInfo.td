//=- AIE2GenRegisterInfo.td - AIEngine V2 Register Definitions ----*- tablegen -*-=//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// AIEngine V2 is an exposed pipeline architecture.  As a result, this file
// must model the processor pipeline with reasonably high fidelity in order
// to enable the generation of correct code.
//
//===----------------------------------------------------------------------===//
//* Automatically generated file, do not edit! *

let Namespace = "AIE2" in {

  class AIE2Reg<string n> : Register<n> {
  } class AIE2GPReg<bits<5> Enc, string n> : Register<n> {
    let HWEncoding{4 - 0} = Enc;
  }

  class AIE220BitReg<bits<3> Enc, string n> : Register<n> {
    let HWEncoding{2 - 0} = Enc;
  }

class AIE2RegisterClass <int size, int align, list<ValueType> regTypes, dag reglist, RegAltNameIndex idx = NoRegAltName> :
               RegisterClass<"AIE2", regTypes, /*alignment*/size, reglist, idx> {
  dag Regs = reglist;
  let RegInfos = RegInfoByHwMode<
      [DefaultMode],
      [RegInfo</*size*/size, /*spill size*/size, /*spill alignment*/align>]>;
  let DecodeZeroBitOperand = true;
}

  class AIE2ScalarRegisterClass<dag reglist, RegAltNameIndex idx = NoRegAltName>
      : AIE2RegisterClass<32, 32, [v8i1, v16i1, bf16, i20, v2bf16, i32, f32, v4i8, v2i16], reglist,
                          idx>;

  class AIE2Vector128RegisterClass<dag reglist> :
      AIE2RegisterClass<128, 128, [v16i8, v8i16, v8bf16, v4i32, v4f32], reglist> {
    let hasCompleteDecoder = 0;
  }

  class AIE2Vector256RegisterClass<dag reglist> :
      AIE2RegisterClass<256, 256, [v32i8, v16i16, v16bf16, v8f32, v8i32], reglist> {
    let hasCompleteDecoder = 0;
  }

  class AIE2Vector512RegisterClass<dag reglist> :
      AIE2RegisterClass<512, 256, [v64i8, v32i16, v32bf16, v16i32, v16f32], reglist> {
    let hasCompleteDecoder = 0;
  }

  class AIE2Vector1024RegisterClass<dag reglist> :

      AIE2RegisterClass<1024, 256, [v128i8, v64i16, v64bf16, v32i32, v32f32], reglist>;

  class AIE2Acc256RegisterClass<dag reglist> :
      AIE2RegisterClass<256, 256, [v4i64], reglist> {
    let hasCompleteDecoder = 0;
  }

  class AIE2Acc512RegisterClass<dag reglist> :
      AIE2RegisterClass<512, 256, [v8i64, v16f32], reglist> {
    let hasCompleteDecoder = 0;
  }

  class AIE2Acc1024RegisterClass<dag reglist> :
      AIE2RegisterClass<1024, 256, [v16i64], reglist> {
    let hasCompleteDecoder = 0;
  }

class AIE220BitRegisterClass <dag reglist, RegAltNameIndex idx = NoRegAltName> :
                 RegisterClass<"AIE2", [i20], /*alignment*/ 32, reglist, idx> {
  dag Regs = reglist;
  let RegInfos = RegInfoByHwMode<
      [DefaultMode],
      [RegInfo</*size*/20, /*spill size*/32, /*spill alignment*/32>]>;
}

class AIE2Dim2DRegisterClass <dag reglist, RegAltNameIndex idx = NoRegAltName> :
                 RegisterClass<"AIE2", [i20], /*alignment*/ 32, reglist, idx> {
  dag Regs = reglist;
  let RegInfos = RegInfoByHwMode<
      [DefaultMode],
      [RegInfo</*size*/80, /*spill size*/128, /*spill alignment*/32>]>;
}

class AIE2Dim3DRegisterClass <dag reglist, RegAltNameIndex idx = NoRegAltName> :
                 RegisterClass<"AIE2", [i20], /*alignment*/ 32, reglist, idx> {
  dag Regs = reglist;
  let RegInfos = RegInfoByHwMode<
      [DefaultMode],
      [RegInfo</*size*/160, /*spill size*/256, /*spill alignment*/32>]>;
}

  class AIE2VReg2FixedBits411<bits<4> Enc, string n> : Register<n> {
    let HWEncoding{1 - 0} = 0b11;
    let HWEncoding{5 - 2} = Enc;
  }

// Special registers
  class AIE2SPLReg<bits<4> Enc, string n> : Register<n> {
    let HWEncoding{2 - 0} = 0b111;
    let HWEncoding{6 - 3} = Enc;
  }
  // Loop start
  def LS : AIE2SPLReg<0, "ls">, DwarfRegNum<[358]>;
  // Decompress pointer
  def DP : AIE2SPLReg<2, "dp">, DwarfRegNum<[359]>;
  // Link register
  def lr : AIE2SPLReg<4, "lr">, DwarfRegNum<[360]>;
  // Loop end
  def LE : AIE2SPLReg<8, "le">, DwarfRegNum<[361]>;
  // Loop count
  def LC : AIE2SPLReg<10, "lc">, DwarfRegNum<[362]>;
  // Stack Pointer
  def SP : AIE2SPLReg<12, "sp">, DwarfRegNum<[363]>;
  // Core ID
  def CORE_ID : AIE2SPLReg<6, "CORE_ID">;

  def eSpecial20 : AIE220BitRegisterClass<(add LS, DP, lr, LE, SP, CORE_ID)>;

  // Control registers
  class AIE2ControlReg<bits<4> Enc, string n> : Register<n> {
    let HWEncoding{3 - 0} = Enc;
  }
  def crSat : AIE2ControlReg<0b1001, "crSat">;
  def crRnd : AIE2ControlReg<0b0110, "crRnd">;
  def crFPMask : AIE2ControlReg<0b0011, "crFPMask">;
  def crF2IMask : AIE2ControlReg<0b0010, "crF2IMask">;
  def crF2FMask : AIE2ControlReg<0b0001, "crF2FMask">;
  def crSRSSign : AIE2ControlReg<0b1000, "crSRSSign">;
  def crUPSSign : AIE2ControlReg<0b1010, "crUPSSign">;
  def crPackSign : AIE2ControlReg<0b0101, "crPackSign">;
  def crUnpackSign : AIE2ControlReg<0b1011, "crUnpackSign">;
  def crVaddSign : AIE2ControlReg<0b0000, "crVaddSign">;
  def crSCDEn : AIE2ControlReg<0b0111, "crSCDEn">;
  def crMCDEn : AIE2ControlReg<0b0100, "crMCDEn">;

  def mCRm : AIE2RegisterClass<32, 32, [i32],
      (add crSat, crRnd, crFPMask, crF2IMask, crF2FMask,
       crSRSSign, crUPSSign, crPackSign, crUnpackSign, crVaddSign,
       crSCDEn, crMCDEn)>;

  // Status registers
  class AIE2StatusReg<bits<4> Enc, string n> : Register<n> {
    let HWEncoding{3 - 0} = Enc;
  }
  def srCarry : AIE2StatusReg<0b0000, "srCarry">;
  def srSS0 : AIE2StatusReg<0b0111, "srSS0">;
  def srMS0 : AIE2StatusReg<0b0101, "srMS0">;
  def srSRS_of : AIE2StatusReg<0b0110, "srSRS_of">;
  def srUPS_of : AIE2StatusReg<0b1001, "srUPS_of">;
  def srCompr_uf : AIE2StatusReg<0b0001, "srCompr_uf">;
  def srSparse_of : AIE2StatusReg<0b1000, "srSparse_of">;
  def srFPFlags : AIE2StatusReg<0b0100, "srFPFlags">;
  def srF2IFlags : AIE2StatusReg<0b0011, "srF2IFlags">;
  def srF2FFlags : AIE2StatusReg<0b0010, "srF2FFlags">;

  let HWEncoding = 0 in
    def tile_cntr : Register<"tile_cntr">;

  def mSRm : AIE2RegisterClass<32, 32, [i32],
      (add srCarry, srSS0, srMS0, srSRS_of, srUPS_of, srCompr_uf,
       srSparse_of, srFPFlags, srF2IFlags, srF2FFlags)>;

  def amhh0 : AIE2VReg2FixedBits411<0b0000, "amhh0">,
              DwarfRegNum<[1]>;

  def amhh1 : AIE2VReg2FixedBits411<0b0001, "amhh1">, DwarfRegNum<[3]>;

  def amhh2 : AIE2VReg2FixedBits411<0b0010, "amhh2">, DwarfRegNum<[5]>;

  def amhh3 : AIE2VReg2FixedBits411<0b0011, "amhh3">, DwarfRegNum<[7]>;

  def amhh4 : AIE2VReg2FixedBits411<0b0100, "amhh4">, DwarfRegNum<[9]>;

  def amhh5 : AIE2VReg2FixedBits411<0b0101, "amhh5">, DwarfRegNum<[11]>;

  def amhh6 : AIE2VReg2FixedBits411<0b0110, "amhh6">, DwarfRegNum<[13]>;

  def amhh7 : AIE2VReg2FixedBits411<0b0111, "amhh7">, DwarfRegNum<[15]>;

  def amhh8 : AIE2VReg2FixedBits411<0b1000, "amhh8">, DwarfRegNum<[17]>;

  def eAMHH : AIE2Acc256RegisterClass<(add amhh0, amhh1, amhh2, amhh3, amhh4,
                                          amhh5, amhh6, amhh7, amhh8)>;

  class AIE2VReg2FixedBits410<bits<4> Enc, string n> : Register<n> {
    let HWEncoding{1 - 0} = 0b10;
    let HWEncoding{5 - 2} = Enc;
  }

  def amhl0 : AIE2VReg2FixedBits410<0b0000, "amhl0">,
              DwarfRegNum<[19]>;

  def amhl1 : AIE2VReg2FixedBits410<0b0001, "amhl1">, DwarfRegNum<[21]>;

  def amhl2 : AIE2VReg2FixedBits410<0b0010, "amhl2">, DwarfRegNum<[23]>;

  def amhl3 : AIE2VReg2FixedBits410<0b0011, "amhl3">, DwarfRegNum<[25]>;

  def amhl4 : AIE2VReg2FixedBits410<0b0100, "amhl4">, DwarfRegNum<[27]>;

  def amhl5 : AIE2VReg2FixedBits410<0b0101, "amhl5">, DwarfRegNum<[29]>;

  def amhl6 : AIE2VReg2FixedBits410<0b0110, "amhl6">, DwarfRegNum<[31]>;

  def amhl7 : AIE2VReg2FixedBits410<0b0111, "amhl7">, DwarfRegNum<[33]>;

  def amhl8 : AIE2VReg2FixedBits410<0b1000, "amhl8">, DwarfRegNum<[35]>;

  def eAMHL : AIE2Acc256RegisterClass<(add amhl0, amhl1, amhl2, amhl3, amhl4,
                                          amhl5, amhl6, amhl7, amhl8)>;

  class AIE2VReg2FixedBits401<bits<4> Enc, string n> : Register<n> {
    let HWEncoding{1 - 0} = 0b01;
    let HWEncoding{5 - 2} = Enc;
  }

  def amlh0 : AIE2VReg2FixedBits401<0b0000, "amlh0">,
              DwarfRegNum<[37]>;

  def amlh1 : AIE2VReg2FixedBits401<0b0001, "amlh1">, DwarfRegNum<[39]>;

  def amlh2 : AIE2VReg2FixedBits401<0b0010, "amlh2">, DwarfRegNum<[41]>;

  def amlh3 : AIE2VReg2FixedBits401<0b0011, "amlh3">, DwarfRegNum<[43]>;

  def amlh4 : AIE2VReg2FixedBits401<0b0100, "amlh4">, DwarfRegNum<[45]>;

  def amlh5 : AIE2VReg2FixedBits401<0b0101, "amlh5">, DwarfRegNum<[47]>;

  def amlh6 : AIE2VReg2FixedBits401<0b0110, "amlh6">, DwarfRegNum<[49]>;

  def amlh7 : AIE2VReg2FixedBits401<0b0111, "amlh7">, DwarfRegNum<[51]>;

  def amlh8 : AIE2VReg2FixedBits401<0b1000, "amlh8">, DwarfRegNum<[53]>;

  def eAMLH : AIE2Acc256RegisterClass<(add amlh0, amlh1, amlh2, amlh3, amlh4,
                                          amlh5, amlh6, amlh7, amlh8)>;

  class AIE2VReg2FixedBits400<bits<4> Enc, string n> : Register<n> {
    let HWEncoding{1 - 0} = 0b00;
    let HWEncoding{5 - 2} = Enc;
  }

  def amll0 : AIE2VReg2FixedBits400<0b0000, "amll0">,
              DwarfRegNum<[55]>;

  def amll1 : AIE2VReg2FixedBits400<0b0001, "amll1">, DwarfRegNum<[57]>;

  def amll2 : AIE2VReg2FixedBits400<0b0010, "amll2">, DwarfRegNum<[59]>;

  def amll3 : AIE2VReg2FixedBits400<0b0011, "amll3">, DwarfRegNum<[61]>;

  def amll4 : AIE2VReg2FixedBits400<0b0100, "amll4">, DwarfRegNum<[63]>;

  def amll5 : AIE2VReg2FixedBits400<0b0101, "amll5">, DwarfRegNum<[65]>;

  def amll6 : AIE2VReg2FixedBits400<0b0110, "amll6">, DwarfRegNum<[67]>;

  def amll7 : AIE2VReg2FixedBits400<0b0111, "amll7">, DwarfRegNum<[69]>;

  def amll8 : AIE2VReg2FixedBits400<0b1000, "amll8">, DwarfRegNum<[71]>;

  def eAMLL : AIE2Acc256RegisterClass<(add amll0, amll1, amll2, amll3, amll4,
                                          amll5, amll6, amll7, amll8)>;

  def mAMm : AIE2Acc256RegisterClass<(add eAMHH, eAMHL, eAMLH, eAMLL)>;
  def mAMs : AIE2Acc256RegisterClass<(add eAMHH, eAMHL, eAMLH, eAMLL)>;

  class AIE2VReg1FixedBits41<bits<4> Enc, string n, list<Register> subregs> : Register<n> {
    let HWEncoding{0} = 0b1;
    let HWEncoding{4 - 1} = Enc;
    let SubRegs = subregs;
  }

  let SubRegIndices = [sub_256_lo, sub_256_hi], CoveredBySubRegs = 1 in {
  def bmh0 : AIE2VReg1FixedBits41<0b0000, "bmh0", [amhl0, amhh0]>, DwarfRegNum<[73]>;
  def bmh1 : AIE2VReg1FixedBits41<0b0001, "bmh1", [amhl1, amhh1]>, DwarfRegNum<[75]>;
  def bmh2 : AIE2VReg1FixedBits41<0b0010, "bmh2", [amhl2, amhh2]>, DwarfRegNum<[77]>;
  def bmh3 : AIE2VReg1FixedBits41<0b0011, "bmh3", [amhl3, amhh3]>, DwarfRegNum<[79]>;
  def bmh4 : AIE2VReg1FixedBits41<0b0100, "bmh4", [amhl4, amhh4]>, DwarfRegNum<[81]>;
  def bmh5 : AIE2VReg1FixedBits41<0b0101, "bmh5", [amhl5, amhh5]>, DwarfRegNum<[83]>;
  def bmh6 : AIE2VReg1FixedBits41<0b0110, "bmh6", [amhl6, amhh6]>, DwarfRegNum<[85]>;
  def bmh7 : AIE2VReg1FixedBits41<0b0111, "bmh7", [amhl7, amhh7]>, DwarfRegNum<[87]>;
  def bmh8 : AIE2VReg1FixedBits41<0b1000, "bmh8", [amhl8, amhh8]>, DwarfRegNum<[89]>;
  } // let SubRegIndices = [sub_256_lo, sub_256_hi], CoveredBySubRegs = 1

  def eBMH : AIE2Acc512RegisterClass<(add bmh0, bmh1, bmh2, bmh3, bmh4, bmh5,
                                      bmh6, bmh7, bmh8)>;

  class AIE2VReg1FixedBits40<bits<4> Enc, string n, list<Register> subregs> : Register<n> {
    let HWEncoding{0} = 0b0;
    let HWEncoding{4 - 1} = Enc;
    let SubRegs = subregs;
  }

  let SubRegIndices = [sub_256_lo, sub_256_hi], CoveredBySubRegs = 1 in {
  def bml0 : AIE2VReg1FixedBits40<0b0000, "bml0", [amll0, amlh0]>, DwarfRegNum<[91]>;
  def bml1 : AIE2VReg1FixedBits40<0b0001, "bml1", [amll1, amlh1]>, DwarfRegNum<[93]>;
  def bml2 : AIE2VReg1FixedBits40<0b0010, "bml2", [amll2, amlh2]>, DwarfRegNum<[95]>;
  def bml3 : AIE2VReg1FixedBits40<0b0011, "bml3", [amll3, amlh3]>, DwarfRegNum<[97]>;
  def bml4 : AIE2VReg1FixedBits40<0b0100, "bml4", [amll4, amlh4]>, DwarfRegNum<[99]>;
  def bml5 : AIE2VReg1FixedBits40<0b0101, "bml5", [amll5, amlh5]>, DwarfRegNum<[101]>;
  def bml6 : AIE2VReg1FixedBits40<0b0110, "bml6", [amll6, amlh6]>, DwarfRegNum<[103]>;
  def bml7 : AIE2VReg1FixedBits40<0b0111, "bml7", [amll7, amlh7]>, DwarfRegNum<[105]>;
  def bml8 : AIE2VReg1FixedBits40<0b1000, "bml8", [amll8, amlh8]>, DwarfRegNum<[107]>;
  } // let SubRegIndices = [sub_256_lo, sub_256_hi], CoveredBySubRegs = 1

  def eBML : AIE2Acc512RegisterClass<(add bml0, bml1, bml2, bml3, bml4, bml5,
                                         bml6, bml7, bml8)>;

  def eBMSH : AIE2Acc512RegisterClass<(add bmh0, bmh1, bmh2, bmh3, bmh4, bmh5,
                                       bmh6, bmh7)>;

  def eBMSL : AIE2Acc512RegisterClass<(add bml0, bml1, bml2, bml3, bml4,
                                          bml5, bml6, bml7)>;

  def mBMSm : AIE2Acc512RegisterClass<(add eBMSH, eBMSL)>;
  def mBMm : AIE2Acc512RegisterClass<(add eBMH, eBML)>;
  def mBMs : AIE2Acc512RegisterClass<(add eBMH, eBML)>;
  def mBMa : AIE2Acc512RegisterClass<(add eBMH, eBML)>;

  class AIE2VReg0FixedBits4<bits<4> Enc, string n, list<Register> subregs> : Register<n> {
    let HWEncoding{3 - 0} = Enc;
    let SubRegs = subregs;
  }

  let SubRegIndices = [sub_512_lo, sub_512_hi], CoveredBySubRegs = 1 in {
  def cm0 : AIE2VReg0FixedBits4<0b0000, "cm0", [bml0, bmh0]>, DwarfRegNum<[109]>;
  def cm1 : AIE2VReg0FixedBits4<0b0001, "cm1", [bml1, bmh1]>, DwarfRegNum<[111]>;
  def cm2 : AIE2VReg0FixedBits4<0b0010, "cm2", [bml2, bmh2]>, DwarfRegNum<[113]>;
  def cm3 : AIE2VReg0FixedBits4<0b0011, "cm3", [bml3, bmh3]>, DwarfRegNum<[115]>;
  def cm4 : AIE2VReg0FixedBits4<0b0100, "cm4", [bml4, bmh4]>, DwarfRegNum<[117]>;
  def cm5 : AIE2VReg0FixedBits4<0b0101, "cm5", [bml5, bmh5]>, DwarfRegNum<[119]>;
  def cm6 : AIE2VReg0FixedBits4<0b0110, "cm6", [bml6, bmh6]>, DwarfRegNum<[121]>;
  def cm7 : AIE2VReg0FixedBits4<0b0111, "cm7", [bml7, bmh7]>, DwarfRegNum<[123]>;
  def cm8 : AIE2VReg0FixedBits4<0b1000, "cm8", [bml8, bmh8]>, DwarfRegNum<[125]>;
  } // let SubRegIndices = [sub_512_lo, sub_512_hi], CoveredBySubRegs = 1

  def eCM : AIE2Acc1024RegisterClass<(add cm0, cm1, cm2, cm3, cm4, cm5, cm6,
                                         cm7, cm8)>;

  def mCMm : AIE2Acc1024RegisterClass<(add eCM)>;
  def mCMs : AIE2Acc1024RegisterClass<(add eCM)>;
  def mCMa : AIE2Acc1024RegisterClass<(add eCM)>;

  foreach i = 0 - 7 in {
    def dc #i : AIE220BitReg<i, "dc" #i>, DwarfRegNum<[!add(i, 142)]>;
  }

  def eDC
      : AIE220BitRegisterClass<(add dc0, dc1, dc2, dc3, dc4, dc5, dc6, dc7)>;

  def eDCH : AIE220BitRegisterClass<(add dc4, dc5, dc6, dc7)>;

  def eDCL : AIE220BitRegisterClass<(add dc0, dc1, dc2, dc3)>;

  foreach i = 0 - 7 in {
    def dj #i : AIE220BitReg<i, "dj" #i>, DwarfRegNum<[!add(i, 150)]>;
  }

  def eDJ
      : AIE220BitRegisterClass<(add dj0, dj1, dj2, dj3, dj4, dj5, dj6, dj7)>;

  def eDJH : AIE220BitRegisterClass<(add dj4, dj5, dj6, dj7)>;

  def eDJL : AIE220BitRegisterClass<(add dj0, dj1, dj2, dj3)>;

  foreach i = 0 - 7 in {
    def dn #i : AIE220BitReg<i, "dn" #i>, DwarfRegNum<[!add(i, 158)]>;
  }

  def eDN
      : AIE220BitRegisterClass<(add dn0, dn1, dn2, dn3, dn4, dn5, dn6, dn7)>;

  def eDNH : AIE220BitRegisterClass<(add dn4, dn5, dn6, dn7)>;

  def eDNL : AIE220BitRegisterClass<(add dn0, dn1, dn2, dn3)>;

  foreach i = 0 - 7 in {
    def m #i : AIE220BitReg<i, "m" #i>, DwarfRegNum<[!add(i, 182)]>;
  }

  def eM : AIE220BitRegisterClass<(add m0, m1, m2, m3, m4, m5, m6, m7)>;

  class AIE2VReg0FixedBits3<bits<3> Enc, string n, list<Register> subregs> : Register<n> {
    let HWEncoding{2 - 0} = Enc;
    let SubRegs = subregs;
  }

  let SubRegIndices = [sub_mod, sub_dim_size, sub_dim_stride, sub_dim_count], CoveredBySubRegs = 1 in {
  def d0 : AIE2VReg0FixedBits3<0b000, "d0", [m0, dn0, dj0, dc0]>, DwarfRegNum<[127]>;
  def d1 : AIE2VReg0FixedBits3<0b001, "d1", [m1, dn1, dj1, dc1]>, DwarfRegNum<[129]>;
  def d2 : AIE2VReg0FixedBits3<0b010, "d2", [m2, dn2, dj2, dc2]>, DwarfRegNum<[131]>;
  def d3 : AIE2VReg0FixedBits3<0b011, "d3", [m3, dn3, dj3, dc3]>, DwarfRegNum<[133]>;
  def d4 : AIE2VReg0FixedBits3<0b100, "d4", [m4, dn4, dj4, dc4]>, DwarfRegNum<[135]>;
  def d5 : AIE2VReg0FixedBits3<0b101, "d5", [m5, dn5, dj5, dc5]>, DwarfRegNum<[137]>;
  def d6 : AIE2VReg0FixedBits3<0b110, "d6", [m6, dn6, dj6, dc6]>, DwarfRegNum<[139]>;
  def d7 : AIE2VReg0FixedBits3<0b111, "d7", [m7, dn7, dj7, dc7]>, DwarfRegNum<[141]>;
  } // let SubRegIndices = [sub_mod, sub_dim_size, sub_dim_stride, sub_dim_count], CoveredBySubRegs = 1

  def eD : AIE2Dim2DRegisterClass<(add d0, d1, d2, d3, d4, d5, d6, d7)>;

  class AIE2Regs3D<bits<2> Enc, string n, list<Register> subregs> : Register<n> {
    let HWEncoding{1 - 0} = Enc;
    let SubRegs = subregs;
  }

  let SubRegIndices = [sub_lo_dim, sub_hi_dim], CoveredBySubRegs = 1 in {
  def d0_3d : AIE2Regs3D<0b00, "d0", [d0, d4]>;
  def d1_3d : AIE2Regs3D<0b01, "d1", [d1, d5]>;
  def d2_3d : AIE2Regs3D<0b10, "d2", [d2, d6]>;
  def d3_3d : AIE2Regs3D<0b11, "d3", [d3, d7]>;
  } // let SubRegIndices = [sub_lo_dim, sub_hi_dim], CoveredBySubRegs = 1

  def eDS : AIE2Dim3DRegisterClass<(add d0_3d, d1_3d, d2_3d, d3_3d)>;

  def mDm : AIE220BitRegisterClass<(add eDC, eDJ, eDN, eM)>;

  class AIE2VReg3FixedBits1000<bits<1> Enc, string n> : Register<n> {
    let HWEncoding{2 - 0} = 0b000;
    let HWEncoding{3} = Enc;
  }

  def DMb : AIE2VReg3FixedBits1000<0b0, "DMb">,
            DwarfRegNum<[191]>;

  def DMh : AIE2VReg3FixedBits1000<0b1, "DMh">, DwarfRegNum<[193]>;

  foreach i = 0 - 7 in {
    def p #i : AIE220BitReg<i, "p" #i>, DwarfRegNum<[!add(i, 194)]>;
  }

  def eP : AIE220BitRegisterClass<(add p0, p1, p2, p3, p4, p5, p6, p7)>;

  // Scalar registers (GPR)
  foreach i = 0 - 31 in {
    def r #i : AIE2GPReg<i, "r" #i>, DwarfRegNum<[!add(i, 234)]>;
  }

  def eR : AIE2ScalarRegisterClass<(add r0, r1, r2, r3, r4, r5, r6, r7, r8, r9,
                                    r10, r11, r12, r13, r14, r15, r16, r17, r18,
                                    r19, r20, r21, r22, r23, r24, r25, r26, r27,
                                    r28, r29, r30, r31)>;

  // Singleton register classes
  def eR26 : AIE2ScalarRegisterClass<(add r26)>;
  def eR27 : AIE2ScalarRegisterClass<(add r27)>;
  def eR28 : AIE2ScalarRegisterClass<(add r28)>;
  def eR29 : AIE2ScalarRegisterClass<(add r29)>;
  def eR31 : AIE2ScalarRegisterClass<(add r31)>;

  def eRS4 : AIE2ScalarRegisterClass<(add r16, r17, r18, r19)>;

  def eRS8
      : AIE2ScalarRegisterClass<(add r16, r17, r18, r19, r20, r21, r22, r23)>;

  // Even-Odd register pairs, starting at R16
  class AIE2GPRPair<bits<3> Enc, int first, int second> :
      Register<"r" # second # ":r" # first> {
    let HWEncoding{2 - 0} = Enc;
    let SubRegs = [!cast<Register>("r"#first),!cast<Register>("r"#second)];
  }
  foreach i = 0 - 7 in {
    let SubRegIndices = [sub_l_even, sub_l_odd], CoveredBySubRegs = 1 in
      def l # i : AIE2GPRPair<i, !add(!mul(i,2), 16), !add(!mul(i,2), 17)>,
                  DwarfRegNum<[!add(!mul(i, 2), 167)]>;
  }

  def eL : AIE2RegisterClass<64, 32, [v8i8, v4i16, v4bf16, v2i32],
                             (add l0, l1, l2, l3, l4, l5, l6, l7)>;

  class AIE2ShiftReg<bits<2> Enc, string n> : Register<n> {
    let HWEncoding{1 - 0} = Enc;
  }

  def s0 : AIE2ShiftReg<0b00, "s0">, DwarfRegNum<[267]>;
  def s1 : AIE2ShiftReg<0b01, "s1">, DwarfRegNum<[269]>;
  def s2 : AIE2ShiftReg<0b10, "s2">, DwarfRegNum<[271]>;
  def s3 : AIE2ShiftReg<0b11, "s3">, DwarfRegNum<[273]>;

  def eS : AIE2ScalarRegisterClass<(add s0, s1, s2, s3)>;

  def mSs : AIE2ScalarRegisterClass<(add eS)>;
  def mSm : AIE2ScalarRegisterClass<(add eS)>;

  class AIE2VReg2FixedBits301<bits<3> Enc, string n> : Register<n> {
    let HWEncoding{1 - 0} = 0b01;
    let HWEncoding{4 - 2} = Enc;
  }

  def wh0 : AIE2VReg2FixedBits301<0b000, "wh0">,
            DwarfRegNum<[275]>;

  def wh2 : AIE2VReg2FixedBits301<0b001, "wh2">, DwarfRegNum<[277]>;

  def wh4 : AIE2VReg2FixedBits301<0b010, "wh4">, DwarfRegNum<[279]>;

  def wh6 : AIE2VReg2FixedBits301<0b011, "wh6">, DwarfRegNum<[281]>;

  def wh8 : AIE2VReg2FixedBits301<0b100, "wh8">, DwarfRegNum<[283]>;

  def wh10 : AIE2VReg2FixedBits301<0b101, "wh10">, DwarfRegNum<[285]>;

  def eWHE : AIE2Vector256RegisterClass<(add wh0, wh2, wh4, wh6, wh8, wh10)>;

  class AIE2VReg2FixedBits311<bits<3> Enc, string n> : Register<n> {
    let HWEncoding{1 - 0} = 0b11;
    let HWEncoding{4 - 2} = Enc;
  }

  def wh1 : AIE2VReg2FixedBits311<0b000, "wh1">,
            DwarfRegNum<[287]>;

  def wh3 : AIE2VReg2FixedBits311<0b001, "wh3">, DwarfRegNum<[289]>;

  def wh5 : AIE2VReg2FixedBits311<0b010, "wh5">, DwarfRegNum<[291]>;

  def wh7 : AIE2VReg2FixedBits311<0b011, "wh7">, DwarfRegNum<[293]>;

  def wh9 : AIE2VReg2FixedBits311<0b100, "wh9">, DwarfRegNum<[295]>;

  def wh11 : AIE2VReg2FixedBits311<0b101, "wh11">, DwarfRegNum<[297]>;

  def eWHO : AIE2Vector256RegisterClass<(add wh1, wh3, wh5, wh7, wh9, wh11)>;

  class AIE2VReg2FixedBits300<bits<3> Enc, string n> : Register<n> {
    let HWEncoding{1 - 0} = 0b00;
    let HWEncoding{4 - 2} = Enc;
  }

  def wl0 : AIE2VReg2FixedBits300<0b000, "wl0">,
            DwarfRegNum<[299]>;

  def wl2 : AIE2VReg2FixedBits300<0b001, "wl2">, DwarfRegNum<[301]>;

  def wl4 : AIE2VReg2FixedBits300<0b010, "wl4">, DwarfRegNum<[303]>;

  def wl6 : AIE2VReg2FixedBits300<0b011, "wl6">, DwarfRegNum<[305]>;

  def wl8 : AIE2VReg2FixedBits300<0b100, "wl8">, DwarfRegNum<[307]>;

  def wl10 : AIE2VReg2FixedBits300<0b101, "wl10">, DwarfRegNum<[309]>;

  def eWLE : AIE2Vector256RegisterClass<(add wl0, wl2, wl4, wl6, wl8, wl10)>;

  class AIE2VReg2FixedBits310<bits<3> Enc, string n> : Register<n> {
    let HWEncoding{1 - 0} = 0b10;
    let HWEncoding{4 - 2} = Enc;
  }

  def wl1 : AIE2VReg2FixedBits310<0b000, "wl1">,
            DwarfRegNum<[311]>;

  def wl3 : AIE2VReg2FixedBits310<0b001, "wl3">, DwarfRegNum<[313]>;

  def wl5 : AIE2VReg2FixedBits310<0b010, "wl5">, DwarfRegNum<[315]>;

  def wl7 : AIE2VReg2FixedBits310<0b011, "wl7">, DwarfRegNum<[317]>;

  def wl9 : AIE2VReg2FixedBits310<0b100, "wl9">, DwarfRegNum<[319]>;

  def wl11 : AIE2VReg2FixedBits310<0b101, "wl11">, DwarfRegNum<[321]>;

  def eWLO : AIE2Vector256RegisterClass<(add wl1, wl3, wl5, wl7, wl9, wl11)>;

  def eWL : AIE2Vector256RegisterClass<(add eWLE, eWLO)>;
  def eWH : AIE2Vector256RegisterClass<(add eWHE, eWHO)>;
  def mWm : AIE2Vector256RegisterClass<(add eWHE, eWHO, eWLE, eWLO)>;
  def mWm_1 : AIE2Vector256RegisterClass<(add mWm)>;
  def mWa : AIE2Vector256RegisterClass<(add eWHE, eWHO, eWLE, eWLO)>;
  def mWb : AIE2Vector256RegisterClass<(add eWHE, eWHO, eWLE, eWLO)>;
  def mWs : AIE2Vector256RegisterClass<(add eWHE, eWHO, eWLE, eWLO)>;

  let SubRegIndices = [sub_256_lo, sub_256_hi], CoveredBySubRegs = 1 in {
  def x0 : AIE2VReg<0b0000, "x0", [wl0, wh0]>, DwarfRegNum<[323]>;
  def x2 : AIE2VReg<0b0010, "x2", [wl2, wh2]>, DwarfRegNum<[325]>;
  def x4 : AIE2VReg<0b0100, "x4", [wl4, wh4]>, DwarfRegNum<[327]>;
  def x6 : AIE2VReg<0b0110, "x6", [wl6, wh6]>, DwarfRegNum<[329]>;
  def x8 : AIE2VReg<0b1000, "x8", [wl8, wh8]>, DwarfRegNum<[331]>;
  def x10 : AIE2VReg<0b1010, "x10", [wl10, wh10]>, DwarfRegNum<[333]>;

  def x1 : AIE2VReg<0b0001, "x1", [wl1, wh1]>, DwarfRegNum<[335]>;
  def x3 : AIE2VReg<0b0011, "x3", [wl3, wh3]>, DwarfRegNum<[337]>;
  def x5 : AIE2VReg<0b0101, "x5", [wl5, wh5]>, DwarfRegNum<[339]>;
  def x7 : AIE2VReg<0b0111, "x7", [wl7, wh7]>, DwarfRegNum<[341]>;
  def x9 : AIE2VReg<0b1001, "x9", [wl9, wh9]>, DwarfRegNum<[343]>;
  def x11 : AIE2VReg<0b1011, "x11", [wl11, wh11]>, DwarfRegNum<[345]>;
  } // let SubRegIndices = [sub_256_lo, sub_256_hi], CoveredBySubRegs = 1

  def eXE : AIE2Vector512RegisterClass<(add x0, x2, x4, x6, x8, x10)>;
  def eXO : AIE2Vector512RegisterClass<(add x1, x3, x5, x7, x9, x11)>;

  def mXm : AIE2Vector512RegisterClass<(add eXE, eXO)>;
  def mXn : AIE2Vector512RegisterClass<(add eXE, eXO)>;
  def mXv : AIE2Vector512RegisterClass<(add eXE, eXO)>;
  def mXw : AIE2Vector512RegisterClass<(add eXE, eXO)>;
  def mXa : AIE2Vector512RegisterClass<(add eXE, eXO)>;
  def mXs : AIE2Vector512RegisterClass<(add eXE, eXO)>;

  let SubRegIndices = [sub_512_lo, sub_512_hi], CoveredBySubRegs = 1 in {
  def y2 : AIE2VReg<0b0000, "y2", [x4, x5]>, DwarfRegNum<[347]>;
  def y3 : AIE2VReg<0b0001, "y3", [x6, x7]>, DwarfRegNum<[349]>;
  def y4 : AIE2VReg<0b0010, "y4", [x8, x9]>, DwarfRegNum<[351]>;
  def y5 : AIE2VReg<0b0011, "y5", [x10, x11]>, DwarfRegNum<[353]>;
  } // let SubRegIndices = [sub_512_lo, sub_512_hi], CoveredBySubRegs = 1

  def eYs : AIE2Vector1024RegisterClass<(add y2, y3, y4, y5)>;

  // Synthetic 64-bit mask registers
  // Mask registers are actually 128-bit wide, but the 64-bit sub-registers can
  // be grouped together with 256-bit vector registers to form 320-bit
  // vector-mask registers
  foreach i = 0 - 3 in {
    def ql #i : AIE2VReg<i, "ql" #i>;
    def qh #i : AIE2VReg<i, "qh" #i>;
  }

  // 128-bit mask registers
  foreach i = 0 - 3 in {
    let SubRegIndices = [sub_lo_mask, sub_hi_mask], CoveredBySubRegs = 1 in {
      def q #i : AIE2VReg<i, "q" #i, [!cast<Register>("ql" #i), !cast<Register>("qh" #i)]>;
    }
  }

  def eQQEs : AIE2Vector128RegisterClass<(add q0, q2)>;
  def eQQOs : AIE2Vector128RegisterClass<(add q1, q3)>;

  def mQQm : AIE2Vector128RegisterClass<(add eQQEs, eQQOs)>;
  def mQQa : AIE2Vector128RegisterClass<(add eQQEs, eQQOs)>;
  def mQQs : AIE2Vector128RegisterClass<(add eQQEs, eQQOs)>;

  // 320-bit vector-mask registers
  // These consist of 64-bit mask + 256-bit vector registers
  foreach i = 0 - 3 in {
    let SubRegIndices = [sub_q, sub_w], CoveredBySubRegs = 1 in {
      def qwl #i : AIE2VReg<i, "qwl" #i, [!cast<Register>("ql" #i), !cast<Register>("wl" #i)]>;
      def qwh #i : AIE2VReg<i, "qwh" #i, [!cast<Register>("qh" #i), !cast<Register>("wh" #i)]>;
    }
  }

  // 640-bit vector-mask registers
  // These consist of two 320-bit vector-mask registers
  foreach i = 0 - 3 in {
    let SubRegIndices = [sub_lo_vecmask, sub_hi_vecmask], CoveredBySubRegs = 1 in {
      def qx #i : AIE2VReg<i, "qx" #i, [!cast<Register>("qwl" #i), !cast<Register>("qwh" #i)]>;
    }
  }

  def eQQXEs : AIE2ScalarRegisterClass<(add qx0, qx2)>;
  def eQQXOs : AIE2ScalarRegisterClass<(add qx1, qx3)>;

  def eQXHEs : AIE2ScalarRegisterClass<(add qwh0, qwh2)>;
  def eQXHOs : AIE2ScalarRegisterClass<(add qwh1, qwh3)>;

  def eQXLEs : AIE2ScalarRegisterClass<(add qwl0, qwl2)>;
  def eQXLOs : AIE2ScalarRegisterClass<(add qwl1, qwl3)>;

  def mQXHb : AIE2ScalarRegisterClass<(add eQXHEs, eQXHOs)>;
  def mQXLb : AIE2ScalarRegisterClass<(add eQXLEs, eQXLOs)>;
  def mQQXw : AIE2ScalarRegisterClass<(add eQQXEs, eQQXOs)>;
  def mQXHLb : AIE2ScalarRegisterClass<(add mQXHb, mQXLb)>;

// These are compound register classes. Generating the register pressure logic
// for them and all their intersections/unions can get complicated and it is
// frankly not needed. Note that the pre-RA scheduler can get quite confused if
// there are too many pressure sets to track, and will happily consider an
// invalid pressure set as valid, making heuristics flimsy at best.
let GeneratePressureSet = false in {
  def spill_eS_to_eR : AIE2ScalarRegisterClass<(add eS, eR)>;
  def spill_eP_to_eR : AIE2ScalarRegisterClass<(add eP, eR)>;
  def spill_eM_to_eR : AIE2ScalarRegisterClass<(add eM, eR, eDN)>;
  def spill_eDN_to_eR : AIE2ScalarRegisterClass<(add eDN, eR)>;
  def spill_eDJ_to_eR : AIE2ScalarRegisterClass<(add eDJ, eR, eDN)>;
  def spill_eDC_to_eR : AIE2ScalarRegisterClass<(add eDC, eR)>;

  def ePmDm : AIE220BitRegisterClass<(add eP, mDm)>;
  def eRLC : AIE2ScalarRegisterClass<(add eR, LC)>;
  def eRCR : AIE2ScalarRegisterClass<(add mCRm, eR)>;

  def mAluCg : AIE2ScalarRegisterClass<(add eR, LC)>;
  def mMvSclDst : AIE2ScalarRegisterClass<(add eR, mDm, eP, eS, LE, LS, lr, SP, mCRm, mSRm, DP, LC)>;
  def mMvSclDstCg : AIE2ScalarRegisterClass<(add mMvSclDst)>;
  def mMvSclSrc : AIE2ScalarRegisterClass<(add mMvSclDst, CORE_ID)>;

  def mLdaScl : AIE2ScalarRegisterClass<(add eP, eR, eDC, eDJ, eDN, eM, lr)>;
  def mSclSt : AIE2ScalarRegisterClass<(add eP, eR, eDC, eDJ, eDN, eM, lr)>;
  def mLdaCg : AIE2ScalarRegisterClass<(add eP, eR, eDC, eDJ, eDN, eM, LC)>;
  def mSclMS : AIE2ScalarRegisterClass<(add eR, mDm, eP)>;

  def mShflDst   : AIE2Vector512RegisterClass<(add mXm, mBMSm)>;

// VMOV register classes
// These classes only encode operand constraints, and will be constrained before
// register allocation using register bank assignments. That's good, because
// these are big classes, and they would crash the TableGen backend when trying
// to generate the register pressure logic. isAllocatable = false simplifies the
// work for the RegisterInfoEmitter backend (mainly, no need to compute register
// unit weights), and solves the problem.
let isAllocatable = false in {
  def mMvAMWQSrc : AIE2Vector256RegisterClass<(add mAMm, mWm, mQQm)>;
  def mMvAMWQDst : AIE2Vector256RegisterClass<(add mMvAMWQSrc)>;
  def mMvBMXSrc  : AIE2Vector512RegisterClass<(add mBMm, mXm)>;
  def mMvBMXDst  : AIE2Vector512RegisterClass<(add mMvBMXSrc)>;
}
} // let GeneratePressureSet = false

} // End AIE2 Namespace
