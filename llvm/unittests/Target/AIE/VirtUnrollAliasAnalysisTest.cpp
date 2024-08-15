//===- VirtUnrollAliasAnalysisTest.cpp ------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEBaseAliasAnalysis.h"
#include "llvm/CodeGen/MIRParser/MIRParser.h"
#include "llvm/CodeGen/MachineBlockFrequencyInfo.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineSizeOpts.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "gtest/gtest.h"
#include <vector>

using namespace llvm;

namespace {

std::unique_ptr<LLVMTargetMachine> createTargetMachine() {
  auto TT(Triple::normalize("aie2--"));
  std::string Error;
  const Target *TheTarget = TargetRegistry::lookupTarget(TT, Error);
  llvm::errs() << Error << "\n";
  return std::unique_ptr<LLVMTargetMachine>(static_cast<LLVMTargetMachine *>(
      TheTarget->createTargetMachine(TT, "", "", TargetOptions(), std::nullopt,
                                     std::nullopt, CodeGenOptLevel::Default)));
}

class VirtUnrollAliasAnalysisTest : public testing::Test {
protected:
  static const char *MIRString;
  LLVMContext Context;
  std::unique_ptr<LLVMTargetMachine> TM;
  std::unique_ptr<MachineModuleInfo> MMI;
  std::unique_ptr<MIRParser> Parser;
  std::unique_ptr<Module> M;

  static void SetUpTestCase() {
    LLVMInitializeAIETargetInfo();
    LLVMInitializeAIETarget();
    LLVMInitializeAIETargetMC();
  }

  void SetUp() override {
    TM = createTargetMachine();
    std::unique_ptr<MemoryBuffer> MBuffer =
        MemoryBuffer::getMemBuffer(MIRString);
    Parser = createMIRParser(std::move(MBuffer), Context);
    if (!Parser)
      report_fatal_error("null MIRParser");
    M = Parser->parseIRModule();
    if (!M)
      report_fatal_error("parseIRModule failed");
    M->setTargetTriple(TM->getTargetTriple().getTriple());
    M->setDataLayout(TM->createDataLayout());
    MMI = std::make_unique<MachineModuleInfo>(TM.get());
    if (Parser->parseMachineFunctions(*M, *MMI.get()))
      report_fatal_error("parseMachineFunctions failed");
  }

  MachineFunction *getMachineFunction(Module *M, StringRef Name) {
    auto F = M->getFunction(Name);
    if (!F)
      report_fatal_error("null Function");
    auto &MF = MMI->getOrCreateMachineFunction(*F);
    return &MF;
  }

  void getInstructions(const MachineBasicBlock *MBB,
                       std::vector<const MachineInstr *> &Instr) {
    for (const MachineInstr &MI : *MBB) {
      Instr.push_back(&MI);
    }
  }
};

TEST_F(VirtUnrollAliasAnalysisTest, TestSameIterators) {
  const MachineFunction *F = getMachineFunction(M.get(), "test");
  const MachineBasicBlock *MBB = F->getBlockNumbered(2);
  std::vector<const MachineInstr *> Instr;
  getInstructions(MBB, Instr);
  const MachineInstr *Load1 = Instr[0];
  const MachineInstr *Store1 = Instr[4];
  const MachineInstr *Load2 = Instr[5];
  const MachineInstr *Store2 = Instr[6];
  const MachineInstr *Load3 = Instr[7];
  const MachineInstr *Store3 = Instr[8];

  // Same load, same virt unroll.
  EXPECT_TRUE(AIE::aliasAcrossVirtualUnrolls(Load1, Load1, 0, 0) ==
              AliasResult::MayAlias);
  // Same load, same virt unroll (3rd iteration).
  EXPECT_TRUE(AIE::aliasAcrossVirtualUnrolls(Load1, Load1, 2, 2) ==
              AliasResult::MayAlias);
  // Same load, diff. virt unroll.
  EXPECT_TRUE(AIE::aliasAcrossVirtualUnrolls(Load1, Load1, 0, 1) ==
              AliasResult::NoAlias);
  // Diff. load, same virt unroll.
  EXPECT_TRUE(AIE::aliasAcrossVirtualUnrolls(Load1, Load2, 0, 0) ==
              AliasResult::NoAlias);
  // Store and load with same virt unroll, same pointer step.
  EXPECT_TRUE(AIE::aliasAcrossVirtualUnrolls(Load1, Store1, 0, 0) ==
              AliasResult::MayAlias);
  // Store and load with same virt unroll, same pointer step.
  EXPECT_TRUE(AIE::aliasAcrossVirtualUnrolls(Load2, Store2, 0, 0) ==
              AliasResult::MayAlias);
  // Store and load with diff virt unroll, same pointer step.
  EXPECT_TRUE(AIE::aliasAcrossVirtualUnrolls(Load3, Store3, 4, 0) ==
              AliasResult::NoAlias);
  // Store and load with diff virt unroll, diff pointer step.
  // This case is interesting because the first store from the 3rd iteration
  // will alias with the 3rd load of the second iteration since we are not
  // updating the pointer after the last load and store (post. inc.).
  EXPECT_TRUE(AIE::aliasAcrossVirtualUnrolls(Store1, Load3, 3, 2) ==
              AliasResult::MayAlias);
}

const char *VirtUnrollAliasAnalysisTest::MIRString = R"MIR(
--- |
  ; Function Attrs: nofree nosync nounwind memory(none)
  declare { ptr, i20 } @llvm.aie2.add.2d(ptr, i20, i20, i20, i20) #0
  
