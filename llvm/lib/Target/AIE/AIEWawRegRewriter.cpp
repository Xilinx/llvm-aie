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
#include "llvm/CodeGen/MachineLoopInfo.h"
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

cl::opt<bool> DisablePass(
    "aie-waw-disable",
    cl::desc("Disable the WAW Register Renaming in loops (default false)"),
    cl::init(false), cl::Hidden);

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
    AU.addRequired<VirtRegMap>();
    AU.addPreserved<VirtRegMap>();
    AU.addRequired<SlotIndexes>();
    AU.addPreserved<SlotIndexes>();
    AU.addRequired<LiveIntervals>();
    AU.addPreserved<LiveIntervals>();
    AU.addRequired<LiveRegMatrix>();
    AU.addPreserved<LiveRegMatrix>();
    AU.addRequired<MachineLoopInfo>();
    AU.addPreserved<MachineLoopInfo>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &Fn) override;

private:
};

/// Find Free register of the same register Class type and
// exclude the forbidden Physical register from the result

static llvm::SmallVector<MCPhysReg, 16> getPriorityReplacementRegs(
    const Register Reg,
    const llvm::SmallVector<MCPhysReg, 16> &ForbiddenPhysRegs,
    const llvm::SmallVector<MCPhysReg, 16> &CSPhyRegs,
    const MachineFunction &MF, const VirtRegMap &VRM, LiveRegMatrix &LRM,
    const MachineRegisterInfo &MRI, const LiveIntervals &LIS,
    const AIEBaseRegisterInfo &TRI);

static bool
replaceReg(const Register Reg,
           const llvm::SmallVector<MCPhysReg, 16> &ForbiddenPhysRegs,
           llvm::SmallVector<MCPhysReg, 16> &AlreadyReplaced,
           const llvm::SmallVector<MCPhysReg, 16> &CSPhyRegs,
           const MachineFunction &MF, VirtRegMap &VRM, LiveRegMatrix &LRM,
           const MachineRegisterInfo &MRI, const LiveIntervals &LIS,
           const AIEBaseRegisterInfo &TRI);

static llvm::SmallVector<const MachineBasicBlock *, 4>
getLoopBBs(const MachineLoopInfo &MLI);
static bool isWAWCandidate(const Register &Reg, const MachineInstr &MI,
                           llvm::SmallVector<MCPhysReg, 16> &UsedPhyRegs,
                           llvm::SmallVector<MCPhysReg, 16> &UsedVirtRegs,
                           VirtRegMap &VRM, MachineRegisterInfo &MRI);

static bool containsLoop(const MachineFunction &MF);
bool isCopyElimination(const MachineInstr &MI, const VirtRegMap &VRM);

static llvm::SmallVector<MCPhysReg, 16>
getBBForbiddenReplacements(const MachineBasicBlock *MBB, const VirtRegMap &VRM,
                           const MachineRegisterInfo &MRI,
                           const AIEBaseRegisterInfo *TRI,
                           const llvm::SmallVector<MCPhysReg, 16> &CSPhyRegs);

static bool couldBeCalleeSaveEvasiveRegister(const MCPhysReg &PhysReg,
                                             Register Reg,
                                             const MachineFunction &MF,
                                             const LiveRegMatrix &LRM,
                                             const MachineRegisterInfo &MRI,
                                             const AIEBaseRegisterInfo &TRI);

