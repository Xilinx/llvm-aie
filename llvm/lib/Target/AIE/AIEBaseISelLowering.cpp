//===-- AIEBaseISelLowering.cpp - AIE IR Lowering Interface -----*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines the common interfaces that all AIE versions rely on to
// lower IR.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseISelLowering.h"
#include "AIEBaseInstrInfo.h"
#include "AIEBaseSubtarget.h"
#include "MCTargetDesc/AIE2MCTargetDesc.h"
#include "MCTargetDesc/AIEMCTargetDesc.h"
#include "MCTargetDesc/aie2p/AIE2PMCTargetDesc.h"
#include "MCTargetDesc/aie2ps/AIE2PSMCTargetDesc.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/IR/RuntimeLibcalls.h"
#include "llvm/MC/MCRegister.h"
using namespace llvm;

#define DEBUG_TYPE "aie-lower"

static cl::opt<bool>
    AllowVecRegMemOps("aie-vect-reg-mem-ops", cl::init(true), cl::Hidden,
                      cl::desc("Allow the usage of vector registers when "
                               "lowering mem[cpy|set|mov]."));

cl::opt<bool>
    VecCCLibcalls("aie-libcalls-preserve-vectors", cl::init(true), cl::Hidden,
                  cl::desc("Assume all vector registers are callee-saved by "
                           "builtin library functions."));

