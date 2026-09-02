//===-- AIEBaseInstrInfo.cpp - AIE Instruction Information ------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIE implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseInstrInfo.h"
#include "AIE.h"
#include "AIEBasePipelinerLoopInfo.h"
#include "AIEBaseRegisterInfo.h"
#include "AIEBaseSubtarget.h"
#include "AIEHazardRecognizer.h"
#include "MCTargetDesc/AIEFormat.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/CodeGen/GlobalISel/GenericMachineInstrs.h"
#include "llvm/CodeGen/LivePhysRegs.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/Operator.h"
#include "llvm/MC/MCInstrItineraries.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"
#include <limits>

#define DEBUG_TYPE "aie-codegen"

using namespace llvm;

STATISTIC(NumFoldImmAttempts, "Number of foldImmediate attempts (AIE)");
STATISTIC(NumFoldImmBlockedMultiUse,
          "foldImmediate calls blocked by hasOneNonDBGUse mitigation");
STATISTIC(NumFoldImmSuccesses, "Number of foldImmediate successes (AIE)");
static cl::opt<bool> AIEFoldImmRequireOneUse(
    "aie-fold-imm-require-one-use", cl::init(true), cl::Hidden,
    cl::desc("Only fold immediate into COPY when the constant has a single "
             "non-debug use (so the def can be DCE'd)."));

static cl::opt<bool> AIEDisableFoldImm(
    "aie-disable-fold-imm", cl::init(false), cl::Hidden,
    cl::desc("Completely disable the AIE foldImmediate override (fall back to "
             "default no-op behaviour)."));

static cl::opt<bool>
    NoCheapInstHoisting("aie-no-cheap-inst-hoising",
                        cl::desc("Disable hoisting of cheap instructions"),
                        cl::init(false), cl::Hidden);
static cl::opt<bool> AccurateMemEdges(
    "aie-accurate-mem-edges",
    cl::desc("Model memory ordering edges with an exact latency"),
    cl::init(true), cl::Hidden);

namespace {
// Note we might want to make NumDelaySlots a property somewhere in .td
const constexpr unsigned NumDelaySlots = 5;
} // namespace

unsigned
AIEBaseInstrInfo::getNumDelaySlots(const MachineInstr &MI,
                                   MachineInstr::QueryType Query) const {
  return MI.hasDelaySlot(Query) ? NumDelaySlots : 0;
}

std::optional<AIEBaseInstrInfo::PseudoBranchExpandInfo>
AIEBaseInstrInfo::getPseudoBranchExpandInfo(const MachineInstr &MI) const {
  return {};
}

/// insertNoop - Insert a noop into the instruction stream at the specified
/// point.  This is used in Post-RA Scheduling to insert no-ops for functional
/// correctness.
void AIEBaseInstrInfo::insertNoop(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator MI) const {
  DebugLoc DL;
  BuildMI(MBB, MI, DL, get(getNopOpcode()));
}

bool AIEBaseInstrInfo::reverseBranchCondition(
    SmallVectorImpl<MachineOperand> &Cond) const {
  assert((Cond.size() == 2) && "Invalid branch condition!");
  // TODO We are returning Success in analyzeBranch for PseudoLoopEnd
  // which means we should be able to reverse the branch. Currently we are
  // supporting single BB loop for Zero-overhead loop, and there's no block
  // reordering that can make the loop block the fallthrough of the block
  // itself. Once we support multi BB, we will need to support
  // reverseBranchCondition.
  // For now, the PseudoLoopEnd opcode will trigger an abort in
  // getOppositeBranchOpcode().
  Cond[0].setImm(getOppositeBranchOpcode(Cond[0].getImm()));
  return false;
}

// The contents of values added to Cond are not examined outside of
// AIEInstrInfo, giving us flexibility in what to push to it. For AIE, we
// push BranchOpcode and the register that contains the branching condition.
// Returns whether we understand the flow.
static bool parseCondBranch(MachineInstr &LastInst, MachineBasicBlock *&Target,
                            SmallVectorImpl<MachineOperand> &Cond) {
  // Block ends with fall-through condbranch.
  assert(LastInst.getDesc().isConditionalBranch() &&
         "Unknown conditional branch");
  // We are supposed to compose Cond here. Why would it contain something?
  assert(Cond.size() == 0 && "Unexpected information in Cond");
  // We don't understand dynamic flow.
  if (!LastInst.getOperand(1).isMBB()) {
    return false;
  }
  // Everything that we consider a conditional branch has the same signature:
  // (otherop, targetbb)
  Cond.push_back(MachineOperand::CreateImm(LastInst.getOpcode()));
  Cond.push_back(LastInst.getOperand(0));
  Target = LastInst.getOperand(1).getMBB();

  return true;
}

// Implement jump optimizations (combined with removeBranch and insertBranch)
// See llvm/Codegen/TargetInstrInfo.h for details.
bool AIEBaseInstrInfo::analyzeBranch(MachineBasicBlock &MBB,
                                     MachineBasicBlock *&TBB,
                                     MachineBasicBlock *&FBB,
                                     SmallVectorImpl<MachineOperand> &Cond,
                                     bool AllowModify) const {

  const bool Success = false;
  const bool Unhandled = true;

  TBB = FBB = nullptr;
  Cond.clear();

  // If the block has no terminators, it just falls into the block after it.
  // If the last instruction is a barrier, verify_machineinstrs will complain
  // about the fallthrough
  // We also check for isDelayedSchedBarrier(): its presence indicates we are
  // at a late stage, and branches aren't easily understandable anymore.
  MachineBasicBlock::iterator I = MBB.getLastNonDebugInstr();
  if (I == MBB.end() || (!isUnpredicatedTerminator(*I) && !I->isBarrier() &&
                         !isDelayedSchedBarrier(*I)))
    return Success;

  // Count the number of terminators and find the first unconditional or
  // indirect branch.
  MachineBasicBlock::iterator FirstUncondOrIndirectBr = MBB.end();
  int NumTerminators = 0;
  for (auto J = I.getReverse(); J != MBB.rend() && isUnpredicatedTerminator(*J);
       J++) {
    auto &Term = *J;
    NumTerminators++;
    if (Term.getDesc().isUnconditionalBranch() ||
        Term.getDesc().isIndirectBranch()) {
      FirstUncondOrIndirectBr = J.getReverse();
    }
  }

  // If AllowModify is true, we can erase any terminators after
  // FirstUncondOrIndirectBR.
  if (AllowModify && FirstUncondOrIndirectBr != MBB.end() &&
      !isHardwareLoopJNZ(FirstUncondOrIndirectBr->getOpcode())) {
    while (std::next(FirstUncondOrIndirectBr) != MBB.end()) {
      auto &Dead = *std::next(FirstUncondOrIndirectBr);
      Dead.eraseFromParent();
      NumTerminators--;
    }
    I = FirstUncondOrIndirectBr;
  }

  // We can't handle blocks that end in an indirect branch.
  if (I->getDesc().isIndirectBranch())
    return Unhandled;

  // We can't handle blocks with more than 2 terminators.
  if (NumTerminators > 2)
    return Unhandled;

  // Handle a single unconditional branch.
  if (NumTerminators == 1 && I->getDesc().isUnconditionalBranch()) {
    // We might have unconditional branches to symbols that aren't
    // Basic Blocks because of tail call elimination.
    if (!I->getOperand(0).isMBB())
      return Unhandled;
    TBB = I->getOperand(0).getMBB();
    return Success;
  }

  // Handle a single conditional branch.
  if (NumTerminators == 1 && I->getDesc().isConditionalBranch()) {
    return parseCondBranch(*I, TBB, Cond) ? Success : Unhandled;
  }

  // Handle a conditional branch followed by an unconditional branch.
  if (NumTerminators == 2 && std::prev(I)->getDesc().isConditionalBranch() &&
      I->getDesc().isUnconditionalBranch()) {
    auto Prev = std::prev(I);
    if (!parseCondBranch(*Prev, TBB, Cond)) {
      return Unhandled;
    }

    // We might have unconditional branches to symbols that aren't
    // Basic Blocks because of tail call elimination.
    if (!I->getOperand(0).isMBB())
      return Unhandled;
    FBB = I->getOperand(0).getMBB();
    return Success;
  }

  if (AllowModify && I->getDesc().isUnconditionalBranch() &&
      isHardwareLoopJNZ(std::prev(I)->getOpcode())) {

    MachineBasicBlock *UncondDest = I->getOperand(0).getMBB();
    if (MBB.isLayoutSuccessor(UncondDest))
      removeBranch(MBB);
  }

  // Otherwise, we can't handle this.
  return Unhandled;
}

unsigned AIEBaseInstrInfo::removeBranch(MachineBasicBlock &MBB,
                                        int *BytesRemoved) const {
  if (BytesRemoved)
    *BytesRemoved = 0;
  MachineBasicBlock::iterator I = MBB.getLastNonDebugInstr();
  if (I == MBB.end())
    return 0;

  if (!I->getDesc().isUnconditionalBranch() &&
      !I->getDesc().isConditionalBranch())
    return 0;

  // Remove the branch.
  I->eraseFromParent();
  if (BytesRemoved)
    *BytesRemoved += getInstSizeInBytes(*I);

  I = MBB.end();

  if (I == MBB.begin())
    return 1;
  --I;
  if (!I->getDesc().isConditionalBranch())
    return 1;

  // Remove the branch.
  I->eraseFromParent();
  if (BytesRemoved)
    *BytesRemoved += getInstSizeInBytes(*I);
  return 2;
}

// Inserts a branch into the end of the specific MachineBasicBlock, returning
// the number of instructions inserted.
unsigned AIEBaseInstrInfo::insertBranch(
    MachineBasicBlock &MBB, MachineBasicBlock *TBB, MachineBasicBlock *FBB,
    ArrayRef<MachineOperand> Cond, const DebugLoc &DL, int *BytesAdded) const {
  if (BytesAdded)
    *BytesAdded = 0;

  // Shouldn't be a fall through.
  assert(TBB && "InsertBranch must not be told to insert a fallthrough");

  // Unconditional branch.
  if (Cond.empty()) {
    MachineInstr &MI = *BuildMI(&MBB, DL, get(getJumpOpcode())).addMBB(TBB);
    if (BytesAdded)
      *BytesAdded += getInstSizeInBytes(MI);
    return 1;
  }
  assert(Cond.size() == 2);

  // Cond[0] contains the branch opcode (JZ, JNZ, PseudoLoopEnd)
  // Cond[1] contains opcode dependent information, the last-bundle symbol for
  // PseudoLoopEnd, the condition register for JZ/JNZ
  unsigned Opc = Cond[0].getImm();
  MachineInstrBuilder CBranchBuilder = BuildMI(&MBB, DL, get(Opc));
  CBranchBuilder.add(Cond[1]).addMBB(TBB);
  if (BytesAdded)
    *BytesAdded += getInstSizeInBytes(*CBranchBuilder);

  // One-way conditional branch.
  if (!FBB)
    return 1;

  // Two-way conditional branch.
  MachineInstr &MI = *BuildMI(&MBB, DL, get(getJumpOpcode())).addMBB(FBB);
  if (BytesAdded)
    *BytesAdded += getInstSizeInBytes(MI);
  return 2;
}

