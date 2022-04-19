#include "simulationRunner.h"


void copy_red() {
    
    for(int i = 0; i < size; i++)
        grid_main[i] = grid_temp_main[i];
    
    
    if(np > 1) {

        if(pid < np - 1)
            for(int i = 0; i < 4 * N; i++)
                ghost_low[i] = ghost_low_temp[i];
            
        
        if(pid > 0)
            for(int i = 0; i < 4 * N; i++)
                ghost_high[i] = ghost_high_temp[i];
            
    }
    
}

void copy_black() {
    struct entity* cell;
    struct entity* temp_cell;

    for(int i = 0; i < size; i++) {
        cell = &grid_main[i];
        temp_cell = &grid_temp_main[i];
        
        cell->name = temp_cell->name;
        cell->age = temp_cell->age;
        cell->lastGenEat = temp_cell->lastGenEat;
        cell->moved = 0;
        temp_cell->moved = 0;

        if(cell->name == FOX && cell->lastGenEat == starvationAgeF) {
            cell->name = ' ';
            cell->lastGenEat = 0;
            cell->age = 0;
            cell->moved = 0;
            temp_cell->name = ' ';
            temp_cell->lastGenEat = 0;
            temp_cell->age = 0;
            temp_cell->moved = 0;
        }
        
    }
}


void run_sub_generation(int flag, struct entity* grid, struct entity* grid_temp) {
    int p = 0;
    struct entity* ent; // sus (Check note below)
    int lines = ((M / np) * pid);

    if(np == 1) {
        for(int k = 0; k < sudoM; k++) {
            //printf("k eh %d em pid %d \n",k,pid);
            p = ((lines + k) % 2 == flag) ? 0 : 1;

            for(int j = p; j < N; j += 2) {
                int current = k * N + j;

                ent = &grid[current];
                // kinda sus about the results with this tho
                if((ent->name == FOX || ent->name == RABBIT) && ent->moved == 0) {
                    int* moves = get_available_moves(k, j, grid);

                    if(moves[4] > 0) {
                        int c = calculate_movement(k, j, moves[4]);

                        make_move(k, j, moves, c, grid, grid_temp);
                    }
                }
            }
        }

    } else {
        if(pid == 0) { //se pid 0 ignorar ultima linha
            for(int k = 0; k < sudoM - 1; k++) {
                p = ((lines + k) % 2 == flag) ? 0 : 1;

                for(int j = p; j < N; j += 2) {
                    int current = k * N + j;

                    ent = &grid[current];
                    if((ent->name == FOX || ent->name == RABBIT) && ent->moved == 0) {
                        int* moves = get_available_moves(k, j, grid);

                        if(moves[4] > 0){
                            int c = calculate_movement(k, j, moves[4]);

                            make_move(k, j, moves, c, grid, grid_temp);
                        }
                    }
                }
            }

        } else if(pid == np - 1) { //se pid for o ultimo ignorar a primeira linha
            for(int k = 1; k < sudoM; k++) {
                p = ((lines + k) % 2 == flag) ? 0 : 1;

                for(int j = p; j < N; j += 2) {
                    int current = k * N + j;

                    ent = &grid[current];
                    if((ent->name == FOX || ent->name == RABBIT) && ent->moved == 0) {
                        int* moves = get_available_moves(k, j, grid);

                        if(moves[4] > 0) {
                            int c = calculate_movement(k, j, moves[4]);

                            make_move(k, j, moves, c, grid, grid_temp);
                        }
                    }
                    
                }
            }

        } else { //se pid for no meio ignorar primeira e ultima linha
            for(int k = 1; k < sudoM - 1; k++) {
                p = ((lines + k) % 2 == flag) ? 0 : 1;

                for(int j = p; j < N; j += 2) {
                    int current = k * N + j;

                    ent = &grid[current];
                    if((ent->name == FOX || ent->name == RABBIT) && ent->moved == 0) {
                        int* moves = get_available_moves(k, j, grid);

                        if(moves[4] > 0) {
                            int c = calculate_movement(k, j, moves[4]);

                            make_move(k, j, moves, c, grid, grid_temp);
                        }
                    }
                }
            }
        }
    }
    
}

void run_sub_generation_ghost(int flag, int placeFlag, struct entity* ghost, struct entity* ghost_temp) {
    int p = 0;
    struct entity* ent;
    int lines = 0;

    if(placeFlag == 0)
        lines = (M / np) * (pid + 1) - 2;
    else
        lines = ((M / np) * pid) - 2;


    for(int k = 1; k < 3; k++) {
        p = ((lines + k) % 2 == flag) ? 0 : 1;

        for(int j = p; j < N; j += 2) {
            int current = k * N + j;

            ent = &ghost[current];
            if((ent->name == FOX || ent->name == RABBIT) && ent->moved == 0) {
                int* moves = get_available_moves(k, j, ghost);

                if(moves[4] > 0) {
                    int c = calculate_movement_ghost(k, j, moves[4], placeFlag);

                    make_move(k, j, moves, c, ghost, ghost_temp);
                }
            }
        }
        
    }
}


