#include "sorting_numbers_qsort_ish.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#define MAXIMUM 10 //max number of numbers to work with

struct timber{
    unsigned long int file_index; //to use with ultoa
    struct timber * next;
    int already_read;
    int capacity;
}; //a list containing numbers of created files

int make_new_node(struct timber * head, const int amount, const unsigned long int x) {
    assert(NULL != head);
    struct timber * new_node = (struct timber *)malloc(sizeof(* new_node));
    if (NULL == new_node) {
        fprintf(stderr, "\nUnable to allocate memory for a new node in *make_new_node*\n");
        return -1;
    }
    struct timber * temporary = head->next; //inserting a new node after the head and before the body of a list
    head->next = new_node;
    new_node->file_index = x;
    new_node->already_read = 0;
    new_node->capacity = amount;
    new_node->next = temporary;
    return 0;
} //after this, memory was allocated

void delete_current_node(struct timber *head, struct timber * current) {
    assert((NULL != head) && (NULL != current));
    struct timber *temp_head = head;
    while (NULL != temp_head) {
        if (temp_head->next->file_index == current->file_index) {
            break;
        }
        else {
            temp_head = temp_head->next;
        }
    } //after the loop temp_head is previous to the element we want to delete
    temp_head->next = current->next; //exclude current node from timber
    free(current); //delete current node
} //finds the one before current and deletes the node after

void terminator_of_nodes(struct timber *false_head) {
    assert(NULL != false_head);
    if (NULL != false_head->next) {
        terminator_of_nodes(false_head->next);
    }
    free(false_head);
} //recursively frees memory

size_t number_of_digits(const unsigned long int x) {
    if (0 == ((x - (x % 10)) / 10)) { return 1; }
    else { return (1 + number_of_digits((x - (x % 10)) / 10)); }
} //number of digits in a given number

void delete_used_file(const char * filename) {
    assert(NULL != filename);
    remove(filename);
}

size_t find_last_dot_pos(const char * filename) {
    assert(NULL != filename);
    size_t last_dot_pos = 0;
    size_t i = 0;
    while ('\0' != filename[i]) {
        if ('.' == filename[i]) {
            last_dot_pos = i;
        }
        i += 1;
    }
    return last_dot_pos;
}

char * make_output_filename(const char *orig_filename) {
    assert(NULL != orig_filename);
    size_t last_dot_pos = find_last_dot_pos(orig_filename);
    char *suffix = ".output";
    char *output_filename = (char*)calloc(strlen(orig_filename) + strlen(suffix) + 1, sizeof(char));
    if (NULL == output_filename) {
        return NULL;
    }
    memcpy(output_filename, orig_filename, last_dot_pos);
    memcpy(output_filename + last_dot_pos, suffix, strlen(suffix));
    memcpy(output_filename + last_dot_pos + strlen(suffix), orig_filename + last_dot_pos, strlen(orig_filename) - last_dot_pos);
    output_filename[strlen(suffix) + strlen(orig_filename)] = '\0';
    return output_filename;
} //returns allocated memory

int make_output_file(const char *output_filename) {
    assert(NULL != output_filename);
    FILE *f = fopen(output_filename, "w");
    if (NULL == f) {
        fprintf(stderr, "\nUnable to create output file %s in *make_output*\n", output_filename);
        return -1;
    }
    fclose(f);
    return 0;
} //open and close

char * make_new_filename(const char * filename, const unsigned long int file_index) {
    assert(NULL != filename);
    size_t n_o_digits = number_of_digits(file_index); //to know how lengthy should the string be
    char *order_str = (char*)calloc(n_o_digits + 1, sizeof(*order_str)); //get the string ready
    if (NULL == order_str) {
        fprintf(stderr, "\nUnable to allocate memory for order_str in *make_new_filename*\n");
        return NULL;
    }
    //_ultoa(file_index, order_str, 10);
    sprintf(order_str, "%lu", file_index); //now order_str is filled with symbols
    order_str[n_o_digits] = '\0'; //last symbol is eos
    char *new_filename = (char*)calloc(strlen(filename) + n_o_digits + 2, sizeof(*new_filename)); //a new filename (filename.order.txt) (2 is for '.' and '\0')
    if (NULL == new_filename) {
        fprintf(stderr, "\nUnable to allocate memory for new_filename in *make_new_filename*\n");
        free(order_str);
        return NULL;
    }
    strcpy(new_filename, filename);
    new_filename[strlen(filename)] = '.';
    for (int i = 0; i <= n_o_digits; i++) {
        new_filename[strlen(filename) + i + 1] = order_str[i];
    }
    free(order_str);
    return new_filename;
}// !!returns allocated memory pls free when necessary!!

