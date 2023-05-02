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

Grid Grid::copy_grid(bool copy_colors)
{
    Grid g = Grid(this, copy_colors);
    return g;
}

Grid *Grid::copy_grid_as_ptr(bool copy_colors)
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

/// @brief
/// @param line
/// @param col
/// @return Return the score of the orthogonals cell + the cell itself
int Grid::get_cross_score(int line, int col) const
{
    return get_number(line, col) + get_number(line, col - 1) + get_number(line, col + 1) + get_number(line - 1, col) + get_number(line + 1, col);
}

int Grid::calculate_red_piece(int line, int col) const
{
    return -get_number(line, col);
}

int Grid::calculate_yellow_piece(int line, int col) const
{
    return get_number(line, col) - (nb_color_around_cell(line, col, 'J') == 0 ? 0 : penality);
}

int Grid::calculate_green_piece(int line, int col) const
{
    int point = get_cross_score(line, col);
    for (long unsigned int i = 0; i < get_around_colors(line, col).length(); ++i)
    {
        if (get_around_colors(line, col)[i] == 'V')
        {
            point -= penality;
        }
    }
    return point;
}

int Grid::calculate_orange_piece(int line, int col, int all) const
{
    return -nb_colors_in_lines_cols(line, col, all, 'O') * penality;
}

/// @brief calculate the score given coordinates
/// @param line
/// @param col
/// @param color the color supposed to be there at these coordinates
/// @param all if all set to false, only S, SO, SW and W will be analysed
/// @return Return the score at this cell following the given color
int Grid::calcul_score_place(int line, int col, char color, bool all) const
{
    int point = 0;
    switch (color)
    {
    case 'J':
        point = calculate_yellow_piece(line, col);
        break;

    case 'V':
        point = calculate_green_piece(line, col);
        break;

    case 'N':
        point = get_number(line, col) - 1;
        break;

    case 'O':
        point = calculate_orange_piece(line, col, all);
        break;

    case 'R':
        point = calculate_red_piece(line, col);
        break;

    case 'B':
        break;

    default:
        break;
    }

    return point;
}

/// @brief calculate the whole score of the grid
/// @return Return the score of this Grid
int Grid::calcul_score() const
{
    int **c = numbers;
    char **p = colors;
    int t = size;
    int pena = penality;

    int score = 0;
    int nb_black = 0;
    int d_positif = 0;
    int d_negatif = 0;
    int score_black = 0;
    int score_yellow = 0;
    int score_green = 0;
    int score_orange = 0;
    int score_red = 0;
    int score_blue = 0;

    for (int l = 0; l < t; ++l)
    {
        for (int col = 0; col < t; ++col)
        {
            if (p[l][col] == 'J')
            {
                score_yellow += c[l][col];
                if ((get_color(l - 1, col - 1) != 'J') and (get_color(l - 1, col) != 'J') and (get_color(l - 1, col + 1) != 'J') and (get_color(l, col - 1) != 'J') and (get_color(l + 1, col - 1) != 'J') and (get_color(l + 1, col) != 'J') and (get_color(l + 1, col + 1) != 'J') and (get_color(l + 1, col) != 'J'))
                {
                    score_yellow -= pena;
                }
            }

            else if (p[l][col] == 'V')
            {
                score_green += c[l][col] + get_number(l - 1, col) + get_number(l + 1, col) + get_number(l, col - 1) + get_number(l, col + 1);
                score_green -= pena * ((get_color(l + 1, col) == 'V') + (get_color(l + 1, col + 1) == 'V') + (get_color(l, col + 1) == 'V') + (get_color(l + 1, col - 1) == 'V'));
                /* if (get_color(i + 1, j) == 'V')
                    score_green -= pena;
                if (get_color(i + 1, j + 1) == 'V')
                    score_green -= pena;
                if (get_color(i + 1, j + 1) == 'V')
                    score_green -= pena;
                if (get_color(i + 1, j - 1) == 'V')
                    score_green -= pena; */
            }

            else if (p[l][col] == 'N')
            {
                nb_black += 1;
                score_black += c[l][col] - 1;
            }

            else if (p[l][col] == 'B')
            {
                if (c[l][col] < 0)
                    d_negatif += 1;
                else if (c[l][col] > 0)
                    d_positif += 1;
            }

            else if (p[l][col] == 'O')
            {
                int a = l, b = col, c = l, d = col;

                for (int k = l + 1; k < t; ++k)
                {
                    if (p[k][col] == 'O')
                        score_orange -= pena;
                    c--;
                    d++;
                    if (get_color(c, d) == 'O')
                        score_orange -= pena;
                }

                for (int k = col + 1; k < t; ++k)
                {
                    if (p[l][k] == 'O')
                        score_orange -= pena;
                    a++;
                    b++;
                    if (get_color(a, b) == 'O')
                        score_orange -= pena;
                }
            }

            else if (p[l][col] == 'R')
            {
                score_red += -c[l][col];
            }
        }
    }

    if (nb_black <= t)
        score_black = score_black * 2;

    if (d_negatif > d_positif)
        score_blue -= (d_negatif - d_positif) * pena;

    // std::cout << "Yellow : " << score_yellow << ", Green : " << score_green << ", Black : " << score_black << ", Blue : " << score_blue << ", Orange : " << score_orange << ", Red : " << score_red << std::endl;

    score += score_yellow + score_green + score_black + score_blue + score_orange + score_red;

    return score;
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
    print_tttab((const int ***)scores_tab, size, size, 4);

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
    Grid *gfdf = bestScores->firstGridLink->grid;

    std::cout << "\n\n\n";
    gfdf->optimize_grid();
    gfdf->print_colors();
    std::cout << "score = " << gfdf->calcul_score() << "\n";

    bestScores->clear(false);
    glg->clear(true);
}

