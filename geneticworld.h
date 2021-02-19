#ifndef GENETICWORLD_H
#define GENETICWORLD_H

#include <cassert>
#include <QThread>
#include <QTimer>
#include <QList>

#include "botstruct.h"


class GeneticWorld : public QObject {
    Q_OBJECT
protected:
    void run();

public:
    GeneticWorld(uint genom_len, uint max_energy, uint max_x, uint max_y);
    ~GeneticWorld();
    Bot *newBot();
    void start(uint delay);
    void stop();

    uint generation = 0;
    uint process_delay = 1; // ms

    uint genome_len;
    uint max_energy;
    uint max_x, max_y;
    float mutate_chance = 0.1;

    QList<Bot*> bots;
private:
    QList<uint> die_bots;
    QTimer *timer = new QTimer(this);

    bool reproduction(Bot bot);
    uint getPhotosynthesisEnergy(uint y);
    uint getMineralsEnergy(uint y);
    int *oppositeBot(Bot bot, int *xy);
    int findBot(int *xy);
    bool checkCoords(int *xy);
    void botStep(uint i);
    void deleteBot(uint index);
    void clearDie();

private slots:
    void process();
};

#endif // GENETICWORLD_H
