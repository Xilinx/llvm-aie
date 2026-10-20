//===- AIESimulator.cpp - MCSimulator for a single AIE core ---------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
/// \file
/// A core over flat memory. The ports that a real tile provides - locks,
/// streams, cascade - are absent here and refuse access, so a design that
/// needs them says so instead of appearing to run.
//
//===----------------------------------------------------------------------===//

#include "AIEExecutor.h"
#include "TargetInfo/AIETargetInfo.h"
#include "llvm/MC/MCSimulator.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/raw_ostream.h"
#include <optional>

using namespace llvm;
using namespace llvm::AIESim;

namespace {

/// Flat memory with no locks, streams or cascade.
class FlatMemory : public AIEHostInterface {
public:
  explicit FlatMemory(raw_ostream &Out) : Out(Out) {}

  void map(uint64_t Addr, ArrayRef<uint8_t> Data) {
    if (Bytes.size() < Addr + Data.size())
      Bytes.resize(Addr + Data.size(), 0);
    Mapped.resize(Bytes.size(), false);
    llvm::copy(Data, Bytes.begin() + Addr);
    std::fill(Mapped.begin() + Addr, Mapped.begin() + Addr + Data.size(), true);
  }

  void mapZeroed(uint64_t Addr, uint64_t Size) {
    if (Bytes.size() < Addr + Size) {
      Bytes.resize(Addr + Size, 0);
      Mapped.resize(Bytes.size(), false);
    }
    std::fill(Mapped.begin() + Addr, Mapped.begin() + Addr + Size, true);
  }

  ArrayRef<uint8_t> fetch(uint64_t Addr) override {
    if (Addr >= Bytes.size() || !Mapped[Addr])
      return {};
    return ArrayRef(Bytes).drop_front(Addr);
  }

  PortStatus load(uint64_t Addr, unsigned NumBytes, APInt &Value) override {
    if (!isMapped(Addr, NumBytes))
      return PortStatus::Fault;
    Value = APInt(NumBytes * 8, 0);
    for (unsigned I = 0; I != NumBytes; ++I)
      Value |= APInt(NumBytes * 8, Bytes[Addr + I]) << (8 * I);
    return PortStatus::Ok;
  }

  PortStatus store(uint64_t Addr, unsigned NumBytes,
                   const APInt &Value) override {
    if (!isMapped(Addr, NumBytes))
      return PortStatus::Fault;
    for (unsigned I = 0; I != NumBytes; ++I)
      Bytes[Addr + I] = Value.extractBitsAsZExtValue(8, 8 * I);
    return PortStatus::Ok;
  }

  void putChar(char C) override { Out << C; }

  void raiseEvent(unsigned Id) override { Out << "event " << Id << "\n"; }

  ArrayRef<uint8_t> range(uint64_t Addr, uint64_t Size) const {
    if (Addr + Size > Bytes.size())
      return {};
    return ArrayRef(Bytes).slice(Addr, Size);
  }

private:
  bool isMapped(uint64_t Addr, uint64_t Size) const {
    if (Addr + Size > Mapped.size())
      return false;
    return std::all_of(Mapped.begin() + Addr, Mapped.begin() + Addr + Size,
                       [](bool B) { return B; });
  }

  raw_ostream &Out;
  std::vector<uint8_t> Bytes;
  std::vector<bool> Mapped;
};

class AIESimulator : public MCSimulator {
public:
  AIESimulator(const MCInstrInfo &MII, const MCRegisterInfo &MRI,
               const MCDisassembler &DisAsm, std::unique_ptr<AIESemantics> Sem,
               raw_ostream &Out)
      : MII(MII), MRI(MRI), DisAsm(DisAsm), Sem(std::move(Sem)), Mem(Out) {}

  void mapMemory(uint64_t Addr, ArrayRef<uint8_t> Bytes) override {
    Mem.map(Addr, Bytes);
  }
  void mapZeroedMemory(uint64_t Addr, uint64_t Size) override {
    Mem.mapZeroed(Addr, Size);
  }
  ArrayRef<uint8_t> readMemory(uint64_t Addr, uint64_t Size) const override {
    return Mem.range(Addr, Size);
  }

  Status run(uint64_t Entry, uint64_t MaxSteps) override {
    Exec.emplace(DisAsm, MII, MRI, *Sem, Mem, Entry);
    for (uint64_t Steps = 0; Steps != MaxSteps; ++Steps) {
      switch (Exec->step()) {
      case StepResult::Retired:
        break;
      case StepResult::Done:
        return Status::Done;
      case StepResult::Fault:
        return Status::Fault;
      case StepResult::Stalled:
        return Status::Stalled;
      }
    }
    return Status::StepLimit;
  }

  uint64_t getPC() const override { return Exec ? Exec->getState().PC : 0; }
  StringRef getFaultMessage() const override {
    return Exec ? Exec->getFaultMessage() : StringRef();
  }

  void printStatistics(raw_ostream &OS) const override {
    if (!Exec)
      return;
    const AIECoreState &State = Exec->getState();
    OS << "bundles: " << State.RetiredBundles << '\n';
    // Cycles exceed bundles by exactly what structural hazards cost, which is
    // the per-op occupancy a bundle count cannot show.
    OS << "cycles: " << State.Cycle << '\n';
    OS << "stall-cycles: " << State.StallCycles << '\n';
  }

  void printRegisters(raw_ostream &OS) const override {
    if (Exec)
      Exec->getState().Regs.print(OS);
  }

  void
  getCoverage(SmallVectorImpl<std::pair<unsigned, bool>> &Out) const override {
    if (!Exec)
      return;
    for (unsigned Opc : Exec->getExecutedOpcodes())
      Out.push_back({Opc, !Exec->getUnmodelledOpcodes().count(Opc)});
  }

private:
  const MCInstrInfo &MII;
  const MCRegisterInfo &MRI;
  const MCDisassembler &DisAsm;
  std::unique_ptr<AIESemantics> Sem;
  FlatMemory Mem;
  std::optional<AIEExecutor> Exec;
};

MCSimulator *createAIESimulator(const MCSubtargetInfo &STI,
                                const MCInstrInfo &MII,
                                const MCRegisterInfo &MRI,
                                const MCDisassembler &DisAsm,
                                raw_ostream &Out) {
  std::unique_ptr<AIESemantics> Sem = createSemantics(STI, MII, MRI);
  if (!Sem)
    return nullptr;
  return new AIESimulator(MII, MRI, DisAsm, std::move(Sem), Out);
}

} // namespace

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeAIETargetSim() {
  TargetRegistry::RegisterMCSimulator(getTheAIE2PTarget(), createAIESimulator);
}
