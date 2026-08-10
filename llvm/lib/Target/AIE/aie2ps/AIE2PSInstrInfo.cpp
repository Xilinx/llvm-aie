//===- AIE2PSInstrInfo.cpp AIE2ps Instruction Information -*------- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIE2ps implementation of the TargetInstrInfo
// class.
//
//===----------------------------------------------------------------------===//

#include "AIE2PSInstrInfo.h"
#include "AIE2PSRegisterInfo.h"
#include "AIE2PSSubtarget.h"
#include "AIE2PSTargetMachine.h"
#include "AIEBaseInstrInfo.h"
#include "AIEHazardRecognizer.h"
#include "AIEMachineFunctionInfo.h"
#include "AIEMachineScheduler.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "MCTargetDesc/aie2ps/AIE2PSMCTargetDesc.h"
#include "llvm/CodeGen/GlobalISel/GenericMachineInstrs.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/IR/IntrinsicsAIE2.h"
#include "llvm/IR/IntrinsicsAIE2PS.h"

#define DEBUG_TYPE "aie-codegen"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "AIE2PSGenInstrInfo.inc"

#include "AIE2PSGenMemoryCycles.inc"
#include "AIE2PSGenPreSchedLowering.inc"
#include "AIE2PSGenSplitInstrTables.inc"
#include "AIE2PSGenVarInstructionItin.inc"

namespace {
const AIE2PSMCFormats AIE2PSFormats;
} // namespace

AIE2PSInstrInfo::AIE2PSInstrInfo()
    : AIE2PSGenInstrInfo(AIE2PS::ADJCALLSTACKUP, AIE2PS::ADJCALLSTACKDOWN) {
  FormatInterface = &AIE2PSFormats;
  FuncUnitWrapper::setFormatInterface(FormatInterface);
}

unsigned AIE2PSInstrInfo::getReturnOpcode() const { return AIE2PS::PseudoRET; }

unsigned AIE2PSInstrInfo::getNopOpcode() const { return AIE2PS::NOP; }

unsigned AIE2PSInstrInfo::getMvSclOpcode() const {
  return AIE2PS::MOV_alu_mv_mv_mv_scl;
}

unsigned AIE2PSInstrInfo::getMvSclMultiSlotPseudoOpcode() const {
  return AIE2PS::MOVX_mvx_cr_imm;
}

static MCRegister getLoSubReg(const TargetRegisterInfo &TRI, MCRegister Reg) {
  if (AIE2PS::eLRegClass.contains(Reg))
    return TRI.getSubReg(Reg, AIE2PS::sub_l_even);
  if (AIE2PS::EXPVEC64RegClass.contains(Reg) ||
      AIE2PS::mGGaRegClass.contains(Reg))
    return TRI.getSubReg(Reg, AIE2PS::sub_lo_exp);
  if (AIE2PS::mEYwRegClass.contains(Reg))
    return TRI.getSubReg(Reg, AIE2PS::sub_bfp640_lo);
  if (AIE2PS::mFEYwRegClass.contains(Reg))
    return TRI.getSubReg(Reg, AIE2PS::sub_bfp768_lo);
  if (AIE2PS::VEC512RegClass.contains(Reg))
    return TRI.getSubReg(Reg, AIE2PS::sub_256_lo);
  if (AIE2PS::VEC1024RegClass.contains(Reg))
    return TRI.getSubReg(Reg, AIE2PS::sub_512_lo);
  if (AIE2PS::ACC1024RegClass.contains(Reg))
    return TRI.getSubReg(Reg, AIE2PS::sub_512_acc_lo);
  if (AIE2PS::ACC2048RegClass.contains(Reg))
    return TRI.getSubReg(Reg, AIE2PS::sub_1024_acc_lo);
  if (AIE2PS::FIFO1024RegClass.contains(Reg))
    return TRI.getSubReg(Reg, AIE2PS::sub_lo_fifo);
  llvm_unreachable("unhandled case in getLoSubReg");
}

static MCRegister getHiSubReg(const TargetRegisterInfo &TRI, MCRegister Reg) {
  if (AIE2PS::eLRegClass.contains(Reg))
    return TRI.getSubReg(Reg, AIE2PS::sub_l_odd);
  if (AIE2PS::EXPVEC64RegClass.contains(Reg) ||
      AIE2PS::mGGaRegClass.contains(Reg))
    return TRI.getSubReg(Reg, AIE2PS::sub_hi_exp);
  if (AIE2PS::mEYwRegClass.contains(Reg))
    return TRI.getSubReg(Reg, AIE2PS::sub_bfp640_hi);
  if (AIE2PS::mFEYwRegClass.contains(Reg))
    return TRI.getSubReg(Reg, AIE2PS::sub_bfp768_hi);
  if (AIE2PS::VEC512RegClass.contains(Reg))
    return TRI.getSubReg(Reg, AIE2PS::sub_256_hi);
  if (AIE2PS::VEC1024RegClass.contains(Reg))
    return TRI.getSubReg(Reg, AIE2PS::sub_512_hi);
  if (AIE2PS::ACC1024RegClass.contains(Reg))
    return TRI.getSubReg(Reg, AIE2PS::sub_512_acc_hi);
  if (AIE2PS::ACC2048RegClass.contains(Reg))
    return TRI.getSubReg(Reg, AIE2PS::sub_1024_acc_hi);
  if (AIE2PS::FIFO1024RegClass.contains(Reg))
    return TRI.getSubReg(Reg, AIE2PS::sub_hi_fifo);
  llvm_unreachable("unhandled case in getHiSubReg");
}

unsigned AIE2PSInstrInfo::getAddrIntrinsic2D() const {
  return Intrinsic::aie2ps_add_2d;
}

unsigned AIE2PSInstrInfo::getAddrIntrinsic3D() const {
  return Intrinsic::aie2ps_add_3d;
}

unsigned AIE2PSInstrInfo::getIdOnlyLockIntrinsic(unsigned PtrID) const {
  switch (PtrID) {
  case Intrinsic::aie2ps_acquire_ptr:
    return Intrinsic::aie2ps_acquire;
  case Intrinsic::aie2ps_acquire_cond_ptr:
    return Intrinsic::aie2ps_acquire_cond;
  case Intrinsic::aie2ps_release_ptr:
    return Intrinsic::aie2ps_release;
  case Intrinsic::aie2ps_release_cond_ptr:
    return Intrinsic::aie2ps_release_cond;
  default:
    return Intrinsic::not_intrinsic;
  }
}

unsigned AIE2PSInstrInfo::getPtrAdd2DOpcode() const {
  return AIE2PS::PADD_2D_pseudo;
}

unsigned AIE2PSInstrInfo::getPtrAdd3DOpcode() const {
  return AIE2PS::PADD_3D_pseudo;
}

Register AIE2PSInstrInfo::getVaddSignControlRegister() const {
  return AIE2PS::vaddSign0;
}

unsigned AIE2PSInstrInfo::getOpCode(MachineInstr &I) const {
  const MachineRegisterInfo &MRI = I.getMF()->getRegInfo();
  unsigned IntrinsicID = cast<GIntrinsic>(I).getIntrinsicID();
  switch (IntrinsicID) {
  // vsrs
  case Intrinsic::aie2ps_I256_v16_acc32_srs:
  case Intrinsic::aie2ps_I256_v8_acc64_srs:
    return AIE2PS::VSRS_2x_mv_w_srs_bm_srsSign0;
  case Intrinsic::aie2ps_I512_v32_acc32_srs:
  case Intrinsic::aie2ps_I512_v16_acc64_srs:
    return AIE2PS::VSRS_2x_mv_x_srs_cm_srsSign0;
  case Intrinsic::aie2ps_I512_v64_acc32_srs:
  case Intrinsic::aie2ps_I512_v32_acc64_srs:
    return AIE2PS::VSRS_4x_mv_x_srs_dm_srsSign0;
  case Intrinsic::aie2ps_I256_v32_acc32_srs:
  case Intrinsic::aie2ps_I256_v16_acc64_srs:
    return AIE2PS::VSRS_4x_mv_w_srs_cm_srsSign0;
  // vups
  case Intrinsic::aie2ps_acc32_v16_I256_ups:
  case Intrinsic::aie2ps_acc64_v8_I256_ups:
    return AIE2PS::VUPS_2x_mv_ups_w2b_upsSign0;
  case Intrinsic::aie2ps_acc32_v32_I256_ups:
  case Intrinsic::aie2ps_acc64_v16_I256_ups:
    return AIE2PS::VUPS_4x_mv_ups_w2c_upsSign0;
  case Intrinsic::aie2ps_acc32_v32_I512_ups:
  case Intrinsic::aie2ps_acc64_v16_I512_ups:
    return AIE2PS::VUPS_2x_mv_ups_x2c_upsSign0;
  case Intrinsic::aie2ps_acc32_v64_I512_ups:
  case Intrinsic::aie2ps_acc64_v32_I512_ups:
    return AIE2PS::VUPS_4x_mv_ups_x2d_upsSign0;
  // Vmax Intrinsic
  case Intrinsic::aie2ps_vmax_lt8:
    return AIE2PS::VMAX_LT_8_vaddSign0;
  case Intrinsic::aie2ps_vmax_lt16:
    return AIE2PS::VMAX_LT_16_vaddSign0;
  case Intrinsic::aie2ps_vmax_lt32:
    return AIE2PS::VMAX_LT_32_vaddSign0;
  // Vmin Intrinsic
  case Intrinsic::aie2ps_vmin_ge8:
    return AIE2PS::VMIN_GE_8_vaddSign0;
  case Intrinsic::aie2ps_vmin_ge16:
    return AIE2PS::VMIN_GE_16_vaddSign0;
  case Intrinsic::aie2ps_vmin_ge32:
    return AIE2PS::VMIN_GE_32_vaddSign0;
  // VGE / VLT
  case Intrinsic::aie2ps_vlt8:
    return AIE2PS::VLT_8_vaddSign0;
  case Intrinsic::aie2ps_vlt16:
    return AIE2PS::VLT_16_vaddSign0;
  case Intrinsic::aie2ps_vlt32:
    return AIE2PS::VLT_32_vaddSign0;
  case Intrinsic::aie2ps_vge8:
    return AIE2PS::VGE_8_vaddSign0;
  case Intrinsic::aie2ps_vge16:
    return AIE2PS::VGE_16_vaddSign0;
  case Intrinsic::aie2ps_vge32:
    return AIE2PS::VGE_32_vaddSign0;
  // VMAXDIFF_LT
  case Intrinsic::aie2ps_vmaxdiff_lt8:
    return AIE2PS::VMAXDIFF_LT_8_vaddSign0;
  case Intrinsic::aie2ps_vmaxdiff_lt16:
    return AIE2PS::VMAXDIFF_LT_16_vaddSign0;
  case Intrinsic::aie2ps_vmaxdiff_lt32:
    return AIE2PS::VMAXDIFF_LT_32_vaddSign0;
  // VABS_GTZ
  case Intrinsic::aie2ps_vabs_gtz8:
    return AIE2PS::VABS_GTZ8_vaddSign0;
  case Intrinsic::aie2ps_vabs_gtz16:
    return AIE2PS::VABS_GTZ16_vaddSign0;
  case Intrinsic::aie2ps_vabs_gtz32:
    return AIE2PS::VABS_GTZ32_vaddSign0;
  // VSUB_LT/VSUB_GE
  case Intrinsic::aie2ps_vsub_lt8:
    return AIE2PS::VSUB_LT_8_vaddSign0;
  case Intrinsic::aie2ps_vsub_lt16:
    return AIE2PS::VSUB_LT_16_vaddSign0;
  case Intrinsic::aie2ps_vsub_lt32:
    return AIE2PS::VSUB_LT_32_vaddSign0;
  case Intrinsic::aie2ps_vsub_ge8:
    return AIE2PS::VSUB_GE_8_vaddSign0;
  case Intrinsic::aie2ps_vsub_ge16:
    return AIE2PS::VSUB_GE_16_vaddSign0;
  case Intrinsic::aie2ps_vsub_ge32:
    return AIE2PS::VSUB_GE_32_vaddSign0;
  // Pack
  case Intrinsic::aie2ps_pack_I1024_I8_I16:
  case Intrinsic::aie2ps_pack_I1024_I4_I8:
  case Intrinsic::aie2ps_pack_I512_I8_I16:
  case Intrinsic::aie2ps_pack_I512_I4_I8: {
    Register SignReg = I.getOperand(3).getReg();
    auto Sign = getIConstantVRegValWithLookThrough(SignReg, MRI);
    bool isSigned = Sign && Sign->Value.getZExtValue();

    if (IntrinsicID == Intrinsic::aie2ps_pack_I512_I8_I16 ||
        IntrinsicID == Intrinsic::aie2ps_pack_I512_I4_I8)
      return isSigned ? AIE2PS::VPACK_mv_pack_w_packSign1
                      : AIE2PS::VPACK_mv_pack_w_packSign0;
    else
      return isSigned ? AIE2PS::VPACK_mv_pack_x_packSign1
                      : AIE2PS::VPACK_mv_pack_x_packSign0;
  }
  // Unpack
  case Intrinsic::aie2ps_unpack_I1024_I16_I8:
  case Intrinsic::aie2ps_unpack_I1024_I8_I4:
  case Intrinsic::aie2ps_unpack_I512_I16_I8:
  case Intrinsic::aie2ps_unpack_I512_I8_I4: {
    Register SignReg = I.getOperand(3).getReg();
    auto Sign = getIConstantVRegValWithLookThrough(SignReg, MRI);
    bool isSigned = Sign && Sign->Value.getZExtValue();
    if (IntrinsicID == Intrinsic::aie2ps_unpack_I512_I16_I8 ||
        IntrinsicID == Intrinsic::aie2ps_unpack_I512_I8_I4)
      return isSigned ? AIE2PS::VUNPACK_mv_unpack_w_unpackSign1
                      : AIE2PS::VUNPACK_mv_unpack_w_unpackSign0;
    else
      return isSigned ? AIE2PS::VUNPACK_mv_unpack_x_unpackSign1
                      : AIE2PS::VUNPACK_mv_unpack_x_unpackSign0;
  }
  // Cascade stream read (SCD)
  case Intrinsic::aie2ps_scd_read_vec:
    return AIE2PS::VMOV_alu_mv_alu_mv_scd_x;
  case Intrinsic::aie2ps_scd_read_acc32:
    return AIE2PS::VMOV_alu_mv_alu_mv_scd_bm;
  case Intrinsic::aie2ps_scd_expand_lo:
    return AIE2PS::VMOV_0_mv_scd_cm;
  case Intrinsic::aie2ps_scd_expand_hi:
    return AIE2PS::VMOV_1_mv_scd_cm;
  case Intrinsic::aie2ps_scd_ACC2048: {
    Register SrcReg = I.getOperand(3).getReg();
    if (auto Src = getIConstantVRegValWithLookThrough(SrcReg, MRI)) {
      unsigned SrcConstVal = Src->Value.getZExtValue();
      switch (SrcConstVal) {
      case 0:
        return AIE2PS::VMOV_0_mv_scd_dm_imm;
      case 1:
        return AIE2PS::VMOV_1_mv_scd_dm_imm;
      case 2:
        return AIE2PS::VMOV_2;
      case 3:
        return AIE2PS::VMOV_3;
      default:
        llvm_unreachable("Unexpected SrcConstVal for SCD");
      }
    }
    llvm_unreachable("Unexpected non-constant for SCD");
  }
  case Intrinsic::aie2ps_scd_expand_ACC1024:
  case Intrinsic::aie2ps_scd_expand_ACC2048:
    return AIE2PS::VMOV_alu_mv_alu_mv_scd_dm_reg;
  case Intrinsic::aie2ps_scd_expand_ACC1024_incr:
  case Intrinsic::aie2ps_scd_expand_ACC2048_incr:
    return AIE2PS::VMOV_alu_mv_alu_mv_scd_dm_dyn;
  // Cascade stream write (MCD)
  case Intrinsic::aie2ps_mcd_write_vec:
    return AIE2PS::VMOV_st_mv_mcd_x;
  case Intrinsic::aie2ps_mcd_write_acc32:
    return AIE2PS::VMOV_st_mv_mcd_bm;
  // Scalar stream intrinsics
  case Intrinsic::aie2ps_get_ss:
    return AIE2PS::MOV_lda;
  case Intrinsic::aie2ps_get_ss_nb:
    return AIE2PS::MOV_nb_lda;
  case Intrinsic::aie2ps_put_ms:
    return AIE2PS::MOV_st_mMStream_tlast_reg;
  case Intrinsic::aie2ps_put_ms_nb:
    return AIE2PS::MOV_nb_st_mMStream_tlast_reg;
  default:
    llvm_unreachable("Unexpected Intrinsic ID");
  }
}

