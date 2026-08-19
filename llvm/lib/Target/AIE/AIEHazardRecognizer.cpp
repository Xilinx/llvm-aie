//===-- AIEHazardRecognizer.cpp - AIE Post RA Hazard Recognizer ---===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines the hazard recognizer for scheduling on AIE.
//
//===----------------------------------------------------------------------===//

#include "AIEHazardRecognizer.h"
#include "AIEBaseSubtarget.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "llvm/Analysis/ValueTracking.h"
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
#include <climits>
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
    "aie-addrspace-none-is-safe", cl::Hidden, cl::init(true),
    cl::desc("Assume that addrspace(0) doesn't cause conflicts."));

static cl::opt<bool>
    PointerHazardRecognition("aie-recognize-pointer-hazards", cl::Hidden,
                             cl::init(true),
                             cl::desc("Recognize pointer hazards"));

const AIEBaseMCFormats *FuncUnitWrapper::FormatInterface = nullptr;
void FuncUnitWrapper::setFormatInterface(const AIEBaseMCFormats *Formats) {
  FormatInterface = Formats;
}

bool FuncUnitWrapper::operator==(const FuncUnitWrapper &Other) const {
  return Required == Other.Required && Reserved == Other.Reserved &&
         Slots == Other.Slots && Conflicts == Other.Conflicts &&
         MemoryBanks == Other.MemoryBanks &&
         LoadMemObjectsBits == Other.LoadMemObjectsBits &&
         StoreMemObjectsBits == Other.StoreMemObjectsBits;
}

void FuncUnitWrapper::dump() const {
  const char *const Digits = "0123456789";
  const char *const Spacer = "-|";

  auto PrintFU = [&](const std::string &FUName, ResourceSet FU) {
    dbgs() << FUName;

    for (int J = FU.getNumBits() - 1; J >= 0; J--)
      dbgs() << (FU.contains(J) ? Digits[J % 10] : Spacer[J % 10 == 0]);
  };
  auto PrintResource = [&](const std::string &ResourceName, uint64_t Resource) {
    dbgs() << ResourceName;
    for (int J = 9; J >= 0; J--)
      dbgs() << ((Resource & (1ULL << J)) ? Digits[J] : '-');
  };

  PrintFU("Req     : ", Required);
  PrintResource(" Slots : ", Slots);
  PrintResource(" Memorybanks : ", MemoryBanks);
  PrintResource(" LoadMemObjBits : ", LoadMemObjectsBits);
  PrintResource(" StoreMemObjBits : ", StoreMemObjectsBits);
  if (Reserved.empty())
    return;
  PrintFU("\n\t   Rsrv : ", Reserved);
}

void FuncUnitWrapper::clearResources() {
  IssueCount = 0;
  Required.clear();
  Reserved.clear();
  Slots = 0;
  Conflicts = 0;
  MemoryBanks = 0;
  LoadMemObjectsBits = 0;
  StoreMemObjectsBits = 0;
}

bool FuncUnitWrapper::isEmpty() const {
  return Required.empty() && Reserved.empty() && Slots == 0 &&
         MemoryBanks == 0 && LoadMemObjectsBits == 0 &&
         StoreMemObjectsBits == 0;
}

void FuncUnitWrapper::blockResources() {
  Required = ~ResourceSet();
  Reserved = ~ResourceSet();
  Slots = ~0;
  // Since the HW stalls in the event of memory bank conflicts, we don't need to
  // block the resource. It is overly conservative if we block all memory banks.
  // The same applies for MemoryObjectBits.
}

FuncUnitWrapper &FuncUnitWrapper::operator|=(const FuncUnitWrapper &Other) {
  Required |= Other.Required;
  Reserved |= Other.Reserved;
  Slots |= Other.Slots;
  Conflicts |= Other.Conflicts;
  MemoryBanks |= Other.MemoryBanks;
  LoadMemObjectsBits |= Other.LoadMemObjectsBits;
  StoreMemObjectsBits |= Other.StoreMemObjectsBits;
  return *this;
}

