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
#include "llvm/MC/MCInstrItineraries.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace llvm::AIESim;

AIEHostInterface::~AIEHostInterface() = default;
AIESemantics::~AIESemantics() = default;

/// Walks one class's stage list, calling \p Fn once per occupied cycle with the
/// cycle relative to issue. Mirrors AIEHazardRecognizer's anyStage: a stage
/// occupies its units for getCycles() cycles, then the cursor moves on by
/// getNextCycles(), which is zero for a stage that runs alongside the next one.
template <typename FnT>
static void forEachStageCycle(const InstrItineraryData &Itin, unsigned SchedClass,
                              FnT Fn) {
  if (Itin.isEmpty() || Itin.isEndMarker(SchedClass))
    return;
  unsigned Cycle = 0;
  for (const InstrStage &IS : Itin.getStages(SchedClass)) {
    for (unsigned C = 0; C != IS.getCycles(); ++C)
      Fn(Cycle + C, IS.getUnits(), IS.getReservationKind());
    Cycle += IS.getNextCycles();
  }
}

unsigned ResourceScoreboard::depthFor(const InstrItineraryData &Itin,
                                      const MCInstrInfo &MII) {
  // Over the classes instructions actually name, rather than a class count the
  // itinerary does not expose.
  unsigned Max = 1;
  for (unsigned Opc = 0, E = MII.getNumOpcodes(); Opc != E; ++Opc)
    forEachStageCycle(Itin, MII.get(Opc).getSchedClass(),
                      [&](unsigned C, uint64_t, unsigned) {
                        Max = std::max(Max, C + 1);
                      });
  return Max;
}

const ResourceScoreboard::Slot *ResourceScoreboard::peek(uint64_t C) const {
  const Slot &S = Ring[C % Ring.size()];
  return S.At == C ? &S : nullptr;
}

ResourceScoreboard::Slot &ResourceScoreboard::open(uint64_t C) {
  Slot &S = Ring[C % Ring.size()];
  if (S.At != C) {
    S.At = C;
    S.Required.clear();
    S.Reserved.clear();
  }
  return S;
}

unsigned ResourceScoreboard::delay(const InstrItineraryData &Itin,
                                   ArrayRef<unsigned> SchedClasses,
                                   uint64_t At) const {
  auto Holds = [](ArrayRef<uint16_t> Units, uint64_t U) {
    return llvm::is_contained(Units, uint16_t(U));
  };
  for (unsigned D = 0, E = Ring.size(); D != E; ++D) {
    bool Fits = true;
    for (unsigned SC : SchedClasses)
      forEachStageCycle(Itin, SC, [&](unsigned C, uint64_t Unit, unsigned Kind) {
        const Slot *S = peek(At + D + C);
        if (!S)
          return;
        // Required conflicts with either kind; Reserved only with Required,
        // which is what lets any number of ordinary accesses coexist while a
        // part-word store's Required window excludes them all.
        if (Holds(S->Required, Unit) ||
            (Kind == InstrStage::Required && Holds(S->Reserved, Unit)))
          Fits = false;
      });
    if (Fits)
      return D;
  }
  // Past the ring every reservation has expired, so this cannot loop forever.
  return Ring.size();
}

SmallVector<uint16_t, 4>
ResourceScoreboard::blame(const InstrItineraryData &Itin,
                          ArrayRef<unsigned> SchedClasses, uint64_t At) const {
  SmallVector<uint16_t, 4> Out;
  for (unsigned SC : SchedClasses)
    forEachStageCycle(Itin, SC, [&](unsigned C, uint64_t Unit, unsigned Kind) {
      const Slot *S = peek(At + C);
      if (!S)
        return;
      const bool Hit =
          llvm::is_contained(S->Required, uint16_t(Unit)) ||
          (Kind == InstrStage::Required &&
           llvm::is_contained(S->Reserved, uint16_t(Unit)));
      if (Hit && !llvm::is_contained(Out, uint16_t(Unit)))
        Out.push_back(uint16_t(Unit));
    });
  return Out;
}

void ResourceScoreboard::reserve(const InstrItineraryData &Itin,
                                 ArrayRef<unsigned> SchedClasses, uint64_t At) {
  for (unsigned SC : SchedClasses)
    forEachStageCycle(Itin, SC, [&](unsigned C, uint64_t Unit, unsigned Kind) {
      Slot &S = open(At + C);
      auto &Into = Kind == InstrStage::Required ? S.Required : S.Reserved;
      if (!llvm::is_contained(Into, uint16_t(Unit)))
        Into.push_back(uint16_t(Unit));
    });
}

