//=== lib/CodeGen/GlobalISel/AIECombinerHelper.cpp
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIECombinerHelper.h"
#include "AIE.h"
#include "AIE2TargetMachine.h"
#include "AIEBaseInstrInfo.h"
#include "MCTargetDesc/AIE2MCTargetDesc.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/Analysis/VectorUtils.h"
#include "llvm/CodeGen/GlobalISel/GenericMachineInstrs.h"
#include "llvm/CodeGen/GlobalISel/LegalizerHelper.h"
#include "llvm/CodeGen/GlobalISel/Utils.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegionInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/IR/IntrinsicsAIE2.h"
#include "llvm/IR/IntrinsicsAIE2P.h"
#include <optional>

#define DEBUG_TYPE "aie-combine"

using namespace llvm;

static cl::opt<unsigned> ShuffleMaxNumInsertions(
    "aie-shuffle-combine-max-inserts", cl::Hidden, cl::init(0),
    cl::desc(
        "Maximum number of insertions allowed to scalarize a shuffle pattern"));
static cl::opt<bool> EnableOffsetCombine(
    "aie-offset-combine", cl::Hidden, cl::init(true),
    cl::desc("Enable combines of load and stores with an offset"));
static cl::opt<bool> EnablePostIncCombine(
    "aie-postinc-combine", cl::Hidden, cl::init(true),
    cl::desc("Enable combines of load and stores with post increments"));
static cl::opt<bool> EnableGreedyAddressCombine(
    "aie-greedy-address-combines", cl::Hidden, cl::init(false),
    cl::desc("Enable greedy combines without checking for later uses of the "
             "base pointer"));
static cl::opt<bool>
    EnableS20Narrowing("aie-s20-narrowing", cl::Hidden, cl::init(true),
                       cl::desc("Enable s20 operand narrowing optimization"));

cl::opt<bool> InlineMemCalls("aie-inline-mem-calls", cl::init(true), cl::Hidden,
                             cl::desc("Inline mem calls when profitable."));

cl::opt<bool> CombineVecShiftByZero(
    "aie-combine-vec-shift-by-zero", cl::init(true), cl::Hidden,
    cl::desc("Combine vectors shift by zero into copies."));

static unsigned getNumMaskUndefs(const ArrayRef<int> &Mask,
                                 unsigned StartIndex) {
  unsigned Count = 0;
  for (unsigned I = StartIndex; I < Mask.size(); ++I) {
    if (Mask[I] == -1) {
      ++Count;
    }
  }
  return Count;
}

bool MaskMatch::isValidMask(const ArrayRef<int> Mask) const {
  for (unsigned Idx = 0; Idx < Mask.size(); ++Idx) {
    if (Mask[Idx] == -1)
      continue;

    // Check not undef values (not -1) of the mask
    if ((unsigned)Mask[Idx] != getMaskValue(Idx)) {
      return false;
    }
  }
  return true;
}

ShuffleMaskValidity
MaskMatch::getShuffleMaskValidity(const ArrayRef<int> Mask) const {
  SmallVector<unsigned, 4> MaskExceptions;
  for (unsigned Idx = 0; Idx < Mask.size(); ++Idx) {
    if (Mask[Idx] == -1)
      continue;

    // Check not undef values (not -1) of the mask
    if ((unsigned)Mask[Idx] != getMaskValue(Idx)) {
      MaskExceptions.push_back(Idx);
    }
  }

  if (MaskExceptions.empty())
    return {true, MaskExceptions};
  return {false, MaskExceptions};
}

bool MaskMatch::isMaskWithAllUndefs(ArrayRef<int> Mask) {
  for (unsigned I = 0; I < Mask.size(); ++I) {
    if (Mask[I] != -1)
      return false;
  }
  return true;
}

// Checks if the mask is in range, allowing some values to be
// undef (-1), but not all.
bool MaskMatch::isMaskWithinRangeOrUndef(ArrayRef<int> Mask, int MinValue,
                                         int MaxValue) {
  return llvm::any_of(Mask, [](int v) { return v != -1; }) &&
         llvm::all_of(Mask, [=](int v) {
           return v == -1 || (v >= MinValue && v <= MaxValue);
         });
}

std::optional<unsigned> MaskMatch::getHeight(ArrayRef<int> Mask,
                                             unsigned Period) {
  for (unsigned I = 0; I < Mask.size(); ++I) {
    if (Mask[I] != -1)
      return Mask[I] - (Period == 0 ? I : I % Period);
  }
  return std::nullopt;
}

/// This function returns the unique index in the shuffle mask \p Mask if the
/// unique index exists.
std::optional<int> MaskMatch::getUniqueIndex(ArrayRef<int> Mask) {
  std::optional<int> UniqOpIdx;
  for (unsigned I = 0; I < Mask.size(); I++) {
    int Idx = Mask[I];
    if (Idx == -1)
      continue;

    if (!UniqOpIdx) {
      UniqOpIdx = Idx;
      continue;
    }

    if (UniqOpIdx != Idx) {
      return std::nullopt;
    }
  }
  return UniqOpIdx;
}

static std::unordered_map<int, unsigned>
getMaskFrequencyMap(const ArrayRef<int> Mask) {
  assert(!MaskMatch::isMaskWithAllUndefs(Mask));
  std::unordered_map<int, unsigned> FrequencyMap;
  for (int Idx : Mask) {
    if (Idx == -1)
      continue;
    FrequencyMap[Idx]++;
  }
  return FrequencyMap;
}

std::optional<FrequentIndexResult>
MaskMatch::getFrequentIndexResult(const ArrayRef<int> Mask,
                                  unsigned MinFrequency = 0) {

  // Set the default value for MinFrequency
  if (MinFrequency == 0) {
    MinFrequency = Mask.size() / 2;
  }

  std::unordered_map<int, unsigned> FrequencyMap = getMaskFrequencyMap(Mask);
  unsigned DontCareCount = getNumMaskUndefs(Mask, 0);

  auto [FrequentValue, HighestFrequency] = *std::max_element(
      FrequencyMap.begin(), FrequencyMap.end(),
      [](const std::pair<int, unsigned> p1, const std::pair<int, unsigned> p2) {
        return p1.second < p2.second;
      });

  unsigned HighestAdjustedFrequency = HighestFrequency + DontCareCount;
  if (HighestAdjustedFrequency < MinFrequency) {
    return std::nullopt;
  }

  unsigned NonMatchingCount = Mask.size() - HighestAdjustedFrequency;

  unsigned FrequentIdx = 0;
  for (unsigned I = 0; I < Mask.size(); I++) {
    int MaskValue = Mask[I];
    if (MaskValue == FrequentValue) {
      FrequentIdx = I;
      break;
    }
  }

  return FrequentIndexResult{FrequentIdx, NonMatchingCount};
}

static MachineInstr *findPreIncMatch(MachineInstr &MemI,
                                     MachineRegisterInfo &MRI,
                                     CombinerHelper &Helper,
                                     const AIEBaseInstrInfo &TII,
                                     AIE::FoundCombiners *GlobalCombinerPtr) {
  // This is currently done with patterns in instruction selection.
  // No need to do it here.
  const unsigned VecSize =
      MRI.getType(MemI.getOperand(0).getReg()).getSizeInBits();
  if (VecSize > TII.getMaxSupportedLdStIncSize()) {
    return nullptr;
  }

  if (!EnableOffsetCombine)
    return nullptr;
  Register Addr = MemI.getOperand(1).getReg();
  MachineInstr *AddrDef = getDefIgnoringCopies(Addr, MRI);
  if (AddrDef->getOpcode() == TargetOpcode::G_PTR_ADD) {
    // 2 Instructions are in the Combiner
    BitVector RemoveInstrs(2);
    GlobalCombinerPtr->append(AIE::Combiner(
        /*CombineInstrs=*/std::vector<MachineInstr *>{AddrDef, &MemI},
        /*CombinedInstrOpcode=*/TII.getOffsetMemOpcode(MemI.getOpcode()),
        /*InsertionPoint=*/&MemI, /*CombineRoot=*/&MemI,
        /*MoveUpInstrsToInsertionPoint=*/std::vector<MachineInstr *>{},
        /*RemoveInstrs=*/RemoveInstrs, /*Name=*/"Offset-legacy"));
    return AddrDef;
  }
  return nullptr;
}

/// Checks if any operand of \a MI is defined by \a Def.
/// This is not transitive: it will not look at how the uses of \a Def are
/// defined.
bool isUseOf(const MachineInstr &MI, const MachineInstr &Def) {
  for (auto &Defs : Def.all_defs()) {
    for (auto &MIUse : MI.all_uses()) {
      if (MIUse.isReg() && Defs.getReg() == MIUse.getReg())
        return true;
    }
  }
  return false;
}

///  Check for dead \a InBetweenMI MI and copy-like instructions that can be
///  coalesced once \a MemI and \a Dest are combined.
bool isNonCoalesceableUseOf(const MachineInstr &MemI,
                            const MachineInstr &InBetweenMI,
                            const MachineInstr &Dest,
                            const MachineRegisterInfo &MRI) {

  if (isTriviallyDead(InBetweenMI, MRI))
    return false;

  // We can delay an instruction after a copy, if the copy just
  // connects MemI and Dest. After combining, this copy will be dead.
  if (InBetweenMI.isCopy() &&
      MRI.hasOneNonDBGUse(InBetweenMI.getOperand(1).getReg()) &&
      MRI.hasOneNonDBGUse(InBetweenMI.getOperand(0).getReg())) {
    const MachineInstr *CopyOrignMI =
        MRI.getVRegDef(InBetweenMI.getOperand(1).getReg());
    const MachineInstr *CopyDestMI =
        &*MRI.use_instr_nodbg_begin(InBetweenMI.getOperand(0).getReg());
    if (CopyOrignMI == &MemI && CopyDestMI == &Dest)
      return false;
  }

  return isUseOf(InBetweenMI, MemI);
}

/// \return true if \a MemI can be moved just before \a Dest in order to allow
/// post-increment combining
bool llvm::canDelayMemOp(MachineInstr &MemI, MachineInstr &Dest,
                         const MachineRegisterInfo &MRI) {
  if (MemI.getParent() != Dest.getParent())
    return false;
  auto MII = std::next(MemI.getIterator());
  auto MIE = Dest.getIterator();
  auto InstrRange = make_range(MII, MIE);
  bool SawStore = MemI.mayStore();
  auto UnsafeToMovePast = [&](const MachineInstr &MI) {
    return isNonCoalesceableUseOf(MemI, MI, Dest, MRI) ||
           !MI.isSafeToMove(nullptr, SawStore);
  };
  return none_of(InstrRange, UnsafeToMovePast);
}

/// \return true if \a Dest can be moved just after \a MemI in order to allow
/// combining
bool llvm::canAdvanceOp(MachineInstr &MemI, MachineInstr &Dest,
                        const MachineRegisterInfo &MRI) {
  assert(Dest.getOpcode() != TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS &&
         "Cannot advance Dest MI with side effects");
  assert(!Dest.mayLoadOrStore() && "Cannot advance load/store Dest MI");
  if (MemI.getParent() != Dest.getParent())
    return false;
  auto MII = std::next(MemI.getIterator());
  auto MIE = Dest.getIterator();
  auto InstrRange = make_range(MII, MIE);
  auto UnsafeToMoveBefore = [&](const MachineInstr &MI) {
    // Conditions that indicate it is unsafe to move:
    // 1 - G_INTRINSIC_W_SIDE_EFFECTS without explicit output, which may include
    // writing to a control register.
    // 2 - Crossing the definition of an input operand of Dest.
    return ((MI.getOpcode() == TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS &&
             MI.defs().empty()) ||
            isUseOf(Dest, MI));
  };
  return none_of(InstrRange, UnsafeToMoveBefore);
}

/// Find the def instruction for \p Reg, folding away any trivial copies and
/// bitcasts. May return nullptr if \p Reg is not a generic virtual register.
MachineInstr *
llvm::getDefIgnoringCopiesAndBitcasts(Register Reg,
                                      const MachineRegisterInfo &MRI) {

  MachineInstr *DefInstr = MRI.getVRegDef(Reg);

  auto IsSingleUseCopyOrBitcast = [&](const MachineInstr *MI) {
    return (MI->isCopy() ||
            (DefInstr->getOpcode() == TargetOpcode::G_BITCAST)) &&
           MRI.hasOneNonDBGUse(MI->getOperand(0).getReg());
  };

  auto UseVirtReg = [&](const MachineInstr *MI) {
    return MI->getOperand(1).getReg().isVirtual();
  };

  // No other use for this copy/bitcast.
  // Stop if we reach an use of a physical register.
  while (DefInstr && IsSingleUseCopyOrBitcast(DefInstr) && UseVirtReg(DefInstr))
    DefInstr = MRI.getVRegDef(DefInstr->getOperand(1).getReg());

  return DefInstr;
}

MachineInstr *findLastRegUseInBB(Register Reg, MachineInstr &IgnoreUser,
                                 MachineRegisterInfo &MRI,
                                 CombinerHelper &Helper,
                                 const AIEBaseInstrInfo &TII) {
  MachineInstr *LastRegUse = nullptr;
  for (auto &Use : MRI.use_nodbg_instructions(Reg)) {
    if (&Use == &IgnoreUser || Use.getParent() != IgnoreUser.getParent())
      continue;
    if (!LastRegUse || Helper.dominates(*LastRegUse, Use))
      LastRegUse = &Use;
  }
  return LastRegUse;
}

bool checkRegUsesDominate(Register Reg, MachineInstr &Instr,
                          MachineInstr &IgnoreUser, MachineRegisterInfo &MRI,
                          CombinerHelper &Helper, const AIEBaseInstrInfo &TII) {
  for (auto &Use : MRI.use_nodbg_instructions(Reg)) {
    if (&Use == &IgnoreUser)
      continue;
    if (!Helper.dominates(Use, Instr))
      return false;
  }
  return true;
}

MachineInstr *
findEarliestInsertPoint(MachineInstr &Instr, MachineInstr &NoMoveBeforeInstr,
                        bool NoMoveBeforeLastUse, MachineRegisterInfo &MRI,
                        CombinerHelper &Helper, const AIEBaseInstrInfo &TII) {
  // get all defs and possibly the end point
  // filter out the ones that are not in the BB of Instr and find the latest one
  // and return the insertion point after it
  assert(&Instr != &NoMoveBeforeInstr &&
         "NoMoveBeforeInstr and Instr have to be different");
  MachineInstr *EarliestInstrPos = nullptr;
  if (NoMoveBeforeInstr.getParent() == Instr.getParent()) {
    if (Helper.dominates(NoMoveBeforeInstr, Instr)) {
      // Since the Instr has to be after, NoMoveBeforeInstr can never be the
      // last instrucion of the BB
      EarliestInstrPos =
          &*next_nodbg(NoMoveBeforeInstr.getIterator(),
                       NoMoveBeforeInstr.getParent()->instr_end());
    } else {
      // If NoMoveBeforeInstr is after Instr we don't move Instr up
      return &Instr;
    }
  } else {
    // NoMoveBeforeInstr and Instr are in different BBs.
    // At the moment we don't combine instructions across BB boundaries and
    // we can just return the current position of the Instr. If in the future
    // we want to combine across BB boundaries this case will probably have
    // to be handled differently.
    return &Instr;
  }
  // Loop over all definitions of the uses of the instruction and make sure we
  // do not move past any of them (except G_CONSTANT which we can fix after
  // applying the move)
  for (auto &Use : Instr.uses()) {
    if (!Use.isReg())
      continue;
    MachineInstr *DefInstr = MRI.getUniqueVRegDef(Use.getReg());
    // Ignore moves past G_CONSTANT
    if (DefInstr->getOpcode() == TargetOpcode::G_CONSTANT)
      continue;
    if (DefInstr->getParent() == Instr.getParent() &&
        Helper.dominates(*EarliestInstrPos, *DefInstr)) {
      // Since the Def is in the same BB as Instr and Instr has to be afterwards
      // DefInstr can never be the last Instruction of the BB
      EarliestInstrPos = &*next_nodbg(DefInstr->getIterator(),
                                      DefInstr->getParent()->instr_end());
    }
    if (NoMoveBeforeLastUse) {
      // For some instructions we might not want to move before the latest use
      // of a register, e.g. if the instruction we are moving is going to change
      // the register inplace, as it is the case for PTR_ADDs.
      // %0 = $p0
      // %1 = LOAD %0
      // STORE $r0, [%0, #32]
      // %2 = PTR_ADD %0, #64
      // In this case for example we would not want to move the PTR_ADD before
      // the STORE because the PTR_ADD changes %0 inplace and if there is a
      // later use of %0 (in this case the STORE), then we would introduce an
      // unnecessary COPY. It is better to combine the LOAD and the PTR_ADD
      // after the STORE.
      MachineInstr *LastRegUse =
          findLastRegUseInBB(Use.getReg(), Instr, MRI, Helper, TII);
      if (LastRegUse && Helper.dominates(*LastRegUse, Instr) &&
          Helper.dominates(*EarliestInstrPos, *LastRegUse)) {
        // Since LastRegUse is in the same BB as Instr and it dominates Instr,
        // and both instruction are not the same there will always be an
        // instruction after LastRegUse
        EarliestInstrPos = &*next_nodbg(LastRegUse->getIterator(),
                                        LastRegUse->getParent()->instr_end());
      }
    }
  }
  return EarliestInstrPos;
}

static std::vector<MachineInstr *>
findConstantOffsetsToMove(MachineInstr &PtrAdd, MachineInstr &PtrAddInsertLoc,
                          const MachineRegisterInfo &MRI,
                          CombinerHelper &Helper) {
  // By moving the PtrAdd up without considering if we are moving past a
  // G_CONSTANT defining one of the uses of the PtrAdd we are generating
  // incorrect code (use before def). We have to search those G_CONSTANTs and
  // move them up as well.
  std::vector<MachineInstr *> GConstsToMove;

  for (auto Use : PtrAdd.uses()) {
    if (!Use.isReg())
      continue;
    MachineInstr *Def = MRI.getUniqueVRegDef(Use.getReg());
    if (Def->getOpcode() == TargetOpcode::G_CONSTANT &&
        Def->getParent() == PtrAdd.getParent() &&
        Helper.dominates(PtrAddInsertLoc, *Def))
      GConstsToMove.push_back(Def);
  }
  return GConstsToMove;
}

