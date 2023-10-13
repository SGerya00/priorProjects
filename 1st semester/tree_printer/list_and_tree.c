#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "list_and_tree.h"

//list functions
int make_new_node(struct list *true_head, const long long int v_value) {
    //true_head can be NULL when this function is called first time
    struct list *new_node = (struct list*)malloc(sizeof(struct list));
    if (NULL == new_node) {
        fprintf(stderr, "\nUnable to allocate enough memory for a new node in *make_new_node*\n");
        return -1;
    }
    new_node->value = v_value;
    struct list *temporary = true_head->next;   //inserting !!!
    true_head->next = new_node;                 //after !!!!!!!
    new_node->next = temporary;                 //true_head !!!
    return 0;
} //allocates memory, please free in a loop

void delete_next_node(struct list *current) {
    assert(NULL != current);
    struct list *temporary = current->next; //remember
    current->next = current->next->next; //exclude from list
    free(temporary); //remove
} //good

void clean_sorted_list(const struct list *true_head, size_t counter, size_t *new_counter) {
    assert(NULL != true_head);
    struct list *current = true_head->next;
    for (size_t i = 0; i < (counter - 1); i++) {
        if (current->value == current->next->value) {
            delete_next_node(current);
        }
        else {
            *new_counter += 1;
            current = current->next;
        }
    }
} //returns length of list without repetitions while also deleting repeating elements

void delete_entire_list(struct list *true_head) {
    assert(NULL != true_head);
    struct list *temp_head = true_head;
    if (NULL != temp_head->next->next) {
        delete_entire_list(temp_head->next);
    }
    delete_next_node(temp_head);
} //after recursion we are left with true_head, please free separately
//tree functions
struct tree * make_a_leaf(const long long int v_value) {
    //destination can be NULL the first time
    struct tree *new_leaf = (struct tree*)malloc(sizeof(struct tree));
    if (NULL == new_leaf) {
        fprintf(stderr, "\nUnable to allocate enough memory for a new leaf\n");
        return NULL;
    }
    new_leaf->value = v_value;
    new_leaf->bottom_left = NULL;
    new_leaf->bottom_right = NULL;
    return new_leaf;
} //returns pointer to allocated memory (please free in a loop)

void print_tree(const struct tree *big_root) {
    if (NULL == big_root) {
        return;
    }
    printf("(");
    print_tree(big_root->bottom_left);
    printf("%lli", big_root->value);
    print_tree(big_root->bottom_right);
    printf(")");
} //good

void delete_leaf(struct tree *big_root) {
    assert(NULL != big_root);
    free(big_root);
} //good

void delete_entire_tree(struct tree *big_root) {
    assert(NULL != big_root);
    if (NULL != big_root->bottom_left) {
        if ((NULL != big_root->bottom_left->bottom_left) || (NULL != big_root->bottom_left->bottom_right)) {
            delete_entire_tree(big_root->bottom_left);
        }
        else {
            delete_leaf(big_root->bottom_left);
            big_root->bottom_left = NULL;
        }
    }
    if (NULL != big_root->bottom_right) {
        if ((NULL != big_root->bottom_right->bottom_left) || (NULL != big_root->bottom_right->bottom_right)) {
            delete_entire_tree(big_root->bottom_right);
        }
        else {
            delete_leaf(big_root->bottom_right);
            big_root->bottom_right = NULL;
        }
    }
    delete_leaf(big_root);
} //after recursion we are left with the root element we gave to function, please free separately

struct tree *make_tree(size_t size_of_sub, struct list *false_head) {
    assert(NULL != false_head);
    struct tree *destination = NULL; //important
    if (1 == size_of_sub) {
        destination = make_a_leaf(false_head->value);
        if (NULL == destination) {
            return NULL;
        } //memory was not allocated
        else {
            return destination;
        }
    }
    struct list *temp_head = false_head;
    // a loop to find middle_element
    size_t middle_element = (size_of_sub - 1) / 2;
    for (size_t i = 0; i < middle_element; i++) {
        temp_head = temp_head->next;
    } // loop over, now temp_head is on list's middle element
    destination = make_a_leaf(temp_head->value);
    if (NULL == destination) {
        return NULL;
    } //not it's either -1 or destination points to memory allocated to a new leaf
    //for left sub tree
    size_t left_size = (size_of_sub - 1) / 2;
    if (0 < left_size) {
        struct list *left_start = false_head;
        destination->bottom_left = make_tree(left_size, left_start);
        if (NULL == destination->bottom_left) {
            printf("\nTree is incomplete, not enough memory\n");
            return destination; //get out of recursion
        }
    } // recursively create more sub trees (or get -1 and carry it out of the recursion)
    //for right sub tree
    size_t right_size = (size_of_sub - 1) - ((size_of_sub - 1) / 2);
    if (0 < right_size) {
        struct list *right_start = temp_head->next;
        destination->bottom_right = make_tree(right_size, right_start);
        if (NULL == destination->bottom_right) {
            printf("\nTree is incomplete, not enough memory\n");
            return destination; //get out of recursion
        }
    } //recursively create more sub trees (or get -1 and carry it out of the recursion)
    return destination;
} //gets list, makes tree, doesn't delete list, please delete separately
