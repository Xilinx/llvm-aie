//===-AIE2PSGenDecoderMethods.h -----------------------------*- tablegen
//-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//* Automatically generated file, do not edit! *

template <typename InsnType>
static DecodeStatus
DecodeLDA_2D_dml_lda2_scalar_EE(MCInst &MI, InsnType &Insn, uint64_t Address,
                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeLDA_2D_dml_lda2_scalar_EG(MCInst &MI, InsnType &Insn, uint64_t Address,
                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeLDA_2D_dml_lda2_scalar_GG(MCInst &MI, InsnType &Insn, uint64_t Address,
                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeLDA_2D_dml_lda_scalar_F(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeLDA_2D_dml_lda_scalar_L(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_2D_dms_lda_g(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_2D_dms_lda_scalar(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_2D_dmv_lda_eg2(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_2D_dmv_lda_f(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_2D_dmv_lda_feg(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_2D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeLDA_3D_dml_lda2_scalar_EE(MCInst &MI, InsnType &Insn, uint64_t Address,
                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeLDA_3D_dml_lda2_scalar_EG(MCInst &MI, InsnType &Insn, uint64_t Address,
                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeLDA_3D_dml_lda2_scalar_GG(MCInst &MI, InsnType &Insn, uint64_t Address,
                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeLDA_3D_dml_lda_scalar_F(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeLDA_3D_dml_lda_scalar_L(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_3D_dms_lda_g(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_3D_dms_lda_scalar(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_3D_dmv_lda_eg2(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_3D_dmv_lda_f(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_3D_dmv_lda_feg(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_3D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_TM_2D(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_TM_3D(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodePADD_2D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodePADD_3D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodePADDB_2D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                   const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodePADDB_3D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                   const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeST_2D_dml_sts_scalar(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeST_2D_dms_sts_g(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeST_2D_dms_sts_scalar(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeST_2D_dmv_sts_f(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeST_2D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeST_3D_dml_sts_scalar(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeST_3D_dms_sts_g(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeST_3D_dms_sts_scalar(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeST_3D_dmv_sts_f(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeST_3D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeST_TM_2D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                   const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeST_TM_3D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                   const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_vmul_bf_core_X_QY(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_vmul_bf_core_X_X(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_vmul_bf_core_Y_R(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_vmul_bf_core_Y_Y(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_half_vmul_bf_core_X_R(MCInst &MI, InsnType &Insn,
                                                 uint64_t Address,
                                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_half_vmul_bf_core_X_Y(MCInst &MI, InsnType &Insn,
                                                 uint64_t Address,
                                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVADDMAC_f_vaddmac_bf_half_vmul_bf_core_Y_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVADDMAC_f_vaddmac_bf_half_vmul_bf_half_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_R(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_Y(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_R(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_Y(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_R(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_Y(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_Y_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_half_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_R(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_Y(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_Y_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_half_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bfp13(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bfp13_scd_add_scd_incr(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bfp13_scd_add_scd_noincr(MCInst &MI, InsnType &Insn,
                                                 uint64_t Address,
                                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bfp16(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bfp16_scd_add_scd_incr(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bfp16_scd_add_scd_noincr(MCInst &MI, InsnType &Insn,
                                                 uint64_t Address,
                                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_vmul_bf_core_X_QY(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_vmul_bf_core_X_X(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_vmul_bf_core_Y_R(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_vmul_bf_core_Y_Y(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_half_vmul_bf_core_X_R(MCInst &MI, InsnType &Insn,
                                                 uint64_t Address,
                                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_half_vmul_bf_core_X_Y(MCInst &MI, InsnType &Insn,
                                                 uint64_t Address,
                                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVADDMSC_f_vaddmac_bf_half_vmul_bf_core_Y_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVADDMSC_f_vaddmac_bf_half_vmul_bf_half_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_R(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_Y(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_R(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_Y(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_R(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_Y(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_Y_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_half_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_R(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_Y(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_Y_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_half_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bfp13(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bfp13_scd_add_scd_incr(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bfp13_scd_add_scd_noincr(MCInst &MI, InsnType &Insn,
                                                 uint64_t Address,
                                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bfp16(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bfp16_scd_add_scd_incr(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bfp16_scd_add_scd_noincr(MCInst &MI, InsnType &Insn,
                                                 uint64_t Address,
                                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_2D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_2D_CONV_fp32_bf16_dmw_lda_ups_bf16(MCInst &MI, InsnType &Insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_2D_CONV_fp32(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsS(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_2D_UPS(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsS(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_2D_dmw_lda_feg2(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_2D_dmx_lda_bm(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_2D_dmx_lda_x(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_3D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_3D_CONV_fp32_bf16_dmw_lda_ups_bf16(MCInst &MI, InsnType &Insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_3D_CONV_fp32(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsS(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_3D_UPS(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsS(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_3D_dmw_lda_feg2(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_3D_dmx_lda_bm(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_3D_dmx_lda_x(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_FILL(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_POP_2D_1(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_POP_2D(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_2D_dmx_lda_fifo_x(MCInst &MI, InsnType &Insn, uint64_t Address,
                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_POP_3D_1(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_POP_3D(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_3D_dmx_lda_fifo_x(MCInst &MI, InsnType &Insn, uint64_t Address,
                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_dmx_lda_fifo_ex_bfp16_fifo_1d_pop(MCInst &MI, InsnType &Insn,
                                                 uint64_t Address,
                                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_dmx_lda_fifo_ex_bfp16_normal_pop(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_POP_dmx_lda_fifo_fex_bfp13_fifo_1d_pop(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_dmx_lda_fifo_fex_bfp13_normal_pop(MCInst &MI, InsnType &Insn,
                                                 uint64_t Address,
                                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_dmx_lda_fifo_x_fifo_1d_pop(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_dmx_lda_fifo_x_normal_pop(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_mx4_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_mx4_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_POP_s_fifo_1d_pop(MCInst &MI, InsnType &Insn,
                                                 uint64_t Address,
                                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_POP_s_normal_pop(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_2D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_2D_UNPACK_dmw_ldb_unpack_unpackS(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_2D_UNPACK_dmx_ldb_unpack_unpackS(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_2D_dmx_ldb_x(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_3D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_3D_UNPACK_dmw_ldb_unpack_unpackS(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_3D_UNPACK_dmx_ldb_unpack_unpackS(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_3D_dmx_ldb_x(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_FILLX(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_FILL(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_POPX(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_POP_2D(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_POP_2D_1(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_2D_dmx_ldb_fifo_x(MCInst &MI, InsnType &Insn, uint64_t Address,
                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_POP_3D(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_POP_3D_1(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_3D_dmx_ldb_fifo_x(MCInst &MI, InsnType &Insn, uint64_t Address,
                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_dmx_ldb_fifo_ex_bfp16_fifo_1d_pop(MCInst &MI, InsnType &Insn,
                                                 uint64_t Address,
                                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_dmx_ldb_fifo_ex_bfp16_normal_pop(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_POP_dmx_ldb_fifo_fex_bfp13_fifo_1d_pop(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_dmx_ldb_fifo_fex_bfp13_normal_pop(MCInst &MI, InsnType &Insn,
                                                 uint64_t Address,
                                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_dmx_ldb_fifo_x_fifo_1d_pop(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_dmx_ldb_fifo_x_normal_pop(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_mx4_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_mx4_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_POP_s_fifo_1d_pop(MCInst &MI, InsnType &Insn,
                                                 uint64_t Address,
                                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_POP_s_normal_pop(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMAC_f_vmac_bf_vmul_bf_core_X_QY(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMAC_f_vmac_bf_vmul_bf_core_X_X(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMAC_f_vmac_bf_vmul_bf_core_Y_R(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMAC_f_vmac_bf_vmul_bf_core_Y_Y(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMAC_f_vmac_bf_half_vmul_bf_core_X_R(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMAC_f_vmac_bf_half_vmul_bf_core_X_Y(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMAC_f_vmac_bf_half_vmul_bf_core_Y_QY(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMAC_f_vmac_bf_half_vmul_bf_half_core_X_X(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVMAC_f_vmac_bfp13(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVMAC_f_vmac_bfp16(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMSC_f_vmac_bf_vmul_bf_core_X_QY(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMSC_f_vmac_bf_vmul_bf_core_X_X(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMSC_f_vmac_bf_vmul_bf_core_Y_R(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMSC_f_vmac_bf_vmul_bf_core_Y_Y(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMSC_f_vmac_bf_half_vmul_bf_core_X_R(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMSC_f_vmac_bf_half_vmul_bf_core_X_Y(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMSC_f_vmac_bf_half_vmul_bf_core_Y_QY(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMSC_f_vmac_bf_half_vmul_bf_half_core_X_X(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVMSC_f_vmac_bfp13(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVMSC_f_vmac_bfp16(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMUL_f_vmul_bf_vmul_bf_core_X_QY(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMUL_f_vmul_bf_vmul_bf_core_X_X(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMUL_f_vmul_bf_vmul_bf_core_Y_R(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMUL_f_vmul_bf_vmul_bf_core_Y_Y(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMUL_f_vmul_bf_half_vmul_bf_core_X_R(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMUL_f_vmul_bf_half_vmul_bf_core_X_Y(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMUL_f_vmul_bf_half_vmul_bf_core_Y_QY(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVMUL_f_vmul_bf_half_vmul_bf_half_core_X_X(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVMUL_f_vmul_bfp13(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVMUL_f_vmul_bfp16(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVNEGMUL_f_vmul_bf_vmul_bf_core_X_QY(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVNEGMUL_f_vmul_bf_vmul_bf_core_X_X(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVNEGMUL_f_vmul_bf_vmul_bf_core_Y_R(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVNEGMUL_f_vmul_bf_vmul_bf_core_Y_Y(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVNEGMUL_f_vmul_bf_half_vmul_bf_core_X_R(MCInst &MI, InsnType &Insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVNEGMUL_f_vmul_bf_half_vmul_bf_core_X_Y(MCInst &MI, InsnType &Insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVNEGMUL_f_vmul_bf_half_vmul_bf_core_Y_QY(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVNEGMUL_f_vmul_bf_half_vmul_bf_half_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVNEGMUL_f_vmul_bfp13(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVNEGMUL_f_vmul_bfp16(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_2D_128_dmv_sts_eg(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_2D_128_dmv_sts_feg(MCInst &MI, InsnType &Insn,
                                                 uint64_t Address,
                                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_2D_128_dmv_sts_w(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVST_2D_CONV_bf16_fp32_dmw_sts_srs_bf16(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_2D_CONV(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_2D_CONV_1(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVST_2D_PACK_dmw_sts_pack_packS(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVST_2D_PACK_dmx_sts_pack_packS(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_2D_SRS(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVST_2D_SRS_2x_dmw_sts_srs_bm_srsS(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVST_2D_SRS_4x_dmx_sts_srs_dm_srsS(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_2D_dmw_sts_feg2(MCInst &MI, InsnType &Insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_2D_dmw_sts_w(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_2D_dmx_sts_bm(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_2D_dmx_sts_x(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_3D_128_dmv_sts_eg(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_3D_128_dmv_sts_feg(MCInst &MI, InsnType &Insn,
                                                 uint64_t Address,
                                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_3D_128_dmv_sts_w(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVST_3D_CONV_bf16_fp32_dmw_sts_srs_bf16(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_3D_CONV(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_3D_CONV_1(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVST_3D_PACK_dmw_sts_pack_packS(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVST_3D_PACK_dmx_sts_pack_packS(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_3D_SRS(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVST_3D_SRS_2x_dmw_sts_srs_bm_srsS(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVST_3D_SRS_4x_dmx_sts_srs_dm_srsS(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_3D_dmw_sts_feg2(MCInst &MI, InsnType &Insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_3D_dmw_sts_w(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_3D_dmx_sts_bm(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_3D_dmx_sts_x(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_FLUSH_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_FLUSH_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);