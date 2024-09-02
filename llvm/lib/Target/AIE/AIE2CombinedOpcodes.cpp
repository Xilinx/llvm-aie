//===- AIE2CombinedOpcodes.cpp -----------------------------------*- C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
/// \file
/// This is the AIE2 combined opcodes class implementation.
//===----------------------------------------------------------------------===//

#include "AIE2CombinedOpcodes.h"
#include "MCTargetDesc/AIE2MCTargetDesc.h"
#include "Utils/AIESelectionUtils.h"
#include "llvm/CodeGen/GlobalISel/GenericMachineInstrs.h"
#include "llvm/CodeGen/GlobalISel/Utils.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/IR/IntrinsicsAIE.h"
#include "llvm/IR/IntrinsicsAIE2.h"

#define DEBUG_TYPE "aie2-combined-opcodes"

using namespace llvm;
using namespace llvm::AIESelectionUtils;

static bool getVLDA_CONVOpcode(const MachineInstr &MemOp,
                               const std::optional<APInt> Immediate,
                               unsigned &ISelOpcode, bool &FitsImmediateRange) {
  if (!MemOp.mayLoad())
    return false;

  switch (MemOp.getOpcode()) {
  case AIE2::G_LOAD:
    ISelOpcode = AIE2::VLDA_CONV_FP32_BF16_ag_idx_imm;
    FitsImmediateRange = true;
    return true;
  case AIE2::G_AIE_OFFSET_LOAD:
    FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
    ISelOpcode = FitsImmediateRange ? AIE2::VLDA_CONV_FP32_BF16_ag_idx_imm
                                    : AIE2::VLDA_CONV_FP32_BF16_ag_idx;
    return true;
  case AIE2::G_AIE_POSTINC_LOAD:
    FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
    ISelOpcode = FitsImmediateRange ? AIE2::VLDA_CONV_FP32_BF16_pstm_nrm_imm
                                    : AIE2::VLDA_CONV_FP32_BF16_pstm_nrm;
    return true;
  case AIE2::G_AIE_POSTINC_2D_LOAD:
    ISelOpcode = AIE2::VLDA_2D_CONV_FP32_BF16;
    FitsImmediateRange = false;
    return true;
  case AIE2::G_AIE_POSTINC_3D_LOAD:
    ISelOpcode = AIE2::VLDA_3D_CONV_FP32_BF16;
    FitsImmediateRange = false;
    return true;
  }
  return false;
}

std::optional<LoadStoreOpcodes> AIE2CombinedOpcodes::getCombinedOpcodeCONV(
    const MachineInstr &MemOp, const MachineInstr &CombOp,
    std::optional<APInt> Immediate) const {
  const bool AlwaysFitsImmediateRange = true;
  const bool NoImmediate = false;
  if (CombOp.getOpcode() != AIE2::G_INTRINSIC_W_SIDE_EFFECTS ||
      cast<GIntrinsic>(CombOp).getIntrinsicID() !=
          Intrinsic::aie2_v16accfloat_to_v16bf16)
    return {};

  assert(getLoadStoreSize(MemOp) == 256 && "Unexpected VST.CONV size");

  unsigned ISelOpcode;
  bool FitsImmediateRange = false;
  switch (MemOp.getOpcode()) {
  case AIE2::G_STORE:
    return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_CONV_BF16_FP32_ag_idx_imm,
                            AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
  case AIE2::G_AIE_OFFSET_STORE:
    FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
    ISelOpcode = FitsImmediateRange ? AIE2::VST_CONV_BF16_FP32_ag_idx_imm
                                    : AIE2::VST_CONV_BF16_FP32_ag_idx;
    return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                            /*OffsetOpcode=*/{}};
  case AIE2::G_AIE_POSTINC_STORE:
    FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
    ISelOpcode = FitsImmediateRange ? AIE2::VST_CONV_BF16_FP32_ag_pstm_nrm_imm
                                    : AIE2::VST_CONV_BF16_FP32_ag_pstm_nrm;
    return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                            /*OffsetOpcode=*/{}};
  case AIE2::G_AIE_POSTINC_2D_STORE:
    return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_CONV_2D_BF16_FP32,
                            NoImmediate,
                            /*OffsetOpcode=*/{}};
  case AIE2::G_AIE_POSTINC_3D_STORE:
    return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_CONV_3D_BF16_FP32,
                            NoImmediate,
                            /*OffsetOpcode=*/{}};
  }
  return {};
}

std::optional<LoadStoreOpcodes> AIE2CombinedOpcodes::getCombinedOpcodeCONVLoad(
    const MachineInstr &MemOp, const MachineInstr &CombOp,
    const std::optional<APInt> Immediate) const {

  if (CombOp.getOpcode() != AIE2::G_INTRINSIC ||
      cast<GIntrinsic>(CombOp).getIntrinsicID() !=
          Intrinsic::aie2_v16bf16_to_v16accfloat)
    return {};

  unsigned ISelOpcode;
  bool FitsImmediateRange = false;

  if (!getVLDA_CONVOpcode(MemOp, Immediate, ISelOpcode, FitsImmediateRange))
    return {};

  assert(getLoadStoreSize(MemOp) == 256 && "Unexpected VLDA.CONV size");

  return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange, /*OffsetOpcode=*/{}};
}

