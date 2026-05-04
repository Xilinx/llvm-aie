//=== lib/CodeGen/GlobalISel/AIECombinerHelper.cpp
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIECombinerHelper.h"
#include "AIE.h"
#include "AIEBaseInstrInfo.h"
#include "AIEBaseSubtarget.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/Analysis/VectorUtils.h"
#include "llvm/CodeGen/GlobalISel/GenericMachineInstrs.h"
#include "llvm/CodeGen/GlobalISel/LegalizerHelper.h"
#include "llvm/CodeGen/GlobalISel/Utils.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegionInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/IR/IntrinsicsAIE2.h"
#include "llvm/IR/IntrinsicsAIE2P.h"
#include "llvm/IR/IntrinsicsAIE2PS.h"
#include "llvm/Support/Alignment.h"
#include "llvm/Support/ErrorHandling.h"
#include <optional>

#define DEBUG_TYPE "aie-combine"

using namespace llvm;

static cl::opt<unsigned> ShuffleMaxNumInsertions(
    "aie-shuffle-combine-max-inserts", cl::Hidden, cl::init(0),
    cl::desc(
        "Maximum number of insertions allowed to scalarize a shuffle pattern"));
static cl::opt<bool> EnableOffsetCombine(
    "aie-offset-combine", cl::Hidden, cl::init(true),
    cl::desc("Enable combines of load and stores with an offset"));
static cl::opt<bool> EnablePostIncCombine(
    "aie-postinc-combine", cl::Hidden, cl::init(true),
    cl::desc("Enable combines of load and stores with post increments"));
static cl::opt<bool> EnableGreedyAddressCombine(
    "aie-greedy-address-combines", cl::Hidden, cl::init(false),
    cl::desc("Enable greedy combines without checking for later uses of the "
             "base pointer"));

static cl::opt<bool> PreferBroadcastOverInsert(
    "aie-prefer-broadcast-over-insert", cl::Hidden, cl::init(true),
    cl::desc("Use broadcast rather than insert-in-undefined to create "
             "scalar values in vector"));

cl::opt<bool> InlineMemCalls("aie-inline-mem-calls", cl::init(true), cl::Hidden,
                             cl::desc("Inline mem calls when profitable."));

cl::opt<bool> FoldInvariantLoads(
    "aie-try-fold-loads", cl::init(true), cl::Hidden,
    cl::desc("Try to fold loads when the data can be proved as constant."));

cl::opt<bool> CombineVecShiftByZero(
    "aie-combine-vec-shift-by-zero", cl::init(true), cl::Hidden,
    cl::desc("Combine vectors shift by zero into copies."));

cl::opt<bool> MemsetOptimizations(
    "aie-optimize-memsets", cl::init(true), cl::Hidden,
    cl::desc("Apply memset optimizations (peeling/align/etc.)."));

static cl::opt<bool> CombineRedundantWidenNarrowConversions(
    "aie-combine-redundant-widen-narrow-conversions", cl::init(true),
    cl::Hidden,
    cl::desc("Eliminate redundant exact widen/narrow conversion pairs"));

namespace {

const LLT S8 = LLT::scalar(8);
const LLT S16 = LLT::scalar(16);
const LLT S20 = LLT::scalar(20);
const LLT S32 = LLT::scalar(32);
const LLT V32S16 = LLT::fixed_vector(32, 16);

const llvm::AIEBaseInstrInfo &getAIETII(MachineIRBuilder &B) {
  return static_cast<const AIEBaseInstrInfo &>(B.getTII());
}

bool isGenericExtractOpcode(unsigned Opc, const AIEBaseInstrInfo &TII) {
  // Check if it's either SEXT or ZEXT extract
  const unsigned ExtractSextOpc = TII.getGenericExtractVectorEltOpcode(true);
  if (Opc == ExtractSextOpc) {
    return true;
  }
  const unsigned ExtractZextOpc = TII.getGenericExtractVectorEltOpcode(false);
  return Opc == ExtractZextOpc;
}

/// We conservatively implement only known cases.
bool mayMIShiftElements(const MachineInstr *MI) {
  switch (MI->getOpcode()) {
  case TargetOpcode::G_FMUL:
  case TargetOpcode::G_FADD:
  case TargetOpcode::G_FSUB:
    return false;
  case TargetOpcode::G_INTRINSIC:
  case TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS: {
    switch (cast<GIntrinsic>(MI)->getIntrinsicID()) {
    case Intrinsic::aie2_v16accfloat_to_v16bf16:
    case Intrinsic::aie2p_v16accfloat_to_v16bf16:
    case Intrinsic::aie2p_v32accfloat_to_v32bf16:
    case Intrinsic::aie2p_I512_I512_ACC1024_bf_mul_conf:
      return false;
    }
  }
  default:
    return true;
  }
}

/// Verify that all uses of a broadcast vector through a chain of operations
/// only extract from position 0. The chain may include G_CONCAT_VECTORS,
/// G_UNMERGE_VALUES, and vector operations.
/// \param Reg The register to verify uses for
/// \param MRI Machine register info
/// \param TII Target instruction info
/// \return true if all uses only extract position 0
bool verifyBroadcastUsesOnlyExtractZero(Register Reg, MachineRegisterInfo &MRI,
                                        const AIEBaseInstrInfo &TII) {
  if (!MRI.hasOneNonDBGUser(Reg))
    return false;

  MachineInstr *UserMI = &*MRI.use_nodbg_instructions(Reg).begin();
  unsigned Opcode = UserMI->getOpcode();

  // For concat, Reg should be the first src operand.
  if (Opcode == TargetOpcode::G_CONCAT_VECTORS) {
    if (UserMI->getOperand(1).getReg() != Reg)
      return false;
    return verifyBroadcastUsesOnlyExtractZero(UserMI->getOperand(0).getReg(),
                                              MRI, TII);
    // For unmerge, the useful operand should be the first one,
    // the other ones, they should be dead.
  }
  if (Opcode == TargetOpcode::G_UNMERGE_VALUES) {
    unsigned OpCount = 0;
    for (auto &MO : UserMI->defs()) {
      Register DefReg = MO.getReg();
      if (OpCount == 0 && !MRI.hasOneUse(DefReg))
        return false;
      if (OpCount && !MRI.use_empty(DefReg))
        return false;
      OpCount++;
    }
    return verifyBroadcastUsesOnlyExtractZero(UserMI->getOperand(0).getReg(),
                                              MRI, TII);
    // If we extract from zero, we succeed, otherwise we fail.
  }
  if (isGenericExtractOpcode(Opcode, TII)) {
    const Register UseIdxReg = UserMI->getOperand(2).getReg();
    auto UseIdx = getIConstantVRegValWithLookThrough(UseIdxReg, MRI);
    return UseIdx && UseIdx->Value.getZExtValue() == 0;
    // If we bitcast, we may need other lanes.
  }
  if (Opcode == TargetOpcode::G_BITCAST) {
    return false;
  }
  if (mayMIShiftElements(UserMI)) {
    return false;
  }

  return verifyBroadcastUsesOnlyExtractZero(UserMI->getOperand(0).getReg(), MRI,
                                            TII);
}

Register buildInsertInUndef(MachineIRBuilder &B, Register Src, LLT VecTy) {
  auto *MRI = B.getMRI();
  if (MRI->getType(Src) != S32) {
    Src = B.buildAnyExt(S32, Src).getReg(0);
  }
  const AIEBaseInstrInfo &TII = getAIETII(B);
  const Register IdxReg = B.buildConstant(S32, 0).getReg(0);
  const Register UndefVec = B.buildUndef(VecTy).getReg(0);
  const unsigned InsertEltOpc = TII.getGenericInsertVectorEltOpcode();
  return B.buildInstr(InsertEltOpc, {VecTy}, {UndefVec, Src, IdxReg}).getReg(0);
}

Register buildBroadcast(MachineIRBuilder &B, Register Src, LLT VecTy) {
  auto *MRI = B.getMRI();
  if (MRI->getType(Src) != S32) {
    Src = B.buildAnyExt(S32, Src).getReg(0);
  }
  const AIEBaseInstrInfo &TII = getAIETII(B);
  const unsigned InsertEltOpc = TII.getGenericBroadcastVectorOpcode();
  return B.buildInstr(InsertEltOpc, {VecTy}, {Src}).getReg(0);
}

Register buildScalarAsVector(MachineIRBuilder &B, Register Src, LLT VecTy) {
  return PreferBroadcastOverInsert ? buildBroadcast(B, Src, VecTy)
                                   : buildInsertInUndef(B, Src, VecTy);
}

// Build an element-wise multiplication into a vector of double width. These are
// typical MAC operations with the incoming accumulator configured to be zero.
// If Negate is true, uses the negating multiply intrinsic.
Register buildWidenMulScalarAsVector(MachineIRBuilder &B, Register Lft,
                                     Register Rgt, bool Negate) {
  // Mode and intrinsic are target dependent.
  auto *MRI = B.getMRI();
  const int MulMode1x1 = 60;
  LLT InTy = MRI->getType(Lft);
  LLT OutTy = InTy.changeElementSize(InTy.getScalarSizeInBits() * 2);
  const Register Acc = B.getMRI()->createGenericVirtualRegister(OutTy);
  const Register Mode = B.buildConstant(S32, MulMode1x1).getReg(0);

  // Choose the appropriate intrinsic based on whether we need negation.
  // Both bf_mul_conf and bf_negmul_conf use the same mode parameter, which
  // controls data types and multiplication configuration (see VecConf in
  // AIE2PInstrPatterns.td). The intrinsic opcode controls the negation
  // behavior via the dynMulNeg bit in the underlying instruction.
  const Intrinsic::ID IntrID =
      Negate ? Intrinsic::aie2p_I512_I512_ACC1024_bf_negmul_conf
             : Intrinsic::aie2p_I512_I512_ACC1024_bf_mul_conf;

  B.buildIntrinsic(IntrID, Acc, true, false)
      .addUse(Lft)
      .addUse(Rgt)
      .addUse(Mode);
  return Acc;
}

Register buildGetFirstElement(MachineIRBuilder &B, Register Vec) {
  auto *MRI = B.getMRI();
  const LLT DstTy = MRI->getType(Vec).getElementType();
  const AIEBaseInstrInfo &TII = getAIETII(B);
  const Register Index = B.buildConstant(S32, 0).getReg(0);
  return B
      .buildInstr(TII.getGenericExtractVectorEltOpcode(/*SignExt*/ true),
                  {DstTy}, {Vec, Index})
      .getReg(0);
}

unsigned getNumMaskUndefs(const ArrayRef<int> &Mask, unsigned StartIndex) {
  unsigned Count = 0;
  for (unsigned I = StartIndex; I < Mask.size(); ++I) {
    if (Mask[I] == -1) {
      ++Count;
    }
  }
  return Count;
}

} // namespace

bool MaskMatch::isValidMask(const ArrayRef<int> Mask) const {
  for (unsigned Idx = 0; Idx < Mask.size(); ++Idx) {
    if (Mask[Idx] == -1)
      continue;

    // Check not undef values (not -1) of the mask
    if ((unsigned)Mask[Idx] != getMaskValue(Idx)) {
      return false;
    }
  }
  return true;
}

ShuffleMaskValidity
MaskMatch::getShuffleMaskValidity(const ArrayRef<int> Mask) const {
  SmallVector<unsigned, 4> MaskExceptions;
  for (unsigned Idx = 0; Idx < Mask.size(); ++Idx) {
    if (Mask[Idx] == -1)
      continue;

    // Check not undef values (not -1) of the mask
    if ((unsigned)Mask[Idx] != getMaskValue(Idx)) {
      MaskExceptions.push_back(Idx);
    }
  }

  if (MaskExceptions.empty())
    return {true, MaskExceptions};
  return {false, MaskExceptions};
}

bool MaskMatch::isMaskWithAllUndefs(ArrayRef<int> Mask) {
  for (unsigned I = 0; I < Mask.size(); ++I) {
    if (Mask[I] != -1)
      return false;
  }
  return true;
}

// Checks if the mask is in range, allowing some values to be
// undef (-1), but not all.
bool MaskMatch::isMaskWithinRangeOrUndef(ArrayRef<int> Mask, int MinValue,
                                         int MaxValue) {
  return llvm::any_of(Mask, [](int v) { return v != -1; }) &&
         llvm::all_of(Mask, [=](int v) {
           return v == -1 || (v >= MinValue && v <= MaxValue);
         });
}

std::optional<unsigned> MaskMatch::getHeight(ArrayRef<int> Mask,
                                             unsigned Period) {
  for (unsigned I = 0; I < Mask.size(); ++I) {
    if (Mask[I] != -1)
      return Mask[I] - (Period == 0 ? I : I % Period);
  }
  return std::nullopt;
}

/// This function returns the unique index in the shuffle mask \p Mask if the
/// unique index exists.
std::optional<int> MaskMatch::getUniqueIndex(ArrayRef<int> Mask) {
  std::optional<int> UniqOpIdx;
  for (unsigned I = 0; I < Mask.size(); I++) {
    int Idx = Mask[I];
    if (Idx == -1)
      continue;

    if (!UniqOpIdx) {
      UniqOpIdx = Idx;
      continue;
    }

    if (UniqOpIdx != Idx) {
      return std::nullopt;
    }
  }
  return UniqOpIdx;
}

static std::unordered_map<int, unsigned>
getMaskFrequencyMap(const ArrayRef<int> Mask) {
  assert(!MaskMatch::isMaskWithAllUndefs(Mask));
  std::unordered_map<int, unsigned> FrequencyMap;
  for (int Idx : Mask) {
    if (Idx == -1)
      continue;
    FrequencyMap[Idx]++;
  }
  return FrequencyMap;
}

std::optional<FrequentIndexResult>
MaskMatch::getFrequentIndexResult(const ArrayRef<int> Mask,
                                  unsigned MinFrequency = 0) {

  // Set the default value for MinFrequency
  if (MinFrequency == 0) {
    MinFrequency = Mask.size() / 2;
  }

  std::unordered_map<int, unsigned> FrequencyMap = getMaskFrequencyMap(Mask);
  unsigned DontCareCount = getNumMaskUndefs(Mask, 0);

  auto [FrequentValue, HighestFrequency] = *std::max_element(
      FrequencyMap.begin(), FrequencyMap.end(),
      [](const std::pair<int, unsigned> p1, const std::pair<int, unsigned> p2) {
        return p1.second < p2.second;
      });

  unsigned HighestAdjustedFrequency = HighestFrequency + DontCareCount;
  if (HighestAdjustedFrequency < MinFrequency) {
    return std::nullopt;
  }

  unsigned NonMatchingCount = Mask.size() - HighestAdjustedFrequency;

  unsigned FrequentIdx = 0;
  for (unsigned I = 0; I < Mask.size(); I++) {
    int MaskValue = Mask[I];
    if (MaskValue == FrequentValue) {
      FrequentIdx = I;
      break;
    }
  }

  return FrequentIndexResult{FrequentIdx, NonMatchingCount};
}

static MachineInstr *findPreIncMatch(MachineInstr &MemI,
                                     MachineRegisterInfo &MRI,
                                     CombinerHelper &Helper,
                                     const AIEBaseInstrInfo &TII,
                                     AIE::FoundCombiners *GlobalCombinerPtr) {
  // This is currently done with patterns in instruction selection.
  // No need to do it here.
  const unsigned VecSize =
      MRI.getType(MemI.getOperand(0).getReg()).getSizeInBits();
  if (VecSize > TII.getMaxSupportedLdStIncSize()) {
    return nullptr;
  }

  if (!EnableOffsetCombine)
    return nullptr;
  Register Addr = MemI.getOperand(1).getReg();
  MachineInstr *AddrDef = getDefIgnoringCopies(Addr, MRI);
  if (AddrDef->getOpcode() == TargetOpcode::G_PTR_ADD) {
    // 2 Instructions are in the Combiner
    BitVector RemoveInstrs(2);
    GlobalCombinerPtr->append(AIE::Combiner(
        /*CombineInstrs=*/std::vector<MachineInstr *>{AddrDef, &MemI},
        /*CombinedInstrOpcode=*/TII.getOffsetMemOpcode(MemI.getOpcode()),
        /*InsertionPoint=*/&MemI, /*CombineRoot=*/&MemI,
        /*MoveUpInstrsToInsertionPoint=*/std::vector<MachineInstr *>{},
        /*RemoveInstrs=*/RemoveInstrs, /*Name=*/"Offset-legacy"));
    return AddrDef;
  }
  return nullptr;
}

/// Checks if any operand of \a MI is defined by \a Def.
/// This is not transitive: it will not look at how the uses of \a Def are
/// defined.
bool isUseOf(const MachineInstr &MI, const MachineInstr &Def) {
  for (auto &Defs : Def.all_defs()) {
    for (auto &MIUse : MI.all_uses()) {
      if (MIUse.isReg() && Defs.getReg() == MIUse.getReg())
        return true;
    }
  }
  return false;
}

/// Combines G_AIE_UNPAD_VECTOR and G_UNMERGE_VALUES operating on the same
/// source by reordering them to enable further optimizations.
///
/// Pattern matched:
///   %src:_(<16 x s32>) = ...
///   %unpad:_(<4 x s32>) = G_AIE_UNPAD_VECTOR %src
///   %a:_(<2 x s32>), %b:_(<2 x s32>) = G_UNMERGE_VALUES %src
///
/// Transforms to:
///   %src:_(<16 x s32>) = ...
///   %new_a:_(<2 x s32>), %new_b:_(<2 x s32>) = G_UNMERGE_VALUES %src
///   %unpad:_(<4 x s32>) = COPY %new_a
///   %a:_(<2 x s32>) = COPY %new_a
///   %b:_(<2 x s32>) = COPY %new_b
bool llvm::matchUnpadUnmerge(MachineInstr &UnpadMI, MachineRegisterInfo &MRI,
                             const AIEBaseInstrInfo &TII,
                             CombinerHelper &Helper,
                             GISelChangeObserver &Observer,
                             BuildFnTy &MatchInfo) {
  assert(UnpadMI.getOpcode() == TII.getGenericUnpadVectorOpcode() &&
         "Expected G_AIE_UNPAD_VECTOR");

  // 1. Get UNPAD operands and types
  const Register UnpadDst = UnpadMI.getOperand(0).getReg();
  const Register UnpadSrc = UnpadMI.getOperand(1).getReg();
  const LLT UnpadDstTy = MRI.getType(UnpadDst);

  // 2. Find G_UNMERGE_VALUES on same source
  MachineInstr *UnmergeMI = nullptr;
  for (MachineInstr &UseMI : MRI.use_nodbg_instructions(UnpadSrc)) {
    if (UseMI.getOpcode() == TargetOpcode::G_UNMERGE_VALUES) {
      UnmergeMI = &UseMI;
      break;
    }
  }

  if (!UnmergeMI)
    return false;

  // 3. Verify type compatibility
  const unsigned NumUnmergeOutputs = UnmergeMI->getNumDefs();
  const LLT UnmergeDst0Ty = MRI.getType(UnmergeMI->getOperand(0).getReg());

  // UNPAD output must match first UNMERGE output
  if (UnpadDstTy != UnmergeDst0Ty)
    return false;

  // Collect all UNMERGE output registers
  SmallVector<Register, 4> UnmergeDsts;
  for (unsigned I = 0; I < NumUnmergeOutputs; ++I) {
    UnmergeDsts.push_back(UnmergeMI->getOperand(I).getReg());
  }

  // Case 1: UNPAD dominates UNMERGE
  const bool UnpadDominates = Helper.dominates(UnpadMI, *UnmergeMI);
  if (UnpadDominates) {
    MatchInfo = [&UnpadMI, UnmergeMI, UnpadDst, UnpadSrc, UnmergeDsts,
                 UnmergeDst0Ty, NumUnmergeOutputs, &MRI,
                 &Observer](MachineIRBuilder &B) {
      // Create new UNMERGE at UNPAD location
      B.setInstr(UnpadMI);
      SmallVector<Register, 4> NewDsts;
      for (unsigned I = 0; I < NumUnmergeOutputs; ++I) {
        NewDsts.push_back(MRI.createGenericVirtualRegister(UnmergeDst0Ty));
      }
      B.buildUnmerge(NewDsts, UnpadSrc);

      // Create copy to UNPAD destination
      B.buildCopy(UnpadDst, NewDsts[0]);

      // Create copies at UNMERGE location for its users
      B.setInsertPt(*UnmergeMI->getParent(), UnmergeMI->getIterator());
      for (unsigned I = 0; I < NumUnmergeOutputs; ++I) {
        B.buildCopy(UnmergeDsts[I], NewDsts[I]);
      }

      // Erase both original instructions
      Observer.erasingInstr(*UnmergeMI);
      UnmergeMI->eraseFromParent();
      Observer.erasingInstr(UnpadMI);
      UnpadMI.eraseFromParent();
    };
    return true;
  }

  const bool UnmergeDominates = Helper.dominates(*UnmergeMI, UnpadMI);
  // Case 2: UNMERGE dominates UNPAD
  if (UnmergeDominates) {
    MatchInfo = [&UnpadMI, UnpadDst, UnmergeDsts,
                 &Observer](MachineIRBuilder &B) {
      // Simply copy from first UNMERGE result to UNPAD destination
      B.setInstr(UnpadMI);
      B.buildCopy(UnpadDst, UnmergeDsts[0]);

      // Erase UNPAD
      Observer.erasingInstr(UnpadMI);
      UnpadMI.eraseFromParent();
    };

    return true;
  }

  return false;
}

///  Check for dead \a InBetweenMI MI and copy-like instructions that can be
///  coalesced once \a MemI and \a Dest are combined.
bool isNonCoalesceableUseOf(const MachineInstr &MemI,
                            const MachineInstr &InBetweenMI,
                            const MachineInstr &Dest,
                            const MachineRegisterInfo &MRI) {

  if (isTriviallyDead(InBetweenMI, MRI))
    return false;

  // We can delay an instruction after a copy, if the copy just
  // connects MemI and Dest. After combining, this copy will be dead.
  if (InBetweenMI.isCopy() &&
      MRI.hasOneNonDBGUse(InBetweenMI.getOperand(1).getReg()) &&
      MRI.hasOneNonDBGUse(InBetweenMI.getOperand(0).getReg())) {
    const MachineInstr *CopyOrignMI =
        MRI.getVRegDef(InBetweenMI.getOperand(1).getReg());
    const MachineInstr *CopyDestMI =
        &*MRI.use_instr_nodbg_begin(InBetweenMI.getOperand(0).getReg());
    if (CopyOrignMI == &MemI && CopyDestMI == &Dest)
      return false;
  }

  return isUseOf(InBetweenMI, MemI);
}

/// \return true if \a MemI can be moved just before \a Dest in order to allow
/// post-increment combining
bool llvm::canDelayMemOp(MachineInstr &MemI, MachineInstr &Dest,
                         const MachineRegisterInfo &MRI) {
  if (MemI.getParent() != Dest.getParent())
    return false;
  auto MII = std::next(MemI.getIterator());
  auto MIE = Dest.getIterator();
  auto InstrRange = make_range(MII, MIE);
  bool SawStore = MemI.mayStore();
  auto UnsafeToMovePast = [&](const MachineInstr &MI) {
    return isNonCoalesceableUseOf(MemI, MI, Dest, MRI) ||
           !MI.isSafeToMove(SawStore);
  };
  return none_of(InstrRange, UnsafeToMovePast);
}

/// \return true if \a Dest can be moved just after \a MemI in order to allow
/// combining
bool llvm::canAdvanceOp(MachineInstr &MemI, MachineInstr &Dest,
                        const MachineRegisterInfo &MRI,
                        bool SideEffectsAreChecked) {
  if (!SideEffectsAreChecked) {
    assert(Dest.getOpcode() != TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS &&
           "Cannot advance Dest MI with side effects");
    assert(!Dest.mayLoadOrStore() && "Cannot advance load/store Dest MI");
  }
  if (MemI.getParent() != Dest.getParent())
    return false;
  auto MII = std::next(MemI.getIterator());
  auto MIE = Dest.getIterator();
  auto InstrRange = make_range(MII, MIE);
  auto UnsafeToMoveBefore = [&](const MachineInstr &MI) {
    // Conditions that indicate it is unsafe to move:
    // 1 - G_INTRINSIC_W_SIDE_EFFECTS without explicit output, which may include
    // writing to a control register.
    // 2 - Crossing the definition of an input operand of Dest.
    return ((MI.getOpcode() == TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS &&
             MI.defs().empty()) ||
            isUseOf(Dest, MI));
  };
  return none_of(InstrRange, UnsafeToMoveBefore);
}

/// Find the def instruction for \p Reg, folding away any trivial copies and
/// bitcasts. May return nullptr if \p Reg is not a generic virtual register.
MachineInstr *
llvm::getDefIgnoringCopiesAndBitcasts(Register Reg,
                                      const MachineRegisterInfo &MRI) {

  MachineInstr *DefInstr = MRI.getVRegDef(Reg);

  auto IsSingleUseCopyOrBitcast = [&](const MachineInstr *MI) {
    return (MI->isCopy() ||
            (DefInstr->getOpcode() == TargetOpcode::G_BITCAST)) &&
           MRI.hasOneNonDBGUse(MI->getOperand(0).getReg());
  };

  auto UseVirtReg = [&](const MachineInstr *MI) {
    return MI->getOperand(1).getReg().isVirtual();
  };

  // No other use for this copy/bitcast.
  // Stop if we reach an use of a physical register.
  while (DefInstr && IsSingleUseCopyOrBitcast(DefInstr) && UseVirtReg(DefInstr))
    DefInstr = MRI.getVRegDef(DefInstr->getOperand(1).getReg());

  return DefInstr;
}

MachineInstr *findLastRegUseInBB(Register Reg, MachineInstr &IgnoreUser,
                                 MachineRegisterInfo &MRI,
                                 CombinerHelper &Helper,
                                 const AIEBaseInstrInfo &TII) {
  MachineInstr *LastRegUse = nullptr;
  for (auto &Use : MRI.use_nodbg_instructions(Reg)) {
    if (&Use == &IgnoreUser || Use.getParent() != IgnoreUser.getParent())
      continue;
    if (!LastRegUse || Helper.dominates(*LastRegUse, Use))
      LastRegUse = &Use;
  }
  return LastRegUse;
}

bool checkRegUsesDominate(Register Reg, MachineInstr &Instr,
                          MachineInstr &IgnoreUser, MachineRegisterInfo &MRI,
                          CombinerHelper &Helper, const AIEBaseInstrInfo &TII) {
  for (auto &Use : MRI.use_nodbg_instructions(Reg)) {
    if (&Use == &IgnoreUser)
      continue;
    if (!Helper.dominates(Use, Instr))
      return false;
  }
  return true;
}

MachineInstr *
findEarliestInsertPoint(MachineInstr &Instr, MachineInstr &NoMoveBeforeInstr,
                        bool NoMoveBeforeLastUse, MachineRegisterInfo &MRI,
                        CombinerHelper &Helper, const AIEBaseInstrInfo &TII) {
  // get all defs and possibly the end point
  // filter out the ones that are not in the BB of Instr and find the latest one
  // and return the insertion point after it
  assert(&Instr != &NoMoveBeforeInstr &&
         "NoMoveBeforeInstr and Instr have to be different");
  MachineInstr *EarliestInstrPos = nullptr;
  if (NoMoveBeforeInstr.getParent() == Instr.getParent()) {
    if (Helper.dominates(NoMoveBeforeInstr, Instr)) {
      // Since the Instr has to be after, NoMoveBeforeInstr can never be the
      // last instrucion of the BB
      EarliestInstrPos =
          &*next_nodbg(NoMoveBeforeInstr.getIterator(),
                       NoMoveBeforeInstr.getParent()->instr_end());
    } else {
      // If NoMoveBeforeInstr is after Instr we don't move Instr up
      return &Instr;
    }
  } else {
    // NoMoveBeforeInstr and Instr are in different BBs.
    // At the moment we don't combine instructions across BB boundaries and
    // we can just return the current position of the Instr. If in the future
    // we want to combine across BB boundaries this case will probably have
    // to be handled differently.
    return &Instr;
  }
  // Loop over all definitions of the uses of the instruction and make sure we
  // do not move past any of them (except G_CONSTANT which we can fix after
  // applying the move)
  for (auto &Use : Instr.uses()) {
    if (!Use.isReg())
      continue;
    MachineInstr *DefInstr = MRI.getUniqueVRegDef(Use.getReg());
    // Ignore moves past G_CONSTANT
    if (DefInstr->getOpcode() == TargetOpcode::G_CONSTANT)
      continue;
    if (DefInstr->getParent() == Instr.getParent() &&
        Helper.dominates(*EarliestInstrPos, *DefInstr)) {
      // Since the Def is in the same BB as Instr and Instr has to be afterwards
      // DefInstr can never be the last Instruction of the BB
      EarliestInstrPos = &*next_nodbg(DefInstr->getIterator(),
                                      DefInstr->getParent()->instr_end());
    }
    if (NoMoveBeforeLastUse) {
      // For some instructions we might not want to move before the latest use
      // of a register, e.g. if the instruction we are moving is going to change
      // the register inplace, as it is the case for PTR_ADDs.
      // %0 = $p0
      // %1 = LOAD %0
      // STORE $r0, [%0, #32]
      // %2 = PTR_ADD %0, #64
      // In this case for example we would not want to move the PTR_ADD before
      // the STORE because the PTR_ADD changes %0 inplace and if there is a
      // later use of %0 (in this case the STORE), then we would introduce an
      // unnecessary COPY. It is better to combine the LOAD and the PTR_ADD
      // after the STORE.
      MachineInstr *LastRegUse =
          findLastRegUseInBB(Use.getReg(), Instr, MRI, Helper, TII);
      if (LastRegUse && Helper.dominates(*LastRegUse, Instr) &&
          Helper.dominates(*EarliestInstrPos, *LastRegUse)) {
        // Since LastRegUse is in the same BB as Instr and it dominates Instr,
        // and both instruction are not the same there will always be an
        // instruction after LastRegUse
        EarliestInstrPos = &*next_nodbg(LastRegUse->getIterator(),
                                        LastRegUse->getParent()->instr_end());
      }
    }
  }
  return EarliestInstrPos;
}

static std::vector<MachineInstr *>
findConstantOffsetsToMove(MachineInstr &PtrAdd, MachineInstr &PtrAddInsertLoc,
                          const MachineRegisterInfo &MRI,
                          CombinerHelper &Helper) {
  // By moving the PtrAdd up without considering if we are moving past a
  // G_CONSTANT defining one of the uses of the PtrAdd we are generating
  // incorrect code (use before def). We have to search those G_CONSTANTs and
  // move them up as well.
  std::vector<MachineInstr *> GConstsToMove;

  for (auto Use : PtrAdd.uses()) {
    if (!Use.isReg())
      continue;
    MachineInstr *Def = MRI.getUniqueVRegDef(Use.getReg());
    if (Def->getOpcode() == TargetOpcode::G_CONSTANT &&
        Def->getParent() == PtrAdd.getParent() &&
        Helper.dominates(PtrAddInsertLoc, *Def))
      GConstsToMove.push_back(Def);
  }
  return GConstsToMove;
}

// Check that MI is after First and not after Last
static bool isBetween(MachineInstr &MI, MachineInstr &First, MachineInstr &Last,
                      CombinerHelper &Helper) {
  assert(First.getParent() == Last.getParent());
  // If it's in another block, it can't be between
  if (MI.getParent() != First.getParent()) {
    return false;
  }

  // We want First < MI && MI <= Last :=: !(MI <= First) && (MI <= Last)
  // and we have dominates(A, B) :=: A <= B
  return !Helper.dominates(MI, First) && Helper.dominates(MI, Last);
}

static MachineInstr *findPostIncMatch(MachineInstr &MemI,
                                      MachineRegisterInfo &MRI,
                                      CombinerHelper &Helper,
                                      const AIEBaseInstrInfo &TII,
                                      AIE::FoundCombiners *GlobalCombinerPtr) {
  if (!EnablePostIncCombine)
    return nullptr;

  const unsigned VecSize =
      MRI.getType(MemI.getOperand(0).getReg()).getSizeInBits();
  if (VecSize > TII.getMaxSupportedLdStIncSize()) {
    return nullptr;
  }
  // 2 Instructions are in the Combiner
  BitVector RemovePtrInc(2);
  // remove PtrInc
  RemovePtrInc.set(0);

  MachineInstr *InsertionPoint = nullptr;
  Register Addr = MemI.getOperand(1).getReg();
  AIE::Combiner TempCombiner;
  for (auto &PtrInc : MRI.use_nodbg_instructions(Addr)) {
    if (MemI.getParent() != PtrInc.getParent())
      continue;
    std::optional<unsigned> CombinedOpcode = TII.getCombinedPostIncOpcode(
        MemI, PtrInc, MRI.getType(MemI.getOperand(0).getReg()).getSizeInBits());
    if (!CombinedOpcode || isTriviallyDead(PtrInc, MRI))
      continue;
    // Find the closest location to the memory operation where the ptr_add can
    // be moved to.
    MachineInstr &PtrAddInsertLoc = *findEarliestInsertPoint(
        PtrInc, /*NoMoveBeforeInstr=*/MemI,
        /*NoMoveBeforeLastUse=*/true, MRI, Helper, TII);
    // If the PtrInc is defined before MemI, we need to make sure that there is
    // no use before def error if we combine the PtrInc into the MemI. We check
    // that none of the uses is between the PtrInc and the MemOp.
    if (Helper.dominates(PtrInc, MemI)) {
      if (any_of(MRI.use_nodbg_instructions(PtrInc.getOperand(0).getReg()),
                 [&](MachineInstr &PtrIncUse) {
                   return isBetween(PtrIncUse, PtrInc, MemI, Helper);
                 })) {
        continue;
      }
      InsertionPoint = &MemI;
      TempCombiner = AIE::Combiner(
          /*CombineInstrs=*/std::vector<MachineInstr *>{&PtrInc, &MemI},
          /*CombinedInstrOpcode=*/*CombinedOpcode,
          /*InsertionPoint=*/InsertionPoint, /*CombineRoot=*/&MemI,
          /*MoveUpInstrsToInsertionPoint=*/std::vector<MachineInstr *>{},
          /*RemoveInstrs=*/RemovePtrInc, /*Name=*/"PostInc1");

      // The offset of the PtrInc might be defined after MemI, in this case we
      // want to verify if it would be possible to insert the combined
      // instruction at the PtrInc instead of the location of MemI. Instruction
      // with side effects are also blocking: Loads, stores, calls, instructions
      // with side effects cannot be moved.
      // TODO: try move other instructions that block us from combining
    } else if (canDelayMemOp(MemI, PtrAddInsertLoc, MRI)) {
      // If Definition of the offset is a G_CONSTANT we have to move that
      // instruction up
      InsertionPoint = &PtrAddInsertLoc;
      TempCombiner = AIE::Combiner(
          /*CombineInstrs=*/std::vector<MachineInstr *>{&PtrInc, &MemI},
          /*CombinedInstrOpcode=*/*CombinedOpcode,
          /*InsertionPoint=*/InsertionPoint, /*CombineRoot=*/&MemI,
          /*MoveUpInstrsToInsertionPoint=*/
          findConstantOffsetsToMove(PtrInc, PtrAddInsertLoc, MRI, Helper),
          /*RemoveInstrs=*/RemovePtrInc, /*Name=*/"PostInc2");
    } else {
      LLVM_DEBUG(dbgs() << "    Ignoring candidate " << PtrInc);
      continue;
    }
    // Only combine postIncs if we know that the original pointer is not used
    // after the insertion point: all uses of the pointer must dominate the
    // insertion point.
    // TODO: This heuristic is very conservative and we should allow combines if
    // a combine does not dominate the insertion point but can never follow the
    // insertion point, e.g. being in a sibling BB.
    bool AddrUsesDominatesInsertPoint =
        checkRegUsesDominate(Addr, *InsertionPoint, PtrInc, MRI, Helper, TII);
    if (EnableGreedyAddressCombine || AddrUsesDominatesInsertPoint) {
      GlobalCombinerPtr->append(TempCombiner);
      return &PtrInc;
    }
  }
  return nullptr;
}

