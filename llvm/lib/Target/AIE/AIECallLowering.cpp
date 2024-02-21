//===- AIECallLowering.cpp - Call lowering --------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
/// \file
/// This file implements the lowering of LLVM calls to machine code calls for
/// GlobalISel.
//
//===----------------------------------------------------------------------===//

#include "AIECallLowering.h"
#include "AIEBaseISelLowering.h"
#include "AIEInstrInfo.h"
#include "AIEMachineFunctionInfo.h"
#include "AIESubtarget.h"
#include "MCTargetDesc/AIE2MCTargetDesc.h"
#include "MCTargetDesc/AIEMCTargetDesc.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/Analysis.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/GlobalISel/Utils.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include <cstdint>

#define DEBUG_TYPE "aie-call-lowering"

using namespace llvm;

AIECallLowering::AIECallLowering(const TargetLowering &TLI)
    : CallLowering(&TLI) {}

namespace {

/// AIE-specific assigner that can switch the CC for each argument based on
/// whether it's a vararg or not.
struct AIEValueAssigner : public CallLowering::ValueAssigner {
  AIEValueAssigner(bool IsIncoming, CCAssignFn *AssignFn,
                   CCAssignFn *AssignFnVarArg = nullptr)
      : ValueAssigner(IsIncoming, AssignFn, AssignFnVarArg) {}

  bool assignArg(unsigned ValNo, EVT OrigVT, MVT ValVT, MVT LocVT,
                 CCValAssign::LocInfo LocInfo,
                 const CallLowering::ArgInfo &Info, ISD::ArgFlagsTy Flags,
                 CCState &State) override {
    if (!Info.IsFixed && !UsesVarargCC) {
      AIEBaseTargetLowering::alignFirstVASlot(State);
      UsesVarargCC = true;
    }
    if (getAssignFn(!Info.IsFixed)(ValNo, ValVT, LocVT, LocInfo, Flags, State))
      return true;
    StackSize = State.getStackSize();
    return false;
  }

private:
  bool UsesVarargCC = false;
};

/// Helper class for values going out through an ABI boundary (used for handling
/// function return values and call parameters).
struct AIEOutgoingValueHandler : public CallLowering::OutgoingValueHandler {
  AIEOutgoingValueHandler(MachineIRBuilder &MIRBuilder,
                          MachineRegisterInfo &MRI, MachineInstrBuilder &MIB)
      : OutgoingValueHandler(MIRBuilder, MRI), MIB(MIB) {}

  Register getStackAddress(uint64_t Size, int64_t Offset,
                           MachinePointerInfo &MPO,
                           ISD::ArgFlagsTy Flags) override {
    auto SizeInSlots =
        alignTo(Size, AIEBaseTargetLowering::getStackArgumentAlignment());
    assert(
        isAligned(AIEBaseTargetLowering::getStackArgumentAlignment(), Offset) &&
        "Stack offset is not aligned");
    MachineFunction &MF = MIRBuilder.getMF();
    LLT PtrTy = LLT::pointer(0, 20);
    LLT S32 = LLT::scalar(32);
    if (!SPReg) {
      auto *TRI =
          static_cast<const AIEBaseRegisterInfo *>(MRI.getTargetRegisterInfo());
      SPReg =
          MIRBuilder.buildCopy(PtrTy, TRI->getStackPointerRegister()).getReg(0);
    }

    // AIE stack grows up, fix the offset which was built by llvm
    // with standard down-growing stacks in mind.
    Offset = -Offset - int64_t(SizeInSlots);

    auto OffsetReg = MIRBuilder.buildConstant(S32, Offset);
    auto AddrReg = MIRBuilder.buildPtrAdd(PtrTy, SPReg, OffsetReg);

    MPO = MachinePointerInfo::getStack(MF, Offset);
    return AddrReg.getReg(0);
  }

