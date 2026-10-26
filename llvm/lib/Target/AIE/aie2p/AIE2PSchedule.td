//=- AIE2PSchedule.td - AIE2p Scheduling Definitions --------*- tablegen -*-=//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

include "AIEBaseSchedule.td"
include "aie2p/AIE2PGenSchedule.td"

def AIE2PSchedModel : AIEBaseSchedModel<AIE2PItineraries>;
