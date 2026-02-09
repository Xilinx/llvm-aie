//===- AIE2PS_rela.inc -----------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//

// 0 : (symbol_addr_AR  + addend )  :  addr [19..0]@64 in iword[16] nostrict
// with default addend 0
case 0:
checkUInt(ctx, Loc, Val, 20, rel);
patch16bytes(Loc, Val, 19, 0, 64);
return;

// 1 : (symbol_addr_AR  + addend )  :  addr [19..0]@5 in iword[6] nostrict
// with default addend 0
case 1:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 19, 0, 5);
return;

// 2 : (symbol_addr_AR  + addend )  :  addr [19..0]@31 in iword[10] nostrict
// with default addend 0
case 2:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 19, 0, 31);
return;

// 3 : (symbol_addr_AR  + addend )  :  addr [19..0]@6 in iword[10] nostrict
// with default addend 0
case 3:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 19, 0, 6);
return;

// 4 : (symbol_addr_AR  + addend )  :  addr [19..0]@48 in iword[12] nostrict
// with default addend 0
case 4:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 19, 0, 48);
return;

// 5 : (symbol_addr_AR  + addend )  :  addr [19..0]@26 in iword[12] nostrict
// with default addend 0
case 5:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 19, 0, 26);
return;

// 6 : (symbol_addr_AR  + addend )  :  addr [19..0]@22 in iword[12] nostrict
// with default addend 0
case 6:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 19, 0, 22);
return;

// 7 : (symbol_addr_AR  + addend )  :  addr [19..0]@43 in iword[14] nostrict
// with default addend 0
case 7:
checkUInt(ctx, Loc, Val, 20, rel);
patch14bytes(Loc, Val, 19, 0, 43);
return;

// 8 : (symbol_addr_AR  + addend )  :  w11 [10..2]@0 [1..0]@16 in iword[16]
// with default addend 0
case 8:
checkUInt(ctx, Loc, Val, 20, rel);
patch16bytes(Loc, Val, 10, 2, 0);
patch16bytes(Loc, Val, 1, 0, 16);
return;

// 9 : (symbol_addr_AR  + addend )  :  w11 [10..2]@4 [1..0]@20 in iword[4]
// with default addend 0
case 9:
checkUInt(ctx, Loc, Val, 20, rel);
patch4bytes(Loc, Val, 10, 2, 4);
patch4bytes(Loc, Val, 1, 0, 20);
return;

// 10 : (symbol_addr_AR  + addend )  :  w11 [10..2]@0 [1..0]@16 in iword[6]
// with default addend 0
case 10:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 10, 2, 0);
patch6bytes(Loc, Val, 1, 0, 16);
return;

// 11 : (symbol_addr_AR  + addend )  :  w11 [10..2]@0 [1..0]@16 in iword[8]
// with default addend 0
case 11:
checkUInt(ctx, Loc, Val, 20, rel);
patch8bytes(Loc, Val, 10, 2, 0);
patch8bytes(Loc, Val, 1, 0, 16);
return;

// 12 : (symbol_addr_AR  + addend )  :  w11 [10..2]@0 [1..0]@16 in iword[10]
// with default addend 0
case 12:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 2, 0);
patch10bytes(Loc, Val, 1, 0, 16);
return;

// 13 : (symbol_addr_AR  + addend )  :  w11 [10..2]@0 [1..0]@16 in iword[12]
// with default addend 0
case 13:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 2, 0);
patch12bytes(Loc, Val, 1, 0, 16);
return;

// 14 : (symbol_addr_AR  + addend )  :  w11 [10..2]@0 [1..0]@16 in iword[14]
// with default addend 0
case 14:
checkUInt(ctx, Loc, Val, 20, rel);
patch14bytes(Loc, Val, 10, 2, 0);
patch14bytes(Loc, Val, 1, 0, 16);
return;

// 15 : (symbol_addr_AR  + addend )  :  w11 [10..6]@59 [5..0]@70 in iword[16]
// with default addend 0
case 15:
checkUInt(ctx, Loc, Val, 20, rel);
patch16bytes(Loc, Val, 10, 6, 59);
patch16bytes(Loc, Val, 5, 0, 70);
return;

