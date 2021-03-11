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

    uint world_parts_count;
    uint start_world_energy;
    float mutate_chance;
    uint process_delay;
    uint genome_len;
    uint max_energy;
    uint eat_power;
    uint max_old;
    uint max_bot_count;
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
