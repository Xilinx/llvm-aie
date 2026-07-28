//===- IntrinsicsAIE2PS.td - Defines AIE2PS intrinsics ------*- tablegen
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//===----------------------------------------------------------------------===//
//
// This file defines all of the AIE2PS-specific intrinsics.
//
//===----------------------------------------------------------------------===//

// aie2ps
let TargetPrefix = "aie2ps" in {

//===----------------------------------------------------------------------===//
class AIE2PSEventIntrinsic
    : DefaultAttrsIntrinsic<[],
                [llvm_i32_ty],
                [IntrHasSideEffects, IntrNoMem]>; //[IntrHasSideEffects, IntrNoMem, ImmArg<ArgIndex<0>>]>;
// Get Core ID
def int_aie2ps_get_coreid : ClangBuiltin<"__builtin_aie2ps_get_coreid">, AIE2PGetCoreID;

// CLB
def int_aie2ps_clb : ClangBuiltin<"__builtin_aie2ps_clb">, AIE2PCLB;

// UPS
class AIE2PSV16AccFloatToV16Float16 : DefaultAttrsIntrinsic<[llvm_v16f16_ty],
                                          [llvm_v16f32_ty],
                                          [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PSV16Float16ToV16AccFloat : DefaultAttrsIntrinsic<[llvm_v16f32_ty], [llvm_v16f16_ty], [IntrNoMem]>;

class AIE2PSV32AccFloatToV32Float16 : DefaultAttrsIntrinsic<[llvm_v32f16_ty],
                                          [llvm_v32f32_ty],
                                          [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PSV32Float16ToV32AccFloat : DefaultAttrsIntrinsic<[llvm_v32f32_ty], [llvm_v32f16_ty], [IntrNoMem]>;

// SRS
class AIE2PSV32AccFloatToV32Float8 : DefaultAttrsIntrinsic<[llvm_v32i8_ty],
                                          [llvm_v32f32_ty, llvm_i32_ty],
                                          [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PSV32AccFloatToV32Bf8 : DefaultAttrsIntrinsic<[llvm_v32i8_ty],
                                          [llvm_v32f32_ty, llvm_i32_ty],
                                          [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PSV64AccFloatToV64MX6 : DefaultAttrsIntrinsic<[llvm_v8i32_ty, llvm_v2i32_ty, llvm_i32_ty, llvm_i32_ty],
                                          [llvm_v64f32_ty],
                                          [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PSV64AccFloatToV64MX9 : DefaultAttrsIntrinsic<[llvm_v16i32_ty, llvm_v2i32_ty, llvm_v2i32_ty],
                                          [llvm_v64f32_ty],
                                          [IntrReadMem, IntrInaccessibleMemOnly]>;

//Locks
def int_aie2ps_event : ClangBuiltin<"__builtin_aie2ps_event">, AIE2PSEventIntrinsic;
def int_aie2ps_acquire :
     ClangBuiltin<"__builtin_aie2ps_acquire">, AIE2PLockIntr;
def int_aie2ps_acquire_cond :
     ClangBuiltin<"__builtin_aie2ps_acquire_cond">, AIE2PLockCondIntr;
def int_aie2ps_release :
     ClangBuiltin<"__builtin_aie2ps_release">, AIE2PLockIntr;
def int_aie2ps_release_cond :
     ClangBuiltin<"__builtin_aie2ps_release_cond">, AIE2PLockCondIntr;
def int_aie2ps_acquire_ptr :
     ClangBuiltin<"__builtin_aie2ps_acquire_ptr">, AIE2PLockPtrIntr;
def int_aie2ps_acquire_cond_ptr :
     ClangBuiltin<"__builtin_aie2ps_acquire_cond_ptr">, AIE2PLockCondPtrIntr;
def int_aie2ps_release_ptr :
     ClangBuiltin<"__builtin_aie2ps_release_ptr">, AIE2PLockPtrIntr;
def int_aie2ps_release_cond_ptr :
     ClangBuiltin<"__builtin_aie2ps_release_cond_ptr">, AIE2PLockCondPtrIntr;
def int_aie2ps_done :
    ClangBuiltin<"__builtin_aie2ps_done">, AIE2PDoneIntr;

// Scheduling barrier
def int_aie2ps_sched_barrier : ClangBuiltin<"__builtin_aie2ps_sched_barrier">, AIE2PSchedBarrier;
// flt2fix and fix2flt
def int_aie2ps_fx2flt : ClangBuiltin<"__builtin_aie2ps_fx2flt">, AIE2PFX2FLT;
def int_aie2ps_flt2fx : ClangBuiltin<"__builtin_aie2ps_flt2fx">, AIE2PFLT2FX;
def int_aie2ps_inv : ClangBuiltin<"__builtin_aie2ps_inv">, AIE2PNLF;
def int_aie2ps_invsqrt : ClangBuiltin<"__builtin_aie2ps_invsqrt">, AIE2PNLF;
def int_aie2ps_sqrtf : ClangBuiltin<"__builtin_aie2ps_sqrtf">, AIE2PNLF;
def int_aie2ps_exp2 : ClangBuiltin<"__builtin_aie2ps_exp2">, AIE2PNLFVec;
class AIE2PSNLFVecAcc : DefaultAttrsIntrinsic<[llvm_v16f32_ty], [llvm_v16f32_ty], [IntrNoMem]>;
def int_aie2ps_exp2_bf20 : ClangBuiltin<"__builtin_aie2ps_exp2_bf20">, AIE2PSNLFVecAcc;
def int_aie2ps_tanh : ClangBuiltin<"__builtin_aie2ps_tanh">, AIE2PNLFVec;
// DIVS
def int_aie2ps_divs : AIE2PDIVS;
// Read/Write for Tile Memory Map
def int_aie2ps_read_tm : ClangBuiltin<"__builtin_aie2ps_read_tm">, AIE2PReadTM;
def int_aie2ps_write_tm : ClangBuiltin<"__builtin_aie2ps_write_tm">, AIE2PWriteTM;
// Mul-Mac
class AIE2PS_ACC2048_ACCFLOAT_ADD : DefaultAttrsIntrinsic<[llvm_v64f32_ty], [llvm_v64f32_ty, llvm_v64f32_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_ACC2048_ACCFLOAT_NEG : DefaultAttrsIntrinsic<[llvm_v64f32_ty], [llvm_v64f32_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_ACC1024_ACCFLOAT_ADD : DefaultAttrsIntrinsic<[llvm_v32f32_ty], [llvm_v32f32_ty, llvm_v32f32_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_ACC1024_ACCFLOAT_NEG : DefaultAttrsIntrinsic<[llvm_v32f32_ty], [llvm_v32f32_ty, llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_ACC2048_ADD : DefaultAttrsIntrinsic<[llvm_v32i64_ty], [llvm_v32i64_ty, llvm_v32i64_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PS_ACC2048_NEG : DefaultAttrsIntrinsic<[llvm_v32i64_ty], [llvm_v32i64_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PS_ACC1024_ADD : DefaultAttrsIntrinsic<[llvm_v16i64_ty], [llvm_v16i64_ty, llvm_v16i64_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PS_ACC1024_NEG : DefaultAttrsIntrinsic<[llvm_v16i64_ty], [llvm_v16i64_ty, llvm_i32_ty], [IntrNoMem]>;

class AIE2PS_BF16_I1024_BF16_ACC2048_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64bf16_ty, llvm_bf16_ty, llvm_v64f32_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I1024_BF16_ACC2048_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64bf16_ty, llvm_bf16_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I1024_BF16_ACC2048_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64bf16_ty, llvm_bf16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_BF16_I1024_BF16_I1024_ACC2048_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64bf16_ty, llvm_v64bf16_ty, llvm_v64f32_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I1024_BF16_I1024_ACC2048_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64bf16_ty, llvm_v64bf16_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I1024_BF16_I1024_ACC2048_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64bf16_ty, llvm_v64bf16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_BF16_I1024_FP16_ACC2048_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64bf16_ty, llvm_f16_ty, llvm_v64f32_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I1024_FP16_ACC2048_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64bf16_ty, llvm_f16_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I1024_FP16_ACC2048_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64bf16_ty, llvm_f16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_BF16_I1024_FP16_I1024_ACC2048_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64bf16_ty, llvm_v64f16_ty, llvm_v64f32_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I1024_FP16_I1024_ACC2048_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64bf16_ty, llvm_v64f16_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I1024_FP16_I1024_ACC2048_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64bf16_ty, llvm_v64f16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_BF16_I512_BF16_ACC1024_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32bf16_ty, llvm_bf16_ty, llvm_v32f32_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I512_BF16_ACC1024_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32bf16_ty, llvm_bf16_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I512_BF16_ACC1024_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32bf16_ty, llvm_bf16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_BF16_I512_BF16_I1024_ACC1024_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32bf16_ty, llvm_v64bf16_ty, llvm_v32f32_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I512_BF16_I1024_ACC1024_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32bf16_ty, llvm_v64bf16_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I512_BF16_I1024_ACC1024_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32bf16_ty, llvm_v64bf16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_BF16_I512_BF16_I512_ACC1024_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32bf16_ty, llvm_v32bf16_ty, llvm_v32f32_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I512_BF16_I512_ACC1024_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32bf16_ty, llvm_v32bf16_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I512_BF16_I512_ACC1024_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32bf16_ty, llvm_v32bf16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_BF16_I512_FP16_ACC1024_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32bf16_ty, llvm_f16_ty, llvm_v32f32_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I512_FP16_ACC1024_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32bf16_ty, llvm_f16_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I512_FP16_ACC1024_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32bf16_ty, llvm_f16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_BF16_I512_FP16_I1024_ACC1024_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32bf16_ty, llvm_v64f16_ty, llvm_v32f32_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I512_FP16_I1024_ACC1024_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32bf16_ty, llvm_v64f16_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I512_FP16_I1024_ACC1024_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32bf16_ty, llvm_v64f16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_BF16_I512_FP16_I512_ACC1024_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32bf16_ty, llvm_v32f16_ty, llvm_v32f32_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I512_FP16_I512_ACC1024_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32bf16_ty, llvm_v32f16_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BF16_I512_FP16_I512_ACC1024_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32bf16_ty, llvm_v32f16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_BFP640_BFP2560_ACC2048_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v16i32_ty, llvm_v2i32_ty, llvm_v2i32_ty,
                               llvm_v16i32_ty, llvm_v16i32_ty, llvm_v16i32_ty, llvm_v16i32_ty,
                               llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty,
                               llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty,
                               llvm_v64f32_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BFP640_BFP2560_ACC2048_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v16i32_ty, llvm_v2i32_ty, llvm_v2i32_ty,
                               llvm_v16i32_ty, llvm_v16i32_ty, llvm_v16i32_ty, llvm_v16i32_ty,
                               llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty,
                               llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty,
                               llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BFP640_BFP2560_ACC2048_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v16i32_ty, llvm_v2i32_ty, llvm_v2i32_ty,
                               llvm_v16i32_ty, llvm_v16i32_ty, llvm_v16i32_ty, llvm_v16i32_ty,
                               llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty,
                               llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BFP768_BFP1536_ACC2048_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v8i32_ty, llvm_v8i32_ty, llvm_v2i32_ty, llvm_v2i32_ty,
                               llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty,
                               llvm_v8i32_ty, llvm_v8i32_ty, llvm_v8i32_ty, llvm_v8i32_ty,
                               llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty,
                               llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty,
                               llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty,
                               llvm_v64f32_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BFP768_BFP1536_ACC2048_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v8i32_ty, llvm_v8i32_ty, llvm_v2i32_ty, llvm_v2i32_ty,
                               llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty,
                               llvm_v8i32_ty, llvm_v8i32_ty, llvm_v8i32_ty, llvm_v8i32_ty,
                               llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty,
                               llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty,
                               llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty,
                               llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_BFP768_BFP1536_ACC2048_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v8i32_ty, llvm_v8i32_ty, llvm_v2i32_ty, llvm_v2i32_ty,
                               llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty,
                               llvm_v8i32_ty, llvm_v8i32_ty, llvm_v8i32_ty, llvm_v8i32_ty,
                               llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty,
                               llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty,
                               llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty,
                               llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_CLR_ACC1024L : DefaultAttrsIntrinsic<[llvm_v32i32_ty], [], [IntrNoMem]>;
class AIE2PS_CLR_ACC2048 : DefaultAttrsIntrinsic<[llvm_v32i64_ty], [], [IntrNoMem]>;

class AIE2PS_FP16_I1024_BF16_ACC2048_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64f16_ty, llvm_bf16_ty, llvm_v64f32_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I1024_BF16_ACC2048_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64f16_ty, llvm_bf16_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I1024_BF16_ACC2048_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64f16_ty, llvm_bf16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_FP16_I1024_BF16_I1024_ACC2048_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64f16_ty, llvm_v64bf16_ty, llvm_v64f32_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I1024_BF16_I1024_ACC2048_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64f16_ty, llvm_v64bf16_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I1024_BF16_I1024_ACC2048_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64f16_ty, llvm_v64bf16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_FP16_I1024_FP16_ACC2048_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64f16_ty, llvm_f16_ty, llvm_v64f32_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I1024_FP16_ACC2048_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64f16_ty, llvm_f16_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I1024_FP16_ACC2048_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64f16_ty, llvm_f16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_FP16_I1024_FP16_I1024_ACC2048_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64f16_ty, llvm_v64f16_ty, llvm_v64f32_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I1024_FP16_I1024_ACC2048_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64f16_ty, llvm_v64f16_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I1024_FP16_I1024_ACC2048_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64f16_ty, llvm_v64f16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_FP16_I512_BF16_ACC1024_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32f16_ty, llvm_bf16_ty, llvm_v32f32_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I512_BF16_ACC1024_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32f16_ty, llvm_bf16_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I512_BF16_ACC1024_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32f16_ty, llvm_bf16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_FP16_I512_BF16_I1024_ACC1024_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32f16_ty, llvm_v64bf16_ty, llvm_v32f32_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I512_BF16_I1024_ACC1024_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32f16_ty, llvm_v64bf16_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I512_BF16_I1024_ACC1024_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32f16_ty, llvm_v64bf16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_FP16_I512_BF16_I512_ACC1024_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32f16_ty, llvm_v32bf16_ty, llvm_v32f32_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I512_BF16_I512_ACC1024_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32f16_ty, llvm_v32bf16_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I512_BF16_I512_ACC1024_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32f16_ty, llvm_v32bf16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_FP16_I512_FP16_ACC1024_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32f16_ty, llvm_f16_ty, llvm_v32f32_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I512_FP16_ACC1024_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32f16_ty, llvm_f16_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I512_FP16_ACC1024_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32f16_ty, llvm_f16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_FP16_I512_FP16_I1024_ACC1024_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32f16_ty, llvm_v64f16_ty, llvm_v32f32_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I512_FP16_I1024_ACC1024_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32f16_ty, llvm_v64f16_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I512_FP16_I1024_ACC1024_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32f16_ty, llvm_v64f16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_FP16_I512_FP16_I512_ACC1024_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32f16_ty, llvm_v32f16_ty, llvm_v32f32_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I512_FP16_I512_ACC1024_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32f16_ty, llvm_v32f16_ty, llvm_v32f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP16_I512_FP16_I512_ACC1024_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v32f32_ty],
                              [llvm_v32f16_ty, llvm_v32f16_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_FP8_I512_FP8_I512_ACC2048_BF_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64i8_ty, llvm_v64i8_ty, llvm_v64f32_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP8_I512_FP8_I512_ACC2048_BF_MAC
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64i8_ty, llvm_v64i8_ty, llvm_v64f32_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;
class AIE2PS_FP8_I512_FP8_I512_ACC2048_BF_MUL
      : DefaultAttrsIntrinsic<[llvm_v64f32_ty],
                              [llvm_v64i8_ty, llvm_v64i8_ty, llvm_i32_ty],
                              [IntrReadMem, IntrInaccessibleMemOnly]>;

class AIE2PS_I1024_I1024_ACC2048_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v64i16_ty, llvm_v64i16_ty, llvm_v64i32_ty, llvm_v64i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;
class AIE2PS_I1024_I1024_ACC2048_MAC
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v64i16_ty, llvm_v64i16_ty, llvm_v64i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;
class AIE2PS_I1024_I1024_ACC2048_MUL
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v64i16_ty, llvm_v64i16_ty, llvm_i32_ty],
                              [IntrNoMem]>;

class AIE2PS_I1024_I512_ACC2048_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v64i16_ty, llvm_v32i16_ty, llvm_v64i32_ty, llvm_v64i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;
class AIE2PS_I1024_I512_ACC2048_MAC
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v64i16_ty, llvm_v32i16_ty, llvm_v64i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;
class AIE2PS_I1024_I512_ACC2048_MUL
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v64i16_ty, llvm_v32i16_ty, llvm_i32_ty],
                              [IntrNoMem]>;

class AIE2PS_I1024_SHORT_ACC2048_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v64i16_ty, llvm_i32_ty, llvm_v64i32_ty, llvm_v64i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;
class AIE2PS_I1024_SHORT_ACC2048_MAC
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v64i16_ty, llvm_i32_ty, llvm_v64i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;
class AIE2PS_I1024_SHORT_ACC2048_MUL
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v64i16_ty, llvm_i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;

class AIE2PS_I512_CHAR_ACC2048_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v32i16_ty, llvm_i32_ty, llvm_v64i32_ty, llvm_v64i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;
class AIE2PS_I512_CHAR_ACC2048_MAC
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v32i16_ty, llvm_i32_ty, llvm_v64i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;
class AIE2PS_I512_CHAR_ACC2048_MUL
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v64i8_ty, llvm_i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;

class AIE2PS_I512_I1024_ACC2048_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v32i16_ty, llvm_v64i16_ty, llvm_v64i32_ty, llvm_v64i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;
class AIE2PS_I512_I1024_ACC2048_MAC
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v32i16_ty, llvm_v64i16_ty, llvm_v64i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;
class AIE2PS_I512_I1024_ACC2048_MUL
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v32i16_ty, llvm_v64i16_ty, llvm_i32_ty],
                              [IntrNoMem]>;

class AIE2PS_I512_I512_ACC1024_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v32i32_ty],
                              [llvm_v32i16_ty, llvm_v32i16_ty, llvm_v32i32_ty, llvm_v32i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;
class AIE2PS_I512_I512_ACC1024_MAC
      : DefaultAttrsIntrinsic<[llvm_v32i32_ty],
                              [llvm_v32i16_ty, llvm_v32i16_ty, llvm_v32i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;
class AIE2PS_I512_I512_ACC1024_MUL
      : DefaultAttrsIntrinsic<[llvm_v32i32_ty],
                              [llvm_v32i16_ty, llvm_v32i16_ty, llvm_i32_ty],
                              [IntrNoMem]>;

class AIE2PS_I512_I512_ACC2048_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v32i16_ty, llvm_v32i16_ty, llvm_v64i32_ty, llvm_v64i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;
class AIE2PS_I512_I512_ACC2048_MAC
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v32i16_ty, llvm_v32i16_ty, llvm_v64i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;
class AIE2PS_I512_I512_ACC2048_MUL
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v64i8_ty, llvm_v64i8_ty, llvm_i32_ty],
                              [IntrNoMem]>;

class AIE2PS_I512_SHORT_ACC2048_ADDMAC
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v32i16_ty, llvm_i32_ty, llvm_v64i32_ty, llvm_v64i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;
class AIE2PS_I512_SHORT_ACC2048_MAC
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v32i16_ty, llvm_i32_ty, llvm_v64i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;
class AIE2PS_I512_SHORT_ACC2048_MUL
      : DefaultAttrsIntrinsic<[llvm_v64i32_ty],
                              [llvm_v32i16_ty, llvm_i32_ty, llvm_i32_ty],
                              [IntrNoMem]>;


def int_aie2ps_ACC2048_accfloat_add_conf : ClangBuiltin<"__builtin_aie2ps_ACC2048_accfloat_add_conf">, AIE2PS_ACC2048_ACCFLOAT_ADD;
def int_aie2ps_ACC2048_accfloat_neg_conf : ClangBuiltin<"__builtin_aie2ps_ACC2048_accfloat_neg_conf">, AIE2PS_ACC2048_ACCFLOAT_NEG;
def int_aie2ps_ACC2048_accfloat_sub_conf : ClangBuiltin<"__builtin_aie2ps_ACC2048_accfloat_sub_conf">, AIE2PS_ACC2048_ACCFLOAT_ADD;
def int_aie2ps_ACC1024_accfloat_add_conf : ClangBuiltin<"__builtin_aie2ps_ACC1024_accfloat_add_conf">, AIE2PS_ACC1024_ACCFLOAT_ADD;
def int_aie2ps_ACC1024_accfloat_neg_conf : ClangBuiltin<"__builtin_aie2ps_ACC1024_accfloat_neg_conf">, AIE2PS_ACC1024_ACCFLOAT_NEG;
def int_aie2ps_ACC1024_accfloat_sub_conf : ClangBuiltin<"__builtin_aie2ps_ACC1024_accfloat_sub_conf">, AIE2PS_ACC1024_ACCFLOAT_ADD;

def int_aie2ps_ACC2048_add_conf : ClangBuiltin<"__builtin_aie2ps_ACC2048_add_conf">, AIE2PS_ACC2048_ADD;
def int_aie2ps_ACC2048_neg_conf : ClangBuiltin<"__builtin_aie2ps_ACC2048_neg_conf">, AIE2PS_ACC2048_NEG;
def int_aie2ps_ACC2048_sub_conf : ClangBuiltin<"__builtin_aie2ps_ACC2048_sub_conf">, AIE2PS_ACC2048_ADD;
def int_aie2ps_ACC1024_add_conf : ClangBuiltin<"__builtin_aie2ps_ACC1024_add_conf">, AIE2PS_ACC1024_ADD;
def int_aie2ps_ACC1024_neg_conf : ClangBuiltin<"__builtin_aie2ps_ACC1024_neg_conf">, AIE2PS_ACC1024_NEG;
def int_aie2ps_ACC1024_sub_conf : ClangBuiltin<"__builtin_aie2ps_ACC1024_sub_conf">, AIE2PS_ACC1024_ADD;

def int_aie2ps_bf16_I1024_bf16_ACC2048_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_bf16_ACC2048_bf_addmac_conf">, AIE2PS_BF16_I1024_BF16_ACC2048_BF_ADDMAC;
def int_aie2ps_bf16_I1024_bf16_ACC2048_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_bf16_ACC2048_bf_addmsc_conf">, AIE2PS_BF16_I1024_BF16_ACC2048_BF_ADDMAC;
def int_aie2ps_bf16_I1024_bf16_ACC2048_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_bf16_ACC2048_bf_mac_conf">, AIE2PS_BF16_I1024_BF16_ACC2048_BF_MAC;
def int_aie2ps_bf16_I1024_bf16_ACC2048_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_bf16_ACC2048_bf_msc_conf">, AIE2PS_BF16_I1024_BF16_ACC2048_BF_MAC;
def int_aie2ps_bf16_I1024_bf16_ACC2048_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_bf16_ACC2048_bf_mul_conf">, AIE2PS_BF16_I1024_BF16_ACC2048_BF_MUL;
def int_aie2ps_bf16_I1024_bf16_ACC2048_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_bf16_ACC2048_bf_negmul_conf">, AIE2PS_BF16_I1024_BF16_ACC2048_BF_MUL;
def int_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_addmac_conf">, AIE2PS_BF16_I1024_BF16_I1024_ACC2048_BF_ADDMAC;
def int_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_addmsc_conf">, AIE2PS_BF16_I1024_BF16_I1024_ACC2048_BF_ADDMAC;
def int_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_mac_conf">, AIE2PS_BF16_I1024_BF16_I1024_ACC2048_BF_MAC;
def int_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_msc_conf">, AIE2PS_BF16_I1024_BF16_I1024_ACC2048_BF_MAC;
def int_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_mul_conf">, AIE2PS_BF16_I1024_BF16_I1024_ACC2048_BF_MUL;
def int_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_bf16_I1024_ACC2048_bf_negmul_conf">, AIE2PS_BF16_I1024_BF16_I1024_ACC2048_BF_MUL;
def int_aie2ps_bf16_I1024_fp16_ACC2048_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_fp16_ACC2048_bf_addmac_conf">, AIE2PS_BF16_I1024_FP16_ACC2048_BF_ADDMAC;
def int_aie2ps_bf16_I1024_fp16_ACC2048_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_fp16_ACC2048_bf_addmsc_conf">, AIE2PS_BF16_I1024_FP16_ACC2048_BF_ADDMAC;
def int_aie2ps_bf16_I1024_fp16_ACC2048_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_fp16_ACC2048_bf_mac_conf">, AIE2PS_BF16_I1024_FP16_ACC2048_BF_MAC;
def int_aie2ps_bf16_I1024_fp16_ACC2048_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_fp16_ACC2048_bf_msc_conf">, AIE2PS_BF16_I1024_FP16_ACC2048_BF_MAC;
def int_aie2ps_bf16_I1024_fp16_ACC2048_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_fp16_ACC2048_bf_mul_conf">, AIE2PS_BF16_I1024_FP16_ACC2048_BF_MUL;
def int_aie2ps_bf16_I1024_fp16_ACC2048_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_fp16_ACC2048_bf_negmul_conf">, AIE2PS_BF16_I1024_FP16_ACC2048_BF_MUL;
def int_aie2ps_bf16_I1024_fp16_I1024_ACC2048_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_fp16_I1024_ACC2048_bf_addmac_conf">, AIE2PS_BF16_I1024_FP16_I1024_ACC2048_BF_ADDMAC;
def int_aie2ps_bf16_I1024_fp16_I1024_ACC2048_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_fp16_I1024_ACC2048_bf_addmsc_conf">, AIE2PS_BF16_I1024_FP16_I1024_ACC2048_BF_ADDMAC;
def int_aie2ps_bf16_I1024_fp16_I1024_ACC2048_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_fp16_I1024_ACC2048_bf_mac_conf">, AIE2PS_BF16_I1024_FP16_I1024_ACC2048_BF_MAC;
def int_aie2ps_bf16_I1024_fp16_I1024_ACC2048_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_fp16_I1024_ACC2048_bf_msc_conf">, AIE2PS_BF16_I1024_FP16_I1024_ACC2048_BF_MAC;
def int_aie2ps_bf16_I1024_fp16_I1024_ACC2048_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_fp16_I1024_ACC2048_bf_mul_conf">, AIE2PS_BF16_I1024_FP16_I1024_ACC2048_BF_MUL;
def int_aie2ps_bf16_I1024_fp16_I1024_ACC2048_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I1024_fp16_I1024_ACC2048_bf_negmul_conf">, AIE2PS_BF16_I1024_FP16_I1024_ACC2048_BF_MUL;
def int_aie2ps_bf16_I512_bf16_ACC1024_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_bf16_ACC1024_bf_addmac_conf">, AIE2PS_BF16_I512_BF16_ACC1024_BF_ADDMAC;
def int_aie2ps_bf16_I512_bf16_ACC1024_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_bf16_ACC1024_bf_addmsc_conf">, AIE2PS_BF16_I512_BF16_ACC1024_BF_ADDMAC;
def int_aie2ps_bf16_I512_bf16_ACC1024_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_bf16_ACC1024_bf_mac_conf">, AIE2PS_BF16_I512_BF16_ACC1024_BF_MAC;
def int_aie2ps_bf16_I512_bf16_ACC1024_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_bf16_ACC1024_bf_msc_conf">, AIE2PS_BF16_I512_BF16_ACC1024_BF_MAC;
def int_aie2ps_bf16_I512_bf16_ACC1024_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_bf16_ACC1024_bf_mul_conf">, AIE2PS_BF16_I512_BF16_ACC1024_BF_MUL;
def int_aie2ps_bf16_I512_bf16_ACC1024_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_bf16_ACC1024_bf_negmul_conf">, AIE2PS_BF16_I512_BF16_ACC1024_BF_MUL;
def int_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_addmac_conf">, AIE2PS_BF16_I512_BF16_I1024_ACC1024_BF_ADDMAC;
def int_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_addmsc_conf">, AIE2PS_BF16_I512_BF16_I1024_ACC1024_BF_ADDMAC;
def int_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_mac_conf">, AIE2PS_BF16_I512_BF16_I1024_ACC1024_BF_MAC;
def int_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_msc_conf">, AIE2PS_BF16_I512_BF16_I1024_ACC1024_BF_MAC;
def int_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_mul_conf">, AIE2PS_BF16_I512_BF16_I1024_ACC1024_BF_MUL;
def int_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_bf16_I1024_ACC1024_bf_negmul_conf">, AIE2PS_BF16_I512_BF16_I1024_ACC1024_BF_MUL;
def int_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_addmac_conf">, AIE2PS_BF16_I512_BF16_I512_ACC1024_BF_ADDMAC;
def int_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_addmsc_conf">, AIE2PS_BF16_I512_BF16_I512_ACC1024_BF_ADDMAC;
def int_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_mac_conf">, AIE2PS_BF16_I512_BF16_I512_ACC1024_BF_MAC;
def int_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_msc_conf">, AIE2PS_BF16_I512_BF16_I512_ACC1024_BF_MAC;
def int_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_mul_conf">, AIE2PS_BF16_I512_BF16_I512_ACC1024_BF_MUL;
def int_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_bf16_I512_ACC1024_bf_negmul_conf">, AIE2PS_BF16_I512_BF16_I512_ACC1024_BF_MUL;
def int_aie2ps_bf16_I512_fp16_ACC1024_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_fp16_ACC1024_bf_addmac_conf">, AIE2PS_BF16_I512_FP16_ACC1024_BF_ADDMAC;
def int_aie2ps_bf16_I512_fp16_ACC1024_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_fp16_ACC1024_bf_addmsc_conf">, AIE2PS_BF16_I512_FP16_ACC1024_BF_ADDMAC;
def int_aie2ps_bf16_I512_fp16_ACC1024_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_fp16_ACC1024_bf_mac_conf">, AIE2PS_BF16_I512_FP16_ACC1024_BF_MAC;
def int_aie2ps_bf16_I512_fp16_ACC1024_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_fp16_ACC1024_bf_msc_conf">, AIE2PS_BF16_I512_FP16_ACC1024_BF_MAC;
def int_aie2ps_bf16_I512_fp16_ACC1024_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_fp16_ACC1024_bf_mul_conf">, AIE2PS_BF16_I512_FP16_ACC1024_BF_MUL;
def int_aie2ps_bf16_I512_fp16_ACC1024_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_fp16_ACC1024_bf_negmul_conf">, AIE2PS_BF16_I512_FP16_ACC1024_BF_MUL;
def int_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_addmac_conf">, AIE2PS_BF16_I512_FP16_I1024_ACC1024_BF_ADDMAC;
def int_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_addmsc_conf">, AIE2PS_BF16_I512_FP16_I1024_ACC1024_BF_ADDMAC;
def int_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_mac_conf">, AIE2PS_BF16_I512_FP16_I1024_ACC1024_BF_MAC;
def int_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_msc_conf">, AIE2PS_BF16_I512_FP16_I1024_ACC1024_BF_MAC;
def int_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_mul_conf">, AIE2PS_BF16_I512_FP16_I1024_ACC1024_BF_MUL;
def int_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_fp16_I1024_ACC1024_bf_negmul_conf">, AIE2PS_BF16_I512_FP16_I1024_ACC1024_BF_MUL;
def int_aie2ps_bf16_I512_fp16_I512_ACC1024_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_fp16_I512_ACC1024_bf_addmac_conf">, AIE2PS_BF16_I512_FP16_I512_ACC1024_BF_ADDMAC;
def int_aie2ps_bf16_I512_fp16_I512_ACC1024_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_fp16_I512_ACC1024_bf_addmsc_conf">, AIE2PS_BF16_I512_FP16_I512_ACC1024_BF_ADDMAC;
def int_aie2ps_bf16_I512_fp16_I512_ACC1024_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_fp16_I512_ACC1024_bf_mac_conf">, AIE2PS_BF16_I512_FP16_I512_ACC1024_BF_MAC;
def int_aie2ps_bf16_I512_fp16_I512_ACC1024_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_fp16_I512_ACC1024_bf_msc_conf">, AIE2PS_BF16_I512_FP16_I512_ACC1024_BF_MAC;
def int_aie2ps_bf16_I512_fp16_I512_ACC1024_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_fp16_I512_ACC1024_bf_mul_conf">, AIE2PS_BF16_I512_FP16_I512_ACC1024_BF_MUL;
def int_aie2ps_bf16_I512_fp16_I512_ACC1024_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_bf16_I512_fp16_I512_ACC1024_bf_negmul_conf">, AIE2PS_BF16_I512_FP16_I512_ACC1024_BF_MUL;
def int_aie2ps_BFP640_BFP2560_ACC2048_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_BFP640_BFP2560_ACC2048_bf_addmac_conf">, AIE2PS_BFP640_BFP2560_ACC2048_BF_ADDMAC;
def int_aie2ps_BFP640_BFP2560_ACC2048_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_BFP640_BFP2560_ACC2048_bf_addmsc_conf">, AIE2PS_BFP640_BFP2560_ACC2048_BF_ADDMAC;
def int_aie2ps_BFP640_BFP2560_ACC2048_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_BFP640_BFP2560_ACC2048_bf_mac_conf">, AIE2PS_BFP640_BFP2560_ACC2048_BF_MAC;
def int_aie2ps_BFP640_BFP2560_ACC2048_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_BFP640_BFP2560_ACC2048_bf_msc_conf">, AIE2PS_BFP640_BFP2560_ACC2048_BF_MAC;
def int_aie2ps_BFP640_BFP2560_ACC2048_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_BFP640_BFP2560_ACC2048_bf_mul_conf">, AIE2PS_BFP640_BFP2560_ACC2048_BF_MUL;
def int_aie2ps_BFP640_BFP2560_ACC2048_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_BFP640_BFP2560_ACC2048_bf_negmul_conf">, AIE2PS_BFP640_BFP2560_ACC2048_BF_MUL;
def int_aie2ps_BFP768_BFP1536_ACC2048_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_BFP768_BFP1536_ACC2048_bf_addmac_conf">, AIE2PS_BFP768_BFP1536_ACC2048_BF_ADDMAC;
def int_aie2ps_BFP768_BFP1536_ACC2048_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_BFP768_BFP1536_ACC2048_bf_addmsc_conf">, AIE2PS_BFP768_BFP1536_ACC2048_BF_ADDMAC;
def int_aie2ps_BFP768_BFP1536_ACC2048_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_BFP768_BFP1536_ACC2048_bf_mac_conf">, AIE2PS_BFP768_BFP1536_ACC2048_BF_MAC;
def int_aie2ps_BFP768_BFP1536_ACC2048_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_BFP768_BFP1536_ACC2048_bf_msc_conf">, AIE2PS_BFP768_BFP1536_ACC2048_BF_MAC;
def int_aie2ps_BFP768_BFP1536_ACC2048_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_BFP768_BFP1536_ACC2048_bf_mul_conf">, AIE2PS_BFP768_BFP1536_ACC2048_BF_MUL;
def int_aie2ps_BFP768_BFP1536_ACC2048_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_BFP768_BFP1536_ACC2048_bf_negmul_conf">, AIE2PS_BFP768_BFP1536_ACC2048_BF_MUL;
def int_aie2ps_clr_ACC1024l_conf : ClangBuiltin<"__builtin_aie2ps_clr_ACC1024l_conf">, AIE2PS_CLR_ACC1024L;
def int_aie2ps_clr_ACC2048_conf : ClangBuiltin<"__builtin_aie2ps_clr_ACC2048_conf">, AIE2PS_CLR_ACC2048;
def int_aie2ps_fp16_I1024_bf16_ACC2048_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_bf16_ACC2048_bf_addmac_conf">, AIE2PS_FP16_I1024_BF16_ACC2048_BF_ADDMAC;
def int_aie2ps_fp16_I1024_bf16_ACC2048_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_bf16_ACC2048_bf_addmsc_conf">, AIE2PS_FP16_I1024_BF16_ACC2048_BF_ADDMAC;
def int_aie2ps_fp16_I1024_bf16_ACC2048_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_bf16_ACC2048_bf_mac_conf">, AIE2PS_FP16_I1024_BF16_ACC2048_BF_MAC;
def int_aie2ps_fp16_I1024_bf16_ACC2048_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_bf16_ACC2048_bf_msc_conf">, AIE2PS_FP16_I1024_BF16_ACC2048_BF_MAC;
def int_aie2ps_fp16_I1024_bf16_ACC2048_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_bf16_ACC2048_bf_mul_conf">, AIE2PS_FP16_I1024_BF16_ACC2048_BF_MUL;
def int_aie2ps_fp16_I1024_bf16_ACC2048_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_bf16_ACC2048_bf_negmul_conf">, AIE2PS_FP16_I1024_BF16_ACC2048_BF_MUL;
def int_aie2ps_fp16_I1024_bf16_I1024_ACC2048_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_bf16_I1024_ACC2048_bf_addmac_conf">, AIE2PS_FP16_I1024_BF16_I1024_ACC2048_BF_ADDMAC;
def int_aie2ps_fp16_I1024_bf16_I1024_ACC2048_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_bf16_I1024_ACC2048_bf_addmsc_conf">, AIE2PS_FP16_I1024_BF16_I1024_ACC2048_BF_ADDMAC;
def int_aie2ps_fp16_I1024_bf16_I1024_ACC2048_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_bf16_I1024_ACC2048_bf_mac_conf">, AIE2PS_FP16_I1024_BF16_I1024_ACC2048_BF_MAC;
def int_aie2ps_fp16_I1024_bf16_I1024_ACC2048_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_bf16_I1024_ACC2048_bf_msc_conf">, AIE2PS_FP16_I1024_BF16_I1024_ACC2048_BF_MAC;
def int_aie2ps_fp16_I1024_bf16_I1024_ACC2048_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_bf16_I1024_ACC2048_bf_mul_conf">, AIE2PS_FP16_I1024_BF16_I1024_ACC2048_BF_MUL;
def int_aie2ps_fp16_I1024_bf16_I1024_ACC2048_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_bf16_I1024_ACC2048_bf_negmul_conf">, AIE2PS_FP16_I1024_BF16_I1024_ACC2048_BF_MUL;
def int_aie2ps_fp16_I1024_fp16_ACC2048_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_fp16_ACC2048_bf_addmac_conf">, AIE2PS_FP16_I1024_FP16_ACC2048_BF_ADDMAC;
def int_aie2ps_fp16_I1024_fp16_ACC2048_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_fp16_ACC2048_bf_addmsc_conf">, AIE2PS_FP16_I1024_FP16_ACC2048_BF_ADDMAC;
def int_aie2ps_fp16_I1024_fp16_ACC2048_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_fp16_ACC2048_bf_mac_conf">, AIE2PS_FP16_I1024_FP16_ACC2048_BF_MAC;
def int_aie2ps_fp16_I1024_fp16_ACC2048_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_fp16_ACC2048_bf_msc_conf">, AIE2PS_FP16_I1024_FP16_ACC2048_BF_MAC;
def int_aie2ps_fp16_I1024_fp16_ACC2048_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_fp16_ACC2048_bf_mul_conf">, AIE2PS_FP16_I1024_FP16_ACC2048_BF_MUL;
def int_aie2ps_fp16_I1024_fp16_ACC2048_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_fp16_ACC2048_bf_negmul_conf">, AIE2PS_FP16_I1024_FP16_ACC2048_BF_MUL;
def int_aie2ps_fp16_I1024_fp16_I1024_ACC2048_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_fp16_I1024_ACC2048_bf_addmac_conf">, AIE2PS_FP16_I1024_FP16_I1024_ACC2048_BF_ADDMAC;
def int_aie2ps_fp16_I1024_fp16_I1024_ACC2048_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_fp16_I1024_ACC2048_bf_addmsc_conf">, AIE2PS_FP16_I1024_FP16_I1024_ACC2048_BF_ADDMAC;
def int_aie2ps_fp16_I1024_fp16_I1024_ACC2048_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_fp16_I1024_ACC2048_bf_mac_conf">, AIE2PS_FP16_I1024_FP16_I1024_ACC2048_BF_MAC;
def int_aie2ps_fp16_I1024_fp16_I1024_ACC2048_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_fp16_I1024_ACC2048_bf_msc_conf">, AIE2PS_FP16_I1024_FP16_I1024_ACC2048_BF_MAC;
def int_aie2ps_fp16_I1024_fp16_I1024_ACC2048_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_fp16_I1024_ACC2048_bf_mul_conf">, AIE2PS_FP16_I1024_FP16_I1024_ACC2048_BF_MUL;
def int_aie2ps_fp16_I1024_fp16_I1024_ACC2048_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I1024_fp16_I1024_ACC2048_bf_negmul_conf">, AIE2PS_FP16_I1024_FP16_I1024_ACC2048_BF_MUL;
def int_aie2ps_fp16_I512_bf16_ACC1024_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_bf16_ACC1024_bf_addmac_conf">, AIE2PS_FP16_I512_BF16_ACC1024_BF_ADDMAC;
def int_aie2ps_fp16_I512_bf16_ACC1024_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_bf16_ACC1024_bf_addmsc_conf">, AIE2PS_FP16_I512_BF16_ACC1024_BF_ADDMAC;
def int_aie2ps_fp16_I512_bf16_ACC1024_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_bf16_ACC1024_bf_mac_conf">, AIE2PS_FP16_I512_BF16_ACC1024_BF_MAC;
def int_aie2ps_fp16_I512_bf16_ACC1024_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_bf16_ACC1024_bf_msc_conf">, AIE2PS_FP16_I512_BF16_ACC1024_BF_MAC;
def int_aie2ps_fp16_I512_bf16_ACC1024_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_bf16_ACC1024_bf_mul_conf">, AIE2PS_FP16_I512_BF16_ACC1024_BF_MUL;
def int_aie2ps_fp16_I512_bf16_ACC1024_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_bf16_ACC1024_bf_negmul_conf">, AIE2PS_FP16_I512_BF16_ACC1024_BF_MUL;
def int_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_addmac_conf">, AIE2PS_FP16_I512_BF16_I1024_ACC1024_BF_ADDMAC;
def int_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_addmsc_conf">, AIE2PS_FP16_I512_BF16_I1024_ACC1024_BF_ADDMAC;
def int_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_mac_conf">, AIE2PS_FP16_I512_BF16_I1024_ACC1024_BF_MAC;
def int_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_msc_conf">, AIE2PS_FP16_I512_BF16_I1024_ACC1024_BF_MAC;
def int_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_mul_conf">, AIE2PS_FP16_I512_BF16_I1024_ACC1024_BF_MUL;
def int_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_bf16_I1024_ACC1024_bf_negmul_conf">, AIE2PS_FP16_I512_BF16_I1024_ACC1024_BF_MUL;
def int_aie2ps_fp16_I512_bf16_I512_ACC1024_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_bf16_I512_ACC1024_bf_addmac_conf">, AIE2PS_FP16_I512_BF16_I512_ACC1024_BF_ADDMAC;
def int_aie2ps_fp16_I512_bf16_I512_ACC1024_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_bf16_I512_ACC1024_bf_addmsc_conf">, AIE2PS_FP16_I512_BF16_I512_ACC1024_BF_ADDMAC;
def int_aie2ps_fp16_I512_bf16_I512_ACC1024_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_bf16_I512_ACC1024_bf_mac_conf">, AIE2PS_FP16_I512_BF16_I512_ACC1024_BF_MAC;
def int_aie2ps_fp16_I512_bf16_I512_ACC1024_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_bf16_I512_ACC1024_bf_msc_conf">, AIE2PS_FP16_I512_BF16_I512_ACC1024_BF_MAC;
def int_aie2ps_fp16_I512_bf16_I512_ACC1024_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_bf16_I512_ACC1024_bf_mul_conf">, AIE2PS_FP16_I512_BF16_I512_ACC1024_BF_MUL;
def int_aie2ps_fp16_I512_bf16_I512_ACC1024_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_bf16_I512_ACC1024_bf_negmul_conf">, AIE2PS_FP16_I512_BF16_I512_ACC1024_BF_MUL;
def int_aie2ps_fp16_I512_fp16_ACC1024_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_fp16_ACC1024_bf_addmac_conf">, AIE2PS_FP16_I512_FP16_ACC1024_BF_ADDMAC;
def int_aie2ps_fp16_I512_fp16_ACC1024_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_fp16_ACC1024_bf_addmsc_conf">, AIE2PS_FP16_I512_FP16_ACC1024_BF_ADDMAC;
def int_aie2ps_fp16_I512_fp16_ACC1024_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_fp16_ACC1024_bf_mac_conf">, AIE2PS_FP16_I512_FP16_ACC1024_BF_MAC;
def int_aie2ps_fp16_I512_fp16_ACC1024_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_fp16_ACC1024_bf_msc_conf">, AIE2PS_FP16_I512_FP16_ACC1024_BF_MAC;
def int_aie2ps_fp16_I512_fp16_ACC1024_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_fp16_ACC1024_bf_mul_conf">, AIE2PS_FP16_I512_FP16_ACC1024_BF_MUL;
def int_aie2ps_fp16_I512_fp16_ACC1024_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_fp16_ACC1024_bf_negmul_conf">, AIE2PS_FP16_I512_FP16_ACC1024_BF_MUL;
def int_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_addmac_conf">, AIE2PS_FP16_I512_FP16_I1024_ACC1024_BF_ADDMAC;
def int_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_addmsc_conf">, AIE2PS_FP16_I512_FP16_I1024_ACC1024_BF_ADDMAC;
def int_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_mac_conf">, AIE2PS_FP16_I512_FP16_I1024_ACC1024_BF_MAC;
def int_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_msc_conf">, AIE2PS_FP16_I512_FP16_I1024_ACC1024_BF_MAC;
def int_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_mul_conf">, AIE2PS_FP16_I512_FP16_I1024_ACC1024_BF_MUL;
def int_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_fp16_I1024_ACC1024_bf_negmul_conf">, AIE2PS_FP16_I512_FP16_I1024_ACC1024_BF_MUL;
def int_aie2ps_fp16_I512_fp16_I512_ACC1024_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_fp16_I512_ACC1024_bf_addmac_conf">, AIE2PS_FP16_I512_FP16_I512_ACC1024_BF_ADDMAC;
def int_aie2ps_fp16_I512_fp16_I512_ACC1024_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_fp16_I512_ACC1024_bf_addmsc_conf">, AIE2PS_FP16_I512_FP16_I512_ACC1024_BF_ADDMAC;
def int_aie2ps_fp16_I512_fp16_I512_ACC1024_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_fp16_I512_ACC1024_bf_mac_conf">, AIE2PS_FP16_I512_FP16_I512_ACC1024_BF_MAC;
def int_aie2ps_fp16_I512_fp16_I512_ACC1024_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_fp16_I512_ACC1024_bf_msc_conf">, AIE2PS_FP16_I512_FP16_I512_ACC1024_BF_MAC;
def int_aie2ps_fp16_I512_fp16_I512_ACC1024_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_fp16_I512_ACC1024_bf_mul_conf">, AIE2PS_FP16_I512_FP16_I512_ACC1024_BF_MUL;
def int_aie2ps_fp16_I512_fp16_I512_ACC1024_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_fp16_I512_fp16_I512_ACC1024_bf_negmul_conf">, AIE2PS_FP16_I512_FP16_I512_ACC1024_BF_MUL;
def int_aie2ps_fp8_I512_fp8_I512_ACC2048_bf_addmac_conf : ClangBuiltin<"__builtin_aie2ps_fp8_I512_fp8_I512_ACC2048_bf_addmac_conf">, AIE2PS_FP8_I512_FP8_I512_ACC2048_BF_ADDMAC;
def int_aie2ps_fp8_I512_fp8_I512_ACC2048_bf_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_fp8_I512_fp8_I512_ACC2048_bf_addmsc_conf">, AIE2PS_FP8_I512_FP8_I512_ACC2048_BF_ADDMAC;
def int_aie2ps_fp8_I512_fp8_I512_ACC2048_bf_mac_conf : ClangBuiltin<"__builtin_aie2ps_fp8_I512_fp8_I512_ACC2048_bf_mac_conf">, AIE2PS_FP8_I512_FP8_I512_ACC2048_BF_MAC;
def int_aie2ps_fp8_I512_fp8_I512_ACC2048_bf_msc_conf : ClangBuiltin<"__builtin_aie2ps_fp8_I512_fp8_I512_ACC2048_bf_msc_conf">, AIE2PS_FP8_I512_FP8_I512_ACC2048_BF_MAC;
def int_aie2ps_fp8_I512_fp8_I512_ACC2048_bf_mul_conf : ClangBuiltin<"__builtin_aie2ps_fp8_I512_fp8_I512_ACC2048_bf_mul_conf">, AIE2PS_FP8_I512_FP8_I512_ACC2048_BF_MUL;
def int_aie2ps_fp8_I512_fp8_I512_ACC2048_bf_negmul_conf : ClangBuiltin<"__builtin_aie2ps_fp8_I512_fp8_I512_ACC2048_bf_negmul_conf">, AIE2PS_FP8_I512_FP8_I512_ACC2048_BF_MUL;
def int_aie2ps_I1024_I1024_ACC2048_addmac_conf : ClangBuiltin<"__builtin_aie2ps_I1024_I1024_ACC2048_addmac_conf">, AIE2PS_I1024_I1024_ACC2048_ADDMAC;
def int_aie2ps_I1024_I1024_ACC2048_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_I1024_I1024_ACC2048_addmsc_conf">, AIE2PS_I1024_I1024_ACC2048_ADDMAC;
def int_aie2ps_I1024_I1024_ACC2048_mac_conf : ClangBuiltin<"__builtin_aie2ps_I1024_I1024_ACC2048_mac_conf">, AIE2PS_I1024_I1024_ACC2048_MAC;
def int_aie2ps_I1024_I1024_ACC2048_msc_conf : ClangBuiltin<"__builtin_aie2ps_I1024_I1024_ACC2048_msc_conf">, AIE2PS_I1024_I1024_ACC2048_MAC;
def int_aie2ps_I1024_I1024_ACC2048_mul_conf : ClangBuiltin<"__builtin_aie2ps_I1024_I1024_ACC2048_mul_conf">, AIE2PS_I1024_I1024_ACC2048_MUL;
def int_aie2ps_I1024_I1024_ACC2048_negmul_conf : ClangBuiltin<"__builtin_aie2ps_I1024_I1024_ACC2048_negmul_conf">, AIE2PS_I1024_I1024_ACC2048_MUL;
def int_aie2ps_I1024_I512_ACC2048_addmac_conf : ClangBuiltin<"__builtin_aie2ps_I1024_I512_ACC2048_addmac_conf">, AIE2PS_I1024_I512_ACC2048_ADDMAC;
def int_aie2ps_I1024_I512_ACC2048_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_I1024_I512_ACC2048_addmsc_conf">, AIE2PS_I1024_I512_ACC2048_ADDMAC;
def int_aie2ps_I1024_I512_ACC2048_mac_conf : ClangBuiltin<"__builtin_aie2ps_I1024_I512_ACC2048_mac_conf">, AIE2PS_I1024_I512_ACC2048_MAC;
def int_aie2ps_I1024_I512_ACC2048_msc_conf : ClangBuiltin<"__builtin_aie2ps_I1024_I512_ACC2048_msc_conf">, AIE2PS_I1024_I512_ACC2048_MAC;
def int_aie2ps_I1024_I512_ACC2048_mul_conf : ClangBuiltin<"__builtin_aie2ps_I1024_I512_ACC2048_mul_conf">, AIE2PS_I1024_I512_ACC2048_MUL;
def int_aie2ps_I1024_I512_ACC2048_negmul_conf : ClangBuiltin<"__builtin_aie2ps_I1024_I512_ACC2048_negmul_conf">, AIE2PS_I1024_I512_ACC2048_MUL;
def int_aie2ps_I1024_short_ACC2048_addmac_conf : ClangBuiltin<"__builtin_aie2ps_I1024_short_ACC2048_addmac_conf">, AIE2PS_I1024_SHORT_ACC2048_ADDMAC;
def int_aie2ps_I1024_short_ACC2048_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_I1024_short_ACC2048_addmsc_conf">, AIE2PS_I1024_SHORT_ACC2048_ADDMAC;
def int_aie2ps_I1024_short_ACC2048_mac_conf : ClangBuiltin<"__builtin_aie2ps_I1024_short_ACC2048_mac_conf">, AIE2PS_I1024_SHORT_ACC2048_MAC;
def int_aie2ps_I1024_short_ACC2048_msc_conf : ClangBuiltin<"__builtin_aie2ps_I1024_short_ACC2048_msc_conf">, AIE2PS_I1024_SHORT_ACC2048_MAC;
def int_aie2ps_I1024_short_ACC2048_mul_conf : ClangBuiltin<"__builtin_aie2ps_I1024_short_ACC2048_mul_conf">, AIE2PS_I1024_SHORT_ACC2048_MUL;
def int_aie2ps_I1024_short_ACC2048_negmul_conf : ClangBuiltin<"__builtin_aie2ps_I1024_short_ACC2048_negmul_conf">, AIE2PS_I1024_SHORT_ACC2048_MUL;
def int_aie2ps_I512_char_ACC2048_addmac_conf : ClangBuiltin<"__builtin_aie2ps_I512_char_ACC2048_addmac_conf">, AIE2PS_I512_CHAR_ACC2048_ADDMAC;
def int_aie2ps_I512_char_ACC2048_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_I512_char_ACC2048_addmsc_conf">, AIE2PS_I512_CHAR_ACC2048_ADDMAC;
def int_aie2ps_I512_char_ACC2048_mac_conf : ClangBuiltin<"__builtin_aie2ps_I512_char_ACC2048_mac_conf">, AIE2PS_I512_CHAR_ACC2048_MAC;
def int_aie2ps_I512_char_ACC2048_msc_conf : ClangBuiltin<"__builtin_aie2ps_I512_char_ACC2048_msc_conf">, AIE2PS_I512_CHAR_ACC2048_MAC;
def int_aie2ps_I512_char_ACC2048_mul_conf : ClangBuiltin<"__builtin_aie2ps_I512_char_ACC2048_mul_conf">, AIE2PS_I512_CHAR_ACC2048_MUL;
def int_aie2ps_I512_char_ACC2048_negmul_conf : ClangBuiltin<"__builtin_aie2ps_I512_char_ACC2048_negmul_conf">, AIE2PS_I512_CHAR_ACC2048_MUL;
def int_aie2ps_I512_I1024_ACC2048_addmac_conf : ClangBuiltin<"__builtin_aie2ps_I512_I1024_ACC2048_addmac_conf">, AIE2PS_I512_I1024_ACC2048_ADDMAC;
def int_aie2ps_I512_I1024_ACC2048_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_I512_I1024_ACC2048_addmsc_conf">, AIE2PS_I512_I1024_ACC2048_ADDMAC;
def int_aie2ps_I512_I1024_ACC2048_mac_conf : ClangBuiltin<"__builtin_aie2ps_I512_I1024_ACC2048_mac_conf">, AIE2PS_I512_I1024_ACC2048_MAC;
def int_aie2ps_I512_I1024_ACC2048_msc_conf : ClangBuiltin<"__builtin_aie2ps_I512_I1024_ACC2048_msc_conf">, AIE2PS_I512_I1024_ACC2048_MAC;
def int_aie2ps_I512_I1024_ACC2048_mul_conf : ClangBuiltin<"__builtin_aie2ps_I512_I1024_ACC2048_mul_conf">, AIE2PS_I512_I1024_ACC2048_MUL;
def int_aie2ps_I512_I1024_ACC2048_negmul_conf : ClangBuiltin<"__builtin_aie2ps_I512_I1024_ACC2048_negmul_conf">, AIE2PS_I512_I1024_ACC2048_MUL;
def int_aie2ps_I512_I512_ACC1024_addmac_conf : ClangBuiltin<"__builtin_aie2ps_I512_I512_ACC1024_addmac_conf">, AIE2PS_I512_I512_ACC1024_ADDMAC;
def int_aie2ps_I512_I512_ACC1024_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_I512_I512_ACC1024_addmsc_conf">, AIE2PS_I512_I512_ACC1024_ADDMAC;
def int_aie2ps_I512_I512_ACC1024_mac_conf : ClangBuiltin<"__builtin_aie2ps_I512_I512_ACC1024_mac_conf">, AIE2PS_I512_I512_ACC1024_MAC;
def int_aie2ps_I512_I512_ACC1024_msc_conf : ClangBuiltin<"__builtin_aie2ps_I512_I512_ACC1024_msc_conf">, AIE2PS_I512_I512_ACC1024_MAC;
def int_aie2ps_I512_I512_ACC1024_mul_conf : ClangBuiltin<"__builtin_aie2ps_I512_I512_ACC1024_mul_conf">, AIE2PS_I512_I512_ACC1024_MUL;
def int_aie2ps_I512_I512_ACC1024_negmul_conf : ClangBuiltin<"__builtin_aie2ps_I512_I512_ACC1024_negmul_conf">, AIE2PS_I512_I512_ACC1024_MUL;
def int_aie2ps_I512_I512_ACC2048_addmac_conf : ClangBuiltin<"__builtin_aie2ps_I512_I512_ACC2048_addmac_conf">, AIE2PS_I512_I512_ACC2048_ADDMAC;
def int_aie2ps_I512_I512_ACC2048_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_I512_I512_ACC2048_addmsc_conf">, AIE2PS_I512_I512_ACC2048_ADDMAC;
def int_aie2ps_I512_I512_ACC2048_mac_conf : ClangBuiltin<"__builtin_aie2ps_I512_I512_ACC2048_mac_conf">, AIE2PS_I512_I512_ACC2048_MAC;
def int_aie2ps_I512_I512_ACC2048_msc_conf : ClangBuiltin<"__builtin_aie2ps_I512_I512_ACC2048_msc_conf">, AIE2PS_I512_I512_ACC2048_MAC;
def int_aie2ps_I512_I512_ACC2048_mul_conf : ClangBuiltin<"__builtin_aie2ps_I512_I512_ACC2048_mul_conf">, AIE2PS_I512_I512_ACC2048_MUL;
def int_aie2ps_I512_I512_ACC2048_negmul_conf : ClangBuiltin<"__builtin_aie2ps_I512_I512_ACC2048_negmul_conf">, AIE2PS_I512_I512_ACC2048_MUL;
def int_aie2ps_I512_short_ACC2048_addmac_conf : ClangBuiltin<"__builtin_aie2ps_I512_short_ACC2048_addmac_conf">, AIE2PS_I512_SHORT_ACC2048_ADDMAC;
def int_aie2ps_I512_short_ACC2048_addmsc_conf : ClangBuiltin<"__builtin_aie2ps_I512_short_ACC2048_addmsc_conf">, AIE2PS_I512_SHORT_ACC2048_ADDMAC;
def int_aie2ps_I512_short_ACC2048_mac_conf : ClangBuiltin<"__builtin_aie2ps_I512_short_ACC2048_mac_conf">, AIE2PS_I512_SHORT_ACC2048_MAC;
def int_aie2ps_I512_short_ACC2048_msc_conf : ClangBuiltin<"__builtin_aie2ps_I512_short_ACC2048_msc_conf">, AIE2PS_I512_SHORT_ACC2048_MAC;
def int_aie2ps_I512_short_ACC2048_mul_conf : ClangBuiltin<"__builtin_aie2ps_I512_short_ACC2048_mul_conf">, AIE2PS_I512_SHORT_ACC2048_MUL;
def int_aie2ps_I512_short_ACC2048_negmul_conf : ClangBuiltin<"__builtin_aie2ps_I512_short_ACC2048_negmul_conf">, AIE2PS_I512_SHORT_ACC2048_MUL;

// VSHUFFLE
class AIE2PSVShuffleBFP640
      : DefaultAttrsIntrinsic<[llvm_v16i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], 
      [llvm_v16i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v16i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
class AIE2PSVShuffleBFP768
      : DefaultAttrsIntrinsic<[llvm_v8i32_ty, llvm_v8i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], 
      [llvm_v8i32_ty, llvm_v8i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, 
      llvm_v8i32_ty, llvm_v8i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;

// BFP FIFO Loads
class AIE2PSFIFO_LD_POP_BFP640 : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_v16i32_ty, llvm_v2i32_ty, llvm_v2i32_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly]>;

class AIE2PSFIFO_LD_POP_BFP768 : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_v8i32_ty, llvm_v8i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly]>;

class AIE2PSFIFO_LD_POP_1D_BFP640 : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_v16i32_ty, llvm_v2i32_ty, llvm_v2i32_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty], [IntrReadMem, IntrArgMemOnly]>;
class AIE2PSFIFO_LD_POP_1D_BFP768 : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_v8i32_ty, llvm_v8i32_ty,
     llvm_v2i32_ty, llvm_v2i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty], [IntrReadMem, IntrArgMemOnly]>;

class AIE2PSFIFO_LD_POP_2D_BFP640 : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty,
     llvm_v16i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty,
     llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty], [IntrReadMem, IntrArgMemOnly]>;
class AIE2PSFIFO_LD_POP_2D_BFP768 : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty, llvm_v8i32_ty, llvm_v8i32_ty,
     llvm_v2i32_ty, llvm_v2i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty], [IntrReadMem, IntrArgMemOnly]>;

class AIE2PSFIFO_LD_POP_3D_BFP640 : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty, llvm_i20_ty,
     llvm_v16i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, 
     llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty], [IntrReadMem, IntrArgMemOnly]>;
class AIE2PSFIFO_LD_POP_3D_BFP768 : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty, llvm_i20_ty,
     llvm_v8i32_ty, llvm_v8i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], 
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, 
     llvm_i20_ty, llvm_i20_ty, llvm_i20_ty], [IntrReadMem, IntrArgMemOnly]>;

// BFP FIFO Stores
class AIE2PSFIFO_ST_PUSH_BFP384 : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, 
     llvm_v8i32_ty, llvm_v2i32_ty, llvm_i32_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly]>;
class AIE2PSFIFO_ST_PUSH_BFP640 : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, 
     llvm_v16i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrWriteMem, IntrArgMemOnly]>;
class AIE2PSFIFO_ST_PUSH_BFP768 : DefaultAttrsIntrinsic<[llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty],
     [llvm_anyptr_ty, llvm_v32i32_ty, llvm_i32_ty, llvm_v8i32_ty, llvm_v8i32_ty, 
     llvm_v2i32_ty, llvm_v2i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], 
     [IntrWriteMem, IntrArgMemOnly]>;


class AIE2PSV2I32V64I8V64I8 : DefaultAttrsIntrinsic<[llvm_v2i32_ty], [llvm_v64i8_ty, llvm_v64i8_ty], [IntrNoMem]>;
class AIE2PSV64I8I64V64I8V64I8 : DefaultAttrsIntrinsic<[llvm_v64i8_ty, llvm_v2i32_ty], [llvm_v64i8_ty, llvm_v64i8_ty], [IntrNoMem]>;

//scl2vec::shift-shiftx-shift_bytes
def int_aie2ps_vshift_I512_I512 : ClangBuiltin<"__builtin_aie2ps_vshift_I512_I512">, AIE2PVShift_I512_I512;
def int_aie2ps_vshift_bf512_bf512 : ClangBuiltin<"__builtin_aie2ps_vshift_bf512_bf512">, AIE2PVShift_bf512_bf512;
//scl2vec::insert
def int_aie2ps_vinsert32_bf512 : ClangBuiltin<"__builtin_aie2ps_vinsert_bf32_bf512">, AIE2PVInsert32_bf512;
def int_aie2ps_vinsert64_bf512 : ClangBuiltin<"__builtin_aie2ps_vinsert_bf64_bf512">, AIE2PVInsert64_bf512;

//scl2vec::broadcast:broadcast_zero:broadcast_one
def int_aie2ps_vbroadcast32_bf512 : ClangBuiltin<"__builtin_aie2ps_vbroadcast_bf32_bf512">, AIE2PVBCST32bf512;
def int_aie2ps_vbroadcast64_bf512 : ClangBuiltin<"__builtin_aie2ps_vbroadcast_bf64_bf512">, AIE2PVBCST64bf512;
//scl2vec::broadcast_elem
def int_aie2ps_vextract_broadcast128_I512 : ClangBuiltin<"__builtin_aie2ps_vextract_broadcast128_I512">, AIE2PVBCST128I512;

//scl2vec::vbcstshfl
def int_aie2ps_vbcst_shuffle8  : ClangBuiltin<"__builtin_aie2ps_vbcst_shuffle8">, AIE2PVBcstShfl_I32;
def int_aie2ps_vbcst_shuffle16 : ClangBuiltin<"__builtin_aie2ps_vbcst_shuffle16">, AIE2PVBcstShfl_I32;
def int_aie2ps_vbcst_shuffle32 : ClangBuiltin<"__builtin_aie2ps_vbcst_shuffle32">, AIE2PVBcstShfl_I32;
def int_aie2ps_vbcst_shuffle64 : ClangBuiltin<"__builtin_aie2ps_vbcst_shuffle64">, AIE2PVBcstShfl_I64;

//shuffle
def int_aie2ps_vshuffle : ClangBuiltin<"__builtin_aie2ps_vshuffle">, AIE2PVShuffle;
def int_aie2ps_vshuffle_BFP640_BFP640_BFP640 :  AIE2PSVShuffleBFP640;
def int_aie2ps_vshuffle_BFP768_BFP768_BFP768 :  AIE2PSVShuffleBFP768;

// Mode Settings
// Set Control Registers
def int_aie2ps_set_ctrl_reg : ClangBuiltin<"__builtin_aie2ps_set_ctrl_reg">, AIE2PSetCtrlReg;
// Get Control Registers
def int_aie2ps_get_ctrl_reg : ClangBuiltin<"__builtin_aie2ps_get_ctrl_reg">, AIE2PGetCtrlReg;

// Set Status Registers
def int_aie2ps_set_status_reg : ClangBuiltin<"__builtin_aie2ps_set_status_reg">, AIE2PSetStatusReg;
// Get Status Registers
def int_aie2ps_get_status_reg : ClangBuiltin<"__builtin_aie2ps_get_status_reg">, AIE2PGetStatusReg;

//ups
def int_aie2ps_acc32_v16_I256_ups : ClangBuiltin<"__builtin_aie2ps_acc32_v16_I256_ups">,  AIE2PAcc32V16I256I;
def int_aie2ps_acc32_v32_I256_ups : ClangBuiltin<"__builtin_aie2ps_acc32_v32_I256_ups">,  AIE2PAcc32V32I256I;
def int_aie2ps_acc32_v32_I512_ups : ClangBuiltin<"__builtin_aie2ps_acc32_v32_I512_ups">,  AIE2PAcc32V32I512I;
def int_aie2ps_acc64_v16_I256_ups : ClangBuiltin<"__builtin_aie2ps_acc64_v16_I256_ups">,  AIE2PAcc64V16I256I;
def int_aie2ps_acc64_v16_I512_ups : ClangBuiltin<"__builtin_aie2ps_acc64_v16_I512_ups">,  AIE2PAcc64V16I512I;
def int_aie2ps_acc64_v8_I256_ups  : ClangBuiltin<"__builtin_aie2ps_acc64_v8_I256_ups">,   AIE2PAcc64V8I256I;
def int_aie2ps_acc32_v64_I512_ups : ClangBuiltin<"__builtin_aie2ps_acc32_v64_I512_ups">,  AIE2PAcc32V64I512I;
def int_aie2ps_acc64_v32_I512_ups : ClangBuiltin<"__builtin_aie2ps_acc64_v32_I512_ups">,  AIE2PAcc64V32I512I;

class AIE2PSI32V32FP16V32FP16 : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_v32f16_ty, llvm_v32f16_ty], [IntrNoMem]>;
class AIE2PSV32FP16I32V32FP16V32FP16I32 : DefaultAttrsIntrinsic<[llvm_v32f16_ty, llvm_i32_ty], [llvm_v32f16_ty, llvm_v32f16_ty], [IntrNoMem]>;

