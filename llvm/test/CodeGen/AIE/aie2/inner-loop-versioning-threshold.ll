; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; The value of the versioning hint picks who owns the guard threshold.
;
; A hint of 1 defers it: the guard is seeded with the -1 placeholder, and the
; versioned marker asks a later pass to fill it in. No iteration-count range
; goes with it, so the hardware-loop gate does not see a minimum of 1.
;
; A hint of 2 or more states the threshold, and it is authoritative. The guard
; holds that value, the high-trip-count copy declares it as its minimum
; iteration count, and the versioned marker is left off: with nothing to fill
; in, the copy reads as an ordinary loop of known minimum trip count. Both the
; seed and the minimum come from the same validated threshold, so a hint that
; is refused leaves neither behind.
;
; RUN: llc -mtriple=aie2 -stop-after=aie-inner-loop-versioning %s -o - \
; RUN:   | FileCheck %s
; RUN: llc -mtriple=aie2 -aie-inner-loop-versioning-min-itercount=8 \
; RUN:   -stop-after=aie-inner-loop-versioning %s -o - \
; RUN:   | FileCheck %s --check-prefix=OPTION
; RUN: llc -mtriple=aie2 -pass-remarks-missed=aie-inner-loop-versioning \
; RUN:   -stop-after=aie-inner-loop-versioning %s -o /dev/null 2>&1 \
; RUN:   | FileCheck %s --check-prefix=REMARK

; A hint of 1 keeps the placeholder, and the marker asking for it to be filled
; in. No iteration-count range.
; CHECK-LABEL: define void @hint_one_defers
; CHECK: call i32 @llvm.aie2.loop.version.threshold(i32 -1)
; CHECK: loop.lver.high:
; CHECK: br i1 %{{.*}}, !llvm.loop [[DEFER:![0-9]+]]
define void @hint_one_defers(ptr noalias %a, ptr noalias %b, i32 %n) {
entry:
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop ]
  %pa = getelementptr i32, ptr %a, i32 %i
  %x = load i32, ptr %pa, align 4
  %y = mul i32 %x, 1234
  %pb = getelementptr i32, ptr %b, i32 %i
  store i32 %y, ptr %pb, align 4
  %i.next = add i32 %i, 1
  %c = icmp slt i32 %i.next, %n
  br i1 %c, label %loop, label %exit, !llvm.loop !0
exit:
  ret void
}

; A hint of 2 is the smallest statable threshold: restating a minimum of 1 is
; what dropping the range avoids in the first place.
; CHECK-LABEL: define void @stated_threshold_two
; CHECK: call i32 @llvm.aie2.loop.version.threshold(i32 2)
; CHECK: loop.lver.high:
; CHECK: br i1 %{{.*}}, !llvm.loop [[TWO:![0-9]+]]
define void @stated_threshold_two(ptr noalias %a, ptr noalias %b, i32 %n) {
entry:
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop ]
  %pa = getelementptr i32, ptr %a, i32 %i
  %x = load i32, ptr %pa, align 4
  %y = mul i32 %x, 1234
  %pb = getelementptr i32, ptr %b, i32 %i
  store i32 %y, ptr %pb, align 4
  %i.next = add i32 %i, 1
  %c = icmp slt i32 %i.next, %n
  br i1 %c, label %loop, label %exit, !llvm.loop !2
exit:
  ret void
}

