//===- AIEVariableInstrItineraryEmitter.cpp - Variable Instr Itinerary info
// generator ----*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This TableGen backend generates table-driven schedule class selection based
// on operand register classes. It produces:
// 1. A flat table of operand register class requirements
// 2. A table of schedule class variants with ArrayRefs into the flat table
// 3. A function for itinerary lookup, getSchedClass().
//
//===----------------------------------------------------------------------===//

#include "AIEItineraryEquivalence.h"
#include "Common/CodeGenDAGPatterns.h"
#include "Common/CodeGenInstruction.h"
#include "Common/CodeGenRegisters.h"
#include "Common/CodeGenSchedule.h"
#include "Common/CodeGenTarget.h"
#include "ConstTable.h"
#include "llvm/Support/Debug.h"
#include "llvm/TableGen/Error.h"
#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/TGTimer.h"
#include "llvm/TableGen/TableGenBackend.h"
#include <set>

using namespace llvm;

#define DEBUG_TYPE "aie-alternate-itinerary-emitter"

namespace {

// Structure to hold the parsed information for each instruction variant.
struct SchedVariantData {
  // The itinerary/sched class name (e.g., "II_ADD_NC_mv_add_ri_mSRF2IFlags").
  StringRef ItineraryName;
  // List of (OpIdx, RegClassName) pairs for this variant.
  std::vector<std::pair<unsigned, StringRef>> OperandRCs;
  // Original index in the instruction's variant list, used to preserve
  // ordering after consolidation.
  unsigned OrigIdx = 0;
};

// Structure to hold all variants for a single instruction.
struct InstrVariantData {
  // The instruction name.
  StringRef InstrName;
  // All variants for this instruction.
  std::vector<SchedVariantData> Variants;
};

class AIEVariableInstrItineraryEmitter {
  // All instructions with their variant data, for table generation.
  std::vector<InstrVariantData> InstrVariants;

public:
  AIEVariableInstrItineraryEmitter(const RecordKeeper &R);
  void run(raw_ostream &OS);

private:
  void emitTables(raw_ostream &OS);
  void emitInterface(raw_ostream &OS);

  // Consolidate variants by finding register class unions for variants with
  // equivalent itineraries.
  void consolidateVariants();

  // Check if two register classes have compatible attributes (SpillSize,
  // Allocatable, CopyCost, etc.) for consolidation.
  static bool haveCompatibleAttributes(const CodeGenRegisterClass &A,
                                       const CodeGenRegisterClass &B);

  // Find a register class whose members are exactly the union of the input
  // classes and has compatible attributes. Returns nullptr if none exists.
  const CodeGenRegisterClass *
  findUnionClass(ArrayRef<const CodeGenRegisterClass *> RCs);

  // Try to merge a group of Multiplicity variants in WorkSet at a given
  // operand position. Tries all candidate subsets to find one with a valid
  // union class. On success, mutates WorkSet and returns true.
  bool tryMergeAtPosition(std::vector<SchedVariantData> &WorkSet,
                          size_t BaseIdx, int DiffPos,
                          ArrayRef<size_t> CandidateIndices,
                          unsigned Multiplicity, StringRef RepItin);

  const RecordKeeper &Records;
  CodeGenTarget Target;
  CodeGenDAGPatterns CDP;
  const CodeGenSchedModels &SchedModels;
  CodeGenRegBank &RegBank;
  const CodeGenProcModel *ItinModel = nullptr;
  std::string CurrentNamespace;

  // Manages itinerary equivalence relationships for schedule class
  // consolidation.
  ItineraryEquivalenceMap ItinEquivMap;
};

} // namespace

AIEVariableInstrItineraryEmitter::AIEVariableInstrItineraryEmitter(
    const RecordKeeper &R)
    : Records(R), Target(R), CDP(R),
      SchedModels(CDP.getTargetInfo().getSchedModels()),
      RegBank(Target.getRegBank()) {
  CurrentNamespace = Target.getName().str();

  // Find the itinerary model and build equivalence map.
  if (SchedModels.hasItineraries()) {
    for (const CodeGenProcModel &ProcModel : SchedModels.procModels()) {
      if (ProcModel.hasItineraries()) {
        assert(ItinModel == nullptr && "Multiple Itin models");
        ItinModel = &ProcModel;
      }
    }

    if (ItinModel)
      ItinEquivMap.build(SchedModels, *ItinModel);
  }
}