// v16bfloat16 to v16accfloat and vice-versa
def int_aie2ps_v16accfloat_to_v16bf16 : ClangBuiltin<"__builtin_aie2ps_v16accfloat_to_v16bf16">, AIE2PV16AccFloatToV16Bf;
def int_aie2ps_v16bf16_to_v16accfloat : ClangBuiltin<"__builtin_aie2ps_v16bf16_to_v16accfloat">, AIE2PV16BfToV16AccFloat;

// v32bfloat16 to v32accfloat and vice-versa
def int_aie2ps_v32accfloat_to_v32bf16 : ClangBuiltin<"__builtin_aie2ps_v32accfloat_to_v32bf16">, AIE2PV32AccFloatToV32Bf;
def int_aie2ps_v32bf16_to_v32accfloat : ClangBuiltin<"__builtin_aie2ps_v32bf16_to_v32accfloat">, AIE2PV32BfToV32AccFloat;

// v16float16 to v16accfloat and vice-versa
def int_aie2ps_v16accfloat_to_v16float16 : ClangBuiltin<"__builtin_aie2ps_v16accfloat_to_v16float16">, AIE2PSV16AccFloatToV16Float16;
def int_aie2ps_v16float16_to_v16accfloat : ClangBuiltin<"__builtin_aie2ps_v16float16_to_v16accfloat">, AIE2PSV16Float16ToV16AccFloat;

