#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <string.h>

#include "NVR_D1_B4_nec_defines.h"

//
typedef unsigned long long int ulli;
//

struct tree {
    unsigned char c;
    ulli char_freq;
    int used_flag;
    short int left_child_pos; //-1 if leaf
    short int right_child_pos; //-1 if leaf
};

struct encoder {
    unsigned short int bit_code_len;
    unsigned char * code;
};

struct q_element {
    unsigned short int pos_in_tree;
    unsigned short int bit_code_len;
    unsigned char * code;
    struct q_element * above;
    struct q_element * below;
};


ulli fill_freq_arr(FILE *input, ulli * const freq_arr) {
    assert(NULL != input);
    assert(NULL != freq_arr);

    register ulli counter = 0;
    int read_char = 0;
    while(1) {
        read_char = fgetc(input);
        if(EOF == read_char) {
            break;
        }
        counter += 1;
        freq_arr[((unsigned int)read_char)] += 1;
    }
    MY_LOG("\nin {%s} counted total of %llu symbols\n", __FUNCTION__, counter);
    return counter;
} //returns total chars read

void fill_tree_arr(const ulli * const freq_arr, struct tree * const tree_arr) {
    assert(NULL != freq_arr);
    assert(NULL != tree_arr);

    register unsigned short int tree_i = 0;
    for (register unsigned short int i = 0; i < MAX_ARR_SIZE; i++) {
        if (0 != freq_arr[i]) {
            MY_LOG("\nin {%s} in freq_arr slot [%hu] is not empty (char is \'%c\'", __FUNCTION__, i, (unsigned char)i);
            //make tree_arr element
            tree_arr[tree_i].c = (unsigned char)i;
            tree_arr[tree_i].char_freq = freq_arr[i];
            tree_arr[tree_i].used_flag = NOT_USED;
            tree_arr[tree_i].left_child_pos = -1;
            tree_arr[tree_i].right_child_pos = -1;
            //
            tree_i += 1;
        }
    }
    MY_LOG("\n");
}

short int find_free_slot(struct tree * const tree_arr) {
    assert(NULL != tree_arr);

    for (register unsigned short int i = 0; i < (MAX_ARR_SIZE * 2); i++) {
        if (0 != tree_arr[i].char_freq) {
            continue;
        } else {
            MY_LOG("\nin {%s} free slot is found, it is %hu", __FUNCTION__, i);
            return ((short int)i);
        }
    }
    assert(NULL);
    return -1; //not supposed to get here
} //returns position of a free slot (-1 if no free slot)

short int get_smallest(struct tree * const tree_arr) {
    assert(NULL != tree_arr);

    short int smallest = -1; //to detect whether smallest is found or not
    ulli temp_max = (ulli)ULLONG_MAX;
    for (register unsigned short int i = 0; i < (MAX_ARR_SIZE * 2); i++) {
        if (0 == tree_arr[i].char_freq) {
            break;
        } //means we passed all actual elements and are now on empty ones
        if (USED == tree_arr[i].used_flag) {
            continue;
        } //skip used ones
        if (tree_arr[i].char_freq <= temp_max) {
            temp_max = tree_arr[i].char_freq;
            smallest = (short int)i;
        }
    }
    if (-1 != smallest) {
        MY_LOG("\nin {%s} [smallest (%hu) is found, it is ((%u)), it's freq is {%llu}]",
               __FUNCTION__, smallest, (unsigned int)(tree_arr[smallest].c), tree_arr[smallest].char_freq);
    }
    return smallest;
} //returns position of smallest out of NOT_USED (-1 if cannot find smallest)

