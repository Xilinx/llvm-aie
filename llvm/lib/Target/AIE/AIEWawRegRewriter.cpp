//===----- AIEWawRegRewriter.cpp - Rewrite Regs to remove Defs ---------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This pass rewrites physical register assignments in critical parts of the
// code (like loops) to break WAW and WAR dependencies.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseInstrInfo.h"
#include "AIEBaseRegisterInfo.h"
#include "Utils/AIELoopUtils.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/LiveDebugVariables.h"
#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/LiveRegMatrix.h"
#include "llvm/CodeGen/LiveStacks.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/SlotIndexes.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/CodeGen/MachineBasicBlock.h>

using namespace llvm;

#define DEBUG_TYPE "aie-waw-reg-rewrite"

namespace {

///
/// This pass rewrites physical register assignments in critical parts of the
/// code (like loops) to break WAW and WAR dependencies.
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
    // no Machine Instructions are added, therefore the SlotIndexes remain
    // constant and preserved
    AU.addRequired<SlotIndexes>();
    AU.addPreserved<SlotIndexes>();
    // no new Virtual Registers are generated, therefore the LiveDebugVariables
    // do not have to be updated
    AU.addRequired<LiveDebugVariables>();
    AU.addPreserved<LiveDebugVariables>();
    AU.addRequired<LiveStacks>();
    AU.addPreserved<LiveStacks>();
    AU.addRequired<LiveIntervals>();
    AU.addPreserved<LiveIntervals>();
    AU.addRequired<LiveRegMatrix>();
    AU.addPreserved<LiveRegMatrix>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &Fn) override;

private:
  MachineFunction *MF = nullptr;
  const MachineRegisterInfo *MRI = nullptr;
  const AIEBaseRegisterInfo *TRI = nullptr;
  VirtRegMap *VRM = nullptr;
  LiveRegMatrix *LRM = nullptr;
  LiveIntervals *LIS = nullptr;
  const TargetInstrInfo *TII = nullptr;

  bool renameMBBPhysRegs(const MachineBasicBlock *MBB);

  /// Get all the defined physical registers that the MachineBasicBlock already
  /// uses. These physical registers should not be used for replacement
  /// candidates, since this would introduce new WAW dependencies, which this
  /// pass tries to remove.
  BitVector getDefinedPhysRegs(const MachineBasicBlock *MBB) const;

  /// returns true if the physical register of Reg was replaced
  bool replaceReg(const Register Reg, BitVector &BlockedPhysRegs);

  /// Find a free register of the same register class type, but
  /// exclude the blocked physical registers from the result.
  /// Otherwise a new WAW dependencies can be introduced, that was previously
  /// removed.
  MCPhysReg getReplacementPhysReg(const Register Reg,
                                  const BitVector &BlockedPhysRegs) const;

  bool isWorthRenaming(const Register &Reg, const BitVector &UsedPhysRegs,
                       const BitVector &VRegWithCopies) const;

  /// return the Physical register of the Register, look it up in VirtRegMap if
  /// the Reg is virtual
  MCPhysReg getAssignedPhysReg(const Register Reg) const;

  bool isIdentityCopy(const MachineInstr &MI) const;

  /// return a BitVector to identify if a VirtualRegister has been defined by at
  /// least one copy.
  /// The Virtual Registers are accessed by the VirtRegIndex
  BitVector getVRegWithCopies(const MachineBasicBlock &MBB) const;
};

MCPhysReg AIEWawRegRewriter::getAssignedPhysReg(const Register Reg) const {
  assert(Reg.isPhysical() || Reg.isVirtual());

  if (Reg.isVirtual())
    return VRM->getPhys(Reg);

  return Reg;
}