// v32float16 to v32accfloat and vice-versa
def int_aie2ps_v32accfloat_to_v32float16 : ClangBuiltin<"__builtin_aie2ps_v32accfloat_to_v32float16">, AIE2PSV32AccFloatToV32Float16;
def int_aie2ps_v32float16_to_v32accfloat : ClangBuiltin<"__builtin_aie2ps_v32float16_to_v32accfloat">, AIE2PSV32Float16ToV32AccFloat;

//srs
def int_aie2ps_I256_v16_acc32_srs : ClangBuiltin<"__builtin_aie2ps_I256_v16_acc32_srs">, AIE2PI256V16Acc32I;
def int_aie2ps_I256_v16_acc64_srs : ClangBuiltin<"__builtin_aie2ps_I256_v16_acc64_srs">, AIE2PI256V16Acc64I;
def int_aie2ps_I256_v32_acc32_srs : ClangBuiltin<"__builtin_aie2ps_I256_v32_acc32_srs">, AIE2PI256V32Acc32I;
def int_aie2ps_I256_v8_acc64_srs  : ClangBuiltin<"__builtin_aie2ps_I256_v8_acc64_srs">, AIE2PI256V8Acc64I;
def int_aie2ps_I512_v16_acc64_srs : ClangBuiltin<"__builtin_aie2ps_I512_v16_acc64_srs">, AIE2PI512V16Acc64I;
def int_aie2ps_I512_v32_acc32_srs : ClangBuiltin<"__builtin_aie2ps_I512_v32_acc32_srs">, AIE2PI512V32Acc32I;
def int_aie2ps_I512_v32_acc64_srs : ClangBuiltin<"__builtin_aie2ps_I512_v32_acc64_srs">, AIE2PI512V32Acc64I;
def int_aie2ps_I512_v64_acc32_srs : ClangBuiltin<"__builtin_aie2ps_I512_v64_acc32_srs">, AIE2PI512V64Acc32I;

