//===-- AIEHazardRecognizer.cpp - AIE Post RA Hazard Recognizer ---===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines the hazard recognizer for scheduling on AIE.
//
//===----------------------------------------------------------------------===//

#include "AIEHazardRecognizer.h"
#include "AIEFormatSelector.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/ResourceScoreboard.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/WithColor.h"
#include "llvm/Support/raw_ostream.h"
#include <cassert>
#include <optional>

using namespace llvm;

// FIXME: We currently use a default of 0 to bypass scoreboard checks in pre-RA
// scheduling. Once bottom-up scheduling is better handling late FuncUnit
// utilization, we might revisit that decision.
static llvm::cl::opt<int> PreRAFuncUnitDepth(
    "aie-prera-func-unit-depth", cl::Hidden, cl::init(0),
    cl::desc("Ignore FuncUnits past certain depth in pre-RA scoreboard"));

const AIEBaseMCFormats *FuncUnitWrapper::FormatInterface = nullptr;
void FuncUnitWrapper::setFormatInterface(const AIEBaseMCFormats *Formats) {
  FormatInterface = Formats;
}
void FuncUnitWrapper::dump() const {
  const char *const Digits = "0123456789";
  const char *const Spacer = "-|";
  const int Upper = std::numeric_limits<InstrStage::FuncUnits>::digits - 1;
  dbgs() << "Req     : ";
  for (int J = Upper; J >= 0; J--)
    dbgs() << ((Required & (1ULL << J)) ? Digits[J % 10] : Spacer[J % 10 == 0]);
  dbgs() << " Slots : ";
  for (int J = 9; J >= 0; J--)
    dbgs() << ((Slots & (1ULL << J)) ? Digits[J] : '-');
  if (!Reserved)
    return;
  dbgs() << "\n\t   Rsrv : ";
  for (int J = Upper; J >= 0; J--)
    dbgs() << ((Reserved & (1ULL << J)) ? Digits[J % 10] : Spacer[J % 10 == 0]);
}

void FuncUnitWrapper::clearResources() {
  IssueCount = 0;
  Required = 0;
  Reserved = 0;
  Slots = 0;
}
bool FuncUnitWrapper::isEmpty() const {
  return Required == 0 && Reserved == 0 && Slots == 0;
}

void FuncUnitWrapper::blockResources() {
  Required = ~0;
  Reserved = ~0;
  Slots = ~0;
}

FuncUnitWrapper &FuncUnitWrapper::operator|=(const FuncUnitWrapper &Other) {
  Required |= Other.Required;
  Reserved |= Other.Reserved;
  Slots |= Other.Slots;
  return *this;
}

bool FuncUnitWrapper::conflict(const FuncUnitWrapper &Other) const {
  if ((Required & Other.Required) != 0 || (Slots & Other.Slots) != 0 ||
      (Reserved & Other.Required) != 0 || (Required & Other.Reserved) != 0) {
    return true;
  }

  // Note: Don't check formats unless both have occupied slots.
  // This allows representing a blocked cycle (Slots = ~0) without knowing
  // the slot and format details.
  return Slots && Other.Slots &&
         !FormatInterface->getPacketFormats().getFormat(Slots | Other.Slots);
}

bool AIEResourceCycle::canReserveResources(MachineInstr &MI) {
  // This is optimistic, just trying to use available parallelism.
  // post ra sched will fix other resource conflicts.
  // TODO: Pipeliner only gives access to the issue cycle. We should
  // somehow know the adjacency of separate cycles. At that point
  // we can maintain a scoreboard.

  // Note : canAdd() can only be called with a "fixed-slot" instruction or
  // Target specific OpCode

  const std::vector<unsigned int> *AlternateInsts =
      Bundle.FormatInterface->getAlternateInstsOpcode(MI.getOpcode());

  if (!AlternateInsts)
    return Bundle.canAdd(&MI);

  return any_of(*AlternateInsts,
                [&](unsigned AltOpcode) { return Bundle.canAdd(AltOpcode); });
}

void AIEResourceCycle::reserveResources(MachineInstr &MI) {
  const std::vector<unsigned int> *AlternateInsts =
      Bundle.FormatInterface->getAlternateInstsOpcode(MI.getOpcode());

  if (!AlternateInsts)
    return Bundle.add(&MI);

  for (unsigned AltOpcode : *AlternateInsts) {
    if (Bundle.canAdd(AltOpcode)) {
      return Bundle.add(&MI, AltOpcode);
    }
  }
  llvm_unreachable("No alternative opcode can reserve resources");
}

