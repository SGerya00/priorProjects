#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define LOGGING_ON

#ifdef LOGGING_ON
    #define MY_LOG(...) fprintf(stdout, __VA_ARGS__)
    #define MY_LOG_FUNC(...) print_everything(__VA_ARGS__)
#else
    #define MY_LOG(...)
    #define MY_LOG_FUNC(...)

#endif


#define SIZE 10
#define MAX_WORD_LEN 25
#define MAX_WORD_LEN_FP "%25s"
#define ESC_SEQ "qqq"

struct cache_list {
    size_t key;
    char *value;
    struct cache_list *next;
    struct cache_list *prev;
};

struct cache_hash_arr {
    size_t key;
    struct cache_list *list_elem;
};

struct Cache {
    struct cache_list *(*cache_list_back);
    struct cache_list *(*cache_list_front);
    struct cache_hash_arr (*cache_arr)[SIZE];
};

void print_everything(struct Cache my_cache) {

    for (int i = 0; i < SIZE; i++) {
        if (((*my_cache.cache_arr)[i].list_elem == NULL) && ((*my_cache.cache_arr)[i].key == 0)) {continue;}
        printf("[[arr elem is %d, it's key is %zu, it points to \"%s\"]]\n",
               i, (*my_cache.cache_arr)[i].key, (*my_cache.cache_arr)[i].list_elem->value);
    }
    printf("\n[[Cache list from back to front is as follows:");
    struct cache_list *current = (*my_cache.cache_list_back);
    while(1) {
        if (NULL == current->next) {
            printf(" \"%s\" ]]\n", current->value);
            break;
        }
        printf(" \"%s\" -->", current->value);
        current = current->next;
    }
}

size_t hash_func(const char * sequence) {
    assert(NULL != sequence);

    MY_LOG("\n  for \"%s\" hash result is: ", sequence);

    size_t result = SIZE; //initial "front element" will have key equal to SIZE - 1, so,
    // in order to avoid allowing access to that element (->value == NULL), we will adapt
    size_t multiplier = 1;
    while(1) {
        if ('\0' == sequence[multiplier - 1]) {
            break;
        }
        result += (    ( (size_t)sequence[multiplier - 1] ) * multiplier    );
        multiplier += 1;
    }
    MY_LOG("%zu\n", result);
    return result;
} //check

void put_in_list_front(struct Cache my_cache, struct cache_list *current_elem) {
    assert(NULL != current_elem);

    if ((NULL == current_elem->next) && (NULL == current_elem->prev)) {
        MY_LOG("\n  inserting brand new element \"%s\" in list (making it list_front)\n", current_elem->value);
        current_elem->prev = (*my_cache.cache_list_front);
        (*my_cache.cache_list_front)->next = current_elem;
        (*my_cache.cache_list_front) = current_elem;
        return;
    } //not even in list
    if (current_elem == (*my_cache.cache_list_front)) {
        MY_LOG("\n[\"%s\" is already in front, don't move]\n", current_elem->value);
        MY_LOG("\n[list_front is now \"%s\"]\n", (*my_cache.cache_list_front)->value);
        return;
    } //already in front
    if (current_elem == (*my_cache.cache_list_back)) {
        MY_LOG("\n  moving \"%s\" from back to front\n", current_elem->value);
        (*my_cache.cache_list_back) = (*my_cache.cache_list_back)->next; //next after last is now last
        (*my_cache.cache_list_back)->prev = NULL;
        //move to front
        current_elem->next = NULL;
        (*my_cache.cache_list_front)->next = current_elem;
        current_elem->prev = (*my_cache.cache_list_front);
        (*my_cache.cache_list_front) = current_elem;
        //
        MY_LOG("\n  list_front is now \"%s\"\n", (*my_cache.cache_list_front)->value);
        return;
    }
    MY_LOG("\n[[  moving \"%s\" from middle (not back or front) to front  ]]\n", current_elem->value);
    //remove from list
    current_elem->prev->next = current_elem->next;
    current_elem->next->prev = current_elem->prev;
    //move to front
    (*my_cache.cache_list_front)->next = current_elem;
    current_elem->prev = (*my_cache.cache_list_front);
    current_elem->next = NULL;
    (*my_cache.cache_list_front) = current_elem;
    //
    MY_LOG("\n[[  list_front is now \"%s\"  ]]\n", (*my_cache.cache_list_front)->value);
}

//
char *get_from_cache(const struct Cache my_cache, size_t key) {

    char *for_return = NULL;
    size_t for_later = 0;
    MY_LOG("\ngetting element with key %zu from cache\n", key);

    for (size_t i = 0; i < SIZE; i++) {
        if ( (*my_cache.cache_arr)[i].key == key ) {
            for_return = (*(*my_cache.cache_arr)[i].list_elem).value;
            for_later = i;
            break;
        }
    }
    if (NULL == for_return) {
        MY_LOG("\nNot in cache, sry\n");
        return NULL;
    }
    put_in_list_front(my_cache, (*my_cache.cache_arr)[for_later].list_elem); //updating cache list
    MY_LOG("\nabout to return \"%s\"\n", for_return);
    return for_return;
}
//

