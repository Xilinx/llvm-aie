;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: opt -mtriple=aie2 -passes=aa-eval -print-all-alias-modref-info -disable-output < %s 2>&1 | FileCheck %s --check-prefix=ENABLED-AIE-AA-DIS
; RUN: opt -mtriple=aie2 -passes=aa-eval -print-all-alias-modref-info -disable-output \
; RUN:    --aie-alias-analysis-disambiguation=false < %s 2>&1 | FileCheck %s --check-prefix=DISABLED-AIE-AA-DES


declare { ptr, i20 } @llvm.aie2.add.2d(ptr, i20, i20, i20, i20)

; ENABLED-AIE-AA-DIS-LABEL: Function: test
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %ptr_load, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %3, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %3, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %7, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %7, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %3, i8* %7
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %3
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %7
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %3
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %7
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %10, i8* %14

; DISABLED-AIE-AA-DES-LABEL: Function: test
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %ptr_load, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %7, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %7, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %7
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %3
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %7
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %3
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %7
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %14

; Function Attrs: nofree norecurse nounwind mustprogress
define dso_local i32 @test(ptr %a, i8 %b, i32 %c) local_unnamed_addr #0 {
entry:
  br label %for.body

for.body:                                         ; preds = %entry, %for.body
  %counter_load = phi i20 [ %9, %for.body ], [ 0, %entry ]
  %counter_store = phi i20 [ %13, %for.body ], [ 0, %entry ]
  %i.08 = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %ptr_load = phi ptr [ %10, %for.body ], [ %a, %entry ]
  %ptr_store = phi ptr [ %14, %for.body ], [ %a, %entry ]
  load i8, ptr %ptr_load
  store i8 %b, ptr %ptr_store

  %1 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %ptr_load, i20 0, i20 1, i20 2, i20 %counter_load)
  %2 = extractvalue { ptr, i20 } %1, 1
  %3 = extractvalue { ptr, i20 } %1, 0
  load i8, ptr %3
  
  %5 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %ptr_store, i20 0, i20 1, i20 2, i20 %counter_store)
  %6 = extractvalue { ptr, i20 } %5, 1
  %7 = extractvalue { ptr, i20 } %5, 0
  store i8 %b, ptr %7

  %8 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %3, i20 0, i20 1, i20 2, i20 %2)
  %9 = extractvalue { ptr, i20 } %8, 1
  %10 = extractvalue { ptr, i20 } %8, 0
  load i8, ptr %10

  %12 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %7, i20 0, i20 1, i20 2, i20 %6)
  %13 = extractvalue { ptr, i20 } %12, 1
  %14 = extractvalue { ptr, i20 } %12, 0
  store i8 %b, ptr %14

  %inc = add nuw nsw i32 %i.08, 1
  %exitcond.not = icmp eq i32 %inc, %c
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret i32 0
}

; ENABLED-AIE-AA-DIS-LABEL: Function: test_diff_iteration
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %ptr_load, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %3, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %3, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %7, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %7, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %3, i8* %7
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %10, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %3
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %10, i8* %7
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %14, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %14, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %14, i8* %3
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %14, i8* %7
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %10, i8* %14

; DISABLED-AIE-AA-DES-LABEL: Function: test_diff_iteration
; DISABLED-AIE-AA-DES:  MayAlias:     i8* %ptr_load, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:     i8* %3, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:     i8* %3, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:     i8* %7, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:     i8* %7, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:     i8* %3, i8* %7
; DISABLED-AIE-AA-DES:  MayAlias:     i8* %10, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:     i8* %10, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:     i8* %10, i8* %3
; DISABLED-AIE-AA-DES:  MayAlias:     i8* %10, i8* %7
; DISABLED-AIE-AA-DES:  MayAlias:     i8* %14, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:     i8* %14, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:     i8* %14, i8* %3
; DISABLED-AIE-AA-DES:  MayAlias:     i8* %14, i8* %7
; DISABLED-AIE-AA-DES:  MayAlias:     i8* %10, i8* %14

; Function Attrs: nofree norecurse nounwind mustprogress
define dso_local i32 @test_diff_iteration(ptr %a, i8 %b, i32 %c) local_unnamed_addr #0 {
entry:
  br label %for.body

