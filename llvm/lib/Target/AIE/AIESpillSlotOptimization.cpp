//===- AIESpillSlotOptimization.cpp - Optimize spill slots ----------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// AIE supports composite registers that may be partially live at
// spill points. This pass decomposes composite spill/reload instructions into
// smaller subreg operations, creating minimized stack slots to reduce memory
// usage.
//
// Two-phase approach:
// - Analysis: Collect spill/reload info, compute liveness, determine per-subreg
//   decomposition details. Flattening of composite register hierarchies is done
//   upfront to avoid iteration.
// - Rewrite: Emit decomposed instructions with lazily-created replacement
// slots.
//   Slots are only allocated when actually used, ensuring minimal stack usage.
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "AIEBaseInstrInfo.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/LivePhysRegs.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/InitializePasses.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/Support/Debug.h"

#include <climits>
#include <cmath>

using namespace llvm;

#define DEBUG_TYPE "aie-spill-slot-opt"

namespace {

/// Bits per byte constant for bit-to-byte conversions.
constexpr unsigned BitsPerByte = CHAR_BIT;
/// AIE does not have native load/store support for smaller than 4 bytes.
constexpr unsigned MinMemSizeBytes = 4;

//===----------------------------------------------------------------------===//
// Data Structures
//===----------------------------------------------------------------------===//

/// Pre-computed info for a single subreg within a spill/reload.
struct SubRegInfo {
  MCRegister Reg;
  unsigned SubRegIndex = 0;
  unsigned SizeBytes = 0;
  int64_t ByteOffset = 0;
  bool IsLive = false;

  SubRegInfo(MCRegister R, unsigned Idx, unsigned Size, int64_t Off, bool Live)
      : Reg(R), SubRegIndex(Idx), SizeBytes(Size), ByteOffset(Off),
        IsLive(Live) {
    assert(Reg.isValid() && "SubRegInfo must have a valid register");
    assert(ByteOffset >= 0 && "ByteOffset must be non-negative");
    assert(SizeBytes >= MinMemSizeBytes &&
           "SubregSpill size is not natively supported");
  }
};

/// Per-instruction spill/reload with pre-computed decomposition info.
struct SpillPoint {
  MachineInstr *MI = nullptr;
  int FI = -1;
  bool IsStore = false;
  bool DecomposeProfitable = false;
  unsigned RegFlags = 0;
  SmallVector<SubRegInfo, 8> SubRegs;

  SpillPoint(MachineInstr &MI, int FI, uint64_t SlotSize,
             const LivePhysRegs &LiveRegs, const AIEBaseInstrInfo &TII,
             const MachineRegisterInfo &MRI);
};

/// A minimized stack slot replacing part of an original spill slot.
struct ReplacementSlot {
  int64_t Offset;
  uint64_t Size;
  Align Alignment;
  const TargetRegisterClass *RC;
  int NewFI = -1;

  ReplacementSlot(int64_t Offset, uint64_t Size, Align Alignment,
                  const TargetRegisterClass *RC)
      : Offset(Offset), Size(Size), Alignment(Alignment), RC(RC) {
    assert(Size >= MinMemSizeBytes &&
           "Replacement slot Cannot be small than Minimal Memory Access Size!");
    assert(RC && "Replacement slot must have a valid register class");
  }

  /// Allocate a stack object for this slot if not already created.
  void allocateStackObject(MachineFrameInfo &MFI) {
    if (NewFI >= 0)
      return;
    assert(Size >= MinMemSizeBytes &&
           "Cannot allocate objects smaller than Minimal Memory Access Size!");
    assert(RC && "ReplacementSlot must have a register class");
    NewFI = MFI.CreateSpillStackObject(Size, Alignment);
    assert(NewFI >= 0 && "Failed to create spill stack object");
  }
};

/// Per-FI tracking of replacement slots and spill points.
struct FrameIndexInfo {
  SmallVector<ReplacementSlot, 2> Slots;
  SmallVector<std::unique_ptr<SpillPoint>, 4> Spills;
  SmallVector<std::unique_ptr<SpillPoint>, 4> Reloads;

  ReplacementSlot *findSlot(int64_t Offset) {
    for (ReplacementSlot &R : Slots)
      if (R.Offset == Offset)
        return &R;
    return nullptr;
  }