/// Checking the following Concat-Unmerge-PHI pattern:
/// bb.0
/// concatIn0 = ...
/// concatIn1 = ...
/// bb.1
/// 1 = phi 7, bb.1, concatIn0, bb.0
/// 2 = phi 10, bb.1 concatIn1, bb.0
/// 3 = G_CONCAT 1, 2
/// ....
/// 6 = ...
/// 7, 8 = G_UNMERGE 6
/// 9,10 = G_UNMERGE 6
/// \return unmerge source (6) if the pattern is valid and the sub-vector index
/// used [ 0 (7, 9) or 1 (8, 10)]. Start the pattern checking from \p ConcatI .
/// Follow \p UseOpIdx (0 to n-1) of G_CONCAT to identify the pattern. Collect
/// all relevant Results in \p MatchData .
static bool findUnmergeOrigin(MachineInstr &ConcatI, const unsigned UseOpIdx,
                              const MachineRegisterInfo &MRI,
                              CombinerHelper &Helper,
                              AIEConcatUnmergeCombineMatchData &MatchData) {

  auto FindPhiUnmergeComponents =
      [UseOpIdx, &Helper, &MRI, &MatchData](
          MachineInstr *PhiInst) -> std::pair<MachineInstr *, Register> {
    // Find Unmerge and Concat Components of the PHI node.
    MachineInstr *UnmergeI = nullptr;
    Register UnmergeDefReg;
    for (unsigned I = 1; I < PhiInst->getNumOperands(); I += 2) {
      Register IncomingReg = PhiInst->getOperand(I).getReg();
      MachineBasicBlock *MBB = PhiInst->getOperand(I + 1).getMBB();
      MachineInstr *IncomingMI = MRI.getVRegDef(IncomingReg);

      // Checking if IncomingMI dominates phi node.
      if (Helper.dominates(*IncomingMI, *PhiInst)) {

        if (MatchData.NewConcatMBB && MatchData.NewConcatMBB != MBB) {
          LLVM_DEBUG(dbgs() << "Parts of the Future Concat do not "
                               "originate in the same MBB.\n");
          return {};
        }

        // Concat Components.
        MatchData.NewConcatMBB = MBB;
        MatchData.ConcatSubVecs[UseOpIdx] = IncomingReg;
        continue;
      }

      if (IncomingMI->getOpcode() != TargetOpcode::G_UNMERGE_VALUES) {
        LLVM_DEBUG(dbgs() << "Could not find Unmerge Instruction\n");
        return {};
      }
      // Unmerge Components.
      if (UnmergeI) {
        LLVM_DEBUG(dbgs() << "Encountered too many Unmerge Instructions\n");
        return {};
      }
      UnmergeDefReg = IncomingReg;
      UnmergeI = IncomingMI;
    }
    return {UnmergeI, UnmergeDefReg};
  };

  // Find PHI node.
  const unsigned OperandIdx = UseOpIdx + 1;
  assert(ConcatI.getOperand(OperandIdx).isReg());
  const Register Reg = ConcatI.getOperand(OperandIdx).getReg();
  MachineInstr *PhiInst = MRI.getVRegDef(Reg);
  if (!PhiInst->isPHI() || !MRI.hasOneNonDBGUse(Reg))
    return false;

  auto [UnmergeInst, UnmergeDefReg] = FindPhiUnmergeComponents(PhiInst);

  if (!UnmergeInst || !MatchData.NewConcatMBB) {
    // Could not find Unmerge or Concat Components.
    return false;
  }

  const auto UnmergeSourceReg = UnmergeInst->uses().begin()->getReg();
  // Check consistency of MatchData.UnmergeSourceReg across different
  // UseOpIdx.
  if (MatchData.UnmergeSourceReg &&
      MatchData.UnmergeSourceReg != UnmergeSourceReg) {
    // Diverging Unmerge Source Registers.
    return false;
  }
  MatchData.UnmergeSourceReg = UnmergeSourceReg;

  const int UnmergeDefIdx =
      UnmergeInst->findRegisterDefOperandIdx(UnmergeDefReg, nullptr);
  assert(UnmergeDefIdx >= 0);
  // Make sure Unmerge and Concat do not reorder the subvectors.
  return UseOpIdx == (unsigned)UnmergeDefIdx;
}

/// This is the matching function for Concat-Unmerge-PHI pattern.
/// Convert the following:
/// bb.0
/// concatIn0 = ...
/// concatIn1 = ...
/// bb.1
/// 1 = phi 7, bb.1, concatIn0, bb.0
/// 2 = phi 8, bb.1 concatIn1, bb.0
/// 3 = G_CONCAT 1, 2
/// ....
/// 6 = ...
/// 7, 8 = G_UNMERGE 6
///
/// Into:
/// bb.0
/// concatIn0 = ...
/// concatIn1 = ...
/// 11 = G_CONCAT concatIn0, concatIn1
/// bb.1
/// 3 = phi 6, bb.1, 11, bb.0
/// ...
/// 6 =
///
/// \p ConcatI is the starting Point for this pattern.
/// \return true if the pattern is found.
bool llvm::matchConcatUnmergePhis(MachineInstr &ConcatI,
                                  MachineRegisterInfo &MRI,
                                  CombinerHelper &Helper,
                                  AIEConcatUnmergeCombineMatchData &MatchInfo) {
  assert(ConcatI.getOpcode() == TargetOpcode::G_CONCAT_VECTORS);
  LLVM_DEBUG(dbgs() << "MF: " << ConcatI.getMF()->getName() << "\n");

  const unsigned NumUses = ConcatI.getNumOperands() - ConcatI.getNumDefs();

  MatchInfo.ConcatSubVecs.resize(NumUses);

  for (unsigned UseOpIdx = 0; UseOpIdx < NumUses; UseOpIdx++) {
    if (!findUnmergeOrigin(ConcatI, /*UseOpIdx=*/UseOpIdx, MRI, Helper,
                           MatchInfo))
      return false;
  }

  assert(MatchInfo.NewConcatMBB);
  assert(MatchInfo.UnmergeSourceReg);
  return true;
}

void llvm::applyConcatUnmergePhis(MachineInstr &ConcatI,
                                  MachineRegisterInfo &MRI, MachineIRBuilder &B,
                                  AIEConcatUnmergeCombineMatchData &MatchInfo,
                                  GISelChangeObserver &Observer) {
  /// Set Insertion Point.
  if (MatchInfo.NewConcatMBB->empty())
    B.setMBB(*MatchInfo.NewConcatMBB);
  else
    B.setInstr(MatchInfo.NewConcatMBB->instr_back());

  // Create new Concat Instruction.
  const auto TargetVecType = MRI.getType(*MatchInfo.UnmergeSourceReg);
  Register ConcatReg = MRI.createGenericVirtualRegister(TargetVecType);
  auto NewConcat = B.buildMergeLikeInstr(ConcatReg, MatchInfo.ConcatSubVecs);
  LLVM_DEBUG(dbgs() << "In bb." << MatchInfo.NewConcatMBB->getNumber()
                    << " created " << *NewConcat.getInstr());

  // Set Insertion Point to top of phi-MBB.
  B.setInsertPt(*ConcatI.getParent(), ConcatI.getParent()->begin());

  // Create new PHI node.
  auto NewPHI = B.buildInstr(TargetOpcode::G_PHI);
  NewPHI.addDef(ConcatI.getOperand(0).getReg());

  // Add first PHI operand (newly create G_CONCAT).
  NewPHI.addUse(NewConcat.getInstr()->getOperand(0).getReg());
  NewPHI->addOperand(MachineOperand::CreateMBB(MatchInfo.NewConcatMBB));

  // Add second PHI operand (unmerge Components).
  NewPHI.addUse(*MatchInfo.UnmergeSourceReg);
  MachineBasicBlock *UnmergeMBB =
      MRI.getVRegDef(*MatchInfo.UnmergeSourceReg)->getParent();
  NewPHI->addOperand(MachineOperand::CreateMBB(UnmergeMBB));
  LLVM_DEBUG(dbgs() << "Created New Instruction " << *NewPHI.getInstr());
  Observer.erasingInstr(ConcatI);
  ConcatI.eraseFromParent();
}

bool llvm::matchGlobalPtrModOptimizer(MachineInstr &MemI,
                                      MachineRegisterInfo &MRI,
                                      CombinerHelper &Helper,
                                      const TargetInstrInfo &TII,
                                      AIE::FoundCombiners *GlobalCombinerPtr) {

  AIE::Combiner *CombineRule = GlobalCombinerPtr->getCombine(&MemI);
  if (!CombineRule) {
    LLVM_DEBUG(dbgs() << "[Global Ptr Inc] Could not find Combine for "
                      << MemI);
    return false;
  }
  assert(CombineRule->CombineInstrs.size() >= 2);
  LLVM_DEBUG(dbgs() << "[Global Ptr Inc] Found\n" << *CombineRule);

  return true;
}

bool llvm::matchLdStInc(MachineInstr &MemI, MachineRegisterInfo &MRI,
                        CombinerHelper &Helper, const TargetInstrInfo &TII,
                        AIE::FoundCombiners *GlobalCombinerPtr) {
  const AIEBaseInstrInfo &AIETII = (const AIEBaseInstrInfo &)TII;

  if (GlobalCombinerPtr->hasAnalysis())
    return false;

  return findPostIncMatch(MemI, MRI, Helper, AIETII, GlobalCombinerPtr) ||
         findPreIncMatch(MemI, MRI, Helper, AIETII, GlobalCombinerPtr);
}

void llvm::applyLdStInc(MachineInstr &MemI, MachineRegisterInfo &MRI,
                        CombinerHelper &Helper, MachineIRBuilder &B,
                        GISelChangeObserver &Observer,
                        AIE::FoundCombiners *GlobalCombinerPtr) {

  AIE::Combiner *CombineResult = GlobalCombinerPtr->getCombine(&MemI);
  assert(CombineResult);

  LLVM_DEBUG(dbgs() << "Applying Combiner "; CombineResult->dumpFull());

  MachineInstr *CombinedInsertionPoint = CombineResult->InsertionPoint;
  unsigned CombinedInstrOpcode = CombineResult->CombinedInstrOpcode;
  assert(CombinedInstrOpcode != (unsigned)-1 && "Invalid OpCode");

  if (CombinedInsertionPoint) {
    B.setInstr(*CombinedInsertionPoint);
  } else {
    B.setMBB(*MemI.getParent());
  }

  // Init combiner and get variables
  MachineInstr *PtrMod = CombineResult->CombineInstrs[0];
  bool RemovePtrMod = CombineResult->RemoveInstrs.any();

  // Debug Loc: Debug Loc of LOAD STORE: MI
  B.setDebugLoc(MemI.getDebugLoc());
  auto NewInstr = B.buildInstr(CombinedInstrOpcode);

  // move Instr right before the InsertionPoint
  for (auto *Instr : CombineResult->MoveUpInstrsToInsertionPoint) {
    if (!Instr->getParent())
      // Instr does not exist anymore, no need to move it
      continue;

    if (Helper.dominates(*Instr, *NewInstr))
      continue;

    Observer.changingInstr(*Instr);
    Instr->moveBefore(NewInstr);
    Observer.changedInstr(*Instr);
    LLVM_DEBUG(dbgs() << "Move Instr before " << *Instr);
  }

  // Move Instr past the InsertionPoint
  if (CombinedInsertionPoint) {
    for (auto *Instr : CombineResult->DelayInstrPastInsertionPoint) {
      if (!Instr->getParent())
        // Instruction may not exist anymore, i.e. a ptr_add that was combined
        // to a post increment Instruction
        continue;

      if (Helper.dominates(*CombinedInsertionPoint, *Instr))
        continue;

      LLVM_DEBUG(dbgs() << "Delaying Instr " << *Instr);
      Observer.changingInstr(*Instr);
      Instr->moveBefore(CombinedInsertionPoint);
      Observer.changedInstr(*Instr);
    }
  }

  if (MemI.mayLoad())
    NewInstr.addDef(MemI.getOperand(0).getReg() /* Loaded value */);
  if (RemovePtrMod)
    // If we remove the instr it is because we have defs that would otherwise
    // be redefined. We have to add these defs into the new instruction.
    for (auto Def : PtrMod->defs())
      if (Def.isReg())
        NewInstr.addDef(Def.getReg());
  if (MemI.getOpcode() == TargetOpcode::G_STORE)
    NewInstr.addUse(MemI.getOperand(0).getReg() /* Stored value */);
  for (auto Use : PtrMod->uses())
    if (Use.isReg())
      NewInstr.addUse(Use.getReg());
  for (auto *Mem : MemI.memoperands())
    NewInstr.addMemOperand(Mem);

  // keep track of Converted Instructions, so that delayInstructions are
  // properly keep track of
  GlobalCombinerPtr->createMapping(&MemI, NewInstr);

  LLVM_DEBUG(dbgs() << *NewInstr.getInstr());

  for (int Idx = CombineResult->RemoveInstrs.find_first(); Idx != -1;
       Idx = CombineResult->RemoveInstrs.find_next(Idx)) {
    auto *RemoveMI = CombineResult->CombineInstrs[Idx];

    // Removed Instructions have to be remapped to the newly Inserted
    // Instructions, so that they are considered when the Removed Instruction
    // should be moved up/down
    GlobalCombinerPtr->createMapping(RemoveMI, NewInstr);

    LLVM_DEBUG(dbgs() << "  Removing " << *RemoveMI);
    assert(RemoveMI->getParent() &&
           "RemoveMI was already deleted. This Combiner may have a conflict "
           "with the Combiner that already removed the MachineInstr.");
    // Notify observer, then remove from BB, then defer deletion.
    // We defer actual deallocation until end of combining pass because
    // later combiners may reference this instruction via the remapping
    // mechanism (createMapping/getMappedInstr).
    Observer.erasingInstr(*RemoveMI);
    RemoveMI->removeFromParent();
    GlobalCombinerPtr->deferDelete(RemoveMI);
  }

  // Notify observer, remove from BB, then defer deletion for root instruction.
  Observer.erasingInstr(MemI);
  MemI.removeFromParent();
  GlobalCombinerPtr->deferDelete(&MemI);
}

// Match all equivalents of these:
// %0:_(<16 x s32>) = G_IMPLICIT_DEF
// %1:_(s32) = G_IMPLICIT_DEF
// %2:_(<16 x s32>) =  G_AIE_ADD_VECTOR_ELT_HI %0, %1(s32)
//
// Combine into:
// %0:_(<16 x s32>) = G_IMPLICIT_DEF
// %2:_(<16 x s32>) = COPY %0
bool llvm::matchAddVecEltUndef(MachineInstr &MI, MachineRegisterInfo &MRI,
                               const TargetInstrInfo &TII) {
  const AIEBaseInstrInfo &AIETII = (const AIEBaseInstrInfo &)TII;
  assert(MI.getOpcode() == AIETII.getGenericAddVectorEltOpcode() &&
         "Expected a G_AIE_ADD_VECTOR_ELT_HI");
  const MachineInstr *SrcVecDef =
      getDefIgnoringCopies(MI.getOperand(1).getReg(), MRI);
  const MachineInstr *SrcEltDef =
      getDefIgnoringCopies(MI.getOperand(2).getReg(), MRI);

  if (SrcVecDef->getOpcode() != TargetOpcode::G_IMPLICIT_DEF ||
      SrcEltDef->getOpcode() != TargetOpcode::G_IMPLICIT_DEF)
    return false;

  return true;
}

void llvm::applyAddVecEltUndef(MachineInstr &MI, MachineRegisterInfo &MRI,
                               MachineIRBuilder &B,
                               GISelChangeObserver &Observer) {
  B.setDebugLoc(MI.getDebugLoc());
  B.buildCopy(MI.getOperand(0), MI.getOperand(1));
  Observer.erasingInstr(MI);
  MI.eraseFromParent();
}

//===----------------------------------------------------------------------===//
// combine_split_intrinsic_for_store
//===----------------------------------------------------------------------===//

/// Returns the split intrinsic ID for intrinsics that can be divided into
/// two smaller operations. This is used to optimize wide intrinsics that feed
/// stores by splitting them into narrower operations that may have better
/// instruction selection.
///
/// Currently supported:
/// - aie2ps_I512_v64_acc32_srs -> aie2ps_I256_v32_acc32_srs
///
/// \param OriginalID The intrinsic ID to check for splitting
/// \return The split intrinsic ID if supported, std::nullopt otherwise
///
/// NOTE: This list may be extended in the future with additional intrinsics
/// after proper benchmarking to ensure the split version provides performance
/// benefits over the original wide intrinsic.
static std::optional<Intrinsic::ID>
getSplitIntrinsic(Intrinsic::ID OriginalID) {
  switch (OriginalID) {
  case Intrinsic::aie2ps_I512_v64_acc32_srs:
    return Intrinsic::aie2ps_I256_v32_acc32_srs;
  // Future intrinsics can be added here after benchmarking
  default:
    return std::nullopt;
  }
}

/// Match and split wide intrinsics that feed stores into narrower operations.
/// This combiner runs in the pre-legalizer stage and handles intrinsics that
/// can be split into two half-width operations.
///
/// Pattern matched:
///   %result = G_INTRINSIC[_W_SIDE_EFFECTS] @wide_intrinsic, %inputs...
///   %bitcast = G_BITCAST %result
///   %lo, %hi = G_UNMERGE_VALUES %bitcast
///   G_STORE %lo, ...
///   G_STORE %hi, ...
///
/// Transforms to:
///   %acc_lo, %acc_hi = G_UNMERGE_VALUES %input_acc
///   %result_lo = G_INTRINSIC[_W_SIDE_EFFECTS] @split_intrinsic, %acc_lo, ...
///   %result_hi = G_INTRINSIC[_W_SIDE_EFFECTS] @split_intrinsic, %acc_hi, ...
///   %new_lo = G_BITCAST %result_lo
///   %new_hi = G_BITCAST %result_hi
///   G_STORE %new_lo, ...
///   G_STORE %new_hi, ...
bool llvm::matchSplitIntrinsicForStore(MachineInstr &MI,
                                       MachineRegisterInfo &MRI,
                                       const AIEBaseInstrInfo &TII,
                                       BuildFnTy &MatchInfo) {
  // 1. Verify this is an intrinsic and check if it can be split
  const unsigned Opcode = MI.getOpcode();
  if (Opcode != TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS &&
      Opcode != TargetOpcode::G_INTRINSIC)
    return false;

  const auto *IntrMI = cast<GIntrinsic>(&MI);
  const Intrinsic::ID IntrinsicID = IntrMI->getIntrinsicID();

  const auto SplitIntrinsicID = getSplitIntrinsic(IntrinsicID);
  if (!SplitIntrinsicID)
    return false;

  // 2. Get intrinsic output register and verify single use
  const Register IntrinsicOutReg = MI.getOperand(0).getReg();

  auto GetSingleOpcodeUse = [&MRI](Register Reg,
                                   unsigned Opcode) -> MachineInstr * {
    if (!MRI.hasOneNonDBGUse(Reg))
      return nullptr;
    MachineInstr *SingleMI = &*MRI.use_nodbg_instructions(Reg).begin();
    if (SingleMI && (SingleMI->getOpcode() == Opcode))
      return SingleMI;
    return nullptr;
  };

  // 3. Check that the single use is a BITCAST
  MachineInstr *BitcastMI =
      GetSingleOpcodeUse(IntrinsicOutReg, TargetOpcode::G_BITCAST);
  if (!BitcastMI)
    return false;

  const Register BitcastReg = BitcastMI->getOperand(0).getReg();

  // 4. Check that the single use is an UNMERGE
  MachineInstr *UnmergeMI =
      GetSingleOpcodeUse(BitcastReg, TargetOpcode::G_UNMERGE_VALUES);
  if (!UnmergeMI)
    return false;

  // 5. Verify UNMERGE produces exactly 2 results
  if (UnmergeMI->getNumDefs() != 2)
    return false;

  // 6. Get the two unmerge output registers
  const Register LoReg = UnmergeMI->getOperand(0).getReg();
  const Register HiReg = UnmergeMI->getOperand(1).getReg();

  if (!GetSingleOpcodeUse(LoReg, TargetOpcode::G_STORE) ||
      !GetSingleOpcodeUse(HiReg, TargetOpcode::G_STORE))
    return false;

  // 7. Extract intrinsic operands (first operand after the intrinsic ID)
  // For G_INTRINSIC_W_SIDE_EFFECTS: operand 0 = def, 1 = ID, 2+ = inputs
  // For G_INTRINSIC: operand 0 = def, 1 = ID, 2+ = inputs
  const Register AccReg = MI.getOperand(2).getReg();
  const Register ShiftReg = MI.getOperand(3).getReg();
  const Register SignReg = MI.getOperand(4).getReg();

  // 8. Derive types from the IR (no hardcoded types!)
  const LLT OrigAccTy = MRI.getType(AccReg);
  const LLT OrigIntrOutTy = MRI.getType(IntrinsicOutReg);

  // Calculate split types by dividing by 2
  const LLT AccHalfTy = OrigAccTy.divide(2);
  const LLT IntrOutHalfTy = OrigIntrOutTy.divide(2);

  // 9. Build the transformation
  // Note: We use applyBuildFnNoErase. We replace register uses and let DCE
  // clean up dead instructions.
  MatchInfo = [=, &MI, &MRI](MachineIRBuilder &B) {
    // Step 1: Unmerge the accumulator into two halves
    const Register AccLoReg = MRI.createGenericVirtualRegister(AccHalfTy);
    const Register AccHiReg = MRI.createGenericVirtualRegister(AccHalfTy);
    B.buildUnmerge({AccLoReg, AccHiReg}, AccReg);

    // Step 2: Create two split intrinsics using the ID from getSplitIntrinsic
    const bool HasSideEffects =
        (Opcode == TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS);

    const Register IntrOutLoReg =
        MRI.createGenericVirtualRegister(IntrOutHalfTy);
    B.buildIntrinsic(*SplitIntrinsicID, IntrOutLoReg, HasSideEffects,
                     /*isConvergent=*/false)
        .addUse(AccLoReg)
        .addUse(ShiftReg)
        .addUse(SignReg);

    const Register IntrOutHiReg =
        MRI.createGenericVirtualRegister(IntrOutHalfTy);
    B.buildIntrinsic(*SplitIntrinsicID, IntrOutHiReg, HasSideEffects,
                     /*isConvergent=*/false)
        .addUse(AccHiReg)
        .addUse(ShiftReg)
        .addUse(SignReg);

    // Step 3: Bitcast each intrinsic result to the store type
    B.buildBitcast(LoReg, IntrOutLoReg);
    B.buildBitcast(HiReg, IntrOutHiReg);

    MI.eraseFromParent();
    UnmergeMI->eraseFromParent();
    BitcastMI->eraseFromParent();
  };

  return true;
}

/// Get an s32/s20 value from an s20 register that comes from either:
/// 1. G_TRUNC of s32 -> returns the original s32 register
/// 2. G_ZEXTLOAD of s16 -> returns the s20 register (already zero-extended)
/// \param S20Reg The s20 register to extract the source from
/// \param MRI Machine register info
/// \param OnlyTruncs If true, only accept G_TRUNC patterns (not G_ZEXTLOAD)
/// Returns std::nullopt if the pattern doesn't match
/// This is not a generic function, this is a helper for some combiners.
static std::optional<Register> getSourceFromS20(Register S20Reg,
                                                MachineRegisterInfo &MRI,
                                                bool OnlyTruncs = false) {
  MachineInstr *DefMI = MRI.getVRegDef(S20Reg);
  if (!DefMI)
    return std::nullopt;

  const LLT S20Ty = MRI.getType(S20Reg);
  if (S20Ty != S20)
    return std::nullopt;

  // Case 1: G_TRUNC s32 -> s20
  if (DefMI->getOpcode() == TargetOpcode::G_TRUNC) {
    const Register SrcReg = DefMI->getOperand(1).getReg();
    const LLT SrcTy = MRI.getType(SrcReg);
    if (SrcTy == S32)
      return SrcReg;
  }

  // Case 2: G_ZEXTLOAD (loads s16, zero-extends to s20)
  // Skip this case if OnlyTruncs is true
  if (!OnlyTruncs && DefMI->getOpcode() == TargetOpcode::G_ZEXTLOAD) {
    if (!DefMI->memoperands_empty()) {
      const MachineMemOperand *MMO = *DefMI->memoperands_begin();
      if (MMO && MMO->getMemoryType() == S16) {
        // The s20 value from ZEXTLOAD is already zero-extended from s16
        return S20Reg;
      }
    }
  }

  return std::nullopt;
}

/// Set the insertion point of \p B to right after \p InsertionPoint,
/// skipping any PHI nodes that immediately follow it.
static void setInsertPtAfterInstr(MachineIRBuilder &B,
                                  MachineInstr *InsertionPoint) {
  MachineBasicBlock *MBB = InsertionPoint->getParent();
  MachineBasicBlock::iterator InsertPt =
      std::next(InsertionPoint->getIterator());
  if (InsertPt != MBB->end() && InsertPt->isPHI())
    InsertPt = MBB->getFirstNonPHI();
  B.setInsertPt(*MBB, *InsertPt);
}

/// Match a pattern of chained G_PTR_ADD operations where offsets come from
/// either G_TRUNC of s32 values or G_ZEXTLOAD of s16 values.
/// Combines them into a single PTR_ADD by adding the offsets in s32 space.
///
/// Patterns matched:
///   1. TRUNC + TRUNC:
///      %s20_1 = G_TRUNC %s32_1
///      %ptr_1 = G_PTR_ADD %base, %s20_1
///      %s20_2 = G_TRUNC %s32_2
///      %ptr_2 = G_PTR_ADD %ptr_1, %s20_2
///
///   2. ZEXTLOAD + TRUNC:
///      %s20_1 = G_ZEXTLOAD %ptr :: (load s16)
///      %ptr_1 = G_PTR_ADD %base, %s20_1
///      %s20_2 = G_TRUNC %s32_2
///      %ptr_2 = G_PTR_ADD %ptr_1, %s20_2
///
///   3. ZEXTLOAD + ZEXTLOAD:
///      %s20_1 = G_ZEXTLOAD %ptr1 :: (load s16)
///      %ptr_1 = G_PTR_ADD %base, %s20_1
///      %s20_2 = G_ZEXTLOAD %ptr2 :: (load s16)
///      %ptr_2 = G_PTR_ADD %ptr_1, %s20_2
///
/// Transforms to:
///   %s32_combined = G_ADD %s32_1, %s32_2  (with G_ZEXT if needed)
///   %s20_combined = G_TRUNC %s32_combined
///   %ptr_2 = G_PTR_ADD %base, %s20_combined
bool llvm::matchChainedPtrAddWithNonConstOffsets(MachineInstr &MI,
                                                 MachineRegisterInfo &MRI,
                                                 CombinerHelper &Helper,
                                                 BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_PTR_ADD && "Expected G_PTR_ADD");

  // This is the second PTR_ADD in the chain
  const Register SecondPtrAddDst = MI.getOperand(0).getReg();
  const Register SecondPtrAddBase = MI.getOperand(1).getReg();
  const Register SecondOffset = MI.getOperand(2).getReg();

  // Get Offset source for second offset (from TRUNC or ZEXTLOAD)
  auto SecondOffsetOpt = getSourceFromS20(SecondOffset, MRI);
  if (!SecondOffsetOpt)
    return false;
  const Register SecondOffsetReg = *SecondOffsetOpt;

  // Check if base comes from another G_PTR_ADD
  MachineInstr *FirstPtrAddMI = MRI.getVRegDef(SecondPtrAddBase);
  if (!FirstPtrAddMI || FirstPtrAddMI->getOpcode() != TargetOpcode::G_PTR_ADD)
    return false;

  // If we try to merge PADDs from different blocks, we may end-up de-hoisting
  // PADDs as ADD inside loops.
  if (MI.getParent() != FirstPtrAddMI->getParent())
    return false;

  const Register FirstPtrAddBase = FirstPtrAddMI->getOperand(1).getReg();
  const Register FirstOffset = FirstPtrAddMI->getOperand(2).getReg();

  // Get Offset source for first offset (from TRUNC or ZEXTLOAD)
  auto FirstOffsetOpt = getSourceFromS20(FirstOffset, MRI);
  if (!FirstOffsetOpt)
    return false;
  const Register FirstOffsetReg = *FirstOffsetOpt;

  // Get the definitions of both s32/s20 source registers
  MachineInstr *FirstOffsetDefMI = MRI.getVRegDef(FirstOffsetReg);
  MachineInstr *SecondOffsetDefMI = MRI.getVRegDef(SecondOffsetReg);

  if (!FirstOffsetDefMI || !SecondOffsetDefMI)
    return false;

  // Check dominance: we need one to dominate the other
  MachineInstr *InsertionPoint = nullptr;
  if (Helper.dominates(*FirstOffsetDefMI, *SecondOffsetDefMI)) {
    InsertionPoint = SecondOffsetDefMI;
  } else if (Helper.dominates(*SecondOffsetDefMI, *FirstOffsetDefMI)) {
    InsertionPoint = FirstOffsetDefMI;
  } else {
    return false;
  }

  // Verify insertion point dominates the final use
  if (!Helper.dominates(*InsertionPoint, MI))
    return false;

  // Build the transformation
  MatchInfo = [=, &MRI, &MI](MachineIRBuilder &B) {
    // Set insertion point right after the dominated definition,
    // skipping any PHI nodes that immediately follow it.
    setInsertPtAfterInstr(B, InsertionPoint);

    // Extend a register to S32 if it is currently S20 (from ZEXTLOAD).
    auto ExtendToS32 = [&](Register Reg) -> Register {
      if (MRI.getType(Reg) == S20) {
        Register Extended = MRI.createGenericVirtualRegister(S32);
        B.buildZExt(Extended, Reg);
        return Extended;
      }
      return Reg;
    };

    // Handle the case where one or both values are s20 (from ZEXTLOAD)
    // We need to extend them to s32 before adding
    const Register FirstS32Extended = ExtendToS32(FirstOffsetReg);
    const Register SecondS32Extended = ExtendToS32(SecondOffsetReg);

    // Build G_ADD of the two s32 values
    const Register CombinedS32 = MRI.createGenericVirtualRegister(S32);
    B.buildAdd(CombinedS32, FirstS32Extended, SecondS32Extended);

    // Build G_TRUNC to s20
    const Register CombinedS20 = MRI.createGenericVirtualRegister(S20);
    B.buildTrunc(CombinedS20, CombinedS32);

    // Build the combined PTR_ADD at the location of the root (second PTR_ADD)
    B.setInstr(MI);
    B.buildPtrAdd(SecondPtrAddDst, FirstPtrAddBase, CombinedS20);
  };

  return true;
}

