# Outer-loop pipelining — AA classification for the conv2d and gemm tests

This note summarises the alias-analysis (AA) situation in the two
outer-loop-pipelined end-to-end tests
`conv2d_int8_outerloop_pipelined.ll` and `gemm_int8_outerloop_pipelined.ll`,
after adding `noalias` to every pointer-typed function argument. Focus
is on the outer-loop **epilogue/cleanup MBB** — the block that drains
the inner-loop accumulators, stores the result, and re-primes the buffer
pointers for the next outer iteration. This is where the pipeliner most
needs AA to permit overlap.

## TL;DR

| test | epilogue MBB | memops | nontrivial pair classes after `noalias` |
|---|---|---|---|
| conv2d | `for.cond.cleanup158.i` | 8 (4 stores, 4 loads) | B → **NoAlias**; D → **MayAlias** |
| gemm   | `for.cond.cleanup99`    | 18 (4 stores, 14 loads) | A → NoAlias (addrspace); B → **NoAlias**; C → NoAlias (constant Δ); D → **MayAlias** |

In both tests the four `nop` cycles between the SRS stores and the next
outer iteration's reloads disappear after `noalias`. Output stores
share bundle slots with the next iteration's MAC prologue / FIFO and
psum reloads.

## AA layers and ptr-add chain tracking

For every pair we walk the following AA layers in order and record the
first one that returns NoAlias (if any). Subsequent layers can only
narrow to NoAlias, never upgrade to MustAlias, so the first NoAlias
wins.

1. **TBAA** — both tests use only `omnipotent char` (`!tbaa !4`).
   Returns MayAlias on every pair, useless.
2. **ScopedNoAlias** — only conv2d carries useful `!alias.scope` /
   `!noalias` metadata on the output stores; gemm has none.
3. **BasicAA**
   - `aliasGEP` decomposes a same-base GEP pair into
     `(constant_offset, [(scaled_variable, scale)*])` and proves
     NoAlias when `|constant_offset| ≥ access_size + variable_residue`.
     If the variable residue is non-empty it returns MayAlias.
   - `aliasPHI` recurses pairwise on PHI incomings.
   - Address-space disjointness check returns NoAlias whenever the
     two pointers live in distinct address spaces.
4. **AIEBaseAA**
   (`llvm/lib/Target/AIE/AIEBaseAliasAnalysis.cpp`) — the ptr-add chain
   tracker. Two distinct pieces:
   - `getUnderlyingObjectAIE` looks through `aie2ps.add.{2,3}d` by
     replacing `extractvalue (aie2ps.add.{2,3}d(ptr, ...), 0)` with the
     intrinsic's `ptr` operand. Used to find the phi/argument root.
   - `aliasAIEIntrinsic` → `trackPtrUpdateChain` →
     `PtrUpdateChainInfo::mayOverlap` walks each pointer back from its
     phi's latch input, counting `aie2ps.add.{2,3}d` and non-zero
     `getelementptr` updates. It proves NoAlias only when both
     pointers share the same root, same stride/parameter operands,
     same starting counters, and **different** update counts.
     **Bails out** (returns `nullopt` ⇒ MayAlias) on:
     - chains that mix `aie2ps.add.{2,3}d` with non-zero
       `getelementptr` (lines 362–366 and 409–412);
     - chains whose external phi-incoming is not equal nor
       structurally lockstep (`isLockStepGEPChain`).

We classify each pair into one of:

- **A** — cross-family, cross-addrspace. Decided by BasicAA's
  addrspace check.
- **B** — cross-family, same addrspace, both bases rooted at distinct
  function-arg pointers. **This is the only category that argument
  `noalias` flips.** Decided by BasicAA's `aliasPHI` recursion.
- **C** — intra-family, constant GEP delta ≥ access size. Decided by
  BasicAA's `aliasGEP` decomposed-offset check.
