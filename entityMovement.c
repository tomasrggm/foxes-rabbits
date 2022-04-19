#include "entityMovement.h"


int* get_available_moves(int i, int j, struct entity* grid) {
    bool isFox = grid[i * N + j].name == FOX;
    int movesR = 0;
    int moves = 0;

    for(int k = 0; k < 5;k++) {
        freeMoves[k] = 0;
        eatMoves[k] = 0;
    }

    if(i != 0 && position_empty(i - 1, j, grid)) {
        freeMoves[0] = 1;
        moves++;
    }
    if(i != 0 && isFox && has_rabbit(i -1, j, grid)) {
        eatMoves[0] = 1;
        movesR++;
    }
    if(i != sudoM - 1 && position_empty(i + 1, j, grid)) {
        freeMoves[2] = 1;
        moves++;
    }
    if(i != sudoM - 1 && isFox && has_rabbit(i + 1, j, grid)) {
        eatMoves[2] = 1;
        movesR++;
    }
    if(j != 0 && position_empty(i, j - 1, grid)) {
        freeMoves[3] = 1;
        moves++;
    }
    if(j != 0 && isFox && has_rabbit(i, j - 1, grid)) {
        eatMoves[3] = 1;
        movesR++;
    }
    if(j != N - 1 && position_empty(i, j + 1, grid)) {
        freeMoves[1] = 1;
        moves++;
    }
    if(j != N - 1 && isFox && has_rabbit(i, j + 1, grid)) {
        eatMoves[1] = 1;
        movesR++;
    }
    
    if(movesR > 0) {
        eatMoves[4] = movesR;
        return eatMoves;
    }

    freeMoves[4] = moves;

    return freeMoves;
}


int calculate_movement(int i, int j, int p) {
    int aid = (M / np) * pid;
    int line = aid + i;
    int c = line * N + j;

    return c % p;
}

int calculate_movement_ghost(int i, int j, int p, int placeFlag) {
    int aid = 0;
    
    if(placeFlag == 0)
        aid = (M / np) * (pid + 1) - 2;
    else
        aid = (M / np) * pid - 2;
    
    int line = aid + i;
    int c = line * N + j;
    
    return c % p;
}


void make_move(int i, int j, int* moves, int c, struct entity* grid, struct entity* grid_temp) {
    int counter = -1;
    int availablemoves = -1;

    for(int i = 0; i < 4; i++) {
        counter++;

        if(moves[i]!= 0) {
            availablemoves++;

            if(availablemoves == c)
                break;
        }
    }

    switch(counter) {
        case 0:
            move_animal(i - 1, j, i, j, grid, grid_temp);
            break;
        case 1:
            move_animal(i, j + 1, i, j, grid, grid_temp);
            break;
        case 2:
            move_animal(i + 1, j, i, j, grid, grid_temp);
            break;
        case 3:
            move_animal(i, j - 1, i, j, grid, grid_temp);
            break;
    }
}


void move_animal(int i, int j, int og_i, int og_j, struct entity* grid, struct entity* grid_temp) {
    int pos = N * i + j;
    int og_pos = N * og_i + og_j;
    
    if(has_rabbit(og_i, og_j, grid))
        move_rabbit(pos, og_pos, grid_temp);
    else
        move_fox(pos, og_pos, grid_temp);
}

void move_rabbit(int pos, int og_pos, struct entity* grid_temp) {
    int bred = 0;

    struct entity * og_cell = &grid_temp[og_pos];
    struct entity * target_cell = &grid_temp[pos];
    

    if(og_cell->age >= breedingAgeR) {
        og_cell->age = 0;
        bred++;
    }

    if(target_cell->name == ' ') {
        target_cell->name = RABBIT;
        target_cell->age = og_cell->age;
    } else if(target_cell->name == FOX) {
        target_cell->lastGenEat = 0;
    } else if(target_cell->name == RABBIT && og_cell->age > target_cell->age) {
        target_cell->age = og_cell->age;
    }

    og_cell->age = 0;

    if(bred == 0)
        og_cell->name = ' ';
    
    target_cell->moved = 1;

}

void move_fox(int pos, int og_pos, struct entity* grid_temp) {
    int bred = 0;

    struct entity * og_cell = &grid_temp[og_pos];
    struct entity * target_cell = &grid_temp[pos];
    
    int starve = og_cell->lastGenEat;
    

    if(og_cell->age >= breedingAgeF) {
        og_cell->age = 0;
        og_cell->lastGenEat = 0;
        bred++;
    }

    if(target_cell->name == ' ') {
        target_cell->name = FOX;
        target_cell->age = og_cell->age;
        target_cell->lastGenEat = starve;
    } else if(target_cell->name == FOX) {
        if(og_cell->age > target_cell->age) {
            target_cell->age = og_cell->age;
            target_cell->lastGenEat = starve;
        } else if(target_cell->age == og_cell->age && target_cell->lastGenEat > starve) {
            target_cell->lastGenEat = starve;
        }
    } else if(target_cell->name == RABBIT) {
        target_cell->name = FOX;
        target_cell->age = og_cell->age;
        target_cell->lastGenEat = 0;
    }

    og_cell->age = 0;
    og_cell->lastGenEat = 0;

    if(bred == 0)
        og_cell->name = ' ';

    target_cell->moved = 1;
}