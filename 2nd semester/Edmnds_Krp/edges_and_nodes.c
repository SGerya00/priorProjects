#include "edges_and_nodes.h"

//#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define INITIAL_CAPACITY 4
#define VISITED 1
#define NOT_VISITED 0

struct read_edge {
    double weight;
    long long int from;
    long long int to;
};

//a pointer to this struct will be .data in HT_element
struct node {
    size_t index;
    //for BFS
    unsigned short int BFS_visited_flag; //will be needed when searching for shortest path via BFS
    struct node *BFS_parent;             //will be used during BFS to know shortest path
    //
    struct edge {
        double flow_capacity;
        double existing_flow;
        struct node *from;
        struct node *to;
        struct edge *reverse_same_edge; //edge that leads from to to from
        //and has existing_flow of (existing_flow * (-1)) and 0 capacity
        //(it is necessary for the algorithm to produce an accurate result)
    } *(*routes)[]; //a pointer to an array of pointers to edges sticking out from that node
    size_t routes_arr_capacity;
    size_t routes_arr_taken;
};

//
double get_read_edge_weight(struct read_edge *const edge) {
    assert(NULL != edge);

    return edge->weight;
}

long long int get_read_edge_from(struct read_edge *const edge) {
    assert(NULL != edge);

    return edge->from;
}

long long int get_read_edge_to(struct read_edge *const edge) {
    assert(NULL != edge);

    return edge->to;
}
//
struct read_edge * create_blank_read_edge() {
    struct read_edge *edge = (struct read_edge*)calloc(1, sizeof(struct read_edge));
    assert(NULL != edge);
    return edge;
} //allocates memory

struct read_edge * make_new_read_edge(double weight, long long int from, long long int to) {

    struct read_edge *new_read_edge = create_blank_read_edge();
    new_read_edge->weight = weight;
    new_read_edge->from = from;
    new_read_edge->to = to;
    return new_read_edge;
} //returns pointer to newly created edge

void delete_read_edge(struct read_edge *read_edge) {
    assert(NULL != read_edge);

    free(read_edge);
}
//

//
size_t get_node_index(struct node *const node) {
    assert(NULL != node);

    return node->index;
}

unsigned short int get_node_BFS_visited_flag(struct node *const node) {
    assert(NULL != node);

    return node->BFS_visited_flag;
}

void change_node_BFS_visited_flag_to_visited(struct node *const node) {
    assert(NULL != node);

    node->BFS_visited_flag = VISITED;
} //changes flag to VISITED

void change_node_BFS_visited_flag_to_not_visited(struct node *const node) {
    assert(NULL != node);

    node->BFS_visited_flag = NOT_VISITED;
} //changes flag to VISITED

struct node * get_node_BFS_parent(struct node *const node) {
    assert(NULL != node);

    return node->BFS_parent;
}

void cast_node_BFS_parent(struct node *const node, struct node *const parent) {
    assert(NULL != node);

    node->BFS_parent = parent;
}

size_t get_node_routes_arr_capacity(struct node *const node) {
    assert(NULL != node);

    return node->routes_arr_capacity;
}

void double_node_routes_arr_capacity(struct node *const node) {
    assert(NULL != node);

    node->routes_arr_capacity *= 2;
}

size_t get_node_routes_arr_taken(struct node *const node) {
    assert(NULL != node);

    return node->routes_arr_taken;
}

void increase_node_routes_arr_taken(struct node *const node) {
    assert(NULL != node);

    node->routes_arr_taken += 1;
}

struct edge *(*get_node_routes_array(struct node *const node))[] {
    assert(NULL != node);

    return node->routes;
}

void cast_node_routes(struct node *const node, struct edge *(*routes)[]) {
    assert(NULL != node);
    assert(NULL != routes);

    node->routes = routes;
}

struct edge * get_node_edge(struct node *const node, size_t index) {
    assert(NULL != node);

    struct edge *(*routes)[] = get_node_routes_array(node);
    struct edge *edge = (*routes)[index];
    return edge;
}

void put_edge_in_node(struct node *const node, struct edge *const edge, size_t index) {
    assert(NULL != node);
    assert(NULL != edge);

    struct edge *(*routes_array)[] = get_node_routes_array(node);

    (*routes_array)[index] = edge;

    increase_node_routes_arr_taken(node);
}
//
double get_edge_flow_capacity(struct edge *const edge) {
    assert(NULL != edge);

    return edge->flow_capacity;
}

