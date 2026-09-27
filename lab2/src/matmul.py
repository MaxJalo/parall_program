# Задача 1: умножение матриц на Python — фрагменты 1–3 методички.
# Запуск: python3 matmul.py 256 512 1024 2048
# Скалярный Python меряется только для N <= PY_MAX (по умолчанию 512): время растёт как N^3,
# и 1024 заняло бы ~10 минут, 2048 — больше часа. Для больших N выводится оценка N^3-экстраполяцией.
import os
import sys
import time

import numpy as np
from numba import njit, prange

PY_MAX = int(os.environ.get("PY_MAX", "512"))


# Фрагмент 1. Скалярное умножение, порядок циклов i -> k -> j
def matmul_scalar_python(A, B):
    M, K = A.shape
    K2, N = B.shape
    assert K == K2, "Incompatible matrix dimensions"
    C = np.zeros((M, N), dtype=np.float32)
    for i in range(M):
        for k in range(K):
            a = A[i, k]
            for j in range(N):
                C[i, j] += a * B[k, j]
    return C


# Фрагмент 3. Numba, параллельно по строкам
@njit(parallel=True)
def matmul_numba(A, B):
    M, K = A.shape
    N = B.shape[1]
    C = np.zeros((M, N), dtype=np.float32)
    for i in prange(M):
        for k in range(K):
            a = A[i, k]
            for j in range(N):
                C[i, j] += a * B[k, j]
    return C


# То же без распараллеливания — для честного сравнения с однопоточным C++
@njit
def matmul_numba_1t(A, B):
    M, K = A.shape
    N = B.shape[1]
    C = np.zeros((M, N), dtype=np.float32)
    for i in range(M):
        for k in range(K):
            a = A[i, k]
            for j in range(N):
                C[i, j] += a * B[k, j]
    return C


def median_s(f, runs):
    f()
    t = []
    for _ in range(runs):
        t0 = time.perf_counter()
        f()
        t.append(time.perf_counter() - t0)
    return sorted(t)[len(t) // 2]


def main():
    sizes = [int(s) for s in sys.argv[1:]] or [256, 512, 1024, 2048]
    print("N,py_scalar_s,py_measured,numpy_s,numba_par_s,numba_1t_s,max_err")
    py_ref = None  # (N, время) последнего реального замера — для экстраполяции
    for n in sizes:
        np.random.seed(42)
        A = np.random.rand(n, n).astype(np.float32)
        B = np.random.rand(n, n).astype(np.float32)
        runs = 10 if n <= 512 else (5 if n <= 1024 else 3)

        # Фрагмент 2. NumPy (BLAS)
        t_np = median_s(lambda: np.dot(A, B), runs)
        C_np = np.dot(A, B)

        t_nb = median_s(lambda: matmul_numba(A, B), runs)
        t_nb1 = median_s(lambda: matmul_numba_1t(A, B), runs)
        err = float(np.max(np.abs(matmul_numba(A, B) - C_np)))

        if n <= PY_MAX:
            t0 = time.perf_counter()
            C_py = matmul_scalar_python(A, B)
            t_py = time.perf_counter() - t0
            err = max(err, float(np.max(np.abs(C_py - C_np))))
            py_ref = (n, t_py)
            measured = 1
        else:
            t_py = py_ref[1] * (n / py_ref[0]) ** 3
            measured = 0
        print(f"{n},{t_py:.6f},{measured},{t_np:.6f},{t_nb:.6f},{t_nb1:.6f},{err:.1e}", flush=True)


if __name__ == "__main__":
    main()