- **D** — intra-family, symbolic GEP delta or symbolic stride
  descriptor. Decided by AIEBaseAA's ptr-add chain tracker — when it
  succeeds. On these tests it **bails** due to the mixed-chain rule,
  so the result falls back to MayAlias.

## conv2d — `for.cond.cleanup158.i`

8 memops, 28 unordered pairs.

| family | members | offset shape | addrspace |
|---|---|---|---|
| output stores (×4) | `%p_out`, `+%idx.ext.i342`, `+%idx.ext.i344`, `+%idx.ext.i344+%idx.ext.i342` | all symbolic | default (0) |
| psum loads (×4) | along an `aie2ps.add.3d` + GEP chain off `%cond` | all symbolic | default (0) |

### Group conv2d-B — output store ↔ psum load (16 pairs)

- TBAA: MayAlias.
- ScopedNoAlias: stores have `!alias.scope = !35 D:%output` and
  `!noalias = !38 {D:%input, D:%weights, D:%acc_in}`. Loads have no
  `!alias.scope` and only inert `A:`/`B:`/`C:` `!noalias` lists. The
  store's `D:%acc_in`-noalias does not fire on an unannotated load →
  MayAlias.
- BasicAA `aliasPHI` recurses on `(%p_out.0423.i, %p_init16.0424.i)`:
  - external pair `(%cond.i, %cond)` — two distinct `noalias` args →
    **NoAlias**.
  - latch pair reduces (through `getUnderlyingObjectAIE`'s look-through
    of `aie2ps.add.3d` and `aie2ps.add.2d`) to GEPs rooted at those
    same `noalias` args → NoAlias.
  - combined: NoAlias.
- AIEBaseAA: not consulted (BasicAA already returned NoAlias).
- **Result: NoAlias** for all 16 pairs.

### Group conv2d-D — output store ↔ output store (6 pairs)

- TBAA: MayAlias.
- ScopedNoAlias: both ops are in the same scope `!35 D:%output` and
  share the same `!noalias = !38`. Same-scope reciprocity is silent →
  MayAlias.
- BasicAA `aliasGEP`: same underlying phi `%p_out.0423.i`, residues
  are `±%idx.ext.i342.i` and/or `±%idx.ext.i344.i` (i20 function
  args, unbounded) → MayAlias.
- AIEBaseAA `aliasAIEIntrinsic`: both bases are the same phi, external
  incomings are equal (`%cond.i`). `trackPtrUpdateChain` walks the
  latch:
  `%57 = extractvalue aie2ps.add.2d(...)` ⇒ `FoundIntrinsicChain = true`,
  next step is `gep i8 %add.ptr.i345.i, +%idx.ext.i342.i` (non-zero
  GEP after intrinsic) → mixed-chain bail → `nullopt` → MayAlias.
- **Result: MayAlias** for all 6 pairs. Store-store ordering is
  naturally enforced by program order in the emitted asm.

### Group conv2d-D — psum load ↔ psum load (6 pairs)

- TBAA: MayAlias.
- ScopedNoAlias: no `!alias.scope` on any load; their `A:`/`B:`/`C:`
  `!noalias` lists have no partner scope and are inert → MayAlias.
- BasicAA `aliasGEP`: same underlying phi `%p_init16.0424.i`, residues
  symbolic → MayAlias.
- AIEBaseAA: same mixed-chain bail as the store group → MayAlias.
- **Result: MayAlias** for all 6 pairs. Read/read, no scheduling cost.

### Summary table — conv2d

| group | pair count | result | deciding layer |
|---|---|---|---|
| B (S ↔ L) | 16 | **NoAlias** | BasicAA `aliasPHI` over `noalias` args |
| D (S ↔ S) | 6 | MayAlias | every layer bails |
| D (L ↔ L) | 6 | MayAlias | every layer bails |

### Schedule effect