bool AIEWawRegRewriter::runOnMachineFunction(MachineFunction &MF) {
  if (DisablePass)
    return false;

  if (!containsLoop(MF))
    return false;

  LLVM_DEBUG(llvm::dbgs() << "*** WAW Loop Register Rewriting: "
                          << MF.getName());
  MF.dump();
  LLVM_DEBUG(llvm::dbgs() << " ***\n");

  MachineRegisterInfo &MRI = MF.getRegInfo();
  auto &TRI =
      *static_cast<const AIEBaseRegisterInfo *>(MRI.getTargetRegisterInfo());
  VirtRegMap &VRM = getAnalysis<VirtRegMap>();
  LiveRegMatrix &LRM = getAnalysis<LiveRegMatrix>();
  LiveIntervals &LIS = getAnalysis<LiveIntervals>();
  MachineLoopInfo &MLI = getAnalysis<MachineLoopInfo>();
  std::map<Register, MCRegister> AssignedPhysRegs;
  bool Modified = false;
  LLVM_DEBUG(VRM.dump());

  llvm::SmallVector<MCPhysReg, 16> CSPhyRegs = TRI.getCSPhyRegs(MF);
  llvm::SmallVector<const MachineBasicBlock *, 4> LoopBBs = getLoopBBs(MLI);

  for (const MachineBasicBlock *MBB : LoopBBs) {
    llvm::SmallVector<MCPhysReg, 16> UsedPhyRegs;
    llvm::SmallVector<MCPhysReg, 16> UsedVirtRegs;
    llvm::SmallVector<MCPhysReg, 16> AlreadyReplaced;

    llvm::SmallVector<MCPhysReg, 16> ForbiddenPhysRegs =
        getBBForbiddenReplacements(MBB, VRM, MRI, &TRI, CSPhyRegs);

    for (const MachineInstr &MI : *MBB) {
      MI.dump();
      if (isCopyElimination(MI, VRM))
        continue;

      for (const MachineOperand &Op : MI.defs()) {

        if (!Op.getReg().isVirtual())
          continue;

        if (VRM.hasRequiredPhys(Op.getReg()))
          continue;

        if (Op.isTied())
          continue;

        Register Reg = Op.getReg();

        if (isWAWCandidate(Reg, MI, UsedPhyRegs, UsedVirtRegs, VRM, MRI)) {
          bool Replaced = replaceReg(Reg, ForbiddenPhysRegs, AlreadyReplaced,
                                     CSPhyRegs, MF, VRM, LRM, MRI, LIS, TRI);
          if (Replaced)
            Modified = true;

        } else {
          // keep track of already visted physical and virtual Regs
          UsedPhyRegs.push_back(VRM.getPhys(Reg));
          UsedVirtRegs.push_back(llvm::Register::virtReg2Index(Reg));
        }
      }
    }
  }

  LLVM_DEBUG(VRM.dump());
  return Modified;
}

static llvm::SmallVector<const MachineBasicBlock *, 4>
getLoopBBs(const MachineLoopInfo &MLI) {
  llvm::SmallVector<const MachineBasicBlock *, 4> LoopBBs;
  for (const MachineLoop *ML : MLI) {
    if (ML->isInnermost())
      LoopBBs.append(ML->block_begin(), ML->block_end());
  }
  return LoopBBs;
}

static bool isWAWCandidate(const Register &Reg, const MachineInstr &MI,
                           llvm::SmallVector<MCPhysReg, 16> &UsedPhyRegs,
                           llvm::SmallVector<MCPhysReg, 16> &UsedVirtRegs,
                           VirtRegMap &VRM, MachineRegisterInfo &MRI) {
  bool FoundPhyReg = UsedPhyRegs.end() != find(UsedPhyRegs, VRM.getPhys(Reg));
  bool FoundVirtReg = UsedVirtRegs.end() ==
                      find(UsedVirtRegs, llvm::Register::virtReg2Index(Reg));

  return MRI.getRegClass(Reg) && FoundPhyReg && FoundVirtReg && !MI.isCopy();
}

static llvm::SmallVector<MCPhysReg, 16>
getBBForbiddenReplacements(const MachineBasicBlock *MBB, const VirtRegMap &VRM,
                           const MachineRegisterInfo &MRI,
                           const AIEBaseRegisterInfo *TRI,
                           const llvm::SmallVector<MCPhysReg, 16> &CSPhyRegs) {
  llvm::SmallVector<MCPhysReg, 16> BlockedPhysRegs;
  MCPhysReg PhysReg;
  for (auto &MI : *MBB) {
    for (auto &Op : MI.defs()) {

      if (Op.getReg().isVirtual())
        PhysReg = VRM.getPhys(Op.getReg());
      else
        PhysReg = Op.getReg();

      BlockedPhysRegs.push_back(PhysReg);
    }
  }

  BlockedPhysRegs.append(CSPhyRegs.begin(), CSPhyRegs.end());
  return BlockedPhysRegs;
}

