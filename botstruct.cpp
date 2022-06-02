#include "botstruct.h"



Bot::Bot(uint genome_len, QPoint xy): xy(xy) {
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

void Bot::move(QPoint new_xy) {
    assert(type != DEAD);
    xy = new_xy;
    hash = hashxy(this);
}

ulong hashxy(QPoint xy) {
    ulong result = xy.x();
    result = result << 16 | xy.y();
    return result;
}

ulong hashxy(Bot *bot) {
    ulong result = bot->getX();
    result = result << 16 | bot->getY();
    return result;
}
