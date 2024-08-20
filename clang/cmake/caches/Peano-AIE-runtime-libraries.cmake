#
# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates

# This file sets up the common options used for building the AIE compiler
# runtime libraries.

set(LLVM_ENABLE_RUNTIMES
      compiler-rt
      libc
    CACHE STRING "")
set(LLVM_LIBC_FULL_BUILD ON CACHE BOOL "")
set(LLVM_FORCE_BUILD_RUNTIME "libc" CACHE STRING "")

set(LLVM_BUILTIN_TARGETS "aie-none-unknown-elf;aie2-none-unknown-elf" CACHE STRING "")
set(LLVM_RUNTIME_TARGETS "${LLVM_BUILTIN_TARGETS}" CACHE STRING "")

foreach(target ${LLVM_BUILTIN_TARGETS})
  # Set the per-target cmake options.
  set(BUILTINS_${target}_CMAKE_BUILD_TYPE Release CACHE STRING "")
  set(RUNTIMES_${target}_CMAKE_BUILD_TYPE Release CACHE STRING "")
  set(BUILTINS_${target}_LLVM_USE_LINKER lld CACHE STRING "")
  set(BUILTINS_${target}_CMAKE_C_FLAGS "--target=${target}" CACHE STRING "")
  set(BUILTINS_${target}_CMAKE_CXX_FLAGS "--target=${target}" CACHE STRING "")
  set(BUILTINS_${target}_CMAKE_ASM_FLAGS "--target=${target}" CACHE STRING "")
  # The runtimes CMake system passes the LLVM_USE_LINKER option set for the host
  # compilation down to the runtimes build. That triggers a compile & link check
  # for the cross compiler, which does not have a complete sysroot yet.
  # As a workaround, pass -nostdlib to the cross compiler. The runtimes build
  # shouldn't expect the stdlibs to be available anyway
  set(BUILTINS_${target}_CMAKE_EXE_LINKER_FLAGS "-nostdlib" CACHE STRING "")
  set(RUNTIMES_${target}_CMAKE_EXE_LINKER_FLAGS "-nostdlib" CACHE STRING "")
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
