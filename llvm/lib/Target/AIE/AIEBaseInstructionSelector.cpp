//===- AIEBaseInstructionSelector.cpp ------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
/// \file
/// This file implements the targeting of the InstructionSelector class for
/// AIEngine.
//===----------------------------------------------------------------------===//

#include "AIEBaseInstructionSelector.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/MC/MCContext.h"

using namespace llvm;

#define DEBUG_TYPE "aiebase-isel"

AIEBaseInstructionSelector::AIEBaseInstructionSelector(
    const AIEBaseSubtarget &STI, const AIEBaseRegisterBankInfo &RBI)
    : InstructionSelector(), TII(*STI.getInstrInfo()),
      TRI(*static_cast<const AIEBaseRegisterInfo *>(STI.getRegisterInfo())),
      RBI(RBI) {}

void AIEBaseInstructionSelector::renderFrameIndex(MachineInstrBuilder &MIB,
                                                  const MachineInstr &MI,
                                                  int OpIdx) const {
  MIB.addFrameIndex((MI.getOperand(1).getIndex()));
}

void AIEBaseInstructionSelector::renderNegateImm(MachineInstrBuilder &MIB,
                                                 const MachineInstr &MI,
                                                 int OpIdx) const {
  assert(MI.getOpcode() == TargetOpcode::G_CONSTANT && OpIdx == -1 &&
         "Expected G_CONSTANT");
  MIB.addImm(-MI.getOperand(1).getCImm()->getSExtValue());
}

bool AIEBaseInstructionSelector::selectCopy(MachineInstr &I,
                                            MachineRegisterInfo &MRI) {

  Register DstReg = I.getOperand(0).getReg();
  if (DstReg.isPhysical())
    return true;

  const TargetRegisterClass *RC = nullptr;
  const RegClassOrRegBank &RCB = MRI.getRegClassOrRegBank(DstReg);
  if (const RegisterBank *RB = RCB.dyn_cast<const RegisterBank *>())
    RC = &TRI.getMinClassForRegBank(*RB, MRI.getType(DstReg));
  if (auto *TRC = RCB.dyn_cast<const TargetRegisterClass *>())
    RC = TRC;
  assert(RC != nullptr && "RC cannot be null");

  // No need to constrain SrcReg. It will get constrained when
  // we hit another of its uses or its defs.
  // Copies do not have constraints.
  if (!RBI.constrainGenericRegister(DstReg, *RC, MRI)) {
    LLVM_DEBUG(dbgs() << "Failed to constrain " << TII.getName(I.getOpcode())
                      << " operand\n");
    return false;
  }

  return true;
}

bool AIEBaseInstructionSelector::selectAddrInsn(MachineIRBuilder &MIB,
                                                MachineInstr &I,
                                                MachineRegisterInfo &MRI) {

  Register PtrOutReg = I.getOperand(0).getReg();
  Register CountOut1Reg = I.getOperand(1).getReg();

  auto IntrinsicID = cast<GIntrinsic>(I).getIntrinsicID();
  if (IntrinsicID == TII.getAddrIntrinsic2D()) {
    Register PtrInReg = I.getOperand(3).getReg();
    Register OffsetReg = I.getOperand(4).getReg();
    Register IncrReg = I.getOperand(5).getReg();
    Register SizeReg = I.getOperand(6).getReg();
    Register CountIn1Reg = I.getOperand(7).getReg();

    if (!RBI.constrainGenericRegister(CountOut1Reg, *TRI.getAddrCountRegClass(),
                                      MRI))
      return false;

    Register DReg =
        createDRegSequence(OffsetReg, IncrReg, SizeReg, CountIn1Reg, MRI);

    MachineInstrBuilder MI =
        MIB.buildInstr(TII.getPtrAdd2DOpcode(), {PtrOutReg, CountOut1Reg}, {})
            .addReg(PtrInReg)
            .addReg(DReg);

    I.eraseFromParent();
    return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
  } else if (IntrinsicID == TII.getAddrIntrinsic3D()) {

    Register CountOut2Reg = I.getOperand(2).getReg();
    Register PtrInReg = I.getOperand(4).getReg();
    Register OffsetReg = I.getOperand(5).getReg();
    Register Incr1Reg = I.getOperand(6).getReg();
    Register Incr2Reg = I.getOperand(7).getReg();
    Register Size1Reg = I.getOperand(8).getReg();
    Register CountIn1Reg = I.getOperand(9).getReg();
    Register Size2Reg = I.getOperand(10).getReg();
    Register CountIn2Reg = I.getOperand(11).getReg();

    if (!RBI.constrainGenericRegister(CountOut1Reg, *TRI.getAddrCountRegClass(),
                                      MRI) ||
        !RBI.constrainGenericRegister(CountOut2Reg, *TRI.getAddrCountRegClass(),
                                      MRI))
      return false;

    Register DReg =
        createDSRegSequence(OffsetReg, Incr1Reg, Incr2Reg, Size1Reg,
                            CountIn1Reg, Size2Reg, CountIn2Reg, MRI);

    MachineInstrBuilder MI =
        MIB.buildInstr(TII.getPtrAdd3DOpcode(),
                       {PtrOutReg, CountOut1Reg, CountOut2Reg}, {})
            .addReg(PtrInReg)
            .addReg(DReg);
    I.eraseFromParent();
    return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
  } else
    llvm_unreachable("Unexpected addressing intrinsic id");
}

