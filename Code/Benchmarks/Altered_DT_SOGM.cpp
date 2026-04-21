#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <fstream>

/*
USE MBSOGM_Visualiser.py TO VIEW EVOLUTION
Can use MBSOGM Tools to analyse correlation data but the purpose of this model was to compare visually with the MBSOGMs

altered dt sog model that preserves mass
distributes to neighbours isotropically (minus one neighbour)
distribute 5 mass to neighbours keep 1 as a stellar remnant

uses an old method of file writing that was replaced for the MBSOGMs
the output file has the same format but this method takes about 20 seconds to save galaxy history for 400 timesteps
the new method takes about 2 seconds to save 1500 timesteps and can be seen in the MBSOGMs
was not important here as simulation durations were short but was needed later to model the MBSOGMs over longer periods
*/

const int NUMBER_OF_RINGS = 50;
const int TIME_STEPS = 200;
const double PI = 3.14159265358979323846;

std::vector<std::pair<int, int>> find_neighbours(const unsigned short& r, const unsigned short& angle_index);

struct Site{
    unsigned short r;
    unsigned short angle_index;
    float theta;
    unsigned short gas_mass;
    unsigned short stellar_mass;
    bool contains_active_star;
    unsigned short lifetime;
    unsigned short size;

    void supernova(std::vector<std::vector<Site>>& polar_grid){
        this->contains_active_star = false;
        this->stellar_mass -= 5;
        std::vector<std::pair<int, int>> nbrs = find_neighbours(r, angle_index);
        if(!nbrs.empty()){
            unsigned short idx = rand() % nbrs.size();
            nbrs.erase(nbrs.begin() + idx);
        }
        for(const auto& [r, angle] : nbrs){
            polar_grid[r][angle].gas_mass += 1;
        }
    }
};

void populate_grid(std::vector<std::vector<Site>>& polar_grid);
void rotate(std::vector<std::vector<Site>>& polar_grid);
void stabilise_grid(std::vector<std::vector<Site>>& polar_grid);
void save_galaxy_history(const std::vector<std::vector<std::vector<Site>>>& galaxy_history);
void save_correlation_history(const std::vector<std::vector<std::vector<Site>>>& galaxy_history);

int main(){
    std::cout << std::endl << std::endl;

    srand(time(0));

    std::vector<std::vector<std::vector<Site>>> galaxy_history;
    std::vector<std::vector<Site>> polar_grid;
    populate_grid(polar_grid);
    
    for(int t = 0; t < TIME_STEPS; t++){
        rotate(polar_grid);

        std::vector<Site*> exploding;
        for(auto& ring : polar_grid){
            for(auto& site : ring){
                if(site.contains_active_star && site.lifetime == 0){
                    exploding.push_back(&site);
                }
            }
        }
        for(auto* star : exploding){
            star->supernova(polar_grid);
        }
        stabilise_grid(polar_grid);

        galaxy_history.push_back(polar_grid);
    }

    save_galaxy_history(galaxy_history);
    save_correlation_history(galaxy_history);

    std::cout << std::endl << std::endl;
    return 0;
}

void populate_grid(std::vector<std::vector<Site>>& polar_grid){
    std::vector<Site> ring;
    ring.push_back({0, 0, 0, 5, 0, false, 0, 0});
    polar_grid.push_back(ring);

    for(int r = 1; r <= NUMBER_OF_RINGS; r++){
        int numStars = 6 * r;
        std::vector<Site> ring;
        float offset = (r % 2 == 0) ? static_cast<float>(PI / numStars) : 0.0f;

        for(int site = 0; site < numStars; site++){
            float theta = static_cast<float>(2 * PI * site / numStars) + offset;
            unsigned short gas_mass = static_cast<unsigned short>(rand() % 6);
            ring.push_back({static_cast<unsigned short>(r), static_cast<unsigned short>(site), theta, gas_mass, 0, false, 0, 0});
        }
        polar_grid.push_back(ring);
    }
}

