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
    GeneticWorld(int genom_len, int max_energy, int max_x, int max_y);
    ~GeneticWorld();
    Bot *newBot();

    int generation = 0;
    float process_delay = 1000; //1 ms
    bool run_flag;

    std::vector<Bot> bots;
private:
    int genome_len;
    int max_energy;
    int max_x, max_y;
    std::vector<int> die_bots;
    float mutate_chance = 0.1;

    void process();
    bool reproduction(Bot bot);
    int getPhotosynthesisEnergy(int y);
    int getMineralsEnergy(int y);
    int *oppositeBot(Bot bot, int *xy);
    int findBot(int x, int y);
    bool checkCoords(int x, int y);
    bool checkCoords(int *xy);
    void botStep(int i);
    void deleteBot(int index);
    void clearDie();
};

#endif // GENETICWORLD_H
