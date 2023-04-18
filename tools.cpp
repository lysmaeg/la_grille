#include "tools.hpp"

/// @brief calculate the length of an integer in print
/// @param n the number we want the length
/// @param current_max the current max, avoiding repeating old values
/// @return return the length as string of n
int size_of_int(int n)
{
    int len = 0;

    if (n < 0) {
        len++;
        n *= -1;
    }

    while (n != 0)
    {
        len++;
        n /= 10;
    }
    // printf("current_max = %d, %d\n", current_max, current_max / 10);
    return len;
}

int get_max_size_of_int_list(int **t, int size)
{
    int max_size = 0, tmp = 0;
    for (int i = 0; i < size; i++)
    {
        for (int k = 0; k < size; k++)
        {
            tmp = size_of_int(t[i][k]);
            if (tmp > max_size) {
                max_size = tmp;
            }
        }
    }
    return max_size;
}