#!/bin/bash
#SBATCH -p gpu
#SBATCH --job-name=ravenna
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=16G
#SBATCH --time=40:00:00
#SBATCH --output=/home/cbr406/Projects/Ravenna/outputs/logs/%x-%j.out
#SBATCH --error=/home/cbr406/Projects/Ravenna/outputs/logs/%x-%j.err

set -euo pipefail

# If this is not a Slurm job, automatically submit this script:
# Only allow hendrixgpu23fl/24fl/25fl/26fl; Slurm decides which one is scheduled
if [[ -z "${SLURM_JOB_ID:-}" ]]; then
    script_path="$(readlink -f "$0")"

    ALLOW_RE='^(hendrixgpu23fl|hendrixgpu24fl|hendrixgpu25fl|hendrixgpu26fl)$'

    EXCLUDE_LIST="$(
      sinfo -h -p gpu -N -o "%N" \
      | sort -u \
      | grep -Ev "$ALLOW_RE" \
      | paste -sd, -
    )"

    echo "Allowed nodes: hendrixgpu23fl,hendrixgpu24fl,hendrixgpu25fl,hendrixgpu26fl"
    echo "Submitting to Slurm; scheduler will choose whichever can run."

    exec sbatch \
      --exclude="$EXCLUDE_LIST" \
      "$script_path" "$@"
fi

# Add the GCC 13.2.0 runtime back
export PATH="/opt/software/gcc/13.2.0/bin:$PATH"
export LD_LIBRARY_PATH="/opt/software/gcc/13.2.0/lib64:/opt/software/gcc/13.2.0/lib:${LD_LIBRARY_PATH:-}"

BASE="/home/cbr406/Projects/Ravenna"

# sbatch run_simple.sh <DATASET> <ENGINE> <BUILD_VARIANT> <L> "<SIZE_LIST>" <TAU>
# e.g.
# sbatch run_simple.sh dblp Ravenna test 15 "12 24 36 48 60" 0.4
# sbatch run_simple.sh dblp Ravenna test original "12 24 36 48 60" "0.2,0.3,0.4,0.5,0.6"
# L can be 15/30/45/60 or the string "original"
# TAU is a single decimal value, or a list

DATASET="$1"
ENGINE="$2"
BUILD_VARIANT="$3"
L="$4"

# Automatically add the prefix
BUILD_DIRNAME="cmake-build-run_${BUILD_VARIANT}"

# Fifth argument: query-size list to run; use defaults if omitted
if [[ $# -ge 5 ]]; then
  read -r -a RUN_SIZES <<< "$5"
else
  RUN_SIZES=(12 24 36 48 60)
fi

# Sixth argument: tau threshold; defaults a list for dblp
TAU="${6:-0.3515,0.4368,0.2876,0.2507,0.2071,0.2840,0.2572,0.2883,0.2232,0.2965,0.2895,0.4913,0.4119,0.1899,0.3373}"


# Fixed parameters; edit them here if needed
TIME_LIMIT=50
FAIRT=2
FILTER="VEQ"
ORDER="CFL"
NUM=-1
SYMMETRY=1
SF="Coverage"

if [[ "$L" == "original" ]]; then
    DATASET_DIR="$BASE/datasets/original"
    DATA_GRAPH="$DATASET_DIR/data_graph/${DATASET}.graph"
    QUERY_DIR="$DATASET_DIR/query_graph/${DATASET}"
else
    DATASET_DIR="$BASE/datasets/L${L}"
    DATA_GRAPH="$DATASET_DIR/data_graph/${DATASET}_${L}.graph"
    QUERY_DIR="$DATASET_DIR/query_graph/${DATASET}_${L}"
fi
BIN="$BASE/$BUILD_DIRNAME/matching/SubgraphMatching.out"

RUN_DATE="$(date +%Y-%m-%d)"
if [[ -n "${SLURM_ARRAY_JOB_ID:-}" ]]; then
  JOB_TAG="array${SLURM_ARRAY_JOB_ID}_task${SLURM_ARRAY_TASK_ID}_job${SLURM_JOB_ID}"
else
  JOB_TAG="job${SLURM_JOB_ID:-nojid}"
fi

RUN_NAME="${ENGINE}_${DATASET}_L${L}_tau${TAU}_${BUILD_VARIANT}-${RUN_DATE}-${JOB_TAG}"
RUN_DIR="$BASE/outputs/$RUN_NAME"

mkdir -p "$RUN_DIR"
mkdir -p "$BASE/outputs/logs"

TSV_FILE="$RUN_DIR/${RUN_NAME}.tsv"
RESULT_FILE="$RUN_DIR/result.log"

echo -e "data_graph\tquery_graph\tt_ms\thard_sat\tcov_sat\tavg_rel_cov\tcall_count\tprune_count\tembedding_cnt" > "$TSV_FILE"

{
  echo "RUN_NAME=$RUN_NAME"
  echo "BIN=$BIN"
  echo "DATA_GRAPH=$DATA_GRAPH"
  echo "QUERY_DIR=$QUERY_DIR"
  echo "RUN_SIZES=${RUN_SIZES[*]}"
  echo "TAU=$TAU"
  echo
} > "$RESULT_FILE"

QUERIES=()
for sz in "${RUN_SIZES[@]}"; do
  while IFS= read -r q; do
    QUERIES+=("$q")
  done < <(find "$QUERY_DIR" -type f -name "query_G_${sz}_*.graph" | sort)
done

echo "TOTAL_GRAPH_FILES=${#QUERIES[@]}" >> "$RESULT_FILE"
echo >> "$RESULT_FILE"

cd "$RUN_DIR"

for QUERY_GRAPH in "${QUERIES[@]}"; do
  echo "==============================" >> "$RESULT_FILE"
  echo "QUERY_GRAPH=$QUERY_GRAPH" >> "$RESULT_FILE"
  echo "==============================" >> "$RESULT_FILE"

    cmd=(
      "$BIN"
      -d "$DATA_GRAPH"
      -q "$QUERY_GRAPH"
      -filter "$FILTER"
      -order "$ORDER"
      -engine "$ENGINE"
      -num "$NUM"
      -symmetry "$SYMMETRY"
      -FairT "$FAIRT"
      -time "$TIME_LIMIT"
      -SF "$SF"
      -tau "$TAU"
    )

  echo "CMD=${cmd[*]}" >> "$RESULT_FILE"

  set +e
    stdbuf -oL -eL "${cmd[@]}" 2>&1 \
      | tee -a "$RESULT_FILE" \
      | awk \
        -v dg="$DATA_GRAPH" \
        -v qg="$QUERY_GRAPH" '
        BEGIN{
          OFS="\t";
          n = split(dg, A, "/"); dg_base = A[n];
          m = split(qg, B, "/"); qg_base = B[m];
        }
        $1=="CHECK" && NF>=8 {
          print dg_base, qg_base, $2, $3, $4, $5, $6, $7, $8
        }' >> "$TSV_FILE"
  set -e

  echo >> "$RESULT_FILE"
done

echo "DONE: $RESULT_FILE" >> "$RESULT_FILE"
echo "TSV: $TSV_FILE" >> "$RESULT_FILE"