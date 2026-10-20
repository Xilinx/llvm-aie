//===-- AIE2PAsmParser.cpp - Assembly Parser for AIE2P --------------*- C++
//-*--//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCParser/AsmLexer.h"
#include "llvm/MC/MCParser/MCAsmParser.h"

#include "AIEBaseOperand.h"
#include "AIEBundle.h"
#include "MCTargetDesc/AIEMCExpr.h"
#include "MCTargetDesc/aie2p/AIE2PMCTargetDesc.h"
#include "TargetInfo/AIETargetInfo.h"
#include "aie2p/AIE2PInstrInfo.h"
#include "aie2p/AIE2PRegisterInfo.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

#define DEBUG_TYPE "mcasmparser"

using namespace llvm;

#include "AIEBaseAsmParser.h"

namespace {
AIE2PMCFormats FormatInterface;
}

namespace {

class AIE2POperand : public AIEBaseOperand {};

class AIE2PAsmParser
    : public AIEBaseAsmParser<AIE2PAsmParser, AIE::MCBundle, AIE2POperand> {
private:
// Auto-generated Match Functions
#define GET_ASSEMBLER_HEADER
#include "AIE2PGenAsmMatcher.inc"

  bool parseIdentifier(OperandVector &Operands) override;
  bool validateInstruction(MCInst &Inst, OperandVector &Operands) override;
  unsigned matchRegister(std::string Name) override;
  bool matchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;
  bool parseImmediate(OperandVector &Operands) override;

public:
  AIE2PAsmParser(const MCSubtargetInfo &STI, MCAsmParser &Parser,
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
#include "AIE2PGenAsmMatcher.inc"

unsigned AIE2PAsmParser::matchRegister(std::string Name) {
  unsigned RegNo = MatchRegisterName(Name);
  if (RegNo)
    return RegNo;
  return MatchRegisterAltName(Name);
}

bool AIE2PAsmParser::validateInstruction(MCInst &Inst,
                                         OperandVector &Operands) {
  // The index of the operand in \a Inst and \a Operands will always be
  // different, because Operands also contains the instruction name, and
  // sometimes other syntax tokens, such as "[", "]" (it will not contain
  // ",")...
  switch (Inst.getOpcode()) {
  case AIE2P::PADDA_pstm_nrm_imm:
  case AIE2P::PADDB_pstm_nrm_imm: {
    const int64_t Imm = Inst.getOperand(2).getImm();
    if (!isValidImm<4, 6>(Imm))
      return Error(Operands[3]->getStartLoc(),
                   "PADDA can only handle immediates in [-2^9, 2^9-1]");
    if (Imm % 64 != 0)
      return Error(Operands[3]->getStartLoc(),
                   "PADDA immediates must be multiples of 64");
    break;
  }
  case AIE2P::MOVA: {
    const int64_t Imm = Inst.getOperand(1).getImm();
    if (!isValidImm<11, 0>(Imm))
      return Error(Operands[2]->getStartLoc(),
                   "MOVA can only handle immediates [-2^10, 2^10-1]");
    break;
  }
  default:
    break;
  }
  return false;
}

bool AIE2PAsmParser::matchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                                             OperandVector &Operands,
                                             MCStreamer &Out,
                                             uint64_t &ErrorInfo,
                                             bool MatchingInlineAsm) {
  MCInst *Inst = getContext().createMCInst();
  LLVM_DEBUG(dbgs() << "Emitting...\t"
                    << "instruction ending with" << getTok().getString());

  // `event #0` and `event #1` spell the immediate inside their AsmString
  // instead of carrying an operand, so the matcher has nothing to match the
  // parsed `#0` against and rejects the line. Choose the opcode from the
  // immediate. An immediate that is neither falls through to the matcher and
  // gets its usual diagnostic. event.warning and event.error are separate
  // mnemonics and need none of this.
  if (Operands.size() >= 2) {
    StringRef Mnemonic = ((AIE2POperand &)*Operands[0]).getToken();
    if (Mnemonic == "event") {
      for (unsigned I = 1; I < Operands.size(); ++I) {
        AIE2POperand &Op = (AIE2POperand &)*Operands[I];
        if (!Op.isImm())
          continue;
        if (const auto *CE = dyn_cast<MCConstantExpr>(Op.getImm())) {
          if (CE->getValue() == 0 || CE->getValue() == 1) {
            Inst->setOpcode(CE->getValue() == 0 ? AIE2P::EVENT_event0
                                                : AIE2P::EVENT_event1);
            return processMatchedInstruction(IDLoc, Operands, Out, Inst);
          }
        }
        break;
      }
    }
  }

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

      ErrorLoc = ((AIE2POperand &)*Operands[ErrorInfo]).getStartLoc();
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
    StringRef Mnemonic = ((AIE2POperand &)*Operands[0]).getToken();
    std::string Suggestion = AIE2PMnemonicSpellCheck(Mnemonic, FBS, 0);
    return Error(IDLoc, "unrecognized instruction mnemonic" + Suggestion);
  }
  }
  llvm_unreachable("Unknown match type detected");
}

static Register convertToD_3D(Register Reg) {
  // We could use the existing eDS/eD reg classes and play around with
  // subreg indices, but that would introduce a dependency with CodeGen.
  switch (Reg) {
  case AIE2P::d0:
    return AIE2P::d0_3d;
  case AIE2P::d1:
    return AIE2P::d1_3d;
  case AIE2P::d2:
    return AIE2P::d2_3d;
  case AIE2P::d3:
    return AIE2P::d3_3d;
  default:
    return Reg;
  }
}
/// parseIdentifier:
///   ::= Identifier
///   ::= Identifier ":" Identifier
bool AIE2PAsmParser::parseIdentifier(OperandVector &Operands) {
  MCRegister RegNo;
  SMLoc Begin;
  SMLoc End;
  if (!parseRegister(RegNo, Begin, End)) {
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
    Operands.push_back(
        AIE2POperand::CreateReg(getContext(), RegNo, Begin, End));
  } else if (matchTokenString(getTok().getString())) {
    // Instructions such as VMOV_st_mv_mcd_x have operands that are not
    // immediates or register: MCD, SCD... This will match these operands
    auto Tok = getTok();
    Operands.push_back(
        AIE2POperand::CreateToken(getContext(), Tok.getString(), Tok.getLoc()));
    Lex();
  } else
    return Error(Begin, "operand is not a register, nor a known identifier");
  return false;
}

/// parseImmediate:
///   ::= "#" Integer
///   ::= "#" Identifier
bool AIE2PAsmParser::parseImmediate(OperandVector &Operands) {
  auto Tok = getTok();
  Operands.push_back(
      AIE2POperand::CreateToken(getContext(), "#", Tok.getLoc()));
  return AIEBaseAsmParser::parseImmediate(Operands);
}

// Register the AIE2P asm parser; called from LLVMInitializeAIEAsmParser.
void initializeAIE2PAsmParser() {
  RegisterMCAsmParser<AIE2PAsmParser> X(getTheAIE2PTarget());
}
