#include "grid_utils.hpp"
#include "grid.hpp"

int Grid::get_size() const { return this->size; }

int Grid::get_penality() const { return this->penality; }

int Grid::get_supposed_score() const { return supposed_score; }

int Grid::get_score() const { return score; }

void Grid::set_score_from_calculation() { score = calcul_score(); }

void Grid::delete_colors() {
  for (int i = 0; i < this->size; i++) {
     delete [] this->colors[i]; 
  }
  delete [] this->colors;
}

/// @brief No need to verify if the coordonates are in the table
/// @param line
/// @param column
/// @return the color if line and column are valid else 0
char Grid::get_color(int line, int column) const {
  return line < 0 or line >= size or column < 0 or column >= size
             ? 0
             : colors[line][column];
}

/// @brief No need to verify if the coordonates are in the table
/// @param line
/// @param column
/// @return the number if line and column are valid else 0
int Grid::get_number(int line, int column) const {
  return line < 0 or line >= size or column < 0 or column >= size
             ? 0
             : numbers[line][column];
}

/// @brief
/// @return An dynamic allocation for the colors
char *Grid::colors_to_string() const {
  int n = size * (size + 1) + 1;
  char *s = new char[n];
  memset(s, 0, n);
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      s[size * i + j + i] = get_color(i, j);
    }
    s[size * (i + 1) + i] = '\n';
  }
  return s;
}

/// @brief print colors in the command line using colors_to_string()
void Grid::print_colors() const {
  char *s = colors_to_string();
  std::cout << s;
  delete[] s;
}

/// @brief print colors in the command line using iteration
void Grid::print_safe_colors() const {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      std::cout << colors[i][j];
    }
    std::cout << '\n';
  }
}

int Grid::nb_empty_cells() const {
  int nb = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (colors[i][j] == 'X') {
        nb++;
      }
    }
  }

  return nb;
}
int Grid::get_coordinates_empty_cell(int *x, int *y) const {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (colors[i][j] == 'X') {
        *x = i;
        *y = j;
        return 0;
      }
    }
  }
  return -1;
}

void Grid::update_green_scores_tab(int ***scores_tab, int line, int column,
                                   int index) {
  for (int i = -1; i < 2; i++) {
    for (int j = -1; j < 2; j++) {
      // printf("l+i = %d, c+j = %d\n", line + i, column + j);
      if (line + i >= 0 and line + i < size and column + j >= 0 and
          column + j < size) {
        scores_tab[line + i][column + j][index] =
            this->calculate_green_piece(line + i, column + j);
      }
    }
  }
}

void Grid::update_yellow_scores_tab(int ***scores_tab, int line, int column,
                                    int index) {
  for (int i = -1; i < 2; i++) {
    for (int j = -1; j < 2; j++) {
      if (line + i >= 0 and line + i < size and column + j >= 0 and
          column + j < size) {
        scores_tab[line + i][column + j][index] =
            this->calculate_yellow_piece(line + i, column + j);
      }
    }
  }
}

void Grid::print_colors_with_score() const {
  this->print_colors();
  std::cout << "With a score of " << this->get_score()
            << ".\n=====-=====-=====\n";
}

/// @brief check if score calculated is equal to the supposed score
/// @return true if equal, false otherwise
bool Grid::is_valid_score() { return score == supposed_score; }

void Grid::switch_colors(Grid *g) {
  char **aux = g->colors;
  g->colors = this->colors;
  this->colors = aux;
}

int Grid::get_nb_penality_blue() {
  int nb_blue_pos = 0, nb_bleu_neg = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (this->colors[i][j] == 'B') {
        if (this->numbers[i][j] > 0) {
          nb_blue_pos++;
        } else if (this->numbers[i][j] < 0) {
          nb_bleu_neg++;
        }
      }
    }
  }
  return abs(std::max(nb_bleu_neg - nb_blue_pos, 0));
}

/// @brief print the numbers using iteration
void Grid::print_numbers() const {
  int max_size = get_max_size_of_int_list(numbers, size);
  for (int i = 0; i < get_size(); i++) {
    std::cout << '|';
    for (int j = 0; j < get_size(); j++) {
      for (int k = 0; k < max_size - size_of_int(get_number(i, j)); k++) {
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
/// @param all if all set to falsem only four directions are given : right,
/// diag-right/bottom, bottom and diag-left/bottom
/// @return return all colors int lines and diagonal (line, col)
std::string *Grid::get_lines_cols(int line, int col, bool all) const {
  std::string *s = new std::string[4];
  bool still_inside = true;

  if (all) {
    still_inside = true;
    for (int i = 1; i < get_size() and still_inside; i++) {
      still_inside =
          add_to_string(s, get_color(line, col - i)) ? true : still_inside;
      still_inside =
          add_to_string(s, get_color(line - i, col)) ? true : still_inside;
      still_inside =
          add_to_string(s, get_color(line - i, col - i)) ? true : still_inside;
      still_inside =
          add_to_string(s, get_color(line - i, col + i)) ? true : still_inside;
    }
  }

  for (int i = 1; i < get_size() and still_inside; i++) {
    still_inside =
        add_to_string(s, get_color(line, col + i)) ? true : still_inside;
    still_inside =
        add_to_string(s, get_color(line + i, col)) ? true : still_inside;
    still_inside =
        add_to_string(s, get_color(line + i, col + i)) ? true : still_inside;
    still_inside =
        add_to_string(s, get_color(line + i, col - i)) ? true : still_inside;
  }

  return s;
}

void Grid::clear_colors() {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      colors[i][j] = 'X';
    }
  }
}

/// @brief an helpful function for the yellow
/// @param line
/// @param col
/// @return Return all the colors as a string around the given coordinates
std::string Grid::get_around_colors(int line, int col) const {
  std::string s = "";
  char c;
  for (int i = -1; i < 2; i++) {
    for (int j = -1; j < 2; j++) {
      if (i == 0 and j == 0)
        continue;
      c = get_color(line + i, col + j);
      if (c != 0) {
        s += c;
      }
    }
  }
  return s;
}

/// @brief an helpful function for the orange
/// @param line
/// @param col
/// @param all if all set to false only the four directions : SO, S, SW and W
/// will be analysed
/// @param color the color to be tested for counting
/// @return the amount of the same color in the diagonals, columns and lines
int Grid::nb_colors_in_lines_cols(int line, int col, int all,
                                  char color) const {
  int nb = 0;
  std::string *s = get_lines_cols(line, col, all);
  for (int i = 0; i < 4; ++i) {
    for (long unsigned int j = 0; j < s[i].length(); ++j)
      if (s[i][j] == color) {
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
int Grid::nb_color_around_cell(int line, int col, char color) const {
  std::string s = get_around_colors(line, col);
  int nb = 0;
  for (long unsigned int i = 0; i < s.length(); i++) {
    if (s[i] == color) {
      nb++;
    }
  }
  return nb;
}

int Grid::nb_color_in_grid(char c) const {
  int nb = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (colors[i][j] == c) {
        nb++;
      }
    }
  }
  return nb;
}

void Grid::nb_pos_neg_left_blue(int *neg, int *pos) const {
  *pos = 0;
  *neg = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (this->colors[i][j] == 'B') {
        if (numbers[i][j] > 0) {
          *pos += 1;
        } else if (numbers[i][j] < 0) {
          *neg += 1;
        }
      }
    }
  }
}