// This primarily differs from the default implementation because
// the instructions used to manipulate the stack pointer during
// function prolog+apilog (PADDA, primarily) have a scheduling
// hazard.  Hence, these instructions need to be visible to
// instruction scheduling to ensure that the hazard is satisfied.
// The default implemenation would treat them as a scheduling
// boundary.
bool AIEBaseInstrInfo::isSchedulingBoundary(const MachineInstr &MI,
                                            const MachineBasicBlock *MBB,
                                            const MachineFunction &MF) const {
  // Terminators and labels can't be scheduled around.
  if (MI.isTerminator() || MI.isPosition())
    return true;

  if (isSchedBarrier(MI))
    return true;

  // INLINEASM_BR can jump to another block
  if (MI.getOpcode() == TargetOpcode::INLINEASM_BR)
    return true;

  return false;
}

bool AIEBaseInstrInfo::isCallBundle(MachineBasicBlock::iterator MII) const {
  MachineBasicBlock::const_instr_iterator I = ++MII->getIterator();
  MachineBasicBlock::instr_iterator E = MII->getParent()->instr_end();
  bool IsReturnAddr = false;
  while (I != E && I->isInsideBundle()) {
    MachineInstr *MI = const_cast<MachineInstr *>(&(*I));
    if (isCall(MI->getOpcode())) {
      IsReturnAddr = true;
      break;
    }
    I++;
  }
  return IsReturnAddr;
}

bool AIEBaseInstrInfo::isJumpBundle(MachineBasicBlock::iterator MII) const {
  MachineBasicBlock::const_instr_iterator I = ++MII->getIterator();
  MachineBasicBlock::instr_iterator E = MII->getParent()->instr_end();
  while (I != E && I->isInsideBundle()) {
    if (I->hasDelaySlot())
      return true;
    ++I;
  }
  return false;
}

bool AIEBaseInstrInfo::isZOLTripCountDef(const MachineInstr &MI,
                                         bool Pristine) const {
  auto ZOLSupport = getZOLSupport();
  return ZOLSupport && MI.getOpcode() == ZOLSupport->SetLoopCountOpcode &&
         MI.getOperand(0).getReg() == ZOLSupport->LCRegister &&
         (!Pristine || MI.getOperand(2).getImm() == 0);
}

bool AIEBaseInstrInfo::isZOLSetupBundle(MachineBasicBlock::iterator MII) const {
  MachineBasicBlock::const_instr_iterator I = ++MII->getIterator();
  MachineBasicBlock::instr_iterator E = MII->getParent()->instr_end();
  bool IsLoopStartSetup = false;
  while (I != E && I->isInsideBundle()) {
    MachineInstr *MI = const_cast<MachineInstr *>(&(*I));
    if (isZeroOverheadLoopSetupInstr(*MI)) {
      IsLoopStartSetup = true;
      break;
    }
    I++;
  }
  return IsLoopStartSetup;
}

unsigned AIEBaseInstrInfo::getLoopSetupDistance() const {
  auto ZOLSupport = getZOLSupport();
  assert(ZOLSupport);
  return ZOLSupport->LoopSetupDistance;
}

bool AIEBaseInstrInfo::isZeroOverheadLoopSetupInstr(
    const MachineInstr &MI) const {
  auto ZOLSupport = getZOLSupport();
  if (!ZOLSupport) {
    return false;
  }

  return isZOLTripCountDef(MI) ||
         ((MI.getOpcode() == ZOLSupport->SetLoopStartOpcode ||
           MI.getOpcode() == ZOLSupport->SetLoopEndOpcode) &&
          ((!ZOLSupport->LSRegister.has_value() &&
            !ZOLSupport->LERegister.has_value()) ||
           (ZOLSupport->LSRegister.has_value() &&
            MI.getOperand(0).getReg() == *ZOLSupport->LSRegister) ||
           (ZOLSupport->LERegister.has_value() &&
            MI.getOperand(0).getReg() == *ZOLSupport->LERegister)));
}

const MachineInstr *
AIEBaseInstrInfo::findZOLTripCountDef(const MachineBasicBlock &MBB,
                                      bool Pristine) const {
  for (auto It = MBB.instr_rbegin(), End = MBB.instr_rend(); It != End; ++It) {
    if (isZOLTripCountDef(*It, Pristine))
      return &*It;
  }
  return nullptr;
}

MachineInstr *AIEBaseInstrInfo::findZOLTripCountDef(MachineBasicBlock &MBB,
                                                    bool Pristine) const {
  return const_cast<MachineInstr *>(findZOLTripCountDef(
      static_cast<const MachineBasicBlock &>(MBB), Pristine));
}

void AIEBaseInstrInfo::adjustTripCount(MachineInstr &MI, int Adjustment) const {
  assert(isZOLTripCountDef(MI));
  auto &Imm = MI.getOperand(2);
  Imm.setImm(Imm.getImm() + Adjustment);
}

bool AIEBaseInstrInfo::isLoopVersionThresholdDef(const MachineInstr &MI) const {
  const auto Opcode = getLoopVersionThresholdOpcode();
  return Opcode && MI.getOpcode() == *Opcode;
}

void AIEBaseInstrInfo::setLoopVersionThreshold(MachineInstr &MI,
                                               int MinTripCount) const {
  assert(isLoopVersionThresholdDef(MI));
  // The threshold materializes into a narrow scalar-move immediate; enforce the
  // narrowest field width (AIE2's simm10) across subtargets. A stage count is
  // tiny in practice, so this only trips on a pathological schedule. Checked
  // unconditionally, not just under assert: a truncated threshold would select
  // the high-trip-count copy for trip counts too small for the schedule, a
  // miscompile.
  if (!isInt<10>(MinTripCount))
    report_fatal_error("PseudoLoopVersionThreshold: minimum trip count does "
                       "not fit the scalar-move immediate");
  MI.getOperand(1).setImm(MinTripCount);
}

bool AIEBaseInstrInfo::isHardwareLoopStart(unsigned Opcode) const {
  const auto ZOLSupport = getZOLSupport();
  return ZOLSupport && Opcode == ZOLSupport->LoopStartOpcode;
}

bool AIEBaseInstrInfo::isHardwareLoopEnd(unsigned Opcode) const {
  const auto ZOLSupport = getZOLSupport();
  return ZOLSupport && Opcode == ZOLSupport->LoopEndOpcode;
}

bool AIEBaseInstrInfo::isHardwareLoopDec(unsigned Opcode) const {
  const auto JNZDSupport = getJNZDSupport();
  return JNZDSupport && Opcode == JNZDSupport->LoopDecOpcode;
}

bool AIEBaseInstrInfo::isHardwareLoopJNZ(unsigned Opcode) const {
  const auto JNZDSupport = getJNZDSupport();
  return JNZDSupport && Opcode == JNZDSupport->LoopJNZOpcode;
}

// Look for the last LoopSetup Bundle.
bool AIEBaseInstrInfo::isLastZOLSetupBundleInMBB(
    MachineBasicBlock::iterator MII) const {
  MachineBasicBlock *MBB = MII->getParent();
  for (auto MI = std::next(MII), End = MBB->end(); MI != End; ++MI) {
    if (isZOLSetupBundle(MI))
      return false;
  }
  return true;
}

// Compute the total size (in bytes) of all instruction bundles in the
// pre-header that follow the last ZOL setup instruction + the number of
// bundles.
std::pair<unsigned, unsigned>
AIEBaseInstrInfo::getPostZOLRegionSizeInfo(MachineBasicBlock &MBB) const {
  unsigned Size = 0;
  unsigned BundleCount = 0;
  for (auto &MI : llvm::reverse(MBB)) {
    if (MI.isDebugInstr())
      continue;

    // We iterate backwards so this is the last setup bundle in the block.
    if (isZOLSetupBundle(&MI)) {
      BundleCount++;
      Size += getAIEMachineBundleSize(MI);
      break;
    }

    if (MI.isBundle())
      BundleCount++;

    Size += getAIEMachineBundleSize(MI);
  }
  return std::make_pair(BundleCount, Size);
}

// Return true if this is ZeroOverhead loop body.
bool AIEBaseInstrInfo::isZOLBody(const MachineBasicBlock &MBB) const {
  auto Last = MBB.getLastNonDebugInstr();

  // If MBB is empty or has no non-debug instructions, return false.
  if (Last == MBB.end())
    return false;

  return isHardwareLoopEnd(Last->getOpcode());
}

// Count the number of Machine Bundles in a MachineBasicBlock.
unsigned
AIEBaseInstrInfo::getZOLBundlesCount(const MachineBasicBlock &MBB) const {
  if (!isZOLBody(MBB))
    return 0;

  auto First = MBB.getFirstNonDebugInstr();
  auto Last = MBB.getLastNonDebugInstr();

  return std::count_if(
      First, Last, [](const MachineInstr &MI) { return !MI.isDebugInstr(); });
}

unsigned AIEBaseInstrInfo::getRegionSizeInBytes(
    llvm::iterator_range<MachineBasicBlock::iterator> Region) const {
  unsigned Size = 0;
  LLVM_DEBUG(dbgs() << "---Region Begin---\n");
  for (auto It = Region.begin(), End = Region.end(); It != End; ++It) {
    Size += getAIEMachineBundleSize(It);
  }
  LLVM_DEBUG(dbgs() << "---Region End---\n");
  LLVM_DEBUG(dbgs() << "Region Size" << " " << Size << "\n");
  return Size;
}

const AIE::MachineBundle AIEBaseInstrInfo::getAIEMachineBundle(
    const MachineBasicBlock::iterator MII) const {
  AIE::MachineBundle Bundle(getFormatInterface());
  // Iterate over the instructions in the bundle.
  MachineBasicBlock::const_instr_iterator I = ++MII->getIterator();
  MachineBasicBlock::instr_iterator E = MII->getParent()->instr_end();
  while (I != E && I->isInsideBundle()) {
    MachineInstr *MI = const_cast<MachineInstr *>(&(*I));
    Bundle.add(MI);
    I++;
  }
  return Bundle;
}

const AIE::ConstMachineBundle AIEBaseInstrInfo::getAIEMachineBundle(
    const MachineBasicBlock::const_iterator MII) const {

  AIE::ConstMachineBundle Bundle = (getFormatInterface());
  // Iterate over the instructions in the bundle.
  MachineBasicBlock::const_instr_iterator I = ++MII->getIterator();
  MachineBasicBlock::const_instr_iterator E = MII->getParent()->instr_end();
  while (I != E && I->isInsideBundle()) {
    MachineInstr *MI = const_cast<MachineInstr *>(&(*I));
    Bundle.add(MI);
    I++;
  }
  return Bundle;
}

unsigned AIEBaseInstrInfo::getAIEMachineBundleSize(
    const MachineBasicBlock::const_iterator MII) const {
  if (MII->isBundle()) {
    AIE::ConstMachineBundle Bundle = getAIEMachineBundle(MII);
    const VLIWFormat *Format = Bundle.getFormatOrNull();
    assert(Format);
    LLVM_DEBUG(dbgs() << Format->Name << "\n");
    return Format->getSize();
  }
  return 0;
}

