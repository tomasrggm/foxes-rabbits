#include "worldManager.h"


// Grid Position Checks

bool position_empty(int i, int j, struct entity* grid) {
    int place = (N * i + j);
    
    return grid[place].name == ' ';
}

bool position_empty_start(int i, int j, struct entity* grid) {
    int place = (N * i + j) - (pid * size);
    
    return grid[place].name == ' ';
}

bool within(int i, int j) {
    int position = i * N + j;
    int lower = pid * size;
    int higher = (pid + 1) * size;

    return (position >= lower && position < higher);
}

void insert_animal(int i, int j, char atype, struct entity* grid, struct entity* grid_temp) {
    int place = (N * i + j) - (pid * size);

    grid[place].name = atype;
    grid[place].age = 0;
    grid[place].lastGenEat = 0;
    grid[place].moved = 0;
    grid_temp[place].name = atype;
    grid_temp[place].age = 0;
    grid_temp[place].lastGenEat = 0;
    grid_temp[place].moved = 0;
}

bool has_rabbit(int i, int j, struct entity* grid) {
    return grid[i * N + j].name == 'R';
}

// ------------------------------------------------------------------------------


// World Simulation

float r4_uni(uint32_t* seed) {
    int seed_input, sseed;
    float r;

    seed_input = *seed;
    *seed ^= (*seed << 13);
    *seed ^= (*seed >> 17);
    *seed ^= (*seed << 5);
    sseed = *seed;
    r = 0.5 + 0.2328306e-09 * (seed_input + sseed);

    return r;
}

void generate_element(int n, char atype, uint32_t* seed) {
    int i, j, k;

    for(k = 0; k < n; k++) {
        i = M * r4_uni(seed);
        j = N * r4_uni(seed);
    
        if(within(i,j) && position_empty_start(i, j, grid_main))
            insert_animal(i, j, atype, grid_main, grid_temp_main);
    }
}

// ------------------------------------------------------------------------------