; The stated value is what the guard compares against, so the high copy is
; entered only from 4 iterations up, and declares the same 4 as its minimum.
; CHECK-LABEL: define void @stated_threshold_four
; CHECK: %[[THR:.*]] = call i32 @llvm.aie2.loop.version.threshold(i32 4)
; CHECK: %[[LOW:.*]] = icmp ult i32 %{{.*}}, %[[THR]]
; CHECK: br i1 %[[LOW]], label %loop.ph, label %loop.ph.lver.high
; CHECK: loop.lver.high:
; CHECK: br i1 %{{.*}}, !llvm.loop [[FOUR:![0-9]+]]
; CHECK: br i1 %{{.*}}, !llvm.loop [[FOURLOW:![0-9]+]]
define void @stated_threshold_four(ptr noalias %a, ptr noalias %b, i32 %n) {
entry:
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop ]
  %pa = getelementptr i32, ptr %a, i32 %i
  %x = load i32, ptr %pa, align 4
  %y = mul i32 %x, 1234
  %pb = getelementptr i32, ptr %b, i32 %i
  store i32 %y, ptr %pb, align 4
  %i.next = add i32 %i, 1
  %c = icmp slt i32 %i.next, %n
  br i1 %c, label %loop, label %exit, !llvm.loop !4
exit:
  ret void
}

; 100 is the largest statable threshold.
; CHECK-LABEL: define void @stated_threshold_maximum
; CHECK: call i32 @llvm.aie2.loop.version.threshold(i32 100)
; CHECK: loop.lver.high:
; CHECK: br i1 %{{.*}}, !llvm.loop [[MAX:![0-9]+]]
define void @stated_threshold_maximum(ptr noalias %a, ptr noalias %b, i32 %n) {
entry:
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop ]
  %pa = getelementptr i32, ptr %a, i32 %i
  %x = load i32, ptr %pa, align 4
  %y = mul i32 %x, 1234
  %pb = getelementptr i32, ptr %b, i32 %i
  store i32 %y, ptr %pb, align 4
  %i.next = add i32 %i, 1
  %c = icmp slt i32 %i.next, %n
  br i1 %c, label %loop, label %exit, !llvm.loop !6
exit:
  ret void
}

; One past the maximum is refused, and refused as a whole: the loop is still
; versioned, but with the placeholder and the marker that asks for it to be
; filled in, exactly as a hint of 1 would be. Clamping instead would hand back
; a threshold the user did not ask for.
; CHECK-LABEL: define void @stated_threshold_above_maximum
; CHECK: call i32 @llvm.aie2.loop.version.threshold(i32 -1)
; CHECK: loop.lver.high:
; CHECK: br i1 %{{.*}}, !llvm.loop [[OVER:![0-9]+]]
; REMARK: loop versioned without its stated threshold of 101, which exceeds the maximum of 100; the threshold is left to be filled in later
define void @stated_threshold_above_maximum(ptr noalias %a, ptr noalias %b,
                                            i32 %n) {
entry:
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop ]
  %pa = getelementptr i32, ptr %a, i32 %i
  %x = load i32, ptr %pa, align 4
  %y = mul i32 %x, 1234
  %pb = getelementptr i32, ptr %b, i32 %i
  store i32 %y, ptr %pb, align 4
  %i.next = add i32 %i, 1
  %c = icmp slt i32 %i.next, %n
  br i1 %c, label %loop, label %exit, !llvm.loop !8
exit:
  ret void
}

; A hint wider than the guard's i32 is the same refusal, and must not be
; truncated into one: 2^32+2 would otherwise state a plausible threshold of 2.
; CHECK-LABEL: define void @stated_threshold_wider_than_guard
; CHECK: call i32 @llvm.aie2.loop.version.threshold(i32 -1)
; CHECK: loop.lver.high:
; CHECK: br i1 %{{.*}}, !llvm.loop [[WIDE:![0-9]+]]
; REMARK: loop versioned without its stated threshold of 4294967298
define void @stated_threshold_wider_than_guard(ptr noalias %a, ptr noalias %b,
                                               i32 %n) {
entry:
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop ]
  %pa = getelementptr i32, ptr %a, i32 %i
  %x = load i32, ptr %pa, align 4
  %y = mul i32 %x, 1234
  %pb = getelementptr i32, ptr %b, i32 %i
  store i32 %y, ptr %pb, align 4
  %i.next = add i32 %i, 1
  %c = icmp slt i32 %i.next, %n
  br i1 %c, label %loop, label %exit, !llvm.loop !10
exit:
  ret void
}

