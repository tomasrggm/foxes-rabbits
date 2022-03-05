#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int M,N;
char* grid;
uint32_t seedW; 

bool position_empty(int i, int j){
    if(grid[M*i + j] == ' '){
        return true;
    }else{
        return false;
    }
}

void insert_animal(int i,int j, char atype){
 grid[i*M + j] = atype; //endereco de memoria contem este carater. Maneira manhosa de iterar colunas numa so linha
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
    for(int k = 0; k < M*N; k++){
        if(k % M == 0){
            printf("\n");
        }
        printf("%c|",grid[k]);
    }
    printf("\n");
}



//quick example to test: ./foxes-rabbits 22 4 4 3 2 2 1 10 4 123
int main(int argc, char* argv[]){ //app name[0], gen[1], M[2], N[3], n-rocks[4], n-rabbits[5], rabbit breeding[6], n-foxes[7], fox breeding[8], fox starve[9], seed[10]
    seedW = atoi(argv[10]);
    M = atoi(argv[2]);
    N = atoi(argv[3]);
    grid = malloc(sizeof(char) * (M*N)); //numero de casas. Vamos pondo uma letra por cada espaco de memoria
    
    
    for(int k = 0; k < (M*N); k++){
        grid[k] = ' ';
        printf("%d\n",k);
    }


    int nrocks = atoi(argv[4]);
    int nrabbits = atoi(argv[5]);
    int nfoxes = atoi(argv[7]);
    generate_element(nrocks,'*',&seedW);
    generate_element(nrabbits,'R',&seedW);
    generate_element(nfoxes,'F',&seedW);

    worldPrinter();

    return 1;
}