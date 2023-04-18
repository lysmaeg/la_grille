cmake_minimum_required(VERSION 3.0)
project(laGrille)

add_compile_options(-Wall)

add_executable(${PROJECT_NAME}
main.cpp
tools.cpp
grid.cpp
)
