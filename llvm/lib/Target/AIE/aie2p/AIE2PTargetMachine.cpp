//===------ AIE2PTargetMachine.cpp - Define TargetMachine for AIE2p ------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Implements the info about AIE2p target spec.
//
//===----------------------------------------------------------------------===//

#include "AIE2PTargetMachine.h"
#include "AIE2PTargetTransformInfo.h"
#include "llvm/CodeGen/LiveInterval.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

using namespace llvm;
extern cl::opt<bool> EnableStagedRA;
extern cl::opt<bool> EnableSuperRegSplitting;
extern cl::opt<bool> AllocateMRegsFirst;
extern cl::opt<bool> EnablePreMISchedCoalescer;
extern cl::opt<bool> EnableAddressChaining;
extern cl::opt<bool> EnableGlobalPtrModOptimizer;
extern cl::opt<bool> EnableWAWRegRewrite;

void AIE2PTargetMachine::anchor() {}

AIE2PTargetMachine::AIE2PTargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       std::optional<Reloc::Model> RM,
                                       std::optional<CodeModel::Model> CM,
                                       CodeGenOptLevel OL, bool JIT)
    : AIEBaseTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, false),
      Subtarget(TT, StringRef("aie2p"), StringRef("aie2p"), FS,
                Options.MCOptions.getABIName(), *this) {
  setGlobalISel(true);
  setFastISel(false);
  setGlobalISelAbort(GlobalISelAbortMode::Enable);
}

// AIE2P Pass Setup
class AIE2PPassConfig final : public AIE2PassConfig {
public:
  AIE2PPassConfig(LLVMTargetMachine &TM, PassManagerBase &PM)
      : AIE2PassConfig(TM, PM) {}
  void addPreRegBankSelect() override;
  void addPreLegalizeMachineIR() override;
  bool addRegAssignAndRewriteOptimized() override;
};

void AIE2PPassConfig::addPreLegalizeMachineIR() {
  addPass(createAIEAddressSpaceFlattening());
  if (getOptLevel() != CodeGenOptLevel::None)
    addPass(createAIE2PPreLegalizerCombiner());
  addPass(createAIEEliminateDuplicatePHI());
}

void AIE2PPassConfig::addPreRegBankSelect() {
  if (getOptLevel() != CodeGenOptLevel::None) {
    addPass(createAIE2PPostLegalizerGenericCombiner());
    if (EnableAddressChaining)
      addPass(createAIEClusterBaseAddress());
    if (EnableGlobalPtrModOptimizer)
      addPass(createAIEPtrModOptimizer());
    addPass(createAIE2PPostLegalizerCustomCombiner());
  }
}

