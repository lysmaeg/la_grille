#include "grid_utils/grid.hpp"

int main(void)
{
    srand(time(NULL));

    //Grid g1 = Grid("../testFiles/test1", ".txt", true); // test le fichier ../testFiles/test1N.txt et ../testFiles/test1C.txt

    // aleatoire
    //g.print_colors();
    //g.generate_random_grid();
    // g.print_colors();
    //g.save_in_file("coucou");
    // std::cout << "Les points de la grille sont " << g.calcul_score() << ".\n";


    /* Grid g = Grid("../testFiles/test2N.txt", "", false);
    g.print_numbers();
    g.build_grid_points(); */


    Grid g = Grid("../testFiles/test5N.txt", "", false);
    g.print_numbers();
    //g.build_grid_points();
    g.brute_force();
    g.print_colors();
    return 0;
}
