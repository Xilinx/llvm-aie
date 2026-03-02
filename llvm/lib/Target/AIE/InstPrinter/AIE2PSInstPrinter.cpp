//===----- AIE2PSInstPrinter.cpp - Convert AIE2p MCInst to asm syntax -----===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This class prints an AIE2ps MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "AIE2PSInstPrinter.h"
#include "AIEInstPrinter.h"
#include "MCTargetDesc/aie2ps/AIE2PSMCTargetDesc.h"
#include "Utils/AIEBaseInfo.h"
#include "aie2ps/AIE2PSInstrInfo.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include <optional>

using namespace llvm;

#define DEBUG_TYPE "aie2ps-asm-printer"

// Include the auto-generated portion of the assembly writer.
#define PRINT_ALIAS_INSTR
#include "AIE2PSGenAsmWriter.inc"

void AIE2PSInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                  StringRef Annot, const MCSubtargetInfo &STI,
                                  raw_ostream &O) {
  AIECommonInstPrinter::printInstr(MI, Address, Annot, STI, O);
}

void AIE2PSInstPrinter::printRegName(raw_ostream &O, MCRegister RegNo) {
  O << getRegisterName(RegNo);
}

template <int offset>
void AIE2PSInstPrinter::printImmOffset(const MCInst *MI, unsigned OpNo,
                                       const MCSubtargetInfo &STI,
                                       raw_ostream &O, const char *Modifier) {
  assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");
  const MCOperand &MO = MI->getOperand(OpNo);
  AIECommonInstPrinter::printImmOffset<offset>(MO, STI, O);
}

// Some AIE2PS instructions print EWL operands as EX. So we override the default
// syntax in printOperand using the lookup table below.
static std::optional<MCRegister> getEWL_bisRegisterAlias(unsigned Reg) {
  static const std::unordered_map<unsigned, MCRegister> registerMap = {
      {AIE2PS::ewl0, AIE2PS::ex0},   {AIE2PS::ewl1, AIE2PS::ex1},
      {AIE2PS::ewl2, AIE2PS::ex2},   {AIE2PS::ewl3, AIE2PS::ex3},
      {AIE2PS::ewl4, AIE2PS::ex4},   {AIE2PS::ewl5, AIE2PS::ex5},
      {AIE2PS::ewl6, AIE2PS::ex6},   {AIE2PS::ewl7, AIE2PS::ex7},
      {AIE2PS::ewl8, AIE2PS::ex8},   {AIE2PS::ewl9, AIE2PS::ex9},
      {AIE2PS::ewl10, AIE2PS::ex10}, {AIE2PS::ewl11, AIE2PS::ex11}};

  auto it = registerMap.find(Reg);
  if (it != registerMap.end()) {
    return it->second;
  }
  return {};
}

void AIE2PSInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                     const MCSubtargetInfo &STI, raw_ostream &O,
                                     const char *Modifier) {
  assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");
  const MCOperand &MO = MI->getOperand(OpNo);

  // Some AIE2PS instructions print EWL operans as EX. So we override the
  // default syntax here.
  if (MO.isReg()) {
    unsigned Reg = MO.getReg();
    if (auto AltRegSyntax = getEWL_bisRegisterAlias(Reg)) {
      switch (MI->getOpcode()) {
      case AIE2PS::VADDMAC_f_vaddmac_bfp16:
      case AIE2PS::VADDMAC_f_vaddmac_bfp16_scd_add_scd_incr:
      case AIE2PS::VADDMAC_f_vaddmac_bfp16_scd_add_scd_noincr:
      case AIE2PS::VADDMSC_f_vaddmac_bfp16:
      case AIE2PS::VADDMSC_f_vaddmac_bfp16_scd_add_scd_incr:
      case AIE2PS::VADDMSC_f_vaddmac_bfp16_scd_add_scd_noincr:
      case AIE2PS::VMAC_f_vmac_bfp16:
      case AIE2PS::VMSC_f_vmac_bfp16:
      case AIE2PS::VMUL_f_vmul_bfp16:
      case AIE2PS::VNEGMUL_f_vmul_bfp16:
        O << getRegisterName(*AltRegSyntax);
        return;
      }
    }
  }

  AIECommonInstPrinter::printOperand(MO, STI, O);
}
