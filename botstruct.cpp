#include "botstruct.h"



Bot::Bot(uint genome_len, uint x, uint y): x(x), y(y) {
    hash = hashxy(this);
    genome.resize(genome_len);
}

Bot::~Bot() {
    genome.clear();
    if (monitoring)
        emit botKilled();
}

void Bot::genomeStatisticInit() {
    mineralsCount = std::count(genome.begin(), genome.end(), commands::minerals);
    photosynthesisCount = std::count(genome.begin(), genome.end(), commands::photosynthesis);
    eatCount = std::count(genome.begin(), genome.end(), commands::eat);
}

void Bot::move(uint *xy) {
    assert(type != DEAD);
    x = xy[0];
    y = xy[1];
    hash = hashxy(this);
}

ulong hashxy(int x,int y) {
    ulong result = x;
    result = result << 16 | y;
    return result;
}

ulong hashxy(Bot *bot) {
    ulong result = bot->getX();
    result = result << 16 | bot->getY();
    return result;
}

ulong hashxy(int *xy) {
    ulong result = xy[0];
    result = result << 16 | xy[1];
    return result;
}
