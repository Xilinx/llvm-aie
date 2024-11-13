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

#include "AIEAlternateDescriptors.h"
#include "AIEBaseSubtarget.h"
#include "AIEBundle.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/CodeGen/LiveInterval.h"
#include "llvm/CodeGen/ResourceCycle.h"
#include "llvm/CodeGen/ResourceScoreboard.h"
#include "llvm/CodeGen/ScheduleHazardRecognizer.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrItineraries.h"

namespace llvm {

class MachineInstr;

void applyFormatOrdering(AIE::MachineBundle &Bundle, const VLIWFormat &Format,
                         MachineInstr *BundleRoot,
                         MachineBasicBlock::iterator InsertPoint);

/// Return all the instructions bundled within \p MI, or itself if
/// it isn't a BUNDLE.
/// \param IncludeRoot Whether to include the BUNDLE instr in the range
inline MachineBasicBlock::instr_range bundled_instrs(MachineInstr &MI,
                                                     bool IncludeRoot = false) {
  MachineBasicBlock::instr_iterator It = MI.getIterator();
  if (!MI.isBundle())
    return make_range(It, std::next(It));
  auto Begin = IncludeRoot ? It : std::next(It);
  return make_range(Begin, getBundleEnd(MI.getIterator()));
}

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

  /// The occupied bank
  MemoryBankBits MemoryBanks = 0;

public:
  /// IssueCount - Count instructions issued in this cycle.
  unsigned IssueCount = 0;

  FuncUnitWrapper(const InstrStage &IS, SlotBits Slots = 0,
                  MemoryBankBits MemoryBanks = 0)
      : Required(IS.getReservationKind() == InstrStage::Required ? IS.getUnits()
                                                                 : 0),
        Reserved(IS.getReservationKind() == InstrStage::Reserved ? IS.getUnits()
                                                                 : 0),
        Slots(Slots), MemoryBanks(MemoryBanks) {}

  static void setFormatInterface(const AIEBaseMCFormats *Formats);

  /// Check whether this cycle is empty
  bool isEmpty() const;

  /// Make this an empty cycle;
  void clearResources();
  /// Make this conflict with any non-empty cycle
  void blockResources();
  FuncUnitWrapper() = default;
  FuncUnitWrapper(InstrStage::FuncUnits Req) : Required(Req), Reserved(0) {}
  FuncUnitWrapper(InstrStage::FuncUnits Req, InstrStage::FuncUnits Res,
                  SlotBits Slots = 0, MemoryBankBits MemoryBanks = 0)
      : Required(Req), Reserved(Res), Slots(Slots), MemoryBanks(MemoryBanks) {}

  /// Compare two FuncUnitWrappers for equality. This is only used for
  /// dumping purposes, quite literally saying "this looks the same"
  bool operator==(const FuncUnitWrapper &Other) const;

  /// Dump a readable version
  void dump() const;

  FuncUnitWrapper &operator|=(const FuncUnitWrapper &Other);
  bool conflict(const FuncUnitWrapper &Other) const;
};

/// This Hazard Recognizer is primarily intended to work together
/// with PostRASchedulerList to implement an in-order VLIW scheduling
/// model without interlocks.
class AIEHazardRecognizer : public ScheduleHazardRecognizer {
  int PipelineDepth = -1;
  int MaxLatency = -1;

  /// Compute the limits from the itinerary data
  void computeMaxLatency();

public:
  /// ScoreboardDepth can be used to speficy a fixed depth without querying the
  /// scheduling model. This is mostly used for testing, for other cases we
  /// should trust the instruction itineraries.
  AIEHazardRecognizer(const AIEBaseInstrInfo *TII, const InstrItineraryData *II,
                      AIEAlternateDescriptors &SelectedAlternateDescs,
                      bool IsPreRA,
                      std::optional<unsigned> ScoreboardDepth = std::nullopt);
  AIEHazardRecognizer(const TargetSubtargetInfo &SubTarget,
                      AIEAlternateDescriptors &SelectedAlternateDescs,
                      bool IsPreRA = false);

  ~AIEHazardRecognizer() override {}

  void Reset() override;
  HazardType getHazardType(SUnit *SU, int DeltaCycles) override;
  void EmitInstruction(SUnit *) override;
  void EmitInstruction(SUnit *, int DeltaCycles) override;
  void AdvanceCycle() override;
  void RecedeCycle() override;

  /// Check conflict with Other shifted by DeltaCycles into the
  /// future relative to *this.
  bool conflict(const AIEHazardRecognizer &Other, int DeltaCycles) const;

  /// Bundle real instructions and move meta instructions afterwards.
  /// TODO: Once PRAS is dropped, this function should not be in here.
  static void applyBundles(const std::vector<AIE::MachineBundle> &Bundles,
                           MachineBasicBlock *MBB);