bool AIEVariableInstrItineraryEmitter::haveCompatibleAttributes(
    const CodeGenRegisterClass &A, const CodeGenRegisterClass &B) {
  // Check that all relevant attributes match.
  // RSI contains SpillSize and SpillAlignment per HwMode.
  if (A.RSI != B.RSI)
    return false;

  if (A.CopyCost != B.CopyCost)
    return false;

  if (A.Allocatable != B.Allocatable)
    return false;

  if (A.AllocationPriority != B.AllocationPriority)
    return false;

  if (A.TSFlags != B.TSFlags)
    return false;

  return true;
}

const CodeGenRegisterClass *AIEVariableInstrItineraryEmitter::findUnionClass(
    ArrayRef<const CodeGenRegisterClass *> RCs) {
  if (RCs.empty())
    return nullptr;

  if (RCs.size() == 1)
    return RCs[0];

  // First check that all input classes have compatible attributes.
  for (size_t I = 1; I < RCs.size(); ++I) {
    if (!haveCompatibleAttributes(*RCs[0], *RCs[I])) {
      LLVM_DEBUG(dbgs() << "  Cannot consolidate: " << RCs[0]->getName()
                        << " and " << RCs[I]->getName()
                        << " have incompatible attributes\n");
      return nullptr;
    }
  }

  // Compute the union of all registers.
  std::set<const CodeGenRegister *> UnionRegs;
  for (const CodeGenRegisterClass *RC : RCs) {
    for (const CodeGenRegister *Reg : RC->getMembers())
      UnionRegs.insert(Reg);
  }

  // Search for a register class that:
  // 1. Has exactly the union of registers.
  // 2. Has compatible attributes with all input classes.
  for (const CodeGenRegisterClass &Candidate : RegBank.getRegClasses()) {
    const auto &CandidateMembers = Candidate.getMembers();
    if (CandidateMembers.size() != UnionRegs.size())
      continue;

    const bool AllMembersMatch =
        llvm::all_of(CandidateMembers, [&](const CodeGenRegister *Reg) {
          return UnionRegs.find(Reg) != UnionRegs.end();
        });

    if (!AllMembersMatch)
      continue;

    if (!haveCompatibleAttributes(Candidate, *RCs[0]))
      continue;

    LLVM_DEBUG(dbgs() << "  Found union class: " << Candidate.getName()
                      << " for {";
               for (const auto *RC : RCs) dbgs() << RC->getName() << " ";
               dbgs() << "}\n");

    return &Candidate;
  }

  return nullptr;
}

bool AIEVariableInstrItineraryEmitter::tryMergeAtPosition(
    std::vector<SchedVariantData> &WorkSet, size_t BaseIdx, int DiffPos,
    ArrayRef<size_t> CandidateIndices, unsigned Multiplicity,
    StringRef RepItin) {
  const SchedVariantData &Base = WorkSet[BaseIdx];
  const size_t NumNeeded = Multiplicity - 1;

  // Try consecutive windows of NumNeeded candidates from the candidate list.
  for (size_t Start = 0; Start + NumNeeded <= CandidateIndices.size();
       ++Start) {
    std::vector<size_t> GroupIndices = {BaseIdx};
    for (size_t I = Start; I < Start + NumNeeded; ++I)
      GroupIndices.push_back(CandidateIndices[I]);

    // Collect the register classes at the differing position.
    std::vector<const CodeGenRegisterClass *> RCs;
    bool AllValid = true;
    for (const size_t GI : GroupIndices) {
      const StringRef RCName = WorkSet[GI].OperandRCs[DiffPos].second;
      const Record *RCDef = Records.getDef(RCName);
      if (!RCDef) {
        AllValid = false;
        break;
      }
      CodeGenRegisterClass *RC = RegBank.getRegClass(RCDef);
      if (!RC) {
        AllValid = false;
        break;
      }
      RCs.push_back(RC);
    }

    if (!AllValid)
      continue;

    const CodeGenRegisterClass *UnionRC = findUnionClass(RCs);
    if (!UnionRC)
      continue;

    // Build the merged variant, preserving the original index of the base.
    SchedVariantData Merged;
    Merged.ItineraryName = RepItin;
    Merged.OrigIdx = Base.OrigIdx;
    for (size_t K = 0; K < Base.OperandRCs.size(); ++K) {
      if (static_cast<int>(K) == DiffPos) {
        Merged.OperandRCs.emplace_back(Base.OperandRCs[K].first,
                                       UnionRC->getName());
      } else {
        Merged.OperandRCs.push_back(Base.OperandRCs[K]);
      }
    }

    // Replace the base variant with the merged one.
    WorkSet[GroupIndices[0]] = std::move(Merged);

    // Remove the other variants in reverse order to preserve indices.
    for (size_t RI = GroupIndices.size() - 1; RI >= 1; --RI)
      WorkSet.erase(WorkSet.begin() + GroupIndices[RI]);

    return true;
  }

  return false;
}

