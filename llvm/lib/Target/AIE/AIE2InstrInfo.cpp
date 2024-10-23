//===-- AIE2InstrInfo.cpp -AIEngine V2 Instruction Information -*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIEngine V2 implementation of the TargetInstrInfo
// class.
//
//===----------------------------------------------------------------------===//

#include "AIE2InstrInfo.h"
#include "AIE2.h"
#include "AIE2RegisterInfo.h"
#include "AIE2Subtarget.h"
#include "AIE2TargetMachine.h"
#include "AIEHazardRecognizer.h"
#include "AIEMachineFunctionInfo.h"
#include "AIEMachineScheduler.h"
#include "AIETiedRegOperands.h"
#include "MCTargetDesc/AIE2MCTargetDesc.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/GlobalISel/GenericMachineInstrs.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/IntrinsicsAIE2.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Automaton.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"

#define DEBUG_TYPE "aie-codegen"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "AIE2GenInstrInfo.inc"

#include "AIE2GenMemoryCycles.inc"
#include "AIE2GenPreSchedLowering.inc"
#include "AIE2GenSplitInstrTables.inc"
#include "AIE2GenVarInstructionItin.inc"

namespace {
const AIE2MCFormats AIE2Formats;
} // namespace

AIE2InstrInfo::AIE2InstrInfo()
    : AIE2GenInstrInfo(AIE2::ADJCALLSTACKUP, AIE2::ADJCALLSTACKDOWN) {
  FormatInterface = &AIE2Formats;
  FuncUnitWrapper::setFormatInterface(FormatInterface);
}

unsigned AIE2InstrInfo::getReturnOpcode() const { return AIE2::PseudoRET; }

unsigned AIE2InstrInfo::getCallOpcode(const MachineFunction &CallerF,
                                      bool IsIndirect, bool IsTailCall) const {
  if (IsTailCall)
    llvm_unreachable("Tail calls not supported yet.\n");
  return IsIndirect ? AIE2::PseudoJL_IND : AIE2::PseudoJL;
}

unsigned AIE2InstrInfo::getNopOpcode(size_t Size) const {
  assert(Size == 0);
  return AIE2::NOP;
}

unsigned AIE2InstrInfo::getOppositeBranchOpcode(unsigned Opc) const {
  switch (Opc) {
  default:
    llvm_unreachable("Unrecognized conditional branch");
  case AIE2::PseudoJNZ:
    return AIE2::PseudoJZ;
  case AIE2::PseudoJZ:
    return AIE2::PseudoJNZ;
  }
  return 0;
}

bool AIE2InstrInfo::isJNZ(unsigned Opc) const { return Opc == AIE2::PseudoJNZ; }
bool AIE2InstrInfo::isJZ(unsigned Opc) const { return Opc == AIE2::PseudoJZ; }
bool AIE2InstrInfo::isCall(unsigned Opc) const {
  return Opc == AIE2::JL || Opc == AIE2::JL_IND;
}

bool AIE2InstrInfo::jumpsToUnknown(unsigned Opc) const {
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
  return Opc == AIE2::RET || Opc == AIE2::JL || Opc == AIE2::JL_IND;
}

bool AIE2InstrInfo::isIConst(unsigned Opc) const {
  switch (Opc) {
  case AIE2::MOVA_lda_cg:
  case AIE2::MOVX_alu_cg:
  case AIE2::MOV_mv_cg:
  case AIE2::MOVXM_lng_cg:
  case AIE2::MOV_RLC_imm10_pseudo:
  case AIE2::MOV_PD_imm10_pseudo:
  case AIE2::MOV_S_imm10_pseudo:
  case AIE2::MOV_scalar_imm10_pseudo:
  case AIE2::MOV_RLC_imm11_pseudo:
  case AIE2::MOV_PD_imm11_pseudo:
    return true;
  default:
    return false;
  }
}

bool AIE2InstrInfo::isBooleanNoOp(unsigned Opc) const {
  return Opc == AIE2::NEZ || AIEBaseInstrInfo::isBooleanNoOp(Opc);
}

bool AIE2InstrInfo::isBooleanNot(unsigned Opc) const {
  return Opc == AIE2::EQZ;
}

bool AIE2InstrInfo::isConstStep(const MachineInstr &MI, int64_t &Step) const {
  unsigned Opcode = MI.getOpcode();
  if (Opcode == AIE2::ADD_add_r_ri) {
    Step = MI.getOperand(2).getImm();
    return true;
  }
  return false;
}

bool AIE2InstrInfo::verifyGenericInstruction(const MachineInstr &MI,
                                             StringRef &ErrInfo) const {
  const MachineRegisterInfo &MRI = MI.getMF()->getRegInfo();
  switch (MI.getOpcode()) {
  case AIE2::G_AIE_ZEXT_EXTRACT_VECTOR_ELT:
  case AIE2::G_AIE_SEXT_EXTRACT_VECTOR_ELT:
    ErrInfo = "Expected 32bit scalar destination";
    return MRI.getType(MI.getOperand(0).getReg()) == LLT::scalar(32);
  case AIE2::G_AIE_PAD_VECTOR_UNDEF:
    return verifySameLaneTypes(MI, ErrInfo) &&
           isLegalTypeToUnpad(MRI.getType(MI.getOperand(0).getReg()),
                              &ErrInfo) &&
           isLegalTypeToPad(MRI.getType(MI.getOperand(1).getReg()), &ErrInfo);
  case AIE2::G_AIE_UNPAD_VECTOR:
    return verifySameLaneTypes(MI, ErrInfo) &&
           isLegalTypeToPad(MRI.getType(MI.getOperand(0).getReg()), &ErrInfo) &&
           isLegalTypeToUnpad(MRI.getType(MI.getOperand(1).getReg()), &ErrInfo);
  default:
    return true;
  }
}

