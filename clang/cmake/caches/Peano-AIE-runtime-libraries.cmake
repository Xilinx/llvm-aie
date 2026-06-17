#
# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates

# This file sets up the common options used for building the AIE compiler
# runtime libraries.

set(LLVM_ENABLE_RUNTIMES
      compiler-rt
      libc
      libcxx
      libcxxabi
    CACHE STRING "")
set(LLVM_LIBC_FULL_BUILD ON CACHE BOOL "")
set(LLVM_FORCE_BUILD_RUNTIME "libc" CACHE STRING "")
# deposits libc, libm, crt into lib/aie2-none-unknown-elf instead of lib/
set(LLVM_ENABLE_PER_TARGET_RUNTIME_DIR ON CACHE BOOL "")

set(LLVM_BUILTIN_TARGETS "aie-none-unknown-elf;aie2-none-unknown-elf;aie2p-none-unknown-elf;aie2ps-none-unknown-elf" CACHE STRING "")
set(LLVM_RUNTIME_TARGETS "${LLVM_BUILTIN_TARGETS}" CACHE STRING "")

foreach(target ${LLVM_BUILTIN_TARGETS})
  # Set the per-target cmake options.
  set(BUILTINS_${target}_CMAKE_BUILD_TYPE Release CACHE STRING "")
  set(RUNTIMES_${target}_CMAKE_BUILD_TYPE Release CACHE STRING "")
  set(BUILTINS_${target}_LLVM_USE_LINKER lld CACHE STRING "")
  set(RUNTIMES_${target}_LLVM_INCLUDE_TESTS OFF CACHE STRING "")
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

  set(RUNTIMES_${target}_LIBCXX_ENABLE_SHARED OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXX_ENABLE_STATIC_ABI_LIBRARY ON CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXX_ENABLE_FILESYSTEM OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXX_ENABLE_RANDOM_DEVICE OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXX_ENABLE_LOCALIZATION OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXX_ENABLE_UNICODE OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXX_ENABLE_WIDE_CHARACTERS OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXX_ENABLE_TIME_ZONE_DATABASE OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXX_USE_COMPILER_RT ON CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXX_ENABLE_EXCEPTIONS OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXX_ENABLE_RTTI OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXX_ENABLE_THREADS OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXX_ENABLE_MONOTONIC_CLOCK OFF CACHE STRING "")
  # disable new/delete in both libcxx and libcxxabi to work around missing alloc/free in libc
  set(RUNTIMES_${target}_LIBCXX_ENABLE_NEW_DELETE_DEFINITIONS OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXX_HEADER_ONLY ON CACHE STRING "")
  # Do not install C++03 frozen headers
  set(RUNTIMES_${target}_LIBCXX_INSTALL_CXX03_HEADERS OFF CACHE BOOL "")
  set(RUNTIMES_${target}_LIBCXX_INCLUDE_BENCHMARKS OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXX_INCLUDE_TESTS OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXX_EXTRA_SITE_DEFINES "_LIBCPP_REMOVE_TRANSITIVE_INCLUDES" CACHE STRING "")
  # disable inclusion of vitis headers for compiler checks
  set(RUNTIMES_${target}_CMAKE_REQUIRED_FLAGS "-mno-vitis-headers" CACHE STRING "")

  set(RUNTIMES_${target}_LIBC_ENABLE_USE_BY_CLANG ON CACHE STRING "")
  # LIBC includes C++ sources which by default trigger inclusion of standard libc++ headers
  # However these are not available while building libc, thus disable the include explicitly
  # Also disable inclusion of vitis headers to speed up compiler checks.
  set(RUNTIMES_${target}_LIBC_COMPILE_OPTIONS_DEFAULT "-nostdinc++;-mno-vitis-headers" CACHE STRING "" FORCE)

  # configure libcxxabi build
  set(RUNTIMES_${target}_LIBCXXABI_ENABLE_SHARED OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXXABI_ENABLE_EXCEPTIONS OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXXABI_USE_COMPILER_RT OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXXABI_ENABLE_THREADS OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXXABI_BAREMETAL ON CACHE STRING "")
  # disable new/delete in both libcxx and libcxxabi to work around missing alloc/free in libc
  set(RUNTIMES_${target}_LIBCXXABI_ENABLE_NEW_DELETE_DEFINITIONS OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXXABI_USE_LLVM_UNWINDER OFF CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXXABI_HEADER_ONLY ON CACHE STRING "")
  set(RUNTIMES_${target}_LIBCXXABI_INCLUDE_TESTS OFF CACHE STRING "")

  # Only build builtins from compiler-rt; disable all other subprojects
  set(RUNTIMES_${target}_COMPILER_RT_BUILD_SANITIZERS OFF CACHE BOOL "")
  set(RUNTIMES_${target}_COMPILER_RT_BUILD_XRAY OFF CACHE BOOL "")
  set(RUNTIMES_${target}_COMPILER_RT_BUILD_LIBFUZZER OFF CACHE BOOL "")
  set(RUNTIMES_${target}_COMPILER_RT_BUILD_PROFILE OFF CACHE BOOL "")
  set(RUNTIMES_${target}_COMPILER_RT_BUILD_CTX_PROFILE OFF CACHE BOOL "")
  set(RUNTIMES_${target}_COMPILER_RT_BUILD_MEMPROF OFF CACHE BOOL "")
  set(RUNTIMES_${target}_COMPILER_RT_BUILD_ORC OFF CACHE BOOL "")
  set(RUNTIMES_${target}_COMPILER_RT_BUILD_GWP_ASAN OFF CACHE BOOL "")
endforeach()
