//===-- AIEOuterLoopPointerOptimizer.cpp - Pointer chain optimization -----===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This pass optimizes pointer chains in outer loops that follow a specific
// structure: prologue -> single-block inner loop -> epilogue.
//
// Optimizations:
// 1. GEP Address Space Canonicalization: Keep GEPs in the PHI's canonical
//    address space. This moves addrspacecast from before GEPs to point-of-use,
//    enabling better GEP chain optimization.
// 2. GEP Canonicalization: Convert non-i8 GEPs to i8-based GEPs for uniformity.
//    This makes pointer arithmetic explicit and helps later optimizations.
// 3. GEP Chain Linking: Link consecutive GEPs with the same base pointer to
//    enable post-increment addressing patterns.
// 4. GEP Hoisting: Move GEPs from bottom (epilogue) to top (prologue) block
//    when safe, reducing code in the epilogue and improving scheduling. We try
//    to avoid stand-alone pointer updates by grouping them with related memory
//    operations.
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include <optional>

using namespace llvm;

#define DEBUG_TYPE "aie-outer-loop-pointer-optimizer"

namespace {

cl::opt<bool> EnableOuterLoopPointerOpt(
    "aie-enable-outer-loop-pointer-opt",
    cl::desc("Enable outer loop pointer optimization"), cl::init(true),
    cl::Hidden);

cl::opt<bool> EnableGEPCanonicalization(
    "aie-enable-gep-canonicalization",
    cl::desc("Enable GEP canonicalization to i8-based GEPs"), cl::init(true),
    cl::Hidden);

cl::opt<bool> EnableGEPChainLinking(
    "aie-enable-gep-chain-linking",
    cl::desc("Enable GEP chain linking for post-increment addressing"),
    cl::init(true), cl::Hidden);

cl::opt<bool>
    EnableGEPHoisting("aie-enable-gep-hoisting",
                      cl::desc("Enable hoisting GEPs from bottom to top block"),
                      cl::init(true), cl::Hidden);

cl::opt<bool> EnableGEPAddressSpaceCanon(
    "aie-enable-gep-addrspace-canon",
    cl::desc(
        "Enable GEP address space canonicalization to PHI's address space"),
    cl::init(true), cl::Hidden);

//===----------------------------------------------------------------------===//
// Helper Functions
//===----------------------------------------------------------------------===//

/// Collect all GEPs from a basic block.
SmallVector<GetElementPtrInst *, 16> collectGEPs(BasicBlock *BB) {
  SmallVector<GetElementPtrInst *, 16> GEPs;
  for (Instruction &I : *BB)
    if (GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(&I))
      GEPs.push_back(GEP);
  return GEPs;
}

/// Check if a GEP has a single index (simple GEP).
bool isSimpleGEP(const GetElementPtrInst *GEP) {
  return GEP->getNumIndices() == 1;
}

/// Check if a GEP is i8-based with a single index.
bool isSimpleI8GEP(const GetElementPtrInst *GEP) {
  return isSimpleGEP(GEP) && GEP->getSourceElementType()->isIntegerTy(8);
}

/// Compute the byte offset for a GEP index with CSE support.
/// Returns Index unchanged if ElemSize is 1.
Value *
getByteOffset(Value *Index, uint64_t ElemSize, Instruction *InsertBefore,
              Loop *OuterLoop, BasicBlock *Preheader,
              DenseMap<std::pair<Value *, uint64_t>, Value *> &PreheaderMuls,
              DenseMap<std::pair<Value *, uint64_t>, Value *> &LocalMuls) {
  // No scaling needed for byte-sized elements
  if (ElemSize == 1)
    return Index;

  const auto Key = std::make_pair(Index, ElemSize);
  const bool IsLoopInvariant = OuterLoop->isLoopInvariant(Index);

  // Select map and insertion point based on loop invariance
  DenseMap<std::pair<Value *, uint64_t>, Value *> &MulMap =
      IsLoopInvariant ? PreheaderMuls : LocalMuls;
  Instruction *const InsertPt =
      IsLoopInvariant ? Preheader->getTerminator() : InsertBefore;

  // CSE lookup
  auto It = MulMap.find(Key);
  if (It != MulMap.end()) {
    LLVM_DEBUG(dbgs() << "OLPO:   Reusing mul for index " << *Index << " * "
                      << ElemSize << "\n");
    return It->second;
  }

  // Create mul at insertion point
  IRBuilder<> Builder(InsertPt);
  Value *Scale = ConstantInt::get(Index->getType(), ElemSize);
  Value *ByteOffset = Builder.CreateMul(Index, Scale, "byte_offset");
  MulMap[Key] = ByteOffset;
  LLVM_DEBUG(dbgs() << "OLPO:   Created mul: " << *ByteOffset << "\n");
  return ByteOffset;
}

//===----------------------------------------------------------------------===//
// GEP Canonicalization Helpers (non-i8 to i8-based)
//===----------------------------------------------------------------------===//

/// Build a new GEP with an i8-based version using the given byte offset.
Value *buildNewGEPWithI8Based(GetElementPtrInst *GEP, Value *ByteOffset) {
  IRBuilder<> Builder(GEP);
  Value *NewGEP =
      Builder.CreateGEP(Builder.getInt8Ty(), GEP->getPointerOperand(),
                        ByteOffset, GEP->getName());
  if (GetElementPtrInst *NewGEPInst = dyn_cast<GetElementPtrInst>(NewGEP))
    NewGEPInst->setIsInBounds(GEP->isInBounds());
  return NewGEP;
}

/// Check if a GEP needs canonicalization to i8-based.
bool needsI8Canonicalization(GetElementPtrInst *GEP) {
  // Skip GEPs that already use i8 as source element type
  if (GEP->getSourceElementType()->isIntegerTy(8))
    return false;
  // We only handle simple GEPs with a single index
  return isSimpleGEP(GEP);
}

//===----------------------------------------------------------------------===//
// GEP Chain Linking Helpers
//===----------------------------------------------------------------------===//

/// State for tracking GEP chains by base pointer.
struct GEPChainState {
  GetElementPtrInst *LastGEP = nullptr;
  int64_t LastOffset = 0;
};

/// Check if a GEP is a valid candidate for chain linking.
/// Must be i8-based with a single positive constant index.
bool isChainLinkCandidate(GetElementPtrInst *GEP, int64_t &OutOffset) {
  if (!isSimpleI8GEP(GEP))
    return false;

  ConstantInt *ConstIdx = dyn_cast<ConstantInt>(GEP->getOperand(1));
  if (!ConstIdx)
    return false;

  OutOffset = ConstIdx->getSExtValue();
  // Must be positive offset
  return OutOffset > 0;
}

/// Try to link a GEP to an existing chain, rewriting it to use delta offset.
/// Returns the new GEP if linked, nullptr otherwise.
GetElementPtrInst *tryLinkToChain(GetElementPtrInst *GEP,
                                  GEPChainState &ChainState,
                                  int64_t CurrentOffset, DominatorTree *DT) {
  GetElementPtrInst *const PrevGEP = ChainState.LastGEP;
  const int64_t PrevOffset = ChainState.LastOffset;
  const int64_t DeltaOffset = CurrentOffset - PrevOffset;

  // Only link if delta is positive (moving forward)
  if (DeltaOffset <= 0)
    return nullptr;

  // Verify the previous GEP dominates this one
  if (!DT->dominates(PrevGEP, GEP))
    return nullptr;

  // Rewrite this GEP to use the previous GEP as its base
  IRBuilder<> Builder(GEP);
  const ConstantInt *ConstIdx = cast<ConstantInt>(GEP->getOperand(1));
  Type *const IdxTy = ConstIdx->getType();
  Value *DeltaOffsetVal = ConstantInt::get(IdxTy, DeltaOffset);
  Value *NewGEP =
      Builder.CreateGEP(Builder.getInt8Ty(), PrevGEP, DeltaOffsetVal,
                        GEP->getName() + ".chained");

  GetElementPtrInst *NewGEPInst = dyn_cast<GetElementPtrInst>(NewGEP);
  NewGEPInst->setIsInBounds(GEP->isInBounds());

  return NewGEPInst;
}

//===----------------------------------------------------------------------===//
// GEP Hoisting Helpers
//===----------------------------------------------------------------------===//

/// Check if a value is used by any memory operation (load/store) in the block.
bool hasMemoryUseInBlock(const Value *V, const BasicBlock *BB) {
  for (const User *U : V->users()) {
    const Instruction *UI = dyn_cast<Instruction>(U);
    if (!UI || UI->getParent() != BB)
      continue;
    if (isa<LoadInst>(UI) || isa<StoreInst>(UI))
      return true;
  }
  return false;
}

/// Check if a GEP's pointer operand is valid for hoisting:
/// - Must be an instruction produced in Top block
/// - Must NOT be a PHI node
bool hasValidHoistableBase(GetElementPtrInst *GEP, BasicBlock *Top) {
  Value *PtrOp = GEP->getPointerOperand();
  Instruction *PtrInst = dyn_cast<Instruction>(PtrOp);

  // Not an instruction
  if (!PtrInst)
    return false;
  // Not defined in Top
  if (PtrInst->getParent() != Top)
    return false;
  // PHI is input, not produced
  if (isa<PHINode>(PtrInst))
    return false;

  return true;
}

/// Check if hoisting would interfere with post-increment folding.
/// Returns true if the pointer is NOT used by memory ops in Bottom.
bool canHoistInterfereWithPostIncFolding(GetElementPtrInst *GEP,
                                         BasicBlock *Bottom,
                                         BasicBlock *Inner) {
  Value *PtrOp = GEP->getPointerOperand();
  Instruction *PtrInst = cast<Instruction>(PtrOp);

  // Check for memory uses in Bottom
  if (hasMemoryUseInBlock(PtrInst, Bottom))
    return false;

  // Check for uses in Inner loop (would extend live range)
  for (const User *U : PtrInst->users()) {
    if (const Instruction *UI = dyn_cast<Instruction>(U)) {
      if (UI->getParent() == Inner)
        return false;
    }
  }
  return true;
}

/// Check if all GEP indices are available at the end of Top block.
bool areIndicesAvailableInTop(GetElementPtrInst *GEP, BasicBlock *Top,
                              DominatorTree *DT) {
  for (unsigned OpIdx = 1; OpIdx < GEP->getNumOperands(); ++OpIdx) {
    const Value *Idx = GEP->getOperand(OpIdx);
    const Instruction *IdxInst = dyn_cast<Instruction>(Idx);
    if (IdxInst && !DT->dominates(IdxInst, Top->getTerminator()))
      return false;
  }
  return true;
}

/// Check if a GEP can be hoisted from Bottom to Top block.
bool canHoistGEP(GetElementPtrInst *GEP, BasicBlock *Top, BasicBlock *Inner,
                 BasicBlock *Bottom, DominatorTree *DT) {
  // Condition 1: Valid hoistable base
  if (!hasValidHoistableBase(GEP, Top))
    return false;

  // Condition 2: No memory interference
  if (!canHoistInterfereWithPostIncFolding(GEP, Bottom, Inner))
    return false;

  // Condition 3: Indices available
  if (!areIndicesAvailableInTop(GEP, Top, DT))
    return false;

  return true;
}

//===----------------------------------------------------------------------===//
// GEP Address Space Canonicalization Helpers
//===----------------------------------------------------------------------===//

/// Rebuild a GEP to use a new base pointer (possibly in a different
/// AddressSpace). Returns the new GEP value, preserving the original's inbounds
/// flag.
Value *rebuildGEPWithNewBase(GetElementPtrInst *GEP, Value *NewBase) {
  IRBuilder<> Builder(GEP);
  SmallVector<Value *, 4> Indices(GEP->indices());
  Value *NewGEP = Builder.CreateGEP(GEP->getSourceElementType(), NewBase,
                                    Indices, GEP->getName());
  if (GetElementPtrInst *NewGEPInst = dyn_cast<GetElementPtrInst>(NewGEP))
    NewGEPInst->setIsInBounds(GEP->isInBounds());
  return NewGEP;
}

/// Insert an addrspacecast at point-of-use, replacing the old value with cast.
Value *insertCastAtUse(Instruction *User, unsigned OperandIdx, Value *NewVal,
                       unsigned TargetAS) {
  IRBuilder<> Builder(User);
  PointerType *TargetPtrTy = PointerType::get(User->getContext(), TargetAS);
  Value *Cast = Builder.CreateAddrSpaceCast(NewVal, TargetPtrTy);
  User->setOperand(OperandIdx, Cast);
  return Cast;
}

/// Find all addrspacecasts from a PHI that cast away from its canonical AS.
/// The casts must be located in the specified BB (which may differ from PHI's
/// block).
SmallVector<AddrSpaceCastInst *, 4> findRootCastsFromPHI(PHINode &PHI,
                                                         BasicBlock *CastBB) {
  SmallVector<AddrSpaceCastInst *, 4> RootCasts;
  const unsigned CanonicalAS = PHI.getType()->getPointerAddressSpace();

  for (User *U : PHI.users()) {
    AddrSpaceCastInst *ASC = dyn_cast<AddrSpaceCastInst>(U);
    if (!ASC || ASC->getParent() != CastBB)
      continue;
    // Must be casting away from canonical AS
    if (ASC->getSrcAddressSpace() != CanonicalAS)
      continue;
    if (ASC->getDestAddressSpace() == CanonicalAS)
      continue;
    RootCasts.push_back(ASC);
  }
  return RootCasts;
}

/// Collect all GEPs that directly use a given value in the specified block.
SmallVector<GetElementPtrInst *, 8> collectGEPsUsingValue(Value *V,
                                                          BasicBlock *BB) {
  SmallVector<GetElementPtrInst *, 8> GEPs;
  for (User *U : V->users()) {
    GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(U);
    if (GEP && GEP->getParent() == BB)
      GEPs.push_back(GEP);
  }
  return GEPs;
}

/// State used during GEP address space canonicalization for a single root cast.
struct GEPCanonState {
  BasicBlock *BB;
  DenseMap<Value *, Value *> ReplacementMap;
  SmallVector<Instruction *, 16> ToErase;
  SmallVector<GetElementPtrInst *, 8> GEPWorklist;

