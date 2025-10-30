#include <iostream>
#include <vector>
#include <ctime>
#include <fstream>

const int NUMBER_OF_RINGS = 100;
const int TIME_STEPS = 50;
const double PI = 3.14159265358979323846;
const float FORMATION_PROBABILITY = 0.18f;

// function prototypes
std::vector<std::pair<int, int>> find_neighbours(const unsigned short& r, const unsigned short& angle_index);

struct Star{
    unsigned short r;
    unsigned short angle_index;
    float theta;
    bool active;

    void supernova(std::vector<std::vector<Star>>& polar_grid){
        this-> active = false;
        
        std::vector<std::pair<int, int>> nbrs = find_neighbours(r, angle_index);
        for(const auto& [r, angle] : nbrs){
            float spread = static_cast<float>(rand()) / RAND_MAX;
            if(spread <= FORMATION_PROBABILITY){
                polar_grid[r][angle].active = true;
            }
        }
    }
};

// function prototypes
void populate_grid(std::vector<std::vector<Star>>& polar_grid);
void rotate(std::vector<std::vector<Star>>& polar_grid);
void save_galaxy_history(const std::vector<std::vector<std::vector<Star>>>& galaxy_history);

int main(){
    std::cout << std::endl << std::endl;

    srand(time(0));

    std::vector<std::vector<std::vector<Star>>> galaxy_history;
    std::vector<std::vector<Star>> polar_grid;
    populate_grid(polar_grid);
    galaxy_history.push_back(polar_grid);
    
    for(int t = 0; t < TIME_STEPS; t++){
        std::vector<Star*> active_stars;
        for(int r = 0; r < polar_grid.size(); r++){
            for(auto& star : polar_grid[r]){
                if(star.active){
                    active_stars.push_back(&star);
                }
            }
        }
        for(auto* star : active_stars){
            star->supernova(polar_grid);
        }
        for(int x = 0; x < 10; x++){
            rotate(polar_grid);
        }
        galaxy_history.push_back(polar_grid);
    }

    save_galaxy_history(galaxy_history);

    std::cout << std::endl << std::endl;
    return 0;
}

void populate_grid(std::vector<std::vector<Star>>& polar_grid){
    std::vector<Star> ring;
    ring.push_back({0, 0, 0, 1});
    polar_grid.push_back(ring);

    for(int r = 1; r <= NUMBER_OF_RINGS; r++){
        int numStars = 6 * r;
        std::vector<Star> ring;

        // half angle offset for even rings
        float offset = (r % 2 == 0) ? static_cast<float>(PI / numStars) : 0.0f;

        for(int site = 0; site < numStars; site++){
            float theta = static_cast<float>(2 * PI * site / numStars) + offset;
            bool isActive = (static_cast<float>(rand()) / RAND_MAX) <= 0.05;

            ring.push_back({static_cast<unsigned short>(r), static_cast<unsigned short>(site), theta, isActive});
        }
        polar_grid.push_back(ring);
    }
}

std::vector<std::pair<int, int>> find_neighbours(const unsigned short& r, const unsigned short& angle_index){
    int numStars = r*6;
    std::vector<std::pair<int, int>> nbrs;
    
    switch(r){
        case 0:{
            nbrs.push_back({1, 0});
            nbrs.push_back({1, 1});
            nbrs.push_back({1, 2});
            nbrs.push_back({1, 3});
            nbrs.push_back({1, 4});
            nbrs.push_back({1, 5});
            return nbrs;
        }
        case 1:{
            int sector = angle_index/r;
            // centre and same ring neighbours
            nbrs.push_back({0, 0});
            nbrs.push_back({r, (angle_index-1 + numStars) % numStars});
            nbrs.push_back({r, (angle_index+1) % numStars});
            
            // outer ring neighbours
            nbrs.push_back({r+1, (angle_index+1 + sector)});
            nbrs.push_back({r+1, (angle_index + sector)});
            return nbrs;
        }
        case NUMBER_OF_RINGS:{
            int sector = angle_index/r;
            // same ring neighbours
            nbrs.push_back({r, (angle_index-1 + numStars) % numStars});
            nbrs.push_back({r, (angle_index+1) % numStars});

            // inner ring neighbours (angke index -1, angle index)
            nbrs.push_back({r-1, (angle_index-1 - sector + numStars-6) % (numStars-6)});
            nbrs.push_back({r-1, (angle_index - sector + numStars-6) % (numStars-6)});
            return nbrs;
        }
        default:{
            int sector = angle_index/r;
            // same ring neighbours
            nbrs.push_back({r, (angle_index-1 + numStars) % numStars});
            nbrs.push_back({r, (angle_index+1) % numStars});

            // inner ring neighbours (angke index -1, angle index)
            nbrs.push_back({r-1, (angle_index-1 - sector + numStars-6) % (numStars-6)});
            nbrs.push_back({r-1, (angle_index - sector + numStars-6) % (numStars-6)});

            // outer neighbours (angle index, angle index +1)
            nbrs.push_back({r+1, (angle_index+1 + sector)});
            nbrs.push_back({r+1, (angle_index + sector)});
            return nbrs;
        }
    }


}

void rotate(std::vector<std::vector<Star>>& polar_grid){
    for(int r = 1; r < polar_grid.size(); r++){
        int numStars = polar_grid[r].size();
        for(int i = 0; i < numStars; i++){
            polar_grid[r][i].angle_index = (polar_grid[r][i].angle_index + 1) % numStars;
            polar_grid[r][i].theta = static_cast<float>(2*PI*polar_grid[r][i].angle_index / numStars);
        }
    }
}

void save_galaxy_history(const std::vector<std::vector<std::vector<Star>>>& galaxy_history){
    std::cout << "Writing Galaxy History" << std::endl;
    std::ofstream outfile("percolation_galaxy_history.txt");
    if(outfile.is_open()){
        for(int t = 0; t < galaxy_history.size(); t++){
            for(const auto& ring : galaxy_history[t]){
                for(const auto& star : ring){
                    outfile << star.r << " " << star.theta << " " << star.active << std::endl;
                }
            }
            outfile << std::endl;
        }
        outfile.close();
        std::cout << "Data written to percolation_galaxy_history.txt" << std::endl;
    }
    else{
        std::cout << "Error opening file";
    }
}
