//===- AIEMachineBundleUtilsTest.cpp --------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "Utils/AIEMachineBundleUtils.h"
#include "AIEBaseInstrInfo.h"
#include "llvm/CodeGen/MIRParser/MIRParser.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "gtest/gtest.h"

using namespace llvm;
using namespace llvm::AIEMachineBundleUtils;

namespace {

std::unique_ptr<TargetMachine> createTargetMachine() {
  Triple TT("aie2ps--");
  std::string Error;
  const Target *TheTarget = TargetRegistry::lookupTarget(TT, Error);
  if (!TheTarget) {
    llvm::errs() << Error << "\n";
    return nullptr;
  }
  return std::unique_ptr<TargetMachine>(
      TheTarget->createTargetMachine(TT, "", "", TargetOptions(), std::nullopt,
                                     std::nullopt, CodeGenOptLevel::Default));
}

class AIEMachineBundleUtilsTest : public testing::Test {
protected:
  LLVMContext Context;
  std::unique_ptr<TargetMachine> TM;
  std::unique_ptr<MachineModuleInfo> MMI;
  std::unique_ptr<MIRParser> Parser;
  std::unique_ptr<Module> M;

  static void SetUpTestCase() {
    LLVMInitializeAIETargetInfo();
    LLVMInitializeAIETarget();
    LLVMInitializeAIETargetMC();
  }

  void parseMIR(const char *MIRString) {
    TM = createTargetMachine();
    if (!TM)
      GTEST_SKIP() << "AIE target not available";

    std::unique_ptr<MemoryBuffer> MBuffer =
        MemoryBuffer::getMemBuffer(MIRString);
    Parser = createMIRParser(std::move(MBuffer), Context);
    if (!Parser)
      report_fatal_error("null MIRParser");
    M = Parser->parseIRModule();
    if (!M)
      report_fatal_error("parseIRModule failed");
    M->setTargetTriple(TM->getTargetTriple());
    M->setDataLayout(TM->createDataLayout());
    MMI = std::make_unique<MachineModuleInfo>(TM.get());
    if (Parser->parseMachineFunctions(*M, *MMI.get()))
      report_fatal_error("parseMachineFunctions failed");
  }

  MachineFunction *getMachineFunction(StringRef Name) {
    auto F = M->getFunction(Name);
    if (!F)
      report_fatal_error("null Function");
    return &MMI->getOrCreateMachineFunction(*F);
  }

  MachineBasicBlock *getMBB(MachineFunction *MF, unsigned Num) {
    return MF->getBlockNumbered(Num);
  }

  const AIEBaseInstrInfo *getTII(MachineFunction *MF) {
    return static_cast<const AIEBaseInstrInfo *>(
        MF->getSubtarget().getInstrInfo());
  }

  /// Extract bundles from an MBB using TII.getAIEMachineBundle()
  std::vector<AIE::MachineBundle>
  getBundlesFromMBB(MachineBasicBlock &MBB, const AIEBaseInstrInfo &TII) {
    std::vector<AIE::MachineBundle> Bundles;
    for (auto MI = MBB.begin(); MI != MBB.end(); ++MI) {
      if (MI->isBundle()) {
        Bundles.push_back(TII.getAIEMachineBundle(MI));
      } else if (!MI->isTerminator() && !MI->isDebugInstr()) {
        // Single instruction not in a bundle - create a bundle with it
        AIE::MachineBundle Bundle(TII.getFormatInterface());
        Bundle.add(&*MI);
        Bundles.push_back(std::move(Bundle));
      }
    }
    return Bundles;
  }
};

// MIR with two MBBs containing identical multi-instruction bundles (matching
// bundles) Each bundle contains a scalar ADD and a vector load (VLDA) -
// different functional units
const char *MatchingBundlesMIR = R"MIR(
--- |
  define void @matching_bundles() {
  entry:
    br label %bb1

  bb1:
    br label %bb2

  bb2:
    ret void
  }

...
---
name:            matching_bundles
tracksRegLiveness: true
body:             |
  bb.0.entry:
    successors: %bb.1
    liveins: $r0, $r1, $p0, $p1
    BUNDLE implicit-def $r2, implicit-def $wl0, implicit-def $srcarry, implicit $r0, implicit $p0 {
      $r2 = ADD_add_r_ri $r0, 1, implicit-def $srcarry
      $wl0 = VLDA_dmw_lda_w_ld_idx_imm $p0, 0
    }
    BUNDLE implicit-def $r3, implicit-def $wl1, implicit-def $srcarry, implicit $r1, implicit $p1 {
      $r3 = ADD_add_r_ri $r1, 2, implicit-def $srcarry
      $wl1 = VLDA_dmw_lda_w_ld_idx_imm $p1, 0
    }

