/*
 * this file contains
 * function declarations and defines
 * necessary for the HT to function properly
 */
#ifndef DIJKSTRA_KEY_AND_DATA_H
#define DIJKSTRA_KEY_AND_DATA_H

#include <stdio.h>

#define KEY_TYPE size_t //just an example

//struct data is in corresponding .c file containing function definitions

size_t hash_function(KEY_TYPE key); //returns size_t hash function value

int compare_keys(KEY_TYPE key1, KEY_TYPE key2); //check if keys are identical, 1 if yes, 0 if no

void free_data(void *data); //frees memory used to store data

void free_key(KEY_TYPE key); //frees memory used for key if it was allocated, otherwise does nothing

#endif //DIJKSTRA_KEY_AND_DATA_H
