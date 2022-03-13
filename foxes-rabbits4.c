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
int genCheck = 0;
// Animal types (or 'names').
char FOX = 'F';
char RABBIT = 'R';
char ROCK = '*';


struct entity{
    char name;
    int age;
    int lastGenEat;
    int moved;
};


bool position_empty(int i, int j){
    return grid[i][j].name == ' ';
}
bool has_rabbit(int i,int j){
    return grid[i][j].name == 'R';
}

bool is_there(int i, int j, char c){
    return grid_temp[i][j].name == c && grid[i][j].name != c;
}

int calculate_movement(int i, int j, int p){
    int c = i * N + j;
    return c % p;
}


void worldPrinter(){
    for(int k = 0; k < M; k++){
        for(int j = 0; j < N; j++){
            printf("%c %d|",grid[k][j].name,grid[k][j].lastGenEat);
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
    int bred = 0;
    

    if(grid_temp[og_i][og_j].age >= breedingAgeR){
        grid_temp[og_i][og_j].age = 0;
        bred++;
    }
    if(grid_temp[i][j].name == ' '){
        grid_temp[i][j].name = RABBIT;
        grid_temp[i][j].age = grid_temp[og_i][og_j].age;
    }
    else if(grid_temp[i][j].name == FOX){
        grid_temp[i][j].lastGenEat = 0;
    }
    else if(grid_temp[i][j].name == RABBIT && grid_temp[og_i][og_j].age > grid_temp[i][j].age){
        grid_temp[i][j].age = grid_temp[og_i][og_j].age;
    }
    grid_temp[og_i][og_j].age = 0;
    if(bred == 0){
        grid_temp[og_i][og_j].name = ' ';
    }
    
    grid_temp[i][j].moved = 1;

}

void move_fox(int i,int j,int og_i,int og_j){
    int bred = 0;
    int starve = grid_temp[og_i][og_j].lastGenEat;
    

    printf("VALORES SAO %d - %d\n",i,j);
    if(grid_temp[og_i][og_j].age >= breedingAgeF){
        grid_temp[og_i][og_j].age = 0;
        grid_temp[og_i][og_j].lastGenEat = 0;
        bred++;
    }

    if(grid_temp[i][j].name == ' '){
        grid_temp[i][j].name = FOX;
        grid_temp[i][j].age = grid_temp[og_i][og_j].age;
        grid_temp[i][j].lastGenEat = starve;
    }
    else if(grid_temp[i][j].name == FOX){
        if(grid_temp[i][j].lastGenEat == 0 && grid_temp[og_i][og_j].age > grid_temp[i][j].age){
            grid_temp[i][j].age = grid_temp[og_i][og_j].age;
        }
        else if(starve == grid_temp[i][j].lastGenEat && grid_temp[og_i][og_j].age > grid_temp[i][j].age){
            grid_temp[i][j].age = grid_temp[og_i][og_j].age;
        }
        else if(starve > grid_temp[i][j].lastGenEat){
            grid_temp[i][j].age = grid_temp[og_i][og_j].age;
            grid_temp[i][j].lastGenEat = starve;
        }
    }
    else if(grid_temp[i][j].name == RABBIT){
        grid_temp[i][j].name = FOX;
        grid_temp[i][j].age = grid_temp[og_i][og_j].age;
        grid_temp[i][j].lastGenEat = 0;
    }
    grid_temp[og_i][og_j].age = 0;
    grid_temp[og_i][og_j].lastGenEat = 0;
    if(bred == 0){
        grid_temp[og_i][og_j].name = ' ';
    }

    grid_temp[i][j].moved = 1;
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
    for(int k = 0; k < 5;k++){
        freeMoves[k] = 0;
        eatMoves[k] = 0;
    }
    if(i != 0 && position_empty(i-1,j)){
        freeMoves[0] = 1;
        moves++;
    }
    if(i != M-1 && position_empty(i+1,j)){
        freeMoves[2] = 1;
        moves++;
    }
    if(j != 0 && position_empty(i,j-1)){
        freeMoves[3] = 1;
        moves++;
    }
    if(j != N-1 && position_empty(i,j+1)){
        freeMoves[1] = 1;
        moves++;
    }
    
    if(i != 0 && isFox && has_rabbit(i-1,j)){
        
        eatMoves[0] = 1;
        movesR++;
    }
    
    if( i != M-1 && isFox && has_rabbit(i+1,j)){
        
        eatMoves[2] = 1;
        movesR++;
    }
    
    if(j != 0 && isFox && has_rabbit(i,j-1)){
        
        eatMoves[3] = 1;
        movesR++;
    }
    
    if(j != N-1 && isFox && has_rabbit(i,j+1)){
        
        eatMoves[1] = 1;
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
    for(int k = 0; k < M; k++){
        p = (k % 2 == flag) ? 0 : 1;
        for(int j = p; j < N; j += 2){
            if((grid[k][j].name == FOX || grid[k][j].name == RABBIT) && grid[k][j].moved == 0){
                int* moves = get_available_moves(k,j);
                if(moves[4] > 0){
                    int c = calculate_movement(k,j,moves[4]);
                    make_move(k,j,moves,c);
                }
                free(moves);
            }
        }
    }
}

void copy_red(){
    for(int i = 0; i < M; i++){
        for(int j = 0; j < N; j++){
            grid[i][j].name = grid_temp[i][j].name;
            grid[i][j].age = grid_temp[i][j].age;
            grid[i][j].lastGenEat = grid_temp[i][j].lastGenEat;
            grid[i][j].moved = grid_temp[i][j].moved;
        }
    }
}

void copy_black(){
    for(int i = 0; i < M; i++){
        for(int j = 0; j < N; j++){
            grid[i][j].name = grid_temp[i][j].name;
            grid[i][j].age = grid_temp[i][j].age;
            grid[i][j].lastGenEat = grid_temp[i][j].lastGenEat;
            grid[i][j].moved = 0;
            grid_temp[i][j].moved = 0;
        }
    }
}


void generations(int n){
    for(int k = 0; k < n; k++){
        for(int i = 0; i < M; i++){
            for(int j = 0; j < N; j++){
                if(grid[i][j].name == FOX || grid[i][j].name == RABBIT){
                    grid[i][j].age++;
                    grid_temp[i][j].age++;
                    if(grid[i][j].name == FOX){
                        grid[i][j].lastGenEat++;
                        grid_temp[i][j].lastGenEat++;
                    }
                }
            }
        
        
        }
        printf("GERACAO %d\n VERMELHA\n",k+1);
        sub_generations(0);
        copy_red();
        worldPrinter();
        printf("PRETA\n");
        sub_generations(1);
        copy_black();
        worldPrinter();
        for(int i = 0; i < M; i++){
            for(int j = 0; j < N; j++){
                if(grid[i][j].name == FOX && grid[i][j].lastGenEat == starvationAgeF){
                    grid[i][j].name = ' ';
                    grid[i][j].lastGenEat = 0;
                    grid[i][j].age = 0;
                    grid[i][j].moved = 0;
                    grid_temp[i][j].name = ' ';
                    grid_temp[i][j].lastGenEat = 0;
                    grid_temp[i][j].age = 0;
                    grid_temp[i][j].moved = 0;
                }
            }
        }
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
    grid = malloc(sizeof(struct entity*) * M);
    grid_temp = malloc(sizeof(struct entity*) * M);

    for(int k = 0; k < M; k++){
        grid[k] = malloc(N * sizeof(struct entity));
        grid_temp[k] = malloc(N * sizeof(struct entity));
    }

    for(int k = 0; k < M;k++){
        for(int j = 0; j < N; j++){
            grid[k][j].name = ' ';
            grid[k][j].age = 0;
            grid[k][j].lastGenEat = 0;
            grid[k][j].moved = 0;
            grid_temp[k][j].name = ' ';
            grid_temp[k][j].age = 0;
            grid_temp[k][j].lastGenEat = 0;
            grid_temp[k][j].moved = 0;
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


    for(int i = 0; i < M;i++){
        free(grid[i]);
        free(grid_temp[i]);
    }
    free(grid);
    free(grid_temp);
    return 0;
}