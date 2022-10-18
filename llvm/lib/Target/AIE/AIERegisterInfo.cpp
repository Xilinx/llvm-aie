//===-- AIERegisterInfo.cpp - AIE Register Information ------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIE implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "AIERegisterInfo.h"
#include "AIE.h"
#include "AIESubtarget.h"
#include "MCTargetDesc/AIEMCTargetDesc.h"
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
#include "AIEGenRegisterInfo.inc"

using namespace llvm;

#define DEBUG_TYPE "aie-reg-info"

AIERegisterInfo::AIERegisterInfo(unsigned HwMode)
    : AIEGenRegisterInfo(AIE::SP, /*DwarfFlavour*/0, /*EHFlavor*/0,
                           /*PC*/0, HwMode) {}

const MCPhysReg *
AIERegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  //auto &Subtarget = MF->getSubtarget<AIESubtarget>();

  return CSR_AIE1_SaveList;
}

BitVector AIERegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  //const TargetFrameLowering *TFI = getFrameLowering(MF);
  BitVector Reserved(getNumRegs());

  // Use markSuperRegs to ensure any register aliases are also reserved

  // SP is only accessible by special instructions.  Without this the
  // instruction verifier complains because SP is always implicitly defined
  // and never killed by the instruction allocator.
  markSuperRegs(Reserved, AIE::SP);

  // LR is also always implicitly defined.
  markSuperRegs(Reserved, AIE::lr);
  assert(checkAllSuperRegsMarked(Reserved));

  // r15 is used to materialize constants for pseudo-instructions
  markSuperRegs(Reserved, AIE::r15);

  return Reserved;
}

const uint32_t *AIERegisterInfo::getNoPreservedMask() const {
  return CSR_NoRegs_RegMask;
}

bool AIERegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                            int SPAdj, unsigned FIOperandNum,
                                            RegScavenger *RS) const {
  assert(SPAdj == 0 && "Unexpected non-zero SPAdj value");

  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();
  MachineRegisterInfo &MRI = MF.getRegInfo();
  const AIEInstrInfo *TII = MF.getSubtarget<AIESubtarget>().getInstrInfo();
  const AIEFrameLowering *TFI = getFrameLowering(MF);
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

  LLVM_DEBUG(dbgs() << "eliminateFrameIndex in Function : " << MF.getName() << "\n");
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

  MachineBasicBlock &MBB = *MI.getParent();
  Register TargetPtrReg = MI.getOperand(0).getReg(); // Should be in PtrRegClass
  unsigned Opc = MI.getOpcode();
  switch (Opc) {
  case AIE::ST_spis_GPR:
  case AIE::ST_spis_PTR:
  case AIE::LDA_spis_GPR:
  case AIE::LDA_spis_PTR:
  case AIE::LDB_spis:
  case AIE::VST_spis:
  case AIE::WVST_spis:
  case AIE::VLDA_spis:
  case AIE::VLDB_spis:
  case AIE::WVLDA_spis:
  case AIE::WVLDB_spis:
  case AIE::ST_idx_GPR:
  case AIE::ST_idx_PTR:
  case AIE::LDA_idx_GPR:
  case AIE::LDA_idx_PTR:
  case AIE::LDB_idx:
  case AIE::VST_idx:
  case AIE::WVST_idx:
  case AIE::VLDA_idx:
  case AIE::VLDB_idx:
  case AIE::WVLDA_idx:
  case AIE::WVLDB_idx:
  case AIE::ST_SPIL_GPR:
  case AIE::ST_SPIL_PTR:
  case AIE::LDA_SPIL_GPR:
  case AIE::LDA_SPIL_PTR:
  case AIE::VST_SPIL:
  case AIE::WVST_SPIL:
  case AIE::VLDA_SPIL:
  case AIE::WVLDA_SPIL:
  case AIE::LR_LOAD:
  case AIE::I384_LOAD:
  case AIE::I512_LOAD:
  case AIE::I768_LOAD:
  case AIE::I1024_LOAD:
  case AIE::VLDA_SPIL_AM_LO_I48:
  case AIE::VLDA_SPIL_AM_HI_I48:
  case AIE::VLDA_AM_spis:
  case AIE::VLDA_AM_sp_imm:
  case AIE::I384_STORE:
  case AIE::I768_STORE:
  case AIE::I512_STORE:
  case AIE::I1024_STORE:
  case AIE::WVST_SPIL_AM_LO_I48:
  case AIE::WVST_SPIL_AM_HI_I48:
  case AIE::VST_SRSS_spis:
  case AIE::VST_SRSU_spis:
    // This is implicitly a stack spill instruction, so simply replace
    // the TargetFrameIndex operand with the right immediate offset.
    // Note that LDB path does not support SPIL instructions
    MI.getOperand(FIOperandNum).ChangeToImmediate(Offset);
    return false;
  case AIE::PADDA_pseudo:
    // This is a place where we probably have a GEP returning a reference to
    // something on the stack.  We need to generate and appropriate instruction
    // sequence depending on the amount of offset, fortunately, this is handled
    // by adjustReg already.
    int FrameIndexOffset = MI.getOperand(FIOperandNum+1).getImm();
    int RealOffset = Offset + FrameIndexOffset;

    if (FrameReg == AIE::SP) {
      // Can't move directly from SP to PTR.
      Register ScratchReg = MRI.createVirtualRegister(&AIE::GPRRegClass);
      BuildMI(MBB, II, DL, TII->get(AIE::MV_SPECIAL2R), ScratchReg)
        .addReg(FrameReg);
      BuildMI(MBB, II, DL, TII->get(AIE::MOV), TargetPtrReg).addReg(ScratchReg);
    } else
      BuildMI(MBB, II, DL, TII->get(AIE::MOV), TargetPtrReg).addReg(FrameReg);

    AIEFrameLowering::adjustReg(TII, MBB, II, DL, TargetPtrReg, RealOffset);
    MI.eraseFromParent();
    return false;
  }
  // Default case, we should really never get to.
  // This is a place where we probably have a GEP returning a reference to
  // something on the stack, but doesn't include one of the instructions above,
  // perhaps because the IR got optimized.  In this case, just emit an
  // expression with the stack pointer and deal with the fact that we don't have
  // an optimized instruction for it.
  Register ScratchReg = MRI.createVirtualRegister(&AIE::GPRRegClass);
  Register PtrReg = MRI.createVirtualRegister(&AIE::PTRRegClass);

  if(Offset == 0 && FrameReg != AIE::SP) {
    // Use the frame pointer directly.
    PtrReg = FrameReg;
  } else {
    if (FrameReg == AIE::SP) {
      BuildMI(MBB, II, DL, TII->get(AIE::MV_SPECIAL2R), ScratchReg)
        .addReg(FrameReg);
      BuildMI(MBB, II, DL, TII->get(AIE::MOV), PtrReg).addReg(ScratchReg);
    } else
      BuildMI(MBB, II, DL, TII->get(AIE::MOV), PtrReg).addReg(FrameReg);

    AIEFrameLowering::adjustReg(TII, MBB, II, DL, PtrReg, Offset);
  }

  MI.getOperand(FIOperandNum).ChangeToRegister(PtrReg,
                                               false,
                                               false,
                                               true/*IsKill*/);
  return false;
}

