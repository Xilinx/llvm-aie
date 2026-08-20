//===- AIE2PInstrInfo.cpp -AIE2p Instruction Information -*------- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIE2p implementation of the TargetInstrInfo
// class.
//
//===----------------------------------------------------------------------===//

#include "AIE2PInstrInfo.h"
#include "AIE2PRegisterInfo.h"
#include "AIE2PSubtarget.h"
#include "AIE2PTargetMachine.h"
#include "AIEBaseInstrInfo.h"
#include "AIEHazardRecognizer.h"
#include "AIEMachineFunctionInfo.h"
#include "AIEMachineScheduler.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "MCTargetDesc/aie2p/AIE2PMCTargetDesc.h"
#include "llvm/CodeGen/GlobalISel/GenericMachineInstrs.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/IR/IntrinsicsAIE2.h"
#include "llvm/IR/IntrinsicsAIE2P.h"

#define DEBUG_TYPE "aie-codegen"

using namespace llvm;

#define GET_COPY_MATERIALIZATION_IMPL
#include "AIE2PGenCopyMaterialization.inc"

#define GET_INSTRINFO_CTOR_DTOR
#include "AIE2PGenInstrInfo.inc"

#include "AIE2PGenMemoryCycles.inc"
#include "AIE2PGenPreSchedLowering.inc"
#include "AIE2PGenSplitInstrTables.inc"
#include "AIE2PGenVarInstructionItin.inc"

namespace {
const AIE2PMCFormats AIE2PFormats;
} // namespace

AIE2PInstrInfo::AIE2PInstrInfo()
    : AIE2PGenInstrInfo(AIE2P::ADJCALLSTACKUP, AIE2P::ADJCALLSTACKDOWN) {
  FormatInterface = &AIE2PFormats;
  FuncUnitWrapper::setFormatInterface(FormatInterface);
}

unsigned AIE2PInstrInfo::getReturnOpcode() const { return AIE2P::PseudoRET; }

unsigned AIE2PInstrInfo::getCallOpcode(const MachineFunction &CallerF,
                                       bool IsIndirect, bool IsTailCall) const {
  if (IsTailCall)
    return IsIndirect ? AIE2P::PseudoJ_TCO_jump_ind
                      : AIE2P::PseudoJ_TCO_jump_imm;
  return IsIndirect ? AIE2P::PseudoJL_IND : AIE2P::PseudoJL;
}

unsigned AIE2PInstrInfo::getNopOpcode() const { return AIE2P::NOP; }

unsigned AIE2PInstrInfo::getMvSclOpcode() const {
  return AIE2P::MOV_alu_mv_mv_mv_scl;
}

unsigned AIE2PInstrInfo::getAddrIntrinsic2D() const {
  return Intrinsic::aie2p_add_2d;
}

unsigned AIE2PInstrInfo::getAddrIntrinsic3D() const {
  return Intrinsic::aie2p_add_3d;
}

unsigned AIE2PInstrInfo::getIdOnlyLockIntrinsic(unsigned PtrID) const {
  switch (PtrID) {
  case Intrinsic::aie2p_acquire_ptr:
    return Intrinsic::aie2p_acquire;
  case Intrinsic::aie2p_acquire_cond_ptr:
    return Intrinsic::aie2p_acquire_cond;
  case Intrinsic::aie2p_release_ptr:
    return Intrinsic::aie2p_release;
  case Intrinsic::aie2p_release_cond_ptr:
    return Intrinsic::aie2p_release_cond;
  default:
    return Intrinsic::not_intrinsic;
  }
}

unsigned AIE2PInstrInfo::getPtrAdd2DOpcode() const {
  return AIE2P::PADD_2D_pseudo;
}

unsigned AIE2PInstrInfo::getPtrAdd3DOpcode() const {
  return AIE2P::PADD_3D_pseudo;
}

unsigned AIE2PInstrInfo::getMvSclMultiSlotPseudoOpcode() const {
  return AIE2P::MOV_scalar_imm11_pseudo;
}

unsigned AIE2PInstrInfo::getAddSclOpcode() const { return AIE2P::ADD_alu_r_rr; }

unsigned AIE2PInstrInfo::getOppositeBranchOpcode(unsigned Opc) const {
  switch (Opc) {
  default:
    llvm_unreachable("Unrecognized conditional branch");
  case AIE2P::PseudoJNZ:
    return AIE2P::PseudoJZ;
  case AIE2P::PseudoJZ:
    return AIE2P::PseudoJNZ;
  }
  return 0;
}

bool AIE2PInstrInfo::isJNZ(unsigned Opc) const {
  return Opc == AIE2P::PseudoJNZ;
}

bool AIE2PInstrInfo::isJZ(unsigned Opc) const { return Opc == AIE2P::PseudoJZ; }

bool AIE2PInstrInfo::jumpsToUnknown(unsigned Opc) const {
  // The use-case of this function is to check whether all our successor blocks
  // are known after pseudo expansion.
  // This relies on having correct successor information to start with, and
  // since our actual branch instructions aren't interpreted after pseudo
  // expansion, the successor information should be carried explicitly from
  // earlier stages.
  // The only cases where we transfer control to an unknown block are calls
  // and returns. We may see calls in bottom regions with a fallthrough.
  // All other branches that are not fully static are created from static
  // branch constructs, which should have supplied and maintained successor
  // information
  return Opc == AIE2P::RET || Opc == AIE2P::JL_lng || Opc == AIE2P::JL_alumv_or;
}

bool AIE2PInstrInfo::isCall(unsigned Opc) const {
  return Opc == AIE2P::JL_alumv_or || Opc == AIE2P::JL_lng;
}

bool AIE2PInstrInfo::isIConst(unsigned Opc) const {
  switch (Opc) {
  case AIE2P::MOVA:
  case AIE2P::MOV_alu_mv_mv_mv_cg:
  case AIE2P::MOVXM:
  case AIE2P::MOVX_alu_cg:
  case AIE2P::MOVX_mvx_cr_imm:
  case AIE2P::MOV_RLC_imm11_pseudo:
  case AIE2P::MOV_PD_imm11_pseudo:
  case AIE2P::MOV_S_imm11_pseudo:
  case AIE2P::MOV_scalar_imm11_pseudo:
    return true;
  default:
    return false;
  }
}

bool AIE2PInstrInfo::isBooleanNoOp(unsigned Opc) const {
  return Opc == AIE2P::NEZ || AIEBaseInstrInfo::isBooleanNoOp(Opc);
}

bool AIE2PInstrInfo::isBooleanNot(unsigned Opc) const {
  return Opc == AIE2P::EQZ;
}

bool AIE2PInstrInfo::isConstStep(const MachineInstr &MI, int64_t &Step) const {
  unsigned Opcode = MI.getOpcode();
  if (Opcode == AIE2P::ADD_add_r_ri) {
    Step = MI.getOperand(2).getImm();
    return true;
  }
  return false;
}

bool AIE2PInstrInfo::verifyGenericInstruction(const MachineInstr &MI,
                                              StringRef &ErrInfo) const {
  const MachineRegisterInfo &MRI = MI.getMF()->getRegInfo();
  switch (MI.getOpcode()) {
  case AIE2P::G_AIE_INSERT_VECTOR_ELT:
    if (MRI.getType(MI.getOperand(0).getReg()).getSizeInBits() !=
        AIE2PInstrInfo::getBasicVectorBitSize()) {
      if (isLegalized(MI)) {
        ErrInfo = "Operation is only legal for 512-bit vector destinations";
        return false;
      }
    }
    return true;
  case AIE2P::G_AIE_ZEXT_EXTRACT_VECTOR_ELT:
  case AIE2P::G_AIE_SEXT_EXTRACT_VECTOR_ELT:
    if (MRI.getType(MI.getOperand(1).getReg()).getSizeInBits() != 512) {
      if (isLegalized(MI)) {
        ErrInfo = "Operation is only legal for 512-bit vector sources";
        return false;
      }
    }
    ErrInfo = "Expected 32/64bit scalar destination";
    return MRI.getType(MI.getOperand(0).getReg()) == LLT::scalar(32) ||
           MRI.getType(MI.getOperand(0).getReg()) == LLT::scalar(64);
  case AIE2P::G_AIE_PAD_VECTOR_UNDEF:
    return verifySameLaneTypes(MI, ErrInfo) &&
           isLegalTypeToUnpad(MRI.getType(MI.getOperand(0).getReg()),
                              &ErrInfo) &&
           isLegalTypeToPad(MRI.getType(MI.getOperand(1).getReg()), &ErrInfo);
  case AIE2P::G_AIE_UNPAD_VECTOR:
    return verifySameLaneTypes(MI, ErrInfo) &&
           isLegalTypeToPad(MRI.getType(MI.getOperand(0).getReg()), &ErrInfo) &&
           isLegalTypeToUnpad(MRI.getType(MI.getOperand(1).getReg()), &ErrInfo);
  case AIE2P::G_AIE_ADD_VECTOR_ELT_HI: {
    const LLT DstTy = MRI.getType(MI.getOperand(0).getReg());
    const LLT SrcTy = MRI.getType(MI.getOperand(2).getReg());
    // This operation is only supported on the basic vector length
    if (DstTy.getSizeInBits() != AIE2PInstrInfo::getBasicVectorBitSize()) {
      ErrInfo = "Operation is only legal for 512-bit vector destinations";
      return false;
    }
    // This operation only supports 32-/64-bit scalar operands
    if (!(MRI.getType(MI.getOperand(2).getReg()) == LLT::scalar(32) ||
          MRI.getType(MI.getOperand(2).getReg()) == LLT::scalar(64))) {
      ErrInfo = "Expected 32/64bit scalar source";
      return false;
    }

    // Element types >= 32-bit must match scalar size (8/16-bit scalars are
    // extended to 32-bit)
    if (DstTy.getScalarSizeInBits() >= 32 && DstTy.getScalarType() != SrcTy) {
      ErrInfo = "Scalar size must match vector element size";
      return false;
    }
    return true;
  }
  default:
    return true;
  }
}

// If we lose memory operands on accesses to some of our special
// memory regions, we may be applying unsafe memory disambiguation.
// Hence, we check these accesses in the machine verifier.
bool AIE2PInstrInfo::verifyMemOperand(const MachineInstr &MI,
                                      StringRef &ErrInfo) const {
  auto CheckMemOp = [&](MachineMemOperand::Flags Flag, AIETargetPSV PSVKind) {
    for (auto &MMO : MI.memoperands()) {
      if (!(MMO->getFlags() & Flag)) {
        continue;
      }
      auto *PSV = MMO->getPseudoValue();
      if (PSV && PSV->kind() == PSVKind) {
        return true;
      }
    }
    return false;
  };

  switch (MI.getOpcode()) {
  case AIE2P::LDA_TM_idx:
  case AIE2P::LDA_TM_idx_imm:
  case AIE2P::LDA_TM_pstm_nrm:
  case AIE2P::LDA_TM_pstm_nrm_imm:
  case AIE2P::LDA_TM_2D:
  case AIE2P::LDA_TM_3D:
    if (!CheckMemOp(MachineMemOperand::Flags::MOLoad, AIETileMem)) {
      ErrInfo = "Required Load TileMemory MemOperand not found";
      return false;
    }
    break;
  case AIE2P::ST_TM_idx:
  case AIE2P::ST_TM_idx_imm:
  case AIE2P::ST_TM_pstm_nrm:
  case AIE2P::ST_TM_pstm_nrm_imm:
  case AIE2P::ST_TM_2D:
  case AIE2P::ST_TM_3D:
    if (!CheckMemOp(MachineMemOperand::Flags::MOStore, AIETileMem)) {
      ErrInfo = "Required Store TileMemory MemOperand not found";
      return false;
    }
    break;
  default:
    break;
  }
  return true;
}

