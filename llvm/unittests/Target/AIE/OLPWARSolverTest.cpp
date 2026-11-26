//===- OLPWARSolverTest.cpp - Unit tests for OLPWARSolver -------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Unit tests for findLastDefsInClones() and collectFreeStoreUses() using
// real AIE2PS instructions and register hierarchy.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseInstrInfo.h"
#include "AIEInterBlockScheduling.h"
#include "AIEOLPWARSolver.h"
#include "llvm/CodeGen/MIRParser/MIRParser.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "gtest/gtest.h"

using namespace llvm;
using namespace llvm::AIE;

namespace {

std::unique_ptr<TargetMachine> createAIE2PSTargetMachine() {
  auto TT(Triple::normalize("aie2ps--"));
  std::string Error;
  const Target *TheTarget = TargetRegistry::lookupTarget(TT, Error);
  if (!TheTarget) {
    llvm::errs() << Error << "\n";
    return nullptr;
  }
  return std::unique_ptr<TargetMachine>(TheTarget->createTargetMachine(
      Triple(TT), "", "", TargetOptions(), std::nullopt, std::nullopt,
      CodeGenOptLevel::Default));
}

/// Test fixture for OLPWARSolver unit tests.
///
/// Parses MIR with real AIE2PS instructions, then extracts MachineInstr
/// pointers to exercise findLastDefsInClones() and collectFreeStoreUses()
/// with the real register hierarchy (dm/cml/cmh overlap relationships)
/// and correct mayLoad()/mayStore() flags from tablegen.
class OLPWARSolverTest : public testing::Test {
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
    TM = createAIE2PSTargetMachine();
    if (!TM)
      GTEST_SKIP() << "AIE2PS target not available";

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

  /// Get MachineInstrs from a basic block, excluding terminators and
  /// scheduling barriers.
  SmallVector<MachineInstr *, 8> getInstrs(MachineBasicBlock *MBB) {
    SmallVector<MachineInstr *, 8> Instrs;
    for (MachineInstr &MI : *MBB) {
      if (MI.isTerminator() || MI.getOpcode() == TargetOpcode::IMPLICIT_DEF)
        continue;
      // Skip scheduling barriers (DelayedSchedBarrier)
      if (MI.isMetaInstruction())
        continue;
      Instrs.push_back(&MI);
    }
    return Instrs;
  }

  /// Create an OLPWARSolver using the target's InstrInfo and RegisterInfo.
  /// Uses a minimal InterBlockScheduling (not dereferenced by the methods
  /// under test).
  struct SolverContext {
    std::unique_ptr<InterBlockScheduling> IBS;
    std::unique_ptr<OLPWARSolver> Solver;
  };

  SolverContext createSolver(MachineFunction *MF) {
    SolverContext Ctx;
    Ctx.IBS = std::make_unique<InterBlockScheduling>(nullptr, false);
    const auto &TII = *static_cast<const AIEBaseInstrInfo *>(
        MF->getSubtarget().getInstrInfo());
    const auto &TRI = *MF->getSubtarget().getRegisterInfo();
    Ctx.Solver = std::make_unique<OLPWARSolver>(*Ctx.IBS, TII, TRI);
    return Ctx;
  }

