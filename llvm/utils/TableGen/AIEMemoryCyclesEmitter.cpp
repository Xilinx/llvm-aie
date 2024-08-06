//===- AIEMemoryCycleEmitter.cpp - Memory ops info Generator ----*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "Common/CodeGenDAGPatterns.h"
#include "Common/CodeGenInstruction.h"
#include "Common/CodeGenSchedule.h"
#include "Common/CodeGenTarget.h"
#include "llvm/Support/Debug.h"
#include "llvm/TableGen/Error.h"
#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/TableGenBackend.h"
#include <limits>

using namespace llvm;

#define DEBUG_TYPE "aie-memory-cycles"

namespace {

/// Tablegen generator for {TargetName}InstrInfo::getFirstMemoryCycle and
/// {TargetName}InstrInfo::getLastMemoryCycle.
/// This looks for MemInstrItinData records to know when memory instructions
/// actually access memory.
class AIEMemoryCyclesEmitter {
  struct MemoryCycles {
    const CodeGenSchedClass *SchedClass; // The sched class.
    unsigned FirstCycle; // The cycle for the first memory operation.
    unsigned LastCycle;  // The cycle for the last memory operation.
  };

public:
  AIEMemoryCyclesEmitter(RecordKeeper &R);

  /// Utility class computing minima and maxima
  class MemCycleGetter {
  public:
    int Min = std::numeric_limits<int>::max();
    int Max = std::numeric_limits<int>::min();
    MemCycleGetter(bool LastCycles) : LastCycles(LastCycles){};
    unsigned operator()(const AIEMemoryCyclesEmitter::MemoryCycles &MC) {
      int Result = int(LastCycles ? MC.LastCycle : MC.FirstCycle);
      Min = std::min(Min, Result);
      Max = std::max(Max, Result);
      return Result;
    }
    bool LastCycles = false;
  };

  /// Generate C++ code from \p ItinMemCycles.
  void emitMemoryCyclesInfo(raw_ostream &OS, MemCycleGetter &Access);

  /// Process the SchedClasses, looking for those with MemoryCycles.
  void run(raw_ostream &OS);

private:
  /// Process a SchedClass and look if it is associated with an itinerary with
  /// MemoryCycles through a MemInstrItinData record.
  /// This populates \p ItinMemCycles.
  void evaluateSchedClass(const CodeGenSchedClass &SchedClass);

  RecordKeeper &Records;
  CodeGenTarget Target;
  CodeGenDAGPatterns CDP;
  const CodeGenSchedModels &SchedModels;
  const CodeGenProcModel *ItinModel = nullptr;

  std::vector<MemoryCycles> ItinMemCycles;
};

} // namespace

AIEMemoryCyclesEmitter::AIEMemoryCyclesEmitter(RecordKeeper &R)
    : Records(R), Target(R), CDP(R),
      SchedModels(CDP.getTargetInfo().getSchedModels()) {
  assert(SchedModels.hasItineraries());
  for (const CodeGenProcModel &ProcModel : SchedModels.procModels()) {
    if (ProcModel.hasItineraries()) {
      assert(ItinModel == nullptr && "Multiple Itin models");
      ItinModel = &ProcModel;
    }
  }
}

void AIEMemoryCyclesEmitter::evaluateSchedClass(
    const CodeGenSchedClass &SchedClass) {
  assert(SchedClass.Index < ItinModel->ItinDefList.size());
  const Record *ItinData = ItinModel->ItinDefList[SchedClass.Index];
  LLVM_DEBUG(dbgs() << "Checking Memory cycles for " << SchedClass.Name << "("
                    << SchedClass.Index << ")\n");
  if (!ItinData || !ItinData->isSubClassOf("MemInstrItinData")) {
    LLVM_DEBUG(dbgs() << "  No MemoryCycles.\n");
    return;
  }

  int FirstCycle = ItinData->getValueAsInt("FirstMemCycle");
  int LastCycle = ItinData->getValueAsInt("LastMemCycle");
  LLVM_DEBUG(dbgs() << "  Found MemoryCycles First=" << FirstCycle
                    << " Last=" << LastCycle << "\n");
  if (FirstCycle < 0 || LastCycle < 0) {
    PrintFatalError(ItinData->getLoc(), "Negative MemoryCycles");
  }
  if (FirstCycle > LastCycle) {
    PrintFatalError(ItinData->getLoc(),
                    "FirstMemCycle greater than LastMemCycle");
  }
  ItinMemCycles.emplace_back(
      MemoryCycles{&SchedClass, unsigned(FirstCycle), unsigned(LastCycle)});
}

// Generate something like:
// std::optional<int>
// AIE2InstrInfo::getFirstMemoryCycle(unsigned SchedClass) const {
//   switch (SchedClass) {
//   default: return {};
//   case 4: return 5; // II_VLDA_2D_UPS
//   case 6: return 7; // II_VST_2D_SRS
//   case 8: return 7; // II_VST_2D_CONV
//   }
// }
void AIEMemoryCyclesEmitter::emitMemoryCyclesInfo(raw_ostream &OS,
                                                  MemCycleGetter &Access) {
  const std::string Prefix(std::string(Target.getName()) + "InstrInfo::");
  const std::string FirstOrLast(Access.LastCycles ? "Last" : "First");
  const std::string EndOfFunction("\n}\n\n");
  OS << "std::optional<int>\n";
  OS << Prefix << "get" << FirstOrLast
     << "MemoryCycle(unsigned SchedClass) const {\n";

  OS << "  switch (SchedClass) {\n"
     << "  default: return {};\n";
  for (const MemoryCycles &MemCycles : ItinMemCycles) {
    unsigned Cycle = Access(MemCycles);
    OS << "  case " << MemCycles.SchedClass->Index << ": return " << Cycle
       << "; // " << MemCycles.SchedClass->Name << "\n";
  }
  OS << "  }" << EndOfFunction;

  OS << "int " << Prefix << "getMin" << FirstOrLast
     << "MemoryCycle() const {\n  return " << Access.Min << ";"
     << EndOfFunction;
  OS << "int " << Prefix << "getMax" << FirstOrLast
     << "MemoryCycle() const {\n  return " << Access.Max << ";"
     << EndOfFunction;
}

void AIEMemoryCyclesEmitter::run(raw_ostream &OS) {
  Records.startTimer("Process definitions");
  for (const CodeGenSchedClass &SchedClass : SchedModels.explicit_classes())
    evaluateSchedClass(SchedClass);

  // Generate code to access scheduling information for memory instructions.
  Records.startTimer("Emit memory ops scheduling info");
  emitSourceFileHeader("Memory ops scheduling info Source Fragment", OS);
  MemCycleGetter GetFirst(/*LastCycles=*/false);
  MemCycleGetter GetLast(/*LastCycles=*/true);
  emitMemoryCyclesInfo(OS, GetFirst);
  emitMemoryCyclesInfo(OS, GetLast);
}

static TableGen::Emitter::OptClass<AIEMemoryCyclesEmitter>
    X("gen-aie-memory-cycles", "Generate memory ops scheduling info");