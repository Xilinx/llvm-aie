//===- AIECopyMaterializationEmitter.cpp - Copy materialization -*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "Common/CodeGenRegisters.h"
#include "Common/CodeGenTarget.h"
#include "ConstTable.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/TableGen/Error.h"
#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/TGTimer.h"
#include "llvm/TableGen/TableGenBackend.h"

using namespace llvm;

namespace {

struct FlatCopy {
  const Record *Opcode;
  const CodeGenSubRegIndex *DstSubReg = nullptr;
  const CodeGenSubRegIndex *SrcSubReg = nullptr;

  bool operator==(const FlatCopy &Other) const {
    return Opcode == Other.Opcode && SrcSubReg == Other.SrcSubReg &&
           DstSubReg == Other.DstSubReg;
  }
};

using FlatPlan = SmallVector<FlatCopy, 8>;

class AIECopyMaterializationEmitter {
public:
  AIECopyMaterializationEmitter(const RecordKeeper &Records)
      : Records(Records), Target(Records), RegBank(Target.getRegBank()) {}

  void run(raw_ostream &OS);

private:
  bool isRecursiveRecipe(const Record *Recipe) const;
  bool matches(const Record *Recipe, const CodeGenRegister *Dst,
               const CodeGenRegister *Src) const;
  const Record *findRecipe(const CodeGenRegister *Dst,
                           const CodeGenRegister *Src) const;
  FlatPlan flattenRecipe(const Record *Recipe);
  void validatePlanForPair(const Record *Recipe, const FlatPlan &Plan,
                           const CodeGenRegister *Dst,
                           const CodeGenRegister *Src);
  FlatPlan expandRecipe(const Record *Recipe, const CodeGenRegister *Dst,
                        const CodeGenRegister *Src,
                        SmallSet<std::pair<unsigned, unsigned>, 8> &Stack);
  void expandAction(const Record *Action, const CodeGenRegister *Dst,
                    const CodeGenRegister *Src,
                    SmallSet<std::pair<unsigned, unsigned>, 8> &Stack,
                    FlatPlan &Plan);
  FlatPlan resolvePair(const CodeGenRegister *Dst, const CodeGenRegister *Src,
                       SmallSet<std::pair<unsigned, unsigned>, 8> &Stack);
  void appendPrefixedPlan(FlatPlan &Plan, FlatPlan ChildPlan,
                          const CodeGenSubRegIndex *DstSubReg,
                          const CodeGenSubRegIndex *SrcSubReg);
  std::string regClassRef(const Record *RC) const;
  std::string subRegRef(const CodeGenSubRegIndex *SubReg) const;
  std::string instrRef(const Record *Instr) const;

  void validateRecipes() const;
  void emitTables(raw_ostream &OS);
  void emitImpl(raw_ostream &OS);

  const RecordKeeper &Records;
  CodeGenTarget Target;
  CodeGenRegBank &RegBank;
  std::string TargetNamespace;
  std::vector<const Record *> Recipes;
};

} // namespace

//===----------------------------------------------------------------------===//
// Recipe selection
//===----------------------------------------------------------------------===//

bool AIECopyMaterializationEmitter::isRecursiveRecipe(
    const Record *Recipe) const {
  for (const Record *Action : Recipe->getValueAsListOfDefs("Actions"))
    if (Action->isSubClassOf("CopyThroughAllSubRegs") ||
        Action->isSubClassOf("CopyThroughSubRegs"))
      return true;
  return false;
}

bool AIECopyMaterializationEmitter::matches(const Record *Recipe,
                                            const CodeGenRegister *Dst,
                                            const CodeGenRegister *Src) const {
  return RegBank.getRegClass(Recipe->getValueAsDef("DstRC"))->contains(Dst) &&
         RegBank.getRegClass(Recipe->getValueAsDef("SrcRC"))->contains(Src);
}

const Record *
AIECopyMaterializationEmitter::findRecipe(const CodeGenRegister *Dst,
                                          const CodeGenRegister *Src) const {
  for (const Record *Recipe : Recipes)
    if (matches(Recipe, Dst, Src))
      return Recipe;
  return nullptr;
}

//===----------------------------------------------------------------------===//
// Flat-plan expansion
//===----------------------------------------------------------------------===//

