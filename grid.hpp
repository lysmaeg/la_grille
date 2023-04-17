#ifndef __GRID__
#define __GRID__


class Grid
{
private:
public:
    int ** numbers;
    int ** pions;
    int size;
    int penality;
    Grid(int size, int penalite);
    ~Grid();
};

Grid::Grid(int s, int p)
{
    size = s;
    penality = p;
    numbers = new int*[size];

}

Grid::~Grid()
{
}

#endif