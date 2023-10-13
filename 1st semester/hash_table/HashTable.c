#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define STARTING_VALUE 8
#define NAME_MAX_LENGTH 51 //+1 is for '\0'

struct hash_table_element {
    char name[NAME_MAX_LENGTH];
    int height;
    int weight;
};

struct hash_table {
    size_t already_taken;
    size_t capacity;
    struct hash_table_element *(*elements)[];
};

size_t hash_function(const char student_names[], const size_t arr_capacity) {
    assert(NULL != student_names);
    size_t result = 0;
    size_t temp_i = 0;
    while ('\0' != student_names[temp_i]) {
        result += (int)(student_names[temp_i]);
        temp_i += 1;
    }
    return (result % arr_capacity);
} //returns hash (position in array)

int insert_in_hash_table_body(struct hash_table_element *new_element,
                              const size_t capacity, size_t position, struct hash_table_element *(*table_body)[]) {
    assert((NULL != new_element) && (NULL != table_body));
    while (position < capacity) {
        if (NULL != (*table_body)[position]) {
            if (0 == (strcmp(new_element->name, (*table_body)[position]->name))) {
                return 1; //do nothing, element is already in table
            } //if keys are identical
            else {
                position += 1; //going further, looking for a free spot
            }
        } //if slot is not empty
        else {
            (*table_body)[position] = new_element; //insertion
            return 0;
        }
    }
    return -1; //need to expand
} //-1 if need to expand, 1 if element with same key already exists, 0 if inserted

int expand_hash_table(struct hash_table *table) {
    assert(NULL != table);
    struct hash_table_element *(*new_table_body)[(table->capacity) * 2] = (struct hash_table_element *(*)[])calloc((table->capacity) * 2, sizeof(struct hash_table_element *));
    if (NULL == new_table_body) {
        fprintf(stderr, "\nUnable to allocate enough memory for hash table expansion\n");
        return -1;
    }
    for (size_t i = 0; i < table->capacity; i++) {
        if (NULL != (*(table->elements))[i]) {
            size_t hash_for_new_table = hash_function((*(table->elements))[i]->name, (table->capacity) * 2);
            insert_in_hash_table_body((*(table->elements))[i], (table->capacity) * 2,
                                      hash_for_new_table, new_table_body); //cannot return anything, but 0
        }
        else {
            continue; //if nothing - don't need to rehash
        }
    }
    free(table->elements);                     //freeing ol body
    table->elements = new_table_body;          //casting new one
    table->capacity = (table->capacity * 2);
    return 0;
} //-1 if not enough memory

int add_to_hash_table(const char *key, const size_t key_size, int first_parameter,
                      int second_parameter, struct hash_table *table) {
    assert((NULL != key) && (NULL != table));
    size_t hash = hash_function(key, table->capacity);
    struct hash_table_element *new_element = (struct hash_table_element *)malloc(sizeof(struct hash_table_element)); //!!memory allocated, pls free
    if (NULL == new_element) {
        fprintf(stderr, "\nUnable to allocate enough memory for a new element in hash table\n");
        return -1;
    }
    memcpy(new_element->name, key, key_size + 1); //put key (+1 for '\0' to be memcpyd as well
    new_element->height = first_parameter; //put first parameter
    new_element->weight = second_parameter; //put second parameter
    int flag = insert_in_hash_table_body(new_element, table->capacity, hash, table->elements);
    if (-1 == flag) {
        free(new_element); //don't worry, we will not forget about current element
        return 1; //need to expand
    }
    else if (1 == flag) {
        free(new_element);
        return 2;
    } //element already in table
    return 0;
} //!!memory was allocated, pls free in loop (-1 if not enough memory, 1 if need to expand, 2 if already in table, 0 if good) (if not 0 then deallocates memory)

int fill_hash_table_sub(FILE *input_file, struct hash_table *table, const size_t current_line) {
    assert((NULL != input_file) && (NULL != table));
    char buffer[NAME_MAX_LENGTH] = {0};
    size_t counter = 0;
    int current_element = 0; //because fgetc returns int
    while (' ' != (char)(current_element = fgetc(input_file))) {
        if ('\n' == (char)current_element) {
            fprintf(stderr, "\nEncountered end of line in line %zu where key is supposed to be\n", current_line);
            return -1;
        }
        else if (EOF == current_element) {
            fprintf(stderr, "\nEncountered EOF in line %zu, parameters for key are absent, 2 expected\n", current_line);
            return -1;
        }
        if (counter == (NAME_MAX_LENGTH - 1)) {
            fprintf(stderr, "\nIn line %zu, name is too long, only "\
                            "using first %d symbols for hash\n", current_line, NAME_MAX_LENGTH - 1);
            counter += 1;
            continue;
        }
        else if (counter >= (NAME_MAX_LENGTH - 1)) {
            continue;
        }
        else {
            buffer[counter] = (char)current_element;
            counter += 1;
        }
    } //getting name
    if (counter > (NAME_MAX_LENGTH - 1)) {
        counter = NAME_MAX_LENGTH - 1;
    }
    buffer[counter] = '\0';
    if ('\0' == buffer[0]) {
        fprintf(stderr, "\nIn line %zu name is absent, no name = bad key\n", current_line);
        return -1;
    }
    int first_parameter = 0;
    int second_parameter = 0;
    if (2 != fscanf(input_file, "%d %d", &first_parameter, &second_parameter)) {
        fprintf(stderr, "\nUnable to read 2 parameters from line %zu\n", current_line);
        return -1;
    } //getting parameters
    while (1) {
        int flag = add_to_hash_table(buffer, strlen(buffer), first_parameter, second_parameter, table);
        if (-1 == flag) {
            return -2;
        } //need to free table, not enough memory
        else if (1 == flag) { //need to expand
            if (-1 == expand_hash_table(table)) {
                return -2;
            } //if not enough memory for expanded hash table
            else {
                continue; //so that after expansion we don't forget to insert element
            }
        }
        else if (2 == flag) {
            return 0;
        } //element already in table
        else {
            break;
        } //if flag == 0 (if element is added to hash table)
    } //after loop new element is supposed to be either added to hash table or not if already in
    table->already_taken = table->already_taken + 1; //if we reached this point, that means one less free slot in hash table
    return 0;
} //reads 1 line from input file, tries to put it in hash table (-1 if incorrect line composition, -2 if not enough memory, 0 if good)

