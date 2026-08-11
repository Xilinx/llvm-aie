//===- AIE2PSemantics.cpp - AIE2p instruction behaviour -------------------===//
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
/// The scalar core of AIE2p: ALU, compare, shift, move, scalar memory, and
/// control flow. Everything else faults by name.
//
//===----------------------------------------------------------------------===//

#include "AIESemantics.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/ArrayRef.h"
#include "MCTargetDesc/aie2p/AIE2PMCTargetDesc.h"
#include "llvm/ADT/Twine.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCInstrItineraries.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/TargetParser/Triple.h"

using namespace llvm;
using namespace llvm::AIESim;

namespace {

/// Byte addresses into a core's data memory are 20 bits wide, the width of the
/// pointer registers that carry them.
constexpr unsigned DataAddrBits = 20;

/// Operand access against the state at the top of the bundle.
///
/// Any read that cannot be answered - an unmodelled register, or one holding a
/// value the model declined to compute - clears \p Ok, and the caller turns
/// that into a named fault rather than a plausible number.
struct Operands {
  const MCInst &MI;
  const AIECoreState &State;
  /// The bundle this instruction issues in, and the per-operand read/write
  /// cycles relative to it. Both come from the same itinerary entry: AIE
  /// records a cycle for EVERY operand, uses included, and a store's source
  /// is read late (cycle 7 on ST_s16_idx) where its pointer is read at 1.
  uint64_t IssueCycle = 0;
  const InstrItineraryData *Itin = nullptr;
  unsigned SchedClass = 0;
  bool Ok = true;
  MCRegister BadReg;

  /// Cycle at which operand \p I is read or written, absolute.
  ///
  /// 1 when the model does not describe it. The compiler schedules against
  /// these same numbers, so an operand it does not describe cannot be one the
  /// schedule relied on, and 1 is the tightest assumption -- it makes the
  /// fewest reads see a value early.
  uint64_t cycleOf(unsigned I) const {
    std::optional<unsigned> C =
        Itin ? Itin->getOperandCycle(SchedClass, I) : std::nullopt;
    return IssueCycle + (C && *C >= 1 ? *C : 1);
  }

  /// Pipeline-forwarding class of operand \p I, 0 when the itinerary gives
  /// none. Compared against the class a value was written with, which is what
  /// InstrItineraryData::hasPipelineForwarding does with the same two numbers.
  unsigned fwdOf(unsigned I) const {
    return Itin ? Itin->getForwardingClass(SchedClass, I) : 0;
  }

  MCRegister reg(unsigned I) {
    if (!MI.getOperand(I).isReg()) {
      Ok = false;
      return MCRegister();
    }
    return MI.getOperand(I).getReg();
  }

  int64_t imm(unsigned I) {
    if (!MI.getOperand(I).isImm()) {
      Ok = false;
      return 0;
    }
    return MI.getOperand(I).getImm();
  }

  uint32_t val(unsigned I) {
    APInt V;
    MCRegister R = reg(I);
    if (!State.Regs.read(R, V, cycleOf(I), fwdOf(I))) {
      Ok = false;
      BadReg = R;
      return 0;
    }
    // A 20-bit pointer or special register read into a 32-bit datapath.
    // UNVERIFIED: the architecture spec has not been checked for whether these
    // widen by zero or by sign extension.
    return V.zextOrTrunc(32).getZExtValue();
  }

  /// The low \p Bits of operand \p I, for a source the 32-bit datapath
  /// assumption in val() does not fit: vbcst.64 reads an eL register pair.
  APInt valN(unsigned I, unsigned Bits) {
    APInt V;
    MCRegister R = reg(I);
    if (!State.Regs.read(R, V, cycleOf(I), fwdOf(I))) {
      Ok = false;
      BadReg = R;
      return APInt(Bits, 0);
    }
    return V.zextOrTrunc(Bits);
  }

  // Same as val(), for a register that is architecturally implicit rather
  // than an MI operand -- sp in a *_spill opcode, which encodes only an
  // offset (aie2p/AIE2PInstrInfo.td's spill pseudos all expand to a fixed
  // "[sp, #$imm]" real opcode with no pointer-register bits at all).
  uint32_t valReg(MCRegister R) {
    APInt V;
    // Implicit operands have no itinerary entry, so this reads at issue + 1:
    // the earliest a value can be needed, which is the assumption that never
    // hands back a result before its producer could have made it.
    if (!State.Regs.read(R, V, IssueCycle + 1)) {
      Ok = false;
      BadReg = R;
      return 0;
    }
    return V.zextOrTrunc(32).getZExtValue();
  }
};

class AIE2PSemantics : public AIESemantics {
public:
  AIE2PSemantics(const MCInstrInfo &MII, const MCRegisterInfo &MRI,
                 InstrItineraryData Itin, ArrayRef<AIESubRegRange> SubRegRanges)
      : MII(MII), MRI(MRI), Itin(std::move(Itin)),
        SubRegRanges(SubRegRanges) {}

  bool isEndOfProgram(const MCInst &MI) const override {
    return MI.getOpcode() == AIE2P::DONE;
  }

  std::array<MCRegister, 3> getLoopRegisters() const override {
    return {AIE2P::lc, AIE2P::ls, AIE2P::le};
  }

  MCRegister getLinkRegister() const override { return AIE2P::lr; }

  ArrayRef<AIESubRegRange> getSubRegRanges() const override {
    return SubRegRanges;
  }

  const InstrItineraryData *getItineraries() const override {
    return Itin.isEmpty() ? nullptr : &Itin;
  }

  StepResult execute(const MCInst &MI, const AIECoreState &State,
                     AIEHostInterface &Host, SlotEffects &Eff,
                     std::string &FaultMsg) override;

private:
  const MCInstrInfo &MII;
  const MCRegisterInfo &MRI;
  InstrItineraryData Itin;
  ArrayRef<AIESubRegRange> SubRegRanges;
};

/// Count leading sign bits, which is what "clb" reports.
uint32_t countLeadingSignBits(uint32_t V) {
  return llvm::countl_zero(int32_t(V) < 0 ? ~V : V) - 1;
}

} // namespace

