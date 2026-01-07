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
  // Get operand cycle from itinerary.
  // This tells us when the operand is accessed relative to instruction issue.
  const std::optional<unsigned> OperandCycle =
      Itin->getOperandCycle(SchedClass, OpIdx);

  // Ensure we have timing information for this operand.
  assert(OperandCycle.has_value() &&
         "Itinerary must provide operand cycle information for all operands");

  return *OperandCycle;
}

// Helper to add an event to the schedule, resizing if necessary
static void addEvent(EventSchedule &Schedule, int Cycle, EventType Type,
                     Register VReg, unsigned SubRegIdx,
                     unsigned ForwardingClass, const MachineInstr *MI,
                     unsigned OpIdx) {
  // Ensure the schedule is large enough
  if (Cycle >= static_cast<int>(Schedule.size())) {
    Schedule.resize(Cycle + 1);
  }

  // Add the event
  Schedule[Cycle].emplace_back(Type, VReg, SubRegIdx, ForwardingClass, MI,
                               OpIdx);
}

void AIEScheduleInterpreter::addInstructionEvents(
    const MachineInstr &MI, int IssueCycle, EventSchedule &Schedule) const {

  LLVM_DEBUG(dbgs() << "Adding events for instruction at cycle " << IssueCycle
                    << ": " << MI);

  // Get scheduling class once for all operands.
  // Use TII.getSchedClass() to resolve variable itineraries based on operand
  // register classes.
  const MCInstrDesc &Desc = MI.getDesc();
  const unsigned SchedClass = TII.getSchedClass(Desc, MI.operands(), MRI);

  // Process all operands
  for (unsigned OpIdx = 0; OpIdx < MI.getNumOperands(); ++OpIdx) {
    const MachineOperand &MO = MI.getOperand(OpIdx);

    // Skip non-register operands
    if (!MO.isReg() || !MO.getReg())
      continue;

    // Skip physical registers for now
    if (!Register::isVirtualRegister(MO.getReg()))
      continue;

    // Skip implicit operands
    if (MO.isImplicit())
      continue;

    const Register VReg = MO.getReg();
    const unsigned SubRegIdx = MO.getSubReg();
    const unsigned ForwardingClass =
        Itin->getForwardingClass(SchedClass, OpIdx);

    // Defs produce Write events; uses produce Read events.
    // ForwardingClass != 0 indicates this operand also accesses a bypass.
    const EventType EType = MO.isDef() ? EventType::Write : EventType::Read;
    const int CycleOffset = getOperandCycle(SchedClass, OpIdx);
    const int Cycle = IssueCycle + CycleOffset;

    addEvent(Schedule, Cycle, EType, VReg, SubRegIdx, ForwardingClass, &MI,
             OpIdx);

    LLVM_DEBUG(dbgs() << "  " << (MO.isDef() ? "Write" : "Read") << " %vreg"
                      << VReg.virtRegIndex();
               if (SubRegIdx) dbgs()
               << ":" << TRI.getSubRegIndexName(SubRegIdx);
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

void AIEScheduleInterpreter::dumpEventSchedule(const EventSchedule &Schedule,
                                               raw_ostream &OS) const {

  // Collect all unique virtual registers, sorted by vreg index for stable
  // output.
  DenseSet<Register> AllVRegsSet;
  for (const auto &CycleEvents : Schedule)
    for (const auto &Event : CycleEvents)
      AllVRegsSet.insert(Event.VReg);
  SmallVector<Register> AllVRegs(AllVRegsSet.begin(), AllVRegsSet.end());
  llvm::sort(AllVRegs, [](Register A, Register B) {
    return A.virtRegIndex() < B.virtRegIndex();
  });

  // Build separate maps for register and bypass events per VReg.
  // Bypass events are derived from ForwardingClass:
  // - Reads with ForwardingClass != 0 also read bypass at same cycle
  // - Writes with ForwardingClass != 0 also write bypass one cycle earlier
  DenseMap<Register, std::map<unsigned, std::string>> RegEventsByVReg;
  DenseMap<Register, std::map<unsigned, std::string>> BypassEventsByVReg;
  for (unsigned Cycle = 0; Cycle < Schedule.size(); ++Cycle) {
    const auto &CycleEvents = Schedule[Cycle];
    for (const auto &Event : CycleEvents) {
      // Add space if there's already an event in this cycle
      if (!RegEventsByVReg[Event.VReg][Cycle].empty()) {
        RegEventsByVReg[Event.VReg][Cycle] += " ";
      }
      RegEventsByVReg[Event.VReg][Cycle] += Event.toString();

      // If this event uses a bypass, add bypass event
      if (Event.ForwardingClass != 0) {
        const int BypassCycle =
            (Event.Type == EventType::Write) ? Cycle - 1 : Cycle;
        if (BypassCycle >= 0) {
          if (!BypassEventsByVReg[Event.VReg][BypassCycle].empty()) {
            BypassEventsByVReg[Event.VReg][BypassCycle] += " ";
          }
          BypassEventsByVReg[Event.VReg][BypassCycle] += Event.toString();
        }
      }
    }
  }

  // Print header with cycle numbers.
  // Reserve 12 characters for register class names to handle long names.
  OS << " RegClass    VReg  |";
  for (unsigned Cycle = 0; Cycle < Schedule.size(); ++Cycle) {
    OS << format(" %4d |", Cycle);
  }
  OS << "\n";

  // Print separator.
  OS << "-------------------+";
  for (unsigned Cycle = 0; Cycle < Schedule.size(); ++Cycle) {
    OS << "------+";
  }
  OS << "\n";

  // Helper lambda to print a row of events
  auto PrintEventRow = [&](const std::map<unsigned, std::string> &Events) {
    for (unsigned Cycle = 0; Cycle < Schedule.size(); ++Cycle) {
      auto It = Events.find(Cycle);
      OS << format(" %-4s |", It != Events.end() ? It->second.c_str() : "");
    }
    OS << "\n";
  };

  // Print each VReg with register events and bypass events on separate lines.
  for (Register VReg : AllVRegs) {
    const auto Reg = VReg.virtRegIndex();
    const char *RCName = TRI.getRegClassName(MRI.getRegClass(VReg));

    // Print register events.
    // Use %-12.12s to left-align, pad to 12 chars, and truncate at 12 chars.
    OS << format(" %-12.12s%5d |", RCName, Reg);
    PrintEventRow(RegEventsByVReg[VReg]);

    // Print bypass events if any exist for this VReg.
    const auto &BypassEvents = BypassEventsByVReg[VReg];
    if (!BypassEvents.empty()) {
      OS << "         bypass    |";
      PrintEventRow(BypassEvents);
    }
  }
}

// Helper function to get lane mask for a register operand
static LaneBitmask getLaneMaskFor(const TargetRegisterInfo &TRI,
                                  const MachineRegisterInfo &MRI,
                                  unsigned SubRegIdx, Register VReg) {
  if (SubRegIdx == 0) {
    // Full/composite register - get the actual lane mask from register class
    const TargetRegisterClass *RC = MRI.getRegClass(VReg);
    return RC->getLaneMask();
  }
  // Specific subregister
  return TRI.getSubRegIndexLaneMask(SubRegIdx);
}

DenseMap<Register, AIE::LivenessVector>
AIEScheduleInterpreter::buildLiveLanes(const EventSchedule &Schedule,
                                       int II) const {

  assert(II > 0 && "Initiation interval must be positive");

  DenseMap<Register, AIE::LivenessVector> LiveLanesByVirtReg;

  if (Schedule.empty())
    return LiveLanesByVirtReg;

  // State: tracks which lanes are currently live when scanning backward
  DenseMap<Register, LaneBitmask> ActiveMask;

  // Process cycles backward
  int MaxCycle = Schedule.size() - 1;
  for (int C = MaxCycle; C >= 0; --C) {
    const auto &Events = Schedule[C];
    int ModuloCycle = C % II;

    // First, record what's live ENTERING this cycle (before any events)
    // This is what was active from processing later cycles
    for (const auto &[VReg, Mask] : ActiveMask) {
      if (Mask.any()) {
        // Ensure the output vector is sized for this VReg
        if (!LiveLanesByVirtReg.count(VReg)) {
          LiveLanesByVirtReg[VReg] = AIE::LivenessVector(II);
        }
        LiveLanesByVirtReg[VReg][ModuloCycle] |= Mask;

        LLVM_DEBUG(dbgs() << "    Lanes " << PrintLaneMask(Mask) << " for %vreg"
                          << VReg.virtRegIndex() << " live entering cycle " << C
                          << " (offset " << ModuloCycle << ")\n");
      }
    }

    // Collect reads for this cycle (they don't make register live in this
    // cycle)
    DenseMap<Register, LaneBitmask> ReadsInCycle;

    // Step 1: Process defs (writes) - they occupy the register and kill lanes
    // going backward
    for (const auto &Event : Events) {
      if (Event.Type == EventType::Write) {
        LaneBitmask M = getLaneMaskFor(TRI, MRI, Event.SubRegIdx, Event.VReg);

        // Ensure the output vector exists for this VReg
        if (!LiveLanesByVirtReg.count(Event.VReg)) {
          LiveLanesByVirtReg[Event.VReg] = AIE::LivenessVector(II);
        }

        // RF write occupies register file at ModuloCycle
        LiveLanesByVirtReg[Event.VReg][ModuloCycle] |= M;

        // If this write uses a bypass, mark bypass write one cycle earlier
        if (Event.ForwardingClass != 0) {
          const int BypassWriteCycle = C - 1;
          if (BypassWriteCycle >= 0) {
            const int BypassModuloCycle = BypassWriteCycle % II;
            LiveLanesByVirtReg[Event.VReg][BypassModuloCycle].addBypassWrite(
                Event.ForwardingClass);

            LLVM_DEBUG(dbgs()
                       << "    Bypass write of class " << Event.ForwardingClass
                       << " at cycle " << BypassWriteCycle << " (offset "
                       << BypassModuloCycle << ")\n");
          }
        }

        // Kill those lanes going backward
        ActiveMask[Event.VReg] &= ~M;

        LLVM_DEBUG(dbgs() << "  Cycle " << C << " (" << ModuloCycle
                          << "): Write %vreg" << Event.VReg.virtRegIndex();
                   if (Event.SubRegIdx) dbgs()
                   << ":" << TRI.getSubRegIndexName(Event.SubRegIdx);
                   dbgs() << " occupies lanes " << PrintLaneMask(M)
                          << " and kills them going backward\n");

        // If no lanes remain active, remove from map
        if (ActiveMask[Event.VReg].none()) {
          ActiveMask.erase(Event.VReg);
        }
      }
    }

    // Step 2: Collect all reads in this cycle
    for (const auto &Event : Events) {
      if (Event.Type == EventType::Read) {
        LaneBitmask M = getLaneMaskFor(TRI, MRI, Event.SubRegIdx, Event.VReg);

        // Accumulate reads for this VReg in this cycle
        ReadsInCycle[Event.VReg] |= M;

        LLVM_DEBUG(dbgs() << "  Cycle " << C << " (" << ModuloCycle
                          << "): Read %vreg" << Event.VReg.virtRegIndex();
                   if (Event.SubRegIdx) dbgs()
                   << ":" << TRI.getSubRegIndexName(Event.SubRegIdx);
                   dbgs() << " lanes " << PrintLaneMask(M) << "\n");

        // If this read uses a bypass, mark bypass read at same cycle
        if (Event.ForwardingClass != 0) {
          if (!LiveLanesByVirtReg.count(Event.VReg)) {
            LiveLanesByVirtReg[Event.VReg] = AIE::LivenessVector(II);
          }
          LiveLanesByVirtReg[Event.VReg][ModuloCycle].addBypassRead(
              Event.ForwardingClass);

          LLVM_DEBUG(dbgs() << "    Bypass read of class "
                            << Event.ForwardingClass << " at cycle " << C
                            << " (offset " << ModuloCycle << ")\n");
        }
      }
    }

    // Step 3: Now propagate reads to ActiveMask for previous cycles
    // Reads don't make the register live in the current cycle
    for (const auto &[VReg, Mask] : ReadsInCycle) {
      // The reads make the register live going backward (but not in this cycle)
      ActiveMask[VReg] |= Mask;

      LLVM_DEBUG(dbgs() << "    %vreg" << VReg.virtRegIndex() << " lanes "
                        << PrintLaneMask(Mask)
                        << " become live going backward from cycle " << C
                        << "\n");
    }
  }

  // At the end, ActiveMask should be empty (all defs should have been seen)
  // If not, we have uses without defs (which would be an error in def-first
  // semantics)
  for (const auto &[VReg, Mask] : ActiveMask) {
    if (Mask.any()) {
      LLVM_DEBUG(dbgs() << "Warning: %vreg" << VReg.virtRegIndex()
                        << " has lanes " << PrintLaneMask(Mask)
                        << " live at beginning (use without def?)\n");
    }
  }

  return LiveLanesByVirtReg;
}

void AIEScheduleInterpreter::dumpLiveLanes(
    const DenseMap<Register, AIE::LivenessVector> &LiveLanesByVirtReg, int II,
    raw_ostream &OS) const {

  if (LiveLanesByVirtReg.empty()) {
    OS << "No live lanes data\n";
    return;
  }

  // Collect and sort VRegs for consistent output.
  SmallVector<Register, 16> VRegs;
  for (const auto &[VReg, _] : LiveLanesByVirtReg) {
    VRegs.push_back(VReg);
  }
  llvm::sort(VRegs, [](Register A, Register B) {
    return A.virtRegIndex() < B.virtRegIndex();
  });

  OS << "Live Lanes (II=" << II << "):\n";
  OS << "VReg   | ";
  for (int T = 0; T < II; ++T) {
    OS << format("t%-6d ", T);
  }
  OS << "\n";

  OS << "-------+";
  for (int T = 0; T < II; ++T) {
    OS << "--------";
  }
  OS << "\n";

  for (Register VReg : VRegs) {
    OS << format("%-6d | ", VReg.virtRegIndex());

    const auto &LanesByOffset = LiveLanesByVirtReg.lookup(VReg);
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
        if (L.getLanes().any()) {
          Indicator = "#";
        }

        // Add bypass read classes.
        if (!L.getBypassReads().empty()) {
          Indicator += "R";
          for (unsigned FC : L.getBypassReads()) {
            Indicator += std::to_string(FC);
          }
        }

        // Add bypass write classes.
        if (!L.getBypassWrites().empty()) {
          Indicator += "W";
          for (unsigned FC : L.getBypassWrites()) {
            Indicator += std::to_string(FC);
          }
        }

        // Pad to 6 characters for alignment.
        while (Indicator.size() < 6) {
          Indicator += " ";
        }
        OS << " " << Indicator << " ";
      } else {
        OS << " ..     ";
      }
    }
    OS << "\n";
  }
}
