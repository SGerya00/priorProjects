#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define SIZE 9
#define PATH_XSTS 1
#define DNT_NEED_NODE 1
#define NEED_NODE 0

void initialise_matrix(size_t (*matrix)[SIZE][SIZE]) {
    assert(NULL != matrix);

    (*matrix)[0][1] = PATH_XSTS;
    (*matrix)[0][5] = PATH_XSTS;
    (*matrix)[1][4] = PATH_XSTS;
    (*matrix)[2][0] = PATH_XSTS;
    (*matrix)[2][5] = PATH_XSTS;
    (*matrix)[4][3] = PATH_XSTS;
    (*matrix)[5][3] = PATH_XSTS;
    (*matrix)[3][2] = PATH_XSTS; //this one
    (*matrix)[6][0] = PATH_XSTS;
    (*matrix)[7][0] = PATH_XSTS;
    (*matrix)[8][2] = PATH_XSTS;
}

void print_seq(const size_t for_print_arr[SIZE]) {
    assert(NULL != for_print_arr);

    for (size_t i = 0; i < SIZE; i++) {
        printf("%zu ", for_print_arr[i] + 1);
    }
}

int do_topol_sort_flag(size_t (*matrix)[SIZE][SIZE], size_t (*for_print_arr)[SIZE]) {
    assert((NULL != matrix) && (NULL != for_print_arr));

    int column_flag = 0;
    int column_arr[SIZE] = {NEED_NODE};
    for (size_t counter = 0; counter < SIZE; counter++) {
        int apr_column_is_found_flag = 0;
        //searches for 1st empty column
        for (size_t j = 0; j < SIZE; j++) { //j - column
            column_flag = 0; //reset
            if (DNT_NEED_NODE == column_arr[j]) {
                continue;
            }
            for (size_t i = 0; i < SIZE; i++) { //i - row
                if (DNT_NEED_NODE == column_arr[i]) {
                    continue;
                }
                if (PATH_XSTS == (*matrix)[i][j]) {
                    column_flag = 1;
                    break;
                }
                else {
                    column_flag = 0;
                    continue;
                }
            }
            if (0 == column_flag) { //first 0 column is found
                apr_column_is_found_flag = 1;
                (*for_print_arr)[counter] = j;
                column_arr[j] = DNT_NEED_NODE; //dismiss
                break;
            }
        } //search stops
        if (0 == apr_column_is_found_flag) {
            return 1;
        }
    }
    return 0; //all good, no loops
}

int main() {
    size_t (*matrix)[SIZE][SIZE] = (size_t (*)[SIZE][SIZE])calloc(SIZE * SIZE, sizeof(size_t));
    if (NULL == matrix) {
        printf("bad");
        return -1;
    }
    initialise_matrix(matrix);
    size_t print_arr[SIZE] = {0};
    int last_flag = do_topol_sort_flag(matrix, &print_arr);
    if (1 == last_flag) {
        printf("\nThere is a loop in a graph.\n");
    }
    else {
        print_seq(print_arr);
    }
    free(matrix);
    return 0;
}
