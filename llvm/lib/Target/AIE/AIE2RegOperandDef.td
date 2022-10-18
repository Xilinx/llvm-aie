//===- AIE2RegOperandDef.td --------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
class AIE2RegisterOperand<RegisterClass c> : RegisterOperand<c> {
  let EncoderMethod = "get" # c # OpValue;
}
 def OP_mDm : AIE2RegisterOperand<mDm>;
 def OP_mMvSclDst_and_eR : RegisterOperand<eR> {
  let EncoderMethod = "getmMvSclDstOpValue";
 }
 def OP_mAluCg : AIE2RegisterOperand<mAluCg>;
 def OP_mMvSclDst : AIE2RegisterOperand<mMvSclDst>;
 def OP_mMvSclDstCg : AIE2RegisterOperand<mMvSclDstCg>;
 def OP_mMvSclSrc : AIE2RegisterOperand<mMvSclSrc>;
 def OP_mLdaScl : AIE2RegisterOperand<mLdaScl>;
 def OP_mSclSt : AIE2RegisterOperand<mSclSt>;
 def OP_mSclMS : AIE2RegisterOperand<mSclMS>;
 def OP_mLdaCg : AIE2RegisterOperand<mLdaCg>;
 def OP_mWm_1  : AIE2RegisterOperand<mWm_1>;

 // VMOV Operands
 def OP_mMvAMWQSrc : AIE2RegisterOperand<mMvAMWQSrc>;
 def OP_mMvAMWQDst : AIE2RegisterOperand<mMvAMWQDst>;
 def OP_mMvBMXSrc : AIE2RegisterOperand<mMvBMXSrc>;
 def OP_mMvBMXDst : AIE2RegisterOperand<mMvBMXDst>;
 def OP_mMcdSrc : AIE2RegisterOperand<mMvBMXDst>;
 def OP_mScdDst : AIE2RegisterOperand<mMvBMXDst>;

 // VSHUFFLE Operands
 def OP_mShflDst : AIE2RegisterOperand<mShflDst>;

 def OP_mRS4m : AIE2RegisterOperand<eRS4>;

// VLDB.SPARSE Operands
def OP_mQXHLb : AIE2RegisterOperand<mQXHLb>;
