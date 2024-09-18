//===- AIEVariableInstrItineraryEmitter.cpp - Variabel Instr Itinerary info
// generator ----*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "Common/CodeGenInstruction.h"
#include "Common/CodeGenTarget.h"
#include "llvm/Support/Debug.h"
#include "llvm/TableGen/Error.h"
#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/TableGenBackend.h"

using namespace llvm;

#define DEBUG_TYPE "aie-alternate-itinerary-emitter"

namespace {

class AIEVariableInstrItineraryEmitter {

  std::map<
      std::vector<std::tuple<
          llvm::StringRef, std::vector<std::pair<llvm::StringRef, unsigned>>>>,
      std::vector<llvm::StringRef>>
      UniqueRegItineraryOpIdx;

public:
  AIEVariableInstrItineraryEmitter(RecordKeeper &R);
  void run(raw_ostream &OS);
  void emitAltItineraryInfo(raw_ostream &OS);
  void emitOperandIndexInfo(raw_ostream &OS);

private:
  RecordKeeper &Records;
  CodeGenTarget Target;
};

} // namespace

AIEVariableInstrItineraryEmitter::AIEVariableInstrItineraryEmitter(
    RecordKeeper &R)
    : Records(R), Target(R) {}

void AIEVariableInstrItineraryEmitter::emitAltItineraryInfo(raw_ostream &OS) {
  const std::string CurrentNamespace = Target.getName().str();
  const std::string EndOfFunction("\n}\n\n");

  OS << "unsigned " << CurrentNamespace << "InstrInfo::getSchedClass(\n";
  OS << "    const MCInstrDesc &Desc,\n";
  OS << "    iterator_range<const MachineOperand *> Operands,\n";
  OS << "    const MachineRegisterInfo &MRI) const {\n";
  if (UniqueRegItineraryOpIdx.empty()) {
    OS << "  return Desc.getSchedClass();" << EndOfFunction;
    return;
  }

  OS << "  if (Operands.begin() == Operands.end())\n";
  OS << "    return Desc.getSchedClass();\n\n";

  OS << "  auto checkRCForOperand =\n";
  OS << "      [&Operands, &MRI](const TargetRegisterClass &ExpectedRC, "
        "unsigned OpIdx) {\n";
  OS << "        const MachineOperand &MO = *std::next(Operands.begin(), "
        "OpIdx);\n";
  OS << "        assert(MO.isReg() && \"Operand must be of type Reg\");\n";
  OS << "        const Register Reg = MO.getReg();\n";
  OS << "        const TargetRegisterClass *RC =\n";
  OS << "            Reg.isVirtual() ? MRI.getRegClass(Reg) : nullptr;\n";
  OS << "        return AIEBaseInstrInfo::regClassMatches(ExpectedRC, RC, "
        "Reg);\n";
  OS << "      };\n\n";

  OS << "  switch (Desc.getOpcode()) {\n";
  OS << "  default:\n";
  OS << "    return Desc.getSchedClass();\n";
  for (const auto &[RegItineraryPairs, Instrs] : UniqueRegItineraryOpIdx) {
    for (const auto &Instr : Instrs)
      OS << "  case " << CurrentNamespace << "::" << Instr << ":\n";
    OS << "  {\n";
    for (const auto &[Itinerary, RegClassList] : RegItineraryPairs) {
      assert(!RegClassList.empty() && "RegClassList cannot be empty");
      OS << "    if (";
      unsigned Count = 0;
      for (const auto &[RegClass, OpIdx] : RegClassList) {
        if (Count)
          OS << " &&\n        ";
        OS << "checkRCForOperand(" << CurrentNamespace << "::" << RegClass
           << "RegClass," << OpIdx << ")";
        Count++;
      }
      OS << ")\n";
      OS << "      return " << CurrentNamespace << "::" << "Sched"
         << "::" << Itinerary << ";\n";
    }
    OS << "    LLVM_DEBUG(dbgs() << \"No matching RegClass found for "
          "instruction: \"\n";
    OS << "      << Desc.getOpcode() << \"\\n\");\n";
    OS << "    return Desc.getSchedClass();\n";
    OS << "  }\n";
  }
  OS << "  }" << EndOfFunction;
}

void AIEVariableInstrItineraryEmitter::run(raw_ostream &OS) {
  Records.startTimer("Process definitions");

  ArrayRef<const CodeGenInstruction *> NumberedInstructions =
      Target.getInstructionsByEnumValue();

  for (const CodeGenInstruction *CGI : NumberedInstructions) {
    Record *R = CGI->TheDef;
    if ((R->getValueAsString("Namespace") == "TargetOpcode") ||
        (!R->getValue("Inst") && !R->getValue("isMultiSlotPseudo")))
      continue;

    std::vector<Record *> AltItinary =
        R->getValueAsListOfDefs("ItineraryRegPairs");
    if (AltItinary.size()) {
      std::vector<std::tuple<llvm::StringRef,
                             std::vector<std::pair<llvm::StringRef, unsigned>>>>
          RegItineraryPairs;
      for (const auto AltItinerary : AltItinary) {
        std::vector<Record *> OpRegType =
            AltItinerary->getValueAsListOfDefs("RegTypeList");
        std::vector<std::pair<llvm::StringRef, unsigned>> OperandRegClass;

        auto checkUnique = [&OperandRegClass](unsigned OpIdx) {
          return !std::any_of(
              OperandRegClass.begin(), OperandRegClass.end(),
              [OpIdx](const auto &Operand) { return Operand.second == OpIdx; });
        };

        for (const auto RegType : OpRegType) {
          unsigned OpIdx = RegType->getValueAsInt("OperandIdx");
          assert(checkUnique(OpIdx) && "OperandIdx must be unique");
          OperandRegClass.push_back(std::make_pair(
              RegType->getValueAsDef("RegClass")->getName(), OpIdx));
        }

        assert(!OperandRegClass.empty() && "OperandRegClass cannot be empty");
        RegItineraryPairs.push_back(
            std::make_tuple(AltItinerary->getValueAsDef("Itinerary")->getName(),
                            OperandRegClass));
      }
      UniqueRegItineraryOpIdx[RegItineraryPairs].push_back(R->getName());
    }
  }

  // Generate code to access scheduling information for instructions with
  // itininary based on RegClass used.
  Records.startTimer(
      "Emit Instruction with Alternate Itininary based on RegClass used");
  emitSourceFileHeader("Instruction itininary based on RegClass", OS);
  emitAltItineraryInfo(OS);
}

static TableGen::Emitter::OptClass<AIEVariableInstrItineraryEmitter>
    X("gen-aie-alternate-itinerary-emitter",
      "Generate scheduling info for instuctions with alternate itineraries "
      "based on register types");
