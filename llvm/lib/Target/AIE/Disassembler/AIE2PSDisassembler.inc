//===-AIE2PSDisassembler.inc.cpp -----------------------------*- tablegen
//-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//* Automatically generated file, do not edit! *

// Decoder Tables for Enumerator Register Classes.
static const unsigned eEYsDecoderTable[] = {ey0, ey1, ey2, ey3};
static const unsigned eDJDecoderTable[] = {dj0, dj1, dj2, dj3,
                                           dj4, dj5, dj6, dj7};
TABLEBASEDDECODER(eDJ)
static const unsigned eEY_sDecoderTable[] = {ey0, ey1, ey2, ey3,
                                             ey4, ey5, 0,   0};
TABLEBASEDDECODER(eEY_s)
static const unsigned eDSDecoderTable[] = {d0_3d, d1_3d, d2_3d, d3_3d};

static const unsigned eDMDecoderTable[] = {dm0, dm1, dm2, dm3,
                                           dm4, dm5, dm6, dm7};
TABLEBASEDDECODER(eDM)
static const unsigned eDNHDecoderTable[] = {dn4, dn5, dn6, dn7};
static const unsigned eDCDecoderTable[] = {dc0, dc1, dc2, dc3,
                                           dc4, dc5, dc6, dc7};
static const unsigned eCMHDecoderTable[] = {cmh0, cmh1, cmh2, cmh3,
                                            cmh4, cmh5, cmh6, cmh7};
TABLEBASEDDECODER(eCMH)
static const unsigned eCMLDecoderTable[] = {cml0, cml1, cml2, cml3,
                                            cml4, cml5, cml6, cml7};
TABLEBASEDDECODER(eCML)
static const unsigned eDJLDecoderTable[] = {dj0, dj1, dj2, dj3};
static const unsigned eLDecoderTable[] = {l0, l1, l2,  l3,  l4,  l5,  l6,  l7,
                                          l8, l9, l10, l11, l12, l13, l14, l15};
TABLEBASEDDECODER(eL)
static const unsigned eDCHDecoderTable[] = {dc4, dc5, dc6, dc7};
static const unsigned eDJHDecoderTable[] = {dj4, dj5, dj6, dj7};
static const unsigned eDNDecoderTable[] = {dn0, dn1, dn2, dn3,
                                           dn4, dn5, dn6, dn7};
static const unsigned eLdFifoRegDecoderTable[] = {lf0, lf1};
static const unsigned ePDecoderTable[] = {p0, p1, p2, p3, p4, p5, p6, p7};
TABLEBASEDDECODER(eP)
static const unsigned eYDecoderTable[] = {llvm::AIE2PS::y0,
                                          llvm::AIE2PS::y1,
                                          llvm::AIE2PS::y2,
                                          llvm::AIE2PS::y3,
                                          llvm::AIE2PS::y4,
                                          llvm::AIE2PS::y5,
                                          0,
                                          0};
TABLEBASEDDECODER(eY)
static const unsigned eFEYDecoderTable[] = {fey0, fey1, fey2, fey3,
                                            fey4, fey5, 0,    0};
static const unsigned eDNLDecoderTable[] = {dn0, dn1, dn2, dn3};
static const unsigned ePSDecoderTable[] = {p0, p1};
static const unsigned eDDecoderTable[] = {d0, d1, d2, d3, d4, d5, d6, d7};
static const unsigned eSDecoderTable[] = {s0, s1, s2, s3};
TABLEBASEDDECODER(eS)
static const unsigned eMDecoderTable[] = {m0, m1, m2, m3, m4, m5, m6, m7};
TABLEBASEDDECODER(eM)
static const unsigned eDCLDecoderTable[] = {dc0, dc1, dc2, dc3};
// Decoder Tables for 'Compound' Register Classes.
static const unsigned mFEXvDecoderTable[] = {
    fex0, fex1, fex2,  fex3,  fex4, fex5, fex6, fex7,
    fex8, fex9, fex10, fex11, 0,    0,    0,    0};
static const unsigned mCMmDecoderTable[] = {cml0, cmh0, cml1, cmh1, cml2, cmh2,
                                            cml3, cmh3, cml4, cmh4, cml5, cmh5,
                                            cml6, cmh6, cml7, cmh7};
TABLEBASEDDECODER(mCMm)
static const unsigned mCRxDecoderTable[] = {crBF8conf,
                                            packSign0,
                                            crF2BMask,
                                            packSign0,
                                            crF2FMask,
                                            srsSign0,
                                            crF2IMask,
                                            srsSign0,
                                            crFP8conf,
                                            upsSign0,
                                            crFPCnvFl2FxMask,
                                            upsSign0,
                                            crFPCnvFx2FlMask,
                                            unpackSign0,
                                            crFPConvSat,
                                            unpackSign0,
                                            crFPMask,
                                            vaddSign0,
                                            crFPNlfMask,
                                            vaddSign0,
                                            crMCDEn,
                                            0,
                                            crPackSize,
                                            0,
                                            crRnd,
                                            0,
                                            crSCDEn,
                                            0,
                                            crSRSMode,
                                            0,
                                            crSat,
                                            0,
                                            crUPSMode,
                                            packSign1,
                                            crUnpackSize,
                                            packSign1,
                                            0,
                                            srsSign1,
                                            0,
                                            srsSign1,
                                            0,
                                            upsSign1,
                                            0,
                                            upsSign1,
                                            0,
                                            unpackSign1,
                                            0,
                                            unpackSign1,
                                            0,
                                            vaddSign1,
                                            0,
                                            vaddSign1,
                                            0,
                                            0,
                                            0,
                                            0,
                                            0,
                                            0,
                                            0,
                                            0,
                                            0,
                                            0,
                                            0,
                                            0};
