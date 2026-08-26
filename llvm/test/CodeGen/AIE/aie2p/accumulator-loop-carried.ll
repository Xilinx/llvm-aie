; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -O2 -mtriple=aie2p %s -o - | FileCheck %s
;
; XFAIL: *
; FIXME: This test documents a live miscompile and is expected to fail until it
; is fixed. Drop the XFAIL together with the fix.
;
;===----------------------------------------------------------------------===;
; AIE2P: loop-carried accumulator update is dropped
;===----------------------------------------------------------------------===;
;
; SYMPTOM
; -------
; The IR below is correct. In %for.body4.i the two SROA-split halves of the
; accumulator (%y_acc.sroa.0.1, %y_acc.sroa.6.1) are well-formed loop-carried
; phis whose back-edge values are the halves of the MAC result %5 computed in
; %_Z3mvm...exit.i. The loop must therefore carry a running sum across its 6
; iterations.
;
; Generated code does not. The accumulator is copied IN at the loop head and
; accumulated into cml3, but cml1 -- the loop-carried register -- is never
; written inside the loop:
;
;         vmov    bmll1, x0          ; preheader: cml1 = 0
;         vmov    bmlh1, x0          ; ...the ONLY writes to cml1 anywhere
;   .LBB0_2:                         ; j-loop head
;         vmov    cml3, cml1         ; copy IN
;         acq     #51, r2
;         ...
;         vmac.f  dm3, dm3, x6, x8, r4   ; accumulate into cml3
;   // %bb.6:                        ; j-loop back edge
;         add      r19, r19, r17     ; no copy-out of cml3 into cml1
;         jz       r20, #.LBB0_2     ; next iteration re-reads cml1 == 0
;
; Every iteration restarts from the preheader value, so all partial sums except
; the last are discarded.
;
; Sub-registers are index-aligned (AIE2PRegisterInfo.td):
;     cml<i> = { bmll<i>, bmlh<i> }        dm<i> = { cml<i>, cmh<i> }
; so dm3 and cml1 are physically disjoint; the accumulation into dm3 provably
; cannot reach cml1. (Aliasing was checked explicitly -- it is not the answer.)
;
; ORIGINAL SOURCE
; ---------------
; Reduced from a bf16 blocked matrix-vector accumulate. A helper that adds into
; a caller-declared accumulator array is inlined into its caller; that inlining
; is what makes the accumulator alloca promotable.
;
;   template <int M, int K>
;   void mvm(bf16 *w, bf16 *x, float *y_acc) {
;     aie::vector<float, M> v = aie::load_v<M>(y_acc);
;     aie::accum<accfloat, M> acc;  acc.from_vector(v);
;     bf16 *w_ptr = w;
;     for (int i = 0; i < K / 32; i++) {
;       aie::vector<bf16, 32> b_col = aie::load_v<32>(x + i * 32);
;       for (int j = 0; j < 32; j++) {
;         aie::vector<bf16, M> w_col = aie::load_v<M>(w_ptr);
;         acc = aie::mac(acc, w_col, (bf16)b_col[j]);
;         w_ptr += M;
;       }
;     }
;     aie::store_v(y_acc, acc.template to_vector<float>());
;   }
;
;   template <int M, int K>
;   void proj(bf16 *y, bf16 *wp, bf16 *wq, bf16 *x, float *y_acc, bool *ping) {
;     for (int i = 0; i < M / 32; i++) {
;       aie::store_v(y_acc, aie::zeros<float, 32>());
;       for (int j = 0; j < K / 256; j++) {            // <-- the broken loop
;         (*ping) = !(*ping);
;         acquire_greater_equal(51, 1u);
;         mvm<32, 256>((*ping) ? wp : wq, x + j * 256, y_acc);
;         release(50, 1);
;       }
;       /* read y_acc out into y + i*32 */
;     }
;   }
;
;   extern "C" void kernel(bf16 *x, bf16 *wp, bf16 *wq, bf16 *y) {
;     static bool ping = false;
;     alignas(aie::vector_decl_align) float y_acc[32];   // THE accumulator
;     proj<256, 1536>(y, wp, wq, x, y_acc, &ping);
;   }
;
; Compile with:
;   clang++ --target=aie2p-none-unknown-elf -O2 -std=c++20 \
;     -DAIE_API_EMULATE_BFLOAT16_MMUL_WITH_BFP16 -D__AIE_API_AIE_ADF_HPP__ \
;     -I<mlir-aie>/include repro.cc -S -emit-llvm -o bad.ll
; Adding __attribute__((noinline)) to mvm() (or to proj()) keeps the
; accumulator in memory and produces correct code.
;
; WHY THIS IS PROVABLY WRONG (static, no execution required)
; ----------------------------------------------------------
; Inside the j-loop the only registers written by any vector or accumulator
; operation are:
;
;     cml3   <- vmov       (the copy-in at the loop head)
;     dm3    <- vmac.f     (the accumulation)
;     x0, x6 <- vldb       (loads of the input data from memory)
;
; There is no vst (no spill) and no vlda (no reload) in the loop, and x0/x6 are
; only ever defined by loads from memory -- they never receive the accumulator.
; So the emitted loop has no mechanism of any kind to carry a value from one
; iteration to the next except cml1, which is never written. The IR requires a
; running sum across 6 iterations; the generated code cannot express one.
;
; HARDWARE EVIDENCE FOR THIS REDUCED CASE
; ---------------------------------------
; The reduced kernel above was executed on Strix / XDNA2 (NPU2) via a small
; IRON harness, with all-ones inputs so that each of the 6 j-iterations
; contributes exactly K_BLOCK = 256 to every accumulator lane. Correct output
; is therefore 1536, and 256 means only the final iteration survived. Both are
; exactly representable in bfloat16, so the verdict carries no rounding
; ambiguity:
;
;     -O2, mvm() inlined     ->  y = 256    (accumulation dropped)
;     -O2, mvm() noinline    ->  y = 1536   (correct)
;
; The same kernel, same inputs, same optimisation level; only inlining differs.
;
; The original kernel this was reduced from behaves identically: -O2 silently
; corrupts results, and marking the helper noinline restores output that is
; bit-identical to -O1. Instruction counts there also show stale data rather
; than lost math -- vmac.f, hardware-loop and vextbcst counts are unchanged
; between good and bad builds, and only the accumulator reloads disappear
; (37 -> 34 vector loads).
;
; Note the lock intrinsics are omitted from the IR here: the miscompile
; reproduces with or without them, so they are not part of the trigger.
;
; TRIAGE (already ruled out -- please do not re-investigate)
; ---------------------------------------------------------
;   * Not the register coalescer. ACC-class COPY counts across
;     `llc -print-after-all` run 0 -> 3 (phi-node-elimination) -> 1
;     (register-coalescer) -> 0 (virtregrewriter), which looks like the
;     coalescer, but building the original kernel with
;     `-mllvm -join-liveintervals=false` still produces corrupted results on
;     hardware. The guilty pass is NOT yet identified.
;   * Not alias/lock ordering. The weight loads remain inside the
;     acq #51 / rel #50 region; lock intrinsics carry no memory(...) clause and
;     are correctly conservative.
;   * Not the IR pipeline. The phis above are well-formed; the defect appears
;     only after ISel/regalloc.
;   * Also excluded on hardware: the LUT/activation code that shares the
;     original translation unit, the software pipeliner
;     (-enable-pipeliner=false), every -O2-only IR pass removed individually,
;     loop-vectorize / simplifycfg hoist-sink / loop-unroll<O2> /
;     licm<allowspeculation> parameter deltas, DSE, and store-forwarding of the
;     scratch buffers.
;
; VERIFYING A FIX
; ---------------
; The accumulation must reach the loop-carried register. A fix may either emit
; a copy-out into it, or coalesce the two registers so the MAC accumulates
; directly into it. This test checks the copy-out form; if the fix coalesces
; instead, regenerate the assertions with utils/update_llc_test_checks.py.
;
; Reproduced with llvm-aie 22.0.0.2026082601+91977805.
;
; CHECK-LABEL: kernel:
; CHECK:       vmov{{[[:space:]]+}}{{cml[0-9]+}}, [[CARRY:cml[0-9]+]]
; CHECK:       vmov{{[[:space:]]+}}[[CARRY]],

