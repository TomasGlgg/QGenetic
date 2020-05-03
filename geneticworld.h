#ifndef GENETICWORLD_H
#define GENETICWORLD_H

#include <cassert>
#include <vector>
#include "botstruct.h"


class GeneticWorld {
public:
    GeneticWorld(int genom_len, int max_energy, int max_x, int max_y);
    std::vector<Bot> bots;
    Bot *newBot();
    void Process();

    int generation = 0;
    bool run;

private:
    int genome_len;
    int max_energy;
    int max_x, max_y;

    int getPhotosynthesisEnergy(int y);
    int getMineralsEnergy(int y);
    void botStep(Bot bot);
    void deleteBot(int index);
};

#endif // GENETICWORLD_H
