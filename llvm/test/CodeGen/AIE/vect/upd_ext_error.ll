;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: not llc -mtriple=aie < %s 2>%t
; RUN: FileCheck --check-prefix=CHECK-ERROR %s <%t
; CHECK-ERROR: error: call to aie_static_assert() marked "dontcall-error": The 2nd argument of this operation must be an immediate operand within: { 0, 1 }
define noundef <8 x i32> @_Z8upd_testv_error() {
entry:
  tail call void @_Z17aie_static_assertv()
  %0 = tail call <8 x i32> @llvm.aie.upd.v.v8i32.hi(<8 x i32> undef, <4 x i32> undef)
  ret <8 x i32> %0
}

declare <8 x i32> @llvm.aie.upd.v.v8i32.hi(<8 x i32>, <4 x i32>)

declare dso_local void @_Z17aie_static_assertv() #0

attributes #0 = { "dontcall-error"="The 2nd argument of this operation must be an immediate operand within: { 0, 1 }" "frame-pointer"="none" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
