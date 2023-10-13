#include <stdio.h>
#include <assert.h>
#include "list_and_tree.h"

void bubble_sort_for_struct_list(size_t number_of_nodes, struct list *start_of_list) {
    assert(NULL != start_of_list);
    struct list *i_element = start_of_list;
    for (size_t i = 0; i < number_of_nodes; i++) {
        i_element = i_element->next;
        struct list *j_element = start_of_list;
        for (size_t j = 0; j < number_of_nodes; j++) {
            j_element = j_element->next;
            if (i_element->value < j_element->value) {
                //swap
                long long int temp = i_element->value;
                i_element->value = j_element->value;
                j_element->value = temp;
                //
            } //compare
        }
    } //sorting complete
} //start_of_list should be true_head

int read_file_make_list(FILE *input_file, struct list *true_head, size_t *counter) {
    assert((NULL != input_file) && (NULL != true_head) && (NULL != counter));
    long long int future_value = 0;
    while(1 == fscanf(input_file, "%lli ", &future_value)) {
        *counter += 1;
        if (-1 == make_new_node(true_head, future_value)) {
            fprintf(stderr, "\nMemory allocation for a list is not completed, freeing previously allocated memory\n");
            fclose(input_file);
            return -1;
        }
    }
    int tester = fgetc(input_file);
    if (EOF != tester) {
        fprintf(stderr, "\nEncountered an unexpected symbol \"%c\" in input_file", tester);
        fprintf(stderr, "\nWorking with everything that abides the rules before that symbol\n\n");
    }
    fclose(input_file); //because we received pointer to open file
    return 0;
} //return amount of read elements, while also making new nodes in list (!!malloc!!, please free in a loop)

FILE *check_argv(const int argc, char *argv[]) {
    assert(NULL != argv);
    if (2 != argc) {
        fprintf(stderr, "\nUnexpected amount of arguments, expected 1\n");
        return NULL;
    }
    FILE *f = fopen(argv[1], "r");
    if (NULL == f) {
        fprintf(stderr, "\nUnable to open file %s\n", argv[1]);
        return NULL;
    }
    else {
        return f;
    }
} //returns pointer to open file, please close later

int main(int argc, char *argv[]) {
    FILE *input_file = check_argv(argc, argv);
    if (NULL == input_file) {
        return -1;
    }
    struct list true_head = {0, NULL};
    size_t counter = 0;
    if (-1 == read_file_make_list(input_file, &true_head, &counter)) { //input file is closed in function
        delete_entire_list(&true_head);
        return -1;
    } //smth went wrong
    else if (0 == counter) {
        fprintf(stderr, "\nFile doesn't start with a number\n");
        return 0;
    }
    bubble_sort_for_struct_list(counter, &true_head);
    size_t new_counter = 1;
    clean_sorted_list(&true_head, counter, &new_counter);
    struct tree *pointer_to_tree = NULL;
    pointer_to_tree = make_tree(new_counter, true_head.next);
    if (NULL == pointer_to_tree) {
        fprintf(stderr, "\nUnable to allocate memory for even a single leaf\n");
        delete_entire_list(&true_head); //deletion
        return -1;
    } //only if not a single leaf was created
    delete_entire_list(&true_head); //don't need list anymore
    print_tree(pointer_to_tree);
    delete_entire_tree(pointer_to_tree);
    printf("\n\nDone\n");
    return 0;
}
