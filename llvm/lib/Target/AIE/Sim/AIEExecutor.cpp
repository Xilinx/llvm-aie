//===- AIEExecutor.cpp - Bundle-at-a-time execution of AIE code -----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEExecutor.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace llvm::AIESim;

AIEHostInterface::~AIEHostInterface() = default;
AIESemantics::~AIESemantics() = default;

AIEExecutor::AIEExecutor(const MCDisassembler &DisAsm, const MCInstrInfo &MII,
                         const MCRegisterInfo &MRI, AIESemantics &Sem,
                         AIEHostInterface &Host, uint64_t EntryPoint)
    : DisAsm(DisAsm), MII(MII), MRI(MRI), Sem(Sem), Host(Host),
      State(MRI, EntryPoint) {
  const std::array<MCRegister, 3> Loop = Sem.getLoopRegisters();
  LCReg = Loop[0];
  LSReg = Loop[1];
  LEReg = Loop[2];
}

static std::string describe(const MCInstrInfo &MII, const MCInst &MI) {
  return (Twine(MII.getName(MI.getOpcode())) + " (opcode " +
          Twine(MI.getOpcode()) + ")")
      .str();
}

StepResult AIEExecutor::executeSlot(const MCInst &MI, SlotEffects &Eff) {
  // A slot whose sub-decoder failed is left cleared by the disassembler, and an
  // opcode of zero is the only trace of that.
  if (MI.getOpcode() == 0 && MI.getNumOperands() == 0) {
    FaultMsg = "undecodable issue slot";
    return StepResult::Fault;
  }

  Executed.insert(MI.getOpcode());
  if (Sem.isEndOfProgram(MI))
    return StepResult::Done;

  const size_t FirstWrite = Eff.RegWrites.size();
  std::string SlotFault;
  StepResult R = Sem.execute(MI, State, Host, Eff, SlotFault);
  if (R == StepResult::Fault) {
    Unmodelled.insert(MI.getOpcode());
    FaultMsg = SlotFault.empty() ? describe(MII, MI) : SlotFault;
    return R;
  }
  if (R != StepResult::Retired)
    return R;

  // Anything the description says this instruction defines but the semantics
  // left alone holds an unknown value from here on.
  const MCInstrDesc &Desc = MII.get(MI.getOpcode());
  auto PoisonUnwritten = [&](MCRegister Reg) {
    if (!any_of(ArrayRef(Eff.RegWrites).drop_front(FirstWrite),
                [&](const auto &W) { return W.first == Reg; }))
      Eff.RegPoisons.push_back(Reg);
  };
  for (MCPhysReg Reg : Desc.implicit_defs())
    PoisonUnwritten(Reg);
  for (unsigned I = 0, E = Desc.getNumDefs(); I != E; ++I)
    if (I < MI.getNumOperands() && MI.getOperand(I).isReg())
      PoisonUnwritten(MI.getOperand(I).getReg());
  return R;
}

void AIEExecutor::commit(const SlotEffects &Eff) {
  for (MCRegister Reg : Eff.RegPoisons)
    State.Regs.poison(Reg);
  for (const auto &[Reg, Value] : Eff.RegWrites)
    State.Regs.write(Reg, Value);
  for (const auto &W : Eff.MemWrites)
    Host.store(W.Addr, W.NumBytes, W.Value);
}

void AIEExecutor::advancePC(uint64_t BundleAddr, uint64_t BundleSize) {
  State.PC = BundleAddr + BundleSize;
  ++State.RetiredBundles;

  if (State.Branch && --State.Branch->BundlesLeft == 0) {
    State.PC = State.Branch->Target;
    State.Branch.reset();
    return;
  }

  // The loop-end label is attached to the last bundle of the body
  // (AIEBaseAsmPrinter.cpp, setPreInstrSymbol), so the back edge is taken as
  // that bundle retires. lc holds the iteration count: the compiler emits
  // "add.nc lc, rN, #0" for a loop of rN iterations.
  if (!LEReg || !LCReg || !LSReg)
    return;
  APInt LE, LC, LS;
  if (!State.Regs.read(LEReg, LE) || !State.Regs.read(LCReg, LC) ||
      !State.Regs.read(LSReg, LS))
    return;
  if (LC.isZero() || LE.getZExtValue() != BundleAddr)
    return;
  LC -= 1;
  State.Regs.write(LCReg, LC);
  if (!LC.isZero())
    State.PC = LS.getZExtValue();
}

StepResult AIEExecutor::step() {
  const uint64_t BundleAddr = State.PC;
  ArrayRef<uint8_t> Bytes = Host.fetch(BundleAddr);
  if (Bytes.empty()) {
    FaultMsg = ("no program memory at " + Twine::utohexstr(BundleAddr)).str();
    return StepResult::Fault;
  }

  MCInst Bundle;
  uint64_t Size = 0;
  if (DisAsm.getInstruction(Bundle, Size, Bytes, BundleAddr, nulls()) !=
      MCDisassembler::Success) {
    FaultMsg = ("undecodable bundle at " + Twine::utohexstr(BundleAddr)).str();
    return StepResult::Fault;
  }

  const bool IsComposite =
      any_of(Bundle, [](const MCOperand &Op) { return Op.isInst(); });

  SlotEffects Eff;
  bool Done = false;
  if (IsComposite) {
    for (const MCOperand &Op : Bundle) {
      if (!Op.isInst()) {
        FaultMsg = "bundle carries a non-instruction operand";
        return StepResult::Fault;
      }
      switch (executeSlot(*Op.getInst(), Eff)) {
      case StepResult::Retired:
        break;
      case StepResult::Done:
        Done = true;
        break;
      case StepResult::Stalled:
        return StepResult::Stalled;
      case StepResult::Fault:
        return StepResult::Fault;
      }
    }
  } else {
    switch (executeSlot(Bundle, Eff)) {
    case StepResult::Retired:
      break;
    case StepResult::Done:
      Done = true;
      break;
    case StepResult::Stalled:
      return StepResult::Stalled;
    case StepResult::Fault:
      return StepResult::Fault;
    }
  }

  commit(Eff);
  if (Done)
    return StepResult::Done;

  if (Eff.Branch && State.Branch) {
    FaultMsg = "control transfer issued inside another one's delay slots";
    return StepResult::Fault;
  }
  advancePC(BundleAddr, Size);
  if (Eff.Branch)
    State.Branch = {*Eff.Branch, NumDelaySlots};
  return StepResult::Retired;
}
