#include "grid.hpp"

Grid::Grid(int s, int p)
{
    size = s;
    penality = p;
    init_tables();
}

Grid::Grid(int s, int p, int **num, const char **colors)
{
    size = s;
    penality = p;
    numbers = num;
    this->colors = copy_ttab_char(colors, size, size, &size, &size);
}

Grid::Grid(const Grid *g, bool copy_colors)
{
    this->size = g->size;
    this->penality = g->penality;
    this->numbers = g->numbers;
    if (!copy_colors)
    {
        this->colors = new char *[size];
        for (int i = 0; i < size; i++)
        {
            this->colors[i] = new char[size];
            memset(this->colors[i], 'X', size);
        }
    }
    else
    {
        this->colors = copy_ttab_char((const char **)g->colors, this->size, this->size, &this->size, &this->size);
    }
}

/// @brief Initialization of Grid
/// @param filename the file's name
/// @param extension only used when test is true
/// @param test set the Grid for testing, if true the filename will be {filename}[N|C]{extension} where N reprensents the numbers and C the colors of each files
Grid::Grid(std::string filename, std::string extension, bool test = false)
{
    if (test)
    {
        if (read_file_for_numbers(filename + "N" + extension) <= -1)
        {
            std::cerr << "An error occured while reading numbers, exiting\n";
            exit(1);
        }
        if (read_file_for_colors(filename + "C" + extension) <= -1)
        {
            std::cerr << "An error occured while reading colors, exiting\n";
            exit(1);
        }
        score = calcul_score();
        print_colors();
        print_numbers();

        if (is_valid_score())
        {
            std::cout << "The score calculated is rightly " << supposed_score << '\n';
        }
        else
        {
            std::cout << "The score was wrongly calculated, the score calculated is " << score << " but the supposed score is " << supposed_score << ".\n";
        }
    }
    else
    {
        if (read_file_for_numbers(filename) <= -1)
        {
            std::cerr << "An error occured while reading numbers, exiting\n";
            exit(1);
        }
    }
}

Grid::~Grid()
{
    for (int i = 0; i < size; i++)
    {
        delete[] colors[i];
    }
    delete[] colors;

    if (is_main_grid)
    {
        for (int i = 0; i < size; i++)
        {
            delete[] numbers[i];
        }
        delete[] numbers;
    }
}

Grid Grid::copy_grid(bool copy_colors) const
{
    Grid g = Grid(this, copy_colors);
    return g;
}

Grid *Grid::copy_grid_as_ptr(bool copy_colors) const
{
    Grid *g = new Grid(this, copy_colors);
    return g;
}

/// @brief init the tables of numbers and colors (dynamic allocation)
void Grid::init_tables()
{
    numbers = new int *[size];
    colors = new char *[size];
    for (int i = 0; i < size; i++)
    {
        numbers[i] = new int[size];
        memset(numbers[i], 0, size);
        colors[i] = new char[size];
        memset(colors[i], 'X', size);
    }
}

/// @brief
/// @param line
/// @param col
/// @return Return the score of the orthogonals cell + the cell itself
int Grid::get_cross_score(int line, int col) const
{
    return get_number(line, col) + get_number(line, col - 1) + get_number(line, col + 1) + get_number(line - 1, col) + get_number(line + 1, col);
}

int ***Grid::build_scores_tab(int black, int green, int yellow, int red, bool all) const
{
    int ***infoTab = new int **[size];

    for (int i = 0; i < size; i++)
    {
        infoTab[i] = new int *[size];
        for (int j = 0; j < size; j++)
        {
            infoTab[i][j] = new int[4];
        }
    }

    // setup table with all values for each color
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            infoTab[i][j][black] = 2 * calcul_score_place(i, j, 'N', true);
            infoTab[i][j][green] = calcul_score_place(i, j, 'V', true);
            infoTab[i][j][yellow] = calcul_score_place(i, j, 'J', true) /* - penality */;
            infoTab[i][j][red] = calcul_score_place(i, j, 'R', true);
        }
    }
    return infoTab;
}

void Grid::delete_scores_tab(int ***scores_tab) const
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            delete[] scores_tab[i][j];
        }
        delete[] scores_tab[i];
    }
    delete[] scores_tab;
}

