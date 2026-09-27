# Задача 1: замер пропускной способности памяти копированием массива (NumPy).
# Отличие от методички: массив сразу генерируется во float32 (rng.random(dtype=np.float32)).
# np.random.rand(...).astype(np.float32) на пике держит в памяти float64-копию (1.5 ГБ),
# а у WSL-машины всего ~3.8 ГБ RAM.
# Запуск: python3 membw.py [rows]   (по умолчанию 8000 -> 768 МБ)
import sys
import time

import numpy as np


def measure_memory_bandwidth_python(rows=8000):
    # Моделирование спутникового снимка: rows x rows пикселей, 3 канала, float32
    cols, channels = rows, 3
    size_bytes = rows * cols * channels * np.dtype('float32').itemsize

    print(f"Создание массива данных ({size_bytes / (1024**3):.3f} ГБ)...")
    rng = np.random.default_rng(0)
    src = rng.random((rows, cols, channels), dtype=np.float32)
    dst = np.empty_like(src)
    np.copyto(dst, src)  # прогрев: страницы dst выделяются при первой записи

    # Замер операции копирования (чтение + запись)
    iterations = 10 if rows >= 2000 else 1000
    print(f"Выполнение {iterations} итераций копирования...")

    start_time = time.perf_counter()
    for _ in range(iterations):
        np.copyto(dst, src)
    end_time = time.perf_counter()

    elapsed_time = end_time - start_time
    # Объём переданных данных за все итерации (чтение + запись)
    total_data_transferred = iterations * size_bytes * 2

    bandwidth_gbps = (total_data_transferred / (1024**3)) / elapsed_time

    print(f"Общее время: {elapsed_time:.4f} сек")
    print(f"Фактическая пропускная способность памяти: {bandwidth_gbps:.2f} ГиБ/с "
          f"({total_data_transferred / 1e9 / elapsed_time:.2f} ГБ/с)")


if __name__ == "__main__":
    measure_memory_bandwidth_python(int(sys.argv[1]) if len(sys.argv) > 1 else 8000)
