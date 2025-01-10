//===- AIESWPSolver.h - Software Pipeliner Solver infrastructure-----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIESWPSOLVER_H
#define LLVM_LIB_TARGET_AIE_AIESWPSOLVER_H

#include "llvm/Config/config.h"
#if LLVM_WITH_Z3
#include "z3++.h"
#endif

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <map>
#include <optional>
#include <set>
#include <vector>

namespace llvm::AIE::Solver {
class Slot {
public:
  int SlotNumber;
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
  int Depth = 0;
  int Height = 0;
  Slot *TheSlot;
  uint64_t MemoryBanks = 0;
  Instruction(Slot *S, uint64_t MemoryBanks)
      : TheSlot(S), MemoryBanks(MemoryBanks) {}
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

class SWPSolver {
  int II = 0;

protected:
  int NumStages;
  int NSlots;
  std::map<int, Slot> Slots;
  std::map<int, Instruction> Instructions;
  std::vector<Latency> Latencies;

  int getII() const {
    assert(II > 0);
    return II;
  }

  // Add a slot to the problem
  Slot &addSlot(int N);
  // Generate the latency constraints
  void latencies();
  // Generate the slot constraints
  void slots();
  // Generate further instruction conflict constraints
  void conflicts();

  int getDepth(int I);
  int getHeight(int I);
  void setDepth(int I, int D);
  void setHeight(int I, int H);

  // Check whether a variable can be non-zero due to depth or height of the
  // associated instruction
  bool feasible(int I, int S, int C);

  // Generate the slot constraint for the given slot
  virtual bool genSlotConstraint(int SlotNo, const Slot &Slot) = 0;
  // Generate a constraint that represents a dependence latency
  virtual bool genLatencyConstraint(const Latency &L) = 0;
  // Generate a mutual exclusion constraint for instructions M and N in any
  // cycle
  virtual void genConflict(int M, int N) = 0;

  // Return the vector of instruction cycles
  // \pre genModel() has returned true
  virtual std::vector<int> getCycles() { return {}; };

public:
  virtual ~SWPSolver();
  // Add an instruction to the problem. It returns a unique Id
  int addInsn(int Slot, uint64_t MemoryBanks);
  // Add a latency between two instructions to the problem.
  // Distance represents the iteration distance, i.e. the number of
  // cfg backedges it spans.
  void addLatency(int Src, int Dst, int Latency, int Distance = 0);
  // Set the desired schedule size in terms of II and stage count
  void setScheduleSize(int I, int NS = 2);
  // Generate the model.
  virtual void genModel() = 0;
  // Call the solver on the model. Return whether it was satisfiable.
  virtual bool solveModel() = 0;
};

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
  virtual void vars() = 0;
  // Generate the constraint that all instructions must be scheduled
  virtual void scheduled() = 0;

  // Return a z3 expression that represents the cycle of an instruction
  // in the linear schedule.
  virtual z3::expr genCycle(int I) = 0;

  // Generate expressions for the cycle of each instruction.
  void cycles();

  // Convenience functions to protect against empty sets.
  static const z3::expr_vector &nonempty(const z3::expr_vector &Elements) {
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
  void genModel() override;
  bool solveModel() override;
  std::vector<int> getCycles() override;
};

// In the binary formulation, we have a lot of binary variables,
// but relatively few, though elaborate constraints.
class Z3BinarySolver : public Z3Solver {
  std::vector<std::optional<z3::expr>> VarDecls;

  std::string varname(int N, int S, int C);
  std::optional<z3::expr> vardecl(int N, int S, int C);

  // generate an expression that represents I running in modulo cycle C
  z3::expr genModuloCycle(int I, int C);

  // If it exists, add a schedule variable declaration to Elements
  void addVar(int N, int S, int C, z3::expr_vector &Elements);

  // generate the constraint that only one instance of Slot is present in
  // cycle C
  bool genSlotConstraint(int SlotNo, const Slot &Slot) override;
  bool genLatencyConstraint(const Latency &L) override;
  void genConflict(int M, int N) override;
  z3::expr genCycle(int I) override;
  void vars() override;
  void scheduled() override;
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
  void vars() override;
  void scheduled() override;
  z3::expr genCycle(int N) override;
  void genConflict(int M, int N) override;
  bool genSlotConstraint(int SlotNo, const Slot &Slot) override;

public:
  Z3IntegerSolver() = default;
};
#endif // LLVM_WITH_Z3

class LPFile : public SWPSolver {
  // Prefix, SlotNumber, InstrNr, Stage, Cycle
  const char *const VarFmt = "%sV%d_%d_%d_%d";

public:
  void genModel() override;
  void scheduled();
  void vars();
  void genCycle(int I, int Sign);
  bool genSlotConstraint(int SlotNo, const Slot &Slot) override;
  bool genLatencyConstraint(const Latency &L) override;
  void genConflict(int M, int N) override;
};

} // namespace llvm::AIE::Solver

#endif // LLVM_LIB_TARGET_AIE_AIESWPSOLVER_H
