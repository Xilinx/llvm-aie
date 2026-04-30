//===-AIE2PSGenSchedule_groupedResources.td -----------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//* Automatically generated file, do not edit! *

// Dummy resource to code empty cycles.
// Preferably used through the EmptyCycles class below.
def EMPTY_FU : FuncUnit;
// Insert n empty cycles. Note that we use EMPTY_FU for zero cycles
class EmptyCycles<int n> : InstrStage<0, [EMPTY_FU], n>;

// Convenience class coding one cycle with one resource
class SimpleCycle<FuncUnit FU> : InstrStage<1, [FU]>;

// Used for non-tail resources of a multi-resource cycle
class PrefixCycle<FuncUnit FU> : InstrStage<1, [FU], 0>;

// Partword stores have complicated dependences due to their read-modify-write nature.
// We use this resource to keep other memory ops at a distance.
def PART_WORD_STORE : FuncUnit;
// This resource use is telling there's a partword store in progress.
def IsPartWordStore : InstrStage<7, [PART_WORD_STORE], 0>;
// This is telling other memory ops to stay clear of a part word store.
// A reserved resource is weak, so it blocks only if a part word store is holding it.
def AvoidPartWordStore : InstrStage<1, [PART_WORD_STORE], 0, Reserved>;

 // Resources
def ACC2_SHH : FuncUnit;
def ACC2_SLH : FuncUnit;
def ACC_B_IN : FuncUnit;
def ACC_B_INH : FuncUnit;
def ACC_B_INL : FuncUnit;
def ACC_R : FuncUnit;
def ACC_RH : FuncUnit;
def ACC_RL : FuncUnit;
def ACC_WL : FuncUnit;
def AGU_RM_PORT : FuncUnit;
def BFACC1SHFTH__A : FuncUnit;
def BFACC1SHFTL__A : FuncUnit;
def BFPEXT__A : FuncUnit;
def BFSHCH__ACC1 : FuncUnit;
def BFSHCH__SUBTRACT_ACC : FuncUnit;
def BF_CMAX_S : FuncUnit;
def BM_WM_1 : FuncUnit;
def CNVOSTAT1 : FuncUnit;
def COL : FuncUnit;
def CRBF8CONF_RV : FuncUnit;
def CRF2BMASK_WM_PORT : FuncUnit;
def CRF2FMASK_WM_PORT : FuncUnit;
def CRF2IMASK_WM_PORT : FuncUnit;
def CRFPCNVFL2FXMASK_WM_PORT : FuncUnit;
def CRFPCNVFX2FLMASK_WM_PORT : FuncUnit;
def CRFPMASK_RV : FuncUnit;
def CRFPMASK_WM_PORT : FuncUnit;
def CRFPNLFMASK_WM_PORT : FuncUnit;
def CRMCDEN_RS : FuncUnit;
def CRSAT_RA : FuncUnit;
def CRSHMODEH__DUMMY : FuncUnit;
def CRSHMODEL__DUMMY : FuncUnit;
def CRUPSMODE_RA : FuncUnit;
def DC_RM_PORT : FuncUnit;
def DC_WM_PORT : FuncUnit;
def DJ_RM_PORT : FuncUnit;
def DJ_WM_PORT : FuncUnit;
def DMS_WM : FuncUnit;
def DMX_STS_FIFO : FuncUnit;
def DMX_WRS : FuncUnit;
def DM_ADA : FuncUnit;
def DM_ADS : FuncUnit;
def DM_RM_L0_PORT : FuncUnit;
def DM_RV_L0 : FuncUnit;
def DM_RV_L0_PORT : FuncUnit;
def DM_WA_L0_PORT : FuncUnit;
def DM_WM_L0_PORT : FuncUnit;
def DM_WV_L0_PORT : FuncUnit;
def DN_RM_PORT : FuncUnit;
def DN_WM_PORT : FuncUnit;
def DONE_FU : FuncUnit;
def EVENT_UPS_OF : FuncUnit;
def F2I__MASK : FuncUnit;
def FEY_RS_L0_PORT : FuncUnit;
def FEY_RW_L0_PORT : FuncUnit;
def FEY_WA_L0_PORT : FuncUnit;
def FEY_WB_L0_PORT : FuncUnit;
def FIFO_STS__ADVANCE_FIFO_R : FuncUnit;
def FIFO_STS__I_XH_S : FuncUnit;
def FP32FLAG__AH : FuncUnit;
def FP32NORML__FLAGS : FuncUnit;
def FP32_VOFLAGS_PH_SCL : FuncUnit;
def LCKREQ : FuncUnit;
def LD_FIFO_RA_PORT : FuncUnit;
def LD_FIFO_WA_PORT : FuncUnit;
def LE_WM : FuncUnit;
def LS_WM : FuncUnit;
def L_RS_L0_PORT : FuncUnit;
def L_WA_L0_PORT : FuncUnit;
def L_WM_L0_PORT : FuncUnit;
def M_RM_PORT : FuncUnit;
def M_WM_PORT : FuncUnit;
def PSA_ACCH_IN : FuncUnit;
def PSA_ACCH_IN_S : FuncUnit;
def PSA_ACCL_IN : FuncUnit;
def PSA_ACCL_IN_S : FuncUnit;
def PSA_MMODEH : FuncUnit;
def PSA_RSRC : FuncUnit;
def PSA_SUMH__A : FuncUnit;
def P_RM_PORT : FuncUnit;
def P_WM_PORT : FuncUnit;
def RSRC_EXECUTION_TRACE : FuncUnit;
def SCD : FuncUnit;
def SCD_CTL : FuncUnit;
def SCD_EXPAND_IN : FuncUnit;
def SCD_EXPAND_INHH : FuncUnit;
def SCD_INCR__O_POS : FuncUnit;
def TM_AD : FuncUnit;
def UPS_F__A : FuncUnit;
def UPS_F__A_L : FuncUnit;
def UPS__H__A : FuncUnit;
def UPS__H__OMUXL : FuncUnit;
def UPS__K : FuncUnit;
def UPS__L__K : FuncUnit;
def UPS__L__KL : FuncUnit;
def UPS__L__OMUXL : FuncUnit;
def UPS__OMUX : FuncUnit;
def VCTRL_IN : FuncUnit;
def VCTRL_SUB0_CG1U : FuncUnit;
def X_EGWL : FuncUnit;
def X_EXP_ELL : FuncUnit;
def Y_EGY : FuncUnit;
def Y_EXP_E : FuncUnit;
def ZACC_DML : FuncUnit;

// Bypasses
def MV_Bypass : Bypass;
def VEC_Bypass : Bypass;

// Itineraries
def II_ABS : InstrItinClass;
def II_ACQ_COND_mLockId_imm : InstrItinClass;
def II_ACQ_COND_mLockId_reg : InstrItinClass;
def II_ACQ_mLockId_imm : InstrItinClass;
def II_ACQ_mLockId_reg : InstrItinClass;
def II_ADC : InstrItinClass;
// instruction: ADDM_NC_mv_add_ri
def II_ADDM_NC_mv_add_ri : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eDJ_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_ri_mLRm_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eDC_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eRe_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eVaddSign_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eP_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eUnpackSign_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eSRSSign_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eUPSSign_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eS_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_ri_mLCm_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_ri_ePackSign_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eDN_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_ri_mSPm_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eM_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eRo_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eDJ_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_ri_mLRm_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eDC_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eRe_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eVaddSign_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eP_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eUnpackSign_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eSRSSign_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eUPSSign_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eS_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_ri_mLCm_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_ri_ePackSign_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eDN_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_ri_mSPm_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eM_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_ri_eRo_eRo : InstrItinClass;

// instruction: ADDM_NC_mv_add_rr
def II_ADDM_NC_mv_add_rr : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eDJ_eRe_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_mLRm_eRe_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eDC_eRe_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eRe_eRe_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eVaddSign_eRe_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eP_eRe_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eUnpackSign_eRe_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eSRSSign_eRe_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eUPSSign_eRe_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eS_eRe_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_mLCm_eRe_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_ePackSign_eRe_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eDN_eRe_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_mSPm_eRe_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eM_eRe_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eRo_eRe_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eDJ_eRo_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_mLRm_eRo_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eDC_eRo_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eRe_eRo_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eVaddSign_eRo_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eP_eRo_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eUnpackSign_eRo_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eSRSSign_eRo_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eUPSSign_eRo_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eS_eRo_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_mLCm_eRo_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_ePackSign_eRo_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eDN_eRo_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_mSPm_eRo_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eM_eRo_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eRo_eRo_eRe : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eDJ_eRe_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_mLRm_eRe_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eDC_eRe_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eRe_eRe_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eVaddSign_eRe_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eP_eRe_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eUnpackSign_eRe_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eSRSSign_eRe_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eUPSSign_eRe_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eS_eRe_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_mLCm_eRe_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_ePackSign_eRe_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eDN_eRe_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_mSPm_eRe_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eM_eRe_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eRo_eRe_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eDJ_eRo_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_mLRm_eRo_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eDC_eRo_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eRe_eRo_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eVaddSign_eRo_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eP_eRo_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eUnpackSign_eRo_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eSRSSign_eRo_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eUPSSign_eRo_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eS_eRo_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_mLCm_eRo_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_ePackSign_eRo_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eDN_eRo_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_mSPm_eRo_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eM_eRo_eRo : InstrItinClass;
def II_ADDM_NC_mv_add_rr_eRo_eRo_eRo : InstrItinClass;

def II_ADDM_NC_mv_cg_or_mv_cg_le_rel : InstrItinClass;
def II_ADDM_NC_mv_cg_or_mv_cg_ls_rel : InstrItinClass;
def II_ADDXM_NC_lng_cg_le_rel : InstrItinClass;
def II_ADDXM_NC_lng_cg_ls_rel : InstrItinClass;
def II_ADD_NC_add_lc_ri : InstrItinClass;
def II_ADD_NC_alu_cg_or_alu_cg_le_rel : InstrItinClass;
def II_ADD_NC_alu_cg_or_alu_cg_ls_rel : InstrItinClass;
def II_ADD_add_r_ri : InstrItinClass;
def II_ADD_alu_r_rr : InstrItinClass;
def II_AND : InstrItinClass;
def II_ASHL : InstrItinClass;
def II_CLB : InstrItinClass;
def II_CLZ : InstrItinClass;
def II_DIVS : InstrItinClass;
def II_DONE : InstrItinClass;
def II_EQZ : InstrItinClass;
def II_EQ : InstrItinClass;
def II_EVENT_ERROR : InstrItinClass;
def II_EVENT_WARNING : InstrItinClass;
def II_EVENT_event0 : InstrItinClass;
def II_EVENT_event1 : InstrItinClass;
def II_EXTEND_s16 : InstrItinClass;
def II_EXTEND_s8 : InstrItinClass;
def II_EXTEND_u16 : InstrItinClass;
def II_EXTEND_u8 : InstrItinClass;
def II_GEU : InstrItinClass;
def II_GE : InstrItinClass;
def II_INVSQRT_nlf_invsqrt : InstrItinClass;
def II_INVSQRT_nlf_invsqrt_mOptConvDel_mS3 : InstrItinClass;
def II_INVSQRT_nlf_invsqrt_mOptConvDel_mS3_mOptConv : InstrItinClass;
def II_INVSQRT_nlf_invsqrt_mOptConv : InstrItinClass;
def II_INV_nlf_inv : InstrItinClass;
def II_INV_nlf_inv_mOptConvDel_mS3 : InstrItinClass;
def II_INV_nlf_inv_mOptConvDel_mS3_mOptConv : InstrItinClass;
def II_INV_nlf_inv_mOptConv : InstrItinClass;
def II_JL_alumv_or : InstrItinClass;
def II_JL_lng : InstrItinClass;
def II_JNZD : InstrItinClass;
def II_JNZ : InstrItinClass;
def II_JZ : InstrItinClass;
def II_J_alumv_or : InstrItinClass;
def II_J_lng : InstrItinClass;
def II_LDA_2D_dml_lda2_scalar_EE : InstrItinClass;
def II_LDA_2D_dml_lda2_scalar_EG : InstrItinClass;
def II_LDA_2D_dml_lda2_scalar_GG : InstrItinClass;
def II_LDA_2D_dml_lda_scalar_F : InstrItinClass;
def II_LDA_2D_dml_lda_scalar_L : InstrItinClass;
def II_LDA_2D_dms_lda_g : InstrItinClass;
// instruction: LDA_2D_dms_lda_scalar
def II_LDA_2D_dms_lda_scalar : InstrItinClass;
def II_LDA_2D_dms_lda_scalar_mLRa : InstrItinClass;
def II_LDA_2D_dms_lda_scalar_eEle : InstrItinClass;
def II_LDA_2D_dms_lda_scalar_eElo : InstrItinClass;
def II_LDA_2D_dms_lda_scalar_eDJ : InstrItinClass;
def II_LDA_2D_dms_lda_scalar_eDC : InstrItinClass;
def II_LDA_2D_dms_lda_scalar_eRe : InstrItinClass;
def II_LDA_2D_dms_lda_scalar_eDN : InstrItinClass;
def II_LDA_2D_dms_lda_scalar_eEhe : InstrItinClass;
def II_LDA_2D_dms_lda_scalar_eP : InstrItinClass;
def II_LDA_2D_dms_lda_scalar_eM : InstrItinClass;
def II_LDA_2D_dms_lda_scalar_eRo : InstrItinClass;
def II_LDA_2D_dms_lda_scalar_eEho : InstrItinClass;

def II_LDA_2D_dmv_lda_eg2 : InstrItinClass;
def II_LDA_2D_dmv_lda_f : InstrItinClass;
def II_LDA_2D_dmv_lda_feg : InstrItinClass;
def II_LDA_2D_s16 : InstrItinClass;
def II_LDA_2D_s8 : InstrItinClass;
def II_LDA_2D_u16 : InstrItinClass;
def II_LDA_2D_u8 : InstrItinClass;
def II_LDA_3D_dml_lda2_scalar_EE : InstrItinClass;
def II_LDA_3D_dml_lda2_scalar_EG : InstrItinClass;
def II_LDA_3D_dml_lda2_scalar_GG : InstrItinClass;
def II_LDA_3D_dml_lda_scalar_F : InstrItinClass;
def II_LDA_3D_dml_lda_scalar_L : InstrItinClass;
def II_LDA_3D_dms_lda_g : InstrItinClass;
// instruction: LDA_3D_dms_lda_scalar
def II_LDA_3D_dms_lda_scalar : InstrItinClass;
def II_LDA_3D_dms_lda_scalar_mLRa : InstrItinClass;
def II_LDA_3D_dms_lda_scalar_eEle : InstrItinClass;
def II_LDA_3D_dms_lda_scalar_eElo : InstrItinClass;
def II_LDA_3D_dms_lda_scalar_eDJ : InstrItinClass;
def II_LDA_3D_dms_lda_scalar_eDC : InstrItinClass;
def II_LDA_3D_dms_lda_scalar_eRe : InstrItinClass;
def II_LDA_3D_dms_lda_scalar_eDN : InstrItinClass;
def II_LDA_3D_dms_lda_scalar_eEhe : InstrItinClass;
def II_LDA_3D_dms_lda_scalar_eP : InstrItinClass;
def II_LDA_3D_dms_lda_scalar_eM : InstrItinClass;
def II_LDA_3D_dms_lda_scalar_eRo : InstrItinClass;
def II_LDA_3D_dms_lda_scalar_eEho : InstrItinClass;

def II_LDA_3D_dmv_lda_eg2 : InstrItinClass;
def II_LDA_3D_dmv_lda_f : InstrItinClass;
def II_LDA_3D_dmv_lda_feg : InstrItinClass;
def II_LDA_3D_s16 : InstrItinClass;
def II_LDA_3D_s8 : InstrItinClass;
def II_LDA_3D_u16 : InstrItinClass;
def II_LDA_3D_u8 : InstrItinClass;
def II_LDA_TM_2D : InstrItinClass;
def II_LDA_TM_3D : InstrItinClass;
def II_LDA_TM_idx : InstrItinClass;
def II_LDA_TM_idx_imm : InstrItinClass;
def II_LDA_TM_pstm_nrm : InstrItinClass;
def II_LDA_TM_pstm_nrm_imm : InstrItinClass;
def II_LDA_dml_lda2_scalar_EE_ld_idx : InstrItinClass;
def II_LDA_dml_lda2_scalar_EE_ld_idx_imm : InstrItinClass;
def II_LDA_dml_lda2_scalar_EE_ld_pstm_nrm : InstrItinClass;
def II_LDA_dml_lda2_scalar_EE_ld_pstm_nrm_imm : InstrItinClass;
def II_LDA_dml_lda2_scalar_EE_spill : InstrItinClass;
def II_LDA_dml_lda2_scalar_EG_ld_idx : InstrItinClass;
def II_LDA_dml_lda2_scalar_EG_ld_idx_imm : InstrItinClass;
def II_LDA_dml_lda2_scalar_EG_ld_pstm_nrm : InstrItinClass;
def II_LDA_dml_lda2_scalar_EG_ld_pstm_nrm_imm : InstrItinClass;
def II_LDA_dml_lda2_scalar_EG_spill : InstrItinClass;
def II_LDA_dml_lda2_scalar_GG_ld_idx : InstrItinClass;
def II_LDA_dml_lda2_scalar_GG_ld_idx_imm : InstrItinClass;
def II_LDA_dml_lda2_scalar_GG_ld_pstm_nrm : InstrItinClass;
def II_LDA_dml_lda2_scalar_GG_ld_pstm_nrm_imm : InstrItinClass;
def II_LDA_dml_lda2_scalar_GG_spill : InstrItinClass;
def II_LDA_dml_lda_scalar_F_ld_idx : InstrItinClass;
def II_LDA_dml_lda_scalar_F_ld_idx_imm : InstrItinClass;
def II_LDA_dml_lda_scalar_F_ld_pstm_nrm : InstrItinClass;
def II_LDA_dml_lda_scalar_F_ld_pstm_nrm_imm : InstrItinClass;
def II_LDA_dml_lda_scalar_F_spill : InstrItinClass;
def II_LDA_dml_lda_scalar_L_ld_idx : InstrItinClass;
def II_LDA_dml_lda_scalar_L_ld_idx_imm : InstrItinClass;
def II_LDA_dml_lda_scalar_L_ld_pstm_nrm : InstrItinClass;
def II_LDA_dml_lda_scalar_L_ld_pstm_nrm_imm : InstrItinClass;
def II_LDA_dml_lda_scalar_L_spill : InstrItinClass;
def II_LDA_dms_lda_g_ld_idx : InstrItinClass;
def II_LDA_dms_lda_g_ld_idx_imm : InstrItinClass;
def II_LDA_dms_lda_g_ld_pstm_nrm : InstrItinClass;
def II_LDA_dms_lda_g_ld_pstm_nrm_imm : InstrItinClass;
def II_LDA_dms_lda_g_spill : InstrItinClass;
// instruction: LDA_dms_lda_scalar_ld_idx
def II_LDA_dms_lda_scalar_ld_idx : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_mLRa : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_eEle : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_eElo : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_eDJ : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_eDC : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_eRe : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_eDN : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_eEhe : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_eP : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_eM : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_eRo : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_eEho : InstrItinClass;

// instruction: LDA_dms_lda_scalar_ld_idx_imm
def II_LDA_dms_lda_scalar_ld_idx_imm : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_imm_mLRa : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_imm_eEle : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_imm_eElo : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_imm_eDJ : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_imm_eDC : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_imm_eRe : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_imm_eDN : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_imm_eEhe : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_imm_eP : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_imm_eM : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_imm_eRo : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_idx_imm_eEho : InstrItinClass;

// instruction: LDA_dms_lda_scalar_ld_pstm_nrm
def II_LDA_dms_lda_scalar_ld_pstm_nrm : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_mLRa : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_eEle : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_eElo : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_eDJ : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_eDC : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_eRe : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_eDN : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_eEhe : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_eP : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_eM : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_eRo : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_eEho : InstrItinClass;

// instruction: LDA_dms_lda_scalar_ld_pstm_nrm_imm
def II_LDA_dms_lda_scalar_ld_pstm_nrm_imm : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_mLRa : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eEle : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eElo : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eDJ : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eDC : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eRe : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eDN : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eEhe : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eP : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eM : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eRo : InstrItinClass;
def II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eEho : InstrItinClass;

// instruction: LDA_dms_lda_scalar_spill
def II_LDA_dms_lda_scalar_spill : InstrItinClass;
def II_LDA_dms_lda_scalar_spill_mLRa : InstrItinClass;
def II_LDA_dms_lda_scalar_spill_eEle : InstrItinClass;
def II_LDA_dms_lda_scalar_spill_eElo : InstrItinClass;
def II_LDA_dms_lda_scalar_spill_eDJ : InstrItinClass;
def II_LDA_dms_lda_scalar_spill_eDC : InstrItinClass;
def II_LDA_dms_lda_scalar_spill_eRe : InstrItinClass;
def II_LDA_dms_lda_scalar_spill_eDN : InstrItinClass;
def II_LDA_dms_lda_scalar_spill_eEhe : InstrItinClass;
def II_LDA_dms_lda_scalar_spill_eP : InstrItinClass;
def II_LDA_dms_lda_scalar_spill_eM : InstrItinClass;
def II_LDA_dms_lda_scalar_spill_eRo : InstrItinClass;
def II_LDA_dms_lda_scalar_spill_eEho : InstrItinClass;

def II_LDA_dmv_lda_eg2_ld_idx : InstrItinClass;
def II_LDA_dmv_lda_eg2_ld_idx_imm : InstrItinClass;
def II_LDA_dmv_lda_eg2_ld_pstm_nrm : InstrItinClass;
def II_LDA_dmv_lda_eg2_ld_pstm_nrm_imm : InstrItinClass;
def II_LDA_dmv_lda_eg2_spill : InstrItinClass;
def II_LDA_dmv_lda_f_ld_idx : InstrItinClass;
def II_LDA_dmv_lda_f_ld_idx_imm : InstrItinClass;
def II_LDA_dmv_lda_f_ld_pstm_nrm : InstrItinClass;
def II_LDA_dmv_lda_f_ld_pstm_nrm_imm : InstrItinClass;
def II_LDA_dmv_lda_f_spill : InstrItinClass;
def II_LDA_dmv_lda_feg_ld_idx : InstrItinClass;
def II_LDA_dmv_lda_feg_ld_idx_imm : InstrItinClass;
def II_LDA_dmv_lda_feg_ld_pstm_nrm : InstrItinClass;
def II_LDA_dmv_lda_feg_ld_pstm_nrm_imm : InstrItinClass;
def II_LDA_dmv_lda_feg_spill : InstrItinClass;
def II_LDA_s16_idx : InstrItinClass;
def II_LDA_s16_idx_imm : InstrItinClass;
def II_LDA_s16_pstm_nrm : InstrItinClass;
def II_LDA_s16_pstm_nrm_imm : InstrItinClass;
def II_LDA_s8_idx : InstrItinClass;
def II_LDA_s8_idx_imm : InstrItinClass;
def II_LDA_s8_pstm_nrm : InstrItinClass;
def II_LDA_s8_pstm_nrm_imm : InstrItinClass;
def II_LDA_u16_idx : InstrItinClass;
def II_LDA_u16_idx_imm : InstrItinClass;
def II_LDA_u16_pstm_nrm : InstrItinClass;
def II_LDA_u16_pstm_nrm_imm : InstrItinClass;
def II_LDA_u8_idx : InstrItinClass;
def II_LDA_u8_idx_imm : InstrItinClass;
def II_LDA_u8_pstm_nrm : InstrItinClass;
def II_LDA_u8_pstm_nrm_imm : InstrItinClass;
def II_LSHL : InstrItinClass;
def II_LTU : InstrItinClass;
def II_LT : InstrItinClass;
def II_MAC : InstrItinClass;
def II_MAXU : InstrItinClass;
def II_MAX : InstrItinClass;
def II_MINU : InstrItinClass;
def II_MIN : InstrItinClass;
// instruction: MOVA
def II_MOVA : InstrItinClass;
def II_MOVA_eDJ : InstrItinClass;
def II_MOVA_mLCaCg : InstrItinClass;
def II_MOVA_eDC : InstrItinClass;
def II_MOVA_eRe : InstrItinClass;
def II_MOVA_eDN : InstrItinClass;
def II_MOVA_eP : InstrItinClass;
def II_MOVA_eM : InstrItinClass;
def II_MOVA_eRo : InstrItinClass;

// instruction: MOVS
def II_MOVS : InstrItinClass;
def II_MOVS_eDJ_eDJ : InstrItinClass;
def II_MOVS_eDC_eDJ : InstrItinClass;
def II_MOVS_eDN_eDJ : InstrItinClass;
def II_MOVS_eP_eDJ : InstrItinClass;
def II_MOVS_eM_eDJ : InstrItinClass;
def II_MOVS_eDJ_eDC : InstrItinClass;
def II_MOVS_eDJ_eDN : InstrItinClass;
def II_MOVS_eDJ_eRe : InstrItinClass;
def II_MOVS_eDJ_eP : InstrItinClass;
def II_MOVS_eDJ_eM : InstrItinClass;
def II_MOVS_eDJ_eRo : InstrItinClass;
def II_MOVS_eDC_eDC : InstrItinClass;
def II_MOVS_eDC_eDN : InstrItinClass;
def II_MOVS_eDC_eRe : InstrItinClass;
def II_MOVS_eDC_eP : InstrItinClass;
def II_MOVS_eDC_eM : InstrItinClass;
def II_MOVS_eDC_eRo : InstrItinClass;
def II_MOVS_eDN_eDC : InstrItinClass;
def II_MOVS_eDN_eDN : InstrItinClass;
def II_MOVS_eDN_eRe : InstrItinClass;
def II_MOVS_eDN_eP : InstrItinClass;
def II_MOVS_eDN_eM : InstrItinClass;
def II_MOVS_eDN_eRo : InstrItinClass;
def II_MOVS_eP_eDC : InstrItinClass;
def II_MOVS_eP_eDN : InstrItinClass;
def II_MOVS_eP_eRe : InstrItinClass;
def II_MOVS_eP_eP : InstrItinClass;
def II_MOVS_eP_eM : InstrItinClass;
def II_MOVS_eP_eRo : InstrItinClass;
def II_MOVS_eM_eDC : InstrItinClass;
def II_MOVS_eM_eDN : InstrItinClass;
def II_MOVS_eM_eRe : InstrItinClass;
def II_MOVS_eM_eP : InstrItinClass;
def II_MOVS_eM_eM : InstrItinClass;
def II_MOVS_eM_eRo : InstrItinClass;

// instruction: MOVXM_lng_cg
def II_MOVXM_lng_cg : InstrItinClass;
def II_MOVXM_lng_cg_eDJ : InstrItinClass;
def II_MOVXM_lng_cg_mLRm : InstrItinClass;
def II_MOVXM_lng_cg_eDC : InstrItinClass;
def II_MOVXM_lng_cg_eRe : InstrItinClass;
def II_MOVXM_lng_cg_mLCmCg : InstrItinClass;
def II_MOVXM_lng_cg_eVaddSign : InstrItinClass;
def II_MOVXM_lng_cg_eP : InstrItinClass;
def II_MOVXM_lng_cg_eUnpackSign : InstrItinClass;
def II_MOVXM_lng_cg_eSRSSign : InstrItinClass;
def II_MOVXM_lng_cg_eUPSSign : InstrItinClass;
def II_MOVXM_lng_cg_eS : InstrItinClass;
def II_MOVXM_lng_cg_ePackSign : InstrItinClass;
def II_MOVXM_lng_cg_eDN : InstrItinClass;
def II_MOVXM_lng_cg_mSPm : InstrItinClass;
def II_MOVXM_lng_cg_eM : InstrItinClass;
def II_MOVXM_lng_cg_eRo : InstrItinClass;

def II_MOVXM_lng_cg_le_abs : InstrItinClass;
def II_MOVXM_lng_cg_ls_abs : InstrItinClass;
def II_MOVX_alu_cg_or : InstrItinClass;
// instruction: MOVX_mvx_cr_imm
def II_MOVX_mvx_cr_imm : InstrItinClass;
def II_MOVX_mvx_cr_imm_mCRFPNlfMask : InstrItinClass;
def II_MOVX_mvx_cr_imm_mCRSCDEn : InstrItinClass;
def II_MOVX_mvx_cr_imm_mCRPackSize : InstrItinClass;
def II_MOVX_mvx_cr_imm_mCRRnd : InstrItinClass;
def II_MOVX_mvx_cr_imm_mCRSat : InstrItinClass;
def II_MOVX_mvx_cr_imm_mCRFPMask : InstrItinClass;
def II_MOVX_mvx_cr_imm_mCRF2IMask : InstrItinClass;
def II_MOVX_mvx_cr_imm_mCRF2FMask : InstrItinClass;
def II_MOVX_mvx_cr_imm_eVaddSign : InstrItinClass;
def II_MOVX_mvx_cr_imm_mCRF2BMask : InstrItinClass;
def II_MOVX_mvx_cr_imm_mCRFPCnvFl2FxMask : InstrItinClass;
def II_MOVX_mvx_cr_imm_eSRSSign : InstrItinClass;
def II_MOVX_mvx_cr_imm_eUnpackSign : InstrItinClass;
def II_MOVX_mvx_cr_imm_mCRFPConvSat : InstrItinClass;
def II_MOVX_mvx_cr_imm_mCRUPSMode : InstrItinClass;
def II_MOVX_mvx_cr_imm_mCRBF8conf : InstrItinClass;
def II_MOVX_mvx_cr_imm_eUPSSign : InstrItinClass;
def II_MOVX_mvx_cr_imm_mCRUnpackSize : InstrItinClass;
def II_MOVX_mvx_cr_imm_ePackSign : InstrItinClass;
def II_MOVX_mvx_cr_imm_mCRMCDEn : InstrItinClass;
def II_MOVX_mvx_cr_imm_mCRFPCnvFx2FlMask : InstrItinClass;
def II_MOVX_mvx_cr_imm_mCRFP8conf : InstrItinClass;
def II_MOVX_mvx_cr_imm_mCRSRSMode : InstrItinClass;

// instruction: MOVX_mvx_cr_r
def II_MOVX_mvx_cr_r : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRFPNlfMask_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRSCDEn_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRPackSize_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRRnd_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRSat_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRFPMask_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRF2IMask_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRF2FMask_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_eVaddSign_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRF2BMask_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRFPCnvFl2FxMask_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_eSRSSign_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_eUnpackSign_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRFPConvSat_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRUPSMode_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRBF8conf_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_eUPSSign_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRUnpackSize_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_ePackSign_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRMCDEn_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRFPCnvFx2FlMask_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRFP8conf_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRSRSMode_eRe : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRFPNlfMask_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRSCDEn_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRPackSize_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRRnd_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRSat_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRFPMask_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRF2IMask_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRF2FMask_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_eVaddSign_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRF2BMask_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRFPCnvFl2FxMask_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_eSRSSign_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_eUnpackSign_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRFPConvSat_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRUPSMode_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRBF8conf_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_eUPSSign_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRUnpackSize_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_ePackSign_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRMCDEn_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRFPCnvFx2FlMask_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRFP8conf_eRo : InstrItinClass;
def II_MOVX_mvx_cr_r_mCRSRSMode_eRo : InstrItinClass;

def II_MOV_CPH_mMStream_tlast_imm : InstrItinClass;
def II_MOV_CPH_mMStream_tlast_reg : InstrItinClass;
def II_MOV_CPH_nb_mMStream_tlast_imm : InstrItinClass;
def II_MOV_CPH_nb_mMStream_tlast_reg : InstrItinClass;
def II_MOV_CPH_nb_tlast : InstrItinClass;
def II_MOV_CPH_tlast : InstrItinClass;
def II_MOV_PH_mMStream_tlast_imm : InstrItinClass;
def II_MOV_PH_mMStream_tlast_reg : InstrItinClass;
def II_MOV_PH_nb_mMStream_tlast_imm : InstrItinClass;
def II_MOV_PH_nb_mMStream_tlast_reg : InstrItinClass;
def II_MOV_PH_nb_tlast : InstrItinClass;
def II_MOV_PH_tlast : InstrItinClass;
def II_MOV_alu_mv_alu_flt2fx : InstrItinClass;
def II_MOV_alu_mv_alu_fx2flt : InstrItinClass;
// instruction: MOV_alu_mv_mv_mv_cg_or
def II_MOV_alu_mv_mv_mv_cg_or : InstrItinClass;
def II_MOV_alu_mv_mv_mv_cg_or_eDJ : InstrItinClass;
def II_MOV_alu_mv_mv_mv_cg_or_mLRm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_cg_or_eDC : InstrItinClass;
def II_MOV_alu_mv_mv_mv_cg_or_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_cg_or_mLCmCg : InstrItinClass;
def II_MOV_alu_mv_mv_mv_cg_or_eVaddSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_cg_or_eP : InstrItinClass;
def II_MOV_alu_mv_mv_mv_cg_or_eUnpackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_cg_or_eSRSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_cg_or_eUPSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_cg_or_eS : InstrItinClass;
def II_MOV_alu_mv_mv_mv_cg_or_ePackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_cg_or_eDN : InstrItinClass;
def II_MOV_alu_mv_mv_mv_cg_or_mSPm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_cg_or_eM : InstrItinClass;
def II_MOV_alu_mv_mv_mv_cg_or_eRo : InstrItinClass;

def II_MOV_alu_mv_mv_mv_cntr2l : InstrItinClass;
def II_MOV_alu_mv_mv_mv_e_mv_eh_to_eh : InstrItinClass;
def II_MOV_alu_mv_mv_mv_e_mv_eh_to_el : InstrItinClass;
def II_MOV_alu_mv_mv_mv_e_mv_eh_to_r : InstrItinClass;
def II_MOV_alu_mv_mv_mv_e_mv_el_to_eh : InstrItinClass;
def II_MOV_alu_mv_mv_mv_e_mv_el_to_el : InstrItinClass;
def II_MOV_alu_mv_mv_mv_e_mv_el_to_r : InstrItinClass;
def II_MOV_alu_mv_mv_mv_e_mv_r_to_eh : InstrItinClass;
def II_MOV_alu_mv_mv_mv_e_mv_r_to_el : InstrItinClass;
def II_MOV_alu_mv_mv_mv_g_mv_gh_to_gh : InstrItinClass;
def II_MOV_alu_mv_mv_mv_g_mv_gh_to_gl : InstrItinClass;
def II_MOV_alu_mv_mv_mv_g_mv_gh_to_r : InstrItinClass;
def II_MOV_alu_mv_mv_mv_g_mv_gl_to_gh : InstrItinClass;
def II_MOV_alu_mv_mv_mv_g_mv_gl_to_gl : InstrItinClass;
def II_MOV_alu_mv_mv_mv_g_mv_gl_to_r : InstrItinClass;
def II_MOV_alu_mv_mv_mv_g_mv_r_to_gh : InstrItinClass;
def II_MOV_alu_mv_mv_mv_g_mv_r_to_gl : InstrItinClass;
def II_MOV_alu_mv_mv_mv_r_sc : InstrItinClass;
// instruction: MOV_alu_mv_mv_mv_sc_cg
def II_MOV_alu_mv_mv_mv_sc_cg : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mSRF2IFlags : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mCRFPNlfMask : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mSRFPCnvFl2Fx : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mSRFifo_of : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mCRPackSize : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mCRRnd : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mSRCarry : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mCRF2IMask : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mCRF2FMask : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mCRF2BMask : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mSRF2FFlags : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mSRFPCnvFx2Fl : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mCRFPConvSat : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mCRBF8conf : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mSRFifo_uf : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mCRMCDEn : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mCRFPCnvFx2FlMask : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mSRMS0 : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mSRUPS_of : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mSRSparse_of : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mCRSRSMode : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mCRSCDEn : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mCRSat : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mCRFPMask : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mCRFPCnvFl2FxMask : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mSRSRS_of : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mSRFPFlags : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mSRF2BFlags : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mCRUPSMode : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mSRFPNlf : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mCRUnpackSize : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mSRSS0 : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_cg_mCRFP8conf : InstrItinClass;

// instruction: MOV_alu_mv_mv_mv_sc_r
def II_MOV_alu_mv_mv_mv_sc_r : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRF2IFlags_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRFPNlfMask_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRFPCnvFl2Fx_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRFifo_of_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRPackSize_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRRnd_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRCarry_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRF2IMask_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRF2FMask_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRF2BMask_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRF2FFlags_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRFPCnvFx2Fl_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRFPConvSat_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRBF8conf_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRFifo_uf_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRMCDEn_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRFPCnvFx2FlMask_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRMS0_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRUPS_of_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRSparse_of_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRSRSMode_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRSCDEn_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRSat_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRFPMask_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRFPCnvFl2FxMask_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRSRS_of_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRFPFlags_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRF2BFlags_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRUPSMode_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRFPNlf_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRUnpackSize_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRSS0_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRFP8conf_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRF2IFlags_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRFPNlfMask_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRFPCnvFl2Fx_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRFifo_of_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRPackSize_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRRnd_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRCarry_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRF2IMask_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRF2FMask_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRF2BMask_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRF2FFlags_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRFPCnvFx2Fl_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRFPConvSat_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRBF8conf_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRFifo_uf_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRMCDEn_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRFPCnvFx2FlMask_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRMS0_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRUPS_of_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRSparse_of_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRSRSMode_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRSCDEn_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRSat_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRFPMask_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRFPCnvFl2FxMask_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRSRS_of_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRFPFlags_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRF2BFlags_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRUPSMode_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRFPNlf_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRUnpackSize_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mSRSS0_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_sc_r_mCRFP8conf_eRo : InstrItinClass;

// instruction: MOV_alu_mv_mv_mv_scl
def II_MOV_alu_mv_mv_mv_scl : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_mLEm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_mLEm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_mLEm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_mLEm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_mLEm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_mLEm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mLEm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_mLEm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_mLEm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_mLEm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_mLEm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_mLEm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_mLEm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_mLEm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_mLEm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_mLEm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_eDJ : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_mLRm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_eDC : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_mLSm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_eVaddSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_eP : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_eUnpackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_eSRSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_mCoreID : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_eUPSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_eS : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_mLCm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_ePackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_eDN : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_mSPm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_eM : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDJ_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_eDJ : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_mLRm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_eDC : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_mLSm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_eVaddSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_eP : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_eUnpackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_eSRSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_mCoreID : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_eUPSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_eS : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_mLCm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_ePackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_eDN : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_mSPm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_eM : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLRm_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_eDJ : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_mLRm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_eDC : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_mLSm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_eVaddSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_eP : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_eUnpackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_eSRSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_mCoreID : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_eUPSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_eS : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_mLCm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_ePackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_eDN : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_mSPm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_eM : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDC_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_eDJ : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_mLRm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_eDC : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_mLSm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_eVaddSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_eP : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_eUnpackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_eSRSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_mCoreID : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_eUPSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_eS : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_mLCm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_ePackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_eDN : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_mSPm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_eM : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRe_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_eDJ : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_mLRm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_eDC : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_mLSm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_eVaddSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_eP : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_eUnpackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_eSRSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_mCoreID : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_eUPSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_eS : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_mLCm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_ePackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_eDN : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSPm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_eM : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eVaddSign_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_eDJ : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_mLRm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_eDC : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_mLSm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_eVaddSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_eP : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_eUnpackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_eSRSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_mCoreID : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_eUPSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_eS : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_mLCm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_ePackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_eDN : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_mSPm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_eM : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eP_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eDJ : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mLRm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eDC : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mLSm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eVaddSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eP : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eUnpackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eSRSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mCoreID : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eUPSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eS : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mLCm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_ePackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eDN : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSPm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eM : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_eDJ : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_mLRm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_eDC : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_mLSm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_eVaddSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_eP : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_eUnpackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_eSRSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_mCoreID : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_eUPSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_eS : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_mLCm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_ePackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_eDN : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSPm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_eM : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eSRSSign_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_eDJ : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_mLRm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_eDC : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_mLSm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_eVaddSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_eP : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_eUnpackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_eSRSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_mCoreID : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_eUPSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_eS : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_mLCm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_ePackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_eDN : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSPm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_eM : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eUPSSign_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_eDJ : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_mLRm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_eDC : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_mLSm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_eVaddSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_eP : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_eUnpackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_eSRSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_mCoreID : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_eUPSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_eS : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_mLCm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_ePackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_eDN : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_mSPm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_eM : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eS_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_eDJ : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_mLRm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_eDC : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_mLSm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_eVaddSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_eP : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_eUnpackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_eSRSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_mCoreID : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_eUPSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_eS : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_mLCm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_ePackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_eDN : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_mSPm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_eM : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mLCm_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_eDJ : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_mLRm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_eDC : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_mLSm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_eVaddSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_eP : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_eUnpackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_eSRSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_mCoreID : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_eUPSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_eS : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_mLCm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_ePackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_eDN : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_mSPm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_eM : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_ePackSign_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_eDJ : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_mLRm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_eDC : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_mLSm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_eVaddSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_eP : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_eUnpackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_eSRSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_mCoreID : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_eUPSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_eS : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_mLCm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_ePackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_eDN : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_mSPm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_eM : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eDN_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_eDJ : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_mLRm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_eDC : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_mLSm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_eVaddSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_eP : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_eUnpackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_eSRSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_mCoreID : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_eUPSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_eS : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_mLCm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_ePackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_eDN : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_mSPm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_eM : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_mSPm_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_eDJ : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_mLRm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_eDC : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_mLSm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_eVaddSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_eP : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_eUnpackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_eSRSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_mCoreID : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_eUPSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_eS : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_mLCm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_ePackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_eDN : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_mSPm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_eM : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eM_eRo : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_eDJ : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_mLRm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_eDC : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_eRe : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_mLSm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_eVaddSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_eP : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_eUnpackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_eSRSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_mCoreID : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_eUPSSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_eS : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_mLCm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_ePackSign : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_eDN : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_mSPm : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_eM : InstrItinClass;
def II_MOV_alu_mv_mv_mv_scl_eRo_eRo : InstrItinClass;

def II_MOV_lda : InstrItinClass;
// instruction: MOV_st_mMStream_tlast_imm
def II_MOV_st_mMStream_tlast_imm : InstrItinClass;
def II_MOV_st_mMStream_tlast_imm_eDJ : InstrItinClass;
def II_MOV_st_mMStream_tlast_imm_eDC : InstrItinClass;
def II_MOV_st_mMStream_tlast_imm_eRe : InstrItinClass;
def II_MOV_st_mMStream_tlast_imm_eDN : InstrItinClass;
def II_MOV_st_mMStream_tlast_imm_eP : InstrItinClass;
def II_MOV_st_mMStream_tlast_imm_eM : InstrItinClass;
def II_MOV_st_mMStream_tlast_imm_eRo : InstrItinClass;

// instruction: MOV_st_mMStream_tlast_reg
def II_MOV_st_mMStream_tlast_reg : InstrItinClass;
def II_MOV_st_mMStream_tlast_reg_eDJ : InstrItinClass;
def II_MOV_st_mMStream_tlast_reg_eDC : InstrItinClass;
def II_MOV_st_mMStream_tlast_reg_eRe : InstrItinClass;
def II_MOV_st_mMStream_tlast_reg_eDN : InstrItinClass;
def II_MOV_st_mMStream_tlast_reg_eP : InstrItinClass;
def II_MOV_st_mMStream_tlast_reg_eM : InstrItinClass;
def II_MOV_st_mMStream_tlast_reg_eRo : InstrItinClass;

// instruction: MOV_d1
def II_MOV_d1 : InstrItinClass;
def II_MOV_d1_eRe_mLEm : InstrItinClass;
def II_MOV_d1_eRo_mLEm : InstrItinClass;
def II_MOV_d1_eRe_eDJ : InstrItinClass;
def II_MOV_d1_eRe_mLRm : InstrItinClass;
def II_MOV_d1_eRe_eDC : InstrItinClass;
def II_MOV_d1_eRe_eRe : InstrItinClass;
def II_MOV_d1_eRe_mLSm : InstrItinClass;
def II_MOV_d1_eRe_eVaddSign : InstrItinClass;
def II_MOV_d1_eRe_eP : InstrItinClass;
def II_MOV_d1_eRe_eUnpackSign : InstrItinClass;
def II_MOV_d1_eRe_eSRSSign : InstrItinClass;
def II_MOV_d1_eRe_mCoreID : InstrItinClass;
def II_MOV_d1_eRe_eUPSSign : InstrItinClass;
def II_MOV_d1_eRe_eS : InstrItinClass;
def II_MOV_d1_eRe_mLCm : InstrItinClass;
def II_MOV_d1_eRe_ePackSign : InstrItinClass;
def II_MOV_d1_eRe_eDN : InstrItinClass;
def II_MOV_d1_eRe_mSPm : InstrItinClass;
def II_MOV_d1_eRe_eM : InstrItinClass;
def II_MOV_d1_eRe_eRo : InstrItinClass;
def II_MOV_d1_eRo_eDJ : InstrItinClass;
def II_MOV_d1_eRo_mLRm : InstrItinClass;
def II_MOV_d1_eRo_eDC : InstrItinClass;
def II_MOV_d1_eRo_eRe : InstrItinClass;
def II_MOV_d1_eRo_mLSm : InstrItinClass;
def II_MOV_d1_eRo_eVaddSign : InstrItinClass;
def II_MOV_d1_eRo_eP : InstrItinClass;
def II_MOV_d1_eRo_eUnpackSign : InstrItinClass;
def II_MOV_d1_eRo_eSRSSign : InstrItinClass;
def II_MOV_d1_eRo_mCoreID : InstrItinClass;
def II_MOV_d1_eRo_eUPSSign : InstrItinClass;
def II_MOV_d1_eRo_eS : InstrItinClass;
def II_MOV_d1_eRo_mLCm : InstrItinClass;
def II_MOV_d1_eRo_ePackSign : InstrItinClass;
def II_MOV_d1_eRo_eDN : InstrItinClass;
def II_MOV_d1_eRo_mSPm : InstrItinClass;
def II_MOV_d1_eRo_eM : InstrItinClass;
def II_MOV_d1_eRo_eRo : InstrItinClass;

// instruction: MOV_d2
def II_MOV_d2 : InstrItinClass;
def II_MOV_d2_eRe_mLEm : InstrItinClass;
def II_MOV_d2_eRo_mLEm : InstrItinClass;
def II_MOV_d2_eRe_eDJ : InstrItinClass;
def II_MOV_d2_eRe_mLRm : InstrItinClass;
def II_MOV_d2_eRe_eDC : InstrItinClass;
def II_MOV_d2_eRe_eRe : InstrItinClass;
def II_MOV_d2_eRe_mLSm : InstrItinClass;
def II_MOV_d2_eRe_eVaddSign : InstrItinClass;
def II_MOV_d2_eRe_eP : InstrItinClass;
def II_MOV_d2_eRe_eUnpackSign : InstrItinClass;
def II_MOV_d2_eRe_eSRSSign : InstrItinClass;
def II_MOV_d2_eRe_mCoreID : InstrItinClass;
def II_MOV_d2_eRe_eUPSSign : InstrItinClass;
def II_MOV_d2_eRe_eS : InstrItinClass;
def II_MOV_d2_eRe_mLCm : InstrItinClass;
def II_MOV_d2_eRe_ePackSign : InstrItinClass;
def II_MOV_d2_eRe_eDN : InstrItinClass;
def II_MOV_d2_eRe_mSPm : InstrItinClass;
def II_MOV_d2_eRe_eM : InstrItinClass;
def II_MOV_d2_eRe_eRo : InstrItinClass;
def II_MOV_d2_eRo_eDJ : InstrItinClass;
def II_MOV_d2_eRo_mLRm : InstrItinClass;
def II_MOV_d2_eRo_eDC : InstrItinClass;
def II_MOV_d2_eRo_eRe : InstrItinClass;
def II_MOV_d2_eRo_mLSm : InstrItinClass;
def II_MOV_d2_eRo_eVaddSign : InstrItinClass;
def II_MOV_d2_eRo_eP : InstrItinClass;
def II_MOV_d2_eRo_eUnpackSign : InstrItinClass;
def II_MOV_d2_eRo_eSRSSign : InstrItinClass;
def II_MOV_d2_eRo_mCoreID : InstrItinClass;
def II_MOV_d2_eRo_eUPSSign : InstrItinClass;
def II_MOV_d2_eRo_eS : InstrItinClass;
def II_MOV_d2_eRo_mLCm : InstrItinClass;
def II_MOV_d2_eRo_ePackSign : InstrItinClass;
def II_MOV_d2_eRo_eDN : InstrItinClass;
def II_MOV_d2_eRo_mSPm : InstrItinClass;
def II_MOV_d2_eRo_eM : InstrItinClass;
def II_MOV_d2_eRo_eRo : InstrItinClass;

// instruction: MOV_d3
def II_MOV_d3 : InstrItinClass;
def II_MOV_d3_eRe_mLEm : InstrItinClass;
def II_MOV_d3_eRo_mLEm : InstrItinClass;
def II_MOV_d3_eRe_eDJ : InstrItinClass;
def II_MOV_d3_eRe_mLRm : InstrItinClass;
def II_MOV_d3_eRe_eDC : InstrItinClass;
def II_MOV_d3_eRe_eRe : InstrItinClass;
def II_MOV_d3_eRe_mLSm : InstrItinClass;
def II_MOV_d3_eRe_eVaddSign : InstrItinClass;
def II_MOV_d3_eRe_eP : InstrItinClass;
def II_MOV_d3_eRe_eUnpackSign : InstrItinClass;
def II_MOV_d3_eRe_eSRSSign : InstrItinClass;
def II_MOV_d3_eRe_mCoreID : InstrItinClass;
def II_MOV_d3_eRe_eUPSSign : InstrItinClass;
def II_MOV_d3_eRe_eS : InstrItinClass;
def II_MOV_d3_eRe_mLCm : InstrItinClass;
def II_MOV_d3_eRe_ePackSign : InstrItinClass;
def II_MOV_d3_eRe_eDN : InstrItinClass;
def II_MOV_d3_eRe_mSPm : InstrItinClass;
def II_MOV_d3_eRe_eM : InstrItinClass;
def II_MOV_d3_eRe_eRo : InstrItinClass;
def II_MOV_d3_eRo_eDJ : InstrItinClass;
def II_MOV_d3_eRo_mLRm : InstrItinClass;
def II_MOV_d3_eRo_eDC : InstrItinClass;
def II_MOV_d3_eRo_eRe : InstrItinClass;
def II_MOV_d3_eRo_mLSm : InstrItinClass;
def II_MOV_d3_eRo_eVaddSign : InstrItinClass;
def II_MOV_d3_eRo_eP : InstrItinClass;
def II_MOV_d3_eRo_eUnpackSign : InstrItinClass;
def II_MOV_d3_eRo_eSRSSign : InstrItinClass;
def II_MOV_d3_eRo_mCoreID : InstrItinClass;
def II_MOV_d3_eRo_eUPSSign : InstrItinClass;
def II_MOV_d3_eRo_eS : InstrItinClass;
def II_MOV_d3_eRo_mLCm : InstrItinClass;
def II_MOV_d3_eRo_ePackSign : InstrItinClass;
def II_MOV_d3_eRo_eDN : InstrItinClass;
def II_MOV_d3_eRo_mSPm : InstrItinClass;
def II_MOV_d3_eRo_eM : InstrItinClass;
def II_MOV_d3_eRo_eRo : InstrItinClass;

// instruction: MOV_d4
def II_MOV_d4 : InstrItinClass;
def II_MOV_d4_eRe_mLEm : InstrItinClass;
def II_MOV_d4_eRo_mLEm : InstrItinClass;
def II_MOV_d4_eRe_eDJ : InstrItinClass;
def II_MOV_d4_eRe_mLRm : InstrItinClass;
def II_MOV_d4_eRe_eDC : InstrItinClass;
def II_MOV_d4_eRe_eRe : InstrItinClass;
def II_MOV_d4_eRe_mLSm : InstrItinClass;
def II_MOV_d4_eRe_eVaddSign : InstrItinClass;
def II_MOV_d4_eRe_eP : InstrItinClass;
def II_MOV_d4_eRe_eUnpackSign : InstrItinClass;
def II_MOV_d4_eRe_eSRSSign : InstrItinClass;
def II_MOV_d4_eRe_mCoreID : InstrItinClass;
def II_MOV_d4_eRe_eUPSSign : InstrItinClass;
def II_MOV_d4_eRe_eS : InstrItinClass;
def II_MOV_d4_eRe_mLCm : InstrItinClass;
def II_MOV_d4_eRe_ePackSign : InstrItinClass;
def II_MOV_d4_eRe_eDN : InstrItinClass;
def II_MOV_d4_eRe_mSPm : InstrItinClass;
def II_MOV_d4_eRe_eM : InstrItinClass;
def II_MOV_d4_eRe_eRo : InstrItinClass;
def II_MOV_d4_eRo_eDJ : InstrItinClass;
def II_MOV_d4_eRo_mLRm : InstrItinClass;
def II_MOV_d4_eRo_eDC : InstrItinClass;
def II_MOV_d4_eRo_eRe : InstrItinClass;
def II_MOV_d4_eRo_mLSm : InstrItinClass;
def II_MOV_d4_eRo_eVaddSign : InstrItinClass;
def II_MOV_d4_eRo_eP : InstrItinClass;
def II_MOV_d4_eRo_eUnpackSign : InstrItinClass;
def II_MOV_d4_eRo_eSRSSign : InstrItinClass;
def II_MOV_d4_eRo_mCoreID : InstrItinClass;
def II_MOV_d4_eRo_eUPSSign : InstrItinClass;
def II_MOV_d4_eRo_eS : InstrItinClass;
def II_MOV_d4_eRo_mLCm : InstrItinClass;
def II_MOV_d4_eRo_ePackSign : InstrItinClass;
def II_MOV_d4_eRo_eDN : InstrItinClass;
def II_MOV_d4_eRo_mSPm : InstrItinClass;
def II_MOV_d4_eRo_eM : InstrItinClass;
def II_MOV_d4_eRo_eRo : InstrItinClass;

// instruction: MOV_d5
def II_MOV_d5 : InstrItinClass;
def II_MOV_d5_eRe_mLEm : InstrItinClass;
def II_MOV_d5_eRo_mLEm : InstrItinClass;
def II_MOV_d5_eRe_eDJ : InstrItinClass;
def II_MOV_d5_eRe_mLRm : InstrItinClass;
def II_MOV_d5_eRe_eDC : InstrItinClass;
def II_MOV_d5_eRe_eRe : InstrItinClass;
def II_MOV_d5_eRe_mLSm : InstrItinClass;
def II_MOV_d5_eRe_eVaddSign : InstrItinClass;
def II_MOV_d5_eRe_eP : InstrItinClass;
def II_MOV_d5_eRe_eUnpackSign : InstrItinClass;
def II_MOV_d5_eRe_eSRSSign : InstrItinClass;
def II_MOV_d5_eRe_mCoreID : InstrItinClass;
def II_MOV_d5_eRe_eUPSSign : InstrItinClass;
def II_MOV_d5_eRe_eS : InstrItinClass;
def II_MOV_d5_eRe_mLCm : InstrItinClass;
def II_MOV_d5_eRe_ePackSign : InstrItinClass;
def II_MOV_d5_eRe_eDN : InstrItinClass;
def II_MOV_d5_eRe_mSPm : InstrItinClass;
def II_MOV_d5_eRe_eM : InstrItinClass;
def II_MOV_d5_eRe_eRo : InstrItinClass;
def II_MOV_d5_eRo_eDJ : InstrItinClass;
def II_MOV_d5_eRo_mLRm : InstrItinClass;
def II_MOV_d5_eRo_eDC : InstrItinClass;
def II_MOV_d5_eRo_eRe : InstrItinClass;
def II_MOV_d5_eRo_mLSm : InstrItinClass;
def II_MOV_d5_eRo_eVaddSign : InstrItinClass;
def II_MOV_d5_eRo_eP : InstrItinClass;
def II_MOV_d5_eRo_eUnpackSign : InstrItinClass;
def II_MOV_d5_eRo_eSRSSign : InstrItinClass;
def II_MOV_d5_eRo_mCoreID : InstrItinClass;
def II_MOV_d5_eRo_eUPSSign : InstrItinClass;
def II_MOV_d5_eRo_eS : InstrItinClass;
def II_MOV_d5_eRo_mLCm : InstrItinClass;
def II_MOV_d5_eRo_ePackSign : InstrItinClass;
def II_MOV_d5_eRo_eDN : InstrItinClass;
def II_MOV_d5_eRo_mSPm : InstrItinClass;
def II_MOV_d5_eRo_eM : InstrItinClass;
def II_MOV_d5_eRo_eRo : InstrItinClass;

// instruction: MOV_d6
def II_MOV_d6 : InstrItinClass;
def II_MOV_d6_eRe_mLEm : InstrItinClass;
def II_MOV_d6_eRo_mLEm : InstrItinClass;
def II_MOV_d6_eRe_eDJ : InstrItinClass;
def II_MOV_d6_eRe_mLRm : InstrItinClass;
def II_MOV_d6_eRe_eDC : InstrItinClass;
def II_MOV_d6_eRe_eRe : InstrItinClass;
def II_MOV_d6_eRe_mLSm : InstrItinClass;
def II_MOV_d6_eRe_eVaddSign : InstrItinClass;
def II_MOV_d6_eRe_eP : InstrItinClass;
def II_MOV_d6_eRe_eUnpackSign : InstrItinClass;
def II_MOV_d6_eRe_eSRSSign : InstrItinClass;
def II_MOV_d6_eRe_mCoreID : InstrItinClass;
def II_MOV_d6_eRe_eUPSSign : InstrItinClass;
def II_MOV_d6_eRe_eS : InstrItinClass;
def II_MOV_d6_eRe_mLCm : InstrItinClass;
def II_MOV_d6_eRe_ePackSign : InstrItinClass;
def II_MOV_d6_eRe_eDN : InstrItinClass;
def II_MOV_d6_eRe_mSPm : InstrItinClass;
def II_MOV_d6_eRe_eM : InstrItinClass;
def II_MOV_d6_eRe_eRo : InstrItinClass;
def II_MOV_d6_eRo_eDJ : InstrItinClass;
def II_MOV_d6_eRo_mLRm : InstrItinClass;
def II_MOV_d6_eRo_eDC : InstrItinClass;
def II_MOV_d6_eRo_eRe : InstrItinClass;
def II_MOV_d6_eRo_mLSm : InstrItinClass;
def II_MOV_d6_eRo_eVaddSign : InstrItinClass;
def II_MOV_d6_eRo_eP : InstrItinClass;
def II_MOV_d6_eRo_eUnpackSign : InstrItinClass;
def II_MOV_d6_eRo_eSRSSign : InstrItinClass;
def II_MOV_d6_eRo_mCoreID : InstrItinClass;
def II_MOV_d6_eRo_eUPSSign : InstrItinClass;
def II_MOV_d6_eRo_eS : InstrItinClass;
def II_MOV_d6_eRo_mLCm : InstrItinClass;
def II_MOV_d6_eRo_ePackSign : InstrItinClass;
def II_MOV_d6_eRo_eDN : InstrItinClass;
def II_MOV_d6_eRo_mSPm : InstrItinClass;
def II_MOV_d6_eRo_eM : InstrItinClass;
def II_MOV_d6_eRo_eRo : InstrItinClass;

def II_MOV_nb_lda : InstrItinClass;
// instruction: MOV_nb_st_mMStream_tlast_imm
def II_MOV_nb_st_mMStream_tlast_imm : InstrItinClass;
def II_MOV_nb_st_mMStream_tlast_imm_eDJ : InstrItinClass;
def II_MOV_nb_st_mMStream_tlast_imm_eDC : InstrItinClass;
def II_MOV_nb_st_mMStream_tlast_imm_eRe : InstrItinClass;
def II_MOV_nb_st_mMStream_tlast_imm_eDN : InstrItinClass;
def II_MOV_nb_st_mMStream_tlast_imm_eP : InstrItinClass;
def II_MOV_nb_st_mMStream_tlast_imm_eM : InstrItinClass;
def II_MOV_nb_st_mMStream_tlast_imm_eRo : InstrItinClass;

// instruction: MOV_nb_st_mMStream_tlast_reg
def II_MOV_nb_st_mMStream_tlast_reg : InstrItinClass;
def II_MOV_nb_st_mMStream_tlast_reg_eDJ : InstrItinClass;
def II_MOV_nb_st_mMStream_tlast_reg_eDC : InstrItinClass;
def II_MOV_nb_st_mMStream_tlast_reg_eRe : InstrItinClass;
def II_MOV_nb_st_mMStream_tlast_reg_eDN : InstrItinClass;
def II_MOV_nb_st_mMStream_tlast_reg_eP : InstrItinClass;
def II_MOV_nb_st_mMStream_tlast_reg_eM : InstrItinClass;
def II_MOV_nb_st_mMStream_tlast_reg_eRo : InstrItinClass;

// instruction: MOV_nb_tlast
def II_MOV_nb_tlast : InstrItinClass;
def II_MOV_nb_tlast_eDJ : InstrItinClass;
def II_MOV_nb_tlast_eDC : InstrItinClass;
def II_MOV_nb_tlast_eRe : InstrItinClass;
def II_MOV_nb_tlast_eDN : InstrItinClass;
def II_MOV_nb_tlast_eP : InstrItinClass;
def II_MOV_nb_tlast_eM : InstrItinClass;
def II_MOV_nb_tlast_eRo : InstrItinClass;

// instruction: MOV_tlast
def II_MOV_tlast : InstrItinClass;
def II_MOV_tlast_eDJ : InstrItinClass;
def II_MOV_tlast_eDC : InstrItinClass;
def II_MOV_tlast_eRe : InstrItinClass;
def II_MOV_tlast_eDN : InstrItinClass;
def II_MOV_tlast_eP : InstrItinClass;
def II_MOV_tlast_eM : InstrItinClass;
def II_MOV_tlast_eRo : InstrItinClass;

def II_MSC : InstrItinClass;
def II_MUL : InstrItinClass;
def II_NEZ : InstrItinClass;
def II_NE : InstrItinClass;
def II_NOPA : InstrItinClass;
def II_NOPB : InstrItinClass;
def II_NOPM : InstrItinClass;
def II_NOPS : InstrItinClass;
def II_NOPV : InstrItinClass;
def II_NOPXM : InstrItinClass;
def II_NOPX : InstrItinClass;
def II_OR : InstrItinClass;
def II_PADDA_2D : InstrItinClass;
def II_PADDA_3D : InstrItinClass;
def II_PADDA_pstm_nrm : InstrItinClass;
def II_PADDA_pstm_nrm_imm : InstrItinClass;
def II_PADDB_2D : InstrItinClass;
def II_PADDB_3D : InstrItinClass;
def II_PADDB_pstm_nrm : InstrItinClass;
def II_PADDB_pstm_nrm_imm : InstrItinClass;
def II_PADDS_2D : InstrItinClass;
def II_PADDS_3D : InstrItinClass;
def II_PADDS_pstm_nrm : InstrItinClass;
def II_PADDS_pstm_nrm_imm : InstrItinClass;
def II_PADDXM_pstm_sp : InstrItinClass;
def II_PADDXM_pstm_sp_imm : InstrItinClass;
def II_POPCOUNT : InstrItinClass;
def II_REL_COND_mLockId_imm : InstrItinClass;
def II_REL_COND_mLockId_reg : InstrItinClass;
def II_REL_mLockId_imm : InstrItinClass;
def II_REL_mLockId_reg : InstrItinClass;
def II_RET : InstrItinClass;
def II_SBC : InstrItinClass;
def II_SEL_EQZ : InstrItinClass;
def II_SEL_NEZ : InstrItinClass;
def II_SQRT_nlf_sqrt : InstrItinClass;
def II_SQRT_nlf_sqrt_mOptConvDel_mS3 : InstrItinClass;
def II_SQRT_nlf_sqrt_mOptConvDel_mS3_mOptConv : InstrItinClass;
def II_SQRT_nlf_sqrt_mOptConv : InstrItinClass;
// instruction: ST_2D_dml_sts_scalar
def II_ST_2D_dml_sts_scalar : InstrItinClass;
def II_ST_2D_dml_sts_scalar_eFlo : InstrItinClass;
def II_ST_2D_dml_sts_scalar_eEGhe_s : InstrItinClass;
def II_ST_2D_dml_sts_scalar_eL : InstrItinClass;
def II_ST_2D_dml_sts_scalar_eEEo : InstrItinClass;
def II_ST_2D_dml_sts_scalar_eFle : InstrItinClass;
def II_ST_2D_dml_sts_scalar_eGGo : InstrItinClass;
def II_ST_2D_dml_sts_scalar_eFhe : InstrItinClass;
def II_ST_2D_dml_sts_scalar_eEGho_s : InstrItinClass;
def II_ST_2D_dml_sts_scalar_eFho : InstrItinClass;
def II_ST_2D_dml_sts_scalar_eEGlo_s : InstrItinClass;
def II_ST_2D_dml_sts_scalar_eEGle_s : InstrItinClass;
def II_ST_2D_dml_sts_scalar_eGGe : InstrItinClass;
def II_ST_2D_dml_sts_scalar_eEEe : InstrItinClass;

def II_ST_2D_dms_sts_g : InstrItinClass;
// instruction: ST_2D_dms_sts_scalar
def II_ST_2D_dms_sts_scalar : InstrItinClass;
def II_ST_2D_dms_sts_scalar_eEle : InstrItinClass;
def II_ST_2D_dms_sts_scalar_eElo : InstrItinClass;
def II_ST_2D_dms_sts_scalar_eDJ : InstrItinClass;
def II_ST_2D_dms_sts_scalar_mLRs : InstrItinClass;
def II_ST_2D_dms_sts_scalar_eDC : InstrItinClass;
def II_ST_2D_dms_sts_scalar_eRe : InstrItinClass;
def II_ST_2D_dms_sts_scalar_eDN : InstrItinClass;
def II_ST_2D_dms_sts_scalar_eEhe : InstrItinClass;
def II_ST_2D_dms_sts_scalar_eP : InstrItinClass;
def II_ST_2D_dms_sts_scalar_eM : InstrItinClass;
def II_ST_2D_dms_sts_scalar_eRo : InstrItinClass;
def II_ST_2D_dms_sts_scalar_eEho : InstrItinClass;

def II_ST_2D_dmv_sts_f : InstrItinClass;
def II_ST_2D_s16 : InstrItinClass;
def II_ST_2D_s8 : InstrItinClass;
// instruction: ST_3D_dml_sts_scalar
def II_ST_3D_dml_sts_scalar : InstrItinClass;
def II_ST_3D_dml_sts_scalar_eFlo : InstrItinClass;
def II_ST_3D_dml_sts_scalar_eEGhe_s : InstrItinClass;
def II_ST_3D_dml_sts_scalar_eL : InstrItinClass;
def II_ST_3D_dml_sts_scalar_eEEo : InstrItinClass;
def II_ST_3D_dml_sts_scalar_eFle : InstrItinClass;
def II_ST_3D_dml_sts_scalar_eGGo : InstrItinClass;
def II_ST_3D_dml_sts_scalar_eFhe : InstrItinClass;
def II_ST_3D_dml_sts_scalar_eEGho_s : InstrItinClass;
def II_ST_3D_dml_sts_scalar_eFho : InstrItinClass;
def II_ST_3D_dml_sts_scalar_eEGlo_s : InstrItinClass;
def II_ST_3D_dml_sts_scalar_eEGle_s : InstrItinClass;
def II_ST_3D_dml_sts_scalar_eGGe : InstrItinClass;
def II_ST_3D_dml_sts_scalar_eEEe : InstrItinClass;

def II_ST_3D_dms_sts_g : InstrItinClass;
// instruction: ST_3D_dms_sts_scalar
def II_ST_3D_dms_sts_scalar : InstrItinClass;
def II_ST_3D_dms_sts_scalar_eEle : InstrItinClass;
def II_ST_3D_dms_sts_scalar_eElo : InstrItinClass;
def II_ST_3D_dms_sts_scalar_eDJ : InstrItinClass;
def II_ST_3D_dms_sts_scalar_mLRs : InstrItinClass;
def II_ST_3D_dms_sts_scalar_eDC : InstrItinClass;
def II_ST_3D_dms_sts_scalar_eRe : InstrItinClass;
def II_ST_3D_dms_sts_scalar_eDN : InstrItinClass;
def II_ST_3D_dms_sts_scalar_eEhe : InstrItinClass;
def II_ST_3D_dms_sts_scalar_eP : InstrItinClass;
def II_ST_3D_dms_sts_scalar_eM : InstrItinClass;
def II_ST_3D_dms_sts_scalar_eRo : InstrItinClass;
def II_ST_3D_dms_sts_scalar_eEho : InstrItinClass;

def II_ST_3D_dmv_sts_f : InstrItinClass;
def II_ST_3D_s16 : InstrItinClass;
def II_ST_3D_s8 : InstrItinClass;
def II_ST_TM_2D : InstrItinClass;
def II_ST_TM_3D : InstrItinClass;
def II_ST_TM_idx : InstrItinClass;
def II_ST_TM_idx_imm : InstrItinClass;
def II_ST_TM_pstm_nrm : InstrItinClass;
def II_ST_TM_pstm_nrm_imm : InstrItinClass;
// instruction: ST_dml_sts_scalar_spill
def II_ST_dml_sts_scalar_spill : InstrItinClass;
def II_ST_dml_sts_scalar_spill_eFlo : InstrItinClass;
def II_ST_dml_sts_scalar_spill_eEGhe_s : InstrItinClass;
def II_ST_dml_sts_scalar_spill_eL : InstrItinClass;
def II_ST_dml_sts_scalar_spill_eEEo : InstrItinClass;
def II_ST_dml_sts_scalar_spill_eFle : InstrItinClass;
def II_ST_dml_sts_scalar_spill_eGGo : InstrItinClass;
def II_ST_dml_sts_scalar_spill_eFhe : InstrItinClass;
def II_ST_dml_sts_scalar_spill_eEGho_s : InstrItinClass;
def II_ST_dml_sts_scalar_spill_eFho : InstrItinClass;
def II_ST_dml_sts_scalar_spill_eEGlo_s : InstrItinClass;
def II_ST_dml_sts_scalar_spill_eEGle_s : InstrItinClass;
def II_ST_dml_sts_scalar_spill_eGGe : InstrItinClass;
def II_ST_dml_sts_scalar_spill_eEEe : InstrItinClass;

// instruction: ST_dml_sts_scalar_st_idx
def II_ST_dml_sts_scalar_st_idx : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_eFlo : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_eEGhe_s : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_eL : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_eEEo : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_eFle : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_eGGo : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_eFhe : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_eEGho_s : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_eFho : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_eEGlo_s : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_eEGle_s : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_eGGe : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_eEEe : InstrItinClass;

// instruction: ST_dml_sts_scalar_st_idx_imm
def II_ST_dml_sts_scalar_st_idx_imm : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_imm_eFlo : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_imm_eEGhe_s : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_imm_eL : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_imm_eEEo : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_imm_eFle : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_imm_eGGo : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_imm_eFhe : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_imm_eEGho_s : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_imm_eFho : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_imm_eEGlo_s : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_imm_eEGle_s : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_imm_eGGe : InstrItinClass;
def II_ST_dml_sts_scalar_st_idx_imm_eEEe : InstrItinClass;

// instruction: ST_dml_sts_scalar_st_pstm_nrm
def II_ST_dml_sts_scalar_st_pstm_nrm : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_eFlo : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_eEGhe_s : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_eL : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_eEEo : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_eFle : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_eGGo : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_eFhe : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_eEGho_s : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_eFho : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_eEGlo_s : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_eEGle_s : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_eGGe : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_eEEe : InstrItinClass;

// instruction: ST_dml_sts_scalar_st_pstm_nrm_imm
def II_ST_dml_sts_scalar_st_pstm_nrm_imm : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_imm_eFlo : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_imm_eEGhe_s : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_imm_eL : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_imm_eEEo : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_imm_eFle : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_imm_eGGo : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_imm_eFhe : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_imm_eEGho_s : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_imm_eFho : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_imm_eEGlo_s : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_imm_eEGle_s : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_imm_eGGe : InstrItinClass;
def II_ST_dml_sts_scalar_st_pstm_nrm_imm_eEEe : InstrItinClass;

def II_ST_dms_sts_g_spill : InstrItinClass;
def II_ST_dms_sts_g_st_idx : InstrItinClass;
def II_ST_dms_sts_g_st_idx_imm : InstrItinClass;
def II_ST_dms_sts_g_st_pstm_nrm : InstrItinClass;
def II_ST_dms_sts_g_st_pstm_nrm_imm : InstrItinClass;
// instruction: ST_dms_sts_scalar_spill
def II_ST_dms_sts_scalar_spill : InstrItinClass;
def II_ST_dms_sts_scalar_spill_eEle : InstrItinClass;
def II_ST_dms_sts_scalar_spill_eElo : InstrItinClass;
def II_ST_dms_sts_scalar_spill_eDJ : InstrItinClass;
def II_ST_dms_sts_scalar_spill_mLRs : InstrItinClass;
def II_ST_dms_sts_scalar_spill_eDC : InstrItinClass;
def II_ST_dms_sts_scalar_spill_eRe : InstrItinClass;
def II_ST_dms_sts_scalar_spill_eDN : InstrItinClass;
def II_ST_dms_sts_scalar_spill_eEhe : InstrItinClass;
def II_ST_dms_sts_scalar_spill_eP : InstrItinClass;
def II_ST_dms_sts_scalar_spill_eM : InstrItinClass;
def II_ST_dms_sts_scalar_spill_eRo : InstrItinClass;
def II_ST_dms_sts_scalar_spill_eEho : InstrItinClass;

// instruction: ST_dms_sts_scalar_st_idx
def II_ST_dms_sts_scalar_st_idx : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_eEle : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_eElo : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_eDJ : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_mLRs : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_eDC : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_eRe : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_eDN : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_eEhe : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_eP : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_eM : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_eRo : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_eEho : InstrItinClass;

// instruction: ST_dms_sts_scalar_st_idx_imm
def II_ST_dms_sts_scalar_st_idx_imm : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_imm_eEle : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_imm_eElo : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_imm_eDJ : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_imm_mLRs : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_imm_eDC : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_imm_eRe : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_imm_eDN : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_imm_eEhe : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_imm_eP : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_imm_eM : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_imm_eRo : InstrItinClass;
def II_ST_dms_sts_scalar_st_idx_imm_eEho : InstrItinClass;

// instruction: ST_dms_sts_scalar_st_pstm_nrm
def II_ST_dms_sts_scalar_st_pstm_nrm : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_eEle : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_eElo : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_eDJ : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_mLRs : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_eDC : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_eRe : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_eDN : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_eEhe : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_eP : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_eM : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_eRo : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_eEho : InstrItinClass;

// instruction: ST_dms_sts_scalar_st_pstm_nrm_imm
def II_ST_dms_sts_scalar_st_pstm_nrm_imm : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_imm_eEle : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_imm_eElo : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_imm_eDJ : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_imm_mLRs : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_imm_eDC : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_imm_eRe : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_imm_eDN : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_imm_eEhe : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_imm_eP : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_imm_eM : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_imm_eRo : InstrItinClass;
def II_ST_dms_sts_scalar_st_pstm_nrm_imm_eEho : InstrItinClass;

def II_ST_dmv_sts_f_spill : InstrItinClass;
def II_ST_dmv_sts_f_st_idx : InstrItinClass;
def II_ST_dmv_sts_f_st_idx_imm : InstrItinClass;
def II_ST_dmv_sts_f_st_pstm_nrm : InstrItinClass;
def II_ST_dmv_sts_f_st_pstm_nrm_imm : InstrItinClass;
def II_ST_s16_idx : InstrItinClass;
def II_ST_s16_idx_imm : InstrItinClass;
def II_ST_s16_pstm_nrm : InstrItinClass;
def II_ST_s16_pstm_nrm_imm : InstrItinClass;
def II_ST_s8_idx : InstrItinClass;
def II_ST_s8_idx_imm : InstrItinClass;
def II_ST_s8_pstm_nrm : InstrItinClass;
def II_ST_s8_pstm_nrm_imm : InstrItinClass;
def II_SUB : InstrItinClass;
def II_VABS_GTZ16_vaddSign0 : InstrItinClass;
def II_VABS_GTZ16_vaddSign1 : InstrItinClass;
def II_VABS_GTZ32_vaddSign0 : InstrItinClass;
def II_VABS_GTZ32_vaddSign1 : InstrItinClass;
def II_VABS_GTZ8_vaddSign0 : InstrItinClass;
def II_VABS_GTZ8_vaddSign1 : InstrItinClass;
def II_VADDMAC_vaddmac_vmul_cm_core_X_QX : InstrItinClass;
def II_VADDMAC_vaddmac_vmul_cm_core_X_QY : InstrItinClass;
def II_VADDMAC_vaddmac_vmul_cm_core_X_R : InstrItinClass;
def II_VADDMAC_vaddmac_vmul_cm_core_X_X : InstrItinClass;
def II_VADDMAC_vaddmac_vmul_cm_core_X_Y : InstrItinClass;
def II_VADDMAC_vaddmac_vmul_cm_core_Y_QX : InstrItinClass;
def II_VADDMAC_vaddmac_vmul_cm_core_Y_R : InstrItinClass;
def II_VADDMAC_vaddmac_vmul_cm_core_Y_X : InstrItinClass;
def II_VADDMAC_vaddmac_vmul_cm_core_Y_Y : InstrItinClass;
def II_VADDMAC_vaddmac_half_vmul_cm_half_core_X_QX : InstrItinClass;
def II_VADDMAC_vaddmac_half_vmul_cm_half_core_X_R : InstrItinClass;
def II_VADDMAC_vaddmac_half_vmul_cm_half_core_X_X : InstrItinClass;
def II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_QX : InstrItinClass;
def II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_QY : InstrItinClass;
def II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_R : InstrItinClass;
def II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_X : InstrItinClass;
def II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_Y : InstrItinClass;
def II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_QX : InstrItinClass;
def II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_R : InstrItinClass;
def II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_X : InstrItinClass;
def II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_Y : InstrItinClass;
def II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_QX : InstrItinClass;
def II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_QY : InstrItinClass;
def II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_R : InstrItinClass;
def II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_X : InstrItinClass;
def II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_Y : InstrItinClass;
def II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_QX : InstrItinClass;
def II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_R : InstrItinClass;
def II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_X : InstrItinClass;
def II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_Y : InstrItinClass;
def II_VADDMAC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_QX : InstrItinClass;
def II_VADDMAC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_R : InstrItinClass;
def II_VADDMAC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_X : InstrItinClass;
def II_VADDMAC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_QX : InstrItinClass;
def II_VADDMAC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_R : InstrItinClass;
def II_VADDMAC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_X : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_vmul_bf_core_X_QY : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_vmul_bf_core_X_X : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_vmul_bf_core_Y_R : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_vmul_bf_core_Y_Y : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_half_vmul_bf_core_X_R : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_half_vmul_bf_core_X_Y : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_half_vmul_bf_core_Y_QY : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_half_vmul_bf_half_core_X_X : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_QY : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_X : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_R : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_Y : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_QY : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_X : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_R : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_Y : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_R : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_Y : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_Y_QY : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_half_core_X_X : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_R : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_Y : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_Y_QY : InstrItinClass;
def II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_half_core_X_X : InstrItinClass;
def II_VADDMAC_f_vaddmac_bfp13 : InstrItinClass;
def II_VADDMAC_f_vaddmac_bfp13_scd_add_scd_incr : InstrItinClass;
def II_VADDMAC_f_vaddmac_bfp13_scd_add_scd_noincr : InstrItinClass;
def II_VADDMAC_f_vaddmac_bfp16 : InstrItinClass;
def II_VADDMAC_f_vaddmac_bfp16_scd_add_scd_incr : InstrItinClass;
def II_VADDMAC_f_vaddmac_bfp16_scd_add_scd_noincr : InstrItinClass;
def II_VADDMSC_vaddmac_vmul_cm_core_X_QX : InstrItinClass;
def II_VADDMSC_vaddmac_vmul_cm_core_X_QY : InstrItinClass;
def II_VADDMSC_vaddmac_vmul_cm_core_X_R : InstrItinClass;
def II_VADDMSC_vaddmac_vmul_cm_core_X_X : InstrItinClass;
def II_VADDMSC_vaddmac_vmul_cm_core_X_Y : InstrItinClass;
def II_VADDMSC_vaddmac_vmul_cm_core_Y_QX : InstrItinClass;
def II_VADDMSC_vaddmac_vmul_cm_core_Y_R : InstrItinClass;
def II_VADDMSC_vaddmac_vmul_cm_core_Y_X : InstrItinClass;
def II_VADDMSC_vaddmac_vmul_cm_core_Y_Y : InstrItinClass;
def II_VADDMSC_vaddmac_half_vmul_cm_half_core_X_QX : InstrItinClass;
def II_VADDMSC_vaddmac_half_vmul_cm_half_core_X_R : InstrItinClass;
def II_VADDMSC_vaddmac_half_vmul_cm_half_core_X_X : InstrItinClass;
def II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_QX : InstrItinClass;
def II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_QY : InstrItinClass;
def II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_R : InstrItinClass;
def II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_X : InstrItinClass;
def II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_Y : InstrItinClass;
def II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_QX : InstrItinClass;
def II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_R : InstrItinClass;
def II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_X : InstrItinClass;
def II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_Y : InstrItinClass;
def II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_QX : InstrItinClass;
def II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_QY : InstrItinClass;
def II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_R : InstrItinClass;
def II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_X : InstrItinClass;
def II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_Y : InstrItinClass;
def II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_QX : InstrItinClass;
def II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_R : InstrItinClass;
def II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_X : InstrItinClass;
def II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_Y : InstrItinClass;
def II_VADDMSC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_QX : InstrItinClass;
def II_VADDMSC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_R : InstrItinClass;
def II_VADDMSC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_X : InstrItinClass;
def II_VADDMSC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_QX : InstrItinClass;
def II_VADDMSC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_R : InstrItinClass;
def II_VADDMSC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_X : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_vmul_bf_core_X_QY : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_vmul_bf_core_X_X : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_vmul_bf_core_Y_R : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_vmul_bf_core_Y_Y : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_half_vmul_bf_core_X_R : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_half_vmul_bf_core_X_Y : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_half_vmul_bf_core_Y_QY : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_half_vmul_bf_half_core_X_X : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_QY : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_X : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_R : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_Y : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_QY : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_X : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_R : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_Y : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_R : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_Y : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_Y_QY : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_half_core_X_X : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_R : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_Y : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_Y_QY : InstrItinClass;
def II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_half_core_X_X : InstrItinClass;
def II_VADDMSC_f_vaddmac_bfp13 : InstrItinClass;
def II_VADDMSC_f_vaddmac_bfp13_scd_add_scd_incr : InstrItinClass;
def II_VADDMSC_f_vaddmac_bfp13_scd_add_scd_noincr : InstrItinClass;
def II_VADDMSC_f_vaddmac_bfp16 : InstrItinClass;
def II_VADDMSC_f_vaddmac_bfp16_scd_add_scd_incr : InstrItinClass;
def II_VADDMSC_f_vaddmac_bfp16_scd_add_scd_noincr : InstrItinClass;
def II_VADDSUB_16 : InstrItinClass;
def II_VADDSUB_32 : InstrItinClass;
def II_VADDSUB_8 : InstrItinClass;
def II_VADD_16 : InstrItinClass;
def II_VADD_32 : InstrItinClass;
def II_VADD_8 : InstrItinClass;
def II_VADD_vacc : InstrItinClass;
def II_VADD_vacc_half : InstrItinClass;
def II_VADD_vacc_scd_add_scd_incr : InstrItinClass;
def II_VADD_vacc_scd_add_scd_noincr : InstrItinClass;
def II_VADD_vacc_scd_half_add_scd_incr_half : InstrItinClass;
def II_VADD_vacc_scd_half_add_scd_noincr_half : InstrItinClass;
def II_VADD_f_vacc_fp : InstrItinClass;
def II_VADD_f_vacc_fp_scd_add_scd_incr : InstrItinClass;
def II_VADD_f_vacc_fp_scd_add_scd_noincr : InstrItinClass;
def II_VADD_f_vacc_half_fp : InstrItinClass;
def II_VADD_f_vacc_half_fp_scd_add_scd_incr_half : InstrItinClass;
def II_VADD_f_vacc_half_fp_scd_add_scd_noincr_half : InstrItinClass;
def II_VADD_mx6_vacc_fp_bfp13 : InstrItinClass;
def II_VADD_mx6_vacc_fp_bfp13_scd_add_scd_incr : InstrItinClass;
def II_VADD_mx6_vacc_fp_bfp13_scd_add_scd_noincr : InstrItinClass;
def II_VADD_mx6_vacc_half_fp_bfp13 : InstrItinClass;
def II_VADD_mx6_vacc_half_fp_bfp13_scd_add_scd_incr_half : InstrItinClass;
def II_VADD_mx6_vacc_half_fp_bfp13_scd_add_scd_noincr_half : InstrItinClass;
def II_VBAND : InstrItinClass;
def II_VBCSTSHFL_16_vec_broadcast_shuffle_bm : InstrItinClass;
def II_VBCSTSHFL_16_vec_broadcast_shuffle_x : InstrItinClass;
def II_VBCSTSHFL_32_vec_broadcast_shuffle_bm : InstrItinClass;
def II_VBCSTSHFL_32_vec_broadcast_shuffle_x : InstrItinClass;
def II_VBCSTSHFL_64_vec_broadcast_shuffle_bm : InstrItinClass;
def II_VBCSTSHFL_64_vec_broadcast_shuffle_x : InstrItinClass;
def II_VBCSTSHFL_8_vec_broadcast_shuffle_bm : InstrItinClass;
def II_VBCSTSHFL_8_vec_broadcast_shuffle_x : InstrItinClass;
def II_VBCST_16 : InstrItinClass;
def II_VBCST_32 : InstrItinClass;
def II_VBCST_64 : InstrItinClass;
def II_VBCST_8 : InstrItinClass;
def II_VBNEG_LTZ_s16 : InstrItinClass;
def II_VBNEG_LTZ_s32 : InstrItinClass;
def II_VBNEG_LTZ_s8 : InstrItinClass;
def II_VBOR : InstrItinClass;
def II_VCLR_vclr : InstrItinClass;
def II_VCLR_vclr_half : InstrItinClass;
def II_VCLR_f_vclr_fp : InstrItinClass;
def II_VCLR_f_vclr_half_fp : InstrItinClass;
def II_VCONV_bf16_fp32_mv_conv_mv_w_srs_bf16 : InstrItinClass;
def II_VCONV_bf16_fp32_mv_conv_mv_x_srs_bf16 : InstrItinClass;
def II_VCONV_bf16_fp32_mv_mcd_bf16_cm : InstrItinClass;
def II_VCONV_bf8_fp32 : InstrItinClass;
def II_VCONV_fp16_fp32_mv_mcd_fp16_cm : InstrItinClass;
def II_VCONV_fp16_fp32_mv_x_srs_fp16 : InstrItinClass;
def II_VCONV_fp32_bf16_alu : InstrItinClass;
def II_VCONV_fp32_bf16_mv_mv_w_ups_bf16 : InstrItinClass;
def II_VCONV_fp32_bf16_mv_mv_x_ups_f16 : InstrItinClass;
def II_VCONV_fp32_fp16_alu : InstrItinClass;
def II_VCONV_fp32_fp16_mv : InstrItinClass;
def II_VCONV_fp8_fp32 : InstrItinClass;
def II_VCONV_mx4_fp32 : InstrItinClass;
def II_VCONV_mx6_fp32 : InstrItinClass;
def II_VCONV_mx9_fp32 : InstrItinClass;
def II_VCONV_s32_bf16_fp32 : InstrItinClass;
def II_VEQZ_16 : InstrItinClass;
def II_VEQZ_32 : InstrItinClass;
def II_VEQZ_8 : InstrItinClass;
def II_VEXP2_bf16 : InstrItinClass;
def II_VEXP2_fp32 : InstrItinClass;
def II_VEXTBCSTSHFL_128 : InstrItinClass;
def II_VEXTBCSTSHFL_16 : InstrItinClass;
def II_VEXTBCSTSHFL_32 : InstrItinClass;
def II_VEXTBCSTSHFL_64 : InstrItinClass;
def II_VEXTBCST_128_vec_extract_broadcast_imm : InstrItinClass;
def II_VEXTBCST_128_vec_extract_broadcast_r : InstrItinClass;
def II_VEXTBCST_16_vec_extract_broadcast_imm : InstrItinClass;
def II_VEXTBCST_16_vec_extract_broadcast_r : InstrItinClass;
def II_VEXTBCST_32_vec_extract_broadcast_imm : InstrItinClass;
def II_VEXTBCST_32_vec_extract_broadcast_r : InstrItinClass;
def II_VEXTBCST_64_vec_extract_broadcast_imm : InstrItinClass;
def II_VEXTBCST_64_vec_extract_broadcast_r : InstrItinClass;
def II_VEXTBCST_8_vec_extract_broadcast_imm : InstrItinClass;
def II_VEXTBCST_8_vec_extract_broadcast_r : InstrItinClass;
def II_VEXTRACT_16_vec_extract_imm_vaddSign0 : InstrItinClass;
def II_VEXTRACT_16_vec_extract_imm_vaddSign1 : InstrItinClass;
def II_VEXTRACT_16_vec_extract_r_vaddSign0 : InstrItinClass;
def II_VEXTRACT_16_vec_extract_r_vaddSign1 : InstrItinClass;
def II_VEXTRACT_32_vec_extract_imm_vaddSign0 : InstrItinClass;
def II_VEXTRACT_32_vec_extract_imm_vaddSign1 : InstrItinClass;
def II_VEXTRACT_32_vec_extract_r_vaddSign0 : InstrItinClass;
def II_VEXTRACT_32_vec_extract_r_vaddSign1 : InstrItinClass;
def II_VEXTRACT_64_vec_extract_imm_vaddSign0 : InstrItinClass;
def II_VEXTRACT_64_vec_extract_imm_vaddSign1 : InstrItinClass;
def II_VEXTRACT_64_vec_extract_r_vaddSign0 : InstrItinClass;
def II_VEXTRACT_64_vec_extract_r_vaddSign1 : InstrItinClass;
def II_VEXTRACT_8_vec_extract_imm_vaddSign0 : InstrItinClass;
def II_VEXTRACT_8_vec_extract_imm_vaddSign1 : InstrItinClass;
def II_VEXTRACT_8_vec_extract_r_vaddSign0 : InstrItinClass;
def II_VEXTRACT_8_vec_extract_r_vaddSign1 : InstrItinClass;
def II_VFLOOR_s32_bf16 : InstrItinClass;
def II_VGE_16_vaddSign0 : InstrItinClass;
def II_VGE_16_vaddSign1 : InstrItinClass;
def II_VGE_32_vaddSign0 : InstrItinClass;
def II_VGE_32_vaddSign1 : InstrItinClass;
def II_VGE_8_vaddSign0 : InstrItinClass;
def II_VGE_8_vaddSign1 : InstrItinClass;
def II_VGE_bf16 : InstrItinClass;
def II_VGE_bf8 : InstrItinClass;
def II_VGE_fp16 : InstrItinClass;
def II_VGE_fp8 : InstrItinClass;
def II_VINSERT_16_mIdxImm0 : InstrItinClass;
def II_VINSERT_16_mR29_insert : InstrItinClass;
def II_VINSERT_32_mIdxImm0 : InstrItinClass;
def II_VINSERT_32_mR29_insert : InstrItinClass;
def II_VINSERT_64_mIdxImm0 : InstrItinClass;
def II_VINSERT_64_mR29_insert : InstrItinClass;
def II_VINSERT_8_mIdxImm0 : InstrItinClass;
def II_VINSERT_8_mR29_insert : InstrItinClass;
def II_VLDA_128_dmv_lda_w_idx : InstrItinClass;
def II_VLDA_128_dmv_lda_w_idx_imm : InstrItinClass;
def II_VLDA_128_dmv_lda_w_pstm_nrm : InstrItinClass;
def II_VLDA_128_dmv_lda_w_pstm_nrm_imm : InstrItinClass;
def II_VLDA_128_dmv_lda_w_spill : InstrItinClass;
def II_VLDA_2D_128 : InstrItinClass;
def II_VLDA_2D_CONV_fp32_bf16_dmw_lda_ups_bf16 : InstrItinClass;
def II_VLDA_2D_CONV_fp32_bf16_dmx_lda_ups_f16 : InstrItinClass;
def II_VLDA_2D_CONV_fp32_fp16 : InstrItinClass;
def II_VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign0 : InstrItinClass;
def II_VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign1 : InstrItinClass;
def II_VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign0 : InstrItinClass;
def II_VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign1 : InstrItinClass;
def II_VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign0 : InstrItinClass;
def II_VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign1 : InstrItinClass;
def II_VLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsSign0 : InstrItinClass;
def II_VLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsSign1 : InstrItinClass;
def II_VLDA_2D_dmw_lda_feg2 : InstrItinClass;
def II_VLDA_2D_dmw_lda_w : InstrItinClass;
def II_VLDA_2D_dmx_lda_bm : InstrItinClass;
def II_VLDA_2D_dmx_lda_x : InstrItinClass;
def II_VLDA_3D_128 : InstrItinClass;
def II_VLDA_3D_CONV_fp32_bf16_dmw_lda_ups_bf16 : InstrItinClass;
def II_VLDA_3D_CONV_fp32_bf16_dmx_lda_ups_f16 : InstrItinClass;
def II_VLDA_3D_CONV_fp32_fp16 : InstrItinClass;
def II_VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign0 : InstrItinClass;
def II_VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign1 : InstrItinClass;
def II_VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign0 : InstrItinClass;
def II_VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign1 : InstrItinClass;
def II_VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign0 : InstrItinClass;
def II_VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign1 : InstrItinClass;
def II_VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign0 : InstrItinClass;
def II_VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign1 : InstrItinClass;
def II_VLDA_3D_dmw_lda_feg2 : InstrItinClass;
def II_VLDA_3D_dmw_lda_w : InstrItinClass;
def II_VLDA_3D_dmx_lda_bm : InstrItinClass;
def II_VLDA_3D_dmx_lda_x : InstrItinClass;
def II_VLDA_CONV_fp32_bf16_dmw_lda_ups_bf16_idx : InstrItinClass;
def II_VLDA_CONV_fp32_bf16_dmw_lda_ups_bf16_idx_imm : InstrItinClass;
def II_VLDA_CONV_fp32_bf16_dmw_lda_ups_bf16_pstm_nrm : InstrItinClass;
def II_VLDA_CONV_fp32_bf16_dmw_lda_ups_bf16_pstm_nrm_imm : InstrItinClass;
def II_VLDA_CONV_fp32_bf16_dmx_lda_ups_f16_idx : InstrItinClass;
def II_VLDA_CONV_fp32_bf16_dmx_lda_ups_f16_idx_imm : InstrItinClass;
def II_VLDA_CONV_fp32_bf16_dmx_lda_ups_f16_pstm_nrm : InstrItinClass;
def II_VLDA_CONV_fp32_bf16_dmx_lda_ups_f16_pstm_nrm_imm : InstrItinClass;
def II_VLDA_CONV_fp32_fp16_idx : InstrItinClass;
def II_VLDA_CONV_fp32_fp16_idx_imm : InstrItinClass;
def II_VLDA_CONV_fp32_fp16_pstm_nrm : InstrItinClass;
def II_VLDA_CONV_fp32_fp16_pstm_nrm_imm : InstrItinClass;
def II_VLDA_FILL : InstrItinClass;
def II_VLDA_POP_2D_dmx_lda_fifo_ex_bfp16 : InstrItinClass;
def II_VLDA_POP_2D_dmx_lda_fifo_fex_bfp13 : InstrItinClass;
def II_VLDA_POP_2D_dmx_lda_fifo_x : InstrItinClass;
def II_VLDA_POP_3D_dmx_lda_fifo_ex_bfp16 : InstrItinClass;
def II_VLDA_POP_3D_dmx_lda_fifo_fex_bfp13 : InstrItinClass;
def II_VLDA_POP_3D_dmx_lda_fifo_x : InstrItinClass;
def II_VLDA_POP_dmx_lda_fifo_ex_bfp16_fifo_1d_pop : InstrItinClass;
def II_VLDA_POP_dmx_lda_fifo_ex_bfp16_normal_pop : InstrItinClass;
def II_VLDA_POP_dmx_lda_fifo_fex_bfp13_fifo_1d_pop : InstrItinClass;
def II_VLDA_POP_dmx_lda_fifo_fex_bfp13_normal_pop : InstrItinClass;
def II_VLDA_POP_dmx_lda_fifo_x_fifo_1d_pop : InstrItinClass;
def II_VLDA_POP_dmx_lda_fifo_x_normal_pop : InstrItinClass;
def II_VLDA_POP_mx4_2D : InstrItinClass;
def II_VLDA_POP_mx4_3D : InstrItinClass;
def II_VLDA_POP_mx4_fifo_1d_pop : InstrItinClass;
def II_VLDA_POP_mx4_normal_pop : InstrItinClass;
def II_VLDA_POP_s_2D : InstrItinClass;
def II_VLDA_POP_s_3D : InstrItinClass;
def II_VLDA_POP_s_fifo_1d_pop : InstrItinClass;
def II_VLDA_POP_s_normal_pop : InstrItinClass;
def II_VLDA_UPS_2x_dmw_lda_ups_w2b_idx_upsSign0 : InstrItinClass;
def II_VLDA_UPS_2x_dmw_lda_ups_w2b_idx_upsSign1 : InstrItinClass;
def II_VLDA_UPS_2x_dmw_lda_ups_w2b_idx_imm_upsSign0 : InstrItinClass;
def II_VLDA_UPS_2x_dmw_lda_ups_w2b_idx_imm_upsSign1 : InstrItinClass;
def II_VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_upsSign0 : InstrItinClass;
def II_VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_upsSign1 : InstrItinClass;
def II_VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_imm_upsSign0 : InstrItinClass;
def II_VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_imm_upsSign1 : InstrItinClass;
def II_VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign0 : InstrItinClass;
def II_VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign1 : InstrItinClass;
def II_VLDA_UPS_2x_dmx_lda_ups_x2c_idx_imm_upsSign0 : InstrItinClass;
def II_VLDA_UPS_2x_dmx_lda_ups_x2c_idx_imm_upsSign1 : InstrItinClass;
def II_VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_upsSign0 : InstrItinClass;
def II_VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_upsSign1 : InstrItinClass;
def II_VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_imm_upsSign0 : InstrItinClass;
def II_VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_imm_upsSign1 : InstrItinClass;
def II_VLDA_UPS_4x_dmw_lda_ups_w2c_idx_upsSign0 : InstrItinClass;
def II_VLDA_UPS_4x_dmw_lda_ups_w2c_idx_upsSign1 : InstrItinClass;
def II_VLDA_UPS_4x_dmw_lda_ups_w2c_idx_imm_upsSign0 : InstrItinClass;
def II_VLDA_UPS_4x_dmw_lda_ups_w2c_idx_imm_upsSign1 : InstrItinClass;
def II_VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_upsSign0 : InstrItinClass;
def II_VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_upsSign1 : InstrItinClass;
def II_VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_imm_upsSign0 : InstrItinClass;
def II_VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_imm_upsSign1 : InstrItinClass;
def II_VLDA_UPS_4x_dmx_lda_ups_x2d_idx_upsSign0 : InstrItinClass;
def II_VLDA_UPS_4x_dmx_lda_ups_x2d_idx_upsSign1 : InstrItinClass;
def II_VLDA_UPS_4x_dmx_lda_ups_x2d_idx_imm_upsSign0 : InstrItinClass;
def II_VLDA_UPS_4x_dmx_lda_ups_x2d_idx_imm_upsSign1 : InstrItinClass;
def II_VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_upsSign0 : InstrItinClass;
def II_VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_upsSign1 : InstrItinClass;
def II_VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_imm_upsSign0 : InstrItinClass;
def II_VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_imm_upsSign1 : InstrItinClass;
def II_VLDA_dmw_lda_feg2_ld_idx : InstrItinClass;
def II_VLDA_dmw_lda_feg2_ld_idx_imm : InstrItinClass;
def II_VLDA_dmw_lda_feg2_ld_pstm_nrm : InstrItinClass;
def II_VLDA_dmw_lda_feg2_ld_pstm_nrm_imm : InstrItinClass;
def II_VLDA_dmw_lda_feg2_spill : InstrItinClass;
def II_VLDA_dmw_lda_w_ld_idx : InstrItinClass;
def II_VLDA_dmw_lda_w_ld_idx_imm : InstrItinClass;
def II_VLDA_dmw_lda_w_ld_pstm_nrm : InstrItinClass;
def II_VLDA_dmw_lda_w_ld_pstm_nrm_imm : InstrItinClass;
def II_VLDA_dmw_lda_w_spill : InstrItinClass;
def II_VLDA_dmx_lda_bm_ld_idx : InstrItinClass;
def II_VLDA_dmx_lda_bm_ld_idx_imm : InstrItinClass;
def II_VLDA_dmx_lda_bm_ld_pstm_nrm : InstrItinClass;
def II_VLDA_dmx_lda_bm_ld_pstm_nrm_imm : InstrItinClass;
def II_VLDA_dmx_lda_bm_spill : InstrItinClass;
def II_VLDA_dmx_lda_x_ld_idx : InstrItinClass;
def II_VLDA_dmx_lda_x_ld_idx_imm : InstrItinClass;
def II_VLDA_dmx_lda_x_ld_pstm_nrm : InstrItinClass;
def II_VLDA_dmx_lda_x_ld_pstm_nrm_imm : InstrItinClass;
def II_VLDA_dmx_lda_x_spill : InstrItinClass;
def II_VLDB_128_idx : InstrItinClass;
def II_VLDB_128_idx_imm : InstrItinClass;
def II_VLDB_128_pstm_nrm : InstrItinClass;
def II_VLDB_128_pstm_nrm_imm : InstrItinClass;
def II_VLDB_2D_128 : InstrItinClass;
def II_VLDB_2D_UNPACK_dmw_ldb_unpack_unpackSign0 : InstrItinClass;
def II_VLDB_2D_UNPACK_dmw_ldb_unpack_unpackSign1 : InstrItinClass;
def II_VLDB_2D_UNPACK_dmx_ldb_unpack_unpackSign0 : InstrItinClass;
def II_VLDB_2D_UNPACK_dmx_ldb_unpack_unpackSign1 : InstrItinClass;
def II_VLDB_2D_dmw_ldb : InstrItinClass;
def II_VLDB_2D_dmx_ldb_x : InstrItinClass;
def II_VLDB_3D_128 : InstrItinClass;
def II_VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign0 : InstrItinClass;
def II_VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign1 : InstrItinClass;
def II_VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign0 : InstrItinClass;
def II_VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign1 : InstrItinClass;
def II_VLDB_3D_dmw_ldb : InstrItinClass;
def II_VLDB_3D_dmx_ldb_x : InstrItinClass;
def II_VLDB_4x16_hi : InstrItinClass;
def II_VLDB_4x16_lo : InstrItinClass;
def II_VLDB_4x32_hi : InstrItinClass;
def II_VLDB_4x32_lo : InstrItinClass;
def II_VLDB_4x64_hi : InstrItinClass;
def II_VLDB_4x64_lo : InstrItinClass;
def II_VLDB_FILLX : InstrItinClass;
def II_VLDB_FILL : InstrItinClass;
def II_VLDB_POPX : InstrItinClass;
def II_VLDB_POP_2D_dmx_ldb_fifo_ex_bfp16 : InstrItinClass;
def II_VLDB_POP_2D_dmx_ldb_fifo_fex_bfp13 : InstrItinClass;
def II_VLDB_POP_2D_dmx_ldb_fifo_x : InstrItinClass;
def II_VLDB_POP_3D_dmx_ldb_fifo_ex_bfp16 : InstrItinClass;
def II_VLDB_POP_3D_dmx_ldb_fifo_fex_bfp13 : InstrItinClass;
def II_VLDB_POP_3D_dmx_ldb_fifo_x : InstrItinClass;
def II_VLDB_POP_dmx_ldb_fifo_ex_bfp16_fifo_1d_pop : InstrItinClass;
def II_VLDB_POP_dmx_ldb_fifo_ex_bfp16_normal_pop : InstrItinClass;
def II_VLDB_POP_dmx_ldb_fifo_fex_bfp13_fifo_1d_pop : InstrItinClass;
def II_VLDB_POP_dmx_ldb_fifo_fex_bfp13_normal_pop : InstrItinClass;
def II_VLDB_POP_dmx_ldb_fifo_x_fifo_1d_pop : InstrItinClass;
def II_VLDB_POP_dmx_ldb_fifo_x_normal_pop : InstrItinClass;
def II_VLDB_POP_mx4_2D : InstrItinClass;
def II_VLDB_POP_mx4_3D : InstrItinClass;
def II_VLDB_POP_mx4_fifo_1d_pop : InstrItinClass;
def II_VLDB_POP_mx4_normal_pop : InstrItinClass;
def II_VLDB_POP_s_2D : InstrItinClass;
def II_VLDB_POP_s_3D : InstrItinClass;
def II_VLDB_POP_s_fifo_1d_pop : InstrItinClass;
def II_VLDB_POP_s_normal_pop : InstrItinClass;
def II_VLDB_UNPACK_dmw_ldb_unpack_idx_unpackSign0 : InstrItinClass;
def II_VLDB_UNPACK_dmw_ldb_unpack_idx_unpackSign1 : InstrItinClass;
def II_VLDB_UNPACK_dmw_ldb_unpack_idx_imm_unpackSign0 : InstrItinClass;
def II_VLDB_UNPACK_dmw_ldb_unpack_idx_imm_unpackSign1 : InstrItinClass;
def II_VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_unpackSign0 : InstrItinClass;
def II_VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_unpackSign1 : InstrItinClass;
def II_VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_imm_unpackSign0 : InstrItinClass;
def II_VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_imm_unpackSign1 : InstrItinClass;
def II_VLDB_UNPACK_dmx_ldb_unpack_idx_unpackSign0 : InstrItinClass;
def II_VLDB_UNPACK_dmx_ldb_unpack_idx_unpackSign1 : InstrItinClass;
def II_VLDB_UNPACK_dmx_ldb_unpack_idx_imm_unpackSign0 : InstrItinClass;
def II_VLDB_UNPACK_dmx_ldb_unpack_idx_imm_unpackSign1 : InstrItinClass;
def II_VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_unpackSign0 : InstrItinClass;
def II_VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_unpackSign1 : InstrItinClass;
def II_VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_imm_unpackSign0 : InstrItinClass;
def II_VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_imm_unpackSign1 : InstrItinClass;
def II_VLDB_dmw_ldb_idx : InstrItinClass;
def II_VLDB_dmw_ldb_idx_imm : InstrItinClass;
def II_VLDB_dmw_ldb_pstm_nrm : InstrItinClass;
def II_VLDB_dmw_ldb_pstm_nrm_imm : InstrItinClass;
def II_VLDB_dmx_ldb_x_idx : InstrItinClass;
def II_VLDB_dmx_ldb_x_idx_imm : InstrItinClass;
def II_VLDB_dmx_ldb_x_pstm_nrm : InstrItinClass;
def II_VLDB_dmx_ldb_x_pstm_nrm_imm : InstrItinClass;
def II_VLT_16_vaddSign0 : InstrItinClass;
def II_VLT_16_vaddSign1 : InstrItinClass;
def II_VLT_32_vaddSign0 : InstrItinClass;
def II_VLT_32_vaddSign1 : InstrItinClass;
def II_VLT_8_vaddSign0 : InstrItinClass;
def II_VLT_8_vaddSign1 : InstrItinClass;
def II_VLT_bf16 : InstrItinClass;
def II_VLT_bf8 : InstrItinClass;
def II_VLT_fp16 : InstrItinClass;
def II_VLT_fp8 : InstrItinClass;
def II_VMAC_vmac_vmul_cm_core_X_QX : InstrItinClass;
def II_VMAC_vmac_vmul_cm_core_X_QY : InstrItinClass;
def II_VMAC_vmac_vmul_cm_core_X_R : InstrItinClass;
def II_VMAC_vmac_vmul_cm_core_X_X : InstrItinClass;
def II_VMAC_vmac_vmul_cm_core_X_Y : InstrItinClass;
def II_VMAC_vmac_vmul_cm_core_Y_QX : InstrItinClass;
def II_VMAC_vmac_vmul_cm_core_Y_R : InstrItinClass;
def II_VMAC_vmac_vmul_cm_core_Y_X : InstrItinClass;
def II_VMAC_vmac_vmul_cm_core_Y_Y : InstrItinClass;
def II_VMAC_vmac_half_vmul_cm_half_core_X_QX : InstrItinClass;
def II_VMAC_vmac_half_vmul_cm_half_core_X_R : InstrItinClass;
def II_VMAC_vmac_half_vmul_cm_half_core_X_X : InstrItinClass;
def II_VMAC_f_vmac_bf_vmul_bf_core_X_QY : InstrItinClass;
def II_VMAC_f_vmac_bf_vmul_bf_core_X_X : InstrItinClass;
def II_VMAC_f_vmac_bf_vmul_bf_core_Y_R : InstrItinClass;
def II_VMAC_f_vmac_bf_vmul_bf_core_Y_Y : InstrItinClass;
def II_VMAC_f_vmac_bf_half_vmul_bf_core_X_R : InstrItinClass;
def II_VMAC_f_vmac_bf_half_vmul_bf_core_X_Y : InstrItinClass;
def II_VMAC_f_vmac_bf_half_vmul_bf_core_Y_QY : InstrItinClass;
def II_VMAC_f_vmac_bf_half_vmul_bf_half_core_X_X : InstrItinClass;
def II_VMAC_f_vmac_bfp13 : InstrItinClass;
def II_VMAC_f_vmac_bfp16 : InstrItinClass;
def II_VMAXDIFF_LT_16_vaddSign0 : InstrItinClass;
def II_VMAXDIFF_LT_16_vaddSign1 : InstrItinClass;
def II_VMAXDIFF_LT_32_vaddSign0 : InstrItinClass;
def II_VMAXDIFF_LT_32_vaddSign1 : InstrItinClass;
def II_VMAXDIFF_LT_8_vaddSign0 : InstrItinClass;
def II_VMAXDIFF_LT_8_vaddSign1 : InstrItinClass;
def II_VMAX_LT_16_vaddSign0 : InstrItinClass;
def II_VMAX_LT_16_vaddSign1 : InstrItinClass;
def II_VMAX_LT_32_vaddSign0 : InstrItinClass;
def II_VMAX_LT_32_vaddSign1 : InstrItinClass;
def II_VMAX_LT_8_vaddSign0 : InstrItinClass;
def II_VMAX_LT_8_vaddSign1 : InstrItinClass;
def II_VMAX_LT_bf16 : InstrItinClass;
def II_VMAX_LT_bf8 : InstrItinClass;
def II_VMAX_LT_fp16 : InstrItinClass;
def II_VMAX_LT_fp8 : InstrItinClass;
def II_VMIN_GE_16_vaddSign0 : InstrItinClass;
def II_VMIN_GE_16_vaddSign1 : InstrItinClass;
def II_VMIN_GE_32_vaddSign0 : InstrItinClass;
def II_VMIN_GE_32_vaddSign1 : InstrItinClass;
def II_VMIN_GE_8_vaddSign0 : InstrItinClass;
def II_VMIN_GE_8_vaddSign1 : InstrItinClass;
def II_VMIN_GE_bf16 : InstrItinClass;
def II_VMIN_GE_bf8 : InstrItinClass;
def II_VMIN_GE_fp16 : InstrItinClass;
def II_VMIN_GE_fp8 : InstrItinClass;
def II_VMOV_0_mv_scd_cm : InstrItinClass;
def II_VMOV_0_mv_scd_dm_imm : InstrItinClass;
def II_VMOV_1_mv_scd_cm : InstrItinClass;
def II_VMOV_1_mv_scd_dm_imm : InstrItinClass;
def II_VMOV_2 : InstrItinClass;
def II_VMOV_3 : InstrItinClass;
def II_VMOV_D_vmov : InstrItinClass;
def II_VMOV_D_vmov_half : InstrItinClass;
def II_VMOV_alu_mv_alu_mv_scd_bm : InstrItinClass;
def II_VMOV_alu_mv_alu_mv_scd_cmh_dyn : InstrItinClass;
def II_VMOV_alu_mv_alu_mv_scd_cmh_reg : InstrItinClass;
def II_VMOV_alu_mv_alu_mv_scd_cml_dyn : InstrItinClass;
def II_VMOV_alu_mv_alu_mv_scd_cml_reg : InstrItinClass;
def II_VMOV_alu_mv_alu_mv_scd_dm_dyn : InstrItinClass;
def II_VMOV_alu_mv_alu_mv_scd_dm_reg : InstrItinClass;
def II_VMOV_alu_mv_alu_mv_scd_x : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_cm : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_ee : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_eg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_eg2 : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_eg2_to_w : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_egw : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_egx : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_f : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_f_to_l : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_few : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_fex : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_ff : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_gg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_l_to_f : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_w : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_w_to_eg2 : InstrItinClass;
// instruction: VMOV_alu_mv_mv_mv_x
def II_VMOV_alu_mv_mv_mv_x : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXe_eXe : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifol_eXe : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eXe : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHH_eXe : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHL_eXe : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eXe : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eXe : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLL_eXe : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLH_eXe : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifoh_eXe : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXo_eXe : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXe_mStFifol : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXe_eLdFifoLReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXe_eBMHH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXe_eBMHL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXe_eLdFifoHReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXe_mFifoExtra : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXe_eBMLL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXe_eBMLH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXe_mStFifoh : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXe_eXo : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifol_mStFifol : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifol_eLdFifoLReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifol_eBMHH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifol_eBMHL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifol_eLdFifoHReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifol_mFifoExtra : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifol_eBMLL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifol_eBMLH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifol_mStFifoh : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifol_eXo : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_mStFifol : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eLdFifoLReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eBMHH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eBMHL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eLdFifoHReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_mFifoExtra : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eBMLL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eBMLH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_mStFifoh : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eXo : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHH_mStFifol : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHH_eLdFifoLReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHH_eBMHH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHH_eBMHL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHH_eLdFifoHReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHH_mFifoExtra : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHH_eBMLL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHH_eBMLH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHH_mStFifoh : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHH_eXo : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHL_mStFifol : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHL_eLdFifoLReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHL_eBMHH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHL_eBMHL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHL_eLdFifoHReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHL_mFifoExtra : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHL_eBMLL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHL_eBMLH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHL_mStFifoh : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMHL_eXo : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_mStFifol : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eLdFifoLReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eBMHH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eBMHL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eLdFifoHReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_mFifoExtra : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eBMLL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eBMLH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_mStFifoh : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eXo : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mFifoExtra_mStFifol : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eLdFifoLReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eBMHH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eBMHL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eLdFifoHReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mFifoExtra_mFifoExtra : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eBMLL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eBMLH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mFifoExtra_mStFifoh : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eXo : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLL_mStFifol : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLL_eLdFifoLReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLL_eBMHH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLL_eBMHL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLL_eLdFifoHReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLL_mFifoExtra : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLL_eBMLL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLL_eBMLH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLL_mStFifoh : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLL_eXo : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLH_mStFifol : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLH_eLdFifoLReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLH_eBMHH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLH_eBMHL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLH_eLdFifoHReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLH_mFifoExtra : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLH_eBMLL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLH_eBMLH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLH_mStFifoh : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eBMLH_eXo : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifoh_mStFifol : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifoh_eLdFifoLReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifoh_eBMHH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifoh_eBMHL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifoh_eLdFifoHReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifoh_mFifoExtra : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifoh_eBMLL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifoh_eBMLH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifoh_mStFifoh : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_mStFifoh_eXo : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXo_mStFifol : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXo_eLdFifoLReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXo_eBMHH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXo_eBMHL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXo_eLdFifoHReg : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXo_mFifoExtra : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXo_eBMLL : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXo_eBMLH : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXo_mStFifoh : InstrItinClass;
def II_VMOV_alu_mv_mv_mv_x_eXo_eXo : InstrItinClass;

def II_VMOV_st_mv_mcd_bm : InstrItinClass;
def II_VMOV_st_mv_mcd_x : InstrItinClass;
def II_VMSC_vmac_vmul_cm_core_X_QX : InstrItinClass;
def II_VMSC_vmac_vmul_cm_core_X_QY : InstrItinClass;
def II_VMSC_vmac_vmul_cm_core_X_R : InstrItinClass;
def II_VMSC_vmac_vmul_cm_core_X_X : InstrItinClass;
def II_VMSC_vmac_vmul_cm_core_X_Y : InstrItinClass;
def II_VMSC_vmac_vmul_cm_core_Y_QX : InstrItinClass;
def II_VMSC_vmac_vmul_cm_core_Y_R : InstrItinClass;
def II_VMSC_vmac_vmul_cm_core_Y_X : InstrItinClass;
def II_VMSC_vmac_vmul_cm_core_Y_Y : InstrItinClass;
def II_VMSC_vmac_half_vmul_cm_half_core_X_QX : InstrItinClass;
def II_VMSC_vmac_half_vmul_cm_half_core_X_R : InstrItinClass;
def II_VMSC_vmac_half_vmul_cm_half_core_X_X : InstrItinClass;
def II_VMSC_f_vmac_bf_vmul_bf_core_X_QY : InstrItinClass;
def II_VMSC_f_vmac_bf_vmul_bf_core_X_X : InstrItinClass;
def II_VMSC_f_vmac_bf_vmul_bf_core_Y_R : InstrItinClass;
def II_VMSC_f_vmac_bf_vmul_bf_core_Y_Y : InstrItinClass;
def II_VMSC_f_vmac_bf_half_vmul_bf_core_X_R : InstrItinClass;
def II_VMSC_f_vmac_bf_half_vmul_bf_core_X_Y : InstrItinClass;
def II_VMSC_f_vmac_bf_half_vmul_bf_core_Y_QY : InstrItinClass;
def II_VMSC_f_vmac_bf_half_vmul_bf_half_core_X_X : InstrItinClass;
def II_VMSC_f_vmac_bfp13 : InstrItinClass;
def II_VMSC_f_vmac_bfp16 : InstrItinClass;
def II_VMUL_vmul_vmul_cm_core_X_QX : InstrItinClass;
def II_VMUL_vmul_vmul_cm_core_X_QY : InstrItinClass;
def II_VMUL_vmul_vmul_cm_core_X_R : InstrItinClass;
def II_VMUL_vmul_vmul_cm_core_X_X : InstrItinClass;
def II_VMUL_vmul_vmul_cm_core_X_Y : InstrItinClass;
def II_VMUL_vmul_vmul_cm_core_Y_QX : InstrItinClass;
def II_VMUL_vmul_vmul_cm_core_Y_R : InstrItinClass;
def II_VMUL_vmul_vmul_cm_core_Y_X : InstrItinClass;
def II_VMUL_vmul_vmul_cm_core_Y_Y : InstrItinClass;
def II_VMUL_vmul_half_vmul_cm_half_core_X_QX : InstrItinClass;
def II_VMUL_vmul_half_vmul_cm_half_core_X_R : InstrItinClass;
def II_VMUL_vmul_half_vmul_cm_half_core_X_X : InstrItinClass;
def II_VMUL_f_vmul_bf_vmul_bf_core_X_QY : InstrItinClass;
def II_VMUL_f_vmul_bf_vmul_bf_core_X_X : InstrItinClass;
def II_VMUL_f_vmul_bf_vmul_bf_core_Y_R : InstrItinClass;
def II_VMUL_f_vmul_bf_vmul_bf_core_Y_Y : InstrItinClass;
def II_VMUL_f_vmul_bf_half_vmul_bf_core_X_R : InstrItinClass;
def II_VMUL_f_vmul_bf_half_vmul_bf_core_X_Y : InstrItinClass;
def II_VMUL_f_vmul_bf_half_vmul_bf_core_Y_QY : InstrItinClass;
def II_VMUL_f_vmul_bf_half_vmul_bf_half_core_X_X : InstrItinClass;
def II_VMUL_f_vmul_bfp13 : InstrItinClass;
def II_VMUL_f_vmul_bfp16 : InstrItinClass;
def II_VNEGMUL_vmul_vmul_cm_core_X_QX : InstrItinClass;
def II_VNEGMUL_vmul_vmul_cm_core_X_QY : InstrItinClass;
def II_VNEGMUL_vmul_vmul_cm_core_X_R : InstrItinClass;
def II_VNEGMUL_vmul_vmul_cm_core_X_X : InstrItinClass;
def II_VNEGMUL_vmul_vmul_cm_core_X_Y : InstrItinClass;
def II_VNEGMUL_vmul_vmul_cm_core_Y_QX : InstrItinClass;
def II_VNEGMUL_vmul_vmul_cm_core_Y_R : InstrItinClass;
def II_VNEGMUL_vmul_vmul_cm_core_Y_X : InstrItinClass;
def II_VNEGMUL_vmul_vmul_cm_core_Y_Y : InstrItinClass;
def II_VNEGMUL_vmul_half_vmul_cm_half_core_X_QX : InstrItinClass;
def II_VNEGMUL_vmul_half_vmul_cm_half_core_X_R : InstrItinClass;
def II_VNEGMUL_vmul_half_vmul_cm_half_core_X_X : InstrItinClass;
def II_VNEGMUL_f_vmul_bf_vmul_bf_core_X_QY : InstrItinClass;
def II_VNEGMUL_f_vmul_bf_vmul_bf_core_X_X : InstrItinClass;
def II_VNEGMUL_f_vmul_bf_vmul_bf_core_Y_R : InstrItinClass;
def II_VNEGMUL_f_vmul_bf_vmul_bf_core_Y_Y : InstrItinClass;
def II_VNEGMUL_f_vmul_bf_half_vmul_bf_core_X_R : InstrItinClass;
def II_VNEGMUL_f_vmul_bf_half_vmul_bf_core_X_Y : InstrItinClass;
def II_VNEGMUL_f_vmul_bf_half_vmul_bf_core_Y_QY : InstrItinClass;
def II_VNEGMUL_f_vmul_bf_half_vmul_bf_half_core_X_X : InstrItinClass;
def II_VNEGMUL_f_vmul_bfp13 : InstrItinClass;
def II_VNEGMUL_f_vmul_bfp16 : InstrItinClass;
def II_VNEG_GTZ_16 : InstrItinClass;
def II_VNEG_GTZ_32 : InstrItinClass;
def II_VNEG_GTZ_8 : InstrItinClass;
def II_VNEG_vneg : InstrItinClass;
def II_VNEG_vneg_half : InstrItinClass;
def II_VNEG_f_vneg_fp : InstrItinClass;
def II_VNEG_f_vneg_half_fp : InstrItinClass;
def II_VNEG_mx6_vneg_fp_bfp13 : InstrItinClass;
def II_VNEG_mx6_vneg_half_fp_bfp13 : InstrItinClass;
def II_VPACK_mv_pack_w_packSign0 : InstrItinClass;
def II_VPACK_mv_pack_w_packSign1 : InstrItinClass;
def II_VPACK_mv_pack_x_packSign0 : InstrItinClass;
def II_VPACK_mv_pack_x_packSign1 : InstrItinClass;
def II_VPUSH_hi_16 : InstrItinClass;
def II_VPUSH_hi_32 : InstrItinClass;
def II_VPUSH_hi_64 : InstrItinClass;
def II_VPUSH_hi_8 : InstrItinClass;
def II_VPUSH_lo_16 : InstrItinClass;
def II_VPUSH_lo_32 : InstrItinClass;
def II_VPUSH_lo_64 : InstrItinClass;
def II_VPUSH_lo_8 : InstrItinClass;
def II_VSEL_16 : InstrItinClass;
def II_VSEL_32 : InstrItinClass;
def II_VSEL_8 : InstrItinClass;
def II_VSHIFT_ALIGN : InstrItinClass;
def II_VSHIFT : InstrItinClass;
def II_VSHUFFLE_vec_shuffle_bm : InstrItinClass;
def II_VSHUFFLE_vec_shuffle_egx : InstrItinClass;
def II_VSHUFFLE_vec_shuffle_fex : InstrItinClass;
def II_VSHUFFLE_vec_shuffle_x : InstrItinClass;
def II_VSRS_2x_mv_w_srs_bm_srsSign0 : InstrItinClass;
def II_VSRS_2x_mv_w_srs_bm_srsSign1 : InstrItinClass;
def II_VSRS_2x_mv_x_srs_cm_srsSign0 : InstrItinClass;
def II_VSRS_2x_mv_x_srs_cm_srsSign1 : InstrItinClass;
def II_VSRS_4x_mv_w_srs_cm_srsSign0 : InstrItinClass;
def II_VSRS_4x_mv_w_srs_cm_srsSign1 : InstrItinClass;
def II_VSRS_4x_mv_x_srs_dm_srsSign0 : InstrItinClass;
def II_VSRS_4x_mv_x_srs_dm_srsSign1 : InstrItinClass;
def II_VST_128_dmv_sts_eg_spill : InstrItinClass;
def II_VST_128_dmv_sts_eg_st_idx : InstrItinClass;
def II_VST_128_dmv_sts_eg_st_idx_imm : InstrItinClass;
def II_VST_128_dmv_sts_eg_st_pstm_nrm : InstrItinClass;
def II_VST_128_dmv_sts_eg_st_pstm_nrm_imm : InstrItinClass;
def II_VST_128_dmv_sts_feg_spill : InstrItinClass;
def II_VST_128_dmv_sts_feg_st_idx : InstrItinClass;
def II_VST_128_dmv_sts_feg_st_idx_imm : InstrItinClass;
def II_VST_128_dmv_sts_feg_st_pstm_nrm : InstrItinClass;
def II_VST_128_dmv_sts_feg_st_pstm_nrm_imm : InstrItinClass;
def II_VST_128_dmv_sts_w_spill : InstrItinClass;
def II_VST_128_dmv_sts_w_st_idx : InstrItinClass;
def II_VST_128_dmv_sts_w_st_idx_imm : InstrItinClass;
def II_VST_128_dmv_sts_w_st_pstm_nrm : InstrItinClass;
def II_VST_128_dmv_sts_w_st_pstm_nrm_imm : InstrItinClass;
def II_VST_2D_128_dmv_sts_eg : InstrItinClass;
def II_VST_2D_128_dmv_sts_feg : InstrItinClass;
def II_VST_2D_128_dmv_sts_w : InstrItinClass;
def II_VST_2D_CONV_bf16_fp32_dmw_sts_srs_bf16 : InstrItinClass;
def II_VST_2D_CONV_bf16_fp32_dmx_sts_srs_bf16 : InstrItinClass;
def II_VST_2D_CONV_bf8_fp32 : InstrItinClass;
def II_VST_2D_CONV_fp16_fp32 : InstrItinClass;
def II_VST_2D_CONV_fp8_fp32 : InstrItinClass;
def II_VST_2D_PACK_dmw_sts_pack_packSign0 : InstrItinClass;
def II_VST_2D_PACK_dmw_sts_pack_packSign1 : InstrItinClass;
def II_VST_2D_PACK_dmx_sts_pack_packSign0 : InstrItinClass;
def II_VST_2D_PACK_dmx_sts_pack_packSign1 : InstrItinClass;
def II_VST_2D_SRS_2x_dm_sts_srs_cm_srsSign0 : InstrItinClass;
def II_VST_2D_SRS_2x_dm_sts_srs_cm_srsSign1 : InstrItinClass;
def II_VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign0 : InstrItinClass;
def II_VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign1 : InstrItinClass;
def II_VST_2D_SRS_4x_dm_sts_srs_cm_srsSign0 : InstrItinClass;
def II_VST_2D_SRS_4x_dm_sts_srs_cm_srsSign1 : InstrItinClass;
def II_VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign0 : InstrItinClass;
def II_VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign1 : InstrItinClass;
def II_VST_2D_dmw_sts_feg2 : InstrItinClass;
def II_VST_2D_dmw_sts_w : InstrItinClass;
def II_VST_2D_dmx_sts_bm : InstrItinClass;
def II_VST_2D_dmx_sts_x : InstrItinClass;
def II_VST_3D_128_dmv_sts_eg : InstrItinClass;
def II_VST_3D_128_dmv_sts_feg : InstrItinClass;
def II_VST_3D_128_dmv_sts_w : InstrItinClass;
def II_VST_3D_CONV_bf16_fp32_dmw_sts_srs_bf16 : InstrItinClass;
def II_VST_3D_CONV_bf16_fp32_dmx_sts_srs_bf16 : InstrItinClass;
def II_VST_3D_CONV_bf8_fp32 : InstrItinClass;
def II_VST_3D_CONV_fp16_fp32 : InstrItinClass;
def II_VST_3D_CONV_fp8_fp32 : InstrItinClass;
def II_VST_3D_PACK_dmw_sts_pack_packSign0 : InstrItinClass;
def II_VST_3D_PACK_dmw_sts_pack_packSign1 : InstrItinClass;
def II_VST_3D_PACK_dmx_sts_pack_packSign0 : InstrItinClass;
def II_VST_3D_PACK_dmx_sts_pack_packSign1 : InstrItinClass;
def II_VST_3D_SRS_2x_dm_sts_srs_cm_srsSign0 : InstrItinClass;
def II_VST_3D_SRS_2x_dm_sts_srs_cm_srsSign1 : InstrItinClass;
def II_VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign0 : InstrItinClass;
def II_VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign1 : InstrItinClass;
def II_VST_3D_SRS_4x_dm_sts_srs_cm_srsSign0 : InstrItinClass;
def II_VST_3D_SRS_4x_dm_sts_srs_cm_srsSign1 : InstrItinClass;
def II_VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign0 : InstrItinClass;
def II_VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign1 : InstrItinClass;
def II_VST_3D_dmw_sts_feg2 : InstrItinClass;
def II_VST_3D_dmw_sts_w : InstrItinClass;
def II_VST_3D_dmx_sts_bm : InstrItinClass;
def II_VST_3D_dmx_sts_x : InstrItinClass;
def II_VST_CONV_bf16_fp32_dmw_sts_srs_bf16_idx : InstrItinClass;
def II_VST_CONV_bf16_fp32_dmw_sts_srs_bf16_idx_imm : InstrItinClass;
def II_VST_CONV_bf16_fp32_dmw_sts_srs_bf16_pstm_nrm : InstrItinClass;
def II_VST_CONV_bf16_fp32_dmw_sts_srs_bf16_pstm_nrm_imm : InstrItinClass;
def II_VST_CONV_bf16_fp32_dmx_sts_srs_bf16_idx : InstrItinClass;
def II_VST_CONV_bf16_fp32_dmx_sts_srs_bf16_idx_imm : InstrItinClass;
def II_VST_CONV_bf16_fp32_dmx_sts_srs_bf16_pstm_nrm : InstrItinClass;
def II_VST_CONV_bf16_fp32_dmx_sts_srs_bf16_pstm_nrm_imm : InstrItinClass;
def II_VST_CONV_bf8_fp32_idx : InstrItinClass;
def II_VST_CONV_bf8_fp32_idx_imm : InstrItinClass;
def II_VST_CONV_bf8_fp32_pstm_nrm : InstrItinClass;
def II_VST_CONV_bf8_fp32_pstm_nrm_imm : InstrItinClass;
def II_VST_CONV_fp16_fp32_idx : InstrItinClass;
def II_VST_CONV_fp16_fp32_idx_imm : InstrItinClass;
def II_VST_CONV_fp16_fp32_pstm_nrm : InstrItinClass;
def II_VST_CONV_fp16_fp32_pstm_nrm_imm : InstrItinClass;
def II_VST_CONV_fp8_fp32_idx : InstrItinClass;
def II_VST_CONV_fp8_fp32_idx_imm : InstrItinClass;
def II_VST_CONV_fp8_fp32_pstm_nrm : InstrItinClass;
def II_VST_CONV_fp8_fp32_pstm_nrm_imm : InstrItinClass;
def II_VST_FLUSH_2D : InstrItinClass;
def II_VST_FLUSH_3D : InstrItinClass;
def II_VST_FLUSH_fifo_1d_flush : InstrItinClass;
def II_VST_FLUSH_normal_flush : InstrItinClass;
def II_VST_FLUSH_d_2D : InstrItinClass;
def II_VST_FLUSH_d_3D : InstrItinClass;
def II_VST_FLUSH_d_fifo_1d_flush : InstrItinClass;
def II_VST_FLUSH_d_normal_flush : InstrItinClass;
def II_VST_PACK_dmw_sts_pack_idx_packSign0 : InstrItinClass;
def II_VST_PACK_dmw_sts_pack_idx_packSign1 : InstrItinClass;
def II_VST_PACK_dmw_sts_pack_idx_imm_packSign0 : InstrItinClass;
def II_VST_PACK_dmw_sts_pack_idx_imm_packSign1 : InstrItinClass;
def II_VST_PACK_dmw_sts_pack_pstm_nrm_packSign0 : InstrItinClass;
def II_VST_PACK_dmw_sts_pack_pstm_nrm_packSign1 : InstrItinClass;
def II_VST_PACK_dmw_sts_pack_pstm_nrm_imm_packSign0 : InstrItinClass;
def II_VST_PACK_dmw_sts_pack_pstm_nrm_imm_packSign1 : InstrItinClass;
def II_VST_PACK_dmx_sts_pack_idx_packSign0 : InstrItinClass;
def II_VST_PACK_dmx_sts_pack_idx_packSign1 : InstrItinClass;
def II_VST_PACK_dmx_sts_pack_idx_imm_packSign0 : InstrItinClass;
def II_VST_PACK_dmx_sts_pack_idx_imm_packSign1 : InstrItinClass;
def II_VST_PACK_dmx_sts_pack_pstm_nrm_packSign0 : InstrItinClass;
def II_VST_PACK_dmx_sts_pack_pstm_nrm_packSign1 : InstrItinClass;
def II_VST_PACK_dmx_sts_pack_pstm_nrm_imm_packSign0 : InstrItinClass;
def II_VST_PACK_dmx_sts_pack_pstm_nrm_imm_packSign1 : InstrItinClass;
def II_VST_PUSH_256_CONV_mx4_fp32 : InstrItinClass;
def II_VST_PUSH_256 : InstrItinClass;
def II_VST_PUSH_384_CONV_mx6_fp32 : InstrItinClass;
def II_VST_PUSH_384 : InstrItinClass;
def II_VST_PUSH_512_dmx_sts_fifo_bare_fex_bfp11 : InstrItinClass;
def II_VST_PUSH_512_dmx_sts_fifo_bare_x : InstrItinClass;
def II_VST_PUSH_576_CONV_mx9_fp32 : InstrItinClass;
def II_VST_PUSH_576 : InstrItinClass;
def II_VST_PUSH_768 : InstrItinClass;
def II_VST_SRS_2x_dm_sts_srs_cm_idx_srsSign0 : InstrItinClass;
def II_VST_SRS_2x_dm_sts_srs_cm_idx_srsSign1 : InstrItinClass;
def II_VST_SRS_2x_dm_sts_srs_cm_idx_imm_srsSign0 : InstrItinClass;
def II_VST_SRS_2x_dm_sts_srs_cm_idx_imm_srsSign1 : InstrItinClass;
def II_VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_srsSign0 : InstrItinClass;
def II_VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_srsSign1 : InstrItinClass;
def II_VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_imm_srsSign0 : InstrItinClass;
def II_VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_imm_srsSign1 : InstrItinClass;
def II_VST_SRS_2x_dmw_sts_srs_bm_idx_srsSign0 : InstrItinClass;
def II_VST_SRS_2x_dmw_sts_srs_bm_idx_srsSign1 : InstrItinClass;
def II_VST_SRS_2x_dmw_sts_srs_bm_idx_imm_srsSign0 : InstrItinClass;
def II_VST_SRS_2x_dmw_sts_srs_bm_idx_imm_srsSign1 : InstrItinClass;
def II_VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_srsSign0 : InstrItinClass;
def II_VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_srsSign1 : InstrItinClass;
def II_VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_imm_srsSign0 : InstrItinClass;
def II_VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_imm_srsSign1 : InstrItinClass;
def II_VST_SRS_4x_dm_sts_srs_cm_idx_srsSign0 : InstrItinClass;
def II_VST_SRS_4x_dm_sts_srs_cm_idx_srsSign1 : InstrItinClass;
def II_VST_SRS_4x_dm_sts_srs_cm_idx_imm_srsSign0 : InstrItinClass;
def II_VST_SRS_4x_dm_sts_srs_cm_idx_imm_srsSign1 : InstrItinClass;
def II_VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_srsSign0 : InstrItinClass;
def II_VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_srsSign1 : InstrItinClass;
def II_VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_imm_srsSign0 : InstrItinClass;
def II_VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_imm_srsSign1 : InstrItinClass;
def II_VST_SRS_4x_dmx_sts_srs_dm_idx_srsSign0 : InstrItinClass;
def II_VST_SRS_4x_dmx_sts_srs_dm_idx_srsSign1 : InstrItinClass;
def II_VST_SRS_4x_dmx_sts_srs_dm_idx_imm_srsSign0 : InstrItinClass;
def II_VST_SRS_4x_dmx_sts_srs_dm_idx_imm_srsSign1 : InstrItinClass;
def II_VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_srsSign0 : InstrItinClass;
def II_VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_srsSign1 : InstrItinClass;
def II_VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_imm_srsSign0 : InstrItinClass;
def II_VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_imm_srsSign1 : InstrItinClass;
def II_VST_dmw_sts_feg2_spill : InstrItinClass;
def II_VST_dmw_sts_feg2_st_idx : InstrItinClass;
def II_VST_dmw_sts_feg2_st_idx_imm : InstrItinClass;
def II_VST_dmw_sts_feg2_st_pstm_nrm : InstrItinClass;
def II_VST_dmw_sts_feg2_st_pstm_nrm_imm : InstrItinClass;
def II_VST_dmw_sts_w_spill : InstrItinClass;
def II_VST_dmw_sts_w_st_idx : InstrItinClass;
def II_VST_dmw_sts_w_st_idx_imm : InstrItinClass;
def II_VST_dmw_sts_w_st_pstm_nrm : InstrItinClass;
def II_VST_dmw_sts_w_st_pstm_nrm_imm : InstrItinClass;
def II_VST_dmx_sts_bm_spill : InstrItinClass;
def II_VST_dmx_sts_bm_st_idx : InstrItinClass;
def II_VST_dmx_sts_bm_st_idx_imm : InstrItinClass;
def II_VST_dmx_sts_bm_st_pstm_nrm : InstrItinClass;
def II_VST_dmx_sts_bm_st_pstm_nrm_imm : InstrItinClass;
def II_VST_dmx_sts_x_spill : InstrItinClass;
def II_VST_dmx_sts_x_st_idx : InstrItinClass;
def II_VST_dmx_sts_x_st_idx_imm : InstrItinClass;
def II_VST_dmx_sts_x_st_pstm_nrm : InstrItinClass;
def II_VST_dmx_sts_x_st_pstm_nrm_imm : InstrItinClass;
def II_VSUB_16 : InstrItinClass;
def II_VSUB_32 : InstrItinClass;
def II_VSUB_8 : InstrItinClass;
def II_VSUB_GE_16_vaddSign0 : InstrItinClass;
def II_VSUB_GE_16_vaddSign1 : InstrItinClass;
def II_VSUB_GE_32_vaddSign0 : InstrItinClass;
def II_VSUB_GE_32_vaddSign1 : InstrItinClass;
def II_VSUB_GE_8_vaddSign0 : InstrItinClass;
def II_VSUB_GE_8_vaddSign1 : InstrItinClass;
def II_VSUB_LT_16_vaddSign0 : InstrItinClass;
def II_VSUB_LT_16_vaddSign1 : InstrItinClass;
def II_VSUB_LT_32_vaddSign0 : InstrItinClass;
def II_VSUB_LT_32_vaddSign1 : InstrItinClass;
def II_VSUB_LT_8_vaddSign0 : InstrItinClass;
def II_VSUB_LT_8_vaddSign1 : InstrItinClass;
def II_VSUB_vacc : InstrItinClass;
def II_VSUB_vacc_half : InstrItinClass;
def II_VSUB_vacc_scd_add_scd_incr : InstrItinClass;
def II_VSUB_vacc_scd_add_scd_noincr : InstrItinClass;
def II_VSUB_vacc_scd_half_add_scd_incr_half : InstrItinClass;
def II_VSUB_vacc_scd_half_add_scd_noincr_half : InstrItinClass;
def II_VSUB_f_vacc_fp : InstrItinClass;
def II_VSUB_f_vacc_fp_scd_add_scd_incr : InstrItinClass;
def II_VSUB_f_vacc_fp_scd_add_scd_noincr : InstrItinClass;
def II_VSUB_f_vacc_half_fp : InstrItinClass;
def II_VSUB_f_vacc_half_fp_scd_add_scd_incr_half : InstrItinClass;
def II_VSUB_f_vacc_half_fp_scd_add_scd_noincr_half : InstrItinClass;
def II_VSUB_mx6_vacc_fp_bfp13 : InstrItinClass;
def II_VSUB_mx6_vacc_fp_bfp13_scd_add_scd_incr : InstrItinClass;
def II_VSUB_mx6_vacc_fp_bfp13_scd_add_scd_noincr : InstrItinClass;
def II_VSUB_mx6_vacc_half_fp_bfp13 : InstrItinClass;
def II_VSUB_mx6_vacc_half_fp_bfp13_scd_add_scd_incr_half : InstrItinClass;
def II_VSUB_mx6_vacc_half_fp_bfp13_scd_add_scd_noincr_half : InstrItinClass;
def II_VTANH : InstrItinClass;
def II_VUNPACK_mv_unpack_w_unpackSign0 : InstrItinClass;
def II_VUNPACK_mv_unpack_w_unpackSign1 : InstrItinClass;
def II_VUNPACK_mv_unpack_x_unpackSign0 : InstrItinClass;
def II_VUNPACK_mv_unpack_x_unpackSign1 : InstrItinClass;
def II_VUPS_2x_mv_ups_w2b_upsSign0 : InstrItinClass;
def II_VUPS_2x_mv_ups_w2b_upsSign1 : InstrItinClass;
def II_VUPS_2x_mv_ups_x2c_upsSign0 : InstrItinClass;
def II_VUPS_2x_mv_ups_x2c_upsSign1 : InstrItinClass;
def II_VUPS_4x_mv_ups_w2c_upsSign0 : InstrItinClass;
def II_VUPS_4x_mv_ups_w2c_upsSign1 : InstrItinClass;
def II_VUPS_4x_mv_ups_x2d_upsSign0 : InstrItinClass;
def II_VUPS_4x_mv_ups_x2d_upsSign1 : InstrItinClass;
def II_XOR : InstrItinClass;

// HAND-CRAFTED BYPASSES AND ITINERARIES
include "ExtraItinDecls.td"
// END HAND-CRAFTED BYPASSES AND ITINERARIES

def AIE2PSItineraries : ProcessorItineraries<
[
// Resources
EMPTY_FU,
PART_WORD_STORE,
ACC2_SHH,
ACC2_SLH,
ACC_B_IN,
ACC_B_INH,
ACC_B_INL,
ACC_R,
ACC_RH,
ACC_RL,
ACC_WL,
AGU_RM_PORT,
BFACC1SHFTH__A,
BFACC1SHFTL__A,
BFPEXT__A,
BFSHCH__ACC1,
BFSHCH__SUBTRACT_ACC,
BF_CMAX_S,
BM_WM_1,
CNVOSTAT1,
COL,
CRBF8CONF_RV,
CRF2BMASK_WM_PORT,
CRF2FMASK_WM_PORT,
CRF2IMASK_WM_PORT,
CRFPCNVFL2FXMASK_WM_PORT,
CRFPCNVFX2FLMASK_WM_PORT,
CRFPMASK_RV,
CRFPMASK_WM_PORT,
CRFPNLFMASK_WM_PORT,
CRMCDEN_RS,
CRSAT_RA,
CRSHMODEH__DUMMY,
CRSHMODEL__DUMMY,
CRUPSMODE_RA,
DC_RM_PORT,
DC_WM_PORT,
DJ_RM_PORT,
DJ_WM_PORT,
DMS_WM,
DMX_STS_FIFO,
DMX_WRS,
DM_ADA,
DM_ADS,
DM_RM_L0_PORT,
DM_RV_L0,
DM_RV_L0_PORT,
DM_WA_L0_PORT,
DM_WM_L0_PORT,
DM_WV_L0_PORT,
DN_RM_PORT,
DN_WM_PORT,
DONE_FU,
EVENT_UPS_OF,
F2I__MASK,
FEY_RS_L0_PORT,
FEY_RW_L0_PORT,
FEY_WA_L0_PORT,
FEY_WB_L0_PORT,
FIFO_STS__ADVANCE_FIFO_R,
FIFO_STS__I_XH_S,
FP32FLAG__AH,
FP32NORML__FLAGS,
FP32_VOFLAGS_PH_SCL,
LCKREQ,
LD_FIFO_RA_PORT,
LD_FIFO_WA_PORT,
LE_WM,
LS_WM,
L_RS_L0_PORT,
L_WA_L0_PORT,
L_WM_L0_PORT,
M_RM_PORT,
M_WM_PORT,
PSA_ACCH_IN,
PSA_ACCH_IN_S,
PSA_ACCL_IN,
PSA_ACCL_IN_S,
PSA_MMODEH,
PSA_RSRC,
PSA_SUMH__A,
P_RM_PORT,
P_WM_PORT,
RSRC_EXECUTION_TRACE,
SCD,
SCD_CTL,
SCD_EXPAND_IN,
SCD_EXPAND_INHH,
SCD_INCR__O_POS,
TM_AD,
UPS_F__A,
UPS_F__A_L,
UPS__H__A,
UPS__H__OMUXL,
UPS__K,
UPS__L__K,
UPS__L__KL,
UPS__L__OMUXL,
UPS__OMUX,
VCTRL_IN,
VCTRL_SUB0_CG1U,
X_EGWL,
X_EXP_ELL,
Y_EGY,
Y_EXP_E,
ZACC_DML,
],[
// Bypasses
MV_Bypass,
VEC_Bypass,
MV_L_Bypass,
],[
// Itineraries

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__abs
InstrItinData<II_ABS, [], [/*d0*/1, /*s0*/1, /*srCarry*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__acquire_cond__mLockId__mLockId_imm__mR23_lock
InstrItinData<II_ACQ_COND_mLockId_imm, [InstrStage<4, [LCKREQ]>], [/*id*/1, /*s1*/1, /*r23*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__acquire_cond__mLockId__mLockId_reg__mR23_lock
InstrItinData<II_ACQ_COND_mLockId_reg, [InstrStage<4, [LCKREQ]>], [/*s0*/1, /*s1*/1, /*r23*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__acquire__mLockId__mLockId_imm
InstrItinData<II_ACQ_mLockId_imm, [InstrStage<4, [LCKREQ]>], [/*id*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__acquire__mLockId__mLockId_reg
InstrItinData<II_ACQ_mLockId_reg, [InstrStage<4, [LCKREQ]>], [/*s0*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__addx
InstrItinData<II_ADC, [], [/*d0*/1, /*s0*/1, /*s1*/1, /*srCarry*/1, /*srCarry*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_add_ri
InstrItinData<II_ADDM_NC_mv_add_ri, [PrefixCycle<DC_WM_PORT>, PrefixCycle<DJ_WM_PORT>, PrefixCycle<DN_WM_PORT>, PrefixCycle<L_WM_L0_PORT>, PrefixCycle<M_WM_PORT>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eDJ_eRe, [PrefixCycle<DJ_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_mLRm_eRe, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eDC_eRe, [PrefixCycle<DC_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eRe_eRe, [PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eVaddSign_eRe, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eP_eRe, [PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eUnpackSign_eRe, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eSRSSign_eRe, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eUPSSign_eRe, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eS_eRe, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_mLCm_eRe, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_ePackSign_eRe, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eDN_eRe, [PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_mSPm_eRe, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eM_eRe, [PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eRo_eRe, [PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eDJ_eRo, [PrefixCycle<DJ_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_mLRm_eRo, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eDC_eRo, [PrefixCycle<DC_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eRe_eRo, [PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eVaddSign_eRo, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eP_eRo, [PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eUnpackSign_eRo, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eSRSSign_eRo, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eUPSSign_eRo, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eS_eRo, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_mLCm_eRo, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_ePackSign_eRo, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eDN_eRo, [PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_mSPm_eRo, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eM_eRo, [PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,
InstrItinData<II_ADDM_NC_mv_add_ri_eRo_eRo, [PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*imm*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_add_rr
InstrItinData<II_ADDM_NC_mv_add_rr, [PrefixCycle<DC_WM_PORT>, PrefixCycle<DJ_WM_PORT>, PrefixCycle<DN_WM_PORT>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<L_WM_L0_PORT>, PrefixCycle<M_WM_PORT>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eDJ_eRe_eRe, [PrefixCycle<DJ_WM_PORT>, PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_mLRm_eRe_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eDC_eRe_eRe, [PrefixCycle<DC_WM_PORT>, PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eRe_eRe_eRe, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eVaddSign_eRe_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eP_eRe_eRe, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eUnpackSign_eRe_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eSRSSign_eRe_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eUPSSign_eRe_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eS_eRe_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_mLCm_eRe_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_ePackSign_eRe_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eDN_eRe_eRe, [PrefixCycle<DN_WM_PORT>, PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_mSPm_eRe_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eM_eRe_eRe, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eRo_eRe_eRe, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eDJ_eRo_eRe, [PrefixCycle<DJ_WM_PORT>, PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_mLRm_eRo_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eDC_eRo_eRe, [PrefixCycle<DC_WM_PORT>, PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eRe_eRo_eRe, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eVaddSign_eRo_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eP_eRo_eRe, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eUnpackSign_eRo_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eSRSSign_eRo_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eUPSSign_eRo_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eS_eRo_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_mLCm_eRo_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_ePackSign_eRo_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eDN_eRo_eRe, [PrefixCycle<DN_WM_PORT>, PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_mSPm_eRo_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eM_eRo_eRe, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eRo_eRo_eRe, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eDJ_eRe_eRo, [PrefixCycle<DJ_WM_PORT>, PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_mLRm_eRe_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eDC_eRe_eRo, [PrefixCycle<DC_WM_PORT>, PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eRe_eRe_eRo, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eVaddSign_eRe_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eP_eRe_eRo, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eUnpackSign_eRe_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eSRSSign_eRe_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eUPSSign_eRe_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eS_eRe_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_mLCm_eRe_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_ePackSign_eRe_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eDN_eRe_eRo, [PrefixCycle<DN_WM_PORT>, PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_mSPm_eRe_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eM_eRe_eRo, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eRo_eRe_eRo, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eDJ_eRo_eRo, [PrefixCycle<DJ_WM_PORT>, PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_mLRm_eRo_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eDC_eRo_eRo, [PrefixCycle<DC_WM_PORT>, PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eRe_eRo_eRo, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eVaddSign_eRo_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eP_eRo_eRo, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eUnpackSign_eRo_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eSRSSign_eRo_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eUPSSign_eRo_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eS_eRo_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_mLCm_eRo_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_ePackSign_eRo_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eDN_eRo_eRo, [PrefixCycle<DN_WM_PORT>, PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_mSPm_eRo_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eM_eRo_eRo, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,
InstrItinData<II_ADDM_NC_mv_add_rr_eRo_eRo_eRo, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_cg_or__mv_cg_le_rel__zlpm__rel2abs
InstrItinData<II_ADDM_NC_mv_cg_or_mv_cg_le_rel, [SimpleCycle<LE_WM>], [/*i*/1, /*le*/1, /*pc*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_cg_or__mv_cg_ls_rel__zlpm__rel2abs
InstrItinData<II_ADDM_NC_mv_cg_or_mv_cg_ls_rel, [SimpleCycle<LS_WM>], [/*i*/1, /*ls*/1, /*pc*/1]>,

// id: me__instr128_or__alu_mv__lng__lng_cg_le_rel__zlpm__rel2abs
InstrItinData<II_ADDXM_NC_lng_cg_le_rel, [SimpleCycle<LE_WM>], [/*i*/1, /*le*/1, /*pc*/1]>,

// id: me__instr128_or__alu_mv__lng__lng_cg_ls_rel__zlpm__rel2abs
InstrItinData<II_ADDXM_NC_lng_cg_ls_rel, [SimpleCycle<LS_WM>], [/*i*/1, /*ls*/1, /*pc*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__add_lc_ri__mLCxCg
InstrItinData<II_ADD_NC_add_lc_ri, [], [/*lc*/1, /*s0*/1, /*imm*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_cg_or__alu_cg_le_rel__zlpx__rel2abs
InstrItinData<II_ADD_NC_alu_cg_or_alu_cg_le_rel, [SimpleCycle<LE_WM>], [/*i*/1, /*le*/1, /*pc*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_cg_or__alu_cg_ls_rel__zlpx__rel2abs
InstrItinData<II_ADD_NC_alu_cg_or_alu_cg_ls_rel, [SimpleCycle<LS_WM>], [/*i*/1, /*ls*/1, /*pc*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__add_r_ri
InstrItinData<II_ADD_add_r_ri, [], [/*d0*/1, /*s0*/1, /*imm*/1, /*srCarry*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__add
InstrItinData<II_ADD_alu_r_rr, [], [/*d0*/1, /*s0*/1, /*s1*/1, /*srCarry*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__and
InstrItinData<II_AND, [], [/*d0*/1, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__sgn
InstrItinData<II_ASHL, [], [/*d0*/1, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__clb
InstrItinData<II_CLB, [], [/*d0*/1, /*s0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__clbu
InstrItinData<II_CLZ, [], [/*d0*/1, /*s0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_dstep__mR31_divs
InstrItinData<II_DIVS, [], [/*d0*/1, /*r31_out*/1, /*r31*/1, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__done
InstrItinData<II_DONE, [EmptyCycles<3>, InstrStage<3, [DONE_FU]>], []>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__seq0
InstrItinData<II_EQZ, [], [/*d0*/1, /*s0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__seq
InstrItinData<II_EQ, [], [/*d0*/1, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__event__event3
InstrItinData<II_EVENT_ERROR, [], []>,

// id: me__instr128_or__alu_mv__alumv_or__alu__event__event2
InstrItinData<II_EVENT_WARNING, [], []>,

// id: me__instr128_or__alu_mv__alumv_or__alu__event__event0
InstrItinData<II_EVENT_event0, [], []>,

// id: me__instr128_or__alu_mv__alumv_or__alu__event__event1
InstrItinData<II_EVENT_event1, [], []>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__se16
InstrItinData<II_EXTEND_s16, [], [/*d0*/1, /*s0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__se8
InstrItinData<II_EXTEND_s8, [], [/*d0*/1, /*s0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__ze16
InstrItinData<II_EXTEND_u16, [], [/*d0*/1, /*s0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__ze8
InstrItinData<II_EXTEND_u8, [], [/*d0*/1, /*s0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__sgeu
InstrItinData<II_GEU, [], [/*d0*/1, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__sge
InstrItinData<II_GE, [], [/*d0*/1, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_invsqrt
InstrItinData<II_INVSQRT_nlf_invsqrt, [EmptyCycles<3>, SimpleCycle<L_WA_L0_PORT>], [/*d0*/4, /*s0*/1, /*srFPNlf*/4, /*crFPNlfMask*/4]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_invsqrt__mOptConvDel__mNlfFlt2fx__mFlt2fx_delayed__mS3
InstrItinData<II_INVSQRT_nlf_invsqrt_mOptConvDel_mS3, [EmptyCycles<3>, PrefixCycle<CNVOSTAT1>, SimpleCycle<L_WA_L0_PORT>], [/*d0*/4, /*mcnv_del*/4, /*s0*/1, /*srFPCnvFl2Fx*/4, /*srFPNlf*/4, /*crFPCnvFl2FxMask*/4, /*crFPNlfMask*/4]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_invsqrt__mOptConvDel__mNlfFlt2fx__mFlt2fx_delayed__mS3__mOptConv__mNlfFx2flt__mFx2flt__mS2
InstrItinData<II_INVSQRT_nlf_invsqrt_mOptConvDel_mS3_mOptConv, [EmptyCycles<3>, PrefixCycle<CNVOSTAT1>, SimpleCycle<L_WA_L0_PORT>], [/*d0*/4, /*mcnv_del*/4, /*s0*/1, /*mcnv*/1, /*srFPCnvFl2Fx*/4, /*srFPCnvFx2Fl*/1, /*srFPNlf*/4, /*crFPCnvFl2FxMask*/4, /*crFPCnvFx2FlMask*/1, /*crFPNlfMask*/4]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_invsqrt__mOptConv__mNlfFx2flt__mFx2flt__mS2
InstrItinData<II_INVSQRT_nlf_invsqrt_mOptConv, [EmptyCycles<3>, SimpleCycle<L_WA_L0_PORT>], [/*d0*/4, /*s0*/1, /*mcnv*/1, /*srFPCnvFx2Fl*/1, /*srFPNlf*/4, /*crFPCnvFx2FlMask*/1, /*crFPNlfMask*/4]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_inv
InstrItinData<II_INV_nlf_inv, [EmptyCycles<3>, SimpleCycle<L_WA_L0_PORT>], [/*d0*/4, /*s0*/1, /*srFPNlf*/4, /*crFPNlfMask*/4]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_inv__mOptConvDel__mNlfFlt2fx__mFlt2fx_delayed__mS3
InstrItinData<II_INV_nlf_inv_mOptConvDel_mS3, [EmptyCycles<3>, PrefixCycle<CNVOSTAT1>, SimpleCycle<L_WA_L0_PORT>], [/*d0*/4, /*mcnv_del*/4, /*s0*/1, /*srFPCnvFl2Fx*/4, /*srFPNlf*/4, /*crFPCnvFl2FxMask*/4, /*crFPNlfMask*/4]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_inv__mOptConvDel__mNlfFlt2fx__mFlt2fx_delayed__mS3__mOptConv__mNlfFx2flt__mFx2flt__mS2
InstrItinData<II_INV_nlf_inv_mOptConvDel_mS3_mOptConv, [EmptyCycles<3>, PrefixCycle<CNVOSTAT1>, SimpleCycle<L_WA_L0_PORT>], [/*d0*/4, /*mcnv_del*/4, /*s0*/1, /*mcnv*/1, /*srFPCnvFl2Fx*/4, /*srFPCnvFx2Fl*/1, /*srFPNlf*/4, /*crFPCnvFl2FxMask*/4, /*crFPCnvFx2FlMask*/1, /*crFPNlfMask*/4]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_inv__mOptConv__mNlfFx2flt__mFx2flt__mS2
InstrItinData<II_INV_nlf_inv_mOptConv, [EmptyCycles<3>, SimpleCycle<L_WA_L0_PORT>], [/*d0*/4, /*s0*/1, /*mcnv*/1, /*srFPCnvFx2Fl*/1, /*srFPNlf*/4, /*crFPCnvFx2FlMask*/1, /*crFPNlfMask*/4]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__jump_link_ind__mPm__mLRx
InstrItinData<II_JL_alumv_or, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<P_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*i*/1, /*lr*/4]>,

// id: me__instr128_or__alu_mv__lng__jump_link_imm__mLRx
InstrItinData<II_JL_lng, [], [/*i*/1, /*lr*/4]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__jumpc_decr_ind__mPm
InstrItinData<II_JNZD, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<P_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*d0*/1, /*s0*/1, /*i*/1, /*srCarry*/1]>,

// id: me__instr128_or__alu_mv__lng__jumpc_imm__bnez
InstrItinData<II_JNZ, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*s0*/1, /*i*/1]>,

// id: me__instr128_or__alu_mv__lng__jumpc_imm__beqz
InstrItinData<II_JZ, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*s0*/1, /*i*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__jump_ind__mPm
InstrItinData<II_J_alumv_or, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<P_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*i*/1]>,

// id: me__instr128_or__alu_mv__lng__jump_imm
InstrItinData<II_J_lng, [], [/*i*/1]>,

// id: me__instr128_or__lda__dml_lda2_scalar_EE__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_2d__mPa__mDa
MemInstrItinData<II_LDA_2D_dml_lda2_scalar_EE, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_EG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_2d__mPa__mDa
MemInstrItinData<II_LDA_2D_dml_lda2_scalar_EG, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_GG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_2d__mPa__mDa
MemInstrItinData<II_LDA_2D_dml_lda2_scalar_GG, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda_scalar_F__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_2d__mPa__mDa
MemInstrItinData<II_LDA_2D_dml_lda_scalar_F, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda_scalar_L__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_2d__mPa__mDa__mLa
MemInstrItinData<II_LDA_2D_dml_lda_scalar_L, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dms_lda_g__ld__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_2d__mPa__mDa
MemInstrItinData<II_LDA_2D_dms_lda_g, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dms_lda_scalar__ld__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_2d__mPa__mDa
MemInstrItinData<II_LDA_2D_dms_lda_scalar, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DC_WM_PORT>, PrefixCycle<DJ_WM_PORT>, PrefixCycle<DN_WM_PORT>, PrefixCycle<FEY_WA_L0_PORT>, PrefixCycle<L_WA_L0_PORT>, PrefixCycle<M_WM_PORT>, SimpleCycle<P_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_2D_dms_lda_scalar_mLRa, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_2D_dms_lda_scalar_eEle, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_2D_dms_lda_scalar_eElo, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_2D_dms_lda_scalar_eDJ, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DJ_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_2D_dms_lda_scalar_eDC, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DC_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_2D_dms_lda_scalar_eRe, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_2D_dms_lda_scalar_eDN, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DN_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_2D_dms_lda_scalar_eEhe, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_2D_dms_lda_scalar_eP, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<P_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_2D_dms_lda_scalar_eM, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<M_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_2D_dms_lda_scalar_eRo, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_2D_dms_lda_scalar_eEho, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_eg2__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_2d__mPa__mDa
MemInstrItinData<II_LDA_2D_dmv_lda_eg2, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_f__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_2d__mPa__mDa
MemInstrItinData<II_LDA_2D_dmv_lda_f, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_feg__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_2d__mPa__mDa
MemInstrItinData<II_LDA_2D_dmv_lda_feg, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_2d__mPa__mDa__lh
MemInstrItinData<II_LDA_2D_s16, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_2d__mPa__mDa__lb
MemInstrItinData<II_LDA_2D_s8, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_2d__mPa__mDa__lhu
MemInstrItinData<II_LDA_2D_u16, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_2d__mPa__mDa__lbu
MemInstrItinData<II_LDA_2D_u8, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_EE__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_3d__mPa__mDSa
MemInstrItinData<II_LDA_3D_dml_lda2_scalar_EE, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_EG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_3d__mPa__mDSa
MemInstrItinData<II_LDA_3D_dml_lda2_scalar_EG, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_GG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_3d__mPa__mDSa
MemInstrItinData<II_LDA_3D_dml_lda2_scalar_GG, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda_scalar_F__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_3d__mPa__mDSa
MemInstrItinData<II_LDA_3D_dml_lda_scalar_F, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda_scalar_L__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_3d__mPa__mDSa__mLa
MemInstrItinData<II_LDA_3D_dml_lda_scalar_L, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dms_lda_g__ld__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_3d__mPa__mDSa
MemInstrItinData<II_LDA_3D_dms_lda_g, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dms_lda_scalar__ld__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_3d__mPa__mDSa
MemInstrItinData<II_LDA_3D_dms_lda_scalar, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DC_WM_PORT>, PrefixCycle<DJ_WM_PORT>, PrefixCycle<DN_WM_PORT>, PrefixCycle<FEY_WA_L0_PORT>, PrefixCycle<L_WA_L0_PORT>, PrefixCycle<M_WM_PORT>, SimpleCycle<P_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_3D_dms_lda_scalar_mLRa, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_3D_dms_lda_scalar_eEle, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_3D_dms_lda_scalar_eElo, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_3D_dms_lda_scalar_eDJ, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DJ_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_3D_dms_lda_scalar_eDC, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DC_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_3D_dms_lda_scalar_eRe, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_3D_dms_lda_scalar_eDN, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DN_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_3D_dms_lda_scalar_eEhe, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_3D_dms_lda_scalar_eP, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<P_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_3D_dms_lda_scalar_eM, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<M_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_3D_dms_lda_scalar_eRo, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_3D_dms_lda_scalar_eEho, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_eg2__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_3d__mPa__mDSa
MemInstrItinData<II_LDA_3D_dmv_lda_eg2, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_f__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_3d__mPa__mDSa
MemInstrItinData<II_LDA_3D_dmv_lda_f, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_feg__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_3d__mPa__mDSa
MemInstrItinData<II_LDA_3D_dmv_lda_feg, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_3d__mPa__mDSa__lh
MemInstrItinData<II_LDA_3D_s16, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_3d__mPa__mDSa__lb
MemInstrItinData<II_LDA_3D_s8, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_3d__mPa__mDSa__lhu
MemInstrItinData<II_LDA_3D_u16, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_3d__mPa__mDSa__lbu
MemInstrItinData<II_LDA_3D_u8, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__tm_lda__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_2d__mPa__mDa
MemInstrItinData<II_LDA_TM_2D, [EmptyCycles<1>, PrefixCycle<DM_ADA>, SimpleCycle<TM_AD>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[4]>>,

// id: me__instr128_or__lda__tm_lda__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_3d__mPa__mDSa
MemInstrItinData<II_LDA_TM_3D, [EmptyCycles<1>, PrefixCycle<DM_ADA>, SimpleCycle<TM_AD>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[4]>>,

// id: me__instr128_or__lda__tm_lda__agua_dms__nrm__agua_dms__normal__agua_dms__idx__mPa__mDJa
MemInstrItinData<II_LDA_TM_idx, [EmptyCycles<1>, PrefixCycle<DM_ADA>, SimpleCycle<TM_AD>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[4]>>,

// id: me__instr128_or__lda__tm_lda__agua_dms__nrm__agua_dms__normal__agua_dms__idx_imm__mPa__m_c06s_step4
MemInstrItinData<II_LDA_TM_idx_imm, [EmptyCycles<1>, PrefixCycle<DM_ADA>, SimpleCycle<TM_AD>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[4]>>,

// id: me__instr128_or__lda__tm_lda__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_nrm__mPa__mMa
MemInstrItinData<II_LDA_TM_pstm_nrm, [EmptyCycles<1>, PrefixCycle<DM_ADA>, SimpleCycle<TM_AD>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[4]>>,

// id: me__instr128_or__lda__tm_lda__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_nrm_imm__mPa__m_c06s_step4
MemInstrItinData<II_LDA_TM_pstm_nrm_imm, [EmptyCycles<1>, PrefixCycle<DM_ADA>, SimpleCycle<TM_AD>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[4]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_EE__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx__mPa__mDJa
MemInstrItinData<II_LDA_dml_lda2_scalar_EE_ld_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_EE__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx_imm__mPa__m_c07s_step8
MemInstrItinData<II_LDA_dml_lda2_scalar_EE_ld_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_EE__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm__mPa__mMa
MemInstrItinData<II_LDA_dml_lda2_scalar_EE_ld_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_EE__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm_imm__mPa__m_c07s_step8
MemInstrItinData<II_LDA_dml_lda2_scalar_EE_ld_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_EE__spill__agua_dml__nrm_spill__agua_dml__spill__mSPa__m_c13n_step8
MemInstrItinData<II_LDA_dml_lda2_scalar_EE_spill, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_EG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx__mPa__mDJa
MemInstrItinData<II_LDA_dml_lda2_scalar_EG_ld_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_EG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx_imm__mPa__m_c07s_step8
MemInstrItinData<II_LDA_dml_lda2_scalar_EG_ld_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_EG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm__mPa__mMa
MemInstrItinData<II_LDA_dml_lda2_scalar_EG_ld_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_EG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm_imm__mPa__m_c07s_step8
MemInstrItinData<II_LDA_dml_lda2_scalar_EG_ld_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_EG__spill__agua_dml__nrm_spill__agua_dml__spill__mSPa__m_c13n_step8
MemInstrItinData<II_LDA_dml_lda2_scalar_EG_spill, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_GG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx__mPa__mDJa
MemInstrItinData<II_LDA_dml_lda2_scalar_GG_ld_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_GG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx_imm__mPa__m_c07s_step8
MemInstrItinData<II_LDA_dml_lda2_scalar_GG_ld_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_GG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm__mPa__mMa
MemInstrItinData<II_LDA_dml_lda2_scalar_GG_ld_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_GG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm_imm__mPa__m_c07s_step8
MemInstrItinData<II_LDA_dml_lda2_scalar_GG_ld_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda2_scalar_GG__spill__agua_dml__nrm_spill__agua_dml__spill__mSPa__m_c13n_step8
MemInstrItinData<II_LDA_dml_lda2_scalar_GG_spill, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda_scalar_F__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx__mPa__mDJa
MemInstrItinData<II_LDA_dml_lda_scalar_F_ld_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda_scalar_F__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx_imm__mPa__m_c07s_step8
MemInstrItinData<II_LDA_dml_lda_scalar_F_ld_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda_scalar_F__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm__mPa__mMa
MemInstrItinData<II_LDA_dml_lda_scalar_F_ld_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda_scalar_F__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm_imm__mPa__m_c07s_step8
MemInstrItinData<II_LDA_dml_lda_scalar_F_ld_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda_scalar_F__spill__agua_dml__nrm_spill__agua_dml__spill__mSPa__m_c13n_step8
MemInstrItinData<II_LDA_dml_lda_scalar_F_spill, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda_scalar_L__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx__mPa__mDJa__mLa
MemInstrItinData<II_LDA_dml_lda_scalar_L_ld_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda_scalar_L__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx_imm__mPa__m_c07s_step8__mLa
MemInstrItinData<II_LDA_dml_lda_scalar_L_ld_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda_scalar_L__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm__mPa__mMa__mLa
MemInstrItinData<II_LDA_dml_lda_scalar_L_ld_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda_scalar_L__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm_imm__mPa__m_c07s_step8__mLa
MemInstrItinData<II_LDA_dml_lda_scalar_L_ld_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dml_lda_scalar_L__spill__agua_dml__nrm_spill__agua_dml__spill__mSPa__m_c13n_step8__mLa
MemInstrItinData<II_LDA_dml_lda_scalar_L_spill, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dms_lda_g__ld__agua_dms__nrm__agua_dms__normal__agua_dms__idx__mPa__mDJa
MemInstrItinData<II_LDA_dms_lda_g_ld_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dms_lda_g__ld__agua_dms__nrm__agua_dms__normal__agua_dms__idx_imm__mPa__m_c06s_step4
MemInstrItinData<II_LDA_dms_lda_g_ld_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dms_lda_g__ld__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_nrm__mPa__mMa
MemInstrItinData<II_LDA_dms_lda_g_ld_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dms_lda_g__ld__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_nrm_imm__mPa__m_c06s_step4
MemInstrItinData<II_LDA_dms_lda_g_ld_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dms_lda_g__spill__agua_dms__nrm_spill__agua_dms__spill__mSPa__m_c12n_step4
MemInstrItinData<II_LDA_dms_lda_g_spill, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dms_lda_scalar__ld__agua_dms__nrm__agua_dms__normal__agua_dms__idx__mPa__mDJa
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DC_WM_PORT>, PrefixCycle<DJ_WM_PORT>, PrefixCycle<DN_WM_PORT>, PrefixCycle<FEY_WA_L0_PORT>, PrefixCycle<L_WA_L0_PORT>, PrefixCycle<M_WM_PORT>, SimpleCycle<P_WM_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_mLRa, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_eEle, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_eElo, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_eDJ, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DJ_WM_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_eDC, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DC_WM_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_eRe, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_eDN, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DN_WM_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_eEhe, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_eP, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<P_WM_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_eM, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<M_WM_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_eRo, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_eEho, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dms_lda_scalar__ld__agua_dms__nrm__agua_dms__normal__agua_dms__idx_imm__mPa__m_c06s_step4
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DC_WM_PORT>, PrefixCycle<DJ_WM_PORT>, PrefixCycle<DN_WM_PORT>, PrefixCycle<FEY_WA_L0_PORT>, PrefixCycle<L_WA_L0_PORT>, PrefixCycle<M_WM_PORT>, SimpleCycle<P_WM_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_imm_mLRa, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_imm_eEle, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_imm_eElo, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_imm_eDJ, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DJ_WM_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_imm_eDC, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DC_WM_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_imm_eRe, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_imm_eDN, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DN_WM_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_imm_eEhe, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_imm_eP, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<P_WM_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_imm_eM, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<M_WM_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_imm_eRo, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_idx_imm_eEho, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dms_lda_scalar__ld__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_nrm__mPa__mMa
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DC_WM_PORT>, PrefixCycle<DJ_WM_PORT>, PrefixCycle<DN_WM_PORT>, PrefixCycle<FEY_WA_L0_PORT>, PrefixCycle<L_WA_L0_PORT>, PrefixCycle<M_WM_PORT>, SimpleCycle<P_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_mLRa, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_eEle, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_eElo, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_eDJ, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DJ_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_eDC, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DC_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_eRe, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_eDN, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DN_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_eEhe, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_eP, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<P_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_eM, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<M_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_eRo, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_eEho, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dms_lda_scalar__ld__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_nrm_imm__mPa__m_c06s_step4
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DC_WM_PORT>, PrefixCycle<DJ_WM_PORT>, PrefixCycle<DN_WM_PORT>, PrefixCycle<FEY_WA_L0_PORT>, PrefixCycle<L_WA_L0_PORT>, PrefixCycle<M_WM_PORT>, SimpleCycle<P_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_mLRa, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eEle, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eElo, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eDJ, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DJ_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eDC, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DC_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eRe, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eDN, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DN_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eEhe, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eP, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<P_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eM, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<M_WM_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eRo, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eEho, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dms_lda_scalar__spill__agua_dms__nrm_spill__agua_dms__spill__mSPa__m_c12n_step4
MemInstrItinData<II_LDA_dms_lda_scalar_spill, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DC_WM_PORT>, PrefixCycle<DJ_WM_PORT>, PrefixCycle<DN_WM_PORT>, PrefixCycle<FEY_WA_L0_PORT>, PrefixCycle<L_WA_L0_PORT>, PrefixCycle<M_WM_PORT>, SimpleCycle<P_WM_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_spill_mLRa, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_spill_eEle, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_spill_eElo, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_spill_eDJ, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DJ_WM_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_spill_eDC, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DC_WM_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_spill_eRe, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_spill_eDN, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DN_WM_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_spill_eEhe, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_spill_eP, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<P_WM_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_spill_eM, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<M_WM_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_spill_eRo, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_dms_lda_scalar_spill_eEho, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_eg2__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx__mPa__mDJa
MemInstrItinData<II_LDA_dmv_lda_eg2_ld_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_eg2__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx_imm__mPa__m_c08s_step16
MemInstrItinData<II_LDA_dmv_lda_eg2_ld_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_eg2__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm__mPa__mMa
MemInstrItinData<II_LDA_dmv_lda_eg2_ld_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_eg2__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm_imm__mPa__m_c08s_step16
MemInstrItinData<II_LDA_dmv_lda_eg2_ld_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_eg2__spill__agua_dmv__nrm_spill__agua_dmv__spill__mSPa__m_c14n_step16
MemInstrItinData<II_LDA_dmv_lda_eg2_spill, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_f__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx__mPa__mDJa
MemInstrItinData<II_LDA_dmv_lda_f_ld_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_f__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx_imm__mPa__m_c08s_step16
MemInstrItinData<II_LDA_dmv_lda_f_ld_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_f__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm__mPa__mMa
MemInstrItinData<II_LDA_dmv_lda_f_ld_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_f__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm_imm__mPa__m_c08s_step16
MemInstrItinData<II_LDA_dmv_lda_f_ld_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_f__spill__agua_dmv__nrm_spill__agua_dmv__spill__mSPa__m_c14n_step16
MemInstrItinData<II_LDA_dmv_lda_f_spill, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_feg__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx__mPa__mDJa
MemInstrItinData<II_LDA_dmv_lda_feg_ld_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_feg__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx_imm__mPa__m_c08s_step16
MemInstrItinData<II_LDA_dmv_lda_feg_ld_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_feg__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm__mPa__mMa
MemInstrItinData<II_LDA_dmv_lda_feg_ld_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_feg__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm_imm__mPa__m_c08s_step16
MemInstrItinData<II_LDA_dmv_lda_feg_ld_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_feg__spill__agua_dmv__nrm_spill__agua_dmv__spill__mSPa__m_c14n_step16
MemInstrItinData<II_LDA_dmv_lda_feg_spill, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__idx__mPa__mDJa__lh
MemInstrItinData<II_LDA_s16_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__idx_imm__mPa__m_c05s_step2__lh
MemInstrItinData<II_LDA_s16_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_nrm__mPa__mMa__lh
MemInstrItinData<II_LDA_s16_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_nrm_imm__mPa__m_c05s_step2__lh
MemInstrItinData<II_LDA_s16_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__idx__mPa__mDJa__lb
MemInstrItinData<II_LDA_s8_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__idx_imm__mPa__m_c04s__lb
MemInstrItinData<II_LDA_s8_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_nrm__mPa__mMa__lb
MemInstrItinData<II_LDA_s8_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_nrm_imm__mPa__m_c04s__lb
MemInstrItinData<II_LDA_s8_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__idx__mPa__mDJa__lhu
MemInstrItinData<II_LDA_u16_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__idx_imm__mPa__m_c05s_step2__lhu
MemInstrItinData<II_LDA_u16_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_nrm__mPa__mMa__lhu
MemInstrItinData<II_LDA_u16_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_nrm_imm__mPa__m_c05s_step2__lhu
MemInstrItinData<II_LDA_u16_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__idx__mPa__mDJa__lbu
MemInstrItinData<II_LDA_u8_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__idx_imm__mPa__m_c04s__lbu
MemInstrItinData<II_LDA_u8_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_nrm__mPa__mMa__lbu
MemInstrItinData<II_LDA_u8_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_nrm_imm__mPa__m_c04s__lbu
MemInstrItinData<II_LDA_u8_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__uns
InstrItinData<II_LSHL, [], [/*d0*/1, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__sltu
InstrItinData<II_LTU, [], [/*d0*/1, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__slt
InstrItinData<II_LT, [], [/*d0*/1, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mac_r_rrr
InstrItinData<II_MAC, [EmptyCycles<1>, SimpleCycle<L_WA_L0_PORT>], [/*d0*/2, /*a0*/2, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr_ext__maxu
InstrItinData<II_MAXU, [], [/*d0*/1, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr_ext__max
InstrItinData<II_MAX, [], [/*d0*/1, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr_ext__minu
InstrItinData<II_MINU, [], [/*d0*/1, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr_ext__min
InstrItinData<II_MIN, [], [/*d0*/1, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__lda__lda_cg
InstrItinData<II_MOVA, [PrefixCycle<DC_WM_PORT>, PrefixCycle<DJ_WM_PORT>, PrefixCycle<DN_WM_PORT>, PrefixCycle<L_WA_L0_PORT>, PrefixCycle<M_WM_PORT>, SimpleCycle<P_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVA_eDJ, [SimpleCycle<DJ_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVA_mLCaCg, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVA_eDC, [SimpleCycle<DC_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVA_eRe, [SimpleCycle<L_WA_L0_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVA_eDN, [SimpleCycle<DN_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVA_eP, [SimpleCycle<P_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVA_eM, [SimpleCycle<M_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVA_eRo, [SimpleCycle<L_WA_L0_PORT>], [/*dst*/1, /*i*/1]>,

// id: me__instr128_or__st__mvs_agu__agus__mv
InstrItinData<II_MOVS, [PrefixCycle<DJ_WM_PORT>, PrefixCycle<DN_WM_PORT>, PrefixCycle<L_RS_L0_PORT>, SimpleCycle<M_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDJ_eDJ, [SimpleCycle<DJ_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDC_eDJ, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDN_eDJ, [SimpleCycle<DN_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eP_eDJ, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eM_eDJ, [SimpleCycle<M_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDJ_eDC, [SimpleCycle<DJ_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDJ_eDN, [SimpleCycle<DJ_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDJ_eRe, [PrefixCycle<DJ_WM_PORT>, SimpleCycle<L_RS_L0_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDJ_eP, [SimpleCycle<DJ_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDJ_eM, [SimpleCycle<DJ_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDJ_eRo, [PrefixCycle<DJ_WM_PORT>, SimpleCycle<L_RS_L0_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDC_eDC, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDC_eDN, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDC_eRe, [SimpleCycle<L_RS_L0_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDC_eP, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDC_eM, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDC_eRo, [SimpleCycle<L_RS_L0_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDN_eDC, [SimpleCycle<DN_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDN_eDN, [SimpleCycle<DN_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDN_eRe, [PrefixCycle<DN_WM_PORT>, SimpleCycle<L_RS_L0_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDN_eP, [SimpleCycle<DN_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDN_eM, [SimpleCycle<DN_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eDN_eRo, [PrefixCycle<DN_WM_PORT>, SimpleCycle<L_RS_L0_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eP_eDC, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eP_eDN, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eP_eRe, [SimpleCycle<L_RS_L0_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eP_eP, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eP_eM, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eP_eRo, [SimpleCycle<L_RS_L0_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eM_eDC, [SimpleCycle<M_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eM_eDN, [SimpleCycle<M_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eM_eRe, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<M_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eM_eP, [SimpleCycle<M_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eM_eM, [SimpleCycle<M_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVS_eM_eRo, [PrefixCycle<L_RS_L0_PORT>, SimpleCycle<M_WM_PORT>], [/*dst*/1, /*src*/1]>,

// id: me__instr128_or__alu_mv__lng__lng_cg
InstrItinData<II_MOVXM_lng_cg, [PrefixCycle<DC_WM_PORT>, PrefixCycle<DJ_WM_PORT>, PrefixCycle<DN_WM_PORT>, PrefixCycle<L_WM_L0_PORT>, PrefixCycle<M_WM_PORT>, SimpleCycle<P_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVXM_lng_cg_eDJ, [SimpleCycle<DJ_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVXM_lng_cg_mLRm, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVXM_lng_cg_eDC, [SimpleCycle<DC_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVXM_lng_cg_eRe, [SimpleCycle<L_WM_L0_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVXM_lng_cg_mLCmCg, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVXM_lng_cg_eVaddSign, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVXM_lng_cg_eP, [SimpleCycle<P_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVXM_lng_cg_eUnpackSign, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVXM_lng_cg_eSRSSign, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVXM_lng_cg_eUPSSign, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVXM_lng_cg_eS, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVXM_lng_cg_ePackSign, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVXM_lng_cg_eDN, [SimpleCycle<DN_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVXM_lng_cg_mSPm, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVXM_lng_cg_eM, [SimpleCycle<M_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOVXM_lng_cg_eRo, [SimpleCycle<L_WM_L0_PORT>], [/*dst*/1, /*i*/1]>,

// id: me__instr128_or__alu_mv__lng__lng_cg_le_abs
InstrItinData<II_MOVXM_lng_cg_le_abs, [SimpleCycle<LE_WM>], [/*i*/1, /*le*/1]>,

// id: me__instr128_or__alu_mv__lng__lng_cg_ls_abs
InstrItinData<II_MOVXM_lng_cg_ls_abs, [SimpleCycle<LS_WM>], [/*i*/1, /*ls*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_cg_or__alu_cg
InstrItinData<II_MOVX_alu_cg_or, [], [/*dst*/1, /*i*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mvx_cr_imm
InstrItinData<II_MOVX_mvx_cr_imm, [PrefixCycle<CRF2BMASK_WM_PORT>, PrefixCycle<CRF2FMASK_WM_PORT>, PrefixCycle<CRF2IMASK_WM_PORT>, PrefixCycle<CRFPCNVFL2FXMASK_WM_PORT>, PrefixCycle<CRFPCNVFX2FLMASK_WM_PORT>, PrefixCycle<CRFPMASK_WM_PORT>, SimpleCycle<CRFPNLFMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_mCRFPNlfMask, [SimpleCycle<CRFPNLFMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_mCRSCDEn, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_mCRPackSize, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_mCRRnd, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_mCRSat, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_mCRFPMask, [SimpleCycle<CRFPMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_mCRF2IMask, [SimpleCycle<CRF2IMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_mCRF2FMask, [SimpleCycle<CRF2FMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_eVaddSign, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_mCRF2BMask, [SimpleCycle<CRF2BMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_mCRFPCnvFl2FxMask, [SimpleCycle<CRFPCNVFL2FXMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_eSRSSign, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_eUnpackSign, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_mCRFPConvSat, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_mCRUPSMode, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_mCRBF8conf, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_eUPSSign, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_mCRUnpackSize, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_ePackSign, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_mCRMCDEn, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_mCRFPCnvFx2FlMask, [SimpleCycle<CRFPCNVFX2FLMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_mCRFP8conf, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_imm_mCRSRSMode, [], [/*dst*/1, /*src*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mvx_cr_r
InstrItinData<II_MOVX_mvx_cr_r, [PrefixCycle<CRF2BMASK_WM_PORT>, PrefixCycle<CRF2FMASK_WM_PORT>, PrefixCycle<CRF2IMASK_WM_PORT>, PrefixCycle<CRFPCNVFL2FXMASK_WM_PORT>, PrefixCycle<CRFPCNVFX2FLMASK_WM_PORT>, PrefixCycle<CRFPMASK_WM_PORT>, SimpleCycle<CRFPNLFMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRFPNlfMask_eRe, [SimpleCycle<CRFPNLFMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRSCDEn_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRPackSize_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRRnd_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRSat_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRFPMask_eRe, [SimpleCycle<CRFPMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRF2IMask_eRe, [SimpleCycle<CRF2IMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRF2FMask_eRe, [SimpleCycle<CRF2FMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_eVaddSign_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRF2BMask_eRe, [SimpleCycle<CRF2BMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRFPCnvFl2FxMask_eRe, [SimpleCycle<CRFPCNVFL2FXMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_eSRSSign_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_eUnpackSign_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRFPConvSat_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRUPSMode_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRBF8conf_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_eUPSSign_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRUnpackSize_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_ePackSign_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRMCDEn_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRFPCnvFx2FlMask_eRe, [SimpleCycle<CRFPCNVFX2FLMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRFP8conf_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRSRSMode_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRFPNlfMask_eRo, [SimpleCycle<CRFPNLFMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRSCDEn_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRPackSize_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRRnd_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRSat_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRFPMask_eRo, [SimpleCycle<CRFPMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRF2IMask_eRo, [SimpleCycle<CRF2IMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRF2FMask_eRo, [SimpleCycle<CRF2FMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_eVaddSign_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRF2BMask_eRo, [SimpleCycle<CRF2BMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRFPCnvFl2FxMask_eRo, [SimpleCycle<CRFPCNVFL2FXMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_eSRSSign_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_eUnpackSign_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRFPConvSat_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRUPSMode_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRBF8conf_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_eUPSSign_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRUnpackSize_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_ePackSign_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRMCDEn_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRFPCnvFx2FlMask_eRo, [SimpleCycle<CRFPCNVFX2FLMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRFP8conf_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOVX_mvx_cr_r_mCRSRSMode_eRo, [], [/*dst*/1, /*src*/1]>,

// id: me__instr128_or__st__st_streams__mv_cph2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_imm__noTlast__mMs
InstrItinData<II_MOV_CPH_mMStream_tlast_imm, [SimpleCycle<L_RS_L0_PORT>], [/*addr*/1, /*nw*/1, /*op*/1, /*id*/1, /*srMS0*/3]>,

// id: me__instr128_or__st__st_streams__mv_cph2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_reg__mR28_tlast__mMs
InstrItinData<II_MOV_CPH_mMStream_tlast_reg, [SimpleCycle<L_RS_L0_PORT>], [/*addr*/1, /*nw*/1, /*op*/1, /*id*/1, /*tlast*/1, /*srMS0*/3]>,

// id: me__instr128_or__st__st_streams__mv_cph2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_imm__noTlast__mMs
InstrItinData<II_MOV_CPH_nb_mMStream_tlast_imm, [SimpleCycle<L_RS_L0_PORT>], [/*addr*/1, /*nw*/1, /*op*/1, /*id*/1, /*srMS0*/3]>,

// id: me__instr128_or__st__st_streams__mv_cph2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_reg__mR28_tlast__mMs
InstrItinData<II_MOV_CPH_nb_mMStream_tlast_reg, [SimpleCycle<L_RS_L0_PORT>], [/*addr*/1, /*nw*/1, /*op*/1, /*id*/1, /*tlast*/1, /*srMS0*/3]>,

// id: me__instr128_or__st__st_streams__mv_cph2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_imm__doTlast__mMs
InstrItinData<II_MOV_CPH_nb_tlast, [SimpleCycle<L_RS_L0_PORT>], [/*addr*/1, /*nw*/1, /*op*/1, /*id*/1, /*srMS0*/3]>,

// id: me__instr128_or__st__st_streams__mv_cph2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_imm__doTlast__mMs
InstrItinData<II_MOV_CPH_tlast, [SimpleCycle<L_RS_L0_PORT>], [/*addr*/1, /*nw*/1, /*op*/1, /*id*/1, /*srMS0*/3]>,

// id: me__instr128_or__st__st_streams__mv_ph2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_imm__noTlast
InstrItinData<II_MOV_PH_mMStream_tlast_imm, [PrefixCycle<COL>, SimpleCycle<L_RS_L0_PORT>], [/*id*/1, /*pcktType*/1, /*srMS0*/3]>,

// id: me__instr128_or__st__st_streams__mv_ph2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_reg__mR28_tlast
InstrItinData<II_MOV_PH_mMStream_tlast_reg, [PrefixCycle<COL>, SimpleCycle<L_RS_L0_PORT>], [/*id*/1, /*pcktType*/1, /*tlast*/1, /*srMS0*/3]>,

// id: me__instr128_or__st__st_streams__mv_ph2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_imm__noTlast
InstrItinData<II_MOV_PH_nb_mMStream_tlast_imm, [PrefixCycle<COL>, SimpleCycle<L_RS_L0_PORT>], [/*id*/1, /*pcktType*/1, /*srMS0*/3]>,

// id: me__instr128_or__st__st_streams__mv_ph2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_reg__mR28_tlast
InstrItinData<II_MOV_PH_nb_mMStream_tlast_reg, [PrefixCycle<COL>, SimpleCycle<L_RS_L0_PORT>], [/*id*/1, /*pcktType*/1, /*tlast*/1, /*srMS0*/3]>,

// id: me__instr128_or__st__st_streams__mv_ph2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_imm__doTlast
InstrItinData<II_MOV_PH_nb_tlast, [PrefixCycle<COL>, SimpleCycle<L_RS_L0_PORT>], [/*id*/1, /*pcktType*/1, /*srMS0*/3]>,

// id: me__instr128_or__st__st_streams__mv_ph2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_imm__doTlast
InstrItinData<II_MOV_PH_tlast, [PrefixCycle<COL>, SimpleCycle<L_RS_L0_PORT>], [/*id*/1, /*pcktType*/1, /*srMS0*/3]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__flt2fx__mFlt2fx__mS3
InstrItinData<II_MOV_alu_mv_alu_flt2fx, [SimpleCycle<CNVOSTAT1>, SimpleCycle<L_WA_L0_PORT>], [/*d0*/2, /*s0*/1, /*s3*/1, /*srFPCnvFl2Fx*/1, /*crFPCnvFl2FxMask*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__fx2flt__mFx2flt__mS2
InstrItinData<II_MOV_alu_mv_alu_fx2flt, [EmptyCycles<1>, SimpleCycle<L_WA_L0_PORT>], [/*d0*/2, /*s0*/1, /*s2*/1, /*srFPCnvFx2Fl*/1, /*crFPCnvFx2FlMask*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_cg_or__mv_cg
InstrItinData<II_MOV_alu_mv_mv_mv_cg_or, [PrefixCycle<DC_WM_PORT>, PrefixCycle<DJ_WM_PORT>, PrefixCycle<DN_WM_PORT>, PrefixCycle<L_WM_L0_PORT>, PrefixCycle<M_WM_PORT>, SimpleCycle<P_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_cg_or_eDJ, [SimpleCycle<DJ_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_cg_or_mLRm, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_cg_or_eDC, [SimpleCycle<DC_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_cg_or_eRe, [SimpleCycle<L_WM_L0_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_cg_or_mLCmCg, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_cg_or_eVaddSign, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_cg_or_eP, [SimpleCycle<P_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_cg_or_eUnpackSign, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_cg_or_eSRSSign, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_cg_or_eUPSSign, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_cg_or_eS, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_cg_or_ePackSign, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_cg_or_eDN, [SimpleCycle<DN_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_cg_or_mSPm, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_cg_or_eM, [SimpleCycle<M_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_cg_or_eRo, [SimpleCycle<L_WM_L0_PORT>], [/*dst*/1, /*i*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_cntr2l__mLm
InstrItinData<II_MOV_alu_mv_mv_mv_cntr2l, [SimpleCycle<L_WM_L0_PORT>], [/*dst*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_eh_to_eh
InstrItinData<II_MOV_alu_mv_mv_mv_e_mv_eh_to_eh, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_eh_to_el
InstrItinData<II_MOV_alu_mv_mv_mv_e_mv_eh_to_el, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_eh_to_r
InstrItinData<II_MOV_alu_mv_mv_mv_e_mv_eh_to_r, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_el_to_eh
InstrItinData<II_MOV_alu_mv_mv_mv_e_mv_el_to_eh, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_el_to_el
InstrItinData<II_MOV_alu_mv_mv_mv_e_mv_el_to_el, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_el_to_r
InstrItinData<II_MOV_alu_mv_mv_mv_e_mv_el_to_r, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_r_to_eh
InstrItinData<II_MOV_alu_mv_mv_mv_e_mv_r_to_eh, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_r_to_el
InstrItinData<II_MOV_alu_mv_mv_mv_e_mv_r_to_el, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_g__mv_gh_to_gh
InstrItinData<II_MOV_alu_mv_mv_mv_g_mv_gh_to_gh, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_g__mv_gh_to_gl
InstrItinData<II_MOV_alu_mv_mv_mv_g_mv_gh_to_gl, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_g__mv_gh_to_r
InstrItinData<II_MOV_alu_mv_mv_mv_g_mv_gh_to_r, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_g__mv_gl_to_gh
InstrItinData<II_MOV_alu_mv_mv_mv_g_mv_gl_to_gh, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_g__mv_gl_to_gl
InstrItinData<II_MOV_alu_mv_mv_mv_g_mv_gl_to_gl, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_g__mv_gl_to_r
InstrItinData<II_MOV_alu_mv_mv_mv_g_mv_gl_to_r, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_g__mv_r_to_gh
InstrItinData<II_MOV_alu_mv_mv_mv_g_mv_r_to_gh, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_g__mv_r_to_gl
InstrItinData<II_MOV_alu_mv_mv_mv_g_mv_r_to_gl, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_r_sc
InstrItinData<II_MOV_alu_mv_mv_mv_r_sc, [SimpleCycle<L_WM_L0_PORT>], [/*dst*/1, /*src*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_sc_cg
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg, [PrefixCycle<CRF2BMASK_WM_PORT>, PrefixCycle<CRF2FMASK_WM_PORT>, PrefixCycle<CRF2IMASK_WM_PORT>, PrefixCycle<CRFPCNVFL2FXMASK_WM_PORT>, PrefixCycle<CRFPCNVFX2FLMASK_WM_PORT>, PrefixCycle<CRFPMASK_WM_PORT>, SimpleCycle<CRFPNLFMASK_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mSRF2IFlags, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mCRFPNlfMask, [SimpleCycle<CRFPNLFMASK_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mSRFPCnvFl2Fx, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mSRFifo_of, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mCRPackSize, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mCRRnd, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mSRCarry, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mCRF2IMask, [SimpleCycle<CRF2IMASK_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mCRF2FMask, [SimpleCycle<CRF2FMASK_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mCRF2BMask, [SimpleCycle<CRF2BMASK_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mSRF2FFlags, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mSRFPCnvFx2Fl, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mCRFPConvSat, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mCRBF8conf, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mSRFifo_uf, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mCRMCDEn, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mCRFPCnvFx2FlMask, [SimpleCycle<CRFPCNVFX2FLMASK_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mSRMS0, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mSRUPS_of, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mSRSparse_of, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mCRSRSMode, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mCRSCDEn, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mCRSat, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mCRFPMask, [SimpleCycle<CRFPMASK_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mCRFPCnvFl2FxMask, [SimpleCycle<CRFPCNVFL2FXMASK_WM_PORT>], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mSRSRS_of, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mSRFPFlags, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mSRF2BFlags, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mCRUPSMode, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mSRFPNlf, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mCRUnpackSize, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mSRSS0, [], [/*dst*/1, /*i*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_cg_mCRFP8conf, [], [/*dst*/1, /*i*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_sc_r
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r, [PrefixCycle<CRF2BMASK_WM_PORT>, PrefixCycle<CRF2FMASK_WM_PORT>, PrefixCycle<CRF2IMASK_WM_PORT>, PrefixCycle<CRFPCNVFL2FXMASK_WM_PORT>, PrefixCycle<CRFPCNVFX2FLMASK_WM_PORT>, PrefixCycle<CRFPMASK_WM_PORT>, SimpleCycle<CRFPNLFMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRF2IFlags_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRFPNlfMask_eRe, [SimpleCycle<CRFPNLFMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRFPCnvFl2Fx_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRFifo_of_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRPackSize_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRRnd_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRCarry_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRF2IMask_eRe, [SimpleCycle<CRF2IMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRF2FMask_eRe, [SimpleCycle<CRF2FMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRF2BMask_eRe, [SimpleCycle<CRF2BMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRF2FFlags_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRFPCnvFx2Fl_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRFPConvSat_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRBF8conf_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRFifo_uf_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRMCDEn_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRFPCnvFx2FlMask_eRe, [SimpleCycle<CRFPCNVFX2FLMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRMS0_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRUPS_of_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRSparse_of_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRSRSMode_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRSCDEn_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRSat_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRFPMask_eRe, [SimpleCycle<CRFPMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRFPCnvFl2FxMask_eRe, [SimpleCycle<CRFPCNVFL2FXMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRSRS_of_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRFPFlags_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRF2BFlags_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRUPSMode_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRFPNlf_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRUnpackSize_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRSS0_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRFP8conf_eRe, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRF2IFlags_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRFPNlfMask_eRo, [SimpleCycle<CRFPNLFMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRFPCnvFl2Fx_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRFifo_of_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRPackSize_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRRnd_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRCarry_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRF2IMask_eRo, [SimpleCycle<CRF2IMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRF2FMask_eRo, [SimpleCycle<CRF2FMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRF2BMask_eRo, [SimpleCycle<CRF2BMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRF2FFlags_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRFPCnvFx2Fl_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRFPConvSat_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRBF8conf_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRFifo_uf_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRMCDEn_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRFPCnvFx2FlMask_eRo, [SimpleCycle<CRFPCNVFX2FLMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRMS0_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRUPS_of_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRSparse_of_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRSRSMode_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRSCDEn_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRSat_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRFPMask_eRo, [SimpleCycle<CRFPMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRFPCnvFl2FxMask_eRo, [SimpleCycle<CRFPCNVFL2FXMASK_WM_PORT>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRSRS_of_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRFPFlags_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRF2BFlags_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRUPSMode_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRFPNlf_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRUnpackSize_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mSRSS0_eRo, [], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_sc_r_mCRFP8conf_eRo, [], [/*dst*/1, /*src*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_scl
InstrItinData<II_MOV_alu_mv_mv_mv_scl, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DC_WM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DJ_WM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<DN_WM_PORT>, PrefixCycle<L_WM_L0_PORT>, PrefixCycle<M_RM_PORT>, PrefixCycle<M_WM_PORT>, PrefixCycle<P_RM_PORT>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_mLEm, [PrefixCycle<COL>, PrefixCycle<DJ_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_mLEm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_mLEm, [PrefixCycle<COL>, PrefixCycle<DC_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_mLEm, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mLEm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_mLEm, [PrefixCycle<COL>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mLEm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mLEm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mLEm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_mLEm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_mLEm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_mLEm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_mLEm, [PrefixCycle<COL>, PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_mLEm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_mLEm, [PrefixCycle<COL>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_mLEm, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DJ_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_mLRm, [PrefixCycle<COL>, PrefixCycle<DJ_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_eRe, [PrefixCycle<COL>, PrefixCycle<DJ_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_mLSm, [PrefixCycle<COL>, PrefixCycle<DJ_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_eVaddSign, [PrefixCycle<COL>, PrefixCycle<DJ_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_WM_PORT>, PrefixCycle<P_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_eUnpackSign, [PrefixCycle<COL>, PrefixCycle<DJ_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_eSRSSign, [PrefixCycle<COL>, PrefixCycle<DJ_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_mCoreID, [PrefixCycle<COL>, PrefixCycle<DJ_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_eUPSSign, [PrefixCycle<COL>, PrefixCycle<DJ_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_eS, [PrefixCycle<COL>, PrefixCycle<DJ_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_mLCm, [PrefixCycle<COL>, PrefixCycle<DJ_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_ePackSign, [PrefixCycle<COL>, PrefixCycle<DJ_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_WM_PORT>, PrefixCycle<DN_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_mSPm, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_WM_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDJ_eRo, [PrefixCycle<COL>, PrefixCycle<DJ_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_mLRm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_eRe, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_mLSm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_eVaddSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<P_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_eUnpackSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_eSRSSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_mCoreID, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_eUPSSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_eS, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_mLCm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_ePackSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DN_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_mSPm, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<M_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLRm_eRo, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_WM_PORT>, PrefixCycle<DJ_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_mLRm, [PrefixCycle<COL>, PrefixCycle<DC_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DC_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_eRe, [PrefixCycle<COL>, PrefixCycle<DC_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_mLSm, [PrefixCycle<COL>, PrefixCycle<DC_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_eVaddSign, [PrefixCycle<COL>, PrefixCycle<DC_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_WM_PORT>, PrefixCycle<P_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_eUnpackSign, [PrefixCycle<COL>, PrefixCycle<DC_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_eSRSSign, [PrefixCycle<COL>, PrefixCycle<DC_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_mCoreID, [PrefixCycle<COL>, PrefixCycle<DC_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_eUPSSign, [PrefixCycle<COL>, PrefixCycle<DC_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_eS, [PrefixCycle<COL>, PrefixCycle<DC_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_mLCm, [PrefixCycle<COL>, PrefixCycle<DC_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_ePackSign, [PrefixCycle<COL>, PrefixCycle<DC_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_WM_PORT>, PrefixCycle<DN_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_mSPm, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_WM_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDC_eRo, [PrefixCycle<COL>, PrefixCycle<DC_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_mLRm, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_eRe, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_mLSm, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_eVaddSign, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, PrefixCycle<P_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_eUnpackSign, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_eSRSSign, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_mCoreID, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_eUPSSign, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_eS, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_mLCm, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_ePackSign, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DN_RM_PORT>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_mSPm, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRe_eRo, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mLRm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eRe, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mLSm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eVaddSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<P_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eUnpackSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eSRSSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mCoreID, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eUPSSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eS, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mLCm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_ePackSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DN_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSPm, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<M_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eRo, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_mLRm, [PrefixCycle<COL>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_eRe, [PrefixCycle<COL>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_mLSm, [PrefixCycle<COL>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_eVaddSign, [PrefixCycle<COL>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<P_RM_PORT>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_eUnpackSign, [PrefixCycle<COL>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_eSRSSign, [PrefixCycle<COL>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_mCoreID, [PrefixCycle<COL>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_eUPSSign, [PrefixCycle<COL>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_eS, [PrefixCycle<COL>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_mLCm, [PrefixCycle<COL>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_ePackSign, [PrefixCycle<COL>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DN_RM_PORT>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_mSPm, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<M_RM_PORT>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eP_eRo, [PrefixCycle<COL>, PrefixCycle<P_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mLRm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eRe, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mLSm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eVaddSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<P_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eUnpackSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eSRSSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mCoreID, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eUPSSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eS, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mLCm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_ePackSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DN_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSPm, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<M_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eRo, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mLRm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eRe, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mLSm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eVaddSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<P_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eUnpackSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eSRSSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mCoreID, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eUPSSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eS, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mLCm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_ePackSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DN_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSPm, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<M_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eRo, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mLRm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eRe, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mLSm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eVaddSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<P_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eUnpackSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eSRSSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mCoreID, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eUPSSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eS, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mLCm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_ePackSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DN_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSPm, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<M_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eRo, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_mLRm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_eRe, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_mLSm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_eVaddSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<P_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_eUnpackSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_eSRSSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_mCoreID, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_eUPSSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_eS, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_mLCm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_ePackSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DN_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_mSPm, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<M_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eS_eRo, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_mLRm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_eRe, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_mLSm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_eVaddSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<P_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_eUnpackSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_eSRSSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_mCoreID, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_eUPSSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_eS, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_mLCm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_ePackSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DN_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_mSPm, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<M_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mLCm_eRo, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_mLRm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_eRe, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_mLSm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_eVaddSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<P_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_eUnpackSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_eSRSSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_mCoreID, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_eUPSSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_eS, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_mLCm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_ePackSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DN_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_mSPm, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<M_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_ePackSign_eRo, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_mLRm, [PrefixCycle<COL>, PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_eRe, [PrefixCycle<COL>, PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_mLSm, [PrefixCycle<COL>, PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_eVaddSign, [PrefixCycle<COL>, PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DN_WM_PORT>, PrefixCycle<P_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_eUnpackSign, [PrefixCycle<COL>, PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_eSRSSign, [PrefixCycle<COL>, PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_mCoreID, [PrefixCycle<COL>, PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_eUPSSign, [PrefixCycle<COL>, PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_eS, [PrefixCycle<COL>, PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_mLCm, [PrefixCycle<COL>, PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_ePackSign, [PrefixCycle<COL>, PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DN_RM_PORT>, PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_mSPm, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DN_WM_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eDN_eRo, [PrefixCycle<COL>, PrefixCycle<DN_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_mLRm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_eRe, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_mLSm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_eVaddSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<P_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_eUnpackSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_eSRSSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_mCoreID, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_eUPSSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_eS, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_mLCm, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_ePackSign, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DN_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_mSPm, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<M_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_mSPm_eRo, [PrefixCycle<COL>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_mLRm, [PrefixCycle<COL>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_eRe, [PrefixCycle<COL>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_mLSm, [PrefixCycle<COL>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_eVaddSign, [PrefixCycle<COL>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<M_WM_PORT>, PrefixCycle<P_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_eUnpackSign, [PrefixCycle<COL>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_eSRSSign, [PrefixCycle<COL>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_mCoreID, [PrefixCycle<COL>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_eUPSSign, [PrefixCycle<COL>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_eS, [PrefixCycle<COL>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_mLCm, [PrefixCycle<COL>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_ePackSign, [PrefixCycle<COL>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DN_RM_PORT>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_mSPm, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<M_RM_PORT>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eM_eRo, [PrefixCycle<COL>, PrefixCycle<M_WM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_mLRm, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_eRe, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_mLSm, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_eVaddSign, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, PrefixCycle<P_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_eUnpackSign, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_eSRSSign, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_mCoreID, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_eUPSSign, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_eS, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_mLCm, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_ePackSign, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DN_RM_PORT>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_mSPm, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,
InstrItinData<II_MOV_alu_mv_mv_mv_scl_eRo_eRo, [PrefixCycle<COL>, PrefixCycle<L_WM_L0_PORT>, SimpleCycle<RSRC_EXECUTION_TRACE>], [/*dst*/1, /*src*/1]>,

// id: me__instr128_or__lda__mv_ss2scl__mSStream__mSStream_b
InstrItinData<II_MOV_lda, [EmptyCycles<6>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*srSS0*/8]>,

// id: me__instr128_or__st__st_streams__mv_scl2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_imm__noTlast
InstrItinData<II_MOV_st_mMStream_tlast_imm, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_st_mMStream_tlast_imm_eDJ, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<DJ_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_st_mMStream_tlast_imm_eDC, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<DC_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_st_mMStream_tlast_imm_eRe, [SimpleCycle<L_RS_L0_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_st_mMStream_tlast_imm_eDN, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<DN_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_st_mMStream_tlast_imm_eP, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<P_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_st_mMStream_tlast_imm_eM, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<M_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_st_mMStream_tlast_imm_eRo, [SimpleCycle<L_RS_L0_PORT>], [/*src*/1, /*srMS0*/3]>,

// id: me__instr128_or__st__st_streams__mv_scl2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_reg__mR28_tlast
InstrItinData<II_MOV_st_mMStream_tlast_reg, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>], [/*src*/1, /*tlast*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_st_mMStream_tlast_reg_eDJ, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<DJ_RM_PORT>], [/*src*/1, /*tlast*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_st_mMStream_tlast_reg_eDC, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<DC_RM_PORT>], [/*src*/1, /*tlast*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_st_mMStream_tlast_reg_eRe, [SimpleCycle<L_RS_L0_PORT>], [/*src*/1, /*tlast*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_st_mMStream_tlast_reg_eDN, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<DN_RM_PORT>], [/*src*/1, /*tlast*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_st_mMStream_tlast_reg_eP, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<P_RM_PORT>], [/*src*/1, /*tlast*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_st_mMStream_tlast_reg_eM, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<M_RM_PORT>], [/*src*/1, /*tlast*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_st_mMStream_tlast_reg_eRo, [SimpleCycle<L_RS_L0_PORT>], [/*src*/1, /*tlast*/1, /*srMS0*/3]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_delay__Delay1
InstrItinData<II_MOV_d1, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_mLEm, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_mLEm, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DJ_RM_PORT>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_mLRm, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DC_RM_PORT>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_eRe, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_mLSm, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_eVaddSign, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<P_RM_PORT>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_eUnpackSign, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_eSRSSign, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_mCoreID, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_eUPSSign, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_eS, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_mLCm, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_ePackSign, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DN_RM_PORT>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_mSPm, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<M_RM_PORT>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRe_eRo, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DJ_RM_PORT>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_mLRm, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DC_RM_PORT>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_eRe, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_mLSm, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_eVaddSign, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<P_RM_PORT>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_eUnpackSign, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_eSRSSign, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_mCoreID, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_eUPSSign, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_eS, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_mLCm, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_ePackSign, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DN_RM_PORT>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_mSPm, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<M_RM_PORT>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,
InstrItinData<II_MOV_d1_eRo_eRo, [SimpleCycle<COL>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_delay__Delay2
InstrItinData<II_MOV_d2, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_mLEm, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_mLEm, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DJ_RM_PORT>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_mLRm, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DC_RM_PORT>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_eRe, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_mLSm, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_eVaddSign, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<P_RM_PORT>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_eUnpackSign, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_eSRSSign, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_mCoreID, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_eUPSSign, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_eS, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_mLCm, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_ePackSign, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DN_RM_PORT>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_mSPm, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<M_RM_PORT>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRe_eRo, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DJ_RM_PORT>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_mLRm, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DC_RM_PORT>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_eRe, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_mLSm, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_eVaddSign, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<P_RM_PORT>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_eUnpackSign, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_eSRSSign, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_mCoreID, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_eUPSSign, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_eS, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_mLCm, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_ePackSign, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DN_RM_PORT>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_mSPm, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<M_RM_PORT>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,
InstrItinData<II_MOV_d2_eRo_eRo, [SimpleCycle<COL>, EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/3, /*src*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_delay__Delay3
InstrItinData<II_MOV_d3, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_mLEm, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_mLEm, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DJ_RM_PORT>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_mLRm, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DC_RM_PORT>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_eRe, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_mLSm, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_eVaddSign, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<P_RM_PORT>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_eUnpackSign, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_eSRSSign, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_mCoreID, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_eUPSSign, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_eS, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_mLCm, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_ePackSign, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DN_RM_PORT>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_mSPm, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<M_RM_PORT>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRe_eRo, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DJ_RM_PORT>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_mLRm, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DC_RM_PORT>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_eRe, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_mLSm, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_eVaddSign, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<P_RM_PORT>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_eUnpackSign, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_eSRSSign, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_mCoreID, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_eUPSSign, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_eS, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_mLCm, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_ePackSign, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DN_RM_PORT>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_mSPm, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<M_RM_PORT>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,
InstrItinData<II_MOV_d3_eRo_eRo, [SimpleCycle<COL>, EmptyCycles<2>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/4, /*src*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_delay__Delay4
InstrItinData<II_MOV_d4, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_mLEm, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_mLEm, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DJ_RM_PORT>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_mLRm, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DC_RM_PORT>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_eRe, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_mLSm, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_eVaddSign, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<P_RM_PORT>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_eUnpackSign, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_eSRSSign, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_mCoreID, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_eUPSSign, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_eS, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_mLCm, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_ePackSign, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DN_RM_PORT>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_mSPm, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<M_RM_PORT>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRe_eRo, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DJ_RM_PORT>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_mLRm, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DC_RM_PORT>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_eRe, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_mLSm, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_eVaddSign, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<P_RM_PORT>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_eUnpackSign, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_eSRSSign, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_mCoreID, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_eUPSSign, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_eS, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_mLCm, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_ePackSign, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DN_RM_PORT>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_mSPm, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<M_RM_PORT>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,
InstrItinData<II_MOV_d4_eRo_eRo, [SimpleCycle<COL>, EmptyCycles<3>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/5, /*src*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_delay__Delay5
InstrItinData<II_MOV_d5, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_mLEm, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_mLEm, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DJ_RM_PORT>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_mLRm, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DC_RM_PORT>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_eRe, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_mLSm, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_eVaddSign, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<P_RM_PORT>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_eUnpackSign, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_eSRSSign, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_mCoreID, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_eUPSSign, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_eS, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_mLCm, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_ePackSign, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DN_RM_PORT>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_mSPm, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<M_RM_PORT>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRe_eRo, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DJ_RM_PORT>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_mLRm, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DC_RM_PORT>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_eRe, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_mLSm, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_eVaddSign, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<P_RM_PORT>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_eUnpackSign, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_eSRSSign, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_mCoreID, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_eUPSSign, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_eS, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_mLCm, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_ePackSign, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DN_RM_PORT>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_mSPm, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<M_RM_PORT>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,
InstrItinData<II_MOV_d5_eRo_eRo, [SimpleCycle<COL>, EmptyCycles<4>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/6, /*src*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_delay__Delay6
InstrItinData<II_MOV_d6, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_mLEm, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_mLEm, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DJ_RM_PORT>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_mLRm, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DC_RM_PORT>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_eRe, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_mLSm, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_eVaddSign, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<P_RM_PORT>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_eUnpackSign, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_eSRSSign, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_mCoreID, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_eUPSSign, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_eS, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_mLCm, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_ePackSign, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DN_RM_PORT>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_mSPm, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<M_RM_PORT>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRe_eRo, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_eDJ, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DJ_RM_PORT>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_mLRm, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_eDC, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DC_RM_PORT>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_eRe, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_mLSm, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_eVaddSign, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_eP, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<P_RM_PORT>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_eUnpackSign, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_eSRSSign, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_mCoreID, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_eUPSSign, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_eS, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_mLCm, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_ePackSign, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_eDN, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<DN_RM_PORT>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_mSPm, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_eM, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<COL>, SimpleCycle<M_RM_PORT>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,
InstrItinData<II_MOV_d6_eRo_eRo, [SimpleCycle<COL>, EmptyCycles<5>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/7, /*src*/1]>,

// id: me__instr128_or__lda__mv_ss2scl__mSStream__mSStream_nb
InstrItinData<II_MOV_nb_lda, [EmptyCycles<6>, SimpleCycle<L_WA_L0_PORT>], [/*dst*/7, /*srSS0*/8]>,

// id: me__instr128_or__st__st_streams__mv_scl2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_imm__noTlast
InstrItinData<II_MOV_nb_st_mMStream_tlast_imm, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_st_mMStream_tlast_imm_eDJ, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<DJ_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_st_mMStream_tlast_imm_eDC, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<DC_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_st_mMStream_tlast_imm_eRe, [SimpleCycle<L_RS_L0_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_st_mMStream_tlast_imm_eDN, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<DN_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_st_mMStream_tlast_imm_eP, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<P_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_st_mMStream_tlast_imm_eM, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<M_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_st_mMStream_tlast_imm_eRo, [SimpleCycle<L_RS_L0_PORT>], [/*src*/1, /*srMS0*/3]>,

// id: me__instr128_or__st__st_streams__mv_scl2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_reg__mR28_tlast
InstrItinData<II_MOV_nb_st_mMStream_tlast_reg, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>], [/*src*/1, /*tlast*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_st_mMStream_tlast_reg_eDJ, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<DJ_RM_PORT>], [/*src*/1, /*tlast*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_st_mMStream_tlast_reg_eDC, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<DC_RM_PORT>], [/*src*/1, /*tlast*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_st_mMStream_tlast_reg_eRe, [SimpleCycle<L_RS_L0_PORT>], [/*src*/1, /*tlast*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_st_mMStream_tlast_reg_eDN, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<DN_RM_PORT>], [/*src*/1, /*tlast*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_st_mMStream_tlast_reg_eP, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<P_RM_PORT>], [/*src*/1, /*tlast*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_st_mMStream_tlast_reg_eM, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<M_RM_PORT>], [/*src*/1, /*tlast*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_st_mMStream_tlast_reg_eRo, [SimpleCycle<L_RS_L0_PORT>], [/*src*/1, /*tlast*/1, /*srMS0*/3]>,

// id: me__instr128_or__st__st_streams__mv_scl2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_imm__doTlast
InstrItinData<II_MOV_nb_tlast, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_tlast_eDJ, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<DJ_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_tlast_eDC, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<DC_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_tlast_eRe, [SimpleCycle<L_RS_L0_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_tlast_eDN, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<DN_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_tlast_eP, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<P_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_tlast_eM, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<M_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_nb_tlast_eRo, [SimpleCycle<L_RS_L0_PORT>], [/*src*/1, /*srMS0*/3]>,

// id: me__instr128_or__st__st_streams__mv_scl2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_imm__doTlast
InstrItinData<II_MOV_tlast, [PrefixCycle<AGU_RM_PORT>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_tlast_eDJ, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<DJ_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_tlast_eDC, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<DC_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_tlast_eRe, [SimpleCycle<L_RS_L0_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_tlast_eDN, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<DN_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_tlast_eP, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<P_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_tlast_eM, [PrefixCycle<AGU_RM_PORT>, SimpleCycle<M_RM_PORT>], [/*src*/1, /*srMS0*/3]>,
InstrItinData<II_MOV_tlast_eRo, [SimpleCycle<L_RS_L0_PORT>], [/*src*/1, /*srMS0*/3]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mac_r_rrr
InstrItinData<II_MSC, [EmptyCycles<1>, SimpleCycle<L_WA_L0_PORT>], [/*d0*/2, /*a0*/2, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mul_r_rr
InstrItinData<II_MUL, [EmptyCycles<1>, SimpleCycle<L_WA_L0_PORT>], [/*d0*/2, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__sne0
InstrItinData<II_NEZ, [], [/*d0*/1, /*s0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__sne
InstrItinData<II_NE, [], [/*d0*/1, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__lda__nop_lda
InstrItinData<II_NOPA, [], []>,

// id: me__instr128_or__ldb__nop_ldb
InstrItinData<II_NOPB, [], []>,

// id: me__instr128_or__alu_mv__alumv_or__mv__nop_mv
InstrItinData<II_NOPM, [], []>,

// id: me__instr128_or__st__nop_sts
InstrItinData<II_NOPS, [], []>,

// id: me__instr128_or__vec__nop_vec
InstrItinData<II_NOPV, [], []>,

// id: me__instr128_or__alu_mv__lng__nop_lng
InstrItinData<II_NOPXM, [], []>,

// id: me__instr128_or__alu_mv__alumv_or__alu__nop_alu
InstrItinData<II_NOPX, [], []>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__or
InstrItinData<II_OR, [], [/*d0*/1, /*s0*/1, /*s1*/1]>,

// id: me__instr128_or__lda__lda_ptr_inc__agua_sa__standalone__agua_sa__sa_modes__agua_sa__pstm_2d__mPa__mDa
InstrItinData<II_PADDA_2D, [], [/*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1]>,

// id: me__instr128_or__lda__lda_ptr_inc__agua_sa__standalone__agua_sa__sa_modes__agua_sa__pstm_3d__mPa__mDSa
InstrItinData<II_PADDA_3D, [], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1]>,

// id: me__instr128_or__lda__lda_ptr_inc__agua_sa__standalone__agua_sa__sa_modes__agua_sa__pstm_nrm__mPa__mMa
InstrItinData<II_PADDA_pstm_nrm, [], [/*ptr_out*/1, /*ptr*/1, /*mod*/1]>,

// id: me__instr128_or__lda__lda_ptr_inc__agua_sa__standalone__agua_sa__sa_modes__agua_sa__pstm_nrm_imm__mPa__m_c10s_step64
InstrItinData<II_PADDA_pstm_nrm_imm, [], [/*ptr_out*/1, /*ptr*/1, /*imm*/1]>,

// id: me__instr128_or__ldb__ldb_ptr_inc__agub_sa__standalone__agub_sa__sa_modes__agub_sa__pstm_2d__mPb__mDb
InstrItinData<II_PADDB_2D, [], [/*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1]>,

// id: me__instr128_or__ldb__ldb_ptr_inc__agub_sa__standalone__agub_sa__sa_modes__agub_sa__pstm_3d__mPb__mDSb
InstrItinData<II_PADDB_3D, [], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1]>,

// id: me__instr128_or__ldb__ldb_ptr_inc__agub_sa__standalone__agub_sa__sa_modes__agub_sa__pstm_nrm__mPb__mMb
InstrItinData<II_PADDB_pstm_nrm, [], [/*ptr_out*/1, /*ptr*/1, /*mod*/1]>,

// id: me__instr128_or__ldb__ldb_ptr_inc__agub_sa__standalone__agub_sa__sa_modes__agub_sa__pstm_nrm_imm__mPb__m_c10s_step64
InstrItinData<II_PADDB_pstm_nrm_imm, [], [/*ptr_out*/1, /*ptr*/1, /*imm*/1]>,

// id: me__instr128_or__st__st_ptr_inc__agus_sa__standalone__agus_sa__sa_modes__agus_sa__pstm_2d__mPs__mDs
InstrItinData<II_PADDS_2D, [], [/*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1]>,

// id: me__instr128_or__st__st_ptr_inc__agus_sa__standalone__agus_sa__sa_modes__agus_sa__pstm_3d__mPs__mDSs
InstrItinData<II_PADDS_3D, [], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1]>,

// id: me__instr128_or__st__st_ptr_inc__agus_sa__standalone__agus_sa__sa_modes__agus_sa__pstm_nrm__mPs__mMs
InstrItinData<II_PADDS_pstm_nrm, [], [/*ptr_out*/1, /*ptr*/1, /*mod*/1]>,

// id: me__instr128_or__st__st_ptr_inc__agus_sa__standalone__agus_sa__sa_modes__agus_sa__pstm_nrm_imm__mPs__m_c10s_step64
InstrItinData<II_PADDS_pstm_nrm_imm, [], [/*ptr_out*/1, /*ptr*/1, /*imm*/1]>,

// id: me__instr128_or__lda__lda_ptr_inc_long__agua_sa_lng__standalone_long__agua_sa_lng__standalone_lng__agua_sa_lng__pstm_sp__mSPa__mMa
InstrItinData<II_PADDXM_pstm_sp, [], [/*mod*/1, /*sp*/1, /*sp*/1]>,

// id: me__instr128_or__lda__lda_ptr_inc_long__agua_sa_lng__standalone_long__agua_sa_lng__standalone_lng__agua_sa_lng__pstm_sp_imm__mSPa__m_c19s_step64
InstrItinData<II_PADDXM_pstm_sp_imm, [], [/*imm*/1, /*sp*/1, /*sp*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__popcnt
InstrItinData<II_POPCOUNT, [], [/*d0*/1, /*s0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__release_cond__mLockId__mLockId_imm__mR23_lock
InstrItinData<II_REL_COND_mLockId_imm, [InstrStage<4, [LCKREQ]>], [/*id*/1, /*s1*/1, /*r23*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__release_cond__mLockId__mLockId_reg__mR23_lock
InstrItinData<II_REL_COND_mLockId_reg, [InstrStage<4, [LCKREQ]>], [/*s0*/1, /*s1*/1, /*r23*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__release__mLockId__mLockId_imm
InstrItinData<II_REL_mLockId_imm, [InstrStage<4, [LCKREQ]>], [/*id*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__release__mLockId__mLockId_reg
InstrItinData<II_REL_mLockId_reg, [InstrStage<4, [LCKREQ]>], [/*s0*/1, /*s1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__jump_return__mLRx
InstrItinData<II_RET, [SimpleCycle<RSRC_EXECUTION_TRACE>], [/*lr*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__subx
InstrItinData<II_SBC, [], [/*d0*/1, /*s0*/1, /*s1*/1, /*srCarry*/1, /*srCarry*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__select_r_rr__IF_FALSE__mR27_select
InstrItinData<II_SEL_EQZ, [], [/*d0*/1, /*s0*/1, /*s1*/1, /*r27*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__select_r_rr__IF_TRUE__mR27_select
InstrItinData<II_SEL_NEZ, [], [/*d0*/1, /*s0*/1, /*s1*/1, /*r27*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_sqrt
InstrItinData<II_SQRT_nlf_sqrt, [EmptyCycles<3>, SimpleCycle<L_WA_L0_PORT>], [/*d0*/4, /*s0*/1, /*srFPNlf*/4, /*crFPNlfMask*/4]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_sqrt__mOptConvDel__mNlfFlt2fx__mFlt2fx_delayed__mS3
InstrItinData<II_SQRT_nlf_sqrt_mOptConvDel_mS3, [EmptyCycles<3>, PrefixCycle<CNVOSTAT1>, SimpleCycle<L_WA_L0_PORT>], [/*d0*/4, /*mcnv_del*/4, /*s0*/1, /*srFPCnvFl2Fx*/4, /*srFPNlf*/4, /*crFPCnvFl2FxMask*/4, /*crFPNlfMask*/4]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_sqrt__mOptConvDel__mNlfFlt2fx__mFlt2fx_delayed__mS3__mOptConv__mNlfFx2flt__mFx2flt__mS2
InstrItinData<II_SQRT_nlf_sqrt_mOptConvDel_mS3_mOptConv, [EmptyCycles<3>, PrefixCycle<CNVOSTAT1>, SimpleCycle<L_WA_L0_PORT>], [/*d0*/4, /*mcnv_del*/4, /*s0*/1, /*mcnv*/1, /*srFPCnvFl2Fx*/4, /*srFPCnvFx2Fl*/1, /*srFPNlf*/4, /*crFPCnvFl2FxMask*/4, /*crFPCnvFx2FlMask*/1, /*crFPNlfMask*/4]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_sqrt__mOptConv__mNlfFx2flt__mFx2flt__mS2
InstrItinData<II_SQRT_nlf_sqrt_mOptConv, [EmptyCycles<3>, SimpleCycle<L_WA_L0_PORT>], [/*d0*/4, /*s0*/1, /*mcnv*/1, /*srFPCnvFx2Fl*/1, /*srFPNlf*/4, /*crFPCnvFx2FlMask*/1, /*crFPNlfMask*/4]>,

// id: me__instr128_or__st__dml_sts_scalar__st__agus_dml__nrm__agus_dml__normal__agus_dml__pstm_2d__mPs__mDs
MemInstrItinData<II_ST_2D_dml_sts_scalar, [AvoidPartWordStore, PrefixCycle<FEY_RS_L0_PORT>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dml_sts_scalar_eFlo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dml_sts_scalar_eEGhe_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dml_sts_scalar_eL, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dml_sts_scalar_eEEo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dml_sts_scalar_eFle, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dml_sts_scalar_eGGo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dml_sts_scalar_eFhe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dml_sts_scalar_eEGho_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dml_sts_scalar_eFho, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dml_sts_scalar_eEGlo_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dml_sts_scalar_eEGle_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dml_sts_scalar_eGGe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dml_sts_scalar_eEEe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dms_sts_g__st__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_2d__mPs__mDs
MemInstrItinData<II_ST_2D_dms_sts_g, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dms_sts_scalar__st__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_2d__mPs__mDs
MemInstrItinData<II_ST_2D_dms_sts_scalar, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<FEY_RS_L0_PORT>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dms_sts_scalar_eEle, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dms_sts_scalar_eElo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dms_sts_scalar_eDJ, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DJ_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dms_sts_scalar_mLRs, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dms_sts_scalar_eDC, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DC_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dms_sts_scalar_eRe, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dms_sts_scalar_eDN, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DN_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dms_sts_scalar_eEhe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dms_sts_scalar_eP, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<P_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dms_sts_scalar_eM, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<M_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dms_sts_scalar_eRo, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_dms_sts_scalar_eEho, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_f__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_2d__mPs__mDs
MemInstrItinData<II_ST_2D_dmv_sts_f, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_sts__dmh_sts__agua_dmh__normal__agua_dmh__pstm_2d__mPa__mDa
MemInstrItinData<II_ST_2D_s16, [IsPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DMS_WM>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/7, /*ptr*/1, /*mod*/1, /*pe2_ads*/6, /*pe2_ads*/6], MemoryCycles<[5, 11]>>,

// id: me__instr128_or__lda__dmhb_sts__dmb_sts__agua_dmb__normal__agua_dmb__pstm_2d__mPa__mDa
MemInstrItinData<II_ST_2D_s8, [IsPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DMS_WM>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/7, /*ptr*/1, /*mod*/1, /*pe2_ads*/6, /*pe2_ads*/6], MemoryCycles<[5, 11]>>,

// id: me__instr128_or__st__dml_sts_scalar__st__agus_dml__nrm__agus_dml__normal__agus_dml__pstm_3d__mPs__mDSs
MemInstrItinData<II_ST_3D_dml_sts_scalar, [AvoidPartWordStore, PrefixCycle<FEY_RS_L0_PORT>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dml_sts_scalar_eFlo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dml_sts_scalar_eEGhe_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dml_sts_scalar_eL, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dml_sts_scalar_eEEo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dml_sts_scalar_eFle, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dml_sts_scalar_eGGo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dml_sts_scalar_eFhe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dml_sts_scalar_eEGho_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dml_sts_scalar_eFho, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dml_sts_scalar_eEGlo_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dml_sts_scalar_eEGle_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dml_sts_scalar_eGGe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dml_sts_scalar_eEEe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dms_sts_g__st__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_3d__mPs__mDSs
MemInstrItinData<II_ST_3D_dms_sts_g, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dms_sts_scalar__st__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_3d__mPs__mDSs
MemInstrItinData<II_ST_3D_dms_sts_scalar, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<FEY_RS_L0_PORT>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dms_sts_scalar_eEle, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dms_sts_scalar_eElo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dms_sts_scalar_eDJ, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DJ_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dms_sts_scalar_mLRs, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dms_sts_scalar_eDC, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DC_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dms_sts_scalar_eRe, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dms_sts_scalar_eDN, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DN_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dms_sts_scalar_eEhe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dms_sts_scalar_eP, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<P_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dms_sts_scalar_eM, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<M_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dms_sts_scalar_eRo, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_dms_sts_scalar_eEho, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_f__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_3d__mPs__mDSs
MemInstrItinData<II_ST_3D_dmv_sts_f, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_sts__dmh_sts__agua_dmh__normal__agua_dmh__pstm_3d__mPa__mDSa
MemInstrItinData<II_ST_3D_s16, [IsPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DMS_WM>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/7, /*ptr*/1, /*mod*/1, /*pe2_ads*/6, /*pe2_ads*/6], MemoryCycles<[5, 11]>>,

// id: me__instr128_or__lda__dmhb_sts__dmb_sts__agua_dmb__normal__agua_dmb__pstm_3d__mPa__mDSa
MemInstrItinData<II_ST_3D_s8, [IsPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DMS_WM>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/7, /*ptr*/1, /*mod*/1, /*pe2_ads*/6, /*pe2_ads*/6], MemoryCycles<[5, 11]>>,

// id: me__instr128_or__st__tm_sts__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_2d__mPs__mDs
MemInstrItinData<II_ST_TM_2D, [PrefixCycle<DMS_WM>, SimpleCycle<L_RS_L0_PORT>, PrefixCycle<DM_ADS>, SimpleCycle<TM_AD>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[4]>>,

// id: me__instr128_or__st__tm_sts__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_3d__mPs__mDSs
MemInstrItinData<II_ST_TM_3D, [PrefixCycle<DMS_WM>, SimpleCycle<L_RS_L0_PORT>, PrefixCycle<DM_ADS>, SimpleCycle<TM_AD>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[4]>>,

// id: me__instr128_or__st__tm_sts__agus_dms__nrm__agus_dms__normal__agus_dms__idx__mPs__mDJs
MemInstrItinData<II_ST_TM_idx, [PrefixCycle<DMS_WM>, SimpleCycle<L_RS_L0_PORT>, PrefixCycle<DM_ADS>, SimpleCycle<TM_AD>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[4]>>,

// id: me__instr128_or__st__tm_sts__agus_dms__nrm__agus_dms__normal__agus_dms__idx_imm__mPs__m_c06s_step4
MemInstrItinData<II_ST_TM_idx_imm, [PrefixCycle<DMS_WM>, SimpleCycle<L_RS_L0_PORT>, PrefixCycle<DM_ADS>, SimpleCycle<TM_AD>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[4]>>,

// id: me__instr128_or__st__tm_sts__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_nrm__mPs__mMs
MemInstrItinData<II_ST_TM_pstm_nrm, [PrefixCycle<DMS_WM>, SimpleCycle<L_RS_L0_PORT>, PrefixCycle<DM_ADS>, SimpleCycle<TM_AD>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[4]>>,

// id: me__instr128_or__st__tm_sts__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_nrm_imm__mPs__m_c06s_step4
MemInstrItinData<II_ST_TM_pstm_nrm_imm, [PrefixCycle<DMS_WM>, SimpleCycle<L_RS_L0_PORT>, PrefixCycle<DM_ADS>, SimpleCycle<TM_AD>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[4]>>,

// id: me__instr128_or__st__dml_sts_scalar__spill__agus_dml__nrm_spill__agus_dml__spill__mSPs__m_c13n_step8
MemInstrItinData<II_ST_dml_sts_scalar_spill, [AvoidPartWordStore, PrefixCycle<FEY_RS_L0_PORT>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_spill_eFlo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_spill_eEGhe_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_spill_eL, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_spill_eEEo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_spill_eFle, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_spill_eGGo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_spill_eFhe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_spill_eEGho_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_spill_eFho, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_spill_eEGlo_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_spill_eEGle_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_spill_eGGe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_spill_eEEe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dml_sts_scalar__st__agus_dml__nrm__agus_dml__normal__agus_dml__idx__mPs__mDJs
MemInstrItinData<II_ST_dml_sts_scalar_st_idx, [AvoidPartWordStore, PrefixCycle<FEY_RS_L0_PORT>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_eFlo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_eEGhe_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_eL, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_eEEo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_eFle, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_eGGo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_eFhe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_eEGho_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_eFho, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_eEGlo_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_eEGle_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_eGGe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_eEEe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dml_sts_scalar__st__agus_dml__nrm__agus_dml__normal__agus_dml__idx_imm__mPs__m_c07s_step8
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_imm, [AvoidPartWordStore, PrefixCycle<FEY_RS_L0_PORT>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_imm_eFlo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_imm_eEGhe_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_imm_eL, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_imm_eEEo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_imm_eFle, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_imm_eGGo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_imm_eFhe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_imm_eEGho_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_imm_eFho, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_imm_eEGlo_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_imm_eEGle_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_imm_eGGe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_idx_imm_eEEe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dml_sts_scalar__st__agus_dml__nrm__agus_dml__normal__agus_dml__pstm_nrm__mPs__mMs
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm, [AvoidPartWordStore, PrefixCycle<FEY_RS_L0_PORT>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_eFlo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_eEGhe_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_eL, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_eEEo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_eFle, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_eGGo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_eFhe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_eEGho_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_eFho, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_eEGlo_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_eEGle_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_eGGe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_eEEe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dml_sts_scalar__st__agus_dml__nrm__agus_dml__normal__agus_dml__pstm_nrm_imm__mPs__m_c07s_step8
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_imm, [AvoidPartWordStore, PrefixCycle<FEY_RS_L0_PORT>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eFlo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eEGhe_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eL, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eEEo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eFle, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eGGo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eFhe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eEGho_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eFho, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eEGlo_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eEGle_s, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eGGe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eEEe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dms_sts_g__spill__agus_dms__nrm_spill__agus_dms__spill__mSPs__m_c12n_step4
MemInstrItinData<II_ST_dms_sts_g_spill, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dms_sts_g__st__agus_dms__nrm__agus_dms__normal__agus_dms__idx__mPs__mDJs
MemInstrItinData<II_ST_dms_sts_g_st_idx, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dms_sts_g__st__agus_dms__nrm__agus_dms__normal__agus_dms__idx_imm__mPs__m_c06s_step4
MemInstrItinData<II_ST_dms_sts_g_st_idx_imm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dms_sts_g__st__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_nrm__mPs__mMs
MemInstrItinData<II_ST_dms_sts_g_st_pstm_nrm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dms_sts_g__st__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_nrm_imm__mPs__m_c06s_step4
MemInstrItinData<II_ST_dms_sts_g_st_pstm_nrm_imm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dms_sts_scalar__spill__agus_dms__nrm_spill__agus_dms__spill__mSPs__m_c12n_step4
MemInstrItinData<II_ST_dms_sts_scalar_spill, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<FEY_RS_L0_PORT>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_spill_eEle, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_spill_eElo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_spill_eDJ, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DJ_RM_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_spill_mLRs, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_spill_eDC, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DC_RM_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_spill_eRe, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_spill_eDN, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DN_RM_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_spill_eEhe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_spill_eP, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<P_RM_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_spill_eM, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<M_RM_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_spill_eRo, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_spill_eEho, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dms_sts_scalar__st__agus_dms__nrm__agus_dms__normal__agus_dms__idx__mPs__mDJs
MemInstrItinData<II_ST_dms_sts_scalar_st_idx, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<FEY_RS_L0_PORT>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_eEle, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_eElo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_eDJ, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DJ_RM_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_mLRs, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_eDC, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DC_RM_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_eRe, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_eDN, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DN_RM_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_eEhe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_eP, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<P_RM_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_eM, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<M_RM_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_eRo, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_eEho, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dms_sts_scalar__st__agus_dms__nrm__agus_dms__normal__agus_dms__idx_imm__mPs__m_c06s_step4
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_imm, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<FEY_RS_L0_PORT>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_imm_eEle, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_imm_eElo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_imm_eDJ, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DJ_RM_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_imm_mLRs, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_imm_eDC, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DC_RM_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_imm_eRe, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_imm_eDN, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DN_RM_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_imm_eEhe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_imm_eP, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<P_RM_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_imm_eM, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<M_RM_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_imm_eRo, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_idx_imm_eEho, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dms_sts_scalar__st__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_nrm__mPs__mMs
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<FEY_RS_L0_PORT>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_eEle, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_eElo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_eDJ, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DJ_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_mLRs, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_eDC, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DC_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_eRe, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_eDN, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DN_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_eEhe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_eP, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<P_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_eM, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<M_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_eRo, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_eEho, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dms_sts_scalar__st__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_nrm_imm__mPs__m_c06s_step4
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_imm, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, PrefixCycle<DC_RM_PORT>, PrefixCycle<DJ_RM_PORT>, PrefixCycle<DN_RM_PORT>, PrefixCycle<FEY_RS_L0_PORT>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<M_RM_PORT>, SimpleCycle<P_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eEle, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eElo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eDJ, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DJ_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_imm_mLRs, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eDC, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DC_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eRe, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eDN, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<DN_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eEhe, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eP, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<P_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eM, [AvoidPartWordStore, PrefixCycle<AGU_RM_PORT>, SimpleCycle<M_RM_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eRo, [AvoidPartWordStore, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,
MemInstrItinData<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eEho, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_f__spill__agus_dmv__nrm_spill__agus_dmv__spill__mSPs__m_c14n_step16
MemInstrItinData<II_ST_dmv_sts_f_spill, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_f__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx__mPs__mDJs
MemInstrItinData<II_ST_dmv_sts_f_st_idx, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_f__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx_imm__mPs__m_c08s_step16
MemInstrItinData<II_ST_dmv_sts_f_st_idx_imm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_f__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm__mPs__mMs
MemInstrItinData<II_ST_dmv_sts_f_st_pstm_nrm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_f__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm_imm__mPs__m_c08s_step16
MemInstrItinData<II_ST_dmv_sts_f_st_pstm_nrm_imm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmhb_sts__dmh_sts__agua_dmh__normal__agua_dmh__idx__mPa__mDJa
MemInstrItinData<II_ST_s16_idx, [IsPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DMS_WM>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/7, /*ptr*/1, /*dj*/1, /*pe2_ads*/6, /*pe2_ads*/6], MemoryCycles<[5, 11]>>,

// id: me__instr128_or__lda__dmhb_sts__dmh_sts__agua_dmh__normal__agua_dmh__idx_imm__mPa__m_c05s_step2
MemInstrItinData<II_ST_s16_idx_imm, [IsPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DMS_WM>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/7, /*ptr*/1, /*imm*/1, /*pe2_ads*/6, /*pe2_ads*/6], MemoryCycles<[5, 11]>>,

// id: me__instr128_or__lda__dmhb_sts__dmh_sts__agua_dmh__normal__agua_dmh__pstm_nrm__mPa__mMa
MemInstrItinData<II_ST_s16_pstm_nrm, [IsPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DMS_WM>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/7, /*ptr*/1, /*mod*/1, /*pe2_ads*/6, /*pe2_ads*/6], MemoryCycles<[5, 11]>>,

// id: me__instr128_or__lda__dmhb_sts__dmh_sts__agua_dmh__normal__agua_dmh__pstm_nrm_imm__mPa__m_c05s_step2
MemInstrItinData<II_ST_s16_pstm_nrm_imm, [IsPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DMS_WM>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/7, /*ptr*/1, /*imm*/1, /*pe2_ads*/6, /*pe2_ads*/6], MemoryCycles<[5, 11]>>,

// id: me__instr128_or__lda__dmhb_sts__dmb_sts__agua_dmb__normal__agua_dmb__idx__mPa__mDJa
MemInstrItinData<II_ST_s8_idx, [IsPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DMS_WM>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/7, /*ptr*/1, /*dj*/1, /*pe2_ads*/6, /*pe2_ads*/6], MemoryCycles<[5, 11]>>,

// id: me__instr128_or__lda__dmhb_sts__dmb_sts__agua_dmb__normal__agua_dmb__idx_imm__mPa__m_c04s
MemInstrItinData<II_ST_s8_idx_imm, [IsPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DMS_WM>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/7, /*ptr*/1, /*imm*/1, /*pe2_ads*/6, /*pe2_ads*/6], MemoryCycles<[5, 11]>>,

// id: me__instr128_or__lda__dmhb_sts__dmb_sts__agua_dmb__normal__agua_dmb__pstm_nrm__mPa__mMa
MemInstrItinData<II_ST_s8_pstm_nrm, [IsPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DMS_WM>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/7, /*ptr*/1, /*mod*/1, /*pe2_ads*/6, /*pe2_ads*/6], MemoryCycles<[5, 11]>>,

// id: me__instr128_or__lda__dmhb_sts__dmb_sts__agua_dmb__normal__agua_dmb__pstm_nrm_imm__mPa__m_c04s
MemInstrItinData<II_ST_s8_pstm_nrm_imm, [IsPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DMS_WM>, SimpleCycle<L_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/7, /*ptr*/1, /*imm*/1, /*pe2_ads*/6, /*pe2_ads*/6], MemoryCycles<[5, 11]>>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__sub
InstrItinData<II_SUB, [], [/*d0*/1, /*s0*/1, /*s1*/1, /*srCarry*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_abs__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_nX__vaddSign0
InstrItinData<II_VABS_GTZ16_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_abs__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_nX__vaddSign1
InstrItinData<II_VABS_GTZ16_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_abs__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_nX__vaddSign0
InstrItinData<II_VABS_GTZ32_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_abs__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_nX__vaddSign1
InstrItinData<II_VABS_GTZ32_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_abs__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_nX__vaddSign0
InstrItinData<II_VABS_GTZ8_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r17_r16*/2, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*d*/MV_Bypass, /*r17_r16*/NoBypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_abs__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_nX__vaddSign1
InstrItinData<II_VABS_GTZ8_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r17_r16*/2, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*d*/MV_Bypass, /*r17_r16*/NoBypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_vmul_cm_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_vmul_cm_core_X_QY, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_vmul_cm_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_vmul_cm_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_vmul_cm_core_X_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_vmul_cm_core_Y_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_vmul_cm_core_Y_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_vmul_cm_core_Y_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_vmul_cm_core_Y_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
InstrItinData<II_VADDMAC_vaddmac_half_vmul_cm_half_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
InstrItinData<II_VADDMAC_vaddmac_half_vmul_cm_half_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
InstrItinData<II_VADDMAC_vaddmac_half_vmul_cm_half_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_QY, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_QY, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
InstrItinData<II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
InstrItinData<II_VADDMAC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
InstrItinData<II_VADDMAC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
InstrItinData<II_VADDMAC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
InstrItinData<II_VADDMAC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
InstrItinData<II_VADDMAC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
InstrItinData<II_VADDMAC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_vmul_bf_core_X_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<2>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RM_L0_PORT>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_vmul_bf_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RM_L0_PORT>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_vmul_bf_core_Y_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RM_L0_PORT>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_vmul_bf_core_Y_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RM_L0_PORT>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_half_vmul_bf_core_X_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_half_vmul_bf_core_X_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_half_vmul_bf_core_Y_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<2>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_half_vmul_bf_half_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_Y_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_half_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_Y_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
InstrItinData<II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_half_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bfp13__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_reg__vmac_acc2_comp0__add_reg__mDMm__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
InstrItinData<II_VADDMAC_f_vaddmac_bfp13, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, PrefixCycle<X_EXP_ELL>, SimpleCycle<Y_EXP_E>, EmptyCycles<1>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RM_L0_PORT>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/6, /*acc1*/3, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bfp13_scd__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_scd__vmac_acc2_comp0__add_scd__vmac_acc2_comp0__add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_comp0__vmac_scd_dm_dyn__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r_incr__mR31_scd__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
InstrItinData<II_VADDMAC_f_vaddmac_bfp13_scd_add_scd_incr, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, PrefixCycle<X_EXP_ELL>, SimpleCycle<Y_EXP_E>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/6, /*acc2_out*/2, /*acc1*/3, /*acc2*/2, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7, /*crSCDEn*/2], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bfp13_scd__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_scd__vmac_acc2_comp0__add_scd__vmac_acc2_comp0__add_scd_noincr__vmac_scd_dm_reg__vmac_scd_dm_comp0__vmac_scd_dm_reg__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r__mR31_scd__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
InstrItinData<II_VADDMAC_f_vaddmac_bfp13_scd_add_scd_noincr, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, PrefixCycle<X_EXP_ELL>, SimpleCycle<Y_EXP_E>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/6, /*acc1*/3, /*acc2*/2, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7, /*crSCDEn*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bfp16__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
InstrItinData<II_VADDMAC_f_vaddmac_bfp16, [PrefixCycle<FEY_RW_L0_PORT>, InstrStage<2, [VCTRL_IN], 0>, InstrStage<2, [VCTRL_SUB0_CG1U], 0>, InstrStage<2, [X_EGWL], 0>, InstrStage<2, [X_EXP_ELL], 0>, InstrStage<2, [Y_EGY], 1>, PrefixCycle<FEY_RW_L0_PORT>, SimpleCycle<Y_EXP_E>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFSHCH__ACC1>, SimpleCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*s1l*/1, /*s2l*/1, /*s2h*/2, /*s1h*/2, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1l*/NoBypass, /*s2l*/NoBypass, /*s2h*/NoBypass, /*s1h*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bfp16_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
InstrItinData<II_VADDMAC_f_vaddmac_bfp16_scd_add_scd_incr, [PrefixCycle<FEY_RW_L0_PORT>, InstrStage<2, [VCTRL_IN], 0>, InstrStage<2, [VCTRL_SUB0_CG1U], 0>, InstrStage<2, [X_EGWL], 0>, InstrStage<2, [X_EXP_ELL], 0>, InstrStage<2, [Y_EGY], 1>, PrefixCycle<FEY_RW_L0_PORT>, SimpleCycle<Y_EXP_E>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, SimpleCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1l*/1, /*s2l*/1, /*s2h*/2, /*s1h*/2, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1l*/NoBypass, /*s2l*/NoBypass, /*s2h*/NoBypass, /*s1h*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bfp16_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
InstrItinData<II_VADDMAC_f_vaddmac_bfp16_scd_add_scd_noincr, [PrefixCycle<FEY_RW_L0_PORT>, InstrStage<2, [VCTRL_IN], 0>, InstrStage<2, [VCTRL_SUB0_CG1U], 0>, InstrStage<2, [X_EGWL], 0>, InstrStage<2, [X_EXP_ELL], 0>, InstrStage<2, [Y_EGY], 1>, PrefixCycle<FEY_RW_L0_PORT>, SimpleCycle<Y_EXP_E>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, SimpleCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*s1l*/1, /*s2l*/1, /*s2h*/2, /*s1h*/2, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1l*/NoBypass, /*s2l*/NoBypass, /*s2h*/NoBypass, /*s1h*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_vmul_cm_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_vmul_cm_core_X_QY, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_vmul_cm_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_vmul_cm_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_vmul_cm_core_X_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_vmul_cm_core_Y_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_vmul_cm_core_Y_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_vmul_cm_core_Y_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_vmul_cm_core_Y_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
InstrItinData<II_VADDMSC_vaddmac_half_vmul_cm_half_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
InstrItinData<II_VADDMSC_vaddmac_half_vmul_cm_half_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
InstrItinData<II_VADDMSC_vaddmac_half_vmul_cm_half_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_QY, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_QY, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
InstrItinData<II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
InstrItinData<II_VADDMSC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
InstrItinData<II_VADDMSC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
InstrItinData<II_VADDMSC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
InstrItinData<II_VADDMSC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
InstrItinData<II_VADDMSC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
InstrItinData<II_VADDMSC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_vmul_bf_core_X_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<2>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RM_L0_PORT>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_vmul_bf_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RM_L0_PORT>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_vmul_bf_core_Y_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RM_L0_PORT>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_vmul_bf_core_Y_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RM_L0_PORT>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_half_vmul_bf_core_X_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_half_vmul_bf_core_X_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_half_vmul_bf_core_Y_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<2>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_half_vmul_bf_half_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_Y_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_half_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_Y_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
InstrItinData<II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_half_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bfp13__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_reg__vmac_acc2_comp0__add_reg__mDMm__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
InstrItinData<II_VADDMSC_f_vaddmac_bfp13, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, PrefixCycle<X_EXP_ELL>, SimpleCycle<Y_EXP_E>, EmptyCycles<1>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RM_L0_PORT>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/6, /*acc1*/3, /*acc2*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bfp13_scd__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_scd__vmac_acc2_comp0__add_scd__vmac_acc2_comp0__add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_comp0__vmac_scd_dm_dyn__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r_incr__mR31_scd__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
InstrItinData<II_VADDMSC_f_vaddmac_bfp13_scd_add_scd_incr, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, PrefixCycle<X_EXP_ELL>, SimpleCycle<Y_EXP_E>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/6, /*acc2_out*/2, /*acc1*/3, /*acc2*/2, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7, /*crSCDEn*/2], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bfp13_scd__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_scd__vmac_acc2_comp0__add_scd__vmac_acc2_comp0__add_scd_noincr__vmac_scd_dm_reg__vmac_scd_dm_comp0__vmac_scd_dm_reg__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r__mR31_scd__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
InstrItinData<II_VADDMSC_f_vaddmac_bfp13_scd_add_scd_noincr, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, PrefixCycle<X_EXP_ELL>, SimpleCycle<Y_EXP_E>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/6, /*acc1*/3, /*acc2*/2, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7, /*crSCDEn*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bfp16__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
InstrItinData<II_VADDMSC_f_vaddmac_bfp16, [PrefixCycle<FEY_RW_L0_PORT>, InstrStage<2, [VCTRL_IN], 0>, InstrStage<2, [VCTRL_SUB0_CG1U], 0>, InstrStage<2, [X_EGWL], 0>, InstrStage<2, [X_EXP_ELL], 0>, InstrStage<2, [Y_EGY], 1>, PrefixCycle<FEY_RW_L0_PORT>, SimpleCycle<Y_EXP_E>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFSHCH__ACC1>, SimpleCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*s1l*/1, /*s2l*/1, /*s2h*/2, /*s1h*/2, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*s1l*/NoBypass, /*s2l*/NoBypass, /*s2h*/NoBypass, /*s1h*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bfp16_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
InstrItinData<II_VADDMSC_f_vaddmac_bfp16_scd_add_scd_incr, [PrefixCycle<FEY_RW_L0_PORT>, InstrStage<2, [VCTRL_IN], 0>, InstrStage<2, [VCTRL_SUB0_CG1U], 0>, InstrStage<2, [X_EGWL], 0>, InstrStage<2, [X_EXP_ELL], 0>, InstrStage<2, [Y_EGY], 1>, PrefixCycle<FEY_RW_L0_PORT>, SimpleCycle<Y_EXP_E>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, SimpleCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*s1l*/1, /*s2l*/1, /*s2h*/2, /*s1h*/2, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1l*/NoBypass, /*s2l*/NoBypass, /*s2h*/NoBypass, /*s1h*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vaddmac_bfp16_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
InstrItinData<II_VADDMSC_f_vaddmac_bfp16_scd_add_scd_noincr, [PrefixCycle<FEY_RW_L0_PORT>, InstrStage<2, [VCTRL_IN], 0>, InstrStage<2, [VCTRL_SUB0_CG1U], 0>, InstrStage<2, [X_EGWL], 0>, InstrStage<2, [X_EXP_ELL], 0>, InstrStage<2, [Y_EGY], 1>, PrefixCycle<FEY_RW_L0_PORT>, SimpleCycle<Y_EXP_E>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, PrefixCycle<PSA_SUMH__A>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, SimpleCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*s1l*/1, /*s2l*/1, /*s2h*/2, /*s1h*/2, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*s1l*/NoBypass, /*s2l*/NoBypass, /*s2h*/NoBypass, /*s1h*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vaddsub__vec_mask_in__vec_mask_in32__vec_add_mX__vec_add_nX
InstrItinData<II_VADDSUB_16, [SimpleCycle<CRBF8CONF_RV>], [/*d*/2, /*s1*/1, /*s2*/1, /*sel*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass, /*sel*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vaddsub__vec_mask_in__vec_mask_in16__vec_add_mX__vec_add_nX
InstrItinData<II_VADDSUB_32, [SimpleCycle<CRBF8CONF_RV>], [/*d*/2, /*s1*/1, /*s2*/1, /*sel*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass, /*sel*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vaddsub__vec_mask_in__vec_mask_in64__mLm__vec_add_mX__vec_add_nX
InstrItinData<II_VADDSUB_8, [SimpleCycle<CRBF8CONF_RV>], [/*d*/2, /*s1*/1, /*s2*/1, /*sel*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass, /*sel*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add__vadd__vec_size__vec_size32__vec_add_mX__vec_add_nX
InstrItinData<II_VADD_16, [SimpleCycle<CRBF8CONF_RV>], [/*d*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add__vadd__vec_size__vec_size16__vec_add_mX__vec_add_nX
InstrItinData<II_VADD_32, [SimpleCycle<CRBF8CONF_RV>], [/*d*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add__vadd__vec_size__vec_size64__vec_add_mX__vec_add_nX
InstrItinData<II_VADD_8, [SimpleCycle<CRBF8CONF_RV>], [/*d*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__vec__vacc__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vacc_cm_core
InstrItinData<II_VADD_vacc, [SimpleCycle<VCTRL_IN>, EmptyCycles<2>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vacc_cm_half_core
InstrItinData<II_VADD_vacc_half, [SimpleCycle<VCTRL_IN>, EmptyCycles<2>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vacc_cm_core
InstrItinData<II_VADD_vacc_scd_add_scd_incr, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vacc_cm_core
InstrItinData<II_VADD_vacc_scd_add_scd_noincr, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vacc_cm_half_core
InstrItinData<II_VADD_vacc_scd_half_add_scd_incr_half, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vacc_cm_half_core
InstrItinData<II_VADD_vacc_scd_half_add_scd_noincr_half, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_fp__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vacc_bf_core
InstrItinData<II_VADD_f_vacc_fp, [SimpleCycle<VCTRL_IN>, EmptyCycles<2>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_fp_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vacc_bf_core
InstrItinData<II_VADD_f_vacc_fp_scd_add_scd_incr, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_fp_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vacc_bf_core
InstrItinData<II_VADD_f_vacc_fp_scd_add_scd_noincr, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_half_fp__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vacc_bf_half_core
InstrItinData<II_VADD_f_vacc_half_fp, [SimpleCycle<VCTRL_IN>, EmptyCycles<2>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_half_fp_scd__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vacc_bf_half_core
InstrItinData<II_VADD_f_vacc_half_fp_scd_add_scd_incr_half, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_half_fp_scd__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vacc_bf_half_core
InstrItinData<II_VADD_f_vacc_half_fp_scd_add_scd_noincr_half, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_fp_bfp13__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_reg__vmac_acc2_comp0__add_reg__mDMm__vacc_bfp13_core
InstrItinData<II_VADD_mx6_vacc_fp_bfp13, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/6, /*acc1*/3, /*acc2*/3, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_fp_bfp13_scd__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_scd__vmac_acc2_comp0__add_scd__vmac_acc2_comp0__add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_comp0__vmac_scd_dm_dyn__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r_incr__mR31_scd__vacc_bfp13_core
InstrItinData<II_VADD_mx6_vacc_fp_bfp13_scd_add_scd_incr, [SimpleCycle<VCTRL_IN>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/6, /*acc2_out*/2, /*acc1*/3, /*acc2*/2, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7, /*crSCDEn*/2], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_fp_bfp13_scd__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_scd__vmac_acc2_comp0__add_scd__vmac_acc2_comp0__add_scd_noincr__vmac_scd_dm_reg__vmac_scd_dm_comp0__vmac_scd_dm_reg__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r__mR31_scd__vacc_bfp13_core
InstrItinData<II_VADD_mx6_vacc_fp_bfp13_scd_add_scd_noincr, [SimpleCycle<VCTRL_IN>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/6, /*acc1*/3, /*acc2*/2, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7, /*crSCDEn*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_half_fp_bfp13__mCMLv__vmac_acc1_add_half__vmac_acc1_comp0__add_half__mCMLv__vmac_acc2_add_reg_half__vmac_acc2_comp0__add_reg_half__mCMLm__vacc_half_bfp13_core
InstrItinData<II_VADD_mx6_vacc_half_fp_bfp13, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/6, /*acc1*/3, /*acc2*/3, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_half_fp_bfp13_scd__mCMLv__vmac_acc1_add_half__vmac_acc1_comp0__add_half__mCMLv__vmac_acc2_add_scd_half__vmac_acc2_comp0__add_scd_half__vmac_acc2_comp0__add_scd_incr_half__vmac_scd_cml_dyn__vmac_scd_dm_comp0__vmac_scd_cml_dyn__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r_incr__mR31_scd__vacc_half_bfp13_core
InstrItinData<II_VADD_mx6_vacc_half_fp_bfp13_scd_add_scd_incr_half, [SimpleCycle<VCTRL_IN>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/6, /*acc2_out*/2, /*acc1*/3, /*acc2*/2, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7, /*crSCDEn*/2], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_half_fp_bfp13_scd__mCMLv__vmac_acc1_add_half__vmac_acc1_comp0__add_half__mCMLv__vmac_acc2_add_scd_half__vmac_acc2_comp0__add_scd_half__vmac_acc2_comp0__add_scd_noincr_half__vmac_scd_cml_reg__vmac_scd_dm_comp0__vmac_scd_cml_reg__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r__mR31_scd__vacc_half_bfp13_core
InstrItinData<II_VADD_mx6_vacc_half_fp_bfp13_scd_add_scd_noincr_half, [SimpleCycle<VCTRL_IN>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/6, /*acc1*/3, /*acc2*/2, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7, /*crSCDEn*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_log__vband__vec_add_mX__vec_add_nX__vec_size16
InstrItinData<II_VBAND, [SimpleCycle<CRBF8CONF_RV>], [/*d*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_bm__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_R__s2v_wide_16__mR29_insert
InstrItinData<II_VBCSTSHFL_16_vec_broadcast_shuffle_bm, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1, /*r29*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_x__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_R__s2v_wide_16__mR29_insert
InstrItinData<II_VBCSTSHFL_16_vec_broadcast_shuffle_x, [], [/*dst*/2, /*src*/1, /*r29*/1], [/*dst*/MV_Bypass, /*src*/NoBypass, /*r29*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_bm__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_R__s2v_wide_32__mR29_insert
InstrItinData<II_VBCSTSHFL_32_vec_broadcast_shuffle_bm, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1, /*r29*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_x__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_R__s2v_wide_32__mR29_insert
InstrItinData<II_VBCSTSHFL_32_vec_broadcast_shuffle_x, [], [/*dst*/2, /*src*/1, /*r29*/1], [/*dst*/MV_Bypass, /*src*/NoBypass, /*r29*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_bm__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_L__s2v_wide_64__mLm__mR29_insert
InstrItinData<II_VBCSTSHFL_64_vec_broadcast_shuffle_bm, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1, /*r29*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_x__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_L__s2v_wide_64__mLm__mR29_insert
InstrItinData<II_VBCSTSHFL_64_vec_broadcast_shuffle_x, [], [/*dst*/2, /*src*/1, /*r29*/1], [/*dst*/MV_Bypass, /*src*/NoBypass, /*r29*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_bm__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_R__s2v_wide_8__mR29_insert
InstrItinData<II_VBCSTSHFL_8_vec_broadcast_shuffle_bm, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1, /*r29*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_x__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_R__s2v_wide_8__mR29_insert
InstrItinData<II_VBCSTSHFL_8_vec_broadcast_shuffle_x, [], [/*dst*/2, /*src*/1, /*r29*/1], [/*dst*/MV_Bypass, /*src*/NoBypass, /*r29*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast__vec_insert_src__vec_insert_src_R__s2v_wide_16__mMvXDst
InstrItinData<II_VBCST_16, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast__vec_insert_src__vec_insert_src_R__s2v_wide_32__mMvXDst
InstrItinData<II_VBCST_32, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast__vec_insert_src__vec_insert_src_L__s2v_wide_64__mLm__mMvXDst
InstrItinData<II_VBCST_64, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast__vec_insert_src__vec_insert_src_R__s2v_wide_8__mMvXDst
InstrItinData<II_VBCST_8, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_not__vbneg_ltz__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_nX
InstrItinData<II_VBNEG_LTZ_s16, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_not__vbneg_ltz__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_nX
InstrItinData<II_VBNEG_LTZ_s32, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_not__vbneg_ltz__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_nX
InstrItinData<II_VBNEG_LTZ_s8, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r17_r16*/2, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*r17_r16*/NoBypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_log__vbor__vec_add_mX__vec_add_nX__vec_size16
InstrItinData<II_VBOR, [SimpleCycle<CRBF8CONF_RV>], [/*d*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__vec__vclr__mDMv
InstrItinData<II_VCLR_vclr, [SimpleCycle<VCTRL_IN>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, SimpleCycle<DM_RV_L0>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6], [/*dst*/VEC_Bypass]>,

// id: me__instr128_or__vec__vclr_half__mCMLv
InstrItinData<II_VCLR_vclr_half, [SimpleCycle<VCTRL_IN>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, SimpleCycle<ZACC_DML>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6], [/*dst*/VEC_Bypass]>,

// id: me__instr128_or__vec__vclr_fp__mDMv
InstrItinData<II_VCLR_f_vclr_fp, [SimpleCycle<VCTRL_IN>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, SimpleCycle<DM_RV_L0>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7], [/*dst*/VEC_Bypass]>,

// id: me__instr128_or__vec__vclr_half_fp__mCMLv
InstrItinData<II_VCLR_f_vclr_half_fp, [SimpleCycle<VCTRL_IN>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7], [/*dst*/VEC_Bypass]>,

// id: me__instr128_or__st__mv_conv__mv_w_srs_bf16__srs_f__mW
InstrItinData<II_VCONV_bf16_fp32_mv_conv_mv_w_srs_bf16, [EmptyCycles<2>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/3, /*src*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2]>,

// id: me__instr128_or__st__mv_conv__mv_x_srs_bf16__srs_f__mX
InstrItinData<II_VCONV_bf16_fp32_mv_conv_mv_x_srs_bf16, [EmptyCycles<2>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/3, /*src*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2]>,

// id: me__instr128_or__st__mv_mcd_bf16_cm__srs_f__mX__mMCD_E3
InstrItinData<II_VCONV_bf16_fp32_mv_mcd_bf16_cm, [EmptyCycles<1>, SimpleCycle<CRMCDEN_RS>], [/*src*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crMCDEn*/2, /*crRnd*/2]>,

// id: me__instr128_or__st__mv_w_srs_bf8__srs_f__mXl__mSs
InstrItinData<II_VCONV_bf8_fp32, [EmptyCycles<2>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/3, /*src*/1, /*i*/1, /*srF2FFlags*/3, /*crBF8conf*/1, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2]>,

// id: me__instr128_or__st__mv_mcd_fp16_cm__srs_f__mX__mMCD_E3
InstrItinData<II_VCONV_fp16_fp32_mv_mcd_fp16_cm, [EmptyCycles<1>, SimpleCycle<CRMCDEN_RS>], [/*src*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crMCDEn*/2, /*crRnd*/2]>,

// id: me__instr128_or__st__mv_x_srs_fp16__srs_f__mX
InstrItinData<II_VCONV_fp16_fp32_mv_x_srs_fp16, [EmptyCycles<2>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/3, /*src*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_bf_cm_slotX__mv_scd_bf_cm__ups_f__double__fp32_from_mode_bf16__mv_scd_E2__short__mSCD_E2__mSCD_comp_E2__mSCD
InstrItinData<II_VCONV_fp32_bf16_alu, [SimpleCycle<SCD_CTL>, PrefixCycle<DM_WA_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<UPS_F__A>], [/*dst*/2, /*crSCDEn*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_w_ups_bf16
InstrItinData<II_VCONV_fp32_bf16_mv_mv_w_ups_bf16, [EmptyCycles<1>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A_L>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_x_ups_f16__ups_f__double__fp32_from_mode_bf16
InstrItinData<II_VCONV_fp32_bf16_mv_mv_x_ups_f16, [EmptyCycles<1>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_bf_cm_slotX__mv_scd_bf_cm__ups_f__double__fp32_from_mode_fp16__mv_scd_E2__short__mSCD_E2__mSCD_comp_E2__mSCD
InstrItinData<II_VCONV_fp32_fp16_alu, [SimpleCycle<SCD_CTL>, PrefixCycle<DM_WA_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<UPS_F__A>], [/*dst*/2, /*crSCDEn*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_x_ups_f16__ups_f__double__fp32_from_mode_fp16
InstrItinData<II_VCONV_fp32_fp16_mv, [EmptyCycles<1>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__st__mv_w_srs_fp8__srs_f__mXl__mSs
InstrItinData<II_VCONV_fp8_fp32, [EmptyCycles<2>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/3, /*src*/1, /*i*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFP8conf*/1, /*crFPConvSat*/2, /*crRnd*/2]>,

// id: me__instr128_or__st__mv_conv__mv_fp32_to_bfp11__bfp__mFp2Bfp11__bfp__basis__mDMs
InstrItinData<II_VCONV_mx4_fp32, [EmptyCycles<3>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/4, /*src*/1, /*srF2BFlags*/3, /*crF2BMask*/3, /*crRnd*/2]>,

// id: me__instr128_or__st__mv_conv__mv_fp32_to_bfp13__bfp__mFp2Bfp13__bfp__basis__mDMs
InstrItinData<II_VCONV_mx6_fp32, [EmptyCycles<3>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/4, /*src*/1, /*srF2BFlags*/3, /*crF2BMask*/3, /*crRnd*/2]>,

// id: me__instr128_or__st__mv_conv__mv_fp32_to_bfp16__bfp__mFp2Bfp16__bfp__basis__mDMs
InstrItinData<II_VCONV_mx9_fp32, [EmptyCycles<3>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/4, /*src*/1, /*srF2BFlags*/3, /*crF2BMask*/3, /*crRnd*/2]>,

// id: me__instr128_or__st__mv_conv__mv_fp32_to_int_bm__bf16_to_int_E2__mv__srs_f__mW__mSs
InstrItinData<II_VCONV_s32_bf16_fp32, [EmptyCycles<2>, PrefixCycle<F2I__MASK>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/3, /*src*/1, /*shft*/1, /*srF2FFlags*/3, /*srF2IFlags*/4, /*crF2FMask*/2, /*crF2IMask*/3, /*crFPConvSat*/2, /*crRnd*/2]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_eqz__vec_cmp_out__vec_cmp_out32__vec_add_nX
InstrItinData<II_VEQZ_16, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*cmp*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_eqz__vec_cmp_out__vec_cmp_out16__vec_add_nX
InstrItinData<II_VEQZ_32, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*cmp*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_eqz__vec_cmp_out__vec_cmp_out64__mLm__vec_add_nX
InstrItinData<II_VEQZ_8, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*cmp*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_nlf_16
InstrItinData<II_VEXP2_bf16, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_nlf_32
InstrItinData<II_VEXP2_fp32, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_shuffle__mR29_insert
InstrItinData<II_VEXTBCSTSHFL_128, [], [/*dst*/2, /*s1*/1, /*idx*/1, /*r29*/1], [/*dst*/MV_Bypass, /*s1*/NoBypass, /*idx*/NoBypass, /*r29*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_shuffle__s2v_wide2_16__mR29_insert
InstrItinData<II_VEXTBCSTSHFL_16, [], [/*dst*/2, /*s1*/1, /*idx*/1, /*r29*/1], [/*dst*/MV_Bypass, /*s1*/NoBypass, /*idx*/NoBypass, /*r29*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_shuffle__s2v_wide2_32__mR29_insert
InstrItinData<II_VEXTBCSTSHFL_32, [], [/*dst*/2, /*s1*/1, /*idx*/1, /*r29*/1], [/*dst*/MV_Bypass, /*s1*/NoBypass, /*idx*/NoBypass, /*r29*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_shuffle__s2v_wide2_64__mR29_insert
InstrItinData<II_VEXTBCSTSHFL_64, [], [/*dst*/2, /*s1*/1, /*idx*/1, /*r29*/1], [/*dst*/MV_Bypass, /*s1*/NoBypass, /*idx*/NoBypass, /*r29*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_imm__vec_extract_broadcast_core__s2v_wide1_128__mMvXDst
InstrItinData<II_VEXTBCST_128_vec_extract_broadcast_imm, [], [/*dst*/2, /*s1*/1, /*idx*/1], [/*dst*/MV_Bypass, /*s1*/NoBypass, /*idx*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_r__vec_extract_broadcast_core__s2v_wide1_128__mMvXDst
InstrItinData<II_VEXTBCST_128_vec_extract_broadcast_r, [], [/*dst*/2, /*s1*/1, /*idx*/1], [/*dst*/MV_Bypass, /*s1*/NoBypass, /*idx*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_imm__vec_extract_broadcast_core__s2v_wide1_16__mMvXDst
InstrItinData<II_VEXTBCST_16_vec_extract_broadcast_imm, [], [/*dst*/2, /*s1*/1, /*idx*/1], [/*dst*/MV_Bypass, /*s1*/NoBypass, /*idx*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_r__vec_extract_broadcast_core__s2v_wide1_16__mMvXDst
InstrItinData<II_VEXTBCST_16_vec_extract_broadcast_r, [], [/*dst*/2, /*s1*/1, /*idx*/1], [/*dst*/MV_Bypass, /*s1*/NoBypass, /*idx*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_imm__vec_extract_broadcast_core__s2v_wide1_32__mMvXDst
InstrItinData<II_VEXTBCST_32_vec_extract_broadcast_imm, [], [/*dst*/2, /*s1*/1, /*idx*/1], [/*dst*/MV_Bypass, /*s1*/NoBypass, /*idx*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_r__vec_extract_broadcast_core__s2v_wide1_32__mMvXDst
InstrItinData<II_VEXTBCST_32_vec_extract_broadcast_r, [], [/*dst*/2, /*s1*/1, /*idx*/1], [/*dst*/MV_Bypass, /*s1*/NoBypass, /*idx*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_imm__vec_extract_broadcast_core__s2v_wide1_64__mMvXDst
InstrItinData<II_VEXTBCST_64_vec_extract_broadcast_imm, [], [/*dst*/2, /*s1*/1, /*idx*/1], [/*dst*/MV_Bypass, /*s1*/NoBypass, /*idx*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_r__vec_extract_broadcast_core__s2v_wide1_64__mMvXDst
InstrItinData<II_VEXTBCST_64_vec_extract_broadcast_r, [], [/*dst*/2, /*s1*/1, /*idx*/1], [/*dst*/MV_Bypass, /*s1*/NoBypass, /*idx*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_imm__vec_extract_broadcast_core__s2v_wide1_8__mMvXDst
InstrItinData<II_VEXTBCST_8_vec_extract_broadcast_imm, [], [/*dst*/2, /*s1*/1, /*idx*/1], [/*dst*/MV_Bypass, /*s1*/NoBypass, /*idx*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_r__vec_extract_broadcast_core__s2v_wide1_8__mMvXDst
InstrItinData<II_VEXTBCST_8_vec_extract_broadcast_r, [], [/*dst*/2, /*s1*/1, /*idx*/1], [/*dst*/MV_Bypass, /*s1*/NoBypass, /*idx*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_16__vaddSign0__mIdxImm
InstrItinData<II_VEXTRACT_16_vec_extract_imm_vaddSign0, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*s1*/1, /*idx*/1, /*vaddSign0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_16__vaddSign1__mIdxImm
InstrItinData<II_VEXTRACT_16_vec_extract_imm_vaddSign1, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*s1*/1, /*idx*/1, /*vaddSign1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_16__vaddSign0
InstrItinData<II_VEXTRACT_16_vec_extract_r_vaddSign0, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*s1*/1, /*idx*/1, /*vaddSign0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_16__vaddSign1
InstrItinData<II_VEXTRACT_16_vec_extract_r_vaddSign1, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*s1*/1, /*idx*/1, /*vaddSign1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_32__vaddSign0__mIdxImm
InstrItinData<II_VEXTRACT_32_vec_extract_imm_vaddSign0, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*s1*/1, /*idx*/1, /*vaddSign0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_32__vaddSign1__mIdxImm
InstrItinData<II_VEXTRACT_32_vec_extract_imm_vaddSign1, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*s1*/1, /*idx*/1, /*vaddSign1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_32__vaddSign0
InstrItinData<II_VEXTRACT_32_vec_extract_r_vaddSign0, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*s1*/1, /*idx*/1, /*vaddSign0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_32__vaddSign1
InstrItinData<II_VEXTRACT_32_vec_extract_r_vaddSign1, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*s1*/1, /*idx*/1, /*vaddSign1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_L__s2v_wide_64__mLm__vaddSign0__mIdxImm
InstrItinData<II_VEXTRACT_64_vec_extract_imm_vaddSign0, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*s1*/1, /*idx*/1, /*vaddSign0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_L__s2v_wide_64__mLm__vaddSign1__mIdxImm
InstrItinData<II_VEXTRACT_64_vec_extract_imm_vaddSign1, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*s1*/1, /*idx*/1, /*vaddSign1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_L__s2v_wide_64__mLm__vaddSign0
InstrItinData<II_VEXTRACT_64_vec_extract_r_vaddSign0, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*s1*/1, /*idx*/1, /*vaddSign0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_L__s2v_wide_64__mLm__vaddSign1
InstrItinData<II_VEXTRACT_64_vec_extract_r_vaddSign1, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*s1*/1, /*idx*/1, /*vaddSign1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_8__vaddSign0__mIdxImm
InstrItinData<II_VEXTRACT_8_vec_extract_imm_vaddSign0, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*s1*/1, /*idx*/1, /*vaddSign0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_8__vaddSign1__mIdxImm
InstrItinData<II_VEXTRACT_8_vec_extract_imm_vaddSign1, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*s1*/1, /*idx*/1, /*vaddSign1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_8__vaddSign0
InstrItinData<II_VEXTRACT_8_vec_extract_r_vaddSign0, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*s1*/1, /*idx*/1, /*vaddSign0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_8__vaddSign1
InstrItinData<II_VEXTRACT_8_vec_extract_r_vaddSign1, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*s1*/1, /*idx*/1, /*vaddSign1*/1]>,

// id: me__instr128_or__st__mv_conv__mv_bf16_to_int_w__bf16_to_int_E1__mv__mSs
InstrItinData<II_VFLOOR_s32_bf16, [SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<F2I__MASK>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/2, /*src*/1, /*shft*/1, /*srF2IFlags*/3, /*crF2IMask*/2]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out32__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VGE_16_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out32__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VGE_16_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out16__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VGE_32_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out16__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VGE_32_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VGE_8_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VGE_8_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f16_compare__vge_bf16__vec_cmp_out32__vec_add_mX__vec_add_nX
InstrItinData<II_VGE_bf16, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_compare__vge_bf8__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX
InstrItinData<II_VGE_bf8, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f16_compare__vge_fp16__vec_cmp_out32__vec_add_mX__vec_add_nX
InstrItinData<II_VGE_fp16, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_compare__vge_fp8__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX
InstrItinData<II_VGE_fp8, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_R__s2v_wide_16__mInsertIdxRed__mIdxImm0
InstrItinData<II_VINSERT_16_mIdxImm0, [], [/*dst*/2, /*s1*/1, /*src*/1], [/*dst*/MV_Bypass, /*s1*/MV_Bypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_R__s2v_wide_16__mInsertIdxRed__mR29_insert
InstrItinData<II_VINSERT_16_mR29_insert, [], [/*dst*/2, /*s1*/1, /*r29*/1, /*src*/1], [/*dst*/MV_Bypass, /*s1*/MV_Bypass, /*r29*/NoBypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_R__s2v_wide_32__mInsertIdxRed__mIdxImm0
InstrItinData<II_VINSERT_32_mIdxImm0, [], [/*dst*/2, /*s1*/1, /*src*/1], [/*dst*/MV_Bypass, /*s1*/MV_Bypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_R__s2v_wide_32__mInsertIdxRed__mR29_insert
InstrItinData<II_VINSERT_32_mR29_insert, [], [/*dst*/2, /*s1*/1, /*r29*/1, /*src*/1], [/*dst*/MV_Bypass, /*s1*/MV_Bypass, /*r29*/NoBypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_L__s2v_wide_64__mLm__mInsertIdxRed__mIdxImm0
InstrItinData<II_VINSERT_64_mIdxImm0, [], [/*dst*/2, /*s1*/1, /*src*/1], [/*dst*/MV_Bypass, /*s1*/MV_Bypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_L__s2v_wide_64__mLm__mInsertIdxRed__mR29_insert
InstrItinData<II_VINSERT_64_mR29_insert, [], [/*dst*/2, /*s1*/1, /*r29*/1, /*src*/1], [/*dst*/MV_Bypass, /*s1*/MV_Bypass, /*r29*/NoBypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_R__s2v_wide_8__mInsertIdxRed__mIdxImm0
InstrItinData<II_VINSERT_8_mIdxImm0, [], [/*dst*/2, /*s1*/1, /*src*/1], [/*dst*/MV_Bypass, /*s1*/MV_Bypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_R__s2v_wide_8__mInsertIdxRed__mR29_insert
InstrItinData<II_VINSERT_8_mR29_insert, [], [/*dst*/2, /*s1*/1, /*r29*/1, /*src*/1], [/*dst*/MV_Bypass, /*s1*/MV_Bypass, /*r29*/NoBypass, /*src*/NoBypass]>,

// id: me__instr128_or__lda__dmv_lda_w__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx__mPa__mDJa
MemInstrItinData<II_VLDA_128_dmv_lda_w_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_w__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx_imm__mPa__m_c08s_step16
MemInstrItinData<II_VLDA_128_dmv_lda_w_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_w__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm__mPa__mMa
MemInstrItinData<II_VLDA_128_dmv_lda_w_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_w__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm_imm__mPa__m_c08s_step16
MemInstrItinData<II_VLDA_128_dmv_lda_w_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_w_spill__agua_dmv__nrm_spill__agua_dmv__spill__mSPa__m_c14n_step16
MemInstrItinData<II_VLDA_128_dmv_lda_w_spill, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_w__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_2d__mPa__mDa
MemInstrItinData<II_VLDA_2D_128, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_bf16__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa__dmw_lda_ups_bf16_core
MemInstrItinData<II_VLDA_2D_CONV_fp32_bf16_dmw_lda_ups_bf16, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A_L>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_bf16
MemInstrItinData<II_VLDA_2D_CONV_fp32_bf16_dmx_lda_ups_f16, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_fp16
MemInstrItinData<II_VLDA_2D_CONV_fp32_fp16, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__KL>], [/*dst*/9, /*ptr_out*/1, /*dc*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__KL>], [/*dst*/9, /*ptr_out*/1, /*dc*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, PrefixCycle<UPS__H__OMUXL>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*ptr_out*/1, /*dc*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, PrefixCycle<UPS__H__OMUXL>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*ptr_out*/1, /*dc*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, SimpleCycle<CRUPSMODE_RA>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*ptr_out*/1, /*dc*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, SimpleCycle<CRUPSMODE_RA>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*ptr_out*/1, /*dc*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, SimpleCycle<UPS__OMUX>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__K>], [/*dst*/9, /*ptr_out*/1, /*dc*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, SimpleCycle<UPS__OMUX>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__K>], [/*dst*/9, /*ptr_out*/1, /*dc*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_feg2__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa
MemInstrItinData<II_VLDA_2D_dmw_lda_feg2, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_w__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa
MemInstrItinData<II_VLDA_2D_dmw_lda_w, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_bm__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa
MemInstrItinData<II_VLDA_2D_dmx_lda_bm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DM_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_x__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa
MemInstrItinData<II_VLDA_2D_dmx_lda_x, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmv_lda_w__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_3d__mPa__mDSa
MemInstrItinData<II_VLDA_3D_128, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_bf16__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa__dmw_lda_ups_bf16_core
MemInstrItinData<II_VLDA_3D_CONV_fp32_bf16_dmw_lda_ups_bf16, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A_L>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_bf16
MemInstrItinData<II_VLDA_3D_CONV_fp32_bf16_dmx_lda_ups_f16, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_fp16
MemInstrItinData<II_VLDA_3D_CONV_fp32_fp16, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__KL>], [/*dst*/9, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__KL>], [/*dst*/9, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, PrefixCycle<UPS__H__OMUXL>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, PrefixCycle<UPS__H__OMUXL>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, SimpleCycle<CRUPSMODE_RA>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, SimpleCycle<CRUPSMODE_RA>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, SimpleCycle<UPS__OMUX>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__K>], [/*dst*/9, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, SimpleCycle<UPS__OMUX>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__K>], [/*dst*/9, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_feg2__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa
MemInstrItinData<II_VLDA_3D_dmw_lda_feg2, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_w__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa
MemInstrItinData<II_VLDA_3D_dmw_lda_w, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_bm__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa
MemInstrItinData<II_VLDA_3D_dmx_lda_bm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DM_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_x__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa
MemInstrItinData<II_VLDA_3D_dmx_lda_x, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_bf16__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa__dmw_lda_ups_bf16_core
MemInstrItinData<II_VLDA_CONV_fp32_bf16_dmw_lda_ups_bf16_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A_L>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_bf16__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32__dmw_lda_ups_bf16_core
MemInstrItinData<II_VLDA_CONV_fp32_bf16_dmw_lda_ups_bf16_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A_L>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_bf16__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa__dmw_lda_ups_bf16_core
MemInstrItinData<II_VLDA_CONV_fp32_bf16_dmw_lda_ups_bf16_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A_L>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_bf16__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32__dmw_lda_ups_bf16_core
MemInstrItinData<II_VLDA_CONV_fp32_bf16_dmw_lda_ups_bf16_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A_L>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_bf16
MemInstrItinData<II_VLDA_CONV_fp32_bf16_dmx_lda_ups_f16_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_bf16
MemInstrItinData<II_VLDA_CONV_fp32_bf16_dmx_lda_ups_f16_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_bf16
MemInstrItinData<II_VLDA_CONV_fp32_bf16_dmx_lda_ups_f16_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_bf16
MemInstrItinData<II_VLDA_CONV_fp32_bf16_dmx_lda_ups_f16_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_fp16
MemInstrItinData<II_VLDA_CONV_fp32_fp16_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_fp16
MemInstrItinData<II_VLDA_CONV_fp32_fp16_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_fp16
MemInstrItinData<II_VLDA_CONV_fp32_fp16_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_fp16
MemInstrItinData<II_VLDA_CONV_fp32_fp16_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<DM_WA_L0_PORT>, SimpleCycle<UPS_F__A>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_x__normal_fill__dmx_lda_fifo_x__core_fill__dmx_lda_fifo_x__normal__mPfa__mLdFifo_a
MemInstrItinData<II_VLDA_FILL, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>], [/*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_ex_bfp16__fifo_2d_pop__dmx_lda_fifo_ex_bfp16__core_pop__dmx_lda_fifo_ex_bfp16__fifo_2d__mPfa__mLdFifo_a__mDa
MemInstrItinData<II_VLDA_POP_2D_dmx_lda_fifo_ex_bfp16, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dc*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_fex_bfp13__fifo_2d_pop__dmx_lda_fifo_fex_bfp13__core_pop__dmx_lda_fifo_fex_bfp13__fifo_2d__mPfa__mLdFifo_a__mDa
MemInstrItinData<II_VLDA_POP_2D_dmx_lda_fifo_fex_bfp13, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dc*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_x__fifo_2d_pop__dmx_lda_fifo_x__core_pop__dmx_lda_fifo_x__fifo_2d__mPfa__mLdFifo_a__mDa
MemInstrItinData<II_VLDA_POP_2D_dmx_lda_fifo_x, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dc*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_ex_bfp16__fifo_3d_pop__dmx_lda_fifo_ex_bfp16__core_pop__dmx_lda_fifo_ex_bfp16__fifo_3d__mPfa__mLdFifo_a__mDSa
MemInstrItinData<II_VLDA_POP_3D_dmx_lda_fifo_ex_bfp16, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_fex_bfp13__fifo_3d_pop__dmx_lda_fifo_fex_bfp13__core_pop__dmx_lda_fifo_fex_bfp13__fifo_3d__mPfa__mLdFifo_a__mDSa
MemInstrItinData<II_VLDA_POP_3D_dmx_lda_fifo_fex_bfp13, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_x__fifo_3d_pop__dmx_lda_fifo_x__core_pop__dmx_lda_fifo_x__fifo_3d__mPfa__mLdFifo_a__mDSa
MemInstrItinData<II_VLDA_POP_3D_dmx_lda_fifo_x, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_ex_bfp16__fifo_1d_pop__dmx_lda_fifo_ex_bfp16__core_pop__dmx_lda_fifo_ex_bfp16__fifo_1d__mPfa__mLdFifo_a__mMa
MemInstrItinData<II_VLDA_POP_dmx_lda_fifo_ex_bfp16_fifo_1d_pop, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_ex_bfp16__normal_pop__dmx_lda_fifo_ex_bfp16__core_pop__dmx_lda_fifo_ex_bfp16__normal__mPfa__mLdFifo_a
MemInstrItinData<II_VLDA_POP_dmx_lda_fifo_ex_bfp16_normal_pop, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_fex_bfp13__fifo_1d_pop__dmx_lda_fifo_fex_bfp13__core_pop__dmx_lda_fifo_fex_bfp13__fifo_1d__mPfa__mLdFifo_a__mMa
MemInstrItinData<II_VLDA_POP_dmx_lda_fifo_fex_bfp13_fifo_1d_pop, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_fex_bfp13__normal_pop__dmx_lda_fifo_fex_bfp13__core_pop__dmx_lda_fifo_fex_bfp13__normal__mPfa__mLdFifo_a
MemInstrItinData<II_VLDA_POP_dmx_lda_fifo_fex_bfp13_normal_pop, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_x__fifo_1d_pop__dmx_lda_fifo_x__core_pop__dmx_lda_fifo_x__fifo_1d__mPfa__mLdFifo_a__mMa
MemInstrItinData<II_VLDA_POP_dmx_lda_fifo_x_fifo_1d_pop, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_x__normal_pop__dmx_lda_fifo_x__core_pop__dmx_lda_fifo_x__normal__mPfa__mLdFifo_a
MemInstrItinData<II_VLDA_POP_dmx_lda_fifo_x_normal_pop, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_fex_bfp11__fifo_2d_pop__dmx_lda_fifo_fex_bfp11__core_pop__dmx_lda_fifo_fex_bfp11__fifo_2d__mPfa__mLdFifo_a__mDa
MemInstrItinData<II_VLDA_POP_mx4_2D, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dc*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_fex_bfp11__fifo_3d_pop__dmx_lda_fifo_fex_bfp11__core_pop__dmx_lda_fifo_fex_bfp11__fifo_3d__mPfa__mLdFifo_a__mDSa
MemInstrItinData<II_VLDA_POP_mx4_3D, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_fex_bfp11__fifo_1d_pop__dmx_lda_fifo_fex_bfp11__core_pop__dmx_lda_fifo_fex_bfp11__fifo_1d__mPfa__mLdFifo_a__mMa
MemInstrItinData<II_VLDA_POP_mx4_fifo_1d_pop, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_fex_bfp11__normal_pop__dmx_lda_fifo_fex_bfp11__core_pop__dmx_lda_fifo_fex_bfp11__normal__mPfa__mLdFifo_a
MemInstrItinData<II_VLDA_POP_mx4_normal_pop, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_qx__fifo_2d_pop__dmx_lda_fifo_qx__core_pop__dmx_lda_fifo_qx__fifo_2d__mPfa__mLdFifo_a__mDa
MemInstrItinData<II_VLDA_POP_s_2D, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dc*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_qx__fifo_3d_pop__dmx_lda_fifo_qx__core_pop__dmx_lda_fifo_qx__fifo_3d__mPfa__mLdFifo_a__mDSa
MemInstrItinData<II_VLDA_POP_s_3D, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_qx__fifo_1d_pop__dmx_lda_fifo_qx__core_pop__dmx_lda_fifo_qx__fifo_1d__mPfa__mLdFifo_a__mMa
MemInstrItinData<II_VLDA_POP_s_fifo_1d_pop, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_qx__normal_pop__dmx_lda_fifo_qx__core_pop__dmx_lda_fifo_qx__normal__mPfa__mLdFifo_a
MemInstrItinData<II_VLDA_POP_s_normal_pop, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, PrefixCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_UPS_2x_dmw_lda_ups_w2b_idx_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__KL>], [/*dst*/9, /*base*/7, /*ptr*/1, /*dj*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_UPS_2x_dmw_lda_ups_w2b_idx_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__KL>], [/*dst*/9, /*base*/7, /*ptr*/1, /*dj*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_UPS_2x_dmw_lda_ups_w2b_idx_imm_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__KL>], [/*dst*/9, /*base*/7, /*ptr*/1, /*imm*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_UPS_2x_dmw_lda_ups_w2b_idx_imm_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__KL>], [/*dst*/9, /*base*/7, /*ptr*/1, /*imm*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__KL>], [/*dst*/9, /*ptr_out*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__KL>], [/*dst*/9, /*ptr_out*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_imm_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__KL>], [/*dst*/9, /*ptr_out*/1, /*base*/7, /*ptr*/1, /*imm*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_imm_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__KL>], [/*dst*/9, /*ptr_out*/1, /*base*/7, /*ptr*/1, /*imm*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, PrefixCycle<UPS__H__OMUXL>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*base*/7, /*ptr*/1, /*dj*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, PrefixCycle<UPS__H__OMUXL>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*base*/7, /*ptr*/1, /*dj*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_UPS_2x_dmx_lda_ups_x2c_idx_imm_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, PrefixCycle<UPS__H__OMUXL>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*base*/7, /*ptr*/1, /*imm*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_UPS_2x_dmx_lda_ups_x2c_idx_imm_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, PrefixCycle<UPS__H__OMUXL>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*base*/7, /*ptr*/1, /*imm*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, PrefixCycle<UPS__H__OMUXL>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*ptr_out*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, PrefixCycle<UPS__H__OMUXL>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*ptr_out*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_imm_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, PrefixCycle<UPS__H__OMUXL>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*ptr_out*/1, /*base*/7, /*ptr*/1, /*imm*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_imm_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, PrefixCycle<UPS__H__OMUXL>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*ptr_out*/1, /*base*/7, /*ptr*/1, /*imm*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_UPS_4x_dmw_lda_ups_w2c_idx_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, SimpleCycle<CRUPSMODE_RA>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*base*/7, /*ptr*/1, /*dj*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_UPS_4x_dmw_lda_ups_w2c_idx_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, SimpleCycle<CRUPSMODE_RA>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*base*/7, /*ptr*/1, /*dj*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_UPS_4x_dmw_lda_ups_w2c_idx_imm_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, SimpleCycle<CRUPSMODE_RA>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*base*/7, /*ptr*/1, /*imm*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_UPS_4x_dmw_lda_ups_w2c_idx_imm_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, SimpleCycle<CRUPSMODE_RA>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*base*/7, /*ptr*/1, /*imm*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, SimpleCycle<CRUPSMODE_RA>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*ptr_out*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, SimpleCycle<CRUPSMODE_RA>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*ptr_out*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_imm_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, SimpleCycle<CRUPSMODE_RA>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*ptr_out*/1, /*base*/7, /*ptr*/1, /*imm*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_imm_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, SimpleCycle<CRUPSMODE_RA>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/9, /*ptr_out*/1, /*base*/7, /*ptr*/1, /*imm*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_UPS_4x_dmx_lda_ups_x2d_idx_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, SimpleCycle<UPS__OMUX>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__K>], [/*dst*/9, /*base*/7, /*ptr*/1, /*dj*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_UPS_4x_dmx_lda_ups_x2d_idx_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, SimpleCycle<UPS__OMUX>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__K>], [/*dst*/9, /*base*/7, /*ptr*/1, /*dj*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_UPS_4x_dmx_lda_ups_x2d_idx_imm_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, SimpleCycle<UPS__OMUX>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__K>], [/*dst*/9, /*base*/7, /*ptr*/1, /*imm*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_UPS_4x_dmx_lda_ups_x2d_idx_imm_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, SimpleCycle<UPS__OMUX>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__K>], [/*dst*/9, /*base*/7, /*ptr*/1, /*imm*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, SimpleCycle<UPS__OMUX>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__K>], [/*dst*/9, /*ptr_out*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, SimpleCycle<UPS__OMUX>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__K>], [/*dst*/9, /*ptr_out*/1, /*base*/7, /*ptr*/1, /*mod*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign0
MemInstrItinData<II_VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_imm_upsSign0, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, SimpleCycle<UPS__OMUX>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__K>], [/*dst*/9, /*ptr_out*/1, /*base*/7, /*ptr*/1, /*imm*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign1
MemInstrItinData<II_VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_imm_upsSign1, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, SimpleCycle<UPS__OMUX>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__K>], [/*dst*/9, /*ptr_out*/1, /*base*/7, /*ptr*/1, /*imm*/1, /*srUPS_of*/9, /*crSat*/7, /*crUPSMode*/8, /*upsSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_feg2__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa
MemInstrItinData<II_VLDA_dmw_lda_feg2_ld_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_feg2__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32
MemInstrItinData<II_VLDA_dmw_lda_feg2_ld_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_feg2__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa
MemInstrItinData<II_VLDA_dmw_lda_feg2_ld_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_feg2__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32
MemInstrItinData<II_VLDA_dmw_lda_feg2_ld_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_feg2__spill__agua_dmw__nrm_spill__agua_dmw__spill__mSPa__m_c15n_step32
MemInstrItinData<II_VLDA_dmw_lda_feg2_spill, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_w__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa
MemInstrItinData<II_VLDA_dmw_lda_w_ld_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_w__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32
MemInstrItinData<II_VLDA_dmw_lda_w_ld_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_w__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa
MemInstrItinData<II_VLDA_dmw_lda_w_ld_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_w__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32
MemInstrItinData<II_VLDA_dmw_lda_w_ld_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmw_lda_w__spill__agua_dmw__nrm_spill__agua_dmw__spill__mSPa__m_c15n_step32
MemInstrItinData<II_VLDA_dmw_lda_w_spill, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_bm__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa
MemInstrItinData<II_VLDA_dmx_lda_bm_ld_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DM_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_bm__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64
MemInstrItinData<II_VLDA_dmx_lda_bm_ld_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DM_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_bm__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa
MemInstrItinData<II_VLDA_dmx_lda_bm_ld_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DM_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_bm__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64
MemInstrItinData<II_VLDA_dmx_lda_bm_ld_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DM_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_bm__spill__agua_dmx__nrm_spill__agua_dmx__spill__mSPa__m_c16n_step64
MemInstrItinData<II_VLDA_dmx_lda_bm_spill, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<DM_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_x__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa
MemInstrItinData<II_VLDA_dmx_lda_x_ld_idx, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_x__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64
MemInstrItinData<II_VLDA_dmx_lda_x_ld_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_x__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa
MemInstrItinData<II_VLDA_dmx_lda_x_ld_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_x__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64
MemInstrItinData<II_VLDA_dmx_lda_x_ld_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__lda__dmx_lda_x__spill__agua_dmx__nrm_spill__agua_dmx__spill__mSPa__m_c16n_step64
MemInstrItinData<II_VLDA_dmx_lda_x_spill, [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/7, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmv_ldb__agub_dmv__nrm__agub_dmv__normal__agub_dmv__idx__mPb__mDJb
MemInstrItinData<II_VLDB_128_idx, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmv_ldb__agub_dmv__nrm__agub_dmv__normal__agub_dmv__idx_imm__mPb__m_c08s_step16
MemInstrItinData<II_VLDB_128_idx_imm, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmv_ldb__agub_dmv__nrm__agub_dmv__normal__agub_dmv__pstm_nrm__mPb__mMb
MemInstrItinData<II_VLDB_128_pstm_nrm, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmv_ldb__agub_dmv__nrm__agub_dmv__normal__agub_dmv__pstm_nrm_imm__mPb__m_c08s_step16
MemInstrItinData<II_VLDB_128_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmv_ldb__agub_dmv__nrm__agub_dmv__normal__agub_dmv__pstm_2d__mPb__mDb
MemInstrItinData<II_VLDB_2D_128, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_2d__mPb__mDb__unpackSign0
MemInstrItinData<II_VLDB_2D_UNPACK_dmw_ldb_unpack_unpackSign0, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1, /*crUnpackSize*/7, /*unpackSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_2d__mPb__mDb__unpackSign1
MemInstrItinData<II_VLDB_2D_UNPACK_dmw_ldb_unpack_unpackSign1, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1, /*crUnpackSize*/7, /*unpackSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_2d__mPb__mDb__mYb__unpackSign0
MemInstrItinData<II_VLDB_2D_UNPACK_dmx_ldb_unpack_unpackSign0, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1, /*crUnpackSize*/7, /*unpackSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_2d__mPb__mDb__mYb__unpackSign1
MemInstrItinData<II_VLDB_2D_UNPACK_dmx_ldb_unpack_unpackSign1, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1, /*crUnpackSize*/7, /*unpackSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmw_ldb__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_2d__mPb__mDb
MemInstrItinData<II_VLDB_2D_dmw_ldb, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_x__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_2d__mPb__mDb
MemInstrItinData<II_VLDB_2D_dmx_ldb_x, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dc*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmv_ldb__agub_dmv__nrm__agub_dmv__normal__agub_dmv__pstm_3d__mPb__mDSb
MemInstrItinData<II_VLDB_3D_128, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_3d__mPb__mDSb__unpackSign0
MemInstrItinData<II_VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign0, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1, /*crUnpackSize*/7, /*unpackSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_3d__mPb__mDSb__unpackSign1
MemInstrItinData<II_VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign1, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1, /*crUnpackSize*/7, /*unpackSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_3d__mPb__mDSb__mYb__unpackSign0
MemInstrItinData<II_VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign0, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1, /*crUnpackSize*/7, /*unpackSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_3d__mPb__mDSb__mYb__unpackSign1
MemInstrItinData<II_VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign1, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1, /*crUnpackSize*/7, /*unpackSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmw_ldb__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_3d__mPb__mDSb
MemInstrItinData<II_VLDB_3D_dmw_ldb, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_x__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_3d__mPb__mDSb
MemInstrItinData<II_VLDB_3D_dmx_ldb_x, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__ldb_or1__dmv_ldb_4x__load_4x16__load_4x_pointer_hi
MemInstrItinData<II_VLDB_4x16_hi, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*src*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__ldb_or1__dmv_ldb_4x__load_4x16__load_4x_pointer_lo
MemInstrItinData<II_VLDB_4x16_lo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*src*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__ldb_or1__dmv_ldb_4x__load_4x32__load_4x_pointer_hi
MemInstrItinData<II_VLDB_4x32_hi, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*src*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__ldb_or1__dmv_ldb_4x__load_4x32__load_4x_pointer_lo
MemInstrItinData<II_VLDB_4x32_lo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*src*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__ldb_or1__dmv_ldb_4x__load_4x64__load_4x_pointer_hi
MemInstrItinData<II_VLDB_4x64_hi, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*src*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__ldb_or1__dmv_ldb_4x__load_4x64__load_4x_pointer_lo
MemInstrItinData<II_VLDB_4x64_lo, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADA>, EmptyCycles<4>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*src*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_x__normal_fillx__dmx_ldb_fifo_x__core_fillx__mR30_fifo_step_e1__mR30_fifo_step_e7__dmx_ldb_fifo_x__normalx__mPfb__mLdFifo_b
MemInstrItinData<II_VLDB_FILLX, [AvoidPartWordStore], [/*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*conf_e1*/1, /*conf_e7*/7, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*lfe*/7, /*lfe*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_x__normal_fill__dmx_ldb_fifo_x__core_fill__dmx_ldb_fifo_x__normal__mPfb__mLdFifo_b
MemInstrItinData<II_VLDB_FILL, [AvoidPartWordStore], [/*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_x__normal_popx__dmx_ldb_fifo_x__core_popx__mR30_fifo_step_e1__mR30_fifo_step_e7__dmx_ldb_fifo_x__normalx__mPfb__mLdFifo_b
MemInstrItinData<II_VLDB_POPX, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*conf_e1*/1, /*conf_e7*/7, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*lfe*/7, /*srFifo_uf*/3, /*lfe*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_ex_bfp16__fifo_2d_pop__dmx_ldb_fifo_ex_bfp16__core_pop__dmx_ldb_fifo_ex_bfp16__fifo_2d__mPfb__mLdFifo_b__mDb
MemInstrItinData<II_VLDB_POP_2D_dmx_ldb_fifo_ex_bfp16, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dc*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_fex_bfp13__fifo_2d_pop__dmx_ldb_fifo_fex_bfp13__core_pop__dmx_ldb_fifo_fex_bfp13__fifo_2d__mPfb__mLdFifo_b__mDb
MemInstrItinData<II_VLDB_POP_2D_dmx_ldb_fifo_fex_bfp13, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dc*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_x__fifo_2d_pop__dmx_ldb_fifo_x__core_pop__dmx_ldb_fifo_x__fifo_2d__mPfb__mLdFifo_b__mDb
MemInstrItinData<II_VLDB_POP_2D_dmx_ldb_fifo_x, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dc*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_ex_bfp16__fifo_3d_pop__dmx_ldb_fifo_ex_bfp16__core_pop__dmx_ldb_fifo_ex_bfp16__fifo_3d__mPfb__mLdFifo_b__mDSb
MemInstrItinData<II_VLDB_POP_3D_dmx_ldb_fifo_ex_bfp16, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_fex_bfp13__fifo_3d_pop__dmx_ldb_fifo_fex_bfp13__core_pop__dmx_ldb_fifo_fex_bfp13__fifo_3d__mPfb__mLdFifo_b__mDSb
MemInstrItinData<II_VLDB_POP_3D_dmx_ldb_fifo_fex_bfp13, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_x__fifo_3d_pop__dmx_ldb_fifo_x__core_pop__dmx_ldb_fifo_x__fifo_3d__mPfb__mLdFifo_b__mDSb
MemInstrItinData<II_VLDB_POP_3D_dmx_ldb_fifo_x, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_ex_bfp16__fifo_1d_pop__dmx_ldb_fifo_ex_bfp16__core_pop__dmx_ldb_fifo_ex_bfp16__fifo_1d__mPfb__mLdFifo_b__mMb
MemInstrItinData<II_VLDB_POP_dmx_ldb_fifo_ex_bfp16_fifo_1d_pop, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_ex_bfp16__normal_pop__dmx_ldb_fifo_ex_bfp16__core_pop__dmx_ldb_fifo_ex_bfp16__normal__mPfb__mLdFifo_b
MemInstrItinData<II_VLDB_POP_dmx_ldb_fifo_ex_bfp16_normal_pop, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_fex_bfp13__fifo_1d_pop__dmx_ldb_fifo_fex_bfp13__core_pop__dmx_ldb_fifo_fex_bfp13__fifo_1d__mPfb__mLdFifo_b__mMb
MemInstrItinData<II_VLDB_POP_dmx_ldb_fifo_fex_bfp13_fifo_1d_pop, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_fex_bfp13__normal_pop__dmx_ldb_fifo_fex_bfp13__core_pop__dmx_ldb_fifo_fex_bfp13__normal__mPfb__mLdFifo_b
MemInstrItinData<II_VLDB_POP_dmx_ldb_fifo_fex_bfp13_normal_pop, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_x__fifo_1d_pop__dmx_ldb_fifo_x__core_pop__dmx_ldb_fifo_x__fifo_1d__mPfb__mLdFifo_b__mMb
MemInstrItinData<II_VLDB_POP_dmx_ldb_fifo_x_fifo_1d_pop, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_x__normal_pop__dmx_ldb_fifo_x__core_pop__dmx_ldb_fifo_x__normal__mPfb__mLdFifo_b
MemInstrItinData<II_VLDB_POP_dmx_ldb_fifo_x_normal_pop, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_fex_bfp11__fifo_2d_pop__dmx_ldb_fifo_fex_bfp11__core_pop__dmx_ldb_fifo_fex_bfp11__fifo_2d__mPfb__mLdFifo_b__mDb
MemInstrItinData<II_VLDB_POP_mx4_2D, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dc*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_fex_bfp11__fifo_3d_pop__dmx_ldb_fifo_fex_bfp11__core_pop__dmx_ldb_fifo_fex_bfp11__fifo_3d__mPfb__mLdFifo_b__mDSb
MemInstrItinData<II_VLDB_POP_mx4_3D, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_fex_bfp11__fifo_1d_pop__dmx_ldb_fifo_fex_bfp11__core_pop__dmx_ldb_fifo_fex_bfp11__fifo_1d__mPfb__mLdFifo_b__mMb
MemInstrItinData<II_VLDB_POP_mx4_fifo_1d_pop, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_fex_bfp11__normal_pop__dmx_ldb_fifo_fex_bfp11__core_pop__dmx_ldb_fifo_fex_bfp11__normal__mPfb__mLdFifo_b
MemInstrItinData<II_VLDB_POP_mx4_normal_pop, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_qx__fifo_2d_pop__dmx_ldb_fifo_qx__core_pop__dmx_ldb_fifo_qx__fifo_2d__mPfb__mLdFifo_b__mDb
MemInstrItinData<II_VLDB_POP_s_2D, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dc*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_qx__fifo_3d_pop__dmx_ldb_fifo_qx__core_pop__dmx_ldb_fifo_qx__fifo_3d__mPfb__mLdFifo_b__mDSb
MemInstrItinData<II_VLDB_POP_s_3D, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*dcl*/1, /*dch*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_qx__fifo_1d_pop__dmx_ldb_fifo_qx__core_pop__dmx_ldb_fifo_qx__fifo_1d__mPfb__mLdFifo_b__mMb
MemInstrItinData<II_VLDB_POP_s_fifo_1d_pop, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*mod*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_qx__normal_pop__dmx_ldb_fifo_qx__core_pop__dmx_ldb_fifo_qx__normal__mPfb__mLdFifo_b
MemInstrItinData<II_VLDB_POP_s_normal_pop, [AvoidPartWordStore, EmptyCycles<7>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/8, /*ptr_out*/1, /*fifo_reg_out*/7, /*pos_out*/1, /*ptr*/1, /*fifo_reg*/7, /*pos*/1, /*srFifo_uf*/3], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__idx__mPb__mDJb__unpackSign0
MemInstrItinData<II_VLDB_UNPACK_dmw_ldb_unpack_idx_unpackSign0, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1, /*crUnpackSize*/7, /*unpackSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__idx__mPb__mDJb__unpackSign1
MemInstrItinData<II_VLDB_UNPACK_dmw_ldb_unpack_idx_unpackSign1, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1, /*crUnpackSize*/7, /*unpackSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__idx_imm__mPb__m_c09s_step32__unpackSign0
MemInstrItinData<II_VLDB_UNPACK_dmw_ldb_unpack_idx_imm_unpackSign0, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1, /*crUnpackSize*/7, /*unpackSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__idx_imm__mPb__m_c09s_step32__unpackSign1
MemInstrItinData<II_VLDB_UNPACK_dmw_ldb_unpack_idx_imm_unpackSign1, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1, /*crUnpackSize*/7, /*unpackSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_nrm__mPb__mMb__unpackSign0
MemInstrItinData<II_VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_unpackSign0, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1, /*crUnpackSize*/7, /*unpackSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_nrm__mPb__mMb__unpackSign1
MemInstrItinData<II_VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_unpackSign1, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1, /*crUnpackSize*/7, /*unpackSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_nrm_imm__mPb__m_c09s_step32__unpackSign0
MemInstrItinData<II_VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_imm_unpackSign0, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1, /*crUnpackSize*/7, /*unpackSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_nrm_imm__mPb__m_c09s_step32__unpackSign1
MemInstrItinData<II_VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_imm_unpackSign1, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1, /*crUnpackSize*/7, /*unpackSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__idx__mPb__mDJb__mYb__unpackSign0
MemInstrItinData<II_VLDB_UNPACK_dmx_ldb_unpack_idx_unpackSign0, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1, /*crUnpackSize*/7, /*unpackSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__idx__mPb__mDJb__mYb__unpackSign1
MemInstrItinData<II_VLDB_UNPACK_dmx_ldb_unpack_idx_unpackSign1, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1, /*crUnpackSize*/7, /*unpackSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__idx_imm__mPb__m_c10s_step64__mYb__unpackSign0
MemInstrItinData<II_VLDB_UNPACK_dmx_ldb_unpack_idx_imm_unpackSign0, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1, /*crUnpackSize*/7, /*unpackSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__idx_imm__mPb__m_c10s_step64__mYb__unpackSign1
MemInstrItinData<II_VLDB_UNPACK_dmx_ldb_unpack_idx_imm_unpackSign1, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1, /*crUnpackSize*/7, /*unpackSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_nrm__mPb__mMb__mYb__unpackSign0
MemInstrItinData<II_VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_unpackSign0, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1, /*crUnpackSize*/7, /*unpackSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_nrm__mPb__mMb__mYb__unpackSign1
MemInstrItinData<II_VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_unpackSign1, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1, /*crUnpackSize*/7, /*unpackSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_nrm_imm__mPb__m_c10s_step64__mYb__unpackSign0
MemInstrItinData<II_VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_imm_unpackSign0, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1, /*crUnpackSize*/7, /*unpackSign0*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_nrm_imm__mPb__m_c10s_step64__mYb__unpackSign1
MemInstrItinData<II_VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_imm_unpackSign1, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1, /*crUnpackSize*/7, /*unpackSign1*/7], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmw_ldb__agub_dmw__nrm__agub_dmw__normal__agub_dmw__idx__mPb__mDJb
MemInstrItinData<II_VLDB_dmw_ldb_idx, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmw_ldb__agub_dmw__nrm__agub_dmw__normal__agub_dmw__idx_imm__mPb__m_c09s_step32
MemInstrItinData<II_VLDB_dmw_ldb_idx_imm, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmw_ldb__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_nrm__mPb__mMb
MemInstrItinData<II_VLDB_dmw_ldb_pstm_nrm, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmw_ldb__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_nrm_imm__mPb__m_c09s_step32
MemInstrItinData<II_VLDB_dmw_ldb_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_x__agub_dmx__nrm__agub_dmx__normal__agub_dmx__idx__mPb__mDJb
MemInstrItinData<II_VLDB_dmx_ldb_x_idx, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_x__agub_dmx__nrm__agub_dmx__normal__agub_dmx__idx_imm__mPb__m_c10s_step64
MemInstrItinData<II_VLDB_dmx_ldb_x_idx_imm, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_x__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_nrm__mPb__mMb
MemInstrItinData<II_VLDB_dmx_ldb_x_pstm_nrm, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__ldb__dmx_ldb_x__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_nrm_imm__mPb__m_c10s_step64
MemInstrItinData<II_VLDB_dmx_ldb_x_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<6>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*ptr_out*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out32__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VLT_16_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out32__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VLT_16_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out16__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VLT_32_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out16__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VLT_32_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VLT_8_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VLT_8_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,


// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f16_compare__vlt_bf16__vec_cmp_out32__vec_add_mX__vec_add_nX
InstrItinData<II_VLT_bf16, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_compare__vlt_bf8__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX
InstrItinData<II_VLT_bf8, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f16_compare__vlt_fp16__vec_cmp_out32__vec_add_mX__vec_add_nX
InstrItinData<II_VLT_fp16, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_compare__vlt_fp8__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX
InstrItinData<II_VLT_fp8, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*cmp*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
InstrItinData<II_VMAC_vmac_vmul_cm_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
InstrItinData<II_VMAC_vmac_vmul_cm_core_X_QY, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
InstrItinData<II_VMAC_vmac_vmul_cm_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
InstrItinData<II_VMAC_vmac_vmul_cm_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
InstrItinData<II_VMAC_vmac_vmul_cm_core_X_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
InstrItinData<II_VMAC_vmac_vmul_cm_core_Y_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
InstrItinData<II_VMAC_vmac_vmul_cm_core_Y_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
InstrItinData<II_VMAC_vmac_vmul_cm_core_Y_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
InstrItinData<II_VMAC_vmac_vmul_cm_core_Y_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
InstrItinData<II_VMAC_vmac_half_vmul_cm_half_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
InstrItinData<II_VMAC_vmac_half_vmul_cm_half_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
InstrItinData<II_VMAC_vmac_half_vmul_cm_half_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
InstrItinData<II_VMAC_f_vmac_bf_vmul_bf_core_X_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
InstrItinData<II_VMAC_f_vmac_bf_vmul_bf_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
InstrItinData<II_VMAC_f_vmac_bf_vmul_bf_core_Y_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
InstrItinData<II_VMAC_f_vmac_bf_vmul_bf_core_Y_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
InstrItinData<II_VMAC_f_vmac_bf_half_vmul_bf_core_X_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
InstrItinData<II_VMAC_f_vmac_bf_half_vmul_bf_core_X_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
InstrItinData<II_VMAC_f_vmac_bf_half_vmul_bf_core_Y_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
InstrItinData<II_VMAC_f_vmac_bf_half_vmul_bf_half_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bfp13__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
InstrItinData<II_VMAC_f_vmac_bfp13, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, PrefixCycle<X_EXP_ELL>, SimpleCycle<Y_EXP_E>, EmptyCycles<1>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/6, /*acc1*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bfp16__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
InstrItinData<II_VMAC_f_vmac_bfp16, [PrefixCycle<FEY_RW_L0_PORT>, InstrStage<2, [VCTRL_IN], 0>, InstrStage<2, [VCTRL_SUB0_CG1U], 0>, InstrStage<2, [X_EGWL], 0>, InstrStage<2, [X_EXP_ELL], 0>, InstrStage<2, [Y_EGY], 1>, PrefixCycle<FEY_RW_L0_PORT>, SimpleCycle<Y_EXP_E>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFSHCH__ACC1>, SimpleCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*s1l*/1, /*s2l*/1, /*s2h*/2, /*s1h*/2, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1l*/NoBypass, /*s2l*/NoBypass, /*s2h*/NoBypass, /*s1h*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmaxdiff_lt__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VMAXDIFF_LT_16_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmaxdiff_lt__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VMAXDIFF_LT_16_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmaxdiff_lt__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VMAXDIFF_LT_32_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmaxdiff_lt__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VMAXDIFF_LT_32_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmaxdiff_lt__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VMAXDIFF_LT_8_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r17_r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*d*/MV_Bypass, /*r17_r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmaxdiff_lt__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VMAXDIFF_LT_8_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r17_r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*d*/MV_Bypass, /*r17_r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmax_lt__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VMAX_LT_16_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmax_lt__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VMAX_LT_16_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmax_lt__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VMAX_LT_32_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmax_lt__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VMAX_LT_32_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmax_lt__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VMAX_LT_8_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r17_r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*d*/MV_Bypass, /*r17_r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmax_lt__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VMAX_LT_8_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r17_r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*d*/MV_Bypass, /*r17_r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f16_min_max__vmax_lt_bf16__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX
InstrItinData<II_VMAX_LT_bf16, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_min_max__vmax_lt_bf8__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX
InstrItinData<II_VMAX_LT_bf8, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r17_r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*r17_r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f16_min_max__vmax_lt_fp16__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX
InstrItinData<II_VMAX_LT_fp16, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_min_max__vmax_lt_fp8__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX
InstrItinData<II_VMAX_LT_fp8, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r17_r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*r17_r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmin_ge__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VMIN_GE_16_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmin_ge__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VMIN_GE_16_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmin_ge__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VMIN_GE_32_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmin_ge__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VMIN_GE_32_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmin_ge__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VMIN_GE_8_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r17_r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*d*/MV_Bypass, /*r17_r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmin_ge__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VMIN_GE_8_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r17_r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*d*/MV_Bypass, /*r17_r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f16_min_max__vmin_ge_bf16__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX
InstrItinData<II_VMIN_GE_bf16, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_min_max__vmin_ge_bf8__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX
InstrItinData<II_VMIN_GE_bf8, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r17_r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*r17_r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f16_min_max__vmin_ge_fp16__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX
InstrItinData<II_VMIN_GE_fp16, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_min_max__vmin_ge_fp8__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX
InstrItinData<II_VMIN_GE_fp8, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r17_r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*r17_r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_cm__mv_scd_E2__long_static__scd_cm_0__mSCD_E2__mSCD_comp_E2__mSCD
InstrItinData<II_VMOV_0_mv_scd_cm, [SimpleCycle<SCD_CTL>, PrefixCycle<BM_WM_1>, PrefixCycle<DM_WM_L0_PORT>, SimpleCycle<SCD>], [/*dst*/2, /*crSCDEn*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_dm_imm__scd_dm_0__mv_scd_dm_core__mDMm__mv_scd_E2__long_dynam__mSCD_E2__mSCD_comp_E2__mSCD
InstrItinData<II_VMOV_0_mv_scd_dm_imm, [SimpleCycle<SCD_CTL>, PrefixCycle<BM_WM_1>, PrefixCycle<DM_WM_L0_PORT>, SimpleCycle<SCD>], [/*scd_core*/2, /*crSCDEn*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_cm__mv_scd_E2__long_static__scd_cm_1__mSCD_E2__mSCD_comp_E2__mSCD
InstrItinData<II_VMOV_1_mv_scd_cm, [SimpleCycle<SCD_CTL>, PrefixCycle<BM_WM_1>, PrefixCycle<DM_WM_L0_PORT>, SimpleCycle<SCD>], [/*dst*/2, /*crSCDEn*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_dm_imm__scd_dm_1__mv_scd_dm_core__mDMm__mv_scd_E2__long_dynam__mSCD_E2__mSCD_comp_E2__mSCD
InstrItinData<II_VMOV_1_mv_scd_dm_imm, [SimpleCycle<SCD_CTL>, PrefixCycle<BM_WM_1>, PrefixCycle<DM_WM_L0_PORT>, SimpleCycle<SCD>], [/*scd_core*/2, /*crSCDEn*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_dm_imm__scd_dm_2__mv_scd_dm_core__mDMm__mv_scd_E2__long_dynam__mSCD_E2__mSCD_comp_E2__mSCD
InstrItinData<II_VMOV_2, [SimpleCycle<SCD_CTL>, PrefixCycle<BM_WM_1>, PrefixCycle<DM_WM_L0_PORT>, SimpleCycle<SCD>], [/*scd_core*/2, /*crSCDEn*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_dm_imm__scd_dm_3__mv_scd_dm_core__mDMm__mv_scd_E2__long_dynam__mSCD_E2__mSCD_comp_E2__mSCD
InstrItinData<II_VMOV_3, [SimpleCycle<SCD_CTL>, PrefixCycle<BM_WM_1>, PrefixCycle<DM_WM_L0_PORT>, SimpleCycle<SCD>], [/*scd_core*/2, /*crSCDEn*/1]>,

// id: me__instr128_or__vec__vmov__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv
InstrItinData<II_VMOV_D_vmov, [SimpleCycle<VCTRL_IN>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass]>,

// id: me__instr128_or__vec__vmov_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv
InstrItinData<II_VMOV_D_vmov_half, [SimpleCycle<VCTRL_IN>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_bm__mv_scd_E2__short__mSCD_E2__mSCD_comp_E2__mSCD
InstrItinData<II_VMOV_alu_mv_alu_mv_scd_bm, [SimpleCycle<SCD_CTL>, PrefixCycle<DM_WM_L0_PORT>, SimpleCycle<SCD>], [/*dst*/2, /*crSCDEn*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_cmh_dyn__mv_scd_cm_cmh__mv_scd_cm_dyn__mv_scd_cm_cmh__mv_scd_cm_core__mCMHm__mv_scd_E2__long_dynam_cm__mSCD_E2__mSCD_comp_E2__mSCD__mSCD_r_incr__mR31_scd
InstrItinData<II_VMOV_alu_mv_alu_mv_scd_cmh_dyn, [PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<BM_WM_1>, PrefixCycle<DM_WM_L0_PORT>, SimpleCycle<SCD>], [/*scd_core*/2, /*r31_out*/1, /*r31*/1, /*crSCDEn*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_cmh_reg__mv_scd_cm_cmh__mv_scd_cm_reg__mv_scd_cm_cmh__mv_scd_cm_core__mCMHm__mv_scd_E2__long_dynam_cm__mSCD_E2__mSCD_comp_E2__mSCD__mSCD_r__mR31_scd
InstrItinData<II_VMOV_alu_mv_alu_mv_scd_cmh_reg, [PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<BM_WM_1>, PrefixCycle<DM_WM_L0_PORT>, SimpleCycle<SCD>], [/*scd_core*/2, /*r31*/1, /*crSCDEn*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_cml_dyn__mv_scd_cm_cml__mv_scd_cm_dyn__mv_scd_cm_cml__mv_scd_cm_core__mCMLm__mv_scd_E2__long_dynam_cm__mSCD_E2__mSCD_comp_E2__mSCD__mSCD_r_incr__mR31_scd
InstrItinData<II_VMOV_alu_mv_alu_mv_scd_cml_dyn, [PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<BM_WM_1>, PrefixCycle<DM_WM_L0_PORT>, SimpleCycle<SCD>], [/*scd_core*/2, /*r31_out*/1, /*r31*/1, /*crSCDEn*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_cml_reg__mv_scd_cm_cml__mv_scd_cm_reg__mv_scd_cm_cml__mv_scd_cm_core__mCMLm__mv_scd_E2__long_dynam_cm__mSCD_E2__mSCD_comp_E2__mSCD__mSCD_r__mR31_scd
InstrItinData<II_VMOV_alu_mv_alu_mv_scd_cml_reg, [PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<BM_WM_1>, PrefixCycle<DM_WM_L0_PORT>, SimpleCycle<SCD>], [/*scd_core*/2, /*r31*/1, /*crSCDEn*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_dm_dyn__mv_scd_dm_core__mDMm__mv_scd_E2__long_dynam__mSCD_E2__mSCD_comp_E2__mSCD__mSCD_r_incr__mR31_scd
InstrItinData<II_VMOV_alu_mv_alu_mv_scd_dm_dyn, [PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<BM_WM_1>, PrefixCycle<DM_WM_L0_PORT>, SimpleCycle<SCD>], [/*scd_core*/2, /*r31_out*/1, /*r31*/1, /*crSCDEn*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_dm_reg__mv_scd_dm_core__mDMm__mv_scd_E2__long_dynam__mSCD_E2__mSCD_comp_E2__mSCD__mSCD_r__mR31_scd
InstrItinData<II_VMOV_alu_mv_alu_mv_scd_dm_reg, [PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<BM_WM_1>, PrefixCycle<DM_WM_L0_PORT>, SimpleCycle<SCD>], [/*scd_core*/2, /*r31*/1, /*crSCDEn*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_x__mv_scd_E2__short__mSCD_E2__mSCD_comp_E2__mSCD
InstrItinData<II_VMOV_alu_mv_alu_mv_scd_x, [SimpleCycle<SCD_CTL>, PrefixCycle<FEY_WA_L0_PORT>, SimpleCycle<SCD>], [/*dst*/2, /*crSCDEn*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_cm
InstrItinData<II_VMOV_alu_mv_mv_mv_cm, [SimpleCycle<DM_RM_L0_PORT>, PrefixCycle<BM_WM_1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ee
InstrItinData<II_VMOV_alu_mv_mv_mv_ee, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_eg
InstrItinData<II_VMOV_alu_mv_mv_mv_eg, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_eg2
InstrItinData<II_VMOV_alu_mv_mv_mv_eg2, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_eg2_to_w
InstrItinData<II_VMOV_alu_mv_mv_mv_eg2_to_w, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_egw
InstrItinData<II_VMOV_alu_mv_mv_mv_egw, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_egx
InstrItinData<II_VMOV_alu_mv_mv_mv_egx, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_f
InstrItinData<II_VMOV_alu_mv_mv_mv_f, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_f_to_l__mLm
InstrItinData<II_VMOV_alu_mv_mv_mv_f_to_l, [EmptyCycles<1>, SimpleCycle<L_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_few
InstrItinData<II_VMOV_alu_mv_mv_mv_few, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_fex
InstrItinData<II_VMOV_alu_mv_mv_mv_fex, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ff
InstrItinData<II_VMOV_alu_mv_mv_mv_ff, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_gg
InstrItinData<II_VMOV_alu_mv_mv_mv_gg, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_l_to_f__mLm
InstrItinData<II_VMOV_alu_mv_mv_mv_l_to_f, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_w
InstrItinData<II_VMOV_alu_mv_mv_mv_w, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_w_to_eg2
InstrItinData<II_VMOV_alu_mv_mv_mv_w_to_eg2, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_x
InstrItinData<II_VMOV_alu_mv_mv_mv_x, [PrefixCycle<DM_RM_L0_PORT>, SimpleCycle<LD_FIFO_RA_PORT>, PrefixCycle<DM_WM_L0_PORT>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXe_eXe, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifol_eXe, [], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eXe, [EmptyCycles<1>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHH_eXe, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHL_eXe, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eXe, [EmptyCycles<1>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eXe, [], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLL_eXe, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLH_eXe, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifoh_eXe, [], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXo_eXe, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXe_mStFifol, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXe_eLdFifoLReg, [SimpleCycle<LD_FIFO_RA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXe_eBMHH, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXe_eBMHL, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXe_eLdFifoHReg, [SimpleCycle<LD_FIFO_RA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXe_mFifoExtra, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXe_eBMLL, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXe_eBMLH, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXe_mStFifoh, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXe_eXo, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifol_mStFifol, [], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifol_eLdFifoLReg, [SimpleCycle<LD_FIFO_RA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifol_eBMHH, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifol_eBMHL, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifol_eLdFifoHReg, [SimpleCycle<LD_FIFO_RA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifol_mFifoExtra, [], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifol_eBMLL, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifol_eBMLH, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifol_mStFifoh, [], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifol_eXo, [], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_mStFifol, [EmptyCycles<1>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eLdFifoLReg, [SimpleCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eBMHH, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eBMHL, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eLdFifoHReg, [SimpleCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_mFifoExtra, [EmptyCycles<1>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eBMLL, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eBMLH, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_mStFifoh, [EmptyCycles<1>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eXo, [EmptyCycles<1>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHH_mStFifol, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHH_eLdFifoLReg, [SimpleCycle<LD_FIFO_RA_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHH_eBMHH, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHH_eBMHL, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHH_eLdFifoHReg, [SimpleCycle<LD_FIFO_RA_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHH_mFifoExtra, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHH_eBMLL, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHH_eBMLH, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHH_mStFifoh, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHH_eXo, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHL_mStFifol, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHL_eLdFifoLReg, [SimpleCycle<LD_FIFO_RA_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHL_eBMHH, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHL_eBMHL, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHL_eLdFifoHReg, [SimpleCycle<LD_FIFO_RA_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHL_mFifoExtra, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHL_eBMLL, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHL_eBMLH, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHL_mStFifoh, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMHL_eXo, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_mStFifol, [EmptyCycles<1>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eLdFifoLReg, [SimpleCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eBMHH, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eBMHL, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eLdFifoHReg, [SimpleCycle<LD_FIFO_RA_PORT>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_mFifoExtra, [EmptyCycles<1>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eBMLL, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eBMLH, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_mStFifoh, [EmptyCycles<1>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eXo, [EmptyCycles<1>, SimpleCycle<LD_FIFO_WA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_mStFifol, [], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eLdFifoLReg, [SimpleCycle<LD_FIFO_RA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eBMHH, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eBMHL, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eLdFifoHReg, [SimpleCycle<LD_FIFO_RA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_mFifoExtra, [], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eBMLL, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eBMLH, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_mStFifoh, [], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eXo, [], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLL_mStFifol, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLL_eLdFifoLReg, [SimpleCycle<LD_FIFO_RA_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLL_eBMHH, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLL_eBMHL, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLL_eLdFifoHReg, [SimpleCycle<LD_FIFO_RA_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLL_mFifoExtra, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLL_eBMLL, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLL_eBMLH, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLL_mStFifoh, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLL_eXo, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLH_mStFifol, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLH_eLdFifoLReg, [SimpleCycle<LD_FIFO_RA_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLH_eBMHH, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLH_eBMHL, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLH_eLdFifoHReg, [SimpleCycle<LD_FIFO_RA_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLH_mFifoExtra, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLH_eBMLL, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLH_eBMLH, [SimpleCycle<DM_RM_L0_PORT>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLH_mStFifoh, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eBMLH_eXo, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifoh_mStFifol, [], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifoh_eLdFifoLReg, [SimpleCycle<LD_FIFO_RA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifoh_eBMHH, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifoh_eBMHL, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifoh_eLdFifoHReg, [SimpleCycle<LD_FIFO_RA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifoh_mFifoExtra, [], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifoh_eBMLL, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifoh_eBMLH, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifoh_mStFifoh, [], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_mStFifoh_eXo, [], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/MV_Bypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXo_mStFifol, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXo_eLdFifoLReg, [SimpleCycle<LD_FIFO_RA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXo_eBMHH, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXo_eBMHL, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXo_eLdFifoHReg, [SimpleCycle<LD_FIFO_RA_PORT>], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXo_mFifoExtra, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXo_eBMLL, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXo_eBMLH, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXo_mStFifoh, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,
InstrItinData<II_VMOV_alu_mv_mv_mv_x_eXo_eXo, [], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/MV_Bypass]>,

// id: me__instr128_or__st__st_streams__mv_mcd_bm__mMCD__mMcdBMSrc
InstrItinData<II_VMOV_st_mv_mcd_bm, [SimpleCycle<CRMCDEN_RS>], [/*src*/1, /*crMCDEn*/1]>,

// id: me__instr128_or__st__st_streams__mv_mcd_x__mMCD__mMcdXSrc
InstrItinData<II_VMOV_st_mv_mcd_x, [PrefixCycle<CRMCDEN_RS>, SimpleCycle<FEY_RS_L0_PORT>], [/*src*/1, /*crMCDEn*/1]>,

// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
InstrItinData<II_VMSC_vmac_vmul_cm_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
InstrItinData<II_VMSC_vmac_vmul_cm_core_X_QY, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
InstrItinData<II_VMSC_vmac_vmul_cm_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
InstrItinData<II_VMSC_vmac_vmul_cm_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
InstrItinData<II_VMSC_vmac_vmul_cm_core_X_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
InstrItinData<II_VMSC_vmac_vmul_cm_core_Y_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
InstrItinData<II_VMSC_vmac_vmul_cm_core_Y_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
InstrItinData<II_VMSC_vmac_vmul_cm_core_Y_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
InstrItinData<II_VMSC_vmac_vmul_cm_core_Y_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
InstrItinData<II_VMSC_vmac_half_vmul_cm_half_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
InstrItinData<II_VMSC_vmac_half_vmul_cm_half_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
InstrItinData<II_VMSC_vmac_half_vmul_cm_half_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
InstrItinData<II_VMSC_f_vmac_bf_vmul_bf_core_X_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
InstrItinData<II_VMSC_f_vmac_bf_vmul_bf_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
InstrItinData<II_VMSC_f_vmac_bf_vmul_bf_core_Y_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
InstrItinData<II_VMSC_f_vmac_bf_vmul_bf_core_Y_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
InstrItinData<II_VMSC_f_vmac_bf_half_vmul_bf_core_X_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
InstrItinData<II_VMSC_f_vmac_bf_half_vmul_bf_core_X_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
InstrItinData<II_VMSC_f_vmac_bf_half_vmul_bf_core_Y_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
InstrItinData<II_VMSC_f_vmac_bf_half_vmul_bf_half_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bfp13__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
InstrItinData<II_VMSC_f_vmac_bfp13, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, PrefixCycle<X_EXP_ELL>, SimpleCycle<Y_EXP_E>, EmptyCycles<1>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/6, /*acc1*/3, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmac_bfp16__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
InstrItinData<II_VMSC_f_vmac_bfp16, [PrefixCycle<FEY_RW_L0_PORT>, InstrStage<2, [VCTRL_IN], 0>, InstrStage<2, [VCTRL_SUB0_CG1U], 0>, InstrStage<2, [X_EGWL], 0>, InstrStage<2, [X_EXP_ELL], 0>, InstrStage<2, [Y_EGY], 1>, PrefixCycle<FEY_RW_L0_PORT>, SimpleCycle<Y_EXP_E>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFSHCH__ACC1>, SimpleCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*s1l*/1, /*s2l*/1, /*s2h*/2, /*s1h*/2, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*s1l*/NoBypass, /*s2l*/NoBypass, /*s2h*/NoBypass, /*s1h*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
InstrItinData<II_VMUL_vmul_vmul_cm_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
InstrItinData<II_VMUL_vmul_vmul_cm_core_X_QY, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
InstrItinData<II_VMUL_vmul_vmul_cm_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
InstrItinData<II_VMUL_vmul_vmul_cm_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
InstrItinData<II_VMUL_vmul_vmul_cm_core_X_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
InstrItinData<II_VMUL_vmul_vmul_cm_core_Y_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
InstrItinData<II_VMUL_vmul_vmul_cm_core_Y_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
InstrItinData<II_VMUL_vmul_vmul_cm_core_Y_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
InstrItinData<II_VMUL_vmul_vmul_cm_core_Y_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
InstrItinData<II_VMUL_vmul_half_vmul_cm_half_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<PSA_MMODEH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
InstrItinData<II_VMUL_vmul_half_vmul_cm_half_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<PSA_MMODEH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
InstrItinData<II_VMUL_vmul_half_vmul_cm_half_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<PSA_MMODEH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bf__mDMv__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
InstrItinData<II_VMUL_f_vmul_bf_vmul_bf_core_X_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bf__mDMv__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
InstrItinData<II_VMUL_f_vmul_bf_vmul_bf_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bf__mDMv__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
InstrItinData<II_VMUL_f_vmul_bf_vmul_bf_core_Y_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bf__mDMv__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
InstrItinData<II_VMUL_f_vmul_bf_vmul_bf_core_Y_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bf_half__mCMLv__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
InstrItinData<II_VMUL_f_vmul_bf_half_vmul_bf_core_X_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bf_half__mCMLv__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
InstrItinData<II_VMUL_f_vmul_bf_half_vmul_bf_core_X_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bf_half__mCMLv__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
InstrItinData<II_VMUL_f_vmul_bf_half_vmul_bf_core_Y_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bf_half__mCMLv__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
InstrItinData<II_VMUL_f_vmul_bf_half_vmul_bf_half_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bfp13__mDMv__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
InstrItinData<II_VMUL_f_vmul_bfp13, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, PrefixCycle<X_EXP_ELL>, SimpleCycle<Y_EXP_E>, EmptyCycles<1>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bfp16__mDMv__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
InstrItinData<II_VMUL_f_vmul_bfp16, [PrefixCycle<FEY_RW_L0_PORT>, InstrStage<2, [VCTRL_IN], 0>, InstrStage<2, [VCTRL_SUB0_CG1U], 0>, InstrStage<2, [X_EGWL], 0>, InstrStage<2, [X_EXP_ELL], 0>, InstrStage<2, [Y_EGY], 1>, PrefixCycle<FEY_RW_L0_PORT>, SimpleCycle<Y_EXP_E>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFSHCH__ACC1>, SimpleCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*s1l*/1, /*s2l*/1, /*s2h*/2, /*s1h*/2, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*s1l*/NoBypass, /*s2l*/NoBypass, /*s2h*/NoBypass, /*s1h*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
InstrItinData<II_VNEGMUL_vmul_vmul_cm_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
InstrItinData<II_VNEGMUL_vmul_vmul_cm_core_X_QY, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
InstrItinData<II_VNEGMUL_vmul_vmul_cm_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
InstrItinData<II_VNEGMUL_vmul_vmul_cm_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
InstrItinData<II_VNEGMUL_vmul_vmul_cm_core_X_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
InstrItinData<II_VNEGMUL_vmul_vmul_cm_core_Y_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
InstrItinData<II_VNEGMUL_vmul_vmul_cm_core_Y_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
InstrItinData<II_VNEGMUL_vmul_vmul_cm_core_Y_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
InstrItinData<II_VNEGMUL_vmul_vmul_cm_core_Y_Y, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
InstrItinData<II_VNEGMUL_vmul_half_vmul_cm_half_core_X_QX, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<PSA_MMODEH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1, /*srSparse_of*/2], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
InstrItinData<II_VNEGMUL_vmul_half_vmul_cm_half_core_X_R, [PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<PSA_MMODEH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
InstrItinData<II_VNEGMUL_vmul_half_vmul_cm_half_core_X_X, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<1>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<PSA_MMODEH>, SimpleCycle<ZACC_DML>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bf__mDMv__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
InstrItinData<II_VNEGMUL_f_vmul_bf_vmul_bf_core_X_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bf__mDMv__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
InstrItinData<II_VNEGMUL_f_vmul_bf_vmul_bf_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bf__mDMv__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
InstrItinData<II_VNEGMUL_f_vmul_bf_vmul_bf_core_Y_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bf__mDMv__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
InstrItinData<II_VNEGMUL_f_vmul_bf_vmul_bf_core_Y_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bf_half__mCMLv__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
InstrItinData<II_VNEGMUL_f_vmul_bf_half_vmul_bf_core_X_R, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<L_RS_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bf_half__mCMLv__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
InstrItinData<II_VNEGMUL_f_vmul_bf_half_vmul_bf_core_X_Y, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bf_half__mCMLv__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
InstrItinData<II_VNEGMUL_f_vmul_bf_half_vmul_bf_core_Y_QY, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, SimpleCycle<Y_EGY>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*srSparse_of*/2, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bf_half__mCMLv__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
InstrItinData<II_VNEGMUL_f_vmul_bf_half_vmul_bf_half_core_X_X, [PrefixCycle<CRBF8CONF_RV>, PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, SimpleCycle<VCTRL_SUB0_CG1U>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/8, /*crBF8conf*/1, /*crFP8conf*/1, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bfp13__mDMv__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
InstrItinData<II_VNEGMUL_f_vmul_bfp13, [PrefixCycle<FEY_RW_L0_PORT>, PrefixCycle<VCTRL_IN>, PrefixCycle<VCTRL_SUB0_CG1U>, PrefixCycle<X_EXP_ELL>, SimpleCycle<Y_EXP_E>, EmptyCycles<1>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<CRSHMODEH__DUMMY>, PrefixCycle<CRSHMODEL__DUMMY>, SimpleCycle<DM_RV_L0>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/6, /*s1*/1, /*s2*/1, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7], [/*dst*/VEC_Bypass, /*s1*/NoBypass, /*s2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vmul_bfp16__mDMv__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
InstrItinData<II_VNEGMUL_f_vmul_bfp16, [PrefixCycle<FEY_RW_L0_PORT>, InstrStage<2, [VCTRL_IN], 0>, InstrStage<2, [VCTRL_SUB0_CG1U], 0>, InstrStage<2, [X_EGWL], 0>, InstrStage<2, [X_EXP_ELL], 0>, InstrStage<2, [Y_EGY], 1>, PrefixCycle<FEY_RW_L0_PORT>, SimpleCycle<Y_EXP_E>, PrefixCycle<CRSHMODEH__DUMMY>, SimpleCycle<CRSHMODEL__DUMMY>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<BFPEXT__A>, PrefixCycle<DM_RV_L0>, PrefixCycle<PSA_MMODEH>, PrefixCycle<PSA_RSRC>, SimpleCycle<PSA_SUMH__A>, PrefixCycle<BFSHCH__ACC1>, SimpleCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, SimpleCycle<PSA_ACCL_IN>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*s1l*/1, /*s2l*/1, /*s2h*/2, /*s1h*/2, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*s1l*/NoBypass, /*s2l*/NoBypass, /*s2h*/NoBypass, /*s1h*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_not__vneg_gtz__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_nX
InstrItinData<II_VNEG_GTZ_16, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_not__vneg_gtz__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_nX
InstrItinData<II_VNEG_GTZ_32, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_not__vneg_gtz__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_nX
InstrItinData<II_VNEG_GTZ_8, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r17_r16*/2, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*r17_r16*/NoBypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__vec__vneg__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vacc_cm_core
InstrItinData<II_VNEG_vneg, [SimpleCycle<VCTRL_IN>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vneg_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vacc_cm_half_core
InstrItinData<II_VNEG_vneg_half, [SimpleCycle<VCTRL_IN>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vneg_fp__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vacc_bf_core
InstrItinData<II_VNEG_f_vneg_fp, [SimpleCycle<VCTRL_IN>, EmptyCycles<2>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vneg_half_fp__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vacc_bf_half_core
InstrItinData<II_VNEG_f_vneg_half_fp, [SimpleCycle<VCTRL_IN>, EmptyCycles<2>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vneg_fp_bfp13__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vacc_bfp13_core
InstrItinData<II_VNEG_mx6_vneg_fp_bfp13, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<ACC2_SHH>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/6, /*acc1*/3, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vneg_half_fp_bfp13__mCMLv__vmac_acc1_add_half__vmac_acc1_comp0__add_half__mCMLv__vacc_half_bfp13_core
InstrItinData<II_VNEG_mx6_vneg_half_fp_bfp13, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<ACC2_SLH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/6, /*acc1*/3, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc*/NoBypass]>,

// id: me__instr128_or__st__mv_conv__mv_pack_w__packSign0
InstrItinData<II_VPACK_mv_pack_w_packSign0, [SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/2, /*src*/1, /*crPackSize*/1, /*crSat*/1, /*packSign0*/1]>,

// id: me__instr128_or__st__mv_conv__mv_pack_w__packSign1
InstrItinData<II_VPACK_mv_pack_w_packSign1, [SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/2, /*src*/1, /*crPackSize*/1, /*crSat*/1, /*packSign1*/1]>,

// id: me__instr128_or__st__mv_conv__mv_pack_x__mYs__packSign0
InstrItinData<II_VPACK_mv_pack_x_packSign0, [SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/2, /*src*/1, /*crPackSize*/1, /*crSat*/1, /*packSign0*/1]>,

// id: me__instr128_or__st__mv_conv__mv_pack_x__mYs__packSign1
InstrItinData<II_VPACK_mv_pack_x_packSign1, [SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/2, /*src*/1, /*crPackSize*/1, /*crSat*/1, /*packSign1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_hi__vec_push_hi_R__s2v_wide_16
InstrItinData<II_VPUSH_hi_16, [], [/*d*/2, /*s1*/1, /*s2*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_hi__vec_push_hi_R__s2v_wide_32
InstrItinData<II_VPUSH_hi_32, [], [/*d*/2, /*s1*/1, /*s2*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_hi__vec_push_hi_L__s2v_wide_64__mLm
InstrItinData<II_VPUSH_hi_64, [], [/*d*/2, /*s1*/1, /*s2*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_hi__vec_push_hi_R__s2v_wide_8
InstrItinData<II_VPUSH_hi_8, [], [/*d*/2, /*s1*/1, /*s2*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_lo__vec_push_lo_R__s2v_wide_16
InstrItinData<II_VPUSH_lo_16, [], [/*d*/2, /*s1*/1, /*s2*/1], [/*d*/MV_Bypass, /*s1*/NoBypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_lo__vec_push_lo_R__s2v_wide_32
InstrItinData<II_VPUSH_lo_32, [], [/*d*/2, /*s1*/1, /*s2*/1], [/*d*/MV_Bypass, /*s1*/NoBypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_lo__vec_push_lo_L__s2v_wide_64__mLm
InstrItinData<II_VPUSH_lo_64, [], [/*d*/2, /*s1*/1, /*s2*/1], [/*d*/MV_Bypass, /*s1*/NoBypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_lo__vec_push_lo_R__s2v_wide_8
InstrItinData<II_VPUSH_lo_8, [], [/*d*/2, /*s1*/1, /*s2*/1], [/*d*/MV_Bypass, /*s1*/NoBypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vsel__vec_mask_in__vec_mask_in32__vec_add_mX__vec_add_nX
InstrItinData<II_VSEL_16, [SimpleCycle<CRBF8CONF_RV>], [/*d*/2, /*s1*/1, /*s2*/1, /*sel*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass, /*sel*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vsel__vec_mask_in__vec_mask_in16__vec_add_mX__vec_add_nX
InstrItinData<II_VSEL_32, [SimpleCycle<CRBF8CONF_RV>], [/*d*/2, /*s1*/1, /*s2*/1, /*sel*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass, /*sel*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vsel__vec_mask_in__vec_mask_in64__mLm__vec_add_mX__vec_add_nX
InstrItinData<II_VSEL_8, [SimpleCycle<CRBF8CONF_RV>], [/*d*/2, /*s1*/1, /*s2*/1, /*sel*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass, /*sel*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_shift_align__mSm__mR30_shiftx
InstrItinData<II_VSHIFT_ALIGN, [], [/*d*/2, /*s1*/1, /*pre*/1, /*s2*/1, /*shift*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*pre*/NoBypass, /*s2*/MV_Bypass, /*shift*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_shift
InstrItinData<II_VSHIFT, [], [/*d*/2, /*s1*/1, /*s2*/1, /*shift*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass, /*shift*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_shuffle_bm
InstrItinData<II_VSHUFFLE_vec_shuffle_bm, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*s1*/1, /*s2*/1, /*mod*/1], [/*dst*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass, /*mod*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_shuffle_egx
InstrItinData<II_VSHUFFLE_vec_shuffle_egx, [], [/*dst*/2, /*s1*/1, /*s2*/1, /*mod*/1], [/*dst*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass, /*mod*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_shuffle_fex
InstrItinData<II_VSHUFFLE_vec_shuffle_fex, [], [/*dst*/2, /*s1*/1, /*s2*/1, /*mod*/1], [/*dst*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass, /*mod*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_shuffle_x
InstrItinData<II_VSHUFFLE_vec_shuffle_x, [], [/*dst*/2, /*s1*/1, /*s2*/1, /*mod*/1], [/*dst*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass, /*mod*/NoBypass]>,

// id: me__instr128_or__st__mv_conv__mv_w_srs_bm__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
InstrItinData<II_VSRS_2x_mv_w_srs_bm_srsSign0, [EmptyCycles<3>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/4, /*src*/1, /*base*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1]>,

// id: me__instr128_or__st__mv_conv__mv_w_srs_bm__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
InstrItinData<II_VSRS_2x_mv_w_srs_bm_srsSign1, [EmptyCycles<3>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/4, /*src*/1, /*base*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1]>,

// id: me__instr128_or__st__mv_conv__mv_x_srs_cm__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
InstrItinData<II_VSRS_2x_mv_x_srs_cm_srsSign0, [EmptyCycles<3>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/4, /*src*/1, /*base*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1]>,

// id: me__instr128_or__st__mv_conv__mv_x_srs_cm__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
InstrItinData<II_VSRS_2x_mv_x_srs_cm_srsSign1, [EmptyCycles<3>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/4, /*src*/1, /*base*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1]>,

// id: me__instr128_or__st__mv_conv__mv_w_srs_cm__srs__mWssrs__srs__baseSrs__mSs__srsSign0
InstrItinData<II_VSRS_4x_mv_w_srs_cm_srsSign0, [EmptyCycles<3>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/4, /*src*/1, /*base*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1]>,

// id: me__instr128_or__st__mv_conv__mv_w_srs_cm__srs__mWssrs__srs__baseSrs__mSs__srsSign1
InstrItinData<II_VSRS_4x_mv_w_srs_cm_srsSign1, [EmptyCycles<3>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/4, /*src*/1, /*base*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1]>,

// id: me__instr128_or__st__mv_conv__mv_x_srs_dm__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
InstrItinData<II_VSRS_4x_mv_x_srs_dm_srsSign0, [EmptyCycles<3>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/4, /*src*/1, /*base*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1]>,

// id: me__instr128_or__st__mv_conv__mv_x_srs_dm__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
InstrItinData<II_VSRS_4x_mv_x_srs_dm_srsSign1, [EmptyCycles<3>, SimpleCycle<FEY_WA_L0_PORT>], [/*dst*/4, /*src*/1, /*base*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1]>,

// id: me__instr128_or__st__dmv_sts_eg__spill__agus_dmv__nrm_spill__agus_dmv__spill__mSPs__m_c14n_step16
MemInstrItinData<II_VST_128_dmv_sts_eg_spill, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_eg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx__mPs__mDJs
MemInstrItinData<II_VST_128_dmv_sts_eg_st_idx, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_eg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx_imm__mPs__m_c08s_step16
MemInstrItinData<II_VST_128_dmv_sts_eg_st_idx_imm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_eg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm__mPs__mMs
MemInstrItinData<II_VST_128_dmv_sts_eg_st_pstm_nrm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_eg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm_imm__mPs__m_c08s_step16
MemInstrItinData<II_VST_128_dmv_sts_eg_st_pstm_nrm_imm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_feg__spill__agus_dmv__nrm_spill__agus_dmv__spill__mSPs__m_c14n_step16
MemInstrItinData<II_VST_128_dmv_sts_feg_spill, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_feg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx__mPs__mDJs
MemInstrItinData<II_VST_128_dmv_sts_feg_st_idx, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_feg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx_imm__mPs__m_c08s_step16
MemInstrItinData<II_VST_128_dmv_sts_feg_st_idx_imm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_feg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm__mPs__mMs
MemInstrItinData<II_VST_128_dmv_sts_feg_st_pstm_nrm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_feg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm_imm__mPs__m_c08s_step16
MemInstrItinData<II_VST_128_dmv_sts_feg_st_pstm_nrm_imm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_w__spill__agus_dmv__nrm_spill__agus_dmv__spill__mSPs__m_c14n_step16
MemInstrItinData<II_VST_128_dmv_sts_w_spill, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_w__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx__mPs__mDJs
MemInstrItinData<II_VST_128_dmv_sts_w_st_idx, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_w__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx_imm__mPs__m_c08s_step16
MemInstrItinData<II_VST_128_dmv_sts_w_st_idx_imm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_w__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm__mPs__mMs
MemInstrItinData<II_VST_128_dmv_sts_w_st_pstm_nrm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_w__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm_imm__mPs__m_c08s_step16
MemInstrItinData<II_VST_128_dmv_sts_w_st_pstm_nrm_imm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_eg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_2d__mPs__mDs
MemInstrItinData<II_VST_2D_128_dmv_sts_eg, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_feg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_2d__mPs__mDs
MemInstrItinData<II_VST_2D_128_dmv_sts_feg, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_w__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_2d__mPs__mDs
MemInstrItinData<II_VST_2D_128_dmv_sts_w, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_srs_bf16__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__srs_f__mW
MemInstrItinData<II_VST_2D_CONV_bf16_fp32_dmw_sts_srs_bf16, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_srs_bf16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__srs_f__mX
MemInstrItinData<II_VST_2D_CONV_bf16_fp32_dmx_sts_srs_bf16, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmw_sts_srs_bf8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__srs_f__mXl__mSs
MemInstrItinData<II_VST_2D_CONV_bf8_fp32, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*i*/1, /*ptr*/1, /*mod*/1, /*srF2FFlags*/3, /*crBF8conf*/1, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_srs_fp16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__srs_f__mX
MemInstrItinData<II_VST_2D_CONV_fp16_fp32, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmw_sts_srs_fp8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__srs_f__mXl__mSs
MemInstrItinData<II_VST_2D_CONV_fp8_fp32, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*i*/1, /*ptr*/1, /*mod*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFP8conf*/1, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__dmw_sts_pack_core__packSign0
MemInstrItinData<II_VST_2D_PACK_dmw_sts_pack_packSign0, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*crPackSize*/1, /*crSat*/1, /*packSign0*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__dmw_sts_pack_core__packSign1
MemInstrItinData<II_VST_2D_PACK_dmw_sts_pack_packSign1, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*crPackSize*/1, /*crSat*/1, /*packSign1*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__dmx_sts_pack_core__mYs__packSign0
MemInstrItinData<II_VST_2D_PACK_dmx_sts_pack_packSign0, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*crPackSize*/1, /*crSat*/1, /*packSign0*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__dmx_sts_pack_core__mYs__packSign1
MemInstrItinData<II_VST_2D_PACK_dmx_sts_pack_packSign1, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*crPackSize*/1, /*crSat*/1, /*packSign1*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_2D_SRS_2x_dm_sts_srs_cm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_2D_SRS_2x_dm_sts_srs_cm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__srs__mWssrs__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_2D_SRS_4x_dm_sts_srs_cm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__srs__mWssrs__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_2D_SRS_4x_dm_sts_srs_cm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmw_sts_feg2__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs
MemInstrItinData<II_VST_2D_dmw_sts_feg2, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_w__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs
MemInstrItinData<II_VST_2D_dmw_sts_w, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_bm__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs
MemInstrItinData<II_VST_2D_dmx_sts_bm, [AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_x__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs
MemInstrItinData<II_VST_2D_dmx_sts_x, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dc*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_eg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_3d__mPs__mDSs
MemInstrItinData<II_VST_3D_128_dmv_sts_eg, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_feg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_3d__mPs__mDSs
MemInstrItinData<II_VST_3D_128_dmv_sts_feg, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmv_sts_w__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_3d__mPs__mDSs
MemInstrItinData<II_VST_3D_128_dmv_sts_w, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_srs_bf16__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__srs_f__mW
MemInstrItinData<II_VST_3D_CONV_bf16_fp32_dmw_sts_srs_bf16, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_srs_bf16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__srs_f__mX
MemInstrItinData<II_VST_3D_CONV_bf16_fp32_dmx_sts_srs_bf16, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmw_sts_srs_bf8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__srs_f__mXl__mSs
MemInstrItinData<II_VST_3D_CONV_bf8_fp32, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*i*/1, /*ptr*/1, /*mod*/1, /*srF2FFlags*/3, /*crBF8conf*/1, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_srs_fp16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__srs_f__mX
MemInstrItinData<II_VST_3D_CONV_fp16_fp32, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmw_sts_srs_fp8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__srs_f__mXl__mSs
MemInstrItinData<II_VST_3D_CONV_fp8_fp32, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*i*/1, /*ptr*/1, /*mod*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFP8conf*/1, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__dmw_sts_pack_core__packSign0
MemInstrItinData<II_VST_3D_PACK_dmw_sts_pack_packSign0, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*crPackSize*/1, /*crSat*/1, /*packSign0*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__dmw_sts_pack_core__packSign1
MemInstrItinData<II_VST_3D_PACK_dmw_sts_pack_packSign1, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*crPackSize*/1, /*crSat*/1, /*packSign1*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__dmx_sts_pack_core__mYs__packSign0
MemInstrItinData<II_VST_3D_PACK_dmx_sts_pack_packSign0, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*crPackSize*/1, /*crSat*/1, /*packSign0*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__dmx_sts_pack_core__mYs__packSign1
MemInstrItinData<II_VST_3D_PACK_dmx_sts_pack_packSign1, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*crPackSize*/1, /*crSat*/1, /*packSign1*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_3D_SRS_2x_dm_sts_srs_cm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_3D_SRS_2x_dm_sts_srs_cm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__srs__mWssrs__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_3D_SRS_4x_dm_sts_srs_cm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__srs__mWssrs__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_3D_SRS_4x_dm_sts_srs_cm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmw_sts_feg2__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs
MemInstrItinData<II_VST_3D_dmw_sts_feg2, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_w__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs
MemInstrItinData<II_VST_3D_dmw_sts_w, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_bm__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs
MemInstrItinData<II_VST_3D_dmx_sts_bm, [AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_x__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs
MemInstrItinData<II_VST_3D_dmx_sts_x, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*dcl*/1, /*dch*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_srs_bf16__agus_dmw__fifo__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__srs_f__mW
MemInstrItinData<II_VST_CONV_bf16_fp32_dmw_sts_srs_bf16_idx, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmw_sts_srs_bf16__agus_dmw__fifo__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__srs_f__mW
MemInstrItinData<II_VST_CONV_bf16_fp32_dmw_sts_srs_bf16_idx_imm, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmw_sts_srs_bf16__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__srs_f__mW
MemInstrItinData<II_VST_CONV_bf16_fp32_dmw_sts_srs_bf16_pstm_nrm, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmw_sts_srs_bf16__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__srs_f__mW
MemInstrItinData<II_VST_CONV_bf16_fp32_dmw_sts_srs_bf16_pstm_nrm_imm, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_srs_bf16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__srs_f__mX
MemInstrItinData<II_VST_CONV_bf16_fp32_dmx_sts_srs_bf16_idx, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_srs_bf16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__srs_f__mX
MemInstrItinData<II_VST_CONV_bf16_fp32_dmx_sts_srs_bf16_idx_imm, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_srs_bf16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__srs_f__mX
MemInstrItinData<II_VST_CONV_bf16_fp32_dmx_sts_srs_bf16_pstm_nrm, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_srs_bf16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__srs_f__mX
MemInstrItinData<II_VST_CONV_bf16_fp32_dmx_sts_srs_bf16_pstm_nrm_imm, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmw_sts_srs_bf8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__srs_f__mXl__mSs
MemInstrItinData<II_VST_CONV_bf8_fp32_idx, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*src*/1, /*i*/1, /*ptr*/1, /*dj*/1, /*srF2FFlags*/3, /*crBF8conf*/1, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmw_sts_srs_bf8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__srs_f__mXl__mSs
MemInstrItinData<II_VST_CONV_bf8_fp32_idx_imm, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*src*/1, /*i*/1, /*ptr*/1, /*imm*/1, /*srF2FFlags*/3, /*crBF8conf*/1, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmw_sts_srs_bf8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__srs_f__mXl__mSs
MemInstrItinData<II_VST_CONV_bf8_fp32_pstm_nrm, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*i*/1, /*ptr*/1, /*mod*/1, /*srF2FFlags*/3, /*crBF8conf*/1, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmw_sts_srs_bf8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__srs_f__mXl__mSs
MemInstrItinData<II_VST_CONV_bf8_fp32_pstm_nrm_imm, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*i*/1, /*ptr*/1, /*imm*/1, /*srF2FFlags*/3, /*crBF8conf*/1, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_srs_fp16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__srs_f__mX
MemInstrItinData<II_VST_CONV_fp16_fp32_idx, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_srs_fp16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__srs_f__mX
MemInstrItinData<II_VST_CONV_fp16_fp32_idx_imm, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_srs_fp16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__srs_f__mX
MemInstrItinData<II_VST_CONV_fp16_fp32_pstm_nrm, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_srs_fp16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__srs_f__mX
MemInstrItinData<II_VST_CONV_fp16_fp32_pstm_nrm_imm, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmw_sts_srs_fp8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__srs_f__mXl__mSs
MemInstrItinData<II_VST_CONV_fp8_fp32_idx, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*src*/1, /*i*/1, /*ptr*/1, /*dj*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFP8conf*/1, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmw_sts_srs_fp8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__srs_f__mXl__mSs
MemInstrItinData<II_VST_CONV_fp8_fp32_idx_imm, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*src*/1, /*i*/1, /*ptr*/1, /*imm*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFP8conf*/1, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmw_sts_srs_fp8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__srs_f__mXl__mSs
MemInstrItinData<II_VST_CONV_fp8_fp32_pstm_nrm, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*i*/1, /*ptr*/1, /*mod*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFP8conf*/1, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmw_sts_srs_fp8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__srs_f__mXl__mSs
MemInstrItinData<II_VST_CONV_fp8_fp32_pstm_nrm_imm, [EmptyCycles<1>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*i*/1, /*ptr*/1, /*imm*/1, /*srF2FFlags*/3, /*crF2FMask*/2, /*crFP8conf*/1, /*crFPConvSat*/2, /*crRnd*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_x__fifo_2d_flush__dmx_sts_fifo_bare_x__core_flush__mStFifo__dmx_sts_fifo_bare_x__fifo_2d__mPfs__mR26_fifo_st__mDs
MemInstrItinData<II_VST_FLUSH_2D, [SimpleCycle<FIFO_STS__I_XH_S>, AvoidPartWordStore, SimpleCycle<FIFO_STS__ADVANCE_FIFO_R>, PrefixCycle<DMX_STS_FIFO>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*fifo_reg_out*/2, /*ptr_out*/1, /*avail_out*/1, /*dc*/1, /*fifo_reg*/2, /*ptr*/1, /*avail*/1, /*mod*/1, /*srFifo_of*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_x__fifo_3d_flush__dmx_sts_fifo_bare_x__core_flush__mStFifo__dmx_sts_fifo_bare_x__fifo_3d__mPfs__mR26_fifo_st__mDSs
MemInstrItinData<II_VST_FLUSH_3D, [SimpleCycle<FIFO_STS__I_XH_S>, AvoidPartWordStore, SimpleCycle<FIFO_STS__ADVANCE_FIFO_R>, PrefixCycle<DMX_STS_FIFO>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*fifo_reg_out*/2, /*ptr_out*/1, /*avail_out*/1, /*dcl*/1, /*dch*/1, /*fifo_reg*/2, /*ptr*/1, /*avail*/1, /*mod*/1, /*srFifo_of*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_x__fifo_1d_flush__dmx_sts_fifo_bare_x__core_flush__mStFifo__dmx_sts_fifo_bare_x__fifo_1d__mPfs__mR26_fifo_st__mMs
MemInstrItinData<II_VST_FLUSH_fifo_1d_flush, [SimpleCycle<FIFO_STS__I_XH_S>, AvoidPartWordStore, SimpleCycle<FIFO_STS__ADVANCE_FIFO_R>, PrefixCycle<DMX_STS_FIFO>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*fifo_reg_out*/2, /*ptr_out*/1, /*avail_out*/1, /*fifo_reg*/2, /*ptr*/1, /*avail*/1, /*mod*/1, /*srFifo_of*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_x__normal_flush__dmx_sts_fifo_bare_x__core_flush__mStFifo__dmx_sts_fifo_bare_x__normal__mPfs__mR26_fifo_st
MemInstrItinData<II_VST_FLUSH_normal_flush, [SimpleCycle<FIFO_STS__I_XH_S>, AvoidPartWordStore, SimpleCycle<FIFO_STS__ADVANCE_FIFO_R>, PrefixCycle<DMX_STS_FIFO>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*fifo_reg_out*/2, /*ptr_out*/1, /*avail_out*/1, /*fifo_reg*/2, /*ptr*/1, /*avail*/1, /*srFifo_of*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_conv_ex_ebs16__fifo_2d_flush__dmx_sts_fifo_conv_ex_ebs16__core_flush__mStFifo__dmx_sts_fifo_conv_ex_ebs16__fifo_2d__mPfs__mR26_fifo_st__mDs
MemInstrItinData<II_VST_FLUSH_d_2D, [EmptyCycles<2>, SimpleCycle<FIFO_STS__I_XH_S>, AvoidPartWordStore, SimpleCycle<FIFO_STS__ADVANCE_FIFO_R>, PrefixCycle<DMX_STS_FIFO>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*fifo_reg_out*/4, /*ptr_out*/1, /*avail_out*/1, /*dc*/1, /*fifo_reg*/4, /*ptr*/1, /*avail*/1, /*mod*/1, /*srFifo_of*/2], MemoryCycles<[8]>>,

// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_conv_ex_ebs16__fifo_3d_flush__dmx_sts_fifo_conv_ex_ebs16__core_flush__mStFifo__dmx_sts_fifo_conv_ex_ebs16__fifo_3d__mPfs__mR26_fifo_st__mDSs
MemInstrItinData<II_VST_FLUSH_d_3D, [EmptyCycles<2>, SimpleCycle<FIFO_STS__I_XH_S>, AvoidPartWordStore, SimpleCycle<FIFO_STS__ADVANCE_FIFO_R>, PrefixCycle<DMX_STS_FIFO>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*fifo_reg_out*/4, /*ptr_out*/1, /*avail_out*/1, /*dcl*/1, /*dch*/1, /*fifo_reg*/4, /*ptr*/1, /*avail*/1, /*mod*/1, /*srFifo_of*/2], MemoryCycles<[8]>>,

// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_conv_ex_ebs16__fifo_1d_flush__dmx_sts_fifo_conv_ex_ebs16__core_flush__mStFifo__dmx_sts_fifo_conv_ex_ebs16__fifo_1d__mPfs__mR26_fifo_st__mMs
MemInstrItinData<II_VST_FLUSH_d_fifo_1d_flush, [EmptyCycles<2>, SimpleCycle<FIFO_STS__I_XH_S>, AvoidPartWordStore, SimpleCycle<FIFO_STS__ADVANCE_FIFO_R>, PrefixCycle<DMX_STS_FIFO>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*fifo_reg_out*/4, /*ptr_out*/1, /*avail_out*/1, /*fifo_reg*/4, /*ptr*/1, /*avail*/1, /*mod*/1, /*srFifo_of*/2], MemoryCycles<[8]>>,

// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_conv_ex_ebs16__normal_flush__dmx_sts_fifo_conv_ex_ebs16__core_flush__mStFifo__dmx_sts_fifo_conv_ex_ebs16__normal__mPfs__mR26_fifo_st
MemInstrItinData<II_VST_FLUSH_d_normal_flush, [EmptyCycles<2>, SimpleCycle<FIFO_STS__I_XH_S>, AvoidPartWordStore, SimpleCycle<FIFO_STS__ADVANCE_FIFO_R>, PrefixCycle<DMX_STS_FIFO>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*fifo_reg_out*/4, /*ptr_out*/1, /*avail_out*/1, /*fifo_reg*/4, /*ptr*/1, /*avail*/1, /*srFifo_of*/2], MemoryCycles<[8]>>,

// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__dmw_sts_pack_core__packSign0
MemInstrItinData<II_VST_PACK_dmw_sts_pack_idx_packSign0, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1, /*crPackSize*/1, /*crSat*/1, /*packSign0*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__dmw_sts_pack_core__packSign1
MemInstrItinData<II_VST_PACK_dmw_sts_pack_idx_packSign1, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1, /*crPackSize*/1, /*crSat*/1, /*packSign1*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__dmw_sts_pack_core__packSign0
MemInstrItinData<II_VST_PACK_dmw_sts_pack_idx_imm_packSign0, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1, /*crPackSize*/1, /*crSat*/1, /*packSign0*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__dmw_sts_pack_core__packSign1
MemInstrItinData<II_VST_PACK_dmw_sts_pack_idx_imm_packSign1, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1, /*crPackSize*/1, /*crSat*/1, /*packSign1*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__dmw_sts_pack_core__packSign0
MemInstrItinData<II_VST_PACK_dmw_sts_pack_pstm_nrm_packSign0, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*crPackSize*/1, /*crSat*/1, /*packSign0*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__dmw_sts_pack_core__packSign1
MemInstrItinData<II_VST_PACK_dmw_sts_pack_pstm_nrm_packSign1, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*crPackSize*/1, /*crSat*/1, /*packSign1*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__dmw_sts_pack_core__packSign0
MemInstrItinData<II_VST_PACK_dmw_sts_pack_pstm_nrm_imm_packSign0, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1, /*crPackSize*/1, /*crSat*/1, /*packSign0*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__dmw_sts_pack_core__packSign1
MemInstrItinData<II_VST_PACK_dmw_sts_pack_pstm_nrm_imm_packSign1, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1, /*crPackSize*/1, /*crSat*/1, /*packSign1*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__dmx_sts_pack_core__mYs__packSign0
MemInstrItinData<II_VST_PACK_dmx_sts_pack_idx_packSign0, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1, /*crPackSize*/1, /*crSat*/1, /*packSign0*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__dmx_sts_pack_core__mYs__packSign1
MemInstrItinData<II_VST_PACK_dmx_sts_pack_idx_packSign1, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1, /*crPackSize*/1, /*crSat*/1, /*packSign1*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__dmx_sts_pack_core__mYs__packSign0
MemInstrItinData<II_VST_PACK_dmx_sts_pack_idx_imm_packSign0, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1, /*crPackSize*/1, /*crSat*/1, /*packSign0*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__dmx_sts_pack_core__mYs__packSign1
MemInstrItinData<II_VST_PACK_dmx_sts_pack_idx_imm_packSign1, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1, /*crPackSize*/1, /*crSat*/1, /*packSign1*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__dmx_sts_pack_core__mYs__packSign0
MemInstrItinData<II_VST_PACK_dmx_sts_pack_pstm_nrm_packSign0, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*crPackSize*/1, /*crSat*/1, /*packSign0*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__dmx_sts_pack_core__mYs__packSign1
MemInstrItinData<II_VST_PACK_dmx_sts_pack_pstm_nrm_packSign1, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1, /*crPackSize*/1, /*crSat*/1, /*packSign1*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__dmx_sts_pack_core__mYs__packSign0
MemInstrItinData<II_VST_PACK_dmx_sts_pack_pstm_nrm_imm_packSign0, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1, /*crPackSize*/1, /*crSat*/1, /*packSign0*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__dmx_sts_pack_core__mYs__packSign1
MemInstrItinData<II_VST_PACK_dmx_sts_pack_pstm_nrm_imm_packSign1, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1, /*crPackSize*/1, /*crSat*/1, /*packSign1*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_conv_few_bfp11__normal_push__dmx_sts_fifo_conv_few_bfp11__core_push__bfp__mFp2Bfp11__bfp__basis__mDMs__mStFifo__dmx_sts_fifo_conv_few_bfp11__normal__mPfs__mR26_fifo_st
MemInstrItinData<II_VST_PUSH_256_CONV_mx4_fp32, [EmptyCycles<3>, AvoidPartWordStore, SimpleCycle<FIFO_STS__ADVANCE_FIFO_R>, PrefixCycle<DMX_STS_FIFO>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*fifo_reg_out*/4, /*ptr_out*/1, /*avail_out*/1, /*fifo_reg*/4, /*src*/1, /*ptr*/1, /*avail*/1, /*srF2BFlags*/3, /*srFifo_of*/2, /*crF2BMask*/3, /*crRnd*/2], MemoryCycles<[8]>>,

// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_few_bfp11__normal_push__dmx_sts_fifo_bare_few_bfp11__core_push__mStFifo__dmx_sts_fifo_bare_few_bfp11__normal__mPfs__mR26_fifo_st
MemInstrItinData<II_VST_PUSH_256, [SimpleCycle<FEY_RS_L0_PORT>, AvoidPartWordStore, SimpleCycle<FIFO_STS__ADVANCE_FIFO_R>, PrefixCycle<DMX_STS_FIFO>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*fifo_reg_out*/2, /*ptr_out*/1, /*avail_out*/1, /*fifo_reg*/2, /*src*/1, /*ptr*/1, /*avail*/1, /*srFifo_of*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_conv_few__normal_push__dmx_sts_fifo_conv_few__core_push__bfp__mFp2Bfp13__bfp__basis__mDMs__mStFifo__dmx_sts_fifo_conv_few__normal__mPfs__mR26_fifo_st
MemInstrItinData<II_VST_PUSH_384_CONV_mx6_fp32, [EmptyCycles<3>, AvoidPartWordStore, SimpleCycle<FIFO_STS__ADVANCE_FIFO_R>, PrefixCycle<DMX_STS_FIFO>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*fifo_reg_out*/4, /*ptr_out*/1, /*avail_out*/1, /*fifo_reg*/4, /*src*/1, /*ptr*/1, /*avail*/1, /*srF2BFlags*/3, /*srFifo_of*/2, /*crF2BMask*/3, /*crRnd*/2], MemoryCycles<[8]>>,

// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_few__normal_push__dmx_sts_fifo_bare_few__core_push__mStFifo__dmx_sts_fifo_bare_few__normal__mPfs__mR26_fifo_st
MemInstrItinData<II_VST_PUSH_384, [SimpleCycle<FEY_RS_L0_PORT>, AvoidPartWordStore, SimpleCycle<FIFO_STS__ADVANCE_FIFO_R>, PrefixCycle<DMX_STS_FIFO>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*fifo_reg_out*/2, /*ptr_out*/1, /*avail_out*/1, /*fifo_reg*/2, /*src*/1, /*ptr*/1, /*avail*/1, /*srFifo_of*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_fex_bfp11__normal_push__dmx_sts_fifo_bare_fex_bfp11__core_push__mStFifo__dmx_sts_fifo_bare_fex_bfp11__normal__mPfs__mR26_fifo_st
MemInstrItinData<II_VST_PUSH_512_dmx_sts_fifo_bare_fex_bfp11, [SimpleCycle<FEY_RS_L0_PORT>, AvoidPartWordStore, SimpleCycle<FIFO_STS__ADVANCE_FIFO_R>, PrefixCycle<DMX_STS_FIFO>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*fifo_reg_out*/2, /*ptr_out*/1, /*avail_out*/1, /*fifo_reg*/2, /*src*/1, /*ptr*/1, /*avail*/1, /*srFifo_of*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_x__normal_push__dmx_sts_fifo_bare_x__core_push__mStFifo__dmx_sts_fifo_bare_x__normal__mPfs__mR26_fifo_st
MemInstrItinData<II_VST_PUSH_512_dmx_sts_fifo_bare_x, [SimpleCycle<FEY_RS_L0_PORT>, AvoidPartWordStore, SimpleCycle<FIFO_STS__ADVANCE_FIFO_R>, PrefixCycle<DMX_STS_FIFO>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*fifo_reg_out*/2, /*ptr_out*/1, /*avail_out*/1, /*fifo_reg*/2, /*src*/1, /*ptr*/1, /*avail*/1, /*srFifo_of*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_conv_ex_ebs16__normal_push__dmx_sts_fifo_conv_ex_ebs16__core_push__bfp__mFp2Bfp16__bfp__basis__mDMs__mStFifo__dmx_sts_fifo_conv_ex_ebs16__normal__mPfs__mR26_fifo_st
MemInstrItinData<II_VST_PUSH_576_CONV_mx9_fp32, [EmptyCycles<3>, AvoidPartWordStore, SimpleCycle<FIFO_STS__ADVANCE_FIFO_R>, PrefixCycle<DMX_STS_FIFO>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*fifo_reg_out*/4, /*ptr_out*/1, /*avail_out*/1, /*fifo_reg*/4, /*src*/1, /*ptr*/1, /*avail*/1, /*srF2BFlags*/3, /*srFifo_of*/2, /*crF2BMask*/3, /*crRnd*/2], MemoryCycles<[8]>>,

// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_ex__normal_push__dmx_sts_fifo_bare_ex__core_push__mStFifo__dmx_sts_fifo_bare_ex__normal__mPfs__mR26_fifo_st
MemInstrItinData<II_VST_PUSH_576, [SimpleCycle<FEY_RS_L0_PORT>, AvoidPartWordStore, SimpleCycle<FIFO_STS__ADVANCE_FIFO_R>, PrefixCycle<DMX_STS_FIFO>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*fifo_reg_out*/2, /*ptr_out*/1, /*avail_out*/1, /*fifo_reg*/2, /*src*/1, /*ptr*/1, /*avail*/1, /*srFifo_of*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_fex__normal_push__dmx_sts_fifo_bare_fex__core_push__mStFifo__dmx_sts_fifo_bare_fex__normal__mPfs__mR26_fifo_st
MemInstrItinData<II_VST_PUSH_768, [SimpleCycle<FEY_RS_L0_PORT>, AvoidPartWordStore, SimpleCycle<FIFO_STS__ADVANCE_FIFO_R>, PrefixCycle<DMX_STS_FIFO>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*fifo_reg_out*/2, /*ptr_out*/1, /*avail_out*/1, /*fifo_reg*/2, /*src*/1, /*ptr*/1, /*avail*/1, /*srFifo_of*/2], MemoryCycles<[6]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_SRS_2x_dm_sts_srs_cm_idx_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*base*/1, /*ptr*/1, /*dj*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_SRS_2x_dm_sts_srs_cm_idx_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*base*/1, /*ptr*/1, /*dj*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_SRS_2x_dm_sts_srs_cm_idx_imm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*base*/1, /*ptr*/1, /*imm*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_SRS_2x_dm_sts_srs_cm_idx_imm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*base*/1, /*ptr*/1, /*imm*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_imm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*base*/1, /*ptr*/1, /*imm*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_imm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*base*/1, /*ptr*/1, /*imm*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_SRS_2x_dmw_sts_srs_bm_idx_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*src*/1, /*base*/1, /*ptr*/1, /*dj*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_SRS_2x_dmw_sts_srs_bm_idx_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*src*/1, /*base*/1, /*ptr*/1, /*dj*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_SRS_2x_dmw_sts_srs_bm_idx_imm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*src*/1, /*base*/1, /*ptr*/1, /*imm*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_SRS_2x_dmw_sts_srs_bm_idx_imm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*src*/1, /*base*/1, /*ptr*/1, /*imm*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_imm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*base*/1, /*ptr*/1, /*imm*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_imm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*base*/1, /*ptr*/1, /*imm*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__srs__mWssrs__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_SRS_4x_dm_sts_srs_cm_idx_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*src*/1, /*base*/1, /*ptr*/1, /*dj*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__srs__mWssrs__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_SRS_4x_dm_sts_srs_cm_idx_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*src*/1, /*base*/1, /*ptr*/1, /*dj*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__srs__mWssrs__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_SRS_4x_dm_sts_srs_cm_idx_imm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*src*/1, /*base*/1, /*ptr*/1, /*imm*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__srs__mWssrs__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_SRS_4x_dm_sts_srs_cm_idx_imm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*src*/1, /*base*/1, /*ptr*/1, /*imm*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__srs__mWssrs__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__srs__mWssrs__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__srs__mWssrs__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_imm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*base*/1, /*ptr*/1, /*imm*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__srs__mWssrs__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_imm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*base*/1, /*ptr*/1, /*imm*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_SRS_4x_dmx_sts_srs_dm_idx_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*base*/1, /*ptr*/1, /*dj*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_SRS_4x_dmx_sts_srs_dm_idx_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*base*/1, /*ptr*/1, /*dj*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_SRS_4x_dmx_sts_srs_dm_idx_imm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*base*/1, /*ptr*/1, /*imm*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_SRS_4x_dmx_sts_srs_dm_idx_imm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*base*/1, /*ptr*/1, /*imm*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*base*/1, /*ptr*/1, /*mod*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
MemInstrItinData<II_VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_imm_srsSign0, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*base*/1, /*ptr*/1, /*imm*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign0*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
MemInstrItinData<II_VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_imm_srsSign1, [EmptyCycles<2>, AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*base*/1, /*ptr*/1, /*imm*/1, /*srSRS_of*/4, /*crRnd*/1, /*crSRSMode*/1, /*crSat*/1, /*srsSign1*/1], MemoryCycles<[7]>>,

// id: me__instr128_or__st__dmw_sts_feg2__spill__agus_dmw__nrm_spill__agus_dmw__spill__mSPs__m_c15n_step32
MemInstrItinData<II_VST_dmw_sts_feg2_spill, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_feg2__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx__mPs__mDJs
MemInstrItinData<II_VST_dmw_sts_feg2_st_idx, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_feg2__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32
MemInstrItinData<II_VST_dmw_sts_feg2_st_idx_imm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_feg2__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs
MemInstrItinData<II_VST_dmw_sts_feg2_st_pstm_nrm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_feg2__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32
MemInstrItinData<II_VST_dmw_sts_feg2_st_pstm_nrm_imm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_w__spill__agus_dmw__nrm_spill__agus_dmw__spill__mSPs__m_c15n_step32
MemInstrItinData<II_VST_dmw_sts_w_spill, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_w__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx__mPs__mDJs
MemInstrItinData<II_VST_dmw_sts_w_st_idx, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_w__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32
MemInstrItinData<II_VST_dmw_sts_w_st_idx_imm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_w__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs
MemInstrItinData<II_VST_dmw_sts_w_st_pstm_nrm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmw_sts_w__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32
MemInstrItinData<II_VST_dmw_sts_w_st_pstm_nrm_imm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_bm__spill__agus_dmx__nrm_spill__agus_dmx__spill__mSPs__m_c16n_step64
MemInstrItinData<II_VST_dmx_sts_bm_spill, [AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_bm__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx__mPs__mDJs
MemInstrItinData<II_VST_dmx_sts_bm_st_idx, [AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_bm__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64
MemInstrItinData<II_VST_dmx_sts_bm_st_idx_imm, [AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_bm__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs
MemInstrItinData<II_VST_dmx_sts_bm_st_pstm_nrm, [AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_bm__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64
MemInstrItinData<II_VST_dmx_sts_bm_st_pstm_nrm_imm, [AvoidPartWordStore, EmptyCycles<1>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_x__spill__agus_dmx__nrm_spill__agus_dmx__spill__mSPs__m_c16n_step64
MemInstrItinData<II_VST_dmx_sts_x_spill, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*imm*/1, /*sp*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_x__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx__mPs__mDJs
MemInstrItinData<II_VST_dmx_sts_x_st_idx, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*dj*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_x__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64
MemInstrItinData<II_VST_dmx_sts_x_st_idx_imm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_x__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs
MemInstrItinData<II_VST_dmx_sts_x_st_pstm_nrm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*mod*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__st__dmx_sts_x__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64
MemInstrItinData<II_VST_dmx_sts_x_st_pstm_nrm_imm, [AvoidPartWordStore, SimpleCycle<FEY_RS_L0_PORT>, PrefixCycle<DMX_WRS>, SimpleCycle<DM_ADS>], [/*ptr_out*/1, /*src*/1, /*ptr*/1, /*imm*/1], MemoryCycles<[5]>>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add__vsub__vec_size__vec_size32__vec_add_mX__vec_add_nX
InstrItinData<II_VSUB_16, [SimpleCycle<CRBF8CONF_RV>], [/*d*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add__vsub__vec_size__vec_size16__vec_add_mX__vec_add_nX
InstrItinData<II_VSUB_32, [SimpleCycle<CRBF8CONF_RV>], [/*d*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add__vsub__vec_size__vec_size64__vec_add_mX__vec_add_nX
InstrItinData<II_VSUB_8, [SimpleCycle<CRBF8CONF_RV>], [/*d*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_ge__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VSUB_GE_16_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_ge__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VSUB_GE_16_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_ge__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VSUB_GE_32_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_ge__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VSUB_GE_32_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_ge__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VSUB_GE_8_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r17_r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*d*/MV_Bypass, /*r17_r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_ge__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VSUB_GE_8_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r17_r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*d*/MV_Bypass, /*r17_r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_lt__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VSUB_LT_16_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_lt__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VSUB_LT_16_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_lt__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VSUB_LT_32_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_lt__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VSUB_LT_32_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*d*/MV_Bypass, /*r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_lt__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VSUB_LT_8_vaddSign0, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r17_r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*d*/MV_Bypass, /*r17_r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_lt__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VSUB_LT_8_vaddSign1, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*d*/2, /*r17_r16*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*d*/MV_Bypass, /*r17_r16*/NoBypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__vec__vacc__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vacc_cm_core
InstrItinData<II_VSUB_vacc, [SimpleCycle<VCTRL_IN>, EmptyCycles<2>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vacc_cm_half_core
InstrItinData<II_VSUB_vacc_half, [SimpleCycle<VCTRL_IN>, EmptyCycles<2>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/4, /*acc*/1], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vacc_cm_core
InstrItinData<II_VSUB_vacc_scd_add_scd_incr, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vacc_cm_core
InstrItinData<II_VSUB_vacc_scd_add_scd_noincr, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vacc_cm_half_core
InstrItinData<II_VSUB_vacc_scd_half_add_scd_incr_half, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vacc_cm_half_core
InstrItinData<II_VSUB_vacc_scd_half_add_scd_noincr_half, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, SimpleCycle<DM_WV_L0_PORT>], [/*dst*/6, /*acc1*/4, /*acc2*/3, /*acc*/1, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_fp__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vacc_bf_core
InstrItinData<II_VSUB_f_vacc_fp, [SimpleCycle<VCTRL_IN>, EmptyCycles<2>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_fp_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vacc_bf_core
InstrItinData<II_VSUB_f_vacc_fp_scd_add_scd_incr, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_fp_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vacc_bf_core
InstrItinData<II_VSUB_f_vacc_fp_scd_add_scd_noincr, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_half_fp__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vacc_bf_half_core
InstrItinData<II_VSUB_f_vacc_half_fp, [SimpleCycle<VCTRL_IN>, EmptyCycles<2>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/4, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_half_fp_scd__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vacc_bf_half_core
InstrItinData<II_VSUB_f_vacc_half_fp_scd_add_scd_incr_half, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc2_out*/3, /*acc1*/4, /*acc2*/3, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_half_fp_scd__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vacc_bf_half_core
InstrItinData<II_VSUB_f_vacc_half_fp_scd_add_scd_noincr_half, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/7, /*acc1*/4, /*acc2*/3, /*acc*/1, /*srFPFlags*/8, /*crFPMask*/8, /*crSCDEn*/3], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_fp_bfp13__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_reg__vmac_acc2_comp0__add_reg__mDMm__vacc_bfp13_core
InstrItinData<II_VSUB_mx6_vacc_fp_bfp13, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, SimpleCycle<DM_RV_L0_PORT>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/6, /*acc1*/3, /*acc2*/3, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_fp_bfp13_scd__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_scd__vmac_acc2_comp0__add_scd__vmac_acc2_comp0__add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_comp0__vmac_scd_dm_dyn__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r_incr__mR31_scd__vacc_bfp13_core
InstrItinData<II_VSUB_mx6_vacc_fp_bfp13_scd_add_scd_incr, [SimpleCycle<VCTRL_IN>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/6, /*acc2_out*/2, /*acc1*/3, /*acc2*/2, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7, /*crSCDEn*/2], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_fp_bfp13_scd__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_scd__vmac_acc2_comp0__add_scd__vmac_acc2_comp0__add_scd_noincr__vmac_scd_dm_reg__vmac_scd_dm_comp0__vmac_scd_dm_reg__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r__mR31_scd__vacc_bfp13_core
InstrItinData<II_VSUB_mx6_vacc_fp_bfp13_scd_add_scd_noincr, [SimpleCycle<VCTRL_IN>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_IN>, PrefixCycle<ACC_B_INH>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, SimpleCycle<SCD_EXPAND_IN>, PrefixCycle<BFSHCH__ACC1>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTH__A>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCH_IN>, PrefixCycle<PSA_ACCH_IN_S>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_R>, PrefixCycle<ACC_RH>, PrefixCycle<ACC_RL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, SimpleCycle<FP32FLAG__AH>], [/*dst*/6, /*acc1*/3, /*acc2*/2, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7, /*crSCDEn*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_half_fp_bfp13__mCMLv__vmac_acc1_add_half__vmac_acc1_comp0__add_half__mCMLv__vmac_acc2_add_reg_half__vmac_acc2_comp0__add_reg_half__mCMLm__vacc_half_bfp13_core
InstrItinData<II_VSUB_mx6_vacc_half_fp_bfp13, [SimpleCycle<VCTRL_IN>, EmptyCycles<1>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RM_L0_PORT>, PrefixCycle<DM_RV_L0_PORT>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/6, /*acc1*/3, /*acc2*/3, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/VEC_Bypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_half_fp_bfp13_scd__mCMLv__vmac_acc1_add_half__vmac_acc1_comp0__add_half__mCMLv__vmac_acc2_add_scd_half__vmac_acc2_comp0__add_scd_half__vmac_acc2_comp0__add_scd_incr_half__vmac_scd_cml_dyn__vmac_scd_dm_comp0__vmac_scd_cml_dyn__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r_incr__mR31_scd__vacc_half_bfp13_core
InstrItinData<II_VSUB_mx6_vacc_half_fp_bfp13_scd_add_scd_incr_half, [SimpleCycle<VCTRL_IN>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/6, /*acc2_out*/2, /*acc1*/3, /*acc2*/2, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7, /*crSCDEn*/2], [/*dst*/VEC_Bypass, /*acc2_out*/NoBypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__vec__vacc_half_fp_bfp13_scd__mCMLv__vmac_acc1_add_half__vmac_acc1_comp0__add_half__mCMLv__vmac_acc2_add_scd_half__vmac_acc2_comp0__add_scd_half__vmac_acc2_comp0__add_scd_noincr_half__vmac_scd_cml_reg__vmac_scd_dm_comp0__vmac_scd_cml_reg__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r__mR31_scd__vacc_half_bfp13_core
InstrItinData<II_VSUB_mx6_vacc_half_fp_bfp13_scd_add_scd_noincr_half, [SimpleCycle<VCTRL_IN>, PrefixCycle<SCD_CTL>, SimpleCycle<SCD_INCR__O_POS>, PrefixCycle<ACC_B_INL>, PrefixCycle<DM_RV_L0_PORT>, PrefixCycle<SCD>, PrefixCycle<SCD_EXPAND_IN>, PrefixCycle<SCD_EXPAND_INHH>, SimpleCycle<ZACC_DML>, PrefixCycle<BFSHCH__SUBTRACT_ACC>, PrefixCycle<BF_CMAX_S>, SimpleCycle<PSA_RSRC>, PrefixCycle<BFACC1SHFTL__A>, PrefixCycle<PSA_ACCL_IN>, SimpleCycle<PSA_ACCL_IN_S>, PrefixCycle<ACC_RL>, PrefixCycle<ACC_WL>, PrefixCycle<DM_WV_L0_PORT>, SimpleCycle<FP32NORML__FLAGS>, PrefixCycle<CRFPMASK_RV>, PrefixCycle<FP32FLAG__AH>, SimpleCycle<FP32_VOFLAGS_PH_SCL>], [/*dst*/6, /*acc1*/3, /*acc2*/2, /*acc*/1, /*srFPFlags*/7, /*crFPMask*/7, /*crSCDEn*/2], [/*dst*/VEC_Bypass, /*acc1*/VEC_Bypass, /*acc2*/NoBypass, /*acc*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_nlf__vtanh
InstrItinData<II_VTANH, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/MV_Bypass, /*src*/NoBypass]>,

// id: me__instr128_or__ldb__ldb_or1__mv_unpack_w__unpackSign0
InstrItinData<II_VUNPACK_mv_unpack_w_unpackSign0, [EmptyCycles<6>, PrefixCycle<FEY_RS_L0_PORT>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*src*/7, /*crUnpackSize*/7, /*unpackSign0*/7]>,

// id: me__instr128_or__ldb__ldb_or1__mv_unpack_w__unpackSign1
InstrItinData<II_VUNPACK_mv_unpack_w_unpackSign1, [EmptyCycles<6>, PrefixCycle<FEY_RS_L0_PORT>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*src*/7, /*crUnpackSize*/7, /*unpackSign1*/7]>,

// id: me__instr128_or__ldb__ldb_or1__mv_unpack_x__mYb__unpackSign0
InstrItinData<II_VUNPACK_mv_unpack_x_unpackSign0, [EmptyCycles<6>, PrefixCycle<FEY_RS_L0_PORT>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*src*/7, /*crUnpackSize*/7, /*unpackSign0*/7]>,

// id: me__instr128_or__ldb__ldb_or1__mv_unpack_x__mYb__unpackSign1
InstrItinData<II_VUNPACK_mv_unpack_x_unpackSign1, [EmptyCycles<6>, PrefixCycle<FEY_RS_L0_PORT>, SimpleCycle<FEY_WB_L0_PORT>], [/*dst*/7, /*src*/7, /*crUnpackSize*/7, /*unpackSign1*/7]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_w2b__ups_mov__w2b__ups_mov__base__mSm__upsSign0
InstrItinData<II_VUPS_2x_mv_ups_w2b_upsSign0, [SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__KL>], [/*dst*/3, /*src*/1, /*base*/1, /*srUPS_of*/3, /*crSat*/1, /*crUPSMode*/2, /*upsSign0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_w2b__ups_mov__w2b__ups_mov__base__mSm__upsSign1
InstrItinData<II_VUPS_2x_mv_ups_w2b_upsSign1, [SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__KL>], [/*dst*/3, /*src*/1, /*base*/1, /*srUPS_of*/3, /*crSat*/1, /*crUPSMode*/2, /*upsSign1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_x2c__ups_mov__x2c__ups_mov__base__mSm__upsSign0
InstrItinData<II_VUPS_2x_mv_ups_x2c_upsSign0, [SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, PrefixCycle<UPS__H__OMUXL>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/3, /*src*/1, /*base*/1, /*srUPS_of*/3, /*crSat*/1, /*crUPSMode*/2, /*upsSign0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_x2c__ups_mov__x2c__ups_mov__base__mSm__upsSign1
InstrItinData<II_VUPS_2x_mv_ups_x2c_upsSign1, [SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, PrefixCycle<UPS__H__OMUXL>, SimpleCycle<UPS__L__OMUXL>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/3, /*src*/1, /*base*/1, /*srUPS_of*/3, /*crSat*/1, /*crUPSMode*/2, /*upsSign1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_w2c__ups_mov__w2c__ups_mov__base__mSm__upsSign0
InstrItinData<II_VUPS_4x_mv_ups_w2c_upsSign0, [SimpleCycle<CRSAT_RA>, SimpleCycle<CRUPSMODE_RA>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/3, /*src*/1, /*base*/1, /*srUPS_of*/3, /*crSat*/1, /*crUPSMode*/2, /*upsSign0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_w2c__ups_mov__w2c__ups_mov__base__mSm__upsSign1
InstrItinData<II_VUPS_4x_mv_ups_w2c_upsSign1, [SimpleCycle<CRSAT_RA>, SimpleCycle<CRUPSMODE_RA>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__L__K>], [/*dst*/3, /*src*/1, /*base*/1, /*srUPS_of*/3, /*crSat*/1, /*crUPSMode*/2, /*upsSign1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_x2d__ups_mov__x2d__mDMm__ups_mov__base__mSm__upsSign0
InstrItinData<II_VUPS_4x_mv_ups_x2d_upsSign0, [SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, SimpleCycle<UPS__OMUX>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__K>], [/*dst*/3, /*src*/1, /*base*/1, /*srUPS_of*/3, /*crSat*/1, /*crUPSMode*/2, /*upsSign0*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_x2d__ups_mov__x2d__mDMm__ups_mov__base__mSm__upsSign1
InstrItinData<II_VUPS_4x_mv_ups_x2d_upsSign1, [SimpleCycle<CRSAT_RA>, PrefixCycle<CRUPSMODE_RA>, PrefixCycle<UPS__H__A>, SimpleCycle<UPS__OMUX>, PrefixCycle<DM_WM_L0_PORT>, PrefixCycle<EVENT_UPS_OF>, SimpleCycle<UPS__K>], [/*dst*/3, /*src*/1, /*base*/1, /*srUPS_of*/3, /*crSat*/1, /*crUPSMode*/2, /*upsSign1*/1]>,

// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__xor
InstrItinData<II_XOR, [], [/*d0*/1, /*s0*/1, /*s1*/1]>,

include "ExtraItineraries.td"

]>;
