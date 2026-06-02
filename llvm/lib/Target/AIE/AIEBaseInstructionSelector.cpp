//===- AIEBaseInstructionSelector.cpp ------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
/// \file
/// This file implements the targeting of the InstructionSelector class for
/// AIEngine.
//===----------------------------------------------------------------------===//

#include "AIEBaseInstructionSelector.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/MC/MCContext.h"
#include <optional>

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

// Try to match BRCOND(Intrinsic::loop_decrement)
bool AIEBaseInstructionSelector::selectBrCondLoopDecrementReg(
    MachineInstr &BrCond, MachineRegisterInfo &MRI) {

  assert(BrCond.getOpcode() == TargetOpcode::G_BRCOND && "Not a G_BRCOND");

  auto JNZDSupport = TII.getJNZDSupport();
  if (!JNZDSupport) {
    LLVM_DEBUG(dbgs() << "JNZD loop selection not supported\n");
    return false;
  }

  Register CondReg = BrCond.getOperand(0).getReg();
  const unsigned CondRB = RBI.getRegBank(CondReg, MRI, TRI)->getID();

  // The condition needs to reside in GPRs
  if (CondRB != TRI.getGPRRegBankID())
    return false;

  auto *Cond = getDefIgnoringCopies(CondReg, MRI);

  assert(Cond && "Conditional branch without a condition!?");

  struct JNZDArgs {
    Register NewLC;
    Register PrevLC;
    MachineInstr *IntrinInst;
  };
  // This checks for the following pattern:
  // bb.loop.body:
  //  %newLC:gprregbank(s32) = llvm.loop.decrement.reg, %prevLC(s32), 1
  //  %cond:gprregbank(s32) = G_ICMP intpred(ne), %newLC(s32), 0
  //  G_BRCOND %4(s32), %bb.loop.body
  auto IsJNZDPattern =
      [](const MachineInstr &MI,
         const MachineRegisterInfo &MRI) -> std::optional<JNZDArgs> {
    if (MI.getOpcode() != TargetOpcode::G_ICMP)
      return std::nullopt;

    const auto &PredOp = MI.getOperand(1);
    const auto Pred = static_cast<CmpInst::Predicate>(PredOp.getPredicate());

    if (Pred != CmpInst::ICMP_NE)
      return std::nullopt;

    auto CmpRHS =
        getIConstantVRegValWithLookThrough(MI.getOperand(3).getReg(), MRI);
    if (!CmpRHS || CmpRHS->Value != 0)
      return std::nullopt;

    auto *CmpLHS = MRI.getVRegDef(MI.getOperand(2).getReg());
    if (CmpLHS->getOpcode() != TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS)
      return std::nullopt;

    const unsigned IntrinID = cast<GIntrinsic>(*CmpLHS).getIntrinsicID();
    if (IntrinID != Intrinsic::loop_decrement_reg)
      return std::nullopt;

    JNZDArgs Args;
    Args.NewLC = CmpLHS->getOperand(0).getReg();
    Args.PrevLC = CmpLHS->getOperand(2).getReg();
    Args.IntrinInst = CmpLHS;
    return Args;
  };

  if (auto Args = IsJNZDPattern(*Cond, MRI)) {
    Register BlockAddrReg =
        MRI.createVirtualRegister(JNZDSupport->PointerRegisterClass);
    MachineBasicBlock *DestMBB = BrCond.getOperand(1).getMBB();
    MIB.buildInstr(JNZDSupport->MovBlockAddrOpcode, {BlockAddrReg}, {})
        .addMBB(DestMBB);
    auto LoopDec = MIB.buildInstr(JNZDSupport->LoopDecOpcode, {Args->NewLC},
                                  {Args->PrevLC});
    MIB.buildInstr(JNZDSupport->LoopJNZOpcode, {},
                   {LoopDec->getOperand(0).getReg(), BlockAddrReg});

    BrCond.eraseFromParent();
    makeDeadMI(*Args->IntrinInst, MRI);
    return constrainSelectedInstRegOperands(*LoopDec, TII, TRI, RBI);
  }

  return false;
}

