#!/usr/bin/env bash
# Run all libdecimal benchmarks and collect output into a single timestamped log.
# Usage: bench/run-all.sh [build-dir]
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${1:-${SCRIPT_DIR}/../build}"
LOG_DIR="${SCRIPT_DIR}/../bench-results"
STAMP=$(date '+%Y%m%d-%H%M%S')
LOG_FILE="${LOG_DIR}/bench-${STAMP}.txt"

BENCHMARKS=(
    bench-construct-from-pair
    bench-decompose
    bench-compare
    bench-arithmetic
    bench-string
    bench-ops-fee
    bench-encode-decode
    bench-mixed-ops
)

mkdir -p "${LOG_DIR}"

echo "==> building benchmarks (ninja -j$(nproc))"
cmake --build "${BUILD_DIR}" --target "${BENCHMARKS[@]}" -- -j"$(nproc)" 2>&1

echo ""
echo "==> running ${#BENCHMARKS[@]} benchmarks — baseline: boost decimal"
echo "==> log: ${LOG_FILE}"
echo ""

{
    echo "stamp: ${STAMP}"
    echo "host:  $(uname -srm)"
    echo "cpu:   $(grep -m1 'model name' /proc/cpuinfo | cut -d: -f2 | xargs)"
    echo "build: $(cmake --build "${BUILD_DIR}" --target bench-arithmetic -- -n 2>&1 | grep -oP '(?<=\-O)\d' | head -1 || echo 'unknown')"
    echo ""

    for b in "${BENCHMARKS[@]}"; do
        echo "════════════════════════════════════════════════════════════════"
        echo "benchmark: ${b}"
        echo "════════════════════════════════════════════════════════════════"
        "${BUILD_DIR}/${b}"
        echo ""
    done
} 2>&1 | tee "${LOG_FILE}"

echo ""
echo "done. results in: ${LOG_FILE}"
