#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Modifications (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates

# ------------------------------------------------------------------------------
# Compiler features definition and flags
# ------------------------------------------------------------------------------

# Reports wall-clock (HH:MM:SS.mmm) and epoch milliseconds for probe timing.
# Falls back to whole-second resolution if `date` is unavailable (non-POSIX host).
if(NOT COMMAND _libc_probe_now)
  function(_libc_probe_now out_wall out_epoch_ms)
    execute_process(
      COMMAND date "+%H:%M:%S.%3N %s%3N"
      OUTPUT_VARIABLE now OUTPUT_STRIP_TRAILING_WHITESPACE
      RESULT_VARIABLE rc ERROR_QUIET)
    if(rc EQUAL 0)
      separate_arguments(now)
      list(GET now 0 wall)
      list(GET now 1 epoch_ms)
    else()
      string(TIMESTAMP wall "%H:%M:%S")
      string(TIMESTAMP epoch_s "%s")
      math(EXPR epoch_ms "${epoch_s} * 1000")
    endif()
    set(${out_wall} "${wall}" PARENT_SCOPE)
    set(${out_epoch_ms} "${epoch_ms}" PARENT_SCOPE)
  endfunction()
endif()

set(
  ALL_COMPILER_FEATURES
    "builtin_ceil_floor_rint_trunc"
    "builtin_fmax_fmin"
    "builtin_fmaxf16_fminf16"
    "builtin_round"
    "builtin_roundeven"
    "float16"
    "float16_conversion"
    "float128"
    "fixed_point"
    "cfloat16"
    "cfloat128"
)

# Making sure ALL_COMPILER_FEATURES is sorted.
list(SORT ALL_COMPILER_FEATURES)

# Compiler features that are unavailable on GPU targets with the in-tree Clang.
set(
  CPU_ONLY_COMPILER_FEATURES
    "float128"
)

# Function to check whether the compiler supports the provided set of features.
# Usage:
# compiler_supports(
#   <output variable>
#   <list of cpu features>
# )
function(compiler_supports output_var features)
  _intersection(var "${LIBC_CPU_FEATURES}" "${features}")
  if("${var}" STREQUAL "${features}")
    set(${output_var} TRUE PARENT_SCOPE)
  else()
    unset(${output_var} PARENT_SCOPE)
  endif()
endfunction()

# ------------------------------------------------------------------------------
# Internal helpers and utilities.
# ------------------------------------------------------------------------------

# Computes the intersection between two lists.
function(_intersection output_var list1 list2)
  foreach(element IN LISTS list1)
    if("${list2}" MATCHES "(^|;)${element}(;|$)")
      list(APPEND tmp "${element}")
    endif()
  endforeach()
  set(${output_var} ${tmp} PARENT_SCOPE)
endfunction()

set(AVAILABLE_COMPILER_FEATURES "")