def int_aie2ps_v32accfloat_to_v32float8  : ClangBuiltin<"__builtin_aie2ps_v32accfloat_to_v32float8">, AIE2PSV32AccFloatToV32Float8;
def int_aie2ps_v32accfloat_to_v32bfloat8 : ClangBuiltin<"__builtin_aie2ps_v32accfloat_to_v32bfloat8">, AIE2PSV32AccFloatToV32Bf8;

def int_aie2ps_v64accfloat_to_v64mx4 : AIE2PSV64AccFloatToV64MX6;
def int_aie2ps_v64accfloat_to_v64mx6 : AIE2PSV64AccFloatToV64MX6;
def int_aie2ps_v64accfloat_to_v64mx9 : AIE2PSV64AccFloatToV64MX9;

// vabs_gtz
def int_aie2ps_vabs_gtz8  : AIE2PV64I8V2I32V64I8I32;
def int_aie2ps_vabs_gtz16 : AIE2PV32I16I32V32I16I32;
def int_aie2ps_vabs_gtz32 : AIE2PV16I32I32V16I32I32;

// vaddsub
def int_aie2ps_vaddsub8  : ClangBuiltin<"__builtin_aie2ps_vaddsub8">, AIE2PV64I8V64I8V64I8I64;
def int_aie2ps_vaddsub16 : ClangBuiltin<"__builtin_aie2ps_vaddsub16">, AIE2PV32I16V32I16V32I16I32;
def int_aie2ps_vaddsub32 : ClangBuiltin<"__builtin_aie2ps_vaddsub32">, AIE2PV16I32V16I32V16I32I32;