  ; Function Attrs: nofree nosync nounwind memory(readwrite, inaccessiblemem: none)
  define dso_local i32 @test(ptr %a, i8 %b, i32 %c) local_unnamed_addr #1 {
  entry:
    %exitcond = icmp eq i32 %c, 0
    br i1 %exitcond, label %for.cond.cleanup, label %for.body.preheader
  
  for.body.preheader:                               ; preds = %entry
    br label %for.body
  
  for.body:                                         ; preds = %for.body.preheader, %for.body
    %lsr.iv = phi i32 [ %c, %for.body.preheader ], [ %lsr.iv.next, %for.body ]
    %counter_load = phi i20 [ %7, %for.body ], [ 0, %for.body.preheader ]
    %counter_store = phi i20 [ %10, %for.body ], [ 0, %for.body.preheader ]
    %sum0 = phi i8 [ %sum3, %for.body ], [ 0, %for.body.preheader ]
    %ptr_load = phi ptr [ %8, %for.body ], [ %a, %for.body.preheader ]
    %ptr_store = phi ptr [ %11, %for.body ], [ %a, %for.body.preheader ]
    %load1 = load i8, ptr %ptr_load, align 1
    %0 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr nonnull %ptr_load, i20 0, i20 1, i20 2, i20 %counter_load)
    %1 = extractvalue { ptr, i20 } %0, 1
    %2 = extractvalue { ptr, i20 } %0, 0
    store i8 %b, ptr %ptr_store, align 1
    %3 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr nonnull %ptr_store, i20 0, i20 1, i20 2, i20 %counter_store)
    %4 = extractvalue { ptr, i20 } %3, 1
    %5 = extractvalue { ptr, i20 } %3, 0
    %load2 = load i8, ptr %2, align 1
    %6 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr nonnull %2, i20 0, i20 1, i20 2, i20 %1)
    %7 = extractvalue { ptr, i20 } %6, 1
    %8 = extractvalue { ptr, i20 } %6, 0
    store i8 %b, ptr %5, align 1
    %9 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr nonnull %5, i20 0, i20 1, i20 2, i20 %4)
    %10 = extractvalue { ptr, i20 } %9, 1
    %11 = extractvalue { ptr, i20 } %9, 0
    %load3 = load i8, ptr %8, align 1
    store i8 %b, ptr %11, align 1
    %sum1 = add i8 %load1, %sum0
    %sum2 = add i8 %sum1, %load2
    %sum3 = add i8 %sum2, %load3
    %lsr.iv.next = add i32 %lsr.iv, -1
    %exitcond.not = icmp eq i32 %lsr.iv.next, 0
    br i1 %exitcond.not, label %for.cond.cleanup.loopexit, label %for.body
  
  for.cond.cleanup.loopexit:                        ; preds = %for.body
    %12 = zext i8 %sum3 to i32
    br label %for.cond.cleanup
  
  for.cond.cleanup:                                 ; preds = %for.cond.cleanup.loopexit, %entry
    %retsum = phi i32 [ 0, %entry ], [ %12, %for.cond.cleanup.loopexit ]
    ret i32 %retsum
  }
  
  attributes #0 = { nofree nosync nounwind memory(none) }
  attributes #1 = { nofree nosync nounwind memory(readwrite, inaccessiblemem: none) }