// Implement CopyToReg/CopyFromReg
void AIE2PSInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator MBBI,
                                  const DebugLoc &DL, Register DstReg,
                                  Register SrcReg, bool KillSrc,
                                  bool RenamableDest, bool RenamableSrc) const {
  MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
  const TargetRegisterInfo &TRI = *MRI.getTargetRegisterInfo();

  if (AIE2PS::mMvSclSrcRegClass.contains(SrcReg) &&
      AIE2PS::mMvSclDstRegClass.contains(DstReg)) {
    // Build MultiSlotPseudo in preference
    const unsigned MOVSclOpcode = getScalarMovOpcode(DstReg, SrcReg);
    BuildMI(MBB, MBBI, DL, get(MOVSclOpcode), DstReg)
        .addReg(SrcReg, getKillRegState(KillSrc));
    // clang-format off
#define HANDLE_MOV_CASE(SRC_CLASS, DST_CLASS, OPCODE)                          \
  } else if ((AIE2PS::SRC_CLASS##RegClass.contains(SrcReg)) &&                 \
             (AIE2PS::DST_CLASS##RegClass.contains(DstReg))) {                 \
    BuildMI(MBB, MBBI, DL, get(AIE2PS::MOV_alu_mv_mv_mv_##OPCODE), DstReg)     \
        .addReg(SrcReg, getKillRegState(KillSrc));
  HANDLE_MOV_CASE(eR, mSCm, sc_r)
  HANDLE_MOV_CASE(mSCm, eR, r_sc)
  HANDLE_MOV_CASE(mElm, mElm, e_mv_el_to_el)
  HANDLE_MOV_CASE(mEhm, mEhm, e_mv_eh_to_eh)
  HANDLE_MOV_CASE(mElm, mEhm, e_mv_el_to_eh)
  HANDLE_MOV_CASE(mEhm, mElm, e_mv_eh_to_el)
  HANDLE_MOV_CASE(eR, mEhm, e_mv_r_to_eh)
  HANDLE_MOV_CASE(eR, mElm, e_mv_r_to_el)
  HANDLE_MOV_CASE(mEhm, eR, e_mv_eh_to_r)
  HANDLE_MOV_CASE(mElm, eR, e_mv_el_to_r)
  HANDLE_MOV_CASE(mGlm, mGlm, g_mv_gl_to_gl)
  HANDLE_MOV_CASE(mGhm, mGhm, g_mv_gh_to_gh)
  HANDLE_MOV_CASE(mGlm, mGhm, g_mv_gl_to_gh)
  HANDLE_MOV_CASE(mGhm, mGlm, g_mv_gh_to_gl)
  HANDLE_MOV_CASE(eR, mGhm, g_mv_r_to_gh)
  HANDLE_MOV_CASE(eR, mGlm, g_mv_r_to_gl)
  HANDLE_MOV_CASE(mGhm, eR, g_mv_gh_to_r)
  HANDLE_MOV_CASE(mGlm, eR, g_mv_gl_to_r)
#undef HANDLE_MOV_CASE
    // clang-format on
  } else if ((AIE2PS::eLRegClass.contains(SrcReg)) &&
             (AIE2PS::eLRegClass.contains(DstReg))) {
    copyThroughSubRegs(MBB, MBBI, DL, DstReg, SrcReg, KillSrc);
  } else if ((AIE2PS::eDRegClass.contains(SrcReg)) &&
             (AIE2PS::eDRegClass.contains(DstReg))) {
    copyThroughSubRegs(MBB, MBBI, DL, DstReg, SrcReg, KillSrc);
  } else if ((AIE2PS::eDSRegClass.contains(SrcReg)) &&
             (AIE2PS::eDSRegClass.contains(DstReg))) {
    copyThroughSubRegs(MBB, MBBI, DL, DstReg, SrcReg, KillSrc);
    // clang-format off
#define HANDLE_VMOV_CASE(SRC_CLASS, DST_CLASS, OPCODE)                         \
  } else if ((AIE2PS::SRC_CLASS##RegClass.contains(SrcReg)) &&                 \
             (AIE2PS::DST_CLASS##RegClass.contains(DstReg))) {                 \
    BuildMI(MBB, MBBI, DL, get(AIE2PS::VMOV_alu_mv_mv_mv_##OPCODE), DstReg)    \
        .addReg(SrcReg, getKillRegState(KillSrc));
  HANDLE_VMOV_CASE(mFm, mFm, f)
  HANDLE_VMOV_CASE(mWm, mWm, w)
  HANDLE_VMOV_CASE(mCMm, mCMm, cm)
  HANDLE_VMOV_CASE(mEEm, mEEm, ee)
  HANDLE_VMOV_CASE(mEGm, mEGm, eg)
  HANDLE_VMOV_CASE(mFFm, mFFm, ff)
  HANDLE_VMOV_CASE(mGGm, mGGm, gg)
  HANDLE_VMOV_CASE(mEWm, mEWm, egw)
  HANDLE_VMOV_CASE(mEXm, mEXm, egx)
  HANDLE_VMOV_CASE(mEG2m, mEG2m, eg2)
  HANDLE_VMOV_CASE(mFEWm, mFEWm, few)
  HANDLE_VMOV_CASE(mFEXm, mFEXm, fex)
  HANDLE_VMOV_CASE(mFm, mLm, f_to_l)
  HANDLE_VMOV_CASE(mLm, mFm, l_to_f)
  HANDLE_VMOV_CASE(mWm, mEG2m, w_to_eg2)
  HANDLE_VMOV_CASE(mEG2m, mWm, eg2_to_w)
  HANDLE_VMOV_CASE(mMvBMXSrc, mMvBMXDst, x)
#undef HANDLE_VMOV_CASE
    // clang-format on
  } else if ((AIE2PS::eLRegClass.contains(SrcReg)) &&
             (AIE2PS::EXPVEC64RegClass.contains(DstReg))) {
    copyPhysReg(MBB, MBBI, DL, getLoSubReg(TRI, DstReg),
                getLoSubReg(TRI, SrcReg), KillSrc);
    copyPhysReg(MBB, MBBI, DL, getHiSubReg(TRI, DstReg),
                getHiSubReg(TRI, SrcReg), KillSrc);
  } else if ((AIE2PS::EXPVEC64RegClass.contains(SrcReg)) &&
             (AIE2PS::eLRegClass.contains(DstReg))) {
    copyPhysReg(MBB, MBBI, DL, getLoSubReg(TRI, DstReg),
                getLoSubReg(TRI, SrcReg), KillSrc);
    copyPhysReg(MBB, MBBI, DL, getHiSubReg(TRI, DstReg),
                getHiSubReg(TRI, SrcReg), KillSrc);
  } else if ((AIE2PS::eLRegClass.contains(SrcReg) &&
              AIE2PS::mGGaRegClass.contains(DstReg)) ||
             (AIE2PS::mGGaRegClass.contains(SrcReg) &&
              AIE2PS::eLRegClass.contains(DstReg))) {
    copyPhysReg(MBB, MBBI, DL, getLoSubReg(TRI, DstReg),
                getLoSubReg(TRI, SrcReg), KillSrc);
    copyPhysReg(MBB, MBBI, DL, getHiSubReg(TRI, DstReg),
                getHiSubReg(TRI, SrcReg), KillSrc);
  } else if ((AIE2PS::mEYwRegClass.contains(SrcReg)) &&
             (AIE2PS::mEYwRegClass.contains(DstReg))) {
    copyPhysReg(MBB, MBBI, DL, getLoSubReg(TRI, DstReg),
                getLoSubReg(TRI, SrcReg), KillSrc);
    copyPhysReg(MBB, MBBI, DL, getHiSubReg(TRI, DstReg),
                getHiSubReg(TRI, SrcReg), KillSrc);
  } else if ((AIE2PS::mFEYwRegClass.contains(SrcReg)) &&
             (AIE2PS::mFEYwRegClass.contains(DstReg))) {
    copyPhysReg(MBB, MBBI, DL, getLoSubReg(TRI, DstReg),
                getLoSubReg(TRI, SrcReg), KillSrc);
    copyPhysReg(MBB, MBBI, DL, getHiSubReg(TRI, DstReg),
                getHiSubReg(TRI, SrcReg), KillSrc);
  } else if ((AIE2PS::ACC1024RegClass.contains(SrcReg) ||
              AIE2PS::VEC1024RegClass.contains(SrcReg) ||
              AIE2PS::FIFO1024RegClass.contains(SrcReg)) &&
             (AIE2PS::ACC1024RegClass.contains(DstReg) ||
              AIE2PS::VEC1024RegClass.contains(DstReg) ||
              AIE2PS::FIFO1024RegClass.contains(DstReg))) {
    copyPhysReg(MBB, MBBI, DL, getLoSubReg(TRI, DstReg),
                getLoSubReg(TRI, SrcReg), KillSrc);
    copyPhysReg(MBB, MBBI, DL, getHiSubReg(TRI, DstReg),
                getHiSubReg(TRI, SrcReg), KillSrc);
  } else if ((AIE2PS::ACC2048RegClass.contains(SrcReg)) &&
             (AIE2PS::ACC2048RegClass.contains(DstReg))) {
    copyPhysReg(MBB, MBBI, DL, getLoSubReg(TRI, DstReg),
                getLoSubReg(TRI, SrcReg), KillSrc);
    copyPhysReg(MBB, MBBI, DL, getHiSubReg(TRI, DstReg),
                getHiSubReg(TRI, SrcReg), KillSrc);
  } else if ((AIE2PS::ePSRFLdFRegClass.contains(SrcReg)) &&
             (AIE2PS::ePSRFLdFRegClass.contains(DstReg))) {
    copyThroughSubRegs(MBB, MBBI, DL, DstReg, SrcReg, KillSrc);
  } else {
    errs() << "copyPhysReg: cannot copy " << TRI.getName(SrcReg) << " -> "
           << TRI.getName(DstReg) << '\n';
    llvm_unreachable("unhandled case in copyPhysReg");
  }
}

static const TargetRegisterClass *
constrainRegClass(MachineRegisterInfo &MRI, const TargetRegisterClass *RC,
                  unsigned Reg) {
  if (RC == nullptr || Register::isPhysicalRegister(Reg))
    return RC;

  // eP, eM, eDN, eDJ, eDC, eSpecial20
  if (auto *NewRC = MRI.constrainRegClass(Reg, &AIE2PS::eP_as_32BitRegClass))
    return NewRC;
  if (auto *NewRC = MRI.constrainRegClass(Reg, &AIE2PS::eM_as_32BitRegClass))
    return NewRC;
  if (auto *NewRC = MRI.constrainRegClass(Reg, &AIE2PS::eDN_as_32BitRegClass))
    return NewRC;
  if (auto *NewRC = MRI.constrainRegClass(Reg, &AIE2PS::eDJ_as_32BitRegClass))
    return NewRC;
  if (auto *NewRC = MRI.constrainRegClass(Reg, &AIE2PS::eDC_as_32BitRegClass))
    return NewRC;
  if (auto *NewRC =
          MRI.constrainRegClass(Reg, &AIE2PS::eSpecial20_as_32BitRegClass))
    return NewRC;
  return RC;
}

Register AIE2PSInstrInfo::isLoadFromStackSlot(const MachineInstr &MI,
                                              int &FrameIndex) const {
  switch (MI.getOpcode()) {
  default:
    return 0;
  case AIE2PS::LDA_R_SPILL:
  case AIE2PS::LDA_L_SPILL:
  case AIE2PS::LDA_D_SPILL:
  case AIE2PS::LDA_DS_SPILL:
  case AIE2PS::VLDA_V_SPILL:
  case AIE2PS::VLDA_W_SPILL:
  case AIE2PS::VLDA_X_SPILL:
  case AIE2PS::VLDA_Y_SPILL:
  case AIE2PS::VLDA_BM_SPILL:
  case AIE2PS::VLDA_CM_SPILL:
  case AIE2PS::VLDA_DM_SPILL:
  case AIE2PS::VLDA_E_SPILL:
  case AIE2PS::VLDA_EE_SPILL:
  case AIE2PS::VLDA_F_SPILL:
  case AIE2PS::VLDA_FF_SPILL:
  case AIE2PS::VLDA_G_SPILL:
  case AIE2PS::VLDA_GG_SPILL:
  case AIE2PS::VLDA_EG_SPILL:
  case AIE2PS::VLDA_EG2_SPILL:
  case AIE2PS::VLDA_FEG_SPILL:
  case AIE2PS::VLDA_FEG2_SPILL:
  case AIE2PS::VLDA_EW_SPILL:
  case AIE2PS::VLDA_EX_SPILL:
  case AIE2PS::VLDA_EY_SPILL:
  case AIE2PS::VLDA_FEW_SPILL:
  case AIE2PS::VLDA_FEX_SPILL:
  case AIE2PS::VLDA_FEY_SPILL:
    break;
  }

  if (MI.getOperand(1).isFI()) {
    FrameIndex = MI.getOperand(1).getIndex();
    return MI.getOperand(0).getReg();
  }

  return 0;
}

Register AIE2PSInstrInfo::isStoreToStackSlot(const MachineInstr &MI,
                                             int &FrameIndex) const {
  switch (MI.getOpcode()) {
  default:
    return 0;
  case AIE2PS::ST_R_SPILL:
  case AIE2PS::ST_L_SPILL:
  case AIE2PS::ST_D_SPILL:
  case AIE2PS::ST_DS_SPILL:
  case AIE2PS::VST_V_SPILL:
  case AIE2PS::VST_W_SPILL:
  case AIE2PS::VST_X_SPILL:
  case AIE2PS::VST_Y_SPILL:
  case AIE2PS::VST_BM_SPILL:
  case AIE2PS::VST_CM_SPILL:
  case AIE2PS::VST_DM_SPILL:
  case AIE2PS::VST_E_SPILL:
  case AIE2PS::VST_EE_SPILL:
  case AIE2PS::VST_F_SPILL:
  case AIE2PS::VST_FF_SPILL:
  case AIE2PS::VST_G_SPILL:
  case AIE2PS::VST_GG_SPILL:
  case AIE2PS::VST_EG_SPILL:
  case AIE2PS::VST_EG2_SPILL:
  case AIE2PS::VST_FEG_SPILL:
  case AIE2PS::VST_FEG2_SPILL:
  case AIE2PS::VST_EW_SPILL:
  case AIE2PS::VST_EX_SPILL:
  case AIE2PS::VST_EY_SPILL:
  case AIE2PS::VST_FEW_SPILL:
  case AIE2PS::VST_FEX_SPILL:
  case AIE2PS::VST_FEY_SPILL:
    break;
  }

  if (MI.getOperand(1).isFI()) {
    FrameIndex = MI.getOperand(1).getIndex();
    return MI.getOperand(0).getReg();
  }

  return 0;
}

SmallVector<AIEBaseInstrInfo::AIEPseudoExpandInfo, 4>
AIE2PSInstrInfo::getSpillPseudoExpandInfo(const TargetRegisterInfo &TRI,
                                          MachineInstr &MI) const {
  if (!MI.isPseudo())
    return {};

  switch (MI.getOpcode()) {
  case AIE2PS::ST_R_SPILL:
    return {{AIE2PS::ST_dms_sts_scalar_spill}};
  case AIE2PS::ST_L_SPILL:
    return {{AIE2PS::ST_dml_sts_scalar_spill}};
  case AIE2PS::ST_D_SPILL:
    return {{AIE2PS::ST_dms_sts_scalar_spill, AIE2PS::sub_mod},
            {AIE2PS::ST_dms_sts_scalar_spill, AIE2PS::sub_dim_size},
            {AIE2PS::ST_dms_sts_scalar_spill, AIE2PS::sub_dim_stride},
            {AIE2PS::ST_dms_sts_scalar_spill, AIE2PS::sub_dim_count}};
  case AIE2PS::ST_DS_SPILL:
    return {{AIE2PS::ST_D_SPILL, AIE2PS::sub_lo_dim},
            {AIE2PS::ST_D_SPILL, AIE2PS::sub_hi_dim}};
  case AIE2PS::VST_V_SPILL:
    return {{AIE2PS::VST_128_dmv_sts_w_spill}};
  case AIE2PS::VST_W_SPILL:
    return {{AIE2PS::VST_dmw_sts_w_spill}};
  case AIE2PS::VST_X_SPILL:
    return {{AIE2PS::VST_dmx_sts_x_spill}};
  case AIE2PS::VST_Y_SPILL:
    return {{AIE2PS::VST_X_SPILL, AIE2PS::sub_512_lo},
            {AIE2PS::VST_X_SPILL, AIE2PS::sub_512_hi}};
  case AIE2PS::VST_BM_SPILL:
    return {{AIE2PS::VST_dmx_sts_bm_spill}};
  case AIE2PS::VST_CM_SPILL:
    return {{AIE2PS::VST_BM_SPILL, AIE2PS::sub_512_acc_lo},
            {AIE2PS::VST_BM_SPILL, AIE2PS::sub_512_acc_hi}};
  case AIE2PS::VST_DM_SPILL:
    return {{AIE2PS::VST_CM_SPILL, AIE2PS::sub_1024_acc_lo},
            {AIE2PS::VST_CM_SPILL, AIE2PS::sub_1024_acc_hi}};
  case AIE2PS::VST_E_SPILL:
    return {{AIE2PS::ST_dms_sts_scalar_spill}};
  case AIE2PS::VST_EE_SPILL:
    return {{AIE2PS::ST_dml_sts_scalar_spill}};
  case AIE2PS::VST_F_SPILL:
    return {{AIE2PS::ST_dml_sts_scalar_spill}};
  case AIE2PS::VST_FF_SPILL:
    return {{AIE2PS::ST_dmv_sts_f_spill}};
  case AIE2PS::VST_G_SPILL:
    return {{AIE2PS::ST_dms_sts_g_spill}};
  case AIE2PS::VST_GG_SPILL:
    return {{AIE2PS::ST_dml_sts_scalar_spill}};
  case AIE2PS::VST_EG_SPILL:
    return {{AIE2PS::ST_dml_sts_scalar_spill}};
  case AIE2PS::VST_EG2_SPILL:
    return {{AIE2PS::VST_128_dmv_sts_eg_spill}};
  case AIE2PS::VST_FEG_SPILL:
    return {{AIE2PS::VST_128_dmv_sts_feg_spill}};
  case AIE2PS::VST_FEG2_SPILL:
    return {{AIE2PS::VST_dmw_sts_feg2_spill}};
  case AIE2PS::VST_EW_SPILL: // FIXME: Use VST_EG_SPILL
    return {{AIE2PS::VST_W_SPILL, AIE2PS::sub_bfp_v256},
            {AIE2PS::VST_G_SPILL, AIE2PS::sub_bfp_g32},
            {AIE2PS::VST_E_SPILL, AIE2PS::sub_bfp_e32}};
  case AIE2PS::VST_EX_SPILL: // FIXME: Use VST_X_SPILL and VST_EG2_SPILL
    return {{AIE2PS::VST_W_SPILL, AIE2PS::sub_bfp_v256},
            {AIE2PS::VST_W_SPILL, AIE2PS::sub_bfp320_hi_then_sub_bfp_v256},
            {AIE2PS::VST_G_SPILL, AIE2PS::sub_bfp_g32},
            {AIE2PS::VST_G_SPILL, AIE2PS::sub_bfp320_hi_then_sub_bfp_g32},
            {AIE2PS::VST_E_SPILL, AIE2PS::sub_bfp_e32},
            {AIE2PS::VST_E_SPILL, AIE2PS::sub_bfp320_hi_then_sub_bfp_e32}};
  case AIE2PS::VST_EY_SPILL: // FIXME: Use 2xVST_X_SPILL and 2xVST_EG2_SPILL
    return {{AIE2PS::VST_W_SPILL, AIE2PS::sub_bfp_v256},
            {AIE2PS::VST_W_SPILL, AIE2PS::sub_bfp320_hi_then_sub_bfp_v256},
            {AIE2PS::VST_W_SPILL, AIE2PS::sub_bfp640_hi_then_sub_bfp_v256},
            {AIE2PS::VST_W_SPILL,
             AIE2PS::sub_bfp640_hi_then_sub_bfp320_hi_then_sub_bfp_v256},
            {AIE2PS::VST_G_SPILL, AIE2PS::sub_bfp_g32},
            {AIE2PS::VST_G_SPILL, AIE2PS::sub_bfp320_hi_then_sub_bfp_g32},
            {AIE2PS::VST_G_SPILL, AIE2PS::sub_bfp640_hi_then_sub_bfp_g32},
            {AIE2PS::VST_G_SPILL,
             AIE2PS::sub_bfp640_hi_then_sub_bfp320_hi_then_sub_bfp_g32},
            {AIE2PS::VST_E_SPILL, AIE2PS::sub_bfp_e32},
            {AIE2PS::VST_E_SPILL, AIE2PS::sub_bfp320_hi_then_sub_bfp_e32},
            {AIE2PS::VST_E_SPILL, AIE2PS::sub_bfp640_hi_then_sub_bfp_e32},
            {AIE2PS::VST_E_SPILL,
             AIE2PS::sub_bfp640_hi_then_sub_bfp320_hi_then_sub_bfp_e32}};
  case AIE2PS::VST_FEW_SPILL: // FIXME: Use VST_FEG_SPILL
    return {{AIE2PS::VST_W_SPILL, AIE2PS::sub_few_bfp_w},
            {AIE2PS::VST_F_SPILL, AIE2PS::sub_few_bfp_f64},
            {AIE2PS::VST_G_SPILL, AIE2PS::sub_few_bfp_g32},
            {AIE2PS::VST_E_SPILL, AIE2PS::sub_few_bfp_e32}};
  case AIE2PS::VST_FEX_SPILL: // FIXME: Use VST_X_SPILL and VST_FEG2_SPILL
    return {{AIE2PS::VST_W_SPILL, AIE2PS::sub_few_bfp_w},
            {AIE2PS::VST_W_SPILL, AIE2PS::sub_bfp384_hi_then_sub_few_bfp_w},
            {AIE2PS::VST_F_SPILL, AIE2PS::sub_few_bfp_f64},
            {AIE2PS::VST_F_SPILL, AIE2PS::sub_bfp384_hi_then_sub_few_bfp_f64},
            {AIE2PS::VST_G_SPILL, AIE2PS::sub_few_bfp_g32},
            {AIE2PS::VST_G_SPILL, AIE2PS::sub_bfp384_hi_then_sub_few_bfp_g32},
            {AIE2PS::VST_E_SPILL, AIE2PS::sub_few_bfp_e32},
            {AIE2PS::VST_E_SPILL, AIE2PS::sub_bfp384_hi_then_sub_few_bfp_e32}};
  case AIE2PS::VST_FEY_SPILL: // FIXME: Use 2xVST_X_SPILL and 2xVST_FEG2_SPILL
    return {{AIE2PS::VST_W_SPILL, AIE2PS::sub_few_bfp_w},
            {AIE2PS::VST_W_SPILL, AIE2PS::sub_bfp384_hi_then_sub_few_bfp_w},
            {AIE2PS::VST_W_SPILL, AIE2PS::sub_bfp768_hi_then_sub_few_bfp_w},
            {AIE2PS::VST_W_SPILL,
             AIE2PS::sub_bfp768_hi_then_sub_bfp384_hi_then_sub_few_bfp_w},
            {AIE2PS::VST_F_SPILL, AIE2PS::sub_few_bfp_f64},
            {AIE2PS::VST_F_SPILL, AIE2PS::sub_bfp384_hi_then_sub_few_bfp_f64},
            {AIE2PS::VST_F_SPILL, AIE2PS::sub_bfp768_hi_then_sub_few_bfp_f64},
            {AIE2PS::VST_F_SPILL,
             AIE2PS::sub_bfp768_hi_then_sub_bfp384_hi_then_sub_few_bfp_f64},
            {AIE2PS::VST_G_SPILL, AIE2PS::sub_few_bfp_g32},
            {AIE2PS::VST_G_SPILL, AIE2PS::sub_bfp384_hi_then_sub_few_bfp_g32},
            {AIE2PS::VST_G_SPILL, AIE2PS::sub_bfp768_hi_then_sub_few_bfp_g32},
            {AIE2PS::VST_G_SPILL,
             AIE2PS::sub_bfp768_hi_then_sub_bfp384_hi_then_sub_few_bfp_g32},
            {AIE2PS::VST_E_SPILL, AIE2PS::sub_few_bfp_e32},
            {AIE2PS::VST_E_SPILL, AIE2PS::sub_bfp384_hi_then_sub_few_bfp_e32},
            {AIE2PS::VST_E_SPILL, AIE2PS::sub_bfp768_hi_then_sub_few_bfp_e32},
            {AIE2PS::VST_E_SPILL,
             AIE2PS::sub_bfp768_hi_then_sub_bfp384_hi_then_sub_few_bfp_e32}};

  case AIE2PS::LDA_R_SPILL:
    return {{AIE2PS::LDA_dms_lda_scalar_spill}};
  case AIE2PS::LDA_L_SPILL:
    return {{AIE2PS::LDA_dml_lda_scalar_L_spill}};
  case AIE2PS::LDA_D_SPILL:
    return {{AIE2PS::LDA_dms_lda_scalar_spill, AIE2PS::sub_mod},
            {AIE2PS::LDA_dms_lda_scalar_spill, AIE2PS::sub_dim_size},
            {AIE2PS::LDA_dms_lda_scalar_spill, AIE2PS::sub_dim_stride},
            {AIE2PS::LDA_dms_lda_scalar_spill, AIE2PS::sub_dim_count}};
  case AIE2PS::LDA_DS_SPILL:
    return {{AIE2PS::LDA_D_SPILL, AIE2PS::sub_lo_dim},
            {AIE2PS::LDA_D_SPILL, AIE2PS::sub_hi_dim}};
  case AIE2PS::VLDA_V_SPILL:
    return {{AIE2PS::VLDA_128_dmv_lda_w_spill}};
  case AIE2PS::VLDA_W_SPILL:
    return {{AIE2PS::VLDA_dmw_lda_w_spill}};
  case AIE2PS::VLDA_X_SPILL:
    return {{AIE2PS::VLDA_dmx_lda_x_spill}};
  case AIE2PS::VLDA_Y_SPILL:
    return {{AIE2PS::VLDA_X_SPILL, AIE2PS::sub_512_lo},
            {AIE2PS::VLDA_X_SPILL, AIE2PS::sub_512_hi}};
  case AIE2PS::VLDA_BM_SPILL:
    return {{AIE2PS::VLDA_dmx_lda_bm_spill}};
  case AIE2PS::VLDA_CM_SPILL:
    return {{AIE2PS::VLDA_BM_SPILL, AIE2PS::sub_512_acc_lo},
            {AIE2PS::VLDA_BM_SPILL, AIE2PS::sub_512_acc_hi}};
  case AIE2PS::VLDA_DM_SPILL:
    return {{AIE2PS::VLDA_CM_SPILL, AIE2PS::sub_1024_acc_lo},
            {AIE2PS::VLDA_CM_SPILL, AIE2PS::sub_1024_acc_hi}};
  case AIE2PS::VLDA_E_SPILL:
    return {{AIE2PS::LDA_dms_lda_scalar_spill}};
  case AIE2PS::VLDA_EE_SPILL:
    return {{AIE2PS::LDA_dml_lda2_scalar_EE_spill}};
  case AIE2PS::VLDA_F_SPILL:
    return {{AIE2PS::LDA_dml_lda_scalar_F_spill}};
  case AIE2PS::VLDA_FF_SPILL:
    return {{AIE2PS::LDA_dmv_lda_f_spill}};
  case AIE2PS::VLDA_G_SPILL:
    return {{AIE2PS::LDA_dms_lda_g_spill}};
  case AIE2PS::VLDA_GG_SPILL:
    return {{AIE2PS::LDA_dml_lda2_scalar_GG_spill}};
  case AIE2PS::VLDA_EG_SPILL:
    return {{AIE2PS::LDA_dml_lda2_scalar_EG_spill}};
  case AIE2PS::VLDA_EG2_SPILL:
    return {{AIE2PS::LDA_dmv_lda_eg2_spill}};
  case AIE2PS::VLDA_FEG_SPILL:
    return {{AIE2PS::LDA_dmv_lda_feg_spill}};
  case AIE2PS::VLDA_FEG2_SPILL:
    return {{AIE2PS::VLDA_dmw_lda_feg2_spill}};
  case AIE2PS::VLDA_EW_SPILL: // FIXME: Use VLDA_EG_SPILL
    return {{AIE2PS::VLDA_W_SPILL, AIE2PS::sub_bfp_v256},
            {AIE2PS::VLDA_G_SPILL, AIE2PS::sub_bfp_g32},
            {AIE2PS::VLDA_E_SPILL, AIE2PS::sub_bfp_e32}};
  case AIE2PS::VLDA_EX_SPILL: // FIXME: Use VLDA_EG2_SPILL
    return {{AIE2PS::VLDA_W_SPILL, AIE2PS::sub_bfp_v256},
            {AIE2PS::VLDA_W_SPILL, AIE2PS::sub_bfp320_hi_then_sub_bfp_v256},
            {AIE2PS::VLDA_G_SPILL, AIE2PS::sub_bfp_g32},
            {AIE2PS::VLDA_G_SPILL, AIE2PS::sub_bfp320_hi_then_sub_bfp_g32},
            {AIE2PS::VLDA_E_SPILL, AIE2PS::sub_bfp_e32},
            {AIE2PS::VLDA_E_SPILL, AIE2PS::sub_bfp320_hi_then_sub_bfp_e32}};
  case AIE2PS::VLDA_EY_SPILL: // FIXME: Use 2xVLDA_X_SPILL and 2xVLDA_EG2_SPILL
    return {{AIE2PS::VLDA_W_SPILL, AIE2PS::sub_bfp_v256},
            {AIE2PS::VLDA_W_SPILL, AIE2PS::sub_bfp320_hi_then_sub_bfp_v256},
            {AIE2PS::VLDA_W_SPILL, AIE2PS::sub_bfp640_hi_then_sub_bfp_v256},
            {AIE2PS::VLDA_W_SPILL,
             AIE2PS::sub_bfp640_hi_then_sub_bfp320_hi_then_sub_bfp_v256},
            {AIE2PS::VLDA_G_SPILL, AIE2PS::sub_bfp_g32},
            {AIE2PS::VLDA_G_SPILL, AIE2PS::sub_bfp320_hi_then_sub_bfp_g32},
            {AIE2PS::VLDA_G_SPILL, AIE2PS::sub_bfp640_hi_then_sub_bfp_g32},
            {AIE2PS::VLDA_G_SPILL,
             AIE2PS::sub_bfp640_hi_then_sub_bfp320_hi_then_sub_bfp_g32},
            {AIE2PS::VLDA_E_SPILL, AIE2PS::sub_bfp_e32},
            {AIE2PS::VLDA_E_SPILL, AIE2PS::sub_bfp320_hi_then_sub_bfp_e32},
            {AIE2PS::VLDA_E_SPILL, AIE2PS::sub_bfp640_hi_then_sub_bfp_e32},
            {AIE2PS::VLDA_E_SPILL,
             AIE2PS::sub_bfp640_hi_then_sub_bfp320_hi_then_sub_bfp_e32}};
  case AIE2PS::VLDA_FEW_SPILL: // FIXME: Use VLDA_FEG_SPILL
    return {{AIE2PS::VLDA_W_SPILL, AIE2PS::sub_few_bfp_w},
            {AIE2PS::VLDA_F_SPILL, AIE2PS::sub_few_bfp_f64},
            {AIE2PS::VLDA_G_SPILL, AIE2PS::sub_few_bfp_g32},
            {AIE2PS::VLDA_E_SPILL, AIE2PS::sub_few_bfp_e32}};
  case AIE2PS::VLDA_FEX_SPILL: // FIXME: Use VLDA_X_SPILL and VLDA_FEG2_SPILL
    return {{AIE2PS::VLDA_W_SPILL, AIE2PS::sub_few_bfp_w},
            {AIE2PS::VLDA_W_SPILL, AIE2PS::sub_bfp384_hi_then_sub_few_bfp_w},
            {AIE2PS::VLDA_F_SPILL, AIE2PS::sub_few_bfp_f64},
            {AIE2PS::VLDA_F_SPILL, AIE2PS::sub_bfp384_hi_then_sub_few_bfp_f64},
            {AIE2PS::VLDA_G_SPILL, AIE2PS::sub_few_bfp_g32},
            {AIE2PS::VLDA_G_SPILL, AIE2PS::sub_bfp384_hi_then_sub_few_bfp_g32},
            {AIE2PS::VLDA_E_SPILL, AIE2PS::sub_few_bfp_e32},
            {AIE2PS::VLDA_E_SPILL, AIE2PS::sub_bfp384_hi_then_sub_few_bfp_e32}};
  case AIE2PS::VLDA_FEY_SPILL: // FIXME: Use 2xVLDA_X_SPILL and
                               // 2xVLDA_FEG2_SPILL
    return {{AIE2PS::VLDA_W_SPILL, AIE2PS::sub_few_bfp_w},
            {AIE2PS::VLDA_W_SPILL, AIE2PS::sub_bfp384_hi_then_sub_few_bfp_w},
            {AIE2PS::VLDA_W_SPILL, AIE2PS::sub_bfp768_hi_then_sub_few_bfp_w},
            {AIE2PS::VLDA_W_SPILL,
             AIE2PS::sub_bfp768_hi_then_sub_bfp384_hi_then_sub_few_bfp_w},
            {AIE2PS::VLDA_F_SPILL, AIE2PS::sub_few_bfp_f64},
            {AIE2PS::VLDA_F_SPILL, AIE2PS::sub_bfp384_hi_then_sub_few_bfp_f64},
            {AIE2PS::VLDA_F_SPILL, AIE2PS::sub_bfp768_hi_then_sub_few_bfp_f64},
            {AIE2PS::VLDA_F_SPILL,
             AIE2PS::sub_bfp768_hi_then_sub_bfp384_hi_then_sub_few_bfp_f64},
            {AIE2PS::VLDA_G_SPILL, AIE2PS::sub_few_bfp_g32},
            {AIE2PS::VLDA_G_SPILL, AIE2PS::sub_bfp384_hi_then_sub_few_bfp_g32},
            {AIE2PS::VLDA_G_SPILL, AIE2PS::sub_bfp768_hi_then_sub_few_bfp_g32},
            {AIE2PS::VLDA_G_SPILL,
             AIE2PS::sub_bfp768_hi_then_sub_bfp384_hi_then_sub_few_bfp_g32},
            {AIE2PS::VLDA_E_SPILL, AIE2PS::sub_few_bfp_e32},
            {AIE2PS::VLDA_E_SPILL, AIE2PS::sub_bfp384_hi_then_sub_few_bfp_e32},
            {AIE2PS::VLDA_E_SPILL, AIE2PS::sub_bfp768_hi_then_sub_few_bfp_e32},
            {AIE2PS::VLDA_E_SPILL,
             AIE2PS::sub_bfp768_hi_then_sub_bfp384_hi_then_sub_few_bfp_e32}};
  default:
    // TODO: Implement other pseudos. Unreachable is replaced with return of an
    // empty struct to allow testing elimination of frame index. This is
    // equivalent to an unimplemented getSpillPseudoExpandInfo.
    return {};
    // llvm_unreachable("Un-handled spill opcode.");
  }
}

// Store a register to a stack slot.  Used in eliminating FrameIndex pseduo-ops.
void AIE2PSInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator I,
                                          Register SrcReg, bool IsKill, int FI,
                                          const TargetRegisterClass *RC,
                                          const TargetRegisterInfo *TRI,
                                          Register VReg,
                                          MachineInstr::MIFlag Flags) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();

  // Provide a store memory operand for a register store, resolving it
  // from other memory refs during scheduler dag generation
  auto CreateMMO = [&MF = *MBB.getParent()](int FI) {
    MachineFrameInfo &MFI = MF.getFrameInfo();
    MachinePointerInfo PtrInfo = MachinePointerInfo::getFixedStack(MF, FI);
    MachineMemOperand *MMO =
        MF.getMachineMemOperand(PtrInfo, MachineMemOperand::MOStore,
                                MFI.getObjectSize(FI), MFI.getObjectAlign(FI));
    return MMO;
  };
  RC = constrainRegClass(MBB.getParent()->getRegInfo(), RC, SrcReg);

  unsigned Opcode;
  LLVM_DEBUG(dbgs() << "Attempting to Store: " << SrcReg << " To " << FI
                    << "\n");

  auto bounceViaRegClass = [&](const TargetRegisterClass *BounceRC) {
    MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
    Register TmpReg = MRI.createVirtualRegister(BounceRC);
    BuildMI(MBB, I, DL, get(AIE2PS::COPY), TmpReg)
        .addReg(SrcReg, getKillRegState(IsKill));
    return storeRegToStackSlot(MBB, I, TmpReg, /*IsKill*/ true, FI, BounceRC,
                               TRI, VReg, Flags);
  };

  if (regClassMatches(AIE2PS::eRRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::ST_R_SPILL;
  } else if (regClassMatches(AIE2PS::eLRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::ST_L_SPILL;
  } else if (regClassMatches(AIE2PS::eDRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::ST_D_SPILL;
  } else if (regClassMatches(AIE2PS::eDSRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::ST_DS_SPILL;
  } else if (regClassMatches(AIE2PS::VEC256RegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_W_SPILL;
  } else if (regClassMatches(AIE2PS::VEC512RegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_X_SPILL;
  } else if (regClassMatches(AIE2PS::VEC1024RegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_Y_SPILL;
  } else if (regClassMatches(AIE2PS::ACC512RegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_BM_SPILL;
  } else if (regClassMatches(AIE2PS::ACC1024RegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_CM_SPILL;
  } else if (regClassMatches(AIE2PS::ACC2048RegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_DM_SPILL;
  } else if (regClassMatches(AIE2PS::FIFO512RegClass, RC, SrcReg)) {
    return bounceViaRegClass(&AIE2PS::VEC512RegClass);
  } else if (regClassMatches(AIE2PS::FIFO1024RegClass, RC, SrcReg)) {
    return bounceViaRegClass(&AIE2PS::VEC1024RegClass);
  } else if (regClassMatches(AIE2PS::ePSRFLdFRegClass, RC, SrcReg)) {
    MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
    Register TmpReg = MRI.createVirtualRegister(&AIE2PS::VEC1024RegClass);
    if (SrcReg.isPhysical()) {
      BuildMI(MBB, I, DL, get(AIE2PS::COPY), TmpReg)
          .addReg(TRI->getSubReg(SrcReg, AIE2PS::sub_fifo),
                  getKillRegState(IsKill));
      BuildMI(MBB, I, DL, get(AIE2PS::VST_PLFR_SPILL))
          .addReg(TmpReg, getKillRegState(true))
          .addReg(TRI->getSubReg(SrcReg, AIE2PS::sub_avail),
                  getKillRegState(IsKill))
          .addReg(TRI->getSubReg(SrcReg, AIE2PS::sub_ptr),
                  getKillRegState(IsKill))
          .addFrameIndex(FI)
          .addMemOperand(CreateMMO(FI));
    } else {
      BuildMI(MBB, I, DL, get(AIE2PS::COPY), TmpReg)
          .addReg(SrcReg, getKillRegState(IsKill), AIE2PS::sub_fifo);
      BuildMI(MBB, I, DL, get(AIE2PS::VST_PLFR_SPILL))
          .addReg(TmpReg, getKillRegState(true))
          .addReg(SrcReg, getKillRegState(IsKill), AIE2PS::sub_avail)
          .addReg(SrcReg, getKillRegState(IsKill), AIE2PS::sub_ptr)
          .addFrameIndex(FI)
          .addMemOperand(CreateMMO(FI));
    }
    return;
  } else if (regClassMatches(AIE2PS::mEsRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_E_SPILL;
  } else if (regClassMatches(AIE2PS::mEEsRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_EE_SPILL;
  } else if (regClassMatches(AIE2PS::mFsRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_F_SPILL;
  } else if (regClassMatches(AIE2PS::mFFsRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_FF_SPILL;
  } else if (regClassMatches(AIE2PS::mGsRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_G_SPILL;
  } else if (regClassMatches(AIE2PS::mGGsRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_GG_SPILL;
  } else if (regClassMatches(AIE2PS::mEGsRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_EG_SPILL;
  } else if (regClassMatches(AIE2PS::mEG2sRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_EG2_SPILL;
  } else if (regClassMatches(AIE2PS::mFEGsRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_FEG_SPILL;
  } else if (regClassMatches(AIE2PS::mFEG2sRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_FEG2_SPILL;
  } else if (regClassMatches(AIE2PS::mEWmRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_EW_SPILL;
  } else if (regClassMatches(AIE2PS::mEXmRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_EX_SPILL;
  } else if (regClassMatches(AIE2PS::mEYwRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_EY_SPILL;
  } else if (regClassMatches(AIE2PS::mFEWmRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_FEW_SPILL;
  } else if (regClassMatches(AIE2PS::mFEXmRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_FEX_SPILL;
  } else if (regClassMatches(AIE2PS::mFEYwRegClass, RC, SrcReg)) {
    Opcode = AIE2PS::VST_FEY_SPILL;
  } else if (regClassMatches(AIE2PS::mSclStRegClass, RC, SrcReg)) {
    // Anything that can be stored in a scalar register can use R_SPILL.
    Opcode = AIE2PS::ST_R_SPILL;
  } else if (regClassMatches(AIE2PS::eSRegClass, RC, SrcReg) ||
             regClassMatches(AIE2PS::spill_eS_to_eRRegClass, RC, SrcReg)) {
    // Can't spill these directly.  Need to bounce through a GPR.
    return bounceViaRegClass(&AIE2PS::eRRegClass);
  } else {
    LLVM_DEBUG(I->dump());
    llvm_unreachable("Can't store this register to stack slot: is it virtual?");
  }

  // To store a stack slot we generate a store indirect via the stack
  // pointer.  The actual offset will be an immediate, but for right
  // now stuff in a virtual "FrameIndex" argument to represent the
  // offset that will be figured out later.  The offset is generated
  // by AIERegisterInfo::eliminateFrameIndex().
  BuildMI(MBB, I, DL, get(Opcode))
      .addReg(SrcReg, getKillRegState(IsKill))
      .addFrameIndex(FI)
      .addMemOperand(CreateMMO(FI));
}

// Load a register to a stack slot.  Used in eliminating FrameIndex pseduo-ops.
void AIE2PSInstrInfo::loadRegFromStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator I, Register DstReg,
    int FI, const TargetRegisterClass *RC, const TargetRegisterInfo *TRI,
    Register VReg, MachineInstr::MIFlag Flags) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();
  unsigned Opcode;

  // Provide a load memory operand for a register load, resolving it
  // from other memory refs during scheduler dag generation
  auto CreateMMO = [&MF = *MBB.getParent()](int FI) {
    MachineFrameInfo &MFI = MF.getFrameInfo();
    MachinePointerInfo PtrInfo = MachinePointerInfo::getFixedStack(MF, FI);
    MachineMemOperand *MMO =
        MF.getMachineMemOperand(PtrInfo, MachineMemOperand::MOLoad,
                                MFI.getObjectSize(FI), MFI.getObjectAlign(FI));
    return MMO;
  };
  RC = constrainRegClass(MBB.getParent()->getRegInfo(), RC, DstReg);

  auto bounceViaRegClass = [&](const TargetRegisterClass *BounceRC) {
    MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
    Register TmpReg = MRI.createVirtualRegister(BounceRC);
    loadRegFromStackSlot(MBB, I, TmpReg, FI, BounceRC, TRI, VReg, Flags);
    BuildMI(MBB, I, DL, get(AIE2PS::COPY), DstReg)
        .addReg(TmpReg, getKillRegState(true));
    return;
  };

  if (regClassMatches(AIE2PS::eRRegClass, RC, DstReg)) {
    Opcode = AIE2PS::LDA_R_SPILL;
  } else if (regClassMatches(AIE2PS::eLRegClass, RC, DstReg)) {
    Opcode = AIE2PS::LDA_L_SPILL;
  } else if (regClassMatches(AIE2PS::eDRegClass, RC, DstReg)) {
    Opcode = AIE2PS::LDA_D_SPILL;
  } else if (regClassMatches(AIE2PS::eDSRegClass, RC, DstReg)) {
    Opcode = AIE2PS::LDA_DS_SPILL;
  } else if (regClassMatches(AIE2PS::VEC256RegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_W_SPILL;
  } else if (regClassMatches(AIE2PS::VEC512RegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_X_SPILL;
  } else if (regClassMatches(AIE2PS::VEC1024RegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_Y_SPILL;
  } else if (regClassMatches(AIE2PS::ACC512RegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_BM_SPILL;
  } else if (regClassMatches(AIE2PS::ACC1024RegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_CM_SPILL;
  } else if (regClassMatches(AIE2PS::ACC2048RegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_DM_SPILL;
  } else if (regClassMatches(AIE2PS::FIFO512RegClass, RC, DstReg)) {
    return bounceViaRegClass(&AIE2PS::VEC512RegClass);
  } else if (regClassMatches(AIE2PS::FIFO1024RegClass, RC, DstReg)) {
    return bounceViaRegClass(&AIE2PS::VEC1024RegClass);
  } else if (regClassMatches(AIE2PS::ePSRFLdFRegClass, RC, DstReg)) {
    // VLDA_PLFR_SPILL is a single, full def of the composite DstReg. It loads
    // sub_avail/sub_ptr directly and reloads sub_fifo via a VEC1024 scratch
    // (AIE2PS has no direct FIFO reload) that is a second def of the pseudo so
    // the register allocator (not the scavenger) provides it. The actual
    // scratch->FIFO bounce copy is materialized when the pseudo is expanded in
    // eliminateFrameIndex. Keeping a single def here avoids reconstructing the
    // composite from pieces, which would break its (sub-register) liveness.
    MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
    Register TmpReg = MRI.createVirtualRegister(&AIE2PS::VEC1024RegClass);
    BuildMI(MBB, I, DL, get(AIE2PS::VLDA_PLFR_SPILL))
        .addReg(TmpReg, RegState::Define)
        .addReg(DstReg, RegState::Define)
        .addFrameIndex(FI)
        .addMemOperand(CreateMMO(FI));
    return;
  } else if (regClassMatches(AIE2PS::mEsRegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_E_SPILL;
  } else if (regClassMatches(AIE2PS::mEEsRegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_EE_SPILL;
  } else if (regClassMatches(AIE2PS::mFsRegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_F_SPILL;
  } else if (regClassMatches(AIE2PS::mFFsRegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_FF_SPILL;
  } else if (regClassMatches(AIE2PS::mGsRegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_G_SPILL;
  } else if (regClassMatches(AIE2PS::mGGsRegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_GG_SPILL;
  } else if (regClassMatches(AIE2PS::mEGsRegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_EG_SPILL;
  } else if (regClassMatches(AIE2PS::mEG2sRegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_EG2_SPILL;
  } else if (regClassMatches(AIE2PS::mFEGsRegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_FEG_SPILL;
  } else if (regClassMatches(AIE2PS::mFEG2sRegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_FEG2_SPILL;
  } else if (regClassMatches(AIE2PS::mEWmRegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_EW_SPILL;
  } else if (regClassMatches(AIE2PS::mEXmRegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_EX_SPILL;
  } else if (regClassMatches(AIE2PS::mEYwRegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_EY_SPILL;
  } else if (regClassMatches(AIE2PS::mFEWmRegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_FEW_SPILL;
  } else if (regClassMatches(AIE2PS::mFEXmRegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_FEX_SPILL;
  } else if (regClassMatches(AIE2PS::mFEYwRegClass, RC, DstReg)) {
    Opcode = AIE2PS::VLDA_FEY_SPILL;
  } else if (regClassMatches(AIE2PS::mLdaSclRegClass, RC, DstReg)) {
    // Anything that can be loaded into a scalar register can use R_SPILL.
    Opcode = AIE2PS::LDA_R_SPILL;
  } else if (regClassMatches(AIE2PS::eSRegClass, RC, DstReg) ||
             regClassMatches(AIE2PS::spill_eS_to_eRRegClass, RC, DstReg)) {
    // Can't spill these directly.  Need to bounce through a GPR.
    return bounceViaRegClass(&AIE2PS::eRRegClass);
  } else {
    LLVM_DEBUG(I->dump());
    llvm_unreachable(
        "Can't load this register from stack slot: is it virtual?");
  }

  // To load from a stack slot we generate a load indirect via the
  // stack pointer.  The actual offset will be an immediate, but for
  // right now stuff in a virtual "FrameIndex" argument to represent
  // the offset that will be figured out later.  The offset is
  // generated by AIERegisterInfo::eliminateFrameIndex().
  BuildMI(MBB, I, DL, get(Opcode), DstReg)
      .addFrameIndex(FI)
      .addMemOperand(CreateMMO(FI));
}

unsigned AIE2PSInstrInfo::getAddSclOpcode() const {
  return AIE2PS::ADD_alu_r_rr;
}

unsigned AIE2PSInstrInfo::getOppositeBranchOpcode(unsigned Opc) const {
  switch (Opc) {
  default:
    llvm_unreachable("Unrecognized conditional branch");
  case AIE2PS::PseudoJNZ:
    return AIE2PS::PseudoJZ;
  case AIE2PS::PseudoJZ:
    return AIE2PS::PseudoJNZ;
  }
  return 0;
}

unsigned AIE2PSInstrInfo::getJumpOpcode() const {
  return AIE2PS::PseudoJ_jump_imm;
}

unsigned AIE2PSInstrInfo::getPseudoMoveOpcode() const {
  return AIE2PS::PseudoMove;
}

std::optional<unsigned>
AIE2PSInstrInfo::getConstantMovOpcode(MachineRegisterInfo &MRI,
                                      unsigned int Reg, APInt &Val) const {
  const auto &TRI =
      static_cast<const AIE2PRegisterInfo *>(MRI.getTargetRegisterInfo());
  unsigned int ImmSize = Val.getSignificantBits();

  const TargetRegisterClass *DstRegClass = nullptr;
  const RegClassOrRegBank &RCB = MRI.getRegClassOrRegBank(Reg);
  if (const RegisterBank *RB = RCB.dyn_cast<const RegisterBank *>())
    DstRegClass = &TRI->getMinClassForRegBank(*RB, MRI.getType(Reg));
  if (auto *TRC = RCB.dyn_cast<const TargetRegisterClass *>())
    DstRegClass = TRC;
  assert(DstRegClass != nullptr && "RC cannot be null");
  if (ImmSize <= 11) {
    if (regClassMatches(AIE2PS::mAluCgRegClass, DstRegClass, Reg))
      return AIE2PS::MOV_RLC_imm11_pseudo;
    if (regClassMatches(AIE2PS::mAguDstRegClass, DstRegClass, Reg) ||
        regClassMatches(AIE2PS::ePRegClass, DstRegClass, Reg) ||
        regClassMatches(AIE2PS::mDmRegClass, DstRegClass, Reg) ||
        regClassMatches(AIE2PS::eRRegClass, DstRegClass, Reg))
      return AIE2PS::MOV_PD_imm11_pseudo;
    if (regClassMatches(AIE2PS::eSRegClass, DstRegClass, Reg))
      return AIE2PS::MOV_S_imm11_pseudo;
    if (regClassMatches(AIE2PS::mMvSclDstRegClass, DstRegClass, Reg))
      return AIE2PS::MOV_scalar_imm11_pseudo;
  }
  if (ImmSize <= 32)
    return AIE2PS::MOVXM_lng_cg;

  return std::nullopt;
}

unsigned AIE2PSInstrInfo::getScalarMovOpcode(Register DstReg,
                                             Register SrcReg) const {
  return (AIE2PS::eRRegClass.contains(SrcReg) &&
          AIE2PS::eRRegClass.contains(DstReg))
             ? AIE2PS::MOV_OR_pseudo
         : (AIE2PS::mAguSrcRegClass.contains(SrcReg) &&
            AIE2PS::mAguDstRegClass.contains(DstReg))
             ? AIE2PS::MOV_scalar_pseudo
             : AIE2PS::MOV_alu_mv_mv_mv_scl;
}

bool AIE2PSInstrInfo::jumpsToUnknown(unsigned Opc) const {
  return Opc == AIE2PS::RET || Opc == AIE2PS::JL_lng ||
         Opc == AIE2PS::JL_alumv_or;
}

bool AIE2PSInstrInfo::isCall(unsigned Opc) const {
  return Opc == AIE2PS::JL_lng || Opc == AIE2PS::JL_alumv_or;
}

bool AIE2PSInstrInfo::isIConst(unsigned Opc) const {
  switch (Opc) {
  case AIE2PS::MOVA:
  case AIE2PS::MOV_alu_mv_mv_mv_cg_or:
  case AIE2PS::MOVXM_lng_cg:
  case AIE2PS::MOVX_alu_cg_or:
  case AIE2PS::MOVX_mvx_cr_imm:
  case AIE2PS::MOV_RLC_imm11_pseudo:
  case AIE2PS::MOV_PD_imm11_pseudo:
  case AIE2PS::MOV_S_imm11_pseudo:
  case AIE2PS::MOV_scalar_imm11_pseudo:
    return true;
  default:
    return false;
  }
}

bool AIE2PSInstrInfo::isLock(unsigned Opc) const {
  switch (Opc) {
  default:
    break;
  case AIE2PS::ACQ_mLockId_imm:
  case AIE2PS::ACQ_mLockId_reg:
  case AIE2PS::ACQ_COND_mLockId_imm:
  case AIE2PS::ACQ_COND_mLockId_reg:
  case AIE2PS::REL_mLockId_imm:
  case AIE2PS::REL_mLockId_reg:
  case AIE2PS::REL_COND_mLockId_imm:
  case AIE2PS::REL_COND_mLockId_reg:
    return true;
  }
  return false;
}

std::optional<unsigned>
AIE2PSInstrInfo::getDoneLatency(const unsigned Opc) const {
  return (Opc == AIE2PS::DONE) ? std::optional<unsigned>(6) : std::nullopt;
}

int AIE2PSInstrInfo::isRoundRobinSlotCandidate(MachineInstr &MI) const {
  const unsigned Opc = MI.getOpcode();
  if (Opc == AIE2PS::VLD_FILL_512_pseudo)
    return 1;
  return 0;
}

unsigned AIE2PSInstrInfo::getCallOpcode(const MachineFunction &CallerF,
                                        bool IsIndirect,
                                        bool IsTailCall) const {

  if (IsTailCall)
    return IsIndirect ? AIE2PS::PseudoJ_TCO_jump_ind
                      : AIE2PS::PseudoJ_TCO_jump_imm;
  return IsIndirect ? AIE2PS::PseudoJL_IND : AIE2PS::PseudoJL;
}

unsigned AIE2PSInstrInfo::getCycleSeparatorOpcode() const {
  return AIE2PS::CYCLE_SEPARATOR;
}

bool AIE2PSInstrInfo::isOffsetInImmediateRange(
    unsigned Opcode, unsigned LoadStoreSize,
    std::optional<APInt> Offset) const {
  if (!Offset)
    return false;

  switch (Opcode) {
  case AIE2PS::G_AIE_OFFSET_STORE:
  case AIE2PS::G_AIE_OFFSET_LOAD: {
    switch (LoadStoreSize) {
    case 8:
      return checkSignedImmediateRange<4, 1>(Offset);
    case 16:
      return checkSignedImmediateRange<4, 2>(Offset);
    case 20:
    case 32:
      return checkSignedImmediateRange<4, 4>(Offset);
    case 128:
      return checkSignedImmediateRange<4, 16>(Offset);
    case 256:
      return checkSignedImmediateRange<4, 32>(Offset);
    case 512:
      return checkSignedImmediateRange<4, 64>(Offset);
    case 1024:
      return checkSignedImmediateRangeSplitting<4, 64, 64>(Offset);
    case 2048:
      return checkSignedImmediateRangeSplitting<4, 64, 192>(Offset);
    default:
      return false;
    }
  }
  case AIE2PS::G_AIE_OFFSET_SEXTLOAD:
  case AIE2PS::G_AIE_OFFSET_ZEXTLOAD:
  case AIE2PS::G_AIE_POSTINC_ZEXTLOAD:
  case AIE2PS::G_AIE_POSTINC_SEXTLOAD: {
    switch (LoadStoreSize) {
    case 8:
      return checkSignedImmediateRange<4, 1>(Offset);
    case 16:
      return checkSignedImmediateRange<4, 2>(Offset);
    default:
      return false;
    }
  }
  case AIE2PS::G_AIE_POSTINC_STORE:
  case AIE2PS::G_AIE_POSTINC_LOAD: {
    switch (LoadStoreSize) {
    case 8:
      return checkSignedImmediateRange<4, 1>(Offset);
    case 16:
      return checkSignedImmediateRange<4, 2>(Offset);
    case 20:
    case 32:
      return checkSignedImmediateRange<4, 4>(Offset);
    case 128:
      return checkSignedImmediateRange<4, 16>(Offset);
    case 256:
      return checkSignedImmediateRange<4, 32>(Offset);
    case 512:
    case 1024:
    case 2048:
      return checkSignedImmediateRange<4, 64>(Offset);
    default:
      return false;
    }
  }
  default:
    return false;
  }
}

unsigned AIE2PSInstrInfo::getOffsetMemOpcode(unsigned BaseMemOpcode) const {
  switch (BaseMemOpcode) {
  case TargetOpcode::G_STORE:
    return AIE2PS::G_AIE_OFFSET_STORE;
  case TargetOpcode::G_LOAD:
    return AIE2PS::G_AIE_OFFSET_LOAD;
  case TargetOpcode::G_SEXTLOAD:
    return AIE2PS::G_AIE_OFFSET_SEXTLOAD;
  case TargetOpcode::G_ZEXTLOAD:
    return AIE2PS::G_AIE_OFFSET_ZEXTLOAD;
  }
  llvm_unreachable("not a generic load/store");
}

bool AIE2PSInstrInfo::isGenericOffsetMemOpcode(unsigned Opcode) const {
  return ((Opcode == AIE2PS::G_AIE_OFFSET_STORE) ||
          (Opcode == AIE2PS::G_AIE_OFFSET_LOAD) ||
          (Opcode == AIE2PS::G_AIE_OFFSET_SEXTLOAD) ||
          (Opcode == AIE2PS::G_AIE_OFFSET_ZEXTLOAD));
}

bool AIE2PSInstrInfo::isFifoStoreConvOpcode(unsigned Opcode) const {
  return ((Opcode == AIE2PS::VST_PUSH_256_CONV_mx4_fp32) ||
          (Opcode == AIE2PS::VST_PUSH_384_CONV_mx6_fp32) ||
          (Opcode == AIE2PS::VST_PUSH_576_CONV_mx9_fp32));
}

std::optional<unsigned>
AIE2PSInstrInfo::getStoreFlushConvOpcode(unsigned StoreFlushOpcode) const {
  switch (StoreFlushOpcode) {
  case AIE2PS::VST_FLUSH_normal_flush:
    return AIE2PS::VST_FLUSH_d_normal_flush;
  case AIE2PS::VST_FLUSH_fifo_1d_flush:
    return AIE2PS::VST_FLUSH_d_fifo_1d_flush;
  case AIE2PS::VST_FLUSH_2D:
    return AIE2PS::VST_FLUSH_d_2D;
  case AIE2PS::VST_FLUSH_3D:
    return AIE2PS::VST_FLUSH_d_3D;
  }
  return std::nullopt;
}

std::optional<unsigned> AIE2PSInstrInfo::getCombinedPostIncOpcode(
    MachineInstr &BaseMemI, MachineInstr &PostIncI, TypeSize Size) const {
  switch (PostIncI.getOpcode()) {
  case TargetOpcode::G_PTR_ADD:
    switch (BaseMemI.getOpcode()) {
    case TargetOpcode::G_STORE:
      return AIE2PS::G_AIE_POSTINC_STORE;
    case TargetOpcode::G_LOAD:
      return AIE2PS::G_AIE_POSTINC_LOAD;
    case TargetOpcode::G_SEXTLOAD:
      return AIE2PS::G_AIE_POSTINC_SEXTLOAD;
    case TargetOpcode::G_ZEXTLOAD:
      return AIE2PS::G_AIE_POSTINC_ZEXTLOAD;
    }
    break;
  case TargetOpcode::G_INTRINSIC:
    switch (cast<GIntrinsic>(PostIncI).getIntrinsicID()) {
    case Intrinsic::aie2ps_add_2d:
      switch (BaseMemI.getOpcode()) {
      case TargetOpcode::G_STORE:
        return AIE2PS::G_AIE_POSTINC_2D_STORE;
      case TargetOpcode::G_LOAD:
        return AIE2PS::G_AIE_POSTINC_2D_LOAD;
      case TargetOpcode::G_SEXTLOAD:
        return AIE2PS::G_AIE_POSTINC_2D_SEXTLOAD;
      case TargetOpcode::G_ZEXTLOAD:
        return AIE2PS::G_AIE_POSTINC_2D_ZEXTLOAD;
      }
      break;
    case Intrinsic::aie2ps_add_3d:
      switch (BaseMemI.getOpcode()) {
      case TargetOpcode::G_STORE:
        return AIE2PS::G_AIE_POSTINC_3D_STORE;
      case TargetOpcode::G_LOAD:
        return AIE2PS::G_AIE_POSTINC_3D_LOAD;
      case TargetOpcode::G_SEXTLOAD:
        return AIE2PS::G_AIE_POSTINC_3D_SEXTLOAD;
      case TargetOpcode::G_ZEXTLOAD:
        return AIE2PS::G_AIE_POSTINC_3D_ZEXTLOAD;
      }
      break;
    }
    break;
  }
  return {};
}

std::optional<AIEBaseInstrInfo::ZOLSupport>
AIE2PSInstrInfo::getZOLSupport() const {
  AIEBaseInstrInfo::ZOLSupport Result;

  Result.LoopStartOpcode = AIE2PS::LoopStart;
  Result.LoopEndOpcode = AIE2PS::PseudoLoopEnd;
  Result.SetLoopCountOpcode = AIE2PS::ADD_NC_add_lc_ri;
  Result.SetLoopStartOpcode = AIE2PS::MOVXM_lng_cg_ls_abs;
  Result.SetLoopEndOpcode = AIE2PS::MOVXM_lng_cg_le_abs;
  // We need a 112 bytes distance from the loop setup to the loop end label,
  // which requires 7 bundles of 16 bytes.
  Result.LoopSetupDistance = 7;
  Result.LCRegister = AIE2PS::lc;

  return Result;
}

std::optional<unsigned> AIE2PSInstrInfo::getLoopVersionThresholdOpcode() const {
  return AIE2PS::PseudoLoopVersionThreshold;
}

std::optional<AIEBaseInstrInfo::JNZDSupport>
AIE2PSInstrInfo::getJNZDSupport() const {
  AIEBaseInstrInfo::JNZDSupport Result;

  Result.MovBlockAddrOpcode = AIE2PS::MOVXM_lng_cg;
  Result.PointerRegisterClass = &AIE2PS::eP_as_32BitRegClass;
  Result.LoopDecOpcode = AIE2PS::LoopDec;
  Result.LoopJNZOpcode = AIE2PS::LoopJNZ;
  Result.DecTripCountOpcode = AIE2PS::ADDM_NC_mv_add_ri;
  Result.LoopJNZDOpcode = AIE2PS::PseudoJNZD;
  return Result;
}

using IfConvSupport = AIEBaseInstrInfo::IfConvSupport;
std::optional<IfConvSupport> AIE2PSInstrInfo::getIfConvSupport() const {
  IfConvSupport Result;

  Result.BranchToSelectMap[AIE2PS::PseudoJNZ] = AIE2PS::SEL_NEZ;
  Result.BranchToSelectMap[AIE2PS::PseudoJZ] = AIE2PS::SEL_EQZ;

  Result.ScalarRegisterClass = &AIE2PS::eRRegClass;
  Result.SelectRegisterClass = &AIE2PS::mR27_selectRegClass;

  Result.registerOperandIndex(IfConvSupport::TrueReg, 0);
  Result.registerOperandIndex(IfConvSupport::FalseReg, 1);
  Result.registerOperandIndex(IfConvSupport::ConditionReg, 2);

  return Result;
}

Register AIE2PSInstrInfo::getPackSignCReg() const { return AIE2PS::packSign0; }

Register AIE2PSInstrInfo::getUnpackSignCReg() const {
  return AIE2PS::unpackSign0;
}

Register AIE2PSInstrInfo::getSSStatusReg() const { return AIE2PS::srSS0; }

Register AIE2PSInstrInfo::getMSStatusReg() const { return AIE2PS::srMS0; }

unsigned AIE2PSInstrInfo::getMoveToMSOpcode(MachineInstr &I,
                                            unsigned ConstTLastVal) const {
  const bool UseTLastImm = (ConstTLastVal == 0);
  const unsigned IntrinsicID = cast<GIntrinsic>(I).getIntrinsicID();
  switch (IntrinsicID) {
  case Intrinsic::aie2ps_put_ms:
    return UseTLastImm ? AIE2PS::MOV_st_mMStream_tlast_imm : AIE2PS::MOV_tlast;
  case Intrinsic::aie2ps_put_ms_nb:
    return UseTLastImm ? AIE2PS::MOV_nb_st_mMStream_tlast_imm
                       : AIE2PS::MOV_nb_tlast;
  default:
    llvm_unreachable("Unexpected Intrinsic ID");
  }
}

unsigned AIE2PSInstrInfo::getScalarRegSize() const { return 32; }

unsigned AIE2PSInstrInfo::getBasicVecRegSize() const { return 256; }

unsigned AIE2PSInstrInfo::getBasicVectorBitSize() const { return 512; }

unsigned AIE2PSInstrInfo::getMaxVectorBitSize() const { return 2048; }

unsigned AIE2PSInstrInfo::getMaxSupportedLdStIncSize() const { return 2048; }

unsigned AIE2PSInstrInfo::getGenericAddVectorEltOpcode() const {
  return AIE2PS::G_AIE_ADD_VECTOR_ELT_HI;
}

unsigned AIE2PSInstrInfo::getGenericInsertVectorEltOpcode() const {
  return AIE2PS::G_AIE_INSERT_VECTOR_ELT;
}

unsigned AIE2PSInstrInfo::getGenericExtractVectorEltOpcode(bool SignExt) const {
  return SignExt ? AIE2PS::G_AIE_SEXT_EXTRACT_VECTOR_ELT
                 : AIE2PS::G_AIE_ZEXT_EXTRACT_VECTOR_ELT;
}

unsigned AIE2PSInstrInfo::getGenericUnpadVectorOpcode() const {
  return AIE2PS::G_AIE_UNPAD_VECTOR;
}

unsigned AIE2PSInstrInfo::getGenericPadVectorOpcode() const {
  return AIE2PS::G_AIE_PAD_VECTOR_UNDEF;
}

unsigned AIE2PSInstrInfo::getGenericBroadcastVectorOpcode() const {
  return AIE2PS::G_AIE_BROADCAST_VECTOR;
}

unsigned AIE2PSInstrInfo::getGenericVSelOpcode() const {
  return AIE2PS::G_AIE_VSEL;
}

unsigned AIE2PSInstrInfo::getGenericVShiftOpcode() const {
  return AIE2PS::G_AIE_VSHIFT_RIGHT;
}

unsigned AIE2PSInstrInfo::getGenericPostIncLoadOpcode() const {
  return AIE2PS::G_AIE_POSTINC_LOAD;
}

unsigned AIE2PSInstrInfo::getGenericPostIncStoreOpcode() const {
  return AIE2PS::G_AIE_POSTINC_STORE;
}

unsigned AIE2PSInstrInfo::getGenericShuffleVectorOpcode() const {
  return AIE2PS::G_AIE_SHUFFLE_VECTOR;
}

unsigned AIE2PSInstrInfo::getGenericExtractSubvectorOpcode() const {
  return AIE2PS::G_AIE_EXTRACT_SUBVECTOR;
}

unsigned AIE2PSInstrInfo::getGenericIntegerComparisonOpcode() const {
  return AIE2PS::G_AIE_VECTOR_ICMP;
}

// Note: Some pseudos like spill/reload are already expanded in
// eliminateFrameIndex.
bool AIE2PSInstrInfo::expandPostRAPseudo(MachineInstr &MI) const {
  auto DL = MI.getDebugLoc();
  MachineBasicBlock &MBB = *MI.getParent();
  switch (MI.getOpcode()) {
  case AIE2PS::PseudoMove: {
    Register Dst = MI.getOperand(0).getReg();
    Register Src = MI.getOperand(1).getReg();
    const unsigned MOVSclOpcode = getScalarMovOpcode(Dst, Src);
    BuildMI(MBB, MI, DL, get(MOVSclOpcode), Dst)
        .addReg(Src, getKillRegState(MI.getOperand(1).isKill()));
    MI.eraseFromParent();
    return true;
  }
  }
  return false;
}

static AIEAlternateDescriptors &getSelectedAltDescs(const ScheduleDAGMI *DAG) {
  if (DAG->hasVRegLiveness())
    return static_cast<const AIEScheduleDAGMILive *>(DAG)
        ->getSchedImpl()
        ->getSelectedAltDescs();
  return static_cast<const AIEScheduleDAGMI *>(DAG)
      ->getSchedImpl()
      ->getSelectedAltDescs();
}

ScheduleHazardRecognizer *AIE2PSInstrInfo::CreateTargetMIHazardRecognizer(
    const InstrItineraryData *II, const ScheduleDAGMI *DAG) const {
  // AIE has a fully exposed pipeline, resource and format conflicts must be
  // exactly modelled.
  return new AIEHazardRecognizer(this, II, getSelectedAltDescs(DAG),
                                 /*IsPreRA=*/DAG->hasVRegLiveness());
}

bool AIE2PSInstrInfo::isDelayedSchedBarrier(const MachineInstr &MI) const {
  return MI.getOpcode() == AIE2PS::DelayedSchedBarrier;
}

bool AIE2PSInstrInfo::isSchedBarrier(const MachineInstr &MI) const {
  return (MI.getOpcode() == AIE2PS::SCHED_BARRIER ||
          MI.getOpcode() == AIE2PS::PseudoLoopEnd ||
          MI.getOpcode() == AIE2PS::MOV_alu_mv_mv_mv_cntr2l ||
          isDelayedSchedBarrier(MI));
}

unsigned
AIE2PSInstrInfo::getNumReservedDelaySlots(const MachineInstr &MI) const {
  return 0;
}

std::pair<unsigned, unsigned>
AIE2PSInstrInfo::decomposeMachineOperandsTargetFlags(unsigned TF) const {
  const unsigned Mask = AIEII::MO_DIRECT_FLAG_MASK;
  return std::make_pair(TF & Mask, TF & ~Mask);
}

ArrayRef<std::pair<unsigned, const char *>>
AIE2PSInstrInfo::getSerializableDirectMachineOperandTargetFlags() const {
  using namespace AIEII;
  static const std::pair<unsigned, const char *> TargetFlags[] = {
      {MO_GLOBAL, "aie2ps-global"}};
  return ArrayRef(TargetFlags);
}

SmallVector<TiedRegOperands, 4>
AIE2PSInstrInfo::getTiedRegInfo(unsigned Opcode) const {
  const SmallVector<SubRegSplit, 8> Split2DReg = {
      SubRegSplit(AIE2PS::sub_mod), SubRegSplit(AIE2PS::sub_dim_size),
      SubRegSplit(AIE2PS::sub_dim_stride), SubRegSplit(AIE2PS::sub_dim_count)};
  const SmallVector<SubRegSplit, 8> Split3DReg = {
      SubRegSplit(AIE2PS::sub_mod),
      SubRegSplit(AIE2PS::sub_dim_size),
      SubRegSplit(AIE2PS::sub_dim_stride),
      SubRegSplit(AIE2PS::sub_dim_count),
      SubRegSplit(AIE2PS::sub_hi_dim_then_sub_mod, /*IsUndef=*/true),
      SubRegSplit(AIE2PS::sub_hi_dim_then_sub_dim_size),
      SubRegSplit(AIE2PS::sub_hi_dim_then_sub_dim_stride),
      SubRegSplit(AIE2PS::sub_hi_dim_then_sub_dim_count)};
  switch (Opcode) {
  // ========================================================================
  // STD_2D_LOAD: Standard 2D Load Instructions WITH vector dst (25
  // instructions)
  // Signature: (outs DST:$dst, eP:$ptr_out, eDC:$dc), (ins
  // eP:$ptr, eD:$mod)
  // dc at output position 2, mod at input position 1 → absolute OpIdx 4
  // Tied: dc (OpIdx=2) = mod.sub_dim_count (OpIdx=4)
  // ========================================================================
  case AIE2PS::LDA_2D_dml_lda_scalar_L:
  case AIE2PS::LDA_2D_dml_lda_scalar_F:
  case AIE2PS::LDA_2D_dml_lda2_scalar_EE:
  case AIE2PS::LDA_2D_dml_lda2_scalar_EG:
  case AIE2PS::LDA_2D_dml_lda2_scalar_GG:
  case AIE2PS::LDA_2D_dms_lda_scalar:
  case AIE2PS::LDA_2D_dms_lda_g:
  case AIE2PS::LDA_2D_dmv_lda_f:
  case AIE2PS::LDA_2D_dmv_lda_feg:
  case AIE2PS::LDA_2D_dmv_lda_eg2:
  case AIE2PS::LDA_2D_s16:
  case AIE2PS::LDA_2D_s8:
  case AIE2PS::LDA_2D_u16:
  case AIE2PS::LDA_2D_u8:
  case AIE2PS::LDA_TM_2D:
  case AIE2PS::VLDA_2D_128:
  case AIE2PS::VLDA_2D_CONV_fp32_bf16_dmw_lda_ups_bf16:
  case AIE2PS::VLDA_2D_CONV_fp32_bf16_dmx_lda_ups_f16:
  case AIE2PS::VLDA_2D_CONV_fp32_fp16:
  case AIE2PS::VLDA_2D_dmw_lda_w:
  case AIE2PS::VLDA_2D_dmw_lda_feg2:
  case AIE2PS::VLDA_2D_dmx_lda_x:
  case AIE2PS::VLDA_2D_dmx_lda_bm:
  case AIE2PS::VLDB_2D_128:
  case AIE2PS::VLD_2D_128_pseudo:
  case AIE2PS::VLDB_2D_UNPACK_dmw_ldb_unpack_unpackSign0:
  case AIE2PS::VLDB_2D_UNPACK_dmw_ldb_unpack_unpackSign1:
  case AIE2PS::VLDB_2D_UNPACK_dmx_ldb_unpack_unpackSign0:
  case AIE2PS::VLDB_2D_UNPACK_dmx_ldb_unpack_unpackSign1:
  case AIE2PS::VLDB_2D_dmw_ldb:
  case AIE2PS::VLDB_2D_dmx_ldb_x:
  case AIE2PS::VLD_2D_w_pseudo:
  case AIE2PS::VLD_2D_x_pseudo:
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/2, /*SubRegIdx=*/AIE2PS::sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/4, /*SubRegIdx=*/AIE2PS::NoSubRegister,
                    /*SubRegsSplit=*/Split2DReg}}}};

  // ========================================================================
  // STD_2D_STORE: Standard 2D Store Instructions (17 instructions)
  // Signature: (outs eP:$ptr_out, eDC:$dc), (ins SRC:$src, eP:$ptr, eD:$mod)
  // dc at output position 1, mod at input position 2 → absolute OpIdx 4
  // Tied: dc (OpIdx=1) = mod.sub_dim_count (OpIdx=4)
  // ========================================================================
  case AIE2PS::ST_2D_dml_sts_scalar:
  case AIE2PS::ST_2D_dms_sts_scalar:
  case AIE2PS::ST_2D_dms_sts_g:
  case AIE2PS::ST_2D_dmv_sts_f:
  case AIE2PS::ST_2D_s16:
  case AIE2PS::ST_2D_s8:
  case AIE2PS::ST_TM_2D:
  case AIE2PS::VST_2D_128_dmv_sts_eg:
  case AIE2PS::VST_2D_128_dmv_sts_feg:
  case AIE2PS::VST_2D_128_dmv_sts_w:
  case AIE2PS::VST_2D_CONV_bf16_fp32_dmw_sts_srs_bf16:
  case AIE2PS::VST_2D_CONV_bf16_fp32_dmx_sts_srs_bf16:
  case AIE2PS::VST_2D_CONV_fp16_fp32:
  case AIE2PS::VST_2D_PACK_dmw_sts_pack_packSign0:
  case AIE2PS::VST_2D_PACK_dmw_sts_pack_packSign1:
  case AIE2PS::VST_2D_PACK_dmx_sts_pack_packSign0:
  case AIE2PS::VST_2D_PACK_dmx_sts_pack_packSign1:
  case AIE2PS::VST_2D_dmw_sts_w:
  case AIE2PS::VST_2D_dmw_sts_feg2:
  case AIE2PS::VST_2D_dmx_sts_x:
  case AIE2PS::VST_2D_dmx_sts_bm:
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2PS::sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/4, /*SubRegIdx=*/AIE2PS::NoSubRegister,
                    /*SubRegsSplit=*/Split2DReg}}}};

  // ========================================================================
  // STD_2D_STORE: Stores with eS:$i operand (2 instructions)
  // Signature: (outs eP:$ptr_out, eDC:$dc), (ins SRC:$src, eS:$i, eP:$ptr,
  // eD:$mod)
  // dc at output position 1, mod at input position 3 → absolute OpIdx 5
  // Tied: dc (OpIdx=1) = mod.sub_dim_count (OpIdx=5)
  // ========================================================================
  case AIE2PS::VST_2D_CONV_fp8_fp32:
  case AIE2PS::VST_2D_CONV_bf8_fp32:
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2PS::sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/5, /*SubRegIdx=*/AIE2PS::NoSubRegister,
                    /*SubRegsSplit=*/Split2DReg}}}};

  // ========================================================================
  // STD_2D_STORE: Stores with extra operands (mod at OpIdx 5) (8 instructions)
  // Signature: (outs eP:$ptr_out, eDC:$dc), (ins SRC:$src, ..., eP:$ptr,
  // eD:$mod)
  // dc at output position 1, mod at input position 4 → absolute OpIdx 5
  // Tied: dc (OpIdx=1) = mod.sub_dim_count (OpIdx=5)
  // ========================================================================
  case AIE2PS::VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign0:
  case AIE2PS::VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign1:
  case AIE2PS::VST_2D_SRS_2x_dm_sts_srs_cm_srsSign0:
  case AIE2PS::VST_2D_SRS_2x_dm_sts_srs_cm_srsSign1:
  case AIE2PS::VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign0:
  case AIE2PS::VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign1:
  case AIE2PS::VST_2D_SRS_4x_dm_sts_srs_cm_srsSign0:
  case AIE2PS::VST_2D_SRS_4x_dm_sts_srs_cm_srsSign1:
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2PS::sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/5, /*SubRegIdx=*/AIE2PS::NoSubRegister,
                    /*SubRegsSplit=*/Split2DReg}}}};

  // ========================================================================
  // VST_FLUSH_2D: 2D Vector Store Flush Instructions (2 instructions)
  // Signature: (outs eS_or_FP:$base_out, eP:$ptr_out, ePush:$push_out,
  // eDC:$dc), (ins VX_512:$src, eS_or_FP:$base, eP:$ptr, ePush:$push, eD:$mod)
  // dc at output position 3, mod at input position 4 → absolute OpIdx 7
  // Tied: dc (OpIdx=3) = mod.sub_dim_count (OpIdx=7)
  // ========================================================================
  case AIE2PS::VST_FLUSH_2D:
  case AIE2PS::VST_FLUSH_d_2D:
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/3, /*SubRegIdx=*/AIE2PS::sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/7, /*SubRegIdx=*/AIE2PS::NoSubRegister,
                    /*SubRegsSplit=*/Split2DReg}}}};

  // ========================================================================
  // STD_3D_LOAD: Standard 3D Load Instructions (26 instructions)
  // Signature: (outs DST:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins
  // eP:$ptr, eDS:$mod)
  // dcl at output position 2, dch at position 3, mod at
  // input position 1 → absolute OpIdx 5
  // Tied: dcl (OpIdx=2) = mod.sub_dim_count,
  //       dch (OpIdx=3) = mod.sub_hi_dim_then_sub_dim_count (OpIdx=5)
  // ========================================================================
  case AIE2PS::LDA_3D_dml_lda_scalar_L:
  case AIE2PS::LDA_3D_dml_lda_scalar_F:
  case AIE2PS::LDA_3D_dml_lda2_scalar_EE:
  case AIE2PS::LDA_3D_dml_lda2_scalar_EG:
  case AIE2PS::LDA_3D_dml_lda2_scalar_GG:
  case AIE2PS::LDA_3D_dms_lda_scalar:
  case AIE2PS::LDA_3D_dms_lda_g:
  case AIE2PS::LDA_3D_dmv_lda_feg:
  case AIE2PS::LDA_3D_dmv_lda_f:
  case AIE2PS::LDA_3D_dmv_lda_eg2:
  case AIE2PS::LDA_3D_s16:
  case AIE2PS::LDA_3D_s8:
  case AIE2PS::LDA_3D_u16:
  case AIE2PS::LDA_3D_u8:
  case AIE2PS::LDA_TM_3D:
  case AIE2PS::VLDA_3D_128:
  case AIE2PS::VLD_3D_128_pseudo:
  case AIE2PS::VLDA_3D_CONV_fp32_bf16_dmw_lda_ups_bf16:
  case AIE2PS::VLDA_3D_CONV_fp32_bf16_dmx_lda_ups_f16:
  case AIE2PS::VLDA_3D_CONV_fp32_fp16:
  case AIE2PS::VLDA_3D_dmw_lda_w:
  case AIE2PS::VLDA_3D_dmw_lda_feg2:
  case AIE2PS::VLDA_3D_dmx_lda_bm:
  case AIE2PS::VLDA_3D_dmx_lda_x:
  case AIE2PS::VLDB_3D_128:
  case AIE2PS::VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign0:
  case AIE2PS::VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign1:
  case AIE2PS::VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign0:
  case AIE2PS::VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign1:
  case AIE2PS::VLDB_3D_dmw_ldb:
  case AIE2PS::VLDB_3D_dmx_ldb_x:
  case AIE2PS::VLD_3D_w_pseudo:
  case AIE2PS::VLD_3D_x_pseudo:
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2PS::sub_dim_count},
            {/*OpIdx=*/3, /*SubRegIdx=*/AIE2PS::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/5, /*SubRegIdx=*/AIE2PS::NoSubRegister,
                    /*SubRegsSplit=*/Split3DReg}}}};

  // ========================================================================
  // STD_3D_STORE: Standard 3D Store Instructions (19 instructions)
  // Signature: (outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins SRC:$src,
  // eP:$ptr, eDS:$mod)
  // dcl at output position 1, dch at position 2, mod at
  // input position 2 → absolute OpIdx 5
  // Tied: dcl (OpIdx=1) = mod.sub_dim_count,
  //       dch (OpIdx=2) = mod.sub_hi_dim_then_sub_dim_count (OpIdx=5)
  // ========================================================================
  case AIE2PS::ST_3D_dml_sts_scalar:
  case AIE2PS::ST_3D_dms_sts_scalar:
  case AIE2PS::ST_3D_dms_sts_g:
  case AIE2PS::ST_3D_dmv_sts_f:
  case AIE2PS::ST_3D_s16:
  case AIE2PS::ST_3D_s8:
  case AIE2PS::ST_TM_3D:
  case AIE2PS::VST_3D_128_dmv_sts_eg:
  case AIE2PS::VST_3D_128_dmv_sts_feg:
  case AIE2PS::VST_3D_128_dmv_sts_w:
  case AIE2PS::VST_3D_CONV_bf16_fp32_dmw_sts_srs_bf16:
  case AIE2PS::VST_3D_CONV_bf16_fp32_dmx_sts_srs_bf16:
  case AIE2PS::VST_3D_CONV_fp16_fp32:
  case AIE2PS::VST_3D_PACK_dmw_sts_pack_packSign0:
  case AIE2PS::VST_3D_PACK_dmw_sts_pack_packSign1:
  case AIE2PS::VST_3D_PACK_dmx_sts_pack_packSign0:
  case AIE2PS::VST_3D_PACK_dmx_sts_pack_packSign1:
  case AIE2PS::VST_3D_dmw_sts_w:
  case AIE2PS::VST_3D_dmw_sts_feg2:
  case AIE2PS::VST_3D_dmx_sts_bm:
  case AIE2PS::VST_3D_dmx_sts_x:
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/1, /*SubRegIdx=*/AIE2PS::sub_dim_count},
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2PS::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/5, /*SubRegIdx=*/AIE2PS::NoSubRegister,
                    /*SubRegsSplit=*/Split3DReg}}}};

    // ========================================================================
  // STD_3D_STORE: Stores with eS:$i operand (2 instructions)
  // Signature: (outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins SRC:$src, eS:$i,
  // eP:$ptr, eDS:$mod)
  // dcl at output position 1, dch at position 2, mod at
  // input position 3 → absolute OpIdx 6
  // Tied: dcl (OpIdx=1) = mod.sub_dim_count,
  //       dch (OpIdx=2) = mod.sub_hi_dim_then_sub_dim_count (OpIdx=6)
  // ========================================================================
  case AIE2PS::VST_3D_CONV_fp8_fp32:
  case AIE2PS::VST_3D_CONV_bf8_fp32:
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/1, /*SubRegIdx=*/AIE2PS::sub_dim_count},
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2PS::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/6, /*SubRegIdx=*/AIE2PS::NoSubRegister,
                    /*SubRegsSplit=*/Split3DReg}}}};

  // ========================================================================
  // STD_3D_STORE: Stores with extra operands (mod at OpIdx 6) (8 instructions)
  // Signature: (outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins SRC:$src, ...,
  // eP:$ptr, eDS:$mod)
  // dcl at output position 1, dch at position 2, mod at
  // input position 4 → absolute OpIdx 6
  // Tied: dcl (OpIdx=1) = mod.sub_dim_count,
  //       dch (OpIdx=2) = mod.sub_hi_dim_then_sub_dim_count (OpIdx=6)
  // ========================================================================
  case AIE2PS::VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign0:
  case AIE2PS::VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign1:
  case AIE2PS::VST_3D_SRS_2x_dm_sts_srs_cm_srsSign0:
  case AIE2PS::VST_3D_SRS_2x_dm_sts_srs_cm_srsSign1:
  case AIE2PS::VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign0:
  case AIE2PS::VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign1:
  case AIE2PS::VST_3D_SRS_4x_dm_sts_srs_cm_srsSign0:
  case AIE2PS::VST_3D_SRS_4x_dm_sts_srs_cm_srsSign1:
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/1, /*SubRegIdx=*/AIE2PS::sub_dim_count},
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2PS::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/6, /*SubRegIdx=*/AIE2PS::NoSubRegister,
                    /*SubRegsSplit=*/Split3DReg}}}};

  // ========================================================================
  // UPS_2D_LOAD: 2D UPS Load Instructions with base operand (8 instructions)
  // Signature: (outs DST:$dst, eP:$ptr_out, eDC:$dc), (ins eS:$base, eP:$ptr,
  // eD:$mod)
  // dc at output position 2, mod at input position 2 → absolute OpIdx 5
  // Tied: dc (OpIdx=2) = mod.sub_dim_count (OpIdx=5)
  // ========================================================================
  case AIE2PS::VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign0:
  case AIE2PS::VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign1:
  case AIE2PS::VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign0:
  case AIE2PS::VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign1:
  case AIE2PS::VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign0:
  case AIE2PS::VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign1:
  case AIE2PS::VLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsSign0:
  case AIE2PS::VLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsSign1:
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/2, /*SubRegIdx=*/AIE2PS::sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/5, /*SubRegIdx=*/AIE2PS::NoSubRegister,
                    /*SubRegsSplit=*/Split2DReg}}}};

  // ========================================================================
  // UPS_3D_LOAD: 3D UPS Load Instructions with base operand (8 instructions)
  // Signature: (outs DST:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins
  // eS:$base, eP:$ptr, eDS:$mod)
  // dcl at output position 2, dch at position 3,
  // mod at input position 2 → absolute OpIdx 6
  // Tied: dcl (OpIdx=2) = mod.sub_dim_count,
  //       dch (OpIdx=3) = mod.sub_hi_dim_then_sub_dim_count (OpIdx=6)
  // ========================================================================
  case AIE2PS::VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign0:
  case AIE2PS::VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign1:
  case AIE2PS::VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign0:
  case AIE2PS::VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign1:
  case AIE2PS::VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign0:
  case AIE2PS::VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign1:
  case AIE2PS::VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign0:
  case AIE2PS::VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign1:
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2PS::sub_dim_count},
            {/*OpIdx=*/3, /*SubRegIdx=*/AIE2PS::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/6, /*SubRegIdx=*/AIE2PS::NoSubRegister,
                    /*SubRegsSplit=*/Split3DReg}}}};

  // ========================================================================
  // VST_FLUSH_3D: 3D Vector Store Flush Instructions (2 instructions)
  // Signature: (outs eS_or_FP:$base_out, eP:$ptr_out, ePush:$push_out,
  // eDCL:$dcl, eDCH:$dch), (ins VX_512:$src, eS_or_FP:$base, eP:$ptr,
  // ePush:$push, eDS:$mod)
  // dcl at output position 3, dch at position 4, mod at input position 4 →
  // absolute OpIdx 8 Tied: dcl (OpIdx=3) = mod.sub_dim_count,
  //       dch (OpIdx=4) = mod.sub_hi_dim_then_sub_dim_count (OpIdx=8)
  // ========================================================================
  case AIE2PS::VST_FLUSH_3D:
  case AIE2PS::VST_FLUSH_d_3D:
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/3, /*SubRegIdx=*/AIE2PS::sub_dim_count},
            {/*OpIdx=*/4, /*SubRegIdx=*/AIE2PS::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/8, /*SubRegIdx=*/AIE2P::NoSubRegister,
                    /*SubRegsSplit=*/Split3DReg}}}};

  // ========================================================================
  // PADD_2D: 2D Pointer Increment Instructions (4 instructions)
  // Signature: (outs eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod)
  // dc at output position 1, mod at input position 1 → absolute OpIdx 3
  // Tied: dc (OpIdx=1) = mod.sub_dim_count (OpIdx=3)
  // ========================================================================
  case AIE2PS::PADDA_2D:
  case AIE2PS::PADDB_2D:
  case AIE2PS::PADDS_2D:
  case AIE2PS::PADD_2D_pseudo:
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2PS::sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/3, /*SubRegIdx=*/AIE2PS::NoSubRegister,
                    /*SubRegsSplit=*/Split2DReg}}}};

  // ========================================================================
  // PADD_3D: 3D Pointer Increment Instructions (4 instructions)
  // Signature: (outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr,
  // eDS:$mod) dcl at output position 1, dch at position 2, mod at input
  // position 1 → absolute OpIdx 4
  // Tied: dcl (OpIdx=1) = mod.sub_dim_count,
  //       dch (OpIdx=2) = mod.sub_hi_dim_then_sub_dim_count (OpIdx=4)
  // ========================================================================
  case AIE2PS::PADDA_3D:
  case AIE2PS::PADDB_3D:
  case AIE2PS::PADDS_3D:
  case AIE2PS::PADD_3D_pseudo:
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/1, /*SubRegIdx=*/AIE2PS::sub_dim_count},
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2PS::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/4, /*SubRegIdx=*/AIE2PS::NoSubRegister,
                    /*SubRegsSplit=*/Split3DReg}}}};

  // ========================================================================
  // FILL Instructions (3 instructions)
  // Signature: (outs ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, mRF2x:$pos_out),
  //            (ins ePS:$ptr, eLdFifoReg:$fifo_reg, mRF2x:$pos)
  // Tied: ptr_out (OpIdx=0) ← ptr (OpIdx=3)
  //       fifo_reg_out (OpIdx=1) ← fifo_reg (OpIdx=4)
  //       pos_out (OpIdx=2) ← pos (OpIdx=5)
  // ========================================================================
  case AIE2PS::VLDA_FILL:
  case AIE2PS::VLDB_FILL:
  case AIE2PS::VLD_FILL_512_pseudo:
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/0, /*SubRegIdx=*/AIE2PS::sub_ptr},
                    {/*OpIdx=*/1, /*SubRegIdx=*/AIE2PS::sub_fifo},
                    {/*OpIdx=*/2, /*SubRegIdx=*/AIE2PS::sub_avail}},
        /*SrcOps=*/
        {{/*OpIdx=*/3, /*SubRegIdx=*/AIE2PS::sub_ptr},
         {/*OpIdx=*/4, /*SubRegIdx=*/AIE2PS::sub_fifo},
         {/*OpIdx=*/5, /*SubRegIdx=*/AIE2PS::sub_avail}},
        /*NewSuperClass=*/&AIE2PS::ePSRFLdFRegClass}};

  // ========================================================================
  // FILLX Instruction (1 instruction)
  // Signature: (outs ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, mRF2b:$pos_out),
  //            (ins mR30_fifo_step_e1:$conf_e1, mR30_fifo_step_e7:$conf_e7,
  //                 ePS:$ptr, eLdFifoReg:$fifo_reg, mRF2b:$pos)
  // Tied: ptr_out (OpIdx=0) ← ptr (OpIdx=5)
  //       fifo_reg_out (OpIdx=1) ← fifo_reg (OpIdx=6)
  //       pos_out (OpIdx=2) ← pos (OpIdx=7)
  // ========================================================================
  case AIE2PS::VLDB_FILLX:
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/0, /*SubRegIdx=*/AIE2PS::sub_ptr},
                    {/*OpIdx=*/1, /*SubRegIdx=*/AIE2PS::sub_fifo},
                    {/*OpIdx=*/2, /*SubRegIdx=*/AIE2PS::sub_avail}},
        /*SrcOps=*/
        {{/*OpIdx=*/5, /*SubRegIdx=*/AIE2PS::sub_ptr},
         {/*OpIdx=*/6, /*SubRegIdx=*/AIE2PS::sub_fifo},
         {/*OpIdx=*/7, /*SubRegIdx=*/AIE2PS::sub_avail}},
        /*NewSuperClass=*/&AIE2PS::ePSRFLdFRegClass}};

  // ========================================================================
  // POPX Instruction (1 instruction)
  // Signature: (outs mXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out,
  //                  mRF2b:$pos_out),
  //            (ins mR30_fifo_step_e1:$conf_e1, mR30_fifo_step_e7:$conf_e7,
  //                 ePS:$ptr, eLdFifoReg:$fifo_reg, mRF2b:$pos)
  // Tied: ptr_out (OpIdx=1) ← ptr (OpIdx=6)
  //       fifo_reg_out (OpIdx=2) ← fifo_reg (OpIdx=7)
  //       pos_out (OpIdx=3) ← pos (OpIdx=8)
  // ========================================================================
  case AIE2PS::VLDB_POPX:
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2PS::sub_ptr},
                    {/*OpIdx=*/2, /*SubRegIdx=*/AIE2PS::sub_fifo},
                    {/*OpIdx=*/3, /*SubRegIdx=*/AIE2PS::sub_avail}},
        /*SrcOps=*/
        {{/*OpIdx=*/6, /*SubRegIdx=*/AIE2PS::sub_ptr},
         {/*OpIdx=*/7, /*SubRegIdx=*/AIE2PS::sub_fifo},
         {/*OpIdx=*/8, /*SubRegIdx=*/AIE2PS::sub_avail}},
        /*NewSuperClass=*/&AIE2PS::ePSRFLdFRegClass}};

  // ========================================================================
  // Normal POP Instructions (9 instructions: 6 VLDA/VLDB + 3 pseudos)
  // Signature: (outs DST:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out,
  //                  mRF2x:$pos_out),
  //            (ins ePS:$ptr, eLdFifoReg:$fifo_reg, mRF2x:$pos)
  // Tied: ptr_out (OpIdx=1) ← ptr (OpIdx=4)
  //       fifo_reg_out (OpIdx=2) ← fifo_reg (OpIdx=5)
  //       pos_out (OpIdx=3) ← pos (OpIdx=6)
  // ========================================================================
  case AIE2PS::VLDA_POP_dmx_lda_fifo_x_normal_pop:
  case AIE2PS::VLDA_POP_dmx_lda_fifo_ex_bfp16_normal_pop:
  case AIE2PS::VLDA_POP_dmx_lda_fifo_fex_bfp13_normal_pop:
  case AIE2PS::VLDB_POP_dmx_ldb_fifo_x_normal_pop:
  case AIE2PS::VLDB_POP_dmx_ldb_fifo_ex_bfp16_normal_pop:
  case AIE2PS::VLDB_POP_dmx_ldb_fifo_fex_bfp13_normal_pop:
  case AIE2PS::VLD_POP_512_normal_pop_pseudo:
  case AIE2PS::VLD_POP_640_normal_pop_pseudo:
  case AIE2PS::VLD_POP_768_normal_pop_pseudo:
  case AIE2PS::VLDB_POP_s_normal_pop:
  case AIE2PS::VLDA_POP_s_normal_pop:
  case AIE2PS::VLD_POP_s_normal_pop_pseudo:
  case AIE2PS::VLDB_POP_mx4_normal_pop:
  case AIE2PS::VLDA_POP_mx4_normal_pop:
  case AIE2PS::VLD_POP_mx4_normal_pop_pseudo:
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2PS::sub_ptr},
                    {/*OpIdx=*/2, /*SubRegIdx=*/AIE2PS::sub_fifo},
                    {/*OpIdx=*/3, /*SubRegIdx=*/AIE2PS::sub_avail}},
        /*SrcOps=*/
        {{/*OpIdx=*/4, /*SubRegIdx=*/AIE2PS::sub_ptr},
         {/*OpIdx=*/5, /*SubRegIdx=*/AIE2PS::sub_fifo},
         {/*OpIdx=*/6, /*SubRegIdx=*/AIE2PS::sub_avail}},
        /*NewSuperClass=*/&AIE2PS::ePSRFLdFRegClass}};

  // ========================================================================
  // FIFO 1D POP Instructions (9 instructions: 6 VLDA/VLDB + 3 pseudos)
  // Signature: (outs DST:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out,
  //                  mRF2x:$pos_out),
  //            (ins ePS:$ptr, eLdFifoReg:$fifo_reg, mRF2x:$pos, eM:$mod)
  // Tied: ptr_out (OpIdx=1) ← ptr (OpIdx=4)
  //       fifo_reg_out (OpIdx=2) ← fifo_reg (OpIdx=5)
  //       pos_out (OpIdx=3) ← pos (OpIdx=6)
  // ========================================================================
  case AIE2PS::VLDA_POP_dmx_lda_fifo_x_fifo_1d_pop:
  case AIE2PS::VLDA_POP_dmx_lda_fifo_ex_bfp16_fifo_1d_pop:
  case AIE2PS::VLDA_POP_dmx_lda_fifo_fex_bfp13_fifo_1d_pop:
  case AIE2PS::VLDB_POP_dmx_ldb_fifo_x_fifo_1d_pop:
  case AIE2PS::VLDB_POP_dmx_ldb_fifo_ex_bfp16_fifo_1d_pop:
  case AIE2PS::VLDB_POP_dmx_ldb_fifo_fex_bfp13_fifo_1d_pop:
  case AIE2PS::VLD_POP_512_fifo_1d_pop_pseudo:
  case AIE2PS::VLD_POP_640_fifo_1d_pop_pseudo:
  case AIE2PS::VLD_POP_768_fifo_1d_pop_pseudo:
  case AIE2PS::VLDB_POP_s_fifo_1d_pop:
  case AIE2PS::VLDA_POP_s_fifo_1d_pop:
  case AIE2PS::VLD_POP_s_fifo_1d_pop_pseudo:
  case AIE2PS::VLDB_POP_mx4_fifo_1d_pop:
  case AIE2PS::VLDA_POP_mx4_fifo_1d_pop:
  case AIE2PS::VLD_POP_mx4_fifo_1d_pop_pseudo:
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2PS::sub_ptr},
                    {/*OpIdx=*/2, /*SubRegIdx=*/AIE2PS::sub_fifo},
                    {/*OpIdx=*/3, /*SubRegIdx=*/AIE2PS::sub_avail}},
        /*SrcOps=*/
        {{/*OpIdx=*/4, /*SubRegIdx=*/AIE2PS::sub_ptr},
         {/*OpIdx=*/5, /*SubRegIdx=*/AIE2PS::sub_fifo},
         {/*OpIdx=*/6, /*SubRegIdx=*/AIE2PS::sub_avail}},
        /*NewSuperClass=*/&AIE2PS::ePSRFLdFRegClass}};

  // ========================================================================
  // VLD_POP_2D: 2D FIFO Pop Load Instructions (9 instructions)
  // Signature: (outs DST:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out,
  //                  mRF2x:$pos_out, eDC:$dc),
  //            (ins ePS:$ptr, eLdFifoReg:$fifo_reg, mRF2x:$pos, eD:$mod)
  // Tied: ptr_out (OpIdx=1) ← ptr (OpIdx=5)
  //       fifo_reg_out (OpIdx=2) ← fifo_reg (OpIdx=6)
  //       pos_out (OpIdx=3) ← pos (OpIdx=7)
  //       dc (OpIdx=4) = mod.sub_dim_count (OpIdx=8)
  // ========================================================================
  case AIE2PS::VLDB_POP_2D_dmx_ldb_fifo_x:
  case AIE2PS::VLDB_POP_2D_dmx_ldb_fifo_ex_bfp16:
  case AIE2PS::VLDB_POP_2D_dmx_ldb_fifo_fex_bfp13:
  case AIE2PS::VLDA_POP_2D_dmx_lda_fifo_x:
  case AIE2PS::VLDA_POP_2D_dmx_lda_fifo_ex_bfp16:
  case AIE2PS::VLDA_POP_2D_dmx_lda_fifo_fex_bfp13:
  case AIE2PS::VLD_POP_512_2D_pseudo:
  case AIE2PS::VLD_POP_640_2D_pseudo:
  case AIE2PS::VLD_POP_768_2D_pseudo:
  case AIE2PS::VLDB_POP_s_2D:
  case AIE2PS::VLDA_POP_s_2D:
  case AIE2PS::VLD_POP_s_2D_pseudo:
  case AIE2PS::VLDA_POP_mx4_2D:
  case AIE2PS::VLDB_POP_mx4_2D:
  case AIE2PS::VLD_POP_mx4_2D_pseudo:
    return {TiedRegOperands{
                /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2PS::sub_ptr},
                            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2PS::sub_fifo},
                            {/*OpIdx=*/3, /*SubRegIdx=*/AIE2PS::sub_avail}},
                /*SrcOps=*/
                {{/*OpIdx=*/5, /*SubRegIdx=*/AIE2PS::sub_ptr},
                 {/*OpIdx=*/6, /*SubRegIdx=*/AIE2PS::sub_fifo},
                 {/*OpIdx=*/7, /*SubRegIdx=*/AIE2PS::sub_avail}},
                /*NewSuperClass=*/&AIE2PS::ePSRFLdFRegClass},
            TiedRegOperands{
                /*DstOps=*/{{/*OpIdx=*/4, /*SubRegIdx=*/AIE2PS::sub_dim_count}},
                /*SrcOp=*/
                {{/*OpIdx=*/8, /*SubRegIdx=*/AIE2PS::NoSubRegister,
                  /*SubRegsSplit=*/Split2DReg}}}};

  // ========================================================================
  // VLD_POP_3D: 3D FIFO Pop Load Instructions (9 instructions)
  // Signature: (outs DST:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out,
  //                  mRF2x:$pos_out, eDCL:$dcl, eDCH:$dch),
  //            (ins ePS:$ptr, eLdFifoReg:$fifo_reg, mRF2x:$pos, eDS:$mod)
  // Tied: ptr_out (OpIdx=1) ← ptr (OpIdx=6)
  //       fifo_reg_out (OpIdx=2) ← fifo_reg (OpIdx=7)
  //       pos_out (OpIdx=3) ← pos (OpIdx=8)
  //       dcl (OpIdx=4) = mod.sub_dim_count,
  //       dch (OpIdx=5) = mod.sub_hi_dim_then_sub_dim_count (OpIdx=9)
  // ========================================================================
  case AIE2PS::VLDB_POP_3D_dmx_ldb_fifo_x:
  case AIE2PS::VLDB_POP_3D_dmx_ldb_fifo_ex_bfp16:
  case AIE2PS::VLDB_POP_3D_dmx_ldb_fifo_fex_bfp13:
  case AIE2PS::VLDA_POP_3D_dmx_lda_fifo_x:
  case AIE2PS::VLDA_POP_3D_dmx_lda_fifo_ex_bfp16:
  case AIE2PS::VLDA_POP_3D_dmx_lda_fifo_fex_bfp13:
  case AIE2PS::VLD_POP_512_3D_pseudo:
  case AIE2PS::VLD_POP_640_3D_pseudo:
  case AIE2PS::VLD_POP_768_3D_pseudo:
  case AIE2PS::VLDB_POP_s_3D:
  case AIE2PS::VLDA_POP_s_3D:
  case AIE2PS::VLD_POP_s_3D_pseudo:
  case AIE2PS::VLDB_POP_mx4_3D:
  case AIE2PS::VLDA_POP_mx4_3D:
  case AIE2PS::VLD_POP_mx4_3D_pseudo:
    return {
        TiedRegOperands{
            /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2PS::sub_ptr},
                        {/*OpIdx=*/2, /*SubRegIdx=*/AIE2PS::sub_fifo},
                        {/*OpIdx=*/3, /*SubRegIdx=*/AIE2PS::sub_avail}},
            /*SrcOps=*/
            {{/*OpIdx=*/6, /*SubRegIdx=*/AIE2PS::sub_ptr},
             {/*OpIdx=*/7, /*SubRegIdx=*/AIE2PS::sub_fifo},
             {/*OpIdx=*/8, /*SubRegIdx=*/AIE2PS::sub_avail}},
            /*NewSuperClass=*/&AIE2PS::ePSRFLdFRegClass},
        TiedRegOperands{
            /*DstOps=*/{{/*OpIdx=*/4, /*SubRegIdx=*/AIE2PS::sub_dim_count},
                        {/*OpIdx=*/5,
                         /*SubRegIdx=*/AIE2PS::sub_hi_dim_then_sub_dim_count}},
            /*SrcOp=*/
            {{/*OpIdx=*/9, /*SubRegIdx=*/AIE2PS::NoSubRegister,
              /*SubRegsSplit=*/Split3DReg}}}};
  case AIE2PS::VMUL_f_vmul_bfp16:
  case AIE2PS::VNEGMUL_f_vmul_bfp16:
    return {
        TiedRegOperands{/*DstOps=*/{}, /*SrcOps=*/
                        {{/*OpIdx=*/1, /*SubRegIdx=*/AIE2PS::sub_bfp320_lo},
                         {/*OpIdx=*/4, /*SubRegIdx=*/AIE2PS::sub_bfp320_hi}},
                        /*NewSuperClass=*/&AIE2PS::mEXmRegClass}};
  case AIE2PS::VMAC_f_vmac_bfp16:
  case AIE2PS::VMSC_f_vmac_bfp16:
    return {
        TiedRegOperands{/*DstOps=*/{}, /*SrcOps=*/
                        {{/*OpIdx=*/2, /*SubRegIdx=*/AIE2PS::sub_bfp320_lo},
                         {/*OpIdx=*/5, /*SubRegIdx=*/AIE2PS::sub_bfp320_hi}},
                        /*NewSuperClass=*/&AIE2PS::mEXmRegClass}};
  case AIE2PS::VADDMAC_f_vaddmac_bfp16:
  case AIE2PS::VADDMSC_f_vaddmac_bfp16:
    return {
        TiedRegOperands{/*DstOps=*/{}, /*SrcOps=*/
                        {{/*OpIdx=*/3, /*SubRegIdx=*/AIE2PS::sub_bfp320_lo},
                         {/*OpIdx=*/6, /*SubRegIdx=*/AIE2PS::sub_bfp320_hi}},
                        /*NewSuperClass=*/&AIE2PS::mEXmRegClass}};

  default:
    return {};
  }
}

