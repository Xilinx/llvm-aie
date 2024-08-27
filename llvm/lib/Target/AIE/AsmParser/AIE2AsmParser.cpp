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

#include "AIEBaseAsmParser.h"

namespace {
AIE2MCFormats FormatInterface;
}

namespace {

class AIE2Operand : public AIEBaseOperand {};

class AIE2AsmParser
    : public AIEBaseAsmParser<AIE2AsmParser, AIE::MCBundle, AIE2Operand> {
private:
// Auto-generated Match Functions
#define GET_ASSEMBLER_HEADER
#include "AIE2GenAsmMatcher.inc"

  bool parseIdentifier(OperandVector &Operands) override;
  bool validateInstruction(MCInst &Inst, OperandVector &Operands) override;
  unsigned matchRegister(std::string Name) override;
  bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;

public:
  AIE2AsmParser(const MCSubtargetInfo &STI, MCAsmParser &Parser,
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
#include "AIE2GenAsmMatcher.inc"

unsigned AIE2AsmParser::matchRegister(std::string Name) {
  unsigned RegNo = MatchRegisterName(Name);
  if (RegNo)
    return RegNo;
  return MatchRegisterAltName(Name);
}

bool AIE2AsmParser::validateInstruction(MCInst &Inst, OperandVector &Operands) {
  // The index of the operand in \a Inst and \a Operands will always be
  // different, because Operands also contains the instruction name, and
  // sometimes other syntax tokens, such as "[", "]" (it will not contain
  // ",")...
  switch (Inst.getOpcode()) {
  case AIE2::PADDA_lda_ptr_inc_idx_imm:
  case AIE2::PADDS_st_ptr_inc_idx_imm: {
    const int64_t Imm = Inst.getOperand(2).getImm();
    if (!isValidImm<10, 2>(Imm))
      return Error(Operands[3]->getStartLoc(),
                   "PADDA/PADDS cannot handle immediates > +-2^12");
    if (Imm % 4 != 0)
      return Error(Operands[3]->getStartLoc(),
                   "PADDA/PADDS immediates must be multiples of 4");
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
    return processMatchedInstruction(IDLoc, Operands, Out, Inst);
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
  llvm_unreachable("Unknown match type detected");
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

void LLVMInitializeAIE2AsmParser() {
  RegisterMCAsmParser<AIE2AsmParser> X(getTheAIE2Target());
}
