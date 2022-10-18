//===-- AIE2AsmParser.cpp - Assembly Parser for AIE2 --------------*- C++ -*--//
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

#include "AIE2.h"
#include "AIE2InstrInfo.h"
#include "AIE2RegisterInfo.h"
#include "AIEBaseOperand.h"
#include "AIEBundle.h"
#include "MCTargetDesc/AIE2MCTargetDesc.h"
#include "MCTargetDesc/AIEMCExpr.h"
#include "TargetInfo/AIETargetInfo.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

#define DEBUG_TYPE "mcasmparser"

using namespace llvm;

namespace {
AIE2MCFormats FormatInterface;
}

namespace {

class AIE2AsmParser : public MCTargetAsmParser {
private:
  /// Returns true if Imm is an int contained in N bits with a step of F
  template <unsigned N, unsigned F>
  constexpr inline bool isValidImm(int64_t Imm) {
    return isInt<N + F>(Imm);
  }

  bool validateInstruction(MCInst &Inst, OperandVector &Operands);

  bool emitBundle(MCStreamer &Out);

// Auto-generated Match Functions
#define GET_ASSEMBLER_HEADER
#include "AIE2GenAsmMatcher.inc"

  /// parseImmediate:
  ///   ::= "#" Integer
  ///   ::= "#" Identifier
  /// F = "#"
  /// Parse an immediate, emits an error if it fails.
  bool parseImmediate(OperandVector &Operands);

  /// parseIdentifier:
  ///   ::= Identifer
  ///   ::= Identifer ":" Identifier
  /// F = Identifer
  /// Parse a register, emits an error if it fails.
  bool parseIdentifier(OperandVector &Operands);

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

  AIE::MCBundle Bundle;

public:
  AIE2AsmParser(const MCSubtargetInfo &STI, MCAsmParser &Parser,
                const MCInstrInfo &MII, const MCTargetOptions &Options)
      : MCTargetAsmParser(Options, STI, MII), Bundle(&FormatInterface) {
    // TODO: There might be some stuff we want to initialize
    setAvailableFeatures(ComputeAvailableFeatures(getSTI().getFeatureBits()));
  }

  // Parse a register
  bool parseRegister(MCRegister &RegNo, SMLoc &StartLoc,
                     SMLoc &EndLoc) override;

  ParseStatus tryParseRegister(MCRegister &RegNo, SMLoc &StartLoc,
                               SMLoc &EndLoc) override;

  /// Parses ".____" (e.g .delay_slot)
  bool ParseDirective(AsmToken DirectiveID) override;

  bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;

  /// ParseInstruction:
  ///   ::= Identifier
  ///   ::= Identifier Operand [ "," Operand ]
  /// Parse an instruction \a Name, storing all operands in \a Operands
  bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;
};

class AIE2Operand : public AIEBaseOperand {};

} // end anonymous namespace

#define GET_MATCHER_IMPLEMENTATION
#define GET_REGISTER_MATCHER
#define GET_MNEMONIC_SPELL_CHECKER
#include "AIE2GenAsmMatcher.inc"

