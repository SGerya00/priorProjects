#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALIVE '1'
#define DEAD '0'
#define ALIVE_FP "O"
#define DEAD_FP "-"
#define IFC "\nIncorrect file composition\n"

FILE *open_file_for_r(const char * filename) {
    FILE *f = fopen(filename, "r");
    if (NULL == f) {
        fprintf(stderr, "\nUnable to open file %s in *open_file_for_r*\n", filename);
    }
    return f;
}

size_t rules_line_check_sub(const char *example, const char *c, size_t big_i) {
    size_t small_i = 0;
    while (small_i < strlen(example)) {
        if (example[small_i] != c[big_i]) {
            printf(IFC"expected '%s' in rules line\n", example);
            return 0;
        }
        small_i += 1;
        big_i += 1;
    }
    return big_i;
} //to use in rules_line_check

int rules_line_check(const char *c) {
    size_t big_i = 0; //to navigate the line
    size_t small_i = 0; //to navigate elsewhere
    big_i = rules_line_check_sub("x = ", c, big_i);
    if (0 == big_i) {
        return -1;
    } //first check
    while ((c[big_i] >= '0') && (c[big_i] <= '9')) {
        big_i += 1;
    } //skipping the numbers and moving big_i
    big_i = rules_line_check_sub(", y = ", c, big_i);
    if (0 == big_i) {
        return -1;
    } //second check
    while ((c[big_i] >= '0') && (c[big_i] <= '9')) {
        big_i += 1;
    } //skipping the numbers and moving big_i
    big_i = rules_line_check_sub(", rule = B", c, big_i);
    if (0 == big_i) {
        return -1;
    } //third check
    size_t rules_digits = 0;
    while ((c[big_i] >= '0') && (c[big_i] <= '8')) { //'9' (DEC 57) cannot be a rule
        rules_digits += 1;
        big_i += 1;
    } //skipping the numbers and moving big_i
    if (rules_digits > 9) { //max of 9 digits for a B rule
        printf(IFC"Incorrect rules for birth (too much)\n");
        return -1;
    }
    rules_digits = 0;
    char *fourth_check = "/S";
    small_i = 0;
    while (small_i < 2) {
        if (fourth_check[small_i] != c[big_i]) {
            printf(IFC"expected '/S'\n");
            return -1;
        }
        small_i += 1;
        big_i += 1;
    } //after the loop big_i was increased by 2
    while ((c[big_i] >= '0') && (c[big_i] <= '8')) { //'9' (DEC 57) cannot be a rule
        rules_digits += 1;
        big_i += 1;
    } //skipping the numbers and moving big_i
    if (rules_digits > 9) { //max of 9 digits for an S rule
        printf(IFC"Incorrect rules for survival (too much)\n");
        return -1;
    }
    rules_digits = 0;
    //last check (to see that no symbols follow the rules)
    if ('\n' != c[big_i]) {
        printf(IFC"unexpected symbols after survival rules\n");
        return -1;
    } //last character should be '\n'
    return 0;
} //to use in check_file

