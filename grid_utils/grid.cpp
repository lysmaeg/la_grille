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
    size = g->size;
    penality = g->penality;
    numbers = g->numbers;
    if (!copy_colors)
    {
        colors = new char *[size];
        for (int i = 0; i < size; i++)
        {
            colors[i] = new char[size];
            memset(colors[i], 'X', size);
        }
    }
    else
    {
        colors = copy_ttab_char((const char **)g->colors, size, size, &size, &size);
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
            if (numbers[i][j] == c)
            {
                nb++;
            }
        }
    }
    return nb;
}

void Grid::nb_pos_neg_left(int *neg, int *pos) const
{
    *pos = 0;
    *neg = 0;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (numbers[i][j] > 0) {
                *pos += 1;
            } else if (numbers[i][j] < 0) {
                *neg += 1;
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

void Grid::fill_blank(GridLinkGuard *glg, int pieces_left)
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
    fill_blank(glg, pieces_left - 1);
    colors[x][y] = 'O';
    fill_blank(glg, pieces_left - 1);
    colors[x][y] = 'X';
}

// this function does not generate the best grid, the around of green is not really taken in mind
void Grid::build_grid_points()
{
    std::string s = "RVNJ";
    const int black = 2, green = 1, yellow = 3, red = 0;
    int max_cur, color_cur;
    int ***scores_tab = build_scores_tab(black, green, yellow, red, true);
    couple coordinates_max;

    /* int nb_neg_left = 0, nb_pos_left = 0, current_neg_blue_point = 0;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (numbers[i][j] > 0)
            {
                nb_pos_left++;
            }
            else if (numbers[i][j] < 0)
            {
                nb_neg_left++;
            }
        }
    }

    current_neg_blue_point = abs(std::min(0, nb_pos_left - nb_neg_left)); */

    int nb_black = 0, nb_red = 0;
    int placed_pieces = 0;

    do
    {
        max_cur = 0;
        color_cur = 0;
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
            /* print_colors();
            printf("current = %d\n", current_neg_blue_point);
            if (numbers[coordinates_max.line][coordinates_max.column] > 0)
            {
                nb_pos_left--;
            }
            else if (numbers[coordinates_max.line][coordinates_max.column] < 0)
            {
                nb_neg_left--;
            }
            printf("after current = %d\n", current_neg_blue_point); */

            /* current_neg_blue_point = abs(std::min(0, nb_pos_left - nb_neg_left));
            if (current_neg_blue_point > 0 and scores_tab[coordinates_max.line][coordinates_max.column][color_cur] - penality <= 0)
            {
                colors[coordinates_max.line][coordinates_max.column] = 'B';
                nb_pos_left++;
                current_neg_blue_point = abs(std::min(0, nb_pos_left - nb_neg_left));
                continue;
            } */

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
                if (nb_color_around_cell(coordinates_max.line, coordinates_max.column, 'J') == 0)
                // cloch
                {
                    for (int i = -1; i < 2; i++)
                    {
                        for (int j = -1; j < 2; j++)
                        {
                            if (coordinates_max.line + i >= 0 and coordinates_max.line + i < size and coordinates_max.column + j >= 0 and coordinates_max.column + j < size)
                            {
                                scores_tab[coordinates_max.line + i][coordinates_max.column + j][yellow] += penality;
                            }
                        }
                    }
                    // print_tttab((const int ***)infoTab, size, size, 4);

                    // printf("at the end\n");
                }

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

    } while (placed_pieces <= size * size and max_cur != 0);
    printf("placed_pieces = %d, max_cur %d\n", placed_pieces, max_cur);

    delete_scores_tab(scores_tab);

    GridLinkGuard *glg = new GridLinkGuard;
    fill_blank(glg, nb_empty_cells());

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
    gfdf->optimize_grid_recur(true, q, -1, -1, 3);

    GridLinkGuard *m = q->get_best_scores();
    m->pretty_print();
    m->get_first_grid()->save_in_file("solution_4_b.txt");

    // //gfdf->print_colors();
    // //std::cout << "score = " << gfdf->calcul_score() << "\n";

    delete m;
    delete q;
    delete gfdf;
    delete bestScores;
    delete glg;
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

void Grid::optimize_grid_recur(bool is_main, GridLinkGuard *glg, coupleLink *cl, int limit_recur)
{
    //printf("called\n");

    if (limit_recur == 0)
    {
        glg->addGrid(this);
        //std::cout << "size = " << glg->get_size() << "\n";
        return;
    }

    char **p = colors;
    // int **c = numbers;

    int co = -1, col;
    // int score = calcul_score();
    int score_yellow = INT32_MIN, score_black = INT32_MIN, score_blue = INT32_MIN, score_green = INT32_MIN, score_orange = INT32_MIN;

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

            for (int k = 0; k < 4; ++k)
            {
                if (col == s[k])
                {
                    co = k;
                    break;
                }
            }

            /* if (col == 'B' or col == 'O' or scores_tab[i][j][co] < 0 or c[i][j] > 0)
            { */
                if (col != 'J')
                {
                    p[i][j] = 'J';

                    Grid *g = this->copy_grid_as_ptr(true);
                    score_yellow = g->calcul_score();
                    // if (score_yellow <= score)
                    // {
                        g->optimize_grid_recur(false, glg, add_first_CL(cl, i, j), limit_recur - 1);
                    // }
                }

                if (col != 'V')
                {
                    p[i][j] = 'V';

                    Grid *g = this->copy_grid_as_ptr(true);
                    score_green = g->calcul_score();
                    // if (score_green <= score)
                    // {
                        g->optimize_grid_recur(false, glg, add_first_CL(cl, i, j), limit_recur - 1);
                    // }
                }

                if (col != 'N' and nb_black < size)
                {
                    p[i][j] = 'N';
                    Grid *g = this->copy_grid_as_ptr(true);
                    score_black = g->calcul_score();
                    // if (score_black <= score)
                    // {
                        g->optimize_grid_recur(false, glg, add_first_CL(cl, i, j), limit_recur - 1);
                    // }
                }

                if (col != 'B')
                {
                    p[i][j] = 'B';
                    Grid *g = this->copy_grid_as_ptr(true);
                    score_blue = g->calcul_score();
                    // if (score_blue <= score)
                    // {
                        g->optimize_grid_recur(false, glg, add_first_CL(cl, i, j), limit_recur - 1);
                    // }
                }

                if (col != 'O')
                {
                    p[i][j] = 'O';
                    Grid *g = this->copy_grid_as_ptr(true);
                    score_orange = g->calcul_score();
                    // if (score_orange <= score)
                    // {
                        g->optimize_grid_recur(false, glg, add_first_CL(cl, i, j), limit_recur - 1);
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

    this->delete_scores_tab(scores_tab);
    glg->addGrid(this);
}


void Grid::orange_blue()
{
    char **p = colors;
    int **c = numbers;
    //int nb_blue = 0;
    int lig, col;
    
    // placement des bleu sur les positifs
    for(int i = 0; i < size; ++i)
    {
        for(int j = 0; j < size; ++j)
        {
            if (p[i][j] == 'X' and c[i][j] > 0)
            {
                p[i][j] = 'B';
                //nb_blue ++;
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
                    if (p[i][k] == 'X') p[i][k] = 'Q';
                }
                for (int k = 0; k < size; ++k)
                {
                    if (p[k][j] == 'X') p[k][j] = 'Q';
                }

                // premiere diagonale
                if (i > j)
                {
                    for (int k = 0; k < size-(i-j); ++k)
                    {
                        if (p[k][i-j + k] == 'X') p[k][i-j + k] = 'Q';
                    }
                }
                else
                {
                    for (int k = 0; k < size-(j -i); ++k)
                    {
                        if (p[k][j-i + k] == 'X') p[k][j-i + k] = 'Q';
                    }
                }

                int jb = i+j;
                // deuxieme diagonale
                if (jb < size-1)
                {
                    for (int k = 0; k < jb+1; ++k)
                    {
                        if (p[jb-k][k] == 'X') p[jb-k][k] = 'Q';
                    }
                }
                else
                {
                    for (int k = 0; k < i+j+1; ++k)
                    {
                        if (p[size-1-k][jb-(size-1)+k] == 'X') p[size-1-k][jb-(size-1)+k] = 'Q';
                    }
                }
            }
        }
    }
    if (p[0][0] == 'X') p[0][0] = 'O';



    // placement du reste des bleu
    for(int i = 0; i < size; ++i)
    {
        for(int j = 0; j < size; ++j)
        {
            if (p[i][j] == 'X' or p[i][j] == 'Q')
            {
                p[i][j] = 'B';
            }
        }
    }
    
}