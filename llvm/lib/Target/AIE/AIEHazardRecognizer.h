//===--- AIEHazardRecognizer.h - AIE Post RA Hazard Recognizer ----===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// This file defines the hazard recognizer for scheduling on AIE.
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEHAZARDRECOGNIZER_H
#define LLVM_LIB_TARGET_AIE_AIEHAZARDRECOGNIZER_H

#include "AIEBaseSubtarget.h"
#include "AIEBundle.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/CodeGen/LiveInterval.h"
#include "llvm/CodeGen/ResourceCycle.h"
#include "llvm/CodeGen/ResourceScoreboard.h"
#include "llvm/CodeGen/ScheduleHazardRecognizer.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrItineraries.h"

namespace llvm {

// To be merged with AIEResourceCycle
class FuncUnitWrapper {
  /// The format interface to interpret bundle constraints
  static const AIEBaseMCFormats *FormatInterface;

  /// Bitset of the required resources
  InstrStage::FuncUnits Required = 0;
  /// Bitset of the reserved resources
  InstrStage::FuncUnits Reserved = 0;

  /// The occupied slots. This is currently redundant with Bundle
  SlotBits Slots = 0;

public:
  /// IssueCount - Count instructions issued in this cycle.
  unsigned IssueCount = 0;

  FuncUnitWrapper(const InstrStage &IS, SlotBits Slots = 0)
      : Required(IS.getReservationKind() == InstrStage::Required ? IS.getUnits()
                                                                 : 0),
        Reserved(IS.getReservationKind() == InstrStage::Reserved ? IS.getUnits()
                                                                 : 0),
        Slots(Slots) {}

  static void setFormatInterface(const AIEBaseMCFormats *Formats);

  /// Check whether this cycle is empty
  bool isEmpty() const;

  /// Make this an empty cycle;
  void clearResources();
  /// Make this conflict with any non-empty cycle
  void blockResources();
  FuncUnitWrapper() = default;
  FuncUnitWrapper(InstrStage::FuncUnits Req) : Required(Req), Reserved(0) {}
  FuncUnitWrapper(InstrStage::FuncUnits Req, InstrStage::FuncUnits Res)
      : Required(Req), Reserved(Res) {}

  /// Dump a readable version
  void dump() const;

  FuncUnitWrapper &operator|=(const FuncUnitWrapper &Other);
  bool conflict(const FuncUnitWrapper &Other) const;
};

/// This Hazard Recognizer is primarily intended to work together
/// with PostRASchedulerList to implement an in-order VLIW scheduling
/// model without interlocks.
class AIEHazardRecognizer : public ScheduleHazardRecognizer {
public:
  AIEHazardRecognizer(const AIEBaseInstrInfo *TII, const InstrItineraryData *II,
                      const ScheduleDAG *DAG);

  ~AIEHazardRecognizer() override {}

  void Reset() override;
  HazardType getHazardType(SUnit *SU, int DeltaCycles) override;
  bool emitNoopsIfNoInstructionsAvailable() override { return true; }
  void EmitInstruction(SUnit *) override;
  void EmitInstruction(SUnit *, int DeltaCycles) override;
  void EmitNoop() override;
  unsigned PreEmitNoops(SUnit *) override;
  void AdvanceCycle() override;
  void RecedeCycle() override;

  /// Check conflict with Other shifted by DeltaCycles into the
  /// future relative to *this.
  bool conflict(const AIEHazardRecognizer &Other, int DeltaCycles) const;

  /// Obsolescent interfaces for PRAS
  void StartBlock(MachineBasicBlock *MBB) override;
  void EndBlock(MachineBasicBlock *MBB) override;

  /// Bundle real instructions and move meta instructions afterwards.
  /// TODO: Once PRAS is dropped, this function should not be in here.
  static void applyBundles(const std::vector<AIE::MachineBundle> &Bundles,
                           MachineBasicBlock *MBB);

  bool currentCycleHasInstr(bool CountMetaInstrs = false) const;

  const std::vector<AIE::MachineBundle> &getBundles() const { return Bundles; }
  std::vector<AIE::MachineBundle> &getBundles() { return Bundles; }

  /// Sets the number of cycles (from the current cycle) for which instructions
  /// cannot be scheduled, with the exception of delay slot instructions.
  /// That number is decremented every time the current cycle is advanced
  /// or receded, until it reaches 0. In the latter case, instructions can be
  /// issued again.
  void setReservedCycles(unsigned Cycles);

  /// Update the scoreboard for Schedclass at DeltaCycles. It only touches the
  /// scoreboard, in particular, it doesn't add instructions to the bundle.
  /// \param FUDepthLimit Restricts the depth to which itinerary
  ///        resources are recorded in the scoreboard. This is mainly for
  ///        use from the pre-RA scheduler, where detailed resource modelling
  ///        doesn't pay off.
  void emitInScoreboard(unsigned SchedClass, int DeltaCycles,
                        std::optional<int> FUDepthLimit);

  /// Block all scoreboard resources at DeltaCycles
  void blockCycleInScoreboard(int DeltaCycle);

  /// Recede the scoreboard by N cycles
  void recedeScoreboard(int N);

  // Dump the scoreboard
  void dumpScoreboard() const;

protected:
  ScheduleHazardRecognizer::HazardType
  getHazardType(unsigned SchedClass, int DeltaCycles,
                std::optional<int> FUDepthLimit);

  unsigned computeScoreboardDepth() const;

private:
  ResourceScoreboard<FuncUnitWrapper> Scoreboard;
  const AIEBaseInstrInfo *TII;
  const InstrItineraryData *ItinData;
  static int NumInstrsScheduled;
  std::vector<AIE::MachineBundle> Bundles;
  AIE::MachineBundle CurrentBundle;
  unsigned IssueLimit = 1;
  unsigned ReservedCycles = 0;
};

// Provide the DFAPacketizer interface for the MachinePipeliner
// For now, just track the format constraints, using Bundle
class AIEResourceCycle : public ResourceCycle {
  AIE::MachineBundle Bundle;

public:
  AIEResourceCycle(const AIEBaseMCFormats *FormatInterface)
      : Bundle(FormatInterface) {}
  void clearResources() override { Bundle.clear(); }
  bool canReserveResources(MachineInstr &MI) override;
  void reserveResources(MachineInstr &MI) override;

  // For now, never called from MachinePipeliner
  // FIXME: perhaps expose an opcode interface in Bundle
  bool canReserveResources(const MCInstrDesc *MID) override { return false; }
  void reserveResources(const MCInstrDesc *MID) override {}
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEHAZARDRECOGNIZER_H
