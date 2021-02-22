#ifndef GENETICWORLD_H
#define GENETICWORLD_H

#include <cassert>
#include <cmath>
#include <QThread>
#include <QList>
#include <QMutex>

#include "botstruct.h"


enum Commands {
    reproduction_command = -8,  // reproduction   = -8
    eat_command,                // eat            = -7
    check_command,              // check          = -6
    minerals_command,           // minerals       = -5
    photosynthesis_command,     // photosynthesis = -4
    right_command,              // right          = -3
    left_command,               // left           = -2
    step_command                // step           = -1
};

class GeneticWorld : public QThread {
    Q_OBJECT
protected:
    void run();

public:
    GeneticWorld();
    ~GeneticWorld();
    Bot *newBot();
    void stop();

    uint getPhotosynthesisEnergy(uint y);
    uint getMineralsEnergy(uint y);

    uint world_parts_count = 10;
    uint start_world_energy = 5;
    float mutate_chance = 0.1;
    uint process_delay = 1; // ms
    uint genome_len = 10;
    uint max_energy = 30;
    uint eat_power = max_energy/2;
    uint max_old = 100;

    int max_x, max_y;
    uint part_lenght;

    //statistic
    uint kills = 0;
    uint generation = 0;
    uint mutation_count = 0;

    QList<Bot*> bots;
    QMutex bots_mutex;
private:
    QList<uint> die_bots;
    bool run_flag;

    bool reproduction(Bot bot);
    int* oppositeBot(Bot bot, int *xy);
    int findBot(int *xy);
    bool checkCoords(int *xy);
    int* translateCoords(int *xy);
    void botStep(uint i);
    void deleteBot(uint index);
    void clearDie();
};

#endif // GENETICWORLD_H
