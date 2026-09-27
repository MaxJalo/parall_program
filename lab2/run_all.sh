#!/usr/bin/env bash
# Лабораторная 2: сборка и запуск всех задач. Запуск: cd lab2 && bash run_all.sh
# C++ меряется на одном ядре (taskset -c 2). Python — без привязки: NumPy/Numba parallel
# используют все 4 vCPU, как предполагает методичка; отдельно меряется Numba в 1 поток.
set -e
cd "$(dirname "$0")"
mkdir -p bin results
CXX="g++ -std=c++17 -Wall -Wextra -O3 -march=native"
RUN="taskset -c 2"

count_fp() {  # $1 = бинарник, $2 = имя функции: скалярные / xmm / ymm FP-инструкции
    objdump -d -C --no-show-raw-insn -M intel "$1" |
        awk -v fn="<$2(" '/^[0-9a-f]+ <.*>:$/ {if (done) exit; inside = index($0, fn) > 0; if (inside) done = 1} inside' |
        awk '$2 ~ /^v?(add|mul|fmadd[0-9]+)ss$/ {s++}
             $2 ~ /^v?(add|mul|fmadd[0-9]+)ps$/ { if ($0 ~ /ymm/) y++; else x++ }
             END {printf "скаляр: %d, xmm: %d, ymm: %d\n", s, x, y}'
}

echo "=== 0. Окружение ==="
lscpu | grep -E "Model name|^CPU\(s\)|L1d|L2|L3"
g++ --version | head -1
python3 -c "import numpy, scipy, numba; print('NumPy', numpy.__version__, '| SciPy', scipy.__version__, '| Numba', numba.__version__)"

echo; echo "=== 1. Задача 1: умножение матриц ==="
$CXX -fopenmp-simd src/matmul.cpp -o bin/matmul -fopt-info-vec-optimized 2>&1 |
    grep -E "matmul.cpp:[0-9]+:.*(vectorized|versioned)" | sort -V -u | tee results/matmul_vec_report.txt
echo "--- C++ (1 ядро)"
$RUN ./bin/matmul 256 512 1024 2048 | tee results/matmul_cpp.csv
echo "--- Python (скалярный Python меряется до N=512, дальше оценка ~N^3)"
PY_MAX=512 python3 src/matmul.py 256 512 1024 2048 | tee results/matmul_py.csv
echo "--- FP-инструкции:"
for fn in matmul_scalar matmul_auto matmul_avx2 matmul_blocked; do
    printf "  %-16s " "$fn:"; count_fp bin/matmul $fn
done

echo; echo "=== 2. Задача 2: свёртка, ядро Гаусса 3x3 ==="
$CXX src/conv.cpp -o bin/conv -fopt-info-vec-optimized 2>&1 |
    grep -E "conv.cpp:[0-9]+:.*vectorized" | sort -V -u | tee results/conv_vec_report.txt
echo "--- C++ (1 ядро)"
$RUN ./bin/conv gauss3 512 1024 2048 4096 | tee results/conv_gauss3_cpp.csv
echo "--- Python (скалярный Python меряется до N=2048, дальше оценка ~N^2)"
PY_MAX=2048 python3 src/conv.py gauss3 512 1024 2048 4096 | tee results/conv_gauss3_py.csv
echo "--- FP-инструкции:"
for fn in convolve2d_scalar convolve2d_auto convolve2d_auto_nobranch convolve2d_avx2 convolve1d_horizontal convolve1d_vertical convolve2d_separable_vec; do
    printf "  %-26s " "$fn:"; count_fp bin/conv $fn
done

echo; echo "=== 3. Индивидуальное задание (вариант 6): свёртка, ядро усреднения 7x7 ==="
echo "--- C++ (1 ядро)"
$RUN ./bin/conv box7 512 1024 2048 4096 | tee results/conv_box7_cpp.csv
echo "--- Python (скалярный Python меряется до N=1024, дальше оценка ~N^2)"
PY_MAX=1024 python3 src/conv.py box7 512 1024 2048 4096 | tee results/conv_box7_py.csv

echo; echo "=== 4. Задача 3: блочная обработка и выравнивание ==="
$CXX src/blocking.cpp -o bin/blocking
$RUN ./bin/blocking all 4096 | tee results/blocking_4096.txt
$RUN ./bin/blocking all 512 | tee results/blocking_512.txt
echo "--- Python, фрагмент 15"
python3 src/blocking.py 4096 | tee results/blocking_py.txt
echo "--- Cachegrind (промахи кэша; perf в WSL2 недоступен)"
if command -v valgrind > /dev/null; then
    : > results/cachegrind.txt
    for v in frag17_blocked_vector frag16_blocked_aligned avx2_unaligned avx2_aligned avx2_blocked_unaligned avx2_blocked_aligned; do
        valgrind --tool=cachegrind --cache-sim=yes --cachegrind-out-file=/dev/null \
            ./bin/blocking one $v 2048 2>&1 | grep -E "done|D1  misses|LLd misses|D1  miss rate|LLd miss rate" |
            sed "s/^==[0-9]*== /$v: /" | tee -a results/cachegrind.txt
    done
else
    echo "valgrind не установлен: sudo apt install valgrind, затем перезапустить этот скрипт"
fi

echo; echo "=== 5. Графики ==="
python3 src/plot.py results
