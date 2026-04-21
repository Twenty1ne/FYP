import tkinter as tk
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np

## load fractal dimension data from the file produced in C++
def load_fractal_history(filepath):
    with open(filepath, 'r') as f:
        raw = f.read().strip().split('\n\n')

    history = []
    for block in raw:
        pairs = []
        for line in block.strip().split('\n'):
            logR, logN = line.split()
            pairs.append((float(logR), float(logN)))
        history.append(pairs)

    return history

## create a gui and slider to scroll through the fractal dimension at each timestep
class FractalViewer:
    def __init__(self, master, history):
        self.master = master
        self.history = history
        self.current_step = tk.IntVar(value=0)

        self.compute_all_dimensions()

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

        ## seperate window showing the measured fractal dimension vs time
        self.create_dimension_history_window()
        self.plot_dimension_history()

        ## create initial plot
        self.update_plot(0)

    def compute_all_dimensions(self):
        self.D_history = []

        for timestep in self.history:
            logR, logN = zip(*timestep)
            logR = np.array(logR)
            logN = np.array(logN)

            slope, intercept = np.polyfit(logR, logN, 1)
            self.D_history.append(slope)

    def create_dimension_history_window(self):
        self.win2 = tk.Toplevel(self.master)
        self.win2.title("Fractal Dimension Evolution Over Time")

        self.fig2, self.ax2 = plt.subplots(figsize=(8, 4))
        self.canvas2 = FigureCanvasTkAgg(self.fig2, master=self.win2)
        self.canvas2.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=True)

    def plot_dimension_history(self):
        self.ax2.clear()
        self.ax2.plot(self.D_history, 'o-', color='purple')
        self.ax2.set_xlabel("Timestep")
        self.ax2.set_ylabel("Fractal Dimension")
        self.ax2.set_title("Fractal Dimension Evolution Over Time")
        self.ax2.grid(True, ls="--", alpha=0.5)
        self.canvas2.draw()

    ## update the plot when the user interacts with the slider
    def update_plot(self, val):
        step = int(float(val))
        self.ax.clear()

        data = self.history[step]
        logR, logN = zip(*data)
        logR = np.array(logR)
        logN = np.array(logN)

        ## regression fit
        slope, intercept = np.polyfit(logR, logN, 1)
        fit_line = slope * logR + intercept

        self.ax.plot(logR, logN, 'o', label="data")
        self.ax.plot(logR, fit_line, '-', label=f"fit (D={slope:.3f})")

        self.ax.set_xlabel("log(resolution)")
        self.ax.set_ylabel("log(pixel count)")
        self.ax.set_title(f"Fractal Dimension @ Timestep {step}: D = {slope:.3f}")
        self.ax.grid(True, ls="--", alpha=0.5)
        self.ax.legend()

        self.canvas.draw()

if __name__ == "__main__":
    root = tk.Tk()
    root.title("Fractal Dimension Viewer")
    root.protocol("WM_DELETE_WINDOW", root.quit)

    filepath = "fractal_dimension_history.txt"
    history = load_fractal_history(filepath)

    viewer = FractalViewer(root, history)
    root.mainloop()
