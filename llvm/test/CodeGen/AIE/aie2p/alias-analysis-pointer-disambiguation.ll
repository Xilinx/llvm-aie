;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
; RUN: opt -mtriple=aie2p -passes=aa-eval --aie-alias-analysis-disambiguation=true -disable-output < %s 2>&1 | FileCheck %s --check-prefix=AIE-AA-DIS-ENABLE
; RUN: opt -mtriple=aie2p -passes=aa-eval --aie-alias-analysis-disambiguation=false -disable-output \
; RUN:     < %s 2>&1 | FileCheck %s --check-prefix=AIE-AA-DIS-DISABLE


; AIE-AA-DIS-ENABLE: ===== Alias Analysis Evaluator Report =====
; AIE-AA-DIS-ENABLE-NEXT:   133 Total Alias Queries Performed
; AIE-AA-DIS-ENABLE-NEXT:   71 no alias responses (53.3%)
; AIE-AA-DIS-ENABLE-NEXT:   60 may alias responses (45.1%)
; AIE-AA-DIS-ENABLE-NEXT:   0 partial alias responses (0.0%)
; AIE-AA-DIS-ENABLE-NEXT:   2 must alias responses (1.5%)
; AIE-AA-DIS-ENABLE-NEXT:   Alias Analysis Evaluator Pointer Alias Summary: 53%/45%/0%/1%
; AIE-AA-DIS-ENABLE-NEXT:   296 Total ModRef Queries Performed
; AIE-AA-DIS-ENABLE-NEXT:   296 no mod/ref responses (100.0%)
; AIE-AA-DIS-ENABLE-NEXT:   0 mod responses (0.0%)
; AIE-AA-DIS-ENABLE-NEXT:   0 ref responses (0.0%)
; AIE-AA-DIS-ENABLE-NEXT:   0 mod & ref responses (0.0%)
; AIE-AA-DIS-ENABLE-NEXT:   Alias Analysis Evaluator Mod/Ref Summary: 100%/0%/0%/0%

; AIE-AA-DIS-DISABLE: ===== Alias Analysis Evaluator Report =====
; AIE-AA-DIS-DISABLE-NEXT:   133 Total Alias Queries Performed
; AIE-AA-DIS-DISABLE-NEXT:   0 no alias responses (0.0%)
; AIE-AA-DIS-DISABLE-NEXT:   131 may alias responses (98.4%)
; AIE-AA-DIS-DISABLE-NEXT:   0 partial alias responses (0.0%)
; AIE-AA-DIS-DISABLE-NEXT:   2 must alias responses (1.5%)
; AIE-AA-DIS-DISABLE-NEXT:   Alias Analysis Evaluator Pointer Alias Summary: 0%/98%/0%/1%
; AIE-AA-DIS-DISABLE-NEXT:   296 Total ModRef Queries Performed
; AIE-AA-DIS-DISABLE-NEXT:   296 no mod/ref responses (100.0%)
; AIE-AA-DIS-DISABLE-NEXT:   0 mod responses (0.0%)
; AIE-AA-DIS-DISABLE-NEXT:   0 ref responses (0.0%)
; AIE-AA-DIS-DISABLE-NEXT:   0 mod & ref responses (0.0%)
; AIE-AA-DIS-DISABLE-NEXT:   Alias Analysis Evaluator Mod/Ref Summary: 100%/0%/0%/0%