int Grid::glouton(int value)
{
    std::string s = "RVNJ";
    const int black = 2, green = 1, yellow = 3, red = 0;
    int max_cur, color_cur;
    int ***scores_tab = build_scores_tab(black, green, yellow, red, true);
    couple coordinates_max;
    const int max_pieces = size * size;
    int nb_black = nb_color_in_grid('N'), placed_pieces = max_pieces - nb_empty_cells(), nb_red = nb_color_in_grid('R');
    // std::cout << "nb red = " << nb_red << '\n';

    do
    {
        max_cur = value;
        color_cur = -1;
        coordinates_max = couple{0, 0};
        // print_tttab((const int ***) scores_tab, size, size, 4);
        // std::cout << '\n';
        // print_colors();
        // std::cout << '\n';
        // iterate through all table to find the max value, green should be more considered
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                // printf("j = %d\n", j);
                // cell already taken
                if (colors[i][j] != 'X')
                {
                    continue;
                }

                // iterate through each color to find the best color at this place
                for (int k = 0; k < 4; k++)
                {
                    // handle priorities between colors
                    if (scores_tab[i][j][k] < max_cur or (k == black and nb_black >= size) or (k == red and nb_red >= 1))
                    {
                        // there is this condition to optimize the code (instead of checking two there is only on checked which coule be more true)
                        continue;
                    }
                    else if (scores_tab[i][j][k] > max_cur)
                    {
                        max_cur = scores_tab[i][j][k];
                        // printf("max cur = %d\n",  max_cur);
                        color_cur = k;
                        coordinates_max = couple{i, j};
                        // printf("coor = %d, %d\n", i, j);
                    }
                    else
                    {
                        // are equal
                        // do something
                    }
                }
            }
        }
        if (max_cur != value)
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
                // printf("update green\n");
                // substract penality for each cell around it
                this->update_green_scores_tab(scores_tab, coordinates_max.line, coordinates_max.column, green);

            default:
                break;
            }
        }

    } while (placed_pieces <= max_pieces and max_cur != value);
    // printf("placed_pieces = %d, max_cur %d\n", placed_pieces, max_cur);
    // this->print_colors();
    delete_scores_tab(scores_tab);
    return placed_pieces;
}

