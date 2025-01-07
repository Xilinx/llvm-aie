//===-AIE2PDisassembler.inc -----------------------------*- tablegen
//-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//* Automatically generated file, do not edit! *

// Decoder Tables for Enumerator Register Classes.
static const unsigned eUnpackSignDecoderTable[] = {unpackSign0, unpackSign1};
static const unsigned eLDecoderTable[] = {l0, l1, l2,  l3,  l4,  l5,  l6,  l7,
                                          l8, l9, l10, l11, l12, l13, l14, l15};
TABLEBASEDDECODER(eL)
static const unsigned eBMSHLDecoderTable[] = {bmhl0, bmhl1, bmhl2, bmhl3};
static const unsigned eDCHDecoderTable[] = {dc4, dc5, dc6, dc7};
static const unsigned eQXsoDecoderTable[] = {qx1, qx3};
static const unsigned eEXoDecoderTable[] = {ex1, ex3, ex5, ex7, ex9, ex11};
static const unsigned eEXeDecoderTable[] = {ex0, ex2, ex4, ex6, ex8, ex10};
static const unsigned eEYDecoderTable[] = {ey0, ey1, ey2, ey3, ey4, ey5};
TABLEBASEDDECODER(eEY)
static const unsigned eLdFifoHRegDecoderTable[] = {lfh0, lfh1};
static const unsigned eBMLHDecoderTable[] = {bmlh0, bmlh1, bmlh2, bmlh3, bmlh4};
static const unsigned eUPSSignDecoderTable[] = {upsSign0, upsSign1};
static const unsigned ePackSignDecoderTable[] = {packSign0, packSign1};
static const unsigned eLdFifoLRegDecoderTable[] = {lfl0, lfl1};
static const unsigned eQEXsoDecoderTable[] = {qex1, qex3};
static const unsigned eDJHDecoderTable[] = {dj4, dj5, dj6, dj7};
static const unsigned eRDecoderTable[] = {
    r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,  r8,  r9,  r10,
    r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21,
    r22, r23, r24, r25, r26, r27, r28, r29, r30, r31};
TABLEBASEDDECODER(eR)
static const unsigned eRF2DecoderTable[] = {r24, r25};
static const unsigned eDNDecoderTable[] = {dn0, dn1, dn2, dn3,
                                           dn4, dn5, dn6, dn7};
static const unsigned eEloDecoderTable[] = {el1, el3, el5, el7, el9, el11};
static const unsigned eEhoDecoderTable[] = {eh1, eh3, eh5, eh7, eh9, eh11};
static const unsigned eBMHHDecoderTable[] = {bmhh0, bmhh1, bmhh2, bmhh3, bmhh4};
static const unsigned eDJDecoderTable[] = {dj0, dj1, dj2, dj3,
                                           dj4, dj5, dj6, dj7};
TABLEBASEDDECODER(eDJ)
static const unsigned eLdFifoRegDecoderTable[] = {lf0, lf1};
static const unsigned eQQsoDecoderTable[] = {q1, q3};
static const unsigned eDSDecoderTable[] = {d0_3d, d1_3d, d2_3d, d3_3d};

static const unsigned eDMDecoderTable[] = {dm0, dm1, dm2, dm3, dm4};
TABLEBASEDDECODER(eDM)
static const unsigned eQQseDecoderTable[] = {q0, q2};
static const unsigned eEheDecoderTable[] = {eh0, eh2, eh4, eh6, eh8, eh10};
static const unsigned eBMHLDecoderTable[] = {bmhl0, bmhl1, bmhl2, bmhl3, bmhl4};
static const unsigned eVaddSignDecoderTable[] = {vaddSign0, vaddSign1};
static const unsigned ePDecoderTable[] = {p0, p1, p2, p3, p4, p5, p6, p7};
TABLEBASEDDECODER(eP)
static const unsigned eDNHDecoderTable[] = {dn4, dn5, dn6, dn7};
static const unsigned eXoDecoderTable[] = {x1, x3, x5, x7, x9, x11};
static const unsigned eYDecoderTable[] = {llvm::AIE2P::y0, llvm::AIE2P::y1,
                                          llvm::AIE2P::y2, llvm::AIE2P::y3,
                                          llvm::AIE2P::y4, llvm::AIE2P::y5};
TABLEBASEDDECODER(eY)
static const unsigned eWhoDecoderTable[] = {wh1, wh3, wh5, wh7, wh9, wh11};
static const unsigned eDCDecoderTable[] = {dc0, dc1, dc2, dc3,
                                           dc4, dc5, dc6, dc7};
static const unsigned eRS16DecoderTable[] = {r16, r17, r18, r19, r20, r21,
                                             r22, r23, r24, r25, r26, r27,
                                             r28, r29, r30, r31};
