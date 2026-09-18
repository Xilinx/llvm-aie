//===- AIEScheduleInterpreter.cpp - Schedule-aware itinerary interpreter -===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements a schedule-aware interpreter that computes register
// file (RF) occupancy windows from scheduled MachineInstrs and itinerary
// data.
//
//===----------------------------------------------------------------------===//

#include "AIEScheduleInterpreter.h"
#include "AIEBaseInstrInfo.h"
#include "AIELivenessVector.h"
#include "AIERegDefUseTracker.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/MC/LaneBitmask.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrItineraries.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"
#include <cassert>
#include <map>

#define DEBUG_TYPE "aie-schedule-interpreter"

using namespace llvm;

AIEScheduleInterpreter::AIEScheduleInterpreter(const MachineFunction &MF)
    : TII(static_cast<const AIEBaseInstrInfo &>(
          *MF.getSubtarget().getInstrInfo())),
      TRI(*MF.getSubtarget().getRegisterInfo()), MRI(MF.getRegInfo()),
      Itin(MF.getSubtarget().getInstrItineraryData()) {
  assert(Itin && !Itin->isEmpty() &&
         "Instruction itinerary data must be provided");
}

int AIEScheduleInterpreter::getOperandCycle(unsigned SchedClass,
                                            unsigned OpIdx) const {
  // Any operand index derived from an actual instruction is admissible within
  // the AIE family.  On older AIE architectures, implicit operand indices may
  // fall outside the itinerary table; return 0 (issue cycle) as the
  // established fallback, consistent with AIERegMemEventTracker.
  return static_cast<int>(Itin->getOperandCycle(SchedClass, OpIdx).value_or(0));
}

int AIEScheduleInterpreter::getOperandAccessCycle(const MachineInstr &MI,
                                                  int IssueCycle,
                                                  unsigned OpIdx) const {
  const MCInstrDesc &Desc = MI.getDesc();
  const unsigned SchedClass = TII.getSchedClass(Desc, MI.operands(), MRI);
  return IssueCycle + getOperandCycle(SchedClass, OpIdx);
}

// Helper to add an event to the schedule, resizing if necessary.
static void addEvent(EventSchedule &Schedule, int Cycle, EventType Type,
                     unsigned LRIndex, unsigned SubRegIdx,
                     unsigned ForwardingClass, const MachineInstr *MI,
                     unsigned OpIdx) {
  if (Cycle >= static_cast<int>(Schedule.size()))
    Schedule.resize(Cycle + 1);
  Schedule[Cycle].emplace_back(Type, LRIndex, SubRegIdx, ForwardingClass, MI,
                               OpIdx);
}

void AIEScheduleInterpreter::addInstructionEvents(
    const MachineInstr &MI, int IssueCycle, EventSchedule &Schedule,
    const RegLiveRangeTracker &Tracker) const {

  LLVM_DEBUG(dbgs() << "Adding events for instruction at cycle " << IssueCycle
                    << ": " << MI);

  // Get scheduling class once for all operands.
  // Use TII.getSchedClass() to resolve variable itineraries based on operand
  // register classes.
  const MCInstrDesc &Desc = MI.getDesc();
  const unsigned SchedClass = TII.getSchedClass(Desc, MI.operands(), MRI);

  // Process all operands.
  for (unsigned OpIdx = 0; OpIdx < MI.getNumOperands(); ++OpIdx) {
    const MachineOperand &MO = MI.getOperand(OpIdx);

    // Skip non-register operands.
    if (!MO.isReg() || !MO.getReg())
      continue;

    // Look up the LRIndex for this operand.  Handles both explicit and
    // implicit operands that belong to a tracked live range; unregistered
    // operands (the vast majority of implicit operands) are skipped here.
    const auto Idx = Tracker.getIndexForOperand(&MO);
    if (!Idx)
      continue;
    const unsigned LRIndex = *Idx;
    const unsigned SubRegIdx = MO.getSubReg();
    const unsigned ForwardingClass =
        Itin->getForwardingClass(SchedClass, OpIdx);

    // Defs produce Write events; uses produce Read events.
    // ForwardingClass != 0 indicates this operand also accesses a bypass.
    const EventType EType = MO.isDef() ? EventType::Write : EventType::Read;
    const int CycleOffset = getOperandCycle(SchedClass, OpIdx);
    const int Cycle = IssueCycle + CycleOffset;

    addEvent(Schedule, Cycle, EType, LRIndex, SubRegIdx, ForwardingClass, &MI,
             OpIdx);

    LLVM_DEBUG(
        dbgs() << "  " << (MO.isDef() ? "Write" : "Read") << " LR#" << LRIndex;
        if (SubRegIdx) dbgs() << ":" << TRI.getSubRegIndexName(SubRegIdx);
        dbgs() << " at cycle " << Cycle;
        if (ForwardingClass) dbgs()
        << " (forwarding class " << ForwardingClass << ")";
        dbgs() << "\n");
  }
}

