#ifndef __COMMON_H__
#define __COMMON_H__


#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <omp.h>
#include <stddef.h>
#include <mpi.h>


//World Generation
extern uint32_t seedW;
 

//MPI
extern int pid, np;
extern int elementsPerProcess;

extern MPI_Status status; 

// Entities
typedef struct entity{
    char name;
    int age;
    int lastGenEat;
    int moved;
} ent;
extern MPI_Datatype mpi_entity_type;

extern int breedingAgeR;
extern int breedingAgeF;
extern int starvationAgeF;

// Movement
extern int* freeMoves;
extern int* eatMoves;
 
// Grids
extern int M, N, sudoM;
extern int size;

extern struct entity* grid_main;
extern struct entity* grid_temp_main;
 
extern struct entity* ghost_low;
extern struct entity* ghost_high;
extern struct entity* ghost_low_temp;
extern struct entity* ghost_high_temp;

extern char FOX;
extern char RABBIT;
extern char ROCK;

#endif