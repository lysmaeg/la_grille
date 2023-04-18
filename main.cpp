#include "grid.hpp"

int main(void)
{

    Grid g = Grid("../test1", ".txt", true);

    /* Grid *g = read_file_to_colors("../testC1.txt");
    read_file_to_numbers(g, "../testG1.txt");
    std::cout << g->calcul_score() << '\n'; */
    return 0;
}
