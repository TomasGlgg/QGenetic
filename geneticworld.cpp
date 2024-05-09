#include "geneticworld.h"


inline int floorDivision(int x, int y) {
    return floor(static_cast<float>(x) / static_cast<float>(y));
}


GeneticWorld::~GeneticWorld() {
    qDeleteAll(bots);
    bots.clear();
    killedBots.clear();
}

void GeneticWorld::deadBot2Alive(Bot* bot) {
    bot->type = ALIVE;
    bot->direction = 0;
    bot->minerals = 0;
    bot->iterator = 0;
    bot->old = 0;
    if (bot->monitoring) {
        emit bot->botKilled();
        bot->monitoring = false;
    }

    bot->usedPhotosynthesis = 0, bot->usedMinerals = 0, bot->usedEat = 0;
    bot->mineralsCount = 0, bot->photosynthesisCount = 0, bot->eatCount = 0;

    int res = killedBots.remove(bot);
    assert(res);
}

Bot *GeneticWorld::newBot(QPoint xy) {
    Bot *bot;
    int block = checkCoords(xy);
    switch (block) {
        case ERROR:
        case BOT: return nullptr;

        case DEADBOT:
            bot = bots.value(hashxy(xy));
            deadBot2Alive(bot);
            break;

        case NULLBLOCK:
            bot = new Bot(genomeLen, xy);
            ulong hash = bot->getHash();
            //botsMutex.lock();
            bots[hash] = bot;
            //botsMutex.unlock();
            break;
    }
    aliveBotsCount++;
    return bot;
}

inline void GeneticWorld::eatBot(Bot *bot, bool noOrganic) {
    if (bot->type != ALIVE) return;
    if (organicEnabled && !noOrganic) {
        bot->type = ORGANIC;
        bot->old = 0;
     } else {
        clearOrganic(bot);
    }
     aliveBotsCount--;
}

inline void GeneticWorld::clearOrganic(Bot *bot) {
    bot->type = DEAD;
    killedBots << bot;
}

inline uint GeneticWorld::botPart(Bot *bot) {
    return floorDivision(bot->getY(), partLength);
}

float GeneticWorld::getPhotosynthesisCoefficient() {
    float coeff = ((1+qCos(2.*generation/generationsInYear*M_PI))/2);
    return minPhotosynthesisCoefficient + coeff * (1-minPhotosynthesisCoefficient);
}

