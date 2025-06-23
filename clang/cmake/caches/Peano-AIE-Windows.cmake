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
# Compile libc here, to ensure libc-hdrgen is available on the host
set(LLVM_ENABLE_PROJECTS
      clang
      clang-tools-extra
      lld
      libc
    CACHE STRING "")
# Also need to set these to ensure libc-hdrgen gets built
set(LLVM_LIBC_FULL_BUILD
    ON CACHE BOOL "")
set(LLVM_FORCE_BUILD_RUNTIME
    ON CACHE BOOL "")
set(LIBC_HDRGEN_ONLY
    ON CACHE BOOL "")

# Can't enable libc here: the cmake files will ignore it on Windows.
set(LLVM_ENABLE_RUNTIMES
      compiler-rt
#      libc
    CACHE STRING "")

set(LLVM_BUILTIN_TARGETS "aie-none-unknown-elf;aie2-none-unknown-elf" CACHE STRING "")
set(LLVM_RUNTIME_TARGETS "${LLVM_BUILTIN_TARGETS}" CACHE STRING "")

foreach(target ${LLVM_BUILTIN_TARGETS})
  # Set the per-target cmake options.
  set(BUILTINS_${target}_CMAKE_BUILD_TYPE Release CACHE STRING "")
  set(RUNTIMES_${target}_CMAKE_BUILD_TYPE Release CACHE STRING "")
#  set(BUILTINS_${target}_LLVM_USE_LINKER lld CACHE STRING "")
#  set(RUNTIMES_${target}_LLVM_USE_LINKER lld CACHE STRING "")
  set(BUILTINS_${target}_CMAKE_C_FLAGS "--target=${target}" CACHE STRING "")
  set(BUILTINS_${target}_CMAKE_CXX_FLAGS "--target=${target}" CACHE STRING "")
  set(BUILTINS_${target}_CMAKE_ASM_FLAGS "--target=${target}" CACHE STRING "")
endforeach()

set(LIBCXX_ENABLE_SHARED OFF CACHE BOOL "")
set(LIBCXX_ENABLE_THREADS OFF CACHE BOOL "")
set(LIBCXXABI_ENABLE_SHARED OFF CACHE BOOL "")
set(LIBCXXABI_ENABLE_THREADS OFF CACHE BOOL "")
set(LIBCXX_ENABLE_ABI_LINKER_SCRIPT OFF CACHE BOOL "")
set(LIBCXX_LINK_TESTS_WITH_SHARED_LIBCXX OFF CACHE BOOL "")
set(LIBCXXABI_LINK_TESTS_WITH_SHARED_LIBCXX OFF CACHE BOOL "")
set(LIBCXXABI_LINK_TESTS_WITH_SHARED_LIBCXXABI OFF CACHE BOOL "")
set(LIBCXX_ENABLE_STATIC_ABI_LIBRARY ON CACHE BOOL "")

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
  ${LLVM_TOOLCHAIN_TOOLS}
  CACHE STRING "")

#  builtins-aie2
#  runtimes-aie2