// If we lose memory operands on accesses to some of our special
// memory regions, we may be applying unsafe memory disambiguation.
// Hence, we check these accesses in the machine verifier.
bool AIE2InstrInfo::verifyMemOperand(const MachineInstr &MI,
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
  case AIE2::LDA_TM:
    if (!CheckMemOp(MachineMemOperand::Flags::MOLoad, AIETileMem)) {
      ErrInfo = "Required Load TileMemory MemOperand not found";
      return false;
    }
    break;
  case AIE2::ST_TM:
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
unsigned AIE2InstrInfo::getJumpOpcode() const { return AIE2::PseudoJ_jump_imm; }

unsigned AIE2InstrInfo::getPseudoMoveOpcode() const { return AIE2::PseudoMove; }

unsigned AIE2InstrInfo::getOffsetMemOpcode(unsigned BaseMemOpcode) const {
  switch (BaseMemOpcode) {
  case TargetOpcode::G_STORE:
    return AIE2::G_AIE_OFFSET_STORE;
  case TargetOpcode::G_LOAD:
    return AIE2::G_AIE_OFFSET_LOAD;
  case TargetOpcode::G_SEXTLOAD:
    return AIE2::G_AIE_OFFSET_SEXTLOAD;
  case TargetOpcode::G_ZEXTLOAD:
    return AIE2::G_AIE_OFFSET_ZEXTLOAD;
  }
  llvm_unreachable("not a generic load/store");
}

std::optional<unsigned> AIE2InstrInfo::getCombinedPostIncOpcode(
    MachineInstr &BaseMemI, MachineInstr &PostIncI, TypeSize Size) const {
  switch (PostIncI.getOpcode()) {
  case TargetOpcode::G_PTR_ADD:
    if (Size >= 1024)
      return {};
    switch (BaseMemI.getOpcode()) {
    case TargetOpcode::G_STORE:
      return AIE2::G_AIE_POSTINC_STORE;
    case TargetOpcode::G_LOAD:
      return AIE2::G_AIE_POSTINC_LOAD;
    case TargetOpcode::G_SEXTLOAD:
      return AIE2::G_AIE_POSTINC_SEXTLOAD;
    case TargetOpcode::G_ZEXTLOAD:
      return AIE2::G_AIE_POSTINC_ZEXTLOAD;
    }
    break;
  case TargetOpcode::G_INTRINSIC:
    switch (cast<GIntrinsic>(PostIncI).getIntrinsicID()) {
    case Intrinsic::aie2_add_2d:
      if (Size >= 1024)
        return {};
      switch (BaseMemI.getOpcode()) {
      case TargetOpcode::G_STORE:
        return AIE2::G_AIE_POSTINC_2D_STORE;
      case TargetOpcode::G_LOAD:
        return AIE2::G_AIE_POSTINC_2D_LOAD;
      case TargetOpcode::G_SEXTLOAD:
        return AIE2::G_AIE_POSTINC_2D_SEXTLOAD;
      case TargetOpcode::G_ZEXTLOAD:
        return AIE2::G_AIE_POSTINC_2D_ZEXTLOAD;
      }
      break;
    case Intrinsic::aie2_add_3d:
      if (Size >= 1024)
        return {};
      switch (BaseMemI.getOpcode()) {
      case TargetOpcode::G_STORE:
        return AIE2::G_AIE_POSTINC_3D_STORE;
      case TargetOpcode::G_LOAD:
        return AIE2::G_AIE_POSTINC_3D_LOAD;
      case TargetOpcode::G_SEXTLOAD:
        return AIE2::G_AIE_POSTINC_3D_SEXTLOAD;
      case TargetOpcode::G_ZEXTLOAD:
        return AIE2::G_AIE_POSTINC_3D_ZEXTLOAD;
      }
      break;
    }
    break;
  }
  return {};
}

MCSlotKind AIE2InstrInfo::getSlotKind(unsigned Opcode) const {
  return FormatInterface->getSlotKind(Opcode);
}

const MCSlotInfo *AIE2InstrInfo::getSlotInfo(const MCSlotKind Kind) const {
  return FormatInterface->getSlotInfo(Kind);
}

const PacketFormats &AIE2InstrInfo::getPacketFormats() const {
  return FormatInterface->getPacketFormats();
}

// Implement CopyToReg/CopyFromReg
void AIE2InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator MBBI,
                                const DebugLoc &DL, MCRegister DstReg,
                                MCRegister SrcReg, bool KillSrc) const {
  MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
  const TargetRegisterInfo &TRI = *MRI.getTargetRegisterInfo();

  // TODO : add support for 128-bit mask register
  if (AIE2::mMvSclSrcRegClass.contains(SrcReg) &&
      AIE2::mMvSclDstRegClass.contains(DstReg)) {
    const unsigned MOVSclOpcode = getScalarMovOpcode(DstReg, SrcReg);
    BuildMI(MBB, MBBI, DL, get(MOVSclOpcode), DstReg)
        .addReg(SrcReg, getKillRegState(KillSrc));
  } else if ((AIE2::eLRegClass.contains(SrcReg)) &&
             (AIE2::eLRegClass.contains(DstReg))) {
    BuildMI(MBB, MBBI, DL, get(AIE2::MOV_mv_scl),
            TRI.getSubReg(DstReg, AIE2::sub_l_even))
        .addReg(TRI.getSubReg(SrcReg, AIE2::sub_l_even),
                getKillRegState(KillSrc));
    BuildMI(MBB, MBBI, DL, get(AIE2::MOV_mv_scl),
            TRI.getSubReg(DstReg, AIE2::sub_l_odd))
        .addReg(TRI.getSubReg(SrcReg, AIE2::sub_l_odd),
                getKillRegState(KillSrc));
  } else if ((AIE2::eDRegClass.contains(SrcReg)) &&
             (AIE2::eDRegClass.contains(DstReg))) {
    copyThroughSubRegs(MBB, MBBI, DL, DstReg, SrcReg, KillSrc);
  } else if ((AIE2::eDSRegClass.contains(SrcReg)) &&
             (AIE2::eDSRegClass.contains(DstReg))) {
    copyThroughSubRegs(MBB, MBBI, DL, DstReg, SrcReg, KillSrc);
  } else if ((AIE2::VEC128RegClass.contains(SrcReg) ||
              AIE2::VEC256RegClass.contains(SrcReg) ||
              AIE2::ACC256RegClass.contains(SrcReg)) &&
             (AIE2::VEC128RegClass.contains(DstReg) ||
              AIE2::VEC256RegClass.contains(DstReg) ||
              AIE2::ACC256RegClass.contains(DstReg))) {
    BuildMI(MBB, MBBI, DL, get(AIE2::VMOV_mv_w), DstReg)
        .addReg(SrcReg, getKillRegState(KillSrc));
  } else if ((AIE2::VEC512RegClass.contains(SrcReg) ||
              AIE2::ACC512RegClass.contains(SrcReg)) &&
             (AIE2::VEC512RegClass.contains(DstReg) ||
              AIE2::ACC512RegClass.contains(DstReg))) {
    BuildMI(MBB, MBBI, DL, get(AIE2::VMOV_mv_x), DstReg)
        .addReg(SrcReg, getKillRegState(KillSrc));
  } else if ((AIE2::VEC1024RegClass.contains(SrcReg)) &&
             (AIE2::VEC1024RegClass.contains(DstReg))) {
    BuildMI(MBB, MBBI, DL, get(AIE2::VMOV_mv_x),
            TRI.getSubReg(DstReg, AIE2::sub_512_lo))
        .addReg(TRI.getSubReg(SrcReg, AIE2::sub_512_lo),
                getKillRegState(KillSrc));
    BuildMI(MBB, MBBI, DL, get(AIE2::VMOV_mv_x),
            TRI.getSubReg(DstReg, AIE2::sub_512_hi))
        .addReg(TRI.getSubReg(SrcReg, AIE2::sub_512_hi),
                getKillRegState(KillSrc));
  } else if ((AIE2::ACC1024RegClass.contains(SrcReg)) &&
             (AIE2::ACC1024RegClass.contains(DstReg))) {
    BuildMI(MBB, MBBI, DL, get(AIE2::VMOV_mv_cm), DstReg)
        .addReg(SrcReg, getKillRegState(KillSrc));
  } else if ((AIE2::VEC1024RegClass.contains(SrcReg) ||
              AIE2::ACC1024RegClass.contains(SrcReg)) &&
             (AIE2::VEC1024RegClass.contains(DstReg) ||
              AIE2::ACC1024RegClass.contains(DstReg))) {
    BuildMI(MBB, MBBI, DL, get(AIE2::VMOV_mv_x),
            TRI.getSubReg(DstReg, AIE2::sub_512_lo))
        .addReg(TRI.getSubReg(SrcReg, AIE2::sub_512_lo),
                getKillRegState(KillSrc));
    BuildMI(MBB, MBBI, DL, get(AIE2::VMOV_mv_x),
            TRI.getSubReg(DstReg, AIE2::sub_512_hi))
        .addReg(TRI.getSubReg(SrcReg, AIE2::sub_512_hi),
                getKillRegState(KillSrc));
  } else if ((AIE2::SPARSEVEC640RegClass.contains(SrcReg)) &&
             (AIE2::SPARSEVEC640RegClass.contains(DstReg))) {
    BuildMI(MBB, MBBI, DL, get(AIE2::VMOV_mv_x),
            TRI.getSubReg(DstReg, AIE2::sub_sparse_x))
        .addReg(TRI.getSubReg(SrcReg, AIE2::sub_sparse_x),
                getKillRegState(KillSrc));
    BuildMI(MBB, MBBI, DL, get(AIE2::VMOV_mv_w),
            TRI.getSubReg(DstReg, AIE2::sub_sparse_q))
        .addReg(TRI.getSubReg(SrcReg, AIE2::sub_sparse_q),
                getKillRegState(KillSrc));
  } else {
    llvm_unreachable("unhandled case in copyPhysReg");
  }
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

  // FIXME?? Is it safe to constrain a reg class at this stage?
  // My current belief is yes:
  //  - we actually aren't constraining the set of allowed physical registers,
  //  - and anyway, eP_as_32BitRegClass is (funnily) considered a subclass of
  //    eP, meaning the other instructions which make use of Reg shouldn't
  //    complain. (Same for e.g. eM).
  // Does that mean we could just use eP_as_32BitRegClass and
  // eM_as_32BitRegClass everywhere??

  if (auto *NewRC = MRI.constrainRegClass(Reg, &AIE2::eP_as_32BitRegClass))
    return NewRC;
  if (auto *NewRC = MRI.constrainRegClass(Reg, &AIE2::eM_as_32BitRegClass))
    return NewRC;
  if (auto *NewRC = MRI.constrainRegClass(Reg, &AIE2::eDC_as_32BitRegClass))
    return NewRC;
  if (auto *NewRC = MRI.constrainRegClass(Reg, &AIE2::eDJ_as_32BitRegClass))
    return NewRC;
  if (auto *NewRC = MRI.constrainRegClass(Reg, &AIE2::eDN_as_32BitRegClass))
    return NewRC;
  return RC;
}

Register AIE2InstrInfo::isLoadFromStackSlot(const MachineInstr &MI,
                                            int &FrameIndex) const {
  switch (MI.getOpcode()) {
  default:
    return 0;
  case AIE2::LDA_dms_spill:
  case AIE2::LDA_dmv_lda_q_ag_spill:
  case AIE2::VLDA_dmw_lda_am_ag_spill:
  case AIE2::VLDA_dmw_lda_w_ag_spill:
  case AIE2::VLDA_L_SPILL:
  case AIE2::VLDA_X_SPILL:
  case AIE2::VLDA_QX_SPILL:
  case AIE2::VLDA_Y_SPILL:
  case AIE2::VLDA_BM_SPILL:
  case AIE2::VLDA_CM_SPILL:
  case AIE2::LDA_D_SPILL:
  case AIE2::LDA_DS_SPILL:
    break;
  }

  if (MI.getOperand(1).isFI()) {
    FrameIndex = MI.getOperand(1).getIndex();
    return MI.getOperand(0).getReg();
  }

  return 0;
}

