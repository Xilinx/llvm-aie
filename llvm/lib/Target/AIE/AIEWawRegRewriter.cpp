//===----- AIEWawRegRewriter.cpp - Rewrite Regs to remove Defs ---------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2025 Advanced Micro Devices, Inc. or its affiliates
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

#include "llvm/ADT/BitVector.h"
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
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/CodeGen/MachineBasicBlock.h>

using namespace llvm;

#define DEBUG_TYPE "aie-waw-reg-rewrite"

static cl::opt<bool> AggressiveReAlloc(
    "aie-aggressive-realloc", cl::Hidden, cl::init(false),
    cl::desc("Aggressively de-allocate live-through registers to favor "
             "loop-local registers"));
static cl::opt<bool> GPRRealloc("aie-gpr-realloc", cl::Hidden, cl::init(false),
                                cl::desc("Re-allocate GPRs as well"));

namespace {

using RoundRobin = std::list<MCPhysReg>;

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

  /// Returns true if the physical register \p Reg was replaced
  bool replaceReg(const Register Reg, RoundRobin &Registers,
                  BitVector &UsedUnits);

  void unassignReg(Register Reg);
  void assignReg(Register Reg, MCPhysReg PhysReg);

  /// Find a free register of the same register class type
  MCPhysReg getReplacementPhysReg(const Register Reg, RoundRobin &Registers,
                                  BitVector &UsedUnits) const;

  /// Whether \p Reg should be considered a candidate for re-assignment.
  bool isWorthRenaming(const Register &Reg,
                       const BitVector &VRegWithCopies) const;

  /// return the Physical register of the Register, look it up in VirtRegMap if
  /// the Reg is virtual
  MCPhysReg getAssignedPhysReg(const Register Reg) const;

  bool isIdentityCopy(const MachineInstr &MI) const;

  /// return a BitVector to identify if a VirtualRegister has been defined by at
  /// least one copy.
  /// The Virtual Registers are accessed by the VirtRegIndex
  BitVector getVRegWithCopies(const MachineBasicBlock &MBB) const;

  /// MachineInstructions can write into sub-registers (%0:sub_256_lo and
  /// %0:sub_256_hi). In the VirtualRegisterMap, they block the same
  /// register (i.e. $x0). This can cause issues, when detecting Write
  /// After Write (WAW) dependencies. To mitigate this, only use the last
  /// definition of a virtual register to count the definitions, so that
  /// writing into subregisters does not already trigger the register
  /// replacement. In this case $x0 would have already been replaced, even
  /// though there is no real WAW dependency.
  ///                                                                          \
  /// undef %0.sub_256_lo:mxa, _, _= VLDA_2D_dmw_lda_w _,_                     \
  /// % 0.sub_256_hi:vec512 = VLDA_dmw_lda_w_ag_idx_imm _,_                    \
  /// %7:mxm, %8:el = VMAX_LT_D8 %0, $x6                                       \
  ///                                                                          \
  /// returns a mapping between a virtual register and its last defining machine
  /// instruction.
  IndexedMap<const MachineInstr *, VirtReg2IndexFunctor>
  getLastVRegDef(const MachineBasicBlock &MBB) const;
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
  unsigned MaxVReg = 0;
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

  // Collect all the virtual registers that have at least a copy instruction
  // that defines them. Subregisters may contain constants that may be shared
  // across different virtual registers. Renaming would reintroduce unnecessary
  // copies, if physical registers are shared. Also do not rename copies, since
  // they could be removed in a later pass.
  BitVector VRegWithCopies = getVRegWithCopies(*MBB);

  IndexedMap<const MachineInstr *, VirtReg2IndexFunctor> LastVRegDef =
      getLastVRegDef(*MBB);

  // Record the candidates and their original allocation
  using OriginalAllocation =
      std::vector<std::pair<const MachineOperand *, Register>>;
  OriginalAllocation Candidates;

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
      // several definitions of the same virtual register are not relevant
      // because even if the virtual register is renamed, by construction
      // all the definitions would be renamed as well and achieve nothing wrt
      // WAW dependency resolution
      if (LastVRegDef[Reg] != &MI)
        continue;

