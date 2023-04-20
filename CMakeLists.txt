cmake_minimum_required(VERSION 3.0)
project(laGrille)

add_compile_options(-Wall -O3)

add_executable(${PROJECT_NAME}
grid_utils/grid.cpp
main.cpp
tools.cpp
grid_utils/grid_files_manager.cpp
grid_utils/grid_utils.cpp
)
