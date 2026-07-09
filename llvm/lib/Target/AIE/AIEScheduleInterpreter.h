//===- AIEScheduleInterpreter.h - Schedule-aware itinerary interpreter ---===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains a schedule-aware interpreter that computes register
// file (RF) occupancy windows from scheduled MachineInstrs and itinerary
// data, enabling cycle-accurate interference computation.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIESCHEDULEINTERPRETER_H
#define LLVM_LIB_TARGET_AIE_AIESCHEDULEINTERPRETER_H

#include "AIELivenessVector.h"
#include "llvm/ADT/DenseMap.h"
#include <string>
#include <vector>

namespace llvm {

struct AIEBaseInstrInfo;
class MachineFunction;
class MachineInstr;
class MachineRegisterInfo;
class RegLiveRangeTracker;
class TargetRegisterInfo;
class InstrItineraryData;

/// Event types for register file access
enum class EventType { Read, Write };

/// Event structure to track register accesses
class RFEvent {
public:
  EventType Type;           // Read or Write
  unsigned LRIndex;         // Live range index (RegLiveRange::getIndex())
  unsigned SubRegIdx;       // Subregister index (0 for full register)
  unsigned ForwardingClass; // Forwarding/bypass class (0 = no bypass)
  const MachineInstr *MI;   // Source instruction
  unsigned OpIdx;           // Operand index

  RFEvent(EventType T, unsigned LRI, unsigned S, unsigned F,
          const MachineInstr *M, unsigned O)
      : Type(T), LRIndex(LRI), SubRegIdx(S), ForwardingClass(F), MI(M),
        OpIdx(O) {}

  /// Format the event action as a short string suitable for tabular display.
  ///
  /// Returns a 3-character wide string: the action character ('R' or 'W')
  /// followed by the two-digit subregister index if non-zero, or two spaces
  /// for a full-register access.
  std::string toString() const;
};

/// Event schedule indexed by cycle
using EventSchedule = std::vector<std::vector<RFEvent>>;

/// Schedule interpreter that computes RF occupancy windows
class AIEScheduleInterpreter {
  const AIEBaseInstrInfo &TII;
  const TargetRegisterInfo &TRI;
  const MachineRegisterInfo &MRI;
  const InstrItineraryData *Itin;

  /// Get the cycle offset when an operand is accessed given a scheduling class
  /// Returns the offset from issue cycle
  int getOperandCycle(unsigned SchedClass, unsigned OpIdx) const;

public:
  explicit AIEScheduleInterpreter(const MachineFunction &MF);

  /// Return the absolute cycle at which operand \p OpIdx of \p MI is
  /// accessed when the instruction is issued at \p IssueCycle.
  int getOperandAccessCycle(const MachineInstr &MI, int IssueCycle,
                            unsigned OpIdx) const;

  /// Add events for a single instruction to the event schedule.
  ///
  /// Processes all tracked register operands and maps them to their compact
  /// live range index via \p Tracker.getIndexForOperand().  Operands with no
  /// matching live range are skipped.
  ///
  /// \param MI The machine instruction to process
  /// \param IssueCycle The cycle when the instruction is issued
  /// \param Schedule The event schedule to update (will be resized if needed)
  /// \param Tracker The live range tracker providing the operand → LRIndex map.
  void addInstructionEvents(const MachineInstr &MI, int IssueCycle,
                            EventSchedule &Schedule,
                            const RegLiveRangeTracker &Tracker) const;

  /// Dump the event schedule in a tabular format
  ///
  /// Displays cycles in rows and live range indices in aligned columns,
  /// showing 'R' for reads and 'W' for writes.
  ///
  /// \param Schedule The event schedule to dump
  /// \param Tracker The live range tracker used to look up register class names
  /// \param OS Output stream to write to
  void dumpEventSchedule(const EventSchedule &Schedule,
                         const RegLiveRangeTracker &Tracker,
                         raw_ostream &OS) const;

  /// Build per-lane modulo-II live range masks from an event schedule
  ///
  /// Uses a backward scan to compute which lanes of each live range are live
  /// at each modulo-II offset. The result is a map from LRIndex to a
  /// LivenessVector, where LiveLanesByLRIndex[LRIndex][t] indicates which
  /// lanes are live at offset t (0 <= t < II).
  ///
  /// \param Schedule The event schedule to analyze
  /// \param II The initiation interval for modulo scheduling
  /// \param Tracker The live range tracker used to look up register classes
  /// \return Map of LRIndex to per-offset lane masks
  DenseMap<unsigned, AIE::LivenessVector>
  buildLiveLanes(const EventSchedule &Schedule, int II,
                 const RegLiveRangeTracker &Tracker) const;

  /// Dump the live lanes in a readable format
  ///
  /// \param LiveLanesByLRIndex The live lanes data to dump
  /// \param II The initiation interval
  /// \param OS Output stream to write to
  void dumpLiveLanes(
      const DenseMap<unsigned, AIE::LivenessVector> &LiveLanesByLRIndex, int II,
      raw_ostream &OS) const;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIESCHEDULEINTERPRETER_H
