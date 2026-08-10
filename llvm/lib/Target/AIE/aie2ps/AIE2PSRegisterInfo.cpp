//===---- AIE2PSRegisterInfo.cpp - AIE2ps Register Information-*---- C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIE2ps implementation of the TargetRegisterInfo
// class.
//
//===----------------------------------------------------------------------===//

#include "AIE2PSRegisterInfo.h"
#include "AIE2PSRegisterBankInfo.h"
#include "AIE2PSSubtarget.h"
#include "MCTargetDesc/aie2ps/AIE2PSMCTargetDesc.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/Support/ErrorHandling.h"

#define GET_REGINFO_TARGET_DESC
#include "AIE2PSGenRegisterInfo.inc"

using namespace llvm;

#define DEBUG_TYPE "aie-reg-info"

extern cl::opt<bool> SimplifyCRSRRegs;
extern llvm::cl::opt<unsigned> ReservedGPRs;

AIE2PSRegisterInfo::AIE2PSRegisterInfo(unsigned HwMode)
    : AIE2PSGenRegisterInfo(AIE2PS::sp, /*DwarfFlavour*/ 0, /*EHFlavor*/ 0,
                            /*PC*/ 0, HwMode) {}
const MCPhysReg *
AIE2PSRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_AIE2PS_SaveList;
}

// TODO: Add other remaining reserved registers.
BitVector AIE2PSRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  const TargetFrameLowering *TFI = getFrameLowering(MF);
  BitVector Reserved(getNumRegs());

  // Won't ever appear, but reserved.
  markSuperRegs(Reserved, AIE2PS::pc);
  // SP is only accessible by special instructions.  Without this the
  // instruction verifier complains because SP is always implicitly defined
  // and never killed by the instruction allocator.
  if (TFI->hasFP(MF))
    markSuperRegs(Reserved, AIE2PS::p7);
  markSuperRegs(Reserved, AIE2PS::sp);

  // LR is also always implicitly defined.
  markSuperRegs(Reserved, AIE2PS::lr);

  // Reserve control and status registers
  for (auto &Reg : AIE2PS::mSCmRegClass) {
    markSuperRegs(Reserved, Reg);
  }
  for (auto &Reg : AIE2PS::mCRm_fileRegClass) {
    markSuperRegs(Reserved, Reg);
  }

  // Reserve pe2_ads register which is used implicitly by part-word store
  // instructions to hold the store address between the read and
  // the post-modify writeback.
  markSuperRegs(Reserved, AIE2PS::pe2_ads);

  // If requested, reserve GPRs to artificially increase the register pressure.
  // We reserve them "from the end" because the first GPRs are typically used
  // by the calling convention.
  unsigned CurrentReservedGPRs = 0;
  for (auto &Reg : reverse(AIE2PS::eRRegClass)) {
    if (CurrentReservedGPRs == ReservedGPRs)
      break;
    ++CurrentReservedGPRs;
    markSuperRegs(Reserved, Reg);
  }

  // CORE_ID is reserved.
  markSuperRegs(Reserved, AIE2PS::CORE_ID);

  // Mark the hardware loop related register as reserved,
  // otherwise they are considered dead
  markSuperRegs(Reserved, AIE2PS::lc);
  markSuperRegs(Reserved, AIE2PS::ls);
  markSuperRegs(Reserved, AIE2PS::le);
  assert(checkAllSuperRegsMarked(Reserved));
  return Reserved;
}

const uint32_t *AIE2PSRegisterInfo::getNoPreservedMask() const {
  return CSR_NoRegs_RegMask;
}

// Fully expand a spill/reload pseudo (\p SubMI) that has already had its stack
// offset materialized as an immediate, turning it into native instructions.
// If \p Encodable is false, the offset is too large to be encoded directly, so
// the stack pointer is first moved into a scratch register and indexed
// addressing is used (mirrors the standalone scalar-spill handling).
static void emitAndExpandSubSpill(MachineBasicBlock &MBB, const DebugLoc &DL,
                                  const AIE2PSInstrInfo *TII,
                                  const TargetRegisterInfo &TRI,
                                  MachineInstr *SubMI, int64_t SubOffset,
                                  bool Encodable, Register SPRegPhys) {
  if (Encodable) {
    TII->expandSpillPseudo(*SubMI, TRI, /*SubRegOffsetAlign=*/Align(4));
  } else {
    MachineFunction &MF = *MBB.getParent();
    Register SPReg =
        MF.getRegInfo().createVirtualRegister(&AIE2PS::eP_as_32BitRegClass);
    BuildMI(MBB, *SubMI, DL, TII->get(TII->getMvSclOpcode()), SPReg)
        .addReg(SPRegPhys);
    TII->expandSpillPseudo(*SubMI, TRI, /*SubRegOffsetAlign=*/Align(4), SPReg,
                           SubOffset);
  }
}

