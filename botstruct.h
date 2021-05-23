#ifndef BOTSTRUCT_H
#define BOTSTRUCT_H

#include <QObject>
#include <QList>

#include "commands.h"

enum BotType {
    ALIVE,
    ORGANIC,
    KILLED
};

class Bot : public QObject {
    Q_OBJECT
private:
    uint x, y;
    ulong hash;

public:
    Bot(uint genome_len, uint x, uint y);
    virtual ~Bot();

    void move(uint *xy);
    uint getX();
    uint getY();
    ulong getHash();
    void genomeStatisticInit();

    QList<int> genome;

    uint direction = 0;
    int energy;
    int minerals = 0;
    uint iterator = 0;
    uint old = 0;
    uint type = ALIVE;

    uint usedPhotosynthesis = 0, usedMinerals = 0, usedEat = 0;     // used gens
    uint mineralsCount = 0, photosynthesisCount = 0, eatCount = 0;  // gen count

    bool monitoring = false;

signals:
    void botKilled();
};


ulong hashxy(int x, int y);

ulong hashxy(Bot *bot);

ulong hashxy(int *xy);

#endif // BOTSTRUCT_H
