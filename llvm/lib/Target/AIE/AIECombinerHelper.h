//=== lib/CodeGen/GlobalISel/AIECombinerHelper.h --------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIECOMBINERHELPER_H
#define LLVM_LIB_TARGET_AIE_AIECOMBINERHELPER_H

#include "llvm/CodeGen/GlobalISel/CombinerHelper.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/MachineInstr.h"

namespace llvm {

struct AIEBaseInstrInfo;

struct AIELoadStoreCombineMatchData {
  /// Matched PtrAdd instruction
  MachineInstr *Instr;
  /// Opcode of the combined instruction
  unsigned CombinedInstrOpcode;
  /// If null insert instruction at the end of the BB, otherwise insert just
  /// before this Instruction
  MachineInstr *CombinedInsertPoint;
  /// Additional instructions to be moved just before Instr
  std::vector<MachineInstr *> ExtraInstrsToMove;
  /// Should Instr (the PtrAdd) be removed after the combine was applied
  bool RemoveInstr;
};

/// Look for any PtrAdd instruction that use the same base as \a MI that can be
/// combined with it and stores it in \a MatchData
/// \return true if an instruction is found
bool matchLdStInc(MachineInstr &MI, MachineRegisterInfo &MRI,
                  AIELoadStoreCombineMatchData &MatchData,
                  CombinerHelper &Helper, const TargetInstrInfo &TII);
/// Combines \a MI and the instruction stored in \a MatchData
void applyLdStInc(MachineInstr &MI, MachineRegisterInfo &MRI,
                  MachineIRBuilder &B, AIELoadStoreCombineMatchData &MatchData,
                  GISelChangeObserver &Observer);
/// Look for  with G_IMPLICIT_DEF source operands
/// \return true if such an instruction is found
bool matchAddVecEltUndef(MachineInstr &MI, MachineRegisterInfo &MRI);
/// Combine G_AIE_ADD_VECTOR_ELT_HI with COPY
void applyAddVecEltUndef(MachineInstr &MI, MachineRegisterInfo &MRI,
                         MachineIRBuilder &B);
/// combine G_GLOBAL_VALUE with G_CONSTANT and store in \a MatchData
/// \return true if it is possible to combine
void applyGlobalValOffset(MachineInstr &MI, MachineRegisterInfo &MRI,
                          MachineIRBuilder &B, GISelChangeObserver &Observer,
                          uint64_t &MatchInfo);
bool matchGlobalValOffset(MachineInstr &MI, MachineRegisterInfo &MRI,
                          uint64_t &MatchInfo);
/// \return true if \a MemI can be moved just before \a Dest in order to allow
/// post-increment combining
bool canDelayMemOp(MachineInstr &MemI, MachineInstr &Dest,
                   MachineRegisterInfo &MRI);
/// \return true if \a Dest can be moved just after \a MemI in order to allow
/// combining
bool canAdvanceOp(MachineInstr &MemI, MachineInstr &Dest,
                  const MachineRegisterInfo &MRI);
/// Find the def instruction for \p Reg, folding away any trivial copies and
/// bitcasts. May return nullptr if \p Reg is not a generic virtual register.
MachineInstr *getDefIgnoringCopiesAndBitcasts(Register Reg,
                                              const MachineRegisterInfo &MRI);

class InstrNode {
  MachineInstr *BaseNode;
  /// A user of \p BaseNode for which the latter can be re-materialized.
  mutable MachineInstr *RematerializeForUser;

public:
  InstrNode() {
    BaseNode = nullptr;
    RematerializeForUser = nullptr;
  }
  InstrNode(MachineInstr *InstrNode, MachineInstr *DirNode = nullptr) {
    BaseNode = InstrNode;
    RematerializeForUser = DirNode;
    assert(!RematerializeForUser || isRematerializable());
  }

  // Define comparison operators
  bool operator<(const InstrNode &Node) const {
    if (RematerializeForUser && Node.RematerializeForUser)
      return (BaseNode < Node.BaseNode) ||
             ((BaseNode == Node.BaseNode) &&
              (RematerializeForUser < Node.RematerializeForUser));
    else
      return (BaseNode < Node.BaseNode);
  }
  bool operator==(const InstrNode &Node) const {
    if (RematerializeForUser && Node.RematerializeForUser)
      return (BaseNode == Node.BaseNode) &&
             (RematerializeForUser == Node.RematerializeForUser);
    else
      return (BaseNode == Node.BaseNode);
  }