TABLEBASEDDECODER(mCRx)
static const unsigned mAguSrcDecoderTable[] = {
    r0,  dc0, p0, 0, r1,  dj0, 0, 0, r2,  dn0, 0, 0, r3,  m0, 0, 0,
    r4,  dc1, p1, 0, r5,  dj1, 0, 0, r6,  dn1, 0, 0, r7,  m1, 0, 0,
    r8,  dc2, p2, 0, r9,  dj2, 0, 0, r10, dn2, 0, 0, r11, m2, 0, 0,
    r12, dc3, p3, 0, r13, dj3, 0, 0, r14, dn3, 0, 0, r15, m3, 0, 0,
    r16, dc4, p4, 0, r17, dj4, 0, 0, r18, dn4, 0, 0, r19, m4, 0, 0,
    r20, dc5, p5, 0, r21, dj5, 0, 0, r22, dn5, 0, 0, r23, m5, 0, 0,
    r24, dc6, p6, 0, r25, dj6, 0, 0, r26, dn6, 0, 0, r27, m6, 0, 0,
    r28, dc7, p7, 0, r29, dj7, 0, 0, r30, dn7, 0, 0, r31, m7, 0, 0};
TABLEBASEDDECODER(mAguSrc)
static const unsigned mEGmDecoderTable[] = {
    egl0,  egh0,  egl1, egh1, egl2, egh2, egl3, egh3, egl4, egh4,  egl5,
    egh5,  egl6,  egh6, egl7, egh7, egl8, egh8, egl9, egh9, egl10, egh10,
    egl11, egh11, 0,    0,    0,    0,    0,    0,    0,    0};
TABLEBASEDDECODER(mEGm)
static const unsigned mBMaDecoderTable[] = {
    bmll0, bmlh0, bmhl0, bmhh0, bmll1, bmlh1, bmhl1, bmhh1, bmll2, bmlh2, bmhl2,
    bmhh2, bmll3, bmlh3, bmhl3, bmhh3, bmll4, bmlh4, bmhl4, bmhh4, bmll5, bmlh5,
    bmhl5, bmhh5, bmll6, bmlh6, bmhl6, bmhh6, bmll7, bmlh7, bmhl7, bmhh7};
TABLEBASEDDECODER(mBMa)
static const unsigned mEG2aDecoderTable[] = {
    eg0, eg1, eg2, eg3, eg4, eg5, eg6, eg7, eg8, eg9, eg10, eg11, 0, 0, 0, 0};
TABLEBASEDDECODER(mEG2a)
static const unsigned mEhmDecoderTable[] = {
    eh0, eh1, eh2, eh3, eh4, eh5, eh6, eh7, eh8, eh9, eh10, eh11, 0, 0, 0, 0};
TABLEBASEDDECODER(mEhm)
static const unsigned mXwDecoderTable[] = {x0, x1, x2,  x3,  x4, x5, x6, x7,
                                           x8, x9, x10, x11, 0,  0,  0,  0};
TABLEBASEDDECODER(mXw)
static const unsigned mRv8DecoderTable[] = {r8,  r9,  r10, r11,
                                            r12, r13, r14, r15};
TABLEBASEDDECODER(mRv8)
static const unsigned mShflBMDstDecoderTable[] = {
    bmll0, bmlh0, bmhl0, bmhh0, bmll1, bmlh1, bmhl1, bmhh1,
    bmll2, bmlh2, bmhl2, bmhh2, bmll3, bmlh3, bmhl3, bmhh3};
TABLEBASEDDECODER(mShflBMDst)
static const unsigned mFEXaDecoderTable[] = {
    fex0, fex1, fex2,  fex3,  fex4, fex5, fex6, fex7,
    fex8, fex9, fex10, fex11, 0,    0,    0,    0};
static const unsigned mFEG2sDecoderTable[] = {
    feg0, feg1, feg2,  feg3,  feg4, feg5, feg6, feg7,
    feg8, feg9, feg10, feg11, 0,    0,    0,    0};
TABLEBASEDDECODER(mFEG2s)
static const unsigned mFaDecoderTable[] = {
    fl0,  fh0,  fl1, fh1, fl2, fh2, fl3, fh3, fl4, fh4,  fl5,
    fh5,  fl6,  fh6, fl7, fh7, fl8, fh8, fl9, fh9, fl10, fh10,
    fl11, fh11, 0,   0,   0,   0,   0,   0,   0,   0};
TABLEBASEDDECODER(mFa)
static const unsigned mGhmDecoderTable[] = {
    gh0, gh1, gh2, gh3, gh4, gh5, gh6, gh7, gh8, gh9, gh10, gh11, 0, 0, 0, 0};
TABLEBASEDDECODER(mGhm)
static const unsigned mMvSclDstDecoderTable[] = {
    m0,  r0,  packSign0,   0, dn0, r1,  p0,          0, dj0, r2,  srsSign0,  0,
    dc0, r3,  s0,          0, m1,  r4,  upsSign0,    0, dn1, r5,  p1,        0,
    dj1, r6,  unpackSign0, 0, dc1, r7,  lc,          0, m2,  r8,  vaddSign0, 0,
    dn2, r9,  p2,          0, dj2, r10, 0,           0, dc2, r11, s1,        0,
    m3,  r12, 0,           0, dn3, r13, p3,          0, dj3, r14, 0,         0,
    dc3, r15, lr,          0, m4,  r16, packSign1,   0, dn4, r17, p4,        0,
    dj4, r18, srsSign1,    0, dc4, r19, s2,          0, m5,  r20, upsSign1,  0,
    dn5, r21, p5,          0, dj5, r22, unpackSign1, 0, dc5, r23, sp,        0,
    m6,  r24, vaddSign1,   0, dn6, r25, p6,          0, dj6, r26, 0,         0,
    dc6, r27, s3,          0, m7,  r28, 0,           0, dn7, r29, p7,        0,
    dj7, r30, 0,           0, dc7, r31, 0,           0};