bool AIE2PSRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                             int SPAdj, unsigned FIOperandNum,
                                             RegScavenger *RS) const {

  assert(SPAdj == 0 && "Unexpected non-zero SPAdj value");
  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getMF();
  const AIE2PSInstrInfo *TII =
      MF.getSubtarget<AIE2PSSubtarget>().getInstrInfo();
  const AIE2PSFrameLowering *TFI = getFrameLowering(MF);
  DebugLoc DL = MI.getDebugLoc();

  MachineBasicBlock &MBB = *MI.getParent();
  MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
  const TargetRegisterInfo &TRI = *MRI.getTargetRegisterInfo();
  // Assume that we have a frame index operand, followed by an immediate offset.
  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();

  Register FrameReg;

  // Select the base pointer (BP) and calculate the actual offset from BP
  // to the beginning of the object at index FI.
  int Offset = TFI->getFrameIndexReference(MF, FrameIndex, FrameReg).getFixed();
  int ObjSize = MF.getFrameInfo().getObjectSize(FrameIndex);
  int ObjectOffset = MF.getFrameInfo().getObjectOffset(FrameIndex);
  int StackSize = MF.getFrameInfo().getStackSize();
  int OffsetAdjustment = MF.getFrameInfo().getOffsetAdjustment();
  int LocalFrameSize = MF.getFrameInfo().getLocalFrameSize();

  LLVM_DEBUG(dbgs() << "eliminateFrameIndex in Function : " << MF.getName()
                    << "\n");
  LLVM_DEBUG(dbgs() << "FrameInfo:\n");
  LLVM_DEBUG(MF.getFrameInfo().print(MF, dbgs()));

  LLVM_DEBUG(dbgs() << "Parent Instruction : " << MI << "\n");
  LLVM_DEBUG(dbgs() << "FrameIndex         : " << FrameIndex << "\n");
  LLVM_DEBUG(dbgs() << "ObjSize            : " << ObjSize << "\n");
  LLVM_DEBUG(dbgs() << "FrameOffset        : " << Offset << "\n");
  LLVM_DEBUG(dbgs() << "ObjectOffset       : " << ObjectOffset << "\n");
  LLVM_DEBUG(dbgs() << "OffsetAdj          : " << OffsetAdjustment << "\n");
  LLVM_DEBUG(dbgs() << "StackSize          : " << StackSize << "\n");
  LLVM_DEBUG(dbgs() << "LocalFrameSize     : " << LocalFrameSize << "\n");
  unsigned Opc = MI.getOpcode();
  switch (Opc) {
    // Instructions named *_spill implicitly a stack spill instruction, so
    // simply replace the TargetFrameIndex operand with the right immediate
    // offset. However, bigger offsets cannot be encodable. In this scenario, we
    // need to use offsets using registers.

    // NOTE: Although the register scavenger can often find a spare register, an
    // emergency spill slot might be needed to guarantee success. AIE reserves
    // an emergency slot in `processFunctionBeforeFrameFinalized`

    // Note that LDB path does not support SPILL instructions

  case AIE2PS::LDA_dml_lda2_scalar_EE_spill:
  case AIE2PS::LDA_dml_lda2_scalar_EG_spill:
  case AIE2PS::LDA_dml_lda2_scalar_GG_spill:
  case AIE2PS::LDA_dml_lda_scalar_F_spill:
  case AIE2PS::LDA_dml_lda_scalar_L_spill:
  case AIE2PS::LDA_dms_lda_g_spill:
  case AIE2PS::LDA_dms_lda_scalar_spill:
  case AIE2PS::LDA_dmv_lda_eg2_spill:
  case AIE2PS::LDA_dmv_lda_f_spill:
  case AIE2PS::LDA_dmv_lda_feg_spill:
  case AIE2PS::VLDA_128_dmv_lda_w_spill:
  case AIE2PS::VLDA_dmw_lda_feg2_spill:
  case AIE2PS::VLDA_dmw_lda_w_spill:
  case AIE2PS::VLDA_dmx_lda_bm_spill:
  case AIE2PS::VLDA_dmx_lda_x_spill:
  case AIE2PS::ST_dml_sts_scalar_spill:
  case AIE2PS::ST_dms_sts_g_spill:
  case AIE2PS::ST_dms_sts_scalar_spill:
  case AIE2PS::ST_dmv_sts_f_spill:
  case AIE2PS::VST_128_dmv_sts_eg_spill:
  case AIE2PS::VST_128_dmv_sts_feg_spill:
  case AIE2PS::VST_128_dmv_sts_w_spill:
  case AIE2PS::VST_dmw_sts_feg2_spill:
  case AIE2PS::VST_dmw_sts_w_spill:
  case AIE2PS::VST_dmx_sts_bm_spill:
  case AIE2PS::VST_dmx_sts_x_spill:
    MI.getOperand(FIOperandNum).ChangeToImmediate(Offset);
    return false;
  case AIE2PS::LDA_R_SPILL:
  case AIE2PS::ST_R_SPILL:
  case AIE2PS::LDA_D_SPILL:
  case AIE2PS::ST_D_SPILL:
  case AIE2PS::LDA_DS_SPILL:
  case AIE2PS::ST_DS_SPILL:
  case AIE2PS::VLDA_E_SPILL:
  case AIE2PS::VST_E_SPILL:
  case AIE2PS::VLDA_G_SPILL:
  case AIE2PS::VST_G_SPILL:
  case AIE2PS::VLDA_GG_SPILL:
  case AIE2PS::VST_GG_SPILL:
  case AIE2PS::VLDA_EW_SPILL:
  case AIE2PS::VST_EW_SPILL:
  case AIE2PS::VLDA_FEW_SPILL:
  case AIE2PS::VST_FEW_SPILL: {
    // When a pseudo instruction expands to multiple instructions, this case
    // looks for the smallest encodable offset that can be used.
    // The stack grows upward so if Offset is in range, the offsets of its
    // sub-register spills should also be fine.
    if (isEncodableAsNegativeInt<9, 4>(Offset)) {
      MI.getOperand(FIOperandNum).ChangeToImmediate(Offset);
      TII->expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(4));
    } else {
      Register SPReg =
          MF.getRegInfo().createVirtualRegister(&AIE2PS::eP_as_32BitRegClass);
      BuildMI(MBB, II, DL, TII->get(TII->getMvSclOpcode()), SPReg)
          .addReg(getStackPointerRegister());
      TII->expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(4), SPReg,
                             Offset);
    }
    return true;
  }
  case AIE2PS::LDA_L_SPILL:
  case AIE2PS::ST_L_SPILL:
  case AIE2PS::VLDA_EE_SPILL:
  case AIE2PS::VST_EE_SPILL:
  case AIE2PS::VLDA_F_SPILL:
  case AIE2PS::VST_F_SPILL:
  case AIE2PS::VLDA_EG_SPILL:
  case AIE2PS::VST_EG_SPILL:
  case AIE2PS::VLDA_EX_SPILL:
  case AIE2PS::VST_EX_SPILL:
  case AIE2PS::VLDA_FEX_SPILL:
  case AIE2PS::VST_FEX_SPILL:
  case AIE2PS::VLDA_EY_SPILL:
  case AIE2PS::VST_EY_SPILL:
  case AIE2PS::VLDA_FEY_SPILL:
  case AIE2PS::VST_FEY_SPILL: {
    if (isEncodableAsNegativeInt<9, 8>(Offset)) {
      MI.getOperand(FIOperandNum).ChangeToImmediate(Offset);
      TII->expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(4));
    } else {
      Register SPReg =
          MF.getRegInfo().createVirtualRegister(&AIE2PS::eP_as_32BitRegClass);
      BuildMI(MBB, II, DL, TII->get(TII->getMvSclOpcode()), SPReg)
          .addReg(getStackPointerRegister());
      TII->expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(4), SPReg,
                             Offset);
    }
    return true;
  }
  case AIE2PS::VLDA_V_SPILL:
  case AIE2PS::VST_V_SPILL:
  case AIE2PS::VLDA_FF_SPILL:
  case AIE2PS::VST_FF_SPILL:
  case AIE2PS::VLDA_EG2_SPILL:
  case AIE2PS::VST_EG2_SPILL:
  case AIE2PS::VLDA_FEG_SPILL:
  case AIE2PS::VST_FEG_SPILL: {
    if (isEncodableAsNegativeInt<9, 16>(Offset)) {
      MI.getOperand(FIOperandNum).ChangeToImmediate(Offset);
      TII->expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(4));
    } else {
      Register SPReg =
          MF.getRegInfo().createVirtualRegister(&AIE2PS::eP_as_32BitRegClass);
      BuildMI(MBB, II, DL, TII->get(TII->getMvSclOpcode()), SPReg)
          .addReg(getStackPointerRegister());
      TII->expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(4), SPReg,
                             Offset);
    }
    return true;
  }
  case AIE2PS::VLDA_W_SPILL:
  case AIE2PS::VST_W_SPILL:
  case AIE2PS::VLDA_FEG2_SPILL:
  case AIE2PS::VST_FEG2_SPILL: {
    if (isEncodableAsNegativeInt<9, 32>(Offset)) {
      MI.getOperand(FIOperandNum).ChangeToImmediate(Offset);
      TII->expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(4));
    } else {
      Register SPReg =
          MF.getRegInfo().createVirtualRegister(&AIE2PS::eP_as_32BitRegClass);
      BuildMI(MBB, II, DL, TII->get(TII->getMvSclOpcode()), SPReg)
          .addReg(getStackPointerRegister());
      TII->expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(4), SPReg,
                             Offset);
    }
    return true;
  }
  case AIE2PS::VLDA_X_SPILL:
  case AIE2PS::VST_X_SPILL:
  case AIE2PS::VLDA_BM_SPILL:
  case AIE2PS::VST_BM_SPILL: {
    if (isEncodableAsNegativeInt<9, 64>(Offset)) {
      MI.getOperand(FIOperandNum).ChangeToImmediate(Offset);
      TII->expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(4));
    } else {
      Register SPReg =
          MF.getRegInfo().createVirtualRegister(&AIE2PS::eP_as_32BitRegClass);
      BuildMI(MBB, II, DL, TII->get(TII->getMvSclOpcode()), SPReg)
          .addReg(getStackPointerRegister());
      TII->expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(4), SPReg,
                             Offset);
    }
    return true;
  }
  case AIE2PS::VLDA_Y_SPILL:
  case AIE2PS::VST_Y_SPILL:
  case AIE2PS::VLDA_CM_SPILL:
  case AIE2PS::VST_CM_SPILL:
  case AIE2PS::VLDA_DM_SPILL:
  case AIE2PS::VST_DM_SPILL: {
    // These expand into the native dmx x/bm stores/loads, whose SP offset field
    // is c16n_step64. The encodable range is therefore the step-64 range of the
    // expanded access, not the 128-byte logical size of the composite register
    // (using step 128 would wrongly accept offsets in [-65536, -32768) and emit
    // an unencodable immediate on the expanded x/bm spill).
    if (isEncodableAsNegativeInt<9, 64>(Offset)) {
      MI.getOperand(FIOperandNum).ChangeToImmediate(Offset);
      TII->expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(4));
    } else {
      Register SPReg =
          MF.getRegInfo().createVirtualRegister(&AIE2PS::eP_as_32BitRegClass);
      BuildMI(MBB, II, DL, TII->get(TII->getMvSclOpcode()), SPReg)
          .addReg(getStackPointerRegister());
      TII->expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(4), SPReg,
                             Offset);
    }
    return true;
  }
  case AIE2PS::VST_PLFR_SPILL:
  case AIE2PS::VLDA_PLFR_SPILL: {
    // The ePSRFLdF composite is laid out in the spill slot as:
    //   [sub_fifo (bounced via VEC1024)] [sub_avail (32b)] [sub_ptr (32b)]
    // sub_fifo uses the Y (step-128) path and the two scalar halves the
    // scalar (R) path. Each sub-spill is emitted at its own stack offset and
    // then fully expanded into native instructions.
    //
    // Operand layouts:
    //   VST_PLFR_SPILL  $fifo(use), $avail(use), $ptr(use), <FI>
    //   VLDA_PLFR_SPILL $fifo(def, VEC1024 scratch), $plfr(full def), <FI>
    // For the store the FIFO half has already been bounced into the VEC1024
    // operand. For the reload the scalar halves are sub-registers of the
    // composite $plfr def, and the FIFO half is loaded into the VEC1024 scratch
    // and then bounced into $plfr's sub_fifo with a copy.
    const bool IsStore = (Opc == AIE2PS::VST_PLFR_SPILL);
    const int FifoSize = 128;
    const int ScalarSize = 4;
    const Register SPPhys = getStackPointerRegister();
    const MachineMemOperand *OrigMMO =
        MI.memoperands_empty() ? nullptr : *MI.memoperands_begin();

    const Register FifoBounce = MI.getOperand(0).getReg();
    Register FifoPhys, AvailReg, PtrReg;
    if (IsStore) {
      AvailReg = MI.getOperand(1).getReg();
      PtrReg = MI.getOperand(2).getReg();
    } else {
      const Register Plfr = MI.getOperand(1).getReg();
      FifoPhys = TRI.getSubReg(Plfr, AIE2PS::sub_fifo);
      AvailReg = TRI.getSubReg(Plfr, AIE2PS::sub_avail);
      PtrReg = TRI.getSubReg(Plfr, AIE2PS::sub_ptr);
    }

    auto SubMMO = [&](int ByteOff, int Size) -> MachineMemOperand * {
      return OrigMMO ? MF.getMachineMemOperand(OrigMMO, ByteOff, Size)
                     : nullptr;
    };

    struct PendingSub {
      MachineInstr *MI;
      int64_t Offset;
      bool Encodable;
    };
    SmallVector<PendingSub, 3> PendingSubs;
    auto BuildSub = [&](unsigned Opcode, Register Reg, int ByteOff, int Size,
                        bool Encodable) {
      const int64_t SubOff = Offset + ByteOff;
      MachineInstrBuilder MIB = BuildMI(MBB, II, DL, TII->get(Opcode));
      if (IsStore)
        MIB.addReg(Reg);
      else
        MIB.addReg(Reg, RegState::Define);
      MIB.addImm(SubOff);
      if (MachineMemOperand *MMO = SubMMO(ByteOff, Size))
        MIB.addMemOperand(MMO);
      PendingSubs.push_back({MIB.getInstr(), SubOff, Encodable});
    };

    const unsigned FifoOpc =
        IsStore ? AIE2PS::VST_Y_SPILL : AIE2PS::VLDA_Y_SPILL;
    const unsigned ScalarOpc =
        IsStore ? AIE2PS::ST_R_SPILL : AIE2PS::LDA_R_SPILL;

    // The FIFO half is bounced through a VEC1024 and spilled with VST_Y_SPILL /
    // VLDA_Y_SPILL, which expand into the native dmx x stores/loads. Those
    // natives encode the SP offset as c16n_step64, so the encodable range is
    // determined by the step-64 access (not the 128-byte logical size of the
    // composite), matching the standalone VEC512 (X) spill check.
    BuildSub(FifoOpc, FifoBounce, /*ByteOff=*/0, FifoSize,
             isEncodableAsNegativeInt<9, 64>(Offset));
    BuildSub(ScalarOpc, AvailReg, /*ByteOff=*/FifoSize, ScalarSize,
             isEncodableAsNegativeInt<9, 4>(Offset + FifoSize));
    BuildSub(ScalarOpc, PtrReg, /*ByteOff=*/FifoSize + ScalarSize, ScalarSize,
             isEncodableAsNegativeInt<9, 4>(Offset + FifoSize + ScalarSize));

    // For the reload, bounce the loaded FIFO data from the VEC1024 scratch into
    // the composite's sub_fifo physical register.
    if (!IsStore)
      BuildMI(MBB, II, DL, TII->get(AIE2PS::COPY), FifoPhys)
          .addReg(FifoBounce, getKillRegState(true));

    MI.eraseFromParent();
    for (const PendingSub &Sub : PendingSubs)
      emitAndExpandSubSpill(MBB, DL, TII, TRI, Sub.MI, Sub.Offset,
                            Sub.Encodable, SPPhys);
    return true;
  }
  case AIE2PS::PseudoFI: {
    // DstReg = FI;
    // Replace with DstReg = FrameReg, DstReg += Offset;

    Register DstReg = II->getOperand(0).getReg();
    BuildMI(MBB, II, DL, TII->get(AIE2PS::MOV_alu_mv_mv_mv_scl), DstReg)
        .addReg(FrameReg);

    // Offset can only be a multiple of 64 for PADD imm.
    // Check for 10 bits (in which 6 LSBs are always zeros)
    // for PADD pseudo to expand to PADD A or B.
    if ((Offset % 64 == 0) && isInt<4 + 6>(Offset)) {
      BuildMI(MBB, II, DL, TII->get(AIE2PS::PADD_imm_pseudo), DstReg)
          .addReg(DstReg)
          .addImm(Offset);
    } else {
      Register ScratchReg =
          MF.getRegInfo().createVirtualRegister(&AIE2PS::eMRegClass);
      if (isIntN(11, Offset)) {
        BuildMI(MBB, II, DL, TII->get(AIE2PS::MOV_PD_imm11_pseudo), ScratchReg)
            .addImm(Offset);
      } else {
        BuildMI(MBB, II, DL, TII->get(AIE2PS::MOVXM_lng_cg), ScratchReg)
            .addImm(Offset);
      }
      BuildMI(MBB, II, DL, TII->get(AIE2PS::PADD_mod_pseudo), DstReg)
          .addReg(DstReg)
          .addReg(ScratchReg);
    }
    II->eraseFromParent();
    return true;
  }
  default:
    break;
  }
  llvm_unreachable("Un-implemented");
}