// TODO: implement folding for opcodes other than COPY
bool AIEBaseInstrInfo::foldImmediate(MachineInstr &UseMI, MachineInstr &DefMI,
                                     Register Reg,
                                     MachineRegisterInfo *MRI) const {

  if (AIEDisableFoldImm)
    return false;

  // Only handle COPY instructions as the use
  if (!UseMI.isCopy())
    return false;

  // Check if DefMI is a move-immediate instruction
  if (!isIConst(DefMI.getOpcode()))
    return false;

  // Bail out if operand 1 is not an immediate (e.g., a GlobalAddress
  // relocation)
  if (!DefMI.getOperand(1).isImm())
    return false;

  int64_t ImmVal = DefMI.getOperand(1).getImm();

  // Get the destination register of the COPY
  Register DstReg = UseMI.getOperand(0).getReg();

  // Only handle virtual registers - physical registers are more complex
  if (!DstReg.isVirtual())
    return false;

  ++NumFoldImmAttempts;

  // Only fold when the constant has a single non-debug use.
  // The TargetInstrInfo::foldImmediate contract lets the caller (PeepholeOpt)
  // erase DefMI when hasOneNonDBGUse(Reg) holds; without this guard we leave
  // DefMI alive for other consumers and end up materializing the constant
  // twice, inflating register pressure.
  if (AIEFoldImmRequireOneUse && !MRI->hasOneNonDBGUse(Reg)) {
    ++NumFoldImmBlockedMultiUse;
    return false;
  }

  // Get the appropriate move-immediate opcode for the destination register.
  // Bail out if no suitable opcode exists for this register/immediate combo.
  APInt ImmAPInt(32, ImmVal, /*isSigned=*/true);
  std::optional<unsigned> NewOpc = getConstantMovOpcode(*MRI, DstReg, ImmAPInt);
  if (!NewOpc)
    return false;

  // The returned opcode may require a register class that is wider (32-bit)
  // than the current DstReg class (e.g. 20-bit pointer classes). Constrain
  // DstReg to the intersection of its current class and the instruction's
  // operand class. This handles e.g. eP → eP_as_32Bit for MOVXM.
  // If constraining fails, DstReg is in an incompatible class (e.g. a control
  // register) and we must bail out.
  const MCInstrDesc &NewMCID = get(*NewOpc);
  const TargetRegisterInfo *TRI = MRI->getTargetRegisterInfo();
  const MachineFunction &MF = *UseMI.getParent()->getParent();
  if (const TargetRegisterClass *OpRC = getRegClass(NewMCID, 0, TRI, MF)) {
    if (!MRI->constrainRegClass(DstReg, OpRC))
      return false;
  }

  MachineBasicBlock &MBB = *UseMI.getParent();
  const DebugLoc &DL = UseMI.getDebugLoc();
  BuildMI(MBB, UseMI, DL, NewMCID, DstReg).addImm(ImmAPInt.getSExtValue());

  // Remove the old COPY
  UseMI.eraseFromParent();

  ++NumFoldImmSuccesses;
  return true;
}

unsigned
AIEBaseInstrInfo::getMBBSizeInBytes(const MachineBasicBlock &MBB) const {
  unsigned Size = 0;
  for (auto &MI : MBB) {
    Size += getAIEMachineBundleSize(&MI);
  }
  return Size;
}

unsigned computeRegStateFlags(const MachineOperand &RegOp) {
  assert(RegOp.isReg() && "Not a register operand");
  assert(!RegOp.getSubReg() && "RegOp has SubReg flags set");
  return getDefRegState(RegOp.isDef()) | getKillRegState(RegOp.isKill()) |
         getDeadRegState(RegOp.isDead()) | getUndefRegState(RegOp.isUndef());
}

class SpillExpandHelper {
  LivePhysRegs LRs;
  MachineInstr &MI;

  void computeLiveInsAt() {
    // Collect liveins phys regs at MI
    LRs.addLiveIns(*MI.getParent());
    auto LivenessEnd = MachineBasicBlock::iterator(MI);
    SmallVector<std::pair<MCPhysReg, const MachineOperand *>, 2> Clobbers;
    for (MachineInstr &LiveMI :
         make_range(MI.getParent()->begin(), LivenessEnd))
      LRs.stepForward(LiveMI, Clobbers);
  }
  void computeLiveOutsAt() {
    // Collect liveouts phys regs at MI
    LRs.addLiveOuts(*MI.getParent());
    auto LivenessEnd = MachineBasicBlock::iterator(MI).getReverse();
    for (MachineInstr &LiveMI :
         make_range(MI.getParent()->rbegin(), LivenessEnd))
      LRs.stepBackward(LiveMI);
  }
  bool tracksLiveness() const {
    return MI.getMF()->getProperties().hasProperty(
        MachineFunctionProperties::Property::TracksLiveness);
  }

public:
  SpillExpandHelper(MachineInstr &MI, const TargetRegisterInfo &TRI)
      : LRs(TRI), MI(MI) {
    LLVM_DEBUG(llvm::dbgs() << "Collecting Live phys regs at " << MI);
    if (!tracksLiveness())
      return;
    if (MI.mayStore())
      computeLiveInsAt();
    else
      computeLiveOutsAt();
    LLVM_DEBUG(LRs.dump());
  }

  bool isLiveReg(Register Reg) {
    // If the physreg is not available, it means it's at least partially
    // defined, in which case it must be spilled/reloaded.
    return !tracksLiveness() || !LRs.available(MI.getMF()->getRegInfo(), Reg);
  }
};

void AIEBaseInstrInfo::expandSpillPseudo(
    MachineInstr &MI, const TargetRegisterInfo &TRI, Align SubRegOffsetAlign,
    Register SPReg, std::optional<int64_t> OffsetVal) const {
  auto ExpandInfos = getSpillPseudoExpandInfo(TRI, MI);
  if (ExpandInfos.empty()) {
    // Nothing to expand
    return;
  }

  const MachineOperand &RegOp = MI.getOperand(0);
  Register Reg = RegOp.getReg();
  assert(Reg.isPhysical() && "Expected physical register for spill expansion");
  unsigned RegFlags = computeRegStateFlags(RegOp);
  int64_t Offset =
      OffsetVal.has_value() ? *OffsetVal : MI.getOperand(1).getImm();
  auto &MBB = *MI.getParent();
  auto DL = MI.getDebugLoc();

  // Provide a memory operand, resolving the location from other memory refs
  // during scheduling graph generation.
  auto CreateMMO = [&MF = *MBB.getParent(), &MI](int64_t Offset, unsigned Size,
                                                 Align SubRegOffsetAlign) {
    assert(MI.hasOneMemOperand());
    const MachineMemOperand *MMO = *(MI.memoperands().begin());
    return MF.getMachineMemOperand(MMO, Offset, Size);
  };

  SpillExpandHelper SEH(MI, TRI);

  int64_t MemOffset = Offset;
  SmallVector<MachineInstr *, 4> ExpandedInsts;
  for (AIEPseudoExpandInfo EI : ExpandInfos) {
    unsigned MemorySize = 0;
    if (EI.MemSize) {
      MemorySize = EI.MemSize;
    } else {
      const TargetRegisterClass *RC = TRI.getMinimalPhysRegClass(Reg);
      if (EI.SubRegIndex) {
        RC = TRI.getSubRegisterClass(RC, EI.SubRegIndex);
        assert(RC && "Missing sub-register class for spill expansion");
      }
      MemorySize = TRI.getSpillSize(*RC);
    }
    assert(MemorySize && "Spill instruction uses no memory ?!");

    // Only expand into an instruction if the register is live, or if the
    // instruction cannot be removed due to ordering.
    Register SubReg =
        EI.SubRegIndex ? Register(TRI.getSubReg(Reg, EI.SubRegIndex)) : Reg;
    if (MI.hasOrderedMemoryRef() || SEH.isLiveReg(SubReg)) {
      MachineInstrBuilder MIB;
      // If the SPReg is set, we need to spill using register offset. However,
      // for pseudo instructions, we skip this step since we will come back to
      // it once we expand the pseudo instruction into multiple instructions.
      if (SPReg && !get(EI.ExpandedOpCode).isPseudo()) {
        auto OffsetRegInfo =
            getRegOffsetSpillInstrInfoFromImmOffset(EI.ExpandedOpCode);
        MachineFunction &MF = *MI.getMF();
        Register OffsetReg =
            MF.getRegInfo().createVirtualRegister(OffsetRegInfo.OffsetRC);
        BuildMI(MBB, MI, DL, get(OffsetRegInfo.AdjustOffsetOpcode), OffsetReg)
            .addImm(MemOffset);
        MIB = BuildMI(MBB, MI, DL, get(OffsetRegInfo.SpillOpCode))
                  .addReg(SubReg, RegFlags)
                  .addReg(SPReg)
                  .addReg(OffsetReg);
      } else {
        MIB = BuildMI(MBB, MI, DL, get(EI.ExpandedOpCode))
                  .addReg(SubReg, RegFlags)
                  .addImm(MemOffset);
      }

      // If safe, refine the size and offset of the MMO while maintaining the
      // MachinePointerInfo location.
      if (MI.hasOneMemOperand() && EI.SubRegIndex) {
        MIB.addMemOperand(
            CreateMMO(MemOffset - Offset, MemorySize, SubRegOffsetAlign));
      } else {
        // Otherwise, just keep what was there.
        MIB.cloneMemRefs(MI);
      }

      // If the subreg isn't live but must still be spilled, add undef.
      if (RegOp.isUse() && !SEH.isLiveReg(SubReg)) {
        MIB->getOperand(0).setIsUndef();
      }
      ExpandedInsts.push_back(MIB.getInstr());
    }

    // Increment the offset for the next spill instruction
    MemOffset += int64_t(MemorySize);
  }

  MI.eraseFromParent();

  // The expanded instructions might need to be expanded again.
  // E.g. In AIE2 eDS spills are expanded into 2 eD spills, which are themselves
  // expanded into 4 other spills.
  for (MachineInstr *ExpandedMI : ExpandedInsts)
    expandSpillPseudo(*ExpandedMI, TRI, SubRegOffsetAlign, SPReg);
}

static bool matchesCopyRecipe(const CopyRecipe &Recipe, MCRegister DstReg,
                              MCRegister SrcReg) {
  return Recipe.DstRC->contains(DstReg) && Recipe.SrcRC->contains(SrcReg);
}

static const CopyRecipe *findCopyRecipe(const CopyTableView &CopyTable,
                                        MCRegister DstReg, MCRegister SrcReg) {
  for (const CopyRecipe &Recipe : CopyTable.Recipes)
    if (matchesCopyRecipe(Recipe, DstReg, SrcReg))
      return &Recipe;
  return nullptr;
}

const CopyTableView &AIEBaseInstrInfo::getCopyTable() const {
  static constexpr CopyTableView Empty{};
  return Empty;
}

void AIEBaseInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                   MachineBasicBlock::iterator MBBI,
                                   const DebugLoc &DL, Register DstReg,
                                   Register SrcReg, bool KillSrc,
                                   bool /* RenamableDest */,
                                   bool /* RenamableSrc */) const {
  const TargetRegisterInfo &TRI =
      *MBB.getParent()->getRegInfo().getTargetRegisterInfo();
  if (!materializeCopyFromTable(MBB, MBBI, DL, DstReg, SrcReg, KillSrc)) {
    errs() << "copyPhysReg: cannot copy " << TRI.getName(SrcReg) << " -> "
           << TRI.getName(DstReg) << '\n';
    llvm_unreachable("unhandled case in copyPhysReg");
  }
}

bool AIEBaseInstrInfo::materializeCopyFromTable(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI,
    const DebugLoc &DL, MCRegister DstReg, MCRegister SrcReg,
    bool KillSrc) const {
  const CopyTableView &CopyTable = getCopyTable();
  const CopyRecipe *MatchedRecipe = findCopyRecipe(CopyTable, DstReg, SrcReg);
  if (!MatchedRecipe)
    return false;

  assert(MatchedRecipe->FirstCopy + MatchedRecipe->NumCopies <=
             CopyTable.Tuples.size() &&
         "copy recipe exceeds tuple table");
  const TargetRegisterInfo &TRI =
      *MBB.getParent()->getRegInfo().getTargetRegisterInfo();
  const CopyTuple *Tuples = CopyTable.Tuples.data() + MatchedRecipe->FirstCopy;
  for (const CopyTuple &Tuple : ArrayRef(Tuples, MatchedRecipe->NumCopies)) {
    Register CopyDst =
        Tuple.DstSubRegIdx ? TRI.getSubReg(DstReg, Tuple.DstSubRegIdx) : DstReg;
    Register CopySrc =
        Tuple.SrcSubRegIdx ? TRI.getSubReg(SrcReg, Tuple.SrcSubRegIdx) : SrcReg;
    assert(CopyDst && CopySrc && "copy tuple names an invalid subregister");
    BuildMI(MBB, MBBI, DL, get(Tuple.MoveOpcode), CopyDst)
        .addReg(CopySrc, getKillRegState(KillSrc));
  }
  return true;
}

