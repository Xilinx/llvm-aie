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
#include "MCTargetDesc/aie2p/AIE2PMCTargetDesc.h"
#include "llvm/ADT/Twine.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
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
  bool Ok = true;
  MCRegister BadReg;

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
    if (!State.Regs.read(R, V)) {
      Ok = false;
      BadReg = R;
      return 0;
    }
    // A 20-bit pointer or special register read into a 32-bit datapath.
    // UNVERIFIED: the architecture spec has not been checked for whether these
    // widen by zero or by sign extension.
    return V.zextOrTrunc(32).getZExtValue();
  }

  // Same as val(), for a register that is architecturally implicit rather
  // than an MI operand -- sp in a *_spill opcode, which encodes only an
  // offset (aie2p/AIE2PInstrInfo.td's spill pseudos all expand to a fixed
  // "[sp, #$imm]" real opcode with no pointer-register bits at all).
  uint32_t valReg(MCRegister R) {
    APInt V;
    if (!State.Regs.read(R, V)) {
      Ok = false;
      BadReg = R;
      return 0;
    }
    return V.zextOrTrunc(32).getZExtValue();
  }
};

class AIE2PSemantics : public AIESemantics {
public:
  AIE2PSemantics(const MCInstrInfo &MII, const MCRegisterInfo &MRI)
      : MII(MII), MRI(MRI) {}

  bool isEndOfProgram(const MCInst &MI) const override {
    return MI.getOpcode() == AIE2P::DONE;
  }

  std::array<MCRegister, 3> getLoopRegisters() const override {
    return {AIE2P::lc, AIE2P::ls, AIE2P::le};
  }

  MCRegister getLinkRegister() const override { return AIE2P::lr; }

  StepResult execute(const MCInst &MI, const AIECoreState &State,
                     AIEHostInterface &Host, SlotEffects &Eff,
                     std::string &FaultMsg) override;

private:
  const MCInstrInfo &MII;
  const MCRegisterInfo &MRI;
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

  Operands Op{MI, State};
  auto Def32 = [&](unsigned Idx, uint32_t V) {
    Eff.RegWrites.emplace_back(Op.reg(Idx), APInt(32, V));
  };
  auto DefAddr = [&](unsigned Idx, uint32_t V) {
    Eff.RegWrites.emplace_back(Op.reg(Idx), APInt(DataAddrBits, V));
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
    Eff.RegWrites.emplace_back(Op.reg(DstIdx),
                               Signed ? V.sext(32) : V.zext(32));
    return StepResult::Retired;
  };

  auto scalarStore = [&](unsigned SrcIdx, uint32_t Addr, unsigned NumBytes) {
    Eff.MemWrites.push_back(
        {Addr, NumBytes, APInt(NumBytes * 8, Op.val(SrcIdx))});
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

  case AIE2P::MOVA:
  case AIE2P::MOVXM:
  case AIE2P::MOV_alu_mv_mv_mv_cg:
    Def32(0, uint32_t(Op.imm(1)));
    break;
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
    if (!State.Regs.read(AIE2P::lr, V)) {
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
  case AIE2P::PADDA_pstm_nrm:
  case AIE2P::PADDB_pstm_nrm:
  case AIE2P::PADDS_pstm_nrm:
    DefAddr(0, access(1, int32_t(Op.val(2))));
    break;

  // (dst)(ptr, imm): load from ptr + imm, pointer unchanged.
  case AIE2P::LDA_dms_lda_idx_imm:
    R = scalarLoad(0, access(1, Op.imm(2)), 4, /*Signed=*/false);
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

  // (dst, ptr_out)(ptr, imm): load from ptr, then advance the pointer.
  case AIE2P::LDA_dms_lda_pstm_nrm_imm:
    R = scalarLoad(0, access(2, 0), 4, /*Signed=*/false);
    DefAddr(1, access(2, Op.imm(3)));
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

  // (ptr_out)(src, ptr, imm)
  case AIE2P::ST_dms_sts_pstm_nrm_imm:
    scalarStore(1, access(2, 0), 4);
    DefAddr(0, access(2, Op.imm(3)));
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

std::unique_ptr<AIESemantics>
llvm::AIESim::createSemantics(const MCSubtargetInfo &STI,
                              const MCInstrInfo &MII,
                              const MCRegisterInfo &MRI) {
  if (STI.getTargetTriple().getArch() == Triple::aie2p)
    return std::make_unique<AIE2PSemantics>(MII, MRI);
  return nullptr;
}
