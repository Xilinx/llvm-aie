//===-- AIEMCCodeEmitter.cpp - Convert AIE code to machine code -------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file supplies generic AIEMCCodeEmitter utilities.
//
//===----------------------------------------------------------------------===//

#include "AIEMCFixupKinds.h"
#include "AIEMCFormats.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCInst.h"

#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/Casting.h"

namespace llvm {

class AIEBaseMCCodeEmitter : public MCCodeEmitter {
  AIEBaseMCCodeEmitter(const AIEBaseMCCodeEmitter &) = delete;
  void operator=(const AIEBaseMCCodeEmitter &) = delete;

protected:
  MCContext &Ctx;
  MCInstrInfo const &MCII;

  std::unique_ptr<const AIEMCFixupKinds> MCFixupKinds;

  const AIEBaseMCFormats &FormatInterface;

public:
  AIEBaseMCCodeEmitter(MCContext &ctx, MCInstrInfo const &MCII,
                       std::unique_ptr<const AIEMCFixupKinds> MCFixupKinds,
                       const AIEBaseMCFormats &FormatInterface)
      : Ctx(ctx), MCII(MCII), MCFixupKinds(MCFixupKinds.release()),
        FormatInterface(FormatInterface) {}

  ~AIEBaseMCCodeEmitter() override {}

  void encodeInstruction(const MCInst &MI, SmallVectorImpl<char> &CB,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;

  virtual void getBinaryCodeForInstr(const MCInst &MI,
                                     SmallVectorImpl<MCFixup> &Fixups,
                                     APInt &Inst, APInt &Scratch,
                                     const MCSubtargetInfo &STI) const = 0;
  void getMachineOpValue(const MCInst &MI, const MCOperand &MO, APInt &op,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const;

  /// Return a modified version of the BaseFixups container, where the Fixups
  /// offsets have been translated in the VLIW/Composite scope (previously
  /// expressed in the standalone instruction scope). As a side-effect, the
  /// BaseFixups container is cleared. The transformation performed is given
  /// below (FFOS => FOCE):
  ///
  /// A typical AIE Standalone instruction encoding looks like:
  ///         ___________________Slot______________________
  ///        |      __________Instruction_____________     |
  /// MSB    |     |         ____FixupField___        |    |    LSB
  ///  |-----|-----|--------|-----------------|-------|----|-----|
  ///  0 => SOSI            |                               InstrSize*8-1
  ///    ====|==========> FFOS
  ///        |<============>|
  ///              FFRS
  /// with:
  /// - SOSI: Slot offset in the Standalone Instruction
  /// - FFOS: Fixup Field Offset in the Standalone instruction
  /// - FFRS: Fixup Field relative to the Slot position = FFOS - SOSI
  ///
  /// An example of an AIE Composite instruction where the transformation might
  /// occur:
  ///                               ___________Slot___________
  ///                              |     ___Instruction____   |
  /// MSB   ____OtherSlot___       |    |     ___FP___     |  |  LSB
  ///  |---|----------------|------|----|----|--------|----|--|---|
  ///  0 =======================> SOCE       |                    |
  ///                              |<=======>|              CompositeSize*8-1
  ///                                 FFRS   |
  ///                                       FOCE
  /// with:
  /// - SOCE: Slot offset in the Composite instruction's Encoding
  /// - FOCE: Fixup Offset in the Composite Encoding = SOCE + FFRS
  /// NOTE: The size of the fixup is invariant.
  SmallVector<MCFixup>
  translateFixupsInComposite(const MCInst &CompositeInst, const MCInst &SubInst,
                             SmallVector<MCFixup> &BaseFixups) const;
};

#define DEBUG_TYPE "mccodeemitter"

namespace {
/// Computes the encoding of a signed immediate value that is
/// represented as a multiple of four in the encoding
/// N Is the number of bits of the field
/// offset A value to be added before coding
/// isNegative The value is implicitly negated. Used for range check
/// \param MI The parent machine instruction
/// \param OpNo The operand number to which this value relates
/// \param Op [OUT] Yields the computed encoding
/// \param Fixups The fixups for the instruction
/// \param STI The subtarget
template <int N, int offset, bool isNegative>
void getSImmOpValueX4(const MCInst &MI, unsigned OpNo, APInt &Op,
                      SmallVectorImpl<MCFixup> &Fixups,
                      const MCSubtargetInfo &STI) {
  const MCOperand &MO = MI.getOperand(OpNo);
  if (MO.isImm()) {
    int64_t Imm = MO.getImm();
    int64_t Min = isNegative ? minIntN(N + 3) : minIntN(N + 2);
    int64_t Max = isNegative ? -4 : maxIntN(N + 2);
    Imm = Imm + offset;
    assert((Imm & 0x3) == 0 && "Value must be divisible by 4!");
    assert(Imm >= Min && Imm <= Max &&
           "can not represent value in the given immediate type range!");
    Imm >>= 2;
    Op = Imm;
  } else
    llvm_unreachable("Unhandled expression!");
}

/// Similar to getSImmOpValueX4, but for multiples of 16
template <int N, int offset, bool isNegative>
void getSImmOpValueX16(const MCInst &MI, unsigned OpNo, APInt &Op,
                       SmallVectorImpl<MCFixup> &Fixups,
                       const MCSubtargetInfo &STI) {
  const MCOperand &MO = MI.getOperand(OpNo);

  if (MO.isImm()) {
    int64_t Imm = MO.getImm();
    int64_t Min = isNegative ? minIntN(N + 5) : minIntN(N + 4);
    int64_t Max = isNegative ? -16 : maxIntN(N + 4);
    assert((Imm & 0xF) == 0 && "Value must be divisible by 16!");
    assert(Imm >= Min && Imm <= Max &&
           "can not represent value in the given immediate type range!");
    Imm >>= 4;
    Op = Imm;
  } else
    llvm_unreachable("Unhandled expression!");
}

/// Similar to getSImmOpValue4, but for multiples of 32
template <int N, int offset, bool isNegative>
void getSImmOpValueX32(const MCInst &MI, unsigned OpNo, APInt &Op,
                       SmallVectorImpl<MCFixup> &Fixups,
                       const MCSubtargetInfo &STI) {
  const MCOperand &MO = MI.getOperand(OpNo);

  if (MO.isImm()) {
    int64_t Imm = MO.getImm();
    int64_t Min = isNegative ? minIntN(N + 6) : minIntN(N + 5);
    int64_t Max = isNegative ? -32 : maxIntN(N + 5);
    assert((Imm & 0x1F) == 0 && "Value must be divisible by 32!");
    assert(Imm >= Min && Imm <= Max &&
           "can not represent value in the given immediate type range!");
    Imm >>= 5;
    Op = Imm;
  } else
    llvm_unreachable("Unhandled expression!");
}

} // namespace
} // namespace llvm