AIEBaseTargetLowering::AIEBaseTargetLowering(const TargetMachine &TM,
                                             const AIEBaseSubtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {

  AIEABI::ABI ABI = Subtarget.getTargetABI();
  assert(ABI == AIEABI::ABI_VITIS && "Only VITIS ABI supported");

  // AIE's compare instructions produce a 0/1
  // Branch/select instructions compare against 0
  setBooleanContents(ZeroOrOneBooleanContent);

  // Functions must be aligned on 16 byte boundaries.
  setMinFunctionAlignment(Align(16));
  setPrefFunctionAlignment(Align(16));

  // Arguments are 32-bit aligned on the stack
  setMinStackArgumentAlignment(getStackArgumentAlignment());

  if (Subtarget.isAIE2() || Subtarget.isAIE2P() || Subtarget.isAIE2PS()) {
    MaxStoresPerMemset = 32;
    MaxStoresPerMemsetOptSize = 16;
    MaxStoresPerMemcpy = 32;
    MaxStoresPerMemcpyOptSize = 16;
    MaxStoresPerMemmove = 32;
    MaxStoresPerMemmoveOptSize = 16;
  }
}

static bool AllocateSplitArg(CCState &State, ArrayRef<MCPhysReg> RegList) {
  auto NumChunks = State.getPendingLocs().size();

  // If registers are given, try to get as many as needed, and fall back to the
  // stack entirely otherwise.
  ArrayRef<MCPhysReg> RegResult = State.AllocateRegBlock(RegList, NumChunks);
  if (!RegResult.empty()) {
    assert(RegResult.size() >= State.getPendingLocs().size());
    for (const auto &[PL, Reg] : zip(State.getPendingLocs(), RegResult)) {
      PL.convertToReg(Reg);
      State.addLoc(PL);
    }
    State.getPendingLocs().clear();
    return true;
  }

  // Otherwise, write everything to the stack.
  // Stack on AIE is growing upwards, meaning the offsets given by AllocateStack
  // will be subtracted from SP. A lower offset therefore corresponds to a
  // higher address. E.g. SP-4 > SP-8
  // To preserve ABI compatibilty, the bits have to be ordered as if there was a
  // single chunk. Therefore, the slots are allocated in reverse order.
  unsigned ChunckSize = 4;
  unsigned AllocSize = ChunckSize * NumChunks;
  unsigned StackOffset = State.AllocateStack(AllocSize, Align(4));
  unsigned NextSlot = StackOffset + AllocSize - ChunckSize;
  for (auto &PL : State.getPendingLocs()) {
    PL.convertToMem(NextSlot);
    State.addLoc(PL);
    NextSlot -= ChunckSize;
  }
  State.getPendingLocs().clear();
  return true;
}

static bool Handle_Split_Arg(unsigned &ValNo, MVT &ValVT, MVT &LocVT,
                             CCValAssign::LocInfo &LocInfo,
                             ISD::ArgFlagsTy &ArgFlags, CCState &State,
                             ArrayRef<MCPhysReg> RegList) {
  assert(LocVT == MVT::i32 && "VT should be split in 32-bits chunks");
  assert(ArgFlags.isSplit() || !State.getPendingLocs().empty());

  State.getPendingLocs().push_back(
      CCValAssign::getPending(ValNo, ValVT, LocVT, LocInfo));
  if (!ArgFlags.isSplitEnd()) {
    // Only start allocating regs/stack when the last chunk is being processed.
    return true;
  }
  return AllocateSplitArg(State, RegList);
}

static bool CC_AIE1_Handle_Split_Arg(unsigned &ValNo, MVT &ValVT, MVT &LocVT,
                                     CCValAssign::LocInfo &LocInfo,
                                     ISD::ArgFlagsTy &ArgFlags,
                                     CCState &State) {
  return Handle_Split_Arg(ValNo, ValVT, LocVT, LocInfo, ArgFlags, State,
                          {AIE::r6, AIE::r7, AIE::r8, AIE::r9});
}

static bool CC_AIEX_Handle_Split_Arg_Stack(unsigned &ValNo, MVT &ValVT,
                                           MVT &LocVT,
                                           CCValAssign::LocInfo &LocInfo,
                                           ISD::ArgFlagsTy &ArgFlags,
                                           CCState &State) {
  return Handle_Split_Arg(ValNo, ValVT, LocVT, LocInfo, ArgFlags, State,
                          /*RegList=*/{});
}

static bool CC_AIE2_Handle_Split_Arg(unsigned &ValNo, MVT &ValVT, MVT &LocVT,
                                     CCValAssign::LocInfo &LocInfo,
                                     ISD::ArgFlagsTy &ArgFlags,
                                     CCState &State) {
  return Handle_Split_Arg(ValNo, ValVT, LocVT, LocInfo, ArgFlags, State,
                          {AIE2::r0, AIE2::r1, AIE2::r2, AIE2::r3, AIE2::r4,
                           AIE2::r5, AIE2::r6, AIE2::r7});
}
namespace {
const std::array<std::array<MCPhysReg, 2>, 4> SparseRegPairs = {
    {{AIE2::x0, AIE2::q0},
     {AIE2::x2, AIE2::q2},
     {AIE2::x1, AIE2::q1},
     {AIE2::x3, AIE2::q3}}};
}

template <typename RegSetType>
static ArrayRef<MCPhysReg> allocateRegs(CCState &State,
                                        const RegSetType &RegSet) {
  constexpr unsigned NumRegs =
      std::tuple_size<typename RegSetType::value_type>::value;

  for (const auto &RegPair : RegSet) {
    if (!State.AllocateRegBlock(RegPair, NumRegs).empty())
      return RegPair;
  }
  return ArrayRef<MCPhysReg>();
}

static ArrayRef<MCPhysReg> allocateSparseRegPair(CCState &State) {
  return allocateRegs(State, SparseRegPairs);
}

namespace {
const std::array<std::array<MCPhysReg, 2>, 12> Bfp16RegPairs576Bit = {
    {{AIE2P::x0, AIE2P::e0},
     {AIE2P::x2, AIE2P::e2},
     {AIE2P::x4, AIE2P::e4},
     {AIE2P::x6, AIE2P::e6},
     {AIE2P::x8, AIE2P::e8},
     {AIE2P::x10, AIE2P::e10},
     {AIE2P::x1, AIE2P::e1},
     {AIE2P::x3, AIE2P::e3},
     {AIE2P::x5, AIE2P::e5},
     {AIE2P::x7, AIE2P::e7},
     {AIE2P::x9, AIE2P::e9},
     {AIE2P::x11, AIE2P::e11}}};

const std::array<std::array<MCPhysReg, 4>, 6> Bfp16RegSet1052Bit = {
    {{AIE2P::x0, AIE2P::x1, AIE2P::e0, AIE2P::e1},
     {AIE2P::x4, AIE2P::x5, AIE2P::e4, AIE2P::e5},
     {AIE2P::x8, AIE2P::x9, AIE2P::e8, AIE2P::e9},
     {AIE2P::x2, AIE2P::x3, AIE2P::e2, AIE2P::e3},
     {AIE2P::x6, AIE2P::x7, AIE2P::e6, AIE2P::e7},
     {AIE2P::x10, AIE2P::x11, AIE2P::e10, AIE2P::e11}}};
} // namespace

static ArrayRef<MCPhysReg> allocateBfp16RegPair(CCState &State) {
  return allocateRegs(State, Bfp16RegPairs576Bit);
}
static ArrayRef<MCPhysReg> allocateBfp16RegSet(CCState &State) {
  return allocateRegs(State, Bfp16RegSet1052Bit);
}
static bool CC_AIE2_SPARSE(unsigned ValNo, MVT ValVT, MVT LocVT,
                           CCValAssign::LocInfo LocInfo,
                           ISD::ArgFlagsTy ArgFlags, CCState &State) {
  if (LocVT == MVT::v64i8 || LocVT == MVT::v32i16 || LocVT == MVT::v16i32 ||
      LocVT == MVT::v32bf16 || LocVT == MVT::v16f32) {
    // Delay assignments until we get both the x and q components of the sparse
    // type
    State.getPendingLocs().push_back(
        CCValAssign::getPending(ValNo, ValVT, LocVT, LocInfo));
    return true;
  }

  if (LocVT == MVT::i128) {
    // Allocate both the pending X register, and the current mask register.
    CCValAssign VecLoc = State.getPendingLocs().front();
    State.getPendingLocs().clear();
    auto SparseRegPair = allocateSparseRegPair(State);
    if (!SparseRegPair.empty()) {
      auto VecReg = SparseRegPair.front();
      auto MaskReg = SparseRegPair.back();
      VecLoc.convertToReg(VecReg);
      State.addLoc(VecLoc);
      State.addLoc(CCValAssign::getReg(ValNo, ValVT, MaskReg, LocVT, LocInfo));
      return true;
    } else {
      VecLoc.convertToMem(State.AllocateStack(64, Align(32)));
      State.addLoc(VecLoc);
      // Offset which was built by llvm is with standard down-growing stacks in
      // mind. So, for allocated vector above we get offset 0. And later we
      // adjust this offset to -64 based on the size to match up-growing AIE
      // stack.
      //
      // But the problem is, as per ABI, mask register must be 32 byte
      // aligned in the stack, and State.AllocateStack(16, Align(16)) will
      // produce an offset of 64 and later when we adjust the offset it becomes
      // -80(-64 - 16), and it is not 32 byte aligned. So we add a fixup of 16
      // to the offset and it matches the ABI. Also, allocated stack size is
      // 16 byte short and we allocate a dummy 16 byte stack .
      unsigned OffsetMask = State.AllocateStack(32, Align(16)) + 16;
      State.addLoc(
          CCValAssign::getMem(ValNo, ValVT, OffsetMask, LocVT, LocInfo));
      return true;
    }
  }

  return false; // CC didn't match.
}

static bool CC_AIE2P_BFP16(unsigned ValNo, MVT ValVT, MVT LocVT,
                           CCValAssign::LocInfo LocInfo,
                           ISD::ArgFlagsTy ArgFlags, CCState &State) {
  unsigned NumElts = State.getPendingLocs().size();
  if (LocVT == MVT::v64i8 || (LocVT == MVT::v8i8 && NumElts == 2)) {
    // Delay assignments until we get  x and e components of the v64bfp16 and
    // x1, x2, e1, e2 for v128bfp16 type
    State.getPendingLocs().push_back(
        CCValAssign::getPending(ValNo, ValVT, LocVT, LocInfo));
    return true;
  }

  if (LocVT == MVT::v8i8 && (NumElts == 1 || NumElts == 3)) {

    CCValAssign MantissaVecLoc0 = State.getPendingLocs().front();
    if (NumElts == 1) {
      // Allocate both the pending X register, and the current exponent
      // register.
      State.getPendingLocs().clear();
      auto BFP16RegPair = allocateBfp16RegPair(State);
      if (!BFP16RegPair.empty()) {
        auto VecReg = BFP16RegPair.front();
        auto ExponentReg = BFP16RegPair.back();
        MantissaVecLoc0.convertToReg(VecReg);
        State.addLoc(MantissaVecLoc0);
        State.addLoc(
            CCValAssign::getReg(ValNo, ValVT, ExponentReg, LocVT, LocInfo));
        return true;
      }
      unsigned Exponent = State.AllocateStack(8, Align(4)) + 56;
      // This extra stack allocation is to compatible with ABI
      State.AllocateStack(8, Align(32));
      MantissaVecLoc0.convertToMem(State.AllocateStack(64, Align(32)));
      State.addLoc(MantissaVecLoc0);
      State.addLoc(CCValAssign::getMem(ValNo, ValVT, Exponent, LocVT, LocInfo));
      return true;

    } else if (NumElts == 3) {
      // Allocate the pending x1, x2 and e1 registers,
      // and the current exponent
      // register.
      CCValAssign MantissaVecLoc1 = State.getPendingLocs()[1];
      CCValAssign ExponentVecLoc1 = State.getPendingLocs()[2];
      State.getPendingLocs().clear();
      auto BFP16RegSet = allocateBfp16RegSet(State);
      if (!BFP16RegSet.empty()) {
        auto VecReg1 = BFP16RegSet.front();
        auto VecReg2 = BFP16RegSet[1];
        auto ExponentReg1 = BFP16RegSet[2];
        auto ExponentReg2 = BFP16RegSet[3];
        MantissaVecLoc0.convertToReg(VecReg1);
        State.addLoc(MantissaVecLoc0);
        MantissaVecLoc1.convertToReg(VecReg2);
        State.addLoc(MantissaVecLoc1);
        ExponentVecLoc1.convertToReg(ExponentReg1);
        State.addLoc(ExponentVecLoc1);
        State.addLoc(
            CCValAssign::getReg(ValNo, ValVT, ExponentReg2, LocVT, LocInfo));
        return true;
      }
      // To compatible with ABI, exponent must allocate first and then mantissa.
      // Also, offset of exponent has to be adjusted to have contiguous memory
      // for bfp16.
      unsigned Exponent2 = State.AllocateStack(8, Align(4)) + 48;
      unsigned Exponent1 = State.AllocateStack(8, Align(32)) + 24;
      unsigned Mantissa2 = State.AllocateStack(64, Align(32));
      unsigned Mantissa1 = State.AllocateStack(64, Align(32));
      MantissaVecLoc0.convertToMem(Mantissa1);
      State.addLoc(MantissaVecLoc0);
      MantissaVecLoc1.convertToMem(Mantissa2);
      State.addLoc(MantissaVecLoc1);
      ExponentVecLoc1.convertToMem(Exponent1);
      State.addLoc(ExponentVecLoc1);
      State.addLoc(
          CCValAssign::getMem(ValNo, ValVT, Exponent2, LocVT, LocInfo));
      return true;
    }
  }

  return false; // CC didn't match.
}
static bool CC_AIE2P_Handle_Consecutive_Regs(unsigned ValNo, MVT ValVT,
                                             MVT LocVT,
                                             CCValAssign::LocInfo LocInfo,
                                             ISD::ArgFlagsTy ArgFlags,
                                             CCState &State) {
  if (CC_AIE2_SPARSE(ValNo, ValVT, LocVT, LocInfo, ArgFlags, State))
    return true;
  return CC_AIE2P_BFP16(ValNo, ValVT, LocVT, LocInfo, ArgFlags, State);
}

enum MXRegSizes {
  MXReg320 = 320,
  MXReg640 = 640,
  MXReg1280 = 1280,
  MXReg384 = 384,
  MXReg768 = 768,
  MXReg1536 = 1536,
  MXReg2560 = 2560
};

namespace {

const std::array<std::array<MCPhysReg, 3>, 24> RegSet320BitMX9 = {
    {{AIE2PS::wl0, AIE2PS::gl0, AIE2PS::el0},
     {AIE2PS::wh0, AIE2PS::gh0, AIE2PS::eh0},
     {AIE2PS::wl1, AIE2PS::gl1, AIE2PS::el1},
     {AIE2PS::wh1, AIE2PS::gh1, AIE2PS::eh1},
     {AIE2PS::wl2, AIE2PS::gl2, AIE2PS::el2},
     {AIE2PS::wh2, AIE2PS::gh2, AIE2PS::eh2},
     {AIE2PS::wl3, AIE2PS::gl3, AIE2PS::el3},
     {AIE2PS::wh3, AIE2PS::gh3, AIE2PS::eh3},
     {AIE2PS::wl4, AIE2PS::gl4, AIE2PS::el4},
     {AIE2PS::wh4, AIE2PS::gh4, AIE2PS::eh4},
     {AIE2PS::wl5, AIE2PS::gl5, AIE2PS::el5},
     {AIE2PS::wh5, AIE2PS::gh5, AIE2PS::eh5},
     {AIE2PS::wl6, AIE2PS::gl6, AIE2PS::el6},
     {AIE2PS::wh6, AIE2PS::gh6, AIE2PS::eh6},
     {AIE2PS::wl7, AIE2PS::gl7, AIE2PS::el7},
     {AIE2PS::wh7, AIE2PS::gh7, AIE2PS::eh7},
     {AIE2PS::wl8, AIE2PS::gl8, AIE2PS::el8},
     {AIE2PS::wh8, AIE2PS::gh8, AIE2PS::eh8},
     {AIE2PS::wl9, AIE2PS::gl9, AIE2PS::el9},
     {AIE2PS::wh9, AIE2PS::gh9, AIE2PS::eh9},
     {AIE2PS::wl10, AIE2PS::gl10, AIE2PS::el10},
     {AIE2PS::wh10, AIE2PS::gh10, AIE2PS::eh10},
     {AIE2PS::wl11, AIE2PS::gl11, AIE2PS::el11},
     {AIE2PS::wh11, AIE2PS::gh11, AIE2PS::eh11}}};

const std::array<std::array<MCPhysReg, 3>, 12> RegSet640BitMX9 = {
    {{AIE2PS::x0, AIE2PS::g0, AIE2PS::e0},
     {AIE2PS::x2, AIE2PS::g2, AIE2PS::e2},
     {AIE2PS::x4, AIE2PS::g4, AIE2PS::e4},
     {AIE2PS::x6, AIE2PS::g6, AIE2PS::e6},
     {AIE2PS::x8, AIE2PS::g8, AIE2PS::e8},
     {AIE2PS::x10, AIE2PS::g10, AIE2PS::e10},
     {AIE2PS::x1, AIE2PS::g1, AIE2PS::e1},
     {AIE2PS::x3, AIE2PS::g3, AIE2PS::e3},
     {AIE2PS::x5, AIE2PS::g5, AIE2PS::e5},
     {AIE2PS::x7, AIE2PS::g7, AIE2PS::e7},
     {AIE2PS::x9, AIE2PS::g9, AIE2PS::e9},
     {AIE2PS::x11, AIE2PS::g11, AIE2PS::e11}}};

const std::array<std::array<MCPhysReg, 6>, 6> RegSet1280BitMX9 = {{
    {AIE2PS::x0, AIE2PS::x1, AIE2PS::g0, AIE2PS::g1, AIE2PS::e0, AIE2PS::e1},
    {AIE2PS::x2, AIE2PS::x3, AIE2PS::g2, AIE2PS::g3, AIE2PS::e2, AIE2PS::e3},
    {AIE2PS::x4, AIE2PS::x5, AIE2PS::g4, AIE2PS::g5, AIE2PS::e4, AIE2PS::e5},
    {AIE2PS::x6, AIE2PS::x7, AIE2PS::g6, AIE2PS::g7, AIE2PS::e6, AIE2PS::e7},
    {AIE2PS::x8, AIE2PS::x9, AIE2PS::g8, AIE2PS::g9, AIE2PS::e8, AIE2PS::e9},
    {AIE2PS::x10, AIE2PS::x11, AIE2PS::g10, AIE2PS::g11, AIE2PS::e10,
     AIE2PS::e11},
}};

const std::array<std::array<MCPhysReg, 12>, 3> RegSet2560BitMX9 = {{
    {AIE2PS::x0, AIE2PS::x1, AIE2PS::x2, AIE2PS::x3, AIE2PS::g0, AIE2PS::g1,
     AIE2PS::g2, AIE2PS::g3, AIE2PS::e0, AIE2PS::e1, AIE2PS::e2, AIE2PS::e3},
    {AIE2PS::x4, AIE2PS::x5, AIE2PS::x6, AIE2PS::x7, AIE2PS::g4, AIE2PS::g5,
     AIE2PS::g6, AIE2PS::g7, AIE2PS::e4, AIE2PS::e5, AIE2PS::e6, AIE2PS::e7},
    {AIE2PS::x8, AIE2PS::x9, AIE2PS::x10, AIE2PS::x11, AIE2PS::g8, AIE2PS::g9,
     AIE2PS::g10, AIE2PS::g11, AIE2PS::e8, AIE2PS::e9, AIE2PS::e10,
     AIE2PS::e11},
}};

const std::array<std::array<MCPhysReg, 4>, 24> RegSet384BitMX6 = {
    {{AIE2PS::wl0, AIE2PS::fl0, AIE2PS::gl0, AIE2PS::el0},
     {AIE2PS::wh0, AIE2PS::fh0, AIE2PS::gh0, AIE2PS::eh0},
     {AIE2PS::wl2, AIE2PS::fl2, AIE2PS::gl2, AIE2PS::el2},
     {AIE2PS::wh2, AIE2PS::fh2, AIE2PS::gh2, AIE2PS::eh2},
     {AIE2PS::wl4, AIE2PS::fl4, AIE2PS::gl4, AIE2PS::el4},
     {AIE2PS::wh4, AIE2PS::fh4, AIE2PS::gh4, AIE2PS::eh4},
     {AIE2PS::wl6, AIE2PS::fl6, AIE2PS::gl6, AIE2PS::el6},
     {AIE2PS::wh6, AIE2PS::fh6, AIE2PS::gh6, AIE2PS::eh6},
     {AIE2PS::wl8, AIE2PS::fl8, AIE2PS::gl8, AIE2PS::el8},
     {AIE2PS::wh8, AIE2PS::fh8, AIE2PS::gh8, AIE2PS::eh8},
     {AIE2PS::wl10, AIE2PS::fl10, AIE2PS::gl10, AIE2PS::el10},
     {AIE2PS::wh10, AIE2PS::fh10, AIE2PS::gh10, AIE2PS::eh10},
     {AIE2PS::wl1, AIE2PS::fl1, AIE2PS::gl1, AIE2PS::el1},
     {AIE2PS::wh1, AIE2PS::fh1, AIE2PS::gh1, AIE2PS::eh1},
     {AIE2PS::wl3, AIE2PS::fl3, AIE2PS::gl3, AIE2PS::el3},
     {AIE2PS::wh3, AIE2PS::fh3, AIE2PS::gh3, AIE2PS::eh3},
     {AIE2PS::wl5, AIE2PS::fl5, AIE2PS::gl5, AIE2PS::el5},
     {AIE2PS::wh5, AIE2PS::fh5, AIE2PS::gh5, AIE2PS::eh5},
     {AIE2PS::wl7, AIE2PS::fl7, AIE2PS::gl7, AIE2PS::el7},
     {AIE2PS::wh7, AIE2PS::fh7, AIE2PS::gh7, AIE2PS::eh7},
     {AIE2PS::wl9, AIE2PS::fl9, AIE2PS::gl9, AIE2PS::el9},
     {AIE2PS::wh9, AIE2PS::fh9, AIE2PS::gh9, AIE2PS::eh9},
     {AIE2PS::wl11, AIE2PS::fl11, AIE2PS::gl11, AIE2PS::el11},
     {AIE2PS::wh11, AIE2PS::fh11, AIE2PS::gh11, AIE2PS::eh11}}};

const std::array<std::array<MCPhysReg, 8>, 12> RegSet768BitMX6 = {{
    {AIE2PS::wl0, AIE2PS::wh0, AIE2PS::fl0, AIE2PS::fh0, AIE2PS::gl0,
     AIE2PS::gh0, AIE2PS::el0, AIE2PS::eh0},
    {AIE2PS::wl2, AIE2PS::wh2, AIE2PS::fl2, AIE2PS::fh2, AIE2PS::gl2,
     AIE2PS::gh2, AIE2PS::el2, AIE2PS::eh2},
    {AIE2PS::wl4, AIE2PS::wh4, AIE2PS::fl4, AIE2PS::fh4, AIE2PS::gl4,
     AIE2PS::gh4, AIE2PS::el4, AIE2PS::eh4},
    {AIE2PS::wl6, AIE2PS::wh6, AIE2PS::fl6, AIE2PS::fh6, AIE2PS::gl6,
     AIE2PS::gh6, AIE2PS::el6, AIE2PS::eh6},
    {AIE2PS::wl8, AIE2PS::wh8, AIE2PS::fl8, AIE2PS::fh8, AIE2PS::gl8,
     AIE2PS::gh8, AIE2PS::el8, AIE2PS::eh8},
    {AIE2PS::wl10, AIE2PS::wh10, AIE2PS::fl10, AIE2PS::fh10, AIE2PS::gl10,
     AIE2PS::gh10, AIE2PS::el10, AIE2PS::eh10},
    {AIE2PS::wl1, AIE2PS::wh1, AIE2PS::fl1, AIE2PS::fh1, AIE2PS::gl1,
     AIE2PS::gh1, AIE2PS::el1, AIE2PS::eh1},
    {AIE2PS::wl3, AIE2PS::wh3, AIE2PS::fl3, AIE2PS::fh3, AIE2PS::gl3,
     AIE2PS::gh3, AIE2PS::el3, AIE2PS::eh3},
    {AIE2PS::wl5, AIE2PS::wh5, AIE2PS::fl5, AIE2PS::fh5, AIE2PS::gl5,
     AIE2PS::gh5, AIE2PS::el5, AIE2PS::eh5},
    {AIE2PS::wl7, AIE2PS::wh7, AIE2PS::fl7, AIE2PS::fh7, AIE2PS::gl7,
     AIE2PS::gh7, AIE2PS::el7, AIE2PS::eh7},
    {AIE2PS::wl9, AIE2PS::wh9, AIE2PS::fl9, AIE2PS::fh9, AIE2PS::gl9,
     AIE2PS::gh9, AIE2PS::el9, AIE2PS::eh9},
    {AIE2PS::wl11, AIE2PS::wh11, AIE2PS::fl11, AIE2PS::fh11, AIE2PS::gl11,
     AIE2PS::gh11, AIE2PS::el11, AIE2PS::eh11},
}};

const std::array<std::array<MCPhysReg, 16>, 6> RegSet1536BitMX6 = {
    {{AIE2PS::wl0, AIE2PS::wh0, AIE2PS::wl1, AIE2PS::wh1, AIE2PS::fl0,
      AIE2PS::fh0, AIE2PS::fl1, AIE2PS::fh1, AIE2PS::gl0, AIE2PS::gh0,
      AIE2PS::gl1, AIE2PS::gh1, AIE2PS::el0, AIE2PS::eh0, AIE2PS::el1,
      AIE2PS::eh1},
     {AIE2PS::wl2, AIE2PS::wh2, AIE2PS::wl3, AIE2PS::wh3, AIE2PS::fl2,
      AIE2PS::fh2, AIE2PS::fl3, AIE2PS::fh3, AIE2PS::gl2, AIE2PS::gh2,
      AIE2PS::gl3, AIE2PS::gh3, AIE2PS::el2, AIE2PS::eh2, AIE2PS::el3,
      AIE2PS::eh3},
     {AIE2PS::wl4, AIE2PS::wh4, AIE2PS::wl5, AIE2PS::wh5, AIE2PS::fl4,
      AIE2PS::fh4, AIE2PS::fl5, AIE2PS::fh5, AIE2PS::gl4, AIE2PS::gh4,
      AIE2PS::gl5, AIE2PS::gh5, AIE2PS::el4, AIE2PS::eh4, AIE2PS::el5,
      AIE2PS::eh5},
     {AIE2PS::wl6, AIE2PS::wh6, AIE2PS::wl7, AIE2PS::wh7, AIE2PS::fl6,
      AIE2PS::fh6, AIE2PS::fl7, AIE2PS::fh7, AIE2PS::gl6, AIE2PS::gh6,
      AIE2PS::gl7, AIE2PS::gh7, AIE2PS::el6, AIE2PS::eh6, AIE2PS::el7,
      AIE2PS::eh7},
     {AIE2PS::wl8, AIE2PS::wh8, AIE2PS::wl9, AIE2PS::wh9, AIE2PS::fl8,
      AIE2PS::fh8, AIE2PS::fl9, AIE2PS::fh9, AIE2PS::gl8, AIE2PS::gh8,
      AIE2PS::gl9, AIE2PS::gh9, AIE2PS::el8, AIE2PS::eh8, AIE2PS::el9,
      AIE2PS::eh9},
     {AIE2PS::wl10, AIE2PS::wh10, AIE2PS::wl11, AIE2PS::wh11, AIE2PS::fl10,
      AIE2PS::fh10, AIE2PS::fl11, AIE2PS::fh11, AIE2PS::gl10, AIE2PS::gh10,
      AIE2PS::gl11, AIE2PS::gh11, AIE2PS::el10, AIE2PS::eh10, AIE2PS::el11,
      AIE2PS::eh11}}};

template <size_t M, size_t N>
std::optional<std::array<MCPhysReg, M>>
AllocateMXReg(CCState &State, std::array<std::array<MCPhysReg, M>, N> RegSet) {
  for (const auto &RegList : RegSet) {
    if (!State.AllocateRegBlock(RegList, M).empty())
      return RegList;
  }
  return std::nullopt;
}

static bool
AllocateMXTyArgsToRegister(CCState &State,
                           SmallVectorImpl<CCValAssign> &PendingMembers,
                           ArrayRef<MCPhysReg> AllocatedRegSet) {
  unsigned NumElts = PendingMembers.size();
  assert(AllocatedRegSet.size() == NumElts &&
         "Number of elements mismatch in BFP type calling convention");
  for (unsigned int Idx = 0; Idx < NumElts; Idx++) {
    auto AllocatedReg = AllocatedRegSet[Idx];
    auto MemberLoc = PendingMembers[Idx];
    MemberLoc.convertToReg(AllocatedReg);
    State.addLoc(MemberLoc);
  }
  State.getPendingLocs().clear();
  return true;
}

// gets the allocated stack size for MX types as per ABI.
unsigned getABIMXTypesStackSizeInBytes(unsigned MXRegSize) {
  switch (MXRegSize) {
  case MXReg320:
  case MXReg384:
    return 64;
  case MXReg640:
  case MXReg768:
    return 128;
  case MXReg1280:
  case MXReg1536:
    return 192;
  case MXReg2560:
    return 320;
  default:
    llvm_unreachable("Illegal MX register size!");
  }
}

unsigned getNumOfElemFromAIE2PSMXType(unsigned MXRegSize) {
  switch (MXRegSize) {
  case MXReg320:
  case MXReg640:
    return 3;
  case MXReg1280:
    return 6;
  case MXReg2560:
    return 12;
  case MXReg384:
    return 4;
  case MXReg768:
    return 8;
  case MXReg1536:
    return 16;
  default:
    return 0;
  }
}
unsigned getAIE2PSAlignmentInBytesFromRegSize(unsigned RegSize) {
  switch (RegSize) {
  case 32:
  case 64:
    return 4;
  case 128:
    return 16;
  case 256:
  case 512:
  case 1024:
  case 2048:
    return 32;
  default:
    llvm_unreachable("Illegal register size!");
  }
}

// Allocate stack to MX type and should match the ABI. As per ABI, MX
// register is copied to contiguous memory, the key is where to start from or
// size of the stack. For example, v64mx9 is of size 80 bytes(640 bits), but as
// per ABI it takes 128 bytes. i.e. in a single stack frame, the offset has to
// be -128. Hence we allocate extra stack, but do not link to any mx type
// members. Also, we allocate members in the opposite direction as they appear
// in struct to match with ABI
bool AllocateMXTyArgsToStack(CCState &State,
                             SmallVectorImpl<CCValAssign> &PendingMembers,
                             unsigned RegSize /*In Bits*/) {
  unsigned NumElts = PendingMembers.size();
  std::vector<unsigned> StackOffset;
  // Create dummy stack to satisfy ABI
  State.AllocateStack(getABIMXTypesStackSizeInBytes(RegSize) - (RegSize / 8),
                      Align(4 /*Default alignment*/));
  for (int Idx = NumElts - 1; Idx >= 0; Idx--) {
    auto MemberLoc = PendingMembers[Idx];
    auto SizeInBytes = MemberLoc.getLocVT().getSizeInBits() / 8;
    StackOffset.push_back(State.AllocateStack(
        SizeInBytes,
        Align(getAIE2PSAlignmentInBytesFromRegSize(SizeInBytes * 8))));
  }
  for (unsigned Idx = 0; Idx < NumElts; Idx++) {
    auto MemberLoc = PendingMembers[Idx];
    MemberLoc.convertToMem(StackOffset[NumElts - (Idx + 1)]);
    State.addLoc(MemberLoc);
  }
  State.getPendingLocs().clear();
  return true;
}

unsigned
getPendingCCValAssignSize(SmallVectorImpl<CCValAssign> &PendingMembers) {
  unsigned Size = 0;
  for (auto It : PendingMembers)
    Size += It.getLocVT().getSizeInBits();
  return Size;
}

bool matchBFPTypeSize(SmallVectorImpl<CCValAssign> &PendingMembers) {
  unsigned Size = getPendingCCValAssignSize(PendingMembers);
  return getNumOfElemFromAIE2PSMXType(Size) == PendingMembers.size();
}
} // namespace

static bool CC_AIE2PS_BFP(unsigned ValNo, MVT ValVT, MVT LocVT,
                          CCValAssign::LocInfo LocInfo,
                          ISD::ArgFlagsTy ArgFlags, CCState &State) {
  SmallVectorImpl<CCValAssign> &PendingMembers = State.getPendingLocs();
  // Add the current argument to pending list
  PendingMembers.push_back(
      CCValAssign::getPending(ValNo, ValVT, LocVT, LocInfo));
  unsigned NumElts = PendingMembers.size();
  assert(NumElts <= 16 && "Not a BFP type!");

  if (NumElts < 3 || !matchBFPTypeSize(PendingMembers)) {
    // Minimum elements in a bfp type is 3 for aie2ps.
    return true;
  }

  auto allocate = [&](auto &RegSet, unsigned RegSize) {
    auto Reg = AllocateMXReg(State, RegSet);
    if (Reg) {
      return AllocateMXTyArgsToRegister(State, PendingMembers, *Reg);
    }
    return AllocateMXTyArgsToStack(State, PendingMembers, RegSize);
  };

  switch (getPendingCCValAssignSize(PendingMembers)) {
  case MXReg320:
    return allocate(RegSet320BitMX9, MXReg320);
  case MXReg640:
    return allocate(RegSet640BitMX9, MXReg640);
  case MXReg1280:
    return allocate(RegSet1280BitMX9, MXReg1280);
  case MXReg2560:
    return allocate(RegSet2560BitMX9, MXReg2560);
  case MXReg384:
    return allocate(RegSet384BitMX6, MXReg384);
  case MXReg768:
    return allocate(RegSet768BitMX6, MXReg768);
  case MXReg1536:
    return allocate(RegSet1536BitMX6, MXReg1536);
  default:
    return false;
  }
}

static bool CC_AIE2PS_Handle_Consecutive_Regs(unsigned ValNo, MVT ValVT,
                                              MVT LocVT,
                                              CCValAssign::LocInfo LocInfo,
                                              ISD::ArgFlagsTy ArgFlags,
                                              CCState &State) {
  return CC_AIE2PS_BFP(ValNo, ValVT, LocVT, LocInfo, ArgFlags, State);
}

static bool CC_AIE2P_Handle_Split_Arg(unsigned &ValNo, MVT &ValVT, MVT &LocVT,
                                      CCValAssign::LocInfo &LocInfo,
                                      ISD::ArgFlagsTy &ArgFlags,
                                      CCState &State) {
  return Handle_Split_Arg(ValNo, ValVT, LocVT, LocInfo, ArgFlags, State,
                          {AIE2P::r0, AIE2P::r1, AIE2P::r2, AIE2P::r3,
                           AIE2P::r4, AIE2P::r5, AIE2P::r6, AIE2P::r7});
}

static bool CC_AIE_Handle_Split_Arg_Ret(unsigned &ValNo, MVT &ValVT, MVT &LocVT,
                                        CCValAssign::LocInfo &LocInfo,
                                        ISD::ArgFlagsTy &ArgFlags,
                                        CCState &State) {
  assert(LocVT == MVT::i32 && "VT should be split in 32-bits chunks");
  assert(ArgFlags.isSplit() || !State.getPendingLocs().empty());

  State.getPendingLocs().push_back(
      CCValAssign::getPending(ValNo, ValVT, LocVT, LocInfo));

  // Only start allocating regs/stack when the last chunk is being processed.
  if (!ArgFlags.isSplitEnd()) {
    return true;
  }

  // Try to get as many 32-bits registers as needed, and fail otherwise.
  // r1 is typically not used for return values, but split types are returned
  // in more than one register. Try r0-r1 before using the other regs.
  // TODO: check the ABI if e.g. r2 should be used with larger types
  auto NumChunks = State.getPendingLocs().size();
  // FIXME: State.AllocateRegBlock returns an ArrayRef slice of its argument,
  //        so we need carefully ensure the lifetime of said arguments, below,
  //        is long enough to reach their use. Bad API design!
  SmallVector<MCPhysReg> R0R1 = {AIE::r0, AIE::r1};
  SmallVector<MCPhysReg> R6R8 = {AIE::r6, AIE::r7, AIE::r8};
  ArrayRef<MCPhysReg> RegResult = State.AllocateRegBlock(R0R1, NumChunks);
  if (RegResult.empty()) {
    RegResult = State.AllocateRegBlock(R6R8, NumChunks);
  }

  if (RegResult.empty()) {
#ifndef NDEBUG
    dbgs() << "Could not allocate " << NumChunks
           << " GPRs for split type ValVT=" << EVT(ValVT).getEVTString()
           << " LocVT=" << EVT(LocVT).getEVTString() << '\n';
#endif
    llvm_unreachable("Unable to allocate split type");
  }

  assert(RegResult.size() >= State.getPendingLocs().size());
  for (const auto &[PL, Reg] : zip(State.getPendingLocs(), RegResult)) {
    PL.convertToReg(Reg);
    State.addLoc(PL);
  }
  State.getPendingLocs().clear();
  return true;
}

static bool CC_AIE_Handle_V2I32_Arg(unsigned &ValNo, MVT &ValVT, MVT &LocVT,
                                    CCValAssign::LocInfo &LocInfo,
                                    ISD::ArgFlagsTy &ArgFlags, CCState &State) {
  static const MCPhysReg RegList[] = {AIE::r6, AIE::r7, AIE::r8, AIE::r9};
  // Try to get the first register.
  if (Register Reg = State.AllocateReg(RegList)) {
    State.addLoc(CCValAssign::getCustomReg(ValNo, ValVT, Reg, LocVT, LocInfo));
  } else {
    // Assign whole thing in stack.
    State.addLoc(CCValAssign::getCustomMem(
        ValNo, ValVT, State.AllocateStack(8, Align(4)), LocVT, LocInfo));
    return true;
  }
  // Try to get the second register.
  if (Register Reg = State.AllocateReg(RegList))
    State.addLoc(CCValAssign::getCustomReg(ValNo, ValVT, Reg, LocVT, LocInfo));
  else
    State.addLoc(CCValAssign::getCustomMem(
        ValNo, ValVT, State.AllocateStack(4, Align(4)), LocVT, LocInfo));
  return true;
}

static bool CC_AIE_Handle_V2I32_Ret(unsigned &ValNo, MVT &ValVT, MVT &LocVT,
                                    CCValAssign::LocInfo &LocInfo,
                                    ISD::ArgFlagsTy &ArgFlags, CCState &State) {
  static const MCPhysReg RegList[] = {AIE::r0, AIE::r1};

  // Try to get the first register.
  if (Register Reg = State.AllocateReg(RegList))
    State.addLoc(CCValAssign::getCustomReg(ValNo, ValVT, Reg, LocVT, LocInfo));
  else
    return false;

  // Try to get the second register.
  if (Register Reg = State.AllocateReg(RegList))
    State.addLoc(CCValAssign::getCustomReg(ValNo, ValVT, Reg, LocVT, LocInfo));
  else
    return false;
  return true;
}

static bool CC_AIE2PS_Handle_Split_Arg(unsigned &ValNo, MVT &ValVT, MVT &LocVT,
                                       CCValAssign::LocInfo &LocInfo,
                                       ISD::ArgFlagsTy &ArgFlags,
                                       CCState &State) {
  return Handle_Split_Arg(ValNo, ValVT, LocVT, LocInfo, ArgFlags, State,
                          {AIE2PS::r0, AIE2PS::r1, AIE2PS::r2, AIE2PS::r3,
                           AIE2PS::r4, AIE2PS::r5, AIE2PS::r6, AIE2PS::r7});
}

#include "AIE2GenCallingConv.inc"
#include "AIE2PGenCallingConv.inc"
#include "AIE2PSGenCallingConv.inc"
#include "AIEGenCallingConv.inc"
CCAssignFn *AIEBaseTargetLowering::CCAssignFnForCall(bool IsVarArg) const {
  if (Subtarget.isAIE2())
    return IsVarArg ? CC_AIE2_Stack : CC_AIE2;
  else if (Subtarget.isAIE2P())
    return IsVarArg ? CC_AIE2P_Stack : CC_AIE2P;
  else if (Subtarget.isAIE2PS())
    return IsVarArg ? CC_AIE2PS_Stack : CC_AIE2PS;
  else
    return IsVarArg ? CC_AIE_Stack : CC_AIE;
}

CCAssignFn *AIEBaseTargetLowering::CCAssignFnForReturn() const {
  if (Subtarget.isAIE2())
    return RetCC_AIE2;
  else if (Subtarget.isAIE2P())
    return RetCC_AIE2P;
  else if (Subtarget.isAIE2PS())
    return RetCC_AIE2PS;
  else
    return RetCC_AIE;
}
/// isEligibleForTailCallOptimization - Check whether the call is eligible
/// for tail call optimization.
/// Note: This is modelled after ARM's IsEligibleForTailCallOptimization.
bool AIEBaseTargetLowering::isEligibleForTailCallOptimization(
    CCState &CCInfo, CallLoweringInfo &CLI, MachineFunction &MF,
    const SmallVector<CCValAssign, 16> &ArgLocs) const {

  auto &Callee = CLI.Callee;
  auto CalleeCC = CLI.CallConv;
  auto IsVarArg = CLI.IsVarArg;
  auto &Outs = CLI.Outs;
  auto &Caller = MF.getFunction();
  auto CallerCC = Caller.getCallingConv();

  // Do not tail call opt functions with "disable-tail-calls" attribute.
  if (Caller.getFnAttribute("disable-tail-calls").getValueAsString() == "true")
    return false;

  // Exception-handling functions need a special set of instructions to
  // indicate a return to the hardware. Tail-calling another function would
  // probably break this.
  // TODO: The "interrupt" attribute isn't currently defined by AIE. This
  // should be expanded as new function attributes are introduced.
  if (Caller.hasFnAttribute("interrupt"))
    return false;

  // Do not tail call opt functions with varargs.
  if (IsVarArg)
    return false;

  // Do not tail call opt if the stack is used to pass parameters.
  if (CCInfo.getStackSize() != 0)
    return false;

  // Do not tail call opt if any parameters need to be passed indirectly.
  // Since long doubles (fp128) and i128 are larger than 2*XLEN, they are
  // passed indirectly. So the address of the value will be passed in a
  // register, or if not available, then the address is put on the stack. In
  // order to pass indirectly, space on the stack often needs to be allocated
  // in order to store the value. In this case the CCInfo.getStackSize()
  // != 0 check is not enough and we need to check if any CCValAssign ArgsLocs
  // are passed CCValAssign::Indirect.
  for (auto &VA : ArgLocs)
    if (VA.getLocInfo() == CCValAssign::Indirect)
      return false;

  // Do not tail call opt if either caller or callee uses struct return
  // semantics.
  auto IsCallerStructRet = Caller.hasStructRetAttr();
  auto IsCalleeStructRet = Outs.empty() ? false : Outs[0].Flags.isSRet();
  if (IsCallerStructRet || IsCalleeStructRet)
    return false;

  // Externally-defined functions with weak linkage should not be
  // tail-called. The behaviour of branch instructions in this situation (as
  // used for tail calls) is implementation-defined, so we cannot rely on the
  // linker replacing the tail call with a return.
  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee)) {
    const GlobalValue *GV = G->getGlobal();
    if (GV->hasExternalWeakLinkage())
      return false;
  }

  // The callee has to preserve all registers the caller needs to preserve.
  const TargetRegisterInfo *TRI = Subtarget.getRegisterInfo();
  const uint32_t *CallerPreserved = TRI->getCallPreservedMask(MF, CallerCC);
  if (CalleeCC != CallerCC) {
    const uint32_t *CalleePreserved = TRI->getCallPreservedMask(MF, CalleeCC);
    if (!TRI->regmaskSubsetEqual(CallerPreserved, CalleePreserved))
      return false;
  }

  // Byval parameters hand the function a pointer directly into the stack area
  // we want to reuse during a tail call. Working around this *is* possible
  // but less efficient and uglier in LowerCall.
  for (auto &Arg : Outs)
    if (Arg.Flags.isByVal())
      return false;

  return true;
}