int fill_hash_table(FILE *input_file, struct hash_table *table) {
    assert((NULL != input_file) && (NULL != table));
    size_t line_counter = 0;
    while (1) {
        line_counter += 1;
        int flag = fill_hash_table_sub(input_file, table, line_counter);
        if ((-1 == flag) || (-2 == flag)) {
            return -1;
        } //incorrect line composition or not enough memory
        else {
            int last_char = fgetc(input_file);
            if ('\n' == (char)last_char) {
                if (((table->capacity) - (table->already_taken)) < ((table->capacity) / 4)) {
                    if (-1 == expand_hash_table(table)) {
                        return -1;
                    } //not enough memory
                } //need to expand, less than 25% left
                continue; //move on to next line in file
            }
            else if (EOF == (char)last_char) {
                break;
            } //nothing else in file
            else {
                fprintf(stderr, "\nEncountered unexpected symbol \"%c\" in line %zu\n", (char)last_char, line_counter);
                return -1;
            }
        }
    }
    return 0; //all went well
} //-1 if bad (not enough memory or incorrect line file composition)

int search_in_hash_table(const struct hash_table *table, const char *key) {
    assert((NULL != table) && (NULL != key));
    size_t hash_for_key = hash_function(key, table->capacity);
    while(hash_for_key < table->capacity) {
        if (NULL == (*(table->elements))[hash_for_key]) {
            printf("\nElement with key \"%s\" doesn't exist\n", key);
            return -1;
        }
        else if (0 == strcmp(((*(table->elements))[hash_for_key]->name), key)) {
            printf("\nElement is found, key is %s, first parameter is %d, second parameter is %d\n",
                   ((*(table->elements))[hash_for_key]->name),
                   ((*(table->elements))[hash_for_key]->height),
                   ((*(table->elements))[hash_for_key]->weight));
            return 0;
        }
        else {
            hash_for_key += 1;
        }

    }
    printf("\nElement with key \"%s\" doesn't exist\n", key);
    return -1;
} //-1 if doesn't exist, 0 if does

void delete_entire_hash(struct hash_table *table) {
    assert(NULL != table);
    for (size_t i = 0; i < table->capacity; i++) {
        if (NULL != (*(table->elements))[i]) {
            free((*(table->elements))[i]);
        } //if not NULL then free
    } //free pointers from array of pointers
    free(table->elements); //free array of pointers
    free(table); //free hash table
}

int check_arguments(int argc, char *argv[]) {
    assert(NULL != argv);
    if (2 != argc) {
        fprintf(stderr, "\nUnexpected number of arguments, expected 2\n");
        return -1;
    }
    FILE *input_file = fopen(argv[1], "r");
    if (NULL == input_file) {
        fprintf(stderr, "\nUnable to open file %s for reading\n", argv[1]);
        return -1;
    }
    fclose(input_file);
    return 0;
} //-1 if bad, 0 if good

int main(int argc, char *argv[]) {
    assert(NULL != argv);
    if (-1 == check_arguments(argc, argv)) {
        return -1;
    }
    //!!creating initial hash_table
    struct hash_table_element *(*table_body)[STARTING_VALUE] = (struct hash_table_element *(*)[])calloc\
                                                               (STARTING_VALUE, sizeof(struct hash_table_element *)); //pointer to array of pointers to hash_table_element
    if (NULL == table_body) {
        fprintf(stderr, "\nUnable to allocate enough memory for hash table body\n");
        return -1;
    }
    struct hash_table *hash_table = (struct hash_table *)malloc(sizeof(struct hash_table));
    if (NULL == hash_table) {
        fprintf(stderr, "\nUnable to allocate enough memory for hash table\n");
        free(table_body);
        return -1;
    }
    hash_table->elements = table_body;
    hash_table->capacity = STARTING_VALUE;
    hash_table->already_taken = 0;
    FILE *input_file = fopen(argv[1], "r");
    if (NULL == input_file) {
        fprintf(stderr, "\nUnable to open file %s in *main*\n", argv[1]);
        free(hash_table);
        free(table_body);
        return -1;
    } //!!creation is over
    int flag = fill_hash_table(input_file, hash_table);
    if (-1 == flag) {
        delete_entire_hash(hash_table); //deletes body and table as well as everything from body
        fclose(input_file);
        return -1;
    }
    fclose(input_file); //don't need that anymore
    while (1) {
        char key_value[NAME_MAX_LENGTH] = {0};
        printf("\nWho do you want to find? (\"qqq\" to exit) ----> ");
        if (1 != scanf("%50s", key_value)) { //NAME_MAX_LENGTH
            fprintf(stderr, "\nUnable to read your key\n");
        }
        key_value[NAME_MAX_LENGTH - 1] = '\0';
        if (0 == strcmp(key_value, "qqq")) {
            break;
        } //exit
        search_in_hash_table(hash_table, key_value);
    }
    delete_entire_hash(hash_table); //deletes body and table as well as everything from body
    return 0;
}
