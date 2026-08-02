//===- AIEISSCAbi.cpp - The C ABI a loadable AIE core engine exports ------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
/// \file
/// Exports the executor through `aie_iss_c_abi.h` so an embedder that has no
/// LLVM headers can drive it. A Peano distribution ships libLLVM.so and no
/// headers, and mlir-aie is separately built and separately versioned, so the
/// boundary has to be C resolved at run time rather than a C++ link.
///
/// Everything outside the datapath arrives as a host callback, so this file
/// holds no memory of its own: program bytes, data, locks, streams and the
/// cascade all belong to the embedder.
//
//===----------------------------------------------------------------------===//

#include "aie_iss_c_abi.h"

#include "AIEExecutor.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/TargetParser/Triple.h"

#include <memory>
#include <mutex>
#include <string>

using namespace llvm;
using namespace llvm::AIESim;

namespace {

/// The longest bundle the AIE2P size table encodes. `fetch` degrades from here
/// because the embedder maps program memory exactly, so a bundle at the end of
/// a section has fewer than this many bytes behind it.
constexpr unsigned MaxBundleBytes = 16;

/// Adapts the embedder's C callbacks to the executor's port.
///
/// Refusal is preserved rather than smoothed over: a callback returning 0 means
/// unmapped for memory (a fault) and not-ready for locks, streams and the
/// cascade (a stall). Collapsing the two would let a design that deadlocks look
/// like one that faults, and vice versa.
class CallbackHost final : public AIEHostInterface {
public:
  explicit CallbackHost(const aie_iss_host_callbacks &CB) : CB(CB) {}

  ArrayRef<uint8_t> fetch(uint64_t Addr) override {
    if (Addr > UINT32_MAX)
      return {};
    // Ask for the longest bundle first and shrink until the embedder accepts
    // one, so a short final bundle still decodes. Sizes are even, per the
    // encoding's 2-byte granularity.
    for (unsigned Size = MaxBundleBytes; Size >= 2; Size -= 2)
      if (CB.read(CB.ctx, static_cast<uint32_t>(Addr), FetchBuf, Size))
        return ArrayRef(FetchBuf, Size);
    return {};
  }

  PortStatus load(uint64_t Addr, unsigned NumBytes, APInt &Out) override {
    if (Addr > UINT32_MAX || NumBytes > sizeof(DataBuf))
      return PortStatus::Fault;
    if (!CB.read(CB.ctx, static_cast<uint32_t>(Addr), DataBuf, NumBytes))
      return PortStatus::Fault;
    Out = APInt(NumBytes * 8, 0);
    for (unsigned I = 0; I != NumBytes; ++I)
      Out |= APInt(NumBytes * 8, DataBuf[I]) << (8 * I);
    return PortStatus::Ok;
  }

  PortStatus store(uint64_t Addr, unsigned NumBytes,
                   const APInt &Value) override {
    if (Addr > UINT32_MAX || NumBytes > sizeof(DataBuf))
      return PortStatus::Fault;
    for (unsigned I = 0; I != NumBytes; ++I)
      DataBuf[I] = Value.extractBitsAsZExtValue(8, 8 * I);
    return CB.write(CB.ctx, static_cast<uint32_t>(Addr), DataBuf, NumBytes)
               ? PortStatus::Ok
               : PortStatus::Fault;
  }

  PortStatus acquireLock(unsigned Id, int32_t Value) override {
    return CB.try_acquire_lock(CB.ctx, Id, Value) ? PortStatus::Ok
                                                  : PortStatus::Stall;
  }

  PortStatus releaseLock(unsigned Id, int32_t Value) override {
    CB.release_lock(CB.ctx, Id, Value);
    return PortStatus::Ok;
  }

  PortStatus readStream(unsigned Port, APInt &Out) override {
    uint32_t Word = 0;
    int TLast = 0;
    if (!CB.try_read_stream(CB.ctx, Port, &Word, &TLast))
      return PortStatus::Stall;
    Out = APInt(32, Word);
    return PortStatus::Ok;
  }

  PortStatus writeStream(unsigned Port, const APInt &Value,
                         bool TLast) override {
    return CB.try_write_stream(CB.ctx, Port, Value.getZExtValue(), TLast)
               ? PortStatus::Ok
               : PortStatus::Stall;
  }

  PortStatus readCascade(APInt &Out) override {
    uint8_t Buf[64] = {};
    if (!CB.try_read_cascade(CB.ctx, Buf))
      return PortStatus::Stall;
    Out = APInt(512, 0);
    for (unsigned I = 0; I != sizeof(Buf); ++I)
      Out |= APInt(512, Buf[I]) << (8 * I);
    return PortStatus::Ok;
  }