void Grid::glouton_stochastique(int max_pos)
{
    std::string s = "RVNJ";
    const int black = 2, green = 1, yellow = 3, red = 0;
    int max_cur = 0, color_cur;
    int ***scores_tab = build_scores_tab(black, green, yellow, red, true);
    couple coordinates_max;
    const int max_pieces = size * size;
    int nb_black = nb_color_in_grid('N'), placed_pieces = max_pieces - nb_empty_cells(), nb_red = nb_color_in_grid('R');
    // std::cout << "nb red = " << nb_red << '\n';

    const int current_max_pieces = max_pos /*  - generate_random_number(0, 4) */;

    int chosen_place;

    const int size_choice = size + 2;
    int chosen_pieces_size = 0;
    couple *cl = new couple[size_choice];
    char *co = new char[size_choice];

    do
    {
        chosen_pieces_size = 0;
        for (int x = 0; x < size_choice; x++)
        {
            // printf("x = %d\n", x);

            max_cur = 0;
            color_cur = -1;
            coordinates_max = couple{0, 0};
            // iterate through all table to find the max value, green should be more considered
            for (int i = 0; i < size; i++)
            {
                for (int j = 0; j < size; j++)
                {
                    // cell already taken
                    if (colors[i][j] != 'X' or contains_couple(cl, chosen_pieces_size, i, j))
                    {
                        continue;
                    }

                    // iterate through each color to find the best color at this place
                    for (int k = 0; k < 4; k++)
                    {
                        // handle priorities between colors
                        if (scores_tab[i][j][k] < max_cur or (k == black and nb_black >= size) or (k == red and nb_red >= 1))
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
            // printf("here2\n");
            if (max_cur != 0)
            {

                // colors[coordinates_max.line][coordinates_max.column] = s[color_cur];
                // placed_pieces++;

                cl[x] = coordinates_max;
                co[x] = s[color_cur];
                chosen_pieces_size++;

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
            }
            else
            {
                if (x != 0)
                {
                    max_cur = 1;
                }
                // no more places for pieces
                break;
            }
        }

        if (chosen_pieces_size != 0)
        {
            // printf("before crash value = %d\n", chosen_pieces_size);
            if (chosen_pieces_size == 1)
            {
                chosen_place = 0;
            }
            else
            {
                chosen_place = generate_random_number(0, chosen_pieces_size - 1);
            }
            // printf("after crash\n");
            colors[cl[chosen_place].line][cl[chosen_place].column] = co[chosen_place];
            placed_pieces++;
        }

    } while (placed_pieces <= current_max_pieces and max_cur != 0);
    delete[] cl;
    delete[] co;
    delete_scores_tab(scores_tab);
}

// this function does not generate the best grid, the around of green is not really taken in mind
void Grid::build_grid_points(bool *write_all, char *output_file, int secs)
{

    int nb_placed_glouton = this->glouton(0);
    print_colors();
    printf("%d\n", nb_placed_glouton);
    bool exit = false;
    clock_t start = clock();

    this->fill_and_opti(nb_placed_glouton, &start, secs, &exit);
    set_score_from_calculation();
    print_colors_with_score();

    for (int i = 0; i < 1000; i++)
    {
        boucle_alea(nb_placed_glouton, &start, secs, &exit);
    }
    this->set_score_from_calculation();
    this->print_colors_with_score();
}

void Grid::boucle_alea(int nb_placed_glouton, clock_t *start, int secs, bool *exit)
{
    Grid *tmpGrid = this->copy_grid_as_ptr(true);
    int count = 1;
    while (!*exit)
    {
        printf("%d ième testé\n", count++);
        tmpGrid->fill_and_opti(nb_placed_glouton, start, secs, exit);
        tmpGrid->set_score_from_calculation();
        // tmpGrid->print_colors_with_score();
        if (tmpGrid->get_score() > this->score)
        {
            this->switch_colors(tmpGrid);
            this->score = tmpGrid->get_score();
            this->print_colors_with_score();
        }
        tmpGrid->clear_colors();
    }
    delete tmpGrid;
}

void Grid::generate_random_grid_numbers(int minimun, int maximum)
{
    for (int i = 0; i < this->size; i++)
    {
        for (int j = 0; j < this->size; j++)
        {
            this->numbers[i][j] = generate_random_number(minimun, maximum);
        }
    }
}

void Grid::find_value(int value, bool *write_all, char *output_file)
{
    Grid *bestGrid = this->copy_grid_as_ptr(true);
    bestGrid->build_grid_points(write_all, output_file, 10);
    if (bestGrid->get_score() < value)
    {
        std::cout << "The given value is higher than the best value that we can compute\n";
        exit(0);
    }
}

/// @brief generate a random grid of colors for this Grid
void Grid::generate_random_grid()
{
    srand(time(NULL));
    // 4 -> 16 (15)
    // 4N -> 4/15
    //
    generate_random_number(0, 1);

    char **p = colors;

    int a = generate_random_number(0, size - 1);
    int b = generate_random_number(0, size - 1);
    p[a][b] = 'R';

    int compt = size * size - 1;
    int black = size;
    int blue = size;
    int green = 0.4 * (compt - black - blue);
    int yellow = green;

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            if ((i != a) or (j != b))
            {
                int n = generate_random_number(0, compt);

                if (n >= 0 and n < black)
                {
                    p[i][j] = 'N';
                    black--;
                }
                else if (n >= black and n < black + blue)
                {
                    p[i][j] = 'B';
                    blue--;
                }
                else if (n >= black + blue and n < black + blue + yellow)
                {
                    p[i][j] = 'J';
                    yellow--;
                }
                else if (n >= black + blue + yellow and n < black + blue + yellow + green)
                {
                    p[i][j] = 'V';
                    green--;
                }
                else
                {
                    p[i][j] = 'O';
                }
            }
            compt -= 1;
        }
    }
}

void Grid::optimize_grid_full()
{
    char **p = colors;
    int **c = numbers;

    int co = -1, col;
    int score = calcul_score(), initial_score = score;
    int score_yellow = INT32_MIN, score_black = INT32_MIN, score_blue = INT32_MIN, score_green = INT32_MIN, score_orange = INT32_MIN;

    int nb_black = nb_color_in_grid('N');

    std::string s = "RVNJ";
    const int black = 2, green = 1, yellow = 3, red = 0;
    int ***scores_tab = build_scores_tab(black, green, yellow, red, true);

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            col = p[i][j];

            for (int k = 0; k < 4; ++k)
            {
                if (col == s[k])
                {
                    co = k;
                    break;
                }
            }

            if (col == 'B' or col == 'O' or scores_tab[i][j][co] < 0 or (c[i][j] > 0 and scores_tab[i][j][co] < penality))
            {
                if (p[i][j] != 'J')
                {
                    p[i][j] = 'J';
                    score_yellow = this->calcul_score();
                    if (score_yellow <= score)
                        p[i][j] = col;
                    else
                    {
                        score = score_yellow;
                        col = 'J';
                    }
                }

                if (p[i][j] != 'V')
                {
                    p[i][j] = 'V';
                    score_green = this->calcul_score();
                    if (score_green <= score)
                        p[i][j] = col;
                    else
                    {
                        score = score_green;
                        col = 'V';
                    }
                }

                if (p[i][j] != 'N' and nb_black < size)
                {
                    p[i][j] = 'N';
                    score_black = this->calcul_score();
                    if (score_black <= score)
                        p[i][j] = col;
                    else
                    {
                        score = score_black;
                        col = 'N';
                        ++nb_black;
                    }
                }

                if (p[i][j] != 'B')
                {
                    p[i][j] = 'B';
                    score_blue = this->calcul_score();
                    if (score_blue <= score)
                        p[i][j] = col;
                    else
                    {
                        score = score_blue;
                        col = 'B';
                    }
                }

                if (p[i][j] != 'O')
                {
                    p[i][j] = 'O';
                    score_orange = this->calcul_score();
                    if (score_orange <= score)
                        p[i][j] = col;
                    else
                    {
                        score = score_orange;
                        col = 'O';
                    }
                }

                if (p[i][j] == 'N' and col != 'N')
                    nb_black--;

                if (score_blue > initial_score or score_black > initial_score or score_green > initial_score or score_orange > initial_score or score_yellow > initial_score)
                {
                    // optimize_grid_full(i, j, limit_recur - 1);
                }
            }
        }
    }

    this->delete_scores_tab(scores_tab);
}