bool AIEBaseInstructionSelector::selectSetLoopIterations(
    MachineInstr &I, MachineRegisterInfo &MRI, MachineIRBuilder &MIB) {
  auto ZOLSupport = TII.getZOLSupport();
  assert(ZOLSupport);
  auto LS = MIB.buildInstr(ZOLSupport->LoopStartOpcode, {}, {I.getOperand(1)})
                .addImm(0);
  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*LS, TII, TRI, RBI);
}

// Try to match BRCOND(Intrinsic::loop_decrement)
bool AIEBaseInstructionSelector::selectBrCondLoopDecrement(
    MachineInstr &BrCond, MachineRegisterInfo &MRI) {

  assert(BrCond.getOpcode() == TargetOpcode::G_BRCOND);
  auto ZOLSupport = TII.getZOLSupport();
  if (!ZOLSupport) {
    return false;
  }
  MachineOperand &MO = BrCond.getOperand(0);
  Register CondReg = MO.getReg();

  // Check if the condition is a LoopDecrement
  auto *LoopDec = getDefIgnoringCopies(CondReg, MRI);
  if (!LoopDec ||
      LoopDec->getOpcode() != TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS) {
    return false;
  }
  auto *LoopDecIntrinsic = cast<GIntrinsic>(LoopDec);
  if (LoopDecIntrinsic->getIntrinsicID() == Intrinsic::loop_decrement) {
    MachineBasicBlock *DestMBB = BrCond.getOperand(1).getMBB();
    MCContext &Context = BrCond.getParent()->getParent()->getContext();
    MCSymbol *EndLabel = Context.createNamedTempSymbol("_LEnd");
    MIB.buildInstr(ZOLSupport->LoopEndOpcode).addSym(EndLabel).addMBB(DestMBB);
    makeDeadMI(*LoopDec, MRI);
    BrCond.eraseFromParent();
    return true;
  }
  return false;
}

bool AIEBaseInstructionSelector::selectG_BRCOND(MachineInstr &I,
                                                MachineRegisterInfo &MRI) {
  // Try matching ZOL loop end
  if (selectBrCondLoopDecrement(I, MRI)) {
    return true;
  }
  // resort to TableGen'ed selection patterns
  return selectImpl(I, *CoverageInfo);
}

bool AIEBaseInstructionSelector::selectG_IMPLICIT_DEF(
    MachineInstr &I, MachineRegisterInfo &MRI) {
  I.setDesc(TII.get(TargetOpcode::IMPLICIT_DEF));
  // Make sure no input operands are passed to IMPLICIT_DEF
  while (I.getNumOperands() > 1)
    I.removeOperand(1);
  const Register DstReg = I.getOperand(0).getReg();
  const RegClassOrRegBank &RegClassOrBank = MRI.getRegClassOrRegBank(DstReg);
  const TargetRegisterClass *DstRC =
      RegClassOrBank.dyn_cast<const TargetRegisterClass *>();
  if (!DstRC) {
    const RegisterBank &RB = *RegClassOrBank.get<const RegisterBank *>();
    DstRC = &TRI.getMinClassForRegBank(RB, MRI.getType(DstReg));
  }
  return RBI.constrainGenericRegister(DstReg, *DstRC, MRI);
}