bool AIEBaseInstructionSelector::selectG_BRCOND(MachineInstr &I,
                                                MachineRegisterInfo &MRI) {
  // Try matching JNZD loop end
  if (selectBrCondLoopDecrementReg(I, MRI)) {
    return true;
  }
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
    const RegisterBank &RB = *cast<const RegisterBank *>(RegClassOrBank);
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
    const RegisterBank &RB = *cast<const RegisterBank *>(RegClassOrBank);
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

MachineInstrBuilder AIEBaseInstructionSelector::setStatusRegister(
    MachineIRBuilder &MIB, Register StatusReg, unsigned Val) {
  auto Opcode = TII.getSetStatusRegisterOpcode();
  return MIB.buildInstr(Opcode, {StatusReg}, {}).addImm(Val);
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
  unsigned OpCode = TII.getOpCode(I);
  if (TLastVal) {
    unsigned ConstTLastVal = TLastVal->Value.getZExtValue();
    OpCode = TII.getMoveToMSOpcode(I, ConstTLastVal);
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
  unsigned OpCode = TII.getOpCode(I);
  if (TLastVal) {
    unsigned ConstTLastVal = TLastVal->Value.getZExtValue();
    OpCode = TII.getMoveToMSOpcode(I, ConstTLastVal);
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

bool AIEBaseInstructionSelector::canCombineCONVStore(MachineInstr &MemOp,
                                                     MachineInstr &CombOp) {
  const std::optional<APInt> NoImmediate = {};
  return getCombinedOpcodeCONVStore(MemOp, CombOp, NoImmediate).has_value();
}

std::optional<LoadStoreOpcodes>
AIEBaseInstructionSelector::getCombinedOpcodeCONVStore(
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
  Register LoadResult = CONVI.getOperand(2).getReg();
  MachineInstr *LoadOp = getDefIgnoringCopiesAndBitcasts(LoadResult, MRI);
  assert(LoadOp && "Expected SSA.");
  MachineInstr *InsertionPoint = &CONVI;

  // We can try to advance the combined
  // instruction to the load's position.
  if (canAdvanceOp(*LoadOp, CONVI, MRI)) {
    InsertionPoint = LoadOp;
  } else if (!canDelayMemOp(*LoadOp, CONVI, MRI)) {
    // Do not try to combine if one of the load's defs is used by another
    // instruction between the load and the VCONV or if there is a store
    // between the load and the VCONV.
    return false;
  }

  if (!canCombineCONVLoad(*LoadOp, CONVI) ||
      LoadOp->getParent() != CONVI.getParent() || !MRI.hasOneUse(LoadResult))
    return false;

  MIB.setInstr(*InsertionPoint);

  std::optional<AddressingModeInfo> AMI =
      getOrDefineAddressingRegister(*LoadOp, MRI);

  if (!AMI) {
    if (InsertionPoint != &CONVI)
      MIB.setInstr(CONVI);
    return false;
  }

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

bool AIEBaseInstructionSelector::selectG_AIE_LOAD_UNPACK(
    MachineInstr &UNPACKI, MachineRegisterInfo &MRI) {
  Register LoadResult = (std::next(UNPACKI.uses().begin()))->getReg();
  MachineInstr *LoadOp = getDefIgnoringCopiesAndBitcasts(LoadResult, MRI);
  if (!LoadOp)
    return false;

  // Should we build the instruction at load's position?
  bool ShouldAdvanceOp = false;

  // Do not try to combine if one of the load's defs is used by another
  // instruction between the load and the VUNPACK or if there is a store
  // between the load and the VUNPACK.
  if (!canDelayMemOp(*LoadOp, UNPACKI, MRI)) {
    // If we cannot delay the load, we can try to advance the combined
    // instruction to the load's position.
    if (canAdvanceOp(*LoadOp, UNPACKI, MRI))
      ShouldAdvanceOp = true;
    else
      return false;
  }

  if (!canCombineUNPACKLoad(*LoadOp, UNPACKI, MRI))
    return false;

  std::optional<AddressingModeInfo> AddrModeInfo =
      getOrDefineAddressingRegister(*LoadOp, MRI);
  if (!AddrModeInfo)
    return false;

  Register DstReg = UNPACKI.getOperand(0).getReg();
  // In this case of G_INTRINSIC operand 1 is target intrinsic
  // In this case the operand 2 is the source register which is the loaded value
  Register SignReg = UNPACKI.getOperand(3).getReg();

  auto SignVal = getIConstantVRegValWithLookThrough(SignReg, MRI);
  bool ConstantSign = SignVal.has_value();
  std::optional<LoadStoreOpcodes> LSO = getCombinedOpcodeUNPACKLoad(
      *LoadOp, UNPACKI, AddrModeInfo->ImmediateOffset,
      ConstantSign ? SignVal.value().Value == 0x1 : false);

  assert(LSO && "Unexpected VLDB.UNPACK combine failure");

  if (ShouldAdvanceOp)
    MIB.setInstr(*LoadOp);
  else
    MIB.setInstr(UNPACKI);

  setUnpackSizeRegister(MIB, cast<GIntrinsic>(UNPACKI).getIntrinsicID());

  auto NewInstr = MIB.buildInstr(LSO->ISelOpcode);

  NewInstr.addDef(DstReg);

  for (auto *Def = std::next(LoadOp->defs().begin());
       Def != LoadOp->defs().end(); ++Def) {
    NewInstr.addDef(Def->getReg());
  }

  addAddressingMode(NewInstr, *AddrModeInfo, LSO->FitsImmediateRange, false,
                    MRI);

  NewInstr.cloneMemRefs(*LoadOp);

  if (!ConstantSign)
    setUnsetCtrlRegister(MIB, *NewInstr, MRI, TRI.getUnpackSignCtrlReg(),
                         SignReg);

  UNPACKI.eraseFromParent();
  makeDeadMI(*LoadOp, MRI);

  return constrainSelectedInstRegOperands(*NewInstr.getInstr(), TII, TRI, RBI);
}

bool AIEBaseInstructionSelector::selectG_AIE_STORE_PACK(
    MachineInstr &StoreI, MachineRegisterInfo &MRI) {

  Register PackResult = (StoreI.uses().begin())->getReg();
  MachineInstr *PackOp = getDefIgnoringCopiesAndBitcasts(PackResult, MRI);

  if (!canCombinePACK(StoreI, *PackOp, MRI))
    return false;

  std::optional<AddressingModeInfo> AddrModeInfo =
      getOrDefineAddressingRegister(StoreI, MRI);
  if (!AddrModeInfo)
    return false;

  // Note: Operand 1 is the ID of the intrinsic
  Register SrcReg = PackOp->getOperand(2).getReg();
  Register SignReg = PackOp->getOperand(3).getReg();

  unsigned MemOpLoadStoreSize = getLoadStoreSize(StoreI);
  TypeSize SrcRegSize = MRI.getType(SrcReg).getSizeInBits();
  assert(((MemOpLoadStoreSize == 256 && SrcRegSize == 512) ||
          (MemOpLoadStoreSize == 512 && SrcRegSize == 1024)) &&
         "Unexpected VST.PACK size");

  auto SignVal = getIConstantVRegValWithLookThrough(SignReg, MRI);
  bool ConstantSign = SignVal ? true : false;
  // SignVal = 1 for signed and 0 for dynamically signed
  std::optional<LoadStoreOpcodes> LSO = getCombinedOpcodePACK(
      StoreI, *PackOp, AddrModeInfo->ImmediateOffset,
      ConstantSign ? SignVal.value().Value == 0x1 : false);

  assert(LSO && "Unexpected VST.PACK combine failure");

  // Note: the output size (I8 or I4) is not encoded as part of the instruction,
  // but it is read from the crPackSize register.
  auto NewInstr = MIB.buildInstr(LSO->ISelOpcode);

  for (auto Def : StoreI.defs())
    NewInstr.addDef(Def.getReg());

  NewInstr.addUse(SrcReg);

  addAddressingMode(NewInstr, *AddrModeInfo, LSO->FitsImmediateRange, false,
                    MRI);

  NewInstr.cloneMemRefs(StoreI);

  // Set the crPackSize before NewInstr
  // Selects the size of the Pack instructions
  // 0 – Destination is 4 bits
  // 1 – Destination is 8 bits
  Register PackSizeReg = TRI.getPackSizeCtrlReg();
  if (PackSizeReg.isValid()) {
    const bool Is8Bit =
        isPackI8Intrinsic(cast<GIntrinsic>(PackOp)->getIntrinsicID());
    auto Opcode = TII.getMvSclMultiSlotPseudoOpcode();
    MIB.setInstr(*NewInstr);
    MIB.buildInstr(Opcode, {PackSizeReg}, {}).addImm((unsigned)Is8Bit);
  }

  Register PackSignReg = TRI.getPackSignCtrlReg();
  if (!ConstantSign && PackSignReg.isValid())
    setUnsetCtrlRegister(MIB, *NewInstr, MRI, PackSignReg, SignReg);

  StoreI.eraseFromParent();
  makeDeadMI(*PackOp, MRI);
  return constrainSelectedInstRegOperands(*NewInstr.getInstr(), TII, TRI, RBI);
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
// registers to its defs.
// Erasing the load instruction breaks later on in the selection code. That is
// because we keep an iterator on erased instructions. This breaks while
// trying to eliminate a trivially dead instruction which requires access to
// its memory operands which have been erased, thus leading to a seg fault. To
// remedy this, we keep the load to be removed by the trivial dead code
// elimination and we make sure to assign a new virtual register definition to
// its live operands to respect SSA.
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

void AIEBaseInstructionSelector::insertPtrAddForOffset(MachineRegisterInfo &MRI,
                                                       MachineInstr &MemI) {
  // The offset is not an immediate or the immediate does not fit the immediate
  // range. Instruction select PTR_ADD for the splitting of instruction. E.g.:
  // $x0 = G_AIE_OFFSET_LOAD %ptr, %offset has to be selected to
  // %new_ptr = PTR_ADD %ptr, %offset
  // $wh0 = VLDA_dmw_lda_w_ag_idx_imm %new_ptr, #32
  // $wl0 = VLDA_dmw_lda_w_ag_idx_imm %new_ptr, #0

  // This function only gets called for G_AIE_OFFSET_LOAD AND G_AIE_OFFSET_STORE
  // Both instruction have the pointer and the offset in the same operands
  assert(TII.isGenericOffsetMemOpcode(MemI.getOpcode()) &&
         "Unexpected instruction in instrPtrAddForOffset");
  const unsigned PointerRegIndex = 1;
  const unsigned OffsetRegIndex = 2;

  Register NewPtrReg =
      MRI.cloneVirtualRegister(MemI.getOperand(PointerRegIndex).getReg());
  MachineInstrBuilder NewPtr =
      MIB.buildInstr(TargetOpcode::G_PTR_ADD)
          .addDef(NewPtrReg)
          .addReg(MemI.getOperand(PointerRegIndex).getReg())
          .addReg(MemI.getOperand(OffsetRegIndex).getReg());

  if (!selectImpl(*NewPtr.getInstr(), *CoverageInfo))
    llvm_unreachable("Unexpected failure selecting G_PTR_ADD");

  MemI.getOperand(PointerRegIndex).setReg(NewPtrReg);
}

bool AIEBaseInstructionSelector::selectStartLoop(MachineInstr &I,
                                                 MachineRegisterInfo &MRI) {

  assert(I.getOpcode() == TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS);

  auto JNZDSupport = TII.getJNZDSupport();
  if (!JNZDSupport) {
    return false;
  }

  const Register DstReg = I.getOperand(0).getReg();
  // The first argument to start_loop_iterations is the loop trip count.
  // We need to pre-adjust that number to receive the proper backedge-taken
  // count
  if (auto Const =
          getIConstantVRegValWithLookThrough(I.getOperand(2).getReg(), MRI)) {
    auto OpCode = TII.getConstantMovOpcode(MRI, DstReg, Const->Value);
    assert(OpCode && "Failed to get constant mov opcode during ISel");
    auto Mov = MIB.buildInstr(*OpCode, {DstReg}, {})
                   .addImm(Const->Value.getSExtValue() - 1);
    I.eraseFromParent();
    return constrainSelectedInstRegOperands(*Mov, TII, TRI, RBI);
  }

  // Not a constant trip count, decrement at runtime
  auto ADDI = MIB.buildInstr(JNZDSupport->DecTripCountOpcode, {I.getOperand(0)},
                             {I.getOperand(2)})
                  .addImm(-1);
  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*ADDI, TII, TRI, RBI);
}

bool AIEBaseInstructionSelector::selectG_SEXT_INREG(
    MachineInstr &I, MachineRegisterInfo &MRI,
    const std::pair<unsigned, unsigned> &Opcodes) {
  Register DstReg = I.getOperand(0).getReg();
  Register SrcReg = I.getOperand(1).getReg();

  const RegisterBank *DstRB = RBI.getRegBank(DstReg, MRI, TRI);
  const RegisterBank *SrcRB = RBI.getRegBank(SrcReg, MRI, TRI);

  // We only support sign-extension on GPRs
  if (DstRB->getID() != SrcRB->getID() ||
      DstRB->getID() != TRI.getGPRRegBankID())
    return false;

  int64_t Imm = I.getOperand(2).getImm();
  MachineInstrBuilder MI;
  if (Imm == 8) {
    MI = MIB.buildInstr(Opcodes.first, {DstReg}, {SrcReg});
  } else if (Imm == 16) {
    MI = MIB.buildInstr(Opcodes.second, {DstReg}, {SrcReg});
  } else {
    llvm_unreachable("Cannot handle type in selectG_SEXT_INREG");
  }

  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
}

bool AIEBaseInstructionSelector::selectG_TRUNC(MachineInstr &I,
                                               MachineRegisterInfo &MRI,
                                               const unsigned SubRegIdx) {
  Register SrcReg = I.getOperand(1).getReg();
  LLT SrcTy = MRI.getType(SrcReg);
  unsigned SrcSize = SrcTy.getSizeInBits();
  // G_TRUNC S32 <- S64
  if (SrcSize == 64) {
    Register DstReg = I.getOperand(0).getReg();
    MachineInstrBuilder MI = MIB.buildInstr(TargetOpcode::COPY, {DstReg}, {})
                                 .addReg(SrcReg, 0, SubRegIdx);
    I.eraseFromParent();
    return selectCopy(*MI.getInstr(), MRI);
  } else if (SrcTy.isVector()) {
    assert(SrcSize >= 512 && "Invalid vector size for G_TRUNC source vector!");
    return selectImpl(I, *CoverageInfo);
  } else {
    I.setDesc(TII.get(TargetOpcode::COPY));
    return selectCopy(I, MRI);
  }
}

bool AIEBaseInstructionSelector::selectG_CONSTANT(MachineInstr &I,
                                                  MachineRegisterInfo &MRI) {
  // TODO: it isn't easy to rely on TableGen patterns, as there's poor support
  // for pointer types. If GlobalISel ever get its own pattern language with
  // pointer types properly supported, we should use it.
  const Register DstReg = I.getOperand(0).getReg();
  const LLT Ty = MRI.getType(DstReg);
  assert((Ty == LLT::pointer(0, 20) || Ty == LLT::scalar(20) ||
          Ty == LLT::scalar(32)) &&
         "Only support 20, 32-bit integer and 20-bit pointer constants");
  const RegisterBank &DstRB = *RBI.getRegBank(DstReg, MRI, TRI);
  assert((DstRB.getID() == TRI.getPTRRegBankID() ||
          DstRB.getID() == TRI.getMODRegBankID() ||
          DstRB.getID() == TRI.getGPRRegBankID()) &&
         "Expected constants only on GPR, MOD and PTR register banks");

  APInt Imm = I.getOperand(1).getCImm()->getValue();
  auto OpCode = TII.getConstantMovOpcode(MRI, DstReg, Imm);
  assert(OpCode && "Failed to get constant mov opcode during ISel");
  MachineInstr &MI = *MIB.buildInstr(*OpCode, {DstReg}, {})
                          .addImm(Imm.getSExtValue())
                          .getInstr();

  I.eraseFromParent();
  return constrainSelectedInstRegOperands(MI, TII, TRI, RBI);
}

bool AIEBaseInstructionSelector::selectGetCoreID(MachineInstr &I,
                                                 MachineRegisterInfo &MRI,
                                                 Register CoreID) {

  Register DstReg = I.getOperand(0).getReg();

  auto CopyInstr =
      MIB.buildInstr(TargetOpcode::COPY, {DstReg}, {}).addReg(CoreID);
  if (!selectCopy(*CopyInstr, MRI)) {
    return false;
  }

  I.eraseFromParent();
  return true;
}

bool AIEBaseInstructionSelector::selectReadTM(MachineInstr &I,
                                              MachineRegisterInfo &MRI,
                                              unsigned Opcode) {
  Register Dest = I.getOperand(0).getReg();
  Register Ptr = I.getOperand(2).getReg();

  MachineMemOperand *MMO = getTileMemOperand(
      I, MachineMemOperand::MOLoad | MachineMemOperand::MOVolatile);
  MachineInstrBuilder MI =
      MIB.buildInstr(Opcode, {Dest}, {Ptr}).addMemOperand(MMO).addImm(0x0);

  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
}

bool AIEBaseInstructionSelector::selectWriteTM(MachineInstr &I,
                                               MachineRegisterInfo &MRI,
                                               unsigned Opcode) {
  Register Value = I.getOperand(1).getReg();
  Register Ptr = I.getOperand(2).getReg();

  MachineMemOperand *MMO = getTileMemOperand(I, MachineMemOperand::MOStore);
  MachineInstrBuilder MI =
      MIB.buildInstr(Opcode, {}, {Value, Ptr}).addMemOperand(MMO).addImm(0x0);

  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
}

// Select extract 128-bit vectors
bool AIEBaseInstructionSelector::selectExtractI128(MachineInstr &I,
                                                   Register DstReg,
                                                   Register SrcReg,
                                                   MachineRegisterInfo &MRI,
                                                   MachineFunction &MF) {
  LLT DstTy = MRI.getType(DstReg);
  assert(DstTy.getSizeInBits() == 128);
  LLT SrcTy = MRI.getType(SrcReg);

  unsigned SubReg = getNoSubRegIdx();
  switch (SrcTy.getSizeInBits()) {
  case 256:
    SubReg = getNoSubRegIdx();
    break;
  case 512:
    SubReg = getSub256LoIdx();
    break;
  default:
    llvm_unreachable("Unexpected input size for extracting 128-bit vector");
  }

  // Select using a COPY to a 128-bit register.
  MachineInstr *CopyMI = MIB.buildInstr(TargetOpcode::COPY, {DstReg}, {})
                             .addReg(SrcReg, 0, SubReg);
  constrainOperandRegClass(MF, TRI, MRI, TII, RBI, *CopyMI, getVEC128RegClass(),
                           CopyMI->getOperand(0));

  I.eraseFromParent();
  return true;
}

// Build Instruction to set control register
bool AIEBaseInstructionSelector::selectSetControlRegister(
    MachineInstr &I, MachineRegisterInfo &MRI) {

  const Register IdxReg = I.getOperand(1).getReg();
  const Register SrcReg = I.getOperand(2).getReg();

  // Check if the argument is constant for register map index.
  const auto Idx = getIConstantVRegValWithLookThroughOrFail(
      IdxReg, MRI, "Expected const value for control register map index.");

  const Register CtrlReg = TRI.getControlRegister(Idx.Value.getZExtValue());

  // Handle const input val for control regs.
  if (const auto Src = getIConstantVRegValWithLookThrough(SrcReg, MRI)) {
    const unsigned SrcConstVal =
        TRI.matchControlRegisterBitwidth(CtrlReg, Src->Value.getZExtValue());

    MachineInstrBuilder MI = setCtrlRegister(MIB, CtrlReg, SrcConstVal);
    I.eraseFromParent();
    return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
  }

  auto CopyInstr =
      MIB.buildInstr(TargetOpcode::COPY, {CtrlReg}, {}).addReg(SrcReg);
  if (!selectCopy(*CopyInstr, MRI))
    return false;

  I.eraseFromParent();
  return true;
}

bool AIEBaseInstructionSelector::selectG_AIE_UNPAD_VECTOR(
    MachineInstr &I, Register DstReg, Register SrcReg, MachineRegisterInfo &MRI,
    MachineFunction &MF) {
  const LLT DstTy = MRI.getType(DstReg);
  if (DstTy.getSizeInBits() == 128)
    return selectExtractI128(I, DstReg, SrcReg, MRI, MF);

  assert(DstTy.getSizeInBits() == 256);
  const LLT SrcTy = MRI.getType(SrcReg);
  const unsigned SrcTySize = SrcTy.getSizeInBits();
  assert(SrcTySize == 512);

  // Select using a COPY to a 256-bit register.
  MachineInstr *CopyMI = MIB.buildInstr(TargetOpcode::COPY, {DstReg}, {})
                             .addReg(SrcReg, 0, getSub256LoIdx());
  constrainOperandRegClass(MF, TRI, MRI, TII, RBI, *CopyMI, getVEC256RegClass(),
                           CopyMI->getOperand(0));
  I.eraseFromParent();
  return true;
}

bool AIEBaseInstructionSelector::selectSetI128(MachineInstr &I,
                                               MachineOperand &DstReg,
                                               MachineOperand &SrcReg,
                                               MachineRegisterInfo &MRI,
                                               MachineFunction &MF) {
  LLT SrcTy = MRI.getType(SrcReg.getReg());
  assert(SrcTy.getSizeInBits() == 128);
  LLT DstTy = MRI.getType(DstReg.getReg());
  const unsigned DstTySize = DstTy.getSizeInBits();
  assert(DstTySize == 256 || DstTySize == 512);

  // Constrain input vector to VEC128 RC, and output to VEC256/VEC512
  const TargetRegisterClass &OutRC =
      DstTySize == 256 ? getVEC256RegClass() : getVEC512RegClass();
  constrainOperandRegClass(MF, TRI, MRI, TII, RBI, I, getVEC128RegClass(),
                           SrcReg);
  constrainOperandRegClass(MF, TRI, MRI, TII, RBI, I, OutRC, DstReg);

  if (DstTySize == 256) {
    MIB.buildInstr(TargetOpcode::COPY, {DstReg}, {SrcReg});
  } else if (DstTySize == 512) {
    auto SrcInW =
        MIB.buildInstr(TargetOpcode::COPY, {&getVEC256RegClass()}, {SrcReg});
    // Create 512-bit sources from 256-bit sources.
    MIB.buildInstr(TargetOpcode::REG_SEQUENCE, {DstReg}, {})
        .addReg(SrcInW.getReg(0))
        .addImm(getSub256LoIdx());
  } else {
    llvm_unreachable("Expected 256 or 512 bit input vector");
  }

  I.eraseFromParent();
  return true;
}

// Build Instruction to get control register
bool AIEBaseInstructionSelector::selectGetControlRegister(
    MachineInstr &I, MachineRegisterInfo &MRI) {

  const Register DstReg = I.getOperand(0).getReg();
  // In this case of G_INTRINSIC operand 1 is target intrinsic
  const Register IdxReg = I.getOperand(2).getReg();

  // Check if the argument is constant for register map index.
  const auto Idx = getIConstantVRegValWithLookThroughOrFail(
      IdxReg, MRI, "Expected const value for control register map index.");

  const Register CtrlReg = TRI.getControlRegister(Idx.Value.getZExtValue());
  const MachineFunction &MF = *I.getParent()->getParent();

  if (!RBI.constrainGenericRegister(DstReg, *TRI.getGPRRegClass(MF), MRI))
    return false;

  auto CopyInstr =
      MIB.buildInstr(TargetOpcode::COPY, {DstReg}, {}).addReg(CtrlReg);
  if (!selectCopy(*CopyInstr, MRI))
    return false;

  I.eraseFromParent();
  return true;
}

bool AIEBaseInstructionSelector::selectG_AIE_PAD_VECTOR_UNDEF(
    MachineInstr &I, MachineOperand &DstReg, MachineOperand &SrcReg,
    MachineRegisterInfo &MRI, MachineFunction &MF) {
  const LLT SrcTy = MRI.getType(SrcReg.getReg());
  if (SrcTy.getSizeInBits() == 128)
    return selectSetI128(I, DstReg, SrcReg, MRI, MF);

  assert(SrcTy.getSizeInBits() == 256);
  const LLT DstTy = MRI.getType(DstReg.getReg());
  const unsigned DstTySize = DstTy.getSizeInBits();
  assert(DstTySize == 512);

  // Constrain input vector to VEC256 RC, and output to VEC512
  const TargetRegisterClass &OutRC = getVEC512RegClass();
  constrainOperandRegClass(MF, TRI, MRI, TII, RBI, I, getVEC256RegClass(),
                           SrcReg);
  constrainOperandRegClass(MF, TRI, MRI, TII, RBI, I, OutRC, DstReg);
  MIB.buildInstr(TargetOpcode::REG_SEQUENCE, {DstReg}, {SrcReg})
      .addImm(getSub256LoIdx());

  I.eraseFromParent();
  return true;
}

// Build Instruction to set status register
bool AIEBaseInstructionSelector::selectSetStatusRegister(
    MachineInstr &I, MachineRegisterInfo &MRI) {

  const Register IdxReg = I.getOperand(1).getReg();
  const Register SrcReg = I.getOperand(2).getReg();

  // Check if the argument is constant for register map index.
  const auto Idx = getIConstantVRegValWithLookThroughOrFail(
      IdxReg, MRI, "Expected const value for status register map index.");

  const Register StatusReg = TRI.getStatusRegister(Idx.Value.getZExtValue());

  // Handle const input val for status regs.
  if (const auto Src = getIConstantVRegValWithLookThrough(SrcReg, MRI)) {
    const unsigned SrcConstVal =
        TRI.matchStatusRegisterBitwidth(StatusReg, Src->Value.getZExtValue());

    MachineInstrBuilder MI = setStatusRegister(MIB, StatusReg, SrcConstVal);
    I.eraseFromParent();
    return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
  }

  auto CopyInstr =
      MIB.buildInstr(TargetOpcode::COPY, {StatusReg}, {}).addReg(SrcReg);
  if (!selectCopy(*CopyInstr, MRI))
    return false;

  I.eraseFromParent();
  return true;
}

// Build Instruction to get status register
bool AIEBaseInstructionSelector::selectGetStatusRegister(
    MachineInstr &I, MachineRegisterInfo &MRI) {

  const Register DstReg = I.getOperand(0).getReg();
  // In this case of G_INTRINSIC operand 1 is target intrinsic
  const Register IdxReg = I.getOperand(2).getReg();

  // Check if the argument is constant for register map index.
  const auto Idx = getIConstantVRegValWithLookThroughOrFail(
      IdxReg, MRI, "Expected const value for status register map index.");

  const Register StatusReg = TRI.getStatusRegister(Idx.Value.getZExtValue());
  const MachineFunction &MF = *I.getParent()->getParent();

  if (!RBI.constrainGenericRegister(DstReg, *TRI.getGPRRegClass(MF), MRI))
    return false;

  auto CopyInstr =
      MIB.buildInstr(TargetOpcode::COPY, {DstReg}, {}).addReg(StatusReg);
  if (!selectCopy(*CopyInstr, MRI))
    return false;

  I.eraseFromParent();
  return true;
}

bool AIEBaseInstructionSelector::selectVUPS(
    MachineInstr &I, MachineRegisterInfo &MRI,
    std::optional<unsigned> crUPSModeVal) {
  // First try to match UPS combine
  if (selectG_AIE_LOAD_UPS(I, MRI, crUPSModeVal))
    return true;

  const Register DstReg = I.getOperand(0).getReg();
  // In this case of G_INTRINSIC_W_SIDE_EFFECTS operand 1 is target intrinsic
  const Register SrcReg = I.getOperand(2).getReg();
  const Register ShftReg = I.getOperand(3).getReg();
  const Register SignReg = I.getOperand(4).getReg();

  if (crUPSModeVal.has_value()) {
    // Selects the mode of the accumulator for UPS instructions
    // 0 – 32-bit accumulator lane
    // 1 – 64-bit accumulator lane
    MIB.setInstr(I);
    setCtrlRegister(MIB, TII.getUPSModeControlRegister(), *crUPSModeVal);
  }

  if (const auto SignVal = getIConstantVRegValWithLookThrough(SignReg, MRI)) {
    // Handle constant sign through instruction patterns
    return selectImpl(I, *CoverageInfo);
  }

  const unsigned OpCode = TII.getOpCode(I);
  MachineInstrBuilder MI =
      MIB.buildInstr(OpCode, {DstReg}, {}).addReg(SrcReg).addReg(ShftReg);

  setUnsetCtrlRegister(MIB, *MI, MRI, TII.getUPSSignControlRegister(), SignReg);

  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
}

bool AIEBaseInstructionSelector::selectG_AIE_STORE_SRS(
    MachineInstr &StoreI, MachineRegisterInfo &MRI) {
  Register SrsResult = (StoreI.uses().begin())->getReg();
  MachineInstr *SrsOp = getDefIgnoringCopiesAndBitcasts(SrsResult, MRI);

  assert(SrsOp && "Expected SSA.");

  if (!canCombineSRS(StoreI, *SrsOp, MRI))
    return false;

  std::optional<AddressingModeInfo> AddrModeInfo =
      getOrDefineAddressingRegister(StoreI, MRI);
  if (!AddrModeInfo)
    return false;

  // Note: Operand 1 is the ID of the intrinsic
  Register SrcReg = SrsOp->getOperand(2).getReg();
  Register ShftReg = SrsOp->getOperand(3).getReg();
  Register SignReg = SrsOp->getOperand(4).getReg();

  auto SignVal = getIConstantVRegValWithLookThrough(SignReg, MRI);
  bool ConstantSign = SignVal.has_value();
  // SignVal = 1 for signed and 0 for unsigned
  std::optional<LoadStoreOpcodes> LSO =
      getCombinedOpcodeSRS(StoreI, *SrsOp, AddrModeInfo->ImmediateOffset,
                           ConstantSign ? SignVal->Value == 0x1 : false);

  assert(LSO && "Unexpected VST.SRS combine failure");

  // Set SRS mode control register if needed
  MIB.setInstr(StoreI);
  std::optional<unsigned> SRSMode =
      getSRSModeForIntrinsic(cast<GIntrinsic>(SrsOp)->getIntrinsicID());
  if (SRSMode)
    setCtrlRegister(MIB, TRI.getSRSModeCtrlReg(), *SRSMode);

  auto NewInstr = MIB.buildInstr(LSO->ISelOpcode);

  for (auto Def : StoreI.defs())
    NewInstr.addDef(Def.getReg());

  NewInstr.addUse(SrcReg);
  NewInstr.addUse(ShftReg);

  addAddressingMode(NewInstr, *AddrModeInfo, LSO->FitsImmediateRange, false,
                    MRI);

  NewInstr.cloneMemRefs(StoreI);

  if (!ConstantSign)
    setUnsetCtrlRegister(MIB, *NewInstr, MRI, TRI.getSRSSignCtrlReg(), SignReg);

  makeDeadMI(*SrsOp, MRI);
  StoreI.eraseFromParent();
  return constrainSelectedInstRegOperands(*NewInstr.getInstr(), TII, TRI, RBI);
}

// FIFO Store selection helpers - shared implementation for AIE2P and AIE4
bool AIEBaseInstructionSelector::selectVST_FIFO_Push(MachineInstr &I,
                                                     MachineRegisterInfo &MRI) {
  const unsigned Opcode = TII.getOpCode(I);
  const Register PtrOut = I.getOperand(0).getReg();
  const Register FifoOut = I.getOperand(1).getReg();
  const Register AvailOut = I.getOperand(2).getReg();
  const Register PtrIn = I.getOperand(4).getReg();
  const Register VecIn = I.getOperand(5).getReg();
  const Register FifoIn = I.getOperand(6).getReg();
  const Register AvailIn = I.getOperand(7).getReg();

  auto MI = MIB.buildInstr(Opcode, {FifoOut, PtrOut, AvailOut},
                           {FifoIn, VecIn, PtrIn, AvailIn});
  MI.cloneMemRefs(I);
  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
}

bool AIEBaseInstructionSelector::selectVST_FIFO_Flush(
    MachineInstr &I, MachineRegisterInfo &MRI) {
  const unsigned Opcode = TII.getOpCode(I);
  const Register PtrOut = I.getOperand(0).getReg();
  const Register FifoOut = I.getOperand(1).getReg();
  const Register AvailOut = I.getOperand(2).getReg();
  const Register PtrIn = I.getOperand(4).getReg();
  const Register FifoIn = I.getOperand(5).getReg();
  const Register AvailIn = I.getOperand(6).getReg();

  auto MI = MIB.buildInstr(Opcode, {FifoOut, PtrOut, AvailOut},
                           {FifoIn, PtrIn, AvailIn});
  MI.cloneMemRefs(I);
  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
}

bool AIEBaseInstructionSelector::selectVST_FIFO_Flush1D(
    MachineInstr &I, MachineRegisterInfo &MRI) {
  const unsigned Opcode = TII.getOpCode(I);
  const Register PtrOut = I.getOperand(0).getReg();
  const Register FifoOut = I.getOperand(1).getReg();
  const Register AvailOut = I.getOperand(2).getReg();
  const Register PtrIn = I.getOperand(4).getReg();
  const Register FifoIn = I.getOperand(5).getReg();
  const Register AvailIn = I.getOperand(6).getReg();
  const Register OffsetReg = I.getOperand(7).getReg();

  auto MI = MIB.buildInstr(Opcode, {FifoOut, PtrOut, AvailOut},
                           {FifoIn, PtrIn, AvailIn, OffsetReg});
  MI.cloneMemRefs(I);
  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
}

bool AIEBaseInstructionSelector::selectVST_FIFO_Flush2D(
    MachineInstr &I, MachineRegisterInfo &MRI) {
  const unsigned Opcode = TII.getOpCode(I);
  const Register PtrOut = I.getOperand(0).getReg();
  const Register FifoOut = I.getOperand(1).getReg();
  const Register AvailOut = I.getOperand(2).getReg();
  const Register CountOut1Reg = I.getOperand(3).getReg();
  const Register PtrIn = I.getOperand(5).getReg();
  const Register FifoIn = I.getOperand(6).getReg();
  const Register AvailIn = I.getOperand(7).getReg();
  const Register OffsetReg = I.getOperand(8).getReg();
  const Register SizeReg = I.getOperand(9).getReg();
  const Register CountIn1Reg = I.getOperand(10).getReg();
  const Register IncrReg = I.getOperand(11).getReg();

  const Register DReg =
      createDRegSequence(OffsetReg, IncrReg, SizeReg, CountIn1Reg, MRI);

  auto MI = MIB.buildInstr(Opcode, {FifoOut, PtrOut, AvailOut, CountOut1Reg},
                           {FifoIn, PtrIn, AvailIn, DReg});
  MI.cloneMemRefs(I);
  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
}

bool AIEBaseInstructionSelector::selectVST_FIFO_Flush3D(
    MachineInstr &I, MachineRegisterInfo &MRI) {
  const unsigned Opcode = TII.getOpCode(I);
  const Register PtrOut = I.getOperand(0).getReg();
  const Register FifoOut = I.getOperand(1).getReg();
  const Register AvailOut = I.getOperand(2).getReg();
  const Register CountOut1Reg = I.getOperand(3).getReg();
  const Register CountOut2Reg = I.getOperand(4).getReg();
  const Register PtrIn = I.getOperand(6).getReg();
  const Register FifoIn = I.getOperand(7).getReg();
  const Register AvailIn = I.getOperand(8).getReg();
  const Register OffsetReg = I.getOperand(9).getReg();
  const Register Size1Reg = I.getOperand(10).getReg();
  const Register CountIn1Reg = I.getOperand(11).getReg();
  const Register Incr1Reg = I.getOperand(12).getReg();
  const Register Size2Reg = I.getOperand(13).getReg();
  const Register CountIn2Reg = I.getOperand(14).getReg();
  const Register Incr2Reg = I.getOperand(15).getReg();

  const Register DSReg =
      createDSRegSequence(OffsetReg, Incr1Reg, Incr2Reg, Size1Reg, CountIn1Reg,
                          Size2Reg, CountIn2Reg, MRI);

  auto MI = MIB.buildInstr(
      Opcode, {FifoOut, PtrOut, AvailOut, CountOut1Reg, CountOut2Reg},
      {FifoIn, PtrIn, AvailIn, DSReg});
  MI.cloneMemRefs(I);
  I.eraseFromParent();
  return constrainSelectedInstRegOperands(*MI, TII, TRI, RBI);
}