std::optional<unsigned> AIEBaseInstrInfo::getCopyCost(MCRegister DstReg,
                                                      MCRegister SrcReg) const {
  const CopyRecipe *Recipe = findCopyRecipe(getCopyTable(), DstReg, SrcReg);
  return Recipe ? std::optional(Recipe->NumCopies) : std::nullopt;
}

static bool isPreRA(const MachineFunction &MF) {
  return !MF.getProperties().hasProperty(
      MachineFunctionProperties::Property::NoVRegs);
}

// Reminder: this is used to compute the latency of RAW edges only.
std::optional<unsigned> AIEBaseInstrInfo::getOperandLatency(
    const InstrItineraryData *ItinData, const MachineInstr &DefMI,
    unsigned DefIdx, const MachineInstr &UseMI, unsigned UseIdx) const {
  std::optional<int> Lat = getSignedOperandLatency(ItinData, DefMI, DefIdx,
                                                   UseMI, UseIdx, SDep::Data);
  if (Lat) {
    // We have to maintain the semantics here and "cap" negative latencies at 0.
    int SignedLat = *Lat;
    return std::max(0, SignedLat);
  }

  // We are lacking scheduling info for the instructions, return an optimistic
  // value for the latency.
  assert(isPreRA(*DefMI.getMF()));
  return Lat;
}

std::optional<int> AIEBaseInstrInfo::getSignedOperandLatency(
    const InstrItineraryData *ItinData, const MachineInstr &SrcMI,
    unsigned SrcOpIdx, const MachineInstr &DstMI, unsigned DstOpIdx,
    SDep::Kind Kind) const {
  // We mostly copy the default implementation of getOperandLatency, but we
  // support WAR and WAW dependencies as well.
  // We also look a bit closer at zero-cost instructions to help pre-RA sched.

  // Use resolved schedule classes based on register classes to get correct
  // bypass information and operand latencies.
  const MachineRegisterInfo &MRI = SrcMI.getMF()->getRegInfo();
  const unsigned SrcClass = getSchedClass(
      SrcMI.getDesc(), make_range(SrcMI.operands_begin(), SrcMI.operands_end()),
      MRI);
  const unsigned DstClass = getSchedClass(
      DstMI.getDesc(), make_range(DstMI.operands_begin(), DstMI.operands_end()),
      MRI);
  std::optional<unsigned> SrcCycle =
      ItinData->getOperandCycle(SrcClass, SrcOpIdx);
  std::optional<unsigned> DstCycle =
      ItinData->getOperandCycle(DstClass, DstOpIdx);

  // This architecture has strict scheduling requirements. We require
  // itineraries for all "real" instructions.
  // For pre-RA scheduling, we cannot expect all instructions to have
  // itineraries as there still are generic opcodes.
  if (!isPreRA(*SrcMI.getMF())) {

    // In post-RA scheduling, "meta" instructions scheduled in a cycle are
    // always moved after the "real" instructions in that cycle. The latencies
    // below are meant to keep the natural ordering of the dependence.
    // Note this is purely cosmetic. If the instructions were to be re-ordered,
    // nothing would really happen because meta instructions are not emitted.
    // See schedule/meta_instrs.mir.
    if (SrcMI.isMetaInstruction() || isHardwareLoopStart(SrcMI.getOpcode()))
      return 1;
    if (DstMI.isMetaInstruction() || isHardwareLoopStart(DstMI.getOpcode()))
      return 0;

    if (SrcClass == 0 && !SrcMI.isMetaInstruction()) {
      LLVM_DEBUG(llvm::dbgs()
                 << "Warning!: no Scheduling class for " << SrcMI << "\n");
      report_fatal_error("Missing scheduling info.");
    }
    if (DstClass == 0 && !DstMI.isMetaInstruction()) {
      LLVM_DEBUG(llvm::dbgs()
                 << "Warning!: no Scheduling class for " << DstMI << "\n");
      report_fatal_error("Missing scheduling info.");
    }
    if (!SrcCycle && !SrcMI.isMetaInstruction()) {
      LLVM_DEBUG(llvm::dbgs()
                 << "Warning!: Scheduling class contains no information for "
                 << "Operand " << SrcOpIdx << " of Src " << SrcMI);
      report_fatal_error("Missing scheduling info.");
    }
    if (!DstCycle && !DstMI.isMetaInstruction()) {
      LLVM_DEBUG(llvm::dbgs()
                 << "Warning!: Scheduling class contains no information for "
                 << "Operand " << DstOpIdx << " of Dst " << DstMI);
      report_fatal_error("Missing scheduling info.");
    }
  }

  // Zero cost instructions like INSERT_SUBREG used as def or use will return
  // an unknown latency in the standard itinerary logic, which would be max-ed
  // to 0. By setting the use cycle to the earliest possible, we make sure that
  // the result operand latency of late writers is respected. As an example.
  // VLDA -> INSERT_SUBREG -> VMAX_LT wouldn't resepect the load latency in
  // pre-regalloc scheduling, since both edges get zero latency.
  //
  // Note that this takes the hit of a transitive latency on this intermediate
  // zero-cost node conservatively; a late reading user of DstMI will be pushed
  // later than necessary.
  if (Kind == SDep::Data && !DstCycle && isZeroCost(DstMI.getOpcode())) {
    DstCycle = 1;
  }

  if (!SrcCycle || !DstCycle) {
    LLVM_DEBUG(llvm::dbgs() << "Warning!: Missing scheduling info:\n"
                            << "  From Op #" << SrcOpIdx << " of " << SrcMI
                            << "  To   Op #" << DstOpIdx << " of " << DstMI);
    return std::nullopt;
  }

  int SrcCycleVal = *SrcCycle;
  int DstCycleVal = *DstCycle;
  if (Kind == SDep::Data) {
    // Typical bypass case: data from a producer is available earlier for a
    // consumer.
    SrcCycleVal -=
        getNumBypassedCycles(ItinData, SrcClass, SrcOpIdx, DstClass, DstOpIdx);
  }
  if (Kind == SDep::Anti) {
    // "Reverse" case: we need to penalize WAR dependencies, because if the
    // bypass is taken by the write, then the read could get the newly-written
    // value, not respecting the WAR edge. See negative_latencies/bypass.mir
    DstCycleVal -=
        getNumBypassedCycles(ItinData, DstClass, DstOpIdx, SrcClass, SrcOpIdx);
  }

  int Diff = SrcCycleVal - DstCycleVal;
  int Latency = Kind == SDep::Anti ? (Diff) : (Diff + 1);
  return Latency;
}

unsigned AIEBaseInstrInfo::getNumBypassedCycles(
    const InstrItineraryData *ItinData, unsigned DefSchedClass, unsigned DefIdx,
    unsigned UseSchedClass, unsigned UseIdx) const {

  // FIXME: This assumes one cycle benefit for every pipeline forwarding.
  return ItinData->hasPipelineForwarding(DefSchedClass, DefIdx, UseSchedClass,
                                         UseIdx)
             ? 1
             : 0;
}

int AIEBaseInstrInfo::getConservativeMemoryLatency(
    unsigned SrcSchedClass) const {
  if (!AccurateMemEdges)
    return 1;

  std::optional<int> SrcCycle = getLastMemoryCycle(SrcSchedClass);
  int WorstDstCycle = getMinFirstMemoryCycle();

  return SrcCycle ? *SrcCycle - WorstDstCycle + 1 : 0;
}

std::optional<int>
AIEBaseInstrInfo::getMemoryLatency(unsigned SrcSchedClass,
                                   unsigned DstSchedClass) const {
  if (!AccurateMemEdges)
    return 1;

  std::optional<int> SrcCycle = getLastMemoryCycle(SrcSchedClass);
  std::optional<int> DstCycle = getFirstMemoryCycle(DstSchedClass);

  if (!SrcCycle || !DstCycle)
    return std::nullopt;

  return (int)(*SrcCycle) - (int)(*DstCycle) + 1;
}

std::optional<int>
AIEBaseInstrInfo::getFirstMemoryCycle(unsigned SchedClass) const {
  return std::nullopt;
}

std::optional<int>
AIEBaseInstrInfo::getLastMemoryCycle(unsigned SchedClass) const {
  return std::nullopt;
}

int AIEBaseInstrInfo::getMinFirstMemoryCycle() const {
  return std::numeric_limits<unsigned>().max();
}

int AIEBaseInstrInfo::getMaxFirstMemoryCycle() const {
  return std::numeric_limits<unsigned>().min();
}

int AIEBaseInstrInfo::getMinLastMemoryCycle() const {
  return std::numeric_limits<unsigned>().max();
}

int AIEBaseInstrInfo::getMaxLastMemoryCycle() const {
  return std::numeric_limits<unsigned>().min();
}

SmallVector<int, 2>
AIEBaseInstrInfo::getMemoryCycles(unsigned SchedClass) const {
  return {};
}
/// FIXME: Delays for locks to reach the core aren't completely described in
/// the ISA. The numbers are therefore conservative.
int AIEBaseInstrInfo::getCoreStallCycleAfterLock() const { return 2; }
int AIEBaseInstrInfo::getCoreResumeCycleAfterLock() const { return 8; }
int AIEBaseInstrInfo::getCoreResumeCycleAfterAcquireStore() const {
  return getCoreResumeCycleAfterLock();
}
int AIEBaseInstrInfo::getCoreResumeCycleAfterAcquireLoad() const {
  return getCoreResumeCycleAfterLock();
}
int AIEBaseInstrInfo::getCoreResumeCycleAfterReleaseStore() const {
  return getCoreResumeCycleAfterLock();
}
int AIEBaseInstrInfo::getCoreResumeCycleAfterReleaseLoad() const {
  return getCoreResumeCycleAfterLock();
}

std::optional<int>
AIEBaseInstrInfo::getLockResumeDelay(const MachineInstr &Lock,
                                     const MachineInstr &MemMI) const {
  const bool IsLoad = MemMI.mayLoad();
  const bool IsStore = MemMI.mayStore();
  // The resume window only constrains memory operations.
  if (!IsLoad && !IsStore)
    return std::nullopt;

  const unsigned SchedClass = getSchedClass(MemMI.getDesc(), MemMI.operands(),
                                            MemMI.getMF()->getRegInfo());

  // TM ops hold no lckLdaRsrc/lckStRsrc tokens, so the resume-cycle formula
  // does not apply; only same-bundle issue must be prevented.
  if (isTMAccessSchedClass(SchedClass))
    return 1;

  // some Instructions have mayLoadOrStore but don't access memory, e.g., stream
  // ss moves
  std::optional<int> OptFirstMemCycle =
      getFirstMemoryCycleForLockOrdering(SchedClass);
  if (!OptFirstMemCycle)
    return std::nullopt;

  // Handle partword stores, that load and store
  const bool IsLoadOnly = IsLoad && !IsStore;
  const int AcquireResumeCycle = IsLoad ? getCoreResumeCycleAfterAcquireLoad()
                                        : getCoreResumeCycleAfterAcquireStore();
  const int ReleaseResumeCycle = IsLoadOnly
                                     ? getCoreResumeCycleAfterReleaseLoad()
                                     : getCoreResumeCycleAfterReleaseStore();
  const int CoreResumeCycle = isAcquire(Lock.getOpcode()) ? AcquireResumeCycle
                              : isRelease(Lock.getOpcode())
                                  ? ReleaseResumeCycle
                                  : getCoreResumeCycleAfterLock();

  const bool IsStoreOnly = IsStore && !IsLoad;
  const int FirstIssueMemCycle =
      isRelease(Lock.getOpcode()) && IsStoreOnly
          ? std::min(*OptFirstMemCycle, ReleaseResumeCycle)
          : *OptFirstMemCycle;
  return CoreResumeCycle - FirstIssueMemCycle + 1;
}

