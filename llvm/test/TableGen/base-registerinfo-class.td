// RUN: llvm-tblgen -gen-register-info -I %p/../../include -I %p/Common %s \
// RUN:     | FileCheck %s --check-prefix=DEFAULT
// RUN: llvm-tblgen -gen-register-info -I %p/../../include -I %p/Common %s -base-registerinfo-class BaseRegInfoClass \
// RUN:     | FileCheck %s --check-prefix=BASE

// Verify the -base-registerinfo-class option

include "reg-with-subregs-common.td"

// DEFAULT: struct TestTargetGenRegisterInfo : public TargetRegisterInfo
// DEFAULT: TestTargetGenRegisterInfo(
// DEFAULT:  : TargetRegisterInfo(

// BASE: struct TestTargetGenRegisterInfo : public BaseRegInfoClass
// BASE: TestTargetGenRegisterInfo(
// BASE:  : BaseRegInfoClass(