FILE * create_new_file(const unsigned long int order, const char * filename) {
    assert(NULL != filename);
    char * new_filename = make_new_filename(filename, order);
    if (NULL == new_filename) {
        fprintf(stderr, "\n...in *create_new_file*\n");
        return NULL;
    }
    FILE *f = fopen(new_filename, "w"); //now we create a file
    if (NULL == f) {
        fprintf(stderr, "\nUnable to open freshly created file in *create_new_file*\n");
        return NULL;
    } //now the file exists
    free(new_filename); //!!because it was allocated in make_new_filename!!
    return f; //all went well
}

int get_put_number(int key, int (*compare)(const void *, const void *),
                   const char * filename, FILE *open_output, struct timber * head) {
    assert((NULL != compare) && (NULL != filename) && (NULL != head) && (NULL != open_output));
    struct timber * temp_head = head->next;
    struct timber *est_node = NULL;
    long long int est_number = 0;
    char * file_w_est = (char*)malloc(1); //so we can free it in a loop before allocating
    if (1 == key) {
        est_number = LLONG_MAX;
    }
    else {
        est_number = LLONG_MIN;
    }
    while (NULL != temp_head) {
        char * new_filename = make_new_filename(filename, temp_head->file_index); //!!memory was allocated!!
        if (NULL == new_filename) {
            fprintf(stderr, "\n...in *get_put_number*\n");
            free(file_w_est);
            return -1;
        }
        FILE *f = fopen(new_filename, "r");
        if (NULL == f) {
            fprintf(stderr, "\nUnable to open file %s in *get_put_number*\n", new_filename);
            free(file_w_est);
            free(new_filename);
            return -1;
        }
        long long int potentially_different;
        for (int i = 0; i < temp_head->already_read; i++) {
            fscanf(f, "%lli", &potentially_different);
        } //to skip numbers that were already read previously
        fscanf(f, "%lli", &potentially_different);
        if (1 == (compare(&potentially_different, &est_number))) {
            est_number = potentially_different;
            free(file_w_est);
            file_w_est = make_new_filename(filename, temp_head->file_index); //!!memory was allocated!!
            est_node = temp_head;
        }
        temp_head = temp_head->next;
        fclose(f);
        free(new_filename); //because it was allocated in make_new_filename at the beginning of the loop
    } //after the loop we have the smallest/biggest element and respectful position in struct timber
    fprintf(open_output, "%lli ", est_number); //put number in output +
    if ((est_node->already_read + 1) == (est_node->capacity)) {
        delete_used_file(file_w_est);
        delete_current_node(head, est_node);
    }
    else {
        est_node->already_read += 1;
    }
    free(file_w_est);
    return 0;
} //head should be &true_head (uphead)

void put_sorted_into_file(const long long int * elem_list, size_t amount, FILE * temp_file) {
    assert((NULL != temp_file) && (NULL != elem_list));
    for (size_t i = 0; i < amount; i++) {
        fprintf(temp_file, "%lli ", elem_list[i]);
    }
    fclose(temp_file); //all went well
}

void swap_ints_in_array(void * x, void * y) {
    assert((NULL != x) && (NULL != y));
    long long int t = *(long long int*)x;
    *(long long int*)x = *(long long int*)y;
    *(long long int*)y = t;
} //!!!!!!!!!!!!!!!

int compare_ints_i(const void * x, const void * y) {
    assert((NULL != x) && (NULL != y));
    long long int reg_x = *(long long int*)x;
    long long int reg_y = *(long long int*)y;
    if (reg_x == reg_y) {
        return 0;
    }
    else if (reg_x < reg_y) {
        return 1;
    }
    else {
        return -1;
    }
} // returns 1 if x < y

int compare_ints_d(const void * x, const void * y) {
    assert((NULL != x) && (NULL != y));
    long long int reg_x = *(long long int*)x;
    long long int reg_y = *(long long int*)y;
    if (reg_x == reg_y) {
        return 0;
    }
    else if (reg_x > reg_y) {
        return 1;
    }
    else {
        return -1;
    }
} // returns 1 if x > y

int check_argv(int *key, int argc, char * argv[]) {
    assert((NULL != argv) && (NULL != key));
    if (argc < 3 || argc > 3) {
        fprintf(stderr, "\nUnexpected amount of arguments, expected 3\n");
        return -1;
    } //check amount of arguments
    if ('d' == argv[2][1]) {
        *key = -1;
        return 0;
    }
    else if ('i' == argv[2][1]) {
        *key = 1;
        return 0;
    }
    else {
        fprintf(stderr, "\nUnexpected key, expected \"-i\" or \"-d\"\n");
        return -1;
    } //argv[2] = key to sorting
}

