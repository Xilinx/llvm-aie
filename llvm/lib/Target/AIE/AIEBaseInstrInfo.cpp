//===-- AIEBaseInstrInfo.cpp - AIE Instruction Information ------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIE implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseInstrInfo.h"
#include "AIE.h"
#include "AIEBasePipelinerLoopInfo.h"
#include "AIEHazardRecognizer.h"
#include "AIESubtarget.h"
#include "AIETargetMachine.h"
#include "MCTargetDesc/AIEFormat.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/LivePhysRegs.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/MC/MCInstrItineraries.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Automaton.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include <limits>

#define DEBUG_TYPE "aie-codegen"

using namespace llvm;

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

unsigned
AIEBaseInstrInfo::getNumReservedDelaySlots(const MachineInstr &MI) const {
  return getNumDelaySlots(MI);
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

void AIEBaseInstrInfo::expandSpillPseudo(MachineInstr &MI,
                                         const TargetRegisterInfo &TRI,
                                         Align SubRegOffsetAlign) const {
  auto ExpandInfos = getSpillPseudoExpandInfo(MI);
  if (ExpandInfos.empty()) {
    // Nothing to expand
    return;
  }

  const MachineOperand &RegOp = MI.getOperand(0);
  Register Reg = RegOp.getReg();
  unsigned RegFlags = computeRegStateFlags(RegOp);
  int64_t Offset = MI.getOperand(1).getImm();
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
    const int BitsInByte = 8;

    unsigned MemorySize = 0;
    if (EI.MemSize) {
      MemorySize = EI.MemSize;
    } else {
      assert(EI.SubRegIndex);
      MemorySize = divideCeil(TRI.getSubRegIdxSize(EI.SubRegIndex), BitsInByte);
      MemorySize = alignTo(MemorySize, SubRegOffsetAlign);
    }
    assert(MemorySize && "Spill instruction uses no memory ?!");

    // Only expand into an instruction if the register is live, or if the
    // instruction cannot be removed due to ordering.
    Register SubReg =
        EI.SubRegIndex ? Register(TRI.getSubReg(Reg, EI.SubRegIndex)) : Reg;
    if (MI.hasOrderedMemoryRef() || SEH.isLiveReg(SubReg)) {
      MachineInstrBuilder MIB = BuildMI(MBB, MI, DL, get(EI.ExpandedOpCode))
                                    .addReg(SubReg, RegFlags)
                                    .addImm(MemOffset);

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
    expandSpillPseudo(*ExpandedMI, TRI, SubRegOffsetAlign);
}

/// Collect all the "atomic" sub-registers that make up \ref Reg.
/// Atomic means that the registers inserted in \ref SubRegs won't have any
/// sub-registers according to \ref TRI.
/// Note that based on how sub-registers are defined, there might be aliasing
/// registers in \ref SubRegs.
static void collectSubRegs(MCRegister Reg, SmallSet<MCRegister, 8> &SubRegs,
                           const TargetRegisterInfo &TRI) {
  if (TRI.subregs(Reg).empty()) {
    SubRegs.insert(Reg);
  } else {
    for (MCRegister SubReg : TRI.subregs(Reg))
      collectSubRegs(SubReg, SubRegs, TRI);
  }
}

void AIEBaseInstrInfo::copyThroughSubRegs(MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator MBBI,
                                          const DebugLoc &DL, MCRegister DstReg,
                                          MCRegister SrcReg,
                                          bool KillSrc) const {
  MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
  const TargetRegisterInfo &TRI = *MRI.getTargetRegisterInfo();

  SmallSet<MCRegister, 8> SrcSubRegs;
  collectSubRegs(SrcReg, SrcSubRegs, TRI);

  for (MCRegister SrcSubReg : SrcSubRegs) {
    unsigned SubRegIdx = TRI.getSubRegIndex(SrcReg, SrcSubReg);
    MCRegister DstSubReg = TRI.getSubReg(DstReg, SubRegIdx);
    copyPhysReg(MBB, MBBI, DL, DstSubReg, SrcSubReg, KillSrc);
  }
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

  unsigned SrcClass = SrcMI.getDesc().getSchedClass();
  unsigned DstClass = DstMI.getDesc().getSchedClass();
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
    // consumer
    SrcCycleVal -=
        getNumBypassedCycles(ItinData, SrcMI, SrcOpIdx, DstMI, DstOpIdx);
  }
  if (Kind == SDep::Anti) {
    // "Reverse" case: we need to penalize WAR dependencies, because if the
    // bypass is taken by the write, then the read could get the newly-written
    // value, not respecting the WAR edge. See negative_latencies/bypass.mir
    DstCycleVal -=
        getNumBypassedCycles(ItinData, DstMI, DstOpIdx, SrcMI, SrcOpIdx);
  }

  int Diff = SrcCycleVal - DstCycleVal;
  int Latency = Kind == SDep::Anti ? (Diff) : (Diff + 1);
  return Latency;
}

unsigned AIEBaseInstrInfo::getNumBypassedCycles(
    const InstrItineraryData *ItinData, const MachineInstr &DefMI,
    unsigned DefIdx, const MachineInstr &UseMI, unsigned UseIdx) const {
  // FIXME: This assumes one cycle benefit for every pipeline forwarding.
  return ItinData->hasPipelineForwarding(
             DefMI.getDesc().getSchedClass(), DefIdx,
             UseMI.getDesc().getSchedClass(), UseIdx)
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

unsigned
AIEBaseInstrInfo::getSchedClass(const MCInstrDesc &Desc,
                                iterator_range<const MachineOperand *> Operands,
                                const MachineRegisterInfo &MRI) const {
  return Desc.getSchedClass();
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

bool AIEBaseInstrInfo::verifyTiedRegisters(const MachineInstr &MI,
                                           StringRef &ErrInfo) const {
  const TargetSubtargetInfo &ST = MI.getMF()->getSubtarget();
  const TargetRegisterInfo &TRI = *ST.getRegisterInfo();
  auto VerifyTiedReg = [&](const MachineOperand &Op,
                           const MachineOperand &TiedOp,
                           unsigned ExpectedSubReg) {
    if (!Op.isReg() || !TiedOp.isReg()) {
      ErrInfo = "Tied operand must be a register";
      return false;
    }
    if (!Register::isPhysicalRegister(Op.getReg()) ||
        !Register::isPhysicalRegister(TiedOp.getReg()))
      return true;
    auto TiedReg = ExpectedSubReg
                       ? TRI.getSubReg(TiedOp.getReg(), ExpectedSubReg)
                       : TiedOp.getReg().asMCReg();
    if (Op.getReg() != TiedReg) {
      ErrInfo = "Tied physical registers must match";
      return false;
    }
    if (Op.isRenamable()) {
      ErrInfo = "Tied register is renamable";
      return false;
    }
    return TargetInstrInfo::verifyInstruction(MI, ErrInfo);
  };

  for (const TiedRegOperands &Regs : getTiedRegInfo(MI)) {
    const MachineOperand &SrcOp = MI.getOperand(Regs.SrcOp.OpIdx);
    if (!VerifyTiedReg(SrcOp, SrcOp, Regs.SrcOp.SubRegIdx))
      return false;
    for (const OperandSubRegMapping &DstOp : Regs.DstOps) {
      if (!VerifyTiedReg(MI.getOperand(DstOp.OpIdx), SrcOp, DstOp.SubRegIdx))
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
  if (RC && TRC.hasSubClassEq(RC))
    return true;
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