Register AIE2PSRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  const AIE2PSFrameLowering *TFI = getFrameLowering(MF);
  // There is no explicit frame pointer register.
  // Note that this needs to remain consistent with code in AIEFrameLowering.
  return TFI->hasFP(MF) ? AIE2PS::p7 : AIE2PS::sp;
}

Register AIE2PSRegisterInfo::getStackPointerRegister() const {
  return AIE2PS::sp;
}

const TargetRegisterClass *
AIE2PSRegisterInfo::getLargestLegalSuperClass(const TargetRegisterClass *RC,
                                              const MachineFunction &MF) const {
  // TODO: This implimentation is not complete
  if (AIE2PS::eRRegClass.hasSubClassEq(RC))
    return &AIE2PS::eRRegClass;

  if (AIE2PS::ePRegClass.hasSubClassEq(RC))
    return &AIE2PS::spill_eP_to_eRRegClass;
  if (AIE2PS::eMRegClass.hasSubClassEq(RC))
    return &AIE2PS::spill_eM_to_eRRegClass;
  if (AIE2PS::eDNRegClass.hasSubClassEq(RC))
    return &AIE2PS::spill_eDN_to_eRRegClass;
  if (AIE2PS::eDJRegClass.hasSubClassEq(RC))
    return &AIE2PS::spill_eDJ_to_eRRegClass;
  if (AIE2PS::eDCRegClass.hasSubClassEq(RC))
    return &AIE2PS::spill_eDC_to_eRRegClass;
  if (AIE2PS::eSRegClass.hasSubClassEq(RC))
    return &AIE2PS::spill_eS_to_eRRegClass;

  return RC;
}