bool AIEBaseInstructionSelector::selectG_PHI(MachineInstr &I,
                                             MachineRegisterInfo &MRI) {
  const Register DstReg = I.getOperand(0).getReg();
  const RegClassOrRegBank &RegClassOrBank = MRI.getRegClassOrRegBank(DstReg);
  const TargetRegisterClass *DstRC =
      RegClassOrBank.dyn_cast<const TargetRegisterClass *>();
  if (!DstRC) {
    const RegisterBank &RB = *RegClassOrBank.get<const RegisterBank *>();
    DstRC = &TRI.getMinClassForRegBank(RB, MRI.getType(DstReg));
  }
  I.setDesc(TII.get(TargetOpcode::PHI));
  return RBI.constrainGenericRegister(DstReg, *DstRC, MRI);
}

void AIEBaseInstructionSelector::setUnsetCtrlRegister(
    MachineIRBuilder &MIB, MachineInstr &I, MachineInstr &EndI,
    MachineRegisterInfo &MRI, Register CRReg, Register ValueReg,
    unsigned DefaultCRVal) {
  auto Opcode = TII.getMvSclMultiSlotPseudoOpcode();
  // Set the crReg based on ValueReg parameter before I
  MIB.setInstr(I);
  if (auto Val = getIConstantVRegValWithLookThrough(ValueReg, MRI)) {
    unsigned ConstCRVal = Val->Value.getZExtValue();
    if (ConstCRVal == DefaultCRVal)
      return;
    MIB.buildInstr(Opcode, {CRReg}, {}).addImm(ConstCRVal);
  } else {
    auto CopyInstr =
        MIB.buildInstr(TargetOpcode::COPY, {CRReg}, {}).addReg(ValueReg);
    if (!selectCopy(*CopyInstr, MRI)) {
      dbgs() << "Failed to set and unset control register for: " << I << "\n";
      llvm_unreachable("Failed to set and unset control register");
    }
  }
  // Set the crReg based on DefaultCRVal after I
  MIB.setInstr(*EndI.getNextNode());
  MIB.buildInstr(Opcode, {CRReg}, {}).addImm(DefaultCRVal);
}

void AIEBaseInstructionSelector::setUnsetCtrlRegister(
    MachineIRBuilder &MIB, MachineInstr &I, MachineRegisterInfo &MRI,
    Register CRReg, Register ValueReg, unsigned DefaultCRVal) {
  setUnsetCtrlRegister(MIB, I, I, MRI, CRReg, ValueReg, DefaultCRVal);
}

MachineInstrBuilder
AIEBaseInstructionSelector::setCtrlRegister(MachineIRBuilder &MIB,
                                            Register CRReg, unsigned Val) {
  auto Opcode = TII.getMvSclMultiSlotPseudoOpcode();
  return MIB.buildInstr(Opcode, {CRReg}, {}).addImm(Val);
}

void AIEBaseInstructionSelector::addSplitMemOperands(
    MachineInstr &I, MachineInstrBuilder &Higher, MachineInstrBuilder &Lower,
    unsigned Offset, unsigned SplitFactor) {
  const MachineMemOperand *MMO =
      I.memoperands().size() == 1 ? *(I.memoperands().begin()) : nullptr;
  if (MMO) {
    llvm::LLT OrgType = MMO->getType();
    unsigned SplitElemCount = OrgType.getNumElements() / SplitFactor;
    unsigned ScalarSize = OrgType.getScalarType().getScalarSizeInBits();
    LLT PartType = LLT::fixed_vector(SplitElemCount, ScalarSize);
    Higher.addMemOperand(MF->getMachineMemOperand(
        MMO, Offset + ((SplitElemCount * ScalarSize) / 8), PartType));
    Lower.addMemOperand(MF->getMachineMemOperand(MMO, Offset + 0, PartType));
  }
}

AddressingModeInfo AIEBaseInstructionSelector::createAddressModeInfo(
    MachineInstr &MemI, MachineOperand &SrcDstOp, MachineOperand &PtrOp,
    std::optional<Register> OffsetReg, MachineRegisterInfo &MRI) {
  std::optional<ValueAndVReg> OffsetVVReg = {};
  std::optional<APInt> ImmediateOffset = {};
  if (OffsetReg &&
      mi_match(*OffsetReg, MRI, llvm::MIPatternMatch::m_GCst(OffsetVVReg))) {
    LLVM_DEBUG(dbgs() << "Found an immediate offset: "
                      << OffsetVVReg->Value.getSExtValue() << "\n");
    ImmediateOffset = OffsetVVReg->Value;
  }

  return {MemI, SrcDstOp, PtrOp, OffsetReg, ImmediateOffset};
}

