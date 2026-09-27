#!/usr/bin/env bash
# Сборка и запуск всех примеров лекции 1. Запуск: bash run_all.sh
set -euo pipefail
cd "$(dirname "$0")"
mkdir -p bin results

CXX="g++ -std=c++17 -Wall -Wextra"

echo "=== 00. Процессор и кэши ==="
lscpu | grep -E "Model name|^CPU\(s\)|Thread|Core|Socket|L1d|L2|L3|NUMA"
lscpu | grep -o -w -E "sse4_2|avx|avx2|fma|avx512f" | sort -u | tr '\n' ' '; echo
for i in 0 1 2 3; do
  d=/sys/devices/system/cpu/cpu0/cache/index$i
  echo "L$(cat $d/level) $(cat $d/type): $(cat $d/size), $(cat $d/ways_of_associativity)-way, линия $(cat $d/coherency_line_size) Б"
done

echo; echo "=== 01. Амдал и Густафсон ==="
$CXX -O2 code/01_amdahl.cpp -o bin/01_amdahl && ./bin/01_amdahl
python3 code/02_amdahl_plot.py

echo; echo "=== 03. Проход по размерам (иерархия памяти) ==="
$CXX -O3 -march=native -ffast-math code/03_cache_sweep.cpp -o bin/03_cache_sweep
taskset -c 2 ./bin/03_cache_sweep | tee results/cache_sweep.csv
python3 code/03_cache_plot.py

echo; echo "=== 04. Шаг доступа (stride) ==="
$CXX -O2 code/04_stride.cpp -o bin/04_stride && taskset -c 2 ./bin/04_stride

echo; echo "=== 05. Ассоциативность, конфликтные промахи ==="
$CXX -O2 code/05_assoc.cpp -o bin/05_assoc && taskset -c 2 ./bin/05_assoc

echo; echo "=== 06. False sharing ==="
$CXX -O2 -pthread code/06_false_sharing.cpp -o bin/06_false_sharing && ./bin/06_false_sharing

echo; echo "=== 07. Привязка потоков ==="
$CXX -O2 -pthread code/07_affinity.cpp -o bin/07_affinity && ./bin/07_affinity
echo "NUMA-узлы:"; ls -d /sys/devices/system/node/node* 2>/dev/null || echo "нет данных"

echo; echo "=== 08. Roofline и скалярное произведение ==="
$CXX -O3 -march=native -fopenmp-simd code/08_roofline_dot.cpp -o bin/08_roofline_dot
taskset -c 2 ./bin/08_roofline_dot | tee results/roofline_dot.txt
measured=$(awk '$1=="100000000"{print $5}' results/roofline_dot.txt)
python3 code/09_roofline_plot.py "$measured"
