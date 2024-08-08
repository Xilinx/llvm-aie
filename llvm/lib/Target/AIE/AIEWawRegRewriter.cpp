//===----- AEIWarRegRewriter.cpp - Rewrite Register to not include multiple Defs
//---------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Implementation of a Loop Register rewriting pass, that reduces WAW physical
// register conflicts, so that software pipelining compact loops better.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseInstrInfo.h"
#include "AIEBaseRegisterInfo.h"

#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/CodeGen/LiveDebugVariables.h"
#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/LiveRegMatrix.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/SlotIndexes.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "aie-wawreg-rewrite"

namespace {

///
/// This rewrites Registers, if they are redefined in the same Loop body and
/// could cause WAW issues in the loop pipelining.
/// The pass will update the \p VirtRegMap so that the new vregs have fixed
/// assignments, guaranteeing that the reg renaming does not cause conflicts.
class AIEWawRegRewriter : public MachineFunctionPass {

public:
  static char ID;
  AIEWawRegRewriter() : MachineFunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    // AU.addPreserved<MachineBlockFrequencyInfo>();
    AU.addRequired<VirtRegMap>();
    AU.addPreserved<VirtRegMap>();
    AU.addRequired<SlotIndexes>();
    AU.addPreserved<SlotIndexes>();
    // AU.addRequired<LiveDebugVariables>();
    // AU.addPreserved<LiveDebugVariables>();
    // AU.addRequired<LiveStacks>();
    // AU.addPreserved<LiveStacks>();
    AU.addRequired<LiveIntervals>();
    AU.addPreserved<LiveIntervals>();
    AU.addRequired<LiveRegMatrix>();
    AU.addPreserved<LiveRegMatrix>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &Fn) override;

private:
  std::map<const MachineOperand *, std::set<MCPhysReg>>
  getWarRenameRegs(const MachineFunction &MF, const VirtRegMap &VRM,
                   LiveRegMatrix &LRM, const AIEBaseRegisterInfo *TRI,
                   const MachineRegisterInfo *MRI, const LiveIntervals &LIS,
                   const std::set<MCPhysReg> &CSPhyRegs) const;
};

static std::set<MCPhysReg> getCSPhyRegs(const MachineRegisterInfo &MRI,
                                        const AIEBaseRegisterInfo *TRI);

static bool containsLoop(const MachineFunction &MF);
bool isCopyElimination(const MachineInstr &MI, const VirtRegMap &VRM);

static std::set<MCPhysReg>
getBBForbiddenReplacements(const MachineBasicBlock &MBB, const VirtRegMap &VRM,
                           const MachineRegisterInfo &MRI,
                           const AIEBaseRegisterInfo *TRI,
                           const std::set<MCPhysReg> &CSPhyRegs);

/// Find Free register of the same register Class type and
// exclude the forbidden Physical register from the result
static std::set<MCPhysReg> getPriorityReplacementRegs(
    const MachineBasicBlock &MBB, const Register Reg, const VirtRegMap &VRM,
    LiveRegMatrix &LRM, const MachineRegisterInfo &MRI,
    const LiveIntervals &LIS, const AIEBaseRegisterInfo *TRI,
    const std::set<MCPhysReg> &ForbiddenPhysRegs);

bool AIEWawRegRewriter::runOnMachineFunction(MachineFunction &MF) {
  LLVM_DEBUG(llvm::dbgs() << "*** WAW Loop Register Rewriting: " << MF.getName()
                          << " ***\n");

  if (!containsLoop(MF)) {
    LLVM_DEBUG(llvm::dbgs() << "Could Not detect any loop body\n");
    return false;
  }

  MachineRegisterInfo &MRI = MF.getRegInfo();
  auto &TRI =
      *static_cast<const AIEBaseRegisterInfo *>(MRI.getTargetRegisterInfo());
  VirtRegMap &VRM = getAnalysis<VirtRegMap>();
  LiveRegMatrix &LRM = getAnalysis<LiveRegMatrix>();
  LiveIntervals &LIS = getAnalysis<LiveIntervals>();
  std::map<Register, MCRegister> AssignedPhysRegs;
  bool Modified = false;

  std::set<MCPhysReg> CSPhyRegs = getCSPhyRegs(MRI, &TRI);

  std::map<const MachineOperand *, std::set<MCPhysReg>> PriorityRenameRegs =
      getWarRenameRegs(MF, VRM, LRM, &TRI, &MRI, LIS, CSPhyRegs);

  LLVM_DEBUG(VRM.dump());
  std::set<Register> AlreadyReplaced;
  for (const auto &KeyValuePair : PriorityRenameRegs) {
    Register Reg = KeyValuePair.first->getReg();
    const std::set<MCPhysReg> &AlternativePhyRegs = KeyValuePair.second;

    LLVM_DEBUG(
        dbgs() << "\nAnalysing: MBB"
               << KeyValuePair.first->getParent()->getParent()->getName();
        dbgs() << "\n     " << printReg(Reg, &TRI, 0, &MRI) << ":"
               << printRegClassOrBank(Reg, MRI, &TRI) << " ";
        KeyValuePair.first->getParent()->print(dbgs()););

    for (const MCPhysReg &PhysReg : AlternativePhyRegs) {
      // iterate alternative PhysRegs and allocate first found reg that
      // 1) has not been replaced yet
      // 2) does not interfere

      if (AlreadyReplaced.end() != find(AlreadyReplaced, PhysReg))
        continue;

      const llvm::LiveInterval &LI = LIS.getInterval(Reg);
      LiveRegMatrix::InterferenceKind IK = LRM.checkInterference(LI, PhysReg);

      if (IK == llvm::LiveRegMatrix::IK_Free) {
        LLVM_DEBUG(dbgs() << "     Potential WAR Issue: Virtual Register "
                          << llvm::Register::virtReg2Index(Reg)
                          << " will replace " << TRI.getName(VRM.getPhys(Reg))
                          << " with " << printReg(PhysReg, &TRI, 0, &MRI)
                          << '\n');

        VRM.clearVirt(Reg);
        LRM.assign(LI, PhysReg);
        AlreadyReplaced.insert(PhysReg);
        Modified = true;

        break;
      }
    }
  }

  LLVM_DEBUG(VRM.dump());
  return Modified;
}