static bool onlyAllocateLIwith3DInstruction(MachineRegisterInfo &MRI,
                                            const TargetInstrInfo &TII,
                                            const LiveInterval *LI) {
  const Register Reg = LI->reg();
  return std::any_of(
      MRI.use_nodbg_instructions(Reg).begin(),
      MRI.use_nodbg_instructions(Reg).end(), [&](const MachineInstr &MI) {
        switch (MI.getOpcode()) {
        case AIE2P::LDA_3D_dms_lda:
        case AIE2P::LDA_3D_dmv_lda_q:
        case AIE2P::LDA_3D_s16:
        case AIE2P::LDA_3D_s8:
        case AIE2P::LDA_3D_u16:
        case AIE2P::LDA_3D_u8:
        case AIE2P::LDA_TM_3D:
        case AIE2P::ST_3D_dms_sts:
        case AIE2P::ST_3D_dmv_sts_q:
        case AIE2P::ST_3D_s16:
        case AIE2P::ST_3D_s8:
        case AIE2P::ST_TM_3D:
        case AIE2P::VLDA_3D_128:
        case AIE2P::VLDA_3D_CONV_fp32_bf16_dmw_lda_ups_bf:
        case AIE2P::VLDA_3D_CONV_fp32_bf16_dmx_lda_ups_bf:
        case AIE2P::VLDA_3D_dmw_lda_w:
        case AIE2P::VLDA_3D_dmx_lda_bm:
        case AIE2P::VLDA_3D_dmx_lda_fifohl:
        case AIE2P::VLDA_3D_dmx_lda_x:
        case AIE2P::VLDB_3D_128:
        case AIE2P::VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign0:
        case AIE2P::VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign1:
        case AIE2P::VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign0:
        case AIE2P::VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign1:
        case AIE2P::VLDB_3D_dmw_ldb:
        case AIE2P::VLDB_3D_dmx_ldb_x:
        case AIE2P::VST_3D_128:
        case AIE2P::VST_3D_CONV_bf16_fp32_dmw_sts_srs_bf:
        case AIE2P::VST_3D_CONV_bf16_fp32_dmx_sts_srs_bf:
        case AIE2P::VST_3D_PACK_dmw_sts_pack_packSign0:
        case AIE2P::VST_3D_PACK_dmw_sts_pack_packSign1:
        case AIE2P::VST_3D_PACK_dmx_sts_pack_packSign0:
        case AIE2P::VST_3D_PACK_dmx_sts_pack_packSign1:
        case AIE2P::VST_3D_dmw_sts_w:
        case AIE2P::VST_3D_dmx_sts_bm:
        case AIE2P::VST_3D_dmx_sts_fifohl:
        case AIE2P::VST_3D_dmx_sts_x:
        case AIE2P::VLD_3D_w_pseudo:
        case AIE2P::VLD_3D_x_pseudo:
        case AIE2P::VLD_3D_128_pseudo:
        case AIE2P::PADDA_3D:
        case AIE2P::PADDB_3D:
        case AIE2P::PADDS_3D:
        case AIE2P::PADD_3D_pseudo:
        case AIE2P::VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign1:
        case AIE2P::VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign0:
        case AIE2P::VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign1:
        case AIE2P::VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign0:
        case AIE2P::VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign1:
        case AIE2P::VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign0:
        case AIE2P::VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign1:
        case AIE2P::VST_3D_SRS_2x_dm_sts_srs_cm_srsSign0:
        case AIE2P::VST_3D_SRS_2x_dm_sts_srs_cm_srsSign1:
        case AIE2P::VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign0:
        case AIE2P::VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign1:
        case AIE2P::VST_3D_SRS_4x_dm_sts_srs_cm_srsSign0:
        case AIE2P::VST_3D_SRS_4x_dm_sts_srs_cm_srsSign1:
        case AIE2P::VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign0:
        case AIE2P::VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign1:
        case AIE2P::VST_FLUSH_512_3D:
        case AIE2P::VST_FLUSH_512_CONV_3D:
        case AIE2P::VLDA_POP_512_3D:
        case AIE2P::VLDA_POP_544_3D:
        case AIE2P::VLDA_POP_576_3D:
        case AIE2P::VLDA_POP_640_3D:
        case AIE2P::VLDA_POP_704_3D:
        case AIE2P::VLDB_POP_512_3D:
        case AIE2P::VLDB_POP_544_3D:
        case AIE2P::VLDB_POP_576_3D:
        case AIE2P::VLDB_POP_640_3D:
        case AIE2P::VLDB_POP_704_3D:
        case AIE2P::VLD_POP_512_3D_pseudo:
        case AIE2P::VLD_POP_544_3D_pseudo:
        case AIE2P::VLD_POP_576_3D_pseudo:
        case AIE2P::VLD_POP_640_3D_pseudo:
        case AIE2P::VLD_POP_704_3D_pseudo:
        case AIE2P::LDA_3D_dms_lda_split:
        case AIE2P::LDA_3D_dmv_lda_q_split:
        case AIE2P::LDA_3D_s16_split:
        case AIE2P::LDA_3D_s8_split:
        case AIE2P::LDA_3D_u16_split:
        case AIE2P::LDA_3D_u8_split:
        case AIE2P::LDA_TM_3D_split:
        case AIE2P::ST_3D_dms_sts_split:
        case AIE2P::ST_3D_dmv_sts_q_split:
        case AIE2P::ST_3D_s16_split:
        case AIE2P::ST_3D_s8_split:
        case AIE2P::ST_TM_3D_split:
        case AIE2P::VLDA_3D_128_split:
        case AIE2P::VLDA_3D_CONV_fp32_bf16_dmw_lda_ups_bf_split:
        case AIE2P::VLDA_3D_CONV_fp32_bf16_dmx_lda_ups_bf_split:
        case AIE2P::VLDA_3D_dmw_lda_w_split:
        case AIE2P::VLDA_3D_dmx_lda_bm_split:
        case AIE2P::VLDA_3D_dmx_lda_fifohl_split:
        case AIE2P::VLDA_3D_dmx_lda_x_split:
        case AIE2P::VLDB_3D_128_split:
        case AIE2P::VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign0_split:
        case AIE2P::VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign1_split:
        case AIE2P::VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign0_split:
        case AIE2P::VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign1_split:
        case AIE2P::VLDB_3D_dmw_ldb_split:
        case AIE2P::VLDB_3D_dmx_ldb_x_split:
        case AIE2P::VST_3D_128_split:
        case AIE2P::VST_3D_CONV_bf16_fp32_dmw_sts_srs_bf_split:
        case AIE2P::VST_3D_CONV_bf16_fp32_dmx_sts_srs_bf_split:
        case AIE2P::VST_3D_PACK_dmw_sts_pack_packSign0_split:
        case AIE2P::VST_3D_PACK_dmw_sts_pack_packSign1_split:
        case AIE2P::VST_3D_PACK_dmx_sts_pack_packSign0_split:
        case AIE2P::VST_3D_PACK_dmx_sts_pack_packSign1_split:
        case AIE2P::VST_3D_dmw_sts_w_split:
        case AIE2P::VST_3D_dmx_sts_bm_split:
        case AIE2P::VST_3D_dmx_sts_fifohl_split:
        case AIE2P::VST_3D_dmx_sts_x_split:
        case AIE2P::VLD_3D_w_pseudo_split:
        case AIE2P::VLD_3D_x_pseudo_split:
        case AIE2P::VLD_3D_128_pseudo_split:
        case AIE2P::PADDA_3D_split:
        case AIE2P::PADDB_3D_split:
        case AIE2P::PADDS_3D_split:
        case AIE2P::PADD_3D_pseudo_split:
        case AIE2P::VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign1_split:
        case AIE2P::VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign0_split:
        case AIE2P::VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign1_split:
        case AIE2P::VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign0_split:
        case AIE2P::VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign1_split:
        case AIE2P::VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign0_split:
        case AIE2P::VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign1_split:
        case AIE2P::VST_3D_SRS_2x_dm_sts_srs_cm_srsSign0_split:
        case AIE2P::VST_3D_SRS_2x_dm_sts_srs_cm_srsSign1_split:
        case AIE2P::VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign0_split:
        case AIE2P::VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign1_split:
        case AIE2P::VST_3D_SRS_4x_dm_sts_srs_cm_srsSign0_split:
        case AIE2P::VST_3D_SRS_4x_dm_sts_srs_cm_srsSign1_split:
        case AIE2P::VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign0_split:
        case AIE2P::VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign1_split:
        case AIE2P::VST_FLUSH_512_3D_split:
        case AIE2P::VST_FLUSH_512_CONV_3D_split:
        case AIE2P::VLDA_POP_512_3D_split:
        case AIE2P::VLDA_POP_544_3D_split:
        case AIE2P::VLDA_POP_576_3D_split:
        case AIE2P::VLDA_POP_640_3D_split:
        case AIE2P::VLDA_POP_704_3D_split:
        case AIE2P::VLDB_POP_512_3D_split:
        case AIE2P::VLDB_POP_544_3D_split:
        case AIE2P::VLDB_POP_576_3D_split:
        case AIE2P::VLDB_POP_640_3D_split:
        case AIE2P::VLDB_POP_704_3D_split:
        case AIE2P::VLD_POP_512_3D_pseudo_split:
        case AIE2P::VLD_POP_544_3D_pseudo_split:
        case AIE2P::VLD_POP_576_3D_pseudo_split:
        case AIE2P::VLD_POP_640_3D_pseudo_split:
        case AIE2P::VLD_POP_704_3D_pseudo_split:
          return true;
        default:
          return false;
        }
      });
}

