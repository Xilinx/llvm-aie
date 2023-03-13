// RUN: %clang_cc1 %s %ccflags -o - | FileCheck %s

int foo() {
  return 0;
}