  /// Find the MCRegister by name from a register in the MIR.
  MCRegister findReg(MachineFunction *MF, StringRef Name) {
    const auto &TRI = *MF->getSubtarget().getRegisterInfo();
    for (unsigned I = 1, E = TRI.getNumRegs(); I < E; ++I) {
      if (StringRef(TRI.getName(I)) == Name)
        return MCRegister(I);
    }
    report_fatal_error("Register not found: " + Name);
  }
};

// ===================================================================
// MIR for the tests. Each basic block provides instructions for a
// specific test scenario.
//
// bb.0: Two VMAC instructions defining dm3 and dm2 (single clone tests)
// bb.1: Store/load pattern for collectFreeStoreUses
//        VST_SRS $cml3, VST_SRS $cmh3, VLDA $cml3, VLDA $cmh3
// bb.2: Interleaved pattern
//        VST_SRS $cml3, VLDA $cml3, VST_SRS $cmh3, VLDA $cmh3
// bb.3: Non-load writer pattern
//        VST_SRS $cml3, VMAC $dm3 (not a load!)
// bb.4: Incomplete coverage pattern
//        VST_SRS $cml3, VLDA $cml3 (only cml3, missing cmh3)
// ===================================================================
const char *TestMIR = R"MIR(
--- |
  define void @war_solver_test() {
  entry:
    ret void
  }
...
---
name:            war_solver_test
tracksRegLiveness: true
body:             |
  bb.0.entry:
    successors: %bb.1
    liveins: $dm3, $ewl5, $ey0, $ey1, $ewh5, $r8, $dm2, $ewl7, $ewh7
    $dm3 = VMAC_f_vmac_bfp16 $dm3, $ewl5, $ey0, $ey1, $ewh5, $r8, implicit-def $srfpflags, implicit $crfpmask
    $dm2 = VMAC_f_vmac_bfp16 $dm2, $ewl7, $ey0, $ey1, $ewh7, $r8, implicit-def $srfpflags, implicit $crfpmask

  bb.1:
    successors: %bb.2
    liveins: $cml3, $cmh3, $s0, $s1, $p0, $p1, $dj0, $dj1
    VST_SRS_2x_dm_sts_srs_cm_idx_srsSign0 $cml3, $s0, $p0, $dj0, implicit-def $srsrs_of, implicit $crrnd, implicit $crsrsmode, implicit $crsat, implicit $srssign0
    VST_SRS_2x_dm_sts_srs_cm_idx_srsSign0 $cmh3, $s1, $p1, $dj1, implicit-def $srsrs_of, implicit $crrnd, implicit $crsrsmode, implicit $crsat, implicit $srssign0
    $cml3 = VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign0 $s0, $p0, $dj0, implicit-def $srups_of, implicit $crsat, implicit $crupsmode, implicit $upssign0
    $cmh3 = VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign0 $s1, $p1, $dj1, implicit-def $srups_of, implicit $crsat, implicit $crupsmode, implicit $upssign0

  bb.2:
    successors: %bb.3
    liveins: $cml3, $cmh3, $s0, $s1, $p0, $p1, $dj0, $dj1
    VST_SRS_2x_dm_sts_srs_cm_idx_srsSign0 $cml3, $s0, $p0, $dj0, implicit-def $srsrs_of, implicit $crrnd, implicit $crsrsmode, implicit $crsat, implicit $srssign0
    $cml3 = VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign0 $s0, $p0, $dj0, implicit-def $srups_of, implicit $crsat, implicit $crupsmode, implicit $upssign0
    VST_SRS_2x_dm_sts_srs_cm_idx_srsSign0 $cmh3, $s1, $p1, $dj1, implicit-def $srsrs_of, implicit $crrnd, implicit $crsrsmode, implicit $crsat, implicit $srssign0
    $cmh3 = VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign0 $s1, $p1, $dj1, implicit-def $srups_of, implicit $crsat, implicit $crupsmode, implicit $upssign0

  bb.3:
    successors: %bb.4
    liveins: $cml3, $cmh3, $s0, $p0, $dj0, $dm3, $ewl5, $ey0, $ey1, $ewh5, $r8
    VST_SRS_2x_dm_sts_srs_cm_idx_srsSign0 $cml3, $s0, $p0, $dj0, implicit-def $srsrs_of, implicit $crrnd, implicit $crsrsmode, implicit $crsat, implicit $srssign0
    $dm3 = VMAC_f_vmac_bfp16 $dm3, $ewl5, $ey0, $ey1, $ewh5, $r8, implicit-def $srfpflags, implicit $crfpmask

