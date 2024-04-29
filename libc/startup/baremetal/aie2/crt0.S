//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
.global __start
.type __start, STT_FUNC
.global _sp_start_value_DM_stack
.global _main_init
__start:
        MOVXM sp, #_sp_start_value_DM_stack
        JL  #_main_init
        NOP
        NOP
        NOP
        NOP
        NOP
