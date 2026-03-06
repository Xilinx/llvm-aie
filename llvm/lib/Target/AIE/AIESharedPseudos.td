//===-- AIESharedPseudos.td - Shared pseudo instructions ---*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Shared pseudo instruction classes for instructions that exist with the same
// name across AIE targets. These classes share the same name across AIE targets to be in
// Layer 2 of the enum layout, guaranteeing identical enum values across all
// target compilations.
//
// This file must be included AFTER the target's InstrFormats.td, so that
// `Pseudo` resolves to the target-specific class (e.g., AIE2Inst or AIE2PInst
// based).
//
//===----------------------------------------------------------------------===//

// Base class for all shared pseudos. Provides a base class for shared pseudo instructions in
// enum placement in Layer 2.
class AIESharedPseudo<dag outs, dag ins, string opcodestr = "",
                      string argstr = "">
    : Pseudo<outs, ins, opcodestr, argstr> {
}

//===----------------------------------------------------------------------===//
// Identical across targets (no parameters needed)
//===----------------------------------------------------------------------===//

class AIEDelayedSchedBarrier : AIESharedPseudo<(outs), (ins)> {
  let isMeta = true;
  let hasSideEffects = false;
  let mayLoad = false;
  let mayStore = false;
}

class AIECycleSeparator : AIESharedPseudo<(outs), (ins)> {
  let isMeta = true;
  let hasSideEffects = false;
  let mayLoad = false;
  let mayStore = false;
}

class AIELoopDec
    : AIESharedPseudo<(outs eR:$dst), (ins eR:$src),
                      "loop_dec", "${dst}, ${src}"> {
  let hasSideEffects = false;
  let mayLoad = false;
  let mayStore = false;
  let Defs = [srCarry];
}

class AIELoopJNZ
    : AIESharedPseudo<(outs), (ins eR:$tc, eP:$target),
                      "loop_jnz", "${tc}, ${target}"> {
  let hasSideEffects = false;
  let mayLoad = false;
  let mayStore = false;
  let isBranch = true;
  let isIndirectBranch = true;
  let isTerminator = true;
}

//===----------------------------------------------------------------------===//
// Parameterized (operand types differ per target)
//===----------------------------------------------------------------------===//

class AIELoopStart<Operand adj_type>
    : AIESharedPseudo<(outs), (ins eR:$src, adj_type:$adj),
                      "loop_start", "${src}, ${adj}"> {
  let hasSideEffects = true;
  let mayLoad = false;
  let mayStore = false;
  let isNotDuplicable = true;
}

class AIEPseudoLoopEnd<Operand addr_type>
    : AIESharedPseudo<(outs), (ins addr_type:$lastInstr, addr_type:$target),
                      "pseudo_loop_end", "${lastInstr}, ${target}"> {
  let hasSideEffects = true;
  let mayLoad = false;
  let mayStore = false;
  let isNotDuplicable = true;
  let isBranch = true;
  let isTerminator = true;
  let isMeta = true;
}

class AIEAdjCallStackDown<Register sp_reg>
    : AIESharedPseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2)> {
  let hasSideEffects = false;
  let mayLoad = false;
  let mayStore = false;
  let Defs = [sp_reg];
  let Uses = [sp_reg];
}

class AIEAdjCallStackUp<Register sp_reg>
    : AIESharedPseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2)> {
  let hasSideEffects = false;
  let mayLoad = false;
  let mayStore = false;
  let Defs = [sp_reg];
  let Uses = [sp_reg];
}