void AIECopyMaterializationEmitter::appendPrefixedPlan(
    FlatPlan &Plan, FlatPlan ChildPlan, const CodeGenSubRegIndex *DstSubReg,
    const CodeGenSubRegIndex *SrcSubReg) {
  auto Prefix = [&](const CodeGenSubRegIndex *&SubReg,
                    const CodeGenSubRegIndex *Prefix) {
    if (!SubReg) {
      SubReg = Prefix;
      return;
    }
    SubReg = RegBank.getCompositeSubRegIndex(
        const_cast<CodeGenSubRegIndex *>(Prefix),
        const_cast<CodeGenSubRegIndex *>(SubReg));
  };

  for (FlatCopy &Copy : ChildPlan) {
    Prefix(Copy.DstSubReg, DstSubReg);
    Prefix(Copy.SrcSubReg, SrcSubReg);
    Plan.push_back(std::move(Copy));
  }
}

FlatPlan AIECopyMaterializationEmitter::resolvePair(
    const CodeGenRegister *Dst, const CodeGenRegister *Src,
    SmallSet<std::pair<unsigned, unsigned>, 8> &Stack) {
  const std::pair<unsigned, unsigned> Pair{Dst->EnumValue, Src->EnumValue};
  if (!Stack.insert(Pair).second)
    PrintFatalError("recursive copy recipe cycle for " + Src->getName() +
                    " -> " + Dst->getName());
  const Record *Recipe = findRecipe(Dst, Src);
  if (!Recipe)
    PrintFatalError("no copy recipe for recursive leaf " + Src->getName() +
                    " -> " + Dst->getName());
  FlatPlan Plan = expandRecipe(Recipe, Dst, Src, Stack);
  Stack.erase(Pair);
  return Plan;
}

void AIECopyMaterializationEmitter::expandAction(
    const Record *Action, const CodeGenRegister *Dst,
    const CodeGenRegister *Src,
    SmallSet<std::pair<unsigned, unsigned>, 8> &Stack, FlatPlan &Plan) {
  if (Action->isSubClassOf("CopyTuple")) {
    FlatCopy Copy{Action->getValueAsDef("MoveOpcode")};
    if (!Action->isValueUnset("DstSubReg"))
      Copy.DstSubReg = RegBank.getSubRegIdx(Action->getValueAsDef("DstSubReg"));
    if (!Action->isValueUnset("SrcSubReg"))
      Copy.SrcSubReg = RegBank.getSubRegIdx(Action->getValueAsDef("SrcSubReg"));
    Plan.push_back(std::move(Copy));
    return;
  }

  if (Action->isSubClassOf("CopyThroughSubRegs")) {
    std::vector<const Record *> DstIndices =
        Action->getValueAsListOfDefs("DstSubRegs");
    std::vector<const Record *> SrcIndices =
        Action->getValueAsListOfDefs("SrcSubRegs");
    if (DstIndices.size() != SrcIndices.size())
      PrintFatalError(Action->getLoc(),
                      "recursive copy must pair every source subregister");
    if (DstIndices.empty())
      PrintFatalError(Action->getLoc(),
                      "recursive copy must name a source subregister");
    for (auto [DstDef, SrcDef] : llvm::zip(DstIndices, SrcIndices)) {
      CodeGenSubRegIndex *DstIdx = RegBank.getSubRegIdx(DstDef);
      CodeGenSubRegIndex *SrcIdx = RegBank.getSubRegIdx(SrcDef);
      auto DstIt = Dst->getSubRegs().find(DstIdx);
      auto SrcIt = Src->getSubRegs().find(SrcIdx);
      if (DstIt == Dst->getSubRegs().end() || SrcIt == Src->getSubRegs().end())
        PrintFatalError(Action->getLoc(),
                        "recursive copy names an invalid subregister");
      appendPrefixedPlan(Plan, resolvePair(DstIt->second, SrcIt->second, Stack),
                         DstIdx, SrcIdx);
    }
    return;
  }

  if (!Action->isSubClassOf("CopyThroughAllSubRegs"))
    PrintFatalError(Action->getLoc(), "unknown copy recipe action");

  SetVector<const CodeGenRegister *> SubRegs;
  Src->addSubRegsPreOrder(SubRegs, RegBank);
  if (SubRegs.empty())
    PrintFatalError(Action->getLoc(),
                    "atomic copy traversal requires a composite register");
  for (const CodeGenRegister *SrcSubReg : SubRegs) {
    if (!SrcSubReg->getSubRegs().empty())
      continue;
    CodeGenSubRegIndex *SubRegIdx = Src->getSubRegIndex(SrcSubReg);
    auto DstIt = Dst->getSubRegs().find(SubRegIdx);
    if (DstIt == Dst->getSubRegs().end())
      PrintFatalError(Action->getLoc(),
                      "destination lacks an atomic source subregister");
    appendPrefixedPlan(Plan, resolvePair(DstIt->second, SrcSubReg, Stack),
                       SubRegIdx, SubRegIdx);
  }
}

