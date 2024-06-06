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

// A helper class to split a dag into two dags. The elements in [0, idx) will
// be added to DagBeforeSplit, those in [idx + 1, size(DAG)) will be in
// DagAfterSplit. Note that the element at index idx won't be in either of those.
class SplitDAGOps<dag DAG, int idx> {
  list<string> DagKeysBefore = !foreach(i, !range(0, idx), !getdagname(DAG, i));
  list<DAGOperand> DagArgsBefore = !foreach(key, DagKeysBefore,
      !getdagarg<DAGOperand>(DAG, key));
  list<string> DagKeysAfter = !foreach(i, !range(!add(idx, 1), !size(DAG)), !getdagname(DAG, i));
  list<DAGOperand> DagArgsAfter = !foreach(key, DagKeysAfter,
      !getdagarg<DAGOperand>(DAG, key));
  dag DagBeforeSplit = !dag(!getdagop(DAG), DagArgsBefore, DagKeysBefore);
  dag DagAfterSplit = !dag(!getdagop(DAG), DagArgsAfter, DagKeysAfter);
}

// A pseudo instruction that has similar properties to RealInst, but the operand
// at index idx is split into NewSplitIns.
// Note that the to-be-split operand must be an input.
class SplitPseudo<Instruction RealInst, int idx, dag NewSplitIns> :
    AIEBaseInst<(outs), (ins), [], "", "", "">, AIE_HasTiedSubregister, AIEPseudoInstrAttributes {

  let OutOperandList = RealInst.OutOperandList;
  SplitDAGOps SplitOps = SplitDAGOps<RealInst.InOperandList, !sub(idx, !size(OutOperandList))>;
  let InOperandList = !con(SplitOps.DagBeforeSplit, NewSplitIns, SplitOps.DagAfterSplit);

  // Copy properties over from RealInst
  let Constraints = RealInst.Constraints;
  let mayStore = RealInst.mayStore;
  let mayLoad = RealInst.mayLoad;
  let hasSideEffects = RealInst.hasSideEffects;
  let Defs = RealInst.Defs;
  let Uses = RealInst.Uses;
  let Namespace = RealInst.Namespace;
}