Register AIERegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  const AIEFrameLowering *TFI = getFrameLowering(MF);
  // There is no explicit frame pointer register.
  // Note that this needs to remain consistent with code in AIEFrameLowering.
  return TFI->hasFP(MF) ? AIE::p7 : AIE::SP;
}

const TargetRegisterClass *
AIERegisterInfo::getPointerRegClass(const MachineFunction &MF,
                                    unsigned Kind) const {
  // Generally speaking, all pointer operations have to go through PTR
  // registers.
  return &AIE::PTRRegClass;
}


const uint32_t *
AIERegisterInfo::getCallPreservedMask(const MachineFunction & MF,
                                        CallingConv::ID /*CC*/) const {
  //auto &Subtarget = MF.getSubtarget<AIESubtarget>();
  return CSR_AIE1_RegMask;
}

bool AIERegisterInfo::isTypeLegalForClass(const TargetRegisterClass &RC,
                                          LLT T) const {

  // This function is used by the machine verifier to infer a register class for
  // a physical register by checking for each register class that contains the
  // physical register whether it supports the type to be copied

  // Since the TableGen backends do not support pointer types in register
  // classes yet, we need to manually select the register class for pointer
  // types.

  // PTR registers obviously support pointers
  if (T == LLT::pointer(0, 20) && RC.getID() == AIE::PTRRegClass.getID())
    return true;

  // The stack pointer is copied to a virtual register when accessing the stack,
  // thus we need to add handling for it here, too
  if (T == LLT::pointer(0,20) && RC.getID() == AIE::SP_onlyRegClassID)
    return true;

  // Copying a s20 type to/from a physical register may only copy 20-bits,
  // otherwise the machine verifier complains about mismatching copy sizes.
  // Thus, we need to force s20 types to be legal only on PTR/MOD register
  // classes, as those registers have 20-bit sizes. That means we cannot copy a
  // s20 value to e.g. r6, we need to extend the value first to an s32 type.
  if (T == LLT::scalar(20)) {
    if ((RC.getID() == AIE::PTRRegClassID) ||
        (RC.getID() == AIE::MODRegClassID))
      return true;
    return false;
  }

  if (T == LLT::pointer(0, 20) && RC.getID() == AIE::MODRegClassID)
    return true;

  if (T == LLT::pointer(0, 20) && RC.getID() == AIE::mCSRegClassID)
    return true;

  return TargetRegisterInfo::isTypeLegalForClass(RC, T);
}

Register AIERegisterInfo::getStackPointerRegister() const { return AIE::SP; }
const TargetRegisterClass *
AIERegisterInfo::getGPRRegClass(const MachineFunction &MF) const {
  return &AIE::GPRRegClass;
}