std::optional<int>
AIEBaseInstrInfo::getLockStallDelay(const MachineInstr &MemMI) const {
  // The stall window only constrains memory operations.
  if (!MemMI.mayLoadOrStore())
    return std::nullopt;

  // Ensure the memory operation retires before the core stalls. For load-only
  // instructions, use the lock-ordering cycle (write-back stage) rather than
  // the bare memory-access cycle, since the load data must be fully committed
  // before the lock can observe it. nullopt means no backward lock-ordering
  // constraint for this class (e.g. vmovx ss has mayLoadOrStore but does not
  // access memory).
  const bool IsLoadOnly = MemMI.mayLoad() && !MemMI.mayStore();
  std::optional<int> OptLastMemCycle = getLastMemoryCycleForLockOrdering(
      getSchedClass(MemMI.getDesc(),
                    make_range(MemMI.operands_begin(), MemMI.operands_end()),
                    MemMI.getMF()->getRegInfo()),
      IsLoadOnly);
  if (!OptLastMemCycle)
    return std::nullopt;

  return *OptLastMemCycle - getCoreStallCycleAfterLock() + 1;
}

/// Walk pointer casts / inbounds GEPs to an identified object (alloca, global,
/// noalias argument, noalias call, ...).
static const Value *getIdentifiedObject(const Value *V) {
  if (!V)
    return nullptr;

  V = V->stripPointerCasts();
  while (const auto *GEP = dyn_cast<GEPOperator>(V)) {
    if (!GEP->isInBounds())
      return nullptr;
    V = GEP->getPointerOperand()->stripPointerCasts();
  }

  return isIdentifiedObject(V) ? V : nullptr;
}

/// Part-word stores may elide lock delays only when lock and store pointers
/// trace to distinct IR objects that carry source-level noalias (restrict).
static bool
partWordStoreLockMayUseAliasAnalysis(const MachineMemOperand *LockMMO,
                                     const MachineMemOperand *MemMMO) {
  const Value *LockObj = getIdentifiedObject(LockMMO->getValue());
  const Value *MemObj = getIdentifiedObject(MemMMO->getValue());
  if (!LockObj || !MemObj || LockObj == MemObj)
    return false;

  auto HasSourceNoAlias = [](const Value *Identified) {
    if (const auto *A = dyn_cast<Argument>(Identified))
      return A->hasNoAliasAttr();
    return isNoAliasCall(Identified);
  };
  return HasSourceNoAlias(LockObj) && HasSourceNoAlias(MemObj);
}

// Derived from MachineInstr::mayAlias MMO-pair logic.
static bool memOperandsMayAlias(const MachineFrameInfo &MFI, AAResults *AA,
                                bool UseTBAA, const MachineMemOperand *MMOa,
                                const MachineMemOperand *MMOb) {
  int64_t OffsetA = MMOa->getOffset();
  int64_t OffsetB = MMOb->getOffset();
  int64_t MinOffset = std::min(OffsetA, OffsetB);

  LocationSize WidthA = MMOa->getSize();
  LocationSize WidthB = MMOb->getSize();
  bool KnownWidthA = WidthA.hasValue();
  bool KnownWidthB = WidthB.hasValue();
  bool BothMMONonScalable = !WidthA.isScalable() && !WidthB.isScalable();

  const Value *ValA = MMOa->getValue();
  const Value *ValB = MMOb->getValue();
  bool SameVal = (ValA && ValB && (ValA == ValB));
  if (!SameVal) {
    const PseudoSourceValue *PSVa = MMOa->getPseudoValue();
    const PseudoSourceValue *PSVb = MMOb->getPseudoValue();
    if (PSVa && ValB && !PSVa->mayAlias(&MFI))
      return false;
    if (PSVb && ValA && !PSVb->mayAlias(&MFI))
      return false;
    if (PSVa && PSVb && (PSVa == PSVb))
      SameVal = true;
  }

  if (SameVal && BothMMONonScalable) {
    if (!KnownWidthA || !KnownWidthB)
      return true;
    int64_t MaxOffset = std::max(OffsetA, OffsetB);
    int64_t LowWidth = (MinOffset == OffsetA)
                           ? WidthA.getValue().getKnownMinValue()
                           : WidthB.getValue().getKnownMinValue();
    return (MinOffset + LowWidth > MaxOffset);
  }

  if (!AA)
    return true;

  if (!ValA || !ValB)
    return true;

  assert((OffsetA >= 0) && "Negative MachineMemOperand offset");
  assert((OffsetB >= 0) && "Negative MachineMemOperand offset");

  if ((WidthA.isScalable() && OffsetA > 0) ||
      (WidthB.isScalable() && OffsetB > 0))
    return true;

  int64_t OverlapA =
      KnownWidthA ? WidthA.getValue().getKnownMinValue() + OffsetA - MinOffset
                  : MemoryLocation::UnknownSize;
  int64_t OverlapB =
      KnownWidthB ? WidthB.getValue().getKnownMinValue() + OffsetB - MinOffset
                  : MemoryLocation::UnknownSize;

  LocationSize LocA = (WidthA.isScalable() || !KnownWidthA)
                          ? WidthA
                          : LocationSize::precise(OverlapA);
  LocationSize LocB = (WidthB.isScalable() || !KnownWidthB)
                          ? WidthB
                          : LocationSize::precise(OverlapB);

  return !AA->isNoAlias(
      MemoryLocation(ValA, LocA, UseTBAA ? MMOa->getAAInfo() : AAMDNodes()),
      MemoryLocation(ValB, LocB, UseTBAA ? MMOb->getAAInfo() : AAMDNodes()));
}

bool AIEBaseInstrInfo::mayLockOrderWithMemOp(const MachineInstr &Lock,
                                             const MachineInstr &Mem,
                                             AAResults *AA,
                                             bool UseTBAA) const {
  assert(isLock(Lock.getOpcode()) &&
         "mayLockOrderWithMemOp expects a lock instruction");
  assert(Mem.mayLoadOrStore() &&
         "mayLockOrderWithMemOp expects a mayLoadOrStore instruction");
  if (Lock.memoperands_empty() || Mem.memoperands_empty())
    return true;

  const MachineFrameInfo &MFI = Lock.getMF()->getFrameInfo();
  const MachineMemOperand *LockMMO = *Lock.memoperands_begin();

  // Part-word stores: elide lock delays only on source noalias (restrict), not
  // on AA-inferred NoAlias. No AA on this path — user annotation is the
  // contract.
  const bool PartWordStore = Mem.mayStore() && isPartWordMemoryInst(Mem);

  // Cannot use MachineInstr::mayAlias(Lock, Mem): it requires both
  // instructions to be mayLoadOrStore, but ACQ/REL have no mayLoad/mayStore
  // (locks are not DM accesses). The ptr-coupled MMO is annotative — it carries
  // MOLoad only for GISel MMO construction, not because acq/rel loads memory.
  // mayAlias would return false immediately and skip all lock delays. Compare
  // MMO pairs directly via memOperandsMayAlias instead.
  for (const MachineMemOperand *MemMMO : Mem.memoperands()) {
    if (PartWordStore) {
      if (!partWordStoreLockMayUseAliasAnalysis(LockMMO, MemMMO))
        return true;
      continue;
    }
    if (memOperandsMayAlias(MFI, AA, UseTBAA, LockMMO, MemMMO))
      return true;
  }

  return false;
}

// Helper function to find instruction variant info by opcode using binary
// search. Returns nullptr if not found.
static const InstrVariantInfo *
findInstrVariantInfo(const VarItinInterface &Interface, unsigned Opcode) {
  if (!Interface.hasVariants())
    return nullptr;

  auto It = llvm::lower_bound(Interface.InstrVariants, Opcode,
                              [](const InstrVariantInfo &Info, unsigned Op) {
                                return Info.Opcode < Op;
                              });

  if (It != Interface.InstrVariants.end() && It->Opcode == Opcode)
    return &*It;

  return nullptr;
}

// Helper function to check if all operand RC requirements match for a variant.
// An OperandRegInfo with neither RC nor Reg set (e.g. a non-register or
// unclassified virtual register operand) causes the variant not to match,
// falling back to the static default schedule class.
static bool variantMatches(const SchedVariantInfo &Variant,
                           ArrayRef<OperandRegInfo> OperandRegs) {
  for (const OperandRCRequirement &Req : Variant.OperandRCs) {
    // Check if the operand index is within bounds.
    if (Req.OpIdx >= OperandRegs.size())
      return false;

    const OperandRegInfo &OpInfo = OperandRegs[Req.OpIdx];
    const bool HasRC = OpInfo.RC != nullptr;
    const bool HasReg = OpInfo.Reg.isValid();

    // Non-register or unclassified virtual register operands cannot satisfy
    // any RC requirement — this variant does not match.
    if (!HasRC && !HasReg)
      return false;

    assert(!(HasRC && HasReg) && "OperandRegInfo cannot have both RC and Reg");
    assert((!HasReg || OpInfo.Reg.isPhysical()) &&
           "OperandRegInfo Reg must be physical");

    if (HasRC) {
      if (!Req.RC->hasSubClassEq(OpInfo.RC))
        return false;
    } else {
      // Physical register - check if the required RC contains it.
      if (!Req.RC->contains(OpInfo.Reg))
        return false;
    }
  }
  return true;
}

// Build a vector of OperandRegInfo from a range of MachineOperands.
// Non-register operands and virtual registers with no register class are
// represented as a default OperandRegInfo (both Reg invalid and RC null),
// which causes any variant requiring that operand index not to match.
static SmallVector<OperandRegInfo>
buildOperandRegInfos(iterator_range<const MachineOperand *> Operands,
                     const MachineRegisterInfo &MRI) {
  const TargetRegisterInfo &TRI = *MRI.getTargetRegisterInfo();
  SmallVector<OperandRegInfo> Result;
  for (const MachineOperand &MO : Operands) {
    if (!MO.isReg() || !MO.getReg().isValid()) {
      Result.emplace_back();
      continue;
    }
    const Register Reg = MO.getReg();
    if (Reg.isPhysical()) {
      Result.emplace_back(Reg);
    } else {
      // Use getRegClassOrNull to avoid asserting on generic virtual registers
      // (GlobalISel VRegs that have only types, not register classes). A null
      // RC produces a default OperandRegInfo so variant matching falls back to
      // the static default schedule class.
      const TargetRegisterClass *RC = MRI.getRegClassOrNull(Reg);
      if (RC && MO.getSubReg())
        RC = TRI.getSubRegisterClass(RC, MO.getSubReg());
      Result.emplace_back(RC);
    }
  }
  return Result;
}

// Helper function to find the matching variant for given operand register
// info. Returns nullptr if no variant matches.
static const SchedVariantInfo *
findMatchingVariant(const InstrVariantInfo *InstrInfo,
                    ArrayRef<OperandRegInfo> OperandRegs) {
  if (!InstrInfo)
    return nullptr;

  for (const SchedVariantInfo &Variant : InstrInfo->Variants) {
    if (variantMatches(Variant, OperandRegs))
      return &Variant;
  }

  return nullptr;
}