Register AIE2InstrInfo::isStoreToStackSlot(const MachineInstr &MI,
                                           int &FrameIndex) const {
  switch (MI.getOpcode()) {
  default:
    return 0;
  case AIE2::ST_dms_spill:
  case AIE2::ST_dmv_sts_q_ag_spill:
  case AIE2::VST_dmw_sts_am_ag_spill:
  case AIE2::VST_dmw_sts_w_ag_spill:
  case AIE2::VST_L_SPILL:
  case AIE2::VST_X_SPILL:
  case AIE2::VST_QX_SPILL:
  case AIE2::VST_Y_SPILL:
  case AIE2::VST_BM_SPILL:
  case AIE2::VST_CM_SPILL:
  case AIE2::ST_D_SPILL:
  case AIE2::ST_DS_SPILL:
    break;
  }

  if (MI.getOperand(1).isFI()) {
    FrameIndex = MI.getOperand(1).getIndex();
    return MI.getOperand(0).getReg();
  }

  return 0;
}

// Store a register to a stack slot.  Used in eliminating FrameIndex pseduo-ops.
void AIE2InstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                        MachineBasicBlock::iterator I,
                                        Register SrcReg, bool IsKill, int FI,
                                        const TargetRegisterClass *RC,
                                        const TargetRegisterInfo *TRI,
                                        Register VReg) const {
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
  LLVM_DEBUG(dbgs() << "Attempting to Store: " << printReg(SrcReg, TRI)
                    << " To " << FI << "\n");
  if (regClassMatches(AIE2::mSclStRegClass, RC, SrcReg)) {
    Opcode = AIE2::ST_dms_spill;
  } else if (regClassMatches(AIE2::eLRegClass, RC, SrcReg)) {
    Opcode = AIE2::VST_L_SPILL;
  } else if (regClassMatches(AIE2::VEC256RegClass, RC, SrcReg)) {
    Opcode = AIE2::VST_dmw_sts_w_ag_spill;
  } else if (regClassMatches(AIE2::VEC128RegClass, RC, SrcReg)) {
    Opcode = AIE2::ST_dmv_sts_q_ag_spill;
  } else if (regClassMatches(AIE2::VEC512RegClass, RC, SrcReg)) {
    Opcode = AIE2::VST_X_SPILL;
  } else if (regClassMatches(AIE2::SPARSEVEC640RegClass, RC, SrcReg)) {
    Opcode = AIE2::VST_QX_SPILL;
  } else if (regClassMatches(AIE2::VEC1024RegClass, RC, SrcReg)) {
    Opcode = AIE2::VST_Y_SPILL;
  } else if (regClassMatches(AIE2::ACC256RegClass, RC, SrcReg)) {
    Opcode = AIE2::VST_dmw_sts_am_ag_spill;
  } else if (regClassMatches(AIE2::ACC512RegClass, RC, SrcReg)) {
    Opcode = AIE2::VST_BM_SPILL;
  } else if (regClassMatches(AIE2::ACC1024RegClass, RC, SrcReg)) {
    Opcode = AIE2::VST_CM_SPILL;
  } else if (regClassMatches(AIE2::eDRegClass, RC, SrcReg)) {
    Opcode = AIE2::ST_D_SPILL;
  } else if (regClassMatches(AIE2::eDSRegClass, RC, SrcReg)) {
    Opcode = AIE2::ST_DS_SPILL;
  } else if (regClassMatches(AIE2::eSRegClass, RC, SrcReg) ||
             regClassMatches(AIE2::spill_eS_to_eRRegClass, RC, SrcReg)) {
    // Can't spill these directly.  Need to bounce through a GPR.
    MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
    Register ScratchReg = MRI.createVirtualRegister(&AIE2::eRRegClass);
    BuildMI(MBB, I, DL, get(AIE2::MOV_mv_scl), ScratchReg)
        .addReg(SrcReg, getKillRegState(IsKill));
    Opcode = AIE2::ST_dms_spill;
    SrcReg = ScratchReg;
    IsKill = true;
  } else {
    LLVM_DEBUG(dbgs() << "*** Tried inserting before:"; I->dump());
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
void AIE2InstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                         MachineBasicBlock::iterator I,
                                         Register DstReg, int FI,
                                         const TargetRegisterClass *RC,
                                         const TargetRegisterInfo *TRI,
                                         Register VReg) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();

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

  unsigned Opcode;
  LLVM_DEBUG(dbgs() << "Attempting to Load: " << printReg(DstReg, TRI)
                    << " From " << FI << "\n");
  if (regClassMatches(AIE2::mLdaSclRegClass, RC, DstReg)) {
    Opcode = AIE2::LDA_dms_spill;
  } else if (regClassMatches(AIE2::eLRegClass, RC, DstReg)) {
    Opcode = AIE2::VLDA_L_SPILL;
  } else if (regClassMatches(AIE2::VEC256RegClass, RC, DstReg)) {
    Opcode = AIE2::VLDA_dmw_lda_w_ag_spill;
  } else if (regClassMatches(AIE2::VEC128RegClass, RC, DstReg)) {
    Opcode = AIE2::LDA_dmv_lda_q_ag_spill;
  } else if (regClassMatches(AIE2::VEC512RegClass, RC, DstReg)) {
    Opcode = AIE2::VLDA_X_SPILL;
  } else if (regClassMatches(AIE2::VEC1024RegClass, RC, DstReg)) {
    Opcode = AIE2::VLDA_Y_SPILL;
  } else if (regClassMatches(AIE2::SPARSEVEC640RegClass, RC, DstReg)) {
    Opcode = AIE2::VLDA_QX_SPILL;
  } else if (regClassMatches(AIE2::ACC256RegClass, RC, DstReg)) {
    Opcode = AIE2::VLDA_dmw_lda_am_ag_spill;
  } else if (regClassMatches(AIE2::ACC512RegClass, RC, DstReg)) {
    Opcode = AIE2::VLDA_BM_SPILL;
  } else if (regClassMatches(AIE2::ACC1024RegClass, RC, DstReg)) {
    Opcode = AIE2::VLDA_CM_SPILL;
  } else if (regClassMatches(AIE2::eDRegClass, RC, DstReg)) {
    Opcode = AIE2::LDA_D_SPILL;
  } else if (regClassMatches(AIE2::eDSRegClass, RC, DstReg)) {
    Opcode = AIE2::LDA_DS_SPILL;
  } else if (regClassMatches(AIE2::eSRegClass, RC, DstReg) ||
             regClassMatches(AIE2::spill_eS_to_eRRegClass, RC, DstReg)) {
    // Can't spill these directly.  Need to bounce through a GPR.
    MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
    Register Reg = MRI.createVirtualRegister(&AIE2::eRRegClass);
    BuildMI(MBB, I, DL, get(AIE2::LDA_dms_spill), Reg)
        .addFrameIndex(FI)
        .addMemOperand(CreateMMO(FI));
    BuildMI(MBB, I, DL, get(AIE2::MOV_mv_scl), DstReg)
        .addReg(Reg, getKillRegState(true));
    return;
  } else {
    LLVM_DEBUG(dbgs() << "*** Tried inserting before:"; I->dump());
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
AIE2InstrInfo::getSpillPseudoExpandInfo(const MachineInstr &MI) const {
  if (!MI.isPseudo())
    return {};

  switch (MI.getOpcode()) {
  case AIE2::VLDA_L_SPILL:
    return {{AIE2::LDA_dms_spill, AIE2::sub_l_even},
            {AIE2::LDA_dms_spill, AIE2::sub_l_odd}};
  case AIE2::VLDA_X_SPILL:
    return {{AIE2::VLDA_dmw_lda_w_ag_spill, AIE2::sub_256_lo},
            {AIE2::VLDA_dmw_lda_w_ag_spill, AIE2::sub_256_hi}};
  case AIE2::VLDA_QX_SPILL:
    return {{AIE2::VLDA_dmw_lda_w_ag_spill, AIE2::sub_256_lo},
            {AIE2::VLDA_dmw_lda_w_ag_spill, AIE2::sub_256_hi},
            {AIE2::LDA_dmv_lda_q_ag_spill, AIE2::sub_sparse_q}};
  case AIE2::VLDA_Y_SPILL:
    return {{AIE2::VLDA_X_SPILL, AIE2::sub_512_lo},
            {AIE2::VLDA_X_SPILL, AIE2::sub_512_hi}};
  case AIE2::VST_L_SPILL:
    return {{AIE2::ST_dms_spill, AIE2::sub_l_even},
            {AIE2::ST_dms_spill, AIE2::sub_l_odd}};
  case AIE2::VST_X_SPILL:
    return {{AIE2::VST_dmw_sts_w_ag_spill, AIE2::sub_256_lo},
            {AIE2::VST_dmw_sts_w_ag_spill, AIE2::sub_256_hi}};
  case AIE2::VST_QX_SPILL:
    return {{AIE2::VST_dmw_sts_w_ag_spill, AIE2::sub_256_lo},
            {AIE2::VST_dmw_sts_w_ag_spill, AIE2::sub_256_hi},
            {AIE2::ST_dmv_sts_q_ag_spill, AIE2::sub_sparse_q}};
  case AIE2::VST_Y_SPILL:
    return {{AIE2::VST_X_SPILL, AIE2::sub_512_lo},
            {AIE2::VST_X_SPILL, AIE2::sub_512_hi}};
  case AIE2::VLDA_BM_SPILL:
    return {{AIE2::VLDA_dmw_lda_am_ag_spill, AIE2::sub_256_lo},
            {AIE2::VLDA_dmw_lda_am_ag_spill, AIE2::sub_256_hi}};
  case AIE2::VLDA_CM_SPILL:
    return {{AIE2::VLDA_BM_SPILL, AIE2::sub_512_lo},
            {AIE2::VLDA_BM_SPILL, AIE2::sub_512_hi}};
  case AIE2::VST_BM_SPILL:
    return {{AIE2::VST_dmw_sts_am_ag_spill, AIE2::sub_256_lo},
            {AIE2::VST_dmw_sts_am_ag_spill, AIE2::sub_256_hi}};
  case AIE2::VST_CM_SPILL:
    return {{AIE2::VST_BM_SPILL, AIE2::sub_512_lo},
            {AIE2::VST_BM_SPILL, AIE2::sub_512_hi}};
  case AIE2::LDA_D_SPILL:
    return {{AIE2::LDA_dms_spill, AIE2::sub_mod},
            {AIE2::LDA_dms_spill, AIE2::sub_dim_size},
            {AIE2::LDA_dms_spill, AIE2::sub_dim_stride},
            {AIE2::LDA_dms_spill, AIE2::sub_dim_count}};
  case AIE2::ST_D_SPILL:
    return {{AIE2::ST_dms_spill, AIE2::sub_mod},
            {AIE2::ST_dms_spill, AIE2::sub_dim_size},
            {AIE2::ST_dms_spill, AIE2::sub_dim_stride},
            {AIE2::ST_dms_spill, AIE2::sub_dim_count}};
  case AIE2::LDA_DS_SPILL:
    return {{AIE2::LDA_dms_spill, AIE2::sub_mod},
            {AIE2::LDA_dms_spill, AIE2::sub_dim_size},
            {AIE2::LDA_dms_spill, AIE2::sub_dim_stride},
            {AIE2::LDA_dms_spill, AIE2::sub_dim_count},
            {AIE2::LDA_dms_spill, AIE2::sub_hi_dim_then_sub_mod},
            {AIE2::LDA_dms_spill, AIE2::sub_hi_dim_then_sub_dim_size},
            {AIE2::LDA_dms_spill, AIE2::sub_hi_dim_then_sub_dim_stride},
            {AIE2::LDA_dms_spill, AIE2::sub_hi_dim_then_sub_dim_count}};
  case AIE2::ST_DS_SPILL:
    return {{AIE2::ST_dms_spill, AIE2::sub_mod},
            {AIE2::ST_dms_spill, AIE2::sub_dim_size},
            {AIE2::ST_dms_spill, AIE2::sub_dim_stride},
            {AIE2::ST_dms_spill, AIE2::sub_dim_count},
            {AIE2::ST_dms_spill, AIE2::sub_hi_dim_then_sub_mod},
            {AIE2::ST_dms_spill, AIE2::sub_hi_dim_then_sub_dim_size},
            {AIE2::ST_dms_spill, AIE2::sub_hi_dim_then_sub_dim_stride},
            {AIE2::ST_dms_spill, AIE2::sub_hi_dim_then_sub_dim_count}};
  default:
    llvm_unreachable("Un-handled spill opcode.");
  }
}

unsigned AIE2InstrInfo::getConstantMovOpcode(MachineRegisterInfo &MRI,
                                             unsigned int Reg,
                                             APInt &Val) const {

  const auto &TRI =
      static_cast<const AIE2RegisterInfo *>(MRI.getTargetRegisterInfo());
  unsigned int ImmSize = Val.getSignificantBits();

  const TargetRegisterClass *DstRegClass = nullptr;
  const RegClassOrRegBank &RCB = MRI.getRegClassOrRegBank(Reg);
  if (const RegisterBank *RB = RCB.dyn_cast<const RegisterBank *>())
    DstRegClass = &TRI->getMinClassForRegBank(*RB, MRI.getType(Reg));
  if (auto *TRC = RCB.dyn_cast<const TargetRegisterClass *>())
    DstRegClass = TRC;
  assert(DstRegClass != nullptr && "RC cannot be null");

  if (ImmSize <= 10) {
    if (regClassMatches(AIE2::eRLCRegClass, DstRegClass, Reg))
      return AIE2::MOV_RLC_imm10_pseudo;
    if (regClassMatches(AIE2::ePmDmRegClass, DstRegClass, Reg))
      return AIE2::MOV_PD_imm10_pseudo;
    if (regClassMatches(AIE2::eSRegClass, DstRegClass, Reg))
      return AIE2::MOV_S_imm10_pseudo;
    if (regClassMatches(AIE2::mMvSclDstRegClass, DstRegClass, Reg))
      return AIE2::MOV_scalar_imm10_pseudo;
  } else if (ImmSize <= 11) {
    if (regClassMatches(AIE2::eRLCRegClass, DstRegClass, Reg))
      return AIE2::MOV_RLC_imm11_pseudo;
    if (regClassMatches(AIE2::ePmDmRegClass, DstRegClass, Reg))
      return AIE2::MOV_PD_imm11_pseudo;
  }
  if (ImmSize <= 32) {
    return AIE2::MOVXM_lng_cg;
  }
  dbgs() << "Imm. Size: " << ImmSize << "\n"
         << "DstRegClass ID: " << DstRegClass->getID() << "\n";
  llvm_unreachable("Expected imm. size <= 32 bits");
  dbgs() << "DstRegClass ID: " << DstRegClass->getID() << "\n";
}

unsigned AIE2InstrInfo::getScalarMovOpcode(Register DstReg,
                                           Register SrcReg) const {
  return (AIE2::eRRegClass.contains(SrcReg) &&
          AIE2::eRRegClass.contains(DstReg))
             ? AIE2::MOV_SCL_pseudo
             : AIE2::MOV_mv_scl;
}

unsigned AIE2InstrInfo::getCycleSeparatorOpcode() const {
  return AIE2::CYCLE_SEPARATOR;
}

unsigned AIE2InstrInfo::getGenericAddVectorEltOpcode() const {
  return AIE2::G_AIE_ADD_VECTOR_ELT_HI;
}

unsigned AIE2InstrInfo::getGenericInsertVectorEltOpcode() const {
  return AIE2::G_AIE_INSERT_VECTOR_ELT;
}

unsigned AIE2InstrInfo::getGenericExtractVectorEltOpcode(bool SignExt) const {
  return SignExt ? AIE2::G_AIE_SEXT_EXTRACT_VECTOR_ELT
                 : AIE2::G_AIE_ZEXT_EXTRACT_VECTOR_ELT;
}

unsigned AIE2InstrInfo::getGenericPadVectorOpcode() const {
  return AIE2::G_AIE_PAD_VECTOR_UNDEF;
}

unsigned AIE2InstrInfo::getGenericUnpadVectorOpcode() const {
  return AIE2::G_AIE_UNPAD_VECTOR;
}

unsigned int getVLDSparseOpcode(unsigned int PseudoOpc) {
  switch (PseudoOpc) {
  case AIE2::PSEUDO_VLD_SPARSE_POP_16_set_low:
  case AIE2::PSEUDO_VLD_SPARSE_POP_16_and_get_pointer:
  case AIE2::PSEUDO_VLD_SPARSE_POP_16_insert_hi:
    return AIE2::VLDB_SPARSE_POP_16;
  case AIE2::PSEUDO_VLD_SPARSE_PEEK_16_set_low:
  case AIE2::PSEUDO_VLD_SPARSE_PEEK_16_and_get_pointer:
  case AIE2::PSEUDO_VLD_SPARSE_PEEK_16_insert_hi:
    return AIE2::VLDB_SPARSE_PEEK_16;
  case AIE2::PSEUDO_VLD_SPARSE_RESET_16_and_get_pointer:
    return AIE2::VLDB_SPARSE_RESET_16;
  case AIE2::PSEUDO_VLD_SPARSE_FILL_16_and_get_pointer:
    return AIE2::VLDB_SPARSE_FILL_16;
  case AIE2::PSEUDO_VLD_SPARSE_POP_8_set_low:
  case AIE2::PSEUDO_VLD_SPARSE_POP_8_and_get_pointer:
  case AIE2::PSEUDO_VLD_SPARSE_POP_8_insert_hi:
    return AIE2::VLDB_SPARSE_POP_8;
  case AIE2::PSEUDO_VLD_SPARSE_PEEK_8_set_low:
  case AIE2::PSEUDO_VLD_SPARSE_PEEK_8_and_get_pointer:
  case AIE2::PSEUDO_VLD_SPARSE_PEEK_8_insert_hi:
    return AIE2::VLDB_SPARSE_PEEK_8;
  case AIE2::PSEUDO_VLD_SPARSE_RESET_8_and_get_pointer:
    return AIE2::VLDB_SPARSE_RESET_8;
  case AIE2::PSEUDO_VLD_SPARSE_FILL_8_and_get_pointer:
    return AIE2::VLDB_SPARSE_FILL_8;
  case AIE2::PSEUDO_VLD_SPARSE_POP_4_set_low:
  case AIE2::PSEUDO_VLD_SPARSE_POP_4_and_get_pointer:
  case AIE2::PSEUDO_VLD_SPARSE_POP_4_insert_hi:
    return AIE2::VLDB_SPARSE_POP_4;
  case AIE2::PSEUDO_VLD_SPARSE_PEEK_4_set_low:
  case AIE2::PSEUDO_VLD_SPARSE_PEEK_4_and_get_pointer:
  case AIE2::PSEUDO_VLD_SPARSE_PEEK_4_insert_hi:
    return AIE2::VLDB_SPARSE_PEEK_4;
  case AIE2::PSEUDO_VLD_SPARSE_RESET_4_and_get_pointer:
    return AIE2::VLDB_SPARSE_RESET_4;
  case AIE2::PSEUDO_VLD_SPARSE_FILL_4_and_get_pointer:
    return AIE2::VLDB_SPARSE_FILL_4;
  }
  llvm_unreachable("unreachable: Failed to get sparse load opcode");
  return AIE2::INSTRUCTION_LIST_END;
}

bool AIE2InstrInfo::expandPostRAPseudo(MachineInstr &MI) const {
  auto DL = MI.getDebugLoc();
  MachineBasicBlock &MBB = *MI.getParent();
  MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
  const TargetRegisterInfo &TRI = *MRI.getTargetRegisterInfo();

  switch (MI.getOpcode()) {
  case AIE2::VLDA_L_SPILL:
  case AIE2::VST_L_SPILL:
  case AIE2::VLDA_X_SPILL:
  case AIE2::VLDA_QX_SPILL:
  case AIE2::VST_X_SPILL:
  case AIE2::VST_QX_SPILL:
  case AIE2::VLDA_BM_SPILL:
  case AIE2::VST_BM_SPILL:
  case AIE2::VLDA_Y_SPILL:
  case AIE2::VST_Y_SPILL:
  case AIE2::VLDA_CM_SPILL:
  case AIE2::VST_CM_SPILL:
  case AIE2::LDA_D_SPILL:
  case AIE2::ST_D_SPILL:
  case AIE2::LDA_DS_SPILL:
  case AIE2::ST_DS_SPILL: {
    expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(4));
    return true;
  }
  case AIE2::PseudoMove: {
    Register Dst = MI.getOperand(0).getReg();
    Register Src = MI.getOperand(1).getReg();
    const unsigned MOVSclOpcode = getScalarMovOpcode(Dst, Src);
    BuildMI(MBB, MI, DL, get(MOVSclOpcode), Dst)
        .addReg(Src, getKillRegState(true));
    MI.eraseFromParent();
    return true;
  }
  case AIE2::PSEUDO_VLD_SPARSE_PEEK_4_set_low:
  case AIE2::PSEUDO_VLD_SPARSE_POP_4_set_low:
  case AIE2::PSEUDO_VLD_SPARSE_PEEK_8_set_low:
  case AIE2::PSEUDO_VLD_SPARSE_POP_8_set_low:
  case AIE2::PSEUDO_VLD_SPARSE_PEEK_16_set_low:
  case AIE2::PSEUDO_VLD_SPARSE_POP_16_set_low: {
    Register PtrIn = MI.getOperand(2).getReg();
    Register PtrOut = MI.getOperand(0).getReg();
    Register SparseOut = MI.getOperand(1).getReg();
    Register QWl = SparseOut.asMCReg() - 4;

    BuildMI(MBB, MI, DL, get(getVLDSparseOpcode(MI.getOpcode())))
        .addDef(PtrOut)
        .addDef(QWl)
        .addUse(PtrIn);

    MI.eraseFromParent();
    return true;
  }
  case AIE2::PSEUDO_VLD_SPARSE_PEEK_4_and_get_pointer:
  case AIE2::PSEUDO_VLD_SPARSE_POP_4_and_get_pointer:
  case AIE2::PSEUDO_VLD_SPARSE_PEEK_8_and_get_pointer:
  case AIE2::PSEUDO_VLD_SPARSE_POP_8_and_get_pointer:
  case AIE2::PSEUDO_VLD_SPARSE_PEEK_16_and_get_pointer:
  case AIE2::PSEUDO_VLD_SPARSE_POP_16_and_get_pointer: {
    Register PtrIn = MI.getOperand(2).getReg();
    Register SparseOut = MI.getOperand(1).getReg();
    Register QWl = SparseOut.asMCReg() - 4;
    BuildMI(MBB, MI, DL, get(getVLDSparseOpcode(MI.getOpcode())))
        .addDef(PtrIn)
        .addDef(QWl)
        .addUse(PtrIn);
    BuildMI(MBB, MI, DL, get(AIE2::MOV_mv_scl))
        .addDef(AIE2::dn0)
        .addUse(AIE2::DP);
    MI.eraseFromParent();
    return true;
  }
  case AIE2::PSEUDO_VLD_SPARSE_PEEK_4_insert_hi:
  case AIE2::PSEUDO_VLD_SPARSE_POP_4_insert_hi:
  case AIE2::PSEUDO_VLD_SPARSE_PEEK_8_insert_hi:
  case AIE2::PSEUDO_VLD_SPARSE_POP_8_insert_hi:
  case AIE2::PSEUDO_VLD_SPARSE_PEEK_16_insert_hi:
  case AIE2::PSEUDO_VLD_SPARSE_POP_16_insert_hi: {
    Register PtrIn = MI.getOperand(2).getReg();
    Register PtrOut = MI.getOperand(0).getReg();
    Register SparseOut = MI.getOperand(1).getReg();
    Register QWh = SparseOut.asMCReg() - 8;

    BuildMI(MBB, MI, DL, get(getVLDSparseOpcode(MI.getOpcode())))
        .addDef(PtrOut)
        .addDef(QWh)
        .addUse(PtrIn);
    MI.eraseFromParent();
    return true;
  }
  case AIE2::PSEUDO_VLD_SPARSE_RESET_4_and_get_pointer:
  case AIE2::PSEUDO_VLD_SPARSE_FILL_4_and_get_pointer:
  case AIE2::PSEUDO_VLD_SPARSE_RESET_8_and_get_pointer:
  case AIE2::PSEUDO_VLD_SPARSE_FILL_8_and_get_pointer:
  case AIE2::PSEUDO_VLD_SPARSE_RESET_16_and_get_pointer:
  case AIE2::PSEUDO_VLD_SPARSE_FILL_16_and_get_pointer: {
    Register PtrIn = MI.getOperand(1).getReg();
    BuildMI(MBB, MI, DL, get(getVLDSparseOpcode(MI.getOpcode())))
        .addDef(PtrIn)
        .addUse(PtrIn);
    BuildMI(MBB, MI, DL, get(AIE2::MOV_mv_scl))
        .addDef(AIE2::dn0)
        .addUse(AIE2::DP);
    MI.eraseFromParent();
    return true;
  }
  }
  return false;
}

ScheduleHazardRecognizer *AIE2InstrInfo::CreateTargetPostRAHazardRecognizer(
    const InstrItineraryData *II, const ScheduleDAG *DAG) const {
  llvm_unreachable("AIE2 is not meant to use the post-RA list scheduler. "
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
AIE2InstrInfo::CreateTargetMIHazardRecognizer(const InstrItineraryData *II,
                                              const ScheduleDAGMI *DAG) const {
  // AIE has a fully exposed pipeline, resource and format conflicts must be
  // exactly modelled.
  return new AIEHazardRecognizer(this, II, getSelectedAltDescs(DAG),
                                 /*IsPreRA=*/DAG->hasVRegLiveness());
}

/// insertNoop - Insert a noop into the instruction stream at the specified
/// point.  This is used in Post-RA Scheduling to insert no-ops for functional
/// correctness.
void AIE2InstrInfo::insertNoop(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator MI) const {
  DebugLoc DL;
  BuildMI(MBB, MI, DL, get(AIE2::NOP));
}

// Check whether Opc is a lock operation
// This is used to adjust the latency of barrier edges into them
bool AIE2InstrInfo::isLock(unsigned Opc) const {
  switch (Opc) {
  default:
    break;
  case AIE2::ACQ_mLockId_imm:
  case AIE2::ACQ_mLockId_reg:
  case AIE2::ACQ_COND_mLockId_imm:
  case AIE2::ACQ_COND_mLockId_reg:
  case AIE2::REL_mLockId_imm:
  case AIE2::REL_mLockId_reg:
  case AIE2::REL_COND_mLockId_imm:
  case AIE2::REL_COND_mLockId_reg:
    return true;
  }
  return false;
}

bool AIE2InstrInfo::isDelayedSchedBarrier(const MachineInstr &MI) const {
  return MI.getOpcode() == AIE2::DelayedSchedBarrier;
}

bool AIE2InstrInfo::isSchedBarrier(const MachineInstr &MI) const {
  return (MI.getOpcode() == AIE2::SCHED_BARRIER ||
          MI.getOpcode() == AIE2::PseudoLoopEnd ||
          MI.getOpcode() == AIE2::MOV_CNTR || isDelayedSchedBarrier(MI));
}

unsigned AIE2InstrInfo::getNumReservedDelaySlots(const MachineInstr &MI) const {
  return 0;
}

SmallVector<TiedRegOperands, 4>
AIE2InstrInfo::getTiedRegInfo(unsigned Opcode) const {
  const SmallVector<SubRegSplit, 8> Split2DReg = {
      SubRegSplit(AIE2::sub_mod), SubRegSplit(AIE2::sub_dim_size),
      SubRegSplit(AIE2::sub_dim_stride), SubRegSplit(AIE2::sub_dim_count)};
  const SmallVector<SubRegSplit, 8> Split3DReg = {
      SubRegSplit(AIE2::sub_mod),
      SubRegSplit(AIE2::sub_dim_size),
      SubRegSplit(AIE2::sub_dim_stride),
      SubRegSplit(AIE2::sub_dim_count),
      SubRegSplit(AIE2::sub_hi_dim_then_sub_mod, /*IsUndef=*/true),
      SubRegSplit(AIE2::sub_hi_dim_then_sub_dim_size),
      SubRegSplit(AIE2::sub_hi_dim_then_sub_dim_stride),
      SubRegSplit(AIE2::sub_hi_dim_then_sub_dim_count)};

  switch (Opcode) {
  case AIE2::VLD_2D_pseudo:
  case AIE2::LDA_2D_dmv_lda_q:
  case AIE2::LDA_2D_dms_lda:
  case AIE2::LDA_2D_S8_dmhb_lda:
  case AIE2::LDA_2D_S16_dmhb_lda:
  case AIE2::LDA_2D_U8_dmhb_lda:
  case AIE2::LDA_2D_U16_dmhb_lda:
  case AIE2::VLDA_2D_dmw_lda_w:
  case AIE2::VLDA_2D_dmw_lda_am:
  case AIE2::VLDA_2D_CONV_FP32_BF16:
  case AIE2::VLDB_2D:
  case AIE2::VLDB_2D_128:
  case AIE2::VLDB_2D_UNPACK_S8_S4:
  case AIE2::VLDB_2D_UNPACK_S16_S8:
  case AIE2::VLDB_2D_UNPACK_D8_D4:
  case AIE2::VLDB_2D_UNPACK_D16_D8:
    // Constraints = "$count_out=$mod.sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/2, /*SubRegIdx=*/AIE2::sub_dim_count}},
        /*SrcOp=*/{/*OpIdx=*/4, /*SubRegIdx=*/AIE2::NoSubRegister,
                   /*SubRegsSplit=*/Split2DReg}}};
  case AIE2::LDA_3D_dmv_lda_q:
  case AIE2::LDA_3D_dms_lda:
  case AIE2::LDA_3D_S8_dmhb_lda:
  case AIE2::LDA_3D_S16_dmhb_lda:
  case AIE2::LDA_3D_U8_dmhb_lda:
  case AIE2::LDA_3D_U16_dmhb_lda:
  case AIE2::VLD_3D_pseudo:
  case AIE2::VLDA_3D_dmw_lda_w:
  case AIE2::VLDA_3D_dmw_lda_am:
  case AIE2::VLDA_3D_CONV_FP32_BF16:
  case AIE2::VLDB_3D:
  case AIE2::VLDB_3D_128:
  case AIE2::VLDB_3D_UNPACK_S8_S4:
  case AIE2::VLDB_3D_UNPACK_S16_S8:
  case AIE2::VLDB_3D_UNPACK_D8_D4:
  case AIE2::VLDB_3D_UNPACK_D16_D8:
    // Constraints = "$count_lo_out=$mod.sub_dim_count,
    //                $count_hi_out=$mod.sub_hi_dim_then_sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2::sub_dim_count},
            {/*OpIdx=*/3, /*SubRegIdx=*/AIE2::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{/*OpIdx=*/5, /*SubRegIdx=*/AIE2::NoSubRegister,
                   /*SubRegsSplit=*/Split3DReg}}};
  case AIE2::VLDA_2D_UPS_S32_D16:
  case AIE2::VLDA_2D_UPS_S64_D32:
  case AIE2::VLDA_2D_UPS_S32_D8:
  case AIE2::VLDA_2D_UPS_S64_D16:
  case AIE2::VLDA_2D_UPS_S32_S16:
  case AIE2::VLDA_2D_UPS_S64_S32:
  case AIE2::VLDA_2D_UPS_S32_S8:
  case AIE2::VLDA_2D_UPS_S64_S16:
    // Constraints = "$count_out=$mod.sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/2, /*SubRegIdx=*/AIE2::sub_dim_count}},
        /*SrcOp=*/{/*OpIdx=*/5, /*SubRegIdx=*/AIE2::NoSubRegister,
                   /*SubRegsSplit=*/Split2DReg}}};
  case AIE2::VLDA_3D_UPS_S32_D16:
  case AIE2::VLDA_3D_UPS_S64_D32:
  case AIE2::VLDA_3D_UPS_S32_D8:
  case AIE2::VLDA_3D_UPS_S64_D16:
  case AIE2::VLDA_3D_UPS_S32_S16:
  case AIE2::VLDA_3D_UPS_S64_S32:
  case AIE2::VLDA_3D_UPS_S32_S8:
  case AIE2::VLDA_3D_UPS_S64_S16:
    // Constraints = "$count_lo_out=$mod.sub_dim_count,
    //                $count_hi_out=$mod.sub_hi_dim_then_sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2::sub_dim_count},
            {/*OpIdx=*/3, /*SubRegIdx=*/AIE2::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{/*OpIdx=*/6, /*SubRegIdx=*/AIE2::NoSubRegister,
                   /*SubRegsSplit=*/Split3DReg}}};
  case AIE2::ST_2D_dmv_sts_q:
  case AIE2::VST_2D_dmw_sts_am:
  case AIE2::VST_2D_dmw_sts_w:
  case AIE2::VST_2D_128:
  case AIE2::ST_2D_dms_sts:
  case AIE2::ST_2D_S8:
  case AIE2::ST_2D_S16:
    // Constraints = "$count_out=$mod.sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2::sub_dim_count}},
        /*SrcOp=*/{/*OpIdx=*/4, /*SubRegIdx=*/AIE2::NoSubRegister,
                   /*SubRegsSplit=*/Split2DReg}}};
  case AIE2::VST_2D_PACK_D4_D8:
  case AIE2::VST_2D_PACK_D8_D16:
  case AIE2::VST_2D_PACK_S4_S8:
  case AIE2::VST_2D_PACK_S8_S16:
    // Constraints = "$count_out=$mod.sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2::sub_dim_count}},
        /*SrcOp=*/{/*OpIdx=*/3, /*SubRegIdx=*/AIE2::NoSubRegister,
                   /*SubRegsSplit=*/Split2DReg}}};
  case AIE2::ST_3D_dmv_sts_q:
  case AIE2::VST_3D_dmw_sts_w:
  case AIE2::VST_3D_128:
  case AIE2::VST_3D_dmw_sts_am:
  case AIE2::ST_3D_dms_sts:
  case AIE2::ST_3D_S8:
  case AIE2::ST_3D_S16:
    // Constraints = "$count_lo_out=$mod.sub_dim_count,
    //                $count_hi_out=$mod.sub_hi_dim_then_sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/1, /*SubRegIdx=*/AIE2::sub_dim_count},
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{/*OpIdx=*/5, /*SubRegIdx=*/AIE2::NoSubRegister,
                   /*SubRegsSplit=*/Split3DReg}}};
  case AIE2::VST_3D_PACK_D4_D8:
  case AIE2::VST_3D_PACK_D8_D16:
  case AIE2::VST_3D_PACK_S4_S8:
  case AIE2::VST_3D_PACK_S8_S16:
    // Constraints = "$count_lo_out=$mod.sub_dim_count,
    //                $count_hi_out=$mod.sub_hi_dim_then_sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/1, /*SubRegIdx=*/AIE2::sub_dim_count},
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{/*OpIdx=*/4, /*SubRegIdx=*/AIE2::NoSubRegister,
                   /*SubRegsSplit=*/Split3DReg}}};
  case AIE2::VST_CONV_2D_BF16_FP32:
    // Constraints = "$count_out=$mod.sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2::sub_dim_count}},
        /*SrcOp=*/{/*OpIdx=*/3, /*SubRegIdx=*/AIE2::NoSubRegister,
                   /*SubRegsSplit=*/Split2DReg}}};
  case AIE2::VST_CONV_3D_BF16_FP32:
    // Constraints = "$count_lo_out=$mod.sub_dim_count,
    //                $count_hi_out=$mod.sub_hi_dim_then_sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/1, /*SubRegIdx=*/AIE2::sub_dim_count},
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{/*OpIdx=*/4, /*SubRegIdx=*/AIE2::NoSubRegister,
                   /*SubRegsSplit=*/Split3DReg}}};
  case AIE2::VST_2D_SRS_D8_S32:
  case AIE2::VST_2D_SRS_D16_S64:
  case AIE2::VST_2D_SRS_D16_S32:
  case AIE2::VST_2D_SRS_D32_S64:
  case AIE2::VST_2D_SRS_S8_S32:
  case AIE2::VST_2D_SRS_S16_S64:
  case AIE2::VST_2D_SRS_S16_S32:
  case AIE2::VST_2D_SRS_S32_S64:
    // Constraints = "$count_out=$mod.sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2::sub_dim_count}},
        /*SrcOp=*/{/*OpIdx=*/3, /*SubRegIdx=*/AIE2::NoSubRegister,
                   /*SubRegsSplit=*/Split2DReg}}};
  case AIE2::VST_3D_SRS_D8_S32:
  case AIE2::VST_3D_SRS_D16_S64:
  case AIE2::VST_3D_SRS_D16_S32:
  case AIE2::VST_3D_SRS_D32_S64:
  case AIE2::VST_3D_SRS_S8_S32:
  case AIE2::VST_3D_SRS_S16_S64:
  case AIE2::VST_3D_SRS_S16_S32:
  case AIE2::VST_3D_SRS_S32_S64:
    // Constraints = "$count_lo_out=$mod.sub_dim_count,
    //                $count_hi_out=$mod.sub_hi_dim_then_sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/1, /*SubRegIdx=*/AIE2::sub_dim_count},
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/{/*OpIdx=*/4, /*SubRegIdx=*/AIE2::NoSubRegister,
                   /*SubRegsSplit=*/Split3DReg}}};
  case AIE2::PADDA_2D:
  case AIE2::PADDB_2D:
  case AIE2::PADDS_2D:
    // Constraints = "$count_out=$mod.sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{{/*OpIdx=*/1, /*SubRegIdx=*/AIE2::sub_dim_count}},
        /*SrcOp=*/
        {/*OpIdx=*/3, /*SubRegIdx=*/AIE2::NoSubRegister,
         /*SubRegsSplit=*/Split2DReg}}};
  case AIE2::PADDA_3D:
  case AIE2::PADDB_3D:
  case AIE2::PADDS_3D:
    // Constraints = "$count_lo_out=$mod.sub_dim_count,
    //                $count_hi_out=$mod.sub_hi_dim_then_sub_dim_count"
    return {TiedRegOperands{
        /*DstOps=*/{
            {/*OpIdx=*/1, /*SubRegIdx=*/AIE2::sub_dim_count},
            {/*OpIdx=*/2, /*SubRegIdx=*/AIE2::sub_hi_dim_then_sub_dim_count}},
        /*SrcOp=*/
        {/*OpIdx=*/4, /*SubRegIdx=*/AIE2::NoSubRegister,
         /*SubRegsSplit=*/Split3DReg}}};
  default:
    return {};
  }
}

