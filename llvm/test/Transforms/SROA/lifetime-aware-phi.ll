; RUN: opt < %s -passes=sroa -S | FileCheck %s

; SROA should use lifetime intrinsics to avoid creating recurrent PHI nodes
; when promoting struct allocas.
;
; The alloca is reused across loop iterations. Without lifetime markers, SROA
; must conservatively assume the i32 field retains its value on the skip path,
; so it inserts a back-edge PHI at the loop header carrying the previous
; iteration's value.
;
; With lifetime.start at the top of the loop, the alloca's content is
; logically undefined at that point each iteration. SROA should propagate this
; onto each slice alloca: PromoteMemToReg can then treat lifetime.start as an
; implicit "store undef", breaking the back-edge dependence and eliminating
; the recurrent PHI.
;
; Currently SROA ignores lifetime markers for this purpose, so both functions
; produce a recurrent PHI. This test documents the desired behaviour after a fix.

%struct.S = type { i8, i32, i8 }

declare void @use(i32)
declare void @llvm.lifetime.start.p0(i64, ptr nocapture)
declare void @llvm.lifetime.end.p0(i64, ptr nocapture)

; Without lifetime markers a recurrent PHI at the loop header is unavoidable.
define void @without_lifetime(i32 %val, i1 %c1, i1 %c2) {
; CHECK-LABEL: @without_lifetime(
; CHECK:       loop:
; CHECK-NEXT:    %{{.*}} = phi i32 [ undef, %entry ], [ %{{.*}}, %cleanup ]
entry:
  %s = alloca %struct.S, align 4
  br label %loop

loop:
  br i1 %c1, label %init, label %skip

init:
  %gep0 = getelementptr inbounds %struct.S, ptr %s, i32 0, i32 0
  store i8 65, ptr %gep0, align 4
  %gep1 = getelementptr inbounds %struct.S, ptr %s, i32 0, i32 1
  store i32 %val, ptr %gep1, align 4
  %gep2 = getelementptr inbounds %struct.S, ptr %s, i32 0, i32 2
  store i8 90, ptr %gep2, align 4
  br label %read

skip:
  br label %read

read:
  %gep1r = getelementptr inbounds %struct.S, ptr %s, i32 0, i32 1
  %v = load i32, ptr %gep1r, align 4
  call void @use(i32 %v)
  br label %cleanup

cleanup:
  br i1 %c2, label %loop, label %exit

exit:
  ret void
}

; With lifetime markers the back-edge PHI is unnecessary: lifetime.start
; at the top of the loop makes the alloca's content undef at that point,
; so the skip path should yield undef rather than carrying a stale value.
define void @with_lifetime(i32 %val, i1 %c1, i1 %c2) {
; CHECK-LABEL: @with_lifetime(
; CHECK:       loop:
; No recurrent PHI should be created at the loop header.
; CHECK-NOT:     phi
; CHECK:         br i1 %c1
; The only PHI is at the read block, merging %val (init) with undef (skip).
; CHECK:       read:
; CHECK-NEXT:    %{{.*}} = phi i32 [ %val, %init ], [ undef, %skip ]
entry:
  %s = alloca %struct.S, align 4
  br label %loop

loop:
  call void @llvm.lifetime.start.p0(i64 12, ptr %s)
  br i1 %c1, label %init, label %skip

init:
  %gep0 = getelementptr inbounds %struct.S, ptr %s, i32 0, i32 0
  store i8 65, ptr %gep0, align 4
  %gep1 = getelementptr inbounds %struct.S, ptr %s, i32 0, i32 1
  store i32 %val, ptr %gep1, align 4
  %gep2 = getelementptr inbounds %struct.S, ptr %s, i32 0, i32 2
  store i8 90, ptr %gep2, align 4
  br label %read

skip:
  br label %read

read:
  %gep1r = getelementptr inbounds %struct.S, ptr %s, i32 0, i32 1
  %v = load i32, ptr %gep1r, align 4
  call void @use(i32 %v)
  br label %cleanup

cleanup:
  call void @llvm.lifetime.end.p0(i64 12, ptr %s)
  br i1 %c2, label %loop, label %exit

exit:
  ret void
}

; lifetime.end shortens live ranges: after lifetime.end the value is undef,
; so a PHI at the loop header carrying the value through the back-edge from
; the block containing lifetime.end is unnecessary.
define void @with_lifetime_end(i32 %val, i1 %c1, i1 %c2) {
; CHECK-LABEL: @with_lifetime_end(
; CHECK:       loop:
; No recurrent PHI at the loop header.
; CHECK-NOT:     phi
; CHECK:         br i1 %c1
; CHECK:       read:
; CHECK-NEXT:    %{{.*}} = phi i32 [ %val, %init ], [ undef, %skip ]
entry:
  %s = alloca %struct.S, align 4
  br label %loop

loop:
  call void @llvm.lifetime.start.p0(i64 12, ptr %s)
  br i1 %c1, label %init, label %skip

init:
  %gep0 = getelementptr inbounds %struct.S, ptr %s, i32 0, i32 0
  store i8 65, ptr %gep0, align 4
  %gep1 = getelementptr inbounds %struct.S, ptr %s, i32 0, i32 1
  store i32 %val, ptr %gep1, align 4
  %gep2 = getelementptr inbounds %struct.S, ptr %s, i32 0, i32 2
  store i8 90, ptr %gep2, align 4
  br label %read

skip:
  br label %read

read:
  %gep1r = getelementptr inbounds %struct.S, ptr %s, i32 0, i32 1
  %v = load i32, ptr %gep1r, align 4
  call void @use(i32 %v)
  call void @llvm.lifetime.end.p0(i64 12, ptr %s)
  br i1 %c2, label %loop, label %exit

exit:
  ret void
}