// Check that MI is after First and not after Last
static bool isBetween(MachineInstr &MI, MachineInstr &First, MachineInstr &Last,
                      CombinerHelper &Helper) {
  assert(First.getParent() == Last.getParent());
  // If it's in another block, it can't be between
  if (MI.getParent() != First.getParent()) {
    return false;
  }

  // We want First < MI && MI <= Last :=: !(MI <= First) && (MI <= Last)
  // and we have dominates(A, B) :=: A <= B
  return !Helper.dominates(MI, First) && Helper.dominates(MI, Last);
}

static MachineInstr *findPostIncMatch(MachineInstr &MemI,
                                      MachineRegisterInfo &MRI,
                                      CombinerHelper &Helper,
                                      const AIEBaseInstrInfo &TII,
                                      AIE::FoundCombiners *GlobalCombinerPtr) {
  if (!EnablePostIncCombine)
    return nullptr;

  const unsigned VecSize =
      MRI.getType(MemI.getOperand(0).getReg()).getSizeInBits();
  if (VecSize > TII.getMaxSupportedLdStIncSize()) {
    return nullptr;
  }
  // 2 Instructions are in the Combiner
  BitVector RemovePtrInc(2);
  // remove PtrInc
  RemovePtrInc.set(0);

  MachineInstr *InsertionPoint = nullptr;
  Register Addr = MemI.getOperand(1).getReg();
  AIE::Combiner TempCombiner;
  for (auto &PtrInc : MRI.use_nodbg_instructions(Addr)) {
    if (MemI.getParent() != PtrInc.getParent())
      continue;
    std::optional<unsigned> CombinedOpcode = TII.getCombinedPostIncOpcode(
        MemI, PtrInc, MRI.getType(MemI.getOperand(0).getReg()).getSizeInBits());
    if (!CombinedOpcode || isTriviallyDead(PtrInc, MRI))
      continue;
    // Find the closest location to the memory operation where the ptr_add can
    // be moved to.
    MachineInstr &PtrAddInsertLoc = *findEarliestInsertPoint(
        PtrInc, /*NoMoveBeforeInstr=*/MemI,
        /*NoMoveBeforeLastUse=*/true, MRI, Helper, TII);
    // If the PtrInc is defined before MemI, we need to make sure that there is
    // no use before def error if we combine the PtrInc into the MemI. We check
    // that none of the uses is between the PtrInc and the MemOp.
    if (Helper.dominates(PtrInc, MemI)) {
      if (any_of(MRI.use_nodbg_instructions(PtrInc.getOperand(0).getReg()),
                 [&](MachineInstr &PtrIncUse) {
                   return isBetween(PtrIncUse, PtrInc, MemI, Helper);
                 })) {
        continue;
      }
      InsertionPoint = &MemI;
      TempCombiner = AIE::Combiner(
          /*CombineInstrs=*/std::vector<MachineInstr *>{&PtrInc, &MemI},
          /*CombinedInstrOpcode=*/*CombinedOpcode,
          /*InsertionPoint=*/InsertionPoint, /*CombineRoot=*/&MemI,
          /*MoveUpInstrsToInsertionPoint=*/std::vector<MachineInstr *>{},
          /*RemoveInstrs=*/RemovePtrInc, /*Name=*/"PostInc1");

      // The offset of the PtrInc might be defined after MemI, in this case we
      // want to verify if it would be possible to insert the combined
      // instruction at the PtrInc instead of the location of MemI. Instruction
      // with side effects are also blocking: Loads, stores, calls, instructions
      // with side effects cannot be moved.
      // TODO: try move other instructions that block us from combining
    } else if (canDelayMemOp(MemI, PtrAddInsertLoc, MRI)) {
      // If Definition of the offset is a G_CONSTANT we have to move that
      // instruction up
      InsertionPoint = &PtrAddInsertLoc;
      TempCombiner = AIE::Combiner(
          /*CombineInstrs=*/std::vector<MachineInstr *>{&PtrInc, &MemI},
          /*CombinedInstrOpcode=*/*CombinedOpcode,
          /*InsertionPoint=*/InsertionPoint, /*CombineRoot=*/&MemI,
          /*MoveUpInstrsToInsertionPoint=*/
          findConstantOffsetsToMove(PtrInc, PtrAddInsertLoc, MRI, Helper),
          /*RemoveInstrs=*/RemovePtrInc, /*Name=*/"PostInc2");
    } else {
      LLVM_DEBUG(dbgs() << "    Ignoring candidate " << PtrInc);
      continue;
    }
    // Only combine postIncs if we know that the original pointer is not used
    // after the insertion point: all uses of the pointer must dominate the
    // insertion point.
    // TODO: This heuristic is very conservative and we should allow combines if
    // a combine does not dominate the insertion point but can never follow the
    // insertion point, e.g. being in a sibling BB.
    bool AddrUsesDominatesInsertPoint =
        checkRegUsesDominate(Addr, *InsertionPoint, PtrInc, MRI, Helper, TII);
    if (EnableGreedyAddressCombine || AddrUsesDominatesInsertPoint) {
      GlobalCombinerPtr->append(TempCombiner);
      return &PtrInc;
    }
  }
  return nullptr;
}

/// Checking the following Concat-Unmerge-PHI pattern:
/// bb.0
/// concatIn0 = ...
/// concatIn1 = ...
/// bb.1
/// 1 = phi 7, bb.1, concatIn0, bb.0
/// 2 = phi 10, bb.1 concatIn1, bb.0
/// 3 = G_CONCAT 1, 2
/// ....
/// 6 = ...
/// 7, 8 = G_UNMERGE 6
/// 9,10 = G_UNMERGE 6
/// \return unmerge source (6) if the pattern is valid and the sub-vector index
/// used [ 0 (7, 9) or 1 (8, 10)]. Start the pattern checking from \p ConcatI .
/// Follow \p UseOpIdx (0 to n-1) of G_CONCAT to identify the pattern. Collect
/// all relevant Results in \p MatchData .
static bool findUnmergeOrigin(MachineInstr &ConcatI, const unsigned UseOpIdx,
                              const MachineRegisterInfo &MRI,
                              CombinerHelper &Helper,
                              AIEConcatUnmergeCombineMatchData &MatchData) {

  auto FindPhiUnmergeComponents =
      [UseOpIdx, &Helper, &MRI, &MatchData](
          MachineInstr *PhiInst) -> std::pair<MachineInstr *, Register> {
    // Find Unmerge and Concat Components of the PHI node.
    MachineInstr *UnmergeI = nullptr;
    Register UnmergeDefReg;
    for (unsigned I = 1; I < PhiInst->getNumOperands(); I += 2) {
      Register IncomingReg = PhiInst->getOperand(I).getReg();
      MachineBasicBlock *MBB = PhiInst->getOperand(I + 1).getMBB();
      MachineInstr *IncomingMI = MRI.getVRegDef(IncomingReg);

      // Checking if IncomingMI dominates phi node.
      if (Helper.dominates(*IncomingMI, *PhiInst)) {

        if (MatchData.NewConcatMBB && MatchData.NewConcatMBB != MBB) {
          LLVM_DEBUG(dbgs() << "Parts of the Future Concat do not "
                               "originate in the same MBB.\n");
          return {};
        }

        // Concat Components.
        MatchData.NewConcatMBB = MBB;
        MatchData.ConcatSubVecs[UseOpIdx] = IncomingReg;
        continue;
      }

      if (IncomingMI->getOpcode() != TargetOpcode::G_UNMERGE_VALUES) {
        LLVM_DEBUG(dbgs() << "Could not find Unmerge Instruction\n");
        return {};
      }
      // Unmerge Components.
      if (UnmergeI) {
        LLVM_DEBUG(dbgs() << "Encountered too many Unmerge Instructions\n");
        return {};
      }
      UnmergeDefReg = IncomingReg;
      UnmergeI = IncomingMI;
    }
    return {UnmergeI, UnmergeDefReg};
  };

  // Find PHI node.
  const unsigned OperandIdx = UseOpIdx + 1;
  assert(ConcatI.getOperand(OperandIdx).isReg());
  const Register Reg = ConcatI.getOperand(OperandIdx).getReg();
  MachineInstr *PhiInst = MRI.getVRegDef(Reg);
  if (!PhiInst->isPHI() || !MRI.hasOneNonDBGUse(Reg))
    return false;

  auto [UnmergeInst, UnmergeDefReg] = FindPhiUnmergeComponents(PhiInst);

  if (!UnmergeInst || !MatchData.NewConcatMBB) {
    // Could not find Unmerge or Concat Components.
    return false;
  }

  const auto UnmergeSourceReg = UnmergeInst->uses().begin()->getReg();
  // Check consistency of MatchData.UnmergeSourceReg across different
  // UseOpIdx.
  if (MatchData.UnmergeSourceReg &&
      MatchData.UnmergeSourceReg != UnmergeSourceReg) {
    // Diverging Unmerge Source Registers.
    return false;
  }
  MatchData.UnmergeSourceReg = UnmergeSourceReg;

  const int UnmergeDefIdx =
      UnmergeInst->findRegisterDefOperandIdx(UnmergeDefReg, nullptr);
  assert(UnmergeDefIdx >= 0);
  // Make sure Unmerge and Concat do not reorder the subvectors.
  return UseOpIdx == (unsigned)UnmergeDefIdx;
}

/// This is the matching function for Concat-Unmerge-PHI pattern.
/// Convert the following:
/// bb.0
/// concatIn0 = ...
/// concatIn1 = ...
/// bb.1
/// 1 = phi 7, bb.1, concatIn0, bb.0
/// 2 = phi 8, bb.1 concatIn1, bb.0
/// 3 = G_CONCAT 1, 2
/// ....
/// 6 = ...
/// 7, 8 = G_UNMERGE 6
///
/// Into:
/// bb.0
/// concatIn0 = ...
/// concatIn1 = ...
/// 11 = G_CONCAT concatIn0, concatIn1
/// bb.1
/// 3 = phi 6, bb.1, 11, bb.0
/// ...
/// 6 =
///
/// \p ConcatI is the starting Point for this pattern.
/// \return true if the pattern is found.
bool llvm::matchConcatUnmergePhis(MachineInstr &ConcatI,
                                  MachineRegisterInfo &MRI,
                                  CombinerHelper &Helper,
                                  AIEConcatUnmergeCombineMatchData &MatchInfo) {
  assert(ConcatI.getOpcode() == TargetOpcode::G_CONCAT_VECTORS);
  LLVM_DEBUG(dbgs() << "MF: " << ConcatI.getMF()->getName() << "\n");

  const unsigned NumUses = ConcatI.getNumOperands() - ConcatI.getNumDefs();

  MatchInfo.ConcatSubVecs.resize(NumUses);

  for (unsigned UseOpIdx = 0; UseOpIdx < NumUses; UseOpIdx++) {
    if (!findUnmergeOrigin(ConcatI, /*UseOpIdx=*/UseOpIdx, MRI, Helper,
                           MatchInfo))
      return false;
  }

  assert(MatchInfo.NewConcatMBB);
  assert(MatchInfo.UnmergeSourceReg);
  return true;
}

void llvm::applyConcatUnmergePhis(MachineInstr &ConcatI,
                                  MachineRegisterInfo &MRI, MachineIRBuilder &B,
                                  AIEConcatUnmergeCombineMatchData &MatchInfo,
                                  GISelChangeObserver &Observer) {
  /// Set Insertion Point.
  if (MatchInfo.NewConcatMBB->empty())
    B.setMBB(*MatchInfo.NewConcatMBB);
  else
    B.setInstr(MatchInfo.NewConcatMBB->instr_back());

  // Create new Concat Instruction.
  const auto TargetVecType = MRI.getType(*MatchInfo.UnmergeSourceReg);
  Register ConcatReg = MRI.createGenericVirtualRegister(TargetVecType);
  auto NewConcat = B.buildMergeLikeInstr(ConcatReg, MatchInfo.ConcatSubVecs);
  LLVM_DEBUG(dbgs() << "In bb." << MatchInfo.NewConcatMBB->getNumber()
                    << " created " << *NewConcat.getInstr());

  // Set Insertion Point to top of phi-MBB.
  B.setInsertPt(*ConcatI.getParent(), ConcatI.getParent()->begin());

  // Create new PHI node.
  auto NewPHI = B.buildInstr(TargetOpcode::G_PHI);
  NewPHI.addDef(ConcatI.getOperand(0).getReg());

  // Add first PHI operand (newly create G_CONCAT).
  NewPHI.addUse(NewConcat.getInstr()->getOperand(0).getReg());
  NewPHI->addOperand(MachineOperand::CreateMBB(MatchInfo.NewConcatMBB));

  // Add second PHI operand (unmerge Components).
  NewPHI.addUse(*MatchInfo.UnmergeSourceReg);
  MachineBasicBlock *UnmergeMBB =
      MRI.getVRegDef(*MatchInfo.UnmergeSourceReg)->getParent();
  NewPHI->addOperand(MachineOperand::CreateMBB(UnmergeMBB));
  LLVM_DEBUG(dbgs() << "Created New Instruction " << *NewPHI.getInstr());
  ConcatI.removeFromParent();
}

bool llvm::matchGlobalPtrModOptimizer(MachineInstr &MemI,
                                      MachineRegisterInfo &MRI,
                                      CombinerHelper &Helper,
                                      const TargetInstrInfo &TII,
                                      AIE::FoundCombiners *GlobalCombinerPtr) {

  AIE::Combiner *CombineRule = GlobalCombinerPtr->getCombine(&MemI);
  if (!CombineRule) {
    LLVM_DEBUG(dbgs() << "[Global Ptr Inc] Could not find Combine for "
                      << MemI);
    return false;
  }
  assert(CombineRule->CombineInstrs.size() >= 2);
  LLVM_DEBUG(dbgs() << "[Global Ptr Inc] Found\n" << *CombineRule);

  return true;
}

bool llvm::matchLdStInc(MachineInstr &MemI, MachineRegisterInfo &MRI,
                        CombinerHelper &Helper, const TargetInstrInfo &TII,
                        AIE::FoundCombiners *GlobalCombinerPtr) {
  const AIEBaseInstrInfo &AIETII = (const AIEBaseInstrInfo &)TII;

  if (GlobalCombinerPtr->hasAnalysis())
    return false;

  return findPostIncMatch(MemI, MRI, Helper, AIETII, GlobalCombinerPtr) ||
         findPreIncMatch(MemI, MRI, Helper, AIETII, GlobalCombinerPtr);
}

void llvm::applyLdStInc(MachineInstr &MemI, MachineRegisterInfo &MRI,
                        CombinerHelper &Helper, MachineIRBuilder &B,
                        GISelChangeObserver &Observer,
                        AIE::FoundCombiners *GlobalCombinerPtr) {

  AIE::Combiner *CombineResult = GlobalCombinerPtr->getCombine(&MemI);
  assert(CombineResult);

  LLVM_DEBUG(dbgs() << "Applying Combiner "; CombineResult->dumpFull());

  MachineInstr *CombinedInsertionPoint = CombineResult->InsertionPoint;
  unsigned CombinedInstrOpcode = CombineResult->CombinedInstrOpcode;
  assert(CombinedInstrOpcode != (unsigned)-1 && "Invalid OpCode");

  if (CombinedInsertionPoint) {
    B.setInstr(*CombinedInsertionPoint);
  } else {
    B.setMBB(*MemI.getParent());
  }

  // Init combiner and get variables
  MachineInstr *PtrMod = CombineResult->CombineInstrs[0];
  bool RemovePtrMod = CombineResult->RemoveInstrs.any();

  // Debug Loc: Debug Loc of LOAD STORE: MI
  B.setDebugLoc(MemI.getDebugLoc());
  auto NewInstr = B.buildInstr(CombinedInstrOpcode);

  // move Instr right before the InsertionPoint
  for (auto *Instr : CombineResult->MoveUpInstrsToInsertionPoint) {
    if (!Instr->getParent())
      // Instr does not exist anymore, no need to move it
      continue;

    if (Helper.dominates(*Instr, *NewInstr))
      continue;

    Instr->moveBefore(NewInstr);
    LLVM_DEBUG(dbgs() << "Move Instr before " << *Instr);
  }

  // Move Instr past the InsertionPoint
  if (CombinedInsertionPoint) {
    for (auto *Instr : CombineResult->DelayInstrPastInsertionPoint) {
      if (!Instr->getParent())
        // Instruction may not exist anymore, i.e. a ptr_add that was combined
        // to a post increment Instruction
        continue;

      if (Helper.dominates(*CombinedInsertionPoint, *Instr))
        continue;

      LLVM_DEBUG(dbgs() << "Delaying Instr " << *Instr);
      Instr->moveBefore(CombinedInsertionPoint);
    }
  }

  if (MemI.mayLoad())
    NewInstr.addDef(MemI.getOperand(0).getReg() /* Loaded value */);
  if (RemovePtrMod)
    // If we remove the instr it is because we have defs that would otherwise
    // be redefined. We have to add these defs into the new instruction.
    for (auto Def : PtrMod->defs())
      if (Def.isReg())
        NewInstr.addDef(Def.getReg());
  if (MemI.getOpcode() == TargetOpcode::G_STORE)
    NewInstr.addUse(MemI.getOperand(0).getReg() /* Stored value */);
  for (auto Use : PtrMod->uses())
    if (Use.isReg())
      NewInstr.addUse(Use.getReg());
  for (auto *Mem : MemI.memoperands())
    NewInstr.addMemOperand(Mem);

  // keep track of Converted Instructions, so that delayInstructions are
  // properly keep track of
  GlobalCombinerPtr->createMapping(&MemI, NewInstr);

  LLVM_DEBUG(dbgs() << *NewInstr.getInstr());

  for (int Idx = CombineResult->RemoveInstrs.find_first(); Idx != -1;
       Idx = CombineResult->RemoveInstrs.find_next(Idx)) {
    auto *RemoveMI = CombineResult->CombineInstrs[Idx];

    // Removed Instructions have to be remapped to the newly Inserted
    // Instructions, so that they are considered when the Removed Instruction
    // should be moved up/down
    GlobalCombinerPtr->createMapping(RemoveMI, NewInstr);

    LLVM_DEBUG(dbgs() << "  Removing " << *RemoveMI);
    assert(RemoveMI->getParent() &&
           "RemoveMI was already deleted. This Combiner may have a conflict "
           "with the Combiner that already removed the MachineInstr.");
    RemoveMI->removeFromParent();
  }
  MemI.removeFromParent();
}