bool AIE2InstrInfo::isHardwareLoopDec(unsigned Opcode) const {
  return Opcode == AIE2::LoopDec;
}
bool AIE2InstrInfo::isHardwareLoopJNZ(unsigned Opcode) const {
  return Opcode == AIE2::LoopJNZ;
}

bool AIE2InstrInfo::isHardwareLoopStart(unsigned Opcode) const {
  return Opcode == AIE2::LoopStart;
}

bool AIE2InstrInfo::isHardwareLoopEnd(unsigned Opcode) const {
  return Opcode == AIE2::PseudoLoopEnd;
}

bool AIE2InstrInfo::isZOLTripCountDef(const MachineInstr &MI,
                                      bool Pristine) const {
  return MI.getOpcode() == AIE2::ADD_NC &&
         MI.getOperand(0).getReg() == AIE2::LC &&
         (!Pristine || MI.getOperand(2).getImm() == 0);
}

void AIE2InstrInfo::adjustTripCount(MachineInstr &MI, int Adjustment) const {
  assert(isZOLTripCountDef(MI));
  auto &Imm = MI.getOperand(2);
  Imm.setImm(Imm.getImm() + Adjustment);
}

bool AIE2InstrInfo::isZeroOverheadLoopSetupInstr(const MachineInstr &MI) const {
  return isZOLTripCountDef(MI) || (MI.getOpcode() == AIE2::MOVXM_lng_cg &&
                                   (MI.getOperand(0).getReg() == AIE2::LS ||
                                    MI.getOperand(0).getReg() == AIE2::LE));
}

