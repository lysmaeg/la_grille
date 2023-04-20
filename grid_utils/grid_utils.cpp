#include "grid_utils.hpp"

int Grid::get_size() const
{
    return this->size;
}

int Grid::get_penality() const
{
    return this->penality;
}

int Grid::get_supposed_score() const
{
    return supposed_score;
}

/// @brief No need to verify if the coordonates are in the table
/// @param line 
/// @param column 
/// @return the color if line and column are valid else 0
char Grid::get_color(int line, int column) const
{
    return line < 0 or line >= size or column < 0 or column >= size ? 0 : colors[line][column];
}

/// @brief No need to verify if the coordonates are in the table
/// @param line 
/// @param column 
/// @return the number if line and column are valid else 0
int Grid::get_number(int line, int column) const
{
    return line < 0 or line >= size or column < 0 or column >= size ? 0 : numbers[line][column];
}

/// @brief 
/// @return An dynamic allocation for the colors
char *Grid::colors_to_string() const
{
    int n = size * (size + 1) + 1;
    char *s = new char[n];
    memset(s, 0, n);
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            s[size * i + j + i] = get_color(i, j);
        }
        s[size * (i + 1) + i] = '\n';
    }
    return s;
}

/// @brief print colors in the command line using colors_to_string()
void Grid::print_colors() const
{
    char *s = colors_to_string();
    std::cout << s;
    delete[] s;
}

/// @brief print colors in the command line using iteration
void Grid::print_safe_colors() const
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            std::cout << colors[i][j];
        }
        std::cout << '\n';
    }
}

/// @brief check if score calculated is equal to the supposed score
/// @return true if equal, false otherwise
bool Grid::is_valid_score()
{
    return score == supposed_score;
}

/// @brief print the numbers using iteration
void Grid::print_numbers() const
{
    int max_size = get_max_size_of_int_list(numbers, size);
    std::cout << "max_size = " << max_size << '\n';
    for (int i = 0; i < get_size(); i++)
    {
        std::cout << '|';
        for (int j = 0; j < get_size(); j++)
        {
            for (int k = 0; k < max_size - size_of_int(get_number(i, j)); k++)
            {
                std::cout << ' ';
            }

            std::cout << get_number(i, j) << "|";
        }
        std::cout << '\n';
    }
}

/// @brief
/// @param line
/// @param col
/// @param all if all set to falsem only four directions are given : right, diag-right/bottom, bottom and diag-left/bottom
/// @return return all colors int lines and diagonal (line, col)
std::string *Grid::get_lines_cols(int line, int col, bool all) const
{
    std::string *s = new std::string[4];
    bool still_inside = true;

    if (all)
    {
        still_inside = true;
        for (int i = 1; i < get_size() and still_inside; i++)
        {
            still_inside = add_to_string(s, get_color(line, col - 1)) ? true : still_inside;
            still_inside = add_to_string(s, get_color(line - i, col)) ? true : still_inside;
            still_inside = add_to_string(s, get_color(line - i, col - i)) ? true : still_inside;
            still_inside = add_to_string(s, get_color(line - i, col + i)) ? true : still_inside;
        }
    }

    for (int i = 1; i < get_size() and still_inside; i++)
    {
        still_inside = add_to_string(s, get_color(line, col + 1)) ? true : still_inside;
        still_inside = add_to_string(s, get_color(line + i, col)) ? true : still_inside;
        still_inside = add_to_string(s, get_color(line + i, col + i)) ? true : still_inside;
        still_inside = add_to_string(s, get_color(line + i, col - i)) ? true : still_inside;
    }

    return s;
}
