#include "geneticworld.h"



GeneticWorld::~GeneticWorld() {
    qDeleteAll(bots);
    bots.clear();
    killed_bots.clear();
}
GeneticWorld::GeneticWorld() {
    setPriority(QThread::HighPriority);
   // QList<Bot*> bots;
    // < 0 - команды
    // > 0 - goto
}

Bot *GeneticWorld::newBot(int x, int y) {
    Bot *new_bot = new Bot(genome_len, x, y);
    ulong hash = new_bot->hash;
    bots[hash] = new_bot;
    return new_bot;
}

inline void GeneticWorld::killBot(Bot *bot) {
    if (organic_enabled)
        bot->type = ORGANIC;
    else
        eatOrganic(bot);
}

inline void GeneticWorld::eatOrganic(Bot *bot) {
    bot->type = KILLED;
    killed_bots.push_back(bot);
}

uint GeneticWorld::getPhotosynthesisEnergy(uint y) {
    uint part = ceil(static_cast<float>(y) / static_cast<float>(part_lenght));
    if (part > (world_parts_count - start_world_energy))
        return start_world_energy - (world_parts_count - part);
    return 0;
}

uint GeneticWorld::getMineralsCount(uint y) {
    uint part = ceil(static_cast<float>(y) / static_cast<float>(part_lenght));
    if (part <= start_world_energy)
        return start_world_energy - part + 1;
    return 0;
}

int* GeneticWorld::translateCoords(int *xy) {
    if (xy[0]>=max_x) xy[0] = 0;
    if (xy[0]<0) xy[0] = max_x-1;
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
    if (xy[1] < 0 || xy[1] >= max_y) return false;
    ulong hash = hashxy(xy);
    return !bots.contains(hash);
}


bool GeneticWorld::reproduction(Bot *bot) {
    int xy[2];
    oppositeBot(bot, xy);
    if (bot->energy<max_energy/2) return false;
    bot->energy /= 2;
    if (!checkCoords(xy)) return false;
    bots_mutex.lock();
    Bot *new_bot = newBot(xy[0], xy[1]);
    new_bot->direction = bot->direction;
    new_bot->energy = bot->energy/2;

    //copy genom and mutate
    int k;
    for (uint i = 0; i<genome_len; i++) {
        float random = rand()/(RAND_MAX + 1.);
        if (random<mutate_chance) {
            k = rand()%3-1; // [-1, 1]
            new_bot->genome[i] = bot->genome[i] + k;
            mutation_count++;
        } else
            new_bot->genome[i] = bot->genome[i];
    }
    bots_mutex.unlock();
    return true;
}

void GeneticWorld::moveBot(Bot *bot, int *xy) {
    bots_mutex.lock();
    bots.remove(bot->hash);
    bot->move(xy);
    bots[bot->hash] = bot;
    bots_mutex.unlock();
}

void GeneticWorld::botStep(Bot *bot) {
    bot->old++;
    if (bot->type == ORGANIC) {
        if (bot->old >= max_organic_old) {
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

    if (bot->energy>max_energy) {
        bot->energy = max_energy;
        reproduction(bot);
    }

    uint command_index = bot->iterator;
    int command = bot->genome[command_index];
    switch (command) {
        case reproduction_command: {
            if (!reproduction(bot)) killBot(bot);
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
            bot->minerals = 0;
            break;
        }
        case left_command: {
            if (bot->direction==0) bot->direction = 7;
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
                if (target_bot->type != KILLED) {
                    if (target_bot->type == ORGANIC) {
                        eatOrganic(target_bot);
                        bot->energy += max_energy/2;
                    } else if (target_bot->energy > eat_power) {
                        target_bot->energy -= eat_power;
                        bot->energy += eat_power;
                    } else {
                        bot->energy += target_bot->energy;
                        eatOrganic(target_bot);
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

    if (bot->energy <= 0 || bot->old >= max_old)
        killBot(bot);

    bot->iterator++;
    bot->iterator %= genome_len;
}

uint GeneticWorld::aliveBotsCount() {
    uint count = 0;
    foreach (Bot *bot, bots)
        if (bot->type == ALIVE) count++;
    return count;
}

void GeneticWorld::clearKilled() {
    Bot *bot;
    foreach (bot, killed_bots) {
        assert(bots.contains(bot->hash));
        bots.remove(bot->hash);
        delete bot;

    }
    killed_bots.clear();
}


void GeneticWorld::run() {
    run_flag = true;
    while (run_flag) {
        if (process_delay) {
            if (process_delay*1000 > processing_time)
                usleep(process_delay*1000 - processing_time);
        }
        auto start = std::chrono::steady_clock::now();

        foreach (Bot *bot, bots) {
            if (bot->type != KILLED)
                botStep(bot);
        }
        if (killed_bots.size())
            clearKilled();
        generation++;

        auto end = std::chrono::steady_clock::now();
        processing_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }
}


void GeneticWorld::stop() {
    run_flag = false;
}
