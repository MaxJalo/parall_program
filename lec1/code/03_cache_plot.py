"""Строит график пропускной способности по CSV из 03_cache_sweep (stdin или results/cache_sweep.csv)."""
import csv
import sys
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt

root = Path(__file__).resolve().parent.parent
src = Path(sys.argv[1]) if len(sys.argv) > 1 else root / "results" / "cache_sweep.csv"
rows = list(csv.DictReader(src.open()))
kb = [int(r["size_kb"]) for r in rows]
gbps = [float(r["gbps"]) for r in rows]

fig, ax = plt.subplots(figsize=(9, 4.5))
ax.plot(kb, gbps, marker="o")
for x, name in ((48, "L1d 48 КБ"), (1280, "L2 1.25 МБ"), (12288, "L3 12 МБ")):
    ax.axvline(x, color="gray", ls="--", alpha=0.6)
    ax.text(x * 1.05, max(gbps) * 0.95, name, rotation=90, va="top")
ax.set_xscale("log", base=2)
ax.set_xlabel("Размер массива, КБ")
ax.set_ylabel("Пропускная способность чтения, ГБ/с")
ax.set_title("Иерархия памяти i5-12450H (одно ядро)")
ax.grid(True, alpha=0.3)
out = root / "results" / "fig_cache_sweep.png"
fig.tight_layout()
fig.savefig(out, dpi=120)
print(f"Сохранено: {out}")
