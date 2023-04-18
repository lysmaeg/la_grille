#include "grid.hpp"

Grid::Grid(int s, int p)
{
    size = s;
    penality = p;
    init_tables();
}

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
    delete[] numbers;
    delete[] colors;
}

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

int Grid::read_file_for_numbers(std::string filename)
{
    std::ifstream file(filename.c_str());
    if (!file)
    {
        std::cerr << "Cannot open the file\n";
        return -1;
    }

    bool has_inited = size != 0;

    file >> size;
    file >> penality;
    if (!file.good())
    {
        std::cerr << "The file is not formatted correctly\n";
        return -1;
    }

    if (!has_inited)
    {
        init_tables();
    }

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            file >> numbers[i][j];
            if (!file.good() && !file.eof())
            {
                std::cerr << "An error occured : i = " << i << ", j = " << j << "\n";
                return -1;
            }
        }
    }

    return 0;
}

int Grid::read_file_for_colors(std::string filename)
{

    std::ifstream file(filename.c_str());
    if (!file)
    {
        std::cerr << "Cannot open the file\n";
        return -1;
    }
    file >> size;
    file >> penality;
    file.get();
    if (!file.good())
    {
        std::cerr << "The file is not formatted correctly\n";
        return -1;
    }

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            colors[i][j] = (char)file.get();
        }
        file.get();
    }
    file >> supposed_score;
    return 0;
}

int Grid::save_in_file(std::string filename)
{
    std::ofstream file(filename);
    if (!file)
    {
        std::cerr << "Cannot open file to write in it\n";
        return -1;
    }

    char *s = colors_to_string();
    file << s;
    delete[] s;
    if (!file.good())
    {
        std::cerr << "An error occured while writing the colors into the file\n";
        return -1;
    }

    file << calcul_score();
    if (!file.good())
    {
        std::cerr << "An error occured while writing the score into the file\n";
        return -1;
    }
    return 0;
}

int Grid::get_supposed_score() const
{
    return supposed_score;
}

int Grid::get_size() const
{
    return this->size;
}

int Grid::get_penality() const
{
    return this->penality;
}

char Grid::get_color(int line, int column) const
{
    return line < 0 or line >= size or column < 0 or column >= size ? 0 : colors[line][column];
}

int Grid::get_number(int line, int column) const
{
    return line < 0 or line >= size or column < 0 or column >= size ? 0 : numbers[line][column];
}

char *Grid::colors_to_string()
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

void Grid::print_colors()
{
    char *s = colors_to_string();
    std::cout << s;
    delete[] s;
}

bool Grid::is_valid_score()
{
    return score == supposed_score;
}

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

    score += score_yellow + score_green + score_black + score_blue + score_orange + score_red;

    return score;
}