const std::set<int> &AIE2PSRegisterInfo::getSubRegSplit(int RegClassId) const {
  static const std::set<int> NoSplit = {AIE2PS::NoSubRegister};
  static const std::set<int> Mod2DSplit = {
      AIE2PS::sub_mod, AIE2PS::sub_dim_size, AIE2PS::sub_dim_stride,
      AIE2PS::sub_dim_count};
  static const std::set<int> Mod3DSplit = {
      AIE2PS::sub_mod,
      AIE2PS::sub_dim_size,
      AIE2PS::sub_dim_stride,
      AIE2PS::sub_dim_count,
      AIE2PS::sub_hi_dim_then_sub_mod,
      AIE2PS::sub_hi_dim_then_sub_dim_size,
      AIE2PS::sub_hi_dim_then_sub_dim_stride,
      AIE2PS::sub_hi_dim_then_sub_dim_count};
  switch (RegClassId) {
  case AIE2PS::eDRegClassID:
    return Mod2DSplit;
  case AIE2PS::eDSRegClassID:
    return Mod3DSplit;
  }
  return NoSplit;
}

const TargetRegisterClass &
AIE2PSRegisterInfo::getMinClassForRegBank(const RegisterBank &RB,
                                          LLT Ty) const {
  switch (RB.getID()) {
  case AIE2PS::GPRRegBankID:
    return Ty.getSizeInBits() <= 32 ? AIE2PS::eRRegClass : AIE2PS::eLRegClass;
  case AIE2PS::PTRRegBankID:
    return AIE2PS::ePRegClass;
  case AIE2PS::MODRegBankID:
    return AIE2PS::mDmRegClass;
  case AIE2PS::VRegBankID: {
    switch (Ty.getSizeInBits()) {
    case 128:
      return AIE2PS::VEC128RegClass;
    case 256:
      return AIE2PS::VEC256RegClass;
    case 512:
      return AIE2PS::VEC512RegClass;
    case 1024:
      return AIE2PS::VEC1024RegClass;
    default:
      llvm_unreachable("Unsupported register size.");
    }
  }
  case AIE2PS::AccRegBankID: {
    switch (Ty.getSizeInBits()) {
    case 512:
      return AIE2PS::ACC512RegClass;
    case 1024:
      return AIE2PS::ACC1024RegClass;
    case 2048:
      return AIE2PS::ACC2048RegClass;
    default:
      llvm_unreachable("Unsupported register size.");
    }
  }
  case AIE2PS::FifoRegBankID: {
    switch (Ty.getSizeInBits()) {
    case 512:
      return AIE2PS::FIFO512RegClass;
    case 1024:
      return AIE2PS::FIFO1024RegClass;
    default:
      llvm_unreachable("Unsupported register size.");
    }
  }
  }
  llvm_unreachable("Unexpected register bank.");
}