void AIEVariableInstrItineraryEmitter::consolidateVariants() {
  unsigned TotalVariantsBefore = 0;
  unsigned TotalVariantsAfter = 0;

  LLVM_DEBUG(dbgs() << "ItineraryEquivalenceMap stats: "
                    << ItinEquivMap.getNumEquivalent() << " equivalent out of "
                    << ItinEquivMap.getNumTotal() << " total, "
                    << ItinEquivMap.getNumEquivalenceClasses()
                    << " equivalence classes\n");

  for (InstrVariantData &InstrData : InstrVariants) {
    TotalVariantsBefore += InstrData.Variants.size();

    // Group variants by equivalence class. Use the global representative
    // as the grouping key to ensure only genuinely equivalent itineraries
    // are grouped together.
    std::map<StringRef, std::vector<size_t>> RepToVariantIndices;
    for (size_t I = 0; I < InstrData.Variants.size(); ++I) {
      const StringRef OrigItin = InstrData.Variants[I].ItineraryName;
      const StringRef Rep = ItinEquivMap.getRepresentative(OrigItin);
      RepToVariantIndices[Rep].push_back(I);
    }

    std::vector<SchedVariantData> ConsolidatedVariants;

    for (auto &[Rep, Indices] : RepToVariantIndices) {
      // Use the first variant's itinerary as the instruction-local
      // representative.
      const StringRef LocalRepItin =
          InstrData.Variants[Indices[0]].ItineraryName;
      if (Indices.size() == 1) {
        ConsolidatedVariants.push_back(
            std::move(InstrData.Variants[Indices[0]]));
        continue;
      }

      // Check if all variants have the same operand indices.
      std::set<unsigned> ExpectedOpIndices;
      for (const auto &[OpIdx, RCName] :
           InstrData.Variants[Indices[0]].OperandRCs)
        ExpectedOpIndices.insert(OpIdx);

      auto GetOpIndices = [&](size_t Idx) {
        std::set<unsigned> OpIndices;
        for (const auto &[OpIdx, RCName] : InstrData.Variants[Idx].OperandRCs)
          OpIndices.insert(OpIdx);
        return OpIndices;
      };

      const bool AllSameOperands =
          llvm::all_of(ArrayRef(Indices).drop_front(), [&](size_t Idx) {
            return GetOpIndices(Idx) == ExpectedOpIndices;
          });

      if (!AllSameOperands) {
        for (size_t Idx : Indices)
          ConsolidatedVariants.push_back(std::move(InstrData.Variants[Idx]));
        continue;
      }

      // Build a working set of variants to consolidate.
      std::vector<SchedVariantData> WorkSet;
      for (size_t Idx : Indices)
        WorkSet.push_back(InstrData.Variants[Idx]);

      // Try N-tuple consolidation for N = 2, then N = 4.
      for (unsigned Multiplicity : {2u, 4u}) {
        bool Changed = true;
        while (Changed) {
          Changed = false;
          if (WorkSet.size() < Multiplicity)
            break;

          for (size_t I = 0; I < WorkSet.size() && !Changed; ++I) {
            const SchedVariantData &Vi = WorkSet[I];

            // Find variants that differ in exactly one operand position.
            std::map<int, std::vector<size_t>> DiffPosToIndices;
            for (size_t J = I + 1; J < WorkSet.size(); ++J) {
              const SchedVariantData &Vj = WorkSet[J];
              if (Vi.OperandRCs.size() != Vj.OperandRCs.size())
                continue;

              int DifferingOpIdx = -1;
              bool Compatible = true;
              for (size_t K = 0; K < Vi.OperandRCs.size(); ++K) {
                if (Vi.OperandRCs[K].first != Vj.OperandRCs[K].first) {
                  Compatible = false;
                  break;
                }
                if (Vi.OperandRCs[K].second != Vj.OperandRCs[K].second) {
                  if (DifferingOpIdx >= 0) {
                    Compatible = false;
                    break;
                  }
                  DifferingOpIdx = static_cast<int>(K);
                }
              }

              if (Compatible && DifferingOpIdx >= 0)
                DiffPosToIndices[DifferingOpIdx].push_back(J);
            }

            for (auto &[DiffPos, Candidates] : DiffPosToIndices) {
              if (Candidates.size() + 1 < Multiplicity)
                continue;
              if (!tryMergeAtPosition(WorkSet, I, DiffPos, Candidates,
                                      Multiplicity, LocalRepItin))
                continue;
              Changed = true;
              break;
            }
          }
        }
      }

      LLVM_DEBUG(if (WorkSet.size() < Indices.size()) {
        dbgs() << "Consolidated " << Indices.size() << " -> " << WorkSet.size()
               << " variants for " << InstrData.InstrName
               << " (rep: " << LocalRepItin << ")\n";
      });

      // All variants in this equivalence group use the local representative.
      for (auto &V : WorkSet) {
        V.ItineraryName = LocalRepItin;
        ConsolidatedVariants.push_back(std::move(V));
      }
    }

    // Sort by original index to preserve the input ordering. Merged variants
    // inherit the index of their first constituent, so they appear at the
    // position of the earliest original variant.
    std::sort(ConsolidatedVariants.begin(), ConsolidatedVariants.end(),
              [](const SchedVariantData &A, const SchedVariantData &B) {
                return A.OrigIdx < B.OrigIdx;
              });

    InstrData.Variants = std::move(ConsolidatedVariants);
    TotalVariantsAfter += InstrData.Variants.size();
  }

  LLVM_DEBUG(dbgs() << "Variant consolidation: " << TotalVariantsBefore
                    << " -> " << TotalVariantsAfter << " ("
                    << (TotalVariantsBefore - TotalVariantsAfter)
                    << " removed)\n");
}