int check_file(FILE *filename) {
    if (NULL == filename) {
        fprintf(stderr, "\nFile pointer is NULL in *check_file*\n");
        return -1;
    }
    FILE *workspace = filename;
    size_t no_com_lc = 0; //no comment line counter
    int rules_xst = 0; //if rules exist (flag)
    int rle_xsts = 0; //if rle lines exist (flag)
    size_t no_of_lines = 0;
    while (1) {
        char c[72] = {0}; //from 0 to 69, then \n then \0
        char *string_status = fgets(c, sizeof(c), workspace);
        //if the file is empty and there were no lines
        if ((NULL == string_status) && (0 == no_of_lines)) {
            printf("\nEmpty file, please change\n");
            fclose(workspace);
            return -1;
        }
        if (NULL == string_status) {
            break;
        } //no more lines in file
        if (('#' == c[0]) && (0 == rules_xst) && (0 == rle_xsts)) {
            no_of_lines += 1;
        } //a line is a comment
        else {          //found a no-comment line, need to check length
            size_t old_temp_i = 0;
            int eol_xsts = 0; //flag
            while ('\0' != c[old_temp_i]) {
                if (('\n' == c[old_temp_i]) || ('!' == c[old_temp_i])) {
                    eol_xsts = 1; //found the EOL (or '!' which according to the rules is also EOL)
                }
                old_temp_i += 1;
            }
            if (0 == eol_xsts) { //did not find an EOL
                printf(IFC"Line is too long (or no '!' at the end of RLE)\n");
                fclose(workspace);
                return -1;
            }
            if (0 == rules_xst) { //to check if we are working with the rules line
                rules_xst = 1; //considering current line a rules line
                no_of_lines += 1;
                no_com_lc += 1;
                if (-1 == rules_line_check(c)) {
                    fclose(workspace);
                    return -1;
                }
            }
            else { //working with RLE
                no_of_lines += 1;
                no_com_lc += 1;
                rle_xsts = 1;
                size_t temp_i = 0;
                while (('\n' != c[temp_i]) && ('\0' != c[temp_i])) {
                    if ((111 != c[temp_i]) && (98 != c[temp_i]) \
                    && (36 != c[temp_i]) && (33 != c[temp_i])) { //character is not 'o','b','$' or '!'
                        if ((c[temp_i] < 48) || (c[temp_i] > 57)) {
                            printf("\nUnexpected symbol in RLE line: %c\n", c[temp_i]);
                            fclose(workspace);
                            return -1;
                        } //not a number
                    }
                    if ('!' == c[temp_i]) {
                        return 0; //true exit
                    } //only happens if file is correctly composed
                    temp_i += 1;
                }
                //if '\n' == c[temp_i] -> don't care
                if ('\0' == c[temp_i]) {
                    printf(IFC"No '!' found\n");
                    fclose(workspace);
                    return -1;
                }
            }
        }
    }
    if (0 == no_com_lc) {
        printf(IFC"Comments only\n");
        fclose(workspace);
        return -1;
    }
    if (0 == rle_xsts) {
        printf(IFC"No RLE\n");
        fclose(workspace);
        return -1;
    }
    printf(IFC"No '!' found\n");
    return -1;
} //checks if the file is correctly composed (-1 if not)

size_t number_of_digits(size_t x) {
    if (0 == ((x - (x % 10)) / 10)) {
        return 1;
    }
    else {
        return (1 + number_of_digits((x - (x % 10)) / 10));
    }
} //number of digits in a given number

void set_rules(const char * string_of_rules, int * rfb, int * rfs) {
    size_t local_i = 0;
    size_t local_i_for_rules = 0;
    //reading the B rules
    while ('/' != string_of_rules[local_i]) {
        rfb[local_i_for_rules] = string_of_rules[local_i] - '0'; //turns char into int
        local_i += 1;
        local_i_for_rules += 1;
    }
    rfb[local_i_for_rules] = 9;
    local_i += 1;
    local_i_for_rules = 0;
    //reading the S rules
    while ('\0' != string_of_rules[local_i]) {
        rfs[local_i_for_rules] = string_of_rules[local_i] - '0'; //same as previous
        local_i += 1;
        local_i_for_rules += 1;
    }
    rfs[local_i_for_rules] = 9;
} //reads rules from string_of_rules and puts them in corresponding lists

void Print_on_screen(const char * mtrx_list, const unsigned long width, const unsigned long height) {
    printf("\n");
    for (int row_number = 0; row_number < height; row_number++) {
        for (int column_number = 0; column_number < width; column_number++) {
            if (ALIVE == mtrx_list[column_number + (row_number * width)]) {
                printf(ALIVE_FP);
            }
            else {
                printf(DEAD_FP);
            }
        }
        printf("\n");
    }
    printf("\n");
} //prints the matrix on screen

