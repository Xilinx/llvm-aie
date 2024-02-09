//===-- AIEBaseISelLowering.cpp - AIE IR Lowering Interface -----*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines the common interfaces that all AIE versions rely on to
// lower IR.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseISelLowering.h"
#include "AIESubtarget.h" // For AIEBaseSubTarget
#include "MCTargetDesc/AIE2MCTargetDesc.h"
#include "MCTargetDesc/AIEMCTargetDesc.h"
#include "llvm/MC/MCRegister.h"

using namespace llvm;

#define DEBUG_TYPE "aie-lower"

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
}

static bool AllocateSplitArg(CCState &State, ArrayRef<MCPhysReg> RegList) {
  auto NumChunks = State.getPendingLocs().size();

  // If registers are given, try to get as many as needed, and fall back to the
  // stack entirely otherwise.
  MCPhysReg RegBegin = State.AllocateRegBlock(RegList, NumChunks);
  if (RegBegin) {
    for (auto &PL : State.getPendingLocs()) {
      PL.convertToReg(RegBegin);
      State.addLoc(PL);
      ++RegBegin;
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
                           AIE2::r5, AIE2::r6, AIE::r7});
}
namespace {
const std::array<std::array<MCPhysReg, 2>, 4> SparseRegPairs = {
    {{AIE2::x0, AIE2::q0},
     {AIE2::x2, AIE2::q2},
     {AIE2::x1, AIE2::q1},
     {AIE2::x3, AIE2::q3}}};
}
ArrayRef<MCPhysReg> AllocateSparseRegPair(CCState &State) {
  for (const std::array<MCPhysReg, 2> &RegPair : SparseRegPairs) {
    if (unsigned Block = State.AllocateRegBlock(RegPair, 2))
      return RegPair;
  }
  return ArrayRef<MCPhysReg>();
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
    auto SparseRegPair = AllocateSparseRegPair(State);
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
      unsigned OffsetMask = State.AllocateStack(16, Align(32));
      State.addLoc(
          CCValAssign::getMem(ValNo, ValVT, OffsetMask, LocVT, LocInfo));
      return true;
    }
  }

  return false; // CC didn't match.
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
  MCPhysReg RegBegin = State.AllocateRegBlock({AIE::r0, AIE::r1}, NumChunks);
  if (!RegBegin) {
    RegBegin = State.AllocateRegBlock({AIE::r6, AIE::r7, AIE::r8}, NumChunks);
  }

  if (!RegBegin) {
#ifndef NDEBUG
    dbgs() << "Could not allocate " << NumChunks
           << " GPRs for split type ValVT=" << EVT(ValVT).getEVTString()
           << " LocVT=" << EVT(LocVT).getEVTString() << '\n';
#endif
    llvm_unreachable("Unable to allocate split type");
  }

  for (auto &PL : State.getPendingLocs()) {
    PL.convertToReg(RegBegin);
    State.addLoc(PL);
    ++RegBegin;
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

#include "AIE2GenCallingConv.inc"
#include "AIEGenCallingConv.inc"
CCAssignFn *AIEBaseTargetLowering::CCAssignFnForCall(bool IsVarArg) const {
  if (Subtarget.isAIE2())
    return IsVarArg ? CC_AIE2_Stack : CC_AIE2;
  else
    return IsVarArg ? CC_AIE_Stack : CC_AIE;
}

CCAssignFn *AIEBaseTargetLowering::CCAssignFnForReturn() const {
  if (Subtarget.isAIE2())
    return RetCC_AIE2;
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

void AIEBaseTargetLowering::alignFirstVASlot(CCState &CCInfo) {
  auto NextOffset = CCInfo.getStackSize();
  auto NextVAOffset = alignTo(NextOffset, Align(32));
  CCInfo.AllocateStack(NextVAOffset - NextOffset, Align(4));
}

MVT AIEBaseTargetLowering::getVectorIdxTy(const DataLayout &DL) const {
  return MVT::i32;
}
