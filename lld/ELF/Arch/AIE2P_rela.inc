//===- AIE2P_rela.inc ------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//

// 0 : (symbol_addr_AR  + addend )  :  addr [19..0]@63 in iword[16] nostrict //
// with default addend 0
case 0:
checkUInt(Loc, Val, 20, rel);
patch16bytes(Loc, Val, 19, 0, 63);
return;

// 1 : (symbol_addr_AR  + addend )  :  addr [19..0]@5 in iword[6] nostrict //
// with default addend 0
case 1:
checkUInt(Loc, Val, 20, rel);
patch6bytes(Loc, Val, 19, 0, 5);
return;

// 2 : (symbol_addr_AR  + addend )  :  addr [19..0]@31 in iword[10] nostrict //
// with default addend 0
case 2:
checkUInt(Loc, Val, 20, rel);
patch10bytes(Loc, Val, 19, 0, 31);
return;

// 3 : (symbol_addr_AR  + addend )  :  addr [19..0]@10 in iword[10] nostrict //
// with default addend 0
case 3:
checkUInt(Loc, Val, 20, rel);
patch10bytes(Loc, Val, 19, 0, 10);
return;

// 4 : (symbol_addr_AR  + addend )  :  addr [19..0]@47 in iword[12] nostrict //
// with default addend 0
case 4:
checkUInt(Loc, Val, 20, rel);
patch12bytes(Loc, Val, 19, 0, 47);
return;

// 5 : (symbol_addr_AR  + addend )  :  addr [19..0]@26 in iword[12] nostrict //
// with default addend 0
case 5:
checkUInt(Loc, Val, 20, rel);
patch12bytes(Loc, Val, 19, 0, 26);
return;

// 6 : (symbol_addr_AR  + addend )  :  addr [19..0]@22 in iword[12] nostrict //
// with default addend 0
case 6:
checkUInt(Loc, Val, 20, rel);
patch12bytes(Loc, Val, 19, 0, 22);
return;

// 7 : (symbol_addr_AR  + addend )  :  addr [19..0]@42 in iword[14] nostrict //
// with default addend 0
case 7:
checkUInt(Loc, Val, 20, rel);
patch14bytes(Loc, Val, 19, 0, 42);
return;

// 8 : (symbol_addr_AR  + addend )  :  w11 [10..0]@0 in iword[16]      // with
// default addend 0
case 8:
checkUInt(Loc, Val, 20, rel);
patch16bytes(Loc, Val, 10, 0, 0);
return;

// 9 : (symbol_addr_AR  + addend )  :  w11 [10..0]@5 in iword[4]      // with
// default addend 0
case 9:
checkUInt(Loc, Val, 20, rel);
patch4bytes(Loc, Val, 10, 0, 5);
return;

// 10 : (symbol_addr_AR  + addend )  :  w11 [10..0]@0 in iword[6]      // with
// default addend 0
case 10:
checkUInt(Loc, Val, 20, rel);
patch6bytes(Loc, Val, 10, 0, 0);
return;

// 11 : (symbol_addr_AR  + addend )  :  w11 [10..0]@0 in iword[8]      // with
// default addend 0
case 11:
checkUInt(Loc, Val, 20, rel);
patch8bytes(Loc, Val, 10, 0, 0);
return;

// 12 : (symbol_addr_AR  + addend )  :  w11 [10..0]@0 in iword[10]      // with
// default addend 0
case 12:
checkUInt(Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 0, 0);
return;

// 13 : (symbol_addr_AR  + addend )  :  w11 [10..0]@0 in iword[12]      // with
// default addend 0
case 13:
checkUInt(Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 0, 0);
return;

// 14 : (symbol_addr_AR  + addend )  :  w11 [10..0]@0 in iword[14]      // with
// default addend 0
case 14:
checkUInt(Loc, Val, 20, rel);
patch14bytes(Loc, Val, 10, 0, 0);
return;

// 15 : (symbol_addr_AR  + addend )  :  w11 [10..6]@58 [5..0]@69 in iword[16] //
// with default addend 0
case 15:
checkUInt(Loc, Val, 20, rel);
patch16bytes(Loc, Val, 10, 6, 58);
patch16bytes(Loc, Val, 5, 0, 69);
return;

