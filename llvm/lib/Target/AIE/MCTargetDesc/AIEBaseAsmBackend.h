//===-- AIEBaseAsmBackend.h - AIE Assembler Backend -----------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEBASEASMBACKEND_H
#define LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEBASEASMBACKEND_H

#include "MCTargetDesc/AIEMCFixupKinds.h"
#include "MCTargetDesc/AIEMCTargetDesc.h"
#include "Utils/AIEBaseInfo.h"
#include "llvm/ADT/bit.h"
#include "llvm/MC/MCAsmBackend.h"

namespace llvm {
class MCAssembler;
class MCSubTargetInfo;
class MCObjectTargetWriter;
class raw_ostream;

class AIEBaseAsmBackend : public MCAsmBackend {
protected:
  const MCSubtargetInfo &STI;
  uint8_t OSABI;
  const MCTargetOptions &TargetOptions;
  AIEABI::ABI TargetABI = AIEABI::ABI_VITIS;
  /// DelaySlot and NeedsAlign are set in conjunction.
  /// Delayslot counts down from the delay slot holder, and will assert that
  /// delay slots don't contain other delay slot holders.
  /// NeedsAlign is encoding an alignment request for the end of the last
  /// delay slot, effectively saying whether the delay slot holder was a call
  /// instruction, which needs alignment of the return address. Note that the
  /// alignment of branch targets is done separately at the start of each block
  int DelaySlot = 0;
  bool NeedsAlign = false;

public:
  AIEBaseAsmBackend(const MCSubtargetInfo &STI, uint8_t OSABI,
                    const MCTargetOptions &Options)
      : MCAsmBackend(endianness::little), STI(STI), OSABI(OSABI),
        TargetOptions(Options) {
    (void)STI;
  }
  virtual ~AIEBaseAsmBackend() override {}

  void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved,
                  const MCSubtargetInfo *STI) const override;

  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override;

  void emitInstructionEnd(MCObjectStreamer &OS, const MCInst &Inst) override;

  bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                            const MCRelaxableFragment *DF,
                            const MCAsmLayout &Layout) const override {
    // All fixups are symbolic references that don't change by relaxation,
    // which only adds nop slots.
    return false;
  }

  const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const override {
    // If the kind is a base LLVM Fixup
    if (!AIEMCFixupKinds::isTargetFixup(Kind))
      return MCAsmBackend::getFixupKindInfo(Kind);

    // Otherwise the Kind is target specific. In AIE, MCFixupKindInfo is not
    // sufficient to fully describe a fixup (which targets possibly multiple
    // fields). Thus, this method must not be used to retrieve the fixup's
    // semantic. Instead, we rely on the FixupField structure to describe each
    // independent patch.

    // getFixupKindInfo() is called by the MCAssembler when evaluating the
    // fixups, even though they will not be relaxed. This is why we can't put
    // an unreachable here and we always return a Dummy value with a "0" flag.
    static MCFixupKindInfo Dummy = {"", 0, 0, 0};
    return Dummy;
  }

  bool mayNeedRelaxation(const MCInst &Inst,
                         const MCSubtargetInfo &STI) const override;

  void relaxInstruction(MCInst &Inst,
                        const MCSubtargetInfo &STI) const override = 0;

  bool relaxPerInstruction() const override { return true; }
  unsigned maxRelaxIncrement(const MCInst &Inst,
                             const MCSubtargetInfo &STI) const override = 0;

  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override = 0;

  const MCTargetOptions &getTargetOptions() const { return TargetOptions; }
  AIEABI::ABI getTargetABI() const { return TargetABI; }

  virtual bool isCall(unsigned Opcode) const = 0;
  virtual bool isDelaySlotInstr(unsigned Opcode) const = 0;

  /// Check if the given MCInst has a delay slot instruction, either as itself
  /// or as a sub-instruction in a bundle; @param IsCallInstr Set to **true** if
  /// the instruction with the delay slots is a call.
  bool hasDelaySlotInstr(const MCInst &Inst, bool &IsCallInstr,
                         const Triple &TargetTriple) const;
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEBASEASMBACKEND_H
