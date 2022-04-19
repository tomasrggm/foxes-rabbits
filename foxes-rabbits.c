#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <omp.h>
#include <stddef.h>
#include <mpi.h>
#include "common.h"
#include "worldManager.h"
#include "entityMovement.h"
#include "simulationRunner.h"


int main(int argc, char* argv[]){ //app name[0], gen[1], M[2], N[3], n-rocks[4], n-rabbits[5], rabbit breeding[6], n-foxes[7], fox breeding[8], fox starve[9], seed[10]
    
    //get parameters
    seedW = atoi(argv[10]);

    int ngen = atoi(argv[1]);

    M = atoi(argv[2]);
    N = atoi(argv[3]);
    size = M * N;

    breedingAgeR = atoi(argv[6]);
    breedingAgeF = atoi(argv[8]);
    starvationAgeF = atoi(argv[9]);

    int nrocks = atoi(argv[4]);
    int nrabbits = atoi(argv[5]);
    int nfoxes = atoi(argv[7]);
    



    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    const int nitems = 4;
    int blocklengths[4] = {1, 1, 1, 1};
    MPI_Datatype types[4] = {MPI_BYTE, MPI_INT, MPI_INT, MPI_INT};
    MPI_Aint     offsets[4];

    offsets[0] = offsetof(ent, name);
    offsets[1] = offsetof(ent, age);
    offsets[2] = offsetof(ent, lastGenEat);
    offsets[3] = offsetof(ent, moved);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_entity_type);
    MPI_Type_commit(&mpi_entity_type);
    
    int sizePerBlock = size / np;

    

    if(pid < np - 1) {
        size = sizePerBlock;
    } else {
        int temp = size - (pid * sizePerBlock);

        size = temp;
    }


    //start grid
    ghost_low = malloc(sizeof(struct entity) * N * 4);
    ghost_low_temp = malloc(sizeof(struct entity) * N * 4);
    ghost_high_temp = malloc(sizeof(struct entity) * N * 4);
    ghost_high = malloc(sizeof(struct entity) * N * 4);

    grid_main = malloc(sizeof(struct entity) * size);
    grid_temp_main = malloc(sizeof(struct entity) * size);
    sudoM = size / N;

    freeMoves = malloc(sizeof(int) * 5);
    eatMoves = malloc(sizeof(int) * 5);
    
    for(int k = 0; k < 5; k++) {
        freeMoves[k] = 0;
        eatMoves[k] = 0;
    }

    for(int k = 0; k < size; k++) {
        grid_main[k].name = ' ';
        grid_main[k].age = 0;
        grid_main[k].lastGenEat = 0;
        grid_main[k].moved = 0;
        grid_temp_main[k].name = ' ';
        grid_temp_main[k].age = 0;
        grid_temp_main[k].lastGenEat = 0;
        grid_temp_main[k].moved = 0;
        
    }

    generate_element(nrocks,'*', &seedW);
    generate_element(nrabbits,'R', &seedW);
    generate_element(nfoxes,'F', &seedW);

    double exec_time;
    exec_time = -omp_get_wtime();
    run_simulation(ngen);
    exec_time += omp_get_wtime();
    fprintf(stderr, "%.1fs\n", exec_time);

    MPI_Finalize();

    int final_rabbits = 0;
    int final_foxes = 0;
    int final_rocks = 0;

    for (int i = 0; i < size; i++) {
        if (grid_main[i].name == FOX)
            final_foxes++;
        else if (grid_main[i].name == RABBIT)
            final_rabbits++;
        else if (grid_main[i].name == ROCK)
            final_rocks++;  
    }



    printf("%d %d %d\n", final_rocks, final_rabbits, final_foxes);



    free(freeMoves);
    free(eatMoves);
    free(grid_main);
    free(grid_temp_main);
    free(ghost_high);
    free(ghost_high_temp);
    free(ghost_low);
    free(ghost_low_temp);
    
    return 0;
}