#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>

/*
USE MBSOGM_Visualiser.py TO VIEW THE EVOLUTION OF THE GALAXY
USE fractal_dimension_calculation.cpp TO CALCULATE FRACTAL DIMENSION AT EACH TIMESTEP
USE measure_galaxy_cluster_sizes.cpp TO MEASURE GALAXY CLUSTER SIZES AT EACH TIMESTEP
USE morans_calculation.cpp TO CALCULATE MORANS I AT EACH TIMESTEP
USE mass_component_plotting.py TO PLOT THE MASS COMPONENTS OF THE SYSTEM

improves the definition of mass
populates the grid with 0 to 1,000,000 solar masses
collapse threshold is set to the minimum mass of a GMC (10,000)
gas is distributed through its various phases here all of which can be found n the Site struct
*/

// constants to be used throughout the simulation
const int NUMBER_OF_RINGS = 50;
const int TIME_STEPS = 150;
const double PI = 3.14159265358979323846;
const double SPONTANEOUS_FORMATION = 0.001;

// find neighbours function prototype
std::vector<std::pair<int, int>> find_neighbours(const unsigned short& r, const unsigned short& angle_index);

// site object: contains all variables a site can posses as well as a member function to allow the site to undergo supernova collapse
struct Site{
    unsigned short r;
    unsigned short angle_index;
    double theta;
    unsigned int cold_gas;
    unsigned int low_mass_stars;
    unsigned int high_mass_stars;
    unsigned int sn_remnants;
    unsigned int heated_gas;
    bool contains_active_star;
    bool nearby_supernova;
    unsigned short lifetime;
    unsigned int new_stellar_mass;

    // member function to alter the various mass compinents and distribute heated gas to neighbouring sites
    void supernova(std::vector<std::vector<Site>>& polar_grid){
        unsigned int extra_returned_mass = 0.8*this->high_mass_stars;
        this->contains_active_star = false;
        this->nearby_supernova = false;
        this->sn_remnants += 0.2*this->high_mass_stars;
        this->high_mass_stars = 0;
        this->new_stellar_mass = 0;

        std::vector<std::pair<int, int>> nbrs = find_neighbours(r, angle_index);
        int N = (nbrs.size() >= 5) ? nbrs.size() : 6;
        unsigned int returned_mass_per_site = (this->heated_gas + extra_returned_mass) / N;
        this->heated_gas = 0;
        for(const auto& [r, angle] : nbrs){
            polar_grid[r][angle].heated_gas += returned_mass_per_site;
            polar_grid[r][angle].nearby_supernova = true;
        }
    }
};

// all other function prototypes
double myRand();
void populate_grid(std::vector<std::vector<Site>>& polar_grid);
void rotate(std::vector<std::vector<Site>>& polar_grid, std::vector<std::vector<unsigned long>>& galaxy_mass, std::string& galaxy_string, std::string& correlation_string);
void gmc_collapse(std::vector<std::vector<Site>>& polar_grid);
void save_galaxy_history(const std::vector<std::string>& galaxy_history);
void save_mass_history(const std::vector<std::vector<unsigned long>>& galaxy_mass);
void save_correlation_history(const std::vector<std::string>& correlation_history);

int main(){
    std::cout << std::endl << std::endl;

    // file strings to store data for later analysis
    std::vector<std::string> galaxy_history;
    galaxy_history.reserve(TIME_STEPS);
    std::vector<std::string> correlation_history;
    correlation_history.reserve(TIME_STEPS);

    // initialise polar grid vector
    std::vector<std::vector<Site>> polar_grid;
    populate_grid(polar_grid);
    // vector containing the mass components of the galaxy at each timestep
    std::vector<std::vector<unsigned long>> galaxy_mass;


    // begin simulation. each timestep represents 1 Myr
    for(int t = 0; t < TIME_STEPS; t++){
        std::string galaxy_string;
        galaxy_string.reserve(30000);
        std::string correlation_string;
        correlation_string.reserve(30000);

        rotate(polar_grid, galaxy_mass, galaxy_string, correlation_string);

        // all sites with active stars ready to supernova are added to a vector
        std::vector<Site*> exploding;
        for(auto& ring : polar_grid){
            for(auto& site : ring){
                if(site.contains_active_star && site.lifetime == 0){
                    exploding.push_back(&site);
                }
            }
        }
        // sites ready to supernova have their member function called
        for(auto* star : exploding){
            star->supernova(polar_grid);
        }
        gmc_collapse(polar_grid);

        // the grid geometry is saved to a vector for analysis
        galaxy_string.push_back('\n');
        correlation_string.push_back('\n');
        galaxy_history.push_back(std::move(galaxy_string));
        correlation_history.push_back(std::move(correlation_string));
    }

    // saving data to files
    save_galaxy_history(galaxy_history);
    save_mass_history(galaxy_mass);
    save_correlation_history(correlation_history);

    std::cout << std::endl << std::endl;
    return 0;
}

