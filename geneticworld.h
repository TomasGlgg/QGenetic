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


enum Commands {
    check_my_energy = -12,      // check my energy  = -12
    share_command,              // share command    = -11
    reproduction_command,       // reproduction     = -10
    eat_command,                // eat              = -9
    steal_command,              // steal command    = -8
    check_command,              // check            = -7
    minerals_command,           // minerals         = -6
    photosynthesis_command,     // photosynthesis   = -5
    convert_minerals_command,   // convert minerals = -4
    right_command,              // right            = -3
    left_command,               // left             = -2
    step_command                // step             = -1
};


class GeneticWorld : public QThread {
    Q_OBJECT
protected:
    void run();

public:
    ~GeneticWorld();
    Bot *newBot(int x, int y);
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
    bool checkCoords(int *xy);
    int* translateCoords(int *xy);
    void botStep(Bot *bot);
    inline void eatBot(Bot *bot, bool noOrganic=false);
    inline bool eatOrganic(Bot *bot);
    void clearKilled();
};

#endif // GENETICWORLD_H
