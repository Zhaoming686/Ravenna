#!/bin/bash
#SBATCH -p gpu
#SBATCH --job-name=cpu_probe
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=1G
#SBATCH --time=00:05:00
#SBATCH --output=/home/cbr406/Projects/Mix-Match/SubgraphMatchingSurvey-main/vlabel/outputs/logs/%x-%j.out
#SBATCH --error=/home/cbr406/Projects/Mix-Match/SubgraphMatchingSurvey-main/vlabel/outputs/logs/%x-%j.err

set -euo pipefail

BASE="/home/cbr406/Projects/Mix-Match/SubgraphMatchingSurvey-main/vlabel"
LOG_DIR="$BASE/outputs/logs"
OUT_DIR="$BASE/outputs/cpu_probe"

mkdir -p "$LOG_DIR" "$OUT_DIR"

RESULT_TSV="${RESULT_TSV:-$OUT_DIR/cpu_config.tsv}"

HOST="$(hostname)"
DATE_NOW="$(date '+%F %T')"

get_lscpu_field() {
    local key="$1"
    lscpu | awk -F: -v key="$key" '
        $1 ~ key {
            gsub(/^[ \t]+/, "", $2)
            print $2
            exit
        }
    '
}

has_flag() {
    local flag="$1"
    if echo "$FLAGS" | grep -qw "$flag"; then
        echo "YES"
    else
        echo "NO"
    fi
}

VENDOR="$(get_lscpu_field '^Vendor ID')"
MODEL_NAME="$(get_lscpu_field '^Model name')"
ARCH="$(get_lscpu_field '^Architecture')"

SOCKETS="$(get_lscpu_field 'Socket\(s\)')"
CORES_PER_SOCKET="$(get_lscpu_field 'Core\(s\) per socket')"
THREADS_PER_CORE="$(get_lscpu_field 'Thread\(s\) per core')"
LOGICAL_CPUS="$(get_lscpu_field '^CPU\(s\)')"
NUMA_NODES="$(get_lscpu_field 'NUMA node\(s\)')"

CPU_MAX_MHZ="$(get_lscpu_field 'CPU max MHz')"
CPU_MIN_MHZ="$(get_lscpu_field 'CPU min MHz')"

L1D="$(get_lscpu_field '^L1d cache')"
L1I="$(get_lscpu_field '^L1i cache')"
L2="$(get_lscpu_field '^L2 cache')"
L3="$(get_lscpu_field '^L3 cache')"

MEM_MB="$(awk '/MemTotal/ {printf "%d", $2/1024}' /proc/meminfo)"

FLAGS="$(lscpu | awk -F: '/Flags/ {print $2; exit}')"

AVX="$(has_flag avx)"
AVX2="$(has_flag avx2)"
AVX512F="$(has_flag avx512f)"
AVX512BW="$(has_flag avx512bw)"
AVX512DQ="$(has_flag avx512dq)"
AVX512CD="$(has_flag avx512cd)"
AVX512VL="$(has_flag avx512vl)"
AVX512VNNI="$(has_flag avx512_vnni)"
AVX512BF16="$(has_flag avx512_bf16)"
AVX512FP16="$(has_flag avx512_fp16)"

AMX_TILE="$(has_flag amx_tile)"
AMX_INT8="$(has_flag amx_int8)"
AMX_BF16="$(has_flag amx_bf16)"

DETAIL_LOG="$OUT_DIR/${HOST}_${SLURM_JOB_ID}.cpuinfo.txt"

{
    echo "HOST=$HOST"
    echo "DATE=$DATE_NOW"
    echo "SLURM_JOB_ID=${SLURM_JOB_ID:-N/A}"
    echo "SLURM_JOB_NODELIST=${SLURM_JOB_NODELIST:-N/A}"
    echo

    echo "---- lscpu ----"
    lscpu || true
    echo

    echo "---- memory ----"
    free -h || true
    echo

    echo "---- selected flags ----"
    echo "AVX=$AVX"
    echo "AVX2=$AVX2"
    echo "AVX512F=$AVX512F"
    echo "AVX512BW=$AVX512BW"
    echo "AVX512DQ=$AVX512DQ"
    echo "AVX512CD=$AVX512CD"
    echo "AVX512VL=$AVX512VL"
    echo "AVX512VNNI=$AVX512VNNI"
    echo "AVX512BF16=$AVX512BF16"
    echo "AVX512FP16=$AVX512FP16"
    echo "AMX_TILE=$AMX_TILE"
    echo "AMX_INT8=$AMX_INT8"
    echo "AMX_BF16=$AMX_BF16"
} > "$DETAIL_LOG" 2>&1

LOCK_FILE="$RESULT_TSV.lock"

exec 200>"$LOCK_FILE"
flock 200

if [[ ! -s "$RESULT_TSV" ]]; then
    echo -e "host\tdate\tjob_id\tvendor\tmodel_name\tarch\tsockets\tcores_per_socket\tthreads_per_core\tlogical_cpus\tnuma_nodes\tmem_mb\tcpu_min_mhz\tcpu_max_mhz\tl1d\tl1i\tl2\tl3\tavx\tavx2\tavx512f\tavx512bw\tavx512dq\tavx512cd\tavx512vl\tavx512_vnni\tavx512_bf16\tavx512_fp16\tamx_tile\tamx_int8\tamx_bf16\tdetail_log" > "$RESULT_TSV"
fi

echo -e "${HOST}\t${DATE_NOW}\t${SLURM_JOB_ID:-N/A}\t${VENDOR}\t${MODEL_NAME}\t${ARCH}\t${SOCKETS}\t${CORES_PER_SOCKET}\t${THREADS_PER_CORE}\t${LOGICAL_CPUS}\t${NUMA_NODES}\t${MEM_MB}\t${CPU_MIN_MHZ}\t${CPU_MAX_MHZ}\t${L1D}\t${L1I}\t${L2}\t${L3}\t${AVX}\t${AVX2}\t${AVX512F}\t${AVX512BW}\t${AVX512DQ}\t${AVX512CD}\t${AVX512VL}\t${AVX512VNNI}\t${AVX512BF16}\t${AVX512FP16}\t${AMX_TILE}\t${AMX_INT8}\t${AMX_BF16}\t${DETAIL_LOG}" >> "$RESULT_TSV"

flock -u 200

echo "Wrote summary row to: $RESULT_TSV"
echo "Wrote detail log to: $DETAIL_LOG"