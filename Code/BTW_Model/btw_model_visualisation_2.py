import tkinter as tk
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np
from matplotlib.colors import ListedColormap
from scipy.ndimage import binary_fill_holes

## altered version of btw_model_visualisation.py that outlines the collapsed areas
## helps to see the scaleless distribution of cluster sizes

def main():
    file_location = "btw_model_grid_data.txt"

    with open(file_location, 'r') as file:
        grid_history = []
        current_grid = []

        for line in file:
            line = line.strip()
            if line == "":
                if current_grid:
                    grid_history.append(np.array(current_grid, dtype=int))
                    current_grid = []
            else:
                row = [int(char) for char in line]
                current_grid.append(row)

        if current_grid:
            grid_history.append(np.array(current_grid, dtype=int))

    ## create gui
    app = tk.Tk()
    app.title("BTW Sandpile Cell Changes")

    fig, ax = plt.subplots(figsize=(6, 6))
    canvas = FigureCanvasTkAgg(fig, master=app)
    canvas.get_tk_widget().pack()

    slider = ttk.Scale(
        app,
        from_=0,
        to=len(grid_history) - 1,
        orient='horizontal',
        command=lambda val: show_changes(int(float(val)), ax, canvas, grid_history)
    )
    slider.pack(fill='x', expand=True, padx=10, pady=10)

    ## show initial blank canvas
    ax.imshow(np.zeros_like(grid_history[0]), cmap="gray", vmin=0, vmax=1)
    ax.set_title("Step 0")
    canvas.draw()

    def on_close():
        app.quit()
        app.destroy()

    app.protocol("WM_DELETE_WINDOW", on_close)
    app.mainloop()


def show_changes(index, ax, canvas, grid_history):
    if not hasattr(show_changes, "change_map"):
        show_changes.change_map = np.zeros_like(grid_history[0], dtype=int)

    step_color = (index % 20) + 1

    if index > 0:
        prev_grid = grid_history[index - 1]
        curr_grid = grid_history[index]

        ## find changed cells
        changed = (curr_grid != prev_grid)

        ## fill enclosed regions formed by changed cells
        filled = binary_fill_holes(changed)

        ## update change_map with this steps color
        show_changes.change_map[filled] = step_color

    ## build colormap
    base_colors = ["black"] + [plt.get_cmap("tab20")(i) for i in range(20)]
    cmap = ListedColormap(base_colors)

    ax.clear()
    ax.imshow(show_changes.change_map, cmap=cmap, vmin=0, vmax=20)
    ax.set_title(f"Changes up to Step {index}")
    canvas.draw()


if __name__ == "__main__":
    main()