# Try compile a C file to check if flag is supported.
foreach(feature IN LISTS ALL_COMPILER_FEATURES)
  set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
  set(compile_options ${LIBC_COMPILE_OPTIONS_NATIVE})
  set(link_options "")
  if(${feature} STREQUAL "fixed_point")
    list(APPEND compile_options "-ffixed-point")
  elseif(${feature} MATCHES "^builtin_" OR
         ${feature} STREQUAL "float16_conversion")
    set(compile_options ${LIBC_COMPILE_OPTIONS_DEFAULT})
    set(link_options -nostdlib)
    # The compiler might handle calls to math builtins by generating calls to
    # the respective libc math functions, in which case we cannot use these
    # builtins in our implementations of these functions. We check that this is
    # not the case by trying to link an executable, since linking would fail due
    # to unresolved references with -nostdlib if calls to libc functions were
    # generated.
    #
    # We also had issues with soft-float float16 conversion functions using both
    # compiler-rt and libgcc, so we also check whether we can convert from and
    # to float16 without calls to compiler runtime functions by trying to link
    # an executable with -nostdlib.
    set(CMAKE_TRY_COMPILE_TARGET_TYPE EXECUTABLE)
  endif()

  set(check_src ${LIBC_SOURCE_DIR}/cmake/modules/compiler_features/check_${feature}.cpp)
  _libc_probe_now(probe_wall probe_start_ms)
  message(STATUS "[${probe_wall}] compiler feature probe '${feature}' start (src: ${check_src})")

  if(LIBC_TARGET_OS_IS_GPU)
    # CUDA shouldn't be required to build the libc, only to test it, so we can't
    # try to build CUDA binaries here. Since GPU builds are always compiled with
    # the in-tree Clang, we just hardcode which compiler features are available
    # when targeting GPUs.
    if(feature IN_LIST CPU_ONLY_COMPILER_FEATURES)
      set(has_feature FALSE)
    else()
      set(has_feature TRUE)
    endif()
  else()
    try_compile(
      has_feature
      ${CMAKE_CURRENT_BINARY_DIR}/compiler_features
      SOURCES ${check_src}
      COMPILE_DEFINITIONS -I${LIBC_SOURCE_DIR} ${compile_options}
      LINK_OPTIONS ${link_options}
    )
  endif()

  _libc_probe_now(probe_wall probe_end_ms)
  math(EXPR probe_ms "${probe_end_ms} - ${probe_start_ms}")
  message(STATUS "[${probe_wall}] compiler feature probe '${feature}' done ${probe_ms}ms (supported: ${has_feature})")

  if(has_feature)
    list(APPEND AVAILABLE_COMPILER_FEATURES ${feature})
    if(${feature} STREQUAL "float16")
      set(LIBC_TYPES_HAS_FLOAT16 TRUE)
    elseif(${feature} STREQUAL "float16_conversion")
      add_compile_definitions(__LIBC_USE_FLOAT16_CONVERSION)
    elseif(${feature} STREQUAL "float128")
      set(LIBC_TYPES_HAS_FLOAT128 TRUE)
    elseif(${feature} STREQUAL "fixed_point")
      set(LIBC_COMPILER_HAS_FIXED_POINT TRUE)
    elseif(${feature} STREQUAL "cfloat16")
      set(LIBC_TYPES_HAS_CFLOAT16 TRUE)
    elseif(${feature} STREQUAL "cfloat128")
      set(LIBC_TYPES_HAS_CFLOAT128 TRUE)
    elseif(${feature} STREQUAL "builtin_ceil_floor_rint_trunc")
      set(LIBC_COMPILER_HAS_BUILTIN_CEIL_FLOOR_RINT_TRUNC TRUE)
    elseif(${feature} STREQUAL "builtin_fmax_fmin")
      set(LIBC_COMPILER_HAS_BUILTIN_FMAX_FMIN TRUE)
    elseif(${feature} STREQUAL "builtin_fmaxf16_fminf16")
      set(LIBC_COMPILER_HAS_BUILTIN_FMAXF16_FMINF16 TRUE)
    elseif(${feature} STREQUAL "builtin_round")
      set(LIBC_COMPILER_HAS_BUILTIN_ROUND TRUE)
    elseif(${feature} STREQUAL "builtin_roundeven")
      set(LIBC_COMPILER_HAS_BUILTIN_ROUNDEVEN TRUE)
    endif()
  endif()
endforeach()

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(compile_options ${LIBC_COMPILE_OPTIONS_DEFAULT})
set(link_options "")

message(STATUS "Compiler features available: ${AVAILABLE_COMPILER_FEATURES}")

### Compiler Feature Detection ###

# clang-8+, gcc-12+
check_cxx_compiler_flag("-ftrivial-auto-var-init=pattern" LIBC_CC_SUPPORTS_PATTERN_INIT)

# clang-6+, gcc-13+
check_cxx_compiler_flag("-nostdlib++" LIBC_CC_SUPPORTS_NOSTDLIBPP)

# clang-3.0+
check_cxx_compiler_flag("-nostdlibinc" LIBC_CC_SUPPORTS_NOSTDLIBINC)
