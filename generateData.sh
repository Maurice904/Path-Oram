#!/usr/bin/env bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

set -euo pipefail

N=$1

STORAGE_FILE="storage.txt"
OPER_FILE="operations.txt"

echo "generate $N inital data to $STORAGE_FILE"
python3 "$SCRIPT_DIR/scripts/fileGen.py" storage "$N" \
    --output "$STORAGE_FILE" \
    --max-value 1000000000

echo "generate $N initial data to $OPER_FILE"
python3 "$SCRIPT_DIR/scripts/fileGen.py" operation "$N" \
    --output "$OPER_FILE" \
    --max-position "$N" \
    --max-value 1000000000 \
    --read-ratio 0.5

echo "Done!"
echo "  $STORAGE_FILE ($N data)"
echo "  $OPER_FILE ($N operations)"
