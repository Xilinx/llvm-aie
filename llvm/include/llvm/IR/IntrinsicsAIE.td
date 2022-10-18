//===- IntrinsicsAIE.td - Defines AIE intrinsics -------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines all of the AIE-specific intrinsics.
//
//===----------------------------------------------------------------------===//

let TargetPrefix = "aie" in {

//===----------------------------------------------------------------------===//
class EventIntrinsic
    : Intrinsic<[],
                [llvm_i32_ty],
                [IntrHasSideEffects, IntrNoMem]>; //[IntrHasSideEffects, IntrNoMem, ImmArg<ArgIndex<0>>]>;
// Atomics
// FIXME: The flags for this are wrong.  However, when setting the
// flags "correctly", then clang -O3 complains.
class LockIntrinsic
    : Intrinsic<[],
                [llvm_i32_ty, llvm_i32_ty],
                []>; // IntrHasSideEffects, IntrNoMem]>; //, ImmArg<ArgIndex<1>>]>;
// class LockImmIntrinsic
//     : Intrinsic<[],
//                 [llvm_i32_ty, llvm_i32_ty],
//                 [IntrHasSideEffects, IntrNoMem, ImmArg<ArgIndex<0>>, ImmArg<ArgIndex<1>>]>;
class PacketHeaderIntrinsic
    : Intrinsic<[llvm_i32_ty],
                [llvm_i32_ty, llvm_i32_ty],
                []>; //IntrHasSideEffects, IntrNoMem]; //, ImmArg<ArgIndex<1>>, ImmArg<ArgIndex<2>>]>;
class CtrlPacketHeaderIntrinsic
    : Intrinsic<[llvm_i32_ty],
                [llvm_i20_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty],
                []>; //IntrHasSideEffects, IntrNoMem, ImmArg<ArgIndex<1>>, ImmArg<ArgIndex<2>>]>;
class StreamReadIntrinsic
    : Intrinsic<[llvm_i32_ty],
                [llvm_i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
                //[IntrHasSideEffects, IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
class StreamWriteIntrinsic
    : Intrinsic<[],
                [llvm_i32_ty, llvm_i32_ty],
                [IntrHasSideEffects, IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
class StreamWriteLastIntrinsic
    : Intrinsic<[],
                [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty],
                [IntrHasSideEffects, IntrNoMem, ImmArg<ArgIndex<0>>]>;
class StreamReadFIntrinsic
    : Intrinsic<[llvm_float_ty],
                [llvm_i32_ty],
                [IntrHasSideEffects, IntrNoMem, ImmArg<ArgIndex<0>>]>;
class StreamWriteFIntrinsic
    : Intrinsic<[],
                [llvm_i32_ty, llvm_float_ty],
                [IntrHasSideEffects, IntrNoMem, ImmArg<ArgIndex<0>>]>;
class StreamWriteFLastIntrinsic
    : Intrinsic<[],
                [llvm_i32_ty, llvm_float_ty, llvm_i32_ty],
                [IntrHasSideEffects, IntrNoMem, ImmArg<ArgIndex<0>>]>;

// Set/Get a bit in a value, mapped to a GPR
class BitSetIntrinsic
	 : Intrinsic<[llvm_i32_ty],
		          [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<2>>]>;
class BitGetIntrinsic
	 : Intrinsic<[llvm_i32_ty],
		          [llvm_i32_ty, llvm_i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<1>>]>;

// Set/Get a bit in a specific control/status register
class BitSetRegIntrinsic
	 : Intrinsic<[llvm_i32_ty],
		          [llvm_i32_ty, llvm_i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<1>>]>;
class BitGetRegIntrinsic
	 : Intrinsic<[llvm_i32_ty],
		          [llvm_i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;

// Non-linear functions
class NLFFltFltIntrinsic
    : Intrinsic<[llvm_float_ty],
                [llvm_float_ty],
                [IntrNoMem]>;
class NLFFltFixIntrinsic
    : Intrinsic<[llvm_float_ty],
                [llvm_i32_ty, llvm_i32_ty],
                [IntrNoMem]>;
class NLFFixFltIntrinsic
    : Intrinsic<[llvm_i32_ty],
                [llvm_i32_ty, llvm_float_ty],
                [IntrNoMem]>;
class NLFFixFixIntrinsic
    : Intrinsic<[llvm_i32_ty],
                [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty],
                [IntrNoMem]>;

class VecFP4Intrinsic
     : Intrinsic<[llvm_v4f32_ty], [], [IntrNoMem]>;
class VecFP8Intrinsic
     : Intrinsic<[llvm_v8f32_ty], [], [IntrNoMem]>;
class VecFP16Intrinsic
     : Intrinsic<[llvm_v16f32_ty], [], [IntrNoMem]>;
class VecFP32Intrinsic
     : Intrinsic<[llvm_v32f32_ty], [], [IntrNoMem]>;
//Vector Int
class VecV2I32Intrinsic
     : Intrinsic<[llvm_v2i32_ty], [], [IntrNoMem]>;
class VecV16I16Intrinsic
     : Intrinsic<[llvm_v16i16_ty], [], [IntrNoMem]>;
class VecV32I16Intrinsic
     : Intrinsic<[llvm_v32i16_ty], [], [IntrNoMem]>;
class VecV16I32Intrinsic
     : Intrinsic<[llvm_v16i32_ty], [], [IntrNoMem]>;
class VecV32I32Intrinsic
     : Intrinsic<[llvm_v32i32_ty], [], [IntrNoMem]>;
class VecV128I8Intrinsic
     : Intrinsic<[llvm_v128i8_ty], [], [IntrNoMem]>;

class VecV8I48Intrinsic
     : Intrinsic<[llvm_v8i48_ty], [], [IntrNoMem]>;

class VecV16I48Intrinsic
     : Intrinsic<[llvm_v16i48_ty], [], [IntrNoMem]>;

class UPDVV8I32Intrinsic
	 : Intrinsic<[llvm_v8i32_ty],
		          [llvm_v8i32_ty, llvm_v4i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;

class UPDWV16I32Intrinsic
	 : Intrinsic<[llvm_v16i32_ty],
		          [llvm_v16i32_ty, llvm_v8i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
class EXTVV8I32Intrinsic
	 : Intrinsic<[llvm_v4i32_ty],
		          [llvm_v8i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;

class EXTWV16I32Intrinsic
	 : Intrinsic<[llvm_v8i32_ty],
		          [llvm_v16i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
class EXTWV32I16Intrinsic
	 : Intrinsic<[llvm_v16i16_ty],
		          [llvm_v32i16_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
class ConcatV32I16Intrinsic
	 : Intrinsic<[llvm_v32i16_ty],
		          [llvm_v16i16_ty, llvm_v16i16_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
class ConcatV64I16Intrinsic
	 : Intrinsic<[llvm_v64i16_ty],
		          [llvm_v32i16_ty, llvm_v32i16_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;

class VLMUL4V32I32Intrinsic
	 : Intrinsic<[llvm_v8i48_ty],
		          [llvm_v32i32_ty, llvm_v8i32_ty, llvm_i32_ty, llvm_i32_ty,
                   llvm_i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
class VLMUL4V16I32Intrinsic
	 : Intrinsic<[llvm_v8i48_ty],
		          [llvm_v16i32_ty, llvm_v8i32_ty, llvm_i32_ty, llvm_i32_ty,
                   llvm_i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
class VLMUL8V32I32Intrinsic
	 : Intrinsic<[llvm_v16i48_ty],
		          [llvm_v32i32_ty, llvm_v8i32_ty, llvm_i32_ty, llvm_i32_ty,
                   llvm_i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
class VMUL16V32I32Intrinsic
	 : Intrinsic<[llvm_v16i48_ty],
		          [llvm_v32i32_ty, llvm_v16i16_ty, llvm_i32_ty, llvm_i32_ty,
                   llvm_i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;

class VMUL16V128I8Intrinsic
	 : Intrinsic<[llvm_v16i48_ty],
		          [llvm_v128i8_ty, llvm_v32i8_ty, llvm_i32_ty, llvm_i32_ty,
                   llvm_i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
//class VMUL16V128I8Intrinsic
//	 : Intrinsic<[llvm_v16i48_ty],
//		          [llvm_v128i8_ty, llvm_i32_ty, llvm_v2i32_ty, llvm_i32_ty,
  //                 llvm_v32i8_ty, llvm_i32_ty, llvm_v2i32_ty, llvm_v2i32_ty],
//					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
class VMAC16V32I32Intrinsic
	 : Intrinsic<[llvm_v16i48_ty],
		          [llvm_v32i32_ty, llvm_v16i16_ty, llvm_v16i48_ty, llvm_i32_ty, llvm_i32_ty,
                   llvm_i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
class VMAC16V64I16Intrinsic
	 : Intrinsic<[llvm_v16i48_ty],
		          [llvm_v64i16_ty, llvm_v8i32_ty, llvm_v16i48_ty, llvm_i32_ty, llvm_i32_ty,
                   llvm_i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
class VMAC16V128I8Intrinsic
	 : Intrinsic<[llvm_v16i48_ty],
		          [llvm_v128i8_ty, llvm_v32i8_ty, llvm_v16i48_ty, llvm_i32_ty, llvm_i32_ty,
                   llvm_i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
class VMAC80Intrinsic
	 : Intrinsic<[llvm_v16i48_ty],
		          [llvm_v8i64_ty, llvm_v32i32_ty, llvm_v16i48_ty, llvm_i32_ty, llvm_i32_ty,
                   llvm_i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
class VecFPMULIntrinsic
	 : Intrinsic<[llvm_v8f32_ty],
		          [llvm_v32f32_ty, llvm_i32_ty, llvm_i32_ty,
                   llvm_v8f32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
class VecFPMACIntrinsic
	 : Intrinsic<[llvm_v8f32_ty],
		          [llvm_v8f32_ty, llvm_v32f32_ty, llvm_i32_ty, llvm_i32_ty,
                                  llvm_v8f32_ty, llvm_i32_ty, llvm_i32_ty,
                                  llvm_i32_ty, llvm_i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;

// Floating point intrinsics with symmetric operand types
class VecFPSimpleMULIntrinsic
	 : Intrinsic<[llvm_v8f32_ty/*, llvm_i32_ty*/], [llvm_v8f32_ty, llvm_v8f32_ty,
                                   llvm_i32_ty, llvm_i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
class VecFPSimpleMACIntrinsic
	 : Intrinsic<[llvm_v8f32_ty/*, llvm_i32_ty*/], [llvm_v8f32_ty,
                                   llvm_v8f32_ty, llvm_v8f32_ty,
                                   llvm_i32_ty, llvm_i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
//srs
class BsrsV16I8V16Acc48
	 : Intrinsic<[llvm_v16i8_ty], [llvm_v16i48_ty, llvm_i32_ty],
					 [IntrNoMem]>; //, ImmArg<ArgIndex<0>>]>;
//compare intrinsic
class VCMPV32INT16
	 : Intrinsic<[llvm_v32i16_ty],
		          [llvm_v32i16_ty, llvm_i32_ty, llvm_i32_ty,
                           llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty],
					 [IntrNoMem]>;
class VCMPV32INT16_CMP
	 : Intrinsic<[llvm_v32i16_ty],
		          [llvm_v32i16_ty, llvm_i32_ty, llvm_i32_ty,
                           llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_i32_ty],
					 [IntrNoMem]>;
class VCMPV32INT16_SELECT
	 : Intrinsic<[llvm_v32i16_ty],
		          [llvm_v32i16_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty,
                           llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty],
					 [IntrNoMem]>;
//pac-unpack
class VPACKV16INT16
	 : Intrinsic<[llvm_v16i8_ty], [llvm_v16i16_ty],
					 [IntrNoMem]>;

def int_aie_event : ClangBuiltin<"__builtin_aie_event">, EventIntrinsic;
def int_aie_lock_acquire_reg :
	 ClangBuiltin<"__builtin_aie_acquire">,     LockIntrinsic;
def int_aie_lock_release_reg :
	 ClangBuiltin<"__builtin_aie_release">,     LockIntrinsic;
// def int_aie_lock_acquire_imm :
// 	 ClangBuiltin<"__builtin_aie_acquire_imm">, LockImmIntrinsic;
// def int_aie_lock_release_imm :
// 	 ClangBuiltin<"__builtin_aie_release_imm">, LockImmIntrinsic;

def int_aie_bitset : ClangBuiltin<"__builtin_aie_bitset">, BitSetIntrinsic;
def int_aie_bitget : ClangBuiltin<"__builtin_aie_bitget">, BitGetIntrinsic;
def int_aie_bitset_md0 : ClangBuiltin<"__builtin_aie_bitset_md0">, BitSetRegIntrinsic;
def int_aie_bitset_md1 : ClangBuiltin<"__builtin_aie_bitset_md1">, BitSetRegIntrinsic;
def int_aie_bitget_mc0 : ClangBuiltin<"__builtin_aie_bitget_mc0">, BitGetRegIntrinsic;
def int_aie_bitget_mc1 : ClangBuiltin<"__builtin_aie_bitget_mc1">, BitGetRegIntrinsic;

def int_aie_sqrt_flt_flt    : ClangBuiltin<"__builtin_aie_sqrt_flt_flt">,    NLFFltFltIntrinsic;
def int_aie_invsqrt_flt_flt : ClangBuiltin<"__builtin_aie_invsqrt_flt_flt">, NLFFltFltIntrinsic;
def int_aie_inv_flt_flt     : ClangBuiltin<"__builtin_aie_inv_flt_flt">,     NLFFltFltIntrinsic;
def int_aie_sqrt_fix_flt    : ClangBuiltin<"__builtin_aie_sqrt_fix_flt">,    NLFFixFltIntrinsic;
def int_aie_invsqrt_fix_flt : ClangBuiltin<"__builtin_aie_invsqrt_fix_flt">, NLFFixFltIntrinsic;
def int_aie_inv_fix_flt     : ClangBuiltin<"__builtin_aie_inv_fix_flt">,     NLFFixFltIntrinsic;
def int_aie_sqrt_flt_fix    : ClangBuiltin<"__builtin_aie_sqrt_flt_fix">,    NLFFltFixIntrinsic;
def int_aie_invsqrt_flt_fix : ClangBuiltin<"__builtin_aie_invsqrt_flt_fix">, NLFFltFixIntrinsic;
def int_aie_inv_flt_fix     : ClangBuiltin<"__builtin_aie_inv_flt_fix">,     NLFFltFixIntrinsic;
def int_aie_sqrt_fix_fix    : ClangBuiltin<"__builtin_aie_sqrt_fix_fix">,    NLFFixFixIntrinsic;
def int_aie_invsqrt_fix_fix : ClangBuiltin<"__builtin_aie_invsqrt_fix_fix">, NLFFixFixIntrinsic;
def int_aie_inv_fix_fix     : ClangBuiltin<"__builtin_aie_inv_fix_fix">,     NLFFixFixIntrinsic;

def int_aie_packet_header :
	 ClangBuiltin<"__builtin_aie_packet_header">, PacketHeaderIntrinsic;
def int_aie_ctrl_packet_header : CtrlPacketHeaderIntrinsic;
def int_aie_get_ss : ClangBuiltin<"__builtin_aie_get_ss">, StreamReadIntrinsic;
def int_aie_put_ms : ClangBuiltin<"__builtin_aie_put_ms">, StreamWriteIntrinsic;
def int_aie_getf_ss : ClangBuiltin<"__builtin_aie_getf_ss">, StreamReadFIntrinsic;
def int_aie_putf_ms : ClangBuiltin<"__builtin_aie_putf_ms">, StreamWriteFIntrinsic;

def int_aie_v4f32undef : ClangBuiltin<"__builtin_aie_v4f32undef">, VecFP4Intrinsic;
def int_aie_v8f32undef : ClangBuiltin<"__builtin_aie_v8f32undef">, VecFP8Intrinsic;
def int_aie_v16f32undef : ClangBuiltin<"__builtin_aie_v16f32undef">, VecFP16Intrinsic;
def int_aie_v32f32undef : ClangBuiltin<"__builtin_aie_v32f32undef">, VecFP32Intrinsic;
def int_aie_vfpmul : ClangBuiltin<"__builtin_aie_vfpmul">, VecFPMULIntrinsic;
def int_aie_vfpmac : ClangBuiltin<"__builtin_aie_vfpmac">, VecFPMACIntrinsic;
def int_aie_vfpsimplemul : ClangBuiltin<"__builtin_aie_vfpsimplemul">, VecFPSimpleMULIntrinsic;
def int_aie_vfpsimplemac : ClangBuiltin<"__builtin_aie_vfpsimplemac">, VecFPSimpleMACIntrinsic;
//def int_aie_vfmac : ClangBuiltin<"__builtin_aie_vfmac">, VecFPMACIntrinsic;

//Vector Int Intrinsic
def int_aie_v2i32undef : ClangBuiltin<"__builtin_aie_v2i32undef">, VecV2I32Intrinsic;
def int_aie_v16i16undef : ClangBuiltin<"__builtin_aie_v16i16undef">, VecV16I16Intrinsic;
def int_aie_v32i16undef : ClangBuiltin<"__builtin_aie_v32i16undef">, VecV32I16Intrinsic;
def int_aie_v16i32undef : ClangBuiltin<"__builtin_aie_v16i32undef">, VecV16I32Intrinsic;
def int_aie_v32i32undef : ClangBuiltin<"__builtin_aie_v32i32undef">, VecV32I32Intrinsic;
def int_aie_v128i8undef : ClangBuiltin<"__builtin_aie_v128i8undef">, VecV128I8Intrinsic;
def int_aie_v8i48undef : ClangBuiltin<"__builtin_aie_v8i48undef">, VecV8I48Intrinsic;
def int_aie_v16i48undef : ClangBuiltin<"__builtin_aie_v16i48undef">, VecV16I48Intrinsic;
//upd intrinsic
def int_aie_upd_v_v8i32_lo : ClangBuiltin<"__builtin_aie_upd_v_v8i32_lo">, UPDVV8I32Intrinsic;
def int_aie_upd_v_v8i32_hi : ClangBuiltin<"__builtin_aie_upd_v_v8i32_hi">, UPDVV8I32Intrinsic;
def int_aie_upd_w_v16i32_lo : ClangBuiltin<"__builtin_aie_upd_w_v16i32_lo">, UPDWV16I32Intrinsic;
def int_aie_upd_w_v16i32_hi : ClangBuiltin<"__builtin_aie_upd_w_v16i32_hi">, UPDWV16I32Intrinsic;
//ext intrinsic
def int_aie_ext_v_v8i32_lo : ClangBuiltin<"__builtin_aie_ext_v_v8i32_lo">, EXTVV8I32Intrinsic;
def int_aie_ext_v_v8i32_hi : ClangBuiltin<"__builtin_aie_ext_v_v8i32_hi">, EXTVV8I32Intrinsic;
def int_aie_ext_w_v16i32_lo : ClangBuiltin<"__builtin_aie_ext_w_v16i32_lo">, EXTWV16I32Intrinsic;
def int_aie_ext_w_v16i32_hi : ClangBuiltin<"__builtin_aie_ext_w_v16i32_hi">, EXTWV16I32Intrinsic;
def int_aie_ext_w_v32i16_lo : ClangBuiltin<"__builtin_aie_ext_w_v32i16_lo">, EXTWV32I16Intrinsic;
def int_aie_ext_w_v32i16_hi : ClangBuiltin<"__builtin_aie_ext_w_v32i16_hi">, EXTWV32I16Intrinsic;
//concat intrinsic
def int_aie_concat_v16i16 : ClangBuiltin<"__builtin_aie_concat_v16i16">, ConcatV32I16Intrinsic;
def int_aie_concat_v64i16 : ClangBuiltin<"__builtin_aie_concat_v64i16">, ConcatV64I16Intrinsic;
//mac,mul intrinsic
def int_aie_lmul4_v32int32 : ClangBuiltin<"__builtin_aie_mul4_v32int32_v8int32_am_sw80">, VLMUL4V32I32Intrinsic;
def int_aie_lmul4_v16int32 : ClangBuiltin<"__builtin_aie_mul4_v16int32_v8int32_am_sw80">, VLMUL4V16I32Intrinsic;
def int_aie_lmul8_v32int32 : ClangBuiltin<"__builtin_aie_mul8_v32int32_v8int32_bm_sw80">, VLMUL8V32I32Intrinsic;
def int_aie_mul16_v32int32 : ClangBuiltin<"__builtin_aie_mul16_v32int32_v16int16_bm_sw48">, VMUL16V32I32Intrinsic;
def int_aie_mul16_v128int8 : ClangBuiltin<"__builtin_aie_mul16_v128int8_v32int8_bm_sw48">, VMUL16V128I8Intrinsic;
def int_aie_mac16_v32int32 : ClangBuiltin<"__builtin_aie_mac16_v32int32_v16int16_bm_sw48">, VMAC16V32I32Intrinsic;
def int_aie_mac16_v64int16 : ClangBuiltin<"__builtin_aie_mac16_v64int16_v8int32_bm_sw48">, VMAC16V64I16Intrinsic;
def int_aie_mac16_v128int8 : ClangBuiltin<"__builtin_aie_mac16_v128int8_v32int8_bm_sw48">, VMAC16V128I8Intrinsic;
def int_aie_vmac80 : ClangBuiltin<"__builtin_aie_vmac80">, VMAC80Intrinsic;
//srs
def int_aie_bsrs_v16i8_v16acc48 : ClangBuiltin<"__builtin_aie_bsrs_v16i8">, BsrsV16I8V16Acc48;
def int_aie_ubsrs_v16i8_v16acc48 : ClangBuiltin<"__builtin_aie_ubsrs_v16i8">, BsrsV16I8V16Acc48;
//compare intrinsic
def int_aie_prim_v32int16 : ClangBuiltin<"__builtin_aie_prim_v32int16">, VCMPV32INT16;
def int_aie_prim_v32int16_cmp : ClangBuiltin<"__builtin_aie_prim_v32int16_cmp">, VCMPV32INT16_CMP;
def int_aie_prim_v32int16_select : ClangBuiltin<"__builtin_aie_prim_v32int16_select">, VCMPV32INT16_SELECT;
//pack-unpack intrinsic
def int_aie_pack_v16int16 : ClangBuiltin<"__builtin_aie_pack_v16int16">, VPACKV16INT16;
} // TargetPrefix = "aie"