unsigned AIE2PInstrInfo::getJumpOpcode() const {
  return AIE2P::PseudoJ_jump_imm;
}

unsigned AIE2PInstrInfo::getPseudoMoveOpcode() const {
  return AIE2P::PseudoMove;
}

unsigned AIE2PInstrInfo::getOffsetMemOpcode(unsigned BaseMemOpcode) const {
  switch (BaseMemOpcode) {
  case TargetOpcode::G_STORE:
    return AIE2P::G_AIE_OFFSET_STORE;
  case TargetOpcode::G_LOAD:
    return AIE2P::G_AIE_OFFSET_LOAD;
  case TargetOpcode::G_SEXTLOAD:
    return AIE2P::G_AIE_OFFSET_SEXTLOAD;
  case TargetOpcode::G_ZEXTLOAD:
    return AIE2P::G_AIE_OFFSET_ZEXTLOAD;
  }
  llvm_unreachable("not a generic load/store");
}

bool AIE2PInstrInfo::isGenericOffsetMemOpcode(unsigned Opcode) const {
  return ((Opcode == AIE2P::G_AIE_OFFSET_STORE) ||
          (Opcode == AIE2P::G_AIE_OFFSET_LOAD) ||
          (Opcode == AIE2P::G_AIE_OFFSET_SEXTLOAD) ||
          (Opcode == AIE2P::G_AIE_OFFSET_ZEXTLOAD));
}

bool AIE2PInstrInfo::isGenericMemOpcode(unsigned Opcode) const {
  if (isGenericOffsetMemOpcode(Opcode))
    return true;
  switch (Opcode) {
  case TargetOpcode::G_LOAD:
  case TargetOpcode::G_SEXTLOAD:
  case TargetOpcode::G_ZEXTLOAD:
  case TargetOpcode::G_STORE:
  case AIE2P::G_AIE_POSTINC_LOAD:
  case AIE2P::G_AIE_POSTINC_SEXTLOAD:
  case AIE2P::G_AIE_POSTINC_ZEXTLOAD:
  case AIE2P::G_AIE_POSTINC_STORE:
  case AIE2P::G_AIE_POSTINC_2D_LOAD:
  case AIE2P::G_AIE_POSTINC_2D_SEXTLOAD:
  case AIE2P::G_AIE_POSTINC_2D_ZEXTLOAD:
  case AIE2P::G_AIE_POSTINC_2D_STORE:
  case AIE2P::G_AIE_POSTINC_3D_LOAD:
  case AIE2P::G_AIE_POSTINC_3D_SEXTLOAD:
  case AIE2P::G_AIE_POSTINC_3D_ZEXTLOAD:
  case AIE2P::G_AIE_POSTINC_3D_STORE:
    return true;
  default:
    return false;
  }
}

bool AIE2PInstrInfo::isFifoStoreConvOpcode(unsigned Opcode) const {
  return ((Opcode == AIE2P::VST_PUSH_544_CONV_bfp16ebs16_ebs8) ||
          (Opcode == AIE2P::VST_PUSH_544_CONV_bfp16ebs16_fp32) ||
          (Opcode == AIE2P::VST_PUSH_576_CONV_bfp16ebs8_fp32));
}

int AIE2PInstrInfo::isRoundRobinSlotCandidate(MachineInstr &MI) const {
  const unsigned Opc = MI.getOpcode();
  if (Opc == AIE2P::VLD_FILL_512_pseudo)
    return 1;
  return 0;
}

std::optional<unsigned>
AIE2PInstrInfo::getStoreFlushConvOpcode(unsigned StoreFlushOpcode) const {
  switch (StoreFlushOpcode) {
  case AIE2P::VST_FLUSH_512_normal_flush:
    return AIE2P::VST_FLUSH_512_CONV_normal_flush;
  case AIE2P::VST_FLUSH_512_fifo_1d_flush:
    return AIE2P::VST_FLUSH_512_CONV_fifo_1d_flush;
  case AIE2P::VST_FLUSH_512_2D:
    return AIE2P::VST_FLUSH_512_CONV_2D;
  case AIE2P::VST_FLUSH_512_3D:
    return AIE2P::VST_FLUSH_512_CONV_3D;
  }
  return std::nullopt;
}

std::optional<unsigned> AIE2PInstrInfo::getCombinedPostIncOpcode(
    MachineInstr &BaseMemI, MachineInstr &PostIncI, TypeSize Size) const {
  switch (PostIncI.getOpcode()) {
  case TargetOpcode::G_PTR_ADD:
    switch (BaseMemI.getOpcode()) {
    case TargetOpcode::G_STORE:
      return AIE2P::G_AIE_POSTINC_STORE;
    case TargetOpcode::G_LOAD:
      return AIE2P::G_AIE_POSTINC_LOAD;
    case TargetOpcode::G_SEXTLOAD:
      return AIE2P::G_AIE_POSTINC_SEXTLOAD;
    case TargetOpcode::G_ZEXTLOAD:
      return AIE2P::G_AIE_POSTINC_ZEXTLOAD;
    }
    break;
  case TargetOpcode::G_INTRINSIC:
    switch (cast<GIntrinsic>(PostIncI).getIntrinsicID()) {
    case Intrinsic::aie2p_add_2d:
      switch (BaseMemI.getOpcode()) {
      case TargetOpcode::G_STORE:
        return AIE2P::G_AIE_POSTINC_2D_STORE;
      case TargetOpcode::G_LOAD:
        return AIE2P::G_AIE_POSTINC_2D_LOAD;
      case TargetOpcode::G_SEXTLOAD:
        return AIE2P::G_AIE_POSTINC_2D_SEXTLOAD;
      case TargetOpcode::G_ZEXTLOAD:
        return AIE2P::G_AIE_POSTINC_2D_ZEXTLOAD;
      }
      break;
    case Intrinsic::aie2p_add_3d:
      switch (BaseMemI.getOpcode()) {
      case TargetOpcode::G_STORE:
        return AIE2P::G_AIE_POSTINC_3D_STORE;
      case TargetOpcode::G_LOAD:
        return AIE2P::G_AIE_POSTINC_3D_LOAD;
      case TargetOpcode::G_SEXTLOAD:
        return AIE2P::G_AIE_POSTINC_3D_SEXTLOAD;
      case TargetOpcode::G_ZEXTLOAD:
        return AIE2P::G_AIE_POSTINC_3D_ZEXTLOAD;
      }
      break;
    }
    break;
  }
  return {};
}

