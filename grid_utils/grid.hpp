#ifndef __GRID__
#define __GRID__
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include "../tools.hpp"
#include <thread>
#include "grid_struct.hpp"

class GridLinkGuard;
class GridLink;

class Grid
{
private:
    int size;
    int penality;
    int supposed_score = 0;
    int score = 0;
    bool is_main_grid = false;

    void brute_force_recur(GridLinkGuard *glg, int pieces_left, int *nb_thread, bool red_placed, int *nb_tested);
    void brute_force_launcher(GridLinkGuard *glg, int pieces_left, int *nb_thread, bool red_posed, int x, int y, char c, int *nb_tested);

    int ***build_scores_tab(int black, int green, int yellow, int red, bool all) const;
    void delete_scores_tab(int ***scores_tab) const;

public:
    int **numbers;
    char **colors;

    Grid(int size, int penalite);
    Grid(std::string filename, std::string extension, bool test);
    Grid(int size, int penalite, int **num, const char **colors);
    Grid(const Grid *g, bool copy_colors);
    ~Grid();
    Grid copy_grid(bool copy_colors) const;
    Grid *copy_grid_as_ptr(bool copy_colors) const;

    void init_tables();

    // grid_utils
    int get_size() const;
    int get_penality() const;
    int get_supposed_score() const;
    int get_score() const;
    void set_score_from_calculation();

    char get_color(int line, int column) const;
    int get_number(int line, int column) const;

    char *colors_to_string() const;
    void print_numbers() const;
    void print_colors() const;
    void print_safe_colors() const;
    void print_colors_with_score() const;

    int nb_empty_cells() const;
    int get_coordinates_empty_cell(int *x, int *y) const;

    void update_green_scores_tab(int ***scores_tab, int line, int column, int index);

    bool is_valid_score();

    /* --- */

    // grid_files_manager.hpp
    int read_file_for_numbers(std::string filename);
    int read_file_for_colors(std::string filename);
    int save_in_file(std::string filename) const;

    /* --- */

    std::string get_around_colors(int line, int col) const;
    std::string *get_lines_cols(int line, int col, bool all) const; // grid_utils
    int nb_colors_in_lines_cols(int line, int col, int all, char color) const;

    int get_cross_score(int line, int col) const;
    int nb_color_around_cell(int line, int col, char color) const;
    int nb_color_in_grid(char c) const;
    void nb_pos_neg_left(int *neg, int *pos) const;

    /* score calculation */

    int calculate_red_piece(int line, int col) const;
    int calculate_yellow_piece(int line, int col) const;
    int calculate_green_piece(int line, int col) const;
    int calculate_orange_piece(int line, int col, int all) const;

    int calcul_score_place(int line, int col, char color, bool all) const;

    int calcul_score() const;

    /* --- */

    void generate_random_grid();
    void build_grid_points();
    void fill_blank(GridLinkGuard *glg, int pieces_left);

    void optimize_grid_full();
    void optimize_grid_recur(bool main, GridLinkGuard *glg, coupleLink *cl, int limit_recur);

    void orange_blue();

    void brute_force();
};

#endif