TABLEBASEDDECODER(mMvSclDst)
static const unsigned mRV16sDecoderTable[] = {
    r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15};
TABLEBASEDDECODER(mRV16s)
static const unsigned mRsDecoderTable[] = {
    r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,  r8,  r9,  r10,
    r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21,
    r22, r23, r24, r25, r26, r27, r28, r29, r30, r31};
TABLEBASEDDECODER(mRs)
static const unsigned mWbDecoderTable[] = {
    wl0,  wh0,  wl1, wh1, wl2, wh2, wl3, wh3, wl4, wh4,  wl5,
    wh5,  wl6,  wh6, wl7, wh7, wl8, wh8, wl9, wh9, wl10, wh10,
    wl11, wh11, 0,   0,   0,   0,   0,   0,   0,   0};
TABLEBASEDDECODER(mWb)
static const unsigned mMvBMXSrcDecoderTable[] = {
    bmll0, x0,    bmlh0, lfh0,  bmhl0, x1,    bmhh0, lfh0,  bmll1, x2,    bmlh1,
    lfl0,  bmhl1, x3,    bmhh1, lfl0,  bmll2, x4,    bmlh2, lfe,   bmhl2, x5,
    bmhh2, lfe,   bmll3, x6,    bmlh3, sfl,   bmhl3, x7,    bmhh3, sfl,   bmll4,
    x8,    bmlh4, lfh1,  bmhl4, x9,    bmhh4, lfh1,  bmll5, x10,   bmlh5, lfl1,
    bmhl5, x11,   bmhh5, lfl1,  bmll6, 0,     bmlh6, sfh,   bmhl6, 0,     bmhh6,
    sfh,   bmll7, 0,     bmlh7, 0,     bmhl7, 0,     bmhh7, 0};
TABLEBASEDDECODER(mMvBMXSrc)
static const unsigned mEGaDecoderTable[] = {
    egl0,  egh0,  egl1, egh1, egl2, egh2, egl3, egh3, egl4, egh4,  egl5,
    egh5,  egl6,  egh6, egl7, egh7, egl8, egh8, egl9, egh9, egl10, egh10,
    egl11, egh11, 0,    0,    0,    0,    0,    0,    0,    0};
TABLEBASEDDECODER(mEGa)
static const unsigned mFFaDecoderTable[] = {f0, f1, f2,  f3,  f4, f5, f6, f7,
                                            f8, f9, f10, f11, 0,  0,  0,  0};
TABLEBASEDDECODER(mFFa)
static const unsigned mXsDecoderTable[] = {x0, x1, x2,  x3,  x4, x5, x6, x7,
                                           x8, x9, x10, x11, 0,  0,  0,  0};
TABLEBASEDDECODER(mXs)
static const unsigned mEXsDecoderTable[] = {
    ex0, ex1, ex2, ex3, ex4, ex5, ex6, ex7, ex8, ex9, ex10, ex11, 0, 0, 0, 0};
TABLEBASEDDECODER(mEXs)
static const unsigned mCMaDecoderTable[] = {cml0, cmh0, cml1, cmh1, cml2, cmh2,
                                            cml3, cmh3, cml4, cmh4, cml5, cmh5,
                                            cml6, cmh6, cml7, cmh7};
TABLEBASEDDECODER(mCMa)
static const unsigned mEWmDecoderTable[] = {
    ewl0,  ewh0,  ewl1, ewh1, ewl2, ewh2, ewl3, ewh3, ewl4, ewh4,  ewl5,
    ewh5,  ewl6,  ewh6, ewl7, ewh7, ewl8, ewh8, ewl9, ewh9, ewl10, ewh10,
    ewl11, ewh11, 0,    0,    0,    0,    0,    0,    0,    0};
TABLEBASEDDECODER(mEWm)
static const unsigned mFEG2aDecoderTable[] = {
    feg0, feg1, feg2,  feg3,  feg4, feg5, feg6, feg7,
    feg8, feg9, feg10, feg11, 0,    0,    0,    0};
TABLEBASEDDECODER(mFEG2a)
static const unsigned mElmDecoderTable[] = {
    el0, el1, el2, el3, el4, el5, el6, el7, el8, el9, el10, el11, 0, 0, 0, 0};
TABLEBASEDDECODER(mElm)
static const unsigned mRF2aDecoderTable[] = {r24, r25};
static const unsigned mFEWmDecoderTable[] = {
    fewl0, fewh0, fewl1, fewh1, fewl2,  fewh2,  fewl3,  fewh3,
    fewl4, fewh4, fewl5, fewh5, fewl6,  fewh6,  fewl7,  fewh7,
    fewl8, fewh8, fewl9, fewh9, fewl10, fewh10, fewl11, fewh11,
    0,     0,     0,     0,     0,      0,      0,      0};
TABLEBASEDDECODER(mFEWm)
static const unsigned mEWhvDecoderTable[] = {
    ewh0, ewh1, ewh2,  ewh3,  ewh4, ewh5, ewh6, ewh7,
    ewh8, ewh9, ewh10, ewh11, 0,    0,    0,    0};
static const unsigned mFEXnDecoderTable[] = {
    fex0, fex1, fex2,  fex3,  fex4, fex5, fex6, fex7,
    fex8, fex9, fex10, fex11, 0,    0,    0,    0};
