//===--- AIEGlobalCombinerPtrMods.cpp - Global Pointer Modifier combiner --===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Define Pre-Increment (Offset) and Post-Increment Combiners for the global
// combiner search.
//
//===----------------------------------------------------------------------===//

#include "AIEGlobalCombinerPtrMods.h"
#include "llvm/CodeGen/GlobalISel/GenericMachineInstrs.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/GlobalISel/Utils.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"

namespace llvm::AIE {
#define DEBUG_TYPE "global-combiner"

static cl::opt<bool> PenalizeExtraCursorPostInc(
    "aie-postinc-extra-cursor-penalty", cl::Hidden, cl::init(true),
    cl::desc("Discount the PtrMod gain of a post-increment that would keep a "
             "separately-advanced loop base live across a pointer chain."));

const static int PtrModBits = 20;
namespace {
unsigned getLoadStoreSize(const MachineInstr &MI) {
  // We are guaranteed to have MMOs during Instruction Selection.
  // We need them to select the correct instruction when they depend on the
  // size in memory and not on the register size. E.g.: part word stores.
  return (*MI.memoperands_begin())->getSizeInBits().getValue();
}

std::optional<APInt> getImm(const MachineInstr &PtrAdd,
                            const MachineRegisterInfo &MRI) {
  assert(PtrAdd.getOpcode() == TargetOpcode::G_PTR_ADD);
  auto OffsetMO = PtrAdd.getOperand(2);
  std::optional<ValueAndVReg> Offset =
      getIConstantVRegValWithLookThrough(OffsetMO.getReg(), MRI);
  if (!Offset)
    return {};

  return APInt(PtrModBits, Offset->Value.getSExtValue(), /*isSigned=*/
               true);
}

/// Walk up a chain of constant-offset G_PTR_ADDs starting at \p Ptr.
/// \return the register the chain is rooted at and the accumulated
/// displacement from that root.
std::pair<Register, int64_t> getChainRoot(Register Ptr,
                                          const MachineRegisterInfo &MRI) {
  int64_t Offset = 0;
  while (Ptr.isVirtual()) {
    const MachineInstr *Def = MRI.getVRegDef(Ptr);
    if (!Def || Def->getOpcode() != TargetOpcode::G_PTR_ADD)
      break;
    std::optional<APInt> Imm = getImm(*Def, MRI);
    if (!Imm)
      break;
    Offset += Imm->getSExtValue();
    Ptr = Def->getOperand(1).getReg();
  }
  return {Ptr, Offset};
}

} // namespace

const std::vector<Register> PointerModifierCombiner::getClusterRegs() const {
  auto *PtrMod = getPtrInc();
  auto InputPtrIdx = TII->getInputPtrIdx(*PtrMod, *MRI);
  assert(InputPtrIdx);
  auto InputPtrReg = PtrMod->getOperand(*InputPtrIdx).getReg();

  const auto OpIdx = TII->getOutputPtrIdx(*PtrMod, *MRI);
  assert(OpIdx);
  auto OuputPtrReg = PtrMod->getOperand(*OpIdx).getReg();
  return {InputPtrReg, OuputPtrReg};
}

std::vector<MachineInstr *> PointerModifierCombiner::getCombineCandidates(
    MachineInstr *MemI, const AIE::DataDependenceHelper &DAG) const {
  const unsigned VecSize =
      MRI->getType(MemI->getOperand(0).getReg()).getSizeInBits();
  if (VecSize > TII->getMaxSupportedLdStIncSize())
    return {};

  return getPtrInstrs(MemI);
}

UsageCount
PointerModifierCombiner::getUsageCount(Register Addr,
                                       const MachineDominatorTree &MDT) const {
  int UseDefsPastUseInstr = 0;
  int PtrModCount = 0;
  bool PHIUsageInSameMBB = false;

  const MachineInstr *PtrInc = getPtrInc();
  const auto *MBB = PtrInc->getParent();

  for (auto &User : MRI->use_nodbg_instructions(Addr)) {
    if (User.isPHI() && MBB == User.getParent()) {
      PHIUsageInSameMBB = true;
      continue;
    }

    if (contains(User))
      continue;

    const auto *UserMBB = User.getParent();
    if (!MDT.dominates(MBB, UserMBB))
      continue;

    if (UserMBB == MBB) {
      auto *UserSUnit = DAG->getSUnit(&User);
      if (!UserSUnit)
        continue;
      if (UserSUnit->NodeNum < InsertionPointNodeNum)
        continue;
    }

    LLVM_DEBUG(dbgs() << "Checking " << User);
    if (TII->isNativeS20Consumer(User, *MRI))
      PtrModCount += 1;
    else
      UseDefsPastUseInstr += 1;
  }

  if (PHIUsageInSameMBB)
    UseDefsPastUseInstr++;

  return {/*PtrModCount=*/PtrModCount,
          /*NonPtrModCount=*/UseDefsPastUseInstr};
}

bool PointerModifierCombiner::canMove(const SUnit *Candidate,
                                      const bool MoveDown) const {
  // Moving another combiner may change the ordering of the final Combiners,
  // thus overlap gain Calculation & conflicts should be made aware of the
  // changes in the final Combiners ordering.
  /// Fixme: Properly calculate Overlap-gain and conflicts if combiners are
  /// moved and allow all instructions to be moved.
  assert(Candidate);
  if (!isCombineRootCandidate(Candidate->getInstr()))
    return true;

  // Ordering of Memory Instructions has to be preserved.
  // Note: Requires that the InsertionPoint is always the Memory Instruction
  const bool MIBelowInsertion = Candidate->NodeNum > InsertionPointNodeNum;
  return (MoveDown && MIBelowInsertion) || (!MoveDown && !MIBelowInsertion);
}

bool PointerModifierCombiner::isCombineRootCandidate(
    const MachineInstr *MI) const {
  switch (MI->getOpcode()) {
  case TargetOpcode::G_STORE:
  case TargetOpcode::G_LOAD:
  case TargetOpcode::G_SEXTLOAD:
  case TargetOpcode::G_ZEXTLOAD:
    return true;
  }
  return false;
}

const PtrModGain &PointerModifierCombiner::getGain() const { return Gain; }

bool PointerModifierCombiner::setupCombiner(
    std::vector<MachineInstr *> CombineInstrs,
    const AIE::DataDependenceHelper *DAG) {
  setupCombineInstrs(CombineInstrs, DAG);
  return true;
}

void PointerModifierCombiner::setupCombineInstrs(
    std::vector<MachineInstr *> CombineInstrs,
    const AIE::DataDependenceHelper *DAG) {
  assert(CombineInstrs.size() == 2);
  assert(DAG);
  assert(CombineInstrs[1]->mayLoadOrStore());
  this->CombinerData.CombineInstrs = CombineInstrs;

  CombinerData.RemoveInstrs.resize(2);
  if (RemovePtrMod)
    // ptr modifier
    CombinerData.RemoveInstrs.set(0);

  this->DAG = DAG;
  for (auto *MI : this->CombinerData.CombineInstrs) {
    auto *SUnit = DAG->getSUnit(MI);
    if (!SUnit) {
      assert(!MI->mayLoadOrStore());
      // if CombineInstrs exist outside of the current MBB,
      // canReorderCombineInstrs should take care of removing invalid
      // combiners, i.e. combiners, that need to move the
      // outside-of-the-MBB-instruction
      continue;
    }

    CombineInstrNodeNum.push_back(SUnit->NodeNum);
  }

  // set CombineRoot
  CombinerData.CombineRoot = CombineInstrs[1];
  assert(isCombineRootCandidate(CombinerData.CombineRoot));
  CombineRootNodeNum = CombineInstrNodeNum.back();

  setInsertionPoint();
}

void PointerModifierCombiner::setInsertionPoint() {
  MachineInstr *MemI = getMemI();
  assert(MemI->mayLoadOrStore());
  CombinerData.InsertionPoint = MemI;
  InsertionPointNodeNum = CombineInstrNodeNum.back();
}

const MachineInstr *PointerModifierCombiner::getPtrInc() const {
  return CombinerData.CombineInstrs[0];
}

MachineInstr *PointerModifierCombiner::getPtrInc() {
  return CombinerData.CombineInstrs[0];
}

bool PointerModifierCombiner::hasOverlapPenalty(
    const GenericCombiner *Combiner) const {
  // Fixme: consider Ordering in the Overlap Penalty calculation.
  // Currently these two Variants have the same penalty, even though variant 1
  // should have a lower penalty, since Variant 2 has to copy %p0 twice:
  // Variant 1:
  // p1 = COPY p0
  // VLDA [p0], 64
  // VLDA [p1, 64]
  // VLDA [p1, 128]
  // Variant 2:
  // p1 = COPY p0
  // VLDA [p0], 64
  // VLDA [p1, 64]
  // p0 = COPY p1
  // VLDA [p0], 128
  // VLDA [p0, 64]

  if (!Combiner)
    return false;

  const PointerModifierCombiner *PtrModCombiner =
      static_cast<const PointerModifierCombiner *>(Combiner);

  if (PtrModCombiner->getPtrInc() != getPtrInc())
    // if combiners don't overlap, there cannot be an overlap penalty
    return false;

  const bool NoRemoveInstrsCombiner =
      Combiner->CombinerData.RemoveInstrs.none();
  if ((Combiner->CombinerData.RemoveInstrs.any() &&
       CombinerData.RemoveInstrs.any()) ||
      (NoRemoveInstrsCombiner && CombinerData.RemoveInstrs.none()))
    // OverlapPenalty can only occur, if one Combiner has RemoveInstructions
    // and the Other Combiner does not have Remove instructions
    return false;

  const auto ToRemoveInsertionNodeNum = NoRemoveInstrsCombiner
                                            ? InsertionPointNodeNum
                                            : Combiner->InsertionPointNodeNum;
  const auto NonRemoveInsertionNodeNum = NoRemoveInstrsCombiner
                                             ? Combiner->InsertionPointNodeNum
                                             : InsertionPointNodeNum;

  // overlap penalty occurs, if Non-remove Use occurs after a Remove Combiner
  return ToRemoveInsertionNodeNum < NonRemoveInsertionNodeNum;
}

CombinerGain PointerModifierCombiner::getOverlapPenalty(
    const GenericCombiner *Combiner) const {
  if (!hasOverlapPenalty(Combiner))
    return {};

  LLVM_DEBUG(dbgs() << "Overlap Penalty\n");
  PtrModGain Obj;
  Obj.setNoCopy(true);
  return Obj;
}

CombinerGain PointerModifierCombiner::getImmediateReuseGain(
    const std::vector<APInt> &UsedImmediates) const {

  bool ReuseImmediate = any_of(getAllImmInRegs(), [&](APInt &ImmInReg) {
    return find(UsedImmediates, ImmInReg) != UsedImmediates.end();
  });

  if (!ReuseImmediate)
    return {};

  LLVM_DEBUG(dbgs() << "Immediate Register Reuse, using Ideal Gain\n");
  PtrModGain Obj;
  Obj.setValidImm(true);
  return Obj;
}

bool PointerModifierCombiner::tryToSetCombinedOpCode() {
  auto OpCode = getOpCode(getPtrInc(), getMemI());
  if (!OpCode)
    return false;

  CombinerData.CombinedInstrOpcode = *OpCode;
  return true;
}

// -------------------------- OffsetCombiner ---------------------------------//

bool OffsetCombiner::isCombineCandidate(MachineInstr &CombineRoot,
                                        MachineInstr &Candidate) const {
  return getOpCode(&CombineRoot, &Candidate).has_value();
}

std::vector<MachineInstr *>
OffsetCombiner::getPtrInstrs(MachineInstr *MemI) const {
  assert(MemI->mayLoadOrStore());
  const int PtrIdx = 1;
  const auto InputPtrReg = MemI->getOperand(PtrIdx).getReg();
  MachineOperand *PtrOrigin = MRI->getOneDef(InputPtrReg);
  if (!PtrOrigin)
    return {};

  auto *PtrInc = PtrOrigin->getParent();
  if (!getOpCode(PtrInc, MemI))
    return {};

  // Skip if the G_PTR_ADD is inside a loop and the memory instruction is
  // outside that loop. This avoids creating offset combines that would depend
  // on intermediate pointer values from the loop body.
  assert(MLI && "MachineLoopInfo must be provided");
  const MachineBasicBlock *PtrIncMBB = PtrInc->getParent();
  const MachineBasicBlock *MemIMBB = MemI->getParent();
  if (PtrIncMBB != MemIMBB) {
    const MachineLoop *PtrIncLoop = MLI->getLoopFor(PtrIncMBB);
    if (PtrIncLoop && !PtrIncLoop->contains(MemIMBB)) {
      LLVM_DEBUG(dbgs() << "  [OffsetCombiner] Skipping: G_PTR_ADD is in loop, "
                           "MemI is outside\n");
      return {};
    }
  }

  return {PtrInc};
}

std::unique_ptr<GenericCombiner> OffsetCombiner::clone() const {
  return std::make_unique<OffsetCombiner>(*this);
}

std::optional<std::pair<std::vector<SUnit *>, std::vector<SUnit *>>>
OffsetCombiner::getInstructionsToMove(const AIE::DataDependenceHelper &DAG) {
  auto *PtrAdd = getPtrInc();
  if (!getImm(*PtrAdd, *MRI)) {
    /// Offset is not an immediate and the OffsetCombiner is not eligible for
    /// reordering.
    // Since the Offset already dominates the MemoryInstruction (where the
    // insertion happens), no checks have to be performed.
    return {{/*MoveUp=*/{}, /*MoveDown=*/{}}};
  }

  auto *SUnitPtrAdd = DAG.getSUnit(PtrAdd);
  if (!SUnitPtrAdd) {
    /// PtrAdd is an Immediate but it is outside of the MBB, so it already
    /// dominates the MemoryInstruction. No checks have to be performed.
    return {{/*MoveUp=*/{}, /*MoveDown=*/{}}};
  }

  /// Immediate Offset can be a reordering Candidate. Therefore, track Immediate
  /// Offset, so it can be moved in case of a reordering.
  return {{/*MoveUp=*/{SUnitPtrAdd}, /*MoveDown=*/{}}};
}

void OffsetCombiner::adjustGain(const MachineDominatorTree &MDT) {
  const auto *PtrAdd = getPtrInc();
  assert(PtrAdd->getOpcode() == TargetOpcode::G_PTR_ADD);

  const auto DefAddr = PtrAdd->getOperand(0).getReg();
  // If another pointer modifier occurs, discount the gain.
  // If there are only uses of the PtrAdd Instruction that do not modify the
  // pointer, we do not have to discount the gain, since the Uses can be Offset
  // Load/Stores.
  auto UsageCounter = getUsageCount(DefAddr, MDT);
  if (UsageCounter.PtrModCount > 0) {
    LLVM_DEBUG(dbgs() << "Encountered PtrMod \n");
    // PtrMod cannot be removed, because  the result is still needed in another
    // PtrMod
    Gain.setPtrMod(0);
  }

  ImmOffset = getImm(*PtrAdd, *MRI);
  if (!ImmOffset)
    return;

  if (!TII->isOffsetInImmediateRange(
          getCombinedOpCode(), getLoadStoreSize(*CombinerData.CombineRoot),
          ImmOffset)) {
    // do not add ImmOffset to ImmInRegs
    // Multiple ptr_adds with constant immediates may be merged into a single
    // ptr_add with the sum of combined Immediates. This happens if Address
    // chaining is enabled and Preincrements are used as combiners. In this
    // specific case, these ImmOffsets are only a suggestion, rather than the
    // actual final Immediate Value.
    LLVM_DEBUG(dbgs() << "no valid imm range!\n");
    Gain.setValidImm(false);
    return;
  }
}

std::optional<unsigned> OffsetCombiner::getOpCode(MachineInstr *PtrInc,
                                                  MachineInstr *MemI) const {
  assert(TII);
  if (PtrInc->getOpcode() != TargetOpcode::G_PTR_ADD)
    return {};

  return TII->getOffsetMemOpcode(MemI->getOpcode());
}

bool OffsetCombiner::canReorder() const { return ImmOffset.has_value(); }

bool OffsetCombiner::isReorderCandidate(
    const GenericCombiner *PostIncCombiner) const {
  auto GetInputPtr = [&](const MachineInstr *PtrMod) {
    auto InputPtrIdx = TII->getInputPtrIdx(*PtrMod, *MRI);
    assert(InputPtrIdx);
    return PtrMod->getOperand(*InputPtrIdx);
  };

  const PointerModifierCombiner *PtrModCombiner =
      static_cast<const PointerModifierCombiner *>(PostIncCombiner);
  if (!PtrModCombiner->isPostInc())
    return false;

  // only allow loads to be reordered
  if (getMemI()->mayStore() || PtrModCombiner->getMemI()->mayStore())
    return false;

  // Same MBB check
  auto *PtrAdd = getPtrInc();
  auto *PostIncPtrMod = PtrModCombiner->getPtrInc();
  if (PtrAdd->getParent() != PostIncPtrMod->getParent())
    return false;

  // Same Input Ptr Check
  auto InputPtr = GetInputPtr(PtrAdd);
  auto PostIncInputPtr = GetInputPtr(PostIncPtrMod);
  if (!InputPtr.isIdenticalTo(PostIncInputPtr))
    return false;

  // Check if Store Instruction of Offset dominates PostInc
  auto *MemI = getMemI();
  if (MemI->mayStore()) {
    auto Source = MemI->getOperand(0);
    assert(Source.isReg());
    auto *DefSource = MRI->getUniqueVRegDef(Source.getReg());
    if (!DefSource)
      return false;
    auto *DefSUnit = DAG->getSUnit(DefSource);
    if (DefSUnit &&
        DefSUnit->NodeNum > PostIncCombiner->InsertionPointNodeNum) {
      // Source of Offset-Store would be after the new InsertionPoint and thus
      // generate invalid mir
      return false;
    }
  }

  // Reject if a scheduling barrier (side-effecting instruction, call, etc.)
  // sits between the two insertion points. Moving the load above such a
  // barrier would violate memory ordering.
  const SUnit &OffsetSU = DAG->SUnits[InsertionPointNodeNum];
  for (const SDep &Pred : OffsetSU.Preds) {
    if (!Pred.isBarrier())
      continue;

    // The instruction would move from its current position to the candidate's
    // insertion point. Reject if the barrier sits in that range.
    const unsigned BarrierNodeNum = Pred.getSUnit()->NodeNum;
    const bool DestBeforeBarrier =
        PostIncCombiner->InsertionPointNodeNum < BarrierNodeNum;
    const bool SourceAfterBarrier = InsertionPointNodeNum > BarrierNodeNum;
    if (DestBeforeBarrier && SourceAfterBarrier)
      return false;
  }

  // OffsetCombiner occurs after PostIncCombiner
  return InsertionPointNodeNum > PostIncCombiner->InsertionPointNodeNum;
}

// -------------------------- PostIncCombiner --------------------------------//

bool PostIncCombiner::isCombineCandidate(MachineInstr &MemI,
                                         MachineInstr &Candidate) const {
  auto MemSize = MRI->getType(MemI.getOperand(0).getReg()).getSizeInBits();
  return TII->getCombinedPostIncOpcode(MemI, Candidate, MemSize).has_value();
}

bool PostIncCombiner::setupCombiner(std::vector<MachineInstr *> CombineInstrs,
                                    const AIE::DataDependenceHelper *DAG) {
  setupCombineInstrs(CombineInstrs, DAG);
  assert(CombinerData.CombineInstrs.size() == 2);

  const MachineInstr *PtrInc = getPtrInc();
  const MachineInstr *MemI = getMemI();
  if (PtrInc->getParent() != MemI->getParent())
    /// Combiner spans across multiple MBBs
    return false;

  UserIntrinsic = PtrInc->getOpcode() != TargetOpcode::G_PTR_ADD;
  CombinerData.Name = UserIntrinsic ? "UserIntrinsic" : "PostInc";

  return true;
}

std::unique_ptr<GenericCombiner> PostIncCombiner::clone() const {
  return std::make_unique<PostIncCombiner>(*this);
}

// What this blocks, using a loop that reads three vectors off a base advanced
// by a separate 2D increment. Address clustering has turned the sibling
// offsets 384/256/128 into a chain, so every link feeds the next one:
//
//   %p    = G_PHI %entry, %bb.0, %next, %bb.1
//   %p384 = G_PTR_ADD %p, 384
//   %a    = G_LOAD %p384
//   %p256 = G_PTR_ADD %p384, -128
//   %b    = G_LOAD %p256
//   %p128 = G_PTR_ADD %p256, -128
//   %c    = G_LOAD %p128
//   %next = add.2d(%p, ...)          <- advances %p, not part of the chain
//
// Post-incrementing the chain is locally cheaper (it absorbs a ptr_add per
// link), but %p has to survive the whole walk because add.2d still reads it.
// That needs a second cursor, which costs a copy of %p plus a padda in every
// iteration:
//
//   movs p6, p4;  padda [p6], #384
//   vldb x10, [p6], #-128;  vldb x11, [p6, #-128];  vldb x3, [p6, #0]
//
// Declining the post-increment leaves the chain intact for instruction
// selection, which rebases each load onto %p and lets the chain die:
//
//   vldb x10, [p4, #384];  vldb x3, [p4, #256];  vldb x11, [p4, #128]
//
// This does not fire when the chain tail is itself the loop advance, i.e. when
// %next is a link of the same chain. Then a single cursor walks the whole
// group and the post-increments are strictly better, so they are left alone.
bool PostIncCombiner::chainingForcesExtraCursor() const {
  const MachineInstr *PtrMod = getPtrInc();
  const MachineBasicBlock *LoopMBB = PtrMod->getParent();
  auto InputPtrIdx = TII->getInputPtrIdx(*PtrMod, *MRI);
  if (!InputPtrIdx)
    return false;

  auto [Root, RootOffset] =
      getChainRoot(PtrMod->getOperand(*InputPtrIdx).getReg(), *MRI);

  // The offset form is only viable if instruction selection can rebase the
  // access onto the chain root, which needs the flattened displacement to be
  // encodable.
  const MachineInstr *MemI = getMemI();
  if (!TII->isOffsetInImmediateRange(
          TII->getOffsetMemOpcode(MemI->getOpcode()), getLoadStoreSize(*MemI),
          APInt(PtrModBits, RootOffset, /*isSigned=*/true)))
    return false;

  // Outside a loop the extra cursor is a one-off setup cost with no
  // per-iteration payoff, so it is not worth giving up the post-increment.
  // Restrict to a header PHI in this block with at most two incomings
  // (preheader + latch).
  const MachineInstr *RootDef =
      Root.isVirtual() ? MRI->getVRegDef(Root) : nullptr;
  const auto *Phi = dyn_cast_or_null<GPhi>(RootDef);
  if (!Phi || Phi->getParent() != LoopMBB || Phi->getNumIncomingValues() > 2)
    return false;

  auto GetIncomingReg =
      [](const GPhi &P,
         const MachineBasicBlock *MBB) -> std::optional<Register> {
    for (unsigned I = 0, E = P.getNumIncomingValues(); I != E; ++I)
      if (P.getIncomingBlock(I) == MBB)
        return P.getIncomingValue(I);
    return std::nullopt;
  };

  std::optional<Register> IncReg = GetIncomingReg(*Phi, LoopMBB);
  if (!IncReg)
    return false;

  // If the value carried around the backedge is itself part of the chain, the
  // walk doubles as the loop advance and one cursor is enough.
  Register Carried = getChainRoot(*IncReg, *MRI).first;
  if (Carried == Root)
    return false;

  // Flattening the chain leaves the advance as the only pointer modifier, so it
  // gets folded into an access, which then has to wait for the stride:
  //
  //   %s        = compute ...           <- recomputed every iteration
  //   %v, %next = POSTINC_LOAD %p, %s   <- pulled onto the critical path
  //
  // A stride defined outside the loop is ready on entry, so the fold is free.
  //
  // TODO: an add.2d/3d advance can hit the same problem when its dimension
  // operands are computed in the loop instead of set up before it. Those
  // operands are not checked here.
  const MachineInstr *AdvanceMI =
      Carried.isVirtual() ? MRI->getVRegDef(Carried) : nullptr;
  if (AdvanceMI && AdvanceMI->getOpcode() == TargetOpcode::G_PTR_ADD) {
    Register Stride = AdvanceMI->getOperand(2).getReg();
    const MachineInstr *StrideDef =
        Stride.isVirtual() ? MRI->getVRegDef(Stride) : nullptr;
    if (!StrideDef || StrideDef->getParent() == LoopMBB)
      return false;
  }
  return true;
}

void PostIncCombiner::adjustGain(const MachineDominatorTree &MDT) {
  const auto *PtrAdd = getPtrInc();

  auto InputPtrIdx = TII->getInputPtrIdx(*PtrAdd, *MRI);
  assert(InputPtrIdx);

  // Input pointer may be copied in later usages, penalize post-inc gain
  Gain.GainVector[2] = 0;
  if (PenalizeExtraCursorPostInc && chainingForcesExtraCursor()) {
    LLVM_DEBUG(dbgs() << "Chaining forces an extra cursor\n");
    // Keeping the chain costs a copy of the root every iteration, while the
    // offset form lets instruction selection rebase onto the root and drop
    // the whole chain. Do not credit the post-increment for the ptr_add it
    // absorbs.
    Gain.setPtrMod(0);
  }
  if (UserIntrinsic)
    /// prioritize user intrinsics
    Gain.setPtrMod(2);

  if (PtrAdd->getOpcode() != TargetOpcode::G_PTR_ADD)
    return;

  std::optional<APInt> ImmOffset = getImm(*PtrAdd, *MRI);
  if (!ImmOffset)
    return;

  if (!TII->isOffsetInImmediateRange(
          getCombinedOpCode(), getLoadStoreSize(*CombinerData.CombineRoot),
          ImmOffset)) {
    ImmInRegs.push_back(*ImmOffset);
    LLVM_DEBUG(dbgs() << "no valid imm range!\n");
    Gain.setValidImm(false);
  }
}

std::vector<MachineInstr *>
PostIncCombiner::getPtrInstrs(MachineInstr *CombineRoot) const {
  assert(isCombineRootCandidate(CombineRoot));
  std::vector<MachineInstr *> ResultInstr;
  // Load/Store have the input pointer at operand[1]
  const int PtrIdx = 1;
  Register PtrReg = CombineRoot->getOperand(PtrIdx).getReg();
  for (auto &Use : MRI->use_nodbg_instructions(PtrReg)) {
    if (isCombineCandidate(*CombineRoot, Use))
      ResultInstr.push_back(&Use);
  }

  return ResultInstr;
}

std::optional<unsigned> PostIncCombiner::getOpCode(MachineInstr *PtrInc,
                                                   MachineInstr *MemI) const {
  assert(TII);
  return TII->getCombinedPostIncOpcode(
      *MemI, *PtrInc,
      MRI->getType(getMemI()->getOperand(0).getReg()).getSizeInBits());
}

// -------------------------- PtrModGain -------------------------------------//

void PtrModGain::setPtrMod(const int Value) { GainVector[0] = Value; }

void PtrModGain::setValidImm(const bool ValidImm) {
  GainVector[1] = ValidImm ? 1 : 0;
}

void PtrModGain::setNoCopy(const bool NoCopy) {
  GainVector[2] = NoCopy ? 1 : 0;
}

} // namespace llvm::AIE
