//===- AIESWPSolver.h - Software Pipeliner Solver infrastructure-----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIESWPSOLVER_H
#define LLVM_LIB_TARGET_AIE_AIESWPSOLVER_H

#include "llvm/ADT/BitVector.h"
#include "llvm/Config/config.h"
#if LLVM_WITH_Z3
#include "z3++.h"
#endif

#include <cassert>
#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <vector>

namespace llvm::AIE::Solver {

// These classes describe the feature of the ISA that model the constraints
// Each instruction has a slot. Dependences between two instructions carry a
// latency.
// Most focus until now was given to the binary model, where we associate a
// set of binary variables with each instruction, each representing whether that
// instruction is scheduled in a particular cycle.
// The variables are indexed by instruction, stage, modulo_cycle (N, S, C).
// Slot constraints tie the instruction's slot to a modulo cycle, and each slot
// defines constraints to only have one instruction occupy that slot in any
// modulo cycle.
// The cycle in which an instruction is scheduled is expressed as a weighted
// sum over these boolean variables, and the latency constraints use the
// difference of pairs of these cycle expressions.
// Finally, each instruction should have exactly one of its variables true.
class Slot {
public:
  const int SlotNumber;
  Slot(int N) : SlotNumber(N) {}
  std::set<int> Instructions;
};

class Latency {
public:
  int Src;
  int Dst;
  int Lat;
  int Dist;
  Latency(int S, int D, int L, int Dist) : Src(S), Dst(D), Lat(L), Dist(Dist) {}
};

class Instruction {
public:
  const int Id;
  const Slot *const TheSlot;
  const uint64_t MemoryBanks = 0;
  const bool HasSideEffect = true;
  int Depth = 0;
  int Height = 0;
  Instruction(int Id, Slot *S, uint64_t MemoryBanks, bool HasSideEffect)
      : Id(Id), TheSlot(S), MemoryBanks(MemoryBanks),
        HasSideEffect(HasSideEffect) {}
};

// Per-cycle FU fingerprint of an instruction. BitVector (not uint64_t)
// so the model scales past 64 FUs (AIE2PS has 106). See fuConflict().
struct ResourceUseEntry {
  int Instr;
  int CycleOffset; // relative to the instruction's issue cycle
  BitVector Required;
  BitVector Reserved;
};

class ProblemSize {
public:
  ProblemSize();
  enum SizeComponent {
    NVariables,
    NInstrConstraints,
    NSlotConstraints,
    NConflicts,
    NLatencyTerms,
    NLatencyConstraints,
    NLCDLatencyConstraints,

    NumSizeComponents
  };
  int Counts[NumSizeComponents];
  const int &operator[](SizeComponent I) const { return Counts[I]; }
  int &operator[](SizeComponent I) { return Counts[I]; }
  void oneMore(SizeComponent C) { Counts[C]++; }
  void dump();
};

class SWPSolver;

class SolverData {
  // Maps the opaque slot number to the slot definition.
  std::map<int, Slot> Slots;
  // Holds all instructions.
  std::vector<Instruction> Instructions;
  // Holds all latencies.
  std::vector<Latency> Latencies;
  // One entry per (Instr, CycleOffset) with Required/Reserved BitVectors
  // OR-aggregated across stages covering CycleOffset.
  std::vector<ResourceUseEntry> ResourceUses;
  // Add a slot to the problem
  Slot &addSlot(int N);

public:
  // Add an instruction to the problem. It returns a unique Id
  int addInstruction(int Slot, uint64_t MemoryBanks, bool HasSideEffect);
  // Add a latency between two instructions to the problem.
  // Distance represents the iteration distance, i.e. the number of
  // cfg backedges it spans.
  void addLatency(int Src, int Dst, int Latency, int Distance = 0);
  /// Mark FU bit \p FUIndex as Required (\p IsRequired) or Reserved on
  /// the (\p InstrId, \p CycleOffset) entry, creating it if absent.
  void addResourceUse(int InstrId, int CycleOffset, unsigned FUIndex,
                      bool IsRequired);

  // Post-process when all data has been added.
  // II supplies the initiation interval.
  void finalize(int II);

  // Set the depth and the height of an instruction
  void setDepth(int I, int D);
  void setHeight(int I, int H);
  // Instruction queries
  int getDepth(int I) const;
  int getHeight(int I) const;
  bool hasSideEffect(int I) const;
  // Check whether it is feasible that instruction \p I can run in
  // cycle \p LinearCycl, given that the length of the schedule is Length
  bool isFeasible(int I, int LinearCycle, int Length) const;

  const std::map<int, Slot> &getSlots() const { return Slots; }
  const std::vector<Latency> &getLatencies() const { return Latencies; }
  const std::vector<Instruction> &getInstructions() const {
    return Instructions;
  }
  const std::vector<ResourceUseEntry> &getResourceUses() const {
    return ResourceUses;
  }
};

class SWPSolver {
  int II = 0;

protected:
  int NumStages;

public:
  virtual ~SWPSolver();

  // Return the specified II
  int getII() const {
    assert(II > 0);
    return II;
  }

  // Generate the latency constraints
  void latencies(const SolverData &Data);
  // Generate the slot constraints
  void slots(const SolverData &Data);

