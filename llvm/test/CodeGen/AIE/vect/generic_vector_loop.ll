;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc < %s -mtriple=aie | FileCheck %s

declare i8* @malloc(i64)

declare void @free(i8*)

define void @conv2d(float* %0, float* %1, float* %2) !dbg !3 {
; CHECK-LABEL: conv2d:
; CHECK:       .Lfunc_begin0:

  br label %4, !dbg !7

4:                                                ; preds = %133, %3
  %5 = phi i64 [ %134, %133 ], [ 0, %3 ]
  %6 = icmp slt i64 %5, 16, !dbg !9
  br i1 %6, label %7, label %135, !dbg !10

7:                                                ; preds = %10, %4
  %8 = phi i64 [ %132, %10 ], [ 0, %4 ]
  %9 = icmp slt i64 %8, 256, !dbg !11
  br i1 %9, label %10, label %133, !dbg !12

10:                                               ; preds = %7
  %11 = sub i64 272, %8, !dbg !13
  %12 = trunc i64 %11 to i32, !dbg !14
  %13 = insertelement <8 x i32> undef, i32 %12, i32 0, !dbg !15
  %14 = shufflevector <8 x i32> %13, <8 x i32> undef, <8 x i32> zeroinitializer, !dbg !16
  %15 = icmp slt <8 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>, %14, !dbg !17
  %16 = mul i64 %5, 272, !dbg !18
  %17 = add i64 %16, %8, !dbg !19
  %18 = getelementptr float, float* %0, i64 %17, !dbg !20
  %19 = bitcast float* %18 to <8 x float>*, !dbg !21
  %20 = call <8 x float> @llvm.masked.load.v8f32.p0v8f32(<8 x float>* %19, i32 4, <8 x i1> %15, <8 x float> zeroinitializer), !dbg !22
  %21 = getelementptr float, float* %1, i64 0, !dbg !23
  %22 = load float, float* %21, align 4, !dbg !24
  %23 = insertelement <8 x float> undef, float %22, i32 0, !dbg !25
  %24 = shufflevector <8 x float> %23, <8 x float> undef, <8 x i32> zeroinitializer, !dbg !26
  %25 = fmul <8 x float> %20, %24, !dbg !27
  %26 = fadd <8 x float> %25, zeroinitializer, !dbg !28
  %27 = add i64 %8, 1, !dbg !29
  %28 = sub i64 272, %27, !dbg !30
  %29 = trunc i64 %28 to i32, !dbg !31
  %30 = insertelement <8 x i32> undef, i32 %29, i32 0, !dbg !32
  %31 = shufflevector <8 x i32> %30, <8 x i32> undef, <8 x i32> zeroinitializer, !dbg !33
  %32 = icmp slt <8 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>, %31, !dbg !34
  %33 = add i64 %16, %27, !dbg !35
  %34 = getelementptr float, float* %0, i64 %33, !dbg !36
  %35 = bitcast float* %34 to <8 x float>*, !dbg !37
  %36 = call <8 x float> @llvm.masked.load.v8f32.p0v8f32(<8 x float>* %35, i32 4, <8 x i1> %32, <8 x float> zeroinitializer), !dbg !38
  %37 = getelementptr float, float* %1, i64 1, !dbg !39
  %38 = load float, float* %37, align 4, !dbg !40
  %39 = insertelement <8 x float> undef, float %38, i32 0, !dbg !41
  %40 = shufflevector <8 x float> %39, <8 x float> undef, <8 x i32> zeroinitializer, !dbg !42
  %41 = fmul <8 x float> %36, %40, !dbg !43
  %42 = fadd <8 x float> %26, %41, !dbg !44
  %43 = add i64 %8, 2, !dbg !45
  %44 = sub i64 272, %43, !dbg !46
  %45 = trunc i64 %44 to i32, !dbg !47
  %46 = insertelement <8 x i32> undef, i32 %45, i32 0, !dbg !48
  %47 = shufflevector <8 x i32> %46, <8 x i32> undef, <8 x i32> zeroinitializer, !dbg !49
  %48 = icmp slt <8 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>, %47, !dbg !50
  %49 = add i64 %16, %43, !dbg !51
  %50 = getelementptr float, float* %0, i64 %49, !dbg !52
  %51 = bitcast float* %50 to <8 x float>*, !dbg !53
  %52 = call <8 x float> @llvm.masked.load.v8f32.p0v8f32(<8 x float>* %51, i32 4, <8 x i1> %48, <8 x float> zeroinitializer), !dbg !54
  %53 = getelementptr float, float* %1, i64 2, !dbg !55
  %54 = load float, float* %53, align 4, !dbg !56
  %55 = insertelement <8 x float> undef, float %54, i32 0, !dbg !57
  %56 = shufflevector <8 x float> %55, <8 x float> undef, <8 x i32> zeroinitializer, !dbg !58
  %57 = fmul <8 x float> %52, %56, !dbg !59
  %58 = fadd <8 x float> %42, %57, !dbg !60
  %59 = add i64 %5, 1, !dbg !61
  %60 = mul i64 %59, 272, !dbg !62
  %61 = add i64 %60, %8, !dbg !63
  %62 = getelementptr float, float* %0, i64 %61, !dbg !64
  %63 = bitcast float* %62 to <8 x float>*, !dbg !65
  %64 = call <8 x float> @llvm.masked.load.v8f32.p0v8f32(<8 x float>* %63, i32 4, <8 x i1> %15, <8 x float> zeroinitializer), !dbg !66
  %65 = getelementptr float, float* %1, i64 3, !dbg !67
  %66 = load float, float* %65, align 4, !dbg !68
  %67 = insertelement <8 x float> undef, float %66, i32 0, !dbg !69
  %68 = shufflevector <8 x float> %67, <8 x float> undef, <8 x i32> zeroinitializer, !dbg !70
  %69 = fmul <8 x float> %64, %68, !dbg !71
  %70 = fadd <8 x float> %58, %69, !dbg !72
  %71 = add i64 %60, %27, !dbg !73
  %72 = getelementptr float, float* %0, i64 %71, !dbg !74
  %73 = bitcast float* %72 to <8 x float>*, !dbg !75
  %74 = call <8 x float> @llvm.masked.load.v8f32.p0v8f32(<8 x float>* %73, i32 4, <8 x i1> %32, <8 x float> zeroinitializer), !dbg !76
  %75 = getelementptr float, float* %1, i64 4, !dbg !77
  %76 = load float, float* %75, align 4, !dbg !78
  %77 = insertelement <8 x float> undef, float %76, i32 0, !dbg !79
  %78 = shufflevector <8 x float> %77, <8 x float> undef, <8 x i32> zeroinitializer, !dbg !80
  %79 = fmul <8 x float> %74, %78, !dbg !81
  %80 = fadd <8 x float> %70, %79, !dbg !82
  %81 = add i64 %60, %43, !dbg !83
  %82 = getelementptr float, float* %0, i64 %81, !dbg !84
  %83 = bitcast float* %82 to <8 x float>*, !dbg !85
  %84 = call <8 x float> @llvm.masked.load.v8f32.p0v8f32(<8 x float>* %83, i32 4, <8 x i1> %48, <8 x float> zeroinitializer), !dbg !86
  %85 = getelementptr float, float* %1, i64 5, !dbg !87
  %86 = load float, float* %85, align 4, !dbg !88
  %87 = insertelement <8 x float> undef, float %86, i32 0, !dbg !89
  %88 = shufflevector <8 x float> %87, <8 x float> undef, <8 x i32> zeroinitializer, !dbg !90
  %89 = fmul <8 x float> %84, %88, !dbg !91
  %90 = fadd <8 x float> %80, %89, !dbg !92
  %91 = add i64 %5, 2, !dbg !93
  %92 = mul i64 %91, 272, !dbg !94
  %93 = add i64 %92, %8, !dbg !95
  %94 = getelementptr float, float* %0, i64 %93, !dbg !96
  %95 = bitcast float* %94 to <8 x float>*, !dbg !97
  %96 = call <8 x float> @llvm.masked.load.v8f32.p0v8f32(<8 x float>* %95, i32 4, <8 x i1> %15, <8 x float> zeroinitializer), !dbg !98
  %97 = getelementptr float, float* %1, i64 6, !dbg !99
  %98 = load float, float* %97, align 4, !dbg !100
  %99 = insertelement <8 x float> undef, float %98, i32 0, !dbg !101
  %100 = shufflevector <8 x float> %99, <8 x float> undef, <8 x i32> zeroinitializer, !dbg !102
  %101 = fmul <8 x float> %96, %100, !dbg !103
  %102 = fadd <8 x float> %90, %101, !dbg !104
  %103 = add i64 %92, %27, !dbg !105
  %104 = getelementptr float, float* %0, i64 %103, !dbg !106
  %105 = bitcast float* %104 to <8 x float>*, !dbg !107
  %106 = call <8 x float> @llvm.masked.load.v8f32.p0v8f32(<8 x float>* %105, i32 4, <8 x i1> %32, <8 x float> zeroinitializer), !dbg !108
  %107 = getelementptr float, float* %1, i64 7, !dbg !109
  %108 = load float, float* %107, align 4, !dbg !110
  %109 = insertelement <8 x float> undef, float %108, i32 0, !dbg !111
  %110 = shufflevector <8 x float> %109, <8 x float> undef, <8 x i32> zeroinitializer, !dbg !112
  %111 = fmul <8 x float> %106, %110, !dbg !113
  %112 = fadd <8 x float> %102, %111, !dbg !114
  %113 = add i64 %92, %43, !dbg !115
  %114 = getelementptr float, float* %0, i64 %113, !dbg !116
  %115 = bitcast float* %114 to <8 x float>*, !dbg !117
  %116 = call <8 x float> @llvm.masked.load.v8f32.p0v8f32(<8 x float>* %115, i32 4, <8 x i1> %48, <8 x float> zeroinitializer), !dbg !118
  %117 = getelementptr float, float* %1, i64 8, !dbg !119
  %118 = load float, float* %117, align 4, !dbg !120
  %119 = insertelement <8 x float> undef, float %118, i32 0, !dbg !121
  %120 = shufflevector <8 x float> %119, <8 x float> undef, <8 x i32> zeroinitializer, !dbg !122
  %121 = fmul <8 x float> %116, %120, !dbg !123
  %122 = fadd <8 x float> %112, %121, !dbg !124
  %123 = sub i64 256, %8, !dbg !125
  %124 = trunc i64 %123 to i32, !dbg !126
  %125 = insertelement <8 x i32> undef, i32 %124, i32 0, !dbg !127
  %126 = shufflevector <8 x i32> %125, <8 x i32> undef, <8 x i32> zeroinitializer, !dbg !128
  %127 = icmp slt <8 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>, %126, !dbg !129
  %128 = mul i64 %5, 256, !dbg !130
  %129 = add i64 %128, %8, !dbg !131
  %130 = getelementptr float, float* %2, i64 %129, !dbg !132
  %131 = bitcast float* %130 to <8 x float>*, !dbg !133
  call void @llvm.masked.store.v8f32.p0v8f32(<8 x float> %122, <8 x float>* %131, i32 4, <8 x i1> %127), !dbg !134
  %132 = add i64 %8, 8, !dbg !135
  br label %7, !dbg !136

133:                                              ; preds = %7
  %134 = add i64 %5, 1, !dbg !137
  br label %4, !dbg !138

135:                                              ; preds = %4
  ret void, !dbg !139
}