/// Find Virtual Register that should not map to the same physical register
/// in a WAW conflict, even though they don't interfere. This is done,
/// so that pipelining can be performed.
std::map<const MachineOperand *, std::set<MCPhysReg>>
AIEWawRegRewriter::getWarRenameRegs(
    MachineFunction const &MF, const VirtRegMap &VRM, LiveRegMatrix &LRM,
    const AIEBaseRegisterInfo *TRI, const MachineRegisterInfo *MRI,
    const LiveIntervals &LIS, const std::set<MCPhysReg> &CSPhyRegs) const {
  MF.dump();
  std::map<const MachineOperand *, std::set<MCPhysReg>> PriorityRenameRegs;
  for (const MachineBasicBlock &MBB : MF) {

    if (MBB.succ_end() == find(MBB.successors(), &MBB))
      continue;

    std::set<MCPhysReg> UsedPhyRegs;
    std::set<Register> UsedVirtRegs;

    std::set<MCPhysReg> ForbiddenPhysRegs =
        getBBForbiddenReplacements(MBB, VRM, *MRI, TRI, CSPhyRegs);

    // get all WAW definitions that map to the same physical register
    // these are candidates for register renaming
    for (const MachineInstr &MI : MBB) {
      const MCInstrDesc &MCID = MI.getDesc();
      MI.dump();
      if (isCopyElimination(MI, VRM))
        continue;

      for (unsigned I = 0, E = MI.getNumOperands(); I != E; ++I) {
        const MachineOperand &Op = MI.getOperand(I);
        if (!Op.isReg())
          continue;

        if (!Op.isDef())
          continue;

        if (!Op.getReg().isVirtual())
          continue;

        if (VRM.hasRequiredPhys(Op.getReg()))
          continue;

        int TiedTo = MCID.getOperandConstraint(I, MCOI::TIED_TO);
        if (TiedTo != -1)
          continue;
        Register Reg = Op.getReg();

        bool FoundPhyReg =
            UsedPhyRegs.end() != find(UsedPhyRegs, VRM.getPhys(Reg));
        bool FoundVirtReg =
            UsedVirtRegs.end() ==
            find(UsedVirtRegs, llvm::Register::virtReg2Index(Reg));

        if (MRI->getRegClass(Reg) && FoundPhyReg && FoundVirtReg &&
            !MI.isCopy()) {
          // get physical register to replace candidate
          auto Replacements = getPriorityReplacementRegs(
              MBB, Reg, VRM, LRM, *MRI, LIS, TRI, ForbiddenPhysRegs);
          if (!Replacements.empty()) {
            PriorityRenameRegs[&Op] = Replacements;
            LLVM_DEBUG(dbgs()
                       << "Potential WAR Issue: Physical Register Reused: "
                       << printReg(Reg, TRI, 0, MRI) << '\n');
          } else {
            LLVM_DEBUG(dbgs()
                       << "Potential WAR Issue: Could not find replacement for "
                       << printReg(Reg, TRI, 0, MRI) << '\n');
          }
        } else {
          // keep track of already visted physical and virtual Regs
          UsedPhyRegs.insert(VRM.getPhys(Reg));
          UsedVirtRegs.insert(llvm::Register::virtReg2Index(Reg));
        }
      }
    }
  }

  LLVM_DEBUG(
      dbgs() << "Dumping found Priority Registers and their replacements\n";);
  for (const auto &KeyValuePair : PriorityRenameRegs) {
    Register Reg = KeyValuePair.first->getReg();
    const std::set<MCPhysReg> &AlternativePhyRegs = KeyValuePair.second;

    LLVM_DEBUG(dbgs() << "Renaming Candidate " << printReg(Reg, TRI, 0, MRI)
                      << " Ordering";
               for (const MCPhysReg &PhysReg
                    : AlternativePhyRegs) {
                 dbgs() << " " << printReg(PhysReg, TRI, 0, MRI);
               } dbgs()
               << "\n";);
  }
  return PriorityRenameRegs;
}

