#
# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
add_llvm_component_library(LLVMAIEInfo
  AIETargetInfo.cpp

  LINK_COMPONENTS
  Support
  MC

  ADD_TO_COMPONENT
  AIE
  )
