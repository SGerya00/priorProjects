#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

#include "HT_v2_075.h"
#include "edges_and_nodes.h"
#include "key_and_data.h"

#define MAX_EDGES 4000

#define LOGGING_ON
#ifdef LOGGING_ON
#define MY_LOG(...) fprintf(stdout, __VA_ARGS__)
#else
#define MY_LOG(...)
#endif

void print_constraints() {
    fprintf(stdout, "\n|--------------------C O N S T R A I N T S--------------------|");
    fprintf(stdout, "\n|Programme will only read up to %u lines containing edges", MAX_EDGES);
    fprintf(stdout, "\n|Programme will only allow node numbers >= 0");
    fprintf(stdout, "\n|Negative and/or 0 edge weight is not allowed");
    fprintf(stdout, "\n|--------------------C O N S T R A I N T S--------------------|");
}

int reach_EOL(FILE *file) {
    assert(NULL != file);
    fscanf(file, "%*[^\n]"); //skip all that is not \n
    int temp = fgetc(file); //get next
    if ('\n' == (char)temp) {
        return 1;
    } else { // EOF == temp
        return 0;
    }
} //1 if now at the start of a new line, 0 if EOF

int skip_ws_check_EOL(FILE *file) {
    assert(NULL != file);

    fscanf(file, "%*[ ]"); //skip white-spaces
    int read_char = fgetc(file);
    if ('\n' == read_char) {
        fseek(file, -1, SEEK_CUR); //because fgetc
        return -1; //no edge in current line
    } else if (EOF == read_char) {
        return 0;
    } else {
        fseek(file, -1, SEEK_CUR); //because fgetc
    }
    return 1;
} //skips ws's and checks what is after them (-1 if \n, 0 if EOF, otherwise 1)

void skip_rest_of_number(FILE *file) {
    assert(NULL != file);

    fscanf(file, "%*[^ \n]");
} //skips everything except ws and \n

void alert(const unsigned short int line, const short int code) {

    fprintf(stdout, "\nA: in line %hu:", line);
    switch(code) {
        case(0): fprintf(stdout, " unable to read \"from\", \"to\" and/or \"weight\" value");
            return;
        case(1): fprintf(stdout, " parameter \"%s\" is < 0", "from");
            return;
        case(2): fprintf(stdout, " parameter \"%s\" is < 0", "to");
            return;
        case(3): fprintf(stdout, " parameter \"%s\" is <= 0", "weight");
            return;
    }
} //0-3 //returns reach_EOL result

int check_edge(struct read_edge *read_edge, unsigned short int line) {

    int good_flag = 0;
    if (NULL == read_edge) {
        alert(line, 0);
        good_flag = -1;
    } else if (get_read_edge_from(read_edge) < 0) {
        alert(line, 1);
    } else if (get_read_edge_to(read_edge) < 0) {
        alert(line, 2);
    } else if (get_read_edge_weight(read_edge) <= 0) {
        alert(line, 3);
    } else {
        good_flag = 1;
    }
    return good_flag;
}
//returns 1 if good edge, 0 if one of 3 parameters is bad, -1 if *edge is NULL

struct read_edge * read_edge_make_read_edge(FILE *file) {
    assert(NULL != file);

    long long int from = 0;
    long long int to = 0;
    double weight = 0;
    //read from
    int local_result = skip_ws_check_EOL(file);
    if (1 != local_result) {
        return NULL; //edge was not successfully read
    }
    int result = fscanf(file, "%10lli", &from);
    if (1 != result) {
        return NULL; //edge was not successfully read
    }
    //
    skip_rest_of_number(file);
    //read to
    local_result = skip_ws_check_EOL(file);
    if (1 != local_result) {
        return NULL; //edge was not successfully read
    }
    result = fscanf(file, "%10lli", &to);
    if (1 != result) {
        return NULL; //edge was not successfully read
    }
    //
    skip_rest_of_number(file);
    //read weight
    local_result = skip_ws_check_EOL(file);
    if (1 != local_result) {
        return NULL; //edge was not successfully read
    }
    result = fscanf(file, "%10lf", &weight);
    if (1 != result) {
        return NULL; //edge was not successfully read
    }
    //
    //at this point, 3 parameters are supposed to be read
    MY_LOG("\n---edge is read, it has [%lli], [%lli] and [%f]", from, to, weight);
    struct read_edge *new_edge = make_new_read_edge(weight, from, to);
    return new_edge; //all 3 parameters read
} //returns pointer to newly created edge, NULL if some parameters were not read

