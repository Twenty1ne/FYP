import tkinter as tk
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np

## load cluster size distribution data from the file created with C++
def load_cluster_history(filepath):
    with open(filepath, 'r') as f:
        raw = f.read().strip().split('\n\n')

    history = []
    for block in raw:
        pairs = []
        for line in block.strip().split('\n'):
            s, f = line.strip().split()
            pairs.append((int(s), int(f)))
        history.append(pairs)

    return history

## function to logarithmically bin the cluster sizes
def log_bin(sizes, freqs, bins_per_decade=5):
    sizes = np.array(sizes, dtype=float)
    freqs = np.array(freqs, dtype=float)

    mask = (sizes > 0) & (freqs > 0)
    sizes = sizes[mask]
    freqs = freqs[mask]

    if len(sizes) == 0:
        return np.array([]), np.array([])

    min_s = sizes.min()
    max_s = sizes.max()

    num_decades = np.log10(max_s) - np.log10(min_s)
    num_bins = max(1, int(num_decades * bins_per_decade))

    edges = np.logspace(np.log10(min_s), np.log10(max_s), num_bins + 1)

    binned_freqs = np.zeros(num_bins)
    binned_centers = np.zeros(num_bins)

    for i in range(num_bins):
        left = edges[i]
        right = edges[i + 1]

        mask = (sizes >= left) & (sizes < right)
        if mask.sum() > 0:
            binned_freqs[i] = freqs[mask].sum()
            binned_centers[i] = np.sqrt(left * right)
        else:
            binned_freqs[i] = 0
            binned_centers[i] = np.sqrt(left * right)

    mask = binned_freqs > 0
    return binned_centers[mask], binned_freqs[mask]

## create gui and slider to visulaise the cluster size distribution at each timestep
class ClusterViewer:
    def __init__(self, master, history):
        self.master = master
        self.history = history
        self.current_step = tk.IntVar(value=0)

        self.fig, self.ax = plt.subplots(figsize=(7, 6))
        self.canvas = FigureCanvasTkAgg(self.fig, master=master)
        self.canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=True)

        self.scroll = ttk.Scale(
            master,
            from_=0,
            to=len(history) - 1,
            orient='horizontal',
            variable=self.current_step,
            command=self.update_plot
        )
        self.scroll.pack(fill=tk.X, padx=10, pady=10)

        ## initial plot
        self.update_plot(0)

    ## update plot when the user interacts with the slider
    def update_plot(self, val):
        step = int(float(val))
        self.ax.clear()

        data = self.history[step]

        if data:
            sizes, freqs = zip(*data)
            sizes = np.array(sizes, dtype=float)
            freqs = np.array(freqs, dtype=float)

            # logarithmic binning
            bin_sizes, bin_freqs = log_bin(sizes, freqs, bins_per_decade=5)

            self.ax.scatter(bin_sizes, bin_freqs, s=40, c='black', label="Log-binned data")

            # regression fit only if there is at least 2 points
            if len(bin_sizes) >= 2:
                log_s = np.log10(bin_sizes)
                log_f = np.log10(bin_freqs)

                b, a = np.polyfit(log_s, log_f, 1)
                exponent = b

                s_fit = np.linspace(min(bin_sizes), max(bin_sizes), 200)
                f_fit = 10**(a + b * np.log10(s_fit))

                self.ax.plot(s_fit, f_fit, 'r--', label=f"Critical Exponent = {exponent:.3f}")

        self.ax.set_xscale("log")
        self.ax.set_yscale("log")
        self.ax.set_xlim(0.5, 500)
        self.ax.set_ylim(0.5, 500)

        self.ax.set_xlabel("Cluster Size")
        self.ax.set_ylabel("Frequency")
        self.ax.set_title(f"Cluster Size Distribution (Logarithmic Bins) @ Timestep {step}")
        self.ax.grid(True, which="both", ls="--", alpha=0.5)
        self.ax.legend()

        self.canvas.draw()


if __name__ == "__main__":
    root = tk.Tk()
    root.title("Cluster Size Distribution Viewer")
    root.protocol("WM_DELETE_WINDOW", root.quit)

    filepath = "cluster_distribution_history.txt"
    history = load_cluster_history(filepath)

    viewer = ClusterViewer(root, history)
    root.mainloop()
