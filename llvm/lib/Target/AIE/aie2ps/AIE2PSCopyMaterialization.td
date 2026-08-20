//===-- AIE2PSCopyMaterialization.td - AIE2PS copy tables -*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

include "llvm/Target/AIECopyMaterialization.td"

defm COPY_SCL_eR : DirectCopy<eR, eR, MOV_OR_pseudo, 20>;
defm COPY_SCL_AGU : DirectCopy<mAguDst, mAguSrc, MOV_scalar_pseudo, 10>;
defm COPY_SCL : DirectCopy<mMvSclDst, mMvSclSrc,
                           MOV_alu_mv_mv_mv_scl>;

def COPY_eL : CopyRecipe<eL, eL, [CopyAllSubRegs]>;
def COPY_eD : CopyRecipe<eD, eD, [CopyAllSubRegs]>;
def COPY_eDS : CopyRecipe<eDS, eDS, [CopyAllSubRegs]>;
def COPY_ePSRFLdF : CopyRecipe<ePSRFLdF, ePSRFLdF,
                               [CopyAllSubRegs]>;

multiclass RecursiveCopy<RegisterClass dst, RegisterClass src,
    SubRegIndex dstLo, SubRegIndex dstHi, SubRegIndex srcLo, SubRegIndex srcHi> {
  def NAME : CopyRecipe<dst, src, [
    CopyThroughSubRegs<[dstLo, dstHi], [srcLo, srcHi]>
  ]>;
}

defm COPY_eL_EXPVEC64 : RecursiveCopy<EXPVEC64, eL,
    sub_lo_exp, sub_hi_exp, sub_l_even, sub_l_odd>;
defm COPY_EXPVEC64_eL : RecursiveCopy<eL, EXPVEC64,
    sub_l_even, sub_l_odd, sub_lo_exp, sub_hi_exp>;
defm COPY_eL_mGGa : RecursiveCopy<mGGa, eL,
    sub_lo_exp, sub_hi_exp, sub_l_even, sub_l_odd>;
defm COPY_mGGa_eL : RecursiveCopy<eL, mGGa,
    sub_l_even, sub_l_odd, sub_lo_exp, sub_hi_exp>;
defm COPY_mEYw : RecursiveCopy<mEYw, mEYw,
    sub_bfp640_lo, sub_bfp640_hi, sub_bfp640_lo, sub_bfp640_hi>;
defm COPY_mFEYw : RecursiveCopy<mFEYw, mFEYw,
    sub_bfp768_lo, sub_bfp768_hi, sub_bfp768_lo, sub_bfp768_hi>;

defm COPY_VEC1024 : RecursiveCopy<VEC1024, VEC1024,
    sub_512_lo, sub_512_hi, sub_512_lo, sub_512_hi>;
defm COPY_VEC1024_ACC1024 : RecursiveCopy<ACC1024, VEC1024,
    sub_512_acc_lo, sub_512_acc_hi, sub_512_lo, sub_512_hi>;
defm COPY_VEC1024_FIFO1024 : RecursiveCopy<FIFO1024, VEC1024,
    sub_lo_fifo, sub_hi_fifo, sub_512_lo, sub_512_hi>;
defm COPY_ACC1024_VEC1024 : RecursiveCopy<VEC1024, ACC1024,
    sub_512_lo, sub_512_hi, sub_512_acc_lo, sub_512_acc_hi>;
defm COPY_ACC1024_FIFO1024 : RecursiveCopy<FIFO1024, ACC1024,
    sub_lo_fifo, sub_hi_fifo, sub_512_acc_lo, sub_512_acc_hi>;
defm COPY_FIFO1024_VEC1024 : RecursiveCopy<VEC1024, FIFO1024,
    sub_512_lo, sub_512_hi, sub_lo_fifo, sub_hi_fifo>;
defm COPY_FIFO1024_ACC1024 : RecursiveCopy<ACC1024, FIFO1024,
    sub_512_acc_lo, sub_512_acc_hi, sub_lo_fifo, sub_hi_fifo>;
defm COPY_FIFO1024 : RecursiveCopy<FIFO1024, FIFO1024,
    sub_lo_fifo, sub_hi_fifo, sub_lo_fifo, sub_hi_fifo>;

defm COPY_eR_mSCm : DirectCopy<mSCm, eR, MOV_alu_mv_mv_mv_sc_r>;
defm COPY_mSCm_eR : DirectCopy<eR, mSCm, MOV_alu_mv_mv_mv_r_sc>;
defm COPY_mElm_mElm : DirectCopy<mElm, mElm,
                                 MOV_alu_mv_mv_mv_e_mv_el_to_el>;