//
void delete_list_back(struct cache_list *(*list_back)) {
    assert((NULL != list_back) && (NULL != (*list_back)));

    MY_LOG("\n  deleting last node, it is \"%s\"\n", (*list_back)->value);
    struct cache_list *temp_pointer = (*list_back);
    (*list_back)->next->prev = NULL;
    (*list_back) = (*list_back)->next;
    free(temp_pointer->value); //don't forget that value is also allocated memory
    free(temp_pointer); //only after value is deleted, otherwise portion of allocated memory lost
} //frees list_back (list element gone, completely)

size_t delete_from_cache(struct cache_hash_arr (*cache_arr)[], struct cache_list *(*list_back)) {
    assert((NULL != cache_arr) && (NULL != list_back) && (NULL != (*list_back)));

    size_t for_later = 0;
    MY_LOG("\n  ----  searching for array element with %zu key (last element)\n", (*list_back)->key);
    for (size_t i = 0; i < SIZE; i++) {
        if ( (*cache_arr)[i].key == (*list_back)->key) {
            for_later = i;
            break;
        }
    } //search for array element with that key

    delete_list_back(list_back); //frees allocated memory and changes ->next

    MY_LOG("\n  ----  updating array, emptying slot %zu and making it pointer to NULL\n", for_later);
    (*cache_arr)[for_later].key = 0;
    (*cache_arr)[for_later].list_elem = NULL;
    return for_later;
}
//

void put_in_cache(struct Cache my_cache, struct cache_list *new_element) {
    assert(NULL != new_element);


    MY_LOG("\n  checking if \"%s\" is in array", new_element->value);
    for (size_t i = 0; i < SIZE; i++) {
        if (new_element->key == (*my_cache.cache_arr)[i].key) {
            MY_LOG("\n  found it in array, slot number [%zu]", i);
            put_in_list_front(my_cache, (*my_cache.cache_arr)[i].list_elem);
            return;
        } //if keys match (in array)
    }

    //find free slot
    MY_LOG("\n  looking for a free slot in array\n");
    size_t for_later = SIZE;
    for (size_t i = 0; i < SIZE; i++) {
        if (0 == (*my_cache.cache_arr)[i].key) {
            for_later = i;
            MY_LOG("\n  found it, it is %zu\n", for_later);
            break;
        }
    } //for_later ---> free slot in cache array
    if (SIZE == for_later) {
        MY_LOG("\n  -- didn't find a free slot, deleting last element -> \"%s\"\n", (*my_cache.cache_list_back)->value); //!!good for print, doesn't use value == (null) anywhere else!!
        for_later = delete_from_cache(my_cache.cache_arr, my_cache.cache_list_back);
    } //no free slot, so delete last

    MY_LOG("\n  new free slot is %zu\n", for_later);
    (*my_cache.cache_arr)[for_later].key = new_element->key;
    (*my_cache.cache_arr)[for_later].list_elem = new_element;
    put_in_list_front(my_cache, (*my_cache.cache_arr)[for_later].list_elem);
}

//
void free_everything(struct Cache my_cache) {

    MY_LOG("\nfreeing everything\n");
    for (size_t i = 0; i < SIZE; i++) {
        if (NULL != (*my_cache.cache_arr)[i].list_elem) {
            if (NULL != (*my_cache.cache_arr)[i].list_elem->value) {
                MY_LOG("\nFreeing char sequence \"%s\"", (*my_cache.cache_arr)[i].list_elem->value);
                free((*my_cache.cache_arr)[i].list_elem->value); //remove char sequence that uses allocated memory
            }
            MY_LOG("\nFreeing corresponding element");
            free((*my_cache.cache_arr)[i].list_elem); //remove list element that uses allocated memory
        }
    }
    MY_LOG("\nFinally freeing array");
    free(my_cache.cache_arr); //free array that uses allocated memory
} //frees values, list_elements and cache_arr
//