/// Match a pattern of G_AIE_POSTINC_LOAD/STORE followed by G_PTR_ADD where both
/// offsets come from G_TRUNC of s32 values. Combines them by updating the
/// POSTINC to use the combined offset.
bool llvm::matchPostIncLoadStorePtrAddWithTrunc(MachineInstr &MI,
                                                MachineRegisterInfo &MRI,
                                                CombinerHelper &Helper,
                                                const AIEBaseInstrInfo &TII,
                                                GISelChangeObserver &Observer,
                                                BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_PTR_ADD && "Expected G_PTR_ADD");

  // This is the PTR_ADD that follows the POSTINC_LOAD
  const Register PtrAddDst = MI.getOperand(0).getReg();
  const Register PtrAddBase = MI.getOperand(1).getReg();
  const Register PtrAddOffset = MI.getOperand(2).getReg();

  LLVM_DEBUG(dbgs() << "Checking POSTINC_MEMOP+PTR_ADD pattern for: " << MI);

  // Get Offset source for PTR_ADD offset (only from TRUNC, not ZEXTLOAD)
  auto PtrAddOffsetOpt =
      getSourceFromS20(PtrAddOffset, MRI, /*OnlyTruncs=*/true);

  if (!PtrAddOffsetOpt) {
    LLVM_DEBUG(dbgs() << "  PTR_ADD offset not from G_TRUNC\n");
    return false;
  }

  const Register PtrAddOffsetReg = *PtrAddOffsetOpt;

  // Check if base comes from G_AIE_POSTINC_LOAD or G_AIE_POSTINC_STORE
  MachineInstr *PostIncMI = MRI.getVRegDef(PtrAddBase);
  if (!PostIncMI)
    return false;

  const unsigned PostIncOpc = PostIncMI->getOpcode();
  const bool IsPostIncLoad = (PostIncOpc == TII.getGenericPostIncLoadOpcode());
  const bool IsPostIncStore =
      (PostIncOpc == TII.getGenericPostIncStoreOpcode());

  if (!IsPostIncLoad && !IsPostIncStore)
    return false;

  // Verify the POSTINC's pointer output has only one use (the PTR_ADD)
  if (!MRI.hasOneNonDBGUse(PtrAddBase))
    return false;

  // POSTINC_LOAD has: def0 (data), def1 (pointer), use0 (base ptr), use1
  // (offset) POSTINC_STORE has: def0 (pointer), use0 (data), use1 (base ptr),
  // use2 (offset)
  const unsigned PtrOutIdx = IsPostIncLoad ? 1 : 0;
  const unsigned OffsetIdx = 3;

  const Register PostIncPtr = PostIncMI->getOperand(PtrOutIdx).getReg();
  const Register PostIncOffset = PostIncMI->getOperand(OffsetIdx).getReg();

  // Verify pointer output matches PTR_ADD base
  if (PostIncPtr != PtrAddBase)
    return false;

  // Get Offset source for POSTINC offset (only from TRUNC, not ZEXTLOAD)
  auto PostIncOffsetOpt =
      getSourceFromS20(PostIncOffset, MRI, /*OnlyTruncs=*/true);
  if (!PostIncOffsetOpt)
    return false;
  const Register PostIncOffsetReg = *PostIncOffsetOpt;

  // Get the definitions of both s32 source registers
  MachineInstr *PostIncDefMI = MRI.getVRegDef(PostIncOffsetReg);
  MachineInstr *PtrAddDefMI = MRI.getVRegDef(PtrAddOffsetReg);

  if (!PostIncDefMI || !PtrAddDefMI)
    return false;

  // Check dominance: we need one to dominate the other
  Register DominatingReg, DominatedReg;
  MachineInstr *InsertionPoint = nullptr;

  if (Helper.dominates(*PostIncDefMI, *PtrAddDefMI)) {
    // PostInc Offset dominates PtrAdd Offset
    DominatingReg = PostIncOffsetReg;
    DominatedReg = PtrAddOffsetReg;
    InsertionPoint = PtrAddDefMI;
  } else if (Helper.dominates(*PtrAddDefMI, *PostIncDefMI)) {
    // PtrAdd Offset dominates PostInc Offset
    DominatingReg = PtrAddOffsetReg;
    DominatedReg = PostIncOffsetReg;
    InsertionPoint = PostIncDefMI;
  } else {
    // No dominance relation - cannot proceed safely
    return false;
  }

  // Verify insertion point dominates the POSTINC
  if (!Helper.dominates(*InsertionPoint, *PostIncMI))
    return false;

  // Build the lambda that will perform the transformation
  MatchInfo = [=, &MRI, &MI, &Observer](MachineIRBuilder &B) {
    // Set insertion point right after the dominated definition,
    // skipping any PHI nodes that immediately follow it.
    setInsertPtAfterInstr(B, InsertionPoint);

    // Build G_ADD of the two s32 values
    const Register CombinedS32 = MRI.createGenericVirtualRegister(S32);
    B.buildAdd(CombinedS32, DominatingReg, DominatedReg);

    // Build G_TRUNC to s20
    const Register CombinedS20 = MRI.createGenericVirtualRegister(S20);
    B.buildTrunc(CombinedS20, CombinedS32);

    MI.eraseFromParent();

    // Update the POSTINC (LOAD or STORE) to use the combined offset and output
    // to PtrAddDst
    Observer.changingInstr(*PostIncMI);
    PostIncMI->getOperand(OffsetIdx).setReg(CombinedS20); // Update offset
    PostIncMI->getOperand(PtrOutIdx).setReg(PtrAddDst); // Update pointer output
    Observer.changedInstr(*PostIncMI);
  };

  return true;
}

//===----------------------------------------------------------------------===//
// combine_redundant_widen_narrow_conversion
//===----------------------------------------------------------------------===//

/// Match redundant widen <-> narrow conversion pairs and eliminate them.
/// Pattern matched:
///   %wide:_(<16 x s32>) = G_INTRINSIC
///   intrinsic(@llvm.aie2ps.v16bf16.to.v16accfloat), %narrow(<16 x s16>)
///   %result:_(<16 x s16>) = G_INTRINSIC_W_SIDE_EFFECTS
///   intrinsic(@llvm.aie2ps.v16accfloat.to.v16bf16), %wide(<16 x s32>)
///
/// Transforms to:
///   %result:_(<16 x s16>) = COPY %narrow(<16 x s16>)
bool llvm::matchRedundantWidenNarrowConversion(MachineInstr &MI,
                                               MachineRegisterInfo &MRI,
                                               const AIEBaseInstrInfo &TII,
                                               BuildFnTy &MatchInfo) {
  // Check if optimization is enabled
  if (!CombineRedundantWidenNarrowConversions)
    return false;

  // Must be G_INTRINSIC_W_SIDE_EFFECTS (the "to narrow" conversion)
  if (MI.getOpcode() != TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS)
    return false;

  const auto *IntrMI = cast<GIntrinsic>(&MI);
  const Intrinsic::ID ToNarrowID = IntrMI->getIntrinsicID();

  // Get target-specific conversion pairs
  ArrayRef<AIEBaseInstrInfo::WidenNarrowConversionPair> ConversionPairs =
      TII.getWidenNarrowConversionPairs();

  // Find matching conversion pair for this intrinsic
  unsigned ToWideID = 0;
  for (const auto &Pair : ConversionPairs) {
    if (Pair.ToNarrowIntrinsicID == ToNarrowID) {
      ToWideID = Pair.ToWideIntrinsicID;
      break;
    }
  }

  // Not a recognized "to narrow" conversion for this target
  if (ToWideID == 0)
    return false;

  // Get the wide value source register (operand 2 for
  // G_INTRINSIC_W_SIDE_EFFECTS)
  const Register WideReg = MI.getOperand(2).getReg();

  // Get the definition of the wide value
  const MachineInstr *WideDefMI = MRI.getVRegDef(WideReg);
  if (!WideDefMI || WideDefMI->getOpcode() != TargetOpcode::G_INTRINSIC)
    return false;

  // Check if it's the matching "to wide" conversion
  const auto *WideIntrMI = cast<GIntrinsic>(WideDefMI);
  if (WideIntrMI->getIntrinsicID() != ToWideID)
    return false;

  // Get the original narrow source (operand 2 for G_INTRINSIC)
  const Register OrigNarrowReg = WideDefMI->getOperand(2).getReg();

  // Get result register
  const Register ResultReg = MI.getOperand(0).getReg();

  // Build the transformation: replace with COPY
  MatchInfo = [ResultReg, OrigNarrowReg](MachineIRBuilder &B) {
    B.buildCopy(ResultReg, OrigNarrowReg);
  };

  return true;
}

// Return the base offset, base offset is decided based on the
// maximum number of distances which are simm3 three bit signed
// for part-word and/or imm6x4 6-bits scaled by 4 for full word.
// Count is the number of distances which fall in the range of
// simm3 and imm6x4. The heuristic considers the base offset optimal
// if Count is maximum for the base offset.
static uint64_t findOptimalOffset(
    std::set<std::pair<uint64_t, uint64_t>> &OffsetElemSizePairSet) {
  uint64_t OptOffset = 0;
  uint64_t MaxCount = 0;
  for (auto const &i : OffsetElemSizePairSet) {
    uint64_t Count = 0;
    for (auto const &j : OffsetElemSizePairSet) {
      int64_t Imm = j.first - i.first;
      // Offset is simm3 for part word?
      if (j.second <= 2 && isInt<3>(Imm)) {
        Count++;
        continue;
      }
      // Offset is imm6x4 ?
      if (j.second >= 4 && (isInt<6 + 2>(Imm) && (Imm % 4) == 0))
        Count++;
    }
    // Choose the first base offset having MaxCount.
    // TODO: Can we choose a base offset having maximal count
    // closer to part word?
    if (Count > MaxCount) {
      MaxCount = Count;
      OptOffset = i.first;
    }
  }
  return OptOffset;
}

/// \returns true if it is possible to fold a below sequence of MIRs
/// into a G_GLOBAL_VALUE.
/// From : %1 = G_GLOBAL_VALUE @a
///        %2 = G_CONSTANT off
///        %3 = G_PTR_ADD %1, %2
/// TO :   G_GLOBAL_VALUE @a + off

bool llvm::matchGlobalValOffset(MachineInstr &MI, MachineRegisterInfo &MRI,
                                uint64_t &MatchInfo) {

  assert(MI.getOpcode() == TargetOpcode::G_GLOBAL_VALUE);
  MachineFunction &MF = *MI.getMF();
  auto &GlobalOp = MI.getOperand(1);
  auto *GV = GlobalOp.getGlobal();
  if (GV->isThreadLocal())
    return false;
  // transform %g = G_GLOBAL_VALUE @a, for any other case return false
  if (GlobalOp.getOffset() != 0)
    return false;
  unsigned OpFlags =
      MF.getSubtarget<AIEBaseSubtarget>().classifyGlobalReference(
          GV, MF.getTarget());
  if (OpFlags != AIEII::MO_None)
    return false;

  // Look for a G_GLOBAL_VALUE only used by G_PTR_ADDs against offsets:
  //
  //  %g = G_GLOBAL_VALUE @a
  //  %ptr1 = G_PTR_ADD %g, off1
  //  %ptr2 = G_PTR_ADD %g, off2
  //  ...
  //  %ptrN = G_PTR_ADD %g, offN
  //
  // Identify the Offset. => optimal_offset(off1, off2,..,offN)
  // apply and transform to  [using applyGlobalValOffset]
  // %off = G_GLOBAL_VALUE @a + Offset
  // %g = G_PTR_ADD %off, -Offset
  // %ptr1 = G_PTR_ADD %g, off1
  //
  Register Dst = MI.getOperand(0).getReg();
  uint64_t Offset = 0ull;
  // set of access element size and offset pair
  std::set<std::pair<uint64_t, uint64_t>> OffsetElemSizePairSet;
  for (auto &UseInstr : MRI.use_nodbg_instructions(Dst)) {
    GLoadStore *LdStMI = dyn_cast<GLoadStore>(&UseInstr);
    if ((UseInstr.getOpcode() != TargetOpcode::G_PTR_ADD) && !LdStMI)
      return false;
    if (LdStMI) {
      MachineMemOperand *MMO = *UseInstr.memoperands_begin();
      OffsetElemSizePairSet.insert(
          std::make_pair(Offset, MMO->getSize().getValue()));
    } else {
      auto Cst = getIConstantVRegValWithLookThrough(
          UseInstr.getOperand(2).getReg(), MRI);
      if (!Cst)
        return false;
      APInt Val = Cst->Value;
      assert(Val.isNonNegative() && "Expected a NonNegative Constant Offset");
      const auto UseInstIter =
          MRI.use_nodbg_instructions(UseInstr.getOperand(0).getReg());
      for (auto &Use : UseInstIter) {
        if (dyn_cast<GLoadStore>(&Use)) {
          MachineMemOperand *MMO = *Use.memoperands_begin();
          OffsetElemSizePairSet.insert(
              std::make_pair(Val.getZExtValue(), MMO->getSize().getValue()));
        }
      }
    }
  }
  Offset = findOptimalOffset(OffsetElemSizePairSet);
  if (Offset <= 0)
    return false;

  // Offset should not be greater than 20-bit
  if (Offset >= (1ull << 19))
    return false;

  Type *T = GV->getValueType();
  // Return false if it does not makes sense to take the size of this type
  // or Offset is greater than the size allocated for the global object.
  if (!T->isSized() ||
      Offset > GV->getParent()->getDataLayout().getTypeAllocSize(T))
    return false;
  MatchInfo = Offset;
  return true;
}

void llvm::applyGlobalValOffset(MachineInstr &MI, MachineRegisterInfo &MRI,
                                MachineIRBuilder &B,
                                GISelChangeObserver &Observer,
                                uint64_t &MatchInfo) {
  uint64_t Offset = MatchInfo;
  B.setInsertPt(*MI.getParent(), ++MI.getIterator());
  Observer.changingInstr(MI);
  auto &GlobalOp = MI.getOperand(1);
  auto *GV = GlobalOp.getGlobal();
  unsigned Flags = GlobalOp.getTargetFlags();
  GlobalOp.ChangeToGA(GV, Offset, Flags | AIEII::MO_GLOBAL);
  Register Dst = MI.getOperand(0).getReg();
  Register NewGVDst = MRI.cloneVirtualRegister(Dst);
  MI.getOperand(0).setReg(NewGVDst);
  Observer.changedInstr(MI);
  //
  //  %off = G_GLOBAL_VALUE @a + Offset
  //  %g = G_PTR_ADD %off, -Offset
  //  %ptr1 = G_PTR_ADD %g, off1
  //  ...
  B.buildPtrAdd(
      Dst, NewGVDst,
      B.buildConstant(LLT::scalar(20), -static_cast<int64_t>(Offset)));
}

bool llvm::matchExtractVecEltAndExt(
    MachineInstr &MI, MachineRegisterInfo &MRI,
    std::pair<MachineInstr *, bool> &MatchInfo) {

  assert(MI.getOpcode() == TargetOpcode::G_EXTRACT_VECTOR_ELT &&
         "Expected a extract_vector_elt");
  Register DstReg = MI.getOperand(0).getReg();
  LLT SrcVecTy = MRI.getType(MI.getOperand(1).getReg());
  // Extracts from vectors <= 64-bits are lowered to bit-arithmetic in
  // legalization
  if (SrcVecTy.getSizeInBits() <= 64)
    return false;
  LLT SrcEltTy = SrcVecTy.getElementType();
  if (SrcEltTy != S8 && SrcEltTy != S16)
    return false;
  if (!MRI.hasOneNonDBGUse(DstReg))
    return false;
  MachineInstr *ExtMI = &*MRI.use_instr_nodbg_begin(DstReg);
  switch (ExtMI->getOpcode()) {
  case TargetOpcode::G_ANYEXT:
  case TargetOpcode::G_SEXT:
    MatchInfo = std::make_pair(ExtMI, 1);
    return true;
  case TargetOpcode::G_ZEXT:
    MatchInfo = std::make_pair(ExtMI, 0);
    return true;
  default:
    return false;
  }
  return false;
}

void llvm::applyExtractVecEltAndExt(MachineInstr &MI, MachineRegisterInfo &MRI,
                                    MachineIRBuilder &B,
                                    std::pair<MachineInstr *, bool> &MatchInfo,
                                    GISelChangeObserver &Observer) {
  B.setInstrAndDebugLoc(MI);
  auto [MatchMI, IsSignedExt] = MatchInfo;
  const Register ExtractDstReg = MI.getOperand(0).getReg();
  const LLT ExtractDstTy = MRI.getType(ExtractDstReg);
  const Register ExtendDstReg = MatchMI->getOperand(0).getReg();
  const LLT ExtendDstTy = MRI.getType(ExtendDstReg);
  const Register SrcReg0 = MI.getOperand(1).getReg();
  const Register SrcReg1 = MI.getOperand(2).getReg();
  const LLT S32 = LLT::scalar(32);
  const AIEBaseInstrInfo &AIETII = (const AIEBaseInstrInfo &)B.getTII();
  const unsigned Opcode =
      AIETII.getGenericExtractVectorEltOpcode(/*sign ext*/ IsSignedExt);
  const Register ExtractElt32BitDst = MRI.createGenericVirtualRegister(S32);
  B.buildInstr(Opcode, {ExtractElt32BitDst}, {SrcReg0, SrcReg1});

  const unsigned AssertOpcode =
      IsSignedExt ? TargetOpcode::G_ASSERT_SEXT : TargetOpcode::G_ASSERT_ZEXT;
  if (ExtendDstTy == LLT::scalar(32)) {
    B.buildAssertInstr(AssertOpcode, ExtendDstReg, ExtractElt32BitDst,
                       ExtractDstTy.getSizeInBits());
  } else {
    const Register Assert32BitDst = MRI.createGenericVirtualRegister(S32);
    B.buildAssertInstr(AssertOpcode, Assert32BitDst, ExtractElt32BitDst,
                       ExtractDstTy.getSizeInBits());
    B.buildExtOrTrunc(MatchMI->getOpcode(), ExtendDstReg, Assert32BitDst);
  }

  Observer.erasingInstr(MI);
  MI.eraseFromParent();
  Observer.erasingInstr(*MatchMI);
  MatchMI->eraseFromParent();
}

static std::optional<Register>
getSplatVectorSrcReg(const MachineInstr &MI, const MachineRegisterInfo &MRI,
                     std::pair<unsigned, unsigned> Range) {
  auto IsUndef = [&](const MachineOperand &Op) {
    const MachineInstr *Undef = MRI.getVRegDef(Op.getReg());
    return Undef && Undef->getOpcode() == TargetOpcode::G_IMPLICIT_DEF;
  };
  const unsigned Start = Range.first;
  const unsigned End = Range.second;
  // First non-undef operand.
  Register SrcReg = 0;
  bool FoundSrc = false;
  bool AllUndef = true;

  // Find the first non-undef operand as the reference.
  for (unsigned I = Start; I < End; I++) {
    const MachineOperand &Op = MI.getOperand(I);
    if (!IsUndef(Op)) {
      if (!FoundSrc) {
        SrcReg = Op.getReg();
        FoundSrc = true;
      } else if (Op.getReg() != SrcReg) {
        return std::nullopt;
      }
      AllUndef = false;
    }
  }

  if (AllUndef)
    SrcReg = MI.getOperand(1).getReg();

  return SrcReg;
}

// Match something like:
// %0:_(<32 x s16>) = G_BUILD_VECTOR %1:_(s16), ... x32
//
// To turn it into
// %0:_(<32 x s16>) = G_AIE_BROADCAST_VECTOR %1:_(s16)
bool llvm::matchSplatVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                            std::pair<Register, Register> &MatchInfo) {

  assert(MI.getOpcode() == TargetOpcode::G_BUILD_VECTOR &&
         "Expected a G_BUILD_VECTOR");

  const Register DstVecReg = MI.getOperand(0).getReg();
  const LLT DstVecTy = MRI.getType(DstVecReg);
  const unsigned DstVecSize = DstVecTy.getSizeInBits();

  switch (DstVecSize) {
  case 128:
  case 256:
  case 512:
  case 1024:
  case 2048:
    break;
  default:
    // unimplemented.
    return false;
  }

  const unsigned NumOps = MI.getNumOperands();
  auto SrcReg = getSplatVectorSrcReg(MI, MRI, std::make_pair(1, NumOps));
  if (!SrcReg)
    return false;

  MatchInfo = {DstVecReg, *SrcReg};
  return true;
}

static void buildUnmergeVector(MachineIRBuilder &B, MachineRegisterInfo &MRI,
                               Register DstReg, Register SrcReg,
                               unsigned NumSubVectors, unsigned SubIdx) {
  const LLT DstTy = MRI.getType(DstReg);
  SmallVector<Register, 4> SubVecs;
  for (unsigned I = 0; I < NumSubVectors; I++) {
    if (I == SubIdx)
      SubVecs.push_back(DstReg);
    else
      SubVecs.push_back(MRI.createGenericVirtualRegister(DstTy));
  }
  B.buildUnmerge(SubVecs, SrcReg);
}

static void buildBroadcastVector(MachineIRBuilder &B, MachineRegisterInfo &MRI,
                                 Register SrcReg, Register DstVecReg) {
  const AIEBaseInstrInfo &AIETII = (const AIEBaseInstrInfo &)B.getTII();
  const LLT SrcTy = MRI.getType(SrcReg);
  const LLT DstVecTy = MRI.getType(DstVecReg);
  const unsigned DstVecSize = DstVecTy.getSizeInBits();
  auto IsConstantZeroReg = [&](const Register Reg) {
    auto Cst = getAnyConstantVRegValWithLookThrough(Reg, MRI);
    return Cst && Cst->Value.isZero();
  };
  // Check if the source is constant zero and build a 2048-bit
  // vector destination.
  auto isConstantZero = IsConstantZeroReg(SrcReg) && DstVecSize == 2048;
  if (SrcTy == LLT::scalar(8) || SrcTy == LLT::scalar(16)) {
    const LLT S32 = LLT::scalar(32);
    Register Src32BitReg = MRI.createGenericVirtualRegister(S32);
    B.buildAnyExt(Src32BitReg, SrcReg);
    SrcReg = Src32BitReg;
  }
  if (SrcTy == LLT::scalar(64)) {
    const LLT V2S32 = LLT::fixed_vector(2, 32);
    Register Src64BitReg = MRI.createGenericVirtualRegister(V2S32);
    B.buildBitcast(Src64BitReg, SrcReg);
    SrcReg = Src64BitReg;
  }
  const bool SupportsNative1024Broadcast = AIETII.supportsNative1024Broadcast();
  const bool CanBroadcastDirectly =
      DstVecSize == 512 || isConstantZero ||
      (DstVecSize == 1024 && SupportsNative1024Broadcast);
  if (CanBroadcastDirectly) {
    // Build the G_AIE_BROADCAST_VECTOR instruction directly.
    B.buildInstr(AIETII.getGenericBroadcastVectorOpcode(), {DstVecReg},
                 {SrcReg});
  } else if (DstVecSize == 2048 && SupportsNative1024Broadcast) {
    const unsigned DstElmtSize = DstVecTy.getElementType().getSizeInBits();
    const unsigned DstVec1024BitLen = 1024 / DstElmtSize;
    Register DstVec1024BitReg = MRI.createGenericVirtualRegister(
        LLT::fixed_vector(DstVec1024BitLen, DstElmtSize));
    B.buildInstr(AIETII.getGenericBroadcastVectorOpcode(), {DstVec1024BitReg},
                 {SrcReg});
    B.buildConcatVectors({DstVecReg}, {DstVec1024BitReg, DstVec1024BitReg});
  } else {
    const unsigned DstElmtSize = DstVecTy.getElementType().getSizeInBits();
    const unsigned DstVec512BitLen = 512 / DstElmtSize;

    // Create a 512-bit generic virtual register for the destination vector
    // as 256-bit broadcast support is not available and 1024-bit may require
    // concatenation on targets without native support.
    Register DstVec512BitReg = MRI.createGenericVirtualRegister(
        LLT::fixed_vector(DstVec512BitLen, DstElmtSize));

    // Build the G_AIE_BROADCAST_VECTOR instruction for the 512-bit vector.
    B.buildInstr(AIETII.getGenericBroadcastVectorOpcode(), {DstVec512BitReg},
                 {SrcReg});
    if (DstVecSize == 128 || DstVecSize == 256) {
      const unsigned NumSubVectors = 512 / DstVecSize;
      // Unmerge the 512-bit vector into the 128/256-bit destination vector.
      buildUnmergeVector(B, MRI, DstVecReg, DstVec512BitReg, NumSubVectors, 0);
    } else if (DstVecSize == 1024) {
      // Concatenate two 512-bit vectors to form a 1024-bit destination vector
      // when native 1024-bit broadcast is unavailable.
      B.buildConcatVectors({DstVecReg}, {DstVec512BitReg, DstVec512BitReg});
    } else if (DstVecSize == 2048) {
      // Concatenate 4 512-bit vectors to form a 2048-bit destination vector.
      B.buildConcatVectors({DstVecReg}, {DstVec512BitReg, DstVec512BitReg,
                                         DstVec512BitReg, DstVec512BitReg});
    }
  }
}

bool llvm::applySplatVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                            MachineIRBuilder &B,
                            std::pair<Register, Register> &MatchInfo,
                            GISelChangeObserver &Observer) {
  B.setInstrAndDebugLoc(MI);
  auto [DstVecReg, SrcReg] = MatchInfo;
  buildBroadcastVector(B, MRI, SrcReg, DstVecReg);
  Observer.erasingInstr(MI);
  MI.eraseFromParent();
  return true;
}

// Match something like:
// %0:_(<32 x s16>) = G_BUILD_VECTOR %2:(s16), %2:(s16), %1:(s16) ... x32
//
// To turn it into
// %3:_(<32 x s16>) = G_AIE_BROADCAST_VECTOR %2:_(s16)
// %0:(<32 x s16>) = G_AIE_INSERT_VECTOR_ELT %3:(<32 x s16>), %1:_(s16), 2
bool llvm::matchSingleDiffLaneBuildVector(
    MachineInstr &MI, MachineRegisterInfo &MRI,
    AIESingleDiffLaneBuildVectorMatchData &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_BUILD_VECTOR &&
         "Expected a G_BUILD_VECTOR");

  const Register DstVecReg = MI.getOperand(0).getReg();
  const LLT DstVecTy = MRI.getType(DstVecReg);
  const unsigned DstVecSize = DstVecTy.getSizeInBits();

  switch (DstVecSize) {
  case 128:
  case 256:
  case 512:
  case 1024:
  case 2048:
    break;
  default:
    // unimplemented
    return false;
  }
  // DenseMap to hold unique registers and their (count, last index)
  DenseMap<Register, std::pair<unsigned, unsigned>> UniqueRegs;
  const unsigned NumOps = MI.getNumOperands();
  for (unsigned i = 1; i < NumOps; i++) {
    const Register OpReg = MI.getOperand(i).getReg();
    auto &RegInfo = UniqueRegs[OpReg];
    RegInfo.first += 1;
    RegInfo.second = i - 1;

    if (UniqueRegs.size() > 2)
      return false;
  }
  // Ensure exactly 2 unique registers to match the single differing lane build
  // vector pattern. More than 2 registers won't match; 1 unique register would
  // be a splat vector combine
  if (UniqueRegs.size() != 2)
    return false;

  Register SplatReg, DifferingReg;
  unsigned DifferingIndex;

  // Identify splat (multiple uses) and differing (single use) registers
  for (const auto &[Reg, RegInfo] : UniqueRegs) {
    if (RegInfo.first == 1) {
      DifferingReg = Reg;
      DifferingIndex = RegInfo.second;
    } else {
      SplatReg = Reg;
    }
  }
  // Validate that one register was used exactly once
  if (!DifferingReg.isValid() || !SplatReg.isValid())
    return false;

  // Ignore G_IMPLICIT_DEF to avoid conflicts with \fn matchBroadcastElement
  const MachineInstr *SplatRegDef = getDefIgnoringCopies(SplatReg, MRI);
  if (!SplatRegDef || SplatRegDef->getOpcode() == TargetOpcode::G_IMPLICIT_DEF)
    return false;

  MatchInfo = {DstVecReg, SplatReg, DifferingReg, DifferingIndex};
  return true;
}

bool llvm::applySingleDiffLaneBuildVector(
    MachineInstr &MI, MachineRegisterInfo &MRI, MachineIRBuilder &B,
    AIESingleDiffLaneBuildVectorMatchData &MatchInfo,
    GISelChangeObserver &Observer) {
  B.setInstrAndDebugLoc(MI);
  const Register DstVecReg = MatchInfo.DstVecReg;
  const LLT DstVecRegTy = MRI.getType(DstVecReg);
  const Register BcstDstReg = MRI.createGenericVirtualRegister(DstVecRegTy);
  const LLT S32 = LLT::scalar(32);

  buildBroadcastVector(B, MRI, MatchInfo.SplatReg, BcstDstReg);
  const Register IdxReg =
      B.buildConstant(S32, MatchInfo.DifferingIndex).getReg(0);
  B.buildInsertVectorElement(DstVecReg, BcstDstReg, MatchInfo.DifferingReg,
                             IdxReg);
  Observer.erasingInstr(MI);
  MI.eraseFromParent();
  return true;
}

// Match something like:
// %0:_(<32 x s16>) = G_BUILD_VECTOR %1:_(s16), ... x16, %2:_(s16), ... x16
//
// To turn it into
// %3:_(<16 x s16>) = G_BUILD_VECTOR %1:_(s16), ... x16
// %4:_(<16 x s16>) = G_BUILD_VECTOR %2:_(s16), ... x16
// %0:_(<32 x s16>) = G_CONCAT_VECTORS %3:_(<16 x s16>), %4:_(<16 x s16>)
// These sub-G_BUILD_VECTOR instructions may later be combined into broadcast
// instructions by combine_splat_vector.
// TODO: Remove the original splat vector match and implement the same here.
bool llvm::matchSymmetricBuildVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                                     GISelChangeObserver &Observer,
                                     BuildFnTy &MatchInfo) {

  assert(MI.getOpcode() == TargetOpcode::G_BUILD_VECTOR &&
         "Expected a G_BUILD_VECTOR");
  const Register DstVecReg = MI.getOperand(0).getReg();
  const LLT DstVecTy = MRI.getType(DstVecReg);
  const unsigned DstVecSize = DstVecTy.getSizeInBits();

  switch (DstVecSize) {
  case 256:
  case 512:
  case 1024:
  case 2048:
    break;
  default:
    // unimplemented
    return false;
  }

  // TODO: Split the G_BUILD_VECTOR either into 3/4 and 1/4 parts,
  // or 1/4 and 3/4 parts, and then check if any part qualifies as a splat.
  const unsigned NumOps = MI.getNumOperands();
  const unsigned HalfNumElts = NumOps / 2 + 1;
  auto FirstHalfSrcReg =
      getSplatVectorSrcReg(MI, MRI, std::make_pair(1, HalfNumElts));
  auto SecondHalfSrcReg =
      getSplatVectorSrcReg(MI, MRI, std::make_pair(HalfNumElts, NumOps));

  MatchInfo = [&MI, &Observer, DstVecTy](MachineIRBuilder &B) {
    B.setInstrAndDebugLoc(MI);
    LegalizerHelper Helper(B.getMF(), Observer, B);
    // Splits the G_BUILD_VECTOR into two half-sized G_BUILD_VECTOR operations
    // and then emits a G_CONCAT_VECTORS to combine them into final vector.
    Helper.fewerElementsVector(
        MI, 0,
        DstVecTy.changeElementCount(
            DstVecTy.getElementCount().divideCoefficientBy(2)));
  };

  return (FirstHalfSrcReg.has_value() || SecondHalfSrcReg.has_value());
}

// Match something like:
// %0(<4 x s32>), dead %1(<4 x s32>), dead %2(<4 x s32>), dead %3(<4 x s32>)
//   = G_UNMERGE_VALUES %10(<16 x s32>)
//
// To turn it into
// %0(<4 x s32>) = G_AIE_UNPAD_VECTOR %10(<16 x s32>)
bool llvm::matchUnpadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                            const AIEBaseInstrInfo &TII) {
  assert(MI.getOpcode() == TargetOpcode::G_UNMERGE_VALUES &&
         "Expected an unmerge");

  // Check that the first lane is a 128-bit vector, and the others are dead.
  LLT UnmergedTy = MRI.getType(MI.getOperand(0).getReg());
  LLT InputTy = MRI.getType(MI.getOperand(MI.getNumDefs()).getReg());
  if (!TII.isLegalTypeToUnpad(InputTy) || !TII.isLegalTypeToPad(UnmergedTy)) {
    return false;
  }
  for (unsigned Idx = 1, EndIdx = MI.getNumDefs(); Idx != EndIdx; ++Idx) {
    if (!MRI.use_nodbg_empty(MI.getOperand(Idx).getReg()))
      return false;
  }
  return true;
}

void llvm::applyUnpadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                            MachineIRBuilder &B,
                            GISelChangeObserver &Observer) {
  B.setInstrAndDebugLoc(MI);
  const AIEBaseInstrInfo &AIETII = (const AIEBaseInstrInfo &)B.getTII();
  Register DstReg = MI.getOperand(0).getReg();
  Register SrcReg = MI.getOperand(MI.getNumDefs()).getReg();
  B.buildInstr(AIETII.getGenericUnpadVectorOpcode(), {DstReg}, {SrcReg});
  Observer.erasingInstr(MI);
  MI.eraseFromParent();
}

/// Match a chain of G_AIE_VSHIFT_RIGHT operations generated by legalization
/// of 128-bit vector UNMERGE/CONCAT operations. The pattern arises from:
/// 1. G_UNMERGE_VALUES legalization creating shifts to extract 128-bit halves
/// 2. G_CONCAT_VECTORS legalization creating shifts to combine 128-bit vectors
///
/// Pattern matched (from legalization):
///   %undefined1:_(<8 x s32>) = G_IMPLICIT_DEF
///   %concat:_(<16 x s32>) = G_CONCAT_VECTORS %data(<8 x s32>), %undefined1
///   %undefined2:_(<16 x s32>) = G_IMPLICIT_DEF
///   %shift1:_(<16 x s32>) = G_AIE_VSHIFT_RIGHT %concat, %undefined2, 16
///   %shift2:_(<16 x s32>) = G_AIE_VSHIFT_RIGHT %undefined2, %shift1, 48
///
/// Transforms to:
///   %shift2:_(<16 x s32>) = COPY %concat
bool llvm::matchVShiftChainToCopy(MachineInstr &MI, MachineRegisterInfo &MRI,
                                  const AIEBaseInstrInfo &TII,
                                  BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TII.getGenericVShiftOpcode() &&
         "Expected G_AIE_VSHIFT_RIGHT");

  // Get second operand of second shift (should be first VSHIFT result)
  const Register Src2 = MI.getOperand(2).getReg();
  const MachineInstr *FirstShift = getDefIgnoringCopies(Src2, MRI);
  if (!FirstShift || FirstShift->getOpcode() != TII.getGenericVShiftOpcode())
    return false;

  // Get shift amounts - must be constants
  const auto Shift1Amt =
      getIConstantVRegVal(FirstShift->getOperand(3).getReg(), MRI);
  const auto Shift2Amt = getIConstantVRegVal(MI.getOperand(3).getReg(), MRI);

  if (!Shift1Amt || !Shift2Amt)
    return false;

  // Get the data source from first VSHIFT (should be CONCAT or PAD result)
  const Register DataSrcReg = FirstShift->getOperand(1).getReg();

  // The data source can be either G_CONCAT_VECTORS or G_AIE_PAD_VECTOR_UNDEF
  // (CONCAT gets converted to PAD by another combiner)
  MachineInstr *DataSrcMI = getDefIgnoringCopies(DataSrcReg, MRI);
  if (!DataSrcMI)
    return false;

  Register ActualDataSrc;
  const LLT DataSrcTy = MRI.getType(DataSrcReg);

  if (DataSrcMI->getOpcode() == TargetOpcode::G_CONCAT_VECTORS) {
    // Pattern: G_CONCAT_VECTORS %data(<8 x s32>), %undefined(<8 x s32>)

    // Verify CONCAT has exactly 2 operands (1 def + 2 sources)
    if (DataSrcMI->getNumOperands() != 3)
      return false;

    // Get the actual data source and padding operand from CONCAT
    ActualDataSrc = DataSrcMI->getOperand(1).getReg();
    const Register ConcatPadding = DataSrcMI->getOperand(2).getReg();

    // Verify CONCAT's second operand is G_IMPLICIT_DEF
    MachineInstr *ConcatPaddingDef = MRI.getVRegDef(ConcatPadding);
    if (!ConcatPaddingDef ||
        ConcatPaddingDef->getOpcode() != TargetOpcode::G_IMPLICIT_DEF)
      return false;

  } else if (DataSrcMI->getOpcode() == TII.getGenericPadVectorOpcode()) {
    // Pattern: G_AIE_PAD_VECTOR_UNDEF %data(<8 x s32>)

    // Get the actual data source from PAD
    ActualDataSrc = DataSrcMI->getOperand(1).getReg();
    const LLT ActualDataTy = MRI.getType(ActualDataSrc);

    // Verify types match expected pattern: <8 x s32> → <16 x s32>
    if (ActualDataTy.getSizeInBits() != DataSrcTy.getSizeInBits() / 2)
      return false;

  } else {
    // Data source is neither CONCAT nor PAD
    return false;
  }

  // Verify ActualDataSrc type
  const LLT ActualDataTy = MRI.getType(ActualDataSrc);

  // Verify unused shift sources (shift-in operands) are G_IMPLICIT_DEF
  // First shift: operand[2] is the shift-in source
  const Register FirstShiftSrc2 = FirstShift->getOperand(2).getReg();
  MachineInstr *FirstShiftSrc2Def = MRI.getVRegDef(FirstShiftSrc2);
  if (!FirstShiftSrc2Def ||
      FirstShiftSrc2Def->getOpcode() != TargetOpcode::G_IMPLICIT_DEF)
    return false;

  // Second shift: operand[1] is the shift-in source
  const Register SecondShiftSrc1 = MI.getOperand(1).getReg();
  MachineInstr *SecondShiftSrc1Def = MRI.getVRegDef(SecondShiftSrc1);
  if (!SecondShiftSrc1Def ||
      SecondShiftSrc1Def->getOpcode() != TargetOpcode::G_IMPLICIT_DEF)
    return false;

  // Calculate expected shift amounts based on actual data source size
  const unsigned ActualSrcBytes = ActualDataTy.getSizeInBits() / 8;

  // For 256-bit source (<8 x s32> = 32 bytes):
  // First shift: 32/2 = 16 bytes (shifts data to position for extraction)
  // Second shift: 32 + 16 = 48 bytes (positions data with zero padding)
  const unsigned ExpectedShift1 = ActualSrcBytes / 2;
  const unsigned ExpectedShift2 = ActualSrcBytes + ActualSrcBytes / 2;

  // Verify the pattern matches expected shifts
  if (*Shift1Amt != ExpectedShift1 || *Shift2Amt != ExpectedShift2)
    return false;

  const Register DstReg = MI.getOperand(0).getReg();

  // Build simplified replacement: just copy the CONCAT/PAD result
  // The CONCAT/PAD already has the form [data | undefined], which is equivalent
  // to the result of the shift chain [data | zeros/undefined]
  MatchInfo = [=](MachineIRBuilder &B) { B.buildCopy(DstReg, DataSrcReg); };

  return true;
}