int G_N_o_I() {
    printf("Enter number of iterations. \n");
    int n_o_i = -1;
    scanf("%d", &n_o_i);
    if (n_o_i < 0) {
        return -1;
    }
    return n_o_i;
} //gets number of iteration (-1 if bad input)

size_t if_rules_apply(size_t counter, const size_t n_o_neighbours, const int * rules) {
    //last symbol in a list of rules
    if (9 == rules[counter]) {
        return 0;
    }
    //if the rule applies
    if (rules[counter] == n_o_neighbours) {
        return 1;
    }
        //if not, check the next value in a list of rules
    else {
        return if_rules_apply(counter + 1, n_o_neighbours, rules);
    }
} //returns 1 if rules apply to the subject, 0 if not

int add_to_matrix(char * mtrx_list, size_t x, size_t y, const unsigned long width) {
    size_t position = ((y * width) + x);
    mtrx_list[position] = ALIVE;
    return 0;
} //adds a cell to the matrix (height is not needed)

int read_the_file(FILE *filename, char *string_of_rules, unsigned long *width, unsigned long *height) {
    if (NULL == filename) {
        fprintf(stderr, "\nFile pointer is NULL in *read_the_file*\n");
        return -1;
    }
    int matrix_start_counter = 0; //a parameter that will be returned (do not put in loop)
    while (1) {
        char c[72] = {0};
        char sizer[47] = {0}; //a line is <= 70, it has "x = *, y = *, rules = B*/S*" = 23 and 70 - 23 = 47
        char *string_status = fgets(c, sizeof(c), filename);//workspace for filename
        if (NULL == string_status) {
            fprintf(stderr, "\nUnable to get a string in *read_the_file*\n");
            fclose(filename);
            return -1;
        } //bad line
        if ('#' == c[0]) {
            matrix_start_counter += 1;
        } //a comment line, skipping it
        else {
            if ('x' == c[0]) { //a line containing width, height and rules
                matrix_start_counter += 1;
                size_t temp_i_gnrl = 4; //jumping over symbols in a read line
                size_t temp_i_f = 0;
                while (',' != c[temp_i_gnrl]) {
                    sizer[temp_i_f] = c[temp_i_gnrl];
                    temp_i_gnrl += 1;
                    temp_i_f += 1;
                } //approaching first ',' (after width)
                sizer[temp_i_f] = '\0';
                char *proxie;
                *width = strtoul(sizer, &proxie, 10);
                temp_i_f = 0;
                temp_i_gnrl += 6; //jumping over symbols again
                while (',' != c[temp_i_gnrl]) {
                    sizer[temp_i_f] = c[temp_i_gnrl];
                    temp_i_gnrl += 1;
                    temp_i_f += 1;
                } //approaching second ',' (after height)
                sizer[temp_i_f] = '\0';
                *height = strtoul(sizer, &proxie, 10);
                //reading the rules
                temp_i_f = 0;
                temp_i_gnrl += 10; //jumping over symbols yet again
                while ('/' != c[temp_i_gnrl]) {
                    string_of_rules[temp_i_f] = c[temp_i_gnrl];
                    temp_i_gnrl += 1;
                    temp_i_f += 1;
                }
                string_of_rules[temp_i_f] = '/';
                temp_i_f += 1;
                temp_i_gnrl += 2; //jumping over '/S'
                while ('\n' != c[temp_i_gnrl]) {
                    string_of_rules[temp_i_f] = c[temp_i_gnrl];
                    temp_i_gnrl += 1;
                    temp_i_f += 1;
                }
                string_of_rules[temp_i_f] = '\0';
            }
            else {
                fclose(filename);
                return matrix_start_counter; //number of a first line that contains RLE
            } //encountered a line that is neither a comment nor width/height/rules (RLE)
        }
    }
} //changes the size, reads the rules, returns the number of a string with coordinates (-1 if cannot)

