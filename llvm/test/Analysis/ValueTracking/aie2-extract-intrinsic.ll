;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: opt -passes=aa-eval -print-all-alias-modref-info -disable-output < %s 2>&1 | FileCheck %s

%struct.reduce_params_t = type <{ i32, i32, i32, i32, i32, i32, i32, i16, i16, i16, i16, i16, i16, i16, i16, i16, i16, i32, i16, i16, %struct.reducemax_params_t, [7 x i8] }>
%struct.reducemax_params_t = type { i8 }


; CHECK: NoAlias:      i16* %num_iter, <8 x i32>* %p_out_save_group.sroa.0.1
; Function Attrs: mustprogress noinline
define linkonce_odr dso_local void @function_NoAlias1(ptr noalias %ifm, ptr noalias %ofm, ptr noalias %params)  align 2 {
entry:
  br label %if.end54

if.end54:                                         ; preds = %if.end54.loopexit, %if.end
  %p_out_group.sroa.0.0 = phi ptr [ %ofm, %entry ]
  br label %for.cond56

for.cond56:                                       ; preds = %for.body60, %entry
  %iter = phi i32 [ 0, %if.end54 ], [ %inc61, %for.inc61 ]
  %p_out_save_group.sroa.0.1 = phi ptr [ %p_out_group.sroa.0.0, %if.end54], [ %extr1, %for.inc61 ]
  %num_iter = getelementptr inbounds %struct.reduce_params_t, ptr %params, i32 0, i32 7
  %ld = load i16, ptr %num_iter, align 4, !tbaa !26
  %conv57 = sext i16 %ld to i32
  %cmp58 = icmp slt i32 %iter, %conv57
  br i1 %cmp58, label %for.body60, label %for.cond.cleanup59

for.body60:                                       ; preds = %for.cond56
  %a = load i20, ptr %ifm, align 4, !tbaa !26
  %val = load <8 x i32>, ptr %ifm, align 4, !tbaa !26
  store <8 x i32> %val, ptr %p_out_save_group.sroa.0.1, align 32, !tbaa !30
  %call1 = call { ptr, i20 } @llvm.aie2.add.2d(ptr %p_out_save_group.sroa.0.1, i20 %a, i20 32, i20 %a, i20 %a)
  %extr1 = extractvalue { ptr, i20 } %call1, 0
  br label %for.inc61

for.inc61:                                        ; preds = %for.body60
  %inc61 = add nsw i32 %iter, 1
  br label %for.cond56

for.cond.cleanup59:                               ; preds = %for.cond56
  ret void

}

; CHECK: NoAlias:      i16* %num_iter, <8 x i32>* %p_out_save_group.sroa.0.1
; Function Attrs: mustprogress noinline
define linkonce_odr dso_local void @function_NoAlias2(ptr noalias %ifm, ptr noalias %ofm, ptr %params)  align 2 {
entry:
  br label %if.end54

if.end54:                                         ; preds = %if.end54.loopexit, %if.end
  %p_out_group.sroa.0.0 = phi ptr [ %ofm, %entry ]
  br label %for.cond56

for.cond56:                                       ; preds = %for.body60, %entry
  %iter = phi i32 [ 0, %if.end54 ], [ %inc61, %for.inc61 ]
  %p_out_save_group.sroa.0.1 = phi ptr [ %p_out_group.sroa.0.0, %if.end54], [ %extr1, %for.inc61 ]
  %num_iter = getelementptr inbounds %struct.reduce_params_t, ptr %params, i32 0, i32 7
  %ld = load i16, ptr %num_iter, align 4, !tbaa !26
  %conv57 = sext i16 %ld to i32
  %cmp58 = icmp slt i32 %iter, %conv57
  br i1 %cmp58, label %for.body60, label %for.cond.cleanup59

for.body60:                                       ; preds = %for.cond56
  %a = load i20, ptr %ifm, align 4, !tbaa !26
  %val = load <8 x i32>, ptr %ifm, align 4, !tbaa !26
  store <8 x i32> %val, ptr %p_out_save_group.sroa.0.1, align 32, !tbaa !30
  %call1 = call { ptr, i20 } @llvm.aie2.add.2d(ptr %p_out_save_group.sroa.0.1, i20 %a, i20 32, i20 %a, i20 %a)
  %extr1 = extractvalue { ptr, i20 } %call1, 0
  br label %for.inc61

for.inc61:                                        ; preds = %for.body60
  %inc61 = add nsw i32 %iter, 1
  br label %for.cond56

for.cond.cleanup59:                               ; preds = %for.cond56
  ret void

}