// vbneg_ltz
def int_aie2ps_vbneg_ltz8  : AIE2PV64I8V2I32V64I8;
def int_aie2ps_vbneg_ltz16 : AIE2PV32I16I32V32I16;
def int_aie2ps_vbneg_ltz32 : AIE2PV16I32I32V16I32;

// veqz
def int_aie2ps_veqz8  : ClangBuiltin<"__builtin_aie2ps_veqz8">, AIE2PV2I32V64I8;
def int_aie2ps_veqz16 : ClangBuiltin<"__builtin_aie2ps_veqz16">, AIE2PI32V32I16;
def int_aie2ps_veqz32 : ClangBuiltin<"__builtin_aie2ps_veqz32">, AIE2PI32V16I32;

// vge
def int_aie2ps_vge8  : ClangBuiltin<"__builtin_aie2ps_vge8">, AIE2PV2I32V64I8V64I8I32;
def int_aie2ps_vgefloat8  : ClangBuiltin<"__builtin_aie2ps_vgefloat8">, AIE2PSV2I32V64I8V64I8;
def int_aie2ps_vgebfloat8  : ClangBuiltin<"__builtin_aie2ps_vgebfloat8">, AIE2PSV2I32V64I8V64I8;
def int_aie2ps_vge16 : ClangBuiltin<"__builtin_aie2ps_vge16">, AIE2PI32V32I16V32I16I32;
def int_aie2ps_vge32 : ClangBuiltin<"__builtin_aie2ps_vge32">, AIE2PI32V16I32V16I32I32;
def int_aie2ps_vgebfloat16 : ClangBuiltin<"__builtin_aie2ps_vgebfloat16">, AIE2PI32V32BF16V32BF16;
def int_aie2ps_vgefloat16 : ClangBuiltin<"__builtin_aie2ps_vgefloat16">, AIE2PSI32V32FP16V32FP16;

