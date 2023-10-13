#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_SIBLINGS 10
#define FLAG int

//
#define GRAPH_SIZE 10
#define INITIAL 0
//

#define OUTPUT stdout
#define PROGRESS_LOG(...) fprintf(OUTPUT, __VA_ARGS__)

#define LOGGING_ON
#ifdef LOGGING_ON
#define MY_LOG(...) fprintf(stdout, __VA_ARGS__)
#else
#define MY_LOG(...)
#endif

struct node {
    int id;
    struct node *siblings[MAX_SIBLINGS];
    size_t siblings_left;
};

struct list {
    int id;
    struct list *next;
};

void print_loop(const struct list *start) {
    assert(NULL != start);
    const struct list *copy = start;
    fprintf(OUTPUT, "\n");
    while(1) {
        fprintf(OUTPUT, "{%d} ", copy->id);
        if (start == copy->next) {
            break;
        }
        copy = copy->next;
    }
    fprintf(OUTPUT, "\n");
}

void hard_code_graph(struct node graph[]) {
    assert(NULL != graph);
    /*
    graph[0].id = 0;
    graph[0].siblings_left = 2;
    graph[0].siblings[0] = &(graph[2]);
    graph[0].siblings[1] = &(graph[3]);

    graph[1].id = 1;
    graph[1].siblings_left = 2;
    graph[1].siblings[0] = &(graph[3]);
    graph[1].siblings[1] = &(graph[4]);

    graph[2].id = 2;
    graph[2].siblings_left = 2;
    graph[2].siblings[0] = &(graph[4]);
    graph[2].siblings[1] = &(graph[0]);

    graph[3].id = 3;
    graph[3].siblings_left = 2;
    graph[3].siblings[0] = &(graph[0]);
    graph[3].siblings[1] = &(graph[1]);

    graph[4].id = 4;
    graph[4].siblings_left = 2;
    graph[4].siblings[0] = &(graph[1]);
    graph[4].siblings[1] = &(graph[2]);
    */

    ///*
    graph[0].id = 0;
    graph[0].siblings_left = 2;
    graph[0].siblings[0] = &(graph[1]);
    graph[0].siblings[1] = &(graph[1]);

    graph[1].id = 1;
    graph[1].siblings_left = 4;
    graph[1].siblings[0] = &(graph[0]);
    graph[1].siblings[1] = &(graph[0]);
    graph[1].siblings[2] = &(graph[2]);
    graph[1].siblings[3] = &(graph[2]);

    graph[2].id = 2;
    graph[2].siblings_left = 4;
    graph[2].siblings[0] = &(graph[1]);
    graph[2].siblings[1] = &(graph[1]);
    graph[2].siblings[2] = &(graph[3]);
    graph[2].siblings[3] = &(graph[5]);

    graph[3].id = 3;
    graph[3].siblings_left = 2;
    graph[3].siblings[0] = &(graph[2]);
    graph[3].siblings[1] = &(graph[4]);

    graph[4].id = 4;
    graph[4].siblings_left = 2;
    graph[4].siblings[0] = &(graph[3]);
    graph[4].siblings[1] = &(graph[5]);

    graph[5].id = 5;
    graph[5].siblings_left = 2;
    graph[5].siblings[0] = &(graph[2]);
    graph[5].siblings[1] = &(graph[4]);
     //*/
}

FLAG check_criteria(struct node graph[], size_t size_o_graph) {
    assert(NULL != graph);

    for (size_t i = 0; i < size_o_graph; i++) {
        if (0 != ((graph[i]).siblings_left % 2)) {
            fprintf(OUTPUT, "\nImpossible to find cycle, node [%zu] has uneven (%zu) number of siblings\n",
                    graph[i].id, graph[i].siblings_left);
            return -1;
        }
    }
    return 0;
} //0 if all good, -1 if uneven n_o siblings

void delete_arc(struct node *from, struct node *to) {
    assert((NULL != from) && (NULL != to));

    int flag = 0;
    for (size_t i = 0; i < MAX_SIBLINGS; i++) {
        if ((from->siblings[i] == to) && (0 == flag)) {
            MY_LOG("\n   Arc [%d -> %d] is deleted", from->id, to->id);
            from->siblings[i] = NULL;
            flag = 1;
        }
    }
    flag = 0;
    for (size_t i = 0; i < MAX_SIBLINGS; i++) {
        if ((to->siblings[i] == from) && (0 == flag)) {
            MY_LOG("\n   Arc [%d -> %d] is deleted", to->id, from->id);
            to->siblings[i] = NULL;
            flag = 1;
        }
    }
}

struct node *choose_next(struct node *current) {
    assert(NULL != current);