  bb.4:
    liveins: $cml3, $s0, $p0, $dj0
    VST_SRS_2x_dm_sts_srs_cm_idx_srsSign0 $cml3, $s0, $p0, $dj0, implicit-def $srsrs_of, implicit $crrnd, implicit $crsrsmode, implicit $crsat, implicit $srssign0
    $cml3 = VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign0 $s0, $p0, $dj0, implicit-def $srups_of, implicit $crsat, implicit $crupsmode, implicit $upssign0
    RET implicit $lr
    DelayedSchedBarrier

...
)MIR";

// =====================================================================
// findLastDefsInClones tests
// =====================================================================

/// A single clone defining dm3 should be accepted.
TEST_F(OLPWARSolverTest, FindLastDefs_SingleCloneWriter) {
  parseMIR(TestMIR);
  auto *MF = getMachineFunction("war_solver_test");
  auto SC = createSolver(MF);
  auto Instrs = getInstrs(MF->getBlockNumbered(0));
  ASSERT_GE(Instrs.size(), 2u);

  // Use only the first VMAC ($dm3 = VMAC ...) as a clone.
  BlockState BS(MF->getBlockNumbered(0));
  BS.TopInsert.Clones = {Instrs[0]};
  // Need a non-empty Bundles for the InsertDescriptor to not be "empty",
  // but findLastDefsInClones only reads Clones.
  BS.TopInsert.Bundles = {MachineBundle({Instrs[0]}, nullptr)};

  auto LastDefs = SC.Solver->findLastDefsInClones(BS);

  // dm3 should be in the map (single writer, only self-read).
  MCRegister DM3 = findReg(MF, "dm3");
  EXPECT_TRUE(LastDefs.count(DM3))
      << "dm3 should be accepted with a single clone writer";
  EXPECT_EQ(LastDefs[DM3]->getParent(), Instrs[0]);
}

/// Two independent clones defining different registers (dm3, dm2) should
/// both be accepted.
TEST_F(OLPWARSolverTest, FindLastDefs_TwoIndependentClones) {
  parseMIR(TestMIR);
  auto *MF = getMachineFunction("war_solver_test");
  auto SC = createSolver(MF);
  auto Instrs = getInstrs(MF->getBlockNumbered(0));
  ASSERT_GE(Instrs.size(), 2u);

  // Both VMACs as clones: dm3 and dm2 are independent.
  BlockState BS(MF->getBlockNumbered(0));
  BS.TopInsert.Clones = {Instrs[0], Instrs[1]};
  BS.TopInsert.Bundles = {MachineBundle({Instrs[0]}, nullptr),
                          MachineBundle({Instrs[1]}, nullptr)};

  auto LastDefs = SC.Solver->findLastDefsInClones(BS);

  MCRegister DM3 = findReg(MF, "dm3");
  MCRegister DM2 = findReg(MF, "dm2");
  EXPECT_TRUE(LastDefs.count(DM3)) << "dm3 should be accepted";
  EXPECT_TRUE(LastDefs.count(DM2)) << "dm2 should be accepted";
}

/// Two clones both writing overlapping registers should be rejected.
/// We simulate this by using the same dm3 VMAC clone twice -- the
/// second "clone" overlaps the first.
TEST_F(OLPWARSolverTest, FindLastDefs_MultipleOverlappingWriters) {
  parseMIR(TestMIR);
  auto *MF = getMachineFunction("war_solver_test");
  auto SC = createSolver(MF);
  auto Instrs = getInstrs(MF->getBlockNumbered(0));
  ASSERT_GE(Instrs.size(), 2u);

  // The first VMAC defines dm3. The second also uses ey0, ey1 which
  // don't overlap with dm3. But if we had two clones both defining dm3
  // we'd reject. We'll test this by giving the same instruction twice --
  // but findLastDefsInClones skips the defining instruction itself.
  //
  // Instead, let's test a different scenario: clone 0 defines dm3,
  // and clone 1 reads cml3 (a sub-register of dm3 -> overlaps).
  // VMAC reads dm3 as first source operand. dm3 overlaps dm3.
  // The validation skips the defining instruction (Clone == DefMI),
  // so if there's another clone reading an overlapping register, it
  // should be rejected.
  //
  // We use the bb.3 vmac and bb.0 vmac both defining dm3.
  auto Instrs3 = getInstrs(MF->getBlockNumbered(3));
  ASSERT_GE(Instrs3.size(), 2u);
  // Instrs3[1] is VMAC $dm3, Instrs[0] is also VMAC $dm3.
  // Both define dm3 -- and both read dm3 (as accumulator input).

  BlockState BS(MF->getBlockNumbered(0));
  BS.TopInsert.Clones = {Instrs[0], Instrs3[1]};
  BS.TopInsert.Bundles = {MachineBundle({Instrs[0]}, nullptr),
                          MachineBundle({Instrs3[1]}, nullptr)};

  auto LastDefs = SC.Solver->findLastDefsInClones(BS);

  MCRegister DM3 = findReg(MF, "dm3");
  // dm3 should be rejected: two clones both define (and read) dm3.
  EXPECT_FALSE(LastDefs.count(DM3))
      << "dm3 should be rejected with multiple overlapping writers";
}