void fill_HT_with_nodes(FILE *file, struct HT *hash_table) {
    assert(NULL != file);
    assert(NULL != hash_table);

    //read edges, fill HT with nodes
    for (register unsigned int line_counter = 1; line_counter <= MAX_EDGES; line_counter++) {

        struct read_edge *new_read_edge = read_edge_make_read_edge(file);
        //check if edge is good to work with
        int check_result = check_edge(new_read_edge, line_counter);
        if (-1 == check_result) {
            if (1 != reach_EOL(file)) {
                break; //EOF reached, stopping reading
            } else {
                continue;
            }
        } else if (0 == check_result) {
            delete_read_edge(new_read_edge);
            if (1 != reach_EOL(file)) {
                break; //EOF reached, stopping reading
            } else {
                continue;
            }
        }
        //
        //connect nodes using edge (create nodes and put them in hash_table if necessary)
        MY_LOG("\n---edge is good, working with it");
        long long int from = get_read_edge_from(new_read_edge); //from serves as both key for HT_element and index for node
        MY_LOG("\n   from is (%llu)", from);
        long long int to = get_read_edge_to(new_read_edge); //to serves as both key for HT_element and index for node
        MY_LOG("\n   to is (%llu)", to);
        struct node *node_from = (struct node *)search_in_HT(hash_table, (KEY_TYPE)from); //find from in hash_table
        if (NULL == node_from) {
            MY_LOG("\n   didn't find node_from in HT, making it");
            node_from = create_new_node((KEY_TYPE)from);
            put_in_HT(hash_table, (KEY_TYPE)from, node_from);
        } //if not in hash_table, create such element and put in hash_table
        struct node *node_to = (struct node *)search_in_HT(hash_table, (KEY_TYPE)to); //find to in hash_table
        if (NULL == node_to) {
            MY_LOG("\n   didn't find node_to in HT, making it");
            node_to = create_new_node((KEY_TYPE)to);
            put_in_HT(hash_table, (KEY_TYPE)to, node_to);
        } //if not in hash_table, create such element and put in hash_table
        //at this point we have both from and to and are ready to create edge (not read_edge)
        MY_LOG("\n   putting actual and reverse edges in nodes");
        struct edge *actual_edge = create_edge(get_read_edge_weight(new_read_edge), node_from, node_to);
        size_t index = get_node_routes_arr_taken(node_from);
        put_edge_in_node(node_from, actual_edge, index); //do edge
        struct edge *reverse_actual = get_edge_reverse_same(actual_edge);
        index = get_node_routes_arr_taken(node_to);
        put_edge_in_node(node_to, reverse_actual, index);

        delete_read_edge(new_read_edge); //information was "drained", don't need it anymore
        //
        //if there is a need to expand -> expand
        expand_node_routes_array_if_nec(node_from);
        expand_node_routes_array_if_nec(node_to);

        if (0 == reach_EOL(file)) {
            break; //EOF reached, stopping reading
        }
    }
    //
}

int read_dep_and_dest(size_t *departure, size_t *destination) {
    assert(NULL != departure);
    assert(NULL != destination);
    fprintf(stdout, "\n\n\nPlease, enter index of source and dump: ");
    long long int local_departure = 0;
    long long int local_destination = 0;
    int result = fscanf(stdin, "%lli %lli", &local_departure, &local_destination);
    if (2 != result) {
        fprintf(stdout, "\nIncorrect input, expected *>=0* *>=0* (for departure and destination)");
    } else if (local_departure < 0) {
        fprintf(stdout, "\nDeparture is expected to be >= 0");
    } else if (local_destination < 0) {
        fprintf(stdout, "\nDestination is expected to be >= 0");
    } else {
        *departure = (size_t)local_departure;
        *destination = (size_t)local_destination;
        return 0;
    }
    return -1;
}

//
struct queue_elem {
    struct queue_elem *above;
    struct queue_elem *below;
    struct node *node;
};

int q_is_empty(struct queue_elem *const first, struct queue_elem *const last) {
    assert(NULL != first);
    assert(NULL != last);

    if (last->above == first) {
        return 1;
    } else {
        return 0;
    }
}

struct queue_elem *create_q_elem(struct node *const node) {
    assert(NULL != node);

    struct queue_elem *new_q_elem = (struct queue_elem *)calloc(1, sizeof(struct queue_elem));
    assert(NULL != new_q_elem);
    new_q_elem->node = node;
    new_q_elem->above = NULL;
    new_q_elem->below = NULL;

    return new_q_elem;
}

void insert_in_q(struct queue_elem *const for_insertion, struct queue_elem *const last) {
    assert(NULL != for_insertion);
    assert(NULL != last);

    for_insertion->below = last;
    for_insertion->above = last->above;
    last->above = for_insertion;
    for_insertion->above->below = for_insertion;
}

struct queue_elem *get_from_q(struct queue_elem *first) {
    assert(NULL != first);

    return first->below;
}

void delete_q_elem(struct queue_elem *elem) {
    assert(NULL != elem);

    free(elem);
}

