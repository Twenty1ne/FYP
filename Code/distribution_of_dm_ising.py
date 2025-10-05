import matplotlib.pyplot as plt
import numpy as np
import math
from collections import Counter

def main():
    
    lattice_dimension = 10
    flips = 0
    iterations = 50000
    
    temperature = 4
    
    lattice = np.ones(shape=(lattice_dimension, lattice_dimension), dtype = "int")
    magnetisation = [1]
    for trial in range(iterations):
        rows, columns = lattice.shape
        for i in range(rows):
            for j in range(columns):
                nbr = find_neighbours(lattice, rows, columns, i, j)
                sum_of_si_sj = sum(np.array(nbr)*lattice[i][j])
                e_flip = 2*(sum_of_si_sj)

                lattice, flips = flip(e_flip, temperature, lattice, i, j, flips)
                
        magnetisation.append(int(np.sum(np.array(lattice)))/(lattice_dimension**2))

    print(f"Temperature {temperature} Completed")
    print(flips, " flips\nCurrent Lattice")
    print(lattice)

    delta_M_raw = np.diff(magnetisation).tolist()
    delta_M = [round(float(value), 3) for value in delta_M_raw]

    merged = []
    buffer = delta_M[0]

    for i in range(1, len(delta_M)):
        current = delta_M[i]
        if (buffer >= 0 and current >= 0) or (buffer < 0 and current < 0):
            buffer += current
        else:
            merged.append(round(buffer, 3))
            buffer = current

    merged.append(round(buffer, 3))
    freq = dict(Counter(merged))

    plot(freq, temperature)

def find_neighbours(lattice, rows, columns, i, j):
    nbr = []
    nbr.append(int(lattice[i][j+1])) if j < columns-1 else nbr.append(int(lattice[i][0]))
    nbr.append(int(lattice[i][j-1])) if j > 0 else nbr.append(int(lattice[i][-1]))
    nbr.append(int(lattice[i-1][j])) if i > 0 else nbr.append(int(lattice[-1][j]))
    nbr.append(int(lattice[i+1][j])) if i < rows-1 else nbr.append(int(lattice[0][j]))

    return nbr

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

def plot(freq, temperature):
    plt.figure(figsize=(12, 7))
    magnetisation = list(freq.keys())
    frequency = list(freq.values())

    plt.bar(magnetisation, frequency, width = 0.01, edgecolor = "black", label=f"Temperature = {temperature}")

    plt.title("Distribution of Magnetisation Changes", fontsize=16, fontweight='bold')
    plt.xlabel("Change in Magnetisation", fontsize=14)
    plt.ylabel("Frequency", fontsize=14)

    # plt.xscale("log")
    plt.yscale("log")

    plt.xlim(-2, 2)
    
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.legend()
    
    plt.show()

if __name__ == "__main__":
    main()