  bb.1.bb1:
    successors: %bb.2
    liveins: $r0, $r1, $p0, $p1
    BUNDLE implicit-def $r2, implicit-def $wl0, implicit-def $srcarry, implicit $r0, implicit $p0 {
      $r2 = ADD_add_r_ri $r0, 1, implicit-def $srcarry
      $wl0 = VLDA_dmw_lda_w_ld_idx_imm $p0, 0
    }
    BUNDLE implicit-def $r3, implicit-def $wl1, implicit-def $srcarry, implicit $r1, implicit $p1 {
      $r3 = ADD_add_r_ri $r1, 2, implicit-def $srcarry
      $wl1 = VLDA_dmw_lda_w_ld_idx_imm $p1, 0
    }

  bb.2.bb2:
    RET implicit $lr
    DelayedSchedBarrier

...
)MIR";

// MIR with two MBBs where first multi-instruction bundle matches but second
// doesn't First bundle: ADD + VLDA (matches in both MBBs) Second bundle: ADD +
// NOP vs ADD + VLDA (different)
const char *PartialMatchMIR = R"MIR(
--- |
  define void @partial_match() {
  entry:
    br label %bb1

  bb1:
    br label %bb2

  bb2:
    ret void
  }

...
---
name:            partial_match
tracksRegLiveness: true
body:             |
  bb.0.entry:
    successors: %bb.1
    liveins: $r0, $r1, $p0
    BUNDLE implicit-def $r2, implicit-def $wl0, implicit-def $srcarry, implicit $r0, implicit $p0 {
      $r2 = ADD_add_r_ri $r0, 1, implicit-def $srcarry
      $wl0 = VLDA_dmw_lda_w_ld_idx_imm $p0, 0
    }
    BUNDLE implicit-def $r3, implicit-def $srcarry, implicit $r1 {
      $r3 = ADD_add_r_ri $r1, 2, implicit-def $srcarry
      NOP
    }

  bb.1.bb1:
    successors: %bb.2
    liveins: $r0, $r1, $p0, $p1
    BUNDLE implicit-def $r2, implicit-def $wl0, implicit-def $srcarry, implicit $r0, implicit $p0 {
      $r2 = ADD_add_r_ri $r0, 1, implicit-def $srcarry
      $wl0 = VLDA_dmw_lda_w_ld_idx_imm $p0, 0
    }
    BUNDLE implicit-def $r3, implicit-def $wl1, implicit-def $srcarry, implicit $r1, implicit $p1 {
      $r3 = ADD_add_r_ri $r1, 2, implicit-def $srcarry
      $wl1 = VLDA_dmw_lda_w_ld_idx_imm $p1, 0
    }

  bb.2.bb2:
    RET implicit $lr
    DelayedSchedBarrier

...
)MIR";

// MIR with two MBBs with different multi-instruction bundles from the start
// bb.0: NOP + NOP (uses different functional units than scalar+vector)
// bb.1: ADD + VLDA
const char *NoMatchMIR = R"MIR(
--- |
  define void @no_match() {
  entry:
    br label %bb1

  bb1:
    br label %bb2

  bb2:
    ret void
  }

...
---
name:            no_match
tracksRegLiveness: true
body:             |
  bb.0.entry:
    successors: %bb.1
    liveins: $r0, $r1, $p0
    BUNDLE implicit-def $r2, implicit-def $srcarry, implicit $r0 {
      $r2 = ADD_add_r_ri $r0, 1, implicit-def $srcarry
      NOP
    }

  bb.1.bb1:
    successors: %bb.2
    liveins: $r0, $r1, $p0
    BUNDLE implicit-def $r2, implicit-def $wl0, implicit-def $srcarry, implicit $r0, implicit $p0 {
      $r2 = ADD_add_r_ri $r0, 1, implicit-def $srcarry
      $wl0 = VLDA_dmw_lda_w_ld_idx_imm $p0, 0
    }

  bb.2.bb2:
    RET implicit $lr
    DelayedSchedBarrier

