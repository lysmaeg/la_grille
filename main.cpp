#include "grid_utils/grid.hpp"
#include <cstring>

void print_help_message() {
  std::cout << "Welcome in the home page : \n\nThis command has to way to be "
               "used :\n\n";
  std::cout << "1) GRID SOLVER\n\n";
  std::cout << "./exec [-h | o | t | w] INPUT_FILE\n\n";
  std::cout << "=> -o : specify the output file, if not given the output will "
               "only be in the terminal\n";
  std::cout << "=> -t : specify the time that the program requires\n";
  std::cout << "=> -h / --help : print this help message\n";
  std::cout << "=> -w / --write-all : Write all best solutions found into "
               "several files\n";
  std::cout << "\n2) GRID GENERATOR\n\n";
  std::cout << "./exec -g / --generate-numbers [-h | p | s | m ] OUTPUT_FILE , "
               "the following options are mandatory\n\n";
  std::cout << "=> -g / --generate-numbers : mandatory for the generation of "
               "the random numbers of a grid\n";
  std::cout
      << "=> -s / --size : specify the size of the grid to be generated\n";
  std::cout << "=> -p / --penality : specify the penality of the grid to be "
               "generated\n";
  std::cout << "=> -m / --minmax : specify the minmum and the maximum of the "
               "values in the grid\n";
}

Grid *manage_arguments(int argc, char **args, options *opts) {
  bool write_all_given = false;
  opts->write_all = false;
  opts->solve_grid = true;
  Grid *g;
  char *file_given = NULL;

  int size = 0, penality = 0, mi = 0, ma = 0;

  for (int i = 1; i < argc; i++) {
    char *t = args[i];
    printf("t = %s\n", t);
    if (strcmp(t, "-o") == 0) {
      i++;
      opts->output_file = args[i];
    } else if (strcmp("-h", t) == 0 or strcmp("--help", t) == 0) {
      print_help_message();
      exit(0);
    } else if (strcmp("-v", t) == 0 or strcmp("--verbose", t) == 0) {
      opts->verbose = true;
    } else if (strcmp("-w", t) == 0 or strcmp("--write-all", t) == 0) {
      if (!write_all_given) {
        write_all_given = true;
        opts->write_all = true;
      }
    } else if (strcmp("-s", t) == 0 or strcmp("--size", t) == 0) {
      if (++i < argc) {
        size = std::stoi(args[i]);
      } else {
        std::cerr << "Please specify a parameters after the option " << t
                  << '\n';
      }
    } else if (strcmp("-t", t) == 0) {
      if (++i < argc) {
        opts->secs = std::stoi(args[i]);
      } else {
        std::cerr << "Please specify a parameters after the option " << t
                  << '\n';
      }
    } else if (strcmp("-p", t) == 0 or strcmp("--penality", t) == 0) {

      if (++i < argc) {
        penality = std::stoi(args[i]);
      } else {
        std::cerr << "Please specify a parameter after the option " << t
                  << '\n';
      }
    } else if (strcmp("-m", t) == 0 or strcmp("--minmax", t) == 0) {
      if (++i < argc) {
        mi = std::stoi(args[i]);
      } else {
        std::cerr << "Please provide the minimum and the maximum\n";
        exit(0);
      }

      if (++i < argc) {
        ma = std::stoi(args[i]);
      } else {
        std::cerr << "Please provide the maximum\n";
        exit(0);
      }
    } else if (strcmp("-g", t) == 0 or strcmp("--generate-numbers", t) == 0) {
      opts->solve_grid = false;
    } else {
      if (file_given == NULL) {
        file_given = t;
      } else {
        std::cerr << "You cannot give the input file twice, exiting\n";
        print_help_message();
        exit(0);
      }
    }
  }

  if (file_given == NULL) {
    std::cout << "You did not give the input file, exiting\n";
    print_help_message();
    exit(EXIT_SUCCESS);
  } else {
    if (opts->solve_grid) {
      g = new Grid(file_given, "", false);
      if (g->read_file_for_numbers(file_given) <= -1) {
        std::cerr << "An error occured, cannot read the file " << file_given
                  << "\n";
        exit(EXIT_SUCCESS);
      }
    } else {
      if (size == 0 or penality == 0 or (mi == 0 and ma == 0)) {
        std::cerr << "For the generation of a random grid filled with numbers, "
                     "please provide a size and a penality with the options -s "
                     "and -p, you also have to set the options -m for the "
                     "minimum and the maximum values\n";
        exit(0);
      }
      g = new Grid(size, penality);
      g->generate_random_grid_numbers(mi, ma);
      g->write_numbers_into_file(file_given);
    }
  }
  return g;
}

int main(int argc, char **args) {
  srand(time(NULL));

  options opts;
  Grid *g = manage_arguments(argc, args, &opts);
  if (opts.solve_grid) {
    g->print_numbers();
    g->build_grid_points(&opts);
  }

  return 0;

  // 12413
  // 23931 avec opti full
}