  void assignValueToReg(Register ValVReg, Register PhysReg,
                        const CCValAssign &VA) override {
    assert(VA.isRegLoc() && "Value shouldn't be assigned to reg");
    assert(VA.getLocReg() == PhysReg && "Assigning to the wrong reg?");

    Register ExtReg = extendRegister(ValVReg, VA);
    MIRBuilder.buildCopy(PhysReg, ExtReg);
    MIB.addUse(PhysReg, RegState::Implicit);
  }

  void assignValueToAddress(Register ValVReg, Register Addr,
                            LLT MemTy /*uint64_t Size*/,
                            const MachinePointerInfo &MPO,
                            const CCValAssign &VA) override {
    MachineFunction &MF = MIRBuilder.getMF();
    uint64_t LocMemOffset = VA.getLocMemOffset();
    auto *MMO =
        MF.getMachineMemOperand(MPO, MachineMemOperand::MOStore, MemTy,
                                commonAlignment(Align(32), LocMemOffset));
    MIRBuilder.buildStore(ValVReg, Addr, *MMO);
    }

  MachineInstrBuilder &MIB;
  // Cache the SP register vreg if we need it more than once in this call site.
  Register SPReg;
};

} // end anonymous namespace

SmallVector<CallLowering::ArgInfo, 4> AIECallLowering::createSplitRetInfos(
    const Value &RetVal, ArrayRef<Register> VRegs, const Function &F) const {
  const DataLayout &DL = F.getParent()->getDataLayout();
  ArgInfo OrigRetInfo(VRegs, RetVal.getType(), 0);
  setArgFlags(OrigRetInfo, AttributeList::ReturnIndex, DL, F);

  SmallVector<ArgInfo, 4> SplitRetInfos;
  splitToValueTypes(OrigRetInfo, SplitRetInfos, DL, F.getCallingConv());
  return SplitRetInfos;
}

AIECallLowering::ArgAssignments::ArgAssignments(MachineFunction &MF,
                                                ArrayRef<ArgInfo> ArgInfos)
    : ArgInfos(ArgInfos.begin(), ArgInfos.end()),
      CCInfo(MF.getFunction().getCallingConv(), MF.getFunction().isVarArg(), MF,
             ArgLocs, MF.getFunction().getContext()) {}

SmallVector<Register, 4>
AIECallLowering::ArgAssignments::getAssignedRegisters() const {
  SmallVector<Register, 4> Regs;
  for (const CCValAssign &Assignment : ArgLocs) {
    if (Assignment.isRegLoc()) {
      Regs.push_back(Assignment.getLocReg());
    }
  }
  return Regs;
}

void AIECallLowering::ArgAssignments::reserveRegisters(
    ArrayRef<Register> Regs) {
  for (Register Reg : Regs) {
    CCInfo.AllocateReg(Reg);
  }
}

unsigned AIECallLowering::ArgAssignments::getAssignedStackSize() const {
  return CCInfo.getStackSize();
}

void AIECallLowering::ArgAssignments::reserveStackSize(unsigned Size) {
  CCInfo.AllocateStack(Size, Align(1));
}

bool AIECallLowering::determineAndLegalizeAssignments(
    ArgAssignments &AA, ValueAssigner &Assigner) const {
  auto SetLocVT = [](CCValAssign &Loc, MVT NewVT) {
    assert(Loc.isRegLoc());
    Loc = CCValAssign::getReg(Loc.getValNo(), NewVT, Loc.getLocReg(), NewVT,
                              Loc.getLocInfo());
  };

  if (!CallLowering::determineAssignments(Assigner, AA.getArgInfos(),
                                          AA.getCCInfo()))
    return false;

  // We might have determined the argument locations using illegal types (e.g.
  // 128-bits vectors on AIE2).
  const auto *TLI = getTLI<AIEBaseTargetLowering>();
  for (CCValAssign &Loc : AA.getArgLocs()) {
    if (!Loc.isRegLoc())
      continue;
    MVT NewVT = TLI->getRegisterTypeForCallingConvAssignment(
        AA.getCCInfo().getContext(), AA.getCCInfo().getCallingConv(),
        Loc.getLocVT());
    if (NewVT != Loc.getLocVT()) {
      SetLocVT(Loc, NewVT);
    }
  }
  return true;
}