std::vector<MachineBasicBlock::iterator>
AIE2InstrInfo::getAlignmentBoundaries(MachineBasicBlock &MBB) const {
  std::vector<MachineBasicBlock::iterator> AlgnCandidates;
  unsigned DelaySlot = 0;

  // LoopSetupDistance will be set to number of instructions (7). In
  // PostRAScheduler, this is enforced by setting the exit latency in the
  // schduler dag mutator
  unsigned LoopSetupDistance = 0;
  bool IsCall = false;
  for (auto MI = MBB.begin(), End = MBB.end(); MI != End; ++MI) {
    if (MI->isBundle()) {
      // Return Address Candidate
      IsCall = isCallBundle(MI);
      if (IsCall && DelaySlot > 0)
        llvm_unreachable("Cannot have branch in branch delay slot!\n");

      if (DelaySlot > 0) {
        DelaySlot--;
        if (DelaySlot == 0)
          /* Region + 1 => RegionEnd */
          AlgnCandidates.emplace_back(std::next(MI));
      }

      // create regions of singleton bundle for schedule margin bundles,
      // alignment algorithm will force fill each bundle to 128-bit due
      // to the alignment requirement of 16-byte for the alignment region.
      if (LoopSetupDistance > 0) {
        AlgnCandidates.emplace_back(MI);
        LoopSetupDistance--;
      }

      if (IsCall)
        DelaySlot = getNumDelaySlots(*MI);

      // Distance of 112 bytes in terms of PM addresses corresponds to
      // 7 fully-expanded 128-bit instructions.
      if (isZOLSetupBundle(MI) && isLastZOLSetupBundleInMBB(MI))
        LoopSetupDistance = 7;
    } else if (isHardwareLoopEnd(MI->getOpcode())) {
      if (DelaySlot > 0)
        llvm_unreachable("Cannot have HWLoopEnd in branch delay slot!\n");
      // The previous instruction is the last bundle of the hardware loop
      // and should be aligned.
      AlgnCandidates.emplace_back(std::prev(MI));
    } else if (!MI->isMetaInstruction()) {
      // single instruction, there should not be any
      // after Bundle Finalization Pass
      llvm_unreachable("Found an un-expected standalone instruction !");
    }
  }
  if (LoopSetupDistance > 0)
    llvm_unreachable(
        "LoopStart Region must have a length of at-least 7 bundles!\n");

  return AlgnCandidates;
}

