# Задача 3: блочная обработка на Python (фрагмент 15) и проверка размещения массивов NumPy.
import sys
import time

import numpy as np
from scipy.signal import convolve2d


# Фрагмент 15. Блочная обработка
def convolve2d_blocked(image, kernel, block_size=256):
    H, W = image.shape
    kh, kw = kernel.shape
    ph, pw = kh // 2, kw // 2
    result = np.zeros_like(image, dtype=np.float32)
    for i0 in range(0, H, block_size):
        for j0 in range(0, W, block_size):
            i1 = min(i0 + block_size, H)
            j1 = min(j0 + block_size, W)
            # Расширяем блок на границы ядра
            ie0 = max(0, i0 - ph)
            ie1 = min(H, i1 + ph)
            je0 = max(0, j0 - pw)
            je1 = min(W, j1 + pw)
            block = image[ie0:ie1, je0:je1]
            conv = convolve2d(block, kernel, mode='same')
            # Вырезаем центральную часть
            offset_i = i0 - ie0
            offset_j = j0 - je0
            result[i0:i1, j0:j1] = conv[offset_i:offset_i + (i1 - i0), offset_j:offset_j + (j1 - j0)]
    return result


def median_s(f, runs=5):
    f()
    t = []
    for _ in range(runs):
        t0 = time.perf_counter()
        f()
        t.append(time.perf_counter() - t0)
    return sorted(t)[len(t) // 2]


n = int(sys.argv[1]) if len(sys.argv) > 1 else 4096
kernel = np.array([[1, 2, 1], [2, 4, 2], [1, 2, 1]], dtype=np.float32) / 16.0
image = np.zeros((n, n), dtype=np.float32, order='C')
image[:] = np.random.default_rng(42).random((n, n), dtype=np.float32)

print(f"Изображение {n}x{n}: C-непрерывный = {image.flags['C_CONTIGUOUS']}, "
      f"адрес % 64 = {image.ctypes.data % 64}, адрес % 32 = {image.ctypes.data % 32}")
t_full = median_s(lambda: convolve2d(image, kernel, mode='same'))
t_blk = median_s(lambda: convolve2d_blocked(image, kernel, 256))
err = np.max(np.abs(convolve2d_blocked(image, kernel, 256) - convolve2d(image, kernel, mode='same')))
print(f"SciPy целиком:      {t_full * 1e3:8.1f} мс")
print(f"SciPy блоками 256:  {t_blk * 1e3:8.1f} мс  ({t_full / t_blk:.2f}x), ошибка {err:.1e}")
