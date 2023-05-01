#include "grid_struct.hpp"
#include "grid.hpp"

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

void GridLinkGuard::init()
{
    this->size = 0;
    this->firstGridLink = nullptr;
    this->lastGridLink = nullptr;
}

void GridLinkGuard::addGrid(Grid *g)
{
    GridLink *gl = new GridLink;
    g->set_score_from_calculation();
    gl->grid = g;
    gl->next = this->firstGridLink;

    if (this->firstGridLink == nullptr) {
        this->lastGridLink = gl;
    }

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

        if (this->firstGridLink == nullptr) {
            this->lastGridLink = nullptr;
        }

        this->size--;
    }
}

void GridLinkGuard::clear(bool clear_grid)
{
    while (this->size != 0)
    {
        pop_first(clear_grid);
    }
    this->init();
}

/// @brief extend a GridLinkGuard with another one, consuming the given GridLinkGuard (ie, The GridLinkGuard will be deleted)
/// @param glg GridLinkGuard consumed that is added to this
void GridLinkGuard::extend(GridLinkGuard *glg)
{
    // add given GridLinkGuard to the end of the this GridLinkGuard
    this->lastGridLink->next = glg->firstGridLink;
    this->lastGridLink = glg->lastGridLink;
    glg->init();
    delete glg;

}

GridLinkGuard *GridLinkGuard::copy_as_ptr()
{
    GridLinkGuard *glg = new GridLinkGuard;
    GridLink *glTmp = this->firstGridLink;
    while (glTmp != nullptr)
    {
        glg->addGrid(glTmp->grid->copy_grid_as_ptr(true));
        glTmp = glTmp->next;
    }
    
    return glg;
}

/// @brief create a new GridLisGuard storing the best scores copying the grid(s) selected
/// @return a pointer the GridLinkGuard dynamically allocated
GridLinkGuard *GridLinkGuard::get_best_scores()
{
    if (this->firstGridLink == nullptr)
    {
        std::cout << "The set of grids is empty !\n";
    }

    // setup temp GridLink
    GridLink *gl = this->firstGridLink->next;

    // setup GridLinkGuard for the best grids
    GridLinkGuard *glgBestTemp = new GridLinkGuard(this->firstGridLink->grid);

    while (gl != nullptr)
    {
        // current focus better than our current best
        if (gl->grid->get_score() >= glgBestTemp->firstGridLink->grid->get_score())
        {
            // strictly superior, clear best
            if (gl->grid->get_score() > glgBestTemp->firstGridLink->grid->get_score())
            {
                glgBestTemp->clear(false);
            }
            glgBestTemp->addGrid(gl->grid);
        }
        gl = gl->next;
    }

    //glgBestTemp->print_all_scores();

    GridLinkGuard *glgBest = glgBestTemp->copy_as_ptr();
    printf("here\n");
    glgBest->print_all_scores();
    glgBestTemp->clear(false);
    delete glgBestTemp;

    return glgBest;
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