TABLEBASEDDECODER(eRS16)
static const unsigned eXeDecoderTable[] = {x0, x2, x4, x6, x8, x10};
static const unsigned eSRSSignDecoderTable[] = {srsSign0, srsSign1};
static const unsigned eQYsDecoderTable[] = {qy0, qy1};
TABLEBASEDDECODER(eQYs)
static const unsigned eDNLDecoderTable[] = {dn0, dn1, dn2, dn3};
static const unsigned eQEYsDecoderTable[] = {qey0, qey1};
TABLEBASEDDECODER(eQEYs)
static const unsigned eBMSLHDecoderTable[] = {bmlh0, bmlh1, bmlh2, bmlh3};
static const unsigned eWheDecoderTable[] = {wh0, wh2, wh4, wh6, wh8, wh10};
static const unsigned eCMHDecoderTable[] = {cmh0, cmh1, cmh2, cmh3, cmh4};
static const unsigned eBMSLLDecoderTable[] = {bmll0, bmll1, bmll2, bmll3};
static const unsigned eEleDecoderTable[] = {el0, el2, el4, el6, el8, el10};
static const unsigned ePSDecoderTable[] = {p0, p1};
static const unsigned eQXseDecoderTable[] = {qx0, qx2};
static const unsigned eQEXseDecoderTable[] = {qex0, qex2};
static const unsigned eDDecoderTable[] = {d0, d1, d2, d3, d4, d5, d6, d7};
static const unsigned eWloDecoderTable[] = {wl1, wl3, wl5, wl7, wl9, wl11};
static const unsigned eSDecoderTable[] = {s0, s1, s2, s3};
TABLEBASEDDECODER(eS)
static const unsigned eWleDecoderTable[] = {wl0, wl2, wl4, wl6, wl8, wl10};
static const unsigned eCMLDecoderTable[] = {cml0, cml1, cml2, cml3, cml4};
static const unsigned eBMLLDecoderTable[] = {bmll0, bmll1, bmll2, bmll3, bmll4};
static const unsigned eMDecoderTable[] = {m0, m1, m2, m3, m4, m5, m6, m7};
TABLEBASEDDECODER(eM)
static const unsigned eBMSHHDecoderTable[] = {bmhh0, bmhh1, bmhh2, bmhh3};
static const unsigned eDJLDecoderTable[] = {dj0, dj1, dj2, dj3};
static const unsigned eDCLDecoderTable[] = {dc0, dc1, dc2, dc3};
// Decoder Tables for 'Compound' Register Classes.
static const unsigned mWaDecoderTable[] = {
    wh0, wl0, wh1, wl1, wh2, wl2, wh3, wl3, wh4,  wl4,  wh5,  wl5,
    wh6, wl6, wh7, wl7, wh8, wl8, wh9, wl9, wh10, wl10, wh11, wl11};
TABLEBASEDDECODER(mWa)
static const unsigned mXmDecoderTable[] = {x0, x1, x2, x3, x4,  x5,
                                           x6, x7, x8, x9, x10, x11};
TABLEBASEDDECODER(mXm)
static const unsigned mCMmDecoderTable[] = {cml0, cmh0, cml1, cmh1, cml2,
                                            cmh2, cml3, cmh3, cml4, cmh4};
TABLEBASEDDECODER(mCMm)
static const unsigned mQEXsmDecoderTable[] = {qex0, qex1, qex2, qex3};
TABLEBASEDDECODER(mQEXsm)
static const unsigned mEsDecoderTable[] = {
    eh0, el0, eh1, el1, eh2, el2, eh3, el3, eh4,  el4,  eh5,  el5,
    eh6, el6, eh7, el7, eh8, el8, eh9, el9, eh10, el10, eh11, el11};
static const unsigned mAguSrcDecoderTable[] = {
    r0,  dc0, p0, 0, r1,  dj0, 0, 0, r2,  dn0, 0, 0, r3,  m0, 0, 0,
    r4,  dc1, p1, 0, r5,  dj1, 0, 0, r6,  dn1, 0, 0, r7,  m1, 0, 0,
    r8,  dc2, p2, 0, r9,  dj2, 0, 0, r10, dn2, 0, 0, r11, m2, 0, 0,
    r12, dc3, p3, 0, r13, dj3, 0, 0, r14, dn3, 0, 0, r15, m3, 0, 0,
    r16, dc4, p4, 0, r17, dj4, 0, 0, r18, dn4, 0, 0, r19, m4, 0, 0,
    r20, dc5, p5, 0, r21, dj5, 0, 0, r22, dn5, 0, 0, r23, m5, 0, 0,
    r24, dc6, p6, 0, r25, dj6, 0, 0, r26, dn6, 0, 0, r27, m6, 0, 0,
    r28, dc7, p7, 0, r29, dj7, 0, 0, r30, dn7, 0, 0, r31, m7};