/// @brief optimize recursively the grid, to be known the grid that calls this function will be added to glg
/// @param glg the given GridLinkGuard that stores the Grids generated by the optimization
/// @param cl a list structure to store the coordinates that should be tested, avoiding repeated branches
/// @param limit_recur 2 => couple modified, 3 => triple, 4 => ...
void Grid::optimize_grid_recur(GridLinkGuard *glg, coupleLink *cl, int limit_recur)
{
    // printf("called\n");

    if (limit_recur == 0)
    {
        glg->addGrid(this);
        return;
    }

    char **p = colors;
    // int **c = numbers;

    // int co = -1;
    int col;
    // int score = calcul_score();
    // int score_yellow = INT32_MIN, score_black = INT32_MIN, score_blue = INT32_MIN, score_green = INT32_MIN, score_orange = INT32_MIN;

    int nb_black = nb_color_in_grid('N');

    std::string s = "RVNJ";
    const int black = 2, green = 1, yellow = 3, red = 0;
    int ***scores_tab = build_scores_tab(black, green, yellow, red, true);

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {

            if (contains_CL(cl, i, j))
            {
                continue;
            }
            col = p[i][j];

            /* for (int k = 0; k < 4; ++k)
            {
                if (col == s[k])
                {
                    co = k;
                    break;
                }
            } */

            /* if (col == 'B' or col == 'O' or scores_tab[i][j][co] < 0 or c[i][j] > 0)
            { */
            if (col != 'J')
            {
                p[i][j] = 'J';

                Grid *g = this->copy_grid_as_ptr(true);
                // score_yellow = g->calcul_score();

                if (limit_recur >= 2)
                {
                    // if (score_yellow <= score)
                    // {
                    g->optimize_grid_recur(glg, add_first_CL(cl, i, j), limit_recur - 1);
                    // }
                }
                else
                {
                    glg->addGrid(g);
                }
            }

            if (col != 'V')
            {
                p[i][j] = 'V';

                Grid *g = this->copy_grid_as_ptr(true);
                // score_green = g->calcul_score();
                // if (score_green <= score)
                // {
                if (limit_recur >= 2)
                {
                    g->optimize_grid_recur(glg, add_first_CL(cl, i, j), limit_recur - 1);
                }
                else
                {
                    glg->addGrid(g);
                }
                // }
            }

            if (col != 'N' and nb_black < size)
            {
                p[i][j] = 'N';
                Grid *g = this->copy_grid_as_ptr(true);
                // score_black = g->calcul_score();
                // if (score_black <= score)
                // {
                if (limit_recur >= 2)
                {
                    g->optimize_grid_recur(glg, add_first_CL(cl, i, j), limit_recur - 1);
                }
                else
                {
                    glg->addGrid(g);
                }
                // }
            }

            if (col != 'B')
            {
                p[i][j] = 'B';
                Grid *g = this->copy_grid_as_ptr(true);
                // score_blue = g->calcul_score();
                // if (score_blue <= score)
                // {
                if (limit_recur >= 2)
                {
                    g->optimize_grid_recur(glg, add_first_CL(cl, i, j), limit_recur - 1);
                }
                else
                {
                    glg->addGrid(g);
                }
                // }
            }

            if (col != 'O')
            {
                p[i][j] = 'O';
                Grid *g = this->copy_grid_as_ptr(true);
                // score_orange = g->calcul_score();
                // if (score_orange <= score)
                // {
                if (limit_recur >= 2)
                {
                    g->optimize_grid_recur(glg, add_first_CL(cl, i, j), limit_recur - 1);
                }
                else
                {
                    glg->addGrid(g);
                }
                // }
            }

            if (p[i][j] == 'N' and col != 'N')
                nb_black--;

            p[i][j] = col;

            /* if (score_blue > initial_score or score_black > initial_score or score_green > initial_score or score_orange > initial_score or score_yellow > initial_score)
            {
                // optimize_grid_full(i, j, limit_recur - 1);
            } */
            // }
        }
    }

    delete cl;
    this->delete_scores_tab(scores_tab);
    glg->addGrid(this);
}

