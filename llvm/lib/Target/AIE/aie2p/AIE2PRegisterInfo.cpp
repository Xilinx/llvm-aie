//===---- AIE2PRegisterInfo.cpp - AIE2p Register Information--*---- C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIE2p implementation of the TargetRegisterInfo
// class.
//
//===----------------------------------------------------------------------===//

#include "AIE2PRegisterInfo.h"
#include "AIE2PRegisterBankInfo.h"
#include "AIE2PSubtarget.h"
#include "MCTargetDesc/aie2p/AIE2PMCTargetDesc.h"
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
#include "AIE2PGenRegisterInfo.inc"

using namespace llvm;

#define DEBUG_TYPE "aie-reg-info"

extern cl::opt<bool> SimplifyCRSRRegs;

cl::opt<bool> EnableCoalescingForWideCopy(
    "aie-enable-widen-copy-coalescing",
    cl::desc("Enable register coalescing for widening Copy"), cl::init(false),
    cl::Hidden);

extern llvm::cl::opt<unsigned> ReservedGPRs;

AIE2PRegisterInfo::AIE2PRegisterInfo(unsigned HwMode)
    : AIE2PGenRegisterInfo(AIE2P::sp, /*DwarfFlavour*/ 0, /*EHFlavor*/ 0,
                           /*PC*/ 0, HwMode) {}

const MCPhysReg *
AIE2PRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_AIE2P_SaveList;
}

BitVector AIE2PRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  const TargetFrameLowering *TFI = getFrameLowering(MF);
  BitVector Reserved(getNumRegs());

  // Use markSuperRegs to ensure any register aliases are also reserved

  // SP is only accessible by special instructions.  Without this the
  // instruction verifier complains because SP is always implicitly defined
  // and never killed by the instruction allocator.
  if (TFI->hasFP(MF))
    markSuperRegs(Reserved, AIE2P::p7);
  markSuperRegs(Reserved, AIE2P::sp);

  // LR is also always implicitly defined.
  markSuperRegs(Reserved, AIE2P::lr);

  // Reserve control and status registers
  for (auto &Reg : AIE2P::mCRmRegClass) {
    markSuperRegs(Reserved, Reg);
  }
  for (auto &Reg : AIE2P::mSRmRegClass) {
    markSuperRegs(Reserved, Reg);
  }
  for (auto &Reg : AIE2P::mCRFPRegClass) {
    markSuperRegs(Reserved, Reg);
  }

  // Reserve implicit part-word store register pe2_ads
  markSuperRegs(Reserved, AIE2P::pe2_ads);

  // If requested, reserve GPRs to artificially increase the register pressure.
  // We reserve them "from the end" because the first GPRs are typically used
  // by the calling convention.
  unsigned CurrentReservedGPRs = 0;
  for (auto &Reg : reverse(AIE2P::eRRegClass)) {
    if (CurrentReservedGPRs == ReservedGPRs)
      break;
    ++CurrentReservedGPRs;
    markSuperRegs(Reserved, Reg);
  }

  // CORE_ID is reserved.
  markSuperRegs(Reserved, AIE2P::CORE_ID);

  // Mark the hardware loop related register as reserved,
  // otherwise they are considered dead
  markSuperRegs(Reserved, AIE2P::lc);
  markSuperRegs(Reserved, AIE2P::ls);
  markSuperRegs(Reserved, AIE2P::le);
  assert(checkAllSuperRegsMarked(Reserved));
  return Reserved;
}

bool AIE2PRegisterInfo::isSimplifiableReservedReg(MCRegister PhysReg) const {
  return SimplifyCRSRRegs && (AIE2P::mCRmRegClass.contains(PhysReg) ||
                              AIE2P::mSRmRegClass.contains(PhysReg) ||
                              AIE2P::mCRFPRegClass.contains(PhysReg));
}

const uint32_t *AIE2PRegisterInfo::getNoPreservedMask() const {
  return CSR_NoRegs_RegMask;
}

