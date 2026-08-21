//===- AIELoopUtilsTest.cpp -----------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "Utils/AIELoopUtils.h"
#include "llvm/CodeGen/MIRParser/MIRParser.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "gtest/gtest.h"

using namespace llvm;
using namespace llvm::AIELoopUtils;

namespace {

std::unique_ptr<TargetMachine> createTargetMachine() {
  auto TT(Triple::normalize("aie2ps--"));
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

class AIELoopUtilsTest : public testing::Test {
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
};

// MIR with OLP structure: OuterPreheader -> SteadyTop -> SteadyInner ->
// SteadyBottom -> PeeledIterTop -> PeeledIterInner -> PeeledIterBottom
// The outer latch (bb.3) has the OLP success metadata.
const char *NonSpeculativeMIR = R"MIR(
--- |
  define void @olp_non_speculative() {
  entry:
    br label %outer.preheader

  outer.preheader:
    br label %steady.top

  steady.top:
    br label %steady.inner

  steady.inner:
    br i1 true, label %steady.inner, label %steady.bottom, !llvm.loop !0

  steady.bottom:
    br i1 true, label %steady.top, label %peeled.top, !llvm.loop !1

  peeled.top:
    br label %peeled.inner

  peeled.inner:
    br i1 true, label %peeled.inner, label %peeled.bottom, !llvm.loop !2

  peeled.bottom:
    ret void
  }

  !0 = distinct !{!0}
  !1 = distinct !{!1, !3}
  !2 = distinct !{!2}
  !3 = !{!"llvm.loop.hint.aie_outerloop_pipeliner_success", i64 1}

...
---
name:            olp_non_speculative
tracksRegLiveness: true
body:             |
  bb.0.entry:
    successors: %bb.1

  bb.1.outer.preheader:
    successors: %bb.2

  bb.2.steady.top:
    successors: %bb.3

  bb.3.steady.inner:
    successors: %bb.3, %bb.4
    liveins: $r0, $r3
    $r1 = ADD_add_r_ri $r0, 1, implicit-def $srcarry
    $r2 = ADD_add_r_ri $r1, 2, implicit-def $srcarry
    $r3 = ADD_add_r_ri $r3, -1, implicit-def $srcarry
    PseudoJNZ $r3, %bb.3

  bb.4.steady.bottom:
    successors: %bb.2, %bb.5
    liveins: $r0
    PseudoJNZ $r0, %bb.2

  bb.5.peeled.top:
    successors: %bb.6

  bb.6.peeled.inner:
    successors: %bb.6, %bb.7
    liveins: $r0, $r3
    $r1 = ADD_add_r_ri $r0, 1, implicit-def $srcarry
    $r2 = ADD_add_r_ri $r1, 2, implicit-def $srcarry
    $r3 = ADD_add_r_ri $r3, -1, implicit-def $srcarry
    PseudoJNZ $r3, %bb.6

  bb.7.peeled.bottom:
    RET implicit $lr
    DelayedSchedBarrier

...
)MIR";

// MIR with speculative OLP structure (no peeled iteration region)
const char *SpeculativeMIR = R"MIR(
--- |
  define void @olp_speculative() {
  entry:
    br label %outer.preheader

  outer.preheader:
    br label %steady.top

  steady.top:
    br label %steady.inner

  steady.inner:
    br i1 true, label %steady.inner, label %steady.bottom, !llvm.loop !0

  steady.bottom:
    br i1 true, label %steady.top, label %exit, !llvm.loop !1

  exit:
    ret void
  }

  !0 = distinct !{!0}
  !1 = distinct !{!1, !2, !3}
  !2 = !{!"llvm.loop.hint.aie_outerloop_pipeliner_success", i64 1}
  !3 = !{!"llvm.loop.hint.aie_outerloop_pipeliner_speculative", i64 1}

...
---
name:            olp_speculative
tracksRegLiveness: true
body:             |
  bb.0.entry:
    successors: %bb.1

  bb.1.outer.preheader:
    successors: %bb.2

  bb.2.steady.top:
    successors: %bb.3

  bb.3.steady.inner:
    successors: %bb.3, %bb.4
    liveins: $r0, $r3
    $r1 = ADD_add_r_ri $r0, 1, implicit-def $srcarry
    $r3 = ADD_add_r_ri $r3, -1, implicit-def $srcarry
    PseudoJNZ $r3, %bb.3

  bb.4.steady.bottom:
    successors: %bb.2, %bb.5
    liveins: $r0
    PseudoJNZ $r0, %bb.2

  bb.5.exit:
    RET implicit $lr
    DelayedSchedBarrier

...
)MIR";