/// Match G_AIE_UNPAD_VECTOR fed by G_AIE_PAD_VECTOR_UNDEF and combine to COPY.
/// Transforms:
///   %padded:_(<16 x s32>) = G_AIE_PAD_VECTOR_UNDEF %src(<8 x s32>)
///   %dst:_(<8 x s32>) = G_AIE_UNPAD_VECTOR %padded(<16 x s32>)
/// Into:
///   %dst:_(<8 x s32>) = COPY %src(<8 x s32>)
bool llvm::matchPadUnpadToCopy(MachineInstr &MI, MachineRegisterInfo &MRI,
                               const AIEBaseInstrInfo &TII,
                               BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TII.getGenericUnpadVectorOpcode() &&
         "Expected G_AIE_UNPAD_VECTOR");

  // Get the source of the unpad operation
  const Register UnpadDst = MI.getOperand(0).getReg();
  const Register UnpadSrc = MI.getOperand(1).getReg();

  // Check if source is G_AIE_PAD_VECTOR_UNDEF
  const MachineInstr *PadMI = MRI.getVRegDef(UnpadSrc);
  if (!PadMI || PadMI->getOpcode() != TII.getGenericPadVectorOpcode())
    return false;

  const Register PadSrc = PadMI->getOperand(1).getReg();

  // Type validation: ensure input of PAD matches output of UNPAD
  const LLT PadSrcTy = MRI.getType(PadSrc);
  const LLT UnpadDstTy = MRI.getType(UnpadDst);

  if (PadSrcTy != UnpadDstTy)
    return false;

  // Build the lambda that will create the copy
  MatchInfo = [=](MachineIRBuilder &B) { B.buildCopy(UnpadDst, PadSrc); };

  return true;
}

/// Match G_AIE_UNPAD_VECTOR fed by G_AIE_PAD_VECTOR_UNDEF where the types
/// don't match exactly, allowing fusion into a single G_AIE_PAD_VECTOR_UNDEF.
/// Transforms:
///   %padded:_(<16 x s32>) = G_AIE_PAD_VECTOR_UNDEF %src(<4 x s32>)
///   %dst:_(<8 x s32>) = G_AIE_UNPAD_VECTOR %padded(<16 x s32>)
/// Into:
///   %dst:_(<8 x s32>) = G_AIE_PAD_VECTOR_UNDEF %src(<4 x s32>)
bool llvm::matchPadUnpadFusion(MachineInstr &MI, MachineRegisterInfo &MRI,
                               const AIEBaseInstrInfo &TII,
                               BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TII.getGenericUnpadVectorOpcode() &&
         "Expected G_AIE_UNPAD_VECTOR");

  // Get the source of the unpad operation
  Register UnpadDst = MI.getOperand(0).getReg();
  Register UnpadSrc = MI.getOperand(1).getReg();

  // Check if source is G_AIE_PAD_VECTOR_UNDEF
  MachineInstr *PadMI = MRI.getVRegDef(UnpadSrc);
  if (!PadMI || PadMI->getOpcode() != TII.getGenericPadVectorOpcode())
    return false;

  Register PadSrc = PadMI->getOperand(1).getReg();

  // Get the types
  LLT PadSrcTy = MRI.getType(PadSrc);
  LLT PadDstTy = MRI.getType(PadMI->getOperand(0).getReg());
  LLT UnpadDstTy = MRI.getType(UnpadDst);

  // If input and output types match, this should be handled by
  // matchPadUnpadToCopy which creates a COPY
  if (PadSrcTy == UnpadDstTy)
    return false;

  // Verify that we can legally pad from PadSrc to UnpadDst
  if (!TII.isLegalTypeToPad(PadSrcTy) || !TII.isLegalTypeToUnpad(UnpadDstTy))
    return false;

  // Verify the intermediate type is also legal
  if (!TII.isLegalTypeToUnpad(PadDstTy))
    return false;

  // Build the lambda that will create the fused pad
  MatchInfo = [=, &TII](MachineIRBuilder &B) {
    B.buildInstr(TII.getGenericPadVectorOpcode(), {UnpadDst}, {PadSrc});
  };

  return true;
}

/// Match G_AIE_PAD_VECTOR_UNDEF fed by another G_AIE_PAD_VECTOR_UNDEF.
/// Transforms:
///   %intermediate:_(<8 x s32>) = G_AIE_PAD_VECTOR_UNDEF %src(<4 x s32>)
///   %dst:_(<16 x s32>) = G_AIE_PAD_VECTOR_UNDEF %intermediate(<8 x s32>)
/// Into:
///   %dst:_(<16 x s32>) = G_AIE_PAD_VECTOR_UNDEF %src(<4 x s32>)
bool llvm::matchPadPadFusion(MachineInstr &MI, MachineRegisterInfo &MRI,
                             const AIEBaseInstrInfo &TII,
                             BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TII.getGenericPadVectorOpcode() &&
         "Expected G_AIE_PAD_VECTOR_UNDEF");

  const Register OuterDst = MI.getOperand(0).getReg();
  const Register OuterSrc = MI.getOperand(1).getReg();

  // Check if source is also G_AIE_PAD_VECTOR_UNDEF
  MachineInstr *InnerPadMI = MRI.getVRegDef(OuterSrc);
  if (!InnerPadMI || InnerPadMI->getOpcode() != TII.getGenericPadVectorOpcode())
    return false;

  const Register InnerSrc = InnerPadMI->getOperand(1).getReg();

  // Get types
  const LLT InnerSrcTy = MRI.getType(InnerSrc);
  const LLT OuterDstTy = MRI.getType(OuterDst);

  // Verify we can legally pad directly from inner source to outer destination
  if (!TII.isLegalTypeToPad(InnerSrcTy) || !TII.isLegalTypeToUnpad(OuterDstTy))
    return false;

  // Build the fused PAD
  MatchInfo = [=, &TII](MachineIRBuilder &B) {
    B.buildInstr(TII.getGenericPadVectorOpcode(), {OuterDst}, {InnerSrc});
  };

  return true;
}

/// Match G_AIE_UNPAD_VECTOR fed by another G_AIE_UNPAD_VECTOR.
/// Transforms:
///   %intermediate:_(<8 x s32>) = G_AIE_UNPAD_VECTOR %src(<16 x s32>)
///   %dst:_(<4 x s32>) = G_AIE_UNPAD_VECTOR %intermediate(<8 x s32>)
/// Into:
///   %dst:_(<4 x s32>) = G_AIE_UNPAD_VECTOR %src(<16 x s32>)
bool llvm::matchUnpadUnpadFusion(MachineInstr &MI, MachineRegisterInfo &MRI,
                                 const AIEBaseInstrInfo &TII,
                                 BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TII.getGenericUnpadVectorOpcode() &&
         "Expected G_AIE_UNPAD_VECTOR");

  const Register OuterDst = MI.getOperand(0).getReg();
  const Register OuterSrc = MI.getOperand(1).getReg();

  // Check if source is also G_AIE_UNPAD_VECTOR
  MachineInstr *InnerUnpadMI = MRI.getVRegDef(OuterSrc);
  if (!InnerUnpadMI ||
      InnerUnpadMI->getOpcode() != TII.getGenericUnpadVectorOpcode())
    return false;

  const Register InnerSrc = InnerUnpadMI->getOperand(1).getReg();

  // Get types
  const LLT InnerSrcTy = MRI.getType(InnerSrc);
  const LLT OuterDstTy = MRI.getType(OuterDst);

  // Verify we can legally unpad directly from inner source to outer destination
  if (!TII.isLegalTypeToUnpad(InnerSrcTy) || !TII.isLegalTypeToPad(OuterDstTy))
    return false;

  // Build the fused UNPAD
  MatchInfo = [=, &TII](MachineIRBuilder &B) {
    B.buildInstr(TII.getGenericUnpadVectorOpcode(), {OuterDst}, {InnerSrc});
  };

  return true;
}

/// Match G_AIE_UNPAD_VECTOR fed by G_CONCAT_VECTORS where UNPAD discards
/// upper elements, allowing fusion into a smaller G_CONCAT_VECTORS.
/// Transforms:
///   %concat:_(<16 x s32>) = G_CONCAT_VECTORS %a(<4 x s32>), %b(<4 x s32>),
///                                            %c(<4 x s32>), %d(<4 x s32>)
///   %dst:_(<8 x s32>) = G_AIE_UNPAD_VECTOR %concat(<16 x s32>)
/// Into:
///   %dst:_(<8 x s32>) = G_CONCAT_VECTORS %a(<4 x s32>), %b(<4 x s32>)
bool llvm::matchConcatUnpadFusion(MachineInstr &MI, MachineRegisterInfo &MRI,
                                  const AIEBaseInstrInfo &TII,
                                  BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TII.getGenericUnpadVectorOpcode() &&
         "Expected G_AIE_UNPAD_VECTOR");

  // Get UNPAD operands
  const Register UnpadDst = MI.getOperand(0).getReg();
  const Register UnpadSrc = MI.getOperand(1).getReg();

  // Check if source is G_CONCAT_VECTORS
  MachineInstr *ConcatMI = MRI.getVRegDef(UnpadSrc);
  if (!ConcatMI || ConcatMI->getOpcode() != TargetOpcode::G_CONCAT_VECTORS)
    return false;

  // Get types
  const LLT UnpadDstTy = MRI.getType(UnpadDst);
  const LLT ConcatDstTy = MRI.getType(UnpadSrc);
  const LLT ConcatSrcTy = MRI.getType(ConcatMI->getOperand(1).getReg());

  const unsigned UnpadElements = UnpadDstTy.getNumElements();
  const unsigned ConcatOpElements = ConcatSrcTy.getNumElements();

  // Verify element counts align
  // This ensures the UNPAD output size is an exact multiple of each CONCAT
  // operand size, so we can cleanly extract complete operands.
  // Example: If UNPAD outputs 8 elements and each CONCAT operand has 4
  //          elements, then 8 % 4 = 0 (aligned), and we need exactly 2
  //          operands.
  // Counter-example: If UNPAD outputs 8 elements but each CONCAT operand has
  //                  3 elements, then 8 % 3 = 2 (NOT aligned), and we can't
  //                  cleanly extract 8 elements using 3-element chunks.
  if (UnpadElements % ConcatOpElements != 0)
    return false;

  const unsigned NumNeededOps = UnpadElements / ConcatOpElements;
  const unsigned TotalConcatOps = ConcatMI->getNumOperands() - 1; // -1 for def

  // Must discard at least one operand to be beneficial
  if (NumNeededOps >= TotalConcatOps)
    return false;

  // Verify CONCAT has only one use (the UNPAD)
  if (!MRI.hasOneNonDBGUse(UnpadSrc))
    return false;

  // Verify types are legal
  if (!TII.isLegalTypeToUnpad(ConcatDstTy))
    return false;

  // Build the transformation lambda
  MatchInfo = [=](MachineIRBuilder &B) {
    SmallVector<Register, 4> NeededOps;
    for (unsigned I = 0; I < NumNeededOps; I++) {
      NeededOps.push_back(ConcatMI->getOperand(I + 1).getReg());
    }
    B.buildConcatVectors(UnpadDst, NeededOps);
  };

  return true;
}

/// Match G_CONCAT_VECTORS with nested CONCAT or PAD operands that can be
/// flattened into a single CONCAT.
/// Transforms:
///   %a:_(<8 x s32>) = G_CONCAT_VECTORS %x(<4 x s32>), %y(<4 x s32>)
///   %b:_(<8 x s32>) = G_AIE_PAD_VECTOR_UNDEF %z(<4 x s32>)
///   %dst:_(<16 x s32>) = G_CONCAT_VECTORS %a(<8 x s32>), %b(<8 x s32>)
/// Into:
///   %dst:_(<16 x s32>) = G_CONCAT_VECTORS %x(<4 x s32>), %y(<4 x s32>),
///                                         %z(<4 x s32>), <undefined>
bool llvm::matchFlattenNestedConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                                    const AIEBaseInstrInfo &TII,
                                    BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_CONCAT_VECTORS &&
         "Expected G_CONCAT_VECTORS");

  const Register DstReg = MI.getOperand(0).getReg();
  const LLT DstTy = MRI.getType(DstReg);
  const unsigned DstElements = DstTy.getNumElements();

  // Collect flattened operands and track if we found nesting (always with
  // padding)
  SmallVector<Register, 8> FlattenedOps;
  unsigned TotalElements = 0;
  bool FoundNestingToFlatten = false;
  // Process each operand of the outer CONCAT
  for (unsigned I = 1; I < MI.getNumOperands(); ++I) {
    const Register OpReg = MI.getOperand(I).getReg();
    MachineInstr *OpMI = MRI.getVRegDef(OpReg);
    const LLT OpTy = MRI.getType(OpReg);
    const unsigned OpElements = OpTy.getNumElements();

    if (OpMI->getOpcode() == TargetOpcode::G_CONCAT_VECTORS) {
      // Nested CONCAT - flatten by extracting all its operands
      for (unsigned J = 1; J < OpMI->getNumOperands(); ++J) {
        const Register SubOp = OpMI->getOperand(J).getReg();
        const LLT SubOpTy = MRI.getType(SubOp);

        FlattenedOps.push_back(SubOp);
        TotalElements += SubOpTy.getNumElements();
      }
    } else if (OpMI->getOpcode() == TII.getGenericPadVectorOpcode()) {
      // PAD - unwrap to source and mark padding needed
      // Flat only if we have a pad.
      FoundNestingToFlatten = true;
      const Register PadSrc = OpMI->getOperand(1).getReg();
      const LLT PadSrcTy = MRI.getType(PadSrc);
      const unsigned PadSrcElements = PadSrcTy.getNumElements();

      FlattenedOps.push_back(PadSrc);
      TotalElements += PadSrcElements;

      // Calculate padding in terms of sub-vectors
      // The sub-vector size is the PAD source size
      const unsigned SubVecElements = PadSrcElements;
      const unsigned NumSubVecs = OpElements / SubVecElements;
      const unsigned PaddingSubVecs = NumSubVecs - 1;

      for (unsigned K = 0; K < PaddingSubVecs; ++K) {
        FlattenedOps.push_back(
            Register()); // One undefined sub-vector placeholder
      }
      TotalElements += PaddingSubVecs * SubVecElements;
    } else {
      // Regular operand - keep as-is

      FlattenedOps.push_back(OpReg);
      TotalElements += OpElements;
    }
  }

  // Must have found at least one nested structure to flatten
  if (!FoundNestingToFlatten)
    return false;

  // Verify total elements match destination
  if (TotalElements != DstElements)
    return false;

  // Verify all valid operands in FlattenedOps have the same type
  // and ensure we have at least one valid operand
  std::optional<LLT> SubVecTy;
  for (const Register Op : FlattenedOps) {
    if (Op.isValid()) {
      const LLT OpTy = MRI.getType(Op);
      if (!SubVecTy) {
        SubVecTy = OpTy;
      } else if (OpTy != *SubVecTy) {
        // Type mismatch in flattened operands - can't create valid CONCAT
        return false;
      }
    }
  }

  if (!SubVecTy)
    return false;

  // Build the transformation lambda
  // All validation is complete - this lambda is guaranteed to succeed
  MatchInfo = [=](MachineIRBuilder &B) {
    SmallVector<Register, 8> FinalOps;

    // Build final operand list, creating undefined for placeholders
    for (const Register Op : FlattenedOps) {
      if (!Op.isValid()) {
        // Create undefined for padding with the same type as other operands
        FinalOps.push_back(B.buildUndef(*SubVecTy).getReg(0));
      } else {
        FinalOps.push_back(Op);
      }
    }

    B.buildConcatVectors(DstReg, FinalOps);
  };

  return true;
}

/// Match G_AIE_PAD_VECTOR_UNDEF fed by G_AIE_UNPAD_VECTOR and combine to COPY.
/// Transforms:
///   %unpadded:_(<4 x s32>) = G_AIE_UNPAD_VECTOR %src(<16 x s32>)
///   %dst:_(<16 x s32>) = G_AIE_PAD_VECTOR_UNDEF %unpadded(<4 x s32>)
/// Into:
///   %dst:_(<16 x s32>) = COPY %src(<16 x s32>)
bool llvm::matchUnpadPadToCopy(MachineInstr &MI, MachineRegisterInfo &MRI,
                               const AIEBaseInstrInfo &TII,
                               BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TII.getGenericPadVectorOpcode() &&
         "Expected G_AIE_PAD_VECTOR_UNDEF");

  // Get the source of the pad operation
  Register PadDst = MI.getOperand(0).getReg();
  Register PadSrc = MI.getOperand(1).getReg();

  // Check if source is G_AIE_UNPAD_VECTOR
  MachineInstr *UnpadMI = MRI.getVRegDef(PadSrc);
  if (!UnpadMI || UnpadMI->getOpcode() != TII.getGenericUnpadVectorOpcode())
    return false;

  Register UnpadSrc = UnpadMI->getOperand(1).getReg();

  // Type validation: ensure output of UNPAD matches input of PAD
  LLT UnpadDstTy = MRI.getType(UnpadMI->getOperand(0).getReg());
  LLT PadSrcTy = MRI.getType(PadSrc);

  if (UnpadDstTy != PadSrcTy)
    return false;

  // Verify input of UNPAD matches output of PAD
  LLT UnpadSrcTy = MRI.getType(UnpadSrc);
  LLT PadDstTy = MRI.getType(PadDst);

  if (UnpadSrcTy != PadDstTy)
    return false;

  // Verify types are legal for these operations
  if (!TII.isLegalTypeToPad(PadSrcTy) || !TII.isLegalTypeToUnpad(UnpadSrcTy))
    return false;

  // Build the lambda that will create the copy
  MatchInfo = [=](MachineIRBuilder &B) { B.buildCopy(PadDst, UnpadSrc); };

  return true;
}

// Match something like:
// %0:_(s32), %1:_(s32), %2:_(s32), %3:_(s32) = G_UNMERGE_VALUES %10(<4 x s32>)
// %4:_(s32) = G_IMPLICIT_DEF
// %11:_(<8 x s32>) = G_BUILD_VECTOR %0, %1, %2, %3, %4, %4, %4, %4
//
// To turn it into:
// %11:_(<8 x s32>) = G_AIE_PAD_VECTOR_UNDEF %10
bool llvm::matchPadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                          const AIEBaseInstrInfo &TII,
                          Register &MatchedInputVector) {
  assert(MI.getOpcode() == TargetOpcode::G_BUILD_VECTOR &&
         "Expected build vector");
  MachineInstr *Unmerge = MRI.getVRegDef(MI.getOperand(1).getReg());
  if (!Unmerge || Unmerge->getOpcode() != TargetOpcode::G_UNMERGE_VALUES)
    return false;

  // Look for G_UNMERGE_VALUES of a 128-bit register
  Register UnmergedReg = Unmerge->getOperand(Unmerge->getNumDefs()).getReg();
  LLT UnmergedInputTy = MRI.getType(UnmergedReg);
  LLT DstVectorTy = MRI.getType(MI.getOperand(0).getReg());
  if (!TII.isLegalTypeToPad(UnmergedInputTy) ||
      !TII.isLegalTypeToUnpad(DstVectorTy))
    return false;

  // Verify all the lanes of UnmergedReg are correctly copied into the
  // BUILD_VECTOR, and the remaining lanes are implicit defs.
  for (unsigned Idx = 1, EndIdx = MI.getNumExplicitOperands(); Idx != EndIdx;
       ++Idx) {
    unsigned LaneIdx = Idx - 1;
    Register LaneReg = MI.getOperand(Idx).getReg();
    if (LaneIdx < Unmerge->getNumDefs() &&
        LaneReg != Unmerge->getOperand(LaneIdx).getReg())
      return false;
    if (LaneIdx >= Unmerge->getNumDefs() &&
        MRI.getUniqueVRegDef(LaneReg)->getOpcode() !=
            TargetOpcode::G_IMPLICIT_DEF)
      return false;
  }

  MatchedInputVector = UnmergedReg;
  return true;
}

bool llvm::matchConcatPadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                                const AIEBaseInstrInfo &TII,
                                Register &MatchedInputVector) {
  assert(MI.getOpcode() == TargetOpcode::G_CONCAT_VECTORS &&
         "Expected concat vector");

  auto DstReg = MI.getOperand(0).getReg();
  LLT DstVectorTy = MRI.getType(DstReg);
  auto SrcReg1 = MI.getOperand(1).getReg();
  LLT SrcVector1Ty = MRI.getType(SrcReg1);
  if (!TII.isLegalTypeToPad(SrcVector1Ty) ||
      !TII.isLegalTypeToUnpad(DstVectorTy))
    return false;

  for (unsigned Index = 2; Index < MI.getNumOperands(); Index++) {
    if (!getOpcodeDef(TargetOpcode::G_IMPLICIT_DEF,
                      MI.getOperand(Index).getReg(), MRI))
      return false;
  }

  MatchedInputVector = SrcReg1;
  return true;
}

void llvm::applyPadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                          MachineIRBuilder &B, Register MatchedInputVector,
                          GISelChangeObserver &Observer) {
  B.setInstrAndDebugLoc(MI);
  const AIEBaseInstrInfo &AIETII = (const AIEBaseInstrInfo &)B.getTII();
  Register DstReg = MI.getOperand(0).getReg();
  B.buildInstr(AIETII.getGenericPadVectorOpcode(), {DstReg},
               {MatchedInputVector});
  Observer.erasingInstr(MI);
  MI.eraseFromParent();
}

/// Match something like this:
///  %68:_(s32) = G_CONSTANT i32 0
///  %93:_(s32) = G_CONSTANT i32 1
///  %209:_(<16 x s64>) = G_INTRINSIC intrinsic(@llvm.aie2.concat.1024.512.acc),
///                   %206(<8 x s64>), %208(<8 x s64>)
///  %216:_(<8 x s64>) = G_INTRINSIC intrinsic(@llvm.aie2.ext.512.1024.acc),
///                   %209(<16 x s64>), %68(s32)
///  %219:_(<8 x s64>) = G_INTRINSIC intrinsic(@llvm.aie2.ext.512.1024.acc),
///                    %209(<16 x s64>), %93(s32)

/// To convert to:
///   %216:_(<8 x s64>) = COPY %206(<8 x s64>)
///   %219:_(<8 x s64>) = COPY %208(<8 x s64>)
bool llvm::matchExtractConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                              const AIEBaseInstrInfo &TII,
                              Register &MatchInfo) {

  std::optional<const AIEBaseInstrInfo::VExtractOpInfo> ExtractOp =
      TII.getVExtractOpInfo(MI);

  if (!ExtractOp)
    return false;

  auto Cst = getIConstantVRegValWithLookThrough(
      MI.getOperand(ExtractOp->SubVectorIndex).getReg(), MRI);

  if (!Cst)
    return false;

  const unsigned ExtractSize =
      MRI.getType(MI.getOperand(0).getReg()).getSizeInBits();

  MachineInstr &SrcMI = *MRI.getVRegDef(MI.getOperand(ExtractOp->Src).getReg());

  Register SrcReg;
  unsigned ConcatSize = 0;
  const unsigned Index = Cst->Value.getZExtValue();

  if (const auto ConcatOp = TII.getVConcatOpInfo(SrcMI)) {
    SrcReg = SrcMI.getOperand(Index + ConcatOp->FirstOperand).getReg();
    ConcatSize = MRI.getType(SrcReg).getSizeInBits();
  }

  MatchInfo = SrcReg;
  return ConcatSize == ExtractSize;
}

void llvm::applyExtractConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                              MachineIRBuilder &B, Register &MatchInfo,
                              GISelChangeObserver &Observer) {
  B.setInstrAndDebugLoc(MI);
  Register DstReg = MI.getOperand(0).getReg();
  Register SrcReg = MatchInfo;

  B.buildCopy(DstReg, SrcReg);
  Observer.erasingInstr(MI);
  MI.eraseFromParent();
}

/// Match something like this:
/// %209:_(<16 x s32>) = G_INTRINSIC intrinsic(@llvm.aie2.concat.I512.I256),
///         %95(<8 x s32>), %98(<8 x s32>)
/// %252:_(<8 x s32>), %253:_(<8 x s32>) = G_UNMERGE_VALUES %209(<16 x s32>)
///
/// To convert to:
/// 252:_(<8 x s32>) = COPY %95(<8 x s32>)
/// 253:_(<8 x s32>) = COPY %98(<8 x s32>)
bool llvm::matchUnmergeConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                              const AIEBaseInstrInfo &TII,
                              std::pair<MachineInstr *, unsigned> &MatchInfo) {

  MachineInstr &SrcMI =
      *MRI.getVRegDef(MI.getOperand(MI.getNumOperands() - 1).getReg());

  std::optional<const AIEBaseInstrInfo::VConcatOpInfo> ConcatOp =
      TII.getVConcatOpInfo(SrcMI);

  if (!ConcatOp)
    return false;

  // We always have more operands for the intrinsic.
  if (MI.getNumOperands() !=
      SrcMI.getNumOperands() - ConcatOp->NumOfNonRegOperands)
    return false;

  MatchInfo = std::make_pair(&SrcMI, ConcatOp->FirstOperand);
  return true;
}

void llvm::applyUnmergeConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                              MachineIRBuilder &B,
                              std::pair<MachineInstr *, unsigned> &MatchInfo,
                              GISelChangeObserver &Observer) {
  B.setInstrAndDebugLoc(MI);
  auto [ConcatMI, Offset] = MatchInfo;

  for (unsigned Op = 0; Op < MI.getNumOperands() - 1; Op++) {
    Register DstReg = MI.getOperand(Op).getReg();
    Register SrcReg = ConcatMI->getOperand(Op + Offset).getReg();
    B.buildCopy(DstReg, SrcReg);
  }

  Observer.erasingInstr(MI);
  MI.eraseFromParent();
}

/// Check if two machine instructions are equivalent for CSE purposes.
/// Instructions are considered equivalent if they have:
/// - The same opcode
/// - The same number of operands
/// - Matching input operands (starting from operand 1, skipping the def)
/// \param MI1 First instruction to compare
/// \param MI2 Second instruction to compare
/// \return true if instructions are equivalent
static bool isEquivalentMI(const MachineInstr &MI1, const MachineInstr &MI2) {
  // Must be the same opcode
  if (MI1.getOpcode() != MI2.getOpcode())
    return false;

  // Must have the same number of operands
  const unsigned NumOps = MI1.getNumOperands();
  if (MI2.getNumOperands() != NumOps)
    return false;

  // Check if all input operands match (starting from operand 1)
  for (unsigned I = 1; I < NumOps; ++I) {
    if (MI1.getOperand(I).getReg() != MI2.getOperand(I).getReg())
      return false;
  }

  return true;
}

/// Match duplicate vector operations with identical operands for CSE
/// optimization.
///
/// Currently handles: G_CONCAT_VECTORS
///
/// This combiner can be extended to other pure vector operations after proper
/// testing, such as:
/// - G_AIE_UNPAD_VECTOR: Extracts lower elements from a padded vector
/// - G_AIE_PAD_VECTOR_UNDEF: Pads a vector with undefined upper elements
///
/// The implementation is already general enough to support these operations.
/// To extend, simply add the desired opcodes to the wip_match_opcode list in
/// the combine_cse_vector_ops rule in AIECombine.td.
///
/// Algorithm:
/// Uses MRI to efficiently find potential duplicates by checking all users of
/// the first input operand. For each candidate with matching opcode and operand
/// count, verifies all operands match exactly. If a dominating duplicate is
/// found, replaces the current operation with a copy from the earlier result.
///
/// \param MI The instruction to check for duplication
/// \param MRI Machine register info for querying definitions and uses
/// \param Helper Combiner helper for dominance checks
/// \param MatchInfo Output parameter - register to copy from if match found
/// \return true if a dominating duplicate was found
bool llvm::matchCSEVectorOp(MachineInstr &MI, MachineRegisterInfo &MRI,
                            CombinerHelper &Helper, Register &MatchInfo) {

  // The tablegen rule filters to only the supported opcodes:
  // G_CONCAT_VECTORS.
  // We don't need to assert here since the match pattern guarantees this

  const unsigned NumOps = MI.getNumOperands();
  if (NumOps < 2)
    return false;

  // Get the first input operand (operand 1 for all these operations)
  const Register FirstInput = MI.getOperand(1).getReg();

  // Check all users of the first input register to find potential duplicates
  for (MachineInstr &UserMI : MRI.use_nodbg_instructions(FirstInput)) {
    // Skip the current instruction itself
    if (&UserMI == &MI)
      continue;

    // Check if instructions are equivalent
    if (!isEquivalentMI(MI, UserMI))
      continue;

    // Check dominance: UserMI must dominate MI for safe CSE
    if (Helper.dominates(UserMI, MI)) {
      // Found a dominating duplicate - return its result register
      MatchInfo = UserMI.getOperand(0).getReg();
      return true;
    }
  }

  return false;
}

/// This function tracks chain of vector updates using .upd vector intrinsic.
static std::map<unsigned, Register>
trackVectorUpdateChain(MachineInstr &MI, MachineRegisterInfo &MRI,
                       const AIEBaseInstrInfo &TII) {

  std::optional<const AIEBaseInstrInfo::VUpdateOpInfo> UpdateOp =
      TII.getVUpdateOpInfo(MI);

  if (!UpdateOp)
    return {};

  auto Cst = getIConstantVRegValWithLookThrough(
      MI.getOperand(UpdateOp->SubVectorIndex).getReg(), MRI);

  if (!Cst)
    return {};

  std::map<unsigned, Register> IndexRegMap = trackVectorUpdateChain(
      *MRI.getVRegDef(MI.getOperand(UpdateOp->Src).getReg()), MRI, TII);

  const unsigned Index = Cst->Value.getZExtValue();
  Register RegLane = MI.getOperand(UpdateOp->SrcSubVec).getReg();

  // Check if we already have this update in the chain.
  if (IndexRegMap.find(Index) != IndexRegMap.end())
    return {};

  IndexRegMap[Index] = RegLane;

  return IndexRegMap;
}

/// Match something like this:
///  %21:_(s32) = G_CONSTANT i32 0
///  %51:_(s32) = G_CONSTANT i32 1
///  %96:_(<8 x s32>) = G_BITCAST %95(<32 x s8>)
///  %97:_(<16 x s32>) = G_INTRINSIC intrinsic(@llvm.aie2.upd.I512.I256),
///           %9(<16 x s32>), %96(<8 x s32>), %21(s32)
///  %99:_(<8 x s32>) = G_BITCAST %98(<32 x s8>)
///  %100:_(<16 x s32>) = G_INTRINSIC intrinsic(@llvm.aie2.upd.I512.I256),
///           %97(<16 x s32>), %99(<8 x s32>), %51(s32)

/// To convert to:
///  %96:_(<16 x s32>) = G_CONCAT_VECTORS  %96(<8 x s32>), %99(<8 x s32>)
bool llvm::matchUpdToConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                            const AIEBaseInstrInfo &TII,
                            std::map<unsigned, Register> &IndexRegMap) {

  IndexRegMap = trackVectorUpdateChain(MI, MRI, TII);

  if (IndexRegMap.size() == 0)
    return false;

  const unsigned UpdSize =
      MRI.getType(MI.getOperand(0).getReg()).getSizeInBits();

  unsigned ConcatenatedSize = 0;
  for (auto IndexReg : IndexRegMap) {
    Register Reg = IndexReg.second;
    ConcatenatedSize += MRI.getType(Reg).getSizeInBits();
  }

  if (UpdSize != ConcatenatedSize)
    return false;

  return true;
}

/// Find a use of \p MI in the same block where it can be moved
MachineInstr &findClosestToUseInsertPoint(MachineInstr &MI,
                                          MachineRegisterInfo &MRI) {

  for (auto &User : MRI.use_instructions(MI.getOperand(0).getReg())) {
    if (User.isPHI())
      continue;
    if (User.getParent() == MI.getParent() && canDelayMemOp(MI, User, MRI))
      return User;
  }

  return MI;
}

