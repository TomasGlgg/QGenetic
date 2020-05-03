#ifndef GENETICWORLD_H
#define GENETICWORLD_H

#include <cassert>
#include <vector>
#include "botstruct.h"


class GeneticWorld {
public:
    GeneticWorld(int genom_len, int max_energy);
    GeneticWorld();
    std::vector<Bot> bots;
    Bot *newBot();

private:
    int genome_len;
    int max_energy;

    void deleteBot(int index);
};

#endif // GENETICWORLD_H
