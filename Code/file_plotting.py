import matplotlib.pyplot as plt

def main():
    x_data = []
    y_data = []

    ## reading file
    file_location = "-----------"
    with open(file_location, "r") as file:
        for line in file:
            parts = line.strip().split()
            if len(parts) == 2:
                x, y = float(parts[0]), float(parts[1])
                x_data.append(x)
                y_data.append(y)

    ## plotting
    plt.figure(figsize = (12, 7))
    plt.scatter(x_data, y_data, marker = 'o')

    plt.title("BTW Model System Mass", fontsize=16, fontweight='bold')
    plt.xlabel("Time (Iterations)", fontsize=14)
    plt.ylabel("Total Mass", fontsize=14)

    plt.grid(True, linestyle='--', alpha=0.6)
    plt.axhline(0, color='gray', linestyle='--', linewidth=1)
    
    plt.show()
    
if __name__ == "__main__":
    main()