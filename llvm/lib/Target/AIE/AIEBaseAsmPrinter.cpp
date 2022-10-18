//===-- AIEBaseAsmPrinter.cpp - AIE LLVM assembly writer ------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the AIE assembly language.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseAsmPrinter.h"
#include "AIEBundle.h"
#include "InstPrinter/AIEInstPrinter.h"
#include "llvm/Analysis/MemoryLocation.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

void AIEBaseAsmPrinter::EmitToStreamer(MCStreamer &S, const MCInst &Inst) {
  // This searches for SymbolRefs in sub-instruction operands to register them
  // as used symbols. This mimics the base MCStreamer::emitInstruction
  for (const auto &MO : Inst) {
    if (MO.isInst()) {
      const MCInst &SI = *MO.getInst();
      for (const auto &SMO : SI) {
        if (SMO.isExpr()) {
          S.visitUsedExpr(*SMO.getExpr());
        }
      }
    }
  }
  AsmPrinter::EmitToStreamer(S, Inst);
}

void AIEBaseAsmPrinter::emitFunctionBodyStart() {
  // AIEBaseAsmPrinter lives for the whole Module, clear out data from the
  // previous function.
  FunctionTakenMBBAddresses.clear();

  // Run through all instructions to collect the MBBs whose address
  // is taken. This later helps us to decide whether or not a MBB
  // might be fall-through.
  for (const MachineBasicBlock &MBB : *MF) {
    for (const MachineInstr &MI : MBB.instrs()) {
      for (ConstMIBundleOperands OP(MI); OP.isValid(); ++OP) {
        if (OP->isMBB()) {
          FunctionTakenMBBAddresses.insert(OP->getMBB());
        }
      }
    }
  }
}

/// emitComments - Pretty-print comments for Bundled Instructions.
static void emitComments(const MachineInstr *MI, raw_ostream &CommentOS) {
  auto *MF = MI->getMF();
  auto &Subtarget = MF->getSubtarget();
  auto *TII = static_cast<const AIEBaseInstrInfo *>(Subtarget.getInstrInfo());

  // Check for spills and reloads

  // We assume a single instruction only has a spill or reload, not
  // both.
  std::optional<unsigned> Size;
  if ((Size = MI->getRestoreSize(TII))) {
    CommentOS << *Size << "-byte Reload";
  } else if ((Size = MI->getFoldedRestoreSize(TII))) {
    if (*Size) {
      if (*Size == unsigned(MemoryLocation::UnknownSize))
        CommentOS << "Unknown-size Folded Reload";
      else
        CommentOS << *Size << "-byte Folded Reload";
    }
  } else if ((Size = MI->getSpillSize(TII))) {
    CommentOS << *Size << "-byte Spill";
  } else if ((Size = MI->getFoldedSpillSize(TII))) {
    if (*Size) {
      if (*Size == unsigned(MemoryLocation::UnknownSize))
        CommentOS << "Unknown-size Folded Spill";
      else
        CommentOS << *Size << "-byte Folded Spill";
    }
  }

  // Check for spill-induced copies
  if (MI->getAsmPrinterFlag(MachineInstr::ReloadReuse))
    CommentOS << " Reload Reuse";
}

void AIEBaseAsmPrinter::emitInstruction(const MachineInstr *MI) {

  if (MI->isBundle()) {
    // Bundles are fed through the regular encoder by adding the bundled
    // instructions as operands to a (synthetic) format instruction.
    // We reconstruct the VLIW bundle and retrieve the opcode for that format
    // from it.
    // We add the bundled instructions in the order defined by the
    // format, filling missing slots with the appropriate nop.
    const MachineBasicBlock *MBB = MI->getParent();
    auto *MF = MBB->getParent();
    auto &Subtarget = MF->getSubtarget();
    auto *TII = static_cast<const AIEBaseInstrInfo *>(Subtarget.getInstrInfo());
    AIE::MachineBundle Bundle(TII->getFormatInterface());

    raw_ostream &CommentOS = OutStreamer->getCommentOS();
    if (isVerbose() && MI->hasDelaySlot()) {
      assert(DelaySlotCounter == 0);
      DelaySlotCounter = TII->getNumDelaySlots(*MI);
    }

    MachineBasicBlock::const_instr_iterator I = ++MI->getIterator();
    while (I != MBB->instr_end() && I->isInsideBundle()) {
      MachineInstr *SlotMI = const_cast<MachineInstr *>(&(*I));
      Bundle.add(SlotMI);
      if (isVerbose())
        emitComments(SlotMI, CommentOS);

      I++;
    }
    if (isVerbose() && DelaySlotCounter > 0 && !MI->hasDelaySlot())
      CommentOS << " Delay Slot " << DelaySlotCounter-- << "\n";

    MCInst MCBundle;
    const VLIWFormat *Format = Bundle.getFormatOrNull();
    assert(Format);
    MCBundle.setOpcode(Format->Opcode);

    for (MCSlotKind Slot : Format->getSlots()) {
      const MCSlotInfo *SlotInfo = TII->getSlotInfo(Slot);
      assert(SlotInfo);

      llvm::MachineInstr *Instr = Bundle.at(Slot);
      // This can't be stack-based.
      MCInst *MCI = OutContext.createMCInst();
      if (!Instr) {
        MCI->setOpcode(SlotInfo->getNOPOpcode());
      } else {
        LowerAIEMachineInstrToMCInst(Instr, *MCI, *this);
      }

      // Don't actually emit this Sub-instruction.  Instead, add it
      // as an operand of the containing multi-slot instruction.
      // The AIEBaseMCCodeEmitter will handle encoding things properly.
      MCBundle.addOperand(MCOperand::createInst(MCI));
    }
    EmitToStreamer(*OutStreamer, MCBundle);
  } else if (MI->isMetaInstruction()) {
    // Do not emit anything for meta instructions.
  } else {
    // Other instructions might be single instructions or a
    // delaySlot instruction with multiple child NOOPS.
    MachineBasicBlock::const_instr_iterator I = MI->getIterator();
    MachineBasicBlock::const_instr_iterator E = MI->getParent()->instr_end();
    // The DelaySlotFiller uses a bundle to combine the delay slots with
    // the original control flow operation.  Here we explicitly iterate
    // over the instructions in the delay slots
    do {
      if (I->hasDelaySlot()) {
        // Currently all our instructions have 5 delay slots.
        DelaySlotCounter = 5;
      }

      // Give some comments about delay slots.
      if (DelaySlotCounter > 0 && !I->hasDelaySlot())
        OutStreamer->getCommentOS()
            << "Delay Slot " << DelaySlotCounter-- << "\n";
      // Do any auto-generated pseudo lowerings.
      if (emitPseudoExpansionLowering(*OutStreamer, &*I))
        continue;

      MCInst TmpInst;
      LowerAIEMachineInstrToMCInst(&*I, TmpInst, *this);
      EmitToStreamer(*OutStreamer, TmpInst);
    } while ((++I != E) && I->isInsideBundle()); // Delay slot check
  }
}