...
)MIR";

TEST_F(AIEMachineBundleUtilsTest, CountMatchingLeadingBundles_EmptyBundles) {
  std::vector<AIE::MachineBundle> EmptyA;
  std::vector<AIE::MachineBundle> EmptyB;

  unsigned Count = countMatchingLeadingBundles(EmptyA, EmptyB);
  EXPECT_EQ(Count, 0u);
}

TEST_F(AIEMachineBundleUtilsTest, CountMatchingLeadingBundles_FullMatch) {
  parseMIR(MatchingBundlesMIR);
  MachineFunction *MF = getMachineFunction("matching_bundles");
  const AIEBaseInstrInfo *TII = getTII(MF);

  // bb.0 and bb.1 have identical instructions
  MachineBasicBlock *MBB0 = getMBB(MF, 0);
  MachineBasicBlock *MBB1 = getMBB(MF, 1);

  std::vector<AIE::MachineBundle> BundlesA = getBundlesFromMBB(*MBB0, *TII);
  std::vector<AIE::MachineBundle> BundlesB = getBundlesFromMBB(*MBB1, *TII);

  ASSERT_FALSE(BundlesA.empty());
  ASSERT_FALSE(BundlesB.empty());

  unsigned Count = countMatchingLeadingBundles(BundlesA, BundlesB);
  // All bundles should match
  EXPECT_EQ(Count, std::min(BundlesA.size(), BundlesB.size()));
}

TEST_F(AIEMachineBundleUtilsTest, CountMatchingLeadingBundles_PartialMatch) {
  parseMIR(PartialMatchMIR);
  MachineFunction *MF = getMachineFunction("partial_match");
  const AIEBaseInstrInfo *TII = getTII(MF);

  // bb.0 has ADD, SUB; bb.1 has ADD, ADD
  // First instruction matches (ADD), second doesn't (SUB vs ADD)
  MachineBasicBlock *MBB0 = getMBB(MF, 0);
  MachineBasicBlock *MBB1 = getMBB(MF, 1);

  std::vector<AIE::MachineBundle> BundlesA = getBundlesFromMBB(*MBB0, *TII);
  std::vector<AIE::MachineBundle> BundlesB = getBundlesFromMBB(*MBB1, *TII);

  ASSERT_GE(BundlesA.size(), 2u);
  ASSERT_GE(BundlesB.size(), 2u);

  unsigned Count = countMatchingLeadingBundles(BundlesA, BundlesB);
  // Only first bundle should match
  EXPECT_EQ(Count, 1u);
}

TEST_F(AIEMachineBundleUtilsTest, CountMatchingLeadingBundles_NoMatch) {
  parseMIR(NoMatchMIR);
  MachineFunction *MF = getMachineFunction("no_match");
  const AIEBaseInstrInfo *TII = getTII(MF);

  // bb.0 has SUB; bb.1 has ADD - different from the start
  MachineBasicBlock *MBB0 = getMBB(MF, 0);
  MachineBasicBlock *MBB1 = getMBB(MF, 1);

  std::vector<AIE::MachineBundle> BundlesA = getBundlesFromMBB(*MBB0, *TII);
  std::vector<AIE::MachineBundle> BundlesB = getBundlesFromMBB(*MBB1, *TII);

  ASSERT_FALSE(BundlesA.empty());
  ASSERT_FALSE(BundlesB.empty());

  unsigned Count = countMatchingLeadingBundles(BundlesA, BundlesB);
  EXPECT_EQ(Count, 0u);
}

TEST_F(AIEMachineBundleUtilsTest, CountMatchingLeadingBundles_FilterStops) {
  parseMIR(MatchingBundlesMIR);
  MachineFunction *MF = getMachineFunction("matching_bundles");
  const AIEBaseInstrInfo *TII = getTII(MF);

  MachineBasicBlock *MBB0 = getMBB(MF, 0);
  MachineBasicBlock *MBB1 = getMBB(MF, 1);

  std::vector<AIE::MachineBundle> BundlesA = getBundlesFromMBB(*MBB0, *TII);
  std::vector<AIE::MachineBundle> BundlesB = getBundlesFromMBB(*MBB1, *TII);

  ASSERT_FALSE(BundlesA.empty());
  ASSERT_FALSE(BundlesB.empty());

  // Filter that always returns true - should stop immediately
  auto AlwaysStopFilter = [](const MachineInstr &) { return true; };

  unsigned Count =
      countMatchingLeadingBundles(BundlesA, BundlesB, AlwaysStopFilter);
  EXPECT_EQ(Count, 0u);
}

