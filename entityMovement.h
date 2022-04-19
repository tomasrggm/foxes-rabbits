#ifndef __ENTITY_MOVEMENT_H__
#define __ENTITY_MOVEMENT_H__

#include "common.h"
#include "worldManager.h"

int* get_available_moves(int i, int j, struct entity* grid);

int calculate_movement(int i, int j, int p);
int calculate_movement_ghost(int i, int j, int p, int placeFlag);

void make_move(int i, int j, int* moves, int c, struct entity* grid, struct entity* grid_temp);

void move_animal(int i, int j, int og_i, int og_j, struct entity* grid, struct entity* grid_temp);
void move_rabbit(int pos, int og_pos, struct entity* grid_temp);
void move_fox(int pos, int og_pos, struct entity* grid_temp);



#endif