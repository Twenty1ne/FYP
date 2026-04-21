import tkinter as tk
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np

## reads galaxy grid history and stores the info for each timestep
def load_galaxy_history(filepath):
    with open(filepath, 'r') as f:
        raw = f.read().strip().split('\n\n')

    history = []
    for timestep in raw:
        points = []
        for line in timestep.strip().split('\n'):
            r, theta, high_mass_stars, low_mass_stars = line.strip().split()

            high_mass_stars = int(high_mass_stars)
            low_mass_stars = int(low_mass_stars)
            r = float(r)
            theta = float(theta)
            x = r * np.cos(theta)
            y = r * np.sin(theta)

            ## scales the sizes of the high mass stars and low mass stars as the values are based on mass which are not suited for plotting
            high_mass_stars_scaled = np.sqrt(np.sqrt(high_mass_stars)) * 10
            low_mass_stars_scaled = np.sqrt(np.sqrt(low_mass_stars))

            points.append((x, y, high_mass_stars_scaled, low_mass_stars_scaled))
        history.append(points)

    return history

## creates the gui with the slider to scroll through timesteps
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

    ## update plot when the user interacts with the slider
    def update_plot(self, val):
        step = int(float(val))
        self.ax.clear()
        points = self.history[step]

        if points:
            x, y, high_mass_stars, low_mass_stars = zip(*points)

            ## RED POINTS (low-mass stars)
            red_x = []
            red_y = []
            red_sizes = []

            for i in range(len(points)):
                if low_mass_stars[i] > 0:
                    red_x.append(x[i])
                    red_y.append(y[i])
                    red_sizes.append(low_mass_stars[i])

            if red_x:
                self.ax.scatter(red_x, red_y, c='red', s=red_sizes)

            ## BLACK POINTS (high-mass stars)
            self.ax.scatter(x, y, c='black', s=high_mass_stars)

        self.ax.set_title(f"Time Step {step}")
        self.ax.set_xlim(-NUMBER_OF_RINGS - 1, NUMBER_OF_RINGS + 1)
        self.ax.set_ylim(-NUMBER_OF_RINGS - 1, NUMBER_OF_RINGS + 1)
        self.ax.set_aspect('equal')
        self.canvas.draw()


if __name__ == "__main__":
    NUMBER_OF_RINGS = 50

    root = tk.Tk()
    root.title("Galaxy Viewer")
    root.protocol("WM_DELETE_WINDOW", root.quit)

    filepath = "MBSOGM_III_history.txt"
    history = load_galaxy_history(filepath)

    viewer = GalaxyViewer(root, history)
    root.mainloop()
