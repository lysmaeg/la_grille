#include "grid.hpp"

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
