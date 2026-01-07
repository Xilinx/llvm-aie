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
#include "llvm/CodeGen/Register.h"
#include <string>
#include <vector>

namespace llvm {

struct AIEBaseInstrInfo;
class MachineFunction;
class MachineInstr;
class MachineRegisterInfo;
class TargetRegisterInfo;
class InstrItineraryData;

/// Event types for register file access
enum class EventType { Read, Write };

/// Event structure to track register accesses
class RFEvent {
public:
  EventType Type;           // Read or Write
  Register VReg;            // Virtual register
  unsigned SubRegIdx;       // Subregister index (0 for full register)
  unsigned ForwardingClass; // Forwarding/bypass class (0 = no bypass)
  const MachineInstr *MI;   // Source instruction
  unsigned OpIdx;           // Operand index

  RFEvent(EventType T, Register V, unsigned S, unsigned F,
          const MachineInstr *M, unsigned O)
      : Type(T), VReg(V), SubRegIdx(S), ForwardingClass(F), MI(M), OpIdx(O) {}

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

  /// Add events for a single instruction to the event schedule
  ///
  /// Processes all register operands of the instruction and adds their
  /// read/write events to the schedule based on the issue cycle and
  /// itinerary timing information.
  ///
  /// \param MI The machine instruction to process
  /// \param IssueCycle The cycle when the instruction is issued
  /// \param Schedule The event schedule to update (will be resized if needed)
  void addInstructionEvents(const MachineInstr &MI, int IssueCycle,
                            EventSchedule &Schedule) const;

  /// Dump the event schedule in a tabular format
  ///
  /// Displays cycles in rows and virtual registers in aligned columns,
  /// showing 'R' for reads and 'W' for writes.
  ///
  /// \param Schedule The event schedule to dump
  /// \param OS Output stream to write to
  void dumpEventSchedule(const EventSchedule &Schedule, raw_ostream &OS) const;

  /// Build per-lane modulo-II live range masks from an event schedule
  ///
  /// Uses a backward scan to compute which lanes of each virtual register
  /// are live at each modulo-II offset. The result is a map from VReg to
  /// a LaneMaskVector, where LiveLanesByVirtReg[VReg][t] indicates
  /// which lanes are live at offset t (0 <= t < II).
  ///
  /// \param Schedule The event schedule to analyze
  /// \param II The initiation interval for modulo scheduling
  /// \return Map of VReg to per-offset lane masks
  DenseMap<Register, AIE::LivenessVector>
  buildLiveLanes(const EventSchedule &Schedule, int II) const;

  /// Dump the live lanes in a readable format
  ///
  /// \param LiveLanesByVirtReg The live lanes data to dump
  /// \param II The initiation interval
  /// \param OS Output stream to write to
  void dumpLiveLanes(
      const DenseMap<Register, AIE::LivenessVector> &LiveLanesByVirtReg, int II,
      raw_ostream &OS) const;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIESCHEDULEINTERPRETER_H
