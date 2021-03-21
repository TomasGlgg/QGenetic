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
#include "commands.h"


class GeneticWorld : public QThread {
    Q_OBJECT
protected:
    void run();

public:
    ~GeneticWorld();
    Bot *newBot(uint x, uint y);
    void stop();

    uint getPhotosynthesisEnergy(uint y);
    uint getMineralsCount(uint y);

    uint worldPartsCount;
    uint startWorldEnergy;
    float mutateChance;
    uint processDelay;
    uint genomeLen;
    uint maxEnergy;
    uint newBotEnergy;
    uint stealPower;
    uint maxOld;
    uint maxOrganicOld;
    uint maxBotCount;
    uint organicEnergy;
    uint mineralPrice;
    bool organicEnabled = true;

    int maxX, maxY;
    uint partLenght;

    //statistic
    uint kills = 0;
    uint generation = 0;
    uint mutationCount = 0;
    uint aliveBotsCount = 0;

    uint processingTime = 0;

    QHash<ulong, Bot*> bots;
    QMutex botsMutex;
    bool runFlag;
private:
    QList<Bot*> killedBots;

    void moveBot(Bot *bot, int *xy);
    bool reproduction(Bot *bot);
    int* oppositeBot(Bot *bot, int *xy);
    inline bool checkCoords(int *xy);
    int* translateCoords(int *xy);
    void organicStep(Bot *bot);
    void botStep(Bot *bot);
    inline void eatBot(Bot *bot, bool noOrganic=false);
    inline bool eatOrganic(Bot *bot);
    void clearKilled();
};

#endif // GENETICWORLD_H