bool AIE2PRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                            int SPAdj, unsigned FIOperandNum,
                                            RegScavenger *RS) const {
  assert(SPAdj == 0 && "Unexpected non-zero SPAdj value");
  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getMF();
  const AIE2PInstrInfo *TII = MF.getSubtarget<AIE2PSubtarget>().getInstrInfo();
  const AIE2PFrameLowering *TFI = getFrameLowering(MF);
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

  case AIE2P::LDA_dms_lda_spill:
  case AIE2P::ST_dms_sts_spill:
  case AIE2P::LDA_dmv_lda_q_spill:
  case AIE2P::VLDA_128_dmv_lda_w_spill:
  case AIE2P::VLDA_dmw_lda_w_spill:
  case AIE2P::VLDA_dmx_lda_bm_spill:
  case AIE2P::VLDA_dmx_lda_fifohl_spill:
  case AIE2P::VLDA_dmx_lda_x_spill:
  case AIE2P::ST_dmv_sts_q_spill:
  case AIE2P::VST_128_dmv_sts_w_spill:
  case AIE2P::VST_dmw_sts_w_spill:
  case AIE2P::VST_dmx_sts_bm_spill:
  case AIE2P::VST_dmx_sts_fifohl_spill:
  case AIE2P::VST_dmx_sts_x_spill:
  case AIE2P::VLDA_512_COMPOSED_REG_SPILL:
  case AIE2P::VST_512_COMPOSED_REG_SPILL:
    MI.getOperand(FIOperandNum).ChangeToImmediate(Offset);
    return false;
  case AIE2P::LDA_R_SPILL:
  case AIE2P::ST_R_SPILL:
  case AIE2P::VLDA_L_SPILL:
  case AIE2P::VST_L_SPILL:
  case AIE2P::LDA_D_SPILL:
  case AIE2P::ST_D_SPILL:
  case AIE2P::LDA_DS_SPILL:
  case AIE2P::ST_DS_SPILL:
  case AIE2P::VST_EX_SPILL:
  case AIE2P::VST_E_SPILL:
  case AIE2P::VLDA_E_SPILL:
  case AIE2P::VLDA_EX_SPILL: {
    // When a pseudo instruction expands to multiple instructions, this case
    // looks for the smallest encodable offset that can be used.
    // The stack grows upward so if Offset is in range, the offsets of its
    // sub-register spills should also be fine.
    if (isEncodableAsNegativeInt<9, 4>(Offset)) {
      MI.getOperand(FIOperandNum).ChangeToImmediate(Offset);
      TII->expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(4));
    } else {
      Register SPReg =
          MF.getRegInfo().createVirtualRegister(&AIE2P::ePRegClass);
      BuildMI(MBB, II, DL, TII->get(TII->getMvSclOpcode()), SPReg)
          .addReg(getStackPointerRegister());
      TII->expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(4), SPReg,
                             Offset);
    }
    return true;
  }
  case AIE2P::VLDA_PLFR_SPILL:
  case AIE2P::VST_PLFR_SPILL: {
    // Slot layout: [sub_fifo 128B][sub_avail 4B][sub_ptr 4B]. sub_fifo moves
    // through the FIFO path, whose SP offset must be a multiple of 64, and
    // ePSRFLdF's spill alignment guarantees it -- so a misaligned offset means
    // a wrong frame object, and must reach the emitter, which rejects it. The
    // indexed form would encode cleanly and write to a truncated address.
    // <9,4> is sub_fifo's field, the tightest of the three.
    const bool Aligned = Offset % 64 == 0;
    assert(Aligned &&
           "ePSRFLdF composite spill needs a 64-byte aligned offset");
    if (!Aligned || isEncodableAsNegativeInt<9, 4>(Offset)) {
      MI.getOperand(FIOperandNum).ChangeToImmediate(Offset);
      TII->expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(4));
    } else {
      Register SPReg =
          MF.getRegInfo().createVirtualRegister(&AIE2P::ePRegClass);
      BuildMI(MBB, II, DL, TII->get(TII->getMvSclOpcode()), SPReg)
          .addReg(getStackPointerRegister());
      TII->expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(4), SPReg,
                             Offset);
    }
    return true;
  }
  case AIE2P::VST_DM_SPILL:
  case AIE2P::VST_CM_SPILL:
  case AIE2P::VST_FIFO_SPILL:
  case AIE2P::VST_Y_SPILL:
  case AIE2P::VLDA_DM_SPILL:
  case AIE2P::VLDA_CM_SPILL:
  case AIE2P::VLDA_FIFO_SPILL:
  case AIE2P::VLDA_Y_SPILL:
    MI.getOperand(FIOperandNum).ChangeToImmediate(Offset);
    TII->expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(4));
    return true;
  case AIE2P::PseudoFI: {
    // DstReg = FI;
    // Replace with DstReg = FrameReg, DstReg += Offset;

    Register DstReg = II->getOperand(0).getReg();
    BuildMI(MBB, II, DL, TII->get(AIE2P::MOV_alu_mv_mv_mv_scl), DstReg)
        .addReg(FrameReg);

    // Offset can only be a multiple of 64 for PADD imm.
    // Check for 10 bits (in which 6 LSBs are always zeros)
    // for PADD pseudo to expand to PADD A or B.
    if ((Offset % 64 == 0) && isInt<4 + 6>(Offset)) {
      BuildMI(MBB, II, DL, TII->get(AIE2P::PADD_imm_pseudo), DstReg)
          .addReg(DstReg)
          .addImm(Offset);
    } else {
      Register ScratchReg =
          MF.getRegInfo().createVirtualRegister(&AIE2P::eMRegClass);
      if (isIntN(11, Offset)) {
        BuildMI(MBB, II, DL, TII->get(AIE2P::MOV_PD_imm11_pseudo), ScratchReg)
            .addImm(Offset);
      } else {
        BuildMI(MBB, II, DL, TII->get(AIE2P::MOVXM), ScratchReg).addImm(Offset);
      }
      BuildMI(MBB, II, DL, TII->get(AIE2P::PADD_mod_pseudo), DstReg)
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
  return false;
}

Register AIE2PRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  const AIE2PFrameLowering *TFI = getFrameLowering(MF);
  // There is no explicit frame pointer register.
  // Note that this needs to remain consistent with code in AIEFrameLowering.
  return TFI->hasFP(MF) ? AIE2P::p7 : AIE2P::sp;
}

