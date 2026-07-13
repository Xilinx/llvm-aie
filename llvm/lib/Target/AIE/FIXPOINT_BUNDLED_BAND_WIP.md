# FixPoint Fixed-Band Scheduling — WIP: Bundled Representation Exploration

**Status: WORK IN PROGRESS with a known regression. Do not merge as-is.**
Branch base: `eceb1860` (committed, good). The uncommitted-turned-WIP changes on top explore a bundled representation and currently regress one test — details below.

## Background
FixPoint scheduling places software-pipelined (SWP) prologue/epilogue "fixed bands" together with free instructions. Two fixed bands exist per region: a **top-fixed** band (SWP epilogue) and a **bottom-fixed** band (SWP prologue). Free instructions co-issue into the bands' open VLIW slots.

## What is committed and good (`eceb1860`)
The **loose per-MI model**:
- The fixed band is identified by **owned MI pointers** (`Region::TopFixedMIs`/`BotFixedMIs`), not by physical block position — this removed the per-pass "restore/strip" dance.
- An **aggressive debug placement assert** (`verifyFixedBandGeometry`) verifies every fixed-band MI lands at the cycle dictated by the band geometry (independent ground truth).
- The band is emitted as **loose, per-instruction SUnits** during scheduling; `finalizeBundle` is deferred to commit. Each fixed instruction is its own SUnit with its own true ready-cycle.

## What this WIP explores (the uncommitted-now-committed diff)
A **bundled representation**: emit the fixed band as real `BUNDLE`s (`ApplyBundling=true`), create **one SUnit per bundle**, per-bundle pinning, delete the `MIToCycle`/`verifyTopFixedBundleCycles` machinery, replace `BandGeometry` with `top_fixed_cycles()`/`bot_fixed_cycles()` arrays. Net ~-65 lines. Goal: stop "ripping apart" bundles into loose MIs.

## Known regression (why this is WIP, not done)
Test: `llvm/test/CodeGen/AIE/aie2/schedule/postpipeliner/hardsigmoid-templated-double.mir`, block `bb.2` (where loop-1 drain / bottom-fixed abuts loop-2 fill / top-fixed). Schedule is **+1 VLIW cycle** vs baseline.

Root cause: at the abutment cycle, one SUnit (`SU20`) was made to contain BOTH a bottom-band `VMAX_LT_S16` (deep chain, ready cycle 8) AND a top-band `VLDB_UNPACK` (ready cycle 0). Welding two independent instructions into one SUnit inflates its `TopReadyCycle` to the max (16), which trips `checkInterZoneConflicts` (`AIEMachineScheduler.cpp:1442,1518-1543`) and forces ~8 spurious grow-to-fit bumps (`SchedulingLength` 14->22). Those bumps slide the top-fixed band's pinned floor (`pinTopFixedBand`) and break the co-issue alignment. In the loose model the same two instructions are separate SUnits (ready cycles 0 and 8, both satisfied) so no false conflict fires.

Second diff (benign, acceptable): `interleave-prologue.mir` — a `dead` flag missing on a bundled `implicit-def` (liveness recompute from earlier `finalizeBundle`); no verifier or downstream-pass impact.

## Findings established during investigation
- Legacy (non-FixPoint) **already co-issues** free MIs into fixed bundles (via the hazard scoreboard + read-back merge). Co-issue is not FixPoint-specific.
- The regression is caused by **cross-band SUnit welding**, not by the physical `BUNDLE` header itself: `checkInterZoneConflicts` reads SUnit `TopReadyCycle`s, never the block's bundle header.
- **Deferring `finalizeBundle` does NOT fix it** — that targets the header; the problem is SUnit granularity/welding. Deferring while still recording one-MI-per-bundle would also drop the co-issued sibling (a correctness break).

## Plan / open question (next step — not yet resolved)
Rethink so the **top-fixed band, bottom-fixed band, and free instructions are all DISTINCT SUnits**; a co-issue cycle must be multiple SUnits sharing a cycle, never one welded SUnit. Open questions to resolve before this can land:
1. Why exactly did a top-band MI and a bottom-band MI end up in one SUnit? (Trace `TopInsert`/`BottomInsert` population, `emitInterBlockTop/Bottom`, `recordFixedMIs`, `createFixedSUDAGNodes`.)
2. Does separating the bands (no cross-band weld) fix the `hardsigmoid` regression?
3. **Intra-band welding:** does a multi-MI band bundle held as one SUnit reintroduce the same ready-cycle inflation when a *free* instruction co-issues into that cycle — forcing per-MI granularity within a band too?
4. Verdict: is a per-band-bundle design meaningfully distinct from / better than the committed loose per-MI model (`eceb1860`), or does correctly avoiding all harmful welding converge back on loose?

## Fallback
If the rethink converges on loose, keep `eceb1860` and drop this WIP. The loose model is correct because per-instruction SUnits preserve independent ready-cycles, which is what enables flexible co-issue.

## Key files
`AIEInterBlockScheduling.{h,cpp}`, `AIEBaseSubtarget.cpp`, `AIEMachineScheduler.{cpp,h}`