// generates a random number between 0 and 1 (excluding 0)
double myRand(){
    static thread_local std::mt19937_64 rng(std::random_device{}());
    static thread_local std::uniform_real_distribution<double> dist(0.0, 1.0);
    double x;
    do{
        x = dist(rng);
    }while(x == 0);

    return x;
}

// populates the grid with 0 to 1,000,000 solar masses randomly
void populate_grid(std::vector<std::vector<Site>>& polar_grid){
    std::vector<Site> ring;
    ring.push_back({0, 0, 0, 10000000, 0, 0, 0, 0, false, false, 0, 0});
    polar_grid.push_back(ring);

    for(int r = 1; r <= NUMBER_OF_RINGS; r++){
        int numStars = 6 * r;
        std::vector<Site> ring;
        // offset for theta to handle staggered placement of even radii
        double offset = (r % 2 == 0) ? static_cast<double>(PI / numStars) : 0.0;

        for(int site = 0; site < numStars; site++){
            double theta = static_cast<double>(2 * PI * site / numStars) + offset;
            unsigned int cold_gas = static_cast<unsigned int>(myRand() * 1000000);
            ring.push_back({static_cast<unsigned short>(r), static_cast<unsigned short>(site), theta, cold_gas, 0, 0, 0, 0, false, false, 0, 0});
        }
        polar_grid.push_back(ring);
    }
}