TABLEBASEDDECODER(mAguSrc)
static const unsigned mQXsbDecoderTable[] = {qx0, qx1, qx2, qx3};
static const unsigned mEhmDecoderTable[] = {eh0, eh1, eh2, eh3, eh4,  eh5,
                                            eh6, eh7, eh8, eh9, eh10, eh11};
TABLEBASEDDECODER(mEhm)
static const unsigned mLdaSclDecoderTable[] = {
    m0, eh0,  r0,  p0, dn0, el0,  r1,  lr, dj0, eh1,  r2,  0, dc0, el1,  r3,  0,
    m1, eh2,  r4,  p1, dn1, el2,  r5,  0,  dj1, eh3,  r6,  0, dc1, el3,  r7,  0,
    m2, eh4,  r8,  p2, dn2, el4,  r9,  0,  dj2, eh5,  r10, 0, dc2, el5,  r11, 0,
    m3, eh6,  r12, p3, dn3, el6,  r13, 0,  dj3, eh7,  r14, 0, dc3, el7,  r15, 0,
    m4, eh8,  r16, p4, dn4, el8,  r17, 0,  dj4, eh9,  r18, 0, dc4, el9,  r19, 0,
    m5, eh10, r20, p5, dn5, el10, r21, 0,  dj5, eh11, r22, 0, dc5, el11, r23, 0,
    m6, 0,    r24, p6, dn6, 0,    r25, 0,  dj6, 0,    r26, 0, dc6, 0,    r27, 0,
    m7, 0,    r28, p7, dn7, 0,    r29, 0,  dj7, 0,    r30, 0, dc7, 0,    r31};
TABLEBASEDDECODER(mLdaScl)
static const unsigned mQQsmDecoderTable[] = {q0, q1, q2, q3};
TABLEBASEDDECODER(mQQsm)
static const unsigned mWsDecoderTable[] = {
    wh0, wl0, wh1, wl1, wh2, wl2, wh3, wl3, wh4,  wl4,  wh5,  wl5,
    wh6, wl6, wh7, wl7, wh8, wl8, wh9, wl9, wh10, wl10, wh11, wl11};
TABLEBASEDDECODER(mWs)
static const unsigned mXwDecoderTable[] = {x0, x1, x2, x3, x4,  x5,
                                           x6, x7, x8, x9, x10, x11};
TABLEBASEDDECODER(mXw)
static const unsigned mAguDstDecoderTable[] = {
    dc0, dj0, dn0, m0, p0, 0, 0, 0, dc1, dj1, dn1, m1, p1, 0, 0, 0,
    dc2, dj2, dn2, m2, p2, 0, 0, 0, dc3, dj3, dn3, m3, p3, 0, 0, 0,
    dc4, dj4, dn4, m4, p4, 0, 0, 0, dc5, dj5, dn5, m5, p5, 0, 0, 0,
    dc6, dj6, dn6, m6, p6, 0, 0, 0, dc7, dj7, dn7, m7, p7};
TABLEBASEDDECODER(mAguDst)
static const unsigned mEXmDecoderTable[] = {ex0, ex1, ex2, ex3, ex4,  ex5,
                                            ex6, ex7, ex8, ex9, ex10, ex11};
TABLEBASEDDECODER(mEXm)
static const unsigned mQXsaDecoderTable[] = {qx0, qx1, qx2, qx3};
static const unsigned mDmDecoderTable[] = {
    m0, dn0, dj0, dc0, m1, dn1, dj1, dc1, m2, dn2, dj2, dc2, m3, dn3, dj3, dc3,
    m4, dn4, dj4, dc4, m5, dn5, dj5, dc5, m6, dn6, dj6, dc6, m7, dn7, dj7, dc7};
static const unsigned mQQssDecoderTable[] = {q0, q1, q2, q3};
TABLEBASEDDECODER(mQQss)
static const unsigned mQXsmDecoderTable[] = {qx0, qx1, qx2, qx3};
TABLEBASEDDECODER(mQXsm)
static const unsigned mShflBMDstDecoderTable[] = {
    bmll0, bmlh0, bmhl0, bmhh0, bmll1, bmlh1, bmhl1, bmhh1,
    bmll2, bmlh2, bmhl2, bmhh2, bmll3, bmlh3, bmhl3, bmhh3};
TABLEBASEDDECODER(mShflBMDst)
static const unsigned mBMmDecoderTable[] = {
    bmll0, bmlh0, bmhl0, bmhh0, bmll1, bmlh1, bmhl1, bmhh1, bmll2, bmlh2,
    bmhl2, bmhh2, bmll3, bmlh3, bmhl3, bmhh3, bmll4, bmlh4, bmhl4, bmhh4};