const TargetRegisterClass *AIE2PSRegisterInfo::getConstrainedRegClassForOperand(
    const MachineOperand &MO, const MachineRegisterInfo &MRI) const {
  // AIE has classes with mixed types, which aren't properly handled in llvm.
  // Here, we go back to safety by using the RegisterBank assignment to pick
  // a valid subclass.
  if (const RegisterBank *RB = MRI.getRegBankOrNull(MO.getReg())) {
    // Note that after selecting the target instruction, the class below will
    // typically be constrained further by constrainSelectedInstRegOperands(),
    // which eventually calls MachineRegisterInfo::constrainRegClass().
    // The latter will pick the largest class which covers both the currently
    // assigned RC, and the RC below.
    //
    // E.g.     G_STORE %1:ep(p0), %0:ptrregbank(p0)
    // becomes  G_STORE %1:ep_as_32bit(p0), %0:ptrregbank(p0)
    // because  ep_as_32bit is the largest common subclass of ep and mSclSt_aie.
    return &getMinClassForRegBank(*RB, MRI.getType(MO.getReg()));
  }

  return nullptr;
}

bool AIE2PSRegisterInfo::isTypeLegalForClass(const TargetRegisterClass &RC,
                                             LLT T) const {
  // FIXME pointer registers are same as aie2, anything else?
  //  Note: With the current TableGen backend, isTypeLegalForClass will never
  //  return true when LLT is a LLT::pointer type. In most cases, that is fine,
  //  because the MachineVerifier will use getMinimalPhysRegClass() as a
  //  fallback mechanism to derive type information (e.g. the register size).
  //
  //  However, this function will be needed as long as there will be classes
  //  with both GPR and PTR types. When that happens, TableGen can create
  //  subclasses with the pointer registers contained in the mixed classes. The
  //  problem is that those classes will have the properties of the mixed class,
  //  i.e. they will represent 32-bit registers. This means that if a pointer
  //  register is part of a mixed PTR/GPR class, its size will be considered as
  //  32 bits. (Thus making the Machine verifier shout.) One way around the
  //  problem is to specifically mention that LLT::pointer types are legal on
  //  pointer classes. That way, the fallback mechanism using
  //  getMinimalPhysRegClass() isn't used.
  //
  //  TLDR: for pointer registers which are part of mixed PTR/GPR classes, there
  //  needs to be specific handling here of the legal register classes. The same
  //  is also applicable for modifiers in compound register classes.

  // PTR/MOD registers obviously support pointers
  if (T == LLT::pointer(0, 20) && RC.getID() == AIE2PS::ePRegClassID)
    return true;
  if (T == LLT::pointer(0, 20) && RC.getID() == AIE2PS::mDmRegClassID)
    return true;
  if (T == LLT::pointer(0, 20) && RC.getID() == AIE2PS::eSpecial20RegClassID)
    return true;

  // Scalars of 20 bits fit in PTRs
  if (T == LLT::scalar(20)) {
    // 20-bit scalars cannot be allowed on 32-bit reg classes, otherwise llvm
    // will consider that a PTR reg copied as a scalar is 32 bits.
    // FIXME: We should not mix PTRs and GPRs in reg classes.
    if ((RC.getID() == AIE2PS::ePRegClassID) ||
        (RC.getID() == AIE2PS::mDmRegClassID))
      return true;
    return false;
  }

  return TargetRegisterInfo::isTypeLegalForClass(RC, T);
}