// vlt
def int_aie2ps_vlt8  : ClangBuiltin<"__builtin_aie2ps_vlt8">, AIE2PV2I32V64I8V64I8I32;
def int_aie2ps_vltfloat8  : ClangBuiltin<"__builtin_aie2ps_vltfloat8">, AIE2PSV2I32V64I8V64I8;
def int_aie2ps_vltbfloat8  : ClangBuiltin<"__builtin_aie2ps_vltbfloat8">, AIE2PSV2I32V64I8V64I8;
def int_aie2ps_vlt16 : ClangBuiltin<"__builtin_aie2ps_vlt16">, AIE2PI32V32I16V32I16I32;
def int_aie2ps_vlt32 : ClangBuiltin<"__builtin_aie2ps_vlt32">, AIE2PI32V16I32V16I32I32;
def int_aie2ps_vltfloat16 : ClangBuiltin<"__builtin_aie2ps_vltfloat16">, AIE2PSI32V32FP16V32FP16;
def int_aie2ps_vltbfloat16 : ClangBuiltin<"__builtin_aie2ps_vltbfloat16">, AIE2PI32V32BF16V32BF16;

// vmaxdiff_lt
def int_aie2ps_vmaxdiff_lt8  : AIE2PV64I8I64V64I8V64I8I32;
def int_aie2ps_vmaxdiff_lt16 : AIE2PV32I16I32V32I16V32I16I32;
def int_aie2ps_vmaxdiff_lt32 : AIE2PV16I32I32V16I32V16I32I32;