  /// Get or create a replacement slot at the given offset. Multiple subregs at
  /// the same offset may share a slot (StackSlotColoring ensures this is safe).
  ReplacementSlot &getOrCreateSlot(int64_t Offset, uint64_t Size,
                                   Align Alignment,
                                   const TargetRegisterClass *RC) {
    if (ReplacementSlot *R = findSlot(Offset)) {
      // hasConflictingLanes() rejects any slot whose live lanes share an offset
      // with mismatched sizes, so a shared offset must have a matching size.
      // Alignment can still differ: equal size does not imply the same register
      // class, and alignment derives from the class, so keep the max.
      assert(Size == R->Size && "Lanes sharing an offset must have equal size");
      if (Alignment > R->Alignment)
        R->Alignment = Alignment;
      return *R;
    }
    return Slots.emplace_back(Offset, Size, Alignment, RC);
  }

  bool empty() const { return Slots.empty(); }
};

//===----------------------------------------------------------------------===//
// Pass Implementation
//===----------------------------------------------------------------------===//

class AIESpillSlotOptimization : public MachineFunctionPass {
public:
  static char ID;
  AIESpillSlotOptimization() : MachineFunctionPass(ID) {
    initializeAIESpillSlotOptimizationPass(*PassRegistry::getPassRegistry());
  }

