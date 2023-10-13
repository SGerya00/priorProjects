#include "edges_and_nodes.h"

//#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define INITIAL_CAPACITY 4
#define DONE 1
#define NOT_DONE 0

struct edge {
    double weight;
    long long int from;
    long long int to;
};

//a pointer to this struct will be .data in HT_element
struct node {
    size_t index;
    unsigned short int done_flag; //0 if not done, 1 otherwise
    double shortest_path_from_PoD; //Point of Departure
    struct neighbours_DA { //double array
        size_t capacity;
        size_t taken;
        //two following arrays are filled from left to right without blank elements between them
        double (*AO_weights)[]; //array of
        struct node *(*AO_neighbours)[]; //array of
    } *neighbours_DA;
};

//
double get_edge_weight(struct edge *const edge) {
    assert(NULL != edge);

    return edge->weight;
}

long long int get_edge_from(struct edge *const edge) {
    assert(NULL != edge);

    return edge->from;
}

long long int get_edge_to(struct edge *const edge) {
    assert(NULL != edge);

    return edge->to;
}
//

//
struct edge * create_blank_edge() {
    struct edge *edge = (struct edge*)calloc(1, sizeof(struct edge));
    assert(NULL != edge);
    return edge;
} //allocates memory

struct edge * make_new_edge(double weight, long long int from, long long int to) {

    struct edge *new_edge = create_blank_edge();
    new_edge->weight = weight;
    new_edge->from = from;
    new_edge->to = to;
    return new_edge;
} //returns pointer to newly created edge

void delete_edge(struct edge *edge) {
    assert(NULL != edge);

    free(edge);
}
//

//
size_t get_neighbours_capacity(struct neighbours_DA *const neighbours_DA) {
    assert(NULL != neighbours_DA);

    return neighbours_DA->capacity;
}

size_t get_neighbours_taken(struct neighbours_DA *const neighbours_DA) {
    assert(NULL != neighbours_DA);

    return neighbours_DA->taken;
}

double (*get_AO_weights(struct neighbours_DA *const neighbours_DA))[] {
    assert(NULL != neighbours_DA);

    return neighbours_DA->AO_weights;
}

struct node *(*get_AO_neighbours(struct neighbours_DA *const neighbours_DA))[] {
    assert(NULL != neighbours_DA);

    return neighbours_DA->AO_neighbours;
}

void increase_neighbours_DA_taken(struct neighbours_DA *const neighbours_DA) {
    assert(NULL != neighbours_DA);

    neighbours_DA->taken += 1;
}
//
//
size_t get_node_index(struct node *const node) {
    assert(NULL != node);

    return node->index;
}

unsigned short int get_node_done_flag(struct node *const node) {
    assert(NULL != node);

    return node->done_flag;
}

void change_node_done_flag_to_done(struct node *const node) {
    assert(NULL != node);

    node->done_flag = DONE;
}

double get_node_shortest_path(struct node *const node) {
    assert(NULL != node);

    return node->shortest_path_from_PoD;
}

void change_node_shortest_path(struct node *const node, double shortest_path) {
    assert(NULL != node);

    node->shortest_path_from_PoD = shortest_path;
}

struct neighbours_DA * get_node_neighbours_DA(struct node *const node) {
    assert(NULL != node);

    return node->neighbours_DA;
}

size_t get_node_capacity(struct node *const node) {
    assert(NULL != node);

    struct neighbours_DA *neighbours_DA = get_node_neighbours_DA(node);
    size_t capacity = get_neighbours_capacity(neighbours_DA);
    return capacity;
}

size_t get_node_taken(struct node *const node) {
    assert(NULL != node);

    struct neighbours_DA *neighbours_DA = get_node_neighbours_DA(node);
    size_t taken = get_neighbours_taken(neighbours_DA);
    return taken;
}

double get_node_weight_index(struct node *const node, size_t index) {
    assert(NULL != node);

    struct neighbours_DA *neighbours_DA = get_node_neighbours_DA(node);
    double (*AO_weights)[] = get_AO_weights(neighbours_DA);
    double weight = (*AO_weights)[index];
    return weight;
}