bool FuncUnitWrapper::conflict(const FuncUnitWrapper &Other) const {
  // Load-Load and Store-Store conflicts are checked independently.
  // Loads and stores don't conflict with each other on AIE (separate HW ports).
  if ((Slots & Other.Slots) != 0 || (MemoryBanks & Other.MemoryBanks) != 0 ||
      (LoadMemObjectsBits & Other.LoadMemObjectsBits) != 0 ||
      (StoreMemObjectsBits & Other.StoreMemObjectsBits) != 0 ||
      (Conflicts & Other.Slots) != 0 || (Slots & Other.Conflicts) != 0 ||
      Required.overlap(Other.Required) || Reserved.overlap(Other.Required) ||
      Required.overlap(Other.Reserved)) {

    return true;
  }

  // Note: Don't check formats unless both have occupied slots.
  // This allows representing a blocked cycle (Slots = ~0) without knowing
  // the slot and format details.
  return Slots && Other.Slots &&
         !FormatInterface->isFormatAvailable(Slots | Other.Slots);
}

namespace {

using FuncUnitWrapperAction =
    std::function<bool(int C, const FuncUnitWrapper &)>;

// Central itinerary data interpreter
// It will call Action with the cycle number and the FuncUnits,
// returning true as soon as Action returns true.
// A full traversal can be made by returning false.
bool anyStage(ArrayRef<const InstrStage> Stages, FuncUnitWrapperAction Action,
              std::optional<int> FUDepthLimit = std::nullopt) {
  int Cycle = 0;
  for (const InstrStage &IS : Stages) {
    if (FUDepthLimit && Cycle >= *FUDepthLimit) {
      break;
    }
    const FuncUnitWrapper ThisCycle(IS);
    for (unsigned C = 0; C < IS.getCycles(); C++) {
      if (Action(Cycle + C, ThisCycle)) {
        return true;
      }
    }
    Cycle += IS.getNextCycles();
  }
  return false;
}
} // namespace

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
  if (AlternateInsts) {
    for (const auto AltInstOpcode : *AlternateInsts) {
      ScheduleHazardRecognizer::HazardType Haz =
          getHazardType(Scoreboard, MI, TII->get(AltInstOpcode), DeltaCycles);
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

  return getHazardType(Scoreboard, MI, DeltaCycles);
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
  assert(Scoreboard.isInRange(DeltaCycles));
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
                     getMemoryObjectsBits(&BundledMI), BundledMI.operands(),
                     MRI, DeltaCycles);
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

static SlotBits getConflictSet(const MCInstrDesc &Desc,
                               const AIEBaseMCFormats &Formats) {
  MCSlotKind SlotKind = Formats.getSlotKind(Desc.getOpcode());
  if (SlotKind != MCSlotKind())
    return Formats.getSlotInfo(SlotKind)->getConflictSet();

  return 0;
}

namespace {
auto toHazardType(bool Conflict) {
  return Conflict ? ScheduleHazardRecognizer::NoopHazard
                  : ScheduleHazardRecognizer::NoHazard;
}
} // namespace

ScheduleHazardRecognizer::HazardType AIEHazardRecognizer::getHazardType(
    const ResourceScoreboard<FuncUnitWrapper> &TheScoreboard,
    const MachineInstr *MI, const MCInstrDesc &Desc, int DeltaCycles) const {
  return getHazardType(TheScoreboard, Desc, getMemoryBanks(MI),
                       getMemoryObjectsBits(MI), MI->operands(),
                       MI->getMF()->getRegInfo(), DeltaCycles);
}

ScheduleHazardRecognizer::HazardType AIEHazardRecognizer::getHazardType(
    const ResourceScoreboard<FuncUnitWrapper> &TheScoreboard,
    const MachineInstr *MI, int DeltaCycles) const {
  return getHazardType(TheScoreboard, *SelectedAltDescs.getDesc(MI),
                       getMemoryBanks(MI), getMemoryObjectsBits(MI),
                       MI->operands(), MI->getMF()->getRegInfo(), DeltaCycles);
}

// These functions interpret the itinerary, translating InstrStages
// to ResourceCycles to apply.
// We deviate from the standard ScoreboardHazardRecognizer by not
// recognizing alternatives
ScheduleHazardRecognizer::HazardType AIEHazardRecognizer::getHazardType(
    const ResourceScoreboard<FuncUnitWrapper> &TheScoreboard,
    const MCInstrDesc &Desc, MemoryBankBits MemoryBanks,
    MemoryObjectPair MemObjectsBits,
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
      getConflictSet(Desc, *TII->getFormatInterface()), MemoryBanks,
      MemObjectsBits, TII->getMemoryCycles(SchedClass), DeltaCycles,
      FUDepthLimit));
}