for.body:                                         ; preds = %entry, %for.body
  %counter_load = phi i20 [ %9, %for.body ], [ 0, %entry ]
  %counter_store = phi i20 [ %13, %for.body ], [ 0, %entry ]
  %i.08 = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %ptr_load = phi ptr [ %10, %for.body ], [ %a, %entry ]
  %ptr_store = phi ptr [ %14, %for.body ], [ %a, %entry ]
  load i8, ptr %ptr_load
  store i8 %b, ptr %ptr_store

  %1 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %ptr_load, i20 0, i20 1, i20 2, i20 %counter_load)
  %2 = extractvalue { ptr, i20 } %1, 1
  %3 = extractvalue { ptr, i20 } %1, 0
  load i8, ptr %3
  
  ; here we break the iteration, we cannot state NoAlias.
  %5 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %ptr_store, i20 16, i20 1, i20 2, i20 %counter_store)
  %6 = extractvalue { ptr, i20 } %5, 1
  %7 = extractvalue { ptr, i20 } %5, 0
  store i8 %b, ptr %7

  %8 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %3, i20 0, i20 1, i20 2, i20 %2)
  %9 = extractvalue { ptr, i20 } %8, 1
  %10 = extractvalue { ptr, i20 } %8, 0
  load i8, ptr %10

  %12 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %7, i20 0, i20 1, i20 2, i20 %6)
  %13 = extractvalue { ptr, i20 } %12, 1
  %14 = extractvalue { ptr, i20 } %12, 0
  store i8 %b, ptr %14

  %inc = add nuw nsw i32 %i.08, 1
  %exitcond.not = icmp eq i32 %inc, %c
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret i32 0
}

; ENABLED-AIE-AA-DIS-LABEL: Function: test_with_gep_zero
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %ptr_load, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %3, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %3, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %gep, i8* %ptr_load 
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %gep, i8* %ptr_store 
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %3, i8* %gep 
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %3
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %gep 
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %3
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %gep
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %10, i8* %14

; DISABLED-AIE-AA-DES-LABEL: Function: test_with_gep_zero
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %ptr_load, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %gep, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %gep, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %gep
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %3
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %gep
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %3
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %gep
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %14

; Function Attrs: nofree norecurse nounwind mustprogress
define dso_local i32 @test_with_gep_zero(ptr %a, i8 %b, i32 %c) local_unnamed_addr #0 {
entry:
  br label %for.body

for.body:                                         ; preds = %entry, %for.body
  %counter_load = phi i20 [ %9, %for.body ], [ 0, %entry ]
  %counter_store = phi i20 [ %13, %for.body ], [ 0, %entry ]
  %i.08 = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %ptr_load = phi ptr [ %10, %for.body ], [ %a, %entry ]
  %ptr_store = phi ptr [ %14, %for.body ], [ %a, %entry ]
  load i8, ptr %ptr_load
  store i8 %b, ptr %ptr_store

  %1 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %ptr_load, i20 0, i20 1, i20 2, i20 %counter_load)
  %2 = extractvalue { ptr, i20 } %1, 1
  %3 = extractvalue { ptr, i20 } %1, 0
  load i8, ptr %3
  
  %5 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %ptr_store, i20 0, i20 1, i20 2, i20 %counter_store)
  %6 = extractvalue { ptr, i20 } %5, 1
  %7 = extractvalue { ptr, i20 } %5, 0

  ; GEP with offset zero is ok.
  %gep = getelementptr inbounds i8, ptr %7, i64 0

  store i8 %b, ptr %gep

  %8 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %3, i20 0, i20 1, i20 2, i20 %2)
  %9 = extractvalue { ptr, i20 } %8, 1
  %10 = extractvalue { ptr, i20 } %8, 0
  load i8, ptr %10

  %12 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %gep, i20 0, i20 1, i20 2, i20 %6)
  %13 = extractvalue { ptr, i20 } %12, 1
  %14 = extractvalue { ptr, i20 } %12, 0
  store i8 %b, ptr %14

  %inc = add nuw nsw i32 %i.08, 1
  %exitcond.not = icmp eq i32 %inc, %c
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret i32 0
}

