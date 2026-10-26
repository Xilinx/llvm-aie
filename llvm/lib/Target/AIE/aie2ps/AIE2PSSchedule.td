//=- AIE2PSSchedule.td - AIE2ps Scheduling Definitions --------*- tablegen -*-=//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
include "AIEBaseSchedule.td"
include "aie2ps/AIE2PSGenSchedule.td"

def AIE2PSSchedModel : AIEBaseSchedModel<AIE2PSItineraries>;
