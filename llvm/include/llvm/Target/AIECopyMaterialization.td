//===--- AIECopyMaterialization.td - AIE copy materialization -*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// TableGen definitions for physical-register copies which consist of one or
// more moves between corresponding source and destination (sub)registers.
//
//===----------------------------------------------------------------------===//

class CopyAction;

// One ordinary source-to-destination instruction in a copy recipe.
class CopyTuple<Instruction moveOpcode> : CopyAction {
  SubRegIndex DstSubReg = ?;
  SubRegIndex SrcSubReg = ?;
  Instruction MoveOpcode = moveOpcode;
}

class SubRegCopy<SubRegIndex dstSubReg, SubRegIndex srcSubReg,
                 Instruction moveOpcode> : CopyTuple<moveOpcode> {
  let DstSubReg = dstSubReg;
  let SrcSubReg = srcSubReg;
}

class SameSubRegCopy<SubRegIndex subReg, Instruction moveOpcode>
    : SubRegCopy<subReg, subReg, moveOpcode>;

class FullRegCopy<Instruction moveOpcode> : CopyTuple<moveOpcode>;

class CopyThroughAllSubRegs : CopyAction;
class CopyThroughSubRegs<list<SubRegIndex> dstSubRegs,
                         list<SubRegIndex> srcSubRegs> : CopyAction {
  list<SubRegIndex> DstSubRegs = dstSubRegs;
  list<SubRegIndex> SrcSubRegs = srcSubRegs;
}

def CopyAllSubRegs : CopyThroughAllSubRegs;

class CopyRecipe<RegisterClass dstRC, RegisterClass srcRC,
                 list<CopyAction> actions, int priority = 0> {
  RegisterClass DstRC = dstRC;
  RegisterClass SrcRC = srcRC;
  list<CopyAction> Actions = actions;
  int Priority = priority;
}

multiclass DirectCopy<RegisterClass dst, RegisterClass src, Instruction opcode,
                      int priority = 0> {
  def NAME : CopyRecipe<dst, src, [FullRegCopy<opcode>], priority>;
}