...
---
name:            test
alignment:       16
legalized:       true
regBankSelected: true
selected:        true
tracksRegLiveness: true
tracksDebugUserValues: true
registers:       []
liveins:         []
calleeSavedRegisters: [ '$lr', '$r16', '$r17', '$r18', '$r19', '$r20', 
                        '$r21', '$r22', '$r23', '$p6', '$p7' ]

body:             |
  bb.0.entry (align 16):
    successors: %bb.4(0x30000000), %bb.1(0x50000000)
    liveins: $p0, $r1, $r2
  
    JZ renamable $r2, %bb.4
    DelayedSchedBarrier
  
  bb.1.for.body.preheader:
    successors: %bb.2(0x80000000)
    liveins: $p0, $r1, $r2
  
    renamable $m0 = MOV_PD_imm10_pseudo 0
    renamable $dj0 = MOV_PD_imm10_pseudo 1
    renamable $dn0 = MOV_PD_imm10_pseudo 2
    renamable $r0 = MOV_RLC_imm10_pseudo 0
    $dc0 = MOV_mv_scl $m0
    $dc1 = MOV_mv_scl $m0
    $p1 = MOV_mv_scl $p0
  
  bb.2.for.body (align 16):
    successors: %bb.3(0x04000000), %bb.2(0x7c000000)
    liveins: $d0:0x0000000000000870, $d1:0x0000000000000010, $dc0, $dc1, $dj0, $dn0, $m0, $p0, $p1, $r0, $r1, $r2
  
    $r3, $p0, $dc0 = LDA_2D_S8_dmhb_lda killed $p0, $d0 :: (load (s8) from %ir.ptr_load)
    $m1 = MOV_mv_scl $m0
    $dn1 = MOV_mv_scl $dn0
    $dj1 = MOV_mv_scl $dj0
    $p1, $dc1 = ST_2D_S8 $r1, killed $p1, $d1 :: (store (s8) into %ir.ptr_store)
    $r4, $p0, $dc0 = LDA_2D_S8_dmhb_lda killed $p0, $d0 :: (load (s8) from %ir.2)
    $p1, $dc1 = ST_2D_S8 $r1, killed $p1, $d1 :: (store (s8) into %ir.5)
    renamable $r5 = LDA_S8_ag_idx_imm renamable $p0, 0 :: (load (s8) from %ir.8)
    ST_S8_ag_idx_imm renamable $r1, renamable $p1, 0 :: (store (s8) into %ir.11)
    renamable $r0 = ADD killed renamable $r3, killed renamable $r0, implicit-def dead $srcarry
    renamable $r0 = ADD killed renamable $r0, killed renamable $r4, implicit-def dead $srcarry
    renamable $r2 = ADD_add_r_ri killed renamable $r2, -1, implicit-def dead $srcarry
    renamable $r0 = ADD killed renamable $r0, killed renamable $r5, implicit-def dead $srcarry
    JNZ renamable $r2, %bb.2
    DelayedSchedBarrier
  
  bb.3.for.cond.cleanup.loopexit:
    successors:
    liveins: $r0
  
    renamable $r0 = EXTENDu8 killed renamable $r0
    RET implicit $lr
    DelayedSchedBarrier implicit $r0
  
  bb.4 (align 16):
    renamable $r0 = MOV_RLC_imm10_pseudo 0
    RET implicit $lr
    DelayedSchedBarrier implicit $r0

...
)MIR";

} // anonymous namespace