TABLEBASEDDECODER(mBMm)
static const unsigned mMvBMXDstDecoderTable[] = {
    bmll0, x0,    bmlh0, lfh0,  bmhl0, x1,    bmhh0, 0,     bmll1, x2,    bmlh1,
    lfl0,  bmhl1, x3,    bmhh1, 0,     bmll2, x4,    bmlh2, lfe,   bmhl2, x5,
    bmhh2, 0,     bmll3, x6,    bmlh3, sfl,   bmhl3, x7,    bmhh3, 0,     bmll4,
    x8,    bmlh4, lfh1,  bmhl4, x9,    bmhh4, 0,     0,     x10,   0,     lfl1,
    0,     x11,   0,     0,     0,     0,     0,     sfh};
TABLEBASEDDECODER(mMvBMXDst)
static const unsigned mShflXDstDecoderTable[] = {x0, x1, x2, x3, x4,  x5,
                                                 x6, x7, x8, x9, x10, x11};
TABLEBASEDDECODER(mShflXDst)
static const unsigned mMvSclDstDecoderTable[] = {m0,
                                                 r0,
                                                 srCarry,
                                                 upsSign0,
                                                 dn0,
                                                 r1,
                                                 p0,
                                                 crSCDEn,
                                                 dj0,
                                                 r2,
                                                 srF2BFlags,
                                                 crF2IMask,
                                                 dc0,
                                                 r3,
                                                 s0,
                                                 ls,
                                                 m1,
                                                 r4,
                                                 srF2FFlags,
                                                 vaddSign0,
                                                 dn1,
                                                 r5,
                                                 p1,
                                                 crUnpackSize,
                                                 dj1,
                                                 r6,
                                                 srF2IFlags,
                                                 crFPNlfMask,
                                                 dc1,
                                                 r7,
                                                 packSign0,
                                                 0,
                                                 m2,
                                                 r8,
                                                 srFPCnvFl2Fx,
                                                 unpackSign0,
                                                 dn2,
                                                 r9,
                                                 p2,
                                                 crSat,
                                                 dj2,
                                                 r10,
                                                 srFPCnvFx2Fl,
                                                 crFPCnvFx2FlMask,
                                                 dc2,
                                                 r11,
                                                 s1,
                                                 0,
                                                 m3,
                                                 r12,
                                                 srFPFlags,
                                                 crF2BMask,
                                                 dn3,
                                                 r13,
                                                 p3,
                                                 le,
                                                 dj3,
                                                 r14,
                                                 srFPNlf,
                                                 crPackSize,
                                                 dc3,
                                                 r15,
                                                 srsSign0,
                                                 0,
                                                 m4,
                                                 r16,
                                                 srFifo_of,
                                                 upsSign1,
                                                 dn4,
                                                 r17,
                                                 p4,
                                                 crSRSMode,
                                                 dj4,
                                                 r18,
                                                 srFifo_uf,
                                                 crFPCnvFl2FxMask,
                                                 dc4,
                                                 r19,
                                                 s2,
                                                 sp,
                                                 m5,
                                                 r20,
                                                 srMS0,
                                                 vaddSign1,
                                                 dn5,
                                                 r21,
                                                 p5,
                                                 lc,
                                                 dj5,
                                                 r22,
                                                 srSRS_of,
                                                 crMCDEn,
                                                 dc5,
                                                 r23,
                                                 packSign1,
                                                 0,
                                                 m6,
                                                 r24,
                                                 srSS0,
                                                 unpackSign1,
                                                 dn6,
                                                 r25,
                                                 p6,
                                                 crUPSMode,
                                                 dj6,
                                                 r26,
                                                 srSparse_of,
                                                 crFPMask,
                                                 dc6,
                                                 r27,
                                                 s3,
                                                 0,
                                                 m7,
                                                 r28,
                                                 srUPS_of,
                                                 crF2FMask,
                                                 dn7,
                                                 r29,
                                                 p7,
                                                 lr,
                                                 dj7,
                                                 r30,
                                                 0,
                                                 crRnd,
                                                 dc7,
                                                 r31,
                                                 srsSign1};
TABLEBASEDDECODER(mMvSclDst)
static const unsigned mMcdXSrcDecoderTable[] = {x0, x1, x2, x3, x4,  x5,
                                                x6, x7, x8, x9, x10, x11};
static const unsigned mEXnDecoderTable[] = {ex0, ex1, ex2, ex3, ex4,  ex5,
                                            ex6, ex7, ex8, ex9, ex10, ex11};
