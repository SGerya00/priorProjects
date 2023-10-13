#include "HT_v2_075.h"

//#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//#include "key_and_data.h"

struct HT_element {
    KEY_TYPE key;
    void *data;
};

#define EXPANSION_FACTOR ((float)0.75) //should be (0 < EXPANSION_FACTOR < 1)
#define INITIAL_CAPACITY ((size_t)16)

struct HT {
    size_t capacity;
    size_t occupied;
    struct HT_element *(*array)[];
};

//get smth specific from HT_element
KEY_TYPE get_HTE_key(struct HT_element *const element) {
    assert(NULL != element);

    return element->key;
}

void *get_HTE_data(struct HT_element *const element) {
    assert(NULL != element);

    return element->data;
}
//

//put smth in HT_element
void cast_HTE_key(struct HT_element *const element, KEY_TYPE key) {
    assert(NULL != element);

    element->key = key;
}

void cast_HTE_data(struct HT_element *const element, void *data) {
    assert(NULL != element);
    assert(NULL != data);

    element->data = data;
}
//

//delete element
void free_HTE(struct HT_element *const element) {
    assert(NULL != element);

    free(element);
}

void delete_HTE(struct HT_element *const element) {
    assert(NULL != element);

    KEY_TYPE key = get_HTE_key(element);
    free_key(key);
    void *data = get_HTE_data(element);
    free_data(data);
    free_HTE(element);
}
//

//get smth from hash_table
size_t get_HT_capacity(struct HT *const hash_table) {
    assert(NULL != hash_table);

    return hash_table->capacity;
}

size_t get_HT_occupied(struct HT *const hash_table) {
    assert(NULL != hash_table);

    return hash_table->occupied;
}

struct HT_element *(* get_HT_array(struct HT *const hash_table))[] {
    assert(NULL != hash_table);

    return hash_table->array;
} //returns pointer to array

struct HT_element * get_HT_element(struct HT *const hash_table, const size_t index) {
    assert(NULL != hash_table);

    struct HT_element *(*array)[] = get_HT_array(hash_table);
    struct HT_element *element = (*array)[index];
    return element;
} //may return NULL if corresponding slot contains NULL
//

//
int slot_is_empty(struct HT *const hash_table, size_t index) {
    assert(NULL != hash_table);

    if (NULL == get_HT_element(hash_table, index)) {
        return 1;
    } else {
        return 0;
    }
} //1 if yes, 0 if no

int need_expanding(struct HT *const hash_table) {
    assert(NULL != hash_table);

    float result = ( ((float)(get_HT_occupied(hash_table))) / ((float)(get_HT_capacity(hash_table))) );
    if (result >= EXPANSION_FACTOR) {
        return 1;
    } else {
        return 0;
    }
} //returns 1 if occupied/capacity ratio exceeds EXPANSION_FACTOR, 0 otherwise

size_t calculate_index(struct HT *const hash_table, size_t hash_result) {
    assert(NULL != hash_table);

    size_t index = ( hash_result % (get_HT_capacity(hash_table)) );
    return index;
}
//

//delete HT
void free_HT_array(struct HT *const hash_table) {
    assert(NULL != hash_table);

    struct HT_element *(*array)[] = get_HT_array(hash_table);
    free(array);
}

void free_HT(struct HT *const hash_table) {
    assert(NULL != hash_table);

    free(hash_table);
}

void delete_HT(struct HT *const hash_table) {
    assert(NULL != hash_table);

    size_t capacity = get_HT_capacity(hash_table);
    //free memory allocated for elements
    for (register size_t index = 0; index < capacity; index++) {
        if (slot_is_empty(hash_table, index)) {
            continue;
        } else {
            struct HT_element *element = get_HT_element(hash_table, index);
            //element's contents
            free_key(get_HTE_key(element));
            free_data((get_HTE_data(element)));
            //
            //element itself
            free_HTE(element);
            //
        }
    }
    //

    //free memory allocated for array
    free_HT_array(hash_table);
    //

    //free memory allocated for element of struct HT
    free_HT(hash_table);
    //
}
//

void set_HT_capacity(struct HT *const hash_table, size_t capacity) {
    assert(NULL != hash_table);

    hash_table->capacity = capacity;
}