double get_edge_existing_flow(struct edge *const edge) {
    assert(NULL != edge);

    return edge->existing_flow;
}

double get_edge_remaining_capacity(struct edge *const edge) {
    assert(NULL != edge);

    double f_c = get_edge_flow_capacity(edge);
    double e_f = get_edge_existing_flow(edge);
    double rem_cap = f_c - e_f;
    return rem_cap;
}

struct node * get_edge_from(struct edge *const edge) {
    assert(NULL != edge);

    return edge->from;
}

struct node * get_edge_to(struct edge *const edge) {
    assert(NULL != edge);

    return edge->to;
}

struct edge * get_edge_reverse_same(struct edge *const edge) {
    assert(NULL != edge);

    return edge->reverse_same_edge;
}

void add_to_edge_existing_flow(struct edge *const edge, double new_flow) {
    assert(NULL != edge);

    edge->existing_flow += new_flow;
    struct edge *reverse_same_edge = get_edge_reverse_same(edge);
    reverse_same_edge->existing_flow -= new_flow;
} //adds to both edge and reverse_same
//
struct edge *create_edge(double flow_capacity, struct node *const from, struct node *const to) {
    assert(NULL != from);
    assert(NULL != to);

    struct edge *new_edge = (struct edge *)calloc(1, sizeof(struct edge));
    assert(NULL != new_edge);

    new_edge->flow_capacity = flow_capacity;
    new_edge->from = from;
    new_edge->to = to;

    struct edge *reverse_new_edge = (struct edge *)calloc(1, sizeof(struct edge));
    assert(NULL != reverse_new_edge);

    reverse_new_edge->flow_capacity = (double)0;
    reverse_new_edge->from = to;
    reverse_new_edge->to = from;

    new_edge->reverse_same_edge = reverse_new_edge;
    reverse_new_edge->reverse_same_edge = new_edge;

    return new_edge;
}
//creates edge and it's reversed brother, returns pointer to new edge

void delete_edge(struct edge *edge) {
    assert(NULL != edge);

    free(edge);
} //doesn't free reverse_same_edge, it will be freed in loop for other node

struct edge *(*create_routes_array())[] {

    struct edge *(*routes_array)[] = (struct edge *(*)[])calloc(INITIAL_CAPACITY, sizeof(struct edge *));
    assert(NULL != routes_array);

    return routes_array;
}

void delete_routes_array(struct edge *(*routes_arr)[]) {
    assert(NULL != routes_arr);

    free(routes_arr);
    //do not free elements in array, just array itself
}

struct node * create_new_node(size_t index) {

    struct node *new_node = (struct node *)calloc(1, sizeof(struct node));
    assert(NULL != new_node);

    new_node->index = index;
    new_node->BFS_visited_flag = NOT_VISITED;
    new_node->BFS_parent = NULL;
    new_node->routes = create_routes_array();

    return new_node;
} //creates new node, it has blank array of INITIAL_CAPACITY

void delete_node(struct node *node) {
    assert(NULL != node);

    size_t taken = get_node_routes_arr_taken(node);
    struct edge *(*routes_arr)[] = get_node_routes_array(node);
    for (register size_t i = 0; i < taken; i++) {
        delete_edge(  (*routes_arr)[i]  );
    }
    delete_routes_array(routes_arr);

    free(node);
} //frees memory allocated for edges, array of edges (routes) and element itself

void expand_node_routes_array_if_nec(struct node *const node) {
    assert(NULL != node);

    size_t capacity = get_node_routes_arr_capacity(node);
    size_t taken = get_node_routes_arr_taken(node);
    if (taken == capacity) {
        size_t new_capacity = capacity * 2;
        struct edge *(*new_routes_arr)[] = (struct edge *(*)[])calloc(new_capacity, sizeof(struct edge *));
        assert(NULL != new_routes_arr);

        struct edge *(*old_routes_arr)[] = get_node_routes_array(node);
        for (register size_t i = 0; i < taken; i++) {
            (*new_routes_arr)[i] = (*old_routes_arr)[i];
        } //copy from old to new
        double_node_routes_arr_capacity(node);
        cast_node_routes(node, new_routes_arr);
        delete_routes_array(old_routes_arr);
    }
}
//
