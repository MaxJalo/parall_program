"""Roofline-график для i5-12450H: крыша + алгоритмы из таблицы лекции (п. 5.5).

Аргумент (необязательный): измеренные GFLOPS скалярного произведения на 100M элементов,
чтобы поставить на график реальную точку.
"""
import sys
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

PI = 140.8     # GFLOPS, одно P-ядро, AVX2 FMA, float
BETA = 38.4    # ГБ/с, 1 x DDR5-4800
ridge = PI / BETA

I = np.logspace(-2, 3, 400)
P = np.minimum(PI, BETA * I)

fig, ax = plt.subplots(figsize=(9, 5))
ax.loglog(I, P, lw=2, label=f"крыша: min({PI}, {BETA}*I)")
ax.axvline(ridge, color="gray", ls="--")
ax.text(ridge * 1.1, 2, f"I* = {ridge:.2f} FLOP/байт")

algos = {"сложение векторов": 0.125, "скалярное произв.": 0.25, "БПФ": 5, "блочный GEMM": 200}
for name, ai in algos.items():
    ax.plot(ai, min(PI, BETA * ai), "o")
    ax.annotate(name, (ai, min(PI, BETA * ai)), textcoords="offset points", xytext=(5, -12))

if len(sys.argv) > 1:
    measured = float(sys.argv[1])
    ax.plot(0.25, measured, "r*", ms=14, label=f"dot, измерено: {measured:.2f} GFLOPS")

ax.set_xlabel("Арифметическая интенсивность I, FLOP/байт")
ax.set_ylabel("Производительность, GFLOPS")
ax.set_title("Roofline: Intel Core i5-12450H")
ax.grid(True, which="both", alpha=0.3)
ax.legend(loc="lower right")
out = Path(__file__).resolve().parent.parent / "results" / "fig_roofline.png"
fig.tight_layout()
fig.savefig(out, dpi=120)
print(f"Сохранено: {out}")
