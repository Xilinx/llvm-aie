//===-- AIEInnerLoopPointerOptimizer.cpp - Post-inc pointer opts ----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This pass optimizes pointer chains in standalone (leaf) single-block loops —
// loops with no subloops and whose header and latch are the same block.  It
// enables post-increment load/store instruction selection by transforming
// pre-increment pointer PHIs and repositioning GEPs.
//
// Invariant: the loop body is a single basic block (header == latch).
// This is enforced by InnerLoopStructure::tryBuildFrom.
//
// Optimization 1 — normalizePhiToLoadBase (Pattern 1):
//   Detects pointer PHIs whose back-edge GEP (phi + Stride) is placed at the
//   top of the loop body before any loads.  Transforms the PHI so it arrives
//   at the first load address each iteration:
//
//     Before: phi = [init | phi+S];  preinc = phi+S;  load *preinc
//     After:  phi = [init+S | phi+S];  load *phi
//
//   The one-time init shift is inserted in the preheader.  The new back-edge
//   GEP is placed at the end of the body.  Live-out PHIs and back-edge GEPs
//   are left untouched.
//
// Optimization 2 — buildPostIncChain (Pattern 2):
//   For PHIs already in load-base form (phi used directly as a memory address),
//   repositions each GEP in the post-increment chain to appear immediately
//   after the last load/store that uses the preceding chain node.  This creates
//   adjacent (load, ptr+=delta) pairs for post-increment instruction selection.
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "Utils/AIEIRUtils.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "aie-inner-loop-pointer-optimizer"

namespace {

cl::opt<bool> EnableInnerLoopPointerOpt(
    "aie-enable-inner-loop-pointer-opt",
    cl::desc("Enable inner loop pointer optimization"), cl::init(true),
    cl::Hidden);

cl::opt<bool> EnablePhiNormalization(
    "aie-enable-phi-normalization",
    cl::desc("Enable pre-increment phi normalization to load-base form"),
    cl::init(true), cl::Hidden);

cl::opt<bool> EnablePostIncChain(
    "aie-enable-post-inc-chain",
    cl::desc("Enable post-increment GEP chain building for direct phi uses"),
    cl::init(true), cl::Hidden);

//===----------------------------------------------------------------------===//
// Local helpers
//===----------------------------------------------------------------------===//

/// Returns the constant integer last index of \p GEP, or nullptr if the last
/// index is not a compile-time constant.
ConstantInt *getConstantGEPLastIndex(const GetElementPtrInst *GEP) {
  return dyn_cast<ConstantInt>(GEP->getOperand(GEP->getNumOperands() - 1));
}

/// Returns true if \p V is used as a direct memory address (possibly through
/// a transparent chain of AddrSpaceCasts) without an intervening GEP.
/// This identifies Pattern 2 (load-base form) so that normalizePhiToLoadBase
/// can skip PHIs that are already in that form.
bool hasDirectMemAccess(Value *V) {
  for (User *U : V->users()) {
    if (isa<LoadInst>(U) || isa<StoreInst>(U))
      return true;
    if (isa<AddrSpaceCastInst>(U) && hasDirectMemAccess(U))
      return true;
  }
  return false;
}

/// Recursively validates the use-def tree rooted at \p V and collects all
/// constant-index GEPs (excluding \p ExcludeGEP) into \p SiblingGEPs.
///
/// Each node in the tree must be one of:
///   - GetElementPtrInst with a constant last index (collected, then recurse)
///   - AddrSpaceCastInst (transparent wrapper, recurse into its users)
///   - LoadInst / StoreInst (terminal — OK)
///
/// A GEP that belongs to a different block than \p Body, or has a
/// non-constant last index, causes the function to return false immediately.
/// Any other user type also returns false.
bool collectValidUsers(Value *V, GetElementPtrInst *ExcludeGEP,
                       BasicBlock *Body,
                       SmallVectorImpl<GetElementPtrInst *> &SiblingGEPs) {
  for (User *U : V->users()) {
    if (auto *GEP = dyn_cast<GetElementPtrInst>(U)) {
      if (GEP == ExcludeGEP)
        continue;
      if (GEP->getParent() != Body || !getConstantGEPLastIndex(GEP))
        return false;
      SiblingGEPs.push_back(GEP);
      if (!collectValidUsers(GEP, ExcludeGEP, Body, SiblingGEPs))
        return false;
    } else if (isa<AddrSpaceCastInst>(U)) {
      if (!collectValidUsers(U, ExcludeGEP, Body, SiblingGEPs))
        return false;
    } else if (!isa<LoadInst>(U) && !isa<StoreInst>(U)) {
      return false; // Unrecognized user type.
    }
  }
  return true;
}

/// Returns the unique constant-stride i8 GEP in \p Body that uses \p Node as
/// its pointer operand, or nullptr if no such GEP exists.
GetElementPtrInst *findNextChainGEP(Value *Node, BasicBlock *Body) {
  for (User *U : Node->users()) {
    auto *const GEP = dyn_cast<GetElementPtrInst>(U);
    if (!GEP || GEP->getParent() != Body)
      continue;
    int64_t Off = 0;
    if (AIEIRUtils::isChainLinkCandidate(GEP, Off))
      return GEP;
  }
  return nullptr;
}

//===----------------------------------------------------------------------===//
// InnerLoopStructure
//===----------------------------------------------------------------------===//

/// Represents the structure of a standalone single-block loop:
///   preheader -> Body (header == latch)
///
/// Invariant enforced by tryBuildFrom: the loop has no subloops, has a
/// preheader, and has a single latch that is the same block as the header.
class InnerLoopStructure {
  /// The single loop block (header == latch == body).
  BasicBlock *Body = nullptr;
  Loop *TheLoop = nullptr;

public:
  /// Try to build an InnerLoopStructure from \p L.
  /// Returns std::nullopt if the loop has subloops, no preheader, no single
  /// latch, or the latch is not the same block as the header (not
  /// single-block).
  static std::optional<InnerLoopStructure> tryBuildFrom(Loop *L, LoopInfo &LI);