std::optional<LoadStoreOpcodes> AIE2CombinedOpcodes::getCombinedOpcodePACK(
    const MachineInstr &MemOp, const MachineInstr &CombOp,
    std::optional<APInt> Immediate, bool IsSigned, bool Is32Lanes) const {
  const bool AlwaysFitsImmediateRange = true;

  if (CombOp.getOpcode() != AIE2::G_INTRINSIC_W_SIDE_EFFECTS ||
      (cast<GIntrinsic>(CombOp).getIntrinsicID() !=
           Intrinsic::aie2_pack_I4_I8 &&
       cast<GIntrinsic>(CombOp).getIntrinsicID() !=
           Intrinsic::aie2_pack_I8_I16))
    return {};

  assert(getLoadStoreSize(MemOp) == 256 && "Unexpected VST.PACK size");

  unsigned ISelOpcode;
  bool FitsImmediateRange = false;
  const bool NoImmediate = false;

  if (IsSigned) {
    switch (MemOp.getOpcode()) {
    case AIE2::G_STORE:
      if (Is32Lanes) {
        return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_PACK_S8_S16_ag_idx_imm,
                                AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
      } else {
        return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_PACK_S4_S8_ag_idx_imm,
                                AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
      }
      break;
    case AIE2::G_AIE_OFFSET_STORE:
      if (Is32Lanes) {
        FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
        ISelOpcode = FitsImmediateRange ? AIE2::VST_PACK_S8_S16_ag_idx_imm
                                        : AIE2::VST_PACK_S8_S16_ag_idx;
        return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                /*OffsetOpcode=*/{}};
      } else {
        FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
        ISelOpcode = FitsImmediateRange ? AIE2::VST_PACK_S4_S8_ag_idx_imm
                                        : AIE2::VST_PACK_S4_S8_ag_idx;
        return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                /*OffsetOpcode=*/{}};
      }
      break;
    case AIE2::G_AIE_POSTINC_STORE:
      FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
      if (Is32Lanes) {
        ISelOpcode = FitsImmediateRange ? AIE2::VST_PACK_S8_S16_ag_pstm_nrm_imm
                                        : AIE2::VST_PACK_S8_S16_ag_pstm_nrm;
        return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                /*OffsetOpcode=*/{}};
      } else {
        ISelOpcode = FitsImmediateRange ? AIE2::VST_PACK_S4_S8_ag_pstm_nrm_imm
                                        : AIE2::VST_PACK_S4_S8_ag_pstm_nrm;
        return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                /*OffsetOpcode=*/{}};
      }
      break;
    case AIE2::G_AIE_POSTINC_2D_STORE:
      if (Is32Lanes) {
        return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_2D_PACK_S8_S16,
                                NoImmediate,
                                /*OffsetOpcode=*/{}};
      } else {
        return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_2D_PACK_S4_S8,
                                NoImmediate,
                                /*OffsetOpcode=*/{}};
      }
      break;
    case AIE2::G_AIE_POSTINC_3D_STORE:
      if (Is32Lanes) {
        return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_3D_PACK_S8_S16,
                                NoImmediate,
                                /*OffsetOpcode=*/{}};
      } else {
        return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_3D_PACK_S4_S8,
                                NoImmediate,
                                /*OffsetOpcode=*/{}};
      }
      break;
    default:
      return {};
    }
  } else {
    switch (MemOp.getOpcode()) {
    case AIE2::G_STORE:
      if (Is32Lanes) {
        return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_PACK_D8_D16_ag_idx_imm,
                                AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
      } else {
        return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_PACK_D4_D8_ag_idx_imm,
                                AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
      }
      break;
    case AIE2::G_AIE_OFFSET_STORE:
      if (Is32Lanes) {
        FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
        ISelOpcode = FitsImmediateRange ? AIE2::VST_PACK_D8_D16_ag_idx_imm
                                        : AIE2::VST_PACK_D8_D16_ag_idx;
        return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                /*OffsetOpcode=*/{}};
      } else {
        FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
        ISelOpcode = FitsImmediateRange ? AIE2::VST_PACK_D4_D8_ag_idx_imm
                                        : AIE2::VST_PACK_D4_D8_ag_idx;
        return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                /*OffsetOpcode=*/{}};
      }
      break;
    case AIE2::G_AIE_POSTINC_STORE:
      FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
      if (Is32Lanes) {
        ISelOpcode = FitsImmediateRange ? AIE2::VST_PACK_D8_D16_ag_pstm_nrm_imm
                                        : AIE2::VST_PACK_D8_D16_ag_pstm_nrm;
        return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                /*OffsetOpcode=*/{}};
      } else {
        ISelOpcode = FitsImmediateRange ? AIE2::VST_PACK_D4_D8_ag_pstm_nrm_imm
                                        : AIE2::VST_PACK_D4_D8_ag_pstm_nrm;
        return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                /*OffsetOpcode=*/{}};
      }
      break;
    case AIE2::G_AIE_POSTINC_2D_STORE:
      if (Is32Lanes) {
        return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_2D_PACK_D8_D16,
                                NoImmediate,
                                /*OffsetOpcode=*/{}};
      } else {
        return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_2D_PACK_D4_D8,
                                NoImmediate,
                                /*OffsetOpcode=*/{}};
      }
      break;
    case AIE2::G_AIE_POSTINC_3D_STORE:
      if (Is32Lanes) {
        return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_3D_PACK_D8_D16,
                                NoImmediate,
                                /*OffsetOpcode=*/{}};
      } else {
        return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_3D_PACK_D4_D8,
                                NoImmediate,
                                /*OffsetOpcode=*/{}};
      }
      break;
    default:
      return {};
    }
  }
  return {};
}