void increase_HT_occupied(struct HT *const hash_table) {
    assert(NULL != hash_table);

    hash_table->occupied += 1;
}

int check_if_keys_are_same(struct HT_element *const element1, struct HT_element *const element2) {
    assert(NULL != element1);
    assert(NULL != element2);

    KEY_TYPE key1 = get_HTE_key(element1);
    KEY_TYPE key2 = get_HTE_key(element2);

    if (1 == compare_keys(key1, key2)) {
        return 1;
    } else {
        return 0;
    }
} //1 if yes, 0 if no

void replace_HT_element(struct HT *const hash_table, const size_t index, struct HT_element *const element) {
    assert(NULL != hash_table);
    assert(NULL != element);

    struct HT_element *temp = get_HT_element(hash_table, index);
    struct HT_element *(*array)[] = get_HT_array(hash_table);
    (*array)[index] = element;
    delete_HTE(temp);
}
//if element that needs inserting has the same key as another element in hash table, already existing one gets replaced

void insert_in_HT(struct HT *const hash_table, struct HT_element *const for_insertion, const size_t index) {
    assert(NULL != hash_table);
    assert(NULL != for_insertion);

    //(*HT_array)[index] = for_insertion;
    struct HT_element *(*const array)[] = get_HT_array(hash_table);
    //check slot itself
    if (slot_is_empty(hash_table, index)) {
        (*array)[index] = for_insertion; //insert
        increase_HT_occupied(hash_table);
        return;
    } else {
        struct HT_element *element = get_HT_element(hash_table, index);
        if (check_if_keys_are_same(element, for_insertion)) {
            replace_HT_element(hash_table, index, for_insertion);
            return;
        }
    }
    //check from index to right
    size_t capacity = get_HT_capacity(hash_table);
    for (register size_t i = (index + 1); i < capacity; i++) {
        if (slot_is_empty(hash_table, i)) {
            (*array)[i] = for_insertion; //insert
            increase_HT_occupied(hash_table);
            return;
        } else {
            struct HT_element *element = get_HT_element(hash_table, i);
            if (check_if_keys_are_same(element, for_insertion)) {
                replace_HT_element(hash_table, i, for_insertion);
                return;
            }
        }
    }
    //if reached this section, it means element is still not inserted, check from left to index
    for (register size_t i = 0; i < index; i++) {
        if (slot_is_empty(hash_table, i)) {
            (*array)[i] = for_insertion; //insert
            increase_HT_occupied(hash_table);
            return;
        } else {
            struct HT_element *element = get_HT_element(hash_table, i);
            if (check_if_keys_are_same(element, for_insertion)) {
                replace_HT_element(hash_table, i, for_insertion);
                return;
            }
        }
    }
    assert(NULL);
}
//linear probing // returns index of now occupied HT slot

void cast_array_to_HT(struct HT *const hash_table, struct HT_element *(*const array)[]) {
    assert(NULL != hash_table);
    assert(NULL != array);

    hash_table->array = array;
}

size_t calculate_new_index(const size_t old_index, struct HT *const hash_table, const size_t new_capacity) {
    assert(NULL != hash_table);

    struct HT_element *element = get_HT_element(hash_table, old_index);
    KEY_TYPE key = get_HTE_key(element);
    size_t hash_result = hash_function(key);
    size_t new_index = ( hash_result % new_capacity );
    return new_index;
}