unsigned AIE2InstrInfo::getPseudoJNZDOpcode() const { return AIE2::PseudoJNZD; }

unsigned AIE2InstrInfo::getNumBypassedCycles(const InstrItineraryData *ItinData,
                                             const MachineInstr &DefMI,
                                             unsigned DefIdx,
                                             const MachineInstr &UseMI,
                                             unsigned UseIdx) const {
  auto GetForwardingClass = [&](const MachineInstr &MI, unsigned OpIdx) {
    const MachineRegisterInfo &MRI = MI.getMF()->getRegInfo();
    return ItinData->getForwardingClass(
        getSchedClass(MI.getDesc(), MI.operands(), MRI), OpIdx);
  };

  // FIXME: This assumes one cycle benefit for every pipeline forwarding.
  unsigned DefForwarding = GetForwardingClass(DefMI, DefIdx);
  unsigned UseForwarding = GetForwardingClass(UseMI, UseIdx);
  return (DefForwarding && DefForwarding == UseForwarding) ? 1 : 0;
}

std::pair<unsigned, unsigned>
AIE2InstrInfo::decomposeMachineOperandsTargetFlags(unsigned TF) const {
  const unsigned Mask = AIEII::MO_DIRECT_FLAG_MASK;
  return std::make_pair(TF & Mask, TF & ~Mask);
}

