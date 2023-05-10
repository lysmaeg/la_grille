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

const int GridLinkGuard::get_size() const
{
    return this->size;
}

void GridLinkGuard::addGrid(Grid *g)
{
    GridLink *gl = new GridLink;
    g->set_score_from_calculation();
    gl->grid = g;
    gl->next = this->firstGridLink;

    if (this->firstGridLink == nullptr)
    {
        this->lastGridLink = gl;
    }

    this->firstGridLink = gl;
    this->size++;

    // clear Guard and get only the best of them
    if (size >= this->clear_at)
    {
        this->convert_to_best_scores();
    }
}

/// @brief remove the first item of the GridLinkGuard
/// @param clear_grid if true, the first grid will be deleted
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

        if (this->firstGridLink == nullptr)
        {
            this->lastGridLink = nullptr;
        }

        this->size--;
    }
}

/// @brief clear the grid
/// @param clear_grid if true, the grids that the GridLinkGuard contains are freed
void GridLinkGuard::clear(bool clear_grid)
{
    while (this->size != 0)
    {
        pop_first(clear_grid);
    }
    this->init();
}

/// @brief extend a GridLinkGuard with another one, consuming the given GridLinkGuard (ie, The GridLinkGuard will be deleted and freed)
/// @param glg GridLinkGuard consumed that is added to this
void GridLinkGuard::extend(GridLinkGuard *glg)
{
    // add given GridLinkGuard to the start of the this GridLinkGuard
    if (this->firstGridLink == nullptr) {
        this->firstGridLink = glg->firstGridLink;
        this->lastGridLink = glg->lastGridLink;
    }   
    else if (glg->lastGridLink != nullptr)
    {
        glg->lastGridLink->next = this->firstGridLink;
        this->firstGridLink = glg->firstGridLink;
    }


    this->size += glg->size;
    if (this->size >= this->clear_at) {
        this->convert_to_best_scores();
    }

    glg->init();
    delete glg;
}

bool GridLinkGuard::contains_same_colors(const Grid *g) const
{
    if (g == nullptr)
    {
        return true;
    }
    if (this->firstGridLink == nullptr)
    {
        return false;
    }
    GridLink *gl = this->firstGridLink;
    while (gl != nullptr)
    {
        if (are_equal_ttab_char((const char **)g->colors, (const char **)gl->grid->colors, g->get_size(), g->get_size()))
        {
            return true;
        }
        gl = gl->next;
    }
    return false;
}

void GridLinkGuard::switch_colors_with_first(Grid *g)
{
    if (this->firstGridLink == nullptr)
    {
        return;
    }
    g->switch_colors(this->firstGridLink->grid);
}



GridLinkGuard *GridLinkGuard::solve_all_grids()
{
    // this->pretty_print();
    GridLinkGuard *storageGrids = new GridLinkGuard;

    GridLinkGuard **storageGridsThread = new GridLinkGuard *[this->size];
    for (int i = 0; i < this->size; i++)
    {
        storageGridsThread[i] = new GridLinkGuard;
    }

    std::thread *threadTab = new std::thread[this->size];

    GridLink *glTemp = this->firstGridLink;
    int i = 0;
    while (glTemp != nullptr)
    {
        glTemp->grid->fill_blank();
        // glTemp->grid->copy_grid_as_ptr(true)->optimize_grid_recur(storageGrids, nullptr, 2)
        threadTab[i] = std::thread(&Grid::optimize_grid_recur, glTemp->grid->copy_grid_as_ptr(true), storageGridsThread[i], nullptr, 2);
        printf("called\n");
        glTemp = glTemp->next;
        i++;
    }

    for (i = 0; i < this->size; i++)
    {
        if (threadTab[i].joinable())
        {
            printf("waiting for one\n");
            threadTab[i].join();
        }
        printf("thread finished\n");
        storageGrids->extend(storageGridsThread[i]);
    }
    delete[] storageGridsThread;
    delete[] threadTab;

    printf("finished\n");
    GridLinkGuard *bestGlg = storageGrids->get_best_scores();
    delete storageGrids;
    return bestGlg;
}

void GridLinkGuard::save_all_in_files(char *output_file)
{
    GridLink *gl = this->firstGridLink;
    int i = 1;
    std::string filebase = output_file;
    while (gl != nullptr)
    {
        gl->grid->save_in_file(filebase + std::to_string(i));
        gl = gl->next;
        i++;
    }
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
GridLinkGuard *GridLinkGuard::get_best_scores() const
{
    if (this->firstGridLink == nullptr)
    {
        std::cout << "The set of grids is empty !\n";
        return new GridLinkGuard;
    }

    // setup temp GridLink
    GridLink *gl = this->firstGridLink->next;

    // setup GridLinkGuard for the best grids
    GridLinkGuard *glgBestTemp = new GridLinkGuard(this->firstGridLink->grid);

    while (gl != nullptr)
    {
        if (glgBestTemp->firstGridLink != nullptr)
        {
            // current focus better than our current best
            if (gl->grid->get_score() >= glgBestTemp->firstGridLink->grid->get_score())
            {
                // strictly superior, clear best
                if (gl->grid->get_score() > glgBestTemp->firstGridLink->grid->get_score())
                {
                    glgBestTemp->clear(false);
                }

                // check if the grid is no already in the list
                if (not glgBestTemp->contains_same_colors(gl->grid))
                {
                    glgBestTemp->addGrid(gl->grid);
                }
            }
        }
        gl = gl->next;
    }

    // glgBestTemp->print_all_scores();

    GridLinkGuard *glgBest = glgBestTemp->copy_as_ptr();
    glgBestTemp->clear(false);
    delete glgBestTemp;

    return glgBest;
}

/// @brief convert the current GridLinkGuard into a GridLinkGuard storing the Grid with the best scores
void GridLinkGuard::convert_to_best_scores()
{
    GridLinkGuard *glg = this->get_best_scores();
    this->clear(true);
    this->extend(glg);
}

const Grid *GridLinkGuard::get_first_grid() const
{
    return this->firstGridLink != nullptr ? this->firstGridLink->grid : nullptr;
}

std::string *GridLinkGuard::get_pretty_print()
{
    std::string *s = new std::string;
    GridLink *gl = this->firstGridLink;

    if (this->firstGridLink != nullptr)
    {
        *s = this->firstGridLink->grid->get_score();
        while (gl != nullptr)
        {
            *s += ", " + std::to_string(gl->grid->get_score());
            gl = gl->next;
        }
    }
    return s;
}

void GridLinkGuard::pretty_print() const
{
    std::cout << "\n=====-=====-=====\n";
    GridLink *gl = this->firstGridLink;
    if (gl != nullptr)
    {
        while (gl != nullptr)
        {   
            std::cout << gl->grid->get_nb_penality_blue() << "\n";
            gl->grid->print_colors_with_score();

            gl = gl->next;
        }
    }
}

void GridLinkGuard::fill_blank_all()
{
    GridLink *gl = this->firstGridLink;

    while (gl != nullptr)
    {
        gl->grid->set_score_from_calculation();
        // gl->grid->print_colors_with_score();

        gl->grid->orange_blue();
        gl->grid->print_colors();
        gl->grid->optimize_grid_full();
        gl->grid->optimize_grid_full();
        gl->grid->set_score_from_calculation();
        // gl->grid->print_colors_with_score();

        // std::cout << gl->grid->get_nb_penality_blue() << '\n';

        gl->grid->yellow_replace_blue();
        gl->grid->set_score_from_calculation();
        // gl->grid->print_colors_with_score();
        gl->grid->fill_blank();

        gl = gl->next;
    }
    // this->pretty_print();
    
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