void AIEBaseInstructionSelector::addAddressingMode(MachineInstrBuilder &MIB,
                                                   AddressingModeInfo &AMI,
                                                   bool FitsImmediateRange,
                                                   bool RenderFrameIndex,
                                                   MachineRegisterInfo &MRI) {
  using namespace TargetOpcode;

  MachineInstr *PtrDef = MRI.getVRegDef(AMI.PtrOp.getReg());
  // Only render frame index if we are dealing with an instruction that supports
  // it (we get that information in RenderFrameIndex from the callee) and if the
  // AMI does not have an offset or modifier register, which indicates that the
  // instruction is neither pre-increment nor post-increment
  if (RenderFrameIndex && !AMI.OffsetReg &&
      PtrDef->getOpcode() == G_FRAME_INDEX) {
    renderFrameIndex(MIB, *PtrDef, 1);
  } else if (FitsImmediateRange && AMI.ImmediateOffset) {
    MIB.addUse(AMI.PtrOp.getReg());
    MIB.addImm(AMI.ImmediateOffset->getSExtValue());
  } else if (AMI.OffsetReg) {
    MIB.addUse(AMI.PtrOp.getReg());
    MIB.addUse(*AMI.OffsetReg);
  } else {
    MIB.addUse(AMI.PtrOp.getReg());
  }
}
/// Create a REG_SEQUENCE instruction using the registers in \p Regs.
static MachineInstr &createTuple(Register DstReg, ArrayRef<Register> SrcRegs,
                                 ArrayRef<unsigned> SubRegs,
                                 MachineIRBuilder &MIB) {
  assert(SrcRegs.size() == SubRegs.size());
  auto RegSequence = MIB.buildInstr(TargetOpcode::REG_SEQUENCE, {DstReg}, {});
  for (unsigned I = 0, E = SrcRegs.size(); I < E; ++I) {
    RegSequence.addUse(SrcRegs[I]);
    RegSequence.addImm(SubRegs[I]);
  }
  return *RegSequence;
}

bool AIEBaseInstructionSelector::selectLRegSequence(MachineIRBuilder &MIB,
                                                    MachineInstr &I,
                                                    MachineRegisterInfo &MRI) {
  assert(I.getNumOperands() == 3);
  Register DstReg = I.getOperand(0).getReg();
  Register SrcReg1 = I.getOperand(1).getReg();
  Register SrcReg2 = I.getOperand(2).getReg();
  const RegisterBank &DstRB = *RBI.getRegBank(DstReg, MRI, TRI);
  const RegisterBank &Src1RB = *RBI.getRegBank(SrcReg1, MRI, TRI);
  const RegisterBank &Src2RB = *RBI.getRegBank(SrcReg2, MRI, TRI);
  const MachineFunction &MF = *I.getParent()->getParent();
  auto GPRRegBankID =
      RBI.getRegBankFromRegClass(*(TRI.getGPRRegClass(MF)), LLT()).getID();
  if (DstRB.getID() != GPRRegBankID || Src1RB.getID() != GPRRegBankID ||
      Src2RB.getID() != GPRRegBankID)
    return false;
  std::vector<unsigned> SubRegs;
  TRI.getTargetSubRegs(SubRegs, 32, DstRB);
  createTuple(DstReg, {SrcReg1, SrcReg2}, SubRegs, MIB);
  const RegisterBank &GPRBank =
      RBI.getRegBankFromRegClass(*(TRI.getGPRRegClass(MF)), LLT());
  for (MachineOperand &Op : I.operands()) {
    LLT Type = MRI.getType(Op.getReg());
    const TargetRegisterClass &RC = TRI.getMinClassForRegBank(GPRBank, Type);
    if (!RBI.constrainGenericRegister(Op.getReg(), RC, MRI))
      return false;
  }
  I.eraseFromParent();
  return true;
}

static void createSubRegCopies(ArrayRef<Register> DstRegs, Register SrcReg,
                               ArrayRef<unsigned> SubRegs,
                               MachineIRBuilder &MIB) {
  assert(DstRegs.size() == SubRegs.size());
  for (size_t Idx = 0; Idx != DstRegs.size(); ++Idx) {
    Register DstReg = DstRegs[Idx];
    unsigned SubReg = SubRegs[Idx];
    MIB.buildInstr(TargetOpcode::COPY, {DstReg}, {})
        .addReg(SrcReg, /*flags=*/0, SubReg);
  }
}

