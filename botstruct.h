#ifndef BOTSTRUCT_H
#define BOTSTRUCT_H
#include <QList>

enum BotType {
    ALIVE,
    ORGANIC,
    KILLED
};

class Bot {
private:
    int x, y;
public:
    Bot(uint genome_len, int x, int y);
    ~Bot();

    void move(int *xy);
    int getX();
    int getY();

    QList<int> genome;
    ulong hash;

    uint direction;
    uint energy;
    uint minerals = 0;
    uint iterator = 0;
    uint old = 0;
    uint type = ALIVE;

    uint used_photosynthesis = 0, used_minerals = 0, used_eat = 0;
};


ulong hashxy(int x, int y);

ulong hashxy(Bot *bot);

ulong hashxy(int *xy);

#endif // BOTSTRUCT_H
