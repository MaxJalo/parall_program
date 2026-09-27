# Задача 2: скалярное произведение на Python — нативный цикл и numpy.dot.
# Отличия от методички: numpy.dot меряется медианой из 21 запуска (один вызов ~2 мс — шум),
# добавлен вариант с обычными списками Python (без накладных расходов доступа к ndarray).
import time

import numpy as np


def benchmark_dot_product_python():
    n = 10_000_000
    rng = np.random.default_rng(0)
    a = rng.random(n, dtype=np.float32)
    b = rng.random(n, dtype=np.float32)

    print(f"Размерность векторов: {n}")

    # 1. Нативный цикл Python по ndarray (эталон медленной работы)
    print("Замер нативного цикла Python (может занять время)...")
    start = time.perf_counter()
    res_native = 0.0
    for i in range(n):
        res_native += a[i] * b[i]
    end = time.perf_counter()
    t_native = end - start
    print(f"Нативный цикл Python (ndarray): {t_native:.4f} сек")

    # 1б. Тот же цикл по спискам Python
    al, bl = a.tolist(), b.tolist()
    start = time.perf_counter()
    res_list = 0.0
    for x, y in zip(al, bl):
        res_list += x * y
    t_list = time.perf_counter() - start
    print(f"Нативный цикл Python (list, zip): {t_list:.4f} сек")

    # 2. Векторизованная операция NumPy
    print("Замер numpy.dot...")
    times = []
    for _ in range(21):
        start = time.perf_counter()
        res_numpy = np.dot(a, b)
        times.append(time.perf_counter() - start)
    t_numpy = sorted(times)[len(times) // 2]
    print(f"NumPy dot: {t_numpy:.6f} сек (медиана из 21), {t_numpy * 1e3:.3f} мс")

    exact = np.dot(a.astype(np.float64), b.astype(np.float64))
    print(f"Результаты совпадают: {np.isclose(res_native, res_numpy, rtol=1e-4)}")
    print(f"  цикл по ndarray: {res_native:.2f} (тип {type(res_native).__name__}), "
          f"отн. ошибка {abs(res_native - exact) / exact:.1e}")
    print(f"  цикл по list:    {res_list:.2f} (тип {type(res_list).__name__}), "
          f"отн. ошибка {abs(res_list - exact) / exact:.1e}")
    print(f"  numpy.dot:       {res_numpy:.2f} (тип {type(res_numpy).__name__}), "
          f"отн. ошибка {abs(res_numpy - exact) / exact:.1e}")
    print(f"  точно (float64): {exact:.2f}")
    print(f"Ускорение numpy.dot относительно цикла: {t_native / t_numpy:.0f}x")


if __name__ == "__main__":
    benchmark_dot_product_python()
