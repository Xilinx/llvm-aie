//===-- AIESplitInstructionRewriter.cpp - Passes for _split instructions --===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEBaseInstrInfo.h"
#include "AIETiedRegOperands.h"

#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/SlotIndexes.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "aie-ra"

namespace {

/// Replace instructions with tuple regs into _split instructions with atomic
/// regs. This is meant to be run before the RA pass.
/// E.g. for AIE2: PADDA_2D -> PADDA_2D_split
class AIESplitInstrBuilder : public MachineFunctionPass {

public:
  static char ID;
  AIESplitInstrBuilder() : MachineFunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    AU.addUsedIfAvailable<SlotIndexes>();
    AU.addPreserved<SlotIndexes>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &Fn) override;

private:
  /// Add the atomic sub-regs of \p InitialMO as new operands to \p MIB.
  void addRegOperands(MachineInstrBuilder &MIB, const MachineOperand &InitialMO,
                      ArrayRef<SubRegSplit> NewSubRegs);

  /// Replace \p MI with a new instruction where the tuple operands are
  /// rewritten into multiple operands for their different sub-registers
  /// according to \p OperandsInfo.
  /// THe new instruction will have \p SplitOpcode as new opcode.
  void rewriteInstruction(const TiedRegOperands &OperandsInfo, MachineInstr &MI,
                          unsigned SplitOpcode);
};

bool AIESplitInstrBuilder::runOnMachineFunction(MachineFunction &MF) {
  LLVM_DEBUG(llvm::dbgs() << "Creating split instructions: " << MF.getName()
                          << "\n");

  auto &TII =
      *static_cast<const AIEBaseInstrInfo *>(MF.getSubtarget().getInstrInfo());

  for (MachineBasicBlock &MBB : MF) {
    for (MachineInstr &MI : make_early_inc_range(MBB)) {
      if (std::optional<unsigned> SplitOpcode =
              TII.getOpcodeWithAtomicOperands(MI.getOpcode())) {
        assert(TII.getTiedRegInfo(MI).size() == 1);
        rewriteInstruction(TII.getTiedRegInfo(MI).front(), MI, *SplitOpcode);
      }
    }
  }

  return true;
}

void AIESplitInstrBuilder::addRegOperands(MachineInstrBuilder &MIB,
                                          const MachineOperand &InitialMO,
                                          ArrayRef<SubRegSplit> NewSubRegs) {
  assert(!NewSubRegs.empty());
  for (const SubRegSplit &SRS : NewSubRegs) {
    MachineOperand NewMO = InitialMO;
    NewMO.setSubReg(SRS.SubReg);
    if (SRS.IsUndef)
      NewMO.setIsUndef();
    MIB->addOperand(NewMO);
  }
}

void AIESplitInstrBuilder::rewriteInstruction(
    const TiedRegOperands &OperandsInfo, MachineInstr &MI,
    unsigned SplitOpcode) {

  LLVM_DEBUG(llvm::dbgs() << "Rewriting into split instruction: " << MI);
  MachineBasicBlock &MBB = *MI.getParent();
  MachineFunction &MF = *MBB.getParent();
  auto *TII = MF.getSubtarget().getInstrInfo();

  unsigned NumInitialOps = MI.getNumOperands();
  MachineInstrBuilder MIB =
      BuildMI(MBB, MI, MI.getDebugLoc(), TII->get(SplitOpcode));

  // Iterate over all the operands and rewrite those that are reg tuples.
  for (unsigned OpIdx = 0; OpIdx < NumInitialOps; ++OpIdx) {
    const MachineOperand &MO = MI.getOperand(OpIdx);
    if (const OperandSubRegMapping *SRM = OperandsInfo.findOperandInfo(OpIdx);
        SRM && !SRM->SubRegsSplit.empty()) {
      addRegOperands(MIB, MO, SRM->SubRegsSplit);
    } else if (!MO.isImplicit()) {
      // Note implicit ops are added by default by BuildMI.
      MIB->addOperand(MO);
    }
  }

  MIB->cloneMemRefs(MF, MI);
  if (SlotIndexes *Indexes = getAnalysisIfAvailable<SlotIndexes>()) {
    // Maintaining SlotIndexes is easy and help avoiding unnecessary test
    // updates. Renumbering instruction slots can cause LiveIntervals
    // to be estimated a different size, which then changes the allocation
    // priority inside Greedy.
    Indexes->replaceMachineInstrInMaps(MI, *MIB);
  }
  MI.eraseFromBundle();
}