void remove_from_q(struct queue_elem *elem) {
    assert(NULL != elem);

    elem->above->below = elem->below;
    elem->below->above = elem->above;
    delete_q_elem(elem);
}

void erase_q(struct queue_elem *first, struct queue_elem *last) {
    assert(NULL != first);
    assert(NULL != last);

    while(1) {
        struct queue_elem *current = first->below;
        if (current == last) {
            break;
        }
        remove_from_q(current);
    }
    delete_q_elem(first);
    delete_q_elem(last);
}

struct node *BFS(struct node *const source, struct node *const dump) {
    assert(NULL != source);
    assert(NULL != dump);
    MY_LOG("\n-----in {%s}-----\n", __FUNCTION__);
    struct queue_elem *first = (struct queue_elem *)calloc(1, sizeof(struct queue_elem));
    assert(NULL != first);
    struct queue_elem *last = (struct queue_elem *)calloc(1, sizeof(struct queue_elem));
    assert(NULL != last);
    first->below = last;
    last->above = first;

    struct queue_elem *initial = create_q_elem(source);
    insert_in_q(initial, last);

    int path_was_found_flag = 0;
    while(!q_is_empty(first, last)) {
        struct queue_elem *current = get_from_q(first);
        MY_LOG("\ngot (%zu) from queue", get_node_index(current->node));
        //
        struct node *current_node = current->node;
        change_node_BFS_visited_flag_to_visited(current_node);
        //
        remove_from_q(current); //removing it from queue
        size_t taken = get_node_routes_arr_taken(current_node);
        for (register size_t i = 0; i < taken; i++) {
            struct edge *current_edge = get_node_edge(current_node, i);
            MY_LOG("\nedge #%zu is ", i);
            if (get_edge_remaining_capacity(current_edge) <= (double)0 ) {
                MY_LOG("<= 0");
                continue;
            } //if cannot travel through this edge, skip it
            MY_LOG("fine");
            struct node *neighbour = get_edge_to(current_edge);
            MY_LOG(", it leads to neighbour (%zu), it is good", get_node_index(neighbour));
            if (1 == get_node_BFS_visited_flag(neighbour)) {
                MY_LOG(" (no, it is visited)");
                continue;
            } //if node is already visited, skip it (and edge leading to it)
            cast_node_BFS_parent(neighbour, current_node);
            change_node_BFS_visited_flag_to_visited(neighbour);
            if (neighbour == dump) {
                path_was_found_flag = 1;
                break;
            } //if shortest path is found
            MY_LOG(" and it was not destination (not dump)");
            struct queue_elem *new_q_elem = create_q_elem(neighbour);
            insert_in_q(new_q_elem, last);
        }
        if (1 == path_was_found_flag) {
            break;
        }
    }
    erase_q(first, last);
    if (0 == path_was_found_flag) {
        return NULL;
    } else {
        return dump;
    }
} //returns NULL if path was not found, pointer to dump otherwise
//

void reset_visited_flags_and_parents(struct HT *const hash_table) {
    assert(NULL != hash_table);

    size_t capacity = get_HT_capacity(hash_table);
    for (register size_t i = 0; i < capacity; i++) {
        if (slot_is_empty(hash_table, i)) {
            continue;
        } else {
            struct node * node = (struct node *)get_element_from_HT(hash_table, i);
            change_node_BFS_visited_flag_to_not_visited(node);
            cast_node_BFS_parent(node, NULL);
        }
    }
}

void modify_graph(struct node *dump) {
    assert(NULL != dump);
    MY_LOG("\n\nin {%s}", __FUNCTION__);
    //finding bottleneck
    double minimum = (double)0;
    struct node *current = dump;
    MY_LOG("\ndump is (%zu)", get_node_index(current));
    while(1) {
        MY_LOG("\ncurrent is (%zu)", get_node_index(current));
        MY_LOG(", it's parent is ");
        struct node *parent = get_node_BFS_parent(current);
        if (NULL == parent) {
            MY_LOG("no parent");
            break;
        }
        MY_LOG("[%zu]", get_node_index(parent));
        size_t index = get_node_routes_arr_taken(parent);
        for (register size_t i = 0; i < index; i++) {
            struct edge *current_edge = get_node_edge(parent, i);
            if ((get_edge_to(current_edge) == current) && (get_edge_remaining_capacity(current_edge) > 0) ) {
                index = i;
                break;
            }
        }
        struct edge *to_current = get_node_edge(parent, index);
        MY_LOG(", edge leading to it has F_C of [%f] and E_F of [%f]", get_edge_flow_capacity(to_current),
                                                                       get_edge_existing_flow(to_current));
        double remaining_capacity = get_edge_remaining_capacity(to_current);
        if ((remaining_capacity < minimum) || (minimum == (double)0)) {
            minimum = remaining_capacity;
        }
        current = parent;
    }
    //
    MY_LOG("\nin {%s} minimum is found, it is [%f]", __FUNCTION__, minimum);
    //actually modifying
    current = dump;
    MY_LOG("\nin {%s} path is (backwards):", __FUNCTION__);
    while(1) {
        struct node *parent = get_node_BFS_parent(current);
        if (NULL == parent) {
            break;
        }
        MY_LOG(" (%zu)", get_node_index(parent));
        size_t index = get_node_routes_arr_taken(parent);
        for (register size_t i = 0; i < index; i++) {
            struct edge *current_edge = get_node_edge(parent, i);
            if ((get_edge_to(current_edge) == current) && (get_edge_remaining_capacity(current_edge) > 0) ) {
                index = i;
                break;
            }
        }
        struct edge *to_current = get_node_edge(parent, index);
        add_to_edge_existing_flow(to_current, minimum);
        current = parent;
    }
    //
}