bool AIECallLowering::handleAssignments(ArgAssignments &AA,
                                        ValueHandler &Handler,
                                        MachineIRBuilder &MIRBuilder) const {
  return CallLowering::handleAssignments(
      Handler, AA.getArgInfos(), AA.getCCInfo(), AA.getArgLocs(), MIRBuilder);
}

/// Lower the return value for the already existing \p Ret. This assumes that
/// \p MIRBuilder's insertion point is correct.
bool AIECallLowering::lowerReturnVal(
    MachineIRBuilder &MIRBuilder, const Value *Val, ArrayRef<Register> VRegs,
    FunctionLoweringInfo::SavedRetCCState &RetAssignments,
    MachineInstrBuilder &Ret) const {
  assert(RetAssignments.RetVal == Val);
  if (!Val)
    // Nothing to do here.
    return true;

  auto &MF = MIRBuilder.getMF();
  ArgAssignments AA(MF, createSplitRetInfos(*Val, VRegs, MF.getFunction()));

  CCAssignFn *AssignFn = getTLI<AIEBaseTargetLowering>()->CCAssignFnForReturn();
  AIEValueAssigner Assigner(/*IsIncoming=*/false, AssignFn);
  if (!determineAndLegalizeAssignments(AA, Assigner)) {
    return false;
  }
  assert(AA.getAssignedRegisters() == RetAssignments.AssignedRegs &&
         AA.getAssignedStackSize() == RetAssignments.ReservedStackSize);

  for (Register Reg : AA.getAssignedRegisters())
    MF.getRegInfo().disableCalleeSavedRegister(Reg);

  AIEOutgoingValueHandler Handler(MIRBuilder, MF.getRegInfo(), Ret);
  return handleAssignments(AA, Handler, MIRBuilder);
}

bool AIECallLowering::preLowerReturn(const Value *RetVal,
                                     ArrayRef<Register> VRegs,
                                     FunctionLoweringInfo &FLI) const {
  if (!RetVal) {
    FLI.PreDeterminedRetAssignments = {RetVal, /*AssignedRegs=*/{},
                                       /*ReservedStackSize=*/0};
    return true;
  }

  MachineFunction &MF = *FLI.MF;
  ArgAssignments AA(MF, createSplitRetInfos(*RetVal, VRegs, MF.getFunction()));

  CCAssignFn *AssignFn = getTLI<AIEBaseTargetLowering>()->CCAssignFnForReturn();
  AIEValueAssigner Assigner(/*IsIncoming=*/false, AssignFn);
  if (!determineAndLegalizeAssignments(AA, Assigner)) {
    return false;
  }

  // Save the return assignments so they can be picked up when lowering
  // formal arguments.
  FLI.PreDeterminedRetAssignments = {RetVal, AA.getAssignedRegisters(),
                                     AA.getAssignedStackSize()};
  return true;
}

bool AIECallLowering::lowerReturn(MachineIRBuilder &MIRBuilder,
                                  const Value *Val, ArrayRef<Register> VRegs,
                                  FunctionLoweringInfo &FLI) const {
  assert(!Val == VRegs.empty() && "Return value without a vreg");
  auto &TII = static_cast<const AIEBaseInstrInfo &>(MIRBuilder.getTII());
  auto Ret = MIRBuilder.buildInstrNoInsert(TII.getReturnOpcode());

  if (!lowerReturnVal(MIRBuilder, Val, VRegs, *FLI.PreDeterminedRetAssignments,
                      Ret))
    return false;

  MIRBuilder.insertInstr(Ret);
  return true;
}

namespace {
struct AIEIncomingValueHandler : public CallLowering::IncomingValueHandler {
  AIEIncomingValueHandler(MachineIRBuilder &MIRBuilder,
                          MachineRegisterInfo &MRI)
      : IncomingValueHandler(MIRBuilder, MRI) {}