std::vector<std::pair<int, int>> find_neighbours(const unsigned short& r, const unsigned short& angle_index){
    int numStars = r * 6;
    std::vector<std::pair<int, int>> nbrs;

    switch(r){
        case 0:
            for(int i = 0; i < 6; i++){
                nbrs.push_back({1, i});
            }
            return nbrs;
        case 1:{
            int sector = angle_index / r;
            nbrs.push_back({0, 0});
            nbrs.push_back({r, (angle_index - 1 + numStars) % numStars});
            nbrs.push_back({r, (angle_index + 1) % numStars});
            nbrs.push_back({r + 1, (angle_index + 1 + sector)});
            nbrs.push_back({r + 1, (angle_index + sector)});
            return nbrs;
        }
        case NUMBER_OF_RINGS:{
            int sector = angle_index / r;
            nbrs.push_back({r, (angle_index - 1 + numStars) % numStars});
            nbrs.push_back({r, (angle_index + 1) % numStars});
            nbrs.push_back({r - 1, (angle_index - 1 - sector + numStars - 6) % (numStars - 6)});
            nbrs.push_back({r - 1, (angle_index - sector + numStars - 6) % (numStars - 6)});
            return nbrs;
        }
        default:{
            int sector = angle_index / r;
            nbrs.push_back({r, (angle_index - 1 + numStars) % numStars});
            nbrs.push_back({r, (angle_index + 1) % numStars});
            nbrs.push_back({r - 1, (angle_index - 1 - sector + numStars - 6) % (numStars - 6)});
            nbrs.push_back({r - 1, (angle_index - sector + numStars - 6) % (numStars - 6)});
            nbrs.push_back({r + 1, (angle_index + 1 + sector)});
            nbrs.push_back({r + 1, (angle_index + sector)});
            return nbrs;
        }
    }
}

void stabilise_grid(std::vector<std::vector<Site>>& polar_grid){
    for(int r = 0; r < polar_grid.size(); r++){
        for(auto& site : polar_grid[r]){
            if(site.gas_mass >= 5 && !site.contains_active_star){
                site.contains_active_star = true;
                site.lifetime = 10;
                site.gas_mass -= 5;
                site.stellar_mass += 6;
                site.size = 100;
            }
        }
    }
}

void rotate(std::vector<std::vector<Site>>& polar_grid){

    if(polar_grid[0][0].contains_active_star && polar_grid[0][0].lifetime > 0){
        polar_grid[0][0].lifetime -= 1;
    }
    if(polar_grid[0][0].size > 0){
        polar_grid[0][0].size -= 1;
    }

    int current_mass = 0;
    for(int r = 1; r < polar_grid.size(); r++){
        int numStars = polar_grid[r].size();
        float offset = (r % 2 == 0) ? static_cast<float>(PI / numStars) : 0.0f;

        std::rotate(polar_grid[r].begin(), polar_grid[r].end() - 1, polar_grid[r].end());

        for(int i = 0; i < numStars; i++){
            polar_grid[r][i].angle_index = i;
            polar_grid[r][i].theta = static_cast<float>(2 * PI * polar_grid[r][i].angle_index / numStars) + offset;
            if(polar_grid[r][i].contains_active_star && polar_grid[r][i].lifetime > 0){
                polar_grid[r][i].lifetime -= 1;
            }
            if(polar_grid[r][i].size > 0){
                polar_grid[r][i].size -= 1;
            }
        }
    }
}

void save_galaxy_history(const std::vector<std::vector<std::vector<Site>>>& galaxy_history){
    std::cout << "Writing Galaxy History" << std::endl;
    std::ofstream outfile("Altered_DT_SOGM_history.txt");
    if(outfile.is_open()){
        for(int t = 0; t < galaxy_history.size(); t++){
            for(const auto& ring : galaxy_history[t]){
                for(const auto& star : ring){
                    outfile << star.r << " " << star.theta << " " << star.size << " " << star.stellar_mass << std::endl;
                }
            }
            outfile << std::endl;
        }
        outfile.close();
        std::cout << "Data written to Altered_DT_SOGM_history.txt" << std::endl;
    }
    else{
        std::cout << "Error opening file" << std::endl;
    }
}

void save_correlation_history(const std::vector<std::vector<std::vector<Site>>>& galaxy_history){
    std::cout << "Writing Correlation History" << std::endl;
    std::ofstream outfile("Altered_DT_SOGM_correlation_history.txt");
    if(outfile.is_open()){
        for(int t = 0; t < galaxy_history.size(); t++){
            for(const auto& ring : galaxy_history[t]){
                for(const auto& star : ring){
                    outfile << star.r << " " << star.angle_index << " " << star.size << " " << star.stellar_mass << std::endl;
                }
            }
            outfile << std::endl;
        }
        outfile.close();
        std::cout << "Data written to Altered_DT_SOGM_correlation_history.txt" << std::endl;
    }
    else{
        std::cout << "Error opening file" << std::endl;
    }
}
