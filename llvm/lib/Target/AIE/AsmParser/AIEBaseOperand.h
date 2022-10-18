//===- AIEBaseOperand.h - Parsed AIEX machine operand -----------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEBASEOPERAND_H
#define LLVM_LIB_TARGET_AIE_AIEBASEOPERAND_H

#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Timer.h"
namespace llvm {

struct AIEBaseOperand : public MCParsedAsmOperand {
  enum KindTy {
    Token,
    Register,
    Immediate,
  } Kind;

  SMLoc StartLoc, EndLoc;

  MCContext &Ctx;

  struct TokTy {
    const char *Data;
    unsigned Length;
  };

  struct RegTy {
    unsigned RegNum;
  };

  struct ImmTy {
    const MCExpr *Val;
  };

  union {
    struct TokTy Tok;
    struct RegTy Reg;
    struct ImmTy Imm;
  };

  AIEBaseOperand(KindTy K, MCContext &Ctx) : Kind(K), Ctx(Ctx) {}

public:
  AIEBaseOperand(const AIEBaseOperand &o) = default;

  bool isToken() const override { return Kind == Token; };
  bool isImm() const override { return Kind == Immediate; };
  bool isReg() const override { return Kind == Register; };
  bool isMem() const override { llvm_unreachable("No isMem"); };

  /// getStartLoc - Get the location of the first token of this operand.
  SMLoc getStartLoc() const override { return StartLoc; }
  /// getEndLoc - Get the location of the last token of this operand.
  SMLoc getEndLoc() const override { return EndLoc; }

  unsigned getReg() const override {
    assert(Kind == Register && "Invalid access!");

    return Reg.RegNum;
  }

  StringRef getToken() const {
    assert(Kind == Token && "Invalid access!");
    return StringRef(Tok.Data, Tok.Length);
  }

  const MCExpr *getImm() const {
    assert(Kind == Immediate && "Invalid access!");
    return Imm.Val;
  }

  /// addRegOperands and addImmOperands are used to convert AIEBaseOperands when
  /// MatchAndEmitInstruction will manage to find a match and try to convert the
  /// list of operands into a MCInst.
  void addRegOperands(MCInst &Inst, unsigned N) const;

  void addImmOperands(MCInst &Inst, unsigned N) const;

  void print(raw_ostream &OS) const override;

  static std::unique_ptr<AIEBaseOperand> CreateToken(MCContext &Context,
                                                     StringRef Str, SMLoc S);

  static std::unique_ptr<AIEBaseOperand>
  CreateReg(MCContext &Context, unsigned RegNum, SMLoc S, SMLoc E);

  static std::unique_ptr<AIEBaseOperand>
  CreateImm(MCContext &Context, const MCExpr *Val, SMLoc S, SMLoc E);
};

} // namespace llvm

#endif // #define LLVM_LIB_TARGET_AIE_AIEBASEOPERAND_H
