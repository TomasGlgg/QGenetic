#include "botstruct.h"


Bot::Bot(uint genome_len, uint x, uint y): x(x), y(y) {
    for (uint i = 0; i<genome_len; i++) {
        genome.push_back(0);
    }
    hashxy(this);
}

Bot::~Bot() {
    genome.clear();
}

void Bot::genomeInited() {
    mineralsCount = std::count(genome.begin(), genome.end(), commands::minerals_command);
    photosynthesisCount = std::count(genome.begin(), genome.end(), commands::photosynthesis_command);
    eatCount = std::count(genome.begin(), genome.end(), commands::eat_command);
}

uint Bot::getX() { return x; }
uint Bot::getY() { return y; }

void Bot::move(uint *xy) {
    assert(type != KILLED);
    x = xy[0];
    y = xy[1];
    hashxy(this);
}

ulong hashxy(int x,int y) {
    ulong result = x;
    result = result << 16 | y;
    return result;
}

ulong hashxy(Bot *bot) {
    ulong result = bot->getX();
    result = result << 16 | bot->getY();
    bot->hash = result;
    return result;
}

ulong hashxy(int *xy) {
    ulong result = xy[0];
    result = result << 16 | xy[1];
    return result;
}
