; RUN: opt -O1 -S -mtriple=aie2-none-unknown-elf -aie-internalize-symbols < %s | FileCheck --check-prefix=ONLYMAIN %s
; RUN: opt -O1 -S -mtriple=aie2-none-unknown-elf -aie-internalize-symbols -aie-internalize-skip-functions="skipfunc1,skipfunc2,skipfunc3" < %s | FileCheck --check-prefix=FUNCLIST %s

; ONLYMAIN-NOT: @gvar_unused
; FUNCLIST-NOT: @gvar_unused
@gvar_unused = addrspace(1) global i32 undef, align 4

; ONLYMAIN: @gvar_used{{.*}}addrspace(1) global i32 undef, align 4
; FUNCLIST: @gvar_used{{.*}}addrspace(1) global i32 undef, align 4
@gvar_used = addrspace(1) global i32 undef, align 4

; ONLYMAIN: @gvar_func_ptr{{.*}}global ptr null
; FUNCLIST: @gvar_func_ptr{{.*}}global ptr null
@gvar_func_ptr = global ptr null

; ONLYMAIN-NOT: define{{.*}}void @func_unused
; FUNCLIST-NOT: define{{.*}}void @func_unused
define void @func_unused(ptr addrspace(1) %out, i32 %val) {
entry:
  store volatile i32 %val, ptr addrspace(1) %out
  ret void
}

; ONLYMAIN: define{{.*}}void @func_used_noinline
; FUNCLIST-NOT: define{{.*}}void @func_used_noinline
define void @func_used_noinline(ptr addrspace(1) %out, i32 %val) #0 {
entry:
  store volatile i32 %val, ptr addrspace(1) %out
  ret void
}

; ONLYMAIN-NOT: define{{.*}}void @func_used_alwaysinline
; FUNCLIST-NOT: define{{.*}}void @func_used_alwaysinline
define void @func_used_alwaysinline(ptr addrspace(1) %out, i32 %val) #1 {
entry:
  store volatile i32 %val, ptr addrspace(1) %out
  ret void
}

; ONLYMAIN: declare void @decl_func()
; FUNCLIST-NOT: declare void @decl_func()
declare void @decl_func()

; ONLYMAIN: define{{.*}}void @main()
; ONLYMAIN: call{{.*}}void @func_used_noinline
; ONLYMAIN: store volatile
; ONLYMAIN: call{{.*}}void @decl_func()
; ONLYMAIN: ret void
; FUNCLIST-NOT: define{{.*}}void @main()
; FUNCLIST-NOT: call{{.*}}void @func_used_noinline
; FUNCLIST-NOT: store volatile
; FUNCLIST-NOT: call{{.*}}void @decl_func()
; FUNCLIST-NOT: ret void
define void @main() {
entry:
    %val = add i32 42, 0
    tail call fastcc void @func_used_noinline(ptr addrspace(1) @gvar_used, i32 %val)
    tail call fastcc void @func_used_alwaysinline(ptr addrspace(1) @gvar_used, i32 %val)
    tail call fastcc void @decl_func()
    ret void
}

; ONLYMAIN-NOT: define{{.*}}void @func_used_skipfunc1
; FUNCLIST: define{{.*}}void @func_used_skipfunc1
define void @func_used_skipfunc1(ptr addrspace(1) %out, i32 %val) #0 {
entry:
  store i32 %val, ptr addrspace(1) %out
  ret void
}

; ONLYMAIN-NOT: define{{.*}}void @skipfunc1()
; ONLYMAIN-NOT: call{{.*}}void @func_used_skipfunc1
; ONLYMAIN-NOT: ret void
; FUNCLIST: define{{.*}}void @skipfunc1()
; FUNCLIST: call{{.*}}void @func_used_skipfunc1
; FUNCLIST: ret void
define void @skipfunc1() {
entry:
    %val = add i32 42, 0
    tail call fastcc void @func_used_skipfunc1(ptr addrspace(1) @gvar_used, i32 %val)
    ret void
}

; ONLYMAIN-NOT: define{{.*}}void @func_used_skipfunc2
; FUNCLIST: define{{.*}}void @func_used_skipfunc2
define void @func_used_skipfunc2(ptr addrspace(1) %out, i32 %val) #0 {
entry:
  store i32 %val, ptr addrspace(1) %out
  ret void
}

; ONLYMAIN-NOT: define{{.*}}void @skipfunc2()
; ONLYMAIN-NOT: call{{.*}}void @func_used_skipfunc2
; ONLYMAIN-NOT: ret void
; FUNCLIST: define{{.*}}void @skipfunc2()
; FUNCLIST: call{{.*}}void @func_used_skipfunc2
; FUNCLIST: ret void
define void @skipfunc2() {
entry:
    %val = add i32 42, 0
    tail call fastcc void @func_used_skipfunc2(ptr addrspace(1) @gvar_used, i32 %val)
    ret void
}

; ONLYMAIN-NOT: define{{.*}}void @func_used_skipfunc3
; FUNCLIST: define{{.*}}void @func_used_skipfunc3
define void @func_used_skipfunc3(ptr addrspace(1) %out, i32 %val) #0 {
entry:
  store i32 %val, ptr addrspace(1) %out
  ret void
}

; ONLYMAIN-NOT: define{{.*}}void @skipfunc3()
; ONLYMAIN-NOT: store ptr @func_used_skipfunc3, ptr @gvar_func_ptr, align 4
; ONLYMAIN-NOT: ret void
; FUNCLIST: define{{.*}}void @skipfunc3()
; FUNCLIST: store ptr @func_used_skipfunc3, ptr @gvar_func_ptr, align 4
; FUNCLIST: ret void
define void @skipfunc3() {
entry:
    store ptr @func_used_skipfunc3, ptr @gvar_func_ptr, align 4
    ret void
}

attributes #0 = { noinline nounwind }
attributes #1 = { alwaysinline nounwind }