ArrayRef<std::pair<unsigned, const char *>>
AIE2InstrInfo::getSerializableDirectMachineOperandTargetFlags() const {
  using namespace AIEII;
  static const std::pair<unsigned, const char *> TargetFlags[] = {
      {MO_GLOBAL, "aie2-global"}};
  return ArrayRef(TargetFlags);
}

bool AIE2InstrInfo::canHoistCheapInst(const MachineInstr &MI) const {
  if (!AIEBaseInstrInfo::canHoistCheapInst(MI))
    return false;

  // Generally, we want to hoist cross-bank copies to decrease the pressure
  // on GPRs inside loops. This is mostly needed for reg classes into which it's
  // not directly possible to load or materialize constants. In AIE2, that's eS.
  const MachineRegisterInfo &MRI = MI.getMF()->getRegInfo();
  return MI.getOpcode() == TargetOpcode::COPY &&
         AIE2::eSRegClass.hasSubClassEq(
             MRI.getRegClass(MI.getOperand(0).getReg()));
}

std::optional<const AIEBaseInstrInfo::VConcatOpInfo>
AIE2InstrInfo::getVConcatOpInfo(const MachineInstr &MI) const {

  std::optional<const AIEBaseInstrInfo::VConcatOpInfo> BaseOpInfo =
      AIEBaseInstrInfo::getVConcatOpInfo(MI);

  if (BaseOpInfo)
    return BaseOpInfo;

  if (MI.getOpcode() != TargetOpcode::G_INTRINSIC)
    return std::nullopt;

  const GIntrinsic &GMI = cast<const GIntrinsic>(MI);

  switch (GMI.getIntrinsicID()) {
  case Intrinsic::aie2_concat_I512_I256:
  case Intrinsic::aie2_concat_I1024_I512:
  case Intrinsic::aie2_concat_I1024_I256:

  case Intrinsic::aie2_concat_bf512_bf256:
  case Intrinsic::aie2_concat_bf1024_bf512:
  case Intrinsic::aie2_concat_bf1024_bf256:

  case Intrinsic::aie2_concat_512_256_acc:
  case Intrinsic::aie2_concat_1024_512_acc:
  case Intrinsic::aie2_concat_1024_256_acc:
    return VConcatOpInfo{2, 1};
  default:
    return std::nullopt;
  }
}