      if (isWorthRenaming(Reg, VRegWithCopies)) {
        assert(VRM->hasPhys(Reg));
        MCRegister AssignedPhysReg = VRM->getPhys(Reg);
        Candidates.emplace_back(&MO, AssignedPhysReg);
        LLVM_DEBUG(dbgs() << "Candidate " << printReg(Reg, TRI, 0, MRI) << ":"
                          << TRI->getRegClassName(MRI->getRegClass(Reg)) << " ("
                          << TRI->getName(AssignedPhysReg) << ")\n");
      }
    }
  }

  // Free physregs of all candidates and register their regclasses
  std::set<const TargetRegisterClass *> RegClasses;
  for (auto &[MO, Org] : Candidates) {
    auto VReg = MO->getReg();
    if (VRM->hasPhys(VReg))
      unassignReg(VReg);
    auto *RC = MRI->getRegClass(VReg);
    RegClasses.insert(RC);
  }
  LLVM_DEBUG(dbgs() << "Renaming " << Candidates.size() << " candidates in "
                    << RegClasses.size() << " classes\n");

  // If requested, unassign MBB's liveins as well to get even more freedom
  if (AggressiveReAlloc) {
    for (unsigned I = 0, E = MRI->getNumVirtRegs(); I != E; ++I) {
      Register Reg = Register::index2VirtReg(I);
      if (!LIS->hasInterval(Reg) || !RegClasses.count(MRI->getRegClass(Reg)))
        continue;
      LiveInterval &LI = LIS->getInterval(Reg);
      if (LIS->isLiveInToMBB(LI, MBB) && VRM->hasPhys(Reg)) {
        unassignReg(Reg);
      }
    }
  }

  // Reallocate all virtual registers in Candidates.
  // Return true if successful.
  auto ReAllocate = [&](OriginalAllocation &Candidates, RoundRobin &Registers) {
    BitVector UsedUnits;
    UsedUnits.resize(TRI->getNumRegUnits());
    for (auto &[MO, Org] : Candidates) {
      auto VReg = MO->getReg();
      if (!replaceReg(VReg, Registers, UsedUnits)) {
        LLVM_DEBUG(dbgs() << "Renaming " << printReg(VReg, TRI, 0, MRI)
                          << " failed\n");
        return false;
      }
    }
    return true;
  };

  // Reapply the original allocation to all Candidates
  auto RevertAllocation = [&](OriginalAllocation &Candidates) {
    // The partial allocation may conflict with the original one in ugly ways.
    // To be safe, reset all allocations first.
    for (auto &[MO, Org] : Candidates) {
      auto VReg = MO->getReg();
      if (VRM->hasPhys(VReg)) {
        unassignReg(VReg);
      }
    }
    for (auto &[MO, Org] : Candidates) {
      auto VReg = MO->getReg();
      assignReg(VReg, Org);
    }
  };

  // Least-Recently-Used list of physical registers for assignments to VRegs.
  // Physical registers that have recently been used are moved to the back.
  std::list<MCPhysReg> LRURegisters;

  // For each reg class, allocate the candidates in round-robin fashion.
  // If we fail, we fall back to the original allocation
  BitVector ExcludedPhysRegs{TRI->getNumRegs()};

  // Exclude CSRs
  for (const MCPhysReg *CSR = MRI->getCalleeSavedRegs(); CSR && *CSR; ++CSR)
    ExcludedPhysRegs[*CSR] = true;

  for (const auto *RC : RegClasses) {

    LLVM_DEBUG(dbgs() << "Allowed registers in RC=" << TRI->getRegClassName(RC)
                      << ":");
    for (MCPhysReg PhysReg : RC->getRegisters()) {
      if (!ExcludedPhysRegs[PhysReg]) {
        LLVM_DEBUG(dbgs() << " " << printReg(PhysReg, TRI));
        LRURegisters.push_back(PhysReg);
      }
      ExcludedPhysRegs[PhysReg] = true;
    }
    LLVM_DEBUG(dbgs() << "\n");
  }
  if (!ReAllocate(Candidates, LRURegisters)) {
    RevertAllocation(Candidates);
    return false;
  }

  return true;
}

