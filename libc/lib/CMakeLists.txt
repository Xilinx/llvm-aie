#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Modifications (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
set(libc_archive_targets "")
set(libc_archive_names "")
set(libc_archive_entrypoint_lists "")
if(LLVM_LIBC_FULL_BUILD)
  if(LIBC_TARGET_ARCHITECTURE_IS_GPU)
    list(APPEND libc_archive_names cgpu mgpu)
  else()
    list(APPEND libc_archive_names c m)
  endif()
  list(APPEND libc_archive_targets libc libm)
  list(APPEND libc_archive_entrypoint_lists
       TARGET_LIBC_ENTRYPOINTS TARGET_LIBM_ENTRYPOINTS)
else()
  list(APPEND libc_archive_names llvmlibc)
  list(APPEND libc_archive_targets libc)
  list(APPEND libc_archive_entrypoint_lists TARGET_LLVMLIBC_ENTRYPOINTS)
endif()

set(added_archive_targets "")
foreach(archive IN ZIP_LISTS
        libc_archive_names libc_archive_targets libc_archive_entrypoint_lists)
  if(NOT ${archive_2})
    # If an entrypoint list is missing, then skip adding targets for it.
    continue()
  endif()
  add_entrypoint_library(
    ${archive_1}
    DEPENDS
      ${${archive_2}}
  )

  # The dependency below is necessary because it's possible for libc-headers to generate
  # and want to install headers that no component of ${archive_1} depend on.
  # This causes issues when running 'make install' because libc-headers is all implemented
  # with custom targets and in older version of cmake (~pre-3.20, apparently)
  # there is not good support for describing install dependencies on custom targets.
  add_dependencies(${archive_1} libc-headers)

  set_target_properties(
    ${archive_1}
    PROPERTIES
      ARCHIVE_OUTPUT_NAME ${archive_0}
  )
  list(APPEND added_archive_targets ${archive_1})
endforeach()

if(LIBC_TARGET_TRIPLE)
  set(LIBC_INSTALL_LIBRARY_DIR lib${LLVM_LIBDIR_SUFFIX}/${LIBC_TARGET_TRIPLE})
elseif(LLVM_ENABLE_PER_TARGET_RUNTIME_DIR AND NOT LIBC_GPU_BUILD)
  set(LIBC_INSTALL_LIBRARY_DIR
      lib${LLVM_LIBDIR_SUFFIX}/${LLVM_DEFAULT_TARGET_TRIPLE})
else()
  set(LIBC_INSTALL_LIBRARY_DIR lib${LLVM_LIBDIR_SUFFIX})
endif()

install(
  TARGETS ${added_archive_targets}
  ARCHIVE DESTINATION ${LIBC_INSTALL_LIBRARY_DIR}
  COMPONENT libc-static-archives
)

add_llvm_install_targets(
  install-libc-static-archives
  DEPENDS ${added_archive_targets}
  COMPONENT libc-static-archives
)