void AIEVariableInstrItineraryEmitter::emitTables(raw_ostream &OS) {
  // Note: OperandRCRequirement, SchedVariantInfo, InstrVariantInfo, and
  // VarItinInterface are defined in AIEBaseInstrInfo.h.

  // Table 1: Flat table of operand register class requirements.
  ConstTable OpReqTable("llvm::OperandRCRequirement", "OperandRCRequirements");

  // Table 2: Schedule class variants with ArrayRefs into OpReqTable.
  ConstTable VariantsTable("SchedVariantInfo", "SchedVariants");

  // Table 3: Instruction to variants mapping with ArrayRefs into
  // VariantsTable.
  ConstTable InstrTable("InstrVariantInfo", "InstrVariantInfos");

  for (const InstrVariantData &InstrData : InstrVariants) {
    VariantsTable.mark(InstrData.InstrName.str().c_str());

    for (const SchedVariantData &Variant : InstrData.Variants) {
      OpReqTable.mark(Variant.ItineraryName.str().c_str());

      for (const auto &[OpIdx, RegClassName] : Variant.OperandRCs) {
        OpReqTable << "  {" << OpIdx << ", &" << CurrentNamespace
                   << "::" << RegClassName.str() << "RegClass}";
        OpReqTable.next();
      }

      VariantsTable << "  {" << CurrentNamespace
                    << "::Sched::" << Variant.ItineraryName.str() << ", "
                    << OpReqTable.arrayRef() << "}";
      VariantsTable.next();
    }

    InstrTable << "  {" << CurrentNamespace << "::" << InstrData.InstrName.str()
               << ", " << VariantsTable.arrayRef() << "}";
    InstrTable.next();
  }

  // Finalize and emit the tables.
  OS << OpReqTable.finish();
  OS << VariantsTable.finish();
  OS << InstrTable.finish();
}

