#!/usr/bin/env bash
# run_benchmarks.sh — Performance test for ORAM CLI using existing storage.txt & operations.txt

set -euo pipefail

# 1. Compile ORAM CLI tool
echo "Compiling ORAM CLI tool..."
g++ -std=c++17 -O2 path_oram.cpp Tree.cpp -o path_oram_cli

echo
echo "Using existing storage.txt & operations.txt"
echo
# 3. Run ORAM CLI with test files and measure time
echo "Running ORAM CLI with storage.txt & operations.txt..."
TIMEFORMAT="CLI runtime: %R seconds"
TIMEFORMAT="CLI runtime: %R seconds"
{ time bash -c "printf 'store storage.txt
operate operations.txt
exit
' | ./path_oram_cli > /dev/null"; }

echo
# All done
echo "=== CLI performance test completed ==="
