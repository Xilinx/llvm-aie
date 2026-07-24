//===---AIEBaseTargetTransformInfo.cpp - AIEngine specific TTI ------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEBaseTargetTransformInfo.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"

using namespace llvm;

#define DEBUG_TYPE "aietti"

static cl::opt<bool>
    EnableAIEHardwareLoops("enable-aie-hardware-loops",
                           cl::desc("Enable hardware loops on AIE"),
                           cl::init(true), cl::Hidden);

static cl::opt<bool> EnableZOLForLoopsWithoutMinIterCount(
    "enable-aie-zol-without-minitercount",
    cl::desc("Enable zol for loops without minitercount"), cl::init(true),
    cl::Hidden);

static cl::opt<bool>
    AllowAIEZOL("enable-aie-zero-overhead-loops",
                cl::desc("Enable true zero overhead hardware loops on AIE"),
                cl::init(true), cl::Hidden);

/// We currently override MinIterCountHLReject == 1 to
/// a) remove loop guards and
/// b) disable HW loop generation
/// todo: separate loop guard removal from disabling HW Loops.
/// Note: Any Itercount provided by the user > 0 removes the loop guard in the
/// middle-end.
static cl::opt<int> MinIterCountHLReject(
    "aie-hardware-loops-minitercount", cl::Hidden, cl::init(1),
    cl::desc("Minimum trip count threshold for HL rejection"));

static cl::opt<bool>
    ForceHLGeneration("aie-force-hl-gen", cl::Hidden, cl::init(false),
                      cl::desc("Force HL generation ignoring metadata info."));

static cl::opt<bool>
    ConsiderLSROuterLoops("aie-lsr-consider-outer", cl::Hidden, cl::init(false),
                          cl::desc("Whether to consider outer loops for LSR"));

static cl::opt<bool>
    EnablePartialUnroll("aie-unroll-partial", cl::Hidden, cl::init(true),
                        cl::desc("Whether to partially unroll loops"));
static cl::opt<bool> EnableRuntimeUnroll(
    "aie-unroll-runtime", cl::Hidden, cl::init(false),
    cl::desc("Whether to unroll loops with runtime checks"));
static cl::opt<unsigned>
    MaxUnrollCount("aie-unroll-max-count", cl::Hidden, cl::init(4),
                   cl::desc("Maximum partial unroll count for loops"));
static cl::opt<int>
    MaxUnrollLoads("aie-unroll-max-loads", cl::Hidden, cl::init(-1),
                   cl::desc("Maximum partial unroll count for loops"));
static cl::opt<unsigned>
    MaxUnrollCost("aie-unroll-max-cost", cl::Hidden, cl::init(200),
                  cl::desc("Maximum partial unroll cost for loops"));
static cl::opt<unsigned> PreferSwpOverUnroll(
    "aie-prefer-swp-over-unroll", cl::Hidden, cl::init(9),
    cl::desc("Aim for pipelining if MinIterCount is at least this value."));

static cl::opt<bool>
    UnrollOnlyLoopsWithPragma("aie-unroll-only-pragma-loops",
                              cl::desc("Only unroll loops guarded by pragmas"),
                              cl::init(true), cl::Hidden);

static cl::opt<unsigned> LoopIdiomUnrollingThreshold(
    "aie-loop-idiom-unrolling-threshold",
    cl::desc("Unrolling threshold for unrolling vector idiom loops"),
    cl::init(350), cl::Hidden);

static cl::opt<bool> MergeCongruentIVs(
    "aie-merge-congruent-ivs", cl::Hidden, cl::init(false),
    cl::desc("Override AIE's default behavior for congruent IV merging. "
             "When set, allows merging even when IVs have separate load/store "
             "uses."));

