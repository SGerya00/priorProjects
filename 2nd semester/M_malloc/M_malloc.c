#include "M_malloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define N_O_SMALL_BLOCKS 16 //128
#define S_O_SMALL_BLOCK_MEMORY 16
#define S_O_S_A_INFO_BLOCK (sizeof(void*) + sizeof(int))
//void* for pointer to correspondent block, int for "taken after this one"
#define S_O_S_A_INFO_SECTION ((N_O_SMALL_BLOCKS) * (S_O_S_A_INFO_BLOCK))
#define S_O_S_A_MEMORY_SECTION ((N_O_SMALL_BLOCKS) * (S_O_SMALL_BLOCK_MEMORY))
#define MEMORY_FOR_S_A ((S_O_S_A_INFO_SECTION) + (S_O_S_A_MEMORY_SECTION))
// (|void*||int|...|void*||int||memory block 1||memory block 2|...|memory block last|)


//
#define N_O_BIG_BLOCKS 16 //128
#define S_O_BIG_BLOCK_MEMORY 512
#define S_O_L_A_ADDITIONAL_INFO ((sizeof(int)) * 2) //int for size of at front and back of block
#define S_O_L_A_BLOCK ((S_O_BIG_BLOCK_MEMORY) + (S_O_L_A_ADDITIONAL_INFO))
#define MEMORY_FOR_L_A ((N_O_BIG_BLOCKS) * (S_O_L_A_BLOCK))
// (|int||memory block 1||int| |int||memory block 2||int| ... |int||memory block last||int|)

char *memory_for_small_allocations;
char *memory_for_large_allocations;

//
void ask_for_memory_for_my_malloc() {

    memory_for_small_allocations = (char*)malloc(MEMORY_FOR_S_A);
    assert(NULL != memory_for_small_allocations);
    memory_for_large_allocations = (char*)malloc(MEMORY_FOR_L_A);
    assert(NULL != memory_for_large_allocations);
}

void give_back_memory() {
    assert(NULL != memory_for_small_allocations);
    assert(NULL != memory_for_large_allocations);
    free(memory_for_small_allocations);
    free(memory_for_large_allocations);
}
//

void prepare_memory_for_usage() {
    //small allocations
    char *copy_of_MFSA = memory_for_small_allocations;
    char *pointer_to_info_section = copy_of_MFSA;
    char *pointer_to_memory_section = (copy_of_MFSA + (N_O_SMALL_BLOCKS * S_O_S_A_INFO_BLOCK));
    for (register int i = 0; i < N_O_SMALL_BLOCKS; i++) {
        char *pointer_to_particular_place = (pointer_to_info_section + i * S_O_S_A_INFO_BLOCK);
        void **pointer_to_pointer = (void **)pointer_to_particular_place;
        (*pointer_to_pointer) = (pointer_to_memory_section + i * S_O_SMALL_BLOCK_MEMORY);
        int *pointer_to_taken_after = (int*)(pointer_to_particular_place + sizeof(void*));
        (*pointer_to_taken_after) = 0;
    }
    //

    //large allocations
    char *copy_of_MFLA = memory_for_large_allocations;
    char *pointer_to_block = copy_of_MFLA;
    for (register int i = 0; i < N_O_BIG_BLOCKS; i++) {
        char *pointer_to_current_block = (pointer_to_block + i * S_O_L_A_BLOCK);
        int *pointer_to_first_size_section = (int*)pointer_to_current_block;
        (*pointer_to_first_size_section) = S_O_BIG_BLOCK_MEMORY;
        int *pointer_to_second_size_section =
                (int*)(pointer_to_current_block + sizeof(int) + S_O_BIG_BLOCK_MEMORY);
        (*pointer_to_second_size_section) = S_O_BIG_BLOCK_MEMORY;
    }
    //
}

//Small allocations
void * S_A_find_free_block_of_apr_size(size_t size) {

    //calculating how many block are needed
    int n_o_blocks_needed = (int)(size / S_O_SMALL_BLOCK_MEMORY);
    if ((size % S_O_SMALL_BLOCK_MEMORY) != 0) {
        n_o_blocks_needed += 1;
    }
    //

    //searching
    char *local_memory_for_small_allocations = memory_for_small_allocations;
    register int i = 0;
    int available_counter = 0;
    char *info_section_of_block_that_will_be_returned = NULL;
    while(1) {
        char *current_info_section = (local_memory_for_small_allocations + i * S_O_S_A_INFO_BLOCK);
        if (NULL == info_section_of_block_that_will_be_returned) {
            info_section_of_block_that_will_be_returned = current_info_section;
        }
        char *taken_field = (current_info_section + sizeof(void*));
        int taken_value = *((int *)(taken_field));
        if (0 == taken_value) {
            available_counter += 1;
        } else {
            available_counter = 0;
            info_section_of_block_that_will_be_returned = NULL;
            i += (taken_value - 1); //hop over blocks that are surely taken
        }
        if (n_o_blocks_needed == available_counter) {
            break;
        }
        i += 1;
        if (i == N_O_SMALL_BLOCKS) {
            break;
        }
    }

    if (i == N_O_SMALL_BLOCKS) {
        return NULL; //needed number of memory blocks was not found
    } else {
        char * taken_filed = (info_section_of_block_that_will_be_returned + sizeof(void*));
        int *taken_field_actual = (int*)taken_filed;
        (*taken_field_actual) = n_o_blocks_needed;
        return info_section_of_block_that_will_be_returned;
    }
    //
} //returns pointer to info block

