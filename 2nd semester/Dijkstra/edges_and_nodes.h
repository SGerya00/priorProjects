/*
 * this file contains
 * functions and structures
 * that are specific to
 * a current problem
 * and are not universal
 */

#ifndef DIJKSTRA_EDGES_AND_NODES_H
#define DIJKSTRA_EDGES_AND_NODES_H

#include <stdio.h>

//
struct edge;

double get_edge_weight(struct edge *const edge);

long long int get_edge_from(struct edge *const edge);

long long int get_edge_to(struct edge *const edge);

struct edge * make_new_edge(double weight, long long int from, long long int to);

void delete_edge(struct edge *edge);
//

//
struct node;

size_t get_node_index(struct node *const node);

unsigned short int get_node_done_flag(struct node *const node);

void change_node_done_flag_to_done(struct node *const node);

double get_node_shortest_path(struct node *const node);

void change_node_shortest_path(struct node *const node, double shortest_path);

size_t get_node_taken(struct node *const node);

double get_node_weight_index(struct node *const node, size_t index);

struct node * get_node_neighbour_index(struct node *const node, size_t index);

struct node *create_new_node(const size_t index);

void free_node(struct node *node);
//
//
int node_neighbours_DA_needs_expanding(struct node *const node);

void increase_neighbours_DA(struct node *node);
//
//
void sort_neighbours_DA_in_node(struct node *const node);

void put_edge_in_node(struct edge *const edge,  struct node *const from, struct node *const to);
//

#endif //DIJKSTRA_EDGES_AND_NODES_H
