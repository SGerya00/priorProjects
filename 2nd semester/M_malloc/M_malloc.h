#ifndef M_MALLOC_M_MALLOC_H
#define M_MALLOC_M_MALLOC_H

//
void ask_for_memory_for_my_malloc();

void give_back_memory();

void prepare_memory_for_usage();
//

void * M_malloc(long long int size);

void M_free(void *ptr_to_memory);

//optional
void M_safe_free(void *ptr_to_memory);

void visualise_memory();
//

#endif //M_MALLOC_M_MALLOC_H