struct node * get_node_neighbour_index(struct node *const node, size_t index) {
    assert(NULL != node);

    struct neighbours_DA *neighbours_DA = get_node_neighbours_DA(node);
    struct node *(*AO_neighbours)[] = get_AO_neighbours(neighbours_DA);
    struct node *neighbour = (*AO_neighbours)[index];
    return neighbour;
}

int node_neighbours_DA_needs_expanding(struct node *const node) {
    assert(NULL != node);

    size_t capacity = get_node_capacity(node);
    size_t taken = get_node_taken(node);
    if (capacity == taken) {
        return 1;
    } else {
        return 0;
    }
} //returns 1 if capacity == taken, 0 otherwise
//

//
double (*make_AO_weights(size_t size))[] {
    double (*AO_weights)[] = (double (*)[])calloc(size, sizeof(double));
    assert(NULL != AO_weights);
    return AO_weights;
} //allocates memory

struct node *(*make_AO_neighbours(size_t size))[] {
    struct node *(*AO_neighbours)[] = (struct node *(*)[])calloc(size, sizeof(struct node *));
    assert(NULL != AO_neighbours);
    return AO_neighbours;
} //allocates memory

struct neighbours_DA * create_blank_neighbours_DA() {
    struct neighbours_DA *neighbours = (struct neighbours_DA*)calloc(1, sizeof(struct neighbours_DA));
    assert(NULL != neighbours);
    return neighbours;
} //allocates memory

void fill_neighbours_DA(struct neighbours_DA *const neighbours_DA, const size_t capacity, const size_t taken,
                        double (*AO_weights)[], struct node *(*AO_neighbours)[]) {
    assert(NULL != neighbours_DA);
    assert(NULL != AO_weights);
    assert(NULL != AO_neighbours);

    neighbours_DA->capacity = capacity;
    neighbours_DA->taken = taken;
    neighbours_DA->AO_weights = AO_weights;
    neighbours_DA->AO_neighbours = AO_neighbours;
}

struct node * create_blank_node() {
    struct node *new_node = (struct node *)calloc(1, sizeof(struct node));
    assert(NULL != new_node);
    return new_node;
} //allocates memory

void fill_node(struct node *const node, const size_t index, unsigned short int done_flag,
               const double shortest_path, struct neighbours_DA *const neighbours_DA) {
    assert(NULL != node);
    assert(NULL != neighbours_DA);

    node->index = index;
    node->done_flag = done_flag;
    node->shortest_path_from_PoD = shortest_path;
    node->neighbours_DA = neighbours_DA;
}

struct node *create_new_node(const size_t index) {
    double (*AO_weights)[] = make_AO_weights(INITIAL_CAPACITY);
    struct node *(*AO_neighbours)[] = make_AO_neighbours(INITIAL_CAPACITY);
    struct neighbours_DA *neighbours_DA = create_blank_neighbours_DA();
    fill_neighbours_DA(neighbours_DA, INITIAL_CAPACITY, 0, AO_weights, AO_neighbours);
    struct node *new_node = create_blank_node();
    fill_node(new_node, index, NOT_DONE, 0, neighbours_DA);
    return new_node;
} //returns pointer to newly created node
//

//
void free_AO_weights(double (*AO_weights)[]) {
    assert(NULL != AO_weights);

    free(AO_weights);
}

void free_AO_neighbours(struct node *(*AO_neighbours)[]) {
    assert(NULL != AO_neighbours);
    //should not free memory to which element of array are pointing
    free(AO_neighbours);
}

void free_neighbours_DA(struct neighbours_DA *neighbours_DA) {
    assert(NULL != neighbours_DA);
    double (*for_free_AO_weights)[] = get_AO_weights(neighbours_DA);
    free_AO_weights(for_free_AO_weights);
    struct node *(*for_free_AO_neighbours)[] = get_AO_neighbours(neighbours_DA);
    free_AO_neighbours(for_free_AO_neighbours);
    free(neighbours_DA);
}

void free_node(struct node *node) {
    assert(NULL != node);

    struct neighbours_DA *neighbours_DA = get_node_neighbours_DA(node);
    free_neighbours_DA(neighbours_DA);

    free(node);
}
//

