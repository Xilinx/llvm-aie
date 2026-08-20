# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates

// RUN: split-file %s %t
// RUN: llvm-tblgen -gen-aie-copy-materialization -I %t -I %p/../../include %t/valid.td | FileCheck %s
// RUN: not llvm-tblgen -gen-aie-copy-materialization -I %t -I %p/../../include %t/duplicate.td 2>&1 | FileCheck %s --check-prefix=DUPLICATE
// RUN: not llvm-tblgen -gen-aie-copy-materialization -I %t -I %p/../../include %t/missing-leaf.td 2>&1 | FileCheck %s --check-prefix=MISSING
// RUN: not llvm-tblgen -gen-aie-copy-materialization -I %t -I %p/../../include %t/invalid-subreg.td 2>&1 | FileCheck %s --check-prefix=INVALID
// RUN: not llvm-tblgen -gen-aie-copy-materialization -I %t -I %p/../../include %t/nonuniform.td 2>&1 | FileCheck %s --check-prefix=NONUNIFORM
// RUN: not llvm-tblgen -gen-aie-copy-materialization -I %t -I %p/../../include %t/ambiguous.td 2>&1 | FileCheck %s --check-prefix=AMBIGUOUS
// RUN: not llvm-tblgen -gen-aie-copy-materialization -I %t -I %p/../../include %t/shadowed-direct.td 2>&1 | FileCheck %s --check-prefix=SHADOWED-DIRECT
// RUN: not llvm-tblgen -gen-aie-copy-materialization -I %t -I %p/../../include %t/empty-actions.td 2>&1 | FileCheck %s --check-prefix=EMPTY-ACTIONS
// RUN: not llvm-tblgen -gen-aie-copy-materialization -I %t -I %p/../../include %t/empty-subregs.td 2>&1 | FileCheck %s --check-prefix=EMPTY-SUBREGS

// CHECK: static constexpr CopyTuple const CopyTuples[] = {
// CHECK: TestTarget::sub_lo, TestTarget::sub_lo, TestTarget::MOV0
// CHECK-NEXT: TestTarget::sub_hi, TestTarget::sub_hi, TestTarget::MOV0
// CHECK: static constexpr CopyRecipe const CopyRecipes[] = {
// CHECK: {&TestTarget::LeafRegClass, &TestTarget::LeafRegClass, 0, 1}
// CHECK-NEXT: {&TestTarget::PairRegClass, &TestTarget::PairRegClass, 1, 2}
// CHECK-NEXT: {&TestTarget::DstLeafRegClass, &TestTarget::LeafRegClass, 3, 1}
// CHECK: const CopyTableView &TestTargetInstrInfo::getCopyTable() const
// CHECK: static constexpr CopyTableView Table{CopyRecipes, CopyTuples};
// CHECK-NOT: CopyOperand
// CHECK-NOT: CopyStep
// CHECK-NOT: CopyInstrInfo

// DUPLICATE: error: duplicate copy recipe match
// MISSING: error: no copy recipe for recursive leaf R0 -> R0
// INVALID: error: recursive copy names an invalid subregister
// NONUNIFORM: error: copy recipe has a nonuniform recursive expansion
// AMBIGUOUS: error: equal-priority copy recipes {{.*}} have overlapping matches
// SHADOWED-DIRECT: error: direct-copy register pair selects a non-direct copy rule
// EMPTY-ACTIONS: error: copy recipe must contain at least one action
// EMPTY-SUBREGS: error: recursive copy must name a source subregister

//--- common.td
include "llvm/Target/Target.td"
include "llvm/Target/AIECopyMaterialization.td"

def TestTarget : Target;

