#
# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates

# This file sets up a CMakeCache for the Peano AIE toolchain build.

set(CMAKE_BUILD_TYPE Release CACHE STRING "")
set(LLVM_ENABLE_ASSERTIONS ON CACHE BOOL "")
set(LLVM_ENABLE_TERMINFO OFF CACHE BOOL "")
set(BUILD_SHARED_LIBS OFF BOOL)

set(LLVM_TARGETS_TO_BUILD
      X86
    CACHE STRING "")

set(LLVM_EXPERIMENTAL_TARGETS_TO_BUILD
      AIE
    CACHE STRING "")

# Enable the LLVM projects and runtimes.
set(LLVM_ENABLE_PROJECTS
      clang
      clang-tools-extra
      lld
    CACHE STRING "")

include(${CMAKE_CURRENT_LIST_DIR}/Peano-AIE-runtime-libraries.cmake)

set(LLVM_CCACHE_BUILD ON BOOL)
# Configure the tools and libraries to be installed
set(LLVM_INSTALL_TOOLCHAIN_ONLY ON CACHE BOOL "")
set(LLVM_TOOLCHAIN_TOOLS
  llc
  llvm-ar
  llvm-cxxfilt
  llvm-dwarfdump
  llvm-nm
  llvm-objdump
  llvm-readelf
  llvm-readobj
  llvm-size
  llvm-symbolizer
  opt
  CACHE STRING "")

set(LLVM_DISTRIBUTION_COMPONENTS
  clang
  lld
  clang-resource-headers
  builtins-aie2
  runtimes-aie2
  ${LLVM_TOOLCHAIN_TOOLS}
  CACHE STRING "")
