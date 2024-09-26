; ModuleID = '<stdin>'
source_filename = "0_0/src/0_0.cc"
target datalayout = "e-m:e-p:20:32-i1:8:32-i8:8:32-i16:16:32-i32:32:32-f32:32:32-i64:32-f64:32-a:0:32-n32"
target triple = "aie2-none-unknown-elf"

$run_fn = comdat any

; Function Attrs: nounwind memory(none)
declare <16 x bfloat> @llvm.aie2.v16bfloat16() #0

; Function Attrs: nounwind memory(none)
declare <32 x bfloat> @llvm.aie2.v32bfloat16() #0

; Function Attrs: nounwind memory(none)
declare <32 x bfloat> @llvm.aie2.vbroadcast16.bf512(bfloat) #0

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(inaccessiblemem: write)
declare void @llvm.assume(i1 noundef) #1

; Function Attrs: nounwind memory(none)
declare <16 x bfloat> @llvm.aie2.ext.bf256.bf512(<32 x bfloat>, i32) #0

; Function Attrs: nounwind memory(none)
declare <32 x bfloat> @llvm.aie2.set.bf512.bf256(<16 x bfloat>, i32) #0

; Function Attrs: nounwind memory(none)
declare <32 x bfloat> @llvm.aie2.upd.bf512.bf256(<32 x bfloat>, <16 x bfloat>, i32) #0

; Function Attrs: nounwind memory(inaccessiblemem: read)
declare <8 x i64> @llvm.aie2.bf.mac16.conf(<32 x bfloat>, <32 x bfloat>, <8 x i64>, i32) #2

; Function Attrs: nounwind memory(none)
declare <8 x i64> @llvm.aie2.v16accfloat() #0

; Function Attrs: nounwind memory(inaccessiblemem: read)
declare <8 x i64> @llvm.aie2.bf.mul16.conf(<32 x bfloat>, <32 x bfloat>, i32) #2

; Function Attrs: nounwind memory(none)
declare <8 x i64> @llvm.aie2.v16bf16.to.v16accfloat(<16 x bfloat>) #0

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(inaccessiblemem: read)
declare <8 x i64> @llvm.aie2.add.accfloat(<8 x i64>, <8 x i64>, i32) #3

; Function Attrs: nounwind memory(inaccessiblemem: read)
declare <16 x bfloat> @llvm.aie2.v16accfloat.to.v16bf16(<8 x i64>) #2

; Function Attrs: mustprogress noinline
define weak_odr dso_local void @run_fn(ptr noalias %ifm, ptr noalias %ofm, ptr nonnull align 32 dereferenceable(64) %params) local_unnamed_addr #4 comdat align 2 {
entry:
  %0 = tail call noundef <16 x bfloat> @llvm.aie2.v16bfloat16()
  %1 = tail call noundef <32 x bfloat> @llvm.aie2.v32bfloat16()
  %2 = tail call noundef <32 x bfloat> @llvm.aie2.vbroadcast16.bf512(bfloat 0xR3F80)
  %3 = tail call <16 x bfloat> @llvm.aie2.ext.bf256.bf512(<32 x bfloat> %2, i32 0)
  %4 = tail call noundef <32 x bfloat> @llvm.aie2.vbroadcast16.bf512(bfloat 0xR3F00)
  %5 = tail call <16 x bfloat> @llvm.aie2.ext.bf256.bf512(<32 x bfloat> %4, i32 0)
  %op_params5.i = getelementptr inbounds i8, ptr %params, i20 32
  %6 = load i8, ptr %op_params5.i, align 32, !tbaa !4
  %tobool.not.i = icmp eq i8 %6, 0
  %7 = tail call noundef <8 x i64> @llvm.aie2.v16accfloat()
  %8 = tail call noundef <8 x i64> @llvm.aie2.v16bf16.to.v16accfloat(<16 x bfloat> %3)
  %9 = tail call noundef <8 x i64> @llvm.aie2.clr16f.conf()
  %10 = tail call noundef <8 x i64> @llvm.aie2.add.accfloat(<8 x i64> %8, <8 x i64> %9, i32 28)
  %11 = tail call noundef <16 x bfloat> @llvm.aie2.v16accfloat.to.v16bf16(<8 x i64> %10)
  %12 = tail call <32 x bfloat> @llvm.aie2.set.bf512.bf256(<16 x bfloat> %11, i32 0)
  %13 = tail call <32 x bfloat> @llvm.aie2.set.bf512.bf256(<16 x bfloat> %3, i32 0)
  br i1 %tobool.not.i, label %if.else.i, label %if.then.i

if.then.i:                                        ; preds = %entry
  %14 = tail call noundef i32 @llvm.aie2.vgebf16(<32 x bfloat> %12, <32 x bfloat> %13)
  br label %for.body.lr.ph