int read_the_coordinates(size_t starter, const char *TheFile, char *mtrx_to_add,\
                         const unsigned long width, const unsigned long height) {
    size_t c_row = 0;
    size_t c_column = 0;
    FILE * workspace = fopen(TheFile, "r");
    if (NULL == workspace) {
        fprintf(stderr, "\nUnable to open file in *read_the_coordinates*\n");
        return -1;
    }
    char c[72] = {0}; //exactly enough
    size_t total_already_in_line = 0; //to check how much space is left in a line of a matrix
    size_t total_shifts = 1;
    size_t maximka = 0;
    if (width >= height) {
        maximka = width;
    }
    else {
        maximka = height;
    } //what's bigger: width or height
    size_t len_of_max = number_of_digits(maximka); //number of digits in a biggest allowed coordinate
    char amm_list[len_of_max + 1]; //a list for values that allows no more digits than in height or width
    amm_list[0] = '1';  //so strtoul makes it a 1,
    amm_list[1] = '\0'; //same in a loop
    unsigned long amount = 0; //a value that comes from strtoul to a sequence of numbers in RLE
    size_t amm_i = 0; //a number of integers already read in a sequence of numbers
    for (int i = 0; i < starter; i++) {
        char *string_status = fgets(c, sizeof(c), workspace);
        if (NULL == string_status) {
            fprintf(stderr, "\nUnable to get a string in *read_the_coordinates*\n");
            fclose(workspace);
            return -1;
        }
    } //to move to the string with RLE coordinates by jumping over other strings
    int zero_defies_RLE_logic = 0;
    while (1) { //loop that reads the line
        char *string_status = fgets(c, sizeof(c), workspace);
        if (NULL == string_status) {
            fprintf(stderr, "\nUnable to get a string in *read_the_coordinates*\n");
            fclose(workspace);
            return -1;
        }
        size_t gnrl_i = 0;
        while (('\n' != c[gnrl_i]) && ('!' != c[gnrl_i])) {
            if ('o' == c[gnrl_i]) { //encountered an ALIVE symbol - put it into matrix "amount" of times
                amm_i = 0; //to indicate that programme is no longer reading a sequence of numbers
                zero_defies_RLE_logic = 0;
                char *proxie;
                amount = strtoul(amm_list, &proxie, 10);
                if (amount > (width - total_already_in_line)) {
                    printf(IFC"Too much in RLE\n");
                    fclose(workspace);
                    return -1;
                } //to not go out of bounds of the matrix
                for (int i = 0; i < amount; i++) {
                    add_to_matrix(mtrx_to_add, c_column, c_row, width);
                    total_already_in_line += 1;
                    c_column += 1;
                }
                gnrl_i += 1;
                amm_list[0] = '1';
                amm_list[1] = '\0';
            }
            if ('b' == c[gnrl_i]) { //encountered a DEAD symbol
                amm_i = 0; //to indicate that programme is no longer reading a sequence of numbers
                zero_defies_RLE_logic = 0;
                char *proxie;
                amount = strtoul(amm_list, &proxie, 10);
                //to not go out of bounds of the matrix
                if (amount > (width - total_already_in_line)) {
                    printf(IFC"Too much in RLE\n");
                    fclose(workspace);
                    return -1;
                }
                for (int i = 0; i < amount; i++) {
                    total_already_in_line += 1;
                    c_column += 1;
                } //do not need to add - the default state of a cell is DEAD
                gnrl_i += 1;
                amm_list[0] = '1';
                amm_list[1] = '\0';
            }
            if ('$' == c[gnrl_i]) { //encountered a line shift symbol
                amm_i = 0;
                zero_defies_RLE_logic = 0;
                char *proxie;
                amount = strtoul(amm_list, &proxie, 10);
                if (amount > (height - total_shifts)) {
                    printf(IFC"Too much in RLE\n");
                    fclose(workspace);
                    return -1;
                } //to not go out of bounds of the matrix
                for (int i = 0; i < amount; i++) {
                    total_shifts += 1;
                    c_row += 1;
                } //shift the line downwards an "amount" of times
                total_already_in_line = 0;
                c_column = 0;
                gnrl_i += 1;
                amm_list[0] = '1';
                amm_list[1] = '\0';
            }
            if ((c[gnrl_i] >= '0') && (c[gnrl_i] <= '9')) { //encountered a number
                if ((0 == zero_defies_RLE_logic) && ('0' == c[gnrl_i])) {
                    printf("\nEncountered a number that starts with 0 which defies RLE logic\n");
                    return -1;
                }
                else {
                    zero_defies_RLE_logic = 1;
                } //a crutch, but works
                //if the sequence of numbers is exceeding the allowed (len_of_max) amount of digits
                if (len_of_max == amm_i) {          //amm_i starts as 0, so it can be used
                    printf(IFC"Too much in RLE\n"); //as a position of an element in a list
                    fclose(workspace);              //so if len_of_max is 3 and amm_i is 3
                    return -1;                      //and there is another number in a line
                }                                   // '==' stops the programme
                amm_list[amm_i] = c[gnrl_i];
                amm_i += 1;
                amm_list[amm_i] = '\0';
                gnrl_i += 1;
            }
        }
        if ('!' == c[gnrl_i]) {
            break;
        }
    }
    fclose(workspace);
    return 0;
} //reads the coordinates from file (-1 if cannot)

