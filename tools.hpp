#ifndef __TOOLS__
#define __TOOLS__
#include <iostream>
#include <cmath>
#include <string>

struct couple {
    int line;
    int column;
};

int size_of_int(int n);
int get_max_size_of_int_list(int **t, int size);
bool add_to_string(std::string *&s, char c);
int generate_random_number(int mi, int ma);

int *copy_tab(const int *t1, int s1, int *s2);
int **copy_ttab(const int **t1, int s1_0, int s1_1, int *s2_0, int *s2_1);
char *copy_tab_char(const char *t1, int s1, int *s2);
char **copy_ttab_char(const char **t1, int s1_0, int s1_1, int *s2_0, int *s2_1);

void print_tab(const int *t, int s);
void print_ttab(const int **t, int s1, int s2);
void print_tttab(const int ***t, int s1, int s2, int s3);

#endif