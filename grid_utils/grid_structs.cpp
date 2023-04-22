#include "grid_struct.hpp"
#include "grid.hpp"

void GridLinkGuard::addGrid(Grid *g)
{
    GridLink *gl = new GridLink;
    g->set_score_from_calculation();
    gl->grid = g;
    gl->next = this->firstGridLink;
    this->firstGridLink = gl;
    this->size++;
}

void GridLinkGuard::pop_first(bool clear_grid)
{
    if (size != 0 and this->firstGridLink != nullptr)
    {
        GridLink *save = this->firstGridLink;
        this->firstGridLink = save->next;
        if (clear_grid)
        {
            delete save->grid;
        }
        delete save;

        this->size--;
    }
}

void GridLinkGuard::clear(bool clear_grid)
{
    while (this->size != 0)
    {
        pop_first(clear_grid);
    }
    this->size = 0;
    this->firstGridLink = nullptr;
}

GridLinkGuard *GridLinkGuard::get_best_scores()
{
    if (size == 0)
    {
        std::cout << "The set of grids is empty !\n";
    }

    // setup temp GridLink
    GridLink *gl = this->firstGridLink->next;

    // setup GridLinkGuard for the best grids
    GridLinkGuard *glg = new GridLinkGuard(this->firstGridLink->grid);

    while (gl != nullptr)
    {

        if (gl->grid->get_score() >= glg->firstGridLink->grid->get_score())
        {
            if (gl->grid->get_score() > glg->firstGridLink->grid->get_score())
            {
                glg->clear(false);
            }
            glg->addGrid(gl->grid);
        }
        gl = gl->next;
    }
    return glg;
}

std::string *GridLinkGuard::get_pretty_print()
{
    std::string *s = new std::string;
    GridLink *gl = this->firstGridLink;
    
    if (this->firstGridLink != nullptr) {
        *s = this->firstGridLink->grid->get_score();
        while (gl != nullptr) {
            *s += ", " + std::to_string(gl->grid->get_score());
            gl = gl->next;
        }
    }
    return s;
}

void GridLinkGuard::pretty_print() const {
    std::cout << "\n=====-=====-=====\n";
    GridLink *gl = this->firstGridLink;
    if (gl != nullptr) {
        while (gl != nullptr) {
            gl->grid->print_colors_with_score();

            gl = gl->next;
        }
    }
}

void GridLinkGuard::print_all_scores()
{
    GridLink *gl = this->firstGridLink;
    while (gl != nullptr)
    {
        std::cout << gl->grid->get_score() << '\n';
        gl = gl->next;
    }
}

GridLinkGuard::GridLinkGuard()
{
    this->size = 0;
    this->firstGridLink = nullptr;
}

GridLinkGuard::GridLinkGuard(Grid *g)
{
    this->size = 1;
    this->firstGridLink = new GridLink;
    this->firstGridLink->grid = g;
}

GridLinkGuard::~GridLinkGuard()
{
    this->clear(true);
}