; ENABLED-AIE-AA-DIS-LABEL: Function: test_with_gep_non_zero
; ENABLED-AIE-AA-DIS:  MayAlias:      i8* %ptr_load, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  NoAlias:       i8* %3, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  MayAlias:      i8* %3, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  MayAlias:      i8* %gep, i8* %ptr_load 
; ENABLED-AIE-AA-DIS:  MayAlias:      i8* %gep, i8* %ptr_store 
; ENABLED-AIE-AA-DIS:  MayAlias:      i8* %3, i8* %gep 
; ENABLED-AIE-AA-DIS:  NoAlias:       i8* %10, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  MayAlias:      i8* %10, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  NoAlias:       i8* %10, i8* %3
; ENABLED-AIE-AA-DIS:  MayAlias:      i8* %10, i8* %gep 
; ENABLED-AIE-AA-DIS:  MayAlias:      i8* %14, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  MayAlias:      i8* %14, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  MayAlias:      i8* %14, i8* %3
; ENABLED-AIE-AA-DIS:  MayAlias:      i8* %14, i8* %gep
; ENABLED-AIE-AA-DIS:  MayAlias:      i8* %10, i8* %14

; DISABLED-AIE-AA-DES-LABEL: Function: test_with_gep_non_zero
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %ptr_load, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %gep, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %gep, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %gep
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %3
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %gep
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %3
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %gep
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %14

; Function Attrs: nofree norecurse nounwind mustprogress
define dso_local i32 @test_with_gep_non_zero(ptr %a, i8 %b, i32 %c) local_unnamed_addr #0 {
entry:
  br label %for.body

for.body:                                         ; preds = %entry, %for.body
  %counter_load = phi i20 [ %9, %for.body ], [ 0, %entry ]
  %counter_store = phi i20 [ %13, %for.body ], [ 0, %entry ]
  %i.08 = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %ptr_load = phi ptr [ %10, %for.body ], [ %a, %entry ]
  %ptr_store = phi ptr [ %14, %for.body ], [ %a, %entry ]
  load i8, ptr %ptr_load
  store i8 %b, ptr %ptr_store

  %1 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %ptr_load, i20 0, i20 1, i20 2, i20 %counter_load)
  %2 = extractvalue { ptr, i20 } %1, 1
  %3 = extractvalue { ptr, i20 } %1, 0
  load i8, ptr %3
  
  %5 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %ptr_store, i20 0, i20 1, i20 2, i20 %counter_store)
  %6 = extractvalue { ptr, i20 } %5, 1
  %7 = extractvalue { ptr, i20 } %5, 0

  ; GEP with offset != from zero is NOT ok.
  %gep = getelementptr inbounds i8, ptr %7, i64 2

  store i8 %b, ptr %gep

  %8 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %3, i20 0, i20 1, i20 2, i20 %2)
  %9 = extractvalue { ptr, i20 } %8, 1
  %10 = extractvalue { ptr, i20 } %8, 0
  load i8, ptr %10

  %12 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %gep, i20 0, i20 1, i20 2, i20 %6)
  %13 = extractvalue { ptr, i20 } %12, 1
  %14 = extractvalue { ptr, i20 } %12, 0
  store i8 %b, ptr %14

  %inc = add nuw nsw i32 %i.08, 1
  %exitcond.not = icmp eq i32 %inc, %c
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret i32 0
}

; ENABLED-AIE-AA-DIS-LABEL: Function: broke_counter_chain
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %ptr_load, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %3, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %3, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %7, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %7, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %3, i8* %7
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %10, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %10, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %10, i8* %3
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %10, i8* %7
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %14, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %14, i8* %3
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %7
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %10, i8* %14

; DISABLED-AIE-AA-DES-LABEL: Function: broke_counter_chain
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %ptr_load, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %7, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %7, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %7
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %3
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %7
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %3
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %7
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %14

; Function Attrs: nofree norecurse nounwind mustprogress
define dso_local i32 @broke_counter_chain(ptr %a, i8 %b, i32 %c) local_unnamed_addr #0 {
entry:
  br label %for.body