double calculate_max_flow(struct node *const dump) {
    assert(NULL != dump);

    double summ = (double)0;
    size_t taken = get_node_routes_arr_taken(dump);
    for (register size_t i = 0; i < taken; i++) {
        struct edge * edge = get_node_edge(dump, i);
        if (get_edge_flow_capacity(edge) == 0) {
            summ += (-1) * get_edge_existing_flow(edge);
        }
    }

    return summ;
}

double algorithm(struct HT *const hash_table, size_t departure, size_t destination) {
    assert(NULL != hash_table);

    struct node *dep_node = search_in_HT(hash_table, departure);
    if (NULL == dep_node) {
        fprintf(stdout, "\nNode [%zu] does not exist", departure);
        return (double)0;
    }
    struct node *dest_node = search_in_HT(hash_table, destination);
    if (NULL == dest_node) {
        fprintf(stdout, "\nNode [%zu] does not exist", destination);
        return (double)0;
    }

    while(1) {
        MY_LOG("\nresetting flags and parents...");
        reset_visited_flags_and_parents(hash_table);
        MY_LOG("\ndoing BFS...");
        struct node *dump = BFS(dep_node, dest_node);
        if (NULL == dump) {
            break;
        }
        MY_LOG("\nmodifying graph...");
        modify_graph(dump);
    }
    MY_LOG("\ncalculating max flow...");
    double max_flow = calculate_max_flow(dest_node);
    MY_LOG("\nalgorithm done");
    return max_flow;
}

//----------------------------------------------------------------------------------------------------------------------
void print_HT(struct HT *const hash_table) {
    assert(NULL != hash_table);
    printf("\nPrinting HT...\n");
    size_t capacity = get_HT_capacity(hash_table);
    for (register size_t index = 0; index < capacity; index++) {
        struct node *node = (struct node*)get_element_from_HT(hash_table, index);
        if (NULL == node) {
            continue;
        } //this slot is empty
        printf("\nIn slot #%zu we have node [%zu]", index, get_node_index(node));
        size_t taken = get_node_routes_arr_taken(node);
        printf(", it has [%zu] edges", taken);
        for (register size_t i = 0; i < taken; i++) {
            struct edge *its_edge = get_node_edge(node, i);
            printf("\n----looking at edge #%zu", i);
            size_t neighbours_index = get_node_index(  get_edge_to(its_edge)  );
            printf("\n----it has flow_capacity of (%f) and existing_flow of (%f), leads to node [%zu]",
                   get_edge_flow_capacity(its_edge), get_edge_existing_flow(its_edge), neighbours_index);
        }
    }
    printf("\nPrinting HT done.");
}
//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    if (2 != argc) {
        fprintf(stderr, "\nUnexpected number of arguments (%d), expected 1", argc - 1);
        return -1;
    }
    //input file
    FILE *file = fopen(argv[1], "rt");
    assert(NULL != file);
    //
    print_constraints();
    //create HT
    struct HT *hash_table = create_HT();
    //
    fill_HT_with_nodes(file, hash_table);
    //
    //close file as we will not use it later
    fclose(file);
    //
    //check-------------------------------------------------------------------------------------------------------------
    print_HT(hash_table);
    //check-------------------------------------------------------------------------------------------------------------
    //departure and destination
    size_t departure = 0;
    size_t destination = 0;
    int result = read_dep_and_dest(&departure, &destination);
    if (-1 == result) {
        delete_HT(hash_table);
        return -1;
    }
    //
    //actual algorithm
    double max_flow = algorithm(hash_table, departure, destination);
    fprintf(stdout, "\nMax flow is... {%f}", max_flow);
    //
    //delete HT
    delete_HT(hash_table);
    //
    return 0;
}