size_t Neighbours_check(size_t subject_of_inspection, const char * mtrx_list,\
						const unsigned long width, const unsigned long height) {
    size_t n_o_neighbours = 0;
    size_t current_subject = 0;
    int row_modifier_a = 0;
    int row_modifier_b = 0;
    int column_modifier_r = 0;
    int column_modifier_l = 0;
    if (0 == ((subject_of_inspection - (subject_of_inspection % width)) / width)) {
        row_modifier_a = 1;
        row_modifier_b = 0;
    }
    if ((height - 1) == ((subject_of_inspection - (subject_of_inspection % width)) / width)) {
        row_modifier_a = 0;
        row_modifier_b = -1;
    }
    if (0 == (subject_of_inspection % width)) {
        column_modifier_r = 0;
        column_modifier_l = 1;
    }
    if ((width - 1) == (subject_of_inspection % width)) {
        column_modifier_r = -1;
        column_modifier_l = 0;
    }
    //to the right
    current_subject = (subject_of_inspection + (column_modifier_r * width) + 1);
    if (ALIVE == mtrx_list[current_subject]) {
        n_o_neighbours += 1;
    }
    //to the left
    current_subject = (subject_of_inspection + (column_modifier_l * width) - 1);
    if (ALIVE == mtrx_list[current_subject]) {
        n_o_neighbours += 1;
    }
    //above right
    current_subject = (subject_of_inspection + (row_modifier_a * (width * height))\
    + (column_modifier_r * width) - (width - 1));
    if (ALIVE == mtrx_list[current_subject]) {
        n_o_neighbours += 1;
    }
    //above left
    current_subject = (subject_of_inspection + (row_modifier_a * (width * height))\
    + (column_modifier_l * width) - (width + 1));
    if (ALIVE == mtrx_list[current_subject]) {
        n_o_neighbours += 1;
    }
    //above
    current_subject = (subject_of_inspection + (row_modifier_a * (width * height)) - width);
    if (ALIVE == mtrx_list[current_subject]) {
        n_o_neighbours += 1;
    }
    //below right
    current_subject = (subject_of_inspection + (row_modifier_b * (width * height))\
    + (column_modifier_r * width) + (width + 1));
    if (ALIVE == mtrx_list[current_subject]) {
        n_o_neighbours += 1;
    }
    //below left
    current_subject = (subject_of_inspection + (row_modifier_b * (width * height))\
    + (column_modifier_l * width) + (width - 1));
    if (ALIVE == mtrx_list[current_subject]) {
        n_o_neighbours += 1;
    }
    //below
    current_subject = (subject_of_inspection + (row_modifier_b * (width * height)) + width);
    if (ALIVE == mtrx_list[current_subject]) {
        n_o_neighbours += 1;
    }
    return n_o_neighbours;
} //returns number of neighbours