AIEExecutor::AIEExecutor(const MCDisassembler &DisAsm, const MCInstrInfo &MII,
                         const MCRegisterInfo &MRI, AIESemantics &Sem,
                         AIEHostInterface &Host, uint64_t EntryPoint)
    : DisAsm(DisAsm), MII(MII), MRI(MRI), Sem(Sem), Host(Host),
      State(MRI, EntryPoint, Sem.getSubRegRanges()), Itin(Sem.getItineraries()),
      Hazards(Itin ? ResourceScoreboard::depthFor(*Itin, MII) : 1) {
  const std::array<MCRegister, 3> Loop = Sem.getLoopRegisters();
  LCReg = Loop[0];
  LSReg = Loop[1];
  LEReg = Loop[2];
  LRReg = Sem.getLinkRegister();
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
  // left alone holds an unknown value from here on. jl's link register is the
  // one exception: Eff.Link means the executor will compute and commit it
  // itself once the delay slots retire (advancePC), so it is scheduled, not
  // abandoned, and must not be poisoned here.
  const MCInstrDesc &Desc = MII.get(MI.getOpcode());
  auto PoisonUnwritten = [&](MCRegister Reg) {
    if (Eff.Link && Reg == LRReg)
      return;
    if (!any_of(ArrayRef(Eff.RegWrites).drop_front(FirstWrite),
                [&](const auto &W) { return W.Reg == Reg; }))
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
  for (const SlotEffects::RegWrite &W : Eff.RegWrites)
    State.Regs.write(W.Reg, W.Value, W.VisibleAt);
  llvm::append_range(PendingStores, Eff.MemWrites);
}

bool AIEExecutor::drainStores(bool Final) {
  auto Ready = [&](const SlotEffects::MemWrite &W) {
    return Final || W.SampleAt <= State.Cycle;
  };
  bool Ok = true;
  for (const SlotEffects::MemWrite &W : PendingStores) {
    if (!Ready(W))
      continue;
    APInt V;
    // Read the source at the cycle the store samples it, which is what lets a
    // producer scheduled AFTER the store still supply its data.
    if (!State.Regs.read(W.SrcReg, V, W.SampleAt)) {
      // A fault, not a skip. This used to set the message and carry on, so a
      // store that could not be performed simply did not happen: memory kept
      // whatever was there, a later load read it back, and nothing said so.
      // It means the schedule sampled the source inside its producer's
      // latency window, which a compiler covers and hand-written assembly
      // can get wrong -- so it is exactly the case that needs naming.
      FaultMsg = ("store source " + Twine(MRI.getName(W.SrcReg)) +
                  " holds no value this model has computed")
                     .str();
      Ok = false;
      continue;
    }
    // To the width the store writes, whatever that is. This used to go via
    // 32 bits, which was the scalar datapath showing through: a vector store
    // names a 512-bit source and would have committed only its low word.
    // A fused store narrows on the way out; an ordinary one writes the
    // register's bits. Either way the source was read at SampleAt, so the
    // arithmetic sees the value the pipeline actually presents.
    Host.store(W.Addr, W.NumBytes,
               (W.Narrow ? W.Narrow(V) : V).zextOrTrunc(W.NumBytes * 8));
  }
  llvm::erase_if(PendingStores, Ready);
  return Ok;
}

void AIEExecutor::advancePC(uint64_t BundleAddr, uint64_t BundleSize) {
  State.PC = BundleAddr + BundleSize;
  ++State.RetiredBundles;
  ++State.Cycle;

  if (State.Branch && --State.Branch->BundlesLeft == 0) {
    // State.PC is currently the address right after the delay-slot bundle
    // that just retired -- the natural fallthrough point, reached by
    // accumulating the real encoded size of each delay-slot bundle one
    // step at a time. That is exactly jl's return address, so link is
    // written from it here, before Target overwrites it, rather than
    // computed by peeking ahead at issue time (which bundle sizes had not
    // been fetched yet).
    // Visible AS the branch resolves, not a cycle later: the callee's first
    // bundle is the very next one and reads lr at its own issue + 1, so a
    // later stamp would hide the return address from the instruction the
    // branch just jumped to. (Which is exactly what it did -- `ret` returned
    // to 0 until this was stamped at the resolving bundle.)
    if (State.Branch->Link && LRReg)
      State.Regs.write(LRReg, APInt(64, State.PC), State.Cycle);
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
  // The zero-overhead loop is hardware reading its own registers as the last
  // body bundle retires, not an instruction operand, so it sees whatever has
  // landed by now.
  if (!State.Regs.read(LEReg, LE, State.Cycle) ||
      !State.Regs.read(LCReg, LC, State.Cycle) ||
      !State.Regs.read(LSReg, LS, State.Cycle))
    return;
  if (LC.isZero() || LE.getZExtValue() != BundleAddr)
    return;
  LC -= 1;
  State.Regs.write(LCReg, LC, State.Cycle);
  if (!LC.isZero())
    State.PC = LS.getZExtValue();
}

const AIEExecutor::Bundle *AIEExecutor::decode(uint64_t Addr) {
  auto It = Decoded.find(Addr);
  if (It != Decoded.end())
    return &It->second;

  ArrayRef<uint8_t> Bytes = Host.fetch(Addr);
  if (Bytes.empty()) {
    FaultMsg = ("no program memory at " + Twine::utohexstr(Addr)).str();
    return nullptr;
  }

  Bundle B;
  if (DisAsm.getInstruction(B.Inst, B.Size, Bytes, Addr, nulls()) !=
      MCDisassembler::Success) {
    FaultMsg = ("undecodable bundle at " + Twine::utohexstr(Addr)).str();
    return nullptr;
  }
  return &Decoded.try_emplace(Addr, std::move(B)).first->second;
}

void AIEExecutor::schedClassesOf(const MCInst &Bundle,
                                 SmallVectorImpl<unsigned> &Out) const {
  Out.clear();
  if (any_of(Bundle, [](const MCOperand &Op) { return Op.isInst(); })) {
    for (const MCOperand &Op : Bundle)
      if (Op.isInst())
        Out.push_back(MII.get(Op.getInst()->getOpcode()).getSchedClass());
    return;
  }
  Out.push_back(MII.get(Bundle.getOpcode()).getSchedClass());
}

StepResult AIEExecutor::step() {
  const uint64_t BundleAddr = State.PC;
  const Bundle *B = decode(BundleAddr);
  if (!B)
    return StepResult::Fault;
  const MCInst &Bundle = B->Inst;
  const uint64_t Size = B->Size;

  // A structural hazard is the core holding itself back, so time passes here
  // rather than being reported to the embedder: nothing external can resolve
  // it, unlike the port stall StepResult::Stalled means. The clock moves before
  // anything reads it, so the bundle's operand cycles and any store it defers
  // are all denominated in the cycle it really issued.
  if (Itin) {
    schedClassesOf(Bundle, SchedClasses);
    const unsigned Wait = Hazards.delay(*Itin, SchedClasses, State.Cycle);
    // A stall count on its own does not say WHICH resource, and that is the
    // question every use of it starts with -- the first measurement over
    // compiled aievec kernels came back as one unit family and would have read
    // as noise without this.
    if (Wait && getenv("AIE_SIM_DEBUG_HAZARD")) {
      errs() << "HAZARD wait=" << Wait << " pc=0x"
             << Twine::utohexstr(BundleAddr) << " units";
      for (uint16_t U : Hazards.blame(*Itin, SchedClasses, State.Cycle))
        errs() << " " << U;
      errs() << "\n";
    }
    State.Cycle += Wait;
    State.StallCycles += Wait;
  }

  // Stores whose source cycle has arrived commit before this bundle runs.
  if (!drainStores())
    return StepResult::Fault;

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
  if (Itin)
    Hazards.reserve(*Itin, SchedClasses, State.Cycle);
  if (Done) {
    // The program is over; stores still waiting on their source do land, and
    // the values they want were produced by instructions that already ran.
    if (!drainStores(/*Final=*/true))
      return StepResult::Fault;
    return StepResult::Done;
  }

  if (Eff.Branch && State.Branch) {
    FaultMsg = "control transfer issued inside another one's delay slots";
    return StepResult::Fault;
  }
  advancePC(BundleAddr, Size);
  if (Eff.Branch)
    State.Branch = {*Eff.Branch, NumDelaySlots, Eff.Link};
  return StepResult::Retired;
}