// vmax_lt
def int_aie2ps_vmax_lt8  : AIE2PV64I8I64V64I8V64I8I32;
def int_aie2ps_vmax_ltfloat8  : AIE2PSV64I8I64V64I8V64I8;
def int_aie2ps_vmax_ltbfloat8  : AIE2PSV64I8I64V64I8V64I8;
def int_aie2ps_vmax_lt16 : AIE2PV32I16I32V32I16V32I16I32;
def int_aie2ps_vmax_lt32 : AIE2PV16I32I32V16I32V16I32I32;
def int_aie2ps_vmax_ltfloat16  : AIE2PSV32FP16I32V32FP16V32FP16I32;
def int_aie2ps_vmax_ltbfloat16  : AIE2PV32BF16I32V32BF16V32BF16I32;

// vmin_ge
def int_aie2ps_vmin_ge8  : AIE2PV64I8I64V64I8V64I8I32;
def int_aie2ps_vmin_gefloat8  : AIE2PSV64I8I64V64I8V64I8;
def int_aie2ps_vmin_gebfloat8  : AIE2PSV64I8I64V64I8V64I8;
def int_aie2ps_vmin_ge16 : AIE2PV32I16I32V32I16V32I16I32;
def int_aie2ps_vmin_ge32 : AIE2PV16I32I32V16I32V16I32I32;
def int_aie2ps_vmin_gefloat16 : AIE2PSV32FP16I32V32FP16V32FP16I32;
def int_aie2ps_vmin_gebfloat16 : AIE2PV32BF16I32V32BF16V32BF16I32;

