//===- AIEOuterLoopPipelinerTest.cpp -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2026 Advanced Micro Devices, Inc. or its
// affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Analysis/TargetTransformInfoImpl.h"
#include "llvm/AsmParser/Parser.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Utils.h"
#include "gtest/gtest.h"

using namespace llvm;

namespace {

class TestTTIImpl : public TargetTransformInfoImplCRTPBase<TestTTIImpl> {
  using BaseT = TargetTransformInfoImplCRTPBase<TestTTIImpl>;

public:
  explicit TestTTIImpl(const DataLayout &DL) : BaseT(DL) {}

  bool isLeanStage0Intrinsic(const Instruction &I) const {
    const auto *Call = dyn_cast<CallInst>(&I);
    return Call && Call->getCalledFunction() &&
           Call->getCalledFunction()->getName() == "marker";
  }
};

std::unique_ptr<TargetMachine> createAIE2PTargetMachine() {
  const std::string TargetTriple = Triple::normalize("aie2p--");
  std::string Error;
  const Target *TheTarget = TargetRegistry::lookupTarget(TargetTriple, Error);
  EXPECT_NE(TheTarget, nullptr) << Error;
  if (!TheTarget)
    return nullptr;
  return std::unique_ptr<TargetMachine>(TheTarget->createTargetMachine(
      TargetTriple, "", "", TargetOptions(), std::nullopt, std::nullopt,
      CodeGenOptLevel::Default));
}

class AIEOuterLoopPipelinerTest : public testing::Test {
protected:
  static void SetUpTestSuite() {
    LLVMInitializeAIETargetInfo();
    LLVMInitializeAIETarget();
    LLVMInitializeAIETargetMC();
  }
};

TEST_F(AIEOuterLoopPipelinerTest, CollectLeanStage0) {
  LLVMContext Context;
  SMDiagnostic Error;
  std::unique_ptr<Module> M = parseAssemblyString(R"IR(
    declare void @llvm.set.loop.iterations.i32(i32)
    declare i1 @llvm.loop.decrement.i32(i32)
    declare i32 @marker2(i32)
    declare void @marker(i32, i32)

    define void @test(ptr %a, ptr %c, i32 %n, i32 %m) {
    entry:
      %has.work = icmp ugt i32 %n, 1
      br i1 %has.work, label %outer.header, label %exit

    outer.header:
      %i = phi i32 [ 0, %entry ], [ %i.next, %outer.latch ]
      %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
      %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
      %address = getelementptr inbounds i32, ptr %a.ptr, i32 1
      %loaded = load i32, ptr %address, align 4
      %not.a.load = add i32 %i, 1
      %marker.input = call i32 @marker2(i32 %i)
      call void @marker(i32 %loaded, i32 %marker.input)
      call void @llvm.set.loop.iterations.i32(i32 %m)
      br label %inner.header

    inner.header:
      %result = phi i32 [ 0, %outer.header ], [ %result.next, %inner.header ]
      %result.next = add i32 %result, %not.a.load
      %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
      br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

    outer.latch:
      store i32 %result.next, ptr %c.ptr, align 4
      %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 1
      %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
      %i.next = add nuw i32 %i, 1
      %outer.cond = icmp eq i32 %i.next, %n
      br i1 %outer.cond, label %exit, label %outer.header, !llvm.loop !0

    exit:
      ret void
    }

    !0 = distinct !{!0, !2, !3, !4, !5}
    !1 = distinct !{!1, !2}
    !2 = !{!"llvm.loop.mustprogress"}
    !3 = !{!"llvm.loop.itercount.range", i32 2}
    !4 = !{!"llvm.loop.hint.aie-enable-outer-loop-pipelining", i64 1}
    !5 = !{!"llvm.loop.hint.aie-outer-loop-pipelining-lean-stage0", i64 1}
  )IR",
                                                  Error, Context);
  ASSERT_TRUE(M) << Error.getMessage().str();

  std::unique_ptr<TargetMachine> TM = createAIE2PTargetMachine();
  ASSERT_TRUE(TM);
  M->setTargetTriple(TM->getTargetTriple());
  M->setDataLayout(TM->createDataLayout());

  legacy::PassManager PM;
  PM.add(TM->createPassConfig(PM));
  PM.add(createTargetTransformInfoWrapperPass(
      TargetIRAnalysis([](const Function &F) {
        return TargetTransformInfo(TestTTIImpl(F.getDataLayout()));
      })));
  PM.add(createLoopSimplifyPass());
  PM.add(createHardwareLoopsLegacyPass());
  PM.add(createAIEOuterLoopPipelinerPass());
  PM.run(*M);

  BasicBlock *Stage0 = nullptr;
  BasicBlock *Stage1 = nullptr;
  for (BasicBlock &BB : *M->getFunction("test")) {
    if (BB.getName() == "stage0.top")
      Stage0 = &BB;
    else if (BB.getName() == "steady.stage1.top")
      Stage1 = &BB;
  }
  ASSERT_NE(Stage0, nullptr);
  ASSERT_NE(Stage1, nullptr);

  const auto HasInstructionNamed = [](const BasicBlock &BB, StringRef Name) {
    return any_of(BB, [Name](const Instruction &I) {
      return I.getName().starts_with(Name);
    });
  };
  const auto HasCallTo = [](const BasicBlock &BB, StringRef Callee) {
    return any_of(BB, [Callee](const Instruction &I) {
      const auto *Call = dyn_cast<CallInst>(&I);
      return Call && Call->getCalledFunction() &&
             Call->getCalledFunction()->getName() == Callee;
    });
  };
  EXPECT_TRUE(HasInstructionNamed(*Stage0, "address"));
  EXPECT_TRUE(HasInstructionNamed(*Stage0, "loaded"));
  EXPECT_TRUE(HasCallTo(*Stage0, "marker2"));
  EXPECT_TRUE(HasCallTo(*Stage0, "marker"));
  EXPECT_FALSE(HasInstructionNamed(*Stage0, "not.a.load"));
  EXPECT_FALSE(HasCallTo(*Stage1, "marker2"));
  EXPECT_FALSE(HasCallTo(*Stage1, "marker"));
  EXPECT_TRUE(HasInstructionNamed(*Stage1, "not.a.load"));
}

} // namespace