const TargetRegisterClass *
AIE2PRegisterInfo::getPointerRegClass(const MachineFunction &MF,
                                      unsigned Kind) const {
  llvm_unreachable("Un-implemented");
}

const uint32_t *
AIE2PRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                        CallingConv::ID CC) const {
  switch (CC) {
  case CallingConv::AIE_PreserveAll_Vec:
    return CSR_AIE2P_Vec_RegMask;
  default:
    return CSR_AIE2P_RegMask;
  }
}

bool AIE2PRegisterInfo::isTypeLegalForClass(const TargetRegisterClass &RC,
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
  if (T.isPointer() && T.getSizeInBits() == 20 &&
      (RC.getID() == AIE2P::ePRegClassID ||
       RC.getID() == AIE2P::mDmRegClassID ||
       RC.getID() == AIE2P::eSpecial20RegClassID))
    return true;

  // Scalars of 20 bits fit in PTRs
  if (T == LLT::scalar(20)) {
    // 20-bit scalars cannot be allowed on 32-bit reg classes, otherwise llvm
    // will consider that a PTR reg copied as a scalar is 32 bits.
    // FIXME: We should not mix PTRs and GPRs in reg classes.
    if ((RC.getID() == AIE2P::ePRegClassID) ||
        (RC.getID() == AIE2P::mDmRegClassID))
      return true;
    return false;
  }

  return TargetRegisterInfo::isTypeLegalForClass(RC, T);
}