TABLEBASEDDECODER(mEXn)
static const unsigned mMvSclDstCgDecoderTable[] = {m0,
                                                   r0,
                                                   srCarry,
                                                   upsSign0,
                                                   dn0,
                                                   r1,
                                                   p0,
                                                   crSCDEn,
                                                   dj0,
                                                   r2,
                                                   srF2BFlags,
                                                   crF2IMask,
                                                   dc0,
                                                   r3,
                                                   s0,
                                                   ls,
                                                   m1,
                                                   r4,
                                                   srF2FFlags,
                                                   vaddSign0,
                                                   dn1,
                                                   r5,
                                                   p1,
                                                   crUnpackSize,
                                                   dj1,
                                                   r6,
                                                   srF2IFlags,
                                                   crFPNlfMask,
                                                   dc1,
                                                   r7,
                                                   packSign0,
                                                   0,
                                                   m2,
                                                   r8,
                                                   srFPCnvFl2Fx,
                                                   unpackSign0,
                                                   dn2,
                                                   r9,
                                                   p2,
                                                   crSat,
                                                   dj2,
                                                   r10,
                                                   srFPCnvFx2Fl,
                                                   crFPCnvFx2FlMask,
                                                   dc2,
                                                   r11,
                                                   s1,
                                                   0,
                                                   m3,
                                                   r12,
                                                   srFPFlags,
                                                   crF2BMask,
                                                   dn3,
                                                   r13,
                                                   p3,
                                                   le,
                                                   dj3,
                                                   r14,
                                                   srFPNlf,
                                                   crPackSize,
                                                   dc3,
                                                   r15,
                                                   srsSign0,
                                                   0,
                                                   m4,
                                                   r16,
                                                   srFifo_of,
                                                   upsSign1,
                                                   dn4,
                                                   r17,
                                                   p4,
                                                   crSRSMode,
                                                   dj4,
                                                   r18,
                                                   srFifo_uf,
                                                   crFPCnvFl2FxMask,
                                                   dc4,
                                                   r19,
                                                   s2,
                                                   sp,
                                                   m5,
                                                   r20,
                                                   srMS0,
                                                   vaddSign1,
                                                   dn5,
                                                   r21,
                                                   p5,
                                                   lc,
                                                   dj5,
                                                   r22,
                                                   srSRS_of,
                                                   crMCDEn,
                                                   dc5,
                                                   r23,
                                                   packSign1,
                                                   0,
                                                   m6,
                                                   r24,
                                                   srSS0,
                                                   unpackSign1,
                                                   dn6,
                                                   r25,
                                                   p6,
                                                   crUPSMode,
                                                   dj6,
                                                   r26,
                                                   srSparse_of,
                                                   crFPMask,
                                                   dc6,
                                                   r27,
                                                   s3,
                                                   0,
                                                   m7,
                                                   r28,
                                                   srUPS_of,
                                                   crF2FMask,
                                                   dn7,
                                                   r29,
                                                   p7,
                                                   lr,
                                                   dj7,
                                                   r30,
                                                   0,
                                                   crRnd,
                                                   dc7,
                                                   r31,
                                                   srsSign1};
TABLEBASEDDECODER(mMvSclDstCg)
static const unsigned mSclMSDecoderTable[] = {
    r0,  0, m0, 0, r1,  0, dn0, 0, r2,  0, dj0, 0, r3,  p0, dc0, 0,
    r4,  0, m1, 0, r5,  0, dn1, 0, r6,  0, dj1, 0, r7,  p1, dc1, 0,
    r8,  0, m2, 0, r9,  0, dn2, 0, r10, 0, dj2, 0, r11, p2, dc2, 0,
    r12, 0, m3, 0, r13, 0, dn3, 0, r14, 0, dj3, 0, r15, p3, dc3, 0,
    r16, 0, m4, 0, r17, 0, dn4, 0, r18, 0, dj4, 0, r19, p4, dc4, 0,
    r20, 0, m5, 0, r21, 0, dn5, 0, r22, 0, dj5, 0, r23, p5, dc5, 0,
    r24, 0, m6, 0, r25, 0, dn6, 0, r26, 0, dj6, 0, r27, p6, dc6, 0,
    r28, 0, m7, 0, r29, 0, dn7, 0, r30, 0, dj7, 0, r31, p7, dc7};
TABLEBASEDDECODER(mSclMS)
static const unsigned mXnDecoderTable[] = {x0, x1, x2, x3, x4,  x5,
                                           x6, x7, x8, x9, x10, x11};
TABLEBASEDDECODER(mXn)
static const unsigned mBMSmDecoderTable[] = {
    bmll0, bmlh0, bmhl0, bmhh0, bmll1, bmlh1, bmhl1, bmhh1,
    bmll2, bmlh2, bmhl2, bmhh2, bmll3, bmlh3, bmhl3, bmhh3};
