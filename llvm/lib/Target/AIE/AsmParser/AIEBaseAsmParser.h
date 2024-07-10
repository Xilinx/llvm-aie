//===-- AIEBaseAsmParser.h - Assembly Parser for AIE --------------*- C++ -*--//
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

namespace {

template <typename Parser, typename BundleType, typename OperandType>
class AIEBaseAsmParser : public llvm::MCTargetAsmParser {
protected:
  /// Returns true if Imm is an int contained in N bits with a step of F
  template <unsigned N, unsigned F>
  constexpr inline bool isValidImm(int64_t Imm) {
    return isInt<N + F>(Imm);
  }

  /// parseIdentifier:
  ///   ::= Identifer
  ///   ::= Identifer ":" Identifier
  /// F = Identifer
  /// Parse a register, emits an error if it fails.
  virtual bool parseIdentifier(OperandVector &Operands) = 0;
  virtual bool validateInstruction(llvm::MCInst &Inst,
                                   OperandVector &Operands) = 0;
  virtual unsigned matchRegister(std::string name) = 0;

  void emitBundle(llvm::MCStreamer &Out);

  /// parseImmediate:
  ///   ::= "#" Integer
  ///   ::= "#" Identifier
  /// F = "#"
  /// Parse an immediate, emits an error if it fails.
  bool parseImmediate(OperandVector &Operands);

  /// parseIndirectOrIndexedMode
  ///   ::= "[" <ptr> "]"
  ///   ::= "[" <ptr> "," Register "]"
  ///   ::= "[" <ptr> "," "#" Integer "]"
  /// F = "["
  /// Parse an "addressing mode", emits an error if it fails
  bool parseIndirectOrIndexedMode(OperandVector &Operands);

  /// parseOperand:
  ///   ::= IndirectOrIndexedMode
  ///   ::= Register
  ///   ::= Immediate
  /// F = "[", "#", Identifier
  /// Parse an operand, emits an error if no First is matched.
  /// The parsed operand will be appended in \a Operands
  bool parseOperand(OperandVector &Operands);

  bool processMatchedInstruction(SMLoc IDLoc, OperandVector &Operands,
                                 MCStreamer &Out, MCInst *Inst);

public:
  BundleType Bundle;

  llvm::AIEBaseMCFormats &Formats;

  AIEBaseAsmParser(const llvm::MCSubtargetInfo &STI,
                   const llvm::MCInstrInfo &MII,
                   const llvm::MCTargetOptions &Options,
                   llvm::AIEBaseMCFormats &Formats)
      : llvm::MCTargetAsmParser(Options, STI, MII), Bundle(&Formats),
        Formats(Formats) {}

  // Parse a symbol in a call
  ParseStatus parseCallSymbol(OperandVector &Operands);

  // Parse a register
  bool parseRegister(MCRegister &RegNo, SMLoc &StartLoc,
                     SMLoc &EndLoc) override {

    return !tryParseRegister(RegNo, StartLoc, EndLoc).isSuccess();
  }

  ParseStatus tryParseRegister(llvm::MCRegister &RegNo, SMLoc &StartLoc,
                               SMLoc &EndLoc) override;

  /// Parses ".____" (e.g .delay_slot)
  bool ParseDirective(AsmToken DirectiveID) override;

  /// ParseInstruction:
  ///   ::= Identifier
  ///   ::= Identifier Operand [ "," Operand ]
  /// Parse an instruction \a Name, storing all operands in \a Operands
  bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;
};

} // end anonymous namespace

template <typename Parser, typename BundleType, typename OperandType>
ParseStatus AIEBaseAsmParser<Parser, BundleType, OperandType>::tryParseRegister(
    MCRegister &RegNo, SMLoc &StartLoc, SMLoc &EndLoc) {
  SmallVector<AsmToken, 3> Lookahead;
  for (int i = 0; i < 3; i++) {
    Lookahead.push_back(getTok());
    Lex();
  }
  const AsmToken &Tok = Lookahead.front();
  StartLoc = Tok.getLoc();
  EndLoc = Tok.getEndLoc();
  std::string Name(Tok.getIdentifier());
  // Check if it is a combined register
  if (Lookahead[1].is(AsmToken::Colon) &&
      Lookahead[2].is(AsmToken::Identifier)) {
    Name += Lookahead[1].getString();
    Name += Lookahead[2].getString();
  } else {
    // Unlex the two last tokens
    getLexer().UnLex(Lookahead.pop_back_val());
    getLexer().UnLex(Lookahead.pop_back_val());
  }

  RegNo = matchRegister(Name);
  // Unlex the tokens of the vector if no match was found
  if (!RegNo) {
    for (auto Tok = Lookahead.rbegin(); Tok != Lookahead.rend(); Tok++)
      getLexer().UnLex(*Tok);
  }
  return RegNo ? ParseStatus::Success : ParseStatus::NoMatch;
}

