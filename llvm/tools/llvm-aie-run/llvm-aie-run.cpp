//===- llvm-aie-run.cpp - Run an AIE object file --------------------------===//
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
/// Loads an AIE object file into the target's MCSimulator, executes it, and
/// reports registers, memory and instruction coverage.
//
//===----------------------------------------------------------------------===//

#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSimulator.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/WithColor.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/TargetParser/Triple.h"
#include <map>

using namespace llvm;

static cl::OptionCategory Cat("llvm-aie-run options");

static cl::opt<std::string> InputFile(cl::Positional, cl::Required,
                                      cl::desc("<object file>"), cl::cat(Cat));
static cl::opt<std::string> TripleName("triple", cl::desc("Target triple"),
                                       cl::init("aie2p"), cl::cat(Cat));
static cl::opt<std::string> EntrySymbol("entry",
                                        cl::desc("Symbol to start from"),
                                        cl::init("_start"), cl::cat(Cat));
static cl::opt<unsigned long long>
    MaxBundles("max-bundles", cl::desc("Give up after this many bundles"),
               cl::init(1u << 20), cl::cat(Cat));
static cl::opt<bool> PrintRegs("print-regs",
                               cl::desc("Dump the non-zero registers on exit"),
                               cl::cat(Cat));
static cl::list<std::string>
    DumpMem("dump-mem", cl::desc("Dump <addr>:<bytes> of data memory on exit"),
            cl::cat(Cat));
static cl::list<std::string>
    Scratch("scratch", cl::desc("Map <addr>:<bytes> of zeroed data memory"),
            cl::cat(Cat));
static cl::opt<bool> Coverage("coverage",
                              cl::desc("Report the opcodes this run reached"),
                              cl::cat(Cat));

namespace {

[[noreturn]] void fail(const Twine &Msg) {
  WithColor::error(errs(), "llvm-aie-run") << Msg << '\n';
  exit(1);
}

std::pair<uint64_t, uint64_t> parseRange(StringRef Spec, StringRef Option) {
  auto [AddrStr, SizeStr] = Spec.split(':');
  uint64_t Addr, Size;
  if (AddrStr.getAsInteger(0, Addr) || SizeStr.getAsInteger(0, Size))
    fail("bad --" + Option + " range: " + Spec);
  return {Addr, Size};
}

} // namespace