/// Replace instructions with atomic regs into instructions with tuple regs.
/// This is meant to be run at the end of the RA pass pipeline to prepare
/// for scheduling and encoding.
/// E.g. for AIE2: PADDA_2D_split -> PADDA_2D
class AIESplitInstrReplacer : public MachineFunctionPass {

public:
  static char ID;
  AIESplitInstrReplacer() : MachineFunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &Fn) override;

private:
  /// Add a new tuple reg operand to \p MIB that represents all the sub-regs
  /// in \p SplitSubRegs.
  /// \param MIB New instruction to add the operand to.
  /// \param FirstSplitMO Pointer to the MachineOperand of the first sub-reg
  ///        operand in the initial instruction. The next MOs can be accessed
  ///        by ++FirstSplitMO.
  /// \param SplitSubRegs Information on how the sub-reg operands compose a
  ///        tuple register.
  /// \param TupleRC Register class that the new tuple reg belongs to.
  void addRegOperand(MachineInstrBuilder &MIB,
                     MachineInstr::const_mop_iterator FirstSplitMO,
                     ArrayRef<SubRegSplit> SplitSubRegs,
                     const TargetRegisterClass &TupleRC,
                     const TargetRegisterInfo *TRI);

  /// Replace \p MI with a new instruction where multiple "atomic" operands are
  /// rewritten into a single tuple reg operand that encapsulates the initial
  /// operands.
  /// The new instruction will have \p OpcodeWithTuple as new opcode.
  void rewriteInstruction(const TiedRegOperands &OperandsInfo, MachineInstr &MI,
                          unsigned OpcodeWithTuple);
};

bool AIESplitInstrReplacer::runOnMachineFunction(MachineFunction &MF) {
  LLVM_DEBUG(llvm::dbgs() << "Replacing split instructions: " << MF.getName()
                          << "\n");

  auto &TII =
      *static_cast<const AIEBaseInstrInfo *>(MF.getSubtarget().getInstrInfo());

  for (MachineBasicBlock &MBB : MF) {
    for (MachineInstr &MI : make_early_inc_range(MBB)) {
      if (std::optional<unsigned> OpcodeWithTuple =
              TII.getOpcodeWithTupleOperands(MI.getOpcode())) {
        assert(TII.getTiedRegInfo(*OpcodeWithTuple).size() == 1);
        rewriteInstruction(TII.getTiedRegInfo(*OpcodeWithTuple).front(), MI,
                           *OpcodeWithTuple);
      }
    }
  }

  return true;
}

void AIESplitInstrReplacer::addRegOperand(
    MachineInstrBuilder &MIB, MachineInstr::const_mop_iterator FirstSplitMO,
    ArrayRef<SubRegSplit> SplitSubRegs, const TargetRegisterClass &TupleRC,
    const TargetRegisterInfo *TRI) {
  assert(!SplitSubRegs.empty());
  MachineBasicBlock &MBB = *MIB->getParent();

  // Find the SuperReg which contains all the "split" atomic sub-registers.
  MCRegister TupleReg = MCRegister::NoRegister;
  MachineInstr::const_mop_iterator SplitMO = FirstSplitMO;
  for (const SubRegSplit &SRS : SplitSubRegs) {
    assert(SplitMO != FirstSplitMO->getParent()->operands_end());
    LLVM_DEBUG(llvm::dbgs()
               << "  Finding super-reg for " << *SplitMO
               << " in RC=" << TRI->getRegClassName(&TupleRC) << "\n");

    MCRegister SuperReg =
        TRI->getMatchingSuperReg(SplitMO->getReg(), SRS.SubReg, &TupleRC);
    assert(SuperReg != MCRegister::NoRegister);
    assert(TupleReg == MCRegister::NoRegister || SuperReg == TupleReg);
    TupleReg = SuperReg;

    // Update MBB liveins if the atomic reg operand was livein.
    if (MBB.isLiveIn(SplitMO->getReg())) {
      LaneBitmask RegMask = TRI->getSubRegIndexLaneMask(SRS.SubReg);
      MBB.addLiveIn(TupleReg, RegMask);
    }

    ++SplitMO;
  }

  // Now add the super-reg as a new operand.
  // TODO: Do we need to add operand flags?
  assert(TupleReg != MCRegister::NoRegister);
  LLVM_DEBUG(llvm::dbgs() << "  Found super-reg: " << printReg(TupleReg, TRI)
                          << "\n");
  MIB.addReg(TupleReg);

  // We added some new livein regmasks, unify them.
  MBB.sortUniqueLiveIns();
}

