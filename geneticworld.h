#ifndef GENETICWORLD_H
#define GENETICWORLD_H

#include <cassert>
#include <vector>


struct Bot {
    int iterator = 0;
    int direction = 1;
    int energy;
    int *genom;
    int old = 0;
    int xy[2];
};


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
