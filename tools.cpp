#include "tools.hpp"

/// @brief calculate the length of an integer in print
/// @param n the number we want the length
/// @param current_max the current max, avoiding repeating old values
/// @return return the length as string of n
int size_of_int(int n)
{
    int len = 0;

    if (n < 0)
    {
        len++;
        n *= -1;
    }
    else if (n == 0)
    {
        return 1;
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
            if (tmp > max_size)
            {
                max_size = tmp;
            }
        }
    }
    return max_size;
}

/// @brief
/// @param s
/// @param c
/// @return if add in string true returned
bool add_to_string(std::string *&s, char c)
{
    if (c != 0)
    {
        s[0] += c;
        return true;
    }
    return false;
}

int generate_random_number(int mi, int ma)
{
    return (rand() % abs(ma - mi + 1)) + mi;
}

int *copy_tab(const int *t1, int s1, int *s2)
{
    *s2 = s1;
    int *t = new int[s1];
    for (int i = 0; i < s1; i++)
    {
        t[i] = t1[i];
    }
    return t;
}

int **copy_ttab(const int **t1, int s1_0, int s1_1, int *s2_0, int *s2_1)
{
    int **t = new int *[s1_0];
    *s2_0 = s1_0;
    for (int i = 0; i < s1_0; i++)
    {
        t[i] = copy_tab(t1[i], s1_1, s2_1);
    }
    return t;
}

char *copy_tab_char(const char *t1, int s1, int *s2)
{
    *s2 = s1;
    char *t = new char[s1];
    for (int i = 0; i < s1; i++)
    {
        t[i] = t1[i];
    }
    return t;
}

char **copy_ttab_char(const char **t1, int s1_0, int s1_1, int *s2_0, int *s2_1)
{
    char **t = new char *[s1_0];
    *s2_0 = s1_0;
    for (int i = 0; i < s1_0; i++)
    {
        t[i] = copy_tab_char(t1[i], s1_1, s2_1);
    }
    return t;
}

void print_tab(const int *t, int s)
{
    std::cout << "[";
    for (int i = 0; i < s; i++)
    {
        std::cout << t[i] << " ";
    }
    std::cout << "]";
}
void print_ttab(const int **t, int s1, int s2)
{
    std::cout << "[";
    for (int i = 0; i < s1 - 1; i++)
    {
        print_tab(t[i], s2);
        std::cout << ",\n";
    }
    print_tab(t[s2 - 1], s2);
    std::cout << "]\n";
}
void print_tttab(const int ***t, int s1, int s2, int s3)
{
    std::cout << "[";
    for (int i = 0; i < s1; i++)
    {
        print_ttab(t[i], s2, s3);
        std::cout << "\n";
    }
}