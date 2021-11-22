#include "geneticworld.h"


inline int floorDivision(int x, int y) {
    return floor(static_cast<float>(x) / static_cast<float>(y));
}


GeneticWorld::~GeneticWorld() {
    qDeleteAll(bots);
    bots.clear();
    killedBots.clear();
}

Bot *GeneticWorld::newBot(uint x, uint y) {
    Bot *new_bot = new Bot(genomeLen, x, y);
    ulong hash = new_bot->getHash();
    //botsMutex.lock();
    bots[hash] = new_bot;
    //botsMutex.unlock();
    aliveBotsCount++;
    return new_bot;
}

inline void GeneticWorld::eatBot(Bot *bot, bool noOrganic) {
    if (bot->type != ALIVE) return;
    if (organicEnabled && !noOrganic) {
        bot->type = ORGANIC;
        bot->old = 0;
        aliveBotsCount--;
     } else {
        killOrganic(bot);
        aliveBotsCount--;
    }
}

inline void GeneticWorld::killOrganic(Bot *bot) {
    bot->type = DEAD;
    killedBots << bot;
}

inline uint GeneticWorld::botPart(Bot *bot) {
    return floorDivision(bot->getY(), partLength);
}

uint GeneticWorld::getPhotosynthesisEnergy(int y) {
    uint part = floorDivision(y, partLength);
    if (part >= (worldPartsCount - startWorldPhotosynthesisEnergy))
        return startWorldPhotosynthesisEnergy - (worldPartsCount - part) + 1;
    return 0;
}

uint GeneticWorld::getMineralsCount(uint y) {
    uint part = floorDivision(y, partLength);
    if (part < mineralsPartSize*startWorldMinerals)
        return startWorldMinerals-floorDivision(part, mineralsPartSize);
    return 0;
}

bool GeneticWorld::checkSimilarity(Bot *bot1, Bot *bot2) {
    int differencesCount = 0;
    for (int i = 0; i<genomeLen; i++) {
        if (bot1->genome[i] != bot2->genome[i]) differencesCount++;
        if (differencesCount > 1) return false;
    }
    return true;
}

int* GeneticWorld::translateCoords(int *xy) {
    if (xy[0] >= maxX) xy[0] = 0;
    if (xy[0] < 0) xy[0] = maxX-1;
    return xy;
}

int* GeneticWorld::oppositeBot(Bot *bot, int *xy) {
   xy[0] = bot->getX();
   xy[1] = bot->getY();
   switch (bot->direction) {
       case UP: {
           xy[1]++;
           break;
       }
       case UP_RIGHT: {
           xy[0]++;
           xy[1]++;
           break;
       }
       case RIGHT: {
           xy[0]++;
           break;
       }
       case RIGHT_DOWN: {
           xy[0]++;
           xy[1]--;
           break;
       }
       case DOWN: {
           xy[1]--;
           break;
       }
       case DOWN_LEFT: {
           xy[1]--;
           xy[0]--;
           break;
       }
       case LEFT: {
           xy[0]--;
           break;
       }
       case LEFT_UP: {
           xy[0]--;
           xy[1]++;
           break;
       }
   }
   translateCoords(xy);
   return xy;
}

inline bool GeneticWorld::checkCoords(int *xy) {
    if (xy[1] < 0 || xy[1] >= maxY) return false;
    ulong hash = hashxy(xy);
    return !bots.contains(hash);
}


bool GeneticWorld::reproduction(Bot *bot) {
    int xy[2];
    oppositeBot(bot, xy);
    if (bot->energy<reproductionPrice) return false;
    bot->energy -= reproductionPrice;
    if (!checkCoords(xy)) return false;
    Bot *new_bot = newBot(xy[0], xy[1]);
    new_bot->energy = newBotEnergy;
    new_bot->direction = bot->direction;
    new_bot->genome = QList(bot->genome);

    //copy genome and mutate
    int k;
    float random;
    random = rand()/(RAND_MAX + 1.);
    if (random < botMutateChance) {
        int index = rand() % genomeLen;
        bot->genome[index] += rand()%(mutateGenRange*2)-mutateGenRange;
    }
    new_bot->genomeStatisticInit();
    return true;
}

void GeneticWorld::moveBot(Bot *bot, int *xy) {
    //botsMutex.lock();
    bots.remove(bot->getHash());
    bot->move((uint*)xy);
    bots[bot->getHash()] = bot;
    //botsMutex.unlock();
}

void GeneticWorld::organicStep(Bot *bot) {
    if (bot->old >= maxOrganicOld) {
        killOrganic(bot);
        return;
    }
    int xy[2];
    xy[0] = bot->getX();
    xy[1] = bot->getY() - 1;
    if (checkCoords(xy))
        moveBot(bot, xy);
}

void GeneticWorld::mutateBotGenome(Bot *bot) {
    for (int i = 0; i<mutateAttackCount; i++) {
        int index = rand() % genomeLen;
        bot->genome[index] += rand()%(mutateGenRange*2)-mutateGenRange;
    }
    bot->genomeStatisticInit();
}