short int build_tree_in_array(struct tree * const tree_arr) {
    assert(NULL != tree_arr);
    MY_LOG("\nin {%s} building tree in tree_arr", __FUNCTION__);
    short int root_pos = -1;
    while(1) {
        short int smallest1 = get_smallest(tree_arr); //first
        assert(-1 != smallest1); //-------------------------------------------------------------------------------------
        tree_arr[smallest1].used_flag = USED;
        short int smallest2 = get_smallest(tree_arr); //second
        if (-1 != smallest2) {
            MY_LOG("\nin {%s} 2 smallest were found...", __FUNCTION__);
            tree_arr[smallest2].used_flag = USED;
            short int free_slot = find_free_slot(tree_arr);
            assert(-1 != free_slot); //---------------------------------------------------------------------------------
            //make new tree element
            /* tree_arr[free_slot].c = '\0'; */ //not necessary
            tree_arr[free_slot].used_flag = NOT_USED;
            tree_arr[free_slot].char_freq = tree_arr[smallest1].char_freq + tree_arr[smallest2].char_freq;
            tree_arr[free_slot].left_child_pos = smallest1;
            tree_arr[free_slot].right_child_pos = smallest2;
            //
        } //managed to find 2 element to unite them
        else {
            MY_LOG("\nin {%s} only 1 smallest was found...", __FUNCTION__);
            root_pos = smallest1;
            MY_LOG("\nit is %hu (root btw)", root_pos);
            break;
        } //didn't find smallest2 -> one tree element left without pair -> it's root
    }
    assert(-1 != root_pos); //------------------------------------------------------------------------------------------
    MY_LOG("\nbuilding is done\n");
    return root_pos;
} //builds tree in tree_arr and returns position of it's root (last .freq != 0 element)

int q_is_empty(struct q_element * const last, struct q_element * const first) {
    assert(NULL != last);
    assert(NULL != first);
    MY_LOG("\nchecking whether q is empty or not");
    return ((last->above == first) || (first->below == last));
} //1 if yes, 0 if no

int corresponds_to_a_leaf(struct q_element * const current, struct tree * const tree_arr) {
    assert(NULL != current);
    assert(NULL != tree_arr);
    MY_LOG("\nchecking if corresponds to a leaf");
    return (-1 == tree_arr[current->pos_in_tree].left_child_pos); //can be right - both are -1 or neither are -1
} //1 if yes, 0 if no

unsigned char * allocate_for_code() {
    MY_LOG("\nallocating for code");
    unsigned char * byte_code = (unsigned char *)calloc(MAX_CODE_LEN, sizeof(char));
    assert(NULL != byte_code);
    return byte_code;
} //allocates memory with calloc and returns pointer

void insert_in_q(struct q_element * const for_insertion, struct q_element * const last) {
    assert(NULL != for_insertion);
    assert(NULL != last);
    MY_LOG("\ninserting [%hu] in q", for_insertion->pos_in_tree);
    for_insertion->above = last->above;
    for_insertion->below = last;
    last->above = for_insertion;
    for_insertion->above->below = for_insertion;
} //before last

void remove_from_q_and_delete(struct q_element *current, int w_code_or_not) {
    assert(NULL != current);
    MY_LOG("\nremoving from q and deleting [%hu]", current->pos_in_tree);
    //remove from q
    current->above->below = current->below;
    current->below->above = current->above;
    //
    //delete current
    if (1 == w_code_or_not) {
        free(current->code);
    }
    free(current);
    //
} //removes from q and frees element bcs it was previously allocated (if w_c_o_n == 1 -> frees .code as well)

void make_child_code(struct q_element * const parent, struct q_element * const child, int left_or_right) {
    assert(NULL != parent);
    assert(NULL != child);
    MY_LOG("\nmaking child_code of [%hu], left/right %d", parent->pos_in_tree, left_or_right);
    child->code = allocate_for_code();
    memcpy(child->code, parent->code, MAX_CODE_LEN);
    if (left_or_right) {
        unsigned int offset = (parent->bit_code_len) % 8;
        unsigned int which_char = (parent->bit_code_len) / 8;
        (child->code)[which_char] = (  (child->code)[which_char] | (ONE_ZERO >> offset) );
    }
} //(allocates memory) copies parent code and inserts 1 after it if necessary

struct q_element * make_child(struct q_element * const parent, struct tree * const tree_arr, int left_or_right) {
    assert(NULL != parent);
    assert(NULL != tree_arr);
    MY_LOG("\nmaking child of [%hu], left/right %d", parent->pos_in_tree, left_or_right);
    struct q_element * const child = (struct q_element * const)calloc(1, sizeof(struct q_element));
    assert(NULL != child);