/// A clone that reads an overlapping register of a def in another clone
/// should cause rejection.
TEST_F(OLPWARSolverTest, FindLastDefs_OtherCloneReader) {
  parseMIR(TestMIR);
  auto *MF = getMachineFunction("war_solver_test");
  auto SC = createSolver(MF);

  // bb.0 Instrs[0]: $dm3 = VMAC $dm3, ...  (defines and reads dm3)
  // bb.1 Instrs[0]: VST_SRS $cml3, ...      (reads cml3, sub-reg of dm3)
  auto Instrs0 = getInstrs(MF->getBlockNumbered(0));
  auto Instrs1 = getInstrs(MF->getBlockNumbered(1));
  ASSERT_GE(Instrs0.size(), 1u);
  ASSERT_GE(Instrs1.size(), 1u);

  BlockState BS(MF->getBlockNumbered(0));
  // Clone 0 defines dm3, Clone 1 (a VST_SRS) reads cml3.
  // cml3 overlaps dm3 -> dm3 should be rejected.
  BS.TopInsert.Clones = {Instrs0[0], Instrs1[0]};
  BS.TopInsert.Bundles = {MachineBundle({Instrs0[0]}, nullptr),
                          MachineBundle({Instrs1[0]}, nullptr)};

  auto LastDefs = SC.Solver->findLastDefsInClones(BS);

  MCRegister DM3 = findReg(MF, "dm3");
  EXPECT_FALSE(LastDefs.count(DM3))
      << "dm3 should be rejected: another clone reads cml3 (overlaps dm3)";
}

// =====================================================================
// collectFreeStoreUses tests
// =====================================================================

/// Normal store->load pattern: stores first, then loads that completely
/// redefine the register. Should return the store uses.
TEST_F(OLPWARSolverTest, CollectFreeStoreUses_NormalPattern) {
  parseMIR(TestMIR);
  auto *MF = getMachineFunction("war_solver_test");
  auto SC = createSolver(MF);

  // bb.1: VST $cml3, VST $cmh3, VLDA $cml3, VLDA $cmh3
  auto Instrs = getInstrs(MF->getBlockNumbered(1));
  ASSERT_EQ(Instrs.size(), 4u);

  MCRegister DM3 = findReg(MF, "dm3");
  auto FreeUses = SC.Solver->collectFreeStoreUses(DM3, Instrs);

  // Should return store uses (cml3 and cmh3 operands from the two VST_SRS).
  EXPECT_FALSE(FreeUses.empty())
      << "Normal store->load pattern should return free uses";
  EXPECT_EQ(FreeUses.size(), 2u)
      << "Should have 2 free uses (one for cml3, one for cmh3)";
}