// vneg_gtz
def int_aie2ps_vneg_gtz8  : AIE2PV64I8V2I32V64I8;
def int_aie2ps_vneg_gtz16 : AIE2PV32I16I32V32I16;
def int_aie2ps_vneg_gtz32 : AIE2PV16I32I32V16I32;

// vsel
def int_aie2ps_vsel8  : ClangBuiltin<"__builtin_aie2ps_vsel8">, AIE2PV64I8V64I8V64I8I64;
def int_aie2ps_vsel16 : ClangBuiltin<"__builtin_aie2ps_vsel16">, AIE2PV32I16V32I16V32I16I32;
def int_aie2ps_vsel32 : ClangBuiltin<"__builtin_aie2ps_vsel32">, AIE2PV16I32V16I32V16I32I32;

// vsub_ge
def int_aie2ps_vsub_ge8  : AIE2PV64I8I64V64I8V64I8I32;
def int_aie2ps_vsub_ge16 : AIE2PV32I16I32V32I16V32I16I32;
def int_aie2ps_vsub_ge32 : AIE2PV16I32I32V16I32V16I32I32;

// vsub_lt
def int_aie2ps_vsub_lt8  : AIE2PV64I8I64V64I8V64I8I32;
def int_aie2ps_vsub_lt16 : AIE2PV32I16I32V32I16V32I16I32;
def int_aie2ps_vsub_lt32 : AIE2PV16I32I32V16I32V16I32I32;

// vector bfloat16 to vector int
def int_aie2ps_v16bf16_to_v16i32  : ClangBuiltin<"__builtin_aie2ps_v16bf16_to_v16i32">, AIE2PV16BF16ToV16I;

// Streams
// Cascade stream read
def int_aie2ps_scd_read_vec : ClangBuiltin<"__builtin_aie2ps_scd_read_vec">, AIE2PV16I32I32I32;
def int_aie2ps_scd_read_acc32 : ClangBuiltin<"__builtin_aie2ps_scd_read_acc32">, AIE2PV16Acc32I32I32;
def int_aie2ps_scd_expand_lo : ClangBuiltin<"__builtin_aie2ps_scd_expand_lo">, AIE2PV32Acc32I32;
def int_aie2ps_scd_expand_hi : ClangBuiltin<"__builtin_aie2ps_scd_expand_hi">, AIE2PV32Acc32I32;

// Cascade stream write
def int_aie2ps_mcd_write_vec : ClangBuiltin<"__builtin_aie2ps_mcd_write_vec">, AIE2PvV16I32I32;
def int_aie2ps_mcd_write_acc32 : ClangBuiltin<"__builtin_aie2ps_mcd_write_acc32">, AIE2PvV8Acc64I32;

// Scalar stream read
def int_aie2ps_get_ss : AIE2P_get_ss;
def int_aie2ps_get_ss_nb : AIE2P_get_ss;

// Scalar stream write with tlast
def int_aie2ps_put_ms : ClangBuiltin<"__builtin_aie2ps_put_ms">, AIE2P_put_ms;
def int_aie2ps_put_ms_nb : AIE2P_put_ms_nb;

// Read data into accumulator lane
def int_aie2ps_scd_expand_ACC1024 : ClangBuiltin<"__builtin_aie2ps_scd_expand_ACC1024">, AIE2PV32ACC32I32I32;
def int_aie2ps_scd_ACC2048 : ClangBuiltin<"__builtin_aie2ps_scd_ACC2048">, AIE2PV64ACC32I32I32;
def int_aie2ps_scd_expand_ACC2048 : ClangBuiltin<"__builtin_aie2ps_scd_expand_ACC2048">, AIE2PV64ACC32I32I32;
def int_aie2ps_scd_expand_ACC1024_incr: AIE2PV16ACC64I32I32;
def int_aie2ps_scd_expand_ACC2048_incr: AIE2PV32ACC64I32I32;

// Addressing intrinsics
def int_aie2ps_add_2d : AIE2PAdd2D;
def int_aie2ps_add_3d : AIE2PAdd3D;

// LOAD_4X
def int_aie2ps_load_4x16_lo : ClangBuiltin<"__builtin_aie2ps_load_4x16_lo">, AIE2PLOAD4X;
def int_aie2ps_load_4x16_hi : ClangBuiltin<"__builtin_aie2ps_load_4x16_hi">, AIE2PLOAD4X;
def int_aie2ps_load_4x32_lo : ClangBuiltin<"__builtin_aie2ps_load_4x32_lo">, AIE2PLOAD4X;
def int_aie2ps_load_4x32_hi : ClangBuiltin<"__builtin_aie2ps_load_4x32_hi">, AIE2PLOAD4X;
def int_aie2ps_load_4x64_lo : ClangBuiltin<"__builtin_aie2ps_load_4x64_lo">, AIE2PLOAD4X;
def int_aie2ps_load_4x64_hi : ClangBuiltin<"__builtin_aie2ps_load_4x64_hi">, AIE2PLOAD4X;

// Pack-Unpack
def int_aie2ps_pack_I512_I8_I16 : ClangBuiltin<"__builtin_aie2ps_pack_I512_I8_I16">, AIE2PPackI512I8I16;
def int_aie2ps_pack_I512_I4_I8 : ClangBuiltin<"__builtin_aie2ps_pack_I512_I4_I8">, AIE2PPackI512I4I8;
def int_aie2ps_unpack_I512_I16_I8 : ClangBuiltin<"__builtin_aie2ps_unpack_I512_I16_I8">, AIE2PUnPackI512I16I8;
def int_aie2ps_unpack_I512_I8_I4 : ClangBuiltin<"__builtin_aie2ps_unpack_I512_I8_I4">, AIE2PUnPackI512I8I4;
def int_aie2ps_pack_I1024_I8_I16 : ClangBuiltin<"__builtin_aie2ps_pack_I1024_I8_I16">, AIE2PPackI1024I8I16;
def int_aie2ps_pack_I1024_I4_I8 : ClangBuiltin<"__builtin_aie2ps_pack_I1024_I4_I8">, AIE2PPackI1024I4I8;
def int_aie2ps_unpack_I1024_I16_I8 : ClangBuiltin<"__builtin_aie2ps_unpack_I1024_I16_I8">, AIE2PUnPackI1024I16I8;
def int_aie2ps_unpack_I1024_I8_I4 : ClangBuiltin<"__builtin_aie2ps_unpack_I1024_I8_I4">, AIE2PUnPackI1024I8I4;

// FIFO Loads
def int_aie2ps_fifo_ld_fill: AIE2PFIFO_LD_FILL;
def int_aie2ps_fifo_ld_pop_512_unaligned: AIE2PFIFO_LD_POPUnaligned;
def int_aie2ps_fifo_ld_pop_BFP640: AIE2PSFIFO_LD_POP_BFP640;
def int_aie2ps_fifo_ld_pop_BFP768: AIE2PSFIFO_LD_POP_BFP768;

def int_aie2ps_fifo_ld_pop_1d_unaligned: AIE2PFIFO_LD_POPUnaligned1D;
def int_aie2ps_fifo_ld_pop_1d_BFP640: AIE2PSFIFO_LD_POP_1D_BFP640;
def int_aie2ps_fifo_ld_pop_1d_BFP768: AIE2PSFIFO_LD_POP_1D_BFP768;

def int_aie2ps_fifo_ld_pop_2d_unaligned: AIE2PFIFO_LD_POPUnaligned2D;
def int_aie2ps_fifo_ld_pop_2d_BFP640: AIE2PSFIFO_LD_POP_2D_BFP640;
def int_aie2ps_fifo_ld_pop_2d_BFP768: AIE2PSFIFO_LD_POP_2D_BFP768;

def int_aie2ps_fifo_ld_pop_3d_unaligned: AIE2PFIFO_LD_POPUnaligned3D;
def int_aie2ps_fifo_ld_pop_3d_BFP640: AIE2PSFIFO_LD_POP_3D_BFP640;
def int_aie2ps_fifo_ld_pop_3d_BFP768: AIE2PSFIFO_LD_POP_3D_BFP768;

def int_aie2ps_fifo_ld_fillx: AIE2PFIFO_LD_FILLX;
def int_aie2ps_fifo_ld_popx: AIE2PFIFO_LD_POPX;

// FIFO Stores
def int_aie2ps_fifo_st_push_512: AIE2PFIFO_ST_PUSH_512_BFP16;
def int_aie2ps_fifo_st_push_BFP384: AIE2PSFIFO_ST_PUSH_BFP384;
def int_aie2ps_fifo_st_push_BFP640: AIE2PSFIFO_ST_PUSH_BFP640;
def int_aie2ps_fifo_st_push_BFP768: AIE2PSFIFO_ST_PUSH_BFP768;

def int_aie2ps_fifo_st_flush: AIE2PFIFO_ST_FLUSH;
def int_aie2ps_fifo_st_flush_conv: AIE2PFIFO_ST_FLUSH;

def int_aie2ps_fifo_st_flush_1d: AIE2PFIFO_ST_FLUSH_1D;
def int_aie2ps_fifo_st_flush_1d_conv: AIE2PFIFO_ST_FLUSH_1D;

def int_aie2ps_fifo_st_flush_2d: AIE2PFIFO_ST_FLUSH_2D;
def int_aie2ps_fifo_st_flush_2d_conv: AIE2PFIFO_ST_FLUSH_2D;

def int_aie2ps_fifo_st_flush_3d: AIE2PFIFO_ST_FLUSH_3D;
def int_aie2ps_fifo_st_flush_3d_conv: AIE2PFIFO_ST_FLUSH_3D;

} // TargetPrefix = "aie2ps"
