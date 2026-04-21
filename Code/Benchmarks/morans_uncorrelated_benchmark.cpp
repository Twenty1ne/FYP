#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>

/*
script to create benchmark polar grids to test the morans script
adapted from the main MBSOGMs code
*/

const int NUMBER_OF_RINGS = 50;
const double PI = 3.14159265358979323846;

struct Site{
    unsigned short r;
    unsigned short angle_index;
    float theta;
    unsigned int star_size;
};

void populate_grid(std::vector<std::vector<Site>>& polar_grid);
void save_galaxy_history(const std::vector<std::vector<Site>>& galaxy_history);

int main(){
    std::cout << std::endl << std::endl;

    std::vector<std::vector<std::vector<Site>>> galaxy_history;
    std::vector<std::vector<Site>> polar_grid;
    populate_grid(polar_grid);

    save_galaxy_history(polar_grid);

    std::cout << std::endl << std::endl;
    return 0;
}

void populate_grid(std::vector<std::vector<Site>>& polar_grid){
    std::vector<Site> ring;
    ring.push_back({0, 0, 0, 0});
    polar_grid.push_back(ring);

    for(int r = 1; r <= NUMBER_OF_RINGS; r++){
        int numStars = 6 * r;
        std::vector<Site> ring;
        float offset = (r % 2 == 0) ? static_cast<float>(PI / numStars) : 0.0f;

        for(int site = 0; site < numStars; site++){
            float theta = static_cast<float>(2 * PI * site / numStars) + offset;
            // populates any odd sum of r and site index. this is as checkerboarded as one can make this type of geometry
            unsigned int star_size = ((r + site) % 2 == 0) ? 0 : 10;
            ring.push_back({static_cast<unsigned short>(r), static_cast<unsigned short>(site), theta, star_size});
        }
        polar_grid.push_back(ring);
    }
}

void save_galaxy_history(const std::vector<std::vector<Site>>& galaxy_history){
    std::cout << "Writing Galaxy History" << std::endl;
    std::ofstream outfile("Morans_I_Uncorrelated_Benchmark.txt");
    if(outfile.is_open()){
        for(const auto& ring : galaxy_history){
            for(const auto& star : ring){
                outfile << star.r << " " << star.angle_index << " " << star.star_size << " " << star.theta << std::endl;
            }
        }
        outfile << std::endl;
        outfile.close();
        std::cout << "Morans_I_Uncorrelated_Benchmark.txt" << std::endl;
    }
    else{
        std::cout << "Error opening file" << std::endl;
    }
}