target datalayout = "e-m:e-p:20:32-i1:8:32-i8:8:32-i16:16:32-i32:32:32-f32:32:32-i64:32-f64:32-a:0:32-n32"
target triple = "aie2p-none-unknown-elf"

@_ZZ6kernelE9is_w_ping = internal unnamed_addr global i8 0, align 4

; Function Attrs: mustprogress nofree norecurse nosync nounwind
define dso_local void @kernel(ptr readonly captures(none) %x, ptr readonly captures(none) %w_ping, ptr readonly captures(none) %w_pong, ptr writeonly captures(none) %y) local_unnamed_addr #0 {
entry:
  %_ZZ6kernelE9is_w_ping.promoted6 = load i8, ptr @_ZZ6kernelE9is_w_ping, align 4, !tbaa !2
  br label %for.body.i

for.body.i:                                       ; preds = %for.cond.cleanup3.i, %entry
  %_ZZ6kernelE9is_w_ping.promoted7 = phi i8 [ %_ZZ6kernelE9is_w_ping.promoted6, %entry ], [ %storedv.i, %for.cond.cleanup3.i ]
  %i.027.i = phi i32 [ 0, %entry ], [ %inc12.i, %for.cond.cleanup3.i ]
  br label %for.body4.i

for.cond.cleanup3.i:                              ; preds = %_Z3mvmILi32ELi256EEvP8bfloat16S1_Pf.exit.i
  %mul6.i = shl nuw nsw i32 %i.027.i, 5
  %idx.ext7.i = trunc nuw nsw i32 %mul6.i to i20
  %add.ptr8.i = getelementptr inbounds nuw bfloat, ptr %y, i20 %idx.ext7.i
  %0 = tail call noundef <32 x bfloat> @llvm.aie2p.v32accfloat.to.v32bf16(<32 x float> %5)
  store <32 x bfloat> %0, ptr %add.ptr8.i, align 64, !tbaa !6
  %inc12.i = add nuw nsw i32 %i.027.i, 1
  %exitcond28.not.i = icmp eq i32 %inc12.i, 8
  br i1 %exitcond28.not.i, label %_Z4projILi256ELi1536EEvP8bfloat16S1_S1_S1_PfPb.exit, label %for.body.i, !llvm.loop !7

