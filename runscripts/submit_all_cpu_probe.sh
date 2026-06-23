#!/usr/bin/env bash
set -euo pipefail

BASE="/home/cbr406/Projects/Ravenna"
LOG_DIR="$BASE/outputs/logs"
OUT_DIR="$BASE/outputs/cpu_probe"
RESULT_TSV="$OUT_DIR/cpu_config.tsv"

mkdir -p "$LOG_DIR" "$OUT_DIR"

# Clear old results
rm -f "$RESULT_TSV" "$RESULT_TSV.lock"

PARTITION="${PARTITION:-gpu}"

echo "Partition: $PARTITION"
echo "Result:    $RESULT_TSV"
echo

# Automatically read all nodes in the gpu partition from sinfo
sinfo -h -p "$PARTITION" -N -o "%N %T" | sort -u | while read -r node state; do
    if [[ -z "$node" ]]; then
        continue
    fi

    # Skip unavailable nodes such as down / drain / fail / maint
    if echo "$state" | grep -Eiq 'down|drain|draining|drained|fail|maint|unknown'; then
        echo "SKIP   $node  state=$state"
        continue
    fi

    echo "SUBMIT $node  state=$state"

    sbatch \
      --partition="$PARTITION" \
      --nodelist="$node" \
      --job-name="cpu_${node}" \
      --export=ALL,RESULT_TSV="$RESULT_TSV" \
      cpu_probe.sbatch
done

echo
echo "Submitted all available nodes."
echo
echo "Check jobs:"
echo "  squeue -u \$USER -n cpu_probe"
echo "  squeue -u \$USER | grep cpu_"
echo
echo "After jobs finish, view result:"
echo "  column -t -s \$'\t' $RESULT_TSV | less -S"