void llvm::applyUpdToConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                            MachineIRBuilder &B,
                            std::map<unsigned, Register> &IndexRegMap,
                            GISelChangeObserver &Observer) {
  B.setDebugLoc(MI.getDebugLoc());
  B.setInstr(findClosestToUseInsertPoint(MI, MRI));

  SmallVector<Register, 4> SrcRegs;
  for (unsigned Op = 0; Op < IndexRegMap.size(); Op++) {
    SrcRegs.push_back(IndexRegMap[Op]);
  }

  B.buildConcatVectors(MI.getOperand(0).getReg(), SrcRegs);

  Observer.erasingInstr(MI);
  MI.eraseFromParent();
}

bool llvm::matchLoadStoreSplit(GLoadStore &MI, MachineRegisterInfo &MRI,
                               const AIEBaseInstrInfo &TII,
                               unsigned &MaxMemSize) {

  const Register ValReg = MI.getReg(0);
  const LLT ValTy = MRI.getType(ValReg);
  const bool IsLoad = isa<GLoad>(MI);
  MaxMemSize = TII.getMaxLoadStoreSize();

  if (!TII.isProfitableToSplitType(ValTy))
    return false;

  /// Avoid splitting operations that can be combined `as is`.
  if (IsLoad) {
    for (MachineInstr &ConvInstr : MRI.use_instructions(ValReg)) {
      if (TII.canCombineWithLoadStore(ConvInstr))
        return false;
    }
  } else {
    MachineInstr &ConvInstr = *getDefIgnoringCopiesAndBitcasts(ValReg, MRI);
    if (TII.canCombineWithLoadStore(ConvInstr))
      return false;
  }

  return true;
}

void llvm::applyLoadStoreSplit(GLoadStore &MI, MachineRegisterInfo &MRI,
                               MachineIRBuilder &B, const unsigned MaxMemSize,
                               GISelChangeObserver &Observer) {

  assert(MaxMemSize && "MaxMemSize should be specified!");
  B.setInstrAndDebugLoc(MI);
  MachineFunction &MF = B.getMF();
  const bool IsLoad = isa<GLoad>(MI);
  const Register ValReg = MI.getReg(0);
  const Register AddrReg = MI.getPointerReg();
  const LLT ValTy = MRI.getType(ValReg);
  const LLT PtrTy = MRI.getType(AddrReg);
  const LLT OffsetTy = LLT::scalar(PtrTy.getSizeInBits());
  const unsigned NumParts = ValTy.getSizeInBits() / MaxMemSize;
  const LLT NarrowTy = ValTy.divide(NumParts);
  const MachineMemOperand MMO = MI.getMMO();

  SmallVector<Register, 8> NarrowRegs;
  if (!IsLoad)
    extractParts(ValReg, NarrowTy, NumParts, NarrowRegs, B, MRI);

  for (int I = NumParts - 1; I >= 0; I--) {
    const unsigned ByteOffset = I * NarrowTy.getSizeInBytes();
    Register NewAddrReg;
    B.materializePtrAdd(NewAddrReg, AddrReg, OffsetTy, ByteOffset);
    MachineMemOperand *NewMMO =
        MF.getMachineMemOperand(&MMO, ByteOffset, NarrowTy);

    if (IsLoad) {
      Register Dst = MRI.createGenericVirtualRegister(NarrowTy);
      NarrowRegs.push_back(Dst);
      B.buildLoad(Dst, NewAddrReg, *NewMMO);
    } else {
      B.buildStore(NarrowRegs[I], NewAddrReg, *NewMMO);
    }
  }

  if (IsLoad) {
    std::reverse(NarrowRegs.begin(), NarrowRegs.end());
    B.buildConcatVectors(ValReg, NarrowRegs);
  }

  Observer.erasingInstr(MI);
  MI.eraseFromParent();
}

/// Match something like this:
///  %293:_(s20) = G_CONSTANT i20 32
///  %67:_(s20) = G_CONSTANT i20 64
///  %68:_(p0) = nuw G_PTR_ADD %61, %67(s20)
///  %295:_(<16 x s16>) = G_AIE_OFFSET_LOAD %68(p0), %293(s20)

/// To convert to:
///  %298:_(s20) = G_CONSTANT i20 96
///  %295:_(<16 x s16>) = G_AIE_OFFSET_LOAD %61(p0), %298(s20)
bool llvm::matchOffsetLoadStorePtrAdd(MachineInstr &MI,
                                      MachineRegisterInfo &MRI,
                                      const AIEBaseInstrInfo &TII,
                                      std::pair<Register, int64_t> &RegOffset) {

  const Register AddrReg = MI.getOperand(1).getReg();

  const auto CstOffsetLoadStore =
      getIConstantVRegValWithLookThrough(MI.getOperand(2).getReg(), MRI);

  if (!CstOffsetLoadStore)
    return false;

  MachineInstr *DefAddrRegInstr = MRI.getVRegDef(AddrReg);

  if (DefAddrRegInstr->getOpcode() != TargetOpcode::G_PTR_ADD)
    return false;

  const auto CstDefAddrRegInstr = getIConstantVRegValWithLookThrough(
      DefAddrRegInstr->getOperand(2).getReg(), MRI);

  if (!CstDefAddrRegInstr)
    return false;

  RegOffset.first = DefAddrRegInstr->getOperand(1).getReg();
  RegOffset.second = CstDefAddrRegInstr->Value.getSExtValue() +
                     CstOffsetLoadStore->Value.getSExtValue();

  return true;
}

void llvm::applyOffsetLoadStorePtrAdd(
    MachineInstr &MI, MachineRegisterInfo &MRI, MachineIRBuilder &B,
    const std::pair<Register, int64_t> &RegOffset,
    GISelChangeObserver &Observer) {
  B.setInstrAndDebugLoc(MI);

  Register NewOffsetReg =
      B.buildConstant(LLT::scalar(20), RegOffset.second).getReg(0);

  Observer.changingInstr(MI);
  MI.getOperand(1).setReg(RegOffset.first);
  MI.getOperand(2).setReg(NewOffsetReg);
  Observer.changedInstr(MI);
}

/// Match something like this:
///  %0:_(s20) = COPY $m0
///  %1:_(p0) = COPY $p0
///  %2:_(<16 x s32>) = COPY $x0
///  %6:_(p0) = G_PTR_ADD %1, %0(s20)
///  %18:_(s20) = G_CONSTANT i20 32
///  G_AIE_OFFSET_STORE %15(<8 x s32>), %6(p0), %18(s20)
///  G_AIE_OFFSET_STORE %14(<8 x s32>), %1(p0), %0(s20)

/// To convert to (pointer reuse/CSE):
///  %0:_(s20) = COPY $m0
///  %1:_(p0) = COPY $p0
///  %2:_(<16 x s32>) = COPY $x0
///  %6:_(p0) = G_PTR_ADD %1, %0(s20)
///  %18:_(s20) = G_CONSTANT i20 32
///  %19:_(s20) = G_CONSTANT i20 0
///  G_AIE_OFFSET_STORE %15(<8 x s32>), %6(p0), %18(s20)
///  G_AIE_OFFSET_STORE %14(<8 x s32>), %6(p0), %19(s20)
bool llvm::matchOffsetLoadStoreSharePtrAdd(MachineInstr &MI,
                                           MachineRegisterInfo &MRI,
                                           CombinerHelper &Helper,
                                           const AIEBaseInstrInfo &TII,
                                           Register &PtrAddReg) {
  const Register PtrReg = MI.getOperand(1).getReg();
  const Register OffsetReg = MI.getOperand(2).getReg();

  const auto OffsetCst = getIConstantVRegValWithLookThrough(OffsetReg, MRI);

  // If we have a constant here, don't touch because it is better
  // to stay folded. Otherwise we will fold again in the previous
  // combiner.
  if (OffsetCst)
    return false;

  for (auto &Use : MRI.use_nodbg_instructions(PtrReg)) {
    if (Use.getOpcode() != TargetOpcode::G_PTR_ADD)
      continue;
    if (Use.getOperand(2).getReg() != OffsetReg)
      continue;
    if (Use.getParent() != MI.getParent())
      continue;
    if (!Helper.dominates(Use, MI))
      continue;

    Register PaddDestReg = Use.getOperand(0).getReg();

    // Dead instruction? Don't use it!
    // Ony use if at least another instruction is using it.
    if (hasNItemsOrMore(MRI.use_instr_nodbg_begin(PaddDestReg),
                        MRI.use_instr_nodbg_end(), 1)) {
      PtrAddReg = PaddDestReg;
      return true;
    }
  }

  return false;
}

bool llvm::matchCopyOfImplicitDef(MachineInstr &MI, MachineRegisterInfo &MRI) {
  assert(MI.isCopy() && "Expected a COPY instruction");
  const Register DstReg = MI.getOperand(0).getReg();
  const Register SrcReg = MI.getOperand(1).getReg();
  if (!DstReg.isVirtual() || !SrcReg.isVirtual())
    return false;
  const MachineInstr *SrcDef = MRI.getVRegDef(SrcReg);
  if (!SrcDef || SrcDef->getOpcode() != TargetOpcode::G_IMPLICIT_DEF)
    return false;
  return MRI.getType(DstReg) == MRI.getType(SrcReg);
}

void llvm::applyCopyOfImplicitDef(MachineInstr &MI, MachineRegisterInfo &MRI,
                                  MachineIRBuilder &B,
                                  GISelChangeObserver &Observer) {
  const Register DstReg = MI.getOperand(0).getReg();
  const Register SrcReg = MI.getOperand(1).getReg();
  const RegisterBank *DstRB = MRI.getRegBankOrNull(DstReg);
  const RegisterBank *SrcRB = MRI.getRegBankOrNull(SrcReg);
  if (DstRB == SrcRB) {
    MRI.replaceRegWith(DstReg, SrcReg);
  } else {
    const LLT DstTy = MRI.getType(DstReg);
    const Register NewReg = MRI.createGenericVirtualRegister(DstTy);
    if (DstRB)
      MRI.setRegBank(NewReg, *DstRB);
    B.setInsertPt(*MI.getParent(), MI.getIterator());
    B.buildInstr(TargetOpcode::G_IMPLICIT_DEF, {NewReg}, {});
    MRI.replaceRegWith(DstReg, NewReg);
  }
  Observer.erasingInstr(MI);
  MI.eraseFromParent();
}

void llvm::applyOffsetLoadStoreSharePtrAdd(MachineInstr &MI,
                                           MachineRegisterInfo &MRI,
                                           MachineIRBuilder &B,
                                           Register &PtrAddReg,
                                           GISelChangeObserver &Observer) {

  Register NewOffsetReg = B.buildConstant(LLT::scalar(20), 0).getReg(0);

  Observer.changingInstr(MI);
  MI.getOperand(1).setReg(PtrAddReg);
  MI.getOperand(2).setReg(NewOffsetReg);
  Observer.changedInstr(MI);
}

static bool isPowerOfTwoOrZero(unsigned Height) {
  return Height == 0 || (Height > 1 && has_single_bit(Height));
}

/// \returns true if it is possible to combine the below sequence of MIRs
/// into a COPY.
/// From : %1:_(<64 x s8>) = G_IMPLICIT_DEF
///        %2:_(<16 x s32>) = G_BITCAST %1:_(<64 x s8>)
///        %3:_(s32) = G_CONSTANT i32 0
///        %4:_(<16 x s32>) = G_INTRINSIC
///        intrinsic(@llvm.aie[2/2p].vshift.I512.I512), %X:_(<16 x s32>),
///        %2:_(<16 x s32>), %3:_(s32), %3:_(s32)
/// To :   4%:_(<16 x s32>) = COPY %X
/// Or even:
/// From : %1:_(<64 x s8>) = G_IMPLICIT_DEF
///        %2:_(s32) = G_CONSTANT i32 0
///        %3:_(<16 x s32>) = G_INTRINSIC
///        intrinsic(@llvm.aie[2/2p].vshift.I512.I512), %X:_(<16 x s32>),
///        %1:_(<16 x s32>), %2:_(s32), %2:_(s32)
/// To :   3%:_(<16 x s32>) = COPY %X
bool llvm::tryToCombineVectorShiftsByZero(MachineInstr &MI,
                                          MachineRegisterInfo &MRI,
                                          GISelChangeObserver &Observer) {

  const Register DstReg = MI.getOperand(0).getReg();
  const Register SrcReg = MI.getOperand(2).getReg();
  const Register ThirdSrcReg = MI.getOperand(4).getReg();
  const Register ShiftAmtSrcReg = MI.getOperand(5).getReg();

  auto IsConstantZeroReg = [&](const Register Reg) {
    auto Cst = getIConstantVRegValWithLookThrough(Reg, MRI);
    return Cst && Cst->Value.isZero();
  };

  if (!IsConstantZeroReg(ThirdSrcReg) || !IsConstantZeroReg(ShiftAmtSrcReg))
    return false;

  MachineIRBuilder MIRBuilder(MI);
  MIRBuilder.buildCopy(DstReg, SrcReg);
  Observer.erasingInstr(MI);
  MI.eraseFromParent();

  return true;
}

/// Match a G_SHUFFLE_VECTOR that splats a single element from a source.
/// The source can be either a G_BUILD_VECTOR or a scalar (e.g., s32/s64 from
/// <1 x i32>/<1 x i64> lowered to scalar type).
///
/// Patterns matched:
/// 1. Vector source from G_BUILD_VECTOR:
///      %src:_(<N x sM>) = G_BUILD_VECTOR %e0, %e1, ..., %eK
///      %dst:_(<P x sM>) = G_SHUFFLE_VECTOR %src, %undefined, shufflemask(K, K,
///      ...)
/// 2. Scalar source (treated as single-element):
///      %src:_(sM) = ...
///      %dst:_(<N x sM>) = G_SHUFFLE_VECTOR %src, %undefined, shufflemask(0, 0,
///      ...)
///
/// Returns (DstReg, ElemReg) in MatchInfo for the apply function to create a
/// broadcast. For scalar sources, ElemReg is the scalar register itself.
/// For vector sources, ElemReg is extracted from the G_BUILD_VECTOR operands.
///
/// \param MI The G_SHUFFLE_VECTOR instruction to match.
/// \param MRI The MachineRegisterInfo for type and def queries.
/// \param MatchInfo Output pair of (destination register, element register).
/// \returns true if the pattern matches and MatchInfo is populated.
bool llvm::matchBroadcastElement(MachineInstr &MI, MachineRegisterInfo &MRI,
                                 std::pair<Register, Register> &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_SHUFFLE_VECTOR);
  const auto MaybeSplatIndex = getSplatIndex(MI);

  if (!MaybeSplatIndex.has_value())
    return false;

  const LLT SrcTy = MRI.getType(MI.getOperand(1).getReg());

  unsigned SrcNumElems = 1;
  if (SrcTy.isVector())
    SrcNumElems = SrcTy.getNumElements();

  unsigned Idx;
  unsigned AdjustSrcElemIdx = 0;
  if (MaybeSplatIndex.value() < (int)SrcNumElems) {
    Idx = 1;
  } else {
    Idx = 2;
    AdjustSrcElemIdx = SrcNumElems;
  }

  const Register SrcVecReg = MI.getOperand(Idx).getReg();
  MachineInstr *SrcVec = MRI.getUniqueVRegDef(SrcVecReg);

  const Register DstReg = MI.getOperand(0).getReg();

  if (!MRI.getType(SrcVecReg).isVector()) {
    MatchInfo = std::make_pair(DstReg, SrcVecReg);
    return true;
  }
  if (!SrcVec || SrcVec->getOpcode() != TargetOpcode::G_BUILD_VECTOR) {
    return false;
  }

  const unsigned SrcElemIdx = MaybeSplatIndex.value() + 1;
  const Register ElemReg =
      SrcVec->getOperand(SrcElemIdx - AdjustSrcElemIdx).getReg();
  MatchInfo = std::make_pair(DstReg, ElemReg);
  return true;
}

/// \returns true if it is possible to combine the shuffle vector to VSEL.
/// E.g.:
/// From :  %0:_(<16 x s32>) = COPY $x0
///         %1:_(<16 x s32>) = COPY $x1
///         %2:_(<16 x s32>) = G_SHUFFLE_VECTOR %X(<16 x s32>), %1(<16 x s32>),
///         shufflemask(0, 1, 2, 3, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
///         31)
/// To :    %3:_(s32) = G_CONSTANT i32 0xFFF0
///         %4:_(<16 x s32>) = G_AIE_VSEL %0, %1, %3(s32)
bool llvm::matchShuffleToVSel(MachineInstr &MI, MachineRegisterInfo &MRI,
                              const AIEBaseInstrInfo &TII,
                              BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_SHUFFLE_VECTOR);

  const unsigned BasicVectorBitSize = TII.getBasicVectorBitSize();
  const unsigned ScalarRegSize = TII.getScalarRegSize();
  const unsigned DoubleScalarRegSize = ScalarRegSize * 2;

  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  const Register Src2Reg = MI.getOperand(2).getReg();
  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  if (Src1Ty.getSizeInBits() != BasicVectorBitSize ||
      Src1Ty.getElementType().getSizeInBits() >= DoubleScalarRegSize)
    return false;

  const unsigned NumDstElems = DstTy.getNumElements();
  assert(NumDstElems == Mask.size());
  const unsigned NumSrcElems = Src1Ty.getNumElements();
  if (NumDstElems > NumSrcElems)
    return false;
  const unsigned NumSubVectors = NumSrcElems / NumDstElems;
  if ((NumSubVectors != 1 && NumSubVectors != 2) ||
      NumSrcElems % NumDstElems != 0) {
    return false;
  }

  // Someone should make undef out of this.
  if (MaskMatch::isMaskWithAllUndefs(Mask))
    return false;

  // Check that the shuffle mask can be converted into VSel condition vector:
  // Each element can select from the corresponding element from the first or
  // the second vector.
  // Hence, the mask value should either be don't care, equal to the index,
  // or equal to the index + NumSrcElems
  // This immediately defines the mask vector of the VSEL.
  uint64_t DstMask = 0;
  auto MatchVSEL = [&](unsigned Shift) {
    DstMask = 0;
    for (unsigned I = 0; I < NumDstElems; I++) {
      const int Idx = Mask[I];
      if (Idx == -1) {
        continue;
      }
      const int EffPos = I + Shift;
      if (Idx == EffPos)
        continue;

      if (Idx != EffPos + (int)NumSrcElems) {
        return false;
      }
      DstMask |= uint64_t(1) << I;
    }
    return true;
  };

  int SubIdx = 0;
  // A subvector can be matched with a shift corresponding to the subvector to
  // extract.
  auto MatchSubVector = [&]() {
    for (unsigned Shift = 0; Shift < NumSrcElems; Shift += NumDstElems) {
      if (MatchVSEL(Shift)) {
        return true;
      }
      SubIdx++;
    }
    return false;
  };

  if (!MatchSubVector()) {
    return false;
  }

  MatchInfo = [=, &MRI, &TII](MachineIRBuilder &B) {
    const unsigned ScalarSize =
        NumDstElems == DoubleScalarRegSize ? DoubleScalarRegSize : 32;
    MachineInstrBuilder MaskReg =
        B.buildConstant(LLT::scalar(ScalarSize), DstMask);
    const unsigned VSelOpc = TII.getGenericVSelOpcode();
    if (NumDstElems == NumSrcElems)
      B.buildInstr(VSelOpc, {DstReg}, {Src1Reg, Src2Reg, MaskReg});
    else { // NumDstElems < NumSrcElems
      const unsigned Src1ElemtSize = Src1Ty.getElementType().getSizeInBits();
      const unsigned Src1Vec512BitLen = BasicVectorBitSize / Src1ElemtSize;
      const LLT VSelDstTy = LLT::fixed_vector(Src1Vec512BitLen, Src1ElemtSize);
      const Register VSelDstReg = MRI.createGenericVirtualRegister(VSelDstTy);
      B.buildInstr(VSelOpc, {VSelDstReg}, {Src1Reg, Src2Reg, MaskReg});
      buildUnmergeVector(B, MRI, DstReg, VSelDstReg, NumSubVectors, SubIdx);
    }
  };
  return true;
}

/// \returns true if it is possible to combine the shuffle vector with a mask
/// that extracts an element from the first source vector and broadcasts
/// it. E.g.:
/// From :  %X:_(<4 x s64>) = COPY $wl0
///         %1:_(<4 x s64>) = COPY $wl1
///         %2:_(<8 x s64>) = G_SHUFFLE_VECTOR %X(<4 x s64>), %1(<4 x s64>),
///         shufflemask(3, 3, 3, 3, 3, 3, 3, 3)
/// To :    %3:_(s64) = G_EXTRACT_VECTOR_ELT %X, 3
///         %2:_(<8 x s64>) = G_AIE_BROADCAST_VECTOR %3(s64)
static bool matchShuffleToVecEltBroadcast(MachineInstr &MI,
                                          MachineRegisterInfo &MRI,
                                          const AIEBaseInstrInfo &TII,
                                          BuildFnTy &MatchInfo) {
  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  std::optional<int> UniqOpIdx = MaskMatch::getUniqueIndex(Mask);
  if (!UniqOpIdx)
    return false;

  assert(UniqOpIdx >= 0 && "Couldn't find a unique operand to extract!");

  const Register DstReg = MI.getOperand(0).getReg();
  const Register SrcReg = MI.getOperand(1).getReg();
  const LLT SrcTy = MRI.getType(SrcReg);

  // Handle scalar sources (e.g., from <1 x i64> lowered to s64).
  // For scalar, only element 0 exists, so broadcast it directly.
  if (!SrcTy.isVector()) {
    if (*UniqOpIdx != 0)
      return false;
    MatchInfo = [=, &MRI](MachineIRBuilder &B) {
      buildBroadcastVector(B, MRI, SrcReg, DstReg);
    };
    return true;
  }

  MatchInfo = [=, &MRI](MachineIRBuilder &B) {
    const LLT DstElemTy = SrcTy.getElementType();
    auto Extr =
        B.buildExtractVectorElementConstant(DstElemTy, SrcReg, *UniqOpIdx);
    buildBroadcastVector(B, MRI, Extr.getReg(0), DstReg);
  };
  return true;
}

/// Check prerequisites to extract a subvector
static bool checkExtractSubvectorPrerequisites(const AIEBaseInstrInfo &TII,
                                               const LLT DstTy,
                                               const LLT SrcTy) {
  const unsigned ScalarRegSize = TII.getScalarRegSize();
  const unsigned VecRegSize = TII.getBasicVecRegSize();
  const unsigned SrcTySize = SrcTy.getSizeInBits();
  const unsigned DstTySize = DstTy.getSizeInBits();

  if (!DstTy.isVector() || !SrcTy.isVector() || SrcTySize < ScalarRegSize ||
      (DstTySize != ScalarRegSize && DstTySize != 2 * ScalarRegSize))
    return false;

  // Currently, we cannot extract vectors for the case when the size of the
  // source vector is less than the basic vector register size (of the target).
  if (SrcTySize < VecRegSize)
    return false;

  //  This should be handled by a separate combine that copies SrcReg to
  //  DstReg.
  return SrcTySize != DstTySize;
}

/// Build G_AIE_EXTRACT_SUBVECTOR
static MachineInstrBuilder
buildExtractSubvector(MachineIRBuilder &B, MachineRegisterInfo &MRI,
                      const AIEBaseInstrInfo &TII, Register DstVecReg,
                      Register SrcVecReg, unsigned SubIdx) {
  const unsigned ScalarRegSize = TII.getScalarRegSize();
  const unsigned BasicVectorBitSize = TII.getBasicVectorBitSize();

  const unsigned Opc = TII.getGenericExtractSubvectorOpcode();
  const LLT SrcTy = MRI.getType(SrcVecReg);
  const LLT DstTy = MRI.getType(DstVecReg);
  const unsigned SrcTySize = SrcTy.getSizeInBits();

  // Natively supported source vector type
  if (SrcTySize == BasicVectorBitSize) {
    auto Cst = B.buildConstant(LLT::scalar(ScalarRegSize), SubIdx);
    return B.buildInstr(Opc, {DstVecReg}, {SrcVecReg, Cst});
  }

  // Source vectors of a non-native size are converted to vectors of the native
  // size
  const unsigned Src1ElemtSize = SrcTy.getElementType().getSizeInBits();
  const unsigned Src1Vec512BitLen = BasicVectorBitSize / Src1ElemtSize;
  const LLT NewSrc1Ty = LLT::fixed_vector(Src1Vec512BitLen, Src1ElemtSize);
  const Register NewSrcReg = MRI.createGenericVirtualRegister(NewSrc1Ty);

  if (SrcTySize < BasicVectorBitSize) {
    const Register ImplicitDef = B.buildUndef(SrcTy).getReg(0);
    SmallVector<Register, 15> ConcatOps = {SrcVecReg};
    unsigned NumImplicitDef = BasicVectorBitSize / SrcTySize - 1;
    while (NumImplicitDef-- > 0) {
      ConcatOps.push_back(ImplicitDef);
    }
    B.buildConcatVectors({NewSrcReg}, ConcatOps);
    auto Cst = B.buildConstant(LLT::scalar(ScalarRegSize), SubIdx);
    return B.buildInstr(Opc, {DstVecReg}, {NewSrcReg, Cst});
  }

  // Source vectors with the size greater than the native source vector size
  const unsigned NumDstElems = DstTy.getNumElements();
  const unsigned NumSrc1Elems = SrcTy.getNumElements();
  const unsigned NumSubVectors = NumSrc1Elems / NumDstElems;
  const unsigned SizeCoefficient = SrcTySize / BasicVectorBitSize;
  const unsigned NumSubVectorsNativeSize = NumSubVectors / SizeCoefficient;
  unsigned NewSubIdx = SubIdx % NumSubVectorsNativeSize;

  SmallVector<Register, 4> SubRegs;
  unsigned NewSrcRegPosition = SubIdx / NumSubVectorsNativeSize;
  for (unsigned I = 0; I < SizeCoefficient; ++I) {
    if (I == NewSrcRegPosition)
      SubRegs.push_back(NewSrcReg);
    else
      SubRegs.push_back(MRI.createGenericVirtualRegister(NewSrc1Ty));
  }

  B.buildUnmerge(SubRegs, SrcVecReg);
  auto Cst = B.buildConstant(LLT::scalar(ScalarRegSize), NewSubIdx);
  return B.buildInstr(Opc, {DstVecReg}, {NewSrcReg, Cst});
}

/// Match something like this:
///  %1:_(<16 x s32>) = COPY $x0
///  %2:_(<16 x s32>) = COPY $x1
///  %0:_(<8 x s32>) = G_SHUFFLE_VECTOR %1(<16 x s32>), %2(<16 x s32>),
///  shufflemask(8, 9, 10, 11, 12, 13, 14, 15)
///  PseudoRET implicit $lr, implicit %0

/// To convert to:
/// %1:_(<16 x s32>) = COPY $x0
/// %2:_(<8 x s32>), %3:_(<8 x s32>) = G_UNMERGE_VALUES %1(<16 x s32>)
/// PseudoRET implicit $lr, implicit %3(<8 x s32>)
static bool matchShuffleToUnmerge(MachineInstr &MI, MachineRegisterInfo &MRI,
                                  BuildFnTy &MatchInfo, unsigned SubIdx,
                                  unsigned NumSubVectors) {
  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();

  // TODO: Select into G_EXTRACT_SUBVECTOR once it is more widely supported
  MatchInfo = [=, &MRI](MachineIRBuilder &B) {
    buildUnmergeVector(B, MRI, DstReg, Src1Reg, NumSubVectors, SubIdx);
  };
  return true;
}

/// Match something like this:
///  %1:_(<16 x s16>) = COPY $wl0
///  %2:_(<16 x s16>) = COPY $wl1
///  %0:_(<4 x s16>) = G_SHUFFLE_VECTOR %1(<16 x s16>), %2(<16 x s16>),
///  shufflemask(4, 5, 6, 7)

/// To convert to:
/// %1:_(<16 x s16>) = COPY $wl0
/// %2:_(s32) = G_CONSTANT i32 1
/// %3:_(<4 x s16>) = G_AIE_EXTRACT_SUBVECTOR %1(<16 x s16>), %2(s32)
/// NOTE: This combine works ONLY for 32- and 64-bit outputs!
static bool matchShuffleToAIEExtractSubvec(
    MachineInstr &MI, MachineRegisterInfo &MRI, const AIEBaseInstrInfo &TII,
    BuildFnTy &MatchInfo, unsigned SubIdx, unsigned NumSubVectors) {
  const unsigned GPRSize = TII.getScalarRegSize();
  const unsigned ExtractSubvecNativeSrcSize = TII.getBasicVectorBitSize();

  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  const unsigned Src1TySize = Src1Ty.getSizeInBits();

  if (!checkExtractSubvectorPrerequisites(TII, DstTy, Src1Ty))
    return false;

  const unsigned Opc = TII.getGenericExtractSubvectorOpcode();

  // Natively supported source vector type
  if (Src1TySize == ExtractSubvecNativeSrcSize) {
    MatchInfo = [=](MachineIRBuilder &B) {
      auto Cst = B.buildConstant(LLT::scalar(GPRSize), SubIdx);
      B.buildInstr(Opc, {DstReg}, {Src1Reg, Cst});
    };

    return true;
  }

  // Source vectors of a non-native size are converted to vectors of the native
  // size
  const unsigned Src1ElmtSize = Src1Ty.getElementType().getSizeInBits();
  const unsigned Src1Vec512BitLen = ExtractSubvecNativeSrcSize / Src1ElmtSize;
  const LLT NewSrc1Ty = LLT::fixed_vector(Src1Vec512BitLen, Src1ElmtSize);
  const Register NewSrcReg = MRI.createGenericVirtualRegister(NewSrc1Ty);

  if (Src1TySize < ExtractSubvecNativeSrcSize) {
    MatchInfo = [=](MachineIRBuilder &B) {
      const Register ImplicitDef = B.buildUndef(Src1Ty).getReg(0);
      SmallVector<Register, 15> ConcatOps = {Src1Reg};
      unsigned NumImplicitDef = ExtractSubvecNativeSrcSize / Src1TySize - 1;
      while (NumImplicitDef-- > 0) {
        ConcatOps.push_back(ImplicitDef);
      }
      B.buildConcatVectors({NewSrcReg}, ConcatOps);
      auto Cst = B.buildConstant(LLT::scalar(GPRSize), SubIdx);
      B.buildInstr(Opc, {DstReg}, {NewSrcReg, Cst});
    };
    return true;
  }

  // Source vectors with the size greater than the native source vector size
  MatchInfo = [=, &MRI](MachineIRBuilder &B) {
    const unsigned SizeCoefficient = Src1TySize / ExtractSubvecNativeSrcSize;
    const unsigned NumSubVectorsNativeSize = NumSubVectors / SizeCoefficient;
    unsigned NewSubIdx = SubIdx % NumSubVectorsNativeSize;

    SmallVector<Register, 4> SubRegs;
    unsigned NewSrcRegPosition = SubIdx / NumSubVectorsNativeSize;
    for (unsigned I = 0; I < SizeCoefficient; ++I) {
      if (I == NewSrcRegPosition)
        SubRegs.push_back(NewSrcReg);
      else
        SubRegs.push_back(MRI.createGenericVirtualRegister(NewSrc1Ty));
    }

    B.buildUnmerge(SubRegs, Src1Reg);
    auto Cst = B.buildConstant(LLT::scalar(GPRSize), NewSubIdx);
    B.buildInstr(Opc, {DstReg}, {NewSrcReg, Cst});
  };
  return true;
}

/// The method does some checks and calls matchShuffleToAIEExtractSubvec and
/// matchShuffleToUnmerge which extract subvectors is possible.
bool llvm::matchShuffleToExtractSubvec(MachineInstr &MI,
                                       MachineRegisterInfo &MRI,
                                       const AIEBaseInstrInfo &TII,
                                       BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_SHUFFLE_VECTOR);

  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  const unsigned Src1TySize = Src1Ty.getSizeInBits();

  if (!DstTy.isVector() || !Src1Ty.isVector())
    return false;

  //  This should be handled by a separate combine that copies Src1Reg to
  //  DstReg.
  if (Src1TySize == DstTy.getSizeInBits())
    return false;

  const unsigned NumDstElems = DstTy.getNumElements();
  const unsigned NumSrc1Elems = Src1Ty.getNumElements();

  // Not an extract pattern
  if (NumSrc1Elems <= NumDstElems)
    return false;

  // Unlikely to select into a subregister copy
  if (NumSrc1Elems % NumDstElems != 0)
    return false;

  if (MaskMatch::isMaskWithAllUndefs(Mask))
    return false;

  const unsigned NumSubVectors = NumSrc1Elems / NumDstElems;
  auto GetSubvecExtractIdx = [=, &Mask]() -> std::optional<unsigned> {
    for (unsigned SubVecIdx = 0; SubVecIdx < NumSubVectors; ++SubVecIdx) {
      MaskMatch SequentialMask{/*Height*/ SubVecIdx * NumDstElems};
      if (SequentialMask.isValidMask(Mask))
        return SubVecIdx;
    }

    return std::nullopt;
  };

  std::optional<unsigned> SubvecExtractIdx = GetSubvecExtractIdx();

  // Not an extract pattern
  if (!SubvecExtractIdx)
    return false;

  if (matchShuffleToAIEExtractSubvec(MI, MRI, TII, MatchInfo,
                                     SubvecExtractIdx.value(), NumSubVectors))
    return true;
  if (matchShuffleToUnmerge(MI, MRI, MatchInfo, SubvecExtractIdx.value(),
                            NumSubVectors))
    return true;

  return false;
}

/// Match something like this:
///  %1:_(<8 x s32>) = COPY $wl0
///  %2:_(<8 x s32>) = COPY $wl1
///  %0:_(<8 x s32>) = G_SHUFFLE_VECTOR %1(<8 x s32>), %2(<8 x s32>),
///  shufflemask(4, 5, 6, 7, 4, 5, 6, 7)

/// To convert to:
/// %1:_(<8 x s32>) = COPY $wl0
/// %2:_(s32) = G_CONSTANT i32 1
/// %3:_(<4 x s32>) = G_AIE_EXTRACT_SUBVECTOR %1(<8 x s32>), %2(s32)
/// %4:_(<16 x s32>) = G_AIE_BROADCAST_VECTOR %3(<4 x s32>)
/// %5:_(<8 x s32>) = G_AIE_UNPAD_VECTOR %4(<16 x s32>)

