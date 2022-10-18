//===-- AIERegisterInfo.td - AIEngine Register defs --------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the AIEngine register files
//===----------------------------------------------------------------------===//

let Namespace = "AIE" in {

class AIEReg<string n> : Register<n> {
}
// Some registers are defined in the architecture with Dwarf info,
// but don't show up in the instruction encodings.
class AIEPseudoReg<string n> : AIEReg<n> {
  let HWEncoding = 0;
}
// Register encodings from mMvScl, mSclSt, mLdaScl
class AIEGPReg<bits<4> Enc, string n> : Register<n> {
  let HWEncoding{3-0} = Enc;
  let HWEncoding{5-4} = 0b11;
}
class AIEPTRReg<bits<3> Enc, string n> : Register<n> {
  let HWEncoding{2-0} = Enc;
  let HWEncoding{5-3} = 0b101;
}
class AIEMODReg<bits<3> Enc, string n> : Register<n> {
  let HWEncoding{2-0} = Enc;
  let HWEncoding{5-3} = 0b100;
}
class AIESPReg<bits<6> Enc, string n> : Register<n> {
  let HWEncoding{5-0} = Enc;
}
class AIEVReg<bits<4> Enc, string n, list<Register> subregs = []> : Register<n> {
  let HWEncoding{3-0} = Enc;
  let SubRegs = subregs;
}

// FIXME: CL, CH, CB, CS move around in the encoding space.
// How to deal with that?
class AIECBReg<bits<3> Enc, string n> : Register<n> {
  let HWEncoding{2-0} = Enc;
}
class AIECSReg<bits<3> Enc, string n> : Register<n> {
  let HWEncoding{2-0} = Enc;
}
class AIECtrlReg<bits<3> Enc, string n, list<Register> subregs = []> : Register<n> {
  let HWEncoding{2-0} = Enc;
  let SubRegs = subregs;
}
class AIESReg<bits<3> Enc, string n> : Register<n> {
  let HWEncoding{2-0} = Enc;
}

//def ABIRegAltName : RegAltNameIndex;

// Integer registers

//let RegAltNameIndices = [ABIRegAltName] in {
foreach i = 0-15 in {
  def r#i  : AIEGPReg<i, "r"#i>, DwarfRegNum<[!add(i,152)]>;
}
//}

// Pointer Registers
foreach i = 0-7 in {
  def p#i  : AIEPTRReg<i, "p"#i>, DwarfRegNum<[!add(i,140)]>;
}

// Modifier Registers
foreach i = 0-7 in {
  def m#i  : AIEMODReg<i, "m"#i>, DwarfRegNum<[!add(i,114)]>;
}

  // Register number here come from the mv2special and special2mv
  // instructions
  /* mSpecialDst Encoding
     000001	mSP
     000101	mLR
     000100	mLS
     000011	mLE
     000010	mLC
     1011x	mMD
     1010x	mMC
     100	mS */
  // Stack Pointer
  def SP : AIESPReg<1,"sp">, DwarfRegNum<[200]>;
  def LC : AIESPReg<2,"lc">, DwarfRegNum<[110]>;
  def LE : AIESPReg<3,"le">, DwarfRegNum<[111]>;
  def LS : AIESPReg<4,"ls">, DwarfRegNum<[113]>;
  // Link Register.  Stores return address during function call
  def lr : AIESPReg<5,"lr">, DwarfRegNum<[112]>;

foreach i = 0-7 in {
  def s#i  : AIESPReg<!add(i,32), "s"#i>, DwarfRegNum<[!add(i,185)]>;
}
  def mc0 : AIESPReg<40,"mc0">, DwarfRegNum<[124]>;
  def mc1 : AIESPReg<41,"mc1">, DwarfRegNum<[125]>;
  def md0 : AIESPReg<44,"md0">, DwarfRegNum<[128]>;
  def md1 : AIESPReg<45,"md1">, DwarfRegNum<[129]>;


// RISCV archeology
def XLenVT : ValueTypeByHwMode<[DefaultMode],
                               [i32]>;
def CCType : ValueTypeByHwMode<[DefaultMode],
                               [i32]>;

// 128-bit vector registers
foreach i = 0-7 in {
  def vl#i  : AIEVReg<i, "vl"#i>, DwarfRegNum<[!add(i,240)]>;
}
foreach i = 0-7 in {
  def vh#i  : AIEVReg<!add(i,8), "vh"#i>, DwarfRegNum<[!add(i,232)]>;
}
// Subregister indices.
def sub_128bit_lo  : SubRegIndex<128>;
def sub_128bit_hi  : SubRegIndex<128, 128>;
def sub_256bit_lo  : SubRegIndex<256>;
def sub_256bit_hi  : SubRegIndex<256, 256>;
def sub_512bit_lo  : SubRegIndex<512>;
def sub_512bit_hi  : SubRegIndex<512, 512>;
def sub_32_lo  : SubRegIndex<32>;
def sub_32_hi  : SubRegIndex<32, 32>;
def sub_384bit_lo  : SubRegIndex<384>; //(0..383)
def sub_384bit_hi  : SubRegIndex<384, 384>; //(384..767)

// 256-bit vector registers
let SubRegIndices = [sub_128bit_lo, sub_128bit_hi], CoveredBySubRegs = 1 in {
def wr0  : AIEVReg<0, "wr0", [vl0, vh0]>, DwarfRegNum<[296]>;
def wr1  : AIEVReg<1, "wr1", [vl1, vh1]>, DwarfRegNum<[297]>;
def wr2  : AIEVReg<2, "wr2", [vl2, vh2]>, DwarfRegNum<[298]>;
def wr3  : AIEVReg<3, "wr3", [vl3, vh3]>, DwarfRegNum<[299]>;
def wc0  : AIEVReg<4, "wc0", [vl4, vh4]>, DwarfRegNum<[276]>;
def wc1  : AIEVReg<5, "wc1", [vl5, vh5]>, DwarfRegNum<[277]>;
def wd0  : AIEVReg<6, "wd0", [vl6, vh6]>, DwarfRegNum<[284]>;
def wd1  : AIEVReg<7, "wd1", [vl7, vh7]>, DwarfRegNum<[285]>;
}
// 512-bit vector registers
let SubRegIndices = [sub_256bit_lo, sub_256bit_hi], CoveredBySubRegs = 1 in {
def xa  : AIEVReg<0, "xa", [wr0, wr1]>, DwarfRegNum<[306]>;
def xb  : AIEVReg<1, "xb", [wr2, wr3]>, DwarfRegNum<[307]>;
def xc  : AIEVReg<2, "xc", [wc0, wc1]>, DwarfRegNum<[308]>;
def xd  : AIEVReg<3, "xd", [wd0, wd1]>, DwarfRegNum<[309]>;
}
// 1024-bit vector registers
let SubRegIndices = [sub_512bit_lo, sub_512bit_hi], CoveredBySubRegs = 1 in {
def ya  : AIEVReg<0, "ya", [xa, xb]>, DwarfRegNum<[310]>;
def yd  : AIEVReg<1, "yd", [xd, xb]>, DwarfRegNum<[311]>;
}

// 384-bit accumulator (8x48-bit) registers
foreach i = 0-3 in {
  def aml#i  : AIEVReg<i, "aml"#i>, DwarfRegNum<[!add(i,12)]>;
}
foreach i = 0-3 in {
  def amh#i  : AIEVReg<!add(i,4), "amh"#i>, DwarfRegNum<[!add(i,8)]>;
}
// Wide accumulator (8x80-bit/16x48-bit) registers
let SubRegIndices = [sub_384bit_lo, sub_384bit_hi], CoveredBySubRegs = 1 in {
  def bm0  : AIEVReg<0, "bm0", [aml0, amh0]>, DwarfRegNum<[16]>;
  def bm1  : AIEVReg<1, "bm1", [aml1, amh1]>, DwarfRegNum<[17]>;
  def bm2  : AIEVReg<2, "bm2", [aml2, amh2]>, DwarfRegNum<[18]>;
  def bm3  : AIEVReg<3, "bm3", [aml3, amh3]>, DwarfRegNum<[19]>;
}
foreach i = 0-7 in {
  def cb#i  : AIECBReg<i, "cb"#i>, DwarfRegNum<[!add(i,36)]>;
}
foreach i = 0-7 in {
  def cs#i  : AIECSReg<i, "cs"#i>, DwarfRegNum<[!add(i,93)]>;
}

// Cascade.
def scd  : AIEPseudoReg<"scd">, DwarfRegNum<[198]>;
def mcd  : AIEPseudoReg<"mcd">, DwarfRegNum<[126]>;

def tile_ctrl  : AIEPseudoReg<"tile_ctrl">, DwarfRegNum<[211]>;

// control registers
foreach i = 0-7 in {
  def cl#i  : AIECtrlReg<i, "cl"#i>, DwarfRegNum<[!add(i,60)]>;
}
foreach i = 0-7 in {
  def ch#i  : AIECtrlReg<i, "ch"#i>, DwarfRegNum<[!add(i,44)]>;
}
// Wide control registers
let SubRegIndices = [sub_32_lo, sub_32_hi], CoveredBySubRegs = 1 in {
  foreach i = 0-7 in {
    def c#i  : AIECtrlReg<i, "c"#i,
                  [!cast<AIECtrlReg>("cl"#i), !cast<AIECtrlReg>("ch"#i)]>,
                  DwarfRegNum<[!add(i,20)]>;
  }
}

// We have lots of different register classes, representing register
// restrictions for different instructions
class AIERegisterClass <int size, list<ValueType> regTypes, dag reglist, RegAltNameIndex idx = NoRegAltName> :
		RegisterClass<"AIE", regTypes, /*alignment*/size, reglist, idx> {
  dag Regs = reglist;
  let DecodeZeroBitOperand = true;
  let RegInfos = RegInfoByHwMode<
      [DefaultMode],
      [RegInfo</*size*/size, /*spill size*/size, /*spill alignment*/size>]>;
}
class AIEScalarRegisterClass <dag reglist, RegAltNameIndex idx = NoRegAltName> :
		AIERegisterClass<32, [v8i1, v16i1, i20, i32, f32], reglist, idx>;

class AIEPointerRegisterClass <dag reglist, RegAltNameIndex idx = NoRegAltName> :
		  RegisterClass<"AIE", [i20], /*alignment*/ 32, reglist, idx> {
  dag Regs = reglist;
  let RegInfos = RegInfoByHwMode<
      [DefaultMode],
      [RegInfo</*size*/20, /*spill size*/32, /*spill alignment*/32>]>;
}

/////////////////////////
// Basic Register Classes

// The order of registers represents the preferred allocation sequence.
// Registers are listed in the order caller-save, callee-save, specials.
def GPR : AIEScalarRegisterClass<(add
    (sequence "r%u", 10, 15),
    (sequence "r%u", 5, 9),
    (sequence "r%u", 0, 4)
  )>;

// Some instructions (e.g. mv_ssb2scl) can only access 7 registers.
def GPR0_7 : AIEScalarRegisterClass<(sequence "r%u", 0, 7)>;

// Some instructions (e.g. select_r_rr) can only operate on r0.
def GPR0 : AIEScalarRegisterClass<(add r0)>;

// Some instructions (e.g. nlf_combo) can only operate on r1.
def GPR1 : AIEScalarRegisterClass<(add r1)>;

// Some instructions (e.g. fx2flt, flt2fx) can only operate on certain scale registers.
def S2 : AIEScalarRegisterClass<(add s2)>;
def S3 : AIEScalarRegisterClass<(add s3)>;

def PTR : AIEPointerRegisterClass<(sequence "p%u", 0, 7)>;
def MOD : AIEPointerRegisterClass<(sequence "m%u", 0, 7)>;
def LR  : AIEPointerRegisterClass<(add lr)>;
// cb0-cb7 registers for indirect jumps
def mCB : AIEPointerRegisterClass<(sequence "cb%u", 0, 7)>;
def mCS : AIEPointerRegisterClass<(sequence "cs%u", 0, 7)>;
def mS : AIERegisterClass<8, [i32], (sequence "s%u", 0, 7)>;
def S7 : AIERegisterClass<8, [i32], (add s7)>;
// FIXME: 20 bits
def SPR : AIEScalarRegisterClass<(add
	 SP, LC, LE, LS, LR,
	 mS, mc0, mc1, md0, md1
  )>;

// 64-bit control registers
def mCL : AIERegisterClass<32, [i32], (sequence "cl%u", 0, 7)>;
def mCH : AIERegisterClass<32, [i32], (sequence "ch%u", 0, 7)>;
def mC  : AIERegisterClass<64, [v2i32], (sequence "c%u", 0, 7)>;

// Vector registers
class AIEVector128RegisterClass<dag reglist> :
    AIERegisterClass<128, [v16i8, v8i16, v4i32, v4f32], reglist>;
class AIEVector256RegisterClass<dag reglist> :
    AIERegisterClass<256, [v32i8, v16i16, v8f32, v8i32, v4i64, f32], reglist>;
class AIEVector512RegisterClass<dag reglist> :
    AIERegisterClass<512, [v64i8, v32i16, v16i32, v16f32, v8i64], reglist>;
class AIEVector1024RegisterClass<dag reglist> :
    AIERegisterClass<1024, [v128i8, v64i16, v32i32, v32f32], reglist>;
def VECFWA : AIEVector256RegisterClass<(add wr0, wr1)>;
def VECFWB : AIEVector256RegisterClass<(add wr2, wr3)>;
def VECFWC : AIEVector256RegisterClass<(add wc0, wc1)>;
def VECFWD : AIEVector256RegisterClass<(add wd0, wd1)>;

def VEC128 : AIEVector128RegisterClass<
  (add (sequence "vl%u", 0, 7), (sequence "vh%u", 0, 7))>;
def VEC256 : AIEVector256RegisterClass<(add wr0, wr1, wr2, wr3, wc0, wc1, wd0, wd1)>;
def VEC512 : AIEVector512RegisterClass<(add xa, xb, xc, xd)>;
def VEC1024 : AIEVector1024RegisterClass<(add ya, yd)>;

class AIEVector384RegisterClass<dag reglist> :
    AIERegisterClass<384, [v8i48], reglist>;
class AIEVector768RegisterClass<dag reglist> :
    AIERegisterClass<768, [v16i48], reglist>;
def AML : AIEVector384RegisterClass<(add (sequence "aml%u", 0, 3))>;
def AMH : AIEVector384RegisterClass<(add (sequence "amh%u", 0, 3))>;
def ACC384 : AIEVector384RegisterClass<(add AML, AMH)>;
def ACC768 : AIEVector768RegisterClass<(add (sequence "bm%u", 0, 3))>;
/////////////////////
//// Register Subsets
def SP_only : AIEPointerRegisterClass<(add SP)>;
def MC0 : AIEScalarRegisterClass<(add mc0)>;
def MC1 : AIEScalarRegisterClass<(add mc1)>;
def MD0 : AIEScalarRegisterClass<(add md0)>;
def MD1 : AIEScalarRegisterClass<(add md1)>;
def mMCMD : AIEScalarRegisterClass<(add mc0, mc1, md0, md1)>;
def mVn : AIEVector128RegisterClass<(add VEC128)>;
def mWABv : AIEVector256RegisterClass<(add VECFWA, VECFWB)>;
// Presumably mWABDv could also be the VECFWB, but this doesn't seem to
// actually work.
def mWAv : AIEVector256RegisterClass<(add VECFWA)>;
def mWABDv : AIEVector256RegisterClass<(add VECFWA, VECFWD)>;
def mWBDv  : AIEVector256RegisterClass<(add VECFWB, VECFWD)>;
def mWCDv  : AIEVector256RegisterClass<(add VECFWC, VECFWD)>;
def mXABv : AIEVector512RegisterClass<(add xa, xb)>;
def mXCDv : AIEVector512RegisterClass<(add xc, xd)>;

///////////////////////
//// Register Supersets
def GPRPTRMOD : AIEScalarRegisterClass<(add GPR, PTR, MOD)>; // The encodings for these registers are often the same
def mRCm : AIEScalarRegisterClass<(add GPR, mCL, mCH)>;
def PTRMODCSCB : AIEPointerRegisterClass<(add PTR, MOD, mCS, mCB)>;
def mMvScl : AIEScalarRegisterClass<(add mRCm, PTRMODCSCB)>;
def mLdaScl : AIEScalarRegisterClass<(add mRCm, PTRMODCSCB)>;
def mLdbScl : AIEScalarRegisterClass<(add GPR, PTR, MOD, mCL, mCH, LR, LS, LE, LC)>;
def mSclSt_PTR : AIEPointerRegisterClass<(add PTR, MOD, LR)>;
def mSclSt : AIEScalarRegisterClass<(add mRCm, mSclSt_PTR)>;
def mAluCg12 : AIEScalarRegisterClass<(add MOD, GPR, mCS, LC)>;
def mLdaCg10 : AIEScalarRegisterClass<(add GPR0_7, MOD, mCS, LC)>;
def mMv0Cg20 : AIEScalarRegisterClass<(add GPR, PTR, mCL, mCH, LR, LS, SP, mCB)>;
} // Namespace = "AIE"
