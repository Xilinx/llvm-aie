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
#include "AIEBaseSubtarget.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/ResourceScoreboard.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/WithColor.h"
#include "llvm/Support/raw_ostream.h"
#include <cassert>
#include <limits>
#include <optional>

using namespace llvm;

static llvm::cl::opt<int> PreMISchedFUDepth(
    "aie-premisched-fu-depth", cl::Hidden, cl::init(16),
    cl::desc("Ignore FuncUnits past certain depth in pre-RA scoreboard"));
static llvm::cl::opt<bool> PreMISchedIgnoreUnknownSlots(
    "aie-premisched-ignore-unknown-slots", cl::Hidden, cl::init(true),
    cl::desc("Do not block a whole cycle for unknown-slot instructions"));

static cl::opt<unsigned>
    UserScoreboardDepth("aie-scoreboard-depth", cl::init(128),
                        cl::desc("Override maximum scoreboard depth to use."));

static cl::opt<bool> AddressSpaceNoneIsSafe(
    "aie-addrspace-none-is-safe", cl::Hidden, cl::init(false),
    cl::desc("Assume that addrspace(0) doesn't cause conflicts."));

const AIEBaseMCFormats *FuncUnitWrapper::FormatInterface = nullptr;
void FuncUnitWrapper::setFormatInterface(const AIEBaseMCFormats *Formats) {
  FormatInterface = Formats;
}

bool FuncUnitWrapper::operator==(const FuncUnitWrapper &Other) const {
  return Required == Other.Required && Reserved == Other.Reserved &&
         Slots == Other.Slots && MemoryBanks == Other.MemoryBanks;
}

void FuncUnitWrapper::dump() const {
  const char *const Digits = "0123456789";
  const char *const Spacer = "-|";
  const int Upper = std::numeric_limits<InstrStage::FuncUnits>::digits - 1;

  auto PrintFU = [&](const std::string &FUName, InstrStage::FuncUnits FU) {
    dbgs() << FUName;
    for (int J = Upper; J >= 0; J--)
      dbgs() << ((FU & (1ULL << J)) ? Digits[J % 10] : Spacer[J % 10 == 0]);
  };
  auto PrintResource = [&](const std::string &ResourceName, uint64_t Resource) {
    dbgs() << ResourceName;
    for (int J = 9; J >= 0; J--)
      dbgs() << ((Resource & (1ULL << J)) ? Digits[J] : '-');
  };

  PrintFU("Req     : ", Required);
  PrintResource(" Slots : ", Slots);
  PrintResource(" Memorybanks : ", MemoryBanks);
  if (!Reserved)
    return;
  PrintFU("\n\t   Rsrv : ", Reserved);
}

void FuncUnitWrapper::clearResources() {
  IssueCount = 0;
  Required = 0;
  Reserved = 0;
  Slots = 0;
  MemoryBanks = 0;
}
bool FuncUnitWrapper::isEmpty() const {
  return Required == 0 && Reserved == 0 && Slots == 0 && MemoryBanks == 0;
}

void FuncUnitWrapper::blockResources() {
  Required = ~0;
  Reserved = ~0;
  Slots = ~0;
  // Since the HW stalls in the event of memory bank conflicts, we don't need to
  // block the resource. It is overly conservative if we block all memory banks.
}

FuncUnitWrapper &FuncUnitWrapper::operator|=(const FuncUnitWrapper &Other) {
  Required |= Other.Required;
  Reserved |= Other.Reserved;
  Slots |= Other.Slots;
  MemoryBanks |= Other.MemoryBanks;
  return *this;
}

