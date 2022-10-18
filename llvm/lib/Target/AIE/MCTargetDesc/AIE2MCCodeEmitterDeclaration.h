//===- AIE2MCCodeEmitterDeclaration.h ---------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

void getmAluCgOpValue(const MCInst &MI, unsigned OpNo, APInt &Op,
                      SmallVectorImpl<MCFixup> &Fixups,
                      const MCSubtargetInfo &STI) const;

void getmMvSclDstOpValue(const MCInst &MI, unsigned OpNo, APInt &Op,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const;

void getmMvSclDstCgOpValue(const MCInst &MI, unsigned OpNo, APInt &Op,
                           SmallVectorImpl<MCFixup> &Fixups,
                           const MCSubtargetInfo &STI) const;

void getmMvSclSrcOpValue(const MCInst &MI, unsigned OpNo, APInt &Op,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const;

void getmLdaSclOpValue(const MCInst &MI, unsigned OpNo, APInt &Op,
                       SmallVectorImpl<MCFixup> &Fixups,
                       const MCSubtargetInfo &STI) const;

void getmSclMSOpValue(const MCInst &MI, unsigned OpNo, APInt &Op,
                      SmallVectorImpl<MCFixup> &Fixups,
                      const MCSubtargetInfo &STI) const;

void getmSclStOpValue(const MCInst &MI, unsigned OpNo, APInt &Op,
                      SmallVectorImpl<MCFixup> &Fixups,
                      const MCSubtargetInfo &STI) const;

void getmLdaCgOpValue(const MCInst &MI, unsigned OpNo, APInt &Op,
                      SmallVectorImpl<MCFixup> &Fixups,
                      const MCSubtargetInfo &STI) const;

void getmMvAMWQDstOpValue(const MCInst &MI, unsigned OpNo, APInt &Op,
                          SmallVectorImpl<MCFixup> &Fixups,
                          const MCSubtargetInfo &STI) const;

void getmMvAMWQSrcOpValue(const MCInst &MI, unsigned OpNo, APInt &Op,
                          SmallVectorImpl<MCFixup> &Fixups,
                          const MCSubtargetInfo &STI) const;

void getmMvBMXSrcOpValue(const MCInst &MI, unsigned OpNo, APInt &Op,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const;

void getmMvBMXDstOpValue(const MCInst &MI, unsigned OpNo, APInt &Op,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const;

void geteRS4OpValue(const MCInst &MI, unsigned OpNo, APInt &Op,
                    SmallVectorImpl<MCFixup> &Fixups,
                    const MCSubtargetInfo &STI) const;

void getmShflDstOpValue(const MCInst &MI, unsigned OpNo, APInt &Op,
                        SmallVectorImpl<MCFixup> &Fixups,
                        const MCSubtargetInfo &STI) const;

void getmWm_1OpValue(const MCInst &MI, unsigned OpNo, APInt &Op,
                     SmallVectorImpl<MCFixup> &Fixups,
                     const MCSubtargetInfo &STI) const;

void getmQXHLbOpValue(const MCInst &MI, unsigned OpNo, APInt &Op,
                      SmallVectorImpl<MCFixup> &Fixups,
                      const MCSubtargetInfo &STI) const;
