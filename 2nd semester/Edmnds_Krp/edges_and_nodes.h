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
struct read_edge;

struct read_edge * make_new_read_edge(double weight, long long int from, long long int to);

void delete_read_edge(struct read_edge *read_edge);

double get_read_edge_weight(struct read_edge *const edge);

long long int get_read_edge_from(struct read_edge *const edge);

long long int get_read_edge_to(struct read_edge *const edge);
//

//
struct node;
struct edge;

size_t get_node_index(struct node *const node);

unsigned short int get_node_BFS_visited_flag(struct node *const node);

void change_node_BFS_visited_flag_to_visited(struct node *const node);

void change_node_BFS_visited_flag_to_not_visited(struct node *const node);

struct node * get_node_BFS_parent(struct node *const node);

void cast_node_BFS_parent(struct node *const node, struct node *const parent);

size_t get_node_routes_arr_taken(struct node *const node);

struct edge * get_node_edge(struct node *const node, size_t index);

void put_edge_in_node(struct node *const node, struct edge *const edge, size_t index);
//
double get_edge_flow_capacity(struct edge *const edge);

double get_edge_existing_flow(struct edge *const edge);

double get_edge_remaining_capacity(struct edge *const edge);

struct node * get_edge_from(struct edge *const edge);

struct node * get_edge_to(struct edge *const edge);

struct edge * get_edge_reverse_same(struct edge *const edge);
//
void add_to_edge_existing_flow(struct edge *const edge, double new_flow);

struct edge *create_edge(double flow_capacity, struct node *const from, struct node *const to);
//
struct node * create_new_node(size_t index);

void delete_node(struct node *node);

void expand_node_routes_array_if_nec(struct node *const node);
//

#endif //DIJKSTRA_EDGES_AND_NODES_H