  PortStatus writeCascade(const APInt &Value) override {
    uint8_t Buf[64] = {};
    for (unsigned I = 0; I != sizeof(Buf); ++I)
      Buf[I] = Value.extractBitsAsZExtValue(8, 8 * I);
    return CB.try_write_cascade(CB.ctx, Buf) ? PortStatus::Ok
                                             : PortStatus::Stall;
  }

  void putChar(char C) override { CB.put_char(CB.ctx, C); }

private:
  aie_iss_host_callbacks CB;
  uint8_t FetchBuf[MaxBundleBytes] = {};
  uint8_t DataBuf[64] = {};
};

/// Maps an ABI ISA selector to the triple whose MC layer decodes it.
const char *tripleForIsa(int Isa) {
  switch (Isa) {
  case AIE_ISS_ISA_AIE2:
    return "aie2";
  case AIE_ISS_ISA_AIE2P:
    return "aie2p";
  case AIE_ISS_ISA_AIE2PS:
    return "aie2ps";
  default:
    return nullptr;
  }
}

/// Registers the AIE target once per process. The embedder dlopens this
/// library and never calls an LLVM initializer itself.
void ensureTargetRegistered() {
  static std::once_flag Once;
  std::call_once(Once, [] {
    InitializeAllTargetInfos();
    InitializeAllTargetMCs();
    InitializeAllDisassemblers();
  });
}

} // namespace

/// One core, and everything the MC layer needs to decode for it.
///
/// The MC objects are per-core rather than shared because MCContext owns the
/// decoded sub-instructions the executor caches, and a shared context would
/// make one core's lifetime depend on another's.
struct aie_iss_core {
  std::unique_ptr<const MCRegisterInfo> MRI;
  std::unique_ptr<const MCInstrInfo> MII;
  std::unique_ptr<const MCSubtargetInfo> STI;
  std::unique_ptr<MCContext> Ctx;
  std::unique_ptr<MCDisassembler> DisAsm;
  std::unique_ptr<AIESemantics> Sem;
  std::unique_ptr<CallbackHost> Host;
  std::unique_ptr<AIEExecutor> Exec;
  StringMap<MCRegister> RegsByName;
  /// Backs the `error` pointer, which the ABI keeps valid until the next call
  /// on this core.
  std::string Error;

  /// Rebuilds the executor at \p PC. Also used by reset(), where dropping the
  /// decode cache is required and not merely tidy: the embedder may have
  /// rewritten program memory while the core was disabled.
  void restart(uint64_t PC) {
    Exec = std::make_unique<AIEExecutor>(*DisAsm, *MII, *MRI, *Sem, *Host, PC);
  }
};