bool AIEBaseInstructionSelector::selectG_UNMERGE_VALUES(
    MachineIRBuilder &MIB, MachineInstr &I, MachineRegisterInfo &MRI) {
  assert(I.getNumOperands() == 3);
  const Register DstReg1 = I.getOperand(0).getReg();
  const Register DstReg2 = I.getOperand(1).getReg();
  const Register SrcReg = I.getOperand(2).getReg();

  const LLT Dst1Ty = MRI.getType(DstReg1);
  const RegisterBank &Dst1RB = *RBI.getRegBank(DstReg1, MRI, TRI);
  auto Size = Dst1Ty.getSizeInBits();
  std::vector<unsigned> SubRegs;
  TRI.getTargetSubRegs(SubRegs, Size, Dst1RB);
  assert(SubRegs.size() == 2 &&
         "Expected SrcReg to decompose into two sub-registers");
  createSubRegCopies({DstReg1, DstReg2}, SrcReg, {SubRegs[0], SubRegs[1]}, MIB);

  for (MachineOperand &Op : I.operands()) {
    LLT Type = MRI.getType(Op.getReg());
    const TargetRegisterClass &RC = TRI.getMinClassForRegBank(Dst1RB, Type);
    if (!RBI.constrainGenericRegister(Op.getReg(), RC, MRI))
      return false;
  }

  I.eraseFromParent();
  return true;
}

bool AIEBaseInstructionSelector::selectG_PTR_ADD(MachineIRBuilder &MIB,
                                                 MachineInstr &I,
                                                 MachineRegisterInfo &MRI) {
  Register DstReg = I.getOperand(0).getReg();
  Register Src1Reg = I.getOperand(1).getReg();
  Register Src2Reg = I.getOperand(2).getReg();

  const RegisterBank *DstRB = RBI.getRegBank(DstReg, MRI, TRI);
  const RegisterBank *Src1RB = RBI.getRegBank(Src1Reg, MRI, TRI);
  const RegisterBank *Src2RB = RBI.getRegBank(Src2Reg, MRI, TRI);

  // Pointer addition on GPRs is a simple ADD, and requires all operands in GPRs
  const unsigned TargetGPRRegBank = TRI.getGPRRegBankID();
  if (DstRB->getID() == TargetGPRRegBank) {
    if (Src1RB->getID() != TargetGPRRegBank ||
        Src1RB->getID() != Src2RB->getID())
      return false;

    // FIXME: Constants on the RHS could be folded into the ADD instruction by
    // relying on the TableGen patterns for G_ADD on GPRRegbank
    MachineInstr &MI =
        *MIB.buildInstr(TII.getAddSclOpcode(), {DstReg}, {Src1Reg, Src2Reg});
    I.eraseFromParent();
    return constrainSelectedInstRegOperands(MI, TII, TRI, RBI);
  }

  // Standard PTR bank case handled through patterns.
  return selectImpl(I, *CoverageInfo);
}

bool AIEBaseInstructionSelector::selectGetSS(MachineInstr &I,
                                             MachineRegisterInfo &MRI,
                                             MachineIRBuilder &MIB) {
  Register ValReg = I.getOperand(0).getReg();
  Register StatusReg = I.getOperand(1).getReg();
  // In this case of G_INTRINSIC operand 2 is target intrinsic

  unsigned OpCode = TII.getOpCode(I);
  MachineInstrBuilder MI = MIB.buildInstr(OpCode, {ValReg}, {});

  auto CopyInstr = MIB.buildInstr(TargetOpcode::COPY, {StatusReg},
                                  {Register(TII.getSSStatusReg())});
  if (!selectCopy(*CopyInstr, MRI)) {
    return false;
  }

  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
}

bool AIEBaseInstructionSelector::selectPutMSB(MachineInstr &I,
                                              MachineRegisterInfo &MRI,
                                              MachineIRBuilder &MIB) {
  // In this case of G_INTRINSIC operand 0 is target intrinsic
  Register ValReg = I.getOperand(1).getReg();
  Register TLastReg = I.getOperand(2).getReg();
  auto TLastVal = getIConstantVRegValWithLookThrough(TLastReg, MRI);
  unsigned OpCode = TII.getMvScl2MSTlastRegOpcode();
  if (TLastVal) {
    unsigned ConstTLastVal = TLastVal->Value.getZExtValue();
    OpCode = TII.getMvScl2MS(ConstTLastVal);
  }
  MachineInstrBuilder MI = MIB.buildInstr(OpCode, {}, {ValReg});
  if (!TLastVal) {
    MI.addReg(TLastReg);
  }

  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
}

