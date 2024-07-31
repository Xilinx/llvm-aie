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

#include "AIEBaseInstrInfo.h"
#include "AIEBaseRegisterInfo.h"

#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/CodeGen/LiveDebugVariables.h"
#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/LiveRegMatrix.h"
#include "llvm/CodeGen/LiveStacks.h"
#include "llvm/CodeGen/MachineBlockFrequencyInfo.h"
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
    AU.addPreserved<MachineBlockFrequencyInfo>();
    AU.addRequired<VirtRegMap>();
    AU.addPreserved<VirtRegMap>();
    AU.addRequired<SlotIndexes>();
    AU.addPreserved<SlotIndexes>();
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
  std::map<const MachineOperand *, std::set<MCPhysReg>>
  getWAR_RenameRegs(const MachineFunction &MF, const VirtRegMap &VRM,
                    LiveRegMatrix &LRM, const AIEBaseRegisterInfo *TRI,
                    const MachineRegisterInfo *MRI,
                    const LiveIntervals &LIS) const;
};

static std::set<MCPhysReg> getPriorityReplacementRegs(
    const MachineBasicBlock &MBB, const Register Reg, const VirtRegMap &VRM,
    LiveRegMatrix &LRM, const MachineRegisterInfo &MRI,
    const LiveIntervals &LIS, const AIEBaseRegisterInfo *TRI);

bool AIEWawRegRewriter::runOnMachineFunction(MachineFunction &MF) {
  LLVM_DEBUG(llvm::dbgs() << "*** WAW Loop Register Rewriting: " << MF.getName()
                          << " ***\n");

  MachineRegisterInfo &MRI = MF.getRegInfo();
  auto &TRI =
      *static_cast<const AIEBaseRegisterInfo *>(MRI.getTargetRegisterInfo());
  VirtRegMap &VRM = getAnalysis<VirtRegMap>();
  LiveRegMatrix &LRM = getAnalysis<LiveRegMatrix>();
  LiveIntervals &LIS = getAnalysis<LiveIntervals>();
  std::map<Register, MCRegister> AssignedPhysRegs;
  bool Modified = false;

  std::map<const MachineOperand *, std::set<MCPhysReg>> PriorityRenameRegs =
      getWAR_RenameRegs(MF, VRM, LRM, &TRI, &MRI, LIS);

  // Collect already-assigned VRegs that can be split into smaller ones.
  LLVM_DEBUG(VRM.dump());
  std::set<Register> AlreadyReplaced;
  for (const auto &KeyValuePair : PriorityRenameRegs) {
    Register Reg = KeyValuePair.first->getReg();
    const std::set<MCPhysReg> &AlternativePhyRegs = KeyValuePair.second;

    LLVM_DEBUG(dbgs() << "\nAnalysing " << printReg(Reg, &TRI, 0, &MRI) << ":"
                      << printRegClassOrBank(Reg, MRI, &TRI) << " ";
               KeyValuePair.first->getParent()->print(dbgs()); dbgs());
    // iterate RC and allocate first found reg that does not interfere
    for (const MCPhysReg &PhysReg : AlternativePhyRegs) {
      const llvm::LiveInterval &LI = LIS.getInterval(Reg);
      LiveRegMatrix::InterferenceKind IK = LRM.checkInterference(LI, PhysReg);
      if (IK == llvm::LiveRegMatrix::IK_Free &&
          AlreadyReplaced.end() == find(AlreadyReplaced, PhysReg)) {
        LLVM_DEBUG(dbgs() << "Potential WAR Issue: Virtual Register "
                          << llvm::Register::virtReg2Index(Reg)
                          << " will replace " << TRI.getName(VRM.getPhys(Reg))
                          << " with " << printReg(PhysReg, &TRI, 0, &MRI)
                          << '\n');
        VRM.clearVirt(Reg);
        VRM.assignVirt2Phys(Reg, PhysReg);
        AlreadyReplaced.insert(PhysReg);
        Modified = true;

        break;
      }
    }
  }

  LLVM_DEBUG(VRM.dump());
  return Modified;
}