TABLEBASEDDECODER(mFEXn)
static const unsigned mMvSclSrcDecoderTable[] = {
    m0,  r0,  packSign0,   ls, dn0, r1,  p0,      0,
    dj0, r2,  srsSign0,    0,  dc0, r3,  s0,      0,
    m1,  r4,  upsSign0,    0,  dn1, r5,  p1,      0,
    dj1, r6,  unpackSign0, 0,  dc1, r7,  CORE_ID, 0,
    m2,  r8,  vaddSign0,   sp, dn2, r9,  p2,      0,
    dj2, r10, 0,           0,  dc2, r11, s1,      0,
    m3,  r12, 0,           0,  dn3, r13, p3,      0,
    dj3, r14, 0,           0,  dc3, r15, lc,      0,
    m4,  r16, packSign1,   0,  dn4, r17, p4,      0,
    dj4, r18, srsSign1,    0,  dc4, r19, s2,      0,
    m5,  r20, upsSign1,    0,  dn5, r21, p5,      0,
    dj5, r22, unpackSign1, 0,  dc5, r23, le,      0,
    m6,  r24, vaddSign1,   0,  dn6, r25, p6,      0,
    dj6, r26, 0,           0,  dc6, r27, s3,      0,
    m7,  r28, 0,           0,  dn7, r29, p7,      0,
    dj7, r30, 0,           0,  dc7, r31, lr,      0};
TABLEBASEDDECODER(mMvSclSrc)
static const unsigned mSclStDecoderTable[] = {
    m0,  el0, r0,   p0,   dn0, eh0, r1,  lr,   dj0, el1, r2,  p0,   dc0,
    eh1, r3,  lr,   m1,   el2, r4,  p1,  dn1,  eh2, r5,  lr,  dj1,  el3,
    r6,  p1,  dc1,  eh3,  r7,  lr,  m2,  el4,  r8,  p2,  dn2, eh4,  r9,
    lr,  dj2, el5,  r10,  p2,  dc2, eh5, r11,  lr,  m3,  el6, r12,  p3,
    dn3, eh6, r13,  lr,   dj3, el7, r14, p3,   dc3, eh7, r15, lr,   m4,
    el8, r16, p4,   dn4,  eh8, r17, lr,  dj4,  el9, r18, p4,  dc4,  eh9,
    r19, lr,  m5,   el10, r20, p5,  dn5, eh10, r21, lr,  dj5, el11, r22,
    p5,  dc5, eh11, r23,  lr,  m6,  0,   r24,  p6,  dn6, 0,   r25,  lr,
    dj6, 0,   r26,  p6,   dc6, 0,   r27, lr,   m7,  0,   r28, p7,   dn7,
    0,   r29, lr,   dj7,  0,   r30, p7,  dc7,  0,   r31, lr};
TABLEBASEDDECODER(mSclSt)
static const unsigned mFmDecoderTable[] = {
    fl0,  fh0,  fl1, fh1, fl2, fh2, fl3, fh3, fl4, fh4,  fl5,
    fh5,  fl6,  fh6, fl7, fh7, fl8, fh8, fl9, fh9, fl10, fh10,
    fl11, fh11, 0,   0,   0,   0,   0,   0,   0,   0};
TABLEBASEDDECODER(mFm)
static const unsigned mRyDecoderTable[] = {
    r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,  r8,  r9,  r10,
    r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21,
    r22, r23, r24, r25, r26, r27, r28, r29, r30, r31};
TABLEBASEDDECODER(mRy)
static const unsigned mEG2sDecoderTable[] = {
    eg0, eg1, eg2, eg3, eg4, eg5, eg6, eg7, eg8, eg9, eg10, eg11, 0, 0, 0, 0};
TABLEBASEDDECODER(mEG2s)
static const unsigned mEXaDecoderTable[] = {
    ex0, ex1, ex2, ex3, ex4, ex5, ex6, ex7, ex8, ex9, ex10, ex11, 0, 0, 0, 0};
TABLEBASEDDECODER(mEXa)
static const unsigned mRzDecoderTable[] = {
    r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,  r8,  r9,  r10,
    r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21,
    r22, r23, r24, r25, r26, r27, r28, r29, r30, r31};
TABLEBASEDDECODER(mRz)
static const unsigned mFFsDecoderTable[] = {f0, f1, f2,  f3,  f4, f5, f6, f7,
                                            f8, f9, f10, f11, 0,  0,  0,  0};
TABLEBASEDDECODER(mFFs)
static const unsigned mFEWaDecoderTable[] = {
    fewl0, fewh0, fewl1, fewh1, fewl2,  fewh2,  fewl3,  fewh3,
    fewl4, fewh4, fewl5, fewh5, fewl6,  fewh6,  fewl7,  fewh7,
    fewl8, fewh8, fewl9, fewh9, fewl10, fewh10, fewl11, fewh11,
    0,     0,     0,     0,     0,      0,      0,      0};
TABLEBASEDDECODER(mFEWa)
static const unsigned mFEGaDecoderTable[] = {
    fegl0, fegh0, fegl1, fegh1, fegl2,  fegh2,  fegl3,  fegh3,
    fegl4, fegh4, fegl5, fegh5, fegl6,  fegh6,  fegl7,  fegh7,
    fegl8, fegh8, fegl9, fegh9, fegl10, fegh10, fegl11, fegh11,
    0,     0,     0,     0,     0,      0,      0,      0};
TABLEBASEDDECODER(mFEGa)
static const unsigned mEXbDecoderTable[] = {
    ex0, ex1, ex2, ex3, ex4, ex5, ex6, ex7, ex8, ex9, ex10, ex11, 0, 0, 0, 0};
static const unsigned mEEaDecoderTable[] = {e0, e1, e2,  e3,  e4, e5, e6, e7,
                                            e8, e9, e10, e11, 0,  0,  0,  0};
TABLEBASEDDECODER(mEEa)
static const unsigned mWaDecoderTable[] = {
    wl0,  wh0,  wl1, wh1, wl2, wh2, wl3, wh3, wl4, wh4,  wl5,
    wh5,  wl6,  wh6, wl7, wh7, wl8, wh8, wl9, wh9, wl10, wh10,
    wl11, wh11, 0,   0,   0,   0,   0,   0,   0,   0};
