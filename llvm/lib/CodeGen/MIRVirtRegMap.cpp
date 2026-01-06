//===- MIRVirtRegMap.cpp - MIR VirtRegMap Info -----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2026 Advanced Micro Devices, Inc. or its
// affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the MIRVirtRegMapWrapperLegacy pass, which provides
// access to VirtRegMap assignments loaded from MIR files.
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/MIRVirtRegMap.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/InitializePasses.h"

using namespace llvm;

char MIRVirtRegMapWrapperLegacy::ID = 0;

INITIALIZE_PASS(MIRVirtRegMapWrapperLegacy, "mir-virtregmap",
                "MIR Virtual Register Map", false, true)

MIRVirtRegMapWrapperLegacy::MIRVirtRegMapWrapperLegacy()
    : MachineFunctionPass(ID) {
  initializeMIRVirtRegMapWrapperLegacyPass(*PassRegistry::getPassRegistry());
}

bool MIRVirtRegMapWrapperLegacy::runOnMachineFunction(MachineFunction &MF) {
  // Get MIR-loaded info from MachineFunction (if it exists)
  Info = MF.getMIRVirtRegMapInfo();
  return false;
}

void MIRVirtRegMapWrapperLegacy::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
  MachineFunctionPass::getAnalysisUsage(AU);
}