// Match all equivalents of these:
// %0:_(<16 x s32>) = G_IMPLICIT_DEF
// %1:_(s32) = G_IMPLICIT_DEF
// %2:_(<16 x s32>) =  G_AIE_ADD_VECTOR_ELT_HI %0, %1(s32)
//
// Combine into:
// %0:_(<16 x s32>) = G_IMPLICIT_DEF
// %2:_(<16 x s32>) = COPY %0
bool llvm::matchAddVecEltUndef(MachineInstr &MI, MachineRegisterInfo &MRI,
                               const TargetInstrInfo &TII) {
  const AIEBaseInstrInfo &AIETII = (const AIEBaseInstrInfo &)TII;
  assert(MI.getOpcode() == AIETII.getGenericAddVectorEltOpcode() &&
         "Expected a G_AIE_ADD_VECTOR_ELT_HI");
  const MachineInstr *SrcVecDef =
      getDefIgnoringCopies(MI.getOperand(1).getReg(), MRI);
  const MachineInstr *SrcEltDef =
      getDefIgnoringCopies(MI.getOperand(2).getReg(), MRI);

  if (SrcVecDef->getOpcode() != TargetOpcode::G_IMPLICIT_DEF ||
      SrcEltDef->getOpcode() != TargetOpcode::G_IMPLICIT_DEF)
    return false;

  return true;
}

void llvm::applyAddVecEltUndef(MachineInstr &MI, MachineRegisterInfo &MRI,
                               MachineIRBuilder &B) {
  B.setDebugLoc(MI.getDebugLoc());
  B.buildCopy(MI.getOperand(0), MI.getOperand(1));
  MI.removeFromParent();
}

// Return the base offset, base offset is decided based on the
// maximum number of distances which are simm3 three bit signed
// for part-word and/or imm6x4 6-bits scaled by 4 for full word.
// Count is the number of distances which fall in the range of
// simm3 and imm6x4. The heuristic considers the base offset optimal
// if Count is maximum for the base offset.
static uint64_t findOptimalOffset(
    std::set<std::pair<uint64_t, uint64_t>> &OffsetElemSizePairSet) {
  uint64_t OptOffset = 0;
  uint64_t MaxCount = 0;
  for (auto const &i : OffsetElemSizePairSet) {
    uint64_t Count = 0;
    for (auto const &j : OffsetElemSizePairSet) {
      int64_t Imm = j.first - i.first;
      // Offset is simm3 for part word?
      if (j.second <= 2 && isInt<3>(Imm)) {
        Count++;
        continue;
      }
      // Offset is imm6x4 ?
      if (j.second >= 4 && (isInt<6 + 2>(Imm) && (Imm % 4) == 0))
        Count++;
    }
    // Choose the first base offset having MaxCount.
    // TODO: Can we choose a base offset having maximal count
    // closer to part word?
    if (Count > MaxCount) {
      MaxCount = Count;
      OptOffset = i.first;
    }
  }
  return OptOffset;
}

/// \returns true if it is possible to fold a below sequence of MIRs
/// into a G_GLOBAL_VALUE.
/// From : %1 = G_GLOBAL_VALUE @a
///        %2 = G_CONSTANT off
///        %3 = G_PTR_ADD %1, %2
/// TO :   G_GLOBAL_VALUE @a + off

bool llvm::matchGlobalValOffset(MachineInstr &MI, MachineRegisterInfo &MRI,
                                uint64_t &MatchInfo) {

  assert(MI.getOpcode() == TargetOpcode::G_GLOBAL_VALUE);
  MachineFunction &MF = *MI.getMF();
  auto &GlobalOp = MI.getOperand(1);
  auto *GV = GlobalOp.getGlobal();
  if (GV->isThreadLocal())
    return false;
  // transform %g = G_GLOBAL_VALUE @a, for any other case return false
  if (GlobalOp.getOffset() != 0)
    return false;
  unsigned OpFlags = MF.getSubtarget<AIE2Subtarget>().classifyGlobalReference(
      GV, MF.getTarget());
  if (OpFlags != AIEII::MO_None)
    return false;

  // Look for a G_GLOBAL_VALUE only used by G_PTR_ADDs against offsets:
  //
  //  %g = G_GLOBAL_VALUE @a
  //  %ptr1 = G_PTR_ADD %g, off1
  //  %ptr2 = G_PTR_ADD %g, off2
  //  ...
  //  %ptrN = G_PTR_ADD %g, offN
  //
  // Identify the Offset. => optimal_offset(off1, off2,..,offN)
  // apply and transform to  [using applyGlobalValOffset]
  // %off = G_GLOBAL_VALUE @a + Offset
  // %g = G_PTR_ADD %off, -Offset
  // %ptr1 = G_PTR_ADD %g, off1
  //
  Register Dst = MI.getOperand(0).getReg();
  uint64_t Offset = 0ull;
  // set of access element size and offset pair
  std::set<std::pair<uint64_t, uint64_t>> OffsetElemSizePairSet;
  for (auto &UseInstr : MRI.use_nodbg_instructions(Dst)) {
    GLoadStore *LdStMI = dyn_cast<GLoadStore>(&UseInstr);
    if ((UseInstr.getOpcode() != TargetOpcode::G_PTR_ADD) && !LdStMI)
      return false;
    if (LdStMI) {
      MachineMemOperand *MMO = *UseInstr.memoperands_begin();
      OffsetElemSizePairSet.insert(
          std::make_pair(Offset, MMO->getSize().getValue()));
    } else {
      auto Cst = getIConstantVRegValWithLookThrough(
          UseInstr.getOperand(2).getReg(), MRI);
      if (!Cst)
        return false;
      APInt Val = Cst->Value;
      assert(Val.isNonNegative() && "Expected a NonNegative Constant Offset");
      const auto UseInstIter =
          MRI.use_nodbg_instructions(UseInstr.getOperand(0).getReg());
      for (auto &Use : UseInstIter) {
        if (dyn_cast<GLoadStore>(&Use)) {
          MachineMemOperand *MMO = *Use.memoperands_begin();
          OffsetElemSizePairSet.insert(
              std::make_pair(Val.getZExtValue(), MMO->getSize().getValue()));
        }
      }
    }
  }
  Offset = findOptimalOffset(OffsetElemSizePairSet);
  if (Offset <= 0)
    return false;

  // Offset should not be greater than 20-bit
  if (Offset >= (1ull << 19))
    return false;

  Type *T = GV->getValueType();
  // Return false if it does not makes sense to take the size of this type
  // or Offset is greater than the size allocated for the global object.
  if (!T->isSized() ||
      Offset > GV->getParent()->getDataLayout().getTypeAllocSize(T))
    return false;
  MatchInfo = Offset;
  return true;
}

void llvm::applyGlobalValOffset(MachineInstr &MI, MachineRegisterInfo &MRI,
                                MachineIRBuilder &B,
                                GISelChangeObserver &Observer,
                                uint64_t &MatchInfo) {
  uint64_t Offset = MatchInfo;
  B.setInsertPt(*MI.getParent(), ++MI.getIterator());
  Observer.changingInstr(MI);
  auto &GlobalOp = MI.getOperand(1);
  auto *GV = GlobalOp.getGlobal();
  unsigned Flags = GlobalOp.getTargetFlags();
  GlobalOp.ChangeToGA(GV, Offset, Flags | AIEII::MO_GLOBAL);
  Register Dst = MI.getOperand(0).getReg();
  Register NewGVDst = MRI.cloneVirtualRegister(Dst);
  MI.getOperand(0).setReg(NewGVDst);
  Observer.changedInstr(MI);
  //
  //  %off = G_GLOBAL_VALUE @a + Offset
  //  %g = G_PTR_ADD %off, -Offset
  //  %ptr1 = G_PTR_ADD %g, off1
  //  ...
  B.buildPtrAdd(
      Dst, NewGVDst,
      B.buildConstant(LLT::scalar(20), -static_cast<int64_t>(Offset)));
}

bool llvm::matchExtractVecEltAndExt(
    MachineInstr &MI, MachineRegisterInfo &MRI,
    std::pair<MachineInstr *, bool> &MatchInfo) {

  assert(MI.getOpcode() == TargetOpcode::G_EXTRACT_VECTOR_ELT &&
         "Expected a extract_vector_elt");
  Register DstReg = MI.getOperand(0).getReg();
  const LLT S8 = LLT::scalar(8);
  const LLT S16 = LLT::scalar(16);
  LLT SrcVecTy = MRI.getType(MI.getOperand(1).getReg());
  // Extracts from vectors <= 64-bits are lowered to bit-arithmetic in
  // legalization
  if (SrcVecTy.getSizeInBits() <= 64)
    return false;
  LLT SrcEltTy = SrcVecTy.getElementType();
  if (SrcEltTy != S8 && SrcEltTy != S16)
    return false;
  if (!MRI.hasOneNonDBGUse(DstReg))
    return false;
  MachineInstr *ExtMI = &*MRI.use_instr_nodbg_begin(DstReg);
  switch (ExtMI->getOpcode()) {
  case TargetOpcode::G_ANYEXT:
  case TargetOpcode::G_SEXT:
    MatchInfo = std::make_pair(ExtMI, 1);
    return true;
  case TargetOpcode::G_ZEXT:
    MatchInfo = std::make_pair(ExtMI, 0);
    return true;
  default:
    return false;
  }
  return false;
}

void llvm::applyExtractVecEltAndExt(
    MachineInstr &MI, MachineRegisterInfo &MRI, MachineIRBuilder &B,
    std::pair<MachineInstr *, bool> &MatchInfo) {
  B.setInstrAndDebugLoc(MI);
  auto [MatchMI, IsSignedExt] = MatchInfo;
  const Register ExtractDstReg = MI.getOperand(0).getReg();
  const LLT ExtractDstTy = MRI.getType(ExtractDstReg);
  const Register ExtendDstReg = MatchMI->getOperand(0).getReg();
  const LLT ExtendDstTy = MRI.getType(ExtendDstReg);
  const Register SrcReg0 = MI.getOperand(1).getReg();
  const Register SrcReg1 = MI.getOperand(2).getReg();
  const LLT S32 = LLT::scalar(32);
  const AIEBaseInstrInfo &AIETII = (const AIEBaseInstrInfo &)B.getTII();
  const unsigned Opcode =
      AIETII.getGenericExtractVectorEltOpcode(/*sign ext*/ IsSignedExt);
  const Register ExtractElt32BitDst = MRI.createGenericVirtualRegister(S32);
  B.buildInstr(Opcode, {ExtractElt32BitDst}, {SrcReg0, SrcReg1});

  const unsigned AssertOpcode =
      IsSignedExt ? TargetOpcode::G_ASSERT_SEXT : TargetOpcode::G_ASSERT_ZEXT;
  if (ExtendDstTy == LLT::scalar(32)) {
    B.buildAssertInstr(AssertOpcode, ExtendDstReg, ExtractElt32BitDst,
                       ExtractDstTy.getSizeInBits());
  } else {
    const Register Assert32BitDst = MRI.createGenericVirtualRegister(S32);
    B.buildAssertInstr(AssertOpcode, Assert32BitDst, ExtractElt32BitDst,
                       ExtractDstTy.getSizeInBits());
    B.buildExtOrTrunc(MatchMI->getOpcode(), ExtendDstReg, Assert32BitDst);
  }

  MI.eraseFromParent();
  MatchMI->eraseFromParent();
}

static std::optional<Register>
getSplatVectorSrcReg(const MachineInstr &MI, const MachineRegisterInfo &MRI,
                     std::pair<unsigned, unsigned> Range) {
  auto IsUndef = [&](const MachineOperand &Op) {
    const MachineInstr *Undef = MRI.getVRegDef(Op.getReg());
    return Undef && Undef->getOpcode() == TargetOpcode::G_IMPLICIT_DEF;
  };
  const unsigned Start = Range.first;
  const unsigned End = Range.second;
  // First non-undef operand.
  Register SrcReg = 0;
  bool FoundSrc = false;
  bool AllUndef = true;

  // Find the first non-undef operand as the reference.
  for (unsigned I = Start; I < End; I++) {
    const MachineOperand &Op = MI.getOperand(I);
    if (!IsUndef(Op)) {
      if (!FoundSrc) {
        SrcReg = Op.getReg();
        FoundSrc = true;
      } else if (Op.getReg() != SrcReg) {
        return std::nullopt;
      }
      AllUndef = false;
    }
  }

  if (AllUndef)
    SrcReg = MI.getOperand(1).getReg();

  return SrcReg;
}

// Match something like:
// %0:_(<32 x s16>) = G_BUILD_VECTOR %1:_(s16), ... x32
//
// To turn it into
// %0:_(<32 x s16>) = G_AIE_BROADCAST_VECTOR %1:_(s16)
bool llvm::matchSplatVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                            std::pair<Register, Register> &MatchInfo) {

  assert(MI.getOpcode() == TargetOpcode::G_BUILD_VECTOR &&
         "Expected a G_BUILD_VECTOR");

  const Register DstVecReg = MI.getOperand(0).getReg();
  const LLT DstVecTy = MRI.getType(DstVecReg);
  const unsigned DstVecSize = DstVecTy.getSizeInBits();

  switch (DstVecSize) {
  case 128:
  case 256:
  case 512:
  case 1024:
  case 2048:
    break;
  default:
    // unimplemented.
    return false;
  }

  const unsigned NumOps = MI.getNumOperands();
  auto SrcReg = getSplatVectorSrcReg(MI, MRI, std::make_pair(1, NumOps));
  if (!SrcReg)
    return false;

  MatchInfo = {DstVecReg, *SrcReg};
  return true;
}

static void buildUnmergeVector(MachineIRBuilder &B, MachineRegisterInfo &MRI,
                               Register DstReg, Register SrcReg,
                               unsigned NumSubVectors, unsigned SubIdx) {
  const LLT DstTy = MRI.getType(DstReg);
  SmallVector<Register, 4> SubVecs;
  for (unsigned I = 0; I < NumSubVectors; I++) {
    if (I == SubIdx)
      SubVecs.push_back(DstReg);
    else
      SubVecs.push_back(MRI.createGenericVirtualRegister(DstTy));
  }
  B.buildUnmerge(SubVecs, SrcReg);
}

static void buildBroadcastVector(MachineIRBuilder &B, MachineRegisterInfo &MRI,
                                 Register SrcReg, Register DstVecReg) {
  const AIEBaseInstrInfo &AIETII = (const AIEBaseInstrInfo &)B.getTII();
  const LLT SrcTy = MRI.getType(SrcReg);
  const LLT DstVecTy = MRI.getType(DstVecReg);
  const unsigned DstVecSize = DstVecTy.getSizeInBits();

  auto IsConstantZeroReg = [&](const Register Reg) {
    auto Cst = getAnyConstantVRegValWithLookThrough(Reg, MRI);
    return Cst && Cst->Value.isZero();
  };
  // Check if the source is constant zero and build a 2048-bit
  // vector destination.
  auto isConstantZero = IsConstantZeroReg(SrcReg) && DstVecSize == 2048;
  if (SrcTy == LLT::scalar(8) || SrcTy == LLT::scalar(16)) {
    const LLT S32 = LLT::scalar(32);
    Register Src32BitReg = MRI.createGenericVirtualRegister(S32);
    B.buildAnyExt(Src32BitReg, SrcReg);
    SrcReg = Src32BitReg;
  }
  if (SrcTy == LLT::scalar(64)) {
    const LLT V2S32 = LLT::fixed_vector(2, 32);
    Register Src64BitReg = MRI.createGenericVirtualRegister(V2S32);
    B.buildBitcast(Src64BitReg, SrcReg);
    SrcReg = Src64BitReg;
  }
  // Check if the destination vector size is 512 bits or if the destination
  // vector size is 2048 bits and the sources are constant zero.
  if (DstVecSize == 512 || isConstantZero) {
    // Build the G_AIE_BROADCAST_VECTOR instruction for a 512-bit vector.
    B.buildInstr(AIETII.getGenericBroadcastVectorOpcode(), {DstVecReg},
                 {SrcReg});
  } else {
    const unsigned DstElmtSize = DstVecTy.getElementType().getSizeInBits();
    const unsigned DstVec512BitLen = 512 / DstElmtSize;

    // Create a 512-bit generic virtual register for the destination vector
    // as 256-bit and 1024-bit broadcast support is not available.
    Register DstVec512BitReg = MRI.createGenericVirtualRegister(
        LLT::fixed_vector(DstVec512BitLen, DstElmtSize));

    // Build the G_AIE_BROADCAST_VECTOR instruction for the 512-bit vector.
    B.buildInstr(AIETII.getGenericBroadcastVectorOpcode(), {DstVec512BitReg},
                 {SrcReg});
    if (DstVecSize == 128 || DstVecSize == 256) {
      const unsigned NumSubVectors = 512 / DstVecSize;
      // Unmerge the 512-bit vector into the 128/256-bit destination vector.
      buildUnmergeVector(B, MRI, DstVecReg, DstVec512BitReg, NumSubVectors, 0);
    } else if (DstVecSize == 1024) {
      // Concatenate two 512-bit vectors to form a 1024-bit destination vector.
      B.buildConcatVectors({DstVecReg}, {DstVec512BitReg, DstVec512BitReg});
    } else if (DstVecSize == 2048) {
      // Concatenate 4 512-bit vectors to form a 2048-bit destination vector.
      B.buildConcatVectors({DstVecReg}, {DstVec512BitReg, DstVec512BitReg,
                                         DstVec512BitReg, DstVec512BitReg});
    }
  }
}

bool llvm::applySplatVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                            MachineIRBuilder &B,
                            std::pair<Register, Register> &MatchInfo) {
  B.setInstrAndDebugLoc(MI);
  auto [DstVecReg, SrcReg] = MatchInfo;
  buildBroadcastVector(B, MRI, SrcReg, DstVecReg);
  MI.eraseFromParent();
  return true;
}