std::string RFEvent::toString() const {
  const char Action = (Type == EventType::Read) ? 'R' : 'W';
  std::string ActionStr;
  if (SubRegIdx != 0) {
    // Include subreg info if present (format as R## or W##).
    raw_string_ostream Stream(ActionStr);
    Stream << format("%c%02d", Action, SubRegIdx);
  } else {
    // No subreg, just the action with padding.
    ActionStr = Action;
    ActionStr += "  ";
  }
  return ActionStr;
}

void AIEScheduleInterpreter::dumpEventSchedule(
    const EventSchedule &Schedule, const RegLiveRangeTracker &Tracker,
    raw_ostream &OS) const {

  // Collect all unique LR indices, sorted for stable output.
  DenseSet<unsigned> AllLRIndexSet;
  for (const auto &CycleEvents : Schedule)
    for (const auto &Event : CycleEvents)
      AllLRIndexSet.insert(Event.LRIndex);
  SmallVector<unsigned> AllLRIndices(AllLRIndexSet.begin(),
                                     AllLRIndexSet.end());
  llvm::sort(AllLRIndices);

  // Build separate maps for register and bypass events per LRIndex.
  // Bypass events are derived from ForwardingClass:
  // - Reads with ForwardingClass != 0 also read bypass at same cycle.
  // - Writes with ForwardingClass != 0 also write bypass one cycle earlier.
  DenseMap<unsigned, std::map<unsigned, std::string>> RegEventsByLRIndex;
  DenseMap<unsigned, std::map<unsigned, std::string>> BypassEventsByLRIndex;
  for (unsigned Cycle = 0; Cycle < Schedule.size(); ++Cycle) {
    const auto &CycleEvents = Schedule[Cycle];
    for (const auto &Event : CycleEvents) {
      if (!RegEventsByLRIndex[Event.LRIndex][Cycle].empty())
        RegEventsByLRIndex[Event.LRIndex][Cycle] += " ";
      RegEventsByLRIndex[Event.LRIndex][Cycle] += Event.toString();

      if (Event.ForwardingClass != 0) {
        const int BypassCycle =
            (Event.Type == EventType::Write) ? Cycle - 1 : Cycle;
        if (BypassCycle >= 0) {
          if (!BypassEventsByLRIndex[Event.LRIndex][BypassCycle].empty())
            BypassEventsByLRIndex[Event.LRIndex][BypassCycle] += " ";
          BypassEventsByLRIndex[Event.LRIndex][BypassCycle] += Event.toString();
        }
      }
    }
  }

  // Print header with cycle numbers.
  // Reserve 12 characters for register class names to handle long names.
  OS << " RegClass    LRIdx |";
  for (unsigned Cycle = 0; Cycle < Schedule.size(); ++Cycle)
    OS << format(" %4d |", Cycle);
  OS << "\n";

  // Print separator.
  OS << "-------------------+";
  for (unsigned Cycle = 0; Cycle < Schedule.size(); ++Cycle)
    OS << "------+";
  OS << "\n";

  // Helper lambda to print a row of events.
  auto PrintEventRow = [&](const std::map<unsigned, std::string> &Events) {
    for (unsigned Cycle = 0; Cycle < Schedule.size(); ++Cycle) {
      auto It = Events.find(Cycle);
      OS << format(" %-4s |", It != Events.end() ? It->second.c_str() : "");
    }
    OS << "\n";
  };

  // Print each LRIndex with register events and bypass events on separate
  // lines.
  for (unsigned LRIndex : AllLRIndices) {
    const RegLiveRange *LR = &Tracker[LRIndex];
    const char *RCName = (LR->getRegisterClass())
                             ? TRI.getRegClassName(LR->getRegisterClass())
                             : "unknown";

    // Use %-12.12s to left-align, pad to 12 chars, and truncate at 12 chars.
    OS << format(" %-12.12s%5u |", RCName, LRIndex);
    PrintEventRow(RegEventsByLRIndex[LRIndex]);

    const auto &BypassEvents = BypassEventsByLRIndex[LRIndex];
    if (!BypassEvents.empty()) {
      OS << "         bypass    |";
      PrintEventRow(BypassEvents);
    }
  }
}