void Grid::orange_blue()
{
    char **p = colors;
    int **c = numbers;
    // int nb_blue = 0;

    // placement des bleu sur les positifs
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            if (p[i][j] == 'X' and c[i][j] > 0)
            {
                p[i][j] = 'B';
                // nb_blue ++;
            }
        }
    }

    // placement des orange
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            if (p[i][j] == 'X' and (i != 0 and j != 0))
            {
                p[i][j] = 'O';

                // lignes et colonnes
                for (int k = 0; k < size; ++k)
                {
                    if (p[i][k] == 'X')
                        p[i][k] = 'Q';
                }
                for (int k = 0; k < size; ++k)
                {
                    if (p[k][j] == 'X')
                        p[k][j] = 'Q';
                }

                // premiere diagonale
                if (i > j)
                {
                    for (int k = 0; k < size - (i - j); ++k)
                    {
                        if (p[k][i - j + k] == 'X')
                            p[k][i - j + k] = 'Q';
                    }
                }
                else
                {
                    for (int k = 0; k < size - (j - i); ++k)
                    {
                        if (p[k][j - i + k] == 'X')
                            p[k][j - i + k] = 'Q';
                    }
                }

                // deuxieme diagonale
                int jb = i + j;
                if (jb < size - 1)
                {
                    for (int k = 0; k < jb + 1; ++k)
                    {
                        if (p[jb - k][k] == 'X')
                            p[jb - k][k] = 'Q';
                    }
                }
                else
                {
                    for (int k = 0; k < (size - 1) * 2 + 1 - jb; ++k)
                    {
                        if (p[size - 1 - k][jb - (size - 1) + k] == 'X')
                            p[size - 1 - k][jb - (size - 1) + k] = 'Q';
                    }
                }
            }
        }
    }
    if (p[0][0] == 'X')
        p[0][0] = 'O';

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            if (p[i][j] == 'Q')
            {
                p[i][j] = 'X';
            }
        }
    }

    // std::cout << "avant\n";
    // this->print_colors();
    glouton(penality * (-1));
    // this->print_safe_colors();
    // std::cout << "apres\n";

    // placement du reste des bleu
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            if (p[i][j] == 'X')
            {
                p[i][j] = 'B';
            }
        }
    }
}

