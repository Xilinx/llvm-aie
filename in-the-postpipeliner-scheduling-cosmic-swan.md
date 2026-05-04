# Generalize the post-pipeliner's "fixed region" scoreboard scheduling to all MBBs

---

## Current plan: Phase 1 — `pushTopFixedToPredecessor` outer-SWP push primitive

### Status going into this task

* **Engine work (Phase 1 Steps 1–4)** is committed:
  - `FixedRegionScoreboardScheduler` engine.
  - Top/Bot fixed conflict-resolution shift loop.
  - `unionInto`, `hasInternalConflict`, `conflictBetween` helpers.
  - Engine + helper unit tests (39/39 passing).
  - PostPipeliner migrated to engine; AIEMachineScheduler
    Top/Bot priming routed through engine in borrow mode.
* **Delay-slot migration (Phase 1 Step 5, commit `6e395bbae324`)**
  is committed and live. It treats delay-slot branches (RET, JL,
  JL_IND, JZ, JNZ, JNZD, J_jump_imm, J_jump_ind) as single-entry
  bot-fixed SUs anchored by an artificial chain edge
  `branch → ExitSU` lat=N=`getNumDelaySlots()`. 138 tests
  regenerated to absorb schedule changes.
* **Known regressions deferred to a follow-up**:
  - `aie2/end-to-end/Add2D-red.ll`, `aie2/bfloat16.ll bxor_v32bf16`,
    `aie2/fadd.ll`, `aie2/fsub.ll`, `aie2/float_to_bfloat16.ll`,
    `aie2/conv2d_offset_test.ll`, `aie2/extract.ll`,
    `aie2/intrinsics-128bit.ll`, `aie2/ra/tie-subregs-flow-3d.mir`,
    `aie2/schedule/loopaware/loop-epilogue.mir` — all show the
    same Pattern A: a free SU shifts from delay slot 1 (= bot 0
    = MBB last cycle) to delay slot 2; NOP appears at DS1; +1
    cycle.
  - Mechanism: Conservative band's `blockResources()` at
    scoreboard offset `[1, D]` (post-MBB cycles) intersects the
    pipeline-tail FU stages of free SUs landing at bot 0. Free
    SUs get pushed to bot 1; cascade through Pred-edge BotReady
    propagation pushes everything up by 1.
  - The fix space (Variant 2a — relax band for function-exit;
    Variant 2a-extended — flip `hasUnknownSuccessors` for
    `succ_empty()`; Phase 2 projection scoreboards) is
    documented in the "Variant 2a investigation" section
    further down. Not implementing now.

### What this task delivers

Implements `pushTopFixedToPredecessor`, the outer-SWP push
primitive. This enables an outer-SWP loop to relocate the
bottom-most cycle of its TopInsert (epilogue) into a
predecessor's TopInsert, exposing additional schedule slots in
the SWP-pushed prologue/epilogue boundary.

### Design (from existing plan)

```cpp
// In AIEInterBlockScheduling.{h,cpp}.
//
// Move the bottom-most cycle of TopInsert in `From` to the bottom of
// TopInsert in `To` (a predecessor of `From`). Returns false if the
// resulting layout cannot be made conflict-free within MaxShift cycles
// of Top/Bot conflict-resolution shift.
bool pushTopFixedToPredecessor(BlockState &From, BlockState &To,
                               int MaxShift = 4);
```

Drives the engine's Top/Bot conflict-resolution shift loop on
`To` after each push. The "if the push collides with any
existing top fixed [...]" requirement is the same shift loop
reused — no separate push-only conflict resolver.

### Critical files

* `llvm/lib/Target/AIE/AIEInterBlockScheduling.h` — declare
  `pushTopFixedToPredecessor` on `InterBlockScheduling` class
  (or as a free function in the AIE namespace, depending on
  call-site needs).
* `llvm/lib/Target/AIE/AIEInterBlockScheduling.cpp` —
  implementation. Walks the relocation logic:
  1. Identify the bottom-most cycle of `From.TopInsert` (the
     last bundle).
  2. Construct a target shape for `To.TopInsert`: append that
     bundle.
  3. Use `FixedRegionScoreboardScheduler` to verify the merged
     `To.TopInsert + To.BotInsert + To.RegionBottomFixed`
     fits without conflicts (drives `primeAllRegions` +
     conflict-resolution shift loop).
  4. On success: commit the move (mutate `From` and `To` lists,
     update affected MIR positions if needed, invalidate any
     cached projections / scoreboards on `From` / `To`).
  5. On failure: leave inputs unchanged; return false.
* `llvm/lib/Target/AIE/AIEFixedRegionScoreboardScheduler.{h,cpp}`
  — engine API extension if needed: a query that takes a
  candidate top-fixed shape and returns whether it fits.
  Probably reusable via existing `primeAllRegions`.
* `unittests/Target/AIE/InterBlockSchedulingPushTest.cpp` (new)
  — unit tests:
  - `PushTopFixedHelper`: 3 successive successful pushes.
  - `PushTopFixedRefusal`: scoreboard conflict; assert input
    unchanged on failure.

### Verification

1. `ninja llc` (assertions on).
2. **Unit tests**: build and run
   `unittests/Target/AIE/AIETests`. Expect 41/41 (39 existing +
   2 new). All new tests verify push semantics without driving
   a full codegen pipeline.
3. **Codegen identity**: `ninja check-llvm-codegen-aie` must
   stay at 2145/2153 PASS. The new helper has no callers in
   this PR so it should be NFC for the codegen suite.
4. **Drive-by smoke test**: write a small test that exercises
   `pushTopFixedToPredecessor` from a debug-only entry point
   (e.g., a cl::opt-gated invocation in InterBlockScheduling) on
   a known SWP MBB pair. Verify the relocated MIR is correct.
   Skip if too invasive for this PR; rely on unit tests.

### Implementation order

1. **Step 1**: declare API in
   `AIEInterBlockScheduling.h`. Stub body returns `false` so it
   compiles. Add to CMakeLists if needed.
2. **Step 2**: implement core logic. Identify the move source/
   target, drive the engine's verify path, mutate state.
3. **Step 3**: write unit tests.
4. **Step 4**: build, run unit tests, run codegen suite.
5. **Step 5**: commit as a NFC-on-codegen-suite addition. The
   helper is unused by callers in this commit; a separate
   future change will wire it in for outer-SWP loops.

### Out of scope

* Outer-SWP policy code that decides *when* to call
  `pushTopFixedToPredecessor` and the driver that invokes it
  after innermost loops are scheduled. Phase 1 only delivers
  the primitive + tests. The policy / driver follows.
* Phase 2 projection scoreboards (separate PR).
* Regression fixes documented above (deferred).

### After this task

Phase 1 follow-ups (per task list):
* Phase 1 codegen tests (refactor-engine-identity sentinel,
  delay-slot-via-fixed-bottom).
* Final Phase 1 verification.

---

## Deferred: Variant 2a — relax the Conservative band for function-exit MBBs

### Context (latest finding)

Earlier investigation passes worked through several incorrect
hypotheses (kept below for reference). The most recent trace
(`test_fadd_bfloat`, default issue) pinpointed the regression cause:

1. **Conservative band IS already at post-MBB cycles**. After
   `initializeBotScoreBoard`'s priming + `AlignScoreboardToCycleOne`
   (recede by `Depth+1`), the band lives at scoreboard offsets
   `[1, D]` where `D = pipelineDepth`. Offset `0` represents the
   MBB's last cycle (within-MBB); offsets `[1, D]` represent
   "post-MBB" cycles (the unknown next MBB / caller code).

2. **mov_scl's pipeline stage at cycle 1** crosses the boundary
   into the band. `checkConflict`'s `anyStage` walks mov_scl's
   itinerary stages and checks each at `scoreboard[DeltaCycles +
   stage_cycle]`. For mov_scl at `DeltaCycles=0` (= bot 0 = MBB
   last cycle), stage at `cycle=1` lands at `scoreboard[1]` =
   first post-MBB cycle = blocked by Conservative band's
   `blockResources()` (sets `Required`, `Reserved`, `Slots` to
   all-ones). Hazard. Scan falls through to `DeltaCycles=-1`
   (= bot 1), where stage at cycle 1 lands at `scoreboard[0]` =
   MBB last cycle = empty. No hazard. Sets
   `SU.BotReadyCycle = CurrCycle - DeltaCycles = 1`.

3. **Why this is a NEW regression, not a pre-existing one**:
   in OLD code (`6e395bbae324~1`), the same band, same mov_scl
   itinerary, same scoreboard offsets — yet the trace shows
   `Ready @0c` for SU(14). The schedule sequence differs because:
   * OLD: RET is a free SU. Both SU(14) and SU(15) RET arrive in
     `BotQ.A` (Available) at startup. The scheduler picks RET
     first via the delay-slot tie-break, then SU(14). At
     scheduling time, SU(14)'s BotReady is already 0 from initial
     release; the hazard scan path that bumps BotReady to 1
     doesn't execute because SU(14) was Available immediately.
   * NEW: RET is a fixed bot SU. The fixed-SU bypass at
     `AIEMachineScheduler.cpp:735-744`:
     ```cpp
     if (SUnit *FixedSU = getNextUnscheduledFixedInstr(Zone)) {
       if (FixedSU != &SU)
         return false;
       ...
     }
     ```
     forces SU(14) into Pending while RET is unscheduled. After
     RET pre-schedules, SU(14) is **re-evaluated** through the
     hazard scan, which finds the band-vs-tail conflict and bumps
     BotReady to 1.

### Why the user's question matters

> Why would RET block instructions inside its own MBB?

It doesn't — directly. The Conservative band is what blocks. But
the band represents "post-MBB" resource demand, and a free SU at
MBB last cycle has its **pipeline tail** extending into post-MBB.
The band's `blockResources()` is maximally conservative (all FUs
blocked), so any SU with a non-zero pipeline tail conflicts.

For **function-exit RET regions**, "post-MBB" is the caller's
code. AIE has no FU carryover across function calls (a return
ends the pipeline; the caller's resume executes in its own
pipeline state). The Conservative full-block band is **overly
conservative** for function-exit cases — there's no actual
hardware reason to block free-SU pipeline tails at function exit.

### Variant 2a — relax band priming for function-exit MBBs

**Change**: in `AIEPostRASchedStrategy::initializeBotScoreBoard`
(`AIEMachineScheduler.cpp:262-356`), when
`Trust == ScoreboardTrust::Conservative` AND
`CurMBB->succ_empty()`, skip the BlockCycle loop entirely. The
scoreboard is left with no band — function-exit MBBs treat
post-MBB as having no resource demand.

```cpp
void AIEPostRASchedStrategy::initializeBotScoreBoard(ScoreboardTrust Trust) {
  // ... existing setup ...
  int FirstBlockedCycle = 0;
  if (Trust != ScoreboardTrust::Conservative) {
    // ... existing successor-replay path ...
  } else if (CurMBB->succ_empty()) {
    // Function-exit MBBs (RET): "post-MBB" is the caller, which
    // runs with its own pipeline state. AIE has no cross-call FU
    // carryover, so the Conservative full-block band is overly
    // conservative here. Set FirstBlockedCycle = pipelineDepth so
    // the BlockCycle loop becomes a no-op (matches the empty
    // successor case in non-Conservative trust).
    FirstBlockedCycle = Engine.getPipelineDepth();
  }

  auto Cap = InterBlock.getBlockedResourceCap(CurMBB);
  if (Cap && IsBottomRegion) {
    int Margin = Engine.getPipelineDepth() - *Cap;
    FirstBlockedCycle = std::max(FirstBlockedCycle, Margin);
  }

  // Block cycles in [FirstBlockedCycle, Depth) — empty for function-exit.
  for (int Cycle = FirstBlockedCycle; Cycle < Depth; Cycle++) {
    BlockCycle(Cycle);
  }

  AlignScoreboardToCycleOne();
  // ...
}
```

The `Cap && IsBottomRegion` check still runs and may set a
non-zero `FirstBlockedCycle` from `getBlockedResourceCap` for
specific cases (SWP-related). That stays intact.

### Critical files

* `llvm/lib/Target/AIE/AIEMachineScheduler.cpp` —
  `initializeBotScoreBoard` (around line 262-356). Single
  conditional branch added.

### Implications

**Fixes**:
* All function-exit RET regressions: `bfloat16.ll bxor_v32bf16`
  (multi-issue path), `fadd.ll`, `fsub.ll`,
  `float_to_bfloat16.ll`, etc. — free SUs at bot 0 (DS1) no
  longer pushed to bot 1 by the band.

**Does NOT fix**:
* Mid-CFG delay-slot regressions (`tie-subregs-flow-3d.mir`,
  `loop-epilogue.mir`, `intrinsics-128bit.ll` with JZ/JNZ
  branches; `Add2D-red.ll` blocks 1 & 2 with JZ/J). Those have
  non-empty `MBB->successors()` — the band stays as before.
* `bxor_v32bf16` under `--issue-limit=1`: that case is a
  different mechanism (issue-limit forced serialization
  cascading SU(6) at bot 1 → SU(4) bot 2 → SU(3) bot 3 → SU(2)
  past delay slots). Not addressed by 2a.

### Add2D-red.ll classification (proves 2a is incomplete)

Default-issue `aie2/end-to-end/Add2D-red.ll` has THREE regressed
blocks, all same Pattern A (DS1 instruction → DS2, NOP at DS1):

| Block | Branch | Successor | 2a fixes |
|---|---|---|---|
| 1 | `JZ r7, .LBB0_2` | `.LBB0_2` (loop) | **NO** |
| 2 | `J #.LBB0_5` | `.LBB0_5` (cleanup) | **NO** |
| 3 | `RET` | (none) | **YES** |

To address ALL regressed cases (function-exit + mid-CFG), the
plan needs to extend beyond 2a's `succ_empty()` special case.

### Variant 2a-extended — bring forward Phase 2-lite

Two-tier band priming in `initializeBotScoreBoard`:

1. **`MBB->succ_empty()` (function-exit)**: empty band
   (`FirstBlockedCycle = pipelineDepth`). Same as 2a base case.

2. **`!succ_empty()` AND all successors scheduled**: project
   each successor's actual head into the band via the existing
   non-Conservative path (which already replays
   `SBS.getTop().Bundles` into the scoreboard at offsets
   `[Cycle, Cycle+1, ...]`). The current code already supports
   this when `Trust != Conservative`; the change is just to
   make `Conservative` flip to `Absolute` (or a new tighter
   trust mode) when `successorsAreScheduled` returns true.

