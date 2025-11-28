//===-- AIEBaseRegisterInfo.td - Base Register Info for AIE -*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains a generic utility class for AIE register classes
//
//===----------------------------------------------------------------------===//

// Base class for all AIE register classes across all targets
// This centralizes common decoder properties that should be applied to all
// register classes in the AIE targets.
class AIEBaseRegisterClass<string namespace, list<ValueType> regTypes, int alignment,
                    dag regList, RegAltNameIndex idx = NoRegAltName> :
            RegisterClass<namespace, regTypes, alignment, regList, idx> {
  // Enable decoding of zero-bit operands
  bit DecodeZeroBitOperand = true;
  
  // Disable complete decoder for all register classes to avoid decoder
  // conflicts between instructions with overlapping encodings
  bit hasCompleteDecoder = false;
}

// Base class for all AIE register operands across all targets
// This centralizes common decoder properties for register operands.
class AIEBaseRegisterOperand<RegisterClass c> : RegisterOperand<c> {
  // Set encoder method based on register class name
  let EncoderMethod = "get" # c # OpValue;
  
  // Disable complete decoder for all register operands to avoid decoder
  // conflicts between instructions with overlapping encodings
  let hasCompleteDecoder = false;
}

