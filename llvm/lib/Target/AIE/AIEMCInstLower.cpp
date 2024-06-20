//===-- AIEMCInstLower.cpp - Convert AIE MachineInstr to an MCInst ----------=//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains code to lower AIE MachineInstrs to their corresponding
// MCInst records.
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "MCTargetDesc/AIEMCExpr.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

static MCOperand lowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym,
                                    const AsmPrinter &AP) {
    //llvm_unreachable("lowerSymbolOperand unsupported");
  MCContext &Ctx = AP.OutContext;
  AIEMCExpr::VariantKind Kind;

  switch (MO.getTargetFlags()) {
  default:
      //  llvm_unreachable("Unknown target flag on GV operand");

  case AIEII::MO_None:
    Kind = AIEMCExpr::VK_AIE_None;
    break;
  case AIEII::MO_CALL:
    Kind = AIEMCExpr::VK_AIE_CALL;
    break;
  case AIEII::MO_GLOBAL:
    Kind = AIEMCExpr::VK_AIE_GLOBAL;
    break;
  // case AIEII::MO_LO:
  //   Kind = AIEMCExpr::VK_AIE_LO;
  //   break;
  // case AIEII::MO_HI:
  //   Kind = AIEMCExpr::VK_AIE_HI;
  //   break;
  // case AIEII::MO_PCREL_LO:
  //   Kind = AIEMCExpr::VK_AIE_PCREL_LO;
  //   break;
  // case AIEII::MO_PCREL_HI:
  //   Kind = AIEMCExpr::VK_AIE_PCREL_HI;
  //   break;
    }

  const MCExpr *ME =
      MCSymbolRefExpr::create(Sym, MCSymbolRefExpr::VK_None, Ctx);

  if (!MO.isJTI() && !MO.isMBB() && MO.getOffset())
    ME = MCBinaryExpr::createAdd(
        ME, MCConstantExpr::create(MO.getOffset(), Ctx), Ctx);

  if (Kind != AIEMCExpr::VK_AIE_None)
      ME = AIEMCExpr::create(ME, Kind, Ctx);
  return MCOperand::createExpr(ME);
}

bool llvm::LowerAIEMachineOperandToMCOperand(const MachineOperand &MO,
                                               MCOperand &MCOp,
                                               const AsmPrinter &AP) {
  switch (MO.getType()) {
  default:
  case MachineOperand::MO_Register:
    // Ignore all implicit register operands.
    if (MO.isImplicit())
      return false;
    MCOp = MCOperand::createReg(MO.getReg());
    break;
  case MachineOperand::MO_RegisterMask:
    // Regmasks are like implicit defs.
    return false;
  case MachineOperand::MO_Immediate:
    MCOp = MCOperand::createImm(MO.getImm());
    break;
  case MachineOperand::MO_MachineBasicBlock:
    MCOp = lowerSymbolOperand(MO, MO.getMBB()->getSymbol(), AP);
    break;
  case MachineOperand::MO_GlobalAddress:
    MCOp = lowerSymbolOperand(MO, AP.getSymbol(MO.getGlobal()), AP);
    break;
  case MachineOperand::MO_BlockAddress:
    MCOp = lowerSymbolOperand(
        MO, AP.GetBlockAddressSymbol(MO.getBlockAddress()), AP);
    break;
  case MachineOperand::MO_ExternalSymbol:
    MCOp = lowerSymbolOperand(
        MO, AP.GetExternalSymbolSymbol(MO.getSymbolName()), AP);
    break;
  case MachineOperand::MO_ConstantPoolIndex:
    MCOp = lowerSymbolOperand(MO, AP.GetCPISymbol(MO.getIndex()), AP);
    break;
  case MachineOperand::MO_JumpTableIndex:
    MCOp = lowerSymbolOperand(MO, AP.GetJTISymbol(MO.getIndex()), AP);
    break;
  }
  return true;
}

void llvm::LowerAIEMachineInstrToMCInst(const MachineInstr *MI, MCInst &OutMI,
                                          const AsmPrinter &AP) {
  OutMI.setOpcode(MI->getOpcode());

  for (const MachineOperand &MO : MI->operands()) {
    MCOperand MCOp;
    if (LowerAIEMachineOperandToMCOperand(MO, MCOp, AP))
      OutMI.addOperand(MCOp);
  }
}
