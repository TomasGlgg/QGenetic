#ifndef GENETICWORLD_H
#define GENETICWORLD_H

#include <QThread>
#include <QSet>
#include <QHash>
#include <QMutex>
#include <QElapsedTimer>
#include <QPoint>

#include <cassert>
#include <cmath>

#include "botstruct.h"
#include "enums.h"


class GeneticWorld : public QThread {
    Q_OBJECT
protected:
    void run();

public:  // == settings ==
    ~GeneticWorld();
    Bot *newBot(QPoint xy);
    void stop();

    uint getPhotosynthesisEnergy(int y);
    uint getMineralsCount(uint y);
    float getPhotosynthesisCoefficient();

    uint worldPartsCount;
    uint startWorldPhotosynthesisEnergy;
    uint startWorldMinerals;
    uint mineralsPartSize;
    float minPhotosynthesisCoefficient;

    uint processDelay;

    uint generationsInYear;

    // bots
    float botMutateChance;
    uint genomeLen;
    uint maxEnergy;
    uint newBotEnergy;
    uint maxOld;
    uint maxOrganicOld;
    uint maxBotCount;
    uint organicEnergy;
    uint reproductionPrice;
    bool organicEnabled;
    int mutateGenRange;
    uint mineralPrice;

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
    void translateCoords(QPoint &xy);
    QPoint oppositeBot(Bot *bot);
    inline bool checkCoords(QPoint xy);
    bool reproduction(Bot *bot);
    void moveBot(Bot *bot, QPoint xy);
    void organicStep(Bot *bot);
    void botStep(Bot *bot);
    void mutateBotGenome(Bot *bot);
    inline uint botPart(Bot *bot);
    inline void eatBot(Bot *bot, bool noOrganic=false);
    inline void clearOrganic(Bot *bot);
    void clearKilled();
};

#endif // GENETICWORLD_H
