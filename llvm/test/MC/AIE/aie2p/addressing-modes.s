//===- addressing-modes.s ---------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: llvm-mc -triple aie2p %s -o - | FileCheck %s
// RUN: llvm-mc -triple aie2p %s -o %t -filetype=obj
// RUN: llvm-objdump --triple=aie2p -dr %t --no-print-imm-hex | FileCheck %s


// CHECK: lda r1, [p0, #0]
// CHECK: lda p3, [p1, #28]
// CHECK: lda lr, [sp, #-28]
// CHECK: lda p2, [sp, #-24]
// CHECK: lda r6, [sp, #-32]
// CHECK: st r4, [p2, #0]
// CHECK: st p6, [p1, #28]
// CHECK: st r0, [sp, #-32]
// CHECK: st p6, [sp, #-28]
// CHECK: vlda wl3, [p2, #-256]
// CHECK: vlda wl3, [sp, #-256]
// CHECK: vlda wl1, [p4], m1
// CHECK: vlda bmll3, [p2, #-256]
// CHECK: vlda bmll3, [sp, #-256]
// CHECK: vlda bmll1, [p4], m1
// CHECK: vlda wh6, [p5, dj0]
// CHECK: vldb wl3, [p0, #-128]
// CHECK: vldb wl1, [p4], m1
// CHECK: vlda.fill.512 [p1, lf1, r25]
// CHECK: vlda.pop.512 x0, [p0, lf0, r24, m3]
// CHECK: vlda.pop.512.2d x0, [p0, lf0, r24, d0]
// CHECK: vlda.pop.512.3d x0, [p0, lf0, r24, d2]
// CHECK: vlda.2d wl3, [p2], d0
// CHECK: vlda.2d bmll0, [p2], d0
// CHECK: vldb.2d wl3, [p2], d0
// CHECK: vlda.3d wl3, [p2], d0
// CHECK: vlda.3d bmll3, [p2], d0
// CHECK: vldb.3d wl2, [p1], d1
// CHECK: vst wl3, [p2, #0]
// CHECK: vst wl3, [sp, #-32]
// CHECK: vst wl3, [p2, dj2]
// CHECK: vst wl3, [p2], m4
// CHECK: vst wl3, [p2], #0
// CHECK: vst.2d wh6, [p5], d3
// CHECK: vst.2d bmll2, [p5], d3
// CHECK: vst.3d wh6, [p5], d3
// CHECK: vst.3d bmll4, [p5], d3
// CHECK: paddxm [sp], #64
// CHECK: padda [p0], #-64
// CHECK: padda.2d [p7], d4
// CHECK: padda.3d [p6], d3
// CHECK: paddxm [sp], #64
// CHECK: paddb [p0], #-64
// CHECK: paddb.2d [p7], d4
// CHECK: paddb.3d [p6], d3
// CHECK: padds [p0], #-64
// CHECK: padds.2d [p7], d4
// CHECK: padds.3d [p6], d3

    lda r1, [p0, #0]
    lda p3, [p1, #28]
    lda lr, [sp, #-28]
    lda p2, [sp, #-24]
    lda r6, [sp, #-32]

    st r4, [p2, #0]
    st p6, [p1, #28]
    st r0, [sp, #-32]
    st p6, [sp, #-28]

    vlda wl3, [p2, #-256]
    vlda wl3, [sp, #-256]
    vlda wl1, [p4], m1
    vlda bmll3, [p2, #-256]
    vlda bmll3, [sp, #-256]
    vlda bmll1, [p4], m1
    vlda wh6, [p5, dj0]
    vldb wl3, [p0, #-128]
    vldb wl1, [p4], m1

    vlda.fill.512 [p1, lf1, r25]
    vlda.pop.512 x0, [p0, lf0, r24, m3]
    vlda.pop.512.2d x0, [p0, lf0, r24, d0]
    vlda.pop.512.3d x0, [p0, lf0, r24, d2]

    vldb.pop.576  ex10, [p1, lf1, r25]
    vlda.2d wl3, [p2], d0
    vlda.2d bmll0, [p2], d0
    vldb.2d wl3, [p2], d0
    vlda.3d wl3, [p2], d0
    vlda.3d bmll3, [p2], d0
    vldb.3d wl2, [p1], d1

    vst wl3, [p2, #0]
    vst wl3, [sp, #-32]
    vst wl3, [p2, dj2]
    vst wl3, [p2], m4
    vst wl3, [p2], #0

    vst.2d wh6, [p5], d3
    vst.2d bmll2, [p5], d3
    vst.3d wh6, [p5], d3
    vst.3d bmll4, [p5], d3

    paddxm [sp], #64
    padda [p0], #-64
    padda.2d [p7], d4
    padda.3d [p6], d3

    paddxm [sp], #64
    paddb [p0], #-64
    paddb.2d [p7], d4
    paddb.3d [p6], d3

    padds [p0], #-64
    padds.2d [p7], d4
    padds.3d [p6], d3