bool AIETTICommon::isLoweredToCall(const Function *F) const {

  // Memory operations.
  if (F->getIntrinsicID() == Intrinsic::memcpy ||
      F->getIntrinsicID() == Intrinsic::memset ||
      F->getIntrinsicID() == Intrinsic::memmove) {
    return true;
  }

  // Disclaimer: We are currently distanced from the legalization of
  // instructions, as we have an additional layer (IRTranslator) in between.
  // While retrieving SDAG legalization rules is typically straightforward, the
  // same cannot be said for GISel. Certain intrinsics may be translated as G_F*
  // opcodes (of which there may be multiple) or as G_INTRINSIC. Although we can
  // attempt to predict the outcome of legalization - such as by examining
  // types - this method is often error-prone due to our customized legalization
  // rules. To clarify, let's compile a comprehensive list of intrinsics that
  // can be overridden by the target.
  if (F->getIntrinsicID() == Intrinsic::fmuladd) {
    return true;
  }

  // fmin/fmax are legal for f32 but lower to a libcall for f64 (double).
  if ((F->getIntrinsicID() == Intrinsic::minnum ||
       F->getIntrinsicID() == Intrinsic::maxnum) &&
      F->getReturnType()->getScalarType()->isDoubleTy()) {
    return true;
  }

  return !F->isIntrinsic();
}

bool AIETTICommon::isAllowedInZOL(Instruction &I) const {
  Type *Ty = I.getType();
  if (Ty->getScalarType()->isDoubleTy()) {
    return false;
  }
  switch (I.getOpcode()) {
  case Instruction::FAdd:
  case Instruction::FSub:
  case Instruction::FMul:
  case Instruction::FNeg:
    if (!Ty->getScalarType()->isBFloatTy()) {
      return false;
    }
    break;
  case Instruction::FCmp: {
    Type *CmpTy = I.getOperand(0)->getType();
    if (!CmpTy->getScalarType()->isBFloatTy()) {
      return false;
    }
    break;
  }
  case Instruction::FPExt: {
    // Extend from bfloat16 to float32
    Type *FromTy = I.getOperand(0)->getType();
    if (!FromTy->getScalarType()->isBFloatTy() ||
        !Ty->getScalarType()->isFloatTy()) {
      return false;
    }
    break;
  }
  case Instruction::FPTrunc: {
    // Trunc from float32 to bfloat16
    Type *FromTy = I.getOperand(0)->getType();
    if (!FromTy->getScalarType()->isFloatTy() ||
        !Ty->getScalarType()->isBFloatTy()) {
      return false;
    }
    break;
  }

  case Instruction::FDiv:
  case Instruction::FRem:
  case Instruction::FPToSI:
  case Instruction::FPToUI:
  case Instruction::UIToFP:
  case Instruction::SIToFP:
    return false;
  case Instruction::SDiv:
  case Instruction::UDiv:
  case Instruction::SRem:
  case Instruction::URem:
    return false;
  case Instruction::Mul:
    // i32 MUL is legal; i64 MUL lowers to a __muldi3 libcall.
    if (!Ty->isVectorTy() && Ty->getScalarType()->getScalarSizeInBits() > 32)
      return false;
    break;
  }
  return true;
}

void AIETTICommon::adjustUnrollingPreferences(
    Loop *L, ScalarEvolution &SE, TTI::UnrollingPreferences &UP,
    OptimizationRemarkEmitter *ORE) const {
  UP.Partial = EnablePartialUnroll;
  UP.Runtime = EnableRuntimeUnroll;
  UP.MaxCount = MaxUnrollCount;
  UP.FullUnrollMaxCount = 32;
  UP.Threshold = MaxUnrollCost;
  UP.AllowExpensiveTripCount = true;

  if (L->getNumBlocks() == 1) {
    BasicBlock *LoopBlock = L->getHeader();
    if (MaxUnrollLoads >= 0) {
      int NumLoads = count_if(*LoopBlock, [](const Instruction &I) {
        return I.mayReadFromMemory();
      });
      if (NumLoads)
        UP.MaxCount =
            std::min(UP.MaxCount, unsigned(MaxUnrollLoads / NumLoads));
    }
    auto MinIterCount = getMinTripCount(L->getLoopID());
    if (MinIterCount && *MinIterCount >= PreferSwpOverUnroll) {
      UP.Partial = false;
      UP.Runtime = false;
    }
  }
}