// Match something like:
// %0:_(<32 x s16>) = G_BUILD_VECTOR %2:(s16), %2:(s16), %1:(s16) ... x32
//
// To turn it into
// %3:_(<32 x s16>) = G_AIE_BROADCAST_VECTOR %2:_(s16)
// %0:(<32 x s16>) = G_AIE_INSERT_VECTOR_ELT %3:(<32 x s16>), %1:_(s16), 2
bool llvm::matchSingleDiffLaneBuildVector(
    MachineInstr &MI, MachineRegisterInfo &MRI,
    AIESingleDiffLaneBuildVectorMatchData &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_BUILD_VECTOR &&
         "Expected a G_BUILD_VECTOR");

  const Register DstVecReg = MI.getOperand(0).getReg();
  const LLT DstVecTy = MRI.getType(DstVecReg);
  const unsigned DstVecSize = DstVecTy.getSizeInBits();

  switch (DstVecSize) {
  case 128:
  case 256:
  case 512:
  case 1024:
  case 2048:
    break;
  default:
    // unimplemented
    return false;
  }
  // DenseMap to hold unique registers and their (count, last index)
  DenseMap<Register, std::pair<unsigned, unsigned>> UniqueRegs;
  const unsigned NumOps = MI.getNumOperands();
  for (unsigned i = 1; i < NumOps; i++) {
    const Register OpReg = MI.getOperand(i).getReg();
    auto &RegInfo = UniqueRegs[OpReg];
    RegInfo.first += 1;
    RegInfo.second = i - 1;

    if (UniqueRegs.size() > 2)
      return false;
  }
  // Ensure exactly 2 unique registers to match the single differing lane build
  // vector pattern. More than 2 registers won't match; 1 unique register would
  // be a splat vector combine
  if (UniqueRegs.size() != 2)
    return false;

  Register SplatReg, DifferingReg;
  unsigned DifferingIndex;

  // Identify splat (multiple uses) and differing (single use) registers
  for (const auto &[Reg, RegInfo] : UniqueRegs) {
    if (RegInfo.first == 1) {
      DifferingReg = Reg;
      DifferingIndex = RegInfo.second;
    } else {
      SplatReg = Reg;
    }
  }
  // Validate that one register was used exactly once
  if (!DifferingReg.isValid() || !SplatReg.isValid())
    return false;

  // Ignore G_IMPLICIT_DEF to avoid conflicts with \fn matchBroadcastElement
  const MachineInstr *SplatRegDef = getDefIgnoringCopies(SplatReg, MRI);
  if (!SplatRegDef || SplatRegDef->getOpcode() == TargetOpcode::G_IMPLICIT_DEF)
    return false;

  MatchInfo = {DstVecReg, SplatReg, DifferingReg, DifferingIndex};
  return true;
}

bool llvm::applySingleDiffLaneBuildVector(
    MachineInstr &MI, MachineRegisterInfo &MRI, MachineIRBuilder &B,
    AIESingleDiffLaneBuildVectorMatchData &MatchInfo) {
  B.setInstrAndDebugLoc(MI);
  const Register DstVecReg = MatchInfo.DstVecReg;
  const LLT DstVecRegTy = MRI.getType(DstVecReg);
  const Register BcstDstReg = MRI.createGenericVirtualRegister(DstVecRegTy);
  const LLT S32 = LLT::scalar(32);

  buildBroadcastVector(B, MRI, MatchInfo.SplatReg, BcstDstReg);
  const Register IdxReg =
      B.buildConstant(S32, MatchInfo.DifferingIndex).getReg(0);
  B.buildInsertVectorElement(DstVecReg, BcstDstReg, MatchInfo.DifferingReg,
                             IdxReg);
  MI.eraseFromParent();
  return true;
}

// Match something like:
// %0:_(<32 x s16>) = G_BUILD_VECTOR %1:_(s16), ... x16, %2:_(s16), ... x16
//
// To turn it into
// %3:_(<16 x s16>) = G_BUILD_VECTOR %1:_(s16), ... x16
// %4:_(<16 x s16>) = G_BUILD_VECTOR %2:_(s16), ... x16
// %0:_(<32 x s16>) = G_CONCAT_VECTORS %3:_(<16 x s16>), %4:_(<16 x s16>)
// These sub-G_BUILD_VECTOR instructions may later be combined into broadcast
// instructions by combine_splat_vector.
// TODO: Remove the original splat vector match and implement the same here.
bool llvm::matchSymmetricBuildVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                                     GISelChangeObserver &Observer,
                                     BuildFnTy &MatchInfo) {

  assert(MI.getOpcode() == TargetOpcode::G_BUILD_VECTOR &&
         "Expected a G_BUILD_VECTOR");
  const Register DstVecReg = MI.getOperand(0).getReg();
  const LLT DstVecTy = MRI.getType(DstVecReg);
  const unsigned DstVecSize = DstVecTy.getSizeInBits();

  switch (DstVecSize) {
  case 256:
  case 512:
  case 1024:
  case 2048:
    break;
  default:
    // unimplemented
    return false;
  }

  // TODO: Split the G_BUILD_VECTOR either into 3/4 and 1/4 parts,
  // or 1/4 and 3/4 parts, and then check if any part qualifies as a splat.
  const unsigned NumOps = MI.getNumOperands();
  const unsigned HalfNumElts = NumOps / 2 + 1;
  auto FirstHalfSrcReg =
      getSplatVectorSrcReg(MI, MRI, std::make_pair(1, HalfNumElts));
  auto SecondHalfSrcReg =
      getSplatVectorSrcReg(MI, MRI, std::make_pair(HalfNumElts, NumOps));

  MatchInfo = [&MI, &Observer, DstVecTy](MachineIRBuilder &B) {
    B.setInstrAndDebugLoc(MI);
    LegalizerHelper Helper(B.getMF(), Observer, B);
    // Splits the G_BUILD_VECTOR into two half-sized G_BUILD_VECTOR operations
    // and then emits a G_CONCAT_VECTORS to combine them into final vector.
    Helper.fewerElementsVector(
        MI, 0,
        DstVecTy.changeElementCount(
            DstVecTy.getElementCount().divideCoefficientBy(2)));
  };

  return (FirstHalfSrcReg.has_value() || SecondHalfSrcReg.has_value());
}

// Match something like:
// %0(<4 x s32>), dead %1(<4 x s32>), dead %2(<4 x s32>), dead %3(<4 x s32>)
//   = G_UNMERGE_VALUES %10(<16 x s32>)
//
// To turn it into
// %0(<4 x s32>) = G_AIE_UNPAD_VECTOR %10(<16 x s32>)
bool llvm::matchUnpadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                            const AIEBaseInstrInfo &TII) {
  assert(MI.getOpcode() == TargetOpcode::G_UNMERGE_VALUES &&
         "Expected an unmerge");

  // Check that the first lane is a 128-bit vector, and the others are dead.
  LLT UnmergedTy = MRI.getType(MI.getOperand(0).getReg());
  LLT InputTy = MRI.getType(MI.getOperand(MI.getNumDefs()).getReg());
  if (!TII.isLegalTypeToUnpad(InputTy) || !TII.isLegalTypeToPad(UnmergedTy)) {
    return false;
  }
  for (unsigned Idx = 1, EndIdx = MI.getNumDefs(); Idx != EndIdx; ++Idx) {
    if (!MRI.use_nodbg_empty(MI.getOperand(Idx).getReg()))
      return false;
  }
  return true;
}

void llvm::applyUnpadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                            MachineIRBuilder &B) {
  B.setInstrAndDebugLoc(MI);
  const AIEBaseInstrInfo &AIETII = (const AIEBaseInstrInfo &)B.getTII();
  Register DstReg = MI.getOperand(0).getReg();
  Register SrcReg = MI.getOperand(MI.getNumDefs()).getReg();
  B.buildInstr(AIETII.getGenericUnpadVectorOpcode(), {DstReg}, {SrcReg});
  MI.eraseFromParent();
}

// Match something like:
// %0:_(s32), %1:_(s32), %2:_(s32), %3:_(s32) = G_UNMERGE_VALUES %10(<4 x s32>)
// %4:_(s32) = G_IMPLICIT_DEF
// %11:_(<8 x s32>) = G_BUILD_VECTOR %0, %1, %2, %3, %4, %4, %4, %4
//
// To turn it into:
// %11:_(<8 x s32>) = G_AIE_PAD_VECTOR_UNDEF %10
bool llvm::matchPadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                          const AIEBaseInstrInfo &TII,
                          Register &MatchedInputVector) {
  assert(MI.getOpcode() == TargetOpcode::G_BUILD_VECTOR &&
         "Expected build vector");
  MachineInstr *Unmerge = MRI.getVRegDef(MI.getOperand(1).getReg());
  if (!Unmerge || Unmerge->getOpcode() != TargetOpcode::G_UNMERGE_VALUES)
    return false;

  // Look for G_UNMERGE_VALUES of a 128-bit register
  Register UnmergedReg = Unmerge->getOperand(Unmerge->getNumDefs()).getReg();
  LLT UnmergedInputTy = MRI.getType(UnmergedReg);
  LLT DstVectorTy = MRI.getType(MI.getOperand(0).getReg());
  if (!TII.isLegalTypeToPad(UnmergedInputTy) ||
      !TII.isLegalTypeToUnpad(DstVectorTy))
    return false;

  // Verify all the lanes of UnmergedReg are correctly copied into the
  // BUILD_VECTOR, and the remaining lanes are implicit defs.
  for (unsigned Idx = 1, EndIdx = MI.getNumExplicitOperands(); Idx != EndIdx;
       ++Idx) {
    unsigned LaneIdx = Idx - 1;
    Register LaneReg = MI.getOperand(Idx).getReg();
    if (LaneIdx < Unmerge->getNumDefs() &&
        LaneReg != Unmerge->getOperand(LaneIdx).getReg())
      return false;
    if (LaneIdx >= Unmerge->getNumDefs() &&
        MRI.getUniqueVRegDef(LaneReg)->getOpcode() !=
            TargetOpcode::G_IMPLICIT_DEF)
      return false;
  }

  MatchedInputVector = UnmergedReg;
  return true;
}

bool llvm::matchConcatPadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                                const AIEBaseInstrInfo &TII,
                                Register &MatchedInputVector) {
  assert(MI.getOpcode() == TargetOpcode::G_CONCAT_VECTORS &&
         "Expected concat vector");

  auto DstReg = MI.getOperand(0).getReg();
  LLT DstVectorTy = MRI.getType(DstReg);
  auto SrcReg1 = MI.getOperand(1).getReg();
  LLT SrcVector1Ty = MRI.getType(SrcReg1);
  if (!TII.isLegalTypeToPad(SrcVector1Ty) ||
      !TII.isLegalTypeToUnpad(DstVectorTy))
    return false;

  for (unsigned Index = 2; Index < MI.getNumOperands(); Index++) {
    if (!getOpcodeDef(TargetOpcode::G_IMPLICIT_DEF,
                      MI.getOperand(Index).getReg(), MRI))
      return false;
  }

  MatchedInputVector = SrcReg1;
  return true;
}

void llvm::applyPadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                          MachineIRBuilder &B, Register MatchedInputVector) {
  B.setInstrAndDebugLoc(MI);
  const AIEBaseInstrInfo &AIETII = (const AIEBaseInstrInfo &)B.getTII();
  Register DstReg = MI.getOperand(0).getReg();
  B.buildInstr(AIETII.getGenericPadVectorOpcode(), {DstReg},
               {MatchedInputVector});
  MI.eraseFromParent();
}

/// Match something like this:
///  %68:_(s32) = G_CONSTANT i32 0
///  %93:_(s32) = G_CONSTANT i32 1
///  %209:_(<16 x s64>) = G_INTRINSIC intrinsic(@llvm.aie2.concat.1024.512.acc),
///                   %206(<8 x s64>), %208(<8 x s64>)
///  %216:_(<8 x s64>) = G_INTRINSIC intrinsic(@llvm.aie2.ext.512.1024.acc),
///                   %209(<16 x s64>), %68(s32)
///  %219:_(<8 x s64>) = G_INTRINSIC intrinsic(@llvm.aie2.ext.512.1024.acc),
///                    %209(<16 x s64>), %93(s32)

/// To convert to:
///   %216:_(<8 x s64>) = COPY %206(<8 x s64>)
///   %219:_(<8 x s64>) = COPY %208(<8 x s64>)
bool llvm::matchExtractConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                              const AIEBaseInstrInfo &TII,
                              Register &MatchInfo) {

  std::optional<const AIEBaseInstrInfo::VExtractOpInfo> ExtractOp =
      TII.getVExtractOpInfo(MI);

  if (!ExtractOp)
    return false;

  auto Cst = getIConstantVRegValWithLookThrough(
      MI.getOperand(ExtractOp->SubVectorIndex).getReg(), MRI);

  if (!Cst)
    return false;

  const unsigned ExtractSize =
      MRI.getType(MI.getOperand(0).getReg()).getSizeInBits();

  MachineInstr &SrcMI = *MRI.getVRegDef(MI.getOperand(ExtractOp->Src).getReg());

  Register SrcReg;
  unsigned ConcatSize = 0;
  const unsigned Index = Cst->Value.getZExtValue();

  if (const auto ConcatOp = TII.getVConcatOpInfo(SrcMI)) {
    SrcReg = SrcMI.getOperand(Index + ConcatOp->FirstOperand).getReg();
    ConcatSize = MRI.getType(SrcReg).getSizeInBits();
  }

  MatchInfo = SrcReg;
  return ConcatSize == ExtractSize;
}

void llvm::applyExtractConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                              MachineIRBuilder &B, Register &MatchInfo) {
  B.setInstrAndDebugLoc(MI);
  Register DstReg = MI.getOperand(0).getReg();
  Register SrcReg = MatchInfo;

  B.buildCopy(DstReg, SrcReg);
  MI.eraseFromParent();
}

/// Match something like this:
/// %209:_(<16 x s32>) = G_INTRINSIC intrinsic(@llvm.aie2.concat.I512.I256),
///         %95(<8 x s32>), %98(<8 x s32>)
/// %252:_(<8 x s32>), %253:_(<8 x s32>) = G_UNMERGE_VALUES %209(<16 x s32>)
///
/// To convert to:
/// 252:_(<8 x s32>) = COPY %95(<8 x s32>)
/// 253:_(<8 x s32>) = COPY %98(<8 x s32>)
bool llvm::matchUnmergeConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                              const AIEBaseInstrInfo &TII,
                              std::pair<MachineInstr *, unsigned> &MatchInfo) {

  MachineInstr &SrcMI =
      *MRI.getVRegDef(MI.getOperand(MI.getNumOperands() - 1).getReg());

  std::optional<const AIEBaseInstrInfo::VConcatOpInfo> ConcatOp =
      TII.getVConcatOpInfo(SrcMI);

  if (!ConcatOp)
    return false;

  // We always have more operands for the intrinsic.
  if (MI.getNumOperands() !=
      SrcMI.getNumOperands() - ConcatOp->NumOfNonRegOperands)
    return false;

  MatchInfo = std::make_pair(&SrcMI, ConcatOp->FirstOperand);
  return true;
}

void llvm::applyUnmergeConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                              MachineIRBuilder &B,
                              std::pair<MachineInstr *, unsigned> &MatchInfo) {
  B.setInstrAndDebugLoc(MI);
  auto [ConcatMI, Offset] = MatchInfo;

  for (unsigned Op = 0; Op < MI.getNumOperands() - 1; Op++) {
    Register DstReg = MI.getOperand(Op).getReg();
    Register SrcReg = ConcatMI->getOperand(Op + Offset).getReg();
    B.buildCopy(DstReg, SrcReg);
  }

  MI.eraseFromParent();
}

/// This function tracks chain of vector updates using .upd vector intrinsic.
static std::map<unsigned, Register>
trackVectorUpdateChain(MachineInstr &MI, MachineRegisterInfo &MRI,
                       const AIEBaseInstrInfo &TII) {

  std::optional<const AIEBaseInstrInfo::VUpdateOpInfo> UpdateOp =
      TII.getVUpdateOpInfo(MI);

  if (!UpdateOp)
    return {};

  auto Cst = getIConstantVRegValWithLookThrough(
      MI.getOperand(UpdateOp->SubVectorIndex).getReg(), MRI);

  if (!Cst)
    return {};

  std::map<unsigned, Register> IndexRegMap = trackVectorUpdateChain(
      *MRI.getVRegDef(MI.getOperand(UpdateOp->Src).getReg()), MRI, TII);

  const unsigned Index = Cst->Value.getZExtValue();
  Register RegLane = MI.getOperand(UpdateOp->SrcSubVec).getReg();

  // Check if we already have this update in the chain.
  if (IndexRegMap.find(Index) != IndexRegMap.end())
    return {};

  IndexRegMap[Index] = RegLane;

  return IndexRegMap;
}

/// Match something like this:
///  %21:_(s32) = G_CONSTANT i32 0
///  %51:_(s32) = G_CONSTANT i32 1
///  %96:_(<8 x s32>) = G_BITCAST %95(<32 x s8>)
///  %97:_(<16 x s32>) = G_INTRINSIC intrinsic(@llvm.aie2.upd.I512.I256),
///           %9(<16 x s32>), %96(<8 x s32>), %21(s32)
///  %99:_(<8 x s32>) = G_BITCAST %98(<32 x s8>)
///  %100:_(<16 x s32>) = G_INTRINSIC intrinsic(@llvm.aie2.upd.I512.I256),
///           %97(<16 x s32>), %99(<8 x s32>), %51(s32)

/// To convert to:
///  %96:_(<16 x s32>) = G_CONCAT_VECTORS  %96(<8 x s32>), %99(<8 x s32>)
bool llvm::matchUpdToConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                            const AIEBaseInstrInfo &TII,
                            std::map<unsigned, Register> &IndexRegMap) {

  IndexRegMap = trackVectorUpdateChain(MI, MRI, TII);

  if (IndexRegMap.size() == 0)
    return false;

  const unsigned UpdSize =
      MRI.getType(MI.getOperand(0).getReg()).getSizeInBits();

  unsigned ConcatenatedSize = 0;
  for (auto IndexReg : IndexRegMap) {
    Register Reg = IndexReg.second;
    ConcatenatedSize += MRI.getType(Reg).getSizeInBits();
  }

  if (UpdSize != ConcatenatedSize)
    return false;

  return true;
}

/// Find a use of \p MI in the same block where it can be moved
MachineInstr &findClosestToUseInsertPoint(MachineInstr &MI,
                                          MachineRegisterInfo &MRI) {

  for (auto &User : MRI.use_instructions(MI.getOperand(0).getReg())) {
    if (User.isPHI())
      continue;
    if (User.getParent() == MI.getParent() && canDelayMemOp(MI, User, MRI))
      return User;
  }

  return MI;
}