for.body4.i:                                      ; preds = %_Z3mvmILi32ELi256EEvP8bfloat16S1_Pf.exit.i, %for.body.i
  %storedv.i5 = phi i8 [ %_ZZ6kernelE9is_w_ping.promoted7, %for.body.i ], [ %storedv.i, %_Z3mvmILi32ELi256EEvP8bfloat16S1_Pf.exit.i ]
  %y_acc.sroa.6.1 = phi <16 x i32> [ zeroinitializer, %for.body.i ], [ %shuffle1.i.i.i.i.i.i.i.i.i.i.i.i.i.i, %_Z3mvmILi32ELi256EEvP8bfloat16S1_Pf.exit.i ]
  %y_acc.sroa.0.1 = phi <16 x i32> [ zeroinitializer, %for.body.i ], [ %shuffle.i.i.i.i.i.i.i.i.i.i.i.i.i, %_Z3mvmILi32ELi256EEvP8bfloat16S1_Pf.exit.i ]
  %j.026.i = phi i32 [ 0, %for.body.i ], [ %inc.i, %_Z3mvmILi32ELi256EEvP8bfloat16S1_Pf.exit.i ]
  %loadedv.i = trunc nuw i8 %storedv.i5 to i1
  %lnot.i = xor i1 %loadedv.i, true
  %storedv.i = zext i1 %lnot.i to i8
  store i8 %storedv.i, ptr @_ZZ6kernelE9is_w_ping, align 4, !tbaa !2
  %cond.i = select i1 %loadedv.i, ptr %w_pong, ptr %w_ping
  %mul.i = shl nuw nsw i32 %j.026.i, 8
  %idx.ext.i = trunc nuw nsw i32 %mul.i to i20
  %add.ptr.i = getelementptr inbounds nuw bfloat, ptr %x, i20 %idx.ext.i
  tail call void @llvm.aie2p.acquire(i32 51, i32 -1)
  %shuffle.i.uncasted.i.i.i.i.i.i.i.i.i.uncasted = shufflevector <16 x i32> %y_acc.sroa.0.1, <16 x i32> %y_acc.sroa.6.1, <32 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 12, i32 13, i32 14, i32 15, i32 16, i32 17, i32 18, i32 19, i32 20, i32 21, i32 22, i32 23, i32 24, i32 25, i32 26, i32 27, i32 28, i32 29, i32 30, i32 31>
  %shuffle.i.uncasted.i.i.i.i.i.i.i.i.i = bitcast <32 x i32> %shuffle.i.uncasted.i.i.i.i.i.i.i.i.i.uncasted to <32 x float>
  br label %for.body.i.i

