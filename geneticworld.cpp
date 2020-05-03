#include "geneticworld.h"

struct commands {
    const int photosynthesis = -3;
    const int minerals = -2;
    const int step = -1;
};

GeneticWorld::GeneticWorld(int genom_len, int max_energy) {
    assert(genom_len!=0);
    this->genome_len = genom_len;
    this->max_energy = max_energy;

    std::vector<Bot*> bots;
    // < 0 - команды
    // > 0 - goto
}


GeneticWorld::GeneticWorld() {}

Bot *GeneticWorld::newBot() {
    bots.push_back(Bot());
    return &bots.back();
}

void GeneticWorld::deleteBot(int index) {
    bots.erase(bots.begin()+index);
}