std::optional<const AIEBaseInstrInfo::VUpdateOpInfo>
AIE2InstrInfo::getVUpdateOpInfo(const MachineInstr &MI) const {

  if (MI.getOpcode() != TargetOpcode::G_INTRINSIC)
    return std::nullopt;

  const GIntrinsic &GMI = cast<const GIntrinsic>(MI);

  switch (GMI.getIntrinsicID()) {
  case Intrinsic::aie2_upd_I512_I256:
  case Intrinsic::aie2_upd_I1024_I512:
  case Intrinsic::aie2_upd_I1024_I256:

  case Intrinsic::aie2_upd_bf512_bf256:
  case Intrinsic::aie2_upd_bf1024_bf512:
  case Intrinsic::aie2_upd_bf1024_bf256:

  case Intrinsic::aie2_upd_512_256_acc:
  case Intrinsic::aie2_upd_1024_512_acc:
  case Intrinsic::aie2_upd_1024_256_acc:
    return VUpdateOpInfo{2, 3, 4};
  default:
    return std::nullopt;
  }
}

std::optional<const AIEBaseInstrInfo::VExtractOpInfo>
AIE2InstrInfo::getVExtractOpInfo(const MachineInstr &MI) const {

  if (MI.getOpcode() != TargetOpcode::G_INTRINSIC)
    return std::nullopt;

  const GIntrinsic &GMI = cast<const GIntrinsic>(MI);

  switch (GMI.getIntrinsicID()) {
  case Intrinsic::aie2_ext_I256_I512:
  case Intrinsic::aie2_ext_I512_I1024:
  case Intrinsic::aie2_ext_I256_I1024:

  case Intrinsic::aie2_ext_bf256_bf512:
  case Intrinsic::aie2_ext_bf512_bf1024:
  case Intrinsic::aie2_ext_bf256_bf1024:

  case Intrinsic::aie2_ext_256_512_acc:
  case Intrinsic::aie2_ext_512_1024_acc:
  case Intrinsic::aie2_ext_256_1024_acc:
    return VExtractOpInfo{2, 3};
  default:
    return std::nullopt;
  }
}

unsigned AIE2InstrInfo::getMaxLoadStoreSize() const { return 256; }

bool AIE2InstrInfo::canCombineWithLoadStore(const MachineInstr &MI) const {

  if (!isa<GIntrinsic>(MI))
    return false;

  const unsigned ID = cast<GIntrinsic>(MI).getIntrinsicID();

  switch (ID) {
  case Intrinsic::aie2_I256_v16_acc32_srs:
  case Intrinsic::aie2_I256_v16_acc64_srs:
  case Intrinsic::aie2_I256_v32_acc32_srs:
  case Intrinsic::aie2_I256_v8_acc64_srs:
  case Intrinsic::aie2_I512_v16_acc64_srs:
  case Intrinsic::aie2_I512_v32_acc32_srs:

  case Intrinsic::aie2_acc32_v16_I256_ups:
  case Intrinsic::aie2_acc32_v32_I256_ups:
  case Intrinsic::aie2_acc32_v32_I512_ups:
  case Intrinsic::aie2_acc64_v16_I256_ups:
  case Intrinsic::aie2_acc64_v16_I512_ups:
  case Intrinsic::aie2_acc64_v8_I256_ups:
    return true;
  }
  return false;
}

bool AIE2InstrInfo::isProfitableToSplitType(const LLT Ty) const {
  const LLT V16S32 = LLT::fixed_vector(16, 32);
  const LLT V32S16 = LLT::fixed_vector(32, 16);
  const LLT V64S8 = LLT::fixed_vector(64, 8);

  if (Ty == V16S32 || Ty == V32S16 || Ty == V64S8)
    return true;

  return false;
}

using AbstractOp = AIEBaseInstrInfo::AbstractOp;

std::optional<const AbstractOp>
AIE2InstrInfo::parseAbstractOp(const MachineInstr &MI) const {

  switch (MI.getOpcode()) {
  case AIE2::VADD_32:
    return AbstractOp{AbstractOp::OperationType::VECTOR_ADD,
                      {MI.getOperand(1).getReg(), MI.getOperand(2).getReg()},
                      {}};
  case AIE2::VBCST_32:
    return AbstractOp{AbstractOp::OperationType::VECTOR_BROADCAST,
                      {},
                      {MI.getOperand(1).getReg()}};
  case AIE2::VSEL_32:
    return AbstractOp{AbstractOp::OperationType::VECTOR_SELECT,
                      {MI.getOperand(1).getReg(), MI.getOperand(2).getReg()},
                      {MI.getOperand(3).getReg()}};
  case AIE2::VLDB_4x16_LO:
  case AIE2::VLDB_4x16_HI:
  case AIE2::VLDB_4x32_LO:
  case AIE2::VLDB_4x32_HI:
  case AIE2::VLDB_4x64_LO:
  case AIE2::VLDB_4x64_HI:
    return AbstractOp{AbstractOp::OperationType::VECTOR_XWAY_LOAD,
                      {MI.getOperand(1).getReg()},
                      {}};
  }
  return std::nullopt;
}