  Register getStackAddress(uint64_t Size, int64_t Offset,
                           MachinePointerInfo &MPO,
                           ISD::ArgFlagsTy Flags) override {
    auto SizeInSlots =
        alignTo(Size, AIEBaseTargetLowering::getStackArgumentAlignment());
    assert(
        isAligned(AIEBaseTargetLowering::getStackArgumentAlignment(), Offset) &&
        "Stack offset is not aligned");

    auto &MFI = MIRBuilder.getMF().getFrameInfo();

    // Byval is assumed to be writable memory, but other stack passed arguments
    // are not.
    const bool IsImmutable = !Flags.isByVal();

    // AIE stack grows up, fix the offset which was built by llvm
    // with standard down-growing stacks in mind.
    Offset = -Offset - int64_t(SizeInSlots);

    int FI = MFI.CreateFixedObject(Size, Offset, IsImmutable);
    MPO = MachinePointerInfo::getFixedStack(MIRBuilder.getMF(), FI);

    return MIRBuilder.buildFrameIndex(LLT::pointer(MPO.getAddrSpace(), 20), FI)
        .getReg(0);
  }

  void assignValueToAddress(Register ValVReg, Register Addr, LLT MemTy,
                            const MachinePointerInfo &MPO,
                            const CCValAssign &VA) override {
    MachineFunction &MF = MIRBuilder.getMF();

    auto MMO = MF.getMachineMemOperand(
        MPO, MachineMemOperand::MOLoad | MachineMemOperand::MOInvariant, MemTy,
        inferAlignFromPtrInfo(MF, MPO));
    MIRBuilder.buildLoad(ValVReg, Addr, *MMO);
  }

  void assignValueToReg(Register ValVReg, Register PhysReg,
                        const CCValAssign &VA) override {
    markPhysRegUsed(PhysReg);
    IncomingValueHandler::assignValueToReg(ValVReg, PhysReg, VA);
  }

  /// Marking a physical register as used is different between formal
  /// parameters, where it's a basic block live-in, and call returns, where it's
  /// an implicit-def of the call instruction.
  virtual void markPhysRegUsed(unsigned PhysReg) = 0;
};

struct FormalArgHandler : public AIEIncomingValueHandler {
  FormalArgHandler(MachineIRBuilder &MIRBuilder, MachineRegisterInfo &MRI)
      : AIEIncomingValueHandler(MIRBuilder, MRI) {}

  void markPhysRegUsed(unsigned PhysReg) override {
    MIRBuilder.getMBB().addLiveIn(PhysReg);
  }
};

struct CallReturnHandler : public AIEIncomingValueHandler {
  CallReturnHandler(MachineIRBuilder &MIRBuilder, MachineRegisterInfo &MRI,
                    MachineInstrBuilder MIB)
      : AIEIncomingValueHandler(MIRBuilder, MRI), MIB(MIB) {}

  void markPhysRegUsed(unsigned PhysReg) override {
    MIB.addDef(PhysReg, RegState::Implicit);
  }

  MachineInstrBuilder MIB;
};
} // namespace