  void clear() {
    ReplacementMap.clear();
    ToErase.clear();
    GEPWorklist.clear();
  }
};

/// Process a single GEP user - handles three cases:
/// 1. Round-trip cast back to canonical AS -> eliminate
/// 2. Chained GEP -> add to worklist
/// 3. Other instruction -> insert cast at point-of-use
bool processGEPUser(User *GEPUser, GetElementPtrInst *OldGEP, Value *NewGEP,
                    unsigned TargetAS, unsigned CanonicalAS,
                    GEPCanonState &State) {
  bool Modified = false;

  // Case 1: Round-trip cast back to canonical AS -> eliminate
  if (AddrSpaceCastInst *ASC = dyn_cast<AddrSpaceCastInst>(GEPUser)) {
    if (ASC->getDestAddressSpace() == CanonicalAS) {
      LLVM_DEBUG(dbgs() << "OLPO:     Eliminating round-trip cast: " << *ASC
                        << "\n");
      ASC->replaceAllUsesWith(NewGEP);
      State.ToErase.push_back(ASC);
      Modified = true;
    }
    return Modified;
  }

  // Case 2: Chained GEP -> add to worklist for processing
  if (GetElementPtrInst *ChildGEP = dyn_cast<GetElementPtrInst>(GEPUser)) {
    if (ChildGEP->getParent() == State.BB)
      State.GEPWorklist.push_back(ChildGEP);
    return Modified;
  }

  // Case 3: Other users (loads, stores, etc.) -> insert cast at point-of-use
  Instruction *UI = dyn_cast<Instruction>(GEPUser);
  if (!UI || UI->getParent() != State.BB)
    return Modified;

  for (unsigned OpIdx = 0; OpIdx < UI->getNumOperands(); ++OpIdx) {
    if (UI->getOperand(OpIdx) != OldGEP)
      continue;
    insertCastAtUse(UI, OpIdx, NewGEP, TargetAS);
    LLVM_DEBUG(dbgs() << "OLPO:     Inserted cast at use for operand " << OpIdx
                      << "\n");
    Modified = true;
  }
  return Modified;
}

/// Process a single GEP - rebuild it in canonical AS and handle users.
bool processGEP(GetElementPtrInst *GEP, unsigned TargetAS, unsigned CanonicalAS,
                GEPCanonState &State) {
  Value *OldBase = GEP->getPointerOperand();

  // Get the new base pointer from replacement map
  auto It = State.ReplacementMap.find(OldBase);
  if (It == State.ReplacementMap.end()) {
    LLVM_DEBUG(dbgs() << "OLPO:     Skip GEP (base not in map): " << *GEP
                      << "\n");
    return false;
  }
  Value *NewBase = It->second;

  // Rebuild GEP in canonical address space
  Value *NewGEP = rebuildGEPWithNewBase(GEP, NewBase);
  LLVM_DEBUG(dbgs() << "OLPO:     Rebuilt GEP in canonical AS: " << *NewGEP
                    << "\n");
  State.ReplacementMap[GEP] = NewGEP;

  // Process all users of the old GEP
  bool Modified = false;
  SmallVector<User *, 8> Users(GEP->users());
  for (User *GEPUser : Users)
    Modified |=
        processGEPUser(GEPUser, GEP, NewGEP, TargetAS, CanonicalAS, State);

  // Mark old GEP for erasure (will be cleaned up if unused)
  State.ToErase.push_back(GEP);
  Modified = true;

  return Modified;
}

/// Process a root cast from a PHI (cast away from canonical AS).
bool processRootCast(PHINode &PHI, AddrSpaceCastInst *RootCast) {
  const unsigned CanonicalAS = PHI.getType()->getPointerAddressSpace();
  const unsigned TargetAS = RootCast->getDestAddressSpace();
  BasicBlock *const BB = RootCast->getParent();

  LLVM_DEBUG(dbgs() << "OLPO:   Found cast away from canonical: " << *RootCast
                    << "\n"
                    << "         Target AS: " << TargetAS << "\n");

  // Initialize state for this root cast
  GEPCanonState State;
  State.BB = BB;
  State.ReplacementMap[RootCast] = &PHI;

  // Seed worklist with GEPs directly using the root cast
  State.GEPWorklist = collectGEPsUsingValue(RootCast, BB);

  // Process GEP chain via worklist
  bool Modified = false;
  while (!State.GEPWorklist.empty()) {
    GetElementPtrInst *GEP = State.GEPWorklist.pop_back_val();
    Modified |= processGEP(GEP, TargetAS, CanonicalAS, State);
  }

  // Cleanup: erase dead instructions in forward order
  // (casts are added before GEPs, so erasing casts first makes GEPs unused)
  for (Instruction *I : State.ToErase) {
    if (I->use_empty())
      I->eraseFromParent();
  }

  // Erase root cast if now unused
  if (RootCast->use_empty()) {
    RootCast->eraseFromParent();
    Modified = true;
  }

  return Modified;
}

//===----------------------------------------------------------------------===//
// LoopStructure
//===----------------------------------------------------------------------===//

/// Represents the structure of a target loop:
///   preheader -> top (prologue) -> inner (single block) -> bottom (epilogue)
///
/// Detects loops with a single-block inner loop nested inside an outer loop,
/// where the outer loop header serves as prologue and the latch as epilogue.
class LoopStructure {
  // Outer loop header (prologue)
  BasicBlock *Top = nullptr;
  // Single-block inner loop
  BasicBlock *InnerHeader = nullptr;
  // Outer loop latch (epilogue)
  BasicBlock *Bottom = nullptr;
  Loop *OuterLoop = nullptr;
  Loop *InnerLoop = nullptr;

public:
  /// Try to build a LoopStructure from the given outer loop.
  /// Returns std::nullopt if the loop doesn't match the expected pattern.
  static std::optional<LoopStructure> tryBuildFrom(Loop *L, LoopInfo &LI);

