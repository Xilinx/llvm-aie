; RUN: llc -mtriple=%triple < %s | FileCheck %s

define i64 @i64_test(i64 %i) nounwind readnone {
  ret i64 %i
}
