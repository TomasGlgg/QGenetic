#include "geneticworld.h"



GeneticWorld::~GeneticWorld() {
    qDeleteAll(bots);
    bots.clear();
    killedBots.clear();
}

Bot *GeneticWorld::newBot(int x, int y) {
    Bot *new_bot = new Bot(genomeLen, x, y);
    ulong hash = new_bot->hash;
    botsMutex.lock();
    bots[hash] = new_bot;
    botsMutex.unlock();
    aliveBotsCount++;
    return new_bot;
}

inline void GeneticWorld::eatBot(Bot *bot, bool noOrganic) {
    if (bot->type != ALIVE) return;
    if (organicEnabled && !noOrganic) {
        bot->type = ORGANIC;
        aliveBotsCount--;
     } else {
        if (eatOrganic(bot)) aliveBotsCount--;
    }
}

inline bool GeneticWorld::eatOrganic(Bot *bot) {
    bot->type = KILLED;
    killedBots.push_back(bot);
    return true;
}

uint GeneticWorld::getPhotosynthesisEnergy(uint y) {
    uint part = ceil(static_cast<float>(y) / static_cast<float>(partLenght));
    if (part >= (worldPartsCount - startWorldEnergy))
        return startWorldEnergy - (worldPartsCount - part);
    return 0;
}

uint GeneticWorld::getMineralsCount(uint y) {
    uint part = ceil(static_cast<float>(y) / static_cast<float>(partLenght));
    if (part <= startWorldEnergy)
        return startWorldEnergy - part + 1;
    return 0;
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
       case 0: {
           xy[1]++;
           break;
       }
       case 1: {
           xy[0]++;
           xy[1]++;
           break;
       }
       case 2: {
           xy[0]++;
           break;
       }
       case 3: {
           xy[0]++;
           xy[1]--;
           break;
       }
       case 4: {
           xy[1]--;
           break;
       }
       case 5: {
           xy[1]--;
           xy[0]--;
           break;
       }
       case 6: {
           xy[0]--;
           break;
       }
       case 7: {
           xy[0]--;
           xy[1]++;
           break;
       }
   }
   translateCoords(xy);
   return xy;
}

bool GeneticWorld::checkCoords(int *xy) {
    if (xy[1] < 0 || xy[1] >= maxY) return false;
    ulong hash = hashxy(xy);
    return !bots.contains(hash);
}


bool GeneticWorld::reproduction(Bot *bot) {
    int xy[2];
    oppositeBot(bot, xy);
    if (bot->energy<newBotEnergy) return false;
    bot->energy -= newBotEnergy + 1;
    if (!checkCoords(xy)) return false;
    Bot *new_bot = newBot(xy[0], xy[1]);
    new_bot->energy = newBotEnergy;

    //copy genom and mutate
    int k;
    for (uint i = 0; i<genomeLen; i++) {
        float random = rand()/(RAND_MAX + 1.);
        if (random<mutateChance) {
            k = rand()%3-1; // [-1, 1]
            new_bot->genome[i] = bot->genome[i] + k;
            mutationCount++;
        } else
            new_bot->genome[i] = bot->genome[i];
    }
    return true;
}

void GeneticWorld::moveBot(Bot *bot, int *xy) {
    botsMutex.lock();
    bots.remove(bot->hash);
    bot->move(xy);
    bots[bot->hash] = bot;
    botsMutex.unlock();
}

void GeneticWorld::botStep(Bot *bot) {
    bot->old++;
    if (bot->type == ORGANIC) {
        if (bot->old >= maxOrganicOld + maxOld) {
            eatOrganic(bot);
            return;
        }
        int xy[2];
        xy[0] = bot->getX();
        xy[1] = bot->getY() - 1;
        if (checkCoords(xy))
            moveBot(bot, xy);

        return;
    }

    if (bot->energy>maxEnergy) {
        bot->energy = maxEnergy;
        reproduction(bot);
    }

    uint command_index = bot->iterator;
    int command = bot->genome[command_index];
    switch (command) {
        case reproduction_command: {
            if (!reproduction(bot)) eatBot(bot);
            break;
        }
        case photosynthesis_command: {
            bot->used_photosynthesis++;
            uint new_energy = getPhotosynthesisEnergy(bot->getY());
            bot->energy += new_energy;
            break;
        }
        case minerals_command: {
            bot->used_minerals++;
            uint new_minerals = getMineralsCount(bot->getY());
            bot->minerals += new_minerals;
            break;
        }
        case convert_minerals_command: {
            bot->energy += bot->minerals/4;
            bot->minerals = bot->minerals%4;
            break;
        }
        case left_command: {
            if (bot->direction == 0) bot->direction = 7;
            else bot->direction--;
            break;
        }
        case right_command: {
            bot->direction++;
            bot->direction %= 8;
            break;
        }
        case step_command: {
            int xy[2];
            oppositeBot(bot, xy);
            if (checkCoords(xy))
                moveBot(bot, xy);

            break;
        }
        case eat_command: {
            int xy[2];
            oppositeBot(bot, xy);
            ulong target_hash = hashxy(xy);
            if (bots.contains(target_hash)) {
                bot->used_eat++;
                Bot* target_bot = bots.value(target_hash);
                if (target_bot->type == ORGANIC) {
                    bot->energy += 10;
                    eatOrganic(target_bot);
                }
            }
            break;
        }
        case steal_command: {
            int xy[2];
            oppositeBot(bot, xy);
            ulong target_hash = hashxy(xy);
            if (bots.contains(target_hash)) {
                bot->used_eat++;
                Bot* target_bot = bots.value(target_hash);
                if (target_bot->type != KILLED) {
                    if (target_bot->energy > stealPower) {
                        target_bot->energy -= stealPower;
                        bot->energy += stealPower;
                    } else {
                        bot->energy += target_bot->energy;
                        eatBot(target_bot, true);
                        kills++;
                    }
                }
            }
            break;
        }
        case share_command: {
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
        case check_command: {
            int xy[2];
            oppositeBot(bot, xy);
            ulong target_hash = hashxy(xy);
            if (!bots.contains(target_hash)) {
                bot->iterator += 2;
            } else if (bots[target_hash]->type == ORGANIC) {
                bot->iterator++;
            }
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
        assert(bots.remove(bot->hash));
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
            if (bot->type != KILLED)
                botStep(bot);
        }
        if (killedBots.size())
            clearKilled();
        if (!organicEnabled) assert(bots.size() == aliveBotsCount);
        generation++;

        processingTime = startTime.nsecsElapsed()/1000;
    }
}


void GeneticWorld::stop() {
    runFlag = false;
}
