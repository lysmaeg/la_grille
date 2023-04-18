#ifndef __TOOLS__
#define __TOOLS__
#include <iostream>
#include <cmath>

/// @brief calculate the length of an integer in print
/// @param n the number we want the length
/// @param current_max the current max, avoiding repeating old values
/// @return return the length as string of n
int size_of_int(int n);
int get_max_size_of_int_list(int **t, int size);

#endif