#ifndef __SIMULATION_RUNNER_H__
#define __SIMULATION_RUNNER_H__

#include "common.h"
#include "entityMovement.h"

//World Simulation
void copy_red();
void copy_black();

void run_sub_generation(int flag, struct entity* grid, struct entity* grid_temp);
void run_sub_generation_ghost(int flag, int placeFlag, struct entity* ghost, struct entity* ghost_temp);

void run_simulation(int n);

#endif