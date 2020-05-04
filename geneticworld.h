#ifndef GENETICWORLD_H
#define GENETICWORLD_H

#include <cassert>
#include <vector>
#include <QThread>
#include <QTimer>
#include <unistd.h>
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
    int process_delay = 1000; //1 ms
    bool run_flag;

private:
    int genome_len;
    int max_energy;
    int max_x, max_y;
    QTimer *timer;

    void process();
    void reproduction(Bot bot);
    int getPhotosynthesisEnergy(int y);
    int getMineralsEnergy(int y);
    int *oppositeBot(Bot bot, int *xy);
    int findBot(int x, int y);
    bool checkCoords(int x, int y);
    bool checkCoords(int *xy);
    void botStep(Bot bot);
    void deleteBot(int index);
};

#endif // GENETICWORLD_H