void Grid::brute_force_recur(GridLinkGuard *glg, int pieces_left, int *nb_thread, bool red_posed)
{
    if (pieces_left == 0)
    {
        glg->addGrid(this->copy_grid_as_ptr(true));
        return;
    }

    int x, y;
    if (this->get_coordinates_empty_cell(&x, &y) == -1)
    {
        std::cerr << "An error occured...\n";
        exit(EXIT_FAILURE);
    }

    printf("here x = %d, y = %d, nbth = %d\n", x, y, *nb_thread);
    this->print_colors();

    if (!red_posed)
    {
        this->colors[x][y] = 'R';
        brute_force_recur(glg, pieces_left - 1, nb_thread, true);
        this->colors[x][y] = 'X';
    }
    else
    {
        if (pieces_left >= 4)
        {
            std::string s = "BVNJO";
            std::thread *threadIdTab = new std::thread[s.length()];

            for (int i = 0; i < (int)s.length(); i++)
            {
                if (*nb_thread < NB_MAX_THREAD)
                {
                    (*nb_thread)++;
                    threadIdTab[i] = std::thread(&Grid::brute_force_launcher, this->copy_grid_as_ptr(true), glg, pieces_left, nb_thread, red_posed, x, y, s[i]);

                    // wait all thread to be finished
                    for (int i = 0; i < (int)s.length(); i++)
                    {
                        threadIdTab[i].join();
                        (*nb_thread)--;
                    }
                }
                else
                {
                    this->colors[x][y] = s[i];
                    brute_force_recur(glg, pieces_left - 1, nb_thread, red_posed);
                    this->colors[x][y] = 'X';
                }
            }

            delete[] threadIdTab;
        }
        else
        {
            std::string s = "BVNJO";
            for (int i = 0; i < (int)s.length(); i++)
            {
                this->colors[x][y] = s[i];
                brute_force_recur(glg, pieces_left - 1, nb_thread, red_posed);
                this->colors[x][y] = 'X';
            }
        }
    }
}

void Grid::brute_force_launcher(GridLinkGuard *glg, int pieces_left, int *nb_thread, bool red_posed, int x, int y, char c)
{
    // printf("thread started x = %d, y = %d, c = %c\n", x, y, c);
    GridLinkGuard *glgThread = new GridLinkGuard;

    this->colors[x][y] = c;
    brute_force_recur(glgThread, pieces_left - 1, nb_thread, red_posed);
    this->colors[x][y] = 'X';
    printf("thread finished\n");

    GridLinkGuard *bestScoresThread = glgThread->get_best_scores();
    printf("before extending\n");
    glg->extend(bestScoresThread);
    printf("after extend\n");

    delete glgThread;
}

void Grid::brute_force()
{
    GridLinkGuard *glg = new GridLinkGuard;
    int nb_thread = 0;
    this->brute_force_recur(glg, nb_empty_cells(), &nb_thread, false);
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

void Grid::optimize_grid(int line, int column, int limit_recur)
{
    printf("called\n");

    if (limit_recur == 0)
    {
        return;
    }

    char **p = colors;
    // int **c = numbers;

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

            if (line == i and column == j)
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

            if (col == 'B' or col == 'O' or scores_tab[i][j][co] < 0)
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
                    optimize_grid(i, j, limit_recur - 1);
                }
            }
        }
    }

    this->delete_scores_tab(scores_tab);
}