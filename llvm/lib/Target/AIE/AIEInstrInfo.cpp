//===-- AIEInstrInfo.cpp - AIE Instruction Information ------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIE implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "AIEInstrInfo.h"
#include "AIE.h"
#include "AIEHazardRecognizerPRAS.h"
#include "AIESubtarget.h"
#include "AIETargetMachine.h"
#include "MCTargetDesc/AIEFormat.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Automaton.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"

#define DEBUG_TYPE "aie-codegen"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "AIEGenInstrInfo.inc"

namespace {
const AIEMCFormats AIE1Formats;
} // namespace

AIEInstrInfo::AIEInstrInfo()
    : AIEGenInstrInfo(AIE::ADJCALLSTACKUP, AIE::ADJCALLSTACKDOWN) {
  FormatInterface = &AIE1Formats;
  FuncUnitWrapper::setFormatInterface(FormatInterface);
}

MCSlotKind AIEInstrInfo::getSlotKind(unsigned Opcode) const {
  return FormatInterface->getSlotKind(Opcode);
}

const MCSlotInfo *AIEInstrInfo::getSlotInfo(const MCSlotKind Kind) const {
  return FormatInterface->getSlotInfo(Kind);
}

const PacketFormats &AIEInstrInfo::getPacketFormats() const {
  return FormatInterface->getPacketFormats();
}

ScheduleHazardRecognizer*
AIEInstrInfo::CreateTargetPostRAHazardRecognizer(
      const InstrItineraryData *II, const ScheduleDAG *DAG) const {

    // AIE has a fully exposed pipeline, so we have to insert
    // Noops in the case of instruction dependence hazards.
  return new AIEHazardRecognizerPRAS(this, II, DAG);
}

unsigned AIEInstrInfo::isLoadFromStackSlot(const MachineInstr &MI,
                                             int &FrameIndex) const {
  switch (MI.getOpcode()) {
  default:
    return 0;
  case AIE::LDA_spis_PTR:
  case AIE::LDA_spis_GPR:
  case AIE::LDA_SPIL_PTR:
  case AIE::LDA_SPIL_GPR:
  case AIE::LDB_spis:
    break;
  }

  if (MI.getOperand(1).isFI()) {
    FrameIndex = MI.getOperand(1).getIndex();
    return MI.getOperand(0).getReg();
  }

  return 0;
}

unsigned AIEInstrInfo::isStoreToStackSlot(const MachineInstr &MI,
                                            int &FrameIndex) const {
  switch (MI.getOpcode()) {
  default:
    return 0;
  case AIE::ST_spis_GPR:
  case AIE::ST_SPIL_GPR:
  case AIE::ST_spis_PTR:
  case AIE::ST_SPIL_PTR:
  case AIE::VST_spis:
    break;
  }

  if (MI.getOperand(1).isFI()) {
    FrameIndex = MI.getOperand(1).getIndex();
    return MI.getOperand(0).getReg();
  }

  return 0;
}

