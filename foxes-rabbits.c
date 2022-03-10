#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int M,N;
int breedingAgeR;
int breedingAgeF;
struct entity** grid;
struct entity** grid_temp;
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

void handle_conflict(){

}

//atualizar a casa consoante o movimento
void move_rabbit(int i, int j, int og_i, int og_j){
    grid_temp[i][j].name = 'Z';
    grid_temp[i][j].age = grid[og_i][og_j].age;
    if(grid_temp[i][j].age != breedingAgeR){
        grid_temp[og_i][og_j].name = ' ';
        grid_temp[og_i][og_j].age = 0;
        
    }else{
        grid_temp[og_i][og_j].age = 0;
        grid_temp[i][j].age = 0;
    }
    
}

//atualizar a casa consoante o movimento
void move_fox(int i, int j, int og_i, int og_j){
    grid_temp[i][j].name = 'V';
    grid_temp[i][j].age = grid[og_i][og_j].age;
    if(grid_temp[i][j].age != breedingAgeF){
        grid_temp[og_i][og_j].name = ' ';
        grid_temp[og_i][og_j].age = 0;
    }else{
        grid_temp[og_i][og_j].age = 0;
        grid_temp[i][j].age = 0;
    }
    
}




int* checkAvailability(int i, int j){ // {up,right,down,left,movesAvailable}
    int* available = malloc(sizeof(int)*5);
    int moves = 0;
    bool isfox = grid[i][j].name == 'F';
    if(i != 0 && position_empty(i-1,j) || (isfox && grid[i-1][j].name == 'R')){
        available[0] = 1;
        moves++;
    }
    if(i != M-1 && position_empty(i+1,j) || (isfox && grid[i+1][j].name == 'R')){
        available[2] = 1;
        moves++;
    }
    if(j != 0 && position_empty(i,j-1) || (isfox && grid[i][j-1].name == 'R')){
        available[3] = 1;
        moves++;
    }
    if(j != N-1 && position_empty(i,j+1) || (isfox && grid[i][j+1].name == 'R')){
        available[1] = 1;
        moves++;
    }
    available[4] = moves;
    
    return available;
}

int calculate_movement(int i, int j, int p){
    int c = i * N + j;
    return c % p;
}

void coordinateMovement(int i, int j, int* movements,int moveFlag){
    if(grid[i][j].name == ' ' || grid[i][j].name == '*'){
        return;
    }
    int counter = -1;
    int availablemoves = -1;
    for(int i = 0; i < 4;i++){
        counter++;
        if(movements[i]!= 0){
            availablemoves++;
            if(availablemoves == moveFlag){
                break;
            }
        }
    }
    switch(counter){
        case 0:
            if(grid[i][j].name == 'R'){
                grid[i][j].age++;
                move_rabbit(i-1,j,i,j);
            }else{
                grid[i][j].age++;
                move_fox(i-1,j,i,j);
            };break;
        case 1:
            if(grid[i][j].name == 'R'){
                grid[i][j].age++;
                move_rabbit(i,j+1,i,j);
            }else{
                grid[i][j].age++;
                move_fox(i,j+1,i,j);
            };break;
        case 2:
            if(grid[i][j].name == 'R'){
                grid[i][j].age++;
                move_rabbit(i+1,j,i,j);
            }else{
                grid[i][j].age++;
                move_fox(i+1,j,i,j);
            };break;
        case 3:
            if(grid[i][j].name == 'R'){
                grid[i][j].age++;
                move_rabbit(i,j-1,i,j);
            }else{
                grid[i][j].age++;
                move_fox(i,j-1,i,j);
            };break;
    }
    
}

//mexer com o vermelho (flag = 0) e o preto (flag = 1)
void pass(int flag){
    int p = 0;
    for(int k = 0; k < N; k++){
        p = (k % 2 == flag) ? 0 : 1;
        for(p; p < M; p += 2){
            int* choices = checkAvailability(k,p);
            int availableMoves = choices[4];
            if(availableMoves != 0){
                int moveFlag = calculate_movement(k,p,availableMoves);
                coordinateMovement(k,p,choices,moveFlag);
            }
        }
    }
    for(int k = 0; k < N;k++){
        for(int j = 0; j < M;j++){
            if(grid_temp[k][j].name == 'V'){
                grid_temp[k][j].name = 'F';
            }
            if(grid_temp[k][j].name == 'Z'){
                grid_temp[k][j].name = 'R';
            }
            grid[k][j] = grid_temp[k][j];
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
    int nrocks = atoi(argv[4]);
    int nrabbits = atoi(argv[5]);
    int nfoxes = atoi(argv[7]);

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
            grid_temp[k][j].name = ' ';
        }
    }
    //
    




    generate_element(nrocks,'*',&seedW);
    generate_element(nrabbits,'R',&seedW);
    generate_element(nfoxes,'F',&seedW);

    worldPrinter();
    pass(0);
    printf("\n");
    worldPrinter();
    //pass(1);
    //printf("%d\n",calculate_movement(2,2,3));
    return 1;
}