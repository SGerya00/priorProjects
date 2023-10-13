#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define BOARD_SIZE_ROW 32
#define BOARD_SIZE_COLUMN 32
#define PATH_LEN (BOARD_SIZE_ROW * BOARD_SIZE_COLUMN)
#define KNIGHT_MAX_MOVES 8
#define DEFAULT 0

struct position {
    long long int row;
    long long int column;
};

struct possible_move {
    int n_o_exodus;
    struct position poss_pos;
};

void print_board(long long int (*board)[BOARD_SIZE_ROW][BOARD_SIZE_COLUMN]) {
    assert(NULL != board);

    for (long long int i = 0; i < BOARD_SIZE_ROW; i++) {
        printf("\n");
        for (long long int j = 0; j < BOARD_SIZE_COLUMN; j++) {
            printf("|%4lli|", (*board)[i][j]);
        }
    }
}

int out_of_bounds_check(const struct position current_pos, long long int delta_x, long long int delta_y) {
    if ((0 > (current_pos.row + delta_x)) ||
        ((BOARD_SIZE_ROW - 1) < (current_pos.row + delta_x)) ||
        (0 > (current_pos.column + delta_y)) ||
        ((BOARD_SIZE_COLUMN - 1) < (current_pos.column + delta_y)) ) {
        return 0; //out of bounds
    }
    else {
        return 1;
    }
}

int check_move(const struct position current_pos, long long int delta_x,
               long long int delta_y, const long long int (*board)[BOARD_SIZE_ROW][BOARD_SIZE_COLUMN]) {
    assert(NULL != board);

    if (0 == out_of_bounds_check(current_pos, delta_x, delta_y)) {
        return 0; //out of bounds
    }
    else {
        if (DEFAULT == (*board)[current_pos.row + delta_x][current_pos.column + delta_y]) {
            return 1; //empty, can move
        }
        else {
            return 0; //not empty, cannot move
        }
    }
} //returns 1 if move is possible, 0 if not

int count_exodus(const struct position current_pos,
                 const struct position (*delta_arr)[KNIGHT_MAX_MOVES],
                 const long long int (*board)[BOARD_SIZE_ROW][BOARD_SIZE_COLUMN]) {
    assert((NULL != board) && (NULL != delta_arr));

    int exodus_counter = 0;
    for (int i = 0; i < KNIGHT_MAX_MOVES; i++) {
        exodus_counter += check_move(current_pos, (*delta_arr)[i].row, (*delta_arr)[i].column, board);
    }
    return exodus_counter;
}

void asc_sort_arr_of_pos_moves(struct possible_move (*poss_move_arr)[KNIGHT_MAX_MOVES]) {
    assert(NULL != poss_move_arr);

    for (int i = 0; i < KNIGHT_MAX_MOVES; i++) {
        if (0 == (*poss_move_arr)[i].n_o_exodus) {
            continue;
        }
        for (int j = 0; j < KNIGHT_MAX_MOVES; j++) {
            if ((*poss_move_arr)[i].n_o_exodus < (*poss_move_arr)[j].n_o_exodus) {
                struct possible_move temp = {0, {0, 0}};

                temp = (*poss_move_arr)[i];
                (*poss_move_arr)[i] = (*poss_move_arr)[j];
                (*poss_move_arr)[j] = temp;
            } //switch places
        }
    }
}

void find_all_poss_moves(const struct position (*delta_arr)[KNIGHT_MAX_MOVES],
                         const struct position current_pos,
                         struct possible_move (*poss_move_arr)[KNIGHT_MAX_MOVES],
                         const long long int (*board)[BOARD_SIZE_ROW][BOARD_SIZE_COLUMN]) {
    assert((NULL != delta_arr) && (NULL != poss_move_arr) && (NULL != board));

    for (int i = 0; i < KNIGHT_MAX_MOVES; i++) {
        if (0 == out_of_bounds_check(current_pos, (*delta_arr)[i].row, (*delta_arr)[i].column)) {
            continue;
        } //out of bounds, check next one
        if (DEFAULT != (*board)[current_pos.row + (*delta_arr)[i].row][current_pos.column + (*delta_arr)[i].column]) {
            continue;
        } //already taken, check next one
        struct position new_pos = {current_pos.row + (*delta_arr)[i].row,
                                   current_pos.column + (*delta_arr)[i].column};
        (*poss_move_arr)[i].n_o_exodus = count_exodus(new_pos, delta_arr, board);
        if (0 != (*poss_move_arr)[i].n_o_exodus) {
            (*poss_move_arr)[i].poss_pos.row = new_pos.row;
            (*poss_move_arr)[i].poss_pos.column = new_pos.column;
        } //if possible to move further from this one
    }
}

