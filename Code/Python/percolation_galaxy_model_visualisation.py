import tkinter as tk
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np

def load_galaxy_history(filepath):
    with open(filepath, 'r') as f:
        raw = f.read().strip().split('\n\n')
    history = []
    for timestep in raw:
        points = []
        for line in timestep.strip().split('\n'):
            r, theta, active = line.strip().split()
            if int(active) == 1:
                r = float(r)
                theta = float(theta)
                x = r * np.cos(theta)
                y = r * np.sin(theta)
                points.append((x, y))
        history.append(points)
    return history

class GalaxyViewer:
    def __init__(self, master, history):
        self.master = master
        self.history = history
        self.current_step = tk.IntVar(value=0)

        self.fig, self.ax = plt.subplots(figsize=(8, 8))
        self.canvas = FigureCanvasTkAgg(self.fig, master=master)
        self.canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=True)

        self.scroll = ttk.Scale(master, from_=0, to=len(history)-1,
                                orient='horizontal', variable=self.current_step,
                                command=self.update_plot)
        self.scroll.pack(fill=tk.X, padx=10, pady=10)

        self.update_plot(0)

    def update_plot(self, val):
        step = int(float(val))
        self.ax.clear()
        points = self.history[step]
        if points:
            x, y = zip(*points)
            self.ax.scatter(x, y, c='black', s = 20)
        self.ax.set_title(f"Time Step {step}")
        self.ax.set_xlim(-NUMBER_OF_RINGS-1, NUMBER_OF_RINGS+1)
        self.ax.set_ylim(-NUMBER_OF_RINGS-1, NUMBER_OF_RINGS+1)
        self.ax.set_aspect('equal')
        self.canvas.draw()

if __name__ == "__main__":
    NUMBER_OF_RINGS = 100
    root = tk.Tk()
    root.title("Percolation Galaxy Viewer")
    root.protocol("WM_DELETE_WINDOW", root.quit)

    filepath = "-------------"
    history = load_galaxy_history(filepath)
    viewer = GalaxyViewer(root, history)

    root.mainloop()