for.body.i.i:                                     ; preds = %for.cond.cleanup3.i.i, %for.body4.i
  %i.031.i.i = phi i32 [ 0, %for.body4.i ], [ %inc9.i.i, %for.cond.cleanup3.i.i ]
  %w_ptr.030.i.i = phi ptr [ %cond.i, %for.body4.i ], [ %scevgep.i.i, %for.cond.cleanup3.i.i ]
  %1 = phi <32 x float> [ %shuffle.i.uncasted.i.i.i.i.i.i.i.i.i, %for.body4.i ], [ %5, %for.cond.cleanup3.i.i ]
  %mul.i.i = shl nuw nsw i32 %i.031.i.i, 5
  %idx.ext.i.i = trunc nuw nsw i32 %mul.i.i to i20
  %add.ptr.i.i = getelementptr inbounds nuw bfloat, ptr %add.ptr.i, i20 %idx.ext.i.i
  %2 = load <32 x bfloat>, ptr %add.ptr.i.i, align 64, !tbaa !6, !noalias !9
  br label %_ZNK3aie15vector_elem_refI8bfloat16Lj32EEcvS1_Ev.exit.i.i

for.cond.cleanup3.i.i:                            ; preds = %_ZNK3aie15vector_elem_refI8bfloat16Lj32EEcvS1_Ev.exit.i.i
  %scevgep.i.i = getelementptr i8, ptr %w_ptr.030.i.i, i20 2048
  %inc9.i.i = add nuw nsw i32 %i.031.i.i, 1
  %exitcond33.not.i.i = icmp eq i32 %inc9.i.i, 8
  br i1 %exitcond33.not.i.i, label %_Z3mvmILi32ELi256EEvP8bfloat16S1_Pf.exit.i, label %for.body.i.i, !llvm.loop !16

_ZNK3aie15vector_elem_refI8bfloat16Lj32EEcvS1_Ev.exit.i.i: ; preds = %_ZNK3aie15vector_elem_refI8bfloat16Lj32EEcvS1_Ev.exit.i.i, %for.body.i.i
  %j.028.i.i = phi i32 [ 0, %for.body.i.i ], [ %inc.i.i, %_ZNK3aie15vector_elem_refI8bfloat16Lj32EEcvS1_Ev.exit.i.i ]
  %w_ptr.127.i.i = phi ptr [ %w_ptr.030.i.i, %for.body.i.i ], [ %add.ptr7.i.i, %_ZNK3aie15vector_elem_refI8bfloat16Lj32EEcvS1_Ev.exit.i.i ]
  %acc.sroa.0.126.i.i = phi <32 x float> [ %1, %for.body.i.i ], [ %5, %_ZNK3aie15vector_elem_refI8bfloat16Lj32EEcvS1_Ev.exit.i.i ]
  %3 = load <32 x bfloat>, ptr %w_ptr.127.i.i, align 64, !tbaa !6, !noalias !17
  %4 = extractelement <32 x bfloat> %2, i32 %j.028.i.i
  %splat.splatinsert.i.i.i.i.i.i.i.i.i.i.i.i.i = insertelement <32 x bfloat> poison, bfloat %4, i64 0
  %splat.splat.i.i.i.i.i.i.i.i.i.i.i.i.i = shufflevector <32 x bfloat> %splat.splatinsert.i.i.i.i.i.i.i.i.i.i.i.i.i, <32 x bfloat> poison, <32 x i32> zeroinitializer
  %5 = tail call noundef <32 x float> @llvm.aie2p.I512.I512.ACC1024.bf.mac.conf(<32 x bfloat> %3, <32 x bfloat> %splat.splat.i.i.i.i.i.i.i.i.i.i.i.i.i, <32 x float> %acc.sroa.0.126.i.i, i32 828)
  %add.ptr7.i.i = getelementptr inbounds nuw i8, ptr %w_ptr.127.i.i, i20 64
  %inc.i.i = add nuw nsw i32 %j.028.i.i, 1
  %exitcond.not.i.i = icmp eq i32 %inc.i.i, 32
  br i1 %exitcond.not.i.i, label %for.cond.cleanup3.i.i, label %_ZNK3aie15vector_elem_refI8bfloat16Lj32EEcvS1_Ev.exit.i.i, !llvm.loop !24

_Z3mvmILi32ELi256EEvP8bfloat16S1_Pf.exit.i:       ; preds = %for.cond.cleanup3.i.i
  %6 = bitcast <32 x float> %5 to <32 x i32>
  %shuffle.i.i.i.i.i.i.i.i.i.i.i.i.i = shufflevector <32 x i32> %6, <32 x i32> poison, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 12, i32 13, i32 14, i32 15>
  %shuffle1.i.i.i.i.i.i.i.i.i.i.i.i.i.i = shufflevector <32 x i32> %6, <32 x i32> poison, <16 x i32> <i32 16, i32 17, i32 18, i32 19, i32 20, i32 21, i32 22, i32 23, i32 24, i32 25, i32 26, i32 27, i32 28, i32 29, i32 30, i32 31>
  tail call void @llvm.aie2p.release(i32 50, i32 1)
  %inc.i = add nuw nsw i32 %j.026.i, 1
  %exitcond.not.i = icmp eq i32 %inc.i, 6
  br i1 %exitcond.not.i, label %for.cond.cleanup3.i, label %for.body4.i, !llvm.loop !25