// seperates the grid into sectors which can be used to find a sites neighbours using some creative indexing
std::vector<std::pair<int, int>> find_neighbours(const unsigned short& r, const unsigned short& angle_index){
    int numStars = r * 6;
    std::vector<std::pair<int, int>> nbrs;

    // switch statement depending on where the site is located
    switch(r){
        // centre sites takes all of ring 1 (6 nbrs)
        case 0:{
            for(int i = 0; i < 6; i++){
                nbrs.push_back({1, i});
            }
            return nbrs;
        }
        // ring 1 takes two beside, two above and the centre cell (5 nbrs)
        case 1:{
            int sector = angle_index / r;
            nbrs.push_back({0, 0});
            nbrs.push_back({r, (angle_index - 1 + numStars) % numStars});
            nbrs.push_back({r, (angle_index + 1) % numStars});
            nbrs.push_back({r + 1, (angle_index + 1 + sector)});
            nbrs.push_back({r + 1, (angle_index + sector)});
            return nbrs;
        }
        // edge sites take 2 beside and two below (4 nbrs)
        case NUMBER_OF_RINGS:{
            int sector = angle_index / r;
            nbrs.push_back({r, (angle_index - 1 + numStars) % numStars});
            nbrs.push_back({r, (angle_index + 1) % numStars});
            nbrs.push_back({r - 1, (angle_index - 1 - sector + numStars - 6) % (numStars - 6)});
            nbrs.push_back({r - 1, (angle_index - sector + numStars - 6) % (numStars - 6)});
            return nbrs;
        }
        // all other sites take two beside, two above and two below (6 nbrs)
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

// GMCs collapse here and form a cluster of stars and leave behind other phases of gas mass
void gmc_collapse(std::vector<std::vector<Site>>& polar_grid){
    for(int r = 0; r < polar_grid.size(); r++){
        for(auto& site : polar_grid[r]){
            if(site.cold_gas >= 10000 && !site.contains_active_star && site.nearby_supernova){
                site.contains_active_star = true;
                site.lifetime = 10;
                site.new_stellar_mass = 0.1*site.cold_gas;
                site.low_mass_stars += 0.9*site.new_stellar_mass;
                site.high_mass_stars += 0.1*site.new_stellar_mass;
                site.heated_gas += 0.9*site.cold_gas;
                site.cold_gas = 0;
            }
        }
    }
}

// function to handle differential rotation and mass component tracking
void rotate(std::vector<std::vector<Site>>& polar_grid, std::vector<std::vector<unsigned long>>& galaxy_mass, std::string& galaxy_string, std::string& correlation_string){
    unsigned long total_cold_gas = polar_grid[0][0].cold_gas;
    unsigned long total_low_mass_stars = polar_grid[0][0].low_mass_stars;
    unsigned long total_high_mass_stars = polar_grid[0][0].high_mass_stars;
    unsigned long total_sn_remnants = polar_grid[0][0].sn_remnants;
    unsigned long total_heated_gas = polar_grid[0][0].heated_gas;

    // decrement the lifetime of active sites (centre case)
    if(polar_grid[0][0].contains_active_star && polar_grid[0][0].lifetime > 0){
        polar_grid[0][0].lifetime--;
    }

    // writing centre cell to galaxy history string
    galaxy_string.append(std::to_string(polar_grid[0][0].r));
    galaxy_string.push_back(' ');
    galaxy_string.append(std::to_string(polar_grid[0][0].theta));
    galaxy_string.push_back(' ');
    galaxy_string.append(std::to_string(polar_grid[0][0].new_stellar_mass));
    galaxy_string.push_back(' ');
    galaxy_string.append(std::to_string(polar_grid[0][0].low_mass_stars));
    galaxy_string.push_back('\n');

    // writing centre cell to correlation history string
    correlation_string.append(std::to_string(polar_grid[0][0].r));
    correlation_string.push_back(' ');
    correlation_string.append(std::to_string(polar_grid[0][0].angle_index));
    correlation_string.push_back(' ');
    correlation_string.append(std::to_string(polar_grid[0][0].high_mass_stars));
    correlation_string.push_back(' ');
    correlation_string.append(std::to_string(polar_grid[0][0].low_mass_stars));
    correlation_string.push_back('\n');

    for(int r = 1; r < polar_grid.size(); r++){
        int numStars = polar_grid[r].size();
        double offset = (r % 2 == 0) ? static_cast<double>(PI / numStars) : 0.0;

        // pushes last term of the vector to the start (rotation of a ring)
        std::rotate(polar_grid[r].begin(), polar_grid[r].end() - 1, polar_grid[r].end());

        // asigns new angle and angle index to the rotated sites of the ring
        for(int i = 0; i < numStars; i++){
            polar_grid[r][i].angle_index = i;
            polar_grid[r][i].theta = static_cast<double>(2 * PI * polar_grid[r][i].angle_index / numStars) + offset;
            // nearby supernova is used as a flag that allows for spontaneous formation and general collapse
            polar_grid[r][i].nearby_supernova = (myRand() <= SPONTANEOUS_FORMATION) ? true : false;
            // decrement the lifetime of all other active sites
            if(polar_grid[r][i].contains_active_star && polar_grid[r][i].lifetime > 0){
                polar_grid[r][i].lifetime--;
            }
            
            // accumulation of mass components in the current grid
            total_cold_gas += polar_grid[r][i].cold_gas;
            total_low_mass_stars += polar_grid[r][i].low_mass_stars;
            total_high_mass_stars += polar_grid[r][i].high_mass_stars;
            total_sn_remnants += polar_grid[r][i].sn_remnants;
            total_heated_gas += polar_grid[r][i].heated_gas;

            // writing to galaxy history string
            galaxy_string.append(std::to_string(polar_grid[r][i].r));
            galaxy_string.push_back(' ');
            galaxy_string.append(std::to_string(polar_grid[r][i].theta));
            galaxy_string.push_back(' ');
            galaxy_string.append(std::to_string(polar_grid[r][i].new_stellar_mass));
            galaxy_string.push_back(' ');
            galaxy_string.append(std::to_string(polar_grid[r][i].low_mass_stars));
            galaxy_string.push_back('\n');

            // writing to correlation history string
            correlation_string.append(std::to_string(polar_grid[r][i].r));
            correlation_string.push_back(' ');
            correlation_string.append(std::to_string(polar_grid[r][i].angle_index));
            correlation_string.push_back(' ');
            correlation_string.append(std::to_string(polar_grid[r][i].high_mass_stars));
            correlation_string.push_back(' ');
            correlation_string.append(std::to_string(polar_grid[r][i].low_mass_stars));
            correlation_string.push_back('\n');

        }
    }
    // creates a vector of the various mass components in the grid at a given timestep
    std::vector<unsigned long> current_masses = {
        total_cold_gas,
        total_low_mass_stars,
        total_high_mass_stars,
        total_sn_remnants,
        total_heated_gas,
        total_cold_gas + total_low_mass_stars + total_high_mass_stars + total_sn_remnants + total_heated_gas
    };
    // adds the vector of mass components to another vector for analysis
    galaxy_mass.push_back(current_masses);
}

// saves the grid geometry history for viewing
void save_galaxy_history(const std::vector<std::string>& galaxy_history){
    std::cout << "Writing Galaxy History" << std::endl;
    std::ofstream outfile("MBSOGM_I_history.txt");
    if (!outfile.is_open()) {
        std::cout << "Error opening file\n";
        return;
    }

    for (const auto& timestep : galaxy_history) {
        outfile << timestep;
    }

    outfile.close();
    std::cout << "Data written to MBSOGM_I_history.txt\n";
}

// saves the mass component history for plotting
void save_mass_history(const std::vector<std::vector<unsigned long>>& galaxy_mass){
    std::cout << "Writing Mass History" << std::endl;
    std::ofstream outfile("MBSOGM_I_mass_history.txt");
    if(outfile.is_open()){
        for(int i = 0; i < galaxy_mass.size(); i++){
            outfile << i << ' ';
            for(int j = 0; j < galaxy_mass[i].size(); j++){
                outfile << galaxy_mass[i][j] << ' ';
            }
            outfile << std::endl;
        }
        outfile.close();
        std::cout << "Data written to MBSOGM_I_mass_history.txt" << std::endl;
    }
    else{
        std::cout << "Error opening file" << std::endl;
    }
}

// saves the correlation data for further analysis with other tools developed in C++
void save_correlation_history(const std::vector<std::string>& correlation_history){
    std::cout << "Writing Correlation History" << std::endl;
    std::ofstream outfile("MBSOGM_I_correlation_history.txt");
    if (!outfile.is_open()) {
        std::cout << "Error opening file\n";
        return;
    }

    for (const auto& timestep : correlation_history) {
        outfile << timestep;
    }

    outfile.close();
    std::cout << "Data written to MBSOGM_I_correlation_history.txt\n";
}