for.body:                                         ; preds = %entry, %for.body
  %counter_load = phi i20 [ %9, %for.body ], [ 0, %entry ]
  %counter_store = phi i20 [ %13, %for.body ], [ 0, %entry ]
  %i.08 = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %ptr_load = phi ptr [ %10, %for.body ], [ %a, %entry ]
  %ptr_store = phi ptr [ %14, %for.body ], [ %a, %entry ]
  load i8, ptr %ptr_load
  store i8 %b, ptr %ptr_store

  %1 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %ptr_load, i20 0, i20 1, i20 2, i20 %counter_load)
  %2 = extractvalue { ptr, i20 } %1, 1
  %3 = extractvalue { ptr, i20 } %1, 0
  load i8, ptr %3
  
  %5 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %ptr_store, i20 0, i20 1, i20 2, i20 %counter_store)
  %6 = extractvalue { ptr, i20 } %5, 1
  %7 = extractvalue { ptr, i20 } %5, 0
  store i8 %b, ptr %7

  ; here we break the counter chain.
  %8 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %3, i20 0, i20 1, i20 2, i20 2)
  %9 = extractvalue { ptr, i20 } %8, 1
  %10 = extractvalue { ptr, i20 } %8, 0
  load i8, ptr %10

  %12 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %7, i20 0, i20 1, i20 2, i20 %6)
  %13 = extractvalue { ptr, i20 } %12, 1
  %14 = extractvalue { ptr, i20 } %12, 0
  store i8 %b, ptr %14

  %inc = add nuw nsw i32 %i.08, 1
  %exitcond.not = icmp eq i32 %inc, %c
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret i32 0
}

; ENABLED-AIE-AA-DIS-LABEL: Function: diff_initial_counters
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %ptr_load, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %3, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %3, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %7, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %7, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %3, i8* %7
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %10, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %3
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %10, i8* %7
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %14, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %14, i8* %3
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %7
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %10, i8* %14

; DISABLED-AIE-AA-DES-LABEL: Function: diff_initial_counters
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %ptr_load, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %7, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %7, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %7
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %3
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %7
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %3
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %7
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %14

; Function Attrs: nofree norecurse nounwind mustprogress
define dso_local i32 @diff_initial_counters(ptr %a, i8 %b, i32 %c) local_unnamed_addr #0 {
entry:
  br label %for.body

for.body:                                         ; preds = %entry, %for.body
  %counter_load = phi i20 [ %9, %for.body ], [ 0, %entry ]
  %counter_store = phi i20 [ %13, %for.body ], [ 2, %entry ] ; diff. counter initial value.
  %i.08 = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %ptr_load = phi ptr [ %10, %for.body ], [ %a, %entry ]
  %ptr_store = phi ptr [ %14, %for.body ], [ %a, %entry ]
  load i8, ptr %ptr_load
  store i8 %b, ptr %ptr_store

  %1 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %ptr_load, i20 0, i20 1, i20 2, i20 %counter_load)
  %2 = extractvalue { ptr, i20 } %1, 1
  %3 = extractvalue { ptr, i20 } %1, 0
  load i8, ptr %3
  
  %5 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %ptr_store, i20 0, i20 1, i20 2, i20 %counter_store)
  %6 = extractvalue { ptr, i20 } %5, 1
  %7 = extractvalue { ptr, i20 } %5, 0
  store i8 %b, ptr %7

  %8 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %3, i20 0, i20 1, i20 2, i20 %2)
  %9 = extractvalue { ptr, i20 } %8, 1
  %10 = extractvalue { ptr, i20 } %8, 0
  load i8, ptr %10

  %12 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %7, i20 0, i20 1, i20 2, i20 %6)
  %13 = extractvalue { ptr, i20 } %12, 1
  %14 = extractvalue { ptr, i20 } %12, 0
  store i8 %b, ptr %14

  %inc = add nuw nsw i32 %i.08, 1
  %exitcond.not = icmp eq i32 %inc, %c
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret i32 0
}

; ENABLED-AIE-AA-DIS-LABEL: Function: with_addrspace_cast
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %ptr_load, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %3, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %3, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %ptr_load, i8 addrspace(3)* %ptrcast
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %ptr_store, i8 addrspace(3)* %ptrcast 
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %3, i8 addrspace(3)* %ptrcast
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %3
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8 addrspace(3)* %ptrcast 
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %ptr_load
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %ptr_store
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %3
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8 addrspace(3)* %ptrcast 
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %10, i8* %14

; DISABLED-AIE-AA-DES-LABEL: Function: with_addrspace_cast
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %ptr_load, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %ptr_load, i8 addrspace(3)* %ptrcast
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %ptr_store, i8 addrspace(3)* %ptrcast
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8 addrspace(3)* %ptrcast
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %3
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8 addrspace(3)* %ptrcast
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %ptr_load
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %ptr_store
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %3
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8 addrspace(3)* %ptrcast
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %14