3. **`!succ_empty()` AND any successor unscheduled**: fall back
   to current Conservative full-block (today's behavior). This
   covers irreducible CFGs and the bottom-up scheduling first
   pass before successors are scheduled.

In code, the gating condition that decides Conservative vs
non-Conservative is already at line 500:
```cpp
const bool Conservative = !(IsBottomRegion && successorsAreScheduled(CurMBB));
```

The only thing missing is making `successorsAreScheduled` return
true for function-exit MBBs (currently it returns false because
`hasUnknownSuccessors` returns true on `succ_empty()`).

**Minimal change**:

```cpp
bool hasUnknownSuccessors(...) {
  if (MBB->succ_empty()) {
-   return true;
+   // Pure return blocks: post-MBB is the caller, with no AIE
+   // FU carryover. Treat as "no unknown successors" — band
+   // primed from empty successor list (= no priming).
+   return false;
  }
  ...
}
```

With this change:
* Function-exit MBBs: `successorsAreScheduled` returns true →
  `Conservative = false` → non-Conservative path runs → empty
  successor list → no replay, `FirstBlockedCycle =
  pipelineDepth` → no band. **Block 3 fixed**.
* Mid-CFG with scheduled successors: same path → real successor
  head replayed into band. **Blocks 1, 2 fixed** (provided the
  successor is scheduled before this MBB).
* Mid-CFG with unscheduled successors: stays Conservative.

This is even smaller than the original 2a sketch — single-line
change to `hasUnknownSuccessors`.

### Implementation steps (revised)

1. Verify scheduling order: confirm that
   `tie-subregs-flow-3d.mir`-style mid-CFG branches' successors
   ARE scheduled before this MBB at the time
   `initializeBotScoreBoard` runs. If yes, the single-line
   change suffices. If no, need to also reorder MBB scheduling
   so successors come first (already the case for bottom-up
   CFG order in `defineSchedulingOrder`).
2. Apply the `hasUnknownSuccessors` change.
3. Build llc, run regression on `Add2D-red.ll`. All three
   blocks should revert.
4. Run full suite. Regenerate any tests with schedule changes
   (expect tightening, not regression). Hand-audit.
5. If unscheduled-successor mid-CFG cases remain (irreducible
   CFGs or specific scheduling orders), they keep current
   Conservative behavior — no functional change there.

**Risk**: function-exit MBBs lose Conservative cross-boundary
safety. If AIE actually has FU carryover across function calls
(e.g., long-pipeline ops that complete in the caller's first
cycles), removing the band could allow real hardware conflicts.
Empirically check by:
1. Verifying that the regressed tests still execute correctly
   (no functional change, only schedule tightness).
2. Checking AIE2 documentation / hardware spec for cross-call
   pipeline state.
3. If risk is real, narrow the fix: keep band for some FU
   classes (e.g., MAC, vector pipelines that span many cycles)
   but relax for short-pipeline FUs (mov_scl). Or only relax
   for genuinely terminal RET (not for tail-call-like JL_IND).

### Verification

1. `ninja llc`.
2. **Empirical bisection check (if needed)**: dump SchedDAG +
   schedule trace for `test_fadd_bfloat` with the change.
   Confirm SU(14).BotReadyCycle stays at 0 at scheduling time.
3. **Targeted tests**: rerun the function-exit regressed tests.
   Expect cycle counts to revert to OLD baseline:
   * `aie2/bfloat16.ll`: bxor (multi-issue), bneg, max_lt, ...
   * `aie2/fadd.ll`: test_fadd_bfloat, test_fadd_float.
   * `aie2/fsub.ll`, `aie2/float_to_bfloat16.ll`.
4. **Mid-CFG branches** stay regressed (Phase 2 fixes those).
   Tests like `tie-subregs-flow-3d.mir`, `loop-epilogue.mir`
   keep their current regenerated CHECK lines.
5. **Cat C non-delay-slot tests** unchanged: `bitwisenot.mir`,
   `conv2d.mir`, `set.ll`. Those don't use the function-exit
   band logic.
6. **Full**: `ninja check-llvm-codegen-aie` + AIETests.
   Regenerate any tests whose CHECK lines change to absorb the
   tightening. Document each regen in a fixup commit on top of
   `6e395bbae324`.

### Implementation steps

1. Read current `initializeBotScoreBoard` body precisely
   (`AIEMachineScheduler.cpp:262-356`) — confirm exact line
   range and structure.
2. Add the `else if (CurMBB->succ_empty())` branch.
3. Build llc, run targeted regression on the 3 default-issue
   tests (`fadd.ll`, `fsub.ll`, `float_to_bfloat16.ll`).
4. If those revert to OLD line counts, run full suite.
5. Regenerate any tests that change. Create fixup commit.
6. If those tests still regress (Variant 2a doesn't fix them),
   diagnose further — the priming math may differ from my
   reading, or `mov_scl`'s itinerary may have a stage I haven't
   accounted for.

### Original investigation (kept below for reference)

---

## Current investigation: bxor_v32bf16 QoR regression

### Symptom

`llvm/test/CodeGen/AIE/aie2/bfloat16.ll @bxor_v32bf16` schedule grew
from **8 cycles → 9 cycles** under the new commit
(`6e395bbae324`). One free vector SU (`vband x0, x0, x2`) moved from
delay-slot 5 (= bot cycle 4, just below RET) to a NEW pre-RET cycle
(= bot cycle 6, above RET in MIR), pushing the function out by one
cycle.

OLD schedule (8 cycles):
```
mov r0, r16              ← cycle 0 (bot 7)
vbneg_ltz.s16 x0, r16,x4 ← cycle 1 (bot 6)
ret lr                   ← cycle 2 (bot 5)  *fixed cycle*
vband x0, x0, x2         ← delay slot 5 (bot 4)
vbneg_ltz.s16 x2, r16,x2 ← delay slot 4 (bot 3)
vband x2, x2, x4         ← delay slot 3 (bot 2)
vbor x0, x2, x0          ← delay slot 2 (bot 1)
mov r16, r0              ← delay slot 1 (bot 0)
```

NEW schedule (9 cycles):
```
mov r0, r16              ← bot 8
vbneg_ltz.s16 x0, r16,x4 ← bot 7
vband x0, x0, x2         ← bot 6   ← regression: now ABOVE RET
ret lr                   ← bot 5
vbneg_ltz.s16 x2, r16,x2 ← delay 5 (bot 4)
vband x2, x2, x4         ← delay 4 (bot 3)
mov r16, r0              ← delay 3 (bot 2)
vbor x0, x2, x0          ← delay 2 (bot 1)
nop                      ← delay 1 (bot 0)
```

### Question: why is this region scheduled bottom-up at all?

`AIEPostRASchedStrategy::initialize` (`AIEMachineScheduler.cpp:531-540`)
sets:

```cpp
RegionBottomUpCycles = std::max(BottomUpCycles.getValue(), DelaySlotCycles);
RegionTopDownCycles  = Reg.getTopFixedBundles().size();
IsTopDown = (RegionBottomUpCycles == 0) || (RegionTopDownCycles > 0);
```

For `bxor_v32bf16` the region ends in `RET` so `DelaySlotCycles =
N+1 = 6`, `BottomUpCycles` cl::opt is 0, `RegionTopDownCycles = 0`
(no top fixed bundles). The branch evaluates `IsTopDown = false` —
bottom-up is selected.

**Why bottom-up is necessary for delay-slot regions**: the branch
must issue at *exactly* `getNumDelaySlots()` cycles before the MBB
end. The bot zone counts cycles from the MBB end; a bot anchor at
cycle N is a deterministic placement floor. Top-down scheduling
counts cycles from the MBB start, which depends on the (unknown)
total length, so it cannot anchor "N from end" without first knowing
the length.

**The schedule is bidirectional, not pure bottom-up.** The switch
back to top-down happens at `pickNodeAndCycle`
(`AIEMachineScheduler.cpp:599-606`):

```cpp
if (!IsTopDown && Bot.getCurrCycle() >= RegionBottomUpCycles) {
  IsTopDown = true;  // bot zone has filled all bot-mandated cycles
}
```

For `bxor_v32bf16`:
1. Bot zone fills bot cycles 0..5 (= delay slots 1..5 + RET cycle).
2. Once `Bot.CurrCycle >= 6`, scheduler switches to top-down.
3. Remaining free SUs (SU(0), SU(1), and possibly SU(2)) are placed
   top-down from cycle 0 of the region.

This is why "regular" (top-down) scheduling DOES occur here — but
only for the *prefix* of the region, after the delay-slot tail has
been bot-filled. There is no purely top-down alternative for
delay-slot regions because the branch's MIR position is not
negotiable.

**Are top-down schedules more compact in general?** Slightly, on
average, because top-down packs from cycle 0 with a tighter view of
which SUs are ready first. But for delay-slot regions the bot-zone
phase is mandatory — the question is moot for the prefix (which is
top-down already once the bot phase finishes).

### The actual root cause (revised — supersedes the earlier "RET pre-emit blocks bot 5" hypothesis)

The earlier analysis identified RET's pre-emitted scoreboard footprint
as the conflict source at bot cycle 5. That's only the SECOND step
of the chain. The PRIMARY cause is
`upgradeFreeSUExitEdgesViaScoreboard`
(`AIEMachineScheduler.cpp:434-488`) inflating free-SU heights via
artificial `SU → ExitSU` edges:

```
SU(6) mov_r16 in OLD: lat to ExitSU = 0 (K-1 of MaxLat=1).
                      Height = 0. BotReady = 0.

SU(6) mov_r16 in NEW: upgradeFreeSUExitEdgesViaScoreboard probes
                      scoreboard for SU(6) at offsets 0, 1, …
                      Conservative band blocks bot cycles [0, K-1]
                      (K ≈ 2 for AIE2 mov pipeline depth + post-exit
                      reservation). SafeDistance = 2 → adds artificial
                      edge SU(6) → ExitSU with lat=2. Height = 2.
                      BotReady = 2.

Propagated upward through SU(3) → SU(6) Out lat=2 edge:
  SU(3).Height = max(SU(4).Height + 1, SU(6).Height + 2, lat-to-ExitSU)
                = max(3, 4, ?) = 4   (was 3 in OLD: max(3, 0+2, 1) = 3)
```

The Height of SU(3) determines its bot cycle floor. With Height=3 in
OLD, SU(3) lands at bot 3 (delay slot 4), leaving bot 4 (delay slot
5) free for SU(2) vband_a. With Height=4 in NEW, SU(3) lands at bot
4 (delay slot 5), forcing SU(2) (anti-dep, must be at higher bot
cycle than SU(3)) to bot ≥ 5. RET's anchor at bot 5 collides; SU(2)
pushed to bot 6.

### Why can't `vband x0, x0, x2` (SU(2)) fit in a delay slot?

The MIR-order constraint that forces SU(2) above RET is the anti-dep
edge `SU(2) Anti → SU(3)` lat=0:

```
SU(2): VBAND $x0, $x0, $x2          ; reads $x2
SU(3): $x2 = VBNEG_LTZ ...           ; writes $x2
```

If SU(3) executes before SU(2) in issued order, SU(2) reads the NEW
value of $x2 (vbneg's output) instead of the original. Different
program. So `MIR_pos(SU(2)) < MIR_pos(SU(3))` is mandatory.

In bot-zone arithmetic: `bot(SU(2)) > bot(SU(3))`.

OLD schedule:
* SU(3) lands at bot 3 (Height=3).
* SU(2) lands at bot 4 (delay slot 5). Anti-dep respected. Fits.

NEW schedule:
* SU(3) lands at bot 4 (Height=4 due to ExitSU edge upgrade).
* SU(2) needs bot > 4. Bot 5 = RET (resource conflict via pre-emit).
  Bot 6 is the first free option. SU(2) lands at bot 6 = ABOVE RET.

So SU(2) *can* in principle fit at delay slot 5 — but only if SU(3)
is at delay slot 4 (bot 3). The Height inflation pushed SU(3) one
cycle later, which cascaded into SU(2) being pushed past the entire
delay-slot region PLUS the RET cycle.

### Sharper question: is bot-up scheduling needed at all once the branch is fixed?

**No.** The bot-up phase exists for one reason — to anchor the
branch at exactly `getNumDelaySlots()` cycles before the MBB end.
With this commit's fixed-SU machinery, the branch's bot cycle is
already anchored by the artificial chain edge
`branch → ExitSU` with lat = `getNumDelaySlots()`
(`AIEBaseSubtarget.cpp::EmitFixedSUnits::createFixedSUDAGNodes`).
The legacy bot-up floor at `AIEMachineScheduler.cpp:531`:

```cpp
RegionBottomUpCycles = std::max(BottomUpCycles.getValue(), DelaySlotCycles);
```

is redundant for delay-slot regions whose branch is now a fixed bot
SU. The bot-up phase no longer contributes anything the chain
edge doesn't already enforce.

**This sidesteps the regression but introduces a different
problem.** If we drop the `DelaySlotCycles` floor:

* Region starts top-down. Top zone schedules free SUs from cycle
  0, using Depth (which is NOT inflated by
  `upgradeFreeSUExitEdgesViaScoreboard`'s ExitSU edges).
* RET (bot-fixed) sits in Top.Pending; `doesNotProgressInZone(Top,
  RET)` returns true. Top-zone Available eventually empties.
* `mustSwitchToBottomUp` fires. Switch to bot. RET pre-scheduled
  at bot 5 via the fixed-SU bypass.

**The new problem**: top-zone CurrCycle progresses 0, 1, 2, ...
until top.Available empties. Bot zone (when triggered) owns cycles
[L-6, L-1] (RET + 5 delay slots). For free SUs that should
naturally fill delay slot cycles, top-down placement may run them
into top cycles 0..L-7, leaving the delay-slot region as NOPs.

That's because top-down doesn't *prefer* placing SUs in late
cycles; it packs greedily from cycle 0. The OLD bot-up phase
actively *filled* delay slots from the bottom, biasing free SUs
into late cycles. Without bot-up, delay slots become a vacuum the
top-down pass doesn't reach.

So: switching to top-down avoids the height-inflation cascade but
trades it for under-filled delay slots. **For bxor_v32bf16 we want
free SUs IN delay slots — top-down only doesn't deliver that.**

The actual fix needs to keep bot-up scheduling for delay-slot
regions (so free SUs fill delay slots), but eliminate the height
inflation that's pushing them above the branch. That points to
mitigation #2 (fix `upgradeFreeSUExitEdgesViaScoreboard` to not
count the post-MBB conservative band against this MBB's free
SUs), not mitigation #1.

### CORRECTED ROOT CAUSE: RET's pre-emitted pipeline tail blocks bot 0

The earlier "gate `upgradeFreeSUExitEdgesViaScoreboard`" hypothesis
was wrong. Empirical verification: building with and without the
gate produces byte-identical asm under both `--issue-limit=1` and
default issue-width for `bxor_v32bf16` and other regressed tests.
The probe is NOT the cause.

**Audit of test RUN lines** (which `--issue-limit` they use):
* `--issue-limit=1`: `tie-subregs-flow-3d.mir`, `bfloat16.ll`,
  `conv2d_offset_test.ll`, `extract.ll`, `intrinsics-128bit.ll`.
* `--issue-limit=6`: `loop-epilogue.mir`.
* default issue-width: `fadd.ll`, `fsub.ll`, `float_to_bfloat16.ll`.

So the regressions are NOT specific to `--issue-limit=1`. They
appear at every issue width.

**The actual mechanism — RET's pipeline tail emission**:

`AIEPostRASchedStrategy::schedNode` for fixed bot SUs (my custom
emit-only path) calls
`HR->EmitInstruction(RET, DeltaCycles=-N)` where N=5 for RET.
`EmitInstruction` consults RET's SchedClass and emits its full
**resource footprint** into the scoreboard, including the
pipeline-tail FU reservations that span K cycles forward in time.

In bot-scoreboard indexing where bot 0 = MBB last cycle:
* RET at scoreboard offset `-5` (= bot 5).
* Pipeline tail spans offsets `-5` through `-5 + K - 1`.
* For RET with K = 6, the tail reaches offset `0` = **bot 0 = DS1
  = MBB last cycle**.

When free SUs are subsequently scheduled bot-up, the runtime
hazard scan in `pickOnlyChoice` consults the same scoreboard and
finds bot 0 blocked by RET's tail. Free SUs that would naturally
land at bot 0 (e.g. `mov r16, r3` in `test_fadd_bfloat`) get
pushed to bot 1 (DS2). The cascade through Pred edges
(BotReadyCycle propagation: `pred.BotReady = max(pred.BotReady,
succ.BotReady + edge_lat)`) shifts all delay-slot SUs up by 1.
Eventually some SU's required cycle exceeds the delay-slot range
(bot 4 = DS5), forcing it into the prefix above RET. NOP appears
at bot 0 (DS1) because no SU can land there.

**Why OLD didn't hit this**: RET was a free SU. Bot zone scheduled
free SUs first, then RET LAST (via tie-break / `RegionBottomUpCycles`
floor). By the time RET's resources hit the scoreboard, all free SUs
had committed their cycles. RET's tail emission didn't propagate
back as a constraint on free-SU placement — there was no "free SUs
yet to place" at that point.

**Confirmation under `--issue-limit=1`**: with single-issue, the
same mechanism applies but with additional pressure: SU(5) at bot 1
fully occupies the cycle, forcing SU(6) up to bot 2, SU(4) to bot 3,
SU(3) to bot 4 — and SU(2) (anti-dep to SU(3)) is then forced past
bot 4 entirely. This is an amplified version of the same root cause:
RET's pre-emission at bot 5 blocks bot 5 for SU(2), forcing it to
bot 6 (above RET).

**Confirmation under default issue-width (fadd.ll)**: for
`test_fadd_bfloat` post-RA SchedDAG dump shows SU(15) RET pre-scheduled
at bot 5 (Ready @5c), then `Cycle: 1 BotQ.A` bumps CurrCycle to 1,
and SU(14) `mov r16,r3` lands at bot 1 (Ready @1c) — not at bot 0
where its natural BotReady would have placed it. RET's tail emission
blocks bot 0. The cascade then pushes:
* `vextract.s16` from bot 2 (DS3 in OLD) to bot 3 (DS4 in NEW).
* `vconv.bf16.fp32` from bot 4 (DS5 in OLD) to bot 5 (bundled with
  RET in NEW — different slot, co-issues).
* Schedule grows by 1 cycle in the prefix (extra NOP).

### Chosen fix: revert the gate, design Phase 2 properly

The gate is ineffective. **Revert it.**

The proper fix is one of two paths, both more invasive than a
one-line gate:

1. **Don't pre-emit RET's pipeline tail.** Modify the custom
   emit-only path in `schedNode` to emit only RET's *issue cycle*
   resources (not the K-cycle pipeline tail). Free SUs would then
   see bot 0..bot 4 unblocked by RET, and bot 5 only minimally
   reserved (just RET's issue slot). Cascade disappears.

   Risk: the tail emission models a real hardware reservation —
   skipping it might allow free SUs to schedule at cycles that
   genuinely conflict with RET's pipeline. Need to verify whether
   AIE's pipeline genuinely needs the tail reservation visible to
   the scheduler.

2. **Phase 2's successor-projection scoreboard.** For function-exit
   RET regions the projection is empty (no successors). The
   `initializeBotScoreBoard` path with `Trust = Absolute` already
   leaves these cycles unblocked (FirstBlockedCycle =
   pipelineDepth). The issue is that RET's tail emission (during
   pre-schedule) RE-blocks them.

   So Phase 2 alone doesn't fix this — it'd still hit the
   pre-emission tail issue. But Phase 2 + (1) together would be the
   complete fix.

### Recommended sequencing

* **Step A (DONE)**: gate reverted in working tree.
* **Step B (PARTIAL)**: prototype that completely skips
  `HR->EmitInstruction(SU, DeltaCycles)` for fixed-SU pre-emit.
  Empirical results:
  * **`bxor_v32bf16` (--issue-limit=1)**: regression FIXED. 8 cycles
    matches OLD.
  * **`test_fadd_bfloat` (default issue)**: regression PERSISTS.
    Still 17 cycles vs OLD's 16. SU(14) `mov r16,r3` lands at bot 1
    even when RET's footprint isn't emitted.

  → Two different mechanisms. The RET-tail-emission hypothesis
  explains the issue-limit=1 cases but NOT the default-issue
  cases.

  For the default-issue case, the trace shows:
  * SU(14) is in `BotQ.P` (Pending) at CurrCycle=0, not Available.
  * Pending implies BotReady > CurrCycle — i.e., SU(14).BotReady ≥ 1
    despite having ExitSU.Preds.SU(14).lat = 0 (K-1 fixup applied).
  * Where the +1 comes from is not yet identified — needs deeper
    investigation of how SU(14).BotReady gets set during DAG
    initialization.

* **Step B continued**: trace SU(14).BotReady origin in fadd_bfloat
  to identify the second mechanism. Hypotheses to verify:
  1. The chain edge `RET → ExitSU` lat=N=5 affects ExitSU's release
     order, which interacts with `releasePending` to delay SU(14).
  2. Some initialization in `EmitFixedSUnits`'s eager pre-set
     interacts with non-fixed SUs unexpectedly.
  3. `upgradeFreeSUExitEdgesViaScoreboard` is still inflating heights
     for these tests despite being function-exit MBBs.

* **Step C (Phase 2)**: successor / predecessor projection
  scoreboards as previously planned. Doesn't address either of the
  current mechanisms directly but is the long-term architectural
  improvement.

### Original (now-rejected) gate analysis — kept for reference

#### Commit 1 — Implementation

Single change in `AIEPostRASchedStrategy::upgradeFreeSUExitEdgesViaScoreboard`
(`AIEMachineScheduler.cpp:434-488`). Add an early return at the
top:

```cpp
void AIEPostRASchedStrategy::upgradeFreeSUExitEdgesViaScoreboard(
    ScheduleDAGMI *Dag) {
  AIEHazardRecognizer *BotHR = getAIEHazardRecognizer(Bot);
  if (!BotHR || BotHR->getMaxLookAhead() == 0)
    return;

  // For delay-slot regions the bot zone extends N+1 cycles past
  // the Conservative band; free SUs land in delay slots via the
  // runtime hazard scan, which already enforces cross-boundary
  // safety per-pick. Static SU → ExitSU edges added by this probe
  // would cascade through ancestor Heights / BotReady (e.g.
  // SU(3) → SU(6) Out lat=2 propagating SU(6).BotReady=2 into
  // SU(3).BotReady=4), pushing free SUs out of delay slots in
  // anti-dep / WAW chain shapes (bxor_v32bf16 et al.). Phase 2's
  // successor-projection scoreboard fixes this at the root by
  // making the band reflect real successor demand instead of a
  // full-block Conservative; until then, skip the probe here.
  if (getDelaySlotInstr(RegionBegin, RegionEnd))
    return;

  // ... (rest of probe loop unchanged)
}
```

#### Commit 1 — Verification

1. `ninja llc`.
2. **Targeted regression check**: rerun the ~10 currently-failing
   delay-slot tests; expect them to revert to OLD bit-identical
   (or close — some may have minor scheduler-tie-break differences
   that need regen).
3. **Cat C regression check**: `bitwisenot.mir` and the 7
   previously-hanging tests
   (`aie2/schedule/postpipeliner/conv2d.mir`,
   `aie2p/end-to-end/{add-att-broadcasting-aa,
   conv2d-dw-bf16, conv2d_bfp16_kernel_red, gemm-bfp16}.ll`,
   `aie2p/schedule/events-and-post-swp.mir`,
   `schedule/sched-initialize-topscb-once.ll`) — must still
   complete in ≤ 5s. The probe still runs for these (they're
   non-delay-slot regions).
4. **Full**: `ninja check-llvm-codegen-aie` — expect close to
   2153/2153 passing. Hand-audit any remaining diffs.
5. `ninja check-llvm-unittests-target-aie` — 39/39.

#### Commit 1 — Commit message

`[AIE][NFC-ish] Skip ExitSU edge upgrade for delay-slot regions`

Body: explain the cascade mechanism (Height inflation
propagating through Pred/Succ edges in the SchedDAG), reference
the bxor_v32bf16 case, and note that Phase 2's
successor-projection scoreboard is the architectural fix. The
gate is a band-aid until then.

**Commit 2 (Phase 2) — successor / predecessor projection
scoreboards.** The architectural fix that replaces the
Conservative full-block band with a `ResourceScoreboard<FuncUnitWrapper>`
projection built from real successor / predecessor MBB schedules.
For function-exit RET regions the projection is empty (no MBB
successors); the probe finds no conflict; no edge added. For
mid-CFG branches the projection reflects the actual neighbour
resource demand. Already specified in Phase 2 of this plan; the
existing description was updated below to call out its role as
the proper fix for this class of regressions.

### Fix direction (history kept for reference)

The earlier "RET pre-emit blocks bot 5" was a contributing factor
(it determines whether SU(2) fits at bot 5 vs bot 6 once forced past
bot 4), but the LEADING cause is the height inflation from
`upgradeFreeSUExitEdgesViaScoreboard` — and **the fundamental
remediation is to drop bot-up scheduling for delay-slot regions
whose branch is a fixed bot SU**:

1. **Drop the `DelaySlotCycles` floor when the trailing branch is
   a fixed bot SU** (preferred). Set `RegionBottomUpCycles =
   BottomUpCycles.getValue()` (= 0 in the default case) instead of
   `max(BottomUpCycles, DelaySlotCycles)` for those regions. The
   chain edge `branch → ExitSU` already anchors the branch; the
   floor is redundant. Region schedules top-down; Depth-based
   placement bypasses the ExitSU height inflation entirely.
   Removes `upgradeFreeSUExitEdgesViaScoreboard`'s feedback path
   for the regression.

2. **Make `upgradeFreeSUExitEdgesViaScoreboard` aware of MBB-end vs
   post-exit band**. SU(6) mov_r16 with M=1 reserves only one bot
   cycle. The conservative K-cycle band past MBB end models the
   post-MBB future, not the bot zone proper. The current probe
   treats them as one continuous scoreboard. Fix: only require the
   SU's reservation footprint to fit within the bot zone proper
   (bot cycle 0..L-1), not the projected post-MBB band.

3. **Accept the +1 cycle regression** (current state). Functionally
   correct.

### Why it happens — step by step (low-level — original analysis kept for reference)

The post-RA SchedDAG for the region (from `--debug-only=machine-scheduler`)
shows the relevant Succs/Preds:

```
SU(2) vband_a: VBAND $x0,$x0,$x2 — Height=4
  Successors: SU(5) Out lat=1, SU(5) Data lat=1, SU(3) Anti lat=0,
              ExitSU lat=2
SU(3) vbneg_b: VBNEG_LTZ_S16 $x2 — Height=4
  Successors: SU(4) Out lat=1, SU(4) Data lat=1, SU(6) Out lat=2,
              ExitSU lat=2
SU(7) RET implicit $lr — Height=5  *fixed, chain → ExitSU lat=5*
  Successors: ExitSU Ord lat=5 Artificial
```

**Crucial dependency**: `SU(2).Succs` contains `SU(3) Anti lat=0`.
This is the WAR edge "vband_a reads $x2 → vbneg_b writes $x2". In
bot-zone bottom-up scheduling, anti-deps mean the **predecessor SU
(in MIR) must be at a HIGHER bot cycle than the successor**. So
`vband_a.bot_cycle > vbneg_b.bot_cycle`.

**Heights are identical OLD vs NEW**: SU(2)=4, SU(3)=4, SU(7)=5. The
register dep edges are the same (Phase 0b doesn't add free→fixed
register edges in this region — RET reads only $lr, which no free SU
defines).

**Where the schedules diverge — the timing of RET's scoreboard
emission:**

NEW (with my pre-schedule path in `AIEPostRASchedStrategy::schedNode`):
1. `pickOnlyChoice(Bot)` picks RET FIRST via the fixed-SU bypass
   (`isAvailableNode` returns true at any `DeltaCycles ≥ MinDelta`).
2. My custom emit-only path calls `HR->EmitInstruction(RET, -5)` —
   RET's resource footprint is committed to the scoreboard at offset
   −5 (= bot cycle 5) BEFORE any free SU is considered.
3. Free SUs scheduled bottom-up: SU(5) vbor at bot 1, SU(6) mov_r16
   at bot 2, SU(4) vband_b at bot 3, SU(3) vbneg_b at bot 4.
4. SU(2) vband_a is released only AFTER SU(3) is scheduled (because
   `SU(2).Succs` includes SU(3) — anti-dep — and bot zone releases
   SUs when all their Succs are scheduled). So SU(3) is always
   scheduled before SU(2).
5. SU(2)'s `BotReadyCycle` after release = `max(SU(3).BotReady + 0,
   SU(5).BotReady + 1) = max(4, 2) = 4`.
6. Scheduler tries SU(2) at `CurrCycle = 4`, `DeltaCycles = 0`:
   `checkHazard` returns hazard at offset 0 — SU(3)'s vector slot is
   reserved at that scoreboard position.
7. Scan tries `DeltaCycles = -1` (= bot cycle 5, RET's cycle): RET's
   resource footprint (committed in step 2) overlaps SU(2)'s vector
   FU/slot demand. `checkHazard` returns hazard.
8. Scan succeeds at `DeltaCycles = -2` (= bot cycle 6, ABOVE RET).
   SU(2) scheduled at bot 6, region grows by 1.

OLD (legacy flow, no pre-schedule):
1. RET was a FREE SU. Bot zone scheduled it through the standard
   hazard-scan path, which only emits its resources to the scoreboard
   when `bumpNode` actually places it.
2. `RegionBottomUpCycles = DelaySlotCycles + 1 = 6` floor + the
   delay-slot tie-break preferred RET to be picked LAST among bot
   candidates (or at least later than free SUs filling delay slots).
3. Free SUs filled bot cycles 0-4 (delay slots 1-5). RET went last
   at bot cycle 5.
4. By the time RET's resources hit the scoreboard, every free SU
   was already placed. No conflict propagated back to push any free
   SU above RET.

### Root cause in one sentence

**Pre-emitting RET's scoreboard footprint at bot cycle 5 *before*
free SUs are placed creates a real resource conflict (RET's FU/slot
overlaps a free vector op's demand) that — combined with the
anti-dep ordering forcing `SU(2)` to bot cycle ≥ 5 — pushes `SU(2)`
above RET. The legacy code emitted RET's resources LAST, after all
free SUs had committed to their cycles, so the conflict never
materialized into a region-growth event.**

### Where this generalizes

This is not a bug — it's a tightening-vs-loosening tradeoff. The
new behavior is functionally correct (the program executes the same
operations in compatible order). It happens whenever:

* A delay-slot region has a free SU with an anti-dep / WAW chain
  that forces its bot cycle to ≥ the branch's anchor cycle.
* The free SU's resource demand overlaps the branch's resource
  footprint at that cycle.

The 138 regenerated tests in the commit absorbed many such cases.
Specific examples already audited:
* `aie2/schedule/resource/p_rm.mir` — 5 MOVs WAW-chained, last
  pushed above JL_IND. +1 cycle.
* `aie2/ra/tie-subregs-flow.mir` — MOV $m1 pushed above RET. +1
  cycle.
* `aie2/bfloat16.ll @bxor_v32bf16` — vband_a pushed above RET. +1
  cycle.

### Possible remediations (not yet decided)

1. **Don't pre-emit fixed-SU resources to scoreboard** — only
   emit them at the moment the scheduler "would" have placed them
   (= last in bot zone). This restores the OLD ordering. Drawback:
   the scoreboard wouldn't reflect fixed-SU constraints during free
   SU scheduling, possibly allowing slot-conflicting placements that
   the OLD code rejected via the legacy machinery.
2. **Schedule fixed SUs in a separate pre-pass that emits resources
   but allows free SUs to co-locate**. A modified scoreboard query
   that "ignores" the fixed SU's footprint when checking a free SU
   that's slot-disjoint with the branch.
3. **Accept the +1 cycle regression** — it's small, only affects
   regions with the specific anti-dep + slot-overlap shape, and
   the schedule remains functionally correct.

---

## Historical: Fix 2 SCHED-DUMP failures, then commit

### Context

After landing the full delay-slot-as-fixed-SU work (Phase 0a, Phase 0b
structural + strip + eager pre-set, scoreboard-aware ExitSU edges,
single-entry synthesis, chain-lat augmentation, pre-schedule of fixed
SUs without bumping CurrCycle, isFixedSU range fix), the test suite is
at **2143/2153 PASS (99.54%)** with 2 remaining failures:

* `aie2/end-to-end/Conv2D-red.ll` — fails only the `SCHED-DUMP` check,
  ASM check passes.
* `aie2/end-to-end/Conv2D-red-swp.ll` — only has a `SCHED-DUMP` check,
  no `ASM` check.

Both fail because `llvm/utils/imisched.py` (the QoR sentinel parser)
errors out with `Error: Actions=2 Schedules=3`. The
`schedlogparser.SCHED_ACTION_RE` regex expects every scheduled SU to
emit:

```
Pick Bot ONLY1
Scheduling SU(N) <mi>
  Ready @Nc
```

The `Ready @Nc` line is emitted by `SchedBoundary::bumpNode`
(`MachineScheduler.cpp:2938`). My pre-schedule custom-emit path for
fixed SUs in `AIEPostRASchedStrategy::schedNode` bypasses `bumpNode`
entirely (to avoid the unwanted `CurrCycle` advance) and therefore
omits the `Ready @Nc` log line. The parser sees N "Scheduling SU(N)"
markers but only N-K matching schedule action blocks (K = number of
fixed SUs scheduled via the custom path).

### Fix

Emit the `Ready @Nc` log line in the custom-emit path so the
`schedlogparser` regex matches every scheduled SU. One-line addition
in `AIEPostRASchedStrategy::schedNode`'s fixed-SU branch:

```cpp
if (isFixedSU(*SU, /*IsTop=*/IsTopNode)) {
  AIEHazardRecognizer *HR = ...;
  SchedBoundary &Zone = IsTopNode ? Top : Bot;
  int DeltaCycles = ...;
  HR->EmitInstruction(SU, DeltaCycles);
  // Emit the cycle marker that schedlogparser.SCHED_ACTION_RE expects
  // after "Scheduling SU(N)". The standard bumpNode path emits this
  // line; our custom emit-only path bypasses bumpNode but still needs
  // to emit the marker so QoR sentinel parsers (imisched.py) can match
  // each scheduled SU to a complete schedule action block.
  unsigned ReadyCycle =
      IsTopNode ? SU->TopReadyCycle : SU->BotReadyCycle;
  LLVM_DEBUG(dbgs() << "  Ready @" << ReadyCycle << "c\n");
  return;
}
```

### Verification

1. `ninja llc`.
2. `build/bin/llvm-lit -j 4 ./llvm/test/CodeGen/AIE/` — expect 2145
   PASS, 0 FAIL (the 2 SCHED-DUMP tests now pass; the 5 expected fails
   stay as they were).
3. `./build/unittests/Target/AIE/AIETests` — expect 39/39.
4. Spot-check `set.ll` (correctness) and `bitwisenot.mir` (no hang).

### Commit

After full PASS, create a commit covering all of:

* `llvm/lib/Target/AIE/AIEMachineScheduler.{h,cpp}`
* `llvm/lib/Target/AIE/AIEBaseSubtarget.cpp`
* `llvm/lib/Target/AIE/AIEBaseInstrInfo.{h,cpp}`
* `llvm/lib/Target/AIE/AIEInterBlockScheduling.{h,cpp}`
* 138 regenerated test files under `llvm/test/CodeGen/AIE/`
* 1 manually-fixed test (`small.mir` — restored CHECK lines that
  `update_mir_test_checks.py` over-pruned)

Single commit (per existing user feedback, prefer one bundled commit
over many small ones for refactors in this area). Message should
explain: Phase 0b structural change + delay-slot-as-fixed-SU synthesis
+ pre-schedule path. Co-authored line for Claude Opus 4.7. Don't push.

---

## Historical: Treat every delay-slot branch as a single-entry bot-fixed SU (no empty pseudos)

### Context — what this is fixing and why

Phase 0b (fixed SUs in SchedDAG before `buildEdges` runs) is required to
let `buildEdges` create register dependency edges between free SUs and
fixed SUs. Without this, `set.ll`-style cases (`and r0, r0, r1` defining
`r0`, followed by a `jz r0` that becomes a fixed delay-slot branch) can
schedule the `and` *after* the `jz`, executing a different program.
Phase 0b is correctness-critical, not an optimisation.

Phase 0b on its own exposes **Cat C — infinite cycle-bumping**: in
`bitwisenot.mir`'s exit block (`bb.2`), the trailing `RET` (a free,
delay-slot SU) gets stuck in `Bot.Pending` because
`Zone.checkHazard(RET, DeltaCycles)` returns `hazard=true` for every
`DeltaCycles` in `[MinDelta, 0]`. The `pickOnlyChoice` cycle-bumping
loop runs forever, with `RET.BotReadyCycle` and `CurrCycle` growing in
lockstep. Confirmed by reading `AIEMachineScheduler.cpp:656-714`
(`isAvailableNode` free-SU path) and the in-tree
`MachineScheduler.cpp:3121-3149` (`pickOnlyChoice`):

1. After top zone schedules SU#0..SU#9 (10 SWP-epilogue fixed bundles),
   only `RET` is left, and it is rejected from top via
   `doesNotProgressInZone` (`AIEMachineScheduler.cpp:501-509`,
   `Zone.isTop() && SU.hasDelaySlot()` is true).
2. `mustSwitchToBottomUp` flips `IsTopDown=false`. `RET` released to
   `Bot.Pending` with `BotReadyCycle = K-1 = 5` from the `RegionEndEdges`
   K/K-1 backward latency edge.
3. `pickOnlyChoice` finds `Bot.Available` empty. Enters the
   `for (...; Available.empty(); ++i) { bumpCycle; releasePending; }`
   loop. Each iteration calls `releaseNode(RET, ...)` →
   `isAvailableNode(RET, Bot, ...)`. `RET` is FREE so the fixed-SU
   bypass at `:665-673` is skipped; we fall through to the hazard scan
   loop at `:700-712`.
4. `Zone.checkHazard(RET, DeltaCycles)` returns hazard=true for every
   `DeltaCycles` in `[MinDelta, 0]`. The scan loop falls through to
   `SU.BotReadyCycle = max(BotReadyCycle, CurrCycle - MinDelta)` which
   grows `BotReadyCycle` by ~128 each iteration. `BotReadyCycle` and
   `CurrCycle` race in lockstep; `checkHazard` keeps returning hazard
   for the same scoreboard slot offsets every cycle. 1.8M+ cycle bumps,
   no progress.
5. The hazard-true-forever stems from `bb.2` being an exit block (no
   successors). `initializeBotScoreBoard` (`:261-340`,
   `Trust=Conservative`) leaves `FirstBlockedCycle=0` and primes no
   successor tail, so the scoreboard's pipeline-depth blockage at the
   bottom is the only state — and `RET`'s resource demand always
   conflicts with it.

**The structural fix.** Treat every delay-slot branch (RET, JL, JL_IND,
JZ, JNZ, JNZD, J_jump_imm, J_jump_ind — confirmed by the
`PreSchedInstExpansion<X, DelayedSchedBarrier>` patterns in
`AIE2InstrInfo.td:446,448,452,456,458,461,463,473,480,482` and analogous
in `AIE2P*InstrInfo.td`) as a single-entry bot-fixed SU.
`isAvailableNode`'s fixed-SU bypass (`:665-673`) then routes the branch
through pure cycle-alignment (`DeltaCycles >= MinDelta`), **never
calling `checkHazard` for it**. Cat C disappears.

This is the same intent as β.1 v1's dormant `synthesiseDelaySlotBotFixed`
(`AIEInterBlockScheduling.cpp:747-780`) and the rolled-back β.1.5, but
**without the empty BUNDLE pseudo cycle anchors** that those approaches
inserted into MIR. Instead, the branch's cycle is anchored by setting
the chain edge `branch → ExitSU` latency to `getNumDelaySlots(branch)`
in `EmitFixedSUnits::createFixedSUDAGNodes`. Branch height becomes N,
correctly placing it at "cycle N from bot" = MBB cycle L-N-1. No MIR
mutation, no commit-time bundling change, no hazard-recognizer bypass
for empty pseudos, no top-root release surprise.

### Approach overview

Five layered changes; each is independently buildable and testable.
Phase 0a + bundle-aware `MemoryEdges` are already in the working tree
and verified NFC at 2145/2145.

```diff
+ Phase 0a               RegionEndEdges skips fixed SUs by MIR position
+ MemoryEdges helper     Bundle-aware getMaxMemoryLatency walks BUNDLE inner MIs
+ Phase 0b structural    Fixed SUs created in buildGraph before buildEdges,
+                        reverse iter for bot fixed (NodeNum invariant),
+                        eager BotReady/TopReady pre-set in EmitFixedSUnits
+ Strip fixed↔fixed      After buildEdges, drop every non-Artificial edge whose
+                        BOTH endpoints are fixed SUs (top↔top, bot↔bot,
+                        top↔bot). Required because SWP-pushed BUNDLE pseudos
+                        in TopFixedBundles contain modulo-cloned MIs sharing
+                        physical registers; buildEdges' register-edge
+                        emission across BUNDLE wrappers can create cycles
+                        in Pred/Succ that hang ComputeDepth/ComputeHeight.
+ Single-entry synthesis Every delay-slot branch (any region shape) becomes
+                        BS.RegionBottomFixed[r] = [branch_bundle] (1 entry)
+ Chain lat augmentation lat(branch → ExitSU) = getNumDelaySlots(branch)
+                        when branch.hasDelaySlot() and chain is single-entry
```

Why "single-entry" replaces β.1 v1's "branch + N empties": with chain
lat = N, `branch.getHeight() = N` directly, anchoring the branch at
the right cycle without any MIR mutation. The N delay-slot positions
remain free MIR cycles for the legacy `RegionBottomUpCycles` floor /
free-SU fill / NOP padding to handle — exactly what they do today.

### File-by-file changes

#### `llvm/lib/Target/AIE/AIEMachineScheduler.{h,cpp}` — Phase 0b structural

```diff
 void AIEPostRASchedStrategy::buildGraph(...) {
   DAG.clearDAG();
   ...
   for (int S = 0; S < NCopies; S++) {
+    // top-fixed forward (matches top chain iteration in EmitFixedSUnits)
+    for (MachineInstr &MI : Region.top_fixed_instrs())
+      DAG.initSUnit(MI);
     for (MachineInstr *I : Region.getFreeInstructions())
       DAG.initSUnit(*I);
+    // bot-fixed REVERSE (matches reverse chain iteration; preserves
+    // FirstBotFixedSU = lowest-NodeNum, LastBotFixedSU = highest-NodeNum
+    // invariant that the OLD `if (!FirstBotFixedSU)` semantics rely on)
+    for (MachineInstr &MI : reverse(Region.bot_fixed_instrs()))
+      DAG.initSUnit(MI);
   }
   DAG.ExitSU.setInstr(Region.getExitInstr());
   DAG.makeMaps();
   DAG.buildEdges(Context->AA);   // sees ALL SUs → free↔fixed reg edges
+  // Strip fixed↔fixed register/memory edges. The artificial chain
+  // installed later by EmitFixedSUnits is the source of truth for
+  // fixed-fixed cycle distance; buildEdges' register-edge emission
+  // over modulo-cloned BUNDLE pseudos in SWP push can create both
+  // directions of edges between adjacent bundles (BUNDLE wrapper
+  // operand lists fold many inner MIs into one merged def/use list,
+  // and kill-and-redefine tracking can register edges in both
+  // directions across two adjacent BUNDLE wrappers). Result: a cycle
+  // in Pred/Succ that hangs ComputeDepth / ComputeHeight on the very
+  // first call (the bitwisenot.mir crash). Free↔fixed and free→free
+  // edges remain — those are what fix set.ll-style branch correctness.
+  removeFixedFixedRegisterEdges(DAG, Region);
 }
```

`removeFixedFixedRegisterEdges` is a small helper that:
1. Builds `SmallPtrSet<const MachineInstr*, 16> FixedMIs` from
   `Region.top_fixed_instrs()` ∪ `Region.bot_fixed_instrs()`.
2. For each `SUnit &SU` in `DAG.SUnits`:
   * If `SU.getInstr()` is not in `FixedMIs`, skip.
   * Else walk `SU.Preds` (collected up-front into a `SmallVector` to
     avoid invalidation), and for every `SDep` whose source is also
     in `FixedMIs` AND whose `getKind() != SDep::Artificial`, call
     `SU.removePred(Dep)`. `removePred` removes the symmetric `Succs`
     entry on the other end, keeping the graph consistent.

The strip happens BEFORE `EmitFixedSUnits` runs (which adds the
artificial chain). At strip time, `FixedMIs` corresponds to the
intended fixed-SU set; `EmitFixedSUnits`'s chain-creating reverse
iteration walks the same set later and adds artificial edges that
the strip leaves alone (different `SDep::Kind`).

```diff
-SUnit &AIEPostRASchedStrategy::addFixedSUnit(MachineInstr &MI, bool IsTop) {
+SUnit &AIEPostRASchedStrategy::markFixedSUnit(MachineInstr &MI, bool IsTop) {
+  // SUnit was already created in buildGraph; just look it up.
+  SUnit *SU = DAG->getSUnit(&MI);
+  assert(SU && "Fixed MI must already have an SUnit from buildGraph");
+  unsigned SUNum = SU->NodeNum;
   ...
 }
```

Header rename in `AIEMachineScheduler.h`: `addFixedSUnit` → `markFixedSUnit`.

#### `llvm/lib/Target/AIE/AIEBaseSubtarget.cpp` — chain latency + eager pre-set

In `EmitFixedSUnits::createFixedSUDAGNodes` (`:412-419`):

```diff
 SUnit *Succ = &DAG->ExitSU;
 for (MachineInstr &MI : reverse(CurRegion.bot_fixed_instrs())) {
-  SUnit &FixedSU = Scheduler->addFixedSUnit(MI, /*IsTop=*/false);
+  SUnit &FixedSU = Scheduler->markFixedSUnit(MI, /*IsTop=*/false);
   SDep Dep(&FixedSU, SDep::Artificial);
-  Dep.setLatency(Succ == &DAG->ExitSU ? 0 : 1);
+  // For the bottom-most chain edge, if MI is a delay-slot branch,
+  // anchor its cycle at "N from bot" via the chain latency. This
+  // replaces β.1 v1's N empty BUNDLE pseudo cycle anchors —
+  // single-entry chain achieves the same height without MIR mutation.
+  unsigned Lat = (Succ == &DAG->ExitSU) ? 0 : 1;
+  if (Succ == &DAG->ExitSU && MI.hasDelaySlot())
+    Lat = TII->getNumDelaySlots(MI);
+  Dep.setLatency(Lat);
   Succ->addPred(Dep);
   Succ = &FixedSU;
 }
+
+// Phase 0b: with free→fixed register edges now present in fixed SUs'
+// Preds, releasePredecessors iterates a multi-entry vector. A
+// cascading release of a free pred can fire getNextUnscheduledFixedInstr's
+// `BotReady == Height` assertion BEFORE the chain Pred has propagated.
+// Pre-set here from the chain-determined heights so the invariant
+// holds regardless of subsequent release order.
+for (MachineInstr &MI : CurRegion.bot_fixed_instrs())
+  if (SUnit *SU = DAG->getSUnit(&MI))
+    SU->BotReadyCycle = std::max<unsigned>(SU->BotReadyCycle,
+                                            SU->getHeight());
+for (MachineInstr &MI : CurRegion.top_fixed_instrs())
+  if (SUnit *SU = DAG->getSUnit(&MI))
+    SU->TopReadyCycle = std::max<unsigned>(SU->TopReadyCycle,
+                                            SU->getDepth());
 DAG->makeMaps();
```

#### `llvm/lib/Target/AIE/AIEInterBlockScheduling.{h,cpp}` — single-entry synthesis

Replace β.1 v1's `synthesiseDelaySlotBotFixed`
(`AIEInterBlockScheduling.cpp:747-780`) with a single-entry, region-aware
synthesis that fires for the **production** shape (delay-slot MI followed
by `DelayedSchedBarrier`) AND the trailing-no-barrier shape (rare, only
appears in hand-authored MIR):

```diff
 static void synthesiseDelaySlotBotFixed(BlockState &BS, MachineBasicBlock *BB,
+                                         MachineBasicBlock::iterator RegionBegin,
                                          MachineBasicBlock::iterator RegionEnd,
                                          const AIEBaseInstrInfo *TII) {
-  if (!BS.LocalBottomFixed.empty()) return;
-  if (RegionEnd != BB->end()) return;
-  if (RegionEnd == BB->begin()) return;
-  MachineBasicBlock::iterator BranchIt = std::prev(RegionEnd);
-  if (!BranchIt->hasDelaySlot()) return;
-  const unsigned N = TII->getNumDelaySlots(*BranchIt);
-  if (N == 0) return;
-
-  // Insert N empty BUNDLE pseudo MIs after the branch.
-  DebugLoc DL;
-  for (unsigned I = 0; I < N; ++I)
-    BuildMI(*BB, BB->end(), DL, TII->get(TargetOpcode::BUNDLE));
-
-  // Build BS.LocalBottomFixed = [branch_bundle, empty × N].
-  const AIEBaseMCFormats *FormatInterface = TII->getFormatInterface();
-  MachineBundle BranchBundle(FormatInterface);
-  BranchBundle.add(&*BranchIt);
-  BS.LocalBottomFixed.push_back(std::move(BranchBundle));
-  for (unsigned I = 0; I < N; ++I)
-    BS.LocalBottomFixed.emplace_back(FormatInterface);
+  // Find the unique delay-slot MI in the region. Mid-MBB CALLs produce
+  // their own DelayedSchedBarrier-trailed regions; trailing RET does too.
+  // Both shapes are caught by getDelaySlotInstr.
+  MachineInstr *Branch = getDelaySlotInstr(RegionBegin, RegionEnd);
+  if (!Branch) {
+    BS.RegionBottomFixed.emplace_back();  // empty backing for this region
+    return;
+  }
+
+  // Single-entry BotFixedBundles. Cycle anchoring is done by the chain
+  // edge `branch → ExitSU` with lat=getNumDelaySlots(branch) — see
+  // EmitFixedSUnits::createFixedSUDAGNodes. No MIR mutation needed.
+  const AIEBaseMCFormats *FormatInterface = TII->getFormatInterface();
+  MachineBundle BranchBundle(FormatInterface);
+  BranchBundle.add(Branch);
+  BS.RegionBottomFixed.emplace_back();
+  BS.RegionBottomFixed.back().push_back(std::move(BranchBundle));
 }
```

`BlockState` gains a per-region storage:

```diff
 std::vector<MachineBundle> LocalBottomFixed;
+
+ /// Per-region single-entry [branch_bundle] backing. Indexed by region
+ /// number. Empty inner vector when the region has no delay-slot MI.
+ std::vector<std::vector<MachineBundle>> RegionBottomFixed;
```

`enterRegion` selects the right backing for each region (production or
trailing shape); SWP `BottomInsert` continues to apply only when no
delay-slot MI is present (preheaders never end in delay-slot branches,
so the two paths don't collide):

```diff
 void InterBlockScheduling::enterRegion(...) {
   ...
-  synthesiseDelaySlotBotFixed(BS, BB, RegionEnd, TII);
+  synthesiseDelaySlotBotFixed(BS, BB, RegionBegin, RegionEnd, TII);
   ArrayRef<MachineBundle> TopFixedBundles = ...;
   ArrayRef<MachineBundle> BotFixedBundles;
-  if (RegionEnd == BB->end()) {
-    if (!BS.LocalBottomFixed.empty())
-      BotFixedBundles = ArrayRef<MachineBundle>(BS.LocalBottomFixed);
-    else
-      BotFixedBundles = ArrayRef<MachineBundle>(BS.BottomInsert);
+  if (!BS.RegionBottomFixed.empty() &&
+      !BS.RegionBottomFixed.back().empty()) {
+    BotFixedBundles = ArrayRef<MachineBundle>(BS.RegionBottomFixed.back());
+  } else if (RegionEnd == BB->end()) {
+    BotFixedBundles = ArrayRef<MachineBundle>(BS.BottomInsert);
   }
   BS.addRegion(BB, RegionBegin, RegionEnd, TopFixedBundles, BotFixedBundles);
 }
```

`Region` constructor invariants relaxed for the barrier-trailed shape
(`AIEInterBlockScheduling.cpp:1156`):

```diff
- assert(BotFixedBundles.empty() || End == BB->end());
+ // BotFixedBundles applies when the region's MIR ends at BB->end() OR
+ // at a DelayedSchedBarrier (production shape: branch immediately
+ // followed by the barrier).
+ assert(BotFixedBundles.empty() || End == BB->end() ||
+        (End != BB->end() && End->getOpcode() == AIE::DelayedSchedBarrier));
```

`bot_fixed_instrs()` (`AIEInterBlockScheduling.h:196`) anchored at
region end (= `ExitInstr->getIterator()` when set, else `BB->end()`):

```diff
 inline iterator_range<fixed_iterator> bot_fixed_instrs() const {
-  fixed_iterator FixedBegin = std::prev(BB->end(), BotFixedBundles.size());
-  return make_range(FixedBegin, BB->end());
+  fixed_iterator End = ExitInstr ? ExitInstr->getIterator() : BB->end();
+  fixed_iterator FixedBegin = std::prev(End, BotFixedBundles.size());
+  return make_range(FixedBegin, End);
 }
```

`top_fixed_instrs()` is unchanged — top fixed always anchors at `BB->begin()`.

### Why this is safe and minimal

**No MIR mutation.** We do not insert empty `BUNDLE` pseudos. The N
delay-slot positions in MIR remain whatever they are today (typically
just unscheduled cycles that the legacy machinery fills with NOPs or
free SUs). The branch's MIR position stays where it is.

**No commit-time bundling change.** The β.1.5 risk of
`materializeEmptyBundles` / `applyBundles` mishandling empty BUNDLE
pseudos is avoided entirely.

**Legacy machinery unchanged.** `RegionBottomUpCycles` floor
(`AIEMachineScheduler.cpp:451-464`) still fires:
`max(BotFixedBundles.size(), DelaySlotCycles+1) = max(1, N+1) = N+1`.
Bot zone correctly sized at N+1 cycles. Top-zone reject and tie-break
(`:502-509, 1013-1019`) still fire for free SUs in the delay-slot
region — they're irrelevant for the branch (now fixed) but defense in
depth for any other MIs in the region.

**Multi-region MBB safe.** Mid-MBB CALL + trailing RET produce two
regions, each with its own delay-slot MI. `getDelaySlotInstr` finds
each one in its own region. `RegionBottomFixed[r]` per-region storage
keeps them isolated. SWP push has no overlap because preheaders never
contain delay-slot terminators.

**Phase 0b register edges work as designed.** `buildEdges` runs over
the full SU set including the now-fixed branch. For `set.ll`,
`buildEdges` adds the `and → jz` register edge. The branch is
anchored at cycle N via the chain; `and` is forced strictly earlier
by the register edge. Branch correctness restored.

**Cat C resolved by construction.** `RET` in `bb.2` is now a fixed
bot SU. `getNextUnscheduledFixedInstr(BotZone)` returns it. The
fixed-SU bypass at `AIEMachineScheduler.cpp:665-673` returns
`DeltaCycles >= MinDelta` (always true for sane CurrCycle) without
calling `checkHazard`. The cycle-bumping loop in `pickOnlyChoice`
exits as soon as `Available` is non-empty.

### Critical files

* `llvm/lib/Target/AIE/AIEMachineScheduler.{h,cpp}` — `buildGraph`,
  `markFixedSUnit` (renamed from `addFixedSUnit`).
* `llvm/lib/Target/AIE/AIEBaseSubtarget.cpp` —
  `EmitFixedSUnits::createFixedSUDAGNodes` (chain lat + eager pre-set).
* `llvm/lib/Target/AIE/AIEInterBlockScheduling.{h,cpp}` —
  `synthesiseDelaySlotBotFixed`, `BlockState::RegionBottomFixed`,
  `enterRegion` per-region gating, `Region` constructor assert,
  `bot_fixed_instrs()` anchor.

### Verification

1. `ninja llc` (assertions on).
2. **Cat C regression check first**:
   `llc llvm/test/CodeGen/AIE/aie2/schedule/postpipeliner/bitwisenot.mir
    -mtriple=aie2 -run-pass=postmisched -o -` completes in ≤ 5s and
    produces valid output. Expected behavior:
   * `RET` is in `BotFixedBundles` for the trailing region of `bb.2`.
   * `getNextUnscheduledFixedInstr(BotZone)` returns `RET`'s SU.
   * `isAvailableNode(RET, BotZone)` short-circuits via the fixed-SU
     bypass without ever calling `checkHazard`.
   * Schedule completes; no infinite loop.
3. **set.ll branch correctness check**:
   `llc llvm/test/CodeGen/AIE/aie2/set.ll -mtriple=aie2` produces
    output where the `and r0, r0, r1` MI lands strictly before the
    `jz r0, exit` MI in MIR order. Spot-check: `buildEdges` added the
    register edge (`-debug-only=machine-scheduler` shows `SU(and) →
    SU(jz)` edge with non-zero latency; the SchedDAG dump confirms it
    is a `Data` edge, not artificial).
4. **Full codegen suite**:
   `ninja check-llvm-codegen-aie`. Expected outcomes:
   * **Best case (NFC):** 2145/2145 green. Means today's free-SU
     placement of delay-slot branches via tie-break + floor was
     equivalent to the new fixed-SU placement.
   * **Likely case** (a small set of SWP-test diffs): SWP exit/entry
     blocks tighten because real free↔fixed register edges replace the
     conservative `getLatencyCap` / `getBlockedResourceCap` for
     intra-MBB hops. For each diff:
     1. Confirm data-dep ordering preserved (no reads of stale regs).
     2. Confirm modulo / SWP invariants (no shift across SWP boundary).
     3. If both hold: regen with `update_*_test_checks.py`.
     4. Otherwise: investigate (= bug, not test churn).
5. **Unit tests**: `ninja check-llvm-unittests-target-aie` — 39/39.
6. **Spot-check** the trailing-no-barrier shape (rare): hand-author a
   `.mir` test with a trailing `RET` and no `DelayedSchedBarrier`,
   confirm the same fixed-SU path fires.

### What this plan does NOT change

* Empty BUNDLE pseudo insertion in MIR — not done. Avoided.
* `materializeEmptyBundles` / `applyBundles` commit-time bundling — not
  changed. Branch is still emitted in its MIR position; delay-slot
  cycles still emit NOPs or free-SU bundles via today's path.
* β.1 v1's existing dormant code (`synthesiseDelaySlotBotFixed`) —
  REPLACED with the single-entry version. The empty-pseudo logic is
  removed.
* Mid-MBB delay-slot regions — fully covered by this plan because
  `getDelaySlotInstr` is region-scoped (not MBB-scoped). Mid-MBB CALL
  in the production shape has its own region (split by the
  `DelayedSchedBarrier` after it), and the new synthesis fires for
  that region's branch.
* Cross-MBB projection scoreboards (Phase 2) — separate PR.

### Sequencing

* **Step A** — land Phase 0a + bundle-aware `MemoryEdges` helper as a
  single small commit (already in working tree, NFC verified at
  2145/2145).
* **Step B** — land Phase 0b structural change (buildGraph fixed-SU
  creation in MIR-with-reverse-bot order, `markFixedSUnit` rename,
  `removeFixedFixedRegisterEdges` strip, eager BotReady/TopReady
  pre-set in `createFixedSUDAGNodes`). Verify `bitwisenot.mir`
  no longer crashes in `ComputeDepth/Height` (the strip is what
  removes the SchedDAG cycle from modulo-cloned epilogue bundles);
  set.ll-style cases now have register edges (verify with debug dump).
* **Step C** — land single-entry synthesis + chain-latency
  augmentation. Verify Cat C resolved (`bitwisenot.mir` completes
  through scheduling, no infinite cycle-bumping in `pickOnlyChoice`),
  `set.ll` correct (`and → jz` data dep respected), full suite green
  or expected-tightened.
* **Step D** (if any test diffs) — regenerate with `update_*_test_checks.py`
  after hand-audit; commit with explicit "expected schedule tightening
  from real free↔fixed register edges" rationale.

Steps A, B, C are separate commits. Steps B and C must land together
or B must land first as a known-Cat-C-broken intermediate (ideally
avoided — prefer landing both at once if the only remaining issue
after Step B is Cat C, which Step C resolves by routing the branch
through the fixed-SU bypass).

### Status after current implementation attempt

Step A is in the working tree. Step B (Phase 0b structural +
`removeFixedFixedRegisterEdges` strip + eager BotReady/TopReady
pre-set) and Step C (single-entry synthesis + chain-lat augmentation)
are landed. **Verified working**:

* `bitwisenot.mir` runs to completion (Cat C resolved — RET as
  fixed-SU bypasses `checkHazard` in `isAvailableNode`).
* `set.ll` correctly places `and r0, r0, r1` strictly before
  `jz r0, .LBB0_2` in MIR (Phase 0b's free→fixed register edge is
  active and respected).

**Outstanding bug — 7 hangs in `check-llvm-codegen-aie`**:

The hangs share a single mechanism. `bitwisenot.mir`'s `bb.2` was
fixed because it had no free SUs in the region. Larger exit-block
shapes (e.g. `sched-initialize-topscb-once.ll` with a SWP epilogue
+ call-arg setup + `JL_IND`) DO have free SUs that get stuck in
`Bot.Pending` after the fixed branch is bypass-scheduled. The
mechanism is **Conservative bot scoreboard creates an implicit
"post-exit reservation" that's not modeled in the SchedDAG**, and
the free SUs' low `BotReadyCycle` collides with that implicit
reservation in a way that triggers `pickOnlyChoice`'s lockstep
fall-through.

**Concrete trace** (load_store_with_call, bb.exit, `BotCurrCycle=6`
after `JL_IND` scheduled at chain anchor cycle 5):

1. `mova p1.BotReadyCycle = 1` (from `RegionEndEdges` edge to
   `ExitSU` with `lat = MaxLatency(mova) = 1`).
2. `BotCurrCycle - BotReady = 5` → scan starts at `DeltaCycles = +5`.
3. `Conservative` bot scoreboard primes `Depth ≈ 10` cycles with
   ALL slots/FUs busy (this represents "we don't know what the
   post-exit code does, assume worst"). After `recedeScoreboard`,
   the blocked positions cover the cycles within `Depth` of MBB end.
4. Scan iterates `5, 4, 3, 2, 1, 0, -1, …, -128`. Every offset in
   the band lands in the conservatively-blocked region (or hits
   `JL_IND`'s freshly-emitted resources at cycle 5). `checkHazard`
   returns hazard for the entire scan range.
5. Fall-through: `mova.BotReadyCycle = max(1, 6 + 128) = 134`.
6. Cycle bump: `BotCurrCycle = 7`, `BotReady = 134`,
   `DeltaCycles = -127`. Scan range = `[-128, -127]`. Both blocked.
   `BotReady = 135`. **Lockstep growth of 1/cycle**, unbounded.
7. `Available` never non-empty → `pickNodeAndCycle` never re-enters
   → `if (!IsTopDown && BotCurrCycle >= RegionBottomUpCycles)`
   fallback to top-down never fires. Hang.

**Why not pre-existing**: before Phase 0b's structural change,
`JL_IND` was a FREE SU, going through the hazard scan along with
`mova`. It also fell through. But because all of `JL_IND`,
`mova p1..p4` were initially in `Pending` simultaneously (none in
`Available`), and the scoreboard at `BotCurrCycle = 0` had a wider
NoHazard window past the conservative blockage, the scheduler
scheduled SUs in NodeOrder/MIR-position order *before* the scan
exhausted itself. With my fixed-SU bypass scheduling `JL_IND`
first at cycle 5, `BotCurrCycle` jumps to 6, and now the free SUs
face the scan starting at `DeltaCycles = +5` — entirely inside
the blocked band — and lockstep starts.

### The fix: scoreboard-aware `SU → ExitSU` edge

Make the implicit "post-exit reservation" explicit in the SchedDAG.

**Math**. For free SU `F` with data latency `M = MaxLatency(F)`:
- F at `F → ExitSU` distance `D` reserves bot cycles `[D − M + 1, D]`.
- Conservative scoreboard blocks bot cycles `[0, K − 1]` (K = Conservative
  band depth, ≈ `Depth` extended by `getBlockedResourceCap` margin).
- No-conflict requirement: `D − M + 1 > K − 1`, i.e. **`D ≥ K + M − 1`**.

The existing `RegionEndEdges` sets `D = M`. That satisfies the data-flow
"result available by ExitSU" constraint but ignores the conservative
band: F's reservation `[1, M]` collides with the band `[0, K − 1]`.
The fix raises `D` to at least `K + M − 1` whenever the scoreboard
shows a conflict.

**Implementation**. Probe the actual scoreboard rather than computing
`K + M − 1` from priming parameters — the probe loop converges to it
naturally and also captures any non-Conservative priming sources
(successor-MBB tail, AccountForAlign slack, ZOL margin):

```cpp
// In a sibling mutator to RegionEndEdges, scheduled AFTER it but
// BEFORE EmitFixedSUnits.
class ScoreboardAwareExitEdges : public ScheduleDAGMutation {
  void apply(ScheduleDAGInstrs *DAG) override {
    auto *Sched  = static_cast<AIEScheduleDAGMI*>(DAG)->getSchedImpl();
    auto *BotHR  = Sched->getAIEHazardRecognizer(/*Bot=*/true);

    // Probe scoreboard primed identically to the runtime bot scoreboard
    // (Conservative blocking + AccountForAlign + successor-tail replay
    // + ZOL margin). Factor the priming logic out of
    // initializeBotScoreBoard so this mutator and runtime share it.
    ResourceScoreboard<FuncUnitWrapper> Probe = primeBotScoreboard(...);

    SmallPtrSet<const MachineInstr*, 16> FixedMIs;
    for (const MachineInstr &MI : Region.top_fixed_instrs()) FixedMIs.insert(&MI);
    for (const MachineInstr &MI : Region.bot_fixed_instrs()) FixedMIs.insert(&MI);

    for (SUnit &SU : DAG->SUnits) {
      const MachineInstr *MI = SU.getInstr();
      if (FixedMIs.count(MI)) continue;        // fixed SUs anchored by chain

      // Walk the F → ExitSU distance upward from the existing edge
      // latency until F's reservation no longer hits a blocked cycle.
      // Probe.hasConflict handles MI's data-latency span automatically;
      // the loop converges to D = K + M − 1 (or higher if other priming
      // sources extend the blocked region).
      unsigned SafeDistance = MaxLatency(MI);   // existing lower bound (= M)
      while (Probe.hasConflict(*MI, /*offset=*/-SafeDistance)) {
        if (++SafeDistance >= BotHR->getMaxLookAhead())
          break;                                 // saturate; do not exceed buffer
      }

      // Upgrade edge F → ExitSU to SafeDistance if larger than what
      // RegionEndEdges already added.
      if (SafeDistance > existingEdgeLatency(SU, DAG->ExitSU)) {
        SDep Dep(&SU, SDep::Artificial);
        Dep.setLatency(SafeDistance);
        DAG->ExitSU.addPred(Dep, /*Required=*/true);
      }
    }
  }
};
```

**What this changes for the hanging case**:

| | before fix | after fix |
|---|---|---|
| `mova.BotReadyCycle` (initial) | 1 | `SafeDistance` (≥ Depth ≈ 10) |
| First `DeltaCycles` checked when bot zone considers `mova` | `BotCurrCycle - 1` (often positive, in blocked band) | `BotCurrCycle - SafeDistance` (≤ 0, near current cycle) |
| Scan range | starts at high positive, traverses blocked future cycles before reaching unblocked past | starts at 0 or negative, immediately within scoreboard's productive window |
| Fall-through path | hit on every call → lockstep growth | not reached on the first call → SU schedules cleanly |

**Subsumes existing mechanisms**:

* The existing `establishSafeFreeSUToPrologueDistances` in
  `AIEBaseSubtarget.cpp:455-513` is a special case of this — it
  considers register/memory events from a SWP loop's first iter
  but not generic scoreboard blocking. Generalizing means one
  mechanism instead of two parallel ones.
* The "delay-slot count + 1" boost previously considered as a
  patch is also subsumed: for a delay-slot region, the scoreboard
  probe naturally finds `SafeDistance ≥ N + 1` because the
  delay-slot reservation blocks those cycles.

**Subsumes `removeFixedFixedRegisterEdges`'s sister problem**: the
existing strip removes register edges between fixed SUs (which
would otherwise inflate fixed-SU heights past their chain anchor).
The scoreboard-aware exit edge is the analog for free SUs — it
prevents under-estimation of free-SU heights when an implicit
reservation makes their natural BotReady untenable.

**Architecturally**: replaces an implicit constraint (scoreboard
priming) with an explicit one (DAG edge). The scoreboard becomes
purely about cycle-level resource arbitration during the scheduling
step; cross-MBB reservations are encoded in the DAG.

### Critical files to modify

* `llvm/lib/Target/AIE/AIEBaseSubtarget.cpp`:
  * Factor `initializeBotScoreBoard`'s priming logic into a helper
    callable from both runtime and the new mutator.
  * Add `ScoreboardAwareExitEdges` mutator (or extend
    `RegionEndEdges`).
  * Register it in the `Mutations.emplace_back(...)` ladder
    (around `:967-984`), AFTER `RegionEndEdges` and `MemoryEdges`,
    BEFORE `EmitFixedSUnits` (so it sees the post-strip DAG and
    upgrades non-Artificial edges' latency, but doesn't touch the
    chain edges added by `EmitFixedSUnits` later).
* `llvm/lib/Target/AIE/AIEMachineScheduler.cpp`:
  * `initializeBotScoreBoard` calls the shared priming helper.

### Verification

1. `ninja llc` (assertions on).
2. `bitwisenot.mir` still passes (NFC for the simple case).
3. `set.ll` still places `and` before `jz` (NFC for the
   correctness case).
4. **All 7 hanging tests now complete in ≤ 5s each**:
   `aie2/schedule/postpipeliner/conv2d.mir`,
   `aie2p/end-to-end/{add-att-broadcasting-aa, conv2d-dw-bf16,
   conv2d_bfp16_kernel_red, gemm-bfp16}.ll`,
   `aie2p/schedule/events-and-post-swp.mir`,
   `schedule/sched-initialize-topscb-once.ll`.
5. `check-llvm-codegen-aie` end-to-end: expect 4 schedule diffs
   (bundle layout / 1-cycle tightening from real free↔fixed
   register edges) — hand-audit and regenerate with
   `update_*_test_checks.py`. No more failures.
6. `check-llvm-unittests-target-aie` 39/39.

---

## Historical context: previous Phase 0b plan (kept for reference)

### Why now

The β.1.5 attempt (twice rolled back) revealed that the existing
fixed-SU machinery is unsound for any case where a fixed SU has
register-level data dependencies on free SUs in the same region.
Concretely, in `aie2/set.ll`:

* `and r0, r0, r1` (free SU) defines `r0`.
* `jz r0, target` (after synthesis: fixed SU) reads `r0`.
* The legal cycle ordering is `cycle(and) < cycle(jz)`.
* In the rolled-back synthesis, `and` was placed at delay slot 1
  (cycle T+5) while `jz` issued at cycle T. The branch read the
  stale input `r0` (= `idx`) instead of `idx & 1`. Different program.

Root cause is structural in `AIEPostRASchedStrategy::buildGraph`
(`llvm/lib/Target/AIE/AIEMachineScheduler.cpp:1488-1528`): SUnits
are created **only for `Region.getFreeInstructions()`** before
`DAG.buildEdges` runs. Fixed SUnits are added later by the
`EmitFixedSUnits` mutator (`llvm/lib/Target/AIE/AIEBaseSubtarget.cpp:356`)
which adds **only artificial chain edges** to anchor cycle distance
— it does not (and cannot post-buildEdges) add register-dep edges.
The result: fixed SUs have no register-level visibility into the
SchedDAG, and the scheduler is free to violate data deps.

This bug is latent for the existing SWP use of fixed bundles
(SWP-pushed prologue/epilogue instructions don't have intra-region
register deps to free SUs in the surrounding MBB — register flow
across the SWP boundary is handled coarsely by
`getLatencyCap` / `getBlockedResourceCap`). The bug surfaces the
moment we try to fix a delay-slot branch as an SU, because branches
*do* have intra-region register-read deps.

### The change

Move SUnit creation for fixed instructions from the
`EmitFixedSUnits` mutator into `AIEPostRASchedStrategy::buildGraph`
so that **all** SUnits (free + fixed) exist before `DAG.buildEdges`
runs. `buildEdges` then computes register def→use edges between
every pair, including free→fixed. The mutator's job shrinks to
"look up the existing SU and add the artificial chain edge" — no
SU creation.

```diff
 void AIEPostRASchedStrategy::buildGraph(...) {
   DAG.clearDAG();
   ...
   for (int S = 0; S < NCopies; S++) {
+    // Top-fixed first (in MIR order), then free, then bot-fixed.
+    // This matches the MIR layout and keeps SU NodeNum monotonic
+    // with cycle ordering, which the existing fixed-SU range
+    // bookkeeping (FirstTopFixedSU / FirstBotFixedSU /
+    // LastBotFixedSU) relies on.
+    for (MachineInstr &MI : Region.top_fixed_instrs())
+      DAG.initSUnit(MI);
     for (MachineInstr *I : Region.getFreeInstructions())
       DAG.initSUnit(*I);
+    for (MachineInstr &MI : Region.bot_fixed_instrs())
+      DAG.initSUnit(MI);
   }
   DAG.ExitSU.setInstr(Region.getExitInstr());
   DAG.makeMaps();
   DAG.buildEdges(Context->AA);   // now sees ALL SUs
 }
```

```diff
-SUnit &AIEPostRASchedStrategy::addFixedSUnit(MachineInstr &MI, bool IsTop) {
-  ...
-  unsigned SUNum = DAG->initSUnit(MI).value();
-  SUnit &SU = DAG->SUnits[SUNum];
+SUnit &AIEPostRASchedStrategy::markFixedSUnit(MachineInstr &MI, bool IsTop) {
+  // SUnit was already created in buildGraph; just look it up and
+  // record its NodeNum in the appropriate fixed-range bookkeeping.
+  SUnit *SU = DAG->getSUnit(&MI);
+  assert(SU && "Fixed MI must already have an SUnit from buildGraph");
+  unsigned SUNum = SU->NodeNum;
   if (IsTop) {
     if (!FirstTopFixedSU) FirstTopFixedSU = SUNum;
   } else {
     if (!FirstBotFixedSU) FirstBotFixedSU = SUNum;
     LastBotFixedSU = SUNum;
   }
-  return SU;
+  return *SU;
 }
```

`EmitFixedSUnits::createFixedSUDAGNodes`
(`AIEBaseSubtarget.cpp:360-387`) calls `markFixedSUnit` instead of
`addFixedSUnit`. The artificial-chain edge logic stays unchanged.

### Why the post-pipeliner is unaffected

* **Pipelining stage** (`SchedulingStage::Pipelining`,
  `AIEMachineScheduler.cpp:1575-1589`): asserts both
  `Region.getTopFixedBundles().empty()` and
  `Region.getBotFixedBundles().empty()` before driving its own
  modulo schedule. The new `top_fixed_instrs()` / `bot_fixed_instrs()`
  loops in `buildGraph` therefore iterate empty ranges. `NCopies = 2`
  for pipelining still creates two copies of the free SUs only.
* **PostPipeliner internals** (`AIEPostPipeliner.{h,cpp}`): zero
  references to `addFixedSUnit`, `EmitFixedSUnits`,
  `FirstTopFixedSU` / `FirstBotFixedSU` / `LastBotFixedSU`. Its
  `BoundaryNode`-skipping (`AIEPostPipeliner.cpp:228, 247, 784, 891`)
  iterates `[FirstUnscheduled, LastUnscheduled)` on its own
  bookkeeping, untouched by extra fixed SUnits if any were ever to
  appear.
* **Non-pipelining loop stages**: fall through to
  `ScheduleDAGMI::schedule()` (`:1591-1593`), the standard path.
  Fixed SUnits today are also empty for those stages because the
  loop body runs through the post-pipeliner gathering phase first.

### Expected behavior delta on existing tests

For SWP push (Epilogue's `TopFixedBundles`, PreHeader's
`BottomFixedBundles`): today the SWP-pushed instructions have **no
register-dep edges** to free SUs in the surrounding MBB. The
conservative caps (`getLatencyCap` / `getBlockedResourceCap`) keep
schedules sound. With this change, real register edges replace the
conservative caps' role for this specific intra-MBB hop, so:

* If today's caps were tight enough to imply the same constraints
  as the real edges → NFC.
* If today's caps were over-conservative (more common case) → the
  scheduler can now schedule more aggressively → potentially
  tighter schedules in some SWP exit/entry blocks. Those tests
  would need regeneration; expected diffs are NOPs disappearing,
  not different ordering.
* If a test reveals the caps were *under-conservative* (unlikely but
  possible) → that test was masking a latent bug. Treat as bug to
  fix, not as test churn.

For non-SWP MBBs without fixed bundles: NFC.

### Files to modify

* `llvm/lib/Target/AIE/AIEMachineScheduler.cpp`
  * `AIEPostRASchedStrategy::buildGraph` (`:1488-1528`): add
    fixed-MI SUnit creation in MIR-order interleaved with free-MI
    creation.
  * `addFixedSUnit` (`:1531-1551`): rename to `markFixedSUnit`,
    change body from `DAG->initSUnit(MI).value()` to
    `DAG->getSUnit(&MI)->NodeNum`. Keep the assertion `!(IsTop &&
    FirstBotFixedSU)` and the FirstTopFixedSU/FirstBotFixedSU/
    LastBotFixedSU bookkeeping intact.
* `llvm/lib/Target/AIE/AIEMachineScheduler.h`
  * Rename the public declaration of `addFixedSUnit` →
    `markFixedSUnit` to match.
* `llvm/lib/Target/AIE/AIEBaseSubtarget.cpp`
  * `EmitFixedSUnits::createFixedSUDAGNodes` (`:360-387`):
    `Scheduler->addFixedSUnit(...)` → `Scheduler->markFixedSUnit(...)`
    in both the top and bot loops. Chain logic unchanged.

### Two-step Phase 0

The change has to land in two steps because of an asymmetric-latency
trick in `RegionEndEdges` (AIEBaseSubtarget.cpp:259-336) that interacts
badly with fixed SUs once they're in the SchedDAG.

#### What `RegionEndEdges` does and the K/K-1 asymmetry

`RegionEndEdges` removes `ExitSU.Preds` wholesale and re-adds an
artificial Order edge `SU → ExitSU` for every SU with
`lat = MaxLatency(MI)` — the SU's pipeline depth K. It then walks
`ExitSU.Preds` again and reduces the latency to `K-1`. The Succs side
on each SU stays at K. The asymmetry is intentional and documented in
the source: it lets a pipeline-K instruction issue at cycle K-1 (the
K-1th from bottom) so its pipeline completes EXACTLY at the region
boundary, instead of leaving the bottom K-1 cycles as NOPs.

For a free SU the asymmetry is invisible — `BotReadyCycle = K-1` is
just a placement constraint, `Height = K` is the chain-positioning
view, and the scheduler uses `BotReadyCycle`. They're never compared.

For a fixed SU, `getNextUnscheduledFixedInstr` asserts
`BotReadyCycle == Height`. The fixed-SU machinery uses the artificial
chain (lat=0/1, **symmetric**) and assumes no asymmetry. With fixed
SUs in the SchedDAG before `RegionEndEdges` runs, the K/K-1
asymmetry leaks in: `Height = K`, `BotReadyCycle = K-1`, assert fires.

#### Phase 0a — `RegionEndEdges` skips fixed SUs

Skip fixed SUs in `RegionEndEdges`'s loop. The skip is by MIR
position (top_fixed_instrs() ∪ bot_fixed_instrs()). NFC today: no
fixed SUs exist when `RegionEndEdges` runs (the later
`EmitFixedSUnits` mutator creates them), so the predicate matches
nothing.

```diff
 class RegionEndEdges : public ScheduleDAGMutation {
   ...
   void apply(ScheduleDAGInstrs *DAG) override {
     ...
+    // Identify fixed-SU MIs by Region position. Skip them — they
+    // are cycle-anchored by the artificial chain installed in
+    // EmitFixedSUnits and should not receive the K/K-1 asymmetric
+    // edge that pulls free SUs into the bottom K-1 cycles. Today
+    // this set is empty (fixed SUs are created in EmitFixedSUnits,
+    // which runs LATER), so this predicate matches nothing — pure
+    // NFC. Phase 0b makes fixed SUs exist at this point and the
+    // skip starts mattering.
+    SmallPtrSet<const MachineInstr *, 16> FixedMIs;
+    if (auto *Sched = static_cast<AIEScheduleDAGMI *>(DAG)->getSchedImpl()) {
+      const Region &Reg = Sched->getInterBlock()
+          .getBlockState(DAG->getBB()).getCurrentRegion();
+      for (const MachineInstr &MI : Reg.top_fixed_instrs())
+        FixedMIs.insert(&MI);
+      for (const MachineInstr &MI : Reg.bot_fixed_instrs())
+        FixedMIs.insert(&MI);
+    }
+
     for (SUnit &SU : DAG->SUnits) {
       MachineInstr &MI = *SU.getInstr();
+      if (FixedMIs.count(&MI))
+        continue;
       ...
     }
     ...
   }
 };
```

The matching skip in the second pass (`for (SDep &PredEdge : DAG->ExitSU.Preds)`)
is implicit because no edges to `ExitSU` were added for fixed SUs in
the first pass.

##### Phase 0a verification

`ninja check-llvm-codegen-aie` must be 100% green and bit-identical
to baseline. Any diff is a bug — the predicate matches no MIs in the
current code.

##### Phase 0a files

* `llvm/lib/Target/AIE/AIEBaseSubtarget.cpp` — `RegionEndEdges::apply`
  picks up the skip predicate.

#### Phase 0b — fixed SUs in the SchedDAG

This is the original "Make the SchedDAG include fixed SUnits" change.
Now sound because Phase 0a removed the K/K-1 asymmetry that would
otherwise corrupt fixed-SU heights.

##### Bug uncovered during Phase 0b implementation: `FirstBotFixedSU` range inversion

The original `addFixedSUnit` (`AIEMachineScheduler.cpp:1531`) sets
`FirstBotFixedSU` with `if (!FirstBotFixedSU) FirstBotFixedSU = SUNum;`
— i.e. only on the first call. This was correct in the old code
because `initSUnit` was called from INSIDE `addFixedSUnit` during
`createFixedSUDAGNodes`'s **reverse** iteration over
`bot_fixed_instrs`. SU NodeNums tracked CREATION ORDER. The first
call (= last bot MI in MIR = bottom-most) created the LOWEST
NodeNum. So `FirstBotFixedSU = lowest`, `LastBotFixedSU = highest`,
range covers all bot-fixed SUs correctly.

After Phase 0b, SUs are created in `buildGraph` in MIR order, so
NodeNums track MIR ORDER (top-fixed[0..T-1] → free[T..T+F-1] →
bot-fixed[T+F..T+F+B-1]). `createFixedSUDAGNodes` still iterates
`reverse(bot_fixed_instrs)`, so `markFixedSUnit`'s first call sees
the HIGHEST NodeNum. Result: `FirstBotFixedSU = highest`,
`LastBotFixedSU = lowest`. Range is **inverted** — empty for all
NodeNums.

`isFixedSU(SU, false)` then returns `false` for every bot-fixed
SU. The `IsNonBotFixedSU` filter in
`establishSafeFreeSUToPrologueDistances`
(`AIEBaseSubtarget.cpp:474-478`) lets bot-fixed SUs slip through,
and the function adds artificial `lat=K` edges to ExitSU for them.
The later chain edge (`lat=0`) gets latency-MAX'd to `lat=K` by
`addPred` dedup. `BotReadyCycle == getHeight()` fails, scheduler
crashes.

**Fix as part of Phase 0b: match `createFixedSUDAGNodes`'s
iteration order in `buildGraph`.** Iterate bot-fixed in REVERSE
MIR order in `buildGraph` so SU creation order aligns with the
chain creation order. Then the OLD `if (!FirstBotFixedSU)`
semantics keep working — first call = last MIR bot-fixed = lowest
NodeNum, last call = first MIR bot-fixed = highest NodeNum, range
is correct. `markFixedSUnit` body stays as the OLD `addFixedSUnit`
(modulo the `initSUnit` → `getSUnit` swap and the rename).

```diff
 for (int S = 0; S < NCopies; S++) {
   for (MachineInstr &MI : Region.top_fixed_instrs())
     DAG.initSUnit(MI);         // forward — matches top chain iteration
   for (MachineInstr *I : Region.getFreeInstructions())
     DAG.initSUnit(*I);
-  for (MachineInstr &MI : Region.bot_fixed_instrs())
+  for (MachineInstr &MI : reverse(Region.bot_fixed_instrs()))
     DAG.initSUnit(MI);         // reverse — matches bot chain iteration
 }
```

Rationale: the OLD code's NodeNum-vs-creation-order invariant
(NodeNums track creation order, which is reverse MIR for bot
fixed) is what makes `if (!FirstBotFixedSU)` correct. Phase 0b's
job is to preserve that invariant by mirroring the iteration
direction in `buildGraph`. This keeps the change minimal and
behavior-preserving.

##### Second issue: `Preds` iteration order with free→fixed register edges

After the reverse-iteration fix above, AIE2 tests pass but the
AIE2P `Conv2D_bfp16_conv.mir` (and similar) still fails the
`BotReadyCycle == getHeight()` assertion in
`getNextUnscheduledFixedInstr` for a bot-fixed SU.

**Root cause**: Phase 0b's whole point is to make `buildEdges` see
fixed SUs so it can create register edges between free SUs and
fixed SUs (this is what fixes `set.ll`'s `and → jz` data dep).
Concretely, a bot-fixed SU like `SU#20` (a `VLDB`) ends up with
`Preds` = {register edges from free SUs (8 entries), chain edge
from `SU#21` (1 entry, added LAST by `EmitFixedSUnits`)}.

When `SU#20` schedules, `releasePredecessors(SU#20)` walks
`SU#20.Preds` in vector order. Processing the FIRST register-edge
Pred decrements that free SU's `NumSuccsLeft`; if it hits 0,
`releaseBottomNode(free)` cascades to
`Bot.releaseNode(free) → isAvailableNode(free) →
getNextUnscheduledFixedInstr` — and the assertion fires for the
NEXT bot-fixed SU (`SU#21`). At this moment, `SU#21.BotReadyCycle`
hasn't been propagated yet (the chain Pred is the LAST entry in
`SU#20.Preds` and we're still on the FIRST). So `BotReadyCycle = 0`,
`Height = 4`, mismatch.

In the OLD code, fixed SUs had only ONE Pred (the chain edge), so
no iteration-order issue arose; `BotReadyCycle == Height` held by
construction. Phase 0b changes the `Preds` shape, which changes
the timing of lazy propagation.

**Fix (chosen — Option 1): pre-propagate `BotReadyCycle`/`TopReadyCycle`
right after chain creation.** In
`EmitFixedSUnits::createFixedSUDAGNodes`, after the chain edges
are added, walk the fixed SUs and set
`BotReadyCycle = max(BotReadyCycle, getHeight())` for bot-fixed
and `TopReadyCycle = max(TopReadyCycle, getDepth())` for top-fixed.
The chain just defined the cycle anchor; this makes that anchor
visible to the scheduler immediately rather than waiting for lazy
`releasePred` propagation.

```diff
 SUnit *Succ = &DAG->ExitSU;
 for (MachineInstr &MI : reverse(CurRegion.bot_fixed_instrs())) {
   ...
   Succ->addPred(Dep);
   Succ = &FixedSU;
 }
+
+// After chain creation, eagerly propagate the chain-determined
+// BotReadyCycle / TopReadyCycle onto fixed SUs. With Phase 0b's
+// free→fixed register edges, a fixed SU's Preds now mixes the
+// chain edge with register edges. releasePredecessors iterates
+// in vector order; register-edge releases of free SUs may
+// cascade into getNextUnscheduledFixedInstr's assertion BEFORE
+// the chain edge has been processed, leaving the next fixed
+// SU's BotReadyCycle stale (= 0). Pre-setting it here makes the
+// invariant hold regardless of subsequent iteration order.
+//
+// After my outgoing-edge strip, fixed SUs have only the chain
+// edge in Succs, so subsequent releasePred calls re-set
+// BotReady = max(Height, succ.BotReady + chain_lat) = Height —
+// a no-op. The pre-set value is stable.
+for (MachineInstr &MI : CurRegion.bot_fixed_instrs())
+  if (SUnit *SU = DAG->getSUnit(&MI))
+    SU->BotReadyCycle = std::max<unsigned>(SU->BotReadyCycle,
+                                            SU->getHeight());
+for (MachineInstr &MI : CurRegion.top_fixed_instrs())
+  if (SUnit *SU = DAG->getSUnit(&MI))
+    SU->TopReadyCycle = std::max<unsigned>(SU->TopReadyCycle,
+                                            SU->getDepth());

 DAG->makeMaps();
```

**Why Option 1 over weakening the assertion to `>=`**: the
strict `BotReady == Height` invariant is what locks fixed SUs at
their EXACT chain-anchored cycles. Weakening to `>=` would let
fixed SUs drift past the chain anchor, which silently breaks SWP
modulo schedules (prologue defs feeding the loop body at the
wrong modulo cycle) and creates a layer disagreement (MIR
position locked but scheduler-cycle drifts). Option 1 preserves
the OLD semantics exactly; it just restores the timing of the
invariant in a world where Preds are no longer single-entry.

##### Phase 0b verification (after Option 1 lands)

* `ninja llc && ninja check-llvm-codegen-aie`. Best case: NFC at
  2146/2146. Likely case: a small set of SWP-test diffs (NOPs
  disappearing) because real free↔fixed register edges replace
  the conservative `getLatencyCap`/`getBlockedResourceCap` caps
  for intra-MBB hops. For each diff: hand-audit data-dep
  ordering, regen with `update_*_test_checks.py`. If schedules
  reorder operations or shift modulo cycles → bug, investigate.
* `ninja check-llvm-unittests-target-aie` — 39/39.

##### Phase 0b status — IN PROGRESS via Option B (proper integration)

Phase 0b's structural change (fixed SUs in the DAG before
`buildEdges` runs) unblocks Phase 1's delay-slot branch
correctness work but exposes three categories of failures
because pre-existing AIE-specific scheduler code paths assume
the old "fixed SUs are added later by `EmitFixedSUnits`"
contract. Each affected code path needs auditing and patching.

**Failure categories observed:**

* **Category A — `MemoryEdges` fatal "Missing memory latency
  info"**. With BUNDLE pseudos in the SchedDAG (= SWP-pushed
  bundle headers), `MI.getDesc().getSchedClass()` returns 0
  (BUNDLE pseudo has no MCID::SchedClass). `getMemoryLatency(0,
  ...)` returns `nullopt` and the `ExactLatencies=true` path
  reports `fatal_error`.
* **Category B — `BotReadyCycle == getHeight()` assertion**. With
  free→fixed register edges (which Phase 0b's whole point is to
  add), a fixed SU's `Preds` mixes register edges from free SUs
  (front of vector) with the chain edge from `EmitFixedSUnits`
  (tail). `releasePredecessors` walks `Preds` in vector order;
  cascading release of a free SU's pred fires the assertion on
  the NEXT fixed SU before its chain Pred has propagated.
* **Category C — Infinite cycle-bumping in bot zone** (e.g.
  `bitwisenot.mir`). After scheduling some top fixed SUs and
  switching to bot zone, the scheduler can't find an available
  SU and bumps `CurrCycle` ~1.8M times. Suspected interaction
  between Phase 0b's strip and the AIE-specific free-SU queue
  flow that releases delay-slot terminators (RET) to bot zone
  via `releasePredecessors(&ExitSU)`. Not yet root-caused.

**Approach (Option B — proper integration)**: audit each AIE
mutator and `AIEBaseInstrInfo` query that operates on potentially-
bundled MIs in the SchedDAG, and teach it to handle BUNDLE
pseudos by walking their inner MIs. Patch each site.

Where the code path queries MEMORY latency between two MIs, use
the **most conservative latency across all (src_inner,
dst_inner) memory-op pairs** — concretely
`max(LastMemoryCycle of src inner mems) - min(FirstMemoryCycle
of dst inner mems) + 1`. Required because two memory ops in the
same VLIW slot can have different latencies (e.g. `VLD` pseudo
and `VLD_Fill`); the dependency must respect the worst case.

##### Step 1: bundle-aware memory latency helpers

Add to `llvm/lib/Target/AIE/AIEBaseInstrInfo.{h,cpp}` (companions
to existing `getMemoryLatency` / `getLastMemoryCycle` /
`getFirstMemoryCycle` at `:648,661,668`):

```cpp
// For a MI that is either a normal MI or a BUNDLE pseudo wrapping
// inner MIs, return the maximum LastMemoryCycle across its
// memory-op MIs. Returns nullopt if no inner MI has memory cycle
// info.
std::optional<int>
AIEBaseInstrInfo::getMaxLastMemoryCycle(const MachineInstr &MI) const;

// Symmetric: minimum FirstMemoryCycle across memory-op MIs.
std::optional<int>
AIEBaseInstrInfo::getMinFirstMemoryCycle(const MachineInstr &MI) const;

// Convenience wrapper that handles bundles end-to-end.
std::optional<int>
AIEBaseInstrInfo::getMaxMemoryLatency(const MachineInstr &SrcMI,
                                      const MachineInstr &DstMI) const;
```

Implementation walks bundle inner MIs via
`MachineBasicBlock::instr_iterator` (or `MIBundleOperands`
analog), computes the max/min across inner memory ops, and
returns the worst-case latency. Single-MI case (= MI is not a
BUNDLE pseudo) reduces to today's `getMemoryLatency` behavior.

##### Step 2: extend MemoryEdges to use bundle-aware helper

Modify `MemoryEdges::apply` in
`llvm/lib/Target/AIE/AIEBaseSubtarget.cpp:715-757`:

```diff
-std::optional<int> MemLat = TII->getMemoryLatency(
-    SrcMI.getDesc().getSchedClass(), MI.getDesc().getSchedClass());
+std::optional<int> MemLat = TII->getMaxMemoryLatency(SrcMI, MI);
 int Latency = 1;
 if (MemLat.has_value()) {
   Latency = *MemLat;
 } else if (ExactLatencies) {
   ...
   report_fatal_error("Missing memory latency info.");
 }
```

This preserves processing of fixed SUs (they still have memory
deps that matter for free-SU placement) while computing the
latency from the actual memory ops inside the bundle, with the
worst-case across multiple ops in the same VLIW slot.

##### Step 3: address Category C (infinite cycle-bumping)

Once Step 1 + Step 2 fix Category A, the rolled-back Phase 0b's
runtime hang in `bitwisenot.mir` should resurface for proper
investigation. Likely angles to investigate:

* Strip is removing a critical edge that broke a release path.
  Specifically: `RegionEndEdges` (Phase 0a) + my outgoing-edge
  strip together leave SOME SU without an outgoing edge to
  ExitSU OR without a Pred from EntrySU. That SU is then never
  released to any zone and the scheduler gets stuck.
* Investigation tools: enable `-debug-only=machine-scheduler`,
  inspect the SchedDAG just before scheduling, identify SUs with
  `NumPredsLeft > 0 && NumSuccsLeft > 0` that never schedule,
  and trace why their Preds/Succs don't release them.

This step is open-ended; Step 1 + Step 2 are mechanical and
unblock the investigation.

##### Step 4: audit other code paths that query SchedClass

After Step 2, audit and patch each remaining
`getDesc().getSchedClass()` query site that processes a MI from
the SchedDAG (any of the Pred/Succ edges could now point at a
BUNDLE pseudo SU):

* `LockDelays::apply` (`AIEBaseSubtarget.cpp:186-235`): queries
  `TII->getLastMemoryCycle(LdSt->getDesc().SchedClass)` and
  `TII->getFirstMemoryCycle(LdSt->getDesc().SchedClass)` where
  `LdSt` could be a BUNDLE pseudo. Update to use the helpers.
* Any other site grep'd from
  `getDesc().getSchedClass()` / `getDesc().SchedClass` that
  takes its MI from a SchedDAG SUnit.

##### Phase 0b verification (after Steps 1-4 land)

* `ninja llc && ninja check-llvm-codegen-aie`. Best case: NFC at
  2146/2146. Likely: a small set of SWP-test diffs (tighter
  schedules) because the bundle-aware memory latency replaces
  conservative defaults. For each diff: hand-audit data-dep
  ordering, regen with `update_*_test_checks.py`. If schedules
  reorder operations or shift modulo cycles → bug, investigate.
* `ninja check-llvm-unittests-target-aie` — 39/39.
* Spot-check `bitwisenot.mir` and `conv2d.mir` complete in ≤ 5s
  (regression for Category C).

### What I tried first (rolled back)

A naive version of this change — calling `DAG.initSUnit` for top-
fixed + free + bot-fixed in MIR order before `DAG.buildEdges` —
crashed `gemm-1.mir` at the assertion
`NextSU->BotReadyCycle == NextSU->getHeight()` in
`getNextUnscheduledFixedInstr` (`AIEMachineScheduler.cpp:632`).

Root cause: SWP-pushed bundles in the preheader contain CLONES of
loop-body MIs from multiple iterations sharing physical registers
(post-RA). With register-edge tracking enabled across the fixed
range, `buildEdges` adds **fixed-fixed register edges** with real
operand latencies (e.g. MAC chains carry 4-cycle dependencies).
These latencies dominate the artificial chain's `lat=1`-between-
adjacent-fixed-bundles, raising bot fixed SUs' DAG height beyond
the cycle their chain edge anchored. The SchedDAG ends up wanting
to delay SWP-prologue MIs PAST their modulo-schedule cycle,
breaking modulo correctness.

**The artificial chain is — and must stay — the source of truth
for fixed-fixed cycle distance** in SWP. Modulo arithmetic makes
"4-cycle MAC chains in a unrolled prologue" valid even when the
chain says "1 cycle apart"; the linear DAG can't represent this.

Free→fixed and fixed→free register edges, on the other hand, are
exactly what we want: they encode the data flow that a free SU
must respect when scheduled around the fixed bundles. The
`set.ll`-style branch correctness bug needs free→fixed edges to
exist; it does not need fixed-fixed edges.

### The change (revised)

After `DAG.buildEdges` runs over the full SUnit list, **strip the
register edges between fixed SUs** — keep only the artificial
chain edges that the `EmitFixedSUnits` mutator added (or will
add). Free↔fixed edges and free→free edges remain.

```diff
 void AIEPostRASchedStrategy::buildGraph(...) {
   ...
+  // Add SUnits for top-fixed + free + bot-fixed in MIR order so
+  // buildEdges sees the full instruction set and can compute
+  // proper register def→use edges in every direction.
   for (int S = 0; S < NCopies; S++) {
+    for (MachineInstr &MI : Region.top_fixed_instrs())
+      DAG.initSUnit(MI);
     for (MachineInstr *I : Region.getFreeInstructions())
       DAG.initSUnit(*I);
+    for (MachineInstr &MI : Region.bot_fixed_instrs())
+      DAG.initSUnit(MI);
   }
   DAG.ExitSU.setInstr(Region.getExitInstr());
   DAG.makeMaps();
   DAG.buildEdges(Context->AA);
+  // Strip fixed↔fixed register edges. The artificial chain
+  // installed later by `EmitFixedSUnits` is the source of truth
+  // for fixed-fixed cycle distance — register edges among them
+  // can carry real latencies that conflict with the chain (e.g.
+  // SWP-pushed MAC chains in modulo-cloned prologues).
+  removeFixedFixedRegisterEdges(DAG, Region);
   ...
 }
```

`removeFixedFixedRegisterEdges` identifies fixed SUs by their MIR
position (top_fixed_instrs() ∪ bot_fixed_instrs()), then for each
fixed SU walks its `Preds` and removes any non-artificial edge
whose source is also a fixed SU (also clearing the symmetric
`Succs` entry on the other end via `SU->removePred`).

`addFixedSUnit` becomes `markFixedSUnit` (lookup not create) as
before. `EmitFixedSUnits::createFixedSUDAGNodes` calls the renamed
function and adds the same artificial chain edges.

### Why SWP correctness is preserved

* **Fixed-fixed (within SWP prologue/epilogue):** stripped. Chain
  edges (lat=1 between adjacent bundles) drive cycle distance.
  Modulo schedule is honored as today.
* **Fixed-fixed register deps that today's "conservative cap" was
  hiding:** also stripped. Same conservative behavior as today.
* **Free↔fixed:** retained. Free SUs see the SWP prologue's defs
  and uses correctly. This is a TIGHTENING vs today (the
  conservative cap was the only mechanism); intentional, expected
  to remove some redundant NOPs in tests with SWP epilogue/prologue
  surroundings.
* **Free→free:** retained, identical to today.

### Why the delay-slot branch correctness is fixed

For `set.ll`-style cases:
* `jz r0` (synthesized fixed SU at end of trailing region) reads
  `r0`.
* `and r0, r0, r1` (free SU) defines `r0`.
* `buildEdges` over the full SU list produces a free→fixed
  register edge `and → jz` with the operand-defined latency.
* `removeFixedFixedRegisterEdges` does NOT remove this edge (it's
  not fixed→fixed).
* The SchedDAG honors the edge; `and` lands strictly before `jz`.
  The branch reads `idx & 1`, not the stale `idx`.

### Staged delivery

Two commits, in order. β.1.5 does not start until Commit 1 is
green and approved.

**Commit 1 — Phase 0 only: include fixed SUnits in the SchedDAG.**

* Apply the three file edits above and nothing else.
* Build: `ninja llc`.
* Run `ninja check-llvm-codegen-aie`. Expected outcomes:
  * **Best case (NFC):** all 2154 tests green. The conservative
    caps were already implying the same constraints as the new
    register edges. Commit and move on.
  * **Likely case (a small set of SWP-test diffs):** a handful of
    SWP-related tests fail with CHECK-NEXT mismatches because the
    schedule tightened (fewer NOPs). For each diff:
    1. Confirm the new schedule preserves the data-dep ordering of
       all relevant operations (no reads of stale registers, no
       writes after the consumer).
    2. Confirm the new schedule respects modulo / SWP invariants
       — no shift across the SWP boundary that breaks pipelining.
    3. If both hold: regen the affected test with
       `update_llc_test_checks.py` or `update_mir_test_checks.py`.
       Inspect the regenerated CHECK lines to make sure the regen
       output makes sense.
    4. If either check fails: that diff is a bug — investigate
       before regenerating.
* Run `ninja check-llvm-unittests-target-aie` — must be 39/39.
* Commit message: `[AIE][NFC-ish] Include fixed SUnits in SchedDAG`
  with a body explaining the Phase 0 motivation (sets up sound
  register-dep edges between free and fixed SUs, prerequisite for
  delay-slot fixed-branch synthesis).

**Commit 2 — β.1.5 on top.**

The previously-rolled-back synthesis (per-region gating +
barrier-trailed support + empty BUNDLE pseudo cycle anchors +
hazard-recogniser bypass for empty pseudos) becomes sound on top
of Commit 1. Re-apply, verify NFC for the trailing-region case
(branch position unchanged, delay-slot count unchanged, free-SU
deps respected), and audit any further test diffs.

---

## Context

The AIE backend currently has **two** schedulers that each model "things
outside the schedulable window that the scoreboard must respect" in their
own private way:

* **`AIEPostRASchedStrategy`** (regular post-RA scheduler in
  `llvm/lib/Target/AIE/AIEMachineScheduler.cpp`) operates on a
  `Region(TopFixedBundles, free, BotFixedBundles)` defined in
  `AIEInterBlockScheduling.h:138`. The fixed bundles are intra-MBB (SWP
  prologue pushed into a preheader, SWP epilogue pushed into an exit
  block). It primes a Top and Bot `AIEHazardRecognizer` scoreboard via
  `emitInScoreboard()` / `blockCycleInScoreboard()` (`:282`, `:373`,
  `:406`). Cross-MBB latency is handled coarsely via
  `InterBlockScheduling::getLatencyCap()` and `getBlockedResourceCap()`
  (`AIEInterBlockScheduling.h:389,393`) — scalar caps, not concrete
  scoreboard footprints.
* **`PostPipeliner`** (`AIEPostPipeliner.{h,cpp}`) ignores `Region`
  entirely. It uses its own modulo `ResourceScoreboard<FuncUnitWrapper>`
  (`:291`), seeds it by emitting first-iteration instructions into every
  future modulo copy (`:846-911`), and uses `BoundaryNode` skipping
  (`:247,266,803`) instead of `TopFixedBundles`/`BotFixedBundles`. The
  "fixed" thing the scoreboard respects is *future iterations of the
  same loop body*, not external code.

This plan unifies the two paths into one engine, makes that engine the
**only** scoreboard-priming and `fitInInterval` path in the AIE backend,
and (Phase 2) replaces the conservative scalar caps with concrete
predecessor/successor scoreboard projections built by per-cycle union
across neighbouring MBBs.

Delivered in two phases per the user's direction: **Phase 1 = refactor
+ unification + delay-slot migration + outer-SWP push primitive**;
**Phase 2 = cross-MBB projection scoreboards**.

## How other LLVM backends handle this (and why AIE is unusual)

Cross-MBB long-latency awareness during instruction scheduling is rare
in LLVM in-tree backends. Three patterns exist:

| Pattern | Backend | Mechanism | Cross-block carryover |
|---|---|---|---|
| Block-local reset | Hexagon (`HexagonHazardRecognizer::Reset`, `HexagonVLIWPacketizer::endPacket`), PowerPC (`PPCDispatchGroupSBHazardRecognizer::Reset`), generic `PostRASchedulerList:402-411`, `ScoreboardHazardRecognizer::Reset` | Scoreboard cleared at every MBB boundary | None |
| Insert waits, don't reorder around them | AMDGPU `SIInsertWaitcnts.cpp` `WaitcntBrackets::merge()` (~2104-2140), per-MBB `BlockInfo.Incoming` (610), worklist fixpoint (2517-2563); GCN `hasHazard` / `getWaitStatesSince` (487-534) backward CFG walk | Latency closed by inserting `s_waitcnt`, separate dataflow pass | Implicit; scheduler stays block-local |
| Encode tails into the DAG via `ExitSU` | Generic `ScheduleDAGInstrs::addSchedBarrierDeps` (220-262), artificial `ExitSU` edges with `Latency-1` (950-966) | Single synthetic `ExitSU` per region | Intra-region only |

AIE is genuinely different: no hardware stall, no `s_waitcnt` analog,
deep VLIW pipelines that require *scheduling around* long latencies.
Hence the unique `InterBlockScheduling` infrastructure (loop-aware
fixpoint with `LatencyMargin` / `ResourceMargin`, `InterBlockEdges`
cross-boundary DDG). The closest in *spirit* is AMDGPU's
`WaitcntBrackets::merge()` lattice merge of per-MBB-entry latency
state — but the consumer is different: AIE wants the merged state to
gate *reordering* via a scoreboard, not to insert waits.

This proposal is therefore a refinement of an already-AIE-unique
infrastructure, not an import from another backend.

---

## Phase 1 — Unification + delay-slot migration + outer-SWP push primitive

### Phase 1 — New

#### `FixedRegionScoreboardScheduler` engine

```cpp
// llvm/lib/Target/AIE/AIEFixedRegionScoreboardScheduler.h
class FixedRegionScoreboardScheduler {
public:
  struct Config {
    int II;                       // 0 = no modulo; >0 = modulo II
    int ScoreboardSize;
    bool BottomUp;

    // Intra-MBB fixed bundles (SWP epilogue / prologue inserts; the
    // branch + delay slots in BotFixedBundles).
    ArrayRef<MachineBundle> TopFixedBundles;
    ArrayRef<MachineBundle> BotFixedBundles;

    // External-MBB footprint projections (Phase 1 leaves both null;
    // Phase 2 populates). Caller-owned; engine holds const pointers.
    // Type is ResourceScoreboard, NOT ArrayRef<MachineBundle>:
    //   - per-cycle union across multiple neighbours is bitwise-OR of
    //     FuncUnitWrapper, the natural scoreboard operation;
    //   - the underlying instructions live in other MBBs and are
    //     emitted there, so no bundle-level identity is needed here;
    //   - covers memory bank / object conflicts via FuncUnitWrapper
    //     for free.
    const ResourceScoreboard<FuncUnitWrapper> *PredScoreboard = nullptr;
    const ResourceScoreboard<FuncUnitWrapper> *SuccScoreboard = nullptr;
    ScoreboardTrust PredTrust = ScoreboardTrust::Conservative;
    ScoreboardTrust SuccTrust = ScoreboardTrust::Conservative;
  };

  FixedRegionScoreboardScheduler(const AIEHazardRecognizer &HR,
                                 const Config &Cfg);

  std::optional<int> fitInInterval(const SUnit &SU, int Earliest, int Latest);
  void primeAllRegions();        // single-position emission for fixed layers
  void emit(const SUnit &SU, int Cycle);  // modulo-broadcasts when II > 0
  void dump() const;
  void dumpLayer(StringRef LayerName) const;

private:
  const AIEHazardRecognizer &HR;
  Config Cfg;
  ResourceScoreboard<FuncUnitWrapper> Scoreboard;   // free + Top/Bot fixed
  bool checkConflict(const MachineInstr &MI, int Cycle) const;
};
```

The engine absorbs the implementations of:

* `PostPipelinerStrategy::fitInInterval` (`AIEPostPipeliner.cpp:60-79`).
* First-iteration emission loop (`AIEPostPipeliner.cpp:846-911`).
* Validation replay loop (`AIEPostPipeliner.cpp:1052-1072`).
* Top/Bot scoreboard priming sites in
  `AIEMachineScheduler.cpp:282,285,373,406`.

After Phase 1, exactly one place in the AIE backend emits into a
scoreboard: this engine. `BoundaryNode`-skipping logic
(`AIEPostPipeliner.cpp:247,266,803`) stays in `PostPipeliner` because
it's DAG-level Earliest/Latest propagation, not scoreboard mechanics.

#### Top/Bot fixed conflict-resolution shift loop (inside the engine)

When TopFixed and BotFixed share one central scoreboard, their merged
occupancy can conflict — only relevant when `Cfg.II > 0` (post-pipeliner
with both fixed regions populated, e.g. the outer-SWP verification
test). Direct cycle collisions are precluded by `Region` construction
(`L >= K_top + K_bot`), so the only runtime case is a modulo
collision: `t % II == b % II` for some Top cycle `t` and Bot cycle `b`
whose merged resource demand can't be satisfied.

```text
shift = 0
loop:
  prime central scoreboard with TopFixed at [0, K_top)
                          and BotFixed at [L-K_bot, L) shifted by `shift`
  if any cycle's (TopLayer[c] | BotLayer[c]) hasInternalConflict():
    shift += 1
    L     += 1                 // grow MBB by one nop cycle
    if shift > shift_max: report failure
    continue
  break
```

Asymmetric on purpose: only BotFixed shifts down (cycle index grows);
cycle 0 is the absolute MBB start. Pushing Bot down is equivalent to
pushing Top up in conflict-elimination terms, but bot-down keeps the
MBB entry meaningful. The free region grows alongside `L`, giving free
instructions *more* room — the user's "minimize free-region
interference" goal.

#### `unionInto` per-cycle scoreboard merge helper

```cpp
// In AIEHazardRecognizer.h, alongside FuncUnitWrapper::operator|=.
void unionInto(ResourceScoreboard<FuncUnitWrapper> &Dst,
               const ResourceScoreboard<FuncUnitWrapper> &Src);
```

Implementation: `for c: Dst[c] |= Src[c]`, using existing
`operator|=` (`AIEHazardRecognizer.h:124`). Phase 1 nails down the
type so Phase 2's `buildPredProjection` / `buildSuccProjection` can
plug in without further engine work.

#### `FuncUnitWrapper::hasInternalConflict` and `conflictBetween`

```cpp
// Promoted from the comparison logic inside FuncUnitWrapper::conflict
// (AIEHazardRecognizer.cpp:128-143) so the engine's shift loop can
// reuse the exact same predicates.
bool FuncUnitWrapper::hasInternalConflict() const;
static bool conflictBetween(const FuncUnitWrapper &A,
                            const FuncUnitWrapper &B);
```

Both reuse, not duplicate, today's check (slots, banks, memory
objects, required FUs, format compatibility).

#### `pushTopFixedToPredecessor` outer-SWP push primitive

```cpp
// In AIEInterBlockScheduling.{h,cpp}.
//
// Move the bottom-most cycle of TopInsert in `From` to the bottom of
// TopInsert in `To` (a predecessor of `From`). Returns false if the
// resulting layout cannot be made conflict-free within MaxShift cycles
// of Top/Bot conflict-resolution shift.
bool pushTopFixedToPredecessor(BlockState &From, BlockState &To,
                               int MaxShift = 4);
```

Drives the engine's conflict-resolution shift loop on `To` after each
push. The user's "if the push collides with any existing top fixed
[...] mechanism" requirement is the same shift loop reused — no
separate push-only conflict resolver.

#### Phase 1 unit tests

* `unittests/Target/AIE/FixedRegionScoreboardSchedulerTest.cpp`:
  * `OuterSwpRegularPath` — non-modulo mode with non-empty
    TopFixedBundles + non-empty BotFixedBundles; assert
    `fitInInterval` never returns a cycle inside the fixed bands.
  * `OuterSwpTopBotConflictShifts` — modulo mode with constructed
    Top/Bot conflict; assert the shift loop fires and reports the
    expected new `L`.
  * `PostPipelineriIdentity` — drives the engine in modulo mode with
    empty fixed bundles; bit-identical to today's first-iteration
    emission.
* `unittests/Target/AIE/FuncUnitWrapperTest.cpp` —
  `unionInto`, `conflictBetween`, `hasInternalConflict` happy path
  + edge cases (empty, format-incompatible, slot-overflow).
* `unittests/Target/AIE/InterBlockSchedulingPushTest.cpp` —
  `PushTopFixedHelper` (3 successive pushes, then refusal),
  `PushTopFixedConflict` (conflict that cannot be absorbed; assert
  unchanged inputs on failure).

#### Phase 1 codegen tests

* `llvm/test/CodeGen/AIE/aie2ps/schedule/postpipeliner/refactor-engine-identity.mir` —
  small SWP loop matching `maxpool-9instr.mir` body; CHECK lines
  identical to the existing maxpool test. Sentinel against future
  refactors.
* `llvm/test/CodeGen/AIE/aie2*/schedule/delay-slot-via-fixed-bottom.mir` —
  representative MBB ending in a `hasDelaySlot()` branch with 5
  delay-slot fills. CHECK that the branch lands at the right cycle,
  useful free instructions migrate into the 5 trailing cycles, and
  `-aie-reserved-delay-slots=2` reserves 2 trailing empty cycles.

### Phase 1 — Changed

#### `AIEPostPipeliner.{h,cpp}`

The three duplicated scoreboard-driven bodies collapse into engine
calls:

* `fitInInterval` (`:60-79`) → `engine.fitInInterval(SU, E, L)`.
* First-iteration scoreboard emission (`:846-911`) →
  `engine.emit(SU, Cycle)` (modulo-broadcasts when `Cfg.II > 0`).
* Validation replay (`:1052-1072`) → loop calling `engine.emit` over
  the recorded schedule.

Engine becomes a member of `PostPipeliner`; one engine per `schedule()`
invocation, configured with `Cfg.II = II` and empty fixed-region
arrays for innermost loops. Heuristic / strategy / boundary-node
logic is unchanged.

#### `AIEMachineScheduler.cpp`

* Top/Bot scoreboard priming at `:282,285,373,406` becomes one
  engine `primeAllRegions()` call per direction (Top-direction and
  Bot-direction engines, mirroring today's two-scoreboard split).
* `enterRegion` synthesises a backing array for the in-MBB delay-slot
  bundles whenever the region's trailing terminator has
  `hasDelaySlot()`. Concretely:
  1. Insert N+R empty `BUNDLE` pseudo MIs after the branch in MBB
     (using `BuildMI(*BB, ..., TII->get(TargetOpcode::BUNDLE))` —
     same mechanism `emitBundles` uses today for empty bundles in
     SWP).
  2. Populate `BS.LocalBottomFixed = [branch_bundle, empty_bundle ×
     (N+R)]`, with each entry's MachineBundle wrapping its
     corresponding MI in MBB (the branch MI for the first entry, an
     empty BUNDLE pseudo for each trailing entry).
  3. The `Region` constructor uses `BS.LocalBottomFixed` (when
     non-empty) as the backing array for `BotFixedBundles`.
  N = `TII->getNumDelaySlots(branch)`, R = `-aie-reserved-delay-slots`.
* `getTopFixedBundles()` / `getBotFixedBundles()` accessors
  unchanged.
* Every `fitInInterval`-equivalent in this file routes through the
  engine.

#### `AIEInterBlockScheduling.{h,cpp}` — clarified semantics: cycle
= iterator position, fixed empty cycles preserve cycle distance

The Region API for `TopFixedBundles` / `BotFixedBundles` is **not
changed**. What's clarified is the conceptual model that already
matches today's data structure:

**Each entry is one cycle = one MIR iterator position in the MBB.**
That iterator position holds 0 or more MachineInstructions, wrapped
in zero or one MachineBundle:

* **Pinned cycle** (one or more MIs): the iterator position is a
  real instruction or a `BUNDLE` pseudo wrapping multiple MIs.
  Multi-SU cycles work natively via `MachineBundle::Instrs`.
  Scheduler may not reorder, remove, or co-locate other SUs.
* **Fixed empty cycle** (zero MIs): the iterator position is an
  empty `BUNDLE` pseudo MI (one MI with zero inner instructions).
  Today's `emitBundles` already creates these for SWP empties at
  `:815`. The scheduler does not pick or do anything for these
  cycles — they're cycle-distance placeholders.

The post-pipeliner produces complex Top/Bot-fixed sequences with
empties between non-empty bundles to maintain cycle distances; this
model handles that correctly because every cycle has a backing MIR
position.

Region's iterator math is unchanged because the invariant holds:
each `BotFixedBundles[i]` corresponds to exactly one MIR iterator
position (BUNDLE pseudo or pinned MI). `prev(End, size())` is
correct.

```cpp
// AIEInterBlockScheduling.cpp Region constructor — UNCHANGED:
MachineBasicBlock::iterator FreeBegin =
    std::next(Begin, TopFixedBundles.size());
MachineBasicBlock::iterator FreeEnd =
    std::prev(End, BotFixedBundles.size());
```

Engine's `primeAllRegions()` already handles empties correctly: it
iterates each fixed bundle's `Instrs` and calls `HR.emitInScoreboard`
for each — empties have no Instrs so no resource demand is emitted.
No change needed.

The `bundleIsPinned` / `countPinned` helpers added in step α end up
not being needed by the iterator math; they remain as available
documentation predicates for callers that want to count pinned
cycles for diagnostics. Net cost: 8 lines of unused code that
self-documents the data shape.

#### Why this works for both callers

* **Post-pipeliner SWP use:** **fully unchanged**. `PipelineExtractor`
  pushes bundles (with possible empties) into `BS.BottomInsert` /
  `BS.TopInsert`. `emitInterBlockBottom` / `emitInterBlockTop` calls
  `emitBundles` which creates empty BUNDLE pseudos for empties.
  Cycle distances are preserved by the BUNDLE pseudo iterator
  positions.

* **Delay-slot regular-scheduler use:** new path mirrors the SWP
  pattern but in-MBB. `BS.LocalBottomFixed = [branch_bundle,
  empty_bundle × N]`. The branch is the pinned entry; N empty
  BUNDLE pseudos backing the empties are inserted after the branch
  in MBB at `enterRegion` time. Branch is no longer a free SU —
  it's pinned via `BotFixedBundles[0]` — so the top-zone reject,
  the `RegionBottomUpCycles` floor, and the delay-slot tie-break
  all disappear. Delay slots are **fixed empty cycles** (always
  emit as NOP at asm time via the existing empty-BUNDLE-pseudo
  rendering path at `AIEBaseAsmPrinter.cpp:172-177`).

* **Multi-SU bundles in fixed regions:** any pinned entry can wrap
  an arbitrary number of MIs (one MIR `BUNDLE` pseudo wrapping
  many). Region's iterator math counts the BUNDLE pseudo as one
  position regardless of inner count.

#### What we rely on (and what we don't need)

`FixedRegionScoreboardScheduler::fitInInterval` is sufficient for
**resource-aware placement** — the engine's scoreboard already
holds pinned-MI demand from `primeAllRegions`, so when the
scheduler asks "can free SU X land at cycle C?" the answer
correctly accounts for any pinned MI's slot/bank/FU usage at C.
Free SUs that are slot-disjoint with the branch will be told they
fit at the branch's cycle; free SUs that match an empty bundle's
zero-demand cycle will be told they fit there. Standard scheduler
flow.

We **do not** need to insert pinned MIs into the SchedDAG. The
DAG's depth/height calculations are about free-SU dependences;
pinned MIs already have their cycle anchored, so they don't
participate in DAG-driven priorities. The scoreboard captures
the constraint they impose on free-SU placement, which is what
matters.

The one remaining concern is **MIR placement** at commit time.
When the scheduler decides "free SU lands at cycle L-N+k" (a
delay-slot cycle), the MachineInstr has to end up in the right
position in the MBB so that `applyBundles` /
`finalizeBundle` bundles it correctly with whatever's already at
that cycle. Concretely:

* Empty cycle (was an empty `BUNDLE` pseudo): the free SU's MI
  needs to take that MIR position. The empty pseudo is dropped or
  the free SU is added to its bundle.
* Pinned cycle (branch): the free SU's MI needs to be in the
  same `BUNDLE` as the branch.

This is local AIE-side commit logic, not a framework change. It
lives in / near `materializeEmptyBundles` (`AIEMachineScheduler.cpp:773`)
and `AIEHazardRecognizer::applyBundles` (`AIEHazardRecognizer.cpp:336`).

#### Architectural reality discovered during β.1 v2-v5 investigation

The AIE backend's `PreSchedInstExpansion<X, DelayedSchedBarrier>`
tablegen pattern (`AIE2InstrInfo.td:446-482`, similar in
`aie2ps`/`aie2p`) installs a `DelayedSchedBarrier` after **every**
delay-slot-bearing instruction (RET, JL, JL_IND, JZ, JNZ, JNZD,
J_jump_imm, J_jump_ind) before scheduling runs. Consequence:

* By the time `postmisched` runs, every delay-slot MI in MIR is
  followed by its own `DelayedSchedBarrier`. There is **no shape**
  where the delay-slot MI is the very last MI of an MBB without a
  trailing barrier (except in `-run-pass=postmisched` MIR tests
  that hand-author this configuration).
* The general LLVM `MachineScheduler` splits the MBB into one
  region **per** sched-barrier. Each region's `RegionEnd` points to
  *its* `DelayedSchedBarrier` (excluded from the region; the
  delay-slot MI is the last MI of the region). The existing
  `getDelaySlotInstr` assertion at `AIEMachineScheduler.cpp:454`
  encodes this: "if a delay-slot MI is in the region, then
  `RegionEnd` is at a `DelayedSchedBarrier`".
* A function like `call_i32.ll` (one mid-MBB `JL` plus a trailing
  `RET`) therefore has **two** scheduling regions in its sole MBB:
  Region 1 ends at `JL`'s barrier, Region 2 ends at `RET`'s barrier.

This shape is what β.1 v5 hit. The `BS.LocalBottomFixed` synthesis
populated for the *trailing* delay-slot MI (`RET`) was being applied
to *every* region of the MBB at `enterRegion` time — including
Region 1, whose trailing MI is `JL`, not `RET`. The Region
constructor's `FreeEnd`-vs-`BotFixedBundles.front()` invariant
(`AIEInterBlockScheduling.cpp:1178`) correctly fired because
`branch_bundle` wraps `RET` but `FreeEnd` of Region 1 lands on `JL`'s
position.

**Issue 6 root cause:** `BS.LocalBottomFixed` is a per-MBB datum but
the existing `BotFixedBundles` plumbing applies it to every region
of the MBB. For multi-region MBBs (mid-MBB CALL + trailing RET) it
contaminates the wrong region.

**Issue 6 fix:** track the synthesized branch MI as
`BS.LocalBottomFixedBranch` (a `MachineInstr*`) at `enterBlock`
time. At `enterRegion`, only assign `BotFixedBundles =
LocalBottomFixed` when the region's last instruction (= MI just
before `RegionEnd`, which by the architectural reality above is
the delay-slot MI itself) equals `LocalBottomFixedBranch`. Other
regions (mid-MBB delay-slot MIs) get `BotFixedBundles = empty` and
the existing `RegionBottomUpCycles` floor + tie-break + top-zone
reject continues to handle them — until those mechanisms are also
extended (later, via per-region synthesis). For now, only the
**trailing** region per MBB participates in the new mechanism.

#### Concrete delivery for step 19

1. **`enterBlock` synthesis (in `AIEInterBlockScheduling.cpp`).**
   Once per MBB, scan for the trailing delay-slot terminator
   (`std::prev(BB->end())`, skipping a possible single trailing
   `DelayedSchedBarrier`). If found:
   * Insert N empty `BUNDLE` pseudo MIs *between* the branch and
     the trailing `DelayedSchedBarrier` (so the empties live
     inside the trailing region's MIR range, not after it). Use
     `BuildMI(*BB, std::next(BranchIt), ..., TargetOpcode::BUNDLE)`.
   * Populate `BS.LocalBottomFixed = [branch_bundle, empty_bundle × N]`
     and `BS.LocalBottomFixedBranch = &*BranchIt`.
   * Move synthesis to `enterBlock` (not `enterRegion`) so the
     MBB modification happens before `RegionInstrs` is computed —
     fixes Issue 2.

2. **`Region` constructor invariants.** Two updates:
   * `bot_fixed_instrs()` (`AIEInterBlockScheduling.h:196`) anchors
     at `ExitInstr->getIterator()` when `ExitInstr` is non-null
     (i.e. region ends at a barrier or another MI), otherwise
     `BB->end()`. Fixes Issue 3.
   * `End == BB->end()` assertion at `:1175` relaxed to also
     accept `End` at a `DelayedSchedBarrier`. Fixes Issue 1.

3. **Per-region gating in `enterRegion`.** Set `BotFixedBundles =
   BS.LocalBottomFixed` **only** when `BS.LocalBottomFixedBranch !=
   nullptr` and the region's last MI (= `prev(RegionEnd)` after
   skipping a possible barrier) equals `BS.LocalBottomFixedBranch`.
   Fixes Issue 6. Mid-MBB delay-slot regions fall through to today's
   behaviour (no `BotFixedBundles`, old machinery handles them).

4. **Engine priming sees the new BotFixedBundles** for the trailing
   region. `primeAllRegions()` iterates each pinned bundle's `Instrs`
   and emits resource demand:
   * **At cycle L-N-1 (branch's cycle):** branch's slot, FUs,
     format, memory bank/object added. Other VLIW slots remain
     unclaimed — slot-disjoint free SUs co-locate via multi-issue.
   * **At cycles L-N..L-1 (delay-slot empties):** zero demand.
     Free SUs land subject only to inter-cycle latency/dependence
     constraints.
   `fitInInterval` answers correctly without the SchedDAG knowing
   about the branch.

5. **`RegionBottomUpCycles` reads from `Region`** for the trailing
   region only. `getDelaySlotInstr` scan goes away for that case.
   The bot zone size is sourced from
   `Region::getBotFixedBundles().size()`. Mid-MBB regions retain
   today's `getDelaySlotInstr` + floor logic (or are migrated in
   a follow-up).

6. **Top-zone reject and delay-slot tie-break removed for the
   trailing case.** Both rules become unreachable when the branch
   is in `BotFixedBundles[0]` (Issue 4-relevant: must verify they
   still fire correctly for mid-MBB delay-slot MIs that *aren't*
   in `BotFixedBundles`). One safe approach: keep the rules
   conditional on `!isFixedSU(SU, /*IsTop=*/false)` so they
   continue to apply to mid-MBB free-SU branches.

7. **Commit-time MIR bundling.** `materializeEmptyBundles` /
   `applyBundles` updated so that when a bundle in `BS.Bundles`
   lands at a cycle whose MIR position holds an empty BUNDLE
   pseudo, the empty pseudo is removed and the free SU's MI takes
   its place. When a bundle lands at the branch's cycle, the
   free SU joins the branch's bundle. This is the only AIE-side
   mutation new to step 19; carries the highest correctness risk
   and gets dedicated hand-audit during regression.

#### Behavioural delta vs today

* Delay slots that today's scheduler successfully fills with free
  SUs continue to be filled — `fitInInterval` permits the same
  placements. Tests that filled delay slots should remain
  bit-identical.
* Delay slots that today are NOPs stay NOPs.
* The **branch's MIR position is unchanged** — it stays at MBB
  end - N - 1 (where the terminator already lives).
* The N delay-slot positions after the branch are pre-emitted as
  empty BUNDLE pseudos (replacing today's post-scheduling NOP
  insertion via `materializeEmptyBundles`). After scheduling +
  commit, those positions hold real MIs (filled) or empty BUNDLE
  pseudos (asm printer renders as full-NOP bundle, identical to
  today's NOP rendering).

If the commit-time bundling behaves correctly, this is an
NFC change.

#### Staged commit ladder for step 19

Each step is verified in isolation (full `check-llvm-codegen-aie`
green, AIETests 39/39) before the next is layered on. Already
landed: **β.1 v1** (commit `a937b3f3da7d`) — synthesis fires only
for the `RegionEnd == BB->end()` case, which the architectural
reality says is rare/never in production (every delay-slot MI gets
a `DelayedSchedBarrier` inserted before scheduling). It is dormant
in real tests but is committed and NFC.

* **β.1.5 (per-region gating + barrier-trailed support):**
  Move synthesis from `enterRegion` to `enterBlock` (fixes Issue 2);
  extend it to fire for the trailing delay-slot MI followed by
  `DelayedSchedBarrier` (the production shape); update Region
  invariants and `bot_fixed_instrs()` anchor (Issues 1, 3); track
  `BS.LocalBottomFixedBranch` and gate the per-region `BotFixedBundles
  = LocalBottomFixed` assignment on it (Issue 6). Mid-MBB delay-slot
  MIs (Issue 4, 5) remain on the legacy path. Expected NFC at this
  step: synthesis is now reachable for all trailing-RET / trailing-J
  MBBs, but the legacy `RegionBottomUpCycles` floor + tie-break +
  top-zone reject are still in place, so the schedule output is
  unchanged.

  **Status (rolled back twice):** β.1.5 was attempted in two
  variants. Both had to be rolled back. The first surfaced two
  SchedDAG-level issues:

  1. **`getHazardType` on empty BUNDLE pseudos.** The empty BUNDLE
     pseudo MIs synthesised between the branch and the trailing
     `DelayedSchedBarrier` need to be valid SUnits at *some* level —
     either as fixed SUnits flowing through the `isAvailableNode`
     fixed-SU bypass, or with hazard-recognizer changes that
     recognise empty BUNDLE pseudos as no-hazard meta instructions.
     If `createFixedSUDAGNodes` skips them (no SUnit), the bot zone
     `isAvailableNode` path crashes the `getNextUnscheduledFixedInstr`
     `assert(NextSU)` because `DAG->bottom()` lands on an empty
     pseudo with no SUnit. If they get fixed SUnits via the existing
     chain, free SUs released to the *top* zone (e.g. the synthesised
     branch as a top-root) bypass-skip and call `checkHazard` on
     empties anyway. Either path needs a hazard-recogniser bypass
     for empty BUNDLE pseudos in addition to the synthesis.
  2. **Top-root branch released to top zone.** Even when the
     SchedDAG correctly creates fixed SUnits for the empties, the
     synthesised branch (with no register predecessors after fixing)
     becomes a top-root and goes through `releaseTopNode`. The bot-
     zone fixed-SU bypass (which only consults
     `getNextUnscheduledFixedInstr` for the queue's own zone)
     doesn't skip the hazard check for it.

  The second attempt added a hazard-recogniser bypass for empty
  BUNDLE pseudos and re-tried the per-region gating + barrier-
  trailed support. Crashes went away (320 → 0) but **93 tests
  failed with schedule diffs**, and at least one tested case
  (`aie2/set.ll`) revealed a third, more fundamental issue:

  * **Missing register-dep edges between free SUs and fixed
    branch SUs.** `AIEPostRASchedStrategy::buildGraph` calls
    `DAG.buildEdges` over the FREE SUnit list only. The fixed
    branch SU is added later via the `EmitFixedSUnits` mutation,
    after `buildEdges` has already run, so it has only the
    artificial chain edge to ExitSU and **no data-dep edges to
    free SUs**. In `set.ll`, the test computes
    `if (idx & 1) goto exit`, lowered to:

    ```
    and r0, r0, r1   ; r0 := idx & 1
    jz r0, exit      ; reads r0
    ```

    With the synthesis active, `jz` becomes a fixed SU at the
    branch's anchor cycle (= cycle 0 of the bot zone). The
    `and → jz` data dep should keep `and` at a strictly earlier
    cycle. Without the register-dep edge in the DAG, the
    scheduler is free to place `and` *after* `jz` in MIR order
    (delay slot 1), so `jz` reads the OLD `r0` (= `idx`) and the
    branch decision uses the wrong value. **This is a
    correctness regression**, not a tightening — the asm output
    is shorter but executes a different program.

  **Decision:** revert β.1.5 in full. β.1 v1 stays committed
  and dormant. Working tree at 2146/2146 = pre-β.1.5 baseline.

  **Path forward (revised again):** the synthesis approach as
  designed is unsound for delay-slot branches that read
  registers (i.e. all conditional branches and any branch with
  register-defined targets). A sound design must give the
  branch SU's register reads visibility to the scheduler. Two
  options:

  1. **Re-run buildEdges after creating fixed SUs.** Or
     equivalently, build a "phantom" pre-edges pass that
     introduces register-def→fixed-branch edges. Practical
     issue: `buildEdges` is heavy and not designed to be run
     incrementally; rebuilding it post-mutation would touch the
     fixed SUs' SDeps + propagated heights/depths, which the
     scheduler caches.
  2. **Don't fix the branch — use sched-barrier semantics.**
     The branch stays a free SU; the scheduler places it via
     the existing `getDelaySlotInstr` floor + tie-break + top-
     zone reject. The synthesis only inserts the trailing
     empty BUNDLE pseudos as fillable cycle anchors, treats
     them as no-hazard cycle markers, and adjusts the
     commit-time bundling so free SUs that bottom-up-fit at
     the empty cycles physically end up at those MIR
     positions. This preserves the legacy machinery for
     branch placement and only changes the empty-cycle
     fill-in. Smaller scope, safer, but doesn't unify the
     scoreboard model — falls short of the original goal of
     "branch as a fixed SU."

  Option (2) is the safer path forward and aligns with what
  the user described as "scheduler fills delay slots." It
  delivers the user-visible benefit (free SUs filling delay
  slots more aggressively where dependencies allow) without
  the SchedDAG plumbing of (1). Option (1) remains valuable
  if the long-term plan needs the branch as a fixed SU
  (e.g. to feed the engine's `fitInInterval` for branch
  placement decisions across the whole region).

#### Combined β.1.5 + β.1.6 + β.2 (revised path)

Concretely, the smallest change that gets us all the way is:

1. **Synthesis at enterBlock with per-region gating** (β.1.5
   work, repeatable). Inserts N empty BUNDLE pseudos between
   branch and trailing DSB; populates `BS.LocalBottomFixed` and
   `BS.LocalBottomFixedBranch`; gates `BotFixedBundles =
   LocalBottomFixed` to the region containing the synthesised
   branch (`std::find_if` over `[RegionBegin, RegionEnd)`).
2. **Hazard-recogniser bypass for empty BUNDLE pseudos.** Extend
   `AIE::MachineBundle::isNoHazardMetaInstruction` (or the
   `getHazardType` early-return) to recognise
   `TargetOpcode::BUNDLE` MIs with no successor-bundled inner MIs
   as no-hazard meta. This is the structural change that makes
   empty cycle anchors safe to release to either zone.
3. **Region/bot_fixed_instrs() anchor** (β.1.5 work). Anchor
   `bot_fixed_instrs()` at `ExitInstr->getIterator()` when set,
   else `BB->end()`. Relax the Region constructor's
   `End == BB->end()` assertion (fixes Issue 1, 3).
4. **Remove legacy machinery for the trailing case** (β.2 work).
   Conditionally remove `RegionBottomUpCycles` floor / top-zone
   reject / tie-break **only** when the region's branch is in
   `BotFixedBundles`. Mid-MBB delay-slot MIs continue through the
   legacy path (β.3 follow-up).
5. **Commit-time bundling** (β.1.6 work). When a free SU lands
   at a delay-slot cycle whose MIR position holds an empty BUNDLE
   pseudo, the empty pseudo is replaced by the free SU. When it
   lands at the branch's cycle, it joins the branch's bundle.
   Lives in `materializeEmptyBundles` / `applyBundles`.

Verification: end-to-end `check-llvm-codegen-aie` green; hand-
audit any test that diffs (expect zero diffs if all five pieces
behave correctly; intentional diffs are the schedules that
today's tie-break placed at different cycles than
`fitInInterval` chooses).

* **β.1.6 (commit-time bundling):** Update `materializeEmptyBundles` /
  `applyBundles` for the cycle-merge cases (free SU at branch's
  cycle joins branch's bundle; free SU at empty cycle replaces the
  empty BUNDLE pseudo). Still NFC if the legacy path is also still
  in place — the synthesis is structurally identical to the legacy
  output. Hand-audit any test that diffs.

* **β.2 (remove legacy machinery for trailing case):** Remove
  `RegionBottomUpCycles` floor / tie-break / top-zone reject **only
  when the trailing region's branch is in `BotFixedBundles`**. Mid-MBB
  CALL still needs the legacy path (until a follow-up extends
  per-region synthesis to those too). Conditional removal: e.g.
  the floor becomes
  `RegionBottomUpCycles = (LocalBottomFixed-applies-to-this-region)
  ? Region.getBotFixedBundles().size()
  : (legacy DelaySlotCycles)`. Same-shape conditionals for the
  zone-reject and tie-break.

  Expected diff after β.2: tests where today's scheduler successfully
  filled a delay slot with a free SU may regenerate (now driven by
  `fitInInterval` instead of tie-break). Hand-audit each
  regenerated test. The branch's MIR position and delay-slot count
  must stay identical; only filled-vs-NOP content differs.

* **β.3 (mid-MBB delay-slot migration, optional follow-up):**
  Per-region synthesis for mid-MBB CALLs. Enables full removal of
  the legacy delay-slot machinery. May be deferred to a separate PR.

#### Risk

The commit-time bundling change (β.1.6) is the place a regression
could hide. Mitigation: implement it, run the full
`check-llvm-codegen-aie` suite, hand-audit any test that diffs.
The expected diff set is empty (free SUs that land at delay-slot
cycles already do today), so any diff is a bug to investigate.

#### `llvm/lib/Target/AIE/CMakeLists.txt`

* Add `AIEFixedRegionScoreboardScheduler.cpp`.

### Phase 1 — Removed

* **Removed:** `getDelaySlotInstr` static helper at
  `AIEMachineScheduler.cpp:418-425`. Replaced by reading the
  pinned/empty count from `Region::getBotFixedBundles().size()`.
* **Removed:** the `RegionBottomUpCycles =
  TII->getNumDelaySlots(MI) + 1` floor at
  `AIEMachineScheduler.cpp:445-459`. The bot zone size is sourced
  from `Region::getBotFixedBundles().size()`.
* **Removed:** the "delay-slot SU must not enter the Top zone"
  rule at `AIEMachineScheduler.cpp:502-503`. Obsolete — branch is
  no longer a free SU.
* **Removed:** the delay-slot tie-break at
  `AIEMachineScheduler.cpp:1007-1009`. Same rationale.
* **Removed:** no legacy fall-back flag — hard cutover.

### Phase 1 — Verification

* `ninja check-llvm-codegen-aie` — expected to be bit-identical to
  pre-step-19 baseline. Specifically:
  * The branch's MIR position is unchanged.
  * Delay-slot cycle count (5) is unchanged.
  * Delay slots that today the scheduler filled with free SUs
    continue to be filled (`fitInInterval` returns the same cycles
    given the same scoreboard state).
  * Delay slots that today are NOPs continue to be NOPs.
* Hand-audit any test that diffs. Expected diff set is empty; any
  diff is a regression in step 19's commit-time bundling step (5)
  to investigate.
* AIETests stays at 39/39 — no engine-level test changes.

The bundle-aware commit-time merging (step 5 above) is the new
behaviour. It's contained in `materializeEmptyBundles` /
`applyBundles` updates; verification is the test suite's bit-
identical pass.
* Spot-check: rerun all tests under
  `llvm/test/CodeGen/AIE/aie2ps/schedule/postpipeliner/` (incl.
  in-flight `conv2d_fp16.mir`) and
  `llvm/test/CodeGen/AIE/aie2*/schedule/`.
* Build with `-DLLVM_ENABLE_ASSERTIONS=ON`; rerun with
  `-debug-only=postpipeliner-blockers` on a representative loop —
  identical blocker-verdict lines.
* Run unit tests under `unittests/Target/AIE/`.

---

## Phase 2 — Cross-MBB projection scoreboards (separate PR)

Replaces the conservative scalar caps `getLatencyCap()` /
`getBlockedResourceCap()` with concrete projection scoreboards built
from the union of neighbouring MBBs' actual schedules. The engine
config from Phase 1 (`Cfg.PredScoreboard`, `Cfg.SuccScoreboard`,
`PredTrust`, `SuccTrust`) is the consumer; Phase 2 populates the
producer.

**Phase 2 also subsumes the Conservative-band priming inside
`initializeBotScoreBoard` / `initializeTopScoreBoard`** (today
hardcoded full-block when `successorsAreScheduled` is false). The
projection-based band is the proper architectural fix for the
delay-slot QoR regressions documented in the "Current
investigation: bxor_v32bf16" section above. The Commit-1
mitigation (gating `upgradeFreeSUExitEdgesViaScoreboard` on
non-delay-slot regions) is a band-aid that disables the probe in
the cases where the Conservative band's blocked cycles cascade
through ancestor heights and push free SUs out of delay slots.
Phase 2 fixes the root cause: for function-exit RET regions the
projection is empty (no MBB successors) so the probe finds no
conflict and adds no edge; for mid-CFG branches the projection
reflects real successor demand, often much smaller than the
Conservative full-block.

When Phase 2 lands, the Commit-1 gate can be removed (or kept as
a defense-in-depth guard until projection coverage is proven by
the regression suite). Either way, the projection should be the
default consumer of `initializeBotScoreBoard`'s priming, and the
Conservative-trust path should be the explicit fallback when
neighbours are unscheduled.

### Phase 2 — Soundness

At runtime, exactly one predecessor edge enters MBB `B` and one
successor edge leaves. We don't know which. The conservative-but-
tightest static overapproximation is the per-cycle bitwise OR:

```
PredScoreboard(B)[c] = OR over P in pred(B): ResourceFootprint(tail(P))[c]
SuccScoreboard(B)[c] = OR over S in succ(B): ResourceFootprint(head(S))[c]
```

Any single execution path's actual blockage is a subset of this
union; scheduling decisions made under the union are sound for every
path.

### Phase 2 — New

#### `BlockState` projection fields

```cpp
// Added to BlockState in AIEInterBlockScheduling.h.
ResourceScoreboard<FuncUnitWrapper> PredProjection;
ScoreboardTrust                     PredTrust = ScoreboardTrust::Conservative;
bool                                PredProjectionValid = false;

ResourceScoreboard<FuncUnitWrapper> SuccProjection;
ScoreboardTrust                     SuccTrust = ScoreboardTrust::Conservative;
bool                                SuccProjectionValid = false;
```

Cycle frame: `PredProjection[c]` represents the resource demand `c`
cycles before this block's first cycle (read backwards from the join
point); `SuccProjection[c]` represents the demand `c` cycles after
this block's last cycle.

#### `InterBlockScheduling::buildPredProjection` / `buildSuccProjection`

```cpp
// AIEInterBlockScheduling.{h,cpp}, ~120 LOC total.
void buildPredProjection(BlockState &BS);
void buildSuccProjection(BlockState &BS);
void invalidateNeighbourProjections(BlockState &BS);
```

`buildPredProjection`: walk each `P in pred(BS.TheBlock)`. If `P` is
scheduled, derive `P`'s tail scoreboard from
`P_BlockState.getRegions().back().Bundles`'s last
`HR->getPipelineDepth()` cycles via
`HR->emitInScoreboard` against a fresh empty scoreboard. `unionInto`
the result into `BS.PredProjection`. Set `BS.PredTrust` per neighbour
alignment / coverage.

`buildSuccProjection`: symmetric over successors.

`invalidateNeighbourProjections`: when `BS` is rescheduled, mark each
neighbour's matching `*ProjectionValid` field false. Next time a
neighbour re-enters scheduling, it rebuilds.

#### Trust classification (concrete)

Set `*Trust` per the existing enum (`AIEInterBlockScheduling.h:296-304`):

* `Absolute` — every neighbour is scheduled and the join is
  alignment-exact. Engine queries the projection at literal cycle
  offsets.
* `AccountForAlign` — every neighbour scheduled but at least one join
  has `MachineBasicBlock::getAlignment() != 0` introducing one-cycle
  slack. `buildPredProjection` widens by ORing each FuncUnitWrapper
  into both cycle `c` and cycle `c+1`.
* `Conservative` — at least one neighbour unscheduled. Engine ignores
  the projection (treats as empty); caller continues to enforce
  today's `getLatencyCap()` / `getBlockedResourceCap()` outside the
  engine. **No regression vs today** in this case.

#### Scheduling-order interaction

`InterBlockScheduling::defineSchedulingOrder` schedules **loops first,
then bottom-up over the CFG** (`AIEInterBlockScheduling.h:336-337`).
Loop-first is what makes loop tightness the priority — and loops are
the high-value case where projections matter most because their
tail/head feed surrounding straight-line code.

Per-direction availability for a non-loop MBB `B`:

* `SuccProjection`: **available**. Bottom-up order means every
  successor of `B` is already scheduled when `B` is. Trust =
  `Absolute` or `AccountForAlign`.
* `PredProjection`: **conditional**:
  * If `P` is a loop scheduled in the loops-first sweep (the common
    case for the high-value transition), it is already scheduled.
    Trust = `Absolute` / `AccountForAlign`.
  * **If `P` has no fixed instructions** (the user's explicit point),
    `P`'s contribution to the union is empty —
    `unionInto` leaves the result unchanged. No constraint added; no
    Trust downgrade forced.
  * If `P` is a not-yet-scheduled regular block (rare in bottom-up
    order, possible for irreducible CFGs), the union is a strict
    underapproximation. Trust drops to `Conservative`; engine
    discards the projection; legacy cap kicks in.

Loop MBBs in Phase 2: `PredScoreboard` / `SuccScoreboard` typically
empty (loops are scheduled first, before any neighbour). Phase 2 is
**not** trying to project into loop bodies; the win is for
surrounding code consuming a loop's tail.

#### Build / invalidate call sites

* In `InterBlockScheduling::enterRegion` (near
  `AIEInterBlockScheduling.cpp:744-750` `addRegion(...)`): if
  `BS.PredProjectionValid` is false, call `buildPredProjection(BS)`.
  Symmetric for Succ.
* After `BS` finishes scheduling (in `leaveBlock` or equivalent):
  call `invalidateNeighbourProjections(BS)`.

#### Phase 2 codegen tests

Layout under
`llvm/test/CodeGen/AIE/aie2*/schedule/cross-mbb-scoreboard/`. Each
`.mir` runs twice via `-aie-cross-mbb-scoreboard={0,1}` with
`--check-prefix=OLD` and `--check-prefix=NEW` to make the win
self-documenting.

* **Group A — single-pred motivating case.**
  `swp-store-then-load.mir`: SWP loop with a vector `VST`; the
  fall-through MBB after the SWP epilogue starts with a vector `VLD`
  to a `noalias` pointer. OLD: load bundles at cycle 0 of
  `succ_mbb` with margin nops separating it from the store-tail.
  NEW: load bundles tighter; trailing margin nop gone; bundle count
  of `succ_mbb` strictly smaller.
* **Group B — multi-pred union (top side).**
  `multi-pred-union-top.mir`: diamond CFG, P1 ends with MAC tail
  occupying MAC pipe cycles -3..-1, P2 ends with VLD tail occupying
  load pipe cycles -2..-1. OLD: both candidates in B pushed to
  `max(getLatencyCap)`. NEW: family-disjoint candidates relax
  independently — MAC candidate to cycle 3, VLD candidate to cycle
  2.
  `multi-pred-union-resource-conflict.mir`: P1 and P2 both end
  with VLDs to different banks; in B a VLD using either bank must be
  delayed by the *stricter* of the two. Verifies the union is tight.
* **Group C — multi-succ union (bot side).**
  `multi-succ-union-bot.mir`: B has two successors with disjoint
  resource families at their heads; OLD blocks the bottom for both;
  NEW relaxes per-family.
  `multi-succ-cold-path-strictest.mir`: hot-vs-cold successor
  asymmetry; soundness check that the cold (heavy) path's footprint
  is respected.
* **Group D — conservative fallback.**
  `unscheduled-pred-fallback.mir`: irreducible region forcing
  `Conservative` trust; OLD == NEW; debug log line confirms
  fallback fired.
  `function-entry-no-preds.mir`: no predecessors; assert empty
  projection injects no phantom blocked cycles.
* **Group E — alignment.**
  `pred-align-shift.mir`: `AccountForAlign` widens projection by
  one cycle in both shift positions; assert candidate placement
  picks first cycle that is free in both shifts.

#### Phase 2 unit tests

* `unittests/Target/AIE/InterBlockProjectionTest.cpp`:
  * `BuildPredProjection_SingleScheduledPred`: one scheduled
    predecessor, assert projection equals `P`'s tail.
  * `BuildPredProjection_TwoScheduledPreds`: two scheduled, assert
    projection equals union (bitwise OR per cycle).
  * `BuildPredProjection_OneUnscheduled`: one scheduled + one
    unscheduled; assert `Conservative` trust and projection
    discarded by engine.
  * `BuildPredProjection_AlignmentSlack`: `getAlignment() != 0`;
    assert `AccountForAlign` trust and per-cycle widening.
  * Symmetric `BuildSuccProjection_*` cases.
  * `Invalidate_OnNeighbourReschedule`: trigger a reschedule of
    `BS`, assert all neighbours' matching `*ProjectionValid` flip
    to false.

#### Command-line option

* `-aie-cross-mbb-scoreboard` (cl::opt, default 0). Flips to 1 once
  the demo tests stabilise.

### Phase 2 — Changed

* `InterBlockScheduling::enterRegion` and `leaveBlock` — ~20 LOC
  of new calls into the build / invalidate helpers.
* `AIEMachineScheduler.cpp::AIEPostRASchedStrategy::enterRegion`
  (`:839`) — populate `Cfg.PredScoreboard`, `Cfg.SuccScoreboard`,
  `Cfg.PredTrust`, `Cfg.SuccTrust` from the BlockState's projection
  fields. Gate `getLatencyCap()` / `getBlockedResourceCap()` calls
  on `Conservative` trust mode (skip them when a projection is
  active). ~30 LOC.

### Phase 2 — Removed

Nothing. The legacy cap path remains as the `Conservative`-trust
fallback. A future Phase 3 can remove it once the projection path
covers all production CFG shapes.

### Phase 2 — Verification

* `ninja check-llvm-codegen-aie` bit-identical when
  `-aie-cross-mbb-scoreboard=0` — proves the new code is purely
  additive when the flag is off.
* OLD vs NEW prefix CHECKs in each Group A/B/C demo test prove the
  scheduling win.
* `ninja check-llvm-codegen-aie` green at default flag once flipped
  to 1.

---

## Critical files (Phase 1)

* `llvm/lib/Target/AIE/AIEPostPipeliner.{h,cpp}` — strategy
  interface, `fitInInterval` (`cpp:60-79`), first-iteration
  emission (`:846-911`), validation replay (`:1052-1072`).
* `llvm/lib/Target/AIE/AIEMachineScheduler.cpp` — Top/Bot priming
  (`:282,285,373,406`); delay-slot floor (`:418-459,502-503,1007-1009`).
* `llvm/lib/Target/AIE/AIEInterBlockScheduling.{h,cpp}` —
  `Region` (`h:138-187`), `BlockState` (`h:196-293`),
  `PipelineExtractor` (`cpp:298-350`).
* `llvm/lib/Target/AIE/AIEHazardRecognizer.{h,cpp}` —
  `FuncUnitWrapper` (`h:64-126`, `cpp:128-143`), `emitInScoreboard`,
  `blockCycleInScoreboard`, `checkConflict`.
* `llvm/include/llvm/CodeGen/ResourceScoreboard.h` — generic
  scoreboard template (consumed unchanged).

## Coverage of "regmemtracker dependencies"

The user asked whether scoreboards naturally cover the dependency
axis, not just resource. Coverage:

* **Resource axis** — slots, FUs, conflicts: directly in
  `FuncUnitWrapper`, hence in any scoreboard layer.
* **Memory axis** — banks and memory objects: in `FuncUnitWrapper`
  (`AIEHazardRecognizer.cpp:128-143` ANDs `MemoryBanks`,
  `MemoryObjects`). Mem-aliasing across MBBs is covered once
  projections are populated.
* **Register data-dep axis** — long-latency def in pred whose
  result is consumed here, with the def's pipeline tail still
  occupying the writeback slot at this MBB's entry: partially
  covered. The FU/slot reservation of the tail blocks the cycle for
  *resource* hazards. True RAW/WAW dependence on a register class
  crossing the boundary is NOT in the scoreboard — lives in the DAG
  via `addSchedBarrierDeps` / `ExitSU`, intra-region today.

Verdict: scoreboard projection is the right layer for resource +
memory + most hazard-tail effects. Cross-MBB register data deps are
out of scope (see below).

## Out of scope

* Cross-MBB **register** data dependence tracking via phantom
  EntrySU/ExitSU edges (richer extension of `addSchedBarrierDeps`
  walking pred/succ MBBs). Future work; `InterBlockEdges`
  (`AIEInterBlockScheduling.h:37-79`) is the natural starting point.
* Outer-loop SWP integration code itself (the policy that decides
  *when* to call `pushTopFixedToPredecessor`, and the driver that
  invokes it after innermost loops are scheduled). Phase 1 only
  delivers the engine support and the per-cycle push primitive.
* Generalizing modulo (`II`) to non-loop MBBs — modulo stays
  loop-only; engine runs in non-modulo mode (`II = 0`) for non-loop
  call sites.
* Removing the legacy `getLatencyCap` / `getBlockedResourceCap` —
  retained as `Conservative`-trust fallback. Future Phase 3.
* Any change to `BoundaryNode` semantics in `PostPipeliner` —
  Earliest/Latest skipping stays where it is.
