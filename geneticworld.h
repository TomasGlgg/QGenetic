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
    Bot *newBot();

    std::vector<Bot> bots;
    int generation = 0;
    float process_delay = 1000; //1 ms
    bool run_flag;

private:
    int genome_len;
    int max_energy;
    int max_x, max_y;
    float mutate_chance = 0.1;

    void process();
    bool reproduction(Bot bot);
    int getPhotosynthesisEnergy(int y);
    int getMineralsEnergy(int y);
    int *oppositeBot(Bot bot, int *xy);
    int findBot(int x, int y);
    bool checkCoords(int x, int y);
    bool checkCoords(int *xy);
    void botStep(Bot *bot);
    void deleteBot(Bot *bot);
    void clearDie();
};

#endif // GENETICWORLD_H