const TargetRegisterClass &
AIE2PRegisterInfo::getMinClassForRegBank(const RegisterBank &RB, LLT Ty) const {
  switch (RB.getID()) {
  case AIE2P::GPRRegBankID:
    return Ty.getSizeInBits() <= 32 ? AIE2P::eRRegClass : AIE2P::eLRegClass;
  case AIE2P::PTRRegBankID:
    return AIE2P::ePRegClass;
  case AIE2P::MODRegBankID:
    return AIE2P::mDmRegClass;
  case AIE2P::VRegBankID: {
    switch (Ty.getSizeInBits()) {
    case 128:
      return AIE2P::VEC128RegClass;
    case 256:
      return AIE2P::VEC256RegClass;
    case 512:
      return AIE2P::VEC512RegClass;
    case 1024:
      return AIE2P::VEC1024RegClass;
    default:
      llvm_unreachable("Unsupported register size.");
    }
  }
  case AIE2P::AccRegBankID: {
    switch (Ty.getSizeInBits()) {
    case 512:
      return AIE2P::ACC512RegClass;
    case 1024:
      return AIE2P::ACC1024RegClass;
    case 2048:
      return AIE2P::ACC2048RegClass;
    default:
      llvm_unreachable("Unsupported register size.");
    }
  }
  case AIE2P::FifoRegBankID: {
    switch (Ty.getSizeInBits()) {
    case 512:
      return AIE2P::FIFO512RegClass;
    case 1024:
      return AIE2P::FIFO1024RegClass;
    default:
      llvm_unreachable("Unsupported register size.");
    }
  }
  }
  llvm_unreachable("Unexpected register bank.");
}

