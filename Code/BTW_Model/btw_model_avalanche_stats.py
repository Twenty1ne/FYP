import matplotlib.pyplot as plt
from collections import Counter
import numpy as np

def main():
    avalanche_size = []
    avalanche_energy = []
    avalanche_duration = []

    ## reading file
    file_location = "btw_model_avalanche_stats.txt"
    with open(file_location, 'r') as file:
        for line in file:
            parts = line.strip().split()
            if len(parts) == 3:
                avalanche_size.append(int(parts[0]))
                avalanche_energy.append(int(parts[1]))
                avalanche_duration.append(int(parts[2]))

    ## filter out zero or negative values to avoid log errors
    avalanche_size = [s for s in avalanche_size if s >= 1]
    avalanche_energy = [e for e in avalanche_energy if e >= 1]
    avalanche_duration = [d for d in avalanche_duration if d >= 1]

    size_counts_raw = Counter(avalanche_size)
    energy_counts_raw = Counter(avalanche_energy)
    duration_counts_raw = Counter(avalanche_duration)

    size_total = sum(size_counts_raw.values())
    energy_total = sum(energy_counts_raw.values())
    duration_total = sum(duration_counts_raw.values())

    size_counts = {k: v / size_total for k, v in size_counts_raw.items()}
    energy_counts = {k: v / energy_total for k, v in energy_counts_raw.items()}
    duration_counts = {k: v / duration_total for k, v in duration_counts_raw.items()}


    ## avalanche size plot
    plt.figure(figsize=(12, 7))
    plt.scatter(size_counts.keys(), size_counts.values(), marker='o', color='blue', label='Data')
    x_size_line = np.array([0.1, 3000])
    y_size_line = 1.5e-2 * (x_size_line / 10)**-1
    plt.plot(x_size_line, y_size_line, 'k--', label='Slope ≈ -1.0')
    plt.title("BTW Model - Avalanche Size", fontsize=16, fontweight='bold')
    plt.xlabel("Size", fontsize=14)
    plt.ylabel("D(s)", fontsize=14)
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.axhline(0, color='gray', linestyle='--', linewidth=1)
    plt.xscale("log")
    plt.yscale("log")
    plt.ylim(1e-6, 1)
    plt.xlim(0.5, 10000)
    plt.legend()

    ## avalanche energy plot
    plt.figure(figsize=(12, 7))
    plt.scatter(energy_counts.keys(), energy_counts.values(), marker='o', color='green', label='Data')
    x_energy_line = np.array([0.1, 20000])
    y_energy_line = 1.5e-3 * (x_energy_line / 100)**-1.01
    plt.plot(x_energy_line, y_energy_line, 'k--', label='Slope ≈ -1.01')
    plt.title("BTW Model - Avalanche Energy", fontsize=16, fontweight='bold')
    plt.xlabel("Energy", fontsize=14)
    plt.ylabel("D(E)", fontsize=14)
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.axhline(0, color='gray', linestyle='--', linewidth=1)
    plt.xscale("log")
    plt.yscale("log")
    plt.ylim(1e-6, 1)
    plt.xlim(0.5, 100000)
    plt.legend()

    ## avalanche duration plot
    plt.figure(figsize=(12, 7))
    plt.scatter(duration_counts.keys(), duration_counts.values(), marker='o', color='red', label='Data')
    x_duration_line = np.array([0.1, 400])
    y_duration_line = 2.1e-2 * (x_duration_line / 10)**-1.05
    plt.plot(x_duration_line, y_duration_line, 'k--', label='Slope ≈ -1.05')
    plt.title("BTW Model - Avalanche Duration", fontsize=16, fontweight='bold')
    plt.xlabel("Duration", fontsize=14)
    plt.ylabel("D(T)", fontsize=14)
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.axhline(0, color='gray', linestyle='--', linewidth=1)
    plt.xscale("log")
    plt.yscale("log")
    plt.ylim(1e-6, 1)
    plt.xlim(0.5, 1000)
    plt.legend()

    plt.show()

if __name__ == "__main__":
    main()
