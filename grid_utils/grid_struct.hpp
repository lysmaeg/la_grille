#ifndef __GRID_STRUCTS__
#define __GRID_STRUCTS__
#include "grid.hpp"

class Grid;

struct GridLink
{
    Grid *grid = nullptr;
    GridLink *next = nullptr;
};

class GridLinkGuard
{
private:
    int size = 0;
    int clear_at = 10000;
    GridLink *lastGridLink = nullptr;
    GridLink *firstGridLink = nullptr;

public:
    GridLinkGuard();
    GridLinkGuard(Grid *g);
    ~GridLinkGuard();

    void init();
    const int get_size() const;

    void addGrid(Grid *g);
    void addGridStart(Grid *g);
    void addGridEnd(Grid *g);

    void pop_first(bool clear_grid);
    void clear(bool clear_grid);
    void clear_following(GridLink *first, bool clear_grid);
    void extend(GridLinkGuard *glg);
    // void remove_same();
    bool contains_same_colors(const Grid *g) const;
    void switch_colors_with_first(Grid *g);

    GridLinkGuard *solve_all_grids();
    void save_all_in_files(char *output_file);

    GridLinkGuard *copy_as_ptr();

    GridLinkGuard *get_best_scores() const;
    void convert_to_best_scores();
    const Grid *get_first_grid() const;

    void print_all_scores();
    std::string *get_pretty_print();
    void pretty_print() const;

    void fill_blank_all();
    void opti_recur(int model);
};

#endif