/// Find Virtual Register that should not map to the same physical register,
/// even though they don't interfere. This is done, so that pipelining can be
/// performed. e.g.
std::map<const MachineOperand *, std::set<MCPhysReg>>
AIEWawRegRewriter::getWAR_RenameRegs(MachineFunction const &MF,
                                     const VirtRegMap &VRM, LiveRegMatrix &LRM,
                                     const AIEBaseRegisterInfo *TRI,
                                     const MachineRegisterInfo *MRI,
                                     const LiveIntervals &LIS) const {
  std::map<const MachineOperand *, std::set<MCPhysReg>> PriorityRenameRegs;
  for (const MachineBasicBlock &MBB : MF) {
    // collect any register, which that are redefined within te same BB
    if (MBB.succ_end() != find(MBB.successors(), &MBB)) {
      // check if the virtualRegs map to the same physical register
      std::set<Register> UsedPhyRegs;
      std::set<Register> UsedVirtRegs;

      // get all reg defines and collect redefines
      for (const MachineInstr &MI : MBB) {
        const MCInstrDesc &MCID = MI.getDesc();
        MI.dump();
        for (unsigned I = 0, E = MI.getNumOperands(); I != E; ++I) {
          const MachineOperand &Op = MI.getOperand(I);
          if (Op.isReg())
            LLVM_DEBUG(dbgs() << "Checking "
                              << printReg(Op.getReg(), TRI, 0, MRI) << "\n";);
          if (Op.isReg() && Op.isDef() && Op.getReg().isVirtual()) {
            int TiedTo = MCID.getOperandConstraint(I, MCOI::TIED_TO);
            if (TiedTo == -1) {
              Register Reg = Op.getReg();
              // check if def of the register is already in the same MBB
              bool FoundPhyReg =
                  UsedPhyRegs.end() != find(UsedPhyRegs, VRM.getPhys(Reg));
              bool FoundVirtReg =
                  UsedVirtRegs.end() ==
                  find(UsedVirtRegs, llvm::Register::virtReg2Index(Reg));

              if (MRI->getRegClass(Reg) && FoundPhyReg && FoundVirtReg) {
                auto Replacements = getPriorityReplacementRegs(
                    MBB, Reg, VRM, LRM, *MRI, LIS, TRI);
                if (!Replacements.empty()) {
                  PriorityRenameRegs[&Op] = Replacements;
                  LLVM_DEBUG(
                      dbgs()
                      << "Potential WAR Issue: Physical Register Reused: "
                      << printReg(Reg, TRI, 0, MRI) << '\n');
                } else {
                  LLVM_DEBUG(
                      dbgs()
                      << "Potential WAR Issue: Could not find replacement for "
                      << printReg(Reg, TRI, 0, MRI) << '\n');
                }
              } else {
                // keep track of already visted physical and virtual Regs
                UsedPhyRegs.insert(VRM.getPhys(Reg));
                UsedVirtRegs.insert(llvm::Register::virtReg2Index(Reg));
                LLVM_DEBUG(dbgs()
                           << "Potential WAR Issue: New Register will add to "
                              "Visited Nodes: "
                           << printReg(Reg, TRI, 0, MRI) << " With VirtReg of "
                           << llvm::Register::virtReg2Index(Reg)
                           << " and PhyReg of " << VRM.getPhys(Reg) << '\n');
              }
            }
          }
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

static std::set<MCPhysReg> getPriorityReplacementRegs(
    const MachineBasicBlock &MBB, const Register Reg, const VirtRegMap &VRM,
    LiveRegMatrix &LRM, const MachineRegisterInfo &MRI,
    const LiveIntervals &LIS, const AIEBaseRegisterInfo *TRI) {
  std::set<MCPhysReg> PhysRegs;
  std::set<MCPhysReg> BBDefs;
  for (auto &MI : MBB) {
    for (auto &Op : MI.operands()) {
      if (Op.isReg() && Op.isDef()) {
        if (Op.getReg().isVirtual()) {
          BBDefs.insert(VRM.getPhys(Op.getReg()));
        } else if (Op.getReg().isPhysical()) {
          BBDefs.insert(Op.getReg());
        } else {
          LLVM_DEBUG(dbgs() << "Neither virtual nor physical register: ";
                     Op.print(dbgs(), TRI));
        }
      }
    }
  }

  // get all free register that dont interfere, but ignore defs from the MBB
  const TargetRegisterClass *RC = MRI.getRegClass(Reg);
  // iterate RC and allocate first found reg that does not interfere
  for (const MCPhysReg &PhysReg : RC->getRegisters()) {
    const llvm::LiveInterval &LI = LIS.getInterval(Reg);
    LiveRegMatrix::InterferenceKind IK = LRM.checkInterference(LI, PhysReg);
    if (IK == llvm::LiveRegMatrix::IK_Free &&
        BBDefs.end() == find(BBDefs, Reg)) {
      PhysRegs.insert(PhysReg);
    }
  }

  return PhysRegs;
}

} // end anonymous namespace

char AIEWawRegRewriter::ID = 0;
char &llvm::AIEWawRegRewriterID = AIEWawRegRewriter::ID;

INITIALIZE_PASS(AIEWawRegRewriter, "aie-wawreg-rewrite", "AIE waw-reg rewrite",
                false, false)

llvm::FunctionPass *llvm::createAIEWawRegRewriter() {
  return new AIEWawRegRewriter();
}
