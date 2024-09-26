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
#include "AIE2.h"
#include "AIE2InstrInfo.h"
#include "AIE2RegisterInfo.h"
#include "AIEBaseRegisterInfo.h"
#include "AIECombinerHelper.h"
#include "Utils/AIELoopUtils.h"
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
#include <optional>

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
    if (MI.getOpcode() == TargetOpcode::REG_SEQUENCE) {
      // The source of the INSERT_SUBREG chain is a REG_SEQUENCE. Collect all
      // its subregs.
      for (unsigned OpIdx = 1; OpIdx < MI.getNumOperands(); OpIdx += 2) {
        Subregs.try_emplace(MI.getOperand(OpIdx + 1).getImm(),
                            MI.getOperand(OpIdx).getReg());
      }
      return;
    }

    // Recursively follow INSERT_SUBREG chain
    assert(MI.getOpcode() == TargetOpcode::INSERT_SUBREG);
    Subregs.try_emplace(MI.getOperand(3).getImm(), MI.getOperand(2).getReg());
    MachineInstr &SrcMI = *MRI.getVRegDef(MI.getOperand(1).getReg());
    if (SrcMI.getParent() == MI.getParent() &&
        (SrcMI.getOpcode() == TargetOpcode::INSERT_SUBREG ||
         SrcMI.getOpcode() == TargetOpcode::REG_SEQUENCE))
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

/// This function takes one REG_SEQUENCE MI as the main parameter.
/// Then, the consumer of this instruction is analyzed to see if it
/// is a load (LoadUses), then its inputs will be mapped back to MI.
/// For example:
///   A = REG_SEQUENCE (x, y) <- analyzed MI
///   ??<data> ??<ptr> = LOAD (??, A)
/// Will result in:
///   LoadUses[x] = MI;
///   LoadUses[y] = MI;
/// Alternatively, if MI is used by a store, we simply add MI
/// parameters to the NonLoadUses set. We call this set as NonLoadUses
/// because it can also catch other situations, like PADDs for example.
/// Note that we use an asymmetric tracking because we plan to duplicate
/// registers for load uses, keeping stores with the original registers. Also,
/// note that an input register (used in REG_SEQUENCE) will  only be considered
/// for duplication (for load) if it is defined outside the current basic block.
/// This last rule is important to not create a worst scenario for the
/// coalescer.
static void collectIteratorComponentsUsage(
    MachineInstr &MI, const Register DstReg,
    DenseMap<Register, SmallPtrSet<MachineInstr *, 8>> &LoadUses,
    SmallSet<Register, 8> &NonLoadUses, MachineRegisterInfo &MRI) {

  // Take first-use instruction, that will be only one due to the
  // way we lower addressing register's use:
  // A  =  REG_SEQUENCE __
  // __ =  LOAD/STORE (__, A)
  MachineInstr *UseMI = MRI.use_begin(DstReg)->getParent();

  for (auto &Operand : MI.operands()) {
    // Ignore first operand (def.).
    if (!Operand.isReg() || Operand.getReg() == DstReg)
      continue;

    const Register Reg = Operand.getReg();
    MachineInstr *DefMI = MRI.getUniqueVRegDef(Reg);

    // Ignore internal definitions (same MBB).
    if (DefMI && DefMI->getParent() == MI.getParent())
      continue;

    if (UseMI->mayLoad())
      LoadUses[Reg].insert(&MI);
    else
      NonLoadUses.insert(Reg);
  }
}

