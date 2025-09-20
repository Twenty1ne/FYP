import matplotlib.pyplot as plt
import numpy as np
import math
from scipy import constants

def main():
    
    lattice_dimension = 10
    flips = 0
    iterations = 1000
    all_magnetisation_data = {}
    avg_magnetisation = []
    energies_per_spin = []
    energies_squared_per_spin = []
    
    # temperatures = [1.5, 2, 2.25, 4] ## for magnetisation vs time
    # temperatures = np.linspace(1, 4, 31) ## for avg magnetisation vs temp
    # temperatures = np.linspace(0, 5, 51) ## for energy per spin vs temp
    temperatures = np.linspace(1, 5, 41) ## for specific heat vs temp
    
    lattice = np.ones(shape=(lattice_dimension, lattice_dimension), dtype = "int")
    for temp in range(len(temperatures)):
        magnetisation = [1]
        energies = []
        for trial in range(iterations):
            rows, columns = lattice.shape
            for i in range(rows):
                for j in range(columns):
                    nbr = find_neighbours(lattice, rows, columns, i, j)
                    #print("Point: ", lattice[i][j], "\nNeighbours: ", nbr)
                    sum_of_si_sj = sum(np.array(nbr)*lattice[i][j])
                    energies.append(-int(sum_of_si_sj/2))
                    e_flip = 2*(sum_of_si_sj)

                    lattice, flips = flip(e_flip, temperatures[temp], lattice, i, j, flips)
                    
            magnetisation.append(int(np.sum(np.array(lattice)))/(lattice_dimension**2))

        all_magnetisation_data[temp] = magnetisation
        avg_magnetisation.append(abs(sum(magnetisation)/iterations))
        energies_squared = [x**2 for x in energies]
        energies_per_spin.append((sum(energies))/(iterations*(lattice_dimension**2)))
        energies_squared_per_spin.append((sum(energies_squared))/(iterations*(lattice_dimension**2)))

        print(f"Temperature {temperatures[temp]} Completed")
        print(flips, " flips\nCurrent Lattice")
        print(lattice)

    specific_heat = calculate_specific_heat(energies_per_spin, energies_squared_per_spin, temperatures)
    plot(all_magnetisation_data, temperatures, avg_magnetisation, energies_per_spin, specific_heat)

def calculate_specific_heat(energies_per_spin, energies_squared_per_spin, T):
    energies_per_spin_squared = [val**2 for val in energies_per_spin]
    x = [(y**2)/constants.k for y in T]
    energy_variance = [a - b for a, b in zip(energies_squared_per_spin, energies_per_spin_squared)]
    return [c / d for c, d in zip(energy_variance, x)]

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

def plot(all_magnetisation_data, temperatures, avg_magnetisation, energies_per_spin, specific_heat):
    # ## magnetisation vs time
    # for T, magnetisation in all_magnetisation_data.items():
    #     plt.figure(figsize=(12, 7))
    #     time_steps = list(range(len(magnetisation)))
    #     plt.plot(time_steps, magnetisation, label=f"T = {temperatures[T]}", linewidth=1)

    #     plt.title("Magnetisation vs Time", fontsize=16, fontweight='bold')
    #     plt.xlabel("Time Steps", fontsize=14)
    #     plt.ylabel("Magnetisation", fontsize=14)

    #     plt.ylim(-1, 1)
    #     plt.grid(True, linestyle='--', alpha=0.6)
    #     plt.axhline(0, color='gray', linestyle='--', linewidth=1)

    #     plt.legend()
    
    # ## avg magnetisation vs temp
    # plt.figure(figsize=(12, 7))
    # plt.scatter(temperatures, avg_magnetisation, marker='o')

    # plt.title("Average Magnetisation vs Temperature", fontsize=16, fontweight='bold')
    # plt.xlabel("Temperature", fontsize=14)
    # plt.ylabel("Average Magnetisation", fontsize=14)

    # plt.grid(True, linestyle='--', alpha=0.6)
    # plt.axhline(0, color='gray', linestyle='--', linewidth=1)

    # ## energy per spin vs temp
    # plt.figure(figsize=(12, 7))
    # plt.scatter(temperatures, energies_per_spin, marker='o')

    # plt.title("Energy Per Spin vs Temperature", fontsize=16, fontweight='bold')
    # plt.xlabel("Temperature", fontsize=14)
    # plt.ylabel("Energy Per Spin", fontsize=14)

    # plt.grid(True, linestyle='--', alpha=0.6)
    # plt.axhline(0, color='gray', linestyle='--', linewidth=1)
    
    ## specific heat vs temp
    plt.figure(figsize=(12, 7))
    plt.scatter(temperatures, specific_heat, marker='o')

    plt.title("Specific Heat Per Spin vs Temperature", fontsize=16, fontweight='bold')
    plt.xlabel("Temperature", fontsize=14)
    plt.ylabel("Specific Heat Per Spin", fontsize=14)

    plt.grid(True, linestyle='--', alpha=0.6)
    plt.axhline(0, color='gray', linestyle='--', linewidth=1)
    
    plt.show()

if __name__ == "__main__":
    main()