const uint32_t *
AIE2PSRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                         CallingConv::ID CC) const {
  switch (CC) {
  case CallingConv::AIE_PreserveAll_Vec:
    return CSR_AIE2PS_Vec_RegMask;
  default:
    return CSR_AIE2PS_RegMask;
  }
}

const TargetRegisterClass *
AIE2PSRegisterInfo::getGPRRegClass(const MachineFunction &MF) const {
  return &AIE2PS::eRRegClass;
}

unsigned AIE2PSRegisterInfo::getGPRRegBankID() const {
  return AIE2PS::GPRRegBankID;
}

unsigned AIE2PSRegisterInfo::getMODRegBankID() const {
  return AIE2PS::MODRegBankID;
}

unsigned AIE2PSRegisterInfo::getPTRRegBankID() const {
  return AIE2PS::PTRRegBankID;
}

bool AIE2PSRegisterInfo::isFifoPhysReg(const Register Reg) const {
  return Reg.isPhysical() && (AIE2PS::FIFO512RegClass.contains(Reg) ||
                              AIE2PS::FIFO1024RegClass.contains(Reg));
}

bool AIE2PSRegisterInfo::shouldCoalesce(
    MachineInstr *MI, const TargetRegisterClass *SrcRC, unsigned SubReg,
    const TargetRegisterClass *DstRC, unsigned DstSubReg,
    const TargetRegisterClass *NewRC, LiveIntervals &LIS) const {

  // The ePSRFLdF register class represents the complete FIFO load state as a
  // composite of three sub-registers with very different sizes:
  //   sub_ptr   (20-bit pointer, e.g. p0/p1)
  //   sub_avail (32-bit availability count, e.g. r24/r25)
  //   sub_fifo  (1024-bit FIFO buffer, e.g. lf0/lf1)
  //
  // When the coalescer eliminates a chain of copies tracing a pointer or count
  // value back to an early definition (e.g. an argument in the entry block),
  // it can promote the initialization of sub_ptr or sub_avail into that early
  // block.  Because the composite register is treated as a unit, this makes
  // the entire ePSRFLdF register — including the large sub_fifo — live from
  // that early definition point, even though the FIFO buffer is only needed
  // much later when the first VLD_FILL/VLD_POP instruction executes.
  //
  // The artificially extended live range forces the register allocator to hold
  // a physical FIFO load state register (plfr0 or plfr1) across code regions
  // where it is not yet needed, which can create pressure that causes
  // unnecessary spills of the 1024-bit FIFO buffer.
  //
  // Blocking coalescing into the small sub-registers (sub_ptr, sub_avail) of
  // ePSRFLdF keeps the composite register's live range starting at the site of
  // the first real FIFO operation, where multiple FIFO virtual registers in
  // disjoint control-flow regions can share the same two physical registers
  // (plfr0, plfr1) without interference and without any spilling.

  if (AIE2PS::ePSRFLdFRegClass.hasSubClassEq(NewRC) &&
      SubReg != AIE2PS::NoSubRegister && SubReg != AIE2PS::sub_fifo) {
    return false;
  }

  return AIEBaseRegisterInfo::shouldCoalesce(MI, SrcRC, SubReg, DstRC,
                                             DstSubReg, NewRC, LIS);
}