/// This function does the real duplication. We can consider the following
/// example, where we can see how the code is changed:
///  bb.0.entry:
///  successors: %bb.1
///  liveins: $p0, $p1, $r0
///    ???
///    %2:em = MOV_PD_imm10_pseudo 10
///    + %new2:em PseudoMove %2
///    %3:edn = MOV_PD_imm10_pseudo 12
///    + %new3:edn PseudoMove %3
///    %4:edj = MOV_PD_imm10_pseudo 14
///    + %new4:edj PseudoMove %4
///    LoopStart ???
///    PseudoJ_jump_imm %bb.1
///  bb.1:
///  successors: %bb.1, ???
///    - %7:ed = REG_SEQUENCE %2, %subreg.sub_mod,
///    -         %3, %subreg.sub_dim_size, %4, %subreg.sub_dim_stride,
///    -         ???, %subreg.sub_dim_count
///    + %7:ed = REG_SEQUENCE %new2, %subreg.sub_mod,
///    +         %new3, %subreg.sub_dim_size, %new4, %subreg.sub_dim_stride,
///    +         ???, %subreg.sub_dim_count
///    ???, ???, ??? = VLDA_2D_dmw_lda_w ???, %7
///    %11:ed = REG_SEQUENCE %2, %subreg.sub_mod,
///             %3, %subreg.sub_dim_size, %4, %subreg.sub_dim_stride,
///             ???, %subreg.sub_dim_count
///    ???, ??? = VST_2D_dmw_sts_w ???, ???, %11
///    PseudoLoopEnd <mcsymbol .L_LEnd11>, %bb.1
///    PseudoJ_jump_imm ???
static bool tryToDuplicateLoadUse(
    DenseMap<Register, SmallPtrSet<MachineInstr *, 8>> &LoadUses,
    SmallSet<Register, 8> &NonLoadUses, MachineRegisterInfo &MRI,
    const AIEBaseInstrInfo *TII) {

  bool Changed = false;
  for (auto &RegMILoad : LoadUses) {
    const Register Reg = RegMILoad.first;
    if (!NonLoadUses.contains(Reg))
      continue;

    // Used by both loads and stores. We will duplicate for
    // load use.
    MachineInstr *DefReg = MRI.getUniqueVRegDef(Reg);

    MachineBasicBlock::iterator InsertPoint = DefReg->getIterator();
    Register DstReg = MRI.createVirtualRegister(MRI.getRegClass(Reg));
    // Here we cannot use COPY, because Machine CSE will run
    // PerformTrivialCopyPropagation and our work will disappear.
    // smoothly.
    BuildMI(*DefReg->getParent(), ++InsertPoint, DefReg->getDebugLoc(),
            TII->get(TII->getPseudoMoveOpcode()), DstReg)
        .addReg(Reg);

    for (MachineInstr *UseMI : RegMILoad.second) {
      for (auto &Operand : UseMI->operands()) {
        if (Operand.isReg() && Operand.getReg() == Reg)
          Operand.setReg(DstReg);
      }
    }
    Changed = true;
  }
  return Changed;
}

/// This optimization prevents the coalescing and copy propagation of
/// Addressing registers in loops with shared descriptors for load and stores.
//  This is important to avoid unnecessary COPYs of such descriptors when
//  they are coalesced.
bool duplicateAdressingRegs(MachineBasicBlock &MBB, MachineRegisterInfo &MRI) {

  const TargetSubtargetInfo &ST = MBB.getParent()->getSubtarget();
  const AIEBaseInstrInfo *TII =
      static_cast<const AIEBaseInstrInfo *>(ST.getInstrInfo());
  const AIEBaseRegisterInfo *TRI =
      static_cast<const AIEBaseRegisterInfo *>(ST.getRegisterInfo());

  assert(TII->getPseudoMoveOpcode() &&
         "Target must have a PseudoMove instruction");

  // We consider only loops here.
  if (!AIELoopUtils::isSingleMBBLoop(&MBB))
    return false;

  DenseMap<Register, SmallPtrSet<MachineInstr *, 8>> LoadUses;
  SmallSet<Register, 8> NonLoadUses;
  const TargetRegisterClass *Iter2DRC = TRI->get2DIteratorRegClass();
  const TargetRegisterClass *Iter3DRC = TRI->get3DIteratorRegClass();

  // The first part, collect interesting cases.
  for (MachineInstr &MI : MBB) {
    if (MI.getOpcode() != TargetOpcode::REG_SEQUENCE)
      continue;

    const Register DstReg = MI.getOperand(0).getReg();
    const TargetRegisterClass *RC = MRI.getRegClass(DstReg);

    if (RC != Iter2DRC && RC != Iter3DRC)
      continue;

    collectIteratorComponentsUsage(MI, DstReg, LoadUses, NonLoadUses, MRI);
  }

  // The second part, filter the real useful cases,
  // registers used in both load and stores (or non load uses).
  // Then duplicate those registers.
  return tryToDuplicateLoadUse(LoadUses, NonLoadUses, MRI, TII);
}

using Operation = AIEBaseInstrInfo::AbstractOp::OperationType;
using AbstractOp = AIEBaseInstrInfo::AbstractOp;
using OptionalOp = std::optional<AbstractOp>;

bool getGlobalValue(const MachineInstr *MI,
                    SmallPtrSet<const Value *, 4> &GVSet,
                    MachineRegisterInfo &MRI) {

  MI = getDefIgnoringCopiesAndBitcasts(MI->getOperand(1).getReg(), MRI);

  // We need an instruction that explicitly moves a global
  // to a register (move immediate).
  if (MI->getNumOperands() != 2)
    return false;

  const MachineOperand *MO = MI->uses().begin();

  if (MO->isGlobal()) {
    GVSet.insert(MO->getGlobal());
    return true;
  }

  return false;
}

// Recognize BROADCAST (GLOBAL)
bool visitVBroadcast(const AbstractOp &VBroadcast, const AIEBaseInstrInfo *TII,
                     MachineRegisterInfo &MRI,
                     SmallPtrSet<const Value *, 4> &GVSet) {
  assert(VBroadcast.Type == Operation::VECTOR_BROADCAST &&
         "Wrong abstract operation.");
  return getGlobalValue(MRI.getVRegDef(VBroadcast.ScalarSrcRegs[0]), GVSet,
                        MRI);
}

