---
name: aie-kernel-perf-optimization
description: "Performance optimization guide for custom AIE kernel development with Peano. Covers restrict pointers, DM bank annotations, loop pragmas, loop hints, software pipelining (pre-RA vs post-RA), loop versioning, function structure, pointer increments, sub-32-bit limitations, vector alignment, type conversion chains, and reading backend optimization hints from the compiler (remarks via -Rpass*/-fsave-optimization-record and warnings such as -Wpass-failed and the aie-multi-slot-pseudo missing-memory-bank hint). Consult when writing or reviewing AIE kernel C++ code for throughput, when diagnosing pipelining failures or high II, or when interpreting build-log warnings and missed-opportunity remarks from the AIE backend."
---

<!--
This file is licensed under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

(c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
-->

# Performance Optimization Guide for Custom AIE Kernel Development

This guide documents actionable techniques for writing high-performance
AIE kernels, with compiler context explaining why each technique works.
It draws from real optimization work across AIE kernels (Conv2D, GEMM,
MaxPool, Slice, Transpose, etc.) and applies to the Peano compiler.



## 1. Architecture Context

AIE cores are VLIW (Very Long Instruction Word) processors with an
exposed pipeline. Key properties that affect kernel performance:

- **VLIW slots**: Each cycle can issue multiple operations in parallel
  (load, store, scalar, vector, etc.) if they fit into different slots.
  Filling more slots per cycle = higher throughput.
- **In-order execution**: There is no out-of-order engine. The compiler
  must statically schedule instructions to hide latencies.
- **Zero-overhead loops (ZOL)**: Hardware loop support that eliminates
  branch overhead. The compiler converts eligible loops automatically.
- **Multiple memory banks**: Data memory is banked (A, B, C, D). Two
  loads or a load and a store can execute simultaneously if they access
  different banks.
- **Software pipelining**: The most important optimization for inner
  loops. It overlaps consecutive iterations so that instructions from
  iteration N execute while iterations N-(NS-1), ..., N-1 are still
  in-flight (where NS is the number of pipeline stages).

The compiler needs developer hints (restrict, bank annotations, trip
counts, pipelining directives) to produce optimal schedules. Without
them, it must be conservative.



## 2. Restrict Pointers

### What
The `__restrict` qualifier tells the compiler that a pointer does not
alias any other pointer in scope. This is the single most impactful
optimization annotation for AIE kernels.

### Why it matters
Without `__restrict`, the compiler must assume any store could modify
the memory a subsequent load reads from. This forces it to:
- Insert memory barriers between stores and loads
- Prevent reordering across store/load pairs
- Limit or disable software pipelining

With `__restrict`, the compiler knows loads and stores to different
pointers are independent, enabling aggressive instruction reordering
and pipelining.

### Common pitfalls

**Casting away restrict.**
A frequent mistake is declaring function parameters with `__restrict`,
then immediately casting them to unqualified pointers inside the
function body. The cast discards the restrict information.

```cpp
// BAD: restrict is lost after the cast
void foo(dtype *__restrict ifm_ptr, dtype *__restrict ofm_ptr, ...) {
    dtype __aie_dm_resource_a *input_ptr =
        (dtype __aie_dm_resource_a *)ifm_ptr;       // restrict is gone
    dtype __aie_dm_resource_b *output_ptr =
        (dtype __aie_dm_resource_b *)ofm_ptr;        // restrict is gone
    ...
}
```

```cpp
// GOOD: restrict-qualified pointers as parameters, no stripping casts
void foo(
    dtype __aie_dm_resource_a *__restrict input_ptr,
    dtype __aie_dm_resource_b *__restrict ofm_load_ptr,
    dtype __aie_dm_resource_b *__restrict ofm_write_ptr,
    ...) {
    // Use input_ptr, ofm_load_ptr, ofm_write_ptr directly
}
```

**Restrict on buffer references vs raw pointers.**
ADF kernel entry points receive `adf::input_buffer_conf` and
`adf::output_buffer_conf` references. Marking these references as
`__restrict` is **not sufficient** -- the restrict qualifier on a
reference to a buffer object does not propagate to the underlying data
pointer returned by `.data()`.

The function that contains the hot loop must receive raw
`dtype *__restrict` pointers, not buffer references. Call `.data()`
at the boundary (in the entry-point wrapper) and pass the resulting
restrict-qualified pointer into the core function:

```cpp
// BAD: restrict on buffer ref does not help the loop
void my_kernel(
    adf::input_buffer_conf<int8_t, ...> &__restrict ifm,
    adf::output_buffer_conf<int8_t, ...> &__restrict ofm) {

    // .data() returns a plain pointer -- restrict from the ref is gone
    auto *in = ifm.data();
    auto *out = ofm.data();
    for (int i = 0; i < N; i++) {
        out[i] = in[i] + 1;   // compiler assumes in/out may alias
    }
}
```

```cpp
// GOOD: .data() called in wrapper, restrict on raw pointers
void my_kernel(
    adf::input_buffer_conf<int8_t, ...> &__restrict ifm,
    adf::output_buffer_conf<int8_t, ...> &__restrict ofm) {

    my_kernel_core(
        (int8_t *__restrict)ifm.data(),
        (int8_t *__restrict)ofm.data());
}

__attribute__((noinline)) void my_kernel_core(
    int8_t *__restrict in,
    int8_t *__restrict out) {

    for (int i = 0; i < N; i++) {
        out[i] = in[i] + 1;   // compiler knows in/out don't alias
    }
}
```

The key rule: **the function containing the loop must receive
`__restrict` pointers as parameters, not buffer references**. The
`.data()` extraction and restrict cast belong in the wrapper that
calls it.

### Pattern: inline wrapper + noinline core

LLVM does not support scoped restrict: `__restrict` on function
parameters creates a no-alias guarantee that only holds at the
function boundary. When the function is inlined, this scope is
lost and the inlined code inherits the caller's alias information.

This matters when the same pointer is passed for both reading and
writing (e.g., `foo_core(ifm_ptr, ofm_ptr, ofm_ptr)` where
`ofm_ptr` serves as both load and store pointer). Without
`noinline`, the compiler sees both pointers originate from the
same value and must assume they alias. This forces the pipeliner
to stall each iteration's load until the previous iteration's
store completes, preventing it from overlapping iterations and
inflating II.

```cpp
// BAD: only 2 pointers -- ofm used for both load and store.
//      If inlined, compiler sees one ofm pointer and assumes
//      the load and store alias, preventing iteration overlap.
template <typename dtype>
void foo(
    dtype IFM_DM_BANK *__restrict ifm_ptr,
    dtype OFM_DM_BANK *__restrict ofm_ptr,
    params_t &__restrict params) {
    AIE_LOOP_RANGE(4,)
    for (int i = 0; i < params.count; i++) {
        auto in_vec = aie::load_v<32>(ifm_ptr);
        auto ofm_vec = aie::load_v<32>(ofm_ptr);
        auto result = aie::max(in_vec, ofm_vec);
        aie::store_v(ofm_ptr, result);
        ifm_ptr = add_2d_byte(ifm_ptr, params.ifm_iter);
        ofm_ptr = add_2d_byte(ofm_ptr, params.ofm_iter);
    }
}
```

```cpp
// GOOD: noinline core preserves restrict scope at function boundary
template <typename dtype>
__attribute__((noinline)) void foo_core(
    dtype IFM_DM_BANK *__restrict ifm_ptr,
    dtype OFM_DM_BANK *__restrict ofm_load_ptr,
    dtype OFM_DM_BANK *__restrict ofm_write_ptr,
    params_t &__restrict params) {
    AIE_LOOP_RANGE(4,)
    for (int i = 0; i < params.count; i++) {
        auto in_vec = aie::load_v<32>(ifm_ptr);
        auto ofm_vec = aie::load_v<32>(ofm_load_ptr);
        auto result = aie::max(in_vec, ofm_vec);
        aie::store_v(ofm_write_ptr, result);
        ifm_ptr = add_2d_byte(ifm_ptr, params.ifm_iter);
        ofm_load_ptr = add_2d_byte(ofm_load_ptr, params.ofm_iter);
        ofm_write_ptr = add_2d_byte(ofm_write_ptr, params.ofm_iter);
    }
}

// Inline wrapper: casts pointers and forwards
template <uint8_t batch_size, typename dtype>
INLINE void foo(
    adf::input_buffer_conf<dtype, ...> &__restrict ifm,
    adf::output_buffer_conf<dtype, ...> &__restrict ofm,
    params_t &params) {
    foo_core<dtype>(
        (dtype IFM_DM_BANK *__restrict)ifm.data(),
        (dtype OFM_DM_BANK *__restrict)ofm.data(),
        (dtype OFM_DM_BANK *__restrict)ofm.data(),
        params);
}
```

### Where to apply restrict
- All function pointer parameters that don't alias each other
- Parameter struct references (`gemm_params_t &__restrict param`)
- Setup function arguments (`const uint32_t *__restrict lp`)


## 3. DM Bank Annotations

### What
AIE data memory is organized into banks. Each bank supports
only a single memory access per cycle. The VLIW bundle has
two memory slots (slot A and slot B), so to fill both slots
in the same cycle, the two accesses must target different
banks. Annotating pointers with their bank assignment tells
the compiler which bank each access uses, enabling it to
schedule both memory slots in parallel. Accessing the
same bank from both slots causes a hardware stall.

### Syntax
```cpp
// Direct annotation
dtype __aie_dm_resource_a *ptr;    // Bank A
dtype __aie_dm_resource_b *ptr;    // Bank B
dtype __aie_dm_resource_c *ptr;    // Bank C
dtype __aie_dm_resource_d *ptr;    // Bank D
dtype __aie_dm_resource_bd *ptr;   // Bank B or D (flexible)

// Macro shorthand
#define DM_BANK(x) __aie_dm_resource_##x
dtype DM_BANK(a) *ptr;

// Conventional aliases (defined per kernel in *_params.h)
#define IFM_DM_BANK __aie_dm_resource_a
#define WT_DM_BANK  __aie_dm_resource_a
#define OFM_DM_BANK __aie_dm_resource_b
```

### Typical bank assignments

| Data         | Bank | Rationale                                        |
|-------------|------|--------------------------------------------------|
| Input (IFM) | A    | Dedicated input bank for parallel access with outputs |
| Weights     | B    | Separate bank from IFM enables parallel loads    |
| Output (OFM)| B    | Separate bank from inputs enables parallel store + load|
| Bias / TDM  | C/D  | Secondary data, flexible placement               |
| Accumulators| C/D  | Partial sums loaded/stored independently          |


### Common pitfall: same-bank conflict

If two memory operations in the same VLIW bundle target the
same bank, the hardware inserts a stall cycle to serialize
the accesses. In a pipelined loop this stall inflates II.

```cpp
// BAD: both input pointers on bank A -- loads are serialized
static void compute(
    bfloat16 __aie_dm_resource_a *__restrict input_ptr,
    bfloat16 __aie_dm_resource_a *__restrict input2_ptr,
    bfloat16 *__restrict output_ptr,
    int size
)
```

```cpp
// GOOD: input pointers on different banks -- parallel loads
static void compute(
    bfloat16 __aie_dm_resource_a *__restrict input_ptr,
    bfloat16 __aie_dm_resource_b *__restrict input2_ptr,
    bfloat16 *__restrict output_ptr,
    int size
)
```



### Common pitfall: casting away bank annotations

Similar to the restrict pitfall in Section 2, pointer casts can
silently strip DM bank qualifiers. When an intermediate pointer is
created via cast without preserving the bank annotation, the compiler
loses bank information for that pointer and cannot schedule parallel
memory operations.

```cpp
// BAD: cast drops the bank annotation -- compiler loses bank info
bfloat16 IFM_DM_BANK *__restrict p_in = ...;
v32bfloat16 *p_in_vec = (v32bfloat16 *)p_in;  // IFM_DM_BANK is gone
```

```cpp
// GOOD: cast preserves the bank annotation
bfloat16 IFM_DM_BANK *__restrict p_in = ...;
v32bfloat16 IFM_DM_BANK *p_in_vec = (v32bfloat16 IFM_DM_BANK *)p_in;
```

This is easy to miss when changing pointer element types (e.g.,
scalar to vector) for FIFO or vector load operations. Always carry
the bank qualifier through every cast in the chain.

When a load reaches the multi-slot pseudo materializer without a DM
bank qualifier, the compiler still picks a slot for that load, but
it has to fall back to a bank-blind heuristic. The chosen slot is 
often suboptimal and can prevent the pipeliner from finding the best II.
The compiler reports this as a missed-opportunity hint -- a stderr 
warning plus an `-Rpass-missed=aie-multi-slot-pseudo` remark. See 
Section 14 for how to read these hints.


## 4. Sub-32-Bit Store Limitations

Stores narrower than 32 bits (e.g., `int8`, `int16`, `bfloat16`
scalars) are very expensive on AIE. Part-word loads are no worse than
full-width loads, but sub-word stores force the memory hardware to
perform a full-width read-modify-write internally.

- **Performance**: Sub-word stores are significantly more expensive
  than 32-bit or vector-width stores because the hardware must read
  the surrounding word, merge the sub-word value, and write it back.
  The read-modify-write sequence occupies multiple processor pipeline
  stages, inflating II and reducing throughput in pipelined loops. A
  single sub-word store in a loop body can prevent the pipeliner from
  achieving the resource-limited MII.
- **Alias analysis**: The read-modify-write destroys alias information
  because the hardware touches the entire containing word, not just
  the addressed byte(s). The compiler must treat sub-word stores
  conservatively, limiting reordering and pipelining opportunities.

**Rule**: Stores must be at least 32 bits (4 bytes) wide to
avoid the read-modify-write penalty. Use vector-width stores
(`store_v`) in performance-critical loops. Pack data into
vectors before storing. Avoid scalar sub-word stores inside
hot loops.

```cpp
// BAD: scalar int8 stores -- each st.s8 instruction incurs a
//      6-cycle pipeline drain (read-modify-write penalty)
int8_t *ofm = ...;
for (int i = 0; i < 32; i++) {
    ofm[i] = result;  // st.s8: 7 cycles per iteration
}
```

```cpp
// GOOD (bulk data): vector store -- single store, no penalty
aie::vector<int8_t, 32> result_vec = aie::broadcast<int8_t, 32>(result);
aie::store_v(ofm_ptr, result_vec);
ofm_ptr = byte_incr(ofm_ptr, 32);
```

```cpp
// GOOD (few values): pack int8 results into a vector, store once
//      Avoids per-element st.s8; one vector store at the end
aie::vector<int8_t, 4> packed;
packed[0] = a;
packed[1] = b;
packed[2] = c;
packed[3] = d;
aie::store_v(ofm_ptr, packed);  // single 32-bit store
```

## 5. Loop Pragmas Reference

All macros are defined in `utils.h` (available in the mllib/L1/include/common/utils.h).

### Macro table

The table below lists the `AIE_*` wrapper macros and their underlying
`#pragma clang loop` expansions.

| AIE_* macro                       | `#pragma clang loop` option        | Purpose                                                         |
|-----------------------------------|------------------------------------|----------------------------------------------|
| `AIE_LOOP_MIN_ITERATION_COUNT(N)` | `min_iteration_count(N)`           | Minimum trip count; used by the pipeliner and unroller           |
| `AIE_LOOP_MAX_ITERATION_COUNT(N)` | `max_iteration_count(N)`           | Maximum trip count; enables bounded-count optimizations          |
| `AIE_LOOP_RANGE(min,)` / `AIE_LOOP_RANGE(min, max)` | (both combined)       | Convenience macro combining min and optional max         |
| `AIE_PREPARE_FOR_POSTPIPELINING`  | `pipeline(disable)`                | Disable the pre-RA pipeliner; the post-RA pipeliner takes over (see [Section 7](#7-software-pipelining))  |
| `AIE_PREPARE_FOR_PIPELINING`      | (no-op)                            | Legacy; explicitly allows software pipelining (this is the default if a minimum iteration count is provided) |
| `AIE_TRY_INITIATION_INTERVAL(N)`  | `pipeline_initiation_interval(N)`  | Suggest a target II to the pipeliner                            |
| `AIE_LOOP_NO_UNROLL`              | `unroll(disable)`                  | Prevent loop unrolling                                          |
| `AIE_LOOP_UNROLL_FULL`            | `unroll(full)`                     | Fully unroll the loop                                           |
| `AIE_LOOP_UNROLL(N)`              | `unroll_count(N)`                  | Unroll by factor N                                              |
| `AIE_LOOP_HINT(key, value)`       | `hint(key, value)`                 | Pass arbitrary per-loop metadata to specific compiler passes (see [Section 6](#6-loop-hints))   |
| `AIE_KEEP_SWLOOP`                 | (TBD)                              | Keep the software loop structure (prevent ZOL conversion)       |
| `AIE_NO_PIPELINING`               | (TBD)                              | Disable all software pipelining for the loop                    |

The macros that matter most for kernel performance are
`AIE_PREPARE_FOR_POSTPIPELINING`, `AIE_TRY_INITIATION_INTERVAL`,
`AIE_LOOP_RANGE`, and `AIE_LOOP_HINT`.

### Placement
Place the `AIE_*` macros immediately before the `for` statement:

```cpp
AIE_PREPARE_FOR_PIPELINING
AIE_LOOP_RANGE(4,)
for (int i = 0; i < param.inner_count; i++) {
    // loop body
}
```




## 6. Loop Hints

### What
`AIE_LOOP_HINT(key, value)` is a mechanism to pass arbitrary
key-value metadata to specific compiler passes on a per-loop basis.

### How it works
On Peano, `AIE_LOOP_HINT(key, value)` expands to:
```
#pragma clang loop hint(key, value)
```
This attaches `!llvm.loop.hint.<key>` metadata to the loop's back-edge
branch in LLVM IR. The compiler's `LoopOptionOverrides` infrastructure
reads this metadata and overrides the corresponding `cl::opt` default
for that specific loop only.

### Precedence
1. Explicit command-line flag (e.g., `--aie-gpr-realloc=true`) --
   highest priority, always wins
2. Per-loop metadata (`AIE_LOOP_HINT(aie-gpr-realloc, 1)`)
3. `cl::opt` compiled-in default -- lowest priority

### Example: enabling register reallocation for a hot loop
```cpp
AIE_LOOP_HINT(aie-gpr-realloc, 1)
AIE_LOOP_HINT(aie-realloc-loopaware, 1)
AIE_PREPARE_FOR_PIPELINING
AIE_LOOP_RANGE(6,)
for (int i = 0; i < count; i++) {
    // This specific loop gets register reallocation even if the
    // global default is off
}
```

### Available keys
Any `cl::opt` ArgStr defined in the AIE backend can be used as a key.
The value is either an integer (for bool/int/enum options) or a string.

The most common use case is tuning the WAW (Write-After-Write)
register rewriter, which renames registers in pipelined loops to
eliminate WAW hazards. Available options:

| Key                          | Type    | Default | Effect                                                     |
|------------------------------|---------|---------|-------------------------------------------------------------|
| `aie-gpr-realloc`           | 0/1     | 0       | Enable GPR register reallocation in addition to vector regs |
| `aie-realloc-loopaware`     | 0/1     | 0       | Prime the LRU queue so allocations wrap around loop boundary|
| `aie-aggressive-realloc`    | 0/1     | 0       | Aggressively de-allocate live-through registers             |
| `aie-reg-rewrite-mode`      | string  | auto    | Rewriting mode: `basic`, `auto`, `latencyaware`, `swpaware`, `swpaware-auto` |
| `aie-waw-reg-rewrite-min-lat`| int    | 3       | Minimum operand latency considered for WAW rewriting        |
| `aie-realloc-ii-bias`       | int     | 0       | MinII bias for swpaware mode                                |

Example: enabling GPR reallocation with loop-aware priming on a
specific hot loop:
```cpp
AIE_LOOP_HINT(aie-gpr-realloc, 1)
AIE_LOOP_HINT(aie-realloc-loopaware, 1)
AIE_LOOP_HINT(aie-reg-rewrite-mode, swpaware)
```

### Disabling per-loop overrides
For debugging, pass `--aie-ignore-loop-hints` to `llc`
to ignore all `AIE_LOOP_HINT` metadata globally.


## 7. Software Pipelining

### What the pipeliner does
Software pipelining overlaps consecutive iterations of a loop body.
Instead of completing iteration N before starting N+1, the pipeliner
interleaves them so that, e.g., loads for iteration N+2 happen while
iteration N is computing and iteration N-1 is storing.

This fills more VLIW slots per cycle and hides long latencies (memory,
multiply-accumulate).

### Pre-RA vs post-RA pipeliner
Peano has two software pipeliners:
- **Pre-RA pipeliner** (MachinePipeliner): Runs before register
  allocation. It has more scheduling freedom but may produce
  schedules that cause high register pressure and spilling.
  It can also create a guarded prologue (see `AIE_KEEP_SWLOOP`).
- **Post-RA pipeliner**: Runs after register allocation. Register
  assignments are fixed, so it avoids spill-related issues.

The compiler automatically selects which pipeliner to use for each
loop. There is no guarantee that a particular pipeliner was used.

`AIE_PREPARE_FOR_POSTPIPELINING` disables the pre-RA pipeliner for
that loop (via `#pragma clang loop pipeline(disable)`), guaranteeing
the post-RA pipeliner will be the one to attempt pipelining. This is
the only way to ensure the post-RA pipeliner runs on a given loop.

When to use `AIE_PREPARE_FOR_POSTPIPELINING`:
- The pipelined loop has spill code in the loop body
- The pipelined loop contains sequences of register moves
  ("register-fifos")

### Key concepts

Software pipelining splits the loop body into stages of equal length
that are then executed in parallel in pipeline fashion. Each stage
takes II cycles, and NS stages overlap in flight.

**II (Initiation Interval)**: The number of cycles between starting
two consecutive iterations. Lower II = higher throughput. An II of 1
means a new iteration starts every cycle.

**MII (Minimum Initiation Interval)**: The theoretical lower bound,
determined by:
- **Resource MII**: Determined by the most-used VLIW slot.
  Each slot can execute one operation per cycle; the slot
  with the highest demand sets the Resource MII. For example,
  if one iteration uses 2 slot-A ops, 1 slot-B op, 1 ALU op,
  and 1 store, the bottleneck is slot A (2 uses), so
  Resource MII = 2.
- **Recurrence MII**: Determined by data dependence cycles within the
  loop (e.g., accumulator feedback chains)

MII = max(Resource MII, Recurrence MII). The pipeliner tries to find a
schedule at MII; if it fails it increments II and retries.

**NS (Number of Stages)**: How many overlapping iterations are in
flight. More stages = more overlap but also more prologue/epilogue
code and higher register pressure.

**Minimum trip count requirement**: The loop must execute at least NS
times for the pipelined schedule to be valid. This is why
`AIE_LOOP_RANGE` is important -- it tells the pipeliner the loop will
iterate enough times.

### Trip count and AIE_LOOP_RANGE

```cpp
// Tell the compiler this loop executes at least 4 times
AIE_LOOP_RANGE(4,)
for (int i = 0; i < param.count; i++) { ... }

// Tell the compiler this loop executes between 4 and 16 times
AIE_LOOP_RANGE(4, 16)
for (int i = 0; i < param.count; i++) { ... }
```

If you set the minimum too low, the pipeliner may produce suboptimal code. If you set it too high
(higher than the actual runtime count), the program will be incorrect.

### Suggesting a target II
`AIE_TRY_INITIATION_INTERVAL(N)` tells the postpipeliner to try
harder to reach II = N. Normally the pipeliner tries a sequence of
heuristics at each candidate II and moves on if they fail. When a
target II is set, the pipeliner additionally invokes its exact solver
at that II, significantly increasing the chance of finding a valid
schedule at the cost of longer compile time.

```cpp
AIE_TRY_INITIATION_INTERVAL(4)
AIE_PREPARE_FOR_PIPELINING
AIE_LOOP_RANGE(8,)
for (int i = 0; i < count; i++) { ... }
```

The target II is not a guarantee. If no valid schedule exists at the
requested II (due to resource or recurrence constraints), the
pipeliner continues to higher II values. Use optimization remarks to
verify the achieved II.


## 8. Loop Versioning

### The problem
Software pipelining needs a minimum trip count (typically 4-8,
depending on NS). If the loop count is a runtime parameter that might
be small, the pipeliner must either:
- Refuse to pipeline (safe but slow for large counts)
- Generate guarded prologue/epilogue (code size overhead)

### Prefer AIE_LOOP_RANGE over loop versioning
Loop versioning duplicates the entire loop body, which **doubles
program memory (PM) usage** for that loop. Before reaching for
`VERSIONED_LOOP`, consider whether you can simply increase
`AIE_LOOP_RANGE` to guarantee a sufficient minimum iteration count.

If the kernel's calling convention or tiling guarantees the loop
always executes at least N times, set `AIE_LOOP_RANGE(N,)` and avoid
versioning entirely. Loop versioning should only be used when the trip
count genuinely varies below the pipelining threshold at runtime.

### The solution: VERSIONED_LOOP
Split the loop into two runtime paths:
- **High-count path**: The trip count is >= MinIters, so pipelining is
  enabled with appropriate `AIE_LOOP_RANGE` and pipelining pragmas.
- **Low-count path**: The trip count is < MinIters, so pipelining is
  disabled and the loop runs as simple scalar code.

```cpp
// Define the loop body as a lambda
auto body = [&]() __attribute__((always_inline)) {
    target_vec = aie::load_v<VEC_LEN>(ofm_load_ptr);
    ifm_vec = aie::load_v<VEC_LEN>(ifm_load_ptr);
    store_v(ofm_write_ptr, aie::max(target_vec, ifm_vec));
    ofm_load_ptr = byte_incr(ofm_load_ptr, byte_incr_val);
    ofm_write_ptr = byte_incr(ofm_write_ptr, byte_incr_val);
};

// 6 = minimum iterations for pipelining to be profitable
VERSIONED_LOOP(6, param.inner_loop_count, body);
```

### The VERSIONED_LOOP macro
Defined in `utils.h`:
```cpp
#define VERSIONED_LOOP(MinIters, count, body, ...) \
    do { \
        if ((count) >= (MinIters)) { \
            __VA_ARGS__ \
            AIE_PREPARE_FOR_PIPELINING \
            AIE_LOOP_RANGE(MinIters,) \
            for (int _vl_i = 0; _vl_i < (count); _vl_i++) { \
                (body)(); \
            } \
        } else { \
            AIE_NO_PREPARE_FOR_PIPELINING \
            AIE_LOOP_RANGE(1,) \
            AIE_LOOP_NO_UNROLL \
            for (int _vl_i = 0; _vl_i < (count); _vl_i++) { \
                (body)(); \
            } \
        } \
    } while(0)
```

The optional trailing `...` argument accepts extra pragma macros for
the high-count path (e.g., `AIE_LOOP_NO_UNROLL`).

### Choosing MinIters
MinIters must be >= the number of pipeline stages (NS) the compiler
will find. This depends on the loop body complexity:
- Simple load-compute-store: NS ~ 3-4, use MinIters = 4
- Complex MAC with multiple loads: NS ~ 5-7, use MinIters = 6-8
- Check optimization remarks for the actual NS



### Lambda pattern details
The body must be a named lambda, not an inline lambda in the macro
call. This avoids preprocessor issues with commas in template
arguments:

```cpp
// BAD: commas in template args confuse the preprocessor
VERSIONED_LOOP(6, count, [&]() {
    aie::max<aie::vector<int8_t, 32>, aie::vector<int8_t, 32>>(...);
});

// GOOD: named lambda
auto body = [&]() __attribute__((always_inline)) {
    aie::max<aie::vector<int8_t, 32>, aie::vector<int8_t, 32>>(...);
};
VERSIONED_LOOP(6, count, body);
```


## 9. Function Structure for Pipelining

### NOINLINE core, INLINE wrapper

See [Section 2, Pattern: inline wrapper + noinline core](#pattern-inline-wrapper--noinline-core)
for the full explanation of why `noinline` is required. In short:
LLVM does not support scoped restrict, so inlining a function
loses the `__restrict` guarantees on its parameters. When the same
buffer is passed for both load and store, this forces the pipeliner
to assume aliasing and prevents it from overlapping iterations.

The core function containing the hot loop must be `NOINLINE`. The
public API function should be a thin `INLINE` wrapper that casts
pointers and forwards them to the core.

## 10. Pointer Increment Patterns

### Post-increment vs indexed access
Post-increment addressing maps directly to AIE hardware addressing
modes, saving an instruction in the loop body:

```cpp
// BAD: indexed access -- requires separate address computation
aie::vector<bfloat16, 64> in_vec = aie::vector<bfloat16, 64>(ptrA[i]);
ptrB[i] = out_vec.to_native();
```

```cpp
// GOOD: post-increment -- maps to hardware postinc mode
aie::vector<bfloat16, 64> in_vec = aie::vector<bfloat16, 64>(*ptrA++);
*ptrB++ = out_vec.to_native();
```

### 2D/3D hardware iterators
AIE provides hardware address generators with 2D and 3D striding.
Use the iterator macros to leverage them:

```cpp
SETUP_ITR3D_COUNTERS(iterator)

// Inside the loop:
input_ptr = INCR_ITR3D_PARAM(input_ptr, iterator, params)
```

These macros use `add_3d_byte` / `add_2d_byte` intrinsics that map to
the hardware's multi-dimensional address generation unit. The hardware
handles wrap and stride in zero cycles.

### FIFO reads
For unaligned streaming access, use FIFO intrinsics:
```cpp
fifo_state_t fA;
fA.pos = 0;
aie::vector<bfloat16, 32> v0 = fifo_ld_pop(pInput, fA);
```
FIFO operations manage alignment internally and map to efficient
hardware primitives.


## 11. Vector Load/Store Alignment

Vector loads and stores require the address to be aligned to the
vector width. Misaligned accesses cause undefined behavior or
hardware exceptions.

### Alignment requirements
- 256-bit (32-byte) vectors: address must be 32-byte aligned
- 512-bit (64-byte) vectors: address must be 64-byte aligned
- 1024-bit (128-byte) vectors: address must be 128-byte aligned

### Ensuring alignment
- **Buffer allocation**: ADF buffers allocated via `adf::location`
  are naturally aligned. Stack-allocated vectors should use
  `alignas`:
  ```cpp
  alignas(64) int8_t local_buf[256];
  ```
- **Pointer arithmetic**: When using `byte_incr` or manual pointer
  offsets, ensure the increment preserves alignment. Incrementing by
  the vector width in bytes always preserves alignment if the base
  was aligned.
- **Parameter-driven offsets**: If a runtime parameter controls a
  pointer offset, verify the parameter is a multiple of the vector
  alignment. Misaligned offsets from parameters are a common source
  of hard-to-debug crashes.
- **FIFO reads**: FIFO intrinsics (`fifo_ld_pop`) handle unaligned
  access internally and are safe for arbitrary alignment.

### Diagnosing alignment issues
Misaligned vector accesses typically manifest as simulation hangs or
incorrect data (silent corruption). If a kernel produces wrong results
only for certain parameter combinations, check whether the failing
cases produce misaligned pointers.

## 12. Advanced Loop Optimization Techniques

### Prefer compiler pipelining over manual unrolling
Manual loop unrolling (duplicating the loop body N times, reducing the
trip count by N) increases code size and prevents the post-RA
pipeliner from working effectively. The pipeliner needs a single-body
loop to overlap iterations.

Instead of manual unrolling, use `AIE_LOOP_RANGE` with a minimum
equal to the former unroll factor, and let the compiler pipeline:

```cpp
// BAD: manual 8x unroll -- pipeliner cannot overlap iterations
for (int i = 0; i < count; i++) {
    vec1 = load_v<16>(in_ptr); INCR_ITR2D_PARAM(in_ptr, ...)
    vec2 = load_v<16>(in_ptr); INCR_ITR2D_PARAM(in_ptr, ...)
    // ... 6 more copies ...
    store_v(out_ptr, vec1); INCR_ITR2D_PARAM(out_ptr, ...)
    store_v(out_ptr, vec2); INCR_ITR2D_PARAM(out_ptr, ...)
    // ... 6 more copies ...
}

// GOOD: single body, pipelined -- compiler overlaps iterations
AIE_PREPARE_FOR_PIPELINING
AIE_LOOP_RANGE(8,)
for (int i = 0; i < count * 8; i++) {
    vec = load_v<16>(in_ptr); INCR_ITR2D_PARAM(in_ptr, ...)
    store_v(out_ptr, vec);    INCR_ITR2D_PARAM(out_ptr, ...)
}
```

Multiply the trip count by the former unroll factor and set
`AIE_LOOP_RANGE` to guarantee the compiler the new minimum.

### AIE_LOOP_UNROLL for controlled unrolling
When the loop body is too small for the pipeliner to fill all VLIW
slots, controlled unrolling can help by giving the pipeliner a larger
loop body to schedule. Use `AIE_LOOP_UNROLL(N)` together with
pipelining pragmas:

```cpp
AIE_LOOP_UNROLL(2)
AIE_PREPARE_FOR_PIPELINING
AIE_LOOP_RANGE(4,)
for (int w = 0; w < w_loop_cnt; w++) {
    // The compiler unrolls 2x, then pipelines the unrolled body
}
```

This is different from manual unrolling: the compiler still sees a
loop and can pipeline the unrolled body. The trip count does not
change (the compiler adjusts internally).

### Minimize type conversion chains
Type conversion chains that bounce through intermediate types add
instructions to the loop body, increasing II. Use the most direct
conversion available:

```cpp
// BAD: int8 -> acc32 -> int32 -> float -> accfloat -> bfloat16
//   5 conversion steps, each adding instructions to the loop body
aie::vector<int8, 16> in_int8 = *it_input++;
aie::accum<acc32, 16> in_acc32;
in_acc32.template from_vector(in_int8);
aie::vector<int32, 16> in_int32 = in_acc32.to_vector<int32>();
aie::vector<float, 16> in_float = aie::to_float(in_int32, shift);
aie::accum<accfloat, 16> in_accfloat;
in_accfloat.template from_vector(in_float);
auto result = in_accfloat.to_vector<bfloat16>();

// GOOD: int8 -> float -> bfloat16
//   2 conversion steps using direct API
aie::vector<int8, 16> in_int8 = *it_input++;
aie::vector<float, 16> in_float = aie::to_float(in_int8, shift);
auto result = aie::accum<accfloat, 16>(in_float).to_vector<bfloat16>();
```

Check the AIE API documentation for direct conversion functions
(`aie::to_float`, `aie::exp2`, etc.) before writing multi-step
conversion chains.

### Stay in the native data type
When processing data that will be converted later, delay the
conversion and operate in the native type as long as possible. This
reduces the number of instructions in the loop body:

```cpp
// BAD: convert int8 -> float for max reduction
//   float max is more expensive, and conversion adds instructions
for (int i = 0; i < elem_iters; i++) {
    aie::vector<int8, 16> in_int8 = *input_max++;
    // ... 5-step conversion to bfloat16 ...
    max_vec = aie::max(max_vec, converted_vec);
}

// GOOD: find max directly in int8 domain
aie::vector<int8, 16> max_vec_int8 = aie::broadcast<int8, 16>((int8)-128);
AIE_PREPARE_FOR_PIPELINING
AIE_LOOP_RANGE(4,)
for (int i = 0; i < elem_iters; i++) {
    aie::vector<int8, 16> in_int8 = *input_max++;
    max_vec_int8 = aie::max(max_vec_int8, in_int8);
}
int8 max_value = aie::reduce_max(max_vec_int8);
// Convert only the final scalar result
```

### Early exit for zero-trip-count loops
Always guard loops that may have zero iterations:

```cpp
// GOOD: skip all setup when nothing to do
if (param.trim_loops == 0)
    return;
// ... iterator setup ...
AIE_LOOP_RANGE(8,)
for (int i = 0; i < param.trim_loops * 8; i++) { ... }
```

This avoids unnecessary prologue/epilogue execution and simplifies
the compiler's analysis.


## 13. Coding for Peano

### Always use AIE_* macros
Never use raw `#pragma clang loop` directives in kernel code. Use the
`AIE_*` macros from `utils.h` which expand correctly for Peano.

### Template argument deduction
`aie::concat` accepts a tuple of vectors, but when
`std::make_tuple` deduces its template arguments from
expressions like `shuffle(...)`, Clang may infer a
different type than `aie::concat` expects (e.g., a native
vector type instead of `aie::vector`), causing overload
resolution to fail. Specifying the tuple element types
explicitly avoids this:

```cpp
// BAD: implicit template args -- may fail to compile on Peano
auto result = aie::concat(std::make_tuple(
    shuffle(v0, v1, T16_4x16_lo),
    shuffle(v0, v1, T16_4x16_hi)));

```

```cpp
// GOOD: explicit template types -- compiles cleanly on Peano
auto result = aie::concat(
    std::make_tuple<aie::vector<bfloat16, 32>,
                    aie::vector<bfloat16, 32>>(
        shuffle(v0, v1, T16_4x16_lo),
        shuffle(v0, v1, T16_4x16_hi)));
```

### DM bank syntax
Always use `__aie_dm_resource_*` qualifiers for memory bank
annotations:

```cpp
v128mx6_unaligned __aie_dm_resource_bd *pW = ...;
v16accfloat __aie_dm_resource_b *pB = ...;
```


## 14. Reading Optimization Hints (Remarks and Warnings)

Optimization remarks and warnings are the primary tool for evaluating
how well the compiler pipelined a loop and for diagnosing performance
issues. They report the achieved II, stage count, prologue/epilogue
cost, and whether a loop became a zero-overhead loop, and they flag
missed opportunities the compiler could not act on (e.g. a load
without a memory-bank annotation).

The AIE backend exposes this information through two channels:

- **Remarks channel**: structured records emitted by AIE backend
  passes (`postpipeliner`, `aie-hardware-loops`, `aie-asm-printer`,
  `aie-multi-slot-pseudo`). Each remark has a kind: applied
  (`Passed`), missed opportunity (`Missed`), or informational
  (`Analysis`). Remarks are off by default -- they only appear when
  a corresponding `-Rpass*` / `-pass-remarks*` flag is given, or via
  the YAML optimization record.
- **Warnings channel**: diagnostics that always reach the build log
  without any remark flag. Two sources matter for kernel authors:
  the default-on `-Wpass-failed` warnings for dropped IR-level
  pragmas, and the AIE backend's own `WithColor::warning` calls
  (currently the multi-slot materializer's missing-memory-bank
  warning).

The remaining subsections cover the existing llc / YAML workflow
first, then the clang-driven view, then the warnings channel, and
finally the new `aie-multi-slot-pseudo` missing-memory-bank hint.

### Enabling remarks
Pass these flags to `llc` (or via the build system's compiler flags):

```bash
# Write all remarks to stdout (common for piping into FileCheck)
llc -pass-remarks-output=- \
    -pass-remarks-filter='postpipeliner|aie-hardware-loops|aie-asm-printer' \
    -mtriple=aie2p input.ll -o output.s

# Write to a YAML file for offline analysis
llc -pass-remarks-output=remarks.yaml \
    -pass-remarks-filter='postpipeliner|aie-hardware-loops|aie-asm-printer' \
    -mtriple=aie2p input.ll -o output.s
```

### Postpipeliner remarks
The postpipeliner emits a `!Passed` remark when it finds a valid
software-pipelined schedule. The remark contains everything needed to
evaluate the quality of the schedule:

```yaml
--- !Passed
Pass:            postpipeliner
Name:            schedule
Function:        foo
Args:
  - String:          Schedule found
  - II:              '18'              # Initiation interval
  - NS:              '2'               # Number of pipeline stages
  - Loop:            for.body          # The loop basic block
  - Prologue:        entry             # Block where prologue was inserted
  - PrologueBundles: '18'             # VLIW bundles in the prologue
  - Epilogue:        ''                # Block where epilogue was inserted
  - EpilogueBundles: '9'              # VLIW bundles in the epilogue
...
```

**Fields explained:**
- **II**: Cycles between starting consecutive iterations. Lower = better.
  This is the most important number for throughput.
- **NS**: Number of overlapping iterations in flight. More stages =
  more overlap, but also more prologue/epilogue overhead and higher
  register pressure.
- **Loop**: The basic block that contains the pipelined kernel.
- **Prologue / PrologueBundles**: Where the prologue (pipeline ramp-up)
  was placed and how many VLIW bundles it costs. This is one-time
  setup overhead before the loop enters steady state.
- **Epilogue / EpilogueBundles**: Where the epilogue (pipeline drain)
  was placed and how many VLIW bundles it costs.

**If pipelining fails**, no `!Passed` remark is emitted for that loop.
The absence of a remark indicates the pipeliner could not find a
schedule. Common causes:
- Minimum trip count too low for the stage count (add `AIE_LOOP_RANGE`)
- Resource conflicts that prevent any valid II
- Register pressure too high (loop body too complex)

### Evaluating pipeline quality
Use the remark fields to compute the total cycle cost of the loop:

```
total_cycles = PrologueBundles + (trip_count * II) + EpilogueBundles
```

For high trip counts, the prologue and epilogue are amortized and II
dominates. For small trip counts, the overhead matters more.

**Is my II good?** Compare the achieved II against the Minimum
Initiation Interval (MII):
- **Resource MII**: Count the bottleneck resource. For example, if the
  loop body needs 8 loads and the architecture has 2 load ports,
  Resource MII >= 4.
- **Recurrence MII**: Determined by feedback chains (e.g., accumulator
  dependency loops).
- **MII** = max(Resource MII, Recurrence MII).
- An achieved II equal to MII is optimal. II = MII + 1 or +2 is
  typical. II >> MII suggests a scheduling bottleneck worth
  investigating.

**Is the prologue/epilogue cost acceptable?** Large prologue/epilogue
bundles indicate many pipeline stages. If the trip count is small, the
overhead may negate the pipelining benefit. Consider:
- Using `VERSIONED_LOOP` to avoid pipelining for low trip counts
- Reducing loop body complexity (fewer instructions per iteration)
- Checking if `AIE_LOOP_RANGE` accurately reflects the minimum trip
  count

### Hardware loop remarks
The `aie-hardware-loops` pass reports whether each loop was converted
to a zero-overhead loop (ZOL):

```yaml
--- !Analysis
Pass:            aie-hardware-loops
Name:            analysis
Function:        foo
Args:
  - LoopID:          '0'
  - BasicBlock:      for.body
  - Zero-Overhead-Loop: 'true'
```

ZOL conversion eliminates the branch instruction at the end of the
loop body, freeing a VLIW slot and removing the branch latency. This
is both a PM and a performance improvement.

The compiler generates a hardware loop when the minimum iteration
count is known (via `AIE_LOOP_RANGE`). When the minimum is known and
greater than 1, the compiler can also remove the loop guard (the
branch that skips the loop when the trip count is zero), saving
additional PM and cycles.

**Current limitation**: the loop guard is only removed when the
minimum iteration count is strictly greater than 1. Setting
`AIE_LOOP_RANGE(2,)` or higher enables this optimization.

If `Zero-Overhead-Loop` is `false` or the remark is absent, the loop
uses a software branch. Common reasons ZOL conversion fails: the loop
has multiple exits, or the trip count cannot be determined.

### Assembly-level remarks
The `aie-asm-printer` pass reports the code size of each basic block:

```yaml
--- !Analysis
Pass:            aie-asm-printer
Name:            analysis
Function:        foo
Args:
  - BasicBlock:      for.body
  - BundleCount:     '4'
  - ByteCount:       '64'
```

- **BundleCount**: Number of VLIW bundles (one bundle = one cycle).
  For a pipelined loop, the BundleCount of the loop body equals II.
- **ByteCount**: Total instruction bytes. Useful for tracking program
  memory (PM) usage.

### Combining remarks for full picture
For a complete performance picture, enable all three remark passes and
read them together:

1. **aie-hardware-loops**: Is it a ZOL? (eliminates branch overhead)
2. **postpipeliner**: What II / NS was achieved? What is the
   prologue/epilogue cost?
3. **aie-asm-printer**: What is the BundleCount of each block?
   (confirms II and shows non-loop overhead)

Example workflow:
```bash
llc -pass-remarks-output=- \
    -pass-remarks-filter='postpipeliner|aie-hardware-loops|aie-asm-printer' \
    -mtriple=aie2p my_kernel.ll -o my_kernel.s
```
Then grep for `!Passed` to find pipelined loops and check their II.

### aiesim cycle-accurate verification
After optimizing, verify with cycle-accurate simulation:
```bash
make run XPART=<device> TARGET=aiesim
```
Check the cycle count report and compare with the theoretical optimum:
```
optimal_cycles = PrologueBundles + (trip_count * II) + EpilogueBundles
```
If the measured cycle count significantly exceeds this, look for
stalls caused by memory bank conflicts, lock contention, or DMA
latency that the remarks don't capture.


### Backend remark passes and how to view them

The AIE backend currently emits remarks from four passes. Each remark
has a kind -- `Passed` (transformation applied), `Missed` (missed
opportunity), or `Analysis` (informational) -- and is surfaced by a
matching flag: `-Rpass=`, `-Rpass-missed=`, or `-Rpass-analysis=` for
a clang kernel build, with the equivalent `-pass-remarks=`,
`-pass-remarks-missed=`, `-pass-remarks-analysis=` for `llc`. The
earlier subsections show the llc / YAML workflow; the same remarks
are reachable directly from a clang-driven kernel build using the
flags in the last column below.

| Pass                    | Name                  | Kind     | Reports                                                                | Flag to enable                            |
|-------------------------|-----------------------|----------|------------------------------------------------------------------------|-------------------------------------------|
| `postpipeliner`         | `schedule`            | Passed   | II, NS, prologue/epilogue bundles when a schedule is found             | `-Rpass=postpipeliner`                    |
| `postpipeliner`         | `schedule`            | Missed   | Reason pipelining failed (e.g. `Longest circuit does not fit II`)      | `-Rpass-missed=postpipeliner`             |
| `aie-hardware-loops`    | `analysis`            | Analysis | Per-loop ZOL conversion result (`Zero-Overhead-Loop: true/false`)      | `-Rpass-analysis=aie-hardware-loops`      |
| `aie-asm-printer`       | `analysis`            | Analysis | Per-block `BundleCount` and `ByteCount`                                | `-Rpass-analysis=aie-asm-printer`         |
| `aie-multi-slot-pseudo` | `missing-memory-bank` | Missed   | Load reached the multi-slot materializer without a DM bank annotation  | `-Rpass-missed=aie-multi-slot-pseudo`     |

`-Rpass-missed` is the only way (besides the YAML record) to see
`postpipeliner`'s "no schedule found" / "longest circuit does not
fit II" remarks -- they are not emitted as warnings.

For the YAML record (recommended for CI / postmortem analysis),
clang exposes `-fsave-optimization-record`,
`-foptimization-record-file=<path>`, and
`-foptimization-record-passes=<regex>`; these write the same YAML
format as `llc -pass-remarks-output=` documented earlier, and the
`-passes=` flag corresponds to `llc -pass-remarks-filter=`.

Recommended kernel-dev defaults:

```bash
clang ... \
  -Rpass=postpipeliner \
  -Rpass-missed='postpipeliner|aie-multi-slot-pseudo' \
  -Rpass-analysis='aie-hardware-loops|aie-asm-printer'
```

### Warnings channel

Warnings reach the build log without any remark flag.

- **`-Wpass-failed`** (on by default): emitted by the IR-level
  optimizer when an explicitly requested transformation cannot be
  applied. This is what fires when a `#pragma clang loop` directive
  (or an `AIE_*` macro that expands to one) is dropped, e.g.
  `loop not vectorized: the optimizer was unable to perform the
  requested transformation; the transformation might be disabled or
  specified as part of an unsupported transformation ordering`.
  Treat any `-Wpass-failed` warning as evidence that a pragma was
  silently ignored.
- **AIE backend warnings via `WithColor::warning`**: the AIE backend
  also writes warnings directly to stderr, independent of `-Rpass*`
  and of `-Wpass-failed`. The first such warning in tree is the
  multi-slot materializer's missing-memory-bank warning, covered in
  the next subsection.

Note: `postpipeliner` missed schedules are **only** on the remarks
channel -- they do not warn by default. Use
`-Rpass-missed=postpipeliner` (or the YAML record) to see them.

### Missing memory bank annotations (`aie-multi-slot-pseudo`)

**What slot materialization does.** A multi-slot pseudo is a load
opcode that has not yet been bound to a specific issue slot. The
`aie-multi-slot-pseudo` pass picks a concrete slot-bound opcode for
each such load before the post-RA pipeliner runs. Picking the right
slot matters because two loads in the same VLIW bundle must use
different load slots, and each load slot has affinity to specific
memory banks. The materializer uses the load's DM bank annotation
to map it to a slot whose ports match that bank, so that consecutive
loads on different banks can issue in parallel.

**What this hint means.** It fires when the materializer encounters
a load whose pointer carries no DM bank annotation (see Section 3
for the qualifier list). It is a **missed-opportunity** hint, not a
functional failure: the materializer still picks a slot-bound opcode
for the load, but it has to fall back to a bank-blind heuristic. 
The chosen slot is often suboptimal
-- the load lands in the wrong slot and blocks the load ordering
the pipeliner would otherwise use, raising the achieved II.

What the kernel author sees:

- Always, on stderr (no flag required):
  ```
  warning: No memory bank assigned to load in function '<fn>',
  block '<bb>' at <file>:<line>:<col>: <MI dump>
  ```
- With `-Rpass-missed=aie-multi-slot-pseudo` (or in the YAML record),
  a `!Missed` remark anchored to the same debug location:
  ```yaml
  --- !Missed
  Pass:            aie-multi-slot-pseudo
  Name:            missing-memory-bank
  Function:        <fn>
  Args:
    - String: "No memory bank assigned to load in function '<fn>', block '<bb>' at <loc>"
  ```

How to fix: annotate the offending pointer with a DM bank qualifier
(`__aie_dm_resource_a` / `_b` / `_c` / `_d`) -- see Section 3 for
the qualifier list and parallel-access rules. If the load comes from
a helper or cast, check that the bank qualifier is preserved through
every intermediate pointer and is not stripped by a cast (the same
pitfall as Section 2 "Casting away restrict" applies to bank
qualifiers).

Ignoring the hint will not break the kernel, but the suboptimal slot
choice typically shows up later as a higher achieved II in the
`postpipeliner` remark, or as a `BundleCount` that exceeds the
resource MII reported by `aie-asm-printer`.


## 15. Optimization Checklist

Use this checklist when writing or reviewing kernel code:

1. All pointer parameters use `__restrict` where they don't alias
2. No pointer casts strip `__restrict` or DM bank qualifiers
3. Core loop function is `NOINLINE`; public wrapper is `INLINE`
4. Innermost loop has `AIE_LOOP_RANGE` with accurate minimum trip
   count
5. `AIE_PREPARE_FOR_POSTPIPELINING` is present on pipelined inner
   loops
6. DM bank annotations match the data flow (inputs and outputs on
   different banks for parallel access)
7. Variable trip count loops use `VERSIONED_LOOP` or manual
   versioning
8. Post-increment addressing (`*ptr++`) preferred over indexed
   access (`ptr[i]`) in hot loops
9. Accumulator structs use and proper `alignas`
10. Optimization hints checked: achieved II compared with target and
    ZOL conversion confirmed for each pipelined inner loop; scan
    the build log for `-Wpass-failed` warnings and any
    `aie-multi-slot-pseudo` missing-memory-bank warnings, and review
    `-Rpass-missed=postpipeliner` output for missed schedules
11. Parameter structs passed by `__restrict` reference
12. `AIE_*` macros used instead of raw `#pragma clang loop`
13. `std::make_tuple` uses explicit template types where type
    deduction is ambiguous
14. `AIE_LOOP_HINT` used for per-loop compiler tuning when global
    defaults are suboptimal
