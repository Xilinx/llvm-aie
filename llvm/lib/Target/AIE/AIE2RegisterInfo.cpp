//===-- AIE2RegisterInfo.cpp - AIEnigne V2 Register Information--*- C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIEngine V2 implementation of the TargetRegisterInfo
// class.
//
//===----------------------------------------------------------------------===//

#include "AIE2RegisterInfo.h"
#include "AIE2.h"
#include "AIE2RegisterBankInfo.h"
#include "AIE2Subtarget.h"
#include "AIEBaseRegisterInfo.h"
#include "MCTargetDesc/AIE2MCTargetDesc.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"

#define GET_REGINFO_TARGET_DESC
#include "AIE2GenRegisterInfo.inc"

using namespace llvm;

#define DEBUG_TYPE "aie2-reg-info"

// TODO: More flexible syntax, e.g. --aie-reserved-regs=er:10
static llvm::cl::opt<unsigned>
    ReservedGPRs("aie-reserved-gprs", cl::Hidden, cl::init(0),
                 cl::desc("Number of artificially reserved GPRs"));
static llvm::cl::opt<unsigned>
    ReservedMODs("aie-reserved-mods", cl::Hidden, cl::init(0),
                 cl::desc("Number of artificially reserved MODs"));

static llvm::cl::opt<bool>
    SpillStoGPR("aie2-spill-s-to-r", cl::Hidden, cl::init(true),
                cl::desc("Allow spilling S registers to GPRs"));

static llvm::cl::opt<bool>
    Spill1DModstoGPR("aie2-spill-mods-to-r", cl::Hidden, cl::init(true),
                     cl::desc("Allow spilling 1D modifier registers to GPRs"));

static llvm::cl::opt<bool> SimplifyCRSRRegs(
    "aie2-simplify-crsr-edges", cl::Hidden, cl::init(true),
    cl::desc("Allow simplifying redundant CR and SR reg assignments"));

AIE2RegisterInfo::AIE2RegisterInfo(unsigned HwMode)
    : AIE2GenRegisterInfo(AIE2::SP, /*DwarfFlavour*/ 0, /*EHFlavor*/ 0,
                          /*PC*/ 0, HwMode) {}

const MCPhysReg *
AIE2RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_AIE2_SaveList;
}

BitVector AIE2RegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  const TargetFrameLowering *TFI = getFrameLowering(MF);
  BitVector Reserved(getNumRegs());

  // Use markSuperRegs to ensure any register aliases are also reserved

  // SP is only accessible by special instructions.  Without this the
  // instruction verifier complains because SP is always implicitly defined
  // and never killed by the instruction allocator.
  if (TFI->hasFP(MF))
    markSuperRegs(Reserved, AIE2::p7);
  markSuperRegs(Reserved, AIE2::SP);

  // LR is also always implicitly defined.
  markSuperRegs(Reserved, AIE2::lr);

  // Reserve control and status registers
  for (auto &Reg : AIE2::mCRmRegClass) {
    markSuperRegs(Reserved, Reg);
  }
  for (auto &Reg : AIE2::mSRmRegClass) {
    markSuperRegs(Reserved, Reg);
  }

  // If requested, reserve GPRs to artificially increase the register pressure.
  // We reserve them "from the end" because the first GPRs are typically used
  // by the calling convention.
  auto ReserveRegs = [&](const TargetRegisterClass &RC, unsigned Num) {
    unsigned CurrentReservedRegs = 0;
    for (auto &Reg : reverse(RC)) {
      if (CurrentReservedRegs == Num)
        break;
      ++CurrentReservedRegs;
      for (MCSubRegIterator SubReg(Reg, this, /*self=*/true); SubReg.isValid();
           ++SubReg) {
        markSuperRegs(Reserved, *SubReg);
      }
    }
  };

  ReserveRegs(AIE2::eRRegClass, ReservedGPRs);
  ReserveRegs(AIE2::eDRegClass, ReservedMODs);

  // CORE_ID is reserved.
  markSuperRegs(Reserved, AIE2::CORE_ID);

  // Mark the hardware loop related register as reserved,
  // otherwise they are considered dead
  markSuperRegs(Reserved, AIE2::LC);
  markSuperRegs(Reserved, AIE2::LS);
  markSuperRegs(Reserved, AIE2::LE);

  assert(checkAllSuperRegsMarked(Reserved));
  return Reserved;
}

