//===--- AIEGlobalCombinerPtrMods.h - Global Pointer Modifier combiner ----===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Define Pre-Increment (Offset) and Post-Increment Combiners for the global
// combiner search.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEGLOBALCOMBINERPTRMODS_H
#define LLVM_LIB_TARGET_AIE_AIEGLOBALCOMBINERPTRMODS_H
#include "AIEGlobalCombiner.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
namespace llvm::AIE {

struct UsageCount {
  /// How many Ptr Modifier Instructions were encountered (i.e. read and write
  /// Instruction count)
  int PtrModCount = 0;
  /// How many non-Pointer-Modifier Instructions were encountered (i.e. read
  /// only Instruction count)
  int NonPtrModCount = 0;
};

class PtrModGain : public CombinerGain {
public:
  PtrModGain() : CombinerGain() {}
  PtrModGain(std::initializer_list<int> InitialGain)
      : CombinerGain(InitialGain) {}
  PtrModGain(const PtrModGain &Other) = default;

  ~PtrModGain() = default;

  /// Set \p Value to PtrMod Position in Gain
  void setPtrMod(const int Value);
  /// Set Valid Imm Value based on \p ValidImm .
  void setValidImm(const bool ValidImm);
  /// Set NoCopy Value based on \p NoCopy .
  void setNoCopy(const bool NoCopy);
};

class PointerModifierCombiner : public GenericCombiner {
protected:
  PtrModGain Gain;

  void setupCombineInstrs(std::vector<MachineInstr *> CombineInstrs,
                          const AIE::DataDependenceHelper *DAG);

  bool hasOverlapPenalty(const GenericCombiner *Combiner) const;

  CombinerGain
  getOverlapPenalty(const GenericCombiner *Combiner) const override;

  CombinerGain getImmediateReuseGain(
      const std::vector<APInt> &UsedImmediates) const override;

  bool canMove(const SUnit *Candidate, const bool MoveDown) const override;

  virtual std::optional<unsigned> getOpCode(MachineInstr *PtrInc,
                                            MachineInstr *MemI) const = 0;

public:
  using GenericCombiner::GenericCombiner;
  const MachineRegisterInfo *MRI = nullptr;
  const AIEBaseInstrInfo *TII = nullptr;
  const AIE::DataDependenceHelper *DAG = nullptr;
  const MachineLoopInfo *MLI = nullptr;
  bool RemovePtrMod = false;
  bool ReplacePtrModInstr = false;

  PointerModifierCombiner(bool RemoveInstr, bool ReplaceInputPtr,
                          const MachineRegisterInfo *MRI,
                          const AIEBaseInstrInfo *TII,
                          const MachineLoopInfo *MLI, StringRef Name)
      : GenericCombiner(Name), Gain({1, 1, 1}), MRI(MRI), TII(TII), MLI(MLI),
        RemovePtrMod(RemoveInstr), ReplacePtrModInstr(ReplaceInputPtr) {}

  ~PointerModifierCombiner() = default;

  const MachineInstr *getPtrInc() const;
  MachineInstr *getPtrInc();

  MachineInstr *getMemI() { return CombinerData.CombineInstrs[1]; }
  const MachineInstr *getMemI() const { return CombinerData.CombineInstrs[1]; }

  /// \return the Gain that applying the combiner would incurr
  const PtrModGain &getGain() const override;

  void setInsertionPoint() override;

  bool setupCombiner(std::vector<MachineInstr *> CombineInstrs,
                     const AIE::DataDependenceHelper *DAG) override;

  virtual std::vector<MachineInstr *> getPtrInstrs(MachineInstr *MI) const = 0;

  /// \return Input and Output Pointer Registers
  const std::vector<Register> getClusterRegs() const override;

  virtual bool isCombineCandidate(MachineInstr &CombineRoot,
                                  MachineInstr &Candidate) const = 0;

  std::vector<MachineInstr *>
  getCombineCandidates(MachineInstr *MemI,
                       const AIE::DataDependenceHelper &DAG) const override;

  /// \return Usage Count of \p Addr non debug users after the Combiners'
  /// Insertion Point
  UsageCount getUsageCount(Register Addr,
                           const MachineDominatorTree &MDT) const;

  /// \return whether \p MI is a Memory Instruction
  bool isCombineRootCandidate(const MachineInstr *MI) const override;

  /// \return whether Opcode can be set
  bool tryToSetCombinedOpCode() override;

  virtual bool isPostInc() const = 0;
};

class OffsetCombiner : public PointerModifierCombiner {
  std::optional<APInt> ImmOffset;

protected:
  std::optional<unsigned> getOpCode(MachineInstr *PtrInc,
                                    MachineInstr *MemI) const override;

public:
  using PointerModifierCombiner::PointerModifierCombiner;
  OffsetCombiner(const MachineRegisterInfo *MRI, const AIEBaseInstrInfo *TII,
                 const MachineLoopInfo *MLI)
      : PointerModifierCombiner(false, false, MRI, TII, MLI, "Offset") {}

  bool isCombineCandidate(MachineInstr &CombineRoot,
                          MachineInstr &Candidate) const override;

  std::vector<MachineInstr *> getPtrInstrs(MachineInstr *MI) const override;

  std::unique_ptr<GenericCombiner> clone() const override;

  void adjustGain(const MachineDominatorTree &MDT) override;

  std::optional<std::pair<std::vector<SUnit *>, std::vector<SUnit *>>>
  getInstructionsToMove(const AIE::DataDependenceHelper &DAG) override;

  bool isReorderCandidate(const GenericCombiner *Candidate) const override;

  bool canReorder() const override;

  bool isPostInc() const override { return false; }
};

class PostIncCombiner : public PointerModifierCombiner {

  bool isPostIncCandidate(const MachineInstr *PtrMod,
                          const MachineRegisterInfo &MRI) const;

  /// \return whether keeping this post-increment would leave the chain root
  /// live across the chain because the loop advances it separately, forcing a
  /// copy of it on every iteration.
  bool chainingForcesExtraCursor() const;

  bool UserIntrinsic = false;

protected:
  std::optional<unsigned> getOpCode(MachineInstr *PtrInc,
                                    MachineInstr *MemI) const override;

public:
  using PointerModifierCombiner::PointerModifierCombiner;

  PostIncCombiner(const MachineRegisterInfo *MRI, const AIEBaseInstrInfo *TII,
                  const MachineLoopInfo *MLI)
      : PointerModifierCombiner(true, true, MRI, TII, MLI, /*Name=*/"PostInc") {
  }

  // Constructor for derived Classes
  PostIncCombiner(bool ReplaceInstr, const MachineRegisterInfo *MRI,
                  const AIEBaseInstrInfo *TII, const MachineLoopInfo *MLI,
                  StringRef Name)
      : PointerModifierCombiner(ReplaceInstr, ReplaceInstr, MRI, TII, MLI,
                                Name) {}

  bool isCombineCandidate(MachineInstr &CombineRoot,
                          MachineInstr &Candidate) const override;

  bool setupCombiner(std::vector<MachineInstr *> CombineInstrs,
                     const AIE::DataDependenceHelper *DAG) override;

  std::unique_ptr<GenericCombiner> clone() const override;

  void adjustGain(const MachineDominatorTree &MDT) override;

  std::vector<MachineInstr *> getPtrInstrs(MachineInstr *MI) const override;

  bool isReorderCandidate(const GenericCombiner *Candidate) const override {
    return false;
  }

  bool canReorder() const override { return false; }

  bool isPostInc() const override { return true; }
};

} // namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_AIEGLOBALCOMBINERPTRMODS_H
