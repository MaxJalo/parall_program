# Графики время(размер) для лабораторной 2: читает CSV из results/, пишет PNG туда же.
import csv
import sys
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

res = Path(sys.argv[1] if len(sys.argv) > 1 else "results")


def read(name):
    with open(res / name, encoding="utf-8") as f:
        return list(csv.DictReader(f))


def plot(cpp_csv, py_csv, cpp_cols, py_cols, title, out, xlabel):
    cpp, py = read(cpp_csv), read(py_csv)
    fig, ax = plt.subplots(figsize=(9, 5.5))
    for col, label in py_cols:
        xs = [int(r["N"]) for r in py]
        ys = [float(r[col]) for r in py]
        if col == "py_scalar_s":
            meas = [r["py_measured"] == "1" for r in py]
            ax.plot([x for x, m in zip(xs, meas) if m], [y for y, m in zip(ys, meas) if m], "o-", label=label)
            ax.plot([x for x, m in zip(xs, meas) if not m], [y for y, m in zip(ys, meas) if not m],
                    "o--", color=ax.lines[-1].get_color(), alpha=0.5, label=label + " (оценка)")
        else:
            ax.plot(xs, ys, "s-", label=label)
    for col, label in cpp_cols:
        ax.plot([int(r["N"]) for r in cpp], [float(r[col]) for r in cpp], "^-", label=label)
    ax.set_xscale("log", base=2)
    ax.set_yscale("log")
    ax.set_xlabel(xlabel)
    ax.set_ylabel("Время, с (медиана)")
    ax.set_title(title)
    ax.grid(True, which="both", alpha=0.3)
    ax.legend(fontsize=8, ncol=2)
    fig.tight_layout()
    fig.savefig(res / out, dpi=110)
    print(f"Сохранено: {res / out}")


py_mm = [("py_scalar_s", "Python scalar"), ("numpy_s", "NumPy"), ("numba_par_s", "Numba parallel"),
         ("numba_1t_s", "Numba 1 поток")]
cpp_mm = [("scalar_s", "C++ scalar"), ("auto_s", "C++ auto"), ("avx2_s", "C++ AVX2"), ("blocked_s", "C++ block 64")]
plot("matmul_cpp.csv", "matmul_py.csv", cpp_mm, py_mm, "Задача 1: умножение матриц N×N (float32)",
     "fig_matmul.png", "N")

py_cv = [("py_scalar_s", "Python scalar"), ("scipy_s", "SciPy"), ("numba_par_s", "Numba parallel"),
         ("numba_1t_s", "Numba 1 поток")]
cpp_cv = [("scalar_s", "C++ scalar"), ("auto_s", "C++ auto (фр. 12)"), ("auto_nobranch_s", "C++ auto без if"),
          ("avx2_s", "C++ AVX2"), ("separable_s", "C++ separ. (фр. 14)"), ("separable_vec_s", "C++ separ. вект.")]
plot("conv_gauss3_cpp.csv", "conv_gauss3_py.csv", cpp_cv, py_cv, "Задача 2: свёртка N×N, ядро Гаусса 3×3",
     "fig_conv_gauss3.png", "N (изображение N×N)")
plot("conv_box7_cpp.csv", "conv_box7_py.csv", cpp_cv, py_cv, "Вариант 6: свёртка N×N, ядро усреднения 7×7",
     "fig_conv_box7.png", "N (изображение N×N)")
