import matplotlib.pyplot as plt
import numpy as np
import math

def main():
    
    lattice_dimension = 10
    flips = 0
    iterations = 200
    avg_correlations = {0: [], 1: [], 2: [], 3: [], 4: []}
    
    temperatures = [1.5, 2, 2.25, 3, 5]
    
    lattice = np.ones(shape=(lattice_dimension, lattice_dimension), dtype = "int")
    for temp in range(len(temperatures)):
        correlations = {0: [], 1: [], 2: [], 3: [], 4: []}
        for trial in range(iterations):
            rows, columns = lattice.shape
            for i in range(rows):
                for j in range(columns):
                    all_nbrs = find_all_neighbours(lattice, rows, columns, i, j, lattice_dimension)
                    sum_of_si_sj = sum(np.array(all_nbrs[0])*lattice[i][j])
                    e_flip = 2*(sum_of_si_sj)
                    correlations = find_correlation_func(all_nbrs, lattice, i , j, correlations)

                    lattice, flips = flip(e_flip, temperatures[temp], lattice, i, j, flips)
                    
        for x in range(len(correlations)):
            avg_correlations[x] = avg_correlations[x] + [sum(correlations[x])/(iterations*(lattice_dimension**2))]


        print(f"Temperature {temperatures[temp]} Completed")
        print(flips, " flips\nCurrent Lattice")
        print(lattice)

    plot(avg_correlations, temperatures)

def find_correlation_func(all_nbrs, lattice, i , j, correlations):
    for x in range(len(all_nbrs)):
        f = int((sum([lattice[i][j]*nbr for nbr in all_nbrs[x]])))/4
        correlations[x] = correlations[x] + [f]
    return correlations

def find_all_neighbours(lattice, rows, columns, i, j, lattice_dimension):
    all_nbrs = {}
    for x in range(int(lattice_dimension/2)):
        nbr = []
        nbr.append(int(lattice[i][j-(x+1)])) if (j-x) > 0 else nbr.append(int(lattice[i][rows-(x+1-j)]))          # left
        nbr.append(int(lattice[i][j+(x+1)])) if (j+x) < columns-1 else nbr.append(int(lattice[i][x-(columns-1-j)]))   # right
        nbr.append(int(lattice[i-(x+1)][j])) if (i-x) > 0 else nbr.append(int(lattice[rows-(x+1-i)][j]))          # top
        nbr.append(int(lattice[i+(x+1)][j])) if (i+x) < rows-1 else nbr.append(int(lattice[x-(rows-1-i)][j]))      # bottom
        all_nbrs[x] = nbr

    return all_nbrs

def flip(e_flip, T, lattice, i, j, flips):
    boltzmann_factor = (math.e)**(-e_flip/T)

    if e_flip <= 0:
        lattice[i][j] = lattice[i][j]*-1
        flips += 1
    else:
        r = np.random.random()
        if r <= boltzmann_factor:
            lattice[i][j] = lattice[i][j]*-1
            flips += 1

    return lattice, flips

def plot(avg_correlations, temperatures):
    ## magnetisation vs time
    plt.figure(figsize=(12, 7))
    distance = list(avg_correlations.keys())
    for temp in range(len(temperatures)):
        correlation_func = [avg_correlations[d][temp] for d in distance]
        
        plt.plot(distance, correlation_func, label=f"T = {temperatures[temp]}", marker = 'o', linewidth=1)

        plt.title("Correlation Function vs Distance", fontsize=16, fontweight='bold')
        plt.xlabel("Distance", fontsize=14)
        plt.ylabel("Correlation Function", fontsize=14)

        plt.grid(True, linestyle='--', alpha=0.6)
        plt.axhline(0, color='gray', linestyle='--', linewidth=1)

    plt.legend()
    
    plt.show()

if __name__ == "__main__":
    main()