  BasicBlock *getTop() const { return Top; }
  BasicBlock *getInner() const { return InnerHeader; }
  BasicBlock *getBottom() const { return Bottom; }
  BasicBlock *getPreheader() const {
    return OuterLoop ? OuterLoop->getLoopPreheader() : nullptr;
  }
  Loop *getOuterLoop() const { return OuterLoop; }
  Loop *getInnerLoop() const { return InnerLoop; }
};

std::optional<LoopStructure> LoopStructure::tryBuildFrom(Loop *L,
                                                         LoopInfo &LI) {
  LoopStructure LS;
  LS.OuterLoop = L;

  // Check for single subloop
  auto &SubLoops = L->getSubLoops();
  if (SubLoops.size() != 1) {
    LLVM_DEBUG(dbgs() << "OLPO: Outer loop doesn't have exactly one subloop\n");
    return std::nullopt;
  }
  LS.InnerLoop = SubLoops.front();

  // Check outer loop has a single latch
  BasicBlock *const Latch = L->getLoopLatch();
  if (!Latch) {
    LLVM_DEBUG(dbgs() << "OLPO: Outer loop doesn't have a single latch\n");
    return std::nullopt;
  }

  // Check outer loop has a preheader
  if (!L->getLoopPreheader()) {
    LLVM_DEBUG(dbgs() << "OLPO: Outer loop doesn't have a preheader\n");
    return std::nullopt;
  }

  // Inner loop must be a single block
  if (LS.InnerLoop->getNumBlocks() != 1) {
    LLVM_DEBUG(dbgs() << "OLPO: Inner loop is not a single block\n");
    return std::nullopt;
  }

  LS.InnerHeader = LS.InnerLoop->getHeader();

  // Inner loop must have a single exit block
  BasicBlock *const InnerExit = LS.InnerLoop->getExitBlock();
  if (!InnerExit) {
    LLVM_DEBUG(dbgs() << "OLPO: Inner loop doesn't have a single exit block\n");
    return std::nullopt;
  }

  // Inner loop must have a preheader
  BasicBlock *const InnerPreheader = LS.InnerLoop->getLoopPreheader();
  if (!InnerPreheader) {
    LLVM_DEBUG(dbgs() << "OLPO: Inner loop doesn't have a preheader\n");
    return std::nullopt;
  }

  // Top = inner preheader (prologue)
  LS.Top = InnerPreheader;

  // Bottom = inner exit (epilogue)
  LS.Bottom = InnerExit;

  // Verify top is the outer loop header
  if (LS.Top != L->getHeader()) {
    LLVM_DEBUG(
        dbgs() << "OLPO: Inner preheader is not the outer loop header\n");
    return std::nullopt;
  }

  // Verify bottom is the outer loop latch
  if (LS.Bottom != Latch) {
    LLVM_DEBUG(dbgs() << "OLPO: Inner exit is not the outer loop latch\n");
    return std::nullopt;
  }

  LLVM_DEBUG(dbgs() << "OLPO: Found valid loop structure:\n"
                    << "  Top (prologue): " << LS.Top->getName() << "\n"
                    << "  Inner: " << LS.InnerHeader->getName() << "\n"
                    << "  Bottom (epilogue): " << LS.Bottom->getName() << "\n");

  return LS;
}

class AIEOuterLoopPointerOptimizer : public FunctionPass {
public:
  static char ID;
  AIEOuterLoopPointerOptimizer() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addPreserved<LoopInfoWrapperPass>();
    AU.addPreserved<DominatorTreeWrapperPass>();
  }