declare { ptr, i20 } @llvm.aie2p.add.2d(ptr, i20, i20, i20, i20)

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

  %1 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %ptr_load, i20 0, i20 1, i20 2, i20 %counter_load)
  %2 = extractvalue { ptr, i20 } %1, 1
  %3 = extractvalue { ptr, i20 } %1, 0
  load i8, ptr %3
  
  %5 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %ptr_store, i20 0, i20 1, i20 2, i20 %counter_store)
  %6 = extractvalue { ptr, i20 } %5, 1
  %7 = extractvalue { ptr, i20 } %5, 0
  store i8 %b, ptr %7

  %8 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %3, i20 0, i20 1, i20 2, i20 %2)
  %9 = extractvalue { ptr, i20 } %8, 1
  %10 = extractvalue { ptr, i20 } %8, 0
  load i8, ptr %10

  %12 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %7, i20 0, i20 1, i20 2, i20 %6)
  %13 = extractvalue { ptr, i20 } %12, 1
  %14 = extractvalue { ptr, i20 } %12, 0
  store i8 %b, ptr %14

  %inc = add nuw nsw i32 %i.08, 1
  %exitcond.not = icmp eq i32 %inc, %c
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret i32 0
}


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

  %1 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %ptr_load, i20 0, i20 1, i20 2, i20 %counter_load)
  %2 = extractvalue { ptr, i20 } %1, 1
  %3 = extractvalue { ptr, i20 } %1, 0
  load i8, ptr %3
  
  ; here we break the iteration, we cannot state NoAlias.
  %5 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %ptr_store, i20 16, i20 1, i20 2, i20 %counter_store)
  %6 = extractvalue { ptr, i20 } %5, 1
  %7 = extractvalue { ptr, i20 } %5, 0
  store i8 %b, ptr %7

  %8 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %3, i20 0, i20 1, i20 2, i20 %2)
  %9 = extractvalue { ptr, i20 } %8, 1
  %10 = extractvalue { ptr, i20 } %8, 0
  load i8, ptr %10

  %12 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %7, i20 0, i20 1, i20 2, i20 %6)
  %13 = extractvalue { ptr, i20 } %12, 1
  %14 = extractvalue { ptr, i20 } %12, 0
  store i8 %b, ptr %14

  %inc = add nuw nsw i32 %i.08, 1
  %exitcond.not = icmp eq i32 %inc, %c
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret i32 0
}


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

  %1 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %ptr_load, i20 0, i20 1, i20 2, i20 %counter_load)
  %2 = extractvalue { ptr, i20 } %1, 1
  %3 = extractvalue { ptr, i20 } %1, 0
  load i8, ptr %3
  
  %5 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %ptr_store, i20 0, i20 1, i20 2, i20 %counter_store)
  %6 = extractvalue { ptr, i20 } %5, 1
  %7 = extractvalue { ptr, i20 } %5, 0

  ; GEP with offset zero is ok.
  %gep = getelementptr inbounds i8, ptr %7, i64 0

  store i8 %b, ptr %gep

  %8 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %3, i20 0, i20 1, i20 2, i20 %2)
  %9 = extractvalue { ptr, i20 } %8, 1
  %10 = extractvalue { ptr, i20 } %8, 0
  load i8, ptr %10

  %12 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %gep, i20 0, i20 1, i20 2, i20 %6)
  %13 = extractvalue { ptr, i20 } %12, 1
  %14 = extractvalue { ptr, i20 } %12, 0
  store i8 %b, ptr %14

  %inc = add nuw nsw i32 %i.08, 1
  %exitcond.not = icmp eq i32 %inc, %c
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret i32 0
}


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

  %1 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %ptr_load, i20 0, i20 1, i20 2, i20 %counter_load)
  %2 = extractvalue { ptr, i20 } %1, 1
  %3 = extractvalue { ptr, i20 } %1, 0
  load i8, ptr %3
  
  %5 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %ptr_store, i20 0, i20 1, i20 2, i20 %counter_store)
  %6 = extractvalue { ptr, i20 } %5, 1
  %7 = extractvalue { ptr, i20 } %5, 0

  ; GEP with offset != from zero is NOT ok.
  %gep = getelementptr inbounds i8, ptr %7, i64 2

  store i8 %b, ptr %gep

  %8 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %3, i20 0, i20 1, i20 2, i20 %2)
  %9 = extractvalue { ptr, i20 } %8, 1
  %10 = extractvalue { ptr, i20 } %8, 0
  load i8, ptr %10

  %12 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %gep, i20 0, i20 1, i20 2, i20 %6)
  %13 = extractvalue { ptr, i20 } %12, 1
  %14 = extractvalue { ptr, i20 } %12, 0
  store i8 %b, ptr %14

  %inc = add nuw nsw i32 %i.08, 1
  %exitcond.not = icmp eq i32 %inc, %c
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret i32 0
}


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

  %1 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %ptr_load, i20 0, i20 1, i20 2, i20 %counter_load)
  %2 = extractvalue { ptr, i20 } %1, 1
  %3 = extractvalue { ptr, i20 } %1, 0
  load i8, ptr %3
  
  %5 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %ptr_store, i20 0, i20 1, i20 2, i20 %counter_store)
  %6 = extractvalue { ptr, i20 } %5, 1
  %7 = extractvalue { ptr, i20 } %5, 0
  store i8 %b, ptr %7

  ; here we break the counter chain.
  %8 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %3, i20 0, i20 1, i20 2, i20 2)
  %9 = extractvalue { ptr, i20 } %8, 1
  %10 = extractvalue { ptr, i20 } %8, 0
  load i8, ptr %10

  %12 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %7, i20 0, i20 1, i20 2, i20 %6)
  %13 = extractvalue { ptr, i20 } %12, 1
  %14 = extractvalue { ptr, i20 } %12, 0
  store i8 %b, ptr %14

  %inc = add nuw nsw i32 %i.08, 1
  %exitcond.not = icmp eq i32 %inc, %c
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret i32 0
}

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

  %1 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %ptr_load, i20 0, i20 1, i20 2, i20 %counter_load)
  %2 = extractvalue { ptr, i20 } %1, 1
  %3 = extractvalue { ptr, i20 } %1, 0
  load i8, ptr %3
  
  %5 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %ptr_store, i20 0, i20 1, i20 2, i20 %counter_store)
  %6 = extractvalue { ptr, i20 } %5, 1
  %7 = extractvalue { ptr, i20 } %5, 0
  store i8 %b, ptr %7

  %8 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %3, i20 0, i20 1, i20 2, i20 %2)
  %9 = extractvalue { ptr, i20 } %8, 1
  %10 = extractvalue { ptr, i20 } %8, 0
  load i8, ptr %10

  %12 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %7, i20 0, i20 1, i20 2, i20 %6)
  %13 = extractvalue { ptr, i20 } %12, 1
  %14 = extractvalue { ptr, i20 } %12, 0
  store i8 %b, ptr %14

  %inc = add nuw nsw i32 %i.08, 1
  %exitcond.not = icmp eq i32 %inc, %c
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret i32 0
}


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

  %1 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %ptr_load, i20 0, i20 1, i20 2, i20 %counter_load)
  %2 = extractvalue { ptr, i20 } %1, 1
  %3 = extractvalue { ptr, i20 } %1, 0
  load i8, ptr %3
  
  %5 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %ptr_store, i20 0, i20 1, i20 2, i20 %counter_store)
  %6 = extractvalue { ptr, i20 } %5, 1
  %7 = extractvalue { ptr, i20 } %5, 0

  %ptrcast = addrspacecast ptr %7 to ptr addrspace(3)

  store i8 %b, ptr addrspace(3) %ptrcast

  %8 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %3, i20 0, i20 1, i20 2, i20 %2)
  %9 = extractvalue { ptr, i20 } %8, 1
  %10 = extractvalue { ptr, i20 } %8, 0
  load i8, ptr %10

  %12 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %7, i20 0, i20 1, i20 2, i20 %6)
  %13 = extractvalue { ptr, i20 } %12, 1
  %14 = extractvalue { ptr, i20 } %12, 0
  store i8 %b, ptr %14

  %inc = add nuw nsw i32 %i.08, 1
  %exitcond.not = icmp eq i32 %inc, %c
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret i32 0
}


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

  %1 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %first_load_ptr, i20 0, i20 1, i20 2, i20 %counter_load)
  %2 = extractvalue { ptr, i20 } %1, 1
  %3 = extractvalue { ptr, i20 } %1, 0
  load i8, ptr %3
  
  %5 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %first_store_ptr, i20 0, i20 1, i20 2, i20 %counter_store)
  %6 = extractvalue { ptr, i20 } %5, 1
  %7 = extractvalue { ptr, i20 } %5, 0
  store i8 %b, ptr %7

  %8 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %3, i20 0, i20 1, i20 2, i20 %2)
  %9 = extractvalue { ptr, i20 } %8, 1
  %10 = extractvalue { ptr, i20 } %8, 0
  load i8, ptr %10

  %12 = tail call { ptr, i20 } @llvm.aie2p.add.2d(ptr %7, i20 0, i20 1, i20 2, i20 %6)
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
