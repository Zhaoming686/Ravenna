#!/bin/bash
#SBATCH -p gpu
#SBATCH --job-name=genquery
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=16G
#SBATCH --time=04:00:00
#SBATCH --output=/home/cbr406/Projects/Ravenna/outputs/logs/%x-%j.out
#SBATCH --error=/home/cbr406/Projects/Ravenna/outputs/logs/%x-%j.err

set -euo pipefail

BASE="/home/cbr406/Projects/Ravenna"
mkdir -p "$BASE/outputs/logs"

# GCC 13.2.0 runtime
export PATH="/opt/software/gcc/13.2.0/bin:$PATH"
export LD_LIBRARY_PATH="/opt/software/gcc/13.2.0/lib64:/opt/software/gcc/13.2.0/lib:${LD_LIBRARY_PATH:-}"

# actual gen_query executable path
BIN="$BASE/cmake-build-run_test/matching/QueryGen.out"

# sbatch run_genquery.sh 15 dense 42
L="${L:-${1:-15}}"
QPROP="${QPROP:-${2:-dense}}"
SEED="${SEED:-${3:-42}}"

LDIR="$BASE/datasets/L${L}"

RUN_DATE="$(TZ=Europe/Copenhagen date +%Y-%m-%d)"
RUN_DIR="$BASE/outputs/genquery_L${L}_${QPROP}-${RUN_DATE}"
mkdir -p "$RUN_DIR"
LOG="$RUN_DIR/result.log"

{
  echo "HOST=$(hostname)  DATE=$(date)  JOB=${SLURM_JOB_ID:-N/A}"
  echo "BIN=$BIN"
  echo "LDIR=$LDIR  QPROP=$QPROP  SEED=$SEED"
  echo "=============================="
} > "$LOG"

[[ -x "$BIN" ]]              || { echo "ERROR: BIN not executable: $BIN" >> "$LOG"; exit 1; }
[[ -d "$LDIR/data_graph" ]]  || { echo "ERROR: $LDIR/data_graph not found" >> "$LOG"; exit 1; }

cmd=( "$BIN" -Ldir "$LDIR" -qprop "$QPROP" -seed "$SEED" )
echo "CMD=${cmd[*]}" >> "$LOG"

set +e
stdbuf -oL -eL "${cmd[@]}" 2>&1 | tee -a "$LOG"
rc=${PIPESTATUS[0]}
set -e

echo "---- DONE rc=$rc ----" >> "$LOG"
exit "$rc"