//===- aiev2_pckt_header.h --------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

INTRINSIC(void)
put_ms_packet_header(int tlast, unsigned dstID, unsigned pcktType) {
  __builtin_aiev2_put_ms_packet_header(tlast, dstID, pcktType);
}
INTRINSIC(void) put_ms_packet_header(unsigned dstID, unsigned pcktType) {
  __builtin_aiev2_put_ms_packet_header(0, dstID, pcktType);
}
INTRINSIC(void)
put_ms_nb_packet_header(int tlast, unsigned dstID, unsigned pcktType,
                        bool &success) {
  int suc;
  __builtin_aiev2_put_ms_nb_packet_header(tlast, dstID, pcktType, suc);
  success = suc;
}
INTRINSIC(void)
put_ms_nb_packet_header(unsigned dstID, unsigned pcktType, bool &success) {
  int suc;
  __builtin_aiev2_put_ms_nb_packet_header(0, dstID, pcktType, suc);
  success = suc;
}
INTRINSIC(void)
put_ms_ctrl_packet_header(int tlast, unsigned Addr, unsigned n_words,
                          unsigned op_type, unsigned rspID) {
  __builtin_aiev2_put_ms_ctrl_packet_header(tlast, Addr, n_words - 1, op_type,
                                            rspID);
}
INTRINSIC(void)
put_ms_ctrl_packet_header(unsigned Addr, unsigned n_words, unsigned op_type,
                          unsigned rspID) {
  __builtin_aiev2_put_ms_ctrl_packet_header(0, Addr, n_words - 1, op_type,
                                            rspID);
}
INTRINSIC(void)
put_ms_nb_ctrl_packet_header(int tlast, unsigned Addr, unsigned n_words,
                             unsigned op_type, unsigned rspID, bool &success) {
  int suc;
  __builtin_aiev2_put_ms_nb_ctrl_packet_header(tlast, Addr, n_words - 1,
                                               op_type, rspID, suc);
  success = suc;
}
INTRINSIC(void)
put_ms_nb_ctrl_packet_header(unsigned Addr, unsigned n_words, unsigned op_type,
                             unsigned rspID, bool &success) {
  int suc;
  __builtin_aiev2_put_ms_nb_ctrl_packet_header(0, Addr, n_words - 1, op_type,
                                               rspID, suc);
  success = suc;
}
