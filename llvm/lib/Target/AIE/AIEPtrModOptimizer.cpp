//===--- AIEPtrModOptimizer.cpp - Optimizer ptr mod operations ------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Implements the llvm-pass AIEPtrModOptimizer to query Combiners
// (FoundCombiners).
//
//===----------------------------------------------------------------------===//

#include "AIEPtrModOptimizer.h"
#include "AIE.h"
#include "AIEBaseInstrInfo.h"
#include "AIEDataDependenceHelper.h"
#include "AIEGlobalCombiner.h"
#include "AIEGlobalCombinerPtrMods.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/CodeGen/GlobalISel/CSEInfo.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/ScheduleDAGInstrs.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/InitializePasses.h"
#include <memory.h>

#define DEBUG_TYPE "aie-ptr-mod-opt"

extern cl::opt<bool> EnableGlobalPtrModOptimizer;

using namespace llvm;

static const char AIE_PTR_MOD_OPTIMIZER[] = "AIE Pointer Modifier Optimization";

StringRef AIEPtrModOptimizer::getPassName() const {
  return AIE_PTR_MOD_OPTIMIZER;
}
namespace llvm {

bool AIEPtrModOptimizer::runOnMachineFunction(MachineFunction &MF) {
  PtrModRes = std::make_unique<AIE::FoundCombiners>(
      /*Analysis=*/true);

  MachineRegisterInfo &MRI = MF.getRegInfo();
  const auto *TII =
      static_cast<const AIEBaseInstrInfo *>(MF.getSubtarget().getInstrInfo());

  const MachineDominatorTree *MDT = &getAnalysis<MachineDominatorTreeWrapperPass>().getDomTree();
  const MachineLoopInfo *MLI =
      &getAnalysis<MachineLoopInfoWrapperPass>().getLI();

  MachineSchedContext Context;
  Context.MF = &MF;
  Context.AA = &getAnalysis<AAResultsWrapperPass>().getAAResults();

  // To build the edges in the DAG, the reserved Registers have to be freezed
  MRI.freezeReservedRegs();
  const bool AddMutators = false;
  const bool ExactLatencies = false;
  AIE::DataDependenceHelper DAG(Context, AddMutators, ExactLatencies);

  // Fixme: these combiners should be provided by tablegen
  std::vector<const AIE::GenericCombiner *> Combiners;
  auto OffsetCombiner = std::make_unique<AIE::OffsetCombiner>(&MRI, TII, MLI);
  Combiners.push_back(OffsetCombiner.get());
  auto PostInc = std::make_unique<AIE::PostIncCombiner>(&MRI, TII, MLI);
  Combiners.push_back(PostInc.get());
  AIE::AIEGlobalCombiner GlobalCombinerHelper(Combiners, *MDT, DAG, &MRI, TII);

  for (auto &MBB : MF) {
    LLVM_DEBUG(dbgs() << "\n\n\n New MBB:" << MBB.getName() << " ("
                      << MBB.getParent()->getName() << ")\n\n");

    std::vector<const AIE::GenericCombiner *> FoundCombiners =
        GlobalCombinerHelper.getCombiners(MBB);

    if (FoundCombiners.empty()) {
      LLVM_DEBUG(dbgs() << "[Global Ptr Inc] Skipping. No Combiners found!\n");
      continue;
    }

    LLVM_DEBUG(dbgs() << "\n[Solution] MBB : " << MBB.getName() << "\n");
    appendResult(FoundCombiners);
  }

  return false;
}

void AIEPtrModOptimizer::appendResult(
    std::vector<const AIE::GenericCombiner *> &Combiners) {

  for (auto *Combine : Combiners) {
    PtrModRes->append(Combine->CombinerData);
  }
}

void AIEPtrModOptimizer::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<MachineModuleInfoWrapperPass>();
  AU.addRequired<GISelCSEAnalysisWrapperPass>();
  AU.addRequired<TargetPassConfig>();
  AU.addRequired<MachineDominatorTreeWrapperPass>();
  AU.addPreserved<MachineDominatorTreeWrapperPass>();
  AU.addRequired<MachineLoopInfoWrapperPass>();
  AU.addPreserved<MachineLoopInfoWrapperPass>();
  AU.addRequired<AAResultsWrapperPass>();
  AU.setPreservesAll();
}

} // namespace llvm