_Z4projILi256ELi1536EEvP8bfloat16S1_S1_S1_PfPb.exit: ; preds = %for.cond.cleanup3.i
  ret void
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.aie2p.acquire(i32, i32) #1

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(inaccessiblemem: read)
declare <32 x float> @llvm.aie2p.I512.I512.ACC1024.bf.mac.conf(<32 x bfloat>, <32 x bfloat>, <32 x float>, i32) #2

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.aie2p.release(i32, i32) #1

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(inaccessiblemem: read)
declare <32 x bfloat> @llvm.aie2p.v32accfloat.to.v32bf16(<32 x float>) #2

attributes #0 = { mustprogress nofree norecurse nosync nounwind "no-builtin-memcpy" "no-builtin-memmove" "no-builtin-memset" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind willreturn }
attributes #2 = { mustprogress nocallback nofree nosync nounwind willreturn memory(inaccessiblemem: read) }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 22.0.0 (https://github.com/Xilinx/llvm-aie 91977805fa98e0ef4e2239c2b65a3dcb1eee4b41)"}
!2 = !{!3, !3, i64 0}
!3 = !{!"bool", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C++ TBAA"}
!6 = !{!4, !4, i64 0}
!7 = distinct !{!7, !8}
!8 = !{!"llvm.loop.mustprogress"}
!9 = !{!10, !12, !14}
!10 = distinct !{!10, !11, !"_ZN3aie6detail18load_vector_helperI8bfloat16Lj32EL15aie_dm_resource0EE3runEPKS2_: %agg.result"}
!11 = distinct !{!11, !"_ZN3aie6detail18load_vector_helperI8bfloat16Lj32EL15aie_dm_resource0EE3runEPKS2_"}
!12 = distinct !{!12, !13, !"_ZN3aie6detail11load_vectorILj32EL15aie_dm_resource0E8bfloat16EEDaPKT1_: %agg.result"}
!13 = distinct !{!13, !"_ZN3aie6detail11load_vectorILj32EL15aie_dm_resource0E8bfloat16EEDaPKT1_"}
!14 = distinct !{!14, !15, !"_ZN3aie6load_vILj32EL15aie_dm_resource0ETkNS_21DecoratedElemBaseTypeE8bfloat16EENS_6vectorIN22aie_dm_resource_removeIT1_E4typeEXT_EEEPKS5_: %agg.result"}
!15 = distinct !{!15, !"_ZN3aie6load_vILj32EL15aie_dm_resource0ETkNS_21DecoratedElemBaseTypeE8bfloat16EENS_6vectorIN22aie_dm_resource_removeIT1_E4typeEXT_EEEPKS5_"}
!16 = distinct !{!16, !8}
!17 = !{!18, !20, !22}
!18 = distinct !{!18, !19, !"_ZN3aie6detail18load_vector_helperI8bfloat16Lj32EL15aie_dm_resource0EE3runEPKS2_: %agg.result"}
!19 = distinct !{!19, !"_ZN3aie6detail18load_vector_helperI8bfloat16Lj32EL15aie_dm_resource0EE3runEPKS2_"}
!20 = distinct !{!20, !21, !"_ZN3aie6detail11load_vectorILj32EL15aie_dm_resource0E8bfloat16EEDaPKT1_: %agg.result"}
!21 = distinct !{!21, !"_ZN3aie6detail11load_vectorILj32EL15aie_dm_resource0E8bfloat16EEDaPKT1_"}
!22 = distinct !{!22, !23, !"_ZN3aie6load_vILj32EL15aie_dm_resource0ETkNS_21DecoratedElemBaseTypeE8bfloat16EENS_6vectorIN22aie_dm_resource_removeIT1_E4typeEXT_EEEPKS5_: %agg.result"}
!23 = distinct !{!23, !"_ZN3aie6load_vILj32EL15aie_dm_resource0ETkNS_21DecoratedElemBaseTypeE8bfloat16EENS_6vectorIN22aie_dm_resource_removeIT1_E4typeEXT_EEEPKS5_"}
!24 = distinct !{!24, !8}
!25 = distinct !{!25, !8}