TEST_F(AIEMachineBundleUtilsTest,
       CountMatchingLeadingBundles_FilterNeverStops) {
  parseMIR(MatchingBundlesMIR);
  MachineFunction *MF = getMachineFunction("matching_bundles");
  const AIEBaseInstrInfo *TII = getTII(MF);

  MachineBasicBlock *MBB0 = getMBB(MF, 0);
  MachineBasicBlock *MBB1 = getMBB(MF, 1);

  std::vector<AIE::MachineBundle> BundlesA = getBundlesFromMBB(*MBB0, *TII);
  std::vector<AIE::MachineBundle> BundlesB = getBundlesFromMBB(*MBB1, *TII);

  ASSERT_FALSE(BundlesA.empty());
  ASSERT_FALSE(BundlesB.empty());

  // Filter that never stops - should match all
  auto NeverStopFilter = [](const MachineInstr &) { return false; };

  unsigned Count =
      countMatchingLeadingBundles(BundlesA, BundlesB, NeverStopFilter);
  EXPECT_EQ(Count, std::min(BundlesA.size(), BundlesB.size()));
}

//===----------------------------------------------------------------------===//
// Tests for mergeBundles()
//===----------------------------------------------------------------------===//

// MIR for merge tests: 4 bundles in bb.0 (dst/epilogue), 2 bundles in bb.1
// (src/prologue) bb.0: [D0: ADD+VLDA, D1: ADD+VLDA, D2: ADD only (vector slot
// empty), D3: ADD only (vector slot empty)] bb.1: [S0: VLDA only (scalar slot
// empty), S1: VLDA only (scalar slot empty)] With NumBundlesToMerge=2: Result
// should be [D0, D1, D2(ADD)+S0(VLDA), D3(ADD)+S1(VLDA)] This tests that scalar
// ADD from dst merges with vector VLDA from src without slot conflicts.
const char *MergeBundlesMIR = R"MIR(
--- |
  define void @merge_bundles() {
  entry:
    br label %bb1

  bb1:
    br label %bb2

  bb2:
    ret void
  }

...
---
name:            merge_bundles
tracksRegLiveness: true
body:             |
  bb.0.entry:
    successors: %bb.1
    liveins: $r0, $r1, $r2, $r3, $p0, $p1
    ; D0: ADD + VLDA (full bundle)
    BUNDLE implicit-def $r4, implicit-def $wl0, implicit-def $srcarry, implicit $r0, implicit $p0 {
      $r4 = ADD_add_r_ri $r0, 1, implicit-def $srcarry
      $wl0 = VLDA_dmw_lda_w_ld_idx_imm $p0, 0
    }
    ; D1: ADD + VLDA (full bundle)
    BUNDLE implicit-def $r5, implicit-def $wl1, implicit-def $srcarry, implicit $r1, implicit $p1 {
      $r5 = ADD_add_r_ri $r1, 2, implicit-def $srcarry
      $wl1 = VLDA_dmw_lda_w_ld_idx_imm $p1, 0
    }
    ; D2: ADD only - vector slot is empty (will merge with S0 VLDA)
    BUNDLE implicit-def $r6, implicit-def $srcarry, implicit $r2 {
      $r6 = ADD_add_r_ri $r2, 3, implicit-def $srcarry
    }
    ; D3: ADD only - vector slot is empty (will merge with S1 VLDA)
    BUNDLE implicit-def $r7, implicit-def $srcarry, implicit $r3 {
      $r7 = ADD_add_r_ri $r3, 4, implicit-def $srcarry
    }

  bb.1.bb1:
    successors: %bb.2
    liveins: $p2, $p3
    ; S0: VLDA only - scalar slot is empty (will merge into D2)
    BUNDLE implicit-def $wl2, implicit $p2 {
      $wl2 = VLDA_dmw_lda_w_ld_idx_imm $p2, 0
    }
    ; S1: VLDA only - scalar slot is empty (will merge into D3)
    BUNDLE implicit-def $wl3, implicit $p3 {
      $wl3 = VLDA_dmw_lda_w_ld_idx_imm $p3, 0
    }

  bb.2.bb2:
    RET implicit $lr
    DelayedSchedBarrier

