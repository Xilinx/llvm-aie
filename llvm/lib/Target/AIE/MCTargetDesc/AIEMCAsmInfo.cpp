//===- AIEMCAsmInfo.cpp - AIE asm properties --------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEMCAsmInfo.h"
using namespace llvm;

void AIEMCAsmInfo::anchor() {}

AIEMCAsmInfo::AIEMCAsmInfo(const Triple &TT) {
  SupportsDebugInformation = true;
  Data16bitsDirective = "\t.short\t";
  Data32bitsDirective = "\t.word\t";
  Data64bitsDirective = nullptr;
  ZeroDirective = "\t.space\t";
  CommentString = "//";

  MaxInstLength = 16; // We have 16 byte instructions
  UsesELFSectionDirectiveForBSS = true;
  AllowAtInName = true;
  HiddenVisibilityAttr = MCSA_Invalid;
  HiddenDeclarationVisibilityAttr = MCSA_Invalid;
  ProtectedVisibilityAttr = MCSA_Invalid;

  // Debug
  //ExceptionsType = ExceptionHandling::DwarfCFI;
  // DwarfRegNumForCFI = true;
}
