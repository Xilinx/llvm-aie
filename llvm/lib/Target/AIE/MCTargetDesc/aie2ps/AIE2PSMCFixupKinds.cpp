//===-- AIE2PSMCFixupKinds.cpp - AIE2ps Specific Fixup Entries --*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//

#include "AIE2PSMCFixupKinds.h"
#include "AIEMCTargetDesc.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>

using namespace llvm;

const static std::map<unsigned, FixupFlag> AIE2PSInstrFixupFlags = {};

// Need to be placed after InstrFixupFlags definition
#define GET_MCFIXUPKINDS_IMPLEM
#include "FixupInfo/AIE2PSFixupInfo.inc"
