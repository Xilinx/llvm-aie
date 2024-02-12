//===- AIE2RegisterInfo.td ---------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

let Namespace = "AIE2" in {

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

def sub_512_hi_256_lo : ComposedSubRegIndex<sub_512_hi, sub_256_lo>;
def sub_512_hi_256_hi : ComposedSubRegIndex<sub_512_hi, sub_256_hi>;

def sub_lo_mask : SubRegIndex<64,  0>;
def sub_hi_mask : SubRegIndex<64, 64>;

def sub_q : SubRegIndex<64,   0>;
def sub_w : SubRegIndex<256, 64>;

def sub_sparse_q : SubRegIndex<128,   512>;
def sub_sparse_x : SubRegIndex<512,   0>;

}

class AIE2VReg<bits<4> Enc, string n, list<Register> subregs = []> : Register<n> {
  let HWEncoding{3-0} = Enc;
  let SubRegs = subregs;
}

include "AIE2GenRegisterInfo.td"

def VEC128 : AIE2Vector128RegisterClass<(add mQQm, mQQa, mQQs)>;
def VEC256 : AIE2Vector256RegisterClass<(add mWm, mWa, mWb, mWs)>;
def VEC512 : AIE2Vector512RegisterClass<(add mXm, mXn, mXv, mXw, mXa, mXs)>;
def VEC1024 : AIE2Vector1024RegisterClass<(add eYs)>;

def ACC256 : AIE2Acc256RegisterClass<(add eAMHH, eAMHL, eAMLH, eAMLL, mAMm, mAMs)>;
def ACC512 : AIE2Acc512RegisterClass<(add eBMH, eBML, eBMSH, eBMSL, mBMSm, mBMm, mBMs, mBMa)>;
def ACC1024 : AIE2Acc1024RegisterClass<(add eCM)>;
def SPARSEVEC640 : AIE2Vector640RegisterClass<(add eQQXEs, eQQXOs)>;

let Namespace = "AIE2" in {
  // This is a strange class, but llvm would define it anyway by performing an
  // "AND" on e.g. the alu and eR register classes.
  // Here, we just give it a more explicit name.
  def eP_as_32Bit : AIE2ScalarRegisterClass<(add eP)>;
  def eM_as_32Bit : AIE2ScalarRegisterClass<(add eM)>;
  def mDm_as_32Bit : AIE2ScalarRegisterClass<(add mDm)>;
  def eDC_as_32Bit : AIE2ScalarRegisterClass<(add eDC)>;
  def eDJ_as_32Bit : AIE2ScalarRegisterClass<(add eDJ)>;
  def eDN_as_32Bit : AIE2ScalarRegisterClass<(add eDN)>;
  def eSpecial20_as_32Bit : AIE2ScalarRegisterClass<(
      add LS, DP, lr, LE, SP)>;

} //End AIE2 Namespace