  /// Sets the number of cycles (from the current cycle) for which instructions
  /// cannot be scheduled, with the exception of delay slot instructions.
  /// That number is decremented every time the current cycle is advanced
  /// or receded, until it reaches 0. In the latter case, instructions can be
  /// issued again.
  void setReservedCycles(unsigned Cycles);

  /// Update Scoreboard for Schedclass at DeltaCycles.
  /// \param FUDepthLimit Restricts the depth to which itinerary
  ///        resources are recorded in the scoreboard. This is mainly for
  ///        use from the pre-RA scheduler, where detailed resource modelling
  ///        doesn't pay off.
  void emitInScoreboard(ResourceScoreboard<FuncUnitWrapper> &Scoreboard,
                        const MCInstrDesc &Desc, MemoryBankBits MemoryBanks,
                        iterator_range<const MachineOperand *> MIOperands,
                        const MachineRegisterInfo &MRI, int DeltaCycles) const;
  // Apply the above function to the local scoreboard.
  void emitInScoreboard(const MCInstrDesc &Desc, MemoryBankBits MemoryBanks,
                        iterator_range<const MachineOperand *> MIOperands,
                        const MachineRegisterInfo &MRI, int DeltaCycles);

  /// Block all scoreboard resources at DeltaCycles
  void blockCycleInScoreboard(int DeltaCycle);

  /// Recede the scoreboard by N cycles
  void recedeScoreboard(int N);

  // Dump the scoreboard
  void dumpScoreboard() const;

  /// The instructions with memory bank attribute return the address space
  /// number
  MemoryBankBits getMemoryBanks(const MachineInstr *MI) const;

  /// The pipeline depth is the depth of the deepest instruction.
  /// We compute that once from the itineraries.
  unsigned getPipelineDepth() const;

  /// The maximum latency is the worst case latency we can get according to the
  /// operand latencies. This may be overestimating, since we don't take
  /// bypasses or the difference between RAW, WAW and WAR into account.
  unsigned getMaxLatency() const;

  /// This is the maximum of the pipeline depth and the maximum latency. It is
  /// the maximum distance at which two instructions can influence each other.
  int getConflictHorizon() const;

  /// The default scoreboard depth is twice the pipeline depth, so that
  /// we can insert in the past during backward scheduling.
  /// For efficiency, this size is rounded up to a power of two.
  unsigned computeScoreboardDepth() const;

  AIEAlternateDescriptors &getSelectedAltDescs() const {
    return SelectedAltDescs;
  }

  ScheduleHazardRecognizer::HazardType
  getHazardType(const ResourceScoreboard<FuncUnitWrapper> &TheScoreboard,
                const MCInstrDesc &Desc, MemoryBankBits MemoryBanks,
                iterator_range<const MachineOperand *> MIOperands,
                const MachineRegisterInfo &MRI, int DeltaCycles) const;
  bool checkConflict(const ResourceScoreboard<FuncUnitWrapper> &Scoreboard,
                     MachineInstr &MI, int DeltaCycles) const;

protected:
  ScheduleHazardRecognizer::HazardType getHazardType(const MCInstrDesc &Desc,
                                                     int DeltaCycles);
  static bool
  checkConflict(const ResourceScoreboard<FuncUnitWrapper> &Scoreboard,
                const InstrItineraryData *ItinData, unsigned SchedClass,
                SlotBits SlotSet, MemoryBankBits MemoryBanks,
                SmallVector<int, 2> MemoryAccessCycles, int DeltaCycles,
                std::optional<int> FUDepthLimit);

  static void enterResources(ResourceScoreboard<FuncUnitWrapper> &Scoreboard,
                             const InstrItineraryData *ItinData,
                             unsigned SchedClass, SlotBits SlotSet,
                             MemoryBankBits MemoryBanks,
                             SmallVector<int, 2> MemoryAccessCycles,
                             int DeltaCycles, std::optional<int> FUDepthLimit);

private:
  ResourceScoreboard<FuncUnitWrapper> Scoreboard;
  const AIEBaseInstrInfo *TII;
  const InstrItineraryData *ItinData;
  AIEAlternateDescriptors &SelectedAltDescs;
  static int NumInstrsScheduled;
  unsigned IssueLimit = 1;
  unsigned ReservedCycles = 0;

  bool IsPreRA = false;

  // Ignore FuncUnits past a certain pipeline depth.
  // This is set to std::nullopt for the post-RA scheduler.
  std::optional<int> FUDepthLimit;

  // Allow instructions without a known slot to be added to any Bundle.
  // This is set to false for the post-RA scheduler.
  bool IgnoreUnknownSlotSets = false;
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
