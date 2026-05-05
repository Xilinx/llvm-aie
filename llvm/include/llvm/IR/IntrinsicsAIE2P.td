//===- IntrinsicsAIE2P.td - Defines AIE2P intrinsics ------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//===----------------------------------------------------------------------===//
//
// This file defines all of the AIE2P-specific intrinsics.
//
//===----------------------------------------------------------------------===//

//aie2p
let TargetPrefix = "aie2p" in {

//===----------------------------------------------------------------------===//
class AIE2PEventIntrinsic
    : DefaultAttrsIntrinsic<[],
                [llvm_i32_ty],
                [IntrHasSideEffects, IntrNoMem]>; //[IntrHasSideEffects, IntrNoMem, ImmArg<ArgIndex<0>>]>;
class AIEV2PEXT_I32_I64
     : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_v2i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2PSET_I64_I32
     : DefaultAttrsIntrinsic<[llvm_v2i32_ty], [llvm_i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2PUPD_I64_I32
     : DefaultAttrsIntrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty,llvm_i32_ty,llvm_i32_ty], [IntrNoMem]>;
// bfloat16 conversions
class AIEV2PBF16ToInt : DefaultAttrsIntrinsic<[llvm_i32_ty],
      [llvm_bfloat_ty],
      [IntrNoMem]>;

// TM
class AIE2PReadTM : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_ptr_ty], [IntrArgMemOnly]>;
class AIE2PWriteTM : DefaultAttrsIntrinsic<[], [llvm_i32_ty, llvm_ptr_ty], [IntrWriteMem]>;
// FLT2FX, FX2FLT
class AIE2PFX2FLT : DefaultAttrsIntrinsic<[llvm_float_ty], [llvm_i32_ty, llvm_i32_ty], 
      [IntrNoMem]>;
class AIE2PFLT2FX : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_float_ty, llvm_i32_ty], 
      [IntrNoMem]>;
// non-linear float
class AIE2PNLFVec : DefaultAttrsIntrinsic<[llvm_v16bf16_ty], [llvm_v16f32_ty],
      [IntrNoMem]>;
class AIE2PNLF : DefaultAttrsIntrinsic<[llvm_float_ty], [llvm_float_ty],
      [IntrNoMem]>;
