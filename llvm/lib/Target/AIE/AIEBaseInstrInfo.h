//===-- AIEBaseInstrInfo.h - Common AIE InstrInfo ---------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains common TargetInstrInfo code between AIE versions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEBASEINSTRRINFO_H
#define LLVM_LIB_TARGET_AIE_AIEBASEINSTRRINFO_H

#include "AIE.h"
#include "AIEMIRFormatter.h"
#include "AIETiedRegOperands.h"
#include "MCTargetDesc/AIEFormat.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "llvm/CodeGen/ResourceCycle.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include <optional>

namespace llvm {

struct AIEBaseInstrInfo : public TargetInstrInfo {
  using TargetInstrInfo::TargetInstrInfo;

  /// Return the opcode for a return instruction
  virtual unsigned getReturnOpcode() const {
    llvm_unreachable("Target didn't implement getReturnOpcode");
  }

  /// Return the opcode for a call instruction
  /// \param CallerF The function that makes the call
  /// \param IsIndirect Select function pointer call or direct call
  /// \param Select a tail call variant.
  virtual unsigned getCallOpcode(const MachineFunction &CallerF,
                                 bool IsIndirect, bool IsTailCall) const {
    llvm_unreachable("Target didn't implement getCallOpcode");
  }

  /// Return the kind of slot that this instruction can be executed in.
  /// This is used to direct the packetization of simple instructions.
  /// NOTE: If this is called on a Composite Instruction (i.e. an instruction
  /// defining a Packet format, owning possibly multiples slots), the returned
  /// slot will be the default one (unknown).
  virtual MCSlotKind getSlotKind(unsigned Opcode) const {
    llvm_unreachable("Target didn't implement getSlotKind");
  }
  virtual const MCSlotInfo *getSlotInfo(const MCSlotKind Kind) const {
    llvm_unreachable("Target didn't implement getSlotInfo");
  }
  /// Return the Packet formats for this target
  virtual const PacketFormats &getPacketFormats() const {
    llvm_unreachable("Target didn't implement getPacketFormats");
  }
  /// Return a nop of the given byte size, or the smallest if zero.
  virtual unsigned getNopOpcode(size_t Size = 0) const {
    llvm_unreachable("Target didn't implement getNopOpcode");
  }
  /// Return an opcode that reverses the branch condition of a given
  /// instruction
  /// \param Opc Opcode of the branch to reverse
  /// \pre Opc must be a conditional branch
  virtual unsigned getOppositeBranchOpcode(unsigned Opc) const {
    llvm_unreachable("Target didn't implement getOppositeBranchOpcode");
  }
  /// Return the opcode of an unconditional jump
  virtual unsigned getJumpOpcode() const {
    llvm_unreachable("Target didn't implement getJumpOpcode");
  }
  /// Return Multi-Slot Pseudo opcode based on Reg type and imm. size
  virtual unsigned getConstantMovOpcode(MachineRegisterInfo &MRI,
                                        unsigned int Reg, APInt &Val) const {
    llvm_unreachable("Target didn't implement getConstantMovOpcode");
  }
  /// Returns the opcode for CYCLE_SEPARATOR meta instruction.
  /// Used for debugging purposes
  virtual unsigned getCycleSeparatorOpcode() const {
    llvm_unreachable("Target didn't implement getCycleSeparatorOpcode");
  }
  /// Return the opcode to be used for pushing a vector element at the LSB
  /// position in a vector
  virtual unsigned getGenericAddVectorEltOpcode() const {
    llvm_unreachable("Target didn't implement getGenericAddVectorEltOpcode");
  }
  /// Return the opcode to be used for inserting a vector element at an
  /// arbitrary position in a vector
  virtual unsigned getGenericInsertVectorEltOpcode() const {
    llvm_unreachable("Target didn't implement getGenericInsertVectorEltOpcode");
  }
  /// Return the opcode to be used for extracting a vector element
  /// \param signExt True if the extracted element shall be sign extended
  virtual unsigned getGenericExtractVectorEltOpcode(bool SignExt) const {
    llvm_unreachable(
        "Target didn't implement getGenericExtractVectorEltOpcode");
    ;
  }
  /// Return the opcode to be used for padding undefined values in the high bits
  /// of a vector
  virtual unsigned getGenericPadVectorOpcode() const {
    llvm_unreachable("Target didn't implement getGenericPadVectorOpcode");
  }
  /// Return the opcode to be used for extracting a smaller vector by ignoring
  /// the high bits
  virtual unsigned getGenericUnpadVectorOpcode() const {
    llvm_unreachable("Target didn't implement getGenericUnpadVectorOpcode");
  }
  /// Check whether Opc represents a lock instruction
  virtual bool isLock(unsigned Opc) const { return false; }
  /// Check whether this is a delayed scheduling barrier induced from
  /// a preceding instruction with delay slots.
  virtual bool isDelayedSchedBarrier(const MachineInstr &) const {
    return false;
  }
  /// Check whether this is a scheduling barrier
  virtual bool isSchedBarrier(const MachineInstr &) const { return false; }