bool AIEWawRegRewriter::runOnMachineFunction(MachineFunction &MF) {

  SmallVector<const MachineBasicBlock *, 4> LoopMBBs =
      AIELoopUtils::getSingleBlockLoopMBBs(MF);

  if (LoopMBBs.empty())
    return false;

  this->MF = &MF;
  MRI = &MF.getRegInfo();
  TRI = static_cast<const AIEBaseRegisterInfo *>(MRI->getTargetRegisterInfo());
  VRM = &getAnalysis<VirtRegMap>();
  LRM = &getAnalysis<LiveRegMatrix>();
  LIS = &getAnalysis<LiveIntervals>();
  TII = MF.getSubtarget().getInstrInfo();
  bool Modified = false;

  LLVM_DEBUG(dbgs() << "*** WAW Loop Register Rewriting: " << MF.getName());
  LLVM_DEBUG(dbgs() << " ***\n");

  for (const MachineBasicBlock *MBB : LoopMBBs)
    Modified |= renameMBBPhysRegs(MBB);

  return Modified;
}

BitVector
AIEWawRegRewriter::getVRegWithCopies(const MachineBasicBlock &MBB) const {
  // FIXME: The current heuristic would still rename a register used in a copy
  // if there are several inner loops in the kernel. A solution could be to keep
  // a running list of VRegWithCopies across various MBBs.

  SmallVector<unsigned, 16> VRegs;
  uint MaxVReg = 0;
  for (const MachineInstr &MI : MBB) {
    for (const MachineOperand Def : MI.defs()) {
      Register Reg = Def.getReg();

      if (!Reg.isVirtual())
        continue;

      unsigned VRegIndex = Reg.virtRegIndex();
      if (TII->isCopyInstr(MI).has_value())
        VRegs.push_back(VRegIndex);

      if (VRegIndex > MaxVReg)
        MaxVReg = VRegIndex;
    }
  }

  // copy to BitVector so that lookups become very cheap
  BitVector VRegWithCopies(MaxVReg + 1);
  for (const unsigned RegIndex : VRegs)
    VRegWithCopies[RegIndex] = true;

  return VRegWithCopies;
}

bool AIEWawRegRewriter::renameMBBPhysRegs(const MachineBasicBlock *MBB) {
  LLVM_DEBUG(dbgs() << "WAW Reg Renaming BasicBlock "; MBB->dump();
             dbgs() << "\n");
  bool Modified = false;
  // Add the used physical registers one machine instruction at a time.
  // This vector is used to determine, if a physical register has already been
  // defined in the machine basic block.
  BitVector UsedPhysRegs(TRI->getNumRegs());

  // Get a list of registers, that are not allowed as a replacement register.
  // This list gets updated with the newly replaced physical register, so that
  // this pass does not introduce WAW dependencies.
  BitVector BlockedPhysRegs = getDefinedPhysRegs(MBB);

  // Collect all the virtual registers that have at least a copy instruction
  // that defines them. Subregisters may contain constants that may be shared
  // across different virtual registers. Renaming would reintroduce unnecessary
  // copies, if physical registers are shared. Also do not rename copies, since
  // they could be removed in a later pass.
  BitVector VRegWithCopies = getVRegWithCopies(*MBB);

  for (const MachineInstr &MI : *MBB) {

    // Identity copies will be removed in a later pass, therefore, these are not
    // real defines of a physical register
    if (isIdentityCopy(MI))
      continue;

    for (const MachineOperand &MO : MI.defs()) {

      Register Reg = MO.getReg();
      if (!Reg.isVirtual())
        continue;
      if (VRM->hasRequiredPhys(Reg))
        continue;
      if (MO.isTied())
        continue;

      if (isWorthRenaming(Reg, UsedPhysRegs, VRegWithCopies) &&
          replaceReg(Reg, BlockedPhysRegs)) {

        LLVM_DEBUG(dbgs() << MI);
        Modified = true;

      } else {
        // Keep track of already visited physical registers.
        // Incrementally add the encountered physical registers, so that a
        // second occurrence of a physical register can trigger the register
        // rewriting
        // Blocked registers for the replacement are recorded in
        // BlockedPhysRegs. Initially all the used physical registers
        // from the MBB are blocked, so that replacements do not introduce WAW
        // dependencies. Additionally, replaced registers are already blocked in
        // BlockedPhysRegs, so that an additional replacement will not cause a
        // WAW, which this pass is trying to remove.
        UsedPhysRegs[VRM->getPhys(Reg)] = true;
      }
    }
  }

  return Modified;
}

