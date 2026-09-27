# Задача 2 и вариант 6: двумерная свёртка на Python — фрагменты 8–10 методички.
# Запуск: python3 conv.py gauss3 512 1024 2048 4096   или   python3 conv.py box7 ...
# Скалярный Python меряется только для N <= PY_MAX (по умолчанию 512), дальше — оценка
# экстраполяцией по N^2 (4096x4096 с ядром 7x7 заняло бы больше часа).
import os
import sys
import time

import numpy as np
from numba import njit, prange
from scipy.signal import convolve2d

PY_MAX = int(os.environ.get("PY_MAX", "512"))


# Фрагмент 8. Скалярная свёртка
def convolve2d_scalar(image, kernel):
    H, W = image.shape
    kh, kw = kernel.shape
    ph, pw = kh // 2, kw // 2
    result = np.zeros_like(image, dtype=np.float32)
    for i in range(H):
        for j in range(W):
            s = 0.0
            for ki in range(kh):
                for kj in range(kw):
                    ii = i + ki - ph
                    jj = j + kj - pw
                    if 0 <= ii < H and 0 <= jj < W:
                        s += image[ii, jj] * kernel[ki, kj]
            result[i, j] = s
    return result


# Фрагмент 10. Numba, параллельно по строкам
@njit(parallel=True)
def convolve2d_numba(image, kernel):
    H, W = image.shape
    kh, kw = kernel.shape
    ph, pw = kh // 2, kw // 2
    result = np.zeros_like(image)
    for i in prange(H):
        for j in range(W):
            s = 0.0
            for ki in range(kh):
                for kj in range(kw):
                    ii = i + ki - ph
                    jj = j + kj - pw
                    if 0 <= ii < H and 0 <= jj < W:
                        s += image[ii, jj] * kernel[ki, kj]
            result[i, j] = s
    return result


@njit
def convolve2d_numba_1t(image, kernel):
    H, W = image.shape
    kh, kw = kernel.shape
    ph, pw = kh // 2, kw // 2
    result = np.zeros_like(image)
    for i in range(H):
        for j in range(W):
            s = 0.0
            for ki in range(kh):
                for kj in range(kw):
                    ii = i + ki - ph
                    jj = j + kj - pw
                    if 0 <= ii < H and 0 <= jj < W:
                        s += image[ii, jj] * kernel[ki, kj]
            result[i, j] = s
    return result


def make_kernel(name):
    if name == "box7":
        return np.full((7, 7), 1.0 / 49.0, dtype=np.float32)
    return np.array([[1, 2, 1], [2, 4, 2], [1, 2, 1]], dtype=np.float32) / 16.0


def median_s(f, runs):
    f()
    t = []
    for _ in range(runs):
        t0 = time.perf_counter()
        f()
        t.append(time.perf_counter() - t0)
    return sorted(t)[len(t) // 2]


def main():
    kname = sys.argv[1] if len(sys.argv) > 1 else "gauss3"
    sizes = [int(s) for s in sys.argv[2:]] or [512, 1024, 2048, 4096]
    kernel = make_kernel(kname)
    print("N,py_scalar_s,py_measured,scipy_s,numba_par_s,numba_1t_s,max_err")
    py_ref = None
    for n in sizes:
        np.random.seed(42)
        image = np.random.rand(n, n).astype(np.float32)
        runs = 10 if n <= 512 else (5 if n <= 1024 else 3)

        # Фрагмент 9. SciPy (ядро симметричное, поэтому свёртка = кросс-корреляция)
        t_sp = median_s(lambda: convolve2d(image, kernel, mode='same'), runs)
        ref = convolve2d(image, kernel, mode='same')

        t_nb = median_s(lambda: convolve2d_numba(image, kernel), runs)
        t_nb1 = median_s(lambda: convolve2d_numba_1t(image, kernel), runs)
        err = float(np.max(np.abs(convolve2d_numba(image, kernel) - ref)))

        if n <= PY_MAX:
            t0 = time.perf_counter()
            res = convolve2d_scalar(image, kernel)
            t_py = time.perf_counter() - t0
            err = max(err, float(np.max(np.abs(res - ref))))
            py_ref = (n, t_py)
            measured = 1
        else:
            t_py = py_ref[1] * (n / py_ref[0]) ** 2
            measured = 0
        print(f"{n},{t_py:.6f},{measured},{t_sp:.6f},{t_nb:.6f},{t_nb1:.6f},{err:.1e}", flush=True)


if __name__ == "__main__":
    main()
