#!/usr/bin/env bash
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Modifications (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
#
# Diagnoses a multi-minute cmake stall at "Set CPU features" by capturing libc
# CPU/compiler feature-probe timing. Each probe is a full try_compile with the
# just-built cross clang; the instrumented cmake modules emit
# "[HH:MM:SS.mmm] ... done <N>ms" lines, so the offending probe is the one whose
# elapsed time explodes (or the lone "start" line with no "done" if it hangs).
#
# Run from the repo root. The build dir is wiped and configured from scratch:
# cached build folders skip the runtimes sub-configure, so the probes never
# re-run and no log is produced.
#
# Usage: ./diagnose_feature_probe_stall.sh [build-dir]
#   build-dir defaults to "build".
#   BUILD_TYPE env var overrides CMAKE_BUILD_TYPE (default Release); set to
#   RelWithDebInfo to reproduce that configuration.
#
# Produces, per runtime target:
#   <build-dir>/feature-probe-<target>.log   (timestamped sub-configure output)
# Hand these logs back for analysis.

set -u

build_dir="${1:-build}"
build_type="${BUILD_TYPE:-Release}"

# Must run from the repo root of an llvm-aie checkout.
if [[ ! -e .git ]]; then
  echo "error: run from the repo root (no .git here)" >&2
  exit 1
fi
if ! git remote -v | grep -q "llvm-aie"; then
  echo "error: this does not look like an llvm-aie repository" >&2
  exit 1
fi

stamp() { while IFS= read -r line; do printf '[%s] %s\n' "$(date '+%H:%M:%S.%3N')" "$line"; done; }

echo "build dir  : ${build_dir}"
echo "build type : ${build_type}"

# Wipe so the configure (and the feature probes) run from scratch.
echo "=== wiping ${build_dir} ==="
rm -rf "${build_dir}"
mkdir -p "${build_dir}"

# Configure, replicating the project's init-build flags.
echo "=== configuring ==="
configure_log="${build_dir}/feature-probe-configure.log"
(
  cd "${build_dir}"
  cmake ../llvm/ -GNinja \
    -DCMAKE_BUILD_TYPE="${build_type}" \
    -DLLVM_USE_SPLIT_DWARF=true \
    -DLLVM_ENABLE_ASSERTIONS=true \
    -DLLVM_CCACHE_BUILD=true \
    -DCMAKE_CXX_FLAGS=-fdebug-prefix-map="$PWD"=. \
    -DLLVM_USE_LINKER=lld \
    -DLLVM_ENABLE_Z3_SOLVER=ON \
    -DLLVM_Z3_INSTALL_DIR=/scratch/z3/build \
    -C ../clang/cmake/caches/Peano-AIE.cmake
) 2>&1 | stamp | tee "${configure_log}"

if [[ ! -f "${build_dir}/CMakeCache.txt" ]]; then
  echo "error: configure failed; see ${configure_log}" >&2
  exit 1
fi

# Discover configured runtime targets from the freshly written cache.
targets_line="$(grep -m1 '^LLVM_RUNTIME_TARGETS:' "${build_dir}/CMakeCache.txt" || true)"
targets="${targets_line#*=}"
targets="${targets//;/ }"
if [[ -z "${targets}" ]]; then
  echo "error: LLVM_RUNTIME_TARGETS not set in cache; is this a runtimes build?" >&2
  exit 1
fi
echo "targets    : ${targets}"

# Trigger each runtimes sub-configure (builds clang first, then runs the
# instrumented probes) and capture the timestamped output per target.
for tgt in ${targets}; do
  log="${build_dir}/feature-probe-${tgt}.log"
  echo
  echo "=== ${tgt} -> ${log} ==="
  ninja -C "${build_dir}" "runtimes/runtimes-${tgt}-configure" 2>&1 | stamp | tee "${log}"

  echo "--- ${tgt} probe summary ---"
  grep -E "feature probe '.*' done" "${log}" || echo "(no probe lines — target may skip feature detection)"
done

echo
echo "done. collect: ${build_dir}/feature-probe-*.log"