// issue-limit:
// Setting it to one will generate sequential code.
// Higher values allow more instruction level parallellism
// Packetization will always happen and is unaware of this setting.
static cl::opt<int>
    CLIssueLimit("issue-limit", cl::init(6),
                 cl::desc("Issue limit for AIEHazardRecognizer"));

// This is a debugging option. It can be used to pinpoint the first
// instruction where VLIW introduces a bug by bisection.
// Useful upperbound for this bisection is around 6000, corresponding
// to 1024 bundles times 6 instructions per bundle at maximum density.
static cl::opt<int>
    MaxVLIWInstrs("vliw-instrs", cl::Hidden, cl::init(-1),
                  cl::desc("VLIW will switch off after scheduling this many "
                           "instructions in the last vliw region"));

#define DEBUG_TYPE "post-RA-sched"

int AIEHazardRecognizer::NumInstrsScheduled = 0;

AIEHazardRecognizer::AIEHazardRecognizer(const AIEBaseInstrInfo *TII,
                                         const InstrItineraryData *II,
                                         const ScheduleDAG *SchedDAG)
    : TII(TII), ItinData(II), CurrentBundle(TII->getFormatInterface()) {

  int Depth = computeScoreboardDepth();
  Scoreboard.reset(Depth);
  MaxLookAhead = Depth;
  if (CLIssueLimit > 0)
    IssueLimit = CLIssueLimit;

  // Switch off VLIW for every region after scheduling the specified
  // number of instructions
  if (MaxVLIWInstrs >= 0 && NumInstrsScheduled > MaxVLIWInstrs) {
    IssueLimit = 1;
  }
}

void AIEHazardRecognizer::StartBlock(MachineBasicBlock *MBB) {
  // NOTE: This callback is obsolesent
}

void AIEHazardRecognizer::EndBlock(MachineBasicBlock *MBB) {
  // NOTE: This callback is obsolesent

  // This runs after reordering the instruction list in MBB.
  // Now we can set the bundling attributes from the bundles we collected
  // without confusing the reordering.

  // Flushes unfinished bundles at the end of scheduling a basic block.
  // FIXME: Would be nicer if this was guaranteed by the scheduler itself.
  if (!CurrentBundle.empty()) {
    Bundles.emplace_back(CurrentBundle);
    CurrentBundle.clear();
  }

  applyBundles(Bundles, MBB);

  // Clean up BB-local stuff.
  Bundles.clear();
}

void AIEHazardRecognizer::applyBundles(
    const std::vector<AIE::MachineBundle> &Bundles, MachineBasicBlock *MBB) {
  for (auto B : Bundles) {
    LLVM_DEBUG(dbgs() << "---Bundle---\n");
    if (B.empty()) {
      // We have no real instructions. We don't need any bundling
      continue;
    }
    // Remove meta instructions
    for (auto *I : B.getMetaInstrs()) {
      MBB->remove(I);
    }

    // Find the iterator pointing AFTER the last bundle instruction.
    // That's where we will re-insert the slot-ordered instructions, as well as
    // meta instructions.
    MachineBasicBlock::iterator BundleEnd =
        std::next(B.getInstrs().back()->getIterator());

    // AIE1 does not always have formats for standalone instructions.
    // Do not re-order for such cases.
    if (B.size() > 1)
      applyFormatOrdering(B, *B.getFormatOrNull(), /*BundleRoot=*/nullptr,
                          BundleEnd);

    for (auto *I : B.getMetaInstrs()) {
      LLVM_DEBUG(dbgs() << "Meta " << *I);
      MBB->insert(BundleEnd, I);
    }
  }
}

bool AIEHazardRecognizer::currentCycleHasInstr(bool CountMetaInstrs) const {
  return !CurrentBundle.empty() ||
         (CountMetaInstrs && !CurrentBundle.MetaInstrs.empty());
}

void AIEHazardRecognizer::Reset() {
  LLVM_DEBUG(dbgs() << "Reset hazard recognizer\n");
  // Assuming this is the start of a new region. We should
  // flush the previous
  if (!CurrentBundle.empty()) {
    Bundles.emplace_back(CurrentBundle);
    CurrentBundle.clear();
  }
  ReservedCycles = 0;
  Scoreboard.reset();
  SelectedAltOpcodes.clear();
}

