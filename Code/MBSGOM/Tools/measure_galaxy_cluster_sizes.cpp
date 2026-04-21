#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <fstream>
#include <sstream>

// USE cluster_size_analysis.py TO ANALYSE CLUSTER SIZE DISTRIBUTION FOR EACH TIMESTEP

const int NUMBER_OF_RINGS = 50;

// setup minimal Site struct for correlation data
struct Site{
    unsigned short r;
    unsigned short angle_index;
    int contains_star;
    bool seen;
};

// create vectors to store grid data
using Ring = std::vector<Site>;
using PolarGrid = std::vector<Ring>;
using GalaxyHistory = std::vector<PolarGrid>;

// function prototypes
void measure_cluster(PolarGrid& polar_grid, Site& site, int& cluster_size);
std::vector<std::pair<int, int>> find_neighbours(const unsigned short& r, const unsigned short& angle_index);
void save_clusters(std::vector<std::map<int, int>>& cluster_dist_history);

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

        // parse site (get 4 values)
        std::stringstream site_data(line);
        
        unsigned short r, angle_index;
        int size;
        int stellar_mass;

        site_data >> r >> angle_index >> size >> stellar_mass;

        Site site{r, angle_index, ((size > 0) ? 1 : 0), false};

        if(polar_grid.size() <= r){
            polar_grid.resize(r + 1);
        }

        polar_grid[r].push_back(site);
    }

    if(!polar_grid.empty()){
        galaxy_history.push_back(polar_grid);
    }

    std::cout << "Loaded " << galaxy_history.size() << " timesteps" << std::endl;

    // vector to store frequency of each cluster size (logarithmically binned later)
    std::vector<std::map<int, int>> cluster_dist_history;
    std::map<int, int> cluster_dist;
    std::vector<int> clusters;
    int cluster_size = 0;

    for(PolarGrid& polar_grid : galaxy_history){
        cluster_dist.clear();
        clusters.clear();
        cluster_dist.clear();
        clusters.clear();
        for(auto& ring : polar_grid){
            for(auto& site : ring){
                if(site.contains_star > 0 && !site.seen){
                    cluster_size = 1;
                    site.seen = true;
                    measure_cluster(polar_grid, polar_grid[site.r][site.angle_index], cluster_size);
                    clusters.push_back(cluster_size);
                }
            }
        }
        if(!clusters.empty()){
            for(const int& cluster : clusters){
                cluster_dist[cluster]++;
            }
        }
        else{
            // explicitly giving data to empty grid timesteps
            cluster_dist[0] = 0;
        }
        cluster_dist_history.push_back(cluster_dist);
    }
    
    
    save_clusters(cluster_dist_history);
    
    std::cout << std::endl << std::endl;
    return 0;
}

// measures clusters using a DFS described in the thesis
void measure_cluster(PolarGrid& polar_grid, Site& site, int& cluster_size){
    std::vector<std::pair<int, int>> nbrs = find_neighbours(site.r, site.angle_index);
    for(auto& [r, angle] : nbrs){
        if(polar_grid[r][angle].contains_star > 0 && !polar_grid[r][angle].seen){
            polar_grid[r][angle].seen = true;
            cluster_size++;
            measure_cluster(polar_grid, polar_grid[r][angle], cluster_size);
        }
    }
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

// saves cluster data to a file for analysis
void save_clusters(std::vector<std::map<int, int>>& cluster_dist_history){
    std::cout << "Writing Cluster Sizes" << std::endl;
    std::ofstream outfile("cluster_distribution_history.txt");
    if(outfile.is_open()){
        for(std::map<int, int> cluster_dist : cluster_dist_history){
            for(const auto& [value, freq] : cluster_dist){
                outfile << value << ' ' << freq << std::endl;
            }
            outfile << std::endl;
        }
        outfile.close();
        std::cout << "Data written to cluster_distribution_history.txt" << std::endl;
    }
    else{
        std::cout << "Error opening file" << std::endl;
    }
}