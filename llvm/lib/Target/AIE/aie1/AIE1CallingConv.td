//===-- AIECallingConv.td - Calling Conventions AIE ----*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the AIE architecture.
//
//===----------------------------------------------------------------------===//

// Ref: Architecture spec 2.0 section 5.11
// callee_saved : r4, r10, r11, p6, p7;
// argument_registers : r6, r7, r8, r9;
// argument_registers void* : p0, p1, p2, p3, p4, p5;
// argument_registers_return : r0,r1;
// argument_registers_return void* : p0,p1;
// argument_registers v8w16 : VL,VH;
// argument_registers v16w16 : W;
// argument_registers v8w48 : AM;
// argument_registers v16w48 : BM;
// argument_registers v32w16 : XA,XB,XC,XD;
// argument_registers v64w16 : YA,YD;
def AssignToPTR     : CCAssignToReg<[p0, p1, p2, p3, p4, p5]>;
def AssignToVEC128  : CCAssignToReg<[vl0, vl1, vl2, vl3, vl4, vl5, vl6, vl7,
											 vh0, vh1, vh2, vh3, vh4, vh5, vh6, vh7]>;
def AssignToVEC256  : CCAssignToReg<[wr0, wr1, wr2, wr3, wc0, wc1, wd0, wd1]>;
def AssignToVEC512  : CCAssignToReg<[xa, xb, xc, xd]>;
def AssignToVEC1024 : CCAssignToReg<[ya, yd]>;
def AssignToACC384  : CCAssignToReg<[aml0, aml1, aml2, aml3,
											    amh0, amh1, amh2, amh3]>;
def AssignToACC768  : CCAssignToReg<[bm0, bm1, bm2, bm3]>;

def RetCC_AIE : CallingConv<[
  // pointers are passed in registers
  CCIfType<[ptr0], AssignToPTR>,
  CCIf<"ArgFlags.isPointer()", AssignToPTR>, // SelectionDAG pointer support

  // Promote i1/i8/i16 arguments to i32.
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,

  // Vector types return in registers?
  CCIfType<[v16i8, v8i16, v4i32, v4f32], AssignToVEC128>,
  CCIfType<[v32i8, v16i16, v16i16, v8i32, v8f32, v4i64], AssignToVEC256>,
  CCIfType<[v64i8, v32i16, v32i16, v16i32, v16f32, v8i64], AssignToVEC512>,
  CCIfType<[v128i8, v64i16, v64i16, v32i32, v32f32], AssignToVEC1024>,
  CCIfType<[v8i48], AssignToACC384>,
  CCIfType<[v16i48], AssignToACC768>,
  CCIfType<[v2i32], CCCustom<"CC_AIE_Handle_V2I32_Ret">>,


  CCIf<"ArgFlags.isSplit() || !State.getPendingLocs().empty()",
       CCCustom<"CC_AIE_Handle_Split_Arg_Ret">>,

  // i32 are returned in registers
  CCIfType<[i32, f32, v8i1, v16i1], CCAssignToReg<[r0, r6, r7, r8]>>,
]>;

//===----------------------------------------------------------------------===//
// AIE Argument Calling Conventions
//===----------------------------------------------------------------------===//
def CC_AIE_Stack : CallingConv<[
  // Promote i8/i16 arguments to i32.
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,

  // Do not split arguments half on the stack, and half in registers.
  // Handle them specifically.
  CCIf<"ArgFlags.isSplit() || !State.getPendingLocs().empty()",
       CCCustom<"CC_AIEX_Handle_Split_Arg_Stack">>,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned. Same for pointers.
  CCIfType<[i32, f32, v8i1, v16i1, ptr0], CCAssignToStack<4, 4>>,
  CCIf<"ArgFlags.isPointer()", CCAssignToStack<4, 4>>, // SelectionDAG pointer support

  // Vector arguments can go on stack, aligned
  CCIfType<[v16i8, v8i16, v4i32, v4f32], CCAssignToStack<4, 4>>,
  CCIfType<[v32i8, v16i16, v8i32, v8f32, v4i64], CCAssignToStack<32, 32>>,
  CCIfType<[v64i8, v32i16, v16i32, v16f32, v8i64], CCAssignToStack<64, 64>>,
  CCIfType<[v128i8, v64i16, v32i32, v32f32], CCAssignToStack<128, 128>>,
  CCIfType<[v8i48], CCAssignToStack<0, 0>>,
  CCIfType<[v16i48], CCAssignToStack<0, 0>>,

  // TODO: handle custom lowering for v2i32
]>;

def CC_AIE : CallingConv<[
  // pointers are passed in registers
  CCIfType<[ptr0], AssignToPTR>,
  CCIf<"ArgFlags.isPointer()", AssignToPTR>, // SelectionDAG pointer support

  // Promote i8/i16 arguments to i32.
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,

  // Do not split arguments half on the stack, and half in registers.
  // Handle them specifically.
  CCIf<"ArgFlags.isSplit() || !State.getPendingLocs().empty()",
       CCCustom<"CC_AIE1_Handle_Split_Arg">>,

  // The first 4 integer arguments are passed in integer registers.
  // Note that there is no need to handle i64 here, as it has been split in 32
  // bits chunks based on TargetLowering::getNumRegistersForCallingConv().
  // At least that is true for SelectionDAG.
  CCIfType<[i32, f32, v8i1, v16i1], CCAssignToReg<[r6,r7,r8,r9]>>,

  // Vector arguments can be in registers
  CCIfType<[v16i8, v8i16, v4i32, v4f32], AssignToVEC128>,
  CCIfType<[v32i8, v16i16, v8i32, v8f32, v4i64], AssignToVEC256>,
  CCIfType<[v64i8, v32i16, v16i32, v16f32, v8i64], AssignToVEC512>,
  CCIfType<[v128i8, v64i16, v32i32, v32f32, v16i64], AssignToVEC1024>,
  CCIfType<[v8i48], AssignToACC384>,
  CCIfType<[v16i48], AssignToACC768>,

  CCIfType<[v2i32], CCCustom<"CC_AIE_Handle_V2I32_Arg">>,

  // Couldn't pass in registers, try on the stack
  CCDelegateTo<CC_AIE_Stack>,
]>;

def CSR_AIE1
    : CalleeSavedRegs<(add LR,r4,r10,r11,p6,p7)>;

// Needed for implementation of AIERegisterInfo::getNoPreservedMask()
def CSR_NoRegs : CalleeSavedRegs<(add)>;