static const unsigned mXvDecoderTable[] = {x0, x1, x2, x3, x4,  x5,
                                           x6, x7, x8, x9, x10, x11};
TABLEBASEDDECODER(mXv)
static const unsigned mWbDecoderTable[] = {
    wh0, wl0, wh1, wl1, wh2, wl2, wh3, wl3, wh4,  wl4,  wh5,  wl5,
    wh6, wl6, wh7, wl7, wh8, wl8, wh9, wl9, wh10, wl10, wh11, wl11};
TABLEBASEDDECODER(mWb)
static const unsigned mMvBMXSrcDecoderTable[] = {
    bmll0, x0,    bmlh0, lfh0,  bmhl0, x1,    bmhh0, 0,     bmll1, x2,    bmlh1,
    lfl0,  bmhl1, x3,    bmhh1, 0,     bmll2, x4,    bmlh2, lfe,   bmhl2, x5,
    bmhh2, 0,     bmll3, x6,    bmlh3, sfl,   bmhl3, x7,    bmhh3, 0,     bmll4,
    x8,    bmlh4, lfh1,  bmhl4, x9,    bmhh4, 0,     0,     x10,   0,     lfl1,
    0,     x11,   0,     0,     0,     0,     0,     sfh};
TABLEBASEDDECODER(mMvBMXSrc)
static const unsigned mAluCgDecoderTable[] = {
    r0,  lc, r1,  0, r2,  0, r3,  0, r4,  0, r5,  0, r6,  0, r7,  0,
    r8,  0,  r9,  0, r10, 0, r11, 0, r12, 0, r13, 0, r14, 0, r15, 0,
    r16, 0,  r17, 0, r18, 0, r19, 0, r20, 0, r21, 0, r22, 0, r23, 0,
    r24, 0,  r25, 0, r26, 0, r27, 0, r28, 0, r29, 0, r30, 0, r31};
TABLEBASEDDECODER(mAluCg)
static const unsigned mEXwDecoderTable[] = {ex0, ex1, ex2, ex3, ex4,  ex5,
                                            ex6, ex7, ex8, ex9, ex10, ex11};
TABLEBASEDDECODER(mEXw)
static const unsigned mXsDecoderTable[] = {x0, x1, x2, x3, x4,  x5,
                                           x6, x7, x8, x9, x10, x11};
TABLEBASEDDECODER(mXs)
static const unsigned mEXsDecoderTable[] = {ex0, ex1, ex2, ex3, ex4,  ex5,
                                            ex6, ex7, ex8, ex9, ex10, ex11};
TABLEBASEDDECODER(mEXs)
static const unsigned mCMsDecoderTable[] = {cml0, cmh0, cml1, cmh1, cml2,
                                            cmh2, cml3, cmh3, cml4, cmh4};
TABLEBASEDDECODER(mCMs)
static const unsigned mFifoHLRegDecoderTable[] = {lfh0, lfl0, lfe, sfl,
                                                  lfh1, lfl1, sfh};
TABLEBASEDDECODER(mFifoHLReg)
static const unsigned mSRmDecoderTable[] = {
    srCarry,      srF2BFlags, srF2FFlags, srF2IFlags,  srFPCnvFl2Fx,
    srFPCnvFx2Fl, srFPFlags,  srFPNlf,    srFifo_of,   srFifo_uf,
    srMS0,        srSRS_of,   srSS0,      srSparse_of, srUPS_of};
static const unsigned mElmDecoderTable[] = {el0, el1, el2, el3, el4,  el5,
                                            el6, el7, el8, el9, el10, el11};
TABLEBASEDDECODER(mElm)
static const unsigned mQEXswDecoderTable[] = {qex0, qex1, qex2, qex3};
TABLEBASEDDECODER(mQEXsw)
static const unsigned mQQsaDecoderTable[] = {q0, q1, q2, q3};
TABLEBASEDDECODER(mQQsa)
static const unsigned mEXvDecoderTable[] = {ex0, ex1, ex2, ex3, ex4,  ex5,
                                            ex6, ex7, ex8, ex9, ex10, ex11};
TABLEBASEDDECODER(mEXv)
static const unsigned mXaDecoderTable[] = {x0, x1, x2, x3, x4,  x5,
                                           x6, x7, x8, x9, x10, x11};
TABLEBASEDDECODER(mXa)
static const unsigned mWmDecoderTable[] = {
    wh0, wl0, wh1, wl1, wh2, wl2, wh3, wl3, wh4,  wl4,  wh5,  wl5,
    wh6, wl6, wh7, wl7, wh8, wl8, wh9, wl9, wh10, wl10, wh11, wl11};
