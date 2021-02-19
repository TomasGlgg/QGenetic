#include "geneticworld.h"

#define eat_power 30


enum Commands {
    reproduction_command = -7,  // reproduction   = -7
    eat_command,                // eat            = -6
    minerals_command,           // minerals       = -5
    photosynthesis_command,     // photosynthesis = -4
    right_command,              // right          = -3
    left_command,               // left           = -2
    step_command                // step           = -1
};

GeneticWorld::~GeneticWorld() {
    bots.clear();
}
GeneticWorld::GeneticWorld(uint genom_len, uint max_energy, uint max_x, uint max_y) {
    assert(genom_len!=0);
    this->genome_len = genom_len;
    this->max_energy = max_energy;
    this->max_x = max_x;
    this->max_y = max_y;

    connect(timer, SIGNAL(timeout()), this, SLOT(process()));

   // std::vector<Bot*> bots;
    // < 0 - команды
    // > 0 - goto
}

Bot *GeneticWorld::newBot() {
    Bot *new_bot = new Bot(genome_len);
    bots.push_back(new_bot);
    return new_bot;
}

void GeneticWorld::deleteBot(uint index) {
    if (std::find(die_bots.begin(), die_bots.end(), index) == die_bots.end())
        die_bots.push_back(index);
}

uint GeneticWorld::getPhotosynthesisEnergy(uint y) {
    if (y>max_y/6*5) return 5;
    else if (y>max_y/6*4) return 4;
    else if (y>max_y/6*3) return 3;
    else if (y>max_y/6*2) return 1;
    else return 0;
}

uint GeneticWorld::getMineralsEnergy(uint y) {
    if (y<max_y/6*1) return 5;
    else if (y<max_y/6*2) return 4;
    else if (y<max_y/6*3) return 3;
    else if (y<max_y/6*4) return 1;
    else return 0;
}

int GeneticWorld::findBot(int *xy) { //найти бота по координатам
    unsigned int bot_len = bots.size();
    for(uint i = 0; i != bot_len; i++)
        if (bots[i]->x==xy[0] && bots[i]->y==xy[1]) return i;
    return -1;
}

int *GeneticWorld::oppositeBot(Bot bot, int *xy) {
   xy[0] = bot.x;
   xy[1] = bot.y;
   switch (bot.direction) {
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
   return xy;
}

bool GeneticWorld::checkCoords(int *xy) {
    if (xy[0]<0 || xy[1]<0) return false;
    if (xy[0]>=max_x-3 || xy[1]>=max_y-3) return false;
    if (findBot(xy)!=-1) return false;
    return true;
}

bool GeneticWorld::reproduction(Bot bot) {
    int xy[2];
    oppositeBot(bot, xy);
    if (bot.energy<max_energy/2) return false;
    if (!checkCoords(xy)) return false;

    Bot *new_bot = newBot();
    new_bot->direction = bot.direction;
    new_bot->energy = max_energy/2;
    new_bot->x = xy[0];
    new_bot->y = xy[1];

    //copy genom and mutate
    int k;
    for (uint i = 0; i<genome_len; i++) {
        float random = rand()/(RAND_MAX + 1.);
        if (random<(mutate_chance/genome_len)) {
            k = rand()%3-1; // [-1, 1]
            new_bot->genome[i] = bot.genome[i] + k;
            mutation_count++;
        } else
            new_bot->genome[i] = bot.genome[i];
    }
    return true;
}

void GeneticWorld::clearDie() {
    std::sort(die_bots.begin(), die_bots.end(), [](const int a, const int b) {return a > b; });
    for (uint index : die_bots) {
        delete bots[index];
        bots.removeAt(index);
    }
    die_bots.clear();
}

void GeneticWorld::botStep(uint i) { //process gen
    Bot *bot = bots[i];
    uint command_index = bot->iterator;
    int command = bot->genome[command_index];
    switch (command) {
        case reproduction_command: {
            if (!reproduction(*bot)) deleteBot(i);
            break;
        }
        case photosynthesis_command: {
            int new_energy = getPhotosynthesisEnergy(bot->y);
            bot->energy += new_energy;
            break;
        }
        case minerals_command: {
            int new_energy = getMineralsEnergy(bot->y);
            bot->energy += new_energy;
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
            oppositeBot(*bot, xy);
            if (checkCoords(xy)) {
                bot->x = xy[0];
                bot->y = xy[1];
            }
            break;
        }
        case eat_command: {
            int xy[2];
            oppositeBot(*bot, xy);
            int targetindex = findBot(xy);
            if (targetindex != -1) {
                if (bots[targetindex]->energy > eat_power) {
                    bots[targetindex]->energy -= eat_power;
                    bot->energy += eat_power;
                } else {
                    bot->energy += bots[targetindex]->energy;
                    deleteBot(targetindex);
                }
            }
            break;
        }
    }
    bot->energy--;
    if (bot->energy>max_energy) {
        if (!reproduction(*bot)) {
            deleteBot(i);
        }
    } else if (bot->energy<=0) {
        deleteBot(i);
    }
    bot->old++;
    bot->iterator++;
    bot->iterator %= genome_len;
}


void GeneticWorld::process() {
    if (process_delay)
        QThread::msleep(process_delay);
    unsigned int bot_len = bots.size();
    for(unsigned int i = 0; i != bot_len; i++) {
        botStep(i);
    }
    if (die_bots.size())
        clearDie();
    generation++;
}

void GeneticWorld::start(uint delay) {
    timer->start(delay);
}


void GeneticWorld::stop() {
    timer->stop();
}