const TargetRegisterClass *AIE2PRegisterInfo::getConstrainedRegClassForOperand(
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

Register AIE2PRegisterInfo::getStackPointerRegister() const {
  return AIE2P::sp;
}

Register AIE2PRegisterInfo::getControlRegister(unsigned Idx) const {
  // Control Register Map. Keys based on encoding.
  static std::unordered_map<unsigned, Register> ControlRegisterMap = {
      {0, AIE2P::crSat},
      {1, AIE2P::crRnd},
      {2, AIE2P::crFPMask},
      {3, AIE2P::crF2IMask},
      {4, AIE2P::crF2FMask},
      {5, AIE2P::crF2BMask},
      {6, AIE2P::crSRSMode},
      {7, AIE2P::crUPSMode},
      {8, AIE2P::crUnpackSize},
      {9, AIE2P::crPackSize},
      {10, AIE2P::srsSign0},
      {11, AIE2P::srsSign1},
      {12, AIE2P::upsSign0},
      {13, AIE2P::upsSign1},
      {14, AIE2P::packSign0},
      {15, AIE2P::packSign1},
      {16, AIE2P::unpackSign0},
      {17, AIE2P::unpackSign1},
      {18, AIE2P::vaddSign0},
      {19, AIE2P::vaddSign1},
      {20, AIE2P::crSCDEn},
      {21, AIE2P::crMCDEn},
      {22, AIE2P::crFPNlfMask},
      {23, AIE2P::crFPCnvFx2FlMask},
      {24, AIE2P::crFPCnvFl2FxMask}

  };

  if (ControlRegisterMap.find(Idx) != ControlRegisterMap.end())
    return ControlRegisterMap[Idx];
  llvm_unreachable("Unexpected key for control register.");
}

Register AIE2PRegisterInfo::getStatusRegister(unsigned Idx) const {
  // Status Register Map. Keys based on encoding.
  static std::unordered_map<unsigned, Register> StatusRegisterMap = {
      {25, AIE2P::srCarry},     {26, AIE2P::srSS0},
      {27, AIE2P::srMS0},       {28, AIE2P::srSRS_of},
      {29, AIE2P::srUPS_of},    {30, AIE2P::srSparse_of},
      {31, AIE2P::srFifo_of},   {32, AIE2P::srFifo_uf},
      {33, AIE2P::srFPFlags},   {34, AIE2P::srF2IFlags},
      {35, AIE2P::srF2FFlags},  {36, AIE2P::srF2BFlags},
      {37, AIE2P::srFPNlf},     {38, AIE2P::srFPCnvFx2Fl},
      {39, AIE2P::srFPCnvFl2Fx}

  };

  if (StatusRegisterMap.find(Idx) != StatusRegisterMap.end())
    return StatusRegisterMap[Idx];
  llvm_unreachable("Unexpected key for status register.");
}

const TargetRegisterClass *
AIE2PRegisterInfo::getLargestLegalSuperClass(const TargetRegisterClass *RC,
                                             const MachineFunction &MF) const {
  // TODO: This implimentation is not complete
  if (AIE2P::eRRegClass.hasSubClassEq(RC))
    return &AIE2P::eRRegClass;
  if (RC == &AIE2P::eR28RegClass)
    return &AIE2P::eRRegClass;

  if (AIE2P::ePRegClass.hasSubClassEq(RC))
    return &AIE2P::spill_eP_to_eRRegClass;

  if (AIE2P::eMRegClass.hasSubClassEq(RC))
    return &AIE2P::spill_eM_to_eRRegClass;
  if (AIE2P::eDNRegClass.hasSubClassEq(RC))
    return &AIE2P::spill_eDN_to_eRRegClass;
  if (AIE2P::eDJRegClass.hasSubClassEq(RC))
    return &AIE2P::spill_eDJ_to_eRRegClass;
  if (AIE2P::eDCRegClass.hasSubClassEq(RC))
    return &AIE2P::spill_eDC_to_eRRegClass;

  if (AIE2P::eSRegClass.hasSubClassEq(RC))
    return &AIE2P::spill_eS_to_eRRegClass;
  if (RC == &AIE2P::ACC512RegClass || RC == &AIE2P::VEC512RegClass)
    // using hasSubClassEq leads to register coalescer changes (spill_vec512
    // will be used more frequently) and thus change machine scheduling
    return &AIE2P::spill_vec512_to_compositeRegClass;
  return RC;
}

const std::set<int> &AIE2PRegisterInfo::getSubRegSplit(int RegClassId) const {
  static const std::set<int> NoSplit = {AIE2P::NoSubRegister};
  static const std::set<int> Mod2DSplit = {AIE2P::sub_mod, AIE2P::sub_dim_size,
                                           AIE2P::sub_dim_stride,
                                           AIE2P::sub_dim_count};
  static const std::set<int> Mod3DSplit = {
      AIE2P::sub_mod,
      AIE2P::sub_dim_size,
      AIE2P::sub_dim_stride,
      AIE2P::sub_dim_count,
      AIE2P::sub_hi_dim_then_sub_mod,
      AIE2P::sub_hi_dim_then_sub_dim_size,
      AIE2P::sub_hi_dim_then_sub_dim_stride,
      AIE2P::sub_hi_dim_then_sub_dim_count};
  switch (RegClassId) {
  case AIE2P::eDRegClassID:
    return Mod2DSplit;
  case AIE2P::eDSRegClassID:
    return Mod3DSplit;
  }
  return NoSplit;
}

const TargetRegisterClass *
AIE2PRegisterInfo::getGPRRegClass(const MachineFunction &MF) const {
  return &AIE2P::eRRegClass;
}

unsigned AIE2PRegisterInfo::getVectorRegBankID() const {
  return AIE2P::VRegBankID;
}

unsigned AIE2PRegisterInfo::getGPRRegBankID() const {
  return AIE2P::GPRRegBankID;
}

unsigned AIE2PRegisterInfo::getMODRegBankID() const {
  return AIE2P::MODRegBankID;
}

unsigned AIE2PRegisterInfo::getPTRRegBankID() const {
  return AIE2P::PTRRegBankID;
}

void AIE2PRegisterInfo::getTargetSubRegs(std::vector<unsigned> &Subregs,
                                         unsigned Size,
                                         const RegisterBank &RB) const {
  bool IsVec =
      (RB.getID() == AIE2P::VRegBankID || RB.getID() == AIE2P::AccRegBankID);
  if (!IsVec) {
    // TODO: support other subreg cases
    assert(Size == 32 && "Unsupported subreg type for scalar!");
    Subregs.push_back(AIE2P::sub_l_even);
    Subregs.push_back(AIE2P::sub_l_odd);
  } else {
    const bool IsVecRB = RB.getID() == AIE2P::VRegBankID;
    switch (Size) {
    case 256:
      assert(IsVecRB && "expected vector register bank for 256 dest type!");
      Subregs.push_back(AIE2P::sub_256_lo);
      Subregs.push_back(AIE2P::sub_256_hi);
      break;
    case 512:
      Subregs.push_back(IsVecRB ? AIE2P::sub_512_lo : AIE2P::sub_512_acc_lo);
      Subregs.push_back(IsVecRB ? AIE2P::sub_512_hi : AIE2P::sub_512_acc_hi);
      break;
    case 1024:
      assert(!IsVecRB &&
             "expected accumulator register bank for 256 dest type!");
      Subregs.push_back(AIE2P::sub_1024_acc_lo);
      Subregs.push_back(AIE2P::sub_1024_acc_hi);
      break;
    default:
      llvm_unreachable("Unsupported subreg type!");
    }
  }
}

bool AIE2PRegisterInfo::isReservedStickyReg(MCRegister PhysReg) const {
  switch (PhysReg) {
  case AIE2P::srSparse_of:
  case AIE2P::srF2FFlags:
  case AIE2P::srF2BFlags:
  case AIE2P::srF2IFlags:
  case AIE2P::srFPFlags:
  case AIE2P::srSRS_of:
  case AIE2P::srUPS_of:
  case AIE2P::srFifo_of:
  case AIE2P::srFifo_uf:
    return true;
  default:
    return false;
  }
}

bool AIE2PRegisterInfo::isVecOrAccRegClass(
    const TargetRegisterClass &RC) const {
  // ******** Vector classes ********
  if (AIE2P::VEC128RegClass.hasSubClassEq(&RC))
    return true;

  if (AIE2P::VEC256RegClass.hasSubClassEq(&RC))
    return true;

  if (AIE2P::VEC512RegClass.hasSubClassEq(&RC))
    return true;

  if (AIE2P::VEC1024RegClass.hasSubClassEq(&RC))
    return true;

  // ******** Accumulator classes ********

  if (AIE2P::ACC512RegClass.hasSubClassEq(&RC))
    return true;

  if (AIE2P::ACC1024RegClass.hasSubClassEq(&RC))
    return true;

  if (AIE2P::ACC2048RegClass.hasSubClassEq(&RC))
    return true;

  // BFP16 registers
  if (AIE2P::VEC576RegClass.hasSubClassEq(&RC) ||
      AIE2P::eEYRegClass.hasSubClassEq(&RC))
    return true;

  return false;
}

bool AIE2PRegisterInfo::isFifoPhysReg(const Register Reg) const {
  return Reg.isPhysical() && (AIE2P::FIFO512RegClass.contains(Reg) ||
                              AIE2P::FIFO1024RegClass.contains(Reg));
}

bool AIE2PRegisterInfo::shouldCoalesce(
    MachineInstr *MI, const TargetRegisterClass *SrcRC, unsigned SubReg,
    const TargetRegisterClass *DstRC, unsigned DstSubReg,
    const TargetRegisterClass *NewRC, LiveIntervals &LIS) const {

  const unsigned SrcSize = getRegSizeInBits(*SrcRC);
  const unsigned DstSize = getRegSizeInBits(*DstRC);
  MachineFunction *MF = MI->getMF();
  const AIEBaseInstrInfo *TII =
      static_cast<const AIEBaseInstrInfo *>(MF->getSubtarget().getInstrInfo());
  const unsigned BasicVectorSize = TII->getBasicVecRegSize();
  // Should not coalesce if copying from bigger source.
  if (!EnableCoalescingForWideCopy && SrcSize < DstSize &&
      (SrcSize >= BasicVectorSize || DstSize >= BasicVectorSize)) {
    MachineBasicBlock *MBB = MI->getParent();
    LiveInterval &LI = LIS.getInterval(MI->getOperand(1).getReg());
    const MachineInstr *FirstMI =
        LI.empty() ? nullptr : LIS.getInstructionFromIndex(LI.beginIndex());
    const MachineInstr *LastMI =
        LI.empty() ? nullptr : LIS.getInstructionFromIndex(LI.endIndex());
    // Coalescing inside the same basic block found beneficial. So, check that
    // the LiveInterval is not just local to MBB.
    if (!FirstMI || FirstMI->getParent() != MBB || !LastMI ||
        LastMI->getParent() != MBB)
      return false;
  }

  return TargetRegisterInfo::shouldCoalesce(MI, SrcRC, SubReg, DstRC, DstSubReg,
                                            NewRC, LIS);
}

unsigned
AIE2PRegisterInfo::matchControlRegisterBitwidth(Register CtrlReg,
                                                unsigned SrcConstVal) const {
  // Modulo by width of control regs. To constrain the max possible value in
  // the register according to register width.
  switch (CtrlReg) {
  case AIE2P::crSRSMode:
  case AIE2P::crUPSMode:
  case AIE2P::crUnpackSize:
  case AIE2P::crPackSize:
  case AIE2P::srsSign0:
  case AIE2P::srsSign1:
  case AIE2P::upsSign0:
  case AIE2P::upsSign1:
  case AIE2P::packSign0:
  case AIE2P::packSign1:
  case AIE2P::unpackSign0:
  case AIE2P::unpackSign1:
  case AIE2P::vaddSign0:
  case AIE2P::vaddSign1:
  case AIE2P::crSCDEn:
  case AIE2P::crMCDEn:
    return SrcConstVal % (1 << 1);
  case AIE2P::crSat:
    return SrcConstVal % (1 << 2);
  case AIE2P::crRnd:
    return SrcConstVal % (1 << 4);
  case AIE2P::crF2FMask:
  case AIE2P::crF2IMask:
  case AIE2P::crFPMask:
  case AIE2P::crF2BMask:
    return SrcConstVal % (1 << 5);
  case AIE2P::crFPNlfMask:
  case AIE2P::crFPCnvFx2FlMask:
  case AIE2P::crFPCnvFl2FxMask:
    return SrcConstVal % (1 << 8);
  default:
    llvm_unreachable("Unknown control register.");
  }
}

unsigned
AIE2PRegisterInfo::matchStatusRegisterBitwidth(Register StatusReg,
                                               unsigned SrcConstVal) const {
  // Modulo by width of status regs. To constrain the max possible value in
  // the register according to register width.
  switch (StatusReg) {
  case AIE2P::srCarry:
  case AIE2P::srSRS_of:
  case AIE2P::srUPS_of:
  case AIE2P::srSparse_of:
  case AIE2P::srFifo_of:
  case AIE2P::srFifo_uf:
    return SrcConstVal % (1 << 1);
  case AIE2P::srFPFlags:
  case AIE2P::srF2IFlags:
  case AIE2P::srF2FFlags:
  case AIE2P::srF2BFlags:
    return SrcConstVal % (1 << 5);
  case AIE2P::srFPNlf:
  case AIE2P::srFPCnvFx2Fl:
  case AIE2P::srFPCnvFl2Fx:
    return SrcConstVal % (1 << 8);
  case AIE2P::srSS0:
  case AIE2P::srMS0:
    return SrcConstVal;
  default:
    llvm_unreachable("Unknown status register.");
  }
}

Register AIE2PRegisterInfo::getUnpackSignCtrlReg() const {
  return AIE2P::unpackSign0;
}

Register AIE2PRegisterInfo::getPackSignCtrlReg() const {
  return AIE2P::packSign0;
}

Register AIE2PRegisterInfo::getPackSizeCtrlReg() const {
  return AIE2P::crPackSize;
}

Register AIE2PRegisterInfo::getSRSSignCtrlReg() const {
  return AIE2P::srsSign0;
}

Register AIE2PRegisterInfo::getSRSModeCtrlReg() const {
  return AIE2P::crSRSMode;
}