bool AIEWawRegRewriter::isWorthRenaming(const Register &Reg,
                                        const BitVector &VRegWithCopies) const {
  assert(Reg.isVirtual());

  // The register might have been de-allocated when processing another loop.
  if (!VRM->hasPhys(Reg))
    return false;

  // Only consider vec/acc registers as candidates, and optionally GPRs.
  bool IsCandidateClass =
      TRI->isVecOrAccRegClass(*(MRI->getRegClass(Reg))) ||
      (GPRRealloc &&
       TRI->getGPRRegClass(*MF)->hasSubClassEq(MRI->getRegClass(Reg)));
  if (!IsCandidateClass)
    return false;

  return !VRegWithCopies[Reg.virtRegIndex()];
}

void AIEWawRegRewriter::unassignReg(Register VReg) {
  const LiveInterval &LI = LIS->getInterval(VReg);
  LRM->unassign(LI);
}

void AIEWawRegRewriter::assignReg(Register VReg, MCPhysReg PhysReg) {
  const LiveInterval &LI = LIS->getInterval(VReg);
  if (VRM->hasPhys(VReg)) {
    LRM->unassign(LI);
  }
  LRM->assign(LI, PhysReg);
}

bool AIEWawRegRewriter::replaceReg(const Register VReg,
                                   RoundRobin &LRURegisters,
                                   BitVector &UsedUnits) {
  assert(VReg.isVirtual());
  MCPhysReg ReplacementPhysReg =
      getReplacementPhysReg(VReg, LRURegisters, UsedUnits);

  if (ReplacementPhysReg == MCRegister::NoRegister)
    return false;

  LLVM_DEBUG(dbgs() << "     replace: " << printReg(VReg, TRI) << " with "
                    << TRI->getName(ReplacementPhysReg) << '\n');
  assert(Register::isPhysicalRegister(ReplacementPhysReg));

  assignReg(VReg, ReplacementPhysReg);
  return true;
}

/// Returns a vreg of the same class that is exclusively used (and killed)
/// at the point \p VReg gets defined.
std::optional<Register>
getKilledRegAtSingledDefPoint(Register VReg, const MachineRegisterInfo &MRI) {
  MachineOperand *MO = MRI.getOneDef(VReg);
  if (!MO)
    return std::nullopt;

  MachineInstr &DefMI = *MO->getParent();
  auto OnlyUsedByInstr = [&MRI](Register Reg, const MachineInstr &MI) {
    return all_of(MRI.use_instructions(Reg),
                  [&MI](const MachineInstr &UseMI) { return &UseMI == &MI; });
  };

  for (MachineOperand &UseMO : DefMI.explicit_uses()) {
    if (UseMO.isReg() && UseMO.getReg().isVirtual() &&
        MRI.getRegClass(VReg) == MRI.getRegClass(UseMO.getReg()) &&
        OnlyUsedByInstr(UseMO.getReg(), DefMI)) {
      return UseMO.getReg();
    }
  }
  return std::nullopt;
}

void moveRegAndAliasesBack(MCPhysReg PhysReg, RoundRobin &LRURegisters,
                           const TargetRegisterInfo *TRI) {
  for (MCRegAliasIterator AI(MCRegister(PhysReg), TRI, true); AI.isValid();
       ++AI) {
    // TODO: Use hints to speed up the search of aliases?
    auto AliasIt = llvm::find(LRURegisters, *AI);
    if (AliasIt != LRURegisters.end()) {
      LRURegisters.erase(AliasIt);
      LRURegisters.emplace_back(*AI);
    }
  }
}