unsigned AIE2PInstrInfo::getOpCode(MachineInstr &I) const {
  const MachineRegisterInfo &MRI = I.getMF()->getRegInfo();
  unsigned IntrinsicID = cast<GIntrinsic>(I).getIntrinsicID();
  switch (IntrinsicID) {
    // vconv bfp16
  case Intrinsic::aie2p_v64accfloat_to_v64bfp16ebs8:
    return AIE2P::VCONV_bfp16ebs8_fp32;
  case Intrinsic::aie2p_v64accfloat_to_v64bfp16ebs16:
    return AIE2P::VCONV_bfp16ebs16_fp32;
  case Intrinsic::aie2p_v64bfp16ebs8_to_v64bfp16ebs16:
    return AIE2P::VCONV_bfp16ebs16_ebs8;
  // vsrs
  case Intrinsic::aie2p_I256_v16_acc32_srs:
  case Intrinsic::aie2p_I256_v8_acc64_srs:
    return AIE2P::VSRS_2x_mv_w_srs_bm_srsSign0;
  case Intrinsic::aie2p_I512_v32_acc32_srs:
  case Intrinsic::aie2p_I512_v16_acc64_srs:
    return AIE2P::VSRS_2x_mv_x_srs_cm_srsSign0;
  case Intrinsic::aie2p_I512_v64_acc32_srs:
  case Intrinsic::aie2p_I512_v32_acc64_srs:
    return AIE2P::VSRS_4x_mv_x_srs_dm_srsSign0;
  case Intrinsic::aie2p_I256_v32_acc32_srs:
  case Intrinsic::aie2p_I256_v16_acc64_srs:
    return AIE2P::VSRS_4x_mv_w_srs_cm_srsSign0;
  // vups
  case Intrinsic::aie2p_acc32_v16_I256_ups:
  case Intrinsic::aie2p_acc64_v8_I256_ups:
    return AIE2P::VUPS_2x_mv_ups_w2b_upsSign0;
  case Intrinsic::aie2p_acc32_v32_I256_ups:
  case Intrinsic::aie2p_acc64_v16_I256_ups:
    return AIE2P::VUPS_4x_mv_ups_w2c_upsSign0;
  case Intrinsic::aie2p_acc32_v32_I512_ups:
  case Intrinsic::aie2p_acc64_v16_I512_ups:
    return AIE2P::VUPS_2x_mv_ups_x2c_upsSign0;
  case Intrinsic::aie2p_acc32_v64_I512_ups:
  case Intrinsic::aie2p_acc64_v32_I512_ups:
    return AIE2P::VUPS_4x_mv_ups_x2d_upsSign0;
  // VMOV - Cascade stream read access
  case Intrinsic::aie2p_scd_read_vec:
    return AIE2P::VMOV_lda_mv_scd_x;
  case Intrinsic::aie2p_scd_read_acc32:
    return AIE2P::VMOV_lda_mv_scd_bm;
  case Intrinsic::aie2p_scd_expand_lo:
    return AIE2P::VMOV_0_mv_scd_cm;
  case Intrinsic::aie2p_scd_expand_hi:
    return AIE2P::VMOV_1_mv_scd_cm;
  case Intrinsic::aie2p_scd_ACC2048: {
    Register SrcReg = I.getOperand(3).getReg();
    if (auto Src = getIConstantVRegValWithLookThrough(SrcReg, MRI)) {
      unsigned SrcConstVal = Src->Value.getZExtValue();
      switch (SrcConstVal) {
      case 0:
        return AIE2P::VMOV_0_mv_scd_dm_imm;
      case 1:
        return AIE2P::VMOV_1_mv_scd_dm_imm;
      case 2:
        return AIE2P::VMOV_2;
      case 3:
        return AIE2P::VMOV_3;
      default:
        llvm_unreachable("Unexpected SrcConstVal for SCD");
      }
    }
    llvm_unreachable("Unexpected Reached Here");
  }
  case Intrinsic::aie2p_scd_expand_ACC1024:
  case Intrinsic::aie2p_scd_expand_ACC2048:
    return AIE2P::VMOV_lda_mv_scd_dm_reg;
  case Intrinsic::aie2p_scd_expand_ACC1024_incr:
  case Intrinsic::aie2p_scd_expand_ACC2048_incr:
    return AIE2P::VMOV_lda_mv_scd_dm_dyn;
  case Intrinsic::aie2p_get_ss:
    return AIE2P::MOV_lda;
  case Intrinsic::aie2p_get_ss_nb:
    return AIE2P::MOV_nb_lda;
  // VMOV - Cascade stream write access
  case Intrinsic::aie2p_mcd_write_vec:
    return AIE2P::VMOV_st_mv_mcd_x;
  case Intrinsic::aie2p_mcd_write_acc32:
    return AIE2P::VMOV_st_mv_mcd_bm;
  // Vmax Intrinsic
  case Intrinsic::aie2p_vmax_lt8:
    return AIE2P::VMAX_LT_8_vaddSign0;
  case Intrinsic::aie2p_vmax_lt16:
    return AIE2P::VMAX_LT_16_vaddSign0;
  case Intrinsic::aie2p_vmax_lt32:
    return AIE2P::VMAX_LT_32_vaddSign0;
  // Vmin Intrinsic
  case Intrinsic::aie2p_vmin_ge8:
    return AIE2P::VMIN_GE_8_vaddSign0;
  case Intrinsic::aie2p_vmin_ge16:
    return AIE2P::VMIN_GE_16_vaddSign0;
  case Intrinsic::aie2p_vmin_ge32:
    return AIE2P::VMIN_GE_32_vaddSign0;
  // VGE / VLT
  case Intrinsic::aie2p_vlt8:
    return AIE2P::VLT_8_vaddSign0;
  case Intrinsic::aie2p_vlt16:
    return AIE2P::VLT_16_vaddSign0;
  case Intrinsic::aie2p_vlt32:
    return AIE2P::VLT_32_vaddSign0;
  case Intrinsic::aie2p_vge8:
    return AIE2P::VGE_8_vaddSign0;
  case Intrinsic::aie2p_vge16:
    return AIE2P::VGE_16_vaddSign0;
  case Intrinsic::aie2p_vge32:
    return AIE2P::VGE_32_vaddSign0;
    // VMAXDIFF_LT
  case Intrinsic::aie2p_vmaxdiff_lt8:
    return AIE2P::VMAXDIFF_LT_8_vaddSign0;
  case Intrinsic::aie2p_vmaxdiff_lt16:
    return AIE2P::VMAXDIFF_LT_16_vaddSign0;
  case Intrinsic::aie2p_vmaxdiff_lt32:
    return AIE2P::VMAXDIFF_LT_32_vaddSign0;
  // VABS_GTZ
  case Intrinsic::aie2p_vabs_gtz8:
    return AIE2P::VABS_GTZ_8_vaddSign0;
  case Intrinsic::aie2p_vabs_gtz16:
    return AIE2P::VABS_GTZ_16_vaddSign0;
  case Intrinsic::aie2p_vabs_gtz32:
    return AIE2P::VABS_GTZ_32_vaddSign0;
  // VSUB_LT/VSUB_GE
  case Intrinsic::aie2p_vsub_lt8:
    return AIE2P::VSUB_LT_8_vaddSign0;
  case Intrinsic::aie2p_vsub_lt16:
    return AIE2P::VSUB_LT_16_vaddSign0;
  case Intrinsic::aie2p_vsub_lt32:
    return AIE2P::VSUB_LT_32_vaddSign0;
  case Intrinsic::aie2p_vsub_ge8:
    return AIE2P::VSUB_GE_8_vaddSign0;
  case Intrinsic::aie2p_vsub_ge16:
    return AIE2P::VSUB_GE_16_vaddSign0;
  case Intrinsic::aie2p_vsub_ge32:
    return AIE2P::VSUB_GE_32_vaddSign0;
  case Intrinsic::aie2p_BFP576_BFP576_ACC2048_mul_conf:
    return AIE2P::VMUL_f_vmul_bfp_vmul_bfp_core_EX_EX;
  case Intrinsic::aie2p_BFP576_BFP576_ACC2048_negmul_conf:
    return AIE2P::VNEGMUL_f_vmul_bfp_vmul_bfp_core_EX_EX;
  case Intrinsic::aie2p_BFP576_BFP576_ACC2048_mac_conf:
    return AIE2P::VMAC_f_vmac_bfp_vmul_bfp_core_EX_EX;
  case Intrinsic::aie2p_BFP576_BFP576_ACC2048_msc_conf:
    return AIE2P::VMSC_f_vmac_bfp_vmul_bfp_core_EX_EX;
  case Intrinsic::aie2p_BFP576_BFP576_ACC2048_addmac_conf:
    return AIE2P::VADDMAC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EX_EX;
  case Intrinsic::aie2p_BFP576_BFP576_ACC2048_addmsc_conf:
    return AIE2P::VADDMSC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EX_EX;
  case Intrinsic::aie2p_BFP576_BFP1152_ACC2048_mul_conf:
    return AIE2P::VMUL_f_vmul_bfp_vmul_bfp_core_EX_EY;
  case Intrinsic::aie2p_BFP576_BFP1152_ACC2048_negmul_conf:
    return AIE2P::VNEGMUL_f_vmul_bfp_vmul_bfp_core_EX_EY;
  case Intrinsic::aie2p_BFP576_BFP1152_ACC2048_mac_conf:
    return AIE2P::VMAC_f_vmac_bfp_vmul_bfp_core_EX_EY;
  case Intrinsic::aie2p_BFP576_BFP1152_ACC2048_msc_conf:
    return AIE2P::VMSC_f_vmac_bfp_vmul_bfp_core_EX_EY;
  case Intrinsic::aie2p_BFP576_BFP1152_ACC2048_addmac_conf:
    return AIE2P::VADDMAC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EX_EY;
  case Intrinsic::aie2p_BFP576_BFP1152_ACC2048_addmsc_conf:
    return AIE2P::VADDMSC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EX_EY;
  // Pack
  case Intrinsic::aie2p_pack_I1024_I8_I16:
  case Intrinsic::aie2p_pack_I1024_I4_I8:
  case Intrinsic::aie2p_pack_I512_I8_I16:
  case Intrinsic::aie2p_pack_I512_I4_I8: {
    Register SignReg = I.getOperand(3).getReg();
    auto Sign = getIConstantVRegValWithLookThrough(SignReg, MRI);
    bool isSigned = Sign && Sign->Value.getZExtValue();

    if (IntrinsicID == Intrinsic::aie2p_pack_I512_I8_I16 ||
        IntrinsicID == Intrinsic::aie2p_pack_I512_I4_I8)
      return isSigned ? AIE2P::VPACK_mv_pack_w_packSign1
                      : AIE2P::VPACK_mv_pack_w_packSign0;
    else
      return isSigned ? AIE2P::VPACK_mv_pack_x_packSign1
                      : AIE2P::VPACK_mv_pack_x_packSign0;
  }
  // Unpack
  case Intrinsic::aie2p_unpack_I1024_I16_I8:
  case Intrinsic::aie2p_unpack_I1024_I8_I4:
  case Intrinsic::aie2p_unpack_I512_I16_I8:
  case Intrinsic::aie2p_unpack_I512_I8_I4: {
    Register SignReg = I.getOperand(3).getReg();
    auto Sign = getIConstantVRegValWithLookThrough(SignReg, MRI);
    bool isSigned = Sign && Sign->Value.getZExtValue();
    if (IntrinsicID == Intrinsic::aie2p_unpack_I512_I16_I8 ||
        IntrinsicID == Intrinsic::aie2p_unpack_I512_I8_I4)
      return isSigned ? AIE2P::VUNPACK_mv_unpack_w_unpackSign1
                      : AIE2P::VUNPACK_mv_unpack_w_unpackSign0;
    else
      return isSigned ? AIE2P::VUNPACK_mv_unpack_x_unpackSign1
                      : AIE2P::VUNPACK_mv_unpack_x_unpackSign0;
  }
  case Intrinsic::aie2p_vshuffle_576_bfp16:
    return AIE2P::VSHUFFLE_vec_shuffle_ex;
  case Intrinsic::aie2p_put_ms:
    return AIE2P::MOV_st_mMStream_tlast_reg;
  case Intrinsic::aie2p_put_ms_nb:
    return AIE2P::MOV_nb_st_mMStream_tlast_reg;
  // FIFO store intrinsics
  case Intrinsic::aie2p_fifo_st_push_512_bfp16:
    return AIE2P::VST_PUSH_512;
  case Intrinsic::aie2p_fifo_st_flush:
    return AIE2P::VST_FLUSH_512_normal_flush;
  case Intrinsic::aie2p_fifo_st_flush_conv:
    return AIE2P::VST_FLUSH_512_CONV_normal_flush;
  case Intrinsic::aie2p_fifo_st_flush_1d:
    return AIE2P::VST_FLUSH_512_fifo_1d_flush;
  case Intrinsic::aie2p_fifo_st_flush_1d_conv:
    return AIE2P::VST_FLUSH_512_CONV_fifo_1d_flush;
  case Intrinsic::aie2p_fifo_st_flush_2d:
    return AIE2P::VST_FLUSH_512_2D;
  case Intrinsic::aie2p_fifo_st_flush_2d_conv:
    return AIE2P::VST_FLUSH_512_CONV_2D;
  case Intrinsic::aie2p_fifo_st_flush_3d:
    return AIE2P::VST_FLUSH_512_3D;
  case Intrinsic::aie2p_fifo_st_flush_3d_conv:
    return AIE2P::VST_FLUSH_512_CONV_3D;
  default:
    llvm_unreachable("Unexpected Intrinsic ID");
  }
}

Register AIE2PInstrInfo::getVaddSignControlRegister() const {
  return AIE2P::vaddSign0;
}
Register AIE2PInstrInfo::getUPSModeControlRegister() const {
  return AIE2P::crUPSMode;
}
Register AIE2PInstrInfo::getUPSSignControlRegister() const {
  return AIE2P::upsSign0;
}

// Some AIE instructions like Load/Stores take compound register classes
// which can contain registers of different sizes. We need to use the right
// classes to avoid the MachineVerifier complaining about mismatching sizes.
// This was handled differently in AIE1, where _PTR and _GPR variants were
// introduced.
static const TargetRegisterClass *
constrainRegClass(MachineRegisterInfo &MRI, const TargetRegisterClass *RC,
                  unsigned Reg) {
  if (RC == nullptr || Register::isPhysicalRegister(Reg))
    return RC;

  if (auto *NewRC = MRI.constrainRegClass(Reg, &AIE2P::eP_as_32BitRegClass))
    return NewRC;
  if (auto *NewRC = MRI.constrainRegClass(Reg, &AIE2P::eM_as_32BitRegClass))
    return NewRC;
  if (auto *NewRC = MRI.constrainRegClass(Reg, &AIE2P::eDC_as_32BitRegClass))
    return NewRC;
  if (auto *NewRC = MRI.constrainRegClass(Reg, &AIE2P::eDJ_as_32BitRegClass))
    return NewRC;
  if (auto *NewRC = MRI.constrainRegClass(Reg, &AIE2P::eDN_as_32BitRegClass))
    return NewRC;
  return RC;
}