// Recognize SELECT ((SELECT | BROADCAST), ((SELECT | BROADCAST)))
bool visitVSelect(const AbstractOp &VSelect, const AIEBaseInstrInfo *TII,
                  MachineRegisterInfo &MRI,
                  SmallPtrSet<const Value *, 4> &GVSet) {

  assert(VSelect.Type == Operation::VECTOR_SELECT &&
         "Wrong abstract operation.");

  bool Success = true;
  for (const Register Reg : VSelect.VectorSrcRegs) {
    OptionalOp VOp = TII->parseAbstractOp(*MRI.getVRegDef(Reg));
    // We don't recognize this! Here we cannot have anything different
    // from SELECT or BROADCAST.
    if (!VOp)
      return false;
    if (VOp->Type == Operation::VECTOR_SELECT)
      Success &= visitVSelect(*VOp, TII, MRI, GVSet);
    else if (VOp->Type == Operation::VECTOR_BROADCAST)
      Success &= visitVBroadcast(*VOp, TII, MRI, GVSet);
    else
      return false;
  }

  return Success;
}

// Recognize ADD ((ADD | SELECT), Unknown)
bool visitAddressVector(const AbstractOp &VOp, const AIEBaseInstrInfo *TII,
                        MachineRegisterInfo &MRI,
                        SmallPtrSet<const Value *, 4> &GVSet) {

  if (VOp.Type == Operation::VECTOR_SELECT)
    return visitVSelect(VOp, TII, MRI, GVSet);
  if (VOp.Type != Operation::VECTOR_ADD)
    return false;

  return any_of(VOp.VectorSrcRegs, [&](Register SrcReg) {
    OptionalOp VOp = TII->parseAbstractOp(*MRI.getVRegDef(SrcReg));
    return VOp && visitAddressVector(*VOp, TII, MRI, GVSet);
  });
}

// Follow the chain up starting for ADD or directly from SELECT.
bool traverseVecPointerChain(Register Reg, SmallPtrSet<const Value *, 4> &GVSet,
                             const AIEBaseInstrInfo *TII,
                             MachineRegisterInfo &MRI) {
  const MachineInstr *MI = MRI.getVRegDef(Reg);
  // Skip subvector copy.
  if (MI->isCopy() && MI->getOperand(1).getReg().isVirtual())
    MI = MRI.getVRegDef(MI->getOperand(1).getReg());

  OptionalOp VOp = TII->parseAbstractOp(*MI);
  if (!VOp)
    return false;

  return visitAddressVector(*VOp, TII, MRI, GVSet);
}

/// This optimization tries to propagate GlobalValues as MMO of load operations
/// when they are missing.
bool fixLoadMemOpInfo(MachineFunction &MF, MachineBasicBlock &MBB,
                      MachineRegisterInfo &MRI) {

  const TargetSubtargetInfo &ST = MBB.getParent()->getSubtarget();
  const AIEBaseInstrInfo *TII =
      static_cast<const AIEBaseInstrInfo *>(ST.getInstrInfo());

  bool Changed = false;

  for (MachineInstr &MI : MBB) {
    // Fast skip non-load operations.
    if (!MI.mayLoad())
      continue;

    OptionalOp VOp = TII->parseAbstractOp(MI);
    if (!VOp)
      continue;

    if (VOp->Type != Operation::VECTOR_XWAY_LOAD)
      continue;

    SmallPtrSet<const Value *, 4> GVSet;
    if (traverseVecPointerChain(VOp->VectorSrcRegs[0], GVSet, TII, MRI)) {
      // Add gathered GlobalValues as MMOs.
      for (auto GV : GVSet) {
        // As we only know the base pointer and nothing about the
        // result of the address calculation, we simply don't assume
        // size/alignment/offset, so we prevent AA from inferring wrong
        // information.
        MachineMemOperand *PtrLoadMMO = MF.getMachineMemOperand(
            MachinePointerInfo(GV), MachineMemOperand::MOLoad, LLT(), Align());
        MI.addMemOperand(MF, PtrLoadMMO);
        Changed = true;
      }
    }
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

  // 3. Duplicate addressing registers.
  // As we use getPseudoMoveOpcode from TII, that is only available
  // in AIE2.
  if (!MF.getTarget().getTargetTriple().isAIE1()) {
    for (MachineBasicBlock &MBB : MF) {
      Changed |= duplicateAdressingRegs(MBB, MF.getRegInfo());
    }
  }

  // 4. Fix MMO for load instructions that don't use pointers
  // registers (use vector instead, for example).
  for (MachineBasicBlock &MBB : MF) {
    Changed |= fixLoadMemOpInfo(MF, MBB, MF.getRegInfo());
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