TABLEBASEDDECODER(mWa)
static const unsigned mXmDecoderTable[] = {x0, x1, x2,  x3,  x4, x5, x6, x7,
                                           x8, x9, x10, x11, 0,  0,  0,  0};
TABLEBASEDDECODER(mXm)
static const unsigned mBM_ldDecoderTable[] = {
    bmll0, bmlh0, bmhl0, bmhh0, bmll1, bmlh1, bmhl1, bmhh1, bmll2, bmlh2, bmhl2,
    bmhh2, bmll3, bmlh3, bmhl3, bmhh3, bmll4, bmlh4, bmhl4, bmhh4, bmll5, bmlh5,
    bmhl5, bmhh5, bmll6, bmlh6, bmhl6, bmhh6, bmll7, bmlh7, bmhl7, bmhh7};
TABLEBASEDDECODER(mBM_ld)
static const unsigned mFFmDecoderTable[] = {f0, f1, f2,  f3,  f4, f5, f6, f7,
                                            f8, f9, f10, f11, 0,  0,  0,  0};
TABLEBASEDDECODER(mFFm)
static const unsigned mRaDecoderTable[] = {
    r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,  r8,  r9,  r10,
    r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21,
    r22, r23, r24, r25, r26, r27, r28, r29, r30, r31};
TABLEBASEDDECODER(mRa)
static const unsigned mRxDecoderTable[] = {
    r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,  r8,  r9,  r10,
    r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21,
    r22, r23, r24, r25, r26, r27, r28, r29, r30, r31};
TABLEBASEDDECODER(mRx)
static const unsigned mFEXmDecoderTable[] = {
    fex0, fex1, fex2,  fex3,  fex4, fex5, fex6, fex7,
    fex8, fex9, fex10, fex11, 0,    0,    0,    0};
TABLEBASEDDECODER(mFEXm)
static const unsigned mFEXbDecoderTable[] = {
    fex0, fex1, fex2,  fex3,  fex4, fex5, fex6, fex7,
    fex8, fex9, fex10, fex11, 0,    0,    0,    0};
static const unsigned mLdaSclDecoderTable[] = {
    m0,  el0, r0,   p0,   dn0, eh0, r1,  lr,   dj0, el1, r2,  p0,   dc0,
    eh1, r3,  lr,   m1,   el2, r4,  p1,  dn1,  eh2, r5,  lr,  dj1,  el3,
    r6,  p1,  dc1,  eh3,  r7,  lr,  m2,  el4,  r8,  p2,  dn2, eh4,  r9,
    lr,  dj2, el5,  r10,  p2,  dc2, eh5, r11,  lr,  m3,  el6, r12,  p3,
    dn3, eh6, r13,  lr,   dj3, el7, r14, p3,   dc3, eh7, r15, lr,   m4,
    el8, r16, p4,   dn4,  eh8, r17, lr,  dj4,  el9, r18, p4,  dc4,  eh9,
    r19, lr,  m5,   el10, r20, p5,  dn5, eh10, r21, lr,  dj5, el11, r22,
    p5,  dc5, eh11, r23,  lr,  m6,  0,   r24,  p6,  dn6, 0,   r25,  lr,
    dj6, 0,   r26,  p6,   dc6, 0,   r27, lr,   m7,  0,   r28, p7,   dn7,
    0,   r29, lr,   dj7,  0,   r30, p7,  dc7,  0,   r31, lr};
TABLEBASEDDECODER(mLdaScl)
static const unsigned mWsDecoderTable[] = {
    wl0,  wh0,  wl1, wh1, wl2, wh2, wl3, wh3, wl4, wh4,  wl5,
    wh5,  wl6,  wh6, wl7, wh7, wl8, wh8, wl9, wh9, wl10, wh10,
    wl11, wh11, 0,   0,   0,   0,   0,   0,   0,   0};
TABLEBASEDDECODER(mWs)
static const unsigned mAguDstDecoderTable[] = {
    dc0, dj0, dn0, m0, p0, 0, 0, 0, dc1, dj1, dn1, m1, p1, 0, 0, 0,
    dc2, dj2, dn2, m2, p2, 0, 0, 0, dc3, dj3, dn3, m3, p3, 0, 0, 0,
    dc4, dj4, dn4, m4, p4, 0, 0, 0, dc5, dj5, dn5, m5, p5, 0, 0, 0,
    dc6, dj6, dn6, m6, p6, 0, 0, 0, dc7, dj7, dn7, m7, p7, 0, 0, 0};
TABLEBASEDDECODER(mAguDst)
static const unsigned mEXmDecoderTable[] = {
    ex0, ex1, ex2, ex3, ex4, ex5, ex6, ex7, ex8, ex9, ex10, ex11, 0, 0, 0, 0};
TABLEBASEDDECODER(mEXm)
static const unsigned mGGmDecoderTable[] = {g0, g1, g2,  g3,  g4, g5, g6, g7,
                                            g8, g9, g10, g11, 0,  0,  0,  0};
TABLEBASEDDECODER(mGGm)
static const unsigned mFEXsDecoderTable[] = {
    fex0, fex1, fex2,  fex3,  fex4, fex5, fex6, fex7,
    fex8, fex9, fex10, fex11, 0,    0,    0,    0};
TABLEBASEDDECODER(mFEXs)
static const unsigned mBMmDecoderTable[] = {
    bmll0, bmlh0, bmhl0, bmhh0, bmll1, bmlh1, bmhl1, bmhh1, bmll2, bmlh2, bmhl2,
    bmhh2, bmll3, bmlh3, bmhl3, bmhh3, bmll4, bmlh4, bmhl4, bmhh4, bmll5, bmlh5,
    bmhl5, bmhh5, bmll6, bmlh6, bmhl6, bmhh6, bmll7, bmlh7, bmhl7, bmhh7};
