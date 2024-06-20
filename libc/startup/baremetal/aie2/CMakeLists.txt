#
# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
add_startup_object(
  crt0
  SRC
    crt0.S
  COMPILE_OPTIONS
    -mno-vitis-headers
)

add_startup_object(
  crt1
  SRC
    crt1.cc
)