// 16 : (symbol_addr_AR  + addend )  :  w11 [10..6]@5 [5..0]@16 in iword[4]
// with default addend 0
case 16:
checkUInt(ctx, Loc, Val, 20, rel);
patch4bytes(Loc, Val, 10, 6, 5);
patch4bytes(Loc, Val, 5, 0, 16);
return;

// 17 : (symbol_addr_AR  + addend )  :  w11 [10..6]@21 [5..0]@32 in iword[6]
// with default addend 0
case 17:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 10, 6, 21);
patch6bytes(Loc, Val, 5, 0, 32);
return;

// 18 : (symbol_addr_AR  + addend )  :  w11 [10..6]@0 [5..0]@11 in iword[6]
// with default addend 0
case 18:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 10, 6, 0);
patch6bytes(Loc, Val, 5, 0, 11);
return;

// 19 : (symbol_addr_AR  + addend )  :  w11 [10..6]@10 [5..0]@21 in iword[8]
// with default addend 0
case 19:
checkUInt(ctx, Loc, Val, 20, rel);
patch8bytes(Loc, Val, 10, 6, 10);
patch8bytes(Loc, Val, 5, 0, 21);
return;

// 20 : (symbol_addr_AR  + addend )  :  w11 [10..6]@38 [5..0]@49 in iword[8]
// with default addend 0
case 20:
checkUInt(ctx, Loc, Val, 20, rel);
patch8bytes(Loc, Val, 10, 6, 38);
patch8bytes(Loc, Val, 5, 0, 49);
return;

// 21 : (symbol_addr_AR  + addend )  :  w11 [10..6]@53 [5..0]@64 in iword[10]
// with default addend 0
case 21:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 6, 53);
patch10bytes(Loc, Val, 5, 0, 64);
return;

// 22 : (symbol_addr_AR  + addend )  :  w11 [10..6]@26 [5..0]@37 in iword[10]
// with default addend 0
case 22:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 6, 26);
patch10bytes(Loc, Val, 5, 0, 37);
return;

// 23 : (symbol_addr_AR  + addend )  :  w11 [10..6]@1 [5..0]@12 in iword[10]
// with default addend 0
case 23:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 6, 1);
patch10bytes(Loc, Val, 5, 0, 12);
return;

// 24 : (symbol_addr_AR  + addend )  :  w11 [10..6]@43 [5..0]@54 in iword[12]
// with default addend 0
case 24:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 6, 43);
patch12bytes(Loc, Val, 5, 0, 54);
return;

// 25 : (symbol_addr_AR  + addend )  :  w11 [10..6]@21 [5..0]@32 in iword[12]
// with default addend 0
case 25:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 6, 21);
patch12bytes(Loc, Val, 5, 0, 32);
return;

// 26 : (symbol_addr_AR  + addend )  :  w11 [10..6]@17 [5..0]@28 in iword[12]
// with default addend 0
case 26:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 6, 17);
patch12bytes(Loc, Val, 5, 0, 28);
return;

// 27 : (symbol_addr_AR  + addend )  :  w11 [10..6]@38 [5..0]@49 in iword[14]
// with default addend 0
case 27:
checkUInt(ctx, Loc, Val, 20, rel);
patch14bytes(Loc, Val, 10, 6, 38);
patch14bytes(Loc, Val, 5, 0, 49);
return;

// 28 : (symbol_addr_AR  + addend )  :  w11 [10..7]@93 [6..0]@86 in iword[16]
// with default addend 0
case 28:
checkUInt(ctx, Loc, Val, 20, rel);
patch16bytes(Loc, Val, 10, 7, 93);
patch16bytes(Loc, Val, 6, 0, 86);
return;

// 29 : (symbol_addr_AR  + addend )  :  w11 [10..7]@18 [6..0]@11 in iword[4]
// with default addend 0
case 29:
checkUInt(ctx, Loc, Val, 20, rel);
patch4bytes(Loc, Val, 10, 7, 18);
patch4bytes(Loc, Val, 6, 0, 11);
return;

// 30 : (symbol_addr_AR  + addend )  :  w11 [10..7]@35 [6..0]@28 in iword[6]
// with default addend 0
case 30:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 10, 7, 35);
patch6bytes(Loc, Val, 6, 0, 28);
return;