bool AIEHazardRecognizer::checkConflict(
    const ResourceScoreboard<FuncUnitWrapper> &Scoreboard, MachineInstr &MI,
    int DeltaCycles) const {
  const MCInstrDesc &Desc = MI.getDesc();
  const unsigned SchedClass =
      TII->getSchedClass(Desc, MI.operands(), MI.getMF()->getRegInfo());
  const MemoryBankBits MemoryBanks = getMemoryBanks(&MI);
  const MemoryObjectPair MemObjectsBits = getMemoryObjectsBits(&MI);
  return checkConflict(
      Scoreboard, ItinData, SchedClass,
      getSlotSet(Desc, *TII->getFormatInterface(), IgnoreUnknownSlotSets),
      getConflictSet(Desc, *TII->getFormatInterface()), MemoryBanks,
      MemObjectsBits, TII->getMemoryCycles(SchedClass), DeltaCycles,
      std::nullopt);
}

bool AIEHazardRecognizer::checkConflict(MachineInstr &MI,
                                        int DeltaCycles) const {
  return checkConflict(Scoreboard, MI, DeltaCycles);
}

bool AIEHazardRecognizer::checkConflict(
    const ResourceScoreboard<FuncUnitWrapper> &Scoreboard,
    const InstrItineraryData *ItinData, unsigned SchedClass, SlotBits SlotSet,
    SlotBits ConflictSet, MemoryBankBits MemoryBanks,
    MemoryObjectPair MemObjectsBits, SmallVector<int, 2> MemoryAccessCycles,
    int DeltaCycles, std::optional<int> FUDepthLimit) {

  // Verify format hazards
  FuncUnitWrapper EmissionCycle(SlotSet, ConflictSet);
  if (EmissionCycle.conflict(Scoreboard[DeltaCycles]))
    return true;

  // Verify memory bank and shared object hazards
  if (!MemoryAccessCycles.empty()) {
    const SlotBits Slots = 0;
    const SlotBits Conflicts = 0;
    FuncUnitWrapper MemoryAccessCycle(Slots, Conflicts, MemoryBanks,
                                      MemObjectsBits.Load,
                                      MemObjectsBits.Store);

    for (auto Cycles : MemoryAccessCycles) {
      // MemoryAccessCycles starts counting from 1, so we need to subtract 1
      int AccessCycle = DeltaCycles + Cycles - 1;
      assert(Scoreboard.isInRange(AccessCycle));
      if (MemoryAccessCycle.conflict(Scoreboard[AccessCycle])) {
        LLVM_DEBUG(dbgs() << "*** Memory bank/Object conflict in cycle="
                          << AccessCycle << ":\n";
                   MemoryAccessCycle.dump(); dbgs() << "\n");
        return true;
      }
    }
  }

  // Note that DeltaCycles will be negative for bottom-up scheduling.

  /// Check ThisCycle for a conflict at Cycle relative to the start of the
  /// itinerary.
  FuncUnitWrapperAction CycleConflict =
      [&Scoreboard, DeltaCycles](int Cycle, const FuncUnitWrapper &ThisCycle) {
        const int StageCycle = DeltaCycles + Cycle;
        assert(Scoreboard.isInRange(StageCycle));
        if (ThisCycle.conflict(Scoreboard[StageCycle])) {
          LLVM_DEBUG(dbgs() << "*** Hazard in cycle=" << StageCycle
                            << " EC=" << StageCycle - DeltaCycles << ":\n";
                     ThisCycle.dump(); dbgs() << "\n");
          return true;
        }
        return false;
      };

  return anyStage(ItinData->getStages(SchedClass), CycleConflict, FUDepthLimit);
}

void AIEHazardRecognizer::emitInScoreboard(
    const MCInstrDesc &Desc, MemoryBankBits MemoryBanks,
    MemoryObjectPair MemObjectsBits,
    iterator_range<const MachineOperand *> MIOperands,
    const MachineRegisterInfo &MRI, int DeltaCycles) {
  emitInScoreboard(Scoreboard, Desc, MemoryBanks, MemObjectsBits, MIOperands,
                   MRI, DeltaCycles);
}