std::optional<LoadStoreOpcodes>
AIE2CombinedOpcodes::getCombinedOpcodeUNPACKLoad(
    const MachineInstr &MemOp, const MachineInstr &CombOp,
    std::optional<APInt> Immediate, MachineRegisterInfo &MRI) const {
  const bool NoImmediate = false;
  if (CombOp.getOpcode() != AIE2::G_INTRINSIC ||
      (cast<GIntrinsic>(CombOp).getIntrinsicID() !=
           Intrinsic::aie2_unpack_I8_I4 &&
       cast<GIntrinsic>(CombOp).getIntrinsicID() !=
           Intrinsic::aie2_unpack_I16_I8))
    return {};

  if (!MemOp.mayLoad())
    return {};

  assert(getLoadStoreSize(MemOp) == 256 && "Unexpected VLDA.UNPACK size");

  unsigned ISelOpcode;
  Register SignReg = CombOp.getOperand(3).getReg();

  auto Sign = getIConstantVRegValWithLookThrough(SignReg, MRI);
  if (Sign && Sign->Value.getZExtValue()) {
    if (cast<GIntrinsic>(CombOp).getIntrinsicID() ==
        Intrinsic::aie2_unpack_I8_I4) {
      switch (MemOp.getOpcode()) {
      case AIE2::G_AIE_OFFSET_LOAD:
        ISelOpcode = AIE2::VLDB_UNPACK_S8_S4_ag_idx;
        return LoadStoreOpcodes{ISelOpcode, NoImmediate, {}};
      case AIE2::G_AIE_POSTINC_LOAD:
        ISelOpcode = AIE2::VLDB_UNPACK_S8_S4_ag_pstm_nrm;
        return LoadStoreOpcodes{ISelOpcode, NoImmediate, {}};
      case AIE2::G_AIE_POSTINC_2D_LOAD:
        ISelOpcode = AIE2::VLDB_2D_UNPACK_S8_S4;
        return LoadStoreOpcodes{ISelOpcode, NoImmediate, {}};
      case AIE2::G_AIE_POSTINC_3D_LOAD:
        ISelOpcode = AIE2::VLDB_3D_UNPACK_S8_S4;
        return LoadStoreOpcodes{ISelOpcode, NoImmediate, {}};
      }
    } else { // aie2_unpack_I16_I8
      switch (MemOp.getOpcode()) {
      case AIE2::G_AIE_OFFSET_LOAD:
        ISelOpcode = AIE2::VLDB_UNPACK_S16_S8_ag_idx;
        return LoadStoreOpcodes{ISelOpcode, NoImmediate, {}};
      case AIE2::G_AIE_POSTINC_LOAD:
        ISelOpcode = AIE2::VLDB_UNPACK_S16_S8_ag_pstm_nrm;
        return LoadStoreOpcodes{ISelOpcode, NoImmediate, {}};
      case AIE2::G_AIE_POSTINC_2D_LOAD:
        ISelOpcode = AIE2::VLDB_2D_UNPACK_S16_S8;
        return LoadStoreOpcodes{ISelOpcode, NoImmediate, {}};
      case AIE2::G_AIE_POSTINC_3D_LOAD:
        ISelOpcode = AIE2::VLDB_3D_UNPACK_S16_S8;
        return LoadStoreOpcodes{ISelOpcode, NoImmediate, {}};
      }
    }
  } else {
    if (cast<GIntrinsic>(CombOp).getIntrinsicID() ==
        Intrinsic::aie2_unpack_I8_I4) {
      switch (MemOp.getOpcode()) {
      case AIE2::G_AIE_OFFSET_LOAD:
        ISelOpcode = AIE2::VLDB_UNPACK_D8_D4_ag_idx;
        return LoadStoreOpcodes{ISelOpcode, NoImmediate, {}};
      case AIE2::G_AIE_POSTINC_LOAD:
        ISelOpcode = AIE2::VLDB_UNPACK_D8_D4_pstm_nrm;
        return LoadStoreOpcodes{ISelOpcode, NoImmediate, {}};
      case AIE2::G_AIE_POSTINC_2D_LOAD:
        ISelOpcode = AIE2::VLDB_2D_UNPACK_D8_D4;
        return LoadStoreOpcodes{ISelOpcode, NoImmediate, {}};
      case AIE2::G_AIE_POSTINC_3D_LOAD:
        ISelOpcode = AIE2::VLDB_3D_UNPACK_D8_D4;
        return LoadStoreOpcodes{ISelOpcode, NoImmediate, {}};
      }
    } else { // aie2_unpack_I16_I8
      switch (MemOp.getOpcode()) {
      case AIE2::G_AIE_OFFSET_LOAD:
        ISelOpcode = AIE2::VLDB_UNPACK_D16_D8_ag_idx;
        return LoadStoreOpcodes{ISelOpcode, NoImmediate, {}};
      case AIE2::G_AIE_POSTINC_LOAD:
        ISelOpcode = AIE2::VLDB_UNPACK_D16_D8_ag_pstm_nrm;
        return LoadStoreOpcodes{ISelOpcode, NoImmediate, {}};
      case AIE2::G_AIE_POSTINC_2D_LOAD:
        ISelOpcode = AIE2::VLDB_2D_UNPACK_D16_D8;
        return LoadStoreOpcodes{ISelOpcode, NoImmediate, {}};
      case AIE2::G_AIE_POSTINC_3D_LOAD:
        ISelOpcode = AIE2::VLDB_3D_UNPACK_D16_D8;
        return LoadStoreOpcodes{ISelOpcode, NoImmediate, {}};
      }
    }
  }
  return {};
}

