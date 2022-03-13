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

bool is_rabbit (int i, int j) {
    return grid[i][j].name == RABBIT;
}

bool position_empty(int i, int j){
    return grid[i][j].name == ' ';
}

void insert_animal(int i,int j, char atype){
    grid[i][j].name = atype;
    grid[i][j].age = 1;
    grid[i][j].lastGenEat = 1;
    grid_temp[i][j].name = atype;
    grid_temp[i][j].age = 1;
    grid_temp[i][j].lastGenEat = 1;
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
            if(grid[k][j].name == 'V'){
                printf("F|");
            }else if(grid[k][j].name == 'Z'){
                printf("R|");
            }else{
                printf("%c %d|",grid[k][j].name,grid[k][j].age);
            }
            
        }
        printf("\n");
    }
}

// Honestly isto deve ser chamado no fim de cada subgeracao, uma unica vez.
// Devemos dar track aos animais que se mexeram, possivelmente juntos com dados sobre a sua posicao.
// Assim podemos 
bool conflict_rabbit_rabbit (int og_i,int og_j,int i, int j){
    bool rabbit = grid_temp[i][j].name == 'R' || grid_temp[i][j].name == 'Z';
    return (rabbit && grid_temp[i][j].age > grid_temp[og_i][og_j].age);
}
bool conflict_fox_fox (int og_i,int og_j,int i, int j){
    bool fox = grid_temp[i][j].name == 'F' || grid_temp[i][j].name == 'V';
    bool starve = grid_temp[i][j].lastGenEat > grid_temp[og_i][og_j].lastGenEat;
    bool breed = grid_temp[i][j].lastGenEat == grid_temp[og_i][og_j].lastGenEat && grid_temp[i][j].age > grid_temp[og_i][og_j].age;
    return (fox && (starve || breed));
}

//atualizar a casa consoante o movimento
void move_rabbit(int i, int j, int og_i, int og_j){
    if((grid_temp[i][j].name != ' ' && conflict_rabbit_rabbit(og_i,og_j,i,j))  ||  grid_temp[i][j].name == 'F' || grid_temp[i][j].name == 'V'){
        return;
    }
    grid_temp[i][j].name = 'Z';
    grid_temp[i][j].age = grid[og_i][og_j].age;
    if(grid_temp[i][j].age != breedingAgeR){
        grid_temp[og_i][og_j].name = ' ';
        grid_temp[og_i][og_j].age = 0;
        
    }else{
        grid_temp[og_i][og_j].name = 'Z';
        grid_temp[og_i][og_j].age = 0;
        grid_temp[i][j].age = 0;
    }
    
}

//atualizar a casa consoante o movimento
void move_fox(int i, int j, int og_i, int og_j){
    if(grid_temp[i][j].name != ' ' && conflict_fox_fox(og_i,og_j,i,j)){
        if(grid_temp[i][j].lastGenEat == 0){
            grid_temp[i][j].age = grid_temp[i][j].age > grid_temp[og_i][og_j].age ? grid_temp[i][j].age : grid_temp[og_i][og_j].age;
        }
        return;
    }
    grid_temp[i][j].name = 'V';
    grid_temp[i][j].age = grid[og_i][og_j].age;
    if(grid_temp[i][j].age != breedingAgeF){
        grid_temp[og_i][og_j].name = ' ';
        grid_temp[og_i][og_j].age = 0;
    }else{
        grid_temp[og_i][og_j].name = 'V';
        grid_temp[og_i][og_j].age = 0;
        grid_temp[i][j].age = 0;
    }
    
}

void move_animal (int i, int j, int og_i, int og_j) {
    if (is_rabbit(og_i, og_j)) {
        move_rabbit(i,j,og_i,og_j);
    }else{
        move_fox(i,j,og_i,og_j);
    }
}





int* checkAvailability(int i, int j){ // {up,right,down,left,movesAvailable}
    int* available = malloc(sizeof(int)*5);
    int* availableR = malloc(sizeof(int)*5);
    int movesR = 0;
    int moves = 0;
    
    bool isfox = grid[i][j].name == FOX;
    if((i != 0 && position_empty(i-1,j))){
        available[0] = 1;
        moves++;
    }
    if(i != 0 && (isfox && is_rabbit(i-1,j))){
        availableR[0] = 1;
        movesR++;
    }
    if((i != M-1 && position_empty(i+1,j))){
        available[2] = 1;
        moves++;
    }
    if(i != M-1 &&(isfox && is_rabbit(i+1,j))){
        availableR[2] = 1;
        movesR++;
    }
    
    if((j != 0 && position_empty(i,j-1))){
        available[3] = 1;
        moves++;
    }
    if(j != 0 && (isfox && is_rabbit(i,j-1))){
        availableR[3] = 1;
        movesR++;
    }

    if((j != N-1 && position_empty(i,j+1))){
        available[1] = 1;
        moves++;
    }
    if(j != N-1 && isfox && is_rabbit(i,j+1)){
        availableR[1] = 1;
        movesR++;
    }
    if(movesR > 0){
        free(available);
        availableR[4] = movesR;
        return availableR;
    }

    available[4] = moves;
    
    return available;
}

int calculate_movement(int i, int j, int p){
    int c = i * N + j;
    return c % p;
}

void coordinateMovement(int i, int j, int* movements, int moveFlag){
    if(grid[i][j].name == ' ' || grid[i][j].name == ROCK){
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

//mexer com o vermelho (flag = 0) e o preto (flag = 1)
void pass(int flag){
    int p = 0;
    for(int k = 0; k < N; k++){
        p = (k % 2 == flag) ? 0 : 1;
        for(int j = p;j < M; j += 2){
            if(grid[k][j].name == FOX){
                grid[k][j].lastGenEat++;
            }
            int* choices = checkAvailability(k,j);
            int availableMoves = choices[4];
            if(availableMoves != 0){
                int moveFlag = calculate_movement(k,j,availableMoves);
                coordinateMovement(k,j,choices,moveFlag);
            }
        }
    }

}

void clean(){
        for(int k = 0; k < N;k++){
        for(int j = 0; j < M;j++){
            if(grid_temp[k][j].name == 'V'){
                grid_temp[k][j].name = 'F';
            }
            if(grid_temp[k][j].name == 'Z'){
                grid_temp[k][j].name = 'R';
            }
            grid[k][j] = grid_temp[k][j];
            grid[k][j].age++;
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

    double exec_time;
    generate_element(nrocks,ROCK,&seedW);
    generate_element(nrabbits,RABBIT,&seedW);
    generate_element(nfoxes,FOX,&seedW);

    worldPrinter();
    exec_time = -omp_get_wtime();

    for(int i = 0; i < 10;i++){
        printf("geracao %d\n", i+1);
        pass(0);
        worldPrinter();
        printf("\n");
        pass(1);
        clean();
        worldPrinter();
        printf("\n");
    }

    exec_time += omp_get_wtime();
    fprintf(stderr, "%.1fs\n", exec_time);

    //printf("%d\n",calculate_movement(2,2,3));
    return 0;
}