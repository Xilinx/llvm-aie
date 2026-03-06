//===-- AIEGenericOpcode.h - Cross-target opcode parity --------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Always-on opcode parity validation between AIEBaseTarget and each AIE
// subtarget.  Provides the AIE_CHECK_OPCODE_ macro used by each target's
// InstrInfo constructor to verify enum value equality.
//
// Usage in each target's InstrInfo.cpp:
//
//   #include "AIEGenericOpcode.h"
//
//   namespace {
//   void verifyOpcodeParity() {
//     #define HANDLE_TARGET_OPCODE(OPC) \
//       AIE_CHECK_OPCODE_(AIEBase, MyTargetNS, OPC)
//     #define HANDLE_TARGET_OPCODE_MARKER(IDENT, OPC)
//     #include "llvm/Support/TargetOpcodes.def"
//
//     #define AIE_GENERIC_OPCODE(OPC) \
//       AIE_CHECK_OPCODE_(AIEBase, MyTargetNS, OPC)
//     #include "AIEGenericOpcodeList.def"
//   }
//   } // anonymous namespace
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEGENERICOPCODE_H
#define LLVM_LIB_TARGET_AIE_AIEGENERICOPCODE_H

#include "AIEBaseTargetOpcodes.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/ErrorHandling.h"

/// Check a single opcode for parity between BASE_NS and TARGET_NS.
/// Hard-fails via report_fatal_error on mismatch in all build modes.
#define AIE_CHECK_OPCODE_(BASE_NS, TARGET_NS, OPC)                             \
  if (static_cast<unsigned>(BASE_NS::OPC) !=                                   \
      static_cast<unsigned>(TARGET_NS::OPC))                                   \
    llvm::report_fatal_error(                                                  \
        llvm::Twine("opcode parity mismatch: " #BASE_NS "::" #OPC " (") +      \
            llvm::Twine(static_cast<unsigned>(BASE_NS::OPC)) +                 \
            ") != " #TARGET_NS "::" #OPC " (" +                                \
            llvm::Twine(static_cast<unsigned>(TARGET_NS::OPC)) + ")",          \
        /*gen_crash_diag=*/false);

#endif // LLVM_LIB_TARGET_AIE_AIEGENERICOPCODE_H