int Chngs_t_mtrx(const char * mtrx_list_a, char * mtrx_list_b, const int *rfb, const int *rfs,\
					const unsigned long width, const unsigned long height) {
    size_t temp_n_c = 0;
    for (size_t i = 0; i < (width * height); i++) {
        temp_n_c = Neighbours_check(i, mtrx_list_a, width, height);
        if ((DEAD == mtrx_list_a[i]) && (1 == if_rules_apply(0, temp_n_c, rfb))) {
            mtrx_list_b[i] = ALIVE;
        }
        else {
            if ((ALIVE == mtrx_list_a[i]) && (1 == if_rules_apply(0, temp_n_c, rfs))) {
                mtrx_list_b[i] = ALIVE;
            }
            else {
                mtrx_list_b[i] = DEAD;
            }
        }
    } //makes changes to the matrix according to the number of the cell's neighbours
    //checks if matrix a and matrix b are identical (0) or not (1)
    size_t i = 0;
    int equilibrium_status = 0; //(flag)
    while (i < (width * height)) {
        if (mtrx_list_b[i] != mtrx_list_a[i]) {
            equilibrium_status = 1;
            i = (width * height);
        }
        i += 1;
    }
    return equilibrium_status;
} //changes the matrix and checks if identical

int main(int argc, char *argv[]) {
    if (2 != argc) {
        printf("\nUnexpected amount of arguments\n");
        return -1;
    }
    int alzheimer = check_file(open_file_for_r(argv[1]));
    if (-1 == alzheimer) {
        return -1;
    }
    unsigned long width = 0; //most important parameters
    unsigned long height = 0; //ul because they are used with strtoul in programme
    char string_i_want[20]; // enough for (012345678/012345678\0) = 20
    int mtrx_starter = read_the_file(open_file_for_r(argv[1]), string_i_want, &width, &height);
    if (-1 == mtrx_starter) {
        return -1;
    }
    char * list_a = (char*)calloc((width * height), sizeof(*list_a));
    if (NULL == list_a) {
        fprintf(stderr, "\nUnable to allocate an appropriate amount of memory for list_a\n");
        return -1;
    }
    for (int i = 0; i < (width * height); i++) {
        list_a[i] = DEAD;
    }
    char * list_b = (char*)calloc((width * height), sizeof(*list_b));
    if (NULL == list_b) {
        free(list_a);
        fprintf(stderr, "\nUnable to allocate an appropriate amount of memory for list_b\n");
        return -1;
    }
    memcpy(list_b, list_a, (width * height));
    int status = read_the_coordinates(mtrx_starter, argv[1], list_a, width, height);
    if (-1 == status) {
        free(list_a);
        free(list_b);
        return -1;
    }
    int n_o_i = G_N_o_I();
    if (n_o_i == -1) {
        printf("Incorrect input\n");
        free(list_a);
        free(list_b);
        return -1;
    }
    int rfb[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,9}; //-1 is because we will be using
    int rfs[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,9}; // '||' comparison of numbers (also 9 is not allowed to be a rule)
    set_rules(string_i_want, rfb, rfs);
    size_t iteration_counter = 0;
    int equilibrium_check = 0;
    if (0 == n_o_i) {
        Print_on_screen(list_a, width, height);
        return 0;
    }
    while (iteration_counter < n_o_i) {
        equilibrium_check = Chngs_t_mtrx(list_a, list_b, rfb, rfs, width, height);
        if (0 == equilibrium_check) {
            printf("\nThe matrix reached the point of equilibrium on turn %zu\n", iteration_counter);
            iteration_counter = n_o_i;
        }
        else {
            memcpy(list_a, list_b, width * height); //copies b back to a
            Print_on_screen(list_a, width, height);
            iteration_counter += 1;
        }
    }
    return 0;
}