  /// Returns the number of delay slots that this instruction requires.
  /// This might be 0
  virtual unsigned
  getNumDelaySlots(const MachineInstr &MI,
                   MachineInstr::QueryType Query =
                       MachineInstr::QueryType::AnyInBundle) const;

  /// Returns the number of delay slots that should be reserved, i.e.
  /// not filled in by the scheduler.
  virtual unsigned getNumReservedDelaySlots(const MachineInstr &MI) const;

  /// Check whether Opc represents a JNZ instruction. This is mainly for
  /// detecting a downcounting loop branch.
  virtual bool isJNZ(unsigned Opc) const { return false; }

  /// Check whether Opc represents a JZ instruction.
  virtual bool isJZ(unsigned Opc) const { return false; }

  /// Check whether Opc represents a JL/JAL instruction.
  virtual bool isCall(unsigned Opc) const { return false; }

  // Detect instructions that induce control flow to unknown targets,
  // in particular after pseudo expansion, where they are no longer
  // terminators
  virtual bool jumpsToUnknown(unsigned Opcode) const { return false; }

  /// Check whether Opc represents an integer constant.
  /// signature should be Reg <- (Imm);
  virtual bool isIConst(unsigned Opc) const { return false; }

  /// Chek whether Opc represents an instruction that doesn't change the
  /// boolean result.
  /// Signature should be Reg <- (Reg, ...)
  virtual bool isBooleanNoOp(unsigned Opc) const {
    return Opc == TargetOpcode::COPY;
  }

  /// Check whether Opc represent an instruction that inverts a condition
  /// Signature should be Reg <- (Reg, ...)
  virtual bool isBooleanNot(unsigned Opc) const { return false; }

  /// Check whether MI is an increment with a constant amount.
  /// Signature should be Reg <- (Reg, ...)
  /// If it returns true, Step holds the amount.
  virtual bool isConstStep(const MachineInstr &MI, int64_t &Step) const {
    return false;
  }

  // Used for Load/Store combiners
  virtual unsigned getOffsetMemOpcode(unsigned BaseMemOpcode) const {
    llvm_unreachable("Target didn't implement getOffsetMemOpcode");
  }
  virtual std::optional<unsigned>
  getCombinedPostIncOpcode(MachineInstr &BaseMemI, MachineInstr &PtrAddI,
                           TypeSize Size) const {
    llvm_unreachable("Target didn't implement getCombinedPostIncOpcode");
  }

  // Opcodes related to hardware loop handling
  virtual bool isHardwareLoopDec(unsigned Opcode) const { return false; }
  virtual bool isHardwareLoopJNZ(unsigned Opcode) const { return false; }
  virtual bool isHardwareLoopStart(unsigned Opcode) const { return false; }
  virtual bool isHardwareLoopEnd(unsigned Opcode) const { return false; }

  /// Check whether \p MI defines the ZOL tripcount. If this returns true, \p MI
  /// should be suitable for calling adjustTripCount on it.
  /// If \p Pristine is set, we check that it wasn't updated before.
  virtual bool isZOLTripCountDef(const MachineInstr &MI,
                                 bool Pristine = false) const {
    return false;
  }

  /// Lower the tripcount defined by MI with Update, which is a small
  /// negative integer that should be added to the tripcount
  /// \pre isZOLTripCountDef(MI)
  virtual void adjustTripCount(MachineInstr &MI, int Update) const {
    llvm_unreachable("adjustTripCount should have been overridden");
  }

  /// Check whether this is a zero-overhead loop start block
  virtual bool isZeroOverheadLoopSetupInstr(const MachineInstr &) const {
    return false;
  }

  // Return the vector of Alignment Region Boundaries.
  virtual std::vector<MachineBasicBlock::iterator>
  getAlignmentBoundaries(MachineBasicBlock &MBB) const {
    llvm_unreachable("Target didn't implement getAlignmentBoundaries!");
  }
  virtual unsigned getPseudoJNZDOpcode() const {
    llvm_unreachable("Need to implement this hook for hardware loop support.");
  }

