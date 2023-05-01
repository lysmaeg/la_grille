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
    GridLink *firstGridLink = nullptr;
    GridLink *lastGridLink = nullptr;

public:
    GridLinkGuard();
    GridLinkGuard(Grid *g);
    ~GridLinkGuard();

    void init();

    void addGrid(Grid *g);
    void pop_first(bool clear_grid);
    void clear(bool clear_grid);
    void extend(GridLinkGuard *glg);

    GridLinkGuard *copy_as_ptr();

    GridLinkGuard *get_best_scores();
    void print_all_scores();
    std::string *get_pretty_print();
    void pretty_print() const;
};


#endif