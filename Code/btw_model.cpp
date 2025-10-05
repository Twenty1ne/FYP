#include <iostream>
#include <vector>
#include <fstream>

// USE btw_model_visulaisation.py TO VIEW EVOLUTION OF MODEL
// USE file_plotting.py TO VIEW EVOLUTION OF THE MASS OF MODEL

const int GRID_SIZE = 10;
const int ITERATIONS = 100;
const std::vector<std::pair<int, int>> DIRECTIONS = {
    {0, -1},
    {0, 1},
    {-1, 0},
    {1, 0}
};

// function prototypes
void drop_grain(std::vector<std::vector<int>>& grid);
void topple(std::vector<std::vector<int>>& grid);
int record_mass(std::vector<std::vector<int>>& grid);
void save_grid_history(const std::vector<std::vector<std::vector<int>>>& grid_history);
void save_mass_history(const std::vector<int>& mass_history);

int main(){
    std::cout << std::endl << std::endl;
    // initialise variables
    std::vector<std::vector<std::vector<int>>> grid_history;
    std::vector<int> mass_history = {0};

    // build grid
    std::vector<std::vector<int>> grid(GRID_SIZE, std::vector<int>(GRID_SIZE, 0));
    grid_history.push_back(grid);

    for(std::vector<int> row : grid){
        for(int element : row){
            std::cout << element << ' ';
        }
        std::cout << std::endl;
    }

    for(int trial = 0; trial < ITERATIONS; trial++){
        drop_grain(grid);
        grid_history.push_back(grid);
        mass_history.push_back(record_mass(grid));
    }

    // save_grid_history(grid_history);
    // save_mass_history(mass_history);
    
    std::cout << std::endl << std::endl;
    return 0;
}

void drop_grain(std::vector<std::vector<int>>& grid){
    grid[GRID_SIZE/2][GRID_SIZE/2] += 1;
    topple(grid);
}

void topple(std::vector<std::vector<int>>& grid){
    bool unstable = true;
    while(unstable){
        unstable = false;
        for(int i = 0; i < GRID_SIZE; i++){
            for(int j = 0; j < GRID_SIZE; j++){
                if(grid[i][j] >= 4){
                    grid[i][j] -= 4;
                    for(const auto& [dx, dy] : DIRECTIONS){
                        if(0 <= (i+dx) && (i+dx) < GRID_SIZE && 0 <= (j+dy) && (j+dy) < GRID_SIZE){
                            grid[i+dx][j+dy] += 1;
                        }
                    }
                    unstable = true;
                }
            }
        }
    }
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
    std::ofstream outfile("btw_model_data_test.txt");
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
        std::cout << "Data written to btw_model_data_small.txt";
    }
    else{
        std::cout << "Error opening file";
    }
}

void save_mass_history(const std::vector<int>& mass_history){
    // saving mass data to file to plot with python
    std::ofstream outfile("btw_model_mass_data.txt");
    if(outfile.is_open()){
        for(int i = 0; i < mass_history.size(); i++){
            outfile << i << " " << mass_history[i] << std::endl;
        }
        outfile.close();
        std::cout << "Data written to btw_model_mass_data.txt";
    }
    else{
        std::cout << "Error opening file";
    }
}