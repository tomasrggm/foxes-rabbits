#include "common.h"


//World Generation
uint32_t seedW;


//MPI
int pid, np;
int elementsPerProcess;

MPI_Status status;


//Entities
MPI_Datatype mpi_entity_type;

int breedingAgeR;
int breedingAgeF;
int starvationAgeF;


//Movement
int* freeMoves;
int* eatMoves;


//Grids
int M, N, sudoM;
int size;

struct entity* grid_main;
struct entity* grid_temp_main;

struct entity* ghost_low;
struct entity* ghost_high;
struct entity* ghost_low_temp;
struct entity* ghost_high_temp;

char FOX = 'F';
char RABBIT = 'R';
char ROCK = '*';