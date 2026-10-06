//===-- AIE2PCopyMaterialization.td - AIE2P copy tables -*- tablegen -*-===//
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

def COPY_eD : CopyRecipe<eD, eD, [CopyAllSubRegs]>;
def COPY_eDS : CopyRecipe<eDS, eDS, [CopyAllSubRegs]>;
def COPY_ePSRFLdF : CopyRecipe<ePSRFLdF, ePSRFLdF,
                               [CopyAllSubRegs]>;

def COPY_eL : CopyRecipe<eL, eL, [
  SameSubRegCopy<sub_l_even, MOV_alu_mv_mv_mv_scl>,
  SameSubRegCopy<sub_l_odd, MOV_alu_mv_mv_mv_scl>
]>;
defm COPY_mQQsa : DirectCopy<mQQsa, mQQsa, VMOV_alu_mv_mv_q>;
defm COPY_mWm_mQQsm : DirectCopy<mQQsm, mWm, VMOV_alu_mv_mv_w_to_q>;
defm COPY_mQQsm_mWm : DirectCopy<mWm, mQQsm, VMOV_alu_mv_mv_q_to_w>;
defm COPY_mWm : DirectCopy<mWm, mWm, VMOV_alu_mv_mv_w>;

defm COPY_VEC512_VEC512 : DirectCopy<VEC512, VEC512, VMOV_alu_mv_mv_x>;
defm COPY_VEC512_ACC512 : DirectCopy<ACC512, VEC512, VMOV_alu_mv_mv_x>;
defm COPY_VEC512_FIFO512 : DirectCopy<FIFO512, VEC512, VMOV_alu_mv_mv_x>;
defm COPY_ACC512_VEC512 : DirectCopy<VEC512, ACC512, VMOV_alu_mv_mv_x>;
defm COPY_ACC512_ACC512 : DirectCopy<ACC512, ACC512, VMOV_alu_mv_mv_x>;
defm COPY_ACC512_FIFO512 : DirectCopy<FIFO512, ACC512, VMOV_alu_mv_mv_x>;
defm COPY_FIFO512_VEC512 : DirectCopy<VEC512, FIFO512, VMOV_alu_mv_mv_x>;
defm COPY_FIFO512_ACC512 : DirectCopy<ACC512, FIFO512, VMOV_alu_mv_mv_x>;
defm COPY_FIFO512_FIFO512 : DirectCopy<FIFO512, FIFO512, VMOV_alu_mv_mv_x>;

def COPY_VEC1024 : CopyRecipe<VEC1024, VEC1024, [
  SameSubRegCopy<sub_512_lo, VMOV_alu_mv_mv_x>,
  SameSubRegCopy<sub_512_hi, VMOV_alu_mv_mv_x>
]>;
defm COPY_ACC1024 : DirectCopy<ACC1024, ACC1024, VMOV_alu_mv_mv_cm>;
def COPY_VEC1024_ACC1024 : CopyRecipe<ACC1024, VEC1024, [
  SubRegCopy<sub_512_acc_lo, sub_512_lo, VMOV_alu_mv_mv_x>,
  SubRegCopy<sub_512_acc_hi, sub_512_hi, VMOV_alu_mv_mv_x>
]>;
def COPY_ACC1024_VEC1024 : CopyRecipe<VEC1024, ACC1024, [
  SubRegCopy<sub_512_lo, sub_512_acc_lo, VMOV_alu_mv_mv_x>,
  SubRegCopy<sub_512_hi, sub_512_acc_hi, VMOV_alu_mv_mv_x>
]>;
defm COPY_ACC2048 : DirectCopy<ACC2048, ACC2048, VMOV_D>;
defm COPY_mEXm : DirectCopy<mEXm, mEXm, VMOV_alu_mv_mv_ex>;
def COPY_eEY : CopyRecipe<eEY, eEY, [
  SameSubRegCopy<sub_bfp576_lo, VMOV_alu_mv_mv_ex>,
  SameSubRegCopy<sub_bfp576_hi, VMOV_alu_mv_mv_ex>
]>;
defm COPY_SPARSEVEC640 : DirectCopy<SPARSEVEC640, SPARSEVEC640,
                                    VMOV_alu_mv_mv_qx>;