  StringRef getPassName() const override {
    return "AIE Outer Loop Pointer Optimizer";
  }

private:
  LoopInfo *LI = nullptr;
  DominatorTree *DT = nullptr;
  const DataLayout *DL = nullptr;

  bool runOnLoop(Loop *L);
  bool tryOptimizeLoop(LoopStructure &LS);
  bool canonicalizeGEPs(LoopStructure &LS);
  bool canonicalizeGEPsInBlock(
      BasicBlock *BB, LoopStructure &LS,
      DenseMap<std::pair<Value *, uint64_t>, Value *> &PreheaderMuls);
  bool linkGEPChains(LoopStructure &LS);
  bool hoistGEPsToTop(LoopStructure &LS);
  bool canonicalizeGEPAddressSpace(LoopStructure &LS);
};

} // end anonymous namespace

char AIEOuterLoopPointerOptimizer::ID = 0;

char &llvm::AIEOuterLoopPointerOptimizerID = AIEOuterLoopPointerOptimizer::ID;

INITIALIZE_PASS_BEGIN(AIEOuterLoopPointerOptimizer, DEBUG_TYPE,
                      "AIE Outer Loop Pointer Optimizer", false, false)
INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass)
INITIALIZE_PASS_DEPENDENCY(DominatorTreeWrapperPass)
INITIALIZE_PASS_END(AIEOuterLoopPointerOptimizer, DEBUG_TYPE,
                    "AIE Outer Loop Pointer Optimizer", false, false)

