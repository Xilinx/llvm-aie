//===- AIE2PSlotsInclude.td -----------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

let Namespace = "AIE2P" in
{
    // Cooked up Slot for 16-bit InsnFormat.
    // In AIE2P this has a single instruction nop. We keep the size small, so that
    // we can reuse it as artificial nop slot in other formats. This is another
    // trick, used to disambiguate equivalent formats with different size. 
    // As prime example, XM fits stand-alone in 48 bit and in 64 bits, where we
    // want to use the bigger one for alignmment purposes. We tell the encoder
    // which one to use by adding a nop instruction in the bundle. The encoder
    // will find the smallest format covering these two slots, and end up at the
    // 64 bit one. 
    def nop_slot     : InstSlot<"Nop", 1> {
        let FieldToFind = "nop";
        let Artificial = true;
    }
}
