#ifndef BOTSTRUCT_H
#define BOTSTRUCT_H

#include <QList>

#include "commands.h"

enum BotType {
    ALIVE,
    ORGANIC,
    KILLED
};

class Bot {
private:
    uint x, y;
public:
    Bot(uint genome_len, uint x, uint y);
    ~Bot();

    void move(uint *xy);
    uint getX();
    uint getY();
    void genomeInited();

    QList<int> genome;
    ulong hash;

    uint direction = 0;
    uint energy;
    uint minerals = 0;
    uint iterator = 0;
    uint old = 0;
    uint type = ALIVE;

    uint used_photosynthesis = 0, used_minerals = 0, used_eat = 0;
    uint mineralsCount = 0, photosynthesisCount = 0, eatCount = 0;
};


ulong hashxy(int x, int y);

ulong hashxy(Bot *bot);

ulong hashxy(int *xy);

#endif // BOTSTRUCT_H