; The command-line override versions loops carrying no hint at all, so there is
; no stated threshold to read and the placeholder stays. The option's own bound
; is a property of the loop, not a request for a guard threshold.
; OPTION-LABEL: define void @no_hint_declared_range
; OPTION: call i32 @llvm.aie2.loop.version.threshold(i32 -1)
; OPTION: loop.lver.high:
; OPTION: br i1 %{{.*}}, !llvm.loop [[OPTHIGH:![0-9]+]]
; CHECK-LABEL: define void @no_hint_declared_range
; CHECK-NOT: lver
define void @no_hint_declared_range(ptr noalias %a, ptr noalias %b, i32 %n) {
entry:
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop ]
  %pa = getelementptr i32, ptr %a, i32 %i
  %x = load i32, ptr %pa, align 4
  %y = mul i32 %x, 1234
  %pb = getelementptr i32, ptr %b, i32 %i
  store i32 %y, ptr %pb, align 4
  %i.next = add i32 %i, 1
  %c = icmp slt i32 %i.next, %n
  br i1 %c, label %loop, label %exit, !llvm.loop !12
exit:
  ret void
}

; A stated threshold leaves the high copy with a minimum and nothing else; a
; deferred one with the versioned marker and nothing else. The two are never
; both present, which is what keeps a filled-in guard from contradicting a
; minimum that was already declared.
; CHECK-DAG: [[DEFER]] = distinct !{[[DEFER]], [[MARK1:![0-9]+]]}
; CHECK-DAG: [[MARK1]] = !{!"llvm.loop.hint.aie-loop-versioned", i32 1}
; CHECK-DAG: [[TWO]] = distinct !{[[TWO]], [[RANGE2:![0-9]+]]}
; CHECK-DAG: [[RANGE2]] = !{!"llvm.loop.itercount.range", i32 2}
; CHECK-DAG: [[FOUR]] = distinct !{[[FOUR]], [[RANGE4:![0-9]+]]}
; CHECK-DAG: [[RANGE4]] = !{!"llvm.loop.itercount.range", i32 4}
; CHECK-DAG: [[MAX]] = distinct !{[[MAX]], [[RANGE100:![0-9]+]]}
; CHECK-DAG: [[RANGE100]] = !{!"llvm.loop.itercount.range", i32 100}
; CHECK-DAG: [[FOURLOW]] = distinct !{[[FOURLOW]], [[FALLBACK:![0-9]+]]}
; CHECK-DAG: [[FALLBACK]] = !{!"llvm.loop.hint.aie-loop-version-fallback", i32 1}
; CHECK-DAG: [[OVER]] = distinct !{[[OVER]], [[MARKOVER:![0-9]+]]}
; CHECK-DAG: [[MARKOVER]] = !{!"llvm.loop.hint.aie-loop-versioned", i32 101}
; CHECK-DAG: [[WIDE]] = distinct !{[[WIDE]], {{![0-9]+}}}
; OPTION-DAG: [[OPTHIGH]] = distinct !{[[OPTHIGH]], [[OPTMARK:![0-9]+]]}
; OPTION-DAG: [[OPTMARK]] = !{!"llvm.loop.hint.aie-loop-versioned", i32 1}

!0 = distinct !{!0, !1}
!1 = !{!"llvm.loop.hint.aie-loop-versioning", i64 1}
!2 = distinct !{!2, !3}
!3 = !{!"llvm.loop.hint.aie-loop-versioning", i64 2}
!4 = distinct !{!4, !5}
!5 = !{!"llvm.loop.hint.aie-loop-versioning", i64 4}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.hint.aie-loop-versioning", i64 100}
!8 = distinct !{!8, !9}
!9 = !{!"llvm.loop.hint.aie-loop-versioning", i64 101}
!10 = distinct !{!10, !11}
!11 = !{!"llvm.loop.hint.aie-loop-versioning", i64 4294967298}
!12 = distinct !{!12, !13}
!13 = !{!"llvm.loop.itercount.range", i32 2, i32 100}