void AIE2PSRegisterInfo::getTargetSubRegs(std::vector<unsigned> &Subregs,
                                          unsigned Size,
                                          const RegisterBank &RB) const {
  bool IsVec =
      (RB.getID() == AIE2PS::VRegBankID || RB.getID() == AIE2PS::AccRegBankID);
  if (!IsVec) {
    // TODO: support other subreg cases
    assert(Size == 32 && "Unsupported subreg type for scalar!");
    Subregs.push_back(AIE2PS::sub_l_even);
    Subregs.push_back(AIE2PS::sub_l_odd);
  } else {
    const bool IsVecRB = RB.getID() == AIE2PS::VRegBankID;
    switch (Size) {
    case 256:
      assert(IsVecRB && "expected vector register bank for 256 dest type!");
      Subregs.push_back(AIE2PS::sub_256_lo);
      Subregs.push_back(AIE2PS::sub_256_hi);
      break;
    case 512:
      Subregs.push_back(IsVecRB ? AIE2PS::sub_512_lo : AIE2PS::sub_512_acc_lo);
      Subregs.push_back(IsVecRB ? AIE2PS::sub_512_hi : AIE2PS::sub_512_acc_hi);
      break;
    case 1024:
      assert(!IsVecRB &&
             "expected accumulator register bank for 256 dest type!");
      Subregs.push_back(AIE2PS::sub_1024_acc_lo);
      Subregs.push_back(AIE2PS::sub_1024_acc_hi);
      break;
    default:
      llvm_unreachable("Unsupported subreg type!");
    }
  }
}

bool AIE2PSRegisterInfo::isReservedStickyReg(MCRegister PhysReg) const {
  switch (PhysReg) {
  case AIE2PS::srSparse_of:
  case AIE2PS::srF2FFlags:
  case AIE2PS::srF2BFlags:
  case AIE2PS::srF2IFlags:
  case AIE2PS::srFPFlags:
  case AIE2PS::srSRS_of:
  case AIE2PS::srUPS_of:
  case AIE2PS::srFifo_of:
  case AIE2PS::srFifo_uf:
    return true;
  default:
    return false;
  }
}

bool AIE2PSRegisterInfo::isSimplifiableReservedReg(MCRegister PhysReg) const {
  return SimplifyCRSRRegs && (AIE2PS::mSCmRegClass.contains(PhysReg) ||
                              AIE2PS::mCRm_fileRegClass.contains(PhysReg));
}

bool AIE2PSRegisterInfo::isVecOrAccRegClass(
    const TargetRegisterClass &RC) const {
  // ******** Vector classes ********
  if (AIE2PS::VEC128RegClass.hasSubClassEq(&RC))
    return true;

  if (AIE2PS::VEC256RegClass.hasSubClassEq(&RC))
    return true;

  if (AIE2PS::VEC512RegClass.hasSubClassEq(&RC))
    return true;

  if (AIE2PS::VEC1024RegClass.hasSubClassEq(&RC))
    return true;

  // ******** BFP16 vectors ********
  if (AIE2PS::mEWmRegClass.hasSubClassEq(&RC)) // 320-bit
    return true;

  if (AIE2PS::mEXaRegClass.hasSubClassEq(&RC)) // 640-bit
    return true;

  if (AIE2PS::eEY_sRegClass.hasSubClassEq(&RC)) // 1280-bit
    return true;

  // ******** BFP13 vectors ********
  if (AIE2PS::mFEGaRegClass.hasSubClassEq(&RC)) // 128-bit
    return true;

  if (AIE2PS::mFEG2aRegClass.hasSubClassEq(&RC)) // 256-bit
    return true;

  if (AIE2PS::mFEWaRegClass.hasSubClassEq(&RC)) // 384-bit
    return true;

  if (AIE2PS::mFEXmRegClass.hasSubClassEq(&RC)) // 768-bit
    return true;

  // In practice only fey spill reloads reach this rung; see the fey test.
  if (AIE2PS::mFEYwRegClass.hasSubClassEq(&RC)) // 1536-bit
    return true;

  // ******** Accumulator classes (BM/CM/DM) ********
  if (AIE2PS::ACC512RegClass.hasSubClassEq(&RC))
    return true;

  if (AIE2PS::ACC1024RegClass.hasSubClassEq(&RC))
    return true;

  if (AIE2PS::ACC2048RegClass.hasSubClassEq(&RC))
    return true;

  return false;
}