// If the subvector cannot be extracted and broadcasted given the target
// constraints, an Unmerge and Concat are used instead.
static bool matchShuffleToSubvecBroadcast(MachineInstr &MI,
                                          MachineRegisterInfo &MRI,
                                          const AIEBaseInstrInfo &TII,
                                          BuildFnTy &MatchInfo) {
  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);

  // Can't extract a subvector from a scalar source.
  if (!Src1Ty.isVector() || !DstTy.isVector())
    return false;

  const unsigned NumDstElems = DstTy.getNumElements();
  const unsigned NumSrcElems = Src1Ty.getNumElements();
  const unsigned MaskSize = Mask.size();

  if (NumDstElems != MaskSize)
    return false;

  auto CheckSplatMask = [=]() -> std::optional<std::pair<int, unsigned>> {
    // Find the splat mask pattern, start with length 2 and then power of 2.
    for (unsigned SplatMaskLen = 2;
         SplatMaskLen <= NumSrcElems && SplatMaskLen <= MaskSize;
         SplatMaskLen *= 2) {
      if (Mask[0] != -1 && Mask[0] % SplatMaskLen != 0)
        return std::nullopt;

      // Get Height (start value)
      std::optional<unsigned> Height =
          MaskMatch::getHeight(Mask, /*Period*/ SplatMaskLen);
      if (!Height)
        return std::nullopt;

      // Check the mask
      MaskMatch SequentialPeriodicMask{/*Height*/ Height.value(),
                                       /*Period*/ SplatMaskLen};
      if (SequentialPeriodicMask.isValidMask(Mask))
        return std::make_pair(Height.value(), SplatMaskLen);
    }
    return std::nullopt;
  };

  auto SplatMaskData = CheckSplatMask();
  if (!SplatMaskData)
    return false;

  const int SplatMaskStart = SplatMaskData->first;
  const unsigned SplatMaskLen = SplatMaskData->second;

  const LLT ElemTy = Src1Ty.getElementType();
  const LLT DstSubvecType =
      LLT::fixed_vector(SplatMaskLen, ElemTy.getSizeInBits());
  const unsigned SubIdx = SplatMaskStart / SplatMaskLen;
  Register ExtractSubvecDstReg =
      MRI.createGenericVirtualRegister(DstSubvecType);

  // Check whether we can extract the subvector
  const bool CanExtractSubvector =
      checkExtractSubvectorPrerequisites(TII, DstSubvecType, Src1Ty);
  if (CanExtractSubvector) {
    MatchInfo = [=, &MRI, &TII](MachineIRBuilder &B) {
      auto Extract = buildExtractSubvector(B, MRI, TII, ExtractSubvecDstReg,
                                           Src1Reg, SubIdx);
      buildBroadcastVector(B, MRI, Extract.getReg(0), DstReg);
    };
    return true;
  }

  // If we cannot extract the subvector, we try to apply UNMERGE + CONCAT
  const unsigned NumSubVectors = NumSrcElems / SplatMaskLen;

  const unsigned SubVecSize = Src1Ty.getSizeInBits() / NumSubVectors;
  // FIXME: We don't have unmerge/concat support of 64-bit and smaller.
  if (SubVecSize < 128)
    return false;

  // Don't try to unmerge when we have just one subvector.
  // We can overcome with a copy, but other combiners can do a
  // better job for this case.
  if (NumSubVectors > 1 && NumDstElems > SplatMaskLen) {
    MatchInfo = [=, &MRI](MachineIRBuilder &B) {
      buildUnmergeVector(B, MRI, ExtractSubvecDstReg, Src1Reg, NumSubVectors,
                         SubIdx);

      const SmallVector<Register, 2> ConcatOps(NumDstElems / SplatMaskLen,
                                               ExtractSubvecDstReg);
      B.buildConcatVectors({DstReg}, ConcatOps);
    };
    return true;
  }

  return false;
}

/// Match something like this:
///  %1:_(<2 x s32>) = COPY $l0
///  %2:_(<2 x s32>) = G_IMPLICIT_DEF
///  %0:_(<16 x s32>) = G_SHUFFLE_VECTOR %1(<2 x s32>), %2,
///  shufflemask(0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1)

/// To convert to:
///  %1:_(<2 x s32>) = COPY $l0
///  %2:_(<2 x s32>) = G_IMPLICIT_DEF
///  %0:_(<16 x s32>) = G_AIE_BROADCAST_VECTOR %1(<2 x s32>)
static bool matchShuffleToVecBroadcast(MachineInstr &MI,
                                       MachineRegisterInfo &MRI,
                                       const AIEBaseInstrInfo &TII,
                                       BuildFnTy &MatchInfo) {
  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  if (Src1Ty.getSizeInBits() != 64 && Src1Ty.getSizeInBits() != 32)
    return false;
  // Destination must be a vector.
  if (!DstTy.isVector())
    return false;
  const unsigned NumDstElems = DstTy.getNumElements();
  // Handle scalar sources (e.g., s32/s64 from <1 x i32>/<1 x i64>).
  // Treat scalar as 1-element for mask matching.
  const unsigned NumSrcElems = Src1Ty.isVector() ? Src1Ty.getNumElements() : 1;
  if (NumDstElems != Mask.size()) {
    return false;
  }

  // Check the mask
  MaskMatch SequentialPeriodicMask{/*Height*/ 0,
                                   /*Period*/ NumSrcElems};
  if (!SequentialPeriodicMask.isValidMask(Mask))
    return false;

  MatchInfo = [=, &MRI](MachineIRBuilder &B) {
    buildBroadcastVector(B, MRI, Src1Reg, DstReg);
  };

  return true;
}

// If the subvector cannot be extracted and broadcasted given the target
// constraints, an Unmerge and Concat are used instead, such as in
// matchShuffleToSubvecBroadcast.
bool llvm::matchShuffleToBroadcast(MachineInstr &MI, MachineRegisterInfo &MRI,
                                   const AIEBaseInstrInfo &TII,
                                   BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_SHUFFLE_VECTOR);

  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  if (MaskMatch::isMaskWithAllUndefs(Mask))
    return false;

  if (matchShuffleToVecBroadcast(MI, MRI, TII, MatchInfo))
    return true;
  if (matchShuffleToVecEltBroadcast(MI, MRI, TII, MatchInfo))
    return true;
  if (matchShuffleToSubvecBroadcast(MI, MRI, TII, MatchInfo))
    return true;
  return false;
}

/// Match something like this:
///  %1:_(<2 x s32>) = COPY $x0
///  %2:_(<2 x s32>) = G_IMPLICIT_DEF
///  %0:_(<16 x s32>) = G_SHUFFLE_VECTOR %1(<16 x s32>), %2(<16 x s32>),
///  shufflemask(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)

/// To convert to:
///  %0:_(<16 x s32>) = COPY $x0
bool llvm::matchShuffleToCopy(MachineInstr &MI, MachineRegisterInfo &MRI,
                              BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_SHUFFLE_VECTOR);

  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  if (DstTy != Src1Ty)
    return false;

  const unsigned NumSrcElems = Src1Ty.isVector() ? Src1Ty.getNumElements() : 1;
  if (Mask.size() != NumSrcElems)
    return false;

  if (MaskMatch::isMaskWithAllUndefs(Mask))
    return false;

  // Check that the mask is sequential
  MaskMatch SequentialMask{/*Height*/ 0};
  if (!SequentialMask.isValidMask(Mask))
    return false;

  MatchInfo = [=](MachineIRBuilder &B) { B.buildCopy(DstReg, Src1Reg); };

  return true;
}

/// Match something like this:
///  %2:_(<16 x s32>) = G_SHUFFLE_VECTOR %0(<16 x s32>), %1(<16 x s32>),
///  shufflemask(16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

/// To convert to:
//  %4:_(s32) = G_CONSTANT i32 0
//  %5:_(s32) = G_EXTRACT_VECTOR_ELT %0(<16 x s32>), %4(s32)
//  %3:_(<16 x s32>) = G_AIE_BROADCAST_VECTOR %5(s32)ab
//  %6:_(s32) = G_EXTRACT_VECTOR_ELT %1(<16 x s32>), %4(s32)
//  %2:_(<16 x s32>) = G_INSERT_VECTOR_ELT %3, %6(s32), %4(s32)
bool llvm::matchShuffleToExtractInsertEltToBroadcast(MachineInstr &MI,
                                                     MachineRegisterInfo &MRI,
                                                     BuildFnTy &MatchInfo) {

  assert(MI.getOpcode() == TargetOpcode::G_SHUFFLE_VECTOR);

  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  const Register Src2Reg = MI.getOperand(2).getReg();
  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  if (DstTy != Src1Ty)
    return false;

  if (!DstTy.isVector() || !Src1Ty.isVector())
    return false;

  if (DstTy.getSizeInBits() < 128)
    return false;

  const unsigned NumSrcElems = Src1Ty.getNumElements();
  const LLT DstElemTy = MRI.getType(Src1Reg).getElementType();

  if (Mask.size() != NumSrcElems)
    return false;

  if (MaskMatch::isMaskWithAllUndefs(Mask))
    return false;

  unsigned MinFrequency;
  const Triple &T = MI.getMF()->getTarget().getTargetTriple();
  if (T.isAIE2P() || T.isAIE2PS())
    // The scalarization of G_SHUFFLE_VECTOR in the legalizer is more beneficial
    // if there are more exceptions than NumSrcElems / 2 as AIE2P's VINSERT
    // instrutions require a move to a register used for the index unlike VPUSH.
    MinFrequency = (ShuffleMaxNumInsertions != 0) ? ShuffleMaxNumInsertions
                                                  : NumSrcElems / 2;
  else
    llvm_unreachable("MinFrequency unimplemented for target.");

  std::optional<FrequentIndexResult> FrequentIdxResult =
      MaskMatch::getFrequentIndexResult(Mask, MinFrequency);

  if (!FrequentIdxResult)
    return false;

  unsigned FrequentIdx = FrequentIdxResult->FrequentIdx;
  unsigned NonMatchingCount = FrequentIdxResult->NonMatchingCount;

  // This is a pure broadcast pattern. Should be handled by
  // matchShuffleToVecEltBroadcast combine
  if (NonMatchingCount == 0)
    return false;

  int BcstValue = Mask[FrequentIdx];

  MatchInfo = [=, &MRI](MachineIRBuilder &B) {
    Register BroadcastVecReg = MRI.createGenericVirtualRegister(Src1Ty);
    Register VecToExtract = BcstValue < (int)NumSrcElems ? Src1Reg : Src2Reg;
    auto Extr = B.buildExtractVectorElementConstant(DstElemTy, VecToExtract,
                                                    BcstValue % NumSrcElems);
    buildBroadcastVector(B, MRI, Extr.getReg(0), BroadcastVecReg);

    Register InsertSrc = BroadcastVecReg;
    Register InsertDst;

    unsigned InsertionCount = 0;
    for (unsigned Idx = 0; Idx < Mask.size(); ++Idx) {

      if (Mask[Idx] == BcstValue)
        continue;

      if (Mask[Idx] == -1)
        continue;

      Register VecToExtract = Mask[Idx] < (int)NumSrcElems ? Src1Reg : Src2Reg;

      int ExtractIdx = Mask[Idx] % NumSrcElems;
      auto ExtrElt = B.buildExtractVectorElementConstant(
          DstElemTy, VecToExtract, ExtractIdx);

      auto NonMatchingIdxReg = B.buildConstant(LLT::scalar(32), Idx);

      InsertDst = (InsertionCount == NonMatchingCount - 1)
                      ? DstReg
                      : MRI.createGenericVirtualRegister(Src1Ty);

      B.buildInsertVectorElement(InsertDst, InsertSrc, ExtrElt,
                                 NonMatchingIdxReg);
      InsertSrc = InsertDst;
      InsertionCount++;
    }
  };

  return true;
}

/// Match shuffle vectors that are mostly sequential (identity or extract
/// subvector) with a few element insertions from either source.
///
/// Handles two cases:
/// 1. Same-size shuffles: Dst and Src have the same type, mask is mostly
///    identity with exceptions inserted from Src1 or Src2.
/// 2. Shrinking shuffles: Dst is smaller than Src (e.g., 16->8 elements),
///    mask extracts a prefix subvector with exceptions from Src2.
///
/// Same-size example:
///   %2:_(<32 x s16>) = G_SHUFFLE_VECTOR %0, %1, shufflemask(0, 1, ..., 32,
///   ...)
/// Converts to:
///   %elt = G_EXTRACT_VECTOR_ELT %1, 0
///   %2 = G_INSERT_VECTOR_ELT %0, %elt, 8
///
/// Shrinking example:
///   %2:_(<8 x s32>) = G_SHUFFLE_VECTOR %0(<16 x s32>), %1,
///                       shufflemask(0, 16, undef, undef, 4, 5, 6, 7)
/// Converts to:
///   %unpad = G_AIE_UNPAD_VECTOR %0(<16 x s32>)
///   %elt = G_EXTRACT_VECTOR_ELT %1, 0
///   %2 = G_INSERT_VECTOR_ELT %unpad, %elt, 1
bool llvm::matchMostlySequentialShuffleWithInsertions(MachineInstr &MI,
                                                      MachineRegisterInfo &MRI,
                                                      BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_SHUFFLE_VECTOR);

  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  const Register Src2Reg = MI.getOperand(2).getReg();
  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);

  if (!DstTy.isVector() || !Src1Ty.isVector())
    return false;

  const unsigned NumDstElems = DstTy.getNumElements();
  const unsigned NumSrcElems = Src1Ty.getNumElements();

  // Element types must match
  if (DstTy.getElementType() != Src1Ty.getElementType())
    return false;

  // No growing shuffles; shrinking requires divisibility
  if (NumSrcElems % NumDstElems != 0)
    return false;

  const bool IsShrinking = NumSrcElems > NumDstElems;

  const LLT ElemTy = Src1Ty.getElementType();

  unsigned MaxNumInsertions;
  const Triple &T = MI.getMF()->getTarget().getTargetTriple();
  if (T.isAIE2P() || T.isAIE2PS())
    // The scalarization of G_SHUFFLE_VECTOR in the legalizer is more beneficial
    // if there are more exceptions than NumSrcElems / 2 as AIE2P's VINSERT
    // instructions require a move to a register used for the index unlike
    // VPUSH.
    MaxNumInsertions = (ShuffleMaxNumInsertions != 0) ? ShuffleMaxNumInsertions
                                                      : NumSrcElems / 2;
  else
    llvm_unreachable(
        "MaxNumInsertions unimplemented for target. Does the target's Insert "
        "instruction take immediate indices or does it require a register for "
        "the index?");

  if (Mask.size() != NumDstElems)
    return false;

  if (MaskMatch::isMaskWithAllUndefs(Mask))
    return false;

  // Check that the mask is sequential
  MaskMatch SequentialMask{/*Height*/ 0};
  ShuffleMaskValidity SequentialMaskValidity =
      SequentialMask.getShuffleMaskValidity(Mask);
  bool IsValid = SequentialMaskValidity.IsValid;

  // This is a Copy pattern and will be handled by matchShuffleToCopy
  if (IsValid)
    return false;

  SmallVector<unsigned, 4> Exceptions = SequentialMaskValidity.MaskExceptions;
  assert(!Exceptions.empty());

  if (Exceptions.size() >= MaxNumInsertions)
    return false;

  MatchInfo = [=, &MRI](MachineIRBuilder &B) {
    // For shrinking shuffles, first extract the subvector using UNPAD
    Register InsertSrc;
    if (IsShrinking) {
      InsertSrc = MRI.createGenericVirtualRegister(DstTy);
      const AIEBaseInstrInfo &TII = getAIETII(B);
      B.buildInstr(TII.getGenericUnpadVectorOpcode(), {InsertSrc}, {Src1Reg});
    } else {
      InsertSrc = Src1Reg;
    }

    Register InsertDst;
    for (const unsigned ExceptionIdx : Exceptions) {
      Register VecToExtract =
          Mask[ExceptionIdx] < (int)NumSrcElems ? Src1Reg : Src2Reg;

      int ExtractIdx = Mask[ExceptionIdx] % NumSrcElems;
      auto ExtrElt =
          B.buildExtractVectorElementConstant(ElemTy, VecToExtract, ExtractIdx);

      auto ExceptionIdxReg = B.buildConstant(LLT::scalar(32), ExceptionIdx);

      InsertDst = (ExceptionIdx == Exceptions.back())
                      ? DstReg
                      : MRI.createGenericVirtualRegister(DstTy);

      B.buildInsertVectorElement(InsertDst, InsertSrc, ExtrElt,
                                 ExceptionIdxReg);
      InsertSrc = InsertDst;
    }
  };

  return true;
}

bool llvm::matchShuffleToConcatExtractedSubvectors(MachineInstr &MI,
                                                   MachineRegisterInfo &MRI,
                                                   const AIEBaseInstrInfo &TII,
                                                   BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_SHUFFLE_VECTOR);

  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  const Register Src2Reg = MI.getOperand(2).getReg();
  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  if (!DstTy.isVector() || !Src1Ty.isVector())
    return false;

  const LLT ElemTy = Src1Ty.getElementType();
  unsigned ElemSize = ElemTy.getSizeInBits();
  const unsigned NumSrc1Elems = Src1Ty.getNumElements();
  const unsigned NumDstElems = DstTy.getNumElements();
  if (NumDstElems != NumSrc1Elems)
    return false;

  // Get Height (start value)
  std::optional<unsigned> Height = MaskMatch::getHeight(Mask, /*Period*/ 0);
  if (!Height)
    return false;

  // Not extractable
  if (!isPowerOfTwoOrZero((*Height) % NumSrc1Elems))
    return false;

  MaskMatch SequentialMask{/*Height*/ *Height};
  ShuffleMaskValidity SequentialMaskValidity =
      SequentialMask.getShuffleMaskValidity(Mask);
  bool IsValid = SequentialMaskValidity.IsValid;

  // Should be handled by a separate combine shuffleToCopy combine
  if (IsValid)
    return false;

  auto GetSubvecExtractIndices =
      [=, &Mask,
       &SequentialMaskValidity]() -> std::optional<SmallVector<unsigned>> {
    SmallVector<unsigned> SubvecExtractIndices = {0};
    unsigned CurrentMaskPos = 0;
    unsigned CurrentHeight = *Height;
    SmallVector<unsigned, 4> CurrentExceptions =
        SequentialMaskValidity.MaskExceptions;
    ArrayRef<int> CurrentMask = Mask;

    // Check that the exceptions are contiguous or form one block. Since undef
    // values are not counted as exceptions, we have to take them into account
    // in the condition below
    auto ContiguousExceptions = [&CurrentExceptions, &CurrentMask]() {
      return (CurrentExceptions.size() +
              getNumMaskUndefs(CurrentMask, CurrentExceptions[0])) ==
             (CurrentMask.size() - CurrentExceptions[0]);
    };

    while (ContiguousExceptions()) {
      // The Mask has nothing but exceptions
      if (CurrentExceptions.size() == CurrentMask.size()) {
        return std::nullopt;
      }
      SubvecExtractIndices.push_back(CurrentMaskPos + CurrentExceptions[0]);
      CurrentMaskPos += CurrentExceptions[0];

      // Extract the submask at the current exception indices and update the
      // current mask with it
      ArrayRef<int> SubMask =
          CurrentMask.slice(CurrentExceptions[0], CurrentExceptions.size());
      CurrentMask = SubMask;
      CurrentHeight = *MaskMatch::getHeight(CurrentMask, /*Period*/ 0);

      if (!isPowerOfTwoOrZero((CurrentHeight) % NumSrc1Elems))
        return std::nullopt;

      MaskMatch SequentialMask{/*Height*/ CurrentHeight};
      // Check the sequential validity of the new Submask
      ShuffleMaskValidity SequentialMaskValidity =
          SequentialMask.getShuffleMaskValidity(CurrentMask);

      if (SequentialMaskValidity.IsValid)
        return SubvecExtractIndices;

      // Update CurrentExceptions
      CurrentExceptions = SequentialMaskValidity.MaskExceptions;
    }

    // Non contiguous exceptions
    return std::nullopt;
  };

  auto SubvecExtractIndices = GetSubvecExtractIndices();

  // Not a valid pattern
  if (!SubvecExtractIndices)
    return false;

  // Since we can only unmerge and concat vectors of the same size, equalize the
  // extract indices, by inserting additional extract indices if needed. E.g.
  // for the shufflemask:
  // (16, 17, 18, 19, 20, 21, 22, 23, 0, 1, 2, 3, undef, undef, undef, undef)
  // We get SubvecExtractIndices = [0,8,12] which equalized becomes [0,4,8,12]
  auto EqualizeExtractIndices = [=, &SubvecExtractIndices]() {
    if (SubvecExtractIndices->size() < 2)
      return *SubvecExtractIndices;
    // Calculate the smallest difference between consecutive indices
    unsigned MinDiff = Mask.size() - SubvecExtractIndices->back();
    for (unsigned I = 1; I < SubvecExtractIndices->size(); ++I) {
      unsigned Diff =
          (*SubvecExtractIndices)[I] - (*SubvecExtractIndices)[I - 1];
      MinDiff = std::min(MinDiff, Diff);
    }
    // Generate the new indices
    SmallVector<unsigned> UsableSubvecExtractIndices;
    unsigned StartIndex = (*SubvecExtractIndices)[0];
    for (unsigned I = StartIndex; I < Mask.size(); I += MinDiff) {
      UsableSubvecExtractIndices.push_back(I);
    }
    return UsableSubvecExtractIndices;
  };

  SmallVector<unsigned> EqualizedExtractIndices = EqualizeExtractIndices();

  // Return the values in the mask for the given indices. For undef, return the
  // expected value of the provided mask pattern.
  auto getSubMaskValuesAtIndices = [&EqualizedExtractIndices, &Mask,
                                    &SequentialMask]() {
    unsigned NumSubVectors = EqualizedExtractIndices.size();
    SmallVector<unsigned, 4> SubMask;
    for (unsigned Idx : EqualizedExtractIndices) {
      unsigned ExpectedMaskValue =
          SequentialMask.getMaskValue(Idx % NumSubVectors);
      SubMask.push_back(Mask[Idx] == -1 ? ExpectedMaskValue : Mask[Idx]);
    }
    return SubMask;
  };

  SmallVector<unsigned, 4> IndicesToExtract = getSubMaskValuesAtIndices();
  unsigned NumSubVectors = IndicesToExtract.size();

  // The combine is not any better than scalarization
  if (NumSubVectors == NumDstElems)
    return false;

  // Subvectors cannot be extracted
  if (!isPowerOfTwoOrZero(NumSubVectors)) {
    return false;
  }

  unsigned SubVecNumElts = NumDstElems / NumSubVectors;
  unsigned SubVecSize = SubVecNumElts * ElemSize;
  // Check whether we can extract the subvector
  LLT SubvecType = LLT::fixed_vector(SubVecNumElts, ElemSize);
  const bool CanExtractSubvectors =
      checkExtractSubvectorPrerequisites(TII, SubvecType, Src1Ty);

  // FIXME: Enable once we have concat support of 64-bits or smaller.
  if (SubVecSize <= 64)
    return false;

  // Do not combine if any of the subvectors is not extractable with the
  // determined NumSubVectors.
  // FIXME: Dynamically adjust the NumSubVecs and thus the
  // SubVecNumElts to have all subvectors extractable.
  if (!all_of(IndicesToExtract, [=](const unsigned ExtractIdx) {
        return !(ExtractIdx % SubVecNumElts);
      })) {
    return false;
  }

  MatchInfo = [=, &MRI, &TII](MachineIRBuilder &B) {
    SmallVector<Register> ExtractedSubvectors;
    for (unsigned Idx : IndicesToExtract) {
      Register ExtractVec = Idx < NumSrc1Elems ? Src1Reg : Src2Reg;
      unsigned ExtractIdx = Idx % NumSrc1Elems;
      Register ExtractedSubVec = MRI.createGenericVirtualRegister(SubvecType);
      if (CanExtractSubvectors) {
        buildExtractSubvector(B, MRI, TII, ExtractedSubVec, ExtractVec,
                              ExtractIdx);
      } else {
        unsigned UnmergeIdx = ExtractIdx / SubVecNumElts;
        assert(!(ExtractIdx % SubVecNumElts));
        buildUnmergeVector(B, MRI, ExtractedSubVec, ExtractVec, NumSubVectors,
                           UnmergeIdx);
      }
      ExtractedSubvectors.push_back(ExtractedSubVec);
    }
    B.buildConcatVectors({DstReg}, ExtractedSubvectors);
  };

  return true;
}

/// Match something like this:
///  %1:_(s32) = COPY $r0
///  %2:_(<16 x s32>) = COPY $x0
///  %3:_(<16 x s32>) = G_AIE_BROADCAST_VECTOR %1:_(s32)
///  %0:_(<16 x s32>) = G_SHUFFLE_VECTOR %3(<16 x s32>), %2(<16 x s32>),
///  shufflemask(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
/// To convert to:
///  %0:_(<16 x s32>) = G_AIE_BROADCAST_VECTOR %1(s32)
bool llvm::matchShuffleBcstToCopy(MachineInstr &MI, MachineRegisterInfo &MRI,
                                  const TargetInstrInfo &TII,
                                  BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_SHUFFLE_VECTOR);

  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  const Register Src2Reg = MI.getOperand(2).getReg();
  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  if (DstTy != Src1Ty)
    return false;

  ArrayRef<int> Mask = MI.getOperand(3).getShuffleMask();
  const AIEBaseInstrInfo &AIETII = (const AIEBaseInstrInfo &)TII;
  MachineInstr *Src1Vec = MRI.getVRegDef(Src1Reg);
  MachineInstr *Src2Vec = MRI.getVRegDef(Src2Reg);
  unsigned BcstOpcode = AIETII.getGenericBroadcastVectorOpcode();
  bool IsSrc1Bcst = (Src1Vec->getOpcode() == BcstOpcode);
  bool IsSrc2Bcst = (Src2Vec->getOpcode() == BcstOpcode);

  // Check if the first or second source is defined by a Broadcast.
  if (!IsSrc1Bcst && !IsSrc2Bcst)
    return false;

  if (!DstTy.isVector() || !Src1Ty.isVector())
    return false;

  const unsigned NumSrcElems = Src1Ty.getNumElements();
  if (Mask.size() != NumSrcElems)
    return false;

  // Determine the valid mask range
  const int MinSrc1Value = 0, MaxSrc1Value = NumSrcElems - 1;
  const int MinSrc2Value = NumSrcElems, MaxSrc2Value = 2 * NumSrcElems - 1;
  const bool IsValidSrc1Mask =
      IsSrc1Bcst ? MaskMatch(0).isMaskWithinRangeOrUndef(Mask, MinSrc1Value,
                                                         MaxSrc1Value)
                 : false;
  const bool IsValidSrc2Mask =
      IsSrc2Bcst ? MaskMatch(0).isMaskWithinRangeOrUndef(Mask, MinSrc2Value,
                                                         MaxSrc2Value)
                 : false;
  Register SrcReg;
  if (IsSrc1Bcst && IsValidSrc1Mask)
    SrcReg = Src1Reg;
  else if (IsSrc2Bcst && IsValidSrc2Mask)
    SrcReg = Src2Reg;
  else
    return false;

  MatchInfo = [=](MachineIRBuilder &B) { B.buildCopy(DstReg, SrcReg); };

  return true;
}

/// Match:
/// %35:_(s32) = G_CONSTANT i32 0
/// %38:_(s32) = G_CONSTANT i32 1
/// %34:_(s32) = G_EXTRACT_VECTOR_ELT %33(<32 x s32>), %35(s32)
/// %37:_(s32) = G_EXTRACT_VECTOR_ELT %33(<32 x s32>), %38(s32)
/// To convert to:
/// %35:_(s32) = G_CONSTANT i32 0
/// %50:_(<16 x s64>) = G_BITCAST %33(<32 x s32>)
/// %60:_(s64) = G_EXTRACT_VECTOR_ELT %50(<16 x s64>), %35(s32)
/// %34:_(s32), %37:_(s32) = G_UNMERGE_VALUES %60(s64)
bool llvm::matchPairedExtracts(MachineInstr &MI, MachineRegisterInfo &MRI,
                               CombinerHelper &Helper,
                               const TargetInstrInfo &TII,
                               GISelChangeObserver &Observer,
                               BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_EXTRACT_VECTOR_ELT);
  const AIEBaseInstrInfo &AIETII = (const AIEBaseInstrInfo &)TII;
  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  const LLT DstTy = MRI.getType(DstReg);
  const LLT Src1Ty = MRI.getType(Src1Reg);
  const LLT S64 = LLT::scalar(64);
  const LLT S32 = LLT::scalar(32);

  if (Src1Ty.getSizeInBits() < AIETII.getBasicVectorBitSize())
    return false;

  if (DstTy != S32)
    return false;

  auto GetConstantIndex =
      [&](const MachineInstr &ExtractMI) -> std::optional<unsigned> {
    const Register IndexReg = ExtractMI.getOperand(2).getReg();
    if (auto Index = getIConstantVRegValWithLookThrough(IndexReg, MRI))
      return Index->Value.getZExtValue();
    return std::nullopt;
  };

  auto IndexCst = GetConstantIndex(MI);

  if (!IndexCst)
    return false;

  bool isEven = *IndexCst % 2 == 0;

  auto FindPairedExtract = [&](unsigned PairedIndex) -> MachineInstr * {
    for (auto &UseMI : MRI.use_nodbg_instructions(Src1Reg)) {
      if (UseMI.getOpcode() != TargetOpcode::G_EXTRACT_VECTOR_ELT)
        continue;
      if (UseMI.getParent() != MI.getParent())
        continue;
      // This helps to simplify the code in relation to
      // instruction build point. If we miss some opportunity now,
      // we will catch in the next combiner run.
      if (!Helper.dominates(MI, UseMI))
        continue;
      const Register UseDstReg = UseMI.getOperand(0).getReg();
      const LLT UseDstTy = MRI.getType(UseDstReg);
      if (UseDstTy != S32)
        continue;
      auto NextIndexCst = GetConstantIndex(UseMI);
      if (!NextIndexCst)
        continue;
      if (*NextIndexCst == PairedIndex)
        return &UseMI;
    }
    return nullptr;
  };

  const unsigned NextIndex = isEven ? *IndexCst + 1 : *IndexCst - 1;
  MachineInstr *NextExtractMI = FindPairedExtract(NextIndex);
  if (!NextExtractMI)
    return false;

  MatchInfo = [=, &MRI, &Observer](MachineIRBuilder &B) {
    const Register NewExtendedDstReg = MRI.createGenericVirtualRegister(S64);
    const Register NewIdxReg = B.buildConstant(S32, *IndexCst / 2).getReg(0);

    // Bitcast from <2n x s32> to <n x s64>
    const LLT NewType = LLT::fixed_vector(Src1Ty.getNumElements() / 2, S64);
    const Register CastedVecReg = B.buildBitcast(NewType, Src1Reg).getReg(0);

    const Register NextDstReg = NextExtractMI->getOperand(0).getReg();
    // As we need to replace 2 instructions, make the second one dead,
    // so DCE will remove it. Another practical effect is, if we don't do
    // this, we will have a SSA violation because of the last buildCopy.
    const Register NewDeadReg = MRI.cloneVirtualRegister(NextDstReg);
    Observer.changingInstr(*NextExtractMI);
    NextExtractMI->getOperand(0).setReg(NewDeadReg);
    Observer.changedInstr(*NextExtractMI);

    B.buildExtractVectorElement(NewExtendedDstReg, CastedVecReg, NewIdxReg);
    auto Split = B.buildUnmerge(S32, NewExtendedDstReg);
    const unsigned FirstOpIndex = isEven ? 0 : 1;
    B.buildCopy(DstReg, Split->getOperand(FirstOpIndex).getReg());
    B.buildCopy(NextDstReg, Split->getOperand(FirstOpIndex ^ 1).getReg());
  };

  return true;
}

bool llvm::matchBroadcastToShl(MachineInstr &MI, MachineRegisterInfo &MRI,
                               const AIEBaseInstrInfo &TII,
                               BuildFnTy &MatchInfo) {

  assert(MI.getOpcode() == TargetOpcode::G_SHL);

  const Register DstReg = MI.getOperand(0).getReg();
  const LLT DstType = MRI.getType(DstReg);

  if (!DstType.isFixedVector())
    return false;

  const Register SrcReg1 = MI.getOperand(1).getReg();
  const Register SrcReg2 = MI.getOperand(2).getReg();

  const MachineInstr *DefAmtMI = MRI.getVRegDef(SrcReg2);

  if (DefAmtMI->getOpcode() != TII.getGenericBroadcastVectorOpcode())
    return false;

  auto CstSrc =
      getIConstantVRegValWithLookThrough(DefAmtMI->getOperand(1).getReg(), MRI);

  if (!CstSrc)
    return false;

  const int ShiftAmount = CstSrc->Value.getZExtValue();
  assert(ShiftAmount >= 0 && "Invalid shift amount");

  MatchInfo = [=, &MRI](MachineIRBuilder &B) {
    Register CurAddReg = SrcReg1;
    for (int NumAdds = 0; NumAdds < ShiftAmount; NumAdds++) {
      Register AddReg = MRI.cloneVirtualRegister(SrcReg1);
      B.buildInstr(TargetOpcode::G_ADD)
          .addDef(AddReg)
          .addReg(CurAddReg)
          .addReg(CurAddReg);
      CurAddReg = AddReg;
    }

    B.buildCopy(DstReg, CurAddReg);
  };

  return true;
}

// Can we narrow all operands of a PHI node without losing precision?
// We can narrow losing precision, provided that all further users of this
// PHI node are s20 users (in this case we are not interested in the higher
// bits).
static bool
canBeNarrowedWithoutLoss(MachineInstr &Phi,
                         SmallPtrSet<MachineInstr *, 4> &VisitedInstrs,
                         MachineRegisterInfo &MRI) {
  if (VisitedInstrs.contains(&Phi))
    return true;
  VisitedInstrs.insert(&Phi);
  for (unsigned OpNum = 1; OpNum < Phi.getNumOperands(); OpNum += 2) {
    const Register PhiRegIn = Phi.getOperand(OpNum).getReg();
    assert(MRI.getType(PhiRegIn).getSizeInBits() == 32 && "Mixed Phi node");

    MachineInstr *DefMI = MRI.getVRegDef(PhiRegIn);
    const unsigned Opcode = DefMI->getOpcode();
    if (Opcode == TargetOpcode::G_PHI) {
      if (!canBeNarrowedWithoutLoss(*DefMI, VisitedInstrs, MRI))
        return false;
    } else if (Opcode == TargetOpcode::G_CONSTANT) {
      if (!isIntN(20, DefMI->getOperand(1).getCImm()->getSExtValue()))
        return false;
    } else if (Opcode == TargetOpcode::G_ZEXT) {
      if (MRI.getType(DefMI->getOperand(1).getReg()) != LLT::scalar(20))
        return false;
    } else if (Opcode != TargetOpcode::G_IMPLICIT_DEF) {
      return false;
    }
  }
  return true;
}