template <typename Parser, typename BundleType, typename OperandType>
bool AIEBaseAsmParser<Parser, BundleType, OperandType>::ParseDirective(
    AsmToken DirectiveID) {
  // TODO
  return true;
}

template <typename Parser, typename BundleType, typename OperandType>
void AIEBaseAsmParser<Parser, BundleType, OperandType>::emitBundle(
    MCStreamer &Out) {
  LLVM_DEBUG(dbgs() << "Emitting bundle\n");
  MCInst B;
  const auto *Format = Bundle.getFormatOrNull();
  assert(Format);
  for (auto Slot : Format->getSlots()) {
    if (Bundle.at(Slot))
      B.addOperand(MCOperand::createInst(Bundle.at(Slot)));
    else {
      MCInst *NopInst = getContext().createMCInst();
      NopInst->setOpcode(Formats.getSlotInfo(Slot)->getNOPOpcode());
      B.addOperand(MCOperand::createInst(NopInst));
    }
  }
  B.setOpcode(Format->Opcode);
  Out.emitInstruction(B, getSTI());
}

template <typename Parser, typename BundleType, typename OperandType>
bool AIEBaseAsmParser<Parser, BundleType, OperandType>::
    processMatchedInstruction(SMLoc IDLoc, OperandVector &Operands,
                              MCStreamer &Out, MCInst *Inst) {
  Inst->setLoc(IDLoc);
  if (validateInstruction(*Inst, Operands))
    return true;

  LLVM_DEBUG(dbgs() << "Inst = " << *Inst << "\n");
  if (Bundle.canAdd(Inst))
    Bundle.add(Inst);
  else {
    // Lex to end of line
    while (!(getLexer().is(llvm::AsmToken::EndOfStatement) &&
             (getLexer().getTok().getString().empty() ||
              getLexer().getTok().getString()[0] == '\n')))
      Lex();
    Bundle.clear();
    return Error(IDLoc, "incorrect bundle");
  }

  const bool EndOfBundle =
      getTok().getString().empty() || getTok().getString()[0] != ';';
  if (EndOfBundle) {
    emitBundle(Out);
    Bundle.clear();
  }
  return false;
}

template <typename Parser, typename BundleType, typename OperandType>
ParseStatus AIEBaseAsmParser<Parser, BundleType, OperandType>::parseCallSymbol(
    OperandVector &Operands) {
  AsmToken const &Tok = getTok();
  SMLoc S = Tok.getLoc();
  const MCExpr *Res;

  if (getLexer().getKind() != AsmToken::Identifier)
    return ParseStatus::NoMatch;

  StringRef Identifier;
  if (getParser().parseIdentifier(Identifier))
    return ParseStatus::Failure;

  SMLoc E = SMLoc::getFromPointer(S.getPointer() + Identifier.size());

  MCSymbol *Sym = getContext().getOrCreateSymbol(Identifier);
  Res = MCSymbolRefExpr::create(Sym, MCSymbolRefExpr::VK_None, getContext());
  Res = AIEMCExpr::create(Res, AIEMCExpr::VK_AIE_CALL, getContext());
  Operands.push_back(OperandType::CreateImm(getContext(), Res, S, E));
  return ParseStatus::Success;
}

/// parseImmediate:
///   ::= "#" Integer
///   ::= "#" Identifier
template <typename Parser, typename BundleType, typename OperandType>
bool AIEBaseAsmParser<Parser, BundleType, OperandType>::parseImmediate(
    OperandVector &Operands) {
  // Consume "#"
  assert(getTok().getKind() == AsmToken::Hash && "Operand is not '#'");
  Lex();

  AsmToken const &Tok = getTok();
  SMLoc S = Tok.getLoc();
  SMLoc E; // Will get the value later
  const MCExpr *Res;

  switch (Tok.getKind()) {
  case AsmToken::Identifier: {
    StringRef Identifier;
    if (getParser().parseIdentifier(Identifier))
      return true /*MatchOperand_ParseFail*/;

    MCSymbol *Sym = getContext().getOrCreateSymbol(Identifier);
    Res = MCSymbolRefExpr::create(Sym, MCSymbolRefExpr::VK_None, getContext());
    break;
  }
  case AsmToken::Minus:
  case AsmToken::Integer: {
    if (getParser().parseExpression(Res, E))
      return true /*MatchOperand_ParseFail*/;
    break;
  }
  // parse #(global + offset)
  case AsmToken::LParen: {
    // Consume '('
    Lex();
    const MCExpr *SubExpr;
    /// parenexpr ::= expr)
    if (getParser().parseParenExpression(SubExpr, E)) {
      return true /*MatchOperand_ParseFail*/;
    }
    AIEMCExpr::VariantKind VK = AIEMCExpr::VK_AIE_GLOBAL;
    Res = AIEMCExpr::create(SubExpr, VK, getContext());
    break;
  }
  default:
    Lex(); // We could not parse the operand, consume it.
    return Error(S, "immediates must be integers or identifiers");
  }
  Operands.push_back(OperandType::CreateImm(getContext(), Res, S, E));
  return MatchOperand_Success;
}

