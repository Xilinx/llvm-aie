//=== lib/CodeGen/GlobalISel/AIECombinerHelper.cpp
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIECombinerHelper.h"
#include "AIE2TargetMachine.h"
#include "AIEBaseInstrInfo.h"
#include "AIETargetMachine.h"
#include "MCTargetDesc/AIE2MCTargetDesc.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/CodeGen/GlobalISel/GenericMachineInstrs.h"
#include "llvm/CodeGen/GlobalISel/Utils.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineRegionInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/IR/IntrinsicsAIE2.h"
#include <optional>

#define DEBUG_TYPE "aie-combine"

using namespace llvm;

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

MachineInstr *findPreIncMatch(MachineInstr &MemI, MachineRegisterInfo &MRI,
                              CombinerHelper &Helper,
                              AIELoadStoreCombineMatchData &MatchData,
                              const AIEBaseInstrInfo &TII) {
  // This is currently done with patterns in instruction selection.
  // No need to do it here.
  if (MRI.getType(MemI.getOperand(0).getReg()).getSizeInBits() >= 1024)
    return nullptr;
  if (!EnableOffsetCombine)
    return nullptr;
  Register Addr = MemI.getOperand(1).getReg();
  MachineInstr *AddrDef = getDefIgnoringCopies(Addr, MRI);
  if (AddrDef->getOpcode() == TargetOpcode::G_PTR_ADD) {
    MatchData = {AddrDef, TII.getOffsetMemOpcode(MemI.getOpcode()), &MemI,
                 /*ExtraInstrsToMove=*/{},
                 /*RemoveInstr=*/false};
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
                         MachineRegisterInfo &MRI) {
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

std::vector<MachineInstr *>
findConstantOffsetsToMove(MachineInstr &PtrAdd, MachineInstr &PtrAddInsertLoc,
                          MachineRegisterInfo &MRI, CombinerHelper &Helper) {
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
bool isBetween(MachineInstr &MI, MachineInstr &First, MachineInstr &Last,
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

MachineInstr *findPostIncMatch(MachineInstr &MemI, MachineRegisterInfo &MRI,
                               CombinerHelper &Helper,
                               AIELoadStoreCombineMatchData &MatchData,
                               const AIEBaseInstrInfo &TII) {
  if (!EnablePostIncCombine)
    return nullptr;
  if (MRI.getType(MemI.getOperand(0).getReg()).getSizeInBits() >= 1024)
    return nullptr;

  Register Addr = MemI.getOperand(1).getReg();
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
      MatchData = {&PtrInc, *CombinedOpcode, &MemI,
                   /*ExtraInstrsToMove=*/{},
                   /*RemoveInstr=*/true};
      // The offset of the PtrInc might be defined after MemI, in this case we
      // want to verify if it would be possible to insert the combined
      // instruction at the PtrInc instead of the location of MemI. Instruction
      // with side effects are also blocking: Loads, stores, calls, instructions
      // with side effects cannot be moved.
      // TODO: try move other instructions that block us from combining
    } else if (canDelayMemOp(MemI, PtrAddInsertLoc, MRI)) {
      // If Definition of the offset is a G_CONSTANT we have to move that
      // instruction up
      MatchData = {
          &PtrInc, *CombinedOpcode, &PtrAddInsertLoc,
          /*ExtraInstrsToMove=*/
          findConstantOffsetsToMove(PtrInc, PtrAddInsertLoc, MRI, Helper),
          /*RemoveInstr=*/true};
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
    bool AddrUsesDominatesInsertPoint = checkRegUsesDominate(
        Addr, *MatchData.CombinedInsertPoint, PtrInc, MRI, Helper, TII);
    if (EnableGreedyAddressCombine || AddrUsesDominatesInsertPoint)
      return &PtrInc;
  }
  return nullptr;
}

bool llvm::matchLdStInc(MachineInstr &MemI, MachineRegisterInfo &MRI,
                        AIELoadStoreCombineMatchData &MatchData,
                        CombinerHelper &Helper, const TargetInstrInfo &TII) {
  const AIEBaseInstrInfo &AIETII = (const AIEBaseInstrInfo &)TII;
  return findPostIncMatch(MemI, MRI, Helper, MatchData, AIETII) ||
         findPreIncMatch(MemI, MRI, Helper, MatchData, AIETII);
}

void llvm::applyLdStInc(MachineInstr &MI, MachineRegisterInfo &MRI,
                        MachineIRBuilder &B,
                        AIELoadStoreCombineMatchData &MatchData,
                        GISelChangeObserver &Observer) {
  if (MatchData.CombinedInsertPoint) {
    B.setInstr(*MatchData.CombinedInsertPoint);
  } else {
    B.setMBB(*MI.getParent());
  }
  // Debug Loc: Debug Loc of LOAD STORE: MI
  B.setDebugLoc(MI.getDebugLoc());
  auto NewInstr = B.buildInstr(MatchData.CombinedInstrOpcode);
  for (auto *Instr : MatchData.ExtraInstrsToMove) {
    Instr->moveBefore(NewInstr);
  }
  if (MI.mayLoad())
    NewInstr.addDef(MI.getOperand(0).getReg() /* Loaded value */);
  if (MatchData.RemoveInstr)
    // If we remove the instr it is because we have defs that would otherwise
    // be redefined. We have to add these defs into the new instruction.
    for (auto Def : MatchData.Instr->defs())
      if (Def.isReg())
        NewInstr.addDef(Def.getReg());
  if (MI.getOpcode() == TargetOpcode::G_STORE)
    NewInstr.addUse(MI.getOperand(0).getReg() /* Stored value */);
  for (auto Use : MatchData.Instr->uses())
    if (Use.isReg())
      NewInstr.addUse(Use.getReg());
  for (auto *Mem : MI.memoperands())
    NewInstr.addMemOperand(Mem);

  if (MatchData.RemoveInstr)
    MatchData.Instr->removeFromParent();
  MI.removeFromParent();
}

// Match all equivalents of these:
// %0:_(<16 x s32>) = G_IMPLICIT_DEF
// %1:_(s32) = G_IMPLICIT_DEF
// %2:_(<16 x s32>) =  G_AIE_ADD_VECTOR_ELT_HI %0, %1(s32)
//
// Combine into:
// %0:_(<16 x s32>) = G_IMPLICIT_DEF
// %2:_(<16 x s32>) = COPY %0
bool llvm::matchAddVecEltUndef(MachineInstr &MI, MachineRegisterInfo &MRI) {

  assert(MI.getOpcode() == AIE2::G_AIE_ADD_VECTOR_ELT_HI &&
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

/// Checks whether the instruction produces or can be adapted to produce
/// a single S20 output.
static bool canProduceS20(const MachineRegisterInfo &MRI,
                          const MachineInstr &MI) {
  switch (MI.getOpcode()) {
  case TargetOpcode::G_ZEXT:
    // If the ZEXT input is S20, we can just turn MI into a COPY
    return MRI.getType(MI.getOperand(1).getReg()) == LLT::scalar(20);
  case TargetOpcode::G_TRUNC:
  case TargetOpcode::G_LOAD:
  case TargetOpcode::G_PHI:
  case TargetOpcode::G_CONSTANT:
  case TargetOpcode::G_IMPLICIT_DEF:
    return true;
  default:
    return false;
  }
}

/// The function checks if the node can be adapted to produce an S20 value, and
/// recursively looks forward to see if all users can be changed to consume S20
/// inputs.
///
/// \param Start: Source Node identified by traversing backwards from a S20
/// user. The node can have a DirectionNode, in which case only one particular
/// user should be analyzed.
/// \param VisitedNodes: To keep track of visited instructions
bool canNarrowUserTreeToS20(MachineRegisterInfo &MRI, InstrNode Start,
                            std::set<MachineInstr *> &VisitedNodes) {
  Start.dbgPrintNode();

  MachineInstr &MI = *Start.getBaseNode();

  // Ignore nodes that have already been completely visited. This is needed
  // because G_PHI can introduce cycles.
  if (!Start.getRematerializeForUser()) {
    if (VisitedNodes.count(&MI)) {
      return true;
    }
    VisitedNodes.insert(&MI);
  }

  // First check that the instruction itself can be adapted to produce S20
  if (!canProduceS20(MRI, MI)) {
    LLVM_DEBUG(dbgs() << "  Cannot produce S20:" << MI);
    return false;
  }

  // Now check if users can be adapted to consume an S20 input
  assert(MI.getNumExplicitDefs() == 1);
  Register DefReg = MI.getOperand(0).getReg();
  for (MachineInstr &Use : MRI.use_nodbg_instructions(DefReg)) {
    LLVM_DEBUG(dbgs() << "  Verify use for narrowing: " << Use);
    if (MachineInstr *LookAtSingleUse = Start.getRematerializeForUser();
        LookAtSingleUse && &Use != LookAtSingleUse) {
      LLVM_DEBUG(dbgs() << "    Skip irrelevant user: " << Use);
      continue;
    }
    switch (Use.getOpcode()) {
    case TargetOpcode::G_TRUNC:
      // Check if the scalar size of the operand's type is at least 20 bits,
      // this ensures that the G_TRUNC can be safely converted to a COPY.
      if (MRI.getType(Use.getOperand(0).getReg()).getScalarSizeInBits() < 20)
        return false;
      [[fallthrough]];
    case TargetOpcode::G_PTR_ADD:
    case TargetOpcode::G_STORE: // Data operand is later modified to S20 type
      continue;
    case TargetOpcode::G_INTRINSIC:
      switch (cast<GIntrinsic>(Use).getIntrinsicID()) {
      case Intrinsic::aie2_add_2d:
      case Intrinsic::aie2_add_3d:
        continue;
      default:
        LLVM_DEBUG(dbgs() << "    User cannot consume S20: " << Use);
        return false;
      }
    case TargetOpcode::G_ZEXT:
    case TargetOpcode::G_PHI:
      if (!canNarrowUserTreeToS20(MRI, InstrNode(&Use), VisitedNodes))
        return false;
      continue;
    default:
      LLVM_DEBUG(dbgs() << "    User cannot consume S20: " << Use);
      return false;
    }
  }
  LLVM_DEBUG(dbgs() << "  Can be narrowed: " << MI);
  return true;
}

/// The function recursively looks backwards to identify the src nodes that end
/// up defining \p VReg. In particular, this looks through G_TRUNC, COPY and
/// G_PHI to find sources.
///
/// \param Vreg Register for which to find "source" instructions
/// \param UserInstr Instruction that uses \p VReg
/// \param VisitedNodes Keeps track of visited instructions to avoid cycles
/// \param SourceNodes Will contain all identified sources for \p VReg.
void getSrcNodes(Register VReg, MachineInstr *UserInstr,
                 MachineRegisterInfo &MRI,
                 std::set<MachineInstr *> &VisitedNodes,
                 std::set<InstrNode> &SourceNodes) {
  assert(VReg.isVirtual());
  MachineInstr *MI = nullptr;
  const LLT S20 = LLT::scalar(20);

  // Loop over the instructions defining the used vregs until finding a
  // source instruction or a physical register.
  while (VReg.isVirtual()) {
    MI = MRI.getVRegDef(VReg);
    LLVM_DEBUG(dbgs() << "  Visiting producer: " << *MI);

    if (VisitedNodes.count(MI)) {
      LLVM_DEBUG(dbgs() << "  ** Skipping already visited: " << *MI);
      return;
    }
    VisitedNodes.insert(MI);

    // Look through COPY, TRUNC and PHI to find "sources"
    switch (MI->getOpcode()) {
    case TargetOpcode::G_TRUNC:
    case TargetOpcode::COPY:
      VReg = MI->getOperand(1).getReg();
      break;
    case TargetOpcode::G_PHI:
      for (unsigned SrcIdx = 1; SrcIdx < MI->getNumOperands(); SrcIdx += 2) {
        getSrcNodes(MI->getOperand(SrcIdx).getReg(), MI, MRI, VisitedNodes,
                    SourceNodes);
      }
      return;
    case TargetOpcode::G_IMPLICIT_DEF:
    case TargetOpcode::G_CONSTANT:
      // This is an easily re-materializable node, consider it as source.
      LLVM_DEBUG(dbgs() << "  Found source node (remat): " << *MI);
      if (MRI.getType(VReg) != S20)
        SourceNodes.emplace(InstrNode(MI, UserInstr));
      return;
    default:
      // Don't know how to traverse VReg producer, consider as a source
      LLVM_DEBUG(dbgs() << "  Found source node (other): " << *MI);
      if (MRI.getType(VReg) != S20)
        SourceNodes.emplace(InstrNode(MI));
      return;
    }
    UserInstr = MI;
  }

  // We got to a physical register, consider the node using it as a source
  LLVM_DEBUG(dbgs() << "  Found source node (phys reg consumer): "
                    << *UserInstr);
  assert(UserInstr->isCopy() && "Expected phys reg user to be a COPY");
  SourceNodes.emplace(UserInstr);
}

void InstrNode::dbgPrintNode() const {
  LLVM_DEBUG(dbgs() << "  Node : " << *BaseNode);
  if (RematerializeForUser)
    LLVM_DEBUG(dbgs() << "    Rematerialize for: " << *RematerializeForUser);
}

/// Return the S20 input operands of \p MI
std::vector<MachineOperand *> getS20Operands(MachineInstr &MI,
                                             MachineRegisterInfo &MRI) {
  std::vector<MachineOperand *> Operands;
  for (MachineOperand &MO : MI.explicit_uses()) {
    if (MO.isReg() && MRI.getType(MO.getReg()) == LLT::scalar(20))
      Operands.push_back(&MO);
  }
  return Operands;
}

/// The function recursively looks backwards to identify the src nodes that end
/// up defining the S20 inputs of \p MI. Sources whose entire "user tree" can
/// get converted to S20 will get added to \p ValidStartNodes.
bool getOperandsToNarrow(MachineInstr &MI, MachineRegisterInfo &MRI,
                         std::set<InstrNode> &ValidStartNodes) {
  LLVM_DEBUG(dbgs() << "\n*** Trying to narrow operands for " << MI);

  // Find all the source instructions for each S20 operand
  for (const MachineOperand *MO : getS20Operands(MI, MRI)) {
    std::set<InstrNode> SourceNodes;
    std::set<MachineInstr *> VisitedNodes;

    // Collect all the src nodes in PossibleStartNodes for given MO
    LLVM_DEBUG(dbgs() << "Find source nodes for " << *MO << "\n");
    getSrcNodes(MO->getReg(), &MI, MRI, VisitedNodes, SourceNodes);

    // Traverse all users of our source nodes to verify if they can
    // all be narrowed to S20 types.
    LLVM_DEBUG(dbgs() << "Verify source nodes for " << *MO << "\n");
    if (all_of(SourceNodes, [&MRI](const InstrNode &InstrNode) {
          std::set<MachineInstr *> VisitedNodes;
          return canNarrowUserTreeToS20(MRI, InstrNode, VisitedNodes);
        })) {
      ValidStartNodes.insert(SourceNodes.begin(), SourceNodes.end());
    }
  }

  // We have a "match" if we found at least one valid source to narrow.
  return !(ValidStartNodes.empty());
}

/// Checks if the instruction natively consumes S20 for scalar inputs.
static bool isNativeS20Consumer(const MachineInstr &MI) {
  switch (MI.getOpcode()) {
  case TargetOpcode::G_PTR_ADD:
    return true;
  case TargetOpcode::G_INTRINSIC:
    switch (cast<GIntrinsic>(MI).getIntrinsicID()) {
    case Intrinsic::aie2_add_2d:
    case Intrinsic::aie2_add_3d:
      return true;
    default:
      return false;
    }
  default:
    return false;
  }
}

bool llvm::matchS20NarrowingOpt(MachineInstr &MI, MachineRegisterInfo &MRI,
                                std::set<InstrNode> &ValidStartNodes) {
  if (!EnableS20Narrowing || !isNativeS20Consumer(MI))
    return false;

  return getOperandsToNarrow(MI, MRI, ValidStartNodes);
}

/// When the \a InstrNode is G_CONSTANT/G_IMPLICIT_DEF create a new variant of
/// S20 type and use that with the Instruction pointed by \a DirectionNode
void InstrNode::rematerializeStartNode(MachineRegisterInfo &MRI,
                                       MachineIRBuilder &B,
                                       GISelChangeObserver &Observer) {
  assert(isRematerializable() && getRematerializeForUser());
  const LLT S20 = LLT::scalar(20);
  Register NewConstOrImpDef, OldConstOrImplDef;

  OldConstOrImplDef = BaseNode->getOperand(0).getReg();
  switch (BaseNode->getOpcode()) {
  case TargetOpcode::G_CONSTANT:
    NewConstOrImpDef = MRI.createGenericVirtualRegister(S20);
    B.setInstrAndDebugLoc(*BaseNode);
    // TODO: Zero Extend or Sign ?
    BaseNode = B.buildConstant(
        NewConstOrImpDef, BaseNode->getOperand(1).getCImm()->getZExtValue());
    Observer.createdInstr(*BaseNode);
    break;
  case TargetOpcode::G_IMPLICIT_DEF:
    NewConstOrImpDef = MRI.createGenericVirtualRegister(S20);
    B.setInstrAndDebugLoc(*BaseNode);
    BaseNode = B.buildUndef(NewConstOrImpDef);
    Observer.createdInstr(*BaseNode);
    break;
  }

  MachineOperand *Op =
      RematerializeForUser->findRegisterUseOperand(OldConstOrImplDef);
  if (Op) {
    Observer.changingInstr(*RematerializeForUser);
    Op->setReg(NewConstOrImpDef);
    Observer.changedInstr(*RematerializeForUser);
  } else {
    llvm_unreachable("RematerializeForUser must have use of StartNode");
  }
}

/// The function recursively goes through \p Start and its users to adapt them
/// to use S20 types. It stops when it meets a native S20 consumer, like G_TRUNC
/// or G_PTR_ADD instructions.
bool modifyToS20(InstrNode Start, MachineRegisterInfo &MRI, MachineIRBuilder &B,
                 GISelChangeObserver &Observer, CombinerHelper &Helper) {
  const LLT S20 = LLT::scalar(20);
  MachineInstr *StartNodeMI = Start.getBaseNode();

  // If Start can be rematerialized, only modify one user to use the
  // rematerialized instruction and leave the others unchanged.
  if (MachineInstr *RematForUser = Start.getRematerializeForUser()) {
    Start.rematerializeStartNode(MRI, B, Observer);
    StartNodeMI = RematForUser;
  }

  // Easy case
  if (isNativeS20Consumer(*StartNodeMI))
    return true;

  LLVM_DEBUG(dbgs() << "Narrow operand of :" << *StartNodeMI);
  switch (StartNodeMI->getOpcode()) {
  case TargetOpcode::G_LOAD: {
    GLoadStore &LoadInst = cast<GLoadStore>(*StartNodeMI);
    Observer.changingInstr(*StartNodeMI);
    LoadInst.getMMO().setType(S20);
    MRI.setType(StartNodeMI->getOperand(0).getReg(), S20);
    Observer.changedInstr(*StartNodeMI);
    break;
  }
  case TargetOpcode::G_STORE: {
    GLoadStore &StoreInst = cast<GLoadStore>(*StartNodeMI);
    Observer.changingInstr(*StartNodeMI);
    StoreInst.getMMO().setType(S20);
    Observer.changedInstr(*StartNodeMI);
    return true;
  }
  case TargetOpcode::G_PHI: {
    if (MRI.getType(StartNodeMI->getOperand(0).getReg()) == S20) {
      return true;
    }
    Observer.changingInstr(*StartNodeMI);
    MRI.setType(StartNodeMI->getOperand(0).getReg(), S20);
    Observer.changedInstr(*StartNodeMI);
    break;
  }
  case TargetOpcode::COPY:
  case TargetOpcode::G_ZEXT: {
    assert(MRI.getType(StartNodeMI->getOperand(1).getReg()) == S20 &&
           "Source instruction is not of type S20");
    MRI.setType(StartNodeMI->getOperand(0).getReg(), S20);
    Helper.replaceOpcodeWith(*StartNodeMI, TargetOpcode::COPY);
    // FIXME : tryCombineCopy if successful will remove the COPY aka
    // *StartNodeMI and ahead when we try to get uses of *StartNodeMI we will
    // error out Helper.tryCombineCopy(*StartNodeMI);
    break;
  }
  case TargetOpcode::G_TRUNC: {
    // From canNarrowUserTreeToS20, we have a guarantee that the output of
    // G_TRUNC is at least 20-bit. We can then replace each G_TRUNC by a COPY.
    assert(MRI.getType(StartNodeMI->getOperand(1).getReg()).getSizeInBits() >=
               20 &&
           "Source instruction is not of type S20");
    Helper.replaceOpcodeWith(*StartNodeMI, TargetOpcode::COPY);
    Helper.tryCombineCopy(*StartNodeMI);
    return true;
  }
  default: {
    LLVM_DEBUG(dbgs() << "Node :" << *StartNodeMI);
    llvm_unreachable("Unexpected OpCode, while modifying IR");
  }
  }

  switch (StartNodeMI->getOpcode()) {
  case TargetOpcode::COPY:
  case TargetOpcode::G_LOAD:
  case TargetOpcode::G_PHI: {
    const auto UseInstIter =
        MRI.use_nodbg_instructions(StartNodeMI->getOperand(0).getReg());
    std::vector<MachineInstr *> UseInstr;
    // We cannot directly iterate on \var UseInstIter and modify the
    // instruction. Creating a std::vector allows us to iterate without
    // corrupting the iterator allowing us to modify the instructions.
    for (auto &Use : UseInstIter)
      UseInstr.push_back(&Use);
    // Iterate on all the uses and modify the type to s20
    for (auto &Use : UseInstr) {
      InstrNode NextNodeToModify(Use);
      if (!modifyToS20(NextNodeToModify, MRI, B, Observer, Helper))
        llvm_unreachable("All input nodes should have updated");
    }
    break;
  }
  default: {
    LLVM_DEBUG(dbgs() << "Node :" << *StartNodeMI);
    llvm_unreachable("Unexpected OpCode, while modifying IR");
  }
  }
  return true;
}

void llvm::applyS20NarrowingOpt(MachineInstr &MI, MachineRegisterInfo &MRI,
                                MachineIRBuilder &B,
                                GISelChangeObserver &Observer,
                                CombinerHelper &Helper,
                                std::set<InstrNode> &ValidStartNodes) {

  for (const auto &StartNode : ValidStartNodes) {
    if (!modifyToS20(StartNode, MRI, B, Observer, Helper))
      assert(false && "All input nodes should have updated");
  }
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

  const unsigned Opcode = IsSignedExt ? AIE2::G_AIE_SEXT_EXTRACT_VECTOR_ELT
                                      : AIE2::G_AIE_ZEXT_EXTRACT_VECTOR_ELT;
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
  case 256:
  case 512:
  case 1024:
    break;
  default:
    // unimplemented
    return false;
  }

  const unsigned NumOps = MI.getNumOperands();
  const MachineOperand FirstOp = MI.getOperand(1);
  for (unsigned i = 2; i < NumOps; i++) {
    if (!MI.getOperand(i).isIdenticalTo(FirstOp)) {
      return false;
    }
  }
  MatchInfo = std::make_pair(DstVecReg, FirstOp.getReg());
  return true;
}

bool llvm::applySplatVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                            MachineIRBuilder &B,
                            std::pair<Register, Register> &MatchInfo) {
  B.setInstrAndDebugLoc(MI);
  auto [DstVecReg, SrcReg] = MatchInfo;
  const LLT SrcTy = MRI.getType(SrcReg);
  const LLT DstVecTy = MRI.getType(DstVecReg);
  const unsigned DstVecSize = DstVecTy.getSizeInBits();

  if (SrcTy == LLT::scalar(8) || SrcTy == LLT::scalar(16)) {
    const LLT S32 = LLT::scalar(32);
    Register Src32BitReg = MRI.createGenericVirtualRegister(S32);
    B.buildAnyExt(Src32BitReg, SrcReg);
    SrcReg = Src32BitReg;
  }
  // Check if the destination vector size is 512 bits.
  if (DstVecSize == 512) {
    // Build the G_AIE_BROADCAST_VECTOR instruction for a 512-bit vector.
    B.buildInstr(AIE2::G_AIE_BROADCAST_VECTOR, {DstVecReg}, {SrcReg});
  } else {
    const unsigned SrcSize = SrcTy.getSizeInBits();
    const unsigned DstVec512BitLen = 512 / SrcSize;

    // Create a 512-bit generic virtual register for the destination vector
    // as 256-bit and 1024-bit broadcast support is not available.
    Register DstVec512BitReg = MRI.createGenericVirtualRegister(
        LLT::fixed_vector(DstVec512BitLen, SrcSize));

    // Build the G_AIE_BROADCAST_VECTOR instruction for the 512-bit vector.
    B.buildInstr(AIE2::G_AIE_BROADCAST_VECTOR, {DstVec512BitReg}, {SrcReg});
    if (DstVecSize == 256) {
      const Register UnusedSubReg = MRI.createGenericVirtualRegister(DstVecTy);
      // Unmerge the 512-bit vector into the 256-bit destination vector.
      B.buildUnmerge({DstVecReg, UnusedSubReg}, DstVec512BitReg);
    } else if (DstVecSize == 1024) {
      // Concatenate two 512-bit vectors to form a 1024-bit destination vector.
      B.buildConcatVectors({DstVecReg}, {DstVec512BitReg, DstVec512BitReg});
    }
  }
  MI.eraseFromParent();
  return true;
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
  Register DstReg = MI.getOperand(0).getReg();
  Register SrcReg = MI.getOperand(MI.getNumDefs()).getReg();
  B.buildInstr(AIE2::G_AIE_UNPAD_VECTOR, {DstReg}, {SrcReg});
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

void llvm::applyPadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                          MachineIRBuilder &B, Register MatchedInputVector) {
  B.setInstrAndDebugLoc(MI);
  Register DstReg = MI.getOperand(0).getReg();
  B.buildInstr(AIE2::G_AIE_PAD_VECTOR_UNDEF, {DstReg}, {MatchedInputVector});
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