bool AIEOuterLoopPointerOptimizer::runOnFunction(Function &F) {
  if (!EnableOuterLoopPointerOpt)
    return false;

  LLVM_DEBUG(dbgs() << "OLPO: Running on function " << F.getName() << "\n");

  LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
  DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
  DL = &F.getDataLayout();

  bool Changed = false;

  // Process all loops via depth-first traversal starting from top-level loops
  SmallVector<Loop *, 8> Worklist;
  for (Loop *L : *LI)
    Worklist.push_back(L);

  while (!Worklist.empty()) {
    Loop *L = Worklist.pop_back_val();
    // Add subloops to worklist
    for (Loop *SubL : *L)
      Worklist.push_back(SubL);

    // Try to optimize this loop
    Changed |= runOnLoop(L);
  }

  return Changed;
}

bool AIEOuterLoopPointerOptimizer::runOnLoop(Loop *L) {
  LLVM_DEBUG(dbgs() << "OLPO: Analyzing loop with header "
                    << L->getHeader()->getName() << "\n");

  // Try to build the loop structure
  std::optional<LoopStructure> LS = LoopStructure::tryBuildFrom(L, *LI);
  if (!LS) {
    LLVM_DEBUG(dbgs() << "OLPO: Loop doesn't match target structure\n");
    return false;
  }

  return tryOptimizeLoop(*LS);
}

