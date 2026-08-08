//===-- AIE2AsmPrinter.cpp - AIEngine V2 LLVM assembly writer ------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the AIEngine V2 assembly language.
//
//===----------------------------------------------------------------------===//

#include "AIE2AsmPrinter.h"
#include "AIE2TargetMachine.h"
#include "InstPrinter/AIE2InstPrinter.h"
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOptimizationRemarkEmitter.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "aie-asm-printer"

// Simple pseudo-instructions have their lowering (with expansion to real
// instructions) auto-generated.
#include "AIE2GenMCPseudoLowering.inc"

bool AIE2AsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                                     const char *ExtraCode, raw_ostream &OS) {
  // First try the generic code, which knows about modifiers like 'c' and 'n'.
  if (!AsmPrinter::PrintAsmOperand(MI, OpNo, ExtraCode, OS))
    return false;

  const MachineOperand &MO = MI->getOperand(OpNo);
  switch (MO.getType()) {
  case MachineOperand::MO_Immediate:
    OS << MO.getImm();
    return false;
  case MachineOperand::MO_Register:
    OS << AIE2InstPrinter::getRegisterName(MO.getReg());
    return false;
  default:
    break;
  }

  return true;
}

bool AIE2AsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI,
                                           unsigned OpNo, const char *ExtraCode,
                                           raw_ostream &OS) {
  if (!ExtraCode) {
    const MachineOperand &MO = MI->getOperand(OpNo);
    // For now, we only support register memory operands in registers and
    // assume there is no addend
    if (!MO.isReg())
      return true;

    OS << "0(" << AIE2InstPrinter::getRegisterName(MO.getReg()) << ")";
    return false;
  }

  return AsmPrinter::PrintAsmMemoryOperand(MI, OpNo, ExtraCode, OS);
}

bool AIE2AsmPrinter::lowerOperand(const MachineOperand &MO,
                                  MCOperand &MCOp) const {
  return LowerAIEMachineOperandToMCOperand(MO, MCOp, *this);
}

namespace {
/// One execution region of an MBB: consecutive bundles run the same number of
/// times. A delay-slotted branch ends a region; bundles after it start next.
class BundleRegion {
  unsigned BundleCount = 0;
  unsigned ByteCount = 0;
  // Delay-slot bundles still owed by an open branch; 0 = no open branch.
  unsigned PendingDelaySlots = 0;

  void tally(const MachineInstr &MI, const AIEBaseInstrInfo &TII) {
    BundleCount++;
    ByteCount += TII.getAIEMachineBundleSize(MI);
  }

  bool hasPendingDelaySlots() const { return PendingDelaySlots > 0; }
  void consumeDelaySlot() { --PendingDelaySlots; }
  bool delaySlotsExhausted() const { return PendingDelaySlots == 0; }

  // Open a window of N delay slots (N == 0 opens nothing).
  void beginDelaySlots(unsigned N) { PendingDelaySlots = N; }

public:
  unsigned getBundleCount() const { return BundleCount; }
  unsigned getByteCount() const { return ByteCount; }
  bool empty() const { return BundleCount == 0; }

  /// Add one bundle; true when it ends the region (a branch's last delay slot).
  bool appendBundleEndsRegion(const MachineInstr &MI,
                              const AIEBaseInstrInfo &TII) {
    tally(MI, TII);

    // In a branch's delay slots: drain one; the region ends at the last.
    if (hasPendingDelaySlots()) {
      consumeDelaySlot();
      return delaySlotsExhausted();
    }

    // A delay-slotted branch opens a window; other bundles continue the region.
    beginDelaySlots(TII.getNumDelaySlots(MI));
    return false;
  }
};
} // namespace

/// Split MBB's bundles into execution regions in layout order; empty if none.
static SmallVector<BundleRegion, 2>
partitionIntoRegions(const MachineBasicBlock &MBB,
                     const AIEBaseInstrInfo &TII) {
  SmallVector<BundleRegion, 2> Regions;
  BundleRegion Current;
  for (const MachineInstr &MI : MBB) {
    if (!MI.isBundle())
      continue;
    if (Current.appendBundleEndsRegion(MI, TII)) {
      Regions.push_back(Current);
      Current = BundleRegion();
    }
  }
  // Trailing bundles after the last branch (the common single-region block).
  if (!Current.empty())
    Regions.push_back(Current);
  return Regions;
}

// Reset the offset at each function's entry block; avoids a stale MF pointer.
void AIE2AsmPrinter::resetOffsetForNewFunction(const MachineBasicBlock &MBB) {
  if (!MBB.getPrevNode())
    LayoutByteOffset = 0;
}

#ifndef NDEBUG
// Blocks must arrive in layout order; LastEmittedMBB is compared, not deref'd.
void AIE2AsmPrinter::assertLayoutOrder(const MachineBasicBlock &MBB) {
  const MachineBasicBlock *LayoutPred = MBB.getPrevNode();
  assert((!LayoutPred || LastEmittedMBB == LayoutPred) &&
         "MBBs emitted out of layout order; region byte offsets are invalid");
  LastEmittedMBB = &MBB;
}
#endif

void AIE2AsmPrinter::emitRegionRemarks(const MachineBasicBlock &MBB) {
  resetOffsetForNewFunction(MBB);
#ifndef NDEBUG
  assertLayoutOrder(MBB);
#endif

  auto *TII = static_cast<const AIEBaseInstrInfo *>(
      MBB.getParent()->getSubtarget().getInstrInfo());

  // One remark per region; RegionIndex distinguishes a multi-region block.
  unsigned RegionIndex = 0;
  for (const BundleRegion &Region : partitionIntoRegions(MBB, *TII)) {
    const unsigned Offset = LayoutByteOffset;
    LayoutByteOffset += Region.getByteCount();
    ORE->emit([&]() {
      return MachineOptimizationRemarkAnalysis(DEBUG_TYPE, "analysis",
                                               MBB.begin()->getDebugLoc(), &MBB)
             << ore::NV("BasicBlock", MBB.getName())
             << ore::NV("RegionIndex", RegionIndex)
             << ore::NV("BundleCount", Region.getBundleCount())
             << ore::NV("ByteCount", Region.getByteCount())
             << ore::NV("Offset", Offset);
    });
    RegionIndex++;
  }
}

void AIE2AsmPrinter::emitBasicBlockStart(const MachineBasicBlock &MBB) {
  emitRegionRemarks(MBB);

  AsmPrinter::emitBasicBlockStart(MBB);
}

AsmPrinter *
llvm::createAIE2AsmPrinterPass(TargetMachine &TM,
                               std::unique_ptr<MCStreamer> &&Streamer) {
  return new AIE2AsmPrinter(TM, std::move(Streamer));
}

// Register the AIE2, AIE2P, and AIE2PS asm printers; called from
// LLVMInitializeAIEAsmPrinter.
void initializeAIE2AsmPrinters() {
  RegisterAsmPrinter<AIE2AsmPrinter> AP2(getTheAIE2Target());
  // FIXME using AIE2AsmPrinter for AIE2P and AIE2PS target
  RegisterAsmPrinter<AIE2AsmPrinter> AP2P(getTheAIE2PTarget());
  RegisterAsmPrinter<AIE2AsmPrinter> AP2PS(getTheAIE2PSTarget());
}