unsigned
AIEBaseInstrInfo::getSchedClass(const MCInstrDesc &Desc,
                                iterator_range<const MachineOperand *> Operands,
                                const MachineRegisterInfo &MRI) const {
  return getSchedClass(Desc, buildOperandRegInfos(Operands, MRI));
}

unsigned
AIEBaseInstrInfo::getSchedClass(const MCInstrDesc &Desc,
                                ArrayRef<OperandRegInfo> OperandRegs) const {
  // Get the interface from the derived class.
  const VarItinInterface Interface = getVarItinInterface();

  // Look up the instruction in the variant tables.
  const InstrVariantInfo *InstrInfo =
      findInstrVariantInfo(Interface, Desc.getOpcode());

  // Find a matching variant based on operand register info.
  const SchedVariantInfo *Variant = findMatchingVariant(InstrInfo, OperandRegs);

  // Return the matched schedule class, or fall back to the default.
  return Variant ? Variant->SchedClass : Desc.getSchedClass();
}

unsigned
AIEBaseInstrInfo::getNumSchedClassVariants(const MCInstrDesc &Desc) const {
  const VarItinInterface Interface = getVarItinInterface();
  const InstrVariantInfo *InstrInfo =
      findInstrVariantInfo(Interface, Desc.getOpcode());

  return InstrInfo ? InstrInfo->Variants.size() : 0;
}

llvm::ArrayRef<OperandRCRequirement> AIEBaseInstrInfo::getMatchingOperandRCs(
    const MCInstrDesc &Desc, iterator_range<const MachineOperand *> Operands,
    const MachineRegisterInfo &MRI) const {
  const VarItinInterface Interface = getVarItinInterface();
  const InstrVariantInfo *InstrInfo =
      findInstrVariantInfo(Interface, Desc.getOpcode());

  const SchedVariantInfo *Variant =
      findMatchingVariant(InstrInfo, buildOperandRegInfos(Operands, MRI));

  return Variant ? Variant->OperandRCs : ArrayRef<OperandRCRequirement>();
}

bool AIEBaseInstrInfo::isLegalTypeToPad(const LLT &Ty,
                                        StringRef *ErrInfo) const {
  if (Ty.isVector() && (Ty.getSizeInBits() == 128 || Ty.getSizeInBits() == 256))
    return true;
  if (ErrInfo)
    *ErrInfo = "Operand size is illegal";
  return false;
}

bool AIEBaseInstrInfo::isLegalTypeToUnpad(const LLT &Ty,
                                          StringRef *ErrInfo) const {
  if (Ty.isVector() && (Ty.getSizeInBits() == 256 || Ty.getSizeInBits() == 512))
    return true;
  if (ErrInfo)
    *ErrInfo = "Operand size is illegal";
  return false;
}

bool AIEBaseInstrInfo::verifyGenericInstruction(const MachineInstr &MI,
                                                StringRef &ErrInfo) const {
  return true;
}

bool AIEBaseInstrInfo::verifyMemOperand(const MachineInstr &MI,
                                        StringRef &ErrInfo) const {
  return true;
}

static MCRegister findSuperRegister(const MachineInstr &MI,
                                    const TiedRegOperands &TiedRegs,
                                    const TargetRegisterInfo &TRI) {
  Register SrcReg = MI.getOperand(TiedRegs.SrcOps.front().OpIdx).getReg();
  if (!SrcReg.isPhysical())
    return MCRegister::NoRegister;

  if (unsigned SubRegIdx = TiedRegs.SrcOps.front().SubRegIdx) {
    assert(TiedRegs.NewSuperClass && "Incomplete Tied register info");
    return TRI.getMatchingSuperReg(SrcReg.asMCReg(), SubRegIdx,
                                   TiedRegs.NewSuperClass);
  }
  return SrcReg.asMCReg();
}

bool AIEBaseInstrInfo::verifyTiedRegisters(const MachineInstr &MI,
                                           StringRef &ErrInfo) const {
  const TargetSubtargetInfo &ST = MI.getMF()->getSubtarget();
  const TargetRegisterInfo &TRI = *ST.getRegisterInfo();
  auto VerifyTiedReg = [&](const MachineInstr &MI,
                           const OperandSubRegMapping &OM,
                           MCRegister ExpectedSuperReg) {
    const MachineOperand &Op = MI.getOperand(OM.OpIdx);

    if (!Op.isReg()) {
      ErrInfo = "Tied operand must be a register";
      return false;
    }
    if (!Register::isPhysicalRegister(Op.getReg()))
      return true;
    auto ExpectedPhysReg = OM.SubRegIdx
                               ? TRI.getSubReg(ExpectedSuperReg, OM.SubRegIdx)
                               : ExpectedSuperReg;
    if (Op.getReg().asMCReg() != ExpectedPhysReg) {
      ErrInfo = "Tied physical registers must match";
      return false;
    }
    if (Op.isRenamable()) {
      ErrInfo = "Tied register is renamable";
      return false;
    }
    return true;
  };

  for (const TiedRegOperands &Regs : getTiedRegInfo(MI)) {
    MCRegister ExpectedSuperReg = findSuperRegister(MI, Regs, TRI);
    for (const OperandSubRegMapping &DstOp : Regs.DstOps) {
      if (!VerifyTiedReg(MI, DstOp, ExpectedSuperReg))
        return false;
    }
    for (const OperandSubRegMapping &SrcOp : Regs.SrcOps) {
      if (!VerifyTiedReg(MI, SrcOp, ExpectedSuperReg))
        return false;
    }
  }
  return true;
}

bool AIEBaseInstrInfo::verifySameLaneTypes(const MachineInstr &MI,
                                           StringRef &ErrInfo) {
  if (MI.getNumOperands() != 2 || !MI.getOperand(0).isReg() ||
      !MI.getOperand(1).isReg()) {
    ErrInfo = "Both operands must be registers";
    return false;
  }

  const MachineRegisterInfo &MRI = MI.getMF()->getRegInfo();
  LLT DstTy = MRI.getType(MI.getOperand(0).getReg());
  LLT SrcTy = MRI.getType(MI.getOperand(1).getReg());
  if (!DstTy.isVector() || !SrcTy.isVector()) {
    ErrInfo = "Both operands must be of vector type";
    return false;
  }
  if (DstTy.getElementType() != SrcTy.getElementType()) {
    ErrInfo = "Both vector operands must have the same element type";
    return false;
  }
  return true;
}

bool AIEBaseInstrInfo::verifyImplicitOpsOrder(const MachineInstr &MI,
                                              StringRef &ErrInfo) const {
  if (MI.isCall() || isSchedulingBoundary(MI, MI.getParent(), *MI.getMF())) {
    // Scheduling boundaries (calls in particular) can have regmask operands.
    // Those typically get inserted in between the explicit and implicit
    // operands. This therefore shifts the indices of implicit operands and
    // breaks their mapping to operand latencies in Itineraries.
    // This is fine for scheduling boundaries because their itinerary isn't
    // queried, but this is an error for all other instructions.
    // FIXME: "static" implicit operands coming from the descriptor should never
    // be shifted.
    return true;
  }

  auto VerifyMIOperand = [&MI, &ErrInfo](unsigned MIOpIdx,
                                         MCPhysReg ExpectedImplicitReg,
                                         bool IsDef) -> bool {
    assert(MIOpIdx >= MI.getNumExplicitOperands());
    if (MIOpIdx >= MI.getNumOperands()) {
      ErrInfo = "Missing implicit operands compared to descriptor";
      return false;
    }

    const MachineOperand &MIOp = MI.getOperand(MIOpIdx);
    if (!MIOp.isReg() || !MIOp.isImplicit()) {
      ErrInfo = "MI operand not an implicit register as stated in descriptor";
      return false;
    }
    if (MIOp.isDef() != IsDef || MIOp.getReg() != ExpectedImplicitReg) {
      ErrInfo = "Implicit operand in MI not matching that of the descriptor";
      return false;
    }
    return true;
  };

  // Verify that the first implicit operands in MI match the implicit defs in
  // Desc, followed by the implicit uses in Desc.
  unsigned MIOpIdx = MI.getNumExplicitOperands();
  const MCInstrDesc &Desc = MI.getDesc();
  for (MCPhysReg DescImplicitDef : Desc.implicit_defs()) {
    if (!VerifyMIOperand(MIOpIdx, DescImplicitDef, /*IsDef=*/true)) {
      return false;
    }
    ++MIOpIdx;
  }
  for (MCPhysReg DescImplicitUse : Desc.implicit_uses()) {
    if (!VerifyMIOperand(MIOpIdx, DescImplicitUse, /*IsDef=*/false)) {
      return false;
    }
    ++MIOpIdx;
  }

  // All implicit ops mentioned in the descriptor are matching those in MI.
  return true;
}

bool AIEBaseInstrInfo::verifyControlFlowConstraints(const MachineInstr &MI,
                                                    StringRef &ErrInfo) const {

  auto NotAllowedInZOL = [&](const MachineInstr &MI) {
    return !MI.isMetaInstruction() &&
           (MI.isCall() || MI.isBranch() || MI.isIndirectBranch() ||
            isZeroOverheadLoopSetupInstr(MI));
  };

  if (isZOLBody(*MI.getParent())) {
    if (MI.isBundle()) {
      for (const MachineInstr &BundledMI : const_bundled_instrs(MI)) {
        if (NotAllowedInZOL(BundledMI)) {
          ErrInfo = "Invalid bundled instruction in a ZOL loop!";
          return false;
        }
      }
    } else if (NotAllowedInZOL(MI)) {
      ErrInfo = "Invalid instruction in a ZOL loop!";
      return false;
    }
  }
  return true;
}

bool AIEBaseInstrInfo::hasAnnotativeMemOperands(const MachineInstr &MI) const {
  return isLock(MI.getOpcode()) && !MI.memoperands_empty();
}

bool AIEBaseInstrInfo::verifyInstruction(const MachineInstr &MI,
                                         StringRef &ErrInfo) const {
  const Triple &TT = MI.getMF()->getSubtarget().getTargetTriple();
  if (!verifyGenericInstruction(MI, ErrInfo)) {
    return false;
  }
  if (!verifyTiedRegisters(MI, ErrInfo)) {
    return false;
  }
  if (!verifyMemOperand(MI, ErrInfo)) {
    return false;
  }
  if (!verifyControlFlowConstraints(MI, ErrInfo)) {
    return false;
  }
  if (!TT.isAIE1() && !verifyImplicitOpsOrder(MI, ErrInfo)) {
    // FIXME: Some AIE1 tests need updating.
    return false;
  }
  return TargetInstrInfo::verifyInstruction(MI, ErrInfo);
}

bool AIEBaseInstrInfo::canHoistCheapInst(const MachineInstr &MI) const {
  return !NoCheapInstHoisting;
}

// Return true if TRC is a superclass of RC or contains the given reg.
// This is primarily a helper function for the functions below.  The first
// case is active when Reg is a virtual register, but is apparently not
// sufficient alone
bool AIEBaseInstrInfo::regClassMatches(const TargetRegisterClass &TRC,
                                       const TargetRegisterClass *RC,
                                       unsigned Reg) {
  if (RC) {
    // Check if TRC is a superclass of RC.
    if (TRC.hasSubClassEq(RC))
      return true;
    // For two equivalent classes TableGen generates an asymmetric
    // subclass relation. We make it symmetrical if the register
    // classes have the same number of registers.
    if (RC->getNumRegs() == TRC.getNumRegs() && RC->hasSubClassEq(&TRC))
      return true;
  }
  if (TRC.contains(Reg))
    return true;
  return false;
}