  /// Return the opcode of a scalar move
  virtual unsigned getPseudoMoveOpcode() const {
    llvm_unreachable("Target didn't implement getPseudoMoveOpcode!");
  }

  /// Information about tied operands which cannot be modeled using TableGen
  /// constraints.
  virtual SmallVector<TiedRegOperands, 4>
  getTiedRegInfo(unsigned Opcode) const {
    return {};
  }
  SmallVector<TiedRegOperands, 4> getTiedRegInfo(const MachineInstr &MI) const {
    return getTiedRegInfo(MI.getOpcode());
  }

  /// Finds the opcode that is equivalent to \p Opcode except some operands
  /// are expanded into multiple sub-registers operands to facilitate register
  /// allocation.
  /// E.g. for AIE2: PADDA_2D -> PADDA_2D_split
  virtual std::optional<unsigned>
  getOpcodeWithAtomicOperands(unsigned Opcode) const {
    return {};
  }

  /// Finds the opcode that is equivalent to \p Opcode except some sub-register
  /// operands are rewritten into a larger super-register to facilitate later
  /// phases of compilation like instruction encoding.
  /// This is the inverse operation for \ref getOpcodeWithAtomicOperands.
  /// E.g. for AIE2: PADDA_2D_split -> PADDA_2D
  virtual std::optional<unsigned>
  getOpcodeWithTupleOperands(unsigned Opcode) const {
    return {};
  }

  struct PseudoBranchExpandInfo {
    unsigned TargetInstrOpCode;
    unsigned BarrierOpCode;
  };
  virtual std::optional<PseudoBranchExpandInfo>
  getPseudoBranchExpandInfo(const MachineInstr &MI) const;

  // Shared code
  void insertNoop(MachineBasicBlock &MBB,
                  MachineBasicBlock::iterator MI) const override;
  bool
  reverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const override;

  bool analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
                     MachineBasicBlock *&FBB,
                     SmallVectorImpl<MachineOperand> &Cond,
                     bool AllowModify) const override;