// 16 : (symbol_addr_AR  + addend )  :  w11 [10..6]@5 [5..0]@16 in iword[4] //
// with default addend 0
case 16:
checkUInt(Loc, Val, 20, rel);
patch4bytes(Loc, Val, 10, 6, 5);
patch4bytes(Loc, Val, 5, 0, 16);
return;

// 17 : (symbol_addr_AR  + addend )  :  w11 [10..6]@20 [5..0]@31 in iword[6] //
// with default addend 0
case 17:
checkUInt(Loc, Val, 20, rel);
patch6bytes(Loc, Val, 10, 6, 20);
patch6bytes(Loc, Val, 5, 0, 31);
return;

// 18 : (symbol_addr_AR  + addend )  :  w11 [10..6]@0 [5..0]@11 in iword[6] //
// with default addend 0
case 18:
checkUInt(Loc, Val, 20, rel);
patch6bytes(Loc, Val, 10, 6, 0);
patch6bytes(Loc, Val, 5, 0, 11);
return;

// 19 : (symbol_addr_AR  + addend )  :  w11 [10..6]@10 [5..0]@21 in iword[8] //
// with default addend 0
case 19:
checkUInt(Loc, Val, 20, rel);
patch8bytes(Loc, Val, 10, 6, 10);
patch8bytes(Loc, Val, 5, 0, 21);
return;

// 20 : (symbol_addr_AR  + addend )  :  w11 [10..6]@37 [5..0]@48 in iword[8] //
// with default addend 0
case 20:
checkUInt(Loc, Val, 20, rel);
patch8bytes(Loc, Val, 10, 6, 37);
patch8bytes(Loc, Val, 5, 0, 48);
return;

// 21 : (symbol_addr_AR  + addend )  :  w11 [10..6]@53 [5..0]@64 in iword[10] //
// with default addend 0
case 21:
checkUInt(Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 6, 53);
patch10bytes(Loc, Val, 5, 0, 64);
return;

// 22 : (symbol_addr_AR  + addend )  :  w11 [10..6]@26 [5..0]@37 in iword[10] //
// with default addend 0
case 22:
checkUInt(Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 6, 26);
patch10bytes(Loc, Val, 5, 0, 37);
return;

// 23 : (symbol_addr_AR  + addend )  :  w11 [10..6]@5 [5..0]@16 in iword[10] //
// with default addend 0
case 23:
checkUInt(Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 6, 5);
patch10bytes(Loc, Val, 5, 0, 16);
return;

// 24 : (symbol_addr_AR  + addend )  :  w11 [10..6]@42 [5..0]@53 in iword[12] //
// with default addend 0
case 24:
checkUInt(Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 6, 42);
patch12bytes(Loc, Val, 5, 0, 53);
return;

// 25 : (symbol_addr_AR  + addend )  :  w11 [10..6]@21 [5..0]@32 in iword[12] //
// with default addend 0
case 25:
checkUInt(Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 6, 21);
patch12bytes(Loc, Val, 5, 0, 32);
return;

// 26 : (symbol_addr_AR  + addend )  :  w11 [10..6]@17 [5..0]@28 in iword[12] //
// with default addend 0
case 26:
checkUInt(Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 6, 17);
patch12bytes(Loc, Val, 5, 0, 28);
return;

// 27 : (symbol_addr_AR  + addend )  :  w11 [10..6]@37 [5..0]@48 in iword[14] //
// with default addend 0
case 27:
checkUInt(Loc, Val, 20, rel);
patch14bytes(Loc, Val, 10, 6, 37);
patch14bytes(Loc, Val, 5, 0, 48);
return;

// 28 : (symbol_addr_AR  + addend )  :  w11 [10..0]@85 in iword[16]      // with
// default addend 0
case 28:
checkUInt(Loc, Val, 20, rel);
patch16bytes(Loc, Val, 10, 0, 85);
return;

// 29 : (symbol_addr_AR  + addend )  :  w11 [10..0]@12 in iword[4]      // with
// default addend 0
case 29:
checkUInt(Loc, Val, 20, rel);
patch4bytes(Loc, Val, 10, 0, 12);
return;