void AIESplitInstrReplacer::rewriteInstruction(
    const TiedRegOperands &OperandsInfo, MachineInstr &MI,
    unsigned OpcodeWithTuple) {

  LLVM_DEBUG(llvm::dbgs() << "Rewriting into tuple instruction: " << MI);
  MachineBasicBlock &MBB = *MI.getParent();
  MachineFunction &MF = *MBB.getParent();
  auto *TII = MF.getSubtarget().getInstrInfo();
  auto *TRI = MF.getSubtarget().getRegisterInfo();

  unsigned NumSplitOps = MI.getNumOperands();
  MachineInstrBuilder MIB =
      BuildMI(MBB, MI, MI.getDebugLoc(), TII->get(OpcodeWithTuple));

  // Iterate over the operands of the instruction with "split atomic operands"
  // and add them back to the new MIB. When meeting split operands, they are
  // transformed into a single super-reg operand.
  unsigned SplitOpIdx = 0, TupleOpIdx = 0;
  while (SplitOpIdx < NumSplitOps) {
    MachineInstr::const_mop_iterator FirstMO = MI.operands_begin() + SplitOpIdx;
    if (const OperandSubRegMapping *SRM =
            OperandsInfo.findOperandInfo(TupleOpIdx);
        SRM && !SRM->SubRegsSplit.empty()) {
      // This is the first "split atomic" operand of a reg tuple, find the other
      // operands that belong to the tuple, and add it to the new MIB.
      const TargetRegisterClass *TupleRC =
          TII->getRegClass(TII->get(OpcodeWithTuple), TupleOpIdx, TRI, MF);
      assert(TupleRC && "Cannot find RC for super-reg");
      addRegOperand(MIB, FirstMO, SRM->SubRegsSplit, *TupleRC, TRI);
      SplitOpIdx += SRM->SubRegsSplit.size();
    } else {
      // Note implicit ops are added by default by BuildMI.
      if (!FirstMO->isImplicit())
        MIB->addOperand(*FirstMO);
      ++SplitOpIdx;
    }
    ++TupleOpIdx;
  }

  MIB->cloneMemRefs(MF, MI);
  MI.eraseFromBundle();
  LLVM_DEBUG(llvm::dbgs() << "Rewritten into: " << *MIB);
}

} // end anonymous namespace

char AIESplitInstrBuilder::ID = 0;
char &llvm::AIESplitInstrBuilderID = AIESplitInstrBuilder::ID;

INITIALIZE_PASS(AIESplitInstrBuilder, "aie-split-instrs-create",
                "AIE 2D/3D operand splitter", false, false)

llvm::FunctionPass *llvm::createAIESplitInstrBuilder() {
  return new AIESplitInstrBuilder();
}

char AIESplitInstrReplacer::ID = 0;
char &llvm::AIESplitInstrReplacerID = AIESplitInstrReplacer::ID;

INITIALIZE_PASS(AIESplitInstrReplacer, "aie-split-instrs-replace",
                "AIE 1D operands to 2D/3D rewriter", false, false)

llvm::FunctionPass *llvm::createAIESplitInstrReplacer() {
  return new AIESplitInstrReplacer();
}