TABLEBASEDDECODER(mBMm)
static const unsigned mGlmDecoderTable[] = {
    gl0, gl1, gl2, gl3, gl4, gl5, gl6, gl7, gl8, gl9, gl10, gl11, 0, 0, 0, 0};
TABLEBASEDDECODER(mGlm)
static const unsigned mMvBMXDstDecoderTable[] = {
    bmll0, x0,    bmlh0, lfh0,  bmhl0, x1,    bmhh0, lfh0,  bmll1, x2,    bmlh1,
    lfl0,  bmhl1, x3,    bmhh1, lfl0,  bmll2, x4,    bmlh2, lfe,   bmhl2, x5,
    bmhh2, lfe,   bmll3, x6,    bmlh3, sfl,   bmhl3, x7,    bmhh3, sfl,   bmll4,
    x8,    bmlh4, lfh1,  bmhl4, x9,    bmhh4, lfh1,  bmll5, x10,   bmlh5, lfl1,
    bmhl5, x11,   bmhh5, lfl1,  bmll6, 0,     bmlh6, sfh,   bmhl6, 0,     bmhh6,
    sfh,   bmll7, 0,     bmlh7, 0,     bmhl7, 0,     bmhh7, 0};
TABLEBASEDDECODER(mMvBMXDst)
static const unsigned mShflXDstDecoderTable[] = {
    x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, 0, 0, 0, 0};
TABLEBASEDDECODER(mShflXDst)
static const unsigned mEXnDecoderTable[] = {
    ex0, ex1, ex2, ex3, ex4, ex5, ex6, ex7, ex8, ex9, ex10, ex11, 0, 0, 0, 0};
TABLEBASEDDECODER(mEXn)
static const unsigned mRy8DecoderTable[] = {r0, r1, r2, r3, r4, r5, r6, r7};
TABLEBASEDDECODER(mRy8)
static const unsigned mXnDecoderTable[] = {x0, x1, x2,  x3,  x4, x5, x6, x7,
                                           x8, x9, x10, x11, 0,  0,  0,  0};
TABLEBASEDDECODER(mXn)
static const unsigned mMvSclDstCgDecoderTable[] = {
    m0,  r0,  packSign0,   0, dn0, r1,  p0,          0, dj0, r2,  srsSign0,  0,
    dc0, r3,  s0,          0, m1,  r4,  upsSign0,    0, dn1, r5,  p1,        0,
    dj1, r6,  unpackSign0, 0, dc1, r7,  lc,          0, m2,  r8,  vaddSign0, 0,
    dn2, r9,  p2,          0, dj2, r10, 0,           0, dc2, r11, s1,        0,
    m3,  r12, 0,           0, dn3, r13, p3,          0, dj3, r14, 0,         0,
    dc3, r15, lr,          0, m4,  r16, packSign1,   0, dn4, r17, p4,        0,
    dj4, r18, srsSign1,    0, dc4, r19, s2,          0, m5,  r20, upsSign1,  0,
    dn5, r21, p5,          0, dj5, r22, unpackSign1, 0, dc5, r23, sp,        0,
    m6,  r24, vaddSign1,   0, dn6, r25, p6,          0, dj6, r26, 0,         0,
    dc6, r27, s3,          0, m7,  r28, 0,           0, dn7, r29, p7,        0,
    dj7, r30, 0,           0, dc7, r31, 0,           0};
TABLEBASEDDECODER(mMvSclDstCg)
static const unsigned mXvDecoderTable[] = {x0, x1, x2,  x3,  x4, x5, x6, x7,
                                           x8, x9, x10, x11, 0,  0,  0,  0};
TABLEBASEDDECODER(mXv)
static const unsigned mSclMSDecoderTable[] = {
    r0,  0, m0, 0, r1,  0, dn0, 0, r2,  0, dj0, 0, r3,  p0, dc0, 0,
    r4,  0, m1, 0, r5,  0, dn1, 0, r6,  0, dj1, 0, r7,  p1, dc1, 0,
    r8,  0, m2, 0, r9,  0, dn2, 0, r10, 0, dj2, 0, r11, p2, dc2, 0,
    r12, 0, m3, 0, r13, 0, dn3, 0, r14, 0, dj3, 0, r15, p3, dc3, 0,
    r16, 0, m4, 0, r17, 0, dn4, 0, r18, 0, dj4, 0, r19, p4, dc4, 0,
    r20, 0, m5, 0, r21, 0, dn5, 0, r22, 0, dj5, 0, r23, p5, dc5, 0,
    r24, 0, m6, 0, r25, 0, dn6, 0, r26, 0, dj6, 0, r27, p6, dc6, 0,
    r28, 0, m7, 0, r29, 0, dn7, 0, r30, 0, dj7, 0, r31, p7, dc7, 0};
TABLEBASEDDECODER(mSclMS)
static const unsigned mEWlv_bisDecoderTable[] = {
    ewl0, ewl1, ewl2,  ewl3,  ewl4, ewl5, ewl6, ewl7,
    ewl8, ewl9, ewl10, ewl11, 0,    0,    0,    0};
static const unsigned mAluCgDecoderTable[] = {
    r0,  lc, r1,  0, r2,  lc, r3,  0, r4,  lc, r5,  0, r6,  lc, r7,  0,
    r8,  lc, r9,  0, r10, lc, r11, 0, r12, lc, r13, 0, r14, lc, r15, 0,
    r16, lc, r17, 0, r18, lc, r19, 0, r20, lc, r21, 0, r22, lc, r23, 0,
    r24, lc, r25, 0, r26, lc, r27, 0, r28, lc, r29, 0, r30, lc, r31, 0};
TABLEBASEDDECODER(mAluCg)
static const unsigned mEXwDecoderTable[] = {
    ex0, ex1, ex2, ex3, ex4, ex5, ex6, ex7, ex8, ex9, ex10, ex11, 0, 0, 0, 0};