; Function Attrs: nofree norecurse nounwind mustprogress
define dso_local i32 @with_addrspace_cast(ptr %a, i8 %b, i32 %c) local_unnamed_addr #0 {
entry:
  br label %for.body

for.body:                                         ; preds = %entry, %for.body
  %counter_load = phi i20 [ %9, %for.body ], [ 0, %entry ]
  %counter_store = phi i20 [ %13, %for.body ], [ 0, %entry ]
  %i.08 = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %ptr_load = phi ptr [ %10, %for.body ], [ %a, %entry ]
  %ptr_store = phi ptr [ %14, %for.body ], [ %a, %entry ]
  load i8, ptr %ptr_load
  store i8 %b, ptr %ptr_store

  %1 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %ptr_load, i20 0, i20 1, i20 2, i20 %counter_load)
  %2 = extractvalue { ptr, i20 } %1, 1
  %3 = extractvalue { ptr, i20 } %1, 0
  load i8, ptr %3
  
  %5 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %ptr_store, i20 0, i20 1, i20 2, i20 %counter_store)
  %6 = extractvalue { ptr, i20 } %5, 1
  %7 = extractvalue { ptr, i20 } %5, 0

  %ptrcast = addrspacecast ptr %7 to ptr addrspace(3)

  store i8 %b, ptr addrspace(3) %ptrcast

  %8 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %3, i20 0, i20 1, i20 2, i20 %2)
  %9 = extractvalue { ptr, i20 } %8, 1
  %10 = extractvalue { ptr, i20 } %8, 0
  load i8, ptr %10

  %12 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %7, i20 0, i20 1, i20 2, i20 %6)
  %13 = extractvalue { ptr, i20 } %12, 1
  %14 = extractvalue { ptr, i20 } %12, 0
  store i8 %b, ptr %14

  %inc = add nuw nsw i32 %i.08, 1
  %exitcond.not = icmp eq i32 %inc, %c
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret i32 0
}

; ENABLED-AIE-AA-DIS-LABEL: Function: with_addrspace_cast_phi
; ENABLED-AIE-AA-DIS:  MayAlias:    i8* %first_load_ptr, i8* %first_store_ptr 
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %3, i8* %first_load_ptr
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %3, i8* %first_store_ptr
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %7, i8* %first_load_ptr
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %7, i8* %first_store_ptr
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %3, i8* %7
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %first_load_ptr
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %first_store_ptr
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %3
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %10, i8* %7
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %first_load_ptr
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %first_store_ptr
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %3
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %14, i8* %7
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %10, i8* %14
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %first_load_ptr, i8 addrspace(3)* %last_load_ptr
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %first_store_ptr, i8 addrspace(3)* %last_load_ptr
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %3, i8 addrspace(3)* %last_load_ptr
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %7, i8 addrspace(3)* %last_load_ptr
; ENABLED-AIE-AA-DIS:  MustAlias:    i8* %10, i8 addrspace(3)* %last_load_ptr
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %14, i8 addrspace(3)* %last_load_ptr
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %first_load_ptr, i8 addrspace(3)* %last_store_ptr
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %first_store_ptr, i8 addrspace(3)* %last_store_ptr
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %3, i8 addrspace(3)* %last_store_ptr
; ENABLED-AIE-AA-DIS:  NoAlias:      i8* %7, i8 addrspace(3)* %last_store_ptr
; ENABLED-AIE-AA-DIS:  MayAlias:     i8* %10, i8 addrspace(3)* %last_store_ptr
; ENABLED-AIE-AA-DIS:  MustAlias:    i8* %14, i8 addrspace(3)* %last_store_ptr
; ENABLED-AIE-AA-DIS:  MayAlias:     i8 addrspace(3)* %last_load_ptr, i8 addrspace(3)* %last_store_ptr

