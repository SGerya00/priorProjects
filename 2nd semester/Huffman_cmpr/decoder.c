#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NVR_D1_B4_nec_defines.h"

struct decoder_tree {
    char c;
    unsigned int tree_arr_pos;
    struct decoder_tree *b_left;
    struct decoder_tree *b_right;
};

//https://docs.microsoft.com/ru-ru/windows/wsl/install-win10
unsigned int count_tree_size(FILE *file) {
    assert(NULL != file);
    MY_LOG("\n--->Entered count_tree_size");

    unsigned int counter = 0;
    while(1) {
        int c = fgetc(file);
        counter += 1;
        if ((char)c == '\n') {
            int check = fgetc(file);
            fseek(file, -1, SEEK_CUR); //one char back
            if ((char)check == '_') {
                continue;
            } //just a symbol
            else {
                MY_LOG("\n--->Done, tree size is [%u]", counter - 1);
                fseek(file, 0, SEEK_SET); //because file pointer was changed with fgetc
                return counter - 1;
            } //end of tree in file
        }
    }
    assert(NULL);
} //counts

unsigned long long int get_n_o_chars(FILE *file) {
    assert(NULL != file);
    MY_LOG("\n Entered get_n_o_chars");

    unsigned long long int n_o_chars = 0;
    fscanf(file, "%llX", &n_o_chars);
    fseek(file, 1, SEEK_CUR); //move 1 byte further (after meta section)
    MY_LOG("\n N_O_chars if [%llu]", n_o_chars);
    return n_o_chars;
} //gets n_o_chars (aka initial file size), moves file pointer after meta section
//

void put_tree_from_file_to_arr(FILE *file, char (*arr)[]) {
    assert((NULL != file) && (NULL != arr));
    MY_LOG("\n  Entered put_tree_from_file_to_arr");

    unsigned int counter = 0;
    while(1) {
        int c = fgetc(file);
        if ('\n' == (char)c) {
            int cc = fgetc(file);
            fseek(file, -1, SEEK_CUR);
            if ('_' != (char)cc) {
                break;
            }
        }
        (*arr)[counter] = (char)c;
        counter += 1;
    }
    MY_LOG("\n  function is done");
} //puts file pointer before n_o_chars in meta section

unsigned int find_root(const char printed_tree[]) {
    assert(NULL != printed_tree);
    MY_LOG("\n--->Entered find_root");

    unsigned int root_position = 0;
    unsigned int bracket_counter = 0;
    unsigned int i = 0;
    while(1) {
        if (0 == bracket_counter) {
            if ('_' == printed_tree[i]) {
                root_position = i + 1;
                break;
            }
            else if ('$' == printed_tree[i]) {
                root_position = i;
                break;
            }
        }
        if ('(' == printed_tree[i]) {
            if ('_' != printed_tree[i - 1]) {
                bracket_counter += 1;
            }
        }
        else if (')' == printed_tree[i]) {
            if ('_' != printed_tree[i + 1]) {
                bracket_counter -= 1;
            }
        }
        else if ('_' == printed_tree[i]) {
            i += 2;
        }
        i += 1;
    }
    MY_LOG("\n--->root is found, it's position is [%u]", root_position);
    return root_position;
} //searches for root from left to right

//
unsigned int find_left(const char printed_tree[], unsigned int root_pos) {
    assert(NULL != printed_tree);
    MY_LOG("\n  entered find_left");

    unsigned int bracket_counter = 0;
    unsigned int found_pos = root_pos;
    unsigned int i = root_pos - 2; //to skip root and ')' that is to the left of it
    while(1) {
        if (0 == bracket_counter) {
            if ('_' == printed_tree[i]) {
                found_pos = i - 1;
                break;
            }
            else if ('$' == printed_tree[i]) {
                found_pos = i;
                break;
            }
        }
        if (')' == printed_tree[i]) {
            if ('_' != printed_tree[i + 1]) {
                bracket_counter += 1;
            }
        }
        else if ('(' == printed_tree[i]) {
            if ('_' != printed_tree[i - 1]) {
                bracket_counter -= 1;
            }
        }
        i -= 1;
    }
    MY_LOG("\n  left is found, it is [%u] (\"%c\")", found_pos, printed_tree[found_pos]);
    return found_pos;
}

unsigned int find_right(const char printed_tree[], unsigned int root_pos) {
    assert(NULL != printed_tree);
    MY_LOG("\n  entered find_right");

    unsigned int bracket_counter = 0;
    unsigned int found_pos = root_pos;
    unsigned int i = root_pos + 2; //to skip root and '(' that is to the right of it
    while(1) {
        if (0 == bracket_counter) {
            if ('_' == printed_tree[i]) {
                found_pos = i + 1;
                break;
            }
            else if ('$' == printed_tree[i]) {
                found_pos = i;
                break;
            }
        }
        if ('(' == printed_tree[i]) {
            if ('_' != printed_tree[i - 1]) {
                bracket_counter += 1;
            }
        }
        else if (')' == printed_tree[i]) {
            if ('_' != printed_tree[i + 1]) {
                bracket_counter -= 1;
            }
        }
        i += 1;
    }
    MY_LOG("\n  right is found, it is [%u] (\"%c\")", found_pos, printed_tree[found_pos]);
    return found_pos;
}
//

