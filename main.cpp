#include "grid_utils/grid.hpp"

void print_help_message()
{
    std::cout << "Welcome in the home page : \n\nHere are the arguments available :\n\n";
    std::cout << "./exec [-h/--help] [-o] [file to be read]\n\n";
    std::cout << "=> -o : specify the output file, if not given the output will only be in the terminal";
    std::cout << "=> -t : specify the time that the program requires\n";
    std::cout << "=> -h / --help : print this help message\n";
    std::cout << "=> -w / --write-all : \n";
}

Grid *manage_arguments(int argc, char **args, char *&output_file, bool *write_all)
{
    bool input_given = false;
    bool write_all_given = false;
    *write_all = false;
    Grid *g;

    for (int i = 1; i < argc; i++)
    {
        char *t = args[i];
        if (strcmp(t, "-o") == 0)
        {
            i++;
            output_file = args[i];
            printf("%s\n", output_file);
        }
        else if (strcmp("-h", t) == 0 or strcmp("--help", t) == 0)
        {
            print_help_message();
        }
        else if (strcmp("-w", t) == 0 or strcmp("--write-all", t) == 0)
        {
            if (!write_all_given)
            {
                write_all_given = true;
                *write_all = true;
            }
        }
        else
        {
            if (!input_given)
            {
                g = new Grid(t, "", false);
                if (g->read_file_for_numbers(t) <= -1)
                {
                    std::cerr << "An error occured, cannot read the file " << t << "\n";
                    exit(EXIT_SUCCESS);
                }
                input_given = true;
            }
            else
            {
                std::cerr << "You cannot give the input file twice, exiting\n";
                print_help_message();
                exit(0);
            }
        }
    }

    if (!input_given)
    {
        std::cout << "You did not give the input file, exiting\n";
        print_help_message();
        exit(EXIT_SUCCESS);
    }
    return g;
}

int main(int argc, char **args)
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
    // Grid g = Grid("../instances/hors_competition/probleme_4_b.txt", "", false);

    char *output_file = nullptr;
    bool write_all = false;
    Grid *g = manage_arguments(argc, args, output_file, &write_all);

    
    g->print_numbers();
    g->build_grid_points(&write_all, output_file);

    /* g->read_file_for_colors("../testFiles/6abest");
    g->set_score_from_calculation();
    g->print_numbers();
    g->print_colors_with_score();

    GridLinkGuard *glg = new GridLinkGuard;
    g->copy_grid_as_ptr(true)->optimize_grid_recur(glg, nullptr, 2);
    glg->convert_to_best_scores();
    glg->pretty_print();

    glg->opti_recur(2);
    glg->pretty_print();

    glg->opti_recur(2);
    glg->pretty_print(); */

    // glg->opti_recur(4);
    // glg->pretty_print();
    // g.generate_random_grid();

    /* for (int i = 0; i < 200; i++)
    {
        g.optimize_grid_full();
    }
    g.set_score_from_calculation();
    g.print_colors_with_score(); */

    // g.print_colors_with_score();
    // g.optimize_grid_full();
    // g.print_colors_with_score();

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

    // Grid g = Grid("../testFiles/test4N.txt", "", false);
    // g.brute_force();

    // g.~Grid();

    return 0;

    // 23
}