void llvm::applyUpdToConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                            MachineIRBuilder &B,
                            std::map<unsigned, Register> &IndexRegMap) {
  B.setDebugLoc(MI.getDebugLoc());
  B.setInstr(findClosestToUseInsertPoint(MI, MRI));

  SmallVector<Register, 4> SrcRegs;
  for (unsigned Op = 0; Op < IndexRegMap.size(); Op++) {
    SrcRegs.push_back(IndexRegMap[Op]);
  }

  B.buildConcatVectors(MI.getOperand(0).getReg(), SrcRegs);

  MI.eraseFromParent();
}

bool llvm::matchLoadStoreSplit(GLoadStore &MI, MachineRegisterInfo &MRI,
                               const AIEBaseInstrInfo &TII,
                               unsigned &MaxMemSize) {

  const Register ValReg = MI.getReg(0);
  const LLT ValTy = MRI.getType(ValReg);
  const bool IsLoad = isa<GLoad>(MI);
  MaxMemSize = TII.getMaxLoadStoreSize();

  if (!TII.isProfitableToSplitType(ValTy))
    return false;

  /// Avoid splitting operations that can be combined `as is`.
  if (IsLoad) {
    for (MachineInstr &ConvInstr : MRI.use_instructions(ValReg)) {
      if (TII.canCombineWithLoadStore(ConvInstr))
        return false;
    }
  } else {
    MachineInstr &ConvInstr = *getDefIgnoringCopiesAndBitcasts(ValReg, MRI);
    if (TII.canCombineWithLoadStore(ConvInstr))
      return false;
  }

  return true;
}

void llvm::applyLoadStoreSplit(GLoadStore &MI, MachineRegisterInfo &MRI,
                               MachineIRBuilder &B, const unsigned MaxMemSize) {

  assert(MaxMemSize && "MaxMemSize should be specified!");
  B.setInstrAndDebugLoc(MI);
  MachineFunction &MF = B.getMF();
  const bool IsLoad = isa<GLoad>(MI);
  const Register ValReg = MI.getReg(0);
  const Register AddrReg = MI.getPointerReg();
  const LLT ValTy = MRI.getType(ValReg);
  const LLT PtrTy = MRI.getType(AddrReg);
  const LLT OffsetTy = LLT::scalar(PtrTy.getSizeInBits());
  const unsigned NumParts = ValTy.getSizeInBits() / MaxMemSize;
  const LLT NarrowTy = ValTy.divide(NumParts);
  const MachineMemOperand MMO = MI.getMMO();

  SmallVector<Register, 8> NarrowRegs;
  if (!IsLoad)
    extractParts(ValReg, NarrowTy, NumParts, NarrowRegs, B, MRI);

  for (int I = NumParts - 1; I >= 0; I--) {
    const unsigned ByteOffset = I * NarrowTy.getSizeInBytes();
    Register NewAddrReg;
    B.materializePtrAdd(NewAddrReg, AddrReg, OffsetTy, ByteOffset);
    MachineMemOperand *NewMMO =
        MF.getMachineMemOperand(&MMO, ByteOffset, NarrowTy);

    if (IsLoad) {
      Register Dst = MRI.createGenericVirtualRegister(NarrowTy);
      NarrowRegs.push_back(Dst);
      B.buildLoad(Dst, NewAddrReg, *NewMMO);
    } else {
      B.buildStore(NarrowRegs[I], NewAddrReg, *NewMMO);
    }
  }

  if (IsLoad) {
    std::reverse(NarrowRegs.begin(), NarrowRegs.end());
    B.buildConcatVectors(ValReg, NarrowRegs);
  }

  MI.eraseFromParent();
}

/// Match something like this:
///  %293:_(s20) = G_CONSTANT i20 32
///  %67:_(s20) = G_CONSTANT i20 64
///  %68:_(p0) = nuw G_PTR_ADD %61, %67(s20)
///  %295:_(<16 x s16>) = G_AIE_OFFSET_LOAD %68(p0), %293(s20)

/// To convert to:
///  %298:_(s20) = G_CONSTANT i20 96
///  %295:_(<16 x s16>) = G_AIE_OFFSET_LOAD %61(p0), %298(s20)
bool llvm::matchOffsetLoadStorePtrAdd(MachineInstr &MI,
                                      MachineRegisterInfo &MRI,
                                      const AIEBaseInstrInfo &TII,
                                      std::pair<Register, int64_t> &RegOffset) {

  const Register AddrReg = MI.getOperand(1).getReg();

  const auto CstOffsetLoadStore =
      getIConstantVRegValWithLookThrough(MI.getOperand(2).getReg(), MRI);

  if (!CstOffsetLoadStore)
    return false;

  MachineInstr *DefAddrRegInstr = MRI.getVRegDef(AddrReg);

  if (DefAddrRegInstr->getOpcode() != TargetOpcode::G_PTR_ADD)
    return false;

  const auto CstDefAddrRegInstr = getIConstantVRegValWithLookThrough(
      DefAddrRegInstr->getOperand(2).getReg(), MRI);

  if (!CstDefAddrRegInstr)
    return false;

  RegOffset.first = DefAddrRegInstr->getOperand(1).getReg();
  RegOffset.second = CstDefAddrRegInstr->Value.getSExtValue() +
                     CstOffsetLoadStore->Value.getSExtValue();

  return true;
}

void llvm::applyOffsetLoadStorePtrAdd(
    MachineInstr &MI, MachineRegisterInfo &MRI, MachineIRBuilder &B,
    const std::pair<Register, int64_t> &RegOffset) {
  B.setInstrAndDebugLoc(MI);

  Register NewOffsetReg =
      B.buildConstant(LLT::scalar(20), RegOffset.second).getReg(0);

  MI.getOperand(1).setReg(RegOffset.first);
  MI.getOperand(2).setReg(NewOffsetReg);
}

/// Match something like this:
///  %0:_(s20) = COPY $m0
///  %1:_(p0) = COPY $p0
///  %2:_(<16 x s32>) = COPY $x0
///  %6:_(p0) = G_PTR_ADD %1, %0(s20)
///  %18:_(s20) = G_CONSTANT i20 32
///  G_AIE_OFFSET_STORE %15(<8 x s32>), %6(p0), %18(s20)
///  G_AIE_OFFSET_STORE %14(<8 x s32>), %1(p0), %0(s20)

/// To convert to (pointer reuse/CSE):
///  %0:_(s20) = COPY $m0
///  %1:_(p0) = COPY $p0
///  %2:_(<16 x s32>) = COPY $x0
///  %6:_(p0) = G_PTR_ADD %1, %0(s20)
///  %18:_(s20) = G_CONSTANT i20 32
///  %19:_(s20) = G_CONSTANT i20 0
///  G_AIE_OFFSET_STORE %15(<8 x s32>), %6(p0), %18(s20)
///  G_AIE_OFFSET_STORE %14(<8 x s32>), %6(p0), %19(s20)
bool llvm::matchOffsetLoadStoreSharePtrAdd(MachineInstr &MI,
                                           MachineRegisterInfo &MRI,
                                           CombinerHelper &Helper,
                                           const AIEBaseInstrInfo &TII,
                                           Register &PtrAddReg) {
  const Register PtrReg = MI.getOperand(1).getReg();
  const Register OffsetReg = MI.getOperand(2).getReg();

  const auto OffsetCst = getIConstantVRegValWithLookThrough(OffsetReg, MRI);

  // If we have a constant here, don't touch because it is better
  // to stay folded. Otherwise we will fold again in the previous
  // combiner.
  if (OffsetCst)
    return false;

  for (auto &Use : MRI.use_nodbg_instructions(PtrReg)) {
    if (Use.getOpcode() != TargetOpcode::G_PTR_ADD)
      continue;
    if (Use.getOperand(2).getReg() != OffsetReg)
      continue;
    if (Use.getParent() != MI.getParent())
      continue;
    if (!Helper.dominates(Use, MI))
      continue;

    Register PaddDestReg = Use.getOperand(0).getReg();

    // Dead instruction? Don't use it!
    // Ony use if at least another instruction is using it.
    if (hasNItemsOrMore(MRI.use_instr_nodbg_begin(PaddDestReg),
                        MRI.use_instr_nodbg_end(), 1)) {
      PtrAddReg = PaddDestReg;
      return true;
    }
  }

  return false;
}

void llvm::applyOffsetLoadStoreSharePtrAdd(MachineInstr &MI,
                                           MachineRegisterInfo &MRI,
                                           MachineIRBuilder &B,
                                           Register &PtrAddReg) {

  Register NewOffsetReg = B.buildConstant(LLT::scalar(20), 0).getReg(0);

  MI.getOperand(1).setReg(PtrAddReg);
  MI.getOperand(2).setReg(NewOffsetReg);
}

static bool isPowerOfTwoOrZero(unsigned Height) {
  return Height == 0 || (Height > 1 && has_single_bit(Height));
}

/// \returns true if it is possible to combine the below sequence of MIRs
/// into a COPY.
/// From : %1:_(<64 x s8>) = G_IMPLICIT_DEF
///        %2:_(<16 x s32>) = G_BITCAST %1:_(<64 x s8>)
///        %3:_(s32) = G_CONSTANT i32 0
///        %4:_(<16 x s32>) = G_INTRINSIC
///        intrinsic(@llvm.aie[2/2p].vshift.I512.I512), %X:_(<16 x s32>),
///        %2:_(<16 x s32>), %3:_(s32), %3:_(s32)
/// To :   4%:_(<16 x s32>) = COPY %X
/// Or even:
/// From : %1:_(<64 x s8>) = G_IMPLICIT_DEF
///        %2:_(s32) = G_CONSTANT i32 0
///        %3:_(<16 x s32>) = G_INTRINSIC
///        intrinsic(@llvm.aie[2/2p].vshift.I512.I512), %X:_(<16 x s32>),
///        %1:_(<16 x s32>), %2:_(s32), %2:_(s32)
/// To :   3%:_(<16 x s32>) = COPY %X
bool llvm::tryToCombineVectorShiftsByZero(MachineInstr &MI,
                                          MachineRegisterInfo &MRI) {

  const Register DstReg = MI.getOperand(0).getReg();
  const Register SrcReg = MI.getOperand(2).getReg();
  const Register ThirdSrcReg = MI.getOperand(4).getReg();
  const Register ShiftAmtSrcReg = MI.getOperand(5).getReg();

  auto IsConstantZeroReg = [&](const Register Reg) {
    auto Cst = getIConstantVRegValWithLookThrough(Reg, MRI);
    return Cst && Cst->Value.isZero();
  };

  if (!IsConstantZeroReg(ThirdSrcReg) || !IsConstantZeroReg(ShiftAmtSrcReg))
    return false;

  MachineIRBuilder MIRBuilder(MI);
  MIRBuilder.buildCopy(DstReg, SrcReg);
  MI.eraseFromParent();

  return true;
}

bool llvm::matchBroadcastElement(MachineInstr &MI, MachineRegisterInfo &MRI,
                                 std::pair<Register, Register> &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_SHUFFLE_VECTOR);
  const auto MaybeSplatIndex = getSplatIndex(MI);

  if (!MaybeSplatIndex.has_value())
    return false;

  const unsigned SrcNumElems =
      MRI.getType(MI.getOperand(1).getReg()).getNumElements();

  unsigned Idx;
  unsigned AdjustSrcElemIdx = 0;
  if (MaybeSplatIndex.value() < (int)SrcNumElems) {
    Idx = 1;
  } else {
    Idx = 2;
    AdjustSrcElemIdx = SrcNumElems;
  }

  const Register SrcVecReg = MI.getOperand(Idx).getReg();
  MachineInstr *SrcVec = MRI.getUniqueVRegDef(SrcVecReg);

  if (!SrcVec || SrcVec->getOpcode() != TargetOpcode::G_BUILD_VECTOR) {
    return false;
  }

  const Register DstReg = MI.getOperand(0).getReg();
  const unsigned SrcElemIdx = MaybeSplatIndex.value() + 1;
  const Register ElemReg =
      SrcVec->getOperand(SrcElemIdx - AdjustSrcElemIdx).getReg();
  MatchInfo = std::make_pair(DstReg, ElemReg);
  return true;
}

/// \returns true if it is possible to combine the shuffle vector to VSEL.
/// E.g.:
/// From :  %0:_(<16 x s32>) = COPY $x0
///         %1:_(<16 x s32>) = COPY $x1
///         %2:_(<16 x s32>) = G_SHUFFLE_VECTOR %X(<16 x s32>), %1(<16 x s32>),
///         shufflemask(0, 1, 2, 3, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
///         31)
/// To :    %3:_(s32) = G_CONSTANT i32 0xFFF0
///         %4:_(<16 x s32>) = G_AIE_VSEL %0, %1, %3(s32)
bool llvm::matchShuffleToVSel(MachineInstr &MI, MachineRegisterInfo &MRI,
                              const AIEBaseInstrInfo &TII,
                              BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_SHUFFLE_VECTOR);

  const unsigned BasicVectorBitSize = TII.getBasicVectorBitSize();
  const unsigned ScalarRegSize = TII.getScalarRegSize();
  const unsigned DoubleScalarRegSize = ScalarRegSize * 2;

  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  const Register Src2Reg = MI.getOperand(2).getReg();
  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  if (Src1Ty.getSizeInBits() != BasicVectorBitSize ||
      Src1Ty.getElementType().getSizeInBits() >= DoubleScalarRegSize)
    return false;

  const unsigned NumDstElems = DstTy.getNumElements();
  assert(NumDstElems == Mask.size());
  const unsigned NumSrcElems = Src1Ty.getNumElements();
  if (NumDstElems > NumSrcElems)
    return false;
  const unsigned NumSubVectors = NumSrcElems / NumDstElems;
  if ((NumSubVectors != 1 && NumSubVectors != 2) ||
      NumSrcElems % NumDstElems != 0) {
    return false;
  }

  // Someone should make undef out of this.
  if (MaskMatch::isMaskWithAllUndefs(Mask))
    return false;

  // Check that the shuffle mask can be converted into VSel condition vector:
  // Each element can select from the corresponding element from the first or
  // the second vector.
  // Hence, the mask value should either be don't care, equal to the index,
  // or equal to the index + NumSrcElems
  // This immediately defines the mask vector of the VSEL.
  uint64_t DstMask = 0;
  auto MatchVSEL = [&](unsigned Shift) {
    DstMask = 0;
    for (unsigned I = 0; I < NumDstElems; I++) {
      const int Idx = Mask[I];
      if (Idx == -1) {
        continue;
      }
      const int EffPos = I + Shift;
      if (Idx == EffPos)
        continue;

      if (Idx != EffPos + (int)NumSrcElems) {
        return false;
      }
      DstMask |= uint64_t(1) << I;
    }
    return true;
  };

  int SubIdx = 0;
  // A subvector can be matched with a shift corresponding to the subvector to
  // extract.
  auto MatchSubVector = [&]() {
    for (unsigned Shift = 0; Shift < NumSrcElems; Shift += NumDstElems) {
      if (MatchVSEL(Shift)) {
        return true;
      }
      SubIdx++;
    }
    return false;
  };

  if (!MatchSubVector()) {
    return false;
  }

  MatchInfo = [=, &MRI, &TII](MachineIRBuilder &B) {
    const unsigned ScalarSize =
        NumDstElems == DoubleScalarRegSize ? DoubleScalarRegSize : 32;
    MachineInstrBuilder MaskReg =
        B.buildConstant(LLT::scalar(ScalarSize), DstMask);
    const unsigned VSelOpc = TII.getGenericVSelOpcode();
    if (NumDstElems == NumSrcElems)
      B.buildInstr(VSelOpc, {DstReg}, {Src1Reg, Src2Reg, MaskReg});
    else { // NumDstElems < NumSrcElems
      const unsigned Src1ElemtSize = Src1Ty.getElementType().getSizeInBits();
      const unsigned Src1Vec512BitLen = BasicVectorBitSize / Src1ElemtSize;
      const LLT VSelDstTy = LLT::fixed_vector(Src1Vec512BitLen, Src1ElemtSize);
      const Register VSelDstReg = MRI.createGenericVirtualRegister(VSelDstTy);
      B.buildInstr(VSelOpc, {VSelDstReg}, {Src1Reg, Src2Reg, MaskReg});
      buildUnmergeVector(B, MRI, DstReg, VSelDstReg, NumSubVectors, SubIdx);
    }
  };
  return true;
}

/// \returns true if it is possible to combine the shuffle vector with a mask
/// that extracts an element from the first source vector and broadcasts
/// it. E.g.:
/// From :  %X:_(<4 x s64>) = COPY $wl0
///         %1:_(<4 x s64>) = COPY $wl1
///         %2:_(<8 x s64>) = G_SHUFFLE_VECTOR %X(<4 x s64>), %1(<4 x s64>),
///         shufflemask(3, 3, 3, 3, 3, 3, 3, 3)
/// To :    %3:_(s64) = G_EXTRACT_VECTOR_ELT %X, 3
///         %2:_(<8 x s64>) = G_AIE_BROADCAST_VECTOR %3(s64)
static bool matchShuffleToVecEltBroadcast(MachineInstr &MI,
                                          MachineRegisterInfo &MRI,
                                          const AIEBaseInstrInfo &TII,
                                          BuildFnTy &MatchInfo) {
  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  std::optional<int> UniqOpIdx = MaskMatch::getUniqueIndex(Mask);
  if (!UniqOpIdx)
    return false;

  assert(UniqOpIdx >= 0 && "Couldn't find a unique operand to extract!");

  MatchInfo = [=, &MI, &MRI](MachineIRBuilder &B) {
    const Register DstReg = MI.getOperand(0).getReg();
    const Register SrcVecReg = MI.getOperand(1).getReg();
    const LLT DstElemTy = MRI.getType(SrcVecReg).getElementType();
    auto Extr =
        B.buildExtractVectorElementConstant(DstElemTy, SrcVecReg, *UniqOpIdx);
    buildBroadcastVector(B, MRI, Extr.getReg(0), DstReg);
  };
  return true;
}