namespace llvm::AIE {
void FoundCombiners::append(const AIE::Combiner &CombineResult) {
  assert(CombineResult.CombineRoot);
  InstrCombines[CombineResult.CombineRoot] = CombineResult;
  LLVM_DEBUG(dbgs() << "[Solution] "; CombineResult.dumpFull());
}

AIE::Combiner *FoundCombiners::getCombine(MachineInstr *CombineRoot) {
  auto It = find_if(
      InstrCombines,
      [&](const std::pair<MachineInstr *, AIE::Combiner> &CandidatePair) {
        const MachineInstr *CandidateI = CandidatePair.first;
        return CandidateI == CombineRoot;
      });

  if (It == InstrCombines.end())
    return nullptr;

  // remap instructions
  AIE::Combiner &Combiner = It->second;
  remapCombiner(Combiner);
  return &Combiner;
}

void FoundCombiners::createMapping(MachineInstr *ReplacedMI,
                                   MachineInstr *NewlyInsertedMI) {
  assert(NewlyInsertedMI != nullptr);
  assert(ReplacedMI != nullptr);
  ConvertedInstrs[ReplacedMI] = NewlyInsertedMI;
}

MachineInstr *FoundCombiners::getMappedInstr(MachineInstr *ReplacedMI) const {
  auto It = ConvertedInstrs.find(ReplacedMI);
  if (It == ConvertedInstrs.end())
    return nullptr;

  return It->second;
}

std::vector<MachineInstr *>
FoundCombiners::getRemappedInstrs(std::vector<MachineInstr *> &MIs) const {
  std::vector<MachineInstr *> FinalInstrs;
  for (auto *MI : MIs) {
    if (auto *Converted = getMappedInstr(MI))
      FinalInstrs.push_back(Converted);
    else
      FinalInstrs.push_back(MI);
  }
  return FinalInstrs;
}

void FoundCombiners::remapCombiner(AIE::Combiner &Combiner) const {
  Combiner.MoveUpInstrsToInsertionPoint =
      getRemappedInstrs(Combiner.MoveUpInstrsToInsertionPoint);
  Combiner.DelayInstrToInsertionPoint =
      getRemappedInstrs(Combiner.DelayInstrToInsertionPoint);
  Combiner.DelayInstrPastInsertionPoint =
      getRemappedInstrs(Combiner.DelayInstrPastInsertionPoint);

  std::vector<MachineInstr *> InsertionPointVec = {Combiner.InsertionPoint};
  Combiner.InsertionPoint = getRemappedInstrs(InsertionPointVec)[0];
}

const std::map<MachineInstr *, AIE::Combiner> &
FoundCombiners::getInstrCombines() const {
  return InstrCombines;
}

} // namespace llvm::AIE

raw_ostream &llvm::operator<<(raw_ostream &OS, const AIE::FoundCombiners &Val) {
  OS << "{ ";
  for (auto &MemPtr : Val.getInstrCombines()) {
    OS << *MemPtr.first;
  }
  OS << " }";
  return OS;
}

char AIEPtrModOptimizer::ID = 0;
INITIALIZE_PASS_BEGIN(AIEPtrModOptimizer, DEBUG_TYPE, AIE_PTR_MOD_OPTIMIZER,
                      false, false)
INITIALIZE_PASS_DEPENDENCY(GISelCSEAnalysisWrapperPass)
INITIALIZE_PASS_DEPENDENCY(MachineModuleInfoWrapperPass)
INITIALIZE_PASS_DEPENDENCY(TargetPassConfig)
INITIALIZE_PASS_END(AIEPtrModOptimizer, DEBUG_TYPE, AIE_PTR_MOD_OPTIMIZER,
                    false, false)

MachineFunctionPass *llvm::createAIEPtrModOptimizer() {
  return new AIEPtrModOptimizer();
}