// Helper function to get lane mask for a register operand.
// Uses the live range's register class to determine the full lane mask,
// falling back to LaneBitmask::getAll() when the RC is unavailable.
static LaneBitmask getLaneMaskFor(const TargetRegisterInfo &TRI,
                                  const RegLiveRangeTracker &Tracker,
                                  unsigned SubRegIdx, unsigned LRIndex) {
  if (SubRegIdx == 0) {
    // Full/composite register - get the lane mask from the live range's RC.
    const TargetRegisterClass *RC = Tracker[LRIndex].getRegisterClass();
    if (RC)
      return RC->getLaneMask();
    return LaneBitmask::getAll();
  }
  return TRI.getSubRegIndexLaneMask(SubRegIdx);
}

DenseMap<unsigned, AIE::LivenessVector> AIEScheduleInterpreter::buildLiveLanes(
    const EventSchedule &Schedule, int II,
    const RegLiveRangeTracker &Tracker) const {

  assert(II > 0 && "Initiation interval must be positive");

  DenseMap<unsigned, AIE::LivenessVector> LiveLanesByLRIndex;

  if (Schedule.empty())
    return LiveLanesByLRIndex;

  // State: tracks which lanes are currently live when scanning backward.
  DenseMap<unsigned, LaneBitmask> ActiveMask;

  // Process cycles backward.
  int MaxCycle = Schedule.size() - 1;
  for (int C = MaxCycle; C >= 0; --C) {
    const auto &Events = Schedule[C];
    int ModuloCycle = C % II;

    // Record what's live ENTERING this cycle (before any events).
    for (const auto &[LRIndex, Mask] : ActiveMask) {
      if (Mask.any()) {
        if (!LiveLanesByLRIndex.count(LRIndex))
          LiveLanesByLRIndex[LRIndex] = AIE::LivenessVector(II);
        LiveLanesByLRIndex[LRIndex][ModuloCycle] |= Mask;

        LLVM_DEBUG(dbgs() << "    Lanes " << PrintLaneMask(Mask) << " for LR#"
                          << LRIndex << " live entering cycle " << C
                          << " (offset " << ModuloCycle << ")\n");
      }
    }

    // Step 1: Process defs (writes) - they occupy the register and kill lanes
    // going backward.
    for (const auto &Event : Events) {
      if (Event.Type == EventType::Write) {
        LaneBitmask M =
            getLaneMaskFor(TRI, Tracker, Event.SubRegIdx, Event.LRIndex);

        if (!LiveLanesByLRIndex.count(Event.LRIndex))
          LiveLanesByLRIndex[Event.LRIndex] = AIE::LivenessVector(II);

        // RF write occupies register file at ModuloCycle.
        LiveLanesByLRIndex[Event.LRIndex][ModuloCycle] |= M;

        // If this write uses a bypass, mark bypass write one cycle earlier.
        // For dead ranges (no uses) the bypassed value has no reader, so the
        // bypass write occupancy contributes no real constraint and is skipped.
        // Without this, two dead writes to the same scarce register with
        // adjacent issue cycles create an artificial interference through the
        // bypass write slot that prevents both from being allocated.
        if (Event.ForwardingClass != 0 &&
            Tracker[Event.LRIndex].getNumUses() > 0) {
          const int BypassWriteCycle = C - 1;
          if (BypassWriteCycle >= 0) {
            const int BypassModuloCycle = BypassWriteCycle % II;
            LiveLanesByLRIndex[Event.LRIndex][BypassModuloCycle].addBypassWrite(
                Event.ForwardingClass);

            LLVM_DEBUG(dbgs()
                       << "    Bypass write of class " << Event.ForwardingClass
                       << " at cycle " << BypassWriteCycle << " (offset "
                       << BypassModuloCycle << ")\n");
          }
        }

        // Kill those lanes going backward.
        ActiveMask[Event.LRIndex] &= ~M;

        LLVM_DEBUG(dbgs() << "  Cycle " << C << " (" << ModuloCycle
                          << "): Write LR#" << Event.LRIndex;
                   if (Event.SubRegIdx) dbgs()
                   << ":" << TRI.getSubRegIndexName(Event.SubRegIdx);
                   dbgs() << " occupies lanes " << PrintLaneMask(M)
                          << " and kills them going backward\n");

        if (ActiveMask[Event.LRIndex].none())
          ActiveMask.erase(Event.LRIndex);
      }
    }

    // Step 2: Process uses (reads) - they make the register live going
    // backward.
    for (const auto &Event : Events) {
      if (Event.Type == EventType::Read) {
        LaneBitmask M =
            getLaneMaskFor(TRI, Tracker, Event.SubRegIdx, Event.LRIndex);

        // Reads make the register live going backward from this cycle.
        ActiveMask[Event.LRIndex] |= M;

        LLVM_DEBUG(dbgs() << "  Cycle " << C << " (" << ModuloCycle
                          << "): Read LR#" << Event.LRIndex;
                   if (Event.SubRegIdx) dbgs()
                   << ":" << TRI.getSubRegIndexName(Event.SubRegIdx);
                   dbgs() << " lanes " << PrintLaneMask(M)
                          << " become live going backward\n");

        // If this read uses a bypass, mark bypass read at same cycle.
        if (Event.ForwardingClass != 0) {
          if (!LiveLanesByLRIndex.count(Event.LRIndex))
            LiveLanesByLRIndex[Event.LRIndex] = AIE::LivenessVector(II);
          LiveLanesByLRIndex[Event.LRIndex][ModuloCycle].addBypassRead(
              Event.ForwardingClass);

          LLVM_DEBUG(dbgs() << "    Bypass read of class "
                            << Event.ForwardingClass << " at cycle " << C
                            << " (offset " << ModuloCycle << ")\n");
        }
      }
    }
  }

  // At the end, ActiveMask represents the live-in set. Currently
  // we only report it.
  for (const auto &[LRIndex, Mask] : ActiveMask) {
    if (Mask.any()) {
      LLVM_DEBUG(dbgs() << "LR#" << LRIndex << " has lanes "
                        << PrintLaneMask(Mask) << " live at beginning\n");
    }
  }

  return LiveLanesByLRIndex;
}