; CHECK: MayAlias:      i16* %num_iter, <8 x i32>* %p_out_save_group.sroa.0.1
; Function Attrs: mustprogress noinline
define linkonce_odr dso_local void @function_MayAlias(ptr noalias %ifm, ptr %ofm, ptr %params)  align 2 {
entry:
  br label %if.end54

if.end54:                                         ; preds = %if.end54.loopexit, %if.end
  %p_out_group.sroa.0.0 = phi ptr [ %ofm, %entry ]
  br label %for.cond56

for.cond56:                                       ; preds = %for.body60, %entry
  %iter = phi i32 [ 0, %if.end54 ], [ %inc61, %for.inc61 ]
  %p_out_save_group.sroa.0.1 = phi ptr [ %p_out_group.sroa.0.0, %if.end54], [ %extr1, %for.inc61 ]
  %num_iter = getelementptr inbounds %struct.reduce_params_t, ptr %params, i32 0, i32 7
  %ld = load i16, ptr %num_iter, align 4, !tbaa !26
  %conv57 = sext i16 %ld to i32
  %cmp58 = icmp slt i32 %iter, %conv57
  br i1 %cmp58, label %for.body60, label %for.cond.cleanup59

for.body60:                                       ; preds = %for.cond56
  %a = load i20, ptr %ifm, align 4, !tbaa !26
  %val = load <8 x i32>, ptr %ifm, align 4, !tbaa !26
  store <8 x i32> %val, ptr %p_out_save_group.sroa.0.1, align 32, !tbaa !30
  %call1 = call { ptr, i20 } @llvm.aie2.add.2d(ptr %p_out_save_group.sroa.0.1, i20 %a, i20 32, i20 %a, i20 %a)
  %extr1 = extractvalue { ptr, i20 } %call1, 0
  br label %for.inc61

for.inc61:                                        ; preds = %for.body60
  %inc61 = add nsw i32 %iter, 1
  br label %for.cond56

for.cond.cleanup59:                               ; preds = %for.cond56
  ret void

}

; Function Attrs: nounwind memory(none)
declare { ptr, i20 } @llvm.aie2.add.2d(ptr, i20, i20, i20, i20) #13

attributes #0 = { mustprogress "no-jump-tables"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #1 = { mustprogress noinline "no-jump-tables"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }

!llvm.linker.options = !{}
!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 18.0.0"}
!2 = !{}
!13 = !{!"any pointer", !14, i64 0}
!14 = !{!"omnipotent char", !15, i64 0}
!15 = !{!"Simple C++ TBAA"}
!16 = distinct !{!16, !17, !18}
!17 = !{!"llvm.loop.mustprogress"}
!18 = !{!"llvm.loop.unroll.disable"}
!19 = !{!20, !21, i64 12}
!20 = !{!"", !21, i64 0, !21, i64 4, !21, i64 8, !21, i64 12, !21, i64 16, !21, i64 20, !21, i64 24, !22, i64 28, !22, i64 30, !22, i64 32, !22, i64 34, !22, i64 36, !22, i64 38, !22, i64 40, !22, i64 42, !22, i64 44, !22, i64 46, !21, i64 48, !22, i64 52, !22, i64 54, !23, i64 56}
!21 = !{!"int", !14, i64 0}
!22 = !{!"short", !14, i64 0}
!23 = !{!"_ZTS18reducemax_params_tIaE"}
!24 = distinct !{!24, !17, !18}
!25 = !{!20, !21, i64 48}
!26 = !{!20, !22, i64 28}
!27 = !{!28}
!28 = distinct !{!28, !29, !": %params"}
!29 = distinct !{!29, !""}
!30 = !{!14, !14, i64 0}
!31 = distinct !{!31, !17, !18}