/// Interleaved pattern: store, load, store, load. The second store
/// comes after the first load -> rejected.
TEST_F(OLPWARSolverTest, CollectFreeStoreUses_InterleavedRejected) {
  parseMIR(TestMIR);
  auto *MF = getMachineFunction("war_solver_test");
  auto SC = createSolver(MF);

  // bb.2: VST $cml3, VLDA $cml3, VST $cmh3, VLDA $cmh3
  auto Instrs = getInstrs(MF->getBlockNumbered(2));
  ASSERT_EQ(Instrs.size(), 4u);

  MCRegister DM3 = findReg(MF, "dm3");
  auto FreeUses = SC.Solver->collectFreeStoreUses(DM3, Instrs);

  // Phase 2: VST $cmh3 reads cmh3 (overlaps dm3) without writing ->
  // "ReadsReg && !WritesReg" in Phase 2 -> rejected.
  EXPECT_TRUE(FreeUses.empty())
      << "Interleaved store/load pattern should be rejected";
}

/// Non-load writer: a VMAC redefines dm3 (not a load instruction).
/// Should be rejected.
TEST_F(OLPWARSolverTest, CollectFreeStoreUses_NonLoadWriterRejected) {
  parseMIR(TestMIR);
  auto *MF = getMachineFunction("war_solver_test");
  auto SC = createSolver(MF);

  // bb.3: VST $cml3, VMAC $dm3 (VMAC writes dm3 but !mayLoad())
  auto Instrs = getInstrs(MF->getBlockNumbered(3));
  ASSERT_EQ(Instrs.size(), 2u);

  MCRegister DM3 = findReg(MF, "dm3");
  auto FreeUses = SC.Solver->collectFreeStoreUses(DM3, Instrs);

  // Phase 1: VMAC writes dm3 -> transition to Phase 2, but VMAC is
  // not mayLoad() -> rejected.
  EXPECT_TRUE(FreeUses.empty()) << "Non-load writer (VMAC) should be rejected";
}

/// Incomplete register unit coverage: only cml3 is redefined by loads,
/// but cmh3 is never redefined. The final check should fail.
TEST_F(OLPWARSolverTest, CollectFreeStoreUses_IncompleteCoverageRejected) {
  parseMIR(TestMIR);
  auto *MF = getMachineFunction("war_solver_test");
  auto SC = createSolver(MF);

  // bb.4: VST $cml3, VLDA $cml3 (only cml3, no cmh3 load)
  // Use only the first 2 instructions (VST + VLDA), skipping the RET.
  auto AllInstrs = getInstrs(MF->getBlockNumbered(4));
  ASSERT_GE(AllInstrs.size(), 2u);
  SmallVector<MachineInstr *, 2> Instrs = {AllInstrs[0], AllInstrs[1]};

  MCRegister DM3 = findReg(MF, "dm3");
  auto FreeUses = SC.Solver->collectFreeStoreUses(DM3, Instrs);

  // DefinedUnits only covers cml3's register units, not cmh3's ->
  // the final "every register unit covered" check fails.
  EXPECT_TRUE(FreeUses.empty())
      << "Incomplete register unit coverage should be rejected";
}

/// No stores at all (only loads): FreeUses should be empty.
TEST_F(OLPWARSolverTest, CollectFreeStoreUses_NoStoresRejected) {
  parseMIR(TestMIR);
  auto *MF = getMachineFunction("war_solver_test");
  auto SC = createSolver(MF);

  // Extract only the VLDA instructions from bb.1 (indices 2 and 3).
  auto AllInstrs = getInstrs(MF->getBlockNumbered(1));
  ASSERT_GE(AllInstrs.size(), 4u);
  SmallVector<MachineInstr *, 2> OnlyLoads = {AllInstrs[2], AllInstrs[3]};

  MCRegister DM3 = findReg(MF, "dm3");
  auto FreeUses = SC.Solver->collectFreeStoreUses(DM3, OnlyLoads);

  // No stores -> FreeUses is empty -> return {}.
  EXPECT_TRUE(FreeUses.empty())
      << "No store instructions should result in empty free uses";
}

