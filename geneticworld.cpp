#include "geneticworld.h"

#define photosynthesis -3
#define minerals = -2
#define step -1

GeneticWorld::GeneticWorld(int genom_len, int max_energy) {
    assert(genom_len!=0);
    this->genome_len = genom_len;
    this->max_energy = max_energy;

    std::vector<Bot*> bots;
    // < 0 - команды
    // > 0 - goto
}

Bot *GeneticWorld::newBot() {
    bots.push_back(Bot());
    return &bots.back();
}

void GeneticWorld::deleteBot(int index) {
    bots.erase(bots.begin()+index);
}

void GeneticWorld::botStep(Bot bot) {
    int command_index = bot.iterator;
    int command = bot.genom[command_index];
    switch (command) {
        case photosynthesis:
            break;
    }
}
