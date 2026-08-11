//===- AIE2RegOperandDef.td --------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
 def OP_mDm : AIEBaseRegisterOperand<mDm>;
 def OP_mMvSclDst_and_eR : RegisterOperand<eR> {
  let EncoderMethod = "getmMvSclDstOpValue";
 }
 def OP_mAluCg : AIEBaseRegisterOperand<mAluCg>;
 def OP_mMvSclDst : AIEBaseRegisterOperand<mMvSclDst>;
 def OP_mMvSclDstCg : AIEBaseRegisterOperand<mMvSclDstCg>;
 def OP_mMvSclSrc : AIEBaseRegisterOperand<mMvSclSrc>;
 def OP_mLdaScl : AIEBaseRegisterOperand<mLdaScl>;
 def OP_mSclSt : AIEBaseRegisterOperand<mSclSt>;
 def OP_mSclMS : AIEBaseRegisterOperand<mSclMS>;
 def OP_mLdaCg : AIEBaseRegisterOperand<mLdaCg>;
 def OP_mWm_1  : AIEBaseRegisterOperand<mWm_1>;

 // VMOV Operands
 def OP_mMvAMWQSrc : AIEBaseRegisterOperand<mMvAMWQSrc>;
 def OP_mMvAMWQDst : AIEBaseRegisterOperand<mMvAMWQDst>;
 def OP_mMvBMXSrc : AIEBaseRegisterOperand<mMvBMXSrc>;
 def OP_mMvBMXDst : AIEBaseRegisterOperand<mMvBMXDst>;
 def OP_mMcdSrc : AIEBaseRegisterOperand<mMvBMXDst>;
 def OP_mScdDst : AIEBaseRegisterOperand<mMvBMXDst>;

 // VSHUFFLE Operands
 def OP_mShflDst : AIEBaseRegisterOperand<mShflDst>;

 def OP_mRS4m : AIEBaseRegisterOperand<eRS4>;

// VLDB.SPARSE Operands
def OP_mQXHLb : AIEBaseRegisterOperand<mQXHLb>;
