#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <omp.h>
#include <stddef.h>
#include <mpi.h>

MPI_Status status;
MPI_Datatype mpi_entity_type;
int pid,np;
int elementsPerProcess;

int M,N, sudoM;
int size;
int breedingAgeR;
int breedingAgeF;
int starvationAgeF;
struct entity* grid_main;
struct entity* grid_temp_main;

struct entity* ghost_low;
struct entity* ghost_high;
struct entity* ghost_low_temp;
struct entity* ghost_high_temp;

//global pointers
int* freeMoves;
int* eatMoves;
uint32_t seedW; 
//--------------------
// Animal types (or 'names').
char FOX = 'F';
char RABBIT = 'R';
char ROCK = '*';
//--------------------



typedef struct entity{
    char name;
    int age;
    int lastGenEat;
    int moved;
} ent;


bool position_empty_start(int i, int j,struct entity * grid){
    int place = (N*i + j) - (pid*size);
    
    return grid[place].name == ' ';
}
bool position_empty(int i, int j,struct entity * grid){
    int place = (N*i + j);
    
    return grid[place].name == ' ';
}


bool has_rabbit(int i,int j,struct entity * grid){
    return grid[i*N + j].name == 'R';
}


bool within(int i, int j){
    int position = i*N + j;
    int lower = pid * size;
    int higher = (pid+1) * size;
    return (position >= lower && position < higher);
}

int calculate_movement(int i, int j, int p){
    int aid = (M/np)*pid;
    int line = aid + i;
    int c = line * N + j;
    return c % p;
}

int calculate_movement_ghost(int i, int j, int p,int placeFlag){
    int aid = 0;
    if(placeFlag == 0){
        aid = (M/np)*(pid+1) -2;
    }else{
        aid = (M/np)*pid -2;
    }
    int line = aid + i;
    int c = line * N + j;
    return c % p;
}



void insert_animal(int i,int j, char atype,struct entity * grid,struct entity * grid_temp){
    int place = (N*i + j) - (pid*size);
    grid[place].name = atype;
    grid[place].age = 0;
    grid[place].lastGenEat = 0;
    grid[place].moved = 0;
    grid_temp[place].name = atype;
    grid_temp[place].age = 0;
    grid_temp[place].lastGenEat = 0;
    grid_temp[place].moved = 0;
}