// 30 : (symbol_addr_AR  + addend )  :  w11 [10..0]@27 in iword[6]      // with
// default addend 0
case 30:
checkUInt(Loc, Val, 20, rel);
patch6bytes(Loc, Val, 10, 0, 27);
return;

// 31 : (symbol_addr_AR  + addend )  :  w11 [10..0]@37 in iword[8]      // with
// default addend 0
case 31:
checkUInt(Loc, Val, 20, rel);
patch8bytes(Loc, Val, 10, 0, 37);
return;

// 32 : (symbol_addr_AR  + addend )  :  w11 [10..0]@12 in iword[8]      // with
// default addend 0
case 32:
checkUInt(Loc, Val, 20, rel);
patch8bytes(Loc, Val, 10, 0, 12);
return;

// 33 : (symbol_addr_AR  + addend )  :  w11 [10..0]@53 in iword[10]      // with
// default addend 0
case 33:
checkUInt(Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 0, 53);
return;

// 34 : (symbol_addr_AR  + addend )  :  w11 [10..0]@28 in iword[10]      // with
// default addend 0
case 34:
checkUInt(Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 0, 28);
return;

// 35 : (symbol_addr_AR  + addend )  :  w11 [10..0]@32 in iword[10]      // with
// default addend 0
case 35:
checkUInt(Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 0, 32);
return;

// 36 : (symbol_addr_AR  + addend )  :  w11 [10..0]@69 in iword[12]      // with
// default addend 0
case 36:
checkUInt(Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 0, 69);
return;

// 37 : (symbol_addr_AR  + addend )  :  w11 [10..0]@44 in iword[12]      // with
// default addend 0
case 37:
checkUInt(Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 0, 44);
return;

// 38 : (symbol_addr_AR  + addend )  :  w11 [10..0]@48 in iword[12]      // with
// default addend 0
case 38:
checkUInt(Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 0, 48);
return;

// 39 : (symbol_addr_AR  + addend )  :  w11 [10..0]@65 in iword[14]      // with
// default addend 0
case 39:
checkUInt(Loc, Val, 20, rel);
patch14bytes(Loc, Val, 10, 0, 65);
return;

// 40 : (symbol_addr_AR  + addend )  :  w11 [10..0]@64 in iword[14]      // with
// default addend 0
case 40:
checkUInt(Loc, Val, 20, rel);
patch14bytes(Loc, Val, 10, 0, 64);
return;

// 41 : (symbol_addr_AR  + addend )  :  w32 [31..12]@58 [11..0]@85 in iword[16]
// // with default addend 0
case 41:
checkUInt(Loc, Val, 20, rel);
patch16bytes(Loc, Val, 31, 12, 58);
patch16bytes(Loc, Val, 11, 0, 85);
return;

// 42 : (symbol_addr_AR  + addend )  :  w32 [31..12]@0 [11..0]@27 in iword[6] //
// with default addend 0
case 42:
checkUInt(Loc, Val, 20, rel);
patch6bytes(Loc, Val, 31, 12, 0);
patch6bytes(Loc, Val, 11, 0, 27);
return;

// 43 : (symbol_addr_AR  + addend )  :  w32 [31..12]@26 [11..0]@53 in iword[10]
// // with default addend 0
case 43:
checkUInt(Loc, Val, 20, rel);
patch10bytes(Loc, Val, 31, 12, 26);
patch10bytes(Loc, Val, 11, 0, 53);
return;

// 44 : (symbol_addr_AR  + addend )  :  w32 [31..12]@5 [11..0]@32 in iword[10]
// // with default addend 0
case 44:
checkUInt(Loc, Val, 20, rel);
patch10bytes(Loc, Val, 31, 12, 5);
patch10bytes(Loc, Val, 11, 0, 32);
return;

// 45 : (symbol_addr_AR  + addend )  :  w32 [31..12]@42 [11..0]@69 in iword[12]
// // with default addend 0
case 45:
checkUInt(Loc, Val, 20, rel);
patch12bytes(Loc, Val, 31, 12, 42);
patch12bytes(Loc, Val, 11, 0, 69);
return;

// 46 : (symbol_addr_AR  + addend )  :  w32 [31..12]@21 [11..0]@48 in iword[12]
// // with default addend 0
case 46:
checkUInt(Loc, Val, 20, rel);
patch12bytes(Loc, Val, 31, 12, 21);
patch12bytes(Loc, Val, 11, 0, 48);
return;

