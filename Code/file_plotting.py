import matplotlib.pyplot as plt

## general file plotting script to view various relationships
## change the title and axes labels if necessary but the script will function the same regardless
## use this when there is only 1 set of datapoints (x and y) e.g. morans_calculations.txt
## will not work for multiple curve plots

def main():
    x_data = []
    y_data = []

    ## reading file
    file_location = "morans_calculation.txt"
    with open(file_location, "r") as file:
        for line in file:
            parts = line.strip().split()
            if len(parts) == 2:
                x, y = float(parts[0]), float(parts[1])
                x_data.append(x)
                y_data.append(y)

    print(x_data)
    ## plotting
    plt.figure(figsize = (12, 7))
    plt.scatter(x_data, y_data, marker = 'o')

    plt.title("Moran's I of the Mass-Based Self-Organising Galaxy Model", fontsize=16, fontweight='bold')
    plt.xlabel("Time (Iterations)", fontsize=14)
    plt.ylabel("Moran's I", fontsize=14)
    ## uncomment if a log scale is desired
    # plt.xscale("log")
    # plt.yscale("log")

    plt.grid(True, linestyle='--', alpha=0.6)
    plt.axhline(0, color='gray', linestyle='--', linewidth=1)
    
    plt.show()
    
if __name__ == "__main__":
    main()