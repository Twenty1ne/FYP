import tkinter as tk
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.colors import ListedColormap
import numpy as np

def main():
    file_location = "--------"
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

    ## create gui for visualisation
    app = tk.Tk()
    app.title("BTW Sandpile Evolution")

    fig, ax = plt.subplots(figsize=(5, 5))
    canvas = FigureCanvasTkAgg(fig, master=app)
    canvas.get_tk_widget().pack()

    slider = ttk.Scale(
        app,
        from_ = 0,
        to = len(grid_history) - 1,
        orient = 'horizontal',
        command = lambda val: show_grid(int(float(val)), ax, canvas, grid_history)
        )
    slider.pack(fill='x', expand=True, padx=10, pady=10)

    ## show initial state
    im = ax.imshow(grid_history[0], cmap=ListedColormap(['white', 'lightgrey', 'grey', 'black']), vmin=0, vmax=3)
    ax.set_title("Step 0")

    ## colour legend
    fig.colorbar(im, ax=ax, ticks=[0, 1, 2, 3])

    ## destroy window
    def on_close():
        app.quit()
        app.destroy()

    app.protocol("WM_DELETE_WINDOW", on_close)

    app.mainloop()
    
def show_grid(index, ax, canvas, grid_history):
    ax.clear()

    ax.imshow(grid_history[index], cmap=ListedColormap(['white', 'lightgrey', 'grey', 'black']), vmin=0, vmax=3)
    ax.set_title(f"Step {index}")
    canvas.draw()

if __name__ == "__main__":
    main()