// MIR without OLP metadata
const char *NonOLPMIR = R"MIR(
--- |
  define void @non_olp() {
  entry:
    br label %loop

  loop:
    br i1 true, label %loop, label %exit, !llvm.loop !0

  exit:
    ret void
  }

  !0 = distinct !{!0}

...
---
name:            non_olp
tracksRegLiveness: true
body:             |
  bb.0.entry:
    successors: %bb.1

  bb.1.loop:
    successors: %bb.1, %bb.2
    liveins: $r0, $r3
    $r1 = ADD_add_r_ri $r0, 1, implicit-def $srcarry
    $r3 = ADD_add_r_ri $r3, -1, implicit-def $srcarry
    PseudoJNZ $r3, %bb.1

  bb.2.exit:
    RET implicit $lr
    DelayedSchedBarrier

...
)MIR";

// MIR with mismatched inner loops (different opcodes)
const char *MismatchedInnerLoopsMIR = R"MIR(
--- |
  define void @olp_mismatched() {
  entry:
    br label %outer.preheader

  outer.preheader:
    br label %steady.top

  steady.top:
    br label %steady.inner

  steady.inner:
    br i1 true, label %steady.inner, label %steady.bottom, !llvm.loop !0

  steady.bottom:
    br i1 true, label %steady.top, label %peeled.top, !llvm.loop !1

  peeled.top:
    br label %peeled.inner

  peeled.inner:
    br i1 true, label %peeled.inner, label %peeled.bottom, !llvm.loop !2

  peeled.bottom:
    ret void
  }

  !0 = distinct !{!0}
  !1 = distinct !{!1, !3}
  !2 = distinct !{!2}
  !3 = !{!"llvm.loop.hint.aie_outerloop_pipeliner_success", i64 1}

...
---
name:            olp_mismatched
tracksRegLiveness: true
body:             |
  bb.0.entry:
    successors: %bb.1

  bb.1.outer.preheader:
    successors: %bb.2

  bb.2.steady.top:
    successors: %bb.3

  bb.3.steady.inner:
    successors: %bb.3, %bb.4
    liveins: $r0, $r3
    $r1 = ADD_add_r_ri $r0, 1, implicit-def $srcarry
    $r2 = ADD_add_r_ri $r1, 2, implicit-def $srcarry
    $r3 = ADD_add_r_ri $r3, -1, implicit-def $srcarry
    PseudoJNZ $r3, %bb.3

  bb.4.steady.bottom:
    successors: %bb.2, %bb.5
    liveins: $r0
    PseudoJNZ $r0, %bb.2

  bb.5.peeled.top:
    successors: %bb.6

  bb.6.peeled.inner:
    successors: %bb.6, %bb.7
    liveins: $r0, $r3
    NOP
    NOP
    $r3 = ADD_add_r_ri $r3, -1, implicit-def $srcarry
    PseudoJNZ $r3, %bb.6

  bb.7.peeled.bottom:
    RET implicit $lr
    DelayedSchedBarrier

...
)MIR";

TEST_F(AIELoopUtilsTest, OuterLoopStructure_BuildFromNonSpeculative) {
  parseMIR(NonSpeculativeMIR);
  MachineFunction *MF = getMachineFunction("olp_non_speculative");

  // bb.4 is the outer latch (steady.bottom)
  MachineBasicBlock *OuterLatch = getMBB(MF, 4);
  ASSERT_NE(OuterLatch, nullptr);

  auto OLS = OuterLoopStructure::tryBuildFrom(*OuterLatch);
  ASSERT_TRUE(OLS.has_value());

  // Verify all fields are populated
  EXPECT_EQ(OLS->OuterPreheader, getMBB(MF, 1));   // outer.preheader
  EXPECT_EQ(OLS->SteadyTop, getMBB(MF, 2));        // steady.top
  EXPECT_EQ(OLS->SteadyInner, getMBB(MF, 3));      // steady.inner
  EXPECT_EQ(OLS->SteadyBottom, getMBB(MF, 4));     // steady.bottom
  EXPECT_EQ(OLS->PeeledIterTop, getMBB(MF, 5));    // peeled.top
  EXPECT_EQ(OLS->PeeledIterInner, getMBB(MF, 6));  // peeled.inner
  EXPECT_EQ(OLS->PeeledIterBottom, getMBB(MF, 7)); // peeled.bottom

  EXPECT_FALSE(OLS->Speculative);
  EXPECT_TRUE(OLS->hasPeeledIterRegion());
}

