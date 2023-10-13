#include "key_and_data.h"

//#include <stdio.h>
#include <assert.h>

#include "edges_and_nodes.h"

size_t hash_function(KEY_TYPE key) {

    return key;
}

int compare_keys(KEY_TYPE key1, KEY_TYPE key2) {
    if (key1 == key2) {
        return 1;
    } else {
        return 0;
    }
}

void free_data(void *data) {
    assert(NULL != data);

    struct node *cast_data = (struct node*)data;
    delete_node(cast_data);
}

void free_key(KEY_TYPE key) {
    //does nothing, memory for key is not allocated
}