int last_move(const struct position current_pos,
              const struct position (*delta_arr)[KNIGHT_MAX_MOVES],
              long long int (*board)[BOARD_SIZE_ROW][BOARD_SIZE_COLUMN]) {
    assert((NULL != delta_arr) && (NULL != board));

    for (int i = 0; i < KNIGHT_MAX_MOVES; i++) {
        if (0 == out_of_bounds_check(current_pos, (*delta_arr)[i].row, (*delta_arr)[i].column)) {
            continue;
        } //out of bounds
        else {
            if (DEFAULT==(*board)[current_pos.row + (*delta_arr)[i].row][current_pos.column + (*delta_arr)[i].column]) {
                (*board)[current_pos.row + (*delta_arr)[i].row][current_pos.column + (*delta_arr)[i].column] = PATH_LEN;
                return 1; //last move done
            }
            else {
                continue;
            }
        }
    }
    return 0; //unable to make last move
}

int make_move_rec(const struct position current_pos,
                  const long long int n_o_current_move,
                  const struct position (*delta_arr)[KNIGHT_MAX_MOVES],
                  long long int (*board)[BOARD_SIZE_ROW][BOARD_SIZE_COLUMN]) {
    assert((NULL != board) && (NULL != delta_arr));

    struct possible_move (*poss_move_arr)[KNIGHT_MAX_MOVES] =
            (struct possible_move (*)[KNIGHT_MAX_MOVES])calloc(KNIGHT_MAX_MOVES, sizeof(struct possible_move));
    if (NULL == poss_move_arr) {
        fprintf(stderr, "\nUnable to allocate enough memory to finish\n");
        return 0;
    }
    find_all_poss_moves(delta_arr, current_pos, poss_move_arr, board); //fills poss_move_arr with pairs of new coord-s
    asc_sort_arr_of_pos_moves(poss_move_arr);
    if (PATH_LEN > n_o_current_move) {
        for (int i = 0; i < KNIGHT_MAX_MOVES; i++) {
            if (0 == (*poss_move_arr)[i].n_o_exodus) {
                continue;
            } //skip, move is bad
            (*board)[(*poss_move_arr)[i].poss_pos.row][(*poss_move_arr)[i].poss_pos.column] = n_o_current_move;
            int flag = make_move_rec((*poss_move_arr)[i].poss_pos, n_o_current_move +1, delta_arr, board);

            if (1 == flag) {
                free(poss_move_arr);
                return 1; //exiting recursion
            }
            else {
                (*board)[(*poss_move_arr)[i].poss_pos.row][(*poss_move_arr)[i].poss_pos.column] = DEFAULT;
            }
        }
        free(poss_move_arr);
        return 0;
    } //not last move
    else {
        int flag = last_move(current_pos, delta_arr, board);
        if (1 == flag) {
            free(poss_move_arr);
            return 1; //last move made
        }
        else {
            free(poss_move_arr);
            return 0;
        }
    } //last move
}

struct position get_coordinates() {
    struct position start = {0,0}; //init
    while(1) {
        if (2 != scanf("%lli %lli", &(start.row), &(start.column))) {
            printf("\nBad input, please try again\n");
        }
        else {
            if (0 == out_of_bounds_check(start, 0, 0) ) {
                printf("\nBad input, please try again\n");
                continue;
            }
            printf("\ngood input, calculating\n");
            break;
        }
    }
    return start;
}

int main() {
    struct position delta_arr[KNIGHT_MAX_MOVES] = {
            {-2,-1}, {-1,-2}, {1,-2}, {2,-1},
            {2,1}, {1,2}, {-1,2}, {-2,1}
    };
    long long int (*board)[BOARD_SIZE_ROW][BOARD_SIZE_COLUMN] =
            (long long int (*)[BOARD_SIZE_ROW][BOARD_SIZE_COLUMN])calloc(PATH_LEN, sizeof(long long int));
    if (NULL == board) {
        fprintf(stderr, "\nUnable to allocate enough memory for board\n");
        return 0;
    }
    struct position initial_pos = {0,0};
    initial_pos = get_coordinates();
    (*board)[initial_pos.row][initial_pos.column] = 1;
    int flag = make_move_rec(initial_pos, 2, &delta_arr, board); //start searching where to put knight on 2nd move
    if (0 == flag) {
        printf("\nCannot find Hamilton path, sry\n");
        free(board);
        return 0;
    }
    print_board(board);
    free(board);
    return 0;
}