LLT AIEBaseTargetLowering::getOptimalMemOpLLT(
    const MemOp &Op, const AttributeList &FuncAttributes) const {

  if (Subtarget.isAIE2P() || Subtarget.isAIE2PS()) {
    if (AllowVecRegMemOps && Op.size() >= 64 && Op.isAligned(Align(64)))
      return LLT::fixed_vector(16, 32);
  }
  if (Subtarget.isAIE2() || Subtarget.isAIE2P() || Subtarget.isAIE2PS()) {
    if (AllowVecRegMemOps && Op.size() >= 32 && Op.isAligned(Align(32)))
      return LLT::fixed_vector(8, 32);
    if (AllowVecRegMemOps && Op.size() >= 16 && Op.isAligned(Align(16)))
      return LLT::fixed_vector(4, 32);
    if (Op.size() >= 4 && Op.isAligned(Align(4)))
      return LLT::scalar(32);
    if (Op.size() >= 2 && Op.isAligned(Align(2)))
      return LLT::scalar(16);
  }

  return LLT();
}

void AIEBaseTargetLowering::alignFirstVASlot(CCState &CCInfo) {
  auto NextOffset = CCInfo.getStackSize();
  auto NextVAOffset = alignTo(NextOffset, Align(32));
  CCInfo.AllocateStack(NextVAOffset - NextOffset, Align(4));
}