bool AIEBaseInstructionSelector::selectPutMSNB(MachineInstr &I,
                                               MachineRegisterInfo &MRI,
                                               MachineIRBuilder &MIB) {
  Register StatusReg = I.getOperand(0).getReg();
  // In this case of G_INTRINSIC operand 1 is target intrinsic
  Register ValReg = I.getOperand(2).getReg();
  Register TLastReg = I.getOperand(3).getReg();
  auto TLastVal = getIConstantVRegValWithLookThrough(TLastReg, MRI);
  unsigned OpCode = TII.getMvNBScl2MSTlastRegOpcode();
  if (TLastVal) {
    unsigned ConstTLastVal = TLastVal->Value.getZExtValue();
    OpCode = TII.getMvNBScl2MS(ConstTLastVal);
  }
  MachineInstrBuilder MI = MIB.buildInstr(OpCode, {}, {ValReg});
  if (!TLastVal) {
    MI.addReg(TLastReg);
  }

  auto CopyInstr = MIB.buildInstr(TargetOpcode::COPY, {StatusReg},
                                  {Register(TII.getMSStatusReg())});
  if (!selectCopy(*CopyInstr, MRI)) {
    return false;
  }

  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
}

bool AIEBaseInstructionSelector::selectVMAXDIFF_LT(MachineInstr &I,
                                                   MachineRegisterInfo &MRI,
                                                   MachineIRBuilder &MIB) {

  Register DstReg = I.getOperand(0).getReg();
  Register CmpReg = I.getOperand(1).getReg();
  // In this case of G_INTRINSIC operand 2 is target intrinsic
  Register Src1Reg = I.getOperand(3).getReg();
  Register Src2Reg = I.getOperand(4).getReg();
  Register SignReg = I.getOperand(5).getReg();

  if (auto SignVal = getIConstantVRegSExtVal(SignReg, MRI)) {
    // Handle constant sign through instruction patterns
    return selectImpl(I, *CoverageInfo);
  }

  unsigned OpCode = TII.getOpCode(I);
  MachineInstrBuilder MI = MIB.buildInstr(OpCode, {DstReg, CmpReg}, {})
                               .addReg(Src1Reg)
                               .addReg(Src2Reg);

  setUnsetCtrlRegister(MIB, *MI, MRI, TII.getVaddSignControlRegister(),
                       SignReg);

  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
}

bool AIEBaseInstructionSelector::selectVABS_GTZ(MachineInstr &I,
                                                MachineRegisterInfo &MRI,
                                                MachineIRBuilder &MIB) {

  Register DstReg = I.getOperand(0).getReg();
  Register CmpReg = I.getOperand(1).getReg();
  // In this case of G_INTRINSIC operand 2 is target intrinsic
  Register SrcReg = I.getOperand(3).getReg();
  Register SignReg = I.getOperand(4).getReg();

  if (auto SignVal = getIConstantVRegSExtVal(SignReg, MRI)) {
    // Handle constant sign through instruction patterns
    return selectImpl(I, *CoverageInfo);
  }

  unsigned OpCode = TII.getOpCode(I);
  MachineInstrBuilder MI =
      MIB.buildInstr(OpCode, {DstReg, CmpReg}, {}).addReg(SrcReg);

  setUnsetCtrlRegister(MIB, *MI, MRI, TII.getVaddSignControlRegister(),
                       SignReg);

  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
}

bool AIEBaseInstructionSelector::selectVSUB_LTGE(MachineInstr &I,
                                                 MachineRegisterInfo &MRI,
                                                 MachineIRBuilder &MIB) {

  Register DstReg = I.getOperand(0).getReg();
  Register CmpReg = I.getOperand(1).getReg();
  // In this case of G_INTRINSIC operand 2 is target intrinsic
  Register Src1Reg = I.getOperand(3).getReg();
  Register Src2Reg = I.getOperand(4).getReg();
  Register SignReg = I.getOperand(5).getReg();

  if (auto SignVal = getIConstantVRegSExtVal(SignReg, MRI)) {
    // Handle constant sign through instruction patterns
    return selectImpl(I, *CoverageInfo);
  }

  unsigned OpCode = TII.getOpCode(I);
  MachineInstrBuilder MI = MIB.buildInstr(OpCode, {DstReg, CmpReg}, {})
                               .addReg(Src1Reg)
                               .addReg(Src2Reg);

  setUnsetCtrlRegister(MIB, *MI, MRI, TII.getVaddSignControlRegister(),
                       SignReg);

  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
}