    for (size_t i = 0; i < MAX_SIBLINGS; i++) {
        if (NULL == current->siblings[i]) {
            continue;
        }
        if (0 != current->siblings[i]->siblings_left) {
            MY_LOG("\n   About to decrease [%d]",current->id);
            current->siblings_left -= 1;
            MY_LOG("\n   About to decrease {%d}",current->siblings[i]->id);
            current->siblings[i]->siblings_left -= 1;
            return current->siblings[i];
        }
        MY_LOG("\nWhen looking for next for [%d], skipped {%d}", current->id, current->siblings[i]->id);
    }
    return NULL;
} //NULL if no next, decreases siblings_left of both current and next

void make_new(struct node *for_id, struct list *for_connection) {
    assert((NULL != for_id) && (NULL != for_connection));

    struct list *new_element = (struct list *)calloc(1, sizeof(struct list));
    assert(NULL != new_element);
    MY_LOG("\n--- --- ---Making [%d]", for_id->id);
    new_element->id = for_id->id; //->next remains NULL
    for_connection->next = new_element;
}

struct list * find_loop(struct node *start, struct node graph[], struct list *cur_list_pos) {
    assert((NULL != start) && (NULL != graph) && (NULL != cur_list_pos));

    struct node *current = start;
    struct list *current_list = cur_list_pos;
    while(1) {
        //find *some* next element
        struct node *next = choose_next(current);
        MY_LOG("\n---Next is [%d]", next->id);
        MY_LOG("\n---Current is [%d], it has %zu left", current->id, current->siblings_left);
        MY_LOG("\n---Next is [%d], it has %zu left", next->id, next->siblings_left);
        MY_LOG("\n---Deleting arc between next and current");
        delete_arc(next, current);
        if ((NULL == next) || (start->id == next->id)) {
            make_new(next, current_list);
            current_list = current_list->next;
            break;
        }
        //
        make_new(next, current_list);
        MY_LOG("\n-----Added [%d]", next->id);
        //move
        current = next;
        current_list = current_list->next;
        //
    }
    MY_LOG("\n---About to return [%d]", current_list->id);
    return current_list;
}

FLAG continue_with_current(struct list *current, struct node graph[]) {
    assert((NULL != current) && (NULL != graph));

    return (0 != graph[current->id].siblings_left);
} //1 if still some left, 0 if none

void do_it(struct node graph[], struct list *initial) {
    assert((NULL != graph) && (NULL != initial));
    struct list *current_list_node = initial;
    int flag = 0;
    while(1) {
        if (initial == current_list_node) {
            if (1 == flag) {
                break;
            }
        } //came back to start (comparing pointers, not id-s)
        if (0 == continue_with_current(current_list_node, graph)) {
            MY_LOG("\n--Nothing left to do with {%d}, moving on to {%d}", current_list_node->id, current_list_node->next->id);
            current_list_node = current_list_node->next;
            flag = 1;
            continue;
        } //if no more siblings left -> skip, moving on
        struct list *to_remember = current_list_node->next; //to later connect severed list
        MY_LOG("\nAttempting to find loop starting from [%d]", graph[current_list_node->id].id);
        struct list *last = find_loop(&(graph[current_list_node->id]), graph, current_list_node);
        last->next = to_remember; //connecting previously severed list
        print_loop(initial); //-----------------------------------------------------------------------------------------
    }
}

void delete_everything(struct node (*graph)[], struct list *initial) {
    assert((NULL != graph) && (NULL != initial));

    MY_LOG("\nFreeing graph");
    free(graph);
    MY_LOG("\nFreeing list");
    struct list *start = initial->next;
    while(1) {
        struct list *temp = start;
        start = start->next;
        free(temp);
        if (initial == start) {
            break;
        }
    }
    free(initial);
}

int main() {
    PROGRESS_LOG("\nStarting...");
    struct node (*graph)[] = (struct node (*)[])calloc(GRAPH_SIZE, sizeof(struct node));
    assert(NULL != graph);
    PROGRESS_LOG("\nBuilding graph...");
    hard_code_graph(*graph);
    PROGRESS_LOG("\nGraph is built, checking criteria");
    if (-1 == check_criteria(*graph, GRAPH_SIZE)) {
        PROGRESS_LOG("\nCriteria is not met, shutting down");
        return 0;
    } else {
        PROGRESS_LOG("\nCriteria is met, continuing...");
    }
    //initial element
    struct node initial_node = (*graph)[INITIAL];
    struct list *initial_list = (struct list *)calloc(1, sizeof(struct list));
    assert(NULL != initial_list);
    initial_list->id = initial_node.id;
    initial_list->next = initial_list; //loop on self
    //
    //the main dish
    do_it(*graph, initial_list);
    //
    PROGRESS_LOG("\nLoop found, printing...");
    print_loop(initial_list);
    PROGRESS_LOG("\nLoop printed, deleting everything...");
    delete_everything(graph, initial_list);
    PROGRESS_LOG("\nDone");
    return 0;
}
