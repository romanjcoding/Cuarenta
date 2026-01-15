import pandas as pd
import matplotlib.pyplot as plt
import math
from scipy.stats import t
import numpy as np
import seaborn as sns

plt.rcParams["font.family"] = "serif"
plt.rcParams["font.serif"] = ["Times New Roman"] + plt.rcParams["font.serif"]

sns.set_theme(style="whitegrid")
palette = {
    "P1": "#da6c44",
    "P2": "#a6d1dc",
}
df = pd.read_csv("data_early.csv")
df.columns = df.columns.str.strip()

players = ["P1", "P2"]

stats = (
    df[df["Player"].isin(players)]
    .groupby(["Player", "Round", "Captured Cards"])["Δ Score"]
    .agg(mean="mean", std="std", n="count")
    .reset_index()
)

# Compute 99.9% credible interval
credibility = 0.999
alpha = 1 - credibility
stats["ci"] = stats.apply(
    lambda row: (
        t.ppf(1 - alpha / 2, df=row["n"] - 1)
        * row["std"] / np.sqrt(row["n"])
    )
    if row["n"] > 1 else 0,
    axis=1
)

rounds = sorted(stats["Round"].unique())
cols = 2
rows = math.ceil(len(rounds) / cols)

x_min = 0
x_max = 40

y_min = 0
y_max = df["Δ Score"].max()
y_margin = 0.05 * (y_max - y_min)

fig, axes = plt.subplots(rows, cols, figsize=(6 * cols, 4 * rows))
axes = axes.flatten()

for i, rnd in enumerate(rounds):
    ax = axes[i]
    rdata = stats[stats["Round"] == rnd]

    for player in players:
        pdata = rdata[rdata["Player"] == player]

        ax.errorbar(
            pdata["Captured Cards"],
            pdata["mean"],
            yerr=pdata["ci"],
            fmt="o-",
            capsize=4,
            label=player,
            color=palette[player]
        )

    ax.set_title(f"Round {rnd}")
    ax.set_xlabel("Captured Cards")
    ax.set_ylabel("Average Δ Score")

    ax.set_xlim(x_min, x_max)
    ax.set_ylim(y_min - y_margin, y_max + y_margin)

    ax.legend()

for j in range(i + 1, len(axes)):
    fig.delaxes(axes[j])

plt.tight_layout()
plt.show()
