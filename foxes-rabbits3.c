#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <omp.h>

int M,N;
int breedingAgeR;
int breedingAgeF;
int starvationAgeF;
struct entity** grid;
struct entity** grid_temp;
uint32_t seedW; 

// Animal types (or 'names').
char FOX = 'F';
char RABBIT = 'R';
char ROCK = '*';


struct entity{
    char name;
    int age;
    int lastGenEat;
};


bool position_empty(int i, int j){
    return grid[i][j].name == ' ' || grid[i][j].name == 'V' || grid[i][j].name == 'Z';
}
bool has_rabbit(int i,int j){
    return grid[i][j].name == 'R';
}

int calculate_movement(int i, int j, int p){
    int c = i * N + j;
    return c % p;
}


void worldPrinter(){
    for(int k = 0; k < N; k++){
        for(int j = 0; j < M; j++){
            printf("%c %d|",grid[k][j].name,grid[k][j].age);
        }
        printf("\n");
    }
    printf("\n");
}







void insert_animal(int i,int j, char atype){
    grid[i][j].name = atype;
    grid[i][j].age = 0;
    grid[i][j].lastGenEat = 0;
    grid_temp[i][j].name = atype;
    grid_temp[i][j].age = 0;
    grid_temp[i][j].lastGenEat = 0;
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
	if(position_empty(i, j))
	    insert_animal(i, j, atype);
    }
}

void move_rabbit(int i,int j,int og_i,int og_j){

}

void move_fox(int i,int j,int og_i,int og_j){

}

void move_animal (int i, int j, int og_i, int og_j) {
    if (has_rabbit(og_i, og_j)) {
        move_rabbit(i,j,og_i,og_j);
    }else{
        move_fox(i,j,og_i,og_j);
    }
}

int* get_available_moves(int i, int j){
    bool isFox = grid[i][j].name == FOX;
    int* freeMoves = malloc(sizeof(int)*5);
    int* eatMoves = malloc(sizeof(int)*5);
    int movesR = 0;
    int moves = 0;
    if(i != 0 && position_empty(i-1,j)){
        freeMoves[0] = 1;
        moves++;
    }
    if(i != N-1 && position_empty(i+1,j)){
        freeMoves[2] = 1;
        moves++;
    }
    if(j != 0 && position_empty(i,j-1)){
        freeMoves[3] = 1;
        moves++;
    }
    if(j != M-1 && position_empty(i,j+1)){
        freeMoves[1] = 1;
        moves++;
    }
    if(isFox && has_rabbit(i-1,j)){
        eatMoves[0] = 1;
        movesR++;
    }
    if(isFox && has_rabbit(i+1,j)){
        eatMoves[2] = 1;
        movesR++;
    }
    if(isFox && has_rabbit(i,j-1)){
        eatMoves[3] = 1;
        movesR++;
    }
    if(isFox && has_rabbit(i-1,j+1)){
        freeMoves[1] = 1;
        movesR++;
    }
    if(movesR > 0){
        eatMoves[4] = movesR;
        free(freeMoves);
        return eatMoves;
    }else{
        freeMoves[4] = moves;
        free(eatMoves);
        return freeMoves;
    }
}

void make_move(int i, int j, int*moves,int c){
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
            move_animal(i-1,j,i,j);
            break;
        case 1:
            move_animal(i,j+1,i,j);
            break;
        case 2:
            move_animal(i+1,j,i,j);
            break;
        case 3:
            move_animal(i,j-1,i,j);
            break;
    }
}

void sub_generations(int flag){
    int p = 0;
    for(int k = 0; k < N; k++){
        p = (k % 2 == flag) ? 0 : 1;
        for(int j = p; j < M; j += 2){
            if(grid[k][j].name == FOX || grid[k][j].name == RABBIT){
                int* moves = get_available_moves(k,j);
                if(moves[4] > 0){
                    int c = calculate_movement(k,j,moves[4]);
                    make_move(k,j,moves,c);
                }
                
            }
        }
    }
}



void generations(int n){
    for(int i = 0; i < n; i++){

        for(int k = 0; k < N; k++){
            for(int p = 0; p < M;p++){
                if(grid[k][p].name == FOX || grid[k][p].name == RABBIT){
                    grid[k][p].age++;
                    if(grid[k][p].name == FOX){
                        grid[k][p].lastGenEat++;
                    }
                }
            }
            
        }

        sub_generations(0);
        worldPrinter();
        sub_generations(1);
        worldPrinter();

        

    }
}

//quick example to test: ./foxes-rabbits 25 4 4 3 2 1 1 10 4 123
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

    //start grid
    grid = malloc(sizeof(struct entity*) * N);
    grid_temp = malloc(sizeof(struct entity*) * N);

    for(int k = 0; k < N; k++){
        grid[k] = malloc(M * sizeof(struct entity));
        grid_temp[k] = malloc(M * sizeof(struct entity));
    }

    for(int k = 0; k < N;k++){
        for(int j = 0; j < M; j++){
            grid[k][j].name = ' ';
            grid[k][j].age = 0;
            grid[k][j].lastGenEat = 0;
            grid_temp[k][j].name = ' ';
            grid_temp[k][j].age = 0;
            grid_temp[k][j].lastGenEat = 0;
        }
    }

    




    generate_element(nrocks,'*',&seedW);
    generate_element(nrabbits,'R',&seedW);
    generate_element(nfoxes,'F',&seedW);
    worldPrinter();

    double exec_time;
    exec_time = -omp_get_wtime();
    generations(ngen);
    exec_time += omp_get_wtime();
    fprintf(stderr, "%.1fs\n", exec_time);


    for(int i = 0; i < N;i++){
        free(grid[i]);
        free(grid_temp[i]);
    }
    free(grid);
    free(grid_temp);
    return 0;
}