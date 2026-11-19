//===- AIEOLPWARSolver.h - WAR dependency solver for OLP -*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Resolve Write-After-Read (WAR) register dependencies in outer-loop
// pipelined (OLP) epilogue blocks by renaming registers used in the
// TopInsert (fixed epilogue drain) and their fed free store instructions.
//
// In an OLP epilogue, the inner loop's pipeline drain (TopInsert) defines
// accumulator registers (e.g., dm3 via vmac) that are then read by free
// store instructions (e.g., vst.srs.4x dm3). Meanwhile, free load
// instructions (e.g., vlda.ups.2x cml3) write to sub-registers of the
// same accumulator for the next outer-loop iteration. This creates a WAR
// dependency that prevents the scheduler from moving the loads earlier.
//
// The solver renames the last definition of a register in TopInsert and
// the corresponding uses in free stores to a different, available register,
// breaking the WAR dependency and allowing the scheduler maximum freedom.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEOLPWARSOLVER_H
#define LLVM_LIB_TARGET_AIE_AIEOLPWARSOLVER_H

#include "Utils/AIELoopUtils.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCRegister.h"
#include <utility>

namespace llvm {

struct AIEBaseInstrInfo;
class MachineInstr;
class MachineOperand;
class TargetRegisterClass;
class TargetRegisterInfo;

} // namespace llvm

namespace llvm::AIE {

class BlockState;
class InterBlockScheduling;

/// Resolves WAR register dependencies in OLP epilogue blocks by renaming
/// registers in TopInsert (fixed drain) and their fed free instructions.
///
/// For each OLP epilogue block, this solver:
///  1. Walks free store instructions top-down and identifies registers
///     whose stored value comes from TopInsert and that are completely
///     redefined by later free instructions (WAR pattern).
///  2. Checks that the last definition's output operand is not tied.
///  3. Finds an available register of the same class that is not live
///     in the block and not callee-saved.
///  4. Renames the last definition's output and the free store operands
///     to the new register, breaking the WAR dependency.
class OLPWARSolver {
  InterBlockScheduling &IBS;
  const AIEBaseInstrInfo &TII;
  const TargetRegisterInfo &TRI;

  /// A candidate for WAR renaming.
  struct WARCandidate {
    /// The super-register causing the WAR conflict (e.g., dm3).
    MCRegister ConflictReg;

    /// The register class of ConflictReg.
    const TargetRegisterClass *RC;

    /// The last definition of ConflictReg in TopInsert (output operand).
    MachineOperand *LastDefInFixed;

    /// Free operands that read ConflictReg or its sub-registers before
    /// the first redefinition (loads, stores, or other consumers).
    SmallVector<MachineOperand *, 4> FreeUses;

    /// Free load instructions that write to sub-registers of ConflictReg.
    SmallVector<MachineInstr *, 4> ConflictingLoads;

    WARCandidate(MCRegister ConflictReg, const TargetRegisterClass *RC,
                 MachineOperand *LastDefInFixed,
                 SmallVector<MachineOperand *, 4> FreeUses,
                 SmallVector<MachineInstr *, 4> ConflictingLoads)
        : ConflictReg(ConflictReg), RC(RC), LastDefInFixed(LastDefInFixed),
          FreeUses(std::move(FreeUses)),
          ConflictingLoads(std::move(ConflictingLoads)) {}
  };

  /// Result type for collectFreeUsesAndConflictingLoads().
  struct UsesAndLoads {
    SmallVector<MachineOperand *, 4> FreeUses;
    SmallVector<MachineInstr *, 4> ConflictingLoads;
  };

  /// Build a map from each physical register to its last definition
  /// operand across all TopInsert bundles.
  DenseMap<MCRegister, MachineOperand *>
  buildLastDefMap(const BlockState &BS) const;

  /// Find the TopInsert def that provides the value for \p UseReg.
  /// Checks for an exact match first, then walks super-registers.
  /// Returns {DefReg, LastDef} or {NoRegister, nullptr} if not found.
  std::pair<MCRegister, MachineOperand *> findTopInsertDef(
      MCRegister UseReg,
      const DenseMap<MCRegister, MachineOperand *> &LastDefByReg) const;

  /// Check whether any TopInsert instruction after \p LastDef reads
  /// \p DefReg. If so, renaming the last def would break the data flow.
  bool hasPostDefUseInFixed(MCRegister DefReg, const MachineOperand *LastDef,
                            const BlockState &BS) const;

  /// Walk free instructions and separate uses (before the first
  /// redefinition of \p DefReg) from conflicting loads (redefinitions).
  UsesAndLoads
  collectFreeUsesAndConflictingLoads(MCRegister DefReg,
                                     ArrayRef<MachineInstr *> FreeInstrs) const;

  /// Determine the register class for a def operand, falling back to
  /// getMinimalPhysRegClass if the instruction descriptor doesn't
  /// provide one.
  const TargetRegisterClass *getRegClassForDef(const MachineOperand *Def,
                                               MCRegister DefReg) const;

  /// Collect WAR candidates from the epilogue block.
  /// Walks free stores top-down and checks whether the stored value comes
  /// from TopInsert and is completely redefined by later free instructions.
  SmallVector<WARCandidate, 4> collectCandidates(BlockState &BS) const;

  /// Check whether \p Reg is completely redefined by free instructions
  /// that appear after \p StoreMI. Uses register units to verify that
  /// every bit of \p Reg is written by a later definition.
  bool isCompletelyRedefinedLater(MCRegister Reg, const MachineInstr *StoreMI,
                                  ArrayRef<MachineInstr *> FreeInstrs) const;

  /// Check if the last def operand can be renamed (not tied).
  bool canRename(const WARCandidate &C) const;

  /// Compute available registers using the OLP structure: exclude
  /// live-ins of SteadyTop and all registers used in SteadyTop/Inner/Bottom
  /// free instructions.
  DenseSet<MCRegister>
  computeAvailableRegs(const BlockState &BS, const TargetRegisterClass *RC,
                       const AIELoopUtils::OuterLoopStructure &OLP) const;

  /// Rename a candidate's last def and free uses to NewReg.
  /// Returns false if any register mapping fails (no mutations performed).
  bool renameCandidate(WARCandidate &C, MCRegister NewReg) const;

public:
  OLPWARSolver(InterBlockScheduling &IBS, const AIEBaseInstrInfo &TII,
               const TargetRegisterInfo &TRI);

  /// Try to resolve WAR dependencies in a single OLP epilogue block.
  /// Returns true if any renaming was performed.
  bool tryResolve(BlockState &BS) const;
};

} // end namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_AIEOLPWARSOLVER_H
