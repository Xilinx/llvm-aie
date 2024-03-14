//=== AIEPostSelectOptimize.cpp ----------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
/// \file
/// This pass does post-instruction-selection optimizations in the GlobalISel
/// pipeline, before the rest of codegen runs.
//
// More specifically, it erases redundant copies
// We have 2 cases of such copies.
// 1. Identity copies (assignments to self).
// See Test 1 : "crfpmask_multiple" func in
// CodeGen/AIE/aie2/GlobalISel/post-select-optimize-erase-physregs.mir
// E.g. Transform -
// %0 = COPY $crsat               |   |    |   %0 = COPY $crsat
// $crsat = COPY %0               |   |    |   ---------------------------
// %1 = opcode implicit $crsat    |   |    |   %1 = opcode implicit $crsat
// $crsat = COPY %0               |   |    |   ---------------------------
// %2 = opcode implicit $crsat    |   |    |   %2 = opcode implicit $crsat
// %10 = COPY $crsat              |-> to ->|   %10 = COPY $crsat
// $crsat = COPY %10              |   |    |   ---------------------------
// %3 = opcode implicit $crsat    |   |    |   %3 = opcode implicit $crsat
// $crsat = COPY %0               |   |    |   ---------------------------
// %4 = opcode implicit $crsat    |   |    |   %4 = opcode implicit $crsat
//
// 2. Deadcode elimination.
// See Test 5: "crupssign_redundant_reassignments" func in
// CodeGen/AIE/aie2/GlobalISel/post-select-optimize-erase-physregs.mir
// E.g. Transform -
// %0 = COPY $r0                    |   |    | %0 = COPY $r0
// %1 = COPY $r1                    |   |    | %1 = COPY $r1
// $crupssign = COPY %0             |   |    | $crupssign = COPY %0
// %2 = opcode implicit $crupssign  |   |    | %2 = opcode implicit $crupssign
// $crupssign = MOV 0               |-> to ->| -------------------------------
// $crupssign = COPY %0             |   |    | -------------------------------
// %3 = opcode implicit $crupssign  |   |    | %3 = opcode implicit $crupssign
// $crupssign = MOV 0               |   |    | -------------------------------
// $crupssign = COPY %1             |   |    | $crupssign = COPY %1
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/CodeGen/GlobalISel/Utils.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "aie-post-select-optimize"

static cl::opt<bool>
    EnablePostSelectOptimize("aie-post-select-opt", cl::Hidden, cl::init(true),
                             cl::desc("Enable post select optimize."));

namespace {
class AIEPostSelectOptimize : public MachineFunctionPass {
public:
  static char ID;
  AIEPostSelectOptimize();
  StringRef getPassName() const override { return "AIE Post Select Optimizer"; }
  bool runOnMachineFunction(MachineFunction &MF) override;
  void getAnalysisUsage(AnalysisUsage &AU) const override;

private:
  const MachineRegisterInfo *MRI;

  /**
   * @struct PhysRegDefInfoStruct
   * @brief To track the PhysReg defs and the state of their use.
   */
  struct PhysRegDefInfoStruct {
    // Tracks the last absolutely required def.
    MachineInstr *LastRequiredDef = nullptr;
    // Tracks the last visited def. Change with every new def.
    MachineInstr *LastVisitedDef = nullptr;
    // Tracks if the PhysReg is used in between current def and LastVisitedDef.
    bool IsUsed = false;

    PhysRegDefInfoStruct(MachineInstr *MI)
        : LastRequiredDef(MI), LastVisitedDef(MI), IsUsed(false) {}
  };

  /// Collect all VirtReg to PhysReg copies that can be removed at the last.
  DenseMap<Register, MachineInstr *> ErasableVirtRegToPhysRegCopies;

  bool doPeepholeOpts(MachineBasicBlock &MBB);
  bool eraseOrStoreRedundantPhysRegDefs(
      MachineInstr &MI, DenseMap<Register, PhysRegDefInfoStruct> &PhysRegDefs);

  /// Checks if the \p Op is an unallocatable phys reg.
  bool isUnallocatablePhysReg(const MachineOperand &Op);

  /// Checks if the src %virtreg of \p MI defines the same \p
  /// PhysReg as the destination register of the \p MI.
  bool isCopyFromPhysReg(const MachineInstr &MI, const Register PhysReg);

  /// Store the first %virtreg to $physreg copy corresponding to \param MI.
  void storeFirstIdentityCopy(MachineInstr &MI);

  /// Erase \param MI if it is similar to \param LastVisitedDef.
  bool eraseSimilarClobber(MachineInstr &MI, MachineInstr &LastVisitedDef);
};
} // end anonymous namespace

bool AIEPostSelectOptimize::doPeepholeOpts(MachineBasicBlock &MBB) {
  bool Changed = false;
  DenseMap<Register, PhysRegDefInfoStruct> PhysRegDefs;
  for (MachineInstr &MI : make_early_inc_range(MBB)) {
    Changed |= eraseOrStoreRedundantPhysRegDefs(MI, PhysRegDefs);
  }
  return Changed;
}