Before `noalias` the block emitted four explicit `nop` cycles between
the MAC drain and the SRS stores, and the four next-iteration psum
reloads strictly followed the stores. After `noalias` the reloads
(`vlda.ups.2x cml1/cmh1/cml0/cmh0, ..., [p1]...`) move up and share
bundle slots with the stores (`vst.srs.4x cmh1/cml1, ..., [p2,...]`).
The four `nop` cycles are gone.

## gemm — `for.cond.cleanup99`

18 memops, more pairs. Three address spaces are in play.

| family | addrspace | members | offset shape |
|---|---|---|---|
| C stores (×4) | 6 | `%p_c`, `+64`, `+%idx.ext.i478`, `+%idx.ext.i478+64` | mix |
| bias loads (×2) | 6 | `%74 = add.2d(%p_bias,...)`, `+16` | constant |
| mat_a loads (×2) | 5 | `%1`, `+64` | constant |
| mat_b loads (×2) | 5 | `%2`, `+%idx.ext.i` | symbolic |
| psum loads (×8) | 7 | `aie2ps.add.3d` + GEP chain off `%p_psum` | mix |

### Group gemm-A — cross-family, cross-addrspace

All pairs where the two ops live in different address spaces:
C-store (as6) ↔ mat_a/mat_b (as5), C-store (as6) ↔ psum (as7),
bias (as6) ↔ mat_a/mat_b (as5), bias (as6) ↔ psum (as7),
mat_a/mat_b (as5) ↔ psum (as7).

- TBAA: MayAlias.
- ScopedNoAlias: gemm has none → MayAlias.
- BasicAA: address-space disjointness short-circuit → **NoAlias**.
- AIEBaseAA: not consulted.
- **Result: NoAlias.** Was already NoAlias before `noalias`; argument
  attribute is not what decides this group.

### Group gemm-B — C-store ↔ bias-load (both addrspace 6) (8 pairs)

- TBAA: MayAlias.
- ScopedNoAlias: neither op carries `!alias.scope` / `!noalias` → MayAlias.
- BasicAA `aliasPHI` recurses on `(%p_mat_c.0.in763, %p_bias_in.0.in762)`:
  - external pair `(%p_c, %p_bias)` — two distinct `noalias` args →
    **NoAlias**.
  - latch pair reduces (via `aie2ps.add.2d` look-through) to GEPs
    rooted at the same `noalias` args → NoAlias.
- AIEBaseAA: not consulted.
- **Result: NoAlias** for all 8 pairs. *Flipped by `noalias`.*

### Group gemm-B — mat_a-load ↔ mat_b-load (both addrspace 5) (4 pairs)

- TBAA: MayAlias.
- ScopedNoAlias: the loads carry inert `!noalias` lists, no
  `!alias.scope` → MayAlias.
- BasicAA `aliasPHI` recurses to `(%1, %2)` — two distinct `noalias`
  args → **NoAlias**.
- AIEBaseAA: not consulted.
- **Result: NoAlias** for all 4 pairs. *Flipped by `noalias`.*
  Read/read, no scheduling cost; cosmetic improvement only.

### Group gemm-C — bias-load ↔ bias-load (1 pair)

- TBAA: MayAlias.
- BasicAA `aliasGEP`: same base `%74`, Δ = 16, access size = 16
  → **NoAlias**.
- **Result: NoAlias.**

### Group gemm-C — mat_a-load ↔ mat_a-load (1 pair)

- BasicAA `aliasGEP`: same base `%1`, Δ = 64, access size = 64
  → **NoAlias**.
- **Result: NoAlias.**

### Group gemm-C — C-store ↔ C-store with constant Δ (2 pairs)

Pairs `(S₁, S₂) = (+0, +64)` and `(S₃, S₄) = (+%idx.ext.i478, +%idx.ext.i478+64)`:

- BasicAA `aliasGEP`: same base `%p_mat_c.0.in763`, residue is the
  constant `±64`, access size = 64 → **NoAlias**.
- **Result: NoAlias.**

