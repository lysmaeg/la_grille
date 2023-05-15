#include "grid_unused.hpp"


void Grid::fill_blank_recur(GridLinkGuard *glg, int pieces_left)
{
    if (pieces_left == 0)
    {
        glg->addGrid(this->copy_grid_as_ptr(true));
        return;
    }

    int x, y;
    if (get_coordinates_empty_cell(&x, &y) == -1)
    {
        std::cerr << "An error occured...\n";
        exit(EXIT_FAILURE);
    }

    colors[x][y] = 'B';
    fill_blank_recur(glg, pieces_left - 1);
    colors[x][y] = 'O';
    fill_blank_recur(glg, pieces_left - 1);
    colors[x][y] = 'X';
}

void Grid::fill_blank()
{
    // conditions depending of the amount of cells left
    if (nb_empty_cells() <= 20)
    {
        std::cout << "1\n";
        GridLinkGuard *glg = new GridLinkGuard;
        fill_blank_recur(glg, this->nb_empty_cells());
        // glg->convert_to_best_scores();
        glg->get_best_scores()->switch_colors_with_first(this);
        this->yellow_replace_blue();
        this->optimize_grid_full();
        delete glg;
    }
    else
    {
        std::cout << "2\n";
        this->orange_blue();
    }
}

/// @brief brute force the grid, printing the best grid found, the grid is not modified
void Grid::brute_force()
{
    GridLinkGuard *glg = new GridLinkGuard;
    int nb_thread = 0;
    int nb_tested = 0;
    this->brute_force_recur(glg, nb_empty_cells(), &nb_thread, false, &nb_tested);
    while (nb_thread >= 1)
    {
    }
    printf("finished\n");
    GridLinkGuard *bestGlg = glg->get_best_scores();
    bestGlg->pretty_print();
    delete bestGlg;
}


void Grid::glouton_recur(GridLinkGuard *glg, int nb_red, int nb_black, int placed_pieces, int limit_recur, coupleLink *cl)
{
    if (limit_recur == 0)
    {
        return;
    }
    std::string s = "RVNJ";
    const int black = 2, green = 1, yellow = 3, red = 0;
    int max_cur, color_cur;
    int ***scores_tab = build_scores_tab(black, green, yellow, red, true);
    couple coordinates_max;
    const int max_pieces = size * size;

    do
    {
        max_cur = 0;
        color_cur = -1;
        coordinates_max = couple{0, 0};
        // iterate through all table to find the max value, green should be more considered
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                // cell already taken
                if (colors[i][j] != 'X')
                {
                    continue;
                }

                // iterate through each color to find the best color at this place
                for (int k = 0; k < 4; k++)
                {
                    // handle priorities between colors
                    if (scores_tab[i][j][k] < max_cur or (k == black and nb_black == size) or (k == red and nb_red == 1))
                    {
                        // there is this condition to optimize the code (instead of checking two there is only on checked which coule be more true)
                        continue;
                    }
                    else if (scores_tab[i][j][k] > max_cur)
                    {
                        max_cur = scores_tab[i][j][k];
                        color_cur = k;
                        coordinates_max = couple{i, j};
                    }
                    else
                    {
                        // are equal
                        // do something
                    }
                }
            }
        }
        if (max_cur != 0)
        {

            colors[coordinates_max.line][coordinates_max.column] = s[color_cur];
            placed_pieces++;
            // manage colors that impact the same color
            switch (color_cur)
            {
            case black:
                nb_black++;
                break;

            case yellow:
                // add penality for each cell around it
                this->update_yellow_scores_tab(scores_tab, coordinates_max.line, coordinates_max.column, yellow);

                break;
            case red:
                nb_red++;
            case green:
                // substract penality for each cell around it
                this->update_green_scores_tab(scores_tab, coordinates_max.line, coordinates_max.column, green);

            default:
                break;
            }
            if (limit_recur >= 2 /* and color_cur == green */ and numbers[coordinates_max.line][coordinates_max.column] > 0)
            {
                Grid *g = this->copy_grid_as_ptr(true);
                // g->colors[coordinates_max.line][coordinates_max.column] = 'B';
                g->glouton_recur(glg, nb_red, nb_black, placed_pieces, limit_recur - 1, add_first_CL(cl, coordinates_max.line, coordinates_max.column));
            }
        }

    } while (placed_pieces <= max_pieces and max_cur != 0);
    // printf("placed_pieces = %d, max_cur %d\n", placed_pieces, max_cur);
    // this->print_colors();
    delete cl;
    delete_scores_tab(scores_tab);
    glg->addGrid(this);
}


void Grid::brute_force_recur(GridLinkGuard *glg, int pieces_left, int *nb_thread, bool red_posed, int *nb_tested)
{
    if (*nb_tested == 1000000000)
    {
        std::cout << "1 000 000 000 tested\n";
        *nb_tested = 0;
    }
    if (pieces_left == 0)
    {
        *nb_tested += 1;
        glg->addGrid(this->copy_grid_as_ptr(true));
        return;
    }

    std::thread *threadIdTab;
    int x, y;
    if (this->get_coordinates_empty_cell(&x, &y) == -1)
    {
        std::cerr << "An error occured...\n";
        exit(EXIT_FAILURE);
    }

    // printf("here x = %d, y = %d, nbth = %d\n", x, y, *nb_thread);
    // this->print_colors();

    if (!red_posed)
    {
        this->colors[x][y] = 'R';
        brute_force_recur(glg, pieces_left - 1, nb_thread, true, nb_tested);
        this->colors[x][y] = 'X';
    }

    std::string s = "BVNJO";
    if (pieces_left >= 4)
    {

        for (int i = 0; i < (int)s.length(); i++)
        {
            if (*nb_thread < NB_MAX_THREAD)
            {
                threadIdTab = new std::thread[s.length()];
                (*nb_thread)++;
                threadIdTab[i] = std::thread(&Grid::brute_force_launcher, this->copy_grid_as_ptr(true), glg, pieces_left, nb_thread, red_posed, x, y, s[i], nb_tested);

                // wait all thread to be finished
                for (int i = 0; i < (int)s.length(); i++)
                {
                    threadIdTab[i].join();
                    (*nb_thread)--;
                }
                delete[] threadIdTab;
            }
            else
            {
                this->colors[x][y] = s[i];
                brute_force_recur(glg, pieces_left - 1, nb_thread, red_posed, nb_tested);
                this->colors[x][y] = 'X';
            }
        }
    }
    else
    {
        for (int i = 0; i < (int)s.length(); i++)
        {
            this->colors[x][y] = s[i];
            brute_force_recur(glg, pieces_left - 1, nb_thread, red_posed, nb_tested);
            this->colors[x][y] = 'X';
        }
    }
}

void Grid::brute_force_launcher(GridLinkGuard *glg, int pieces_left, int *nb_thread, bool red_posed, int x, int y, char c, int *nb_tested)
{
    // printf("thread started x = %d, y = %d, c = %c\n", x, y, c);
    GridLinkGuard *glgThread = new GridLinkGuard;

    this->colors[x][y] = c;
    brute_force_recur(glgThread, pieces_left - 1, nb_thread, red_posed, nb_tested);
    this->colors[x][y] = 'X';
    printf("thread finished\n");

    GridLinkGuard *bestScoresThread = glgThread->get_best_scores();
    printf("before extending\n");
    glg->extend(bestScoresThread);
    printf("after extend\n");

    delete glgThread;
}