TABLEBASEDDECODER(mEXw)
static const unsigned mCMsDecoderTable[] = {cml0, cmh0, cml1, cmh1, cml2, cmh2,
                                            cml3, cmh3, cml4, cmh4, cml5, cmh5,
                                            cml6, cmh6, cml7, cmh7};
TABLEBASEDDECODER(mCMs)
static const unsigned mWs_128bDecoderTable[] = {
    wl0,  wh0,  wl1, wh1, wl2, wh2, wl3, wh3, wl4, wh4,  wl5,
    wh5,  wl6,  wh6, wl7, wh7, wl8, wh8, wl9, wh9, wl10, wh10,
    wl11, wh11, 0,   0,   0,   0,   0,   0,   0,   0};
TABLEBASEDDECODER(mWs_128b)
static const unsigned mFEGsDecoderTable[] = {
    fegl0, fegh0, fegl1, fegh1, fegl2,  fegh2,  fegl3,  fegh3,
    fegl4, fegh4, fegl5, fegh5, fegl6,  fegh6,  fegl7,  fegh7,
    fegl8, fegh8, fegl9, fegh9, fegl10, fegh10, fegl11, fegh11,
    0,     0,     0,     0,     0,      0,      0,      0};
TABLEBASEDDECODER(mFEGs)
static const unsigned mRS16mDecoderTable[] = {r16, r17, r18, r19, r20, r21,
                                              r22, r23, r24, r25, r26, r27,
                                              r28, r29, r30, r31};
TABLEBASEDDECODER(mRS16m)
static const unsigned mGGaDecoderTable[] = {g0, g1, g2,  g3,  g4, g5, g6, g7,
                                            g8, g9, g10, g11, 0,  0,  0,  0};
TABLEBASEDDECODER(mGGa)
static const unsigned mGaDecoderTable[] = {
    gl0,  gh0,  gl1, gh1, gl2, gh2, gl3, gh3, gl4, gh4,  gl5,
    gh5,  gl6,  gh6, gl7, gh7, gl8, gh8, gl9, gh9, gl10, gh10,
    gl11, gh11, 0,   0,   0,   0,   0,   0,   0,   0};
TABLEBASEDDECODER(mGa)
static const unsigned mXaDecoderTable[] = {x0, x1, x2,  x3,  x4, x5, x6, x7,
                                           x8, x9, x10, x11, 0,  0,  0,  0};
TABLEBASEDDECODER(mXa)
static const unsigned mRF2bDecoderTable[] = {r24, r25};
static const unsigned mWmDecoderTable[] = {
    wl0,  wh0,  wl1, wh1, wl2, wh2, wl3, wh3, wl4, wh4,  wl5,
    wh5,  wl6,  wh6, wl7, wh7, wl8, wh8, wl9, wh9, wl10, wh10,
    wl11, wh11, 0,   0,   0,   0,   0,   0,   0,   0};
TABLEBASEDDECODER(mWm)
static const unsigned mFEWsDecoderTable[] = {
    fewl0, fewh0, fewl1, fewh1, fewl2,  fewh2,  fewl3,  fewh3,
    fewl4, fewh4, fewl5, fewh5, fewl6,  fewh6,  fewl7,  fewh7,
    fewl8, fewh8, fewl9, fewh9, fewl10, fewh10, fewl11, fewh11,
    0,     0,     0,     0,     0,      0,      0,      0};
TABLEBASEDDECODER(mFEWs)
static const unsigned mXs_splitDecoderTable[] = {
    x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, 0, 0, 0, 0};
TABLEBASEDDECODER(mXs_split)
static const unsigned mlStsDecoderTable[] = {
    egl0,  fl0,  e0,  l0,  egh0,  fh0,  g0,  0,   egl1,  fl1,  e1,  l1,
    egh1,  fh1,  g1,  0,   egl2,  fl2,  e2,  l2,  egh2,  fh2,  g2,  0,
    egl3,  fl3,  e3,  l3,  egh3,  fh3,  g3,  0,   egl4,  fl4,  e4,  l4,
    egh4,  fh4,  g4,  0,   egl5,  fl5,  e5,  l5,  egh5,  fh5,  g5,  0,
    egl6,  fl6,  e6,  l6,  egh6,  fh6,  g6,  0,   egl7,  fl7,  e7,  l7,
    egh7,  fh7,  g7,  0,   egl8,  fl8,  e8,  l8,  egh8,  fh8,  g8,  0,
    egl9,  fl9,  e9,  l9,  egh9,  fh9,  g9,  0,   egl10, fl10, e10, l10,
    egh10, fh10, g10, 0,   egl11, fl11, e11, l11, egh11, fh11, g11, 0,
    0,     0,    0,   l12, 0,     0,    0,   0,   0,     0,    0,   l13,
    0,     0,    0,   0,   0,     0,    0,   l14, 0,     0,    0,   0,
    0,     0,    0,   l15, 0,     0,    0,   0};
TABLEBASEDDECODER(mlSts)
static const unsigned mSCmDecoderTable[] = {crBF8conf,
                                            srCarry,
                                            crF2BMask,
                                            srCarry,
                                            crF2FMask,
                                            srF2BFlags,
                                            crF2IMask,
                                            srF2BFlags,
                                            crFP8conf,
                                            srF2FFlags,
                                            crFPCnvFl2FxMask,
                                            srF2FFlags,
                                            crFPCnvFx2FlMask,
                                            srF2IFlags,
                                            crFPConvSat,
                                            srF2IFlags,
                                            crFPMask,
                                            srFPCnvFl2Fx,
                                            crFPNlfMask,
                                            srFPCnvFl2Fx,
                                            crMCDEn,
                                            srFPCnvFx2Fl,
                                            crPackSize,
                                            srFPCnvFx2Fl,
                                            crRnd,
                                            srFPFlags,
                                            crSCDEn,
                                            srFPFlags,
                                            crSRSMode,
                                            srFPNlf,
                                            crSat,
                                            srFPNlf,
                                            crUPSMode,
                                            srFifo_of,
                                            crUnpackSize,
                                            srFifo_of,
                                            0,
                                            srFifo_uf,
                                            0,
                                            srFifo_uf,
                                            0,
                                            srMS0,
                                            0,
                                            srMS0,
                                            0,
                                            srSRS_of,
                                            0,
                                            srSRS_of,
                                            0,
                                            srSS0,
                                            0,
                                            srSS0,
                                            0,
                                            srSparse_of,
                                            0,
                                            srSparse_of,
                                            0,
                                            srUPS_of,
                                            0,
                                            srUPS_of,
                                            0,
                                            0,
                                            0,
                                            0};
