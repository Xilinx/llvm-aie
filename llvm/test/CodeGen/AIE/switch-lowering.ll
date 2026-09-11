; Tests for the AIE target-specific switch-to-OR-of-icmp lowering pass
; (AIESwitchLowering), scheduled in addCodeGenPrepare for AIE2 and later.
;
; The generic CodeGenPrepare pass (which our pass runs after) would otherwise
; fold/DCE these small examples, so we disable it with -disable-cgp to observe
; our pass in isolation.
;
; RUN: llc -mtriple=aie2   -disable-cgp -stop-after=aie-switch-lowering -o - %s | FileCheck %s
; RUN: llc -mtriple=aie2p  -disable-cgp -stop-after=aie-switch-lowering -o - %s | FileCheck %s
; RUN: llc -mtriple=aie2ps -disable-cgp -stop-after=aie-switch-lowering -o - %s | FileCheck %s
;
; Negative: with the pass disabled the switch must survive unchanged.
; RUN: llc -mtriple=aie2 -disable-cgp -aie-switch-or-chain=false \
; RUN:   -stop-after=aie-switch-lowering -o - %s | FileCheck %s --check-prefix=DISABLED

declare void @a()
declare void @b()
declare void @c()

;===----------------------------------------------------------------------===;
; POSITIVE: two scattered case values reaching the same label collapse into a
; single OR-of-icmp + one conditional branch.
;===----------------------------------------------------------------------===;
; CHECK-LABEL: @two_values_one_dest(
; CHECK-NOT:     switch
; CHECK:         %[[E0:.*]] = icmp eq i32 %x, 0
; CHECK-NEXT:    %[[E1:.*]] = icmp eq i32 %x, 2
; CHECK-NEXT:    %[[OR:.*]] = or i1 %[[E0]], %[[E1]]
; CHECK-NEXT:    br i1 %[[OR]], label %then, label %else
;
; DISABLED-LABEL: @two_values_one_dest(
; DISABLED:        switch i32 %x
define void @two_values_one_dest(i32 %x) {
entry:
  switch i32 %x, label %else [
    i32 0, label %then
    i32 2, label %then
  ]
then:
  tail call void @a()
  ret void
else:
  tail call void @b()
  ret void
}

;===----------------------------------------------------------------------===;
; POSITIVE: three scattered values, one destination.
;===----------------------------------------------------------------------===;
; CHECK-LABEL: @three_values_one_dest(
; CHECK-NOT:     switch
; CHECK:         %[[E0:.*]] = icmp eq i32 %x, 1
; CHECK-NEXT:    %[[E1:.*]] = icmp eq i32 %x, 5
; CHECK-NEXT:    %[[O1:.*]] = or i1 %[[E0]], %[[E1]]
; CHECK-NEXT:    %[[E2:.*]] = icmp eq i32 %x, 9
; CHECK-NEXT:    %[[O2:.*]] = or i1 %[[O1]], %[[E2]]
; CHECK-NEXT:    br i1 %[[O2]], label %then, label %else
define void @three_values_one_dest(i32 %x) {
entry:
  switch i32 %x, label %else [
    i32 1, label %then
    i32 5, label %then
    i32 9, label %then
  ]
then:
  call void @a()
  ret void
else:
  call void @b()
  ret void
}

;===----------------------------------------------------------------------===;
; POSITIVE: two destinations, each reached by several values. We emit one
; conditional branch per destination, chaining the false edges.
;===----------------------------------------------------------------------===;
; CHECK-LABEL: @two_dests(
; CHECK-NOT:     switch
; CHECK:       entry:
; CHECK:         %[[A0:.*]] = icmp eq i32 %x, 0
; CHECK-NEXT:    %[[A1:.*]] = icmp eq i32 %x, 4
; CHECK-NEXT:    %[[AO:.*]] = or i1 %[[A0]], %[[A1]]
; CHECK-NEXT:    br i1 %[[AO]], label %da, label %[[NEXT:.*]]
; CHECK:       [[NEXT]]:
; CHECK-NEXT:    %[[B0:.*]] = icmp eq i32 %x, 1
; CHECK-NEXT:    %[[B1:.*]] = icmp eq i32 %x, 3
; CHECK-NEXT:    %[[BO:.*]] = or i1 %[[B0]], %[[B1]]
; CHECK-NEXT:    br i1 %[[BO]], label %db, label %else
define void @two_dests(i32 %x) {
entry:
  switch i32 %x, label %else [
    i32 0, label %da
    i32 4, label %da
    i32 1, label %db
    i32 3, label %db
  ]
da:
  call void @a()
  ret void
db:
  call void @b()
  ret void
else:
  call void @c()
  ret void
}

;===----------------------------------------------------------------------===;
; POSITIVE + PHIs: complex control flow. The switch block %sw is reached from
; %entry; %then is reached both by two switch cases (0 and 2) and by the
; non-switch predecessor %pre.bb, so its PHI legitimately has two %sw entries
; plus a %pre.bb entry. After lowering:
;   - the two duplicate %sw entries collapse into a single entry from the first
;     comparison block (%sw itself), and the %pre.bb entry is preserved;
;   - the %other and default(%else) PHIs are rewired to the second comparison
;     block (%sw.or.next).
; The incoming *values* must be preserved. The PHI results are returned so they
; are not DCE'd.
; CHECK-LABEL: @phi_in_targets(
; CHECK-NOT:     switch
; CHECK:       sw:
; CHECK:         %[[C0:.*]] = icmp eq i32 %x, 0
; CHECK-NEXT:    %[[C2:.*]] = icmp eq i32 %x, 2
; CHECK-NEXT:    %[[CO:.*]] = or i1 %[[C0]], %[[C2]]
; CHECK-NEXT:    br i1 %[[CO]], label %then, label %[[N:.*]]
; The duplicate %sw entries collapse into one; the %pre.bb entry is preserved.
; CHECK:       then:
; CHECK:         %p1 = phi i32 [ 10, %sw ], [ %pre, %pre.bb ]
; CHECK:       other:
; CHECK:         %p2 = phi i32 [ 20, %[[N]] ]
; CHECK:       [[N]]:
; CHECK-NEXT:    %[[D1:.*]] = icmp eq i32 %x, 1
; CHECK-NEXT:    br i1 %[[D1]], label %other, label %else
; CHECK:       else:
; CHECK:         %p3 = phi i32 [ 30, %[[N]] ]
define i32 @phi_in_targets(i32 %x, i1 %g, i32 %pre) {
entry:
  br i1 %g, label %sw, label %pre.bb
pre.bb:
  br label %then
sw:
  switch i32 %x, label %else [
    i32 0, label %then
    i32 2, label %then
    i32 1, label %other
  ]
then:
  %p1 = phi i32 [ 10, %sw ], [ 10, %sw ], [ %pre, %pre.bb ]
  call void @a()
  ret i32 %p1
other:
  %p2 = phi i32 [ 20, %sw ]
  call void @b()
  ret i32 %p2
else:
  %p3 = phi i32 [ 30, %sw ]
  call void @c()
  ret i32 %p3
}

;===----------------------------------------------------------------------===;
; POSITIVE: a contiguous range (0..3) to one destination is rewritten to a
; single OR chain (one branch).
;===----------------------------------------------------------------------===;
; CHECK-LABEL: @contiguous(
; CHECK-NOT:     switch
; CHECK:         br i1 %{{.*}}, label %then, label %else
define void @contiguous(i32 %x) {
entry:
  switch i32 %x, label %else [
    i32 0, label %then
    i32 1, label %then
    i32 2, label %then
    i32 3, label %then
  ]
then:
  call void @a()
  ret void
else:
  call void @b()
  ret void
}

;===----------------------------------------------------------------------===;
; NEGATIVE: too many cases (> aie-switch-or-chain-max-cases, default 8). The
; switch must be left untouched for the generic lowering. Distinct side effects
; keep the switch from being folded away.
;===----------------------------------------------------------------------===;
; CHECK-LABEL: @too_many_cases(
; CHECK:         switch i32 %x
; CHECK-NOT:     icmp eq i32 %x
define void @too_many_cases(i32 %x) {
entry:
  switch i32 %x, label %else [
    i32 0, label %d0
    i32 1, label %d1
    i32 2, label %d2
    i32 3, label %d3
    i32 4, label %d4
    i32 5, label %d5
    i32 6, label %d6
    i32 7, label %d7
    i32 8, label %d8
  ]
d0:
  call void @a()
  ret void
d1:
  call void @b()
  ret void
d2:
  call void @c()
  ret void
d3:
  call void @a()
  ret void
d4:
  call void @b()
  ret void
d5:
  call void @c()
  ret void
d6:
  call void @a()
  ret void
d7:
  call void @b()
  ret void
d8:
  call void @c()
  ret void
else:
  call void @a()
  ret void
}

;===----------------------------------------------------------------------===;
; NEGATIVE: a case whose destination is also the default block. This is a
; degenerate shape we deliberately do not transform; the switch is preserved.
;===----------------------------------------------------------------------===;
; CHECK-LABEL: @case_is_default(
; CHECK:         switch i32 %x
; CHECK-NOT:     %sw.eq
define void @case_is_default(i32 %x) {
entry:
  switch i32 %x, label %def [
    i32 0, label %then
    i32 2, label %def
  ]
then:
  call void @a()
  ret void
def:
  call void @b()
  ret void
}

;===----------------------------------------------------------------------===;
; NEGATIVE: a switch with no cases (only a default) is left alone.
;===----------------------------------------------------------------------===;
; CHECK-LABEL: @no_cases(
; CHECK:         switch i32 %x
define void @no_cases(i32 %x) {
entry:
  switch i32 %x, label %def [
  ]
def:
  call void @a()
  ret void
}

;===----------------------------------------------------------------------===;
; NEGATIVE: too many distinct destinations (> aie-switch-or-chain-max-dests,
; default 2). Here 3 cases fan out to 3 destinations; the OR-chain would emit
; one branch per destination, which is worse than the generic balanced
; bisection, so we leave the switch untouched.
;===----------------------------------------------------------------------===;
; CHECK-LABEL: @all_distinct_dests(
; CHECK:         switch i32 %x
; CHECK-NOT:     %sw.eq
define void @all_distinct_dests(i32 %x) {
entry:
  switch i32 %x, label %else [
    i32 0, label %d0
    i32 1, label %d1
    i32 2, label %d2
  ]
d0:
  call void @a()
  ret void
d1:
  call void @b()
  ret void
d2:
  call void @c()
  ret void
else:
  call void @a()
  ret void
}

;===----------------------------------------------------------------------===;
; NEGATIVE: "barely shared" -- one destination is shared by two values but the
; switch still fans out to more than aie-switch-or-chain-max-dests (default 2)
; destinations (here 4 values -> 3 destinations). The branch count is driven by
; the number of destinations, not by whether some sharing exists, so this is
; left to the generic bisection.
;===----------------------------------------------------------------------===;
; CHECK-LABEL: @many_dests_one_shared(
; CHECK:         switch i32 %x
; CHECK-NOT:     %sw.eq
define void @many_dests_one_shared(i32 %x) {
entry:
  switch i32 %x, label %else [
    i32 0, label %d0
    i32 4, label %d0
    i32 1, label %d1
    i32 2, label %d2
  ]
d0:
  call void @a()
  ret void
d1:
  call void @b()
  ret void
d2:
  call void @c()
  ret void
else:
  call void @a()
  ret void
}

;===----------------------------------------------------------------------===;
; POSITIVE: non-i32 condition type. The comparisons must use the switch
; condition's type (i8 here).
;===----------------------------------------------------------------------===;
; CHECK-LABEL: @i8_cond(
; CHECK-NOT:     switch
; CHECK:         %[[E0:.*]] = icmp eq i8 %x, 0
; CHECK-NEXT:    %[[E1:.*]] = icmp eq i8 %x, 7
; CHECK-NEXT:    %[[O:.*]] = or i1 %[[E0]], %[[E1]]
; CHECK-NEXT:    br i1 %[[O]], label %then, label %else
define void @i8_cond(i8 %x) {
entry:
  switch i8 %x, label %else [
    i8 0, label %then
    i8 7, label %then
  ]
then:
  call void @a()
  ret void
else:
  call void @b()
  ret void
}

;===----------------------------------------------------------------------===;
; POSITIVE: i64 condition type.
;===----------------------------------------------------------------------===;
; CHECK-LABEL: @i64_cond(
; CHECK-NOT:     switch
; CHECK:         icmp eq i64 %x, 0
; CHECK:         icmp eq i64 %x, 100
define void @i64_cond(i64 %x) {
entry:
  switch i64 %x, label %else [
    i64 0, label %then
    i64 100, label %then
  ]
then:
  call void @a()
  ret void
else:
  call void @b()
  ret void
}

;===----------------------------------------------------------------------===;
; POSITIVE: boundary at exactly aie-switch-or-chain-max-cases (8). All 8 values
; share one destination, so this transforms (one branch). (@too_many_cases with
; 9 cases is the negative side of the boundary.)
;===----------------------------------------------------------------------===;
; CHECK-LABEL: @at_max_cases(
; CHECK-NOT:     switch
; CHECK:         br i1 %{{.*}}, label %then, label %else
define void @at_max_cases(i32 %x) {
entry:
  switch i32 %x, label %else [
    i32 0, label %then
    i32 2, label %then
    i32 4, label %then
    i32 6, label %then
    i32 8, label %then
    i32 10, label %then
    i32 12, label %then
    i32 14, label %then
  ]
then:
  call void @a()
  ret void
else:
  call void @b()
  ret void
}

;===----------------------------------------------------------------------===;
; POSITIVE + PHI in the default block with mixed predecessors: %def is reached
; both by the switch's default edge and by an unrelated predecessor %pre.bb.
; After lowering, the switch-edge PHI entry must be rewired to the last
; comparison block while the %pre.bb entry is preserved.
;===----------------------------------------------------------------------===;
; CHECK-LABEL: @phi_in_default(
; CHECK-NOT:     switch
; CHECK:       sw:
; CHECK:         br i1 %{{.*}}, label %then, label %[[N:.*]]
; CHECK:       [[N]]:
; CHECK:         br i1 %{{.*}}, label %other, label %def
; CHECK:       def:
; CHECK:         %pd = phi i32 [ 30, %[[N]] ], [ %pre, %pre.bb ]
define i32 @phi_in_default(i32 %x, i1 %g, i32 %pre) {
entry:
  br i1 %g, label %sw, label %pre.bb
pre.bb:
  br label %def
sw:
  switch i32 %x, label %def [
    i32 0, label %then
    i32 2, label %then
    i32 1, label %other
  ]
then:
  call void @a()
  ret i32 1
other:
  call void @b()
  ret i32 2
def:
  %pd = phi i32 [ 30, %sw ], [ %pre, %pre.bb ]
  call void @c()
  ret i32 %pd
}

;===----------------------------------------------------------------------===;
; POSITIVE: switch inside a loop with a back-edge. The comparison blocks must
; branch back into the loop; the loop-header PHI (fed by the latch) is on an
; unrelated edge and must be preserved.
;===----------------------------------------------------------------------===;
; CHECK-LABEL: @loop_switch(
; CHECK-NOT:     switch
; CHECK:       loop:
; CHECK:         %iv = phi i32 [ 0, %entry ], [ %iv.next, %latch ]
; CHECK:         %[[E0:.*]] = icmp eq i32 %iv, 1
; CHECK-NEXT:    %[[E1:.*]] = icmp eq i32 %iv, 3
; CHECK-NEXT:    %[[O:.*]] = or i1 %[[E0]], %[[E1]]
; CHECK-NEXT:    br i1 %[[O]], label %body, label %latch
define void @loop_switch(i32 %n) {
entry:
  br label %loop
loop:
  %iv = phi i32 [ 0, %entry ], [ %iv.next, %latch ]
  switch i32 %iv, label %latch [
    i32 1, label %body
    i32 3, label %body
  ]
body:
  call void @a()
  br label %latch
latch:
  %iv.next = add i32 %iv, 1
  %c = icmp slt i32 %iv.next, %n
  br i1 %c, label %loop, label %exit
exit:
  ret void
}

;===----------------------------------------------------------------------===;
; POSITIVE: branch_weights metadata is propagated. The taken edge for %then
; gets the summed weight of its cases (10+30=40); the fall-through gets the
; remaining case weights plus the default (20 + 5 = 25).
;===----------------------------------------------------------------------===;
; CHECK-LABEL: @prof_weights(
; CHECK-NOT:     switch
; CHECK:       entry:
; CHECK:         br i1 %{{.*}}, label %then, label %[[N:.*]], !prof [[W0:![0-9]+]]
; CHECK:       [[N]]:
; CHECK:         br i1 %{{.*}}, label %other, label %else
; CHECK-DAG:   [[W0]] = !{!"branch_weights", i32 40, i32 25}
define void @prof_weights(i32 %x) {
entry:
  switch i32 %x, label %else [
    i32 0, label %then
    i32 1, label %other
    i32 2, label %then
  ], !prof !0
then:
  call void @a()
  ret void
other:
  call void @b()
  ret void
else:
  call void @c()
  ret void
}

!0 = !{!"branch_weights", i32 5, i32 10, i32 20, i32 30}
