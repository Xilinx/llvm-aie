//===- AIEBaseOperand.cpp - Parsed AIEX machine operand ---------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEBaseOperand.h"

using namespace llvm;

std::unique_ptr<AIEBaseOperand> AIEBaseOperand::CreateImm(MCContext &Context,
                                                          const MCExpr *Val,
                                                          SMLoc S, SMLoc E) {
  auto Op = std::make_unique<AIEBaseOperand>(KindTy::Immediate, Context);
  Op->Imm.Val = Val;
  Op->StartLoc = S;
  Op->EndLoc = E;
  return Op;
}

std::unique_ptr<AIEBaseOperand>
llvm::AIEBaseOperand::CreateReg(MCContext &Context, unsigned RegNum, SMLoc S,
                                SMLoc E) {
  auto Op = std::make_unique<AIEBaseOperand>(KindTy::Register, Context);
  Op->Reg.RegNum = RegNum;
  Op->StartLoc = S;
  Op->EndLoc = E;
  return Op;
}

std::unique_ptr<AIEBaseOperand>
llvm::AIEBaseOperand::CreateToken(MCContext &Context, StringRef Str, SMLoc S) {
  auto Op = std::make_unique<AIEBaseOperand>(KindTy::Token, Context);
  Op->Tok.Data = Str.data();
  Op->Tok.Length = Str.size();
  Op->StartLoc = S;
  Op->EndLoc = S;
  return Op;
}

void llvm::AIEBaseOperand::print(raw_ostream &OS) const {
  switch (Kind) {
  case Immediate:
    OS << *getImm();
    break;
  case Register:
    OS << "<register x" << getReg() << ">";
    break;
  case Token:
    OS << '\'' << getToken() << '\'';
    break;
  }
}

void llvm::AIEBaseOperand::addImmOperands(MCInst &Inst, unsigned N) const {
  assert(N == 1 && "Invalid number of operands!");
  // TODO: Maybe refactor the converstion to int64_t like RISCV?
  switch (Imm.Val->getKind()) {
  case MCExpr::Constant:
    Inst.addOperand(
        MCOperand::createImm(dyn_cast<MCConstantExpr>(Imm.Val)->getValue()));
    break;
  case MCExpr::SymbolRef:
  case MCExpr::Target:
    Inst.addOperand(MCOperand::createExpr(getImm()));
    break;
  default:
    // TODO: Could be more immediate types
    llvm_unreachable("Unhandled immediate type");
  }
}

void llvm::AIEBaseOperand::addRegOperands(MCInst &Inst, unsigned N) const {
  assert(N == 1 && "Invalid number of operands!");
  Inst.addOperand(MCOperand::createReg(getReg()));
}