bool replaceReg(const Register Reg,
                const llvm::SmallVector<MCPhysReg, 16> &ForbiddenPhysRegs,
                llvm::SmallVector<MCPhysReg, 16> &AlreadyReplaced,
                const llvm::SmallVector<MCPhysReg, 16> &CSPhyRegs,
                const MachineFunction &MF, VirtRegMap &VRM, LiveRegMatrix &LRM,
                const MachineRegisterInfo &MRI, const LiveIntervals &LIS,
                const AIEBaseRegisterInfo &TRI) {
  LLVM_DEBUG(dbgs() << " WAW RegRewriter: Register to replace"
                    << TRI.getName(VRM.getPhys(Reg)) << "\n");
  llvm::SmallVector<MCPhysReg, 16> Replacements = getPriorityReplacementRegs(
      Reg, ForbiddenPhysRegs, CSPhyRegs, MF, VRM, LRM, MRI, LIS, TRI);

  for (const MCPhysReg &PhysReg : Replacements) {
    if (find(AlreadyReplaced, PhysReg) != AlreadyReplaced.end())
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
      AlreadyReplaced.push_back(PhysReg);
      return true;
    }
  }
  return false;
}

llvm::SmallVector<MCPhysReg, 16> getPriorityReplacementRegs(
    const Register Reg,
    const llvm::SmallVector<MCPhysReg, 16> &ForbiddenPhysRegs,
    const llvm::SmallVector<MCPhysReg, 16> &CSPhyRegs,
    const MachineFunction &MF, const VirtRegMap &VRM, LiveRegMatrix &LRM,
    const MachineRegisterInfo &MRI, const LiveIntervals &LIS,
    const AIEBaseRegisterInfo &TRI) {
  llvm::SmallVector<MCPhysReg, 16> PhysRegs;

  const TargetRegisterClass *RC = MRI.getRegClass(Reg);

  for (const MCPhysReg &PhysReg : RC->getRegisters()) {
    // iterate Regs of RC and allocate first found reg that does not
    // interfere
    if (find(ForbiddenPhysRegs, PhysReg) != ForbiddenPhysRegs.end())
      continue;

    if (couldBeCalleeSaveEvasiveRegister(PhysReg, Reg, MF, LRM, MRI, TRI))
      continue;

    const llvm::LiveInterval &LI = LIS.getInterval(Reg);
    LiveRegMatrix::InterferenceKind IK = LRM.checkInterference(LI, PhysReg);
    if (IK == llvm::LiveRegMatrix::IK_Free)
      PhysRegs.push_back(PhysReg);
  }

  std::sort(PhysRegs.begin(), PhysRegs.end(), std::less<MCPhysReg>());
  LLVM_DEBUG(dbgs() << "Renaming Candidate " << printReg(Reg, &TRI, 0, &MRI)
                    << " Ordering";
             for (const MCPhysReg &PhysReg
                  : PhysRegs) {
               dbgs() << " " << printReg(PhysReg, &TRI, 0, &MRI);
             } dbgs()
             << "\n";);
  return PhysRegs;
}

bool containsLoop(const MachineFunction &MF) {
  for (const MachineBasicBlock &MBB : MF) {
    if (MBB.succ_end() != find(MBB.successors(), &MBB))
      return true;
  }
  return false;
}

static bool couldBeCalleeSaveEvasiveRegister(const MCPhysReg &PhysReg,
                                             Register Reg,
                                             const MachineFunction &MF,
                                             const LiveRegMatrix &LRM,
                                             const MachineRegisterInfo &MRI,
                                             const AIEBaseRegisterInfo &TRI) {
  const TargetRegisterClass *CalleeSaveClass = TRI.getCalleeSaveRegClass(MF);
  const TargetRegisterClass *GPRClass = MRI.getRegClass(Reg);
  if (CalleeSaveClass == GPRClass) {
    return !LRM.isPhysRegUsed(PhysReg);
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