struct PhiUsageAnalysisResult {
  bool MayNeedFullPrecision = false;
  bool IsUsedByS20 = false;

  void operator|=(const PhiUsageAnalysisResult &Other) {
    IsUsedByS20 |= Other.IsUsedByS20;
    MayNeedFullPrecision |= Other.MayNeedFullPrecision;
  }
};

// Do we have any s20 user of this PHI node? Also, do we have users that
// may require all 32 bits of the representation (extra users)?
// We don't need to explicitly know the end users of the s20 value, we
// can just look to the truncations.
static PhiUsageAnalysisResult
isUsedByAnyS20Instruction(MachineInstr &Phi,
                          SmallPtrSet<MachineInstr *, 4> &VisitedInstrs,
                          MachineRegisterInfo &MRI) {

  PhiUsageAnalysisResult Result;

  if (VisitedInstrs.contains(&Phi))
    return Result;
  VisitedInstrs.insert(&Phi);

  Register DefReg = Phi.getOperand(0).getReg();
  // We can not break/return from this loop as soon as we have an S20 use
  // because we need to collect MayNeedFullPrecision.
  for (auto &User : MRI.use_instructions(DefReg)) {
    const unsigned Opcode = User.getOpcode();
    if (Opcode == TargetOpcode::G_TRUNC) {
      const unsigned TruncatedSize =
          MRI.getType(User.getOperand(0).getReg()).getScalarSizeInBits();
      if (TruncatedSize == 20)
        Result.IsUsedByS20 = true;
      else if (TruncatedSize > 20)
        Result.MayNeedFullPrecision = true;
    } else if (Opcode == TargetOpcode::G_PHI) {
      Result |= isUsedByAnyS20Instruction(User, VisitedInstrs, MRI);
    } else {
      Result.MayNeedFullPrecision = true;
    }
  }

  return Result;
}

/// Change the type used in a phi node. The new type should have
/// size in bits <= the old type
static void retypePhiNode(MachineInstr &Phi, bool IsSigned, LLT NewType,
                          MachineIRBuilder &B, MachineRegisterInfo &MRI,
                          GISelChangeObserver &Observer,
                          CombinerHelper &Helper) {

  const Register NewDefReg = MRI.createGenericVirtualRegister(NewType);
  const Register DefReg = Phi.getOperand(0).getReg();
  const LLT OldType = MRI.getType(DefReg);
  assert(NewType.getSizeInBits() <= OldType.getSizeInBits() &&
         "New size > Older size");
  const unsigned ChangeUseOpcode =
      NewType.getSizeInBits() < OldType.getSizeInBits()
          ? TargetOpcode::G_TRUNC
          : TargetOpcode::G_BITCAST;
  const unsigned ChangeDefOpcode =
      NewType.getSizeInBits() < OldType.getSizeInBits()
          ? (IsSigned ? TargetOpcode::G_SEXT : TargetOpcode::G_ZEXT)
          : TargetOpcode::G_BITCAST;

  MachineBasicBlock *MBB = Phi.getParent();
  MachineBasicBlock::iterator InsertPt = MBB->getFirstNonPHI();
  B.setInsertPt(*MBB, InsertPt);
  // Change the output.
  B.buildInstr(ChangeDefOpcode).addDef(DefReg).addUse(NewDefReg);
  Observer.changingInstr(Phi);
  Phi.getOperand(0).setReg(NewDefReg);
  Observer.changedInstr(Phi);
  // Change the inputs.
  for (unsigned OpNum = 1; OpNum < Phi.getNumOperands(); OpNum += 2) {
    const Register SrcReg = Phi.getOperand(OpNum).getReg();
    MachineInstr *DefMI = MRI.getVRegDef(SrcReg);

    MachineBasicBlock *DefMIMBB = DefMI->getParent();
    MachineBasicBlock::iterator InsertPt = ++DefMI->getIterator();
    if (InsertPt != DefMIMBB->end() && InsertPt->isPHI())
      InsertPt = DefMIMBB->getFirstNonPHI();

    B.setInsertPt(*DefMI->getParent(), InsertPt);
    B.setDebugLoc(DefMI->getDebugLoc());
    Register NewSrcReg = 0;
    // Try to find an equivalent type conversion operation that dominates Phi,
    // so reuse it. The uniqueness of type conversion are a key point for other
    // combiners, in this way we ensure that all opportunities will be
    // uncovered.
    for (auto &OtherUser : MRI.use_instructions(SrcReg)) {
      if (OtherUser.getOpcode() == ChangeUseOpcode) {
        const Register OtherDstReg = OtherUser.getOperand(0).getReg();
        const LLT OtherType = MRI.getType(OtherDstReg);
        if (OtherType == NewType && Helper.dominates(OtherUser, Phi)) {
          NewSrcReg = OtherDstReg;
          break;
        }
      }
    }

    if (!NewSrcReg) {
      NewSrcReg = MRI.createGenericVirtualRegister(NewType);
      B.buildInstr(ChangeUseOpcode).addDef(NewSrcReg).addUse(SrcReg);
    }

    Observer.changingInstr(Phi);
    Phi.getOperand(OpNum).setReg(NewSrcReg);
    Observer.changedInstr(Phi);
  }
}

/// Narrow Phi nodes to s20, when it is safe.
bool llvm::matchNarrowPhi(MachineInstr &Phi, MachineRegisterInfo &MRI,
                          CombinerHelper &Helper, GISelChangeObserver &Observer,
                          BuildFnTy &MatchInfo) {

  assert(Phi.getOpcode() == TargetOpcode::G_PHI);

  // We can only narrow s32 -> s20.
  if (MRI.getType(Phi.getOperand(0).getReg()) != LLT::scalar(32))
    return false;

  SmallPtrSet<MachineInstr *, 4> VisitedInstrs;
  PhiUsageAnalysisResult Result =
      isUsedByAnyS20Instruction(Phi, VisitedInstrs, MRI);
  if (!Result.IsUsedByS20)
    return false;

  VisitedInstrs.clear();
  const bool CanKeepPrecision =
      canBeNarrowedWithoutLoss(Phi, VisitedInstrs, MRI);

  // We can only narrow in the following 2 situations:
  // * We may lose precision by narrowing this PHI node, but all
  // users are s20 bit users, so higher bits are not important.
  // * We have users that may require full precision, however, we know
  // that the narrowed inputs can be represented as s20 without loss
  // of precision.
  if (!CanKeepPrecision && Result.MayNeedFullPrecision)
    return false;

  // We will do the following transformation:
  // PHI(A, B) -> ZEXT(PHI(TRUNC(A), TRUNC(B)))
  MatchInfo = [=, &MRI, &Phi, &Observer, &Helper](MachineIRBuilder &B) {
    // We change the PHI node instead of building a new one.
    const LLT S20 = LLT::scalar(20);
    // We extend the output also truncate the inputs.
    // We use ZExt (IsSigned) because it is the only safe, considering the
    // previous analysis (isUsedByAnyS20Instruction - trunc case).
    retypePhiNode(Phi, /*IsSigned=*/false, S20, B, MRI, Observer, Helper);
  };

  return true;
}

/// Check if the user is a potentially legal user of s20 type.
/// If it is not legal, it will be legalized as 32 bit operation.
static bool isUsedByLikelyLegalS20User(MachineRegisterInfo &MRI,
                                       const MachineInstr &MI) {
  return any_of(MRI.use_nodbg_instructions(MI.getOperand(0).getReg()),
                [&](const MachineInstr &UseMI) {
                  return UseMI.getOpcode() == TargetOpcode::G_PHI ||
                         UseMI.getOpcode() == TargetOpcode::G_PTR_ADD ||
                         UseMI.getOpcode() == TargetOpcode::G_STORE ||
                         UseMI.getOpcode() == TargetOpcode::G_INTRINSIC ||
                         UseMI.getOpcode() ==
                             TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS ||
                         UseMI.getOpcode() == TargetOpcode::G_INTTOPTR;
                });
}

static void changeLoadStoreDataRegister(MachineInstr &MI, Register DataReg,
                                        MachineRegisterInfo &MRI) {
  GLoadStore &LdStInst = cast<GLoadStore>(MI);
  LdStInst.getMMO().setType(MRI.getType(DataReg));
  LdStInst.getOperand(0).setReg(DataReg);
}

/// Narrow operations that are feeding truncations to s20.
/// Covers G_CONSTANT.
bool llvm::matchNarrowTruncConstant(MachineInstr &MI, MachineRegisterInfo &MRI,
                                    GISelChangeObserver &Observer,
                                    BuildFnTy &MatchInfo) {

  assert(MI.getOpcode() == TargetOpcode::G_TRUNC);

  auto [DstReg, SrcReg] = MI.getFirst2Regs();

  if (MRI.getType(DstReg).getScalarSizeInBits() != 20)
    return false;

  MachineInstr &SrcMI = *MRI.getVRegDef(SrcReg);

  if (SrcMI.getOpcode() != TargetOpcode::G_CONSTANT)
    return false;

  MatchInfo = [=, &MI, &SrcMI, &MRI, &Observer](MachineIRBuilder &B) {
    auto NewConstant = B.buildConstant(
        LLT::scalar(20),
        *getIConstantVRegSExtVal(SrcMI.getOperand(0).getReg(), MRI));
    Register FromReg = MI.getOperand(0).getReg();
    Observer.changingAllUsesOfReg(MRI, FromReg);
    MRI.replaceRegWith(FromReg, NewConstant->getOperand(0).getReg());
    Observer.finishedChangingAllUsesOfReg();
    Observer.erasingInstr(MI);
    MI.eraseFromParent();
  };

  return true;
}

/// True iff Reg has at least one non-debug user and every non-debug
/// user satisfies Pred. Wraps the empty-check + all_of idiom that
/// load-rewriting combines repeat.
template <typename Predicate>
static bool allNonDbgUsersMatch(Register Reg, const MachineRegisterInfo &MRI,
                                Predicate Pred) {
  return !MRI.use_nodbg_empty(Reg) &&
         all_of(MRI.use_nodbg_instructions(Reg), Pred);
}

/// Re-type \p MI (a G_LOAD) to produce a fresh vreg of \p NewDstTy, with
/// proper observer notifications. Returns the new destination register.
/// The caller's old destination register survives in MRI (now without a
/// def) and can still be walked via use lists for any post-rewrite
/// cleanup.
static Register retypeLoadDest(MachineInstr &MI, LLT NewDstTy,
                               MachineRegisterInfo &MRI,
                               GISelChangeObserver &Observer) {
  assert(MI.getOpcode() == TargetOpcode::G_LOAD);
  const Register NewDstReg = MRI.createGenericVirtualRegister(NewDstTy);
  Observer.changingInstr(MI);
  changeLoadStoreDataRegister(MI, NewDstReg, MRI);
  Observer.changedInstr(MI);
  return NewDstReg;
}

/// Narrow operations that are feeding truncations to s20.
/// Covers G_LOAD.
bool llvm::matchNarrowTruncLoad(MachineInstr &MI, MachineRegisterInfo &MRI,
                                CombinerHelper &Helper,
                                GISelChangeObserver &Observer,
                                BuildFnTy &MatchInfo) {

  assert(MI.getOpcode() == TargetOpcode::G_LOAD);

  auto IsProfitableTruncToS20 = [&](const MachineInstr &MaybeTruncMI) {
    if (MaybeTruncMI.getOpcode() != TargetOpcode::G_TRUNC)
      return false;
    const Register DstReg = MaybeTruncMI.getOperand(0).getReg();
    if (MRI.getType(DstReg) != S20)
      return false;
    return isUsedByLikelyLegalS20User(MRI, MaybeTruncMI);
  };

  // We should have a G_LOAD feeding interesting truncations.
  const Register DstReg = MI.getOperand(0).getReg();
  if (!allNonDbgUsersMatch(DstReg, MRI, IsProfitableTruncToS20))
    return false;

  MatchInfo = [=, &MI, &MRI, &Observer](MachineIRBuilder &B) {
    const Register NewDstReg = retypeLoadDest(MI, S20, MRI, Observer);
    // Build Zext after the load, not before.
    MachineBasicBlock &MBB = *MI.getParent();
    B.setInsertPt(MBB, MI.getNextNode() ? MI.getNextNode() : MBB.end());
    B.buildZExt(DstReg, NewDstReg);
  };

  return true;
}

/// Narrow operations that are fed by zext from s20.
/// Covers G_STORE.
bool llvm::matchNarrowZext(MachineInstr &MI, MachineRegisterInfo &MRI,
                           GISelChangeObserver &Observer,
                           BuildFnTy &MatchInfo) {

  assert(MI.getOpcode() == TargetOpcode::G_ZEXT);

  const LLT S20 = LLT::scalar(20);
  const LLT S32 = LLT::scalar(32);
  auto [DstReg, SrcReg] = MI.getFirst2Regs();

  if (MRI.getType(SrcReg) != S20 || MRI.getType(DstReg) != S32)
    return false;

  if (!MRI.hasOneNonDBGUse(DstReg))
    return false;

  MachineInstr &DstMI = *MRI.use_instr_nodbg_begin(DstReg);

  if (DstMI.getOpcode() == TargetOpcode::G_STORE) {
    MatchInfo = [=, &MI, &DstMI, &MRI, &Observer](MachineIRBuilder &B) {
      Observer.changingInstr(DstMI);
      changeLoadStoreDataRegister(DstMI, MI.getOperand(1).getReg(), MRI);
      Observer.changedInstr(DstMI);
      Observer.erasingInstr(MI);
      MI.eraseFromParent();
    };
    return true;
  }

  return false;
}

namespace {
// We match widenings from 16 bit, with possible negations on top.
// Negations commute with conversions and multiplications. We keep track of the
// total number of negations modulo two.
class ExtendOperand {
public:
  Register Source{};
  bool Negate = false;
  ExtendOperand operator-() { return {Source, !Negate}; }
  operator bool() { return Source; }
};

ExtendOperand matchExtend(Register SrcReg, MachineRegisterInfo &MRI) {
  const MachineInstr *SrcMI = MRI.getVRegDef(SrcReg);
  if (SrcMI->getOpcode() == TargetOpcode::G_FPEXT) {
    const Register HalfOp = SrcMI->getOperand(1).getReg();
    if (MRI.getType(HalfOp) != S16) {
      return {};
    }
    return {HalfOp, false};
  }
  if (SrcMI->getOpcode() == TargetOpcode::G_FNEG) {
    return -matchExtend(SrcMI->getOperand(1).getReg(), MRI);
  }
  return {};
}
} // namespace

bool llvm::matchWidenFMul(MachineInstr &FMul, MachineRegisterInfo &MRI,
                          GISelChangeObserver &Observer, BuildFnTy &MatchInfo) {
  if (!FMul.getMF()->getTarget().getTargetTriple().isAIE2P()) {
    return false;
  }

  ExtendOperand Lft = matchExtend(FMul.getOperand(1).getReg(), MRI);
  if (!Lft) {
    return false;
  }
  ExtendOperand Rgt = matchExtend(FMul.getOperand(2).getReg(), MRI);
  if (!Rgt) {
    return false;
  }

  const Register DstReg = FMul.getOperand(0).getReg();
  const bool Negate = Lft.Negate ^ Rgt.Negate;

  // We build extract(mul(tovector(Lft), tovector(Rgt)), 0)
  MatchInfo = [=](MachineIRBuilder &B) {
    const LLT VecTy = V32S16;
    const Register VLhs = buildScalarAsVector(B, Lft.Source, VecTy);
    const Register VRhs = buildScalarAsVector(B, Rgt.Source, VecTy);
    const Register Acc = buildWidenMulScalarAsVector(B, VLhs, VRhs, Negate);
    B.buildCopy(DstReg, buildGetFirstElement(B, Acc));
  };

  return true;
}

// Fold G_TRUNC (G_[ANY|S|Z]EXT x) -> X or (G_[ANY|S|Z]EXT x) or (G_TRUNC x).
bool llvm::matchCombineExtAndTrunc(MachineInstr &MI, MachineRegisterInfo &MRI,
                                   BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_TRUNC && "Expected a G_TRUNC");

  Register TruncSrcReg = MI.getOperand(1).getReg();
  const MachineInstr *SrcMI = MRI.getVRegDef(TruncSrcReg);
  const unsigned SrcOpc = SrcMI->getOpcode();

  if (SrcOpc != TargetOpcode::G_ANYEXT && SrcOpc != TargetOpcode::G_SEXT &&
      SrcOpc != TargetOpcode::G_ZEXT) {
    return false;
  }

  Register ExtSrcReg = SrcMI->getOperand(1).getReg();
  Register TruncDstReg = MI.getOperand(0).getReg();
  const unsigned SrcTySize = MRI.getType(ExtSrcReg).getSizeInBits();
  const unsigned DstTySize = MRI.getType(TruncDstReg).getSizeInBits();

  if (SrcTySize == DstTySize)
    MatchInfo = [=](MachineIRBuilder &B) {
      B.buildCopy(TruncDstReg, ExtSrcReg);
    };
  else if (SrcTySize < DstTySize)
    MatchInfo = [=](MachineIRBuilder &B) {
      B.buildInstr(SrcOpc, {TruncDstReg}, {ExtSrcReg});
    };
  else
    MatchInfo = [=](MachineIRBuilder &B) {
      B.buildTrunc(TruncDstReg, ExtSrcReg);
    };
  return true;
}

bool llvm::matchConstLoad(MachineInstr &MI, MachineRegisterInfo &MRI,
                          GISelChangeObserver &Observer, BuildFnTy &MatchInfo) {

  if (!FoldInvariantLoads)
    return false;

  assert(MI.getOpcode() == TargetOpcode::G_LOAD && "Not G_LOAD");

  const Register DataReg = MI.getOperand(0).getReg();
  const Register PtrReg = MI.getOperand(1).getReg();

  const MachineMemOperand *MMO = *MI.memoperands_begin();
  if (!MMO || !MMO->isInvariant()) {
    LLVM_DEBUG(dbgs() << "Non-invariant load: " << MI);
    return false;
  }

  const LLT DataRegType = MRI.getType(DataReg);
  if (!DataRegType.isScalar() || DataRegType.getSizeInBits() > 32) {
    LLVM_DEBUG(dbgs() << "Non-optimizable invariant load: " << MI);
    return false;
  }

  unsigned Offset = 0;
  const MachineInstr *DefPtrReg = MRI.getVRegDef(PtrReg);
  if (DefPtrReg->getOpcode() == TargetOpcode::G_PTR_ADD) {
    Register OffsetReg = DefPtrReg->getOperand(2).getReg();
    auto Cst = getIConstantVRegValWithLookThrough(OffsetReg, MRI);

    if (!Cst)
      return false;

    Offset = Cst->Value.getZExtValue();
    DefPtrReg = MRI.getVRegDef(DefPtrReg->getOperand(1).getReg());
  }

  if (DefPtrReg->getOpcode() != TargetOpcode::G_GLOBAL_VALUE)
    return false;

  const GlobalValue *GV = DefPtrReg->getOperand(1).getGlobal();
  const Constant *ConstPtr = dyn_cast<const Constant>(GV);
  if (!ConstPtr)
    return false;

  // External constants have zero operands (no visible initializer).
  if (ConstPtr->getNumOperands() == 0)
    return false;

  const Constant *ConstData = dyn_cast<Constant>(ConstPtr->getOperand(0));
  if (!ConstData)
    return false;

  Type *IntTy =
      Type::getIntNTy(ConstData->getContext(), DataRegType.getSizeInBits());
  const Constant *ConstantToFold = llvm::ConstantFoldLoadFromConst(
      const_cast<Constant *>(ConstData), IntTy, APInt(20, Offset),
      GV->getParent()->getDataLayout());

  if (!ConstantToFold) {
    LLVM_DEBUG(dbgs() << "Non-optimizable invariant load using: " << ConstPtr);
    return false;
  }

  const ConstantInt *ScalarConst = dyn_cast<ConstantInt>(ConstantToFold);
  if (!ScalarConst)
    return false;

  const int64_t V = ScalarConst->getSExtValue();
  MatchInfo = [=, &MI, &Observer](MachineIRBuilder &B) {
    const Register IdxReg = B.buildConstant(DataRegType, V).getReg(0);
    B.buildCopy(DataReg, IdxReg);
    Observer.erasingInstr(MI);
    MI.eraseFromParent();
  };

  return true;
}

/// Transform this:
///   %3:_(<32 x s32>) = G_BITCAST %0(<16 x s64>)
///   %4:_(<16 x s32>), %5:_(<16 x s32>) = G_UNMERGE_VALUES %3(<32 x s32>)
/// Into this:
///    %4:_(<8 x s64>), %5:_(<8 x s64>) = G_UNMERGE_VALUES %0(<16 x s64>)
///    %2:_(<16 x s32>) = G_BITCAST %4(<8 x s64>)
///    %3:_(<16 x s32>) = G_BITCAST %5(<8 x s64>)
/// The goal of this combiner is to expose accumulator use. For example,
/// by rotating to unmerge->bitcast we can later optimize a phi node
/// to carry directly an accumulator instead of vector (reducing moves).
bool llvm::matchBitcastUnmerge(MachineInstr &Unmerge, MachineRegisterInfo &MRI,
                               const AIEBaseInstrInfo &TII,
                               GISelChangeObserver &Observer,
                               BuildFnTy &MatchInfo) {
  assert(Unmerge.getOpcode() == TargetOpcode::G_UNMERGE_VALUES);

  Register FirstDefReg = Unmerge.getOperand(0).getReg();
  const LLT DefType = MRI.getType(FirstDefReg);

  if (DefType.getSizeInBits() != TII.getBasicVectorBitSize())
    return false;

  Register SrcReg = (Unmerge.uses().begin())->getReg();
  MachineInstr *Bitcast = MRI.getVRegDef(SrcReg);

  if (Bitcast->getOpcode() != TargetOpcode::G_BITCAST ||
      !MRI.hasOneNonDBGUse(Bitcast->getOperand(0).getReg()))
    return false;

  Register BitcastSrcReg = Bitcast->getOperand(1).getReg();
  const LLT BitcastSrcType = MRI.getType(BitcastSrcReg);
  if (!BitcastSrcType.isFixedVector())
    return false;

  MatchInfo = [=, &MRI, &Unmerge, &Observer](MachineIRBuilder &B) {
    const unsigned NumDefs = Unmerge.getNumDefs();
    const LLT ReducedType = BitcastSrcType.divide(NumDefs);
    Observer.changingInstr(Unmerge);
    Unmerge.uses().begin()->setReg(BitcastSrcReg);
    Observer.changedInstr(Unmerge);
    B.setInsertPt(*Unmerge.getParent(), ++Unmerge.getIterator());

    for (auto &OrigDef : Unmerge.defs()) {
      Register DefReg = MRI.createGenericVirtualRegister(ReducedType);
      Register OrigDefReg = OrigDef.getReg();
      Observer.changingInstr(Unmerge);
      OrigDef.setReg(DefReg);
      Observer.changedInstr(Unmerge);

      B.buildBitcast(OrigDefReg, DefReg);
    }
  };

  return true;
}

static std::optional<LLT> getUseBitcastedType(MachineInstr &Phi,
                                              MachineRegisterInfo &MRI) {

  for (unsigned I = 1; I < Phi.getNumOperands(); I += 2) {
    Register IncomingReg = Phi.getOperand(I).getReg();
    const MachineInstr *IncomingMI = MRI.getVRegDef(IncomingReg);

    if (IncomingMI->getParent() != Phi.getParent())
      continue;

    if (IncomingMI->getOpcode() == TargetOpcode::G_BITCAST) {
      return MRI.getType(IncomingMI->getOperand(1).getReg());
    }
  }
  return std::nullopt;
}

static std::optional<LLT> getDefBitcastedType(MachineInstr &Phi,
                                              MachineRegisterInfo &MRI) {
  Register DefReg = Phi.getOperand(0).getReg();
  if (!MRI.hasOneNonDBGUse(DefReg))
    return std::nullopt;

  const MachineInstr *UseMI = &*MRI.use_instr_nodbg_begin(DefReg);
  if (UseMI->getOpcode() != TargetOpcode::G_BITCAST)
    return std::nullopt;

  return MRI.getType(UseMI->getOperand(0).getReg());
}

/// Transform this:
///  bb.1:
///    %0:_(<16 x s32>) = G_PHI %15(<16 x s32>), %bb.0, %12(<16 x s32>), %bb.1
///    %X:_(<8 x s64>) = G_BITCAST %0(<16 x s32>)
///    ...
///    %Y ...
///    %12:_(<16 x s32>) = G_BITCAST %Y(<8 x s64>)
///    G_BR %bb.1
/// Into this:
///  bb.1:
///    %16:_(<8 x s64>) = G_PHI %17(<8 x s64>), %bb.0, %Y(<8 x s64>), %bb.1
///    ...
///    %Y ...
///    G_BR %bb.1
bool llvm::matchPhiBitcast(MachineInstr &Phi, MachineRegisterInfo &MRI,
                           CombinerHelper &Helper, const AIEBaseInstrInfo &TII,
                           GISelChangeObserver &Observer,
                           BuildFnTy &MatchInfo) {

  assert(Phi.isPHI());
  if (Phi.getNumOperands() != 5)
    return false;

  Register DefReg = Phi.getOperand(0).getReg();
  LLT DefType = MRI.getType(DefReg);
  if (DefType.getSizeInBits() != TII.getBasicVectorBitSize())
    return false;

  auto ToType = getDefBitcastedType(Phi, MRI);
  if (!ToType)
    return false;

  auto FromType = getUseBitcastedType(Phi, MRI);
  if (!FromType)
    return false;

  if (*FromType != *ToType)
    return false;

  const LLT NewType = *FromType;
  MatchInfo = [=, &MRI, &Phi, &Observer, &Helper](MachineIRBuilder &B) {
    // We change the PHI node instead of building a new one,
    // with all types bitcasted.
    retypePhiNode(Phi, /*IsSigned*/ false, NewType, B, MRI, Observer, Helper);
  };

  return true;
}

/// This combiner is intended to fully define undefined operands
/// of s20 types in Phi instructions. The goal is to constrain
/// the MIR to a fully well-defined shape that fits the requirements
/// of our 2D/3D register allocation engine:
/// * After phi-node elimination, we should be able to rewrite/rename
///   each sub-lane copy individually, so all renamed registers should
///   dominate their uses. This is not possible if one side of the Phi
///   node is undefined - in this case the renamed register definition
///   will happen only on the well-defined side of the phi node.
bool llvm::matchPhiOfUndef(MachineInstr &Phi, MachineRegisterInfo &MRI,
                           GISelChangeObserver &Observer,
                           BuildFnTy &MatchInfo) {
  assert(Phi.isPHI());

  const LLT S20 = LLT::scalar(20);
  const LLT DefType = MRI.getType(Phi.getOperand(0).getReg());

  if (DefType != S20)
    return false;

  MachineInstr *UndefMI = nullptr;
  MachineOperand *UndefMO = nullptr;
  for (unsigned I = 1; I < Phi.getNumOperands(); I += 2) {
    MachineOperand &MO = Phi.getOperand(I);
    const Register IncomingReg = MO.getReg();
    MachineInstr *DefMI = MRI.getVRegDef(IncomingReg);
    if (DefMI->getOpcode() == TargetOpcode::G_IMPLICIT_DEF) {
      // We found one case, the next operand can be another case,
      // but let the fixed-point combiner engine to discover.
      UndefMI = DefMI;
      UndefMO = &MO;
      break;
    }
  }

  if (!UndefMI)
    return false;

  MatchInfo = [=, &Phi, &Observer](MachineIRBuilder &B) {
    B.setInstrAndDebugLoc(*UndefMI);
    const Register NewReg = B.buildConstant(S20, 0).getReg(0);
    Observer.changingInstr(Phi);
    UndefMO->setReg(NewReg);
    Observer.changedInstr(Phi);
  };

  return true;
}

// If we have enough bytes set on a stack memset, we can simply
// align this stack object to avoid store scalarization during
// legalization.
bool llvm::matchAlignMemset(MachineInstr &MI, MachineRegisterInfo &MRI,
                            const AIEBaseInstrInfo &TII,
                            GISelChangeObserver &Observer,
                            BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_MEMSET && "Expected a G_MEMSET");

  if (!MemsetOptimizations)
    return false;

  // Try to keep alignment increase as minimum.
  const unsigned BasicVectorByteSize = TII.getBasicVecRegSize() / 8;
  // Half vectors are also supported.
  const unsigned HalfVectorByteSize = BasicVectorByteSize / 2;

  const Register CountReg = MI.getOperand(2).getReg();
  auto Cst = getIConstantVRegValWithLookThrough(CountReg, MRI);
  if (!Cst)
    return false;
  const unsigned ByteCount = Cst->Value.getZExtValue();

  // Can we fill, at least, half of a basic vector?
  if (ByteCount < HalfVectorByteSize)
    return false;

  const Register PtrReg = MI.getOperand(0).getReg();
  const MachineInstr *DefDataInst = MRI.getUniqueVRegDef(PtrReg);

  if (DefDataInst->getOpcode() != TargetOpcode::G_FRAME_INDEX)
    return false;

  if (MI.memoperands_empty())
    return false;
  MachineMemOperand *MMO = MI.memoperands().front();

  const int FrameIndex = DefDataInst->getOperand(1).getIndex();

  const Align OptimalAlign =
      Align(ByteCount < BasicVectorByteSize ? HalfVectorByteSize
                                            : BasicVectorByteSize);
  const Align MMOAlign = MMO->getAlign();

  if (MMOAlign == OptimalAlign)
    return false;

  MatchInfo = [=, &MI](MachineIRBuilder &B) {
    MachineFunction *MF = MI.getMF();
    MachineFrameInfo &MFI = MF->getFrameInfo();
    MFI.setObjectAlignment(FrameIndex, OptimalAlign);
    const LocationSize Size = MMO->getSize();
    MI.dropMemRefs(*MF);
    MI.addMemOperand(*MF,
                     MF->getMachineMemOperand(
                         MachinePointerInfo::getFixedStack(*MF, FrameIndex),
                         MachineMemOperand::MOStore, Size, OptimalAlign));
  };

  return true;
}

static std::optional<std::pair<Register, int64_t>>
getPtrAndConstantOffset(const MachineInstr *MI, unsigned PointerIndex,
                        MachineRegisterInfo &MRI) {
  assert(MI->getOpcode() == TargetOpcode::G_PTR_ADD && "Expected a G_PTR_ADD");

  const Register OffsetReg = MI->getOperand(2).getReg();
  const auto Cst = getIConstantVRegValWithLookThrough(OffsetReg, MRI);

  if (Cst)
    return std::make_pair(MI->getOperand(PointerIndex).getReg(),
                          Cst->Value.getSExtValue());

  return std::nullopt;
}

template <uint64_t TargetAlign> constexpr bool matchAlignment(uint64_t Value) {
  return isAligned(Align(TargetAlign), Value);
}

static std::optional<std::pair<Register, int64_t>>
getPtrAndConstantOffsetFromReg(Register PtrReg, MachineRegisterInfo &MRI) {

  const MachineInstr *DefPtrReg = MRI.getVRegDef(PtrReg);
  if (DefPtrReg->getOpcode() == TargetOpcode::G_PTR_ADD)
    return getPtrAndConstantOffset(DefPtrReg, 1, MRI);

  return std::make_pair(PtrReg, 0);
}

static bool isBasePointerWordAligned(Register BasePtr,
                                     MachineRegisterInfo &MRI) {

  auto IsBasePointerAligned = [&](const MachineInstr *MI) {
    if (!isa<GLoadStore>(MI))
      return false;
    if (MI->memoperands_empty())
      return false;
    const MachineMemOperand *MMO = MI->memoperands().front();
    return matchAlignment<4>(MMO->getAlign().value());
  };

  for (const MachineInstr &MI : MRI.use_instructions(BasePtr)) {
    if (MI.getOpcode() == TargetOpcode::G_PTR_ADD) {
      auto RegAndOffset = getPtrAndConstantOffset(&MI, 0, MRI);
      if (!RegAndOffset || !matchAlignment<4>(RegAndOffset->second))
        continue;
      // In one user is aligned, it is enough for us.
      if (any_of(MRI.use_instructions(RegAndOffset->first),
                 [&](const MachineInstr &UseMI) {
                   return IsBasePointerAligned(&UseMI);
                 }))
        return true;
    } else if (IsBasePointerAligned(&MI)) {
      return true;
    }
  }

  return false;
}

