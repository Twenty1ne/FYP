#include <iostream>
#include <vector>
#include <set>
#include <ctime>
#include <fstream>

// USE btw_model_visulaisation.py TO VIEW EVOLUTION OF MODEL
// USE file_plotting.py TO VIEW EVOLUTION OF THE MASS OF MODEL
// USE btw_model_avalanche_stats.py TO VIEW AVALANCHE STATS OF MODEL

const int GRID_SIZE = 50;
const int ITERATIONS = 100000;
const std::vector<std::pair<int, int>> DIRECTIONS = {
    {0, -1},
    {0, 1},
    {-1, 0},
    {1, 0}
};

struct Avalanche_Stats{
    unsigned short size = 0;       // total unique sites toppled
    unsigned short energy = 0;     // total toppled
    unsigned short duration = 0;   // number of avalanches in a given time step
};

// function prototypes
Avalanche_Stats drop_grain(std::vector<std::vector<int>>& grid);
Avalanche_Stats topple(std::vector<std::vector<int>>& grid);
int record_mass(std::vector<std::vector<int>>& grid);
void save_grid_history(const std::vector<std::vector<std::vector<int>>>& grid_history);
void save_mass_history(const std::vector<int>& mass_history);
void save_avalanche_stats(const std::vector<Avalanche_Stats>& stats);

int main(){
    std::cout << std::endl << std::endl;
    // initialise variables
    std::vector<std::vector<std::vector<int>>> grid_history;
    std::vector<int> mass_history;
    std::vector<Avalanche_Stats> all_avalanche_stats;

    srand(time(0));
    // build grid
    std::vector<std::vector<int>> grid(GRID_SIZE, std::vector<int>(GRID_SIZE, 0));

    for(int burn_in = 0; burn_in < 10000; burn_in++){
        drop_grain(grid);
    }
    
    for(int iteration = 0; iteration < ITERATIONS; iteration++){
        Avalanche_Stats stats = drop_grain(grid);
        grid_history.push_back(grid);
        mass_history.push_back(record_mass(grid));
        all_avalanche_stats.push_back(stats);
    }

    // save_grid_history(grid_history);
    // save_mass_history(mass_history);
    // save_avalanche_stats(all_avalanche_stats);
    
    std::cout << std::endl << std::endl;
    return 0;
}

Avalanche_Stats drop_grain(std::vector<std::vector<int>>& grid){
    int i = std::rand() % GRID_SIZE;
    int j = std::rand() % GRID_SIZE;
    grid[i][j] += 1;
    return topple(grid);
}

Avalanche_Stats topple(std::vector<std::vector<int>>& grid){
    // initialise avalanche stats
    std::set<std::pair<int, int>> toppled_sites;
    unsigned short energy = 0;
    unsigned short duration = 0;
    
    bool unstable = true;
    while(unstable){
        unstable = false;
        for(int i = 0; i < GRID_SIZE; i++){
            for(int j = 0; j < GRID_SIZE; j++){
                if(grid[i][j] >= 4){
                    grid[i][j] -= 4;
                    energy++;
                    toppled_sites.insert({i, j});
                    for(const auto& [dx, dy] : DIRECTIONS){
                        if(0 <= (i+dx) && (i+dx) < GRID_SIZE && 0 <= (j+dy) && (j+dy) < GRID_SIZE){
                            grid[i+dx][j+dy] += 1;
                        }
                    }
                    unstable = true;
                }
            }
        }
        if(unstable){
            duration++;
        }
    }
    return Avalanche_Stats{
        static_cast<unsigned short>(toppled_sites.size()),
        energy,
        duration
    };
}

int record_mass(std::vector<std::vector<int>>& grid){
    int mass = 0;
    for(int i = 0; i < GRID_SIZE; i++){
        for(int j = 0; j < GRID_SIZE; j++){
            mass += grid[i][j];
        }
    }
    return mass;
}

void save_grid_history(const std::vector<std::vector<std::vector<int>>>& grid_history){
    // saving grid data to file to visualise with python
    std::cout << "Writing Grid History" << std::endl;
    std::ofstream outfile("btw_model_grid_data.txt");
    if(outfile.is_open()){
        for(std::vector<std::vector<int>> grid : grid_history){
            for(std::vector<int> row : grid){
                for(int i : row){
                    outfile << i;
                }
                outfile << std::endl;
            }
        outfile << std::endl;
        }
        outfile.close();
        std::cout << "Data written to btw_model_grid_data.txt" << std::endl;
    }
    else{
        std::cout << "Error opening file";
    }
}

void save_mass_history(const std::vector<int>& mass_history){
    // saving mass data to file to plot with python
    std::cout << "Writing Mass History" << std::endl;
    std::ofstream outfile("btw_model_mass_data.txt");
    if(outfile.is_open()){
        for(int i = 0; i < mass_history.size(); i++){
            outfile << i << " " << mass_history[i] << std::endl;
        }
        outfile.close();
        std::cout << "Data written to btw_model_mass_data.txt" << std::endl;
    }
    else{
        std::cout << "Error opening file";
    }
}

void save_avalanche_stats(const std::vector<Avalanche_Stats>& stats){
    std::cout << "Writing Avalanche Stats" << std::endl;
    std::ofstream outfile("btw_model_avalanche_stats.txt");
    if(outfile.is_open()){
        for(int i = 0; i < stats.size(); i++){
            outfile << stats[i].size << ' ' << stats[i].energy << ' ' << stats[i].duration << std::endl;
        }
        outfile.close();
        std::cout << "Data written to btw_model_avalanche_stats_2.txt" << std::endl;
    }
    else{
        std::cout << "Error opening file";
    }
}