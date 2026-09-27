#!/usr/bin/env bash
# Сборка и запуск всех примеров к лекции 2. Запуск: cd lec2 && bash run_all.sh
set -e
cd "$(dirname "$0")"
mkdir -p bin results
CXX="g++ -std=c++20 -Wall -Wextra"
NATIVE="-O3 -march=native"
RUN="taskset -c 2"

# Считает арифметические FP-инструкции в дизассемблере функции: скалярные (ss/sd) и векторные (ps/pd, xmm/ymm)
count_fp() {  # $1 = бинарник, $2 = подстрока имени функции
    objdump -d -C --no-show-raw-insn -M intel "$1" |
        awk -v fn="<$2(" '/^[0-9a-f]+ <.*>:$/ {if (done) exit; inside = index($0, fn) > 0; if (inside) done = 1} inside' |
        awk '$2 ~ /^v?(add|sub|mul|div|fmadd[0-9]*|fnmadd[0-9]*)(ss|sd)$/ {s++}
             $2 ~ /^v?(add|sub|mul|div|fmadd[0-9]*|fnmadd[0-9]*)(ps|pd)$/ { if ($0 ~ /ymm/) y++; else x++ }
             END {printf "скалярных: %d, xmm (128 бит): %d, ymm (256 бит): %d\n", s, x, y}'
}

echo "=== 00. Что умеет процессор и что включает -march=native ==="
lscpu | grep "Model name"
echo "SIMD-флаги CPU: $(grep -o -w -E 'sse4_2|avx|avx2|fma|avx512f' /proc/cpuinfo | sort -u | tr '\n' ' ')"
g++ -march=native -Q --help=target | grep -E -- "-march=|-mavx2 |-mfma |-mavx512f |-mprefer-vector-width"
echo "Счётчики PMU (fp_arith_inst_retired): $(ls /sys/bus/event_source/devices | tr '\n' ' ') — аппаратного cpu нет, perf недоступен"

echo; echo "=== 01. Ширина SIMD-регистров и Амдал для SIMD ==="
$CXX -O2 code/01_simd_width.cpp -o bin/01_simd_width
./bin/01_simd_width

echo; echo "=== 02. Авто-векторизация: примеры 1–3 ==="
$CXX $NATIVE code/02_autovec.cpp -o bin/02_vec -fopt-info-vec-all 2> results/02_report_full.txt
grep -E "02_autovec.cpp:[0-9]+:" results/02_report_full.txt |
    grep -E "optimized: loop|couldn't vectorize|not vectorized:" |
    awk -F: '$2 < 50' | sort -V -u | tee results/02_report.txt
$CXX $NATIVE -fno-tree-vectorize code/02_autovec.cpp -o bin/02_novec
echo "--- отчёт с -fno-trapping-math (примеры 3, 3б, 3в):"
$CXX $NATIVE -fno-trapping-math code/02_autovec.cpp -o bin/02_notrap -fopt-info-vec 2>&1 |
    grep -E "02_autovec.cpp:(27|37|47):.*32 byte" | sort -V -u || true
echo "--- -O3 -march=native"; $RUN ./bin/02_vec
echo "--- -O3 -march=native -fno-tree-vectorize"; $RUN ./bin/02_novec
echo "--- -O3 -march=native -fno-trapping-math"; $RUN ./bin/02_notrap
echo "--- FP-инструкции в vectorizable_example:"
echo -n "  с векторизацией:  "; count_fp bin/02_vec vectorizable_example
echo -n "  без векторизации: "; count_fp bin/02_novec vectorizable_example

echo; echo "=== 03. Прагмы и omp declare simd ==="
$CXX $NATIVE -fopenmp-simd code/03_pragmas.cpp -o bin/03_pragmas -fopt-info-vec 2>&1 |
    grep -E "03_pragmas.cpp:[0-9]+:" | sort -u
$CXX $NATIVE -fopenmp-simd -ffast-math code/03_pragmas.cpp -o bin/03_pragmas_fm
echo "--- без -ffast-math"; $RUN ./bin/03_pragmas
echo "--- с -ffast-math"; $RUN ./bin/03_pragmas_fm
echo "Векторные клоны sin/transform в бинарнике: $(objdump -d bin/03_pragmas_fm | grep -o '_ZGV[A-Za-z0-9_]*' | sort -u | tr '\n' ' ')"

echo; echo "=== 04. Intrinsics: add_avx2, fma_avx2 ==="
$CXX $NATIVE code/04_intrinsics.cpp -o bin/04_intrinsics
$RUN ./bin/04_intrinsics

echo; echo "=== 05. std::experimental::simd ==="
$CXX $NATIVE code/05_std_simd.cpp -o bin/05_std_simd
$RUN ./bin/05_std_simd

echo; echo "=== 06. Выравнивание ==="
$CXX $NATIVE code/06_align.cpp -o bin/06_align
$RUN ./bin/06_align

echo; echo "=== 07. Свёртка 4096x4096, ядро 3x3 ==="
$CXX $NATIVE code/07_conv2d.cpp -o bin/07_conv2d
echo "--- -O3 -march=native"; $RUN ./bin/07_conv2d
$CXX -O0 -mavx2 -mfma code/07_conv2d.cpp -o bin/07_conv2d_O0
echo "--- -O0 (как «без оптимизации» в лекции)"; $RUN ./bin/07_conv2d_O0

echo; echo "=== 08. Микро-ядро умножения матриц 6x8 (FP64) ==="
$CXX $NATIVE code/08_microkernel.cpp -o bin/08_microkernel
$RUN ./bin/08_microkernel

echo; echo "=== 09. AVX-512 маски (только ассемблер, на этом CPU не запустить) ==="
$CXX -O2 -mavx512f -S -masm=intel code/09_avx512_masks.cpp -o results/09_avx512_masks.s
grep -E "kmov|\{k[0-7]\}|zmm" results/09_avx512_masks.s | sort | uniq -c | sort -rn | head -8
