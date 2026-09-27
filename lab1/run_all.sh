#!/usr/bin/env bash
# Лабораторная 1: сборка и запуск всех задач. Запуск: cd lab1 && bash run_all.sh
set -e
cd "$(dirname "$0")"
mkdir -p bin results
CXX="g++ -std=c++17 -Wall -Wextra"
RUN="taskset -c 2"
FLAGS=("-O0" "-O2" "-O3" "-O3 -march=native" "-O3 -march=native -ffast-math")

# Имя бинарника из набора флагов: "-O3 -march=native" -> O3_march_native
tag() { echo "$1" | sed 's/-//g; s/=/_/g; s/ /_/g'; }

# FP-инструкции внутри функции (скалярные ss / векторные ps по ширине регистра, FMA отдельно)
count_fp() {  # $1 = бинарник, $2 = имя функции
    objdump -d -C --no-show-raw-insn -M intel "$1" |
        awk -v fn="<$2(" '/^[0-9a-f]+ <.*>:$/ {if (done) exit; inside = index($0, fn) > 0; if (inside) done = 1} inside' |
        awk '$2 ~ /^v?(add|mul)ss$/ {s++}
             $2 ~ /^vfmadd[0-9]+ss$/ {fs++}
             $2 ~ /^v?(add|mul)ps$/ { if ($0 ~ /ymm/) y++; else x++ }
             $2 ~ /^vfmadd[0-9]+ps$/ {fy++}
             END {printf "скаляр add/mul: %d, FMA скаляр: %d, add/mul xmm: %d, add/mul ymm: %d, FMA вектор: %d\n", s, fs, x, y, fy}'
}

echo "=== 0. Паспорт машины ==="
lscpu | grep -E "Model name|^CPU\(s\)|Thread|Core\(s\)|Socket|L1d|L2|L3|NUMA node\(s\)|Flags" | sed 's/Flags:.*/Flags: (см. ниже)/'
echo "SIMD: $(grep -o -w -E 'sse4_2|avx|avx2|fma|avx512f' /proc/cpuinfo | sort -u | tr '\n' ' ')"
free -h | head -2
uname -r
g++ --version | head -1
python3 -c "import numpy, sys; print('Python', sys.version.split()[0], '| NumPy', numpy.__version__)"
echo "PMU: $(ls /sys/bus/event_source/devices | tr '\n' ' ')"

echo; echo "=== 1. Задача 1: пропускная способность памяти ==="
$CXX -O3 -march=native src/membw.cpp -o bin/membw
echo "--- C++, 8000x8000x3 float32"; $RUN ./bin/membw 8000
echo "--- C++, 500x500x3 float32 (помещается в L3)"; $RUN ./bin/membw 500
echo "--- Python/NumPy, 8000x8000x3"; $RUN python3 src/membw.py 8000
echo "--- Python/NumPy, 500x500x3"; $RUN python3 src/membw.py 500

echo; echo "=== 2. Задача 2: скалярное произведение с разными флагами ==="
: > results/dot_flags.csv
for f in "${FLAGS[@]}"; do
    t=$(tag "$f")
    $CXX $f src/dot.cpp -o bin/dot_$t
    out=$($RUN ./bin/dot_$t)
    ms=$(echo "$out" | awk -F': ' '/медиана/ {split($2, a, " "); print a[1]}')
    res=$(echo "$out" | awk '/Результат/ {print $4}')
    printf "%-32s %8s мс   результат %s\n" "$f" "$ms" "$res"
    echo "$f;$ms" >> results/dot_flags.csv
done
FMA_FLAGS="-O3 -march=native -ffast-math -mtune-ctrl=^avoid_fma256_chains"
$CXX $FMA_FLAGS src/dot.cpp -o bin/dot_fma
printf "%-32s %8s мс  (с принудительным vfmadd231ps ymm)\n" "... + ^avoid_fma256_chains" \
    "$($RUN ./bin/dot_fma | awk -F': ' '/медиана/ {split($2, a, " "); print a[1]}')"
echo "--- Python"
$RUN python3 src/dot.py

echo; echo "=== 3. Задача 3: ассемблер dot_product (локально, как в Godbolt) ==="
for f in "-O0" "-O3" "-O3 -march=native" "-O3 -march=native -ffast-math"; do
    t=$(tag "$f")
    g++ $f -S -masm=intel src/dot_asm.cpp -o results/dot_asm_$t.s
    g++ $f -c src/dot_asm.cpp -o bin/dot_asm_$t.o
    echo "--- $f"
    echo -n "  basic:    "; count_fp bin/dot_asm_$t.o dot_product_basic
    echo -n "  restrict: "; count_fp bin/dot_asm_$t.o dot_product_restrict
done
echo "--- Основной цикл при -O3 -march=native -ffast-math:"
objdump -d -C --no-show-raw-insn -M intel "bin/dot_asm_$(tag '-O3 -march=native -ffast-math').o" |
    awk '/<dot_product_basic/{f=1} /<dot_product_restrict/{f=0} f' | grep -E "vfmadd|vmovups|ymm" | head -8
echo "--- То же с -mtune-ctrl=^avoid_fma256_chains:"
g++ $FMA_FLAGS -c src/dot_asm.cpp -o bin/dot_asm_fma.o
g++ $FMA_FLAGS -S -masm=intel src/dot_asm.cpp -o results/dot_asm_fma.s
objdump -d -C --no-show-raw-insn -M intel bin/dot_asm_fma.o |
    awk '/<dot_product_basic/{f=1} /<dot_product_restrict/{f=0} f' | grep -E "ymm" | head -4

echo; echo "=== 4. Индивидуальное задание 1 (вариант 6): GEMV 2000x4000 float32 ==="
: > results/gemv_flags.csv
for f in "${FLAGS[@]}"; do
    t=$(tag "$f")
    $CXX $f -fopenmp-simd src/gemv.cpp -o bin/gemv_$t
    echo "--- $f -fopenmp-simd"
    $RUN ./bin/gemv_$t | tee results/gemv_$t.txt
    ms=$(awk '/^1\. по строкам/ {print $4}' results/gemv_$t.txt)
    echo "$f;$ms" >> results/gemv_flags.csv
done
echo "--- Что сделал -O3 с циклами (отчёт -fopt-info-loop):"
$CXX -O3 -march=native -fopenmp-simd -c src/gemv.cpp -o /dev/null -fopt-info-loop 2>&1 | grep -i "interchang" | sort -u || true
echo "--- FP-инструкции при -O3 -march=native (флаг варианта):"
for fn in gemv_rows gemv_rows_simd gemv_cols gemv_axpy_colmajor; do
    printf "  %-20s " "$fn:"; count_fp bin/gemv_O3_march_native $fn
done
echo "--- NumPy, все 4 vCPU"
python3 src/gemv.py
echo "--- NumPy, 1 поток на ядре 2"
OPENBLAS_NUM_THREADS=1 $RUN python3 src/gemv.py

python3 src/plot_flags.py results