let Namespace = "TestTarget" in {
def sub_lo : SubRegIndex<32, 0>;
def sub_hi : SubRegIndex<32, 32>;
def sub_bad : SubRegIndex<32, 64>;

def R0 : Register<"r0">;
def R1 : Register<"r1">;
def R2 : Register<"r2">;
def P0 : Register<"p0"> {
  let SubRegs = [R0, R1];
  let SubRegIndices = [sub_lo, sub_hi];
  let CoveredBySubRegs = 1;
}

def Leaf : RegisterClass<"TestTarget", [i32], 32, (add R0, R1)>;
def DstLeaf : RegisterClass<"TestTarget", [i32], 32, (add R2)>;
def Pair : RegisterClass<"TestTarget", [v2i32], 64, (add P0)>;

def MOV0 : Instruction;
def MOV1 : Instruction;
def MOV2 : Instruction;
def MOV3 : Instruction;
}

//--- valid.td
include "common.td"

def COPY_Leaf : CopyRecipe<Leaf, Leaf, [FullRegCopy<MOV0>]>;
def COPY_Pair : CopyRecipe<Pair, Pair, [CopyAllSubRegs]>;
def COPY_ZLeaf_DstLeaf : CopyRecipe<DstLeaf, Leaf, [FullRegCopy<MOV1>]>;

//--- duplicate.td
include "common.td"

def COPY_Leaf0 : CopyRecipe<Leaf, Leaf, [FullRegCopy<MOV0>]>;
def COPY_Leaf1 : CopyRecipe<Leaf, Leaf, [FullRegCopy<MOV0>]>;

//--- empty-actions.td
include "common.td"

def COPY_Leaf : CopyRecipe<Leaf, Leaf, []>;

//--- empty-subregs.td
include "common.td"

def COPY_Pair : CopyRecipe<Pair, Pair, [CopyThroughSubRegs<[], []>]>;

//--- missing-leaf.td
include "common.td"

def COPY_Pair : CopyRecipe<Pair, Pair, [CopyAllSubRegs]>;

//--- invalid-subreg.td
include "common.td"

def COPY_Pair : CopyRecipe<Pair, Pair, [
  CopyThroughSubRegs<[sub_bad], [sub_bad]>
]>;

//--- nonuniform.td
include "common.td"

let Namespace = "TestTarget" in {
def R3 : Register<"r3">;
def P1 : Register<"p1"> {
  let SubRegs = [R2, R3];
  let SubRegIndices = [sub_lo, sub_hi];
  let CoveredBySubRegs = 1;
}
def LeafA : RegisterClass<"TestTarget", [i32], 32, (add R0, R3)>;
def LeafB : RegisterClass<"TestTarget", [i32], 32, (add R1, R2)>;
def PairBoth : RegisterClass<"TestTarget", [v2i32], 64, (add P0, P1)>;
}

def COPY_A_A : CopyRecipe<LeafA, LeafA, [FullRegCopy<MOV0>]>;
def COPY_A_B : CopyRecipe<LeafB, LeafA, [FullRegCopy<MOV1>]>;
def COPY_B_A : CopyRecipe<LeafA, LeafB, [FullRegCopy<MOV2>]>;
def COPY_B_B : CopyRecipe<LeafB, LeafB, [FullRegCopy<MOV3>]>;
def COPY_Pair : CopyRecipe<PairBoth, PairBoth, [CopyAllSubRegs]>;

//--- ambiguous.td
include "common.td"

let Namespace = "TestTarget" in
def LeafSubset : RegisterClass<"TestTarget", [i32], 32, (add R0)>;

def COPY_Leaf : CopyRecipe<Leaf, Leaf, [FullRegCopy<MOV0>]>;
def COPY_LeafSubset
    : CopyRecipe<LeafSubset, LeafSubset, [FullRegCopy<MOV1>]>;

//--- shadowed-direct.td
include "common.td"

let Namespace = "TestTarget" in
def LeafSubset : RegisterClass<"TestTarget", [i32], 32, (add R0)>;

defm COPY_Direct : DirectCopy<Leaf, Leaf, MOV0>;
def COPY_Shadow : CopyRecipe<LeafSubset, LeafSubset,
                             [FullRegCopy<MOV1>, FullRegCopy<MOV2>]> {
  let Priority = 10;
}
