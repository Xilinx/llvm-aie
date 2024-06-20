//===- api.td ----------------------------------------------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
include "config/public_api.td"

include "spec/stdc.td"

def CTypeAPI : PublicAPI<"ctype.h"> {
}

def FEnvAPI : PublicAPI<"fenv.h"> {
  let Types = ["fenv_t", "fexcept_t"];
}

def IntTypesAPI : PublicAPI<"inttypes.h"> {
  let Types = ["imaxdiv_t"];
}

def StdlibAPI : PublicAPI<"stdlib.h"> {
  let Types = [
    "div_t",
    "ldiv_t",
    "lldiv_t",
    "size_t",
    "__bsearchcompare_t",
    "__qsortcompare_t",
    "__atexithandler_t",
  ];
}

def StringAPI : PublicAPI<"string.h"> {
  let Types = ["size_t"];
}

def FenvAPI: PublicAPI<"fenv.h"> {
  let Macros = [
  ];
  let Types = ["fenv_t", "fexcept_t"];
}

def MathAPI : PublicAPI<"math.h"> {
  let Macros = [
  ];
  let Types = ["double_t", "float_t"];
}