### Group gemm-C — psum-load ↔ psum-load with constant Δ (some subset)

Same-base psum pairs whose entire delta reduces to the GEP `+64`
inside one outer iteration.

- BasicAA `aliasGEP`: Δ = 64, access size = 64 → **NoAlias**.
- **Result: NoAlias.**

### Group gemm-D — C-store ↔ C-store with `%idx.ext.i478` residue (4 pairs)

The four cross pairs `{S₁, S₂} × {S₃, S₄}` where Δ contains
`±%idx.ext.i478`:

- TBAA: MayAlias.
- BasicAA `aliasGEP`: residue `±%idx.ext.i478` — `i20` function arg,
  unbounded → MayAlias.
- AIEBaseAA `aliasAIEIntrinsic`: same phi root, external incomings
  equal (`%p_c`). `trackPtrUpdateChain` from latch
  `%69 = extractvalue aie2ps.add.2d(...)` →
  `gep i8 %add.ptr.i479, +64` (non-zero GEP after intrinsic) →
  mixed-chain bail → MayAlias.
- **Result: MayAlias.**

### Group gemm-D — mat_b-load ↔ mat_b-load (1 pair)

- BasicAA `aliasGEP`: same base `%2`, residue `+%idx.ext.i` (i20 fn
  arg) → MayAlias.
- AIEBaseAA: same root, but the load's pointer is a plain GEP off the
  phi; `trackPtrUpdateChain` from latch (`%p_mat_b.1` =
  `extractvalue aie2ps.add.3d(...)`) walks intrinsic + non-zero GEP →
  mixed-chain bail → MayAlias.
- **Result: MayAlias.**

### Group gemm-D — psum-load ↔ psum-load crossing an `add.3d` stride (some pairs)

The psum load chain mixes `aie2ps.add.3d` updates with non-zero GEPs;
any pair whose delta spans an `add.3d` step has a symbolic component.

- BasicAA `aliasGEP`: residue contains symbolic stride
  parameter(s) → MayAlias.
- AIEBaseAA: mixed-chain bail → MayAlias.
- **Result: MayAlias.** Read/read, no scheduling cost.

### Summary table — gemm

| group | pair count (approx) | result | deciding layer |
|---|---|---|---|
| A (cross-as) | many | NoAlias | BasicAA addrspace |
| B (S ↔ L, as6) | 8 | **NoAlias** | BasicAA `aliasPHI` over `noalias` args |
| B (L ↔ L, as5) | 4 | **NoAlias** | BasicAA `aliasPHI` over `noalias` args |
| C (bias·bias) | 1 | NoAlias | BasicAA `aliasGEP` (Δ=16) |
| C (mat_a·mat_a) | 1 | NoAlias | BasicAA `aliasGEP` (Δ=64) |
| C (C·C const-Δ) | 2 | NoAlias | BasicAA `aliasGEP` (Δ=64) |
| C (psum·psum const-Δ) | subset | NoAlias | BasicAA `aliasGEP` (Δ=64) |
| D (C·C with `%idx.ext.i478`) | 4 | MayAlias | mixed-chain bail |
| D (mat_b·mat_b) | 1 | MayAlias | mixed-chain bail |
| D (psum·psum sym-Δ) | rest | MayAlias | mixed-chain bail |

### Schedule effect

Before `noalias`: bias loads and mat_a/mat_b reloads were strictly
after the C stores, with `nop` padding. After `noalias`:

- bias loads `vldb.128 wl6, [p4, #0]` / `vldb.128 wl3, [p4, #16]` move
  up into the MAC-drain region;
- mat_a/mat_b reloads `vlda x10, [p1]` / `vlda x8, [p0]` /
  `vldb.3d x5, [p1]` / `vldb.3d x1, [p0]` likewise overlap with the
  drain;
- the eight `vlda.ups.2x cml3..cmh0, [p6]` psum reloads pack tightly
  together;
