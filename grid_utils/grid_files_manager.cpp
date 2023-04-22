#include "grid_files_manager.hpp"

/// @brief read the numbers of a grid from a file
/// @param filename the file's name
/// @return -1 on failure, 0 on success
int Grid::read_file_for_numbers(std::string filename)
{
    // open file
    std::ifstream file(filename.c_str());
    if (!file)
    {
        std::cerr << "Cannot open the file\n";
        return -1;
    }

    // is size != 0 then the tables have already been inited
    bool has_inited = size != 0;

    // get size and penality
    file >> size;
    file >> penality;
    if (!file.good())
    {
        std::cerr << "The file is not formatted correctly\n";
        return -1;
    }
    is_main_grid = true;

    // init tables from size if not inited
    if (!has_inited)
    {
        init_tables();
    }

    // get all values from file
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

/// @brief read the colors (pieces) from a file
/// @param filename the file's name
/// @return -1 on error, 0 on success
int Grid::read_file_for_colors(std::string filename)
{
    // open files
    std::ifstream file(filename.c_str());
    if (!file)
    {
        std::cerr << "Cannot open the file\n";
        return -1;
    }

    // get size and penality
    file >> size;
    file >> penality;
    file.get();
    if (!file.good())
    {
        std::cerr << "The file is not formatted correctly\n";
        return -1;
    }

    // get all colors
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            colors[i][j] = (char)file.get();
        }
        file.get();
    }

    // get supposed score given by this filem storing it for verification
    file >> supposed_score;
    return 0;
}

/// @brief write in the given file the colors and the score associated to the numbers
/// @param filename the file's name
/// @return -1 on failure, 0 on success
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

    file << '\n';
    if (!file.good())
    {
        std::cerr << "An error occured while writing the score into the file\n";
        return -1;
    }
    return 0;
}