namespace {

/// Copies \p Value into \p Data little-endian, zero-extending or truncating to
/// \p Size. The core-debug register window reads 32 bits at a time regardless
/// of the register's own width, so both directions are ordinary.
void copyOut(const APInt &Value, void *Data, uint32_t Size) {
  auto *Out = static_cast<uint8_t *>(Data);
  for (uint32_t I = 0; I != Size; ++I)
    Out[I] = I * 8 < Value.getBitWidth()
                 ? Value.extractBitsAsZExtValue(
                       std::min<unsigned>(8, Value.getBitWidth() - I * 8), I * 8)
                 : 0;
}

const char *engineName() { return "peano-iss " LLVM_VERSION_STRING; }

int supportsIsa(int Isa) {
  // Semantics, not decode, is the scarce half: createSemantics answers for
  // AIE2P alone today.
  return Isa == AIE_ISS_ISA_AIE2P;
}

aie_iss_core *createCore(int Isa, const aie_iss_host_callbacks *Callbacks) {
  if (!Callbacks || Callbacks->size < sizeof(aie_iss_host_callbacks))
    return nullptr;
  const char *TripleName = tripleForIsa(Isa);
  if (!TripleName)
    return nullptr;

  ensureTargetRegistered();

  auto Core = std::make_unique<aie_iss_core>();
  std::string Err;
  const std::string TripleStr = Triple::normalize(TripleName);
  Triple TT(TripleStr);
  const Target *TheTarget = TargetRegistry::lookupTarget(TT, Err);
  if (!TheTarget)
    return nullptr;

  Core->MRI.reset(TheTarget->createMCRegInfo(TripleStr));
  Core->MII.reset(TheTarget->createMCInstrInfo());
  Core->STI.reset(
      TheTarget->createMCSubtargetInfo(TripleStr, cpuForTriple(TT), ""));
  if (!Core->MRI || !Core->MII || !Core->STI)
    return nullptr;

  Core->Ctx = std::make_unique<MCContext>(TT, nullptr, Core->MRI.get(),
                                          Core->STI.get());
  Core->DisAsm.reset(TheTarget->createMCDisassembler(*Core->STI, *Core->Ctx));
  if (!Core->DisAsm)
    return nullptr;

  Core->Sem = createSemantics(*Core->STI, *Core->MII, *Core->MRI);
  if (!Core->Sem)
    return nullptr;

  // The ABI names registers the way the assembly does; MCRegisterInfo only
  // maps the other way, so invert it once here.
  for (unsigned R = 1, E = Core->MRI->getNumRegs(); R != E; ++R)
    Core->RegsByName.insert({Core->MRI->getName(R), MCRegister(R)});

  Core->Host = std::make_unique<CallbackHost>(*Callbacks);
  Core->restart(0);
  return Core.release();
}

void destroyCore(aie_iss_core *Core) { delete Core; }

void resetCore(aie_iss_core *Core) {
  Core->Error.clear();
  Core->restart(0);
}

void setPC(aie_iss_core *Core, uint32_t PC) { Core->Exec->getState().PC = PC; }

uint32_t getPC(const aie_iss_core *Core) {
  return static_cast<uint32_t>(Core->Exec->getState().PC);
}

int stepCore(aie_iss_core *Core) {
  switch (Core->Exec->step()) {
  case StepResult::Retired:
    return AIE_ISS_RETIRED;
  case StepResult::Stalled:
    return AIE_ISS_STALLED;
  case StepResult::Done:
    return AIE_ISS_DONE;
  case StepResult::Fault:
    Core->Error = Core->Exec->getFaultMessage().str();
    return AIE_ISS_FAULT;
  }
  Core->Error = "unknown step result";
  return AIE_ISS_FAULT;
}

const char *coreError(const aie_iss_core *Core) { return Core->Error.c_str(); }

int readRegister(const aie_iss_core *Core, const char *Name, void *Data,
                 uint32_t Size) {
  if (!Name || !Data)
    return 0;
  auto It = Core->RegsByName.find(Name);
  if (It == Core->RegsByName.end())
    return 0;
  APInt Value;
  if (!Core->Exec->getState().Regs.read(It->second, Value))
    return 0;
  copyOut(Value, Data, Size);
  return 1;
}

int writeRegister(aie_iss_core *Core, const char *Name, const void *Data,
                  uint32_t Size) {
  if (!Name || !Data)
    return 0;
  auto It = Core->RegsByName.find(Name);
  if (It == Core->RegsByName.end())
    return 0;
  unsigned Width = Core->Exec->getState().Regs.getWidth(It->second);
  if (Width == 0)
    return 0;
  const auto *In = static_cast<const uint8_t *>(Data);
  APInt Value(Width, 0);
  for (uint32_t I = 0; I != Size && I * 8 < Width; ++I)
    Value |= APInt(Width, In[I]) << (8 * I);
  return Core->Exec->getState().Regs.write(It->second, Value);
}

uint32_t opcodeCoverage(const aie_iss_core *Core, void *Ctx,
                        void (*Sink)(void *, const char *, int)) {
  if (!Core || !Core->Exec || !Sink)
    return 0;
  // Reported in opcode order rather than DenseSet iteration order: the caller
  // puts this in a record that has to be byte-stable across runs.
  const auto &Executed = Core->Exec->getExecutedOpcodes();
  const auto &Unmodelled = Core->Exec->getUnmodelledOpcodes();
  SmallVector<unsigned, 64> Opcodes(Executed.begin(), Executed.end());
  llvm::sort(Opcodes);
  for (unsigned Opcode : Opcodes)
    Sink(Ctx, Core->MII->getName(Opcode).data(),
         Unmodelled.contains(Opcode) ? 0 : 1);
  return Opcodes.size();
}

const aie_iss_api Api = {
    sizeof(aie_iss_api),
    AIE_ISS_ABI_VERSION,
    engineName,
    supportsIsa,
    createCore,
    destroyCore,
    resetCore,
    setPC,
    getPC,
    stepCore,
    coreError,
    readRegister,
    writeRegister,
    opcodeCoverage,
};

} // namespace

// Not LLVM_EXTERNAL_VISIBILITY: that expands to nothing unless LLVM itself was
// configured as a dylib, which would leave this hidden in an ordinary static
// build and fail at the embedder's dlsym rather than here.
extern "C" LLVM_ATTRIBUTE_VISIBILITY_DEFAULT const aie_iss_api *
aie_iss_get_api(uint32_t RequestedAbiVersion) {
  // One version today. A future engine that serves several would select here,
  // which is why the requested version is a parameter rather than a check the
  // embedder makes on the returned struct.
  if (RequestedAbiVersion != AIE_ISS_ABI_VERSION)
    return nullptr;
  return &Api;
}
