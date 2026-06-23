#!/bin/bash
#SBATCH -p gpu
#SBATCH --job-name=mixmatch_batch
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=16G
#SBATCH --time=40:00:00
#SBATCH --output=/home/cbr406/Projects/Mix-Match/SubgraphMatchingSurvey-main/vlabel/outputs/logs/%x-%A_%a.out
#SBATCH --error=/home/cbr406/Projects/Mix-Match/SubgraphMatchingSurvey-main/vlabel/outputs/logs/%x-%A_%a.err

set -euo pipefail

BASE="/home/cbr406/Projects/Mix-Match/SubgraphMatchingSurvey-main/vlabel"
cd "$BASE"

mkdir -p "$BASE/outputs/logs"

# only use these nodes for the experiments
NODELIST="hendrixgpu23fl,hendrixgpu24fl,hendrixgpu25fl,hendrixgpu26fl"

# datasets to run on
DATASETS=(
  youtube
  patents
  eu2005
  dblp
  web-BerkStan
  yeast
)
#  dblp
#  eu2005
#  patents
#  web-BerkStan
#  youtube
#  yeast

# format is: "ENGINE BUILD_VARIANT"
METHODS=(
  "Ravenna test"
)
#"Ravenna normal"
#  "RavennaForward normal"
#  "RavennaCheck normal"
#  "MMDIV normal"
#  "MMIDIV normal"
#  "MATCODIV normal"
#  "DSQLPDIV normal"
#  "DSQLDIV normal"
LABELS=(original) #LABELS=(original 15 30 45 60)

TAUS=(0.1 0.2 0.4 0.5) #TAUS=(0.1 0.2 0.4 0.5)

# query sizes for each experiment
SIZES="12 24 36 48 60"

# maximum tasks to run in parallel
MAX_PARALLEL=20

NUM_DATASETS=${#DATASETS[@]}
NUM_METHODS=${#METHODS[@]}
NUM_LABELS=${#LABELS[@]}
NUM_TAUS=${#TAUS[@]}

NUM_TASKS=$(( NUM_LABELS * NUM_DATASETS * NUM_METHODS * NUM_TAUS ))

# if not a Slurm array job, then submit it as one
if [[ -z "${SLURM_JOB_ID:-}" ]]; then
  script_path="$(readlink -f "$0")"

  echo "Submitting Slurm job array..."
  echo "Allowed nodes: $NODELIST"
  echo "Datasets: ${DATASETS[*]}"
  echo "Methods: ${METHODS[*]}"
  echo "Total tasks: $NUM_TASKS"

  ALLOW_RE='^(hendrixgpu23fl|hendrixgpu24fl|hendrixgpu25fl|hendrixgpu26fl)$'
  EXCLUDE_LIST="$(
    sinfo -h -p gpu -N -o "%N" \
    | sort -u \
    | grep -Ev "$ALLOW_RE" \
    | paste -sd, -
  )"

    exec sbatch \
      --array=0-$((NUM_TASKS - 1))%${MAX_PARALLEL} \
      --exclude="$EXCLUDE_LIST" \
      "$script_path" "$@"
fi

# prevent accidental submission without --array
if [[ -z "${SLURM_ARRAY_TASK_ID:-}" ]]; then
  echo "ERROR: This script must be submitted as a Slurm job array."
  echo "Use:"
  echo "  ./run_batch_array.sh"
  exit 1
fi

TASK_ID=${SLURM_ARRAY_TASK_ID}

PER_L=$(( NUM_DATASETS * NUM_METHODS * NUM_TAUS ))
PER_D=$(( NUM_METHODS * NUM_TAUS ))

L_INDEX=$(( TASK_ID / PER_L ))
REM=$(( TASK_ID % PER_L ))

DATASET_INDEX=$(( REM / PER_D ))
REM=$(( REM % PER_D ))

METHOD_INDEX=$(( REM / NUM_TAUS ))
TAU_INDEX=$(( REM % NUM_TAUS ))

L="${LABELS[$L_INDEX]}"
DATASET="${DATASETS[$DATASET_INDEX]}"
METHOD="${METHODS[$METHOD_INDEX]}"
TAU="${TAUS[$TAU_INDEX]}"

read -r ENGINE BUILD_VARIANT <<< "$METHOD"

echo "========================================"
echo "SLURM_JOB_ID=$SLURM_JOB_ID"
echo "SLURM_ARRAY_JOB_ID=$SLURM_ARRAY_JOB_ID"
echo "SLURM_ARRAY_TASK_ID=$SLURM_ARRAY_TASK_ID"
echo "NODE=$(hostname)"
echo "DATASET_INDEX=$DATASET_INDEX"
echo "METHOD_INDEX=$METHOD_INDEX"
echo "TAU_INDEX=$TAU_INDEX"
echo "L=$L"
echo "DATASET=$DATASET"
echo "ENGINE=$ENGINE"
echo "BUILD_VARIANT=$BUILD_VARIANT"
echo "SIZES=$SIZES"
echo "TAU=$TAU"
echo "TAUS=${TAUS[*]}"
echo "========================================"

echo ">>> Running tau=$TAU"
bash "$BASE/run_multi_full.sh" "$DATASET" "$ENGINE" "$BUILD_VARIANT" "$L" "$SIZES" "$TAU"