// 31 : (symbol_addr_AR  + addend )  :  w11 [10..7]@34 [6..0]@27 in iword[6]
// with default addend 0
case 31:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 10, 7, 34);
patch6bytes(Loc, Val, 6, 0, 27);
return;

// 32 : (symbol_addr_AR  + addend )  :  w11 [10..7]@45 [6..0]@38 in iword[8]
// with default addend 0
case 32:
checkUInt(ctx, Loc, Val, 20, rel);
patch8bytes(Loc, Val, 10, 7, 45);
patch8bytes(Loc, Val, 6, 0, 38);
return;

// 33 : (symbol_addr_AR  + addend )  :  w11 [10..7]@19 [6..0]@12 in iword[8]
// with default addend 0
case 33:
checkUInt(ctx, Loc, Val, 20, rel);
patch8bytes(Loc, Val, 10, 7, 19);
patch8bytes(Loc, Val, 6, 0, 12);
return;

// 34 : (symbol_addr_AR  + addend )  :  w11 [10..7]@60 [6..0]@53 in iword[10]
// with default addend 0
case 34:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 7, 60);
patch10bytes(Loc, Val, 6, 0, 53);
return;

// 35 : (symbol_addr_AR  + addend )  :  w11 [10..7]@35 [6..0]@28 in iword[10]
// with default addend 0
case 35:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 7, 35);
patch10bytes(Loc, Val, 6, 0, 28);
return;

// 36 : (symbol_addr_AR  + addend )  :  w11 [10..7]@77 [6..0]@70 in iword[12]
// with default addend 0
case 36:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 7, 77);
patch12bytes(Loc, Val, 6, 0, 70);
return;

// 37 : (symbol_addr_AR  + addend )  :  w11 [10..7]@52 [6..0]@45 in iword[12]
// with default addend 0
case 37:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 7, 52);
patch12bytes(Loc, Val, 6, 0, 45);
return;

// 38 : (symbol_addr_AR  + addend )  :  w11 [10..7]@55 [6..0]@48 in iword[12]
// with default addend 0
case 38:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 7, 55);
patch12bytes(Loc, Val, 6, 0, 48);
return;

// 39 : (symbol_addr_AR  + addend )  :  w11 [10..7]@51 [6..0]@44 in iword[12]
// with default addend 0
case 39:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 7, 51);
patch12bytes(Loc, Val, 6, 0, 44);
return;

// 40 : (symbol_addr_AR  + addend )  :  w11 [10..7]@72 [6..0]@65 in iword[14]
// with default addend 0
case 40:
checkUInt(ctx, Loc, Val, 20, rel);
patch14bytes(Loc, Val, 10, 7, 72);
patch14bytes(Loc, Val, 6, 0, 65);
return;

// 41 : (symbol_addr_AR  + addend )  :  w32 [31..12]@59 [11..0]@86 in iword[16]
// with default addend 0
case 41:
checkUInt(ctx, Loc, Val, 20, rel);
patch16bytes(Loc, Val, 31, 12, 59);
patch16bytes(Loc, Val, 11, 0, 86);
return;

// 42 : (symbol_addr_AR  + addend )  :  w32 [31..12]@0 [11..0]@27 in iword[6]
// with default addend 0
case 42:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 31, 12, 0);
patch6bytes(Loc, Val, 11, 0, 27);
return;

// 43 : (symbol_addr_AR  + addend )  :  w32 [31..12]@26 [11..0]@53 in iword[10]
// with default addend 0
case 43:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 31, 12, 26);
patch10bytes(Loc, Val, 11, 0, 53);
return;

// 44 : (symbol_addr_AR  + addend )  :  w32 [31..12]@1 [11..0]@28 in iword[10]
// with default addend 0
case 44:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 31, 12, 1);
patch10bytes(Loc, Val, 11, 0, 28);
return;

// 45 : (symbol_addr_AR  + addend )  :  w32 [31..12]@43 [11..0]@70 in iword[12]
// with default addend 0
case 45:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 31, 12, 43);
patch12bytes(Loc, Val, 11, 0, 70);
return;

