#ifndef SORTING_NUMBERS_QSORT_ISH_H
#define SORTING_NUMBERS_QSORT_ISH_H

void quick_sort(void *start_of_arr, int size_of_arr, int size_of_var,
                int (*compare)(const void *, const void *), void (*swap)(void *, void *));

#endif //SORTING_NUMBERS_QSORT_ISH_H