...
)MIR";

TEST_F(AIEMachineBundleUtilsTest, MergeBundles_BasicMerge) {
  parseMIR(MergeBundlesMIR);
  MachineFunction *MF = getMachineFunction("merge_bundles");
  const AIEBaseInstrInfo *TII = getTII(MF);

  MachineBasicBlock *MBB0 = getMBB(MF, 0); // dst/epilogue: 4 bundles
  MachineBasicBlock *MBB1 = getMBB(MF, 1); // src/prologue: 2 bundles

  std::vector<AIE::MachineBundle> DstBundles = getBundlesFromMBB(*MBB0, *TII);
  std::vector<AIE::MachineBundle> SrcBundles = getBundlesFromMBB(*MBB1, *TII);

  ASSERT_EQ(DstBundles.size(), 4u);
  ASSERT_EQ(SrcBundles.size(), 2u);

  const AIEBaseMCFormats *FormatInterface = TII->getFormatInterface();
  unsigned NopOpc = FormatInterface->getSlotInfo(0)->getNOPOpcode();

  // Merge last 2 dst bundles with first 2 src bundles
  // D2 (ADD only) + S0 (VLDA only) → ADD + VLDA
  // D3 (ADD only) + S1 (VLDA only) → ADD + VLDA
  std::vector<AIE::MachineBundle> Merged =
      mergeBundles(*MF, DstBundles, SrcBundles, 2, FormatInterface, NopOpc);

  // Should have same number of bundles as DstBundles
  ASSERT_EQ(Merged.size(), 4u);

  // First two bundles (D0, D1) keep their original instructions (ADD + VLDA =
  // 2)
  EXPECT_EQ(Merged[0].size(), 2u); // ADD + VLDA
  EXPECT_EQ(Merged[1].size(), 2u); // ADD + VLDA

  // Last two bundles (D2+S0, D3+S1) should have merged instructions
  // D2 had ADD, S0 had VLDA → merged has ADD + VLDA = 2
  // D3 had ADD, S1 had VLDA → merged has ADD + VLDA = 2
  EXPECT_EQ(Merged[2].size(), 2u); // ADD (from D2) + VLDA (from S0)
  EXPECT_EQ(Merged[3].size(), 2u); // ADD (from D3) + VLDA (from S1)
}

TEST_F(AIEMachineBundleUtilsTest, MergeBundles_ZeroMerge) {
  parseMIR(MergeBundlesMIR);
  MachineFunction *MF = getMachineFunction("merge_bundles");
  const AIEBaseInstrInfo *TII = getTII(MF);

  MachineBasicBlock *MBB0 = getMBB(MF, 0);
  MachineBasicBlock *MBB1 = getMBB(MF, 1);

  std::vector<AIE::MachineBundle> DstBundles = getBundlesFromMBB(*MBB0, *TII);
  std::vector<AIE::MachineBundle> SrcBundles = getBundlesFromMBB(*MBB1, *TII);

  ASSERT_EQ(DstBundles.size(), 4u);
  ASSERT_EQ(SrcBundles.size(), 2u);

  const AIEBaseMCFormats *FormatInterface = TII->getFormatInterface();
  unsigned NopOpc = FormatInterface->getSlotInfo(0)->getNOPOpcode();

  // Zero merge - should return clones of DstBundles unchanged
  std::vector<AIE::MachineBundle> Merged =
      mergeBundles(*MF, DstBundles, SrcBundles, 0, FormatInterface, NopOpc);

  // Should have same number of bundles as DstBundles
  ASSERT_EQ(Merged.size(), 4u);

  // No source bundles should be merged (all SrcIdx will be negative)
  // Each bundle just contains cloned dst instructions
  EXPECT_EQ(Merged[0].size(), 2u); // ADD + VLDA
  EXPECT_EQ(Merged[1].size(), 2u); // ADD + VLDA
  EXPECT_EQ(Merged[2].size(), 1u); // Only ADD
  EXPECT_EQ(Merged[3].size(), 1u); // Only ADD
}