/// Check if the virtual register being copied to the phys reg was copied from
/// from the same phys reg.
bool AIEPostSelectOptimize::isCopyFromPhysReg(const MachineInstr &MI,
                                              const Register PhysReg) {
  if (!MI.isCopy())
    return false;
  Register SrcReg = MI.getOperand(1).getReg();
  if (SrcReg.isPhysical())
    return SrcReg == PhysReg;
  MachineInstr *VRegDef = MRI->getVRegDef(SrcReg);
  if (!VRegDef)
    return false;
  return isCopyFromPhysReg(*VRegDef, PhysReg);
}

/// Stores the first identity copy to act as anchor for all the other copies and
/// remove later if applicable.
void AIEPostSelectOptimize::storeFirstIdentityCopy(MachineInstr &MI) {
  assert(MI.getNumOperands() == 2 && "Expected only 2 operands in the MI");

  if (!MI.getOperand(1).isReg())
    return;

  Register DstPhysReg = MI.getOperand(0).getReg();
  Register SrcReg = MI.getOperand(1).getReg();
  // We currently have $PhysReg = COPY SrcReg. Check if the SrcReg is virtual
  // and find the def of the SrcReg to check for %virtreg = COPY $PhysReg.
  if (ErasableVirtRegToPhysRegCopies.count(SrcReg) ||
      !isCopyFromPhysReg(MI, DstPhysReg)) {
    return;
  }

  // If the copy is found and copy propagation will lead to identity copy,
  // then just store the current $PhysReg = COPY %virtreg MI to be removed
  // later.
  ErasableVirtRegToPhysRegCopies[SrcReg] = &MI;
}

/// If there is an intervening clobber but copy propagation suggests it
/// comes from the same PhysReg, it can be removed.
bool AIEPostSelectOptimize::eraseSimilarClobber(MachineInstr &MI,
                                                MachineInstr &LastVisitedDef) {
  if (!isCopyFromPhysReg(LastVisitedDef, MI.getOperand(0).getReg()) ||
      !isCopyFromPhysReg(MI, LastVisitedDef.getOperand(0).getReg()))
    return false;

  Register MISrcReg = MI.getOperand(1).getReg();
  if (ErasableVirtRegToPhysRegCopies.count(MISrcReg) &&
      ErasableVirtRegToPhysRegCopies[MISrcReg] == &MI) {
    ErasableVirtRegToPhysRegCopies.erase(MISrcReg);
  }
  LLVM_DEBUG(dbgs() << "Erasing redundant MI - " << MI);
  MI.eraseFromParent();
  return true;
}

/// Returns true if the register is an Unallocatable physical register.
/// E.g. Control Registers.
inline bool
AIEPostSelectOptimize::isUnallocatablePhysReg(const MachineOperand &Op) {
  return Op.isReg() && Op.getReg().isPhysical() && Op.isDef() &&
         !MRI->isAllocatable(Op.getReg());
}