if.else.i:                                        ; preds = %entry
  %15 = tail call noundef i32 @llvm.aie2.vltbf16(<32 x bfloat> %12, <32 x bfloat> %13)
  br label %for.body.lr.ph

for.body.lr.ph:                                   ; preds = %if.else.i, %if.then.i
  %or.i.i.i.i.pn.in.i = phi i32 [ %15, %if.else.i ], [ %14, %if.then.i ]
  %or.i.i.i.i.pn.i = and i32 %or.i.i.i.i.pn.in.i, 65535
  %16 = load i32, ptr %params, align 32, !tbaa !11
  %div7 = lshr i32 %16, 4
  %cmp = icmp ugt i32 %16, 127
  tail call void @llvm.assume(i1 %cmp)
  %17 = tail call noundef <32 x bfloat> @llvm.aie2.vbroadcast16.bf512(bfloat 0xR4040)
  %18 = tail call <16 x bfloat> @llvm.aie2.ext.bf256.bf512(<32 x bfloat> %17, i32 0)
  %19 = bitcast <32 x bfloat> %13 to <32 x i16>
  br label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret void

for.body:                                         ; preds = %for.body, %for.body.lr.ph
  %i.013 = phi i32 [ 0, %for.body.lr.ph ], [ %add, %for.body ]
  %p_out.0.in12 = phi ptr [ %ofm, %for.body.lr.ph ], [ %add.ptr.i11.i, %for.body ]
  %p_in.0.in11 = phi ptr [ %ifm, %for.body.lr.ph ], [ %add.ptr.i.i, %for.body ]
  %p_out.0 = addrspacecast ptr %p_out.0.in12 to ptr addrspace(6)
  %p_in.0 = addrspacecast ptr %p_in.0.in11 to ptr addrspace(5)
  %20 = load <16 x bfloat>, ptr addrspace(5) %p_in.0, align 32, !tbaa !12
  %add.ptr.i.i = getelementptr inbounds i8, ptr %p_in.0.in11, i20 32
  %21 = tail call <32 x bfloat> @llvm.aie2.set.bf512.bf256(<16 x bfloat> %20, i32 0)
  %22 = tail call <32 x bfloat> @llvm.aie2.set.bf512.bf256(<16 x bfloat> %18, i32 0)
  %23 = tail call { <32 x bfloat>, i32 } @llvm.aie2.vmin.gebf16(<32 x bfloat> %21, <32 x bfloat> %22)
  %24 = extractvalue { <32 x bfloat>, i32 } %23, 0
  %25 = tail call <16 x bfloat> @llvm.aie2.ext.bf256.bf512(<32 x bfloat> %24, i32 0)
  %26 = tail call noundef <32 x bfloat> @llvm.aie2.vbroadcast16.bf512(bfloat 0xRC040)
  %27 = tail call <16 x bfloat> @llvm.aie2.ext.bf256.bf512(<32 x bfloat> %26, i32 0)
  %28 = tail call <32 x bfloat> @llvm.aie2.set.bf512.bf256(<16 x bfloat> %25, i32 0)
  %29 = tail call <32 x bfloat> @llvm.aie2.set.bf512.bf256(<16 x bfloat> %27, i32 0)
  %30 = tail call { <32 x bfloat>, i32 } @llvm.aie2.vmax.ltbf16(<32 x bfloat> %28, <32 x bfloat> %29)
  %31 = extractvalue { <32 x bfloat>, i32 } %30, 0
  %32 = tail call <16 x bfloat> @llvm.aie2.ext.bf256.bf512(<32 x bfloat> %31, i32 0)
  %33 = bitcast <32 x bfloat> %21 to <32 x i16>
  %34 = tail call <32 x i16> @llvm.aie2.vsel16(<32 x i16> %33, <32 x i16> %19, i32 %or.i.i.i.i.pn.i)
  %35 = bitcast <32 x i16> %34 to <32 x bfloat>
  %36 = tail call <16 x bfloat> @llvm.aie2.ext.bf256.bf512(<32 x bfloat> %35, i32 0)
  %37 = tail call noundef <8 x i64> @llvm.aie2.v16bf16.to.v16accfloat(<16 x bfloat> %5)
  %38 = tail call noundef <32 x bfloat> @llvm.aie2.vbroadcast16.bf512(bfloat 0xR3E2B)
  %39 = tail call <16 x bfloat> @llvm.aie2.ext.bf256.bf512(<32 x bfloat> %38, i32 0)
  %40 = tail call noundef <32 x bfloat> @llvm.aie2.vbroadcast16.bf512(bfloat 0xR0000)
  %41 = tail call <16 x bfloat> @llvm.aie2.ext.bf256.bf512(<32 x bfloat> %40, i32 0)
  %42 = tail call <32 x bfloat> @llvm.aie2.set.bf512.bf256(<16 x bfloat> %32, i32 0)
  %43 = tail call <32 x bfloat> @llvm.aie2.upd.bf512.bf256(<32 x bfloat> %42, <16 x bfloat> %41, i32 1)
  %44 = tail call <32 x bfloat> @llvm.aie2.set.bf512.bf256(<16 x bfloat> %39, i32 0)
  %45 = tail call <32 x bfloat> @llvm.aie2.upd.bf512.bf256(<32 x bfloat> %44, <16 x bfloat> %41, i32 1)
  %46 = tail call noundef <8 x i64> @llvm.aie2.bf.mac16.conf(<32 x bfloat> %43, <32 x bfloat> %45, <8 x i64> %37, i32 60)
  %47 = tail call noundef <16 x bfloat> @llvm.aie2.v16accfloat.to.v16bf16(<8 x i64> %46)
  %48 = tail call <32 x bfloat> @llvm.aie2.set.bf512.bf256(<16 x bfloat> %36, i32 0)
  %49 = tail call <32 x bfloat> @llvm.aie2.upd.bf512.bf256(<32 x bfloat> %48, <16 x bfloat> %41, i32 1)
  %50 = tail call <32 x bfloat> @llvm.aie2.set.bf512.bf256(<16 x bfloat> %47, i32 0)
  %51 = tail call <32 x bfloat> @llvm.aie2.upd.bf512.bf256(<32 x bfloat> %50, <16 x bfloat> %41, i32 1)
  %52 = tail call noundef <8 x i64> @llvm.aie2.bf.mul16.conf(<32 x bfloat> %49, <32 x bfloat> %51, i32 60)
  %53 = tail call noundef <16 x bfloat> @llvm.aie2.v16accfloat.to.v16bf16(<8 x i64> %52)
  store <16 x bfloat> %53, ptr addrspace(6) %p_out.0, align 32, !tbaa !12
  %add.ptr.i11.i = getelementptr inbounds i8, ptr %p_out.0.in12, i20 32
  %add = add nuw nsw i32 %i.013, 1
  %exitcond.not = icmp eq i32 %add, %div7
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body, !llvm.loop !13
}