/// Check prerequisites to extract a subvector
static bool checkExtractSubvectorPrerequisites(const AIEBaseInstrInfo &TII,
                                               const LLT DstTy,
                                               const LLT SrcTy) {
  const unsigned ScalarRegSize = TII.getScalarRegSize();
  const unsigned VecRegSize = TII.getBasicVecRegSize();
  const unsigned SrcTySize = SrcTy.getSizeInBits();
  const unsigned DstTySize = DstTy.getSizeInBits();

  if (!DstTy.isVector() || !SrcTy.isVector() || SrcTySize < ScalarRegSize ||
      (DstTySize != ScalarRegSize && DstTySize != 2 * ScalarRegSize))
    return false;

  // Currently, we cannot extract vectors for the case when the size of the
  // source vector is less than the basic vector register size (of the target).
  if (SrcTySize < VecRegSize)
    return false;

  //  This should be handled by a separate combine that copies SrcReg to
  //  DstReg.
  return SrcTySize != DstTySize;
}

/// Build G_AIE_EXTRACT_SUBVECTOR
static MachineInstrBuilder
buildExtractSubvector(MachineIRBuilder &B, MachineRegisterInfo &MRI,
                      const AIEBaseInstrInfo &TII, Register DstVecReg,
                      Register SrcVecReg, unsigned SubIdx) {
  const unsigned ScalarRegSize = TII.getScalarRegSize();
  const unsigned BasicVectorBitSize = TII.getBasicVectorBitSize();

  const unsigned Opc = TII.getGenericExtractSubvectorOpcode();
  const LLT SrcTy = MRI.getType(SrcVecReg);
  const LLT DstTy = MRI.getType(DstVecReg);
  const unsigned SrcTySize = SrcTy.getSizeInBits();

  // Natively supported source vector type
  if (SrcTySize == BasicVectorBitSize) {
    auto Cst = B.buildConstant(LLT::scalar(ScalarRegSize), SubIdx);
    return B.buildInstr(Opc, {DstVecReg}, {SrcVecReg, Cst});
  }

  // Source vectors of a non-native size are converted to vectors of the native
  // size
  const unsigned Src1ElemtSize = SrcTy.getElementType().getSizeInBits();
  const unsigned Src1Vec512BitLen = BasicVectorBitSize / Src1ElemtSize;
  const LLT NewSrc1Ty = LLT::fixed_vector(Src1Vec512BitLen, Src1ElemtSize);
  const Register NewSrcReg = MRI.createGenericVirtualRegister(NewSrc1Ty);

  if (SrcTySize < BasicVectorBitSize) {
    const Register ImplicitDef = B.buildUndef(SrcTy).getReg(0);
    SmallVector<Register, 15> ConcatOps = {SrcVecReg};
    unsigned NumImplicitDef = BasicVectorBitSize / SrcTySize - 1;
    while (NumImplicitDef-- > 0) {
      ConcatOps.push_back(ImplicitDef);
    }
    B.buildConcatVectors({NewSrcReg}, ConcatOps);
    auto Cst = B.buildConstant(LLT::scalar(ScalarRegSize), SubIdx);
    return B.buildInstr(Opc, {DstVecReg}, {NewSrcReg, Cst});
  }

  // Source vectors with the size greater than the native source vector size
  const unsigned NumDstElems = DstTy.getNumElements();
  const unsigned NumSrc1Elems = SrcTy.getNumElements();
  const unsigned NumSubVectors = NumSrc1Elems / NumDstElems;
  const unsigned SizeCoefficient = SrcTySize / BasicVectorBitSize;
  const unsigned NumSubVectorsNativeSize = NumSubVectors / SizeCoefficient;
  unsigned NewSubIdx = SubIdx % NumSubVectorsNativeSize;

  SmallVector<Register, 4> SubRegs;
  unsigned NewSrcRegPosition = SubIdx / NumSubVectorsNativeSize;
  for (unsigned I = 0; I < SizeCoefficient; ++I) {
    if (I == NewSrcRegPosition)
      SubRegs.push_back(NewSrcReg);
    else
      SubRegs.push_back(MRI.createGenericVirtualRegister(NewSrc1Ty));
  }

  B.buildUnmerge(SubRegs, SrcVecReg);
  auto Cst = B.buildConstant(LLT::scalar(ScalarRegSize), NewSubIdx);
  return B.buildInstr(Opc, {DstVecReg}, {NewSrcReg, Cst});
}

/// Match something like this:
///  %1:_(<16 x s32>) = COPY $x0
///  %2:_(<16 x s32>) = COPY $x1
///  %0:_(<8 x s32>) = G_SHUFFLE_VECTOR %1(<16 x s32>), %2(<16 x s32>),
///  shufflemask(8, 9, 10, 11, 12, 13, 14, 15)
///  PseudoRET implicit $lr, implicit %0

/// To convert to:
/// %1:_(<16 x s32>) = COPY $x0
/// %2:_(<8 x s32>), %3:_(<8 x s32>) = G_UNMERGE_VALUES %1(<16 x s32>)
/// PseudoRET implicit $lr, implicit %3(<8 x s32>)
static bool matchShuffleToUnmerge(MachineInstr &MI, MachineRegisterInfo &MRI,
                                  BuildFnTy &MatchInfo, unsigned SubIdx,
                                  unsigned NumSubVectors) {
  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();

  // TODO: Select into G_EXTRACT_SUBVECTOR once it is more widely supported
  MatchInfo = [=, &MRI](MachineIRBuilder &B) {
    buildUnmergeVector(B, MRI, DstReg, Src1Reg, NumSubVectors, SubIdx);
  };
  return true;
}

/// Match something like this:
///  %1:_(<16 x s16>) = COPY $wl0
///  %2:_(<16 x s16>) = COPY $wl1
///  %0:_(<4 x s16>) = G_SHUFFLE_VECTOR %1(<16 x s16>), %2(<16 x s16>),
///  shufflemask(4, 5, 6, 7)

/// To convert to:
/// %1:_(<16 x s16>) = COPY $wl0
/// %2:_(s32) = G_CONSTANT i32 1
/// %3:_(<4 x s16>) = G_AIE_EXTRACT_SUBVECTOR %1(<16 x s16>), %2(s32)
/// NOTE: This combine works ONLY for 32- and 64-bit outputs!
static bool matchShuffleToAIEExtractSubvec(
    MachineInstr &MI, MachineRegisterInfo &MRI, const AIEBaseInstrInfo &TII,
    BuildFnTy &MatchInfo, unsigned SubIdx, unsigned NumSubVectors) {
  const unsigned GPRSize = TII.getScalarRegSize();
  const unsigned ExtractSubvecNativeSrcSize = TII.getBasicVectorBitSize();

  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  const unsigned Src1TySize = Src1Ty.getSizeInBits();

  if (!checkExtractSubvectorPrerequisites(TII, DstTy, Src1Ty))
    return false;

  const unsigned Opc = TII.getGenericExtractSubvectorOpcode();

  // Natively supported source vector type
  if (Src1TySize == ExtractSubvecNativeSrcSize) {
    MatchInfo = [=](MachineIRBuilder &B) {
      auto Cst = B.buildConstant(LLT::scalar(GPRSize), SubIdx);
      B.buildInstr(Opc, {DstReg}, {Src1Reg, Cst});
    };

    return true;
  }

  // Source vectors of a non-native size are converted to vectors of the native
  // size
  const unsigned Src1ElmtSize = Src1Ty.getElementType().getSizeInBits();
  const unsigned Src1Vec512BitLen = ExtractSubvecNativeSrcSize / Src1ElmtSize;
  const LLT NewSrc1Ty = LLT::fixed_vector(Src1Vec512BitLen, Src1ElmtSize);
  const Register NewSrcReg = MRI.createGenericVirtualRegister(NewSrc1Ty);

  if (Src1TySize < ExtractSubvecNativeSrcSize) {
    MatchInfo = [=](MachineIRBuilder &B) {
      const Register ImplicitDef = B.buildUndef(Src1Ty).getReg(0);
      SmallVector<Register, 15> ConcatOps = {Src1Reg};
      unsigned NumImplicitDef = ExtractSubvecNativeSrcSize / Src1TySize - 1;
      while (NumImplicitDef-- > 0) {
        ConcatOps.push_back(ImplicitDef);
      }
      B.buildConcatVectors({NewSrcReg}, ConcatOps);
      auto Cst = B.buildConstant(LLT::scalar(GPRSize), SubIdx);
      B.buildInstr(Opc, {DstReg}, {NewSrcReg, Cst});
    };
    return true;
  }

  // Source vectors with the size greater than the native source vector size
  MatchInfo = [=, &MRI](MachineIRBuilder &B) {
    const unsigned SizeCoefficient = Src1TySize / ExtractSubvecNativeSrcSize;
    const unsigned NumSubVectorsNativeSize = NumSubVectors / SizeCoefficient;
    unsigned NewSubIdx = SubIdx % NumSubVectorsNativeSize;

    SmallVector<Register, 4> SubRegs;
    unsigned NewSrcRegPosition = SubIdx / NumSubVectorsNativeSize;
    for (unsigned I = 0; I < SizeCoefficient; ++I) {
      if (I == NewSrcRegPosition)
        SubRegs.push_back(NewSrcReg);
      else
        SubRegs.push_back(MRI.createGenericVirtualRegister(NewSrc1Ty));
    }

    B.buildUnmerge(SubRegs, Src1Reg);
    auto Cst = B.buildConstant(LLT::scalar(GPRSize), NewSubIdx);
    B.buildInstr(Opc, {DstReg}, {NewSrcReg, Cst});
  };
  return true;
}

/// The method does some checks and calls matchShuffleToAIEExtractSubvec and
/// matchShuffleToUnmerge which extract subvectors is possible.
bool llvm::matchShuffleToExtractSubvec(MachineInstr &MI,
                                       MachineRegisterInfo &MRI,
                                       const AIEBaseInstrInfo &TII,
                                       BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_SHUFFLE_VECTOR);

  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  const unsigned Src1TySize = Src1Ty.getSizeInBits();

  if (!DstTy.isVector() || !Src1Ty.isVector())
    return false;

  //  This should be handled by a separate combine that copies Src1Reg to
  //  DstReg.
  if (Src1TySize == DstTy.getSizeInBits())
    return false;

  const unsigned NumDstElems = DstTy.getNumElements();
  const unsigned NumSrc1Elems = Src1Ty.getNumElements();

  // Not an extract pattern
  if (NumSrc1Elems <= NumDstElems)
    return false;

  // Unlikely to select into a subregister copy
  if (NumSrc1Elems % NumDstElems != 0)
    return false;

  if (MaskMatch::isMaskWithAllUndefs(Mask))
    return false;

  const unsigned NumSubVectors = NumSrc1Elems / NumDstElems;
  auto GetSubvecExtractIdx = [=, &Mask]() -> std::optional<unsigned> {
    for (unsigned SubVecIdx = 0; SubVecIdx < NumSubVectors; ++SubVecIdx) {
      MaskMatch SequentialMask{/*Height*/ SubVecIdx * NumDstElems};
      if (SequentialMask.isValidMask(Mask))
        return SubVecIdx;
    }

    return std::nullopt;
  };

  std::optional<unsigned> SubvecExtractIdx = GetSubvecExtractIdx();

  // Not an extract pattern
  if (!SubvecExtractIdx)
    return false;

  if (matchShuffleToAIEExtractSubvec(MI, MRI, TII, MatchInfo,
                                     SubvecExtractIdx.value(), NumSubVectors))
    return true;
  if (matchShuffleToUnmerge(MI, MRI, MatchInfo, SubvecExtractIdx.value(),
                            NumSubVectors))
    return true;

  return false;
}

/// Match something like this:
///  %1:_(<8 x s32>) = COPY $wl0
///  %2:_(<8 x s32>) = COPY $wl1
///  %0:_(<8 x s32>) = G_SHUFFLE_VECTOR %1(<8 x s32>), %2(<8 x s32>),
///  shufflemask(4, 5, 6, 7, 4, 5, 6, 7)

/// To convert to:
/// %1:_(<8 x s32>) = COPY $wl0
/// %2:_(s32) = G_CONSTANT i32 1
/// %3:_(<4 x s32>) = G_AIE_EXTRACT_SUBVECTOR %1(<8 x s32>), %2(s32)
/// %4:_(<16 x s32>) = G_AIE_BROADCAST_VECTOR %3(<4 x s32>)
/// %5:_(<8 x s32>) = G_AIE_UNPAD_VECTOR %4(<16 x s32>)

// If the subvector cannot be extracted and broadcasted given the target
// constraints, an Unmerge and Concat are used instead.
static bool matchShuffleToSubvecBroadcast(MachineInstr &MI,
                                          MachineRegisterInfo &MRI,
                                          const AIEBaseInstrInfo &TII,
                                          BuildFnTy &MatchInfo) {
  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);

  const unsigned NumDstElems = DstTy.getNumElements();
  const unsigned NumSrcElems = Src1Ty.getNumElements();
  const unsigned MaskSize = Mask.size();

  if (NumDstElems != MaskSize)
    return false;

  auto CheckSplatMask = [=]() -> std::optional<std::pair<int, unsigned>> {
    // Find the splat mask pattern, start with length 2 and then power of 2.
    for (unsigned SplatMaskLen = 2;
         SplatMaskLen <= NumSrcElems && SplatMaskLen <= MaskSize;
         SplatMaskLen *= 2) {
      if (Mask[0] != -1 && Mask[0] % SplatMaskLen != 0)
        return std::nullopt;

      // Get Height (start value)
      std::optional<unsigned> Height =
          MaskMatch::getHeight(Mask, /*Period*/ SplatMaskLen);
      if (!Height)
        return std::nullopt;

      // Check the mask
      MaskMatch SequentialPeriodicMask{/*Height*/ Height.value(),
                                       /*Period*/ SplatMaskLen};
      if (SequentialPeriodicMask.isValidMask(Mask))
        return std::make_pair(Height.value(), SplatMaskLen);
    }
    return std::nullopt;
  };

  auto SplatMaskData = CheckSplatMask();
  if (!SplatMaskData)
    return false;

  const int SplatMaskStart = SplatMaskData->first;
  const unsigned SplatMaskLen = SplatMaskData->second;

  const LLT ElemTy = Src1Ty.getElementType();
  const LLT DstSubvecType =
      LLT::fixed_vector(SplatMaskLen, ElemTy.getSizeInBits());
  const unsigned SubIdx = SplatMaskStart / SplatMaskLen;
  Register ExtractSubvecDstReg =
      MRI.createGenericVirtualRegister(DstSubvecType);

  // Check whether we can extract the subvector
  const bool CanExtractSubvector =
      checkExtractSubvectorPrerequisites(TII, DstSubvecType, Src1Ty);
  if (CanExtractSubvector) {
    MatchInfo = [=, &MRI, &TII](MachineIRBuilder &B) {
      auto Extract = buildExtractSubvector(B, MRI, TII, ExtractSubvecDstReg,
                                           Src1Reg, SubIdx);
      buildBroadcastVector(B, MRI, Extract.getReg(0), DstReg);
    };
    return true;
  }

  // If we cannot extract the subvector, we try to apply UNMERGE + CONCAT
  const unsigned NumSubVectors = NumSrcElems / SplatMaskLen;

  const unsigned SubVecSize = Src1Ty.getSizeInBits() / NumSubVectors;
  // FIXME: We don't have unmerge/concat support of 64-bit and smaller.
  if (SubVecSize < 128)
    return false;

  // Don't try to unmerge when we have just one subvector.
  // We can overcome with a copy, but other combiners can do a
  // better job for this case.
  if (NumSubVectors > 1 && NumDstElems > SplatMaskLen) {
    MatchInfo = [=, &MRI](MachineIRBuilder &B) {
      buildUnmergeVector(B, MRI, ExtractSubvecDstReg, Src1Reg, NumSubVectors,
                         SubIdx);

      const SmallVector<Register, 2> ConcatOps(NumDstElems / SplatMaskLen,
                                               ExtractSubvecDstReg);
      B.buildConcatVectors({DstReg}, ConcatOps);
    };
    return true;
  }

  return false;
}

/// Match something like this:
///  %1:_(<2 x s32>) = COPY $l0
///  %2:_(<2 x s32>) = G_IMPLICIT_DEF
///  %0:_(<16 x s32>) = G_SHUFFLE_VECTOR %1(<2 x s32>), %2,
///  shufflemask(0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1)

/// To convert to:
///  %1:_(<2 x s32>) = COPY $l0
///  %2:_(<2 x s32>) = G_IMPLICIT_DEF
///  %0:_(<16 x s32>) = G_AIE_BROADCAST_VECTOR %1(<2 x s32>)
static bool matchShuffleToVecBroadcast(MachineInstr &MI,
                                       MachineRegisterInfo &MRI,
                                       const AIEBaseInstrInfo &TII,
                                       BuildFnTy &MatchInfo) {
  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  if (Src1Ty.getSizeInBits() != 64 && Src1Ty.getSizeInBits() != 32) {
    return false;
  }
  const unsigned NumDstElems = DstTy.getNumElements();
  const unsigned NumSrcElems = Src1Ty.getNumElements();
  if (NumDstElems != Mask.size()) {
    return false;
  }

  // Check the mask
  MaskMatch SequentialPeriodicMask{/*Height*/ 0,
                                   /*Period*/ NumSrcElems};
  if (!SequentialPeriodicMask.isValidMask(Mask))
    return false;

  MatchInfo = [=, &MRI](MachineIRBuilder &B) {
    buildBroadcastVector(B, MRI, Src1Reg, DstReg);
  };

  return true;
}

// If the subvector cannot be extracted and broadcasted given the target
// constraints, an Unmerge and Concat are used instead, such as in
// matchShuffleToSubvecBroadcast.
bool llvm::matchShuffleToBroadcast(MachineInstr &MI, MachineRegisterInfo &MRI,
                                   const AIEBaseInstrInfo &TII,
                                   BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_SHUFFLE_VECTOR);

  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  if (MaskMatch::isMaskWithAllUndefs(Mask))
    return false;

  if (matchShuffleToVecBroadcast(MI, MRI, TII, MatchInfo))
    return true;
  if (matchShuffleToVecEltBroadcast(MI, MRI, TII, MatchInfo))
    return true;
  if (matchShuffleToSubvecBroadcast(MI, MRI, TII, MatchInfo))
    return true;
  return false;
}

