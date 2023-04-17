#include <iostream>
#include <fstream>
#include <string>
#include "grid.hpp"

void initGrille();

int main(void) {
    return 0;
}

Grid* lire_fichier(std::string filename) {
    std::ifstream file(filename.c_str());
    if (!file) {    
        return nullptr;
    }
    
    int size, penality;
    file >> size;
    file >> penality;
    if (!file.good()) {
        
    }

    Grid *g = new Grid(size, penality);
    
    for (int i = 0; i < g->size; i++)
    {
        for (int j = 0; j < g->size; j++)
        {
            file >> g->numbers[i][j];
            if (!file.good()) {
                std::cerr << "Une erreur est survenue : i = " << i << ", j = " << j << "\n";
            }
        }
        
    }
    
    
    return 0;
}





int calcul_points(Grid * g)
{
    int ** c = g->numbers; 
    int ** p = g->pions; 
    int t = g->size; 
    int pena = g->penality;
    
    int score = 0;
    int nb_noirs = 0;
    int score_noirs = 0;
    int d_positif = 0;
    int d_negatif = 0;

    for (int i = 0; i < t; ++i)
    {
        for (int j = 0; j < t; ++j)
        {
            if (p[i][j] == 'J')
            {
                score += c[i][j];
                if ((p[i-1][j-1] != 'J') and (p[i-1][j] != 'J') and (p[i-1][j+1] != 'J') and (p[i][j-1] != 'J') and (p[i+1][j-1] != 'J') and (p[i+1][j] != 'J') and (p[i+1][j+1] != 'J') and (p[i+1][j] != 'J'))
                {
                    score -= pena;
                }
            }

            else if (p[i][j] == 'V')
            {
                score += p[i][j] + p[i-1][j] + p[i+1][j] + p[i][j-1] + p[i][j+1];
                if (p[i+1][j] == 'V') score -= pena;
                if (p[i+1][j+1] == 'V') score -= pena;
                if (p[i][j+1] == 'V') score -= pena;
                if (p[i-1][j+1] == 'V') score -= pena;
            }

            else if (p[i][j] == 'N')
            {
                nb_noirs += 1;
                score_noirs += p[i][j] - 1;
            }

            else if (p[i][j] == 'B')
            {
                if (c[i][j] < 0) d_negatif += 1;
                else if (c[i][j] > 0) d_positif += 1;
            }

            else if (p[i][j] == 'O')
            {
                
            }
            
            else
            {
                score += c[i][j] * (-1);
            }
        }
    }

    if (nb_noirs <= t) score_noirs = score_noirs * 2;
    score += score_noirs;

    if ((d_negatif - d_positif) > 0) score -= (d_negatif - d_positif) * pena;

    return score;
}