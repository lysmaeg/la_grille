#include "grid.hpp"

Grid::Grid(int s, int p)
{
    size = s;
    penality = p;
    init_tables();
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
        delete[] numbers[i];
        delete[] colors[i];
    }

    delete[] numbers;
    delete[] colors;
}

/// @brief init the tables of numbers and colors (dynamic allocation)
void Grid::init_tables()
{
    numbers = new int *[size];
    colors = new char *[size];
    for (int i = 0; i < size; i++)
    {
        numbers[i] = new int[size];
        colors[i] = new char[size];
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

/// @brief
/// @param line
/// @param col
/// @return Return the score of the orthogonals cell + the cell itself
int Grid::get_cross_score(int line, int col) const
{
    return get_number(line, col) + get_number(line - 1, col - 1) + get_number(line + 1, col + 1) + get_number(line - 1, col + 1) + get_number(line + 1, col - 1);
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
int Grid::calcul_score_place(int line, int col, char color, bool all)
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

    for (int i = 0; i < t; ++i)
    {
        for (int j = 0; j < t; ++j)
        {
            if (p[i][j] == 'J')
            {
                score_yellow += c[i][j];
                if ((get_color(i - 1, j - 1) != 'J') and (get_color(i - 1, j) != 'J') and (get_color(i - 1, j + 1) != 'J') and (get_color(i, j - 1) != 'J') and (get_color(i + 1, j - 1) != 'J') and (get_color(i + 1, j) != 'J') and (get_color(i + 1, j + 1) != 'J') and (get_color(i + 1, j) != 'J'))
                {
                    score_yellow -= pena;
                }
            }

            else if (p[i][j] == 'V')
            {
                score_green += c[i][j] + get_number(i - 1, j) + get_number(i + 1, j) + get_number(i, j - 1) + get_number(i, j + 1);
                if (get_color(i + 1, j) == 'V')
                    score_green -= pena;
                if (get_color(i + 1, j + 1) == 'V')
                    score_green -= pena;
                if (get_color(i, j + 1) == 'V')
                    score_green -= pena;
                if (get_color(i + 1, j - 1) == 'V')
                    score_green -= pena;
            }

            else if (p[i][j] == 'N')
            {
                nb_black += 1;
                score_black += c[i][j] - 1;
            }

            else if (p[i][j] == 'B')
            {
                if (c[i][j] < 0)
                    d_negatif += 1;
                else if (c[i][j] > 0)
                    d_positif += 1;
            }

            else if (p[i][j] == 'O')
            {
                int a = i, b = j, c = i, d = j;

                for (int k = i + 1; k < t; ++k)
                {
                    if (p[k][j] == 'O')
                        score_orange -= pena;
                    c--;
                    d++;
                    if (get_color(c, d) == 'O')
                        score_orange -= pena;
                }

                for (int k = j + 1; k < t; ++k)
                {
                    if (p[i][k] == 'O')
                        score_orange -= pena;
                    a++;
                    b++;
                    if (get_color(a, b) == 'O')
                        score_orange -= pena;
                }
            }

            else
            {
                score_red += c[i][j] * (-1);
            }
        }
    }

    if (nb_black <= t)
        score_black = score_black * 2;

    if (d_negatif > d_positif)
        score_blue -= (d_negatif - d_positif) * pena;

    std::cout << "Yellow : " << score_yellow << ", Green : " << score_green << ", Black : " << score_black << ", Blue : " << score_blue << ", Orange : " << score_orange << ", Red : " << score_red << std::endl;

    score += score_yellow + score_green + score_black + score_blue + score_orange + score_red;

    return score;
}

void Grid::build_grid_points()
{
    int black = 0, green = 1, yellow = 2, red = 3;
    int ***infoTab = new int **[size];
    for (int i = 0; i < size; i++)
    {
        infoTab[i] = new int *[size];
        for (int j = 0; j < size; j++)
        {
            infoTab[i][j] = new int[4];
        }
    }

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            infoTab[i][j][black] = calcul_score_place(i, j, 'N', true);
            infoTab[i][j][green] = calcul_score_place(i, j, 'V', true);
            infoTab[i][j][yellow] = calcul_score_place(i, j, 'J', true);
            infoTab[i][j][red] = calcul_score_place(i, j, 'R', true);
        }
    }

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            delete[] infoTab[i][j];
        }
        delete[] infoTab[i];
    }
    delete[] infoTab;
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