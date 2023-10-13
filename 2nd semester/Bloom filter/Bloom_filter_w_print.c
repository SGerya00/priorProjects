#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "hash_functions.h"

#define MAX_HASH_FUNCS 5
#define ALLOWED_SIZE 25
#define ALLOWED_SIZE_FP "%25s"
#define ONE_ZERO 0x80 //10000000
#define preferred_type char

struct Bloom_filter {
    size_t (*(*arr_of_hash_funcs)[MAX_HASH_FUNCS])();
    int n_o_hash_funcs;
    size_t filter_size; //in bytes
    preferred_type (*Bloom_filter_bit_array)[];
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void recursive(unsigned int copy, int counter) {
    if (7 == counter) {
        printf("%u", copy % 2);
    } else {
        recursive(copy / 2, counter + 1);
        printf("%u", copy % 2);
    }
}
void print_bit_arr_int_bits(struct Bloom_filter filter) {
    printf("\n\n -- Bit array looks like this --\n");
    for (size_t i = 0; i < filter.filter_size; i++) {
        unsigned int copy = (unsigned int)((*filter.Bloom_filter_bit_array)[i]);
        recursive(copy, 0);
        printf("(%d)", (unsigned int)((*filter.Bloom_filter_bit_array)[i]));
        printf(" ");
    }
    printf("\n");
}
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void fill_hash_results(const char *check_word, struct Bloom_filter filter, size_t (*hash_results)[]) {
    assert((NULL != check_word) && (NULL != hash_results));

    printf("\nHash results for this word are:"); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    for (int i = 0; i < filter.n_o_hash_funcs; i++) {
        (*hash_results)[i] = (*filter.arr_of_hash_funcs)[i](check_word);
        printf("%zu ", (*hash_results)[i]); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
}

void add_to_filter(const size_t hash_result, struct Bloom_filter filter) { //size8_t!!

    size_t arr_position = hash_result % (filter.filter_size * 8); //to not go out of bounds
    printf("\n     arr_position is %zu", arr_position); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    size_t char_position = arr_position / 8; //which char in array
    printf("\n     char_position is %zu", char_position); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    size_t in_char_position = arr_position % 8; //which bit in char
    printf("\n     in_char_position is %zu", in_char_position); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    preferred_type zero_one = (preferred_type)(ONE_ZERO >> (in_char_position)); //size8_t!!
    (*filter.Bloom_filter_bit_array)[char_position] = 
    	(preferred_type)(zero_one | (*filter.Bloom_filter_bit_array)[char_position]); //size8_t!!
}

int check_filter_table(const size_t hash_result, struct Bloom_filter filter) { //size8_t!!

    size_t arr_position = hash_result % (filter.filter_size * 8); //to not go out of bounds
    printf("\n     arr_position is %zu", arr_position); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    size_t char_position = arr_position / 8; //char in array
    printf("\n     char_position is %zu", char_position); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    size_t in_char_position = arr_position % 8; //bit in char
    printf("\n     in_char_position is %zu", in_char_position); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    preferred_type zero_one = (preferred_type)(ONE_ZERO >> in_char_position); //size8_t!!
    if (zero_one == (zero_one & ((*filter.Bloom_filter_bit_array)[char_position]))) {
        return 1;
    } //good, belongs to filter array
    return 0; //doesn't belong
}

int check_if_exists(const char *check_word, struct Bloom_filter filter) {
    assert(NULL != check_word);

    size_t hash_results[filter.n_o_hash_funcs];
    memset(hash_results, 0 ,filter.n_o_hash_funcs);
    fill_hash_results(check_word, filter, &hash_results);
    for (int i = 0; i < filter.n_o_hash_funcs; i++) {
        if (0 == check_filter_table(hash_results[i], filter)) {
            return 0;
        } //doesn't exist
    }
    return 1; //does exist
}

void put_word_in_filter(struct Bloom_filter filter, const char current_word[ALLOWED_SIZE]) {
    assert(NULL != current_word);

    size_t hash_results[filter.n_o_hash_funcs];
    memset(hash_results, 0, filter.n_o_hash_funcs); //memset
    fill_hash_results(current_word, filter, &hash_results);
    for (int j = 0; j < filter.n_o_hash_funcs; j++) {
        printf("\nAdding hash result %zu to filter:", hash_results[j]); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        add_to_filter(hash_results[j], filter);
    }
}

void fill_filter_table(FILE *file, int n_o_words, struct Bloom_filter filter) { //size8_t!!
    assert(NULL != file);

    for (int i = 0; i < n_o_words; i++) {
        char current_word[ALLOWED_SIZE] = {0};
        fscanf(file, ALLOWED_SIZE_FP, current_word); //read word
        fscanf(file, "%*[^ \n\0]"); //skip rest of word
        printf("\n\nRead word is ---> %s", current_word); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //
        put_word_in_filter(filter, current_word);
        //
        print_bit_arr_int_bits(filter); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
}

int count_words(FILE *file) {
    assert(NULL != file);

    int word_counter = 0;
    while(1) {
        fscanf(file, "%*[ \n]"); //skip all (white-space)s and (\n)s
        int read_char = fgetc(file);
        if (EOF == read_char) {
            break;
        }
        word_counter += 1;
        fscanf(file, "%*[^ \n\0]"); //skip all actual characters
    }
    fseek(file, 0, SEEK_SET); //back to beginning of file
    return word_counter;
}

int get_n_o_hash_funcs() {
    int n_o_hash_funcs = 0;
    while(1) {
        printf("\nPlease enter number of hash functions: ");
        if (1 == scanf("%d", &n_o_hash_funcs)) {
            if ((MAX_HASH_FUNCS < n_o_hash_funcs) || (1 > n_o_hash_funcs)) {
                printf("\nBad input, to many (or < 1) hash functions chosen, %d available\n", MAX_HASH_FUNCS);
                continue;
            }
            else {
                break;
            }
        } else {
            scanf("%*s"); //works
            printf("\nBad input, %d hash functions available\n", MAX_HASH_FUNCS);
            continue;
        }
    }
    return n_o_hash_funcs;
}

FILE * check_args(int argc, char *argv[]) {
    assert(NULL != argv);

    if (2 != argc) {
        fprintf(stderr, "\nUnexpected number of arguments, expected 2\n");
        return NULL;
    }
    FILE *file = fopen(argv[1], "r");
    if (NULL == file) {
        fprintf(stderr, "\nUnable to open file %s\n", argv[1]);
        return NULL;
    }
    return file;
}

int main(int argc, char *argv[]) {
    FILE *file = check_args(argc, argv);
    if (NULL == file) {
        return 0;
    }
    printf("\nFile %s is open", argv[1]); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    int word_counter = count_words(file); //n_o_words
    if (0 == word_counter) {
        printf("\nFile is empty\n");
        fclose(file);
        return 0;
    }
    printf("\nFound %d words in file", word_counter); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    struct Bloom_filter filter;
    filter.n_o_hash_funcs = get_n_o_hash_funcs(); //how many you would like to use
    filter.filter_size = (word_counter * filter.n_o_hash_funcs); //size of Bloom_filter_array
    printf("\nFilter size in bytes is %zu, in bits ---> %zu", filter.filter_size, filter.filter_size * 8); //!!!!!!!!!!!
    size_t (*func_arr[])(const char *) = {&hash_func1, &hash_func2, &hash_func3, &hash_func4, &hash_func5};
    filter.arr_of_hash_funcs = &func_arr;
    preferred_type (*Bloom_filter_array)[] = 
    	(preferred_type (*)[])calloc(filter.filter_size, sizeof(preferred_type)); //size8_t!!
    if (NULL == Bloom_filter_array) {
        fprintf(stderr, "\nUnable to allocate enough memory for a Bloom filter bit array\n");
        fclose(file);
        return 0;
    }
    printf("\nSuccessfully allocated enough memory for filter bit array"); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    filter.Bloom_filter_bit_array = Bloom_filter_array;
    print_bit_arr_int_bits(filter); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    fill_filter_table(file, word_counter, filter);
    fclose(file);

    char new_word[ALLOWED_SIZE] = {0};
    fprintf(stdout, "\nPlease enter your word: ");
    fscanf(stdin, ALLOWED_SIZE_FP, new_word);
    int flag = check_if_exists(new_word, filter);
    if (0 == flag) {
        printf("\nDoesn't exist\n");
    } else {
        printf("\nExists\n");
    }
    free(Bloom_filter_array);
    return 0;
}