int fill_cache_with_lines_from_file(FILE *file, struct Cache my_cache) {
    assert(NULL != file);

    fscanf(file, "%*[ \n]"); //initial skip (white-space)s
    while(1) {
        int eof_check = fgetc(file);
        if (EOF == eof_check) {
            fprintf(stdout, "\nFile is empty\n");
            return 1;
        } //end of file reached, stop reading
        fseek(file, -1, SEEK_CUR); // 1 step back because of fgetc

        MY_LOG("\nReading a new word");
        char *new_word = (char *)calloc(MAX_WORD_LEN + 1, sizeof(char)); //+1 for '\0'
        if (NULL == new_word) {
            fprintf(stderr, "\nUnable to allocate enough memory for a new word\n");
            return 1;
        }
        fscanf(file, MAX_WORD_LEN_FP, new_word);
        fscanf(file, "%*[^ \n\0]"); //skip rest of word (if rest exists)
        fscanf(file, "%*[ \n]"); //skip (white-space)s and (EOL)s

        MY_LOG(", it is \"%s\"\n", new_word);
        size_t hash_result = hash_func(new_word);
        MY_LOG("  it's hash is %zu\n", hash_result);
        struct cache_list *new_list_elem = (struct cache_list *)calloc(1, sizeof(struct cache_list));
        if (NULL == new_list_elem) {
            fprintf(stderr, "\nUnable to allocate enough memory for a cache list element\n");
            free(new_word);
            return 1;
        }
        //
        new_list_elem->key = hash_result;
        new_list_elem->value = new_word;
        new_list_elem->next = NULL;
        new_list_elem->prev = NULL;
        //
        MY_LOG("  putting it in cache\n");
        put_in_cache(my_cache, new_list_elem);

        eof_check = fgetc(file);
        if (EOF == eof_check) {
            break;
        } //end of file reached, stop reading
        fseek(file, -1, SEEK_CUR); // 1 step back because of fgetc
        MY_LOG_FUNC(my_cache);
    }
    return 0;
}

void initialize_cache_elements(struct cache_hash_arr (*cache_arr)[SIZE],
                               struct cache_list *back_elem, struct cache_list *front_elem) {
    assert((NULL != cache_arr) && (NULL != back_elem) && (NULL != front_elem));
    //
    back_elem->key = 1;
    back_elem->value = NULL;
    back_elem->next = front_elem;
    back_elem->prev = NULL;
    //
    front_elem->key = SIZE - 1;
    front_elem->value = NULL;
    front_elem->prev = back_elem;
    front_elem->next = NULL;
    //
    (*cache_arr)[0].key = back_elem->key;                             //so that
    (*cache_arr)[0].list_elem = back_elem;                            //cache has
    (*cache_arr)[SIZE - 1].key = front_elem->key;                     //2 initial "empty" elements
    (*cache_arr)[SIZE - 1].list_elem = front_elem;                    //for convenience
    //
}

size_t give_key() {
    char *key_c = (char*)calloc(MAX_WORD_LEN, sizeof(char));
    assert(NULL != key_c);
    printf("\nPlease, give key: ");
    scanf(MAX_WORD_LEN_FP, key_c);
    if (0 == strcmp(key_c, ESC_SEQ)) {
        return 0;
    }
    size_t hash_result_for_key = hash_func(key_c);
    return hash_result_for_key;
}

FILE *flag_check_input(int argc, char *argv[]) {
    if (2 != argc) {
        fprintf(stderr, "\nUnexpected number of arguments (%d), expected 2\n", argc);
        return NULL;
    }
    FILE *file = fopen(argv[1], "r");
    if (NULL == file) {
        fprintf(stderr, "\nUnable to open file %s\n", argv[1]);
        return NULL;
    }
    return file;
} //file is opened, please close

int main(int argc, char *argv[]) {
    FILE *file = flag_check_input(argc, argv);
    if (NULL == file) {
        return 0;
    }
    struct cache_hash_arr (*cache_arr)[SIZE] =
            (struct cache_hash_arr (*)[SIZE])calloc(SIZE, sizeof(struct cache_hash_arr));
    if (NULL == cache_arr) {
        fprintf(stderr, "\nUnable to allocate enough memory for cache array\n");
        fclose(file);
        return 0;
    }
    struct cache_list *back_elem = (struct cache_list *)calloc(1, sizeof(struct cache_list));
    struct cache_list *front_elem = (struct cache_list *)calloc(1, sizeof(struct cache_list));
    if ((NULL == back_elem) || (NULL == front_elem)) {
        fprintf(stderr, "\nUnable to allocate enough memory for first two elements of cache\n");
        if (NULL == front_elem) {
            free(back_elem);
        } //enough for back, not enough for front
        free(cache_arr);
        fclose(file);
        return 0;
    }
    initialize_cache_elements(cache_arr, back_elem, front_elem);
    struct Cache my_cache = {0};
    my_cache.cache_list_back = &back_elem;
    my_cache.cache_list_front = &front_elem;
    my_cache.cache_arr = cache_arr;

    int flag = fill_cache_with_lines_from_file(file, my_cache); //allocates memory
    if (1 == flag) {
        free_everything(my_cache);
        fclose(file);
        return 0;
    }
    MY_LOG_FUNC(my_cache);
    while(1) {
        printf("\nuse \""ESC_SEQ"\"to exit\n");
        size_t your_key = give_key();
        if (0 == your_key) {
            break;
        }
        char *getting_from_cache = get_from_cache(my_cache, your_key);
        if (NULL != getting_from_cache) {
            printf("\n\n  ----> With your key \"%s\" was received from cache <----  \n\n", getting_from_cache);
        } else {
            printf("\nNo element in cache matching given key\n");
        }
        MY_LOG_FUNC(my_cache);
    }
    free_everything(my_cache);
    fclose(file);
    return 0;
}