// 46 : (symbol_addr_AR  + addend )  :  w32 [31..12]@21 [11..0]@48 in iword[12]
// with default addend 0
case 46:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 31, 12, 21);
patch12bytes(Loc, Val, 11, 0, 48);
return;

// 47 : (symbol_addr_AR  + addend )  :  w32 [31..12]@17 [11..0]@44 in iword[12]
// with default addend 0
case 47:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 31, 12, 17);
patch12bytes(Loc, Val, 11, 0, 44);
return;

// 48 : (symbol_addr_AR  + addend )  :  w32 [31..12]@38 [11..0]@65 in iword[14]
// with default addend 0
case 48:
checkUInt(ctx, Loc, Val, 20, rel);
patch14bytes(Loc, Val, 31, 12, 38);
patch14bytes(Loc, Val, 11, 0, 65);
return;

// 49 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..6]@59
// [5..0]@70 in iword[16] with default addend 0
case 49:
checkUInt(ctx, Loc, Val, 20, rel);
patch16bytes(Loc, Val, 10, 6, 59);
patch16bytes(Loc, Val, 5, 0, 70);
return;

// 50 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..6]@5
// [5..0]@16 in iword[4] with default addend 0
case 50:
checkUInt(ctx, Loc, Val, 20, rel);
patch4bytes(Loc, Val, 10, 6, 5);
patch4bytes(Loc, Val, 5, 0, 16);
return;

// 51 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..6]@21
// [5..0]@32 in iword[6] with default addend 0
case 51:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 10, 6, 21);
patch6bytes(Loc, Val, 5, 0, 32);
return;

// 52 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..6]@0
// [5..0]@11 in iword[6] with default addend 0
case 52:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 10, 6, 0);
patch6bytes(Loc, Val, 5, 0, 11);
return;

// 53 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..6]@10
// [5..0]@21 in iword[8] with default addend 0
case 53:
checkUInt(ctx, Loc, Val, 20, rel);
patch8bytes(Loc, Val, 10, 6, 10);
patch8bytes(Loc, Val, 5, 0, 21);
return;

// 54 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..6]@38
// [5..0]@49 in iword[8] with default addend 0
case 54:
checkUInt(ctx, Loc, Val, 20, rel);
patch8bytes(Loc, Val, 10, 6, 38);
patch8bytes(Loc, Val, 5, 0, 49);
return;

// 55 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..6]@53
// [5..0]@64 in iword[10] with default addend 0
case 55:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 6, 53);
patch10bytes(Loc, Val, 5, 0, 64);
return;

// 56 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..6]@26
// [5..0]@37 in iword[10] with default addend 0
case 56:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 6, 26);
patch10bytes(Loc, Val, 5, 0, 37);
return;

// 57 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..6]@1
// [5..0]@12 in iword[10] with default addend 0
case 57:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 6, 1);
patch10bytes(Loc, Val, 5, 0, 12);
return;

// 58 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..6]@43
// [5..0]@54 in iword[12] with default addend 0
case 58:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 6, 43);
patch12bytes(Loc, Val, 5, 0, 54);
return;

// 59 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..6]@21
// [5..0]@32 in iword[12] with default addend 0
case 59:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 6, 21);
patch12bytes(Loc, Val, 5, 0, 32);
return;

// 60 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..6]@17
// [5..0]@28 in iword[12] with default addend 0
case 60:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 6, 17);
patch12bytes(Loc, Val, 5, 0, 28);
return;

// 61 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..6]@38
// [5..0]@49 in iword[14] with default addend 0
case 61:
checkUInt(ctx, Loc, Val, 20, rel);
patch14bytes(Loc, Val, 10, 6, 38);
patch14bytes(Loc, Val, 5, 0, 49);
return;

// 62 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..7]@93
// [6..0]@86 in iword[16] with default addend 0
case 62:
checkUInt(ctx, Loc, Val, 20, rel);
patch16bytes(Loc, Val, 10, 7, 93);
patch16bytes(Loc, Val, 6, 0, 86);
return;

// 63 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..7]@18
// [6..0]@11 in iword[4] with default addend 0
case 63:
checkUInt(ctx, Loc, Val, 20, rel);
patch4bytes(Loc, Val, 10, 7, 18);
patch4bytes(Loc, Val, 6, 0, 11);
return;