TEST_F(AIELoopUtilsTest, OuterLoopStructure_BuildFromSpeculative) {
  parseMIR(SpeculativeMIR);
  MachineFunction *MF = getMachineFunction("olp_speculative");

  // bb.4 is the outer latch (steady.bottom)
  MachineBasicBlock *OuterLatch = getMBB(MF, 4);
  ASSERT_NE(OuterLatch, nullptr);

  auto OLS = OuterLoopStructure::tryBuildFrom(*OuterLatch);
  ASSERT_TRUE(OLS.has_value());

  // Steady region should be populated
  EXPECT_EQ(OLS->OuterPreheader, getMBB(MF, 1));
  EXPECT_EQ(OLS->SteadyTop, getMBB(MF, 2));
  EXPECT_EQ(OLS->SteadyInner, getMBB(MF, 3));
  EXPECT_EQ(OLS->SteadyBottom, getMBB(MF, 4));

  // Peeled region should be null (speculative mode)
  EXPECT_EQ(OLS->PeeledIterTop, nullptr);
  EXPECT_EQ(OLS->PeeledIterInner, nullptr);
  EXPECT_EQ(OLS->PeeledIterBottom, nullptr);

  EXPECT_TRUE(OLS->Speculative);
  EXPECT_FALSE(OLS->hasPeeledIterRegion());
}

TEST_F(AIELoopUtilsTest, OuterLoopStructure_NonOLPReturnsNullopt) {
  parseMIR(NonOLPMIR);
  MachineFunction *MF = getMachineFunction("non_olp");

  // bb.1 is a simple loop without OLP metadata
  MachineBasicBlock *LoopBlock = getMBB(MF, 1);
  ASSERT_NE(LoopBlock, nullptr);

  auto OLS = OuterLoopStructure::tryBuildFrom(*LoopBlock);
  EXPECT_FALSE(OLS.has_value());
}

TEST_F(AIELoopUtilsTest, OuterLoopStructure_HasMatchingInnerLoops) {
  parseMIR(NonSpeculativeMIR);
  MachineFunction *MF = getMachineFunction("olp_non_speculative");

  MachineBasicBlock *OuterLatch = getMBB(MF, 4);
  auto OLS = OuterLoopStructure::tryBuildFrom(*OuterLatch);
  ASSERT_TRUE(OLS.has_value());

  // Both inner loops have the same instructions (ADD, ADD, PseudoLoopEnd)
  EXPECT_TRUE(OLS->hasMatchingInnerLoops());
}

TEST_F(AIELoopUtilsTest, OuterLoopStructure_MismatchedInnerLoops) {
  parseMIR(MismatchedInnerLoopsMIR);
  MachineFunction *MF = getMachineFunction("olp_mismatched");

  MachineBasicBlock *OuterLatch = getMBB(MF, 4);
  auto OLS = OuterLoopStructure::tryBuildFrom(*OuterLatch);
  ASSERT_TRUE(OLS.has_value());

  // Inner loops have different opcodes (ADD vs SUB)
  EXPECT_FALSE(OLS->hasMatchingInnerLoops());
}

TEST_F(AIELoopUtilsTest, IsSingleMBBLoop) {
  parseMIR(NonSpeculativeMIR);
  MachineFunction *MF = getMachineFunction("olp_non_speculative");

  // bb.3 (steady.inner) is a single-MBB loop
  EXPECT_TRUE(isSingleMBBLoop(getMBB(MF, 3)));
  // bb.6 (peeled.inner) is a single-MBB loop
  EXPECT_TRUE(isSingleMBBLoop(getMBB(MF, 6)));
  // bb.4 (steady.bottom) is not a single-MBB loop
  EXPECT_FALSE(isSingleMBBLoop(getMBB(MF, 4)));
  // bb.1 (outer.preheader) is not a loop
  EXPECT_FALSE(isSingleMBBLoop(getMBB(MF, 1)));
}

TEST_F(AIELoopUtilsTest, IsOuterLoopPipelined) {
  parseMIR(NonSpeculativeMIR);
  MachineFunction *MF = getMachineFunction("olp_non_speculative");

  // bb.4 (steady.bottom) has OLP metadata
  EXPECT_TRUE(isOuterLoopPipelined(*getMBB(MF, 4)));
  // bb.3 (steady.inner) does not have OLP metadata
  EXPECT_FALSE(isOuterLoopPipelined(*getMBB(MF, 3)));
}

TEST_F(AIELoopUtilsTest, IsOuterLoopSpeculative) {
  parseMIR(SpeculativeMIR);
  MachineFunction *MF = getMachineFunction("olp_speculative");

  // bb.4 (steady.bottom) has speculative metadata
  EXPECT_TRUE(isOuterLoopSpeculative(*getMBB(MF, 4)));
}

} // anonymous namespace
