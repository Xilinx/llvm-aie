//===-- AIELoopPointerOptimizer.cpp - Pointer chain optimization -----===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This pass optimizes pointer chains in nested and standalone loops.
// Two loop shapes are supported:
//
// 1. Nested loop shape (outer + single-block inner):
//      preheader -> Top (prologue) -> Inner (single block) -> Bottom (epilogue)
//
// 2. Standalone loop shape (leaf loop, no subloops):
//      preheader -> Top (header) [-> Bottom (latch)]
//    where Top == Bottom for single-block loops.
//
// All leaf (standalone) loops are processed before their enclosing nested
// loops, ensuring inner-loop pointer patterns are resolved first.
//
// Optimizations (in execution order):
//
//   0. GEP Address Space Canonicalization [both shapes]
//      Move addrspacecast instructions from before GEPs to point-of-use,
//      keeping GEPs in the PHI's canonical address space.  This enables
//      uniform chain detection across the subsequent passes.
//
//   1. GEP Canonicalization [both shapes]
//      Convert non-i8 GEPs to i8-based GEPs so that all pointer arithmetic
//      is expressed as explicit byte offsets.
//
//   2a. Inner-phi Back-edge Folding -- first call [nested only]
//      Fold redundant epilogue GEPs that duplicate the inner loop's exit
//      pointer.  Two sub-phases run within a single function call:
//
//      Phase 1 -- single-stride match:
//        gep(inner_phi, stride)  in Bottom  =>  replaced by back-edge GEP.
//
//      Phase 2 -- full-trip match (requires constant trip count N from
//        @llvm.set.loop.iterations in Top):
//        gep(init, N*stride)  in Bottom  =>  replaced by back-edge GEP.
//
//      Running Phase 2 before chain-linking handles GEPs whose base is
//      directly the Top-incoming value of an inner PHI.
//
//   2b. GEP Chain Linking [both shapes]
//      Rewrite consecutive GEPs that share the same base pointer into a
//      chain where each GEP uses the previous GEP as its base, enabling
//      post-increment addressing (e.g. "load ptr; ptr += stride").
//      linkGEPChains may create new chained GEPs in Bottom whose base is a
//      chained GEP produced in Top; these are handled by pass 2c below.
//
//   2c. Inner-phi Back-edge Folding -- second call [nested only]
//      Re-invoke foldInnerPhiBackEdgeGEPs after chain-linking so that Phase 2
//      can fold the newly-created chained epilogue GEPs (e.g.
//      "%.chained3 = gep %.chained, N*stride" where %.chained is the
//      Top-incoming of an inner PHI).  Phase 1 is idempotent in this call.
//
//   3. GEP Hoisting [both shapes]
//      Move GEPs from the Bottom block to the Top block when safe, reducing
//      live ranges in the epilogue/latch and improving scheduling.
//
//   4. PHI Normalization to Load-base Form [standalone only]
//      Detect pre-increment pointer PHIs (the PHI is advanced before any
//      memory access) and shift the initial value forward by one stride so
//      that the PHI lands directly at the first load address.
//
//   5. Post-increment GEP Chain Building [standalone only]
//      Reposition the post-increment GEP to appear immediately after the
//      last memory user of its base, creating (load, ptr+=delta) adjacency
//      that the backend can select as a single post-increment instruction.
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

#define DEBUG_TYPE "aie-loop-pointer-optimizer"

