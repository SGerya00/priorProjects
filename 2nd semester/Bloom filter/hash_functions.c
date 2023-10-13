#include <stdio.h>
#include <string.h>

void *give_hash_func(int i, size_t (*func_arr[])(const char *)) {
    return func_arr[i];
}

size_t hash_func1(const char *line) {
    size_t length = strlen(line);
    size_t result = 13; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    for (size_t i = 0; i < length; i++) {
        if ('\n' != line[i]) {
            result += (size_t)((unsigned int)(line[i]));
        }
    }
    return result;
}

size_t hash_func2(const char *line) {
    size_t length = strlen(line);
    size_t result = 37; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    for (size_t i = 0; i < length; i++) {
        if ('\n' != line[i]) {
            result += (size_t)((unsigned int)(line[i]));
        }
    }
    return result;
}

size_t hash_func3(const char *line) {
    size_t length = strlen(line);
    size_t result = 44; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    for (size_t i = 0; i < length; i++) {
        if ('\n' != line[i]) {
            result += (size_t)((unsigned int)(line[i]));
        }
    }
    return result;
}

size_t hash_func4(const char *line) {
    size_t length = strlen(line);
    size_t result = 11; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    for (size_t i = 0; i < length; i++) {
        if ('\n' != line[i]) {
            result += (size_t)((unsigned int)(line[i]));
        }
    }
    return result;
}

size_t hash_func5(const char *line) {
    size_t length = strlen(line);
    size_t result = 2; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    for (size_t i = 0; i < length; i++) {
        if ('\n' != line[i]) {
            result += (size_t)((unsigned int)(line[i]));
        }
    }
    return result;
}