ScheduleHazardRecognizer::HazardType
AIEHazardRecognizer::getHazardType(SUnit *SU, int DeltaCycles) {
  MachineInstr *MI = SU->getInstr();
  if (CurrentBundle.isMetaInstruction(MI->getOpcode())) {
    LLVM_DEBUG(dbgs() << "Meta instruction\n");
    return NoHazard;
  }

  if (ReservedCycles && !MI->hasDelaySlot()) {
    LLVM_DEBUG(dbgs() << "Reserved cycle\n");
    return NoopHazard;
  }

  // TODO: In general, we should be able to rely on just the scoreboard,
  // CurrentBundle only applies to DeltaCycles=0:
  //   if (Scoreboard[DeltaCycles].IssueCount >= IssueLimit)
  // Currently, we don't always update the scoreboard, missing
  // some instructions there that *are* inserted in the bundle.
  // See AIECC-451.
  if (CurrentBundle.size() >= IssueLimit) {
    LLVM_DEBUG(dbgs() << "At issue limit\n");
    return NoopHazard;
  }

  std::optional<int> FUDepthLimit;
  if (!CurrentBundle.isPostRA(MI))
    FUDepthLimit = PreRAFuncUnitDepth;

  const std::vector<unsigned int> *AlternateInsts =
      CurrentBundle.FormatInterface->getAlternateInstsOpcode(MI->getOpcode());
  if (AlternateInsts) {
    for (const auto AltInstOpcode : *AlternateInsts) {
      // Check if the real instruction can be added in the current bundle
      if (!CurrentBundle.canAdd(AltInstOpcode))
        continue;
      ScheduleHazardRecognizer::HazardType Haz = getHazardType(
          TII->get(AltInstOpcode).getSchedClass(), DeltaCycles, FUDepthLimit);
      // Check if there is NoHazard, If there is a Hazard or NoopHazard check
      // for the next possible Opcode.
      if (Haz == NoHazard) {
        SelectedAltOpcodes[MI] = AltInstOpcode;
        return NoHazard;
      }
    }
    // In the above loop we are trying to find the best one where there is
    // NoHazard, if the loop is not able to find such case it will be a
    // NoopHazard only.
    return NoopHazard;
  }

  if (!CurrentBundle.canAdd(MI)) {
    LLVM_DEBUG(dbgs() << "Format hazard\n");
    return NoopHazard;
  }
  return getHazardType(MI->getDesc().getSchedClass(), DeltaCycles,
                       FUDepthLimit);
}

bool AIEHazardRecognizer::conflict(const AIEHazardRecognizer &Other,
                                   int DeltaCycles) const {
  return Scoreboard.conflict(Other.Scoreboard, DeltaCycles);
}

// This is called in two circumstances:
// 1) No instruction has its operands available and is ready
// 2) An instruction *is* ready to execute, but cannot execute
// due to an architecture hazard/resource contention.
void AIEHazardRecognizer::AdvanceCycle() {
  LLVM_DEBUG(dbgs() << "Advance cycle, clear state\n");
  Bundles.emplace_back(CurrentBundle);
  CurrentBundle.clear();
  if (ReservedCycles)
    --ReservedCycles;
  Scoreboard.advance();
}

// Similar to AdvanceCycle, but for bottom-up scheduling.
void AIEHazardRecognizer::RecedeCycle() {
  LLVM_DEBUG(dbgs() << "Recede cycle, clear state\n");
  Bundles.emplace_back(CurrentBundle);
  CurrentBundle.clear();
  if (ReservedCycles)
    --ReservedCycles;
  Scoreboard.recede();
}

void AIEHazardRecognizer::blockCycleInScoreboard(int DeltaCycles) {
  assert(Scoreboard.isValidDelta(DeltaCycles));
  Scoreboard[DeltaCycles].blockResources();
}

void AIEHazardRecognizer::recedeScoreboard(int N) {
  while (N--) {
    Scoreboard.recede();
  }
}
void AIEHazardRecognizer::dumpScoreboard() const { Scoreboard.dump(); }

unsigned AIEHazardRecognizer::PreEmitNoops(SUnit *SU) { return 0; }

void AIEHazardRecognizer::EmitInstruction(SUnit *SU) {
  return EmitInstruction(SU, 0);
}

void AIEHazardRecognizer::EmitInstruction(SUnit *SU, int DeltaCycles) {
  assert(DeltaCycles == 0);
  MachineInstr *MI = SU->getInstr();
  LLVM_DEBUG(dbgs() << "Emit Instruction: " << *MI);

  std::optional<int> FUDepthLimit;
  if (!CurrentBundle.isPostRA(MI))
    FUDepthLimit = PreRAFuncUnitDepth;

  // If the instruction has multiple options, find the opcode that was selected
  // and use the latter to update the scoreboard.
  unsigned SelectedOpcode = getSelectedAltOpcode(MI).value_or(MI->getOpcode());
  CurrentBundle.add(MI, SelectedOpcode);
  emitInScoreboard(TII->get(SelectedOpcode).getSchedClass(), DeltaCycles,
                   FUDepthLimit);

  // When requested, we switch off VLIW scheduling after the specified number
  // of instructions are scheduled.
  // Note that we construct new hazard recognizers for every region, so
  // we get a pristine issue limit for the next region.
  ++NumInstrsScheduled;
  if (MaxVLIWInstrs >= 0 && NumInstrsScheduled == MaxVLIWInstrs) {
    LLVM_DEBUG(
        dbgs() << "VLIW switched off after reaching instruction limit\n");
    IssueLimit = 1;
  }
}