void GeneticWorld::botStep(Bot *bot) {
    if (bot->energy>=maxEnergy)
        if (!reproduction(bot)) eatBot(bot);


    int command = bot->genome[bot->iterator];
    switch (command) {
        case commands::reproduction: {
            if (!reproduction(bot)) eatBot(bot);
            break;
        }
        case commands::photosynthesis: {
            bot->usedPhotosynthesis++;
            uint new_energy = getPhotosynthesisEnergy(bot->getY());
            bot->energy += new_energy;
            break;
        }
        case commands::minerals: {
            bot->usedMinerals++;
            uint new_minerals = getMineralsCount(bot->getY());
            bot->minerals += new_minerals;
            break;
        }
        case commands::convert_minerals: {
            if (bot->minerals <= 0) break;
            bot->energy += bot->minerals*mineralPrice;
            bot->minerals = 0;
            break;
        }
        case commands::left: {
            if (bot->direction == 0) bot->direction = 7;
            else bot->direction--;
            break;
        }
        case commands::right: {
            bot->direction++;
            bot->direction %= 8;
            break;
        }
        case commands::step: {
            int xy[2];
            oppositeBot(bot, xy);
            if (checkCoords(xy))
                moveBot(bot, xy);
            break;
        }
        case commands::eat: {
            int xy[2];
            oppositeBot(bot, xy);
            ulong target_hash = hashxy(xy);
            if (bots.contains(target_hash)) {
                Bot* target_bot = bots.value(target_hash);
                if (target_bot->type == ORGANIC) {
                    bot->energy += organicEnergy;
                    killOrganic(target_bot);
                }
            }
            break;
        }
        case commands::steal: {
            int xy[2];
            oppositeBot(bot, xy);
            ulong target_hash = hashxy(xy);
            if (bots.contains(target_hash)) {
                bot->usedEat++;
                Bot* targetBot = bots.value(target_hash);
                if (targetBot->type == ALIVE) {
                    if (bot->minerals >= targetBot->minerals) {
                        bot->minerals -= targetBot->minerals;
                        bot->energy += targetBot->energy * eatK;
                        eatBot(targetBot, true);
                    } else {
                        targetBot->minerals -= bot->minerals;
                        bot->energy += targetBot->energy/2;
                        targetBot->energy /= 2;
                        bot->minerals = 0;
                    }
                }
            }
            break;
        }
        case commands::share: {
            int xy[2];
            oppositeBot(bot, xy);
            ulong target_hash = hashxy(xy);
            if (bots.contains(target_hash)) {
                Bot* target_bot = bots.value(target_hash);
                if (target_bot->energy < bot->energy) {
                    uint energy_delta = bot->energy - target_bot->energy;
                    target_bot->energy += energy_delta;
                    bot->energy -= energy_delta;
                }
            }
            break;
        }
        case commands::check: {  // 1 - empty, 2 - organic, 3 - bot, 4 - similarity bot
            int xy[2];
            oppositeBot(bot, xy);
            ulong target_hash = hashxy(xy);
            if (!bots.contains(target_hash)) break;
            Bot *targetBot = bots.value(target_hash);
            if (targetBot->type == ORGANIC) {
                bot->iterator += 1;
            } else if (targetBot->type == ALIVE) {
                bot->iterator += 2;
                if (checkSimilarity(bot, targetBot)) bot->iterator++;
            }
            break;
        }

        case commands::align: {
            Direction randDir = rand()%2 ? LEFT : RIGHT;
            bot->direction = randDir;
            break;
        }

        case commands::check_my_energy: {
            bot->iterator++;
            int checkableEnergy = bot->genome[bot->iterator%genomeLen];
            if (bot->energy >= checkableEnergy) bot->iterator++;
            break;
        }

        case commands::check_my_level: {
            bot->iterator++;
            int checkableLevel = bot->genome[bot->iterator%genomeLen];
            uint currentPart = botPart(bot);
            if (currentPart >= checkableLevel) bot->iterator++;
            break;
        }

        case commands::check_my_minerals: {
            bot->iterator++;
            int checkableMinerals = bot->genome[bot->iterator%genomeLen];
            if (bot->minerals >= checkableMinerals) bot->iterator++;
            break;
        }

        case commands::check_target_minerals: {
            int xy[2];
            oppositeBot(bot, xy);
            ulong target_hash = hashxy(xy);
            if (bots.contains(target_hash)) {
                int target_minerals = bots.value(target_hash)->minerals;
                if (target_minerals > bot->minerals) bot->iterator++;
            }
            break;
        }

        case commands::mutate: {
            mutateBotGenome(bot);
            bot->energy += 3;
            break;
        }

        case commands::mutate_attack: {
            int xy[2];
            oppositeBot(bot, xy);
            ulong target_hash = hashxy(xy);
            if (bots.contains(target_hash)) mutateBotGenome(bots.value(target_hash));
            break;
        }
        default: {  // unconditional transfer
            bot->iterator += command;
            break;
        }
    }
    bot->energy--;

    if (bot->energy <= 0 || bot->old >= maxOld)
        eatBot(bot);

    bot->iterator++;
    bot->iterator %= genomeLen;
}

void GeneticWorld::clearKilled() {
    Bot *bot;
    foreach (bot, killedBots) {
        assert(bots.remove(bot->getHash()));
        delete bot;

    }
    killedBots.clear();
}


void GeneticWorld::run() {
    setPriority(QThread::HighPriority);
    QElapsedTimer startTime;
    runFlag = true;
    while (runFlag) {
        if (processDelay) {
            if (processDelay*1000 > processingTime)
                usleep(processDelay*1000 - processingTime);
        }

        startTime.start();
        foreach (Bot *bot, bots) {
            switch (bot->type) {
                case ALIVE: {
                    botStep(bot);
                    break;
                }
                case ORGANIC: {
                    organicStep(bot);
                    break;
                }
                case DEAD: {
                    killedBots << bot;
                }
            }
            bot->old++;
        }
        if (killedBots.size()) clearKilled();
        if (!organicEnabled) assert((uint)bots.size() == aliveBotsCount);
        generation++;

        processingTime = startTime.nsecsElapsed()/1000;
    }
}


void GeneticWorld::stop() {
    runFlag = false;
}
