#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <fstream>

// USE file_plotting.py TO PLOT THE DATA FILE

// initialise constants
const int LATTICE_DIMENSIONS = 10;
const float TEMPERATURE = 2.25;

// function prototypes
void flip(int e_flip, std::vector<std::vector<int>>& lattice, int i, int j);
std::vector<int> find_neighbours(const std::vector<std::vector<int>>& lattice, int i, int j);

int main(){
    std::cout << std::endl << std::endl;
    srand(time(0));
    // initialise all variables
    std::vector<int> iterations;
    iterations.reserve(200);
    for(int x = 500; x <= 100000; x += 500){
        iterations.push_back(x);
    }
    std::vector<double> avg_magnetisations;
    avg_magnetisations.reserve(200);

    // create lattice
    std::vector<std::vector<int>> lattice(LATTICE_DIMENSIONS, std::vector<int>(LATTICE_DIMENSIONS, 1));

    // begin loop
    for(int duration : iterations){
        std::vector<double> magnetisation;
        magnetisation.push_back(1.0);

        for(int trial = 0; trial < duration; trial++){
            for(int i = 0; i < LATTICE_DIMENSIONS; i++){
                for(int j = 0; j < LATTICE_DIMENSIONS; j++){
                    std::vector<int> nbrs = find_neighbours(lattice, i, j);
                    // sum nbrs
                    int sum_of_si_sj = 0;
                    for(int nbr : nbrs){
                        sum_of_si_sj += nbr * lattice[i][j];
                    }
                    int e_flip = 2 * sum_of_si_sj;
                    flip(e_flip, lattice, i, j);
                }
            }

            // calculate total magnetisation at this trial
            int total_magnetisation = 0;
            for(const std::vector<int>& rows : lattice){
                for(int spin : rows){
                    total_magnetisation += spin;
                }
            }
            // save each magnetisations from each trial
            magnetisation.push_back(static_cast<double> (total_magnetisation) / (LATTICE_DIMENSIONS * LATTICE_DIMENSIONS));
        }

        // sum of all magnetisations from every trial
        double sum_of_magnetisations = 0;
        for(double mag : magnetisation){
            sum_of_magnetisations += mag;
        }
        // get avg and save it to the vector
        avg_magnetisations.push_back(sum_of_magnetisations / duration);

        std::cout << "Duration " << duration << " Completed" << std::endl;
    }
    
    // printing the avg magnetisations and durations
    for(double m : avg_magnetisations){
        std::cout << m << std::endl;
    }
    for(int duration : iterations){
        std::cout << duration << std::endl;
    }
    
    // // saving data to file to plot with python
    // std::ofstream outfile("avg_mag_vs_duration_data.txt");
    // if(outfile.is_open()){
    //     for(int i = 0; i < iterations.size(); i++){
    //         outfile << iterations[i] << " " << avg_magnetisations[i] << std::endl;
    //     }
    //     outfile.close();
    //     std::cout << "Data written to avg_mag_vs_duration_data.txt";
    // }
    // else{
    //     std::cout << "Error opening file";
    // }
    
    std::cout << std::endl << std::endl;
    return 0;
}

void flip(int e_flip, std::vector<std::vector<int>>& lattice, int i, int j){
    float boltzmann_factor = exp(-e_flip / TEMPERATURE);

    if(e_flip <= 0){
        lattice[i][j] *= -1;
    }
    else{
        float r = static_cast<float>(rand()) / RAND_MAX;
        if(r <= boltzmann_factor){
            lattice[i][j] *= -1;
        }
    }
}

std::vector<int> find_neighbours(const std::vector<std::vector<int>>& lattice, int i, int j){
    std::vector<int> nbrs;

    nbrs.push_back(lattice[i][(j-1 + LATTICE_DIMENSIONS) % LATTICE_DIMENSIONS]);    // left
    nbrs.push_back(lattice[i][(j+1) % LATTICE_DIMENSIONS]);                         // right
    nbrs.push_back(lattice[(i-1 + LATTICE_DIMENSIONS) % LATTICE_DIMENSIONS][j]);    // up
    nbrs.push_back(lattice[(i+1) % LATTICE_DIMENSIONS][j]);                         // down
    
    return nbrs;
}