void build_decoder_tree_rec(const char printed_tree[], struct decoder_tree *root) {
    assert((NULL != printed_tree) && (NULL != root));
    MY_LOG("\n   --->Entered build_decoder_tree_rec");

    if (('_' == printed_tree[root->tree_arr_pos + 1]) && ('_' == printed_tree[root->tree_arr_pos - 1])) {
        MY_LOG("\n   --->\"%c\" is a leaf, returning from recursion", root->c);
        return;
    } //no need to go further, it's a leaf

    unsigned int where_left = find_left(printed_tree, root->tree_arr_pos);
    unsigned int where_right = find_right(printed_tree, root->tree_arr_pos);

    //make left
    struct decoder_tree *b_left_elem = (struct decoder_tree*)calloc(1, sizeof(struct decoder_tree));
    assert(NULL != b_left_elem);
    b_left_elem->tree_arr_pos = where_left;
    b_left_elem->c = printed_tree[where_left];
    MY_LOG("\n   --->left child is made, it is \"%c\"", b_left_elem->c);
    //
    //make right
    struct decoder_tree *b_right_elem = (struct decoder_tree*)calloc(1, sizeof(struct decoder_tree));
    assert(NULL != b_right_elem);
    b_right_elem->tree_arr_pos = where_right;
    b_right_elem->c = printed_tree[where_right];
    MY_LOG("\n   --->right child is made, it is \"%c\"", b_right_elem->c);
    //
    //attach
    root->b_left = b_left_elem;
    root->b_right = b_right_elem;
    MY_LOG("\n   --->attachment complete");
    //
    //recursive left
    build_decoder_tree_rec(printed_tree, root->b_left);
    //
    //recursive right
    build_decoder_tree_rec(printed_tree, root->b_right);
    //
} //builds decoder tree

int is_leaf(struct decoder_tree *potential_leaf) {
    assert(NULL != potential_leaf);

    if ((NULL == potential_leaf->b_left) && (NULL == potential_leaf->b_right)) {
        MY_LOG("\n \'%c\' is indeed a leaf", potential_leaf->c);
        return 1;
    }
    return 0;
} //1 if leaf, 0 if not

void decode_in_output(FILE *input, FILE *output, struct decoder_tree *root, unsigned long long int n_o_chars) {
    assert((NULL != input) && (NULL != output) && (NULL != root));
    MY_LOG("\n--->Entered decode_in_output");

    struct decoder_tree *curr_node = root;
    unsigned long long int n_o_chars_done = 0;
    while(1) {
        int curr_char = fgetc(input);
        MY_LOG("\n   read char is ---> \n\'%d\'\n", curr_char);
        unsigned char one_zero = ONE_ZERO;
        for (unsigned int i = 0; i < 8; i++) {

            unsigned char for_comp = (unsigned char)((one_zero >> i) & (curr_char));

            if (is_leaf(curr_node)) {
                //skip
                /*
                n_o_chars_done += 1;
                fprintf(output, "%c", curr_node->c);
                MY_LOG("\nFound char, it is \n\'%c\'\n character\n", curr_node->c);
                curr_node = root; //back to root
                 */
            } else {
                if ((one_zero >> i) == (for_comp)) {
                    MY_LOG("\ngoing left ");
                    curr_node = curr_node->b_left; //go left if 1
                } else {
                    MY_LOG("\ngoing right ");
                    curr_node = curr_node->b_right; //go right if 0;
                }
            }
            if (is_leaf(curr_node)) {
                n_o_chars_done += 1;
                fprintf(output, "%c", curr_node->c);
                MY_LOG("\nFound char, it is \n\'%c\'\n character\n", curr_node->c);
                curr_node = root; //back to root
            }
            if (n_o_chars_done == n_o_chars) {
                MY_LOG("\n--->Decoding is done(vol'no)\n");
                return;
            }
        }
    }
}

void free_decoder_tree_rec(struct decoder_tree *root) {
    assert(NULL != root);

    if ((NULL == root->b_right) && (NULL == root->b_left)) {
        free(root);
        return;
    }
    free_decoder_tree_rec(root->b_left);
    free_decoder_tree_rec(root->b_right);
    free(root);
}

void free_everything_dec(struct decoder_tree *root, char (*arr)[]) {
    if (NULL != root) {
        MY_LOG("\n->Freeing tree");
        free_decoder_tree_rec(root);
    }
    if (NULL != arr) {
        MY_LOG("\n->Freeing tree in char array");
        free(arr);
    }
    MY_LOG("\n->Freeing complete");
}

int main_decoder(FILE *input_file, FILE *output_file) {
    assert((NULL != input_file) && (NULL != output_file));

    unsigned int tree_size = count_tree_size(input_file);

    char (*printed_tree_arr)[] = (char (*)[])calloc(tree_size, sizeof(char));
    assert(NULL != printed_tree_arr);
    put_tree_from_file_to_arr(input_file, printed_tree_arr);

    struct decoder_tree *actual_root = (struct decoder_tree*)calloc(1, sizeof(struct decoder_tree));
    assert(NULL != actual_root);
    //initialise
    unsigned int actual_root_pos = find_root(*printed_tree_arr);
    actual_root->tree_arr_pos = actual_root_pos;
    actual_root->c = (*printed_tree_arr)[actual_root_pos];
    actual_root->b_left = NULL;
    actual_root->b_right = NULL;
    //
    PROGRESS_LOG("\nTrying to build decoder tree...");
    build_decoder_tree_rec(*printed_tree_arr, actual_root);
    PROGRESS_LOG("\nDecoder tree is successfully built");

    unsigned long long int n_o_chars = get_n_o_chars(input_file);
    PROGRESS_LOG("\nn_o_chars is [%llu]", n_o_chars); //-----------------------------------------
    PROGRESS_LOG("\nDecoding into output file...");
    decode_in_output(input_file, output_file, actual_root, n_o_chars);
    PROGRESS_LOG("\nDecoded data is successfully put in newly created output file");

    free_everything_dec(actual_root, printed_tree_arr);
    PROGRESS_LOG("\n%s is done", __FUNCTION__);
    return 0;
}
