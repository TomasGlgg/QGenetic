#ifndef GENETICWORLD_H
#define GENETICWORLD_H

#include <cassert>
#include <vector>
#include <QThread>
#include <QTimer>
#include <iostream>

#include "botstruct.h"


class GeneticWorld : public QThread {
    Q_OBJECT
protected:
    void run();

public:
    GeneticWorld(uint genom_len, uint max_energy, uint max_x, uint max_y);
    ~GeneticWorld();
    Bot *newBot();

    uint generation = 0;
    uint process_delay = 1000; //1 ms
    bool run_flag;

    uint genome_len;
    uint max_energy;
    uint max_x, max_y;
    float mutate_chance = 0.1;

    std::vector<Bot*> bots;
private:
    std::vector<uint> die_bots;

    void process();
    bool reproduction(Bot bot);
    uint getPhotosynthesisEnergy(uint y);
    uint getMineralsEnergy(uint y);
    uint *oppositeBot(Bot bot, uint *xy);
    int findBot(uint *xy);
    bool checkCoords(uint *xy);
    void botStep(uint i);
    void deleteBot(uint index);
    void clearDie();
};

#endif // GENETICWORLD_H