  unsigned insertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                        MachineBasicBlock *FBB, ArrayRef<MachineOperand> Cond,
                        const DebugLoc &dl,
                        int *BytesAdded = nullptr) const override;

  unsigned removeBranch(MachineBasicBlock &MBB,
                        int *BytesRemoved = nullptr) const override;

  bool isSchedulingBoundary(const MachineInstr &MI,
                            const MachineBasicBlock *MBB,
                            const MachineFunction &MF) const override;

  std::optional<unsigned> getOperandLatency(const InstrItineraryData *ItinData,
                                            const MachineInstr &DefMI,
                                            unsigned DefIdx,
                                            const MachineInstr &UseMI,
                                            unsigned UseIdx) const override;

  // Check if the MII points to a BUNDLE which contains a call instruction
  bool isCallBundle(MachineBasicBlock::iterator MII) const;
  // Check if the MII points to a BUNDLE which contains an instruction
  // setting LE Reg
  bool isZOLSetupBundle(MachineBasicBlock::iterator MII) const;
  bool isLastZOLSetupBundleInMBB(MachineBasicBlock::iterator MII) const;

  /// Central place to compute RAW/WAR/WAW operand latencies.
  /// This uses itineraries when they exist. It returns std::nullopt for
  /// instructions that are not described.
  /// Note that the latency can be negative, e.g. for AIE2:
  ///   ST r0 --anti(-6)--> LD r0
  /// or
  ///   MOV r0, 1 --out(-5)--> LD r0
  std::optional<int> getSignedOperandLatency(const InstrItineraryData *ItinData,
                                             const MachineInstr &DefMI,
                                             unsigned DefIdx,
                                             const MachineInstr &UseMI,
                                             unsigned UseIdx,
                                             SDep::Kind Kind) const;

  /// Returns the number of cycles that are saved if there is a bypass (pipeline
  /// forwarding) between \p DefMI and \p UseMI for the given operands.
  /// This returns 0 if no bypass is taken.
  virtual unsigned getNumBypassedCycles(const InstrItineraryData *ItinData,
                                        const MachineInstr &DefMI,
                                        unsigned DefIdx,
                                        const MachineInstr &UseMI,
                                        unsigned UseIdx) const;

  /// Returns the latency to be observed to preserve the ordering of aliasing
  /// memory operations.
  /// E.g. in AIE2 VST.SRS to VLD has a memory latency of 3 cycles.
  std::optional<int> getMemoryLatency(unsigned SrcSchedClass,
                                      unsigned DstSchedClass) const;

  /// Returns the worst-case latency to be observed to preserve the
  /// ordering of aliasing memory operations. We don't know the destination
  /// of the edge
  int getConservativeMemoryLatency(unsigned SrcSchedClass) const;

  /// Returns cycle for the first memory operation of an instruction.
  /// This is usually the same as \p getLastMemoryCycle except for instructions
  /// that touch memory multiple times like AIE's read-modify-write part-word
  /// stores.
  /// E.g. in AIE2 VST.SRS has a first cycle of 7, ST.s8 a first cycle of 5.
  virtual std::optional<int> getFirstMemoryCycle(unsigned SchedClass) const;

  /// Returns cycle for the last memory operation of an instruction.
  /// This is usually the same as \p getFirstMemoryCycle except for instructions
  /// that touch memory multiple times like AIE's read-modify-write part-word
  /// stores.
  /// E.g. in AIE2 VST.SRS has a last cycle of 7, ST.s8 a last cycle of 11.
  virtual std::optional<int> getLastMemoryCycle(unsigned SchedClass) const;

  /// Return the minimum of FirstMemoryCycle over all sched classes
  virtual int getMinFirstMemoryCycle() const;
  /// Return the maximum of FirstMemoryCycle over all sched classes
  virtual int getMaxFirstMemoryCycle() const;
  /// Return the minimum of LastMemoryCycle over all sched classes
  virtual int getMinLastMemoryCycle() const;
  /// Return the maximum of LastMemoryCycle over all sched classes
  virtual int getMaxLastMemoryCycle() const;
  /// Return cycles for memory operations of an instruction.
  virtual SmallVector<int, 2> getMemoryCycles(unsigned SchedClass) const;

  /// Return the schedclass for the given instruction descriptor based on
  /// operand regclass.
  virtual unsigned
  getSchedClass(const MCInstrDesc &Desc,
                iterator_range<const MachineOperand *> Operands,
                const MachineRegisterInfo &MRI) const;

  const AIEBaseMCFormats *getFormatInterface() const { return FormatInterface; }

  /// Verifies whether Ty is legal as an input to G_AIE_PAD_VECTOR_UNDEF or an
  /// output of G_AIE_UNPAD_VECTOR
  virtual bool isLegalTypeToPad(const LLT &Ty,
                                StringRef *ErrInfo = nullptr) const;

  /// Verifies whether Ty is legal as an input to G_AIE_UNPAD_VECTOR or an
  /// output of G_AIE_PAD_VECTOR_UNDEF
  virtual bool isLegalTypeToUnpad(const LLT &Ty,
                                  StringRef *ErrInfo = nullptr) const;

  virtual bool verifyGenericInstruction(const MachineInstr &MI,
                                        StringRef &ErrInfo) const;
  virtual bool verifyMemOperand(const MachineInstr &MI,
                                StringRef &ErrInfo) const;
  bool verifyTiedRegisters(const MachineInstr &MI, StringRef &ErrInfo) const;
  static bool verifySameLaneTypes(const MachineInstr &MI, StringRef &ErrInfo);
  bool verifyImplicitOpsOrder(const MachineInstr &MI, StringRef &ErrInfo) const;
  bool verifyInstruction(const MachineInstr &MI,
                         StringRef &ErrInfo) const override;

  bool canHoistCheapInst(const MachineInstr &MI) const override;

  static bool regClassMatches(const TargetRegisterClass &TRC,
                              const TargetRegisterClass *RC, unsigned Reg);

  struct VConcatOpInfo {
    // First input operand index.
    unsigned FirstOperand;
    // Number of non-register operands.
    unsigned NumOfNonRegOperands;
  };

  /// Return operand information related to vector concat instrinsic.
  virtual std::optional<const VConcatOpInfo>
  getVConcatOpInfo(const MachineInstr &MI) const;

  struct VUpdateOpInfo {
    // Vector to update operand index.
    unsigned Src;
    // Subvector to insert.
    unsigned SrcSubVec;
    // Position to insert operand index.
    unsigned SubVectorIndex;
  };

  /// Return operand information related to vector update instrinsic.
  virtual std::optional<const VUpdateOpInfo>
  getVUpdateOpInfo(const MachineInstr &MI) const {
    llvm_unreachable("Target didn't implement getVUpdateOpInfo!");
  }

  struct VExtractOpInfo {
    // Vector to update operand index.
    unsigned Src;
    // Position to extract.
    unsigned SubVectorIndex;
  };

  /// Return operand information related to vector extract instrinsic.
  virtual std::optional<const VExtractOpInfo>
  getVExtractOpInfo(const MachineInstr &MI) const {
    llvm_unreachable("Target didn't implement getVExtractOpInfo!");
  }

  /// Return the maximun size for memory operations on this target.
  virtual unsigned getMaxLoadStoreSize() const {
    llvm_unreachable("Target didn't implement getMaxLoadStoreSize!");
  }

  /// Return true if this instruction can be combined with a memory operation.
  virtual bool canCombineWithLoadStore(const MachineInstr &MI) const {
    llvm_unreachable("Target didn't implement canCombineWithLoadStore!");
  }

  /// Return true if the type can be splitted to fit target's restrictions.
  /// For example, by splitting those types in advance, it is possible to
  /// reach more combiners during selection.
  virtual bool isProfitableToSplitType(const LLT Ty) const {
    llvm_unreachable("Target didn't implement isProfitableToSplitType!");
  }

  /// Abstract operations to help the decoding of complex operations.
  struct AbstractOp {
    enum class OperationType : unsigned {
      VECTOR_ADD,
      VECTOR_BROADCAST,
      VECTOR_SELECT,
      VECTOR_XWAY_LOAD
    };
    OperationType Type;
    SmallVector<Register, 2> VectorSrcRegs;
    SmallVector<Register, 2> ScalarSrcRegs;
  };

  /// Retrieve an abstract representation, of an instruction.
  virtual std::optional<const AbstractOp>
  parseAbstractOp(const MachineInstr &MI) const {
    return std::nullopt;
  }