int first_stage_sub(const char *input_filename, int amount,
                    struct timber *uphead, int (*compare_ints)(const void *, const void *),
                    unsigned long int *file_index, long long int array_of_lls[MAXIMUM]) {
    assert((NULL != input_filename) && (NULL != file_index) && (NULL != uphead) && (NULL != array_of_lls));
    *file_index += 1;
    if (-1 == make_new_node(uphead, amount, *file_index)) { //!! memory allocated for new node, free in loop !!
        fprintf(stderr, "\n...in *first_stage_sub*\n");
        return -1; //if didn't allocate memory
    }
    quick_sort(array_of_lls, amount, sizeof(long long int), compare_ints, swap_ints_in_array); //should be good
    FILE * temporary = create_new_file(*file_index, input_filename); //create new file
    if (NULL == temporary) {
        fprintf(stderr, "\n...in *first_stage*\n");
        return -1;
    }
    put_sorted_into_file(array_of_lls, amount, temporary); //put elem_list into new file
    return 0;
}

int first_stage(FILE *input_file, struct timber *uphead,
                int (*compare_ints)(const void *, const void *), const char *input_filename) {
    assert((NULL != input_file) && (NULL != uphead) && (NULL != input_filename));
    long long int current_number = 0;
    int amount = 0;
    long long int array_of_lls[MAXIMUM];
    unsigned long int file_index = 0;
    while (1 == fscanf(input_file, "%lli ", &current_number)) {
        amount += 1;
        array_of_lls[amount - 1] = current_number;
        if (MAXIMUM == amount) {
            if (-1 == first_stage_sub(input_filename, amount, uphead, compare_ints, &file_index, array_of_lls)) {
                terminator_of_nodes(uphead->next); //frees all prior created nodes
                fprintf(stderr, "\n...in *first_stage*\n");
                return -1;
            }
            amount = 0;
        }
    } //loop reading file
    int tester = fgetc(input_file);
    if (EOF != tester) {
        fprintf(stderr, "\nEncountered an unexpected symbol '%c' in file\n", tester);
        fprintf(stderr, "Sorting everything that abides the rules before that symbol\n");
    }
    if (0 != amount) {
        if (-1 == first_stage_sub(input_filename, amount, uphead, compare_ints, &file_index, array_of_lls)) {
            terminator_of_nodes(uphead->next); //frees all prior created nodes
            fprintf(stderr, "\n...in *first_stage*\n");
            return -1;
        }
    }
    fclose(input_file); //loop reading file done!!!
    return 0;
} //reads numbers from file, puts them in temporary files, each has MAXIMUM elements except for, maybe, last one

int main(int argc, char * argv[]) {
    int key = 0;
    if (-1 == check_argv(&key, argc, argv)) {
        return -1; //smth went wrong
    }
    void * appropriate_func = NULL;
    if (1 == key) {
        appropriate_func = &(compare_ints_i); //increase
    }
    else {
        appropriate_func = &(compare_ints_d); //decrease
    }
    FILE *input_file = fopen(argv[1], "r");
    if (NULL == input_file) {
        fprintf(stderr, "\nUnable to open file %s in *main*\n", argv[1]);
        return -1;
    }
    struct timber true_head = {0, NULL, 0, 0};
    struct timber *uphead = &true_head;
    if (-1 == first_stage(input_file, uphead, appropriate_func, argv[1])) {
        fprintf(stderr, "\n...in *main*\nShutting down\n");
        fclose(input_file); //input file is closed
        return -1;
    } //!!input_file is fclosed in first_stage
    char * output_filename = make_output_filename(argv[1]); //allocated memory
    if (NULL == output_filename) {
        fprintf(stderr, "\nUnable to allocate enough memory to make a name for output file\n");
        terminator_of_nodes(uphead->next);
        return -1;
    }
    if (-1 == make_output_file(output_filename)) {
        fprintf(stderr, "\nUnable to finish, shutting down\n");
        free(output_filename);
        terminator_of_nodes(uphead->next);
        return -1;
    }
    FILE *open_output = fopen(output_filename, "w");
    if (NULL == open_output) {
        fprintf(stderr, "\nUnable to access output file\n");
        free(output_filename);
        terminator_of_nodes(uphead->next);
        return -1;
    }
    while (NULL != uphead->next) {
        if (-1 == get_put_number(key, appropriate_func, argv[1], open_output, uphead)) {
            fprintf(stderr, "\n...in *main*\nShutting down");
            free(output_filename);
            fclose(open_output);
            terminator_of_nodes(uphead->next);
            return -1;
        }
    } //loop to fill the output file!!!
    printf("\nFinished sorting, check out the %s file\n", output_filename);
    free(output_filename); //memory freed
    fclose(open_output);
    return 0;
}
