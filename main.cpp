#include "grid_utils/grid.hpp"

int main(void)
{
    srand(time(NULL));

    // Grid g1 = Grid("../testFiles/test1", ".txt", true); // test le fichier ../testFiles/test1N.txt et ../testFiles/test1C.txt

    // aleatoire
    // g.print_colors();
    // g.generate_random_grid();
    // g.print_colors();
    // g.save_in_file("coucou");
    // std::cout << "Les points de la grille sont " << g.calcul_score() << ".\n";

    /* Grid g = Grid("../testFiles/test2N.txt", "", false);
    g.print_numbers();
    g.build_grid_points(); */

    // Grid g = Grid("../testFiles/test4N.txt", "", false);
    Grid g = Grid("../instances/hors_competition/probleme_4_a.txt", "", false);
    g.print_numbers();
    g.build_grid_points();
    // g.print_colors_with_score();
    // g.optimize_grid_full();
    //g.print_colors_with_score();
    
    /* Grid g = Grid("../instances/hors_competition/probleme_4_b.txt", "", false);
    g.print_numbers();

    
    g.build_grid_points();
    // g.brute_force();
    g.print_colors(); */

    /* g.generate_random_grid();
    g.set_score_from_calculation();
    g.print_colors_with_score();

    g.optimize_grid_full();
    g.set_score_from_calculation();
    g.print_colors_with_score(); */

    //Grid g = Grid("../testFiles/test4N.txt", "", false);
    //g.brute_force();

    // g.~Grid();

    return 0;

    // 23
}