// 64 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..7]@35
// [6..0]@28 in iword[6] with default addend 0
case 64:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 10, 7, 35);
patch6bytes(Loc, Val, 6, 0, 28);
return;

// 65 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..7]@34
// [6..0]@27 in iword[6] with default addend 0
case 65:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 10, 7, 34);
patch6bytes(Loc, Val, 6, 0, 27);
return;

// 66 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..7]@45
// [6..0]@38 in iword[8] with default addend 0
case 66:
checkUInt(ctx, Loc, Val, 20, rel);
patch8bytes(Loc, Val, 10, 7, 45);
patch8bytes(Loc, Val, 6, 0, 38);
return;

// 67 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..7]@19
// [6..0]@12 in iword[8] with default addend 0
case 67:
checkUInt(ctx, Loc, Val, 20, rel);
patch8bytes(Loc, Val, 10, 7, 19);
patch8bytes(Loc, Val, 6, 0, 12);
return;

// 68 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..7]@60
// [6..0]@53 in iword[10] with default addend 0
case 68:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 7, 60);
patch10bytes(Loc, Val, 6, 0, 53);
return;

// 69 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..7]@35
// [6..0]@28 in iword[10] with default addend 0
case 69:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 7, 35);
patch10bytes(Loc, Val, 6, 0, 28);
return;

// 70 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..7]@77
// [6..0]@70 in iword[12] with default addend 0
case 70:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 7, 77);
patch12bytes(Loc, Val, 6, 0, 70);
return;

// 71 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..7]@52
// [6..0]@45 in iword[12] with default addend 0
case 71:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 7, 52);
patch12bytes(Loc, Val, 6, 0, 45);
return;

// 72 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..7]@55
// [6..0]@48 in iword[12] with default addend 0
case 72:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 7, 55);
patch12bytes(Loc, Val, 6, 0, 48);
return;

// 73 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..7]@51
// [6..0]@44 in iword[12] with default addend 0
case 73:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 7, 51);
patch12bytes(Loc, Val, 6, 0, 44);
return;

// 74 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  u11 [10..7]@72
// [6..0]@65 in iword[14] with default addend 0
case 74:
checkUInt(ctx, Loc, Val, 20, rel);
patch14bytes(Loc, Val, 10, 7, 72);
patch14bytes(Loc, Val, 6, 0, 65);
return;

// 75 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  addr [19..0]@64 in
// iword[16] nostrict with default addend 0
case 75:
checkUInt(ctx, Loc, Val, 20, rel);
patch16bytes(Loc, Val, 19, 0, 64);
return;

// 76 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  addr [19..0]@5 in
// iword[6] nostrict with default addend 0
case 76:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 19, 0, 5);
return;

// 77 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  addr [19..0]@31 in
// iword[10] nostrict with default addend 0
case 77:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 19, 0, 31);
return;

// 78 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  addr [19..0]@6 in
// iword[10] nostrict with default addend 0
case 78:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 19, 0, 6);
return;

// 79 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  addr [19..0]@48 in
// iword[12] nostrict with default addend 0
case 79:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 19, 0, 48);
return;

// 80 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  addr [19..0]@26 in
// iword[12] nostrict with default addend 0
case 80:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 19, 0, 26);
return;

// 81 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  addr [19..0]@22 in
// iword[12] nostrict with default addend 0
case 81:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 19, 0, 22);
return;

// 82 : ((symbol_addr_AR  + addend )  - item_addr_AR )  :  addr [19..0]@43 in
// iword[14] nostrict with default addend 0
case 82:
checkUInt(ctx, Loc, Val, 20, rel);
patch14bytes(Loc, Val, 19, 0, 43);
return;

// 83 : (symbol_addr_AR  + addend )  :  w8 [7..0]@85 in iword[16]
// with default addend 0
case 83:
checkUInt(ctx, Loc, Val, 20, rel);
patch16bytes(Loc, Val, 7, 0, 85);
return;

// 84 : (symbol_addr_AR  + addend )  :  w8 [7..0]@10 in iword[4]
// with default addend 0
case 84:
checkUInt(ctx, Loc, Val, 20, rel);
patch4bytes(Loc, Val, 7, 0, 10);
return;