TABLEBASEDDECODER(mWm)
static const unsigned mMvSclSrcDecoderTable[] = {m0,
                                                 r0,
                                                 srCarry,
                                                 upsSign0,
                                                 dn0,
                                                 r1,
                                                 p0,
                                                 crSCDEn,
                                                 dj0,
                                                 r2,
                                                 srF2BFlags,
                                                 crF2IMask,
                                                 dc0,
                                                 r3,
                                                 s0,
                                                 lr,
                                                 m1,
                                                 r4,
                                                 srF2FFlags,
                                                 vaddSign0,
                                                 dn1,
                                                 r5,
                                                 p1,
                                                 crUnpackSize,
                                                 dj1,
                                                 r6,
                                                 srF2IFlags,
                                                 crFPNlfMask,
                                                 dc1,
                                                 r7,
                                                 packSign0,
                                                 0,
                                                 m2,
                                                 r8,
                                                 srFPCnvFl2Fx,
                                                 unpackSign0,
                                                 dn2,
                                                 r9,
                                                 p2,
                                                 crSat,
                                                 dj2,
                                                 r10,
                                                 srFPCnvFx2Fl,
                                                 crFPCnvFx2FlMask,
                                                 dc2,
                                                 r11,
                                                 s1,
                                                 sp,
                                                 m3,
                                                 r12,
                                                 srFPFlags,
                                                 crF2BMask,
                                                 dn3,
                                                 r13,
                                                 p3,
                                                 lc,
                                                 dj3,
                                                 r14,
                                                 srFPNlf,
                                                 crPackSize,
                                                 dc3,
                                                 r15,
                                                 srsSign0,
                                                 0,
                                                 m4,
                                                 r16,
                                                 srFifo_of,
                                                 upsSign1,
                                                 dn4,
                                                 r17,
                                                 p4,
                                                 crSRSMode,
                                                 dj4,
                                                 r18,
                                                 srFifo_uf,
                                                 crFPCnvFl2FxMask,
                                                 dc4,
                                                 r19,
                                                 s2,
                                                 ls,
                                                 m5,
                                                 r20,
                                                 srMS0,
                                                 vaddSign1,
                                                 dn5,
                                                 r21,
                                                 p5,
                                                 CORE_ID,
                                                 dj5,
                                                 r22,
                                                 srSRS_of,
                                                 crMCDEn,
                                                 dc5,
                                                 r23,
                                                 packSign1,
                                                 0,
                                                 m6,
                                                 r24,
                                                 srSS0,
                                                 unpackSign1,
                                                 dn6,
                                                 r25,
                                                 p6,
                                                 crUPSMode,
                                                 dj6,
                                                 r26,
                                                 srSparse_of,
                                                 crFPMask,
                                                 dc6,
                                                 r27,
                                                 s3,
                                                 0,
                                                 m7,
                                                 r28,
                                                 srUPS_of,
                                                 crF2FMask,
                                                 dn7,
                                                 r29,
                                                 p7,
                                                 le,
                                                 dj7,
                                                 r30,
                                                 0,
                                                 crRnd,
                                                 dc7,
                                                 r31,
                                                 srsSign1};
TABLEBASEDDECODER(mMvSclSrc)
static const unsigned mQEXsaDecoderTable[] = {qex0, qex1, qex2, qex3};
static const unsigned mCRmDecoderTable[] = {crF2BMask,
                                            packSign0,
                                            crF2FMask,
                                            srsSign0,
                                            crF2IMask,
                                            upsSign0,
                                            crFPMask,
                                            unpackSign0,
                                            crPackSize,
                                            vaddSign0,
                                            crRnd,
                                            crMCDEn,
                                            crSRSMode,
                                            0,
                                            crSat,
                                            0,
                                            crUPSMode,
                                            packSign1,
                                            crUnpackSize,
                                            srsSign1,
                                            0,
                                            upsSign1,
                                            0,
                                            unpackSign1,
                                            0,
                                            vaddSign1,
                                            0,
                                            crSCDEn};
TABLEBASEDDECODER(mCRm)
static const unsigned mSclStDecoderTable[] = {
    m0, eh0,  r0,  p0, dn0, el0,  r1,  lr, dj0, eh1,  r2,  0, dc0, el1,  r3,  0,
    m1, eh2,  r4,  p1, dn1, el2,  r5,  0,  dj1, eh3,  r6,  0, dc1, el3,  r7,  0,
    m2, eh4,  r8,  p2, dn2, el4,  r9,  0,  dj2, eh5,  r10, 0, dc2, el5,  r11, 0,
    m3, eh6,  r12, p3, dn3, el6,  r13, 0,  dj3, eh7,  r14, 0, dc3, el7,  r15, 0,
    m4, eh8,  r16, p4, dn4, el8,  r17, 0,  dj4, eh9,  r18, 0, dc4, el9,  r19, 0,
    m5, eh10, r20, p5, dn5, el10, r21, 0,  dj5, eh11, r22, 0, dc5, el11, r23, 0,
    m6, 0,    r24, p6, dn6, 0,    r25, 0,  dj6, 0,    r26, 0, dc6, 0,    r27, 0,
    m7, 0,    r28, p7, dn7, 0,    r29, 0,  dj7, 0,    r30, 0, dc7, 0,    r31};