/// An unrelated instruction that doesn't touch the register at all
/// should be skipped. Only the stores and loads matter.
TEST_F(OLPWARSolverTest, CollectFreeStoreUses_UnrelatedInstrSkipped) {
  parseMIR(TestMIR);
  auto *MF = getMachineFunction("war_solver_test");
  auto SC = createSolver(MF);

  // Build a custom array: bb.0 Instrs[1] is VMAC $dm2 (unrelated to dm3),
  // followed by the normal bb.1 store->load sequence.
  auto Instrs0 = getInstrs(MF->getBlockNumbered(0));
  auto Instrs1 = getInstrs(MF->getBlockNumbered(1));
  ASSERT_GE(Instrs0.size(), 2u);
  ASSERT_GE(Instrs1.size(), 4u);

  SmallVector<MachineInstr *, 5> Mixed = {
      Instrs0[1], // VMAC $dm2 (unrelated to dm3)
      Instrs1[0], // VST $cml3
      Instrs1[1], // VST $cmh3
      Instrs1[2], // VLDA $cml3
      Instrs1[3], // VLDA $cmh3
  };

  MCRegister DM3 = findReg(MF, "dm3");
  auto FreeUses = SC.Solver->collectFreeStoreUses(DM3, Mixed);

  // The VMAC $dm2 should be skipped (doesn't touch dm3).
  // The remaining store->load pattern is valid.
  EXPECT_FALSE(FreeUses.empty())
      << "Unrelated instruction should be skipped; pattern should be accepted";
  EXPECT_EQ(FreeUses.size(), 2u);
}

/// Verify that the VST_SRS instruction has mayStore() set and the
/// VLDA_UPS instruction has mayLoad() set by tablegen.
TEST_F(OLPWARSolverTest, InstructionFlags_MayLoadMayStore) {
  parseMIR(TestMIR);
  auto *MF = getMachineFunction("war_solver_test");

  auto Instrs = getInstrs(MF->getBlockNumbered(1));
  ASSERT_EQ(Instrs.size(), 4u);

  // VST_SRS should be a store.
  EXPECT_TRUE(Instrs[0]->mayStore()) << "VST_SRS should have mayStore()";
  EXPECT_FALSE(Instrs[0]->mayLoad()) << "VST_SRS should not have mayLoad()";

  // VLDA_UPS should be a load.
  EXPECT_TRUE(Instrs[2]->mayLoad()) << "VLDA_UPS should have mayLoad()";
  EXPECT_FALSE(Instrs[2]->mayStore()) << "VLDA_UPS should not have mayStore()";

  // VMAC should be neither load nor store.
  auto Instrs0 = getInstrs(MF->getBlockNumbered(0));
  EXPECT_FALSE(Instrs0[0]->mayLoad()) << "VMAC should not have mayLoad()";
  EXPECT_FALSE(Instrs0[0]->mayStore()) << "VMAC should not have mayStore()";
}

/// Verify that cml3 and cmh3 are sub-registers of dm3 (the real
/// register hierarchy).
TEST_F(OLPWARSolverTest, RegisterHierarchy_DmSubRegisters) {
  parseMIR(TestMIR);
  auto *MF = getMachineFunction("war_solver_test");
  const auto &TRI = *MF->getSubtarget().getRegisterInfo();

  MCRegister DM3 = findReg(MF, "dm3");
  MCRegister CML3 = findReg(MF, "cml3");
  MCRegister CMH3 = findReg(MF, "cmh3");

  EXPECT_TRUE(TRI.regsOverlap(DM3, CML3)) << "dm3 and cml3 should overlap";
  EXPECT_TRUE(TRI.regsOverlap(DM3, CMH3)) << "dm3 and cmh3 should overlap";
  EXPECT_FALSE(TRI.regsOverlap(CML3, CMH3))
      << "cml3 and cmh3 should not overlap";

  // cml3 and cmh3 should be sub-registers of dm3.
  EXPECT_NE(TRI.getSubRegIndex(DM3, CML3), 0u)
      << "cml3 should be a sub-register of dm3";
  EXPECT_NE(TRI.getSubRegIndex(DM3, CMH3), 0u)
      << "cmh3 should be a sub-register of dm3";
}

} // anonymous namespace