- the four `vst.srs.4x dm3..dm0, [p3]` stores are now bundled with
  next-iteration `vaddmac` MAC-prologue ops and pushed into the `jnz`
  delay slots.

## Why Category D survives `noalias`

`%idx.ext.i478` (gemm output row stride) and the conv2d analogues
`%idx.ext.i342.i` / `%idx.ext.i344.i` are `i20` **function arguments**
used exclusively as `getelementptr` byte indices. The 2D/3D iterator
stride descriptors (`%23..%25`, `%26..%28`, `%3..%7`, `%8..%12`) are
**separate** function arguments. Argument-level `noalias` says nothing
about either kind of value.

The AIEBaseAA ptr-add chain tracker is the layer designed to
disambiguate intra-family Cat-D pairs, but on these tests it bails for
two reasons:

1. **Mixed-chain bail.** The walk from a phi's latch back to the phi
   hits both `aie2ps.add.{2,3}d` (`FoundIntrinsicChain = true`) and
   non-zero `getelementptr` (`FoundGEPChain = true`) on the same path,
   which `trackPtrUpdateChain` rejects.
2. **No cross-parameter equality reasoning.**
   `PtrUpdateChainInfo::hasSameParameters` compares SSA-operand
   equality, not numerical equality. Even with the bail lifted, the
   GEP-step `%idx.ext.i478` and the `add.2d` strides
   `(%23, %24, %25)` are different SSA values, so the tracker has no
   way to relate them.

## Removal paths for Category D

In decreasing order of practical impact:

1. **Caller-side specialization.** Once the per-tile strides
   (`%idx.ext.i*`, `%3..%12`, `%23..%28`) are constants known to the
   compiler, BasicAA disambiguates everything in Cat D directly. The
   `noinline` on `@gemm` / `@conv2d` in these tests is what suppresses
   this in production-like settings.
2. **Per-lane `!alias.scope` annotations.** Give each intra-family
   memop its own scope within a shared domain (`D:%output.lane0..3`
   etc.) and put the sibling lanes in each op's `!noalias` list.
   Sidesteps offset reasoning entirely.
3. **Lifting the mixed-chain bail in
   `AIEBaseAA::trackPtrUpdateChain` + adding inequality reasoning in
   `mayOverlap`.** Allows AIEBaseAA to prove NoAlias on chains that
   look like "same phi, two paths differing only in GEP-index or in
   number of `add.{2,3}d` steps", which captures the Cat-D pairs that
   share a family root.

Argument-level `noalias` ("restrict" handling) addresses none of these
three; it only flips Cat B. That is the right intervention for the
outer-loop pipeliner today because Cat B contains the only
`store ↔ load` MayAlias edges on the critical path through the
epilogue. The remaining Cat-D MayAlias edges are either store-store
(naturally serialised by program order) or read-read (no scheduling
cost), so the schedule actually emitted matches what one would obtain
with full intra-family disambiguation.

## Files

| file | change |
|---|---|
| `llvm/test/CodeGen/AIE/aie2ps/end-to-end/conv2d_int8_outerloop_pipelined.ll` | `noalias` on `%add.ptr3`, `%cond`, `%cond.i`, `%ifm`; CHECK regenerated |
| `llvm/test/CodeGen/AIE/aie2ps/end-to-end/gemm_int8_outerloop_pipelined.ll`   | `noalias` on `%1`, `%2`, `%p_psum`, `%p_c`, `%p_bias`; CHECK regenerated |

## AA reference

- `llvm/lib/Target/AIE/AIEBaseAliasAnalysis.cpp`
  - `getUnderlyingObjectAIE` — looks through `aie2ps.add.{2,3}d`
  - `aliasAIEIntrinsic` — phi-rooted same-origin disambiguator
  - `trackPtrUpdateChain` / `PtrUpdateChainInfo::mayOverlap` — ptr-add
    chain tracker
  - `aliasAcrossVirtualUnrolls` — post-RA scheduler entry point