// 47 : (symbol_addr_AR  + addend )  :  w32 [31..12]@17 [11..0]@44 in iword[12]
// // with default addend 0
case 47:
checkUInt(Loc, Val, 20, rel);
patch12bytes(Loc, Val, 31, 12, 17);
patch12bytes(Loc, Val, 11, 0, 44);
return;

// 48 : (symbol_addr_AR  + addend )  :  w32 [31..12]@37 [11..0]@64 in iword[14]
// // with default addend 0
case 48:
checkUInt(Loc, Val, 20, rel);
patch14bytes(Loc, Val, 31, 12, 37);
patch14bytes(Loc, Val, 11, 0, 64);
return;

// 49 : (symbol_addr_AR  + addend )  :  w8 [7..0]@90 in iword[16]      // with
// default addend 0
case 49:
checkUInt(Loc, Val, 20, rel);
patch16bytes(Loc, Val, 7, 0, 90);
return;

// 50 : (symbol_addr_AR  + addend )  :  w8 [7..0]@17 in iword[4]      // with
// default addend 0
case 50:
checkUInt(Loc, Val, 20, rel);
patch4bytes(Loc, Val, 7, 0, 17);
return;

// 51 : (symbol_addr_AR  + addend )  :  w8 [7..0]@32 in iword[6]      // with
// default addend 0
case 51:
checkUInt(Loc, Val, 20, rel);
patch6bytes(Loc, Val, 7, 0, 32);
return;

// 52 : (symbol_addr_AR  + addend )  :  w8 [7..0]@42 in iword[8]      // with
// default addend 0
case 52:
checkUInt(Loc, Val, 20, rel);
patch8bytes(Loc, Val, 7, 0, 42);
return;

// 53 : (symbol_addr_AR  + addend )  :  w8 [7..0]@17 in iword[8]      // with
// default addend 0
case 53:
checkUInt(Loc, Val, 20, rel);
patch8bytes(Loc, Val, 7, 0, 17);
return;

// 54 : (symbol_addr_AR  + addend )  :  w8 [7..0]@58 in iword[10]      // with
// default addend 0
case 54:
checkUInt(Loc, Val, 20, rel);
patch10bytes(Loc, Val, 7, 0, 58);
return;

// 55 : (symbol_addr_AR  + addend )  :  w8 [7..0]@33 in iword[10]      // with
// default addend 0
case 55:
checkUInt(Loc, Val, 20, rel);
patch10bytes(Loc, Val, 7, 0, 33);
return;

// 56 : (symbol_addr_AR  + addend )  :  w8 [7..0]@37 in iword[10]      // with
// default addend 0
case 56:
checkUInt(Loc, Val, 20, rel);
patch10bytes(Loc, Val, 7, 0, 37);
return;

// 57 : (symbol_addr_AR  + addend )  :  w8 [7..0]@74 in iword[12]      // with
// default addend 0
case 57:
checkUInt(Loc, Val, 20, rel);
patch12bytes(Loc, Val, 7, 0, 74);
return;

// 58 : (symbol_addr_AR  + addend )  :  w8 [7..0]@49 in iword[12]      // with
// default addend 0
case 58:
checkUInt(Loc, Val, 20, rel);
patch12bytes(Loc, Val, 7, 0, 49);
return;

// 59 : (symbol_addr_AR  + addend )  :  w8 [7..0]@53 in iword[12]      // with
// default addend 0
case 59:
checkUInt(Loc, Val, 20, rel);
patch12bytes(Loc, Val, 7, 0, 53);
return;

// 60 : (symbol_addr_AR  + addend )  :  w8 [7..0]@70 in iword[14]      // with
// default addend 0
case 60:
checkUInt(Loc, Val, 20, rel);
patch14bytes(Loc, Val, 7, 0, 70);
return;

// 61 : (symbol_addr_AR  + addend )  :  w8 [7..0]@69 in iword[14]      // with
// default addend 0
case 61:
checkUInt(Loc, Val, 20, rel);
patch14bytes(Loc, Val, 7, 0, 69);
return;