// 85 : (symbol_addr_AR  + addend )  :  w8 [7..0]@27 in iword[6]
// with default addend 0
case 85:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 7, 0, 27);
return;

// 86 : (symbol_addr_AR  + addend )  :  w8 [7..0]@26 in iword[6]
// with default addend 0
case 86:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 7, 0, 26);
return;

// 87 : (symbol_addr_AR  + addend )  :  w8 [7..0]@37 in iword[8]
// with default addend 0
case 87:
checkUInt(ctx, Loc, Val, 20, rel);
patch8bytes(Loc, Val, 7, 0, 37);
return;

// 88 : (symbol_addr_AR  + addend )  :  w8 [7..0]@11 in iword[8]
// with default addend 0
case 88:
checkUInt(ctx, Loc, Val, 20, rel);
patch8bytes(Loc, Val, 7, 0, 11);
return;

// 89 : (symbol_addr_AR  + addend )  :  w8 [7..0]@52 in iword[10]
// with default addend 0
case 89:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 7, 0, 52);
return;

// 90 : (symbol_addr_AR  + addend )  :  w8 [7..0]@27 in iword[10]
// with default addend 0
case 90:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 7, 0, 27);
return;

// 91 : (symbol_addr_AR  + addend )  :  w8 [7..0]@69 in iword[12]
// with default addend 0
case 91:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 7, 0, 69);
return;

// 92 : (symbol_addr_AR  + addend )  :  w8 [7..0]@44 in iword[12]
// with default addend 0
case 92:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 7, 0, 44);
return;

// 93 : (symbol_addr_AR  + addend )  :  w8 [7..0]@47 in iword[12]
// with default addend 0
case 93:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 7, 0, 47);
return;

// 94 : (symbol_addr_AR  + addend )  :  w8 [7..0]@43 in iword[12]
// with default addend 0
case 94:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 7, 0, 43);
return;

// 95 : (symbol_addr_AR  + addend )  :  w8 [7..0]@64 in iword[14]
// with default addend 0
case 95:
checkUInt(ctx, Loc, Val, 20, rel);
patch14bytes(Loc, Val, 7, 0, 64);
return;

// 96 : (symbol_addr_AR  + addend )  :  w8 [7..0]@91 in iword[16]
// with default addend 0
case 96:
checkUInt(ctx, Loc, Val, 20, rel);
patch16bytes(Loc, Val, 7, 0, 91);
return;

// 97 : (symbol_addr_AR  + addend )  :  w8 [7..0]@16 in iword[4]
// with default addend 0
case 97:
checkUInt(ctx, Loc, Val, 20, rel);
patch4bytes(Loc, Val, 7, 0, 16);
return;

// 98 : (symbol_addr_AR  + addend )  :  w8 [7..0]@33 in iword[6]
// with default addend 0
case 98:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 7, 0, 33);
return;

// 99 : (symbol_addr_AR  + addend )  :  w8 [7..0]@32 in iword[6]
// with default addend 0
case 99:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 7, 0, 32);
return;

// 100 : (symbol_addr_AR  + addend )  :  w8 [7..0]@43 in iword[8]
// with default addend 0
case 100:
checkUInt(ctx, Loc, Val, 20, rel);
patch8bytes(Loc, Val, 7, 0, 43);
return;

// 101 : (symbol_addr_AR  + addend )  :  w8 [7..0]@17 in iword[8]
// with default addend 0
case 101:
checkUInt(ctx, Loc, Val, 20, rel);
patch8bytes(Loc, Val, 7, 0, 17);
return;

// 102 : (symbol_addr_AR  + addend )  :  w8 [7..0]@58 in iword[10]
// with default addend 0
case 102:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 7, 0, 58);
return;

// 103 : (symbol_addr_AR  + addend )  :  w8 [7..0]@33 in iword[10]
// with default addend 0
case 103:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 7, 0, 33);
return;

// 104 : (symbol_addr_AR  + addend )  :  w8 [7..0]@75 in iword[12]
// with default addend 0
case 104:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 7, 0, 75);
return;