FlatPlan AIECopyMaterializationEmitter::expandRecipe(
    const Record *Recipe, const CodeGenRegister *Dst,
    const CodeGenRegister *Src,
    SmallSet<std::pair<unsigned, unsigned>, 8> &Stack) {
  FlatPlan Plan;
  for (const Record *Action : Recipe->getValueAsListOfDefs("Actions"))
    expandAction(Action, Dst, Src, Stack, Plan);
  return Plan;
}

void AIECopyMaterializationEmitter::validatePlanForPair(
    const Record *Recipe, const FlatPlan &Plan, const CodeGenRegister *Dst,
    const CodeGenRegister *Src) {
  if (Plan.empty())
    PrintFatalError(Recipe->getLoc(),
                    "copy recipe must materialize at least one copy");

  auto ValidateSubReg = [&](const CodeGenSubRegIndex *SubReg,
                            const CodeGenRegister *Reg) {
    if (SubReg &&
        !Reg->getSubRegs().count(const_cast<CodeGenSubRegIndex *>(SubReg)))
      PrintFatalError(Recipe->getLoc(),
                      "copy plan names an invalid physical subregister");
  };

  for (const FlatCopy &Copy : Plan) {
    ValidateSubReg(Copy.DstSubReg, Dst);
    ValidateSubReg(Copy.SrcSubReg, Src);
  }
}

FlatPlan AIECopyMaterializationEmitter::flattenRecipe(const Record *Recipe) {
  const CodeGenRegister::Vec *DstMembers =
      &RegBank.getRegClass(Recipe->getValueAsDef("DstRC"))->getMembers();
  const CodeGenRegister::Vec *SrcMembers =
      &RegBank.getRegClass(Recipe->getValueAsDef("SrcRC"))->getMembers();
  if (DstMembers->empty() || SrcMembers->empty())
    PrintFatalError(Recipe->getLoc(),
                    "copy recipe matches an empty register class");

  SmallSet<std::pair<unsigned, unsigned>, 8> Stack;
  FlatPlan Plan =
      expandRecipe(Recipe, DstMembers->front(), SrcMembers->front(), Stack);
  for (const CodeGenRegister *Dst : *DstMembers)
    for (const CodeGenRegister *Src : *SrcMembers) {
      FlatPlan Candidate = isRecursiveRecipe(Recipe)
                               ? expandRecipe(Recipe, Dst, Src, Stack)
                               : Plan;
      validatePlanForPair(Recipe, Candidate, Dst, Src);
      if (isRecursiveRecipe(Recipe) && Candidate != Plan)
        PrintFatalError(Recipe->getLoc(),
                        "copy recipe has a nonuniform recursive expansion");
    }
  return Plan;
}

//===----------------------------------------------------------------------===//
// Validation
//===----------------------------------------------------------------------===//

void AIECopyMaterializationEmitter::validateRecipes() const {
  auto IsFullRegisterCopy = [](const Record *Recipe) {
    std::vector<const Record *> Actions =
        Recipe->getValueAsListOfDefs("Actions");
    return Actions.size() == 1 && Actions.front()->isSubClassOf("CopyTuple") &&
           Actions.front()->isValueUnset("SrcSubReg") &&
           Actions.front()->isValueUnset("DstSubReg");
  };

  DenseSet<std::pair<const Record *, const Record *>> ClassPairs;
  for (const Record *Recipe : Recipes) {
    if (Recipe->getValueAsListOfDefs("Actions").empty())
      PrintFatalError(Recipe->getLoc(),
                      "copy recipe must contain at least one action");
    const Record *Dst = Recipe->getValueAsDef("DstRC");
    const Record *Src = Recipe->getValueAsDef("SrcRC");
    if (!ClassPairs.insert({Dst, Src}).second)
      PrintFatalError(Recipe->getLoc(), "duplicate copy recipe match");
  }

  for (const Record *Recipe : Recipes) {
    if (!IsFullRegisterCopy(Recipe))
      continue;
    const CodeGenRegister::Vec &DstMembers =
        RegBank.getRegClass(Recipe->getValueAsDef("DstRC"))->getMembers();
    const CodeGenRegister::Vec &SrcMembers =
        RegBank.getRegClass(Recipe->getValueAsDef("SrcRC"))->getMembers();
    for (const CodeGenRegister *Dst : DstMembers)
      for (const CodeGenRegister *Src : SrcMembers) {
        const Record *SelectedRecipe = findRecipe(Dst, Src);
        if (!SelectedRecipe || !IsFullRegisterCopy(SelectedRecipe))
          PrintFatalError(
              Recipe->getLoc(),
              "direct-copy register pair selects a non-direct copy rule");
      }
  }

  auto ClassesOverlap = [&](const Record *LHS, const Record *RHS) {
    const CodeGenRegisterClass *LRC = RegBank.getRegClass(LHS);
    const CodeGenRegisterClass *RRC = RegBank.getRegClass(RHS);
    for (const CodeGenRegister *Reg : LRC->getMembers())
      if (RRC->contains(Reg))
        return true;
    return false;
  };
  for (auto [Index, LHS] : llvm::enumerate(Recipes)) {
    for (const Record *RHS : ArrayRef(Recipes).drop_front(Index + 1)) {
      if (LHS->getValueAsInt("Priority") != RHS->getValueAsInt("Priority"))
        continue;
      if (ClassesOverlap(LHS->getValueAsDef("SrcRC"),
                         RHS->getValueAsDef("SrcRC")) &&
          ClassesOverlap(LHS->getValueAsDef("DstRC"),
                         RHS->getValueAsDef("DstRC")))
        PrintFatalError(RHS->getLoc(), "equal-priority copy recipes " +
                                           LHS->getName() + " and " +
                                           RHS->getName() +
                                           " have overlapping matches");
    }
  }
}