bool AIECallLowering::lowerFormalArguments(MachineIRBuilder &MIRBuilder,
                                           const Function &F,
                                           ArrayRef<ArrayRef<Register>> VRegs,
                                           FunctionLoweringInfo &FLI) const {
  auto &TLI = *getTLI<AIEBaseTargetLowering>();
  MachineFunction &MF = MIRBuilder.getMF();
  auto &FunctionInfo = *MF.getInfo<AIEMachineFunctionInfo>();

  if (F.arg_empty())
    return true;

  auto &MBB = MIRBuilder.getMBB();
  const auto &DL = MF.getDataLayout();

  SmallVector<ArgInfo, 8> SplitArgInfos;
  unsigned Idx = 0;
  for (const auto &Arg : F.args()) {
    ArgInfo OrigArgInfo(VRegs[Idx], Arg.getType(), Idx);

    setArgFlags(OrigArgInfo, Idx + AttributeList::FirstArgIndex, DL, F);
    splitToValueTypes(OrigArgInfo, SplitArgInfos, DL, F.getCallingConv());

    Idx++;
  }

  ArgAssignments AA(MF, SplitArgInfos);
  if (FLI.PreDeterminedRetAssignments.has_value()) {
    // The registers/slots used by the result type cannot be re-used for args.
    FunctionLoweringInfo::SavedRetCCState &RetAssignments =
        *FLI.PreDeterminedRetAssignments;
    AA.reserveRegisters(RetAssignments.AssignedRegs);
    AA.reserveStackSize(RetAssignments.ReservedStackSize);
  }

  if (!MBB.empty())
    MIRBuilder.setInstr(*MBB.begin());

  FormalArgHandler ArgHandler(MIRBuilder, MIRBuilder.getMF().getRegInfo());
  CCAssignFn *AssignFn = TLI.CCAssignFnForCall();
  AIEValueAssigner Assigner(/*IsIncoming=*/true, AssignFn);
  if (!determineAndLegalizeAssignments(AA, Assigner) ||
      !handleAssignments(AA, ArgHandler, MIRBuilder))
    return false;

  for (Register Reg : AA.getAssignedRegisters())
    MF.getRegInfo().disableCalleeSavedRegister(Reg);

  if (F.isVarArg()) {
    uint64_t StackSize = Assigner.StackSize;

    // Offset of the end address of the first vararg from stack pointer
    // The stack grows up, and we allocate close to SP first, so we cannot know
    // the begin address. This would be (VaArgEndOffset - AlignedVarArgSize),
    // but AlignedVarArgSize is only known when meeting a va_arg instruction.
    int FirstVAEndOffset = -alignTo(StackSize, Align(32));

    // Record the frame index for FirstVAEndOffset, it will be used by VASTART.
    auto &MFI = MIRBuilder.getMF().getFrameInfo();
    int FI = MFI.CreateFixedObject(4, FirstVAEndOffset, true);
    FunctionInfo.setVarArgsFrameIndex(FI);
  }

  MIRBuilder.setMBB(MBB);
  return true;
}

static const uint32_t *
determineCallPreservedMask(MachineFunction &MF,
                           CallLowering::CallLoweringInfo &Info,
                           ArrayRef<Register> CCRegs) {
  auto &SubTarget = MF.getSubtarget();
  const TargetRegisterInfo &TRI = *SubTarget.getRegisterInfo();
  unsigned MaskSize = MachineOperand::getRegMaskSize(TRI.getNumRegs());
  ArrayRef<uint32_t> DefaultMask(TRI.getCallPreservedMask(MF, Info.CallConv),
                                 MaskSize);
  SmallVector<uint32_t, 8> Mask(DefaultMask);

  /// Unset a register from Mask
  auto UnsetReg = [&Mask](MCRegister Reg) {
    Mask[Reg / 32] &= ~(1u << (Reg % 32));
  };

  // Make sure all sub registers of the CC registers are unset in Mask
  for (Register CCReg : CCRegs)
    for (MCSubRegIterator SubReg(CCReg.asMCReg(), &TRI, /*IncludeSelf=*/true);
         SubReg.isValid(); ++SubReg)
      UnsetReg(*SubReg);

  // In case the mask did not change, re-use the default pointer so the
  // MIRPrinter writes a nice name for it.
  if (Mask == DefaultMask)
    return DefaultMask.data();

  uint32_t *CustomMask = MF.allocateRegMask();
  copy(Mask, CustomMask);
  return CustomMask;
}

