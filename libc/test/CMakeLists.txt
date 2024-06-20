#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Modifications (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
add_custom_target(check-libc)
add_custom_target(libc-unit-tests)
add_custom_target(libc-hermetic-tests)
add_dependencies(check-libc libc-unit-tests libc-hermetic-tests)

add_custom_target(exhaustive-check-libc)
add_custom_target(libc-long-running-tests)

add_subdirectory(UnitTest)

if(LIBC_TARGET_ARCHITECTURE_IS_GPU AND
   (NOT TARGET libc.utils.gpu.loader OR NOT TARGET libc.startup.gpu.crt1))
  message(WARNING "Cannot build libc GPU tests, missing loader implementation")
  return()
endif()

add_subdirectory(src)

if(LLVM_LIBC_FULL_BUILD AND NOT LIBC_TARGET_OS_IS_BAREMETAL)
  add_subdirectory(utils)
  add_subdirectory(IntegrationTest)
endif()

if(NOT LLVM_LIBC_FULL_BUILD)
  return()
endif()

if(NOT ${LIBC_TARGET_OS} STREQUAL "linux" AND
   NOT ${LIBC_TARGET_OS} STREQUAL "gpu")
  # Integration tests are currently only available for linux and the GPU.
  return()
endif()
add_subdirectory(integration)