TABLEBASEDDECODER(mSCm)
static const unsigned mBMs_v16w32DecoderTable[] = {
    bmll0, bmlh0, bmhl0, bmhh0, bmll1, bmlh1, bmhl1, bmhh1, bmll2, bmlh2, bmhl2,
    bmhh2, bmll3, bmlh3, bmhl3, bmhh3, bmll4, bmlh4, bmhl4, bmhh4, bmll5, bmlh5,
    bmhl5, bmhh5, bmll6, bmlh6, bmhl6, bmhh6, bmll7, bmlh7, bmhl7, bmhh7};
TABLEBASEDDECODER(mBMs_v16w32)
static const unsigned mGsDecoderTable[] = {
    gl0,  gh0,  gl1, gh1, gl2, gh2, gl3, gh3, gl4, gh4,  gl5,
    gh5,  gl6,  gh6, gl7, gh7, gl8, gh8, gl9, gh9, gl10, gh10,
    gl11, gh11, 0,   0,   0,   0,   0,   0,   0,   0};
TABLEBASEDDECODER(mGs)
static const unsigned mLdaCgDecoderTable[] = {
    r0,  0, m0, 0, r1,  0,  dn0, 0, r2,  0, dj0, 0, r3,  p0, dc0, 0,
    r4,  0, m1, 0, r5,  lc, dn1, 0, r6,  0, dj1, 0, r7,  p1, dc1, 0,
    r8,  0, m2, 0, r9,  0,  dn2, 0, r10, 0, dj2, 0, r11, p2, dc2, 0,
    r12, 0, m3, 0, r13, lc, dn3, 0, r14, 0, dj3, 0, r15, p3, dc3, 0,
    r16, 0, m4, 0, r17, 0,  dn4, 0, r18, 0, dj4, 0, r19, p4, dc4, 0,
    r20, 0, m5, 0, r21, lc, dn5, 0, r22, 0, dj5, 0, r23, p5, dc5, 0,
    r24, 0, m6, 0, r25, 0,  dn6, 0, r26, 0, dj6, 0, r27, p6, dc6, 0,
    r28, 0, m7, 0, r29, lc, dn7, 0, r30, 0, dj7, 0, r31, p7, dc7, 0};
TABLEBASEDDECODER(mLdaCg)
static const unsigned mEEmDecoderTable[] = {e0, e1, e2,  e3,  e4, e5, e6, e7,
                                            e8, e9, e10, e11, 0,  0,  0,  0};
TABLEBASEDDECODER(mEEm)
static const unsigned mFl2FxSrc_WDecoderTable[] = {
    wl0,  wh0,  wl1, wh1, wl2, wh2, wl3, wh3, wl4, wh4,  wl5,
    wh5,  wl6,  wh6, wl7, wh7, wl8, wh8, wl9, wh9, wl10, wh10,
    wl11, wh11, 0,   0,   0,   0,   0,   0,   0,   0};
TABLEBASEDDECODER(mFl2FxSrc_W)
static const unsigned mXbDecoderTable[] = {x0, x1, x2,  x3,  x4, x5, x6, x7,
                                           x8, x9, x10, x11, 0,  0,  0,  0};
TABLEBASEDDECODER(mXb)
static const unsigned mEG2mDecoderTable[] = {
    eg0, eg1, eg2, eg3, eg4, eg5, eg6, eg7, eg8, eg9, eg10, eg11, 0, 0, 0, 0};
TABLEBASEDDECODER(mEG2m)
static const unsigned mBMsDecoderTable[] = {
    bmll0, bmlh0, bmhl0, bmhh0, bmll1, bmlh1, bmhl1, bmhh1, bmll2, bmlh2, bmhl2,
    bmhh2, bmll3, bmlh3, bmhl3, bmhh3, bmll4, bmlh4, bmhl4, bmhh4, bmll5, bmlh5,
    bmhl5, bmhh5, bmll6, bmlh6, bmhl6, bmhh6, bmll7, bmlh7, bmhl7, bmhh7};
TABLEBASEDDECODER(mBMs)
static const unsigned mRmDecoderTable[] = {
    r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,  r8,  r9,  r10,
    r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21,
    r22, r23, r24, r25, r26, r27, r28, r29, r30, r31};
TABLEBASEDDECODER(mRm)

// Decoder Tables for Singleton Register Classes.
FIXEDREGDECODER(mL8m)
FIXEDREGDECODER(mS3)
FIXEDREGDECODER(mR30_shiftx)
FIXEDREGDECODER(mR23_lock)
FIXEDREGDECODER(mPfs)
FIXEDREGDECODER(mR31_scd)
FIXEDREGDECODER(mR31_divs)
FIXEDREGDECODER(mR27_select)
FIXEDREGDECODER(mR16m)
FIXEDREGDECODER(mS2)
FIXEDREGDECODER(mStFifo)
FIXEDREGDECODER(mR26_fifo_st)
FIXEDREGDECODER(mR28_tlast)
FIXEDREGDECODER(mLCxCg)
FIXEDREGDECODER(mR29_insert)