void AIETTICommon::applyLoopIdiomUnrolling(
    Loop *L, TTI::UnrollingPreferences &UP) const {
  if (isScalarizedVectorOpIdiomLoop(L) || isScalarLoop(L)) {
    UP.Threshold = LoopIdiomUnrollingThreshold;
  } else if (UnrollOnlyLoopsWithPragma && !AIELoopUtils::hasUnrollPragma(L)) {
    UP.Threshold = 0;
    UP.Partial = false;
  }
}

// Extract element vector -> intrinsic pattern.
// Checks for sequences where target-specific intrinsics (INV, INVSQRT,
// PUT_MS) are applied to an extracted element.
bool AIETTICommon::isVectorExtractIntrinsicSequence(
    const Instruction &I) const {
  if (auto *MaybeIntrinsicCall = dyn_cast<CallBase>(&I)) {
    Intrinsic::ID ID = MaybeIntrinsicCall->getIntrinsicID();
    if (isVectorExtractIntrinsicID(ID)) {
      if (isa<ExtractElementInst>(MaybeIntrinsicCall->getArgOperand(0)))
        return true;
    }
  }
  return false;
}

// get_ss -> Extract -> Insert vector element pattern.
// Checks for sequences where elements extracted from a GET_SS intrinsic
// call are inserted into a vector.
bool AIETTICommon::isGetSSExtractInsertVectorSequence(
    const Instruction &I) const {
  if (auto *MaybeInsert = dyn_cast<InsertElementInst>(&I)) {
    if (auto *MaybeExtract =
            dyn_cast<ExtractValueInst>(MaybeInsert->getOperand(1))) {
      if (auto *MaybeIntrinsicCall =
              dyn_cast<CallBase>(MaybeExtract->getAggregateOperand())) {
        Intrinsic::ID ID = MaybeIntrinsicCall->getIntrinsicID();
        if (isGetSSIntrinsicID(ID))
          return true;
      }
    }
  }
  return false;
}

bool AIETTICommon::isScalarizedVectorOpIdiomLoop(const Loop *L) const {
  if (L->getNumBlocks() != 1)
    return false;

  bool IsVectorLoopIdiom = false;
  const BasicBlock *LoopBlock = L->getHeader();

  for (auto &I : *LoopBlock) {
    if (isa<LoadInst>(I) || isa<StoreInst>(I))
      return false;
    // Weak detection for vector loop idioms that we want to unroll
    // to be able to further apply combiners.
    // TODO: implement exact detection by looking into IV + insert/extract.
    IsVectorLoopIdiom |= (isVectorExtractIntrinsicSequence(I) ||
                          isGetSSExtractInsertVectorSequence(I));
  }

  return IsVectorLoopIdiom;
}

bool AIETTICommon::isScalarLoop(const Loop *L) {
  if (L->getNumBlocks() != 1)
    return false;

  const BasicBlock *LoopBlock = L->getHeader();
  for (auto &I : *LoopBlock) {
    if (I.getType()->isVectorTy())
      return false;
    if (const auto *SI = dyn_cast<StoreInst>(&I)) {
      if (SI->getValueOperand()->getType()->isVectorTy())
        return false;
    }
  }
  return true;
}

