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

/// @brief an helpful function for the yellow
/// @param line
/// @param col
/// @return Return all the colors as a string around the given coordinates
std::string Grid::get_around_colors(int line, int col) const
{
    std::string s = "";
    char c;
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            if (i == 0 and j == 0)
                continue;
            c = get_color(line + i, col + j);
            if (c != 0)
            {
                s += c;
            }
        }
    }
    return s;
}

/// @brief an helpful function for the orange
/// @param line
/// @param col
/// @param all if all set to false only the four directions : SO, S, SW and W will be analysed
/// @param color the color to be tested for counting
/// @return the amount of the same color in the diagonals, columns and lines
int Grid::nb_colors_in_lines_cols(int line, int col, int all, char color) const
{
    int nb = 0;
    std::string *s = get_lines_cols(line, col, all);
    for (int i = 0; i < 4; ++i)
    {
        for (long unsigned int j = 0; j < s[i].length(); ++j)
            if (s[i][j] == color)
            {
                nb++;
            }
    }
    delete[] s;
    return nb;
}

/// @brief
/// @param line
/// @param col
/// @param color color to be checked
/// @return Return the amount of the same colors around the coordinates
int Grid::nb_color_around_cell(int line, int col, char color) const
{
    std::string s = get_around_colors(line, col);
    int nb = 0;
    for (long unsigned int i = 0; i < s.length(); i++)
    {
        if (s[i] == color)
        {
            nb++;
        }
    }
    return nb;
}

int Grid::nb_color_in_grid(char c) const
{
    int nb = 0;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (colors[i][j] == c)
            {
                nb++;
            }
        }
    }
    return nb;
}

void Grid::nb_pos_neg_left_blue(int *neg, int *pos) const
{
    *pos = 0;
    *neg = 0;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (this->colors[i][j] == 'B')
            {
                if (numbers[i][j] > 0)
                {
                    *pos += 1;
                }
                else if (numbers[i][j] < 0)
                {
                    *neg += 1;
                }
            }
        }
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
            infoTab[i][j][yellow] = calcul_score_place(i, j, 'J', true) - penality;
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

