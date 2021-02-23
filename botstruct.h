#ifndef BOTSTRUCT_H
#define BOTSTRUCT_H
#include <QList>

enum BotType {
    ALIVE,
    ORGANIC
};

class Bot {
public:
    Bot(uint genome_len);
    ~Bot();

    int x, y;
    uint direction;
    uint energy;
    uint minerals;
    QList<int> genome;
    uint iterator;
    uint old;
    uint type;

    uint used_photosynthesis = 0, used_minerals = 0, used_eat = 0;
};

#endif // BOTSTRUCT_H