/// Match something like this:
///  %1:_(<2 x s32>) = COPY $x0
///  %2:_(<2 x s32>) = G_IMPLICIT_DEF
///  %0:_(<16 x s32>) = G_SHUFFLE_VECTOR %1(<16 x s32>), %2(<16 x s32>),
///  shufflemask(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)

/// To convert to:
///  %0:_(<16 x s32>) = COPY $x0
bool llvm::matchShuffleToCopy(MachineInstr &MI, MachineRegisterInfo &MRI,
                              BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_SHUFFLE_VECTOR);

  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  if (DstTy != Src1Ty)
    return false;

  const unsigned NumSrcElems = Src1Ty.isVector() ? Src1Ty.getNumElements() : 1;
  if (Mask.size() != NumSrcElems)
    return false;

  if (MaskMatch::isMaskWithAllUndefs(Mask))
    return false;

  // Check that the mask is sequential
  MaskMatch SequentialMask{/*Height*/ 0};
  if (!SequentialMask.isValidMask(Mask))
    return false;

  MatchInfo = [=](MachineIRBuilder &B) { B.buildCopy(DstReg, Src1Reg); };

  return true;
}

/// Match something like this:
///  %2:_(<16 x s32>) = G_SHUFFLE_VECTOR %0(<16 x s32>), %1(<16 x s32>),
///  shufflemask(16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

/// To convert to:
//  %4:_(s32) = G_CONSTANT i32 0
//  %5:_(s32) = G_EXTRACT_VECTOR_ELT %0(<16 x s32>), %4(s32)
//  %3:_(<16 x s32>) = G_AIE_BROADCAST_VECTOR %5(s32)ab
//  %6:_(s32) = G_EXTRACT_VECTOR_ELT %1(<16 x s32>), %4(s32)
//  %2:_(<16 x s32>) = G_INSERT_VECTOR_ELT %3, %6(s32), %4(s32)
bool llvm::matchShuffleToExtractInsertEltToBroadcast(MachineInstr &MI,
                                                     MachineRegisterInfo &MRI,
                                                     BuildFnTy &MatchInfo) {

  assert(MI.getOpcode() == TargetOpcode::G_SHUFFLE_VECTOR);

  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  const Register Src2Reg = MI.getOperand(2).getReg();
  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  if (DstTy != Src1Ty)
    return false;

  if (!DstTy.isVector() || !Src1Ty.isVector())
    return false;

  if (DstTy.getSizeInBits() < 128)
    return false;

  const unsigned NumSrcElems = Src1Ty.getNumElements();
  const LLT DstElemTy = MRI.getType(Src1Reg).getElementType();

  if (Mask.size() != NumSrcElems)
    return false;

  if (MaskMatch::isMaskWithAllUndefs(Mask))
    return false;

  unsigned MinFrequency;
  if (MI.getMF()->getTarget().getTargetTriple().isAIE2P())
    // The scalarization of G_SHUFFLE_VECTOR in the legalizer is more beneficial
    // if there are more exceptions than NumSrcElems / 2 as AIE2P's VINSERT
    // instrutions require a move to a register used for the index unlike VPUSH.
    MinFrequency = (ShuffleMaxNumInsertions != 0) ? ShuffleMaxNumInsertions
                                                  : NumSrcElems / 2;
  else
    llvm_unreachable("MinFrequency unimplemented for target.");

  std::optional<FrequentIndexResult> FrequentIdxResult =
      MaskMatch::getFrequentIndexResult(Mask, MinFrequency);

  if (!FrequentIdxResult)
    return false;

  unsigned FrequentIdx = FrequentIdxResult->FrequentIdx;
  unsigned NonMatchingCount = FrequentIdxResult->NonMatchingCount;

  // This is a pure broadcast pattern. Should be handled by
  // matchShuffleToVecEltBroadcast combine
  if (NonMatchingCount == 0)
    return false;

  int BcstValue = Mask[FrequentIdx];

  MatchInfo = [=, &MRI](MachineIRBuilder &B) {
    Register BroadcastVecReg = MRI.createGenericVirtualRegister(Src1Ty);
    Register VecToExtract = BcstValue < (int)NumSrcElems ? Src1Reg : Src2Reg;
    auto Extr = B.buildExtractVectorElementConstant(DstElemTy, VecToExtract,
                                                    BcstValue % NumSrcElems);
    buildBroadcastVector(B, MRI, Extr.getReg(0), BroadcastVecReg);

    Register InsertSrc = BroadcastVecReg;
    Register InsertDst;

    unsigned InsertionCount = 0;
    for (unsigned Idx = 0; Idx < Mask.size(); ++Idx) {

      if (Mask[Idx] == BcstValue)
        continue;

      if (Mask[Idx] == -1)
        continue;

      Register VecToExtract = Mask[Idx] < (int)NumSrcElems ? Src1Reg : Src2Reg;

      int ExtractIdx = Mask[Idx] % NumSrcElems;
      auto ExtrElt = B.buildExtractVectorElementConstant(
          DstElemTy, VecToExtract, ExtractIdx);

      auto NonMatchingIdxReg = B.buildConstant(LLT::scalar(32), Idx);

      InsertDst = (InsertionCount == NonMatchingCount - 1)
                      ? DstReg
                      : MRI.createGenericVirtualRegister(Src1Ty);

      B.buildInsertVectorElement(InsertDst, InsertSrc, ExtrElt,
                                 NonMatchingIdxReg);
      InsertSrc = InsertDst;
      InsertionCount++;
    }
  };

  return true;
}

/// Match something like this:
/// %0:_(<32 x s16>) = COPY $x0
/// %1:_(<32 x s16>) = COPY $x1
/// %2:_(<32 x s16>) = G_SHUFFLE_VECTOR %0:_(<32 x s16>), %1:_, shufflemask(0,
/// 1, 2, 3, 4, 5, 6, 7, 32, 9, 10, 11, 12, 13, 14, 15, undef, 17, 18, 19, 20,
/// 21, 22, 23, undef, 25, 26, 27, 28, 29, 30, 31)

/// To convert to:
/// %3:_(s32) = G_CONSTANT i32 0
/// %4:_(<32 x s16>) = G_EXTRACT_VECTOR_ELT %1:_(<32 x s16>), %3:_(s32)
/// %0:_(<32 x s32>) = G_AIE_INSERT_VECTOR_ELT %0:(<32 x s16>), %4:_(s16), 8
bool llvm::matchShuffleToExtractInsertElt(MachineInstr &MI,
                                          MachineRegisterInfo &MRI,
                                          BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_SHUFFLE_VECTOR);

  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  const Register Src2Reg = MI.getOperand(2).getReg();
  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  if (DstTy != Src1Ty)
    return false;

  const unsigned NumSrcElems = Src1Ty.isVector() ? Src1Ty.getNumElements() : 1;
  const LLT ElemTy = MRI.getType(Src1Reg).getElementType();

  unsigned MaxNumInsertions;
  if (MI.getMF()->getTarget().getTargetTriple().isAIE2P())
    // The scalarization of G_SHUFFLE_VECTOR in the legalizer is more beneficial
    // if there are more exceptions than NumSrcElems / 2 as AIE2P's VINSERT
    // instructions require a move to a register used for the index unlike
    // VPUSH.
    MaxNumInsertions = (ShuffleMaxNumInsertions != 0) ? ShuffleMaxNumInsertions
                                                      : NumSrcElems / 2;
  else
    llvm_unreachable(
        "MaxNumInsertions unimplemented for target. Does the target's Insert "
        "instruction take immediate indices or does it require a register for "
        "the index?");

  if (Mask.size() != NumSrcElems)
    return false;

  if (MaskMatch::isMaskWithAllUndefs(Mask))
    return false;

  // Check that the mask is sequential
  MaskMatch SequentialMask{/*Height*/ 0};
  ShuffleMaskValidity SequentialMaskValidity =
      SequentialMask.getShuffleMaskValidity(Mask);
  bool IsValid = SequentialMaskValidity.IsValid;

  // This is a Copy pattern and will be handled by matchShuffleToCopy
  if (IsValid)
    return false;

  SmallVector<unsigned, 4> Exceptions = SequentialMaskValidity.MaskExceptions;
  assert(!Exceptions.empty());

  if (Exceptions.size() >= MaxNumInsertions)
    return false;

  MatchInfo = [=, &MRI](MachineIRBuilder &B) {
    Register InsertSrc = Src1Reg;
    Register InsertDst;

    for (const unsigned ExceptionIdx : Exceptions) {
      Register VecToExtract =
          Mask[ExceptionIdx] < (int)NumSrcElems ? Src1Reg : Src2Reg;

      int ExtractIdx = Mask[ExceptionIdx] % NumSrcElems;
      auto ExtrElt =
          B.buildExtractVectorElementConstant(ElemTy, VecToExtract, ExtractIdx);

      auto ExceptionIdxReg = B.buildConstant(LLT::scalar(32), ExceptionIdx);

      InsertDst = (ExceptionIdx == Exceptions.back())
                      ? DstReg
                      : MRI.createGenericVirtualRegister(Src1Ty);

      B.buildInsertVectorElement(InsertDst, InsertSrc, ExtrElt,
                                 ExceptionIdxReg);
      InsertSrc = InsertDst;
    }
  };

  return true;
}

bool llvm::matchShuffleToConcatExtractedSubvectors(MachineInstr &MI,
                                                   MachineRegisterInfo &MRI,
                                                   const AIEBaseInstrInfo &TII,
                                                   BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_SHUFFLE_VECTOR);

  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  const Register Src2Reg = MI.getOperand(2).getReg();
  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  if (!DstTy.isVector() || !Src1Ty.isVector())
    return false;

  const LLT ElemTy = Src1Ty.getElementType();
  unsigned ElemSize = ElemTy.getSizeInBits();
  const unsigned NumSrc1Elems = Src1Ty.getNumElements();
  const unsigned NumDstElems = DstTy.getNumElements();
  if (NumDstElems != NumSrc1Elems)
    return false;

  // Get Height (start value)
  std::optional<unsigned> Height = MaskMatch::getHeight(Mask, /*Period*/ 0);
  if (!Height)
    return false;

  // Not extractable
  if (!isPowerOfTwoOrZero((*Height) % NumSrc1Elems))
    return false;

  MaskMatch SequentialMask{/*Height*/ *Height};
  ShuffleMaskValidity SequentialMaskValidity =
      SequentialMask.getShuffleMaskValidity(Mask);
  bool IsValid = SequentialMaskValidity.IsValid;

  // Should be handled by a separate combine shuffleToCopy combine
  if (IsValid)
    return false;

  auto GetSubvecExtractIndices =
      [=, &Mask,
       &SequentialMaskValidity]() -> std::optional<SmallVector<unsigned>> {
    SmallVector<unsigned> SubvecExtractIndices = {0};
    unsigned CurrentMaskPos = 0;
    unsigned CurrentHeight = *Height;
    SmallVector<unsigned, 4> CurrentExceptions =
        SequentialMaskValidity.MaskExceptions;
    ArrayRef<int> CurrentMask = Mask;

    // Check that the exceptions are contiguous or form one block. Since undef
    // values are not counted as exceptions, we have to take them into account
    // in the condition below
    auto ContiguousExceptions = [&CurrentExceptions, &CurrentMask]() {
      return (CurrentExceptions.size() +
              getNumMaskUndefs(CurrentMask, CurrentExceptions[0])) ==
             (CurrentMask.size() - CurrentExceptions[0]);
    };

    while (ContiguousExceptions()) {
      // The Mask has nothing but exceptions
      if (CurrentExceptions.size() == CurrentMask.size()) {
        return std::nullopt;
      }
      SubvecExtractIndices.push_back(CurrentMaskPos + CurrentExceptions[0]);
      CurrentMaskPos += CurrentExceptions[0];

      // Extract the submask at the current exception indices and update the
      // current mask with it
      ArrayRef<int> SubMask =
          CurrentMask.slice(CurrentExceptions[0], CurrentExceptions.size());
      CurrentMask = SubMask;
      CurrentHeight = *MaskMatch::getHeight(CurrentMask, /*Period*/ 0);

      if (!isPowerOfTwoOrZero((CurrentHeight) % NumSrc1Elems))
        return std::nullopt;

      MaskMatch SequentialMask{/*Height*/ CurrentHeight};
      // Check the sequential validity of the new Submask
      ShuffleMaskValidity SequentialMaskValidity =
          SequentialMask.getShuffleMaskValidity(CurrentMask);

      if (SequentialMaskValidity.IsValid)
        return SubvecExtractIndices;

      // Update CurrentExceptions
      CurrentExceptions = SequentialMaskValidity.MaskExceptions;
    }

    // Non contiguous exceptions
    return std::nullopt;
  };

  auto SubvecExtractIndices = GetSubvecExtractIndices();

  // Not a valid pattern
  if (!SubvecExtractIndices)
    return false;

  // Since we can only unmerge and concat vectors of the same size, equalize the
  // extract indices, by inserting additional extract indices if needed. E.g.
  // for the shufflemask:
  // (16, 17, 18, 19, 20, 21, 22, 23, 0, 1, 2, 3, undef, undef, undef, undef)
  // We get SubvecExtractIndices = [0,8,12] which equalized becomes [0,4,8,12]
  auto EqualizeExtractIndices = [=, &SubvecExtractIndices]() {
    if (SubvecExtractIndices->size() < 2)
      return *SubvecExtractIndices;
    // Calculate the smallest difference between consecutive indices
    unsigned MinDiff = Mask.size() - SubvecExtractIndices->back();
    for (unsigned I = 1; I < SubvecExtractIndices->size(); ++I) {
      unsigned Diff =
          (*SubvecExtractIndices)[I] - (*SubvecExtractIndices)[I - 1];
      MinDiff = std::min(MinDiff, Diff);
    }
    // Generate the new indices
    SmallVector<unsigned> UsableSubvecExtractIndices;
    unsigned StartIndex = (*SubvecExtractIndices)[0];
    for (unsigned I = StartIndex; I < Mask.size(); I += MinDiff) {
      UsableSubvecExtractIndices.push_back(I);
    }
    return UsableSubvecExtractIndices;
  };

  SmallVector<unsigned> EqualizedExtractIndices = EqualizeExtractIndices();

  // Return the values in the mask for the given indices. For undef, return the
  // expected value of the provided mask pattern.
  auto getSubMaskValuesAtIndices = [&EqualizedExtractIndices, &Mask,
                                    &SequentialMask]() {
    unsigned NumSubVectors = EqualizedExtractIndices.size();
    SmallVector<unsigned, 4> SubMask;
    for (unsigned Idx : EqualizedExtractIndices) {
      unsigned ExpectedMaskValue =
          SequentialMask.getMaskValue(Idx % NumSubVectors);
      SubMask.push_back(Mask[Idx] == -1 ? ExpectedMaskValue : Mask[Idx]);
    }
    return SubMask;
  };

  SmallVector<unsigned, 4> IndicesToExtract = getSubMaskValuesAtIndices();
  unsigned NumSubVectors = IndicesToExtract.size();

  // The combine is not any better than scalarization
  if (NumSubVectors == NumDstElems)
    return false;

  // Subvectors cannot be extracted
  if (!isPowerOfTwoOrZero(NumSubVectors)) {
    return false;
  }

  unsigned SubVecNumElts = NumDstElems / NumSubVectors;
  unsigned SubVecSize = SubVecNumElts * ElemSize;
  // Check whether we can extract the subvector
  LLT SubvecType = LLT::fixed_vector(SubVecNumElts, ElemSize);
  const bool CanExtractSubvectors =
      checkExtractSubvectorPrerequisites(TII, SubvecType, Src1Ty);

  // FIXME: Enable once we have concat support of 64-bits or smaller.
  if (SubVecSize <= 64)
    return false;

  // Do not combine if any of the subvectors is not extractable with the
  // determined NumSubVectors.
  // FIXME: Dynamically adjust the NumSubVecs and thus the
  // SubVecNumElts to have all subvectors extractable.
  if (!all_of(IndicesToExtract, [=](const unsigned ExtractIdx) {
        return !(ExtractIdx % SubVecNumElts);
      })) {
    return false;
  }

  MatchInfo = [=, &MRI, &TII](MachineIRBuilder &B) {
    SmallVector<Register> ExtractedSubvectors;
    for (unsigned Idx : IndicesToExtract) {
      Register ExtractVec = Idx < NumSrc1Elems ? Src1Reg : Src2Reg;
      unsigned ExtractIdx = Idx % NumSrc1Elems;
      Register ExtractedSubVec = MRI.createGenericVirtualRegister(SubvecType);
      if (CanExtractSubvectors) {
        buildExtractSubvector(B, MRI, TII, ExtractedSubVec, ExtractVec,
                              ExtractIdx);
      } else {
        unsigned UnmergeIdx = ExtractIdx / SubVecNumElts;
        assert(!(ExtractIdx % SubVecNumElts));
        buildUnmergeVector(B, MRI, ExtractedSubVec, ExtractVec, NumSubVectors,
                           UnmergeIdx);
      }
      ExtractedSubvectors.push_back(ExtractedSubVec);
    }
    B.buildConcatVectors({DstReg}, ExtractedSubvectors);
  };

  return true;
}