/// Erase or store possible redundant copies local to a basic block.
bool AIEPostSelectOptimize::eraseOrStoreRedundantPhysRegDefs(
    MachineInstr &MI, DenseMap<Register, PhysRegDefInfoStruct> &PhysRegDefs) {
  // Update the IsUsed boolean of PhysRegDefs to track for any uses
  // of the Unallocatable Phys Reg.
  for (auto &[UnallocatablePhysReg, DefInfo] : PhysRegDefs) {
    if (MI.readsRegister(UnallocatablePhysReg) ||
        MI.hasRegisterImplicitUseOperand(UnallocatablePhysReg)) {
      DefInfo.IsUsed = true;
    }
  }

  // Return early if we are not looking at phys reg defines.
  if (MI.getNumOperands() < 2 || !isUnallocatablePhysReg(MI.getOperand(0)))
    return false;

  assert(MI.getNumOperands() == 2 &&
         "Expected only 2 operands in the physreg def!");

  const Register DstPhysReg = MI.getOperand(0).getReg();

  auto PhysRegDefsItr = PhysRegDefs.find(DstPhysReg);
  const bool PhysRegNotFound = PhysRegDefsItr == PhysRegDefs.end();
  if (PhysRegNotFound) {
    // Add a new phys reg defining MI.
    PhysRegDefs.insert(std::pair(DstPhysReg, PhysRegDefInfoStruct(&MI)));
  }

  // Store the first identity copy to be erased later.
  // This is not removed now itself because we need this copy to be treated as
  // anchor to help us find other copies.
  storeFirstIdentityCopy(MI);

  if (PhysRegNotFound)
    return false;

  PhysRegDefInfoStruct &DefInfo = PhysRegDefsItr->second;

  /*CASE 1*/
  // If the MI in the map corresponding to this UnallocatablePhysReg is
  // identical to current MI, erase the current MI, since it is redundant.
  // %0 = COPY $crsat
  // $crsat = COPY %0
  // %1 = opcode implicit $crsat
  // $crsat = COPY %0 <------------ redundant
  // %2 = opcode implicit $crsat
  if (DefInfo.LastVisitedDef->isIdenticalTo(MI)) {
    if (!MI.getOperand(1).isReg() || MI.getOperand(1).getReg().isVirtual()) {
      LLVM_DEBUG(dbgs() << "Erasing redundant MI - " << MI);
      MI.eraseFromParent();
      return true;
    }
  }

  /*CASE 2*/
  // If there is an intervening clobber but copy propagation suggests it
  // comes from the same PhysReg, it can be removed.
  // %0 = COPY $crsat
  // $crsat = COPY %0
  // %1 = opcode implicit $crsat
  // %2 = COPY $crsat
  // $crsat = COPY %2 <------------ redundant
  // %3 = opcode implicit $crsat
  if (eraseSimilarClobber(MI, *DefInfo.LastVisitedDef))
    return true;

  bool Changed = false;
  MachineInstr *CurrentDef = &MI;
  if (!DefInfo.IsUsed) {
    /*CASE 3*/
    // If MI in the map is not similar to current MI, but there is
    // redundancy and no instruction reads that physical register between
    // definitions, erase the MI in the map, since we only need the current MI.
    // E.g.: a fancy case of dead code elimination.
    // %0 = COPY $r0
    // $crupssign = COPY %0 <---------------- LastRequiredDef
    // %1 = opcode implicit $crupssign
    // $crupssign = MOV 0 <------------------ LastVisitedDef : dead
    // $crupssign = COPY %0 <----------------- Current Def
    // %2 = opcode implicit $crupssign
    LLVM_DEBUG(dbgs() << "Erasing redundant MI - " << *DefInfo.LastVisitedDef);
    DefInfo.LastVisitedDef->eraseFromParent();
    Changed = true;

    /*CASE 4*/
    // We first removed the deadcode in between and then check if there are
    // other identical copies.
    // Continuing the above example -
    // %0 = COPY $r0
    // $crupssign = COPY %0 <----------------- LastRequiredDef
    // %1 = opcode implicit $crupssign
    // $crupssign = COPY %0 <----------------- Current Def : also redundant
    // %2 = opcode implicit $crupssign
    if (DefInfo.LastRequiredDef->isIdenticalTo(MI)) {
      LLVM_DEBUG(dbgs() << "Erasing redundant MI - " << MI);
      MI.eraseFromParent();

      // Assign the LastRequiredDef as the CurrentDef, which is eventually
      // assigned to LastVisitedDef since both the LastVisitedDef and the
      // current MI have been erased.
      // This basically serves as another "anchor" to which we can compare
      // are copies with.
      CurrentDef = DefInfo.LastRequiredDef;
    }
  } else {
    // Assign LastVisitedDef to LastRequiredDef since there is a use in between
    // and we cannot remove it.
    DefInfo.LastRequiredDef = DefInfo.LastVisitedDef;
  }
  // Change the LastVisitedDef to CurrentDef to increment our "anchor".
  DefInfo.LastVisitedDef = CurrentDef;
  DefInfo.IsUsed = false;
  return Changed;
}

void AIEPostSelectOptimize::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<TargetPassConfig>();
  AU.addRequired<MachineModuleInfoWrapperPass>();
  AU.setPreservesCFG();
}

AIEPostSelectOptimize::AIEPostSelectOptimize() : MachineFunctionPass(ID) {
  initializeAIEPostSelectOptimizePass(*PassRegistry::getPassRegistry());
}

bool AIEPostSelectOptimize::runOnMachineFunction(MachineFunction &MF) {
  LLVM_DEBUG(dbgs() << "\n******* POST I-SEL OPTIMIZATION PASS *******\n"
                    << "********** Function: " << MF.getName() << '\n');
  if (MF.getProperties().hasProperty(
          MachineFunctionProperties::Property::FailedISel))
    return false;
  assert(MF.getProperties().hasProperty(
             MachineFunctionProperties::Property::Selected) &&
         "Expected a selected MF");

  if (!EnablePostSelectOptimize)
    return false;

  bool Changed = false;
  MRI = &MF.getRegInfo();

  ErasableVirtRegToPhysRegCopies.shrink_and_clear();

  for (MachineBasicBlock &MBB : MF) {
    Changed |= doPeepholeOpts(MBB);
  }

  // Remove all the collected VirtReg to UnallocatablePhysReg copies that would
  // otherwise be inferred as identity copies.
  for (auto &[_, CopyMI] : ErasableVirtRegToPhysRegCopies) {
    LLVM_DEBUG(dbgs() << "Erasing redundant MI - " << *CopyMI);
    CopyMI->eraseFromParent();
    Changed = true;
  }

  return Changed;
}

char AIEPostSelectOptimize::ID = 0;
INITIALIZE_PASS_BEGIN(AIEPostSelectOptimize, DEBUG_TYPE,
                      "Optimize AIE selected instructions", false, false)
INITIALIZE_PASS_END(AIEPostSelectOptimize, DEBUG_TYPE,
                    "Optimize AIE selected instructions", false, false)

namespace llvm {
FunctionPass *createAIEPostSelectOptimize() {
  return new AIEPostSelectOptimize();
}
} // end namespace llvm
