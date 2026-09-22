//===- VirtUnrollAliasAnalysisTest.cpp ------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
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

std::unique_ptr<TargetMachine> createTargetMachine() {
  auto TT(Triple::normalize("aie2--"));
  std::string Error;
  const Target *TheTarget = TargetRegistry::lookupTarget(TT, Error);
  llvm::errs() << Error << "\n";
  return std::unique_ptr<TargetMachine>(static_cast<TargetMachine *>(
      TheTarget->createTargetMachine(TT, "", "", TargetOptions(), std::nullopt,
                                     std::nullopt, CodeGenOptLevel::Default)));
}

class VirtUnrollAliasAnalysisTest : public testing::Test {
protected:
  static const char *MIRString;
  static const char *DifferentStepsMIRString;
  static const char *MMOOffsetMIRString;
  static const char *PAddCollapseMIRString;
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

  void SetUp() override { parse(MIRString); }

  void parse(const char *MIR) {
    TM = createTargetMachine();
    std::unique_ptr<MemoryBuffer> MBuffer = MemoryBuffer::getMemBuffer(MIR);
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

// An in-place narrowing conversion: both pointers start at %p, the load
// advances 8 bytes per iteration and reads 4, the store advances 4 and writes
// 2. The chains are rooted together but run at different rates, so the
// update-count model cannot compare them and the byte intervals must be used.
//
// Addresses, with n the iteration index:
//   load  [8n, 8n+4)
//   store [4n, 4n+2)
//
// The store trails the load and can never reach a later load, but a load can
// be caught by a later store (load at n=1 covers [8,12), store at n=2 covers
// [8,10)). The two directions must therefore get different answers.
TEST_F(VirtUnrollAliasAnalysisTest, TestDifferentSteps) {
  parse(DifferentStepsMIRString);
  const MachineFunction *F = getMachineFunction(M.get(), "inplace_narrow");
  const MachineBasicBlock *MBB = F->getBlockNumbered(1);
  std::vector<const MachineInstr *> Instr;
  getInstructions(MBB, Instr);
  const MachineInstr *Load = Instr[0];
  const MachineInstr *Store = Instr[1];

  // Same iteration: load [0,4) and store [0,2) overlap.
  EXPECT_EQ(AIE::aliasAcrossVirtualUnrolls(Load, Store, 0, 0),
            AliasResult::MayAlias);

  // The store of one iteration never reaches the load of a later one. This is
  // the edge that otherwise pins RecMII to the length of the loop body.
  EXPECT_EQ(AIE::aliasAcrossVirtualUnrolls(Store, Load, 0, 1),
            AliasResult::NoAlias);
  EXPECT_EQ(AIE::aliasAcrossVirtualUnrolls(Store, Load, 0, 4),
            AliasResult::NoAlias);

  // The opposite direction does overlap and must be kept. Answering this one
  // from a single iteration would be wrong: the two accesses are disjoint at
  // n=0 and only collide from n=1 onwards.
  EXPECT_EQ(AIE::aliasAcrossVirtualUnrolls(Load, Store, 0, 1),
            AliasResult::MayAlias);

  // A load never reaches another load, whatever the distance.
  EXPECT_EQ(AIE::aliasAcrossVirtualUnrolls(Load, Load, 0, 1),
            AliasResult::NoAlias);
}

// Same loop as above, but the store is the part of a wider access that sits 8
// bytes past the address of its IR Value, as left behind by splitting a wide
// store. The MachineMemOperand offset must be part of the byte address.
//
//   load  [8n, 8n+4)
//   store [4n+8, 4n+10)
//
// Without the offset the store at n=0 would be [0,2), which a load of the next
// iteration [8,12) never reaches. With it, the store covers [8,10) and does.
TEST_F(VirtUnrollAliasAnalysisTest, TestMMOOffset) {
  parse(MMOOffsetMIRString);
  const MachineFunction *F = getMachineFunction(M.get(), "offset_narrow");
  const MachineBasicBlock *MBB = F->getBlockNumbered(1);
  std::vector<const MachineInstr *> Instr;
  getInstructions(MBB, Instr);
  const MachineInstr *Load = Instr[0];
  const MachineInstr *Store = Instr[1];

  EXPECT_EQ(Store->memoperands().front()->getOffset(), 8);
  EXPECT_EQ(AIE::aliasAcrossVirtualUnrolls(Store, Load, 0, 1),
            AliasResult::MayAlias);
  // Two iterations later the store [8,10) is still behind the load [16,20) and
  // keeps falling further behind, so a distance that is large enough is clear.
  EXPECT_EQ(AIE::aliasAcrossVirtualUnrolls(Store, Load, 0, 4),
            AliasResult::NoAlias);
}

// The store goes through %off = %ps + 4, with a size equal to that offset and
// an add.2d user of %ps. aliasAcrossVirtualUnrolls then queries the store as
// %ps, the base of a wider padd access. The 4 bytes that %off added must still
// be part of the byte address.
//
//   load  [8n+4, 8n+8)
//   store [8n+4, 8n+8)
//
// Without those 4 bytes the store would be [8n, 8n+4), seemingly clear of the
// load it actually overlaps.
TEST_F(VirtUnrollAliasAnalysisTest, TestPAddCollapsedOffset) {
  parse(PAddCollapseMIRString);
  const MachineFunction *F = getMachineFunction(M.get(), "padd_collapse");
  const MachineBasicBlock *MBB = F->getBlockNumbered(1);
  std::vector<const MachineInstr *> Instr;
  getInstructions(MBB, Instr);
  const MachineInstr *Load = Instr[0];
  const MachineInstr *Store = Instr[1];

  EXPECT_EQ(AIE::aliasAcrossVirtualUnrolls(Load, Store, 0, 0),
            AliasResult::MayAlias);
  EXPECT_EQ(AIE::aliasAcrossVirtualUnrolls(Store, Load, 0, 0),
            AliasResult::MayAlias);
  // One iteration apart the accesses are a full stride away and stay so.
  EXPECT_EQ(AIE::aliasAcrossVirtualUnrolls(Store, Load, 0, 1),
            AliasResult::NoAlias);
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

const char *VirtUnrollAliasAnalysisTest::DifferentStepsMIRString = R"MIR(
--- |
  define void @inplace_narrow(ptr noalias %p, i32 %n) {
  entry:
    %umax = call i32 @llvm.umax.i32(i32 %n, i32 1)
    call void @llvm.set.loop.iterations.i32(i32 %umax)
    br label %loop

  loop:                                             ; preds = %loop, %entry
    %pl = phi ptr [ %p, %entry ], [ %pl.next, %loop ]
    %ps = phi ptr [ %p, %entry ], [ %ps.next, %loop ]
    %v = load i32, ptr %pl, align 4
    %w = trunc i32 %v to i16
    store i16 %w, ptr %ps, align 2
    %pl.next = getelementptr inbounds i8, ptr %pl, i20 8
    %ps.next = getelementptr inbounds i8, ptr %ps, i20 4
    %0 = call i1 @llvm.loop.decrement.i32(i32 1)
    br i1 %0, label %loop, label %exit

  exit:                                             ; preds = %loop
    ret void
  }

  declare i32 @llvm.umax.i32(i32, i32)
  declare void @llvm.set.loop.iterations.i32(i32)
  declare i1 @llvm.loop.decrement.i32(i32)

...
---
name:            inplace_narrow
alignment:       16
legalized:       true
regBankSelected: true
selected:        true
tracksRegLiveness: true
body:             |
  bb.0.entry:
    successors: %bb.1(0x80000000)
    liveins: $p0, $r0

    renamable $r1 = MOV_RLC_imm10_pseudo 1
    renamable $r27 = LTU renamable $r1, renamable $r0
    renamable $r0 = SELNEZ killed renamable $r0, killed renamable $r1, killed renamable $r27
    $lc = ADD_NC $r0, 0
    $ls = MOVXM_lng_cg %bb.1
    $le = MOVXM_lng_cg <mcsymbol .L_LEnd0>
    $p1 = MOV_mv_scl $p0

  bb.1.loop:
    successors: %bb.1(0x7c000000), %bb.2(0x04000000)
    liveins: $p0, $p1

    renamable $r0, renamable $p0 = LDA_dms_lda_pstm_nrm_imm killed renamable $p0, 8 :: (load (s32) from %ir.pl)
    renamable $p1 = ST_S16_ag_pstm_nrm_imm killed renamable $r0, killed renamable $p1, 4 :: (store (s16) into %ir.ps)
    PseudoLoopEnd <mcsymbol .L_LEnd0>, %bb.1

  bb.2.exit:
    RET implicit $lr
    DelayedSchedBarrier

...
)MIR";

const char *VirtUnrollAliasAnalysisTest::MMOOffsetMIRString = R"MIR(
--- |
  define void @offset_narrow(ptr noalias %p, i32 %n) {
  entry:
    %umax = call i32 @llvm.umax.i32(i32 %n, i32 1)
    call void @llvm.set.loop.iterations.i32(i32 %umax)
    br label %loop

  loop:                                             ; preds = %loop, %entry
    %pl = phi ptr [ %p, %entry ], [ %pl.next, %loop ]
    %ps = phi ptr [ %p, %entry ], [ %ps.next, %loop ]
    %v = load i32, ptr %pl, align 4
    %w = trunc i32 %v to i16
    store i16 %w, ptr %ps, align 2
    %pl.next = getelementptr inbounds i8, ptr %pl, i20 8
    %ps.next = getelementptr inbounds i8, ptr %ps, i20 4
    %0 = call i1 @llvm.loop.decrement.i32(i32 1)
    br i1 %0, label %loop, label %exit

  exit:                                             ; preds = %loop
    ret void
  }

  declare i32 @llvm.umax.i32(i32, i32)
  declare void @llvm.set.loop.iterations.i32(i32)
  declare i1 @llvm.loop.decrement.i32(i32)

...
---
name:            offset_narrow
alignment:       16
legalized:       true
regBankSelected: true
selected:        true
tracksRegLiveness: true
body:             |
  bb.0.entry:
    successors: %bb.1(0x80000000)
    liveins: $p0, $r0

    renamable $r1 = MOV_RLC_imm10_pseudo 1
    renamable $r27 = LTU renamable $r1, renamable $r0
    renamable $r0 = SELNEZ killed renamable $r0, killed renamable $r1, killed renamable $r27
    $lc = ADD_NC $r0, 0
    $ls = MOVXM_lng_cg %bb.1
    $le = MOVXM_lng_cg <mcsymbol .L_LEnd0>
    $p1 = MOV_mv_scl $p0

  bb.1.loop:
    successors: %bb.1(0x7c000000), %bb.2(0x04000000)
    liveins: $p0, $p1

    renamable $r0, renamable $p0 = LDA_dms_lda_pstm_nrm_imm killed renamable $p0, 8 :: (load (s32) from %ir.pl)
    renamable $p1 = ST_S16_ag_pstm_nrm_imm killed renamable $r0, killed renamable $p1, 4 :: (store (s16) into %ir.ps + 8)
    PseudoLoopEnd <mcsymbol .L_LEnd0>, %bb.1

  bb.2.exit:
    RET implicit $lr
    DelayedSchedBarrier

...
)MIR";

const char *VirtUnrollAliasAnalysisTest::PAddCollapseMIRString = R"MIR(
--- |
  define void @padd_collapse(ptr noalias %p, i32 %n, i32 %x) {
  entry:
    %umax = call i32 @llvm.umax.i32(i32 %n, i32 1)
    call void @llvm.set.loop.iterations.i32(i32 %umax)
    br label %loop

  loop:                                             ; preds = %loop, %entry
    %pl = phi ptr [ %p, %entry ], [ %pl.next, %loop ]
    %ps = phi ptr [ %p, %entry ], [ %ps.next, %loop ]
    %pl.mid = getelementptr inbounds i8, ptr %pl, i20 4
    %v = load i32, ptr %pl.mid, align 4
    %off = getelementptr inbounds i8, ptr %ps, i20 4
    store i32 %x, ptr %off, align 4
    %unused = call { ptr, i20 } @llvm.aie2.add.2d(ptr %ps, i20 0, i20 1, i20 2, i20 0)
    %pl.next = getelementptr inbounds i8, ptr %pl.mid, i20 4
    %ps.next = getelementptr inbounds i8, ptr %ps, i20 8
    %0 = call i1 @llvm.loop.decrement.i32(i32 1)
    br i1 %0, label %loop, label %exit

  exit:                                             ; preds = %loop
    ret void
  }