bool FuncUnitWrapper::conflict(const FuncUnitWrapper &Other) const {
  if ((Required & Other.Required) != 0 || (Slots & Other.Slots) != 0 ||
      (MemoryBanks & Other.MemoryBanks) != 0 ||
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

  // Limit VLD multislot instructions to be NFC for the SW pipeliner.
  if (MI.mayLoad())
    return Bundle.canAdd(AlternateInsts->back());

  return any_of(*AlternateInsts,
                [&](unsigned AltOpcode) { return Bundle.canAdd(AltOpcode); });
}

void AIEResourceCycle::reserveResources(MachineInstr &MI) {
  const std::vector<unsigned int> *AlternateInsts =
      Bundle.FormatInterface->getAlternateInstsOpcode(MI.getOpcode());

  if (!AlternateInsts)
    return Bundle.add(&MI);

  // Limit VLD multislot instructions to be NFC for the SW pipeliner.
  if (MI.mayLoad())
    return Bundle.add(&MI, AlternateInsts->back());

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

AIEHazardRecognizer::AIEHazardRecognizer(
    const AIEBaseInstrInfo *TII, const InstrItineraryData *II,
    AIEAlternateDescriptors &SelectedAlternateDescs, bool IsPreRA,
    std::optional<unsigned> ScoreboardDepth)
    : TII(TII), ItinData(II), SelectedAltDescs(SelectedAlternateDescs),
      IsPreRA(IsPreRA) {

  int Depth = 0;
  if (ScoreboardDepth.has_value()) {
    MaxLatency = *ScoreboardDepth;
    Depth = *ScoreboardDepth;
  } else {
    computeMaxLatency();
    Depth = computeScoreboardDepth();
  }

  Scoreboard.reset(Depth);
  MaxLookAhead = Depth;
  if (CLIssueLimit > 0)
    IssueLimit = CLIssueLimit;

  // Switch off VLIW for every region after scheduling the specified
  // number of instructions
  if (MaxVLIWInstrs >= 0 && NumInstrsScheduled > MaxVLIWInstrs) {
    IssueLimit = 1;
  }

  if (IsPreRA) {
    FUDepthLimit = PreMISchedFUDepth;
    IgnoreUnknownSlotSets = PreMISchedIgnoreUnknownSlots;
  }
}

AIEHazardRecognizer::AIEHazardRecognizer(
    const TargetSubtargetInfo &Subtarget,
    AIEAlternateDescriptors &SelectedAlternateDescs, bool IsPreRA)
    : AIEHazardRecognizer(
          static_cast<const AIEBaseInstrInfo *>(Subtarget.getInstrInfo()),
          Subtarget.getInstrItineraryData(), SelectedAlternateDescs, IsPreRA) {}

namespace llvm {
void applyFormatOrdering(AIE::MachineBundle &Bundle, const VLIWFormat &Format,
                         MachineBasicBlock::iterator InsertPoint) {
  assert(Bundle.SlotMap.size() == Bundle.Instrs.size() &&
         "Bundle has instructions without slot");
  if (Bundle.empty())
    return;

  MachineBasicBlock &MBB = *Bundle.getInstrs()[0]->getParent();
  auto *TII = static_cast<const AIEBaseInstrInfo *>(
      MBB.getParent()->getSubtarget().getInstrInfo());

  // Run over the slots of the format and either insert the occupying
  // instruction or a nop. Reapply bundling.
  MachineInstr *FirstMI = nullptr;
  for (MCSlotKind Slot : Format.getSlots()) {
    const MCSlotInfo *SlotInfo = TII->getSlotInfo(Slot);
    assert(SlotInfo);

    llvm::MachineInstr *Instr = Bundle.at(Slot);
    if (!Instr)
      continue;

    Instr->removeFromBundle();
    MBB.insert(InsertPoint, Instr);
    if (!FirstMI)
      FirstMI = Instr;
    else
      Instr->bundleWithPred();
  }

  if (!MBB.getParent()->getSubtarget().getTargetTriple().isAIE1()) {
    // Make sure bundles are finalized otherwise kill flags can be incorrect,
    // and accessing properties on a bundle header might give un-expected
    // results.
    finalizeBundle(MBB, FirstMI->getIterator());
  }
}
} // namespace llvm

void AIEHazardRecognizer::applyBundles(
    const std::vector<AIE::MachineBundle> &Bundles, MachineBasicBlock *MBB) {
  for (auto B : Bundles) {
    LLVM_DEBUG(dbgs() << "---Bundle---\n");

    // Erase the BUNDLE root to be sure we do not end up with standalone BUNDLEs
    // after de-bundling and re-bundling.
    B.eraseRootFromBlock();

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
        getBundleEnd(B.getInstrs().back()->getIterator());

    // AIE1 does not always have formats for standalone instructions.
    // Do not re-order for such cases.
    if (B.size() > 1)
      applyFormatOrdering(B, *B.getFormatOrNull(), BundleEnd);

    for (auto *I : B.getMetaInstrs()) {
      LLVM_DEBUG(dbgs() << "Meta " << *I);
      MBB->insert(BundleEnd, I);
    }
  }
}

void AIEHazardRecognizer::Reset() {
  LLVM_DEBUG(dbgs() << "Reset hazard recognizer\n");
  ReservedCycles = 0;
  Scoreboard.clear();
  SelectedAltDescs.clear();
}

ScheduleHazardRecognizer::HazardType
AIEHazardRecognizer::getHazardType(SUnit *SU, int DeltaCycles) {
  MachineInstr *MI = SU->getInstr();
  assert(!MI->isBundled() &&
         "Unexpected bundled instruction when checking hazards.");
  if (AIE::MachineBundle::isNoHazardMetaInstruction(MI->getOpcode())) {
    LLVM_DEBUG(dbgs() << "Meta instruction\n");
    return NoHazard;
  }

  if (int(ReservedCycles) + DeltaCycles > 0 && !MI->hasDelaySlot()) {
    LLVM_DEBUG(dbgs() << "Reserved cycle\n");
    return NoopHazard;
  }

  if (Scoreboard[DeltaCycles].IssueCount >= IssueLimit) {
    LLVM_DEBUG(dbgs() << "At issue limit\n");
    return NoopHazard;
  }

  const std::vector<unsigned int> *AlternateInsts =
      TII->getFormatInterface()->getAlternateInstsOpcode(MI->getOpcode());
  const MachineRegisterInfo &MRI = MI->getMF()->getRegInfo();
  if (AlternateInsts) {
    for (const auto AltInstOpcode : *AlternateInsts) {
      ScheduleHazardRecognizer::HazardType Haz =
          getHazardType(Scoreboard, TII->get(AltInstOpcode), getMemoryBanks(MI),
                        MI->operands(), MRI, DeltaCycles);
      // Check if there is NoHazard, If there is a Hazard or NoopHazard check
      // for the next possible Opcode.
      if (Haz == NoHazard) {
        SelectedAltDescs.setAlternateDescriptor(MI, AltInstOpcode);
        return NoHazard;
      }
    }
    // In the above loop we are trying to find the best one where there is
    // NoHazard, if the loop is not able to find such case it will be a
    // NoopHazard only.
    return NoopHazard;
  }

  return getHazardType(Scoreboard, MI->getDesc(), getMemoryBanks(MI),
                       MI->operands(), MRI, DeltaCycles);
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
  if (ReservedCycles)
    --ReservedCycles;
  Scoreboard.advance();
}

// Similar to AdvanceCycle, but for bottom-up scheduling.
void AIEHazardRecognizer::RecedeCycle() {
  LLVM_DEBUG(dbgs() << "Recede cycle, clear state\n");
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

void AIEHazardRecognizer::EmitInstruction(SUnit *SU) {
  return EmitInstruction(SU, 0);
}

void AIEHazardRecognizer::EmitInstruction(SUnit *SU, int DeltaCycles) {
  MachineInstr *MI = SU->getInstr();
  const MachineRegisterInfo &MRI = MI->getMF()->getRegInfo();
  LLVM_DEBUG(dbgs() << "Emit Instruction: " << *MI);
  LLVM_DEBUG(dbgs() << "  With Delta=" << DeltaCycles << "\n");

  for (MachineInstr &BundledMI : bundled_instrs(*MI)) {
    // If the instruction has multiple options, find the opcode that was
    // selected and use the latter to update the scoreboard.
    unsigned SelectedOpcode = SelectedAltDescs.getOpcode(&BundledMI);

    if (AIE::MachineBundle::isNoHazardMetaInstruction(SelectedOpcode))
      continue;

    emitInScoreboard(TII->get(SelectedOpcode), getMemoryBanks(&BundledMI),
                     BundledMI.operands(), MRI, DeltaCycles);
  }

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

void AIEHazardRecognizer::setReservedCycles(unsigned Cycles) {
  this->ReservedCycles = Cycles;
}

static SlotBits getSlotSet(const MCInstrDesc &Desc,
                           const AIEBaseMCFormats &Formats,
                           bool IgnoreUnkownSlotSets) {
  MCSlotKind SlotKind = Formats.getSlotKind(Desc.getOpcode());
  if (SlotKind != MCSlotKind())
    return Formats.getSlotInfo(SlotKind)->getSlotSet();

  // Instructions with no format/slot cannot be added in a non-empty Bundle.
  // Therefore, act as if they block all slots.
  return IgnoreUnkownSlotSets ? 0 : ~0;
}

namespace {
auto toHazardType(bool Conflict) {
  return Conflict ? ScheduleHazardRecognizer::NoopHazard
                  : ScheduleHazardRecognizer::NoHazard;
}
} // namespace

// These functions interpret the itinerary, translating InstrStages
// to ResourceCycles to apply.
// We deviate from the standard ScoreboardHazardRecognizer by not
// recognizing alternatives
ScheduleHazardRecognizer::HazardType AIEHazardRecognizer::getHazardType(
    const ResourceScoreboard<FuncUnitWrapper> &TheScoreboard,
    const MCInstrDesc &Desc, MemoryBankBits MemoryBanks,
    iterator_range<const MachineOperand *> MIOperands,
    const MachineRegisterInfo &MRI, int DeltaCycles) const {
  const unsigned SchedClass = TII->getSchedClass(Desc, MIOperands, MRI);
  if (!IsPreRA && SchedClass == 0 &&
      !AIE::MachineBundle::isNoHazardMetaInstruction(Desc.getOpcode())) {
    LLVM_DEBUG(llvm::dbgs() << "Warning!: no Scheduling class for Opcode="
                            << Desc.getOpcode() << "\n");
    report_fatal_error("Missing scheduling info.");
  }
  return toHazardType(checkConflict(
      TheScoreboard, ItinData, SchedClass,
      getSlotSet(Desc, *TII->getFormatInterface(), IgnoreUnknownSlotSets),
      MemoryBanks, TII->getMemoryCycles(SchedClass), DeltaCycles,
      FUDepthLimit));
}

bool AIEHazardRecognizer::checkConflict(
    const ResourceScoreboard<FuncUnitWrapper> &Scoreboard, MachineInstr &MI,
    int DeltaCycles) const {
  const MCInstrDesc &Desc = MI.getDesc();
  const unsigned SchedClass =
      TII->getSchedClass(Desc, MI.operands(), MI.getMF()->getRegInfo());
  const MemoryBankBits MemoryBanks = getMemoryBanks(&MI);
  return checkConflict(
      Scoreboard, ItinData, SchedClass,
      getSlotSet(Desc, *TII->getFormatInterface(), IgnoreUnknownSlotSets),
      MemoryBanks, TII->getMemoryCycles(SchedClass), DeltaCycles, std::nullopt);
}

bool AIEHazardRecognizer::checkConflict(
    const ResourceScoreboard<FuncUnitWrapper> &Scoreboard,
    const InstrItineraryData *ItinData, unsigned SchedClass, SlotBits SlotSet,
    MemoryBankBits MemoryBanks, SmallVector<int, 2> MemoryAccessCycles,
    int DeltaCycles, std::optional<int> FUDepthLimit) {
  assert(Scoreboard.isValidDelta(DeltaCycles));

  // Verify format hazards
  FuncUnitWrapper EmissionCycle(/*Req=*/0, /*Res=*/0, SlotSet);
  if (EmissionCycle.conflict(Scoreboard[DeltaCycles]))
    return true;

  // Verify memory bank hazards
  if (!MemoryAccessCycles.empty()) {
    FuncUnitWrapper MemoryBankAccessCycle(/*Req=*/0, /*Res=*/0, /*SlotSet=*/0,
                                          MemoryBanks);
    for (auto Cycles : MemoryAccessCycles) {
      // MemoryAccessCycles starts counting from 1, so we need to subtract 1
      int AccessCycle = DeltaCycles + Cycles - 1;
      if (MemoryBankAccessCycle.conflict(Scoreboard[AccessCycle])) {
        LLVM_DEBUG(dbgs() << "*** Memory bank conflict in cycle=" << AccessCycle
                          << ":\n";
                   MemoryBankAccessCycle.dump(); dbgs() << "\n");
        return true;
      }
    }
  }

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
        LLVM_DEBUG(dbgs() << "*** Hazard in cycle=" << StageCycle
                          << " EC=" << StageCycle - DeltaCycles << ":\n";
                   ThisCycle.dump(); dbgs() << "\n");
        return true;
      }
    }

    // Advance the cycle to the next stage.
    Cycle += IS.getNextCycles();
  }

  return false;
}

void AIEHazardRecognizer::emitInScoreboard(
    const MCInstrDesc &Desc, MemoryBankBits MemoryBanks,
    iterator_range<const MachineOperand *> MIOperands,
    const MachineRegisterInfo &MRI, int DeltaCycles) {
  emitInScoreboard(Scoreboard, Desc, MemoryBanks, MIOperands, MRI, DeltaCycles);
}

void AIEHazardRecognizer::emitInScoreboard(
    ResourceScoreboard<FuncUnitWrapper> &TheScoreboard, const MCInstrDesc &Desc,
    MemoryBankBits MemoryBanks,
    iterator_range<const MachineOperand *> MIOperands,
    const MachineRegisterInfo &MRI, int DeltaCycles) const {
  const unsigned SchedClass = TII->getSchedClass(Desc, MIOperands, MRI);
  if (!IsPreRA && SchedClass == 0 &&
      !AIE::MachineBundle::isNoHazardMetaInstruction(Desc.getOpcode())) {
    LLVM_DEBUG(llvm::dbgs() << "Warning!: no Scheduling class for Opcode="
                            << Desc.getOpcode() << "\n");
    report_fatal_error("Missing scheduling info.");
  }
  const SlotBits SlotSet =
      getSlotSet(Desc, *TII->getFormatInterface(), IgnoreUnknownSlotSets);
  enterResources(TheScoreboard, ItinData, SchedClass, SlotSet, MemoryBanks,
                 TII->getMemoryCycles(SchedClass), DeltaCycles, FUDepthLimit);
}

void AIEHazardRecognizer::enterResources(
    ResourceScoreboard<FuncUnitWrapper> &Scoreboard,
    const InstrItineraryData *ItinData, unsigned SchedClass, SlotBits SlotSet,
    MemoryBankBits MemoryBanks, SmallVector<int, 2> MemoryAccessCycles,
    int DeltaCycles, std::optional<int> FUDepthLimit) {
  assert(Scoreboard.isValidDelta(DeltaCycles));

  // Append slot usage
  FuncUnitWrapper EmissionCycle(/*Req=*/0, /*Res=*/0, SlotSet);
  Scoreboard[DeltaCycles] |= EmissionCycle;

  // Append memory bank usage
  if (!MemoryAccessCycles.empty()) {
    FuncUnitWrapper MemoryBankAccessCycle(/*Req=*/0, /*Res=*/0, /*SlotSet=*/0,
                                          MemoryBanks);
    for (auto Cycles : MemoryAccessCycles) {
      Scoreboard[DeltaCycles + Cycles - 1] |= MemoryBankAccessCycle;
    }
  }

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

unsigned AIEHazardRecognizer::getPipelineDepth() const { return PipelineDepth; }

unsigned AIEHazardRecognizer::getMaxLatency() const { return MaxLatency; }

int AIEHazardRecognizer::getConflictHorizon() const {
  return int(std::max(PipelineDepth, MaxLatency));
}

void AIEHazardRecognizer::computeMaxLatency() {
  assert(ItinData && !ItinData->isEmpty());
  unsigned FirstRW = std::numeric_limits<unsigned>().max();
  unsigned LastRW = 0;
  unsigned ItinDepth = 0;
  for (unsigned SchedClass = 0; !ItinData->isEndMarker(SchedClass);
       ++SchedClass) {
    unsigned CurCycle = 0;
    for (const InstrStage &IS : ItinData->getStages(SchedClass)) {
      ItinDepth = std::max(ItinDepth, CurCycle + IS.getCycles());
      CurCycle += IS.getNextCycles();
    }
    for (unsigned OpIdx = 0;; OpIdx++) {
      std::optional<unsigned> OpLat =
          ItinData->getOperandCycle(SchedClass, OpIdx);
      if (!OpLat) {
        break;
      }
      FirstRW = std::min(FirstRW, *OpLat);
      LastRW = std::max(LastRW, *OpLat);
    }
  }

  PipelineDepth = ItinDepth;

  // This is worst case, ignoring bypasses and same-cycle WAR
  MaxLatency = LastRW - FirstRW + 1;
  FirstRW = TII->getMinFirstMemoryCycle();
  LastRW = TII->getMaxLastMemoryCycle();
  MaxLatency = std::max(MaxLatency, int(LastRW - FirstRW + 1));
}

unsigned AIEHazardRecognizer::computeScoreboardDepth() const {
  unsigned Depth = getPipelineDepth();
  if (Depth == 0) {
    return 0;
  }

  return std::max(Depth, UserScoreboardDepth.getValue());
}

MemoryBankBits
AIEHazardRecognizer::getMemoryBanks(const MachineInstr *MI) const {
  if (!(MI->mayLoad() || MI->mayStore()))
    return 0;

  if (MI->memoperands_empty())
    return ~0;

  const AIEBaseSubtarget &STI = AIEBaseSubtarget::get(*MI->getMF());
  const AIEBaseAddrSpaceInfo &ASI = STI.getAddrSpaceInfo();
  MemoryBankBits MemoryBankUsed = 0;
  for (auto &MMO : MI->memoperands()) {
    unsigned AddrSpace = MMO->getAddrSpace();
    MemoryBankBits MemoryBank =
        AddrSpace                ? ASI.getMemoryBanksFromAddressSpace(AddrSpace)
        : AddressSpaceNoneIsSafe ? 0
                                 : ASI.getDefaultMemoryBank();
    MemoryBankUsed |= MemoryBank;
  }
  return MemoryBankUsed;
}