void run_simulation(int n) {
    struct entity* cell;
    struct entity* temp_cell;

    for(int k = 0; k < n; k++) {
        for(int i = 0; i < size; i++) {

            cell = &grid_main[i];
            temp_cell = &grid_temp_main[i];
            
            if(cell->name == FOX || cell->name == RABBIT) {
                cell->age++;
                temp_cell->age++;

                if(cell->name == FOX) {
                    cell->lastGenEat++;
                    temp_cell->lastGenEat++;
                }
            }
            
        }

        run_sub_generation(0, grid_main, grid_temp_main);
        
        if(pid < np - 1) {

            MPI_Send(&grid_temp_main[size - 2 * N], N * 2, mpi_entity_type, pid + 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&ghost_low_temp[N * 2], N * 2, mpi_entity_type, pid + 1, 0, MPI_COMM_WORLD, &status);

            MPI_Recv(&ghost_low[N * 2], N * 2, mpi_entity_type, pid + 1, 0, MPI_COMM_WORLD, &status);
            MPI_Send(&grid_main[size - 2 * N], N * 2, mpi_entity_type, pid + 1, 0, MPI_COMM_WORLD);

            for(int i = 0; i < N * 2; i++) {
                ghost_low[i].age = grid_main[size - (2 * N) + i].age;
                ghost_low[i].lastGenEat = grid_main[size - (2 * N) + i].lastGenEat;
                ghost_low[i].moved = grid_main[size - (2 * N) + i].moved;
                ghost_low[i].name = grid_main[size - (2 * N) + i].name;

                ghost_low_temp[i].age = grid_temp_main[size - (2 * N) + i].age;
                ghost_low_temp[i].lastGenEat = grid_temp_main[size - (2 * N) + i].lastGenEat;
                ghost_low_temp[i].moved = grid_temp_main[size - (2 * N) + i].moved;
                ghost_low_temp[i].name = grid_temp_main[size - (2 * N) + i].name;
            }
            
            run_sub_generation_ghost(0, 0, ghost_low, ghost_low_temp);

            for(int i = 0; i < N * 2; i++) {
                grid_temp_main[size - (2 * N) + i].name = ghost_low_temp[i].name;
                grid_temp_main[size - (2 * N) + i].moved = ghost_low_temp[i].moved;
                grid_temp_main[size - (2 * N) + i].lastGenEat = ghost_low_temp[i].lastGenEat;
                grid_temp_main[size - (2 * N) + i].age = ghost_low_temp[i].age;
            }


        }

        if(pid > 0) {
            
            
            MPI_Recv(&ghost_high_temp[0], N * 2, mpi_entity_type, pid - 1, 0, MPI_COMM_WORLD, &status);
            MPI_Send(&grid_temp_main[0], N * 2, mpi_entity_type, pid - 1, 0, MPI_COMM_WORLD);

            MPI_Send(&grid_main[0], N * 2, mpi_entity_type, pid - 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&ghost_high[0], N * 2, mpi_entity_type, pid - 1, 0, MPI_COMM_WORLD, &status);
            
            for(int i = N * 2; i < N * 4; i++) {
                ghost_high[i].name = grid_main[i - N * 2].name;
                ghost_high[i].moved = grid_main[i - N * 2].moved;
                ghost_high[i].lastGenEat = grid_main[i - N * 2].lastGenEat;
                ghost_high[i].age = grid_main[i - N * 2].age;

                ghost_high_temp[i].name = grid_temp_main[i - N * 2].name;
                ghost_high_temp[i].moved = grid_temp_main[i - N * 2].moved;
                ghost_high_temp[i].lastGenEat = grid_temp_main[i - N * 2].lastGenEat;
                ghost_high_temp[i].age = grid_temp_main[i - N * 2].age;
            }

            run_sub_generation_ghost(0, 1, ghost_high, ghost_high_temp);
            
            for(int i = N * 2; i < N * 4;i++) {
                grid_temp_main[i - N * 2].name = ghost_high_temp[i].name;
                grid_temp_main[i - N * 2].moved = ghost_high_temp[i].moved;
                grid_temp_main[i - N * 2].lastGenEat = ghost_high_temp[i].lastGenEat;
                grid_temp_main[i - N * 2].age = ghost_high_temp[i].age;
            }

        }
        
        copy_red();

        if(pid < np - 1) {
            run_sub_generation_ghost(1, 0, ghost_low, ghost_low_temp);
            for(int i = 0; i < N * 2; i++) {
                grid_temp_main[size - (2 * N) + i].age = ghost_low_temp[i].age;
                grid_temp_main[size - (2 * N) + i].lastGenEat = ghost_low_temp[i].lastGenEat;
                grid_temp_main[size - (2 * N) + i].moved = ghost_low_temp[i].moved;
                grid_temp_main[size - (2 * N) + i].name = ghost_low_temp[i].name;
            }
        }

        if(pid > 0) {
            run_sub_generation_ghost(1, 1, ghost_high, ghost_high_temp);
            for(int i = N * 2; i < N * 4; i++) {
                grid_temp_main[i - N * 2].name = ghost_high_temp[i].name;
                grid_temp_main[i - N * 2].moved = ghost_high_temp[i].moved;
                grid_temp_main[i - N * 2].lastGenEat = ghost_high_temp[i].lastGenEat;
                grid_temp_main[i - N * 2].age = ghost_high_temp[i].age;
            }
        }

        run_sub_generation(1, grid_main, grid_temp_main);
        copy_black();

    }
    
}