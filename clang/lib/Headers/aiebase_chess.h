//===- aiebase_chess.h ------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEBASE_CHESS_H
#define __AIEBASE_CHESS_H

#define chess_assert(x) (void)x
#define chess_const(x) __builtin_constant_p(x)
#define chess_copy(x) x
#define chess_dont_chain(x) (x)
#define chess_error(x)
#define chess_extra_options(x)
#define chess_flatten_loop
#define chess_keep_sw_loop
#define chess_keep_sw_loop2
#define chess_loop_count(x)
#define chess_loop_range(...)
#define chess_manifest(x) (__builtin_constant_p(x) && (x != 0))
#define chess_modulo_scheduling_budget_ratio(x)
#define chess_no_warn_pipelining
#define chess_no_warn_unroll
#define chess_output
#define chess_peel_pipelined_loop(x)
#define chess_pipeline_adjust_preamble(x)
#define chess_pipeline_initiation_interval(x)
#define chess_pipeline_non_leaf_loop_solution(x)
#define chess_prepare_for_pipelining
#define chess_property(x)
#define chess_protect_access
#define chess_require_pipelining(x)
#define chess_separator_scheduler()
#define chess_separator_scheduler_local()
#define chess_separator_scheduler(x)
#define chess_separator()
#define chess_storage(...)
#define chess_unroll_loop_preamble
#define chess_unroll_loop(__VA_ARGS__)
#define chess_warning(x)
#define chess_exit(x)
#define clobbers(...)
#define clobbers_not(...)
#define property(x)

#endif // __AIEBASE_CHESS_H
