// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
// RUN: llvm-tblgen -gen-global-isel -I %p/../../include -I %p/Common %s -o - | FileCheck %s

include "llvm/Target/Target.td"
include "GlobalISelEmitterCommon.td"

// Tests the ModelsInaccessibleMemThroughRegs attribute
def int_intr_1 : Intrinsic<[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;

def NOP : I<(outs GPR32:$dst), (ins GPR32:$src), []>;

def : Pat<(int_intr_1 GPR32:$src),
          (NOP GPR32:$src)> {let ModelsInaccessibleMemThroughRegs = true;}

// CHECK:     GIM_Try, /*On fail goto*//*Label 0*/ GIMT_Encode4([[NEXT_OPCODE_LABEL:[0-9]+]]), // Rule ID 0 //
// CHECK-NEXT:       GIM_CheckOpcode, /*MI*/0, GIMT_Encode2(TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS),
// CHECK-NEXT:       GIM_CheckNumOperands, /*MI*/0, /*Expected*/3,
// CHECK-NEXT:       GIM_CheckIntrinsicID, /*MI*/0, /*Op*/1, GIMT_Encode2(Intrinsic::intr_1),
// CHECK-NEXT:       GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
// CHECK-NEXT:       GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
// CHECK-NEXT:       GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/GIMT_Encode2(MyTarget::GPR32RegClassID),
// CHECK-NEXT:       GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/GIMT_Encode2(MyTarget::GPR32RegClassID),
// CHECK-NEXT:       // (intrinsic_w_chain:{ *:[i32] } 180:{ *:[iPTR] }, GPR32:{ *:[i32] }:$src)  =>  (NOP:{ *:[i32] } GPR32:{ *:[i32] }:$src)
// CHECK-NEXT:       GIR_BuildMI, /*InsnID*/0, /*Opcode*/GIMT_Encode2(MyTarget::NOP),
// CHECK-NEXT:       GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // DstI[dst]
// CHECK-NEXT:       GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/2, // src
// CHECK-NEXT:       GIR_ConstrainSelectedInstOperands, /*InsnID*/0,
// CHECK-NEXT:       GIR_EraseFromParent, /*InsnID*/0,
// CHECK-NEXT:       // GIR_Coverage, 0,
// CHECK-NEXT:       GIR_Done,
// CHECK-NEXT:     // Label 0: @[[NEXT_OPCODE_LABEL]]
// CHECK-NEXT:     GIM_Reject,