bool AIETTICommon::isHardwareLoopProfitable(
    Loop *L, ScalarEvolution &SE, AssumptionCache &AC,
    TargetLibraryInfo *LibInfo, HardwareLoopInfo &HWLoopInfo) const {

  if (!EnableAIEHardwareLoops) {
    LLVM_DEBUG(dbgs() << "AIE Loops: Disabled\n");
    return false;
  }

  if (!SE.hasLoopInvariantBackedgeTakenCount(L)) {
    LLVM_DEBUG(dbgs() << "AIE Loops: No static backedge taken count\n");
    return false;
  }

  const SCEV *BackedgeTakenCount = SE.getBackedgeTakenCount(L);
  assert(!isa<SCEVCouldNotCompute>(BackedgeTakenCount));
  const SCEV *TripCountSCEV = SE.getAddExpr(
      BackedgeTakenCount, SE.getOne(BackedgeTakenCount->getType()));

  // We need to store the trip count in GPR/LC, a 32-bit register.
  if (SE.getUnsignedRangeMax(TripCountSCEV).getBitWidth() > 32) {
    LLVM_DEBUG(dbgs() << "AIE Loops: Trip count does not fit into 32bits\n");
    return false;
  }

  // For now, we'll handle only single BB loops for AIE
  // zero-overhead loop.
  if (L->getNumBlocks() > 1)
    return false;

  if (!ForceHLGeneration) {
    std::optional<int64_t> MinTripCount = getMinTripCount(L, &SE);
    if (MinTripCount) {
      // Reject HL for this case.
      if (*MinTripCount <= MinIterCountHLReject) {
        return false;
      }
    } else if (!EnableZOLForLoopsWithoutMinIterCount) {
      return false;
    }
  }

  // Scan the loop: loops with calls - make it unprofitable
  for (BasicBlock *BB : L->blocks()) {
    for (Instruction &I : *BB) {
      if (isa<CallInst>(I) || isa<InvokeInst>(I)) {
        if (const Function *F = cast<CallBase>(I).getCalledFunction()) {
          if (!isLoweredToCall(F)) {
            continue;
          }
        }
        return false;
      }
      if (!isAllowedInZOL(I)) {
        return false;
      }
    }
  }

  // We don't want to use ZOL in these cases
  LLVMContext &C = L->getHeader()->getContext();
  HWLoopInfo.CountType = Type::getInt32Ty(C);
  HWLoopInfo.LoopDecrement = ConstantInt::get(HWLoopInfo.CountType, 1);
  // We always allow nested hardware loops, but only the innermost loop
  // can use actual zero overhead loop instructions
  HWLoopInfo.IsNestingLegal = true;
  if (L->isInnermost() && AllowAIEZOL) {
    LLVM_DEBUG(dbgs() << "AIE Loops: Loop is ZOL candidate\n");
    HWLoopInfo.CounterInReg = false;
  } else {
    LLVM_DEBUG(dbgs() << "AIE Loops: Loop is JNZD candidate\n");
    HWLoopInfo.CounterInReg = true;
  }
  return true;
}

bool AIETTICommon::isProfitableOuterLSR(const Loop &L) const {
  // Down-counting loops are essentially always profitable for AIE.
  // They typically need a single GPR for counting down, while "up-counting"
  // loop need one for the IV, and one for the upper bound.
  return ConsiderLSROuterLoops.getNumOccurrences() > 0 ? ConsiderLSROuterLoops
                                                       : true;
}