// Store a register to a stack slot.  Used in eliminating FrameIndex pseudo-ops.
void AIE2PInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
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
  if (regClassMatches(AIE2P::mSclStRegClass, RC, SrcReg)) {
    Opcode = AIE2P::ST_R_SPILL;
  } else if (regClassMatches(AIE2P::mQQssRegClass, RC, SrcReg)) {
    Opcode = AIE2P::ST_dmv_sts_q_spill;
  } else if (regClassMatches(AIE2P::mBMsRegClass, RC, SrcReg)) {
    Opcode = AIE2P::VST_dmx_sts_bm_spill;
  } else if (regClassMatches(AIE2P::mFifoHLRegRegClass, RC, SrcReg)) {
    Opcode = AIE2P::VST_dmx_sts_fifohl_spill;
  } else if (regClassMatches(AIE2P::VEC256RegClass, RC, SrcReg)) {
    Opcode = AIE2P::VST_dmw_sts_w_spill;
  } else if (regClassMatches(AIE2P::mXsRegClass, RC, SrcReg)) {
    Opcode = AIE2P::VST_dmx_sts_x_spill;
  } else if (regClassMatches(AIE2P::eLRegClass, RC, SrcReg)) {
    Opcode = AIE2P::VST_L_SPILL;
  } else if (regClassMatches(AIE2P::VEC1024RegClass, RC, SrcReg)) {
    Opcode = AIE2P::VST_Y_SPILL;
  } else if (regClassMatches(AIE2P::ACC1024RegClass, RC, SrcReg)) {
    Opcode = AIE2P::VST_CM_SPILL;
  } else if (regClassMatches(AIE2P::FIFO1024RegClass, RC, SrcReg)) {
    Opcode = AIE2P::VST_FIFO_SPILL;
  } else if (regClassMatches(AIE2P::ePSRFLdFRegClass, RC, SrcReg)) {
    Opcode = AIE2P::VST_PLFR_SPILL;
  } else if (regClassMatches(AIE2P::ACC2048RegClass, RC, SrcReg)) {
    Opcode = AIE2P::VST_DM_SPILL;
  } else if (regClassMatches(AIE2P::eDRegClass, RC, SrcReg)) {
    Opcode = AIE2P::ST_D_SPILL;
  } else if (regClassMatches(AIE2P::eDSRegClass, RC, SrcReg)) {
    Opcode = AIE2P::ST_DS_SPILL;
  } else if (regClassMatches(AIE2P::EXPVEC64RegClass, RC, SrcReg)) {
    Opcode = AIE2P::VST_E_SPILL;
  } else if (regClassMatches(AIE2P::VEC576RegClass, RC, SrcReg)) {
    Opcode = AIE2P::VST_EX_SPILL;
  } else if (regClassMatches(AIE2P::eSRegClass, RC, SrcReg) ||
             regClassMatches(AIE2P::spill_eS_to_eRRegClass, RC, SrcReg)) {
    // Can't spill these directly.  Need to bounce through a GPR.
    MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
    Register ScratchReg = MRI.createVirtualRegister(&AIE2P::eRRegClass);
    BuildMI(MBB, I, DL, get(TargetOpcode::COPY), ScratchReg)
        .addReg(SrcReg, getKillRegState(IsKill));
    Opcode = AIE2P::ST_R_SPILL;
    SrcReg = ScratchReg;
    IsKill = true;
  } else if (regClassMatches(AIE2P::spill_vec512_to_compositeRegClass, RC,
                             SrcReg)) {
    Opcode = AIE2P::VST_512_COMPOSED_REG_SPILL;
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

// Load a register to a stack slot.  Used in eliminating FrameIndex pseudo-ops.
void AIE2PInstrInfo::loadRegFromStackSlot(
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
  if (regClassMatches(AIE2P::mLdaSclRegClass, RC, DstReg)) {
    Opcode = AIE2P::LDA_R_SPILL;
  } else if (regClassMatches(AIE2P::mQQssRegClass, RC, DstReg)) {
    Opcode = AIE2P::LDA_dmv_lda_q_spill;
  } else if (regClassMatches(AIE2P::VEC256RegClass, RC, DstReg)) {
    Opcode = AIE2P::VLDA_dmw_lda_w_spill;
  } else if (regClassMatches(AIE2P::mBMsRegClass, RC, DstReg)) {
    Opcode = AIE2P::VLDA_dmx_lda_bm_spill;
  } else if (regClassMatches(AIE2P::mFifoHLRegRegClass, RC, DstReg)) {
    Opcode = AIE2P::VLDA_dmx_lda_fifohl_spill;
  } else if (regClassMatches(AIE2P::mXsRegClass, RC, DstReg)) {
    Opcode = AIE2P::VLDA_dmx_lda_x_spill;
  } else if (regClassMatches(AIE2P::ACC2048RegClass, RC, DstReg)) {
    Opcode = AIE2P::VLDA_DM_SPILL;
  } else if (regClassMatches(AIE2P::ACC1024RegClass, RC, DstReg)) {
    Opcode = AIE2P::VLDA_CM_SPILL;
  } else if (regClassMatches(AIE2P::FIFO1024RegClass, RC, DstReg)) {
    Opcode = AIE2P::VLDA_FIFO_SPILL;
  } else if (regClassMatches(AIE2P::ePSRFLdFRegClass, RC, DstReg)) {
    Opcode = AIE2P::VLDA_PLFR_SPILL;
  } else if (regClassMatches(AIE2P::VEC1024RegClass, RC, DstReg)) {
    Opcode = AIE2P::VLDA_Y_SPILL;
  } else if (regClassMatches(AIE2P::eLRegClass, RC, DstReg)) {
    Opcode = AIE2P::VLDA_L_SPILL;
  } else if (regClassMatches(AIE2P::eDRegClass, RC, DstReg)) {
    Opcode = AIE2P::LDA_D_SPILL;
  } else if (regClassMatches(AIE2P::eDSRegClass, RC, DstReg)) {
    Opcode = AIE2P::LDA_DS_SPILL;
  } else if (regClassMatches(AIE2P::EXPVEC64RegClass, RC, DstReg)) {
    Opcode = AIE2P::VLDA_E_SPILL;
  } else if (regClassMatches(AIE2P::VEC576RegClass, RC, DstReg)) {
    Opcode = AIE2P::VLDA_EX_SPILL;
  } else if (regClassMatches(AIE2P::eSRegClass, RC, DstReg) ||
             regClassMatches(AIE2P::spill_eS_to_eRRegClass, RC, DstReg)) {
    // Can't spill these directly.  Need to bounce through a GPR.
    MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
    Register Reg = MRI.createVirtualRegister(&AIE2P::eRRegClass);
    BuildMI(MBB, I, DL, get(AIE2P::LDA_R_SPILL), Reg)
        .addFrameIndex(FI)
        .addMemOperand(CreateMMO(FI));
    BuildMI(MBB, I, DL, get(TargetOpcode::COPY), DstReg)
        .addReg(Reg, getKillRegState(true));
    return;
  } else if (regClassMatches(AIE2P::spill_vec512_to_compositeRegClass, RC,
                             DstReg)) {
    Opcode = AIE2P::VLDA_512_COMPOSED_REG_SPILL;
  } else {
    LLVM_DEBUG(std::prev(I)->dump(); I->dump(); std::next(I)->dump();
               llvm::dbgs() << TRI->getRegClassName(RC));
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

SmallVector<AIEBaseInstrInfo::AIEPseudoExpandInfo, 4>
AIE2PInstrInfo::getSpillPseudoExpandInfo(const TargetRegisterInfo &TRI,
                                         MachineInstr &MI) const {
  if (!MI.isPseudo())
    return {};
  return getSpillPseudoExpandInfoByOpcode(MI.getOpcode());
}

SmallVector<AIEBaseInstrInfo::AIEPseudoExpandInfo, 4>
AIE2PInstrInfo::getSpillPseudoExpandInfoByOpcode(unsigned Opcode) const {
  switch (Opcode) {
  case AIE2P::ST_R_SPILL:
    return {{AIE2P::ST_dms_sts_spill}};
  case AIE2P::VST_L_SPILL:
    return {{AIE2P::ST_dms_sts_spill, AIE2P::sub_l_even},
            {AIE2P::ST_dms_sts_spill, AIE2P::sub_l_odd}};
  case AIE2P::VST_CM_SPILL:
    return {{AIE2P::VST_dmx_sts_bm_spill, AIE2P::sub_512_acc_lo},
            {AIE2P::VST_dmx_sts_bm_spill, AIE2P::sub_512_acc_hi}};
  case AIE2P::VST_FIFO_SPILL:
    return {{AIE2P::VST_dmx_sts_fifohl_spill, AIE2P::sub_lo_fifo},
            {AIE2P::VST_dmx_sts_fifohl_spill, AIE2P::sub_hi_fifo}};
  case AIE2P::VST_PLFR_SPILL:
    return {{AIE2P::VST_FIFO_SPILL, AIE2P::sub_fifo},
            {AIE2P::ST_dms_sts_spill, AIE2P::sub_avail},
            {AIE2P::ST_dms_sts_spill, AIE2P::sub_ptr}};

  case AIE2P::VST_DM_SPILL:
    return {{AIE2P::VST_CM_SPILL, AIE2P::sub_1024_acc_lo},
            {AIE2P::VST_CM_SPILL, AIE2P::sub_1024_acc_hi}};
  case AIE2P::VST_Y_SPILL:
    return {{AIE2P::VST_dmx_sts_x_spill, AIE2P::sub_512_lo},
            {AIE2P::VST_dmx_sts_x_spill, AIE2P::sub_512_hi}};
  case AIE2P::ST_D_SPILL:
    return {{AIE2P::ST_dms_sts_spill, AIE2P::sub_mod},
            {AIE2P::ST_dms_sts_spill, AIE2P::sub_dim_size},
            {AIE2P::ST_dms_sts_spill, AIE2P::sub_dim_stride},
            {AIE2P::ST_dms_sts_spill, AIE2P::sub_dim_count}};
  case AIE2P::ST_DS_SPILL:
    return {{AIE2P::ST_dms_sts_spill, AIE2P::sub_mod},
            {AIE2P::ST_dms_sts_spill, AIE2P::sub_dim_size},
            {AIE2P::ST_dms_sts_spill, AIE2P::sub_dim_stride},
            {AIE2P::ST_dms_sts_spill, AIE2P::sub_dim_count},
            {AIE2P::ST_dms_sts_spill, AIE2P::sub_hi_dim_then_sub_mod},
            {AIE2P::ST_dms_sts_spill, AIE2P::sub_hi_dim_then_sub_dim_size},
            {AIE2P::ST_dms_sts_spill, AIE2P::sub_hi_dim_then_sub_dim_stride},
            {AIE2P::ST_dms_sts_spill, AIE2P::sub_hi_dim_then_sub_dim_count}};

  case AIE2P::LDA_R_SPILL:
    return {{AIE2P::LDA_dms_lda_spill}};
  case AIE2P::VLDA_L_SPILL:
    return {{AIE2P::LDA_dms_lda_spill, AIE2P::sub_l_even},
            {AIE2P::LDA_dms_lda_spill, AIE2P::sub_l_odd}};
  case AIE2P::VLDA_CM_SPILL:
    return {{AIE2P::VLDA_dmx_lda_bm_spill, AIE2P::sub_512_acc_lo},
            {AIE2P::VLDA_dmx_lda_bm_spill, AIE2P::sub_512_acc_hi}};
  case AIE2P::VLDA_FIFO_SPILL:
    return {{AIE2P::VLDA_dmx_lda_fifohl_spill, AIE2P::sub_lo_fifo},
            {AIE2P::VLDA_dmx_lda_fifohl_spill, AIE2P::sub_hi_fifo}};
  case AIE2P::VLDA_PLFR_SPILL:
    return {
        {AIE2P::VLDA_FIFO_SPILL, AIE2P::sub_fifo},
        {AIE2P::LDA_dms_lda_spill, AIE2P::sub_avail},
        {AIE2P::LDA_dms_lda_spill, AIE2P::sub_ptr},
    };
  case AIE2P::VLDA_DM_SPILL:
    return {{AIE2P::VLDA_CM_SPILL, AIE2P::sub_1024_acc_lo},
            {AIE2P::VLDA_CM_SPILL, AIE2P::sub_1024_acc_hi}};
  case AIE2P::VLDA_Y_SPILL:
    return {{AIE2P::VLDA_dmx_lda_x_spill, AIE2P::sub_512_lo},
            {AIE2P::VLDA_dmx_lda_x_spill, AIE2P::sub_512_hi}};
  case AIE2P::LDA_D_SPILL:
    return {{AIE2P::LDA_dms_lda_spill, AIE2P::sub_mod},
            {AIE2P::LDA_dms_lda_spill, AIE2P::sub_dim_size},
            {AIE2P::LDA_dms_lda_spill, AIE2P::sub_dim_stride},
            {AIE2P::LDA_dms_lda_spill, AIE2P::sub_dim_count}};
  case AIE2P::LDA_DS_SPILL:
    return {{AIE2P::LDA_dms_lda_spill, AIE2P::sub_mod},
            {AIE2P::LDA_dms_lda_spill, AIE2P::sub_dim_size},
            {AIE2P::LDA_dms_lda_spill, AIE2P::sub_dim_stride},
            {AIE2P::LDA_dms_lda_spill, AIE2P::sub_dim_count},
            {AIE2P::LDA_dms_lda_spill, AIE2P::sub_hi_dim_then_sub_mod},
            {AIE2P::LDA_dms_lda_spill, AIE2P::sub_hi_dim_then_sub_dim_size},
            {AIE2P::LDA_dms_lda_spill, AIE2P::sub_hi_dim_then_sub_dim_stride},
            {AIE2P::LDA_dms_lda_spill, AIE2P::sub_hi_dim_then_sub_dim_count}};
  case AIE2P::VLDA_E_SPILL:
    return {{AIE2P::LDA_dms_lda_spill, AIE2P::sub_lo_exp},
            {AIE2P::LDA_dms_lda_spill, AIE2P::sub_hi_exp}};
  case AIE2P::VLDA_EX_SPILL:
    return {{AIE2P::VLDA_dmx_lda_x_spill, AIE2P::sub_bfp16_x},
            {AIE2P::VLDA_E_SPILL, AIE2P::sub_bfp16_e}};
  case AIE2P::VST_E_SPILL:
    return {{AIE2P::ST_dms_sts_spill, AIE2P::sub_lo_exp},
            {AIE2P::ST_dms_sts_spill, AIE2P::sub_hi_exp}};
  case AIE2P::VST_EX_SPILL:
    return {{AIE2P::VST_dmx_sts_x_spill, AIE2P::sub_bfp16_x},
            {AIE2P::VST_E_SPILL, AIE2P::sub_bfp16_e}};
  case AIE2P::VLDA_512_COMPOSED_REG_SPILL:
  case AIE2P::VST_512_COMPOSED_REG_SPILL:
    // No expansion needed - this is handled in expandPostRAPseudo()
    // where the pseudo is directly replaced with native 512-bit
    // load/store instructions.
    return {};
  default:
    return {};
  }
}

AIEBaseInstrInfo::AIERegOffsetSpillInstrInfo
AIE2PInstrInfo::getRegOffsetSpillInstrInfoFromImmOffset(
    const unsigned Opcode) const {
  switch (Opcode) {
  case AIE2P::ST_dms_sts_spill:
    return {AIE2P::ST_dms_sts_idx, AIE2P::MOVXM, &AIE2P::eDJRegClass};
  case AIE2P::LDA_dms_lda_spill:
    return {AIE2P::LDA_dms_lda_idx, AIE2P::MOVXM, &AIE2P::eDJRegClass};
  case AIE2P::VST_dmx_sts_fifohl_spill:
    return {AIE2P::VST_dmx_sts_fifohl_idx, AIE2P::MOVXM, &AIE2P::eDJRegClass};
  case AIE2P::VLDA_dmx_lda_fifohl_spill:
    return {AIE2P::VLDA_dmx_lda_fifohl_idx, AIE2P::MOVXM, &AIE2P::eDJRegClass};
  case AIE2P::VLDA_dmx_lda_x_spill:
    return {AIE2P::VLDA_dmx_lda_x_idx, AIE2P::MOVXM, &AIE2P::eDJRegClass};
  case AIE2P::VST_dmx_sts_x_spill:
    return {AIE2P::VST_dmx_sts_x_idx, AIE2P::MOVXM, &AIE2P::eDJRegClass};
  default:
    llvm_unreachable("Offset register spill instruction info un-implemented");
  }
}

std::optional<unsigned>
AIE2PInstrInfo::getConstantMovOpcode(MachineRegisterInfo &MRI, unsigned int Reg,
                                     APInt &Val) const {
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
    if (regClassMatches(AIE2P::mAluCgRegClass, DstRegClass, Reg))
      return AIE2P::MOV_RLC_imm11_pseudo;
    if (regClassMatches(AIE2P::mAguDstRegClass, DstRegClass, Reg) ||
        regClassMatches(AIE2P::ePRegClass, DstRegClass, Reg) ||
        regClassMatches(AIE2P::mDmRegClass, DstRegClass, Reg) ||
        regClassMatches(AIE2P::eRRegClass, DstRegClass, Reg))
      return AIE2P::MOV_PD_imm11_pseudo;
    if (regClassMatches(AIE2P::eSRegClass, DstRegClass, Reg))
      return AIE2P::MOV_S_imm11_pseudo;
    if (regClassMatches(AIE2P::mMvSclDstRegClass, DstRegClass, Reg))
      return AIE2P::MOV_scalar_imm11_pseudo;
  }
  if (ImmSize <= 32)
    return AIE2P::MOVXM;

  return std::nullopt;
}

unsigned AIE2PInstrInfo::getCycleSeparatorOpcode() const {
  return AIE2P::CYCLE_SEPARATOR;
}

// Note: Some pseudos like spill/reload are already expanded in
// eliminateFrameIndex.
bool AIE2PInstrInfo::expandPostRAPseudo(MachineInstr &MI) const {
  auto DL = MI.getDebugLoc();
  MachineBasicBlock &MBB = *MI.getParent();
  switch (MI.getOpcode()) {
  case AIE2P::PseudoMove: {
    const Register Dst = MI.getOperand(0).getReg();
    const Register Src = MI.getOperand(1).getReg();
    const bool IsKill = MI.getOperand(1).isKill();
    copyPhysReg(MBB, MI, DL, Dst, Src, IsKill);
    MI.eraseFromParent();
    return true;
  }
  case AIE2P::VLDA_512_COMPOSED_REG_SPILL: {
    unsigned int Opcode;
    const Register Dst = MI.getOperand(0).getReg();
    if (AIE2P::VEC512RegClass.contains(Dst)) {
      Opcode = AIE2P::VLDA_dmx_lda_x_spill;
    } else if (AIE2P::FIFO512RegClass.contains(Dst)) {
      Opcode = AIE2P::VLDA_dmx_lda_fifohl_spill;
    } else if (AIE2P::ACC512RegClass.contains(Dst)) {
      Opcode = AIE2P::VLDA_dmx_lda_bm_spill;
    } else {
      llvm_unreachable("Not a valid register for VLDA_512_COMPOSED_REG_SPILL");
    }
    MI.setDesc(get(Opcode));
    return false;
  }
  case AIE2P::VST_512_COMPOSED_REG_SPILL: {
    unsigned int Opcode;
    const Register Src = MI.getOperand(0).getReg();
    if (AIE2P::VEC512RegClass.contains(Src)) {
      Opcode = AIE2P::VST_dmx_sts_x_spill;
    } else if (AIE2P::FIFO512RegClass.contains(Src)) {
      Opcode = AIE2P::VST_dmx_sts_fifohl_spill;
    } else if (AIE2P::ACC512RegClass.contains(Src)) {
      Opcode = AIE2P::VST_dmx_sts_bm_spill;
    } else {
      llvm_unreachable("Not a valid register for VST_512_COMPOSED_REG_SPILL");
    }
    MI.setDesc(get(Opcode));
    return false;
  }
  }
  return false;
}

ScheduleHazardRecognizer *AIE2PInstrInfo::CreateTargetPostRAHazardRecognizer(
    const InstrItineraryData *II, const ScheduleDAG *DAG) const {
  llvm_unreachable("AIE2P is not meant to use the post-RA list scheduler. "
                   "Please use the MI scheduler instead: postmisched");
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

ScheduleHazardRecognizer *
AIE2PInstrInfo::CreateTargetMIHazardRecognizer(const InstrItineraryData *II,
                                               const ScheduleDAGMI *DAG) const {
  // AIE has a fully exposed pipeline, resource and format conflicts must be
  // exactly modelled.
  return new AIEHazardRecognizer(this, II, getSelectedAltDescs(DAG),
                                 /*IsPreRA=*/DAG->hasVRegLiveness());
}

// Check whether Opc is a lock operation
// This is used to adjust the latency of barrier edges into them
bool AIE2PInstrInfo::isLock(unsigned Opc) const {
  switch (Opc) {
  default:
    break;
  case AIE2P::ACQ_mLockId_imm:
  case AIE2P::ACQ_mLockId_reg:
  case AIE2P::ACQ_COND_mLockId_imm:
  case AIE2P::ACQ_COND_mLockId_reg:
  case AIE2P::REL_mLockId_imm:
  case AIE2P::REL_mLockId_reg:
  case AIE2P::REL_COND_mLockId_imm:
  case AIE2P::REL_COND_mLockId_reg:
    return true;
  }
  return false;
}

// Return an optional latency if Opc is DONE.
std::optional<unsigned>
AIE2PInstrInfo::getDoneLatency(const unsigned Opc) const {
  // AIE2P ISA isn't very clear on the DONE instruction and only mentions a
  // structural conflict at E4..E6. So, conservatively, provide a latency of 6.
  return (Opc == AIE2P::DONE) ? std::optional<unsigned>(6) : std::nullopt;
}

bool AIE2PInstrInfo::isDelayedSchedBarrier(const MachineInstr &MI) const {
  return MI.getOpcode() == AIE2P::DelayedSchedBarrier;
}

bool AIE2PInstrInfo::isSchedBarrier(const MachineInstr &MI) const {
  return (MI.getOpcode() == AIE2P::SCHED_BARRIER ||
          MI.getOpcode() == AIE2P::PseudoLoopEnd ||
          MI.getOpcode() == AIE2P::MOV_alu_mv_mv_mv_cntr2l ||
          isDelayedSchedBarrier(MI));
}

unsigned
AIE2PInstrInfo::getNumReservedDelaySlots(const MachineInstr &MI) const {
  return 0;
}

SmallVector<TiedRegOperands, 4>
AIE2PInstrInfo::getTiedRegInfo(unsigned Opcode) const {
  const SmallVector<SubRegSplit, 8> Split2DReg = {
      SubRegSplit(AIE2P::sub_mod), SubRegSplit(AIE2P::sub_dim_size),
      SubRegSplit(AIE2P::sub_dim_stride), SubRegSplit(AIE2P::sub_dim_count)};
  const SmallVector<SubRegSplit, 8> Split3DReg = {
      SubRegSplit(AIE2P::sub_mod),
      SubRegSplit(AIE2P::sub_dim_size),
      SubRegSplit(AIE2P::sub_dim_stride),
      SubRegSplit(AIE2P::sub_dim_count),
      SubRegSplit(AIE2P::sub_hi_dim_then_sub_mod, /*IsUndef=*/true),
      SubRegSplit(AIE2P::sub_hi_dim_then_sub_dim_size),
      SubRegSplit(AIE2P::sub_hi_dim_then_sub_dim_stride),
      SubRegSplit(AIE2P::sub_hi_dim_then_sub_dim_count)};
  const SmallVector<SubRegSplit, 8> SplitPLFRReg = {
      SubRegSplit(AIE2P::sub_ptr), SubRegSplit(AIE2P::sub_fifo),
      SubRegSplit(AIE2P::sub_avail)};
  // TODO: This should be generated
  switch (Opcode) {
  case AIE2P::LDA_2D_dms_lda:
  case AIE2P::LDA_2D_dmv_lda_q:
  case AIE2P::LDA_2D_s16:
  case AIE2P::LDA_2D_s8:
  case AIE2P::LDA_2D_u16:
  case AIE2P::LDA_2D_u8:
  case AIE2P::VLDA_2D_dmw_lda_w:
  case AIE2P::VLDA_2D_128:
  case AIE2P::VLDA_2D_CONV_fp32_bf16_dmw_lda_ups_bf:
  case AIE2P::VLDA_2D_CONV_fp32_bf16_dmx_lda_ups_bf:
  case AIE2P::VLDA_2D_dmx_lda_bm:
  case AIE2P::VLDA_2D_dmx_lda_fifohl:
  case AIE2P::VLDA_2D_dmx_lda_x:
  case AIE2P::VLDB_2D_128:
  case AIE2P::VLD_2D_128_pseudo:
  case AIE2P::VLDB_2D_UNPACK_dmw_ldb_unpack_unpackSign0:
  case AIE2P::VLDB_2D_UNPACK_dmw_ldb_unpack_unpackSign1:
  case AIE2P::VLDB_2D_UNPACK_dmx_ldb_unpack_unpackSign0:
  case AIE2P::VLDB_2D_UNPACK_dmx_ldb_unpack_unpackSign1:
  case AIE2P::VLDB_2D_dmw_ldb:
  case AIE2P::VLDB_2D_dmx_ldb_x:
  case AIE2P::VLD_2D_w_pseudo:
  case AIE2P::VLD_2D_x_pseudo:
  case AIE2P::LDA_TM_2D:
    // Constraints = "$count_out=$mod.sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/2, /*SubRegIdx=*/AIE2P::sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/4, /*SubRegIdx=*/AIE2P::NoSubRegister,
                    /*SubRegsSplit=*/Split2DReg}}}};
  case AIE2P::VST_FLUSH_512_2D:
  case AIE2P::VST_FLUSH_512_CONV_2D:
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/3, /*SubRegIdx=*/AIE2P::sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/7, /*SubRegIdx=*/AIE2P::NoSubRegister,
                    /*SubRegsSplit=*/Split2DReg}}}};
  case AIE2P::LDA_3D_dms_lda:
  case AIE2P::LDA_3D_dmv_lda_q:
  case AIE2P::LDA_3D_s16:
  case AIE2P::LDA_3D_s8:
  case AIE2P::LDA_3D_u16:
  case AIE2P::LDA_3D_u8:
  case AIE2P::VLDA_3D_dmw_lda_w:
  case AIE2P::VLDA_3D_128:
  case AIE2P::VLD_3D_128_pseudo:
  case AIE2P::VLDA_3D_CONV_fp32_bf16_dmw_lda_ups_bf:
  case AIE2P::VLDA_3D_CONV_fp32_bf16_dmx_lda_ups_bf:
  case AIE2P::VLDB_3D_128:
  case AIE2P::VLDA_3D_dmx_lda_bm:
  case AIE2P::VLDA_3D_dmx_lda_fifohl:
  case AIE2P::VLDA_3D_dmx_lda_x:
  case AIE2P::VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign0:
  case AIE2P::VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign1:
  case AIE2P::VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign0:
  case AIE2P::VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign1:
  case AIE2P::VLDB_3D_dmw_ldb:
  case AIE2P::VLDB_3D_dmx_ldb_x:
  case AIE2P::VLD_3D_w_pseudo:
  case AIE2P::VLD_3D_x_pseudo:
  case AIE2P::LDA_TM_3D:
    // Constraints = "$count_lo_out=$mod.sub_dim_count,
    //                $count_hi_out=$mod.sub_hi_dim_then_sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2P::sub_dim_count},
            {/*OpIdx=*/3, /*SubRegIdx=*/AIE2P::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/5, /*SubRegIdx=*/AIE2P::NoSubRegister,
                    /*SubRegsSplit=*/Split3DReg}}}};
  case AIE2P::VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign0:
  case AIE2P::VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign1:
  case AIE2P::VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign0:
  case AIE2P::VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign1:
  case AIE2P::VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign0:
  case AIE2P::VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign1:
  case AIE2P::VLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsSign0:
  case AIE2P::VLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsSign1:
    // Constraints = "$count_out=$mod.sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/2, /*SubRegIdx=*/AIE2P::sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/5, /*SubRegIdx=*/AIE2P::NoSubRegister,
                    /*SubRegsSplit=*/Split2DReg}}}};
  case AIE2P::VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign0:
  case AIE2P::VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign1:
  case AIE2P::VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign0:
  case AIE2P::VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign1:
  case AIE2P::VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign0:
  case AIE2P::VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign1:
  case AIE2P::VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign0:
  case AIE2P::VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign1:
    // Constraints = "$count_lo_out=$mod.sub_dim_count,
    //                $count_hi_out=$mod.sub_hi_dim_then_sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2P::sub_dim_count},
            {/*OpIdx=*/3, /*SubRegIdx=*/AIE2P::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/6, /*SubRegIdx=*/AIE2P::NoSubRegister,
                    /*SubRegsSplit=*/Split3DReg}}}};
  case AIE2P::VST_FLUSH_512_3D:
  case AIE2P::VST_FLUSH_512_CONV_3D:
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/3, /*SubRegIdx=*/AIE2P::sub_dim_count},
            {/*OpIdx=*/4, /*SubRegIdx=*/AIE2P::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/8, /*SubRegIdx=*/AIE2P::NoSubRegister,
                    /*SubRegsSplit=*/Split3DReg}}}};
  case AIE2P::ST_2D_dms_sts:
  case AIE2P::ST_2D_dmv_sts_q:
  case AIE2P::ST_2D_s16:
  case AIE2P::ST_2D_s8:
  case AIE2P::VST_2D_128:
  case AIE2P::VST_2D_dmw_sts_w:
  case AIE2P::VST_2D_dmx_sts_x:
  case AIE2P::VST_2D_dmx_sts_fifohl:
  case AIE2P::VST_2D_dmx_sts_bm:
  case AIE2P::ST_TM_2D:
    // Constraints = "$count_out=$mod.sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2P::sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/4, /*SubRegIdx=*/AIE2P::NoSubRegister,
                    /*SubRegsSplit=*/Split2DReg}}}};
  case AIE2P::VST_2D_PACK_dmw_sts_pack_packSign0:
  case AIE2P::VST_2D_PACK_dmw_sts_pack_packSign1:
  case AIE2P::VST_2D_PACK_dmx_sts_pack_packSign0:
  case AIE2P::VST_2D_PACK_dmx_sts_pack_packSign1:
  case AIE2P::VST_2D_CONV_bf16_fp32_dmw_sts_srs_bf:
  case AIE2P::VST_2D_CONV_bf16_fp32_dmx_sts_srs_bf:
    // Constraints = "$count_out=$mod.sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2P::sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/4, /*SubRegIdx=*/AIE2P::NoSubRegister,
                    /*SubRegsSplit=*/Split2DReg}}}};
  case AIE2P::ST_3D_dms_sts:
  case AIE2P::ST_3D_dmv_sts_q:
  case AIE2P::ST_3D_s16:
  case AIE2P::ST_3D_s8:
  case AIE2P::VST_3D_128:
  case AIE2P::VST_3D_dmw_sts_w:
  case AIE2P::VST_3D_dmx_sts_x:
  case AIE2P::VST_3D_dmx_sts_fifohl:
  case AIE2P::VST_3D_dmx_sts_bm:
  case AIE2P::ST_TM_3D:
    // Constraints = "$count_lo_out=$mod.sub_dim_count,
    //                $count_hi_out=$mod.sub_hi_dim_then_sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/1, /*SubRegIdx=*/AIE2P::sub_dim_count},
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2P::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/5, /*SubRegIdx=*/AIE2P::NoSubRegister,
                    /*SubRegsSplit=*/Split3DReg}}}};
  case AIE2P::VST_3D_PACK_dmw_sts_pack_packSign0:
  case AIE2P::VST_3D_PACK_dmw_sts_pack_packSign1:
  case AIE2P::VST_3D_PACK_dmx_sts_pack_packSign0:
  case AIE2P::VST_3D_PACK_dmx_sts_pack_packSign1:
  case AIE2P::VST_3D_CONV_bf16_fp32_dmw_sts_srs_bf:
  case AIE2P::VST_3D_CONV_bf16_fp32_dmx_sts_srs_bf:
    // Constraints = "$count_lo_out=$mod.sub_dim_count,
    //                $count_hi_out=$mod.sub_hi_dim_then_sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/1, /*SubRegIdx=*/AIE2P::sub_dim_count},
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2P::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/5, /*SubRegIdx=*/AIE2P::NoSubRegister,
                    /*SubRegsSplit=*/Split3DReg}}}};
  case AIE2P::VST_2D_SRS_2x_dm_sts_srs_cm_srsSign0:
  case AIE2P::VST_2D_SRS_2x_dm_sts_srs_cm_srsSign1:
  case AIE2P::VST_2D_SRS_4x_dm_sts_srs_cm_srsSign0:
  case AIE2P::VST_2D_SRS_4x_dm_sts_srs_cm_srsSign1:
  case AIE2P::VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign0:
  case AIE2P::VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign1:
  case AIE2P::VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign0:
  case AIE2P::VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign1:
    // Constraints = "$count_out=$mod.sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2P::sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/5, /*SubRegIdx=*/AIE2P::NoSubRegister,
                    /*SubRegsSplit=*/Split2DReg}}}};
  case AIE2P::VST_3D_SRS_2x_dm_sts_srs_cm_srsSign0:
  case AIE2P::VST_3D_SRS_2x_dm_sts_srs_cm_srsSign1:
  case AIE2P::VST_3D_SRS_4x_dm_sts_srs_cm_srsSign0:
  case AIE2P::VST_3D_SRS_4x_dm_sts_srs_cm_srsSign1:
  case AIE2P::VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign0:
  case AIE2P::VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign1:
  case AIE2P::VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign0:
  case AIE2P::VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign1:
    // Constraints = "$count_lo_out=$mod.sub_dim_count,
    //                $count_hi_out=$mod.sub_hi_dim_then_sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/1, /*SubRegIdx=*/AIE2P::sub_dim_count},
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2P::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{{/*OpIdx=*/6, /*SubRegIdx=*/AIE2P::NoSubRegister,
                    /*SubRegsSplit=*/Split3DReg}}}};
  case AIE2P::PADDA_2D:
  case AIE2P::PADDB_2D:
  case AIE2P::PADDS_2D:
  case AIE2P::PADD_2D_pseudo:
    // Constraints = "$count_out=$mod.sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2P::sub_dim_count}},
        /*SrcOp=*/
        {{/*OpIdx=*/3, /*SubRegIdx=*/AIE2P::NoSubRegister,
          /*SubRegsSplit=*/Split2DReg}}}};
  case AIE2P::PADDA_3D:
  case AIE2P::PADDB_3D:
  case AIE2P::PADDS_3D:
  case AIE2P::PADD_3D_pseudo:
    // Constraints = "$count_lo_out=$mod.sub_dim_count,
    //                $count_hi_out=$mod.sub_hi_dim_then_sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/1, /*SubRegIdx=*/AIE2P::sub_dim_count},
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2P::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/
        {{/*OpIdx=*/4, /*SubRegIdx=*/AIE2P::NoSubRegister,
          /*SubRegsSplit=*/Split3DReg}}}};
  case AIE2P::VLDB_FILLX_512:
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/0, /*SubRegIdx=*/AIE2P::sub_ptr},
                    {/*OpIdx=*/1, /*SubRegIdx=*/AIE2P::sub_fifo},
                    {/*OpIdx=*/2, /*SubRegIdx=*/AIE2P::sub_avail}},
        /*SrcOp=*/
        {{/*OpIdx=*/5, /*SubRegIdx=*/AIE2P::sub_ptr},
         {/*OpIdx=*/6, /*SubRegIdx=*/AIE2P::sub_fifo},
         {/*OpIdx=*/7, /*SubRegIdx=*/AIE2P::sub_avail}},
        /*NewSuperClass=*/&AIE2P::ePSRFLdFRegClass}};
  case AIE2P::VLDA_FILL_512:
  case AIE2P::VLDB_FILL_512:
  case AIE2P::VLD_FILL_512_pseudo:
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/0, /*SubRegIdx=*/AIE2P::sub_ptr},
                    {/*OpIdx=*/1, /*SubRegIdx=*/AIE2P::sub_fifo},
                    {/*OpIdx=*/2, /*SubRegIdx=*/AIE2P::sub_avail}},
        /*SrcOps=*/
        {{/*OpIdx=*/3, /*SubRegIdx=*/AIE2P::sub_ptr},
         {/*OpIdx=*/4, /*SubRegIdx=*/AIE2P::sub_fifo},
         {/*OpIdx=*/5, /*SubRegIdx=*/AIE2P::sub_avail}},
        /*NewSuperClass=*/&AIE2P::ePSRFLdFRegClass}};

  case AIE2P::VLDB_POPX_512:
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2P::sub_ptr},
                    {/*OpIdx=*/2, /*SubRegIdx=*/AIE2P::sub_fifo},
                    {/*OpIdx=*/3, /*SubRegIdx=*/AIE2P::sub_avail}},
        /*SrcOp=*/
        {{/*OpIdx=*/6, /*SubRegIdx=*/AIE2P::sub_ptr},
         {/*OpIdx=*/7, /*SubRegIdx=*/AIE2P::sub_fifo},
         {/*OpIdx=*/8, /*SubRegIdx=*/AIE2P::sub_avail}},
        /*NewSuperClass=*/&AIE2P::ePSRFLdFRegClass}};
  case AIE2P::VLDA_POP_512_normal_pop:
  case AIE2P::VLDA_POP_544_normal_pop:
  case AIE2P::VLDA_POP_576_normal_pop:
  case AIE2P::VLDA_POP_640_normal_pop:
  case AIE2P::VLDA_POP_704_normal_pop:
  case AIE2P::VLDB_POP_512_normal_pop:
  case AIE2P::VLDB_POP_544_normal_pop:
  case AIE2P::VLDB_POP_576_normal_pop:
  case AIE2P::VLDB_POP_640_normal_pop:
  case AIE2P::VLDB_POP_704_normal_pop:
  case AIE2P::VLD_POP_512_normal_pop_pseudo:
  case AIE2P::VLD_POP_544_normal_pop_pseudo:
  case AIE2P::VLD_POP_576_normal_pop_pseudo:
  case AIE2P::VLD_POP_640_normal_pop_pseudo:
  case AIE2P::VLD_POP_704_normal_pop_pseudo:
  case AIE2P::VLDA_POP_512_fifo_1d_pop:
  case AIE2P::VLDA_POP_544_fifo_1d_pop:
  case AIE2P::VLDA_POP_576_fifo_1d_pop:
  case AIE2P::VLDA_POP_640_fifo_1d_pop:
  case AIE2P::VLDA_POP_704_fifo_1d_pop:
  case AIE2P::VLDB_POP_512_fifo_1d_pop:
  case AIE2P::VLDB_POP_544_fifo_1d_pop:
  case AIE2P::VLDB_POP_576_fifo_1d_pop:
  case AIE2P::VLDB_POP_640_fifo_1d_pop:
  case AIE2P::VLDB_POP_704_fifo_1d_pop:
  case AIE2P::VLD_POP_512_fifo_1d_pop_pseudo:
  case AIE2P::VLD_POP_544_fifo_1d_pop_pseudo:
  case AIE2P::VLD_POP_576_fifo_1d_pop_pseudo:
  case AIE2P::VLD_POP_640_fifo_1d_pop_pseudo:
  case AIE2P::VLD_POP_704_fifo_1d_pop_pseudo:
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2P::sub_ptr},
                    {/*OpIdx=*/2, /*SubRegIdx=*/AIE2P::sub_fifo},
                    {/*OpIdx=*/3, /*SubRegIdx=*/AIE2P::sub_avail}},
        /*SrcOps=*/
        {{/*OpIdx=*/4, /*SubRegIdx=*/AIE2P::sub_ptr},
         {/*OpIdx=*/5, /*SubRegIdx=*/AIE2P::sub_fifo},
         {/*OpIdx=*/6, /*SubRegIdx=*/AIE2P::sub_avail}},
        /*NewSuperClass=*/&AIE2P::ePSRFLdFRegClass}};
  case AIE2P::VLDA_POP_512_2D:
  case AIE2P::VLDA_POP_544_2D:
  case AIE2P::VLDA_POP_576_2D:
  case AIE2P::VLDA_POP_640_2D:
  case AIE2P::VLDA_POP_704_2D:
  case AIE2P::VLDB_POP_512_2D:
  case AIE2P::VLDB_POP_544_2D:
  case AIE2P::VLDB_POP_576_2D:
  case AIE2P::VLDB_POP_640_2D:
  case AIE2P::VLDB_POP_704_2D:
  case AIE2P::VLD_POP_512_2D_pseudo:
  case AIE2P::VLD_POP_544_2D_pseudo:
  case AIE2P::VLD_POP_576_2D_pseudo:
  case AIE2P::VLD_POP_640_2D_pseudo:
  case AIE2P::VLD_POP_704_2D_pseudo:
    return {TiedRegOperands{
                /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2P::sub_ptr},
                            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2P::sub_fifo},
                            {/*OpIdx=*/3, /*SubRegIdx=*/AIE2P::sub_avail}},
                /*SrcOps=*/
                {{/*OpIdx=*/5, /*SubRegIdx=*/AIE2P::sub_ptr},
                 {/*OpIdx=*/6, /*SubRegIdx=*/AIE2P::sub_fifo},
                 {/*OpIdx=*/7, /*SubRegIdx=*/AIE2P::sub_avail}},
                /*NewSuperClass=*/&AIE2P::ePSRFLdFRegClass},
            TiedRegOperands{
                /*DstOps=*/{{/*OpIdx=*/4, /*SubRegIdx=*/AIE2P::sub_dim_count}},
                /*SrcOp=*/
                {{/*OpIdx=*/8, /*SubRegIdx=*/AIE2P::NoSubRegister,
                  /*SubRegsSplit=*/Split2DReg}}}};
  case AIE2P::VLDA_POP_512_3D:
  case AIE2P::VLDA_POP_544_3D:
  case AIE2P::VLDA_POP_576_3D:
  case AIE2P::VLDA_POP_640_3D:
  case AIE2P::VLDA_POP_704_3D:
  case AIE2P::VLDB_POP_512_3D:
  case AIE2P::VLDB_POP_544_3D:
  case AIE2P::VLDB_POP_576_3D:
  case AIE2P::VLDB_POP_640_3D:
  case AIE2P::VLDB_POP_704_3D:
  case AIE2P::VLD_POP_512_3D_pseudo:
  case AIE2P::VLD_POP_544_3D_pseudo:
  case AIE2P::VLD_POP_576_3D_pseudo:
  case AIE2P::VLD_POP_640_3D_pseudo:
  case AIE2P::VLD_POP_704_3D_pseudo:
    return {
        TiedRegOperands{
            /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2P::sub_ptr},
                        {/*OpIdx=*/2, /*SubRegIdx=*/AIE2P::sub_fifo},
                        {/*OpIdx=*/3, /*SubRegIdx=*/AIE2P::sub_avail}},
            /*SrcOps=*/
            {{/*OpIdx=*/6, /*SubRegIdx=*/AIE2P::sub_ptr},
             {/*OpIdx=*/7, /*SubRegIdx=*/AIE2P::sub_fifo},
             {/*OpIdx=*/8, /*SubRegIdx=*/AIE2P::sub_avail}},
            /*NewSuperClass=*/&AIE2P::ePSRFLdFRegClass},
        TiedRegOperands{
            /*DstOps=*/{{/*OpIdx=*/4, /*SubRegIdx=*/AIE2P::sub_dim_count},
                        {/*OpIdx=*/5,
                         /*SubRegIdx=*/AIE2P::sub_hi_dim_then_sub_dim_count}},
            /*SrcOp=*/
            {{/*OpIdx=*/9, /*SubRegIdx=*/AIE2P::NoSubRegister,
              /*SubRegsSplit=*/Split3DReg}}}};
  default:
    return {};
  }
}

