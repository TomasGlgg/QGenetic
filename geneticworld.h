#ifndef GENETICWORLD_H
#define GENETICWORLD_H

#include <QThread>
#include <QList>
#include <QHash>
#include <QMutex>
#include <QElapsedTimer>

#include <cassert>
#include <cmath>

#include "botstruct.h"


enum Commands {
    check_my_energy = -12,      // check my energy  = -12
    share_command,              // share command    = -11
    reproduction_command,       // reproduction     = -10
    eat_command,                // eat              = -9
    steal_command,              // steal command    = -8
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
    ~GeneticWorld();
    Bot *newBot(int x, int y);
    void stop();

    uint getPhotosynthesisEnergy(uint y);
    uint getMineralsCount(uint y);

    uint world_parts_count;
    uint start_world_energy;
    float mutate_chance;
    uint process_delay;
    uint genome_len;
    uint max_energy;
    uint new_bot_energy;
    uint steal_power;
    uint max_old;
    uint max_organic_old;
    uint max_bot_count;
    bool organic_enabled = true;

    int max_x, max_y;
    uint part_lenght;

    //statistic
    uint kills = 0;
    uint generation = 0;
    uint mutation_count = 0;
    uint alive_bots_count = 0;

    uint processing_time = 0;

    QHash<ulong, Bot*> bots;
    QMutex bots_mutex;
    bool run_flag;
private:
    QList<Bot*> killed_bots;

    void moveBot(Bot *bot, int *xy);
    bool reproduction(Bot *bot);
    int* oppositeBot(Bot *bot, int *xy);
    bool checkCoords(int *xy);
    int* translateCoords(int *xy);
    void botStep(Bot *bot);
    inline void eatBot(Bot *bot, bool noOrganic=false);
    inline void eatOrganic(Bot *bot);
    void clearKilled();
};

#endif // GENETICWORLD_H
