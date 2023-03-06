//===- AIEPreSchedLoweringEmitter.cpp - Lowering info Generator -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "CodeGenInstruction.h"
#include "CodeGenTarget.h"
#include "llvm/Support/Debug.h"
#include "llvm/TableGen/Error.h"
#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/TableGenBackend.h"

using namespace llvm;

#define DEBUG_TYPE "aie-presched-lowering"

namespace {

/// A helper class to generate information about pre-scheduling lowering
/// of pseudo instructions.
class AIEPreSchedLoweringEmitter {
public:
  AIEPreSchedLoweringEmitter(RecordKeeper &R) : Records(R), Target(R) {}

  /// Generate C++ code from \p Expansions.
  void emitLoweringInfo(raw_ostream &OS);

  /// Process the tablegen records, looking for those derived from
  /// PreSchedInstExpansion.
  void run(raw_ostream &OS);

private:
  /// Process a record derived from Instruction and PreSchedInstExpansion.
  /// This populates \p Expansions.
  void evaluateExpansion(Record &Rec);

  RecordKeeper &Records;
  CodeGenTarget Target;

  struct PreSchedExpansion {
    CodeGenInstruction SourceInstr; // The source pseudo instruction definition.
    CodeGenInstruction TargetInstr; // The target instruction to lower to.
    CodeGenInstruction BarrierInstr; // The barrier that comes with it.
  };
  SmallVector<PreSchedExpansion, 32> Expansions;
};

} // namespace

void AIEPreSchedLoweringEmitter::evaluateExpansion(Record &Rec) {
  LLVM_DEBUG(dbgs() << "Pre-sched expansion: " << Rec.getName() << "\n");
  CodeGenInstruction SourceInstr(&Rec);
  DefInit *TargetInstrDef =
      cast<DefInit>(Rec.getValue("TargetInstr")->getValue());
  CodeGenInstruction TargetInstr(TargetInstrDef->getDef());
  DefInit *BarrierInstrDef =
      cast<DefInit>(Rec.getValue("BarrierInstr")->getValue());
  CodeGenInstruction BarrierInstr(BarrierInstrDef->getDef());
  Expansions.emplace_back(
      PreSchedExpansion{SourceInstr, TargetInstr, BarrierInstr});
}

// Generate something like:
// std::optional<AIEBaseInstrInfo::PseudoBranchExpandInfo>
// AIE2InstrInfo::getPseudoBranchExpandInfo(const MachineInstr &MI) const {
//   switch (MI.getOpcode()) {
//   case AIE2::PseudoJNZ:
//     return PseudoBranchExpandInfo{AIE2::JNZ, AIE2::DelayedSchedBarrier};
//   case AIE2::PseudoJZ:
//     return PseudoBranchExpandInfo{AIE2::JZ, AIE2::DelayedSchedBarrier};
//   default:
//     return {};
//   }
// }
void AIEPreSchedLoweringEmitter::emitLoweringInfo(raw_ostream &OS) {
  // Emit file header.
  emitSourceFileHeader("Pre-scheduling MI lowering Source Fragment", OS);

  OS << "std::optional<AIEBaseInstrInfo::PseudoBranchExpandInfo>\n";
  OS << Target.getName() << "InstrInfo::"
     << "getPseudoBranchExpandInfo(const MachineInstr &MI) const {\n";

  if (!Expansions.empty()) {
    OS << "  switch (MI.getOpcode()) {\n"
       << "  default: return {};\n";
    for (PreSchedExpansion &Expansion : Expansions) {
      CodeGenInstruction &Source = Expansion.SourceInstr;
      CodeGenInstruction &Dest = Expansion.TargetInstr;
      CodeGenInstruction &Barrier = Expansion.BarrierInstr;
      OS << "  case " << Source.Namespace << "::" << Source.TheDef->getName()
         << ":\n";
      OS << "    return PseudoBranchExpandInfo{" << Dest.Namespace
         << "::" << Dest.TheDef->getName() << ", " << Barrier.Namespace
         << "::" << Barrier.TheDef->getName() << "};\n";
    }
    OS << "  }";
  } else
    OS << "  return {};";

  OS << "\n}\n\n";
}

void AIEPreSchedLoweringEmitter::run(raw_ostream &OS) {
  StringRef Classes[] = {"PreSchedInstExpansion", "Instruction"};

  Records.startTimer("Process definitions");
  for (Record *R : Records.getAllDerivedDefinitions(Classes))
    evaluateExpansion(*R);

  // Generate expansion code to lower the pseudo to an MCInst of the real
  // instruction.
  Records.startTimer("Emit expansion code");
  emitLoweringInfo(OS);
}

static TableGen::Emitter::OptClass<AIEPreSchedLoweringEmitter>
    X("gen-aie-presched-lowering", "Pre-Scheduling lowering Emitter");