int main(int argc, char **argv) {
  InitLLVM X(argc, argv);
  InitializeAllTargetInfos();
  InitializeAllTargetMCs();
  InitializeAllDisassemblers();
  InitializeAllTargetSims();
  cl::HideUnrelatedOptions(Cat);
  cl::ParseCommandLineOptions(argc, argv, "AIE instruction simulator\n");

  std::string Error;
  const std::string TripleStr = Triple::normalize(TripleName);
  Triple TT(TripleStr);
  const Target *TheTarget = TargetRegistry::lookupTarget(TT, Error);
  if (!TheTarget)
    fail(Error);

  std::unique_ptr<const MCRegisterInfo> MRI(
      TheTarget->createMCRegInfo(TripleStr));
  std::unique_ptr<const MCInstrInfo> MII(TheTarget->createMCInstrInfo());
  // AIE names each processor after its architecture. An empty CPU would leave
  // the subtarget without a scheduling model, hence without latencies.
  StringRef CPU = Triple::getArchTypeName(TT.getArch());
  std::unique_ptr<const MCSubtargetInfo> STI(
      TheTarget->createMCSubtargetInfo(TripleStr, CPU, ""));
  if (!MRI || !MII || !STI)
    fail("no MC layer for " + TT.str());

  MCContext Ctx(TT, nullptr, MRI.get(), STI.get());
  std::unique_ptr<MCDisassembler> DisAsm(
      TheTarget->createMCDisassembler(*STI, Ctx));
  if (!DisAsm)
    fail("no disassembler for " + TT.str());

  std::unique_ptr<MCSimulator> Sim(
      TheTarget->createMCSimulator(*STI, *MII, *MRI, *DisAsm, outs()));
  if (!Sim)
    fail("no instruction simulator for " + TT.str());

  ErrorOr<std::unique_ptr<MemoryBuffer>> Buf = MemoryBuffer::getFile(InputFile);
  if (!Buf)
    fail(InputFile + ": " + Buf.getError().message());
  Expected<std::unique_ptr<object::ObjectFile>> ObjOrErr =
      object::ObjectFile::createObjectFile(Buf.get()->getMemBufferRef());
  if (!ObjOrErr)
    fail(toString(ObjOrErr.takeError()));
  object::ObjectFile &Obj = **ObjOrErr;

  // Sections are loaded where they say they live, so an object still carrying
  // relocations is not runnable: every allocatable section sits at 0, on top
  // of the others, and the branch and loop-bound fields the relocations would
  // have filled hold zero. That runs -- straight to the bundle cap, or into a
  // decoded field of zeros -- and looks like a hang rather than a load error.
  // Hand-written straight-line tests relocate nothing and are unaffected;
  // compiler output for anything with a loop always does, because AIE spells
  // branch targets and loop bounds as absolute addresses.
  for (const object::SectionRef &Sec : Obj.sections()) {
    if (Sec.relocations().empty())
      continue;
    Expected<object::section_iterator> Relocated = Sec.getRelocatedSection();
    if (!Relocated)
      fail(toString(Relocated.takeError()));
    object::SectionRef Target =
        *Relocated == Obj.section_end() ? Sec : **Relocated;
    if (!Target.isText() && !Target.isData())
      continue;
    Expected<StringRef> Name = Target.getName();
    fail("unapplied relocations in " +
         (Name ? *Name : StringRef("a loaded section")) +
         ": link this object before running it");
  }

  for (const object::SectionRef &Sec : Obj.sections()) {
    Expected<StringRef> Contents = Sec.getContents();
    if (!Contents)
      fail(toString(Contents.takeError()));
    if (Sec.isBSS())
      Sim->mapZeroedMemory(Sec.getAddress(), Sec.getSize());
    else if (Sec.isText() || Sec.isData())
      Sim->mapMemory(
          Sec.getAddress(),
          ArrayRef(reinterpret_cast<const uint8_t *>(Contents->data()),
                   Contents->size()));
  }

  for (StringRef Spec : Scratch) {
    auto [Addr, Size] = parseRange(Spec, "scratch");
    Sim->mapZeroedMemory(Addr, Size);
  }

  std::optional<uint64_t> Entry;
  for (const object::SymbolRef &Sym : Obj.symbols()) {
    Expected<StringRef> Name = Sym.getName();
    Expected<uint64_t> Addr = Sym.getAddress();
    if (Name && Addr && *Name == EntrySymbol)
      Entry = *Addr;
  }
  if (!Entry)
    fail("no symbol named " + EntrySymbol);

  MCSimulator::Status R = Sim->run(*Entry, MaxBundles);
  if (R == MCSimulator::Status::Stalled)
    fail("stalled with no way to make progress: this embedder has no ports");
  if (R == MCSimulator::Status::Fault) {
    WithColor::error(errs(), "llvm-aie-run")
        << "fault at 0x" << Twine::utohexstr(Sim->getPC()) << ": "
        << Sim->getFaultMessage() << '\n';
  } else if (R != MCSimulator::Status::Done) {
    WithColor::error(errs(), "llvm-aie-run")
        << "did not finish within " << MaxBundles << " bundles\n";
  }

  Sim->printStatistics(outs());
  if (PrintRegs)
    Sim->printRegisters(outs());
  for (StringRef Spec : DumpMem) {
    auto [Addr, Size] = parseRange(Spec, "dump-mem");
    outs() << "mem[0x" << Twine::utohexstr(Addr) << "] =";
    for (uint8_t B : Sim->readMemory(Addr, Size))
      outs() << format(" %02x", B);
    outs() << '\n';
  }
  if (Coverage) {
    SmallVector<std::pair<unsigned, bool>> Reached;
    Sim->getCoverage(Reached);
    std::map<StringRef, bool> ByName;
    for (auto [Opc, Modelled] : Reached)
      ByName[MII->getName(Opc)] = Modelled;
    for (const auto &[Name, Modelled] : ByName)
      outs() << (Modelled ? "modelled   " : "unmodelled ") << Name << '\n';
  }

  return R == MCSimulator::Status::Done ? 0 : 1;
}