    //make child
    if (left_or_right) {
        child->pos_in_tree = tree_arr[parent->pos_in_tree].left_child_pos;
    } else {
        child->pos_in_tree = tree_arr[parent->pos_in_tree].right_child_pos;
    }
    //
    make_child_code(parent, child, left_or_right);
    //
    child->bit_code_len = parent->bit_code_len + 1;
    //
    return child;
}

void build_enc_arr_with_tree(struct encoder * const encoder_arr, struct tree * const tree_arr, const short int root) {
    assert(NULL != encoder_arr);
    assert(NULL != tree_arr);
    assert(root >= 0); //-----------------------------------------------------------------------------------------------
    MY_LOG("\nin {%s} building encoder array", __FUNCTION__);
    //queue
    struct q_element first = {0, 0, NULL, NULL, NULL};
    struct q_element last = {0, 0, NULL, NULL, NULL};
    first.below = &last;
    last.above = &first;
    //

    //initial queue element
    struct q_element * const q_root = (struct q_element * const)calloc(1, sizeof(struct q_element));
    assert(NULL != q_root);
    q_root->code = allocate_for_code();
    q_root->bit_code_len = 0;
    q_root->pos_in_tree = root;
    insert_in_q(q_root, &last);
    //

    while (!q_is_empty(&last, &first)) {
        struct q_element * current = first.below;
        MY_LOG("\ngot element from q, it's code is [%d]", (int)(signed char)((current->code)[0]));
        if (corresponds_to_a_leaf(current, tree_arr)) {
            //fill encoder array element
            unsigned int u_char_code = (unsigned int)(tree_arr[(current->pos_in_tree)].c);
            MY_LOG("\nfound a char, it is (%d)", (int)(signed char)tree_arr[(current->pos_in_tree)].c);
            encoder_arr[u_char_code].bit_code_len = current->bit_code_len;
            encoder_arr[u_char_code].code = current->code;
            assert(NULL != encoder_arr[u_char_code].code); //-----------------------------------------------------------
            //
            remove_from_q_and_delete(current, 0);
        } else {
            //make left child
            struct q_element * left_child = make_child(current, tree_arr, 1);
            MY_LOG("\nleft child is made, it's code is [%d]\n", (int)(signed char)((left_child->code)[0]));
            //
            //make right child
            struct q_element * right_child = make_child(current, tree_arr, 0);
            MY_LOG("\nright child is made, it's code is [%d]\n", (int)(signed char)((right_child->code)[0]));
            //
            insert_in_q(left_child, &last);   //left then right
            insert_in_q(right_child, &last);  //order matters
            remove_from_q_and_delete(current, 1);
        }
    }
    MY_LOG("\nin {%s} encoding array is done\n", __FUNCTION__);
} //builds encoder array (read char serves as index)

void print_meta_rec(FILE *file, const struct tree current, struct tree * const tree_arr) {
    assert(NULL != file);
    assert(NULL != tree_arr);
    if ((-1 == current.left_child_pos) && (-1 == current.right_child_pos)) {
        fprintf(file, "_%c_", current.c); //_ _
        return;
    } //current is leaf

    fprintf(file, "(");
    print_meta_rec(file, tree_arr[current.left_child_pos], tree_arr);
    fprintf(file, ")$(");
    print_meta_rec(file, tree_arr[current.right_child_pos], tree_arr);
    fprintf(file, ")");
}

void print_meta(FILE *file, struct tree * const tree_arr, const short int root, ulli size) {
    assert(NULL != file);
    assert(NULL != tree_arr);
    MY_LOG("\nprinting meta");
    fprintf(file, "[");
    print_meta_rec(file, tree_arr[root], tree_arr);
    fprintf(file, "]");
    fprintf(file, "\n%llX\n", size);
}