; Function Attrs: nounwind memory(inaccessiblemem: read)
declare <8 x i64> @llvm.aie2.clr16f.conf() #2

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(none)
declare i32 @llvm.aie2.vgebf16(<32 x bfloat>, <32 x bfloat>) #5

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(none)
declare i32 @llvm.aie2.vltbf16(<32 x bfloat>, <32 x bfloat>) #5

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(none)
declare { <32 x bfloat>, i32 } @llvm.aie2.vmax.ltbf16(<32 x bfloat>, <32 x bfloat>) #5

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(none)
declare { <32 x bfloat>, i32 } @llvm.aie2.vmin.gebf16(<32 x bfloat>, <32 x bfloat>) #5

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(none)
declare <32 x i16> @llvm.aie2.vsel16(<32 x i16>, <32 x i16>, i32) #5

attributes #0 = { nounwind memory(none) }
attributes #1 = { nocallback nofree nosync nounwind willreturn memory(inaccessiblemem: write) }
attributes #2 = { nounwind memory(inaccessiblemem: read) }
attributes #3 = { nocallback nofree nosync nounwind willreturn memory(inaccessiblemem: read) }
attributes #4 = { mustprogress noinline "no-builtin-memcpy" "no-jump-tables"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #5 = { nocallback nofree nosync nounwind willreturn memory(none) }

!llvm.linker.options = !{}
!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 7, !"Dwarf Version", i32 4}
!1 = !{i32 2, !"Debug Info Version", i32 3}
!2 = !{i32 1, !"wchar_size", i32 4}
!3 = !{!"clang version 19.0.0git (git@gitenterprise.xilinx.com:gaetanb/llvm-aie.git 5bec13dce5ef1a8b0b46f73479ad1bbd7b894a0a)"}
!4 = !{!5, !7, i64 32}
!5 = !{!"_ZTS26elementwise_unary_params_tI18hardswish_params_tIu6__bf16EE", !6, i64 0, !7, i64 4, !7, i64 5, !7, i64 6, !7, i64 7, !9, i64 8, !9, i64 10, !6, i64 12, !6, i64 16, !6, i64 20, !6, i64 24, !6, i64 28, !10, i64 32}
!6 = !{!"int", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C++ TBAA"}
!9 = !{!"short", !7, i64 0}
!10 = !{!"_ZTS18hardswish_params_tIu6__bf16E", !7, i64 0}
!11 = !{!5, !6, i64 0}
!12 = !{!7, !7, i64 0}
!13 = distinct !{!13, !14, !15, !16}
!14 = !{!"llvm.loop.mustprogress"}
!15 = !{!"llvm.loop.itercount.range", i64 8}
!16 = !{!"llvm.loop.unroll.disable"}
