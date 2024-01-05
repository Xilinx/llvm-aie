//===- IntrinsicsAIE2.td - Defines AIE intrinsics -------*- tablegen -*-===//
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

//aie2
let TargetPrefix = "aie2" in {

//===----------------------------------------------------------------------===//
class AIE2EventIntrinsic
    : Intrinsic<[],
                [llvm_i32_ty],
                [IntrHasSideEffects, IntrNoMem]>; //[IntrHasSideEffects, IntrNoMem, ImmArg<ArgIndex<0>>]>;
//Semaphores
class AIE2LockIntr
     : Intrinsic<[],
                 [llvm_i32_ty /* unsigned */, llvm_i32_ty /* (un)signed */],
                 [IntrHasSideEffects, IntrNoMem]>;
class AIE2LockCondIntr
     : Intrinsic<[],
                 [llvm_i32_ty /* unsigned */, llvm_i32_ty /* (un)signed */, llvm_i32_ty /* (un)signed */],
                 [IntrHasSideEffects, IntrNoMem]>;
class AIE2DoneIntr
     : Intrinsic<[], [], [IntrHasSideEffects, IntrNoMem]>;

class AIEV2I512I512MULConf
	 : Intrinsic<[llvm_v16i64_ty], [llvm_v64i8_ty, llvm_v16i32_ty, llvm_i32_ty],
					 [IntrNoMem]>;
class AIEV2I512I512MACConf
	 : Intrinsic<[llvm_v16i64_ty], [llvm_v64i8_ty, llvm_v16i32_ty, llvm_v16i64_ty, llvm_i32_ty],
					 [IntrNoMem]>;
class AIEV2I512I512AddMaconf
	 : Intrinsic<[llvm_v16i64_ty], [llvm_v64i8_ty, llvm_v16i32_ty, llvm_v16i64_ty, llvm_v16i64_ty, llvm_i32_ty],
					 [IntrNoMem]>;

class AIE2bf16MULConf
    : Intrinsic<[llvm_v8i64_ty], [llvm_v32bf16_ty, llvm_v32bf16_ty, llvm_i32_ty],
		       [IntrNoMem]>;
class AIE2bf16MACConf
    : Intrinsic<[llvm_v8i64_ty], [llvm_v32bf16_ty, llvm_v32bf16_ty, llvm_v8i64_ty, llvm_i32_ty],
					 [IntrNoMem]>;
class AIE2bf16AddMACConf
    : Intrinsic<[llvm_v8i64_ty], [llvm_v32bf16_ty, llvm_v32bf16_ty, llvm_v8i64_ty, llvm_v8i64_ty, llvm_i32_ty],
					 [IntrNoMem]>;

class AIEV2AddSubAcc : DefaultAttrsIntrinsic<[llvm_v16i64_ty], [llvm_v16i64_ty, llvm_v16i64_ty, llvm_i32_ty],
					 [IntrNoMem]>;

class AIEV2AddSubAccFloat : DefaultAttrsIntrinsic<[llvm_v8i64_ty], [llvm_v8i64_ty, llvm_v8i64_ty, llvm_i32_ty],
                              [IntrNoMem]>;

class AIEV2NEGCONF : DefaultAttrsIntrinsic<[llvm_v16i64_ty], [llvm_v16i64_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2NEGFPCONF : DefaultAttrsIntrinsic<[llvm_v8i64_ty], [llvm_v8i64_ty, llvm_i32_ty], [IntrNoMem]>;

class AIEV2V16AccFloatToV16Bf : Intrinsic<[llvm_v16bf16_ty], [llvm_v8i64_ty], [IntrNoMem]>;
class AIEV2V16BfToV16AccFloat : Intrinsic<[llvm_v8i64_ty], [llvm_v16bf16_ty], [IntrNoMem]>;

class AIEV2UNDV4Int32
     : Intrinsic<[llvm_v4i32_ty], [], [IntrNoMem]>;
class AIEV2UNDV16Int8
     : Intrinsic<[llvm_v16i8_ty], [], [IntrNoMem]>;
class AIEV2UNDV8Int16
     : Intrinsic<[llvm_v8i16_ty], [], [IntrNoMem]>;
class AIEV2UNDV8Int32
     : Intrinsic<[llvm_v8i32_ty], [], [IntrNoMem]>;
class AIEV2UNDV32Int8
     : Intrinsic<[llvm_v32i8_ty], [], [IntrNoMem]>;
class AIEV2UNDV16Int16
     : Intrinsic<[llvm_v16i16_ty], [], [IntrNoMem]>;
class AIEV2UNDV16Int32
     : Intrinsic<[llvm_v16i32_ty], [], [IntrNoMem]>;
class AIEV2UNDV32Int32
     : Intrinsic<[llvm_v32i32_ty], [], [IntrNoMem]>;
class AIEV2UNDV64Int8
     : Intrinsic<[llvm_v64i8_ty], [], [IntrNoMem]>;
class AIEV2UNDV32Int16
     : Intrinsic<[llvm_v32i16_ty], [], [IntrNoMem]>;
class AIEV2UNDV128Int8
     : Intrinsic<[llvm_v128i8_ty], [], [IntrNoMem]>;
class AIEV2UNDV64Int16
     : Intrinsic<[llvm_v64i16_ty], [], [IntrNoMem]>;
class AIEV2UNDV4ACC64
     : Intrinsic<[llvm_v4i64_ty], [], [IntrNoMem]>;
class AIEV2UNDV8ACC64
     : Intrinsic<[llvm_v8i64_ty], [], [IntrNoMem]>;
class AIEV2UNDV16ACC64
     : Intrinsic<[llvm_v16i64_ty], [], [IntrNoMem]>;
class AIEV2UND8Bf16
     : Intrinsic<[llvm_v8bf16_ty], [], [IntrNoMem]>;
class AIEV2UND16Bf16
     : Intrinsic<[llvm_v16bf16_ty], [], [IntrNoMem]>;
class AIEV2UND32Bf16
     : Intrinsic<[llvm_v32bf16_ty], [], [IntrNoMem]>;
class AIEV2UND64Bf16
     : Intrinsic<[llvm_v64bf16_ty], [], [IntrNoMem]>;
class AIEV2UNDV8Float
     : Intrinsic<[llvm_v8f32_ty], [], [IntrNoMem]>;
class AIEV2UNDV16Float
     : Intrinsic<[llvm_v16f32_ty], [], [IntrNoMem]>;
class AIEV2UNDV32Float
     : Intrinsic<[llvm_v32f32_ty], [], [IntrNoMem]>;

class AIEV2GET_I256_I128
     : Intrinsic<[llvm_v8i32_ty], [llvm_v4i32_ty], [IntrNoMem]>;

// VSHIFT
class AIEV2VShift_I512_I512
     : Intrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty,llvm_v16i32_ty,llvm_i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2VShift_bf128_bf512
     : Intrinsic<[llvm_v8bf16_ty], [llvm_v32bf16_ty,llvm_v32bf16_ty,llvm_i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2VShift_bf512_bf512
     : Intrinsic<[llvm_v32bf16_ty], [llvm_v32bf16_ty,llvm_v32bf16_ty,llvm_i32_ty,llvm_i32_ty], [IntrNoMem]>;
// VINSERT.X
class AIEV2VInsert8_I512
     : Intrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty,llvm_i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2VInsert16_I512
     : Intrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty,llvm_i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2VInsert32_I512
     : Intrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty,llvm_i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2VInsert64_I512
     : Intrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty,llvm_i32_ty,llvm_v2i32_ty], [IntrNoMem]>;
class AIEV2VInsert16_bf512
     : Intrinsic<[llvm_v32bf16_ty], [llvm_v32bf16_ty,llvm_i32_ty,llvm_bfloat_ty], [IntrNoMem]>;
class AIEV2VInsert32_bf512
     : Intrinsic<[llvm_v32bf16_ty], [llvm_v32bf16_ty,llvm_i32_ty,llvm_v2bf16_ty], [IntrNoMem]>;
class AIEV2VInsert64_bf512
     : Intrinsic<[llvm_v32bf16_ty], [llvm_v32bf16_ty,llvm_i32_ty,llvm_v4bf16_ty], [IntrNoMem]>;
class AIEV2VInsert32_accfloat
     : Intrinsic<[llvm_v8i64_ty], [llvm_v8i64_ty,llvm_i32_ty,llvm_float_ty], [IntrNoMem]>;

// VBCST.X
class AIEV2VBCST8I512
     : Intrinsic<[llvm_v64i8_ty], [llvm_i32_ty], [IntrNoMem]>;
class AIEV2VBCST16I512
     : Intrinsic<[llvm_v32i16_ty], [llvm_i32_ty], [IntrNoMem]>;
class AIEV2VBCST32I512
     : Intrinsic<[llvm_v16i32_ty], [llvm_i32_ty], [IntrNoMem]>;
class AIEV2VBCST64I512
     : Intrinsic<[llvm_v16i32_ty], [llvm_v2i32_ty], [IntrNoMem]>;
class AIEV2VBCST16bf512
     : Intrinsic<[llvm_v32bf16_ty], [llvm_bfloat_ty], [IntrNoMem]>;
class AIEV2VBCST32bf512
     : Intrinsic<[llvm_v32bf16_ty], [llvm_v2bf16_ty], [IntrNoMem]>;
class AIEV2VBCST64bf512
     : Intrinsic<[llvm_v32bf16_ty], [llvm_v4bf16_ty], [IntrNoMem]>;
class AIEV2VBCSTF32I512
     : Intrinsic<[llvm_v16f32_ty], [llvm_float_ty], [IntrNoMem]>;
// VEXTBCST.X
class AIEV2VEXTBCST8I512
     : Intrinsic<[llvm_v64i8_ty], [llvm_v64i8_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2VEXTBCST16I512
     : Intrinsic<[llvm_v32i16_ty], [llvm_v32i16_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2VEXTBCST32I512
     : Intrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2VEXTBCST32Bf512
     : Intrinsic<[llvm_v32bf16_ty], [llvm_v32bf16_ty, llvm_i32_ty], [IntrNoMem]>;
// VEXTRACT
class AIEV2VExtElem8I512
     : Intrinsic<[llvm_i32_ty], [llvm_v64i8_ty, llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2VExtElem16I512
     : Intrinsic<[llvm_i32_ty], [llvm_v32i16_ty, llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2VExtElem32I512
     : Intrinsic<[llvm_i32_ty], [llvm_v16i32_ty, llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2VExtElem64I512
     : Intrinsic<[llvm_v2i32_ty], [llvm_v16i32_ty, llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
// VSEL.X
class AIEV2Sel8_I512
     : Intrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty,llvm_v16i32_ty,llvm_v2i32_ty], [IntrNoMem]>;
class AIEV2Sel16_I512
     : Intrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty,llvm_v16i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2Sel32_I512
     : Intrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty,llvm_v16i32_ty,llvm_i32_ty], [IntrNoMem]>;
// UPD
class AIEV2UPD_I64_I32
     : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty,llvm_i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2UPD_I512_I256
     : Intrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty,llvm_v8i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2UPD_I1024_I512
     : Intrinsic<[llvm_v32i32_ty], [llvm_v32i32_ty,llvm_v16i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2UPD_I1024_I256
     : Intrinsic<[llvm_v32i32_ty], [llvm_v32i32_ty,llvm_v8i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2UPD_bf512_bf256
     : Intrinsic<[llvm_v32bf16_ty], [llvm_v32bf16_ty,llvm_v16bf16_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2UPD_bf1024_bf512
     : Intrinsic<[llvm_v64bf16_ty], [llvm_v64bf16_ty,llvm_v32bf16_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2UPD_bf1024_bf256
     : Intrinsic<[llvm_v64bf16_ty], [llvm_v64bf16_ty,llvm_v16bf16_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2UPD_512_256_acc
     : Intrinsic<[llvm_v8i64_ty], [llvm_v8i64_ty,llvm_v4i64_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2UPD_1024_512_acc
     : Intrinsic<[llvm_v16i64_ty], [llvm_v16i64_ty,llvm_v8i64_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2UPD_1024_256_acc
     : Intrinsic<[llvm_v16i64_ty], [llvm_v16i64_ty,llvm_v4i64_ty,llvm_i32_ty], [IntrNoMem]>;
// SET
class AIEV2SET_I64_I32
     : Intrinsic<[llvm_v2i32_ty], [llvm_i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2SET_I512_I256
     : Intrinsic<[llvm_v16i32_ty], [llvm_v8i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2SET_I1024_I512
     : Intrinsic<[llvm_v32i32_ty], [llvm_v16i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2SET_I1024_I256
     : Intrinsic<[llvm_v32i32_ty], [llvm_v8i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2SET_bf512_bf256
     : Intrinsic<[llvm_v32bf16_ty], [llvm_v16bf16_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2SET_bf1024_bf512
     : Intrinsic<[llvm_v64bf16_ty], [llvm_v32bf16_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2SET_bf1024_bf256
     : Intrinsic<[llvm_v64bf16_ty], [llvm_v16bf16_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2SET_512_256_acc
     : Intrinsic<[llvm_v8i64_ty], [llvm_v4i64_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2SET_1024_512_acc
     : Intrinsic<[llvm_v16i64_ty], [llvm_v8i64_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2SET_1024_256_acc
     : Intrinsic<[llvm_v16i64_ty], [llvm_v4i64_ty,llvm_i32_ty], [IntrNoMem]>;
// EXT
class AIEV2EXT_I32_I64
     : Intrinsic<[llvm_i32_ty], [llvm_v2i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2EXT_I256_I512
     : Intrinsic<[llvm_v8i32_ty] , [llvm_v16i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2EXT_I512_I1024
     : Intrinsic<[llvm_v16i32_ty], [llvm_v32i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2EXT_I256_I1024
     : Intrinsic<[llvm_v8i32_ty] , [llvm_v32i32_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2EXT_bf256_bf512
     : Intrinsic<[llvm_v16bf16_ty] , [llvm_v32bf16_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2EXT_bf512_bf1024
     : Intrinsic<[llvm_v32bf16_ty], [llvm_v64bf16_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2EXT_bf256_bf1024
     : Intrinsic<[llvm_v16bf16_ty] , [llvm_v64bf16_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2EXT_256_512_acc
     : Intrinsic<[llvm_v4i64_ty] , [llvm_v8i64_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2EXT_512_1024_acc
     : Intrinsic<[llvm_v8i64_ty], [llvm_v16i64_ty,llvm_i32_ty], [IntrNoMem]>;
class AIEV2EXT_256_1024_acc
     : Intrinsic<[llvm_v4i64_ty] , [llvm_v16i64_ty,llvm_i32_ty], [IntrNoMem]>;
// Concat
class AIEV2Concat_I512_I256
     : Intrinsic<[llvm_v16i32_ty], [llvm_v8i32_ty,llvm_v8i32_ty], [IntrNoMem]>;
class AIEV2Concat_I1024_I256
     : Intrinsic<[llvm_v32i32_ty], [llvm_v8i32_ty,llvm_v8i32_ty,llvm_v8i32_ty,llvm_v8i32_ty], [IntrNoMem]>;
class AIEV2Concat_I1024_I512
     : Intrinsic<[llvm_v32i32_ty], [llvm_v16i32_ty,llvm_v16i32_ty], [IntrNoMem]>;
class AIEV2Concat_bf512_bf256
     : Intrinsic<[llvm_v32bf16_ty], [llvm_v16bf16_ty,llvm_v16bf16_ty], [IntrNoMem]>;
class AIEV2Concat_bf1024_bf256
     : Intrinsic<[llvm_v64bf16_ty], [llvm_v16bf16_ty,llvm_v16bf16_ty,llvm_v16bf16_ty,llvm_v16bf16_ty], [IntrNoMem]>;
class AIEV2Concat_bf1024_bf512
     : Intrinsic<[llvm_v64bf16_ty], [llvm_v32bf16_ty,llvm_v32bf16_ty], [IntrNoMem]>;
class AIEV2Concat_512_256_acc
     : Intrinsic<[llvm_v8i64_ty], [llvm_v4i64_ty,llvm_v4i64_ty], [IntrNoMem]>;
class AIEV2Concat_1024_256_acc
     : Intrinsic<[llvm_v16i64_ty], [llvm_v4i64_ty,llvm_v4i64_ty,llvm_v4i64_ty,llvm_v4i64_ty], [IntrNoMem]>;
class AIEV2Concat_1024_512_acc
     : Intrinsic<[llvm_v16i64_ty], [llvm_v8i64_ty,llvm_v8i64_ty], [IntrNoMem]>;

//srs
class AIEV2I256V16Acc32I
	 : Intrinsic<[llvm_v16i16_ty], [llvm_v8i64_ty, llvm_i32_ty, llvm_i32_ty],
					 [IntrNoMem]>;
class AIEV2I256V16Acc64I
	 : Intrinsic<[llvm_v16i16_ty], [llvm_v16i64_ty, llvm_i32_ty, llvm_i32_ty],
					 [IntrNoMem]>;
class AIEV2I256V32Acc32I
	 : Intrinsic<[llvm_v32i8_ty], [llvm_v16i64_ty, llvm_i32_ty, llvm_i32_ty],
					 [IntrNoMem]>;
class AIEV2I256V8Acc64I
	 : Intrinsic<[llvm_v8i32_ty], [llvm_v8i64_ty, llvm_i32_ty, llvm_i32_ty],
					 [IntrNoMem]>;
class AIEV2I512V16Acc64I
	 : Intrinsic<[llvm_v16i32_ty], [llvm_v16i64_ty, llvm_i32_ty, llvm_i32_ty],
					 [IntrNoMem]>;
class AIEV2I512V32Acc32I
	 : Intrinsic<[llvm_v32i16_ty], [llvm_v16i64_ty, llvm_i32_ty, llvm_i32_ty],
					 [IntrNoMem]>;

// VUPS
class AIEV2Acc32V16I256I : Intrinsic<[llvm_v8i64_ty],  [llvm_v16i16_ty, llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2Acc32V32I256I : Intrinsic<[llvm_v16i64_ty], [llvm_v32i8_ty, llvm_i32_ty, llvm_i32_ty],  [IntrNoMem]>;
class AIEV2Acc32V32I512I : Intrinsic<[llvm_v16i64_ty], [llvm_v32i16_ty, llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2Acc64V16I256I : Intrinsic<[llvm_v16i64_ty], [llvm_v16i16_ty, llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2Acc64V16I512I : Intrinsic<[llvm_v16i64_ty], [llvm_v16i32_ty, llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2Acc64V8I256I  : Intrinsic<[llvm_v8i64_ty],  [llvm_v8i32_ty, llvm_i32_ty, llvm_i32_ty],  [IntrNoMem]>;

class AIEV2V64I8V2I32V64I8I32 : DefaultAttrsIntrinsic<[llvm_v64i8_ty, llvm_v2i32_ty], [llvm_v64i8_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2V32I16I32V32I16I32 : DefaultAttrsIntrinsic<[llvm_v32i16_ty, llvm_i32_ty], [llvm_v32i16_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2V16I32I32V16I32I32 : DefaultAttrsIntrinsic<[llvm_v16i32_ty, llvm_i32_ty], [llvm_v16i32_ty, llvm_i32_ty], [IntrNoMem]>;

class AIEV2V64I8V64I8V64I8I64 : DefaultAttrsIntrinsic<[llvm_v64i8_ty], [llvm_v64i8_ty, llvm_v64i8_ty, llvm_v2i32_ty], [IntrNoMem]>;
class AIEV2V32I16V32I16V32I16I32 : DefaultAttrsIntrinsic<[llvm_v32i16_ty], [llvm_v32i16_ty, llvm_v32i16_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2V16I32V16I32V16I32I32 : DefaultAttrsIntrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty, llvm_i32_ty], [IntrNoMem]>;

class AIEV2V64I8V2I32V64I8 : DefaultAttrsIntrinsic<[llvm_v64i8_ty, llvm_v2i32_ty], [llvm_v64i8_ty], [IntrNoMem]>;
class AIEV2V32I16I32V32I16 : DefaultAttrsIntrinsic<[llvm_v32i16_ty, llvm_i32_ty], [llvm_v32i16_ty], [IntrNoMem]>;
class AIEV2V16I32I32V16I32 : DefaultAttrsIntrinsic<[llvm_v16i32_ty, llvm_i32_ty], [llvm_v16i32_ty], [IntrNoMem]>;

class AIEV2V2I32V64I8 : DefaultAttrsIntrinsic<[llvm_v2i32_ty], [llvm_v64i8_ty], [IntrNoMem]>;
class AIEV2I32V32I16 : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_v32i16_ty], [IntrNoMem]>;
class AIEV2I32V16I32 : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_v16i32_ty], [IntrNoMem]>;

class AIEV2V2I32V64I8V64I8I32 : DefaultAttrsIntrinsic<[llvm_v2i32_ty], [llvm_v64i8_ty, llvm_v64i8_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2I32V32I16V32I16I32 : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_v32i16_ty, llvm_v32i16_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2I32V16I32V16I32I32 : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2I32V32BF16V32BF16 : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_v32bf16_ty, llvm_v32bf16_ty], [IntrNoMem]>;

class AIEV2V64I8I64V64I8V64I8I32 : DefaultAttrsIntrinsic<[llvm_v64i8_ty, llvm_v2i32_ty], [llvm_v64i8_ty, llvm_v64i8_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2V32I16I32V32I16V32I16I32 : DefaultAttrsIntrinsic<[llvm_v32i16_ty, llvm_i32_ty], [llvm_v32i16_ty, llvm_v32i16_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2V16I32I32V16I32V16I32I32 : DefaultAttrsIntrinsic<[llvm_v16i32_ty, llvm_i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2V32BF16I32V32BF16V32BF16I32 : DefaultAttrsIntrinsic<[llvm_v32bf16_ty, llvm_i32_ty], [llvm_v32bf16_ty, llvm_v32bf16_ty], [IntrNoMem]>;

// VBCSTSHFL
class AIEV2VBcstShfl_bf16 : Intrinsic<[llvm_v32bf16_ty], [llvm_bfloat_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2VBcstShfl_I32 : Intrinsic<[llvm_v16i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2VBcstShfl_I64 : Intrinsic<[llvm_v16i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;

// VSHUFFLE
class AIEV2VShuffle : Intrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2VShuffleBf16 : Intrinsic<[llvm_v32bf16_ty], [llvm_v32bf16_ty, llvm_v32bf16_ty, llvm_i32_ty], [IntrNoMem]>;


// PADDX.XX
class AIEV2Add2D : Intrinsic<[llvm_ptr_ty, llvm_i20_ty],
      [llvm_ptr_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty],
      [IntrNoMem]>;
class AIEV2Add3D : Intrinsic<[llvm_ptr_ty, llvm_i20_ty, llvm_i20_ty],
      [llvm_ptr_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty, llvm_i20_ty],
      [IntrNoMem]>;

// LOAD_4X
class AIEV2LOAD4X : DefaultAttrsIntrinsic<[llvm_v8i32_ty], [llvm_v8i32_ty],
      [IntrReadMem]>;


// bfloat16 conversions
class AIEV2FloatToBF16 : Intrinsic<[llvm_bfloat_ty],
      [llvm_float_ty],
      [IntrNoMem]>;
class AIEV2BF16ToInt : Intrinsic<[llvm_i32_ty],
      [llvm_bfloat_ty],
      [IntrNoMem]>;
class AIEV2V16BF16ToV16I : Intrinsic<[llvm_v16i32_ty],
      [llvm_v16bf16_ty, llvm_i32_ty],
      [IntrNoMem]>;

// Streams
class AIEV2V16I32I32I32 : DefaultAttrsIntrinsic<[llvm_v16i32_ty], [llvm_i32_ty], [IntrInaccessibleMemOnly]>;
class AIEV2V16Acc32I32I32 : DefaultAttrsIntrinsic<[llvm_v8i64_ty], [llvm_i32_ty], [IntrInaccessibleMemOnly]>;
class AIEV2V32Acc32I32 : DefaultAttrsIntrinsic<[llvm_v16i64_ty], [llvm_i32_ty], [IntrInaccessibleMemOnly]>;
class AIEV2vV16I32I32 : DefaultAttrsIntrinsic<[], [llvm_v16i32_ty, llvm_i32_ty], [IntrInaccessibleMemOnly]>;
class AIEV2vV16Acc32I32 : DefaultAttrsIntrinsic<[], [llvm_v8i64_ty, llvm_i32_ty], [IntrInaccessibleMemOnly]>;
class AIEV2I32I32 : DefaultAttrsIntrinsic<[llvm_i32_ty, llvm_i32_ty], [], [IntrInaccessibleMemOnly]>;
class AIEV2vI32I32 : DefaultAttrsIntrinsic<[], [llvm_i32_ty, llvm_i32_ty], [IntrInaccessibleMemOnly]>;
class AIEV2I32I32I32 : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrInaccessibleMemOnly]>;
class AIEV2vI32I32I32 : DefaultAttrsIntrinsic<[], [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], [IntrInaccessibleMemOnly]>;
class AIEV2I32I32I32I32 : DefaultAttrsIntrinsic<[llvm_i32_ty],
                                                [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty],
                                                [IntrInaccessibleMemOnly]>;
class AIEV2vI32I32I32I32I32 : DefaultAttrsIntrinsic<[],
                                                    [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty],
                                                    [IntrInaccessibleMemOnly]>;
class AIEV2I32I32I32I32I32I32 : DefaultAttrsIntrinsic<[llvm_i32_ty],
                                                       [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty, llvm_i32_ty],
                                                       [IntrInaccessibleMemOnly]>;

class AIEV2ReadTM : Intrinsic<[llvm_i32_ty], [llvm_ptr_ty], [IntrReadMem]>;
class AIEV2WriteTM : Intrinsic<[], [llvm_i32_ty, llvm_ptr_ty], [IntrWriteMem]>;

// Mode Settings
// Set Control Reg
class AIEV2SetCtrlReg : Intrinsic<[], [llvm_i32_ty, llvm_i32_ty], [IntrWriteMem, IntrInaccessibleMemOnly]>;
// Get Control Reg
class AIEV2GetCtrlReg : Intrinsic<[llvm_i32_ty], [llvm_i32_ty], [IntrReadMem, IntrInaccessibleMemOnly]>;


// Get CoreID
class AIEV2GetCoreID : Intrinsic<[llvm_i32_ty], [], [IntrNoMem]>;

// CLB
class AIEV2CLB : Intrinsic<[llvm_i32_ty], [llvm_i32_ty], [IntrNoMem]>;

// Pack-Unpack
class AIEV2PackI4I8 : Intrinsic<[llvm_v32i8_ty], [llvm_v32i16_ty, llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>; 
class AIEV2PackI8I16 : Intrinsic<[llvm_v32i8_ty], [llvm_v32i16_ty, llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;  
class AIEV2UnPackI8I4 : Intrinsic<[llvm_v32i16_ty], [llvm_v32i8_ty, llvm_i32_ty], [IntrNoMem]>;
class AIEV2UnPackI16I8 : Intrinsic<[llvm_v32i16_ty], [llvm_v32i8_ty, llvm_i32_ty], [IntrNoMem]>;

// Scheduling barrier
class AIEV2SchedBarrier : Intrinsic<[], [], [IntrNoMem, IntrHasSideEffects, IntrConvergent, IntrWillReturn]>;

// clear accumulator
class AIEV2CLR16F : Intrinsic<[llvm_v8i64_ty], [], [IntrNoMem]>;
class AIEV2VCLRACC1024 : Intrinsic<[llvm_v16i64_ty], [], [IntrNoMem]>;

// Extract Sub-Vector
class AIEV2Extract_I128_I512
     : Intrinsic<[llvm_v4i32_ty], [llvm_v16i32_ty], [IntrNoMem]>;

// Set Sub-Vector
class AIEV2Set_I512_I128
     : Intrinsic<[llvm_v16i32_ty], [llvm_v4i32_ty], [IntrNoMem]>;

def int_aie2_event : ClangBuiltin<"__builtin_aiev2_event">, AIE2EventIntrinsic;
//Locks
def int_aie2_acquire :
     ClangBuiltin<"__builtin_aiev2_acquire">, AIE2LockIntr;
def int_aie2_acquire_cond :
     ClangBuiltin<"__builtin_aiev2_acquire_cond">, AIE2LockCondIntr;
def int_aie2_release :
     ClangBuiltin<"__builtin_aiev2_release">, AIE2LockIntr;
def int_aie2_release_cond :
     ClangBuiltin<"__builtin_aiev2_release_cond">, AIE2LockCondIntr;
def int_aie2_done :
    ClangBuiltin<"__builtin_aiev2_done">, AIE2DoneIntr;

def int_aie2_I512_I512_acc64_mul_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_acc64_mul_conf">, AIEV2I512I512MULConf;
def int_aie2_I512_I512_acc64_negmul_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_acc64_negmul_conf">, AIEV2I512I512MULConf;
def int_aie2_I512_I512_ACC1024_acc64_mac_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_acc64_mac_conf">, AIEV2I512I512MACConf;
def int_aie2_I512_I512_ACC1024_acc64_negmac_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_acc64_negmac_conf">, AIEV2I512I512MACConf;
def int_aie2_I512_I512_ACC1024_ACC1024_acc64_addmac_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_ACC1024_acc64_addmac_conf">, AIEV2I512I512AddMaconf;
def int_aie2_I512_I512_ACC1024_ACC1024_acc64_addmsc_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_ACC1024_acc64_addmsc_conf">, AIEV2I512I512AddMaconf;
def int_aie2_I512_I512_ACC1024_ACC1024_acc64_submac_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_ACC1024_acc64_submac_conf">, AIEV2I512I512AddMaconf;
def int_aie2_I512_I512_ACC1024_ACC1024_acc64_submsc_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_ACC1024_acc64_submsc_conf">, AIEV2I512I512AddMaconf;
def int_aie2_I512_I512_ACC1024_acc64_msc_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_acc64_msc_conf">, AIEV2I512I512MACConf;
def int_aie2_I512_I512_ACC1024_acc64_negmsc_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_acc64_negmsc_conf">, AIEV2I512I512MACConf;

def int_aie2_I512_I512_acc32_mul_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_acc32_mul_conf">, AIEV2I512I512MULConf;
def int_aie2_I512_I512_acc32_negmul_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_acc32_negmul_conf">, AIEV2I512I512MULConf;
def int_aie2_I512_I512_ACC1024_acc32_mac_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_acc32_mac_conf">, AIEV2I512I512MACConf;
def int_aie2_I512_I512_ACC1024_acc32_negmac_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_acc32_negmac_conf">, AIEV2I512I512MACConf;
def int_aie2_I512_I512_ACC1024_ACC1024_acc32_addmac_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_ACC1024_acc32_addmac_conf">, AIEV2I512I512AddMaconf;
def int_aie2_I512_I512_ACC1024_ACC1024_acc32_addmsc_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_ACC1024_acc32_addmsc_conf">, AIEV2I512I512AddMaconf;
def int_aie2_I512_I512_ACC1024_ACC1024_acc32_submac_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_ACC1024_acc32_submac_conf">, AIEV2I512I512AddMaconf;
def int_aie2_I512_I512_ACC1024_ACC1024_acc32_submsc_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_ACC1024_acc32_submsc_conf">, AIEV2I512I512AddMaconf;
def int_aie2_I512_I512_ACC1024_acc32_msc_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_acc32_msc_conf">, AIEV2I512I512MACConf;
def int_aie2_I512_I512_ACC1024_acc32_negmsc_conf : ClangBuiltin<"__builtin_aiev2_I512_I512_ACC1024_acc32_negmsc_conf">, AIEV2I512I512MACConf;

def int_aie2_bf_mul16_conf : ClangBuiltin<"__builtin_aiev2_bf_mul16_conf">, AIE2bf16MULConf;
def int_aie2_bf_negmul16_conf : ClangBuiltin<"__builtin_aiev2_bf_negmul16_conf">, AIE2bf16MULConf;
def int_aie2_bf_mac16_conf : ClangBuiltin<"__builtin_aiev2_bf_mac16_conf">, AIE2bf16MACConf;
def int_aie2_bf_msc16_conf : ClangBuiltin<"__builtin_aiev2_bf_msc16_conf">, AIE2bf16MACConf;
def int_aie2_bf_addmac16_conf : ClangBuiltin<"__builtin_aiev2_bf_addmac16_conf">, AIE2bf16AddMACConf;
def int_aie2_bf_addmsc16_conf : ClangBuiltin<"__builtin_aiev2_bf_addmsc16_conf">, AIE2bf16AddMACConf;

// Accumulator word negation
def int_aie2_ACC1024_acc32_neg_conf : ClangBuiltin<"__builtin_aiev2_ACC1024_acc32_neg_conf">, AIEV2NEGCONF;
def int_aie2_ACC1024_acc64_neg_conf : ClangBuiltin<"__builtin_aiev2_ACC1024_acc64_neg_conf">, AIEV2NEGCONF;

// Accumulator addition/subtraction
def int_aie2_add_acc : ClangBuiltin<"__builtin_aiev2_add_acc">, AIEV2AddSubAcc;
def int_aie2_sub_acc : ClangBuiltin<"__builtin_aiev2_sub_acc">, AIEV2AddSubAcc;

// Accumulator Float addition/substraction
def int_aie2_add_accfloat : ClangBuiltin<"__builtin_aiev2_add_accfloat">, AIEV2AddSubAccFloat;
def int_aie2_sub_accfloat : ClangBuiltin<"__builtin_aiev2_sub_accfloat">, AIEV2AddSubAccFloat;

// Accumulator negated Addition
def int_aie2_negadd_acc : ClangBuiltin<"__builtin_aiev2_negadd_acc">, AIEV2AddSubAcc;
def int_aie2_negsub_acc : ClangBuiltin<"__builtin_aiev2_negsub_acc">, AIEV2AddSubAcc;

// Accumulator Float negated Addition
def int_aie2_negadd_accfloat : ClangBuiltin<"__builtin_aiev2_negadd_accfloat">, AIEV2AddSubAccFloat;
def int_aie2_negsub_accfloat : ClangBuiltin<"__builtin_aiev2_negsub_accfloat">, AIEV2AddSubAccFloat;

// Accumulator Float word negation
def int_aie2_ACC512_accfloat_neg_conf : ClangBuiltin<"__builtin_aiev2_ACC512_accfloat_neg_conf">, AIEV2NEGFPCONF;

// v16accfloat to v16bfloat16
def int_aie2_v16accfloat_to_v16bf16 : ClangBuiltin<"__builtin_aiev2_v16accfloat_to_v16bf16">, AIEV2V16AccFloatToV16Bf;
def int_aie2_v16bf16_to_v16accfloat : ClangBuiltin<"__builtin_aiev2_v16bf16_to_v16accfloat">, AIEV2V16BfToV16AccFloat;

//undef
def int_aie2_v4int32 : ClangBuiltin<"__builtin_aiev2_v4int32">, AIEV2UNDV4Int32;
def int_aie2_v16int8 : ClangBuiltin<"__builtin_aiev2_v16int8">, AIEV2UNDV16Int8;
def int_aie2_v8int16 : ClangBuiltin<"__builtin_aiev2_v8int16">, AIEV2UNDV8Int16;
def int_aie2_v8int32 : ClangBuiltin<"__builtin_aiev2_v8int32">, AIEV2UNDV8Int32;
def int_aie2_v32int8 : ClangBuiltin<"__builtin_aiev2_v32int8">, AIEV2UNDV32Int8;
def int_aie2_v16int16 : ClangBuiltin<"__builtin_aiev2_v16int16">, AIEV2UNDV16Int16;
def int_aie2_v16int32 : ClangBuiltin<"__builtin_aiev2_v16int32">, AIEV2UNDV16Int32;
def int_aie2_v32int32 : ClangBuiltin<"__builtin_aiev2_v32int32">, AIEV2UNDV32Int32;
def int_aie2_v64int8 : ClangBuiltin<"__builtin_aiev2_v64int8">, AIEV2UNDV64Int8;
def int_aie2_v32int16 : ClangBuiltin<"__builtin_aiev2_v32int16">, AIEV2UNDV32Int16;
def int_aie2_v128int8  : ClangBuiltin<"__builtin_aiev2_v128int8">, AIEV2UNDV128Int8;
def int_aie2_v64int16 : ClangBuiltin<"__builtin_aiev2_v64int16">, AIEV2UNDV64Int16;
def int_aie2_v8acc32 : ClangBuiltin<"__builtin_aiev2_v8acc32">, AIEV2UNDV4ACC64;
def int_aie2_v16acc32 : ClangBuiltin<"__builtin_aiev2_v16acc32">, AIEV2UNDV8ACC64;
def int_aie2_v32acc32 : ClangBuiltin<"__builtin_aiev2_v32acc32">, AIEV2UNDV16ACC64;
def int_aie2_v4acc64 : ClangBuiltin<"__builtin_aiev2_v4acc64">, AIEV2UNDV4ACC64;
def int_aie2_v8acc64 : ClangBuiltin<"__builtin_aiev2_v8acc64">, AIEV2UNDV8ACC64;
def int_aie2_v16acc64 : ClangBuiltin<"__builtin_aiev2_v16acc64">, AIEV2UNDV16ACC64;
def int_aie2_v8accfloat : ClangBuiltin<"__builtin_aiev2_v8accfloat">, AIEV2UNDV4ACC64;
def int_aie2_v16accfloat : ClangBuiltin<"__builtin_aiev2_v16accfloat">, AIEV2UNDV8ACC64;
def int_aie2_v32accfloat : ClangBuiltin<"__builtin_aiev2_v32accfloat">, AIEV2UNDV16ACC64;
def int_aie2_v8bfloat16 : ClangBuiltin<"__builtin_aiev2_v8bfloat16">, AIEV2UND8Bf16;
def int_aie2_v16bfloat16 : ClangBuiltin<"__builtin_aiev2_v16bfloat16">, AIEV2UND16Bf16;
def int_aie2_v32bfloat16 : ClangBuiltin<"__builtin_aiev2_v32bfloat16">, AIEV2UND32Bf16;
def int_aie2_v64bfloat16 : ClangBuiltin<"__builtin_aiev2_v64bfloat16">, AIEV2UND64Bf16;
def int_aie2_v8float : ClangBuiltin<"__builtin_aiev2_v8float">, AIEV2UNDV8Float;
def int_aie2_v16float : ClangBuiltin<"__builtin_aiev2_v16float">, AIEV2UNDV16Float;
def int_aie2_v32float : ClangBuiltin<"__builtin_aiev2_v32float">, AIEV2UNDV32Float;


//return 256-reg from 128-reg
def int_aie2_get_I256_I128 : ClangBuiltin<"__builtin_aiev2_get_I256_I128">, AIEV2GET_I256_I128;

//scl2vec::shift-shiftx-shift_bytes
def int_aie2_vshift_I512_I512 : ClangBuiltin<"__builtin_aiev2_vshift_I512_I512">, AIEV2VShift_I512_I512;
def int_aie2_vshift_bf512_bf512 : ClangBuiltin<"__builtin_aiev2_vshift_bf512_bf512">, AIEV2VShift_bf512_bf512;
//scl2vec::insert
def int_aie2_vinsert8_I512  : ClangBuiltin<"__builtin_aiev2_vinsert8_I512">,  AIEV2VInsert8_I512;
def int_aie2_vinsert16_I512 : ClangBuiltin<"__builtin_aiev2_vinsert16_I512">, AIEV2VInsert16_I512;
def int_aie2_vinsert32_I512 : ClangBuiltin<"__builtin_aiev2_vinsert32_I512">, AIEV2VInsert32_I512;
def int_aie2_vinsert64_I512 : ClangBuiltin<"__builtin_aiev2_vinsert64_I512">, AIEV2VInsert64_I512;
def int_aie2_vinsert16_bf512 : ClangBuiltin<"__builtin_aiev2_vinsert_bf16_bf512">, AIEV2VInsert16_bf512;
def int_aie2_vinsert32_bf512 : ClangBuiltin<"__builtin_aiev2_vinsert_bf32_bf512">, AIEV2VInsert32_bf512;
def int_aie2_vinsert64_bf512 : ClangBuiltin<"__builtin_aiev2_vinsert_bf64_bf512">, AIEV2VInsert64_bf512;
def int_aie2_vinsert32_accfloat : ClangBuiltin<"__builtin_aiev2_vinsert32_accfloat">, AIEV2VInsert32_accfloat;
//scl2vec::broadcast:broadcast_zero:broadcast_one
def int_aie2_vbroadcast8_I512  : ClangBuiltin<"__builtin_aiev2_vbroadcast8_I512">, AIEV2VBCST8I512;
def int_aie2_vbroadcast16_I512 : ClangBuiltin<"__builtin_aiev2_vbroadcast16_I512">, AIEV2VBCST16I512;
def int_aie2_vbroadcast32_I512 : ClangBuiltin<"__builtin_aiev2_vbroadcast32_I512">, AIEV2VBCST32I512;
def int_aie2_vbroadcast64_I512 : ClangBuiltin<"__builtin_aiev2_vbroadcast64_I512">, AIEV2VBCST64I512;
def int_aie2_vbroadcast16_bf512 : ClangBuiltin<"__builtin_aiev2_vbroadcast_bf16_bf512">, AIEV2VBCST16bf512;
def int_aie2_vbroadcast32_bf512 : ClangBuiltin<"__builtin_aiev2_vbroadcast_bf32_bf512">, AIEV2VBCST32bf512;
def int_aie2_vbroadcast64_bf512 : ClangBuiltin<"__builtin_aiev2_vbroadcast_bf64_bf512">, AIEV2VBCST64bf512;
def int_aie2_vbroadcastfloat_I512 : ClangBuiltin<"__builtin_aiev2_vbroadcastfloat_I512">, AIEV2VBCSTF32I512;
def int_aie2_vbroadcast_zero_acc1024 : ClangBuiltin<"__builtin_aiev2_vbroadcast_zero_acc1024">, AIEV2VCLRACC1024;
//scl2vec::broadcast_elem
def int_aie2_vextract_broadcast8_I512  : ClangBuiltin<"__builtin_aiev2_vextract_broadcast8_I512">, AIEV2VEXTBCST8I512;
def int_aie2_vextract_broadcast16_I512 : ClangBuiltin<"__builtin_aiev2_vextract_broadcast16_I512">, AIEV2VEXTBCST16I512;
def int_aie2_vextract_broadcast32_I512 : ClangBuiltin<"__builtin_aiev2_vextract_broadcast32_I512">, AIEV2VEXTBCST32I512;
def int_aie2_vextract_broadcast32_bf512 : ClangBuiltin<"__builtin_aiev2_vextract_broadcast_bf32_bf512">, AIEV2VEXTBCST32Bf512;
//scl2vec::vbcstshfl
def int_aie2_vbcst_shuffle8  : ClangBuiltin<"__builtin_aiev2_vbcst_shuffle8">, AIEV2VBcstShfl_I32;
def int_aie2_vbcst_shuffle16 : ClangBuiltin<"__builtin_aiev2_vbcst_shuffle16">, AIEV2VBcstShfl_I32;
def int_aie2_vbcst_shuffle_bf16 : ClangBuiltin<"__builtin_aiev2_vbcst_shuffle_bf16">, AIEV2VBcstShfl_bf16;
def int_aie2_vbcst_shuffle32 : ClangBuiltin<"__builtin_aiev2_vbcst_shuffle32">, AIEV2VBcstShfl_I32;
def int_aie2_vbcst_shuffle64 : ClangBuiltin<"__builtin_aiev2_vbcst_shuffle64">, AIEV2VBcstShfl_I64;
//scl2vec::extract_elem
def int_aie2_vextract_elem8_I512 : ClangBuiltin<"__builtin_aiev2_vextract_elem8_I512">, AIEV2VExtElem8I512;
def int_aie2_vextract_elem16_I512 : ClangBuiltin<"__builtin_aiev2_vextract_elem16_I512">, AIEV2VExtElem16I512;
def int_aie2_vextract_elem32_I512 : ClangBuiltin<"__builtin_aiev2_vextract_elem32_I512">, AIEV2VExtElem32I512;
def int_aie2_vextract_elem64_I512 : ClangBuiltin<"__builtin_aiev2_vextract_elem64_I512">, AIEV2VExtElem64I512;

//ext-upd-set-concat::upd_ext
//upd
def int_aie2_upd_I64_I32    : ClangBuiltin<"__builtin_aiev2_upd_I64_I32">,    AIEV2UPD_I64_I32;
def int_aie2_upd_I512_I256  : ClangBuiltin<"__builtin_aiev2_upd_I512_I256">,  AIEV2UPD_I512_I256;
def int_aie2_upd_I1024_I512 : ClangBuiltin<"__builtin_aiev2_upd_I1024_I512">, AIEV2UPD_I1024_I512;
def int_aie2_upd_I1024_I256 : ClangBuiltin<"__builtin_aiev2_upd_I1024_I256">, AIEV2UPD_I1024_I256;
def int_aie2_upd_bf512_bf256  : ClangBuiltin<"__builtin_aiev2_upd_bf512_bf256">,  AIEV2UPD_bf512_bf256;
def int_aie2_upd_bf1024_bf512 : ClangBuiltin<"__builtin_aiev2_upd_bf1024_bf512">, AIEV2UPD_bf1024_bf512;
def int_aie2_upd_bf1024_bf256 : ClangBuiltin<"__builtin_aiev2_upd_bf1024_bf256">, AIEV2UPD_bf1024_bf256;
def int_aie2_upd_512_256_acc  :
   ClangBuiltin<"__builtin_aiev2_upd_ACC512_ACC256">,  AIEV2UPD_512_256_acc;
def int_aie2_upd_1024_512_acc :
   ClangBuiltin<"__builtin_aiev2_upd_ACC1024_ACC512">, AIEV2UPD_1024_512_acc;
def int_aie2_upd_1024_256_acc :
   ClangBuiltin<"__builtin_aiev2_upd_ACC1024_ACC256">, AIEV2UPD_1024_256_acc;
//set
def int_aie2_set_I64_I32    : ClangBuiltin<"__builtin_aiev2_set_I64_I32">,    AIEV2SET_I64_I32;
def int_aie2_set_I512_I256  : ClangBuiltin<"__builtin_aiev2_set_I512_I256">,  AIEV2SET_I512_I256;
def int_aie2_set_I1024_I512 : ClangBuiltin<"__builtin_aiev2_set_I1024_I512">, AIEV2SET_I1024_I512;
def int_aie2_set_I1024_I256 : ClangBuiltin<"__builtin_aiev2_set_I1024_I256">, AIEV2SET_I1024_I256;
def int_aie2_set_bf512_bf256  : ClangBuiltin<"__builtin_aiev2_set_bf512_bf256">,  AIEV2SET_bf512_bf256;
def int_aie2_set_bf1024_bf512 : ClangBuiltin<"__builtin_aiev2_set_bf1024_bf512">, AIEV2SET_bf1024_bf512;
def int_aie2_set_bf1024_bf256 : ClangBuiltin<"__builtin_aiev2_set_bf1024_bf256">, AIEV2SET_bf1024_bf256;
def int_aie2_set_512_256_acc  :
   ClangBuiltin<"__builtin_aiev2_set_ACC512_ACC256">,  AIEV2SET_512_256_acc;
def int_aie2_set_1024_512_acc :
   ClangBuiltin<"__builtin_aiev2_set_ACC1024_ACC512">, AIEV2SET_1024_512_acc;
def int_aie2_set_1024_256_acc :
   ClangBuiltin<"__builtin_aiev2_set_ACC1024_ACC256">, AIEV2SET_1024_256_acc;
//ext
def int_aie2_ext_I32_I64    : ClangBuiltin<"__builtin_aiev2_ext_I32_I64">,    AIEV2EXT_I32_I64;
def int_aie2_ext_I256_I512  : ClangBuiltin<"__builtin_aiev2_ext_I256_I512">,  AIEV2EXT_I256_I512;
def int_aie2_ext_I512_I1024 : ClangBuiltin<"__builtin_aiev2_ext_I512_I1024">, AIEV2EXT_I512_I1024;
def int_aie2_ext_I256_I1024 : ClangBuiltin<"__builtin_aiev2_ext_I256_I1024">, AIEV2EXT_I256_I1024;
def int_aie2_ext_bf256_bf512  : ClangBuiltin<"__builtin_aiev2_ext_bf256_bf512">,  AIEV2EXT_bf256_bf512;
def int_aie2_ext_bf512_bf1024 : ClangBuiltin<"__builtin_aiev2_ext_bf512_bf1024">, AIEV2EXT_bf512_bf1024;
def int_aie2_ext_bf256_bf1024 : ClangBuiltin<"__builtin_aiev2_ext_bf256_bf1024">, AIEV2EXT_bf256_bf1024;

def int_aie2_ext_256_512_acc  :
   ClangBuiltin<"__builtin_aiev2_ext_ACC256_ACC512">,  AIEV2EXT_256_512_acc;
def int_aie2_ext_512_1024_acc :
   ClangBuiltin<"__builtin_aiev2_ext_ACC512_ACC1024">, AIEV2EXT_512_1024_acc;
def int_aie2_ext_256_1024_acc :
   ClangBuiltin<"__builtin_aiev2_ext_ACC256_ACC1024">, AIEV2EXT_256_1024_acc;
//concat
def int_aie2_concat_I512_I256  : ClangBuiltin<"__builtin_aiev2_concat_I512_I256">,  AIEV2Concat_I512_I256;
def int_aie2_concat_I1024_I256 : ClangBuiltin<"__builtin_aiev2_concat_I1024_I256">, AIEV2Concat_I1024_I256;
def int_aie2_concat_I1024_I512 : ClangBuiltin<"__builtin_aiev2_concat_I1024_I512">, AIEV2Concat_I1024_I512;
def int_aie2_concat_bf512_bf256  : ClangBuiltin<"__builtin_aiev2_concat_bf512_bf256">,  AIEV2Concat_bf512_bf256;
def int_aie2_concat_bf1024_bf256 : ClangBuiltin<"__builtin_aiev2_concat_bf1024_bf256">, AIEV2Concat_bf1024_bf256;
def int_aie2_concat_bf1024_bf512 : ClangBuiltin<"__builtin_aiev2_concat_bf1024_bf512">, AIEV2Concat_bf1024_bf512;
def int_aie2_concat_512_256_acc32  :
   ClangBuiltin<"__builtin_aiev2_concat_ACC512_ACC256_acc32">,  AIEV2Concat_512_256_acc;
def int_aie2_concat_1024_256_acc32 :
   ClangBuiltin<"__builtin_aiev2_concat_ACC1024_ACC256_acc32">, AIEV2Concat_1024_256_acc;
def int_aie2_concat_1024_512_acc32 :
   ClangBuiltin<"__builtin_aiev2_concat_ACC1024_ACC512_acc32">, AIEV2Concat_1024_512_acc;
def int_aie2_concat_512_256_acc64  :
   ClangBuiltin<"__builtin_aiev2_concat_ACC512_ACC256_acc64">,  AIEV2Concat_512_256_acc;
def int_aie2_concat_1024_256_acc64 :
   ClangBuiltin<"__builtin_aiev2_concat_ACC1024_ACC256_acc64">, AIEV2Concat_1024_256_acc;
def int_aie2_concat_1024_512_acc64 :
   ClangBuiltin<"__builtin_aiev2_concat_ACC1024_ACC512_acc64">, AIEV2Concat_1024_512_acc;

//srs
def int_aie2_I256_v16_acc32_srs : ClangBuiltin<"__builtin_aiev2_I256_v16_acc32_srs">, AIEV2I256V16Acc32I;
def int_aie2_I256_v16_acc64_srs : ClangBuiltin<"__builtin_aiev2_I256_v16_acc64_srs">, AIEV2I256V16Acc64I;
def int_aie2_I256_v32_acc32_srs : ClangBuiltin<"__builtin_aiev2_I256_v32_acc32_srs">, AIEV2I256V32Acc32I;
def int_aie2_I256_v8_acc64_srs  : ClangBuiltin<"__builtin_aiev2_I256_v8_acc64_srs">, AIEV2I256V8Acc64I;
def int_aie2_I512_v16_acc64_srs : ClangBuiltin<"__builtin_aiev2_I512_v16_acc64_srs">, AIEV2I512V16Acc64I;
def int_aie2_I512_v32_acc32_srs : ClangBuiltin<"__builtin_aiev2_I512_v32_acc32_srs">, AIEV2I512V32Acc32I;

//ups
def int_aie2_acc32_v16_I256_ups : ClangBuiltin<"__builtin_aiev2_acc32_v16_I256_ups">,  AIEV2Acc32V16I256I;
def int_aie2_acc32_v32_I256_ups : ClangBuiltin<"__builtin_aiev2_acc32_v32_I256_ups">,  AIEV2Acc32V32I256I;
def int_aie2_acc32_v32_I512_ups : ClangBuiltin<"__builtin_aiev2_acc32_v32_I512_ups">,  AIEV2Acc32V32I512I;
def int_aie2_acc64_v16_I256_ups : ClangBuiltin<"__builtin_aiev2_acc64_v16_I256_ups">,  AIEV2Acc64V16I256I;
def int_aie2_acc64_v16_I512_ups : ClangBuiltin<"__builtin_aiev2_acc64_v16_I512_ups">,  AIEV2Acc64V16I512I;
def int_aie2_acc64_v8_I256_ups  : ClangBuiltin<"__builtin_aiev2_acc64_v8_I256_ups">,   AIEV2Acc64V8I256I;

//shuffle
def int_aie2_vshuffle : ClangBuiltin<"__builtin_aiev2_vshuffle">, AIEV2VShuffle;
def int_aie2_vshuffle_bf16 : ClangBuiltin<"__builtin_aiev2_vshuffle_bf16">, AIEV2VShuffleBf16;

// vabs_gtz
def int_aie2_vabs_gtz8  : AIEV2V64I8V2I32V64I8I32;
def int_aie2_vabs_gtz16 : AIEV2V32I16I32V32I16I32;
def int_aie2_vabs_gtz32 : AIEV2V16I32I32V16I32I32;

// vaddsub
def int_aie2_vaddsub8  : ClangBuiltin<"__builtin_aiev2_vaddsub8">, AIEV2V64I8V64I8V64I8I64;
def int_aie2_vaddsub16 : ClangBuiltin<"__builtin_aiev2_vaddsub16">, AIEV2V32I16V32I16V32I16I32;
def int_aie2_vaddsub32 : ClangBuiltin<"__builtin_aiev2_vaddsub32">, AIEV2V16I32V16I32V16I32I32;

// vbneg_ltz
def int_aie2_vbneg_ltz8  : AIEV2V64I8V2I32V64I8;
def int_aie2_vbneg_ltz16 : AIEV2V32I16I32V32I16;
def int_aie2_vbneg_ltz32 : AIEV2V16I32I32V16I32;

// veqz
def int_aie2_veqz8  : ClangBuiltin<"__builtin_aiev2_veqz8">, AIEV2V2I32V64I8;
def int_aie2_veqz16 : ClangBuiltin<"__builtin_aiev2_veqz16">, AIEV2I32V32I16;
def int_aie2_veqz32 : ClangBuiltin<"__builtin_aiev2_veqz32">, AIEV2I32V16I32;

// vge
def int_aie2_vge8  : ClangBuiltin<"__builtin_aiev2_vge8">, AIEV2V2I32V64I8V64I8I32;
def int_aie2_vge16 : ClangBuiltin<"__builtin_aiev2_vge16">, AIEV2I32V32I16V32I16I32;
def int_aie2_vge32 : ClangBuiltin<"__builtin_aiev2_vge32">, AIEV2I32V16I32V16I32I32;
def int_aie2_vgebf16 : ClangBuiltin<"__builtin_aiev2_vgebf16">, AIEV2I32V32BF16V32BF16;

// vlt
def int_aie2_vlt8  : ClangBuiltin<"__builtin_aiev2_vlt8">, AIEV2V2I32V64I8V64I8I32;
def int_aie2_vlt16 : ClangBuiltin<"__builtin_aiev2_vlt16">, AIEV2I32V32I16V32I16I32;
def int_aie2_vlt32 : ClangBuiltin<"__builtin_aiev2_vlt32">, AIEV2I32V16I32V16I32I32;
def int_aie2_vltbf16 : ClangBuiltin<"__builtin_aiev2_vltbf16">, AIEV2I32V32BF16V32BF16;

// vmaxdiff_lt
def int_aie2_vmaxdiff_lt8  : AIEV2V64I8I64V64I8V64I8I32;
def int_aie2_vmaxdiff_lt16 : AIEV2V32I16I32V32I16V32I16I32;
def int_aie2_vmaxdiff_lt32 : AIEV2V16I32I32V16I32V16I32I32;

// vmax_lt
def int_aie2_vmax_lt8  : AIEV2V64I8I64V64I8V64I8I32;
def int_aie2_vmax_lt16 : AIEV2V32I16I32V32I16V32I16I32;
def int_aie2_vmax_lt32 : AIEV2V16I32I32V16I32V16I32I32;
def int_aie2_vmax_ltbf16  : AIEV2V32BF16I32V32BF16V32BF16I32;

// vmin_ge
def int_aie2_vmin_ge8  : AIEV2V64I8I64V64I8V64I8I32;
def int_aie2_vmin_ge16 : AIEV2V32I16I32V32I16V32I16I32;
def int_aie2_vmin_ge32 : AIEV2V16I32I32V16I32V16I32I32;
def int_aie2_vmin_gebf16 : AIEV2V32BF16I32V32BF16V32BF16I32;

// vneg_gtz
def int_aie2_vneg_gtz8  : AIEV2V64I8V2I32V64I8;
def int_aie2_vneg_gtz16 : AIEV2V32I16I32V32I16;
def int_aie2_vneg_gtz32 : AIEV2V16I32I32V16I32;

// vsel
def int_aie2_vsel8  : ClangBuiltin<"__builtin_aiev2_vsel8">, AIEV2V64I8V64I8V64I8I64;
def int_aie2_vsel16 : ClangBuiltin<"__builtin_aiev2_vsel16">, AIEV2V32I16V32I16V32I16I32;
def int_aie2_vsel32 : ClangBuiltin<"__builtin_aiev2_vsel32">, AIEV2V16I32V16I32V16I32I32;

// vsub_ge
def int_aie2_vsub_ge8  : AIEV2V64I8I64V64I8V64I8I32;
def int_aie2_vsub_ge16 : AIEV2V32I16I32V32I16V32I16I32;
def int_aie2_vsub_ge32 : AIEV2V16I32I32V16I32V16I32I32;

// vsub_lt
def int_aie2_vsub_lt8  : AIEV2V64I8I64V64I8V64I8I32;
def int_aie2_vsub_lt16 : AIEV2V32I16I32V32I16V32I16I32;
def int_aie2_vsub_lt32 : AIEV2V16I32I32V16I32V16I32I32;

// Addressing intrinsics
def int_aie2_add_2d : AIEV2Add2D;
def int_aie2_add_3d : AIEV2Add3D;

def int_aie2_load_4x16_lo : ClangBuiltin<"__builtin_aiev2_load_4x16_lo">, AIEV2LOAD4X;
def int_aie2_load_4x16_hi : ClangBuiltin<"__builtin_aiev2_load_4x16_hi">, AIEV2LOAD4X;
def int_aie2_load_4x32_lo : ClangBuiltin<"__builtin_aiev2_load_4x32_lo">, AIEV2LOAD4X;
def int_aie2_load_4x32_hi : ClangBuiltin<"__builtin_aiev2_load_4x32_hi">, AIEV2LOAD4X;
def int_aie2_load_4x64_lo : ClangBuiltin<"__builtin_aiev2_load_4x64_lo">, AIEV2LOAD4X;
def int_aie2_load_4x64_hi : ClangBuiltin<"__builtin_aiev2_load_4x64_hi">, AIEV2LOAD4X;

def int_aie2_float_to_bfloat16    : AIEV2FloatToBF16;
def int_aie2_bfloat16_to_int      : ClangBuiltin<"__builtin_bfloat16_to_int">, AIEV2BF16ToInt;
// vector bfloat16 to vector int
def int_aie2_v16bf16_to_v16i32      : ClangBuiltin<"__builtin_aiev2_v16bf16_to_v16i32">, AIEV2V16BF16ToV16I;

// Streams
// Cascade stream read
def int_aie2_scd_read_vec : ClangBuiltin<"__builtin_aiev2_scd_read_vec">, AIEV2V16I32I32I32;
def int_aie2_scd_read_acc32 : ClangBuiltin<"__builtin_aiev2_scd_read_acc32">, AIEV2V16Acc32I32I32;
def int_aie2_scd_expand_lo : ClangBuiltin<"__builtin_aiev2_scd_expand_lo">, AIEV2V32Acc32I32;
def int_aie2_scd_expand_hi : ClangBuiltin<"__builtin_aiev2_scd_expand_hi">, AIEV2V32Acc32I32;

// Cascade stream write
def int_aie2_mcd_write_vec : ClangBuiltin<"__builtin_aiev2_mcd_write_vec">, AIEV2vV16I32I32;
def int_aie2_mcd_write_acc32 : ClangBuiltin<"__builtin_aiev2_mcd_write_acc32">, AIEV2vV16Acc32I32;

// Scalar stream read
class AIEV2_get_ss : DefaultAttrsIntrinsic<[llvm_i32_ty, llvm_i32_ty], [], [IntrHasSideEffects, IntrNoMem]>;
def int_aie2_get_ss : AIEV2_get_ss;
def int_aie2_get_ss_nb : AIEV2_get_ss;

// Scalar stream write with tlast
class AIEV2_put_ms : DefaultAttrsIntrinsic<[], [llvm_i32_ty, llvm_i32_ty], [IntrHasSideEffects, IntrNoMem]>;
def int_aie2_put_ms : ClangBuiltin<"__builtin_aiev2_put_ms">, AIEV2_put_ms;
def int_aie2_put_ms_nb : DefaultAttrsIntrinsic<[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrHasSideEffects, IntrNoMem]>;

// Packet switched stream
// Packet header
def int_aie2_put_ms_packet_header : ClangBuiltin<"__builtin_aiev2_put_ms_packet_header">, AIEV2vI32I32I32;
def int_aie2_put_ms_nb_packet_header : AIEV2I32I32I32I32;

// Control packet header
def int_aie2_put_ms_ctrl_packet_header : ClangBuiltin<"__builtin_aiev2_put_ms_ctrl_packet_header">, AIEV2vI32I32I32I32I32;
def int_aie2_put_ms_nb_ctrl_packet_header : AIEV2I32I32I32I32I32I32;

// Read/Write for Tile Memory Map
def int_aie2_read_tm : ClangBuiltin<"__builtin_aiev2_read_tm">, AIEV2ReadTM;
def int_aie2_write_tm : ClangBuiltin<"__builtin_aiev2_write_tm">, AIEV2WriteTM;

// Mode Settings
// Set Control Registers
def int_aie2_set_ctrl_reg : ClangBuiltin<"__builtin_aiev2_set_ctrl_reg">, AIEV2SetCtrlReg;
// Get Control Registers
def int_aie2_get_ctrl_reg : ClangBuiltin<"__builtin_aiev2_get_ctrl_reg">, AIEV2GetCtrlReg;

// Get Core ID
def int_aie2_get_coreid : ClangBuiltin<"__builtin_aiev2_get_coreid">, AIEV2GetCoreID;

// CLB
def int_aie2_clb : ClangBuiltin<"__builtin_aiev2_clb">, AIEV2CLB;

// Pack-Unpack
def int_aie2_pack_I8_I16 : ClangBuiltin<"__builtin_aiev2_pack_I8_I16">, AIEV2PackI8I16;
def int_aie2_pack_I4_I8 : ClangBuiltin<"__builtin_aiev2_pack_I4_I8">, AIEV2PackI4I8;
def int_aie2_unpack_I16_I8 : ClangBuiltin<"__builtin_aiev2_unpack_I16_I8">, AIEV2UnPackI16I8;
def int_aie2_unpack_I8_I4 : ClangBuiltin<"__builtin_aiev2_unpack_I8_I4">, AIEV2UnPackI8I4;

// Scheduling barrier
def int_aie2_sched_barrier : ClangBuiltin<"__builtin_aiev2_sched_barrier">, AIEV2SchedBarrier;

// accumulator vector with all lanes zeroed
def int_aie2_clr16f_conf : ClangBuiltin<"__builtin_aiev2_clr16f_conf">, AIEV2CLR16F;

// Extract Sub-Vector
def int_aie2_extract_I128_I512 : ClangBuiltin<"__builtin_aiev2_extract_I128_I512">, AIEV2Extract_I128_I512;

// Set Sub-Vector
def int_aie2_set_I512_I128 : ClangBuiltin<"__builtin_aiev2_set_I512_I128">, AIEV2Set_I512_I128;

} // TargetPrefix = "aie2"