// 105 : (symbol_addr_AR  + addend )  :  w8 [7..0]@50 in iword[12]
// with default addend 0
case 105:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 7, 0, 50);
return;

// 106 : (symbol_addr_AR  + addend )  :  w8 [7..0]@53 in iword[12]
// with default addend 0
case 106:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 7, 0, 53);
return;

// 107 : (symbol_addr_AR  + addend )  :  w8 [7..0]@49 in iword[12]
// with default addend 0
case 107:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 7, 0, 49);
return;

// 108 : (symbol_addr_AR  + addend )  :  w8 [7..0]@70 in iword[14]
// with default addend 0
case 108:
checkUInt(ctx, Loc, Val, 20, rel);
patch14bytes(Loc, Val, 7, 0, 70);
return;

// 109 : (symbol_addr_AR  + addend )  :  u11 [10..7]@93 [6..0]@86 in iword[16]
// with default addend 0
case 109:
checkUInt(ctx, Loc, Val, 20, rel);
patch16bytes(Loc, Val, 10, 7, 93);
patch16bytes(Loc, Val, 6, 0, 86);
return;

// 110 : (symbol_addr_AR  + addend )  :  u11 [10..7]@18 [6..0]@11 in iword[4]
// with default addend 0
case 110:
checkUInt(ctx, Loc, Val, 20, rel);
patch4bytes(Loc, Val, 10, 7, 18);
patch4bytes(Loc, Val, 6, 0, 11);
return;

// 111 : (symbol_addr_AR  + addend )  :  u11 [10..7]@35 [6..0]@28 in iword[6]
// with default addend 0
case 111:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 10, 7, 35);
patch6bytes(Loc, Val, 6, 0, 28);
return;

// 112 : (symbol_addr_AR  + addend )  :  u11 [10..7]@34 [6..0]@27 in iword[6]
// with default addend 0
case 112:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 10, 7, 34);
patch6bytes(Loc, Val, 6, 0, 27);
return;

// 113 : (symbol_addr_AR  + addend )  :  u11 [10..7]@45 [6..0]@38 in iword[8]
// with default addend 0
case 113:
checkUInt(ctx, Loc, Val, 20, rel);
patch8bytes(Loc, Val, 10, 7, 45);
patch8bytes(Loc, Val, 6, 0, 38);
return;

// 114 : (symbol_addr_AR  + addend )  :  u11 [10..7]@19 [6..0]@12 in iword[8]
// with default addend 0
case 114:
checkUInt(ctx, Loc, Val, 20, rel);
patch8bytes(Loc, Val, 10, 7, 19);
patch8bytes(Loc, Val, 6, 0, 12);
return;

// 115 : (symbol_addr_AR  + addend )  :  u11 [10..7]@60 [6..0]@53 in iword[10]
// with default addend 0
case 115:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 7, 60);
patch10bytes(Loc, Val, 6, 0, 53);
return;

// 116 : (symbol_addr_AR  + addend )  :  u11 [10..7]@35 [6..0]@28 in iword[10]
// with default addend 0
case 116:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 7, 35);
patch10bytes(Loc, Val, 6, 0, 28);
return;

// 117 : (symbol_addr_AR  + addend )  :  u11 [10..7]@77 [6..0]@70 in iword[12]
// with default addend 0
case 117:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 7, 77);
patch12bytes(Loc, Val, 6, 0, 70);
return;

// 118 : (symbol_addr_AR  + addend )  :  u11 [10..7]@52 [6..0]@45 in iword[12]
// with default addend 0
case 118:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 7, 52);
patch12bytes(Loc, Val, 6, 0, 45);
return;

// 119 : (symbol_addr_AR  + addend )  :  u11 [10..7]@55 [6..0]@48 in iword[12]
// with default addend 0
case 119:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 7, 55);
patch12bytes(Loc, Val, 6, 0, 48);
return;

// 120 : (symbol_addr_AR  + addend )  :  u11 [10..7]@51 [6..0]@44 in iword[12]
// with default addend 0
case 120:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 7, 51);
patch12bytes(Loc, Val, 6, 0, 44);
return;

