//===- AIEOLPWARSolver.h - WAR dependency solver for OLP --------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Break write-after-read register dependences in the dedicated exit of an
// outer-loop pipelined (OLP) inner loop.
//
// A loop-carried recurrence of the inner loop (e.g. dm3 = VMAC dm3, ...) is
// live into the exit, where free instructions read it (e.g. stores) and later
// redefine the register (e.g. loads for the next outer iteration). Once the
// loop is pipelined, the last iteration of the recurrence is a separate clone
// in the epilogue. Giving that clone a fresh register, together with its
// readers in the exit, removes the WAR dependences from the readers to the
// redefinitions.
//
// The solver runs when the pipeline is materialized, before any other consumer
// sees the epilogue, and works on the inter-block DDG from the loop into its
// exit.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEOLPWARSOLVER_H
#define LLVM_LIB_TARGET_AIE_AIEOLPWARSOLVER_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCRegister.h"
#include <optional>

namespace llvm {

class LiveRegUnits;
class MachineBasicBlock;
class MachineOperand;
class SUnit;
class TargetRegisterClass;
class TargetRegisterInfo;

} // namespace llvm

namespace llvm::AIE {

class BlockState;
class InterBlockEdges;
class InterBlockScheduling;

class OLPWARSolver {
  InterBlockScheduling &IBS;
  const TargetRegisterInfo &TRI;

  /// The last-iteration def of a recurrence in the epilogue and the operands
  /// in the exit that read its value.
  struct Candidate {
    MachineOperand *Def;
    const TargetRegisterClass *RC;
    SmallVector<MachineOperand *, 4> Uses;
  };

  /// Build a candidate from the def in epilogue clone \p DefSU that feeds
  /// \p UseReg, or nullopt if it cannot be renamed safely or profitably.
  std::optional<Candidate> analyzeDef(const InterBlockEdges &DDG,
                                      const SUnit &DefSU,
                                      MCRegister UseReg) const;

  /// Append the operands of \p Reader that read \p DefReg to \p Uses. Fails if
  /// the reader takes any unit of \p DefReg from a def other than \p DefSU, or
  /// if an operand cannot be renamed.
  bool collectReaderUses(const SUnit &DefSU, const SUnit &Reader,
                         MCRegister DefReg,
                         SmallVectorImpl<MachineOperand *> &Uses) const;

  /// Registers that must not be picked as a new register: live into or out of
  /// \p Exit, used in \p Exit or its epilogue and prologue bundles, or written
  /// by \p Loop, whose writes may still be in flight in \p Exit.
  void collectUnavailableRegs(const BlockState &Loop, const BlockState &Exit,
                              LiveRegUnits &Unavailable) const;

  /// Rename the def and uses of \p C to \p NewReg.
  void rename(Candidate &C, MCRegister NewReg) const;

public:
  OLPWARSolver(InterBlockScheduling &IBS, const TargetRegisterInfo &TRI);

  /// Try to break WAR dependences in the exit of the just materialized
  /// pipelined loop \p Loop. Returns the exit block if any register was
  /// renamed, in which case the edge from \p Loop to it must be rebuilt.
  MachineBasicBlock *run(const BlockState &Loop) const;
};

} // end namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_AIEOLPWARSOLVER_H
