#ifndef GENETICWORLD_H
#define GENETICWORLD_H

#include <QThread>
#include <QSet>
#include <QHash>
#include <QMutex>
#include <QElapsedTimer>

#include <cassert>
#include <cmath>

#include "botstruct.h"
#include "enums.h"


class GeneticWorld : public QThread {
    Q_OBJECT
protected:
    void run();

public:  // settings
    ~GeneticWorld();
    Bot *newBot(uint x, uint y);
    void stop();

    uint getPhotosynthesisEnergy(int y);
    uint getMineralsCount(uint y);

    uint worldPartsCount;
    uint startWorldPhotosynthesisEnergy;
    uint startWorldMinerals;
    uint mineralsPartSize;

    float botMutateChance;
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
    bool organicEnabled;
    int mutateGenRange;

    // commands settings
    float eatK;
    uint mutateAttackCount;

    int maxX, maxY;
    int partLength;

    //statistic
    uint generation = 0;
    uint aliveBotsCount = 0;

    uint processingTime = 0;

    QHash<ulong, Bot*> bots;
    //QMutex botsMutex;

private:
    bool runFlag;
    QSet<Bot*> killedBots;

    bool checkSimilarity(Bot *bot1, Bot *bot2);
    void mutateBotGenome(Bot *bot);
    inline uint botPart(Bot *bot);
    void moveBot(Bot *bot, int *xy);
    bool reproduction(Bot *bot);
    int* oppositeBot(Bot *bot, int *xy);
    inline bool checkCoords(int *xy);
    int* translateCoords(int *xy);
    void organicStep(Bot *bot);
    void botStep(Bot *bot);
    inline void eatBot(Bot *bot, bool noOrganic=false);
    inline void killOrganic(Bot *bot);
    void clearKilled();
};

#endif // GENETICWORLD_H