// Test verifying instruction cloning in merged bundles
TEST_F(AIEMachineBundleUtilsTest, MergeBundles_ClonesInstructions) {
  parseMIR(MergeBundlesMIR);
  MachineFunction *MF = getMachineFunction("merge_bundles");
  const AIEBaseInstrInfo *TII = getTII(MF);

  MachineBasicBlock *MBB0 = getMBB(MF, 0);
  MachineBasicBlock *MBB1 = getMBB(MF, 1);

  std::vector<AIE::MachineBundle> DstBundles = getBundlesFromMBB(*MBB0, *TII);
  std::vector<AIE::MachineBundle> SrcBundles = getBundlesFromMBB(*MBB1, *TII);

  ASSERT_EQ(DstBundles.size(), 4u);
  ASSERT_EQ(SrcBundles.size(), 2u);

  const AIEBaseMCFormats *FormatInterface = TII->getFormatInterface();
  unsigned NopOpc = FormatInterface->getSlotInfo(0)->getNOPOpcode();

  // Merge and verify instructions are cloned (different pointers)
  std::vector<AIE::MachineBundle> Merged =
      mergeBundles(*MF, DstBundles, SrcBundles, 2, FormatInterface, NopOpc);

  ASSERT_EQ(Merged.size(), 4u);

  // Get instruction pointers from original bundles
  const MachineInstr *OrigD0Add = DstBundles[0].getInstrs().front();
  const MachineInstr *OrigS0Vlda = SrcBundles[0].getInstrs().front();

  // Get instruction pointers from merged bundles
  const MachineInstr *MergedD0Instr = Merged[0].getInstrs().front();
  const MachineInstr *MergedD2Instr = nullptr;
  for (MachineInstr *MI : Merged[2].getInstrs()) {
    MergedD2Instr = MI;
    break;
  }

  // Instructions should be cloned (different addresses)
  EXPECT_NE(OrigD0Add, MergedD0Instr);
  EXPECT_NE(OrigS0Vlda, MergedD2Instr);

  // But opcodes should match
  EXPECT_EQ(OrigD0Add->getOpcode(), MergedD0Instr->getOpcode());
}

//===----------------------------------------------------------------------===//
// Tests for removeLeadingBundles()
//===----------------------------------------------------------------------===//

TEST_F(AIEMachineBundleUtilsTest, RemoveLeadingBundles_RemoveTwo) {
  parseMIR(MergeBundlesMIR);
  MachineFunction *MF = getMachineFunction("merge_bundles");
  const AIEBaseInstrInfo *TII = getTII(MF);

  MachineBasicBlock *MBB0 = getMBB(MF, 0); // 4 bundles

  std::vector<AIE::MachineBundle> OriginalBundles =
      getBundlesFromMBB(*MBB0, *TII);
  ASSERT_EQ(OriginalBundles.size(), 4u);

  // Remove first 2 bundles - should leave 2 remaining
  std::vector<AIE::MachineBundle> Remaining =
      removeLeadingBundles(*MBB0, OriginalBundles, 2, *TII);

  // Should have 2 remaining bundles
  ASSERT_EQ(Remaining.size(), 2u);

  // The remaining bundles should have the same instruction counts as
  // the original bundles[2] and bundles[3]
  EXPECT_EQ(Remaining[0].size(), OriginalBundles[2].size());
  EXPECT_EQ(Remaining[1].size(), OriginalBundles[3].size());
}

TEST_F(AIEMachineBundleUtilsTest, RemoveLeadingBundles_RemoveZero) {
  parseMIR(MergeBundlesMIR);
  MachineFunction *MF = getMachineFunction("merge_bundles");
  const AIEBaseInstrInfo *TII = getTII(MF);

  MachineBasicBlock *MBB0 = getMBB(MF, 0); // 4 bundles

  std::vector<AIE::MachineBundle> OriginalBundles =
      getBundlesFromMBB(*MBB0, *TII);
  ASSERT_EQ(OriginalBundles.size(), 4u);

  // Remove 0 bundles - should return all original bundles
  std::vector<AIE::MachineBundle> Remaining =
      removeLeadingBundles(*MBB0, OriginalBundles, 0, *TII);

  // Should have all 4 bundles
  ASSERT_EQ(Remaining.size(), 4u);
}