// isBlockOnlyReachableByFallthrough - We need to override this since the
// branches are not easily understandable at this stage of the flow.
// AsmPrinter would then think many blocks are fall-through and it would not
// print labels for those.
bool AIEBaseAsmPrinter::isBlockOnlyReachableByFallthrough(
    const MachineBasicBlock *MBB) const {
  if (!AsmPrinter::isBlockOnlyReachableByFallthrough(MBB))
    return false;

  // Not fall-through if BB is part of jump table.
  auto JumpsToBB = [MBB](const MachineJumpTableEntry &JTE) {
    return find(JTE.MBBs, MBB) != JTE.MBBs.end();
  };
  const MachineJumpTableInfo *JTI = MBB->getParent()->getJumpTableInfo();
  if (JTI && any_of(JTI->getJumpTables(), JumpsToBB)) {
    LLVM_DEBUG(dbgs() << "// MBB " << MBB->getName()
                      << " address taken by jumptable\n");
    return false;
  }

  // Conservatively assume MBB isn't fall-through if its address is used
  // anywhere in the function.
  if (FunctionTakenMBBAddresses.contains(MBB)) {
    LLVM_DEBUG(dbgs() << "// MBB " << MBB->getName()
                      << " address taken by instruction\n");
    return false;
  }
  return true;
}

// Mostly copied from the base class
void AIEBaseAsmPrinter::emitXXStructorList(const DataLayout &DL,
                                           const Constant *List, bool IsCtor) {

  auto &Context = OutStreamer->getContext();

  SmallVector<Structor, 8> Structors;
  preprocessXXStructorList(DL, List, Structors);
  if (Structors.empty())
    return;

  // Emit the structors in reverse order if we are using the .ctor/.dtor
  // initialization scheme.
  if (!TM.Options.UseInitArray)
    std::reverse(Structors.begin(), Structors.end());

  const MCSymbol *KeySym = nullptr;
  std::optional<int> Priority;
  for (Structor &S : Structors) {
    if (Priority) {
      assert(S.Priority == *Priority);
    } else {
      Priority = S.Priority;
    }
    if (GlobalValue *GV = S.ComdatKey) {
      if (GV->isDeclarationForLinker())
        // If the associated variable is not defined in this module
        // (it might be available_externally, or have been an
        // available_externally definition that was dropped by the
        // EliminateAvailableExternally pass), some other TU
        // will provide its dynamic initializer.
        continue;

      const MCSymbol *KeySymVal = getSymbol(GV);
      if (KeySymVal) {
        assert(!KeySym || KeySym == KeySymVal);
        KeySym = KeySymVal;
      }
    }
  }

  const size_t PtrSize = 4;
  const Align Align = DL.getPointerPrefAlignment();
  const size_t Size = PtrSize * Structors.size();
  const TargetLoweringObjectFile &Obj = getObjFileLowering();
  MCSection *OutputSection =
      (IsCtor ? Obj.getStaticCtorSection(*Priority, KeySym)
              : Obj.getStaticDtorSection(*Priority, KeySym));
  OutStreamer->switchSection(OutputSection);
  if (OutStreamer->getCurrentSection() != OutStreamer->getPreviousSection())
    emitAlignment(Align);
  // We need to create a named, sized object, otherwise some linkers
  // don't pick up the ctors/dtors sections
  const char *const Name = IsCtor ? ".ctors" : ".dtors";
  auto *Symbol = static_cast<MCSymbolELF *>(Context.getOrCreateSymbol(Name));
  Symbol->setUndefined();
  Symbol->setType(ELF::STT_OBJECT);
  Symbol->setBinding(ELF::STB_LOCAL);
  Symbol->setExternal(false);
  Symbol->setSize(MCConstantExpr::create(Size, Context));
  OutStreamer->emitLabel(Symbol);

  for (Structor &S : Structors) {
    emitXXStructor(DL, S.Func);
  }
}
