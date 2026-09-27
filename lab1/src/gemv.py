# Индивидуальное задание 1, вариант 6: GEMV y = A @ x на NumPy и чистом Python.
# Число потоков OpenBLAS задаётся переменной окружения до запуска: OPENBLAS_NUM_THREADS=1 python3 gemv.py
import os
import time

import numpy as np

M, N = 2000, 4000
RUNS = 30

rng = np.random.default_rng(42)
A = (rng.random((M, N), dtype=np.float32) - 0.5)
x = (rng.random(N, dtype=np.float32) - 0.5)
ref = A.astype(np.float64) @ x.astype(np.float64)

flop = 2 * M * N
nbytes = 4 * (M * N + M + N)

def median_s(f, runs=RUNS):
    f()
    t = []
    for _ in range(runs):
        t0 = time.perf_counter()
        f()
        t.append(time.perf_counter() - t0)
    return sorted(t)[len(t) // 2]

threads = os.environ.get("OPENBLAS_NUM_THREADS", "по умолчанию")
print(f"GEMV {M}x{N} float32, потоков OpenBLAS: {threads}")

t = median_s(lambda: A @ x)
y = A @ x
print(f"NumPy A @ x:         {t * 1e3:8.3f} мс, {flop / t / 1e9:6.2f} GFLOPS, "
      f"{nbytes / t / 1e9:6.2f} ГБ/с, ошибка {np.max(np.abs(y - ref)):.1e}")

At = np.asfortranarray(A)  # то же значение, хранение по столбцам
t = median_s(lambda: At @ x)
print(f"NumPy (Fortran-порядок): {t * 1e3:8.3f} мс, {flop / t / 1e9:6.2f} GFLOPS")

if threads != "1":
    # Чистый Python: только по строкам, 200 строк из 2000 (иначе ~десятки секунд), результат экстраполируем
    rows = A[:200].tolist()
    xl = x.tolist()
    t0 = time.perf_counter()
    yl = [sum(a * b for a, b in zip(row, xl)) for row in rows]
    tp = (time.perf_counter() - t0) * (M / 200)
    print(f"Чистый Python (оценка на 2000 строк): {tp:8.3f} с, {flop / tp / 1e9:.4f} GFLOPS, "
          f"ошибка {max(abs(a - b) for a, b in zip(yl, ref[:200])):.1e}")