; Function Attrs: argmemonly nocallback nofree nosync nounwind readonly willreturn
declare <8 x float> @llvm.masked.load.v8f32.p0v8f32(<8 x float>*, i32 immarg, <8 x i1>, <8 x float>) #0

; Function Attrs: argmemonly nocallback nofree nosync nounwind willreturn writeonly
declare void @llvm.masked.store.v8f32.p0v8f32(<8 x float>, <8 x float>*, i32 immarg, <8 x i1>) #1

attributes #0 = { argmemonly nocallback nofree nosync nounwind readonly willreturn }
attributes #1 = { argmemonly nocallback nofree nosync nounwind willreturn writeonly }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "mlir", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug)
!1 = !DIFile(filename: "LLVMDialectModule", directory: "/")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = distinct !DISubprogram(name: "conv2d", linkageName: "conv2d", scope: null, file: !4, line: 2, type: !5, scopeLine: 2, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
!4 = !DIFile(filename: "<stdin>", directory: "/wrk/hdstaff/stephenn/peano_usage/aie-vectorize")
!5 = !DISubroutineType(types: !6)
!6 = !{}
!7 = !DILocation(line: 10, column: 5, scope: !8)
!8 = !DILexicalBlockFile(scope: !3, file: !4, discriminator: 0)
!9 = !DILocation(line: 12, column: 10, scope: !8)
!10 = !DILocation(line: 13, column: 5, scope: !8)
!11 = !DILocation(line: 15, column: 11, scope: !8)
!12 = !DILocation(line: 16, column: 5, scope: !8)
!13 = !DILocation(line: 19, column: 11, scope: !8)
!14 = !DILocation(line: 21, column: 11, scope: !8)
!15 = !DILocation(line: 24, column: 11, scope: !8)
!16 = !DILocation(line: 25, column: 11, scope: !8)
!17 = !DILocation(line: 26, column: 11, scope: !8)
!18 = !DILocation(line: 27, column: 11, scope: !8)
!19 = !DILocation(line: 28, column: 11, scope: !8)
!20 = !DILocation(line: 29, column: 11, scope: !8)
!21 = !DILocation(line: 30, column: 11, scope: !8)
!22 = !DILocation(line: 31, column: 11, scope: !8)
!23 = !DILocation(line: 35, column: 11, scope: !8)
!24 = !DILocation(line: 36, column: 11, scope: !8)
!25 = !DILocation(line: 38, column: 11, scope: !8)
!26 = !DILocation(line: 39, column: 11, scope: !8)
!27 = !DILocation(line: 40, column: 11, scope: !8)
!28 = !DILocation(line: 41, column: 11, scope: !8)
!29 = !DILocation(line: 42, column: 11, scope: !8)
!30 = !DILocation(line: 43, column: 11, scope: !8)
!31 = !DILocation(line: 44, column: 11, scope: !8)
!32 = !DILocation(line: 45, column: 11, scope: !8)
!33 = !DILocation(line: 46, column: 11, scope: !8)
!34 = !DILocation(line: 47, column: 11, scope: !8)
!35 = !DILocation(line: 48, column: 11, scope: !8)
!36 = !DILocation(line: 49, column: 11, scope: !8)
!37 = !DILocation(line: 50, column: 11, scope: !8)
!38 = !DILocation(line: 51, column: 11, scope: !8)
!39 = !DILocation(line: 53, column: 11, scope: !8)
!40 = !DILocation(line: 54, column: 11, scope: !8)
!41 = !DILocation(line: 55, column: 11, scope: !8)
!42 = !DILocation(line: 56, column: 11, scope: !8)
!43 = !DILocation(line: 57, column: 11, scope: !8)
!44 = !DILocation(line: 58, column: 11, scope: !8)
!45 = !DILocation(line: 59, column: 11, scope: !8)
!46 = !DILocation(line: 60, column: 11, scope: !8)
!47 = !DILocation(line: 61, column: 11, scope: !8)
!48 = !DILocation(line: 62, column: 11, scope: !8)
!49 = !DILocation(line: 63, column: 11, scope: !8)
!50 = !DILocation(line: 64, column: 11, scope: !8)
!51 = !DILocation(line: 65, column: 11, scope: !8)
!52 = !DILocation(line: 66, column: 11, scope: !8)
!53 = !DILocation(line: 67, column: 11, scope: !8)
!54 = !DILocation(line: 68, column: 11, scope: !8)
!55 = !DILocation(line: 70, column: 11, scope: !8)
!56 = !DILocation(line: 71, column: 11, scope: !8)
!57 = !DILocation(line: 72, column: 11, scope: !8)
!58 = !DILocation(line: 73, column: 11, scope: !8)
!59 = !DILocation(line: 74, column: 11, scope: !8)
!60 = !DILocation(line: 75, column: 11, scope: !8)
!61 = !DILocation(line: 76, column: 11, scope: !8)
!62 = !DILocation(line: 77, column: 11, scope: !8)
!63 = !DILocation(line: 78, column: 11, scope: !8)
!64 = !DILocation(line: 79, column: 11, scope: !8)
!65 = !DILocation(line: 80, column: 11, scope: !8)
!66 = !DILocation(line: 81, column: 11, scope: !8)
!67 = !DILocation(line: 84, column: 11, scope: !8)
!68 = !DILocation(line: 85, column: 11, scope: !8)
!69 = !DILocation(line: 86, column: 11, scope: !8)
!70 = !DILocation(line: 87, column: 11, scope: !8)
!71 = !DILocation(line: 88, column: 11, scope: !8)
!72 = !DILocation(line: 89, column: 11, scope: !8)
!73 = !DILocation(line: 90, column: 11, scope: !8)
!74 = !DILocation(line: 91, column: 11, scope: !8)
!75 = !DILocation(line: 92, column: 11, scope: !8)
!76 = !DILocation(line: 93, column: 11, scope: !8)
!77 = !DILocation(line: 95, column: 11, scope: !8)
!78 = !DILocation(line: 96, column: 11, scope: !8)
!79 = !DILocation(line: 97, column: 11, scope: !8)
!80 = !DILocation(line: 98, column: 11, scope: !8)
!81 = !DILocation(line: 99, column: 11, scope: !8)
!82 = !DILocation(line: 100, column: 11, scope: !8)
!83 = !DILocation(line: 101, column: 11, scope: !8)
!84 = !DILocation(line: 102, column: 11, scope: !8)
!85 = !DILocation(line: 103, column: 11, scope: !8)
!86 = !DILocation(line: 104, column: 11, scope: !8)
!87 = !DILocation(line: 106, column: 11, scope: !8)
!88 = !DILocation(line: 107, column: 12, scope: !8)
!89 = !DILocation(line: 108, column: 12, scope: !8)
!90 = !DILocation(line: 109, column: 12, scope: !8)
!91 = !DILocation(line: 110, column: 12, scope: !8)
!92 = !DILocation(line: 111, column: 12, scope: !8)
!93 = !DILocation(line: 112, column: 12, scope: !8)
!94 = !DILocation(line: 113, column: 12, scope: !8)
!95 = !DILocation(line: 114, column: 12, scope: !8)
!96 = !DILocation(line: 115, column: 12, scope: !8)
!97 = !DILocation(line: 116, column: 12, scope: !8)
!98 = !DILocation(line: 117, column: 12, scope: !8)
!99 = !DILocation(line: 120, column: 12, scope: !8)
!100 = !DILocation(line: 121, column: 12, scope: !8)
!101 = !DILocation(line: 122, column: 12, scope: !8)
!102 = !DILocation(line: 123, column: 12, scope: !8)
!103 = !DILocation(line: 124, column: 12, scope: !8)
!104 = !DILocation(line: 125, column: 12, scope: !8)
!105 = !DILocation(line: 126, column: 12, scope: !8)
!106 = !DILocation(line: 127, column: 12, scope: !8)
!107 = !DILocation(line: 128, column: 12, scope: !8)
!108 = !DILocation(line: 129, column: 12, scope: !8)
!109 = !DILocation(line: 131, column: 12, scope: !8)
!110 = !DILocation(line: 132, column: 12, scope: !8)
!111 = !DILocation(line: 133, column: 12, scope: !8)
!112 = !DILocation(line: 134, column: 12, scope: !8)
!113 = !DILocation(line: 135, column: 12, scope: !8)
!114 = !DILocation(line: 136, column: 12, scope: !8)
!115 = !DILocation(line: 137, column: 12, scope: !8)
!116 = !DILocation(line: 138, column: 12, scope: !8)
!117 = !DILocation(line: 139, column: 12, scope: !8)
!118 = !DILocation(line: 140, column: 12, scope: !8)
!119 = !DILocation(line: 142, column: 12, scope: !8)
!120 = !DILocation(line: 143, column: 12, scope: !8)
!121 = !DILocation(line: 144, column: 12, scope: !8)
!122 = !DILocation(line: 145, column: 12, scope: !8)
!123 = !DILocation(line: 146, column: 12, scope: !8)
!124 = !DILocation(line: 147, column: 12, scope: !8)
!125 = !DILocation(line: 148, column: 12, scope: !8)
!126 = !DILocation(line: 149, column: 12, scope: !8)
!127 = !DILocation(line: 150, column: 12, scope: !8)
!128 = !DILocation(line: 151, column: 12, scope: !8)
!129 = !DILocation(line: 152, column: 12, scope: !8)
!130 = !DILocation(line: 153, column: 12, scope: !8)
!131 = !DILocation(line: 154, column: 12, scope: !8)
!132 = !DILocation(line: 155, column: 12, scope: !8)
!133 = !DILocation(line: 156, column: 12, scope: !8)
!134 = !DILocation(line: 157, column: 5, scope: !8)
!135 = !DILocation(line: 158, column: 12, scope: !8)
!136 = !DILocation(line: 159, column: 5, scope: !8)
!137 = !DILocation(line: 161, column: 12, scope: !8)
!138 = !DILocation(line: 162, column: 5, scope: !8)
!139 = !DILocation(line: 164, column: 5, scope: !8)