InstructionCost AIETTICommon::getMemoryOpCost(unsigned Opcode, Type *Src,
                                              Align Alignment,
                                              unsigned AddressSpace,
                                              const DataLayout &DL) const {
  // Only handle Load operations with this custom cost model
  if (Opcode != Instruction::Load)
    return InstructionCost::getInvalid();

  // Only handle vector types
  auto *VTy = dyn_cast<VectorType>(Src);
  if (!VTy)
    return InstructionCost::getInvalid();

  // Calculate vector size in bytes
  const TypeSize VectorSize = DL.getTypeStoreSize(VTy);

  // Only handle fixed-size vectors
  if (VectorSize.isScalable())
    return InstructionCost::getInvalid();

  const unsigned VectorSizeInBytes = VectorSize.getFixedValue();
  const unsigned AlignmentInBytes = Alignment.value();

  // Check if the load is unaligned
  // A vector load is unaligned when alignment in bytes < total vector size in
  // bytes
  if (AlignmentInBytes < VectorSizeInBytes) {
    // Unaligned vector load is expensive!
    // Cost = NumElements × ScalarLoadCost + NumElements × InsertElementCost
    auto *FVTy = cast<FixedVectorType>(VTy);
    const unsigned NumElts = FVTy->getNumElements();

    // Assume scalar load cost is 1 (TCC_Basic) and insert element cost is 1
    // This gives us: NumElts * (1 + 1) = NumElts * 2
    const InstructionCost ScalarLoadCost = TTI::TCC_Basic;
    const InstructionCost InsertCost = TTI::TCC_Basic;

    // Total cost for unaligned load
    return NumElts * (ScalarLoadCost + InsertCost);
  }

  // For aligned loads, return invalid to let the base implementation handle it
  return InstructionCost::getInvalid();
}

// Standalone helper function for AIE congruent IV merging decision
bool llvm::shouldMergeCongruentIVsImpl(const PHINode *IV1, const PHINode *IV2) {
  // Check if command-line override is set
  if (MergeCongruentIVs)
    return true; // Use explicit override value

  // AIE-specific rule: Keep separate pointer IVs when one is used for loads
  // and the other for stores. This enables better instruction scheduling and
  // allows independent address generation units to work in parallel.

  // Helper to analyze if a pointer IV is used for loads and/or stores
  auto AnalyzePointerUses = [](const PHINode *IV) -> std::pair<bool, bool> {
    bool HasLoads = false;
    bool HasStores = false;

    SmallPtrSet<const Value *, 16> Visited;
    SmallVector<const Value *, 16> Worklist;
    Worklist.push_back(IV);
    Visited.insert(IV);

    while (!Worklist.empty()) {
      const Value *V = Worklist.pop_back_val();

      for (const Use &U : V->uses()) {
        const User *Usr = U.getUser();

        if (auto *LI = dyn_cast<LoadInst>(Usr)) {
          HasLoads |= LI->getPointerOperand() == V;
        } else if (auto *SI = dyn_cast<StoreInst>(Usr)) {
          HasStores |= SI->getPointerOperand() == V;
        } else if (isa<GetElementPtrInst>(Usr) || isa<BitCastInst>(Usr)) {
          if (Visited.insert(Usr).second)
            Worklist.push_back(Usr);
        }

        // Early exit: if we found both loads and stores, no need to continue
        if (HasLoads && HasStores)
          return {true, true};
      }
    }

    return {HasLoads, HasStores};
  };

  // Analyze IV1 first
  auto [IV1HasLoads, IV1HasStores] = AnalyzePointerUses(IV1);

  // Early return: If IV1 has both loads and stores, it can't be separated
  if (IV1HasLoads && IV1HasStores)
    return true; // OK to merge - IV1 is mixed

  // Early return: If IV1 has neither loads nor stores, merge is OK
  if (!IV1HasLoads && !IV1HasStores)
    return true;

  // IV1 is pure (only loads OR only stores), check IV2
  auto [IV2HasLoads, IV2HasStores] = AnalyzePointerUses(IV2);

  // Don't merge if they're separated: one load-only, other store-only
  if (IV1HasLoads && !IV1HasStores && IV2HasStores && !IV2HasLoads) {
    LLVM_DEBUG(dbgs() << "AIE TTI: Not merging congruent IVs - "
                      << "keeping separate load/store pointers\n");
    return false;
  }

  if (IV1HasStores && !IV1HasLoads && IV2HasLoads && !IV2HasStores) {
    LLVM_DEBUG(dbgs() << "AIE TTI: Not merging congruent IVs - "
                      << "keeping separate load/store pointers\n");
    return false;
  }

  return true; // OK to merge in all other cases
}
