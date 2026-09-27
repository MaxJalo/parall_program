"""График ускорения S(N) по законам Амдала и Густафсона для нескольких долей P."""
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

N = np.array([1, 2, 4, 8, 16, 32, 64, 128, 256])
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(11, 4.5))

for P in (0.5, 0.9, 0.95, 0.99):
    amdahl = 1.0 / ((1.0 - P) + P / N)
    gustafson = N - (1.0 - P) * (N - 1)
    ax1.plot(N, amdahl, marker="o", label=f"P = {P}")
    ax2.plot(N, gustafson, marker="o", label=f"P = {P}")

for ax, title in ((ax1, "Амдал (фиксированная задача)"), (ax2, "Густафсон (задача растёт с N)")):
    ax.set_xscale("log", base=2)
    ax.set_xlabel("Число ядер N")
    ax.set_ylabel("Ускорение S")
    ax.set_title(title)
    ax.grid(True, alpha=0.3)
    ax.legend()

out = Path(__file__).resolve().parent.parent / "results" / "fig_amdahl.png"
fig.tight_layout()
fig.savefig(out, dpi=120)
print(f"Сохранено: {out}")
