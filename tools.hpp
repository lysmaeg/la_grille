#ifndef __TOOLS__
#define __TOOLS__
#include <iostream>
#include <cmath>
#include <string>

#define NB_MAX_THREAD -1

struct couple
{
    int line;
    int column;
};

struct coupleLink
{
    couple c = couple{-1, -1};
    coupleLink *next = nullptr;
};

coupleLink *add_first_CL(coupleLink *cl, int line, int column);
bool contains_CL(const coupleLink *cl, int line, int column);

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

bool are_equal_ttab(const int **t, const int **u, int s1, int s2);
bool are_equal_tab(const int *t, const int *u, int s);

bool are_equal_ttab_char(const char **t, const char **u, int s1, int s2);
bool are_equal_tab_char(const char *t, const char *u, int s);

#endif