void encode_from_to(FILE *input, FILE *output, const struct encoder * const enc_arr) {
    assert(NULL != input);
    assert(NULL != output);
    assert(NULL != enc_arr);
    MY_LOG("\nin {%s} started encoding", __FUNCTION__);
    fseek(input, 0, SEEK_SET);

    unsigned char buff = ZERO_ZERO;
    unsigned int prev_offset = 0;
    while (1) {
        int cur_char = fgetc(input);
        if (EOF == cur_char) {
            break;
        }
        struct encoder acc_enc = enc_arr[((unsigned int)cur_char)];

        unsigned int length = acc_enc.bit_code_len;
        MY_LOG("\nacc_enc is -> (%u), len is {%u}", (unsigned int)acc_enc.code[0], acc_enc.bit_code_len);
        if (0 == length) {
            length = 1;
        }
        unsigned int counter = 0;
        while (1) {
            MY_LOG("\n[[prev_offset is {%u}", prev_offset);
            unsigned char for_or = (unsigned char)(((acc_enc.code)[counter]) >> prev_offset);
            MY_LOG(" for_or is [%u]", (unsigned int)for_or);
            buff = (unsigned char)(buff | for_or);
            MY_LOG(" buff is (%u)]]", (unsigned int)buff);
            if (8 <= (prev_offset + length)) {
                MY_LOG("\n\n-----Writing %u to output\n", (unsigned int)buff);
                fprintf(output, "%c", buff); //write
                buff = ZERO_ZERO; //reset
                buff = (unsigned char)( buff | (((acc_enc.code)[counter]) << (8 - prev_offset)) ); //rest
            }
            if (length >= 8) {
                prev_offset = ((prev_offset + 8) % 8);
                length -= 8;
            } else {
                prev_offset = ((prev_offset + length) % 8);
                length = 0;
            }
            counter += 1;
            if (counter > (acc_enc.bit_code_len / 8)) {
                break;
            }
            if (8 == acc_enc.bit_code_len) {
                break;
            }
        }
    }
    fprintf(output, "%c", buff);
    MY_LOG("in {%s} encoding is finished...", __FUNCTION__);
}

void free_all(ulli * const freq_arr, struct tree * const tree_arr, struct encoder * const enc_arr) {
    assert(NULL != freq_arr);
    assert(NULL != tree_arr);
    assert(NULL != enc_arr);
    MY_LOG("\nin {%s} trying to free", __FUNCTION__);
    for (register unsigned short int i = 0; i < MAX_ARR_SIZE; i++) {
        if (NULL != enc_arr[i].code) {
            MY_LOG("\nfreeing code for [%hu] in encode array", i);
            free(enc_arr[i].code);
        }
    }
    MY_LOG("\nfreeing encode_array");
    free(enc_arr);
    MY_LOG("\nfreeing tree_array");
    free(tree_arr);
    MY_LOG("\nfreeing freq_array");
    free(freq_arr);
    MY_LOG("\nin {%s} freeing is done\n", __FUNCTION__);
}

int main_encoder2(FILE *input, FILE *output) {
    assert(NULL != input);
    assert(NULL != output);

    ulli * const freq_arr = (ulli * const)calloc(MAX_ARR_SIZE, sizeof(ulli));
    assert(NULL != freq_arr);
    PROGRESS_LOG("\nCounting frequencies...");
    ulli total_chars = fill_freq_arr(input, freq_arr);
    PROGRESS_LOG("\nCounted, total of [%llu]", total_chars);
    assert(total_chars != 0); //----------------------------------------------------------------------------------------

    struct tree * const tree_arr = (struct tree * const)calloc(MAX_ARR_SIZE * 2, sizeof(struct tree));
    assert(NULL != tree_arr);
    PROGRESS_LOG("\nBuilding tree array...");
    fill_tree_arr(freq_arr, tree_arr);

    short int root = build_tree_in_array(tree_arr);
    assert(-1 != root); //----------------------------------------------------------------------------------------------
    PROGRESS_LOG("\nBuilding done, root found");

    struct encoder * const encoder_arr = (struct encoder * const)calloc(MAX_ARR_SIZE, sizeof(struct encoder));
    assert(NULL != encoder_arr);
    PROGRESS_LOG("\nBuilding encoder array...");
    build_enc_arr_with_tree(encoder_arr, tree_arr, root);
    PROGRESS_LOG("\nBuilding encoder array done, printing meta...");
    print_meta(output, tree_arr, root, total_chars);
    PROGRESS_LOG("\nPrinting meta done, encoding...");
    encode_from_to(input, output, encoder_arr);
    PROGRESS_LOG("\nEncoding done, freeing memory...");
    free_all(freq_arr, tree_arr, encoder_arr);
    PROGRESS_LOG("\nFreeing done, all done");
    return 0;
}