TiedRegOperands
AIE2PInstrInfo::getTiedRegInfoForSplitting(unsigned Opcode) const {
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

std::pair<unsigned, unsigned>
AIE2PInstrInfo::decomposeMachineOperandsTargetFlags(unsigned TF) const {
  const unsigned Mask = AIEII::MO_DIRECT_FLAG_MASK;
  return std::make_pair(TF & Mask, TF & ~Mask);
}

ArrayRef<std::pair<unsigned, const char *>>
AIE2PInstrInfo::getSerializableDirectMachineOperandTargetFlags() const {
  using namespace AIEII;
  static const std::pair<unsigned, const char *> TargetFlags[] = {
      {MO_GLOBAL, "aie2p-global"}};
  return ArrayRef(TargetFlags);
}

bool AIE2PInstrInfo::canHoistCheapInst(const MachineInstr &MI) const {
  if (!AIEBaseInstrInfo::canHoistCheapInst(MI))
    return false;
  return false;
}

std::optional<AIEBaseInstrInfo::ZOLSupport>
AIE2PInstrInfo::getZOLSupport() const {
  AIEBaseInstrInfo::ZOLSupport Result;

  Result.LoopStartOpcode = AIE2P::LoopStart;
  Result.LoopEndOpcode = AIE2P::PseudoLoopEnd;
  Result.SetLoopCountOpcode = AIE2P::ADD_NC_mv_add_ri;
  Result.SetLoopStartOpcode = AIE2P::MOVXM;
  Result.SetLoopEndOpcode = AIE2P::MOVXM;
  // We need at 112 bytes distance from the loop setup to the loop end label,
  // which requires 7 bundles of 16 bytes.
  Result.LoopSetupDistance = 7;
  Result.LCRegister = AIE2P::lc;
  Result.LSRegister = AIE2P::ls;
  Result.LERegister = AIE2P::le;

  return Result;
}

std::optional<unsigned> AIE2PInstrInfo::getLoopVersionThresholdOpcode() const {
  return AIE2P::PseudoLoopVersionThreshold;
}

std::optional<AIEBaseInstrInfo::JNZDSupport>
AIE2PInstrInfo::getJNZDSupport() const {
  AIEBaseInstrInfo::JNZDSupport Result;

  Result.MovBlockAddrOpcode = AIE2P::MOVXM;
  Result.PointerRegisterClass = &AIE2P::eP_as_32BitRegClass;
  Result.LoopDecOpcode = AIE2P::LoopDec;
  Result.LoopJNZOpcode = AIE2P::LoopJNZ;
  Result.DecTripCountOpcode = AIE2P::ADD_NC_mv_add_ri;
  Result.LoopJNZDOpcode = AIE2P::PseudoJNZD;
  return Result;
}

bool AIE2PInstrInfo::isOffsetInImmediateRange(
    unsigned Opcode, unsigned LoadStoreSize,
    std::optional<APInt> Offset) const {
  if (!Offset)
    return false;

  switch (Opcode) {
  case AIE2P::G_AIE_OFFSET_STORE:
  case AIE2P::G_AIE_OFFSET_LOAD: {
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
  case AIE2P::G_AIE_OFFSET_SEXTLOAD:
  case AIE2P::G_AIE_OFFSET_ZEXTLOAD:
  case AIE2P::G_AIE_POSTINC_ZEXTLOAD:
  case AIE2P::G_AIE_POSTINC_SEXTLOAD: {
    switch (LoadStoreSize) {
    case 8:
      return checkSignedImmediateRange<4, 1>(Offset);
    case 16:
      return checkSignedImmediateRange<4, 2>(Offset);
    default:
      return false;
    }
  }
  case AIE2P::G_AIE_POSTINC_STORE:
  case AIE2P::G_AIE_POSTINC_LOAD: {
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

unsigned AIE2PInstrInfo::getGenericAddVectorEltOpcode() const {
  return AIE2P::G_AIE_ADD_VECTOR_ELT_HI;
}

unsigned AIE2PInstrInfo::getGenericInsertVectorEltOpcode() const {
  return AIE2P::G_AIE_INSERT_VECTOR_ELT;
}

unsigned AIE2PInstrInfo::getGenericExtractVectorEltOpcode(bool SignExt) const {
  return SignExt ? AIE2P::G_AIE_SEXT_EXTRACT_VECTOR_ELT
                 : AIE2P::G_AIE_ZEXT_EXTRACT_VECTOR_ELT;
}

unsigned AIE2PInstrInfo::getGenericPadVectorOpcode() const {
  return AIE2P::G_AIE_PAD_VECTOR_UNDEF;
}

unsigned AIE2PInstrInfo::getGenericUnpadVectorOpcode() const {
  return AIE2P::G_AIE_UNPAD_VECTOR;
}

unsigned AIE2PInstrInfo::getGenericBroadcastVectorOpcode() const {
  return AIE2P::G_AIE_BROADCAST_VECTOR;
}

unsigned AIE2PInstrInfo::getGenericPostIncLoadOpcode() const {
  return AIE2P::G_AIE_POSTINC_LOAD;
}

unsigned AIE2PInstrInfo::getGenericPostIncStoreOpcode() const {
  return AIE2P::G_AIE_POSTINC_STORE;
}

unsigned AIE2PInstrInfo::getGenericVSelOpcode() const {
  return AIE2P::G_AIE_VSEL;
}

unsigned AIE2PInstrInfo::getGenericVShiftOpcode() const {
  return AIE2P::G_AIE_VSHIFT_RIGHT;
}

unsigned AIE2PInstrInfo::getGenericShuffleVectorOpcode() const {
  return AIE2P::G_AIE_SHUFFLE_VECTOR;
}

unsigned AIE2PInstrInfo::getGenericExtractSubvectorOpcode() const {
  return AIE2P::G_AIE_EXTRACT_SUBVECTOR;
}

unsigned AIE2PInstrInfo::getGenericIntegerComparisonOpcode() const {
  return AIE2P::G_AIE_VECTOR_ICMP;
}

Register AIE2PInstrInfo::getSSStatusReg() const { return AIE2P::srSS0; }

Register AIE2PInstrInfo::getMSStatusReg() const { return AIE2P::srMS0; }

unsigned AIE2PInstrInfo::getMoveToMSOpcode(MachineInstr &I,
                                           unsigned ConstTLastVal) const {
  const bool UseTLastImm = (ConstTLastVal == 0);
  const unsigned IntrinsicID = cast<GIntrinsic>(I).getIntrinsicID();
  switch (IntrinsicID) {
  case Intrinsic::aie2p_put_ms:
    return UseTLastImm ? AIE2P::MOV_st_mMStream_tlast_imm : AIE2P::MOV_tlast;
  case Intrinsic::aie2p_put_ms_nb:
    return UseTLastImm ? AIE2P::MOV_nb_st_mMStream_tlast_imm
                       : AIE2P::MOV_nb_tlast;
  default:
    llvm_unreachable("Unexpected Intrinsic ID");
  }
}

Register AIE2PInstrInfo::getPackSignCReg() const { return AIE2P::packSign0; }

Register AIE2PInstrInfo::getUnpackSignCReg() const {
  return AIE2P::unpackSign0;
}

unsigned AIE2PInstrInfo::getScalarRegSize() const { return 32; }

unsigned AIE2PInstrInfo::getBasicVecRegSize() const { return 256; }

unsigned AIE2PInstrInfo::getBasicVectorBitSize() const { return 512; }

unsigned AIE2PInstrInfo::getMaxVectorBitSize() const { return 2048; }

unsigned AIE2PInstrInfo::getMaxSupportedLdStIncSize() const { return 2048; }

using AbstractOp = AIEBaseInstrInfo::AbstractOp;

std::optional<const AbstractOp>
AIE2PInstrInfo::parseAbstractOp(const MachineInstr &MI) const {

  switch (MI.getOpcode()) {
  case AIE2P::VADD_32:
    return AbstractOp{AbstractOp::OperationType::VECTOR_ADD,
                      {MI.getOperand(1).getReg(), MI.getOperand(2).getReg()},
                      {}};
  case AIE2P::VBCST_32:
    return AbstractOp{AbstractOp::OperationType::VECTOR_BROADCAST,
                      {},
                      {MI.getOperand(1).getReg()}};
  case AIE2P::VSEL_32:
    return AbstractOp{AbstractOp::OperationType::VECTOR_SELECT,
                      {MI.getOperand(1).getReg(), MI.getOperand(2).getReg()},
                      {MI.getOperand(3).getReg()}};
  case AIE2P::VLDB_4x16_lo:
  case AIE2P::VLDB_4x16_hi:
  case AIE2P::VLDB_4x32_lo:
  case AIE2P::VLDB_4x32_hi:
  case AIE2P::VLDB_4x64_lo:
  case AIE2P::VLDB_4x64_hi:
    return AbstractOp{AbstractOp::OperationType::VECTOR_XWAY_LOAD,
                      {MI.getOperand(1).getReg()},
                      {}};
  }
  return std::nullopt;
}

using IfConvSupport = AIEBaseInstrInfo::IfConvSupport;
std::optional<IfConvSupport> AIE2PInstrInfo::getIfConvSupport() const {
  IfConvSupport Result;

  Result.BranchToSelectMap[AIE2P::PseudoJNZ] = AIE2P::SEL_NEZ;
  Result.BranchToSelectMap[AIE2P::PseudoJZ] = AIE2P::SEL_EQZ;

  Result.ScalarRegisterClass = &AIE2P::eRRegClass;
  Result.SelectRegisterClass = &AIE2P::mR27_selectRegClass;

  Result.registerOperandIndex(IfConvSupport::TrueReg, 0);
  Result.registerOperandIndex(IfConvSupport::FalseReg, 1);
  Result.registerOperandIndex(IfConvSupport::ConditionReg, 2);

  return Result;
}
