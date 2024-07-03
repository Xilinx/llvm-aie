//===- MFCommon.inc - Helpers for unit-testing the backend ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2024 Advanced Micro Devices, Inc. or its
// affiliates
//
//===----------------------------------------------------------------------===//

// Add a few Bogus backend classes so we can create MachineInstrs without
// depending on a real target.
class BogusTargetLowering : public TargetLowering {
public:
  BogusTargetLowering(TargetMachine &TM) : TargetLowering(TM) {}
};

class BogusFrameLowering : public TargetFrameLowering {
public:
  BogusFrameLowering()
      : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, Align(4), 4) {}

  void emitPrologue(MachineFunction &MF,
                    MachineBasicBlock &MBB) const override {}
  void emitEpilogue(MachineFunction &MF,
                    MachineBasicBlock &MBB) const override {}
  bool hasFP(const MachineFunction &MF) const override { return false; }
};

static TargetRegisterClass *const BogusRegisterClasses[] = {nullptr};

class BogusRegisterInfo : public TargetRegisterInfo {
public:
  BogusRegisterInfo()
      : TargetRegisterInfo(nullptr, BogusRegisterClasses, BogusRegisterClasses,
                           nullptr, nullptr, nullptr, LaneBitmask(~0u), nullptr,
                           nullptr) {
    InitMCRegisterInfo(nullptr, 0, 0, 0, nullptr, 0, nullptr, 0, nullptr,
                       nullptr, nullptr, nullptr, nullptr, 0, nullptr);
  }

  const MCPhysReg *
  getCalleeSavedRegs(const MachineFunction *MF) const override {
    return nullptr;
  }
  ArrayRef<const uint32_t *> getRegMasks() const override { return std::nullopt; }
  ArrayRef<const char *> getRegMaskNames() const override { return std::nullopt; }
  BitVector getReservedRegs(const MachineFunction &MF) const override {
    return BitVector();
  }
  const RegClassWeight &
  getRegClassWeight(const TargetRegisterClass *RC) const override {
    static RegClassWeight Bogus{1, 16};
    return Bogus;
  }
  unsigned getRegUnitWeight(unsigned RegUnit) const override { return 1; }
  unsigned getNumRegPressureSets() const override { return 0; }
  const char *getRegPressureSetName(unsigned Idx) const override {
    return "bogus";
  }
  unsigned getRegPressureSetLimit(const MachineFunction &MF,
                                  unsigned Idx) const override {
    return 0;
  }
  const int *
  getRegClassPressureSets(const TargetRegisterClass *RC) const override {
    static const int Bogus[] = {0, -1};
    return &Bogus[0];
  }
  const int *getRegUnitPressureSets(unsigned RegUnit) const override {
    static const int Bogus[] = {0, -1};
    return &Bogus[0];
  }

  Register getFrameRegister(const MachineFunction &MF) const override {
    return 0;
  }
  bool eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override {
    return false;

  }
};

template <typename TIIClass> class TestSubTarget : public TargetSubtargetInfo {
public:
  TestSubTarget(TargetMachine &TM, StringRef CPU = "", StringRef TuneCPU = "",
                ArrayRef<SubtargetSubTypeKV> ProcDescs = {})
      : TargetSubtargetInfo(Triple(""), CPU, TuneCPU, "", {}, ProcDescs,
                            nullptr, nullptr, nullptr, nullptr, nullptr,
                            nullptr),
        FL(), TL(TM) {}
  ~TestSubTarget() override {}

  const TargetFrameLowering *getFrameLowering() const override { return &FL; }

  const TargetLowering *getTargetLowering() const override { return &TL; }

  const TargetInstrInfo *getInstrInfo() const override { return &TII; }

  const TargetRegisterInfo *getRegisterInfo() const override { return &TRI; }

private:
  BogusFrameLowering FL;
  BogusRegisterInfo TRI;
  BogusTargetLowering TL;
  TIIClass TII;
};
using BogusSubTarget = TestSubTarget<TargetInstrInfo>;

static TargetOptions getTargetOptionsForBogusMachine() {
  TargetOptions Opts;
  Opts.EmitCallSiteInfo = true;
  return Opts;
}

template <typename STClass> class TestTargetMachine : public LLVMTargetMachine {
public:
  TestTargetMachine()
      : LLVMTargetMachine(Target(), "", Triple(""), "", "",
                          getTargetOptionsForBogusMachine(), Reloc::Static,
                          CodeModel::Small, CodeGenOptLevel::Default),
        ST(*this) {}

  ~TestTargetMachine() override {}

  const TargetSubtargetInfo *getSubtargetImpl(const Function &) const override {
    return &ST;
  }

private:
  STClass ST;
};
using BogusTargetMachine = TestTargetMachine<BogusSubTarget>;

[[maybe_unused]] BogusTargetMachine *createTargetMachine() {
  static BogusTargetMachine BogusTM;
  return &BogusTM;
}

[[maybe_unused]] std::unique_ptr<MachineFunction>
createMachineFunction(LLVMContext &Ctx, Module &M,
                      LLVMTargetMachine *TM = nullptr) {
  auto Type = FunctionType::get(Type::getVoidTy(Ctx), false);
  auto F = Function::Create(Type, GlobalValue::ExternalLinkage, "Test", &M);

  if (!TM)
    TM = createTargetMachine();
  unsigned FunctionNum = 42;
  MachineModuleInfo MMI(TM);
  const TargetSubtargetInfo &STI = *TM->getSubtargetImpl(*F);

  return std::make_unique<MachineFunction>(*F, *TM, STI, FunctionNum, MMI);
}