TABLEBASEDDECODER(mSclSt)
static const unsigned mMcdBMSrcDecoderTable[] = {
    bmll0, bmlh0, bmhl0, bmhh0, bmll1, bmlh1, bmhl1, bmhh1, bmll2, bmlh2,
    bmhl2, bmhh2, bmll3, bmlh3, bmhl3, bmhh3, bmll4, bmlh4, bmhl4, bmhh4};
static const unsigned mQXswDecoderTable[] = {qx0, qx1, qx2, qx3};
TABLEBASEDDECODER(mQXsw)
static const unsigned mEXaDecoderTable[] = {ex0, ex1, ex2, ex3, ex4,  ex5,
                                            ex6, ex7, ex8, ex9, ex10, ex11};
TABLEBASEDDECODER(mEXa)
static const unsigned mQEXsbDecoderTable[] = {qex0, qex1, qex2, qex3};
static const unsigned mLdaCgDecoderTable[] = {
    r0,  0, m0, 0, r1,  0,  dn0, 0, r2,  0, dj0, 0, r3,  p0, dc0, 0,
    r4,  0, m1, 0, r5,  lc, dn1, 0, r6,  0, dj1, 0, r7,  p1, dc1, 0,
    r8,  0, m2, 0, r9,  0,  dn2, 0, r10, 0, dj2, 0, r11, p2, dc2, 0,
    r12, 0, m3, 0, r13, 0,  dn3, 0, r14, 0, dj3, 0, r15, p3, dc3, 0,
    r16, 0, m4, 0, r17, 0,  dn4, 0, r18, 0, dj4, 0, r19, p4, dc4, 0,
    r20, 0, m5, 0, r21, 0,  dn5, 0, r22, 0, dj5, 0, r23, p5, dc5, 0,
    r24, 0, m6, 0, r25, 0,  dn6, 0, r26, 0, dj6, 0, r27, p6, dc6, 0,
    r28, 0, m7, 0, r29, 0,  dn7, 0, r30, 0, dj7, 0, r31, p7, dc7};
TABLEBASEDDECODER(mLdaCg)
static const unsigned mFl2FxSrc_WDecoderTable[] = {
    wh0, wl0, wh1, wl1, wh2, wl2, wh3, wl3, wh4,  wl4,  wh5,  wl5,
    wh6, wl6, wh7, wl7, wh8, wl8, wh9, wl9, wh10, wl10, wh11, wl11};
TABLEBASEDDECODER(mFl2FxSrc_W)
static const unsigned mXbDecoderTable[] = {x0, x1, x2, x3, x4,  x5,
                                           x6, x7, x8, x9, x10, x11};
TABLEBASEDDECODER(mXb)
static const unsigned mBMsDecoderTable[] = {
    bmll0, bmlh0, bmhl0, bmhh0, bmll1, bmlh1, bmhl1, bmhh1, bmll2, bmlh2,
    bmhl2, bmhh2, bmll3, bmlh3, bmhl3, bmhh3, bmll4, bmlh4, bmhl4, bmhh4};
TABLEBASEDDECODER(mBMs)
static const unsigned mEXbDecoderTable[] = {ex0, ex1, ex2, ex3, ex4,  ex5,
                                            ex6, ex7, ex8, ex9, ex10, ex11};

// Decoder Tables for Singleton Register Classes.
FIXEDREGDECODER(mR30_fifo_step_e1)
FIXEDREGDECODER(mL8m)
FIXEDREGDECODER(mR31_divs)
FIXEDREGDECODER(mR26_lock)
FIXEDREGDECODER(mS3)
FIXEDREGDECODER(mR30_shiftx)
FIXEDREGDECODER(mR27_select)
FIXEDREGDECODER(mS2)
FIXEDREGDECODER(mR30_fifo_step_e7)
FIXEDREGDECODER(mPfs)
FIXEDREGDECODER(mStFifo)
FIXEDREGDECODER(mLRx)
FIXEDREGDECODER(mSPs)
FIXEDREGDECODER(mR26_fifo_st)
FIXEDREGDECODER(mR31_scd)
FIXEDREGDECODER(mR28_tlast)
FIXEDREGDECODER(mSPa)
FIXEDREGDECODER(mR16_vcompare)
FIXEDREGDECODER(mR29_insert)
