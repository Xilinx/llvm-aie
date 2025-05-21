//===--- AIEGlobalCombiner.cpp - Global Combiner Helper Interface ---------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Implements the generic algorithmic parts of the global combiner search.
//
//===----------------------------------------------------------------------===//

#include "AIEGlobalCombiner.h"
#include "AIECombinerHelper.h"

#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/IR/IntrinsicsAIE2P.h"
#include "llvm/Support/Debug.h"
#include <memory>
#include <queue>
namespace llvm::AIE {
using std::distance;

#define DEBUG_TYPE "global-combiner"

static cl::opt<int> MaxSearchIterationCount("global-combiner-max-search-iter",
                                            cl::Hidden, cl::init(100000),
                                            cl::desc("Maximum Search Tries."));

std::vector<const GenericCombiner *>
AIEGlobalCombiner::getCombiners(MachineBasicBlock &MBB) {
  generateCombiners(MBB);

  std::vector<const GenericCombiner *> Combiners = findBeneficialCombiners();

  FixedCombiners.insert(FixedCombiners.end(), Combiners.begin(),
                        Combiners.end());
  return Combiners;
}

void AIEGlobalCombiner::generateCombiners(MachineBasicBlock &MBB) {
  clear();

  GlobalCombiner::initDAG(DAG, MBB);

  for (auto &SUnit : DAG.SUnits) {

    for (auto *Combiner : CombinerOptions) {
      if (!Combiner->isCombineRootCandidate(SUnit.getInstr()))
        continue;

      calculateCombineCandidates(SUnit, Combiner);
    }
  }

  calculateCombinerConflicts();
}

std::vector<const GenericCombiner *>
AIEGlobalCombiner::findBeneficialCombiners() {
  std::vector<const GenericCombiner *> FoundCombiners;

  for (auto CombineCandidates : getCombineCandidates(ClusteredCombiners)) {
    LLVM_DEBUG(dbgs() << "Next Cluster Start\n");

    CombineCandidates.filterOut(FixedCombiners);
    CombineCandidates.filterOut(FoundCombiners);

    for (auto *Combiner :
         CombineCandidates.searchCombinerSet(OwnedCombineCandidates))
      FoundCombiners.push_back(Combiner);
  }

  LLVM_DEBUG(dbgs() << "[Global Combiner] Found " << FoundCombiners.size()
                    << " Fixed Combiners\n\n");

  return FoundCombiners;
}

void AIEGlobalCombiner::calculateCombineCandidates(
    SUnit &CombineRoot, const GenericCombiner *Combiner) {
  assert(MDT);

  LLVM_DEBUG(dbgs() << "  [" << Combiner->getName() << "] "
                    << *CombineRoot.getInstr());

  for (std::unique_ptr<GenericCombiner> &Combiner :
       Combiner->applyCombiner(CombineRoot, *MDT, DAG, *MRI, *TII)) {
    Combiner->setGlobalID(OwnedCombineCandidates.size());
    OwnedCombineCandidates.push_back(std::move(Combiner));

    GenericCombiner *CurrentCombiner = OwnedCombineCandidates.back().get();
    const auto BaseRegister = getClusterBaseRegister(*CurrentCombiner);
    ClusteredCombiners[BaseRegister].append(CurrentCombiner);
  }
  LLVM_DEBUG(dbgs() << "\n");
}

void AIEGlobalCombiner::calculateCombinerConflicts() {
  for (unsigned Idx = 0; Idx < OwnedCombineCandidates.size(); Idx++) {
    assert(Idx == OwnedCombineCandidates[Idx].get()->getGlobalID());
    OwnedCombineCandidates[Idx]->setConflicts(OwnedCombineCandidates);
  }
}

std::vector<std::unique_ptr<GenericCombiner>> GenericCombiner::applyCombiner(
    SUnit &CombineRootSUnit, const MachineDominatorTree &MDT,
    const AIE::DataDependenceHelper &DAG, const MachineRegisterInfo &MRI,
    const AIEBaseInstrInfo &TII) const {
  std::vector<std::unique_ptr<GenericCombiner>> AppliedCombiners;
  CombinerGain ZeroGain;

  for (auto *DefI : getCombineCandidates(CombineRootSUnit.getInstr(), DAG)) {
    if (!DefI) {
      LLVM_DEBUG(dbgs() << "  [GCombiner] Skipping: Could not get DefInstr for "
                        << *CombineRootSUnit.getInstr());
      continue;
    }

    if (isTriviallyDead(*DefI, MRI)) {
      LLVM_DEBUG(dbgs() << "  [GCombiner] Skipping: Trivially dead " << *DefI);
      continue;
    }

    std::unique_ptr<GenericCombiner> Combiner = this->clone();
    const bool SetupSuccess =
        Combiner->setupCombiner({DefI, CombineRootSUnit.getInstr()}, &DAG);
    if (!SetupSuccess) {
      LLVM_DEBUG(dbgs() << "  [GCombiner] Skipping; Could not setup Combiner "
                        << *DefI << "\n");
      continue;
    }

    if (!Combiner->tryToSetCombinedOpCode()) {
      LLVM_DEBUG(dbgs() << "      [GCombiner] Opcode is not valid for " << *DefI
                        << "\n");
      continue;
    }

    if (!Combiner->canReorderCombineInstrs(DAG, MRI, TII)) {
      LLVM_DEBUG(dbgs() << "      [GCombiner] Could not move Uses after "
                        << *DefI << "\n");
      continue;
    }
    Combiner->adjustGain(MDT);
    if (Combiner->getGain() < ZeroGain) {
      LLVM_DEBUG(dbgs() << "      [GCombiner] Negative Gain for " << *DefI
                        << "\n");
      continue;
    }

    LLVM_DEBUG(dbgs() << "  [GCombiner] Found Combiner: ";
               Combiner->dumpFull(););
    AppliedCombiners.push_back(std::move(Combiner));
  }
  return AppliedCombiners;
}

Register AIEGlobalCombiner::getClusterBaseRegister(GenericCombiner &Combiner) {
  std::optional<Register> NewBaseReg;
  for (auto &[BaseReg, OverlappingRegs] : BaseRegisterMap) {
    for (const auto &Reg : Combiner.getClusterRegs())
      if (find(OverlappingRegs, Reg) != OverlappingRegs.end()) {
        NewBaseReg = BaseReg;
        break;
      }
    if (NewBaseReg)
      break;
  }

  if (!NewBaseReg) {
    NewBaseReg = Combiner.getClusterRegs()[0];
    BaseRegisterMap[*NewBaseReg] = {};
  }
  std::vector<Register> &OverlappingRegs = BaseRegisterMap[*NewBaseReg];

  for (const auto &Reg : Combiner.getClusterRegs())
    if (find(OverlappingRegs, Reg) == OverlappingRegs.end())
      OverlappingRegs.push_back(Reg);

  return *NewBaseReg;
}

void AIEGlobalCombiner::clear() { ClusteredCombiners.clear(); }

std::vector<CombineCandidates> AIEGlobalCombiner::getCombineCandidates(
    std::map<Register, CombineCandidates> ClusteredCombiners) {
  std::vector<CombineCandidates> Result;
  for (auto Candidate : ClusteredCombiners) {
    Result.push_back(Candidate.second);
  }

  return Result;
}

// -------------------------- CombineCandidates ------------------------------//

std::vector<const GenericCombiner *> CombineCandidates::searchCombinerSet(
    const std::vector<std::unique_ptr<GenericCombiner>>
        &OwnedCombineCandidates) {
  if (Combiners.empty())
    return {};

  const auto *MBB = Combiners[0]->CombinerData.CombineRoot->getParent();
  const unsigned NumCombiner = Combiners[0]->getConflicts().size();
  LLVM_DEBUG(dbgs() << MBB->getName() << " - Combiner Search Start \n");

  // seed greedy solution
  auto BestSolution = getGreedySolution();

  std::vector<const GenericCombiner *> Result;
  BitVector CombinerBitVec = BestSolution.getCombinersBitVector();
  for (int Idx = CombinerBitVec.find_first(); Idx != -1;
       Idx = CombinerBitVec.find_next(Idx)) {
    Result.push_back(Combiners[Idx]);
  }
  return Result;
}

void CombineCandidates::append(GenericCombiner *Combiner) {
  Combiners.push_back(Combiner);
}

CombinerSolution CombineCandidates::getGreedySolution() const {
  unsigned NumCombiner = Combiners[0]->getConflicts().size();

  BitVector Conflicts(NumCombiner);
  CombinerSolution GreedySolution(NumCombiner);
  CombinerGain ZeroGain;

  for (unsigned Idx = 0; Idx < Combiners.size(); Idx++) {
    const auto *Combiner = Combiners[Idx];
    if (Combiner->hasConflict(Conflicts))
      continue;

    if (Combiner->getOverlapGain(GreedySolution.getCombinersBitVector(),
                                 Combiners) <= ZeroGain)
      continue;

    LLVM_DEBUG(dbgs() << "[Greedy] Added " << *Combiner << "\n");
    GreedySolution.add(Combiner, Idx, Combiners);
    Conflicts |= Combiner->getConflicts();
  }

  GreedySolution.setIndex(Combiners.size());
  return GreedySolution;
}

CombinerGain
CombineCandidates::getMaxPotentialGain(const CombinerSolution &Current,
                                       const unsigned Index) const {
  CombinerGain ZeroGain;
  CombinerGain NextGain(Current.getGain());
  for (unsigned Idx = Index; Idx < Combiners.size(); Idx++) {
    GenericCombiner *Combiner = Combiners[Idx];
    if (Current.hasConflict(Combiner))
      continue;

    auto OverlapGain =
        Combiner->getOverlapGain(Current.getCombinersBitVector(), Combiners);
    if (OverlapGain > ZeroGain)
      NextGain += OverlapGain;
  }

  return NextGain;
}

void CombineCandidates::filterOut(
    const std::vector<const GenericCombiner *> &UsedCombiners) {
  for (const auto &Combiner : UsedCombiners) {
    auto HasConflict = [&](const GenericCombiner *Obj) {
      return Combiner->hasConflict(/*ConflictVector=*/Obj->getConflicts());
    };
    erase_if(Combiners, HasConflict);
  }

  LLVM_DEBUG(dbgs() << "  [Search] After Filtering left with "
                    << Combiners.size() << " Options\n");
}

// -------------------------- CombineResult ----------------------------------//

bool GenericCombiner::canReorderCombineInstrs(
    const AIE::DataDependenceHelper &DAG, const MachineRegisterInfo &MRI,
    const AIEBaseInstrInfo &TII) {
  assert(CombinerData.CombineInstrs.size() > 0);
  auto ReorderInstrs = [](std::set<SUnit *> &ToMove) {
    std::vector<SUnit *> ToMoveVec;

    for (SUnit *Candidate : ToMove) {
      ToMoveVec.push_back(Candidate);
    }

    // maintain dependencies between SUnits by keeping the ordering of the MBB
    sort(ToMoveVec, [](const SUnit *A, const SUnit *B) {
      return A->NodeNum < B->NodeNum;
    });

    std::vector<MachineInstr *> OrderedMIs;
    for (auto *SUnit : ToMoveVec) {
      OrderedMIs.push_back(SUnit->getInstr());
    }

    return OrderedMIs;
  };

  auto MoveAbleInstrs = getInstructionsToMove(DAG);
  if (!MoveAbleInstrs)
    return false;

  const unsigned InsertionPointDepth =
      DAG.getSUnit(CombinerData.InsertionPoint)->getDepth();

  auto [MoveUp, MoveDown] = *MoveAbleInstrs;

  for (SUnit *MoveDownCandidate : MoveDown) {
    std::set<SUnit *> ToMove;
    if (!isMovable(*MoveDownCandidate, InsertionPointDepth, DAG,
                   /*MoveDown=*/true, ToMove))
      return false;
    CombinerData.DelayInstrPastInsertionPoint = ReorderInstrs(ToMove);
  }

  for (SUnit *MoveUpCandidate : MoveUp) {
    std::set<SUnit *> ToMove;
    if (!isMovable(*MoveUpCandidate, InsertionPointDepth, DAG,
                   /*MoveDown=*/false, ToMove))
      return false;
    CombinerData.MoveUpInstrsToInsertionPoint = ReorderInstrs(ToMove);
  }

  return true;
}

std::optional<std::pair<std::vector<SUnit *>, std::vector<SUnit *>>>
GenericCombiner::getInstructionsToMove(const AIE::DataDependenceHelper &DAG) {
  assert(InsertionPointNodeNum != (unsigned)-1);
  std::vector<SUnit *> MoveUp;
  std::vector<SUnit *> MoveDown;

  for (auto *MI : CombinerData.CombineInstrs) {
    if (MI == CombinerData.InsertionPoint)
      continue;

    auto *SUnit = DAG.getSUnit(MI);
    if (!SUnit)
      // Instruction is in a different MBB.
      // Fixme: properly check if MI can be moved across MBB borders
      return {};

    const auto MINodeNum = SUnit->NodeNum;
    if (MINodeNum > InsertionPointNodeNum) {
      LLVM_DEBUG(dbgs() << "Check Move up " << *MI);
      MoveUp.push_back(SUnit);
    } else {
      LLVM_DEBUG(dbgs() << "Check Move down " << *MI);
      MoveDown.push_back(SUnit);
    }
  }
  return {{MoveUp, MoveDown}};
}

bool GenericCombiner::isMovable(SUnit &Candidate,
                                const unsigned InsertionPointDepth,
                                const AIE::DataDependenceHelper &DAG,
                                const bool MoveDown,
                                std::set<SUnit *> &ToMove) {

  auto GetNext = [MoveDown](SUnit &Candidate) {
    if (MoveDown) {
      return Candidate.Succs;
    }

    return Candidate.Preds;
  };

  for (auto &Dep : GetNext(Candidate)) {
    MachineInstr *DepI = Dep.getSUnit()->getInstr();
    if (DepI == CombinerData.InsertionPoint) {
      LLVM_DEBUG(dbgs() << "[Movability Check] Dependency to Insertion Point, "
                           "skipping Combiner "
                        << *DepI);
      return false;
    }

    if (!canMove(Dep.getSUnit(), MoveDown)) {
      LLVM_DEBUG(
          dbgs()
          << "[Movability Check] Combiner does not know how to handle move, "
             "skipping Combiner "
          << *DepI);
      return false;
    }

    if (ToMove.count(Dep.getSUnit())) {
      LLVM_DEBUG(dbgs() << "[Movability Check] Already encountered, no need "
                           "to continue movability check for "
                        << *DepI);
      continue;
    }

    ToMove.emplace(Dep.getSUnit());
    LLVM_DEBUG(dbgs() << "[Movability Check] Adding " << *DepI);
    const bool Movable =
        isMovable(*Dep.getSUnit(), InsertionPointDepth, DAG, MoveDown, ToMove);
    if (!Movable)
      return false;
  }
  return true;
}

void GenericCombiner::setConflicts(
    std::vector<std::unique_ptr<GenericCombiner>> &AllCombiners) {
  LLVM_DEBUG(dbgs() << "Conflict check " << *this << "\n");
  ConflictCombiners.resize(AllCombiners.size());

  for (unsigned Idx = 0; Idx < AllCombiners.size(); Idx++) {
    if (hasConflict(*AllCombiners[Idx]))
      ConflictCombiners.set(Idx);
  }
}

unsigned GenericCombiner::getGlobalID() const { return GlobalID; }

void GenericCombiner::setGlobalID(unsigned GlobalID) {
  this->GlobalID = GlobalID;
}

/// \return whether a Combiner is used after a Remove-Combiner, that
/// are part of the same Cluster. The Ordering of the Combiners \p A and \p B is
/// irrelevant.
static bool hasUseAfterRemoval(const GenericCombiner &A,
                               const GenericCombiner &B, const unsigned AIdx,
                               const MachineInstr *MI) {
  auto FindIndex = [](const GenericCombiner &Combiner, const MachineInstr *MI) {
    // get Index of MI in CombineInstrs
    auto It = find(Combiner.CombinerData.CombineInstrs, MI);
    if (It == Combiner.CombinerData.CombineInstrs.end())
      return -1;

    return (int)std::distance(Combiner.CombinerData.CombineInstrs.begin(), It);
  };

  // early exit, if Combiners don't exactly overlap
  for (auto &Reg : A.getClusterRegs()) {
    if (find(B.getClusterRegs(), Reg) == B.getClusterRegs().end())
      return false;
  }

  int BIdx = FindIndex(B, MI);
  if (BIdx == -1)
    return false;

  if (!A.CombinerData.RemoveInstrs[AIdx] && !B.CombinerData.RemoveInstrs[BIdx])
    return false;

  if (A.CombinerData.RemoveInstrs[AIdx] && B.CombinerData.RemoveInstrs[BIdx])
    return true;

  auto RemovalInsertionPointNodeNum = A.InsertionPointNodeNum;
  auto NonRemovalInsertionPointNodeNum = B.InsertionPointNodeNum;
  if (B.CombinerData.RemoveInstrs[BIdx]) {
    // B is the removal Combiner, swap Node Nums
    RemovalInsertionPointNodeNum = B.InsertionPointNodeNum;
    NonRemovalInsertionPointNodeNum = A.InsertionPointNodeNum;
  }

  return NonRemovalInsertionPointNodeNum > RemovalInsertionPointNodeNum;
}

bool GenericCombiner::hasConflict(const GenericCombiner &Combiner) const {
  for (unsigned Idx = 0; Idx < Combiner.CombinerData.CombineInstrs.size();
       Idx++) {
    auto *MI = Combiner.CombinerData.CombineInstrs[Idx];
    if (isCombineRoot(MI) ||
        (contains(*MI) && hasUseAfterRemoval(*this, Combiner, Idx, MI))) {
      LLVM_DEBUG(dbgs() << "    [Conflict] " << *this << " " << Combiner
                        << "\n");
      return true;
    }
  }

  return false;
}

bool GenericCombiner::hasConflict(const BitVector &ConflictVector) const {
  return ConflictVector.test(GlobalID);
}

CombinerGain GenericCombiner::getOverlapGain(
    const BitVector &AlreadyUsedCombiners,
    const std::vector<GenericCombiner *> &AllCombiners) const {
  std::vector<APInt> MaterializedImmCopies;

  auto AddImmRegUses = [&MaterializedImmCopies](
                           const GenericCombiner *Combiner) {
    assert(Combiner);
    for (auto ImmInReg : Combiner->getAllImmInRegs()) {
      if (find(MaterializedImmCopies, ImmInReg) == MaterializedImmCopies.end())
        MaterializedImmCopies.push_back(ImmInReg);
    }
  };

  CombinerGain OverLapGain(getGain());
  for (int Idx = AlreadyUsedCombiners.find_first(); Idx != -1;
       Idx = AlreadyUsedCombiners.find_next(Idx)) {
    const GenericCombiner *UsedCombiner = AllCombiners.at(Idx);
    AddImmRegUses(UsedCombiner);
    OverLapGain -= getOverlapPenalty(UsedCombiner);
  }

  OverLapGain += getImmediateReuseGain(MaterializedImmCopies);

  return OverLapGain;
}

unsigned GenericCombiner::getCombinedOpCode() const {
  return CombinerData.CombinedInstrOpcode;
}

bool GenericCombiner::contains(MachineInstr &MI) const {
  auto It =
      find_if(CombinerData.CombineInstrs, [&MI](const MachineInstr *CombineMI) {
        return CombineMI == &MI;
      });

  return It != CombinerData.CombineInstrs.end();
}

bool GenericCombiner::isCombineRoot(const MachineInstr *MI) const {
  return CombinerData.CombineRoot == MI;
}

void GenericCombiner::dumpFull() const {
  dbgs() << *this << "\n";
  CombinerData.dumpFull();
}

raw_ostream &operator<<(raw_ostream &OS, const GenericCombiner &Val) {
  OS << Val.getName() << "[" << Val.getGlobalID() << "]"
     << " (";
  for (auto *MI : Val.CombinerData.CombineInstrs) {
    OS << MI->getOperand(0) << ",";
  }
  OS << ") ";
  OS << Val.getGain() << " ";

  return OS;
}

// -------------------------- CombinerGain -----------------------------------//

CombinerGain::CombinerGain(std::initializer_list<int> InitialGain) {
  assert(InitialGain.size() <= GainVector.size());
  reset();

  std::copy(InitialGain.begin(), InitialGain.end(), GainVector.begin());
}

void CombinerGain::reset() {
  std::fill(GainVector.begin(), GainVector.end(), 0);
}

bool CombinerGain::operator>(const CombinerGain &Rhs) const {
  return Rhs < *this;
}

bool CombinerGain::operator<=(const CombinerGain &Rhs) const {
  return !(*this > Rhs);
}

bool CombinerGain::operator<(const CombinerGain &Rhs) const {
  assert(Rhs.GainVector.size() == GainVector.size());
  return GainVector < Rhs.GainVector;
}

bool CombinerGain::operator==(const CombinerGain &Rhs) const {
  assert(Rhs.GainVector.size() == GainVector.size());
  return GainVector == Rhs.GainVector;
}

bool CombinerGain::operator!=(const CombinerGain &Rhs) const {
  assert(Rhs.GainVector.size() == GainVector.size());
  return !(*this == Rhs);
}

CombinerGain &CombinerGain::operator+=(const CombinerGain &Rhs) {
  for (unsigned Idx = 0; Idx < GainVector.size(); Idx++) {
    GainVector[Idx] += Rhs.GainVector[Idx];
  }
  return *this;
}

CombinerGain CombinerGain::operator+(const CombinerGain &Rhs) const {
  CombinerGain Result(*this);
  return Result += Rhs;
}

CombinerGain &CombinerGain::operator-=(const CombinerGain &Rhs) {
  for (unsigned Idx = 0; Idx < GainVector.size(); Idx++) {
    GainVector[Idx] -= Rhs.GainVector[Idx];
  }
  return *this;
}

CombinerGain CombinerGain::operator-(const CombinerGain &Rhs) const {
  CombinerGain Result(*this);
  return Result -= Rhs;
}

// -------------------------- CombinerSolution -------------------------------//

CombinerSolution::CombinerSolution(const unsigned NumCombiners)
    : Index(0), Combiners(NumCombiners), ConflictCombiners(NumCombiners) {}

CombinerSolution::CombinerSolution(
    const CombinerSolution &Other, const GenericCombiner *Combiner,
    const CombinerGain &MaxFutureGain, const unsigned Idx,
    const std::vector<GenericCombiner *> &CombinerSubSet)
    : CombinerSolution(Other) {

  Index++;
  this->MaxFutureGain = MaxFutureGain;
  add(Combiner, Idx, CombinerSubSet);
}

void CombinerSolution::add(
    const GenericCombiner *Combiner, const unsigned Idx,
    const std::vector<GenericCombiner *> &CombinerSubSet) {
  if (!Combiner)
    return;

  Gain += Combiner->getOverlapGain(Combiners, CombinerSubSet);
  Combiners.set(Idx);
  ConflictCombiners |= Combiner->getConflicts();
  assert(Combiners.size() == ConflictCombiners.size());
}

void CombinerSolution::remove(const int Idx) { Combiners.reset(Idx); }

void CombinerSolution::recalculateGain(
    const std::vector<GenericCombiner *> &AllCombiners) {
  Gain.reset();
  for (int Idx = Combiners.find_first(); Idx != -1;
       Idx = Combiners.find_next(Idx)) {
    Gain += AllCombiners[Idx]->getOverlapGain(Combiners, AllCombiners);
  }
}

const BitVector &CombinerSolution::getCombinersBitVector() const {
  return Combiners;
}

bool CombinerSolution::hasConflict(const GenericCombiner *Combiner) const {
  return ConflictCombiners[Combiner->getGlobalID()];
}

bool CombinerSolution::operator<(const CombinerSolution &Other) const {
  if (MaxFutureGain == Other.MaxFutureGain)
    // If Potential Future Gains are equal, sort by remaining search steps
    return Index < Other.Index;

  return MaxFutureGain < Other.MaxFutureGain;
}

bool CombinerSolution::operator==(const CombinerSolution &Other) const {
  return Index == Other.Index && Combiners == Other.Combiners;
}

// --------------------------- Combiner --------------------------------------//

Combiner::Combiner(std::vector<MachineInstr *> CombineInstrs,
                   unsigned CombinedInstrOpcode, MachineInstr *InsertionPoint,
                   MachineInstr *CombineRoot,
                   std::vector<MachineInstr *> MoveUpInstrsToCombineRoot,
                   BitVector RemoveInstrs, StringRef Name)
    : CombineRoot(CombineRoot), InsertionPoint(InsertionPoint),
      CombineInstrs(CombineInstrs),
      MoveUpInstrsToInsertionPoint(MoveUpInstrsToCombineRoot),
      CombinedInstrOpcode(CombinedInstrOpcode), RemoveInstrs(RemoveInstrs),
      Name(Name) {}

void Combiner::dumpFull() const { dumpFull(nullptr, nullptr); }

void Combiner::dumpFull(unsigned *GlobalID, CombinerGain *Gain) const {
  if (!GlobalID || !Gain)
    dbgs() << "{Combiner " << Name << "\n";
  else
    dbgs() << "{Combiner " << Name << " [" << *GlobalID << "]; Gain = " << *Gain
           << "\n";

  for (auto *MI : CombineInstrs) {
    dbgs() << "  " << *MI;
  }
  dbgs() << "    Insertion Point: ";
  dbgs() << "              " << *InsertionPoint;

  if (!MoveUpInstrsToInsertionPoint.empty()) {
    dbgs() << "    to Move Up: \n";
    for (const auto *MI : MoveUpInstrsToInsertionPoint)
      dbgs() << "              " << *MI;
  }
  if (!DelayInstrToInsertionPoint.empty()) {
    dbgs() << "    Move to Insertion: \n";
    for (const auto *MI : DelayInstrToInsertionPoint)
      dbgs() << "                  " << *MI;
  }
  if (!DelayInstrPastInsertionPoint.empty()) {
    dbgs() << "    Delay past Insertion: \n";
    for (const auto *MI : DelayInstrPastInsertionPoint)
      dbgs() << "                          " << *MI;
  }
  dbgs() << "}\n";
}

// ---------------------------------------------------------------------------//

void GlobalCombiner::initDAG(AIE::DataDependenceHelper &DAG,
                             MachineBasicBlock &MBB) {
  DAG.clearDAG();
  for (auto &MI : MBB) {
    if (!MI.isTerminator()) {
      DAG.initSUnit(MI);
    }
  }
  DAG.buildEdges();
  DAG.makeMaps();
}

raw_ostream &operator<<(raw_ostream &OS, const CombinerGain &Val) {
  OS << "[";
  for (auto &Gain : Val.GainVector) {
    OS << Gain << ",";
  }
  OS << "]";
  return OS;
}

raw_ostream &operator<<(raw_ostream &OS, const Combiner &Val) {
  OS << Val.Name << " (";
  for (auto *MI : Val.CombineInstrs) {
    OS << MI->getOperand(0) << ",";
  }
  OS << ") ";
  return OS;
}

raw_ostream &operator<<(raw_ostream &OS, const CombinerSolution &Val) {
  OS << "[" << Val.getIndex() << "]"; //=" << Val.getScore() << " ";
  BitVector CombinerVector = Val.getCombinersBitVector();
  for (int Idx = CombinerVector.find_first(); Idx != -1;
       Idx = CombinerVector.find_next(Idx)) {
    dbgs() << Idx << " ";
  }
  LLVM_DEBUG(dbgs() << Val.getGain());

  return OS;
}

} // namespace llvm::AIE