bool AIEBaseInstructionSelector::selectVCompare(MachineInstr &I,
                                                MachineRegisterInfo &MRI,
                                                MachineIRBuilder &MIB) {

  Register CmpReg = I.getOperand(0).getReg();
  // In this case of G_INTRINSIC operand 1 is target intrinsic
  Register Src1Reg = I.getOperand(2).getReg();
  Register Src2Reg = I.getOperand(3).getReg();
  Register SignReg = I.getOperand(4).getReg();

  // Handle constant sign through instruction patterns
  if (selectImpl(I, *CoverageInfo)) {
    return true;
  }

  unsigned OpCode = TII.getOpCode(I);
  MachineInstrBuilder MI =
      MIB.buildInstr(OpCode, {CmpReg}, {}).addReg(Src1Reg).addReg(Src2Reg);

  setUnsetCtrlRegister(MIB, *MI, MRI, TII.getVaddSignControlRegister(),
                       SignReg);

  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
}

bool AIEBaseInstructionSelector::selectVSUB_MIN_MAX(MachineInstr &I,
                                                    MachineRegisterInfo &MRI,
                                                    MachineIRBuilder &MIB) {

  Register DstReg = I.getOperand(0).getReg();
  Register CmpReg = I.getOperand(1).getReg();
  // In this case of G_INTRINSIC operand 2 is target intrinsic
  Register Src1Reg = I.getOperand(3).getReg();
  Register Src2Reg = I.getOperand(4).getReg();
  Register SignReg = I.getOperand(5).getReg();

  if (auto SignVal = getIConstantVRegValWithLookThrough(SignReg, MRI)) {
    // Handle constant sign through instruction patterns
    return selectImpl(I, *CoverageInfo);
  }

  unsigned OpCode = TII.getOpCode(I);
  MachineInstrBuilder MI = MIB.buildInstr(OpCode, {DstReg, CmpReg}, {})
                               .addReg(Src1Reg)
                               .addReg(Src2Reg);

  setUnsetCtrlRegister(MIB, *MI, MRI, TII.getVaddSignControlRegister(),
                       SignReg);

  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
}

void AIEBaseInstructionSelector::buildUnpack(MachineInstr &I,
                                             MachineRegisterInfo &MRI,
                                             MachineIRBuilder &MIB,
                                             MachineInstrBuilder &MI) {
  Register DstReg = I.getOperand(0).getReg();
  // In this case of G_INTRINSIC operand 1 is target intrinsic
  Register SrcReg = I.getOperand(2).getReg();
  Register SignReg = I.getOperand(3).getReg();

  unsigned OpCode = TII.getOpCode(I);
  MI = MIB.buildInstr(OpCode, {DstReg}, {}).addReg(SrcReg);
  auto Sign = getIConstantVRegValWithLookThrough(SignReg, MRI);
  if (!Sign)
    setUnsetCtrlRegister(MIB, *MI, MRI, TII.getUnpackSignCReg(), SignReg);
}

void AIEBaseInstructionSelector::buildPack(MachineInstr &I,
                                           MachineRegisterInfo &MRI,
                                           MachineIRBuilder &MIB,
                                           MachineInstrBuilder &MI) {
  Register DstReg = I.getOperand(0).getReg();
  // In this case of G_INTRINSIC_W_SIDE_EFFECTS, operand 1 is target intrinsic.
  Register SrcReg = I.getOperand(2).getReg();
  Register SignReg = I.getOperand(3).getReg();

  unsigned OpCode = TII.getOpCode(I);
  MI = MIB.buildInstr(OpCode, {DstReg}, {}).addReg(SrcReg);
  auto Sign = getIConstantVRegValWithLookThrough(SignReg, MRI);
  if (!Sign)
    setUnsetCtrlRegister(MIB, *MI, MRI, TII.getPackSignCReg(), SignReg);
}

