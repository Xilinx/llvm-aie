#
# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates

# This file sets up a CMakeCache for the Peano AIE toolchain build.
# This is intended to be used *after* Peano-AIE-Windows.cmake to compile libc,
# since the 'runtimes' configuration mechanism is currently broken on windows

# $SRC points to your source tree.
# $BUILD points to your stage 1 build tree.
# $INSTALL points to the same install directory as the stage 1 build.

# Note: you might need to explicitly build the bin/libc-hdrgen.exe target
# Configure with:
# cmake -GNinja -S $SRC\runtimes \
#    -C $SRC\clang\cmake\caches\Peano-AIE-Windows-LIBC.cmake \
#    -DCMAKE_ASM_COMPILER="$BUILD/bin/clang.exe" \
#    -DCMAKE_C_COMPILER="$BUILD/bin/clang.exe" \
#    -DCMAKE_CXX_COMPILER="$BUILD/bin/clang++.exe" \
#    -DLIBC_HDRGEN_EXE="$BUILD\bin\libc-hdrgen.exe" \
#    -DCMAKE_C_COMPILER_WORKS=TRUE \
#    -DCMAKE_CXX_COMPILER_WORKS=TRUE \
#    -DCMAKE_INSTALL_PREFIX="$INSTALL"

set(CMAKE_BUILD_TYPE Release CACHE STRING "")
set(LLVM_ENABLE_ASSERTIONS ON CACHE BOOL "")
set(LLVM_ENABLE_TERMINFO OFF CACHE BOOL "")
set(BUILD_SHARED_LIBS OFF BOOL)

# Also need to set these to ensure libc-hdrgen gets built
set(LLVM_LIBC_FULL_BUILD
    ON
    CACHE BOOL "")

set(LLVM_ENABLE_RUNTIMES
    libc
    CACHE STRING "")

# Workaround the fact the cmake on windows is hardcoded to make windows
# library names (foo.lib) rather than posix library names (libfoo.a)
set(CMAKE_STATIC_LIBRARY_PREFIX CACHE STRING libc)
set(CMAKE_STATIC_LIBRARY_SUFFIX CACHE STRING ".a")

set(LIBC_TARGET_TRIPLE "aie2-none-unknown-elf" CACHE STRING "")

set(LLVM_CCACHE_BUILD ON BOOL)