void AIEVariableInstrItineraryEmitter::emitInterface(raw_ostream &OS) {
  OS << "// Interface object providing access to the variant tables.\n";
  OS << "static const VarItinInterface " << CurrentNamespace
     << "VarItinInterfaceImpl = {\n";
  OS << "  InstrVariantInfos\n";
  OS << "};\n\n";

  OS << "VarItinInterface " << CurrentNamespace
     << "InstrInfo::getVarItinInterface() const {\n";
  OS << "  return " << CurrentNamespace << "VarItinInterfaceImpl;\n";
  OS << "}\n";
}

void AIEVariableInstrItineraryEmitter::run(raw_ostream &OS) {
  TGTimer &Timer = Records.getTimer();
  Timer.startTimer("Process definitions");

  ArrayRef<const CodeGenInstruction *> NumberedInstructions =
      Target.getInstructions();

  for (const CodeGenInstruction *CGI : NumberedInstructions) {
    const Record *R = CGI->TheDef;
    if ((R->getValueAsString("Namespace") == "TargetOpcode") ||
        (!R->getValue("Inst") && !R->getValue("isMultiSlotPseudo")))
      continue;

    std::vector<const Record *> AltItinary =
        R->getValueAsListOfDefs("ItineraryRegPairs");
    if (AltItinary.empty())
      continue;

    InstrVariantData InstrData;
    InstrData.InstrName = R->getName();

    for (const Record *AltItinerary : AltItinary) {
      SchedVariantData Variant;
      Variant.ItineraryName =
          AltItinerary->getValueAsDef("Itinerary")->getName();

      std::vector<const Record *> OpRegType =
          AltItinerary->getValueAsListOfDefs("RegTypeList");

      auto CheckUnique = [&Variant](unsigned OpIdx) {
        return !std::any_of(
            Variant.OperandRCs.begin(), Variant.OperandRCs.end(),
            [OpIdx](const auto &Operand) { return Operand.first == OpIdx; });
      };

      for (const Record *RegType : OpRegType) {
        const unsigned OpIdx = RegType->getValueAsInt("OperandIdx");
        assert(CheckUnique(OpIdx) && "OperandIdx must be unique");
        const StringRef RegClassName =
            RegType->getValueAsDef("RegClass")->getName();
        Variant.OperandRCs.emplace_back(OpIdx, RegClassName);
      }

      assert(!Variant.OperandRCs.empty() && "OperandRCs cannot be empty");
      InstrData.Variants.push_back(std::move(Variant));
    }

    // Assign original indices to preserve ordering after consolidation.
    for (unsigned I = 0; I < InstrData.Variants.size(); ++I)
      InstrData.Variants[I].OrigIdx = I;

    InstrVariants.push_back(std::move(InstrData));
  }

  // Consolidate variants by finding register class unions for variants with
  // equivalent itineraries and compatible register class attributes.
  consolidateVariants();

  // Sort InstrVariants by opcode for binary search.
  std::sort(InstrVariants.begin(), InstrVariants.end(),
            [&](const InstrVariantData &A, const InstrVariantData &B) {
              const CodeGenInstruction &IA =
                  Target.getInstruction(Records.getDef(A.InstrName));
              const CodeGenInstruction &IB =
                  Target.getInstruction(Records.getDef(B.InstrName));
              return IA.EnumVal < IB.EnumVal;
            });

  Timer.startTimer(
      "Emit Instruction with Alternate Itinerary based on RegClass used");
  emitSourceFileHeader("Instruction itinerary based on RegClass", OS);

  emitTables(OS);
  emitInterface(OS);
}

static TableGen::Emitter::OptClass<AIEVariableInstrItineraryEmitter>
    X("gen-aie-alternate-itinerary-emitter",
      "Generate scheduling info for instructions with alternate itineraries "
      "based on register types");