void Grid::yellow_replace_blue()
{
    char **p = colors;
    int **c = numbers;

    int b;

    int neg;
    int pos;
    nb_pos_neg_left_blue(&neg, &pos);
    // printf("neg = %d, pos = %d\n", neg, pos);

    if (neg > pos)
    {
        for (int i = 0; i < size; ++i)
        {
            for (int j = 0; j < size; ++j)
            {
                nb_pos_neg_left_blue(&neg, &pos);

                if (p[i][j] == 'B')
                {
                    // two blue next to each other
                    // if (pos < neg)
                    // {

                    b = nb_color_around_cell(i, j, 'B');

                    if (b >= 1)
                    {
                        if (get_color(i, j + 1) == 'B')
                        {
                            if ((c[i][j] + c[i][j + 1]) > -penality)
                            {
                                p[i][j] = 'J';
                                p[i][j + 1] = 'J';
                            }
                            b--;
                        }

                        if (get_color(i + 1, j - 1) == 'B' and b >= 1)
                        {
                            if ((c[i][j] + c[i + 1][j - 1]) > -penality)
                            {
                                p[i][j] = 'J';
                                p[i + 1][j - 1] = 'J';
                            }
                            b--;
                        }

                        if (get_color(i + 1, j) == 'B' and b >= 1)
                        {
                            if ((c[i][j] + c[i + 1][j]) > -penality)
                            {
                                p[i][j] = 'J';
                                p[i + 1][j] = 'J';
                            }
                            b--;
                        }

                        if (get_color(i + 1, j + 1) == 'B' and b >= 1)
                        {
                            if ((c[i][j] + c[i + 1][j + 1]) > -penality)
                            {
                                p[i][j] = 'J';
                                p[i + 1][j + 1] = 'J';
                            }
                            b--;
                        }
                    }
                    // }
                }
            }
        }
    }
}

