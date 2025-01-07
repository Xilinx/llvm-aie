//===-AIE2PRegOperandDef.td -----------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//* Automatically generated file, do not edit! *

class AIE2PRegisterOperand<RegisterClass c> : RegisterOperand<c> {
  let EncoderMethod = "get" # c # OpValue;
}

def OP_mWa : AIE2PRegisterOperand<mWa>;
def OP_mXm : AIE2PRegisterOperand<mXm>;
def OP_mCMm : AIE2PRegisterOperand<mCMm>;
def OP_mQEXsm : AIE2PRegisterOperand<mQEXsm>;
def OP_mEs : AIE2PRegisterOperand<mEs>;
def OP_mAguSrc : AIE2PRegisterOperand<mAguSrc>;
def OP_mQXsb : AIE2PRegisterOperand<mQXsb>;
def OP_mEhm : AIE2PRegisterOperand<mEhm>;
def OP_mLdaScl : AIE2PRegisterOperand<mLdaScl>;
def OP_mQQsm : AIE2PRegisterOperand<mQQsm>;
def OP_mWs : AIE2PRegisterOperand<mWs>;
def OP_mXw : AIE2PRegisterOperand<mXw>;
def OP_mAguDst : AIE2PRegisterOperand<mAguDst>;
def OP_mEXm : AIE2PRegisterOperand<mEXm>;
def OP_mQXsa : AIE2PRegisterOperand<mQXsa>;
def OP_mDm : AIE2PRegisterOperand<mDm>;
def OP_mQQss : AIE2PRegisterOperand<mQQss>;
def OP_mQXsm : AIE2PRegisterOperand<mQXsm>;
def OP_mShflBMDst : AIE2PRegisterOperand<mShflBMDst>;
def OP_mBMm : AIE2PRegisterOperand<mBMm>;
def OP_mMvBMXDst : AIE2PRegisterOperand<mMvBMXDst>;
def OP_mShflXDst : AIE2PRegisterOperand<mShflXDst>;
def OP_mMvSclDst : AIE2PRegisterOperand<mMvSclDst>;
def OP_mMcdXSrc : AIE2PRegisterOperand<mMcdXSrc>;
def OP_mEXn : AIE2PRegisterOperand<mEXn>;
def OP_mMvSclDstCg : AIE2PRegisterOperand<mMvSclDstCg>;
def OP_mSclMS : AIE2PRegisterOperand<mSclMS>;
def OP_mXn : AIE2PRegisterOperand<mXn>;
def OP_mBMSm : AIE2PRegisterOperand<mBMSm>;
def OP_mXv : AIE2PRegisterOperand<mXv>;
def OP_mWb : AIE2PRegisterOperand<mWb>;
def OP_mMvBMXSrc : AIE2PRegisterOperand<mMvBMXSrc>;
def OP_mAluCg : AIE2PRegisterOperand<mAluCg>;
def OP_mEXw : AIE2PRegisterOperand<mEXw>;
def OP_mXs : AIE2PRegisterOperand<mXs>;
def OP_mEXs : AIE2PRegisterOperand<mEXs>;
def OP_mCMs : AIE2PRegisterOperand<mCMs>;
def OP_mFifoHLReg : AIE2PRegisterOperand<mFifoHLReg>;
def OP_mSRm : AIE2PRegisterOperand<mSRm>;
def OP_mElm : AIE2PRegisterOperand<mElm>;
def OP_mQEXsw : AIE2PRegisterOperand<mQEXsw>;
def OP_mQQsa : AIE2PRegisterOperand<mQQsa>;
def OP_mEXv : AIE2PRegisterOperand<mEXv>;
def OP_mXa : AIE2PRegisterOperand<mXa>;
def OP_mWm : AIE2PRegisterOperand<mWm>;
def OP_mMvSclSrc : AIE2PRegisterOperand<mMvSclSrc>;
def OP_mQEXsa : AIE2PRegisterOperand<mQEXsa>;
def OP_mCRm : AIE2PRegisterOperand<mCRm>;
def OP_mSclSt : AIE2PRegisterOperand<mSclSt>;
def OP_mMcdBMSrc : AIE2PRegisterOperand<mMcdBMSrc>;
def OP_mQXsw : AIE2PRegisterOperand<mQXsw>;
def OP_mEXa : AIE2PRegisterOperand<mEXa>;
def OP_mQEXsb : AIE2PRegisterOperand<mQEXsb>;
def OP_mLdaCg : AIE2PRegisterOperand<mLdaCg>;
def OP_mFl2FxSrc_W : AIE2PRegisterOperand<mFl2FxSrc_W>;
def OP_mXb : AIE2PRegisterOperand<mXb>;
def OP_mBMs : AIE2PRegisterOperand<mBMs>;
def OP_mEXb : AIE2PRegisterOperand<mEXb>;
