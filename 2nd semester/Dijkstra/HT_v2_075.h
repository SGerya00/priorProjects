/*
 * this .h file should be included after .h file for key and data
 * so that KEY_TYPE
 * which is supposed to be defined in .h file for key and data
 * is considered an actual type
 * in function declaration
 */

#ifndef DIJKSTRA_HT_V2_075_H
#define DIJKSTRA_HT_V2_075_H

#include <stdio.h>
#include "key_and_data.h"

struct HT;

size_t get_HT_capacity(struct HT *const hash_table);

int slot_is_empty(struct HT *const hash_table, size_t index);

void put_in_HT(struct HT *const hash_table, KEY_TYPE key, void *data);

void * get_element_from_HT(struct HT *const hash_table, size_t index);

void * search_in_HT(struct HT *const hash_table, KEY_TYPE key);

struct HT * create_HT();

void delete_HT(struct HT *const hash_table);

#endif //DIJKSTRA_HT_V2_075_H