void * S_A_malloc(long long int size) {

    if (size > (N_O_SMALL_BLOCKS * S_O_SMALL_BLOCK_MEMORY)) {
        return NULL;
    }

    char *info_section = (char*)S_A_find_free_block_of_apr_size((size_t)size);
    if (NULL == info_section) {
        return NULL;
    } else {
        void *pointer_to_memory_block = *((void **)(info_section));
        return pointer_to_memory_block;
    }
} //returns pointer to memory

void S_A_free(char *ptr_to_info_block) {

    int *ptr_to_info_taken = (int *)(ptr_to_info_block + sizeof(void*));
    (*ptr_to_info_taken) = 0; //restore value back to 0
}
//

//Large allocations
int determine_further_blocks_taken(int size) {
    //assert(size < 0);
    if (size < 0) {
        return ((int)(((-1) * size) / S_O_BIG_BLOCK_MEMORY));
    } else {
        return ((int)(size / S_O_BIG_BLOCK_MEMORY));
    }
}

void * L_A_find_free_block_of_apr_size(size_t size) {

    //calculating how many blocks are needed
    int n_o_blocks_needed = (int)(size / S_O_BIG_BLOCK_MEMORY);
    if (0 != (size % S_O_BIG_BLOCK_MEMORY)) {
        n_o_blocks_needed += 1;
    }
    //

    //searching
    char *local_memory_for_large_allocations = memory_for_large_allocations;
    register int i = 0;
    int available_counter = 0;
    char *info_section_of_block_that_will_be_returned = NULL;
    while(1) {
        char *current_block = (local_memory_for_large_allocations + i * S_O_L_A_BLOCK);
        if (NULL == info_section_of_block_that_will_be_returned) {
            info_section_of_block_that_will_be_returned = current_block;
        }
        int current_size = *((int *)(current_block));
        if (current_size < 0) {
            available_counter = 0;
            info_section_of_block_that_will_be_returned = NULL;
            int further_blocks_taken = determine_further_blocks_taken(current_size);
            i += (further_blocks_taken - 1);
        } else {
            available_counter += 1;
        }
        if (n_o_blocks_needed == available_counter) {
            break;
        }
        i += 1;
        if (i == N_O_BIG_BLOCKS) {
            break;
        }
    }
    if (i == N_O_BIG_BLOCKS) {
        return NULL;
    } else {
        int new_size_negative = (int)((n_o_blocks_needed * S_O_BIG_BLOCK_MEMORY) * (-1));
        int *ptr_to_first_size = (int *)(info_section_of_block_that_will_be_returned);
        *ptr_to_first_size = new_size_negative;
        //int *ptr_so_second_size = (int *)(info_section_of_block_that_will_be_returned + sizeof(int) + S_O_BIG_BLOCK_MEMORY);
        int *ptr_so_second_size = (int *)(info_section_of_block_that_will_be_returned +
                                          ((n_o_blocks_needed - 1) * S_O_L_A_BLOCK) +
                                          sizeof(int) + S_O_BIG_BLOCK_MEMORY);
        *ptr_so_second_size = new_size_negative;
        return info_section_of_block_that_will_be_returned;
    }
    //
}

void * L_A_malloc(long long int size) {

    if (size > (N_O_BIG_BLOCKS * S_O_BIG_BLOCK_MEMORY)) {
        return NULL;
    }

    char *info_section = L_A_find_free_block_of_apr_size((size_t)size);
    if (NULL == info_section) {
        return NULL;
    } else {
        void *pointer_to_memory_block = (info_section + sizeof(int));
        return pointer_to_memory_block;
    }
}

void L_A_free(void *ptr_to_memory_block) {
    assert(NULL != ptr_to_memory_block);

    char *block_info_section = (ptr_to_memory_block - sizeof(int));
    int *ptr_to_size = (int *)(block_info_section);
    int size = *ptr_to_size;
    if (size > 0) {
        return;
    } //block is not taken, do nothing
    int further_blocks_taken = determine_further_blocks_taken(size);
    for (register int i = 0; i < further_blocks_taken; i++) {
        char *current_block_info = (block_info_section + i * S_O_L_A_BLOCK);
        int *ptr_to_first_size = (int *)current_block_info;
        *ptr_to_first_size = S_O_BIG_BLOCK_MEMORY;
        int *ptr_to_second_size = (int *)(current_block_info + sizeof(int) + S_O_BIG_BLOCK_MEMORY);
        *ptr_to_second_size = S_O_BIG_BLOCK_MEMORY;
    }
}
//