namespace {

cl::opt<bool> EnableLoopPointerOpt("aie-enable-loop-pointer-opt",
                                   cl::desc("Enable loop pointer optimization"),
                                   cl::init(true), cl::Hidden);

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

cl::opt<bool> EnableInnerPhiBackEdgeFolding(
    "aie-enable-inner-phi-backedge-folding",
    cl::desc("Fold redundant GEPs in epilogue that duplicate the inner loop's "
             "back-edge GEP (nested loops only)"),
    cl::init(true), cl::Hidden);

cl::opt<bool> EnableGEPAddressSpaceCanon(
    "aie-enable-gep-addrspace-canon",
    cl::desc(
        "Enable GEP address space canonicalization to PHI's address space"),
    cl::init(true), cl::Hidden);

cl::opt<bool> EnablePhiNormalization(
    "aie-enable-phi-normalization",
    cl::desc("Enable pre-increment phi normalization to load-base form "
             "(standalone loops only)"),
    cl::init(true), cl::Hidden);

cl::opt<bool> EnablePostIncChain(
    "aie-enable-post-inc-chain",
    cl::desc("Enable post-increment GEP chain building for direct phi uses "
             "(standalone loops only)"),
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
              Loop *TheLoop, BasicBlock *Preheader,
              DenseMap<std::pair<Value *, uint64_t>, Value *> &PreheaderMuls,
              DenseMap<std::pair<Value *, uint64_t>, Value *> &LocalMuls) {
  // No scaling needed for byte-sized elements
  if (ElemSize == 1)
    return Index;

  const auto Key = std::make_pair(Index, ElemSize);
  const bool IsLoopInvariant = TheLoop->isLoopInvariant(Index);

  // Select map and insertion point based on loop invariance
  DenseMap<std::pair<Value *, uint64_t>, Value *> &MulMap =
      IsLoopInvariant ? PreheaderMuls : LocalMuls;
  Instruction *const InsertPt =
      IsLoopInvariant ? Preheader->getTerminator() : InsertBefore;

  // CSE lookup
  auto It = MulMap.find(Key);
  if (It != MulMap.end()) {
    LLVM_DEBUG(dbgs() << "LPO:   Reusing mul for index " << *Index << " * "
                      << ElemSize << "\n");
    return It->second;
  }

  // Create mul at insertion point
  IRBuilder<> Builder(InsertPt);
  Value *Scale = ConstantInt::get(Index->getType(), ElemSize);
  Value *ByteOffset = Builder.CreateMul(Index, Scale, "byte_offset");
  MulMap[Key] = ByteOffset;
  LLVM_DEBUG(dbgs() << "LPO:   Created mul: " << *ByteOffset << "\n");
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
/// Returns true if hoisting is safe (no interference detected).
/// Inner may be nullptr for standalone loops (no inner loop exists).
bool canHoistWithoutInterference(GetElementPtrInst *GEP, BasicBlock *Bottom,
                                 BasicBlock *Inner) {
  Value *PtrOp = GEP->getPointerOperand();
  Instruction *PtrInst = cast<Instruction>(PtrOp);

  // Check for memory uses in Bottom
  if (hasMemoryUseInBlock(PtrInst, Bottom))
    return false;

  // Check for uses in Inner loop (would extend live range).
  // Only applicable when an inner loop exists (nested loop shape).
  if (Inner) {
    for (const User *U : PtrInst->users()) {
      if (const Instruction *UI = dyn_cast<Instruction>(U)) {
        if (UI->getParent() == Inner)
          return false;
      }
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
/// Inner may be nullptr for standalone loops.
bool canHoistGEP(GetElementPtrInst *GEP, BasicBlock *Top, BasicBlock *Inner,
                 BasicBlock *Bottom, DominatorTree *DT) {
  // Condition 1: Valid hoistable base
  if (!hasValidHoistableBase(GEP, Top))
    return false;

  // Condition 2: No memory interference
  if (!canHoistWithoutInterference(GEP, Bottom, Inner))
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
      LLVM_DEBUG(dbgs() << "LPO:     Eliminating round-trip cast: " << *ASC
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
    LLVM_DEBUG(dbgs() << "LPO:     Inserted cast at use for operand " << OpIdx
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
    LLVM_DEBUG(dbgs() << "LPO:     Skip GEP (base not in map): " << *GEP
                      << "\n");
    return false;
  }
  Value *NewBase = It->second;

  // Rebuild GEP in canonical address space
  Value *NewGEP = rebuildGEPWithNewBase(GEP, NewBase);
  LLVM_DEBUG(dbgs() << "LPO:     Rebuilt GEP in canonical AS: " << *NewGEP
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

  LLVM_DEBUG(dbgs() << "LPO:   Found cast away from canonical: " << *RootCast
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

/// Represents the structure of a target loop. Two shapes are supported:
///
/// Nested shape:
///   preheader -> Top (prologue) -> Inner (single block) -> Bottom (epilogue)
///   InnerLoop != nullptr, InnerHeader != nullptr, Top != Bottom.
///
/// Standalone shape (leaf loop, no subloops):
///   preheader -> Top (header) [-> Bottom (latch)]
///   InnerLoop == nullptr, InnerHeader == nullptr.
///   Top == Bottom for single-block loops (header is also the latch).
class LoopStructure {
  // Top block: loop header (nested) or loop header (standalone)
  BasicBlock *Top = nullptr;
  // Single-block inner loop (nested shape only, nullptr for standalone)
  BasicBlock *InnerHeader = nullptr;
  // Bottom block: loop latch (nested) or loop latch (standalone)
  BasicBlock *Bottom = nullptr;
  Loop *TheLoop = nullptr;
  Loop *InnerLoop = nullptr;
  // Constant inner loop trip count, if discoverable from
  // @llvm.set.loop.iterations in the Top (inner preheader) block.
  std::optional<uint64_t> InnerTripCount;

public:
  /// Try to build a LoopStructure for a nested loop (outer + single-block
  /// inner). Returns std::nullopt if the loop doesn't match.
  static std::optional<LoopStructure> tryBuildFrom(Loop *L, LoopInfo &LI);

  /// Try to build a LoopStructure for a standalone (leaf) loop with no
  /// subloops. Returns std::nullopt if the loop doesn't match.
  static std::optional<LoopStructure> tryBuildFromStandalone(Loop *L,
                                                             LoopInfo &LI);

  BasicBlock *getTop() const { return Top; }
  BasicBlock *getInner() const { return InnerHeader; }
  BasicBlock *getBottom() const { return Bottom; }
  BasicBlock *getPreheader() const {
    return TheLoop ? TheLoop->getLoopPreheader() : nullptr;
  }
  Loop *getLoop() const { return TheLoop; }
  Loop *getInnerLoop() const { return InnerLoop; }

  /// Returns the constant inner loop trip count if it could be determined
  /// from an @llvm.set.loop.iterations call in Top, std::nullopt otherwise.
  std::optional<uint64_t> getInnerTripCount() const { return InnerTripCount; }

  /// Returns true for standalone loops (no inner loop).
  bool isStandalone() const { return InnerLoop == nullptr; }

  /// Returns true when the loop body is a single block (Top == Bottom).
  /// Only meaningful for standalone loops.
  bool isSingleBlock() const { return Top == Bottom; }
};

std::optional<LoopStructure> LoopStructure::tryBuildFrom(Loop *L,
                                                         LoopInfo &LI) {
  LoopStructure LS;
  LS.TheLoop = L;

  // Check for single subloop
  auto &SubLoops = L->getSubLoops();
  if (SubLoops.size() != 1) {
    LLVM_DEBUG(dbgs() << "LPO: Loop doesn't have exactly one subloop\n");
    return std::nullopt;
  }
  LS.InnerLoop = SubLoops.front();

  // Check loop has a single latch
  BasicBlock *const Latch = L->getLoopLatch();
  if (!Latch) {
    LLVM_DEBUG(dbgs() << "LPO: Loop doesn't have a single latch\n");
    return std::nullopt;
  }

  // Check loop has a preheader
  if (!L->getLoopPreheader()) {
    LLVM_DEBUG(dbgs() << "LPO: Loop doesn't have a preheader\n");
    return std::nullopt;
  }

  // Inner loop must be a single block
  if (LS.InnerLoop->getNumBlocks() != 1) {
    LLVM_DEBUG(dbgs() << "LPO: Inner loop is not a single block\n");
    return std::nullopt;
  }

  LS.InnerHeader = LS.InnerLoop->getHeader();

  // Inner loop must have a single exit block
  BasicBlock *const InnerExit = LS.InnerLoop->getExitBlock();
  if (!InnerExit) {
    LLVM_DEBUG(dbgs() << "LPO: Inner loop doesn't have a single exit block\n");
    return std::nullopt;
  }

  // Inner loop must have a preheader
  BasicBlock *const InnerPreheader = LS.InnerLoop->getLoopPreheader();
  if (!InnerPreheader) {
    LLVM_DEBUG(dbgs() << "LPO: Inner loop doesn't have a preheader\n");
    return std::nullopt;
  }

  // Top = inner preheader (prologue)
  LS.Top = InnerPreheader;

  // Bottom = inner exit (epilogue)
  LS.Bottom = InnerExit;

  // Verify top is the loop header
  if (LS.Top != L->getHeader()) {
    LLVM_DEBUG(dbgs() << "LPO: Inner preheader is not the loop header\n");
    return std::nullopt;
  }

  // Verify bottom is the loop latch
  if (LS.Bottom != Latch) {
    LLVM_DEBUG(dbgs() << "LPO: Inner exit is not the loop latch\n");
    return std::nullopt;
  }

  // Try to extract the inner loop trip count from @llvm.set.loop.iterations*
  // called in the Top (inner preheader) block.
  for (Instruction &I : *LS.Top) {
    auto *CB = dyn_cast<CallBase>(&I);
    if (!CB || !CB->getCalledFunction())
      continue;
    if (!CB->getCalledFunction()->getName().starts_with(
            "llvm.set.loop.iterations"))
      continue;
    if (auto *C = dyn_cast<ConstantInt>(CB->getArgOperand(0))) {
      LS.InnerTripCount = C->getZExtValue();
      LLVM_DEBUG(dbgs() << "LPO:   Found inner trip count: "
                        << *LS.InnerTripCount << "\n");
    }
    break;
  }

  LLVM_DEBUG(dbgs() << "LPO: Found valid nested loop structure:\n"
                    << "  Top (prologue): " << LS.Top->getName() << "\n"
                    << "  Inner: " << LS.InnerHeader->getName() << "\n"
                    << "  Bottom (epilogue): " << LS.Bottom->getName() << "\n");

  return LS;
}

std::optional<LoopStructure>
LoopStructure::tryBuildFromStandalone(Loop *L, LoopInfo &LI) {
  LoopStructure LS;
  LS.TheLoop = L;
  LS.InnerLoop = nullptr;
  LS.InnerHeader = nullptr;

  // Must have no subloops (leaf loop)
  if (!L->getSubLoops().empty()) {
    LLVM_DEBUG(dbgs() << "LPO: Loop has subloops, not standalone\n");
    return std::nullopt;
  }

  // Must have a preheader
  if (!L->getLoopPreheader()) {
    LLVM_DEBUG(dbgs() << "LPO: Standalone loop doesn't have a preheader\n");
    return std::nullopt;
  }

  // Must have a single latch
  BasicBlock *const Latch = L->getLoopLatch();
  if (!Latch) {
    LLVM_DEBUG(dbgs() << "LPO: Standalone loop doesn't have a single latch\n");
    return std::nullopt;
  }

  LS.Top = L->getHeader();
  LS.Bottom = Latch;

  LLVM_DEBUG(dbgs() << "LPO: Found valid standalone loop structure:\n"
                    << "  Top (header): " << LS.Top->getName() << "\n"
                    << "  Bottom (latch): " << LS.Bottom->getName() << "\n"
                    << "  Single-block: " << (LS.isSingleBlock() ? "yes" : "no")
                    << "\n");

  return LS;
}

class AIELoopPointerOptimizer : public FunctionPass {
public:
  static char ID;
  AIELoopPointerOptimizer() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addPreserved<LoopInfoWrapperPass>();
    AU.addPreserved<DominatorTreeWrapperPass>();
  }

  StringRef getPassName() const override {
    return "AIE Loop Pointer Optimizer";
  }

private:
  LoopInfo *LI = nullptr;
  DominatorTree *DT = nullptr;
  const DataLayout *DL = nullptr;

  bool tryOptimizeLoop(LoopStructure &LS);
  bool canonicalizeGEPs(LoopStructure &LS);
  bool canonicalizeGEPsInBlock(
      BasicBlock *BB, LoopStructure &LS,
      DenseMap<std::pair<Value *, uint64_t>, Value *> &PreheaderMuls);
  bool foldInnerPhiBackEdgeGEPs(LoopStructure &LS);
  bool linkGEPChains(LoopStructure &LS);
  bool hoistGEPsToTop(LoopStructure &LS);
  bool canonicalizeGEPAddressSpace(LoopStructure &LS);
  bool normalizePhiToLoadBase(LoopStructure &LS);
  bool buildPostIncChain(LoopStructure &LS);
};

} // end anonymous namespace

char AIELoopPointerOptimizer::ID = 0;

char &llvm::AIELoopPointerOptimizerID = AIELoopPointerOptimizer::ID;

INITIALIZE_PASS_BEGIN(AIELoopPointerOptimizer, DEBUG_TYPE,
                      "AIE Loop Pointer Optimizer", false, false)
INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass)
INITIALIZE_PASS_DEPENDENCY(DominatorTreeWrapperPass)
INITIALIZE_PASS_END(AIELoopPointerOptimizer, DEBUG_TYPE,
                    "AIE Loop Pointer Optimizer", false, false)

bool AIELoopPointerOptimizer::runOnFunction(Function &F) {
  if (!EnableLoopPointerOpt)
    return false;

  LLVM_DEBUG(dbgs() << "LPO: Running on function " << F.getName() << "\n");

  LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
  DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
  DL = &F.getDataLayout();

  // Collect all loops in DFS order (outer-to-inner).
  SmallVector<Loop *, 8> AllLoops;
  {
    SmallVector<Loop *, 8> Stack;
    for (Loop *L : *LI)
      Stack.push_back(L);
    while (!Stack.empty()) {
      Loop *L = Stack.pop_back_val();
      for (Loop *SubL : *L)
        Stack.push_back(SubL);
      AllLoops.push_back(L);
    }
  }

  bool Changed = false;

  // Phase 1: Standalone (non-nested / leaf) loops.
  // Processing these first ensures inner loops are optimized before the
  // loop optimizer (Phase 2) sees the enclosing nested structure.
  LLVM_DEBUG(dbgs() << "LPO: Phase 1 - Standalone (leaf) loops\n");
  for (Loop *L : AllLoops) {
    if (!L->getSubLoops().empty())
      continue; // Handled in Phase 2
    LLVM_DEBUG(dbgs() << "LPO: Analyzing standalone loop with header "
                      << L->getHeader()->getName() << "\n");
    if (auto LS = LoopStructure::tryBuildFromStandalone(L, *LI))
      Changed |= tryOptimizeLoop(*LS);
  }

  // Phase 2: Nested loops (loop enclosing a single-block inner loop).
  LLVM_DEBUG(dbgs() << "LPO: Phase 2 - Nested loops\n");
  for (Loop *L : AllLoops) {
    LLVM_DEBUG(dbgs() << "LPO: Analyzing loop with header "
                      << L->getHeader()->getName() << "\n");
    if (auto LS = LoopStructure::tryBuildFrom(L, *LI))
      Changed |= tryOptimizeLoop(*LS);
  }

  return Changed;
}

bool AIELoopPointerOptimizer::tryOptimizeLoop(LoopStructure &LS) {
  LLVM_DEBUG(dbgs() << "LPO: Attempting optimization on loop with header "
                    << LS.getLoop()->getHeader()->getName() << "\n");

  bool Changed = false;

  // Optimization 0: Canonicalize GEP address spaces to PHI's address space
  // (run first to enable other optimizations)
  if (EnableGEPAddressSpaceCanon)
    Changed |= canonicalizeGEPAddressSpace(LS);

  // Optimization 1: Canonicalize GEPs to i8-based
  if (EnableGEPCanonicalization)
    Changed |= canonicalizeGEPs(LS);

  // Optimization 2a: Fold redundant inner-phi GEPs in epilogue (nested only).
  // Phase 1 (gep(inner_phi, stride) -> back-edge GEP) must run first so that
  // linkGEPChains can root chains at the back-edge GEP instead of a redundant
  // duplicate. Phase 2 (full-trip match using InnerTripCount) also runs here
  // for GEPs whose base is already the direct Top-incoming of an Inner PHI.
  if (!LS.isStandalone() && EnableInnerPhiBackEdgeFolding)
    Changed |= foldInnerPhiBackEdgeGEPs(LS);

  // Optimization 2b: Link GEP chains for post-increment addressing.
  // linkGEPChains may create new chained GEPs in Bottom whose base is a
  // chained GEP in Top (e.g., %.chained3 = gep %.chained, N*stride).
  // The Inner PHI for these chained GEPs will now reference the chained base
  // (e.g., phi [%.chained, Top] [%76, Inner]), so Phase 2 of
  // foldInnerPhiBackEdgeGEPs can match them.
  if (EnableGEPChainLinking)
    Changed |= linkGEPChains(LS);

  // Optimization 2c: Re-run Phase 2 of inner-phi back-edge folding to catch
  // chained GEPs created by linkGEPChains above (e.g., the pattern
  // %.chained3 = gep %.chained, N*stride where %.chained is the Top-incoming
  // of an Inner PHI). Phase 1 is idempotent here (all simple-stride GEPs were
  // already folded in 2a).
  if (!LS.isStandalone() && EnableInnerPhiBackEdgeFolding)
    Changed |= foldInnerPhiBackEdgeGEPs(LS);

  // Optimization 3: Hoist GEPs from bottom to top
  if (EnableGEPHoisting)
    Changed |= hoistGEPsToTop(LS);

  // Optimizations 4 & 5: Post-increment chain building (standalone only).
  // These two passes convert pre-increment phi patterns (Pattern 1) into
  // direct-phi-use patterns (Pattern 2) and then reposition GEPs after loads
  // to enable post-increment load instruction selection.
  if (LS.isStandalone()) {
    if (EnablePhiNormalization)
      Changed |= normalizePhiToLoadBase(LS);
    if (EnablePostIncChain)
      Changed |= buildPostIncChain(LS);
  }

  return Changed;
}

/// Canonicalize non-i8 GEPs to i8-based GEPs.
/// This converts GEPs like:
///   getelementptr <32 x bfloat>, ptr %p, i20 %idx
/// To:
///   %byte_offset = mul i20 %idx, 64  ; 64 = sizeof(<32 x bfloat>)
///   getelementptr i8, ptr %p, i20 %byte_offset
bool AIELoopPointerOptimizer::canonicalizeGEPs(LoopStructure &LS) {
  LLVM_DEBUG(dbgs() << "LPO: Canonicalizing GEPs in top and bottom blocks\n");

  // Map from (index_value, element_size) to the mul instruction for CSE.
  // Used for loop-invariant indices that can be hoisted to preheader.
  DenseMap<std::pair<Value *, uint64_t>, Value *> PreheaderMuls;

  bool Changed = false;

  // Process top block
  Changed |= canonicalizeGEPsInBlock(LS.getTop(), LS, PreheaderMuls);

  // Process bottom block only if distinct from top (avoid double-processing
  // single-block standalone loops where top == bottom).
  if (LS.getBottom() != LS.getTop())
    Changed |= canonicalizeGEPsInBlock(LS.getBottom(), LS, PreheaderMuls);

  return Changed;
}

bool AIELoopPointerOptimizer::canonicalizeGEPsInBlock(
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
  Loop *const TheLoop = LS.getLoop();
  BasicBlock *const Preheader = LS.getPreheader();

  for (GetElementPtrInst *GEP : GEPsToProcess) {
    Value *const Index = GEP->getOperand(1);
    Type *const SrcElemTy = GEP->getSourceElementType();
    const uint64_t ElemSize = DL->getTypeAllocSize(SrcElemTy);

    // Compute byte offset (returns Index unchanged if ElemSize == 1)
    Value *ByteOffset = getByteOffset(Index, ElemSize, GEP, TheLoop, Preheader,
                                      PreheaderMuls, LocalMuls);

    // Create new i8-based GEP
    Value *NewGEP = buildNewGEPWithI8Based(GEP, ByteOffset);

    LLVM_DEBUG(dbgs() << "LPO:   Replaced: " << *GEP << "\n"
                      << "LPO:   With:     " << *NewGEP << "\n");

    GEP->replaceAllUsesWith(NewGEP);
    GEP->eraseFromParent();
    Changed = true;
  }

  return Changed;
}

/// Fold GEPs in the epilogue (Bottom block) that duplicate the inner loop's
/// back-edge GEP, replacing them with the back-edge GEP directly.
///
/// **Phase 1 -- chain-total match** (base = inner PHI, offset = sum of chain):
///
///   The inner loop may advance the pointer through a chain of GEPs rather
///   than a single GEP.  Phase 1 walks backward from the back-edge GEP to
///   the inner PHI, accumulates the total byte offset, and folds any epilogue
///   GEP whose offset equals that total:
///
///   inner:
///     %phi   = phi ptr [%init, Top] [%back2, inner]
///     %back1 = getelementptr i8, ptr %phi,  i20 <d1>       ; step 1
///     %back2 = getelementptr i8, ptr %back1, i20 <d2>      ; step 2
///     (back-edge)
///
///   epilogue:
///     %dup = getelementptr i8, ptr %phi, i20 <d1+d2>       ; same as %back2!
///
///   -> Replace %dup with %back2 (the inner loop's last-iteration back-edge).
///
///   The single-GEP case (chain length 1, d1 == stride) is a special case.
///
/// **Phase 2 -- full-trip match** (base = init value, offset = N x stride):
///
///   When the inner loop trip count N is known (from @llvm.set.loop.iterations
///   in Top), the pointer after N iterations equals init + N x stride.
///   Any epilogue GEP with that exact (base, offset) pair is therefore the
///   same as the back-edge GEP's final value:
///
///   epilogue:
///     %full = getelementptr i8, ptr %init, i20 <N * stride>  ; = %back_last!
///
///   -> Replace %full with %back.
///
/// Both phases allow the backend to use the register that naturally holds the
/// inner loop's exit pointer, eliminating standalone pointer-advance
/// (paddb/padda) instructions from the epilogue.
bool AIELoopPointerOptimizer::foldInnerPhiBackEdgeGEPs(LoopStructure &LS) {
  LLVM_DEBUG(dbgs() << "LPO: Folding inner-phi back-edge GEPs in epilogue\n");

  BasicBlock *const Inner = LS.getInner();
  BasicBlock *const Bottom = LS.getBottom();
  BasicBlock *const Top = LS.getTop();
  bool Changed = false;

  // Collect GEPs in Bottom (snapshot to avoid iterator invalidation).
  SmallVector<GetElementPtrInst *, 8> BottomGEPs = collectGEPs(Bottom);

  // -----------------------------------------------------------------------
  // Phase 1: Fold gep(inner_phi, stride) -> BackGEP.
  // -----------------------------------------------------------------------
  for (GetElementPtrInst *GEP : BottomGEPs) {
    int64_t Offset = 0;
    if (!isChainLinkCandidate(GEP, Offset))
      continue;

    PHINode *BasePHI = dyn_cast<PHINode>(GEP->getPointerOperand());
    if (!BasePHI || BasePHI->getParent() != Inner)
      continue;

    Value *BackEdgeVal = BasePHI->getIncomingValueForBlock(Inner);
    auto *BackGEP = dyn_cast<GetElementPtrInst>(BackEdgeVal);
    if (!BackGEP)
      continue;

    // Walk backward from BackGEP through the inner-loop GEP chain to
    // BasePHI, accumulating the total offset.  This handles both the
    // single-step case (BackGEP directly off BasePHI) and multi-step chains.
    int64_t TotalOffset = 0;
    GetElementPtrInst *Cur = BackGEP;
    bool ReachesPHI = false;
    while (Cur && Cur->getParent() == Inner) {
      int64_t Delta = 0;
      if (!isChainLinkCandidate(Cur, Delta))
        break;
      TotalOffset += Delta;
      // All deltas are positive; once we exceed Offset the total can only
      // grow further, so stop early.
      if (TotalOffset > Offset)
        break;
      Value *Base = Cur->getPointerOperand();
      if (Base == BasePHI) {
        ReachesPHI = true;
        break;
      }
      Cur = dyn_cast<GetElementPtrInst>(Base);
    }

    if (!ReachesPHI || TotalOffset != Offset)
      continue;

    // Dominance is guaranteed by the loop structure: Inner always dominates
    // Bottom (the epilogue is only reachable through the inner loop's exit),
    // so BackGEP (defined in Inner) always dominates GEP (in Bottom).

    LLVM_DEBUG(dbgs() << "LPO:   [Ph1] Folding epilogue GEP:\n"
                      << "         Redundant:    " << *GEP << "\n"
                      << "         Back-edge:    " << *BackGEP << "\n"
                      << "         TotalOffset:  " << TotalOffset << "\n");

    GEP->replaceAllUsesWith(BackGEP);
    GEP->eraseFromParent();
    Changed = true;
  }

  // -----------------------------------------------------------------------
  // Phase 2: Fold gep(init_val, Nxstride) -> BackGEP, using the inner loop
  // trip count N stored in LoopStructure (from @llvm.set.loop.iterations).
  // -----------------------------------------------------------------------
  std::optional<uint64_t> MaybeTripCount = LS.getInnerTripCount();
  if (!MaybeTripCount) {
    LLVM_DEBUG(dbgs() << "LPO:   [Ph2] No trip count available, skipping\n");
    return Changed;
  }
  const uint64_t N = *MaybeTripCount;
  LLVM_DEBUG(dbgs() << "LPO:   [Ph2] Inner trip count = " << N << "\n");

  // Re-collect Bottom GEPs (Phase 1 may have erased some).
  BottomGEPs = collectGEPs(Bottom);

  // For each epilogue GEP G = gep(Base, Offset), search Base's users for an
  // Inner PHI of the form  phi [Base, Top] [BackGEP, Inner]  with
  //   BackGEP = gep phi_inner, Stride   and   Offset == N * Stride.
  // Searching through users of Base (rather than scanning Inner->phis())
  // avoids fragility against value renumbering introduced by Phase 1.
  for (GetElementPtrInst *GEP : BottomGEPs) {
    int64_t Offset = 0;
    if (!isChainLinkCandidate(GEP, Offset)) {
      LLVM_DEBUG(dbgs() << "LPO:   [Ph2] Skip GEP (not candidate): " << *GEP
                        << "\n");
      continue;
    }
    LLVM_DEBUG(dbgs() << "LPO:   [Ph2] Checking GEP offset=" << Offset << ": "
                      << *GEP << "\n");

    Value *Base = GEP->getPointerOperand();

    // Walk users of Base looking for an Inner PHI whose Top-incoming is Base.
    for (User *U : Base->users()) {
      auto *InnerPHI = dyn_cast<PHINode>(U);
      if (!InnerPHI) {
        LLVM_DEBUG(dbgs() << "LPO:   [Ph2]   user not PHI: " << *U << "\n");
        continue;
      }
      LLVM_DEBUG(dbgs() << "LPO:   [Ph2]   PHI user: " << *InnerPHI
                        << " in block " << InnerPHI->getParent()->getName()
                        << " Inner=" << Inner->getName() << "\n");
      if (InnerPHI->getParent() != Inner)
        continue;
      Value *IncomingFromTop = InnerPHI->getIncomingValueForBlock(Top);
      LLVM_DEBUG(
          dbgs() << "LPO:   [Ph2]   incomingFromTop="
                 << (IncomingFromTop ? IncomingFromTop->getName() : "null")
                 << " Base=" << Base->getName() << "\n");
      if (IncomingFromTop != Base)
        continue;

      Value *BackEdgeVal = InnerPHI->getIncomingValueForBlock(Inner);
      auto *BackGEP = dyn_cast<GetElementPtrInst>(BackEdgeVal);
      if (!BackGEP) {
        LLVM_DEBUG(dbgs() << "LPO:   [Ph2]   back-edge not GEP\n");
        continue;
      }

      int64_t Stride = 0;
      if (!isChainLinkCandidate(BackGEP, Stride)) {
        LLVM_DEBUG(dbgs() << "LPO:   [Ph2]   BackGEP not candidate: "
                          << *BackGEP << "\n");
        continue;
      }
      if (BackGEP->getPointerOperand() != InnerPHI) {
        LLVM_DEBUG(dbgs() << "LPO:   [Ph2]   BackGEP base != InnerPHI\n");
        continue;
      }

      const int64_t FullOffset = static_cast<int64_t>(N) * Stride;
      LLVM_DEBUG(dbgs() << "LPO:   [Ph2]   Offset=" << Offset
                        << " FullOffset=" << FullOffset << " (N=" << N
                        << " stride=" << Stride << ")\n");
      if (Offset != FullOffset)
        continue;

      // Dominance is guaranteed by the loop structure: Inner always dominates
      // Bottom, so BackGEP (defined in Inner) always dominates GEP (in Bottom).

      LLVM_DEBUG(dbgs() << "LPO:   [Ph2] Folding full-trip epilogue GEP:\n"
                        << "         Redundant: " << *GEP << "\n"
                        << "         Inner phi: " << *InnerPHI << "\n"
                        << "         Back-edge: " << *BackGEP << " (N=" << N
                        << " stride=" << Stride << ")\n");

      GEP->replaceAllUsesWith(BackGEP);
      GEP->eraseFromParent();
      Changed = true;
      break; // GEP is gone; move to next
    }
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
bool AIELoopPointerOptimizer::linkGEPChains(LoopStructure &LS) {
  LLVM_DEBUG(dbgs() << "LPO: Linking GEP chains for post-increment\n");

  bool Changed = false;
  BasicBlock *const Top = LS.getTop();
  BasicBlock *const Bottom = LS.getBottom();

  // Track chain state per base pointer
  DenseMap<Value *, GEPChainState> BaseChains;

  // Track GEPs to erase after processing.
  SmallPtrSet<GetElementPtrInst *, 8> ToErase;

  // Collect GEPs in program order from Top and Bottom.
  // When Top == Bottom (single-block standalone loop), only collect once.
  SmallVector<GetElementPtrInst *, 16> AllGEPs = collectGEPs(Top);
  if (Bottom != Top)
    AllGEPs.append(collectGEPs(Bottom));

  // Process GEPs in order
  for (GetElementPtrInst *GEP : AllGEPs) {
    // Skip if already marked for erasure
    if (ToErase.contains(GEP))
      continue;

    // Check if this GEP is a candidate for chain linking
    int64_t CurrentOffset;
    if (!isChainLinkCandidate(GEP, CurrentOffset)) {
      LLVM_DEBUG(dbgs() << "LPO:   Skip (not a chain candidate): " << *GEP
                        << "\n");
      continue;
    }

    Value *const Base = GEP->getPointerOperand();

    // Look up the chain state for this base
    auto It = BaseChains.find(Base);
    if (It == BaseChains.end()) {
      // Start a new chain from:
      // - PHI in Top: loop-carried pointer from loop header
      // - Argument: reading/writing scalar parameters, chaining leads to
      //   small encodable offsets
      PHINode *BasePHI = dyn_cast<PHINode>(Base);
      const bool IsValidPHI = BasePHI && BasePHI->getParent() == Top;
      if (IsValidPHI || isa<Argument>(Base)) {
        // Skip chain if any external user of the base PHI is a GEP: linking
        // GEPs inside the loop when an external GEP also uses the PHI would
        // leave that external GEP as a standalone computation, creating an
        // extra pointer-copy instruction and interfering with the global
        // combiner.  Non-GEP external uses (loads, stores, etc.) do not
        // cause this problem and are allowed.
        if (IsValidPHI) {
          Loop *const TheLoop = LS.getLoop();
          const bool HasExternalGEPUser =
              llvm::any_of(BasePHI->users(), [&](const User *U) {
                const auto *UI = dyn_cast<GetElementPtrInst>(U);
                return UI && !TheLoop->contains(UI->getParent());
              });
          if (HasExternalGEPUser) {
            LLVM_DEBUG(dbgs()
                       << "LPO:   Skip chain (PHI has external GEP user): "
                       << *BasePHI << "\n");
            continue;
          }
        }
        BaseChains[Base] = {GEP, CurrentOffset};
        LLVM_DEBUG(dbgs() << "LPO:   Start chain for base: " << *GEP << "\n");
        continue;
      }

      // For GEPs in Bottom whose base is an Inner-loop PHI: seed the chain
      // from the back-edge GEP (which foldInnerPhiBackEdgeGEPs may have
      // already used to replace the first epilogue GEP with the same offset).
      // This allows subsequent epilogue GEPs (phi+2*stride, phi+3*stride) to
      // chain off the back-edge GEP rather than starting fresh from phi.
      BasicBlock *Inner = LS.getInner();
      if (Inner && BasePHI && BasePHI->getParent() == Inner &&
          GEP->getParent() == Bottom) {
        Value *BackVal = BasePHI->getIncomingValueForBlock(Inner);
        if (auto *BackGEP = dyn_cast<GetElementPtrInst>(BackVal)) {
          int64_t BackOffset = 0;
          if (isChainLinkCandidate(BackGEP, BackOffset) &&
              BackGEP->getPointerOperand() == BasePHI &&
              CurrentOffset > BackOffset && DT->dominates(BackGEP, GEP)) {
            // Seed chain at BackGEP so this GEP (with larger offset) links off.
            BaseChains[Base] = {BackGEP, BackOffset};
            It = BaseChains.find(Base); // re-acquire iterator
            LLVM_DEBUG(dbgs()
                       << "LPO:   Seed inner-phi chain from back-edge: "
                       << *BackGEP << " (offset " << BackOffset << ")\n");
            // Fall through to chain linking below.
          }
        }
      }
      // For GEPs in Bottom whose base is an Inner-block GEP that is itself
      // the back-edge of an Inner PHI: seed the chain from that back-edge GEP.
      // This handles GEPs such as %.chained5 = gep %76, 128 that appear after
      // foldInnerPhiBackEdgeGEPs replaces gep(init, N*stride) with %76.
      if (Inner && !BasePHI && GEP->getParent() == Bottom) {
        if (auto *BaseGEP = dyn_cast<GetElementPtrInst>(Base)) {
          if (BaseGEP->getParent() == Inner) {
            if (auto *InnerPHI =
                    dyn_cast<PHINode>(BaseGEP->getPointerOperand())) {
              if (InnerPHI->getParent() == Inner &&
                  InnerPHI->getIncomingValueForBlock(Inner) == BaseGEP &&
                  DT->dominates(BaseGEP, GEP)) {
                BaseChains[Base] = {GEP, CurrentOffset};
                LLVM_DEBUG(dbgs()
                           << "LPO:   Start chain from inner back-edge GEP: "
                           << *GEP << "\n");
                continue;
              }
            }
          }
        }
      }
      if (It == BaseChains.end())
        continue;
    }

    // Try to link to the existing chain
    GEPChainState &State = It->second;
    GetElementPtrInst *NewGEP = tryLinkToChain(GEP, State, CurrentOffset, DT);

    if (!NewGEP) {
      LLVM_DEBUG(dbgs() << "LPO:   Skip (cannot link to chain): " << *GEP
                        << "\n");
      continue;
    }

    LLVM_DEBUG(dbgs() << "LPO:   Linked GEP to chain:\n"
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
/// For standalone loops:
/// - When Top == Bottom (single-block): nothing to hoist, skip.
/// - When Top != Bottom (multi-block): same conditions apply; the inner-loop
///   interference check is skipped since there is no inner loop.
bool AIELoopPointerOptimizer::hoistGEPsToTop(LoopStructure &LS) {
  LLVM_DEBUG(dbgs() << "LPO: Hoisting GEPs from bottom to top\n");

  BasicBlock *const Top = LS.getTop();
  BasicBlock *const Bottom = LS.getBottom();

  // Nothing to hoist when top and bottom are the same block (single-block
  // standalone loop).
  if (Top == Bottom) {
    LLVM_DEBUG(dbgs() << "LPO:   Skip hoisting: top == bottom\n");
    return false;
  }

  bool Changed = false;
  BasicBlock *const Inner = LS.getInner(); // nullptr for standalone loops

  // Collect GEPs from Bottom that can be hoisted
  SmallVector<GetElementPtrInst *, 8> GEPsToHoist;

  for (Instruction &I : *Bottom) {
    GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(&I);
    if (!GEP)
      continue;

    // Use helper to check all hoisting conditions
    if (!canHoistGEP(GEP, Top, Inner, Bottom, DT)) {
      LLVM_DEBUG(dbgs() << "LPO:   Skip (cannot hoist): " << *GEP << "\n");
      continue;
    }

    LLVM_DEBUG(dbgs() << "LPO:   Can hoist GEP: " << *GEP << "\n");
    GEPsToHoist.push_back(GEP);
  }

  // Move each hoistable GEP to just before the terminator of Top
  const BasicBlock::iterator InsertPoint = Top->getTerminator()->getIterator();

  for (GetElementPtrInst *GEP : GEPsToHoist) {
    LLVM_DEBUG(dbgs() << "LPO:   Hoisting to top: " << *GEP << "\n");
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
bool AIELoopPointerOptimizer::canonicalizeGEPAddressSpace(LoopStructure &LS) {
  LLVM_DEBUG(dbgs() << "LPO: Canonicalizing GEP address spaces to PHI's AS\n");

  BasicBlock *const Top = LS.getTop();
  BasicBlock *const Bottom = LS.getBottom();

  bool Changed = false;

  // Single pass over PHIs in Top, processing casts in both Top and Bottom.
  // For standalone loops the loop PHIs live in Top (the header), so this
  // correctly finds all loop-carried pointer PHIs.
  for (PHINode &PHI : Top->phis()) {
    if (!PHI.getType()->isPointerTy())
      continue;

    LLVM_DEBUG(dbgs() << "LPO:   Processing PHI: " << PHI << "\n"
                      << "         Canonical AS: "
                      << PHI.getType()->getPointerAddressSpace() << "\n");

    // Process casts in Top block
    SmallVector<AddrSpaceCastInst *, 4> TopCasts =
        findRootCastsFromPHI(PHI, Top);
    for (AddrSpaceCastInst *RootCast : TopCasts)
      Changed |= processRootCast(PHI, RootCast);

    // Process casts in Bottom block only if distinct from Top (avoid
    // double-processing single-block standalone loops where top == bottom).
    if (Bottom != Top) {
      SmallVector<AddrSpaceCastInst *, 4> BottomCasts =
          findRootCastsFromPHI(PHI, Bottom);
      for (AddrSpaceCastInst *RootCast : BottomCasts)
        Changed |= processRootCast(PHI, RootCast);
    }
  }

  return Changed;
}

//===----------------------------------------------------------------------===//
// Standalone-loop Post-Increment Helpers
//===----------------------------------------------------------------------===//

/// Collect loads/stores that directly use V, or that use an addrspacecast of V.
/// This covers both bare-pointer and cast-pointer memory patterns.
SmallVector<Instruction *, 4> collectMemUsers(Value *V) {
  SmallVector<Instruction *, 4> MemUsers;
  for (User *U : V->users()) {
    if (isa<LoadInst>(U) || isa<StoreInst>(U)) {
      MemUsers.push_back(cast<Instruction>(U));
    } else if (auto *ASC = dyn_cast<AddrSpaceCastInst>(U)) {
      for (User *UU : ASC->users())
        if (isa<LoadInst>(UU) || isa<StoreInst>(UU))
          MemUsers.push_back(cast<Instruction>(UU));
    }
  }
  return MemUsers;
}

/// Return the instruction from Insns that appears textually LAST inside BB.
/// Returns nullptr if none of the instructions belong to BB.
Instruction *findLastInBlock(ArrayRef<Instruction *> Insns, BasicBlock *BB) {
  // Build a set for O(1) membership tests, then do a single forward scan.
  const SmallPtrSet<Instruction *, 8> InsnSet(Insns.begin(), Insns.end());
  Instruction *Last = nullptr;
  for (Instruction &I : *BB)
    if (InsnSet.contains(&I))
      Last = &I;
  return Last;
}

/// Normalize a pre-increment pointer PHI to load-base form (standalone loops).
///
/// Detects the pattern where the PHI is pre-incremented before any loads:
///   preheader: init
///   loop header:
///     phi      = [init | BackGEP]       ; carries the PRE-incremented pointer
///     BackGEP  = phi + Stride           ; advance before loads
///     load1    = *BackGEP               ; loads at phi + Stride
///     load2    = *(BackGEP + W)         ; loads at phi + Stride + W
///
/// Transforms to load-base form so phi arrives AT the first load address:
///   preheader: new_init = init + Stride ; one-time shift
///   loop header:
///     phi_new  = [new_init | new_back]  ; phi now points to first load
///     load1    = *phi_new               ; unchanged load address
///     load2    = *(phi_new + W)         ; unchanged load address
///     new_back = phi_new + Stride       ; back-edge, placed at end of latch
///
/// Safety conditions (any violation -- skip the phi):
///   - phi must have no direct load/store/addrcast uses (pre-increment form)
///   - phi must not be live-out of the loop
///   - BackGEP must not be live-out of the loop
bool AIELoopPointerOptimizer::normalizePhiToLoadBase(LoopStructure &LS) {
  LLVM_DEBUG(dbgs() << "LPO: Normalizing pre-increment PHIs to load base\n");

  BasicBlock *const Header = LS.getTop();
  BasicBlock *const Latch = LS.getBottom();
  BasicBlock *const Preheader = LS.getPreheader();
  Loop *const L = LS.getLoop();
  Type *const Int8Ty = Type::getInt8Ty(Header->getContext());
  bool Changed = false;

  // Returns true when V has at least one user outside the loop.
  auto IsLiveOut = [&](Value *V) {
    return llvm::any_of(V->users(), [&](User *U) {
      auto *I = dyn_cast<Instruction>(U);
      return I && !L->contains(I->getParent());
    });
  };

  for (PHINode &Phi : Header->phis()) {
    if (!Phi.getType()->isPointerTy())
      continue;

    // --- Structural check ---
    // Back-edge value must be a constant-stride i8 GEP directly off the phi.
    Value *const BackVal = Phi.getIncomingValueForBlock(Latch);
    auto *BackGEP = dyn_cast<GetElementPtrInst>(BackVal);
    if (!BackGEP || BackGEP->getPointerOperand() != &Phi)
      continue;
    int64_t Stride = 0;
    if (!isChainLinkCandidate(BackGEP, Stride))
      continue;

    // --- Pattern check ---
    // Phi must NOT be used directly as a memory address or addrcast source.
    // Direct uses mean loads already use phi (Pattern 2) -- skip.
    const bool HasDirectMemUse = llvm::any_of(Phi.users(), [](const User *U) {
      return isa<LoadInst>(U) || isa<StoreInst>(U) || isa<AddrSpaceCastInst>(U);
    });
    if (HasDirectMemUse) {
      LLVM_DEBUG(dbgs() << "LPO:   Skip phi (already Pattern 2): " << Phi
                        << "\n");
      continue;
    }

    // --- Safety checks ---
    if (IsLiveOut(&Phi)) {
      LLVM_DEBUG(dbgs() << "LPO:   Skip phi (live-out): " << Phi << "\n");
      continue;
    }
    if (IsLiveOut(BackGEP)) {
      LLVM_DEBUG(dbgs() << "LPO:   Skip phi (back-edge GEP live-out): " << Phi
                        << "\n");
      continue;
    }

    LLVM_DEBUG(dbgs() << "LPO:   Normalizing phi: " << Phi
                      << "\n           stride = " << Stride << "\n");

    // --- Transform ---
    Type *const IdxTy = BackGEP->getOperand(1)->getType();
    Value *const StrideVal = ConstantInt::get(IdxTy, Stride);

    // Insert  new_init = phi_init + Stride  in the preheader (one-time shift).
    Value *const PhiInit = Phi.getIncomingValueForBlock(Preheader);
    IRBuilder<> PreheaderBuilder(Preheader->getTerminator());
    Value *const NewInit = PreheaderBuilder.CreateGEP(
        Int8Ty, PhiInit, StrideVal, Phi.getName() + ".shifted.init");
    Phi.setIncomingValueForBlock(Preheader, NewInit);

    // Phi now lands at the same address BackGEP used to compute.
    // Replace all BackGEP uses with Phi so loads use Phi directly.
    BackGEP->replaceAllUsesWith(&Phi);
    BackGEP->eraseFromParent();

    // Insert the new back-edge advancement at the END of the latch.
    // Placing it last lets buildPostIncChain later reposition it after loads.
    IRBuilder<> LatchBuilder(Latch->getTerminator());
    GetElementPtrInst *const NewBack =
        cast<GetElementPtrInst>(LatchBuilder.CreateGEP(
            Int8Ty, &Phi, StrideVal, Phi.getName() + ".back"));
    NewBack->setIsInBounds(true);
    Phi.setIncomingValueForBlock(Latch, NewBack);

    LLVM_DEBUG(dbgs() << "LPO:   Inserted new back-edge GEP: " << *NewBack
                      << "\n");
    Changed = true;
  }
  return Changed;
}

/// Reposition post-increment GEPs to follow their last memory user
/// (standalone loops, Pattern 2).
///
/// For each pointer PHI that is used directly as a memory address, the
/// post-increment GEP chain is:
///   phi -> GEP1 (phi+d1) -> GEP2 (GEP1+d2) -> ...
///
/// Each GEP is moved to appear immediately after the last load/store that
/// uses the preceding chain node.  This creates the (load, ptr+=delta)
/// adjacency that allows the backend to select a single post-increment load
/// instruction.
///
/// Example (single-level chain):
///   Before:
///     %gep1 = phi + 64      ; placed early by earlier passes or front-end
///     %load1 = load *phi
///     %load2 = load *gep1
///     store  *phi
///
///   After:
///     %load1 = load *phi
///     %load2 = load *gep1
///     store  *phi
///     %gep1 = phi + 64      ; now follows the last mem-user of phi (store)
bool AIELoopPointerOptimizer::buildPostIncChain(LoopStructure &LS) {
  LLVM_DEBUG(dbgs() << "LPO: Building post-increment GEP chains\n");

  BasicBlock *const Header = LS.getTop();
  bool Changed = false;

  for (PHINode &Phi : Header->phis()) {
    if (!Phi.getType()->isPointerTy())
      continue;

    // Only handle PHIs with direct memory uses (Pattern 2).
    SmallVector<Instruction *, 4> ChainNodeMemUsers = collectMemUsers(&Phi);
    if (ChainNodeMemUsers.empty())
      continue;

    LLVM_DEBUG(dbgs() << "LPO:   Processing phi with direct mem uses: " << Phi
                      << "\n");

    // Walk the GEP chain: Phi -> GEP1 -> GEP2 -> ...
    // At each step, move the next GEP to after the last mem-user of the
    // current node, then advance to the next node.
    Value *ChainNode = &Phi;

    while (true) {
      // Find the unique chain-link GEP that advances from the current node.
      // In a well-formed post-increment chain there is at most one such GEP.
      GetElementPtrInst *NextChainGEP = nullptr;
      for (User *U : ChainNode->users()) {
        auto *GEP = dyn_cast<GetElementPtrInst>(U);
        if (!GEP || GEP->getParent() != Header)
          continue;
        int64_t Off = 0;
        if (!isChainLinkCandidate(GEP, Off))
          continue;
        NextChainGEP = GEP;
        break;
      }
      if (!NextChainGEP)
        break;

      // Locate the last instruction that reads/writes through the current node.
      Instruction *const LastMemUser =
          findLastInBlock(ChainNodeMemUsers, Header);
      if (!LastMemUser)
        break;

      // Reposition the GEP to immediately after the last memory user so the
      // backend can see (load, gep) or (store, gep) as an adjacent pair.
      if (NextChainGEP->comesBefore(LastMemUser)) {
        LLVM_DEBUG(dbgs() << "LPO:   Moving GEP after last mem user:\n"
                          << "         GEP:   " << *NextChainGEP << "\n"
                          << "         After: " << *LastMemUser << "\n");
        NextChainGEP->moveAfter(LastMemUser);
        Changed = true;
      }

      // Advance to the next link in the chain.
      ChainNode = NextChainGEP;
      ChainNodeMemUsers = collectMemUsers(NextChainGEP);
    }
  }
  return Changed;
}

namespace llvm {
FunctionPass *createAIELoopPointerOptimizerPass() {
  return new AIELoopPointerOptimizer();
}
} // namespace llvm
