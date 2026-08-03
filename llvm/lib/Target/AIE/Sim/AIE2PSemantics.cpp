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
    if (!State.Regs.read(R, V, cycleOf(I))) {
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
    if (!State.Regs.read(R, V, cycleOf(I))) {
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

  Operands Op{MI, State, State.RetiredBundles,
              Itin.isEmpty() ? nullptr : &Itin, Desc.getSchedClass()};
  auto Def32 = [&](unsigned Idx, uint32_t V) {
    Eff.RegWrites.push_back({Op.reg(Idx), APInt(32, V), Op.cycleOf(Idx)});
  };
  auto DefAddr = [&](unsigned Idx, uint32_t V) {
    Eff.RegWrites.push_back(
        {Op.reg(Idx), APInt(DataAddrBits, V), Op.cycleOf(Idx)});
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
                             Op.cycleOf(DstIdx)});
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
        {Op.reg(Idx), APInt(DataAddrBits, V), Op.cycleOf(Idx)});
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
    Eff.RegWrites.push_back({Dst, V, Op.cycleOf(DstIdx)});
    return StepResult::Retired;
  };

  // The source register is NAMED here, not read: a store samples its data at
  // its own operand cycle, which on the narrow forms is 7 cycles after issue,
  // by which time an instruction scheduled after the store may have produced
  // it. The executor reads it when the pipeline gets there.
  auto scalarStore = [&](unsigned SrcIdx, uint32_t Addr, unsigned NumBytes) {
    Eff.MemWrites.push_back(
        {Addr, NumBytes, Op.reg(SrcIdx), Op.cycleOf(SrcIdx)});
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
    Eff.MemWrites.push_back({Addr, W / 8, Src, Op.cycleOf(SrcIdx)});
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
        {Dst, APInt::getSplat(W, Op.valN(1, LaneBits)), Op.cycleOf(0)});
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
    if (!State.Regs.read(AIE2P::lr, V, State.RetiredBundles + 1)) {
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
                             State.RetiredBundles + 1});
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
  // (dst)(ptr, imm): vector loads, same addressing as the scalar idx_imm
  // forms. The x form is 512 bits into a composed register, the w form 256
  // into one that has its own storage, so between them they cover both paths
  // a vector destination can take.
  case AIE2P::VLDA_dmx_lda_x_idx_imm:
  case AIE2P::VLDA_dmw_lda_w_idx_imm:
  case AIE2P::VLDA_dmx_lda_bm_idx_imm:
    R = vectorLoad(0, access(1, Op.imm(2)));
    break;

  // (dst)(ptr, dj): the offset in a register instead of the encoding. dj is a
  // byte offset, and the pointer is unchanged -- no pstm in the name and no
  // ptr_out in the outs list.
  case AIE2P::VLDA_dmx_lda_x_idx:
  case AIE2P::VLDA_dmw_lda_w_idx:
  case AIE2P::VLDA_dmx_lda_bm_idx:
    R = vectorLoad(0, access(1, int32_t(Op.val(2))));
    break;

  // (dst, ptr_out)(ptr, imm): load from ptr, then advance the pointer.
  case AIE2P::VLDA_dmx_lda_x_pstm_nrm_imm:
  case AIE2P::VLDA_dmw_lda_w_pstm_nrm_imm:
  case AIE2P::VLDA_dmx_lda_bm_pstm_nrm_imm:
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
        {Dst, Opc == AIE2P::VBAND ? (A & B) : (A | B), Op.cycleOf(0)});
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
