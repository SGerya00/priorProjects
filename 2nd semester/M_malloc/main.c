#include <stdio.h>
#include "M_malloc.h"

int main() {
    ask_for_memory_for_my_malloc();
    prepare_memory_for_usage();
    visualise_memory();

    char *word = (char*)M_malloc(10);
    if (NULL == word) {
        printf("\ncould not allocate enough memory for \"word\"");
    } else {
        word[0] = 'H';
        word[1] = 'H';
        word[2] = 'H';
        word[3] = 'H';
        word[4] = 'H';
        word[5] = 'H';
        word[6] = 'H';
        word[7] = 'H';
        word[8] = 'H';
        word[9] = '\0';
    }
    size_t *number_sequence = (size_t *)M_malloc(sizeof(size_t) * 7);
    if (NULL == number_sequence) {
        printf("\ncould not allocate enough memory for \"number_sequence\"");
    } else {
        number_sequence[0] = 0;
        number_sequence[1] = 1;
        number_sequence[2] = 2;
        number_sequence[3] = 3;
        number_sequence[4] = 4;
        number_sequence[5] = 5;
        number_sequence[6] = 6;
    }
    void *pointers_to_words[15] = {0};
    for (register int i = 0; i < 15; i++) {
        char *new_word = (char*)M_malloc(12);
        if (NULL != new_word) {
            pointers_to_words[i] = new_word;
            new_word[0] = '\0';
            new_word[1] = '\0';
            new_word[2] = '\0';
            new_word[3] = '\0';
            new_word[4] = '\0';
            new_word[5] = '\0';
            new_word[6] = '\0';
            new_word[7] = '\0';
            new_word[8] = '\0';
            new_word[9] = '\0';
            new_word[10] = '\0';
            new_word[11] = '\0';
        } else {
            printf("\non step (%d) could not get enough memory...", i);
        }
    }
    visualise_memory();

    M_free(number_sequence);
    M_free(pointers_to_words[3]);
    M_free(pointers_to_words[6]);
    M_free(pointers_to_words[9]);
    visualise_memory();

    size_t *big_number_seq1 = (size_t *)M_malloc(2000);
    size_t *big_number_seq2 = (size_t *)M_malloc(3000);
    size_t *big_number_seq3 = (size_t *)M_malloc(600);
    visualise_memory();
    M_free(big_number_seq1);
    M_free(big_number_seq2);
    visualise_memory();


    give_back_memory();
    return 0;
}
