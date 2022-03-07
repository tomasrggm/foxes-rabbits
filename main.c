#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "Manager/worldManager.h"

//quick example to test: ./foxes-rabbits 25 4 4 3 2 1 1 10 4 123
int main(int argc, char* argv[]){ //app name[0], gen[1], M[2], N[3], n-rocks[4], n-rabbits[5], rabbit breeding[6], n-foxes[7], fox breeding[8], fox starve[9], seed[10]
    uint32_t seedW = atoi(argv[10]);
    M = atoi(argv[2]);
    N = atoi(argv[3]);
    grid = malloc(sizeof(char*) * N);
    for(int k = 0; k < N; k++){
        grid[k] = malloc(M * sizeof(char));
    }

    for(int k = 0; k < N;k++){
        for(int j = 0; j < M; j++){
            grid[k][j] = ' ';
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

    return 1;
}