//===----------------------------------------------------------------------===//
// Generated-source emission
//===----------------------------------------------------------------------===//

std::string AIECopyMaterializationEmitter::regClassRef(const Record *RC) const {
  return "&" + TargetNamespace + "::" + RC->getName().str() + "RegClass";
}

std::string AIECopyMaterializationEmitter::subRegRef(
    const CodeGenSubRegIndex *SubReg) const {
  return SubReg ? SubReg->getQualifiedName() : "0";
}

std::string AIECopyMaterializationEmitter::instrRef(const Record *Instr) const {
  return TargetNamespace + "::" + Instr->getName().str();
}

void AIECopyMaterializationEmitter::emitTables(raw_ostream &OS) {
  ConstTable TupleTable("CopyTuple", "CopyTuples");
  ConstTable RecipeTable("CopyRecipe", "CopyRecipes");

  for (const Record *Recipe : Recipes) {
    FlatPlan Plan = flattenRecipe(Recipe);
    unsigned FirstCopy = TupleTable.mark(Recipe->getName().data());
    for (const FlatCopy &Copy : Plan) {
      TupleTable << "  {" << subRegRef(Copy.DstSubReg) << ", "
                 << subRegRef(Copy.SrcSubReg) << ", " << instrRef(Copy.Opcode)
                 << "}";
      TupleTable.next();
    }

    RecipeTable << "  {" << regClassRef(Recipe->getValueAsDef("DstRC")) << ", "
                << regClassRef(Recipe->getValueAsDef("SrcRC")) << ", "
                << FirstCopy << ", " << Plan.size() << "}";
    RecipeTable.next();
  }

  OS << TupleTable.finish();
  OS << RecipeTable.finish();
}

void AIECopyMaterializationEmitter::emitImpl(raw_ostream &OS) {
  OS << "#ifdef GET_COPY_MATERIALIZATION_IMPL\n"
     << "#undef GET_COPY_MATERIALIZATION_IMPL\n\n"
     << "namespace llvm {\n";
  emitTables(OS);
  OS << "const CopyTableView &" << TargetNamespace
     << "InstrInfo::getCopyTable() const {\n"
     << "  static constexpr CopyTableView Table{CopyRecipes, CopyTuples};\n"
     << "  return Table;\n"
     << "}\n"
     << "} // end namespace llvm\n\n"
     << "#endif // GET_COPY_MATERIALIZATION_IMPL\n";
}

void AIECopyMaterializationEmitter::run(raw_ostream &OS) {
  TGTimer &Timer = Records.getTimer();
  Timer.startTimer("Emit copy materialization tables");
  TargetNamespace = Target.getName().str();
  Recipes = Records.getAllDerivedDefinitions("CopyRecipe");
  llvm::sort(Recipes, [&](const Record *LHS, const Record *RHS) {
    int64_t LP = LHS->getValueAsInt("Priority");
    int64_t RP = RHS->getValueAsInt("Priority");
    return LP != RP ? LP > RP : LHS->getName() < RHS->getName();
  });
  validateRecipes();

  emitSourceFileHeader("Copy materialization tables Source Fragment", OS);
  emitImpl(OS);
}

static TableGen::Emitter::OptClass<AIECopyMaterializationEmitter>
    X("gen-aie-copy-materialization",
      "AIE copy materialization tables Emitter");
