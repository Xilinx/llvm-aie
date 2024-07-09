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

#include "AIE.h"
#include "AIEBaseRegisterInfo.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/CodeGen/GlobalISel/Utils.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "aie-post-select-optimize"

static cl::opt<bool>
    EnablePostSelectOptimize("aie-post-select-opt", cl::Hidden, cl::init(true),
                             cl::desc("Enable post select optimize."));

namespace {

/// Information about a COPY that can be tracked by PhysRegCopyTracker.
struct TrackableCopyOperands {
  Register VirtReg;
  MCRegister PhysReg;
  bool IsPhysRegDef; // Whether PhysReg is being redefined.
};

/// Track SSA copies of simple physical registers.
/// Simple means they don't have sub or super registers.
class PhysRegCopyTracker {
  DenseMap<MCRegister, SmallSet<Register, 4>> Copies;
  const TargetRegisterInfo &TRI;

public:
  PhysRegCopyTracker(const MachineRegisterInfo &MRI)
      : TRI(*MRI.getTargetRegisterInfo()) {
    assert(MRI.isSSA() && "PhysRegCopyTracker can only track SSA copies");
  }

  /// Invalidate the copies of \p Reg.
  void invalidateCopies(MCRegister Reg) { Copies.erase(Reg); }

  /// Invalidate copies for any reg defined by \p MI
  void invalidateDefCopies(const MachineInstr &MI) {
    for (const MachineOperand &MO : MI.all_defs()) {
      if (MO.getReg().isPhysical()) {
        invalidateCopies(MO.getReg());
      }
    }
  }

  /// Track that \p VRegCopy is a copy of \p PhysReg
  void trackCopy(MCRegister PhysReg, Register VRegCopy) {
    assert(VRegCopy.isVirtual());
    assert(range_size(TRI.regunits(PhysReg)) == 1 && "Phys reg has aliases.");
    Copies[PhysReg].insert(VRegCopy);
  }

  /// Return true if \p VReg is a copy of \p PhysReg
  bool isCopy(MCRegister PhysReg, Register VReg) const {
    if (auto It = Copies.find(PhysReg); It != Copies.end())
      return It->second.contains(VReg);
    return false;
  }

  /// If it exists, return a virtual register that holds a copy of \p PhysReg.
  std::optional<Register> getVirtualCopy(MCRegister PhysReg) const {
    if (auto It = Copies.find(PhysReg);
        It != Copies.end() && !It->second.empty())
      return *It->second.begin();
    return {};
  }
};

class AIEPostSelectOptimize : public MachineFunctionPass {
public:
  static char ID;
  AIEPostSelectOptimize();
  StringRef getPassName() const override { return "AIE Post Select Optimizer"; }
  bool runOnMachineFunction(MachineFunction &MF) override;
  void getAnalysisUsage(AnalysisUsage &AU) const override;
};
} // end anonymous namespace

/// Returns whether the operands of \p MI can be tracked by PhysRegCopyTracker
std::optional<TrackableCopyOperands>
getTrackableCopyOperands(const MachineInstr &MI,
                         const TargetRegisterInfo &TRI) {
  if (!MI.isCopy())
    return {};

  // We can track the operands of a copy if one is virtual and the other is a
  // safe-to-simplify physical register. This is dictated by PhysRegCopyTracker.
  auto CanTrackCopyOps = [&](Register Op1, Register Op2) {
    return Op1.isVirtual() && Op2.isPhysical() &&
           TRI.isSimplifiableReservedReg(Op2);
  };

  auto [DstReg, SrcReg] = MI.getFirst2Regs();
  if (CanTrackCopyOps(DstReg, SrcReg)) {
    return TrackableCopyOperands{DstReg, SrcReg, /*InvalidatePhysReg=*/false};
  }
  if (CanTrackCopyOps(SrcReg, DstReg)) {
    // Here the phys reg is redefined. Notify PhysRegCopyTracker to invalidate
    // its tracked copies.
    return TrackableCopyOperands{SrcReg, DstReg, /*InvalidatePhysReg=*/true};
  }
  return {};
}

/// Returns true if \p MI is a COPY that was eliminated.
bool trySimplifyCopy(MachineInstr &MI, const PhysRegCopyTracker &CT) {
  if (!MI.isCopy())
    return false;
  auto [DstReg, SrcReg] = MI.getFirst2Regs();
  if (DstReg.isPhysical() && CT.isCopy(DstReg, SrcReg)) {
    // Assigning to DstReg a copy of itself. Just remove MI.
    MI.eraseFromParent();
    return true;
  }
  if (std::optional<Register> EquivVirtSrc;
      SrcReg.isPhysical() && (EquivVirtSrc = CT.getVirtualCopy(SrcReg))) {
    // Avoid defining a new VReg if another available one has the same value.
    MI.getMF()->getRegInfo().replaceRegWith(DstReg, *EquivVirtSrc);
    MI.eraseFromParent();
    return true;
  }
  return false;
}