StepResult AIE2PSemantics::execute(const MCInst &MI, const AIECoreState &State,
                                   AIEHostInterface &Host, SlotEffects &Eff,
                                   std::string &FaultMsg) {
  const unsigned Opc = MI.getOpcode();
  const MCInstrDesc &Desc = MII.get(Opc);
  StringRef Name = MII.getName(Opc);

  // Semantics below index operands positionally, so a description that does not
  // match what was decoded must stop here rather than read the wrong operand.
  if (MI.getNumOperands() != Desc.getNumOperands()) {
    FaultMsg = (Name + ": decoded " + Twine(MI.getNumOperands()) +
                " operands, description has " + Twine(Desc.getNumOperands()))
                   .str();
    return StepResult::Fault;
  }

  Operands Op{MI, State, State.Cycle, Itin.isEmpty() ? nullptr : &Itin,
              Desc.getSchedClass()};
  auto fault = [&](const Twine &Msg) {
    FaultMsg = Msg.str();
    return StepResult::Fault;
  };
  auto Def32 = [&](unsigned Idx, uint32_t V) {
    Eff.RegWrites.push_back({Op.reg(Idx), APInt(32, V), Op.cycleOf(Idx), Op.fwdOf(Idx)});
  };
  auto DefAddr = [&](unsigned Idx, uint32_t V) {
    Eff.RegWrites.push_back(
        {Op.reg(Idx), APInt(DataAddrBits, V), Op.cycleOf(Idx), Op.fwdOf(Idx)});
  };

  // Address of a scalar access, and the post-increment written back to the
  // pointer register.
  auto access = [&](unsigned PtrIdx, int64_t Offset) {
    return uint32_t((Op.val(PtrIdx) + Offset) &
                    maskTrailingOnes<uint32_t>(DataAddrBits));
  };

  // *_spill's base is sp, architecturally fixed rather than an operand.
  auto spAccess = [&](int64_t Offset) {
    return uint32_t((Op.valReg(AIE2P::sp) + Offset) &
                    maskTrailingOnes<uint32_t>(DataAddrBits));
  };

  auto scalarLoad = [&](unsigned DstIdx, uint32_t Addr, unsigned NumBytes,
                        bool Signed) -> StepResult {
    APInt V;
    switch (Host.load(Addr, NumBytes, V)) {
    case PortStatus::Stall:
      return StepResult::Stalled;
    case PortStatus::Fault:
      FaultMsg = (Name + ": no data memory at " + Twine::utohexstr(Addr)).str();
      return StepResult::Fault;
    case PortStatus::Ok:
      break;
    }
    Eff.RegWrites.push_back({Op.reg(DstIdx), Signed ? V.sext(32) : V.zext(32),
                             Op.cycleOf(DstIdx), Op.fwdOf(DstIdx)});
    return StepResult::Retired;
  };

  // 2D post-increment: advance the pointer by one step of a nested walk, and
  // update the iteration counter that says where in the walk we are.
  //
  //     if (count == size) { ptr += mod;  count = 0; }
  //     else               { ptr += incr; count += 1; }
  //
  // The d register is [mod, size, incr, count] -- the field order
  // AIE2InstructionSelector::createDRegSequence builds, laying ModifierReg at
  // sub_mod, SizeReg at sub_dim_size, IncrReg at sub_dim_stride and CountReg
  // at sub_dim_count.
  //
  // The rule itself is read off aie_api, which drives this from C and so
  // fixes what the fields must mean. Its circular_iterator::operator++ is
  //
  //     add_2d_ptr(ptr, -(elems - stride), elems/stride - 1, index, stride)
  //
  // -- step by `stride` for elems/stride - 1 iterations, then jump back by
  // -(elems - stride), which lands exactly on the base. A second, independent
  // use pins it further: fft_dit_radix3 calls add_2d_ptr(p, 1, r/8-1, cnt, 0),
  // an increment of ZERO with a modifier of one, i.e. hold this pointer for
  // r/8 iterations and then step it. Both only work under the rule above.
  //
  // A field of an addressing descriptor, read at the issue cycle since these
  // are not MI operands of their own.
  auto dfield = [&](MCRegister D, unsigned SubIdx) -> uint32_t {
    MCRegister Sub = MRI.getSubReg(D, SubIdx);
    APInt V;
    if (!Sub || !State.Regs.read(Sub, V, Op.IssueCycle + 1)) {
      Op.Ok = false;
      Op.BadReg = Sub;
      return 0;
    }
    return V.zextOrTrunc(32).getZExtValue();
  };

  // Offsets are 20-bit and may be negative, which is how a wrap jumps
  // backwards; masking the sum lets modular arithmetic carry the sign without
  // anything being extended.
  auto advance = [&](uint32_t Ptr, uint32_t Delta) {
    return uint32_t((Ptr + Delta) & maskTrailingOnes<uint32_t>(DataAddrBits));
  };
  auto defCount = [&](unsigned Idx, uint32_t V) {
    Eff.RegWrites.push_back(
        {Op.reg(Idx), APInt(DataAddrBits, V), Op.cycleOf(Idx), Op.fwdOf(Idx)});
  };

  // \returns the new pointer; writes the counter through \p Eff.
  auto step2D = [&](uint32_t Ptr, MCRegister D, unsigned CountIdx) -> uint32_t {
    const uint32_t Mod = dfield(D, AIE2P::sub_mod);
    const uint32_t Size = dfield(D, AIE2P::sub_dim_size);
    const uint32_t Incr = dfield(D, AIE2P::sub_dim_stride);
    const uint32_t Count = dfield(D, AIE2P::sub_dim_count);

    const bool Wrap = Count == Size;
    defCount(CountIdx, Wrap ? 0 : Count + 1);
    return advance(Ptr, Wrap ? Mod : Incr);
  };

  // 3D: the same walk with one more level around it.
  //
  //     if      (c1 != size1) { ptr += incr1; c1 += 1; }
  //     else if (c2 != size2) { ptr += incr2; c1 = 0; c2 += 1; }
  //     else                  { ptr += mod;   c1 = 0; c2 = 0; }
  //
  // The descriptor is two 80-bit dims, and createDSRegSequence fills SEVEN of
  // the eight fields: the hi dim's own mod slot is unused, so the single
  // sub_mod is the outermost wrap shared by the whole walk.
  //
  // aie_api again supplies the rule, and this time side by side with the 2D
  // one, which is what makes the nesting unambiguous:
  //
  //     add_2d_byte(p, inc.inc2, inc.num1, c1, inc.inc1)
  //     add_3d_byte(p, inc.inc3, inc.num1, c1, inc.inc1, inc.num2, c2, inc.inc2)
  //
  // -- the extra level is appended and the wrap moves out to inc3. That an
  // outer increment REPLACES the inner rather than adding to it is visible in
  // aie_api's own sliding_window_dim_3d, which builds inc2 as `step + inc2`
  // and inc3 as `step + inc3`: the inner step is folded in by the caller
  // precisely because the hardware does not apply it on those iterations.
  auto step3D = [&](uint32_t Ptr, MCRegister D, unsigned C1Idx,
                    unsigned C2Idx) -> uint32_t {
    const uint32_t Mod = dfield(D, AIE2P::sub_mod);
    const uint32_t Size1 = dfield(D, AIE2P::sub_dim_size);
    const uint32_t Incr1 = dfield(D, AIE2P::sub_dim_stride);
    const uint32_t Count1 = dfield(D, AIE2P::sub_dim_count);
    const uint32_t Size2 = dfield(D, AIE2P::sub_hi_dim_then_sub_dim_size);
    const uint32_t Incr2 = dfield(D, AIE2P::sub_hi_dim_then_sub_dim_stride);
    const uint32_t Count2 = dfield(D, AIE2P::sub_hi_dim_then_sub_dim_count);

    if (Count1 != Size1) {
      defCount(C1Idx, Count1 + 1);
      defCount(C2Idx, Count2);
      return advance(Ptr, Incr1);
    }
    defCount(C1Idx, 0);
    if (Count2 != Size2) {
      defCount(C2Idx, Count2 + 1);
      return advance(Ptr, Incr2);
    }
    defCount(C2Idx, 0);
    return advance(Ptr, Mod);
  };

  // A vector load. Its width is the destination register class's, where
  // scalarLoad's is the 32-bit datapath, and the value is not extended: it
  // arrives already the width of the register it lands in.
  auto vectorLoad = [&](unsigned DstIdx, uint32_t Addr) -> StepResult {
    const MCRegister Dst = Op.reg(DstIdx);
    const unsigned W = State.Regs.getClassWidth(Dst);
    if (!W || W % 8) {
      FaultMsg = (Name + ": " + MRI.getName(Dst) + " is " + Twine(W) +
                  " bits, not a whole number of bytes")
                     .str();
      return StepResult::Fault;
    }
    APInt V;
    switch (Host.load(Addr, W / 8, V)) {
    case PortStatus::Stall:
      return StepResult::Stalled;
    case PortStatus::Fault:
      FaultMsg = (Name + ": no data memory at " + Twine::utohexstr(Addr)).str();
      return StepResult::Fault;
    case PortStatus::Ok:
      break;
    }
    Eff.RegWrites.push_back({Dst, V, Op.cycleOf(DstIdx), Op.fwdOf(DstIdx)});
    return StepResult::Retired;
  };

  // The source register is NAMED here, not read: a store samples its data at
  // its own operand cycle, which on the narrow forms is 7 cycles after issue,
  // by which time an instruction scheduled after the store may have produced
  // it. The executor reads it when the pipeline gets there.
  auto scalarStore = [&](unsigned SrcIdx, uint32_t Addr, unsigned NumBytes) {
    Eff.MemWrites.push_back(
        {Addr, NumBytes, Op.reg(SrcIdx), Op.cycleOf(SrcIdx), Op.fwdOf(SrcIdx)});
  };

  // vups.Nx: widen each lane of a vector into an accumulator lane, shifting
  // left by \p su on the way in.
  //
  // The lane WIDTH is not in the instruction. One opcode serves two
  // conversions -- VUPS_2x_mv_ups_w2b_upsSign0 is the selection target for
  // both acc32_v16_I256_ups (16 lanes, i16 -> i32) and acc64_v8_I256_ups
  // (8 lanes, i32 -> i64) -- and those produce different bits from the same
  // input. What separates them is the crUPSMode control register, which
  // AIE2PInstructionSelector sets per intrinsic: 0 for every acc32 form, 1 for
  // every acc64 form (AIE2PInstructionSelector.cpp, selectVUPS(I, MRI, 0) and
  // (I, MRI, 1)). The compiler emits the write right before the use, e.g.
  // `$crupsmode = MOV_scalar_imm11_pseudo 0` in
  // test/CodeGen/AIE/aie2p/GlobalIsel/inst-select-postinc-vlda_ups.mir.
  //
  // So mode gives the accumulator lane width, the two register classes give
  // the totals, and the lane count falls out of them. The sign is in the
  // opcode: 0x0 selects upsSign0 and 0x1 upsSign1 in the ISel patterns, and
  // aie_api passes its `v_sign` there, true meaning signed.
  auto upsInto = [&](unsigned DstIdx, const APInt &Src, unsigned ShiftIdx,
                     bool Signed) -> StepResult {
    const MCRegister Dst = Op.reg(DstIdx);
    const unsigned DstBits = State.Regs.getClassWidth(Dst);
    const unsigned SrcBits = Src.getBitWidth();
    const unsigned AccLaneBits = (Op.valReg(AIE2P::crUPSMode) & 1) ? 64 : 32;
    if (!DstBits || !SrcBits || DstBits % AccLaneBits)
      return fault(Name + ": " + MRI.getName(Dst) + " is not a whole number of " +
                   Twine(AccLaneBits) + "-bit accumulator lanes");
    const unsigned Lanes = DstBits / AccLaneBits;
    if (!Lanes || SrcBits % Lanes)
      return fault(Name + ": " + Twine(SrcBits) + " source bits do not divide " +
                   "into " + Twine(Lanes) + " lanes");
    const unsigned SrcLaneBits = SrcBits / Lanes;

    const uint32_t Shift = Op.val(ShiftIdx);
    APInt Acc(DstBits, 0);
    for (unsigned I = 0; I != Lanes; ++I) {
      const APInt Lane = Src.extractBits(SrcLaneBits, I * SrcLaneBits);
      APInt Wide = Signed ? Lane.sext(AccLaneBits) : Lane.zext(AccLaneBits);
      Acc.insertBits(Wide << Shift, I * AccLaneBits);
    }
    Eff.RegWrites.push_back({Dst, Acc, Op.cycleOf(DstIdx), Op.fwdOf(DstIdx)});
    return StepResult::Retired;
  };

  // The register form: source is a vector register, shift at operand 2.
  auto ups = [&](bool Signed) {
    return upsInto(0, Op.valN(1, State.Regs.getClassWidth(Op.reg(1))), 2,
                   Signed);
  };

  // The memory-fused form, vlda.ups: the same widening, over bits just loaded
  // rather than bits in a register. \p SrcBits is the ACCESS width, which the
  // opcode carries as dmw (256) or dmx (512) -- there is no source register to
  // read it from.
  auto fusedUps = [&](unsigned SrcBits, unsigned DstIdx, unsigned ShiftIdx,
                      uint32_t Addr, bool Signed) -> StepResult {
    APInt V;
    switch (Host.load(Addr, SrcBits / 8, V)) {
    case PortStatus::Stall:
      return StepResult::Stalled;
    case PortStatus::Fault:
      return fault(Name + ": no data memory at " + Twine::utohexstr(Addr));
    case PortStatus::Ok:
      break;
    }
    return upsInto(DstIdx, V, ShiftIdx, Signed);
  };

  // vsrs.Nx: narrow each accumulator lane back to a vector lane -- shift
  // right, round, saturate. The inverse of vups, and it reads crSRSMode for
  // the accumulator lane width exactly as vups reads crUPSMode.
  //
  // Two more control registers matter here, and their encodings are llvm-aie's
  // own, from clang/lib/Headers/aie2p/aie2p_defines.h:
  //
  //   crRnd  0 floor  1 ceil  2 sym_floor  3 sym_ceil          -- directional
  //          8 neg_inf  9 pos_inf  10 sym_zero  11 sym_inf
  //          12 conv_even  13 conv_odd                         -- nearest
  //   crSat  0 none (wrap)  1 saturate  3 symmetric
  //
  // The split is the useful part: 0-3 never look at the discarded bits, they
  // just pick a direction. 8-13 round to NEAREST and differ only in how they
  // break an exact half. The saturation values are aie_api's saturation_mode.
  // crRnd values this model implements, checked before any narrowing is set
  // up so the arithmetic itself cannot fail. 0-3 are directional, 8-13 round
  // to nearest; 4-7 are not assigned by aie2p_defines.h.
  auto isKnownRounding = [](uint32_t Rnd) {
    return Rnd <= 3 || (Rnd >= 8 && Rnd <= 13);
  };

  // The narrowing itself, as a plain function of the accumulator bits. Split
  // out from the instruction so the fused stores can hand it to the executor
  // and have it applied when the pipeline reaches their sample cycle.
  auto srsNarrow = [](const APInt &Src, unsigned DstBits, unsigned AccLaneBits,
                      unsigned Shift, uint32_t Rnd, uint32_t Sat,
                      bool Signed) -> APInt {
    const unsigned Lanes = Src.getBitWidth() / AccLaneBits;
    const unsigned DstLaneBits = DstBits / Lanes;
    APInt Out(DstBits, 0);
    for (unsigned I = 0; I != Lanes; ++I) {
      APInt V = Src.extractBits(AccLaneBits, I * AccLaneBits);
      // Arithmetic shift, so Q is the floor quotient and Rem the bits it
      // dropped, taken as a non-negative value below 2^Shift.
      APInt Q = V.ashr(Shift);
      const bool Neg = V.isNegative();
      APInt Rem = Shift ? V - (Q << Shift) : APInt(AccLaneBits, 0);
      const APInt Half = Shift ? APInt(AccLaneBits, 1) << (Shift - 1)
                               : APInt(AccLaneBits, 0);

      bool Up = false;
      if (Shift && !Rem.isZero()) {
        switch (Rnd) {
        case 1: // ceil: always toward +inf
          Up = true;
          break;
        case 2: // sym_floor: always toward zero
          Up = Neg;
          break;
        case 3: // sym_ceil: always away from zero
          Up = !Neg;
          break;
        case 0: // floor: Q already is it
          break;
        default: {
          // Nearest, tie-broken by the mode. Q is the floor result, so Q + 1
          // is the other candidate and Rem against Half decides between them.
          if (Rem.ugt(Half))
            Up = true;
          else if (Rem.ult(Half))
            Up = false;
          else
            switch (Rnd) {
            case 8: Up = false; break;                 // neg_inf
            case 9: Up = true; break;                  // pos_inf
            case 10: Up = Neg; break;                  // sym_zero
            case 11: Up = !Neg; break;                 // sym_inf
            case 12: Up = Q[0]; break;                 // conv_even
            case 13: Up = !Q[0]; break;                // conv_odd
            default:
              // Rejected before the closure is built, so this cannot be
              // reached; see isKnownRounding.
              llvm_unreachable("unvalidated rounding mode");
            }
          break;
        }
        }
      }
      if (Up)
        ++Q;

      // Saturate to the destination lane, then take its low bits.
      if (Sat != 0) {
        const APInt Max = Signed ? APInt::getSignedMaxValue(DstLaneBits)
                                                 .sext(AccLaneBits)
                                 : APInt::getMaxValue(DstLaneBits)
                                                 .zext(AccLaneBits);
        // Symmetric clamps the low end to -Max, so the limits have the same
        // magnitude; ordinary saturation keeps the extra negative value.
        const APInt Min = !Signed ? APInt(AccLaneBits, 0)
                          : Sat == 3
                              ? -Max
                              : APInt::getSignedMinValue(DstLaneBits)
                                    .sext(AccLaneBits);
        // Both comparisons are SIGNED whatever the destination is: the
        // accumulator holds a signed sum, and only the output's
        // interpretation changes. Comparing unsigned here reads a negative
        // lane as enormous and clamps it to the maximum instead of to zero.
        if (Q.sgt(Max))
          Q = Max;
        else if (Q.slt(Min))
          Q = Min;
      }
      Out.insertBits(Q.trunc(DstLaneBits), I * DstLaneBits);
    }
    return Out;
  };

  // The geometry checks, shared by the register and fused forms. \p SrcBits is
  // the accumulator width and \p DstBits the vector one.
  auto srsGeometry = [&](unsigned SrcBits, unsigned DstBits,
                         unsigned &AccLaneBits) -> bool {
    AccLaneBits = (Op.valReg(AIE2P::crSRSMode) & 1) ? 64 : 32;
    if (!SrcBits || SrcBits % AccLaneBits)
      return false;
    const unsigned Lanes = SrcBits / AccLaneBits;
    return Lanes && DstBits && DstBits % Lanes == 0;
  };

  // (vec)(acc, su): narrow into a register.
  auto srs = [&](bool Signed) -> StepResult {
    const MCRegister Dst = Op.reg(0);
    const unsigned DstBits = State.Regs.getClassWidth(Dst);
    const unsigned SrcBits = State.Regs.getClassWidth(Op.reg(1));
    unsigned AccLaneBits = 0;
    if (!srsGeometry(SrcBits, DstBits, AccLaneBits))
      return fault(Name + ": " + Twine(SrcBits) + " accumulator bits and " +
                   Twine(DstBits) + " result bits do not agree on a lane count");
    const uint32_t Rnd = Op.valReg(AIE2P::crRnd);
    if (!isKnownRounding(Rnd))
      return fault(Name + ": crRnd holds " + Twine(Rnd) +
                   ", which is not a rounding mode this model knows");
    Eff.RegWrites.push_back(
        {Dst,
         srsNarrow(Op.valN(1, SrcBits), DstBits, AccLaneBits,
                   Op.val(2) & (AccLaneBits - 1), Rnd, Op.valReg(AIE2P::crSat),
                   Signed),
         Op.cycleOf(0), Op.fwdOf(0)});
    return StepResult::Retired;
  };

  // vst.srs: the same narrowing, but the result goes to memory. The source is
  // NAMED and the arithmetic deferred, exactly as an ordinary store defers --
  // \p SrcIdx's register may be written by an instruction issued after this
  // one, and reading it at issue would quietly take the wrong value.
  // \p Narrowing is the 2x or 4x in the mnemonic. The stored width follows
  // from it and the accumulator's class -- the dm/dmw/dmx prefix does NOT
  // give it away, since `dm` names both a 512-bit store (2x from cm) and a
  // 256-bit one (4x from cm).
  auto fusedSrs = [&](unsigned Narrowing, unsigned SrcIdx, unsigned ShiftIdx,
                      uint32_t Addr, bool Signed) -> StepResult {
    const unsigned SrcBits = State.Regs.getClassWidth(Op.reg(SrcIdx));
    const unsigned DstBits = SrcBits / Narrowing;
    unsigned AccLaneBits = 0;
    if (!srsGeometry(SrcBits, DstBits, AccLaneBits))
      return fault(Name + ": " + Twine(SrcBits) + " accumulator bits and " +
                   Twine(DstBits) + " stored bits do not agree on a lane count");
    // The control registers and the shift are read HERE, at issue, which is
    // right: they are set before the instruction that reads them, and the
    // compiler emits those writes immediately ahead of it.
    const unsigned Shift = Op.val(ShiftIdx) & (AccLaneBits - 1);
    const uint32_t Rnd = Op.valReg(AIE2P::crRnd);
    const uint32_t Sat = Op.valReg(AIE2P::crSat);
    if (!isKnownRounding(Rnd))
      return fault(Name + ": crRnd holds " + Twine(Rnd) +
                   ", which is not a rounding mode this model knows");
    Eff.MemWrites.push_back(
        {Addr, DstBits / 8, Op.reg(SrcIdx), Op.cycleOf(SrcIdx),
         Op.fwdOf(SrcIdx),
         // Always a value: the integer narrowing is total, so this arm never
         // declines and the optional is pure signature.
         [=](const APInt &V) -> std::optional<APInt> {
           return srsNarrow(V, DstBits, AccLaneBits, Shift, Rnd, Sat, Signed);
         }});
    return StepResult::Retired;
  };

  // vmul/vmac: multiply two vectors into an accumulator, optionally adding an
  // accumulator in.
  //
  // The lane-feeding layout is NOT in the opcode -- one opcode serves every
  // shape -- and it is not hidden either. It is in the $acc operand, a
  // configuration word whose bit layout is llvm-aie's own, from
  // aie2p_compute_control in clang/lib/Headers/aie2p/aie2p_vmult.h:
  //
  //   zero_acc<<0  amode<<1  bmode<<3  variant<<5
  //   sgn_y<<8  sgn_x<<9  shift16<<10  sub0<<11  sub1<<12  sub2<<13
  //   sub_mask<<16
  //
  // (amode, bmode, variant) jointly name the shape -- 21 combinations across
  // that header's 6210 wrappers, each spelled out in the wrapper's own name.
  // An unmodelled triple faults with its numbers, so it says which shape it
  // was rather than producing a plausible product.
  //
  // Note bmode is NOT the element width on its own: measured over all the
  // wrappers it is 8-bit only for bmode 1 and 16-bit only for bmode 3, but
  // spans two widths for 0 and 2. The width comes from the shape plus the
  // operand register class.
  auto mac = [&](unsigned S1Idx, unsigned S2Idx, unsigned ConfIdx,
                 int AccIdx) -> StepResult {
    const uint32_t Conf = Op.val(ConfIdx);
    const unsigned AMode = (Conf >> 1) & 3;
    const unsigned BMode = (Conf >> 3) & 3;
    const unsigned Variant = (Conf >> 5) & 7;
    const bool ZeroAcc = Conf & 1;
    const bool SgnY = (Conf >> 8) & 1;
    const bool SgnX = (Conf >> 9) & 1;

    // Every shape is a matrix product C[MxN] = A[MxK] * B[KxN]; elementwise is
    // just K = 1. The name states M, K and N -- `8x8_8x8` is M=8 K=8 N=8 --
    // and aie_api's mmul templates confirm it, instantiating that shape as
    // mmul_8_4<8, 8, 8, TypeA, TypeB, 32> over 64-element operands, which is
    // <M, K, N, ..., accumulator bits>.
    // Elementwise is NOT a matrix product with K = N = 1: C[i] = A[i]*B[i]
    // takes a full-length B, where a matmul with N = 1 would take one column.
    // It gets its own arm rather than a contrived parameterisation.
    // Operand element widths differ per side on the mixed shapes, so they are
    // tracked separately rather than shared.
    unsigned M = 0, K = 0, N = 0, ABits = 0, BBits = 0;
    bool Hadamard = false;
    if (AMode == 0 && BMode == 1 && Variant == 1)
      Hadamard = true, M = 64, ABits = 8, BBits = 8; // elem_64
    else if (AMode == 0 && BMode == 1 && Variant == 0)
      M = 8, K = 8, N = 8, ABits = 8, BBits = 8; // 8x8_8x8
    else if (AMode == 0 && BMode == 2 && Variant == 0)
      M = 8, K = 4, N = 8, ABits = 16, BBits = 8; // 8x4_4x8
    else if (AMode == 0 && BMode == 3 && Variant == 0)
      M = 8, K = 2, N = 8, ABits = 16, BBits = 16; // 8x2_2x8
    else if (AMode == 1 && BMode == 0 && Variant == 0)
      M = 4, K = 2, N = 8, ABits = 32, BBits = 16; // 4x2_2x8
    else if (AMode == 1 && BMode == 2 && Variant == 0)
      M = 4, K = 8, N = 8, ABits = 16, BBits = 8; // 4x8_8x8
    else if (AMode == 1 && BMode == 3 && Variant == 0)
      M = 4, K = 4, N = 8, ABits = 16, BBits = 16; // 4x4_4x8
    else
      return fault(Name + ": MAC shape amode=" + Twine(AMode) + " bmode=" +
                   Twine(BMode) + " variant=" + Twine(Variant) +
                   " is not modelled");

    const MCRegister Dst = Op.reg(0);
    const unsigned DstBits = State.Regs.getClassWidth(Dst);
    const unsigned S1Bits = State.Regs.getClassWidth(Op.reg(S1Idx));
    const unsigned S2Bits = State.Regs.getClassWidth(Op.reg(S2Idx));
    // Most shapes read LESS than the register they are handed -- 8x4_4x8 wants
    // 256 bits of B in a 512-bit class, 4x2_2x8 wants 256 of both -- and the
    // read window is the LOW end. aie_api settles that: it feeds every such
    // operand through vector::grow<N>(idx = 0), which places the source at
    // subvector 0 and leaves the rest undef(), and an instruction cannot read
    // bits its caller declares undefined. The positional cross-check is
    // mmul_16_8<8,4,8>, which issues two mac_4x4_4x8 and moves rows 4-7 DOWN
    // (shuffle_down by 16 elements, ret[i] = v[i+n]) to have them read at all.
    // So the operand register only has to be BIG ENOUGH.
    const unsigned WantA = Hadamard ? M * ABits : M * K * ABits;
    const unsigned WantB = Hadamard ? M * BBits : K * N * BBits;
    if (S1Bits < WantA || S2Bits < WantB)
      return fault(Name + ": shape wants " + Twine(WantA) + " and " +
                   Twine(WantB) + " source bits, got " + Twine(S1Bits) +
                   " and " + Twine(S2Bits));
    const unsigned Lanes = Hadamard ? M : M * N;
    if (!DstBits || DstBits % Lanes)
      return fault(Name + ": " + Twine(DstBits) + " accumulator bits do not " +
                   "divide into " + Twine(Lanes) + " lanes");
    const unsigned AccLaneBits = DstBits / Lanes;

    const APInt A = Op.valN(S1Idx, S1Bits);
    const APInt B = Op.valN(S2Idx, S2Bits);
    APInt Prev(DstBits, 0);
    if (AccIdx >= 0 && !ZeroAcc)
      Prev = Op.valN(unsigned(AccIdx), DstBits);

    auto elem = [&](const APInt &V, unsigned Idx, unsigned Bits, bool Signed) {
      const APInt E = V.extractBits(Bits, Idx * Bits);
      return Signed ? E.sext(AccLaneBits) : E.zext(AccLaneBits);
    };

    // Row-major throughout: A[i][k] at i*K+k, B[k][j] at k*N+j, C[i][j] at
    // lane i*N+j.
    APInt Out(DstBits, 0);
    if (Hadamard) {
      for (unsigned I = 0; I != M; ++I) {
        APInt Sum = Prev.extractBits(AccLaneBits, I * AccLaneBits);
        Sum += elem(A, I, ABits, SgnX) * elem(B, I, BBits, SgnY);
        Out.insertBits(Sum, I * AccLaneBits);
      }
    } else {
      // Row-major throughout: A[i][k] at i*K+k, B[k][j] at k*N+j, C[i][j] at
      // lane i*N+j.
      for (unsigned I = 0; I != M; ++I)
        for (unsigned J = 0; J != N; ++J) {
          APInt Sum = Prev.extractBits(AccLaneBits, (I * N + J) * AccLaneBits);
          for (unsigned Kk = 0; Kk != K; ++Kk)
            Sum += elem(A, I * K + Kk, ABits, SgnX) *
                 elem(B, Kk * N + J, BBits, SgnY);
          Out.insertBits(Sum, (I * N + J) * AccLaneBits);
        }
    }
    Eff.RegWrites.push_back({Dst, Out, Op.cycleOf(0), Op.fwdOf(0)});
    return StepResult::Retired;
  };

  // vmul.f (dst)(s1, s2, conf) / vmac.f (dst)(acc1, s1, s2, conf): elementwise
  // bf16 x bf16 into f32 accumulator lanes, optionally adding a prior
  // accumulator in. The conf operand is an eR but is NOT an accumulator --
  // the patterns pass mulbf16_vecconf.ConfBits, so it is the same
  // configuration word the integer MAC forms take, decoded the same way, and
  // AccIdx < 0 selects the vmul.f (no-accumulate) case exactly as it does in
  // mac() above.
  //
  // NO ROUNDING MODE IS NEEDED FOR THE MULTIPLY, and that is a property of
  // the shape rather than an assumption. A bf16 significand is 8 bits, so a
  // product of two needs at most 16, and an f32 significand holds 24 -- the
  // result is exactly representable. The instruction carries
  // Uses = [crFPMask], but a mask selects which exceptions are REPORTED, and
  // the flags it reports into (Defs = [srFPFlags]) are not computed here; the
  // executor poisons any def the semantics leave alone, so reading a flag
  // later faults rather than returning a stale one.
  //
  // The exactness claim is CHECKED rather than trusted: if the multiply ever
  // rounds, the unknown mode would start to matter, so that faults.
  //
  // The accumulate ADD carries no such guarantee and is not checked for it --
  // summing two arbitrary f32 lanes is ordinarily inexact. Unlike vconv, the
  // vmac.f forms declare no Uses = [crRnd], so the rounding mode is not
  // selectable and round-to-nearest-even is the only one hardware could be
  // applying silently.
  auto mulBf16 = [&](unsigned S1Idx, unsigned S2Idx, unsigned ConfIdx,
                     int AccIdx) -> StepResult {
    const uint32_t Conf = Op.val(ConfIdx);
    const unsigned AMode = (Conf >> 1) & 3;
    const unsigned BMode = (Conf >> 3) & 3;
    const unsigned CMode = (Conf >> 5) & 7;
    const bool ZeroAcc = Conf & 1;
    if (!Op.Ok)
      return fault(Name + ": conf is not readable");
    // AMODE_FP32, BMODE_16x16, CMODE_BF16xBF16_1_elem_1 -- the only shape the
    // bf16 multiply patterns emit. Anything else is a different instruction
    // wearing the same opcode and is not guessed at.
    if (AMode != 2 || BMode != 3 || CMode != 1)
      return fault(Name + ": conf amode=" + Twine(AMode) + " bmode=" +
                   Twine(BMode) + " cmode=" + Twine(CMode) +
                   " is not the elementwise bf16 shape this models");

    const MCRegister Dst = Op.reg(0);
    const unsigned DstBits = State.Regs.getClassWidth(Dst);
    const unsigned S1Bits = State.Regs.getClassWidth(Op.reg(S1Idx));
    const unsigned Lanes = S1Bits / 16;
    if (!Lanes || S1Bits % 16 || DstBits < Lanes * 32)
      return fault(Name + ": " + Twine(S1Bits) + " source bits and " +
                   Twine(DstBits) + " accumulator bits are not " +
                   Twine(Lanes) + " bf16 lanes widening to f32");
    const APInt A = Op.valN(S1Idx, S1Bits);
    const APInt B = Op.valN(S2Idx, S1Bits);
    APInt Prev(DstBits, 0);
    if (AccIdx >= 0 && !ZeroAcc)
      Prev = Op.valN(unsigned(AccIdx), DstBits);
    if (!Op.Ok)
      return fault(Name + ": source is not readable");

    // Lanes past the source fill the rest of the 2048-bit class with zeros.
    // X_X produces 32 lanes into a register that holds 64, and the patterns
    // read back only sub_1024_acc_lo, so nothing observes the top half; zero
    // is the value that cannot pass off a stale result as a new one.
    APInt Out(DstBits, 0);
    for (unsigned I = 0; I != Lanes; ++I) {
      APFloat FA(APFloat::BFloat(), A.extractBits(16, I * 16));
      const APFloat FB(APFloat::BFloat(), B.extractBits(16, I * 16));
      bool Lost = false;
      FA.convert(APFloat::IEEEsingle(), APFloat::rmNearestTiesToEven, &Lost);
      APFloat W = FB;
      W.convert(APFloat::IEEEsingle(), APFloat::rmNearestTiesToEven, &Lost);
      if (FA.multiply(W, APFloat::rmNearestTiesToEven) & APFloat::opInexact)
        return fault(Name + ": lane " + Twine(I) +
                     " rounds, and the rounding mode this uses is not "
                     "established");
      if (AccIdx >= 0) {
        const APFloat FAcc(APFloat::IEEEsingle(),
                            Prev.extractBits(32, I * 32));
        FA.add(FAcc, APFloat::rmNearestTiesToEven);
      }
      Out.insertBits(FA.bitcastToAPInt(), I * 32);
    }
    Eff.RegWrites.push_back({Dst, Out, Op.cycleOf(0), Op.fwdOf(0)});
    return StepResult::Retired;
  };

  // vconv.bf16.fp32 (dst)(src): narrow each f32 accumulator lane to bf16.
  //
  // This is the FIRST op here whose rounding mode is load-bearing. vmul.f
  // needed none because a bf16 product is exact in f32; this direction throws
  // away 16 bits of every lane. The mode is not guessed: the instruction
  // carries Uses = [crF2FMask, crRnd], and crRnd is the same control register
  // the integer SRS path already reads, with the same encoding.
  //
  // bf16 is EXACTLY the top 16 bits of an f32 -- same exponent width, same
  // bias -- so this is a bit-pattern truncation plus, at most, an increment.
  // Working on the pattern rather than through a float conversion is what
  // makes all ten crRnd modes reachable (APFloat offers five) and keeps the
  // tie-breaks identical to srsNarrow's. It also gets subnormals and the
  // normal boundary right for free, since incrementing an IEEE pattern steps
  // to the next representable value across both.
  //
  // The one reinterpretation: srsNarrow works in two's complement, where "up"
  // means toward +inf. A float is sign-magnitude, where truncation always
  // moves toward ZERO and an increment always moves AWAY from it. So each
  // mode is expressed as "increase the magnitude?" instead, which flips the
  // sense on negative lanes.
  // The narrowing itself, as a plain function of the accumulator bits, so the
  // register form and the fused store share one body -- the same split
  // srsNarrow makes for the integer path.
  //
  // Returns nullopt for a lane this model will not narrow, which the register
  // form turns into a fault at issue and the store into one at SampleAt.
  auto bf16Narrow = [](const APInt &Src, unsigned Lanes,
                       uint32_t Rnd) -> std::optional<APInt> {
    APInt Out(Lanes * 16, 0);
    for (unsigned I = 0; I != Lanes; ++I) {
      const uint32_t V = uint32_t(Src.extractBits(32, I * 32).getZExtValue());
      // A NaN whose payload lives only in the discarded low bits would
      // truncate to an infinity, and what the hardware does instead is not
      // established. Infinities need no arm: their low bits are zero, so
      // nothing rounds.
      if ((V >> 23 & 0xFF) == 0xFF && (V & 0x7FFFFF) != 0)
        return std::nullopt;
      APInt T(16, V >> 16);
      const uint32_t Rem = V & 0xFFFF;
      const bool Neg = V >> 31;
      constexpr uint32_t Half = 0x8000;

      bool Away = false;
      if (Rem) {
        switch (Rnd) {
        case 0: // floor: toward -inf, so away from zero only when negative
          Away = Neg;
          break;
        case 1: // ceil: toward +inf
          Away = !Neg;
          break;
        case 2: // sym_floor: toward zero, which truncation already is
          break;
        case 3: // sym_ceil: away from zero
          Away = true;
          break;
        default:
          if (Rem > Half)
            Away = true;
          else if (Rem < Half)
            Away = false;
          else
            switch (Rnd) {
            case 8: Away = Neg; break;                  // neg_inf
            case 9: Away = !Neg; break;                 // pos_inf
            case 10: break;                             // sym_zero
            case 11: Away = true; break;                // sym_inf
            case 12: Away = T[0]; break;                // conv_even
            case 13: Away = !T[0]; break;               // conv_odd
            default:
              llvm_unreachable("unvalidated rounding mode");
            }
          break;
        }
      }
      if (Away)
        ++T;
      Out.insertBits(T, I * 16);
    }
    return Out;
  };

  // vconv.bf16.fp32 (dst)(src): the register form.
  auto convBf16 = [&]() -> StepResult {
    const uint32_t Rnd = Op.valReg(AIE2P::crRnd);
    if (!Op.Ok)
      return fault(Name + ": crRnd is not readable");
    if (!isKnownRounding(Rnd))
      return fault(Name + ": crRnd holds " + Twine(Rnd) +
                   ", which is not a rounding mode this model knows");
    const MCRegister Dst = Op.reg(0);
    const unsigned DstBits = State.Regs.getClassWidth(Dst);
    const unsigned SrcBits = State.Regs.getClassWidth(Op.reg(1));
    const unsigned Lanes = SrcBits / 32;
    if (!Lanes || SrcBits % 32 || DstBits < Lanes * 16)
      return fault(Name + ": " + Twine(SrcBits) + " accumulator bits and " +
                   Twine(DstBits) + " destination bits are not " +
                   Twine(Lanes) + " f32 lanes narrowing to bf16");
    const APInt Src = Op.valN(1, SrcBits);
    if (!Op.Ok)
      return fault(Name + ": source is not readable");
    const std::optional<APInt> Out = bf16Narrow(Src, Lanes, Rnd);
    if (!Out)
      return fault(Name + ": a lane is a NaN, and what this instruction does "
                          "with one is not established");
    Eff.RegWrites.push_back(
        {Dst, Out->zext(DstBits), Op.cycleOf(0), Op.fwdOf(0)});
    return StepResult::Retired;
  };

  // vst.conv.bf16.fp32: the same narrowing, with the result going to memory.
  // The source is NAMED and the arithmetic deferred, exactly as fusedSrs does
  // and for the same reason -- \p SrcIdx's register may be written by an
  // instruction issued after this one.
  //
  // The stored width is the accumulator's lane count times 16; the dm/dmw/dmx
  // prefix does not give it away, so it comes from the source class.
  auto fusedConvSrsBf16 = [&](unsigned SrcIdx, uint32_t Addr) -> StepResult {
    const unsigned SrcBits = State.Regs.getClassWidth(Op.reg(SrcIdx));
    const unsigned Lanes = SrcBits / 32;
    if (!Lanes || SrcBits % 32)
      return fault(Name + ": " + Twine(SrcBits) +
                   " accumulator bits are not whole f32 lanes");
    // crRnd is read HERE, at issue, which is right: it is set before the
    // instruction that reads it. Only the SOURCE is deferred.
    const uint32_t Rnd = Op.valReg(AIE2P::crRnd);
    if (!Op.Ok)
      return fault(Name + ": crRnd is not readable");
    if (!isKnownRounding(Rnd))
      return fault(Name + ": crRnd holds " + Twine(Rnd) +
                   ", which is not a rounding mode this model knows");
    Eff.MemWrites.push_back(
        {Addr, Lanes * 16 / 8, Op.reg(SrcIdx), Op.cycleOf(SrcIdx),
         Op.fwdOf(SrcIdx),
         [=](const APInt &V) { return bf16Narrow(V, Lanes, Rnd); }});
    return StepResult::Retired;
  };

  // vconv.fp32.bf16 (dst)(src): widen each bf16 lane to an f32 accumulator
  // lane -- convBf16's inverse.
  //
  // This direction is EXACT and total, and the opcode says so: it carries no
  // Uses at all, where the narrowing one carries [crF2FMask, crRnd]. bf16 is
  // the top 16 bits of an f32, so widening is those bits with 16 zeros under
  // them. Nothing rounds, nothing saturates, and no lane can be unrepresent-
  // able -- which is why the NaN arm convBf16 needs has no counterpart here.
  // A NaN widens to the same NaN, its payload intact in the high bits.
  //
  // Deliberately NOT routed through upsInto: that widens INTEGER lanes with
  // sext/zext and a shift operand, and it takes its lane width from
  // crUPSMode. These forms have no shift operand and do not read crUPSMode,
  // so borrowing it would make an exact conversion depend on a control
  // register the instruction never mentions.
  auto bf16Widen = [](const APInt &Src, unsigned DstBits) {
    const unsigned Lanes = DstBits / 32;
    APInt Acc(DstBits, 0);
    for (unsigned I = 0; I != Lanes; ++I)
      Acc.insertBits(Src.extractBits(16, I * 16).zext(32) << 16, I * 32);
    return Acc;
  };

  // The geometry both widening forms check: \p SrcBits of bf16 becoming
  // \p DstBits of f32 lanes, 2x either way.
  auto widenGeometry = [&](unsigned SrcBits, unsigned DstBits) {
    return SrcBits && DstBits == 2 * SrcBits && DstBits % 32 == 0;
  };

  // The register form: source width comes from its class.
  auto convUpsBf16 = [&]() -> StepResult {
    const MCRegister Dst = Op.reg(0);
    const unsigned DstBits = State.Regs.getClassWidth(Dst);
    const unsigned SrcBits = State.Regs.getClassWidth(Op.reg(1));
    if (!widenGeometry(SrcBits, DstBits))
      return fault(Name + ": " + Twine(SrcBits) + " source bits and " +
                   Twine(DstBits) + " accumulator bits are not bf16 lanes "
                   "widening to f32");
    const APInt Src = Op.valN(1, SrcBits);
    if (!Op.Ok)
      return fault(Name + ": source is not readable");
    Eff.RegWrites.push_back(
        {Dst, bf16Widen(Src, DstBits), Op.cycleOf(0), Op.fwdOf(0)});
    return StepResult::Retired;
  };

  // vlda.conv.fp32.bf16 -- load bf16 and widen in one instruction. \p SrcBits
  // is the ACCESS width, which the opcode carries as dmw (256) or dmx (512);
  // a fused form has no source register to read it from, exactly as fusedUps.
  auto fusedConvUpsBf16 = [&](unsigned SrcBits, unsigned DstIdx,
                              uint32_t Addr) -> StepResult {
    const MCRegister Dst = Op.reg(DstIdx);
    const unsigned DstBits = State.Regs.getClassWidth(Dst);
    if (!widenGeometry(SrcBits, DstBits))
      return fault(Name + ": a " + Twine(SrcBits) + "-bit access and " +
                   Twine(DstBits) + " accumulator bits are not bf16 lanes "
                   "widening to f32");
    APInt V;
    switch (Host.load(Addr, SrcBits / 8, V)) {
    case PortStatus::Stall:
      return StepResult::Stalled;
    case PortStatus::Fault:
      return fault(Name + ": no data memory at " + Twine::utohexstr(Addr));
    case PortStatus::Ok:
      break;
    }
    Eff.RegWrites.push_back({Dst, bf16Widen(V, DstBits), Op.cycleOf(DstIdx),
                             Op.fwdOf(DstIdx)});
    return StepResult::Retired;
  };

  // A vector store. Its width comes from the source register class, where the
  // scalar forms carry it in the opcode. Same late sampling as those: the
  // register is named, not read, and a composed source composes when the
  // pipeline reaches SampleAt.
  auto vectorStore = [&](unsigned SrcIdx, uint32_t Addr) -> StepResult {
    const MCRegister Src = Op.reg(SrcIdx);
    const unsigned W = State.Regs.getClassWidth(Src);
    if (!W || W % 8) {
      FaultMsg = (Name + ": " + MRI.getName(Src) + " is " + Twine(W) +
                  " bits, not a whole number of bytes")
                     .str();
      return StepResult::Fault;
    }
    Eff.MemWrites.push_back({Addr, W / 8, Src, Op.cycleOf(SrcIdx), Op.fwdOf(SrcIdx)});
    return StepResult::Retired;
  };

  // vpush.hi.N (d)(s1, s2): s1 shifted down one element, with s2 as the new
  // TOP element.
  //
  // The mnemonic says "push" but not toward which end, and the operand list
  // does not either. llvm-aie's own patterns settle it twice over
  // (aie2p/AIE2PInstrPatterns.td): the generic these select from is
  // G_AIE_ADD_VECTOR_ELT_HI, and VPUSH_hi_64 is the combine of
  // vshift(s0, broadcast(s1), 0, 8) -- a shift of exactly one 64-bit element
  // into the concatenation, whose 512-bit window is s0's elements 1.. followed
  // by s1. So the arriving element lands at the high end.
  //
  // The scalar is wider than the element on the 8- and 16-bit forms (an eR
  // holds 32 bits), and the pattern feeds an i32 into a v64i8, so it truncates.
  auto vpushHi = [&](unsigned ElemBits) -> StepResult {
    const MCRegister Dst = Op.reg(0);
    const unsigned W = State.Regs.getClassWidth(Dst);
    if (!W || W % ElemBits)
      return fault(Name + ": " + MRI.getName(Dst) + " is " + Twine(W) +
                   " bits, not a whole number of " + Twine(ElemBits) +
                   "-bit elements");
    const unsigned Lanes = W / ElemBits;
    const APInt Src = Op.valN(1, W);
    const APInt Top = Op.valN(2, ElemBits);
    if (!Op.Ok)
      return fault(Name + ": source is not readable");
    APInt Out(W, 0);
    for (unsigned I = 0; I + 1 != Lanes; ++I)
      Out.insertBits(Src.extractBits(ElemBits, (I + 1) * ElemBits),
                     I * ElemBits);
    Out.insertBits(Top, (Lanes - 1) * ElemBits);
    Eff.RegWrites.push_back({Dst, Out, Op.cycleOf(0), Op.fwdOf(0)});
    return StepResult::Retired;
  };

  // vmax_lt.N / vmin_ge.N (d, cmp)(s1, s2): the elementwise min or max, AND
  // the comparison that chose it. Two defs, which is why the operand list is
  // (d, cmp, s1, s2) -- confirmed against the itinerary, whose operand-cycle
  // list is [d, cmp, s1, s2] plus the implicit sign register.
  //
  // The suffix is the SIGNEDNESS, and it is a 1-bit register rather than an
  // operand: AIE2PRegisterInfo.td defines vaddSign0/vaddSign1 as AIE2P1BitReg
  // 0b0/0b1, and each form carries Uses = [vaddSign0] or [vaddSign1]. Which
  // one means signed is pinned by the standalone compares in
  // aie2p/AIE2PInstrPatterns.td: aie_setlt (SETLT) selects VLT_*_vaddSign1
  // where aie_setult (SETULT) selects VLT_*_vaddSign0. The bf16 forms carry no
  // sign register at all, and the same file says why -- "bf16 is always
  // signed".
  //
  // The mask polarity is the sibling's too, not a reading of the mnemonic:
  // VGE_16 alone is what (aie_setge $rs1, $rs2) selects, so `ge` in an AIE2P
  // mnemonic is s1 >= s2 with the operands in that order, and `lt` is s1 < s2.
  //
  // That makes the select fall out rather than be assumed: vmax_lt's mask is
  // s1 < s2, and the max is s2 exactly when s1 < s2; vmin_ge's mask is
  // s1 >= s2, and the min is s2 exactly when s1 >= s2. Both are the same
  // predicate -- "this lane's result came from s2" -- which is why LT pairs
  // with MAX and GE with MIN instead of arbitrarily, and why one subtract in
  // the vec_sub_min_max datapath yields both results.
  //
  // One mask bit per lane. The instruction ids say so: the .8 form is
  // vec_cmp_out_single64 into mL8m (the 64-bit l8), .16 is out_single32 and
  // .32 is out_single16, both into mR16_vcompare (the 32-bit r16).
  // \p Signed is meaningless when \p IsFloat: bf16 has no unsigned reading,
  // which is why those forms carry no sign register.
  auto minMax = [&](unsigned ElemBits, bool IsMax, bool Signed,
                    bool IsFloat = false) -> StepResult {
    const MCRegister Dst = Op.reg(0);
    const MCRegister Cmp = Op.reg(1);
    const unsigned W = State.Regs.getClassWidth(Dst);
    if (!W || W % ElemBits)
      return fault(Name + ": " + MRI.getName(Dst) + " is " + Twine(W) +
                   " bits, not a whole number of " + Twine(ElemBits) +
                   "-bit lanes");
    const unsigned Lanes = W / ElemBits;
    const unsigned CmpBits = State.Regs.getClassWidth(Cmp);
    if (CmpBits < Lanes)
      return fault(Name + ": " + MRI.getName(Cmp) + " holds " +
                   Twine(CmpBits) + " bits, too few for " + Twine(Lanes) +
                   " lanes");
    const APInt S1 = Op.valN(2, W);
    const APInt S2 = Op.valN(3, W);
    if (!Op.Ok)
      return fault(Name + ": source is not readable");

    APInt Out(W, 0);
    // Bits above the lane count are zeroed. Only the .32 form has any -- 16
    // lanes in a 32-bit r16 -- and nothing read so far says whether the
    // hardware drives them or holds them. Zeroing is the choice that cannot
    // pass off a stale value as a result.
    APInt Mask(CmpBits, 0);
    for (unsigned I = 0; I != Lanes; ++I) {
      const APInt A = S1.extractBits(ElemBits, I * ElemBits);
      const APInt B = S2.extractBits(ElemBits, I * ElemBits);
      bool TakeB;
      if (IsFloat) {
        const APFloat FA(APFloat::BFloat(), A);
        const APFloat FB(APFloat::BFloat(), B);
        if (FA.isNaN() || FB.isNaN())
          return fault(Name + ": lane " + Twine(I) +
                       " compares a NaN, and what this instruction does with "
                       "one is not established");
        TakeB = IsMax ? FA < FB : !(FA < FB);
      } else {
        TakeB = IsMax ? (Signed ? A.slt(B) : A.ult(B))
                      : (Signed ? A.sge(B) : A.uge(B));
      }
      Out.insertBits(TakeB ? B : A, I * ElemBits);
      Mask.setBitVal(I, TakeB);
    }
    Eff.RegWrites.push_back({Dst, Out, Op.cycleOf(0), Op.fwdOf(0)});
    Eff.RegWrites.push_back({Cmp, Mask, Op.cycleOf(1), Op.fwdOf(1)});
    return StepResult::Retired;
  };

  // vadd.N / vsub.N (d)(s1, s2): lane-wise, wrapping, and sign-agnostic.
  //
  // Both of those are read off the defs rather than assumed. These carry no
  // Uses, where vsub_lt/vsub_ge two entries below them in the same file take
  // crVaddSign and vpack above them takes crSat -- so there is no register
  // whose state could make them saturate or split by sign. And
  // AIE2InstrPatterns.td selects them for plain ISD::ADD/ISD::SUB on v64i8,
  // v32i16 and v16i32, which are modular two's complement; a saturating
  // datapath could not be the pattern for those nodes.
  //
  // Sign-agnostic is what makes one arm per width enough: wrapping add and
  // subtract are the same bit pattern signed or unsigned, which is why this
  // family has no vaddSign pair where the min/max family above it does.
  auto addSub = [&](unsigned ElemBits, bool IsSub) -> StepResult {
    const MCRegister Dst = Op.reg(0);
    const unsigned W = State.Regs.getClassWidth(Dst);
    if (!W || W % ElemBits)
      return fault(Name + ": " + MRI.getName(Dst) + " is " + Twine(W) +
                   " bits, not a whole number of " + Twine(ElemBits) +
                   "-bit lanes");
    const unsigned Lanes = W / ElemBits;
    const APInt S1 = Op.valN(1, W);
    const APInt S2 = Op.valN(2, W);
    if (!Op.Ok)
      return fault(Name + ": source is not readable");

    APInt Out(W, 0);
    for (unsigned I = 0; I != Lanes; ++I) {
      const APInt A = S1.extractBits(ElemBits, I * ElemBits);
      const APInt B = S2.extractBits(ElemBits, I * ElemBits);
      Out.insertBits(IsSub ? A - B : A + B, I * ElemBits);
    }
    Eff.RegWrites.push_back({Dst, Out, Op.cycleOf(0), Op.fwdOf(0)});
    return StepResult::Retired;
  };

  // vadd/vsub (dst)(acc1, acc2, conf) over a whole accumulator, and the .f
  // arms of the same. No multiply happens, so the conf's amode is the only
  // field that shapes the result: it names the accumulator lane width.
  //
  // bmode and cmode are deliberately NOT gated on. AIE2PInstrPatterns.td sets
  // cmode on accfp32_vecconf saying it "should be irrelevant for an
  // addition/substraction" and is there only "to silence our tools (e.g.
  // simulator)" -- which makes accfp32_vecconf and mulbf16_vecconf the same
  // 0x3c word. Demanding a multiply shape here would be reading padding as
  // meaning.
  //
  // The integer arms carry no Uses at all, where the vsub_lt neighbour takes
  // crVaddSign and vpack takes crSat, so they wrap and no lane sign reaches
  // them. The .f arms carry Uses = [crFPMask] and no crRnd, the vmac.f
  // situation: the mode is not selectable, and an f32 sum is ordinarily inexact
  // so this does not check for exactness the way the bf16 multiply does.
  //
  // dynZeroAccum is not merely unmodelled but unresolvable from the record:
  // "the first accumulator input to the post-adder" does not say whether that
  // is acc1 or acc2, and for a subtract the two answers differ in sign.
  auto accAddSub = [&](bool IsSub, bool IsFloat) -> StepResult {
    const uint32_t Conf = Op.val(3);
    if (!Op.Ok)
      return fault(Name + ": conf is not readable");
    // dynZeroAccum, accShift, dynMulNeg, dynAcc0Neg, dynAcc1Neg.
    const uint32_t Rewrites =
        (1u << 0) | (1u << 10) | (1u << 11) | (1u << 12) | (1u << 13);
    if (Conf & Rewrites)
      return fault(Name + ": conf " + Twine::utohexstr(Conf) +
                   " asks for a zero-accumulate, an accumulator shift or a "
                   "negation, and none of those is modelled");

    const unsigned AMode = (Conf >> 1) & 3;
    unsigned ElemBits = 0;
    if (IsFloat) {
      if (AMode != 2)
        return fault(Name + ": conf amode=" + Twine(AMode) +
                     " is not AMODE_FP32 on a float accumulator add");
      ElemBits = 32;
    } else if (AMode == 0) {
      ElemBits = 32;
    } else if (AMode == 1) {
      ElemBits = 64;
    } else {
      return fault(Name + ": conf amode=" + Twine(AMode) +
                   " is not an integer accumulator width");
    }

    const MCRegister Dst = Op.reg(0);
    const unsigned W = State.Regs.getClassWidth(Dst);
    if (!W || W % ElemBits)
      return fault(Name + ": " + MRI.getName(Dst) + " is " + Twine(W) +
                   " bits, not a whole number of " + Twine(ElemBits) +
                   "-bit lanes");
    const APInt S1 = Op.valN(1, W);
    const APInt S2 = Op.valN(2, W);
    if (!Op.Ok)
      return fault(Name + ": source is not readable");

    APInt Out(W, 0);
    for (unsigned I = 0; I != W / ElemBits; ++I) {
      const APInt A = S1.extractBits(ElemBits, I * ElemBits);
      const APInt B = S2.extractBits(ElemBits, I * ElemBits);
      if (!IsFloat) {
        Out.insertBits(IsSub ? A - B : A + B, I * ElemBits);
        continue;
      }
      APFloat FA(APFloat::IEEEsingle(), A);
      APFloat FB(APFloat::IEEEsingle(), B);
      if (IsSub)
        FA.subtract(FB, APFloat::rmNearestTiesToEven);
      else
        FA.add(FB, APFloat::rmNearestTiesToEven);
      Out.insertBits(FA.bitcastToAPInt(), I * ElemBits);
    }
    Eff.RegWrites.push_back({Dst, Out, Op.cycleOf(0), Op.fwdOf(0)});
    return StepResult::Retired;
  };

  // vsel.N (d)(s1, s2, sel): one bit of \p sel per lane picks that lane's
  // source. Set takes s2, clear takes s1.
  //
  // That polarity is the compiler's, not a reading of the mnemonic.
  // AIE2InstrPatterns.td lowers a vector (select cond, $rs2, $rs3) to
  // VSEL_32($rs2, $rs3, cond - 1) with cond zero-extended from one bit: a true
  // cond makes sel 0 and must yield $rs2, the FIRST source, and a false cond
  // makes sel all ones and must yield $rs3.
  //
  // Bit I is lane I, which the min/max family above already writes -- the
  // compare masks it produces land in the same eL / eRS8 classes vsel consumes
  // here, at the same lane counts, and mean the same thing: this lane came
  // from s2. So a vmax_lt mask fed straight back into vsel reproduces the max.
  auto vsel = [&](unsigned ElemBits) -> StepResult {
    const MCRegister Dst = Op.reg(0);
    const MCRegister Sel = Op.reg(3);
    const unsigned W = State.Regs.getClassWidth(Dst);
    if (!W || W % ElemBits)
      return fault(Name + ": " + MRI.getName(Dst) + " is " + Twine(W) +
                   " bits, not a whole number of " + Twine(ElemBits) +
                   "-bit lanes");
    const unsigned Lanes = W / ElemBits;
    const unsigned SelBits = State.Regs.getClassWidth(Sel);
    if (SelBits < Lanes)
      return fault(Name + ": " + MRI.getName(Sel) + " holds " + Twine(SelBits) +
                   " bits, too few for " + Twine(Lanes) + " lanes");
    const APInt S1 = Op.valN(1, W);
    const APInt S2 = Op.valN(2, W);
    const APInt M = Op.valN(3, SelBits);
    if (!Op.Ok)
      return fault(Name + ": source is not readable");

    // Bits of sel above the lane count are ignored rather than faulted on: the
    // .32 form has 16 of them in a 32-bit r16, and its natural producer --
    // vmax_lt.32 -- leaves exactly those zeroed, so a fault there would reject
    // the pairing the two instructions exist for.
    APInt Out(W, 0);
    for (unsigned I = 0; I != Lanes; ++I)
      Out.insertBits((M[I] ? S2 : S1).extractBits(ElemBits, I * ElemBits),
                     I * ElemBits);
    Eff.RegWrites.push_back({Dst, Out, Op.cycleOf(0), Op.fwdOf(0)});
    return StepResult::Retired;
  };

  // vshuffle (dst)(s1, s2, mod): read the 1024-bit pair (s1 low, s2 high) as a
  // row-major R x C matrix of W-bit elements, transpose it, and write one
  // 512-bit half. The mode NAMES the shape -- aie2p_enums.h spells all three
  // parts as T<W>_<R>x<C>_<lo|hi> -- and for the twelve _lo/_hi pairs below
  // R * C is exactly 1024 / W, so the matrix tiles the pair with nothing over.
  //
  // Element order is pinned by two statements rather than chosen. AIELegalizer-
  // Helper says mode 8 "maps first 128-bits of src1 vector to lsb positions of
  // output" and mode 9 the second: with row-major indices read out
  // column-major, T128_4x2_lo does put element 0 lowest and _hi element 1.
  // Independently, T512_1x2 is aie_api's bypass mode, which forces lo == s1 and
  // hi == s2, and it does. Across all twelve pairs lo and hi together are a
  // permutation of the pair, so no element is dropped or doubled.
  //
  // Modes 24..55 -- the NL, FFT/sparsity and permute-reduction shapes -- are
  // NOT extrapolated from this. Their names carry an R x C that does not tile
  // the pair (T16_4x4 is 16 elements where the pair holds 64), so they are a
  // different construction, and they fault carrying the mode number.
  auto vshuffle = [&]() -> StepResult {
    struct Shape {
      unsigned W, R, C;
    };
    static const Shape Shapes[] = {
        {8, 64, 2},  {16, 32, 2}, {32, 16, 2}, {64, 8, 2},
        {128, 4, 2}, {256, 2, 2}, {128, 2, 4}, {64, 2, 8},
        {32, 2, 16}, {16, 2, 32}, {8, 2, 64},  {512, 1, 2},
    };
    const uint32_t Mode = Op.val(3);
    if (!Op.Ok)
      return fault(Name + ": mode is not readable");
    if (Mode >= 2 * std::size(Shapes))
      return fault(Name + ": mode " + Twine(Mode) +
                   " is not one of the transpose shapes this models (0..23)");
    const Shape &S = Shapes[Mode / 2];
    const bool Hi = Mode & 1;

    const MCRegister Dst = Op.reg(0);
    const unsigned DstBits = State.Regs.getClassWidth(Dst);
    const unsigned SrcBits = State.Regs.getClassWidth(Op.reg(1));
    if (DstBits != 512 || SrcBits != 512)
      return fault(Name + ": " + Twine(SrcBits) + " source bits into " +
                   Twine(DstBits) + " is not the 512-bit shuffle this models");
    const APInt A = Op.valN(1, 512);
    const APInt B = Op.valN(2, 512);
    if (!Op.Ok)
      return fault(Name + ": source is not readable");
    APInt Pair(1024, 0);
    Pair.insertBits(A, 0);
    Pair.insertBits(B, 512);

    const unsigned N = S.R * S.C;
    APInt Out(512, 0);
    for (unsigned J = 0; J != N / 2; ++J) {
      const unsigned Pos = Hi ? J + N / 2 : J;
      const unsigned Src = (Pos % S.R) * S.C + Pos / S.R;
      Out.insertBits(Pair.extractBits(S.W, Src * S.W), J * S.W);
    }
    Eff.RegWrites.push_back({Dst, Out, Op.cycleOf(0), Op.fwdOf(0)});
    return StepResult::Retired;
  };

  // vextract.N (dst)(s1, idx): one lane into a scalar register, widened.
  //
  // The vaddSign suffix means here what it means on the min/max family, and
  // this is where it is stated outright rather than inferred: the
  // target-generic Extract_512 multiclass in AIEBaseInstrPatterns.td takes
  // (UnsignedOpc, SignedOpc) and binds vextract_zext to the first and
  // vextract_sext to the second, and aie2p passes vaddSign0 then vaddSign1.
  // AIE2 instantiates the same multiclass with VEXTRACT_D8 then VEXTRACT_S8,
  // so a second target's naming lands on the same two slots.
  //
  // The index is a lane, not a byte -- it is c6u on the imm forms, six bits,
  // which is exactly the 64 lanes of the .8 form and more than the others need.
  auto vextract = [&](unsigned ElemBits, bool Signed) -> StepResult {
    const MCRegister Dst = Op.reg(0);
    const MCRegister Src = Op.reg(1);
    const unsigned DstBits = State.Regs.getClassWidth(Dst);
    const unsigned W = State.Regs.getClassWidth(Src);
    if (!W || W % ElemBits)
      return fault(Name + ": " + MRI.getName(Src) + " is " + Twine(W) +
                   " bits, not a whole number of " + Twine(ElemBits) +
                   "-bit lanes");
    const unsigned Lanes = W / ElemBits;
    const MCOperand &Idx = MI.getOperand(2);
    const uint64_t I = Idx.isImm() ? uint64_t(Idx.getImm()) : Op.val(2);
    const APInt V = Op.valN(1, W);
    if (!Op.Ok)
      return fault(Name + ": source is not readable");
    // The register forms take a whole eR, so an out-of-range lane is
    // reachable at runtime rather than only by a malformed encoding. What the
    // hardware does with one is not established, so it faults.
    if (I >= Lanes)
      return fault(Name + ": lane " + Twine(I) + " is outside the " +
                   Twine(Lanes) + " this source holds");
    const APInt E = V.extractBits(ElemBits, unsigned(I) * ElemBits);
    Eff.RegWrites.push_back(
        {Dst, Signed ? E.sext(DstBits) : E.zext(DstBits), Op.cycleOf(0), Op.fwdOf(0)});
    return StepResult::Retired;
  };

  // vinsert.N (dst)(s1, [idx,] src): s1 with the lane at idx replaced, which
  // makes it vextract's inverse and gives each one an oracle for the other.
  //
  // Which operand is the index and which the value is not readable off the asm
  // string -- it spells them in the same order either way -- so it comes from
  // the pattern. AIEBaseInstrPatterns.td's Insert512Pat rewrites the generic
  // (vinsert $src1, Val, Idx) to (Inst $src1, Idx, Val), so the LAST operand
  // is the value and the middle one the index. That vinsert node is declared
  // SDTCisSameAs<0,1> against $src1, which is the other half of the semantics:
  // every lane the index does not name is s1's, unchanged.
  //
  // \p IdxIdx is -1 on the mIdxImm0 form, whose lane is 0 by construction and
  // not by default -- Insert512Imm0Pat matches a literal 0 and drops the index
  // operand, so the encoding has nowhere to put another lane.
  auto vinsert = [&](unsigned ElemBits, int IdxIdx,
                     unsigned SrcIdx) -> StepResult {
    const MCRegister Dst = Op.reg(0);
    const unsigned W = State.Regs.getClassWidth(Dst);
    if (!W || W % ElemBits)
      return fault(Name + ": " + MRI.getName(Dst) + " is " + Twine(W) +
                   " bits, not a whole number of " + Twine(ElemBits) +
                   "-bit lanes");
    const unsigned Lanes = W / ElemBits;
    const uint64_t I = IdxIdx < 0 ? 0 : Op.val(unsigned(IdxIdx));
    const APInt V = Op.valN(1, W);
    const APInt E = Op.valN(SrcIdx, ElemBits);
    if (!Op.Ok)
      return fault(Name + ": source is not readable");
    // Out of range faults rather than wrapping, the same stance vextract takes
    // and for the same reason: r29 holds a whole word here too.
    if (I >= Lanes)
      return fault(Name + ": lane " + Twine(I) + " is outside the " +
                   Twine(Lanes) + " this destination holds");
    APInt Out = V;
    Out.insertBits(E, unsigned(I) * ElemBits);
    Eff.RegWrites.push_back({Dst, Out, Op.cycleOf(0), Op.fwdOf(0)});
    return StepResult::Retired;
  };

  // vextbcst.N (mXm)(s1, idx): the lane at idx repeated across the whole
  // destination -- vextract's lane select feeding vbcst's splat, in one
  // instruction and without the trip through a scalar register.
  //
  // It reuses vextract's index convention rather than assuming one: idx is
  // c6u on the imm forms here too, and six bits is a lane number for every
  // width in the family (the .16 form has 32 lanes) but not a byte offset for
  // any of them. Unlike vextract there is no vaddSign pair, which follows --
  // the destination lane is the same width as the source lane, so there is
  // nothing to widen and no sign to choose.
  auto vextractBroadcast = [&](unsigned ElemBits) -> StepResult {
    const MCRegister Dst = Op.reg(0);
    const MCRegister Src = Op.reg(1);
    const unsigned DstW = State.Regs.getClassWidth(Dst);
    const unsigned W = State.Regs.getClassWidth(Src);
    if (!W || W % ElemBits)
      return fault(Name + ": " + MRI.getName(Src) + " is " + Twine(W) +
                   " bits, not a whole number of " + Twine(ElemBits) +
                   "-bit lanes");
    if (!DstW || DstW % ElemBits)
      return fault(Name + ": " + MRI.getName(Dst) + " is " + Twine(DstW) +
                   " bits, not a whole number of " + Twine(ElemBits) +
                   "-bit lanes");
    const unsigned Lanes = W / ElemBits;
    const MCOperand &Idx = MI.getOperand(2);
    const uint64_t I = Idx.isImm() ? uint64_t(Idx.getImm()) : Op.val(2);
    const APInt V = Op.valN(1, W);
    if (!Op.Ok)
      return fault(Name + ": source is not readable");
    // Same stance as vextract: the register forms take a whole eR, so an
    // out-of-range lane is reachable at runtime and what the hardware does
    // with one is not established.
    if (I >= Lanes)
      return fault(Name + ": lane " + Twine(I) + " is outside the " +
                   Twine(Lanes) + " this source holds");
    const APInt E = V.extractBits(ElemBits, unsigned(I) * ElemBits);
    Eff.RegWrites.push_back(
        {Dst, APInt::getSplat(DstW, E), Op.cycleOf(0), Op.fwdOf(0)});
    return StepResult::Retired;
  };

  // vshift (d)(s1, s2, shift): the 512-bit window of the 1024-bit
  // concatenation {s2 : s1} starting at BYTE offset shift, s1 low.
  //
  // Neither the width of the shift unit nor which operand is the low half is
  // in the operand list, and both are settled without guessing by an opcode
  // this model already executes and tests. AIE2PInstrPatterns.td combines
  // vshift(s0, bcst(s1), 0x0, 0x8) into VPUSH_hi_64, whose behaviour is
  // s0's elements 1.. followed by s1. That is only true of this reading: 8
  // means eight BYTES, which is one 64-bit element, and s1 has to be the LOW
  // half for the discarded element to come off its bottom.
  auto vshift = [&]() -> StepResult {
    const MCRegister Dst = Op.reg(0);
    const unsigned W = State.Regs.getClassWidth(Dst);
    if (!W || W % 8)
      return fault(Name + ": " + MRI.getName(Dst) + " is " + Twine(W) +
                   " bits, not a whole number of bytes");
    const APInt Lo = Op.valN(1, W);
    const APInt Hi = Op.valN(2, W);
    const uint32_t Shift = Op.val(3);
    if (!Op.Ok)
      return fault(Name + ": source is not readable");
    const unsigned Bytes = W / 8;
    // A shift of exactly Bytes is the whole of s2 and still a window; past
    // that the window leaves the concatenation, and what the hardware masks
    // the count to is not established.
    if (Shift > Bytes)
      return fault(Name + ": shift of " + Twine(Shift) + " bytes leaves the " +
                   Twine(2 * Bytes) + "-byte concatenation");
    const APInt Cat = Lo.zext(2 * W) | Hi.zext(2 * W).shl(W);
    Eff.RegWrites.push_back(
        {Dst, Cat.lshr(Shift * 8).trunc(W), Op.cycleOf(0), Op.fwdOf(0)});
    return StepResult::Retired;
  };

  // vbcst.N (mXm)(src): the low N bits of the source repeated across the
  // destination vector. The destination is a composed register, so these are
  // the writes that have to split across leaves to land at all.
  auto broadcast = [&](unsigned LaneBits) -> StepResult {
    const MCRegister Dst = Op.reg(0);
    // From the register class; a literal 512 would be a second, unchecked
    // copy of the class width.
    const unsigned W = State.Regs.getClassWidth(Dst);
    if (!W || W % LaneBits) {
      FaultMsg = (Name + ": " + MRI.getName(Dst) + " is " + Twine(W) +
                  " bits, not a whole number of " + Twine(LaneBits) + "-bit lanes")
                     .str();
      return StepResult::Fault;
    }
    Eff.RegWrites.push_back(
        {Dst, APInt::getSplat(W, Op.valN(1, LaneBits)), Op.cycleOf(0), Op.fwdOf(0)});
    return StepResult::Retired;
  };

  StepResult R = StepResult::Retired;

  switch (Opc) {
  default:
    FaultMsg = (Name + ": no semantics").str();
    return StepResult::Fault;

  case AIE2P::NOP:
  case AIE2P::NOPA:
  case AIE2P::NOPB:
  case AIE2P::NOPM:
  case AIE2P::NOPS:
  case AIE2P::NOPV:
  case AIE2P::NOPX:
  case AIE2P::NOPXM:
    break;

  // Register-register ALU, all (d0)(s0, s1).
  case AIE2P::ADD_alu_r_rr:
    Def32(0, Op.val(1) + Op.val(2));
    break;
  case AIE2P::SUB:
    Def32(0, Op.val(1) - Op.val(2));
    break;
  case AIE2P::MUL:
    Def32(0, Op.val(1) * Op.val(2));
    break;
  // The scalar multiply-accumulate. AIE2PInstrPatterns selects mac from
  // (add $rd, (mul $rs1, $rs2)) and msc from the sub of the same, both at i32
  // and both through plain DAG nodes, which are modular two's complement -- so
  // this wraps for the reason MUL above it does, and needs no lane sign: at a
  // truncated 32 bits the signed and unsigned products share their low half.
  //
  // Four operands where the encoding carries three registers. $a0 is tied to
  // $d0, and the decoder materialises it rather than dropping it: msc r3, r3,
  // r4, r5 decodes to four MCOperands with the first two both r3.
  case AIE2P::MAC:
    Def32(0, Op.val(1) + Op.val(2) * Op.val(3));
    break;
  case AIE2P::MSC:
    Def32(0, Op.val(1) - Op.val(2) * Op.val(3));
    break;
  case AIE2P::AND:
    Def32(0, Op.val(1) & Op.val(2));
    break;
  case AIE2P::OR:
    Def32(0, Op.val(1) | Op.val(2));
    break;
  case AIE2P::XOR:
    Def32(0, Op.val(1) ^ Op.val(2));
    break;
  case AIE2P::EQ:
    Def32(0, Op.val(1) == Op.val(2));
    break;
  case AIE2P::NE:
    Def32(0, Op.val(1) != Op.val(2));
    break;
  case AIE2P::LT:
    Def32(0, int32_t(Op.val(1)) < int32_t(Op.val(2)));
    break;
  case AIE2P::GE:
    Def32(0, int32_t(Op.val(1)) >= int32_t(Op.val(2)));
    break;
  case AIE2P::LTU:
    Def32(0, Op.val(1) < Op.val(2));
    break;
  case AIE2P::GEU:
    Def32(0, Op.val(1) >= Op.val(2));
    break;

  // A negative shift amount is how the backend spells a right shift:
  // AIE2PInstrPatterns.td turns srl/sra into LSHL/ASHL of a negated count.
  case AIE2P::LSHL: {
    const int32_t Amount = int32_t(Op.val(2));
    const uint32_t Value = Op.val(1);
    Def32(0, Amount >= 0 ? Value << (Amount & 31) : Value >> (-Amount & 31));
    break;
  }
  case AIE2P::ASHL: {
    const int32_t Amount = int32_t(Op.val(2));
    const int32_t Value = int32_t(Op.val(1));
    Def32(0, Amount >= 0 ? uint32_t(Value) << (Amount & 31)
                         : uint32_t(Value >> (-Amount & 31)));
    break;
  }

  // Register-immediate ALU, all (d0)(s0, imm).
  case AIE2P::ADD_add_r_ri:
  case AIE2P::ADD_NC_mv_add_ri:
    Def32(0, Op.val(1) + uint32_t(Op.imm(2)));
    break;
  case AIE2P::ADD_NC_mv_add_rr:
    Def32(0, Op.val(1) + Op.val(2));
    break;

  // Unary, all (d0)(s0).
  case AIE2P::ABS: {
    const int32_t Value = int32_t(Op.val(1));
    Def32(0, Value < 0 ? -uint32_t(Value) : uint32_t(Value));
    break;
  }
  case AIE2P::CLZ:
    Def32(0, llvm::countl_zero(Op.val(1)));
    break;
  case AIE2P::CLB:
    Def32(0, countLeadingSignBits(Op.val(1)));
    break;
  case AIE2P::POPCOUNT:
    Def32(0, llvm::popcount(Op.val(1)));
    break;
  case AIE2P::EQZ:
    Def32(0, Op.val(1) == 0);
    break;
  case AIE2P::NEZ:
    Def32(0, Op.val(1) != 0);
    break;
  case AIE2P::EXTEND_s8:
    Def32(0, uint32_t(SignExtend32<8>(Op.val(1))));
    break;
  case AIE2P::EXTEND_s16:
    Def32(0, uint32_t(SignExtend32<16>(Op.val(1))));
    break;
  case AIE2P::EXTEND_u8:
    Def32(0, Op.val(1) & 0xff);
    break;
  case AIE2P::EXTEND_u16:
    Def32(0, Op.val(1) & 0xffff);
    break;

  // (d0)(s0, s1, r27), selecting on the implicit condition register.
  case AIE2P::SEL_EQZ:
    Def32(0, Op.val(3) == 0 ? Op.val(1) : Op.val(2));
    break;
  case AIE2P::SEL_NEZ:
    Def32(0, Op.val(3) != 0 ? Op.val(1) : Op.val(2));
    break;

  // The four spellings AIE2PMultiSlotPseudoInstrInfo.td groups under
  // mov_rlc_imm11_pseudo: one immediate move, four encodings and issue slots.
  case AIE2P::MOVA:
  case AIE2P::MOVX_alu_cg:
  case AIE2P::MOVXM:
  case AIE2P::MOV_alu_mv_mv_mv_cg:
    Def32(0, uint32_t(Op.imm(1)));
    break;
  // movs is the same copy across the address-generation register classes,
  // which is how a loop index reaches dj before an indexed access uses it.
  case AIE2P::MOVS:
  case AIE2P::MOV_alu_mv_mv_mv_scl:
    Def32(0, Op.val(1));
    break;

  // Control transfers take effect after five delay slots.
  case AIE2P::J_lng:
    Eff.Branch = uint64_t(Op.imm(0));
    break;
  case AIE2P::J_alumv_or:
    Eff.Branch = Op.val(0);
    break;
  // jl: same addressing as j, plus Eff.Link so the executor writes lr with
  // the return address once the delay slots retire (AIEExecutor::advancePC)
  // -- not here, since that address depends on the byte size of bundles
  // this instruction has not fetched yet.
  case AIE2P::JL_lng:
    Eff.Branch = uint64_t(Op.imm(0));
    Eff.Link = true;
    break;
  case AIE2P::JL_alumv_or:
    Eff.Branch = Op.val(0);
    Eff.Link = true;
    break;
  // ret lr: lr is an implicit use (asm string hardcodes "lr", no operand),
  // so it is read directly rather than through Op.val(), which indexes
  // MI's operand list.
  case AIE2P::RET: {
    APInt V;
    if (!State.Regs.read(AIE2P::lr, V, State.Cycle + 1)) {
      FaultMsg = (Name + ": lr has no value to return to").str();
      return StepResult::Fault;
    }
    Eff.Branch = V.zextOrTrunc(64).getZExtValue();
    break;
  }
  case AIE2P::JZ:
    if (Op.val(0) == 0)
      Eff.Branch = uint64_t(Op.imm(1));
    break;
  case AIE2P::JNZ:
    if (Op.val(0) != 0)
      Eff.Branch = uint64_t(Op.imm(1));
    break;
  // The compare happens before the decrement, so the counter carries the
  // backedge-taken count (AIEBaseHardwareLoops.cpp file comment).
  case AIE2P::JNZD: {
    const uint32_t Count = Op.val(1);
    if (Count != 0)
      Eff.Branch = Op.val(2);
    Def32(0, Count - 1);
    break;
  }

  // Pointer arithmetic, (ptr_out)(ptr, imm).
  case AIE2P::PADDA_pstm_nrm_imm:
  case AIE2P::PADDB_pstm_nrm_imm:
  case AIE2P::PADDS_pstm_nrm_imm:
    DefAddr(0, access(1, Op.imm(2)));
    break;

  // Same, with the increment in an m register. What an m register holds in
  // this mode is a plain signed BYTE COUNT, which is worth citing rather than
  // assuming: "modifier" is also the name of a field inside the 2D descriptor
  // (AIE2InstructionSelector::createDRegSequence builds a d register as
  // sub_mod + sub_dim_size + sub_dim_stride + sub_dim_count), so the name on
  // its own suggests something structured. It is not. In
  // test/CodeGen/AIE/dyn-stackalloc.ll the compiler lowers `alloca i32, %n` to
  //
  //     lshl r0, r0, r1 ; add r0, r0, #63 ; and r0, r0, r1   (r1 = -64)
  //     mov m0, r0
  //     padda [p1], m0
  //
  // -- m0 is n*4 rounded up to 64, a byte count with fully traceable
  // provenance -- and `padda [p7], #-64` in the same block does the identical
  // job with an immediate. The imm and register forms are one operation.
  //
  // A negative increment survives the 20-bit register: -64 is stored as
  // 0xFFFC0 and access() masks the sum back to DataAddrBits, so modular
  // arithmetic gives p - 64 without sign-extending anything.
  case AIE2P::PADDA_pstm_nrm:
  case AIE2P::PADDB_pstm_nrm:
  case AIE2P::PADDS_pstm_nrm:
    DefAddr(0, access(1, int32_t(Op.val(2))));
    break;

  // (ptr_out, dc)(ptr, d): one step of a 2D walk.
  case AIE2P::PADDA_2D:
  case AIE2P::PADDB_2D:
  case AIE2P::PADDS_2D:
    DefAddr(0, step2D(Op.val(2), Op.reg(3), 1));
    break;

  // (ptr_out, dcl, dch)(ptr, ds): 3D carries a SECOND counter output, so ptr
  // and the descriptor sit one later again.
  case AIE2P::PADDA_3D:
  case AIE2P::PADDB_3D:
  case AIE2P::PADDS_3D:
    DefAddr(0, step3D(Op.val(3), Op.reg(4), 1, 2));
    break;

  // (dst)(ptr, imm): load from ptr + imm, pointer unchanged.
  case AIE2P::LDA_dms_lda_idx_imm:
    R = scalarLoad(0, access(1, Op.imm(2)), 4, /*Signed=*/false);
    break;
  // ()(imm): the frame adjust the prologue and epilogue emit. sp is implicit
  // (Defs/Uses in the def, absent from the asm operands), so it is read and
  // written by name rather than through Op.
  case AIE2P::PADDXM_pstm_sp_imm:
    // sp is implicit here, so it has no operand cycle of its own; +1 matches
    // the read side's assumption for implicit registers.
    Eff.RegWrites.push_back({AIE2P::sp, APInt(DataAddrBits, spAccess(Op.imm(0))),
                             State.Cycle + 1});
    break;

  // (dst)(imm): sp-relative spill restore. What ST_R_SPILL/LDA_R_SPILL
  // (aie2p/AIE2PInstrInfo.td's Pseudo spill forms) actually expand to
  // (AIE2PInstrInfo.cpp's getSpillPseudoExpandInfoByOpcode) -- sp is fixed
  // in the encoding, not a $ptr operand, unlike the general idx_imm forms.
  case AIE2P::LDA_dms_lda_spill:
    R = scalarLoad(0, spAccess(Op.imm(1)), 4, /*Signed=*/false);
    break;
  case AIE2P::LDA_s8_idx_imm:
    R = scalarLoad(0, access(1, Op.imm(2)), 1, /*Signed=*/true);
    break;
  case AIE2P::LDA_u8_idx_imm:
    R = scalarLoad(0, access(1, Op.imm(2)), 1, /*Signed=*/false);
    break;
  case AIE2P::LDA_s16_idx_imm:
    R = scalarLoad(0, access(1, Op.imm(2)), 2, /*Signed=*/true);
    break;
  case AIE2P::LDA_u16_idx_imm:
    R = scalarLoad(0, access(1, Op.imm(2)), 2, /*Signed=*/false);
    break;

  // (dst)(ptr, dj): same loads with the offset in a register instead of the
  // encoding. No pstm in the name and no ptr_out in the outs list, so the
  // pointer is unchanged here too. dj is a byte offset, not an element index.
  case AIE2P::LDA_dms_lda_idx:
    R = scalarLoad(0, access(1, int32_t(Op.val(2))), 4, /*Signed=*/false);
    break;
  case AIE2P::LDA_s8_idx:
    R = scalarLoad(0, access(1, int32_t(Op.val(2))), 1, /*Signed=*/true);
    break;
  case AIE2P::LDA_u8_idx:
    R = scalarLoad(0, access(1, int32_t(Op.val(2))), 1, /*Signed=*/false);
    break;
  case AIE2P::LDA_s16_idx:
    R = scalarLoad(0, access(1, int32_t(Op.val(2))), 2, /*Signed=*/true);
    break;
  case AIE2P::LDA_u16_idx:
    R = scalarLoad(0, access(1, int32_t(Op.val(2))), 2, /*Signed=*/false);
    break;

  // (dst, ptr_out)(ptr, imm): load from ptr, then advance the pointer.
  case AIE2P::LDA_dms_lda_pstm_nrm_imm:
    R = scalarLoad(0, access(2, 0), 4, /*Signed=*/false);
    DefAddr(1, access(2, Op.imm(3)));
    break;
  // (dst, ptr_out)(ptr, m): the same, incremented from a register.
  case AIE2P::LDA_dms_lda_pstm_nrm:
    R = scalarLoad(0, access(2, 0), 4, /*Signed=*/false);
    DefAddr(1, access(2, int32_t(Op.val(3))));
    break;

  // The narrow post-increment loads. Same operand shape as the 32-bit pair
  // above -- checked per form rather than assumed from the naming -- so only
  // the access width and signedness differ. The increment is a byte count on
  // all of them, including the c5s_step2 forms whose encoding is scaled.
  case AIE2P::LDA_s8_pstm_nrm_imm:
    R = scalarLoad(0, access(2, 0), 1, /*Signed=*/true);
    DefAddr(1, access(2, Op.imm(3)));
    break;
  case AIE2P::LDA_u8_pstm_nrm_imm:
    R = scalarLoad(0, access(2, 0), 1, /*Signed=*/false);
    DefAddr(1, access(2, Op.imm(3)));
    break;
  case AIE2P::LDA_s16_pstm_nrm_imm:
    R = scalarLoad(0, access(2, 0), 2, /*Signed=*/true);
    DefAddr(1, access(2, Op.imm(3)));
    break;
  case AIE2P::LDA_u16_pstm_nrm_imm:
    R = scalarLoad(0, access(2, 0), 2, /*Signed=*/false);
    DefAddr(1, access(2, Op.imm(3)));
    break;
  case AIE2P::LDA_s8_pstm_nrm:
    R = scalarLoad(0, access(2, 0), 1, /*Signed=*/true);
    DefAddr(1, access(2, int32_t(Op.val(3))));
    break;
  case AIE2P::LDA_u8_pstm_nrm:
    R = scalarLoad(0, access(2, 0), 1, /*Signed=*/false);
    DefAddr(1, access(2, int32_t(Op.val(3))));
    break;
  case AIE2P::LDA_s16_pstm_nrm:
    R = scalarLoad(0, access(2, 0), 2, /*Signed=*/true);
    DefAddr(1, access(2, int32_t(Op.val(3))));
    break;
  case AIE2P::LDA_u16_pstm_nrm:
    R = scalarLoad(0, access(2, 0), 2, /*Signed=*/false);
    DefAddr(1, access(2, int32_t(Op.val(3))));
    break;

  // ()(src, ptr, imm)
  case AIE2P::ST_dms_sts_idx_imm:
    scalarStore(0, access(1, Op.imm(2)), 4);
    break;
  // ()(src, imm): sp-relative spill, same fixed-sp shape as the LDA form.
  case AIE2P::ST_dms_sts_spill:
    scalarStore(0, spAccess(Op.imm(1)), 4);
    break;
  case AIE2P::ST_s8_idx_imm:
    scalarStore(0, access(1, Op.imm(2)), 1);
    break;
  case AIE2P::ST_s16_idx_imm:
    scalarStore(0, access(1, Op.imm(2)), 2);
    break;

  // ()(src, ptr, dj): the register-offset stores, matching the loads above.
  case AIE2P::ST_dms_sts_idx:
    scalarStore(0, access(1, int32_t(Op.val(2))), 4);
    break;
  case AIE2P::ST_s8_idx:
    scalarStore(0, access(1, int32_t(Op.val(2))), 1);
    break;
  case AIE2P::ST_s16_idx:
    scalarStore(0, access(1, int32_t(Op.val(2))), 2);
    break;

  // (ptr_out)(src, ptr, imm)
  case AIE2P::ST_dms_sts_pstm_nrm_imm:
    scalarStore(1, access(2, 0), 4);
    DefAddr(0, access(2, Op.imm(3)));
    break;
  // (ptr_out)(src, ptr, m)
  case AIE2P::ST_dms_sts_pstm_nrm:
    scalarStore(1, access(2, 0), 4);
    DefAddr(0, access(2, int32_t(Op.val(3))));
    break;

  // The narrow post-increment stores, matching the loads. No unsigned forms
  // here -- a store does not care -- so the family is half the size.
  case AIE2P::ST_s8_pstm_nrm_imm:
    scalarStore(1, access(2, 0), 1);
    DefAddr(0, access(2, Op.imm(3)));
    break;
  case AIE2P::ST_s16_pstm_nrm_imm:
    scalarStore(1, access(2, 0), 2);
    DefAddr(0, access(2, Op.imm(3)));
    break;
  case AIE2P::ST_s8_pstm_nrm:
    scalarStore(1, access(2, 0), 1);
    DefAddr(0, access(2, int32_t(Op.val(3))));
    break;
  case AIE2P::ST_s16_pstm_nrm:
    scalarStore(1, access(2, 0), 2);
    DefAddr(0, access(2, int32_t(Op.val(3))));
    break;
  // (dst)(ptr, imm): vector loads, same addressing as the scalar idx_imm
  // forms. The x form is 512 bits into a composed register, the w form 256
  // into one that has its own storage, so between them they cover both paths
  // a vector destination can take.
  //
  // vldb is the same load on the other port, and the port is not architectural
  // state: mWa and mWb are declared over the same four registers, mXa and mXb
  // over the same mXm. Operands match vlda position for position, immediate
  // types included, so the scaling here is vlda's and not a fresh assumption.
  // Spills are port A only, so the family has no spill form.
  case AIE2P::VLDA_dmx_lda_x_idx_imm:
  case AIE2P::VLDA_dmw_lda_w_idx_imm:
  case AIE2P::VLDA_dmx_lda_bm_idx_imm:
  case AIE2P::VLDB_dmx_ldb_x_idx_imm:
  case AIE2P::VLDB_dmw_ldb_idx_imm:
    R = vectorLoad(0, access(1, Op.imm(2)));
    break;

  // (dst)(ptr, dj): the offset in a register instead of the encoding. dj is a
  // byte offset, and the pointer is unchanged -- no pstm in the name and no
  // ptr_out in the outs list.
  case AIE2P::VLDA_dmx_lda_x_idx:
  case AIE2P::VLDA_dmw_lda_w_idx:
  case AIE2P::VLDA_dmx_lda_bm_idx:
  case AIE2P::VLDB_dmx_ldb_x_idx:
  case AIE2P::VLDB_dmw_ldb_idx:
    R = vectorLoad(0, access(1, int32_t(Op.val(2))));
    break;

  // (dst, ptr_out)(ptr, imm): load from ptr, then advance the pointer.
  case AIE2P::VLDA_dmx_lda_x_pstm_nrm_imm:
  case AIE2P::VLDA_dmw_lda_w_pstm_nrm_imm:
  case AIE2P::VLDA_dmx_lda_bm_pstm_nrm_imm:
  case AIE2P::VLDB_dmx_ldb_x_pstm_nrm_imm:
  case AIE2P::VLDB_dmw_ldb_pstm_nrm_imm:
    R = vectorLoad(0, access(2, 0));
    DefAddr(1, access(2, Op.imm(3)));
    break;

  // (dst)(imm): sp-relative, sp fixed in the encoding rather than an operand.
  case AIE2P::VLDA_dmx_lda_x_spill:
  case AIE2P::VLDA_dmw_lda_w_spill:
  case AIE2P::VLDA_dmx_lda_bm_spill:
    R = vectorLoad(0, spAccess(Op.imm(1)));
    break;

  // ()(src, ptr, imm): the matching vector stores.
  case AIE2P::VST_dmx_sts_x_idx_imm:
  case AIE2P::VST_dmw_sts_w_idx_imm:
  case AIE2P::VST_dmx_sts_bm_idx_imm:
    R = vectorStore(0, access(1, Op.imm(2)));
    break;

  // ()(src, ptr, dj)
  case AIE2P::VST_dmx_sts_x_idx:
  case AIE2P::VST_dmw_sts_w_idx:
  case AIE2P::VST_dmx_sts_bm_idx:
    R = vectorStore(0, access(1, int32_t(Op.val(2))));
    break;

  // (ptr_out)(src, ptr, imm)
  case AIE2P::VST_dmx_sts_x_pstm_nrm_imm:
  case AIE2P::VST_dmw_sts_w_pstm_nrm_imm:
  case AIE2P::VST_dmx_sts_bm_pstm_nrm_imm:
    R = vectorStore(1, access(2, 0));
    DefAddr(0, access(2, Op.imm(3)));
    break;

  // ()(src, imm)
  case AIE2P::VST_dmx_sts_x_spill:
  case AIE2P::VST_dmw_sts_w_spill:
  case AIE2P::VST_dmx_sts_bm_spill:
    R = vectorStore(0, spAccess(Op.imm(1)));
    break;

  // (dst, ptr_out)(ptr, m) and (ptr_out)(src, ptr, m): incremented from an m
  // register, whose meaning in this mode is established at the PADDA cases
  // above.
  case AIE2P::VLDA_dmx_lda_x_pstm_nrm:
  case AIE2P::VLDA_dmw_lda_w_pstm_nrm:
  case AIE2P::VLDA_dmx_lda_bm_pstm_nrm:
  case AIE2P::VLDB_dmx_ldb_x_pstm_nrm:
  case AIE2P::VLDB_dmw_ldb_pstm_nrm:
    R = vectorLoad(0, access(2, 0));
    DefAddr(1, access(2, int32_t(Op.val(3))));
    break;
  case AIE2P::VST_dmx_sts_x_pstm_nrm:
  case AIE2P::VST_dmw_sts_w_pstm_nrm:
  case AIE2P::VST_dmx_sts_bm_pstm_nrm:
    R = vectorStore(1, access(2, 0));
    DefAddr(0, access(2, int32_t(Op.val(3))));
    break;

  // (dst, ptr_out, dc)(ptr, d) and (ptr_out, dc)(src, ptr, d): the same
  // access, stepped by a 2D walk instead of a flat increment.
  case AIE2P::VLDA_2D_dmx_lda_x:
  case AIE2P::VLDA_2D_dmw_lda_w:
  case AIE2P::VLDA_2D_dmx_lda_bm:
    R = vectorLoad(0, access(3, 0));
    DefAddr(1, step2D(Op.val(3), Op.reg(4), 2));
    break;
  // The 2D store carries a dc output the flat forms do not, so src and ptr
  // sit one later than in the pstm_nrm shape: (ptr_out, dc)(src, ptr, d).
  case AIE2P::VST_2D_dmx_sts_x:
  case AIE2P::VST_2D_dmw_sts_w:
  case AIE2P::VST_2D_dmx_sts_bm:
    R = vectorStore(2, access(3, 0));
    DefAddr(0, step2D(Op.val(3), Op.reg(4), 1));
    break;

  // 3D, one output further along again: (dst, ptr_out, dcl, dch)(ptr, ds) and
  // (ptr_out, dcl, dch)(src, ptr, ds).
  case AIE2P::VLDA_3D_dmx_lda_x:
  case AIE2P::VLDA_3D_dmw_lda_w:
  case AIE2P::VLDA_3D_dmx_lda_bm:
    R = vectorLoad(0, access(4, 0));
    DefAddr(1, step3D(Op.val(4), Op.reg(5), 2, 3));
    break;
  case AIE2P::VST_3D_dmx_sts_x:
  case AIE2P::VST_3D_dmw_sts_w:
  case AIE2P::VST_3D_dmx_sts_bm:
    R = vectorStore(3, access(4, 0));
    DefAddr(0, step3D(Op.val(4), Op.reg(5), 1, 2));
    break;

  // (acc)(vec, su): widen into an accumulator. All four conversions share the
  // one body -- the widths come from the register classes.
  case AIE2P::VUPS_2x_mv_ups_w2b_upsSign0:
  case AIE2P::VUPS_2x_mv_ups_x2c_upsSign0:
  case AIE2P::VUPS_4x_mv_ups_w2c_upsSign0:
  case AIE2P::VUPS_4x_mv_ups_x2d_upsSign0:
    R = ups(/*Signed=*/false);
    break;
  case AIE2P::VUPS_2x_mv_ups_w2b_upsSign1:
  case AIE2P::VUPS_2x_mv_ups_x2c_upsSign1:
  case AIE2P::VUPS_4x_mv_ups_w2c_upsSign1:
  case AIE2P::VUPS_4x_mv_ups_x2d_upsSign1:
    R = ups(/*Signed=*/true);
    break;

  // vlda.ups -- load and widen in one instruction. Same widening as vups, and
  // the same addressing modes as vlda; the access width is the dmw/dmx in the
  // opcode, since a fused form has no source register to read it from.
  //
  // The operand indices are read off each outs list rather than shared: a
  // form with a ptr_out shifts su and ptr by one, and the dimensioned ones
  // shift them again for their counter outputs.

  // (dst)(su, ptr, dj), 256-bit access
  case AIE2P::VLDA_UPS_2x_dmw_lda_ups_w2b_idx_upsSign0:
  case AIE2P::VLDA_UPS_4x_dmw_lda_ups_w2c_idx_upsSign0:
    R = fusedUps(256, 0, 1, access(2, int32_t(Op.val(3))), false);
    break;

  // (dst)(su, ptr, dj), 512-bit access
  case AIE2P::VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign0:
  case AIE2P::VLDA_UPS_4x_dmx_lda_ups_x2d_idx_upsSign0:
    R = fusedUps(512, 0, 1, access(2, int32_t(Op.val(3))), false);
    break;

  // (dst)(su, ptr, imm), 256-bit access
  case AIE2P::VLDA_UPS_2x_dmw_lda_ups_w2b_idx_imm_upsSign0:
  case AIE2P::VLDA_UPS_4x_dmw_lda_ups_w2c_idx_imm_upsSign0:
    R = fusedUps(256, 0, 1, access(2, Op.imm(3)), false);
    break;

  // (dst)(su, ptr, imm), 512-bit access
  case AIE2P::VLDA_UPS_2x_dmx_lda_ups_x2c_idx_imm_upsSign0:
  case AIE2P::VLDA_UPS_4x_dmx_lda_ups_x2d_idx_imm_upsSign0:
    R = fusedUps(512, 0, 1, access(2, Op.imm(3)), false);
    break;

  // (dst, ptr_out)(su, ptr, m), 256-bit access
  case AIE2P::VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_upsSign0:
  case AIE2P::VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_upsSign0:
    R = fusedUps(256, 0, 2, access(3, 0), false);
    DefAddr(1, access(3, int32_t(Op.val(4))));
    break;

  // (dst, ptr_out)(su, ptr, m), 512-bit access
  case AIE2P::VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_upsSign0:
  case AIE2P::VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_upsSign0:
    R = fusedUps(512, 0, 2, access(3, 0), false);
    DefAddr(1, access(3, int32_t(Op.val(4))));
    break;

  // (dst, ptr_out)(su, ptr, imm), 256-bit access
  case AIE2P::VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_imm_upsSign0:
  case AIE2P::VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_imm_upsSign0:
    R = fusedUps(256, 0, 2, access(3, 0), false);
    DefAddr(1, access(3, Op.imm(4)));
    break;

  // (dst, ptr_out)(su, ptr, imm), 512-bit access
  case AIE2P::VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_imm_upsSign0:
  case AIE2P::VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_imm_upsSign0:
    R = fusedUps(512, 0, 2, access(3, 0), false);
    DefAddr(1, access(3, Op.imm(4)));
    break;

  // (dst)(su, ptr, dj), 256-bit access
  case AIE2P::VLDA_UPS_2x_dmw_lda_ups_w2b_idx_upsSign1:
  case AIE2P::VLDA_UPS_4x_dmw_lda_ups_w2c_idx_upsSign1:
    R = fusedUps(256, 0, 1, access(2, int32_t(Op.val(3))), true);
    break;

  // (dst)(su, ptr, dj), 512-bit access
  case AIE2P::VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign1:
  case AIE2P::VLDA_UPS_4x_dmx_lda_ups_x2d_idx_upsSign1:
    R = fusedUps(512, 0, 1, access(2, int32_t(Op.val(3))), true);
    break;

  // (dst)(su, ptr, imm), 256-bit access
  case AIE2P::VLDA_UPS_2x_dmw_lda_ups_w2b_idx_imm_upsSign1:
  case AIE2P::VLDA_UPS_4x_dmw_lda_ups_w2c_idx_imm_upsSign1:
    R = fusedUps(256, 0, 1, access(2, Op.imm(3)), true);
    break;

  // (dst)(su, ptr, imm), 512-bit access
  case AIE2P::VLDA_UPS_2x_dmx_lda_ups_x2c_idx_imm_upsSign1:
  case AIE2P::VLDA_UPS_4x_dmx_lda_ups_x2d_idx_imm_upsSign1:
    R = fusedUps(512, 0, 1, access(2, Op.imm(3)), true);
    break;

  // (dst, ptr_out)(su, ptr, m), 256-bit access
  case AIE2P::VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_upsSign1:
  case AIE2P::VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_upsSign1:
    R = fusedUps(256, 0, 2, access(3, 0), true);
    DefAddr(1, access(3, int32_t(Op.val(4))));
    break;

  // (dst, ptr_out)(su, ptr, m), 512-bit access
  case AIE2P::VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_upsSign1:
  case AIE2P::VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_upsSign1:
    R = fusedUps(512, 0, 2, access(3, 0), true);
    DefAddr(1, access(3, int32_t(Op.val(4))));
    break;

  // (dst, ptr_out)(su, ptr, imm), 256-bit access
  case AIE2P::VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_imm_upsSign1:
  case AIE2P::VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_imm_upsSign1:
    R = fusedUps(256, 0, 2, access(3, 0), true);
    DefAddr(1, access(3, Op.imm(4)));
    break;

  // (dst, ptr_out)(su, ptr, imm), 512-bit access
  case AIE2P::VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_imm_upsSign1:
  case AIE2P::VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_imm_upsSign1:
    R = fusedUps(512, 0, 2, access(3, 0), true);
    DefAddr(1, access(3, Op.imm(4)));
    break;

  // 2D walk, 256-bit access
  case AIE2P::VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign0:
  case AIE2P::VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign0:
    R = fusedUps(256, 0, 3, access(4, 0), false);
    DefAddr(1, step2D(Op.val(4), Op.reg(5), 2));
    break;

  // 2D walk, 512-bit access
  case AIE2P::VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign0:
  case AIE2P::VLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsSign0:
    R = fusedUps(512, 0, 3, access(4, 0), false);
    DefAddr(1, step2D(Op.val(4), Op.reg(5), 2));
    break;

  // 3D walk, 256-bit access
  case AIE2P::VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign0:
  case AIE2P::VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign0:
    R = fusedUps(256, 0, 4, access(5, 0), false);
    DefAddr(1, step3D(Op.val(5), Op.reg(6), 2, 3));
    break;

  // 3D walk, 512-bit access
  case AIE2P::VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign0:
  case AIE2P::VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign0:
    R = fusedUps(512, 0, 4, access(5, 0), false);
    DefAddr(1, step3D(Op.val(5), Op.reg(6), 2, 3));
    break;

  // 2D walk, 256-bit access
  case AIE2P::VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign1:
  case AIE2P::VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign1:
    R = fusedUps(256, 0, 3, access(4, 0), true);
    DefAddr(1, step2D(Op.val(4), Op.reg(5), 2));
    break;

  // 2D walk, 512-bit access
  case AIE2P::VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign1:
  case AIE2P::VLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsSign1:
    R = fusedUps(512, 0, 3, access(4, 0), true);
    DefAddr(1, step2D(Op.val(4), Op.reg(5), 2));
    break;

  // 3D walk, 256-bit access
  case AIE2P::VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign1:
  case AIE2P::VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign1:
    R = fusedUps(256, 0, 4, access(5, 0), true);
    DefAddr(1, step3D(Op.val(5), Op.reg(6), 2, 3));
    break;

  // 3D walk, 512-bit access
  case AIE2P::VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign1:
  case AIE2P::VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign1:
    R = fusedUps(512, 0, 4, access(5, 0), true);
    DefAddr(1, step3D(Op.val(5), Op.reg(6), 2, 3));
    break;

  // (vec)(acc, su): the way back down.
  case AIE2P::VSRS_2x_mv_w_srs_bm_srsSign0:
  case AIE2P::VSRS_2x_mv_x_srs_cm_srsSign0:
  case AIE2P::VSRS_4x_mv_w_srs_cm_srsSign0:
  case AIE2P::VSRS_4x_mv_x_srs_dm_srsSign0:
    R = srs(/*Signed=*/false);
    break;
  case AIE2P::VSRS_2x_mv_w_srs_bm_srsSign1:
  case AIE2P::VSRS_2x_mv_x_srs_cm_srsSign1:
  case AIE2P::VSRS_4x_mv_w_srs_cm_srsSign1:
  case AIE2P::VSRS_4x_mv_x_srs_dm_srsSign1:
    R = srs(/*Signed=*/true);
    break;

  // vst.srs -- narrow and store in one instruction. The narrowing is deferred
  // into the MemWrite, so it runs on the accumulator's value at the store's
  // sample cycle rather than at issue.
  //
  // Operand indices again read off each outs list: a ptr_out shifts src and
  // su by one, and the dimensioned forms shift them again per counter.

  // ()(src, su, ptr, dj), 2x
  case AIE2P::VST_SRS_2x_dmw_sts_srs_bm_idx_srsSign0:
  case AIE2P::VST_SRS_2x_dm_sts_srs_cm_idx_srsSign0:
    R = fusedSrs(2, 0, 1, access(2, int32_t(Op.val(3))), false);
    break;

  // ()(src, su, ptr, dj), 4x
  case AIE2P::VST_SRS_4x_dm_sts_srs_cm_idx_srsSign0:
  case AIE2P::VST_SRS_4x_dmx_sts_srs_dm_idx_srsSign0:
    R = fusedSrs(4, 0, 1, access(2, int32_t(Op.val(3))), false);
    break;

  // ()(src, su, ptr, imm), 2x
  case AIE2P::VST_SRS_2x_dmw_sts_srs_bm_idx_imm_srsSign0:
  case AIE2P::VST_SRS_2x_dm_sts_srs_cm_idx_imm_srsSign0:
    R = fusedSrs(2, 0, 1, access(2, Op.imm(3)), false);
    break;

  // ()(src, su, ptr, imm), 4x
  case AIE2P::VST_SRS_4x_dm_sts_srs_cm_idx_imm_srsSign0:
  case AIE2P::VST_SRS_4x_dmx_sts_srs_dm_idx_imm_srsSign0:
    R = fusedSrs(4, 0, 1, access(2, Op.imm(3)), false);
    break;

  // (ptr_out)(src, su, ptr, m), 2x
  case AIE2P::VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_srsSign0:
  case AIE2P::VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_srsSign0:
    R = fusedSrs(2, 1, 2, access(3, 0), false);
    DefAddr(0, access(3, int32_t(Op.val(4))));
    break;

  // (ptr_out)(src, su, ptr, m), 4x
  case AIE2P::VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_srsSign0:
  case AIE2P::VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_srsSign0:
    R = fusedSrs(4, 1, 2, access(3, 0), false);
    DefAddr(0, access(3, int32_t(Op.val(4))));
    break;

  // (ptr_out)(src, su, ptr, imm), 2x
  case AIE2P::VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_imm_srsSign0:
  case AIE2P::VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_imm_srsSign0:
    R = fusedSrs(2, 1, 2, access(3, 0), false);
    DefAddr(0, access(3, Op.imm(4)));
    break;

  // (ptr_out)(src, su, ptr, imm), 4x
  case AIE2P::VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_imm_srsSign0:
  case AIE2P::VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_imm_srsSign0:
    R = fusedSrs(4, 1, 2, access(3, 0), false);
    DefAddr(0, access(3, Op.imm(4)));
    break;

  // ()(src, su, ptr, dj), 2x
  case AIE2P::VST_SRS_2x_dmw_sts_srs_bm_idx_srsSign1:
  case AIE2P::VST_SRS_2x_dm_sts_srs_cm_idx_srsSign1:
    R = fusedSrs(2, 0, 1, access(2, int32_t(Op.val(3))), true);
    break;

  // ()(src, su, ptr, dj), 4x
  case AIE2P::VST_SRS_4x_dm_sts_srs_cm_idx_srsSign1:
  case AIE2P::VST_SRS_4x_dmx_sts_srs_dm_idx_srsSign1:
    R = fusedSrs(4, 0, 1, access(2, int32_t(Op.val(3))), true);
    break;

  // ()(src, su, ptr, imm), 2x
  case AIE2P::VST_SRS_2x_dmw_sts_srs_bm_idx_imm_srsSign1:
  case AIE2P::VST_SRS_2x_dm_sts_srs_cm_idx_imm_srsSign1:
    R = fusedSrs(2, 0, 1, access(2, Op.imm(3)), true);
    break;

  // ()(src, su, ptr, imm), 4x
  case AIE2P::VST_SRS_4x_dm_sts_srs_cm_idx_imm_srsSign1:
  case AIE2P::VST_SRS_4x_dmx_sts_srs_dm_idx_imm_srsSign1:
    R = fusedSrs(4, 0, 1, access(2, Op.imm(3)), true);
    break;

  // (ptr_out)(src, su, ptr, m), 2x
  case AIE2P::VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_srsSign1:
  case AIE2P::VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_srsSign1:
    R = fusedSrs(2, 1, 2, access(3, 0), true);
    DefAddr(0, access(3, int32_t(Op.val(4))));
    break;

  // (ptr_out)(src, su, ptr, m), 4x
  case AIE2P::VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_srsSign1:
  case AIE2P::VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_srsSign1:
    R = fusedSrs(4, 1, 2, access(3, 0), true);
    DefAddr(0, access(3, int32_t(Op.val(4))));
    break;

  // (ptr_out)(src, su, ptr, imm), 2x
  case AIE2P::VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_imm_srsSign1:
  case AIE2P::VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_imm_srsSign1:
    R = fusedSrs(2, 1, 2, access(3, 0), true);
    DefAddr(0, access(3, Op.imm(4)));
    break;

  // (ptr_out)(src, su, ptr, imm), 4x
  case AIE2P::VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_imm_srsSign1:
  case AIE2P::VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_imm_srsSign1:
    R = fusedSrs(4, 1, 2, access(3, 0), true);
    DefAddr(0, access(3, Op.imm(4)));
    break;

  // 2D walk, 2x
  case AIE2P::VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign0:
  case AIE2P::VST_2D_SRS_2x_dm_sts_srs_cm_srsSign0:
    R = fusedSrs(2, 2, 3, access(4, 0), false);
    DefAddr(0, step2D(Op.val(4), Op.reg(5), 1));
    break;

  // 2D walk, 4x
  case AIE2P::VST_2D_SRS_4x_dm_sts_srs_cm_srsSign0:
  case AIE2P::VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign0:
    R = fusedSrs(4, 2, 3, access(4, 0), false);
    DefAddr(0, step2D(Op.val(4), Op.reg(5), 1));
    break;

  // 3D walk, 2x
  case AIE2P::VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign0:
  case AIE2P::VST_3D_SRS_2x_dm_sts_srs_cm_srsSign0:
    R = fusedSrs(2, 3, 4, access(5, 0), false);
    DefAddr(0, step3D(Op.val(5), Op.reg(6), 1, 2));
    break;

  // 3D walk, 4x
  case AIE2P::VST_3D_SRS_4x_dm_sts_srs_cm_srsSign0:
  case AIE2P::VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign0:
    R = fusedSrs(4, 3, 4, access(5, 0), false);
    DefAddr(0, step3D(Op.val(5), Op.reg(6), 1, 2));
    break;

  // 2D walk, 2x
  case AIE2P::VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign1:
  case AIE2P::VST_2D_SRS_2x_dm_sts_srs_cm_srsSign1:
    R = fusedSrs(2, 2, 3, access(4, 0), true);
    DefAddr(0, step2D(Op.val(4), Op.reg(5), 1));
    break;

  // 2D walk, 4x
  case AIE2P::VST_2D_SRS_4x_dm_sts_srs_cm_srsSign1:
  case AIE2P::VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign1:
    R = fusedSrs(4, 2, 3, access(4, 0), true);
    DefAddr(0, step2D(Op.val(4), Op.reg(5), 1));
    break;

  // 3D walk, 2x
  case AIE2P::VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign1:
  case AIE2P::VST_3D_SRS_2x_dm_sts_srs_cm_srsSign1:
    R = fusedSrs(2, 3, 4, access(5, 0), true);
    DefAddr(0, step3D(Op.val(5), Op.reg(6), 1, 2));
    break;

  // 3D walk, 4x
  case AIE2P::VST_3D_SRS_4x_dm_sts_srs_cm_srsSign1:
  case AIE2P::VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign1:
    R = fusedSrs(4, 3, 4, access(5, 0), true);
    DefAddr(0, step3D(Op.val(5), Op.reg(6), 1, 2));
    break;

  // (acc)(acc_in, s1, s2, conf) and (acc)(s1, s2, conf). One opcode per
  // register pairing; the shape is in the conf.
  // The four DENSE pairings share one operand shape and one behaviour: the
  // pairing names only how wide each source register class is, and a shape
  // reads the low M*K / K*N of whatever it is handed. The Q* pairings are
  // absent deliberately -- their $s2 is a sparse class (eQYs / OP_mQXsw, with
  // Defs = [srSparse_of]), which is a different instruction, not a wider one.
  case AIE2P::VMAC_vmul_cm_core_X_X:
  case AIE2P::VMAC_vmul_cm_core_X_Y:
  case AIE2P::VMAC_vmul_cm_core_Y_X:
  case AIE2P::VMAC_vmul_cm_core_Y_Y:
    R = mac(2, 3, 4, /*AccIdx=*/1);
    break;
  // Both widths; the lane count comes from the accumulator, not the arm.
  case AIE2P::VCONV_bf16_fp32_mv_w_srs_bf:
  case AIE2P::VCONV_bf16_fp32_mv_x_srs_bf:
    R = convBf16();
    break;

  // The widening direction, both widths. Same shape: the lane count follows
  // from the register classes, so one body serves the w (256->512) and
  // x (512->1024) arms.
  case AIE2P::VCONV_fp32_bf16_mv_ups_wbf:
  case AIE2P::VCONV_fp32_bf16_mv_ups_xbf:
    R = convUpsBf16();
    break;

  // vlda.conv.fp32.bf16 -- the same widening over bits just loaded. Addressing
  // is vlda's, so the operand indices come off each outs list the way the
  // vlda.ups forms do: a ptr_out shifts ptr and its modifier by one.

  // (dst)(ptr, dj), 256-bit access
  case AIE2P::VLDA_CONV_fp32_bf16_dmw_lda_ups_bf_idx:
    R = fusedConvUpsBf16(256, 0, access(1, int32_t(Op.val(2))));
    break;

  // (dst)(ptr, imm), 256-bit access
  case AIE2P::VLDA_CONV_fp32_bf16_dmw_lda_ups_bf_idx_imm:
    R = fusedConvUpsBf16(256, 0, access(1, Op.imm(2)));
    break;

  // (dst)(ptr, dj), 512-bit access
  case AIE2P::VLDA_CONV_fp32_bf16_dmx_lda_ups_bf_idx:
    R = fusedConvUpsBf16(512, 0, access(1, int32_t(Op.val(2))));
    break;

  // (dst)(ptr, imm), 512-bit access
  case AIE2P::VLDA_CONV_fp32_bf16_dmx_lda_ups_bf_idx_imm:
    R = fusedConvUpsBf16(512, 0, access(1, Op.imm(2)));
    break;

  // (dst, ptr_out)(ptr, m), 256-bit access
  case AIE2P::VLDA_CONV_fp32_bf16_dmw_lda_ups_bf_pstm_nrm:
    R = fusedConvUpsBf16(256, 0, access(2, 0));
    DefAddr(1, access(2, int32_t(Op.val(3))));
    break;

  // (dst, ptr_out)(ptr, imm), 256-bit access
  case AIE2P::VLDA_CONV_fp32_bf16_dmw_lda_ups_bf_pstm_nrm_imm:
    R = fusedConvUpsBf16(256, 0, access(2, 0));
    DefAddr(1, access(2, Op.imm(3)));
    break;

  // (dst, ptr_out)(ptr, m), 512-bit access
  case AIE2P::VLDA_CONV_fp32_bf16_dmx_lda_ups_bf_pstm_nrm:
    R = fusedConvUpsBf16(512, 0, access(2, 0));
    DefAddr(1, access(2, int32_t(Op.val(3))));
    break;

  // (dst, ptr_out)(ptr, imm), 512-bit access
  case AIE2P::VLDA_CONV_fp32_bf16_dmx_lda_ups_bf_pstm_nrm_imm:
    R = fusedConvUpsBf16(512, 0, access(2, 0));
    DefAddr(1, access(2, Op.imm(3)));
    break;

  // vst.conv.bf16.fp32 -- the narrowing direction, out to memory. Addressing
  // is vst's. These forms have no dst, so src sits at operand 0, and the
  // pstm ones add a ptr_out to the outs list which shifts src by one.

  // (src, ptr, dj)
  case AIE2P::VST_CONV_bf16_fp32_dmw_sts_srs_bf_idx:
  case AIE2P::VST_CONV_bf16_fp32_dmx_sts_srs_bf_idx:
    R = fusedConvSrsBf16(0, access(1, int32_t(Op.val(2))));
    break;

  // (src, ptr, imm)
  case AIE2P::VST_CONV_bf16_fp32_dmw_sts_srs_bf_idx_imm:
  case AIE2P::VST_CONV_bf16_fp32_dmx_sts_srs_bf_idx_imm:
    R = fusedConvSrsBf16(0, access(1, Op.imm(2)));
    break;

  // (ptr_out)(src, ptr, m)
  case AIE2P::VST_CONV_bf16_fp32_dmw_sts_srs_bf_pstm_nrm:
  case AIE2P::VST_CONV_bf16_fp32_dmx_sts_srs_bf_pstm_nrm:
    R = fusedConvSrsBf16(1, access(2, 0));
    DefAddr(0, access(2, int32_t(Op.val(3))));
    break;

  // (ptr_out)(src, ptr, imm)
  case AIE2P::VST_CONV_bf16_fp32_dmw_sts_srs_bf_pstm_nrm_imm:
  case AIE2P::VST_CONV_bf16_fp32_dmx_sts_srs_bf_pstm_nrm_imm:
    R = fusedConvSrsBf16(1, access(2, 0));
    DefAddr(0, access(2, Op.imm(3)));
    break;

  // Only two bf pairings exist, and they differ solely in source width, which
  // the lane count is derived from rather than named per arm.
  case AIE2P::VMUL_f_vmul_bf_vmul_bf_core_X_X:
  case AIE2P::VMUL_f_vmul_bf_vmul_bf_core_Y_Y:
    R = mulBf16(1, 2, 3, /*AccIdx=*/-1);
    break;

  // vmac.f, the accumulating form: same two bf pairings, with $acc1 added in.
  // The other four variants of this opcode (vmac_bfp_*, EX/EY/QEX/QEY) are
  // bfp16 and NOT modelled -- their eEY/eQEYs composition is open, PARKED.
  case AIE2P::VMAC_f_vmac_bf_vmul_bf_core_X_X:
  case AIE2P::VMAC_f_vmac_bf_vmul_bf_core_Y_Y:
    R = mulBf16(2, 3, 4, /*AccIdx=*/1);
    break;

  case AIE2P::VMUL_vmul_cm_core_X_X:
  case AIE2P::VMUL_vmul_cm_core_X_Y:
  case AIE2P::VMUL_vmul_cm_core_Y_X:
  case AIE2P::VMUL_vmul_cm_core_Y_Y:
    R = mac(1, 2, 3, /*AccIdx=*/-1);
    break;

  // acq/rel: the blocking lock ports, which are not in the memory map. The
  // acquire VALUE is signed and its sign picks the mode -- negative means
  // AcquireGreaterEqual on its magnitude -- but that is the fabric's rule and
  // the value passes through untouched, so it is not restated here.
  //
  // An acquire that cannot succeed STALLS: nothing architectural changed and
  // the bundle re-issues, which is what the blocking port does. Release never
  // blocks on this subtarget.
  //
  // The .cond forms take r26 as a predicate and are NOT modelled -- nothing
  // read so far says what the predicate is, and a lock taken on a guessed
  // condition deadlocks or corrupts silently.
  case AIE2P::ACQ_mLockId_imm:
  case AIE2P::ACQ_mLockId_reg:
  case AIE2P::REL_mLockId_imm:
  case AIE2P::REL_mLockId_reg: {
    const bool IsImm = Opc == AIE2P::ACQ_mLockId_imm ||
                       Opc == AIE2P::REL_mLockId_imm;
    const unsigned Id = IsImm ? unsigned(Op.imm(0)) : Op.val(0);
    const int32_t Value = int32_t(Op.val(1));
    if (!Op.Ok)
      break;
    const bool IsAcquire =
        Opc == AIE2P::ACQ_mLockId_imm || Opc == AIE2P::ACQ_mLockId_reg;
    switch (IsAcquire ? Host.acquireLock(Id, Value)
                      : Host.releaseLock(Id, Value)) {
    case PortStatus::Ok:
      break;
    case PortStatus::Stall:
      return StepResult::Stalled;
    case PortStatus::Fault:
      return fault(Name + ": lock " + Twine(Id) +
                   " is not provided by this embedder");
    }
    break;
  }

  // (dst)(src): a whole-register move. Width from the class, which is what
  // lets one body serve every arm -- the w form moves 256 bits, the cm form
  // 1024 and vmov.d 2048, and none of them is a special case of another.
  //
  // The class width is also the whole of the accumulator semantics here. cm
  // and dm are not separate files: dm0 IS [cml0, cmh0], so a vmov naming cml0
  // has to leave cmh0 alone, and it does because it writes exactly the 1024
  // bits its class declares. Getting that wrong is invisible until something
  // reads the other half.
  //
  // The mcd and scd forms are deliberately absent: they move a register to or
  // from the cascade stream, and this model has no stream to move it to.
  case AIE2P::VMOV_D:
  case AIE2P::VMOV_alu_mv_mv_cm:
  case AIE2P::VMOV_alu_mv_mv_w: {
    const MCRegister Dst = Op.reg(0);
    const unsigned W = State.Regs.getClassWidth(Dst);
    if (!W) {
      R = fault(Name + ": " + MRI.getName(Dst) + " has no class width");
      break;
    }
    Eff.RegWrites.push_back({Dst, Op.valN(1, W), Op.cycleOf(0), Op.fwdOf(0)});
    break;
  }

  // The whole family goes in together: the eight integer forms differ only in
  // the two flags, so splitting them by which one a sweep happened to reach
  // leaves the rest to fault a round later.
  case AIE2P::VMAX_LT_8_vaddSign0:
    R = minMax(8, /*IsMax=*/true, /*Signed=*/false);
    break;
  case AIE2P::VMAX_LT_8_vaddSign1:
    R = minMax(8, /*IsMax=*/true, /*Signed=*/true);
    break;
  case AIE2P::VMAX_LT_16_vaddSign0:
    R = minMax(16, /*IsMax=*/true, /*Signed=*/false);
    break;
  case AIE2P::VMAX_LT_16_vaddSign1:
    R = minMax(16, /*IsMax=*/true, /*Signed=*/true);
    break;
  case AIE2P::VMAX_LT_32_vaddSign0:
    R = minMax(32, /*IsMax=*/true, /*Signed=*/false);
    break;
  case AIE2P::VMAX_LT_32_vaddSign1:
    R = minMax(32, /*IsMax=*/true, /*Signed=*/true);
    break;
  case AIE2P::VMAX_LT_bf16:
    R = minMax(16, /*IsMax=*/true, /*Signed=*/true, /*IsFloat=*/true);
    break;
  case AIE2P::VMIN_GE_8_vaddSign0:
    R = minMax(8, /*IsMax=*/false, /*Signed=*/false);
    break;
  case AIE2P::VMIN_GE_8_vaddSign1:
    R = minMax(8, /*IsMax=*/false, /*Signed=*/true);
    break;
  case AIE2P::VMIN_GE_16_vaddSign0:
    R = minMax(16, /*IsMax=*/false, /*Signed=*/false);
    break;
  case AIE2P::VMIN_GE_16_vaddSign1:
    R = minMax(16, /*IsMax=*/false, /*Signed=*/true);
    break;
  case AIE2P::VMIN_GE_32_vaddSign0:
    R = minMax(32, /*IsMax=*/false, /*Signed=*/false);
    break;
  case AIE2P::VMIN_GE_32_vaddSign1:
    R = minMax(32, /*IsMax=*/false, /*Signed=*/true);
    break;
  case AIE2P::VMIN_GE_bf16:
    R = minMax(16, /*IsMax=*/false, /*Signed=*/true, /*IsFloat=*/true);
    break;

  // The three widths go in together for the same reason the min/max family
  // does; the corpus issues vadd.32, vsub.16 and vsub.32, and the rest differ
  // only in the lane count.
  case AIE2P::VADD_8:
    R = addSub(8, /*IsSub=*/false);
    break;
  case AIE2P::VADD_16:
    R = addSub(16, /*IsSub=*/false);
    break;
  case AIE2P::VADD_32:
    R = addSub(32, /*IsSub=*/false);
    break;
  case AIE2P::VSUB_8:
    R = addSub(8, /*IsSub=*/true);
    break;
  case AIE2P::VSUB_16:
    R = addSub(16, /*IsSub=*/true);
    break;
  case AIE2P::VSUB_32:
    R = addSub(32, /*IsSub=*/true);
    break;

  case AIE2P::VADD_vmac_cm2_add_reg:
    R = accAddSub(/*IsSub=*/false, /*IsFloat=*/false);
    break;
  case AIE2P::VSUB_vmac_cm2_add_reg:
    R = accAddSub(/*IsSub=*/true, /*IsFloat=*/false);
    break;
  case AIE2P::VADD_f_vmac_cm2_add_reg:
    R = accAddSub(/*IsSub=*/false, /*IsFloat=*/true);
    break;
  case AIE2P::VSUB_f_vmac_cm2_add_reg:
    R = accAddSub(/*IsSub=*/true, /*IsFloat=*/true);
    break;

  case AIE2P::VSHUFFLE_vec_shuffle_x:
  case AIE2P::VSHUFFLE_vec_shuffle_bm:
    R = vshuffle();
    break;

  case AIE2P::VSEL_8:
    R = vsel(8);
    break;
  case AIE2P::VSEL_16:
    R = vsel(16);
    break;
  case AIE2P::VSEL_32:
    R = vsel(32);
    break;

  // Both index forms per width and both signs: the imm and register forms
  // differ only in where the lane number comes from.
  case AIE2P::VEXTRACT_8_vec_extract_imm_vaddSign0:
  case AIE2P::VEXTRACT_8_vec_extract_r_vaddSign0:
    R = vextract(8, /*Signed=*/false);
    break;
  case AIE2P::VEXTRACT_8_vec_extract_imm_vaddSign1:
  case AIE2P::VEXTRACT_8_vec_extract_r_vaddSign1:
    R = vextract(8, /*Signed=*/true);
    break;
  case AIE2P::VEXTRACT_16_vec_extract_imm_vaddSign0:
  case AIE2P::VEXTRACT_16_vec_extract_r_vaddSign0:
    R = vextract(16, /*Signed=*/false);
    break;
  case AIE2P::VEXTRACT_16_vec_extract_imm_vaddSign1:
  case AIE2P::VEXTRACT_16_vec_extract_r_vaddSign1:
    R = vextract(16, /*Signed=*/true);
    break;
  case AIE2P::VEXTRACT_32_vec_extract_imm_vaddSign0:
  case AIE2P::VEXTRACT_32_vec_extract_r_vaddSign0:
    R = vextract(32, /*Signed=*/false);
    break;
  case AIE2P::VEXTRACT_32_vec_extract_imm_vaddSign1:
  case AIE2P::VEXTRACT_32_vec_extract_r_vaddSign1:
    R = vextract(32, /*Signed=*/true);
    break;
  // The 64-bit forms write an eL pair, so the widening is a no-op and the
  // sign only names which opcode the compiler picked.
  case AIE2P::VEXTRACT_64_vec_extract_imm_vaddSign0:
  case AIE2P::VEXTRACT_64_vec_extract_r_vaddSign0:
    R = vextract(64, /*Signed=*/false);
    break;
  case AIE2P::VEXTRACT_64_vec_extract_imm_vaddSign1:
  case AIE2P::VEXTRACT_64_vec_extract_r_vaddSign1:
    R = vextract(64, /*Signed=*/true);
    break;

  // Only the .32 pair: the corpus issues both of its forms and none of the
  // other widths, and unlike a lane-count change the .64 form takes its value
  // from an eL PAIR, which is a different operand shape rather than the same
  // one at another size.
  case AIE2P::VINSERT_32_mIdxImm0:
    R = vinsert(32, /*IdxIdx=*/-1, /*SrcIdx=*/2);
    break;
  case AIE2P::VINSERT_32_mR29_insert:
    R = vinsert(32, /*IdxIdx=*/2, /*SrcIdx=*/3);
    break;

  // Both index forms per width, as on vextract.
  case AIE2P::VEXTBCST_16_vec_extract_broadcast_imm:
  case AIE2P::VEXTBCST_16_vec_extract_broadcast_r:
    R = vextractBroadcast(16);
    break;
  case AIE2P::VEXTBCST_32_vec_extract_broadcast_imm:
  case AIE2P::VEXTBCST_32_vec_extract_broadcast_r:
    R = vextractBroadcast(32);
    break;
  case AIE2P::VEXTBCST_64_vec_extract_broadcast_imm:
  case AIE2P::VEXTBCST_64_vec_extract_broadcast_r:
    R = vextractBroadcast(64);
    break;
  case AIE2P::VEXTBCST_128_vec_extract_broadcast_imm:
  case AIE2P::VEXTBCST_128_vec_extract_broadcast_r:
    R = vextractBroadcast(128);
    break;

  case AIE2P::VSHIFT:
    R = vshift();
    break;

  case AIE2P::VPUSH_hi_8:
    R = vpushHi(8);
    break;
  case AIE2P::VPUSH_hi_16:
    R = vpushHi(16);
    break;
  case AIE2P::VPUSH_hi_32:
    R = vpushHi(32);
    break;
  case AIE2P::VPUSH_hi_64:
    R = vpushHi(64);
    break;

  // (d)(s1, s2) across the whole register. These two are the only elementwise
  // vector ALU ops AIE2P spells without the MAC datapath -- an elementwise add
  // goes through vaddmac and an accumulator -- so they are what a vector
  // datapath can be checked with before accumulators are modelled. There is no
  // vbxor on this subtarget.
  case AIE2P::VBAND:
  case AIE2P::VBOR: {
    const MCRegister Dst = Op.reg(0);
    const unsigned W = State.Regs.getClassWidth(Dst);
    if (!W) {
      FaultMsg = (Name + ": " + MRI.getName(Dst) + " has no width").str();
      return StepResult::Fault;
    }
    const APInt A = Op.valN(1, W);
    const APInt B = Op.valN(2, W);
    Eff.RegWrites.push_back(
        {Dst, Opc == AIE2P::VBAND ? (A & B) : (A | B), Op.cycleOf(0), Op.fwdOf(0)});
    break;
  }

  // The vector broadcasts, the first vector instructions modelled. 8/16/32
  // take a 32-bit eR; 64 takes an eL pair, which is itself composed, so it is
  // also the first instruction to READ through composition rather than write
  // through it.
  case AIE2P::VBCST_8:
    R = broadcast(8);
    break;
  case AIE2P::VBCST_16:
    R = broadcast(16);
    break;
  case AIE2P::VBCST_32:
    R = broadcast(32);
    break;
  case AIE2P::VBCST_64:
    R = broadcast(64);
    break;

  // vclr (dst)(): the accumulator's broadcast of zero, and the same family as
  // the four above -- AIE2PRegisterBankInfo splits one G_AIE_BROADCAST_VECTOR
  // by width with "512-bit size is supported using VBCST and 2048-bit using
  // VCLR".
  //
  // The def takes no source operand, so the only value it can write is a
  // constant, and three independent places name that constant zero: AIE2's def
  // sits under the spec's "9.8 VCLR - Clears accumulator word", AIE2PS selects
  // it from int_aie2ps_clr_ACC2048_conf, and AIECombinerHelper only rewrites a
  // broadcast into one once IsConstantZeroReg has proved the source is zero.
  //
  // Width comes from the class rather than the 2048 the selector asserts: eDM
  // is what the def declares, and reading it from the class is what keeps a
  // narrower dst from having the top of a wider one zeroed under it.
  case AIE2P::VCLR: {
    const MCRegister Dst = Op.reg(0);
    const unsigned W = State.Regs.getClassWidth(Dst);
    if (!W) {
      R = fault(Name + ": " + MRI.getName(Dst) + " has no class width");
      break;
    }
    Eff.RegWrites.push_back({Dst, APInt(W, 0), Op.cycleOf(0), Op.fwdOf(0)});
    break;
  }
  }

  if (!Op.Ok) {
    FaultMsg = Op.BadReg ? (Name + ": " + MRI.getName(Op.BadReg) +
                            " holds no value this model has computed")
                               .str()
                         : (Name + ": unexpected operand kind").str();
    return StepResult::Fault;
  }
  return R;
}

namespace {
#include "AIE2PSubRegRanges.inc"
} // namespace

ArrayRef<AIESubRegRange> llvm::AIESim::subRegRangesForTriple(const Triple &TT) {
  if (TT.getArch() == Triple::aie2p)
    return ArrayRef(AIE2PSubRegRanges);
  return {};
}

StringRef llvm::AIESim::cpuForTriple(const Triple &TT) {
  switch (TT.getArch()) {
  case Triple::aie:
    return "aie";
  case Triple::aie2:
    return "aie2";
  case Triple::aie2p:
    return "aie2p";
  case Triple::aie2ps:
    return "aie2ps";
  default:
    return "";
  }
}

std::unique_ptr<AIESemantics>
llvm::AIESim::createSemantics(const MCSubtargetInfo &STI,
                              const MCInstrInfo &MII,
                              const MCRegisterInfo &MRI) {
  const Triple &TT = STI.getTargetTriple();
  if (TT.getArch() == Triple::aie2p)
    return std::make_unique<AIE2PSemantics>(
        MII, MRI, STI.getInstrItineraryForCPU(cpuForTriple(TT)),
        subRegRangesForTriple(TT));
  return nullptr;
}