  declare { ptr, i20 } @llvm.aie2.add.2d(ptr, i20, i20, i20, i20)
  declare i32 @llvm.umax.i32(i32, i32)
  declare void @llvm.set.loop.iterations.i32(i32)
  declare i1 @llvm.loop.decrement.i32(i32)

...
---
name:            padd_collapse
alignment:       16
legalized:       true
regBankSelected: true
selected:        true
tracksRegLiveness: true
body:             |
  bb.0.entry:
    successors: %bb.1(0x80000000)
    liveins: $p0, $r0, $r1

    renamable $r2 = MOV_RLC_imm10_pseudo 1
    renamable $r27 = LTU renamable $r2, renamable $r0
    renamable $r0 = SELNEZ killed renamable $r0, killed renamable $r2, killed renamable $r27
    $lc = ADD_NC $r0, 0
    $ls = MOVXM_lng_cg %bb.1
    $le = MOVXM_lng_cg <mcsymbol .L_LEnd0>
    $p1 = MOV_mv_scl $p0

  bb.1.loop:
    successors: %bb.1(0x7c000000), %bb.2(0x04000000)
    liveins: $p0, $p1, $r1

    renamable $r0 = LDA_dms_lda_idx_imm renamable $p0, 4 :: (load (s32) from %ir.pl.mid)
    ST_dms_sts_idx_imm renamable $r1, renamable $p1, 4 :: (store (s32) into %ir.off)
    PseudoLoopEnd <mcsymbol .L_LEnd0>, %bb.1

  bb.2.exit:
    RET implicit $lr
    DelayedSchedBarrier

...
)MIR";

} // anonymous namespace
