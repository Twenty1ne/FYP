import matplotlib.pyplot as plt
import numpy as np
import math
from scipy import constants

## this script is too slow so it was re written in C++

def main():
    lattice_dimension = 10
    iterations = np.linspace(5000, 10000, 2) ## 10000 iterations takes much too long to efficiently test for larger durations such as 100000
    iterations = [int(duration) for duration in iterations]
    all_magnetisation_data = {}
    avg_magnetisation = []
    temperature = 2.25

    lattice = np.ones(shape=(lattice_dimension, lattice_dimension), dtype = "int")
    for duration in iterations:
        magnetisation = [1]
        energies = []
        for trial in range(duration):
            rows, columns = lattice.shape
            for i in range(rows):
                for j in range(columns):
                    nbr = find_neighbours(lattice, rows, columns, i, j)
                    sum_of_si_sj = sum(np.array(nbr)*lattice[i][j])
                    e_flip = 2*(sum_of_si_sj)

                    lattice = flip(e_flip, temperature, lattice, i, j)
                    
            magnetisation.append(int(np.sum(np.array(lattice)))/(lattice_dimension**2))

        all_magnetisation_data[duration] = magnetisation
        avg_magnetisation.append(sum(magnetisation)/duration)

        print(f"Duration {duration} Completed")
        print(lattice)

    print(avg_magnetisation)
    print(iterations)
    plot(avg_magnetisation, iterations)

def find_neighbours(lattice, rows, columns, i, j):
    nbr = []
    nbr.append(int(lattice[i][j+1])) if j < columns-1 else nbr.append(int(lattice[i][0]))
    nbr.append(int(lattice[i][j-1])) if j > 0 else nbr.append(int(lattice[i][-1]))
    nbr.append(int(lattice[i-1][j])) if i > 0 else nbr.append(int(lattice[-1][j]))
    nbr.append(int(lattice[i+1][j])) if i < rows-1 else nbr.append(int(lattice[0][j]))

    return nbr

def flip(e_flip, T, lattice, i, j):
    boltzmann_factor = (math.e)**(-e_flip/T)

    if e_flip <= 0:
        lattice[i][j] = lattice[i][j]*-1
    else:
        r = np.random.random()
        if r <= boltzmann_factor:
            lattice[i][j] = lattice[i][j]*-1

    return lattice

def plot(avg_magnetisation, iterations):
    ## avg magnetisation vs duration
    plt.figure(figsize = (12, 7))
    plt.scatter(iterations, avg_magnetisation, marker='o')

    plt.title("Average Magnetisation vs Duration", fontsize=16, fontweight='bold')
    plt.xlabel("Duration", fontsize=14)
    plt.ylabel("Average Magnetisation", fontsize=14)

    plt.grid(True, linestyle='--', alpha=0.6)
    plt.axhline(0, color='gray', linestyle='--', linewidth=1)
    
    plt.show()

if __name__ == "__main__":
    main()