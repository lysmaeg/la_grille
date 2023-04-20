#ifndef __GRID__
#define __GRID__
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include "../tools.hpp"

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

    char get_color(int line, int column) const;
    int get_number(int line, int column) const;

    int calcul_score() const;

    void print_numbers() const;
    void print_colors() const;
    void print_safe_colors() const;
    char *colors_to_string() const;

    // grid_files_manager.hpp
    int read_file_for_numbers(std::string filename);
    int read_file_for_colors(std::string filename);
    int save_in_file(std::string filename);

    bool is_valid_score();

    std::string get_around_colors(int line, int col) const;
    std::string *get_lines_cols(int line, int col, bool all) const;
    int nb_colors_in_lines_cols(int line, int col, int all, char color) const;

    int calcul_score_place(int line, int col, char color, bool all);

    int get_cross_score(int line, int col) const;
    int nb_color_around_cell(int line, int col, char color) const;

    int calculate_red_piece(int line, int col) const;
    int calculate_yellow_piece(int line, int col) const;
    int calculate_green_piece(int line, int col) const;
    int calculate_orange_piece(int line, int col, int all) const;

    void generate_random_grid();
    void build_grid_points();
};

#endif