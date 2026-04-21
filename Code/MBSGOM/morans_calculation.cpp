#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>

// USE file_plotting.py TO PLOT MORANS I VS TIME

const int NUMBER_OF_RINGS = 50;

// setup minimal Site struct for correlation data
struct Site{
    unsigned short r;
    unsigned short angle_index;
    int contains_star;
};

// create vectors to store grid data
using Ring = std::vector<Site>;
using PolarGrid = std::vector<Ring>;
using GalaxyHistory = std::vector<PolarGrid>;

// function prototypes
double calculate_morans(const PolarGrid& polar_grid);
std::vector<std::pair<int, int>> find_neighbours(const unsigned short& r, const unsigned short& angle_index);
void save_morans_correlations(const std::vector<double>& morans_correlations);

int main(){
    std::cout << std::endl << std::endl;

    std::ifstream inFile;
    std::string str;
    inFile.open("MBSOGM_III_correlations_history.txt");

    if(inFile.fail()){
        std::cerr << "File NOT Found!";
        return 1;
    }
    
    GalaxyHistory galaxy_history;
    PolarGrid polar_grid;
    std::string line;
    
    while(std::getline(inFile, line)){

        // detect blank lines and store grid into history for each timestep
        if(line.find_first_not_of(" \t\r\n") == std::string::npos){
            if(!polar_grid.empty()){
                galaxy_history.push_back(polar_grid);
                polar_grid.clear();
            }
            continue;
        }

        // pare site (get 4 values)
        std::stringstream site_data(line);
        
        unsigned short r, angle_index;
        int size;
        int stellar_mass;

        site_data >> r >> angle_index >> size >> stellar_mass;

        Site site{r, angle_index, ((size > 0) ? 1 : 0)};

        if(polar_grid.size() <= r){
            polar_grid.resize(r + 1);
        }

        polar_grid[r].push_back(site);
    }

    if(!polar_grid.empty()){
        galaxy_history.push_back(polar_grid);
    }

    std::cout << "Loaded " << galaxy_history.size() << " timesteps" << std::endl;

    std::vector<double> morans_correlations;
    
    // calculate morans I at each timestep and store it
    for(const PolarGrid& polar_grid : galaxy_history){
        morans_correlations.push_back(calculate_morans(polar_grid));
    }
    std::cout << morans_correlations[0] << std::endl;

    save_morans_correlations(morans_correlations);
    
    std::cout << std::endl << std::endl;
    return 0;
}

// morans I calculations (described in thesis)
double calculate_morans(const PolarGrid& polar_grid){
    const double N = 1 + 6 * (NUMBER_OF_RINGS * (NUMBER_OF_RINGS + 1)) / 2;
    const double W = 6 + 30 + 1200 + 36 * ((NUMBER_OF_RINGS * (NUMBER_OF_RINGS - 1) / 2) - 1);
    double x_avg = 0;
    double denominator = 0;
    double numerator = 0;

    for(const auto& ring : polar_grid){
        for(const auto& site : ring){
            x_avg += site.contains_star;
        }
    }
    x_avg /= N;

    // loop through grid to calculate morans I
    for(const auto& ring : polar_grid){
        for(const auto& site : ring){
            denominator += pow((site.contains_star - x_avg), 2);
            std::vector<std::pair<int, int>> nbrs = find_neighbours(site.r, site.angle_index);
            for(const auto& [r, angle] : nbrs){
                numerator += (site.contains_star - x_avg) * (polar_grid[r][angle].contains_star - x_avg);
            }
        }
    }

    return (N / W) * (numerator / denominator);
}

// find neighbours function can be reused here
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

// save morans data to a file for plotting
void save_morans_correlations(const std::vector<double>& morans_correlations){
    std::cout << "Writing Morans Correlations" << std::endl;
    std::ofstream outfile("morans_calculation.txt");
    if(outfile.is_open()){
        for(int i = 0; i < morans_correlations.size(); i++){
            outfile << i << ' ' << morans_correlations[i] << std::endl;
        }
        outfile.close();
        std::cout << "Data written to morans_calculation.txt" << std::endl;
    }
    else{
        std::cout << "Error opening file" << std::endl;
    }
}