bool AIEOuterLoopPointerOptimizer::tryOptimizeLoop(LoopStructure &LS) {
  LLVM_DEBUG(dbgs() << "OLPO: Attempting optimization on loop with header "
                    << LS.getOuterLoop()->getHeader()->getName() << "\n");

  bool Changed = false;

  // Optimization 0: Canonicalize GEP address spaces to PHI's address space
  // (run first to enable other optimizations)
  if (EnableGEPAddressSpaceCanon)
    Changed |= canonicalizeGEPAddressSpace(LS);

  // Optimization 1: Canonicalize GEPs to i8-based
  if (EnableGEPCanonicalization)
    Changed |= canonicalizeGEPs(LS);

  // Optimization 2: Link GEP chains for post-increment addressing
  if (EnableGEPChainLinking)
    Changed |= linkGEPChains(LS);

  // Optimization 3: Hoist GEPs from bottom to top
  if (EnableGEPHoisting)
    Changed |= hoistGEPsToTop(LS);

  return Changed;
}

/// Canonicalize non-i8 GEPs to i8-based GEPs.
/// This converts GEPs like:
///   getelementptr <32 x bfloat>, ptr %p, i20 %idx
/// To:
///   %byte_offset = mul i20 %idx, 64  ; 64 = sizeof(<32 x bfloat>)
///   getelementptr i8, ptr %p, i20 %byte_offset
bool AIEOuterLoopPointerOptimizer::canonicalizeGEPs(LoopStructure &LS) {
  LLVM_DEBUG(dbgs() << "OLPO: Canonicalizing GEPs in top and bottom blocks\n");

  // Map from (index_value, element_size) to the mul instruction for CSE
  // This is used for loop-invariant indices that can be hoisted to preheader
  DenseMap<std::pair<Value *, uint64_t>, Value *> PreheaderMuls;

  bool Changed = false;

  // Process top (prologue) block
  Changed |= canonicalizeGEPsInBlock(LS.getTop(), LS, PreheaderMuls);

  // Process bottom (epilogue) block
  Changed |= canonicalizeGEPsInBlock(LS.getBottom(), LS, PreheaderMuls);

  return Changed;
}

bool AIEOuterLoopPointerOptimizer::canonicalizeGEPsInBlock(
    BasicBlock *BB, LoopStructure &LS,
    DenseMap<std::pair<Value *, uint64_t>, Value *> &PreheaderMuls) {

  // Local CSE map for non-loop-invariant indices within this block
  DenseMap<std::pair<Value *, uint64_t>, Value *> LocalMuls;

  // Collect GEPs that need canonicalization
  SmallVector<GetElementPtrInst *, 16> GEPsToProcess;
  for (Instruction &I : *BB) {
    GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(&I);
    if (GEP && needsI8Canonicalization(GEP))
      GEPsToProcess.push_back(GEP);
  }

  bool Changed = false;
  Loop *const OuterLoop = LS.getOuterLoop();
  BasicBlock *const Preheader = LS.getPreheader();

  for (GetElementPtrInst *GEP : GEPsToProcess) {
    Value *const Index = GEP->getOperand(1);
    Type *const SrcElemTy = GEP->getSourceElementType();
    const uint64_t ElemSize = DL->getTypeAllocSize(SrcElemTy);

    // Compute byte offset (returns Index unchanged if ElemSize == 1)
    Value *ByteOffset = getByteOffset(Index, ElemSize, GEP, OuterLoop,
                                      Preheader, PreheaderMuls, LocalMuls);

    // Create new i8-based GEP
    Value *NewGEP = buildNewGEPWithI8Based(GEP, ByteOffset);

    LLVM_DEBUG(dbgs() << "OLPO:   Replaced: " << *GEP << "\n"
                      << "OLPO:   With:     " << *NewGEP << "\n");

    GEP->replaceAllUsesWith(NewGEP);
    GEP->eraseFromParent();
    Changed = true;
  }

  return Changed;
}

