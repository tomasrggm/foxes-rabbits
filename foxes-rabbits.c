#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int M,N;
char* grid;

bool position_empty(int i, int j){
    return true;
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
	i = M * r4_uni(seed);
	j = N * r4_uni(seed);

    printf("%d com %d\n",i,j); //verificar as coordenadas tocadas
	if(position_empty(i, j))
	    insert_animal(i, j, atype);
    }
}



//quick example to test: ./foxes-rabbits 22 4 4 3 2 2 1 10 4 123
int main(int argc, char* argv[]){ //app name[0], gen[1], M[2], N[3], n-rocks[4], n-rabbits[5], rabbit breeding[6], n-foxes[7], fox breeding[8], fox starve[9], seed[10]
    uint32_t seed = atoi(argv[10]);
    M = atoi(argv[2]);
    N = atoi(argv[3]);
    grid = malloc(sizeof(char) * (M*N)); //numero de casas. Vamos pondo uma letra por cada espaco de memoria
    generate_element(12,'a',&seed);
    printf("o carater e %c \n",grid[2]); //teste do apontador
    
    return 1;
}