  // Generate the slot constraint for the given slot
  virtual void genSlotConstraint(int SlotNo, const Slot &Slot) = 0;
  // Generate a constraint that represents a dependence latency
  virtual void genLatencyConstraint(const Latency &L) = 0;
  // Forbid (\p InstrA, \p InstrB) from colliding at the same modular cycle:
  // issueA + \p OffsetA = issueB + \p OffsetB (mod II). Default offsets 0
  // recover the legacy "issueA != issueB (mod II)" memory-bank constraint.
  virtual void genConflict(int InstrA, int InstrB, int OffsetA = 0,
                           int OffsetB = 0) = 0;

  // Return the vector of instruction cycles
  // \pre genModel() has returned true
  virtual std::vector<int> getSUCycles() { return {}; };

  // Add an instruction to the problem. It returns a unique Id
  int addInsn(int Slot, uint64_t MemoryBanks, bool HasSideEffect);
  // Add a latency between two instructions to the problem.
  // Distance represents the iteration distance, i.e. the number of
  // cfg backedges it spans.
  void addLatency(int Src, int Dst, int Latency, int Distance = 0);
  // Set the desired schedule size in terms of II and stage count
  void setScheduleSize(int I, int NS = 2);
  // Generate the model.
  virtual void genModel(const SolverData &Data, bool SEFStage) = 0;
  // Call the solver on the model. Return whether it was satisfiable.
  virtual bool solveModel() = 0;
  // Generate further instruction conflict constraints
  void conflicts(const SolverData &Data);
  // Emit FU exclusion constraints from \p Data.ResourceUses: every pair of
  // instructions sharing an FU bit is forbidden from co-occupying its modular
  // cycle.
  void resourceConflicts(const SolverData &Data);
};

// Return the set of solvers to try
std::vector<std::unique_ptr<SWPSolver>> getSolvers();

/// Return true if at least one SWP solver backend is compiled into this
/// build. When false, getSolvers() returns an empty vector and any code
/// path that depends on the solver must fall back gracefully.
bool hasSolver();

#if LLVM_WITH_Z3
class Z3Solver : public SWPSolver {
protected:
  z3::context Context;
  z3::solver Solver;
  // Frequently used subexpression
  z3::expr Zero;
  // Expressions for the cycle of each instruction.
  std::vector<z3::expr> CycleExprs;
  // Problem size parameters
  ProblemSize Sizes;
  virtual ~Z3Solver() = default;

  // Generate the variables for each instruction. As a side effect, it
  // computes the stage count as the lowerbound for a fesible solution
  // \param SEFStage avoids instructions with side effects in stage 0
  virtual void vars(const SolverData &Data, bool SEFStage) = 0;
  // Generate the constraint that all instructions must be scheduled
  virtual void scheduled(const SolverData &Data) = 0;

  // Return a z3 expression that represents the cycle of an instruction
  // in the linear schedule.
  virtual z3::expr genCycle(int I) = 0;

  // Generate expressions for the cycle of each instruction.
  void cycles(const SolverData &Data);

  // Convenience functions to protect against empty sets.
  static const z3::expr_vector &nonEmpty(const z3::expr_vector &Elements) {
    assert(!Elements.empty());
    return Elements;
  }

  void atMost(const z3::expr_vector &Elements, int Limit);

  // Return an estimate of the solver time in milliseconds
  virtual int estimateSolverTime(ProblemSize Counts) const {
    const int SecondsPerMinute = 60;
    const int MillisecondsPerSecond = 1000;
    return 20 * SecondsPerMinute * MillisecondsPerSecond;
  }

public:
  Z3Solver();
  void genModel(const SolverData &Data, bool SEFStage) override;
  bool solveModel() override;
  std::vector<int> getSUCycles() override;
};

// In the binary formulation, we have a lot of binary variables,
// but relatively few, though elaborate constraints.
class Z3BinarySolver : public Z3Solver {
  int EffectiveLength = 0;
  std::vector<std::optional<z3::expr>> VarDecls;

  std::string varName(int N, int S, int C);
  std::optional<z3::expr> varDecl(int N, int S, int C);

  // generate an expression that represents I running in modulo cycle C
  z3::expr genModuloCycle(int I, int C);

  // If it exists, add a schedule variable declaration to Elements
  void addVar(int N, int S, int C, z3::expr_vector &Elements);

  void genSlotConstraint(int SlotNo, const Slot &Slot) override;
  void genLatencyConstraint(const Latency &L) override;
  void genConflict(int InstrA, int InstrB, int OffsetA = 0,
                   int OffsetB = 0) override;
  z3::expr genCycle(int I) override;
  void vars(const SolverData &Data, bool SEFStage) override;
  void scheduled(const SolverData &Data) override;
  int estimateSolverTime(ProblemSize Counts) const override;

public:
  Z3BinarySolver() = default;
};

// In the integer formulation, we have an integer stage and an integer cycle
// variable for each instruction.
// Constraints will be pretty compact, but we will have many of them,
// roughly quadratic in the number of instructions
class Z3IntegerSolver : public Z3Solver {
  std::vector<z3::expr> StageVarDecls;
  std::vector<z3::expr> CycleVarDecls;
  void vars(const SolverData &Data, bool SEFStage) override;
  void scheduled(const SolverData &Data) override;
  z3::expr genCycle(int N) override;
  void genConflict(int A, int B, int OffsetA = 0, int OffsetB = 0) override;
  void genSlotConstraint(int SlotNo, const Slot &Slot) override;

public:
  Z3IntegerSolver() = default;
};
#endif // LLVM_WITH_Z3

} // namespace llvm::AIE::Solver

#endif // LLVM_LIB_TARGET_AIE_AIESWPSOLVER_H