/// Link GEP chains to enable post-increment addressing.
/// This creates complete chains of GEPs that share the same base pointer.
/// Each GEP in the chain uses the previous GEP as its base, enabling
/// post-increment addressing patterns.
///
/// Example:
///   ; Before (all use same base):
///   %ptr64 = getelementptr i8, ptr %base, i20 64
///   %ptr128 = getelementptr i8, ptr %base, i20 128
///   %ptr192 = getelementptr i8, ptr %base, i20 192
///
///   ; After (complete chain):
///   %ptr64 = getelementptr i8, ptr %base, i20 64     ; first uses base
///   %ptr128 = getelementptr i8, ptr %ptr64, i20 64   ; uses previous
///   %ptr192 = getelementptr i8, ptr %ptr128, i20 64  ; uses previous
///
/// This enables post-increment loads: load ptr, ptr += offset
bool AIEOuterLoopPointerOptimizer::linkGEPChains(LoopStructure &LS) {
  LLVM_DEBUG(dbgs() << "OLPO: Linking GEP chains for post-increment\n");

  bool Changed = false;
  BasicBlock *const Top = LS.getTop();
  BasicBlock *const Bottom = LS.getBottom();

  // Track chain state per base pointer
  DenseMap<Value *, GEPChainState> BaseChains;

  // Track GEPs to erase after processing.
  SmallPtrSet<GetElementPtrInst *, 8> ToErase;

  // Collect GEPs in program order from Top and Bottom
  SmallVector<GetElementPtrInst *, 16> AllGEPs = collectGEPs(Top);
  AllGEPs.append(collectGEPs(Bottom));

  // Process GEPs in order
  for (GetElementPtrInst *GEP : AllGEPs) {
    // Skip if already marked for erasure
    if (ToErase.contains(GEP))
      continue;

    // Check if this GEP is a candidate for chain linking
    int64_t CurrentOffset;
    if (!isChainLinkCandidate(GEP, CurrentOffset)) {
      LLVM_DEBUG(dbgs() << "OLPO:   Skip (not a chain candidate): " << *GEP
                        << "\n");
      continue;
    }

    Value *const Base = GEP->getPointerOperand();

    // Look up the chain state for this base
    auto It = BaseChains.find(Base);
    if (It == BaseChains.end()) {
      // Start a new chain from:
      // - PHI in Top: loop-carried pointer from outer loop header
      // - Argument: reading/writing scalar parameters, chaining leads to
      //   small encodable offsets
      PHINode *BasePHI = dyn_cast<PHINode>(Base);
      const bool IsValidPHI = BasePHI && BasePHI->getParent() == Top;
      if (IsValidPHI || isa<Argument>(Base)) {
        BaseChains[Base] = {GEP, CurrentOffset};
        LLVM_DEBUG(dbgs() << "OLPO:   Start chain for base: " << *GEP << "\n");
      }
      continue;
    }

    // Try to link to the existing chain
    GEPChainState &State = It->second;
    GetElementPtrInst *NewGEP = tryLinkToChain(GEP, State, CurrentOffset, DT);

    if (!NewGEP) {
      LLVM_DEBUG(dbgs() << "OLPO:   Skip (cannot link to chain): " << *GEP
                        << "\n");
      continue;
    }

    LLVM_DEBUG(dbgs() << "OLPO:   Linked GEP to chain:\n"
                      << "         Prev: " << *State.LastGEP << " (offset "
                      << State.LastOffset << ")\n"
                      << "         Curr: " << *GEP << " (offset "
                      << CurrentOffset << ")\n"
                      << "         New:  " << *NewGEP << "\n");

    GEP->replaceAllUsesWith(NewGEP);

    // Update chain state: the new GEP becomes the last in chain
    State.LastGEP = NewGEP;
    State.LastOffset = CurrentOffset;

    ToErase.insert(GEP);
    Changed = true;
  }

  // Erase replaced GEPs
  for (GetElementPtrInst *GEP : ToErase)
    GEP->eraseFromParent();

  return Changed;
}

