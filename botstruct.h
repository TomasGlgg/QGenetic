#ifndef BOTSTRUCT_H
#define BOTSTRUCT_H

#include <QObject>
#include <QList>
#include <QPoint>

#include "enums.h"

class Bot : public QObject {
    Q_OBJECT
private:
    QPoint xy;
    ulong hash;

public:
    Bot(uint genome_len, QPoint xy);
    virtual ~Bot();

    void move(QPoint new_xy);
    inline uint getX() { return xy.x(); };
    inline uint getY() { return xy.y(); };
    inline QPoint getXY() { return xy; };
    inline ulong getHash() { return hash; };
    void genomeStatisticInit();

    QList<int8_t> genome;

    uint8_t direction = 0;
    int energy;
    int minerals = 0;
    uint iterator = 0;
    uint old = 0;
    int8_t type = ALIVE;

    uint usedPhotosynthesis = 0, usedMinerals = 0, usedEat = 0;     // used gens
    uint mineralsCount = 0, photosynthesisCount = 0, eatCount = 0;  // gen count

    bool monitoring = false;

signals:
    void botKilled();
};


ulong hashxy(QPoint xy);

ulong hashxy(Bot *bot);

#endif // BOTSTRUCT_H