bool AIE2AsmParser::validateInstruction(MCInst &Inst, OperandVector &Operands) {
  // The index of the operand in \a Inst and \a Operands will always be
  // different, because Operands also contains the instruction name, and
  // sometimes other syntax tokens, such as "[", "]" (it will not contain
  // ",")...
  switch (Inst.getOpcode()) {
  case AIE2::PADDA_lda_ptr_inc_idx_imm: {
    const int64_t Imm = Inst.getOperand(2).getImm();
    if (!isValidImm<10, 2>(Imm))
      return Error(Operands[3]->getStartLoc(),
                   "PADDA cannot handle immediates > +-2^12");
    if (Imm % 4 != 0)
      return Error(Operands[3]->getStartLoc(),
                   "PADDA immediates must be multiples of 4");
    break;
  }
  case AIE2::PADDB_ldb_ptr_inc_nrm_imm: {
    const int64_t Imm = Inst.getOperand(2).getImm();
    if (!isValidImm<9, 2>(Imm))
      return Error(Operands[3]->getStartLoc(),
                   "PADDB cannot handle immediates > +-2^11");
    if (Imm % 4 != 0)
      return Error(Operands[3]->getStartLoc(),
                   "PADDB immediates must be multiples of 4");
    break;
  }
  case AIE2::PADDA_sp_imm: {
    const int64_t Imm = Inst.getOperand(0).getImm();
    if (!isValidImm<13, 5>(Imm))
      return Error(
          Operands[3]->getStartLoc(),
          "PADDA cannot handle immediates > +-2^17 on the stack pointer");
    if (Imm % 32 != 0)
      return Error(Operands[3]->getStartLoc(),
                   "PADDA immediates must be multiples of 32 when incrementing "
                   "the stack pointer");
    break;
  }
  case AIE2::MOVA_lda_cg: {
    const int64_t Imm = Inst.getOperand(1).getImm();
    if (!isValidImm<11, 0>(Imm))
      return Error(Operands[2]->getStartLoc(),
                   "MOVA cannot handle immediates > +-2^11");
    break;
  }
  default:
    break;
  }
  return false;
}

bool AIE2AsmParser::parseRegister(MCRegister &RegNo, SMLoc &StartLoc,
                                  SMLoc &EndLoc) {

  return !tryParseRegister(RegNo, StartLoc, EndLoc).isSuccess();
}

ParseStatus AIE2AsmParser::tryParseRegister(MCRegister &RegNo,
                                            SMLoc &StartLoc,
                                            SMLoc &EndLoc) {
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

  RegNo = MatchRegisterName(Name);
  if (RegNo == AIE2::NoRegister)
    RegNo = MatchRegisterAltName(Name);
  // Unlex the tokens of the vector if no match was found
  if (RegNo == AIE2::NoRegister) {
    for (auto Tok = Lookahead.rbegin(); Tok != Lookahead.rend(); Tok++)
      getLexer().UnLex(*Tok);
  }
  return RegNo == AIE2::NoRegister ? ParseStatus::NoMatch
                                   : ParseStatus::Success;
}

bool AIE2AsmParser::ParseDirective(AsmToken DirectiveID) {
  // TODO
  return true;
}

bool AIE2AsmParser::emitBundle(MCStreamer &Out) {
  LLVM_DEBUG(dbgs() << "Emitting bundle\n");
  MCInst B;
  const auto *Format = Bundle.getFormatOrNull();
  assert(Format);
  for (auto Slot : Format->getSlots()) {
    if (Bundle.at(Slot))
      B.addOperand(MCOperand::createInst(Bundle.at(Slot)));
    else {
      MCInst *NopInst = getContext().createMCInst();
      NopInst->setOpcode(FormatInterface.getSlotInfo(Slot)->getNOPOpcode());
      B.addOperand(MCOperand::createInst(NopInst));
    }
  }
  B.setOpcode(Format->Opcode);
  Out.emitInstruction(B, getSTI());
  Bundle.clear();
  return false;
}

bool AIE2AsmParser::MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                                            OperandVector &Operands,
                                            MCStreamer &Out,
                                            uint64_t &ErrorInfo,
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
    Inst->setLoc(IDLoc);
    if (validateInstruction(*Inst, Operands))
      return true;
    // If we the instruction ends with a ';', or we are already inside a bundle,
    bool EndOfBundle =
        getTok().getString().empty() || getTok().getString()[0] != ';';
    if (!EndOfBundle || !Bundle.empty()) {
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
      if (EndOfBundle)
        return emitBundle(Out);
      return true;
    }
    Out.emitInstruction(*Inst, getSTI());
    return false;
  }
  // TODO: Properly implement the following cases
  case Match_InvalidTiedOperand:
  case Match_InvalidOperand: {
    SMLoc ErrorLoc = IDLoc;
    if (ErrorInfo != ~0ULL) {
      if (ErrorInfo >= Operands.size())
        return Error(ErrorLoc, "too few operands for instructions");

      ErrorLoc = ((AIE2Operand &)*Operands[ErrorInfo]).getStartLoc();
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
    StringRef Mnemonic = ((AIE2Operand &)*Operands[0]).getToken();
    std::string Suggestion = AIE2MnemonicSpellCheck(Mnemonic, FBS, 0);
    return Error(IDLoc, "unrecognized instruction mnemonic" + Suggestion);
  }
  }
  llvm_unreachable("Unkown match type detected");
}