/// Hoist GEPs from bottom block to top block.
///
/// Conditions for hoisting a GEP:
/// 1. The GEP's pointer operand must be an instruction PRODUCED in Top
///    (not a phi node, not an argument - must be a real computation in Top)
/// 2. That operand must NOT be used by any memory operation (load/store)
///    in Bottom - to preserve post-increment folding opportunities
///
/// Example:
///   top:
///     %add.ptr = getelementptr i8, ptr %base, i20 64   ; PRODUCED in Top
///     ...
///     br label %inner
///   inner:
///     ; ...
///     br label %bottom
///   bottom:
///     %next = getelementptr i8, ptr %add.ptr, i20 128  ; candidate
///     ; IF %add.ptr has no memory use in bottom → HOIST
///
/// After:
///   top:
///     %add.ptr = getelementptr i8, ptr %base, i20 64
///     %next = getelementptr i8, ptr %add.ptr, i20 128  ; MOVED here
///     ...
bool AIEOuterLoopPointerOptimizer::hoistGEPsToTop(LoopStructure &LS) {
  LLVM_DEBUG(dbgs() << "OLPO: Hoisting GEPs from bottom to top\n");

  bool Changed = false;
  BasicBlock *const Top = LS.getTop();
  BasicBlock *const Bottom = LS.getBottom();
  BasicBlock *const Inner = LS.getInner();

  // Collect GEPs from Bottom that can be hoisted
  SmallVector<GetElementPtrInst *, 8> GEPsToHoist;

  for (Instruction &I : *Bottom) {
    GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(&I);
    if (!GEP)
      continue;

    // Use helper to check all hoisting conditions
    if (!canHoistGEP(GEP, Top, Inner, Bottom, DT)) {
      LLVM_DEBUG(dbgs() << "OLPO:   Skip (cannot hoist): " << *GEP << "\n");
      continue;
    }

    LLVM_DEBUG(dbgs() << "OLPO:   Can hoist GEP: " << *GEP << "\n");
    GEPsToHoist.push_back(GEP);
  }

  // Move each hoistable GEP to just before the terminator of Top
  const BasicBlock::iterator InsertPoint = Top->getTerminator()->getIterator();

  for (GetElementPtrInst *GEP : GEPsToHoist) {
    LLVM_DEBUG(dbgs() << "OLPO:   Hoisting to top: " << *GEP << "\n");
    GEP->moveBefore(InsertPoint);
    Changed = true;
  }

  return Changed;
}

/// Canonicalize GEP address spaces to the PHI's canonical address space.
///
/// This optimization keeps GEPs in the PHI's address space and moves
/// addrspacecast instructions to point-of-use. It handles two patterns:
///
/// Pattern 1 (round-trip cast - eliminated):
///   %phi = phi ptr addrspace(5) ...
///   %cast = addrspacecast ptr addrspace(5) %phi to ptr addrspace(7)
///   %gep = getelementptr i8, ptr addrspace(7) %cast, i20 128
///   %cast_back = addrspacecast ptr addrspace(7) %gep to ptr addrspace(5)
///   use ptr addrspace(5) %cast_back
/// Becomes:
///   %phi = phi ptr addrspace(5) ...
///   %gep = getelementptr i8, ptr addrspace(5) %phi, i20 128
///   use ptr addrspace(5) %gep
///
/// Pattern 2 (single cast - moved to point-of-use):
///   %phi = phi ptr addrspace(5) ...
///   %cast = addrspacecast ptr addrspace(5) %phi to ptr addrspace(7)
///   %gep = getelementptr i8, ptr addrspace(7) %cast, i20 128
///   store <32 x i16> %val, ptr addrspace(7) %gep
/// Becomes:
///   %phi = phi ptr addrspace(5) ...
///   %gep = getelementptr i8, ptr addrspace(5) %phi, i20 128
///   %cast = addrspacecast ptr addrspace(5) %gep to ptr addrspace(7)
///   store <32 x i16> %val, ptr addrspace(7) %cast
///
/// This enables better GEP chain optimization by keeping GEPs in a
/// consistent (PHI-defined) address space.
bool AIEOuterLoopPointerOptimizer::canonicalizeGEPAddressSpace(
    LoopStructure &LS) {
  LLVM_DEBUG(dbgs() << "OLPO: Canonicalizing GEP address spaces to PHI's AS\n");

  BasicBlock *const Top = LS.getTop();
  BasicBlock *const Bottom = LS.getBottom();

  bool Changed = false;

  // Single pass over PHIs in Top, processing casts in both Top and Bottom
  for (PHINode &PHI : Top->phis()) {
    if (!PHI.getType()->isPointerTy())
      continue;

    LLVM_DEBUG(dbgs() << "OLPO:   Processing PHI: " << PHI << "\n"
                      << "         Canonical AS: "
                      << PHI.getType()->getPointerAddressSpace() << "\n");

    // Process casts in Top block
    SmallVector<AddrSpaceCastInst *, 4> TopCasts =
        findRootCastsFromPHI(PHI, Top);
    for (AddrSpaceCastInst *RootCast : TopCasts)
      Changed |= processRootCast(PHI, RootCast);

    // Process casts in Bottom block
    SmallVector<AddrSpaceCastInst *, 4> BottomCasts =
        findRootCastsFromPHI(PHI, Bottom);
    for (AddrSpaceCastInst *RootCast : BottomCasts)
      Changed |= processRootCast(PHI, RootCast);
  }

  return Changed;
}

namespace llvm {
FunctionPass *createAIEOuterLoopPointerOptimizerPass() {
  return new AIEOuterLoopPointerOptimizer();
}
} // namespace llvm