Register AIE2PSRegisterInfo::getControlRegister(unsigned Idx) const {
  // Control Register Map. Keys based on encoding.
  // The map is based on the encoding in clang/lib/Headers/aie2ps/aie2ps_enums.h
  static std::unordered_map<unsigned, Register> ControlRegisterMap = {
      {0, AIE2PS::crSat},
      {1, AIE2PS::crRnd},
      {2, AIE2PS::crFPMask},
      {3, AIE2PS::crSCDEn},
      {4, AIE2PS::crMCDEn},
      {5, AIE2PS::crUnpackSize},
      {6, AIE2PS::crPackSize},
      {7, AIE2PS::crUPSMode},
      {8, AIE2PS::crSRSMode},
      {9, AIE2PS::crF2IMask},
      {10, AIE2PS::crF2FMask},
      {11, AIE2PS::vaddSign0},
      {12, AIE2PS::vaddSign1},
      {13, AIE2PS::unpackSign0},
      {14, AIE2PS::unpackSign1},
      {15, AIE2PS::packSign0},
      {16, AIE2PS::packSign1},
      {17, AIE2PS::upsSign0},
      {18, AIE2PS::upsSign1},
      {19, AIE2PS::srsSign0},
      {20, AIE2PS::srsSign1},
      {21, AIE2PS::crBF8conf},
      {22, AIE2PS::crFP8conf},
      {23, AIE2PS::crFPConvSat},
      {24, AIE2PS::crFPNlfMask},
      {25, AIE2PS::crFPCnvFx2FlMask},
      {26, AIE2PS::crFPCnvFl2FxMask},
      {27, AIE2PS::crF2BMask},
      {28, AIE2PS::srCarry},
      {29, AIE2PS::srSS0},
      {30, AIE2PS::srMS0},
      {31, AIE2PS::srSRS_of},
      {32, AIE2PS::srUPS_of},
      {33, AIE2PS::srSparse_of},
      {34, AIE2PS::srFifo_of},
      {35, AIE2PS::srFifo_uf},
      {36, AIE2PS::srFPFlags},
      {37, AIE2PS::srF2IFlags},
      {38, AIE2PS::srF2FFlags},
      {39, AIE2PS::srF2BFlags},
      {40, AIE2PS::srFPNlf},
      {41, AIE2PS::srFPCnvFx2Fl},
      {42, AIE2PS::srFPCnvFl2Fx}};

  if (ControlRegisterMap.find(Idx) != ControlRegisterMap.end())
    return ControlRegisterMap[Idx];
  llvm_unreachable("Unexpected key for control register.");
}

Register AIE2PSRegisterInfo::getUnpackSignCtrlReg() const {
  return AIE2PS::unpackSign0;
}

unsigned
AIE2PSRegisterInfo::matchControlRegisterBitwidth(Register CtrlReg,
                                                 unsigned SrcConstVal) const {
  // Modulo by width of control regs.  To constrain the max possible value in
  // the register according to register width.
  switch (CtrlReg) {
  case AIE2PS::crMCDEn:
  case AIE2PS::crSCDEn:
  case AIE2PS::crUnpackSize:
  case AIE2PS::crPackSize:
  case AIE2PS::crUPSMode:
  case AIE2PS::crSRSMode:
  case AIE2PS::vaddSign0:
  case AIE2PS::vaddSign1:
  case AIE2PS::unpackSign0:
  case AIE2PS::unpackSign1:
  case AIE2PS::packSign0:
  case AIE2PS::packSign1:
  case AIE2PS::upsSign0:
  case AIE2PS::upsSign1:
  case AIE2PS::srsSign0:
  case AIE2PS::srsSign1:
    return SrcConstVal % (1 << 1);
  case AIE2PS::crSat:
  case AIE2PS::crBF8conf:
  case AIE2PS::crFP8conf:
  case AIE2PS::crFPConvSat:
    return SrcConstVal % (1 << 2);
  case AIE2PS::crRnd:
    return SrcConstVal % (1 << 4);
  case AIE2PS::crFPMask:
  case AIE2PS::crF2IMask:
  case AIE2PS::crF2FMask:
  case AIE2PS::crF2BMask:
    return SrcConstVal % (1 << 5);
  case AIE2PS::crFPNlfMask:
  case AIE2PS::crFPCnvFx2FlMask:
  case AIE2PS::crFPCnvFl2FxMask:
    return SrcConstVal % (1 << 8);
  default:
    llvm_unreachable("Unknown control register.");
  }
}