/// parseImmediate:
///   ::= "#" Integer
///   ::= "#" Identifier
bool AIE2AsmParser::parseImmediate(OperandVector &Operands) {
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
  Operands.push_back(AIE2Operand::CreateImm(getContext(), Res, S, E));
  return MatchOperand_Success;
}

static Register convertToD_3D(Register Reg) {
  // We could use the existing eDS/eD reg classes and play around with
  // subreg indices, but that would introduce a dependency with CodeGen.
  switch (Reg) {
  case AIE2::d0:
    return AIE2::d0_3d;
  case AIE2::d1:
    return AIE2::d1_3d;
  case AIE2::d2:
    return AIE2::d2_3d;
  case AIE2::d3:
    return AIE2::d3_3d;
  default:
    return Reg;
  }
}

/// parseIdentifier:
///   ::= Identifier
///   ::= Identifier ":" Identifier
bool AIE2AsmParser::parseIdentifier(OperandVector &Operands) {
  MCRegister RegNo;
  SMLoc Begin;
  SMLoc End;
  if (parseRegister(RegNo, Begin, End) == MatchOperand_Success) {

    // FIXME: Some of the registers in eD and eDS have the same names.
    // Here, we backpatch the string-matched eD registers into their eDS
    // super-register when parsing a .3d instruction.
    // This should really be generically handled by MatchRegisterName(), but to
    // be fair, this is strange to have different registers with the same name.
    StringRef InstrName =
        static_cast<AIEBaseOperand *>(Operands[0].get())->getToken();
    if (InstrName.find_insensitive(".3d") != StringRef::npos) {
      RegNo = convertToD_3D(RegNo);
    }

    Operands.push_back(AIE2Operand::CreateReg(getContext(), RegNo, Begin, End));
  } else if (matchTokenString(getTok().getString())) {
    // Instructions such as VMOV_mv_mcd have operands that are not immediates or
    // register: MCD, SCD... This will match these operands
    auto Tok = getTok();
    Operands.push_back(
        AIE2Operand::CreateToken(getContext(), Tok.getString(), Tok.getLoc()));
    Lex();
  } else
    return Error(Begin, "operand is not a register, nor a known identifier");
  return MatchOperand_Success;
}

/// parseIndirectOrIndexedMode
///   ::= "[" <ptr> "]"
///   ::= "[" <ptr> "," Register "]"
///   ::= "[" <ptr> "," "#" Integer "]"
bool AIE2AsmParser::parseIndirectOrIndexedMode(OperandVector &Operands) {
  AsmToken const &LBracToken = getLexer().getTok();
  // First token is "["
  assert(LBracToken.getString() == "[" && "Operand is not '['");
  Operands.push_back(AIE2Operand::CreateToken(
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
    Operands.push_back(AIE2Operand::CreateToken(
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
bool AIE2AsmParser::parseOperand(OperandVector &Operands) {
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
bool AIE2AsmParser::ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                                     SMLoc NameLoc, OperandVector &Operands) {
  // First operand is token for instruction
  Operands.push_back(AIE2Operand::CreateToken(getContext(), Name, NameLoc));

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

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeAIEAsmParser() {
  RegisterMCAsmParser<AIE2AsmParser> X(getTheAIE2Target());
}