protected:
  /// Expand a spill pseudo-instruction into actual target instructions. This
  /// will essentially split the register being handled into its sub-registers,
  /// until there is an actual instruction that can handle them.
  /// In case sub-registers don't have nice offset, those can be aligned using
  /// \ref SubRegOffsetAlign.
  void expandSpillPseudo(MachineInstr &MI, const TargetRegisterInfo &TRI,
                         Align SubRegOffsetAlign = Align(1)) const;

  struct AIEPseudoExpandInfo {
    /// OpCode to expand a PseudoInstruction to. This can be another Pseudo.
    unsigned ExpandedOpCode;
    /// Index of the sub-register to use when splitting the register used
    /// in the initial instruction.
    /// This can be NoSubRegister, but then \ref MemSize must be set.
    unsigned SubRegIndex;
    /// Explicit size (in bytes) that this expanded spill instruction will use
    /// in memory. This is useful when the expansion can't be characterized with
    /// sub-registers.
    int MemSize = 0;
  };

  /// Return information on how to expand a spill (load/store) pseudo
  /// instruction. This returns an empty vector if the instruction does not
  /// need expanding. Otherwise, the size of the vector will match the number
  /// of instructions which \ref MI needs to be expanded to.
  virtual SmallVector<AIEPseudoExpandInfo, 4>
  getSpillPseudoExpandInfo(const MachineInstr &MI) const {
    return {};
  };

  // Copy SrcReg to DstReg through their sub-registers.
  void copyThroughSubRegs(MachineBasicBlock &MBB,
                          MachineBasicBlock::iterator MBBI, const DebugLoc &DL,
                          MCRegister DstReg, MCRegister SrcReg,
                          bool KillSrc) const;

#if 0
  // TODO. I guess this should wait for Davy's PR to land
  // Get me a nop to fill Slot
  // TODO: Strong type for Slot (MCSlotKind)
  virtual unsigned getNopOpcode(unsigned Slot) const = 0;
#endif

  /// If the specific machine instruction is an instruction that moves/copies
  /// value from one register to another register return destination and source
  /// registers as machine operands.
  std::optional<DestSourcePair>
  isCopyInstrImpl(const MachineInstr &MI) const override;

  /// Analyze loop L, which must be a single-basic-block loop, and if the
  /// conditions can be understood enough produce a PipelinerLoopInfo object.
  std::unique_ptr<TargetInstrInfo::PipelinerLoopInfo>
  analyzeLoopForPipelining(MachineBasicBlock *LoopBB) const override;

  // Provide a ResourceCycle interface on top of the hazard recognizer
  ResourceCycle *
  CreateTargetScheduleState(const TargetSubtargetInfo &) const override;

  const AIEBaseMCFormats *FormatInterface = nullptr;

  // MIR formatter overrides for PseudoSourceValues
  const MIRFormatter *getMIRFormatter() const override;
  mutable std::unique_ptr<AIEMIRFormatter> Formatter;
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEBASEINSTRRINFO_H