/// @brief replace blue by yellow
void Grid::yellow_replace_blue_duo()
{
    char **p = colors;
    int **c = numbers;

    int b, d;

    bool changed = false;

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            if (p[i][j] == 'B' and numbers[i][j] < 0)
            {
                b = nb_color_around_cell(i, j, 'B');
                changed = false;
                if (b >= 1)
                {
                    if (get_color(i, j + 1) == 'B' and numbers[i][j + 1] < 0)
                    {
                        for (int r = 0; r < size; r++)
                        {
                            for (int s = 0; s < size; s++)
                            {

                                if (p[r][s] == 'B' and numbers[r][s] > 0)
                                {

                                    d = nb_color_around_cell(r, s, 'B');
                                    if (d >= 1)
                                    {
                                        if (get_color(r, s + 1) == 'B' and numbers[r][s + 1] > 0)
                                        {
                                            if ((c[r][s] + c[r][s + 1] + c[i][j] + c[i][j + 1]) > 0)
                                            {
                                                p[r][s] = 'J';
                                                p[r][s + 1] = 'J';
                                                p[i][j] = 'J';
                                                p[i][j + 1] = 'J';
                                                changed = true;
                                                d--;
                                            }
                                        }

                                        if (get_color(r + 1, s - 1) == 'B' and numbers[r + 1][s - 1] > 0 and d >= 1 and not changed)
                                        {
                                            if ((c[r][s] + c[r + 1][s - 1] + c[i][j] + c[i][j + 1]) > 0)
                                            {
                                                p[r][s] = 'J';
                                                p[r + 1][s - 1] = 'J';
                                                p[i][j] = 'J';
                                                p[i][j + 1] = 'J';
                                                changed = true;
                                                d--;
                                            }
                                        }

                                        if (get_color(r + 1, s) == 'B' and numbers[r + 1][s] > 0 and d >= 1 and not changed)
                                        {
                                            if ((c[r][s] + c[r + 1][s] + c[i][j] + c[i][j + 1]) > 0)
                                            {
                                                p[r][s] = 'J';
                                                p[r + 1][s] = 'J';
                                                p[i][j] = 'J';
                                                p[i][j + 1] = 'J';
                                                changed = true;
                                                d--;
                                            }
                                        }

                                        if (get_color(r + 1, s + 1) == 'B' and numbers[r + 1][s + 1] > 0 and d >= 1 and not changed)
                                        {
                                            if ((c[r][s] + c[r + 1][s + 1] + c[i][j] + c[i][j + 1]) > 0)
                                            {
                                                p[r][s] = 'J';
                                                p[r + 1][s + 1] = 'J';
                                                p[i][j] = 'J';
                                                p[i][j + 1] = 'J';
                                                changed = true;
                                                d--;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        b--;
                    }

                    if (get_color(i + 1, j - 1) == 'B' and numbers[i + 1][j - 1] < 0 and b >= 1)
                    {
                        changed = false;
                        for (int r = 0; r < size; r++)
                        {
                            for (int s = 0; s < size; s++)
                            {

                                if (p[r][s] == 'B' and numbers[r][s] > 0)
                                {

                                    d = nb_color_around_cell(r, s, 'B');
                                    if (d >= 1)
                                    {
                                        if (get_color(r, s + 1) == 'B' and numbers[r][s + 1] > 0)
                                        {
                                            if ((c[r][s] + c[r][s + 1] + c[i][j] + c[i + 1][j - 1]) > 0)
                                            {
                                                p[r][s] = 'J';
                                                p[r][s + 1] = 'J';
                                                p[i][j] = 'J';
                                                p[i + 1][j - 1] = 'J';
                                                changed = true;
                                                d--;
                                            }
                                        }

                                        if (get_color(r + 1, s - 1) == 'B' and numbers[r + 1][s - 1] > 0 and d >= 1 and not changed)
                                        {
                                            if ((c[r][s] + c[r + 1][s - 1] + c[i][j] + c[i + 1][j - 1]) > 0)
                                            {
                                                p[r][s] = 'J';
                                                p[r + 1][s - 1] = 'J';
                                                p[i][j] = 'J';
                                                p[i + 1][j - 1] = 'J';
                                                changed = true;
                                                d--;
                                            }
                                        }

                                        if (get_color(r + 1, s) == 'B' and numbers[r + 1][s] > 0 and d >= 1 and not changed)
                                        {
                                            if ((c[r][s] + c[r + 1][s] + c[i][j] + c[i + 1][j - 1]) > 0)
                                            {
                                                p[r][s] = 'J';
                                                p[r + 1][s] = 'J';
                                                p[i][j] = 'J';
                                                p[i + 1][j - 1] = 'J';
                                                changed = true;
                                                d--;
                                            }
                                        }

                                        if (get_color(r + 1, s + 1) == 'B' and numbers[r + 1][s + 1] > 0 and d >= 1 and not changed)
                                        {
                                            if ((c[r][s] + c[r + 1][s + 1] + c[i][j] + c[i + 1][j - 1]) > 0)
                                            {
                                                p[r][s] = 'J';
                                                p[r + 1][s + 1] = 'J';
                                                p[i][j] = 'J';
                                                p[i + 1][j - 1] = 'J';
                                                changed = true;
                                                d--;
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        if ((c[i][j] + c[i + 1][j - 1]) > -penality)
                        {
                            p[i][j] = 'J';
                            p[i + 1][j - 1] = 'J';
                        }
                        b--;
                    }

                    if (get_color(i + 1, j) == 'B' and numbers[i + 1][j] < 0 and b >= 1)
                    {
                        changed = false;
                        for (int r = 0; r < size; r++)
                        {
                            for (int s = 0; s < size; s++)
                            {

                                if (p[r][s] == 'B' and numbers[r][s] > 0)
                                {

                                    d = nb_color_around_cell(r, s, 'B');
                                    if (d >= 1)
                                    {
                                        if (get_color(r, s + 1) == 'B' and numbers[r][s + 1] > 0)
                                        {
                                            if ((c[r][s] + c[r][s + 1] + c[i][j] + c[i + 1][j]) > 0)
                                            {
                                                p[r][s] = 'J';
                                                p[r][s + 1] = 'J';
                                                p[i][j] = 'J';
                                                p[i + 1][j] = 'J';
                                                changed = true;
                                                d--;
                                            }
                                        }

                                        if (get_color(r + 1, s - 1) == 'B' and numbers[r + 1][s - 1] > 0 and d >= 1 and not changed)
                                        {
                                            if ((c[r][s] + c[r + 1][s - 1] + c[i][j] + c[i + 1][j]) > 0)
                                            {
                                                p[r][s] = 'J';
                                                p[r + 1][s - 1] = 'J';
                                                p[i][j] = 'J';
                                                p[i + 1][j] = 'J';
                                                changed = true;
                                                d--;
                                            }
                                        }

                                        if (get_color(r + 1, s) == 'B' and numbers[r + 1][s] > 0 and d >= 1 and not changed)
                                        {
                                            if ((c[r][s] + c[r + 1][s] + c[i][j] + c[i + 1][j]) > 0)
                                            {
                                                p[r][s] = 'J';
                                                p[r + 1][s] = 'J';
                                                p[i][j] = 'J';
                                                p[i + 1][j] = 'J';
                                                changed = true;
                                                d--;
                                            }
                                        }

                                        if (get_color(r + 1, s + 1) == 'B' and numbers[r + 1][s + 1] > 0 and d >= 1 and not changed)
                                        {
                                            if ((c[r][s] + c[r + 1][s + 1] + c[i][j] + c[i + 1][j]) > 0)
                                            {
                                                p[r][s] = 'J';
                                                p[r + 1][s + 1] = 'J';
                                                p[i][j] = 'J';
                                                p[i + 1][j] = 'J';
                                                changed = true;
                                                d--;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        b--;
                    }

                    if (get_color(i + 1, j + 1) == 'B' and numbers[i + 1][j + 1] < 0 and b >= 1)
                    {
                        changed = false;
                        for (int r = 0; r < size; r++)
                        {
                            for (int s = 0; s < size; s++)
                            {

                                if (p[r][s] == 'B' and numbers[r][s] > 0)
                                {

                                    d = nb_color_around_cell(r, s, 'B');
                                    if (d >= 1)
                                    {
                                        if (get_color(r, s + 1) == 'B' and numbers[r][s + 1] > 0)
                                        {
                                            if ((c[r][s] + c[r][s + 1] + c[i][j] + c[i + 1][j + 1]) > 0)
                                            {
                                                p[r][s] = 'J';
                                                p[r][s + 1] = 'J';
                                                p[i][j] = 'J';
                                                p[i + 1][j + 1] = 'J';
                                                changed = true;
                                                d--;
                                            }
                                        }

                                        if (get_color(r + 1, s - 1) == 'B' and numbers[r + 1][s - 1] > 0 and d >= 1 and not changed)
                                        {
                                            if ((c[r][s] + c[r + 1][s - 1] + c[i][j] + c[i + 1][j + 1]) > 0)
                                            {
                                                p[r][s] = 'J';
                                                p[r + 1][s - 1] = 'J';
                                                p[i][j] = 'J';
                                                p[i + 1][j + 1] = 'J';
                                                changed = true;
                                                d--;
                                            }
                                        }

                                        if (get_color(r + 1, s) == 'B' and numbers[r + 1][s] > 0 and d >= 1 and not changed)
                                        {
                                            if ((c[r][s] + c[r + 1][s] + c[i][j] + c[i + 1][j + 1]) > 0)
                                            {
                                                p[r][s] = 'J';
                                                p[r + 1][s] = 'J';
                                                p[i][j] = 'J';
                                                p[i + 1][j + 1] = 'J';
                                                changed = true;
                                                d--;
                                            }
                                        }

                                        if (get_color(r + 1, s + 1) == 'B' and numbers[r + 1][s + 1] > 0 and d >= 1 and not changed)
                                        {
                                            if ((c[r][s] + c[r + 1][s + 1] + c[i][j] + c[i + 1][j + 1]) > 0)
                                            {
                                                p[r][s] = 'J';
                                                p[r + 1][s + 1] = 'J';
                                                p[i][j] = 'J';
                                                p[i + 1][j + 1] = 'J';
                                                changed = true;
                                                d--;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        b--;
                    }
                }
            }
        }
    }
}

void Grid::fill_and_opti(int max_pieces, clock_t *start, int secs, bool *exit)
{
    this->glouton_stochastique(max_pieces);
    this->orange_blue();
    this->set_score_from_calculation();
    int old_score;
    GridLinkGuard *glg = new GridLinkGuard;
    do
    {
        old_score = this->get_score();
        if (this->size > SIZE_BETWEEN)
        {
            optimize_grid_full(); // moyen
        }
        else
        {

            copy_grid_as_ptr(true)->optimize_grid_recur(glg, nullptr, 2);
            glg->convert_to_best_scores();
            glg->switch_colors_with_first(this);
            glg->clear(true);
        }
        this->yellow_replace_blue_duo(); // important

        set_score_from_calculation();
        if (((0.0 + *start - clock()) / CLOCKS_PER_SEC) >= secs - 1)
        {
            *exit = true;
        }
    } while (old_score != this->score and not *exit);
    yellow_replace_blue();
    delete glg;
}