void AIEHazardRecognizer::emitInScoreboard(
    ResourceScoreboard<FuncUnitWrapper> &TheScoreboard, const MachineInstr &MI,
    const MCInstrDesc &Desc, int DeltaCycles) const {
  emitInScoreboard(TheScoreboard, Desc, getMemoryBanks(&MI),
                   getMemoryObjectsBits(&MI), MI.operands(),
                   MI.getMF()->getRegInfo(), DeltaCycles);
}

void AIEHazardRecognizer::emitInScoreboard(const MachineInstr &MI,
                                           const MCInstrDesc &Desc,
                                           int DeltaCycles) {
  emitInScoreboard(Scoreboard, MI, Desc, DeltaCycles);
}

void AIEHazardRecognizer::emitInScoreboard(
    ResourceScoreboard<FuncUnitWrapper> &TheScoreboard, const MCInstrDesc &Desc,
    MemoryBankBits MemoryBanks, MemoryObjectPair MemObjectsBits,
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
                 MemObjectsBits, TII->getMemoryCycles(SchedClass), DeltaCycles,
                 FUDepthLimit);
}

void AIEHazardRecognizer::enterResources(
    ResourceScoreboard<FuncUnitWrapper> &Scoreboard,
    const InstrItineraryData *ItinData, unsigned SchedClass, SlotBits SlotSet,
    MemoryBankBits MemoryBanks, MemoryObjectPair MemObjectsBits,
    SmallVector<int, 2> MemoryAccessCycles, int DeltaCycles,
    std::optional<int> FUDepthLimit) {

  // Append slot usage
  const SlotBits Conflicts = 0;
  FuncUnitWrapper EmissionCycle(SlotSet, Conflicts);
  Scoreboard[DeltaCycles] |= EmissionCycle;

  // Append memory bank usage
  if (!MemoryAccessCycles.empty()) {
    const SlotBits Slots = 0;
    const SlotBits Conflicts = 0;
    FuncUnitWrapper MemoryBankAndObjectsAccessCycle(
        Slots, Conflicts, MemoryBanks, MemObjectsBits.Load,
        MemObjectsBits.Store);
    for (auto Cycles : MemoryAccessCycles) {
      assert(Scoreboard.isInRange(DeltaCycles + Cycles - 1));
      Scoreboard[DeltaCycles + Cycles - 1] |= MemoryBankAndObjectsAccessCycle;
    }
  }

  Scoreboard[DeltaCycles].IssueCount++;

  // Insert ThisCycle at position Cycle relative to the start of the itinerary.
  auto Insert = [&Scoreboard, DeltaCycles](int Cycle,
                                           const FuncUnitWrapper &ThisCycle) {
    const int ScoreboardCycle = Cycle + DeltaCycles;
    assert(Scoreboard.isInRange(ScoreboardCycle));
    Scoreboard[ScoreboardCycle] |= ThisCycle;
    return false;
  };

  (void)anyStage(ItinData->getStages(SchedClass), Insert, FUDepthLimit);

  LLVM_DEBUG({
    dbgs() << "Scoreboard:\n";
    Scoreboard.dump();
  });
}