void AIEScheduleInterpreter::dumpLiveLanes(
    const DenseMap<unsigned, AIE::LivenessVector> &LiveLanesByLRIndex, int II,
    raw_ostream &OS) const {

  if (LiveLanesByLRIndex.empty()) {
    OS << "No live lanes data\n";
    return;
  }

  // Collect and sort LR indices for consistent output.
  SmallVector<unsigned, 16> LRIndices;
  for (const auto &[LRIndex, _] : LiveLanesByLRIndex)
    LRIndices.push_back(LRIndex);
  llvm::sort(LRIndices);

  OS << "Live Lanes (II=" << II << "):\n";
  OS << "LRIdx  | ";
  for (int T = 0; T < II; ++T)
    OS << format("t%-6d ", T);
  OS << "\n";

  OS << "-------+";
  for (int T = 0; T < II; ++T)
    OS << "--------";
  OS << "\n";

  for (unsigned LRIndex : LRIndices) {
    OS << format("%-6u | ", LRIndex);

    const auto &LanesByOffset = LiveLanesByLRIndex.lookup(LRIndex);
    for (int T = 0; T < II; ++T) {
      const AIE::Liveness &L = LanesByOffset[T];
      if (L.any()) {
        // Build indicator showing lanes and bypass classes.
        // Format examples:
        //   "##    " = lanes only
        //   "#R1   " = lanes + bypass read class 1
        //   "#W2   " = lanes + bypass write class 2
        //   "R1W2  " = bypass read class 1 + bypass write class 2
        //   "#R1W2 " = lanes + bypass read class 1 + bypass write class 2
        std::string Indicator;
        if (L.getLanes().any())
          Indicator = "#";

        // Add bypass read classes.
        if (!L.getBypassReads().empty()) {
          Indicator += "R";
          for (unsigned FC : L.getBypassReads())
            Indicator += std::to_string(FC);
        }

        // Add bypass write classes.
        if (!L.getBypassWrites().empty()) {
          Indicator += "W";
          for (unsigned FC : L.getBypassWrites())
            Indicator += std::to_string(FC);
        }

        // Pad to 6 characters for alignment.
        while (Indicator.size() < 6)
          Indicator += " ";
        OS << " " << Indicator << " ";
      } else {
        OS << " ..     ";
      }
    }
    OS << "\n";
  }
}
