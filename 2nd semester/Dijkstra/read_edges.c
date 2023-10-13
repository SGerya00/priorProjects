#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "key_and_data.h"
#include "edges_and_nodes.h"
#include "HT_v2_075.h"

#define MAX_EDGES 4000

//#define LOGGING_ON
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

void read_Dep_and_Dest(long long int *dep, long long int *dest) {
    fprintf(stdout, "\nPlease, enter point of departure and point of destination: ");
    fscanf(stdin, "%10lli %10lli", dep, dest);
}

int check_Dep_and_Dest(long long int dep, long long int dest) {

    if (dep < 0) {
        fprintf(stdout, "\nDeparture point is <0, should be >=0");
        return -1;
    } else if (dest < 0) {
        fprintf(stdout, "\nDestination point is <0, should be >=0");
        return -1;
    }
    return 0;
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

void skip_rest_of_number(FILE *file) {
    assert(NULL != file);

    fscanf(file, "%*[^ \n]");
} //skips everything except ws and \n

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

int check_edge(struct edge *edge, unsigned short int line) {

    int good_flag = 0;
    if (NULL == edge) {
        alert(line, 0);
        good_flag = -1;
    } else if (get_edge_from(edge) < 0) {
        alert(line, 1);
    } else if (get_edge_to(edge) < 0) {
        alert(line, 2);
    } else if (get_edge_weight(edge) <= 0) {
        alert(line, 3);
    } else {
        good_flag = 1;
    }
    return good_flag;
}
//returns 1 if good edge, 0 if one of 3 parameters is bad, -1 if *edge is NULL

struct edge * read_edge_make_edge(FILE *file) {
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
    struct edge *new_edge = make_new_edge(weight, from, to);
    return new_edge; //all 3 parameters read
} //returns pointer to newly created edge, NULL if some parameters were not read

void fill_HT_with_nodes(FILE *file, struct HT *hash_table) {
    assert(NULL != file);
    assert(NULL != hash_table);

    //read edges, fill HT with nodes
    for (register unsigned int line_counter = 1; line_counter <= MAX_EDGES; line_counter++) {

        struct edge *new_edge = read_edge_make_edge(file);
        //check if edge is good to work with
        int check_result = check_edge(new_edge, line_counter);
        if (-1 == check_result) {
            if (1 != reach_EOL(file)) {
                break; //EOF reached, stopping reading
            } else {
                continue;
            }
        } else if (0 == check_result) {
            delete_edge(new_edge);
            if (1 != reach_EOL(file)) {
                break; //EOF reached, stopping reading
            } else {
                continue;
            }
        }
        //
        //connect nodes using edge (create nodes and put them in hash_table if necessary)
        MY_LOG("\n---edge is good, working with it");
        long long int from = get_edge_from(new_edge); //from serves as both key for HT_element and index for node
        MY_LOG("\n   from is (%llu)", from);
        long long int to = get_edge_to(new_edge); //to serves as both key for HT_element and index for node
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
        MY_LOG("\n   putting edge in node");
        put_edge_in_node(new_edge, node_from, node_to); //do edge
        delete_edge(new_edge); //information was "drained", don't need it anymore
        //if there is a need to expand -> expand
        if (1 == node_neighbours_DA_needs_expanding(node_from)) {
            MY_LOG("\n   need to increase neighbours_DA for from node...");
            increase_neighbours_DA(node_from);
            MY_LOG("\n   increasing done");
        }
        //now hash_table is either same, or 1 element or 2 elements bigger

        if (0 == reach_EOL(file)) {
            break; //EOF reached, stopping reading
        }
    }
    //
}

struct queue_elem {
    struct node *p_node;
    struct queue_elem *above;
    struct queue_elem *below;
};

int q_is_not_empty(struct queue_elem *first, struct queue_elem *last) {
    assert(NULL != first);
    assert(NULL != last);

    if ((first->below == last) || (last->above == first)) {
        return 0;
    } else {
        return 1;
    }
} //returns 1 if not empty, 0 otherwise

struct queue_elem * make_q_element(struct node *node_elem) {
    assert(NULL != node_elem);
    struct queue_elem *new_q_elem = (struct queue_elem*)calloc(1, sizeof(struct queue_elem));
    new_q_elem->p_node = node_elem;
    return new_q_elem;
}

void insert_in_q(struct queue_elem *before_this_one, struct queue_elem *current) {
    assert(NULL != before_this_one);
    assert(NULL != current);

    current->above = before_this_one->above;
    current->below = before_this_one;
    before_this_one->above = current;
    current->above->below = current;
}

void priority_insert_in_q(struct queue_elem *const last, struct queue_elem *const first,
                          struct queue_elem *const current) {
    assert(NULL != last);
    assert(NULL != first);
    assert(NULL != current);

    struct queue_elem *after_this_one = last->above; //worst case scenario - it is first, otherwise an actual node
    struct node *current_node = current->p_node;
    double current_nodes_path = get_node_shortest_path(current_node);

    //looking for a place to insert
    while(1) {
        if (after_this_one == first) {
            insert_in_q(after_this_one->below, current);
            break;
        }
        double after_this_ones_path = get_node_shortest_path(after_this_one->p_node);
        if (after_this_ones_path < current_nodes_path) {
            insert_in_q(after_this_one->below, current);
            break;
        } else {
            after_this_one = after_this_one->above;
        }
    }
    //inserting above element with shorter (smaller) shortest_path
}

int is_in_q(struct node *const node, struct queue_elem *const first, struct queue_elem *const last) {
    assert(NULL != node);
    assert(NULL != first);
    assert(NULL != last);

    struct queue_elem *current = first->below;
    while(1) {
        if (current == last) {
            return 0;
        }
        if (current->p_node == node) {
            return 1;
        } else {
            current = current->below;
        }
    }
}
//1 if is, 0 if isn't

void remove_from_q_and_free(struct queue_elem *current) {
    assert(NULL != current);

    current->above->below = current->below;
    current->below->above = current->above;
    //current->p_node is not supposed to be freed here, it is freed later
    free(current);
}

double find_shortest_path_from_dep_to_dest(struct HT *hash_table, const long long int dep, const long long int dest) {
    assert(NULL != hash_table);

    //find (or not) both nodes with such indexes
    MY_LOG("\nTrying to find departure node...");
    struct node *dep_node = (struct node*)search_in_HT(hash_table, (size_t)dep);
    if (NULL == dep_node) {
        fprintf(stdout, "\nNo node with index [%lli] exists", dep);
        return 0; //no such node
    }
    MY_LOG("departure found, it is [%zu]", get_node_index(dep_node));
    MY_LOG("\nTrying to find destination node...");
    struct node *dest_node = (struct node*)search_in_HT(hash_table, (size_t)dest);
    if (NULL == dest_node) {
        fprintf(stdout, "\nNo node with index [%lli] exists", dest);
        return 0; //no such node
    }
    MY_LOG("destination found, it is [%zu]", get_node_index(dest_node));
    //

    //make queue (dijkstra algorithm requires one)
    MY_LOG("\n   creating 2 queue elements: first and last");
        //two initial elements
    struct queue_elem *first = (struct queue_elem*)calloc(1, sizeof(struct queue_elem)); //free
    assert(NULL != first);
    MY_LOG(" (done) ");
    struct queue_elem *last = (struct queue_elem*)calloc(1, sizeof(struct queue_elem)); //free
    assert(NULL != last);
    MY_LOG(" (done) ");
    first->below = last;
    last->above = first;
        //
        //make first queue element (departure)
    MY_LOG("\n   Making first element...");
    struct queue_elem *dep_q_elem = make_q_element(dep_node);
    MY_LOG("done, inserting in queue...");
    priority_insert_in_q(last, first, dep_q_elem);
    MY_LOG("done");
        //
    while(q_is_not_empty(first, last)) {
        struct queue_elem *q_top = first->below;
        MY_LOG("\n---took first queue element, it is [%zu]", get_node_index(q_top->p_node));
        struct node *q_top_node = q_top->p_node;
        size_t taken = get_node_taken(q_top_node);
        MY_LOG("\n---it's neighbours are: ");
        for (register size_t i = 0; i < taken; i++) {
            struct node *neighbour = get_node_neighbour_index(q_top_node, i);
            MY_LOG(" [%zu], ", get_node_index(neighbour));
            if (1 == get_node_done_flag(neighbour)) {
                continue;
            } //if neighbouring node is already worked with, skip it
            double neighbours_path = get_node_shortest_path(neighbour);
            MY_LOG("it has {%f} path len", neighbours_path);
            double shortest_path = get_node_weight_index(q_top_node, i) + get_node_shortest_path(q_top_node);
            MY_LOG(" and we are offering {%f}", shortest_path);
            if ( (neighbours_path > shortest_path) || ((double)0 == neighbours_path) ) {
                MY_LOG(" -> making a change");
                change_node_shortest_path(neighbour, shortest_path);
            }
            if (!(is_in_q(neighbour, first, last))) {
                MY_LOG(" btw neighbour is not in queue, inserting");
                MY_LOG(" (making q element...");
                struct queue_elem *for_q_neighbour = make_q_element(neighbour);
                MY_LOG("done, inserting...");
                priority_insert_in_q(last, first, for_q_neighbour);
                MY_LOG("done)");
            } //if not already in queue, add it
        }
        MY_LOG("\n for node [%zu] changing flag from not_done to done", get_node_index(q_top_node));
        change_node_done_flag_to_done(q_top_node);
        MY_LOG("\n removing it from queue...");
        remove_from_q_and_free(q_top);
        MY_LOG("done");
    }
    free(first);
    free(last);
    //
    MY_LOG("\n\n loop is over, getting shortest path from dest...");
    double shortest_path = get_node_shortest_path(dest_node);
    MY_LOG(" and got it, it is {%f}", shortest_path);
    if ((double)0 == shortest_path) {
        fprintf(stdout, "\nNo path from node [%lli] to node [%lli] exists", dep, dest);
        return 0;
    } else {
        return shortest_path;
    }
}
//returns 0 if no such path exists, otherwise weight of path
//----------------------------------------------------------------------------------------------------------------------
void print_HT(struct HT *HT) {
    assert(NULL != HT);

    size_t capacity = get_HT_capacity(HT);
    for (register size_t index = 0; index < capacity; index++) {
        struct node *node = (struct node*)get_element_from_HT(HT, index);
        if (NULL == node) {
            continue;
        } //this slot is empty
        MY_LOG("\nIn slot #%zu we have node [%zu]", index, index);
        MY_LOG(", it's shortest path is [%f]", get_node_shortest_path(node));
        size_t taken = get_node_taken(node);
        MY_LOG(", it's taken is [%zu]", taken);
        for (register size_t i = 0; i < taken; i++) {
            MY_LOG("\n   in slot (%zu) it has weight {%f}", i, get_node_weight_index(node, i));
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    if (2 != argc) {
        fprintf(stderr, "\nUnexpected [%d] number of arguments, expected 1", argc - 1);
        return 0;
    }
    MY_LOG("\nOpening file...");
    FILE *file = fopen(argv[1], "rt");
    assert(NULL != file);
    MY_LOG("file is open");
    //
    MY_LOG("\nPrinting constraints...");
    print_constraints();
    MY_LOG("\nconstraints are printed");
    //
    MY_LOG("\nCreating hash table...");
    struct HT *hash_table = create_HT();
    MY_LOG("\nhash table created");
    //
    MY_LOG("\nFilling hash table with nodes...");
    fill_HT_with_nodes(file, hash_table);
    MY_LOG("\nhash table filled");
    //
    //------------------------------------------------------------------------------------------------------------------
    print_HT(hash_table);
    //------------------------------------------------------------------------------------------------------------------
    //sort neighbours_DA for existing elements
    MY_LOG("\nSorting neighbours_DAs in loop...");
    size_t capacity = get_HT_capacity(hash_table);
    for (register size_t index = 0; index < capacity; index++) {
        struct node *node = (struct node*)get_element_from_HT(hash_table, index);
        if (NULL == node) {
            continue;
        } //this slot is empty
        sort_neighbours_DA_in_node(node);
    }
    MY_LOG("\nsorting done");
    //
    //------------------------------------------------------------------------------------------------------------------
    print_HT(hash_table);
    //------------------------------------------------------------------------------------------------------------------
    //
    MY_LOG("\nclosing file...");
    fclose(file);
    MY_LOG("\nfile is closed");
    //
    long long int departure = 0;
    long long int destination = 0;
    MY_LOG("\nReading departure and destination...");
    read_Dep_and_Dest(&departure, &destination);
    if (0 != check_Dep_and_Dest(departure, destination)) {
        MY_LOG("\nDeleting hash table...");
        delete_HT(hash_table);
        MY_LOG("\ndone");
        return 0;
    }
    MY_LOG("\nreading done");
    MY_LOG("\nFinding shortest path...");
    double shortest_path = find_shortest_path_from_dep_to_dest(hash_table, departure, destination);
    MY_LOG("\nshortest path found");
    fprintf(stdout, "\nShortest path is {%f}", shortest_path);
    //
    MY_LOG("\nDeleting hash table...");
    delete_HT(hash_table);
    MY_LOG("\ndone");
    //
    return 0;
}