int AIEHazardRecognizer::computeInstrSelfMII(const MachineInstr &MI) const {
  const MCInstrDesc &Desc = MI.getDesc();
  const unsigned SchedClass =
      TII->getSchedClass(Desc, MI.operands(), MI.getMF()->getRegInfo());
  // Instructions without scheduling info cannot self-conflict.
  if (SchedClass == 0 || PipelineDepth <= 0)
    return 1;

  // Scan all candidate II values. Self-conflict is not monotone: a
  // non-conflicting II can be followed by a conflicting one (e.g. resource
  // uses at cycles {0, 3, 7} conflict at II=7 because 7 mod 7 == 0 mod 7,
  // even if II=5 is conflict-free). We therefore track the largest II that
  // self-conflicts and return that value plus one.
  int SelfMII = 1;
  for (int II = 1; II <= PipelineDepth; ++II) {
    std::vector<FuncUnitWrapper> SelfBoard(II);
    if (anyStage(ItinData->getStages(SchedClass),
                 [&](int Cycle, const FuncUnitWrapper &ThisCycle) {
                   const int Mod = Cycle % II;
                   if (ThisCycle.conflict(SelfBoard[Mod]))
                     return true;
                   SelfBoard[Mod] |= ThisCycle;
                   return false;
                 }))
      SelfMII = II + 1;
  }
  return SelfMII;
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
  FuncUnitWrapperAction MaxDepth = [this](int C, const FuncUnitWrapper &) {
    PipelineDepth = std::max(PipelineDepth, C);
    return false;
  };
  for (unsigned SchedClass = 0; !ItinData->isEndMarker(SchedClass);
       ++SchedClass) {
    anyStage(ItinData->getStages(SchedClass), MaxDepth);
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

  // This is worst case, ignoring bypasses and same-cycle WAR
  MaxLatency = LastRW - FirstRW + 1;
  FirstRW = TII->getMinFirstMemoryCycle();
  LastRW = TII->getMaxLastMemoryCycle();
  MaxLatency = std::max(MaxLatency, int(LastRW - FirstRW + 1));
  PipelineDepth = std::max(PipelineDepth, int(LastRW));
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
  // Bank tracking is load-only: stores cannot collide on banks because
  // AIE has only one store slot per VLIW cycle, and load-vs-store uses separate
  // HW ports.
  if (!MI->mayLoad())
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

bool MemoryObjectEnumerator::isFull() const {
  return ObjectCounter == sizeof(MemoryObjectsBits) * CHAR_BIT;
}

std::optional<unsigned>
MemoryObjectEnumerator::getObjectNumber(const Value *Object) {
  unsigned ObjectNumber = 0;

  // We locate directly the object number.
  auto ItNumber = ObjectNumberingMap.find(Object);
  if (ItNumber != ObjectNumberingMap.end()) {
    ObjectNumber = ItNumber->second;
    return ObjectNumber;
  }

  const Value *ParentObject = getUnderlyingObject(Object);

  // We locate the parent's object number.
  auto ItParentNumber = ObjectNumberingMap.find(ParentObject);
  if (ItParentNumber != ObjectNumberingMap.end()) {
    ObjectNumber = ItParentNumber->second;
    // Reuse the number for this object.
    ObjectNumberingMap[Object] = ObjectNumber;
    return ObjectNumber;
  }

  if (isFull()) {
    LLVM_DEBUG(dbgs() << "MemoryObjectEnumerator with full capacity\n");
    return std::nullopt;
  }

  // Use the same number for both objects.
  ObjectNumber = ObjectCounter++;
  ObjectNumberingMap[ParentObject] = ObjectNumber;
  ObjectNumberingMap[Object] = ObjectNumber;

  return ObjectNumber;
}

/// For instructions using memory operands, return
/// a pair of bit maps representing the used base objects for loads and stores.
/// This is not for correctness, but for wait cycles avoidance.
MemoryObjectPair
AIEHazardRecognizer::getMemoryObjectsBits(const MachineInstr *MI) const {
  MemoryObjectPair Result;

  // If in PreRA, don't constrain the scheduler even more.
  if (IsPreRA || !PointerHazardRecognition)
    return Result;

  // Only track loads and stores for pointer hazard tracking.
  if (!MI->mayLoad() && !MI->mayStore())
    return Result;

  if (MI->memoperands_empty())
    return Result; // optimistic: we will assume no conflict.

  MemoryObjectsBits ObjectsToBitMap = 0;
  for (auto &MMO : MI->memoperands()) {
    const Value *BaseObject = MMO->getValue();
    if (!BaseObject) {
      LLVM_DEBUG(dbgs() << "No Base Object!\n");
      continue;
    }

    const uint64_t One = 1;
    if (auto ObjectNumber = ObjectEnumerator.getObjectNumber(BaseObject))
      ObjectsToBitMap |= One << *ObjectNumber;
  }

  // Assign to the appropriate field based on instruction type.
  if (MI->mayLoad())
    Result.Load = ObjectsToBitMap;
  if (MI->mayStore())
    Result.Store = ObjectsToBitMap;

  return Result;
}
