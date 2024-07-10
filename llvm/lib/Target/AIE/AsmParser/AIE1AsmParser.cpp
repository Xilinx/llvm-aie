//===-- AIEAsmParser.cpp - Assembly Parser for AIE ----------------*- C++ -*--//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCAsmParser.h"

#include "AIE.h"
#include "AIEBaseOperand.h"
#include "AIEBundle.h"
#include "AIEInstrInfo.h"
#include "AIERegisterInfo.h"
#include "MCTargetDesc/AIEMCExpr.h"
#include "MCTargetDesc/AIEMCTargetDesc.h"
#include "TargetInfo/AIETargetInfo.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

#define DEBUG_TYPE "mcasmparser"

using namespace llvm;

#include "AIEBaseAsmParser.h"

namespace {
AIEMCFormats FormatInterface;
}

namespace {

class AIEOperand : public AIEBaseOperand {};

class AIEAsmParser
    : public AIEBaseAsmParser<AIEAsmParser, AIE::MCBundle, AIEOperand> {
private:
// Auto-generated Match Functions
#define GET_ASSEMBLER_HEADER
#include "AIEGenAsmMatcher.inc"

  bool parseIdentifier(OperandVector &Operands) override;
  bool validateInstruction(MCInst &Inst, OperandVector &Operands) override;
  unsigned matchRegister(std::string Name) override;
  bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;

public:
  AIEAsmParser(const MCSubtargetInfo &STI, MCAsmParser &Parser,
               const MCInstrInfo &MII, const MCTargetOptions &Options)
      : AIEBaseAsmParser(STI, MII, Options, FormatInterface) {
    // TODO: There might be some stuff we want to initialize
    setAvailableFeatures(ComputeAvailableFeatures(getSTI().getFeatureBits()));
  }
};

} // end anonymous namespace

#define GET_MATCHER_IMPLEMENTATION
#define GET_REGISTER_MATCHER
#define GET_MNEMONIC_SPELL_CHECKER
#include "AIEGenAsmMatcher.inc"

unsigned AIEAsmParser::matchRegister(std::string Name) {
  unsigned RegNo = MatchRegisterName(Name);
  if (RegNo)
    return RegNo;
  return MatchRegisterAltName(Name);
}

bool AIEAsmParser::validateInstruction(MCInst &Inst, OperandVector &Operands) {
  // FIXME: this deserves some validation.
  return false;
}

bool AIEAsmParser::MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                                           OperandVector &Operands,
                                           MCStreamer &Out, uint64_t &ErrorInfo,
                                           bool MatchingInlineAsm) {
  MCInst *Inst = getContext().createMCInst();
  LLVM_DEBUG(dbgs() << "Emitting...\t"
                    << "instruction ending with" << getTok().getString());

  auto MatchResult =
      MatchInstructionImpl(Operands, *Inst, ErrorInfo, MatchingInlineAsm);
  switch (MatchResult) {
  default:
    break;
  case Match_Success: {
    return processMatchedInstruction(IDLoc, Operands, Out, Inst);
  }
  // TODO: Properly implement the following cases
  case Match_InvalidTiedOperand:
  case Match_InvalidOperand: {
    SMLoc ErrorLoc = IDLoc;
    if (ErrorInfo != ~0ULL) {
      if (ErrorInfo >= Operands.size())
        return Error(ErrorLoc, "too few operands for instructions");

      ErrorLoc = ((AIEOperand &)*Operands[ErrorInfo]).getStartLoc();
      if (ErrorLoc == SMLoc())
        ErrorLoc = IDLoc;
    }
    return Error(ErrorLoc, "invalid operand for instruction");
  }
  case Match_MissingFeature: {
    return Error(IDLoc, "Match_MissingFeature");
  }
  case Match_MnemonicFail: {
    FeatureBitset FBS = ComputeAvailableFeatures(getSTI().getFeatureBits());
    StringRef Mnemonic = ((AIEOperand &)*Operands[0]).getToken();
    std::string Suggestion = AIEMnemonicSpellCheck(Mnemonic, FBS, 0);
    return Error(IDLoc, "unrecognized instruction mnemonic" + Suggestion);
  }
  }
  llvm_unreachable("Unknown match type detected");
}

/// parseIdentifier:
///   ::= Identifier
///   ::= Identifier ":" Identifier
bool AIEAsmParser::parseIdentifier(OperandVector &Operands) {
  MCRegister RegNo;
  SMLoc Begin;
  SMLoc End;
  if (parseRegister(RegNo, Begin, End) == MatchOperand_Success) {
    Operands.push_back(AIEOperand::CreateReg(getContext(), RegNo, Begin, End));
  } else if (matchTokenString(getTok().getString())) {
    // Instructions such as VMOV_mv_mcd have operands that are not immediates or
    // register: MCD, SCD... This will match these operands
    auto Tok = getTok();
    Operands.push_back(
        AIEOperand::CreateToken(getContext(), Tok.getString(), Tok.getLoc()));
    Lex();
  } else
    return Error(Begin, "operand is not a register, nor a known identifier");
  return MatchOperand_Success;
}

void LLVMInitializeAIE1AsmParser() {
  RegisterMCAsmParser<AIEAsmParser> X(getTheAIETarget());
}

void LLVMInitializeAIE2AsmParser();

// Register hook for all the AIE AsmParsers
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeAIEAsmParser() {
  LLVMInitializeAIE1AsmParser();
  LLVMInitializeAIE2AsmParser();
}