bool AIEWawRegRewriter::isWorthRenaming(const Register &Reg,
                                        const BitVector &UsedPhysRegs,
                                        const BitVector &VRegWithCopies) const {
  assert(Reg.isVirtual());

  // Only rename registers mapped to a phys reg assigned more than once
  if (!UsedPhysRegs[VRM->getPhys(Reg)])
    return false;

  return !VRegWithCopies[Reg.virtRegIndex()];
}

BitVector
AIEWawRegRewriter::getDefinedPhysRegs(const MachineBasicBlock *MBB) const {
  BitVector BlockedPhysRegs(TRI->getNumRegs());

  for (const MachineInstr &MI : *MBB) {
    for (const MachineOperand &Op : MI.defs()) {
      MCPhysReg PhysReg = getAssignedPhysReg(Op.getReg());
      if (MCRegister::isPhysicalRegister(PhysReg))
        BlockedPhysRegs[PhysReg] = true;
    }
  }

  return BlockedPhysRegs;
}

bool AIEWawRegRewriter::replaceReg(const Register Reg,
                                   BitVector &BlockedPhysRegs) {
  assert(Reg.isVirtual());
  LLVM_DEBUG(dbgs() << " WAW RegRewriter: Register to replace "
                    << TRI->getName(VRM->getPhys(Reg)) << "\n");

  MCPhysReg ReplacementPhysReg = getReplacementPhysReg(Reg, BlockedPhysRegs);

  if (ReplacementPhysReg == MCRegister::NoRegister)
    return false;

  LLVM_DEBUG(dbgs() << "     WAW Replacement: Virtual Register "
                    << printReg(VRM->getPhys(Reg), TRI, 0, MRI)
                    << " will replace "
                    << printReg(VRM->getPhys(Reg), TRI, 0, MRI) << " with "
                    << printReg(ReplacementPhysReg, TRI, 0, MRI) << '\n');

  const LiveInterval &LI = LIS->getInterval(Reg);
  LRM->unassign(LI);
  LRM->assign(LI, ReplacementPhysReg);
  BlockedPhysRegs[ReplacementPhysReg] = true;
  return true;
}

MCPhysReg AIEWawRegRewriter::getReplacementPhysReg(
    const Register Reg, const BitVector &BlockedPhysRegs) const {
  assert(Reg.isVirtual() && "Reg has to be a virtual register");
  const TargetRegisterClass *RC = MRI->getRegClass(Reg);

  LiveInterval &LI = LIS->getInterval(Reg);
  for (const MCPhysReg &PhysReg : RC->getRegisters()) {

    if (BlockedPhysRegs[PhysReg])
      continue;

    LiveRegMatrix::InterferenceKind IK = LRM->checkInterference(LI, PhysReg);
    if (IK == LiveRegMatrix::IK_Free)
      return PhysReg;
  }
  return MCRegister::NoRegister;
}

bool AIEWawRegRewriter::isIdentityCopy(const MachineInstr &MI) const {
  std::optional<DestSourcePair> DestSource = TII->isCopyInstr(MI);
  if (!DestSource.has_value())
    return false;

  return getAssignedPhysReg(DestSource->Source->getReg()) ==
         getAssignedPhysReg(DestSource->Destination->getReg());
}

} // end anonymous namespace

char AIEWawRegRewriter::ID = 0;
char &llvm::AIEWawRegRewriterID = AIEWawRegRewriter::ID;

INITIALIZE_PASS(AIEWawRegRewriter, DEBUG_TYPE, "AIE waw-reg rewrite", false,
                false)

llvm::FunctionPass *llvm::createAIEWawRegRewriter() {
  return new AIEWawRegRewriter();
}