/// Remove copies to phys regs that are redundant.
bool removeRedundantCopies(MachineBasicBlock &MBB,
                           const MachineRegisterInfo &MRI) {
  const TargetRegisterInfo &TRI = *MRI.getTargetRegisterInfo();
  bool Changed = false;
  PhysRegCopyTracker CT(MRI);

  /// Go through all instructions to collect copies and simplify redundant ones.
  for (MachineInstr &MI : make_early_inc_range(MBB)) {
    if (trySimplifyCopy(MI, CT)) {
      Changed = true;
    } else if (auto Ops = getTrackableCopyOperands(MI, TRI)) {
      if (Ops->IsPhysRegDef) {
        // PhysReg is redefined: Track the copy but invalidate previous ones.
        CT.invalidateCopies(Ops->PhysReg);
      }
      CT.trackCopy(Ops->PhysReg, Ops->VirtReg);
    } else {
      // For any other instruction, be conservative and invalidate copies for
      // the phys regs that MI defines.
      CT.invalidateDefCopies(MI);
    }
  }

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

/// Collect all the subregs used by a chain of INSERT_SUBREG instructions.
std::map<int, Register> collectSubregsChain(const MachineInstr &MI) {
  const MachineRegisterInfo &MRI = MI.getMF()->getRegInfo();
  std::map<int, Register> Subregs;

  // Recursively traverse INSERT_SUBREG chains in a same MBB.
  // E.g. %5:vec512 = INSERT_SUBREG %0, %1, %subreg.sub_256_hi
  //      %6:vec512 = INSERT_SUBREG %5, %2, %subreg.sub_256_lo
  std::function<void(const MachineInstr &)> Impl = [&](const MachineInstr &MI) {
    assert(MI.getOpcode() == TargetOpcode::INSERT_SUBREG);
    Subregs.try_emplace(MI.getOperand(3).getImm(), MI.getOperand(2).getReg());
    MachineInstr &SrcMI = *MRI.getVRegDef(MI.getOperand(1).getReg());
    if (SrcMI.getParent() == MI.getParent() &&
        SrcMI.getOpcode() == TargetOpcode::INSERT_SUBREG)
      Impl(SrcMI);
  };

  Impl(MI);
  return Subregs;
}

SmallSet<int, 8> getMapKeys(const std::map<int, Register> &Map) {
  SmallSet<int, 8> Keys;
  for (const auto &[Key, Value] : Map)
    Keys.insert(Key);
  return Keys;
}

/// Look for INSERT_SUBREG that can be rewritten as REG_SEQUENCE
bool combineINSERT_SUBREG(MachineBasicBlock &MBB) {
  const MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
  const TargetInstrInfo &TII = *MBB.getParent()->getSubtarget().getInstrInfo();
  auto &TRI =
      *static_cast<const AIEBaseRegisterInfo *>(MRI.getTargetRegisterInfo());
  bool Changed = false;

  for (MachineInstr &MI : make_early_inc_range(reverse(MBB))) {
    if (MI.getOpcode() != TargetOpcode::INSERT_SUBREG)
      continue;
    std::map<int, Register> Subregs = collectSubregsChain(MI);
    Register DstReg = MI.getOperand(0).getReg();
    const TargetRegisterClass *DstRC = MRI.getRegClass(DstReg);

    // Skip MI if it isn't a chain of INSERT_SUBREG that can be rewritten as
    // a REG_SEQUENCE instruction.
    if (Subregs.empty() ||
        getMapKeys(Subregs) != TRI.getCoveringSubRegs(*DstRC))
      continue;

    MachineInstrBuilder MIB = BuildMI(
        MBB, MI, MI.getDebugLoc(), TII.get(TargetOpcode::REG_SEQUENCE), DstReg);
    for (const auto &[SubregIdx, Reg] : Subregs) {
      MIB.addReg(Reg).addImm(SubregIdx);
    }
    MI.eraseFromParent();
    Changed = true;
  }

  return Changed;
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

  // 1. Turn INSERT_SUBREG into REG_SEQUENCE when possible
  for (MachineBasicBlock &MBB : MF) {
    Changed |= combineINSERT_SUBREG(MBB);
  }

  // 2. Simplify reserved register assignments
  for (MachineBasicBlock &MBB : MF) {
    Changed |= removeRedundantCopies(MBB, MF.getRegInfo());
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