// DIVS
class AIE2PDIVS : DefaultAttrsIntrinsic<[llvm_i32_ty, llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;

//srs
class AIE2PI256V16Acc32I
	 : DefaultAttrsIntrinsic<[llvm_v16i16_ty], [llvm_v16i32_ty, llvm_i32_ty, llvm_i32_ty],
					 [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI256V16Acc64I
	 : DefaultAttrsIntrinsic<[llvm_v16i16_ty], [llvm_v16i64_ty, llvm_i32_ty, llvm_i32_ty],
					 [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI256V32Acc32I
	 : DefaultAttrsIntrinsic<[llvm_v32i8_ty], [llvm_v32i32_ty, llvm_i32_ty, llvm_i32_ty],
					 [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI256V8Acc64I
	 : DefaultAttrsIntrinsic<[llvm_v8i32_ty], [llvm_v8i64_ty, llvm_i32_ty, llvm_i32_ty],
					 [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI512V16Acc64I
	 : DefaultAttrsIntrinsic<[llvm_v16i32_ty], [llvm_v16i64_ty, llvm_i32_ty, llvm_i32_ty],
					 [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI512V32Acc32I
	 : DefaultAttrsIntrinsic<[llvm_v32i16_ty], [llvm_v32i32_ty, llvm_i32_ty, llvm_i32_ty],
					 [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI512V32Acc64I
	 : DefaultAttrsIntrinsic<[llvm_v32i16_ty], [llvm_v32i64_ty, llvm_i32_ty, llvm_i32_ty],
					 [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI512V64Acc32I
	 : DefaultAttrsIntrinsic<[llvm_v64i8_ty], [llvm_v64i32_ty, llvm_i32_ty, llvm_i32_ty],
					 [IntrReadMem, IntrInaccessibleMemOnly]>;

// UPS
class AIE2PAcc32V16I256I : DefaultAttrsIntrinsic<[llvm_v16i32_ty],
                              [llvm_v16i16_ty, llvm_i32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PAcc32V32I256I : DefaultAttrsIntrinsic<[llvm_v32i32_ty],
                              [llvm_v32i8_ty, llvm_i32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PAcc32V32I512I : DefaultAttrsIntrinsic<[llvm_v32i32_ty],
                              [llvm_v32i16_ty, llvm_i32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PAcc64V16I256I : DefaultAttrsIntrinsic<[llvm_v16i64_ty],
                              [llvm_v16i16_ty, llvm_i32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PAcc64V16I512I : DefaultAttrsIntrinsic<[llvm_v16i64_ty],
                              [llvm_v16i32_ty, llvm_i32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PAcc64V8I256I  : DefaultAttrsIntrinsic<[llvm_v8i64_ty],
                              [llvm_v8i32_ty, llvm_i32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PAcc32V64I512I  : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v64i8_ty, llvm_i32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PAcc64V32I512I  : DefaultAttrsIntrinsic<[llvm_v32i64_ty],
                              [llvm_v32i16_ty, llvm_i32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
// v16accfloat to v16bfloat16
class AIE2PV16AccFloatToV16Bf : DefaultAttrsIntrinsic<[llvm_v16bf16_ty],
                                          [llvm_v16f32_ty],
                                          [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PV16BfToV16AccFloat : DefaultAttrsIntrinsic<[llvm_v16f32_ty], [llvm_v16bf16_ty], [IntrNoMem]>;

// v32accfloat to v32bfloat16
class AIE2PV32AccFloatToV32Bf : DefaultAttrsIntrinsic<[llvm_v32bf16_ty],
                                          [llvm_v32f32_ty],
                                          [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PV32BfToV32AccFloat : DefaultAttrsIntrinsic<[llvm_v32f32_ty], [llvm_v32bf16_ty], [IntrNoMem]>;

// v16accfloat to v8float
class AIE2PV16AccFloatToV8Float : DefaultAttrsIntrinsic<[llvm_v8f32_ty],
                                          [llvm_v16f32_ty],
                                          [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PV8FloatToV16AccFloat : DefaultAttrsIntrinsic<[llvm_v16f32_ty], [llvm_v8f32_ty], [IntrNoMem]>;

// Floating-point accumulator to block floating-point vector register
class AIE2PV64AccFloatToV64bfp16 : DefaultAttrsIntrinsic<[llvm_v64i8_ty, llvm_v8i8_ty],
                                          [llvm_v64f32_ty],
                                          [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PV64bfp16ebs8ToV64bfp16ebs16 : DefaultAttrsIntrinsic<[llvm_v64i8_ty, llvm_v8i8_ty],
                                          [llvm_v64i8_ty, llvm_v8i8_ty],
                                          [IntrReadMem, IntrInaccessibleMemOnly]>;

// v32accfloat to v16float
class AIE2PV32AccFloatToV16Float : DefaultAttrsIntrinsic<[llvm_v16f32_ty],
                                          [llvm_v32f32_ty],
                                          [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PV16FloatToV32AccFloat : DefaultAttrsIntrinsic<[llvm_v32f32_ty], [llvm_v16f32_ty], [IntrNoMem]>;

// Pack-Unpack
class AIE2PPackI512I8I16 : DefaultAttrsIntrinsic<[llvm_v32i8_ty], [llvm_v32i16_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PPackI512I4I8 : DefaultAttrsIntrinsic<[llvm_v32i8_ty], [llvm_v64i8_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PUnPackI512I16I8 : DefaultAttrsIntrinsic<[llvm_v32i16_ty], [llvm_v32i8_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PUnPackI512I8I4 : DefaultAttrsIntrinsic<[llvm_v64i8_ty], [llvm_v32i8_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PPackI1024I8I16 : DefaultAttrsIntrinsic<[llvm_v64i8_ty], [llvm_v64i16_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PPackI1024I4I8 : DefaultAttrsIntrinsic<[llvm_v64i8_ty], [llvm_v128i8_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PUnPackI1024I16I8 : DefaultAttrsIntrinsic<[llvm_v64i16_ty], [llvm_v64i8_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PUnPackI1024I8I4 : DefaultAttrsIntrinsic<[llvm_v128i8_ty], [llvm_v64i8_ty, llvm_i32_ty], [IntrNoMem]>;
//Locks
def int_aie2p_event : ClangBuiltin<"__builtin_aie2p_event">, AIE2PEventIntrinsic;
// EXT
def int_aie2p_ext_I32_I64    : ClangBuiltin<"__builtin_aiev2p_ext_I32_I64">,    AIEV2PEXT_I32_I64;
// SET
def int_aie2p_set_I64_I32    : ClangBuiltin<"__builtin_aiev2p_set_I64_I32">,    AIEV2PSET_I64_I32;
// UPD
def int_aie2p_upd_I64_I32    : ClangBuiltin<"__builtin_aiev2p_upd_I64_I32">,    AIEV2PUPD_I64_I32;

// Pack-Unpack
def int_aie2p_pack_I512_I8_I16 : ClangBuiltin<"__builtin_aie2p_pack_I512_I8_I16">, AIE2PPackI512I8I16;
def int_aie2p_pack_I512_I4_I8 : ClangBuiltin<"__builtin_aie2p_pack_I512_I4_I8">, AIE2PPackI512I4I8;
def int_aie2p_unpack_I512_I16_I8 : ClangBuiltin<"__builtin_aie2p_unpack_I512_I16_I8">, AIE2PUnPackI512I16I8;
def int_aie2p_unpack_I512_I8_I4 : ClangBuiltin<"__builtin_aie2p_unpack_I512_I8_I4">, AIE2PUnPackI512I8I4;
def int_aie2p_pack_I1024_I8_I16 : ClangBuiltin<"__builtin_aie2p_pack_I1024_I8_I16">, AIE2PPackI1024I8I16;
def int_aie2p_pack_I1024_I4_I8 : ClangBuiltin<"__builtin_aie2p_pack_I1024_I4_I8">, AIE2PPackI1024I4I8;
def int_aie2p_unpack_I1024_I16_I8 : ClangBuiltin<"__builtin_aie2p_unpack_I1024_I16_I8">, AIE2PUnPackI1024I16I8;
def int_aie2p_unpack_I1024_I8_I4 : ClangBuiltin<"__builtin_aie2p_unpack_I1024_I8_I4">, AIE2PUnPackI1024I8I4;
// MAC-MUL
class AIE2PADDACC2048ADD : DefaultAttrsIntrinsic<[llvm_v32i64_ty], [llvm_v32i64_ty, llvm_v32i64_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PADDACC2048BFADD : DefaultAttrsIntrinsic<[llvm_v64f32_ty], [llvm_v64f32_ty, llvm_v64f32_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI1024I1024ACC2048ADDMAC : DefaultAttrsIntrinsic<[llvm_v32i64_ty], [llvm_v32i32_ty, llvm_v64i16_ty, llvm_v32i64_ty, llvm_v32i64_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PI1024I1024ACC2048BFADDMAC : DefaultAttrsIntrinsic<[llvm_v64f32_ty], [llvm_v64bf16_ty, llvm_v64bf16_ty, llvm_v64f32_ty, llvm_v64f32_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI1024I1024ACC2048BFMAC : DefaultAttrsIntrinsic<[llvm_v64f32_ty], [llvm_v64bf16_ty, llvm_v64bf16_ty, llvm_v64f32_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI1024I1024ACC2048BFMUL : DefaultAttrsIntrinsic<[llvm_v64f32_ty], [llvm_v64bf16_ty, llvm_v64bf16_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI1024I1024ACC2048MAC : DefaultAttrsIntrinsic<[llvm_v32i64_ty], [llvm_v32i32_ty, llvm_v64i16_ty, llvm_v32i64_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PI1024I1024ACC2048MUL : DefaultAttrsIntrinsic<[llvm_v32i64_ty], [llvm_v32i32_ty, llvm_v64i16_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PI1024I512ACC2048ADDMAC : DefaultAttrsIntrinsic<[llvm_v32i64_ty], [llvm_v32i32_ty, llvm_v32i16_ty, llvm_v32i64_ty, llvm_v32i64_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PI1024I512ACC2048MAC : DefaultAttrsIntrinsic<[llvm_v32i64_ty], [llvm_v32i32_ty, llvm_v32i16_ty, llvm_v32i64_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PI1024I512ACC2048MUL : DefaultAttrsIntrinsic<[llvm_v32i64_ty], [llvm_v32i32_ty, llvm_v32i16_ty, llvm_i32_ty],
[IntrNoMem]>;
class AIE2PI512I1024ACC2048ADDMAC : DefaultAttrsIntrinsic<[llvm_v32i64_ty], [llvm_v16i32_ty, llvm_v64i16_ty, llvm_v32i64_ty, llvm_v32i64_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PI512I1024ACC2048MAC : DefaultAttrsIntrinsic<[llvm_v32i64_ty], [llvm_v16i32_ty, llvm_v64i16_ty, llvm_v32i64_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PI512I1024ACC2048MUL : DefaultAttrsIntrinsic<[llvm_v32i64_ty], [llvm_v16i32_ty, llvm_v64i16_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PI512I512ACC1024ADDMAC : DefaultAttrsIntrinsic<[llvm_v16i64_ty], [llvm_v32i16_ty, llvm_v32i16_ty, llvm_v16i64_ty, llvm_v16i64_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PI512I512ACC1024BFADDMAC : DefaultAttrsIntrinsic<[llvm_v32f32_ty], [llvm_v32bf16_ty, llvm_v32bf16_ty, llvm_v32f32_ty, llvm_v32f32_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI512I512ACC1024BFMAC : DefaultAttrsIntrinsic<[llvm_v32f32_ty], [llvm_v32bf16_ty, llvm_v32bf16_ty, llvm_v32f32_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI512I512ACC1024BFMUL : DefaultAttrsIntrinsic<[llvm_v32f32_ty], [llvm_v32bf16_ty, llvm_v32bf16_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI512I512ACC1024MAC : DefaultAttrsIntrinsic<[llvm_v16i64_ty], [llvm_v32i16_ty, llvm_v32i16_ty, llvm_v16i64_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PI512I512ACC1024MUL : DefaultAttrsIntrinsic<[llvm_v16i64_ty], [llvm_v32i16_ty, llvm_v32i16_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PI512I512ACC2048ADDMAC : DefaultAttrsIntrinsic<[llvm_v32i64_ty], [llvm_v16i32_ty, llvm_v32i16_ty, llvm_v32i64_ty, llvm_v32i64_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PI512I512ACC2048BFADDMAC : DefaultAttrsIntrinsic<[llvm_v64f32_ty], [llvm_v32bf16_ty, llvm_v32bf16_ty, llvm_v64f32_ty, llvm_v64f32_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI512I512ACC2048BFMAC : DefaultAttrsIntrinsic<[llvm_v64f32_ty], [llvm_v32bf16_ty, llvm_v32bf16_ty, llvm_v64f32_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI512I512ACC2048BFMUL : DefaultAttrsIntrinsic<[llvm_v64f32_ty], [llvm_v32bf16_ty, llvm_v32bf16_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI512I512ACC2048MAC : DefaultAttrsIntrinsic<[llvm_v32i64_ty], [llvm_v16i32_ty, llvm_v32i16_ty, llvm_v32i64_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PI512I512ACC2048MUL : DefaultAttrsIntrinsic<[llvm_v32i64_ty], [llvm_v16i32_ty, llvm_v32i16_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PI512I512ACC512BFADDMAC : DefaultAttrsIntrinsic<[llvm_v16f32_ty], [llvm_v32bf16_ty, llvm_v32bf16_ty, llvm_v16f32_ty, llvm_v16f32_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI512I512ACC512BFMAC : DefaultAttrsIntrinsic<[llvm_v16f32_ty], [llvm_v32bf16_ty, llvm_v32bf16_ty, llvm_v16f32_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PI512I512ACC512BFMUL : DefaultAttrsIntrinsic<[llvm_v16f32_ty], [llvm_v32bf16_ty, llvm_v32bf16_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PNEGACC2048BFNEG : DefaultAttrsIntrinsic<[llvm_v64f32_ty], [llvm_v64f32_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PNEGACC2048NEG : DefaultAttrsIntrinsic<[llvm_v32i64_ty], [llvm_v32i64_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PSUBACC2048ADD : DefaultAttrsIntrinsic<[llvm_v32i64_ty], [llvm_v32i64_ty, llvm_v32i64_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PSUBACC2048BFADD : DefaultAttrsIntrinsic<[llvm_v64f32_ty], [llvm_v64f32_ty, llvm_v64f32_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;

// Mode Settings
// Set Control Reg
class AIE2PSetCtrlReg : DefaultAttrsIntrinsic<[], [llvm_i32_ty, llvm_i32_ty], [IntrWriteMem, IntrInaccessibleMemOnly]>;
// Get Control Reg
class AIE2PGetCtrlReg : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;

// Set Status Reg
class AIE2PSetStatusReg : DefaultAttrsIntrinsic<[], [llvm_i32_ty, llvm_i32_ty], [IntrWriteMem, IntrInaccessibleMemOnly]>;
// Get Status Reg
class AIE2PGetStatusReg : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
//Semaphores
class AIE2PLockIntr
     : DefaultAttrsIntrinsic<[],
                 [llvm_i32_ty /* unsigned */, llvm_i32_ty /* (un)signed */],
                 [IntrHasSideEffects, IntrNoMem]>;
class AIE2PLockCondIntr
     : DefaultAttrsIntrinsic<[],
                 [llvm_i32_ty /* unsigned */, llvm_i32_ty /* (un)signed */, llvm_i32_ty /* (un)signed */],
                 [IntrHasSideEffects, IntrNoMem]>;
class AIE2PDoneIntr
     : DefaultAttrsIntrinsic<[], [], [IntrHasSideEffects, IntrNoMem]>;
// Scheduling barrier
class AIE2PSchedBarrier : DefaultAttrsIntrinsic<[], [], [IntrNoMem, IntrHasSideEffects, IntrConvergent, IntrWillReturn]>;
// Get CoreID
class AIE2PGetCoreID : DefaultAttrsIntrinsic<[llvm_i32_ty], [], [IntrNoMem]>;

// CLB
class AIE2PCLB : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_i32_ty], [IntrNoMem]>;

// VSHIFT
class AIE2PVShift_I512_I512
     : DefaultAttrsIntrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty,llvm_v16i32_ty,llvm_i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIE2PVShift_bf512_bf512
     : DefaultAttrsIntrinsic<[llvm_v32bf16_ty], [llvm_v32bf16_ty,llvm_v32bf16_ty,llvm_i32_ty,llvm_i32_ty], [IntrNoMem]>;
// VINSERT.X
class AIE2PVInsert32_bf512
     : DefaultAttrsIntrinsic<[llvm_v32bf16_ty], [llvm_v32bf16_ty,llvm_i32_ty,llvm_v2bf16_ty], [IntrNoMem]>;
class AIE2PVInsert64_bf512
     : DefaultAttrsIntrinsic<[llvm_v32bf16_ty], [llvm_v32bf16_ty,llvm_i32_ty,llvm_v4bf16_ty], [IntrNoMem]>;
class AIE2PVInsert32_accfloat
     : DefaultAttrsIntrinsic<[llvm_v8i64_ty], [llvm_v8i64_ty,llvm_i32_ty,llvm_float_ty], [IntrNoMem]>;

// VBCST.X
class AIE2PVBCST128I512
     : DefaultAttrsIntrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PVBCST32bf512
     : DefaultAttrsIntrinsic<[llvm_v32bf16_ty], [llvm_v2bf16_ty], [IntrNoMem]>;
class AIE2PVBCST64bf512
     : DefaultAttrsIntrinsic<[llvm_v32bf16_ty], [llvm_v4bf16_ty], [IntrNoMem]>;
class AIE2PVBCSTF32I512
     : DefaultAttrsIntrinsic<[llvm_v16f32_ty], [llvm_float_ty], [IntrNoMem]>;
class AIE2PVCLRACC1024 : DefaultAttrsIntrinsic<[llvm_v16i64_ty], [], [IntrNoMem]>;
// VEXTBCST.X
class AIE2PVEXTBCST32Bf512
     : DefaultAttrsIntrinsic<[llvm_v32bf16_ty], [llvm_v32bf16_ty, llvm_i32_ty], [IntrNoMem]>;
// VBCSTSHFL
class AIE2PVBcstShfl_I32 : DefaultAttrsIntrinsic<[llvm_v16i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PVBcstShfl_I64 : DefaultAttrsIntrinsic<[llvm_v16i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
// VSHUFFLE
class AIE2PVShuffle : DefaultAttrsIntrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty, llvm_i32_ty], [IntrNoMem]>;

// PADDX.XX
class AIE2PAdd2D : DefaultAttrsIntrinsic<[llvm_ptr_ty, llvm_i20_ty],
      [llvm_ptr_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty],
      [IntrNoMem]>;
class AIE2PAdd3D : DefaultAttrsIntrinsic<[llvm_ptr_ty, llvm_i20_ty, llvm_i20_ty],
      [llvm_ptr_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty],
      [IntrNoMem]>;

// Addressing intrinsics
def int_aie2p_add_2d : AIE2PAdd2D;
def int_aie2p_add_3d : AIE2PAdd3D;

// LOAD_4X
class AIE2PLOAD4X : DefaultAttrsIntrinsic<[llvm_v8i32_ty], [llvm_v8i32_ty],
      [IntrReadMem]>;

def int_aie2p_load_4x16_lo : ClangBuiltin<"__builtin_aie2p_load_4x16_lo">, AIE2PLOAD4X;
def int_aie2p_load_4x16_hi : ClangBuiltin<"__builtin_aie2p_load_4x16_hi">, AIE2PLOAD4X;
def int_aie2p_load_4x32_lo : ClangBuiltin<"__builtin_aie2p_load_4x32_lo">, AIE2PLOAD4X;
def int_aie2p_load_4x32_hi : ClangBuiltin<"__builtin_aie2p_load_4x32_hi">, AIE2PLOAD4X;
def int_aie2p_load_4x64_lo : ClangBuiltin<"__builtin_aie2p_load_4x64_lo">, AIE2PLOAD4X;
def int_aie2p_load_4x64_hi : ClangBuiltin<"__builtin_aie2p_load_4x64_hi">, AIE2PLOAD4X;

class AIE2PV64I8V2I32V64I8I32 : DefaultAttrsIntrinsic<[llvm_v64i8_ty, llvm_v2i32_ty], [llvm_v64i8_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PV32I16I32V32I16I32 : DefaultAttrsIntrinsic<[llvm_v32i16_ty, llvm_i32_ty], [llvm_v32i16_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PV16I32I32V16I32I32 : DefaultAttrsIntrinsic<[llvm_v16i32_ty, llvm_i32_ty], [llvm_v16i32_ty, llvm_i32_ty], [IntrNoMem]>;

class AIE2PV64I8V64I8V64I8I64 : DefaultAttrsIntrinsic<[llvm_v64i8_ty], [llvm_v64i8_ty, llvm_v64i8_ty, llvm_v2i32_ty], [IntrNoMem]>;
class AIE2PV32I16V32I16V32I16I32 : DefaultAttrsIntrinsic<[llvm_v32i16_ty], [llvm_v32i16_ty, llvm_v32i16_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PV16I32V16I32V16I32I32 : DefaultAttrsIntrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty, llvm_i32_ty], [IntrNoMem]>;

class AIE2PV64I8V2I32V64I8 : DefaultAttrsIntrinsic<[llvm_v64i8_ty, llvm_v2i32_ty], [llvm_v64i8_ty], [IntrNoMem]>;
class AIE2PV32I16I32V32I16 : DefaultAttrsIntrinsic<[llvm_v32i16_ty, llvm_i32_ty], [llvm_v32i16_ty], [IntrNoMem]>;
class AIE2PV16I32I32V16I32 : DefaultAttrsIntrinsic<[llvm_v16i32_ty, llvm_i32_ty], [llvm_v16i32_ty], [IntrNoMem]>;

class AIE2PV2I32V64I8 : DefaultAttrsIntrinsic<[llvm_v2i32_ty], [llvm_v64i8_ty], [IntrNoMem]>;
class AIE2PI32V32I16 : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_v32i16_ty], [IntrNoMem]>;
class AIE2PI32V16I32 : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_v16i32_ty], [IntrNoMem]>;

class AIE2PV2I32V64I8V64I8I32 : DefaultAttrsIntrinsic<[llvm_v2i32_ty], [llvm_v64i8_ty, llvm_v64i8_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PI32V32I16V32I16I32 : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_v32i16_ty, llvm_v32i16_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PI32V16I32V16I32I32 : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PI32V32BF16V32BF16 : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_v32bf16_ty, llvm_v32bf16_ty], [IntrNoMem]>;

class AIE2PV64I8I64V64I8V64I8I32 : DefaultAttrsIntrinsic<[llvm_v64i8_ty, llvm_v2i32_ty], [llvm_v64i8_ty, llvm_v64i8_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PV32I16I32V32I16V32I16I32 : DefaultAttrsIntrinsic<[llvm_v32i16_ty, llvm_i32_ty], [llvm_v32i16_ty, llvm_v32i16_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PV16I32I32V16I32V16I32I32 : DefaultAttrsIntrinsic<[llvm_v16i32_ty, llvm_i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PV32BF16I32V32BF16V32BF16I32 : DefaultAttrsIntrinsic<[llvm_v32bf16_ty, llvm_i32_ty], [llvm_v32bf16_ty, llvm_v32bf16_ty], [IntrNoMem]>;

// bfloat16 conversions
class AIE2PV16BF16ToV16I : DefaultAttrsIntrinsic<[llvm_v16i32_ty],
      [llvm_v16bf16_ty, llvm_i32_ty],
      [IntrReadMem, IntrInaccessibleMemOnly]>;

def int_aie2p_ACC2048_add_conf : ClangBuiltin<"__builtin_aie2p_ACC2048_add_conf">, AIE2PADDACC2048ADD;
def int_aie2p_ACC2048_accfloat_add_conf : ClangBuiltin<"__builtin_aie2p_ACC2048_accfloat_add_conf">, AIE2PADDACC2048BFADD;
def int_aie2p_I1024_I1024_ACC2048_addmac_conf : ClangBuiltin<"__builtin_aie2p_I1024_I1024_ACC2048_addmac_conf">, AIE2PI1024I1024ACC2048ADDMAC;
def int_aie2p_I1024_I1024_ACC2048_bf_addmac_conf : ClangBuiltin<"__builtin_aie2p_I1024_I1024_ACC2048_bf_addmac_conf">, AIE2PI1024I1024ACC2048BFADDMAC;
def int_aie2p_I1024_I1024_ACC2048_bf_mac_conf : ClangBuiltin<"__builtin_aie2p_I1024_I1024_ACC2048_bf_mac_conf">, AIE2PI1024I1024ACC2048BFMAC;
def int_aie2p_I1024_I1024_ACC2048_bf_mul_conf : ClangBuiltin<"__builtin_aie2p_I1024_I1024_ACC2048_bf_mul_conf">, AIE2PI1024I1024ACC2048BFMUL;
def int_aie2p_I1024_I1024_ACC2048_mac_conf : ClangBuiltin<"__builtin_aie2p_I1024_I1024_ACC2048_mac_conf">, AIE2PI1024I1024ACC2048MAC;
def int_aie2p_I1024_I1024_ACC2048_mul_conf : ClangBuiltin<"__builtin_aie2p_I1024_I1024_ACC2048_mul_conf">, AIE2PI1024I1024ACC2048MUL;
def int_aie2p_I1024_I512_ACC2048_addmac_conf : ClangBuiltin<"__builtin_aie2p_I1024_I512_ACC2048_addmac_conf">, AIE2PI1024I512ACC2048ADDMAC;
def int_aie2p_I1024_I512_ACC2048_mac_conf : ClangBuiltin<"__builtin_aie2p_I1024_I512_ACC2048_mac_conf">, AIE2PI1024I512ACC2048MAC;
def int_aie2p_I1024_I512_ACC2048_mul_conf : ClangBuiltin<"__builtin_aie2p_I1024_I512_ACC2048_mul_conf">, AIE2PI1024I512ACC2048MUL;
def int_aie2p_I512_I1024_ACC2048_addmac_conf : ClangBuiltin<"__builtin_aie2p_I512_I1024_ACC2048_addmac_conf">, AIE2PI512I1024ACC2048ADDMAC;
def int_aie2p_I512_I1024_ACC2048_mac_conf : ClangBuiltin<"__builtin_aie2p_I512_I1024_ACC2048_mac_conf">, AIE2PI512I1024ACC2048MAC;
def int_aie2p_I512_I1024_ACC2048_mul_conf : ClangBuiltin<"__builtin_aie2p_I512_I1024_ACC2048_mul_conf">, AIE2PI512I1024ACC2048MUL;
def int_aie2p_I512_I512_ACC1024_addmac_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC1024_addmac_conf">, AIE2PI512I512ACC1024ADDMAC;
def int_aie2p_I512_I512_ACC1024_bf_addmac_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC1024_bf_addmac_conf">, AIE2PI512I512ACC1024BFADDMAC;
def int_aie2p_I512_I512_ACC1024_bf_mac_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC1024_bf_mac_conf">, AIE2PI512I512ACC1024BFMAC;
def int_aie2p_I512_I512_ACC1024_bf_mul_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC1024_bf_mul_conf">, AIE2PI512I512ACC1024BFMUL;
def int_aie2p_I512_I512_ACC1024_mac_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC1024_mac_conf">, AIE2PI512I512ACC1024MAC;
def int_aie2p_I512_I512_ACC1024_mul_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC1024_mul_conf">, AIE2PI512I512ACC1024MUL;
def int_aie2p_I512_I512_ACC2048_addmac_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC2048_addmac_conf">, AIE2PI512I512ACC2048ADDMAC;
def int_aie2p_I512_I512_ACC2048_bf_addmac_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC2048_bf_addmac_conf">, AIE2PI512I512ACC2048BFADDMAC;
def int_aie2p_I512_I512_ACC2048_bf_mac_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC2048_bf_mac_conf">, AIE2PI512I512ACC2048BFMAC;
def int_aie2p_I512_I512_ACC2048_bf_mul_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC2048_bf_mul_conf">, AIE2PI512I512ACC2048BFMUL;
def int_aie2p_I512_I512_ACC2048_mac_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC2048_mac_conf">, AIE2PI512I512ACC2048MAC;
def int_aie2p_I512_I512_ACC2048_mul_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC2048_mul_conf">, AIE2PI512I512ACC2048MUL;
def int_aie2p_I512_I512_ACC512_bf_addmac_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC512_bf_addmac_conf">, AIE2PI512I512ACC512BFADDMAC;
def int_aie2p_I512_I512_ACC512_bf_mac_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC512_bf_mac_conf">, AIE2PI512I512ACC512BFMAC;
def int_aie2p_I512_I512_ACC512_bf_mul_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC512_bf_mul_conf">, AIE2PI512I512ACC512BFMUL;
def int_aie2p_ACC2048_accfloat_neg_conf : ClangBuiltin<"__builtin_aie2p_ACC2048_accfloat_neg_conf">, AIE2PNEGACC2048BFNEG;
def int_aie2p_ACC2048_neg_conf : ClangBuiltin<"__builtin_aie2p_ACC2048_neg_conf">, AIE2PNEGACC2048NEG;
def int_aie2p_ACC2048_sub_conf : ClangBuiltin<"__builtin_aie2p_ACC2048_sub_conf">, AIE2PSUBACC2048ADD;
def int_aie2p_ACC2048_accfloat_sub_conf : ClangBuiltin<"__builtin_aie2p_ACC2048_accfloat_sub_conf">, AIE2PSUBACC2048BFADD;
def int_aie2p_I1024_I1024_ACC2048_addmsc_conf : ClangBuiltin<"__builtin_aie2p_I1024_I1024_ACC2048_addmsc_conf">, AIE2PI1024I1024ACC2048ADDMAC;
def int_aie2p_I1024_I1024_ACC2048_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2p_I1024_I1024_ACC2048_bf_addmsc_conf">, AIE2PI1024I1024ACC2048BFADDMAC;
def int_aie2p_I1024_I1024_ACC2048_bf_msc_conf : ClangBuiltin<"__builtin_aie2p_I1024_I1024_ACC2048_bf_msc_conf">, AIE2PI1024I1024ACC2048BFMAC;
def int_aie2p_I1024_I1024_ACC2048_bf_negmul_conf : ClangBuiltin<"__builtin_aie2p_I1024_I1024_ACC2048_bf_negmul_conf">, AIE2PI1024I1024ACC2048BFMUL;
def int_aie2p_I1024_I1024_ACC2048_msc_conf : ClangBuiltin<"__builtin_aie2p_I1024_I1024_ACC2048_msc_conf">, AIE2PI1024I1024ACC2048MAC;
def int_aie2p_I1024_I1024_ACC2048_negmul_conf : ClangBuiltin<"__builtin_aie2p_I1024_I1024_ACC2048_negmul_conf">, AIE2PI1024I1024ACC2048MUL;
def int_aie2p_I1024_I512_ACC2048_addmsc_conf : ClangBuiltin<"__builtin_aie2p_I1024_I512_ACC2048_addmsc_conf">, AIE2PI1024I512ACC2048ADDMAC;
def int_aie2p_I1024_I512_ACC2048_msc_conf : ClangBuiltin<"__builtin_aie2p_I1024_I512_ACC2048_msc_conf">, AIE2PI1024I512ACC2048MAC;
def int_aie2p_I1024_I512_ACC2048_negmul_conf : ClangBuiltin<"__builtin_aie2p_I1024_I512_ACC2048_negmul_conf">, AIE2PI1024I512ACC2048MUL;
def int_aie2p_I512_I1024_ACC2048_addmsc_conf : ClangBuiltin<"__builtin_aie2p_I512_I1024_ACC2048_addmsc_conf">, AIE2PI512I1024ACC2048ADDMAC;
def int_aie2p_I512_I1024_ACC2048_msc_conf : ClangBuiltin<"__builtin_aie2p_I512_I1024_ACC2048_msc_conf">, AIE2PI512I1024ACC2048MAC;
def int_aie2p_I512_I1024_ACC2048_negmul_conf : ClangBuiltin<"__builtin_aie2p_I512_I1024_ACC2048_negmul_conf">, AIE2PI512I1024ACC2048MUL;
def int_aie2p_I512_I512_ACC1024_addmsc_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC1024_addmsc_conf">, AIE2PI512I512ACC1024ADDMAC;
def int_aie2p_I512_I512_ACC1024_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC1024_bf_addmsc_conf">, AIE2PI512I512ACC1024BFADDMAC;
def int_aie2p_I512_I512_ACC1024_bf_msc_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC1024_bf_msc_conf">, AIE2PI512I512ACC1024BFMAC;
def int_aie2p_I512_I512_ACC1024_bf_negmul_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC1024_bf_negmul_conf">, AIE2PI512I512ACC1024BFMUL;
def int_aie2p_I512_I512_ACC1024_msc_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC1024_msc_conf">, AIE2PI512I512ACC1024MAC;
def int_aie2p_I512_I512_ACC1024_negmul_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC1024_negmul_conf">, AIE2PI512I512ACC1024MUL;
def int_aie2p_I512_I512_ACC2048_addmsc_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC2048_addmsc_conf">, AIE2PI512I512ACC2048ADDMAC;
def int_aie2p_I512_I512_ACC2048_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC2048_bf_addmsc_conf">, AIE2PI512I512ACC2048BFADDMAC;
def int_aie2p_I512_I512_ACC2048_bf_msc_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC2048_bf_msc_conf">, AIE2PI512I512ACC2048BFMAC;
def int_aie2p_I512_I512_ACC2048_bf_negmul_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC2048_bf_negmul_conf">, AIE2PI512I512ACC2048BFMUL;
def int_aie2p_I512_I512_ACC2048_msc_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC2048_msc_conf">, AIE2PI512I512ACC2048MAC;
def int_aie2p_I512_I512_ACC2048_negmul_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC2048_negmul_conf">, AIE2PI512I512ACC2048MUL;
def int_aie2p_I512_I512_ACC512_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC512_bf_addmsc_conf">, AIE2PI512I512ACC512BFADDMAC;
def int_aie2p_I512_I512_ACC512_bf_msc_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC512_bf_msc_conf">, AIE2PI512I512ACC512BFMAC;
def int_aie2p_I512_I512_ACC512_bf_negmul_conf : ClangBuiltin<"__builtin_aie2p_I512_I512_ACC512_bf_negmul_conf">, AIE2PI512I512ACC512BFMUL;

// BFP16 MAC MUL
class AIE2PMUL_BFP16V64BFP16
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty], [llvm_v64i8_ty, llvm_v8i8_ty, llvm_v64i8_ty, llvm_v8i8_ty, llvm_i32_ty],
					 [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PMAC_BFP16V64BFP16
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty], [llvm_v64i8_ty, llvm_v8i8_ty, llvm_v64i8_ty, llvm_v8i8_ty, llvm_v64i32_ty, llvm_i32_ty],
					 [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PADDMAC_BFP16V64BFP16
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty], [llvm_v64i8_ty, llvm_v8i8_ty, llvm_v64i8_ty, llvm_v8i8_ty, llvm_v64i32_ty, llvm_v64i32_ty, llvm_i32_ty],
					 [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PMUL_BFP16V128BFP16
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty], [llvm_v64i8_ty, llvm_v8i8_ty, llvm_v64i8_ty , llvm_v64i8_ty, llvm_v8i8_ty, llvm_v8i8_ty, llvm_i32_ty],
					 [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PMAC_BFP16V128BFP16
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty], [llvm_v64i8_ty, llvm_v8i8_ty, llvm_v64i8_ty , llvm_v64i8_ty, llvm_v8i8_ty, llvm_v8i8_ty, llvm_v64i32_ty, llvm_i32_ty],
					 [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PADDMAC_BFP16V128BFP16
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty], [llvm_v64i8_ty, llvm_v8i8_ty, llvm_v64i8_ty , llvm_v64i8_ty, llvm_v8i8_ty, llvm_v8i8_ty, llvm_v64i32_ty, llvm_v64i32_ty, llvm_i32_ty],
					 [IntrReadMem, IntrInaccessibleMemOnly]>;

def int_aie2p_BFP576_BFP576_ACC2048_mul_conf : ClangBuiltin<"__builtin_aie2p_BFP576_BFP576_ACC2048_mul_conf">, AIE2PMUL_BFP16V64BFP16;
def int_aie2p_BFP576_BFP576_ACC2048_negmul_conf : ClangBuiltin<"__builtin_aie2p_BFP576_BFP576_ACC2048_negmul_conf">, AIE2PMUL_BFP16V64BFP16;
def int_aie2p_BFP576_BFP576_ACC2048_mac_conf : ClangBuiltin<"__builtin_aie2p_BFP576_BFP576_ACC2048_mac_conf">, AIE2PMAC_BFP16V64BFP16;
def int_aie2p_BFP576_BFP576_ACC2048_msc_conf : ClangBuiltin<"__builtin_aie2p_BFP576_BFP576_ACC2048_msc_conf">, AIE2PMAC_BFP16V64BFP16;
def int_aie2p_BFP576_BFP576_ACC2048_addmac_conf : ClangBuiltin<"__builtin_aie2p_BFP576_BFP576_ACC2048_addmac_conf">, AIE2PADDMAC_BFP16V64BFP16;
def int_aie2p_BFP576_BFP576_ACC2048_addmsc_conf : ClangBuiltin<"__builtin_aie2p_BFP576_BFP576_ACC2048_addmsc_conf">, AIE2PADDMAC_BFP16V64BFP16;
def int_aie2p_BFP576_BFP1152_ACC2048_mul_conf : ClangBuiltin<"__builtin_aie2p_BFP576_BFP1152_ACC2048_mul_conf">, AIE2PMUL_BFP16V128BFP16;
def int_aie2p_BFP576_BFP1152_ACC2048_negmul_conf : ClangBuiltin<"__builtin_aie2p_BFP576_BFP1152_ACC2048_negmul_conf">, AIE2PMUL_BFP16V128BFP16;
def int_aie2p_BFP576_BFP1152_ACC2048_mac_conf : ClangBuiltin<"__builtin_aie2p_BFP576_BFP1152_ACC2048_mac_conf">, AIE2PMAC_BFP16V128BFP16;
def int_aie2p_BFP576_BFP1152_ACC2048_msc_conf : ClangBuiltin<"__builtin_aie2p_BFP576_BFP1152_ACC2048_msc_conf">, AIE2PMAC_BFP16V128BFP16;
def int_aie2p_BFP576_BFP1152_ACC2048_addmac_conf : ClangBuiltin<"__builtin_aie2p_BFP576_BFP1152_ACC2048_addmac_conf">, AIE2PADDMAC_BFP16V128BFP16;
def int_aie2p_BFP576_BFP1152_ACC2048_addmsc_conf : ClangBuiltin<"__builtin_aie2p_BFP576_BFP1152_ACC2048_addmsc_conf">, AIE2PADDMAC_BFP16V128BFP16;

//srs
def int_aie2p_I256_v16_acc32_srs : ClangBuiltin<"__builtin_aie2p_I256_v16_acc32_srs">, AIE2PI256V16Acc32I;
def int_aie2p_I256_v16_acc64_srs : ClangBuiltin<"__builtin_aie2p_I256_v16_acc64_srs">, AIE2PI256V16Acc64I;
def int_aie2p_I256_v32_acc32_srs : ClangBuiltin<"__builtin_aie2p_I256_v32_acc32_srs">, AIE2PI256V32Acc32I;
def int_aie2p_I256_v8_acc64_srs  : ClangBuiltin<"__builtin_aie2p_I256_v8_acc64_srs">, AIE2PI256V8Acc64I;
def int_aie2p_I512_v16_acc64_srs : ClangBuiltin<"__builtin_aie2p_I512_v16_acc64_srs">, AIE2PI512V16Acc64I;
def int_aie2p_I512_v32_acc32_srs : ClangBuiltin<"__builtin_aie2p_I512_v32_acc32_srs">, AIE2PI512V32Acc32I;
def int_aie2p_I512_v32_acc64_srs : ClangBuiltin<"__builtin_aie2p_I512_v32_acc64_srs">, AIE2PI512V32Acc64I;
def int_aie2p_I512_v64_acc32_srs : ClangBuiltin<"__builtin_aie2p_I512_v64_acc32_srs">, AIE2PI512V64Acc32I;

//ups
def int_aie2p_acc32_v16_I256_ups : ClangBuiltin<"__builtin_aie2p_acc32_v16_I256_ups">,  AIE2PAcc32V16I256I;
def int_aie2p_acc32_v32_I256_ups : ClangBuiltin<"__builtin_aie2p_acc32_v32_I256_ups">,  AIE2PAcc32V32I256I;
def int_aie2p_acc32_v32_I512_ups : ClangBuiltin<"__builtin_aie2p_acc32_v32_I512_ups">,  AIE2PAcc32V32I512I;
def int_aie2p_acc64_v16_I256_ups : ClangBuiltin<"__builtin_aie2p_acc64_v16_I256_ups">,  AIE2PAcc64V16I256I;
def int_aie2p_acc64_v16_I512_ups : ClangBuiltin<"__builtin_aie2p_acc64_v16_I512_ups">,  AIE2PAcc64V16I512I;
def int_aie2p_acc64_v8_I256_ups  : ClangBuiltin<"__builtin_aie2p_acc64_v8_I256_ups">,   AIE2PAcc64V8I256I;
def int_aie2p_acc32_v64_I512_ups : ClangBuiltin<"__builtin_aie2p_acc32_v64_I512_ups">,  AIE2PAcc32V64I512I;
def int_aie2p_acc64_v32_I512_ups : ClangBuiltin<"__builtin_aie2p_acc64_v32_I512_ups">,  AIE2PAcc64V32I512I;

// v16accfloat to v16bfloat16
def int_aie2p_v16accfloat_to_v16bf16 : ClangBuiltin<"__builtin_aie2p_v16accfloat_to_v16bf16">, AIE2PV16AccFloatToV16Bf;
def int_aie2p_v16bf16_to_v16accfloat : ClangBuiltin<"__builtin_aie2p_v16bf16_to_v16accfloat">, AIE2PV16BfToV16AccFloat;

// v32accfloat to v32bfloat16
def int_aie2p_v32accfloat_to_v32bf16 : ClangBuiltin<"__builtin_aie2p_v32accfloat_to_v32bf16">, AIE2PV32AccFloatToV32Bf;
def int_aie2p_v32bf16_to_v32accfloat : ClangBuiltin<"__builtin_aie2p_v32bf16_to_v32accfloat">, AIE2PV32BfToV32AccFloat;

// v16accfloat to v8float
def int_aie2p_v16accfloat_to_v8float : ClangBuiltin<"__builtin_aie2p_v16accfloat_to_v8float">, AIE2PV16AccFloatToV8Float;
def int_aie2p_v8float_to_v16accfloat : ClangBuiltin<"__builtin_aie2p_v8float_to_v16accfloat">, AIE2PV8FloatToV16AccFloat;

// Floating-point accumulator to block floating-point vector register
def int_aie2p_v64accfloat_to_v64bfp16ebs8 : AIE2PV64AccFloatToV64bfp16;
def int_aie2p_v64accfloat_to_v64bfp16ebs16 : AIE2PV64AccFloatToV64bfp16;
def int_aie2p_v64bfp16ebs8_to_v64bfp16ebs16 : AIE2PV64bfp16ebs8ToV64bfp16ebs16;

// v32accfloat to v16float
def int_aie2p_v32accfloat_to_v16float : ClangBuiltin<"__builtin_aie2p_v32accfloat_to_v16float">, AIE2PV32AccFloatToV16Float;
def int_aie2p_v16float_to_v32accfloat : ClangBuiltin<"__builtin_aie2p_v16float_to_v32accfloat">, AIE2PV16FloatToV32AccFloat;

// Mode Settings
// Set Control Registers
def int_aie2p_set_ctrl_reg : ClangBuiltin<"__builtin_aie2p_set_ctrl_reg">, AIE2PSetCtrlReg;
// Get Control Registers
def int_aie2p_get_ctrl_reg : ClangBuiltin<"__builtin_aie2p_get_ctrl_reg">, AIE2PGetCtrlReg;

// Set Status Registers
def int_aie2p_set_status_reg : ClangBuiltin<"__builtin_aie2p_set_status_reg">, AIE2PSetStatusReg;
// Get Status Registers
def int_aie2p_get_status_reg : ClangBuiltin<"__builtin_aie2p_get_status_reg">, AIE2PGetStatusReg;

// Get Core ID
def int_aie2p_get_coreid : ClangBuiltin<"__builtin_aie2p_get_coreid">, AIE2PGetCoreID;

// CLB
def int_aie2p_clb : ClangBuiltin<"__builtin_aie2p_clb">, AIE2PCLB;

//Locks
def int_aie2p_acquire :
     ClangBuiltin<"__builtin_aie2p_acquire">, AIE2PLockIntr;
def int_aie2p_acquire_cond :
     ClangBuiltin<"__builtin_aie2p_acquire_cond">, AIE2PLockCondIntr;
def int_aie2p_release :
     ClangBuiltin<"__builtin_aie2p_release">, AIE2PLockIntr;
def int_aie2p_release_cond :
     ClangBuiltin<"__builtin_aie2p_release_cond">, AIE2PLockCondIntr;
def int_aie2p_done :
    ClangBuiltin<"__builtin_aie2p_done">, AIE2PDoneIntr;

// Scheduling barrier
def int_aie2p_sched_barrier : ClangBuiltin<"__builtin_aie2p_sched_barrier">, AIE2PSchedBarrier;

//shuffle
def int_aie2p_vshuffle : ClangBuiltin<"__builtin_aie2p_vshuffle">, AIE2PVShuffle;

// vabs_gtz
def int_aie2p_vabs_gtz8  : AIE2PV64I8V2I32V64I8I32;
def int_aie2p_vabs_gtz16 : AIE2PV32I16I32V32I16I32;
def int_aie2p_vabs_gtz32 : AIE2PV16I32I32V16I32I32;

// vaddsub
def int_aie2p_vaddsub8  : ClangBuiltin<"__builtin_aie2p_vaddsub8">, AIE2PV64I8V64I8V64I8I64;
def int_aie2p_vaddsub16 : ClangBuiltin<"__builtin_aie2p_vaddsub16">, AIE2PV32I16V32I16V32I16I32;
def int_aie2p_vaddsub32 : ClangBuiltin<"__builtin_aie2p_vaddsub32">, AIE2PV16I32V16I32V16I32I32;

// vbneg_ltz
def int_aie2p_vbneg_ltz8  : AIE2PV64I8V2I32V64I8;
def int_aie2p_vbneg_ltz16 : AIE2PV32I16I32V32I16;
def int_aie2p_vbneg_ltz32 : AIE2PV16I32I32V16I32;

// veqz
def int_aie2p_veqz8  : ClangBuiltin<"__builtin_aie2p_veqz8">, AIE2PV2I32V64I8;
def int_aie2p_veqz16 : ClangBuiltin<"__builtin_aie2p_veqz16">, AIE2PI32V32I16;
def int_aie2p_veqz32 : ClangBuiltin<"__builtin_aie2p_veqz32">, AIE2PI32V16I32;

// vge
def int_aie2p_vge8  : ClangBuiltin<"__builtin_aie2p_vge8">, AIE2PV2I32V64I8V64I8I32;
def int_aie2p_vge16 : ClangBuiltin<"__builtin_aie2p_vge16">, AIE2PI32V32I16V32I16I32;
def int_aie2p_vge32 : ClangBuiltin<"__builtin_aie2p_vge32">, AIE2PI32V16I32V16I32I32;
def int_aie2p_vgebf16 : ClangBuiltin<"__builtin_aie2p_vgebf16">, AIE2PI32V32BF16V32BF16;

// vlt
def int_aie2p_vlt8  : ClangBuiltin<"__builtin_aie2p_vlt8">, AIE2PV2I32V64I8V64I8I32;
def int_aie2p_vlt16 : ClangBuiltin<"__builtin_aie2p_vlt16">, AIE2PI32V32I16V32I16I32;
def int_aie2p_vlt32 : ClangBuiltin<"__builtin_aie2p_vlt32">, AIE2PI32V16I32V16I32I32;
def int_aie2p_vltbf16 : ClangBuiltin<"__builtin_aie2p_vltbf16">, AIE2PI32V32BF16V32BF16;

// vmaxdiff_lt
def int_aie2p_vmaxdiff_lt8  : AIE2PV64I8I64V64I8V64I8I32;
def int_aie2p_vmaxdiff_lt16 : AIE2PV32I16I32V32I16V32I16I32;
def int_aie2p_vmaxdiff_lt32 : AIE2PV16I32I32V16I32V16I32I32;

// vmax_lt
def int_aie2p_vmax_lt8  : AIE2PV64I8I64V64I8V64I8I32;
def int_aie2p_vmax_lt16 : AIE2PV32I16I32V32I16V32I16I32;
def int_aie2p_vmax_lt32 : AIE2PV16I32I32V16I32V16I32I32;
def int_aie2p_vmax_ltbf16  : AIE2PV32BF16I32V32BF16V32BF16I32;

// vmin_ge
def int_aie2p_vmin_ge8  : AIE2PV64I8I64V64I8V64I8I32;
def int_aie2p_vmin_ge16 : AIE2PV32I16I32V32I16V32I16I32;
def int_aie2p_vmin_ge32 : AIE2PV16I32I32V16I32V16I32I32;
def int_aie2p_vmin_gebf16 : AIE2PV32BF16I32V32BF16V32BF16I32;

// vneg_gtz
def int_aie2p_vneg_gtz8  : AIE2PV64I8V2I32V64I8;
def int_aie2p_vneg_gtz16 : AIE2PV32I16I32V32I16;
def int_aie2p_vneg_gtz32 : AIE2PV16I32I32V16I32;

// vsel
def int_aie2p_vsel8  : ClangBuiltin<"__builtin_aie2p_vsel8">, AIE2PV64I8V64I8V64I8I64;
def int_aie2p_vsel16 : ClangBuiltin<"__builtin_aie2p_vsel16">, AIE2PV32I16V32I16V32I16I32;
def int_aie2p_vsel32 : ClangBuiltin<"__builtin_aie2p_vsel32">, AIE2PV16I32V16I32V16I32I32;

// vsub_ge
def int_aie2p_vsub_ge8  : AIE2PV64I8I64V64I8V64I8I32;
def int_aie2p_vsub_ge16 : AIE2PV32I16I32V32I16V32I16I32;
def int_aie2p_vsub_ge32 : AIE2PV16I32I32V16I32V16I32I32;

// vsub_lt
def int_aie2p_vsub_lt8  : AIE2PV64I8I64V64I8V64I8I32;
def int_aie2p_vsub_lt16 : AIE2PV32I16I32V32I16V32I16I32;
def int_aie2p_vsub_lt32 : AIE2PV16I32I32V16I32V16I32I32;

// vector bfloat16 to vector int
def int_aie2p_v16bf16_to_v16i32  : ClangBuiltin<"__builtin_aie2p_v16bf16_to_v16i32">, AIE2PV16BF16ToV16I;

//scl2vec::shift-shiftx-shift_bytes
def int_aie2p_vshift_I512_I512 : ClangBuiltin<"__builtin_aie2p_vshift_I512_I512">, AIE2PVShift_I512_I512;
def int_aie2p_vshift_bf512_bf512 : ClangBuiltin<"__builtin_aie2p_vshift_bf512_bf512">, AIE2PVShift_bf512_bf512;
//scl2vec::insert
def int_aie2p_vinsert32_bf512 : ClangBuiltin<"__builtin_aie2p_vinsert_bf32_bf512">, AIE2PVInsert32_bf512;
def int_aie2p_vinsert64_bf512 : ClangBuiltin<"__builtin_aie2p_vinsert_bf64_bf512">, AIE2PVInsert64_bf512;
def int_aie2p_vinsert32_accfloat : ClangBuiltin<"__builtin_aie2p_vinsert32_accfloat">, AIE2PVInsert32_accfloat;
//scl2vec::broadcast:broadcast_zero:broadcast_one
def int_aie2p_vbroadcast32_bf512 : ClangBuiltin<"__builtin_aie2p_vbroadcast_bf32_bf512">, AIE2PVBCST32bf512;
def int_aie2p_vbroadcast64_bf512 : ClangBuiltin<"__builtin_aie2p_vbroadcast_bf64_bf512">, AIE2PVBCST64bf512;
def int_aie2p_vbroadcast_zero_acc1024 : ClangBuiltin<"__builtin_aie2p_vbroadcast_zero_acc1024">, AIE2PVCLRACC1024;
//scl2vec::broadcast_elem
def int_aie2p_vextract_broadcast32_bf512 : ClangBuiltin<"__builtin_aie2p_vextract_broadcast_bf32_bf512">, AIE2PVEXTBCST32Bf512;
def int_aie2p_vextract_broadcast128_I512 : ClangBuiltin<"__builtin_aie2p_vextract_broadcast128_I512">, AIE2PVBCST128I512;
//scl2vec::vbcstshfl
def int_aie2p_vbcst_shuffle8  : ClangBuiltin<"__builtin_aie2p_vbcst_shuffle8">, AIE2PVBcstShfl_I32;
def int_aie2p_vbcst_shuffle16 : ClangBuiltin<"__builtin_aie2p_vbcst_shuffle16">, AIE2PVBcstShfl_I32;
def int_aie2p_vbcst_shuffle32 : ClangBuiltin<"__builtin_aie2p_vbcst_shuffle32">, AIE2PVBcstShfl_I32;
def int_aie2p_vbcst_shuffle64 : ClangBuiltin<"__builtin_aie2p_vbcst_shuffle64">, AIE2PVBcstShfl_I64;


// Streams
class AIE2PV16I32I32I32 : DefaultAttrsIntrinsic<[llvm_v16i32_ty], [llvm_i32_ty], [IntrInaccessibleMemOnly]>;
class AIE2PV16Acc32I32I32 : DefaultAttrsIntrinsic<[llvm_v16i32_ty], [llvm_i32_ty], [IntrInaccessibleMemOnly]>;
class AIE2PV32Acc32I32 : DefaultAttrsIntrinsic<[llvm_v32i32_ty], [llvm_i32_ty], [IntrInaccessibleMemOnly]>;
class AIE2PvV16I32I32 : DefaultAttrsIntrinsic<[], [llvm_v16i32_ty, llvm_i32_ty], [IntrInaccessibleMemOnly]>;
class AIE2PvV8Acc64I32 : DefaultAttrsIntrinsic<[], [llvm_v8i64_ty, llvm_i32_ty], [IntrInaccessibleMemOnly]>;
// Streams
// Cascade stream read
def int_aie2p_scd_read_vec : ClangBuiltin<"__builtin_aie2p_scd_read_vec">, AIE2PV16I32I32I32;
def int_aie2p_scd_read_acc32 : ClangBuiltin<"__builtin_aie2p_scd_read_acc32">, AIE2PV16Acc32I32I32;
def int_aie2p_scd_expand_lo : ClangBuiltin<"__builtin_aie2p_scd_expand_lo">, AIE2PV32Acc32I32;
def int_aie2p_scd_expand_hi : ClangBuiltin<"__builtin_aie2p_scd_expand_hi">, AIE2PV32Acc32I32;

// Cascade stream write
def int_aie2p_mcd_write_vec : ClangBuiltin<"__builtin_aie2p_mcd_write_vec">, AIE2PvV16I32I32;
def int_aie2p_mcd_write_acc32 : ClangBuiltin<"__builtin_aie2p_mcd_write_acc32">, AIE2PvV8Acc64I32;

// Scalar stream read
class AIE2P_get_ss : DefaultAttrsIntrinsic<[llvm_i32_ty, llvm_i32_ty], [], [IntrHasSideEffects, IntrNoMem]>;
def int_aie2p_get_ss : AIE2P_get_ss;
def int_aie2p_get_ss_nb : AIE2P_get_ss;

// Scalar stream write with tlast
class AIE2P_put_ms : DefaultAttrsIntrinsic<[], [llvm_i32_ty, llvm_i32_ty], [IntrHasSideEffects, IntrNoMem]>;
class AIE2P_put_ms_nb : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrHasSideEffects, IntrNoMem]>;
// Read data into accumulator lane
def int_aie2p_put_ms : ClangBuiltin<"__builtin_aie2p_put_ms">, AIE2P_put_ms;
def int_aie2p_put_ms_nb : AIE2P_put_ms_nb;
// Read data into accumulator lane
class AIE2PV64ACC32I32I32 : DefaultAttrsIntrinsic<[llvm_v64i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrHasSideEffects, IntrNoMem]>;
class AIE2PV32ACC32I32I32 : DefaultAttrsIntrinsic<[llvm_v32i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrHasSideEffects, IntrNoMem]>;
class AIE2PV16ACC64I32I32 : DefaultAttrsIntrinsic<[llvm_v16i64_ty, llvm_i32_ty], [llvm_i32_ty, llvm_ptr_ty], [IntrHasSideEffects, IntrNoMem]>;
class AIE2PV32ACC64I32I32 : DefaultAttrsIntrinsic<[llvm_v32i64_ty, llvm_i32_ty], [llvm_i32_ty, llvm_ptr_ty], [IntrHasSideEffects, IntrNoMem]>;
def int_aie2p_scd_expand_ACC1024 : ClangBuiltin<"__builtin_aie2p_scd_expand_ACC1024">, AIE2PV32ACC32I32I32;
def int_aie2p_scd_ACC2048 : ClangBuiltin<"__builtin_aie2p_scd_ACC2048">, AIE2PV64ACC32I32I32;
def int_aie2p_scd_expand_ACC2048 : ClangBuiltin<"__builtin_aie2p_scd_expand_ACC2048">, AIE2PV64ACC32I32I32;
def int_aie2p_scd_expand_ACC1024_incr: AIE2PV16ACC64I32I32;
def int_aie2p_scd_expand_ACC2048_incr: AIE2PV32ACC64I32I32;

// Read/Write for Tile Memory Map
def int_aie2p_read_tm : ClangBuiltin<"__builtin_aie2p_read_tm">, AIE2PReadTM;
def int_aie2p_write_tm : ClangBuiltin<"__builtin_aie2p_write_tm">, AIE2PWriteTM;

// flt2fix and fix2flt
def int_aie2p_fx2flt : ClangBuiltin<"__builtin_aie2p_fx2flt">, AIE2PFX2FLT;
def int_aie2p_flt2fx : ClangBuiltin<"__builtin_aie2p_flt2fx">, AIE2PFLT2FX;
def int_aie2p_exp2 : ClangBuiltin<"__builtin_aie2p_exp2">, AIE2PNLFVec;
def int_aie2p_tanh : ClangBuiltin<"__builtin_aie2p_tanh">, AIE2PNLFVec;
def int_aie2p_inv : ClangBuiltin<"__builtin_aie2p_inv">, AIE2PNLF;
def int_aie2p_invsqrt : ClangBuiltin<"__builtin_aie2p_invsqrt">, AIE2PNLF;
def int_aie2p_sqrtf : ClangBuiltin<"__builtin_aie2p_sqrtf">, AIE2PNLF;
// DIVS
def int_aie2p_divs : AIE2PDIVS;

// BFP16 MAC MUL
class AIE2PSHUFFLEBFP16
      : DefaultAttrsIntrinsic<[llvm_v64i8_ty, llvm_v8i8_ty], [llvm_v64i8_ty, llvm_v8i8_ty, llvm_v64i8_ty, llvm_v8i8_ty, llvm_i32_ty],
					 [IntrNoMem]>;
def int_aie2p_vshuffle_576_bfp16 :  AIE2PSHUFFLEBFP16;

// PUSH
class AIE2PFIFO_ST_PUSH_576_BFP16 : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty],
     [llvm_anyptr_ty, llvm_v64i8_ty, llvm_v8i8_ty, llvm_v32i32_ty, llvm_i32_ty],
     [IntrWriteMem, IntrArgMemOnly]>;
def int_aie2p_fifo_st_push_576_bfp16: AIE2PFIFO_ST_PUSH_576_BFP16;
def int_aie2p_fifo_st_push_544_bfp16: AIE2PFIFO_ST_PUSH_576_BFP16;
class AIE2PFIFO_ST_PUSH_512_BFP16 : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty],
     [llvm_anyptr_ty, llvm_v16i32_ty, llvm_v32i32_ty, llvm_i32_ty],
     [IntrWriteMem, IntrArgMemOnly]>;
def int_aie2p_fifo_st_push_512_bfp16: AIE2PFIFO_ST_PUSH_512_BFP16;

// FLUSH normal
class AIE2PFIFO_ST_FLUSH : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty],
     [IntrWriteMem, IntrArgMemOnly]>;
def int_aie2p_fifo_st_flush: AIE2PFIFO_ST_FLUSH;
def int_aie2p_fifo_st_flush_conv: AIE2PFIFO_ST_FLUSH;

// FLUSH 1D
class AIE2PFIFO_ST_FLUSH_1D : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty],
     [IntrWriteMem, IntrArgMemOnly]>;
def int_aie2p_fifo_st_flush_1d: AIE2PFIFO_ST_FLUSH_1D;
def int_aie2p_fifo_st_flush_1d_conv: AIE2PFIFO_ST_FLUSH_1D;

// FLUSH 2D
class AIE2PFIFO_ST_FLUSH_2D : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty],
     [IntrWriteMem, IntrArgMemOnly]>;
def int_aie2p_fifo_st_flush_2d: AIE2PFIFO_ST_FLUSH_2D;
def int_aie2p_fifo_st_flush_2d_conv: AIE2PFIFO_ST_FLUSH_2D;

// FLUSH 3D
class AIE2PFIFO_ST_FLUSH_3D : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty, llvm_i20_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty],
     [IntrWriteMem, IntrArgMemOnly]>;
def int_aie2p_fifo_st_flush_3d: AIE2PFIFO_ST_FLUSH_3D;
def int_aie2p_fifo_st_flush_3d_conv: AIE2PFIFO_ST_FLUSH_3D;

// FILLX
class AIE2PFIFO_LD_FILLX : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_v16i32_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_v16i32_ty, llvm_i32_ty],
     [IntrReadMem, IntrArgMemOnly]>;
def int_aie2p_fifo_ld_fillx: AIE2PFIFO_LD_FILLX;

// POPX
class AIE2PFIFO_LD_POPX : DefaultAttrsIntrinsic<[llvm_v64i8_ty, llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_v16i32_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_v16i32_ty, llvm_i32_ty],
     [IntrReadMem, IntrArgMemOnly]>;
def int_aie2p_fifo_ld_popx: AIE2PFIFO_LD_POPX;

// FILL
class AIE2PFIFO_LD_FILL : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty],
     [IntrReadMem, IntrArgMemOnly]>;
def int_aie2p_fifo_ld_fill: AIE2PFIFO_LD_FILL;

// POP normal
class AIE2PFIFO_LD_POPUnaligned : DefaultAttrsIntrinsic<[llvm_v64i8_ty, llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty],
     [IntrReadMem, IntrArgMemOnly]>;
class AIE2PFIFO_LD_POP_576_BFP16 : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_v64i8_ty, llvm_v8i8_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty],
     [IntrReadMem, IntrArgMemOnly]>;
def int_aie2p_fifo_ld_pop_unaligned: AIE2PFIFO_LD_POPUnaligned;
def int_aie2p_fifo_ld_pop_576_bfp16: AIE2PFIFO_LD_POP_576_BFP16;
def int_aie2p_fifo_ld_pop_544_bfp16: AIE2PFIFO_LD_POP_576_BFP16;

// POP 1D
class AIE2PFIFO_LD_POPUnaligned1D : DefaultAttrsIntrinsic<[llvm_v64i8_ty, llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty],
     [IntrReadMem, IntrArgMemOnly]>;
class AIE2PFIFO_LD_POP_576_BFP161D : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_v64i8_ty, llvm_v8i8_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty],
     [IntrReadMem, IntrArgMemOnly]>;
def int_aie2p_fifo_ld_pop_1d_unaligned: AIE2PFIFO_LD_POPUnaligned1D;
def int_aie2p_fifo_ld_pop_576_1d_bfp16: AIE2PFIFO_LD_POP_576_BFP161D;
def int_aie2p_fifo_ld_pop_544_1d_bfp16: AIE2PFIFO_LD_POP_576_BFP161D;

// POP 2D
class AIE2PFIFO_LD_POPUnaligned2D : DefaultAttrsIntrinsic<[llvm_v64i8_ty, llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty],
     [IntrReadMem, IntrArgMemOnly]>;
class AIE2PFIFO_LD_POP_576_BFP162D : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty, llvm_v64i8_ty, llvm_v8i8_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty],
     [IntrReadMem, IntrArgMemOnly]>;
def int_aie2p_fifo_ld_pop_2d_unaligned: AIE2PFIFO_LD_POPUnaligned2D;
def int_aie2p_fifo_ld_pop_576_2d_bfp16: AIE2PFIFO_LD_POP_576_BFP162D;
def int_aie2p_fifo_ld_pop_544_2d_bfp16: AIE2PFIFO_LD_POP_576_BFP162D;

// POP 3D
class AIE2PFIFO_LD_POPUnaligned3D : DefaultAttrsIntrinsic<[llvm_v64i8_ty, llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty, llvm_i20_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty],
     [IntrReadMem, IntrArgMemOnly]>;
class AIE2PFIFO_LD_POP_576_BFP163D : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty, llvm_i20_ty, llvm_v64i8_ty, llvm_v8i8_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty],
     [IntrReadMem, IntrArgMemOnly]>;
def int_aie2p_fifo_ld_pop_3d_unaligned: AIE2PFIFO_LD_POPUnaligned3D;
def int_aie2p_fifo_ld_pop_576_3d_bfp16: AIE2PFIFO_LD_POP_576_BFP163D;
def int_aie2p_fifo_ld_pop_544_3d_bfp16: AIE2PFIFO_LD_POP_576_BFP163D;

} // TargetPrefix = "aie2p"
