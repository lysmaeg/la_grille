#ifndef __GRID__
#define __GRID__
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include "tools.hpp"

class Grid
{
private:
    int size;
    int penality;
    int supposed_score = 0;
    int score = 0;

public:
    int **numbers;
    char **colors;

    Grid(int size, int penalite);
    Grid(std::string filename, std::string extension, bool test);
    ~Grid();

    void init_tables();

    int get_size() const;
    int get_penality() const;
    int get_supposed_score() const;

    char *colors_to_string();
    char get_color(int line, int column) const;
    int get_number(int line, int column) const;
    int calcul_score() const;
    void print_numbers() const;
    void print_colors();

    int read_file_for_numbers(std::string filename);
    int read_file_for_colors(std::string filename);
    int save_in_file(std::string filename);

    bool is_valid_score();
};

#endif