void AIEHazardRecognizer::EmitNoop() {
  LLVM_DEBUG(dbgs() << "Emit Noop\n");
  // Scheduler will add a nop to the instruction stream when
  // emitting the schedule. It should only do so if the
  // current cycle is empty. We could just advance the scoreboard state,
  // but for consistency we also force an empty bundle.
  AdvanceCycle();
}

void AIEHazardRecognizer::setReservedCycles(unsigned Cycles) {
  this->ReservedCycles = Cycles;
}

// These functions interpret the itinerary, translating InstrStages
// to ResourceCycles to apply.
// We deviate from the standard ScoreboardHazardRecognizer by not
// recognising alternatives
//
ScheduleHazardRecognizer::HazardType
AIEHazardRecognizer::getHazardType(unsigned SchedClass, const int DeltaCycles,
                                   std::optional<int> FUDepthLimit) {
  assert(Scoreboard.isValidDelta(DeltaCycles));
  // Note that Delta will be negative for bottom-up scheduling.
  // Cycle is 'our' cycle at which each stage of the itinerary starts.
  // It gets updated by the increment from the InstrStage.
  int Cycle = DeltaCycles;
  for (const InstrStage &IS : ItinData->getStages(SchedClass)) {
    if (FUDepthLimit && (Cycle - DeltaCycles) >= *FUDepthLimit) {
      break;
    }
    // Check availability of this stage's resources for the specified number
    // of cycles
    const FuncUnitWrapper ThisCycle(IS);
    for (unsigned int C = 0; C < IS.getCycles(); ++C) {
      int StageCycle = Cycle + (int)C;
      assert(StageCycle < Scoreboard.getDepth());

      if (ThisCycle.conflict(Scoreboard[StageCycle])) {
        LLVM_DEBUG(dbgs() << "*** Hazard in execution cycle"
                          << StageCycle - DeltaCycles << ", ");
        return ScheduleHazardRecognizer::NoopHazard;
      }
    }

    // Advance the cycle to the next stage.
    Cycle += IS.getNextCycles();
  }

  return ScheduleHazardRecognizer::NoHazard;
}

void AIEHazardRecognizer::emitInScoreboard(unsigned SchedClass, int DeltaCycles,
                                           std::optional<int> FUDepthLimit) {
  assert(Scoreboard.isValidDelta(DeltaCycles));

  int Cycle = DeltaCycles;
  Scoreboard[Cycle].IssueCount++;
  for (const InstrStage &IS : ItinData->getStages(SchedClass)) {
    if (FUDepthLimit && (Cycle - DeltaCycles) >= *FUDepthLimit) {
      break;
    }
    const FuncUnitWrapper ThisCycle(IS);
    for (unsigned int C = 0; C < IS.getCycles(); ++C) {
      Scoreboard[Cycle + C] |= ThisCycle;
    }

    // Advance the cycle to the next stage.
    Cycle += IS.getNextCycles();
  }

  LLVM_DEBUG({
    dbgs() << "Scoreboard:\n";
    Scoreboard.dump();
  });
}

unsigned AIEHazardRecognizer::computeScoreboardDepth() const {
  assert(ItinData && !ItinData->isEmpty());
  unsigned ItinDepth = 0;
  for (unsigned SchedClass = 0; !ItinData->isEndMarker(SchedClass);
       ++SchedClass) {
    unsigned CurCycle = 0;
    for (const InstrStage &IS : ItinData->getStages(SchedClass)) {
      ItinDepth = std::max(ItinDepth, CurCycle + IS.getCycles());
      CurCycle += IS.getNextCycles();
    }
  }
  if (ItinDepth == 0) {
    return 0;
  }

  // Find the next power-of-2 >= ItinDepth
  unsigned ScoreboardDepth = 1;
  while (ItinDepth > ScoreboardDepth) {
    ScoreboardDepth *= 2;
  }
  return ScoreboardDepth;
}

std::optional<unsigned>
AIEHazardRecognizer::getSelectedAltOpcode(MachineInstr *MI) const {
  if (auto It = SelectedAltOpcodes.find(MI); It != SelectedAltOpcodes.end())
    return It->second;
  return std::nullopt;
}