// Implement CopyToReg/CopyFromReg
// Note: This code reserves r15 for some register bounces.  This is suboptimal,
// but since this code executes *after* register allocation it's too late to
// allocate new registers.  Is there a better solution to this?
void AIEInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator MBBI,
                               const DebugLoc &DL, MCRegister DstReg,
                               MCRegister SrcReg, bool KillSrc) const {
  MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
  const TargetRegisterInfo &TRI = *MRI.getTargetRegisterInfo();

  // Generate a mov instruction
  if ((AIE::mMvSclRegClass.contains(SrcReg) &&
       AIE::mMvSclRegClass.contains(DstReg)) ||
      (AIE::PTRRegClass.contains(SrcReg) &&
       AIE::PTRRegClass.contains(DstReg))) {
    BuildMI(MBB, MBBI, DL, get(AIE::MOV), DstReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
  } else if (AIE::SPRRegClass.contains(SrcReg) &&
             AIE::GPRRegClass.contains(DstReg)) {
    BuildMI(MBB, MBBI, DL, get(AIE::MV_SPECIAL2R), DstReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
  } else if (AIE::GPRRegClass.contains(SrcReg) &&
             AIE::SPRRegClass.contains(DstReg)) {
    BuildMI(MBB, MBBI, DL, get(AIE::MV_R2SPECIAL), DstReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
  } else if (AIE::SPRRegClass.contains(SrcReg) &&
             AIE::PTRRegClass.contains(DstReg)) {
    Register GPRReg = AIE::r15;
    BuildMI(MBB, MBBI, DL, get(AIE::MV_SPECIAL2R), GPRReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
    BuildMI(MBB, MBBI, DL, get(AIE::MOV), DstReg)
      .addReg(GPRReg, getKillRegState(KillSrc));
  } else if (AIE::PTRRegClass.contains(SrcReg) &&
             AIE::SPRRegClass.contains(DstReg)) {
    Register GPRReg = AIE::r15;
    BuildMI(MBB, MBBI, DL, get(AIE::MOV), GPRReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
    BuildMI(MBB, MBBI, DL, get(AIE::MV_R2SPECIAL), DstReg)
      .addReg(GPRReg, getKillRegState(KillSrc));
  } else if (AIE::SPRRegClass.contains(SrcReg) &&
             AIE::SPRRegClass.contains(DstReg)) {
    Register GPRReg = AIE::r15;
    BuildMI(MBB, MBBI, DL, get(AIE::MV_SPECIAL2R), GPRReg)
        .addReg(SrcReg, getKillRegState(KillSrc));
    BuildMI(MBB, MBBI, DL, get(AIE::MV_R2SPECIAL), DstReg)
        .addReg(GPRReg, getKillRegState(KillSrc));
  } else if (AIE::VEC128RegClass.contains(SrcReg) &&
             AIE::VEC128RegClass.contains(DstReg)) {
    BuildMI(MBB, MBBI, DL, get(AIE::MV_V), DstReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
  } else if (AIE::VEC256RegClass.contains(SrcReg) &&
             AIE::VEC256RegClass.contains(DstReg)) {
    BuildMI(MBB, MBBI, DL, get(AIE::MV_W), DstReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
  } else if (AIE::VEC512RegClass.contains(SrcReg) &&
             AIE::VEC512RegClass.contains(DstReg)) {
    BuildMI(MBB, MBBI, DL, get(AIE::MV_X), DstReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
  } else if (AIE::VEC1024RegClass.contains(SrcReg) &&
             AIE::VEC1024RegClass.contains(DstReg)) {
    // The 1024-bit registers share their high-order bits.
    if(SrcReg == AIE::ya && DstReg == AIE::yd) {
      BuildMI(MBB, MBBI, DL, get(AIE::MV_X), AIE::xd)
      .addReg(AIE::xa, getKillRegState(KillSrc));
    } else if(SrcReg == AIE::yd && DstReg == AIE::ya) {
      BuildMI(MBB, MBBI, DL, get(AIE::MV_X), AIE::xa)
      .addReg(AIE::xd, getKillRegState(KillSrc));
    }
  } else if (AIE::GPRRegClass.contains(SrcReg) &&
             AIE::VEC256RegClass.contains(DstReg)) {
    // only for f32 type.
    BuildMI(MBB, MBBI, DL, get(AIE::S2V_SHIFTW0_R32), DstReg)
      .addReg(DstReg, RegState::Undef)
      .addReg(SrcReg, getKillRegState(KillSrc));
  } else if (AIE::mMv0Cg20RegClass.contains(SrcReg) &&
             AIE::VEC256RegClass.contains(DstReg)) {
    // only for f32 type.  Bounce through GPR
    Register GPRReg = AIE::r15;
    BuildMI(MBB, MBBI, DL, get(AIE::MOV), GPRReg)
        .addReg(SrcReg, getKillRegState(KillSrc));
    BuildMI(MBB, MBBI, DL, get(AIE::S2V_SHIFTW0_R32), DstReg)
        .addReg(DstReg, RegState::Undef)
        .addReg(GPRReg, getKillRegState(KillSrc));
  } else if (AIE::VEC256RegClass.contains(SrcReg) &&
             AIE::GPRRegClass.contains(DstReg)) {
    // only for f32 type.
    BuildMI(MBB, MBBI, DL, get(AIE::S2V_EXT_R32), DstReg)
      .addReg(TRI.getSubReg(SrcReg, AIE::sub_128bit_lo), getKillRegState(true))
      .addImm(0);
  } else if (AIE::VEC256RegClass.contains(SrcReg) &&
             AIE::PTRRegClass.contains(DstReg)) {
    // only for f32 type.
    BuildMI(MBB, MBBI, DL, get(AIE::S2V_EXT_P32), DstReg)
      .addReg(TRI.getSubReg(SrcReg, AIE::sub_128bit_lo), getKillRegState(true))
      .addImm(0);
  } else if (AIE::VEC256RegClass.contains(SrcReg) &&
             AIE::mMv0Cg20RegClass.contains(DstReg)) {
    // only for f32 type.  Bounce through GPR
    Register GPRReg = AIE::r15;
    BuildMI(MBB, MBBI, DL, get(AIE::S2V_EXT_R32), GPRReg)
        .addReg(TRI.getSubReg(SrcReg, AIE::sub_128bit_lo),
                getKillRegState(true))
        .addImm(0);
    BuildMI(MBB, MBBI, DL, get(AIE::MOV), DstReg)
        .addReg(GPRReg, getKillRegState(KillSrc));
  } else if (AIE::mCRegClass.contains(SrcReg) &&
             AIE::mCRegClass.contains(DstReg)) {
    BuildMI(MBB, MBBI, DL, get(AIE::MOV), TRI.getSubReg(DstReg, AIE::sub_32_lo))
      .addReg(TRI.getSubReg(SrcReg, AIE::sub_32_lo), getKillRegState(KillSrc));
    BuildMI(MBB, MBBI, DL, get(AIE::MOV), TRI.getSubReg(DstReg, AIE::sub_32_hi))
      .addReg(TRI.getSubReg(SrcReg, AIE::sub_32_hi), getKillRegState(KillSrc));
  } else if (AIE::ACC384RegClass.contains(SrcReg) &&
             AIE::ACC384RegClass.contains(DstReg)) {
    BuildMI(MBB, MBBI, DL, get(AIE::ACCUMULATOR_MOVE), DstReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
  } else {
    assert(false && "unhandled case in copyPhysReg");
  }
}

// Sometimes the instruction set encodes smaller registers (256-bit or 512-bit)
// using a 1024-bit yreg and an offset.
void getYRegForNarrowReg(Register &yreg, int &offset, Register wreg) {
  switch(wreg.id()) {
    case AIE::wr0:
    case AIE::xa:
      yreg = AIE::ya;
      offset = 0;
      return;
    case AIE::wr1:
      yreg = AIE::ya;
      offset = 8;
      return;
    case AIE::wr2:
    case AIE::xb:
      yreg = AIE::ya;
      offset = 16;
      return;
    case AIE::wr3:
      yreg = AIE::ya;
      offset = 24;
      return;
    case AIE::wd0:
    case AIE::xd:
      yreg = AIE::yd;
      offset = 0;
      return;
    case AIE::wd1:
      yreg = AIE::yd;
      offset = 8;
      return;
  }
  assert(false && "Illegal register");
}

// Given a VCMP512 pseudo-op, return the VCMP op with the corresponding type.
unsigned getVCMPforPseudoVCMP(unsigned opcode) {
  switch(opcode) {
    case AIE::VCMP512GPR_S16:
    case AIE::VCMP512_S16:
      return AIE::VCMP_S16;
    case AIE::VCMP512GPR_S32:
    case AIE::VCMP512_S32:
      return AIE::VCMP_S32;
    case AIE::VCMP512GPR_U8:
    case AIE::VCMP512_U8:
      return AIE::VCMP_U8;
    case AIE::VCMP512GPR_S8:
    case AIE::VCMP512_S8:
      return AIE::VCMP_S8;
  }
  assert(false && "Illegal opcode");
}

SmallVector<AIEBaseInstrInfo::AIEPseudoExpandInfo, 4>
AIEInstrInfo::getSpillPseudoExpandInfo(const MachineInstr &MI) const {
  if (!MI.isPseudo())
    return {};

  switch (MI.getOpcode()) {
  case AIE::I384_LOAD:
    return {{AIE::VLDA_SPIL_AM_LO_I48, AIE::NoSubRegister, /*MemSize=*/32},
            {AIE::VLDA_SPIL_AM_HI_I48, AIE::NoSubRegister, /*MemSize=*/32}};
  case AIE::I512_LOAD:
    return {{AIE::WVLDA_SPIL, AIE::sub_256bit_lo},
            {AIE::WVLDA_SPIL, AIE::sub_256bit_hi}};
  case AIE::I768_LOAD:
    return {{AIE::I384_LOAD, AIE::sub_384bit_lo},
            {AIE::I384_LOAD, AIE::sub_384bit_hi}};
  case AIE::I1024_LOAD:
    return {{AIE::I512_LOAD, AIE::sub_512bit_lo},
            {AIE::I512_LOAD, AIE::sub_512bit_hi}};
  case AIE::I384_STORE:
    return {{AIE::WVST_SPIL_AM_LO_I48, AIE::NoSubRegister, /*MemSize=*/32},
            {AIE::WVST_SPIL_AM_HI_I48, AIE::NoSubRegister, /*MemSize=*/32}};
  case AIE::I512_STORE:
    return {{AIE::WVST_SPIL, AIE::sub_256bit_lo},
            {AIE::WVST_SPIL, AIE::sub_256bit_hi}};
  case AIE::I768_STORE:
    return {{AIE::I384_STORE, AIE::sub_384bit_lo},
            {AIE::I384_STORE, AIE::sub_384bit_hi}};
  case AIE::I1024_STORE:
    return {{AIE::I512_STORE, AIE::sub_512bit_lo},
            {AIE::I512_STORE, AIE::sub_512bit_hi}};
  default:
    llvm_unreachable("Un-handled spill opcode.");
  }
}

bool AIEInstrInfo::expandPostRAPseudo(MachineInstr &MI) const {
  auto &MBB = *MI.getParent();
  auto DL = MI.getDebugLoc();

  switch (MI.getOpcode()) {
  case AIE::VFPMAC256GPR:
  case AIE::VFPMAC256: {
    Register wreg = MI.getOperand(3).getReg();
    Register yreg;
    int offset;
    getYRegForNarrowReg(yreg, offset, wreg);
    BuildMI(MBB, MI, DL, get(AIE::MOV_U20), AIE::r15)
        .addImm(offset);
    auto b = BuildMI(MBB, MI, DL, get(AIE::VFPMAC));
    // Swap the first two operands if necessary
    if(MI.getOpcode() == AIE::VFPMAC256GPR)
        b.add(MI.getOperand(1))
        .add(MI.getOperand(0));
    else // VFPMAC256...
        b.add(MI.getOperand(0))
        .add(MI.getOperand(1));

    b.add(MI.getOperand(2))
        .addReg(yreg)
        .addReg(AIE::r15, RegState::Kill)
        .add(MI.getOperand(4))
        .add(MI.getOperand(5))
        .addImm(0)
        .add(MI.getOperand(6))
        .add(MI.getOperand(7))
        .add(MI.getOperand(8));

    MI.eraseFromParent();
    return true;
  }
  case AIE::VFPMUL256GPR:
  case AIE::VFPMUL256: {
    Register wreg = MI.getOperand(2).getReg();
    Register yreg;
    int offset;
    getYRegForNarrowReg(yreg, offset, wreg);
    BuildMI(MBB, MI, DL, get(AIE::MOV_U20), AIE::r15)
        .addImm(offset);
    auto b = BuildMI(MBB, MI, DL, get(AIE::VFPMUL));
    // Swap the first two operands if necessary
    if(MI.getOpcode() == AIE::VFPMUL256GPR)
        b.add(MI.getOperand(1))
        .add(MI.getOperand(0));
    else // VFPMUL256...
        b.add(MI.getOperand(0))
        .add(MI.getOperand(1));

    b.addReg(yreg)
        .addReg(AIE::r15, RegState::Kill)
        .add(MI.getOperand(3))
        .add(MI.getOperand(4))
        .addImm(0)
        .add(MI.getOperand(5))
        .add(MI.getOperand(6))
        .add(MI.getOperand(7));

    MI.eraseFromParent();
    return true;
  }
  case AIE::VCMP512GPR_S16:
  case AIE::VCMP512GPR_S32:
  case AIE::VCMP512GPR_U8:
  case AIE::VCMP512GPR_S8:
  case AIE::VCMP512_S16:
  case AIE::VCMP512_S32:
  case AIE::VCMP512_U8:
  case AIE::VCMP512_S8: {
    Register x1reg = MI.getOperand(2).getReg();
    Register x2reg = MI.getOperand(3).getReg();
    Register y1reg, y2reg;
    int offset1, offset2;
    getYRegForNarrowReg(y1reg, offset1, x1reg);
    getYRegForNarrowReg(y2reg, offset2, x2reg);
    assert(y1reg == y2reg);
    BuildMI(MBB, MI, DL, get(AIE::MOV_U20), AIE::r15)
        .addImm(offset1);
    BuildMI(MBB, MI, DL, get(AIE::MOV_U20), AIE::r5)
        .addImm(offset2);
    auto b = BuildMI(MBB, MI, DL, get(getVCMPforPseudoVCMP(MI.getOpcode())));
    // Swap the first two operands if necessary
    switch(MI.getOpcode()) {
      case AIE::VCMP512GPR_S16:
      case AIE::VCMP512GPR_S32:
      case AIE::VCMP512GPR_U8:
      case AIE::VCMP512GPR_S8:
        b.add(MI.getOperand(1))
        .add(MI.getOperand(0));
        break;
      case AIE::VCMP512_S16:
      case AIE::VCMP512_S32:
      case AIE::VCMP512_U8:
      case AIE::VCMP512_S8:
        b.add(MI.getOperand(0))
        .add(MI.getOperand(1));
    }
    b.addReg(y1reg)
        .addReg(AIE::r15, RegState::Kill) // xstart
        .add(MI.getOperand(4)) // xoffs
        .addReg(AIE::r5, RegState::Kill) // ystart
        .add(MI.getOperand(5)) // yoffs
        .add(MI.getOperand(6)); // conf

    MI.eraseFromParent();
    return true;
  }
  case AIE::LR_LOAD:
    int64_t Imm;
    Imm = MI.getOperand(1).getImm();
    if (isInt<10>(Imm)) {
      // Fits in a 10 bit negative number, compatible with LDB_spis
      MI.setDesc(get(AIE::LDB_spis));
    } else {
      // Fallback to LDA_SPIL, which takes a larger offset encoding.
      // The result of this is something like this:
      // lda.spil r15, [sp, #offset]
      // mov     lr, r15
      MI.setDesc(get(AIE::LDA_SPIL_GPR));
      MI.getOperand(0).setReg(AIE::r15);
      auto b = BuildMI(MBB, MI, DL, get(AIE::MV_R2SPECIAL), AIE::lr)
                   .addReg(AIE::r15, getKillRegState(true));
      MI.moveBefore(b);
    }
    return true;
  case AIE::I384_LOAD:
  case AIE::I512_LOAD:
  case AIE::I768_LOAD:
  case AIE::I1024_LOAD:
  case AIE::I384_STORE:
  case AIE::I512_STORE:
  case AIE::I768_STORE:
  case AIE::I1024_STORE: {
    MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
    const TargetRegisterInfo &TRI = *MRI.getTargetRegisterInfo();
    expandSpillPseudo(MI, TRI, /*SubRegOffsetAlign=*/Align(32));
    return true;
  }
  }
  return false;
}

// Return true if TRC is a superclass of RC or contains the given reg.
// This is primarily a helper function for the functions below.  The first
// case is active when Reg is a virtual register, but is apparently not
// sufficient alone
static bool regClassMatches(const TargetRegisterClass &TRC,
                            const TargetRegisterClass *RC,
                            unsigned Reg) {
  if(RC && TRC.hasSubClassEq(RC)) return true;
  else if(TRC.contains(Reg)) return true;
  else return false;
}

// Store a register to a stack slot.  Used in eliminating FrameIndex pseduo-ops.
void AIEInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
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

  unsigned Opcode;
  LLVM_DEBUG(dbgs() << "Attempting to Store: " << SrcReg << " To " << FI
                    << "\n");
  if (regClassMatches(AIE::mRCmRegClass, RC,
                      SrcReg)) // The 32-bit subset of mSclSt
    Opcode = AIE::ST_SPIL_GPR;
  else if (regClassMatches(AIE::mSclSt_PTRRegClass, RC, SrcReg))
    Opcode = AIE::ST_SPIL_PTR;
  else if (regClassMatches(AIE::VEC128RegClass, RC, SrcReg))
    Opcode = AIE::VST_SPIL;
  else if (regClassMatches(AIE::VEC256RegClass, RC, SrcReg))
    Opcode = AIE::WVST_SPIL;
  else if (regClassMatches(AIE::VEC512RegClass, RC, SrcReg))
    Opcode = AIE::I512_STORE;
  else if (regClassMatches(AIE::VEC1024RegClass, RC, SrcReg))
    Opcode = AIE::I1024_STORE;
  else if (regClassMatches(AIE::mCRegClass, RC, SrcReg)) {
    // Spill high and low separately
    // FIXME : seems to write to the same location twice
    BuildMI(MBB, I, DL, get(AIE::ST_SPIL_GPR))
        .addReg(SrcReg, 0, AIE::sub_32_lo)
        .addFrameIndex(FI)
        .addMemOperand(CreateMMO(FI));
    BuildMI(MBB, I, DL, get(AIE::ST_SPIL_GPR))
        .addReg(SrcReg, getKillRegState(IsKill), AIE::sub_32_hi)
        .addFrameIndex(FI)
        .addMemOperand(CreateMMO(FI));
    return;
  } else if (regClassMatches(AIE::mMCMDRegClass, RC, SrcReg) ||
             regClassMatches(AIE::mCSRegClass, RC, SrcReg)) {
    // Can't spill these directly.  Need to bounce through a GPR.
    MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
    Register Reg = MRI.createVirtualRegister(&AIE::GPRRegClass);
    auto inst = regClassMatches(AIE::mMCMDRegClass, RC, SrcReg)
                    ? AIE::MV_SPECIAL2R
                    : AIE::MOV;
    BuildMI(MBB, I, DL, get(inst), Reg).addReg(SrcReg, getKillRegState(IsKill));
    BuildMI(MBB, I, DL, get(AIE::ST_SPIL_GPR))
        .addReg(Reg, getKillRegState(true))
        .addFrameIndex(FI)
        .addMemOperand(CreateMMO(FI));
    return;
  } else {
    // Opcode = AIE::ST_spis;
    LLVM_DEBUG(I->dump());
    llvm_unreachable("Can't store this register to stack slot: is it virtual?");
  }

  // To store a stack slot we generate a load indirect via the stack
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
void AIEInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                        MachineBasicBlock::iterator I,
                                        Register DstReg, int FI,
                                        const TargetRegisterClass *RC,
                                        const TargetRegisterInfo *TRI,
                                        Register VReg) const {
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

  if (regClassMatches(AIE::mRCmRegClass, RC, DstReg))
    Opcode = AIE::LDA_SPIL_GPR;
  else if (regClassMatches(AIE::PTRMODCSCBRegClass, RC, DstReg))
    Opcode = AIE::LDA_SPIL_PTR;
  else if (regClassMatches(AIE::VEC128RegClass, RC, DstReg))
    Opcode = AIE::VLDA_SPIL;
  else if (regClassMatches(AIE::VEC256RegClass, RC, DstReg))
    Opcode = AIE::WVLDA_SPIL;
  else if (regClassMatches(AIE::LRRegClass, RC, DstReg))
    Opcode = AIE::LR_LOAD; // Pseudo
  else if (regClassMatches(AIE::VEC512RegClass, RC, DstReg))
    Opcode = AIE::I512_LOAD; // Pseudo
  else if (regClassMatches(AIE::VEC1024RegClass, RC, DstReg))
    Opcode = AIE::I1024_LOAD; // Pseudo
  else if (regClassMatches(AIE::mCRegClass, RC, DstReg)) {
    // FIXME: reading the same location twice?
    BuildMI(MBB, I, DL, get(AIE::LDA_SPIL_GPR))
        .addReg(DstReg, RegState::DefineNoRead, AIE::sub_32_lo)
        .addFrameIndex(FI)
        .addMemOperand(CreateMMO(FI));
    BuildMI(MBB, I, DL, get(AIE::LDA_SPIL_GPR))
        .addReg(DstReg, RegState::DefineNoRead, AIE::sub_32_hi)
        .addFrameIndex(FI)
        .addMemOperand(CreateMMO(FI));
    return;
  } else if (regClassMatches(AIE::mMCMDRegClass, RC, DstReg) ||
             regClassMatches(AIE::mCSRegClass, RC, DstReg)) {
    // Can't spill these directly.  Need to bounce through a GPR.
    MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
    Register Reg = MRI.createVirtualRegister(&AIE::GPRRegClass);
    BuildMI(MBB, I, DL, get(AIE::LDA_SPIL_GPR), Reg)
        .addFrameIndex(FI)
        .addMemOperand(CreateMMO(FI));
    auto inst = regClassMatches(AIE::mMCMDRegClass, RC, DstReg)
                    ? AIE::MV_R2SPECIAL
                    : AIE::MOV;
    BuildMI(MBB, I, DL, get(inst), DstReg).addReg(Reg, getKillRegState(true));
    return;
  } else
    llvm_unreachable("Can't load this register from stack slot: is it virtual?");

  // To load from a stack slot we generate a load indirect via the
  // stack pointer.  The actual offset will be an immediate, but for
  // right now stuff in a virtual "FrameIndex" argument to represent
  // the offset that will be figured out later.  The offset is
  // generated by AIERegisterInfo::eliminateFrameIndex().
  BuildMI(MBB, I, DL, get(Opcode), DstReg)
      .addFrameIndex(FI)
      .addMemOperand(CreateMMO(FI));
}

unsigned AIEInstrInfo::getOppositeBranchOpcode(unsigned Opc) const {
  switch (Opc) {
  default:
    llvm_unreachable("Unrecognized conditional branch");
  case AIE::BEQZ:
    return AIE::BNEZ;
  case AIE::BNEZ:
    return AIE::BEQZ;
  }
}

unsigned AIEInstrInfo::getJumpOpcode() const { return AIE::PseudoBR; }

unsigned AIEInstrInfo::getOffsetMemOpcode(unsigned BaseMemOpcode) const {
  switch (BaseMemOpcode) {
  case TargetOpcode::G_STORE:
    return AIE::G_AIE_OFFSET_STORE;
  case TargetOpcode::G_LOAD:
    return AIE::G_AIE_OFFSET_LOAD;
  case TargetOpcode::G_SEXTLOAD:
    return AIE::G_AIE_OFFSET_SEXTLOAD;
  case TargetOpcode::G_ZEXTLOAD:
    return AIE::G_AIE_OFFSET_ZEXTLOAD;
  }
  llvm_unreachable("not a generic load/store");
}

std::optional<unsigned> AIEInstrInfo::getCombinedPostIncOpcode(
    MachineInstr &BaseMemI, MachineInstr &PostIncI, TypeSize Size) const {
  if (Size >= 512 || PostIncI.getOpcode() != TargetOpcode::G_PTR_ADD)
    return {};
  switch (BaseMemI.getOpcode()) {
  case TargetOpcode::G_STORE:
    return AIE::G_AIE_POSTINC_STORE;
  case TargetOpcode::G_LOAD:
    return AIE::G_AIE_POSTINC_LOAD;
  case TargetOpcode::G_SEXTLOAD:
    return AIE::G_AIE_POSTINC_SEXTLOAD;
  case TargetOpcode::G_ZEXTLOAD:
    return AIE::G_AIE_POSTINC_ZEXTLOAD;
  }
  llvm_unreachable("not a generic load/store");
}

unsigned AIEInstrInfo::getInstSizeInBytes(const MachineInstr &MI) const {
    unsigned Opcode = MI.getOpcode();

    switch (Opcode) {
    default: { return get(Opcode).getSize(); }
  // case TargetOpcode::EH_LABEL:
  // case TargetOpcode::IMPLICIT_DEF:
  // case TargetOpcode::KILL:
  // case TargetOpcode::DBG_VALUE:
  //   return 0;
  // case AIE::PseudoCALL:
  // case AIE::PseudoTAIL:
  // case AIE::PseudoLLA:
  //   return 8;
  // case TargetOpcode::INLINEASM:
  // case TargetOpcode::INLINEASM_BR: {
  //   const MachineFunction &MF = *MI.getParent()->getParent();
  //   const auto &TM = static_cast<const AIETargetMachine &>(MF.getTarget());
  //   return getInlineAsmLength(MI.getOperand(0).getSymbolName(),
  //                             *TM.getMCAsmInfo());
  // }
  }
}

std::pair<unsigned, unsigned>
AIEInstrInfo::decomposeMachineOperandsTargetFlags(unsigned TF) const {
  const unsigned Mask = AIEII::MO_DIRECT_FLAG_MASK;
  return std::make_pair(TF & Mask, TF & ~Mask);
}

ArrayRef<std::pair<unsigned, const char *>>
AIEInstrInfo::getSerializableDirectMachineOperandTargetFlags() const {
  using namespace AIEII;
  static const std::pair<unsigned, const char *> TargetFlags[] = {
      {MO_CALL, "aie-call"},
      {MO_GLOBAL, "aie-global"}};
  return ArrayRef(TargetFlags);
}

unsigned AIEInstrInfo::getReturnOpcode() const { return AIE::PseudoRET; }

unsigned AIEInstrInfo::getCallOpcode(const MachineFunction &CallerF,
                                     bool IsIndirect, bool IsTailCall) const {
  if (IsTailCall)
    llvm_unreachable("Tail calls not supported yet.\n");
  return IsIndirect ? AIE::JAL_IND : AIE::JAL;
}

bool AIEInstrInfo::isCall(unsigned Opc) const { return Opc == AIE::JAL; }

unsigned AIEInstrInfo::getNopOpcode(size_t Size) const {
  switch (Size) {
  case 0:
  case 2:
    return AIE::NOP;
  case 4:
    return AIE::NOP32;
  case 8:
    return AIE::NOP64;
  case 12:
    return AIE::NOP96;
  case 16:
    return AIE::NOP128;
  default:
    llvm_unreachable("Unsupported nop size.\n");
  }
  return AIE::NOP;
}

bool AIEInstrInfo::canHoistCheapInst(const MachineInstr &MI) const {
  // AIE1 would probably benefit from hoisting cross-banks copies like AIE2, but
  // QoR needs to be investigated before making the change.
  return false;
}