void * M_malloc(long long int size) {

    if (size <= 0) {
        return NULL;
    }

    if (size > (N_O_BIG_BLOCKS * S_O_BIG_BLOCK_MEMORY)) {
        return NULL;
    }

    if ((size >= ((N_O_SMALL_BLOCKS / 2) * S_O_SMALL_BLOCK_MEMORY)) || (size >= S_O_BIG_BLOCK_MEMORY)) {
        return L_A_malloc(size);
    } else {
        return S_A_malloc(size);
    }
}

void M_safe_free(void *ptr_to_memory) {

    if (NULL == ptr_to_memory) {
        return;
    }

    char *local_memory_for_small_allocations = memory_for_small_allocations;
    char *local_memory_for_large_allocations = memory_for_large_allocations;

    for (register int i = 0; i < N_O_SMALL_BLOCKS; i++) {
        char *current_info_block = (local_memory_for_small_allocations + i * S_O_S_A_INFO_BLOCK);
        void *ptr_to_memory_block = *(void**)(current_info_block);
        if (ptr_to_memory_block == ptr_to_memory) {
            S_A_free(current_info_block);
            return;
        }
    }

    for (register int i = 0; i < N_O_BIG_BLOCKS; i++) {
        char *current_block = (local_memory_for_large_allocations + i * S_O_L_A_BLOCK);
        char *actual_memory = (current_block + sizeof(int));
        void *ptr_to_memory_block = actual_memory;
        if (ptr_to_memory_block == ptr_to_memory) {
            L_A_free(ptr_to_memory);
            return;
        }
    }
    //if ptr_to_memory does not match any pointer to memory block - it is not pointing where it should to be freed
    //hence do nothing
}

void M_free(void *ptr_to_memory) {

    if (NULL == ptr_to_memory) {
        return;
    }

    char *local_memory_for_small_allocations = memory_for_small_allocations;

    for (register int i = 0; i < N_O_SMALL_BLOCKS; i++) {
        char *current_info_block = (local_memory_for_small_allocations + i * S_O_S_A_INFO_BLOCK);
        void *ptr_to_memory_block = *(void**)(current_info_block);
        if (ptr_to_memory_block == ptr_to_memory) {
            S_A_free(current_info_block);
            return;
        }
    }

    L_A_free(ptr_to_memory);
}

//----------------------------------------------------------------------------------------------------------------------
void visualise_small_memory() {

    printf("\nSmall memory looks a bit like this...");

    char *copy_of_MFSA = memory_for_small_allocations;
    char *pointer_to_info_section = copy_of_MFSA;
    char *pointer_to_memory_section = (copy_of_MFSA + (N_O_SMALL_BLOCKS * S_O_S_A_INFO_BLOCK));
    for (register int i = 0; i < N_O_SMALL_BLOCKS; i++) {
        printf("\n");
        char *pointer_to_particular_place = (pointer_to_info_section + i * S_O_S_A_INFO_BLOCK);
        void **pointer_to_pointer = (void **)pointer_to_particular_place;
        printf("pointer: [%zu]", (size_t)(*pointer_to_pointer));
        printf("(real one is [%zu])", (size_t)(pointer_to_memory_section + i * S_O_SMALL_BLOCK_MEMORY));
        int *pointer_to_taken_after = (int*)(pointer_to_particular_place + sizeof(void*));
        printf("; taken after: [%d]", *pointer_to_taken_after);
        printf("; data: {%zu%zu}", *((size_t*)(pointer_to_memory_section + i * S_O_SMALL_BLOCK_MEMORY)),
               *((size_t *)(pointer_to_memory_section + i * S_O_SMALL_BLOCK_MEMORY + sizeof(size_t))));
    }
    printf("\n");
}

void visualise_large_memory() {

    printf("\nLarge memory looks a bit like this...");

    char *copy_of_MFLA = memory_for_large_allocations;
    char *pointer_to_block = copy_of_MFLA;
    for (register int i = 0; i < N_O_BIG_BLOCKS; i++) {
        printf("\n");
        char *pointer_to_current_block = (pointer_to_block + i * S_O_L_A_BLOCK);
        int *pointer_to_first_size_section = (int*)pointer_to_current_block;
        printf("first size: [%d]", *pointer_to_first_size_section);
        printf("; data: |---data---|");
        int *pointer_to_second_size_section =
                (int*)(pointer_to_current_block + sizeof(int) + S_O_BIG_BLOCK_MEMORY);
        printf("; second size: [%d]", *pointer_to_second_size_section);
    }
    printf("\n");
}

void visualise_memory() {
    visualise_small_memory();
    visualise_large_memory();
}
//----------------------------------------------------------------------------------------------------------------------
