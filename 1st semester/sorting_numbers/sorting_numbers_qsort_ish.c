#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void quick_sort(void *start_of_arr, size_t size_of_arr, size_t size_of_var,
                int (*compare)(const void *, const void *), void (*swap)(void *, void *)) {
    assert((NULL != start_of_arr) && (NULL != compare) && (NULL != swap));
    char *c_start_of_arr = (char*)start_of_arr;
    size_t pivot_element = ((size_of_arr + 1) / 2) - 1; //position in an array
    size_t p_leftmost = pivot_element; //will be left side of middle section
    size_t p_rightmost = pivot_element; //will be right side of middle section
    size_t left = 0;
    size_t right = size_of_arr - 1;
    while (left < right) {
        int flag_left = 0;
        while(1) {
            while(1) {
                if (left >= p_leftmost) {
                    if (left == pivot_element) {
                        flag_left = 3;
                    }
                    else {
                        flag_left = 0;
                    }
                    break;
                }
                flag_left = compare(c_start_of_arr + (left * size_of_var),
                                    c_start_of_arr + (pivot_element * size_of_var)); //key * -1 so that it will return -1 if don't need to swap
                if (0 == flag_left) {
                    p_leftmost -= 1;
                    swap(c_start_of_arr + (left * size_of_var), c_start_of_arr + (p_leftmost * size_of_var));
                }
                else {
                    break;
                }
            } //to move elements equal to pivot to the middle/ after loop we know if left element is <>= pivot
            if (1 == flag_left) {
                left += 1;
            } //if no need to swap
            else {
                break;
            }
        } //after loop we stand on the new element or the element that will be swapped places with
        int flag_right = 0;
        while(1) {
            while(1) {
                if (right <= p_rightmost) {
                    if (right == pivot_element) {
                        flag_right = 3;
                    }
                    else {
                        flag_right = 0;
                    }
                    break;
                }
                flag_right = compare(c_start_of_arr + (right * size_of_var),
                                     c_start_of_arr + (pivot_element * size_of_var));
                if (0 == flag_right) {
                    p_rightmost += 1;
                    swap(c_start_of_arr + (right * size_of_var), c_start_of_arr + (p_rightmost * size_of_var));
                }
                else {
                    break;
                }
            } //to move elements equal to pivot to the middle/ after loop we know if right element is <>= pivot
            if (-1 == flag_right) {
                right -= 1;
            } //if no need to swap
            else {
                break;
            }
        } //after loop we stand on the new element or the element that will be swapped places with
        if ((2 >= (abs(flag_left) + abs(flag_right))) && (0 != (abs(flag_left) + abs(flag_right)))) {
            if (0 == flag_right) {
                p_rightmost -= 1;
            } //if the right reached middle section
            else if (0 == flag_left) {
                p_leftmost += 1;
            } //if the left reached middle section
            swap(c_start_of_arr + (left * size_of_var), c_start_of_arr + (right * size_of_var));
        } //one of the elements reached middle section, but not both
        else if (4 == (abs(flag_left) + abs(flag_right))) {
            if (3 == flag_right) {
                pivot_element = left;
            } //to change the position of pivot, in order to remember it's value, since we can't do that with a variable
            else {
                pivot_element = right;
            } //to change the position of pivot, in order to remember it's value, since we can't do that with a variable
            p_leftmost = pivot_element;
            p_rightmost = pivot_element;
            swap(c_start_of_arr + (left * size_of_var), c_start_of_arr + (right * size_of_var));
        } //one of the elements reached pivot, the other one is a number to swap (but not in middle section)
        else {
            break;
        } //either both reached middle section, or both reached pivot, need to stop
    } //after the loop we know the position of the middle section on both ends of array
    if (1 < p_leftmost) {
        quick_sort(c_start_of_arr, p_leftmost, size_of_var, compare, swap);
    } //there still are elements to sort on the left
    if (1 < (size_of_arr - p_rightmost - 1)) {
        quick_sort(c_start_of_arr + ((p_rightmost + 1) * size_of_var),
                   size_of_arr - (p_rightmost + 1), size_of_var, compare, swap);
    } //there still are elements to sort on the right
}