bool AIE2RegisterInfo::isSimplifiableReservedReg(MCRegister PhysReg) const {
  return SimplifyCRSRRegs && (AIE2::mCRmRegClass.contains(PhysReg) ||
                              AIE2::mSRmRegClass.contains(PhysReg));
}

const uint32_t *AIE2RegisterInfo::getNoPreservedMask() const {
  return CSR_NoRegs_RegMask;
}

bool AIE2RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                           int SPAdj, unsigned FIOperandNum,
                                           RegScavenger *RS) const {
  assert(SPAdj == 0 && "Unexpected non-zero SPAdj value");

  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getMF();
  const AIE2InstrInfo *TII = MF.getSubtarget<AIE2Subtarget>().getInstrInfo();
  const AIE2FrameLowering *TFI = getFrameLowering(MF);
  DebugLoc DL = MI.getDebugLoc();

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
  case AIE2::LDA_dms_spill:
  case AIE2::ST_dms_spill:
  case AIE2::LDA_D_SPILL:
  case AIE2::ST_D_SPILL:
  case AIE2::LDA_DS_SPILL:
  case AIE2::ST_DS_SPILL:
  case AIE2::VLDA_dmw_lda_w_ag_spill:
  case AIE2::VST_dmw_sts_w_ag_spill:
  case AIE2::VLDA_dmw_lda_am_ag_spill:
  case AIE2::VST_dmw_sts_am_ag_spill:
  case AIE2::LDA_dmv_lda_q_ag_spill:
  case AIE2::ST_dmv_sts_q_ag_spill:
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
    // This is implicitly a stack spill instruction, so simply replace
    // the TargetFrameIndex operand with the right immediate offset.
    // Note that LDB path does not support SPIL instructions
    MI.getOperand(FIOperandNum).ChangeToImmediate(Offset);
    return false;
  case AIE2::PseudoFI: {
    // DstReg = FI;
    // Replace with DstReg = FrameReg, DstReg += Offset;
    MachineBasicBlock &MBB = *MI.getParent();

    Register DstReg = II->getOperand(0).getReg();
    BuildMI(MBB, II, DL, TII->get(AIE2::MOV_mv_scl), DstReg).addReg(FrameReg);

    // Offset can only be a multiple of 4 for PADD imm.
    // Check for 11 bits (in which 2 LSBs are always zeros)
    // for PADD pseudo to expand to PADD A or B.
    if ((Offset % 4 == 0) && isInt<9 + 2>(Offset)) {
      BuildMI(MBB, II, DL, TII->get(AIE2::PADD_imm_pseudo), DstReg)
          .addReg(DstReg)
          .addImm(Offset);
      // PADDA allows 12 bit imm (include 2 LSBs always 0).
      // Select PADDA if the offset exceeds the 11 bit range.
    } else if ((Offset % 4 == 0) && isInt<10 + 2>(Offset)) {
      BuildMI(MBB, II, DL, TII->get(AIE2::PADDA_lda_ptr_inc_idx_imm), DstReg)
          .addReg(DstReg)
          .addImm(Offset);
      // If offset is greater than the 12 bit range, then we use PADD pseudo
      // with modifier registers.
    } else {
      Register ScratchReg =
          MF.getRegInfo().createVirtualRegister(&AIE2::eMRegClass);
      if (isIntN(11, Offset)) {
        BuildMI(MBB, II, DL, TII->get(AIE2::MOVA_lda_cg), ScratchReg)
            .addImm(Offset);
      } else {
        BuildMI(MBB, II, DL, TII->get(AIE2::MOVXM_lng_cg), ScratchReg)
            .addImm(Offset);
      }
      BuildMI(MBB, II, DL, TII->get(AIE2::PADD_mod_pseudo), DstReg)
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

Register AIE2RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  const AIE2FrameLowering *TFI = getFrameLowering(MF);
  // There is no explicit frame pointer register.
  // Note that this needs to remain consistent with code in AIEFrameLowering.
  return TFI->hasFP(MF) ? AIE2::p7 : AIE2::SP;
}

const TargetRegisterClass *
AIE2RegisterInfo::getPointerRegClass(const MachineFunction &MF,
                                     unsigned Kind) const {
  llvm_unreachable("Un-implemented");
}

const uint32_t *
AIE2RegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID /*CC*/) const {
  return CSR_AIE2_RegMask;
}

bool AIE2RegisterInfo::isTypeLegalForClass(const TargetRegisterClass &RC,
                                           LLT T) const {
  // Note: With the current TableGen backend, isTypeLegalForClass will never
  // return true when LLT is a LLT::pointer type. In most cases, that is fine,
  // because the MachineVerifier will use getMinimalPhysRegClass() as a fallback
  // mechanism to derive type information (e.g. the register size).
  //
  // However, this function will be needed as long as there will be classes with
  // both GPR and PTR types. When that happens, TableGen can create subclasses
  // with the pointer registers contained in the mixed classes. The problem is
  // that those classes will have the properties of the mixed class, i.e. they
  // will represent 32-bit registers. This means that if a pointer register is
  // part of a mixed PTR/GPR class, its size will be considered as 32 bits.
  // (Thus making the Machine verifier shout.)
  // One way around the problem is to specifically mention that LLT::pointer
  // types are legal on pointer classes. That way, the fallback mechanism using
  // getMinimalPhysRegClass() isn't used.
  //
  // TLDR: for pointer registers which are part of mixed PTR/GPR classes, there
  // needs to be specific handling here of the legal register classes. The same
  // is also applicable for modifiers in compound register classes.

  // PTR/MOD registers obviously support pointers
  if (T == LLT::pointer(0, 20) && RC.getID() == AIE2::ePRegClassID)
    return true;
  if (T == LLT::pointer(0, 20) && RC.getID() == AIE2::mDmRegClassID)
    return true;
  if (T == LLT::pointer(0, 20) && RC.getID() == AIE2::eSpecial20RegClassID)
    return true;

  // Scalars of 20 bits fit in PTRs
  if (T == LLT::scalar(20)) {
    // 20-bit scalars cannot be allowed on 32-bit reg classes, otherwise llvm
    // will consider that a PTR reg copied as a scalar is 32 bits.
    // FIXME: We should not mix PTRs and GPRs in reg classes.
    if ((RC.getID() == AIE2::ePRegClassID) ||
        (RC.getID() == AIE2::mDmRegClassID))
      return true;
    return false;
  }

  return TargetRegisterInfo::isTypeLegalForClass(RC, T);
}

const TargetRegisterClass &
AIE2RegisterInfo::getMinClassForRegBank(const RegisterBank &RB, LLT Ty) const {
  switch (RB.getID()) {
  case AIE2::GPRRegBankID:
    return Ty.getSizeInBits() <= 32 ? AIE2::eRRegClass : AIE2::eLRegClass;
  case AIE2::PTRRegBankID:
    return AIE2::ePRegClass;
  case AIE2::MODRegBankID:
    return AIE2::mDmRegClass;
  case AIE2::VRegBankID: {
    switch (Ty.getSizeInBits()) {
    case 128:
      return AIE2::VEC128RegClass;
    case 256:
      return AIE2::VEC256RegClass;
    case 512:
      return AIE2::VEC512RegClass;
    case 1024:
      return AIE2::VEC1024RegClass;
    default:
      llvm_unreachable("Unsupported register size.");
    }
  }
  case AIE2::AccRegBankID: {
    switch (Ty.getSizeInBits()) {
    case 256:
      return AIE2::ACC256RegClass;
    case 512:
      return AIE2::ACC512RegClass;
    case 1024:
      return AIE2::ACC1024RegClass;
    default:
      llvm_unreachable("Unsupported register size.");
    }
  }
  }

  llvm_unreachable("Unexpected register bank.");
}

const TargetRegisterClass *AIE2RegisterInfo::getConstrainedRegClassForOperand(
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

Register AIE2RegisterInfo::getStackPointerRegister() const { return AIE2::SP; }

Register AIE2RegisterInfo::getControlRegister(unsigned Idx) const {
  // Control Register Map. Keys based on encoding.
  static std::unordered_map<unsigned, Register> ControlRegisterMap = {
      {0, AIE2::crVaddSign}, {1, AIE2::crF2FMask},  {2, AIE2::crF2IMask},
      {3, AIE2::crFPMask},   {4, AIE2::crMCDEn},    {5, AIE2::crPackSign},
      {6, AIE2::crRnd},      {7, AIE2::crSCDEn},    {8, AIE2::crSRSSign},
      {9, AIE2::crSat},      {10, AIE2::crUPSSign}, {11, AIE2::crUnpackSign}};

  if (ControlRegisterMap.find(Idx) != ControlRegisterMap.end())
    return ControlRegisterMap[Idx];
  llvm_unreachable("Unexpected key for control register.");
}

const TargetRegisterClass *
AIE2RegisterInfo::getLargestLegalSuperClass(const TargetRegisterClass *RC,
                                            const MachineFunction &MF) const {

  if (AIE2::eRRegClass.hasSubClassEq(RC))
    return &AIE2::eRRegClass;
  if (RC == &AIE2::eR28RegClass)
    return &AIE2::eRRegClass;

  if (AIE2::ePRegClass.hasSubClassEq(RC))
    return &AIE2::spill_eP_to_eRRegClass;

  if (Spill1DModstoGPR && AIE2::eMRegClass.hasSubClassEq(RC))
    return &AIE2::spill_eM_to_eRRegClass;
  if (Spill1DModstoGPR && AIE2::eDNRegClass.hasSubClassEq(RC))
    return &AIE2::spill_eDN_to_eRRegClass;
  if (Spill1DModstoGPR && AIE2::eDJRegClass.hasSubClassEq(RC))
    return &AIE2::spill_eDJ_to_eRRegClass;
  if (Spill1DModstoGPR && AIE2::eDCRegClass.hasSubClassEq(RC))
    return &AIE2::spill_eDC_to_eRRegClass;

  if (SpillStoGPR && AIE2::eSRegClass.hasSubClassEq(RC))
    return &AIE2::spill_eS_to_eRRegClass;

  return RC;
}

const TargetRegisterClass *
AIE2RegisterInfo::getGPRRegClass(const MachineFunction &MF) const {
  return &AIE2::eRRegClass;
}

const std::set<int> &AIE2RegisterInfo::getSubRegSplit(int RegClassId) const {
  static const std::set<int> NoSplit = {AIE2::NoSubRegister};
  static const std::set<int> Mod2DSplit = {AIE2::sub_mod, AIE2::sub_dim_size,
                                           AIE2::sub_dim_stride,
                                           AIE2::sub_dim_count};
  static const std::set<int> Mod3DSplit = {AIE2::sub_mod,
                                           AIE2::sub_dim_size,
                                           AIE2::sub_dim_stride,
                                           AIE2::sub_dim_count,
                                           AIE2::sub_hi_dim_then_sub_mod,
                                           AIE2::sub_hi_dim_then_sub_dim_size,
                                           AIE2::sub_hi_dim_then_sub_dim_stride,
                                           AIE2::sub_hi_dim_then_sub_dim_count};
  switch (RegClassId) {
  case AIE2::eDRegClassID:
    return Mod2DSplit;
  case AIE2::eDSRegClassID:
    return Mod3DSplit;
  }
  return NoSplit;
}

SmallSet<int, 8>
AIE2RegisterInfo::getCoveringSubRegs(const TargetRegisterClass &RC) const {
  // TODO: This could be generated from TableGen by looking at MCRegisters.
  SmallSet<int, 8> Subregs;
  if (AIE2::VEC512RegClass.hasSubClassEq(&RC) ||
      AIE2::ACC512RegClass.hasSubClassEq(&RC)) {
    Subregs.insert(AIE2::sub_256_lo);
    Subregs.insert(AIE2::sub_256_hi);
  }
  if (AIE2::VEC1024RegClass.hasSubClassEq(&RC) ||
      AIE2::ACC1024RegClass.hasSubClassEq(&RC)) {
    Subregs.insert(AIE2::sub_512_lo);
    Subregs.insert(AIE2::sub_512_hi);
  }
  return Subregs;
}

bool AIE2RegisterInfo::isVecOrAccRegClass(const TargetRegisterClass &RC) const {
  // ******** Vector classes ********
  if (AIE2::VEC128RegClass.hasSubClassEq(&RC))
    return true;

  if (AIE2::VEC256RegClass.hasSubClassEq(&RC))
    return true;

  if (AIE2::VEC512RegClass.hasSubClassEq(&RC))
    return true;

  if (AIE2::VEC1024RegClass.hasSubClassEq(&RC))
    return true;

  // ******** Accumulator classes ********
  if (AIE2::ACC256RegClass.hasSubClassEq(&RC))
    return true;

  if (AIE2::ACC512RegClass.hasSubClassEq(&RC))
    return true;

  if (AIE2::ACC1024RegClass.hasSubClassEq(&RC))
    return true;

  return false;
}