struct neighbours_DA *create_expanded_neighbours_DA(struct neighbours_DA *old_neighbours_DA) {
    assert(NULL != old_neighbours_DA);

    //allocate memory for struct neighbours_DA
    struct neighbours_DA *new_neighbours_DA = create_blank_neighbours_DA();
    assert(NULL != new_neighbours_DA);
    //
    //calculate .capacity
    size_t new_capacity = (get_neighbours_capacity(old_neighbours_DA)) * 2; //new capacity is twice the old
    //
    //calculate .taken ( == old taken, it is supposed to remain the same)
    size_t new_taken = get_neighbours_taken(old_neighbours_DA);
    //
    //allocate memory for 2 arrays: weights and pointers to struct node
    double (*new_AO_weights)[] = make_AO_weights(new_capacity);
    struct node *(*new_AO_neighbours)[] = make_AO_neighbours(new_capacity);
    //cast them
    fill_neighbours_DA(new_neighbours_DA, new_capacity, new_taken, new_AO_weights, new_AO_neighbours);
    //
    //to not bother with calling these functions in loop
    double (*old_AO_weights)[] = get_AO_weights(old_neighbours_DA);
    new_AO_weights = get_AO_weights(new_neighbours_DA);

    struct node *(*old_AO_neighbours)[] = get_AO_neighbours(old_neighbours_DA);
    new_AO_neighbours = get_AO_neighbours(new_neighbours_DA);
    //
    //fill new arrays with data from old arrays
    size_t taken = get_neighbours_taken(old_neighbours_DA);
    for (register size_t index = 0; index < taken; index++) {
        (*new_AO_weights)[index] = (*old_AO_weights)[index];
        (*new_AO_neighbours)[index] = (*old_AO_neighbours)[index];
    }
    //
    //free old array
    free_neighbours_DA(old_neighbours_DA);
    //
    //return pointer to new, filled array
    return new_neighbours_DA;
}

void increase_neighbours_DA(struct node *node) {
    assert(NULL != node);

    struct neighbours_DA *old_neighbours_DA = get_node_neighbours_DA(node);
    //make new one (freeing old one inside function, do not worry)
    struct neighbours_DA *new_neighbours_DA = create_expanded_neighbours_DA(old_neighbours_DA);
    //cast to node.neighbours_DA
    node->neighbours_DA = new_neighbours_DA;
}

void sort_neighbours_DA_in_node(struct node *const node) {
    assert(NULL != node);

    size_t taken = get_node_taken(node);
    struct neighbours_DA *neighbours_DA = get_node_neighbours_DA(node);
    double (*AO_weights)[] = get_AO_weights(neighbours_DA);
    struct node *(*AO_neighbours)[] = get_AO_neighbours(neighbours_DA);

    for (register size_t i = 0; i < taken; i++) {
        for (register size_t j = 0; j < taken; j++) {
            if (get_node_weight_index(node, j) > get_node_weight_index(node, i)) {
                double weight_temp = (*AO_weights)[i];
                struct node *node_p_temp = (*AO_neighbours)[i];
                (*AO_weights)[i] = (*AO_weights)[j];
                (*AO_neighbours)[i] = (*AO_neighbours)[j];
                (*AO_weights)[j] = weight_temp;
                (*AO_neighbours)[j] = node_p_temp;
            }
        }
    }
} //bubble sort, ascending

void put_edge_in_node(struct edge *const edge,  struct node *const from, struct node *const to) {
    assert(NULL != edge);
    assert(NULL != from);
    assert(NULL != to);

    struct neighbours_DA *neighbours_DA = get_node_neighbours_DA(from);
    size_t index = get_neighbours_taken(neighbours_DA);
    double weight = get_edge_weight(edge);
    double (*AO_weights)[] = get_AO_weights(neighbours_DA);
    struct node *(*AO_neighbours)[] = get_AO_neighbours(neighbours_DA);
    (*AO_weights)[index] = weight;
    (*AO_neighbours)[index] = to;
    increase_neighbours_DA_taken(neighbours_DA); //add +1 because one slow was taken
} //follow up with node_neighbours_DA_needs_expanding(struct node *const from)
