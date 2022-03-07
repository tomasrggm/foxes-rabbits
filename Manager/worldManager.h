#ifndef __WORLD_MANAGER_H__
#define __WORLD_MANAGER_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

extern char** grid;
extern int M, N; //Rows and Columns


const bool position_empty(int row, int column);

void insert_animal(int row, int column, char atype);

float r4_uni(uint32_t *seed);
void generate_element(int n, char atype, uint32_t * seed);



void worldPrinter();

#endif