std::optional<DestSourcePair>
AIEBaseInstrInfo::isCopyInstrImpl(const MachineInstr &MI) const {
  if (MI.isMoveReg())
    return DestSourcePair{MI.getOperand(0), MI.getOperand(1)};
  return std::nullopt;
}

std::unique_ptr<TargetInstrInfo::PipelinerLoopInfo>
AIEBaseInstrInfo::analyzeLoopForPipelining(MachineBasicBlock *LoopBB) const {
  // Lock instructions require special scheduling constraints (core stall/resume
  // cycles) that are not implemented for software pipelined loops.
  if (hasLockInstruction(*LoopBB))
    return nullptr;
  MachineBasicBlock::iterator I = LoopBB->getFirstTerminator();
  return createAIEBasePipelinerLoopInfo(&(*I), *this);
}

ResourceCycle *
AIEBaseInstrInfo::CreateTargetScheduleState(const TargetSubtargetInfo &) const {
  return new AIEResourceCycle(FormatInterface);
}

const MIRFormatter *AIEBaseInstrInfo::getMIRFormatter() const {
  if (!Formatter.get())
    Formatter = std::make_unique<AIEMIRFormatter>();
  return Formatter.get();
}

/// Return operand information related to vector concat instrinsic.
std::optional<const AIEBaseInstrInfo::VConcatOpInfo>
AIEBaseInstrInfo::getVConcatOpInfo(const MachineInstr &MI) const {

  if (MI.getOpcode() == TargetOpcode::G_CONCAT_VECTORS)
    return VConcatOpInfo{1, 0};
  return std::nullopt;
}

MCSlotKind AIEBaseInstrInfo::getSlotKind(unsigned Opcode) const {
  return FormatInterface->getSlotKind(Opcode);
}

const MCSlotInfo *AIEBaseInstrInfo::getSlotInfo(const MCSlotKind Kind) const {
  return FormatInterface->getSlotInfo(Kind);
}

bool AIEBaseInstrInfo::isMultiSlotPseudo(const MachineInstr &MI) const {
  return MI.isPseudo() &&
         getFormatInterface()->getAlternateInstsOpcode(MI.getOpcode());
}

bool AIEBaseInstrInfo::isPartWordMemoryInst(const MachineInstr &MI) const {
  // Not a memory instruction
  if (!MI.mayLoadOrStore())
    return false;

  // Conservative: assume part-word if no memory operands
  if (MI.memoperands_empty())
    return true;

  // Check if any memory operand has unknown size or accesses less than a full
  // word
  return llvm::any_of(MI.memoperands(), [](const MachineMemOperand *MMO) {
    // The word size for this target
    const unsigned WordSizeInBytes = 4;
    const LocationSize Size = MMO->getSize();
    return (!Size.hasValue() || Size.getValue() < WordSizeInBytes);
  });
}

std::optional<unsigned>
AIEBaseInstrInfo::getSlotOpcode(const MCSlotKind Slot,
                                const MachineInstr &MI) const {
  assert(isMultiSlotPseudo(MI));
  for (const auto &OpCode :
       *getFormatInterface()->getAlternateInstsOpcode(MI.getOpcode())) {
    if (getSlotKind(OpCode) == Slot)
      return OpCode;
  }
  return {};
}

std::vector<MachineBasicBlock::iterator>
AIEBaseInstrInfo::getAlignmentBoundaries(MachineBasicBlock &MBB) const {
  std::vector<MachineBasicBlock::iterator> AlignmentBoundaries;

  unsigned DelaySlot = 0;
  // LoopSetupDistance is the number of bundles between setup and LEND.
  // In PostRAScheduler, this is enforced by setting the exit latency in the
  // scheduler dag mutator.
  int LoopPaddingInBytes = 0;
  bool IsCall = false;
  auto ZOLSupport = getZOLSupport();
  const unsigned ZOLSetupToLoopEndDist =
      ZOLSupport.has_value() ? ZOLSupport->LoopSetupDistance : 0;
  const bool LoopSetupRequiresByteDistance =
      ZOLSupport && ZOLSupport->LoopSetupRequiresByteDistance;
  const unsigned LoopSetupSizeInBytes =
      LoopSetupRequiresByteDistance
          ? getMachineBlockAlignmentBytes() * ZOLSetupToLoopEndDist
          : 0;
  const unsigned MBBAlignment = getMachineBlockAlignmentBytes();

  const unsigned JumpToLENDBytes =
      ZOLSupport ? ZOLSupport->JumpToLoopEndDistanceInBytes : 0;

  // Byte-based counter for padding delay slot bundles of branches near
  // ZOL. Works like LoopPaddingInBytes: each bundle absorbs up to
  // (MBBAlignment - BundleSize) bytes until the deficit is covered.
  int JumpPaddingInBytes = 0;

  // Check if this MBB can have a branch that reaches a ZOL body within
  // JumpToLoopEndDistanceInBytes. Scan forward in layout order; any block
  // whose single successor is a ZOL body is a candidate preheader. This
  // handles jumps not only in the immediate predecessor of the preheader
  // but in any block within range of LEND.
  bool IsGuardOfZOL = false;
  MachineBasicBlock *GuardPreheader = nullptr;
  MachineBasicBlock *GuardZOLBody = nullptr;
  unsigned GuardIntermediateBlocksSize = 0;
  if (JumpToLENDBytes > 0) {
    unsigned AccumulatedSize = 0;
    for (auto NextIt = std::next(MBB.getIterator()),
              EndIt = MBB.getParent()->end();
         NextIt != EndIt; ++NextIt) {
      MachineBasicBlock &NextMBB = *NextIt;
      if (NextMBB.succ_size() == 1 &&
          NextMBB.getFirstTerminator() == NextMBB.end()) {
        MachineBasicBlock *PossibleBody = *NextMBB.successors().begin();
        if (isZOLBody(*PossibleBody)) {
          IsGuardOfZOL = true;
          GuardPreheader = &NextMBB;
          GuardZOLBody = PossibleBody;
          GuardIntermediateBlocksSize = AccumulatedSize;
          break;
        }
      }
      AccumulatedSize += getMBBSizeInBytes(NextMBB);
      if (AccumulatedSize >= JumpToLENDBytes)
        break;
    }
  }

  auto GetPostZOLSetupRegionSize =
      [this](MachineBasicBlock &LoopMBB) -> std::pair<unsigned, unsigned> {
    MachineBasicBlock *Pred =
        AIELoopUtils::getDedicatedFallThroughPreheader(LoopMBB);
    assert(Pred && "Null preheader block!");
    return getPostZOLRegionSizeInfo(*Pred);
  };

  auto LoopSizeExcludingLastBundle = [&](MachineBasicBlock &MBB) -> unsigned {
    if (MBB.empty())
      return 0;

    auto It = MBB.getLastNonDebugInstr();
    if (It == MBB.begin())
      return 0;
    // Step before the PseudoLoopEnd.
    --It;
    while (It != MBB.begin()) {
      if (It->isBundle())
        return getRegionSizeInBytes(llvm::make_range(MBB.begin(), It));
      --It;
    }
    return 0;
  };

  const bool IsZOLBody = isZOLBody(MBB);
  if (IsZOLBody) {
    assert(ZOLSupport);

    // Exclude the LoopEnd bundle as it must be placed in its own standalone
    // region to guarantee alignment. Additionally, there must be a gap
    // (in PM address space) between writing to the ls, le, and lc registers and
    // the LoopEnd instruction.
    const unsigned LoopSize = LoopSizeExcludingLastBundle(MBB);

    if (LoopSize < LoopSetupSizeInBytes) {
      const unsigned PostZOLRegionSize = GetPostZOLSetupRegionSize(MBB).second;
      const int LoopEndDistance =
          LoopSetupSizeInBytes - (PostZOLRegionSize + LoopSize);
      LoopPaddingInBytes = std::max(0, LoopEndDistance);
    }
  }

  for (auto MI = MBB.begin(), End = MBB.end(); MI != End; ++MI) {
    if (MI->isBundle()) {
      // Return Address Candidate.
      IsCall = isCallBundle(MI);
      if (IsCall && DelaySlot > 0)
        llvm_unreachable("Cannot have branch in branch delay slot!\n");

      if (DelaySlot > 0) {
        DelaySlot--;
        if (DelaySlot == 0)
          /* Region + 1 => RegionEnd */
          AlignmentBoundaries.emplace_back(std::next(MI));
      }

      // Create regions of singleton bundle for schedule margin bundles,
      // alignment algorithm will force fill each bundle to the correct number
      // of bits to fulfill the alignment requirement of the region.
      if (LoopPaddingInBytes > 0) {
        AlignmentBoundaries.emplace_back(MI);
        const int BundleSize = getAIEMachineBundleSize(MI);
        LoopPaddingInBytes -= (MBBAlignment - BundleSize);
      }

      // Pad delay slot bundles of branches near ZOL to meet the
      // jump-to-LEND byte distance. Only pads as many bundles as needed.
      if (JumpPaddingInBytes > 0) {
        AlignmentBoundaries.emplace_back(MI);
        const int BundleSize = getAIEMachineBundleSize(MI);
        JumpPaddingInBytes -= (MBBAlignment - BundleSize);
      }

      if (IsCall)
        DelaySlot = getNumDelaySlots(*MI);

      // When a branch is found and there is a ZOL body within
      // JumpToLENDBytes (possibly with intermediate blocks), compute
      // the distance from this branch to LEND and pad delay slots to
      // cover the deficit.
      if (IsGuardOfZOL && isJumpBundle(MI)) {
        unsigned DistToEnd =
            getRegionSizeInBytes(llvm::make_range(MI, MBB.end()));
        unsigned PreheaderSize = getMBBSizeInBytes(*GuardPreheader);
        unsigned LoopBodySize = LoopSizeExcludingLastBundle(*GuardZOLBody);
        int Deficit = static_cast<int>(JumpToLENDBytes) -
                      static_cast<int>(DistToEnd + GuardIntermediateBlocksSize +
                                       PreheaderSize + LoopBodySize);
        if (Deficit > 0) {
          JumpPaddingInBytes = Deficit;
        }
      }
      // Distance in terms of fully-expanded bundles that loop setup should
      // maintain. We force each of these bundles to an alignment boundary.
      if (ZOLSupport && isZOLSetupBundle(MI) && isLastZOLSetupBundleInMBB(MI)) {
        unsigned LoopSize = 0;
        // If we have only one MBB, it must be the loop.
        if (MBB.succ_size() == 1) {
          MachineBasicBlock *LoopSucc = *MBB.successors().begin();
          LoopSize = LoopSizeExcludingLastBundle(*LoopSucc);
          // If we have a loop size, we can consider that it will
          // have, at least, the number of bytes required by alignment.
          if (LoopSize > 0 && LoopSize < getMachineBlockAlignmentBytes())
            LoopSize = getMachineBlockAlignmentBytes();
        }

        if (LoopSize < LoopSetupSizeInBytes) {
          const auto [PostZOLBundleCount, PostZOLSize] =
              getPostZOLRegionSizeInfo(MBB);
          const int AvailablePaddingSpace =
              PostZOLBundleCount * getMachineBlockAlignmentBytes() -
              PostZOLSize;
          const int NeededPadding =
              LoopSetupSizeInBytes - LoopSize - PostZOLSize;
          LoopPaddingInBytes = std::min(AvailablePaddingSpace, NeededPadding);
          // Align the last setup bundle. Given the minimum LoopSetupDistance
          // number of bundles afterwards, sufficient padding is guaranteed.
          if (LoopPaddingInBytes > 0) {
            AlignmentBoundaries.emplace_back(MI);
            const int BundleSize = getAIEMachineBundleSize(MI);
            LoopPaddingInBytes -= (MBBAlignment - BundleSize);
          }
        }
      }
    } else if (isHardwareLoopEnd(MI->getOpcode())) {
      if (DelaySlot > 0)
        llvm_unreachable("Cannot have HWLoopEnd in branch delay slot!\n");
      // The previous instruction is the last bundle of the hardware loop
      // and should be aligned. We should skip meta instructions.
      auto PrevNonMetaMI = std::prev(MI);
      while (PrevNonMetaMI->isMetaInstruction())
        PrevNonMetaMI = std::prev(PrevNonMetaMI);

      AlignmentBoundaries.emplace_back(PrevNonMetaMI);
    } else if (!MI->isMetaInstruction()) {
      // Single instruction, there should not be any
      // after Bundle Finalization Pass.
      llvm_unreachable("Found an un-expected standalone instruction !");
    }
  }

  if (LoopPaddingInBytes > 0)
    llvm_unreachable("LoopStart/LoopBody: insufficient padding!\n");

  AlignmentBoundaries.emplace_back(MBB.end());
  return AlignmentBoundaries;
}

