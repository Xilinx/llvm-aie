//===-- AIEMCExpr.cpp - AIE specific MC expression classes ------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the implementation of the assembly expression modifiers
// accepted by the AIE architecture
//
//===----------------------------------------------------------------------===//

#include "AIEMCExpr.h"
#include "AIE.h"
#include "llvm/MC/MCAsmLayout.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "aiemcexpr"

const AIEMCExpr *AIEMCExpr::create(const MCExpr *Expr, VariantKind Kind,
                                       MCContext &Ctx) {
  return new (Ctx) AIEMCExpr(Expr, Kind);
}

void AIEMCExpr::printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const {
  OS << '#';
  const MCExpr *Expr = getSubExpr();
  bool isSymRefExpr = Expr->getKind() == ExprKind::SymbolRef;
  if (!isSymRefExpr)
    OS << '(';
  Expr->print(OS, MAI);
  if (!isSymRefExpr)
    OS << ')';
}


bool AIEMCExpr::evaluateAsRelocatableImpl(MCValue &Res,
                                            const MCAsmLayout *Layout,
                                            const MCFixup *Fixup) const {
  // if (Kind == VK_AIE_PCREL_LO && evaluatePCRelLo(Res, Layout, Fixup))
  //   return true;

  if (!getSubExpr()->evaluateAsRelocatable(Res, Layout, Fixup))
    return false;

  // // Some custom fixup types are not valid with symbol difference expressions
  // if (Res.getSymA() && Res.getSymB()) {
  //   switch (getKind()) {
  //   default:
  //     return true;
  //   case VK_AIE_LO:
  //   case VK_AIE_HI:
  //   case VK_AIE_PCREL_LO:
  //   case VK_AIE_PCREL_HI:
  //   case VK_AIE_GOT_HI:
  //     return false;
  //   }
  // }

  return true;
}

void AIEMCExpr::visitUsedExpr(MCStreamer &Streamer) const {
  Streamer.visitUsedExpr(*getSubExpr());
}

AIEMCExpr::VariantKind AIEMCExpr::getVariantKindForName(StringRef name) {
  return StringSwitch<AIEMCExpr::VariantKind>(name)
      .Case("call", VK_AIE_CALL)
      .Case("global", VK_AIE_GLOBAL)
      .Default(VK_AIE_Invalid);
}

StringRef AIEMCExpr::getVariantKindName(VariantKind Kind) {
  switch (Kind) {
  default:
    llvm_unreachable("Invalid ELF symbol kind");
  case VK_AIE_CALL:
    return "call";
  case VK_AIE_GLOBAL:
    return "";
  }
}

bool AIEMCExpr::evaluateAsConstant(int64_t &Res) const {
  MCValue Value;

  if (Kind == VK_AIE_CALL)
    return false;

  if (!getSubExpr()->evaluateAsRelocatable(Value, nullptr, nullptr))
    return false;

  if (!Value.isAbsolute())
    return false;

  Res = evaluateAsInt64(Value.getConstant());
  return true;
}

int64_t AIEMCExpr::evaluateAsInt64(int64_t Value) const {
  switch (Kind) {
  default:
    llvm_unreachable("Invalid kind");
  }
}
