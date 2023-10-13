#ifndef INC_18_02_HASH_FUNCTIONS_H
#define INC_18_02_HASH_FUNCTIONS_H

void *give_hash_func(int i, size_t (*func_arr[])(const char *));

size_t hash_func1(const char *line);

size_t hash_func2(const char *line);

size_t hash_func3(const char *line);

size_t hash_func4(const char *line);

size_t hash_func5(const char *line);

#endif //INC_18_02_HASH_FUNCTIONS_H