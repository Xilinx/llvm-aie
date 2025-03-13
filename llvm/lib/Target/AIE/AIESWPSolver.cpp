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

#define DEBUG_TYPE "swpilp"

namespace llvm::AIE::Solver {

static cl::opt<int> AllowedSolverTime(
    "aie-postpipeliner-solver-timeout",
    cl::desc("Specify the timeout value for the solver in ms"), cl::init(2000),
    cl::Hidden);

// Time control is safe, but it we will depend on computer performance and
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

SWPSolver::~SWPSolver() {}

Slot &SWPSolver::addSlot(int N) {
  auto It = Slots.emplace(N, Slot(N)).first;
  return It->second;
}
int SWPSolver::addInsn(int SlotNumber, uint64_t MemoryBanks) {
  Slot *Slot = &addSlot(SlotNumber);
  int NextInsn = Instructions.size();
  Instructions.emplace(NextInsn, Instruction{Slot, MemoryBanks});
  Slot->Instructions.insert(NextInsn);
  return NextInsn;
}

int SWPSolver::getDepth(int I) { return Instructions.at(I).Depth; }

int SWPSolver::getHeight(int I) { return Instructions.at(I).Height; }
void SWPSolver::setDepth(int I, int D) {
  auto &Insn = Instructions.at(I);

  int NewDepth = std::max(Insn.Depth, D);

  // fprintf (stderr, "Depth(%d) -> %d\n", I, NewDepth);
  Insn.Depth = NewDepth;
}

void SWPSolver::setHeight(int I, int H) {
  auto &Insn = Instructions.at(I);

  int NewHeight = std::max(Insn.Height, H);

  // fprintf (stderr, "Height(%d) -> %d\n", I, NewHeight);
  Insn.Height = NewHeight;
}

bool SWPSolver::feasible(int I, int S, int C) {
  int Cycle = S * getII() + C;
  int Length = NumStages * getII();
  return Cycle >= getDepth(I) && Cycle < Length - getHeight(I);
}

void SWPSolver::addLatency(int Src, int Dst, int Latency, int Distance) {
  Latencies.emplace_back(Src, Dst, Latency, Distance);
  if (Distance) {
    return;
  }

  // ad hoc constraint: push Depth of Dst up
  // and push Height of Src up
  // fprintf(stderr, "%d pushes\n", Src);
  setDepth(Dst, getDepth(Src) + Latency);
  setHeight(Src, getHeight(Dst) + Latency);
}

void SWPSolver::setScheduleSize(int I, int NS) {
  II = I;
  NumStages = NS;
}

void SWPSolver::slots() {
  // For each slot, for each cycle, constrain to at most one use
  for (auto &[SlotNo, Slot] : Slots) {
    if (Slot.Instructions.empty()) {
      continue;
    }

    if (!genSlotConstraint(SlotNo, Slot)) {
      return;
    }
  }
}
void SWPSolver::conflicts() {
  for (auto &[M, I] : Instructions) {
    if (!I.MemoryBanks) {
      continue;
    }
    for (auto &[N, J] : Instructions) {
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

void SWPSolver::latencies() {
  // Just sorting the latency constraints like this makes z3 run >2x faster.
  std::sort(Latencies.begin(), Latencies.end(),
            [&](const Latency &A, const Latency &B) {
              if (A.Src != B.Src) {
                return A.Src < B.Src;
              }
              if (A.Dst != B.Dst) {
                return A.Dst < B.Dst;
              }
              return (A.Lat - A.Dist * getII()) > (B.Lat - B.Dist * getII());
            });

  // The latencies between a given pair of nodes are now adjacent
  // and descending, and we have at least one. We only need to
  // generate a constraint for the first, largest, latency for a given pair.
  std::optional<Latency> Prev;
  for (const auto &L : Latencies) {
    if (Prev && L.Src == Prev->Src && L.Dst == Prev->Dst) {
      LLVM_DEBUG(dbgs() << "Skip latency constraint " << L.Src << " -> "
                        << L.Dst << " (" << L.Lat << ")\n");
      continue;
    }
    if (!genLatencyConstraint(L)) {
      return;
    }
    Prev = L;
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
  Solver.add(atmost(nonempty(Elements), Limit));
}

void Z3Solver::cycles() {
  for (auto &[N, I] : Instructions) {
    CycleExprs.push_back(genCycle(N));
  }
}

std::vector<int> Z3Solver::getCycles() {
  z3::model M = Solver.get_model();
  std::vector<int> Cycles;
  for (auto &C : CycleExprs) {
    auto Val = M.eval(C);
    int IntVal;
    Z3_get_numeral_int(Context, Val, &IntVal);
    Cycles.push_back(IntVal);
  }
  return Cycles;
}

void Z3Solver::genModel() {

  // Compute the stage count to make every instruction's interval fit
  int Length = 0;
  for (auto &[N, I] : Instructions) {
    LLVM_DEBUG(dbgs() << I.Depth << " + " << I.Height << "\n");
    Length = std::max(Length, I.Depth + I.Height + 1);
  }
  const int II = getII();
  NumStages = std::max(NumStages, (Length + II - 1) / II);

  LLVM_DEBUG(dbgs() << "Minimum stage count = " << NumStages << "\n");

  vars();
  slots();
  scheduled();
  cycles();
  latencies();
  conflicts();
}

bool Z3Solver::solveModel() {
  if (DeterministicSolver) {
    const int MilliSeconds = estimateSolverTime(Sizes);
    if (MilliSeconds > AllowedSolverTime) {
      return false;
    }
  }
  LLVM_DEBUG(dbgs() << "Solving for II=" << getII() << " NS=" << NumStages);
  switch (Solver.check()) {
  case z3::unsat:
    LLVM_DEBUG(dbgs() << ": Unsatisfiable\n");
    return false;
  case z3::unknown:
    LLVM_DEBUG(dbgs() << ": Unknown\n");
    return false;
  default:
    LLVM_DEBUG(dbgs() << ": Solved\n");
    return true;
  }
}

std::string Z3BinarySolver::varname(int I, int S, int C) {
  std::string Name = "I" + std::to_string(I) + "_" + std::to_string(S) + "_" +
                     std::to_string(C);
  return Name;
}
std::string simpleName(const char *Prefix, int N) {
  std::string Name = Prefix + std::to_string(N);
  return Name;
}

std::optional<z3::expr> Z3BinarySolver::vardecl(int I, int S, int C) {
  return VarDecls[(I * NumStages + S) * getII() + C];
}

void Z3BinarySolver::vars() {
  const int II = getII();
  // Create the scheduled vars.
  for (auto &[N, I] : Instructions) {
    for (int S = 0; S < NumStages; S++) {
      for (int C = 0; C < II; C++) {
        std::optional<z3::expr> OptVar;
        if (feasible(N, S, C)) {
          OptVar = Context.bool_const(varname(N, S, C).c_str());
        }
        Sizes.oneMore(ProblemSize::NVariables);
        VarDecls.push_back(OptVar);
      }
    }
  }
}
void Z3BinarySolver::scheduled() {
  for (auto &[N, I] : Instructions) {
    z3::expr_vector Elements(Context);
    for (int S = 0; S < NumStages; S++) {
      for (int C = 0; C < getII(); C++) {
        addVar(N, S, C, Elements);
      }
    }
    atMost(Elements, 1);
    Solver.add(z3::atleast(nonempty(Elements), 1));
    Sizes.oneMore(ProblemSize::NInstrConstraints);
  }
}

void Z3BinarySolver::addVar(int N, int S, int C, z3::expr_vector &Elements) {
  std::optional<z3::expr> Var = vardecl(N, S, C);
  if (Var) {
    Elements.push_back(*Var);
  }
}

z3::expr Z3BinarySolver::genCycle(int I) {
  z3::expr_vector Elements(Context);
  for (int S = 0; S < NumStages; S++) {
    for (int C = 0; C < getII(); C++) {
      std::optional<z3::expr> Var = vardecl(I, S, C);
      if (!Var) {
        continue;
      }
      int LinearCycle = S * getII() + C;
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

bool Z3BinarySolver::genLatencyConstraint(const Latency &L) {
  LLVM_DEBUG(dbgs() << "Add " << L.Src << " --> " << L.Dst << " L=" << L.Lat
                    << " D=" << L.Dist << "\n");
  z3::expr Distance = Context.int_val(L.Lat - L.Dist * getII());
  Solver.add((CycleExprs[L.Dst] - CycleExprs[L.Src]) >= Distance);
  Sizes.oneMore(L.Dist ? ProblemSize::NLCDLatencyConstraints
                       : ProblemSize::NLatencyConstraints);
#if 0
  if (Solver.check() != z3::sat) {
    std::cout << "  FAILED\n";
    return false; 
  }
#endif
  return true;
}

bool Z3BinarySolver::genSlotConstraint(int SlotNo, const Slot &Slot) {
  for (int C = 0; C < getII(); C++) {
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
  return true;
}

void Z3BinarySolver::genConflict(int M, int N) {
  z3::expr_vector Elements(Context);
  // All stages have a contribution to a particular cycle)
  for (int C = 0; C < getII(); C++) {
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

void Z3IntegerSolver::vars() {
  // Create the stage and cycle vars
  for (auto &[N, I] : Instructions) {
    z3::expr SV = Context.int_const(simpleName("S", N).c_str());
    StageVarDecls.push_back(SV);
    z3::expr CV = Context.int_const(simpleName("C", N).c_str());
    CycleVarDecls.push_back(CV);
  }
}

void Z3IntegerSolver::scheduled() {
  // Constrain the variables to fit in the target schedule size
  for (auto &[N, I] : Instructions) {
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

bool Z3IntegerSolver::genSlotConstraint(int SlotNo, const Slot &Slot) {
  for (int I : Slot.Instructions) {
    for (int J : Slot.Instructions) {
      if (J < I) {
        Solver.add(CycleVarDecls[I] != CycleVarDecls[J]);
      }
    }
  }

  return true;
}
#endif // LLVM_WITH_Z3

void LPFile::genModel() {
  printf("max: ;\n");
  scheduled();
  conflicts();
  latencies();
  vars();
}

// Each instruction needs to be scheduled in some cycle in some stage.
void LPFile::scheduled() {
  for (auto &[I, Insn] : Instructions) {
    printf("// scheduled I%d constraint\n", I);
    const char *Plus = "";
    for (int S = 0; S < NumStages; S++) {
      for (int C = 0; C < getII(); C++) {
        if (!feasible(I, S, C)) {
          continue;
        }
        printf(VarFmt, Plus, Insn.TheSlot->SlotNumber, I, S, C);
        Plus = " + ";
      }
    }
    printf(" = 1;\n");
  }
}

// All variables need to be declared
void LPFile::vars() {
  for (auto &[I, Insn] : Instructions) {
    printf("// scheduled I%d vars\n", I);
    const char *Comma = "";
    printf("sos ");
    for (int S = 0; S < NumStages; S++) {
      for (int C = 0; C < getII(); C++) {
        if (!feasible(I, S, C)) {
          continue;
        }
        printf(VarFmt, Comma, Insn.TheSlot->SlotNumber, I, S, C);
        Comma = ", ";
      }
    }
    printf(" <= 1;\n");
  }
}

// Generate the expression for the (linear) cycle of an instruction
void LPFile::genCycle(int I, int Sign) {
  for (int S = 0; S < NumStages; S++) {
    for (int C = 0; C < getII(); C++) {
      if (!feasible(I, S, C)) {
        continue;
      }
      int Factor = Sign * (S * getII() + C);
      if (!Factor) {
        continue;
      }
      printf(" %+d ", Factor);
      printf(VarFmt, "", Instructions.at(I).TheSlot->SlotNumber, I, S, C);
    }
  }
}

// Only one occurrence of each slot in each cycle
bool LPFile::genSlotConstraint(int SlotNo, const Slot &Slot) {
  for (int C = 0; C < getII(); C++) {
    printf("\n// Slot %d, Cycle %d\n", SlotNo, C);
    const char *Plus = "";
    for (int I : Slot.Instructions) {
      for (int S = 0; S < NumStages; S++) {
        printf(VarFmt, Plus, SlotNo, I, S, C);
        Plus = " + ";
      }
    }
    printf(" <= 1;\n");
  }
  return true;
}

bool LPFile::genLatencyConstraint(const Latency &L) {
  printf("// Lat(%d -> %d) = %d (%d)\n", L.Src, L.Dst, L.Lat, L.Dist);
  genCycle(L.Dst, 1);
  genCycle(L.Src, -1);
  printf(" >= %d;\n", L.Lat - L.Dist * getII());
  return true;
}

void LPFile::genConflict(int M, int N) {
  llvm_unreachable("LPFile should implement genConflict");
}

} // namespace llvm::AIE::Solver