float r4_uni(uint32_t *seed)
{
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



void generate_element(int n, char atype, uint32_t *seed)
{
    int i, j, k;

    for(k = 0; k < n; k++){
	i = M * r4_uni(seed);
	j = N * r4_uni(seed);
	if(within(i,j) && position_empty_start(i, j,grid_main))
	    insert_animal(i, j, atype,grid_main,grid_temp_main);
    }
}

void move_rabbit(int pos,int og_pos,struct entity * grid_temp){
    int bred = 0;

    struct entity * og_cell = &grid_temp[og_pos];
    struct entity * target_cell = &grid_temp[pos];
    

    if(og_cell->age >= breedingAgeR){
        og_cell->age = 0;
        bred++;
    }
    if(target_cell->name == ' '){
        target_cell->name = RABBIT;
        target_cell->age = og_cell->age;
    }
    else if(target_cell->name == FOX){
        target_cell->lastGenEat = 0;
    }
    else if(target_cell->name == RABBIT && og_cell->age > target_cell->age){
        target_cell->age = og_cell->age;
    }
    og_cell->age = 0;
    if(bred == 0){
        og_cell->name = ' ';
    }
    
    target_cell->moved = 1;

}

void move_fox(int pos,int og_pos,struct entity * grid_temp){
    int bred = 0;

    struct entity * og_cell = &grid_temp[og_pos];
    struct entity * target_cell = &grid_temp[pos];
    
    int starve = og_cell->lastGenEat;
    

    if(og_cell->age >= breedingAgeF){
        og_cell->age = 0;
        og_cell->lastGenEat = 0;
        bred++;
    }

    if(target_cell->name == ' '){
        target_cell->name = FOX;
        target_cell->age = og_cell->age;
        target_cell->lastGenEat = starve;
    }
    else if(target_cell->name == FOX){
        if(og_cell->age > target_cell->age){
            
            target_cell->age = og_cell->age;
            target_cell->lastGenEat = starve;
        }
        else if(target_cell->age == og_cell->age && target_cell->lastGenEat > starve){
            target_cell->lastGenEat = starve;
        }
    }
    else if(target_cell->name == RABBIT){
        target_cell->name = FOX;
        target_cell->age = og_cell->age;
        target_cell->lastGenEat = 0;
    }
    og_cell->age = 0;
    og_cell->lastGenEat = 0;
    if(bred == 0){
        og_cell->name = ' ';
    }

    target_cell->moved = 1;
}

void move_animal (int i, int j, int og_i, int og_j,struct entity * grid,struct entity * grid_temp) {
    int pos = N * i + j;
    int og_pos = N * og_i + og_j;
    if (has_rabbit(og_i, og_j,grid)) {
        move_rabbit(pos,og_pos,grid_temp);
    }else{
        move_fox(pos,og_pos,grid_temp);
    }
}

int* get_available_moves(int i, int j,struct entity * grid){
    bool isFox = grid[i*N + j].name == FOX;
    int movesR = 0;
    int moves = 0;
    for(int k = 0; k < 5;k++){
        freeMoves[k] = 0;
        eatMoves[k] = 0;
    }
    if(i != 0 && position_empty(i-1,j,grid)){
        freeMoves[0] = 1;
        moves++;
    }
    if(i != 0 && isFox && has_rabbit(i-1,j,grid)){
        eatMoves[0] = 1;
        movesR++;
    }
    if(i != sudoM-1 && position_empty(i+1,j,grid)){
        freeMoves[2] = 1;
        moves++;
    }
    if(i != sudoM-1 && isFox && has_rabbit(i+1,j,grid)){
        eatMoves[2] = 1;
        movesR++;
    }
    if(j != 0 && position_empty(i,j-1,grid)){
        freeMoves[3] = 1;
        moves++;
    }
    if(j != 0 && isFox && has_rabbit(i,j-1,grid)){
        eatMoves[3] = 1;
        movesR++;
    }
    if(j != N-1 && position_empty(i,j+1,grid)){
        freeMoves[1] = 1;
        moves++;
    }
    if(j != N-1 && isFox && has_rabbit(i,j+1,grid)){
        eatMoves[1] = 1;
        movesR++;
    }
    
    if(movesR > 0){
        eatMoves[4] = movesR;
        return eatMoves;
    }else{
        freeMoves[4] = moves;
        return freeMoves;
    }
}

void make_move(int i, int j, int*moves,int c,struct entity * grid,struct entity * grid_temp){
    int counter = -1;
    int availablemoves = -1;
    for(int i = 0; i < 4;i++){
        counter++;
        if(moves[i]!= 0){
            availablemoves++;
            if(availablemoves == c){
                break;
            }
        }
    }
    switch(counter){
        case 0:
            move_animal(i-1,j,i,j,grid,grid_temp);
            break;
        case 1:
            move_animal(i,j+1,i,j,grid,grid_temp);
            break;
        case 2:
            move_animal(i+1,j,i,j,grid,grid_temp);
            break;
        case 3:
            move_animal(i,j-1,i,j,grid,grid_temp);
            break;
    }
}

void run_sub_generation(int flag, struct entity* grid, struct entity* grid_temp){
    int p = 0;
    struct entity * ent; // sus (Check note below)
    int lines = ((M/np)*pid);
    if(np == 1){
        for(int k = 0; k < sudoM; k++){
            //printf("k eh %d em pid %d \n",k,pid);
            p = ((lines + k) % 2 == flag) ? 0 : 1;
            for(int j = p; j < N; j += 2){
                int current = k * N + j;
                ent = &grid[current];
                // kinda sus about the results with this tho
                if((ent->name == FOX || ent->name == RABBIT) && ent->moved == 0){
                    int* moves = get_available_moves(k,j,grid);
                    if(moves[4] > 0){
                        int c = calculate_movement(k,j,moves[4]);
                        make_move(k,j,moves,c,grid,grid_temp);
                    }
                }
            }
        }
    }else{
        if(pid == 0){ //se pid 0 ignorar ultima linha
            for(int k = 0; k < sudoM-1; k++){
                p = ((lines + k) % 2 == flag) ? 0 : 1;
                for(int j = p; j < N; j += 2){
                    int current = k * N + j;
                    ent = &grid[current];
                    if((ent->name == FOX || ent->name == RABBIT) && ent->moved == 0){
                        int* moves = get_available_moves(k,j,grid);
                        if(moves[4] > 0){
                            int c = calculate_movement(k,j,moves[4]);
                            make_move(k,j,moves,c,grid,grid_temp);
                        }
                    }
                }
            }
        }else if(pid == np-1){ //se pid for o ultimo ignorar a primeira linha
            for(int k = 1; k < sudoM; k++){
                p = ((lines + k) % 2 == flag) ? 0 : 1;
                for(int j = p; j < N; j += 2){
                    int current = k * N + j;
                    ent = &grid[current];
                    if((ent->name == FOX || ent->name == RABBIT) && ent->moved == 0){
                        int* moves = get_available_moves(k,j,grid);
                        if(moves[4] > 0){
                            int c = calculate_movement(k,j,moves[4]);
                            make_move(k,j,moves,c,grid,grid_temp);
                        }
                    }
                    
                }
            }
        }else{ //se pid for no meio ignorar primeira e ultima linha
            for(int k = 1; k < sudoM-1; k++){
                p = ((lines + k) % 2 == flag) ? 0 : 1;
                for(int j = p; j < N; j += 2){
                    int current = k * N + j;
                    ent = &grid[current];
                    if((ent->name == FOX || ent->name == RABBIT) && ent->moved == 0){
                        int* moves = get_available_moves(k,j,grid);
                        if(moves[4] > 0){
                            int c = calculate_movement(k,j,moves[4]);
                            make_move(k,j,moves,c,grid,grid_temp);
                        }
                    }
                }
            }
        }
    }
    
}

void run_sub_generation_ghost(int flag,int placeFlag,struct entity * ghost, struct entity * ghost_temp){
    int p = 0;
    struct entity * ent;
    int lines = 0;
    if(placeFlag == 0){
        lines = (M/np) * (pid+1) - 2;
    }else{
        lines =((M/np)*pid)-2;
    }
    for(int k = 1; k < 3; k++){
        p = ((lines + k) % 2 == flag) ? 0 : 1;
        for(int j = p; j < N; j += 2){
            int current = k * N + j;
            ent = &ghost[current];
            if((ent->name == FOX || ent->name == RABBIT) && ent->moved == 0){
                int* moves = get_available_moves(k,j,ghost);
                if(moves[4] > 0){
                    int c = calculate_movement_ghost(k,j,moves[4],placeFlag);
                    make_move(k,j,moves,c,ghost,ghost_temp);
                }
            }
        }
        
    }
}



void copy_red(){
    
    for(int i = 0; i < size; i++){
        grid_main[i] = grid_temp_main[i];
    }
    if(np > 1){
        if(pid < np-1){
            for(int i = 0; i < 4*N; i++){
                ghost_low[i] = ghost_low_temp[i];
            }
        }
        if(pid > 0){
            for(int i = 0; i < 4*N; i++){
                ghost_high[i] = ghost_high_temp[i];
            }
        }
    }
}

void copy_black(){

    struct entity * cell;
    struct entity * temp_cell;

    for(int i = 0; i < size; i++){

        cell = &grid_main[i];
        temp_cell = &grid_temp_main[i];
        
        cell->name = temp_cell->name;
        cell->age = temp_cell->age;
        cell->lastGenEat = temp_cell->lastGenEat;
        cell->moved = 0;
        temp_cell->moved = 0;
        if(cell->name == FOX && cell->lastGenEat == starvationAgeF){
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


void run_simulation(int n){
    struct entity * cell;
    struct entity * temp_cell;

    for(int k = 0; k < n; k++){
        for(int i = 0; i < size; i++){

            cell = &grid_main[i];
            temp_cell = &grid_temp_main[i];
            
            if(cell->name == FOX || cell->name == RABBIT){
                cell->age++;
                temp_cell->age++;
                if(cell->name == FOX){
                    cell->lastGenEat++;
                    temp_cell->lastGenEat++;
                }
            }
            
        }

        run_sub_generation(0,grid_main,grid_temp_main);
        
        if(pid < np-1){

            MPI_Send(&grid_temp_main[size-2*N], N*2, mpi_entity_type, pid+1, 0, MPI_COMM_WORLD);
            MPI_Recv(&ghost_low_temp[N*2], N*2, mpi_entity_type, pid+1, 0, MPI_COMM_WORLD,&status);

            MPI_Recv(&ghost_low[N*2], N*2, mpi_entity_type, pid+1, 0, MPI_COMM_WORLD,&status);
            MPI_Send(&grid_main[size-2*N], N*2, mpi_entity_type, pid+1, 0, MPI_COMM_WORLD);

            for(int i = 0; i < N*2;i++){
                ghost_low[i].age = grid_main[size-(2*N)+i].age;
                ghost_low[i].lastGenEat = grid_main[size-(2*N)+i].lastGenEat;
                ghost_low[i].moved = grid_main[size-(2*N)+i].moved;
                ghost_low[i].name = grid_main[size-(2*N)+i].name;

                ghost_low_temp[i].age = grid_temp_main[size-(2*N)+i].age;
                ghost_low_temp[i].lastGenEat = grid_temp_main[size-(2*N)+i].lastGenEat;
                ghost_low_temp[i].moved = grid_temp_main[size-(2*N)+i].moved;
                ghost_low_temp[i].name = grid_temp_main[size-(2*N)+i].name;
            }
            
            run_sub_generation_ghost(0,0,ghost_low,ghost_low_temp);

            for(int i = 0; i < N*2;i++){
                grid_temp_main[size-(2*N)+i].name = ghost_low_temp[i].name;
                grid_temp_main[size-(2*N)+i].moved = ghost_low_temp[i].moved;
                grid_temp_main[size-(2*N)+i].lastGenEat = ghost_low_temp[i].lastGenEat;
                grid_temp_main[size-(2*N)+i].age = ghost_low_temp[i].age;
            }


            /*for(int i = 0; i < 4; i++){
                printf("|");
                for(int j = 0; j < N; j++){
                    printf("%c|",ghost_low_temp[i*N+j].name);
                }
                printf("\n");
            }
            printf("\n");*/
        }
        if(pid > 0){
            
            
            MPI_Recv(&ghost_high_temp[0], N*2, mpi_entity_type, pid-1, 0, MPI_COMM_WORLD,&status);
            MPI_Send(&grid_temp_main[0], N*2, mpi_entity_type, pid-1, 0, MPI_COMM_WORLD);

            MPI_Send(&grid_main[0], N*2, mpi_entity_type, pid-1, 0, MPI_COMM_WORLD);
            MPI_Recv(&ghost_high[0], N*2, mpi_entity_type, pid-1, 0, MPI_COMM_WORLD,&status);
            
            for(int i = N*2; i < N*4;i++){
                ghost_high[i].name = grid_main[i - N*2].name;
                ghost_high[i].moved = grid_main[i - N*2].moved;
                ghost_high[i].lastGenEat = grid_main[i - N*2].lastGenEat;
                ghost_high[i].age = grid_main[i - N*2].age;

                ghost_high_temp[i].name = grid_temp_main[i - N*2].name;
                ghost_high_temp[i].moved = grid_temp_main[i - N*2].moved;
                ghost_high_temp[i].lastGenEat = grid_temp_main[i - N*2].lastGenEat;
                ghost_high_temp[i].age = grid_temp_main[i - N*2].age;
            }

            run_sub_generation_ghost(0,1,ghost_high,ghost_high_temp);
            
            for(int i = N*2; i < N*4;i++){
                grid_temp_main[i - N*2].name = ghost_high_temp[i].name;
                grid_temp_main[i - N*2].moved = ghost_high_temp[i].moved;
                grid_temp_main[i - N*2].lastGenEat = ghost_high_temp[i].lastGenEat;
                grid_temp_main[i - N*2].age = ghost_high_temp[i].age;
            }

        }
        
        copy_red();

        if(pid < np-1){
            run_sub_generation_ghost(1,0,ghost_low,ghost_low_temp);
            for(int i = 0; i < N*2;i++){
                grid_temp_main[size-(2*N)+i].age = ghost_low_temp[i].age;
                grid_temp_main[size-(2*N)+i].lastGenEat = ghost_low_temp[i].lastGenEat;
                grid_temp_main[size-(2*N)+i].moved = ghost_low_temp[i].moved;
                grid_temp_main[size-(2*N)+i].name = ghost_low_temp[i].name;
            }
        }
        if(pid > 0){
            run_sub_generation_ghost(1,1,ghost_high,ghost_high_temp);
            for(int i = N*2; i < N*4;i++){
                grid_temp_main[i - N*2].name = ghost_high_temp[i].name;
                grid_temp_main[i - N*2].moved = ghost_high_temp[i].moved;
                grid_temp_main[i - N*2].lastGenEat = ghost_high_temp[i].lastGenEat;
                grid_temp_main[i - N*2].age = ghost_high_temp[i].age;
            }
        }

        run_sub_generation(1,grid_main,grid_temp_main);
        copy_black();

    }
    
}

int main(int argc, char* argv[]){ //app name[0], gen[1], M[2], N[3], n-rocks[4], n-rabbits[5], rabbit breeding[6], n-foxes[7], fox breeding[8], fox starve[9], seed[10]
    
    //get parameters
    seedW = atoi(argv[10]);
    M = atoi(argv[2]);
    N = atoi(argv[3]);
    breedingAgeR = atoi(argv[6]);
    breedingAgeF = atoi(argv[8]);
    starvationAgeF = atoi(argv[9]);
    int nrocks = atoi(argv[4]);
    int nrabbits = atoi(argv[5]);
    int nfoxes = atoi(argv[7]);
    int ngen = atoi(argv[1]);
    size = M * N;



    MPI_Init(&argc,&argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    const int nitems=4;
    int blocklengths[4] = {1,1,1,1};
    MPI_Datatype types[4] = {MPI_BYTE, MPI_INT, MPI_INT, MPI_INT};
    MPI_Aint     offsets[4];

    offsets[0] = offsetof(ent, name);
    offsets[1] = offsetof(ent, age);
    offsets[2] = offsetof(ent, lastGenEat);
    offsets[3] = offsetof(ent, moved);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_entity_type);
    MPI_Type_commit(&mpi_entity_type);
    
    int sizePerBlock = size/np;

    

    if(pid < np-1){
        size = sizePerBlock;
    }else{
        int temp = size - (pid * sizePerBlock);
        size = temp;
    }


    //start grid
    ghost_low = malloc(sizeof(struct entity)* N*4);
    ghost_low_temp = malloc(sizeof(struct entity)* N*4);
    ghost_high_temp = malloc(sizeof(struct entity)* N*4);
    ghost_high = malloc(sizeof(struct entity)* N*4);

    grid_main = malloc(sizeof(struct entity) * size);
    grid_temp_main = malloc(sizeof(struct entity) * size);
    sudoM = size/N;

    freeMoves = malloc(sizeof(int)*5);
    eatMoves = malloc(sizeof(int)*5);
	
    for(int k = 0; k < 5;k++){
        freeMoves[k] = 0;
        eatMoves[k] = 0;
    }

    for(int k = 0; k < size;k++){
        grid_main[k].name = ' ';
        grid_main[k].age = 0;
        grid_main[k].lastGenEat = 0;
        grid_main[k].moved = 0;
        grid_temp_main[k].name = ' ';
        grid_temp_main[k].age = 0;
        grid_temp_main[k].lastGenEat = 0;
        grid_temp_main[k].moved = 0;
        
    }

    generate_element(nrocks,'*',&seedW);
    generate_element(nrabbits,'R',&seedW);
    generate_element(nfoxes,'F',&seedW);

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



     printf("%d %d %d\n",final_rocks,final_rabbits, final_foxes);



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