bool AIECallLowering::lowerCall(MachineIRBuilder &MIRBuilder,
                                CallLoweringInfo &Info) const {

  // We must emit a tail call if we have musttail.
  if (Info.IsMustTailCall) {
    LLVM_DEBUG(dbgs() << "Must-tail calls not supported yet.\n");
    return false;
  }

  MachineFunction &MF = MIRBuilder.getMF();
  const Function &F = MF.getFunction();
  MachineRegisterInfo &MRI = MF.getRegInfo();
  const AIEBaseTargetLowering &TLI = *getTLI<AIEBaseTargetLowering>();
  const DataLayout &DL = F.getParent()->getDataLayout();

  SmallVector<ArgInfo, 8> OutArgs;
  for (auto &OrigArg : Info.OrigArgs)
    splitToValueTypes(OrigArg, OutArgs, DL, Info.CallConv);

  SmallVector<ArgInfo, 8> InArgs;
  if (!Info.OrigRet.Ty->isVoidTy())
    splitToValueTypes(Info.OrigRet, InArgs, DL, Info.CallConv);

  // Pre-determine the assignments for the return type, they will affect
  // the parameter assignments.
  ArgAssignments InAA(MF, InArgs);
  if (!Info.OrigRet.Ty->isVoidTy()) {
    CCAssignFn *RetAssignFn = TLI.CCAssignFnForReturn();
    AIEValueAssigner Assigner(/*IsIncoming=*/true, RetAssignFn);
    if (!determineAndLegalizeAssignments(InAA, Assigner))
      return false;
  }

  const TargetSubtargetInfo &Subtarget = MF.getSubtarget();
  auto &TII = *static_cast<const AIEBaseInstrInfo *>(Subtarget.getInstrInfo());
  MachineInstrBuilder CallSeqStart =
      MIRBuilder.buildInstr(TII.getCallFrameSetupOpcode());

  // Create a temporarily-floating call instruction so we can add the implicit
  // uses of arg registers.
  unsigned Opc = TII.getCallOpcode(MF, Info.Callee.isReg(), false);
  auto MIB = MIRBuilder.buildInstrNoInsert(Opc);
  MIB.add(Info.Callee);

  // The registers/slots used by the result type cannot be re-used for args.
  ArgAssignments OutAA(MF, OutArgs);
  OutAA.reserveRegisters(InAA.getAssignedRegisters());
  OutAA.reserveStackSize(InAA.getAssignedStackSize());

  CCAssignFn *AssignFnFixed = TLI.CCAssignFnForCall(/*IsVarArg=*/false);
  CCAssignFn *AssignFnVa = TLI.CCAssignFnForCall(/*IsVarArg=*/true);
  AIEValueAssigner Assigner(/*IsIncoming=*/false, AssignFnFixed, AssignFnVa);
  AIEOutgoingValueHandler Handler(MIRBuilder, MRI, MIB);
  if (!determineAndLegalizeAssignments(OutAA, Assigner) ||
      !handleAssignments(OutAA, Handler, MIRBuilder))
    return false;

  SmallVector<Register, 8> CCRegs(InAA.getAssignedRegisters());
  CCRegs.append(OutAA.getAssignedRegisters());
  const uint32_t *Mask = determineCallPreservedMask(MF, Info, CCRegs);
  MIB.addRegMask(Mask);
  MIRBuilder.insertInstr(MIB);

  const auto *TRI = Subtarget.getRegisterInfo();
  if (Info.Callee.isReg())
    constrainOperandRegClass(MF, *TRI, MRI, *Subtarget.getInstrInfo(),
                             *Subtarget.getRegBankInfo(), *MIB, MIB->getDesc(),
                             Info.Callee, 0);

  if (!Info.OrigRet.Ty->isVoidTy()) {
    // Re-use the assignments determined in InAA.
    CallReturnHandler Handler(MIRBuilder, MRI, MIB);
    if (!handleAssignments(InAA, Handler, MIRBuilder))
      return false;
  }

  CallSeqStart.addImm(Assigner.StackSize).addImm(0);
  MIRBuilder.buildInstr(TII.getCallFrameDestroyOpcode())
      .addImm(Assigner.StackSize)
      .addImm(0);

  return true;
}
