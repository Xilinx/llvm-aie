//===-- AIEBaseInstrFormats.td -Target Description for AIE--*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains a generic utility class to describe AIEX Instructions
//
//===----------------------------------------------------------------------===//

class AIEBaseInst<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr, string namespace>
    : Instruction, InstFormat {
  let Size = 4;
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let Pattern = pattern;
  let isComposite = false;
  let Namespace = namespace;
  let AsmString = opcodestr # "\t" # argstr;

  // A scratchpad of bits that is used in the encoding to say "these bits do not
  // matter for encoding or decoding".
  field bits<32> dontcare = 0;
}

// Attributes for Pseudo instructions
class AIEPseudoInstrAttributes {
  field bit isPseudo;
  field bit isCodeGenOnly;
  let isPseudo = true;
  let isCodeGenOnly = true;
}