// 121 : (symbol_addr_AR  + addend )  :  u11 [10..7]@72 [6..0]@65 in iword[14]
// with default addend 0
case 121:
checkUInt(ctx, Loc, Val, 20, rel);
patch14bytes(Loc, Val, 10, 7, 72);
patch14bytes(Loc, Val, 6, 0, 65);
return;

// 122 : (symbol_addr_AR  + addend )  :  u11 [10..6]@59 [5..0]@70 in iword[16]
// with default addend 0
case 122:
checkUInt(ctx, Loc, Val, 20, rel);
patch16bytes(Loc, Val, 10, 6, 59);
patch16bytes(Loc, Val, 5, 0, 70);
return;

// 123 : (symbol_addr_AR  + addend )  :  u11 [10..6]@5 [5..0]@16 in iword[4]
// with default addend 0
case 123:
checkUInt(ctx, Loc, Val, 20, rel);
patch4bytes(Loc, Val, 10, 6, 5);
patch4bytes(Loc, Val, 5, 0, 16);
return;

// 124 : (symbol_addr_AR  + addend )  :  u11 [10..6]@21 [5..0]@32 in iword[6]
// with default addend 0
case 124:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 10, 6, 21);
patch6bytes(Loc, Val, 5, 0, 32);
return;

// 125 : (symbol_addr_AR  + addend )  :  u11 [10..6]@0 [5..0]@11 in iword[6]
// with default addend 0
case 125:
checkUInt(ctx, Loc, Val, 20, rel);
patch6bytes(Loc, Val, 10, 6, 0);
patch6bytes(Loc, Val, 5, 0, 11);
return;

// 126 : (symbol_addr_AR  + addend )  :  u11 [10..6]@10 [5..0]@21 in iword[8]
// with default addend 0
case 126:
checkUInt(ctx, Loc, Val, 20, rel);
patch8bytes(Loc, Val, 10, 6, 10);
patch8bytes(Loc, Val, 5, 0, 21);
return;

// 127 : (symbol_addr_AR  + addend )  :  u11 [10..6]@38 [5..0]@49 in iword[8]
// with default addend 0
case 127:
checkUInt(ctx, Loc, Val, 20, rel);
patch8bytes(Loc, Val, 10, 6, 38);
patch8bytes(Loc, Val, 5, 0, 49);
return;

// 128 : (symbol_addr_AR  + addend )  :  u11 [10..6]@53 [5..0]@64 in iword[10]
// with default addend 0
case 128:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 6, 53);
patch10bytes(Loc, Val, 5, 0, 64);
return;

// 129 : (symbol_addr_AR  + addend )  :  u11 [10..6]@26 [5..0]@37 in iword[10]
// with default addend 0
case 129:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 6, 26);
patch10bytes(Loc, Val, 5, 0, 37);
return;

// 130 : (symbol_addr_AR  + addend )  :  u11 [10..6]@1 [5..0]@12 in iword[10]
// with default addend 0
case 130:
checkUInt(ctx, Loc, Val, 20, rel);
patch10bytes(Loc, Val, 10, 6, 1);
patch10bytes(Loc, Val, 5, 0, 12);
return;

// 131 : (symbol_addr_AR  + addend )  :  u11 [10..6]@43 [5..0]@54 in iword[12]
// with default addend 0
case 131:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 6, 43);
patch12bytes(Loc, Val, 5, 0, 54);
return;

// 132 : (symbol_addr_AR  + addend )  :  u11 [10..6]@21 [5..0]@32 in iword[12]
// with default addend 0
case 132:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 6, 21);
patch12bytes(Loc, Val, 5, 0, 32);
return;

// 133 : (symbol_addr_AR  + addend )  :  u11 [10..6]@17 [5..0]@28 in iword[12]
// with default addend 0
case 133:
checkUInt(ctx, Loc, Val, 20, rel);
patch12bytes(Loc, Val, 10, 6, 17);
patch12bytes(Loc, Val, 5, 0, 28);
return;

// 134 : (symbol_addr_AR  + addend )  :  u11 [10..6]@38 [5..0]@49 in iword[14]
// with default addend 0
case 134:
checkUInt(ctx, Loc, Val, 20, rel);
patch14bytes(Loc, Val, 10, 6, 38);
patch14bytes(Loc, Val, 5, 0, 49);
return;