/// parseIndirectOrIndexedMode
///   ::= "[" <ptr> "]"
///   ::= "[" <ptr> "," Register "]"
///   ::= "[" <ptr> "," "#" Integer "]"
template <typename Parser, typename BundleType, typename OperandType>
bool AIEBaseAsmParser<Parser, BundleType, OperandType>::
    parseIndirectOrIndexedMode(OperandVector &Operands) {
  AsmToken const &LBracToken = getLexer().getTok();
  // First token is "["
  assert(LBracToken.getString() == "[" && "Operand is not '['");
  Operands.push_back(OperandType::CreateToken(
      getContext(), LBracToken.getString(), LBracToken.getLoc()));
  Lex();
  if (parseIdentifier(Operands))
    return true;
  if (getLexer().is(AsmToken::Comma)) {
    // Consume ","
    Lex();
    AsmToken const &IdxToken = getTok();
    switch (IdxToken.getKind()) {
    case AsmToken::Hash:
      if (parseImmediate(Operands))
        return true /*MatchOperand_ParseFail*/;
      break;
    case AsmToken::Identifier:
      if (parseIdentifier(Operands))
        return true /*MatchOperand_ParseFail*/;
      break;
    case AsmToken::RBrac:
      return Error(IdxToken.getLoc(), "missing index operand");
    default:
      return Error(IdxToken.getLoc(), "unexpected operand");
    }
  }
  if (getLexer().is(AsmToken::RBrac)) {
    AsmToken const &RBracToken = getLexer().getTok();
    Operands.push_back(OperandType::CreateToken(
        getContext(), RBracToken.getString(), RBracToken.getLoc()));
    Lex();
  } else {
    Error(getLexer().getLoc(), "unexpected operand, expected ']'");
  }
  return MatchOperand_Success;
}

/// parseOperand:
///   ::= IndirectOrIndexedMode
///   ::= Register
///   ::= Immediate
template <typename Parser, typename BundleType, typename OperandType>
bool AIEBaseAsmParser<Parser, BundleType, OperandType>::parseOperand(
    OperandVector &Operands) {
  AsmToken const &Token = getLexer().getTok();
  switch (Token.getKind()) {
  default:
    break;
  case AsmToken::LBrac:
    return parseIndirectOrIndexedMode(Operands);
  case AsmToken::Identifier:
    return parseIdentifier(Operands);
  case AsmToken::Hash:
    return parseImmediate(Operands);
  }
  return Error(Token.getLoc(), "unexpected operand");
}

/// ParseInstruction:
///   ::= Token
///   ::= Token Operand [ "," Operand ]
template <typename Parser, typename BundleType, typename OperandType>
bool AIEBaseAsmParser<Parser, BundleType, OperandType>::ParseInstruction(
    ParseInstructionInfo &Info, StringRef Name, SMLoc NameLoc,
    OperandVector &Operands) {
  // First operand is token for instruction
  Operands.push_back(OperandType::CreateToken(getContext(), Name, NameLoc));

  if (getLexer().is(AsmToken::EndOfStatement)) {
    return false;
  }

  // Parse the first operand
  // Exiting before the EndOfStatement doesn't matter.
  // LLVM will consume all tokens until the next EndOfStatement before calling
  // ParseInstruction again
  if (parseOperand(Operands))
    return true;

  // Collect the rest of operands until the end of the statement, consuming all
  // commas
  while (!getLexer().is(AsmToken::EndOfStatement) &&
         !getLexer().is(AsmToken::Eof)) {
    AsmToken const &Token = getLexer().getTok();
    if (!getLexer().is(AsmToken::Comma))
      return Error(Token.getLoc(), "unexpected operand");
    Lex();

    if (parseOperand(Operands))
      return true;
  }

  // Do NOT consume EndOfStatement: is used to check for end of bundle when
  // emitting
  return false;
}
