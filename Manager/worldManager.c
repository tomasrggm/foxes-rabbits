#include "worldManager.h"

char** grid;
int M, N;

const bool position_empty(int row, int column) {
    return grid[row][column] == ' ';
}

void insert_animal(int row, int column, char atype) {
    grid[row][column] = atype;
}

float r4_uni(uint32_t * seed) {
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

void generate_element(int n, char atype, uint32_t *seed) {
    int i, j, k;

    for(k = 0; k < n; k++){
	    i = M * r4_uni(seed); //axis vertical
	    j = N * r4_uni(seed); //axis horizontal
	    if(position_empty(i, j))
	        insert_animal(i, j, atype);
    }
}

void worldPrinter(){
    for(int k = 0; k < N; k++){
        for(int j = 0; j < M; j++){
            printf("%c|",grid[k][j]);
        }
        printf("\n");
    }
}