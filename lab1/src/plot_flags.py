# График к Задаче 2 и индивидуальному заданию: время от флагов компиляции.
# Читает results/dot_flags.csv и results/gemv_flags.csv (флаги;мс), пишет results/fig_dot_flags.png.
import csv
import sys
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

res = Path(sys.argv[1] if len(sys.argv) > 1 else "results")


def read(name):
    with open(res / name, encoding="utf-8") as f:
        rows = list(csv.reader(f, delimiter=";"))
    return [r[0] for r in rows], [float(r[1]) for r in rows]


fig, axes = plt.subplots(1, 2, figsize=(13, 4.5))
for ax, (name, title) in zip(axes, [("dot_flags.csv", "Задача 2: dot, N = 10⁷ float"),
                                    ("gemv_flags.csv", "Вариант 6: GEMV 2000×4000, по строкам")]):
    labels, ms = read(name)
    bars = ax.barh(labels, ms, color="tab:blue")
    ax.invert_yaxis()
    ax.set_xlabel("Время, мс (медиана)")
    ax.set_title(title)
    for b, v in zip(bars, ms):
        ax.text(b.get_width(), b.get_y() + b.get_height() / 2, f" {v:.2f} ({ms[0] / v:.1f}x)", va="center")
    ax.set_xlim(0, max(ms) * 1.35)
    ax.grid(axis="x", alpha=0.3)
fig.tight_layout()
fig.savefig(res / "fig_dot_flags.png", dpi=110)
print(f"Сохранено: {res / 'fig_dot_flags.png'}")