  StringRef getPassName() const override {
    return "AIE Spill Slot Optimization";
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

private:
  const TargetRegisterInfo *TRI = nullptr;
  const AIEBaseInstrInfo *TII = nullptr;
  MachineFrameInfo *MFI = nullptr;
  MachineRegisterInfo *MRI = nullptr;

  MapVector<int, FrameIndexInfo> OrigSlotDecompositions;

  // Phase 1: Analysis
  void analyzeFunction(MachineFunction &MF);
  bool isValidSpillSlot(int FI, MachineInstr &MI) const;

  // Phase 2: Rewrite
  bool rewriteInstructions(MachineFunction &MF);
  void removeOriginalSlots();
  DenseSet<int64_t>
  processSpillsForSlot(int FI, FrameIndexInfo &Info,
                       SmallVectorImpl<MachineInstr *> &ToErase);
  void processReloadsForSlot(FrameIndexInfo &Info,
                             const DenseSet<int64_t> &StoredOffsets,
                             SmallVectorImpl<MachineInstr *> &ToErase);

  void emitSubRegAccess(const SpillPoint &SP, const SubRegInfo &SI,
                        const ReplacementSlot &R) const;
};

//===----------------------------------------------------------------------===//
// Debug Output
//===----------------------------------------------------------------------===//

raw_ostream &operator<<(raw_ostream &OS, const SubRegInfo &SI) {
  OS << "idx=" << SI.SubRegIndex << " " << (SI.IsLive ? "LIVE" : "UNDEF");
  return OS;
}

raw_ostream &operator<<(raw_ostream &OS, const SpillPoint &SP) {
  OS << (SP.IsStore ? "Spill" : "Reload") << " FI=" << SP.FI << " [";
  for (size_t I = 0; I < SP.SubRegs.size(); ++I) {
    if (I > 0)
      OS << ", ";
    OS << SP.SubRegs[I];
  }
  OS << "]";
  return OS;
}

/// Flattened elementary expansion info with composed subreg index.
struct ElementaryExpandInfo {
  unsigned SubRegIndex = 0;
  unsigned SizeBytes = 0;
};

/// Recursively flatten pseudo spill expansions to elementary (leaf) operations.
/// Composes subreg indices through the hierarchy to get final subregs relative
/// to the original root register.
static void flattenExpandInfos(unsigned Opcode, unsigned ParentSubRegIndex,
                               const AIEBaseInstrInfo &TII,
                               const TargetRegisterInfo &TRI,
                               SmallVectorImpl<ElementaryExpandInfo> &Result) {

  const auto ExpandInfos = TII.getSpillPseudoExpandInfoByOpcode(Opcode);

  // Base case: opcode doesn't expand further - it's elementary.
  if (ExpandInfos.empty()) {
    // Only add an entry if we're in a recursive call (ParentSubRegIndex != 0).
    // At the top level, an empty result indicates no decomposition is possible.
    if (ParentSubRegIndex) {
      ElementaryExpandInfo EEI;
      EEI.SubRegIndex = ParentSubRegIndex;
      Result.push_back(EEI);
    }
    return;
  }

  // Recursive case: expand each child and compose subreg indices.
  for (const auto &EI : ExpandInfos) {
    // Compose the parent's subreg index with this level's subreg index.
    const unsigned ComposedIndex =
        ParentSubRegIndex
            ? TRI.composeSubRegIndices(ParentSubRegIndex, EI.SubRegIndex)
            : EI.SubRegIndex;

    flattenExpandInfos(EI.ExpandedOpCode, ComposedIndex, TII, TRI, Result);

    // Propagate explicit MemSize to the leaf. This is needed for spills that
    // can't be characterized by sub-registers alone (e.g., AIE2P ST_R_SPILL
    // uses NoSubRegister with explicit size).
    if (EI.MemSize && !Result.empty())
      Result.back().SizeBytes = EI.MemSize;
  }
}

/// Build SubRegInfo from elementary expansion info.
static SubRegInfo buildSubRegInfo(Register Reg, int64_t BaseOffset,
                                  const ElementaryExpandInfo &EI,
                                  const TargetRegisterInfo &TRI,
                                  const LivePhysRegs &LiveRegs,
                                  const MachineRegisterInfo &MRI) {
  assert(Reg.isPhysical() && "Expected physical register for spill");
  assert(BaseOffset >= 0 && "BaseOffset must be non-negative");

  const MCRegister SubReg =
      EI.SubRegIndex ? TRI.getSubReg(Reg, EI.SubRegIndex) : Reg.asMCReg();
  assert(SubReg.isValid() && "Failed to get subreg from composed index");

  // Compute memory size: use explicit size if provided, otherwise derive from
  // subreg index size (converting bits to bytes).
  unsigned SizeBytes = EI.SizeBytes;
  if (!SizeBytes && EI.SubRegIndex)
    SizeBytes = divideCeil(TRI.getSubRegIdxSize(EI.SubRegIndex), BitsPerByte);
  SizeBytes = std::max(SizeBytes, MinMemSizeBytes);

  // Compute offset within the spill slot (base + subreg offset in bytes).
  const int64_t ByteOffset =
      BaseOffset +
      (EI.SubRegIndex
           ? ceil(TRI.getSubRegIdxOffset(EI.SubRegIndex) / BitsPerByte)
           : 0);

  const bool IsLive = !LiveRegs.available(MRI, SubReg);
  return SubRegInfo(SubReg, EI.SubRegIndex, SizeBytes, ByteOffset, IsLive);
}

SpillPoint::SpillPoint(MachineInstr &MI, int FI, uint64_t SlotSize,
                       const LivePhysRegs &LiveRegs,
                       const AIEBaseInstrInfo &TII,
                       const MachineRegisterInfo &MRI)
    : MI(&MI), FI(FI), IsStore(MI.mayStore()),
      RegFlags(getRegState(MI.getOperand(0))) {
  assert(FI >= 0 && "Frame index must be non-negative");
  assert(SlotSize > 0 && "Slot size must be positive");
  assert((MI.mayStore() || MI.mayLoad()) &&
         "Expected spill/reload instruction");

  const Register Reg = MI.getOperand(0).getReg();
  assert(Reg.isPhysical() && "Expected physical register in spill instruction");

  assert(MI.hasOneMemOperand() &&
         "Spill instruction must have exactly one MMO");
  const int64_t BaseOffset = (*MI.memoperands_begin())->getOffset();
  const TargetRegisterInfo &TRI = *MRI.getTargetRegisterInfo();

  // Flatten composite spill hierarchy to elementary subregs.
  SmallVector<ElementaryExpandInfo, 8> FlattenedInfos;
  flattenExpandInfos(MI.getOpcode(), /*ParentSubRegIndex=*/0, TII, TRI,
                     FlattenedInfos);

  // Elementary non-pseudo spill (no expansion): create a single SubRegInfo
  // representing the whole register with no decomposition.
  if (FlattenedInfos.empty()) {
    const unsigned SizeBytes = (*MI.memoperands_begin())->getSize().getValue();
    const bool IsLive = !LiveRegs.available(MRI, Reg);
    SubRegs.emplace_back(Reg.asMCReg(), 0, SizeBytes, BaseOffset, IsLive);
    return;
  }

  // Composite spill: build SubRegInfo for each elementary subreg.
  for (const auto &EI : FlattenedInfos) {
    SubRegInfo SI = buildSubRegInfo(Reg, BaseOffset, EI, TRI, LiveRegs, MRI);
    DecomposeProfitable |= SI.IsLive;
    SubRegs.push_back(SI);
  }
  assert(!SubRegs.empty() && "Composite spill must have at least one subreg");

  // Only profitable if decomposition can reduce memory size.
  const bool SizeReduction =
      llvm::any_of(SubRegs, [SlotSize](const SubRegInfo &SI) {
        return SI.SizeBytes < SlotSize;
      });
  if (!SizeReduction)
    DecomposeProfitable = false;
}

} // end anonymous namespace

//===----------------------------------------------------------------------===//
// Phase 1: Analysis
//===----------------------------------------------------------------------===//

char AIESpillSlotOptimization::ID = 0;
char &llvm::AIESpillSlotOptimizationID = AIESpillSlotOptimization::ID;

INITIALIZE_PASS(AIESpillSlotOptimization, DEBUG_TYPE,
                "AIE Spill Slot Optimization", false, false)

bool AIESpillSlotOptimization::isValidSpillSlot(int FI,
                                                MachineInstr &MI) const {
  // We track all spill instructions, including elementary non-pseudo spills
  // (e.g., AIE2P::VST_dmx_sts_x_spill). The SpillPoint constructor handles
  // these by creating a single SubRegInfo.
  return FI >= 0 && FI < MFI->getObjectIndexEnd() &&
         !MFI->isDeadObjectIndex(FI) && MFI->isSpillSlotObjectIndex(FI);
}

void AIESpillSlotOptimization::analyzeFunction(MachineFunction &MF) {
  LLVM_DEBUG(dbgs() << "=== Phase 1: Analysis ===\n");

  OrigSlotDecompositions.clear();

  for (MachineBasicBlock &MBB : MF) {
    // Pass 1 (Backward): Collect reloads with post-MI liveness.
    LivePhysRegs LiveRegs(*TRI);
    LiveRegs.addLiveOuts(MBB);

    for (MachineInstr &MI : reverse(MBB)) {
      int FI = -1;
      if (TII->isLoadFromStackSlot(MI, FI) && isValidSpillSlot(FI, MI)) {
        auto SP = std::make_unique<SpillPoint>(MI, FI, MFI->getObjectSize(FI),
                                               LiveRegs, *TII, *MRI);
        LLVM_DEBUG(dbgs() << "  " << *SP << ": " << MI);
        OrigSlotDecompositions[FI].Reloads.emplace_back(std::move(SP));
      }
      LiveRegs.stepBackward(MI);
    }

    // Pass 2 (Forward): Collect spills with pre-MI liveness.
    LiveRegs.clear();
    LiveRegs.addLiveIns(MBB);
    SmallVector<std::pair<MCPhysReg, const MachineOperand *>, 2> Clobbers;

    for (MachineInstr &MI : MBB) {
      int FI = -1;
      if (TII->isStoreToStackSlot(MI, FI) && isValidSpillSlot(FI, MI)) {
        auto Spill = std::make_unique<SpillPoint>(
            MI, FI, MFI->getObjectSize(FI), LiveRegs, *TII, *MRI);
        LLVM_DEBUG(dbgs() << "  " << *Spill << ": " << MI);

        OrigSlotDecompositions[FI].Spills.emplace_back(std::move(Spill));
      }
      Clobbers.clear();
      LiveRegs.stepForward(MI, Clobbers);
    }
  }

  LLVM_DEBUG({
    dbgs() << "\n  FI Summary: " << MF.getName() << "\n";
    for (const auto &[FI, Info] : OrigSlotDecompositions)
      dbgs() << "    FI=" << FI << ": " << Info.Spills.size() << " spills, "
             << Info.Reloads.size() << " reloads\n";
  });
}
//===----------------------------------------------------------------------===//
// Phase 2: Rewrite Instructions
//===----------------------------------------------------------------------===//

void AIESpillSlotOptimization::emitSubRegAccess(
    const SpillPoint &SP, const SubRegInfo &SI,
    const ReplacementSlot &R) const {
  assert(SP.MI && "SpillPoint must have a valid MachineInstr");
  assert(R.NewFI >= 0 && "ReplacementSlot must be allocated before emission");
  assert(R.RC && "ReplacementSlot must have a valid register class");

  MachineInstr &MI = *SP.MI;
  MachineBasicBlock &MBB = *MI.getParent();
  const bool IsKill = SP.RegFlags & RegState::Kill;

  // Use TII methods to create spill/reload with correct pseudo opcodes.
  if (SP.IsStore) {
    TII->storeRegToStackSlot(MBB, MI.getIterator(), SI.Reg, IsKill, R.NewFI,
                             R.RC, TRI, Register());
  } else {
    TII->loadRegFromStackSlot(MBB, MI.getIterator(), SI.Reg, R.NewFI, R.RC, TRI,
                              Register());
  }

  MachineInstr &NewMI = *std::prev(MI.getIterator());
  // Make sure to copy Renamable Attributes for MachineCopy Coalescing
  const bool IsRenamable = SP.RegFlags & RegState::Renamable;
  NewMI.getOperand(0).setIsRenamable(IsRenamable);
  LLVM_DEBUG(dbgs() << "    Created spill for " << printReg(SI.Reg, TRI)
                    << " -> %stack." << R.NewFI << "\n");
}

/// Process spills for a single frame index: create replacement slots and emit
/// decomposed stores. Returns the set of offsets that have stores (for reload
/// filtering).
DenseSet<int64_t> AIESpillSlotOptimization::processSpillsForSlot(
    int FI, FrameIndexInfo &Info, SmallVectorImpl<MachineInstr *> &ToErase) {
  DenseSet<int64_t> StoredOffsets;
  for (const auto &SP : Info.Spills) {
    if (!SP->DecomposeProfitable)
      continue;

    LLVM_DEBUG(dbgs() << "  Rewriting spill: " << *SP->MI);
    for (const SubRegInfo &SI : SP->SubRegs) {
      if (!SI.IsLive)
        continue;
      StoredOffsets.insert(SI.ByteOffset);

      const auto *RC = TRI->getMinimalPhysRegClass(SI.Reg);
      ReplacementSlot &Slot = Info.getOrCreateSlot(SI.ByteOffset, SI.SizeBytes,
                                                   TRI->getSpillAlign(*RC), RC);
      Slot.allocateStackObject(*MFI);

      LLVM_DEBUG(dbgs() << "  FI=" << FI << " Offset=" << SI.ByteOffset
                        << " -> NewFI=" << Slot.NewFI << " size=" << Slot.Size
                        << " align=" << Slot.Alignment.value() << "\n");

      emitSubRegAccess(*SP, SI, Slot);
    }
    ToErase.push_back(SP->MI);
  }
  return StoredOffsets;
}

/// Process reloads for a single frame index: emit decomposed loads for subregs
/// that have corresponding stores.
/// A reload's IsLive may differ from its corresponding spill's IsLive.
/// The spill's liveness determines what gets stored; the reload's liveness
/// determines what the program needs at that point.
void AIESpillSlotOptimization::processReloadsForSlot(
    FrameIndexInfo &Info, const DenseSet<int64_t> &StoredOffsets,
    SmallVectorImpl<MachineInstr *> &ToErase) {
  for (const auto &SP : Info.Reloads) {
    bool Emitted = false;
    bool HasMatchingStore = false;

    // Note: We cannot skip based on DecomposeProfitable here because we need
    // to check for matching stores and erase the reload if its store was
    // decomposed, even if this reload has no live subregs to emit.
    for (const SubRegInfo &SI : SP->SubRegs) {
      if (!StoredOffsets.contains(SI.ByteOffset))
        continue;

      HasMatchingStore = true;
      // Only emit loads if decomposition is profitable and subreg is live
      if (!SI.IsLive)
        continue;

      LLVM_DEBUG(if (!Emitted) dbgs() << "  Decomposing reload: " << *SP->MI);
      Emitted = true;

      ReplacementSlot *Slot = Info.findSlot(SI.ByteOffset);
      assert(Slot && Slot->NewFI >= 0 &&
             "Attempting to Load without a corresponding Store Slot");

      emitSubRegAccess(*SP, SI, *Slot);
    }

    if (Emitted || HasMatchingStore)
      ToErase.push_back(SP->MI);
  }
}

/// Return true if two live lanes map to the same byte offset with different
/// sizes. StackSlotColoring can coalesce registers from different register
/// files onto one frame index (e.g. a DS descriptor and a DM accumulator).
/// Their lanes then overlap at an offset with mismatched sizes (e.g. a 4-byte
/// scalar and a 128-byte accumulator at offset 0), so a single replacement slot
/// cannot be sized correctly for both. Such slots must be left intact.
static bool hasConflictingLanes(const FrameIndexInfo &Info) {
  // A register's own lanes never share an offset, so a second lane at an
  // occupied offset comes from another coalesced register; a differing size
  // means the lanes belong to different register classes.
  DenseMap<int64_t, unsigned> SizeAtOffset;
  auto LanesConflict =
      [&](const SmallVectorImpl<std::unique_ptr<SpillPoint>> &SpillPoints) {
        for (const auto &SpillPoint : SpillPoints)
          for (const SubRegInfo &SubReg : SpillPoint->SubRegs) {
            if (!SubReg.IsLive)
              continue;
            auto [It, Inserted] =
                SizeAtOffset.try_emplace(SubReg.ByteOffset, SubReg.SizeBytes);
            if (!Inserted && It->second != SubReg.SizeBytes)
              return true;
          }
        return false;
      };
  return LanesConflict(Info.Spills) || LanesConflict(Info.Reloads);
}

bool AIESpillSlotOptimization::rewriteInstructions(MachineFunction &MF) {
  LLVM_DEBUG(dbgs() << "=== Phase 2: Rewrite Instructions ===\n");

  SmallVector<MachineInstr *, 32> ToErase;

  for (auto &[FI, Info] : OrigSlotDecompositions) {
    // Decompose all-or-nothing, and only when safe and profitable:
    //  - every spill must be profitable, else a partial rewrite splits the slot
    //    across the original and replacement objects; and
    //  - no coalesced lanes may conflict, which would give the replacement slot
    //    the wrong size.
    const bool CanDecomposeSlot =
        llvm::all_of(Info.Spills,
                     [](const std::unique_ptr<SpillPoint> &SP) {
                       return SP->DecomposeProfitable;
                     }) &&
        !hasConflictingLanes(Info);
    if (!CanDecomposeSlot) {
      LLVM_DEBUG(dbgs() << "  FI=" << FI << ": skipping decomposition\n");
      continue;
    }

    const DenseSet<int64_t> StoredOffsets =
        processSpillsForSlot(FI, Info, ToErase);
    processReloadsForSlot(Info, StoredOffsets, ToErase);
  }

  const bool Changed = !ToErase.empty();
  LLVM_DEBUG(dbgs() << "  Erasing " << ToErase.size() << " instructions\n");
  for (MachineInstr *MI : ToErase)
    MI->eraseFromParent();

  return Changed;
}

void AIESpillSlotOptimization::removeOriginalSlots() {
  LLVM_DEBUG(dbgs() << "=== Removing Original Slots ===\n");

  // Remove original spill slots that were decomposed into smaller replacement
  // slots. Only slots with replacement slots should be
  // removed; others were not modified by this pass.
  for (auto &[FI, Info] : OrigSlotDecompositions) {
    if (Info.empty())
      continue;
    LLVM_DEBUG(dbgs() << "  Removed FI=" << FI << "\n");
    MFI->RemoveStackObject(FI);
  }
}

//===----------------------------------------------------------------------===//
// Main Entry Point
//===----------------------------------------------------------------------===//

bool AIESpillSlotOptimization::runOnMachineFunction(MachineFunction &MF) {
  LLVM_DEBUG(dbgs() << "*** AIE Spill Slot Optimization: " << MF.getName()
                    << " ***\n");

  TRI = MF.getSubtarget().getRegisterInfo();
  TII = static_cast<const AIEBaseInstrInfo *>(MF.getSubtarget().getInstrInfo());
  MFI = &MF.getFrameInfo();
  MRI = &MF.getRegInfo();
  assert(TRI && "Missing TargetRegisterInfo");
  assert(TII && "Missing AIEBaseInstrInfo");
  assert(MFI && "Missing MachineFrameInfo");
  assert(MRI && "Missing MachineRegisterInfo");

  if (MFI->getNumObjects() == 0) {
    LLVM_DEBUG(dbgs() << "No stack objects, skipping\n");
    return false;
  }

  // Phase 1: Analyze spill/reload instructions and compute elementary
  // decomposition. Flattening is done upfront so no iteration is needed.
  analyzeFunction(MF);

  // Phase 2: Rewrite instructions using decomposed subregs.
  const bool Changed = rewriteInstructions(MF);

  // Phase 3: Remove original slots that were decomposed.
  if (Changed)
    removeOriginalSlots();

  LLVM_DEBUG(dbgs() << "*** Done, changed=" << Changed << " ***\n");
  return Changed;
}

FunctionPass *llvm::createAIESpillSlotOptimization() {
  return new AIESpillSlotOptimization();
}