MCPhysReg AIEWawRegRewriter::getReplacementPhysReg(const Register VReg,
                                                   RoundRobin &LRURegisters,
                                                   BitVector &UsedUnits) const {
  assert(VReg.isVirtual() && "Reg has to be a virtual register");

  /// Whether \p PhysReg was ever used for re-assigning a vreg
  auto WasUsedForReassignment = [TRI = this->TRI,
                                 &UsedUnits](MCPhysReg PhysReg) {
    return any_of(TRI->regunits(PhysReg),
                  [&UsedUnits](MCRegUnit RU) { return UsedUnits.test(RU); });
  };

  LLVM_DEBUG(dbgs() << "     Try to re-assign" << printReg(VReg, TRI) << "\n");
  const TargetRegisterClass *RC = MRI->getRegClass(VReg);
  const LiveInterval &LI = LIS->getInterval(VReg);

  // Find the least-recently assigned register to assign to VReg.
  for (auto It = LRURegisters.begin(); It != LRURegisters.end(); ++It) {
    MCPhysReg PhysReg = *It;

    if (!RC->contains(PhysReg)) {
      continue;
    }
    LiveRegMatrix::InterferenceKind IK = LRM->checkInterference(LI, PhysReg);
    if (IK == LiveRegMatrix::IK_Free) {
      // If the chosen physical register has already been used and the vreg to
      // allocate is defined at a point where another vreg gets killed, prefer
      // reusing the assignment of the killed reg.
      if (std::optional<Register> KilledReg =
              getKilledRegAtSingledDefPoint(VReg, *MRI);
          KilledReg && WasUsedForReassignment(PhysReg)) {
        MCRegister KilledPhysReg = getAssignedPhysReg(*KilledReg);
        if (KilledPhysReg && LRM->checkInterference(LI, KilledPhysReg) ==
                                 LiveRegMatrix::IK_Free) {

          LLVM_DEBUG(dbgs() << "     re-use killed physreg for assigning: "
                            << printReg(VReg, TRI) << " to "
                            << TRI->getName(KilledPhysReg) << '\n');
          PhysReg = KilledPhysReg;
          It = llvm::find(LRURegisters, KilledPhysReg);
          assert(It != LRURegisters.end());
        }
      }

      // Move it to the end of the list. We return, so don't have to
      // care about invalidation
      moveRegAndAliasesBack(PhysReg, LRURegisters, TRI);
      for (MCRegUnit RU : TRI->regunits(PhysReg))
        UsedUnits.set(RU);
      return PhysReg;
    }
    LLVM_DEBUG(dbgs() << "       Cannot assign " << printReg(VReg, TRI)
                      << " to " << TRI->getName(PhysReg)
                      << " due to interference\n");
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

IndexedMap<const MachineInstr *, VirtReg2IndexFunctor>
AIEWawRegRewriter::getLastVRegDef(const MachineBasicBlock &MBB) const {
  IndexedMap<const MachineInstr *, VirtReg2IndexFunctor> LastVRegDef;

  // Initialize the IndexedMap size
  LastVRegDef.grow(Register::index2VirtReg(MRI->getNumVirtRegs()));

  for (const MachineInstr &MI : llvm::reverse(MBB)) {
    for (const MachineOperand &Def : MI.defs()) {

      if (!Def.isReg() || !Def.getReg().isVirtual())
        continue;

      Register Reg = Def.getReg();
      LastVRegDef[Reg] = &MI;
    }
  }
  return LastVRegDef;
}

} // end anonymous namespace

char AIEWawRegRewriter::ID = 0;
char &llvm::AIEWawRegRewriterID = AIEWawRegRewriter::ID;

INITIALIZE_PASS(AIEWawRegRewriter, DEBUG_TYPE, "AIE waw-reg rewrite", false,
                false)

llvm::FunctionPass *llvm::createAIEWawRegRewriter() {
  return new AIEWawRegRewriter();
}