bool AIEBaseInstrInfo::isNativeS20Consumer(
    const MachineInstr &MI, const MachineRegisterInfo &MRI) const {
  const LLT S20 = LLT::scalar(20);
  for (const MachineOperand &MO : MI.uses()) {
    if (!MO.isReg())
      continue;
    const LLT Type = MRI.getType(MO.getReg());
    if (Type == S20)
      return true;
  }

  return false;
}

std::optional<unsigned>
AIEBaseInstrInfo::getInputPtrIdx(const MachineInstr &MI,
                                 const MachineRegisterInfo &MRI) const {
  for (const MachineOperand &MO : MI.uses()) {
    if (!MO.isReg())
      continue;
    const LLT Type = MRI.getType(MO.getReg());
    if (Type.isPointer())
      return MO.getOperandNo();
  }

  return {};
}

std::optional<unsigned>
AIEBaseInstrInfo::getOutputPtrIdx(const MachineInstr &MI,
                                  const MachineRegisterInfo &MRI) const {
  for (const MachineOperand &MO : MI.defs()) {
    if (!MO.isReg())
      continue;
    const LLT Type = MRI.getType(MO.getReg());
    if (Type.isPointer())
      return MO.getOperandNo();
  }

  return {};
}

bool AIEBaseInstrInfo::isExtendLikelyToBeFolded(
    MachineInstr &ExtMI, MachineRegisterInfo &MRI) const {
  assert(ExtMI.getOpcode() == TargetOpcode::G_SEXT ||
         ExtMI.getOpcode() == TargetOpcode::G_ZEXT ||
         ExtMI.getOpcode() == TargetOpcode::G_ANYEXT);

  if (ExtMI.getOpcode() != TargetOpcode::G_ZEXT)
    return false;

  const LLT S20 = LLT::scalar(20);
  const LLT ActualType = MRI.getType(ExtMI.getOperand(1).getReg());
  return ActualType == S20;
}

bool AIEBaseInstrInfo::canInsertSelect(const MachineBasicBlock &MBB,
                                       ArrayRef<MachineOperand> Cond,
                                       Register DstReg, Register TrueReg,
                                       Register FalseReg, int &CondCycles,
                                       int &TrueCycles,
                                       int &FalseCycles) const {

  auto Support = getIfConvSupport();

  if (!Support) {
    LLVM_DEBUG(dbgs() << "No If-Conversion support for this target\n");
    return false;
  }

  // Check register classes.
  const MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
  const TargetRegisterClass *RCTrue = MRI.getRegClass(TrueReg);
  const TargetRegisterClass *RCFalse = MRI.getRegClass(FalseReg);
  const TargetRegisterClass *ScalarRegisterClass = Support->ScalarRegisterClass;

  if (!RCTrue || !RCFalse)
    return false;

  if (ScalarRegisterClass->hasSubClassEq(RCTrue) &&
      ScalarRegisterClass->hasSubClassEq(RCFalse)) {
    // Latencies from Cond+Branch, TrueReg, and FalseReg to DstReg.
    // We force a low Cond+Branch to be permissive as much as
    // possible, considering that is better to control the behavior
    // using getCriticalPathLimit instead of an unrealistic miss prediction
    // penalty (no predictor in AIE).
    CondCycles = 1;
    TrueCycles = 1;
    FalseCycles = 1;
    return true;
  }
  return false;
}

void AIEBaseInstrInfo::insertSelect(MachineBasicBlock &MBB,
                                    MachineBasicBlock::iterator MI,
                                    const DebugLoc &DL, Register DestReg,
                                    ArrayRef<MachineOperand> Cond,
                                    Register TrueReg, Register FalseReg) const {

  auto Support = getIfConvSupport();
  assert(Support && "No If-Conversion support for this target\n");

  // We need to respect the selection instruction register requirements (if any)
  const TargetRegisterClass *SelectRegisterClass = Support->SelectRegisterClass;

  MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
  unsigned BranchOpcode = Cond[0].getImm();
  Register ConditionReg = Cond[1].getReg();

  const unsigned SelectOpcode = Support->branchToSelect(BranchOpcode);
  Register ConstrainedCondReg = MRI.createVirtualRegister(SelectRegisterClass);

  BuildMI(MBB, MI, DL, get(TargetOpcode::COPY), ConstrainedCondReg)
      .addReg(ConditionReg);

  // Retrieve the correct operand order.
  SmallVector<Register, 3> InputOperands;
  InputOperands.push_back(ConstrainedCondReg);
  InputOperands.push_back(TrueReg);
  InputOperands.push_back(FalseReg);

  BuildMI(MBB, MI, DL, get(SelectOpcode), DestReg)
      .addReg(Support->getSelectSrcOperand(InputOperands, 0))
      .addReg(Support->getSelectSrcOperand(InputOperands, 1))
      .addReg(Support->getSelectSrcOperand(InputOperands, 2));
}

bool AIEBaseInstrInfo::areMemAccessesTriviallyDisjoint(
    const MachineInstr &MIa, const MachineInstr &MIb) const {
  if (!MIa.hasOneMemOperand() || !MIb.hasOneMemOperand())
    return false;

  // If mem-operands show that the same address Value is used by both
  // instructions, check for non-overlapping offsets and widths.
  const MachineMemOperand *MMOa = *MIa.memoperands_begin();
  const MachineMemOperand *MMOb = *MIb.memoperands_begin();

  auto CheckOverlapping = [=](int64_t OffsetA, int64_t OffsetB) {
    const LocationSize WidthA = MMOa->getSize(), WidthB = MMOb->getSize();
    const int64_t LowOffset = OffsetA < OffsetB ? OffsetA : OffsetB;
    const int64_t HighOffset = OffsetA < OffsetB ? OffsetB : OffsetA;
    const LocationSize LowWidth = (LowOffset == OffsetA) ? WidthA : WidthB;
    return (LowWidth.hasValue() &&
            LowOffset + (int64_t)LowWidth.getValue() <= HighOffset);
  };

  const int64_t MMOOffsetA = MMOa->getOffset();
  const int64_t MMOOffsetB = MMOb->getOffset();

  const Value *VALa = MMOa->getValue();
  const Value *VALb = MMOb->getValue();
  const bool SameValue = (VALa && VALb && (VALa == VALb));
  if (SameValue)
    return CheckOverlapping(MMOOffsetA, MMOOffsetB);

  const PseudoSourceValue *PSVa = MMOa->getPseudoValue();
  const PseudoSourceValue *PSVb = MMOb->getPseudoValue();

  const bool ExistBothPseudoSources = PSVa && PSVb;
  if (!ExistBothPseudoSources)
    return false;

  const bool SamePseudoSource = PSVa == PSVb;
  if (SamePseudoSource)
    return CheckOverlapping(MMOOffsetA, MMOOffsetB);

  const FixedStackPseudoSourceValue *FixedStackA =
      dyn_cast<FixedStackPseudoSourceValue>(PSVa);
  const FixedStackPseudoSourceValue *FixedStackB =
      dyn_cast<FixedStackPseudoSourceValue>(PSVb);

  // If we have different fixed stack objects, we have disjoint access
  // when offsets are different and we don't have any partial overlap
  // (SameVal check).
  const bool ExistsBothFixedStackObjs = FixedStackA && FixedStackB;
  if (!ExistsBothFixedStackObjs)
    return false;

  const MachineFrameInfo &MFI = MIa.getMF()->getFrameInfo();
  const int64_t ObjOffsetA = MFI.getObjectOffset(FixedStackA->getFrameIndex());
  const int64_t ObjOffsetB = MFI.getObjectOffset(FixedStackB->getFrameIndex());

  return CheckOverlapping(ObjOffsetA, ObjOffsetB);
}

/// Common implementation for isLoadFromStackSlot and isStoreToStackSlot.
/// \param IsLoad true for load detection, false for store detection.
/// \returns The register being loaded/stored, or 0 if not a stack access.
static Register isStackSlotMemoryAccess(const MachineInstr &MI, int &FrameIndex,
                                        bool IsLoad) {
  // Quick reject: check memory access type
  if (IsLoad) {
    if (!MI.mayLoad() || MI.mayStore())
      return 0;
  } else {
    if (!MI.mayStore() || MI.mayLoad())
      return 0;
  }

  const TargetSubtargetInfo &ST = MI.getMF()->getSubtarget();
  const auto &TRI =
      *static_cast<const AIEBaseRegisterInfo *>(ST.getRegisterInfo());

  const Register SPReg = TRI.getStackPointerRegister();
  if (!MI.readsRegister(SPReg, &TRI))
    return 0;

  if (MI.getNumOperands() < 2)
    return 0;

  const MachineOperand &RegOp = MI.getOperand(0);
  if (!RegOp.isReg())
    return 0;
  if (IsLoad ? !RegOp.isDef() : !RegOp.isUse())
    return 0;

  if (!MI.getOperand(1).isFI())
    return 0;

  unsigned MatchingStackMMOs = 0;
  for (const auto *MMO : MI.memoperands()) {
    const bool IsMatching =
        (IsLoad ? MMO->isLoad() : MMO->isStore()) &&
        isa_and_nonnull<FixedStackPseudoSourceValue>(MMO->getPseudoValue());
    if (!IsMatching)
      continue;
    ++MatchingStackMMOs;
  }

  if (!MatchingStackMMOs)
    return 0;

  assert(MatchingStackMMOs == 1 &&
         "Expected exactly one fixed-stack MachineMemOperand");
  assert(MI.hasOneMemOperand() &&
         "Expected stack spill/reload to have exactly one MachineMemOperand");

  FrameIndex = MI.getOperand(1).getIndex();
  return RegOp.getReg();
}

Register AIEBaseInstrInfo::isLoadFromStackSlot(const MachineInstr &MI,
                                               int &FrameIndex) const {
  return isStackSlotMemoryAccess(MI, FrameIndex, /*IsLoad=*/true);
}

Register AIEBaseInstrInfo::isStoreToStackSlot(const MachineInstr &MI,
                                              int &FrameIndex) const {
  return isStackSlotMemoryAccess(MI, FrameIndex, /*IsLoad=*/false);
}

ArrayRef<AIEBaseInstrInfo::WidenNarrowConversionPair>
AIEBaseInstrInfo::getWidenNarrowConversionPairs() const {
  // Default implementation returns empty array.
  // Targets override this to provide their specific conversion pairs.
  return {};
}