def COPY_SPARSEVEC1280 : CopyRecipe<SPARSEVEC1280, SPARSEVEC1280, [
  SameSubRegCopy<sub_even_vecmask_640, VMOV_alu_mv_mv_qx>,
  SameSubRegCopy<sub_odd_vecmask_640, VMOV_alu_mv_mv_qx>
]>;
defm COPY_mQEXsm : DirectCopy<mQEXsm, mQEXsm, VMOV_alu_mv_mv_qex>;
def COPY_eQEYs : CopyRecipe<eQEYs, eQEYs, [
  SameSubRegCopy<sub_even_vecmaskexp_704, VMOV_alu_mv_mv_qex>,
  SameSubRegCopy<sub_odd_vecmaskexp_704, VMOV_alu_mv_mv_qex>
]>;

def COPY_FIFO1024 : CopyRecipe<FIFO1024, FIFO1024, [
  SameSubRegCopy<sub_lo_fifo, VMOV_alu_mv_mv_x>,
  SameSubRegCopy<sub_hi_fifo, VMOV_alu_mv_mv_x>
]>;
def COPY_VEC1024_FIFO1024 : CopyRecipe<FIFO1024, VEC1024, [
  SubRegCopy<sub_lo_fifo, sub_512_lo, VMOV_alu_mv_mv_x>,
  SubRegCopy<sub_hi_fifo, sub_512_hi, VMOV_alu_mv_mv_x>
]>;
def COPY_FIFO1024_VEC1024 : CopyRecipe<VEC1024, FIFO1024, [
  SubRegCopy<sub_512_lo, sub_lo_fifo, VMOV_alu_mv_mv_x>,
  SubRegCopy<sub_512_hi, sub_hi_fifo, VMOV_alu_mv_mv_x>
]>;
def COPY_ACC1024_FIFO1024 : CopyRecipe<FIFO1024, ACC1024, [
  SubRegCopy<sub_lo_fifo, sub_512_acc_lo, VMOV_alu_mv_mv_x>,
  SubRegCopy<sub_hi_fifo, sub_512_acc_hi, VMOV_alu_mv_mv_x>
]>;
def COPY_FIFO1024_ACC1024 : CopyRecipe<ACC1024, FIFO1024, [
  SubRegCopy<sub_512_acc_lo, sub_lo_fifo, VMOV_alu_mv_mv_x>,
  SubRegCopy<sub_512_acc_hi, sub_hi_fifo, VMOV_alu_mv_mv_x>
]>;

def COPY_eL_EXPVEC64 : CopyRecipe<EXPVEC64, eL, [
  SubRegCopy<sub_lo_exp, sub_l_even, MOV_alu_mv_mv_mv_e_mv_r_to_el>,
  SubRegCopy<sub_hi_exp, sub_l_odd, MOV_alu_mv_mv_mv_e_mv_r_to_eh>
]>;
def COPY_EXPVEC64_eL : CopyRecipe<eL, EXPVEC64, [
  SubRegCopy<sub_l_even, sub_lo_exp, MOV_alu_mv_mv_mv_e_mv_el_to_r>,
  SubRegCopy<sub_l_odd, sub_hi_exp, MOV_alu_mv_mv_mv_e_mv_eh_to_r>
]>;
def COPY_EXPVEC64 : CopyRecipe<EXPVEC64, EXPVEC64, [
  SameSubRegCopy<sub_lo_exp, MOV_alu_mv_mv_mv_e_mv_el_to_el>,
  SameSubRegCopy<sub_hi_exp, MOV_alu_mv_mv_mv_e_mv_eh_to_eh>
]>;

defm COPY_mEhm : DirectCopy<mEhm, mEhm, MOV_alu_mv_mv_mv_e_mv_eh_to_eh>;
defm COPY_mElm : DirectCopy<mElm, mElm, MOV_alu_mv_mv_mv_e_mv_el_to_el>;
defm COPY_mElm_mEhm : DirectCopy<mEhm, mElm,
                                 MOV_alu_mv_mv_mv_e_mv_el_to_eh>;
defm COPY_mEhm_mElm : DirectCopy<mElm, mEhm,
                                 MOV_alu_mv_mv_mv_e_mv_eh_to_el>;
defm COPY_mEhm_eR : DirectCopy<eR, mEhm, MOV_alu_mv_mv_mv_e_mv_eh_to_r>;
defm COPY_eR_mEhm : DirectCopy<mEhm, eR, MOV_alu_mv_mv_mv_e_mv_r_to_eh>;
defm COPY_mElm_eR : DirectCopy<eR, mElm, MOV_alu_mv_mv_mv_e_mv_el_to_r>;
defm COPY_eR_mElm : DirectCopy<mElm, eR, MOV_alu_mv_mv_mv_e_mv_r_to_el>;