MVT AIEBaseTargetLowering::getVectorIdxTy(const DataLayout &DL) const {
  return MVT::i32;
}

unsigned AIEBaseTargetLowering::getNumRegistersForCallingConv(
    LLVMContext &Context, CallingConv::ID CC, EVT VT) const {
  if (VT == MVT::i64 || VT == MVT::f64) {
    return 2;
  }
  return TargetLowering::getNumRegistersForCallingConv(Context, CC, VT);
}

MVT AIEBaseTargetLowering::getRegisterTypeForCallingConvAssignment(
    LLVMContext &Context, CallingConv::ID CC, EVT VT) const {
  if (VT == MVT::i64 || VT == MVT::f64)
    return MVT::i32;

  return TargetLowering::getRegisterTypeForCallingConv(Context, CC, VT);
}

MVT AIEBaseTargetLowering::getRegisterTypeForCallingConv(LLVMContext &Context,
                                                         CallingConv::ID CC,
                                                         EVT VT) const {
  if (VT == MVT::i64 || VT == MVT::f64)
    return MVT::i32;

  return TargetLowering::getRegisterTypeForCallingConv(Context, CC, VT);
}

bool AIEBaseTargetLowering::shouldLocalize(
    const MachineInstr &MI, const TargetTransformInfo *TTI) const {
  const MachineFunction &MF = *MI.getMF();
  const auto *TII =
      static_cast<const AIEBaseInstrInfo *>(MF.getSubtarget().getInstrInfo());

  // The target broadcast pseudo is a cheap constant-splat materialization;
  // sink it close to its uses so its live range does not cross calls.
  if (MI.getOpcode() == TII->getGenericBroadcastVectorOpcode())
    return true;

  // A G_BUILD_VECTOR whose operands are all identical is a splat, i.e. another
  // cheaply rematerializable constant-like value worth localizing.
  if (MI.getOpcode() == TargetOpcode::G_BUILD_VECTOR) {
    const Register Src = MI.getOperand(1).getReg();
    bool IsSplat = true;
    for (unsigned I = 2, E = MI.getNumOperands(); I < E; ++I)
      if (MI.getOperand(I).getReg() != Src) {
        IsSplat = false;
        break;
      }
    if (IsSplat)
      return true;
  }

  return TargetLoweringBase::shouldLocalize(MI, TTI);
}
