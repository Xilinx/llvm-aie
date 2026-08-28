//===- AIEItineraryEquivalence.h - Itinerary Equivalence Detection -*- C++ -*-//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file provides utilities for detecting equivalent itineraries.
// Two itineraries are equivalent if they have identical scheduling behavior,
// i.e., the same Stages and OperandCycles.
//
// This is useful for:
// - Consolidating schedule class IDs in VarItinerary tables
// - Reducing table data by storing only one copy per equivalence class
// - Optimizing lookup time
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_UTILS_TABLEGEN_AIEITINERARYEQUIVALENCE_H
#define LLVM_UTILS_TABLEGEN_AIEITINERARYEQUIVALENCE_H

#include "Common/CodeGenSchedule.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Debug.h"
#include "llvm/TableGen/Record.h"
#include <cassert>
#include <map>
#include <vector>

#define DEBUG_TYPE "aie-itinerary-equivalence"

namespace llvm {

/// Captures the scheduling-relevant fields of a single pipeline stage.
///
/// The stage record's name is a debug label and is excluded from comparison.
/// What matters is the flat data: how many cycles the stage occupies, which
/// functional units it requires, how many cycles elapse before the next stage
/// begins (TimeInc), and whether it carries a special timing model marker such
/// as Reserved.
struct StageCycleInfo {
  /// Length of the stage in machine cycles.
  int Cycles = 0;

  /// Functional unit names the stage can use.
  std::vector<StringRef> UnitNames;

  /// Cycles until the start of the next stage (cycle delta between consecutive
  /// stages).
  int TimeInc = 0;

  /// Timing model record name (e.g., "NoItinerary", "Reserved").
  StringRef TimingModelName;

  bool operator==(const StageCycleInfo &Other) const {
    return Cycles == Other.Cycles && TimeInc == Other.TimeInc &&
           UnitNames == Other.UnitNames &&
           TimingModelName == Other.TimingModelName;
  }

  bool operator<(const StageCycleInfo &Other) const {
    if (Cycles != Other.Cycles)
      return Cycles < Other.Cycles;
    if (TimeInc != Other.TimeInc)
      return TimeInc < Other.TimeInc;
    if (UnitNames != Other.UnitNames)
      return UnitNames < Other.UnitNames;
    return TimingModelName < Other.TimingModelName;
  }
};

/// Represents the scheduling signature of an itinerary - used to detect
/// equivalent itineraries that have identical scheduling behavior.
///
/// Two itineraries with the same signature have identical scheduling
/// characteristics and can be treated as equivalent for scheduling purposes.
struct ItinerarySignature {
  /// Per-stage scheduling data. Each entry captures the flat field values of
  /// one stage (Cycles, functional units, TimeInc, timing model), which fully
  /// determine its scheduling behavior. Stage record names are debug labels
  /// and are not included.
  std::vector<StageCycleInfo> Stages;

  /// Operand cycle latencies.
  std::vector<int64_t> OperandCycles;

  /// Bypass identifiers - using bypass record names for comparison.
  std::vector<StringRef> BypassNames;

  bool operator<(const ItinerarySignature &Other) const {
    if (Stages != Other.Stages)
      return Stages < Other.Stages;
    if (OperandCycles != Other.OperandCycles)
      return OperandCycles < Other.OperandCycles;
    return BypassNames < Other.BypassNames;
  }
};

/// Computes the scheduling signature for an itinerary record.
///
/// \param ItinData The InstrItinData record to compute the signature for.
///                 Must not be null.
/// \returns The scheduling signature for the record.
inline ItinerarySignature computeItinerarySignature(const Record *ItinData) {
  assert(ItinData && "ItinData must not be null");

  ItinerarySignature Sig;

  // Extract per-stage scheduling data. For each stage we read the actual
  // field values (Cycles, Units, TimeInc, TM) that determine scheduling
  // behavior. The stage record's name is a debug label and is intentionally
  // excluded from the signature.
  if (ItinData->getValue("Stages")) {
    for (const Record *Stage : ItinData->getValueAsListOfDefs("Stages")) {
      StageCycleInfo Info;
      Info.Cycles = static_cast<int>(Stage->getValueAsInt("Cycles"));
      Info.TimeInc = static_cast<int>(Stage->getValueAsInt("TimeInc"));
      if (Stage->getValue("Units")) {
        for (const Record *Unit : Stage->getValueAsListOfDefs("Units"))
          Info.UnitNames.push_back(Unit->getName());
      }
      if (Stage->getValue("TM")) {
        const Record *TM = Stage->getValueAsDef("TM");
        if (TM)
          Info.TimingModelName = TM->getName();
      }
      Sig.Stages.push_back(std::move(Info));
    }
  }

  // Extract OperandCycles.
  if (ItinData->getValue("OperandCycles")) {
    Sig.OperandCycles = ItinData->getValueAsListOfInts("OperandCycles");
  }

  // Extract Bypass information.
  if (ItinData->getValue("Bypasses")) {
    for (const Record *Bypass : ItinData->getValueAsListOfDefs("Bypasses")) {
      Sig.BypassNames.push_back(Bypass->getName());
    }
  }

  return Sig;
}

/// Manages equivalence relationships between itineraries.
///
/// This class builds and maintains a mapping from itinerary names to their
/// canonical representative. Itineraries with identical signatures map to
/// the same representative, which is the first itinerary encountered with
/// that signature.
class ItineraryEquivalenceMap {
  /// Maps itinerary name to its representative (canonical) itinerary name.
  std::map<StringRef, StringRef> EquivalenceMap;

