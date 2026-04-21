#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <complex>
#include <fstream>
#include <sstream>

// USE fractal_analysis.py TO ANALYSE THE FRACTAL DIMENSION AT EACH TIMESTEP

const double NUMBER_OF_RINGS = 50.0;
const std::vector<double> RESOLUTIONS = {2.0, 4.0, 8.0, 16.0, 32.0, 64.0}; // pixel map resolutions

// setup minimal Site struct for correlation data
struct Site{
    double x;
    double y;
    int contains_star;
};

// create vectors to store grid data
using Grid = std::vector<Site>;
using GalaxyHistory = std::vector<Grid>;

// function prototypes
std::map<double, double> fractal_dimensions(const Grid& grid);
void save_fractals(std::vector<std::map<double, double>>& fractal_dimension_history);

int main(){
    std::cout << std::endl << std::endl;

    std::ifstream inFile;
    std::string str;
    inFile.open("MBSOGM_III_history.txt");

    if(inFile.fail()){
        std::cerr << "File NOT Found!";
        return 1;
    }
    
    GalaxyHistory galaxy_history;
    Grid grid;
    std::string line;
    
    while(std::getline(inFile, line)){

        // detect blank lines and store grid into history for each timestep
        if(line.find_first_not_of(" \t\r\n") == std::string::npos){
            if(!grid.empty()){
                galaxy_history.push_back(grid);
                grid.clear();
            }
            continue;
        }

        // pare site (get 4 values)
        std::stringstream site_data(line);
        
        unsigned short r;
        unsigned int size, angle_index;
        double theta;

        site_data >> r >> theta >> size >> angle_index;

        // transform to cartesian coordinates
        double x = (r * cos(theta)) + NUMBER_OF_RINGS;
        double y = (r * sin(theta)) + NUMBER_OF_RINGS;
        
        Site site{x, y, ((size > 0) ? 1 : 0)};

        grid.push_back(site);
    }

    if(!grid.empty()){
        galaxy_history.push_back(grid);
    }

    std::cout << "Loaded " << galaxy_history.size() << " timesteps" << std::endl;
    
    // calculate fractal dimension
    std::vector<std::map<double, double>> fractal_dimension_history;
    
    for(const Grid& grid : galaxy_history){
        fractal_dimension_history.push_back(fractal_dimensions(grid));
    }

    save_fractals(fractal_dimension_history);

    std::cout << std::endl << std::endl;
    return 0;
}

// creates a pixel map at each specified resolution
std::map<double, double> fractal_dimensions(const Grid& grid){
    std::map<double, double> fractals;
    for(const double& resolution : RESOLUTIONS){
        int pixel_count = 0;
        std::vector<std::vector<int>> pixel_map(resolution, std::vector<int>(resolution, 0));
        double scale = resolution / (2 * NUMBER_OF_RINGS);
        for(const Site& site : grid){
            // if a pixel contains a star then it is counted
            if(site.contains_star){
                unsigned int x_index = static_cast<unsigned int>(site.x * scale);
                unsigned int y_index = static_cast<unsigned int>(site.y * scale);
                if(x_index >= resolution) x_index = resolution - 1;
                if(y_index >= resolution) y_index = resolution - 1;
                pixel_map[y_index][x_index] = 1;
            }
        }
        for(auto& row : pixel_map){
            for(auto& cell : row){
                if(cell == 1) pixel_count++;
            }
        }
        fractals[log(resolution)] = log(pixel_count);
    }

    return fractals;
}

// saves fractal dimension data to a file for analysis
void save_fractals(std::vector<std::map<double, double>>& fractal_dimension_history){
    std::cout << "Writing Fractal Dimensions" << std::endl;
    std::ofstream outfile("fractal_dimension_history.txt");
    if(outfile.is_open()){
        for(std::map<double, double> fractal : fractal_dimension_history){
            for(const auto& [resolution, dimension] : fractal){
                outfile << resolution << ' ' << dimension << std::endl;
            }
            outfile << std::endl;
        }
        outfile.close();
        std::cout << "Data written to fractal_dimension_history.txt" << std::endl;
    }
    else{
        std::cout << "Error opening file" << std::endl;
    }
}