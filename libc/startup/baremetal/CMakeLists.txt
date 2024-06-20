#
# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
function(add_startup_object name)
  cmake_parse_arguments(
    "ADD_STARTUP_OBJECT"
    "ALIAS" # Option argument
    "SRC"   # Single value arguments
    "DEPENDS;COMPILE_OPTIONS" # Multi value arguments
    ${ARGN}
  )

  get_fq_target_name(${name} fq_target_name)
  if(ADD_STARTUP_OBJECT_ALIAS)
    get_fq_deps_list(fq_dep_list ${ADD_STARTUP_OBJECT_DEPENDS})
    add_library(${fq_target_name} ALIAS ${fq_dep_list})
    return()
  endif()

  add_object_library(
    ${name}
    SRCS ${ADD_STARTUP_OBJECT_SRC}
    DEPENDS ${ADD_STARTUP_OBJECT_DEPENDS}
    COMPILE_OPTIONS ${ADD_STARTUP_OBJECT_COMPILE_OPTIONS}
  )
  set_target_properties(
    ${fq_target_name}
    PROPERTIES
      OUTPUT_NAME ${name}.o
  )
endfunction()

if(NOT (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LIBC_TARGET_ARCHITECTURE}))
  message(STATUS "Skipping startup for target architecture ${LIBC_TARGET_ARCHITECTURE}")
  return()
endif()

add_subdirectory(${LIBC_TARGET_ARCHITECTURE})

add_startup_object(
  crt0
  ALIAS
  DEPENDS
    .${LIBC_TARGET_ARCHITECTURE}.crt0
)

add_startup_object(
  crt1
  ALIAS
  DEPENDS
    .${LIBC_TARGET_ARCHITECTURE}.crt1
)

if(LIBC_TARGET_TRIPLE)
  set(LIBC_INSTALL_LIBRARY_DIR lib${LLVM_LIBDIR_SUFFIX}/${LIBC_TARGET_TRIPLE})
elseif(LLVM_ENABLE_PER_TARGET_RUNTIME_DIR AND NOT LIBC_GPU_BUILD)
  set(LIBC_INSTALL_LIBRARY_DIR
      lib${LLVM_LIBDIR_SUFFIX}/${LLVM_DEFAULT_TARGET_TRIPLE})
else()
  set(LIBC_INSTALL_LIBRARY_DIR lib${LLVM_LIBDIR_SUFFIX})
endif()

add_custom_target(libc-startup)
set(startup_components crt0 crt1)
foreach(target IN LISTS startup_components)
  set(fq_target_name libc.startup.baremetal.${target})
  add_dependencies(libc-startup ${fq_target_name})
  install(FILES $<TARGET_OBJECTS:${fq_target_name}>
          DESTINATION ${LIBC_INSTALL_LIBRARY_DIR}
          RENAME $<TARGET_PROPERTY:${fq_target_name},OUTPUT_NAME>
          COMPONENT libc)
endforeach()