  MachineInstr *getBaseNode() const { return BaseNode; }
  MachineInstr *getRematerializeForUser() const { return RematerializeForUser; }
  bool isRematerializable() {
    auto opCode = BaseNode->getOpcode();
    if (opCode == TargetOpcode::G_CONSTANT ||
        opCode == TargetOpcode::G_IMPLICIT_DEF)
      return true;
    return false;
  }

  void dbgPrintNode() const;
  void rematerializeStartNode(MachineRegisterInfo &MRI, MachineIRBuilder &B,
                              GISelChangeObserver &Observer);
};

bool matchS20NarrowingOpt(MachineInstr &MI, MachineRegisterInfo &MRI,
                          std::set<InstrNode> &ValidStartNodes);
void applyS20NarrowingOpt(MachineInstr &MI, MachineRegisterInfo &MRI,
                          MachineIRBuilder &B, GISelChangeObserver &Observer,
                          CombinerHelper &Helper,
                          std::set<InstrNode> &ValidStartNodes);

bool matchExtractVecEltAndExt(MachineInstr &MI, MachineRegisterInfo &MRI,
                              std::pair<MachineInstr *, bool> &MatchInfo);
void applyExtractVecEltAndExt(MachineInstr &MI, MachineRegisterInfo &MRI,
                              MachineIRBuilder &B,
                              std::pair<MachineInstr *, bool> &MatchInfo);

bool matchSplatVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                      std::pair<Register, Register> &MatchInfo);
bool applySplatVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                      MachineIRBuilder &B,
                      std::pair<Register, Register> &MatchInfo);

bool matchUnpadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                      const AIEBaseInstrInfo &TII);
void applyUnpadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                      MachineIRBuilder &B);

bool matchPadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                    const AIEBaseInstrInfo &TII, Register &MatchedInputVector);
void applyPadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                    MachineIRBuilder &B, Register MatchedInputVector);

bool matchExtractConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                        const AIEBaseInstrInfo &TII, Register &MatchInfo);
void applyExtractConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                        MachineIRBuilder &B, Register &MatchInfo);

bool matchUnmergeConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                        const AIEBaseInstrInfo &TII,
                        std::pair<MachineInstr *, unsigned> &MatchInfo);
void applyUnmergeConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                        MachineIRBuilder &B,
                        std::pair<MachineInstr *, unsigned> &MatchInfo);

bool matchUpdToConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                      const AIEBaseInstrInfo &TII,
                      std::map<unsigned, Register> &IndexRegMap);
void applyUpdToConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                      MachineIRBuilder &B,
                      std::map<unsigned, Register> &IndexRegMap);

bool matchLoadStoreSplit(GLoadStore &MI, MachineRegisterInfo &MRI,
                         const AIEBaseInstrInfo &TII, unsigned &MaxMemSize);
void applyLoadStoreSplit(GLoadStore &MI, MachineRegisterInfo &MRI,
                         MachineIRBuilder &B, const unsigned MaxMemSize);

bool matchOffsetLoadStorePtrAdd(MachineInstr &MI, MachineRegisterInfo &MRI,
                                const AIEBaseInstrInfo &TII,
                                std::pair<Register, int64_t> &RegOffset);

void applyOffsetLoadStorePtrAdd(MachineInstr &MI, MachineRegisterInfo &MRI,
                                MachineIRBuilder &B,
                                const std::pair<Register, int64_t> &RegOffset);

bool matchOffsetLoadStoreSharePtrAdd(MachineInstr &MI, MachineRegisterInfo &MRI,
                                     CombinerHelper &Helper,
                                     const AIEBaseInstrInfo &TII,
                                     Register &PtrAddReg);

void applyOffsetLoadStoreSharePtrAdd(MachineInstr &MI, MachineRegisterInfo &MRI,
                                     MachineIRBuilder &B, Register &PtrAddReg);

} // namespace llvm

#endif
