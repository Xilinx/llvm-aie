//===- AIETailDuplication.h - AIE Tail Duplication Pass ----------*- C++
//-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIETAILDUPLICATION_H
#define LLVM_LIB_TARGET_AIE_AIETAILDUPLICATION_H

namespace llvm {

class FunctionPass;
class PassRegistry;

extern char &AIETailDuplicateLateID;
extern char &AIETailDuplicateEarlyID;

FunctionPass *createAIETailDuplicateEarlyPass();
FunctionPass *createAIETailDuplicateLatePass();

void initializeAIETailDuplicateLatePass(PassRegistry &);
void initializeAIETailDuplicateEarlyPass(PassRegistry &);

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIETAILDUPLICATION_H