std::optional<LoadStoreOpcodes> AIE2CombinedOpcodes::getCombinedOpcodeSRSUPS(
    const MachineInstr &MemOp, const MachineInstr &CombOp,
    std::optional<APInt> Immediate, bool IsSigned) const {
  const bool AlwaysFitsImmediateRange = true;
  const bool NoImmediate = false;
  if (CombOp.getOpcode() != AIE2::G_INTRINSIC_W_SIDE_EFFECTS)
    return {};

  unsigned ISelOpcode;
  bool FitsImmediateRange = false;
  if (IsSigned) {
    switch (MemOp.getOpcode()) {
    case AIE2::G_STORE:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I512_v32_acc32_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_SRS_S16_S32_ag_idx_imm,
              AlwaysFitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VST_SRS_S16_S32_ag_idx_imm};
        case Intrinsic::aie2_I512_v16_acc64_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_SRS_S32_S64_ag_idx_imm,
              AlwaysFitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VST_SRS_S32_S64_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I256_v16_acc32_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_SRS_S16_S32_ag_idx_imm,
              AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v16_acc64_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_SRS_S16_S64_ag_idx_imm,
              AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v32_acc32_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_SRS_S8_S32_ag_idx_imm,
              AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v8_acc64_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_SRS_S32_S64_ag_idx_imm,
              AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
        }
      }
      break;
    case AIE2::G_AIE_OFFSET_STORE:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I512_v32_acc32_srs:
          FitsImmediateRange =
              checkImmediateRangeSplitting<3, 32, 32>(Immediate);
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_SRS_S16_S32_ag_idx_imm,
              FitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VST_SRS_S16_S32_ag_idx_imm};
        case Intrinsic::aie2_I512_v16_acc64_srs:
          FitsImmediateRange =
              checkImmediateRangeSplitting<3, 32, 32>(Immediate);
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_SRS_S32_S64_ag_idx_imm,
              FitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VST_SRS_S32_S64_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I256_v16_acc32_srs:
          FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
          ISelOpcode = FitsImmediateRange ? AIE2::VST_SRS_S16_S32_ag_idx_imm
                                          : AIE2::VST_SRS_S16_S32_ag_idx;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v16_acc64_srs:
          FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
          ISelOpcode = FitsImmediateRange ? AIE2::VST_SRS_S16_S64_ag_idx_imm
                                          : AIE2::VST_SRS_S16_S64_ag_idx;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v32_acc32_srs:
          FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
          ISelOpcode = FitsImmediateRange ? AIE2::VST_SRS_S8_S32_ag_idx_imm
                                          : AIE2::VST_SRS_S8_S32_ag_idx;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v8_acc64_srs:
          FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
          ISelOpcode = FitsImmediateRange ? AIE2::VST_SRS_S32_S64_ag_idx_imm
                                          : AIE2::VST_SRS_S32_S64_ag_idx;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        }
      }
      break;
    case AIE2::G_AIE_POSTINC_STORE:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I512_v32_acc32_srs:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VST_SRS_S16_S32_ag_pstm_nrm_imm
                           : AIE2::VST_SRS_S16_S32_ag_pstm_nrm;
          return LoadStoreOpcodes{
              ISelOpcode, FitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VST_SRS_S16_S32_ag_idx_imm};
        case Intrinsic::aie2_I512_v16_acc64_srs:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VST_SRS_S32_S64_ag_pstm_nrm_imm
                           : AIE2::VST_SRS_S32_S64_ag_pstm_nrm;
          return LoadStoreOpcodes{
              ISelOpcode, FitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VST_SRS_S32_S64_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I256_v16_acc32_srs:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VST_SRS_S16_S32_ag_pstm_nrm_imm
                           : AIE2::VST_SRS_S16_S32_ag_pstm_nrm;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v16_acc64_srs:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VST_SRS_S16_S64_ag_pstm_nrm_imm
                           : AIE2::VST_SRS_S16_S64_ag_pstm_nrm;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v32_acc32_srs:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange ? AIE2::VST_SRS_S8_S32_ag_pstm_nrm_imm
                                          : AIE2::VST_SRS_S8_S32_ag_pstm_nrm;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v8_acc64_srs:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VST_SRS_S32_S64_ag_pstm_nrm_imm
                           : AIE2::VST_SRS_S32_S64_ag_pstm_nrm;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        }
      }
      break;
    case AIE2::G_AIE_POSTINC_2D_STORE:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I512_v32_acc32_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_2D_SRS_S16_S32, NoImmediate,
              /*OffsetOpcode=*/AIE2::VST_SRS_S16_S32_ag_idx_imm};
        case Intrinsic::aie2_I512_v16_acc64_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_2D_SRS_S32_S64, NoImmediate,
              /*OffsetOpcode=*/AIE2::VST_SRS_S32_S64_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I256_v16_acc32_srs:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_2D_SRS_S16_S32,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v16_acc64_srs:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_2D_SRS_S16_S64,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v32_acc32_srs:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_2D_SRS_S8_S32,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v8_acc64_srs:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_2D_SRS_S32_S64,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        }
      }
      break;
    case AIE2::G_AIE_POSTINC_3D_STORE:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I512_v32_acc32_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_3D_SRS_S16_S32, NoImmediate,
              /*OffsetOpcode=*/AIE2::VST_SRS_S16_S32_ag_idx_imm};
        case Intrinsic::aie2_I512_v16_acc64_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_3D_SRS_S32_S64, NoImmediate,
              /*OffsetOpcode=*/AIE2::VST_SRS_S32_S64_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I256_v16_acc32_srs:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_3D_SRS_S16_S32,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v16_acc64_srs:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_3D_SRS_S16_S64,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v32_acc32_srs:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_3D_SRS_S8_S32,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v8_acc64_srs:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_3D_SRS_S32_S64,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        }
      }
      break;
    case AIE2::G_LOAD:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc64_v16_I512_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_UPS_S64_S32_ag_idx_imm,
              AlwaysFitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S64_S32_ag_idx_imm};
        case Intrinsic::aie2_acc32_v32_I512_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_UPS_S32_S16_ag_idx_imm,
              AlwaysFitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S32_S16_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc32_v16_I256_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_UPS_S32_S16_ag_idx_imm,
              AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v16_I256_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_UPS_S64_S16_ag_idx_imm,
              AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc32_v32_I256_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_UPS_S32_S8_ag_idx_imm,
              AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v8_I256_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_UPS_S64_S32_ag_idx_imm,
              AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
        }
      }
      break;
    case AIE2::G_AIE_OFFSET_LOAD:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc64_v16_I512_ups:
          FitsImmediateRange =
              checkImmediateRangeSplitting<3, 32, 32>(Immediate);
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_UPS_S64_S32_ag_idx_imm,
              FitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S64_S32_ag_idx_imm};
        case Intrinsic::aie2_acc32_v32_I512_ups:
          FitsImmediateRange =
              checkImmediateRangeSplitting<3, 32, 32>(Immediate);
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_UPS_S32_S16_ag_idx_imm,
              FitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S32_S16_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc32_v16_I256_ups:
          FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
          ISelOpcode = FitsImmediateRange ? AIE2::VLDA_UPS_S32_S16_ag_idx_imm
                                          : AIE2::VLDA_UPS_S32_S16_ag_idx;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v16_I256_ups:
          FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
          ISelOpcode = FitsImmediateRange ? AIE2::VLDA_UPS_S64_S16_ag_idx_imm
                                          : AIE2::VLDA_UPS_S64_S16_ag_idx;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc32_v32_I256_ups:
          FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
          ISelOpcode = FitsImmediateRange ? AIE2::VLDA_UPS_S32_S8_ag_idx_imm
                                          : AIE2::VLDA_UPS_S32_S8_ag_idx;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v8_I256_ups:
          FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
          ISelOpcode = FitsImmediateRange ? AIE2::VLDA_UPS_S64_S32_ag_idx_imm
                                          : AIE2::VLDA_UPS_S64_S32_ag_idx;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        }
      }
      break;
    case AIE2::G_AIE_POSTINC_LOAD:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc64_v16_I512_ups:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VLDA_UPS_S64_S32_ag_pstm_nrm_imm
                           : AIE2::VLDA_UPS_S64_S32_ag_pstm_nrm;
          return LoadStoreOpcodes{
              ISelOpcode, FitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S64_S32_ag_idx_imm};
        case Intrinsic::aie2_acc32_v32_I512_ups:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VLDA_UPS_S32_S16_ag_pstm_nrm_imm
                           : AIE2::VLDA_UPS_S32_S16_ag_pstm_nrm;
          return LoadStoreOpcodes{
              ISelOpcode, FitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S32_S16_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc32_v16_I256_ups:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VLDA_UPS_S32_S16_ag_pstm_nrm_imm
                           : AIE2::VLDA_UPS_S32_S16_ag_pstm_nrm;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v16_I256_ups:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VLDA_UPS_S64_S16_ag_pstm_nrm_imm
                           : AIE2::VLDA_UPS_S64_S16_ag_pstm_nrm;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc32_v32_I256_ups:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VLDA_UPS_S32_S8_ag_pstm_nrm_imm
                           : AIE2::VLDA_UPS_S32_S8_ag_pstm_nrm;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v8_I256_ups:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VLDA_UPS_S64_S32_ag_pstm_nrm_imm
                           : AIE2::VLDA_UPS_S64_S32_ag_pstm_nrm;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        }
      }
      break;
    case AIE2::G_AIE_POSTINC_2D_LOAD:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc32_v32_I512_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_2D_UPS_S32_S16, NoImmediate,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S32_S16_ag_idx_imm};
        case Intrinsic::aie2_acc64_v16_I512_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_2D_UPS_S64_S32, NoImmediate,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S64_S32_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc32_v16_I256_ups:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VLDA_2D_UPS_S32_S16,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v16_I256_ups:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VLDA_2D_UPS_S64_S16,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc32_v32_I256_ups:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VLDA_2D_UPS_S32_S8,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v8_I256_ups:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VLDA_2D_UPS_S64_S32,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        }
      }
      break;
    case AIE2::G_AIE_POSTINC_3D_LOAD:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc32_v32_I512_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_3D_UPS_S32_S16, NoImmediate,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S32_S16_ag_idx_imm};
        case Intrinsic::aie2_acc64_v16_I512_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_3D_UPS_S64_S32, NoImmediate,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S64_S32_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc32_v16_I256_ups:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VLDA_3D_UPS_S32_S16,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v16_I256_ups:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VLDA_3D_UPS_S64_S16,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc32_v32_I256_ups:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VLDA_3D_UPS_S32_S8,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v8_I256_ups:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VLDA_3D_UPS_S64_S32,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        }
      }
      break;
    }
    // isSigned
  } else {
    switch (MemOp.getOpcode()) {
    case AIE2::G_STORE:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I512_v32_acc32_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_SRS_D16_S32_ag_idx_imm,
              AlwaysFitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VST_SRS_D16_S32_ag_idx_imm};
        case Intrinsic::aie2_I512_v16_acc64_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_SRS_D32_S64_ag_idx_imm,
              AlwaysFitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VST_SRS_D32_S64_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I256_v16_acc32_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_SRS_D16_S32_ag_idx_imm,
              AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v16_acc64_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_SRS_D16_S64_ag_idx_imm,
              AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v32_acc32_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_SRS_D8_S32_ag_idx_imm,
              AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v8_acc64_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_SRS_D32_S64_ag_idx_imm,
              AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
        }
      }
      break;
    case AIE2::G_AIE_OFFSET_STORE:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I512_v32_acc32_srs:
          FitsImmediateRange =
              checkImmediateRangeSplitting<3, 32, 32>(Immediate);
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_SRS_D16_S32_ag_idx_imm,
              FitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VST_SRS_D16_S32_ag_idx_imm};
        case Intrinsic::aie2_I512_v16_acc64_srs:
          FitsImmediateRange =
              checkImmediateRangeSplitting<3, 32, 32>(Immediate);
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_SRS_D32_S64_ag_idx_imm,
              FitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VST_SRS_D32_S64_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I256_v16_acc32_srs:
          FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
          ISelOpcode = FitsImmediateRange ? AIE2::VST_SRS_D16_S32_ag_idx_imm
                                          : AIE2::VST_SRS_D16_S32_ag_idx;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v16_acc64_srs:
          FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
          ISelOpcode = FitsImmediateRange ? AIE2::VST_SRS_D16_S64_ag_idx_imm
                                          : AIE2::VST_SRS_D16_S64_ag_idx;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v32_acc32_srs:
          FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
          ISelOpcode = FitsImmediateRange ? AIE2::VST_SRS_D8_S32_ag_idx_imm
                                          : AIE2::VST_SRS_D8_S32_ag_idx;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v8_acc64_srs:
          FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
          ISelOpcode = FitsImmediateRange ? AIE2::VST_SRS_D32_S64_ag_idx_imm
                                          : AIE2::VST_SRS_D32_S64_ag_idx;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        }
      }
      break;
    case AIE2::G_AIE_POSTINC_STORE:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I512_v32_acc32_srs:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VST_SRS_D16_S32_ag_pstm_nrm_imm
                           : AIE2::VST_SRS_D16_S32_ag_pstm_nrm;
          return LoadStoreOpcodes{
              ISelOpcode, FitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VST_SRS_D16_S32_ag_idx_imm};
        case Intrinsic::aie2_I512_v16_acc64_srs:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VST_SRS_D32_S64_ag_pstm_nrm_imm
                           : AIE2::VST_SRS_D32_S64_ag_pstm_nrm;
          return LoadStoreOpcodes{
              ISelOpcode, FitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VST_SRS_D32_S64_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I256_v16_acc32_srs:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VST_SRS_D16_S32_ag_pstm_nrm_imm
                           : AIE2::VST_SRS_D16_S32_ag_pstm_nrm;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v16_acc64_srs:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VST_SRS_D16_S64_ag_pstm_nrm_imm
                           : AIE2::VST_SRS_D16_S64_ag_pstm_nrm;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v32_acc32_srs:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange ? AIE2::VST_SRS_D8_S32_ag_pstm_nrm_imm
                                          : AIE2::VST_SRS_D8_S32_ag_pstm_nrm;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v8_acc64_srs:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VST_SRS_D32_S64_ag_pstm_nrm_imm
                           : AIE2::VST_SRS_D32_S64_ag_pstm_nrm;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        }
      }
      break;
    case AIE2::G_AIE_POSTINC_2D_STORE:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I512_v32_acc32_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_2D_SRS_D16_S32, NoImmediate,
              /*OffsetOpcode=*/AIE2::VST_SRS_D16_S32_ag_idx_imm};
        case Intrinsic::aie2_I512_v16_acc64_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_2D_SRS_D32_S64, NoImmediate,
              /*OffsetOpcode=*/AIE2::VST_SRS_D32_S64_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I256_v16_acc32_srs:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_2D_SRS_D16_S32,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v16_acc64_srs:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_2D_SRS_D16_S64,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v32_acc32_srs:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_2D_SRS_D8_S32,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v8_acc64_srs:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_2D_SRS_D32_S64,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        }
      }
      break;
    case AIE2::G_AIE_POSTINC_3D_STORE:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I512_v32_acc32_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_3D_SRS_D16_S32, NoImmediate,
              /*OffsetOpcode=*/AIE2::VST_SRS_D16_S32_ag_idx_imm};
        case Intrinsic::aie2_I512_v16_acc64_srs:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VST_3D_SRS_D32_S64, NoImmediate,
              /*OffsetOpcode=*/AIE2::VST_SRS_D32_S64_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_I256_v16_acc32_srs:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_3D_SRS_D16_S32,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v16_acc64_srs:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_3D_SRS_D16_S64,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v32_acc32_srs:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_3D_SRS_D8_S32,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_I256_v8_acc64_srs:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VST_3D_SRS_D32_S64,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        }
      }
      break;
    case AIE2::G_LOAD:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc64_v16_I512_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_UPS_S64_D32_ag_idx_imm,
              AlwaysFitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S64_D32_ag_idx_imm};
        case Intrinsic::aie2_acc32_v32_I512_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_UPS_S32_D16_ag_idx_imm,
              AlwaysFitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S32_D16_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc32_v16_I256_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_UPS_S32_D16_ag_idx_imm,
              AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v16_I256_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_UPS_S64_D16_ag_idx_imm,
              AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc32_v32_I256_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_UPS_S32_D8_ag_idx_imm,
              AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v8_I256_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_UPS_S64_D32_ag_idx_imm,
              AlwaysFitsImmediateRange, /*OffsetOpcode=*/{}};
        }
      }
      break;
    case AIE2::G_AIE_OFFSET_LOAD:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc64_v16_I512_ups:
          FitsImmediateRange =
              checkImmediateRangeSplitting<3, 32, 32>(Immediate);
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_UPS_S64_D32_ag_idx_imm,
              FitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S64_D32_ag_idx_imm};
        case Intrinsic::aie2_acc32_v32_I512_ups:
          FitsImmediateRange =
              checkImmediateRangeSplitting<3, 32, 32>(Immediate);
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_UPS_S32_D16_ag_idx_imm,
              FitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S32_D16_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc32_v16_I256_ups:
          FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
          ISelOpcode = FitsImmediateRange ? AIE2::VLDA_UPS_S32_D16_ag_idx_imm
                                          : AIE2::VLDA_UPS_S32_D16_ag_idx;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v16_I256_ups:
          FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
          ISelOpcode = FitsImmediateRange ? AIE2::VLDA_UPS_S64_D16_ag_idx_imm
                                          : AIE2::VLDA_UPS_S64_D16_ag_idx;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc32_v32_I256_ups:
          FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
          ISelOpcode = FitsImmediateRange ? AIE2::VLDA_UPS_S32_D8_ag_idx_imm
                                          : AIE2::VLDA_UPS_S32_D8_ag_idx;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v8_I256_ups:
          FitsImmediateRange = checkImmediateRange<3, 32>(Immediate);
          ISelOpcode = FitsImmediateRange ? AIE2::VLDA_UPS_S64_D32_ag_idx_imm
                                          : AIE2::VLDA_UPS_S64_D32_ag_idx;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        }
      }
      break;
    case AIE2::G_AIE_POSTINC_LOAD:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc64_v16_I512_ups:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VLDA_UPS_S64_D32_ag_pstm_nrm_imm
                           : AIE2::VLDA_UPS_S64_D32_ag_pstm_nrm;
          return LoadStoreOpcodes{
              ISelOpcode, FitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S64_D32_ag_idx_imm};
        case Intrinsic::aie2_acc32_v32_I512_ups:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VLDA_UPS_S32_D16_ag_pstm_nrm_imm
                           : AIE2::VLDA_UPS_S32_D16_ag_pstm_nrm;
          return LoadStoreOpcodes{
              ISelOpcode, FitsImmediateRange,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S32_D16_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc32_v16_I256_ups:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VLDA_UPS_S32_D16_ag_pstm_nrm_imm
                           : AIE2::VLDA_UPS_S32_D16_ag_pstm_nrm;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v16_I256_ups:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VLDA_UPS_S64_D16_ag_pstm_nrm_imm
                           : AIE2::VLDA_UPS_S64_D16_ag_pstm_nrm;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc32_v32_I256_ups:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VLDA_UPS_S32_D8_ag_pstm_nrm_imm
                           : AIE2::VLDA_UPS_S32_D8_ag_pstm_nrm;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v8_I256_ups:
          FitsImmediateRange = checkImmediateRange<4, 32>(Immediate);
          ISelOpcode = FitsImmediateRange
                           ? AIE2::VLDA_UPS_S64_D32_ag_pstm_nrm_imm
                           : AIE2::VLDA_UPS_S64_D32_ag_pstm_nrm;
          return LoadStoreOpcodes{ISelOpcode, FitsImmediateRange,
                                  /*OffsetOpcode=*/{}};
        }
      }
      break;
    case AIE2::G_AIE_POSTINC_2D_LOAD:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc32_v32_I512_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_2D_UPS_S32_D16, NoImmediate,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S32_D16_ag_idx_imm};
        case Intrinsic::aie2_acc64_v16_I512_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_2D_UPS_S64_D32, NoImmediate,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S64_D32_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc32_v16_I256_ups:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VLDA_2D_UPS_S32_D16,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v16_I256_ups:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VLDA_2D_UPS_S64_D16,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc32_v32_I256_ups:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VLDA_2D_UPS_S32_D8,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v8_I256_ups:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VLDA_2D_UPS_S64_D32,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        }
      }
      break;
    case AIE2::G_AIE_POSTINC_3D_LOAD:
      if (getLoadStoreSize(MemOp) == 512) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc32_v32_I512_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_3D_UPS_S32_D16, NoImmediate,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S32_D16_ag_idx_imm};
        case Intrinsic::aie2_acc64_v16_I512_ups:
          return LoadStoreOpcodes{
              /*ISelOpcode=*/AIE2::VLDA_3D_UPS_S64_D32, NoImmediate,
              /*OffsetOpcode=*/AIE2::VLDA_UPS_S64_D32_ag_idx_imm};
        }
      }
      if (getLoadStoreSize(MemOp) == 256) {
        switch (cast<GIntrinsic>(CombOp).getIntrinsicID()) {
        case Intrinsic::aie2_acc32_v16_I256_ups:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VLDA_3D_UPS_S32_D16,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v16_I256_ups:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VLDA_3D_UPS_S64_D16,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc32_v32_I256_ups:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VLDA_3D_UPS_S32_D8,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        case Intrinsic::aie2_acc64_v8_I256_ups:
          return LoadStoreOpcodes{/*ISelOpcode=*/AIE2::VLDA_3D_UPS_S64_D32,
                                  NoImmediate,
                                  /*OffsetOpcode=*/{}};
        }
      }
    }
  }
  return {};
}
