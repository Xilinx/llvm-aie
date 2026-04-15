//===- AIESWPSolver.cpp - Solver infrastructure                            ===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// This file contains an interface to create constraints to model a software
// pipelining problem.
//===----------------------------------------------------------------------===//

#include "AIESWPSolver.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <chrono>
#include <memory>

#define DEBUG_TYPE "swpsolver"
#define DEBUG_SUMMARY(X) DEBUG_WITH_TYPE("postpipeliner-summary", X)

namespace llvm::AIE::Solver {

static cl::opt<int> AllowedSolverTime(
    "aie-postpipeliner-solver-timeout",
    cl::desc("Specify the timeout value for the solver in ms"), cl::init(2000),
    cl::Hidden);

// Real time control is safe, but it will depend on computer performance and
// load whether we find a solution. With this option set, we will estimate
// the time deterministically and use the timeout to call the solver or not.
static cl::opt<bool>
    DeterministicSolver("aie-postpipeliner-deterministic-solver",
                        cl::desc("Make the solver behave deterministically"),
                        cl::init(true), cl::Hidden);

ProblemSize::ProblemSize() {
  for (int C = 0; C < ProblemSize::NumSizeComponents; C++) {
    Counts[C] = 0;
  }
}
void ProblemSize::dump() {
  for (int C = 0; C < ProblemSize::NumSizeComponents; C++) {
    dbgs() << Counts[C] << ", ";
  }
}

std::vector<std::unique_ptr<SWPSolver>> getSolvers() {
  std::vector<std::unique_ptr<SWPSolver>> Solvers;
#if LLVM_WITH_Z3
  Solvers.emplace_back(std::make_unique<Z3BinarySolver>());
#else
  LLVM_DEBUG(dbgs() << "No solver supplied since Z3 is not available");
#endif // LLVM_WITH_Z3
  return Solvers;
}

bool hasSolver() {
#if LLVM_WITH_Z3
  return true;
#else
  return false;
#endif // LLVM_WITH_Z3
}

Slot &SolverData::addSlot(int N) {
  auto It = Slots.emplace(N, Slot(N)).first;
  return It->second;
}

int SolverData::addInstruction(int SlotNumber, uint64_t MemoryBanks,
                               bool HasSideEffect) {
  Slot *const Slot = &addSlot(SlotNumber);
  const int Id = Instructions.size();
  Instructions.emplace_back(Id, Slot, MemoryBanks, HasSideEffect);
  Slot->Instructions.insert(Id);
  return Id;
}

void SolverData::addLatency(int Src, int Dst, int Latency, int Distance) {
  Latencies.emplace_back(Src, Dst, Latency, Distance);
  if (Distance) {
    return;
  }

  // ad hoc constraint: push Depth of Dst up
  // and push Height of Src up
  setDepth(Dst, getDepth(Src) + Latency);
  setHeight(Src, getHeight(Dst) + Latency);
}

void SolverData::finalize(int II) {
  // Just sorting the latency constraints like this makes z3 run >2x faster.
  std::sort(Latencies.begin(), Latencies.end(),
            [II](const Latency &A, const Latency &B) {
              if (A.Src != B.Src) {
                return A.Src < B.Src;
              }
              if (A.Dst != B.Dst) {
                return A.Dst < B.Dst;
              }
              return (A.Lat - A.Dist * II) > (B.Lat - B.Dist * II);
            });
}

bool SolverData::isFeasible(int I, int LinearCycle, int Length) const {
  return LinearCycle >= getDepth(I) && LinearCycle < Length - getHeight(I);
}

int SolverData::getDepth(int I) const { return Instructions.at(I).Depth; }
int SolverData::getHeight(int I) const { return Instructions.at(I).Height; }
bool SolverData::hasSideEffect(int I) const {
  return Instructions.at(I).HasSideEffect;
}

void SolverData::setDepth(int I, int D) {
  auto &Insn = Instructions.at(I);
  Insn.Depth = std::max(Insn.Depth, D);
}

void SolverData::setHeight(int I, int H) {
  auto &Insn = Instructions.at(I);
  Insn.Height = std::max(Insn.Height, H);
}

SWPSolver::~SWPSolver() {}

void SWPSolver::setScheduleSize(int I, int NS) {
  II = I;
  NumStages = NS;
}

void SWPSolver::slots(const SolverData &Data) {
  // For each slot, for each cycle, constrain to at most one use
  for (const auto &[SlotNo, Slot] : Data.getSlots()) {
    if (Slot.Instructions.empty()) {
      continue;
    }

    genSlotConstraint(SlotNo, Slot);
  }
}

void SWPSolver::latencies(const SolverData &Data) {
  // The latencies between a given pair of nodes are adjacent
  // and descending, and we have at least one. We only need to
  // generate a constraint for the first, largest, latency for a given pair.
  std::optional<Latency> Prev;
  for (const auto &L : Data.getLatencies()) {
    if (Prev && L.Src == Prev->Src && L.Dst == Prev->Dst) {
      LLVM_DEBUG(dbgs() << "Skip latency constraint " << L.Src << " -> "
                        << L.Dst << " (" << L.Lat << ")\n");
      continue;
    }
    genLatencyConstraint(L);
    Prev = L;
  }
}

void SWPSolver::conflicts(const SolverData &Data) {
  for (const auto &I : Data.getInstructions()) {
    const int M = I.Id;
    if (!I.MemoryBanks) {
      continue;
    }
    for (const auto &J : Data.getInstructions()) {
      const int N = J.Id;
      // Mutual exclusion is symmetric, there's only a conflict
      // if there's an overlap in the memory banks and the conflict
      // constraint is redundant if the slots already collide
      if (N <= M || (I.MemoryBanks & J.MemoryBanks) == 0 ||
          I.TheSlot == J.TheSlot) {
        continue;
      }

      LLVM_DEBUG(dbgs() << "Bank conflict(" << M << ", " << N << ")\n");
      genConflict(M, N);
    }
  }
}

#if LLVM_WITH_Z3
Z3Solver::Z3Solver() : Solver(Context), Zero(Context.int_val(0)) {
  // timeout behaves undeterministically
  if (!DeterministicSolver) {
    Z3_params Params = Z3_mk_params(Context);
    Z3_params_set_uint(Context, Params, Z3_mk_string_symbol(Context, "timeout"),
                       AllowedSolverTime);
    Z3_solver_set_params(Context, Solver, Params);
  }
}

void Z3Solver::atMost(const z3::expr_vector &Elements, int Limit) {
  Solver.add(atmost(nonEmpty(Elements), Limit));
}

void Z3Solver::cycles(const SolverData &Data) {
  for (const auto &I : Data.getInstructions()) {
    const int N = I.Id;
    CycleExprs.push_back(genCycle(N));
  }
}

std::vector<int> Z3Solver::getSUCycles() {
  std::vector<int> Cycles;
  z3::model M = Solver.get_model();
  for (const auto &C : CycleExprs) {
    auto Val = M.eval(C);
    int IntVal;
    Z3_get_numeral_int(Context, Val, &IntVal);
    Cycles.push_back(IntVal);
  }
  return Cycles;
}

void Z3Solver::genModel(const SolverData &Data, bool SEFStage) {
  // Compute the stage count to make every instruction's interval fit
  int Length = 0;
  for (const auto &I : Data.getInstructions()) {
    const int N = I.Id;
    LLVM_DEBUG(dbgs() << "I" << N << ":" << I.HasSideEffect << " " << I.Depth
                      << " + " << I.Height << " + 1\n");
    // The length should contain what's before I, what's after I and I itself;
    Length = std::max(Length, I.Depth + I.Height + 1);
  }
  const int II = getII();
  NumStages = std::max(NumStages, (Length + II - 1) / II);

  LLVM_DEBUG(dbgs() << "Minimum stage count = " << NumStages
                    << " SEFStage = " << SEFStage << "\n");

  vars(Data, SEFStage);
  slots(Data);
  scheduled(Data);
  cycles(Data);
  latencies(Data);
  conflicts(Data);
}

bool Z3Solver::solveModel() {
  if (AllowedSolverTime == 0) {
    // 0 really means zero milliseconds, not 400 microseconds rounded down.
    return false;
  }
  if (DeterministicSolver) {
    const int MilliSeconds = estimateSolverTime(Sizes);
    if (MilliSeconds > AllowedSolverTime) {
      DEBUG_SUMMARY(dbgs() << "Solver: Estimated time(" << MilliSeconds
                           << " ms) exceeds limit(" << AllowedSolverTime
                           << " ms).\n");
      return false;
    }
  }
  DEBUG_SUMMARY(dbgs() << "Solving for II=" << getII() << " NS=" << NumStages);
  auto Start = std::chrono::high_resolution_clock::now();
  auto Outcome = Solver.check();
  auto End = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> Elapsed = End - Start;
  switch (Outcome) {
  case z3::unsat:
    DEBUG_SUMMARY(dbgs() << ": Unsatisfiable, time=" << Elapsed.count()
                         << " ms\n");
    return false;
  case z3::unknown:
    DEBUG_SUMMARY(dbgs() << ": Unknown, time=" << Elapsed.count() << " ms\n");
    return false;
  default:
    DEBUG_SUMMARY(dbgs() << ": Solved, time=" << Elapsed.count() << " ms\n");
    return true;
  }
}

std::string Z3BinarySolver::varName(int I, int S, int C) {
  std::string Name = "I" + std::to_string(I) + "_" + std::to_string(S) + "_" +
                     std::to_string(C);
  return Name;
}

namespace {
std::string simpleName(const char *Prefix, int N) {
  std::string Name = Prefix + std::to_string(N);
  return Name;
}
} // namespace

std::optional<z3::expr> Z3BinarySolver::varDecl(int I, int S, int C) {
  return VarDecls[(I * NumStages + S) * getII() + C];
}

void Z3BinarySolver::vars(const SolverData &Data, bool SEFStage) {
  // Create the 'scheduled' vars. For a SEFStage, we try to create a first
  // short stage that is side effect free. We rotate the start of the loop
  // to just after that stage. We limit the total length of the schedule
  // accordingly. Effectively, we have a fractional number of stages, with
  // the first stage representing the fractional part.
  const int II = getII();
  // The length of the fractional stage. We may want to be more clever, but
  // it works on our motivating example. Perhaps we want to make it follow
  // the number of SEF instructions. Perhaps we want to cap it.
  const int SEFFraction = II / 2;
  // We subtract the complement of the SEFFraction from the regular length
  EffectiveLength = NumStages * II - SEFStage * (II - SEFFraction);
  for (const auto &I : Data.getInstructions()) {
    const int N = I.Id;
    const int SEFMax = (SEFStage && Data.hasSideEffect(N)) ? SEFFraction : 0;
    for (int S = 0; S < NumStages; S++) {
      for (int C = 0; C < II; C++) {
        const int LinearCycle = S * II + C;
        std::optional<z3::expr> OptVar;

        if (LinearCycle >= SEFMax &&
            Data.isFeasible(N, LinearCycle, EffectiveLength)) {
          OptVar = Context.bool_const(varName(N, S, C).c_str());
          Sizes.oneMore(ProblemSize::NVariables);
        }
        // Note that we have a rigid indexing scheme, and need to have an entry
        // for every {I,S,C} combination. Actual existence is provided by the
        // optional<> wrapping
        VarDecls.push_back(OptVar);
      }
    }
  }
}

void Z3BinarySolver::scheduled(const SolverData &Data) {
  const int II = getII();
  for (const auto &I : Data.getInstructions()) {
    const int N = I.Id;
    z3::expr_vector Elements(Context);
    for (int S = 0; S < NumStages; S++) {
      for (int C = 0; C < II; C++) {
        addVar(N, S, C, Elements);
      }
    }
    atMost(Elements, 1);
    Solver.add(z3::atleast(nonEmpty(Elements), 1));
    Sizes.oneMore(ProblemSize::NInstrConstraints);
  }
}

void Z3BinarySolver::addVar(int N, int S, int C, z3::expr_vector &Elements) {
  std::optional<z3::expr> Var = varDecl(N, S, C);
  if (Var) {
    Elements.push_back(*Var);
  }
}

z3::expr Z3BinarySolver::genCycle(int I) {
  const int II = getII();
  z3::expr_vector Elements(Context);
  for (int S = 0; S < NumStages; S++) {
    for (int C = 0; C < II; C++) {
      std::optional<z3::expr> Var = varDecl(I, S, C);
      if (!Var) {
        continue;
      }
      const int LinearCycle = S * II + C;
      if (!LinearCycle) {
        continue;
      }
      z3::expr Factor = Context.int_val(LinearCycle);
      Elements.push_back(z3::ite(*Var, Factor, Zero));
    }
  }
  if (Elements.empty()) {
    // Z3 can't deal with the sum of zero terms
    return Zero;
  }
  return z3::sum(Elements);
}

void Z3BinarySolver::genLatencyConstraint(const Latency &L) {
  LLVM_DEBUG(dbgs() << "Add " << L.Src << " --> " << L.Dst << " L=" << L.Lat
                    << " D=" << L.Dist << "\n");
  z3::expr Distance = Context.int_val(L.Lat - L.Dist * getII());
  Solver.add((CycleExprs[L.Dst] - CycleExprs[L.Src]) >= Distance);
  Sizes.oneMore(L.Dist ? ProblemSize::NLCDLatencyConstraints
                       : ProblemSize::NLatencyConstraints);
}

void Z3BinarySolver::genSlotConstraint(int SlotNo, const Slot &Slot) {
  const int II = getII();
  for (int C = 0; C < II; C++) {
    z3::expr_vector Elements(Context);
    for (int I : Slot.Instructions) {
      for (int S = 0; S < NumStages; S++) {
        addVar(I, S, C, Elements);
      }
    }
    // Some cycles may not be feasible for all instructions in a slot
    if (Elements.empty()) {
      continue;
    }
    Sizes.oneMore(ProblemSize::NSlotConstraints);
    atMost(Elements, 1);
  }
}

void Z3BinarySolver::genConflict(int M, int N) {
  const int II = getII();
  z3::expr_vector Elements(Context);
  // All stages have a contribution to a particular cycle)
  for (int C = 0; C < II; C++) {
    z3::expr_vector Elements(Context);
    for (int S = 0; S < NumStages; S++) {
      addVar(M, S, C, Elements);
      addVar(N, S, C, Elements);
    }
    if (Elements.empty()) {
      continue;
    }
    Sizes.oneMore(ProblemSize::NConflicts);
    atMost(Elements, 1);
  }
}

int Z3BinarySolver::estimateSolverTime(ProblemSize Counts) const {
  uint64_t Rows = Counts[ProblemSize::NSlotConstraints];
  Rows += Counts[ProblemSize::NLatencyConstraints];
  Rows += Counts[ProblemSize::NLCDLatencyConstraints];
  Rows += 2 * Counts[ProblemSize::NInstrConstraints];
  Rows += Counts[ProblemSize::NConflicts];
  uint64_t Columns = Counts[ProblemSize::NVariables];
  double Score = .02 * Rows * Columns;
  LLVM_DEBUG(dbgs() << "Counts: "; Counts.dump(); dbgs() << "\n");
  LLVM_DEBUG(dbgs() << "EstimatedTime=" << Score << "ms\n");
  return Score;
}

void Z3IntegerSolver::vars(const SolverData &Data, bool SEFStage) {
  assert(!SEFStage && "SEFStage not implemented yet for Z3IntegerSolver");
  // Create the stage and cycle vars
  for (const auto &I : Data.getInstructions()) {
    const int N = I.Id;
    z3::expr SV = Context.int_const(simpleName("S", N).c_str());
    StageVarDecls.push_back(SV);
    z3::expr CV = Context.int_const(simpleName("C", N).c_str());
    CycleVarDecls.push_back(CV);
  }
}

void Z3IntegerSolver::scheduled(const SolverData &Data) {
  // Constrain the variables to fit in the target schedule size
  for (const auto &I : Data.getInstructions()) {
    const int N = I.Id;
    z3::expr &SV = StageVarDecls[N];
    Solver.add(SV >= 0);
    Solver.add(SV < NumStages);
    z3::expr &CV = CycleVarDecls[N];
    Solver.add(CV >= 0);
    Solver.add(CV < getII());
  }
}

z3::expr Z3IntegerSolver::genCycle(int N) {
  return StageVarDecls[N] * getII() + CycleVarDecls[N];
}

void Z3IntegerSolver::genConflict(int M, int N) {
  Solver.add(CycleVarDecls[M] != CycleVarDecls[N]);
}

void Z3IntegerSolver::genSlotConstraint(int SlotNo, const Slot &Slot) {
  for (int I : Slot.Instructions) {
    for (int J : Slot.Instructions) {
      if (J < I) {
        Solver.add(CycleVarDecls[I] != CycleVarDecls[J]);
      }
    }
  }
}
#endif // LLVM_WITH_Z3

} // namespace llvm::AIE::Solver
