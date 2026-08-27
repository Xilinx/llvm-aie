//===-- AIE2PSAsmParser.cpp - Assembly Parser for AIE2PS ----------*- C++ -*--//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCParser/AsmLexer.h"
#include "llvm/MC/MCParser/MCAsmParser.h"

#include "AIEBaseOperand.h"
#include "AIEBundle.h"
#include "MCTargetDesc/AIEMCExpr.h"
#include "MCTargetDesc/aie2ps/AIE2PSMCTargetDesc.h"
#include "TargetInfo/AIETargetInfo.h"
#include "aie2ps/AIE2PSInstrInfo.h"
#include "aie2ps/AIE2PSRegisterInfo.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

#define DEBUG_TYPE "mcasmparser"

using namespace llvm;

#include "AIEBaseAsmParser.h"

namespace {
AIE2PSMCFormats FormatInterface;
}

namespace {

class AIE2PSOperand : public AIEBaseOperand {};

class AIE2PSAsmParser
    : public AIEBaseAsmParser<AIE2PSAsmParser, AIE::MCBundle, AIE2PSOperand> {
private:
// Auto-generated Match Functions
#define GET_ASSEMBLER_HEADER
#include "AIE2PSGenAsmMatcher.inc"

  bool parseIdentifier(OperandVector &Operands) override;
  bool validateInstruction(MCInst &Inst, OperandVector &Operands) override;
  unsigned matchRegister(std::string Name) override;
  bool matchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;
  bool parseImmediate(OperandVector &Operands) override;

public:
  AIE2PSAsmParser(const MCSubtargetInfo &STI, MCAsmParser &Parser,
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
#include "AIE2PSGenAsmMatcher.inc"

unsigned AIE2PSAsmParser::matchRegister(std::string Name) {
  unsigned RegNo = MatchRegisterName(Name);
  if (RegNo)
    return RegNo;
  return MatchRegisterAltName(Name);
}

bool AIE2PSAsmParser::validateInstruction(MCInst &Inst,
                                          OperandVector &Operands) {
  // The index of the operand in \a Inst and \a Operands will always be
  // different, because Operands also contains the instruction name, and
  // sometimes other syntax tokens, such as "[", "]" (it will not contain
  // ",")...
  switch (Inst.getOpcode()) {
  case AIE2PS::PADDA_pstm_nrm_imm:
  case AIE2PS::PADDB_pstm_nrm_imm: {
    const int64_t Imm = Inst.getOperand(2).getImm();
    if (!isValidImm<4, 6>(Imm))
      return Error(Operands[3]->getStartLoc(),
                   "PADDA can only handle immediates in [-2^9, 2^9-1]");
    if (Imm % 64 != 0)
      return Error(Operands[3]->getStartLoc(),
                   "PADDA immediates must be multiples of 64");
    break;
  }
  case AIE2PS::MOVA: {
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

bool AIE2PSAsmParser::matchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                                              OperandVector &Operands,
                                              MCStreamer &Out,
                                              uint64_t &ErrorInfo,
                                              bool MatchingInlineAsm) {
  MCInst *Inst = getContext().createMCInst();
  LLVM_DEBUG(dbgs() << "Emitting...\t"
                    << "instruction ending with" << getTok().getString());

  // Special handling for EVENT instructions with hardcoded immediates
  // EVENT_event0 and EVENT_event1 have no operands but their AsmString
  // includes "#0" or "#1", so we need to handle them specially
  if (Operands.size() >= 2) {
    StringRef Mnemonic = ((AIE2PSOperand &)*Operands[0]).getToken();
    if (Mnemonic == "event") {
      // The operands will be: [0]=mnemonic, [1]="#", [2]=immediate
      // Find the immediate operand (skip tokens like "#")
      for (unsigned i = 1; i < Operands.size(); ++i) {
        AIE2PSOperand &Op = (AIE2PSOperand &)*Operands[i];
        if (Op.isImm()) {
          const MCExpr *Expr = Op.getImm();
          if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Expr)) {
            int64_t ImmVal = CE->getValue();
            if (ImmVal == 0) {
              Inst->setOpcode(AIE2PS::EVENT_event0);
              return processMatchedInstruction(IDLoc, Operands, Out, Inst);
            } else if (ImmVal == 1) {
              Inst->setOpcode(AIE2PS::EVENT_event1);
              return processMatchedInstruction(IDLoc, Operands, Out, Inst);
            }
          }
          break; // Found immediate, stop looking
        }
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

      ErrorLoc = ((AIE2PSOperand &)*Operands[ErrorInfo]).getStartLoc();
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
    StringRef Mnemonic = ((AIE2PSOperand &)*Operands[0]).getToken();
    std::string Suggestion = AIE2PSMnemonicSpellCheck(Mnemonic, FBS, 0);
    return Error(IDLoc, "unrecognized instruction mnemonic" + Suggestion);
  }
  }
  llvm_unreachable("Unknown match type detected");
}

// Some AIE2PS bfp16 instructions (vmac.f, vmsc.f, vmul.f, vnegmul.f,
// vaddmac.f, vaddmsc.f) print their EWL (low-half) operand using the name of
// the enclosing EX register (e.g. "ex11" instead of "ewl11"). The custom
// AIE2PSInstPrinter::printOperand implements this aliasing. To be able to read
// this syntax back, we perform the inverse mapping here: when one of these
// mnemonics is parsed with an EX register operand, we convert it into its
// corresponding EWL register so that the auto-generated matcher (which expects
// the EWL operand class) accepts it.
static Register convertEXToEWL(Register Reg) {
  switch (Reg) {
  case AIE2PS::ex0:
    return AIE2PS::ewl0;
  case AIE2PS::ex1:
    return AIE2PS::ewl1;
  case AIE2PS::ex2:
    return AIE2PS::ewl2;
  case AIE2PS::ex3:
    return AIE2PS::ewl3;
  case AIE2PS::ex4:
    return AIE2PS::ewl4;
  case AIE2PS::ex5:
    return AIE2PS::ewl5;
  case AIE2PS::ex6:
    return AIE2PS::ewl6;
  case AIE2PS::ex7:
    return AIE2PS::ewl7;
  case AIE2PS::ex8:
    return AIE2PS::ewl8;
  case AIE2PS::ex9:
    return AIE2PS::ewl9;
  case AIE2PS::ex10:
    return AIE2PS::ewl10;
  case AIE2PS::ex11:
    return AIE2PS::ewl11;
  default:
    return Reg;
  }
}

// Returns true if the given instruction mnemonic corresponds to a bfp16
// instruction that uses the EX-for-EWL printing alias.
static bool usesEWLBisAlias(StringRef InstrName) {
  return InstrName.equals_insensitive("vmac.f") ||
         InstrName.equals_insensitive("vmsc.f") ||
         InstrName.equals_insensitive("vmul.f") ||
         InstrName.equals_insensitive("vnegmul.f") ||
         InstrName.equals_insensitive("vaddmac.f") ||
         InstrName.equals_insensitive("vaddmsc.f");
}

static Register convertToD_3D(Register Reg) {
  // We could use the existing eDS/eD reg classes and play around with
  // subreg indices, but that would introduce a dependency with CodeGen.
  switch (Reg) {
  case AIE2PS::d0:
    return AIE2PS::d0_3d;
  case AIE2PS::d1:
    return AIE2PS::d1_3d;
  case AIE2PS::d2:
    return AIE2PS::d2_3d;
  case AIE2PS::d3:
    return AIE2PS::d3_3d;
  default:
    return Reg;
  }
}
/// parseIdentifier:
///   ::= Identifier
///   ::= Identifier ":" Identifier
bool AIE2PSAsmParser::parseIdentifier(OperandVector &Operands) {
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
    // For bfp16 instructions, the low-half (EWL) operand is printed using the
    // enclosing EX register name. Convert it back to the EWL register so the
    // matcher, which expects the EWL operand class, accepts it.
    if (usesEWLBisAlias(InstrName)) {
      RegNo = convertEXToEWL(RegNo);
    }
    Operands.push_back(
        AIE2PSOperand::CreateReg(getContext(), RegNo, Begin, End));
  } else if (matchTokenString(getTok().getString())) {
    // Instructions such as VMOV_st_mv_mcd_x have operands that are not
    // immediates or register: MCD, SCD... This will match these operands
    auto Tok = getTok();
    Operands.push_back(AIE2PSOperand::CreateToken(getContext(), Tok.getString(),
                                                  Tok.getLoc()));
    Lex();
  } else
    return Error(Begin, "operand is not a register, nor a known identifier");
  return false;
}

/// parseImmediate:
///   ::= "#" Integer
///   ::= "#" Identifier
bool AIE2PSAsmParser::parseImmediate(OperandVector &Operands) {
  auto Tok = getTok();
  Operands.push_back(
      AIE2PSOperand::CreateToken(getContext(), "#", Tok.getLoc()));
  return AIEBaseAsmParser::parseImmediate(Operands);
}

// Register the AIE2PS asm parser; called from LLVMInitializeAIEAsmParser.
void initializeAIE2PSAsmParser() {
  RegisterMCAsmParser<AIE2PSAsmParser> X(getTheAIE2PSTarget());
}
