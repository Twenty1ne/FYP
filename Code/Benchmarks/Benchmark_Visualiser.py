import tkinter as tk
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np

## this is an altered version of MBSOGM_Visualiser.py made purely to test benchmarks
## there is a lot of unnecessary code here but it was easier to alter the MBSOGM visualiser I already had than to write a new script

def load_galaxy_history(filepath):
    with open(filepath, 'r') as f:
        raw = f.read().strip().split('\n\n')

    history = []
    for timestep in raw:
        points = []
        count = 0
        for line in timestep.strip().split('\n'):
            r, stellar_mass, size, theta = line.strip().split()
            size = int(size)
            if size > 0:
                count += 1
            r = float(r)
            theta = float(theta)
            x = r * np.cos(theta)
            y = r * np.sin(theta)

            points.append((x, y, size, stellar_mass))
        history.append(points)
        print(count)

    return history


class GalaxyViewer:
    def __init__(self, master, history):
        self.master = master
        self.history = history
        self.current_step = tk.IntVar(value=0)

        self.fig, self.ax = plt.subplots(figsize=(8, 8))
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

        self.update_plot(0)

    def update_plot(self, val):
        step = int(float(val))
        self.ax.clear()
        points = self.history[step]

        if points:
            x, y, sizes, stellar_mass = zip(*points)

            self.ax.scatter(x, y, c='black', s=sizes)

        self.ax.set_title(f"Time Step {step}")
        self.ax.set_xlim(-NUMBER_OF_RINGS - 1, NUMBER_OF_RINGS + 1)
        self.ax.set_ylim(-NUMBER_OF_RINGS - 1, NUMBER_OF_RINGS + 1)
        self.ax.set_aspect('equal')
        self.canvas.draw()


if __name__ == "__main__":
    NUMBER_OF_RINGS = 50

    root = tk.Tk()
    root.title("Benchmark Visualiser")
    root.protocol("WM_DELETE_WINDOW", root.quit)

    filepath = "Morans_I_Correlated_Benchmark.txt"
    history = load_galaxy_history(filepath)

    viewer = GalaxyViewer(root, history)
    root.mainloop()