  /// Maps itinerary name to its signature.
  std::map<StringRef, ItinerarySignature> ItineraryToSignature;

  /// Maps signature to the representative itinerary name.
  std::map<ItinerarySignature, StringRef> SignatureToRepresentative;

  /// Statistics for debugging.
  unsigned NumEquivalent = 0;
  unsigned NumTotal = 0;

public:
  /// Build the equivalence map from scheduling models.
  ///
  /// \param SchedModels The scheduling models containing schedule classes.
  /// \param ItinModel The processor model with itinerary definitions.
  void build(const CodeGenSchedModels &SchedModels,
             const CodeGenProcModel &ItinModel) {
    // Process all schedule classes to build the equivalence map.
    for (const CodeGenSchedClass &SchedClass :
         SchedModels.explicitSchedClasses()) {
      // Some schedule classes have no itinerary entry; skip them.
      if (SchedClass.Index >= ItinModel.ItinDefList.size())
        continue;
      const Record *ItinData = ItinModel.ItinDefList[SchedClass.Index];
      if (!ItinData)
        continue;
      addItinerary(SchedClass.Name, computeItinerarySignature(ItinData));
    }

    LLVM_DEBUG(dbgs() << "ItineraryEquivalenceMap: Found " << NumEquivalent
                      << " equivalent itineraries out of " << NumTotal
                      << " total\n");
  }

  /// Add an itinerary to the equivalence map.
  ///
  /// \param ItinName The name of the itinerary.
  /// \param Sig The scheduling signature of the itinerary.
  void addItinerary(StringRef ItinName, const ItinerarySignature &Sig) {
    ++NumTotal;

    // Store the signature for this itinerary.
    ItineraryToSignature[ItinName] = Sig;

    auto It = SignatureToRepresentative.find(Sig);
    if (It == SignatureToRepresentative.end()) {
      // This is the first itinerary with this signature - it becomes the
      // representative.
      SignatureToRepresentative[Sig] = ItinName;
      EquivalenceMap[ItinName] = ItinName;
    } else {
      // Map this itinerary to the existing representative.
      EquivalenceMap[ItinName] = It->second;
      ++NumEquivalent;
      LLVM_DEBUG(dbgs() << "Itinerary " << ItinName << " is equivalent to "
                        << It->second << "\n");
    }
  }

  /// Find the representative itinerary name for a given itinerary.
  ///
  /// \param ItinName The name of the itinerary to look up.
  /// \returns The representative itinerary name, or ItinName if not found.
  StringRef getRepresentative(StringRef ItinName) const {
    auto It = EquivalenceMap.find(ItinName);
    if (It != EquivalenceMap.end())
      return It->second;
    // If not found in the map, return the original name.
    return ItinName;
  }

  /// Check if an itinerary is the representative of its equivalence class.
  bool isRepresentative(StringRef ItinName) const {
    auto It = EquivalenceMap.find(ItinName);
    if (It != EquivalenceMap.end())
      return It->first == It->second;
    return true;
  }

  /// Get the number of equivalent (non-representative) itineraries found.
  unsigned getNumEquivalent() const { return NumEquivalent; }

  /// Get the total number of itineraries processed.
  unsigned getNumTotal() const { return NumTotal; }

  /// Get the number of unique equivalence classes (representatives).
  unsigned getNumEquivalenceClasses() const {
    return SignatureToRepresentative.size();
  }
};

} // namespace llvm

#undef DEBUG_TYPE

#endif // LLVM_UTILS_TABLEGEN_AIEITINERARYEQUIVALENCE_H