bool AIEBaseInstructionSelector::canCombineCONV(MachineInstr &MemOp,
                                                MachineInstr &CombOp) {
  const std::optional<APInt> NoImmediate = {};
  return getCombinedOpcodeCONV(MemOp, CombOp, NoImmediate).has_value();
}

std::optional<LoadStoreOpcodes>
AIEBaseInstructionSelector::getCombinedOpcodeCONV(
    const MachineInstr &MemOp, const MachineInstr &CombOp,
    const std::optional<APInt> Immediate) {
  return {};
}

bool AIEBaseInstructionSelector::canCombineCONVLoad(MachineInstr &MemOp,
                                                    MachineInstr &CombOp) {
  const std::optional<APInt> NoImmediate = {};
  return getCombinedOpcodeCONVLoad(MemOp, CombOp, NoImmediate).has_value();
}

bool AIEBaseInstructionSelector::selectG_AIE_LOAD_CONV(
    MachineInstr &CONVI, MachineRegisterInfo &MRI) {
  Register LoadResult = (std::next(CONVI.uses().begin()))->getReg();
  MachineInstr *LoadOp = getDefIgnoringCopiesAndBitcasts(LoadResult, MRI);
  assert(LoadOp && "Expected SSA.");

  // Do not try to combine if one of the load's defs is used by another
  // instruction between the load and the VCONV or if there is a store
  // between the load and the VCONV.
  if (!canDelayMemOp(*LoadOp, CONVI, MRI))
    return false;

  if (!canCombineCONVLoad(*LoadOp, CONVI) ||
      LoadOp->getParent() != CONVI.getParent() || !MRI.hasOneUse(LoadResult))
    return false;

  std::optional<AddressingModeInfo> AMI =
      getOrDefineAddressingRegister(*LoadOp, MRI);

  if (!AMI)
    return false;

  std::optional<LoadStoreOpcodes> LSO =
      getCombinedOpcodeCONVLoad(AMI->MemI, CONVI, AMI->ImmediateOffset);

  Register DstReg = CONVI.getOperand(0).getReg();

  auto NewInstr = MIB.buildInstr(LSO->ISelOpcode);

  NewInstr.addDef(DstReg);

  for (auto *Def = std::next(AMI->MemI.defs().begin());
       Def != AMI->MemI.defs().end(); ++Def)
    NewInstr.addDef(Def->getReg());

  addAddressingMode(NewInstr, *AMI, LSO->FitsImmediateRange, false, MRI);

  NewInstr.cloneMemRefs(AMI->MemI);

  CONVI.eraseFromParent();

  // Erasing the load instruction breaks later on in the selection code. That is
  // because an iterator is kept on erased instructions. This breaks while
  // trying to eliminate a trivially dead instruction which requires access to
  // its memory operands which have been erased, thus leading to a seg fault. To
  // remedy this, we keep the load to be removed by the trivial dead code
  // elimination and we make sure to assign new virtual register definitions to
  // its live operands to respect SSA.
  makeDeadMI(*LoadOp, MRI);

  return constrainSelectedInstRegOperands(*NewInstr.getInstr(), TII, TRI, RBI);
}

bool AIEBaseInstructionSelector::selectVCONV(MachineInstr &I,
                                             MachineRegisterInfo &MRI) {
  // Try to match CONV combine
  if (selectG_AIE_LOAD_CONV(I, MRI))
    return true;
  // Resort to TableGen'ed selection patterns
  return selectImpl(I, *CoverageInfo);
}

std::optional<AddressingModeInfo>
AIEBaseInstructionSelector::getOrDefineAddressingRegister(
    MachineInstr &MemI, MachineRegisterInfo &MRI) {
  return {};
}

std::optional<LoadStoreOpcodes>
AIEBaseInstructionSelector::getCombinedOpcodeCONVLoad(
    const MachineInstr &MemOp, const MachineInstr &CombOp,
    const std::optional<APInt> Immediate) {
  return {};
}

// Make an instruction trivially dead by creating and distributing new virtual
// registers to its defs
void AIEBaseInstructionSelector::makeDeadMI(MachineInstr &MI,
                                            MachineRegisterInfo &MRI) {
  if (MI.getOpcode() == TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS) {
    MI.setDesc(TII.get(TargetOpcode::G_INTRINSIC));
  }

  for (auto *Def = MI.defs().begin(); Def != MI.defs().end(); ++Def) {
    Register NewReg = MRI.cloneVirtualRegister(Def->getReg());
    Def->setReg(NewReg);
  }
}
