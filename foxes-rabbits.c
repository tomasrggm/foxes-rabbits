#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int M,N;
int breedingAgeR;
int breedingAgeF;
struct entity** grid;
uint32_t seedW; 

struct entity{
    char name;
    int age;
    int lastGenEat;
};

bool position_empty(int i, int j){
    return grid[i][j].name == ' ';
}

void insert_animal(int i,int j, char atype){
    grid[i][j].name = atype;
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
	i = M * r4_uni(seed);//axis vertical
	j = N * r4_uni(seed);//axis horizontal
	if(position_empty(i, j))
	    insert_animal(i, j, atype);
    }
}

void worldPrinter(){
    for(int k = 0; k < N; k++){
        for(int j = 0; j < M; j++){
            printf("%c|",grid[k][j].name);
        }
        printf("\n");
    }
}

void move(int i, int j, char type, int age, int og_i, int og_j){
    grid[i][j].name = type;
    grid[i][j].age = age;
    grid[og_i][og_j].name = ' ';
    grid[og_i][og_j].age = 0;
}


void pass(int flag){
    int p = 0;
    for(int k = 0; k < N; k++){
        p = (k % 2 == flag) ? 0 : 1;
        for(p; p < M; p += 2){
           printf("%d - %d\n",k,p); 
        }
    }
}

int checkAvailability(int i, int j){
    int moves = 0;

    if(i != 0 && grid[i-1][j].name == ' '){
        moves++;
    }
    if(i != M-1 && grid[i+1][j].name == ' '){
        moves++;
    }
    if(j != 0 && grid[i][j-1].name == ' '){
        moves++;
    }
    if(j != N-1 && grid[i][j+1].name == ' '){
        moves++;
    }
    
    return moves;
}

int calculate_movement(int i, int j, int p){
    int c = i * N + j;
    return c % p;
}

void move_rabbit(int i, int j, int og_i, int og_j){
    grid[i][j].name = 'Z';
    grid[i][j].age = grid[og_i][og_j].age;
    if(grid[i][j].age != breedingAgeR){
        grid[og_i][og_j].name = ' ';
        grid[og_i][og_j].age = 0;
    }else{
        grid[og_i][og_j].age = 0;
    }
    
}

void move_fox(int i, int j, int og_i, int og_j){
    grid[i][j].name = 'V';
    grid[i][j].age = grid[og_i][og_j].age;
    if(grid[i][j].age != breedingAgeF){
        grid[og_i][og_j].name = ' ';
        grid[og_i][og_j].age = 0;
    }else{
        grid[og_i][og_j].age = 0;
    }
    
}


//quick example to test: ./foxes-rabbits 25 4 4 3 2 1 1 10 4 123
int main(int argc, char* argv[]){ //app name[0], gen[1], M[2], N[3], n-rocks[4], n-rabbits[5], rabbit breeding[6], n-foxes[7], fox breeding[8], fox starve[9], seed[10]
    seedW = atoi(argv[10]);
    M = atoi(argv[2]);
    N = atoi(argv[3]);
    breedingAgeR = atoi(argv[6]);
    breedingAgeF = atoi(argv[8]);
    grid = malloc(sizeof(struct entity*) * N);
    for(int k = 0; k < N; k++){
        grid[k] = malloc(M * sizeof(struct entity));
    }

    for(int k = 0; k < N;k++){
        for(int j = 0; j < M; j++){
            grid[k][j].name = ' ';
            printf("%d\n",k);
        }
    }
    
    



    int nrocks = atoi(argv[4]);
    int nrabbits = atoi(argv[5]);
    int nfoxes = atoi(argv[7]);
    generate_element(nrocks,'*',&seedW);
    generate_element(nrabbits,'R',&seedW);
    generate_element(nfoxes,'F',&seedW);

    worldPrinter();
    pass(0);
    printf("\n");
    pass(1);
    printf("%d\n",calculate_movement(2,2,3));
    return 1;
}