defm COPY_mEhm_mEhm : DirectCopy<mEhm, mEhm,
                                 MOV_alu_mv_mv_mv_e_mv_eh_to_eh>;
defm COPY_mElm_mEhm : DirectCopy<mEhm, mElm,
                                 MOV_alu_mv_mv_mv_e_mv_el_to_eh>;
defm COPY_mEhm_mElm : DirectCopy<mElm, mEhm,
                                 MOV_alu_mv_mv_mv_e_mv_eh_to_el>;
defm COPY_eR_mEhm : DirectCopy<mEhm, eR, MOV_alu_mv_mv_mv_e_mv_r_to_eh>;
defm COPY_eR_mElm : DirectCopy<mElm, eR, MOV_alu_mv_mv_mv_e_mv_r_to_el>;
defm COPY_mEhm_eR : DirectCopy<eR, mEhm, MOV_alu_mv_mv_mv_e_mv_eh_to_r>;
defm COPY_mElm_eR : DirectCopy<eR, mElm, MOV_alu_mv_mv_mv_e_mv_el_to_r>;
defm COPY_mGlm_mGlm : DirectCopy<mGlm, mGlm,
                                 MOV_alu_mv_mv_mv_g_mv_gl_to_gl>;
defm COPY_mGhm_mGhm : DirectCopy<mGhm, mGhm,
                                 MOV_alu_mv_mv_mv_g_mv_gh_to_gh>;
defm COPY_mGlm_mGhm : DirectCopy<mGhm, mGlm,
                                 MOV_alu_mv_mv_mv_g_mv_gl_to_gh>;
defm COPY_mGhm_mGlm : DirectCopy<mGlm, mGhm,
                                 MOV_alu_mv_mv_mv_g_mv_gh_to_gl>;
defm COPY_eR_mGhm : DirectCopy<mGhm, eR, MOV_alu_mv_mv_mv_g_mv_r_to_gh>;
defm COPY_eR_mGlm : DirectCopy<mGlm, eR, MOV_alu_mv_mv_mv_g_mv_r_to_gl>;
defm COPY_mGhm_eR : DirectCopy<eR, mGhm, MOV_alu_mv_mv_mv_g_mv_gh_to_r>;
defm COPY_mGlm_eR : DirectCopy<eR, mGlm, MOV_alu_mv_mv_mv_g_mv_gl_to_r>;

defm COPY_mFm : DirectCopy<mFm, mFm, VMOV_alu_mv_mv_mv_f>;
defm COPY_mWm : DirectCopy<mWm, mWm, VMOV_alu_mv_mv_mv_w>;
defm COPY_mCMm : DirectCopy<mCMm, mCMm, VMOV_alu_mv_mv_mv_cm>;
defm COPY_mEEm : DirectCopy<mEEm, mEEm, VMOV_alu_mv_mv_mv_ee>;
defm COPY_mEGm : DirectCopy<mEGm, mEGm, VMOV_alu_mv_mv_mv_eg>;
defm COPY_mFFm : DirectCopy<mFFm, mFFm, VMOV_alu_mv_mv_mv_ff>;
defm COPY_mGGm : DirectCopy<mGGm, mGGm, VMOV_alu_mv_mv_mv_gg>;
defm COPY_mEWm : DirectCopy<mEWm, mEWm, VMOV_alu_mv_mv_mv_egw>;
defm COPY_mEXm : DirectCopy<mEXm, mEXm, VMOV_alu_mv_mv_mv_egx>;
defm COPY_mEG2m : DirectCopy<mEG2m, mEG2m, VMOV_alu_mv_mv_mv_eg2>;
defm COPY_mFEWm : DirectCopy<mFEWm, mFEWm, VMOV_alu_mv_mv_mv_few>;
defm COPY_mFEXm : DirectCopy<mFEXm, mFEXm, VMOV_alu_mv_mv_mv_fex>;
defm COPY_mFm_mLm : DirectCopy<mLm, mFm, VMOV_alu_mv_mv_mv_f_to_l>;
defm COPY_mLm_mFm : DirectCopy<mFm, mLm, VMOV_alu_mv_mv_mv_l_to_f>;
defm COPY_mWm_mEG2m : DirectCopy<mEG2m, mWm,
                                  VMOV_alu_mv_mv_mv_w_to_eg2>;
defm COPY_mEG2m_mWm : DirectCopy<mWm, mEG2m,
                                  VMOV_alu_mv_mv_mv_eg2_to_w>;
defm COPY_mMvBMX : DirectCopy<mMvBMXDst, mMvBMXSrc, VMOV_alu_mv_mv_mv_x>;
defm COPY_eDM : DirectCopy<eDM, eDM, VMOV_D_vmov>;
