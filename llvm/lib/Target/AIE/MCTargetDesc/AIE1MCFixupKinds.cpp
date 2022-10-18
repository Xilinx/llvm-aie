//===-- AIE1MCFixupKinds.cpp - AIE1 Specific Fixup Entries ------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE1MCFixupKinds.h"
#include "AIEMCTargetDesc.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>

using namespace llvm;

// This array lists the ambiguous cases of the ISA where signedness is
// important. It should record all the relocatable instructions that need
// disambiguation when choosing a particular fixup/relocation. For AIE1, there
// are only 2 instructions (3 opcodes) of this kind.
// All of the others relocatable instructions are automatically considered as
// (FixupFlag::)Unconstrained when calling getInstrFieldSignedness().
const static std::map<unsigned, FixupFlag> AIE1InstrFixupFlags = {
    {AIE::MOV_S20, FixupFlag::isDataSigned},
    {AIE::MOV_U20, FixupFlag::isDataUnsigned},
    {AIE::MOV_U20_I20, FixupFlag::isDataUnsigned}};

// Need to be placed after InstrFixupFlags definition
#define GET_MCFIXUPKINDS_IMPLEM
#include "FixupInfo/AIE1FixupInfo.inc"
