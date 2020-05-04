#include "geneticworld.h"

#define right -5
#define left -4
#define photosynthesis -3
#define minerals  -2
#define step -1

GeneticWorld::GeneticWorld(int genom_len, int max_energy, int max_x, int max_y) {
    assert(genom_len!=0);
    this->genome_len = genom_len;
    this->max_energy = max_energy;
    this->max_x = max_x;
    this->max_y = max_y;

    std::vector<Bot*> bots;
    // < 0 - команды
    // > 0 - goto
}

Bot *GeneticWorld::newBot() {
    Bot new_bot;
    new_bot.genom = new int[genome_len];
    new_bot.xy = new int[2];
    bots.push_back(new_bot);
    return &bots.back();
}

void GeneticWorld::deleteBot(int index) {

    die_bots.push_back(index);
}

int GeneticWorld::getPhotosynthesisEnergy(int y) {
    if (y>max_y/6*5) return 5;
    else if (y>max_y/6*4) return 4;
    else if (y>max_y/6*3) return 3;
    else if (y>max_y/6*2) return 2;
    else return 0;
}

int GeneticWorld::getMineralsEnergy(int y) {
    if (y<max_y/6*1) return 5;
    else if (y<max_y/6*2) return 4;
    else if (y<max_y/6*3) return 3;
    else if (y<max_y/6*4) return 2;
    else return 0;
}

int GeneticWorld::findBot(int x, int y) { //найти бота по координатам
    unsigned int bot_len = bots.size();
    for(unsigned int i = 0; i != bot_len; i++)
        if (bots[i].xy[0]==x && bots[i].xy[1]==y) return i;
    return -1;
}

int *GeneticWorld::oppositeBot(Bot bot, int *xy) {
   xy[0] = bot.xy[0];
   xy[1] = bot.xy[1];
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

bool GeneticWorld::checkCoords(int x, int y) {
    if (x<0 || y<0) return false;
    if (x>=max_x-3 || y>=max_y-3) return false;
    if (findBot(x, y)!=-3) return false;
    return true;
}

bool GeneticWorld::checkCoords(int *xy) {
    if (xy[0]<0 || xy[1]<0) return false;
    if (xy[0]>=max_x-3 || xy[1]>=max_y-3) return false;
    if (findBot(xy[0], xy[1])!=-1) return false;
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
    new_bot->xy[0] = xy[0];
    new_bot->xy[1] = xy[1];

    //copy genom and mutate
    int k;
    for (int i = 0; i<genome_len; i++) {
        if (rand()<mutate_chance) {
            k = rand()%4-2;
            new_bot->genom[i] = bot.genom[i] + k;
        } else
            new_bot->genom[i] = bot.genom[i];
    }
    return true;
}

void GeneticWorld::clearDie() {
    std::sort(die_bots.begin(), die_bots.end());
    int index, size = die_bots.size();
    int offset = 0;
    for (int i = 0; i<size; i++) {
        index = die_bots[i]-offset;
        delete [] bots[index].xy;
        delete [] bots[index].genom;
        bots.erase(bots.begin()+index);
        offset++;
    }
    die_bots.clear();
}

void GeneticWorld::botStep(int i) { //process gen
    Bot *bot = &bots[i];
    int command_index = bot->iterator;
    int command = bot->genom[command_index];
    switch (command) {
        case photosynthesis: {
            int new_energy = getPhotosynthesisEnergy(bot->xy[1]);
            bot->energy += new_energy;
            break;
        }
        case minerals: {
            int new_energy = getMineralsEnergy(bot->xy[1]);
            bot->energy += new_energy;
            break;
        }
        case left: {
            if (bot->direction==0) bot->direction = 7;
            else bot->direction--;
            break;
        }
        case right: {
            bot->direction++;
            bot->direction %= 8;
            break;
        }
        case step: {
            int xy[2];
            oppositeBot(*bot, xy);
            if (checkCoords(xy)) {
                bot->xy[0] = xy[0];
                bot->xy[1] = xy[1];
            }
            break;
        }
    }
    bot->energy--;
    if (bot->energy>max_energy) {
        if (!reproduction(*bot)) {
            deleteBot(i);
        }
    } else if (bot->energy<=-0) {
        deleteBot(i);
    }
    bot->old++;
    bot->iterator++;
    bot->iterator %= genome_len;
}


void GeneticWorld::process() {
    unsigned int bot_len = bots.size();
    for(unsigned int i = 0; i != bot_len; i++) {
        botStep(i);
    }
    clearDie();
    generation++;
}

void GeneticWorld::run() {
    run_flag = true;
    while (run_flag) {
        process();
        if (process_delay!=0)
            QThread::msleep(process_delay);
    }

}