uint GeneticWorld::getPhotosynthesisEnergy(int y) {
    uint part = floorDivision(y, partLength);
    if (part >= (worldPartsCount - startWorldPhotosynthesisEnergy)) {
        float energy = startWorldPhotosynthesisEnergy - (worldPartsCount - part) + 1;
        energy *= getPhotosynthesisCoefficient();
        return floor(energy);
    }
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

void GeneticWorld::translateCoords(QPoint &xy) {
    if (xy.x() >= maxX) xy.setX(0);
    if (xy.x() < 0) xy.setX(maxX-1);
}

QPoint GeneticWorld::oppositeBot(Bot *bot) {
   QPoint xy = bot->getXY();
   switch (bot->direction) {
       case UP: {
           xy.ry()++;
           break;
       }
       case UP_RIGHT: {
           xy.rx()++;
           xy.ry()++;
           break;
       }
       case RIGHT: {
           xy.rx()++;
           break;
       }
       case RIGHT_DOWN: {
           xy.rx()++;
           xy.ry()--;
           break;
       }
       case DOWN: {
           xy.ry()--;
           break;
       }
       case DOWN_LEFT: {
           xy.rx()--;
           xy.ry()--;
           break;
       }
       case LEFT: {
           xy.rx()--;
           break;
       }
       case LEFT_UP: {
           xy.rx()--;
           xy.ry()++;
           break;
       }
       default: {
           assert(false);
       }
   }
   translateCoords(xy);
   return xy;
}

int GeneticWorld::checkCoords(QPoint xy) {
    if (xy.y() < 0 || xy.y() >= maxY) return ERROR;
    ulong hash = hashxy(xy);
    if (bots.contains(hash)) {
           Bot* bot = bots.value(hash);
           if (bot->type == ALIVE || bot->type == ORGANIC) return BOT;
           return DEADBOT;
    }
    return NULLBLOCK;
}


bool GeneticWorld::reproduction(Bot *bot) {
    QPoint xy = oppositeBot(bot);
    if (bot->energy<reproductionPrice) return false;
    int energy = bot->energy/2;
    bot->energy -= energy;
    bot->energy -= reproductionPrice;
    Bot *new_bot = newBot(xy);
    if (new_bot == nullptr) return false;
    new_bot->energy = energy;
    new_bot->direction = bot->direction;
    new_bot->genome = QList(bot->genome);

    //copy genome and mutate
    int k;
    float random = rand()/(RAND_MAX + 1.);
    if (random < botMutateChance) {
        int index = rand() % genomeLen;
        new_bot->genome[index] += rand()%(mutateGenRange*2)-mutateGenRange;
    }
    new_bot->genomeStatisticInit();
    return true;
}

void GeneticWorld::moveBot(Bot *bot, QPoint xy) {
    //botsMutex.lock();
    bots.remove(bot->getHash());
    bot->move(xy);
    bots[bot->getHash()] = bot;
    //botsMutex.unlock();
}

void GeneticWorld::organicStep(Bot *bot) {
    bot->energy--;
    if (bot->energy <= 0) {
        clearOrganic(bot);
        return;
    }
    QPoint xy = bot->getXY();
    xy.ry()--;
    if (checkCoords(xy) == NULLBLOCK)
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
    int command = bot->genome[bot->iterator];
    switch (command) {
        case commands::reproduction: {
            if (!reproduction(bot)) eatBot(bot);
            bot->energy--;
            break;
        }

        case commands::photosynthesis: {
            bot->usedPhotosynthesis++;
            uint new_energy = getPhotosynthesisEnergy(bot->getY());
            bot->energy += new_energy;
            bot->energy--;
            break;
        }

        case commands::minerals: {
            bot->usedMinerals++;
            uint new_minerals = getMineralsCount(bot->getY());
            bot->minerals += new_minerals;
            bot->energy--;
            break;
        }

        case commands::convert_minerals: {
            if (bot->minerals <= 0) break;
            bot->energy += bot->minerals*mineralPrice;
            bot->minerals = 0;
            bot->energy--;
            break;
        }

        case commands::left: {
            if (bot->direction == 0) bot->direction = 7;
            else bot->direction--;
            bot->energy--;
            break;
        }

        case commands::right: {
            bot->direction++;
            bot->direction %= 8;
            bot->energy--;
            break;
        }

        case commands::step: {
            QPoint xy = oppositeBot(bot);
            if (checkCoords(xy) == NULLBLOCK)
                moveBot(bot, xy);
            bot->energy--;
            break;
        }

        case commands::eat: {
            QPoint xy = oppositeBot(bot);
            ulong target_hash = hashxy(xy);
            Bot* target_bot = bots.value(target_hash, nullptr);
            if (target_bot != nullptr) {
                if (target_bot->type == ORGANIC) {
                    bot->energy += target_bot->energy;
                    clearOrganic(target_bot);
                    bot->usedEat++;
                }
            }
            bot->energy--;
            break;
        }

        case commands::steal: {
            QPoint xy = oppositeBot(bot);
            ulong target_hash = hashxy(xy);
            Bot* target_bot = bots.value(target_hash, nullptr);
            if (target_bot != nullptr) {
                Bot* targetBot = bots.value(target_hash);
                if (targetBot->type == ALIVE) {
                    if (bot->minerals >= targetBot->minerals) {
                        bot->minerals -= targetBot->minerals;
                        targetBot->minerals = 0;
                        bot->energy += targetBot->energy * steal_coeff;
                        eatBot(targetBot, true);
                    } else {
                        targetBot->minerals -= bot->minerals;
                        bot->minerals = 0;
                        //bot->energy += targetBot->energy * steal_coeff;
                        //targetBot->energy *= 1.-steal_coeff;
                    }
                    bot->usedEat++;
                }
            }
            bot->energy--;
            break;
        }

        case commands::share: {
            bot->iterator++;
            int energy = 100*abs(bot->genome[bot->iterator%genomeLen]);

            QPoint xy = oppositeBot(bot);
            ulong target_hash = hashxy(xy);
            Bot* target_bot = bots.value(target_hash, nullptr);
            if (target_bot != nullptr) {
                Bot* target_bot = bots.value(target_hash);
                target_bot->energy += energy;
                bot->energy -= energy;
            }
            bot->energy--;
            break;
        }

        case commands::check: {  // 1 - similarity bot, 2 - bot, 3 - organic, 4 - null, 5 - error
            QPoint xy = oppositeBot(bot);
            int target_block = checkCoords(xy);
            if (target_block == DEADBOT || target_block == NULLBLOCK) {
                bot->iterator += 3;
                break;
            }
            if (target_block == ERROR) {
                bot->iterator += 4;
                break;
            }
            // bot:
            ulong target_hash = hashxy(xy);
            if (bots.contains(target_hash)) {
                Bot *targetBot = bots.value(target_hash);
                if (targetBot->type == ORGANIC) {
                    bot->iterator += 1;
                } else if (targetBot->type == ALIVE) {
                    if (!checkSimilarity(bot, targetBot)) bot->iterator++;
                }
            }
            break;
        }

        case commands::align: {
            Direction randDir = rand()%2 ? LEFT : RIGHT;
            bot->direction = randDir;
            bot->energy--;
            break;
        }

        case commands::check_my_energy: {
            bot->iterator++;
            int checkableEnergy = 100*abs(bot->genome[bot->iterator%genomeLen]);
            if (bot->energy >= checkableEnergy) bot->iterator++;
            break;
        }

        case commands::check_my_level: {
            bot->iterator++;
            int checkableLevel = abs(bot->genome[bot->iterator%genomeLen]);
            uint currentPart = botPart(bot);
            if (currentPart >= checkableLevel) bot->iterator++;
            break;
        }

        case commands::check_my_minerals: {
            bot->iterator++;
            int checkableMinerals = abs(bot->genome[bot->iterator%genomeLen]);
            if (bot->minerals >= checkableMinerals) bot->iterator++;
            break;
        }

        case commands::check_target_minerals: {
            QPoint xy = oppositeBot(bot);
            ulong target_hash = hashxy(xy);
            Bot* target_bot = bots.value(target_hash, nullptr);
            if (target_bot != nullptr) {
                int target_minerals = target_bot->minerals;
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
            QPoint xy = oppositeBot(bot);
            ulong target_hash = hashxy(xy);
            Bot* target_bot = bots.value(target_hash, nullptr);
            if (target_bot != nullptr) mutateBotGenome(target_bot);
            bot->energy--;
            break;
        }

        case commands::change_dir: {
            bot->iterator++;
            int direction = abs(bot->genome[bot->iterator%genomeLen])%8;
            bot->direction = direction;
            bot->energy--;
            break;
        }

        default: {  // unconditional transfer
            bot->iterator += command;
            break;
        }
    }

    if (bot->energy>=maxEnergy)
            if (!reproduction(bot)) eatBot(bot);

    if (bot->energy <= 0 || bot->old >= maxOld)
        eatBot(bot);

    bot->iterator++;
    bot->iterator %= genomeLen;
}

void GeneticWorld::clearKilled() {
    foreach (Bot* bot, killedBots) {
        int res = bots.remove(bot->getHash());
        assert(res);
        delete bot;
    }
    killedBots.clear();
}


void GeneticWorld::run() {
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
        if (!killedBots.empty()) clearKilled();
        if (!organicEnabled) assert((uint)bots.size() == aliveBotsCount);
        generation++;

        processingTime = startTime.nsecsElapsed()/1000;
    }
}


void GeneticWorld::stop() {
    runFlag = false;
}