  BasicBlock *getBody() const { return Body; }
  BasicBlock *getPreheader() const { return TheLoop->getLoopPreheader(); }
  Loop *getLoop() const { return TheLoop; }
};

std::optional<InnerLoopStructure>
InnerLoopStructure::tryBuildFrom(Loop *L, LoopInfo &LI) {
  // Must be a leaf loop (no subloops)
  if (!L->getSubLoops().empty()) {
    LLVM_DEBUG(dbgs() << "ILPO: Loop has subloops, skipping\n");
    return std::nullopt;
  }
  if (!L->getLoopPreheader()) {
    LLVM_DEBUG(dbgs() << "ILPO: Loop has no preheader, skipping\n");
    return std::nullopt;
  }
  BasicBlock *const Latch = L->getLoopLatch();
  if (!Latch) {
    LLVM_DEBUG(dbgs() << "ILPO: Loop has no single latch, skipping\n");
    return std::nullopt;
  }
  // Enforce the single-block invariant: header must be the latch.
  if (Latch != L->getHeader()) {
    LLVM_DEBUG(dbgs() << "ILPO: Loop latch != header (not single-block), "
                         "skipping\n");
    return std::nullopt;
  }

  InnerLoopStructure ILS;
  ILS.TheLoop = L;
  ILS.Body = L->getHeader();

  LLVM_DEBUG(dbgs() << "ILPO: Found single-block standalone loop:\n"
                    << "  Body (header/latch): " << ILS.Body->getName() << "\n"
                    << "  Preheader: " << ILS.getPreheader()->getName()
                    << "\n");
  return ILS;
}

//===----------------------------------------------------------------------===//
// Pass
//===----------------------------------------------------------------------===//

class AIEInnerLoopPointerOptimizer : public FunctionPass {
public:
  static char ID;
  AIEInnerLoopPointerOptimizer() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addPreserved<LoopInfoWrapperPass>();
  }

  StringRef getPassName() const override {
    return "AIE Inner Loop Pointer Optimizer";
  }

private:
  LoopInfo *LI = nullptr;

  bool runOnLoop(Loop *L);
  bool tryOptimizeLoop(InnerLoopStructure &ILS);
  bool normalizePhiToLoadBase(InnerLoopStructure &ILS);
  bool buildPostIncChain(InnerLoopStructure &ILS);
};

} // end anonymous namespace

char AIEInnerLoopPointerOptimizer::ID = 0;
char &llvm::AIEInnerLoopPointerOptimizerID = AIEInnerLoopPointerOptimizer::ID;

INITIALIZE_PASS_BEGIN(AIEInnerLoopPointerOptimizer, DEBUG_TYPE,
                      "AIE Inner Loop Pointer Optimizer", false, false)
INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass)
INITIALIZE_PASS_END(AIEInnerLoopPointerOptimizer, DEBUG_TYPE,
                    "AIE Inner Loop Pointer Optimizer", false, false)

bool AIEInnerLoopPointerOptimizer::runOnFunction(Function &F) {
  if (!EnableInnerLoopPointerOpt)
    return false;
  LLVM_DEBUG(dbgs() << "ILPO: Running on " << F.getName() << "\n");
  LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
  bool Changed = false;
  // Collect all loops and visit leaf loops. tryBuildFrom rejects non-leaf ones.
  SmallVector<Loop *, 8> Worklist;
  for (Loop *L : *LI)
    Worklist.push_back(L);
  while (!Worklist.empty()) {
    Loop *L = Worklist.pop_back_val();
    for (Loop *SubL : *L)
      Worklist.push_back(SubL);
    Changed |= runOnLoop(L);
  }
  return Changed;
}

bool AIEInnerLoopPointerOptimizer::runOnLoop(Loop *L) {
  auto ILS = InnerLoopStructure::tryBuildFrom(L, *LI);
  if (!ILS)
    return false;
  return tryOptimizeLoop(*ILS);
}

bool AIEInnerLoopPointerOptimizer::tryOptimizeLoop(InnerLoopStructure &ILS) {
  LLVM_DEBUG(dbgs() << "ILPO: Optimizing loop body: "
                    << ILS.getBody()->getName() << "\n");
  bool Changed = false;
  if (EnablePhiNormalization)
    Changed |= normalizePhiToLoadBase(ILS);
  if (EnablePostIncChain)
    Changed |= buildPostIncChain(ILS);
  return Changed;
}