TiedRegOperands
AIE2PSInstrInfo::getTiedRegInfoForSplitting(unsigned Opcode) const {
  const auto &TiedRegInfoVector = getTiedRegInfo(Opcode);
  unsigned Size = TiedRegInfoVector.size();
  assert(Size >= 1 && "Expected to have tied register info");

  for (auto &TiedRegInfo : TiedRegInfoVector) {
    if (TiedRegInfo.SrcOps.size() > 1)
      continue;
    if (TiedRegInfo.canSplitSrcOps())
      return TiedRegInfo;
  }
  return {};
}

AIEBaseInstrInfo::AIERegOffsetSpillInstrInfo
AIE2PSInstrInfo::getRegOffsetSpillInstrInfoFromImmOffset(
    const unsigned Opcode) const {
  switch (Opcode) {
  case AIE2PS::ST_dms_sts_scalar_spill:
    return {AIE2PS::ST_dms_sts_scalar_st_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::ST_dml_sts_scalar_spill:
    return {AIE2PS::ST_dml_sts_scalar_st_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::VST_128_dmv_sts_w_spill:
    return {AIE2PS::VST_128_dmv_sts_w_st_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::VST_dmw_sts_w_spill:
    return {AIE2PS::VST_dmw_sts_w_st_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::VST_dmx_sts_x_spill:
    return {AIE2PS::VST_dmx_sts_x_st_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::VST_dmx_sts_bm_spill:
    return {AIE2PS::VST_dmx_sts_bm_st_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::ST_dmv_sts_f_spill:
    return {AIE2PS::ST_dmv_sts_f_st_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::ST_dms_sts_g_spill:
    return {AIE2PS::ST_dms_sts_g_st_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::VST_128_dmv_sts_eg_spill:
    return {AIE2PS::VST_128_dmv_sts_eg_st_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::VST_128_dmv_sts_feg_spill:
    return {AIE2PS::VST_128_dmv_sts_feg_st_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::VST_dmw_sts_feg2_spill:
    return {AIE2PS::VST_dmw_sts_feg2_st_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::LDA_dms_lda_scalar_spill:
    return {AIE2PS::LDA_dms_lda_scalar_ld_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::LDA_dml_lda_scalar_L_spill:
    return {AIE2PS::LDA_dml_lda_scalar_L_ld_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::VLDA_128_dmv_lda_w_spill:
    return {AIE2PS::VLDA_128_dmv_lda_w_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::VLDA_dmw_lda_w_spill:
    return {AIE2PS::VLDA_dmw_lda_w_ld_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::VLDA_dmx_lda_x_spill:
    return {AIE2PS::VLDA_dmx_lda_x_ld_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::VLDA_dmx_lda_bm_spill:
    return {AIE2PS::VLDA_dmx_lda_bm_ld_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::LDA_dml_lda2_scalar_EE_spill:
    return {AIE2PS::LDA_dml_lda2_scalar_EE_ld_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::LDA_dml_lda_scalar_F_spill:
    return {AIE2PS::LDA_dml_lda_scalar_F_ld_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::LDA_dmv_lda_f_spill:
    return {AIE2PS::LDA_dmv_lda_f_ld_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::LDA_dms_lda_g_spill:
    return {AIE2PS::LDA_dms_lda_g_ld_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::LDA_dml_lda2_scalar_GG_spill:
    return {AIE2PS::LDA_dml_lda2_scalar_GG_ld_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::LDA_dml_lda2_scalar_EG_spill:
    return {AIE2PS::LDA_dml_lda2_scalar_EG_ld_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::LDA_dmv_lda_eg2_spill:
    return {AIE2PS::LDA_dmv_lda_eg2_ld_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::LDA_dmv_lda_feg_spill:
    return {AIE2PS::LDA_dmv_lda_feg_ld_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  case AIE2PS::VLDA_dmw_lda_feg2_spill:
    return {AIE2PS::VLDA_dmw_lda_feg2_ld_idx, AIE2PS::MOVXM_lng_cg,
            &AIE2PS::eDJ_as_32BitRegClass};
  default:
    llvm_unreachable("Offset register spill instruction info un-implemented");
  }
}

using AbstractOp = AIEBaseInstrInfo::AbstractOp;

ArrayRef<AIEBaseInstrInfo::WidenNarrowConversionPair>
AIE2PSInstrInfo::getWidenNarrowConversionPairs() const {
  using namespace Intrinsic;
  static const WidenNarrowConversionPair Pairs[] = {
      {aie2ps_v16accfloat_to_v16bf16, aie2ps_v16bf16_to_v16accfloat},
      {aie2ps_v32accfloat_to_v32bf16, aie2ps_v32bf16_to_v32accfloat},
  };
  return Pairs;
}

std::optional<const AbstractOp>
AIE2PSInstrInfo::parseAbstractOp(const MachineInstr &MI) const {

  switch (MI.getOpcode()) {
  case AIE2PS::VADD_32:
    return AbstractOp{AbstractOp::OperationType::VECTOR_ADD,
                      {MI.getOperand(1).getReg(), MI.getOperand(2).getReg()},
                      {}};
  case AIE2PS::VBCST_32:
    return AbstractOp{AbstractOp::OperationType::VECTOR_BROADCAST,
                      {},
                      {MI.getOperand(1).getReg()}};
  case AIE2PS::VSEL_32:
    return AbstractOp{AbstractOp::OperationType::VECTOR_SELECT,
                      {MI.getOperand(1).getReg(), MI.getOperand(2).getReg()},
                      {MI.getOperand(3).getReg()}};
  case AIE2PS::VLDB_4x16_lo:
  case AIE2PS::VLDB_4x16_hi:
  case AIE2PS::VLDB_4x32_lo:
  case AIE2PS::VLDB_4x32_hi:
  case AIE2PS::VLDB_4x64_lo:
  case AIE2PS::VLDB_4x64_hi:
    return AbstractOp{AbstractOp::OperationType::VECTOR_XWAY_LOAD,
                      {MI.getOperand(1).getReg()},
                      {}};
  }
  return std::nullopt;
}