int expand_HT(struct HT *hash_table) {
    assert(NULL != hash_table);

    size_t new_capacity = (get_HT_capacity(hash_table)) * 2;
    struct HT_element *(*new_array)[] = (struct HT_element *(*)[])calloc(new_capacity, sizeof(struct HT_element *));
    if (NULL == new_array) {
        return 0;
    }
    //using temporary HT because other functions (insert_in_HT and delete_HT_array) require a HT and not array
    struct HT fictional_HT;
    fictional_HT.array = new_array;
    fictional_HT.capacity = new_capacity; //here
    fictional_HT.occupied = get_HT_occupied(hash_table); //here
    //

    //fill new_array with elements from old array
    size_t capacity = get_HT_capacity(hash_table);
    for (register size_t index = 0; index < capacity; index++) {
        if (slot_is_empty(hash_table, index)) {
            continue; //skip empty ones
        }
        size_t new_index = calculate_new_index(index, hash_table, new_capacity);
        struct HT_element *for_insertion = get_HT_element(hash_table, index);
        insert_in_HT(&fictional_HT, for_insertion, new_index);
    }
    //

    //remember old_array for free()
    struct HT_element *(*old_array)[] = get_HT_array(hash_table);
    //
    //modify hash_table
    cast_array_to_HT(hash_table, new_array);
    set_HT_capacity(hash_table, new_capacity);
    /* occupied remains the same */
    //free memory
    fictional_HT.array = old_array;
    free_HT_array(&fictional_HT);
    //
    return 1;
} //returns 0 if could not allocate enough memory to expand, 1 if succeeded

//
void put_in_HT(struct HT *const hash_table, KEY_TYPE key, void *data) {
    assert(NULL != data);

    struct HT_element *const element = (struct HT_element *)calloc(1, sizeof(struct HT_element));
    assert(NULL != element);
    cast_HTE_key(element, key);
    cast_HTE_data(element, data);

    size_t hash_result = hash_function(key);
    size_t index = calculate_index(hash_table, hash_result);
    insert_in_HT(hash_table, element, index);
    //check if HT needs to be expanded
    if (1 == need_expanding(hash_table)) {
        expand_HT(hash_table);
    }
    //and expand if needed
}
//creates a new HT_element, casts key and data to it, inserts in HT, returns index of now occupied HT slot

void * get_element_from_HT(struct HT *const hash_table, size_t index) {
    assert(NULL != hash_table);

    struct HT_element *(*array)[] = get_HT_array(hash_table);
    if (slot_is_empty(hash_table, index)) {
        return NULL;
    }
    void * data = ((*array)[index])->data;
    return data;
}
//similar to search_in_HT, but only checks index slot

void * search_in_HT(struct HT *const hash_table, KEY_TYPE key) {
    assert(NULL != hash_table);

    size_t hash_result = hash_function(key);
    size_t index = calculate_index(hash_table, hash_result);

    if (slot_is_empty(hash_table, index)) {
        return NULL;
    } //if slot with index is empty, element with matching key is definitely not in hash table
    struct HT_element *element = get_HT_element(hash_table, index);
    KEY_TYPE key1 = get_HTE_key(element);
    KEY_TYPE key2 = key;
    if (1 == compare_keys(key1, key2)) {
        return (get_HTE_data(element));
    } else {
        //search from index to right
        size_t capacity = get_HT_capacity(hash_table);
        for (register size_t i = (index + 1); i < capacity; i++) {
            if (slot_is_empty(hash_table, i)) {
                continue;
            }
            element = get_HT_element(hash_table, i);
            key1 = get_HTE_key(element);
            //key2 = key; //it stays the same
            if (1 == compare_keys(key1, key2)) {
                return (get_HTE_data(element));
            }
        }
        //if managed to reach this loop, means did not find element with matching key yet
        //search from left to index
        for (register size_t i = 0; i < index; i++) {
            if (slot_is_empty(hash_table, i)) {
                continue;
            }
            element = get_HT_element(hash_table, i);
            key1 = get_HTE_key(element);
            //key2 = key; //it stays the same
            if (1 == compare_keys(key1, key2)) {
                return (get_HTE_data(element));
            }
        }
        //if managed to get here, means did not find element with matching key in hash table
        return NULL;
    }
}
//returns pointer to .data of an element with matching key if it exists, NULL otherwise
//

struct HT * create_HT() {

    struct HT *hash_table = (struct HT *)calloc(1, sizeof(struct HT));
    if (NULL == hash_table) {
        return NULL;
    }
    set_HT_capacity(hash_table, INITIAL_CAPACITY);
    struct HT_element *(*HT_array)[] = (struct HT_element *(*)[])calloc(INITIAL_CAPACITY, sizeof(struct HT_element *));
    if (NULL == HT_array) {
        free_HT(hash_table);
        return NULL;
    }
    cast_array_to_HT(hash_table, HT_array);
    return hash_table;
} //returns pointer to hash_table, NULL if could not allocate enough memory