/// Normalize pre-increment pointer PHIs to load-base form (Pattern 1).
///
/// Detects PHIs whose back-edge value is a constant-stride i8 GEP rooted
/// directly on the PHI (pre-increment pattern), and shifts the PHI so that
/// it lands on the first memory-access address each iteration.  All other
/// constant-offset GEPs rooted on the PHI are adjusted by subtracting the
/// stride so that they continue to address the same memory locations.
///
///   Before:
///     preheader: ...
///     body:
///       %phi    = phi ptr [ %init, %preheader ], [ %preinc, %body ]
///       %preinc = getelementptr i8, ptr %phi, i20 S    ; back-edge GEP first
///       %mem1   = load/store ..., ptr %preinc           ; accesses phi+S
///       %gep2   = getelementptr i8, ptr %phi, i20 S+D  ; sibling GEP
///       %mem2   = load/store ..., ptr %gep2             ; accesses phi+S+D
///
///   After:
///     preheader:
///       %phi.shifted.init = getelementptr i8, ptr %init, i20 S
///     body:
///       %phi    = phi ptr [ %phi.shifted.init, %preheader ], [ %phi.back,
///       %body ] %mem1   = load/store ..., ptr %phi              ; accesses phi
///       (=old+S) %gep2   = getelementptr i8, ptr %phi, i20 D    ; offset
///       adjusted: S+D-S=D %mem2   = load/store ..., ptr %gep2             ;
///       accesses phi+D (=old+S+D) %phi.back = getelementptr i8, ptr %phi, i20
///       S  ; at end of body
///
/// Safety guards:
///   - Live-out PHIs and live-out back-edge GEPs are skipped.
///   - If any sibling GEP off the phi has a non-constant index, the phi is
///     skipped (we cannot safely adjust it).
bool AIEInnerLoopPointerOptimizer::normalizePhiToLoadBase(
    InnerLoopStructure &ILS) {
  LLVM_DEBUG(dbgs() << "ILPO: Normalizing pre-increment PHIs to load base\n");

  BasicBlock *const Body = ILS.getBody();
  BasicBlock *const Preheader = ILS.getPreheader();
  Loop *const L = ILS.getLoop();
  Type *const Int8Ty = Type::getInt8Ty(Body->getContext());
  bool Changed = false;

  // Returns true when V has at least one user outside the loop.
  auto IsLiveOut = [&](Value *V) {
    return llvm::any_of(V->users(), [&](User *U) {
      auto *I = dyn_cast<Instruction>(U);
      return I && !L->contains(I->getParent());
    });
  };

  for (PHINode &Phi : Body->phis()) {
    if (!Phi.getType()->isPointerTy())
      continue;

    // --- Structural check ---
    // Back-edge value must be a constant-stride i8 GEP directly off the phi.
    Value *const BackVal = Phi.getIncomingValueForBlock(Body);
    auto *BackGEP = dyn_cast<GetElementPtrInst>(BackVal);
    if (!BackGEP || BackGEP->getPointerOperand() != &Phi)
      continue;
    int64_t Stride = 0;
    if (!AIEIRUtils::isChainLinkCandidate(BackGEP, Stride))
      continue;

    // --- Pattern check ---
    // Skip PHIs that are already in load-base form (Pattern 2): the phi (or
    // any transparent AddrSpaceCast of it) is used directly by a load/store
    // without an intervening GEP.
    if (hasDirectMemAccess(&Phi)) {
      LLVM_DEBUG(dbgs() << "ILPO:   Skip phi (already Pattern 2): " << Phi
                        << "\n");
      continue;
    }

    // --- Safety checks ---
    if (IsLiveOut(&Phi)) {
      LLVM_DEBUG(dbgs() << "ILPO:   Skip phi (live-out): " << Phi << "\n");
      continue;
    }
    if (IsLiveOut(BackGEP)) {
      LLVM_DEBUG(dbgs() << "ILPO:   Skip phi (back-edge GEP live-out): " << Phi
                        << "\n");
      continue;
    }

    // --- User tree validation ---
    // Recursively validate all users of the phi (excluding the back-edge GEP).
    // Collect adjustable sibling GEPs; bail out if any unrecognized user is
    // found (e.g. a non-constant GEP, a cast-then-GEP we cannot adjust, etc.).
    SmallVector<GetElementPtrInst *, 4> SiblingGEPs;
    if (!collectValidUsers(&Phi, BackGEP, Body, SiblingGEPs)) {
      LLVM_DEBUG(dbgs() << "ILPO:   Skip phi (unhandled user): " << Phi
                        << "\n");
      continue;
    }

    LLVM_DEBUG(dbgs() << "ILPO:   Normalizing phi: " << Phi
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

    // Adjust all sibling GEPs: after shifting phi by Stride, each "phi + K"
    // now addresses "old_phi + K + Stride".  Rewrite to "phi + (K - Stride)"
    // to preserve the original memory address.
    for (GetElementPtrInst *GEP : SiblingGEPs) {
      const ConstantInt *const OldCI = getConstantGEPLastIndex(GEP);
      const int64_t NewOffset = OldCI->getSExtValue() - Stride;
      LLVM_DEBUG(dbgs() << "ILPO:   Adjusting sibling GEP offset: "
                        << OldCI->getSExtValue() << " -> " << NewOffset
                        << "\n");
      GEP->setOperand(GEP->getNumOperands() - 1,
                      ConstantInt::get(OldCI->getType(), NewOffset));
    }

    // Insert the new back-edge advancement at the END of the body.
    // Placing it last lets buildPostIncChain later reposition it after loads.
    IRBuilder<> BodyBuilder(Body->getTerminator());
    GetElementPtrInst *const NewBack =
        cast<GetElementPtrInst>(BodyBuilder.CreateGEP(Int8Ty, &Phi, StrideVal,
                                                      Phi.getName() + ".back"));
    NewBack->setIsInBounds(true);
    Phi.setIncomingValueForBlock(Body, NewBack);

    LLVM_DEBUG(dbgs() << "ILPO:   Inserted new back-edge GEP: " << *NewBack
                      << "\n");
    Changed = true;
  }
  return Changed;
}

/// Reposition and re-root post-increment GEPs (Pattern 2).
///
/// For each pointer PHI used directly as a load address, this function
/// performs two sweeps over the GEP chain:
///   phi -> GEP1 (phi+d1) -> GEP2 (GEP1+d2) -> ...
///
/// Sweep 1 — Reposition:
///   Move each GEP to appear immediately after the last load that uses the
///   preceding chain node.  This creates adjacent (load, ptr+=delta) pairs
///   that allow the backend to select post-increment load instructions.
///
/// Sweep 2 — Re-root:
///   After repositioning, GEPs that are still rooted on the PHI with an
///   absolute offset (e.g. phi+128) are rewritten to be rooted on the
///   preceding GEP with a relative offset (e.g. gep1+64).  This exposes
///   the uniform step size that the backend needs for post-increment
///   instruction selection.
///
/// Example (two loads, stride split into two equal steps d1=d2=64):
///   Before:
///     %gep1  = getelementptr i8, ptr %phi,  i20 64  ; placed early
///     %gep2  = getelementptr i8, ptr %phi,  i20 128 ; absolute offset
///     %load1 = load ..., ptr %phi                    ; load at phi
///     %load2 = load ..., ptr %gep1                   ; load at phi+64
///
///   After Sweep 1 (reposition):
///     %load1 = load ..., ptr %phi                    ; load at phi
///     %gep1  = getelementptr i8, ptr %phi,  i20 64  ; after last load of phi
///     %load2 = load ..., ptr %gep1                   ; load at phi+64
///     %gep2  = getelementptr i8, ptr %phi,  i20 128 ; after last load of gep1
///
///   After Sweep 2 (re-root):
///     %load1 = load ..., ptr %phi                    ; load at phi
///     %gep1  = getelementptr i8, ptr %phi,  i20 64  ; phi + 64
///     %load2 = load ..., ptr %gep1                   ; load at phi+64
///     %gep2  = getelementptr i8, ptr %gep1, i20 64  ; gep1 + 64 (re-rooted)
bool AIEInnerLoopPointerOptimizer::buildPostIncChain(InnerLoopStructure &ILS) {
  LLVM_DEBUG(dbgs() << "ILPO: Building post-increment GEP chains\n");

  BasicBlock *const Body = ILS.getBody();
  bool Changed = false;

  for (PHINode &Phi : Body->phis()) {
    if (!Phi.getType()->isPointerTy())
      continue;

    // Only handle PHIs with direct memory uses (Pattern 2).
    if (AIEIRUtils::collectMemUsers(&Phi).empty())
      continue;

    LLVM_DEBUG(dbgs() << "ILPO:   Processing phi with direct mem uses: " << Phi
                      << "\n");

    // --- Sweep 1: Reposition ---
    // Walk the GEP chain: phi -> GEP1 -> GEP2 -> ...
    // For each node, move its successor GEP to just after the node's last load,
    // then step to that GEP and repeat.
    for (Value *Node = &Phi;;) {
      GetElementPtrInst *const NextGEP = findNextChainGEP(Node, Body);
      if (!NextGEP)
        break;

      const SmallVector<Instruction *, 4> MemUsers =
          AIEIRUtils::collectMemUsers(Node);
      Instruction *const LastLoad = AIEIRUtils::findLastInBlock(MemUsers, Body);
      if (!LastLoad)
        break;

      if (NextGEP->comesBefore(LastLoad)) {
        LLVM_DEBUG(dbgs() << "ILPO:   Moving GEP after last load:\n"
                          << "         GEP:   " << *NextGEP << "\n"
                          << "         After: " << *LastLoad << "\n");
        NextGEP->moveAfter(LastLoad);
        Changed = true;
      }

      Node = NextGEP;
    }

    // --- Sweep 2: Re-root ---
    // Walk the chain again.  For each GEP that is still rooted on the PHI
    // with an absolute offset, rewrite it to be rooted on the preceding GEP
    // with a relative offset (AbsOffset - PrevGEPOffset).
    // This converts "phi + 128" into "gep1 + 64" when gep1 = phi + 64.
    int64_t PrevOffset = 0; // cumulative offset of the previous chain node
    for (Value *Node = &Phi;;) {
      GetElementPtrInst *const NextGEP = findNextChainGEP(Node, Body);
      if (!NextGEP)
        break;

      // Only re-root GEPs that are still anchored on the PHI.
      if (NextGEP->getPointerOperand() == &Phi) {
        const ConstantInt *const CI = getConstantGEPLastIndex(NextGEP);
        if (CI) {
          const int64_t AbsOffset = CI->getSExtValue();
          const int64_t RelOffset = AbsOffset - PrevOffset;
          LLVM_DEBUG(dbgs() << "ILPO:   Re-rooting GEP: phi+" << AbsOffset
                            << " -> prev+" << RelOffset << "\n");
          NextGEP->setOperand(0, Node); // re-root on the previous chain node
          NextGEP->setOperand(NextGEP->getNumOperands() - 1,
                              ConstantInt::get(CI->getType(), RelOffset));
          Changed = true;
        }
      }

      // Advance: the cumulative offset of NextGEP from phi.
      const ConstantInt *const CI = getConstantGEPLastIndex(NextGEP);
      PrevOffset = CI ? CI->getSExtValue() + PrevOffset : 0;
      Node = NextGEP;
    }
  }
  return Changed;
}

namespace llvm {
FunctionPass *createAIEInnerLoopPointerOptimizerPass() {
  return new AIEInnerLoopPointerOptimizer();
}
} // namespace llvm