static bool onlyAllocate3DRegisters(const TargetRegisterInfo &TRI,
                                    const TargetRegisterClass &RC) {
  return AIE2P::eDSRegClass.hasSubClassEq(&RC);
}
static bool onlyAllocate3D2DRegisters(const TargetRegisterInfo &TRI,
                                      const TargetRegisterClass &RC) {
  return AIE2P::eDSRegClass.hasSubClassEq(&RC) ||
         AIE2P::eDRegClass.hasSubClassEq(&RC);
}
static bool onlyAllocateMRegisters(const TargetRegisterInfo &TRI,
                                   const TargetRegisterClass &RC) {
  return AIE2P::eMRegClass.hasSubClassEq(&RC);
}

bool AIE2PPassConfig::addRegAssignAndRewriteOptimized() {

  // Pre-RA scheduling might have exposed simplifiable copies.
  if (EnablePreMISchedCoalescer)
    addPass(&RegisterCoalescerID);

  if (!EnableStagedRA && !EnableSuperRegSplitting)
    return TargetPassConfig::addRegAssignAndRewriteOptimized();

  // Rewrite instructions which use large tuple regs into _split variants
  // to better expose sub-registers and facilitate RA.
  if (EnableSuperRegSplitting)
    addPass(createAIESplitInstrBuilder());

  if (AllocateMRegsFirst)
    addPass(createGreedyRegisterAllocator(onlyAllocateMRegisters));
  if (EnableStagedRA) {
    addPass(createGreedyRegisterAllocator(onlyAllocate3DRegisters,
                                          onlyAllocateLIwith3DInstruction));
    addPass(createAIESuperRegRewriter());
    addPass(createGreedyRegisterAllocator(onlyAllocate3D2DRegisters));
    addPass(createAIESuperRegRewriter());
  }
  addPass(createGreedyRegisterAllocator());
  if (EnableWAWRegRewrite) {
    addPass(createAIEWawRegRewriter());
    addPass(createGreedyRegisterAllocator());
  }
  addPass(createVirtRegRewriter());

  return true;
}

TargetPassConfig *AIE2PTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new AIE2PPassConfig(*this, PM);
}

TargetTransformInfo
AIE2PTargetMachine::getTargetTransformInfo(const Function &F) const {
  return TargetTransformInfo(AIE2PTTIImpl(this, F));
}