; DISABLED-AIE-AA-DES-LABEL: Function: with_addrspace_cast_phi
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %first_load_ptr, i8* %first_store_ptr
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %first_load_ptr
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %first_store_ptr
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %7, i8* %first_load_ptr
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %7, i8* %first_store_ptr
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %3, i8* %7
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %first_load_ptr
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %first_store_ptr
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %3
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %7
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %first_load_ptr
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %first_store_ptr
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %3
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %14, i8* %7
; DISABLED-AIE-AA-DES:  MayAlias:      i8* %10, i8* %14
; DISABLED-AIE-AA-DIS:  MayAlias:      i8* %first_load_ptr, i8 addrspace(3)* %last_load_ptr
; DISABLED-AIE-AA-DIS:  MayAlias:      i8* %first_store_ptr, i8 addrspace(3)* %last_load_ptr
; DISABLED-AIE-AA-DIS:  MayAlias:      i8* %3, i8 addrspace(3)* %last_load_ptr
; DISABLED-AIE-AA-DIS:  MayAlias:      i8* %7, i8 addrspace(3)* %last_load_ptr
; DISABLED-AIE-AA-DIS:  MustAlias:     i8* %10, i8 addrspace(3)* %last_load_ptr
; DISABLED-AIE-AA-DIS:  MayAlias:      i8* %14, i8 addrspace(3)* %last_load_ptr
; DISABLED-AIE-AA-DIS:  MayAlias:      i8* %first_load_ptr, i8 addrspace(3)* %last_store_ptr
; DISABLED-AIE-AA-DIS:  MayAlias:      i8* %first_store_ptr, i8 addrspace(3)* %last_store_ptr
; DISABLED-AIE-AA-DIS:  MayAlias:      i8* %3, i8 addrspace(3)* %last_store_ptr
; DISABLED-AIE-AA-DIS:  MayAlias:      i8* %7, i8 addrspace(3)* %last_store_ptr
; DISABLED-AIE-AA-DIS:  MayAlias:      i8* %10, i8 addrspace(3)* %last_store_ptr
; DISABLED-AIE-AA-DIS:  MustAlias:     i8* %14, i8 addrspace(3)* %last_store_ptr
; DISABLED-AIE-AA-DIS:  MayAlias:      i8 addrspace(3)* %last_load_ptr, i8 addrspace(3)* %last_store_ptr

; Function Attrs: nofree norecurse nounwind mustprogress
define dso_local i32 @with_addrspace_cast_phi(ptr addrspace(3) %a, i8 %b, i32 %c) local_unnamed_addr #0 {
entry:
  br label %for.body

for.body:                                         ; preds = %entry, %for.body
  %counter_load = phi i20 [ %9, %for.body ], [ 0, %entry ]
  %counter_store = phi i20 [ %13, %for.body ], [ 0, %entry ]
  %i.08 = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %ptr_load = phi ptr addrspace(3) [ %last_load_ptr, %for.body ], [ %a, %entry ]
  %ptr_store = phi ptr addrspace(3) [ %last_store_ptr, %for.body ], [ %a, %entry ]
  
  %first_load_ptr = addrspacecast ptr addrspace(3) %ptr_load to ptr
  %first_store_ptr = addrspacecast ptr addrspace(3) %ptr_store to ptr

  load i8, ptr %first_load_ptr
  store i8 %b, ptr %first_store_ptr

  %1 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %first_load_ptr, i20 0, i20 1, i20 2, i20 %counter_load)
  %2 = extractvalue { ptr, i20 } %1, 1
  %3 = extractvalue { ptr, i20 } %1, 0
  load i8, ptr %3
  
  %5 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %first_store_ptr, i20 0, i20 1, i20 2, i20 %counter_store)
  %6 = extractvalue { ptr, i20 } %5, 1
  %7 = extractvalue { ptr, i20 } %5, 0
  store i8 %b, ptr %7

  %8 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %3, i20 0, i20 1, i20 2, i20 %2)
  %9 = extractvalue { ptr, i20 } %8, 1
  %10 = extractvalue { ptr, i20 } %8, 0
  load i8, ptr %10

  %12 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %7, i20 0, i20 1, i20 2, i20 %6)
  %13 = extractvalue { ptr, i20 } %12, 1
  %14 = extractvalue { ptr, i20 } %12, 0
  store i8 %b, ptr %14

  %last_load_ptr = addrspacecast ptr %10 to ptr addrspace(3)
  %last_store_ptr = addrspacecast ptr %14 to ptr addrspace(3)

  load i8, ptr addrspace(3) %last_load_ptr
  store i8 %b, ptr addrspace(3) %last_store_ptr

  %inc = add nuw nsw i32 %i.08, 1
  %exitcond.not = icmp eq i32 %inc, %c
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret i32 0
}
