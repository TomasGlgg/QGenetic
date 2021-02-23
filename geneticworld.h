#ifndef GENETICWORLD_H
#define GENETICWORLD_H

#include <QThread>
#include <QList>
#include <QMutex>

#include <cassert>
#include <cmath>
#include <chrono>

#include "botstruct.h"


enum Commands {
    share_command = -10,        // share_command    = -10
    reproduction_command,       // reproduction     = -9
    eat_command,                // eat              = -8
    check_command,              // check            = -7
    minerals_command,           // minerals         = -6
    photosynthesis_command,     // photosynthesis   = -5
    convert_minerals_command,   // convert minerals = -4
    right_command,              // right            = -3
    left_command,               // left             = -2
    step_command                // step             = -1
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
    uint getMineralsCount(uint y);
    uint aliveBotsCount();

    uint world_parts_count = 10;
    uint start_world_energy = 5;
    float mutate_chance = 0.1;
    uint process_delay = 1; // ms
    uint genome_len = 10;
    uint max_energy = 30;
    uint eat_power = max_energy/2;
    uint max_old = 100;
    uint max_bot_count = max_x*max_y;
    bool organic_enabled = true;

    int max_x, max_y;
    uint part_lenght;

    //statistic
    uint kills = 0;
    uint generation = 0;
    uint mutation_count = 0;

    uint processing_time = 0;

    QList<Bot*> bots;
    QMutex bots_mutex;
    bool run_flag;
private:
    QList<uint> die_bots;
    QList<uint> eaten_organic;

    bool reproduction(Bot &bot);
    int* oppositeBot(Bot bot, int *xy);
    int findBot(int *xy);
    bool checkCoords(int *xy);
    int* translateCoords(int *xy);
    void botStep(uint i);
    void killBot(uint index);
    void eatOrganic(uint index);
    void clearDie();
    void clearOrganic();
};

#endif // GENETICWORLD_H