void Grid::glouton(int value)
{
    std::string s = "RVNJ";
    const int black = 2, green = 1, yellow = 3, red = 0;
    int max_cur, color_cur;
    int ***scores_tab = build_scores_tab(black, green, yellow, red, true);
    couple coordinates_max;
    const int max_pieces = size * size;
    int nb_black = nb_color_in_grid('N'), placed_pieces = max_pieces - nb_empty_cells(), nb_red = nb_color_in_grid('R');
    std::cout << "nb red = " << nb_red << '\n';

    do
    {
        max_cur = value;
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
            if (limit_recur >= 2 and color_cur == green and numbers[coordinates_max.line][coordinates_max.column] > 0)
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

// this function does not generate the best grid, the around of green is not really taken in mind
void Grid::build_grid_points(bool *write_all, char *output_file)
{

    GridLinkGuard *gloutonGlg = new GridLinkGuard;
    Grid *gridCopy = this->copy_grid_as_ptr(true);
    gridCopy->glouton_recur(gloutonGlg, nb_color_in_grid('R'), nb_color_in_grid('N'), size * size - nb_empty_cells(), 6, nullptr);
    // gloutonGlg->pretty_print();
    std::cout << "printingalfd------------------\n";
    gloutonGlg->pretty_print();
    gloutonGlg->fill_blank_all();
    // gloutonGlg->pretty_print();
    std::cout << "printingall\n";
    gloutonGlg->pretty_print();

    gloutonGlg->get_best_scores()->pretty_print();
    // gloutonGlg->pretty_print();

    /* GridLinkGuard *bestGrids = gloutonGlg->solve_all_grids();
    std::cout << "first solved ; \n";
    bestGrids->pretty_print();

    GridLinkGuard *bestGrids2 = bestGrids->solve_all_grids();
    std::cout << "second solved\n";
    bestGrids2->pretty_print();
    bestGrids2->switch_colors_with_first(this);

    // printf("%p\n", output_file);

    if (output_file != nullptr)
    {
        if (!*write_all or bestGrids2->get_size() == 1)
        {
            this->save_in_file(output_file);
        }
        else
        {
            bestGrids2->save_all_in_files(output_file);
        }
    }


    delete gloutonGlg;
    delete bestGrids; */

    // delete bestGrids2;
    // delete bestGrids3;

    /* GridLinkGuard *q = new GridLinkGuard;
    this->glouton(q, 0, 0, 0, 1);
    q->pretty_print();
    GridLinkGuard *a = q->solve_all_grids();
    a->pretty_print();
    delete a;
    delete q; */

    /* GridLinkGuard *glg = new GridLinkGuard;
    fill_blank_recur(glg, nb_empty_cells());

    printf("printing best scores\n");
    GridLinkGuard *bestScores = glg->get_best_scores();
    bestScores->print_all_scores();
    bestScores->pretty_print();

    Grid *gfdf = bestScores->get_first_grid()->copy_grid_as_ptr(true);
    GridLinkGuard *q = new GridLinkGuard;

    // //std::cout << "\n\n\n";
    // // gfdf->optimize_grid_full();
    // // gfdf->set_score_from_calculation();
    // // gfdf->print_colors_with_score();
    gfdf->optimize_grid_recur(true, q, nullptr, 2);

    GridLinkGuard *m = q->get_best_scores();
    m->pretty_print();
    m->get_first_grid()->save_in_file("solution_4_bb.txt");

    // //gfdf->print_colors();
    // //std::cout << "score = " << gfdf->calcul_score() << "\n";

    delete m;
    printf("1\n");
    delete q;
    printf("2\n");
    // delete gfdf;
    printf("3\n");
    delete bestScores;
    printf("4\n");
    delete glg;
    printf("5\n"); */
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

void Grid::find_value(int value, bool *write_all, char *output_file)
{
    Grid *bestGrid = this->copy_grid_as_ptr(true);
    bestGrid->build_grid_points(write_all, output_file);
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

void Grid::optimize_grid_clever()
{
    int nb_pena_blue = this->get_nb_penality_blue();
}

void Grid::orange_blue()
{
    char **p = colors;
    int **c = numbers;
    // int nb_blue = 0;
    int lig, col;

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

    std::cout << "avant\n";
    this->print_colors();
    glouton(penality * (-1));
    this->print_safe_colors();
    std::cout << "apres\n";

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
    printf("neg = %d, pos = %d\n", neg, pos);

    if (neg > pos)
    {
        for (int i = 0; i < size; ++i)
        {
            for (int j = 0; j < size; ++j)
            {
                nb_pos_neg_left_blue(&neg, &pos);

                if (p[i][j] != 'B' or pos >= neg)
                {
                    continue;
                }

                b = nb_color_around_cell(i, j, 'B');
                if (b >= 1)
                {
                    if (get_color(i, j + 1) == 'B')
                    {
                        if ((c[i][j] + c[i][j + 1]) > -penality)
                        {
                            p[i][j] = 'J';
                            p[i][j + 1] = 'J';
                            b--;
                        }
                    }

                    if (get_color(i + 1, j - 1) == 'B' and b >= 1)
                    {
                        if ((c[i][j] + c[i + 1][j - 1]) > -penality)
                        {
                            p[i][j] = 'J';
                            p[i + 1][j - 1] = 'J';
                            b--;
                        }
                    }

                    if (get_color(i + 1, j) == 'B' and b >= 1)
                    {
                        if ((c[i][j] + c[i + 1][j]) > -penality)
                        {
                            p[i][j] = 'J';
                            p[i + 1][j] = 'J';
                            b--;
                        }
                    }

                    if (get_color(i + 1, j + 1) == 'B' and b >= 1)
                    {
                        if ((c[i][j] + c[i + 1][j + 1]) > -penality)
                        {
                            p[i][j] = 'J';
                            p[i + 1][j + 1] = 'J';
                        }
                    }
                }
            }
        }
    }
}