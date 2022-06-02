#include "botstruct.h"



Bot::Bot(uint genome_len, QPoint xy): xy(xy) {
    hash = hashxy(this);
    //genome.resize(genome_len);
}

Bot::~Bot() {
    genome.clear();
    if (monitoring)
        emit botKilled();
}

void Bot::genomeStatisticInit() {
    mineralsCount = genome.count(commands::minerals);
    photosynthesisCount = genome.count( commands::photosynthesis);
    eatCount = genome.count(commands::eat);
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