static std::set<MCPhysReg>
getBBForbiddenReplacements(const MachineBasicBlock &MBB, const VirtRegMap &VRM,
                           const MachineRegisterInfo &MRI,
                           const AIEBaseRegisterInfo *TRI,
                           const std::set<MCPhysReg> &CSPhyRegs) {
  std::set<MCPhysReg> BlockedPhysRegs;
  MCPhysReg PhysReg;
  for (auto &MI : MBB) {
    for (auto &Op : MI.operands()) {

      if (!Op.isReg())
        continue;

      if (!Op.isDef())
        continue;

      if (Op.getReg().isVirtual())
        PhysReg = VRM.getPhys(Op.getReg());
      else
        PhysReg = Op.getReg();

      BlockedPhysRegs.insert(PhysReg);
    }
  }

  BlockedPhysRegs.insert(CSPhyRegs.begin(), CSPhyRegs.end());
  return BlockedPhysRegs;
}

static std::set<MCPhysReg> getPriorityReplacementRegs(
    const MachineBasicBlock &MBB, const Register Reg, const VirtRegMap &VRM,
    LiveRegMatrix &LRM, const MachineRegisterInfo &MRI,
    const LiveIntervals &LIS, const AIEBaseRegisterInfo *TRI,
    const std::set<MCPhysReg> &ForbiddenPhysRegs) {
  std::set<MCPhysReg> PhysRegs;

  const TargetRegisterClass *RC = MRI.getRegClass(Reg);

  for (const MCPhysReg &PhysReg : RC->getRegisters()) {
    // iterate Regs of RC and allocate first found reg that does not interfere
    if (ForbiddenPhysRegs.end() != find(ForbiddenPhysRegs, PhysReg))
      continue;

    const llvm::LiveInterval &LI = LIS.getInterval(Reg);
    LiveRegMatrix::InterferenceKind IK = LRM.checkInterference(LI, PhysReg);
    if (IK == llvm::LiveRegMatrix::IK_Free)
      PhysRegs.insert(PhysReg);
  }
  return PhysRegs;
}

static std::set<MCPhysReg> getCSPhyRegs(const MachineRegisterInfo &MRI,
                                        const AIEBaseRegisterInfo *TRI) {
  std::set<MCPhysReg> CSPhyRegs;
  LLVM_DEBUG(dbgs() << "Callee-saved registers:\n");
  const MCPhysReg *CSRegs = MRI.getCalleeSavedRegs();
  for (const uint16_t *RegPtr = CSRegs; *RegPtr; ++RegPtr) {
    CSPhyRegs.insert(*RegPtr);
    LLVM_DEBUG(dbgs() << printReg(*RegPtr, TRI, 0, &MRI) << " ");
  }
  LLVM_DEBUG(dbgs() << "\n");
  return CSPhyRegs;
}

bool containsLoop(const MachineFunction &MF) {
  for (const MachineBasicBlock &MBB : MF) {
    if (MBB.succ_end() != find(MBB.successors(), &MBB))
      return true;
  }
  return false;
}

bool isCopyElimination(const MachineInstr &MI, const VirtRegMap &VRM) {
  if (!MI.isCopy())
    return false;

  // if src and target are equal, this copy will be eliminated, and
  // therefore should not renamed or even registered
  assert(MI.getNumOperands() == 2 &&
         "Copy MI does not have 2 operands, Naive Copy Elimination will "
         "not work! Please fix.");
  Register DstReg = MI.getOperand(0).getReg();
  Register SrcReg = MI.getOperand(1).getReg();
  MCPhysReg DstPhyReg, SrcPhyReg;
  if (DstReg.isVirtual())
    DstPhyReg = VRM.getPhys(DstReg);
  else
    DstPhyReg = DstReg.asMCReg();

  if (SrcReg.isVirtual())
    SrcPhyReg = VRM.getPhys(SrcReg);
  else
    SrcPhyReg = SrcReg.asMCReg();

  if (DstPhyReg == SrcPhyReg)
    return true;

  return false;
}

} // end anonymous namespace

char AIEWawRegRewriter::ID = 0;
char &llvm::AIEWawRegRewriterID = AIEWawRegRewriter::ID;

INITIALIZE_PASS(AIEWawRegRewriter, "aie-wawreg-rewrite", "AIE waw-reg rewrite",
                false, false)

llvm::FunctionPass *llvm::createAIEWawRegRewriter() {
  return new AIEWawRegRewriter();
}