/// Match something like this:
///  %1:_(s32) = COPY $r0
///  %2:_(<16 x s32>) = COPY $x0
///  %3:_(<16 x s32>) = G_AIE_BROADCAST_VECTOR %1:_(s32)
///  %0:_(<16 x s32>) = G_SHUFFLE_VECTOR %3(<16 x s32>), %2(<16 x s32>),
///  shufflemask(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
/// To convert to:
///  %0:_(<16 x s32>) = G_AIE_BROADCAST_VECTOR %1(s32)
bool llvm::matchShuffleBcstToCopy(MachineInstr &MI, MachineRegisterInfo &MRI,
                                  const TargetInstrInfo &TII,
                                  BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_SHUFFLE_VECTOR);

  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  const Register Src2Reg = MI.getOperand(2).getReg();
  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  if (DstTy != Src1Ty)
    return false;

  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();
  const AIEBaseInstrInfo &AIETII = (const AIEBaseInstrInfo &)TII;
  MachineInstr *Src1Vec = MRI.getVRegDef(Src1Reg);
  MachineInstr *Src2Vec = MRI.getVRegDef(Src2Reg);
  unsigned BcstOpcode = AIETII.getGenericBroadcastVectorOpcode();
  bool IsSrc1Bcst = (Src1Vec->getOpcode() == BcstOpcode);
  bool IsSrc2Bcst = (Src2Vec->getOpcode() == BcstOpcode);

  // Check if the first or second source is defined by a Broadcast.
  if (!IsSrc1Bcst && !IsSrc2Bcst)
    return false;

  if (!DstTy.isVector() || !Src1Ty.isVector())
    return false;

  const unsigned NumSrcElems = Src1Ty.getNumElements();
  if (Mask.size() != NumSrcElems)
    return false;

  // Determine the valid mask range
  const int MinSrc1Value = 0, MaxSrc1Value = NumSrcElems - 1;
  const int MinSrc2Value = NumSrcElems, MaxSrc2Value = 2 * NumSrcElems - 1;
  const bool IsValidSrc1Mask =
      IsSrc1Bcst ? MaskMatch(0).isMaskWithinRangeOrUndef(Mask, MinSrc1Value,
                                                         MaxSrc1Value)
                 : false;
  const bool IsValidSrc2Mask =
      IsSrc2Bcst ? MaskMatch(0).isMaskWithinRangeOrUndef(Mask, MinSrc2Value,
                                                         MaxSrc2Value)
                 : false;
  Register SrcReg;
  if (IsSrc1Bcst && IsValidSrc1Mask)
    SrcReg = Src1Reg;
  else if (IsSrc2Bcst && IsValidSrc2Mask)
    SrcReg = Src2Reg;
  else
    return false;

  MatchInfo = [=](MachineIRBuilder &B) { B.buildCopy(DstReg, SrcReg); };

  return true;
}

/// Match:
/// %35:_(s32) = G_CONSTANT i32 0
/// %38:_(s32) = G_CONSTANT i32 1
/// %34:_(s32) = G_EXTRACT_VECTOR_ELT %33(<32 x s32>), %35(s32)
/// %37:_(s32) = G_EXTRACT_VECTOR_ELT %33(<32 x s32>), %38(s32)
/// To convert to:
/// %35:_(s32) = G_CONSTANT i32 0
/// %50:_(<16 x s64>) = G_BITCAST %33(<32 x s32>)
/// %60:_(s64) = G_EXTRACT_VECTOR_ELT %50(<16 x s64>), %35(s32)
/// %34:_(s32), %37:_(s32) = G_UNMERGE_VALUES %60(s64)
bool llvm::matchPairedExtracts(MachineInstr &MI, MachineRegisterInfo &MRI,
                               CombinerHelper &Helper,
                               const TargetInstrInfo &TII,
                               BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_EXTRACT_VECTOR_ELT);
  const AIEBaseInstrInfo &AIETII = (const AIEBaseInstrInfo &)TII;
  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  const LLT S64 = LLT::scalar(64);
  const LLT S32 = LLT::scalar(32);

  if (Src1Ty.getSizeInBits() < AIETII.getBasicVectorBitSize())
    return false;

  if (DstTy != S32)
    return false;

  auto GetConstantIndex =
      [&](const MachineInstr &ExtractMI) -> std::optional<unsigned> {
    const Register IndexReg = ExtractMI.getOperand(2).getReg();
    if (auto Index = getIConstantVRegValWithLookThrough(IndexReg, MRI))
      return Index->Value.getZExtValue();
    return std::nullopt;
  };

  auto IndexCst = GetConstantIndex(MI);

  if (!IndexCst)
    return false;

  bool isEven = *IndexCst % 2 == 0;

  auto FindPairedExtract = [&](unsigned PairedIndex) -> MachineInstr * {
    for (auto &UseMI : MRI.use_nodbg_instructions(Src1Reg)) {
      if (UseMI.getOpcode() != TargetOpcode::G_EXTRACT_VECTOR_ELT)
        continue;
      if (UseMI.getParent() != MI.getParent())
        continue;
      // This helps to simplify the code in relation to
      // instruction build point. If we miss some opportunity now,
      // we will catch in the next combiner run.
      if (!Helper.dominates(MI, UseMI))
        continue;
      const Register UseDstReg = UseMI.getOperand(0).getReg();
      const LLT UseDstTy = MRI.getType(UseDstReg);
      if (UseDstTy != S32)
        continue;
      auto NextIndexCst = GetConstantIndex(UseMI);
      if (!NextIndexCst)
        continue;
      if (*NextIndexCst == PairedIndex)
        return &UseMI;
    }
    return nullptr;
  };

  const unsigned NextIndex = isEven ? *IndexCst + 1 : *IndexCst - 1;
  MachineInstr *NextExtractMI = FindPairedExtract(NextIndex);
  if (!NextExtractMI)
    return false;

  MatchInfo = [=, &MRI](MachineIRBuilder &B) {
    const Register NewExtendedDstReg = MRI.createGenericVirtualRegister(S64);
    const Register NewIdxReg = B.buildConstant(S32, *IndexCst / 2).getReg(0);

    // Bitcast from <2n x s32> to <n x s64>
    const LLT NewType = LLT::fixed_vector(Src1Ty.getNumElements() / 2, S64);
    const Register CastedVecReg = B.buildBitcast(NewType, Src1Reg).getReg(0);

    const Register NextDstReg = NextExtractMI->getOperand(0).getReg();
    // As we need to replace 2 instructions, make the second one dead,
    // so DCE will remove it. Another practical effect is, if we don't do
    // this, we will have a SSA violation because of the last buildCopy.
    const Register NewDeadReg = MRI.cloneVirtualRegister(NextDstReg);
    NextExtractMI->getOperand(0).setReg(NewDeadReg);

    B.buildExtractVectorElement(NewExtendedDstReg, CastedVecReg, NewIdxReg);
    auto Split = B.buildUnmerge(S32, NewExtendedDstReg);
    const unsigned FirstOpIndex = isEven ? 0 : 1;
    B.buildCopy(DstReg, Split->getOperand(FirstOpIndex).getReg());
    B.buildCopy(NextDstReg, Split->getOperand(FirstOpIndex ^ 1).getReg());
  };

  return true;
}

bool llvm::matchBroadcastToShl(MachineInstr &MI, MachineRegisterInfo &MRI,
                               const AIEBaseInstrInfo &TII,
                               BuildFnTy &MatchInfo) {

  assert(MI.getOpcode() == TargetOpcode::G_SHL);

  const Register DstReg = MI.getOperand(0).getReg();
  const LLT DstType = MRI.getType(DstReg);

  if (!DstType.isFixedVector())
    return false;

  const Register SrcReg1 = MI.getOperand(1).getReg();
  const Register SrcReg2 = MI.getOperand(2).getReg();

  const MachineInstr *DefAmtMI = MRI.getVRegDef(SrcReg2);

  if (DefAmtMI->getOpcode() != TII.getGenericBroadcastVectorOpcode())
    return false;

  auto CstSrc =
      getIConstantVRegValWithLookThrough(DefAmtMI->getOperand(1).getReg(), MRI);

  if (!CstSrc)
    return false;

  const int ShiftAmount = CstSrc->Value.getZExtValue();
  assert(ShiftAmount >= 0 && "Invalid shift amount");

  MatchInfo = [=, &MRI](MachineIRBuilder &B) {
    Register CurAddReg = SrcReg1;
    for (int NumAdds = 0; NumAdds < ShiftAmount; NumAdds++) {
      Register AddReg = MRI.cloneVirtualRegister(SrcReg1);
      B.buildInstr(TargetOpcode::G_ADD)
          .addDef(AddReg)
          .addReg(CurAddReg)
          .addReg(CurAddReg);
      CurAddReg = AddReg;
    }

    B.buildCopy(DstReg, CurAddReg);
  };

  return true;
}

// Can we narrow all operands of a PHI node without losing precision?
// We can narrow losing precision, provided that all further users of this
// PHI node are s20 users (in this case we are not interested in the higher
// bits).
static bool
canBeNarrowedWithoutLoss(MachineInstr &Phi,
                         SmallPtrSet<MachineInstr *, 4> &VisitedInstrs,
                         MachineRegisterInfo &MRI) {
  if (VisitedInstrs.contains(&Phi))
    return true;
  VisitedInstrs.insert(&Phi);
  for (unsigned OpNum = 1; OpNum < Phi.getNumOperands(); OpNum += 2) {
    const Register PhiRegIn = Phi.getOperand(OpNum).getReg();
    assert(MRI.getType(PhiRegIn).getSizeInBits() == 32 && "Mixed Phi node");

    MachineInstr *DefMI = MRI.getVRegDef(PhiRegIn);
    const unsigned Opcode = DefMI->getOpcode();
    if (Opcode == TargetOpcode::G_PHI) {
      if (!canBeNarrowedWithoutLoss(*DefMI, VisitedInstrs, MRI))
        return false;
    } else if (Opcode == TargetOpcode::G_CONSTANT) {
      if (!isIntN(20, DefMI->getOperand(1).getCImm()->getSExtValue()))
        return false;
    } else if (Opcode == TargetOpcode::G_ZEXT) {
      if (MRI.getType(DefMI->getOperand(1).getReg()) != LLT::scalar(20))
        return false;
    } else if (Opcode != TargetOpcode::G_IMPLICIT_DEF) {
      return false;
    }
  }
  return true;
}

struct PhiUsageAnalysisResult {
  bool MayNeedFullPrecision = false;
  bool IsUsedByS20 = false;

  void operator|=(const PhiUsageAnalysisResult &Other) {
    IsUsedByS20 |= Other.IsUsedByS20;
    MayNeedFullPrecision |= Other.MayNeedFullPrecision;
  }
};

// Do we have any s20 user of this PHI node? Also, do we have users that
// may require all 32 bits of the representation (extra users)?
// We don't need to explicitly know the end users of the s20 value, we
// can just look to the truncations.
static PhiUsageAnalysisResult
isUsedByAnyS20Instruction(MachineInstr &Phi,
                          SmallPtrSet<MachineInstr *, 4> &VisitedInstrs,
                          MachineRegisterInfo &MRI) {

  PhiUsageAnalysisResult Result;

  if (VisitedInstrs.contains(&Phi))
    return Result;
  VisitedInstrs.insert(&Phi);

  Register DefReg = Phi.getOperand(0).getReg();
  // We can not break/return from this loop as soon as we have an S20 use
  // because we need to collect MayNeedFullPrecision.
  for (auto &User : MRI.use_instructions(DefReg)) {
    const unsigned Opcode = User.getOpcode();
    if (Opcode == TargetOpcode::G_TRUNC) {
      const unsigned TruncatedSize =
          MRI.getType(User.getOperand(0).getReg()).getScalarSizeInBits();
      if (TruncatedSize == 20)
        Result.IsUsedByS20 = true;
      else if (TruncatedSize > 20)
        Result.MayNeedFullPrecision = true;
    } else if (Opcode == TargetOpcode::G_PHI) {
      Result |= isUsedByAnyS20Instruction(User, VisitedInstrs, MRI);
    } else {
      Result.MayNeedFullPrecision = true;
    }
  }

  return Result;
}

/// Narrow Phi nodes to s20, when it is safe.
bool llvm::matchNarrowPhi(MachineInstr &Phi, MachineRegisterInfo &MRI,
                          GISelChangeObserver &Observer, BuildFnTy &MatchInfo) {

  assert(Phi.getOpcode() == TargetOpcode::G_PHI);

  // We can only narrow s32 -> s20.
  if (MRI.getType(Phi.getOperand(0).getReg()) != LLT::scalar(32))
    return false;

  SmallPtrSet<MachineInstr *, 4> VisitedInstrs;
  PhiUsageAnalysisResult Result =
      isUsedByAnyS20Instruction(Phi, VisitedInstrs, MRI);
  if (!Result.IsUsedByS20)
    return false;

  VisitedInstrs.clear();
  const bool CanKeepPrecision =
      canBeNarrowedWithoutLoss(Phi, VisitedInstrs, MRI);

  // We can only narrow in the following 2 situations:
  // * We may lose precision by narrowing this PHI node, but all
  // users are s20 bit users, so higher bits are not important.
  // * We have users that may require full precision, however, we know
  // that the narrowed inputs can be represented as s20 without loss
  // of precision.
  if (!CanKeepPrecision && Result.MayNeedFullPrecision)
    return false;

  // We will do the following transformation:
  // PHI(A, B) -> ZEXT(PHI(TRUNC(A), TRUNC(B)))
  MatchInfo = [=, &MRI, &Phi, &Observer](MachineIRBuilder &B) {
    // We change the PHI node instead of building a new one.
    const LLT S20 = LLT::scalar(20);
    const Register NewDefReg = MRI.createGenericVirtualRegister(S20);
    const Register DefReg = Phi.getOperand(0).getReg();

    MachineBasicBlock *MBB = Phi.getParent();
    MachineBasicBlock::iterator InsertPt = MBB->getFirstNonPHI();
    B.setInsertPt(*Phi.getParent(), InsertPt);
    // Extend the output.
    // We use ZExt because it is the only safe, considering the
    // previous analysis (isUsedByAnyS20Instruction - trunc case).
    Observer.createdInstr(*B.buildZExt(DefReg, NewDefReg));
    Phi.getOperand(0).setReg(NewDefReg);

    // Truncate the inputs.
    for (unsigned OpNum = 1; OpNum < Phi.getNumOperands(); OpNum += 2) {
      const Register SrcReg = Phi.getOperand(OpNum).getReg();
      const Register NewSrcReg = MRI.createGenericVirtualRegister(S20);
      MachineInstr *DefMI = MRI.getVRegDef(SrcReg);

      MachineBasicBlock *DefMIMBB = DefMI->getParent();
      MachineBasicBlock::iterator InsertPt = ++DefMI->getIterator();
      if (InsertPt != DefMIMBB->end() && InsertPt->isPHI())
        InsertPt = DefMIMBB->getFirstNonPHI();

      B.setInsertPt(*DefMI->getParent(), InsertPt);
      B.setDebugLoc(DefMI->getDebugLoc());
      Observer.createdInstr(*B.buildTrunc(NewSrcReg, SrcReg));
      Observer.changingInstr(Phi);
      Phi.getOperand(OpNum).setReg(NewSrcReg);
      Observer.changedInstr(Phi);
    }
  };

  return true;
}

/// Check if the user is a potentially legal user of s20 type.
/// If it is not legal, it will be legalized as 32 bit operation.
static bool isUsedByLikelyLegalS20User(MachineRegisterInfo &MRI,
                                       const MachineInstr &MI) {
  return any_of(MRI.use_nodbg_instructions(MI.getOperand(0).getReg()),
                [&](const MachineInstr &UseMI) {
                  return UseMI.getOpcode() == TargetOpcode::G_PHI ||
                         UseMI.getOpcode() == TargetOpcode::G_PTR_ADD ||
                         UseMI.getOpcode() == TargetOpcode::G_STORE ||
                         UseMI.getOpcode() == TargetOpcode::G_INTRINSIC ||
                         UseMI.getOpcode() ==
                             TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS;
                });
}

static void changeLoadStoreDataRegister(MachineInstr &MI, Register DataReg,
                                        MachineRegisterInfo &MRI) {
  GLoadStore &LdStInst = cast<GLoadStore>(MI);
  LdStInst.getMMO().setType(MRI.getType(DataReg));
  LdStInst.getOperand(0).setReg(DataReg);
}

/// Narrow operations that are feeding truncations to s20.
/// Covers G_LOAD and G_CONSTANT.
bool llvm::matchNarrowTrunc(MachineInstr &MI, MachineRegisterInfo &MRI,
                            GISelChangeObserver &Observer,
                            BuildFnTy &MatchInfo) {

  assert(MI.getOpcode() == TargetOpcode::G_TRUNC);

  auto [DstReg, SrcReg] = MI.getFirst2Regs();

  if (MRI.getType(DstReg).getScalarSizeInBits() != 20)
    return false;

  MachineInstr &SrcMI = *MRI.getVRegDef(SrcReg);

  if (SrcMI.getOpcode() == TargetOpcode::G_CONSTANT) {
    MatchInfo = [=, &MI, &SrcMI, &MRI](MachineIRBuilder &B) {
      auto NewConstant = B.buildConstant(
          LLT::scalar(20),
          *getIConstantVRegSExtVal(SrcMI.getOperand(0).getReg(), MRI));
      MRI.replaceRegWith(MI.getOperand(0).getReg(),
                         NewConstant->getOperand(0).getReg());
      MI.eraseFromParent();
    };
    return true;
  }

  // Ideally, we could allow more users, provided that they are all TRUNCs.
  // However, if we have more users, the live range of this register could
  // spread through more blocks, and this could lead to more register pressure
  // on s20 registers.
  if (!MRI.hasOneNonDBGUse(SrcReg) || !isUsedByLikelyLegalS20User(MRI, MI))
    return false;

  if (SrcMI.getOpcode() == TargetOpcode::G_LOAD) {
    MatchInfo = [=, &MI, &SrcMI, &MRI, &Observer](MachineIRBuilder &B) {
      Observer.changingInstr(SrcMI);
      changeLoadStoreDataRegister(SrcMI, MI.getOperand(0).getReg(), MRI);
      Observer.changedInstr(SrcMI);
      MI.eraseFromParent();
    };
    return true;
  }

  return false;
}

/// Narrow operations that are fed by zext from s20.
/// Covers G_STORE.
bool llvm::matchNarrowZext(MachineInstr &MI, MachineRegisterInfo &MRI,
                           GISelChangeObserver &Observer,
                           BuildFnTy &MatchInfo) {

  assert(MI.getOpcode() == TargetOpcode::G_ZEXT);

  const LLT S20 = LLT::scalar(20);
  const LLT S32 = LLT::scalar(32);
  auto [DstReg, SrcReg] = MI.getFirst2Regs();

  if (MRI.getType(SrcReg) != S20 || MRI.getType(DstReg) != S32)
    return false;

  if (!MRI.hasOneNonDBGUse(DstReg))
    return false;

  MachineInstr &DstMI = *MRI.use_instr_nodbg_begin(DstReg);

  if (DstMI.getOpcode() == TargetOpcode::G_STORE) {
    MatchInfo = [=, &MI, &DstMI, &MRI, &Observer](MachineIRBuilder &B) {
      Observer.changingInstr(DstMI);
      changeLoadStoreDataRegister(DstMI, MI.getOperand(1).getReg(), MRI);
      Observer.changedInstr(DstMI);
      MI.eraseFromParent();
    };
    return true;
  }

  return false;
}