TEST_F(AIEMachineBundleUtilsTest, RemoveLeadingBundles_RemoveAll) {
  parseMIR(MergeBundlesMIR);
  MachineFunction *MF = getMachineFunction("merge_bundles");
  const AIEBaseInstrInfo *TII = getTII(MF);

  MachineBasicBlock *MBB1 = getMBB(MF, 1); // 2 bundles

  std::vector<AIE::MachineBundle> OriginalBundles =
      getBundlesFromMBB(*MBB1, *TII);
  ASSERT_EQ(OriginalBundles.size(), 2u);

  // Remove all bundles
  std::vector<AIE::MachineBundle> Remaining =
      removeLeadingBundles(*MBB1, OriginalBundles, 2, *TII);

  // Should have 0 remaining bundles
  ASSERT_EQ(Remaining.size(), 0u);
}

//===----------------------------------------------------------------------===//
// Tests for transferLeadingBundles()
//===----------------------------------------------------------------------===//

TEST_F(AIEMachineBundleUtilsTest, TransferLeadingBundles_TransferOne) {
  parseMIR(MergeBundlesMIR);
  MachineFunction *MF = getMachineFunction("merge_bundles");
  const AIEBaseInstrInfo *TII = getTII(MF);

  MachineBasicBlock *MBB0 = getMBB(MF, 0); // 4 bundles - destination
  MachineBasicBlock *MBB1 = getMBB(MF, 1); // 2 bundles - source

  std::vector<AIE::MachineBundle> SrcBundles = getBundlesFromMBB(*MBB1, *TII);
  ASSERT_EQ(SrcBundles.size(), 2u);

  // Transfer 1 bundle from MBB1 to MBB0
  auto [Transferred, Remaining] =
      transferLeadingBundles(*MBB0, *MBB1, SrcBundles, 1, *TII);

  // Should have 1 transferred bundle
  ASSERT_EQ(Transferred.size(), 1u);

  // Should have 1 remaining bundle in source
  ASSERT_EQ(Remaining.size(), 1u);

  // The remaining bundle should have same instruction count as original
  // SrcBundles[1]
  EXPECT_EQ(Remaining[0].size(), SrcBundles[1].size());
}

TEST_F(AIEMachineBundleUtilsTest, TransferLeadingBundles_TransferZero) {
  parseMIR(MergeBundlesMIR);
  MachineFunction *MF = getMachineFunction("merge_bundles");
  const AIEBaseInstrInfo *TII = getTII(MF);

  MachineBasicBlock *MBB0 = getMBB(MF, 0);
  MachineBasicBlock *MBB1 = getMBB(MF, 1);

  std::vector<AIE::MachineBundle> SrcBundles = getBundlesFromMBB(*MBB1, *TII);
  ASSERT_EQ(SrcBundles.size(), 2u);

  // Transfer 0 bundles - should return all original source bundles
  auto [Transferred, Remaining] =
      transferLeadingBundles(*MBB0, *MBB1, SrcBundles, 0, *TII);

  // Should have 0 transferred bundles
  ASSERT_EQ(Transferred.size(), 0u);

  // Should have all 2 bundles remaining
  ASSERT_EQ(Remaining.size(), 2u);
}

TEST_F(AIEMachineBundleUtilsTest, TransferLeadingBundles_TransferAll) {
  parseMIR(MergeBundlesMIR);
  MachineFunction *MF = getMachineFunction("merge_bundles");
  const AIEBaseInstrInfo *TII = getTII(MF);

  MachineBasicBlock *MBB0 = getMBB(MF, 0); // 4 bundles
  MachineBasicBlock *MBB1 = getMBB(MF, 1); // 2 bundles

  std::vector<AIE::MachineBundle> SrcBundles = getBundlesFromMBB(*MBB1, *TII);
  ASSERT_EQ(SrcBundles.size(), 2u);

  // Transfer all bundles from MBB1 to MBB0
  auto [Transferred, Remaining] =
      transferLeadingBundles(*MBB0, *MBB1, SrcBundles, 2, *TII);

  // Should have 2 transferred bundles
  ASSERT_EQ(Transferred.size(), 2u);

  // Should have 0 remaining bundles in source
  ASSERT_EQ(Remaining.size(), 0u);
}

} // anonymous namespace
