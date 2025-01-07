//===-AIE2PGenDecoderMethods.h -----------------------------*- tablegen
//-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//* Automatically generated file, do not edit! *

template <typename InsnType>
static DecodeStatus DecodeDms_lda_2D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmv_lda_q_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmhb_lda_2D(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDms_lda_3D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmv_lda_q_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmhb_lda_3D(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeTm_lda_2D(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeTm_lda_3D(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLda_ptr_inc_2D(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLda_ptr_inc_3D(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLdb_or1_2D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLdb_or1_3D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeSt_ptr_inc_2D(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeSt_ptr_inc_3D(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDms_sts_2D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmv_sts_q_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmhb_sts_2D(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDms_sts_3D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmv_sts_q_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmhb_sts_3D(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeTm_sts_2D(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeTm_sts_3D(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmv_lda_w_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_lda_ups_bf_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_lda_ups_bf_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_lda_ups_w2b_2D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_lda_ups_x2c_2D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_lda_ups_w2c_2D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_lda_ups_x2d_2D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_lda_w_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_lda_bm_2D(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_lda_fifohl_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_lda_x_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmv_lda_w_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_lda_ups_bf_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_lda_ups_bf_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_lda_ups_w2b_3D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_lda_ups_x2c_3D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_lda_ups_w2c_3D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_lda_ups_x2d_3D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_lda_w_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_lda_bm_3D(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_lda_fifohl_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_lda_x_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_FILL_512(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_x__fifo_2d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_x__fifo_3d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_512_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_512_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_ex_ebs16__fifo_2d_pop(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_ex_ebs16__fifo_3d_pop(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_544_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_544_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_ex_ebs8__fifo_2d_pop(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_ex_ebs8__fifo_3d_pop(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_576_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_576_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_qx__fifo_2d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                   const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_qx__fifo_3d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                   const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_640_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_640_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_qex_ebs16__fifo_2d_pop(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_qex_ebs16__fifo_3d_pop(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_704_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_704_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmv_ldb_2D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_ldb_unpack_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_ldb_unpack_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_ldb_2D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_ldb_x_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmv_ldb_3D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_ldb_unpack_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_ldb_unpack_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_ldb_3D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_ldb_x_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_FILLX_512(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_FILL_512(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_POPX_512(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_x__fifo_2d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_x__fifo_3d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_512_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_512_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_ex_ebs16__fifo_2d_pop(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_ex_ebs16__fifo_3d_pop(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_544_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_544_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_ex_ebs8__fifo_2d_pop(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_ex_ebs8__fifo_3d_pop(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_576_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_576_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_qx__fifo_2d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                   const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_qx__fifo_3d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                   const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_640_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_640_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_qex_ebs16__fifo_2d_pop(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_qex_ebs16__fifo_3d_pop(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_704_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_704_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmv_sts_w_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_sts_srs_bf_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_sts_srs_bf_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_sts_pack_2D(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_sts_pack_2D(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDm_sts_srs_cm_2D(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_sts_srs_bm_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_sts_srs_dm_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_sts_w_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_sts_bm_2D(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_sts_fifohl_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_sts_x_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmv_sts_w_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_sts_srs_bf_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_sts_srs_bf_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_sts_pack_3D(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_sts_pack_3D(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDm_sts_srs_cm_3D(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_sts_srs_bm_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_sts_srs_dm_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmw_sts_w_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_sts_bm_3D(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_sts_fifohl_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeDmx_sts_x_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_sts_fifo_bare_x__fifo_2d_flush(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_sts_fifo_bare_x__fifo_3d_flush(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_sts_fifo_conv_x__fifo_2d_flush(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeDmx_sts_fifo_conv_x__fifo_3d_flush(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder);