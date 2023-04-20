#include "grid_utils/grid.hpp"

int main(void)
{

    Grid g1 = Grid("../testFiles/test1", ".txt", true); // test le fichier ../testFiles/test1N.txt et ../testFiles/test1C.txt

    // aleatoire
    Grid g = Grid("../testFiles/test1N.txt", "", false);
    //g.print_colors();
    g.print_numbers();
    g.generate_random_grid();
    g.print_colors();
    g.save_in_file("coucou");
    std::cout << "Les points de la grille sont " << g.calcul_score() << ".\n";
    return 0;
}
