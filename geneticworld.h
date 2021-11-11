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

public:  // settings
    ~GeneticWorld();
    Bot *newBot(uint x, uint y);
    void stop();

    uint getPhotosynthesisEnergy(uint y);
    uint getMineralsCount(uint y);

    uint worldPartsCount;
    uint startWorldPhotosynthesisEnergy;
    uint startWorldMinerals;
    uint mineralsPartSize;

    float mutateChance;
    uint processDelay;
    uint genomeLen;
    uint maxEnergy;
    uint newBotEnergy;
    uint maxOld;
    uint maxOrganicOld;
    uint maxBotCount;
    uint organicEnergy;
    uint mineralPrice;
    uint reproductionPrice;
    bool organicEnabled = true;

    // commands settings
    float eatK;
    float mutateAttackChance;

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

private:
    bool runFlag;
    QList<Bot*> killedBots;

    void mutateBotGenome(Bot *bot);
    void mutateBotGenome(Bot *bot, float chance);
    uint botPart(Bot *bot);
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
