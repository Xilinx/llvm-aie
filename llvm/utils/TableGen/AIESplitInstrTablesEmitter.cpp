//===- AIESplitInstrTablesEmitter.cpp - Lowering info Generator -*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "CodeGenInstruction.h"
#include "CodeGenTarget.h"
#include "llvm/Support/Debug.h"
#include "llvm/TableGen/Error.h"
#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/TableGenBackend.h"

using namespace llvm;

#define DEBUG_TYPE "aie-split-instr-tables"

namespace {

/// A helper class to generate tables mapping instructions with their
/// _split variants.
class AIESplitInstrTablesEmitter {
public:
  AIESplitInstrTablesEmitter(RecordKeeper &R) : Records(R), Target(R) {}

  /// Generate C++ code from \p Mappings.
  void emitMappingInfo(raw_ostream &OS, bool ToSplit);

  /// Process the tablegen records, looking for those derived from
  /// SplitPseudo.
  void run(raw_ostream &OS);

private:
  /// Process a record derived from Instruction and SplitInstMapping.
  /// This populates \p Mappings.
  void evaluateMapping(Record &Rec);

  RecordKeeper &Records;
  CodeGenTarget Target;

  struct SplitInstMapping {
    CodeGenInstruction OrigInstr;  // The fully-fledged target instruction
    CodeGenInstruction SplitInstr; // The _split variant which facilitates RA
  };
  SmallVector<SplitInstMapping, 32> Mappings;
};

} // namespace

void AIESplitInstrTablesEmitter::evaluateMapping(Record &Rec) {
  LLVM_DEBUG(dbgs() << "_split instruction record: " << Rec.getName() << "\n");
  CodeGenInstruction SplitInstr(&Rec);
  DefInit *OrigInstrDef =
      cast<DefInit>(Rec.getValue("OriginalInstr")->getValue());
  CodeGenInstruction OrigInstr(OrigInstrDef->getDef());
  Mappings.emplace_back(SplitInstMapping{OrigInstr, SplitInstr});
}

// Generate something like:
//
// std::optional<unsigned>
// AIE2InstrInfo::getOpcodeWithAtomicOperands(unsigned Opcode) const {
//   switch (Opcode) {
//   case AIE2::PADDA_2D: return AIE2::PADDA_2D_split;
//   default: return {};
//   }
// }
void AIESplitInstrTablesEmitter::emitMappingInfo(raw_ostream &OS,
                                                 bool ToSplit) {
  OS << "std::optional<unsigned>\n";
  OS << Target.getName() << "InstrInfo::";
  if (ToSplit)
    OS << "getOpcodeWithAtomicOperands(unsigned Opcode) const {\n";
  else
    OS << "getOpcodeWithTupleOperands(unsigned Opcode) const {\n";

  if (!Mappings.empty()) {
    OS << "  switch (Opcode) {\n"
       << "  default: return {};\n";
    for (SplitInstMapping &Mapping : Mappings) {
      CodeGenInstruction &Source =
          ToSplit ? Mapping.OrigInstr : Mapping.SplitInstr;
      CodeGenInstruction &Dest =
          ToSplit ? Mapping.SplitInstr : Mapping.OrigInstr;
      OS << "  case " << Source.Namespace << "::" << Source.TheDef->getName()
         << ": return " << Dest.Namespace << "::" << Dest.TheDef->getName()
         << ";\n";
    }
    OS << "  }";
  } else
    OS << "  return {};";

  OS << "\n}\n\n";
}

void AIESplitInstrTablesEmitter::run(raw_ostream &OS) {
  Records.startTimer("Process definitions");
  for (Record *R : Records.getAllDerivedDefinitions("SplitPseudo"))
    evaluateMapping(*R);

  // Generate tables to map original instructions to their _split counterpart.
  Records.startTimer("Emit expansion code");
  emitSourceFileHeader("Mapping info for _split instructions Source Fragment",
                       OS);
  emitMappingInfo(OS, /*ToSplit=*/true);
  emitMappingInfo(OS, /*ToSplit=*/false);
}

static TableGen::Emitter::OptClass<AIESplitInstrTablesEmitter>
    X("gen-aie-split-instr-tables", "Split instr tables Emitter");
