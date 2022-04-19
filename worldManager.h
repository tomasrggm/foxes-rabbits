#ifndef __WORLD_MANAGER_H__
#define __WORLD_MANAGER_H__

#include "common.h"


//Grid Position Checks
bool position_empty(int i, int j, struct entity* grid);
bool position_empty_start(int i, int j, struct entity* grid);
bool within(int i, int j);
void insert_animal(int i, int j, char atype, struct entity* grid, struct entity* grid_temp);
bool has_rabbit(int i, int j, struct entity* grid);

//World Generation
float r4_uni(uint32_t* seed);
void generate_element(int n, char atype, uint32_t* seed);


#endif