// We try to align MEMSETs by peeling out some stores.
// To be effective, we need at least 3 bytes here:
// If we are byte-aligned we can generate
//  - G_STORE s8
//  - G_STORE s16
//  - G_MEMSET (... n - 3 ...)
// If we are short-aligned we can generate
//  - G_STORE s16
//  - G_MEMSET (... n - 2 ...)
bool llvm::matchPeelMemset(MachineInstr &MI, MachineRegisterInfo &MRI,
                           const AIEBaseInstrInfo &TII,
                           GISelChangeObserver &Observer,
                           BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_MEMSET && "Expected a G_MEMSET");

  if (!MemsetOptimizations)
    return false;

  MachineMemOperand *MMO = MI.memoperands().front();

  if (!MMO)
    return false;
  const Align MMOAlign = MMO->getAlign();

  // If it is already aligned we have nothing to do.
  if (matchAlignment<4>(MMOAlign.value()))
    return false;

  const Register SizeReg = MI.getOperand(2).getReg();

  const auto Cst = getIConstantVRegValWithLookThrough(SizeReg, MRI);
  if (!Cst)
    return false;
  const int64_t Size = Cst->Value.getSExtValue();

  Register PtrReg = MI.getOperand(0).getReg();
  const Register DataReg = MI.getOperand(1).getReg();
  const auto CstInit = getIConstantVRegValWithLookThrough(DataReg, MRI);
  if (!CstInit)
    return false;
  const uint64_t Initializer = CstInit->Value.getZExtValue();

  const auto RegAndOffset = getPtrAndConstantOffsetFromReg(PtrReg, MRI);
  if (!RegAndOffset)
    return false;

  const int64_t Offset = RegAndOffset->second;
  PtrReg = RegAndOffset->first;

  // Next step is to prove that the base pointer is word-aligned.
  // As we cannot assume, we can search for aligned uses of the base pointer.
  if (!isBasePointerWordAligned(PtrReg, MRI))
    return false;

  MatchInfo = [=, &MI, &MRI, &Observer](MachineIRBuilder &B) {
    auto &MF = B.getMF();

    auto BuildPADD = [&](int64_t CurrentOffset) {
      Register NewPtrReg = MRI.cloneVirtualRegister(PtrReg);
      Register OffsetReg =
          B.buildConstant(LLT::scalar(20), CurrentOffset).getReg(0);
      B.buildInstr(TargetOpcode::G_PTR_ADD)
          .addDef(NewPtrReg)
          .addReg(PtrReg)
          .addReg(OffsetReg);
      return NewPtrReg;
    };

    auto BuildMMO = [&](LocationSize Size, Align A) {
      MachineMemOperand *NewMMO = MF.getMachineMemOperand(
          MMO->getPointerInfo(), MMO->getFlags(), Size, A);
      return NewMMO;
    };

    int64_t PeelOffset = Offset;
    // If offset is aligned, just fix the alignment.
    if (!matchAlignment<4>(PeelOffset)) {
      // If not short-aligned, align to the next short boundary (1 byte).
      if (!matchAlignment<2>(PeelOffset)) {
        B.buildStore(DataReg, BuildPADD(PeelOffset),
                     *BuildMMO(MMO->getSize(), Align(1)));
        PeelOffset++;
      }

      // If we are short-aligned, but still not word aligned, align to the
      // next word boundary (2 bytes more).
      if (!matchAlignment<4>(PeelOffset)) {
        // Store the next two bytes to align to the next word boundary.
        Register DataRegAdjustedToS16 =
            B.buildConstant(LLT::scalar(16), (Initializer << 8) | Initializer)
                .getReg(0);
        B.buildStore(DataRegAdjustedToS16, BuildPADD(PeelOffset),
                     *BuildMMO(2, Align(2)));
        PeelOffset += 2;
      }
    }

    const unsigned NewSize = Size - (PeelOffset - Offset);

    // No bytes left to memset.
    if (NewSize == 0) {
      Observer.erasingInstr(MI);
      MI.eraseFromParent();
      return;
    }

    const int64_t MemsetOffset = PeelOffset;
    assert(matchAlignment<4>(MemsetOffset) && "Memset still unaligned?");
    // Now, what remains is aligned, we just need to fix Offset, Size and MMO.
    MachineMemOperand *NewMMOMemSet = BuildMMO(MMO->getSize(), Align(4));
    Observer.changingInstr(MI);
    MI.dropMemRefs(MF); // Safe to drop the MMO now.
    MI.addMemOperand(MF, NewMMOMemSet);
    MI.getOperand(2).setReg(
        B.buildConstant(LLT::scalar(20), NewSize).getReg(0));
    MI.getOperand(0).setReg(BuildPADD(MemsetOffset));
    Observer.changedInstr(MI);
  };

  return true;
}

static std::optional<std::pair<Register, int64_t>>
getPtrAndConstantOffsetFromStore(const GStore *StMI, MachineRegisterInfo &MRI) {
  return getPtrAndConstantOffsetFromReg(StMI->getPointerReg(), MRI);
}

// To make a store dead, we convert it to a dead G_ADD and let DCE to do the
// removal
static void makeStoreDead(GStore *StMI, const TargetInstrInfo &TII,
                          MachineRegisterInfo &MRI) {
  MachineFunction &MF = *StMI->getMF();
  const Register DataReg = StMI->getValueReg();
  StMI->dropMemRefs(MF);
  for (int I = StMI->getNumOperands() - 1; I >= 0; I--)
    StMI->removeOperand(I);
  StMI->setDesc(TII.get(TargetOpcode::G_ADD));
  StMI->addOperand(
      MachineOperand::CreateReg(MRI.cloneVirtualRegister(DataReg), true));
  StMI->addOperand(MachineOperand::CreateReg(DataReg, false));
  StMI->addOperand(MachineOperand::CreateReg(DataReg, false));
}

// Split a store of a G_CONCAT_VECTORS result into two half-sized stores.
// This allows each half to be stored independently, avoiding the creation of
// a wide value that may require a more constrained register class.
// Handles G_STORE, G_AIE_OFFSET_STORE and G_AIE_POSTINC_STORE.
//   store (G_CONCAT_VECTORS lo, hi), ptr
// becomes:
//   store hi, ptr + halfsize
//   store lo, ptr
bool llvm::matchSplitConcatStore(MachineInstr &StMI, MachineRegisterInfo &MRI,
                                 const AIEBaseInstrInfo &TII,
                                 BuildFnTy &MatchInfo) {
  // Only G_AIE_POSTINC_STORE has an explicit def (the updated pointer).
  const bool IsPostIncStore = StMI.getNumExplicitDefs() > 0;
  // G_STORE is the only target-independent opcode matched; the remaining
  // non-postinc case is G_AIE_OFFSET_STORE.
  const bool IsOffsetStore =
      !IsPostIncStore && StMI.getOpcode() != TargetOpcode::G_STORE;

  const unsigned DataOpIdx = IsPostIncStore ? 1 : 0;
  const unsigned PtrOpIdx = IsPostIncStore ? 2 : 1;

  const Register DataReg = StMI.getOperand(DataOpIdx).getReg();
  MachineInstr *const ConcatI = MRI.getVRegDef(DataReg);

  // Only match stores of two-operand G_CONCAT_VECTORS at or above the target's
  // maximum vector size, with a single use (the store itself).
  if (!ConcatI || ConcatI->getOpcode() != TargetOpcode::G_CONCAT_VECTORS ||
      ConcatI->getNumOperands() != 3)
    return false;
  if (MRI.getType(DataReg).getSizeInBits() < TII.getMaxVectorBitSize() ||
      !MRI.hasOneNonDBGUse(DataReg))
    return false;

  const Register LoReg = ConcatI->getOperand(1).getReg();
  const Register HiReg = ConcatI->getOperand(2).getReg();
  const LLT HalfTy = MRI.getType(LoReg);
  const unsigned HalfSizeBytes = HalfTy.getSizeInBytes();
  const Register PtrReg = StMI.getOperand(PtrOpIdx).getReg();
  const LLT PtrTy = MRI.getType(PtrReg);

  MatchInfo = [=, &StMI](MachineIRBuilder &B) {
    MachineMemOperand *const MMO = *StMI.memoperands_begin();

    // Compute the effective base pointer.
    // For G_AIE_OFFSET_STORE, incorporate the offset into the base pointer.
    // For G_AIE_POSTINC_STORE, the post-increment is handled separately below.
    Register BasePtrReg = PtrReg;
    if (IsOffsetStore) {
      Register OffsetReg = StMI.getOperand(2).getReg();
      BasePtrReg = B.buildPtrAdd(PtrTy, PtrReg, OffsetReg).getReg(0);
    }
    // Store upper half at base + halfsize first.
    auto HiPtr = B.buildPtrAdd(PtrTy, BasePtrReg,
                               B.buildConstant(LLT::scalar(20), HalfSizeBytes));
    B.buildStore(HiReg, HiPtr.getReg(0),
                 *B.getMF().getMachineMemOperand(MMO, HalfSizeBytes, HalfTy));
    // Store lower half at base pointer last, so that the addressing mode
    // combiner can merge a potential post-increment with it.
    B.buildStore(LoReg, BasePtrReg,
                 *B.getMF().getMachineMemOperand(MMO, 0, HalfTy));

    // For G_AIE_POSTINC_STORE, emit a G_PTR_ADD to produce the updated pointer
    // that replaces the post-increment output. The address combiner can later
    // fold this into one of the half-sized stores.
    if (IsPostIncStore) {
      Register PostIncOut = StMI.getOperand(0).getReg();
      Register IncrReg = StMI.getOperand(3).getReg();
      B.buildPtrAdd(PostIncOut, PtrReg, IncrReg);
    }

    // The dead G_CONCAT_VECTORS will be cleaned up by DCE.
    StMI.eraseFromParent();
  };
  return true;
}

// This combiner tries to pack sequential zero stores into memsets.
// The goal is to reach an optimal number of stores provided we
// use it synergically with the memset expand combiner.
bool llvm::matchSequentialStores(GStore &StMI, MachineRegisterInfo &MRI,
                                 GISelChangeObserver &Observer,
                                 BuildFnTy &MatchInfo) {

  if (!MemsetOptimizations)
    return false;

  const uint64_t MinVectorStoreSize = 16;
  MachineMemOperand *MMO = StMI.memoperands().front();

  if (!MMO)
    return false;
  const Align MMOAlign = MMO->getAlign();

  if (MMOAlign.value() < 4)
    return false;

  const Register DataReg = StMI.getValueReg();
  const LLT DataType = MRI.getType(DataReg);

  // Small alignments are less interesting, we are trying to match
  // vector stores here. Even though we can match some byte/short
  // stores to word stores.
  const bool IsVectorAlignment = MMOAlign.value() >= MinVectorStoreSize;
  // We can merge over aligned small types, provided that
  // the root of the memset (first store) is a byte or short
  // store. The goal is to fold this store with the next ones.
  if (!IsVectorAlignment && DataType.getSizeInBytes() >= 4)
    return false;

  //  A count of zero means that we are not storing zero at all.
  auto GetZeroStoreSizeInBytes = [&](GStore &CurrSt) -> unsigned {
    const Register DataReg = CurrSt.getValueReg();
    const LLT DataType = MRI.getType(DataReg);
    MachineMemOperand *CurrMMO = CurrSt.memoperands().front();

    if (!CurrMMO)
      return 0;
    const Align CurrMMOAlign = MMO->getAlign();

    // We already have a vector store, don't merge it.
    if (DataType.getSizeInBytes() >= CurrMMOAlign.value())
      return 0;
    auto Cst = getIConstantVRegValWithLookThrough(DataReg, MRI);
    return (Cst && Cst->Value.isZero()) ? DataType.getSizeInBytes() : 0;
  };

  auto PtrAndOffset = getPtrAndConstantOffsetFromStore(&StMI, MRI);
  if (!PtrAndOffset)
    return false;
  const auto [Ptr, Offset] = *PtrAndOffset;

  const unsigned ZeroBytes = GetZeroStoreSizeInBytes(StMI);
  if (!ZeroBytes)
    return false;
  int64_t ExpectedOffset = Offset + ZeroBytes;

  std::vector<GStore *> MatchedSeqStores;
  for (MachineInstr &MI : make_range(std::next(StMI.getIterator()),
                                     StMI.getParent()->instr_end())) {
    if (auto *CurrSt = dyn_cast<GStore>(&MI)) {
      const unsigned ZeroBytes = GetZeroStoreSizeInBytes(*CurrSt);
      if (!ZeroBytes) // Non-zero store.
        break;

      auto PtrAndOffset = getPtrAndConstantOffsetFromStore(CurrSt, MRI);
      if (!PtrAndOffset) // Non-constant offset.
        break;

      auto [CurrPtr, CurrOffset] = *PtrAndOffset;

      // Pointers are different or we have non-linear store.
      if ((CurrPtr != Ptr) || (ExpectedOffset != CurrOffset))
        break;

      MatchedSeqStores.push_back(CurrSt);
      ExpectedOffset += ZeroBytes;

    } else if (MI.mayLoadOrStore() || MI.hasUnmodeledSideEffects()) {
      // Bailout to prevent problems related to store reordering.
      break;
    }
  }

  if (MatchedSeqStores.empty())
    return false;

  const unsigned NumberOfBytes = ExpectedOffset - Offset;
  // If we cannot fill a vector, skip, because we will scalarize again
  // and this will be matched again in a loop. However, if we have at least two
  // scalars to merge, go for it. The rationale for scalar merging is: If we
  // have the first scalar store whose size is smaller then the alignment, by
  // combining with the next, we have a chance of reducing the number of stores.
  // For example:
  //   STORE i8 0, [p0] (Align 16)
  //   STORE i16 0, [p0+1]
  //   STORE i8 0, [p0+3]
  // Will be transformed to:
  //   STORE i32 0, [p0] (Align 16)
  if (IsVectorAlignment && NumberOfBytes < MinVectorStoreSize)
    return false;

  // In a pessimistic case, for example:
  //   STORE i8 0, [p0] (Align 16)
  //   STORE i16 0, [p0+1]
  // The result will be just "rotated":
  //   STORE i16 0, [p0] (Align 16)
  //   STORE i8 0, [p0+2]
  // This will cause a loop. We prevent by restricting
  // combinations that will expand again to the same
  // types: 3 bytes.
  if (NumberOfBytes == 3)
    return false;

  MatchInfo = [=, &StMI, &MRI, &Observer](MachineIRBuilder &B) {
    auto &MF = B.getMF();

    MachineMemOperand *NewMMO = MF.getMachineMemOperand(
        MMO->getPointerInfo(), MMO->getFlags(), 8, MMOAlign);

    const Register MemsetDataReg = B.buildConstant(LLT::scalar(8), 0).getReg(0);
    const Register MemsetCountReg =
        B.buildConstant(LLT::scalar(20), NumberOfBytes).getReg(0);
    B.buildInstr(TargetOpcode::G_MEMSET, {},
                 {StMI.getPointerReg(), MemsetDataReg, MemsetCountReg})
        .addImm(0)
        ->addMemOperand(MF, NewMMO);
    Observer.erasingInstr(StMI);
    StMI.eraseFromParent();

    // Tricky part: we cannot erase the matched stores, make them dead.
    for (GStore *ToDeleteMI : MatchedSeqStores) {
      Observer.changingInstr(*ToDeleteMI);
      makeStoreDead(ToDeleteMI, B.getTII(), MRI);
      Observer.changedInstr(*ToDeleteMI);
    }
  };

  return true;
}

namespace {
MachineInstr *getBcstFeedByAssertExtVecExtr(MachineInstr &MI,
                                            MachineRegisterInfo &MRI,
                                            const AIEBaseInstrInfo &TII) {
  assert(isGenericExtractOpcode(MI.getOpcode(), TII));

  /// Get single NonDebug User of \p MI with the opcode \p UseMIOpcode
  auto GetSingleNonDbgUser = [&MRI](MachineInstr &MI,
                                    unsigned UseMIOpcode) -> MachineInstr * {
    const Register Dst = MI.getOperand(0).getReg();
    if (!MRI.hasOneNonDBGUse(Dst))
      // No convexity due to multiple users, skip.
      return nullptr;

    MachineInstr *UserMI = &*MRI.use_nodbg_instructions(Dst).begin();
    if (UserMI->getOpcode() != UseMIOpcode)
      return nullptr; // Did not match Opcode, skip.

    // Found single non debug user with matching opcode.
    return UserMI;
  };

  // Find G_ASSERT_[S/Z]EXT
  MachineInstr *AnyExtMI = nullptr;
  AnyExtMI = GetSingleNonDbgUser(MI, TargetOpcode::G_ASSERT_SEXT);
  if (!AnyExtMI)
    AnyExtMI = GetSingleNonDbgUser(MI, TargetOpcode::G_ASSERT_ZEXT);

  if (!AnyExtMI)
    // Could not find G_ASSERT_[S/Z]EXT
    return nullptr;

  MachineInstr *BcstMI =
      GetSingleNonDbgUser(*AnyExtMI, TII.getGenericBroadcastVectorOpcode());
  return BcstMI;
}
} // namespace

bool llvm::matchExtractVecEltAssertBcst(MachineInstr &MI,
                                        MachineRegisterInfo &MRI,
                                        const AIEBaseInstrInfo &TII,
                                        GISelChangeObserver &Observer,
                                        BuildFnTy &MatchInfo) {
  assert(isGenericExtractOpcode(MI.getOpcode(), TII) &&
         "Expected a extract_vector_elt");
  const MachineInstr *BcstMI = getBcstFeedByAssertExtVecExtr(MI, MRI, TII);
  if (!BcstMI)
    return false;

  MatchInfo = [=, &MI, &MRI, &Observer](MachineIRBuilder &B) {
    MachineInstr &AssertExt =
        *MRI.use_nodbg_instructions(MI.getOperand(0).getReg()).begin();

    MachineOperand &VextrDstMO = MI.getOperand(0);
    Register AssertDst = AssertExt.getOperand(0).getReg();

    // Skip G_ASSERT_[S/Z]EXT
    Observer.changingInstr(MI);
    VextrDstMO.setReg(AssertDst);
    Observer.changedInstr(MI);

    // Remove G_ASSERT_[S/Z]EXT
    Observer.erasingInstr(AssertExt);
    AssertExt.eraseFromParent();
  };

  return true;
}

/// G_AIE_BROADCAST_VECTOR %vec, %scalar
/// G_AIE_[S/Z]EXT_EXTRACT_VECTOR_ELT %dst, %vec, %idx
/// -> COPY %dst, %scalar (with [s/z]ext if element size < 32)
bool llvm::matchExtractBroadcastToScalar(MachineInstr &MI,
                                         MachineRegisterInfo &MRI,
                                         const AIEBaseInstrInfo &TII,
                                         BuildFnTy &MatchInfo) {
  assert(isGenericExtractOpcode(MI.getOpcode(), TII) &&
         "Expected G_AIE_[S/Z]EXT_EXTRACT_VECTOR_ELT");

  // Get the source vector register
  const Register SrcVecReg = MI.getOperand(1).getReg();
  const MachineInstr *SrcMI = MRI.getVRegDef(SrcVecReg);
  if (!SrcMI)
    return false;

  // Check if source is G_AIE_BROADCAST_VECTOR
  if (SrcMI->getOpcode() != TII.getGenericBroadcastVectorOpcode())
    return false;

  // Get the scalar that was broadcast
  const Register BroadcastSrcReg = SrcMI->getOperand(1).getReg();
  const Register DstReg = MI.getOperand(0).getReg();

  // Types must match and 32-bit.
  const LLT SrcTy = MRI.getType(BroadcastSrcReg);
  const LLT DstTy = MRI.getType(DstReg);
  if (SrcTy != DstTy || SrcTy.getScalarSizeInBits() != 32)
    return false;

  // Get the element size of the vector
  const LLT VecTy = MRI.getType(SrcVecReg);
  const unsigned ElemSize = VecTy.getScalarSizeInBits();

  // Determine if this is a sign-extending or zero-extending extract
  const bool IsSext =
      MI.getOpcode() == TII.getGenericExtractVectorEltOpcode(/*SignExt=*/true);

  MatchInfo = [DstReg, BroadcastSrcReg, ElemSize, IsSext,
               DstTy](MachineIRBuilder &B) {
    if (ElemSize < 32) {
      if (IsSext) {
        // Use G_SEXT_INREG for sign extension
        B.buildSExtInReg(DstReg, BroadcastSrcReg, ElemSize);
      } else {
        // Use G_AND with mask for zero extension
        const uint64_t Mask = (1ULL << ElemSize) - 1;
        auto MaskReg = B.buildConstant(DstTy, Mask);
        B.buildAnd(DstReg, BroadcastSrcReg, MaskReg);
      }
    } else {
      B.buildCopy(DstReg, BroadcastSrcReg);
    }
  };

  return true;
}

/// Check if a scalar register contains an MSB-only constant.
/// This is used to optimize XOR operations with MSB-only constants into ADD.
/// XOR with MSB toggles the sign bit, which is equivalent to ADD for these
/// values.
bool llvm::matchMsbScalar(Register ScalarReg, Register BroadcastReg,
                          MachineRegisterInfo &MRI) {
  auto Cst = getIConstantVRegValWithLookThrough(ScalarReg, MRI);
  if (!Cst)
    return false;

  // Get the element type from the broadcast vector destination
  const LLT BroadcastTy = MRI.getType(BroadcastReg);
  if (!BroadcastTy.isVector())
    return false;

  unsigned ElemBitWidth = BroadcastTy.getElementType().getSizeInBits();

  // Get the value as unsigned to properly compare with MSB masks
  const uint64_t Value = Cst->Value.getZExtValue();

  // Check if only MSB is set for the vector element type
  if (ElemBitWidth == 8)
    return Value == 0x80; // 128
  if (ElemBitWidth == 16)
    return Value == 0x8000; // 32768
  if (ElemBitWidth == 32)
    return Value == 0x80000000; // 2147483648 (or -2147483648 as signed)

  return false;
}

/// Match a pattern where:
/// %18:_(<16 x s32>) = COPY $x0
/// %10:_(<16 x s32>) = G_IMPLICIT_DEF
/// %9:_(s32) = G_CONSTANT i32 0
/// %8:_(s32) = G_AIE_SEXT_EXTRACT_VECTOR_ELT %18(<16 x s32>), %9(s32)
/// %22:_(<16 x s32>) = G_AIE_INSERT_VECTOR_ELT %10, %8(s32), %9(s32)
///
/// This can be simplified to:
/// %22:_(<16 x s32>) = COPY %18
bool llvm::matchInsertExtractVectorEltToCopy(MachineInstr &MI,
                                             MachineRegisterInfo &MRI,
                                             const AIEBaseInstrInfo &TII,
                                             BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TII.getGenericInsertVectorEltOpcode() &&
         "Expected G_AIE_INSERT_VECTOR_ELT");

  // Get the insert operands
  const Register InsertDstReg = MI.getOperand(0).getReg();
  const Register InsertSrcVecReg = MI.getOperand(1).getReg();
  const Register InsertedEltReg = MI.getOperand(2).getReg();
  const Register InsertIdxReg = MI.getOperand(3).getReg();

  // Check that the insert source vector is G_IMPLICIT_DEF
  const MachineInstr *InsertSrcMI = MRI.getVRegDef(InsertSrcVecReg);
  if (!InsertSrcMI || InsertSrcMI->getOpcode() != TargetOpcode::G_IMPLICIT_DEF)
    return false;

  // Get the definition of the inserted element
  const MachineInstr *ExtractMI = MRI.getVRegDef(InsertedEltReg);
  if (!ExtractMI)
    return false;

  // Check if it's either SEXT or ZEXT extract
  if (!isGenericExtractOpcode(ExtractMI->getOpcode(), TII)) {
    return false;
  }

  // Get extract operands
  const Register ExtractSrcVecReg = ExtractMI->getOperand(1).getReg();
  const Register ExtractIdxReg = ExtractMI->getOperand(2).getReg();

  // Verify that the insert destination vector type matches the extract source
  // vector type
  const LLT InsertDstTy = MRI.getType(InsertDstReg);
  const LLT ExtractSrcTy = MRI.getType(ExtractSrcVecReg);

  if (InsertDstTy != ExtractSrcTy)
    return false;

  // Check that insert and extract indices are the same
  // They can be the same register, or both constants with the same value
  if (InsertIdxReg != ExtractIdxReg) {
    auto InsertIdxCst = getIConstantVRegValWithLookThrough(InsertIdxReg, MRI);
    auto ExtractIdxCst = getIConstantVRegValWithLookThrough(ExtractIdxReg, MRI);
    if (!InsertIdxCst || !ExtractIdxCst ||
        InsertIdxCst->Value != ExtractIdxCst->Value)
      return false;
  }

  // Copy the extract source vector (the real vector) to the insert destination
  MatchInfo = [=](MachineIRBuilder &B) {
    B.buildCopy(InsertDstReg, ExtractSrcVecReg);
  };

  return true;
}

/// Match a pattern where a broadcast is fed by an extract from position 0,
/// and all uses of the broadcast through a chain of operations only extract
/// from position 0. This allows us to replace the broadcast with a copy of
/// the original vector.
///
/// Pattern:
/// %200:_(s32) = G_AIE_SEXT_EXTRACT_VECTOR_ELT %50(<16 x s32>), %3(s32) // pos
/// 0 %5:_(<16 x s32>) = G_AIE_BROADCAST_VECTOR %200(s32)
/// ... (chain of concat/unmerge/vector ops)
/// %2:_(s32) = G_AIE_SEXT_EXTRACT_VECTOR_ELT %result(<16 x s32>), %3(s32) //
/// pos 0
///
/// Transforms to:
/// %200:_(s32) = G_AIE_SEXT_EXTRACT_VECTOR_ELT %50(<16 x s32>), %3(s32)
/// %5:_(<16 x s32>) = COPY %50(<16 x s32>)  // Copy source vector instead of
/// broadcast
/// ... (chain of operations)
/// %2:_(s32) = G_AIE_SEXT_EXTRACT_VECTOR_ELT %result(<16 x s32>), %3(s32)
bool llvm::matchBroadcastExtractToCopy(MachineInstr &MI,
                                       MachineRegisterInfo &MRI,
                                       const AIEBaseInstrInfo &TII,
                                       BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TII.getGenericBroadcastVectorOpcode() &&
         "Expected G_AIE_BROADCAST_VECTOR");

  // 1. Verify broadcast source is extract from position 0
  const Register BroadcastSrcReg = MI.getOperand(1).getReg();
  const MachineInstr *ExtractMI = MRI.getVRegDef(BroadcastSrcReg);

  if (!ExtractMI || !isGenericExtractOpcode(ExtractMI->getOpcode(), TII))
    return false;

  // Verify extraction is from position 0
  const Register ExtractIdxReg = ExtractMI->getOperand(2).getReg();
  auto ExtractIdx = getIConstantVRegValWithLookThrough(ExtractIdxReg, MRI);
  if (!ExtractIdx || ExtractIdx->Value.getZExtValue() != 0)
    return false;

  // Get the source vector that was extracted from
  const Register ExtractSrcVecReg = ExtractMI->getOperand(1).getReg();
  const LLT ExtractSrcVecTy = MRI.getType(ExtractSrcVecReg);
  const LLT BroadcastDstTy = MRI.getType(MI.getOperand(0).getReg());

  // Types must match exactly
  if (ExtractSrcVecTy != BroadcastDstTy)
    return false;

  // 2. Verify all uses through the chain only extract position 0
  //    using the helper function with single-use checks
  const Register BroadcastDstReg = MI.getOperand(0).getReg();
  if (!verifyBroadcastUsesOnlyExtractZero(BroadcastDstReg, MRI, TII))
    return false;

  MatchInfo = [ExtractSrcVecReg, BroadcastDstReg](MachineIRBuilder &B) {
    B.buildCopy(BroadcastDstReg, ExtractSrcVecReg);
  };

  return true;
}

/// Match G_AIE_VSEL with constant selection masks that can be optimized.
/// Handles three cases:
/// 1. All bits 0 -> COPY src2
/// 2. All bits 1 -> COPY src1
/// 3. Half-and-half pattern (e.g., 0xFF00 or 0x00FF) -> UNMERGE + CONCAT
bool llvm::matchVSelToUnmergeConcatOrCopy(MachineInstr &MI,
                                          MachineRegisterInfo &MRI,
                                          const AIEBaseInstrInfo &TII,
                                          BuildFnTy &MatchInfo) {
  assert(MI.getOpcode() == TII.getGenericVSelOpcode() && "Expected G_AIE_VSEL");

  const Register DstReg = MI.getOperand(0).getReg();
  const Register Src1Reg = MI.getOperand(1).getReg();
  const Register Src2Reg = MI.getOperand(2).getReg();
  const Register SelReg = MI.getOperand(3).getReg();

  const LLT DstTy = MRI.getType(DstReg);
  if (!DstTy.isVector())
    return false;

  // Get the selection mask constant
  auto SelCst = getIConstantVRegValWithLookThrough(SelReg, MRI);
  if (!SelCst)
    return false;

  const uint64_t SelMask = SelCst->Value.getZExtValue();
  const unsigned NumElements = DstTy.getNumElements();

  // Create a mask with all bits set for the number of elements
  const uint64_t AllOnesMask = (1ULL << NumElements) - 1;

  // Case 1: All bits are 0 -> select all from src1
  if (SelMask == 0) {
    MatchInfo = [Src1Reg, DstReg](MachineIRBuilder &B) {
      B.buildCopy(DstReg, Src1Reg);
    };
    return true;
  }

  // Case 2: All bits are 1 -> select all from src2
  if (SelMask == AllOnesMask) {
    MatchInfo = [Src2Reg, DstReg](MachineIRBuilder &B) {
      B.buildCopy(DstReg, Src2Reg);
    };
    return true;
  }

  // Case 3: Half-and-half pattern
  // Check if the mask represents selecting lower half from one source
  // and upper half from another source
  const unsigned HalfElements = NumElements / 2;
  const uint64_t LowerHalfMask = (1ULL << HalfElements) - 1;
  const uint64_t UpperHalfMask = AllOnesMask & ~LowerHalfMask;

  // Check if it's either half-and-half pattern
  if (SelMask == UpperHalfMask || SelMask == LowerHalfMask) {
    const bool IsUpperMask = (SelMask == UpperHalfMask);

    MatchInfo = [Src1Reg, Src2Reg, DstReg, DstTy, IsUpperMask,
                 &MRI](MachineIRBuilder &B) {
      const LLT HalfTy = DstTy.divide(2);

      // Unmerge both sources into halves
      const Register Src1Lo = MRI.createGenericVirtualRegister(HalfTy);
      const Register Src1Hi = MRI.createGenericVirtualRegister(HalfTy);
      B.buildUnmerge({Src1Lo, Src1Hi}, Src1Reg);

      const Register Src2Lo = MRI.createGenericVirtualRegister(HalfTy);
      const Register Src2Hi = MRI.createGenericVirtualRegister(HalfTy);
      B.buildUnmerge({Src2Lo, Src2Hi}, Src2Reg);

      // UpperHalfMask (0xFF00): lo from src1, hi from src2
      // LowerHalfMask (0x00FF): lo from src2, hi from src1
      const Register LowerHalf = IsUpperMask ? Src1Lo : Src2Lo;
      const Register UpperHalf = IsUpperMask ? Src2Hi : Src1Hi;

      B.buildConcatVectors(DstReg, {LowerHalf, UpperHalf});
    };
    return true;
  }

  return false;
}

//===----------------------------------------------------------------------===//
// combine_alternating_build_vector
//===----------------------------------------------------------------------===//

bool llvm::matchAlternatingBuildVector(
    MachineInstr &MI, MachineRegisterInfo &MRI, const AIEBaseInstrInfo &TII,
    AIEAlternatingBuildVectorMatchData &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_BUILD_VECTOR &&
         "Expected a G_BUILD_VECTOR");

  const Register DstVecReg = MI.getOperand(0).getReg();
  const LLT DstVecTy = MRI.getType(DstVecReg);

  // Only handle vectors >= 128 bits. Smaller vectors are kept in GPRs.
  if (!DstVecTy.isVector() || DstVecTy.getSizeInBits() < 128)
    return false;

  const unsigned ElemBits = DstVecTy.getElementType().getSizeInBits();
  const unsigned NumElts = DstVecTy.getNumElements();

  // Detect the minimum repeating period using a single forward pass.
  // This mirrors the sawtooth pattern used for vshuffle masks (MaskMatch):
  // the i-th operand's "index" within the period equals i % Period.
  // We start at Period=1 and double whenever a mismatch falls exactly at the
  // period boundary (i.e., the first position that would need to repeat).
  unsigned Period = 1;
  for (unsigned I = 1; I < NumElts; ++I) {
    if (MI.getOperand(1 + I).getReg() ==
        MI.getOperand(1 + (I % Period)).getReg())
      continue;
    // A mismatch anywhere other than the period boundary means the sequence
    // is not a sawtooth-style repetition; bail out.
    if (I != Period)
      return false;
    // Double the period and check that the merged scalar stays within 32 bits.
    Period *= 2;
    // For now, only handle packed scalars of at most 32 bits.
    if (Period * ElemBits > 32)
      return false;
  }

  // Period==1 means all operands are identical - a pure splat. Those are
  // handled more efficiently by matchSplatVector.
  if (Period == 1)
    return false;

  MatchInfo.DstVecReg = DstVecReg;
  MatchInfo.PeriodElts.clear();
  for (unsigned I = 0; I < Period; ++I)
    MatchInfo.PeriodElts.push_back(MI.getOperand(1 + I).getReg());
  MatchInfo.MergedBits = Period * ElemBits;
  return true;
}

void llvm::applyAlternatingBuildVector(
    MachineInstr &MI, MachineRegisterInfo &MRI, MachineIRBuilder &B,
    AIEAlternatingBuildVectorMatchData &MatchInfo,
    GISelChangeObserver &Observer) {
  B.setInstrAndDebugLoc(MI);

  const Register DstVecReg = MatchInfo.DstVecReg;
  const LLT DstVecTy = MRI.getType(DstVecReg);
  const unsigned NumElts = DstVecTy.getNumElements();
  const unsigned ElemBits = DstVecTy.getElementType().getSizeInBits();
  const unsigned Period = MatchInfo.PeriodElts.size();
  const unsigned MergedBits = MatchInfo.MergedBits;
  const LLT MergedScalarTy = LLT::scalar(MergedBits);

  // Step 1: Pack the Period elements into a merged scalar using Horner's
  // scheme, evaluating from the highest element down to elem[0]:
  //   packed = ((...((elem[P-1] << B) | elem[P-2]) << B | ...) << B) | elem[0]
  //
  // This reuses a single shift constant at every step.  The top element
  // (PeriodElts[Period-1]) only needs G_ANYEXT: after (Period-1) left shifts
  // its upper bits overflow MergedBits and are naturally discarded.  All
  // lower elements need G_ZEXT to avoid polluting the bits above them.
  Register ShiftConst = B.buildConstant(MergedScalarTy, ElemBits).getReg(0);
  Register PackedReg =
      B.buildAnyExt(MergedScalarTy, MatchInfo.PeriodElts[Period - 1]).getReg(0);
  for (int I = (int)Period - 2; I >= 0; --I) {
    Register Shifted =
        B.buildShl(MergedScalarTy, PackedReg, ShiftConst).getReg(0);
    Register ExtElt =
        B.buildZExt(MergedScalarTy, MatchInfo.PeriodElts[I]).getReg(0);
    PackedReg = B.buildOr(MergedScalarTy, Shifted, ExtElt).getReg(0);
  }

  // Step 2: Broadcast the merged scalar into an intermediate vector whose
  // element type is MergedBits. The intermediate vector has the same total
  // bit-width as the destination.
  const unsigned NumGroups = NumElts / Period; // = DstBits / MergedBits
  const LLT IntermediateVecTy = LLT::fixed_vector(NumGroups, MergedBits);
  Register IntermediateVecReg =
      MRI.createGenericVirtualRegister(IntermediateVecTy);
  buildBroadcastVector(B, MRI, PackedReg, IntermediateVecReg);

  // Step 3: Bitcast the intermediate vector back to the original destination
  // type (<NumElts x ElemBits>).
  if (IntermediateVecTy == DstVecTy)
    B.buildCopy(DstVecReg, IntermediateVecReg);
  else
    B.buildBitcast(DstVecReg, IntermediateVecReg);

  Observer.erasingInstr(MI);
  MI.eraseFromParent();
}
